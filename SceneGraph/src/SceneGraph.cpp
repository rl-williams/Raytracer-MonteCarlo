#include "SceneGraph.h"
#include "Node.h"
#include <stack>
#include <string>
#include <iostream>


//Empty SceneGraph Constructor
SceneGraph::SceneGraph() {
	root = NULL;
	selected = NULL;
}


//Destructor
SceneGraph::~SceneGraph() {
}


//add a given node to the scene graph
void SceneGraph::addNode(Node* node) {

	//if no parent node, this is the root node
	if ((node->getParentName()).compare("null") == 0) {
		root = node;
		selected = root;
		root->select();
		return;
	}
	
	//if node has a parent_node, make node its child
	else if (node->getParentNode() != NULL) {
		(node->getParentNode())->addChildNode(node);
		return;
	}

	
	//but if it has a parent_name:
	else if (node->getParentName().compare("null") != 0) {

		string pname = node->getParentName();
		vector<Node*> traversal = preorderTraverse();

		for (vector<Node*>::const_iterator i = traversal.begin(); i != traversal.end(); ++i) {
			string nname = (*i)->getNodeName();
			if (pname.compare(nname) == 0) {
				(*i)->addChildNode(node);
				return;
			}
		}
	}
	

	//otherwise, we'll put it at the end of the traversal
	else {
		vector<Node*> traversal = preorderTraverse();
		Node* endNode = traversal.back();
		endNode->addChildNode(node);
		return;
	}
}


//Get root
Node* SceneGraph::getRoot() {
	return root;
}

//Get selected
Node* SceneGraph::getSelected() {
	return selected;
}

//Select next node in traversal
void SceneGraph::selectNext() {
	vector<Node*> traversal = preorderTraverse();
	for (unsigned int i = 0; i < traversal.size(); ++i) {
		//find the currently selected node...
		if (traversal.at(i)->isSelected()) {

			//and deselect it.
			traversal.at(i)->deselect();

			// if next node exists, select it
			if ((i + 1) < traversal.size()) {
				traversal.at(i+1)->select();
				selected = traversal.at(i+1);
				return;

			// otherwise we're at the end of the traversal, so select the root
			} else {
				traversal.at(0)->select();
				selected = traversal.at(0);
				return;
			}
		}
	}
}


//Traverse the graph and return a vector of nodes in preorder order
vector<Node*> SceneGraph::preorderTraverse() {

	vector<Node*> traversal;

	std::stack<Node*> stack;
	Node* currentNode = getRoot();
	stack.push(currentNode);

	while (stack.size() > 0) {

		currentNode = stack.top();
		stack.pop();

		if (currentNode->getNumChildren() > 0) {
			vector<Node*> children = currentNode->getChildNodes();
			if (!children.empty()) {
				for (vector<Node*>::const_iterator i = children.begin(); i != children.end(); ++i) {
					stack.push(*i);
				}
			}
		}
		traversal.push_back(currentNode);
	}

	return traversal;
}
