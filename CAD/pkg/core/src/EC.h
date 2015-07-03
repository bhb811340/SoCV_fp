#ifndef EC_H
#define EC_H

#include "circuit.h"
#include "path.h"
#include "sat.h"
#include "atpg.h"
#include <string>
#include <vector>

class EC
{
public:
    EC() {
	  _circuit = new Circuit[2];
	  _sat = new Sat;
	  _GateId = new int[2];
	  _dfsorder = new vector<int>[2];
	};
	~EC() {
	  delete [] _GateId;
	  delete [] _dfsorder;
	};
    /***set***/
    void setCircuit(Atpg atpg, int i) {_circuit[i] = atpg.circuit(i);} //get circuit from atpg
    void getPES(vector<int> PES) {_PES = PES;} //get possible equivalence set from atpg.PES
    void cutpointAssign(int i, int j){_GateId[i] = j;}
    /***get***/
	Circuit& getCircuit(int i) {return _circuit[i];}
	Sat& getSat() {return *_sat;}
	int getId(int i) {return _GateId[i];}
    vector<int> getDfsorder(int i) {return _dfsorder[i];}
    vector<int>* getDfsorderPointer() {return _dfsorder;}
	/***sat***/
	void getGateSat(Circuit ckt, Sat s, vector<int> dfsorder, int offset);
	Sat miter(Sat s, vector<int>* dfs, int offset);
	bool solveSat(Sat s);
    void dfsorder(Circuit ckt, int WireId, vector<int> list) {
		Gate g = ckt.gate(ckt.wire(WireId).preGate());
        for(unsigned i = 0; i < g.numInWire(); ++i) {
            int inWire = g.inWire(i);
            if ( ckt.wire(inWire).type() != "PI" && ckt.wire(inWire).type() != "CUT"
			  && ckt.wire(inWire).type() != "CUT_BAR" && 
			  ckt.wire(inWire).type() != "TIE0" && ckt.wire(inWire).type() != "TIE1")
                dfsorder(ckt, inWire, list);
        }
        list.push_back(WireId);
    }
	/***cut***/
    void checkPES();

private:
    Circuit* _circuit;
	Sat* _sat;
	int* _GateId;
	vector<int>* _dfsorder;
    vector<int> _PES;
};

#endif
