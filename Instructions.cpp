#include "Instructions.hpp"

Instructions::~Instructions() {
  memset(&acts, 0, sizeof acts);
}

void Instructions::createInstructions() {
  if (!acts.empty()) {
    memset(&acts, 0, sizeof acts);
    cout << "Old actions was unset\n";
  }
  cout << "Enter a sequence of types of arithmetical or logical actions to be applied.\n";
  cout << "Available operators is +, ^, < (cyclic lshift), > (cyclic rshift)\n";
  cout << "To complete entering type '0 0'. For example: | 5 | 32 & 21 0 0\n>>> ";
  char op = 1;
  uint16_t val = 1;
  while (op != '0' && val != 0 || acts.empty()) {
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
  cin.ignore();
  _getch();
}

void Instructions::readInstructions(const string& path) {
  ifstream ifs(path, ifstream::binary);
  ifs.seekg(0, ifs.end);
  size_t size = ifs.tellg();
  ifs.seekg(0, ifs.beg);
  acts.resize(size / sizeof(Action));
  ifs.read(reinterpret_cast<char*>(&acts[0]), size);
  ifs.close();
  cout << "\nActions was read\n";
  Sleep(750);
}

void Instructions::writeInstructions(const string& path) {
  ofstream ofs(path, ifstream::binary);
  ofs.write(reinterpret_cast<const char*>(&acts[0]), acts.size() * sizeof(Action));
  ofs.close();
  cout << "\nActions written\n";
  Sleep(750);
}

void Instructions::applyActions(string& data, const size_t start, const size_t end) {
  char op;
  uint8_t val;
  for (const auto& operand : acts) {
    op = operand.first;
    val = operand.second;
    for (size_t i = start; i < end; i++)
      data[i] = Ops.at(op)(data[i], val);
  }
}

void Instructions::reverseActions(string& data, const size_t start, const size_t end) {
  Actions revActs = Actions(acts.rbegin(), acts.rend());
  char op;
  uint8_t val;
  for (const auto& operand : revActs) {
    op = operand.first;
    val = operand.second;
    for (size_t i = start; i < end; i++)
      data[i] = revOps.at(op)(data[i], val);
  }
}

bool Instructions::validateOp(const char& op) {
  return Ops.find(op) != Ops.end();
}