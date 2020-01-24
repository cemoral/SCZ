#include "utility.h"
#include <iostream>

using std::cout;
using std::endl;

vector<string> tokenize(string inp, char delim) {
  vector<string> res;
  string token;
  for(auto& c: inp) {
    if(c == delim) {
      res.push_back(token);
      token.clear();
    } else {
      token.push_back(c);
    }
  }
  if(!token.empty())
    res.push_back(token);

  return res;
}

bool double_less_with_epsilon(double a, double b) {
  const double EPS = 1e-10;
  return a < b + EPS;
}
