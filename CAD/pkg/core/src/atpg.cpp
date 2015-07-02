#include "atpg.h"

#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <time.h>
#include <iostream>
#include <fstream>
#include <math.h>
#include <bitset>

using namespace std;

Atpg::Atpg()
{

}

Atpg::~Atpg(){
    _path.clear();
}

void Atpg::loc(){
    // ckt structure
    //duplicateCkt();
    //removeDFF();
    //checkConnection();
    //constructCktSat();
    //initialWriteOutput();
}

Pattern* Atpg::RandomGenPattern()
{
    Pattern* pattern = new Pattern();

    srand( time(NULL) );
    for( unsigned i=0 ; i < circuit(0).numWire(); ++i )
    {
        Wire pWire = circuit(0).wire(i);
        if( pWire.type() == "PI" )
            pattern->value.push_back( rand() );
    }
    _pattern.push_back( pattern );


    return pattern;
}

void Atpg::PossibleEqualSet()
{
    for( unsigned i = 0; i < circuit(0).numWire(); ++i )
    {
        Wire w = circuit(0).wire(i);
        int value = w.valueSet();
        map< int, vector<int> >::iterator it;
        it = PES.find( value );

        if( it == PES.end() )
            PES.insert( it, pair<int, vector<int> >( value, vector<int>() ) );
        PES[ value ].push_back( i );
    }

    for( unsigned i = 0; i < circuit(1).numWire(); ++i )
    {
        Wire w = circuit(1).wire(i);
        int value = w.valueSet();
        map< int, vector<int> >::iterator it;
        it = PES.find( value );

        if( it == PES.end() )
            PES.insert( it, pair<int, vector<int> >( value, vector<int>() ) );
        PES[ value ].push_back( i );
    }

    map< int, vector<int> >::iterator it;
    for( it = PES.begin(); it != PES.end(); it++ )
    {
        //cout<<"IN"<<endl;
        cout<<"Value:"<< bitset<32>( it->first )<<'\t';
        for( unsigned j = 0; j < it->second.size(); ++j )
            cout<< it->second[j] <<' ';
        cout<<endl;
    }
}

void Atpg::EC_cutpoint(){

}


/*
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
*/
