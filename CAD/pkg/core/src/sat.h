#ifndef SAT_H
#define SAT_H

#include "clause.h"

#include <map>
#include <vector>
using namespace std;

class Sat
{
public:
    Sat();
    ~Sat();

    /*** get ***/
    unsigned numClause(){ return _cnf.size(); };
    unsigned numVariable(){ return _variable.size(); };
    int wireIdToVariableId(int wireId);
    int gateIdToVariableId(int gateId);
    bool variableValue(unsigned variableId);
	int getMapSize(){ return _wireVarMap.size(); };

    /*** set ***/
    void addClause(Clause clause){ _cnf.push_back(clause); };
    void addVariable(int wireId);
    void addVarValue(){ _variableValue.push_back(false); };

    void setVarValue(unsigned variableId, bool value);

    void removeClause(string type);
    void changeClauseType(string originalType, string newType);

    /*** write/read cnf file ***/
    void writeCNF(string fileName);
    void readCNF(string fileName);
    bool readSATResult(string fileName);

    /*** debug ***/
    void printWireVarMap();

private:
    vector<Clause> _cnf;
    vector<int> _variable; // notice that _variable[0] is redundant
    map<int, int> _wireVarMap;
    vector<bool> _variableValue;


};

#endif // SAT_H
