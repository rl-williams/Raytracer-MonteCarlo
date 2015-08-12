#include "Node.h"
#include <sstream>


// Constructor
Node::Node(string name, Node* parent) : node_name(name) {
	node_parent = parent;
	selected = false;
	node_transform = glm::mat4();	//initial transformation matrix is just identity matrix
	num_children = 0;
	tx = ty = tz = px = py = pz = rx = ry = rz = 0;
	sx = sy = sz = 1;
}

// Destructor
Node::~Node(void) {
	node_parent = NULL;
	node_children.clear();
}

//recursively move through scene/children and update
void Node::update() {
	node_geometry->transform(node_transform);
	if (!node_children.empty()) {
		for (unsigned int i = 0; i < node_children.size(); ++i) {
			node_children.at(i)->update();
		}
	}
}


// set parent node
void Node::setParentNode(Node* parent) {
	node_parent = parent;
}

// get parent node
Node* Node::getParentNode(void) const{
	return node_parent;
}

// set parent node name
void Node::setParentName(string pname) {
	if (pname.compare("null")) node_parent = NULL;
	parent_name = pname;
}

// get parent node name
string Node::getParentName() {
	return parent_name;
}

// add given child node to current node's children
void Node::addChildNode(Node* child) {
	num_children++;
	if (child != NULL) {
		child->setParentNode(this);
		node_children.push_back(child);
	}
}

// get vector of child nodes
vector<Node*> Node::getChildNodes() {
	return node_children;
}

// get number of children
int Node::getNumChildren() {
	return num_children;
}

// get node name
string Node::getNodeName() {
	return node_name;
}

// set geometry
void Node::setGeometry(Geometry* geo) {
	node_geometry = geo;
}

// get geometry
Geometry* Node::getGeometry() {
	return node_geometry;
}


// get current transformation matrix
glm::mat4 Node::getTransform() {
	glm::mat4 result = glm::translate(glm::mat4(1.0f), glm::vec3(px, py, pz));
	result = glm::scale(glm::mat4(1.0f), glm::vec3(sx, sy, sz)) * result;
	result = glm::rotate(glm::mat4(1.0f), rz * 3.14159f/180.0f, glm::vec3(0,0,1)) * result;
	result = glm::rotate(glm::mat4(1.0f), ry * 3.14159f/180.0f, glm::vec3(0,1,0)) * result;
	result = glm::rotate(glm::mat4(1.0f), rx * 3.14159f/180.0f, glm::vec3(1,0,0)) * result;
	result = glm::translate(glm::mat4(1.0f), glm::vec3(tx, ty, tz)) * result;
	return result;
}


// Make current node "selected"
void Node::select() {
	selected_rgba = this->getRGBA();
	selected = true;
	setRGBA(glm::vec3(255, 255, 255));
}

// Deselect current node
void Node::deselect() {
	setRGBA(selected_rgba);
	selected = false;
}

//is selected
bool Node::isSelected() {
	return selected;
}

//get rgba
glm::vec3 Node::getRGBA() {
	return node_rgba;
}

//set new rgba
void Node::setRGBA(glm::vec3 color) {
	node_rgba = color;
	if (node_geometry != NULL)
		node_geometry->changeColor(node_rgba);
}


// Transformations:
void Node::Pivot(float x, float y, float z) {
	px = x;
	py = y;
	pz = z;
}

void Node::Translate(float x, float y, float z) {
	tx += x;
	ty += y;
	tz += z;
}

void Node::Rotate(float x, float y, float z) {
	rx += x;
	ry += y;
	rz += z;
}

void Node::AddScale(float x, float y, float z) {
	sx += x;
	sy += y;
	sz += z;
}

void Node::Scale(float x, float y, float z) {
	sx *= x;
	sy *= y;
	sz *= z;
}


// Set Material
void Node::setMat(Material m) {
	mat = m;
}

// Get Material
Material Node::getMat() {
	return mat;
}

