
#include "Camera.h"
#include "glm\glm.hpp"


Camera::Camera() {
	reso_w = 0;
	reso_h = 0;
	setCamera();
}

Camera::Camera(glm::vec3 E_config, glm::vec3 C_config, glm::vec3 U_config,
			   float FOVY_config, int reso_w_config, int reso_h_config) {

	E = E_config;
	C = C_config;
	U = U_config;
	reso_w = reso_w_config;
	reso_h = reso_h_config;
	FOVY = FOVY_config;

	setCamera();
}

Camera::~Camera() {
}


void Camera::setCamera() {

	// convert FOVY from degrees to radians
	FOVY *= 3.14159f / 180.f;

	// compute FOVX
	FOVX = atan(tan(FOVY) * (reso_w - 1)/(reso_h - 1));

	A = glm::cross(C, U);	// A <-- C x U
	B = glm::cross(A, C);	// B <-- A x C
	M = E + C;				// M <-- E + C (screen midpoint)
	H = A * ((glm::length(C) * tan(FOVX)) / glm::length(A));	// H <-- (A |C| tan FOVX) / |A|
	V = B * ((glm::length(C) * tan(FOVY)) / glm::length(B));	// V <-- (B |C| tan FOVY) / |B|

	// NDC coordinates
	float sx, sy = 0.0;

	// world point array
	Pw_arr.resize(reso_w * reso_h);
	int pw_index = 0;

	for (int py = 0; py < reso_h; py++) {
		for (int px = 0; px < reso_w; px++) {

			// Get NDC coordinates using reso and current pixel value
			sx = (float) px / reso_w;
			sy = (float) py / reso_h;

			// convert the point to world space
			glm::vec3 pw = M + ((2 * sx - 1) * H) + ((1 - 2 * sy) * V);
			Pw_arr[pw_index] = pw;

			++pw_index;
		}
	}
}


glm::vec3 Camera::GetWorldPointVec(float px, float py) {

	// Get NDC coordinates using reso and current pixel value
	float sx, sy = 0.0;
	sx = (float) px / reso_w;
	sy = (float) py / reso_h;

	// convert the point to world space
	glm::vec3 pw = M + ((2 * sx - 1) * H) + ((1 - 2 * sy) * V);
	return pw;
}


