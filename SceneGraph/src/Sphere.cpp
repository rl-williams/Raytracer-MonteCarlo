#include "Sphere.h"

#include <iostream>

static const float PI = 3.141592653589f;

// Creates a unit sphere.
Sphere::Sphere() :
    Geometry(SPHERE),
    center_(glm::vec3(0.f, 0.f, 0.f)),
    radius_(1.0f)
{
    buildGeometry();
}

Sphere::~Sphere() {}

void Sphere::buildGeometry()
{
    vertices_.clear();
    colors_.clear();
    normals_.clear();
    indices_.clear();

    // Find vertex positions for the sphere.
    unsigned int subdiv_axis = 16;      // vertical slices
    unsigned int subdiv_height = 16;    // horizontal slices
    float dphi = PI / subdiv_height;
    float dtheta = 2.0f * PI / subdiv_axis;
    float epsilon = 0.0001f;
    glm::vec3 color (0.6f, 0.6f, 0.6f);

    // North pole
    glm::vec3 point (0.0f, 1.0f, 0.0f);
    normals_.push_back(point);
    // scale by radius_ and translate by center_
    vertices_.push_back(center_ + radius_ * point);

    for (float phi = dphi; phi < PI; phi += dphi) {
        for (float theta = dtheta; theta <= 2.0f * PI + epsilon; theta += dtheta) {
            float sin_phi = sin(phi);

            point[0] = sin_phi * sin(theta);
            point[1] = cos(phi);
            point[2] = sin_phi * cos(theta);

            normals_.push_back(point);
            vertices_.push_back(center_ + radius_ * point);
        }
    }
    // South pole
    point = glm::vec3(0.0f, -1.0f, 0.0f);
    normals_.push_back(point);
    vertices_.push_back(center_ + radius_ * point);

    // fill in index array.
    // top cap
    for (unsigned int i = 0; i < subdiv_axis - 1; ++i) {
        indices_.push_back(0);
        indices_.push_back(i + 1);
        indices_.push_back(i + 2);
    }
    indices_.push_back(0);
    indices_.push_back(subdiv_axis);
    indices_.push_back(1);

    // middle subdivs
    unsigned int index = 1;
    for (unsigned int i = 0; i < subdiv_height - 2; i++) {
        for (unsigned int j = 0; j < subdiv_axis - 1; j++) {
            // first triangle
            indices_.push_back(index);
            indices_.push_back(index + subdiv_axis);
            indices_.push_back(index + subdiv_axis + 1);

            // second triangle
            indices_.push_back(index);
            indices_.push_back(index + subdiv_axis + 1);
            indices_.push_back(index + 1);

            index++;
        }
        // reuse vertices from start and end point of subdiv_axis slice
        indices_.push_back(index);
        indices_.push_back(index + subdiv_axis);
        indices_.push_back(index + 1);

        indices_.push_back(index);
        indices_.push_back(index + 1);
        indices_.push_back(index + 1 - subdiv_axis);

        index++;
    }

    // end cap
    unsigned int bottom = (subdiv_height - 1) * subdiv_axis + 1;
    unsigned int offset = bottom - subdiv_axis;
    for (unsigned int i = 0; i < subdiv_axis - 1 ; ++i) {
        indices_.push_back(bottom);
        indices_.push_back(i + offset);
        indices_.push_back(i + offset + 1);
    }
    indices_.push_back(bottom);
    indices_.push_back(bottom - 1);
    indices_.push_back(offset);
}


// Returns an Intersection of the ray with a sphere
Intersection Sphere::intersectImpl(const Ray &ray) const {

	Intersection i = Intersection();
	i.t = -1.0f;
	i.normal = glm::vec3(0.f);
	
	//float b = glm::dot(ray.orig - center_, ray.dir);
	float b = glm::dot(2.f * ray.dir, (ray.orig - center_));
	float c = glm::dot(ray.orig - center_, ray.orig - center_) - (radius_*radius_);
	float d = b*b - 4*c;
	
	// no intersection
	if (d < 0) return i;
	
	float t0 = (-b + glm::sqrt(d)) * 0.5f;
	float t1 = (-b - glm::sqrt(d)) * 0.5f;

	// find intersection point
	glm::vec3 interPoint;
	glm::vec3 interPoint1 = ray.orig + ray.dir * t0;
	glm::vec3 interPoint2 = ray.orig + ray.dir * t1;

	// 2 real positive roots t's = two intersection points, pick smaller one
	if (t0 >= 0 && t1 >= 0) {
		if (glm::distance(interPoint1, ray.orig) <= glm::distance(interPoint2, ray.orig)) {
			i.t = t0;
			interPoint = interPoint1;
		} else {
			i.t = t1;
			interPoint = interPoint2;
		}

	// if only one is positive, we're inside the sphere
	} else if (t0 >= 0) {
		i.t = t0;
		interPoint = interPoint1;

	} else if (t1 >= 0) {
		i.t = t1;
		interPoint = interPoint2;

	// if they're both < 0, no intersection
	} else {
		return i;
	}

	i.normal = glm::normalize(interPoint - center_);

	return i;
}