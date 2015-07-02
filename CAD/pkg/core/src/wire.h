#ifndef WIRE_H
#define WIRE_H

#include <string>
#include <vector>

using namespace std;

class Wire
{
public:
    Wire();
    ~Wire();

    /*** set ***/
    void setName(string wireName){ _name = wireName; };
    void setType(string type);
    void setValue(int value);
    void setValueSet( int val ) { _valueSet = val; }
    void setPreGate(int gateId);
    void addPosGate(int gateId){ _posGateId.push_back(gateId); };
    void resetPosGate(){ _posGateId.clear(); };
    void setPossibility(double onePossibility){ _onePossibility = onePossibility; };

    /*** get ***/
    string name(){ return _name; };
    int preGate(){ return _preGateId; };
    int posGate(unsigned posGateNumeration);
    unsigned numPosGate(){return _posGateId.size(); };
    string type();
    int value(){ return _value; };
    int valueSet() { return _valueSet; }
    double onePossibility(){ return _onePossibility; };

    /*** debug ***/
    void print();

private:
    string _name;
    int _preGateId;
    vector<int> _posGateId;
    int _type; // PI = 1, PO = 2, PPI = 3, PPO = 4, NORMAL = 5, REMOVED = 6, UNUSED = 0
    int _value; // true = 1, false = 0, undecided = -1
    int _valueSet;
    double _onePossibility;
};

#endif // WIRE_H
