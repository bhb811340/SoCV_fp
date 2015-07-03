#ifndef CIRCUIT_H
#define CIRCUIT_H

#include <string>
#include <map>

#include "gate.h"
#include "wire.h"
#include "pattern.h"

using namespace std;

class Circuit
{
public:
    Circuit();
    ~Circuit();

    /*** construct ***/
    bool readVerilog(string fileName);
    bool writeVerilog(string fileName);

    /*** HCY add for logic simulation ***/
	bool setLevel();
    //Pattern*    RandomGenPattern();
	void        logicSim( Pattern* PatternSet );
	vector<int> value;
	vector<bool> cutPoint;

    /*** set ***/
    void setName(string cktName){ _name = cktName; };
    void addWire(unsigned wireId, string wireName, string wireType);
    void addGate(unsigned gateId, string gateName, string gateType);
    void addWireMap(string wireName, int wireId){ _wireMap[wireName] = wireId; };
    void addGateMap(string gateName, int gateId){ _gateMap[gateName] = gateId; };

    void setNumGate(unsigned numGate){ _numGate = numGate; };
    void setNumWire(unsigned numWire){ _numWire = numWire; };
    void setNumDuplicatedWire(unsigned numDuplicatedWire){ _numDuplicatedWire = numDuplicatedWire; };
    void setNumDuplicatedGate(unsigned numDuplicatedGate){ _numDuplicatedGate = numDuplicatedGate; };

    /*** get ***/
    unsigned numGate(){ return _numGate; };
    unsigned numWire(){ return _numWire; };
    unsigned numDuplicatedWire(){ return _numDuplicatedWire; };
    unsigned numDuplicatedGate(){ return _numDuplicatedGate; };
    int wireNameToId(string wireName){ return _wireMap.find(wireName)->second; };
    int gateNameToId(string gateName){ return _gateMap.find(gateName)->second; };

    /*** get object ***/
    Gate& gate(unsigned gateId){ return _gate[gateId]; };
    Wire& wire(unsigned wireId){ return _wire[wireId]; };

    /*** debug ***/
    void print();
    void printDuplicated();
    void dumpCircuit();

private:
    string _name;
    Gate* _gate;
    Wire* _wire;
    unsigned _numWire;
    unsigned _numGate;

    unsigned _numDuplicatedWire;
    unsigned _numDuplicatedGate;

    map<string, int> _wireMap;
    map<string ,int> _gateMap;

    /*** construct ***/
    void connectWire();
    void checkWire();
    void checkGate();

    /*** logic sim ***/
    void        AssignPiValue( Pattern* patternSet );
	vector<int> topologicalSequence;

};

#endif // CIRCUIT_H
