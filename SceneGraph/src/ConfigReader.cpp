
#define _CRT_SECURE_NO_WARNINGS

#include "ConfigReader.h"
#include "Geometry.h"
#include "Cube.h"
#include "Sphere.h"
#include "Cylinder.h"
#include "Mesh.h"
#include "objLoader.h"

#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

// Constructors:
ConfigReader::ConfigReader() {
}

// Constructor given filename and scene graph:
ConfigReader::ConfigReader(char* file, SceneGraph* scene) {
		
	ifstream configFile(file);

	if (!configFile.is_open()) {
		cout << "Could not open config file." << endl;
	}

	else {

		while (!configFile.eof()) {

			char line[100];
			char * param[4];

			configFile.getline(line, 100);
			param[0] = strtok(line, " ");

			if (!param[0]) continue;

			//CAMERA Variables:
			if (strcmp(param[0], "CAMERA") == 0) {
				configFile.getline(line, 100);
				param[0] = strtok(line, " ");
			
				while(param[0]) {

					//reso_w & reso_h - ints, screen size/resolution
					if (strcmp(param[0], "RESO") == 0) {
						param[1] = strtok(0, " ");
						param[2] = strtok(0, " ");
						reso_w = stoi(param[1], NULL);
						reso_h = stoi(param[2], NULL);

					//eyep - vector of floats, eye/camera world coordinates
					} else if (strcmp(param[0], "EYEP") == 0) {
						param[1] = strtok(0, " ");
						param[2] = strtok(0, " ");
						param[3] = strtok(0, " ");
						float eyep_x = stof(param[1], NULL);
						float eyep_y = stof(param[2], NULL);
						float eyep_z = stof(param[3], NULL);
						eyep = glm::vec3(eyep_x, eyep_y, eyep_z);
				
					//vdir - vector of floats, viewing vector direction
					} else if (strcmp(param[0], "VDIR") == 0) {
						param[1] = strtok(0, " ");
						param[2] = strtok(0, " ");
						param[3] = strtok(0, " ");
						float vdir_x = stof(param[1], NULL);
						float vdir_y = stof(param[2], NULL);
						float vdir_z = stof(param[3], NULL);
						vdir = glm::vec3 (vdir_x, vdir_y, vdir_z);

					//uvec - vector of floats, up vector
					} else if (strcmp(param[0], "UVEC") == 0) {
						param[1] = strtok(0, " ");
						param[2] = strtok(0, " ");
						param[3] = strtok(0, " ");
						float uvec_x = stof(param[1], NULL);
						float uvec_y = stof(param[2], NULL);
						float uvec_z = stof(param[3], NULL); 
						uvec = glm::vec3 (uvec_x, uvec_y, uvec_z);
				
					//fovy - float, half-angle field of view in y-direction
					} else if (strcmp(param[0], "FOVY") == 0) {
						param[1] = strtok(0, " ");
						fovy = stof(param[1], NULL);
					}

					configFile.getline(line, 100);
					param[0] = strtok(line, " ");
				}
			}

			// RAYTRACING Variables
			else if (strcmp(param[0], "RAYTRACING") == 0) {
				configFile.getline(line, 100);
				param[0] = strtok(line, " ");
			
				while(param[0]) {

					// antialiasing on = 1, off = 0
					if (strcmp(param[0], "ANTIALIAS") == 0) {
						param[1] = strtok(0, " ");
						antialiasing = (stoi(param[1], NULL) > 0);

					// monte carlo direct illumination on = 1, off = 0
					} else if (strcmp(param[0], "MC_DIRECT") == 0) {
						param[1] = strtok(0, " ");
						mc_direct = (stoi(param[1], NULL) > 0);

					// monte carlo indirect illumination on = 1, off = 0
					} else if (strcmp(param[0], "MC_INDIRECT") == 0) {
						param[1] = strtok(0, " ");
						mc_indirect = (stoi(param[1], NULL) > 0);

					// number of shadow rays to sample for each pixel
					} else if (strcmp(param[0], "NUM_SHADOWS") == 0) {
						param[1] = strtok(0, " ");
						num_shadows = stoi(param[1], NULL);

					// number of monte carlo iterations
					} else if (strcmp(param[0], "NUM_MC") == 0) {
						param[1] = strtok(0, " ");
						num_mc = stoi(param[1], NULL);
					}

					configFile.getline(line, 100);
					param[0] = strtok(line, " ");
				}
			}


			//MAT Variables:
			else if (strcmp(param[0], "MAT") == 0) {
				param[1] = strtok(0, " ");

				string mat = param[1];	// name of material
				glm::vec3 diff;			// diffuse color
				glm::vec3 refl;			// reflective/specular color
				float expo;				// specular exponent
				float ior;				// index of refraction
				bool mirr;				// 1 = reflectance, 0 = specular
				bool tran;				// 1 = transparent, 0 = opaque
				int emittance;			// 0 = doesn't emit light, > 1 = emittance value

				configFile.getline(line, 100);
				param[0] = strtok(line, " ");

				while (param[0]) {

					if (strcmp(param[0], "DIFF") == 0) {
						param[1] = strtok(0, " ");
						param[2] = strtok(0, " ");
						param[3] = strtok(0, " ");
						float d_r = stof(param[1], NULL);
						float d_g = stof(param[2], NULL);
						float d_b = stof(param[3], NULL);
						diff = glm::vec3(d_r, d_g, d_b);

					} else if (strcmp(param[0], "REFL") == 0) {
						param[1] = strtok(0, " ");
						param[2] = strtok(0, " ");
						param[3] = strtok(0, " ");
						float r_r = stof(param[1], NULL);
						float r_g = stof(param[2], NULL);
						float r_b = stof(param[3], NULL);
						refl = glm::vec3(r_r, r_g, r_b);
					
					} else if (strcmp(param[0], "EXPO") == 0) {
						param[1] = strtok(0, " ");
						expo = stof(param[1], NULL);

					} else if (strcmp(param[0], "IOR") == 0) {
						param[1] = strtok(0, " ");
						ior = stof(param[1], NULL);

					} else if (strcmp(param[0], "MIRR") == 0) {
						param[1] = strtok(0, " ");
						mirr = (stoi(param[1], NULL) > 0);

					} else if (strcmp(param[0], "TRAN") == 0) {
						param[1] = strtok(0, " ");
						tran = (stoi(param[1], NULL) > 0);
					
					} else if (strcmp(param[0], "EMIT") == 0) {
						param[1] = strtok(0, " ");
						emittance = stoi(param[1], NULL);
					}

					configFile.getline(line, 100);
					param[0] = strtok(line, " ");
				}

				Material m = Material(mat, diff, refl, expo, ior, mirr, tran, emittance);
				all_materials.push_back(m);
			}

			//NODE Variables:
			else if (strcmp(param[0], "NODE") == 0) {
				param[1] = strtok(0, " ");
				node_name = param[1];
				Node* node = new Node(node_name);

				configFile.getline(line, 100);
				param[0] = strtok(line, " ");

				glm::vec3 t;	//translation vector
				float rv_x;		//rotation value in x-direction
				float rv_y;		//rotation value in y-direction
				float rv_z;		//rotation value in z-direction
				glm::vec3 s;	//scale vector
				glm::vec3 c;	//center vector
				
				while (param[0]) {

					if (strcmp(param[0], "TRANSLATION") == 0) {
						param[1] = strtok(0, " ");
						param[2] = strtok(0, " ");			// 1 0 0 Tx
						param[3] = strtok(0, " ");			// 0 1 0 Ty
						float t_x = stof(param[1], NULL);	// 0 0 1 Tz
						float t_y = stof(param[2], NULL);	// 0 0 0 1
						float t_z = stof(param[3], NULL);
						t = glm::vec3(t_x, t_y, t_z);

					} else if (strcmp(param[0], "ROTATION") == 0) {
						param[1] = strtok(0, " ");
						param[2] = strtok(0, " ");
						param[3] = strtok(0, " ");
						rv_x = stof(param[1], NULL);
						rv_y = stof(param[2], NULL);
						rv_z = stof(param[3], NULL);

					} else if (strcmp(param[0], "SCALE") == 0) {
						param[1] = strtok(0, " ");
						param[2] = strtok(0, " ");
						param[3] = strtok(0, " ");
						float s_x = stof(param[1], NULL);
						float s_y = stof(param[2], NULL);
						float s_z = stof(param[3], NULL);
						s = glm::vec3(s_x, s_y, s_z);

					} else if (strcmp(param[0], "CENTER") == 0) {
						param[1] = strtok(0, " ");
						param[2] = strtok(0, " ");
						param[3] = strtok(0, " ");
						float c_x = stof(param[1], NULL);
						float c_y = stof(param[2], NULL);
						float c_z = stof(param[3], NULL);
						c = glm::vec3(c_x, c_y, c_z);

					} else if (strcmp(param[0], "PARENT") == 0) {
						param[1] = strtok(0, " ");
						parent_name = param[1];
						node->setParentName(parent_name);

					} else if (strcmp(param[0], "SHAPE") == 0) {
						param[1] = strtok(0, " ");
						if (strcmp(param[1], "cube") == 0)
							shape = new Cube();
						if (strcmp(param[1], "sphere") == 0)
							shape = new Sphere();
						if (strcmp(param[1], "cylinder") == 0)
							shape = new Cylinder();
						if (strcmp(param[1], "mesh") == 0)
							shape = new Mesh();
						if (strcmp(param[1], "null") == 0)
							shape = NULL;
						node->setGeometry(shape);

					
					} else if (strcmp(param[0], "FILE") == 0) {
						param[1] = strtok(0, " ");
						obj_filename = param[1];
						objLoader loader = objLoader(obj_filename, node);
					

					} else if (strcmp(param[0], "MAT") == 0) {
						param[1] = strtok(0, " ");
						mat_name = param[1];

						for (unsigned int i = 0; i < all_materials.size(); i++) {
							if (all_materials.at(i).getMatName() == mat_name)
								node->setMat(all_materials.at(i));
						}

						// Set color to material's diffuse color
						color = node->getMat().getMatDiff();
						node->setRGBA(color);

						// SET LIGHT LOCATION AND COLOR HERE
						if (node->getMat().getMatEmit() > 0) {
							lcol = node->getMat().getMatDiff();
							lpos = c;
							lightDim = s;
							lightEmit = node->getMat().getMatEmit();
						}
					}

					configFile.getline(line, 100);
					param[0] = strtok(line, " ");
				}
	
				node->Translate(t.x, t.y, t.z);
				node->Rotate(rv_x, rv_y, rv_z);
				node->Scale(s.x, s.y, s.z);
				node->Pivot(-c.x, -c.y, -c.z);
							
				// Add node to the scene:
				scene->addNode(node);
			}
		}
	}
}








// Getters:

int ConfigReader::get_resoW() {
	return reso_w;
}

int ConfigReader::get_resoH() {
	return reso_h;
}

glm::vec3 ConfigReader::get_E() {
	return eyep;
}

glm::vec3 ConfigReader::get_C() {
	return vdir;
}

glm::vec3 ConfigReader::get_U() {
	return uvec;
}

float ConfigReader::get_FOVY() {
	return fovy;
}

glm::vec3 ConfigReader::get_LPOS() {
	return lpos;
}

glm::vec3 ConfigReader::get_LCOL() {
	return lcol;
}

glm::vec3 ConfigReader::get_LightDim() {
	return lightDim;
}

int ConfigReader::get_LightEmit() {
	return lightEmit;
}


bool ConfigReader::get_AA() {
	return antialiasing;
}

bool ConfigReader::get_MCD() {
	return mc_direct;
}
	
bool ConfigReader::get_MCI() {
	return mc_indirect;
}

int ConfigReader::get_Shadows() {
	return num_shadows;
}

int ConfigReader::get_numMC() {
	return num_mc;
}