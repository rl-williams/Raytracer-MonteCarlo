#include "Geometry.h"

#ifndef MESH_H
#define MESH_H

class Mesh : public Geometry
{
public:
    Mesh();
    virtual ~Mesh();

	void setVertices(vector<glm::vec3> v_obj);
	void setIndices(vector<unsigned int> i_obj);
	void setNormIndices(vector<unsigned int> ni_obj);
    virtual void buildGeometry();
	
	Intersection triangleIntersect(const Ray &ray, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) const;

private:
    glm::vec3 center_;
	vector<glm::vec3> vertices_obj;
	vector<unsigned int> indices_obj;
	vector<unsigned int> norm_indices_obj;

protected:
	virtual Intersection intersectImpl(const Ray &ray) const;
};

#endif