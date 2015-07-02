#include "EC.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

void EC::getGateSat(Circuit ckt, Sat s, vector<int> dfsorder, int offset){
    // construct variable map
	s.addVariable(-1);
	s.addVarValue();
    
    for(unsigned i = 0 ; i < ckt.numWire() ; ++i) {
        if(ckt.wire(i).type() == "PI") {
            s.addVariable(i);
            s.addVarValue();
        }
    }
	for(unsigned i = 0 ; i < dfsorder.size() ; ++i){
	    if(ckt.gate(dfsorder[i]).type() != "unknown") {
		    s.addVariable(offset+dfsorder[i]);
		    s.addVarValue();
		}
	}

	// add clause
	Clause clause;
	clause.setType("STRUC");
	for(unsigned i = 0 ; i < dfsorder.size() ; ++i){
	    string gateType = ckt.gate(dfsorder[i]).type();
		clause.resetVariable();
   
        if(gateType == "and"){
		    // AND (i1'+i2'+...+o)(i1+o')(i2+o')(...)
		    for(unsigned j = 0 ; j < ckt.gate(dfsorder[i]).numInWire() ; ++j){
                int inWire = ckt.gate(dfsorder[i]).inWire(j);
                if(ckt.wire(inWire).type() == "PI") //PI
                    clause.addVariable(s.wireIdToVariableId(inWire));
                else 
                    clause.addVariable(s.wireIdToVariableId(offset + ckt.wire(inWire).preGate()));
			    clause.addVariable((-1)*s.wireIdToVariableId(offset + dfsorder[i]));
                s.addClause(clause);
                clause.resetVariable();
            }
		    for(unsigned j = 0 ; j < ckt.gate(dfsorder[i]).numInWire() ; ++j){
                int inWire = ckt.gate(dfsorder[i]).inWire(j);
                if(ckt.wire(inWire).type() == "PI") //PI
                    clause.addVariable((-1) * s.wireIdToVariableId(inWire));
                else
                    clause.addVariable((-1) * s.wireIdToVariableId(offset + ckt.wire(inWire).preGate()));
		    }
		    clause.addVariable(offset+s.wireIdToVariableId(offset + dfsorder[i]));
		    s.addClause(clause);
		    clause.resetVariable();
		}
		else if(gateType == "nand"){
		    // NAND (i1'+i2'+...+o')(i1+o)(i2+o)(...)
		    for(unsigned j = 0 ; j < ckt.gate(dfsorder[i]).numInWire() ; ++j){
		        clause.addVariable(offset+s.wireIdToVariableId(ckt.wire(ckt.gate(dfsorder[i]).inWire(j)).preGate()));
		        clause.addVariable(offset+s.wireIdToVariableId(dfsorder[i]));
                s.addClause(clause);
                clause.resetVariable();
            }
		    for(unsigned j = 0 ; j < ckt.gate(dfsorder[i]).numInWire() ; ++j){
		        clause.addVariable((-1)*s.wireIdToVariableId(ckt.wire(ckt.gate(dfsorder[i]).inWire(j)).preGate())-offset);
            }
            clause.addVariable((-1)*s.wireIdToVariableId(dfsorder[i])-offset);
            s.addClause(clause);
            clause.resetVariable();
        }
		else if(gateType == "or"){
		    // OR (i1+i2+...+o')(i1'+o)(i2'+o)(...)
			for(unsigned j = 0 ; j < ckt.gate(dfsorder[i]).numInWire() ; ++j){
			    clause.addVariable((-1)*s.wireIdToVariableId(ckt.wire(ckt.gate(dfsorder[i]).inWire(j)).preGate())-offset);
				clause.addVariable(offset+s.wireIdToVariableId(dfsorder[i]));
                s.addClause(clause);
				clause.resetVariable();
			}
			for(unsigned j = 0 ; j < ckt.gate(dfsorder[i]).numInWire() ; ++j){
			    clause.addVariable(offset+s.wireIdToVariableId(ckt.wire(ckt.gate(dfsorder[i]).inWire(j)).preGate()));
			}
			clause.addVariable((-1)*s.wireIdToVariableId(dfsorder[i])-offset);
			s.addClause(clause);
			clause.resetVariable();
		}
		else if(gateType == "nor"){
		    // NOR (i1+i2+...+o)(i1'+o')(i2'+o')(...)
			for(unsigned j = 0 ; j < ckt.gate(dfsorder[i]).numInWire() ; ++j){
			    clause.addVariable((-1)*s.wireIdToVariableId(ckt.wire(ckt.gate(dfsorder[i]).inWire(j)).preGate())-offset);
			    clause.addVariable((-1)*s.wireIdToVariableId(dfsorder[i])-offset);
                s.addClause(clause);
                clause.resetVariable();
	        }
	        for(unsigned j = 0 ; j < ckt.gate(dfsorder[i]).numInWire() ; ++j){
	            clause.addVariable(offset+s.wireIdToVariableId(ckt.wire(ckt.gate(dfsorder[i]).inWire(j)).preGate()));
	        }
	        clause.addVariable(offset+s.wireIdToVariableId(dfsorder[i]));
	        s.addClause(clause);
	        clause.resetVariable();
	    }
		else if(gateType == "not"){
		    // NOT (i'+o')(i+o)
			clause.addVariable((-1)*s.wireIdToVariableId(ckt.wire(ckt.gate(dfsorder[i]).inWire(0)).preGate())-offset);
			clause.addVariable((-1)*s.wireIdToVariableId(dfsorder[i])-offset);
			s.addClause(clause);
			clause.resetVariable();
            clause.addVariable(offset+s.wireIdToVariableId(ckt.wire(ckt.gate(dfsorder[i]).inWire(0)).preGate()));
		    clause.addVariable(offset+s.wireIdToVariableId(dfsorder[i]));
		    s.addClause(clause);
		    clause.resetVariable();
		}
        else if(gateType == "buf"){
            // BUF (i+o')(i'+o)
            clause.addVariable(offset+s.wireIdToVariableId(ckt.wire(ckt.gate(dfsorder[i]).inWire(0)).preGate()));
            clause.addVariable((-1)*s.wireIdToVariableId(dfsorder[i])-offset);
            s.addClause(clause);
            clause.resetVariable();
            clause.addVariable((-1)*s.wireIdToVariableId(ckt.wire(ckt.gate(dfsorder[i]).inWire(0)).preGate())-offset);
            clause.addVariable(offset+s.wireIdToVariableId(dfsorder[i]));
            s.addClause(clause);
            clause.resetVariable();
        }

	    else if(gateType == "xor"){
            //XOR (i1'+i2'+i3'+...io')(i1'+i2+i3+...+io)(i1+i2'+...+io)(i1+i2+...+io')
            //(i1'+i2+i3+...+io)(i1+i2'+...+io)
            for(unsigned j = 0; j < ckt.gate(dfsorder[i]).numInWire(); ++j){
                clause.addVariable((-1)*s.wireIdToVariableId(ckt.wire(ckt.gate(dfsorder[i]).inWire(j)).preGate())-offset);
                for(unsigned k = 0; k < ckt.gate(dfsorder[i]).numInWire(); ++k){
                    if(k != j){
                        clause.addVariable(offset+s.wireIdToVariableId(ckt.wire(ckt.gate(dfsorder[i]).inWire(j)).preGate()));
                    }
                }
                clause.addVariable(offset+s.wireIdToVariableId(dfsorder[i]));
                s.addClause(clause);
                clause.resetVariable();
            }
            //(i1+i2+...+io')
            for(unsigned j = 0 ; j < ckt.gate(dfsorder[i]).numInWire() ; ++j){
                clause.addVariable(offset+s.wireIdToVariableId(ckt.wire(ckt.gate(dfsorder[i]).inWire(j)).preGate()));
            }
            clause.addVariable((-1)*s.wireIdToVariableId(dfsorder[i])-offset);
            s.addClause(clause);
            clause.resetVariable();
            //(i1'+i2'+i3'+...io')
            for(unsigned j = 0 ; j < ckt.gate(dfsorder[i]).numInWire() ; ++j){
                clause.addVariable((-1)*s.wireIdToVariableId(ckt.wire(ckt.gate(dfsorder[i]).inWire(j)).preGate())-offset);
            }
            clause.addVariable((-1)*s.wireIdToVariableId(dfsorder[i])-offset);
            s.addClause(clause);
            clause.resetVariable();
        }
    }
}

bool EC::solveSat(Sat s) {
    //sat().writeCNF(spec().cktName() + ".cnf");

    /*string systemCmd = "./MiniSat_v1.14_linux "+ spec().fault(testFaultIndex).name() + "_propagate.cnf " + spec().fault(testFaultIndex).name() + "_propagate.cnf.result > MiniSAT.log";
	system(systemCmd.c_str());*/
return true;
}
