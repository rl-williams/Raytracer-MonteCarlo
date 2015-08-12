
#include "objLoader.h"
#include "Node.h"
#include "Mesh.h"
#include "Geometry.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>

//empty constructor
objLoader::objLoader() {
}

//parse file, send vertices and indices to node's mesh using setVertices and setIndices
objLoader::objLoader(string file, Node* node) {

	ifstream objFile(file);

	if (!objFile.is_open()) {
		cout << "Could not open obj file." << endl;
	}

	else {

		//only need to handle the lines starting with v/vn/f 
		while (!objFile.eof()) {

			std::string line;
			std::getline(objFile, line);

			if (line.size() > 0) {

				// vertices
				if (line[0] == 'v' && line[1] == ' ') {
					float vx, vy, vz;
					sscanf_s(line.c_str() + 2, "%f %f %f", &vx, &vy, &vz);
					vertices_obj.push_back(glm::vec3(vx, vy, vz));
				}

				// normals
				if (line[0] == 'v' && line[1] == 'n') {
					float nx, ny, nz;
					sscanf_s(line.c_str() + 3, "%f %f %f", &nx, &ny, &nz);
					normals_obj.push_back(glm::vec3(nx, ny, nz));
				}

				// triangular faces: v/vt, v/vt/vn, or v//vn
				if (line[0] == 'f') {

					//f v/vt v/vt v/vt
					if (count(line.begin(), line.end(), '/') == 3) {
						int v1, v2, v3;
						int t1, t2, t3;

						sscanf_s(line.c_str() + 2, "%i/%i %i/%i %i/%i",
							 &v1, &t1, &v2, &t2, &v3, &t3);

						indices_obj.push_back(v1-1);
						indices_obj.push_back(v2-1);
						indices_obj.push_back(v3-1);
					}

					else {

						std::size_t found = line.find("//");

						//f v/vt/vn v/vt/vn v/vt/vn
						if (found == std::string::npos) {
							int v1, v2, v3;
							int t1, t2, t3;
							int n1, n2, n3;

							sscanf_s(line.c_str() + 2, "%f/%f/%f %f/%f/%f %f/%f/%f",
								 &v1, &t1, &n1, &v2, &t2, &n2, &v3, &t3, &n3);

							indices_obj.push_back(v1-1);
							indices_obj.push_back(v2-1);
							indices_obj.push_back(v3-1);

							norm_indices_obj.push_back(n1-1);
							norm_indices_obj.push_back(n2-1);
							norm_indices_obj.push_back(n3-1);
						}

						//f v//vn v//vn v//vn
						else {
							int v1, v2, v3;
							int n1, n2, n3;

							sscanf_s(line.c_str() + 2, "%f//%f %f//%f %f//%f",
								 &v1, &n1, &v2, &n2, &v3, &n3);

							indices_obj.push_back(v1-1);
							indices_obj.push_back(v2-1);
							indices_obj.push_back(v3-1);

							norm_indices_obj.push_back(n1-1);
							norm_indices_obj.push_back(n2-1);
							norm_indices_obj.push_back(n3-1);
						}
					}
				}
			}
		}
	}

	if (node->getGeometry()->getGeometryType() == 3) {
		Mesh* shape = new Mesh();
		shape->setVertices(vertices_obj);
		shape->setIndices(indices_obj);
		shape->setNormIndices(norm_indices_obj);
		shape->buildGeometry();
		node->setGeometry(shape);
	}
}

//Destructor
objLoader::~objLoader() {}