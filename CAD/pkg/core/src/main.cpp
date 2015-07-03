#include <iostream>
#include <time.h>
#include <stdlib.h>

#include "atpg.h"
#include "pattern.h"

using namespace std;

int main(int argc, char *argv[])
{
    if(argc != 5){
        cout<<"usage: CAD_EC <inDesignA.v> <inDesignB.v> <outDesignA.v> <outDesignB.v>\n";
        return 0;
    }

    Atpg atpg;
    //atpg.spec().readSpec(argv[1]);
    //atpg.spec().print();
    cout<<"Start parsing circuits... "<<endl;
    atpg.circuit(0).readVerilog(argv[1]);
    cout<<"Circuit "<<argv[1]<<"\tOK !!!"<<endl; 
    atpg.circuit(1).readVerilog(argv[2]); 
    cout<<"Circuit "<<argv[2]<<"\tOK !!!"<<endl; 
    //atpg.circuit().print();

    //atpg.loc();

	atpg.circuit(0).setLevel();
    atpg.circuit(1).setLevel();

    Pattern* patternSet = atpg.RandomGenPattern();
	atpg.circuit(0).logicSim( patternSet );
    atpg.circuit(1).logicSim( patternSet );

    atpg.PossibleEqualSet();

    //atpg.circuit(0).dumpCircuit();
    //atpg.circuit(1).dumpCircuit();

    ///// 2nd iteration   
/*
    cout<<endl<<endl;
    atpg.circuit(0).wire(8).setCutPoint();
    atpg.circuit(1).wire(8).setCutPoint();

    Pattern* patternSet0 = atpg.RandomGenPattern();
	atpg.circuit(0).logicSim( patternSet0 );
    atpg.circuit(1).logicSim( patternSet0 );

    atpg.PossibleEqualSet();

    //atpg.circuit(0).dumpCircuit();
    //atpg.circuit(1).dumpCircuit();
//*/
     
    atpg.circuit(0).writeVerilog(argv[3]);
    atpg.circuit(1).writeVerilog(argv[4]);
    //string cmd = "rm *.cnf *.result *.tmp";
    //system(cmd.c_str());

    return 0;
}

