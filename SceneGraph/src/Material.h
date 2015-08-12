
#ifndef MATERIAL_H
#define MATERIAL_H

#include "glm\glm.hpp"
#include <sstream>

class Material {

public:

	// Constructors
	Material();
	Material(std::string mat, glm::vec3 diff, glm::vec3 refl, float expo, float ior, bool mirr, bool tran, int emittance);
	~Material();

	// Getters
	std::string getMatName();
	glm::vec3 getMatDiff();
	glm::vec3 getMatRefl();
	float getMatExpo();
	float getMatIor();
	bool getMatMirr();
	bool getMatTran();
	int getMatEmit();

protected:

	std::string mat_name;	// name of the material
	glm::vec3 diff_color;	// material's diffuse color, RGB
	glm::vec3 refl_color;	// material's reflective/specular color, RGB
	float spec_expo;		// specular exponent
	float refr_index;		// index of refraction
	bool mirror;			// 1 = reflectance, 0 = specular
	bool transparent;		// 1 = transparent/refractive, 0 = opaque
	int emitLight;			// >1 = emittance value, 0 = no light

};

#endif