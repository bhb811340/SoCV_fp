#include "sat.h"
#include "clause.h"

#include <stdlib.h>
#include <vector>
#include <iostream>
#include <fstream>

using namespace std;

Sat::Sat()
{
}

Sat::~Sat(){
    _cnf.clear();
}

void Sat::addVariable(int wireId){
    if(_wireVarMap.find(wireId) == _wireVarMap.end()){
        _wireVarMap[wireId] = _variable.size();
        _variable.push_back(wireId);
    }
}

void Sat::removeClause(string type){
    vector<unsigned> deleteList;
    for(unsigned i = 0 ; i < _cnf.size() ; ++i){
        if(_cnf[i].type() == type){
            deleteList.push_back(i);
        }
    }

    for(int i = deleteList.size()-1 ; i >= 0 ; i--){
        //cout<<deleteList[i]<<_cnf[deleteList[i]].type()<<"\n";
        _cnf.erase(_cnf.begin()+deleteList[i]);
    }

}

void Sat::changeClauseType(string originalType, string newType){
    for(unsigned i = 0 ; i < _cnf.size() ; ++i){
        if(_cnf[i].type() == originalType){
            _cnf[i].setType(newType);
        }
    }
}

int Sat::wireIdToVariableId(int wireId){
    if(_wireVarMap.find(wireId) == _wireVarMap.end()){
        cout<<"Error: look for an unregistered wire "<<wireId<<" to its corresponding variableId. return -1.\n";
        return -1;
    }
    return _wireVarMap[wireId];
}

void Sat::printWireVarMap(){
    for(map<int,int>::iterator it = _wireVarMap.begin() ; it != _wireVarMap.end() ; ++it){
        cout<<it->first<<" => "<<it->second<<"\n";
    }
}

void Sat::setVarValue(unsigned variableId, bool value){
    if(variableId >= _variableValue.size()){
        cout<<"Error: a variableId out of range when trying to modify the variable value.\n";
        return;
    }

    _variableValue[variableId] = value;
}


void Sat::writeCNF(string fileName){
    ofstream outCNF(fileName.c_str());
    outCNF<<"c "<<fileName<<" for atpg\n";
    outCNF<<"c\n";
    outCNF<<"p cnf "<<_variable.size()-1<<" "<<_cnf.size()<<"\n";

    for(unsigned i = 0 ; i < _cnf.size() ; ++i){
        for(unsigned j = 0 ; j < _cnf[i].length() ; ++j){
            outCNF<<_cnf[i].variable(j)<<" ";
        }
        outCNF<<"0\n";
    }

    outCNF.close();
}

void Sat::readCNF(string fileName){}

bool Sat::readSATResult(string fileName){
    if(_variable.size() != _variableValue.size()){
        cout<<"Warning: the size of value vector does not match that of variable vector.\n";
        _variableValue.clear();
        for(unsigned i = 0 ; i < _variable.size() ; ++i){
            addVarValue();
        }
    }
    ifstream resultFile(fileName.c_str());
    if(!resultFile.is_open()){
        cout<<"Error: file "<<fileName<<" cannot be opened.\n";
        return false;
    }

    string tmp;
    resultFile>>tmp;
    if(tmp == "UNSAT"){
        return false;
    }

    while(resultFile>>tmp){
        //cout<<tmp<<endl;
        int var = atoi(tmp.c_str());
        if(var == 0)
            cout<<"Stage: variable value stored.\n";
        int index = (var < 0)? (-1)*var: var;
        _variableValue[index] = (var < 0)? false: true;
    }

    return true;
}

bool Sat::variableValue(unsigned variableId){
    if(variableId >= _variableValue.size()){
        cout<<"Error: variable "<<variableId<<" does not exist in the variableValue array.\n";
        return _variableValue[0];
    }
    return _variableValue[variableId];
}
