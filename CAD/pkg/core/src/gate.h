#ifndef GATE_H
#define GATE_H

#include <string>
#include <vector>

using namespace std;

class Gate
{
public:
    Gate();
    ~Gate();

    /*** set ***/
    void setName(string name){ _name = name; };
    void setType(string type);
    void setLevel(int level) { _level = level; }
    void addInWire(int wireId){ _inWireId.push_back(wireId); };
    void setOutWire(int wireId);
    void resetInWire(){ _inWireId.clear(); };
    void deleteInWire(int wireId);

    /*** get ***/
    string name(){ return _name; };
    int outWire(){ return _outWireId; };
    int inWire(unsigned inWireNumeration);
    unsigned numInWire(){ return _inWireId.size(); };
    string type();
    int level() { return _level; }
	int typeID(){return _type;};

    /*** debug ***/
    void print();

private:
    string _name;
    vector<int> _inWireId;
    int _outWireId;

    // not = 1, buf = 2, and = 3, nand = 4, or = 5, nor = 6, xor = 7, xnor = 8, unknown = 0
    int _type;
    int _level;
};

#endif // GATE_H
