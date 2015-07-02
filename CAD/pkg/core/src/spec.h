#ifndef SPEC_H
#define SPEC_H

#include <string>
#include <vector>
#include "fault.h"

using namespace std;

class Spec
{
public:
    Spec();
    ~Spec();

    /*** read spec file ***/
    bool readSpec(string specName);

    /*** get object ***/
    Fault& fault(unsigned faultId);

    /*** get ***/
    string specName(){ return _specName; };
    string cktName(){ return _cktName; };
    unsigned numFault(){ return _fault.size(); };

    /*** set ***/
    void setCktName(string cktName){ _cktName = cktName; };

    /*** debug ***/
    void print();


private:
    string _specName;
    string _cktName;
    vector<Fault> _fault;


};

#endif // SPEC_H
