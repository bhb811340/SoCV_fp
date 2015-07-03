#include "wire.h"
#include <iostream>

Wire::Wire()
    : _type(0),
      _value(-1),
      _preGateId(-1),
      _onePossibility(-1)
{
}

Wire::~Wire(){
    _posGateId.clear();
}

int Wire::posGate(unsigned posGateNumeration){
    if(_posGateId.size() == 0){
        cout<<"Error: the wire "<<_name<<" does not have any posGate. return 0.\n";
        return 0;
    }

    if(posGateNumeration >= _posGateId.size()){
        cout<<"Error: "<<posGateNumeration<<" exceeds the number of posGates in wire "<<_name<<", return the first posGate.\n";
        return _posGateId[0];
    }

    return _posGateId[posGateNumeration];
}

void Wire::setType(string type){
    // PI = 1, PO = 2, CUT = 3, CUT_BAR = 4, TIE0 = 5, TIE1 = 6, NORMAL = 7, REMOVED = 8, UNUSED = 0
    if(type == "PI")
        _type = 1;
    else if(type == "PO")
        _type = 2;
    else if(type == "CUT")
        _type = 3;
    else if(type == "CUT_BAR")
        _type = 4;
    else if(type == "TIE0")
        _type = 5;
    else if(type == "TIE1")
        _type = 6;
    else if(type == "NORMAL")
        _type = 7;
    else if(type == "REMOVED")
        _type = 8;
    else if(type == "UNUSED")
        _type = 0;
    else{
        cout<<"Warning: undefined wire type "<<type<<", set type to -1.\n";
        _type = -1;
    }
}

void Wire::setPreGate(int gateId){
    if(_preGateId != -1){
        cout<<"Warning: the wire "<<_name<<" already has a preGate.\n";
    }
    _preGateId = gateId;
}

void Wire::setValue(int value){
    if(value != 1 && value != 0 && value != -1){
        cout<<"Warning: invalid value to wire, set to -1.\n";
    }
    _value = value;
}

string Wire::type(){
    // PI = 1, PO = 2, CUT = 3, CUT_BAR = 4, TIE0 = 5, TIE1 = 6, NORMAL = 7, REMOVED = 8, UNUSED = 0
    switch(_type){
    case 1 : return "PI";
    case 2 : return "PO";
    case 3 : return "CUT";
    case 4 : return "CUT_BAR";
    case 5 : return "TIE0";
    case 6 : return "TIE1";
    case 7 : return "NORMAL";
    case 8 : return "REMOVED";
    case 0 : return "UNUSED";
    default :
        cout<<"Warning: undefined wire type\n";
        return "UNDEFINED";
    }
}

void Wire::print(){
    cout<<_name<<" "<<type()<<"\n";
    cout<<_preGateId<<" -> ";
    for(unsigned i = 0 ; i < _posGateId.size() ; ++i){
        cout<<_posGateId[i]<<" ";
    }
    cout<<"\n";
}
