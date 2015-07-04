#include <iostream>
#include <time.h>
#include <stdlib.h>

#include "atpg.h"
#include "pattern.h"
#include "EC.h"
#include "cut.h"

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
	Cut c;
    map< int, vector<int> >::iterator it;
    for( it = atpg.PES.begin(); it != atpg.PES.end(); it++ )
    {
		vector<int> ckt1;
		vector<int> ckt2;
        cout << "Equivalence set "<< it->first <<" checking"<<endl;
		for (unsigned i = 0; i < it->second.size(); ++i) {
			if(it->second[i] < offset)
				ckt1.push_back(it->second[i]);
			else
				ckt2.push_back(it->second[i] - offset);
		}

        for (unsigned i = 0; i < ckt1.size(); ++i){   
            EC* ec;
            for (unsigned j = 0; j < ckt2.size(); ++j){
                ec = new EC();
       
                ec->setCircuit(0, &atpg.circuit(0));
                ec->setCircuit(1, &atpg.circuit(1));
                
                ec->cutpointAssign(0, ckt1[i]);
                ec->cutpointAssign(1, ckt2[j]);

                ec->dfsorder(ec->getCircuit(0), ec->getId(0), 0 );
                ec->dfsorder(ec->getCircuit(1), ec->getId(1), 1 );
                
                ec->getGateSat(ec->getCircuit(0), ec->getSat(), ec->getDfsorder(0) , 0);
                ec->getGateSat(ec->getCircuit(1), ec->getSat(), ec->getDfsorder(1) , offset);
                
                bool satResult = ec->solveSat(ec->miter(ec->getSat(),ec->getDfsorderPointer(),offset));
                if(satResult == false) {
					c.pushCut(ckt1[i]);
					c.pushCounter(ckt2[j]);
				}
            }
        }

		if (c.getCutSize() > 0 ) {
			for (unsigned i = 0; i < c.getCutSize(); ++i) {
				atpg.circuit(0).wire(c.getCut(i)).setCutPoint();
				atpg.circuit(0).wire(c.getCut(i)).setType("CUT");
			}
		}
		else 
			cout << "No EC cutpoints found!\n";

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

