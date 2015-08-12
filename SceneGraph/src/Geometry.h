#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <vector>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Intersection.h"
#include "Ray.h"

using namespace std;

// An abstract base class for geometry in the scene graph.
class Geometry
{
public:

    enum geometryType {CUBE, SPHERE, CYLINDER, MESH, NONE};
    explicit Geometry(geometryType);
    virtual ~Geometry();

    // Function for building vertex data, i.e. vertices, colors, normals, indices.
    virtual void buildGeometry() = 0;

	// apply transformation matrix to geometry vertices
	void transform(glm::mat4 transMat);

	// change color
	void changeColor(glm::vec3 newColor);


    /* * * Getters * * */
    const vector<glm::vec3>& getVertices() const
    { return vertices_; };

    const vector<glm::vec3>& getNormals() const
    { return normals_; };

    const vector<glm::vec3>& getColors() const
    { return colors_; };
    
	const vector<unsigned int>& getIndices() const
    { return indices_; };

    unsigned int getVertexCount() const
    { return vertices_.size(); };
    
	unsigned int getIndexCount() const
    { return indices_.size(); };

    const geometryType getGeometryType() const
    { return type_; };


	// Compute an intersection with a WORLD-space ray
	Intersection intersect(const glm::mat4 &T, Ray ray_world) const;


protected:
    geometryType type_;

    vector<glm::vec3> vertices_;        // vertex buffer
    vector<glm::vec3> normals_;         // normal buffer
    vector<glm::vec3> colors_;          // color buffer
    vector<unsigned int> indices_;      // index buffer

	// Compute an intersection with an OBJECT-LOCAL-space ray
	virtual Intersection intersectImpl(const Ray &ray) const = 0;
};

#endif