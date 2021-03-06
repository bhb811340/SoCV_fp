#include <iostream>
#include <time.h>
#include <stdlib.h>
#include <bitset>

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
    atpg.printPES();

    //Equivalence checking
    //check each equivalence set
	Cut c;
    int count = 0;
    int offset = atpg.circuit(0).numWire();
    map< int, vector<int> >::iterator it;
    for( it = atpg.PES.begin(); it != atpg.PES.end(); it++ )
    {
		vector<int> ckt1; // wires for circuit 1
		vector<int> ckt2; // wires for circuit 2
        cout << "Equivalence set "<< count <<" checking: ";
        cout << bitset<32>( it->first ) <<" / "<< bitset<32>( ~it->first )<< endl;
        ++count;
		for (unsigned i = 0; i < it->second.size(); ++i) {
			if(it->second[i] < offset)
				ckt1.push_back(it->second[i]);
			else
				ckt2.push_back(it->second[i] - offset);
		}

        for (unsigned i = 0; i < ckt1.size(); ++i){   
            EC* ec;
            for (unsigned j = 0; j < ckt2.size(); ++j){
                //cout<<"Circuit1-wire"<<i<<" vs "<<"Ciruit2-wire"<<j<<endl;
                ec = new EC();
       
                ec->setCircuit(0, &atpg.circuit(0));
                ec->setCircuit(1, &atpg.circuit(1));
               
                //  cutpointAssign( circuit, cutpoint )
                ec->cutpointAssign( 0, ckt1[i]);
                ec->cutpointAssign( 1, ckt2[j]);

                //  dfsorder( circuit   ,  cutpoint  ,  dfsorder )
                ec->dfsorder( ec->getCircuit(0), ec->getId(0), 0 );
                ec->dfsorder( ec->getCircuit(1), ec->getId(1), 1 );
                //ec->printDFS();
                
                ec->getGateSat(ec->getCircuit(0), ec->getSat(), ec->getDfsorder(0) , 0);
                ec->getGateSat(ec->getCircuit(1), ec->getSat(), ec->getDfsorder(1) , offset);
                //ec->getSat()->writeCNF("CNF.txt");
                
                bool satResult = ec->solveSat(ec->miter(ec->getSat(),ec->getDfsorderPointer(),offset));
                if(satResult == false) {
					c.pushCut(ckt1[i]);
					c.pushCounter(ckt2[j]);
				}
            }
        }

		if (c.getCutSize() > 0 ) {
            //cout<<"CUT SIZE" << c.getCutSize()<<endl;
			for (unsigned i = 0; i < c.getCutSize(); ++i) {
                //cout<< c.getCut(i) <<endl;
                //if not PO and PI modify to CUT
				if(atpg.circuit(0).wire(c.getCut(i)).type().compare("PO") &&
				   atpg.circuit(0).wire(c.getCut(i)).type().compare("PI"))
				    atpg.circuit(0).wire(c.getCut(i)).setType("CUT");
                if(atpg.circuit(1).wire(c.getCounter(i)).type().compare("PO") &&
                   atpg.circuit(1).wire(c.getCounter(i)).type().compare("PI"))
                    atpg.circuit(1).wire(c.getCounter(i)).setType("CUT");
			}
		}
        cout<<endl;

    }

    atpg.circuit(0).dumpCircuit();
    atpg.circuit(1).dumpCircuit();

    ///// 2nd iteration   
/*
    cout<<endl<<endl;

    Pattern* patternSet0 = atpg.RandomGenPattern();
	atpg.circuit(0).logicSim( patternSet0 );
    atpg.circuit(1).logicSim( patternSet0 );

    atpg.PossibleEqualSet();

    //atpg.circuit(0).dumpCircuit();
    //atpg.circuit(1).dumpCircuit();
*/
     
    atpg.circuit(0).writeVerilog(argv[3], c.getCutSize());
    atpg.circuit(1).writeVerilog(argv[4], c.getCounterSize());
    //string cmd = "rm *.cnf *.result *.tmp";
    //system(cmd.c_str());

    return 0;
}

