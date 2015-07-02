#ifndef EC_H
#define EC_H

#include "circuit.h"
#include "path.h"
#include "sat.h"
#include <string>

class EC
{
public:
    EC() {
	  _circuit = new Circuit[2];
	  _sat = new Sat[2];
	};
	~EC() {
	  delete [] _circuit;
	  delete [] _sat;
	};

	/***get***/
	Circuit& getCircuit(int i) {return _circuit[i];};
	Sat& getSat(int i){return _sat[i];};

	/***sat***/
	void ckt2sat(Circuit ckt, Sat s);
	Sat miter(Sat s1, Sat s2);
	bool solveSat(Sat s);

	/***cut***/


private:
    Circuit* _circuit;
	Sat* _sat;
};

#endif
