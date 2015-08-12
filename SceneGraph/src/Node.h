#include <vector>
#include "Geometry.h"
#include "Material.h"
#include <glm/glm.hpp>
#include <string>


#ifndef NODE_H
#define NODE_H


class Node
{

public:

	// Constructors
	//Node();
	Node(string name, Node* parent = NULL);
	virtual ~Node(void);

	virtual void update();

	// Setters
	void setParentNode(Node* parent);
	void setParentName(string pname);
	void addChildNode(Node* child);
	void setGeometry(Geometry* geo);
	void setRGBA(glm::vec3 color);
	void setMat(Material m);

	// Getters
	Node* getParentNode(void) const;
	string getParentName();
	vector<Node*> getChildNodes();
	int getNumChildren();
	string getNodeName();
	Geometry* getGeometry();
	glm::mat4 getTransform();
	glm::vec3 getRGBA();
	glm::vec3 selected_rgba;	// original color of currently selected node
	Material getMat();

	// Transformations
	void Translate(float x, float y, float z);
	void Rotate(float x, float y, float z);
	void Scale(float x, float y, float z);
	void AddScale(float x, float y, float z);
	void Pivot(float x, float y, float z);

	// Selection
	void select();
	void deselect();
	bool isSelected();

private:

	string node_name;				// name of node
	string parent_name;				// name of parent node
	Node* node_parent;				// link to parent node
	vector<Node*> node_children;	// links to children nodes
	int num_children;				// number of children that have been added

	Geometry* node_geometry;		// link to geometry (cube, cylinder, sphere, or null)
	glm::mat4 node_transform;		// translation/rotation/scaling
	glm::vec3 node_rgba;			// color

	float tx, ty, tz;
	float rx, ry, rz;
	float sx, sy, sz;
	float px, py, pz;				// pivot info

	bool selected;					// selected or not

	Material mat;					// attached material for the node
};

#endif