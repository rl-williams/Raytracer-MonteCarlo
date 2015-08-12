
#ifndef RAY_H
#define RAY_H

#define GLM_FORCE_RADIANS
#include <glm\glm.hpp>

struct Ray {

	glm::vec3 orig;
	glm::vec3 dir;

	inline Ray() {
	}

	inline Ray(glm::vec3 o, glm::vec3 d)
		: orig(o), dir(d) {
	}

};

#endif