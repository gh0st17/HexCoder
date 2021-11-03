#include "Instructions.hpp"


Instructions::~Instructions() {
  memset(&acts, 0, sizeof acts);
}

void Instructions::createInstructions() {
  cout << "Enter a sequence of types of logical actions to be applied.\n";
  cout << "Available operators is ^, < (cyclic lshift), > (cyclic rshift), +\n";
  cout << "To complete entering type '0 0'. For example: | 5 | 32 & 21 0 0\n> ";
  char op = 1;
  uint16_t val = 1;
  while (op != '0' && val != 0) {
    cin >> op >> val;
    if (op != 0 && val != 0 && validateOp(op)) {
      if (val == 0 || val > 255)
        continue;
      if ((op == '<' || op == '>') && val > 7)
        val %= 8;
      acts.push_back(make_pair(op, val));
    }
  }
  cout << "Entered:\n";
  for (const auto& x : acts)
    cout << x.first << ' ' << +x.second << endl;
  _getch();
}

void Instructions::readInstructions() {

}

void Instructions::writeInstructions() {

}

void Instructions::applyActions(string& data, size_t start, size_t end) {
  for (size_t i = start; i < end; i++)
    for (const auto& operand : acts) {
      const auto& op(Ops.at(operand.first));
      data[i] = op(data[i], operand.second);
    }
}

void Instructions::reverseActions(string& data, size_t start, size_t end) {
  for (size_t i = start; i < end; i++)
    for (const auto& operand : Actions(acts.rbegin(), acts.rend())) {
      const auto& op(revOps.at(operand.first));
      data[i] = op(data[i], operand.second);
    }
}

bool Instructions::validateOp(const char& op) {
  return Ops.find(op) != Ops.end();
}