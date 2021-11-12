#include "Instructions.hpp"

Instructions::~Instructions() {
  zero();
}

void Instructions::createInstructions() {
  if (!acts.empty()) {
    zero();
    cout << "Old actions was unset\n";
  }
  cout << "Enter a sequence of types of arithmetical or logical actions to be applied.\n";
  cout << "Available operators: +, ~ (not/inverse), ^, < (cyclic lshift), > (cyclic rshift)\n";
  cout << "For inversion type any value != 0.\n";
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
  viewInstructions();
  cin.ignore();
}

void Instructions::readInstructions(const string& path) {
  ifstream ifs(path, ifstream::binary);
  ifs.seekg(0, ifs.end);
  size_t size = ifs.tellg();
  ifs.seekg(0, ifs.beg);
  try {
    acts.resize(size / sizeof(Action));
    ifs.read(reinterpret_cast<char*>(&acts[0]), size);
    ifs.close();
  }
  catch (bad_alloc const&) {
    cerr << "Can't allocate memory size " << size << " bytes for actions";
    exit(1);
  }
  Actions::iterator next;
  for (Actions::iterator it = acts.begin(); it != acts.end();) {
    if (!validateOp(it->first))
      it = acts.erase(it);
    else
      ++it;
  }
}

void Instructions::writeInstructions(const string& path) {
  ofstream ofs(path, ifstream::binary);
  ofs.write(reinterpret_cast<const char*>(&acts[0]), acts.size() * sizeof(Action));
  ofs.close();
}

void Instructions::viewInstructions() {
  for (const auto& x : acts)
    cout << x.first << ' ' << +x.second << endl;
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

void Instructions::zero() {
  fill(acts.begin(), acts.end(), Action(0, 0));
}