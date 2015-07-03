#include "circuit.h"
#include "util.h"

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <queue>
#include <algorithm>
#include <cassert>
#include <bitset>
#include <time.h>
#include <iomanip>

using namespace std;

Circuit::Circuit()
{
    _gate = new Gate[buffer_length];
    _wire = new Wire[buffer_length];
    _numGate = 0;
    _numWire = 0;
}

Circuit::~Circuit(){
    delete [] _gate;
    delete [] _wire;
}

bool Circuit::readVerilog(string fileName){
    ifstream inVeriFile(fileName.c_str(), ios::binary);
    if(!inVeriFile.is_open()){
        cout<<"Error: "<<fileName<<" cannot be opened.\n";
        return false;
    }

    // add 1'b0 to wire[0]
    _wire[_numWire].setType("TIE0");
    _wire[_numWire].setName("1'b0");
    _wire[_numWire].setValue(0);
    _wireMap["1'b0"] = _numWire;
    _numWire++;
    // add 1'b1 to wire[1]
    _wire[_numWire].setType("TIE1");
    _wire[_numWire].setName("1'b1");
    _wire[_numWire].setValue(1);
    _wireMap["1'b1"] = _numWire;
    _numWire++;

    string tmp;
    string sliced;  //This is for gate type
    string token = "WAIT";
    bool out_flag = false;
    //bool name_flag = false;
    while(getline(inVeriFile, tmp) && token != "END"){
        if(tmp.find_first_not_of(' ') != string::npos){
            tmp = tmp.substr(tmp.find_first_not_of(' '), tmp.length());
            if(tmp[0] != '/' && tmp[1] != '/'){
                //cout<<tmp<<endl;
                sliced = sliceVerilog(&tmp);
                //cout<<"slice = " << sliced <<endl;
                //cout<<"slice = " << sliced.compare("") <<endl;
                while(sliced.compare("") && token != "END"){
                    //cout<<"abcdkd = "<<sliced<<" "<<token<<endl;
                    //cout<<sliced<<endl;

                    if(!token.compare("CKTNAME")){
                        _name = sliced;
                        token = "WAIT";
                    }
                    else if(!token.compare("INPUT")){
                        if(sliced.compare(";")){
                            if(sliced.compare("GND") && sliced.compare("VDD") && sliced.compare("CK")){
                                _wire[_numWire].setType("PI"); // PI = 1, PO = 2, PPI = 3, PPO = 4, NORMAL = 5, UNUSED = 0
                                _wire[_numWire].setName(sliced);
                                _wireMap[sliced] = _numWire;

                                _numWire++;
                            }
                        }
                        else{
                            //cout<<"end of input\n";
                            token = "WAIT";
                        }
                    }
                    else if(!token.compare("OUTPUT")){
                        if(sliced.compare(";")){
                            _wire[_numWire].setType("PO");
                            _wire[_numWire].setName(sliced);
                            _wireMap[sliced] = _numWire;

                            _numWire++;
                        }
                        else{
                            //cout<<"end of output\n";
                            token = "WAIT";
                        }
                    }
                    else if(!token.compare("WIRE")){
                        if(sliced.compare(";")){
                            if(_wireMap.find(sliced) == _wireMap.end()){
                            _wire[_numWire].setType("NORMAL");
                            _wire[_numWire].setName(sliced);
                            _wireMap[sliced] = _numWire;

                            _numWire++;
                            }
                        }
                        else{
                            //cout<<"end of wire\n";
                            token = "WAIT_GATE";
                        }
                    }
                    else if(!token.compare("NOT")){
                        if(sliced.compare(";")){
                        //add an not gate
                            if(out_flag){
                                string s;
                                stringstream ss(s);
                                ss << _numGate;
                                string gateName = "Gate " + ss.str();
                                _gate[_numGate].setName(gateName);
                                _gate[_numGate].setType("not");
                                _gate[_numGate].setOutWire(_wireMap.find(sliced)->second);
                                out_flag = false;
                            }
                            else{
                                _gate[_numGate].addInWire(_wireMap.find(sliced)->second);
                            }
                        }
                        else{
                            token = "WAIT_GATE";
                            _numGate++;
                        }
                    }
                    else if(!token.compare("BUF")){
                        if(sliced.compare(";")){
                        //add an buf gate
                            if(out_flag){
                                string s;
                                stringstream ss(s);
                                ss << _numGate;
                                string gateName = "Gate " + ss.str();
                                _gate[_numGate].setName(gateName);
                                _gate[_numGate].setType("buf");
                                _gate[_numGate].setOutWire(_wireMap.find(sliced)->second);
                                out_flag = false;
                            }
                            else{
                                _gate[_numGate].addInWire(_wireMap.find(sliced)->second);
                            }
                        }
                        else{
                            token = "WAIT_GATE";
                            _numGate++;
                        }
                    }
                    else if(!token.compare("AND")){
                        if(sliced.compare(";")){
                        //add an and gate
                            if(out_flag){
                                string s;
                                stringstream ss(s);
                                ss << _numGate;
                                string gateName = "Gate " + ss.str();
                                _gate[_numGate].setName(gateName);
                                _gate[_numGate].setType("and");
                                _gate[_numGate].setOutWire(_wireMap.find(sliced)->second);
                                out_flag = false;
                            }
                            else{
                                _gate[_numGate].addInWire(_wireMap.find(sliced)->second);
                            }
                        }
                        else{
                            token = "WAIT_GATE";
                            _numGate++;
                        }
                    }
                    else if(!token.compare("NAND")){
                        if(sliced.compare(";")){
                        //add an nand gate
                            if(out_flag){
                                string s;
                                stringstream ss(s);
                                ss << _numGate;
                                string gateName = "Gate " + ss.str();
                                _gate[_numGate].setName(gateName);
                                _gate[_numGate].setType("nand");
                                _gate[_numGate].setOutWire(_wireMap.find(sliced)->second);
                                out_flag = false;
                            }
                            else{
                                _gate[_numGate].addInWire(_wireMap.find(sliced)->second);
                            }
                        }
                        else{
                            token = "WAIT_GATE";
                            _numGate++;
                        }
                    }
                    else if(!token.compare("OR")){
                        if(sliced.compare(";")){
                        //add an or gate
                            if(out_flag){
                                string s;
                                stringstream ss(s);
                                ss << _numGate;
                                string gateName = "Gate " + ss.str();
                                _gate[_numGate].setName(gateName);
                                _gate[_numGate].setType("or");
                                _gate[_numGate].setOutWire(_wireMap.find(sliced)->second);
                                out_flag = false;
                            }
                            else{
                                _gate[_numGate].addInWire(_wireMap.find(sliced)->second);
                            }
                        }
                        else{
                            token = "WAIT_GATE";
                            _numGate++;
                        }
                    }
                    else if(!token.compare("NOR")){
                        if(sliced.compare(";")){
                        //add an nor gate
                            if(out_flag){
                                string s;
                                stringstream ss(s);
                                ss << _numGate;
                                string gateName = "Gate " + ss.str();
                                _gate[_numGate].setName(gateName);
                                _gate[_numGate].setType("nor");
                                _gate[_numGate].setOutWire(_wireMap.find(sliced)->second);
                                out_flag = false;
                            }
                            else{
                                _gate[_numGate].addInWire(_wireMap.find(sliced)->second);
                            }
                        }
                        else{
                            token = "WAIT_GATE";
                            _numGate++;
                        }
                    }
                    else if(!token.compare("XOR")){
                        if(sliced.compare(";")){
                        //add an xor gate
                            if(out_flag){
                                string s;
                                stringstream ss(s);
                                ss << _numGate;
                                string gateName = "Gate " + ss.str();
                                _gate[_numGate].setName(gateName);
                                _gate[_numGate].setType("xor");
                                _gate[_numGate].setOutWire(_wireMap.find(sliced)->second);
                                out_flag = false;
                            }
                            else{
                                _gate[_numGate].addInWire(_wireMap.find(sliced)->second);
                            }
                        }
                        else{
                            token = "WAIT_GATE";
                            _numGate++;
                        }
                    }
                    else if(!token.compare("XNOR")){
                        if(sliced.compare(";")){
                        //add an xnor gate
                            if(out_flag){
                                string s;
                                stringstream ss(s);
                                ss << _numGate;
                                string gateName = "Gate " + ss.str();
                                _gate[_numGate].setName(gateName);
                                _gate[_numGate].setType("xnor");
                                _gate[_numGate].setOutWire(_wireMap.find(sliced)->second);
                                out_flag = false;
                            }
                            else{
                                _gate[_numGate].addInWire(_wireMap.find(sliced)->second);
                            }
                        }
                        else{
                            token = "WAIT_GATE";
                            _numGate++;
                        }
                    }


                    if(!sliced.compare("module")){
                        token = "CKTNAME";
                    }
                    else if(!sliced.compare("input")){
                        token = "INPUT";
                    }
                    else if(!sliced.compare("output")){
                        token = "OUTPUT";
                    }
                    else if(!sliced.compare("wire")){
                        token = "WIRE";
                    }
                    else if(!token.compare("WAIT_GATE") && !sliced.compare("not")){
                        token = "NOT";
                        out_flag = true;
                    }
                    else if(!token.compare("WAIT_GATE") && !sliced.compare("buf")){
                        token = "BUF";
                        out_flag = true;
                    }
                    else if(!token.compare("WAIT_GATE") && !sliced.compare("and")){
                        token = "AND";
                        out_flag = true;
                    }
                    else if(!token.compare("WAIT_GATE") && !sliced.compare("nand")){
                        token = "NAND";
                        out_flag = true;
                    }
                    else if(!token.compare("WAIT_GATE") && !sliced.compare("or")){
                        token = "OR";
                        out_flag = true;
                    }
                    else if(!token.compare("WAIT_GATE") && !sliced.compare("nor")){
                        token = "NOR";
                        out_flag = true;
                    }
                    else if(!token.compare("WAIT_GATE") && !sliced.compare("xor")){
                        token = "XOR";
                        out_flag = true;
                    }
                    else if(!token.compare("WAIT_GATE") && !sliced.compare("xnor")){
                        token = "XNOR";
                        out_flag = true;
                    }
                    else if(!token.compare("WAIT_GATE") && !sliced.compare("dff")){
                        token = "DFF";
                        out_flag = true;
                    }
                    else if(!token.compare("WAIT_GATE") && !sliced.compare("endmodule")){
                        token = "END";
                    }

                    sliced = sliceVerilog(&tmp);
                }
            }
        }
    }

    connectWire();
    checkWire();

    inVeriFile.close();
    return true;
}

void Circuit::connectWire(){
    for(unsigned i = 0 ; i < _numGate ; ++i){
        _wire[_gate[i].outWire()].setPreGate(i);
        for(unsigned j = 0 ; j < _gate[i].numInWire() ; ++j){
            _wire[_gate[i].inWire(j)].addPosGate(i);
        }
    }
}

void Circuit::print(){
    cout<<"###################################################\n";
    cout<<_name<<" Wire: "<<_numWire<<" Gate: "<<_numGate<<"\n";
    cout<<"Wire: "<<_numWire<<"\n";
    for(unsigned i = 0 ; i < _numWire ; ++i){
        cout<<"Id: "<<i<<" ";
        _wire[i].print();
    }
    cout<<"\n";
    cout<<"Gate: "<<_numGate<<"\n";
    for(unsigned i = 0 ; i < _numGate ; ++i){
        cout<<"Id: "<<i<<" ";
        _gate[i].print();
    }
    cout<<"###################################################\n";
}

void Circuit::printDuplicated(){
    cout<<"###################################################\n";
    cout<<_name<<" Wire: "<<_numDuplicatedWire<<" Gate: "<<_numDuplicatedGate<<"\n";
    cout<<"Wire: "<<_numDuplicatedWire<<"\n";
    for(unsigned i = 0 ; i < _numDuplicatedWire ; ++i){
        cout<<"Id: "<<i<<" ";
        _wire[i].print();
    }
    cout<<"\n";
    cout<<"Gate: "<<_numDuplicatedGate<<"\n";
    for(unsigned i = 0 ; i < _numDuplicatedGate ; ++i){
        cout<<"Id: "<<i<<" ";
        _gate[i].print();
    }
    cout<<"###################################################\n";
}

void Circuit::checkWire(){
    for(unsigned i = 0 ; i < _numWire ; ++i){
        if(wire(i).preGate() == -1){
            if(wire(i).numPosGate() == 0){
                // unused wire
                if(wire(i).type() != "UNUSED"){
                    cout<<"Warning: wire "<<wire(i).name()<<" has no pre/post connection, set the type to UNUSED.\n";
                    wire(i).setType("UNUSED");
                }
            }
            else{
                // pi, TIE0, or TIE1
                if(wire(i).type() != "PI" && wire(i).type() != "TIE0" && wire(i).type() != "TIE1"){
                    cout<<"Warning: wire "<<wire(i).name()<<" has no pre connection, set the type to PI.\n";
                    wire(i).setType("PI");
                }
            }
        }
        else{
            if(wire(i).numPosGate() == 0){
                // po
                if(wire(i).type() != "PO"){
                    cout<<"Warning: wire "<<wire(i).name()<<" has no post connection, set the type to PO.\n";
                    wire(i).setType("PO");
                }
            }
            else{
                // normal
                if(wire(i).type() != "NORMAL"){
                    cout<<"Warning: wire "<<wire(i).name()<<" has normal connection, set the type to NORMAL.\n";
                    wire(i).setType("NORMAL");
                }
            }
        }
    }


    /************/
    for(unsigned i = 0 ; i < _numGate ; ++i){
        unsigned outWireId = gate(i).outWire();
        for(unsigned j = 0 ; j < wire(outWireId).numPosGate() ; ++j){
            unsigned posGateId = wire(outWireId).posGate(j);
            if(gate(i).type() == "dff" && gate(posGateId).type() == "dff"){
                cout<<"Warning: two consecutive dffs "<<gate(i).name()<<" "<<gate(posGateId).name()<<" with wire "<<wire(outWireId).name()<<"\n";
            }
        }
    }

    /************/



}

vector<int> level;
bool levelCompare(int gate1, int gate2)
{
	//cout << gate1 << " " << gate2 << " " << level.size() <<  endl;
	//cout << level[gate1] << " " << level[gate2] << endl;
	return level[gate1] < level[gate2];
}

bool Circuit::setLevel()
{
	// read pi
	vector<int> pi;
	for(unsigned i = 0 ; i < numWire() ; ++i)
	{
		Wire piWire = wire(i);
		if(piWire.type() == "PI")
			pi.push_back(i);
	}
	
	// all gate not visit yet
	queue<int> gates;
	vector<int> visitCount(numGate() , 0);

	for(unsigned i = 0 ; i < pi.size() ; ++i)
	{
		Wire piWire = wire(pi[i]);

		// push fanout gates to queue
		for(unsigned j = 0 ; j < piWire.numPosGate() ; ++j)
		{
			int posGateID = piWire.posGate(j);
			gates.push(posGateID);
		}
	}
	
	// set level of gates
	level.assign(numGate() , -1);

	// visit all gates
	while(!gates.empty())
	{
		int gateID = gates.front();
		gates.pop();

		Gate visitedGate = gate(gateID);

		visitCount[gateID]++;

		if(visitCount[gateID] != (int)visitedGate.numInWire())
			continue;

		// calculate level
		int maxSubLevel = 0;
		for(unsigned i = 0 ; i < visitedGate.numInWire() ; ++i)
		{
			Wire inWire = wire(visitedGate.inWire(i));
			int preGateID = inWire.preGate();
            //cout<< preGateID;
			maxSubLevel = max(maxSubLevel , level[preGateID]);
            //cout<<'\t'<< maxSubLevel<<endl;
		}

		level[gateID] = maxSubLevel + 1;
        gate(gateID).setLevel( maxSubLevel +1 );		

		// push fanout gates to queue
		Wire outWire = wire(visitedGate.outWire());
		for(unsigned j = 0 ; j < outWire.numPosGate() ; ++j)
		{
			int posGateID = outWire.posGate(j);
			gates.push(posGateID);
		}
	}
	
	// sequence
	for(unsigned i = 0 ; i < numGate() ; ++i)
		topologicalSequence.push_back(i);

		
	sort(topologicalSequence.begin() , topologicalSequence.end() , levelCompare);

	if(value.size() == 0)
		value.assign(numWire() , 0);
	//if(cutPoint.size() == 0)
	//	cutPoint.assign(numWire() , 0);

	return true;
}

/*
Pattern* Circuit::RandomGenPattern()
{
    Pattern* pattern = new Pattern();

    srand( time(NULL) );
    for( unsigned i=0 ; i < numWire(); ++i )
    {
       Wire pWire = wire(i);
       if( pWire.type() == "PI" )
           pattern->value.push_back( rand() ); 
    }

    return pattern;
}*/

void Circuit::AssignPiValue( Pattern* PatternSet )
{
    for( unsigned i = 0, j = 0 ; i < numWire(); ++i )
    {
        Wire pWire = wire(i);
        if( pWire.type() == "PI" )
        {
            value[ i ] = PatternSet->value[j];
            wire(i).setValueSet( PatternSet->value[j] );
            ++j;
        }
        else if ( pWire.type() == "TIE0" ) // assign value to TIE0
        {
            value[ i ] = 0;
            wire(i).setValueSet( 0 );
        }
        else if ( pWire.type() == "TIE1" ) // assign value to TIE1
        {
            value[ i ] = ~0;
            wire(i).setValueSet( ~0 );
        }
        else if ( pWire.isCutPoint() ) // assign value to cut points
        {
            value[ i ] = PatternSet->value[j];
            wire(i).setValueSet( PatternSet->value[j] );
            ++j;
        }
    }
}

void Circuit::logicSim( Pattern* PatternSet )
{
    AssignPiValue( PatternSet );

	for(unsigned i = 0 ; i < topologicalSequence.size() ; ++i)
	{
		int gateID = topologicalSequence[i];
		Gate targetGate = gate(gateID);
	    // PI = 1, PO = 2, CUT = 3, CUT_BAR = 4, TIE0 = 5, TIE1 = 6, NORMAL = 7, REMOVED = 8, UNUSED = 0
    	
		int finalValue = value[targetGate.inWire(0)];
		for(unsigned j = 1 ; j < targetGate.numInWire() ; ++j)
		{
			int inWireID = targetGate.inWire(j);
			//assert(level[wire(inWireID).preGate()] <= level[gateID]);
			int inValue = value[inWireID];

			switch(targetGate.typeID())
			{
				case 1:
				case 2:
					finalValue = inValue;
					break;
				case 3:
				case 4:
					finalValue = finalValue & inValue;
					break;
				case 5:
				case 6:
					finalValue = finalValue | inValue;
					break;
				case 7:
				case 8:
					finalValue = finalValue ^ inValue;
					break;
				default:
					cout << "strange gate type id: " << targetGate.typeID() << endl;
					exit(0);
			}
		}
		switch(targetGate.typeID())
		{
			case 1:
			case 4:
			case 6:
			case 8:
				finalValue = ~ finalValue;
				break;
			default:
				break;
		}

        ///cout<< finalValue<<endl;

        Wire fanoutWire = wire( targetGate.outWire() );
		if( ! fanoutWire.isCutPoint()  )
        {
            wire( targetGate.outWire() ).setValueSet( finalValue );
            value[ targetGate.outWire() ] = finalValue;
        }

	}
	return;
}

void Circuit::dumpCircuit()
{
    for( unsigned i = 0; i< numGate(); ++i )
    {
        Gate g = gate( i );
        cout<< g.name() << '\t' ;
        cout<< "level" << g.level() << '\t' ;
        //cout<< bitset<32>( value[i] ) <<'\t';
        cout<< endl;
    }

    for( unsigned j = 0; j< numWire(); ++j )
    {
        Wire w = wire( j );
        cout<< j << '\t';
        cout<< w.name() << '\t';
        cout<< "type:" << setiosflags(ios::left) << setw(8) << w.type() <<'\t';
        if( w.type() != "UNUSED" )
            cout<< "value=" << bitset<32>( w.valueSet() )<<'\t';
        cout<< w.isCutPoint() <<'\t';
        cout<<endl;
    }
}


void Circuit::addWire(unsigned wireId, string wireName, string wireType){
    if(_wire[wireId].type() != "UNUSED"){
        cout<<"Warning: add a wire "<<wireName<<" to a position existing another wire "<<_wire[wireId].name()<<"\n";
    }
    if(wireId >= buffer_length){
        cout<<"Error: try to add a wire with wireID "<<wireId<<" exceeds the length of array, return.\n";
        return;
    }
    _wire[wireId].setName(wireName);
    _wire[wireId].setType(wireType);
}

void Circuit::addGate(unsigned gateId, string gateName, string gateType){
    if(_gate[gateId].type() != "unknown"){
        cout<<"Warning: add a gate "<<gateName<<" to a position existing another gate "<<_gate[gateId].name()<<"\n";
    }
    if(gateId >= buffer_length){
        cout<<"Error: try to add a gate with gateID "<<gateId<<" exceeds the length of array, return.\n";
        return;
    }
    _gate[gateId].setName(gateName);
    _gate[gateId].setType(gateType);
}

bool Circuit::writeVerilog(string fileName)
{
    ofstream outVeriFile(fileName.c_str(), ios::binary);
    if(!outVeriFile.is_open()){
        cout<<"Error: "<<fileName<<" cannot be opened.\n";
        return false;
    }
    // Module term
    bool firstTime = true;
    outVeriFile << "module top ( ";
    for(unsigned i=0;i<_numWire;i++)
    {
        if(_wire[i].type() == "PI" || _wire[i].type() == "PO")
        {
            if(firstTime == true)
            {
                outVeriFile << _wire[i].name();
                firstTime = false;
            }
            else
            {
                outVeriFile << ", " << _wire[i].name();
            }
        }
    }
    outVeriFile << ");" << endl;
    // Inputs
    firstTime = true;
    outVeriFile << "input ";
    for(unsigned i=0;i<_numWire;i++)
    {
        if(_wire[i].type() == "PI")
        {
            if(firstTime == true)
            {
                outVeriFile << _wire[i].name();
                firstTime = false;
            }
            else
            {
                outVeriFile << ", " << _wire[i].name();
            }
        }
    }
    outVeriFile << ";" << endl;
    // Outputs
    firstTime = true;
    outVeriFile << "output ";
    for(unsigned i=0;i<_numWire;i++)
    {
        if(_wire[i].type() == "PO")
        {
            if(firstTime == true)
            {
                outVeriFile << _wire[i].name();
                firstTime = false;
            }
            else
            {
                outVeriFile << ", " << _wire[i].name();
            }
        }
    }
    outVeriFile << ";" << endl;
    // Wires
    firstTime = true;
    outVeriFile << "wire ";
    for(unsigned i=0;i<_numWire;i++)
    {
        if(_wire[i].type() == "NORMAL")
        {
            if(firstTime == true)
            {
                outVeriFile << _wire[i].name();
                firstTime = false;
            }
            else
            {
                outVeriFile << ", " << _wire[i].name();
            }
        }
    }
    outVeriFile << ";" << endl;
    // Gates
    for(unsigned i=0;i<_numGate;i++)
    {
        outVeriFile << _gate[i].type() << " (";
        outVeriFile << _wire[_gate[i].outWire()].name();
        for(unsigned j=0;j<_gate[i].numInWire();j++)
        {
            outVeriFile << ", " << _wire[_gate[i].inWire(j)].name();
        }
        outVeriFile << ");" << endl;
    }

    outVeriFile << endl;
    outVeriFile << "endmodule" << endl;
    outVeriFile.close();
    return true;

}

