#include "Geometry.h"

#ifndef CUBE_H
#define CUBE_H

class Cube : public Geometry
{
public:
    Cube();
    virtual ~Cube();

    virtual void buildGeometry();

private:
    glm::vec3 center_;
    float slength_;
protected:
	virtual Intersection intersectImpl(const Ray &ray) const;
};

#endif