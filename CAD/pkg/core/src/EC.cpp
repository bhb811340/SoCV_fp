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
        if(ckt.wire(i).type() == "PI" || ckt.wire(i).type() == "TIE0" || ckt.wire(i).type() == "TIE1") {
            s.addVariable(i);
            s.addVarValue();
        }
        else {
            s.addVariable(offset+i);
            s.addVarValue();
        }
    }

	// add clause
	Clause clause;
	clause.setType("STRUC");
	for(unsigned i = 0 ; i < dfsorder.size() ; ++i){
		Gate g = ckt.gate(ckt.wire(dfsorder[i]).preGate());
	    string gateType = g.type();
		clause.resetVariable();
   
        if(gateType == "and"){
		    // AND (i1'+i2'+...+o)(i1+o')(i2+o')(...)
		    for(unsigned j = 0 ; j < g.numInWire() ; ++j){
                int inWire = g.inWire(j);
                if(ckt.wire(inWire).type() == "PI" || ckt.wire(inWire).type() == "TIE0" || ckt.wire(inWire).type() == "TIE1") //PI or tie0 or tie1
                    clause.addVariable(s.wireIdToVariableId(inWire)); //i1, i2, ... 
                else 
                    clause.addVariable(s.wireIdToVariableId(offset + inWire)); //i1, i2, ...
			    clause.addVariable((-1)*s.wireIdToVariableId(offset + dfsorder[i])); //o'
                s.addClause(clause);
                clause.resetVariable();
            }
		    for(unsigned j = 0 ; j < g.numInWire() ; ++j){
                int inWire = g.inWire(j);
                if(ckt.wire(inWire).type() == "PI" || ckt.wire(inWire).type() == "TIE0" || ckt.wire(inWire).type() == "TIE1") //PI or tie0 or tie1
                    clause.addVariable((-1) * s.wireIdToVariableId(inWire));
                else
                    clause.addVariable((-1) * s.wireIdToVariableId(offset + inWire));
		    }
		    clause.addVariable(s.wireIdToVariableId(offset + dfsorder[i]));
		    s.addClause(clause);
		    clause.resetVariable(); 
            for(unsigned j = 0 ; j < g.numInWire() ; ++j){
                int inWire = g.inWire(j);
                if(ckt.wire(inWire).type() == "TIE0"){
                    clause.addVariable((-1)*s.wireIdToVariableId(inWire));
                    s.addClause(clause);
                    clause.resetVariable();
                }
                else if (ckt.wire(inWire).type() == "TIE1"){
                    clause.addVariable(s.wireIdToVariableId(inWire));
                    s.addClause(clause);
                    clause.resetVariable();
                }
            }
		}
		else if(gateType == "nand"){
		    // NAND (i1'+i2'+...+o')(i1+o)(i2+o)(...)
		    for(unsigned j = 0 ; j < g.numInWire() ; ++j){
		        int inWire = g.inWire(j);
                if(ckt.wire(inWire).type() == "PI" || ckt.wire(inWire).type() == "TIE0" || ckt.wire(inWire).type() == "TIE1") //PI or tie0 or tie1
                    clause.addVariable(s.wireIdToVariableId(inWire));
                else
		            clause.addVariable(s.wireIdToVariableId(offset + inWire));
                clause.addVariable(s.wireIdToVariableId(offset + dfsorder[i]));
                s.addClause(clause);
                clause.resetVariable();
            }
		    for(unsigned j = 0 ; j < g.numInWire() ; ++j){
		        int inWire = g.inWire(j);
                if(ckt.wire(inWire).type() == "PI" || ckt.wire(inWire).type() == "TIE0" || ckt.wire(inWire).type() == "TIE1") //PI or tie0 or tie1
                    clause.addVariable((-1) * s.wireIdToVariableId(inWire));
                else
                    clause.addVariable((-1) * s.wireIdToVariableId(offset + inWire));
            }
            clause.addVariable((-1)*s.wireIdToVariableId(offset + dfsorder[i]));
            s.addClause(clause);
            clause.resetVariable();
            for(unsigned j = 0 ; j < g.numInWire() ; ++j){
                int inWire = g.inWire(j);
                if(ckt.wire(inWire).type() == "TIE0"){
                    clause.addVariable((-1)*s.wireIdToVariableId(inWire));
                    s.addClause(clause);
                    clause.resetVariable();
                }
                else if (ckt.wire(inWire).type() == "TIE1"){
                    clause.addVariable(s.wireIdToVariableId(inWire));
                    s.addClause(clause);
                    clause.resetVariable();
                }
            }
        }
		else if(gateType == "or"){
		    // OR (i1+i2+...+o')(i1'+o)(i2'+o)(...)
			for(unsigned j = 0 ; j < g.numInWire() ; ++j){
			    int inWire = g.inWire(j);
                if(ckt.wire(inWire).type() == "PI" || ckt.wire(inWire).type() == "TIE0" || ckt.wire(inWire).type() == "TIE1") //PI or tie0 or tie1
				    clause.addVariable((-1)*s.wireIdToVariableId(inWire));
                else
                    clause.addVariable((-1)*s.wireIdToVariableId(offset + inWire));
                clause.addVariable(s.wireIdToVariableId(offset + dfsorder[i]));
                s.addClause(clause);
				clause.resetVariable();
			}
			for(unsigned j = 0 ; j < g.numInWire() ; ++j){
                int inWire = g.inWire(j);
                if(ckt.wire(inWire).type() == "PI" || ckt.wire(inWire).type() == "TIE0" || ckt.wire(inWire).type() == "TIE1") //PI or tie0 or tie1
                    clause.addVariable(s.wireIdToVariableId(inWire));
                else
                    clause.addVariable(s.wireIdToVariableId(offset + inWire));
			}
			clause.addVariable((-1)*s.wireIdToVariableId(offset + dfsorder[i]));
            s.addClause(clause);
			clause.resetVariable();
            for(unsigned j = 0 ; j < g.numInWire() ; ++j){
                int inWire = g.inWire(j);
                if(ckt.wire(inWire).type() == "TIE0"){
                    clause.addVariable((-1)*s.wireIdToVariableId(inWire));
                    s.addClause(clause);
                    clause.resetVariable();
                }
                else if (ckt.wire(inWire).type() == "TIE1"){
                    clause.addVariable(s.wireIdToVariableId(inWire));
                    s.addClause(clause);
                    clause.resetVariable();
                }
            }
		}
		else if(gateType == "nor"){
		    // NOR (i1+i2+...+o)(i1'+o')(i2'+o')(...)
			for(unsigned j = 0 ; j < g.numInWire() ; ++j){
			    int inWire = g.inWire(j);
                if(ckt.wire(inWire).type() == "PI" || ckt.wire(inWire).type() == "TIE0" || ckt.wire(inWire).type() == "TIE1")
                    clause.addVariable((-1) * s.wireIdToVariableId(inWire));
                else
                    clause.addVariable((-1) * s.wireIdToVariableId(offset + inWire));
                clause.addVariable((-1)*s.wireIdToVariableId(offset + dfsorder[i])); 
                s.addClause(clause);
                clause.resetVariable();
	        }
	        for(unsigned j = 0 ; j < g.numInWire() ; ++j){
                int inWire = g.inWire(j);
                if(ckt.wire(inWire).type() == "PI" || ckt.wire(inWire).type() == "TIE0" || ckt.wire(inWire).type() == "TIE1") 
                    clause.addVariable(s.wireIdToVariableId(inWire)); 
                else
                    clause.addVariable(s.wireIdToVariableId(offset + inWire)); 
	        }
	        clause.addVariable(s.wireIdToVariableId(offset + dfsorder[i]));
            s.addClause(clause);
	        clause.resetVariable();
            for(unsigned j = 0 ; j < g.numInWire() ; ++j){
                int inWire = g.inWire(j);
                if(ckt.wire(inWire).type() == "TIE0"){
                    clause.addVariable((-1)*s.wireIdToVariableId(inWire));
                    s.addClause(clause);
                    clause.resetVariable();
                }
                else if (ckt.wire(inWire).type() == "TIE1"){
                    clause.addVariable(s.wireIdToVariableId(inWire));
                    s.addClause(clause);
                    clause.resetVariable();
                }
            }
	    }
		else if(gateType == "not"){
		    // NOT (i'+o')(i+o)
			int inWire = g.inWire(0);
            if(ckt.wire(inWire).type() == "PI" || ckt.wire(inWire).type() == "TIE0" || ckt.wire(inWire).type() == "TIE1"){
                clause.addVariable((-1) * s.wireIdToVariableId(inWire));
                clause.addVariable((-1)*s.wireIdToVariableId(offset + dfsorder[i]));
                s.addClause(clause);
                clause.resetVariable();
                clause.addVariable(s.wireIdToVariableId(inWire)); 
                clause.addVariable(s.wireIdToVariableId(offset + dfsorder[i]));
                s.addClause(clause);
                clause.resetVariable();
            }
            else {
                clause.addVariable((-1) * s.wireIdToVariableId(offset + inWire));
                clause.addVariable((-1)*s.wireIdToVariableId(offset + dfsorder[i]));
                s.addClause(clause);
                clause.resetVariable();
                clause.addVariable(s.wireIdToVariableId(offset + inWire));
                clause.addVariable(s.wireIdToVariableId(offset + dfsorder[i]));
                s.addClause(clause);
                clause.resetVariable();
            }
            if(ckt.wire(inWire).type() == "TIE0"){
                clause.addVariable((-1)*s.wireIdToVariableId(inWire));
                s.addClause(clause);
                clause.resetVariable();             }
            else if (ckt.wire(inWire).type() == "TIE1"){
                clause.addVariable(s.wireIdToVariableId(inWire));
                s.addClause(clause);
                clause.resetVariable();
            }
		}
        else if(gateType == "buf"){
            // BUF (i+o')(i'+o)
            int inWire = g.inWire(0);
            if(ckt.wire(inWire).type() == "PI" || ckt.wire(inWire).type() == "TIE0" || ckt.wire(inWire).type() == "TIE1"){
                clause.addVariable(s.wireIdToVariableId(inWire));
                clause.addVariable((-1)*s.wireIdToVariableId(offset + dfsorder[i]));
                s.addClause(clause);
                clause.resetVariable();
                clause.addVariable((-1)*s.wireIdToVariableId(inWire));
                clause.addVariable(s.wireIdToVariableId(offset + dfsorder[i]));
                s.addClause(clause);
                clause.resetVariable();
            }
            else {
                clause.addVariable(s.wireIdToVariableId(offset + inWire));
                clause.addVariable((-1)*s.wireIdToVariableId(offset + dfsorder[i]));
                s.addClause(clause);
                clause.resetVariable();
                clause.addVariable((-1)*s.wireIdToVariableId(offset + inWire));
                clause.addVariable(s.wireIdToVariableId(offset + dfsorder[i]));
                s.addClause(clause);
                clause.resetVariable();
            }
            if(ckt.wire(inWire).type() == "TIE0"){
                clause.addVariable((-1)*s.wireIdToVariableId(inWire));
                s.addClause(clause);        
                clause.resetVariable();
            }
            else if (ckt.wire(inWire).type() == "TIE1"){
                clause.addVariable(s.wireIdToVariableId(inWire));
                s.addClause(clause);
                clause.resetVariable();
            }
        }

	    else if(gateType == "xor"){
            //XOR (i1'+i2'+i3'+...io')(i1'+i2+i3+...+io)(i1+i2'+...+io)(i1+i2+...+io')
            //(i1'+i2+i3+...+io)(i1+i2'+...+io)
            for(unsigned j = 0; j < g.numInWire(); ++j){
                int inWire = g.inWire(j);
                if(ckt.wire(inWire).type() == "PI" || ckt.wire(inWire).type() == "TIE0" || ckt.wire(inWire).type() == "TIE1") 
                    clause.addVariable((-1) * s.wireIdToVariableId(inWire));
                else
                    clause.addVariable((-1) * s.wireIdToVariableId(offset + inWire));
                for(unsigned k = 0; k < g.numInWire(); ++k){
                    int inWire2 = g.inWire(k);
                    if(k != j){
                        if(ckt.wire(inWire2).type() == "PI" || ckt.wire(inWire2).type() == "TIE0" || ckt.wire(inWire2).type() == "TIE1")
                            clause.addVariable(s.wireIdToVariableId(inWire2));
                        else
                            clause.addVariable(s.wireIdToVariableId(offset + inWire2));
                    }
                }
                clause.addVariable(s.wireIdToVariableId(offset + dfsorder[i]));
                s.addClause(clause);
                clause.resetVariable();
            }
            //(i1+i2+...+io')
            for(unsigned j = 0 ; j < g.numInWire() ; ++j){
                int inWire = g.inWire(j);
                if(ckt.wire(inWire).type() == "PI" || ckt.wire(inWire).type() == "TIE0" || ckt.wire(inWire).type() == "TIE1")
                    clause.addVariable(s.wireIdToVariableId(inWire));
                else
                    clause.addVariable(s.wireIdToVariableId(offset + inWire));
            }
            clause.addVariable((-1)*s.wireIdToVariableId(offset + dfsorder[i])); 
            s.addClause(clause);
            clause.resetVariable();
            //(i1'+i2'+i3'+...io')
            for(unsigned j = 0 ; j < g.numInWire() ; ++j){
                int inWire = g.inWire(j);
                if(ckt.wire(inWire).type() == "PI" || ckt.wire(inWire).type() == "TIE0" || ckt.wire(inWire).type() == "TIE1")
                    clause.addVariable((-1) * s.wireIdToVariableId(inWire));
                else
                    clause.addVariable((-1) * s.wireIdToVariableId(offset + inWire));
            }
            clause.addVariable((-1)*s.wireIdToVariableId(offset + dfsorder[i]));
            s.addClause(clause);
            clause.resetVariable();
            for(unsigned j = 0 ; j < g.numInWire() ; ++j){
                int inWire = g.inWire(j);
                if(ckt.wire(inWire).type() == "TIE0"){
                    clause.addVariable((-1)*s.wireIdToVariableId(inWire));
                    s.addClause(clause);
                    clause.resetVariable();
                }
                else if (ckt.wire(inWire).type() == "TIE1"){
                    clause.addVariable(s.wireIdToVariableId(inWire));
                    s.addClause(clause);
                    clause.resetVariable();
                }
            }
        }
        else if(gateType == "xnor"){
            //XNOR (i1'+i2'+i3'+...io)(i1'+i2+i3+...+io')(i1+i2'+...+io')(i1+i2+...+io)
            
            //(i1'+i2+i3+...+io')(i1+i2'+...+io')
            for(unsigned j = 0; j < g.numInWire(); ++j){
                int inWire = g.inWire(j);
                if(ckt.wire(inWire).type() == "PI" || ckt.wire(inWire).type() == "TIE0" || ckt.wire(inWire).type() == "TIE1")
                    clause.addVariable((-1) * s.wireIdToVariableId(inWire));
                else
                    clause.addVariable((-1) * s.wireIdToVariableId(offset + inWire));
                for(unsigned k = 0; k < g.numInWire(); ++k){
                    int inWire2 = g.inWire(k);
                    if(k != j){
                        if(ckt.wire(inWire2).type() == "PI" || ckt.wire(inWire2).type() == "TIE0" || ckt.wire(inWire2).type() == "TIE1")
                            clause.addVariable(s.wireIdToVariableId(inWire2));
                        else
                            clause.addVariable(s.wireIdToVariableId(offset + inWire2));
                    }
                }
                clause.addVariable((-1)*s.wireIdToVariableId(offset + dfsorder[i]));
                s.addClause(clause);
                clause.resetVariable();
            }
            //(i1+i2+...+io)
            for(unsigned j = 0 ; j < g.numInWire() ; ++j){
                int inWire = g.inWire(j);
                if(ckt.wire(inWire).type() == "PI" || ckt.wire(inWire).type() == "TIE0" || ckt.wire(inWire).type() == "TIE1")
                    clause.addVariable(s.wireIdToVariableId(inWire));
                else
                    clause.addVariable(s.wireIdToVariableId(offset + inWire));
            }
            clause.addVariable(s.wireIdToVariableId(offset + dfsorder[i]));
            s.addClause(clause);
            clause.resetVariable();
            
            //(i1'+i2'+i3'+...io)
            for(unsigned j = 0 ; j < g.numInWire() ; ++j){
                int inWire = g.inWire(j);
                if(ckt.wire(inWire).type() == "PI" || ckt.wire(inWire).type() == "TIE0" || ckt.wire(inWire).type() == "TIE1")
                    clause.addVariable((-1) * s.wireIdToVariableId(inWire));
                else
                    clause.addVariable((-1) * s.wireIdToVariableId(offset + inWire));
            }
            clause.addVariable(s.wireIdToVariableId(offset + dfsorder[i]));
            s.addClause(clause);
            clause.resetVariable();
            
            for(unsigned j = 0 ; j < g.numInWire() ; ++j){
                int inWire = g.inWire(j);
                if(ckt.wire(inWire).type() == "TIE0"){
                    clause.addVariable((-1)*s.wireIdToVariableId(inWire));
                    s.addClause(clause);
                    clause.resetVariable();
                }
                else if (ckt.wire(inWire).type() == "TIE1"){
                    clause.addVariable(s.wireIdToVariableId(inWire));
                    s.addClause(clause);
                    clause.resetVariable();
                }
            }
        }
    }
}

Sat EC::miter(Sat& s, vector<int>* dfs, int offset) {
	
	int out1 = dfs[0].back();
	int out2 = dfs[1].back() + offset;
	int miterVar = s.numVariable() + 1;
	Clause clause;
	clause.setType("STRUC");
	clause.addVariable((-1)*s.wireIdToVariableId(out1));
	clause.addVariable(s.wireIdToVariableId(out2));
	clause.addVariable(miterVar);
	s.addClause(clause);
	clause.resetVariable();
	clause.addVariable(s.wireIdToVariableId(out1));
	clause.addVariable((-1)*s.wireIdToVariableId(out2));
	clause.addVariable(miterVar);
	s.addClause(clause);
	clause.resetVariable();
	clause.addVariable(s.wireIdToVariableId(out1));
	clause.addVariable(s.wireIdToVariableId(out2));
	clause.addVariable((-1)*miterVar);
	s.addClause(clause);
	clause.resetVariable();
	clause.addVariable((-1)*s.wireIdToVariableId(out1));
	clause.addVariable((-1)*s.wireIdToVariableId(out2));
	clause.addVariable((-1)*miterVar);
	s.addClause(clause);
	clause.resetVariable();
	clause.addVariable(miterVar);
	s.addClause(clause);
	clause.resetVariable();

	return s;
}

bool EC::solveSat(Sat s) {
    s.writeCNF("EC_cutpoint.cnf");

    string systemCmd = "./MiniSat_v1.14_linux EC_cutpoint.cnf EC_cutpoint.cnf.result > MiniSAT.log";
	system(systemCmd.c_str());
    string resultFileName = "EC_cutpoint.cnf.result";
    if(s.readSATResult(resultFileName.c_str())){
        cout << "SATISFIABLE\n";
        return true;
    }
    else {
        cout << "UNSATISFIABLE\n";
        return false;
    }
        
}
