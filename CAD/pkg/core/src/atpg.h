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

    /*** Gene Pattern ***/
    Pattern*    RandomGenPattern();
    void        PossibleEqualSet();
    map< int, vector<int> > PES; // map< wire_value, vector< Gate_ID > >
    void        printPES();

    /*** get ***/
    unsigned duplicateIndex(){ return _duplicateIndex; };

    /*** get object ***/
    Spec& spec(){ return _spec; };
    Circuit& circuit(int i){ return _circuit[i]; };
    Sat& sat(){ return _sat; };

    /*** launch on capture ***/
    void loc();

    /*** equivalence checking of cut point***/
    void EC_cutpoint();
    
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

private:
    Spec _spec;
    Circuit _circuit[2];
    Sat _sat;
    
    /*** pattern ***/
    vector<Pattern*> _pattern;

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
