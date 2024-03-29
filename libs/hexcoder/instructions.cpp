#include <instructions.hpp>

Instructions::~Instructions() {
  zero();
}

void Instructions::create_instructions() {
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
  while ((op != '0' && val != 0) || acts.empty()) {
    cin >> op >> val;
    if (op != 0 && val != 0 && validateOp(op)) {
      if (val == 0 || val > 255)
        continue;
      if ((op == '<' || op == '>') && val > 7) {
        val %= 8;
        if (!val)
          continue;
      }
      acts.push_back(make_pair(op, val));
    }
  }
  cout << "Entered:\n";
  view_instructions();
}

void Instructions::read_instructions(const string& path) {
  size_t size;
  try {
    ifstream ifs(path, ifstream::binary);
    ifs.seekg(0, ifs.end);
    size = ifs.tellg();
    if (size & 1)
      size--;
    ifs.seekg(0, ifs.beg);
    acts.resize(size / sizeof(Action));
    ifs.read(reinterpret_cast<char*>(&acts[0]), size);
    ifs.close();
  }
  catch (bad_alloc const&) {
    cerr << "Can't allocate memory size " << size << " bytes for actions";
    exit(1);
  }
  catch (exception const& e) {
    cerr << e.what() << endl;
  }
  char op;
  uint8_t val;
  for (Actions::iterator it = acts.begin(); it != acts.end();) {
    op = it->first;
    val = it->second;
    if (!validateOp(op) || val == 0)
      it = acts.erase(it);
    else {
      if ((op == '<' || op == '>') && val > 7) {
        it->second %= 8;
        if (!it->second) {
          it = acts.erase(it);
          continue;
        }
      }
      ++it;
    }
  }
  if (acts.empty()) {
    cout << "Actions not found";
    exit(1);
  }
}

void Instructions::write_instructions(const string& path) {
  ofstream ofs(path, ifstream::binary);
  ofs.write(reinterpret_cast<const char*>(&acts[0]), acts.size() * sizeof(Action));
  ofs.close();
}

void Instructions::view_instructions() {
  for (const auto& x : acts)
    cout << x.first << ' ' << +x.second << endl;
  cout << "Actions count: " << acts.size() << endl;
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

void Instructions::reverse_actions(string& data, const size_t start, const size_t end) {
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

const size_t Instructions::get_actions_count() {
  return acts.size();
}

bool Instructions::validateOp(const char& op) {
  return Ops.find(op) != Ops.end();
}

void Instructions::zero() {
  fill(acts.begin(), acts.end(), Action(0, 0));
}