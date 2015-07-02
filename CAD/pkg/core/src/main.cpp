#include <iostream>
#include <time.h>
#include <stdlib.h>

#include "atpg.h"
#include "pattern.h"

using namespace std;

int main(int argc, char *argv[])
{
    if(argc != 3){
        cout<<"usage: CAD_EC <designA.v> <designB.v> \n";
        return 0;
    }

    Atpg atpg;
    //atpg.spec().readSpec(argv[1]);
    //atpg.spec().print();
    atpg.circuit(0).readVerilog(argv[1]); 
    atpg.circuit(1).readVerilog(argv[2]); 
    //atpg.circuit().print();

    //atpg.loc();

	atpg.circuit(0).setLevel();
    atpg.circuit(1).setLevel();

    Pattern* patternSet = atpg.RandomGenPattern();
	atpg.circuit(0).logicSim( patternSet );
    atpg.circuit(1).logicSim( patternSet );

    atpg.circuit(0).dumpCircuit();
    atpg.circuit(1).dumpCircuit();

    //string cmd = "rm *.cnf *.result *.tmp";
    //system(cmd.c_str());

    return 0;
}

