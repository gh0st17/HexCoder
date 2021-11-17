#include "Params.hpp"

Params::Params(const int argc, const char* argv[]) {
  string str;
  for (size_t i = 1; i < argc; i++) {
    str = string(argv[i]);
    if (paramsWithVal.find(str) != paramsWithVal.end() &&
      i + 1 < argc &&
      paramsWithVal.find(argv[i + 1]) == paramsWithVal.end()) {
      if (str == "-m" || str == "--method")
        setMethod(argv[++i]);
      else if (str == "-d" || str == "--direction")
        setDirection(argv[++i]);
      else if (str == "--hash")
        setHashAlg(argv[++i]);
      else if (str == "-b")
        setBlockSize(argv[++i]);
      else if (str == "-t")
        setThreadsCount(argv[++i]);
      else if (str == "-f" || str == "--file")
        setFilePath(argv[++i]);
      else if (str == "-a" || str == "--actions")
        setActionsFilePath(argv[++i]);
    }
    else if (paramsWithoutVal.find(str) != paramsWithoutVal.end()) {
      if (str == "--view")
        isView = true;
      else if (str == "-c" || str == "--create")
        isCreate = true;
      else if (str == "-h" || str == "--help")
        printHelp(argv[0]);
      break;
    }
    else if (paramsWithVal.find(str) == paramsWithVal.end() &&
      paramsWithoutVal.find(str) == paramsWithoutVal.end()) {
      cout << "Unknown parameter " << str << endl;
      exit(1);
    }
    else if (paramsWithVal.find(str) != paramsWithVal.end() &&
      (i + 1 == argc ||
      (paramsWithVal.find(argv[i + 1]) != paramsWithVal.end() ||
      paramsWithoutVal.find(argv[i + 1]) == paramsWithoutVal.end()))) {
      cout << "Missing parameter value after " << str << endl;
      exit(1);
    }
    else if (!(i + 1 < argc)) {
      if (str == "-a" || str == "--actions")
        cout << "Actions file path did not set! Exiting.\n";
      else if (str == "-b")
        cout << "Block size did not set! Exiting.\n";
      else if (str == "-f" || str == "--file")
        cout << "File path did not set! Exiting.\n";
      else if (str == "-a" || str == "--actions")
        cout << "Actions file path did not set! Exiting.\n";
      exit(1);
    }
  }
}

void Params::printHelp(string str) {
  auto t_Count = thread::hardware_concurrency();
  cout << "Usage: " << str.substr(str.find_last_of('\\') + 1);
  cout << " [-m {p|a|b}] [-d {e|d}] [--hash {None|MD5|SHA256|SHA512}] ";
  cout << "[-b blockSize] {-f filePath} [-a actionsFilePath] [-t threadsCount] | -c | --view | -h" << endl;
  cout << "-m, --method\t\tp - Password, a - Actions, b - Both. Default Password\n";
  cout << "-d, --direction\t\te - Encrypt, d - Decrypt. Default Encrypt\n";
  cout << "    --hash\t\tHash algorithm for password. Default is SHA256\n";
  cout << "-b,       \t\tBlock size as power of two. Default 28 (256Mb)\n";
  cout << "-f, --file\t\tFile path\n";
  cout << "-a, --actions\t\tActions file path\n";
  cout << "-t,       \t\tThreads count from 1 to " << t_Count << ". Default " << t_Count << "\n";
  cout << "-c, --create\t\tOpen menu for creating and save actions\n";
  cout << "            \t\tParameters above will be ignore\n";
  cout << "    --view\t\tView actions\n";
  cout << "            \t\tParameters above will be ignore\n";
  cout << "-h, --help\t\tPrint this message\n";
  exit(0);
}

Params Params::operator=(const Params& rhs) {
  path = rhs.path;
  actionPath = rhs.path;
  mode = rhs.mode;
  isCreate = rhs.isCreate;
  isView = rhs.isView;
  blockSize = rhs.blockSize;
  threadsCount = rhs.threadsCount;
  method = rhs.method;
  hAlg = rhs.hAlg;
  return *this;
}

const string Params::getHashAlgorithmName() {
  if (hAlg == HashAlgorithm::None)
    return "None";
  else if (hAlg == HashAlgorithm::MD5)
    return "MD5";
  else if (hAlg == HashAlgorithm::SHA256)
    return "SHA256";
  else if (hAlg == HashAlgorithm::SHA512)
    return "SHA512";
}

const string Params::getEncryptionMethodName() {
  if (method == EncryptionMetod::Pass)
    return "Password";
  else if (method == EncryptionMetod::Actions)
    return "Actions";
  else if (method == EncryptionMetod::Both)
    return "Both";
}

void Params::setMethod(const string& method) {
  if (method == "p")
    this->method = EncryptionMetod::Pass;
  else if (method == "a")
    this->method = EncryptionMetod::Actions;
  else if (method == "b")
    this->method = EncryptionMetod::Both;
  else {
    cout << "Unknown method " << method << endl;
    exit(1);
  }
}

void Params::setDirection(const string& direction) {
  if (direction == "e")
    mode = true;
  else if (direction == "d")
    mode = false;
  else {
    cout << "Unknown direction " << direction << endl;
    exit(1);
  }
}

void Params::setHashAlg(const string& hashAlg) {
  if (hashAlg == "None")
    hAlg = HashAlgorithm::None;
  else if (hashAlg == "MD5")
    hAlg = HashAlgorithm::MD5;
  else if (hashAlg == "SHA256")
    hAlg = HashAlgorithm::SHA256;
  else if (hashAlg == "SHA512")
    hAlg = HashAlgorithm::SHA512;
  else {
    cout << "Unknown hash algorithm " << hashAlg << endl;
    exit(1);
  }
}

void Params::setBlockSize(const string& blockSize) {
  uint64_t powerOfTwo;
  try {
    powerOfTwo = stoul(blockSize);
    if (powerOfTwo > 63)
      throw "Too big block size";
    if ((1ULL << powerOfTwo) < thread::hardware_concurrency())
      throw "Too small block size";
    this->blockSize = 1ULL << powerOfTwo;
  }
  catch (exception e) {
    cerr << "Entered not a number";
    exit(1);
  }
  catch (const char* c) {
    cerr << c << endl;
    exit(1);
  }
}

void Params::setThreadsCount(const string& threadsCount) {
  try {
    this->threadsCount = stoul(threadsCount);
    if (this->threadsCount > thread::hardware_concurrency())
      throw "Too many threads count";
    if (this->threadsCount < 1)
      throw "Threads count can not be less 1";
  }
  catch (exception e) {
    cerr << "Entered not a number";
    exit(1);
  }
  catch (const char* c) {
    cerr << c << endl;
    exit(1);
  }
}

void Params::setFilePath(const string& filePath) {
  if (filesystem::exists(filePath))
    path = filePath;
  else {
    cout << "File not exists!\n";
    exit(1);
  }
}

void Params::setActionsFilePath(const string& actionsFilePath) {
  if (filesystem::exists(actionsFilePath))
    actionPath = actionsFilePath;
  else {
    cout << "Actions file not exists!\n";
    exit(1);
  }
}
