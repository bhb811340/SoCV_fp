#ifndef EC_H
#define EC_H

#include "circuit.h"
#include "path.h"
#include "sat.h"
#include "atpg.h"
#include <string>
#include <vector>
#include <iostream>

class EC
{
public:
    EC() {
	  _sat = new Sat;
	  _WireId = new int[2];
	  _dfsorder = new vector<int>[2];
	};
	~EC() {
	  delete [] _WireId;
	  delete [] _dfsorder;
	  delete [] _dfsorder;
	};
    /***set***/
    void setCircuit(int i, Circuit* ckt) {_circuit[i] = ckt;} //get circuit from atpg
    void getPES(vector<int> PES) {_PES = PES;} //get possible equivalence set from atpg.PES
    void cutpointAssign(int i, int j){_WireId[i] = j;}

    /***get***/
	Circuit* getCircuit(int i) {return _circuit[i];}
	Sat* getSat() {return _sat;}
	int getId(int i) {return _WireId[i];}
    vector<int> getDfsorder(int i) {return _dfsorder[i];}
    vector<int>* getDfsorderPointer() {return _dfsorder;}

	/***sat***/
	void getGateSat(Circuit* ckt, Sat* s, vector<int> dfsorder, int offset);
	Sat* miter(Sat* s, vector<int>* dfs, int offset);
	bool solveSat(Sat* s);
    void dfsorder(Circuit* ckt, int WireId, int list ) {
        //cout<< ckt->wire(WireId).name() <<endl;
		Gate g = ckt->gate(ckt->wire(WireId).preGate());
        for(unsigned i = 0; i < g.numInWire(); ++i) {
            int inWire = g.inWire(i);
            if ( ckt->wire(inWire).type() != "PI" 
                && ckt->wire(inWire).type() != "TIE0" 
                && ckt->wire(inWire).type() != "TIE1"
                && !ckt->wire(inWire).isCutPoint()  )
                dfsorder(ckt, inWire, list);
        }
        _dfsorder[list].push_back(WireId);
    }
    void printDFS()
    {
        for( int i =0; i < 2; ++i )
        {
            cout<< "DFS order for circuit " << i <<": ";
            for( unsigned j = 0; j <_dfsorder[i].size(); ++j )
            {
                cout<< _dfsorder[i][j]<<' ';
            }
            cout<<endl;
        }
    }

	/***cut***/
    void checkPES();

private:
    Circuit* _circuit[2];
	Sat* _sat;
	int* _WireId;
	vector<int>* _dfsorder;
    vector<int> _PES;
};

#endif
