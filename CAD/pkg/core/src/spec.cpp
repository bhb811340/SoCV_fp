#include "spec.h"
#include "util.h"

#include <iostream>
#include <fstream>

using namespace std;

Spec::Spec()
{
}

Spec::~Spec(){
    _fault.clear();
}

bool Spec::readSpec(string specName){
    _specName = specName;
    ifstream _inSpecFile(_specName.c_str(), ios::binary);
    if(!_inSpecFile.is_open()){
        cout<<"Error: "<<_specName<<" cannot be opened.\n";
        return false;
    }

    string tmp;
    string sliced;
    bool cut_flag = false;
    bool faultName = false;
    bool faultTran = false;
    bool target = false;
    Fault newFault;
    while(getline(_inSpecFile, tmp)){
        //tmp = tmp.substr(0,tmp.length()-1);
        sliced = sliceString(&tmp);
        if(sliced[0] != '#'){
            while(sliced.compare("")){
                //parse the content
                if(cut_flag){
                    _cktName = sliced;
                    cut_flag = false;
                    faultName = true;
                }
                else if(faultName){
                    newFault.reset();
                    newFault.setName(sliced);
                    faultTran = true;
                    faultName = false;
                }
                else if(faultTran){
                    if(sliced == "slow-to-rise")
                        newFault.setType(true);
                    else if(sliced == "slow-to-fall")
                        newFault.setType(false);
                    faultTran = false;
                    target = true;
                }
                else if(target){
                    if(sliced == "END"){
                        _fault.push_back(newFault);
                        target = false;
                        faultName = true;
                    }
                    else{
                        newFault.addTarget(sliced);
                    }
                }

                if(!sliced.compare("CUT")){
                    cut_flag = true;
                }

                sliced = sliceString(&tmp);
            }
        }
    }

    _inSpecFile.close();
    return true;
}

Fault& Spec::fault(unsigned faultId){
    if(faultId >= _fault.size()){
        cout<<"Warning: calling an unexisted Fault object. _fault["<<faultId<<"], return the first fault.\n";
        return _fault[0];
    }
    return _fault[faultId];
}

void Spec::print(){
    cout<<"spec name: "<<_specName<<"\n";
    cout<<"circuit name: "<<_cktName<<"\n";
    cout<<"#faults: "<<_fault.size()<<"\n";
    for(unsigned i = 0 ; i < _fault.size() ; ++i){
        _fault[i].print();
    }
}
