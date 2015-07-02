#ifndef ATPG_H
#define ATPG_H

#include "spec.h"
#include "circuit.h"
#include "path.h"
#include "sat.h"

using namespace std;

class Atpg
{
public:
    Atpg();
    ~Atpg();

    /*** get ***/
    unsigned duplicateIndex(){ return _duplicateIndex; };

    /*** get object ***/
    Spec& spec(){ return _spec; };
    Circuit& circuit(){ return _circuit; };
    Sat& sat(){ return _sat; };

    /*** launch on capture ***/
    void loc();

    /*** pattern generate ***/
    bool generatePattern(int testFaultIndex);
    void initializeData();

    /*** maximize target ***/
    void calTransitionPossibility(int testFaultIndex);

    /*** sat ***/
    void constructCktSat();

    /*** write output ***/
    void initialWriteOutput();
    void writeOutputFile(unsigned faultIndex, unsigned pathIndex, bool patternFind);

    /*** debug ***/
    void printPath();
    void printWirePossibility();

    void dumpCircuit();


private:
    Spec _spec;
    Circuit _circuit;
    Sat _sat;

    /*** launch on capture ***/
    unsigned _duplicateIndex;
    void duplicateCkt();
    void removeDFF();
    void checkConnection();

    /*** sat structure ***/
    void initialzeWireValue();
    void setFaultClause(string faultName, bool faultType);
    void setPropagationClause(Path& path);
    void setTargetClause(string targetName);

    /*** propagation ***/
    vector<Path> _path;
    Path _tmpPath;
    bool _pathFlag;
    bool propagate(unsigned wireId);

    /*** simulation ***/
    void simulate(string faultName, bool faultType, unsigned pathIndex);
    void initializeSimulation(string faultName, bool faultType, unsigned pathIndex);
    void computePossibility();
    bool isFrontier(unsigned gateId);

    /*** write output ***/
    bool findInPath(unsigned wireId, Path &path);
};

#endif // ATPG_H
