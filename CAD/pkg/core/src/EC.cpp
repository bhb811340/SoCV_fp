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
        else if(ckt.wire(i).type() == "CUT" || ckt.wire(i).type() == "CUT_BAR"){
            s.addVariable(offset+i);
            s.addVarValue();
        }
    }
    int wireVarSize = s.getMapSize(); 
    for(unsigned i = 0 ; i < dfsorder.size() ; ++i){
	    if(ckt.gate(dfsorder[i]).type() != "unknown") {
		    s.addVariable(wireVarSize + offset + dfsorder[i]);
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
                if(ckt.wire(inWire).type() == "PI" || ckt.wire(inWire).type() == "TIE0" || ckt.wire(inWire).type() == "TIE1") //PI or tie0 or tie1
                    clause.addVariable(s.wireIdToVariableId(inWire)); //PI, TIE0, TIE1 - i1 
                else if (ckt.wire(inWire).type() == "CUT" || ckt.wire(inWire).type() == "CUT_BAR"){
                    clause.addVariable(s.wireIdToVariableId(offset+inWire)); // CUT, CUT_BAR - i1
                }
                else 
                    clause.addVariable(s.wireIdToVariableId(wireVarSize + offset + ckt.wire(inWire).preGate())); //other - i1
			    clause.addVariable((-1)*s.wireIdToVariableId(wireVarSize + offset + dfsorder[i])); //o'
                s.addClause(clause);
                clause.resetVariable();
            }
		    for(unsigned j = 0 ; j < ckt.gate(dfsorder[i]).numInWire() ; ++j){
                int inWire = ckt.gate(dfsorder[i]).inWire(j);
                if(ckt.wire(inWire).type() == "PI" || ckt.wire(inWire).type() == "TIE0" || ckt.wire(inWire).type() == "TIE1") //PI or tie0 or tie1
                    clause.addVariable((-1) * s.wireIdToVariableId(inWire));
                else if (ckt.wire(inWire).type() == "CUT" || ckt.wire(inWire).type() == "CUT_BAR"){
                    clause.addVariable((-1)*s.wireIdToVariableId(offset+inWire));
                }
                else
                    clause.addVariable((-1) * s.wireIdToVariableId(wireVarSize + offset + ckt.wire(inWire).preGate()));
		    }
		    clause.addVariable(s.wireIdToVariableId(wireVarSize + offset + dfsorder[i]));
		    s.addClause(clause);
		    clause.resetVariable(); 
            for(unsigned j = 0 ; j < ckt.gate(dfsorder[i]).numInWire() ; ++j){
                int inWire = ckt.gate(dfsorder[i]).inWire(j);
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
		    for(unsigned j = 0 ; j < ckt.gate(dfsorder[i]).numInWire() ; ++j){
		        int inWire = ckt.gate(dfsorder[i]).inWire(j);
                if(ckt.wire(inWire).type() == "PI" || ckt.wire(inWire).type() == "TIE0" || ckt.wire(inWire).type() == "TIE1") //PI or tie0 or tie1
                    clause.addVariable(s.wireIdToVariableId(inWire));
                else if (ckt.wire(inWire).type() == "CUT" || ckt.wire(inWire).type() == "CUT_BAR")
                    clause.addVariable(s.wireIdToVariableId(offset+inWire));
                else
		            clause.addVariable(s.wireIdToVariableId(wireVarSize + offset + ckt.wire(inWire).preGate()));
                clause.addVariable(s.wireIdToVariableId(wireVarSize + offset + dfsorder[i]));
                s.addClause(clause);
                clause.resetVariable();
            }
		    for(unsigned j = 0 ; j < ckt.gate(dfsorder[i]).numInWire() ; ++j){
		        int inWire = ckt.gate(dfsorder[i]).inWire(j);
                if(ckt.wire(inWire).type() == "PI" || ckt.wire(inWire).type() == "TIE0" || ckt.wire(inWire).type() == "TIE1") //PI or tie0 or tie1
                    clause.addVariable((-1) * s.wireIdToVariableId(inWire));
                else if (ckt.wire(inWire).type() == "CUT" || ckt.wire(inWire).type() == "CUT_BAR")
                    clause.addVariable((-1)*s.wireIdToVariableId(offset+inWire));
                else
                    clause.addVariable((-1) * s.wireIdToVariableId(wireVarSize + offset + ckt.wire(inWire).preGate()));
            }
            clause.addVariable((-1)*s.wireIdToVariableId(wireVarSize + offset + dfsorder[i]));
            s.addClause(clause);
            clause.resetVariable();
            for(unsigned j = 0 ; j < ckt.gate(dfsorder[i]).numInWire() ; ++j){
                int inWire = ckt.gate(dfsorder[i]).inWire(j);
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
			for(unsigned j = 0 ; j < ckt.gate(dfsorder[i]).numInWire() ; ++j){
			    int inWire = ckt.gate(dfsorder[i]).inWire(j);
                if(ckt.wire(inWire).type() == "PI" || ckt.wire(inWire).type() == "TIE0" || ckt.wire(inWire).type() == "TIE1") //PI or tie0 or tie1
				    clause.addVariable((-1)*s.wireIdToVariableId(inWire));
                else if (ckt.wire(inWire).type() == "CUT" || ckt.wire(inWire).type() == "CUT_BAR")
                    clause.addVariable((-1)*s.wireIdToVariableId(offset+inWire));
                else
                    clause.addVariable((-1)*s.wireIdToVariableId(wireVarSize + offset + ckt.wire(inWire).preGate()));
                clause.addVariable(s.wireIdToVariableId(wireVarSize + offset + dfsorder[i]));
                s.addClause(clause);
				clause.resetVariable();
			}
			for(unsigned j = 0 ; j < ckt.gate(dfsorder[i]).numInWire() ; ++j){
                int inWire = ckt.gate(dfsorder[i]).inWire(j);
                if(ckt.wire(inWire).type() == "PI" || ckt.wire(inWire).type() == "TIE0" || ckt.wire(inWire).type() == "TIE1") //PI or tie0 or tie1
                    clause.addVariable(s.wireIdToVariableId(inWire));
                else if (ckt.wire(inWire).type() == "CUT" || ckt.wire(inWire).type() == "CUT_BAR")
                    clause.addVariable(s.wireIdToVariableId(offset+inWire));
                else
                    clause.addVariable(s.wireIdToVariableId(wireVarSize + offset + ckt.wire(inWire).preGate()));
			}
			clause.addVariable((-1)*s.wireIdToVariableId(wireVarSize + offset + dfsorder[i]));
            s.addClause(clause);
			clause.resetVariable();
            for(unsigned j = 0 ; j < ckt.gate(dfsorder[i]).numInWire() ; ++j){
                int inWire = ckt.gate(dfsorder[i]).inWire(j);
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
			for(unsigned j = 0 ; j < ckt.gate(dfsorder[i]).numInWire() ; ++j){
			    int inWire = ckt.gate(dfsorder[i]).inWire(j);
                if(ckt.wire(inWire).type() == "PI" || ckt.wire(inWire).type() == "TIE0" || ckt.wire(inWire).type() == "TIE1")
                    clause.addVariable((-1) * s.wireIdToVariableId(inWire));
                else if (ckt.wire(inWire).type() == "CUT" || ckt.wire(inWire).type() == "CUT_BAR")
                    clause.addVariable((-1)*s.wireIdToVariableId(offset+inWire));
                else
                    clause.addVariable((-1) * s.wireIdToVariableId(wireVarSize + offset + ckt.wire(inWire).preGate()));
                clause.addVariable((-1)*s.wireIdToVariableId(wireVarSize + offset + dfsorder[i])); 
                s.addClause(clause);
                clause.resetVariable();
	        }
	        for(unsigned j = 0 ; j < ckt.gate(dfsorder[i]).numInWire() ; ++j){
                int inWire = ckt.gate(dfsorder[i]).inWire(j);
                if(ckt.wire(inWire).type() == "PI" || ckt.wire(inWire).type() == "TIE0" || ckt.wire(inWire).type() == "TIE1") 
                    clause.addVariable(s.wireIdToVariableId(inWire)); 
                else if (ckt.wire(inWire).type() == "CUT" || ckt.wire(inWire).type() == "CUT_BAR")
                    clause.addVariable(s.wireIdToVariableId(offset+inWire));
                else
                    clause.addVariable(s.wireIdToVariableId(wireVarSize + offset + ckt.wire(inWire).preGate())); 
	        }
	        clause.addVariable(s.wireIdToVariableId(wireVarSize + offset + dfsorder[i]));
            s.addClause(clause);
	        clause.resetVariable();
            for(unsigned j = 0 ; j < ckt.gate(dfsorder[i]).numInWire() ; ++j){
                int inWire = ckt.gate(dfsorder[i]).inWire(j);
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
			int inWire = ckt.gate(dfsorder[i]).inWire(0);
            if(ckt.wire(inWire).type() == "PI" || ckt.wire(inWire).type() == "TIE0" || ckt.wire(inWire).type() == "TIE1"){
                clause.addVariable((-1) * s.wireIdToVariableId(inWire));
                clause.addVariable((-1)*s.wireIdToVariableId(wireVarSize + offset + dfsorder[i]));
                s.addClause(clause);
                clause.resetVariable();
                clause.addVariable(s.wireIdToVariableId(inWire)); 
                clause.addVariable(s.wireIdToVariableId(wireVarSize + offset + dfsorder[i]));
                s.addClause(clause);
                clause.resetVariable();
            }
            else if (ckt.wire(inWire).type() == "CUT" || ckt.wire(inWire).type() == "CUT_BAR"){
                clause.addVariable((-1)*s.wireIdToVariableId(offset+inWire));
                clause.addVariable((-1)*s.wireIdToVariableId(wireVarSize + offset + dfsorder[i]));
                s.addClause(clause);
                clause.resetVariable();
                clause.addVariable(s.wireIdToVariableId(offset+inWire));
                clause.addVariable(s.wireIdToVariableId(wireVarSize + offset + dfsorder[i]));
                s.addClause(clause);
                clause.resetVariable();
            }
            else {
                clause.addVariable((-1) * s.wireIdToVariableId(wireVarSize + offset + ckt.wire(inWire).preGate()));
                clause.addVariable((-1)*s.wireIdToVariableId(wireVarSize + offset + dfsorder[i]));
                s.addClause(clause);
                clause.resetVariable();
                clause.addVariable(s.wireIdToVariableId(wireVarSize + offset + ckt.wire(inWire).preGate()));
                clause.addVariable(s.wireIdToVariableId(wireVarSize + offset + dfsorder[i]));
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
            int inWire = ckt.gate(dfsorder[i]).inWire(0);
            if(ckt.wire(inWire).type() == "PI" || ckt.wire(inWire).type() == "TIE0" || ckt.wire(inWire).type() == "TIE1"){
                clause.addVariable(s.wireIdToVariableId(inWire));
                clause.addVariable((-1)*s.wireIdToVariableId(wireVarSize + offset + dfsorder[i]));
                s.addClause(clause);
                clause.resetVariable();
                clause.addVariable((-1)*s.wireIdToVariableId(inWire));
                clause.addVariable(s.wireIdToVariableId(wireVarSize + offset + dfsorder[i]));
                s.addClause(clause);
                clause.resetVariable();
            }
            else if (ckt.wire(inWire).type() == "CUT" || ckt.wire(inWire).type() == "CUT_BAR"){
                clause.addVariable(s.wireIdToVariableId(offset+inWire));
                clause.addVariable((-1)*s.wireIdToVariableId(wireVarSize + offset + dfsorder[i]));
                s.addClause(clause);
                clause.resetVariable();
                clause.addVariable((-1)*s.wireIdToVariableId(offset+inWire));
                clause.addVariable(s.wireIdToVariableId(wireVarSize + offset + dfsorder[i]));
                s.addClause(clause);
                clause.resetVariable();
            }
            else {
                clause.addVariable(s.wireIdToVariableId(wireVarSize + offset + ckt.wire(inWire).preGate()));
                clause.addVariable((-1)*s.wireIdToVariableId(wireVarSize + offset + dfsorder[i]));
                s.addClause(clause);
                clause.resetVariable();
                clause.addVariable((-1)*s.wireIdToVariableId(wireVarSize + offset + ckt.wire(inWire).preGate()));
                clause.addVariable(s.wireIdToVariableId(wireVarSize + offset + dfsorder[i]));
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
            for(unsigned j = 0; j < ckt.gate(dfsorder[i]).numInWire(); ++j){
                int inWire = ckt.gate(dfsorder[i]).inWire(j);
                if(ckt.wire(inWire).type() == "PI" || ckt.wire(inWire).type() == "TIE0" || ckt.wire(inWire).type() == "TIE1") 
                    clause.addVariable((-1) * s.wireIdToVariableId(inWire));
                else if (ckt.wire(inWire).type() == "CUT" || ckt.wire(inWire).type() == "CUT_BAR")
                    clause.addVariable((-1)*s.wireIdToVariableId(offset+inWire));
                else
                    clause.addVariable((-1) * s.wireIdToVariableId(wireVarSize + offset + ckt.wire(inWire).preGate()));
                for(unsigned k = 0; k < ckt.gate(dfsorder[i]).numInWire(); ++k){
                    int inWire2 = ckt.gate(dfsorder[i]).inWire(k);
                    if(k != j){
                        if(ckt.wire(inWire2).type() == "PI" || ckt.wire(inWire2).type() == "TIE0" || ckt.wire(inWire2).type() == "TIE1")
                            clause.addVariable(s.wireIdToVariableId(inWire2));
                        else if (ckt.wire(inWire2).type() == "CUT" || ckt.wire(inWire2).type() == "CUT_BAR")
                            clause.addVariable(s.wireIdToVariableId(offset+inWire2));
                        else
                            clause.addVariable(s.wireIdToVariableId(wireVarSize + offset + ckt.wire(inWire2).preGate()));
                    }
                }
                clause.addVariable(s.wireIdToVariableId(wireVarSize + offset + dfsorder[i]));
                s.addClause(clause);
                clause.resetVariable();
            }
            //(i1+i2+...+io')
            for(unsigned j = 0 ; j < ckt.gate(dfsorder[i]).numInWire() ; ++j){
                int inWire = ckt.gate(dfsorder[i]).inWire(j);
                if(ckt.wire(inWire).type() == "PI" || ckt.wire(inWire).type() == "TIE0" || ckt.wire(inWire).type() == "TIE1")
                    clause.addVariable(s.wireIdToVariableId(inWire));
                else if (ckt.wire(inWire).type() == "CUT" || ckt.wire(inWire).type() == "CUT_BAR")
                    clause.addVariable(s.wireIdToVariableId(offset+inWire));
                else
                    clause.addVariable(s.wireIdToVariableId(wireVarSize + offset + ckt.wire(inWire).preGate()));
            }
            clause.addVariable((-1)*s.wireIdToVariableId(wireVarSize + offset + dfsorder[i])); 
            s.addClause(clause);
            clause.resetVariable();
            //(i1'+i2'+i3'+...io')
            for(unsigned j = 0 ; j < ckt.gate(dfsorder[i]).numInWire() ; ++j){
                int inWire = ckt.gate(dfsorder[i]).inWire(j);
                if(ckt.wire(inWire).type() == "PI" || ckt.wire(inWire).type() == "TIE0" || ckt.wire(inWire).type() == "TIE1")
                    clause.addVariable((-1) * s.wireIdToVariableId(inWire));
                else if (ckt.wire(inWire).type() == "CUT" || ckt.wire(inWire).type() == "CUT_BAR")
                    clause.addVariable((-1)*s.wireIdToVariableId(offset+inWire));
                else
                    clause.addVariable((-1) * s.wireIdToVariableId(wireVarSize + offset + ckt.wire(inWire).preGate()));
            }
            clause.addVariable((-1)*s.wireIdToVariableId(wireVarSize + offset + dfsorder[i]));
            s.addClause(clause);
            clause.resetVariable();
            for(unsigned j = 0 ; j < ckt.gate(dfsorder[i]).numInWire() ; ++j){
                int inWire = ckt.gate(dfsorder[i]).inWire(j);
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
            for(unsigned j = 0; j < ckt.gate(dfsorder[i]).numInWire(); ++j){
                int inWire = ckt.gate(dfsorder[i]).inWire(j);
                if(ckt.wire(inWire).type() == "PI" || ckt.wire(inWire).type() == "TIE0" || ckt.wire(inWire).type() == "TIE1")
                    clause.addVariable((-1) * s.wireIdToVariableId(inWire));
                else if (ckt.wire(inWire).type() == "CUT" || ckt.wire(inWire).type() == "CUT_BAR")
                    clause.addVariable((-1)*s.wireIdToVariableId(offset+inWire));
                else
                    clause.addVariable((-1) * s.wireIdToVariableId(wireVarSize + offset + ckt.wire(inWire).preGate()));
                for(unsigned k = 0; k < ckt.gate(dfsorder[i]).numInWire(); ++k){
                    int inWire2 = ckt.gate(dfsorder[i]).inWire(k);
                    if(k != j){
                        if(ckt.wire(inWire2).type() == "PI" || ckt.wire(inWire2).type() == "TIE0" || ckt.wire(inWire2).type() == "TIE1")
                            clause.addVariable(s.wireIdToVariableId(inWire2));
                        else if (ckt.wire(inWire2).type() == "CUT" || ckt.wire(inWire2).type() == "CUT_BAR")
                            clause.addVariable(s.wireIdToVariableId(offset+inWire2));
                        else
                            clause.addVariable(s.wireIdToVariableId(wireVarSize + offset + ckt.wire(inWire2).preGate()));
                    }
                }
                clause.addVariable((-1)*s.wireIdToVariableId(wireVarSize + offset + dfsorder[i]));
                s.addClause(clause);
                clause.resetVariable();
            }
            //(i1+i2+...+io)
            for(unsigned j = 0 ; j < ckt.gate(dfsorder[i]).numInWire() ; ++j){
                int inWire = ckt.gate(dfsorder[i]).inWire(j);
                if(ckt.wire(inWire).type() == "PI" || ckt.wire(inWire).type() == "TIE0" || ckt.wire(inWire).type() == "TIE1")
                    clause.addVariable(s.wireIdToVariableId(inWire));
                else if (ckt.wire(inWire).type() == "CUT" || ckt.wire(inWire).type() == "CUT_BAR")
                    clause.addVariable(s.wireIdToVariableId(offset+inWire));
                else
                    clause.addVariable(s.wireIdToVariableId(wireVarSize + offset + ckt.wire(inWire).preGate()));
            }
            clause.addVariable(s.wireIdToVariableId(wireVarSize + offset + dfsorder[i]));
            s.addClause(clause);
            clause.resetVariable();
            
            //(i1'+i2'+i3'+...io)
            for(unsigned j = 0 ; j < ckt.gate(dfsorder[i]).numInWire() ; ++j){
                int inWire = ckt.gate(dfsorder[i]).inWire(j);
                if(ckt.wire(inWire).type() == "PI" || ckt.wire(inWire).type() == "TIE0" || ckt.wire(inWire).type() == "TIE1")
                    clause.addVariable((-1) * s.wireIdToVariableId(inWire));
                else if (ckt.wire(inWire).type() == "CUT" || ckt.wire(inWire).type() == "CUT_BAR")
                    clause.addVariable((-1)*s.wireIdToVariableId(offset+inWire));
                else
                    clause.addVariable((-1) * s.wireIdToVariableId(wireVarSize + offset + ckt.wire(inWire).preGate()));
            }
            clause.addVariable(s.wireIdToVariableId(wireVarSize + offset + dfsorder[i]));
            s.addClause(clause);
            clause.resetVariable();
            
            for(unsigned j = 0 ; j < ckt.gate(dfsorder[i]).numInWire() ; ++j){
                int inWire = ckt.gate(dfsorder[i]).inWire(j);
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

Sat EC::miter(Sat s, vector<int>* dfs, int offset) {
	Sat s_miter = s;
	int out1 = dfs[0].back();
	int out2 = dfs[1].back() + offset;
	int miterVar = s_miter.numVariable() + 1;
	Clause clause;
	clause.setType("STRUC");
	clause.addVariable((-1)*s_miter.wireIdToVariableId(out1));
	clause.addVariable(s_miter.wireIdToVariableId(out2));
	clause.addVariable(miterVar);
	s_miter.addClause(clause);
	clause.resetVariable();
	clause.addVariable(s_miter.wireIdToVariableId(out1));
	clause.addVariable((-1)*s_miter.wireIdToVariableId(out2));
	clause.addVariable(miterVar);
	s_miter.addClause(clause);
	clause.resetVariable();
	clause.addVariable(s_miter.wireIdToVariableId(out1));
	clause.addVariable(s_miter.wireIdToVariableId(out2));
	clause.addVariable((-1)*miterVar);
	s_miter.addClause(clause);
	clause.resetVariable();
	clause.addVariable((-1)*s_miter.wireIdToVariableId(out1));
	clause.addVariable((-1)*s_miter.wireIdToVariableId(out2));
	clause.addVariable((-1)*miterVar);
	s_miter.addClause(clause);
	clause.resetVariable();
	clause.addVariable(miterVar);
	s_miter.addClause(clause);
	clause.resetVariable();

	return s_miter;
}

bool EC::solveSat(Sat s) {
    s.writeCNF("EC_cutpoint.cnf");

    string systemCmd = "./MiniSat_v1.14_linux "+ "EC_cutpoint.cnf " + "EC_cutpoint.cnf.result > MiniSAT.log";
	system(systemCmd.c_str());
    if(sat().readSATResult(resultFileName.c_str())){
        cout << "SATISFIABLE\n";
        return true;
    }
    else {
        cout << "UNSATISFIABLE\n";
        return false;
    }
        
}
