#include "Cube.h"
#include <vector>

// Creates a unit cube.
Cube::Cube() :
	Geometry(CUBE),
	center_(glm::vec3(0.f, 0.f, 0.f)),
	slength_(1.0f)
{
	buildGeometry();
}

// Destructor
Cube::~Cube() {}

// Fills the arrays for each instance of Geometry.
void Cube::buildGeometry()
{
    vertices_.clear();
    colors_.clear();
    normals_.clear();
    indices_.clear();

	float h = slength_/2.0f;

	//front
	vertices_.push_back(glm::vec3(center_.x - h, center_.y + h, center_.z + h));	//left upper front
	vertices_.push_back(glm::vec3(center_.x + h, center_.y + h, center_.z + h));	//right upper front
	vertices_.push_back(glm::vec3(center_.x - h, center_.y - h, center_.z + h));	//left lower front
	vertices_.push_back(glm::vec3(center_.x + h, center_.y - h, center_.z + h));	//right upper front

	//back
	vertices_.push_back(glm::vec3(center_.x - h, center_.y + h, center_.z - h));	//left upper back
	vertices_.push_back(glm::vec3(center_.x + h, center_.y + h, center_.z - h));	//right upper back
	vertices_.push_back(glm::vec3(center_.x - h, center_.y - h, center_.z - h));	//left lower back
	vertices_.push_back(glm::vec3(center_.x + h, center_.y - h, center_.z - h));	//right lower back

	//left
	vertices_.push_back(glm::vec3(center_.x - h, center_.y + h, center_.z - h));	//left upper back
	vertices_.push_back(glm::vec3(center_.x - h, center_.y + h, center_.z + h));	//left upper front
	vertices_.push_back(glm::vec3(center_.x - h, center_.y - h, center_.z - h));	//left lower back
	vertices_.push_back(glm::vec3(center_.x - h, center_.y - h, center_.z + h));	//left lower front

	//right
	vertices_.push_back(glm::vec3(center_.x + h, center_.y + h, center_.z + h));	//right upper front
	vertices_.push_back(glm::vec3(center_.x + h, center_.y + h, center_.z - h));	//right upper back
	vertices_.push_back(glm::vec3(center_.x + h, center_.y - h, center_.z + h));	//right lower front
	vertices_.push_back(glm::vec3(center_.x + h, center_.y - h, center_.z - h));	//right lower back

	//top
	vertices_.push_back(glm::vec3(center_.x - h, center_.y + h, center_.z - h));	//left upper back
	vertices_.push_back(glm::vec3(center_.x + h, center_.y + h, center_.z - h));	//right upper back
	vertices_.push_back(glm::vec3(center_.x - h, center_.y + h, center_.z + h));	//left upper front
	vertices_.push_back(glm::vec3(center_.x + h, center_.y + h, center_.z + h));	//right upper front

	//bottom
	vertices_.push_back(glm::vec3(center_.x - h, center_.y - h, center_.z - h));	//left lower back
	vertices_.push_back(glm::vec3(center_.x + h, center_.y - h, center_.z - h));	//right lower back
	vertices_.push_back(glm::vec3(center_.x - h, center_.y - h, center_.z + h));	//left lower front
	vertices_.push_back(glm::vec3(center_.x + h, center_.y - h, center_.z + h));	//right lower front


	// normals - same normals for all four vertices of each face
	for (int i = 0; i < 4; ++i) //front
		normals_.push_back(glm::vec3(0, 0, -1));
	for (int i = 0; i < 4; ++i) //back
		normals_.push_back(glm::vec3(0, 0, 1));
	for (int i = 0; i < 4; ++i) //left
		normals_.push_back(glm::vec3(-1, 0, 0));
	for (int i = 0; i < 4; ++i) //right
		normals_.push_back(glm::vec3(1, 0, 0));
	for (int i = 0; i < 4; ++i) //top
		normals_.push_back(glm::vec3(0, 1, 0));
	for (int i = 0; i < 4; ++i) //bottom
		normals_.push_back(glm::vec3(0, -1, 0));

	// indices
	unsigned int arr[] = { 0, 1, 2,		//front upper triangle
						   1, 2, 3,		//front lower triangle

						   4, 5, 6,		//back upper triangle
						   5, 6, 7,		//back lower triangle

						   8, 9, 10,	//left upper triangle
						   9, 10, 11,   //left lower triangle

						   12, 13, 14,  //right upper triangle
						   13, 14, 15,  //right lower triangle

					       16, 17, 18,  //top upper triangle
						   17, 18, 19,  //top lower triangle

						   20, 21, 22,  //bottom upper triangle
						   21, 22, 23}; //bottom lower triangle

	std::vector<unsigned int> temp (arr, arr + sizeof(arr) / sizeof(unsigned int));
	indices_ = temp;
}

// Returns an Intersection of the ray with a cube
Intersection Cube::intersectImpl(const Ray &ray) const {

	Intersection i;
	i.t = -1.0;
	i.normal = glm::vec3(0.f);

	float d = FLT_MAX;
	float minX = FLT_MAX, minY = FLT_MAX, minZ = FLT_MAX;
	float maxX = FLT_MIN, maxY = FLT_MIN, maxZ = FLT_MIN;
	for (unsigned int j = 0; j < vertices_.size(); j++) {
		if (vertices_.at(j).x < minX) minX = vertices_.at(j).x;
		if (vertices_.at(j).y < minY) minY = vertices_.at(j).y;
		if (vertices_.at(j).z < minZ) minZ = vertices_.at(j).z;
		if (vertices_.at(j).x > maxX) maxX = vertices_.at(j).x;
		if (vertices_.at(j).y > maxY) maxY = vertices_.at(j).y;
		if (vertices_.at(j).z > maxZ) maxZ = vertices_.at(j).z;
	}

	glm::vec3 xPosNorm = glm::vec3(1, 0, 0);
	glm::vec3 xNegNorm = glm::vec3(-1, 0, 0);
	glm::vec3 yPosNorm = glm::vec3(0, 1, 0);
	glm::vec3 yNegNorm = glm::vec3(0, -1, 0);
	glm::vec3 zPosNorm = glm::vec3(0, 0, 1);
	glm::vec3 zNegNorm = glm::vec3(0, 0, -1);

	//+x-plane intersection
	if (glm::dot(xPosNorm, ray.dir) != 0) {

		glm::vec3 planePoint = glm::vec3(center_.x + slength_/2, center_.y, center_.z);
		float t = -glm::dot(xPosNorm, ray.orig - planePoint) / glm::dot(xPosNorm, ray.dir);
		glm::vec3 interPoint = glm::vec3(ray.orig + ray.dir * t);

		if (interPoint.y >= (minY - 0.0001) && interPoint.y <= (maxY + 0.0001) &&
			interPoint.z >= (minZ - 0.0001) && interPoint.z <= (maxZ + 0.0001) &&
			glm::distance(interPoint, ray.orig) < d) {
				d = glm::distance(interPoint, ray.orig);
				i.t = t;
				i.normal = xPosNorm;
		}
	}

	//-x-plane intersection
	if (glm::dot(xNegNorm, ray.dir) != 0) {

		glm::vec3 planePoint = glm::vec3(center_.x - slength_/2, center_.y, center_.z);
		float t = -glm::dot(xNegNorm, ray.orig - planePoint) / glm::dot(xNegNorm, ray.dir);
		glm::vec3 interPoint = glm::vec3(ray.orig + ray.dir * t);

		if (interPoint.y >= (minY - 0.0001) && interPoint.y <= (maxY + 0.0001) &&
			interPoint.z >= (minZ - 0.0001) && interPoint.z <= (maxZ + 0.0001) &&
			glm::distance(interPoint, ray.orig) < d) {
				d = glm::distance(interPoint, ray.orig);
				i.t = t;
				i.normal = xNegNorm;
		}
	}

	//+y-plane intersection
	if (glm::dot(yPosNorm, ray.dir) != 0) {

		glm::vec3 planePoint = glm::vec3(center_.x, center_.y + slength_/2, center_.z);
		float t = -glm::dot(yPosNorm, ray.orig - planePoint) / glm::dot(yPosNorm, ray.dir);
		glm::vec3 interPoint = glm::vec3(ray.orig + ray.dir * t);

		if (interPoint.x >= (minX - 0.0001) && interPoint.x <= (maxX + 0.0001) &&
			interPoint.z >= (minZ - 0.0001) && interPoint.z <= (maxZ + 0.0001) &&
			glm::distance(interPoint, ray.orig) < d) {
				d = glm::distance(interPoint, ray.orig);
				i.t = t;
				i.normal = yPosNorm;
		}
	}

	//-y-plane intersection
	if (glm::dot(yNegNorm, ray.dir) != 0) {

		glm::vec3 planePoint = glm::vec3(center_.x, center_.y - slength_/2, center_.z);
		float t = -glm::dot(yNegNorm, ray.orig - planePoint) / glm::dot(yNegNorm, ray.dir);
		glm::vec3 interPoint = glm::vec3(ray.orig + ray.dir * t);

		if (interPoint.x >= (minX - 0.0001) && interPoint.x <= (maxX + 0.0001) &&
			interPoint.z >= (minZ - 0.0001) && interPoint.z <= (maxZ + 0.0001) &&
			glm::distance(interPoint, ray.orig) < d) {
				d = glm::distance(interPoint, ray.orig);
				i.t = t;
				i.normal = yNegNorm;
		}
	}

	//+z-plane intersection
	if (glm::dot(zPosNorm, ray.dir) != 0) {

		glm::vec3 planePoint = glm::vec3(center_.x, center_.y, center_.z + slength_/2);
		float t = -glm::dot(zPosNorm, ray.orig - planePoint) / glm::dot(zPosNorm, ray.dir);
		glm::vec3 interPoint = glm::vec3(ray.orig + ray.dir * t);

		if (interPoint.x >= (minX - 0.0001) && interPoint.x <= (maxX + 0.0001) &&
			interPoint.y >= (minY - 0.0001) && interPoint.y <= (maxY + 0.0001) &&
			glm::distance(interPoint, ray.orig) < d) {
				d = glm::distance(interPoint, ray.orig);
				i.t = t;
				i.normal = zPosNorm;
		}
	}

	//-z-plane intersection
	if (glm::dot(zNegNorm, ray.dir) != 0) {

		glm::vec3 planePoint = glm::vec3(center_.x, center_.y, center_.z - slength_/2);
		float t = -glm::dot(zNegNorm, ray.orig - planePoint) / glm::dot(zNegNorm, ray.dir);
		glm::vec3 interPoint = glm::vec3(ray.orig + ray.dir * t);

		if (interPoint.x >= (minX - 0.0001) && interPoint.x <= (maxX + 0.0001) &&
			interPoint.y >= (minY - 0.0001) && interPoint.y <= (maxY + 0.0001) &&
			glm::distance(interPoint, ray.orig) < d) {
				d = glm::distance(interPoint, ray.orig);
				i.t = t;
				i.normal = zNegNorm;
		}
	}

	return i;
}