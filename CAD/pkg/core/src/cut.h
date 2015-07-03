#ifndef CUT_H
#define CUT_H

#include "circuit.h"
#include "sat.h"
#include <string>
#include <vector>

class Cut
{
public:
	Cut() {};
	~Cut() { 
		_cutNode.clear(); 
		_counterPart.clear(); };
	int getNode(int i) { return _cutNode[i]; };
	int getCounter(int i) { return _counterPart[i]; };

private:
	vector<int> _cutNode;
	vector<int> _counterPart;
};
