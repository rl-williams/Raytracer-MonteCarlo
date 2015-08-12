
#include "Material.h"
#include <sstream>


// Constructor
Material::Material() {
}

Material::Material(std::string mat, glm::vec3 diff, glm::vec3 refl, float expo, float ior, bool mirr, bool tran, int emittance) {
	mat_name = mat;
	diff_color = diff;
	refl_color = refl;
	spec_expo = expo;
	refr_index = ior;
	mirror = mirr;
	transparent = tran;
	emitLight = emittance;
}

// Destructor
Material::~Material() {
}

// Getters
std::string Material::getMatName() {
	return mat_name;
}

glm::vec3 Material::getMatDiff() {
	return diff_color;
}

glm::vec3 Material::getMatRefl() {
	return refl_color;
}

float Material::getMatExpo() {
	return spec_expo;
}

float Material::getMatIor() {
	return refr_index;
}

bool Material::getMatMirr() {
	return mirror;
}

bool Material::getMatTran() {
	return transparent;
}

int Material::getMatEmit() {
	return emitLight;
}