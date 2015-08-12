#include "Mesh.h"

// Creates a mesh structure with center at the origin
Mesh::Mesh() :
	Geometry(MESH),
	center_(glm::vec3(0.f, 0.f, 0.f))
{
	buildGeometry();
}

// Destructor
Mesh::~Mesh() {}

void Mesh::setVertices(vector<glm::vec3> v_obj) {
	vertices_obj = v_obj;
}

void Mesh::setIndices(vector<unsigned int> i_obj) {
	indices_obj = i_obj;
}

void Mesh::setNormIndices(vector<unsigned int> ni_obj) {
	norm_indices_obj = ni_obj;
}

// Fills the arrays for each instance of Geometry
void Mesh::buildGeometry()
{

	//vertices
	for (unsigned int i = 0; i < indices_obj.size(); ++i) {
		vertices_.push_back(vertices_obj.at(indices_obj.at(i)));
	}

	//normals
	for (unsigned int i = 0; i < vertices_.size(); i += 3) {
		glm::vec3 v1, v2, v3, norm;
		v1 = vertices_.at(i);
		v2 = vertices_.at(i+1);
		v3 = vertices_.at(i+2);

		norm = glm::normalize(glm::cross(v2 - v1, v3 - v1));
		for (int j = 0; j < 3; ++j)
			normals_.push_back(norm);
	}

	// indices
	for (unsigned int i = 0; i < vertices_.size(); i++) {
		indices_.push_back(i);
	}

}

// Returns an Intersection of the ray with a sphere
Intersection Mesh::intersectImpl(const Ray &ray) const {

	Intersection i = Intersection();
	i.t = -1.0f;
	i.normal = glm::vec3(0.f);

	float smallestT = FLT_MAX;

	// check every triangle in the mesh
	int num_triangles = indices_.size() / 3;

	for (int n = 0; n < num_triangles; ++n) {

		// triangle vertices
		glm::vec3 p1 = vertices_.at(indices_.at(n * 3));
		glm::vec3 p2 = vertices_.at(indices_.at(n * 3 + 1));
		glm::vec3 p3 = vertices_.at(indices_.at(n * 3 + 2));

		// triangle intersection
		Intersection triI = triangleIntersect(ray, p1, p2, p3);

		// return only the closest triangle
		if (triI.t != -1.0 && triI.t < smallestT) {
			smallestT = triI.t;
			i = triI;
		}
	}
	
	return i;
}

// Helper triangle intersection function - WITH CORRECT INPUTS FOR TESTING
Intersection Mesh::triangleIntersect(const Ray &ray, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) const {

	Intersection i;
	i.t = -1.0f;
	i.normal = glm::vec3(0.f);

	// calculate triangle normal
	glm::vec3 u = p2 - p1;
	glm::vec3 v = p3 - p1;
	glm::vec3 n = glm::normalize(glm::cross(u, v));

	// Plane intersection
	float nDotD = glm::dot(n, ray.dir);
	if (nDotD == 0) return i;
	glm::vec3 x = ray.orig + ray.dir * (-glm::dot(n, ray.orig - p1) / nDotD);

	// see if it's within the 3 edges
	if (glm::dot(glm::cross(p2 - p1, x - p1), n) < 0) return i;
	if (glm::dot(glm::cross(p3 - p2, x - p2), n) < 0) return i;
	if (glm::dot(glm::cross(p1 - p3, x - p3), n) < 0) return i;

	i.t = -glm::dot(n, ray.orig - p1) / nDotD;
	i.normal = n;
	return i;
}