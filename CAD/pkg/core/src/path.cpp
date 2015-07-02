#include "path.h"

#include <iostream>

using namespace std;

Path::Path()
{
}

void Path::deleteNode(unsigned wireId){
    int toDelete = -1;
    for(unsigned i = 0 ; i < _nodes.size() ; ++i){
        if(wireId == _nodes[i])
            toDelete = i;
    }

    if(toDelete != -1)
        _nodes.erase(_nodes.begin()+toDelete);
    else
        cout<<"Warning: the node to be deleted is not in the path\n";
}

void Path::print(){
    cout<<"PATH: ";
    for(unsigned i = 0 ; i < _nodes.size() ; ++i){
        cout<<_nodes[i]<<"->";
    }
    cout<<"done.\n";
}
