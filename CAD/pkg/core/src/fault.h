#ifndef FAULT_H
#define FAULT_H

#include <string>
#include <vector>

using namespace std;

class Fault
{
public:
    Fault();
    ~Fault();

    /*** modify ***/
    void reset();

    /*** get ***/
    string name(){ return _name; };
    bool type(){ return _type; };
    string target(unsigned targetId){ return _target[targetId]; };
    unsigned numTarget(){ return _target.size(); };

    /*** set ***/
    void setName(string name){ _name = name; };
    void setType(bool type){ _type = type; };
    void addTarget(string target){ _target.push_back(target); };

    /*** debug ***/
    void print();

private:
    string _name;
    bool _type; //slow-to-rise: true, slow-to-fall:false
    vector<string> _target;
};

#endif // FAULT_H
