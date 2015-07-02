#ifndef CLAUSE_H
#define CLAUSE_H

#include <vector>
#include <string>

using namespace std;

class Clause
{
public:
    Clause();
    ~Clause();

    /*** set ***/
    void addVariable(int variable){ _variable.push_back(variable); };
    void deleteVariable(int variable);
    void setType(string type);
    void resetVariable(){ _variable.clear(); };

    /*** get ***/
    unsigned length(){ return _variable.size(); };
    int variable(unsigned variableNumeration){ return _variable[variableNumeration]; };
    string type();


private:
    vector<int> _variable;
    int _type; // ckt structure STRUC = 1, activate fault ACTIV = 2, propagate fault PROPA = 3, maximize transition TRANS = 4, find solution TEMPO = 5, UNKNO = 0
};

#endif // CLAUSE_H
