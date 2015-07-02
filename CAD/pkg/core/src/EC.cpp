#include "EC.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <iostream>
#include <fstream>

using namespace std;

void EC::ckt2sat(Circuit ckt, Sat s){
    // construct variable map
	s.addVariable(-1);
	s.addVarValue();
	for(unsigned i = 0 ; i < ckt.numDuplicatedWire() ; ++i){
	    if(ckt.wire(i).type() == "PI" || ckt.wire(i).type() == "PO" || ckt.wire(i).type() == "PPI" || ckt.wire(i).type() == "PPO" || ckt.wire(i).type() == "NORMAL"){
		    s.addVariable(i);
		    s.addVarValue();
		}
	}

	// add clause
	Clause clause;
	clause.setType("STRUC");
	for(unsigned i = 0 ; i < ckt.numDuplicatedGate() ; ++i){
	    string gateType = ckt.gate(i).type();
		clause.resetVariable();
   
        if(gateType == "and"){
		    // AND (i1'+i2'+...+o)(i1+o')(i2+o')(...)
		    for(unsigned j = 0 ; j < ckt.gate(i).numInWire() ; ++j){
		        clause.addVariable(s.wireIdToVariableId(ckt.gate(i).inWire(j)));
			    clause.addVariable((-1)*s.wireIdToVariableId(ckt.gate(i).outWire()));
                s.addClause(clause);
                clause.resetVariable();
            }
		    for(unsigned j = 0 ; j < ckt.gate(i).numInWire() ; ++j){
		        clause.addVariable((-1)*s.wireIdToVariableId(ckt.gate(i).inWire(j)));
		    }
		    clause.addVariable(s.wireIdToVariableId(ckt.gate(i).outWire()));
		    s.addClause(clause);
		    clause.resetVariable();
		}
		else if(gateType == "nand"){
		    // NAND (i1'+i2'+...+o')(i1+o)(i2+o)(...)
		    for(unsigned j = 0 ; j < ckt.gate(i).numInWire() ; ++j){
		        clause.addVariable(s.wireIdToVariableId(ckt.gate(i).inWire(j)));
		        clause.addVariable(s.wireIdToVariableId(ckt.gate(i).outWire()));
                s.addClause(clause);
                clause.resetVariable();
            }
		    for(unsigned j = 0 ; j < ckt.gate(i).numInWire() ; ++j){
		        clause.addVariable((-1)*s.wireIdToVariableId(ckt.gate(i).inWire(j)));
            }
            clause.addVariable((-1)*s.wireIdToVariableId(ckt.gate(i).outWire()));
            s.addClause(clause);
            clause.resetVariable();
        }
		else if(gateType == "or"){
		    // OR (i1+i2+...+o')(i1'+o)(i2'+o)(...)
			for(unsigned j = 0 ; j < ckt.gate(i).numInWire() ; ++j){
			    clause.addVariable((-1)*s.wireIdToVariableId(ckt.gate(i).inWire(j)));
				clause.addVariable(s.wireIdToVariableId(ckt.gate(i).outWire()));
                s.addClause(clause);
				clause.resetVariable();
			}
			for(unsigned j = 0 ; j < ckt.gate(i).numInWire() ; ++j){
			    clause.addVariable(s.wireIdToVariableId(ckt.gate(i).inWire(j)));
			}
			clause.addVariable((-1)*s.wireIdToVariableId(ckt.gate(i).outWire()));
			s.addClause(clause);
			clause.resetVariable();
		}
		else if(gateType == "nor"){
		    // NOR (i1+i2+...+o)(i1'+o')(i2'+o')(...)
			for(unsigned j = 0 ; j < ckt.gate(i).numInWire() ; ++j){
			    clause.addVariable((-1)*s.wireIdToVariableId(ckt.gate(i).inWire(j)));
			    clause.addVariable((-1)*s.wireIdToVariableId(ckt.gate(i).outWire()));
                s.addClause(clause);
                clause.resetVariable();
	        }
	        for(unsigned j = 0 ; j < ckt.gate(i).numInWire() ; ++j){
	            clause.addVariable(s.wireIdToVariableId(ckt.gate(i).inWire(j)));
	        }
	        clause.addVariable(s.wireIdToVariableId(ckt.gate(i).outWire()));
	        s.addClause(clause);
	        clause.resetVariable();
	    }
		else if(gateType == "not"){
		    // NOT (i'+o')(i+o)
			clause.addVariable((-1)*s.wireIdToVariableId(ckt.gate(i).inWire(0)));
			clause.addVariable((-1)*s.wireIdToVariableId(ckt.gate(i).outWire()));
			s.addClause(clause);
			clause.resetVariable();
            clause.addVariable(s.wireIdToVariableId(ckt.gate(i).inWire(0)));
		    clause.addVariable(s.wireIdToVariableId(ckt.gate(i).outWire()));
		    s.addClause(clause);
		    clause.resetVariable();
		}
        else if(gateType == "buf"){
            // BUF (i+o')(i'+o)
            clause.addVariable(s.wireIdToVariableId(ckt.gate(i).inWire(0)));
            clause.addVariable((-1)*s.wireIdToVariableId(ckt.gate(i).outWire()));
            s.addClause(clause);
            clause.resetVariable();
            clause.addVariable((-1)*s.wireIdToVariableId(ckt.gate(i).inWire(0)));
            clause.addVariable(s.wireIdToVariableId(ckt.gate(i).outWire()));
            s.addClause(clause);
            clause.resetVariable();
        }

	    else if(gateType == "xor"){
            //XOR (i1'+i2'+i3'+...io')(i1'+i2+i3+...+io)(i1+i2'+...+io)(i1+i2+...+io')
            //(i1'+i2+i3+...+io)(i1+i2'+...+io)
            for(unsigned j = 0; j < circuit().gate(i).numInWire(); ++j){
                clause.addVariable((-1)*s.wireIdToVariableId(circuit().gate(i).inWire(j)));
                for(unsigned k = 0; k < circuit().gate(i).numInWire(); ++k){
                    if(k != j){
                        clause.addVariable(s.wireIdToVariableId(circuit().gate(i).inWire(j)));
                    }
                }
                clause.addVariable(s.wireIdToVariableId(circuit().gate(i).outWire()));
                s.addClause(clause);
                clause.resetVariable();
            }
            //(i1+i2+...+io')
            for(unsigned j = 0 ; j < circuit().gate(i).numInWire() ; ++j){
                clause.addVariable(s.wireIdToVariableId(circuit().gate(i).inWire(j)));
            }
            clause.addVariable((-1)*s.wireIdToVariableId(circuit().gate(i).outWire()));
            s.addClause(clause);
            clause.resetVariable();
            //(i1'+i2'+i3'+...io')
            for(unsigned j = 0 ; j < circuit().gate(i).numInWire() ; ++j){
                clause.addVariable((-1)*s.wireIdToVariableId(circuit().gate(i).inWire(j)));
            }
            clause.addVariable((-1)*s.wireIdToVariableId(circuit().gate(i).outWire()));
            s.addClause(clause);
            clause.resetVariable();
        }
    }
}

bool EC::solveSat(Sat s) {
    //sat().writeCNF(spec().cktName() + ".cnf");

    /*string systemCmd = "./MiniSat_v1.14_linux "+ spec().fault(testFaultIndex).name() + "_propagate.cnf " + spec().fault(testFaultIndex).name() + "_propagate.cnf.result > MiniSAT.log";
	system(systemCmd.c_str());*/
}
