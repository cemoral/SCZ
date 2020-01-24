#ifndef ZERG_UTILITY_H
#define ZERG_UTILITY_H
#include <vector>
#include <string>

using std::vector;
using std::string;

vector<string> tokenize(string input, char delim);
/*
* returns a < b + EPS;
*/
bool double_less_with_epsilon(double a, double b);

#endif
