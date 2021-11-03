#include "Instructions.hpp"

Instructions::Instructions() {
  cout << "Enter a sequence of types of logical actions to be applied.\n";
  cout << "For example: | 5 | 32 & 21\n> ";
  string seq;
  getline(cin, seq);
  auto pos1 = seq.find(' ');
  auto pos2 = seq.find(' ', pos1 + 1);
  char op = seq[0], val;
  while (pos2 != string::npos) {

  }
}