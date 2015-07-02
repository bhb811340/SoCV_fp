#include "clause.h"
#include <iostream>

using namespace std;

Clause::Clause()
    : _type(-1)
{
}

Clause::~Clause(){
    _variable.clear();
}

void Clause::deleteVariable(int variable){
    int toDelete = -1;
    for(unsigned i = 0 ; i < _variable.size() ; ++i){
        if(_variable[i] == variable){
            toDelete = i;
            i = _variable.size();
        }
    }

    if(toDelete != -1)
        _variable.erase(_variable.begin() + toDelete);
    else
        cout<<"Warning: try to delete a variable that does not exist.\n";
}

void Clause::setType(string type){
    string clauseType[5]={"STRUC", "ACTIV", "PROPA", "TRANS", "TEMPO"};
    bool flag = false;
    for(unsigned i = 0 ; i < 5 ; ++i){
        if(type == clauseType[i]){
            _type = i+1;
            flag = true;
        }
    }

    if(!flag)
        cout<<"Warning: try to set the clause to an unknown type, set to 0.\n";
}

string Clause::type(){
    if(_type == 0)
        return "UNKNO";
    string clauseType[5]={"STRUC", "ACTIV", "PROPA", "TRANS", "TEMPO"};
    return clauseType[_type-1];
}
