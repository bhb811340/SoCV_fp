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
	int getCut(int i) { return _cutNode[i]; };
	int getCounter(int i) { return _counterPart[i]; };
	unsigned getCutSize() { return _cutNode.size(); };
	unsigned getCounterSize() { return _counterPart.size(); };
	void pushCut(int Id) { _cutNode.push_back(Id); };
	void pushCounter(int Id) { _counterPart.push_back(Id); };

private:
	vector<int> _cutNode;
	vector<int> _counterPart;
};

#endif
