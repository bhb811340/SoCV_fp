#include <iostream>
#include <time.h>
#include <stdlib.h>

#include "atpg.h"
#include "pattern.h"
#include "EC.h"

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

    int offset = atpg.circuit(0).numWire();

    //Equivalence checking
    //check each equivalence set
    map< int, vector<int> >::iterator it;
    for( it = atpg.PES.begin(); it != atpg.PES.end(); it++ )
    {
        cout << "Equivalence set "<< it->first <<" checking"<<endl;
        for (unsigned i = 0; i < it->second.size(); ++i){   
            for (unsigned j = 0; j < it->second.size(); ++j){
                EC ec;
              
                ec.setCircuit(atpg, 0);
                ec.setCircuit(atpg, 1);
                
                ec.cutpointAssign(0, i);
                ec.cutpointAssign(1, j);
                

                ec.dfsorder(ec.getCircuit(0), ec.getId(0),ec.getDfsorder(0));
                ec.dfsorder(ec.getCircuit(1), ec.getId(1),ec.getDfsorder(1));
                
                ec.getGateSat(ec.getCircuit(0), ec.getSat(),ec.getDfsorder(0) , 0);
                ec.getGateSat(ec.getCircuit(1), ec.getSat(),ec.getDfsorder(1) , offset);

                ec.solveSat(ec.miter(ec.getSat(),ec.getDfsorderPointer(),offset));
                
                //ec.getPES(it->second); 
            }
        }
        //cout<<"Value: "<< bitset<32>( it->first )<<" / "<< bitset<32>( ~it->first )<<'\t';
        //for( unsigned j = 0; j < it->second.size(); ++j )
         //   cout<< it->second[j] <<' ';
        //cout<<endl;
    }


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
*/
     
    atpg.circuit(0).writeVerilog(argv[3]);
    atpg.circuit(1).writeVerilog(argv[4]);
    //string cmd = "rm *.cnf *.result *.tmp";
    //system(cmd.c_str());

    return 0;
}

