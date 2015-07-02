#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <iostream>

using namespace std;

static unsigned const buffer_length = 10000000;

/*** utility functions ***/
string sliceString(string* original);
string sliceVerilog(string* original);
#endif // UTIL_H
