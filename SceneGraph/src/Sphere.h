#ifndef SPHERE_H
#define SPHERE_H

#include "Geometry.h"

class Sphere : public Geometry
{
public:
    Sphere();
    virtual ~Sphere();

    virtual void buildGeometry();

private:
    glm::vec3 center_;
    float radius_;

protected:
	virtual Intersection intersectImpl(const Ray &ray) const;
};

#endif