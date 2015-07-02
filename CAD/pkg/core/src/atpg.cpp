#include "atpg.h"

#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <time.h>
#include <iostream>
#include <fstream>

using namespace std;

void Atpg::dumpCircuit()
{
    for( unsigned i = 0; i< _circuit.numGate() ; ++i )
    {
        Gate tGate = _circuit.gate( i );
        cout << tGate.name() <<endl;

    }
    
}

Atpg::Atpg()
{

}

Atpg::~Atpg(){
    _path.clear();
}

void Atpg::loc(){
    // ckt structure
    duplicateCkt();
    removeDFF();
    checkConnection();
    constructCktSat();
    initialWriteOutput();

    for(unsigned i = 0 ; i < spec().numFault() ; ++i){
        cout<<"Stage: generate pattern for fault "<<spec().fault(i).name()<<".\n";
        initializeData();
        generatePattern(i);
    }

}

void Atpg::initializeData(){
    // called before generating pattern of a fault
    initialzeWireValue();
    _path.clear();
    sat().removeClause("TEMPO");
    sat().removeClause("TRANS");
    sat().removeClause("PROPA");
    sat().removeClause("ACTIV");
    sat().removeClause("UNKNO");

}

bool Atpg::generatePattern(int testFaultIndex){

    sat().writeCNF(spec().cktName() + ".cnf");

    // find propagation path of fault
    srand(time(0));
    while(_path.size() < 15){
        _tmpPath.reset();
        _pathFlag = false;
        propagate(circuit().wireNameToId(spec().fault(testFaultIndex).name())+_duplicateIndex);
    }
    //printPath();

    // set cnf of transition fault
    setFaultClause(spec().fault(testFaultIndex).name(), spec().fault(testFaultIndex).type());
    //sat().writeCNF(spec().cktName() + "_transition.cnf");

    // set cnf of propagating fault
    unsigned pathIndex = 0;
    bool isPropagated = false;
    while(pathIndex<_path.size() && !isPropagated){
        cout<<"iteration "<<pathIndex<<"\n";
        setPropagationClause(_path[pathIndex]);
        sat().writeCNF(spec().fault(testFaultIndex).name() + "_propagate.cnf");

        string systemCmd = "./MiniSat_v1.14_linux "+ spec().fault(testFaultIndex).name() + "_propagate.cnf " + spec().fault(testFaultIndex).name() + "_propagate.cnf.result > MiniSAT.log";
        system(systemCmd.c_str());

        string resultFileName = spec().fault(testFaultIndex).name() + "_propagate.cnf.result";
        if(sat().readSATResult(resultFileName.c_str())){
            cout<<"SATISFIABLE\n";
            isPropagated = true;
        }

        pathIndex++;
    }

    if(!isPropagated){
        writeOutputFile(testFaultIndex, pathIndex-1, false);
        return false;
    }
    else if(isPropagated){
        sat().changeClauseType("TEMPO", "PROPA");

        simulate(spec().fault(testFaultIndex).name(), spec().fault(testFaultIndex).type(), pathIndex-1);
        calTransitionPossibility(testFaultIndex);

        cout<<"Stage: maximize the target transition.\n";
        int consecutiveFailure = 0;
        for(unsigned i = 0 ; i < spec().fault(testFaultIndex).numTarget() ; ++i){
            // try every target from high transition probability to low probability
            cout<<spec().fault(testFaultIndex).target(i)<<" ";
            setTargetClause(spec().fault(testFaultIndex).target(i));
            sat().writeCNF(spec().fault(testFaultIndex).name() + "_target.cnf");

            string resultFileName = spec().fault(testFaultIndex).name() + "_target.cnf.result";
            string cnfFileName = spec().fault(testFaultIndex).name() + "_target.cnf ";
            string systemCmd = "./MiniSat_v1.14_linux "+ cnfFileName + " " + resultFileName + " > MiniSAT.log";
            system(systemCmd.c_str());

            if(sat().readSATResult(resultFileName.c_str())){
                sat().changeClauseType("TEMPO", "TRANS");
                consecutiveFailure = consecutiveFailure/3;
            }
            else{
                sat().removeClause("TEMPO");
                consecutiveFailure++;
            }

            if(consecutiveFailure > 80){
                i = spec().fault(testFaultIndex).numTarget();
            }
        }


        writeOutputFile(testFaultIndex, pathIndex-1, true);
    }

    return true;
}

void Atpg::duplicateCkt(){
    _duplicateIndex = (circuit().numWire() >= circuit().numGate())? circuit().numWire(): circuit().numGate();
    //cout<<_duplicateIndex<<" "<<circuit().numGate()<<" "<<circuit().numWire()<<"\n";

    for(unsigned i = 0 ; i < circuit().numWire() ; ++i){
        if(circuit().wire(i).type() != "UNUSED"){
            circuit().addWire(i+_duplicateIndex, circuit().wire(i).name()+"_dup", circuit().wire(i).type());
            circuit().addWireMap(circuit().wire(i).name()+"_dup", i+_duplicateIndex);
            if(circuit().wire(i).preGate() != -1){
                circuit().wire(i+_duplicateIndex).setPreGate(circuit().wire(i).preGate()+_duplicateIndex);
            }
            for(unsigned j = 0 ; j < circuit().wire(i).numPosGate() ; ++j){
                circuit().wire(i+_duplicateIndex).addPosGate(circuit().wire(i).posGate(j)+_duplicateIndex);
            }
        }
    }
    circuit().setNumDuplicatedWire(circuit().numWire() + _duplicateIndex);

    for(unsigned i = 0 ; i < circuit().numGate() ; ++i){
        if(circuit().gate(i).type() != "unknown"){
            circuit().addGate(i+_duplicateIndex, circuit().gate(i).name()+"_dup", circuit().gate(i).type());
            circuit().addGateMap(circuit().gate(i).name()+"_dup", i+_duplicateIndex);
            if(circuit().gate(i).outWire() != -1){
                circuit().gate(i+_duplicateIndex).setOutWire(circuit().gate(i).outWire()+_duplicateIndex);
            }
            for(unsigned j = 0 ; j < circuit().gate(i).numInWire() ; ++j){
                circuit().gate(i+_duplicateIndex).addInWire(circuit().gate(i).inWire(j)+_duplicateIndex);
            }
        }
    }
    circuit().setNumDuplicatedGate(circuit().numGate() + _duplicateIndex);
}

void Atpg::removeDFF(){
    for(unsigned i = 0 ; i < circuit().numGate() ; ++i){
        if(circuit().gate(i).type() == "dff"){
            //dff
            circuit().wire(circuit().gate(i).outWire()).setType("PPI");
            for(unsigned j = 0 ; j < circuit().wire(circuit().gate(i).outWire()+_duplicateIndex).numPosGate() ; ++j){
                circuit().wire(circuit().gate(i).inWire(0)).addPosGate(circuit().wire(circuit().gate(i).outWire()+_duplicateIndex).posGate(j));
                if(circuit().gate(circuit().wire(circuit().gate(i).outWire()+_duplicateIndex).posGate(j)).type() == "dff"){
                    cout<<"Warning: connect a wire to dff's input, set "<<circuit().wire(circuit().gate(i).inWire(0)).name()<<" to PPO.\n";
                    circuit().wire(circuit().gate(i).inWire(0)).setType("PPO");
                }
            }

            //dff_dup
            circuit().wire(circuit().gate(i+_duplicateIndex).inWire(0)).setType("PPO");
            if(circuit().wire(circuit().gate(i+_duplicateIndex).outWire()).type() == "PPO"){
                cout<<"Warning: inhibit to change "<<circuit().wire(circuit().gate(i+_duplicateIndex).outWire()).name()<<" from PPO to REMOVED.\n";
            }
            else{
                circuit().wire(circuit().gate(i+_duplicateIndex).outWire()).setType("REMOVED");
            }

            for(unsigned j = 0 ; j < circuit().wire(circuit().gate(i+_duplicateIndex).outWire()).numPosGate() ; ++j){
                circuit().gate(circuit().wire(circuit().gate(i+_duplicateIndex).outWire()).posGate(j)).addInWire(circuit().gate(i).inWire(0));
                circuit().gate(circuit().wire(circuit().gate(i+_duplicateIndex).outWire()).posGate(j)).deleteInWire(circuit().gate(i+_duplicateIndex).outWire());
            }
        }
    }
}

void Atpg::checkConnection(){
    // can be deleted
    for(unsigned i = 0 ; i < circuit().numDuplicatedWire() ; ++i){
        string wireType = circuit().wire(i).type();
        string wireName = circuit().wire(i).name();
        if(wireType == "UNUSED"){
            if(circuit().wire(i).preGate() != -1 && circuit().wire(i).numPosGate() != 0){
                cout<<"Error: "<<wireName<<" with "<<wireType<<" has connection\n";
            }
        }
        else if(wireType == "PI"){
            if(circuit().wire(i).preGate() != -1){
                cout<<"Error: "<<wireName<<" with "<<wireType<<" has pregate\n";
                    string preGateName = circuit().gate(circuit().wire(i).preGate()).name();
                    string preGateType = circuit().gate(circuit().wire(i).preGate()).type();
                    cout<<"        "<<preGateName<<" with "<<preGateType<<"\n";
            }
        }
        else if(wireType == "PPI"){

        }
        else if(wireType == "PO"){
            if(circuit().wire(i).numPosGate() != 0){
                cout<<"Error: "<<wireName<<" with "<<wireType<<" has posgate\n";
            }
        }
        else if(wireType == "PPO"){

        }
        else if(wireType == "REMOVED"){}
        else{}
    }

    for(unsigned i = 0 ; i < circuit().numDuplicatedGate() ; ++i){
        string gateType = circuit().gate(i).type();
        string gateName = circuit().gate(i).name();
        if(gateType == "unknown"){
            if(circuit().gate(i).numInWire() != 0 && circuit().gate(i).outWire() != -1){
                cout<<"Error: "<<gateName<<" with "<<gateType<<" has connection\n";
            }
        }
        else if(gateType == "dff"){

        }

        for(unsigned j = 0 ; j < circuit().gate(i).numInWire() ; ++j){
            unsigned inWireId = circuit().gate(i).inWire(j);
            string inWireType = circuit().wire(inWireId).type();
            if(inWireType == "REMOVED" || inWireType == "UNUSED"){
                cout<<"Error: "<<gateName<<" with "<<gateType<<" has REMOVED or UNUSED inwire\n";
            }
        }
    }

}

bool Atpg::propagate(unsigned wireId){
    //cout<<circuit().wire(wireId).name()<<circuit().wire(wireId).type()<<"\n";
    _tmpPath.addNode(wireId);
    if(circuit().wire(wireId).type() == "PPO"){
        _path.push_back(_tmpPath);
        _pathFlag = true;
        return true;
    }
    if(circuit().wire(wireId).type() == "PO"){
        _pathFlag = true;
        return false;
    }

    if(circuit().wire(wireId).type() != "NORMAL")
        cout<<"Error: propagate to a "<<circuit().wire(wireId).type()<<" wire "<<circuit().wire(wireId).name()<<".\n";

    while(!_pathFlag){
        int i = rand() % circuit().wire(wireId).numPosGate();

        if(circuit().gate(circuit().wire(wireId).posGate(i)).type() != "dff" && circuit().gate(circuit().wire(wireId).posGate(i)).type() != "unknown"){
            propagate(circuit().gate(circuit().wire(wireId).posGate(i)).outWire());
            _tmpPath.deleteNode(circuit().gate(circuit().wire(wireId).posGate(i)).outWire());
        }
    }

    /*for(unsigned i = 0 ; i < circuit().wire(wireId).numPosGate() ; ++i){
        if(circuit().gate(circuit().wire(wireId).posGate(i)).type() != "dff" && circuit().gate(circuit().wire(wireId).posGate(i)).type() != "unknown"){
            propagate(circuit().gate(circuit().wire(wireId).posGate(i)).outWire());
            _tmpPath.deleteNode(circuit().gate(circuit().wire(wireId).posGate(i)).outWire());
        }
        if(_pathFlag)
            i = circuit().wire(wireId).numPosGate();
    }*/



}

void Atpg::printPath(){
    cout<<"#############  PATH  #############\n";
    for(unsigned i = 0 ; i < _path.size() ; ++i){
        cout<<"path "<<i<<": ";
        for(unsigned j = 0 ; j < _path[i].length(); ++j){
            cout<<_path[i].node(j)<<" ";
        }
        cout<<"\n";
    }
}

void Atpg::initialzeWireValue(){
    // reset wire value and possibility
    for(unsigned i = 0 ; i < circuit().numDuplicatedWire() ; ++i){
        circuit().wire(i).setValue(-1);
        circuit().wire(i).setPossibility(-1);
    }
}

void Atpg::constructCktSat(){
    // construct variable map
    sat().addVariable(-1);
    sat().addVarValue();
    for(unsigned i = 0 ; i < circuit().numDuplicatedWire() ; ++i){
        if(circuit().wire(i).type() == "PI" || circuit().wire(i).type() == "PO" || circuit().wire(i).type() == "PPI" || circuit().wire(i).type() == "PPO" || circuit().wire(i).type() == "NORMAL"){
            sat().addVariable(i);
            sat().addVarValue();
        }
    }
    //sat().printWireVarMap();

    // add clause
    Clause clause;
    clause.setType("STRUC");
    for(unsigned i = 0 ; i < circuit().numDuplicatedGate() ; ++i){
        string gateType = circuit().gate(i).type();
        clause.resetVariable();

        if(gateType == "and"){
            // AND (i1'+i2'+...+o)(i1+o')(i2+o')(...)
            for(unsigned j = 0 ; j < circuit().gate(i).numInWire() ; ++j){
                clause.addVariable(sat().wireIdToVariableId(circuit().gate(i).inWire(j)));
                clause.addVariable((-1)*sat().wireIdToVariableId(circuit().gate(i).outWire()));

                sat().addClause(clause);
                clause.resetVariable();
            }
            for(unsigned j = 0 ; j < circuit().gate(i).numInWire() ; ++j){
                clause.addVariable((-1)*sat().wireIdToVariableId(circuit().gate(i).inWire(j)));
            }
            clause.addVariable(sat().wireIdToVariableId(circuit().gate(i).outWire()));
            sat().addClause(clause);
            clause.resetVariable();
        }
        else if(gateType == "nand"){
            // NAND (i1'+i2'+...+o')(i1+o)(i2+o)(...)
            for(unsigned j = 0 ; j < circuit().gate(i).numInWire() ; ++j){
                clause.addVariable(sat().wireIdToVariableId(circuit().gate(i).inWire(j)));
                clause.addVariable(sat().wireIdToVariableId(circuit().gate(i).outWire()));

                sat().addClause(clause);
                clause.resetVariable();
            }
            for(unsigned j = 0 ; j < circuit().gate(i).numInWire() ; ++j){
                clause.addVariable((-1)*sat().wireIdToVariableId(circuit().gate(i).inWire(j)));
            }
            clause.addVariable((-1)*sat().wireIdToVariableId(circuit().gate(i).outWire()));
            sat().addClause(clause);
            clause.resetVariable();
        }
        else if(gateType == "or"){
            // OR (i1+i2+...+o')(i1'+o)(i2'+o)(...)
            for(unsigned j = 0 ; j < circuit().gate(i).numInWire() ; ++j){
                clause.addVariable((-1)*sat().wireIdToVariableId(circuit().gate(i).inWire(j)));
                clause.addVariable(sat().wireIdToVariableId(circuit().gate(i).outWire()));

                sat().addClause(clause);
                clause.resetVariable();
            }
            for(unsigned j = 0 ; j < circuit().gate(i).numInWire() ; ++j){
                clause.addVariable(sat().wireIdToVariableId(circuit().gate(i).inWire(j)));
            }
            clause.addVariable((-1)*sat().wireIdToVariableId(circuit().gate(i).outWire()));
            sat().addClause(clause);
            clause.resetVariable();
        }
        else if(gateType == "nor"){
            // NOR (i1+i2+...+o)(i1'+o')(i2'+o')(...)
            for(unsigned j = 0 ; j < circuit().gate(i).numInWire() ; ++j){
                clause.addVariable((-1)*sat().wireIdToVariableId(circuit().gate(i).inWire(j)));
                clause.addVariable((-1)*sat().wireIdToVariableId(circuit().gate(i).outWire()));

                sat().addClause(clause);
                clause.resetVariable();
            }
            for(unsigned j = 0 ; j < circuit().gate(i).numInWire() ; ++j){
                clause.addVariable(sat().wireIdToVariableId(circuit().gate(i).inWire(j)));
            }
            clause.addVariable(sat().wireIdToVariableId(circuit().gate(i).outWire()));
            sat().addClause(clause);
            clause.resetVariable();
        }
        else if(gateType == "not"){
            // NOT (i'+o')(i+o)
            clause.addVariable((-1)*sat().wireIdToVariableId(circuit().gate(i).inWire(0)));
            clause.addVariable((-1)*sat().wireIdToVariableId(circuit().gate(i).outWire()));
            sat().addClause(clause);
            clause.resetVariable();

            clause.addVariable(sat().wireIdToVariableId(circuit().gate(i).inWire(0)));
            clause.addVariable(sat().wireIdToVariableId(circuit().gate(i).outWire()));
            sat().addClause(clause);
            clause.resetVariable();
        }
        else if(gateType == "xor"){
            //XOR (i1'+i2'+i3'+...io')(i1'+i2+i3+...+io)(i1+i2'+...+io)(i1+i2+...+io')
            //(i1'+i2+i3+...+io)(i1+i2'+...+io)
            for(unsigned j = 0; j < circuit().gate(i).numInWire(); ++j){
                clause.addVariable((-1)*sat().wireIdToVariableId(circuit().gate(i).inWire(j)));
                for(unsigned k = 0; k < circuit().gate(i).numInWire(); ++k){
                    if(k != j){
                        clause.addVariable(sat().wireIdToVariableId(circuit().gate(i).inWire(j)));
                    }
                }
                clause.addVariable(sat().wireIdToVariableId(circuit().gate(i).outWire()));
                sat().addClause(clause);
                clause.resetVariable();
            }
            //(i1+i2+...+io')
            for(unsigned j = 0 ; j < circuit().gate(i).numInWire() ; ++j){
                clause.addVariable(sat().wireIdToVariableId(circuit().gate(i).inWire(j)));
            }
            clause.addVariable((-1)*sat().wireIdToVariableId(circuit().gate(i).outWire()));
            sat().addClause(clause);
            clause.resetVariable();
            //(i1'+i2'+i3'+...io')
            for(unsigned j = 0 ; j < circuit().gate(i).numInWire() ; ++j){
                clause.addVariable((-1)*sat().wireIdToVariableId(circuit().gate(i).inWire(j)));
            }
            clause.addVariable((-1)*sat().wireIdToVariableId(circuit().gate(i).outWire()));
            sat().addClause(clause);
            clause.resetVariable();

        }

    }

    // PI should be the same in the two time frames
    for(unsigned i = 0 ; i < circuit().numWire() ; ++i){
        if(circuit().wire(i).type() == "PI"){
            // (a+b')(a'+b)
            clause.resetVariable();

            clause.addVariable(sat().wireIdToVariableId(i));
            clause.addVariable((-1)*sat().wireIdToVariableId(i+_duplicateIndex));
            sat().addClause(clause);
            clause.resetVariable();

            clause.addVariable((-1)*sat().wireIdToVariableId(i));
            clause.addVariable(sat().wireIdToVariableId(i+_duplicateIndex));
            sat().addClause(clause);
            clause.resetVariable();
        }
    }
}

void Atpg::setFaultClause(string faultName, bool faultType){
    int faultWireId = circuit().wireNameToId(faultName);

    Clause clause;
    clause.setType("ACTIV");

    if(faultType){
        // slow-to-rise
        clause.addVariable((-1)*sat().wireIdToVariableId(faultWireId));
        sat().addClause(clause);
        clause.resetVariable();
        clause.addVariable(sat().wireIdToVariableId(faultWireId+_duplicateIndex));
        sat().addClause(clause);
        clause.resetVariable();
    }
    else{
        //slow-to-fall
        clause.addVariable(sat().wireIdToVariableId(faultWireId));
        sat().addClause(clause);
        clause.resetVariable();
        clause.addVariable((-1)*sat().wireIdToVariableId(faultWireId+_duplicateIndex));
        sat().addClause(clause);
        clause.resetVariable();
    }
}

void Atpg::setTargetClause(string targetName){
    unsigned wireId = circuit().wireNameToId(targetName);
    wireId = (wireId >= circuit().numWire())? wireId-_duplicateIndex : wireId;
    int variableId = sat().wireIdToVariableId(wireId);
    int variableId2 = sat().wireIdToVariableId(wireId+_duplicateIndex);
    if(variableId == -1 || variableId2 == -1)
        return;

    Clause clause;
    clause.setType("TEMPO");

    clause.addVariable(variableId);
    clause.addVariable(variableId2);
    sat().addClause(clause);

    clause.resetVariable();

    clause.addVariable((-1)*variableId);
    clause.addVariable((-1)*variableId2);
    sat().addClause(clause);
}

void Atpg::setPropagationClause(Path& path){
    sat().removeClause("TEMPO");
    Clause clause;
    clause.setType("TEMPO");

    for(unsigned i = 0 ; i < path.length()-1 ; ++i){
        // set the "neighbor" inputs to uncontrol value
        int currentWireId = path.node(i);
        int nextWireId = path.node(i+1);

        //cout<<circuit().wire(currentWireId).name()<<" "<<circuit().wire(nextWireId).name()<<":  ";

        string gateType = circuit().gate(circuit().wire(nextWireId).preGate()).type();

        if(gateType == "and" || gateType == "nand"){
            // set neighbor inputs to 1
            for(unsigned j = 0 ; j < circuit().gate(circuit().wire(nextWireId).preGate()).numInWire() ; ++j){
                    //cout<<circuit().gate(circuit().wire(nextWireId).preGate()).name()<<" ";
                    //cout<<circuit().wire(circuit().gate(circuit().wire(nextWireId).preGate()).inWire(j)).name()<<endl;
                if(circuit().gate(circuit().wire(nextWireId).preGate()).inWire(j) != currentWireId){

                    //cout<<circuit().wire(circuit().gate(circuit().wire(nextWireId).preGate()).inWire(j)).name()<<endl;
                    clause.addVariable(sat().wireIdToVariableId(circuit().gate(circuit().wire(nextWireId).preGate()).inWire(j)));
                    sat().addClause(clause);
                    clause.resetVariable();
                }
            }
        }
        else if(gateType == "or" || gateType == "nor"){
            // set neighbor inputs to 0
            for(unsigned j = 0 ; j < circuit().gate(circuit().wire(nextWireId).preGate()).numInWire() ; ++j){
                //cout<<circuit().gate(circuit().wire(nextWireId).preGate()).name()<<" ";
                //cout<<circuit().wire(circuit().gate(circuit().wire(nextWireId).preGate()).inWire(j)).name()<<endl;
                if(circuit().gate(circuit().wire(nextWireId).preGate()).inWire(j) != currentWireId){

                    //cout<<circuit().wire(circuit().gate(circuit().wire(nextWireId).preGate()).inWire(j)).name()<<endl;
                    clause.addVariable((-1)*sat().wireIdToVariableId(circuit().gate(circuit().wire(nextWireId).preGate()).inWire(j)));
                    sat().addClause(clause);
                    clause.resetVariable();
                }
            }

        }

    }
}

void Atpg::simulate(string faultName, bool faultType, unsigned pathIndex){
    initializeSimulation(faultName, faultType, pathIndex);
    computePossibility();
    printWirePossibility();
}

void Atpg::initializeSimulation(string faultName, bool faultType, unsigned pathIndex){
    // fault position
    int faultWireId = circuit().wireNameToId(faultName);
    circuit().wire(faultWireId).setValue(faultType? 0: 1);
    circuit().wire(faultWireId+_duplicateIndex).setValue(faultType? 1: 0);

    // propagation path
    for(unsigned i = 0 ; i < _path[pathIndex].length()-1 ; ++i){
        int currentWireId = _path[pathIndex].node(i);
        int nextWireId = _path[pathIndex].node(i+1);

        string gateType = circuit().gate(circuit().wire(nextWireId).preGate()).type();

        if(gateType == "and" || gateType == "nand"){
            // set neighbor inputs to 1
            for(unsigned j = 0 ; j < circuit().gate(circuit().wire(nextWireId).preGate()).numInWire() ; ++j){
                if(circuit().gate(circuit().wire(nextWireId).preGate()).inWire(j) != currentWireId){
                    circuit().wire(circuit().gate(circuit().wire(nextWireId).preGate()).inWire(j)).setValue(1);
                }
            }
        }
        else if(gateType == "or" || gateType == "nor"){
            // set neighbor inputs to 0
            for(unsigned j = 0 ; j < circuit().gate(circuit().wire(nextWireId).preGate()).numInWire() ; ++j){
                if(circuit().gate(circuit().wire(nextWireId).preGate()).inWire(j) != currentWireId){
                    circuit().wire(circuit().gate(circuit().wire(nextWireId).preGate()).inWire(j)).setValue(0);
                }
            }

        }

    }

}

void Atpg::computePossibility(){
    vector<unsigned> gateToBeComputed;

    //initial PI PPI to P(1) = 0.5 = P(0)
    for(unsigned i = 0 ; i < circuit().numDuplicatedWire(); ++i){
        string wireType = circuit().wire(i).type();
        int wirePossibility = circuit().wire(i).onePossibility();

        if((wireType == "PI" || wireType == "PPI") && wirePossibility == -1){
            //cout<<"set "<<circuit().wire(i).name()<<"\n";
            circuit().wire(i).setPossibility(0.5);
            for(unsigned j = 0 ; j < circuit().wire(i).numPosGate() ; ++j){
                string gateType = circuit().gate(circuit().wire(i).posGate(j)).type();
                if(isFrontier(circuit().wire(i).posGate(j)) && gateType != "unknown" && gateType != "dff"){
                    //cout<<"add "<<circuit().gate(circuit().wire(i).posGate(j)).name()<<"\n";
                    gateToBeComputed.push_back(circuit().wire(i).posGate(j));
                }
            }
        }
        else if(wireType == "PPO" && circuit().gate(circuit().wire(i).preGate()).type() == "dff"){
            unsigned dffOutWireId = circuit().gate(circuit().wire(i).preGate()).outWire();
            circuit().wire(dffOutWireId).setPossibility(0.5);
            for(unsigned j = 0 ; j < circuit().wire(dffOutWireId).numPosGate() ; ++j){
                string gateType = circuit().gate(circuit().wire(dffOutWireId).posGate(j)).type();
                if(isFrontier(circuit().wire(dffOutWireId).posGate(j)) && gateType != "unknown" && gateType != "dff"){
                    //cout<<"add "<<circuit().gate(circuit().wire(i).posGate(j)).name()<<"\n";
                    gateToBeComputed.push_back(circuit().wire(dffOutWireId).posGate(j));
                }
            }
        }
    }

    // calculate the one possibility of all wire through all gates
    while(gateToBeComputed.size() != 0){
        unsigned gateId = gateToBeComputed[0];
        string gateType = circuit().gate(gateId).type();
        //cout<<"compute on"<<circuit().gate(gateId).name()<<"\n";
        if(gateType == "unknown" || gateType == "dff"){
            //cout<<"Warning: try to calculate the one possibility over a unknown or dff type gate "<<circuit().gate(gateId).name()<<".\n";
            gateToBeComputed.erase(gateToBeComputed.begin());
            //cout<<"delete "<<circuit().gate(gateId).name()<<"\n";
        }
        else{
            unsigned outWireId = circuit().gate(gateId).outWire();
            if(circuit().wire(outWireId).onePossibility() != -1){
                // if there is exist possibility, use it first
                gateToBeComputed.erase(gateToBeComputed.begin());
                //cout<<"delete "<<circuit().gate(gateId).name()<<" ";
                for(unsigned i = 0 ; i < circuit().wire(outWireId).numPosGate() ; ++i){
                    string postGateType = circuit().gate(circuit().wire(outWireId).posGate(i)).type();
                    if(isFrontier(circuit().wire(outWireId).posGate(i)) && postGateType != "unknown" && postGateType != "dff"){
                        gateToBeComputed.push_back(circuit().wire(outWireId).posGate(i));
                        //cout<<"add "<<circuit().gate(circuit().wire(outWireId).posGate(i)).name()<<"\n";
                    }
                }
            }
            else{
                double piInZero = 1;
                double piInOne = 1;
                for(unsigned i = 0 ; i < circuit().gate(gateId).numInWire() ; ++i){
                    unsigned inWireId = circuit().gate(gateId).inWire(i);
                    piInZero = piInZero * (1-circuit().wire(inWireId).onePossibility());
                    piInOne = piInOne * circuit().wire(inWireId).onePossibility();
                }

                if(gateType == "and"){
                    circuit().wire(outWireId).setPossibility(piInOne);
                }
                else if(gateType == "nand"){
                    circuit().wire(outWireId).setPossibility(1-piInOne);
                }
                else if(gateType == "or"){
                    circuit().wire(outWireId).setPossibility(1-piInZero);
                }
                else if(gateType == "nor"){
                    circuit().wire(outWireId).setPossibility(piInZero);
                }
                else if(gateType == "not"){
                    circuit().wire(outWireId).setPossibility(piInZero);
                }
                else{
                    cout<<"Error: what the fucking illegal type! "<<circuit().gate(gateId).name()<<" "<<gateType<<"\n";
                }

                for(unsigned i = 0 ; i < circuit().wire(outWireId).numPosGate() ; ++i){
                    string postGateType = circuit().gate(circuit().wire(outWireId).posGate(i)).type();
                    if(isFrontier(circuit().wire(outWireId).posGate(i)) && postGateType != "unknown" && postGateType != "dff"){
                        gateToBeComputed.push_back(circuit().wire(outWireId).posGate(i));
                        //cout<<"add "<<circuit().gate(circuit().wire(outWireId).posGate(i)).name()<<"\n";
                    }
                }

                gateToBeComputed.erase(gateToBeComputed.begin());
                //cout<<"delete "<<circuit().gate(gateId).name()<<"\n";
            }
        }

    }
}

bool Atpg::isFrontier(unsigned gateId){
    bool frontier = true;

    for(unsigned i = 0 ; i < circuit().gate(gateId).numInWire() ; ++i){
        unsigned wireId = circuit().gate(gateId).inWire(i);
        if(circuit().wire(wireId).onePossibility() == -1){
             frontier = false;
        }
    }
    return frontier;
}

void Atpg::printWirePossibility(){
    //cout<<"The One Possibility:\n";
    for(unsigned i = 0 ; i < circuit().numDuplicatedWire() ; ++i){
        //cout<<"Id: "<<i<<" "<<circuit().wire(i).name()<<" "<<circuit().wire(i).type()<<" possibility "<<circuit().wire(i).onePossibility()<<"\n";
        if(circuit().wire(i).type() == "NORMAL" && circuit().wire(i).onePossibility() == -1){


            /*
            unsigned nowWireId = i;
            bool fail = false;
            int timer = 0;
            while(circuit().wire(nowWireId).onePossibility() == -1 && !fail && timer <100){
                unsigned testPreGateId = circuit().wire(nowWireId).preGate();
                fail = true;
                cout<<"Now: "<<circuit().gate(testPreGateId).name()<<" "<<circuit().gate(testPreGateId).type()<<endl;
                for(unsigned k = 0 ; k < circuit().gate(testPreGateId).numInWire() ; ++k){
                    unsigned testInWireId = circuit().gate(testPreGateId).inWire(k);
                    if(circuit().wire(testInWireId).onePossibility() == -1){
                        cout<<"inWire: "<<circuit().wire(testInWireId).name()<<" "<<circuit().wire(testInWireId).type()<<endl;
                        fail = false;
                        cout<<nowWireId<<" "<<testInWireId<<endl;
                        nowWireId = testInWireId;
                    }
                }

                if(fail){
                    cout<<circuit().gate(testPreGateId).name()<<" "<<circuit().gate(testPreGateId).type()<<"\ninWires: ";
                    for(unsigned k = 0 ; k < circuit().gate(testPreGateId).numInWire() ; ++k){
                        unsigned testInWireId = circuit().gate(testPreGateId).inWire(k);
                        cout<<circuit().wire(testInWireId).name()<<" "<<circuit().wire(testInWireId).type()<<endl;
                    }
                    return;
                }
                timer++;
            }

            */


            cout<<"Id: "<<i<<" "<<circuit().wire(i).name()<<" "<<circuit().wire(i).type()<<" possibility "<<circuit().wire(i).onePossibility()<<"\n";
            circuit().wire(i).setPossibility(0.5);
            /*
            unsigned preGateId = circuit().wire(i).preGate();
            cout<<"      preGate: "<<circuit().gate(preGateId).name()<<" "<<circuit().gate(preGateId).type()<<"\n";
            for(unsigned j = 0 ; j < circuit().gate(preGateId).numInWire() ; ++j){
                unsigned inWireId = circuit().gate(preGateId).inWire(j);
                cout<<"      "<<circuit().wire(inWireId).name()<<" "<<circuit().wire(inWireId).type()<<" possibility "<<circuit().wire(inWireId).onePossibility()<<"\n";
            }
            */
        }
    }
}

void Atpg::initialWriteOutput(){
    string fileName = spec().specName().substr(0, spec().specName().find_last_of('.')) + ".output";
    fileName = fileName.substr(fileName.find_last_of("/")+1, fileName.length());
    ofstream outputFile(fileName.c_str());
    outputFile<<"# ";
    outputFile<<spec().specName().substr(spec().specName().find_last_of('/')+1, spec().specName().length())<<"\n";
    outputFile<<"CUT "<<spec().cktName()<<"\n";
    outputFile.close();
}

void Atpg::writeOutputFile(unsigned faultIndex, unsigned pathIndex, bool patternFind){
    string faultName = spec().fault(faultIndex).name();
    bool faultType = spec().fault(faultIndex).type();

    string outFileName = spec().specName().substr(0, spec().specName().find_last_of('.')) + ".tmp";
    outFileName = outFileName.substr(outFileName.find_last_of("/")+1, outFileName.length());

    string inFileName = outFileName.substr(0, outFileName.find_last_of('.')) + ".output";

    ifstream inputFile(inFileName.c_str());
    ofstream outputFile(outFileName.c_str());

    string tmp;
    if(inputFile.is_open()){
        while (getline(inputFile, tmp)) {
            outputFile<<tmp<<"\n";
        }
    }

    outputFile<<"\n"<<faultName<<" ";
    outputFile<<(faultType? "slow-to-rise": "slow-to-fall")<<"\n";

    if(patternFind){
        // test pattern
        for(unsigned i = 0 ; i < circuit().numWire() ; ++i){
            if(circuit().wire(i).type() == "PI" || circuit().wire(i).type() == "PPI"){
                outputFile<<"("<<circuit().wire(i).name()<<", "<<(sat().variableValue(sat().wireIdToVariableId(i))? 1: 0)<<") ";
            }
        }

        // test path
        Path tmpInpath;
        tmpInpath.addNode(circuit().wireNameToId(spec().fault(faultIndex).name()));
        Path maxInPath;
        unsigned times = 0;
        while(maxInPath.length() < tmpInpath.length() || times < 5){

            tmpInpath.reset();
            findInPath(circuit().wireNameToId(spec().fault(faultIndex).name()), tmpInpath);

            if(maxInPath.length() < tmpInpath.length()){
                maxInPath.reset();
                for(unsigned i = 0 ; i < tmpInpath.length() ; ++i){
                    maxInPath.addNode(tmpInpath.node(i));
                }
                times = 0;
            }
            else{
                times++;
            }
        }

        outputFile<<"\nPATH "<<maxInPath.length()+_path[pathIndex].length()-1;
        for(int i = maxInPath.length()-1 ; i > 0 ; --i){
            outputFile<<" "<<circuit().wire(maxInPath.node(i)).name();
        }
        for(unsigned i = 0 ; i < _path[pathIndex].length() ; ++i){
            unsigned tmpWireId = (_path[pathIndex].node(i) >= _duplicateIndex)? _path[pathIndex].node(i)-_duplicateIndex: _path[pathIndex].node(i);
            outputFile<<" "<<circuit().wire(tmpWireId).name();
        }
        outputFile<<"\n";


        // target switching

        outputFile<<"SWITCHING ";
        unsigned numTransition = 0;
        string transitionGates = "";
        for(unsigned i = 0 ; i < spec().fault(faultIndex).numTarget() ; ++i){
            int wireId = circuit().wireNameToId(spec().fault(faultIndex).target(i));
            int nextWireId = wireId + _duplicateIndex;

            if(circuit().wire(wireId).type() == "PPI")
                nextWireId = circuit().gate(circuit().wire(wireId).preGate()).inWire(0);

            if(sat().variableValue(sat().wireIdToVariableId(wireId)) != sat().variableValue(sat().wireIdToVariableId(nextWireId))){
                numTransition++;
                transitionGates.append(spec().fault(faultIndex).target(i)+" ");
            }
        }
        outputFile<<numTransition<<" "<<transitionGates<<"\nEND";

    }
    else{
        outputFile<<"END";
    }

    outputFile.close();
    inputFile.close();

    string systemCmd = "cp "+ outFileName + " " + inFileName;
    system(systemCmd.c_str());
}

bool Atpg::findInPath(unsigned wireId, Path& path){
    if(circuit().wire(wireId).type() == "PI" || circuit().wire(wireId).type() == "PPI"){
        path.addNode(wireId);
        return true;
    }

    int i = rand() % circuit().gate(circuit().wire(wireId).preGate()).numInWire();
    path.addNode(wireId);
    if(findInPath(circuit().gate(circuit().wire(wireId).preGate()).inWire(i), path)){
        return true;
    }
    else{
        cout<<"Error: cannot find a in path.\n";
    }
}

void Atpg::calTransitionPossibility(int testFaultIndex){

    double P;
    vector<double> transPossibility;

    for(unsigned i=0; i<spec().fault(testFaultIndex).numTarget(); ++i){
        string wireName = spec().fault(testFaultIndex).target(i);
        Wire *pWire = &circuit().wire( circuit().wireNameToId(wireName) );
        Wire *pWire_dup = &circuit().wire( circuit().wireNameToId(wireName) + _duplicateIndex);

        P = pWire->onePossibility()*(1 - pWire_dup->onePossibility())   +   (1 - pWire->onePossibility())*pWire_dup->onePossibility() ; //1->0 + 0->1
        transPossibility.push_back(P);
    }

    //sort
    vector<string> targetName;
    for(unsigned i=0; i<spec().fault(testFaultIndex).numTarget(); ++i){
        targetName.push_back( spec().fault(testFaultIndex).target(i) );
    }
    spec().fault(testFaultIndex).reset();
    for(unsigned i=0; i<targetName.size(); ++i){
        double max=-1;
        int loc=0;
        for(unsigned j=0; j<targetName.size(); ++j)
            if( transPossibility[j]>max ) { max = transPossibility[j]; loc=j; }

        transPossibility[loc] = -1;
        spec().fault(testFaultIndex).addTarget(targetName[loc]);
    }
}
