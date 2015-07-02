#include <iostream>
#include <time.h>
#include <stdlib.h>

#include "atpg.h"
#include "pattern.h"

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

    Pattern* patternSet = atpg.circuit().RandomGenPattern();
    //for( int i =0; i< patternSet->value.size(); ++i )
    //    cout<<patternSet->value[i] <<endl;

	atpg.circuit().logicSim( patternSet );

    atpg.circuit().dumpCircuit();

    //string cmd = "rm *.cnf *.result *.tmp";
    //system(cmd.c_str());

    return 0;
}

