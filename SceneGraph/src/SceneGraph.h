#include "Node.h"

#ifndef SCENEGRAPH_h
#define SCENEGRAPH_h

class SceneGraph
{

public:
	SceneGraph();
	virtual ~SceneGraph();

	Node* getRoot();
	Node* getSelected();

	void addNode(Node* node);
	vector<Node*> preorderTraverse();

	void selectNext();

private:
	Node* root;
	Node* selected;

};

#endif