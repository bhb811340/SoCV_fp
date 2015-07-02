#include <iostream>
#include <time.h>
#include <stdlib.h>

#include "atpg.h"

using namespace std;

int main(int argc, char *argv[])
{
    if(argc != 2){
        cout<<"usage: CAD_EC  <circuit.v>\n";
        return 0;
    }

    Atpg atpg;
    //atpg.spec().readSpec(argv[1]);
    //atpg.spec().print();
    atpg.circuit().readVerilog(argv[1]);  
    //atpg.circuit().print();

    //atpg.loc();

	atpg.circuit().setLevel();

    //atpg.circuit().printDuplicated();
	//
	atpg.circuit().logicSim();

    atpg.circuit().dumpCircuit();

    //string cmd = "rm *.cnf *.result *.tmp";
    //system(cmd.c_str());

    return 0;
}

