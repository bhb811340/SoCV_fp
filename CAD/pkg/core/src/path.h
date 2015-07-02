#ifndef PATH_H
#define PATH_H

#include <vector>

using namespace std;

class Path
{
public:
    Path();

    /*** set ***/
    void addNode(unsigned wireId){ _nodes.push_back(wireId); };
    void deleteNode(unsigned wireId);
    void reset(){ _nodes.clear(); };

    /*** get ***/
    unsigned node(unsigned nodeNumeration){ return _nodes[nodeNumeration]; };
    unsigned length(){ return _nodes.size(); };

    /*** debug ***/
    void print();

private:
    vector<unsigned> _nodes;
};

#endif // PATH_H
