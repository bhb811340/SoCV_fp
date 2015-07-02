#include "fault.h"
#include <iostream>

using namespace std;

Fault::Fault()
{
}

Fault::~Fault(){
    _target.clear();
}

void Fault::reset(){
    _target.clear();
}

void Fault::print(){
    cout<<_name<<" "<<(_type? "slow-to-rise": "slow-to-fall")<<"\n";
    for(unsigned i = 0 ; i < _target.size() ; ++i){
        cout<<_target[i]<<" ";
    }
    cout<<"\n";
}
