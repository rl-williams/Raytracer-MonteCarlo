
#include <glm\glm.hpp>
#include "Material.h"

#ifndef INTERSECTION_H
#define INTERSECTION_H

#define GLM_FORCE_RADIANS

struct Intersection {
	
	//The parameter 't' along the way which was used.
	//A negative value indicates no intersection.
	float t;

	//The surface normal at the point of intersection.
	//Ignored if t < 0.
	glm::vec3 normal;

	//the 'physical' point of intersection
	glm::vec3 point;

	Material m;
};

#endif
