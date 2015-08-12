
#include "Node.h"

#ifndef OBJLOADER_H
#define OBJLOADER_H


class objLoader {

public:
	objLoader();
	objLoader(string file, Node* node);
	~objLoader();

private:
	vector<glm::vec3> vertices_obj;
	vector<glm::vec3> normals_obj;
	vector<unsigned int> indices_obj;
	vector<unsigned int> norm_indices_obj;
};

#endif