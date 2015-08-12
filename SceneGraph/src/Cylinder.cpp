#include "Cylinder.h"

static const float PI = 3.141592653589f;

// Creates a unit cylinder centered at (0, 0, 0)
Cylinder::Cylinder() :
    Geometry(CYLINDER),
    center_(glm::vec3(0.f, 0.f, 0.f)),
    radius_(0.5f),
    height_(1.0f)
{
    buildGeometry();
}

Cylinder::~Cylinder() {}

void Cylinder::buildGeometry()
{
    vertices_.clear();
    colors_.clear();
    normals_.clear();
    indices_.clear();

    unsigned short subdiv = 20;
    float dtheta = 2 * PI / subdiv;

    glm::vec4 point_top(0.0f, 0.5f * height_, radius_, 1.0f),
        point_bottom (0.0f, -0.5f * height_, radius_, 1.0f);
    vector<glm::vec3> cap_top, cap_bottom;

    // top and bottom cap vertices
    for (int i = 0; i < subdiv + 1; ++i) {
        glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), i * dtheta, glm::vec3(0.f, 1.f, 0.f));
        glm::mat4 translate = glm::translate(glm::mat4(1.0f), center_);

        cap_top.push_back(glm::vec3(translate * rotate * point_top));
        cap_bottom.push_back(glm::vec3(translate * rotate * point_bottom));
    }

    //Create top cap.
    for ( int i = 0; i < subdiv - 2; i++) {
        vertices_.push_back(cap_top[0]);
        vertices_.push_back(cap_top[i + 1]);
        vertices_.push_back(cap_top[i + 2]);
    }
    //Create bottom cap.
    for (int i = 0; i < subdiv - 2; i++) {
        vertices_.push_back(cap_bottom[0]);
        vertices_.push_back(cap_bottom[i + 1]);
        vertices_.push_back(cap_bottom[i + 2]);
    }
    //Create barrel
    for (int i = 0; i < subdiv; i++) {
        //Right-side up triangle
        vertices_.push_back(cap_top[i]);
        vertices_.push_back(cap_bottom[i + 1]);
        vertices_.push_back(cap_bottom[i]);
        //Upside-down triangle
        vertices_.push_back(cap_top[i]);
        vertices_.push_back(cap_top[i + 1]);
        vertices_.push_back(cap_bottom[i + 1]);
    }

    // create normals
    glm::vec3 top_centerpoint(0.0f , 0.5f * height_ , 0.0f),
        bottom_centerpoint(0.0f, -0.5f * height_, 0.0f);
    glm::vec3 normal(0, 1, 0);

    // Create top cap.
    for (int i = 0; i < subdiv - 2; i++) {
        normals_.push_back(normal);
        normals_.push_back(normal);
        normals_.push_back(normal);
    }
    // Create bottom cap.
    for (int i = 0; i < subdiv - 2; i++) {
        normals_.push_back(-normal);
        normals_.push_back(-normal);
        normals_.push_back(-normal);
    }

    // Create barrel
    for (int i = 0; i < subdiv; i++) {
        //Right-side up triangle
        normals_.push_back(glm::normalize(cap_top[i] - top_centerpoint));
        normals_.push_back(glm::normalize(cap_bottom[i + 1] - bottom_centerpoint));
        normals_.push_back(glm::normalize(cap_bottom[i] - bottom_centerpoint));
        //Upside-down triangle
        normals_.push_back(glm::normalize(cap_top[i] - top_centerpoint));
        normals_.push_back(glm::normalize(cap_top[i + 1] - top_centerpoint));
        normals_.push_back(glm::normalize(cap_bottom[i + 1] - bottom_centerpoint));
    }

    // indices
    for (unsigned int i = 0; i < vertices_.size(); ++i) {
        indices_.push_back(i);
    }
}

// Returns an Intersection of the ray with a cylinder
Intersection Cylinder::intersectImpl(const Ray &ray) const {

	Intersection i;

	float b = (ray.orig.x * ray.dir.x) + (ray.orig.z * ray.dir.z);
	float c = (ray.dir.x * ray.dir.x) + (ray.dir.z * ray.dir.z);
	float d = (b * b) - c * (ray.orig.x * ray.orig.x + ray.orig.z * ray.orig.z - radius_ * radius_);

	// no intersection with sides
	if (d < 0) {
		i.t = -1.0f;
		i.normal = glm::vec3(0.f);
		return i;

	// check caps
	} else if (d == 0) {

		// top cap
		glm::vec3 topPoint = glm::vec3(0, height_/2, 0);
		glm::vec3 topNorm = glm::vec3(0, 1, 0);
		float s = glm::dot(topNorm, ray.dir);
		float topD = radius_;
		if (s != 0) {
			glm::vec3 interPointTop = ray.orig + ray.dir * (-glm::dot(topNorm, ray.orig - topPoint) / s);
			topD = glm::distance(interPointTop, topPoint);
			if (topD <= radius_) {
				i.t = -glm::dot(topNorm, ray.orig - topPoint) / s;
				i.normal = topNorm;
			}
		}

		// bottom cap
		glm::vec3 botPoint = glm::vec3(0, -height_/2, 0);
		glm::vec3 botNorm = glm::vec3(0, -1, 0);
		s = glm::dot(botNorm, ray.dir);
		float botD = radius_;
		if (s != 0) {
			glm::vec3 interPointBot = ray.orig + ray.dir * (-glm::dot(botNorm, ray.orig - botPoint) / s);
			botD = glm::distance(interPointBot, botPoint);
			if (botD <= radius_ && botD < topD) {
				i.t = -glm::dot(botNorm, ray.orig - botPoint) / s;
				i.normal = botNorm;
			}
		}
		return i;
	}

	float t1 = -b - glm::sqrt(d);
	float t2 = -b + glm::sqrt(d);

	glm::vec3 interPoint;
	glm::vec3 interPoint1 = ray.orig + ray.dir * t1;
	glm::vec3 interPoint2 = ray.orig + ray.dir * t2;

	if (glm::distance(interPoint1, ray.orig) <= glm::distance(interPoint2, ray.orig)) {
		i.t = t1;
		interPoint = interPoint1;
	} else {
		i.t = t2;
		interPoint = interPoint2;
	}

	// if above/below height
	if (interPoint.y > center_.y + height_ / 2 || interPoint.y < center_.y - height_ / 2) {
		i.t = -1.0f;
		i.normal = glm::vec3(0.f);
		return i;
	}
	// caps
	/*} else if (glm::distance(interPoint, glm::vec3(0, center_.y + height_/2, 0)) <= radius_) {
		i.normal = glm::vec3(0, 1, 0);
		return i;
	} else if (glm::distance(interPoint, glm::vec3(0, center_.y - height_/2, 0)) <= radius_) {
		i.normal = glm::vec3(0, -1, 0);
		return i;
	}*/

	glm::vec3 normPoint = glm::vec3(interPoint.x, center_.y, interPoint.z);
	i.normal = (normPoint - center_) / glm::length(normPoint - center_);
	return i;
}
