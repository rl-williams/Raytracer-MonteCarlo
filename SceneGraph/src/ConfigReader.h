
#ifndef CONFIGREADER
#define CONFIGREADER

#include "Geometry.h"
#include "Material.h"
#include "SceneGraph.h"

using namespace std;

class ConfigReader {

private:

	//variables read in from the config file:
	int reso_w;
	int reso_h;
	glm::vec3 eyep;
	glm::vec3 vdir;
	glm::vec3 uvec;
	float fovy;

	glm::vec3 lpos;
	glm::vec3 lcol;

	string node_name;
	glm::mat4 transMat;
	string parent_name;
	Geometry* shape;
	string obj_filename;
	glm::vec3 color;

	string mat_name;
	vector<Material> all_materials;

	glm::vec3 lightDim;
	int lightEmit;

	//Ray-Tracing stuff
	bool antialiasing;
	bool mc_direct;
	bool mc_indirect;
	int num_shadows;
	int num_mc;

public:

	//constructors:
	ConfigReader();
	ConfigReader(char* file, SceneGraph* scene);

	//getters:
	int get_resoW();
	int get_resoH();
	glm::vec3 get_E();
	glm::vec3 get_C();
	glm::vec3 get_U();
	float get_FOVY();
	glm::vec3 get_LPOS();
	glm::vec3 get_LCOL();
	glm::vec3 get_LightDim();
	int get_LightEmit();
	bool get_AA();
	bool get_MCD();
	bool get_MCI();
	int get_Shadows();
	int get_numMC();
};

#endif