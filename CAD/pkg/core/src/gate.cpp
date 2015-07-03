#include "gate.h"
#include <iostream>
#include <string>

Gate::Gate()
    : 
      _outWireId(-1), 
      _type(0) 
{
}

Gate::~Gate(){
    _inWireId.clear();
}

void Gate::setType(string type){
    // not = 1, buf = 2, and = 3, nand = 4, or = 5, nor = 6, xor = 7, xnor = 8, unknown = 0 
    // not = 1, and = 2, nand = 3, or = 4, nor = 5, dff = 6, unknown = 0
    string TYPE[8] = {"not", "buf", "and", "nand", "or", "nor", "xor", "xnor"};
    bool find = false;
    for(int i = 0 ; i < 8 ; ++i){
        if(type == TYPE[i]){
            _type = i+1;
            find = true;
        }
    }
    if(!find){
        cout<<"Warning: unknown type of gate "<<type<<" set the type of gate to unknown.\n";
        _type = 0;
    }
}

void Gate::setOutWire(int wireId){
    if(_outWireId != -1){
        cout<<"Warning: the gate "<<_name<<" already has a outWire.\n";
    }
    _outWireId = wireId;
}

int Gate::inWire(unsigned inWireNumeration){
    if(_inWireId.size() == 0){
        cout<<"Error: the gate "<<_name<<" does not have any inWire. return 0.\n";
        return 0;
    }
    if(inWireNumeration >= _inWireId.size()){
        cout<<"Error: "<<inWireNumeration<<" exceeds the number of inWires in wire "<<_name<<", return the first inWire.\n";
        return _inWireId[0];
    }

    return _inWireId[inWireNumeration];
}

string Gate::type(){
    // not = 1, and = 2, nand = 3, or = 4, nor = 5, dff = 6, unknown = 0
    //string TYPE[7] = {"unknown", "not", "and", "nand", "or", "nor", "dff"};
    // not = 1, buf = 2, and = 3, nand = 4, or = 5, nor = 6, xor = 7, xnor = 8, unknown = 0 
    string TYPE[9] = {"unknown", "not", "buf", "and", "nand", "or", "nor", "xor", "xnor"};
    return TYPE[_type];
}

void Gate::print(){
    cout<<_name<<"\ttype:"<<type()<<"\tlevel"<<level()<<endl;
    for(unsigned i = 0 ; i < _inWireId.size() ; ++i){
        cout<<'\t'<<_inWireId[i]<<" ";
    }
    cout<<" -> "<<_outWireId<<"\n";
}

void Gate::deleteInWire(int wireId){
    int toDelete = -1;
    for(unsigned i = 0 ; i < numInWire() ; ++i){
        if(_inWireId[i] == wireId)
            toDelete = i;
    }
    if(toDelete == -1){
        cout<<"Warning: to delete an inWire "<<wireId<<" that is not in the list of gate "<<_name<<".\n";
        return;
    }
    _inWireId.erase(_inWireId.begin()+toDelete);
}
