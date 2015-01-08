#ifndef UTILITIES_H
#define UTILITIES_H

#include<iostream>
#include<string>
#include<sstream>
#include <vector>
#include <deque>
#include <limits>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <stdarg.h>


using namespace std;
using std::vector;
using std::deque;


string itos(int i);
void SplitLine(const std::string& str, const std::string& delim, std::deque<std::string>& output);
string ftos(double f);


#endif
