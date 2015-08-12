#ifndef CYLINDER_H
#define CYLINDER_H

#include "Geometry.h"

class Cylinder : public Geometry
{
public:
    Cylinder();
    virtual ~Cylinder();

    virtual void buildGeometry();

private:
    glm::vec3 center_;
    float radius_;
    float height_;

protected:
	virtual Intersection intersectImpl(const Ray &ray) const;
};

#endif