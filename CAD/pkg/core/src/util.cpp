#include "util.h"

#include <string>
#include <iostream>

using namespace std;

string sliceString(string* original){
    if(*original == "")
        return "";

    *original = original->substr(original->find_first_not_of(' '), original->length());
    *original = original->substr(0, original->find_last_not_of(' ')+1);

    string sliced;
    if(original->find_first_of(' ') == string::npos){
        sliced = *original;
        *original = original->substr(0,0);
    }
    else{
        sliced = original->substr(0, original->find_first_of(' '));
        *original = original->substr(original->find_first_of(' ')+1, original->length());
    }

    return sliced;
}

string sliceVerilog(string* original){
    if(original->find_first_not_of(" (),") == string::npos )
        return "";

    *original = original->substr(original->find_first_not_of(" (),"), original->length());
    *original = original->substr(0, original->find_last_not_of(" (),\r")+1);

    string sliced;
    if(original->find_first_of(" (),") == string::npos){
        if(original->find(';') != string::npos){
            *original = original->substr(0, original->find_last_of(';')+1);
            if((*original)[original->length()-1] == ';'){
                sliced = (original->find_first_not_of(';') == string::npos)? ";": original->substr(0, original->length()-1);
                *original = (original->find_first_not_of(';') == string::npos)? "": ";";
            }
            else{
                sliced = *original;
                *original = original->substr(0,0);
            }
        }
        else{
            sliced = *original;
            *original = original->substr(0,0);
        }
    }
    else{
        sliced = original->substr(0, original->find_first_of(" (),"));
        *original = original->substr(original->find_first_of(" (),")+1, original->length());
    }

    return sliced;
}
