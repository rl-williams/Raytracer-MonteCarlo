#include "Geometry.h"

#include "Cube.h"
#include "Sphere.h"
#include "Cylinder.h"
#include "Mesh.h"

//Constructor
Geometry::Geometry(geometryType geomType) :
    type_(geomType)
{
}

//Destructor
Geometry::~Geometry()
{
    vertices_.clear();
    normals_.clear();
    colors_.clear();
    indices_.clear();
}

// Compute an intersection with a WORLD-space ray
Intersection Geometry::intersect(const glm::mat4 &T, Ray ray_world) const {

	// Transform the ray into OBJECT-LOCAL-space, for intersection calculation
	Ray ray_local;
	glm::mat4 T_inverse = glm::inverse(T);
	ray_local.orig = glm::vec3(T_inverse * glm::vec4(ray_world.orig, 1.0));
	ray_local.dir  = glm::normalize(glm::vec3(T_inverse * glm::vec4(ray_world.dir, 0.0)));

	//Compute the intersection in LOCAL-space.
	Intersection isx = intersectImpl(ray_local);

	if (isx.t != -1) {
		//Transform the local-space intersection BACK into world-space
		const glm::vec3 normal_local = isx.normal;
		//glm::vec3 normal_world = glm::normalize(glm::vec3(glm::vec4(normal_local, 0.0) * glm::transpose(glm::inverse(T))));
		glm::vec3 normal_world = glm::normalize(glm::vec3(glm::transpose(glm::inverse(T)) * glm::vec4(normal_local, 0.0)));

		//Transform t and point back into world-space as well
		isx.normal = normal_world;
		isx.t = isx.t / glm::length(glm::vec3(T_inverse * glm::vec4(ray_world.dir, 0.0)));
		isx.point = ray_world.orig + isx.t * ray_world.dir;
	}

	//The final output intersection data is in WORLD-space.
	return isx;
}

// apply transformation matrix to geometry vertices
void Geometry::transform(glm::mat4 transMat) {
}

// change color
void Geometry::changeColor(glm::vec3 newColor) {
	this->colors_.clear();
	for (unsigned int i = 0; i < vertices_.size(); ++i)
		this->colors_.push_back(newColor);
}