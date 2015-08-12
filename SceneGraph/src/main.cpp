// NOTE: This definition forces GLM to use radians (not degrees) for ALL of its
// angle arguments. The documentation may not always reflect this fact.
// YOU SHOULD USE THIS IN ALL FILES YOU CREATE WHICH INCLUDE GLM
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "glew/glew.h"
#include <GL/glut.h>

#include "Camera.h"
#include "EasyBMP.h"
#include "Intersection.h"

#include <fstream>
#include <iostream>
#include <string>
#include <time.h>
#include <stack>

#include "Geometry.h"
#include "Sphere.h"
#include "Cylinder.h"
#include "Mesh.h"

#include "Node.h"
#include "SceneGraph.h"
#include "ConfigReader.h"

#include <omp.h>

static const float PI = 3.141592653589f;

// Vertex arrays needed for drawing
unsigned int vboPos;
unsigned int vboCol;
unsigned int vboNor;
unsigned int vboIdx;

// Attributes
unsigned int locationPos;
unsigned int locationCol;
unsigned int locationNor;
unsigned int unifLightPos;
unsigned int unifLightCol;
unsigned int unifCamPos;

glm::vec3 lightPos;
glm::vec3 lightCol;

// Uniforms
unsigned int unifModel;
unsigned int unifModelInvTr;
unsigned int unifViewProj;

// Needed to compile and link and use the shaders
unsigned int shaderProgram;

// Window dimensions, change if you want a bigger or smaller window
unsigned int windowWidth = 640;
unsigned int windowHeight = 480;

glm::vec3 EYEP;

// Animation/transformation stuff
clock_t old_time;
float rotation = 0.0f;

// SceneGraph
SceneGraph* scene;
vector<Node*> sceneTraversal;
ConfigReader config;

// Helper function to read shader source and put it in a char array
std::string textFileRead(const char*);

// Some other helper functions from CIS 565 and CIS 277
void printLinkInfoLog(int);
void printShaderInfoLog(int);
void printGLErrorLog();

// Standard glut-based program functions
void init(void);
void resize(int, int);
void display(void);
void keypress(unsigned char, int, int);
void mousepress(int button, int state, int x, int y);
void cleanup(void);

void initShader();
void cleanupShader();

//for drawing the scene:
void uploadGeometry(Geometry* geo);
void drawGeometry(Node* node, glm::mat4 matrix);
void drawNode(Node* node, glm::mat4 matrix);

//for raytracing
void raytrace();
glm::vec3 TraceRay(Ray ray, int depth);
Intersection getClosestI(Node* node, glm::mat4 matrix, Ray ray, Intersection intersection);
bool shadowRay(glm::vec3 point1, glm::vec3 point2);

// for HW3C
bool antialiasing;
bool montecarlo_indirect;
bool montecarlo_direct;
int NUM_SHADOWS;
Node* lightNode;
glm::vec3 getRandomPointOnCube(glm::mat4 T);
glm::vec3 getRandomPointOnSphere(glm::mat4 T);
glm::vec3 getCosineWeightedDirection(const glm::vec3& normal);
glm::vec3 lightDim;
glm::vec3 TraceRayMC(Ray ray, int depth, glm::vec3 transmittance, int num_diffuse);
int NUM_MC;


int main(int argc, char** argv)
{
	// Read in from config file and set up scene graph:
	char* filename = argv[1];
	scene = new SceneGraph();
	config = ConfigReader(filename, scene);

	// scene graph
	Node* r = scene->getRoot();
	r->select();

	// raytracing stuff
	antialiasing = config.get_AA();
	montecarlo_indirect = config.get_MCI();
	montecarlo_direct = config.get_MCD();
	NUM_SHADOWS = config.get_Shadows();
	NUM_MC = config.get_numMC();
	lightNode = new Node(" ");

	windowWidth = config.get_resoW();
	windowHeight = config.get_resoH();
	EYEP = config.get_E();
    lightPos = config.get_LPOS();
    lightCol = config.get_LCOL();
	
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("Becky's Scene Graph");
    glewInit();

    init();
    glutDisplayFunc(display);
    glutReshapeFunc(resize);
    glutKeyboardFunc(keypress);
    glutMouseFunc(mousepress);
    glutIdleFunc(display);

    glutMainLoop();
    return 0;
}

void init()
{
    // Create the VBOs and vboIdx we'll be using to render images in OpenGL
    glGenBuffers(1, &vboPos);
    glGenBuffers(1, &vboCol);
    glGenBuffers(1, &vboNor);
    glGenBuffers(1, &vboIdx);

    // Set the color which clears the screen between frames
    glClearColor(0, 0, 0, 1);
    glEnable(GL_DEPTH_TEST);
    glClearDepth(1.0);
    glDepthFunc(GL_LEQUAL);

    // Set up our shaders here
    initShader();

    resize(windowWidth, windowHeight);
    old_time = clock();
}

void initShader()
{
    // Read in the shader program source files
	std::string vertSourceS = textFileRead("shaders/diff.vert.glsl");
    const char *vertSource = vertSourceS.c_str();
	std::string fragSourceS = textFileRead("shaders/diff.frag.glsl");
    const char *fragSource = fragSourceS.c_str();

    // Tell the GPU to create new shaders and a shader program
    GLuint shadVert = glCreateShader(GL_VERTEX_SHADER);
    GLuint shadFrag = glCreateShader(GL_FRAGMENT_SHADER);
    shaderProgram = glCreateProgram();

    // Load and compiler each shader program
    // Then check to make sure the shaders complied correctly
    // - Vertex shader
    glShaderSource    (shadVert, 1, &vertSource, NULL);
    glCompileShader   (shadVert);
    printShaderInfoLog(shadVert);
    // - Fragment shader
    glShaderSource    (shadFrag, 1, &fragSource, NULL);
    glCompileShader   (shadFrag);
    printShaderInfoLog(shadFrag);

    // Link the shader programs together from compiled bits
    glAttachShader  (shaderProgram, shadVert);
    glAttachShader  (shaderProgram, shadFrag);
    glLinkProgram   (shaderProgram);
    printLinkInfoLog(shaderProgram);

    // Clean up the shaders now that they are linked
    glDetachShader(shaderProgram, shadVert);
    glDetachShader(shaderProgram, shadFrag);
    glDeleteShader(shadVert);
    glDeleteShader(shadFrag);

    // Find out what the GLSL locations are, since we can't pre-define these
    locationPos = glGetAttribLocation (shaderProgram, "vs_Position");
    locationNor = glGetAttribLocation (shaderProgram, "vs_Normal");
    locationCol = glGetAttribLocation (shaderProgram, "vs_Color");

    unifViewProj   = glGetUniformLocation(shaderProgram, "u_ViewProj");
    unifModel      = glGetUniformLocation(shaderProgram, "u_Model");
    unifModelInvTr = glGetUniformLocation(shaderProgram, "u_ModelInvTr");
	unifLightPos   = glGetUniformLocation(shaderProgram, "u_LightPos");
	unifLightCol   = glGetUniformLocation(shaderProgram, "u_LightCol");
	unifCamPos     = glGetUniformLocation(shaderProgram, "u_CamPos");
    printGLErrorLog();
}

void cleanup()
{
    glDeleteBuffers(1, &vboPos);
    glDeleteBuffers(1, &vboCol);
    glDeleteBuffers(1, &vboNor);
    glDeleteBuffers(1, &vboIdx);

    glDeleteProgram(shaderProgram);

    delete scene;
}

void keypress(unsigned char key, int x, int y)
{
    switch (key) {

    case 'q':
        cleanup();
        exit(0);
        break;

	case 'n':
		//select the next node in traversal
		scene->selectNext();
		scene->getSelected()->getGeometry()->changeColor(glm::vec3(1, 1, 1));
		break;

	case 'a':
		//translate node (and children) along negative x-axis by 0.5
		(scene->getSelected())->Translate(-0.5f, 0, 0);
		break;

	case 'd':
		//translate node along positive x-axis by 0.5
		(scene->getSelected())->Translate(0.5f, 0, 0);
		break;

	case 'w':
		//translate node along positive y-axis by 0.5
		(scene->getSelected())->Translate(0, 0.5f, 0);
		break;

	case 's':
		//translate node along negative y-axis by 0.5
		(scene->getSelected())->Translate(0, -0.5f, 0);
		break;

	case 'e':
		//translate node along positive z-axis by 0.5
		(scene->getSelected())->Translate(0, 0, 0.5f);
		break;

	case 'r':
		//translate node along negative z-axis by 0.5
		(scene->getSelected())->Translate(0, 0, -0.5f);
		break;

	case 'x':
    	//decrease scale in the x-axis by 0.5
		(scene->getSelected())->AddScale(-0.5f, 0, 0);
		break;
	case 'X':
		//increase scale in the x-axis by 0.5
		(scene->getSelected())->AddScale(0.5f, 0, 0);
		break;

	case 'y':
		//decrease scale in the y-axis by 0.5
		(scene->getSelected())->AddScale(0, -0.5f, 0);
		break;
	case 'Y':
		//increase scale in the y-axis by 0.5
		(scene->getSelected())->AddScale(0, 0.5f, 0);
		break;

	case 'z':
		//decrease scale in the z-axis by 0.5
		(scene->getSelected())->AddScale(0, 0, -0.5f);
		break;
	case 'Z':
		//increase scale in the z-axis by 0.5
		(scene->getSelected())->AddScale(0, 0, 0.5f);
		break;

	case 'j':
		//decrease rotation in the x-axis by 10 degrees
		(scene->getSelected())->Rotate(-10.f, 0, 0);
		break;
	case 'J':
		//increase scale in the x-axis by 10 degrees
		(scene->getSelected())->Rotate(10.f, 0, 0);
		break;

	case 'k':
		//decrease rotation in the y-axis by 10 degrees
		(scene->getSelected())->Rotate(0, -10.f, 0);
		break;
	case 'K':
		//increase rotation in the y-axis by 10 degrees
		(scene->getSelected())->Rotate(0, 10.f, 0);
		break;

	case 'l':
		//decrease rotation in the z-axis by 10 degrees
		(scene->getSelected())->Rotate(0, 0, -10.f);
		break;
	case 'L':
		//increase scale in the z-axis by 10 degrees
		(scene->getSelected())->Rotate(0, 0, 10.f);
		break;

    case 'f':
        //translate light along negative x-axis by 0.5
        lightPos = glm::vec3(lightPos.x - 0.5, lightPos.y, lightPos.z);
        break;
    case 'F':
        //translate light along positive x-axis by 0.5
        lightPos = glm::vec3(lightPos.x + 0.5, lightPos.y, lightPos.z);
        break;

    case 'g':
        //translate light along negative y-axis by 0.5
        lightPos = glm::vec3(lightPos.x, lightPos.y - 0.5, lightPos.z);
        break;
    case 'G':
        //translate light along positive y-axis by 0.5
        lightPos = glm::vec3(lightPos.x, lightPos.y + 0.5, lightPos.z);
        break;

    case 'h':
        //translate light along negative z-axis by 0.5
        lightPos = glm::vec3(lightPos.x, lightPos.y, lightPos.z - 0.5);
        break;
    case 'H':
        //translate light along positive z-axis by 0.5
        lightPos = glm::vec3(lightPos.x, lightPos.y, lightPos.z + 0.5);
        break;

	case 'p':
		// initiate raytracing and create output image
		raytrace();
		break;

    }

    glutPostRedisplay();
}

void mousepress(int button, int state, int x, int y)
{
    // Put any mouse events here
}

void display()
{
    // Clear the screen so that we only see newly drawn images
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    clock_t newTime = clock();
    old_time = newTime;

    // Create a matrix to pass to the model matrix uniform variable in the
    // vertex shader, which is used to transform the vertices in our draw call.
    glm::mat4 modelmat = glm::mat4();

    // Make sure you're using the right program for rendering
    glUseProgram(shaderProgram);

	//TRAVERSE THE SCENE
	drawNode(scene->getRoot(), modelmat);

    // Move the rendering we just made onto the screen
    glutSwapBuffers();

	// write selected node's name to the screen
	string name = scene->getSelected()->getNodeName();
	glColor3f(1,1,1);
	glRasterPos2f(320, 400);
	int len = name.length();
	for (int i = 0; i < len; i++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, name[i]);
	}

    // Check for any GL errors that have happened recently
    printGLErrorLog();
}

// Recursive function for drawing nodes of scene graph
void drawNode(Node* node, glm::mat4 model) {

	// multiply by previous (or identity)
	model = model * node->getTransform();

	// Set light node for raytracing
	if (node->getMat().getMatEmit())
		lightNode = node;

	// if the node has geometry, upload and draw it:
	if (node->getGeometry()) {
		uploadGeometry(node->getGeometry());
		drawGeometry(node, model);
	}

	// recurse through children, if there are any
	vector<Node*> children = node->getChildNodes();
	for (unsigned int i = 0; i < children.size(); ++i) {
		drawNode(children.at(i), model);
	}
}

void uploadGeometry(Geometry* geo)
{
	// Sizes of the various array elements below.
    static const GLsizei SIZE_VEC = sizeof(glm::vec3);
    static const GLsizei SIZE_TRI = 3 * sizeof(unsigned int);

	// Bind+upload vertex data
	glBindBuffer(GL_ARRAY_BUFFER, vboPos);
	glBufferData(GL_ARRAY_BUFFER, geo->getVertexCount() * SIZE_VEC, &(geo->getVertices()[0]), GL_STATIC_DRAW);

	// Bind+upload the color data
	glBindBuffer(GL_ARRAY_BUFFER, vboCol);
	glBufferData(GL_ARRAY_BUFFER, (geo->getColors()).size() * SIZE_VEC, &(geo->getColors()[0]), GL_STATIC_DRAW);

	// Bind+upload the normals
	glBindBuffer(GL_ARRAY_BUFFER, vboNor);
	glBufferData(GL_ARRAY_BUFFER, (geo->getNormals()).size() * SIZE_VEC, &(geo->getNormals()[0]), GL_STATIC_DRAW);

	// Bind+upload the indices to the GL_ELEMENT_ARRAY_BUFFER.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIdx);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, ((geo->getIndices()).size() / 3) * SIZE_TRI, &(geo->getIndices()[0]), GL_STATIC_DRAW);
}

void drawGeometry(Node* node, glm::mat4 matrix)
{
	// Tell the GPU which shader program to use to draw things
    glUseProgram(shaderProgram);

    // Send shader our light position and color
	glUniform3fv(unifLightPos, 1, &lightPos[0]);
	glUniform3fv(unifLightCol, 1, &lightCol[0]);
	glUniform3fv(unifCamPos, 1, &EYEP[0]);

	// Activate our three kinds of vertex information
    glEnableVertexAttribArray(locationPos);
    glEnableVertexAttribArray(locationCol);
    glEnableVertexAttribArray(locationNor);

	// Set the 4x4 model transformation matrices
    // Pointer to the first element of the array
    glUniformMatrix4fv(unifModel, 1, GL_FALSE, &matrix[0][0]);
    // Also upload the inverse transpose for normal transformation
    const glm::mat4 modelInvTranspose = glm::inverse(glm::transpose(matrix));
    glUniformMatrix4fv(unifModelInvTr, 1, GL_FALSE, &modelInvTranspose[0][0]);

    // Tell the GPU where the positions are: in the position buffer
    glBindBuffer(GL_ARRAY_BUFFER, vboPos);
    glVertexAttribPointer(locationPos, 3, GL_FLOAT, false, 0, NULL);

    // Tell the GPU where the colors are: in the color buffer
    glBindBuffer(GL_ARRAY_BUFFER, vboCol);
    glVertexAttribPointer(locationCol, 3, GL_FLOAT, false, 0, NULL);

    // Tell the GPU where the normals are: in the normal buffer
    glBindBuffer(GL_ARRAY_BUFFER, vboNor);
    glVertexAttribPointer(locationNor, 3, GL_FLOAT, false, 0, NULL);

    // Tell the GPU where the indices are: in the index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIdx);

    // Draw the elements
	const int NUM = node->getGeometry()->getIndexCount();
	glDrawElements(GL_TRIANGLES, NUM, GL_UNSIGNED_INT, 0);

    // Shut off the information since we're done drawing.
    glDisableVertexAttribArray(locationPos);
    glDisableVertexAttribArray(locationCol);
    glDisableVertexAttribArray(locationNor);

    // Check for OpenGL errors
    printGLErrorLog();
}

// initiate raytracing using config info and current state of the scene graph
// outputs the raytraced image as a BMP file
void raytrace() {

	// NUMBER OF RAY TRACES HERE
	int rt = 5;

	// Start timer
	time_t startT;
	time(&startT);
	double seconds;

	// Set up BMP
	BMP output;
	output.SetSize(windowWidth, windowHeight);
	output.SetBitDepth(24);

	//Set up camera
	Camera cam = Camera(EYEP, config.get_C(), config.get_U(),
						config.get_FOVY() / 2, windowWidth, windowHeight);

	glm::vec3 D;	//ray direction vector
	glm::vec3 Pw;	//eye world point vector

	// Begin Raytracing
	for (unsigned int y = 0; y < windowHeight; ++y) {

		//progress indicator
		int barWidth = 60;
		cout << "[";
		int pos = barWidth * y/windowHeight;
		for (int i = 0; i < barWidth; ++i) {
			if (i < pos || (i == barWidth-1 && i == pos)) cout << "=";
			else if (i == pos) cout << ">";
			else cout << " ";
		}
		int p = (100 * y / windowHeight) + 1;
		cout << "] " << p << "% \r";
		cout.flush();


		#pragma omp parallel for
		for (int x = 0; x < windowWidth; ++x) {

			glm::vec3 trans = glm::vec3(1.f);	// initial transmittance for rays


			/*if (antialiasing) {

				std::vector<glm::vec3> colorFrag;
				glm::vec3 color = glm::vec3(0.f);

				for (float fragX = x; fragX < x + 1.0f; fragX += 0.5f)
				for (float fragY = y; fragY < y + 1.0f; fragY += 0.5f) {

					Pw = cam.GetWorldPointVec(fragX, fragY);
					D = glm::normalize(Pw - EYEP);

					// normalized ray in world-space
					Ray ray = Ray(EYEP, D);

					//direct illumination
					glm::vec3 c = glm::vec3(0.f);

					// Monte Carlo
					if (montecarlo) {

						//indirect illumination
						glm::vec3 mc_c;
						for (unsigned int i = 0; i < NUM_MC; ++i)
							mc_c += TraceRayMC(ray, rt, trans, 0);
						mc_c /= (float) NUM_MC;

						c += mc_c;
						c /= 2.f;
					} 

					c = glm::clamp(c, 0.f, 1.f);
					c *= 255.0f;
					colorFrag.push_back(c);
				}
				

				for (unsigned int i = 0; i < colorFrag.size(); i++)
					color += colorFrag.at(i);
				color /= colorFrag.size();
				glm::clamp(color, 0.f, 255.f);

				output(x, y)->Red   = color.r;
				output(x, y)->Green = color.g;
				output(x, y)->Blue  = color.b;

			} else {*/


				Pw = cam.GetWorldPointVec(x, y);
				D = glm::normalize(Pw - EYEP);

				// normalized ray in world-space
				Ray ray = Ray(EYEP, D);
				glm::vec3 color = glm::vec3(0.f);
				glm::vec3 color_indirect = glm::vec3(0.f);
				glm::vec3 color_direct = glm::vec3(0.f);


				// indirect illumination
				if (montecarlo_indirect) {

					for (unsigned int i = 0; i < NUM_MC; ++i)
						color_indirect += TraceRayMC(ray, rt, trans, 0);
					color_indirect /= (float) NUM_MC;
					color = color_indirect;

				//direct illumination
				} else if (montecarlo_direct && !montecarlo_indirect) {
					color_direct = TraceRay(ray, rt);
					color = color_direct;
				}

				

				color = glm::clamp(color, 0.f, 1.f);
				color *= 255.0f;

				output(x, y)->Red   = color.r;
				output(x, y)->Green = color.g;
				output(x, y)->Blue  = color.b;
			//}
		}
	}

	output.WriteToFile("output.bmp");
	cout << "\noutput.bmp has been written." << endl;

	// End timer
	time_t endT;
	time(&endT);
	seconds = difftime(endT, startT);
	int minutes = seconds / 60;
	seconds = (int) seconds % 60;
	cout << "Time to Render: " << minutes << "min, " << seconds << "sec" << endl;
}

// Recursive function for finding intersections with all nodes given a ray
Intersection getClosestI(Node* node, glm::mat4 matrix, Ray ray, Intersection intersection) {

	// multiply by previous (or identity)
	matrix = matrix * node->getTransform();

	// if the node has geometry, find its intersection:
	if (node->getGeometry()) {
		Intersection i = node->getGeometry()->intersect(matrix, ray);
		if (i.t > 0) {
			intersection = i;
			intersection.m = node->getMat();
		}
	}

	// recurse through children, if there are any
	vector<Node*> children = node->getChildNodes();
	for (unsigned int j = 0; j < children.size(); ++j) {
		Intersection i = getClosestI(children.at(j), matrix, ray, intersection);
		if ((i.t < intersection.t && i.t > 0) || (intersection.t == -1.0 && i.t > 0)){
			intersection = i;
		}
	}

	return intersection;
}

// MONTE CARLO RAYTRACING - INDIRECT + DIRECT ILLUMINATION
glm::vec3 TraceRayMC(Ray ray, int depth, glm::vec3 transmittance, int num_diffuse) {

	if (depth < 0) return transmittance;

	// Components of global illumination formula
	glm::vec3 lightColor = config.get_LCOL();
	glm::vec3 lightPoint = config.get_LPOS();
	float ambientCoef = 0.2f;
	glm::vec3 diffuseTerm;
	glm::vec3 specularTerm;
	glm::vec3 ambientTerm;

	// find random light point
	glm::mat4 lightT = lightNode->getTransform();
	if (lightNode->getGeometry()->getGeometryType() == 0)
		lightPoint = getRandomPointOnCube(lightT);
	else if (lightNode->getGeometry()->getGeometryType() == 1)
		lightPoint = getRandomPointOnSphere(lightT);

	// intersect ray with all objects and find intersection point (if any) that is closest to start of ray
	Intersection isxPoint;
	isxPoint.t = -1.0f;
	isxPoint.normal = glm::vec3(0.f);
	isxPoint.point = glm::vec3(0.f);
	isxPoint = getClosestI(scene->getRoot(), glm::mat4(), ray, isxPoint);

	// if no intersection...
	if (isxPoint.t == -1.0f) {

		// if ray is nearly parallel to light direction
		float p = glm::dot(ray.orig - ray.dir, ray.orig - lightPoint);
		if (p < 1.001 && p > 0.009) return lightColor;
		
		return glm::vec3(0.f);

	// if we're a light
	} else if (isxPoint.m.getMatEmit() > 0) {
		return (transmittance * lightColor * (float) isxPoint.m.getMatEmit());

	// if there is an intersection...
	} else {

		// normal and material at intersection point
		glm::vec3 N = isxPoint.normal;
		Material isxMat = isxPoint.m;

		// ambient
		ambientTerm = isxMat.getMatDiff() * ambientCoef;

		//* * * REFLECTED * * *//
		if (isxMat.getMatMirr()) {

			// calculate reflected ray
			glm::vec3 point = isxPoint.point + N * 0.0001f;
			glm::vec3 I = ray.dir;
			glm::vec3 R = I - 2 * glm::dot(N, I) * N;
			Ray reflRay = Ray(point, R);
			glm::vec3 reflTransmittance = transmittance * isxMat.getMatRefl();

			return TraceRayMC(reflRay, depth-1, reflTransmittance, num_diffuse);
		}

		//* * * REFRACTIVE * * *//
		if (isxMat.getMatTran()) {

			// no diffuse or ambient terms
			diffuseTerm = glm::vec3(0.f);
			ambientTerm = glm::vec3(0.f);

			// some variables
			glm::vec3 point;
			float iorAir = 1.0f;
			float iorMat = isxMat.getMatIor();
			float n;
			glm::vec3 I = ray.dir;
			glm::vec3 T;

			// if we have/haven't entered the object yet
			if (glm::dot(N, I) > 0) {
				point = isxPoint.point + N * 0.001f;
				N = -N;
				n = iorMat/iorAir;
			} else {
				point = isxPoint.point - N * 0.001f;
				n = iorAir/iorMat;
			}

			// calculate refracted ray
			float theta = 1 - glm::pow(n, 2.f) * (1 - glm::pow(glm::dot(N, I), 2.f));
			if (theta < 0) return glm::vec3(0.f);
			theta = glm::pow(theta, 0.5f);
			T = (-n * glm::dot(N, I) - theta) * N + n * I;
			T = glm::normalize(T);
			Ray refrRay = Ray(point, T);
			glm::vec3 refrTransmittance = transmittance * glm::vec3(1.f); 

			return TraceRayMC(refrRay, depth-1, refrTransmittance, num_diffuse);
		}


		//* * * LAMBERT * * *//
		
		glm::vec3 direct_color = glm::vec3(0.f);
		glm::vec3 indirect_color = glm::vec3(0.f);

		// intersecting with diffuse surface for the first time, compute direct illumination
		if (num_diffuse < 1 && montecarlo_direct) {

			std::vector<glm::vec3> d_colors;
			
			// shadow rays
			for (int i = 0; i < NUM_SHADOWS; i++) {

				// find random point on area light
				if (lightNode->getGeometry()->getGeometryType() == 0)
					lightPoint = getRandomPointOnCube(lightT);
				else if (lightNode->getGeometry()->getGeometryType() == 1)
					lightPoint = getRandomPointOnSphere(lightT);
			
				// if there's no shadow, update diffuse term
				if (!shadowRay(isxPoint.point + N * 0.0001f, lightPoint)) {
				
					// * * SPECULAR * * //
					if (isxMat.getMatExpo() > 0) {

						float n = isxMat.getMatExpo();
						glm::vec3 E = glm::normalize(ray.orig - isxPoint.point);
						glm::vec3 L = glm::normalize(lightPoint - isxPoint.point);
						glm::vec3 R = 2 * glm::dot(L, N) * N - L;

						float spec = glm::clamp(glm::dot(R, E), 0.f, 1.f);
						spec = glm::pow(spec, n);

						specularTerm = spec * lightColor;
					}

					//* * * DIFFUSE * * * //
					float diffuseCoef = glm::clamp(glm::dot(glm::normalize(N), glm::normalize(lightPoint - isxPoint.point)), 0.f, 1.f);
					diffuseTerm = isxMat.getMatDiff() * diffuseCoef;

				// if there is a shadow, diffuse and specular are 0
				} else {
					diffuseTerm = glm::vec3(0.f);
					specularTerm = glm::vec3(0.f);
				}

				// put all diffuse and specular terms in the vector
				d_colors.push_back(diffuseTerm + specularTerm);
			}

			// average all diffuse and specular terms found
			for (unsigned int i = 0; i < d_colors.size(); i++)
				direct_color += d_colors.at(i);
			direct_color /= d_colors.size();

			// add ambient term
			direct_color += ambientTerm;
		}

		// then compute indirect illumination
		glm::vec3 diffCol = isxMat.getMatDiff();
		float absorbance = 1.f - glm::max(glm::max(diffCol.r, diffCol.g), diffCol.b);
		float random = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			
		// if absorbed, return black
		if (random > 1.f - absorbance) {
			indirect_color = glm::vec3(0.f);
			return direct_color;

		// if not absorbed, ray bounces in a cosine-weighted random direction
		} else {
			glm::vec3 newRayDirection = getCosineWeightedDirection(glm::normalize(N));
			Ray lambRay = Ray(isxPoint.point, newRayDirection);
			glm::vec3 lambTransmittance = (transmittance * diffCol) / (1 - absorbance);
			
			indirect_color = TraceRayMC(lambRay, depth-1, lambTransmittance, num_diffuse+1);
		}

		// average the two - NEED TO USE SOLID ANGLES HERE
		if (direct_color == glm::vec3(0.f)) return indirect_color;
		return (indirect_color + direct_color) / 2.f;
	}
}


// RAY-TRACING - DIRECT ILLUMINATION
glm::vec3 TraceRay(Ray ray, int depth) {

	glm::vec3 currColor;

	if (depth < 0) return glm::vec3(0.f);

	// Components of global illumination formula
	glm::vec3 lightColor = config.get_LCOL();
	float ambientCoef = 0.2f;
	glm::vec3 diffuseTerm;
	glm::vec3 specularTerm;
	glm::vec3 ambientTerm;

	// find random light point
	glm::vec3 lightPoint;
	glm::mat4 lightT = lightNode->getTransform();
	if (lightNode->getGeometry()->getGeometryType() == 0)
		lightPoint = getRandomPointOnCube(lightT);
	else if (lightNode->getGeometry()->getGeometryType() == 1)
		lightPoint = getRandomPointOnSphere(lightT);

	// intersect ray with all objects and find intersection point (if any) that is closest to start of ray
	Intersection isxPoint;
	isxPoint.t = -1.0f;
	isxPoint.normal = glm::vec3(0.f);
	isxPoint.point = glm::vec3(0.f);

	isxPoint = getClosestI(scene->getRoot(), glm::mat4(), ray, isxPoint);

	// if no intersection...
	if (isxPoint.t == -1.0f) {

		// if ray is nearly parallel to light direction
		float p = glm::dot(ray.orig - ray.dir, ray.orig - lightPoint);
		if (p < 1.001 && p > 0.009) {
			currColor = lightColor;
		} else {
			currColor = glm::vec3(0.f);
		}

		return currColor;

	// if we're a light
	} else if (isxPoint.m.getMatEmit() > 0) {
		return lightCol;

	// if there is an intersection...
	} else {

		// normal and material at intersection point
		glm::vec3 N = isxPoint.normal;
		Material isxMat = isxPoint.m;

		// ambient
		ambientTerm = isxMat.getMatDiff() * ambientCoef;

		//* * * REFLECTED * * *//
		if (isxMat.getMatMirr()) {

			// calculate reflected ray
			glm::vec3 point = isxPoint.point + N * 0.0001f;
			glm::vec3 I = ray.dir;
			glm::vec3 R = I - 2 * glm::dot(N, I) * N;
			Ray reflRay = Ray(point, R);

			return TraceRay(reflRay, depth-1) * isxMat.getMatRefl();
		}

		//* * * REFRACTIVE * * *//
		if (isxMat.getMatTran()) {

			// no diffuse or ambient terms
			diffuseTerm = glm::vec3(0.f);
			ambientTerm = glm::vec3(0.f);

			// some variables
			glm::vec3 point;
			float iorAir = 1.0f;
			float iorMat = isxMat.getMatIor();
			float n;
			glm::vec3 I = ray.dir;
			glm::vec3 T;

			// if we have/haven't entered the object yet
			if (glm::dot(N, I) > 0) {
				point = isxPoint.point + N * 0.001f;
				N = -N;
				n = iorMat/iorAir;
			} else {
				point = isxPoint.point - N * 0.001f;
				n = iorAir/iorMat;
			}

			// calculate refracted ray
			float theta = 1 - glm::pow(n, 2.f) * (1 - glm::pow(glm::dot(N, I), 2.f));
			if (theta < 0) return glm::vec3(0.f);
			theta = glm::pow(theta, 0.5f);
			T = (-n * glm::dot(N, I) - theta) * N + n * I;
			T = glm::normalize(T);
			Ray refrRay = Ray(point, T);

			return TraceRay(refrRay, depth-1);
		}


		// Shadow Rays
		std::vector<glm::vec3> colors;
		for (int i = 0; i < NUM_SHADOWS; i++) {

			// find random point on area light
			if (lightNode->getGeometry()->getGeometryType() == 0)
				lightPoint = getRandomPointOnCube(lightT);
			else if (lightNode->getGeometry()->getGeometryType() == 1)
				lightPoint = getRandomPointOnSphere(lightT);
			
			// if there's no shadow, update diffuse term
			if (!shadowRay(isxPoint.point + N * 0.0001f, lightPoint)) {
				
				// * * SPECULAR * * //
				if (isxMat.getMatExpo() > 0) {

					float n = isxMat.getMatExpo();
		
					glm::vec3 E = glm::normalize(ray.orig - isxPoint.point);
					glm::vec3 L = glm::normalize(lightPoint - isxPoint.point);
					glm::vec3 R = 2 * glm::dot(L, N) * N - L;

					float spec = glm::clamp(glm::dot(R, E), 0.f, 1.f);
					spec = glm::pow(spec, n);

					specularTerm = spec * lightColor;
				}

				//* * * DIFFUSE * * *//
				float diffuseCoef = glm::clamp(glm::dot(glm::normalize(N), glm::normalize(lightPoint - isxPoint.point)), 0.f, 1.f);
				diffuseTerm = isxMat.getMatDiff() * diffuseCoef;

			// if there is a shadow, diffuse and specular are 0
			} else {
				diffuseTerm = glm::vec3(0.f);
				specularTerm = glm::vec3(0.f);
			}

			// put all diffuse and specular terms in the vector
			colors.push_back(diffuseTerm + specularTerm);
		}

		// average all diffuse and specular terms found
		glm::vec3 finalColor = glm::vec3(0.f);
		for (unsigned int i = 0; i < colors.size(); i++)
			finalColor += colors.at(i);
		finalColor /= colors.size();

		// add ambient term
		finalColor += ambientTerm;
		//finalColor = glm::clamp(finalColor, 0.f, 255.f);

		return finalColor;
	}
}



// Given transformation matrix T of light node, just like intersection, and assume radius etc.
// Returns a random point on a cube
glm::vec3 getRandomPointOnCube(glm::mat4 T) {
	
	lightDim = config.get_LightDim();
	
	// Get surface area of the cube
	float side1 = lightDim.x * lightDim.y;
	float side2 = lightDim.y * lightDim.z;
	float side3 = lightDim.z * lightDim.x;
	float totalArea = 2.f * (side1 + side2 + side3);

	// pick random face weighted by surface area
	float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	// pick two random components for the point in the range (-0.5, 0.5)
	float c1 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX) - 0.5f;
	float c2 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX) - 0.5f;

	glm::vec3 point;
	if (r < side1 / totalArea) {
		point = glm::vec3(c1, c2, 0.5f);	// x-y front
	} else if (r < (side1 * 2) / totalArea) {
		point = glm::vec3(c1, c2, -0.5f);	// x-y back
	} else if (r < (side1 * 2 + side2) / totalArea) {
		point = glm::vec3(0.5f, c1, c2);	// y-z front
	} else if (r < (side1 * 2 + side2 * 2) / totalArea) {
		point = glm::vec3(-0.5f, c1, c2);	// y-z back
	} else if (r < (side1 * 2 + side2 * 2 + side3) / totalArea) {
		point = glm::vec3(c1, 0.5f, c2);	// x-z front
	} else if (r < (side1 * 2 + side2 * 2 + side3) / totalArea) {
		point = glm::vec3(c1, -0.5f, c2);	// x-z back
	}

	// transform point to world space
	point = glm::vec3(T * glm::vec4(point, 1.0));

	return point;
}

// Returns a random point on a sphere
glm::vec3 getRandomPointOnSphere(glm::mat4 T) {

	// generate u, v in the range (0, 1)
	float u = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	float v = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

	float theta = 2.f * PI * u;
	float phi = acos(2.f * v - 1.f);

	// find x, y, z coordinates assuming unit sphere in object space
	glm::vec3 point;
	point.x = sin(phi) * cos(theta);
	point.y = sin(phi) * sin(theta);
	point.z = cos(phi);

	// transform point to world space
	point = glm::vec3(T * glm::vec4(point, 1.0));

	return point;
}

// Given a normal vector, find a cosine-weighted random direction in a hemisphere
glm::vec3 getCosineWeightedDirection(const glm::vec3& normal) {

	// pick two random numbers in the range (0, 1)
	float xi1 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	float xi2 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

	float up = sqrt(xi1);			// cos(theta)
	float over = sqrt(1 - up * up);	// sin(theta)
	float around = xi2 * 2.f * PI;

	// find a direction that is not the normal based off of whether or not the normal's components
	// are all equal to sqrt(1/3) or whether or not at least one component is less than sqrt(1/3).
	const float SQRT_ONE_THIRD = sqrt(1.f/3.f);
	glm::vec3 directionNotNormal;
	if (abs(normal.x) < SQRT_ONE_THIRD) {
		directionNotNormal = glm::vec3(1.f, 0.f, 0.f);
	} else if (abs(normal.y) < SQRT_ONE_THIRD) {
		directionNotNormal = glm::vec3(0.f, 1.f, 0.f);
	} else {
		directionNotNormal = glm::vec3(0.f, 0.f, 1.f);
	}

	// use not-normal direction to generate two perpendicular directions
	glm::vec3 perpDirection1 = glm::normalize(glm::cross(normal, directionNotNormal));
	glm::vec3 perpDirection2 = glm::normalize(glm::cross(normal, perpDirection1));

	return (up * normal) + (cos(around) * over * perpDirection1) + (sin(around) * over * perpDirection2);
}


// true = shadow, false = no shaddow
bool shadowRay(glm::vec3 isxpoint, glm::vec3 lightpoint) {

	// shoot a ray from isxpoint towards light
	Ray ray = Ray(isxpoint, glm::normalize(lightpoint - isxpoint));

	// find nearest intersection point in direction towards light
	Intersection i;
	i.t = -1.0f;
	i.normal = glm::vec3(0.f);
	i.point = glm::vec3(0.f);

	i = getClosestI(scene->getRoot(), glm::mat4(), ray, i);

	if (i.t == -1.0f) return false;
	if (i.m.getMatEmit()) return false;

	// now find distance from light
	float d_light = glm::distance(ray.orig, lightpoint);
	float d_isx   = glm::distance(ray.orig, ray.orig + ray.dir * i.t);

	// if intersection is closer than light, then we have a shadow
	return (d_isx < d_light);
}


void resize(int width, int height)
{
    // Set viewport
    glViewport(0, 0, width, height);

    // Get camera information
	float fovy = config.get_FOVY() * PI / 180;
    glm::mat4 projection = glm::perspective(fovy, width / (float) height, 0.1f, 100.0f);
	glm::mat4 camera = glm::lookAt(config.get_E(), config.get_C() + config.get_E(), config.get_U());
    projection = projection * camera;

    // Upload the projection matrix, which changes only when the screen or camera changes
    glUseProgram(shaderProgram);
    glUniformMatrix4fv(unifViewProj, 1, GL_FALSE, &projection[0][0]);

    glutPostRedisplay();
}

std::string textFileRead(const char *filename)
{
    // http://insanecoding.blogspot.com/2011/11/how-to-read-in-file-in-c.html
    std::ifstream in(filename, std::ios::in);
    if (!in) {
        std::cerr << "Error reading file" << std::endl;
        throw (errno);
    }
    return std::string(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>());
}

void printGLErrorLog()
{
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cerr << "OpenGL error " << error << ": ";
        const char *e =
            error == GL_INVALID_OPERATION             ? "GL_INVALID_OPERATION" :
            error == GL_INVALID_ENUM                  ? "GL_INVALID_ENUM" :
            error == GL_INVALID_VALUE                 ? "GL_INVALID_VALUE" :
            error == GL_INVALID_INDEX                 ? "GL_INVALID_INDEX" :
            "unknown";
        std::cerr << e << std::endl;

        // Throwing here allows us to use the debugger stack trace to track
        // down the error.
#ifndef __APPLE__
        // But don't do this on OS X. It might cause a premature crash.
        // http://lists.apple.com/archives/mac-opengl/2012/Jul/msg00038.html
        throw;
#endif
    }
}

void printLinkInfoLog(int prog)
{
    GLint linked;
    glGetProgramiv(prog, GL_LINK_STATUS, &linked);
    if (linked == GL_TRUE) {
        return;
    }
    std::cerr << "GLSL LINK ERROR" << std::endl;

    int infoLogLen = 0;
    int charsWritten = 0;
    GLchar *infoLog;

    glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &infoLogLen);

    if (infoLogLen > 0) {
        infoLog = new GLchar[infoLogLen];
        // error check for fail to allocate memory omitted
        glGetProgramInfoLog(prog, infoLogLen, &charsWritten, infoLog);
        std::cerr << "InfoLog:" << std::endl << infoLog << std::endl;
        delete[] infoLog;
    }
    // Throwing here allows us to use the debugger to track down the error.
    throw;
}

void printShaderInfoLog(int shader)
{
    GLint compiled;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (compiled == GL_TRUE) {
        return;
    }
    std::cerr << "GLSL COMPILE ERROR" << std::endl;

    int infoLogLen = 0;
    int charsWritten = 0;
    GLchar *infoLog;

    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLen);

    if (infoLogLen > 0) {
        infoLog = new GLchar[infoLogLen];
        // error check for fail to allocate memory omitted
        glGetShaderInfoLog(shader, infoLogLen, &charsWritten, infoLog);
        std::cerr << "InfoLog:" << std::endl << infoLog << std::endl;
        delete[] infoLog;
    }
    // Throwing here allows us to use the debugger to track down the error.
    throw;
}