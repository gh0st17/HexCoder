#include <params.hpp>

Params::Params(const size_t argc, const char* argv[]) {
  string str;
  for (size_t i = 1; i < argc; i++) {
    str = string(argv[i]);
    if (params_val.find(str) != params_val.end() &&
        i + 1 < argc &&
        params_val.find(argv[i + 1]) == params_val.end()) {
      if (str == "-m" || str == "--method")
        set_method(argv[++i]);
      else if (str == "--hash")
        set_hash_alg(argv[++i]);
      else if (str == "-b")
        set_block_size(argv[++i]);
      else if (str == "-t")
        set_threads_count(argv[++i]);
      else if (str == "-f" || str == "--file")
        set_file_path(argv[++i]);
      else if (str == "-a" || str == "--actions")
        set_actions_file_path(argv[++i]);
    }
    else if (params_no_val.find(str) != params_no_val.end()) {
      if (str == "-d" || str == "--decrypt") {
        mode = false;
        continue;
      }
      else if (str == "-v" || str == "--verbose") {
        verbose = true;
        continue;
      }
      else if (str == "--view")
        is_view = true;
      else if (str == "-c" || str == "--create")
        is_create = true;
      else if (str == "-h" || str == "--help")
        print_help(argv[0]);
      break;
    }
    else if (params_val.find(str) == params_val.end() &&
              params_no_val.find(str) == params_no_val.end()) {
      cout << "Unknown parameter " << str << endl;
      exit(1);
    }
    else if (params_val.find(str) != params_val.end() &&
              (i + 1 == argc ||
              (params_val.find(argv[i + 1]) != params_val.end() ||
              params_no_val.find(argv[i + 1]) == params_no_val.end()))) {
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

void Params::print_help(string str) {
  auto t_Count = thread::hardware_concurrency();
  cout << "Usage: " << str.substr(str.find_last_of('\\') + 1);
  cout << " [-m {p|a|b}] [-d] [--hash {None|MD5|SHA256|SHA512}] ";
  cout << "[-b blockSize] {-f filePath} [-a actionsFilePath] [-t threadsCount] [-v] | -c | --view | -h" << endl;
  cout << "-m, --method\t\tp - Password, a - Actions, b - Both. Default Password\n";
  cout << "-d, --decrypt\t\tDecrypt mode. Default Encrypt (unset)\n";
  cout << "    --hash\t\tHash algorithm for password. Default is SHA256\n";
  cout << "-b,       \t\tBlock size as power of two. Default 28 (256Mb)\n";
  cout << "-f, --file\t\tFile path\n";
  cout << "-a, --actions\t\tActions file path\n";
  cout << "-t,       \t\tThreads count from 1 to " << t_Count << ". Default " << t_Count << "\n";
  cout << "-v, --verbose      \t\tThreads count from 1 to " << t_Count << ". Default " << t_Count << "\n";
  cout << "-c, --create\t\tOpen menu for creating and save actions\n";
  cout << "            \t\tParameters above will be ignore\n";
  cout << "    --view\t\tView actions\n";
  cout << "            \t\tParameters above will be ignore\n";
  cout << "-h, --help\t\tPrint this message\n";
  exit(0);
}

Params Params::operator=(const Params& rhs) {
  path = rhs.path;
  action_path = rhs.path;
  mode = rhs.mode;
  is_create = rhs.is_create;
  is_view = rhs.is_view;
  verbose = rhs.verbose;
  block_size = rhs.block_size;
  threads_count = rhs.threads_count;
  method = rhs.method;
  hAlg = rhs.hAlg;
  return *this;
}

const string Params::get_hash_name() {
  if (hAlg == HashAlgorithm::MD5)
    return "MD5";
  else if (hAlg == HashAlgorithm::SHA256)
    return "SHA256";
  else if (hAlg == HashAlgorithm::SHA512)
    return "SHA512";
  else
    return "None";
}

const string Params::get_enc_method_name() {
  if (method == EncryptionMethod::Pass)
    return "Password";
  else if (method == EncryptionMethod::Actions)
    return "Actions";
  else
    return "Both";
}

void Params::set_method(const string& method) {
  if (method == "p")
    this->method = EncryptionMethod::Pass;
  else if (method == "a")
    this->method = EncryptionMethod::Actions;
  else if (method == "b")
    this->method = EncryptionMethod::Both;
  else {
    cout << "Unknown method " << method << endl;
    exit(1);
  }
}

void Params::set_hash_alg(const string& hash_alg) {
  if (hash_alg == "None")
    hAlg = HashAlgorithm::None;
  else if (hash_alg == "MD5")
    hAlg = HashAlgorithm::MD5;
  else if (hash_alg == "SHA256")
    hAlg = HashAlgorithm::SHA256;
  else if (hash_alg == "SHA512")
    hAlg = HashAlgorithm::SHA512;
  else {
    cout << "Unknown hash algorithm " << hash_alg << endl;
    exit(1);
  }
}

void Params::set_block_size(const string& block_size) {
  uint64_t power_of_two;
  try {
    power_of_two = stoul(block_size);
    if (power_of_two > 63)
      throw "Too big block size";
    if ((1ULL << power_of_two) < thread::hardware_concurrency())
      throw "Too small block size";
    this->block_size = 1ULL << power_of_two;
  }
  catch (exception const& e) {
    cerr << "Entered not a number";
    exit(1);
  }
  catch (const char* c) {
    cerr << c << endl;
    exit(1);
  }
}

void Params::set_threads_count(const string& threads_count) {
  try {
    this->threads_count = stoul(threads_count);
    if (this->threads_count > thread::hardware_concurrency())
      throw "Too many threads count";
    if (this->threads_count < 1)
      throw "Threads count can not be less 1";
  }
  catch (exception const& e) {
    cerr << "Entered not a number";
    exit(1);
  }
  catch (const char* c) {
    cerr << c << endl;
    exit(1);
  }
}

void Params::set_file_path(const string& file_path) {
  if (filesystem::exists(file_path))
    path = file_path;
  else {
    cout << "File not exists!\n";
    exit(1);
  }
}

void Params::set_actions_file_path(const string& actions_file_path) {
  if (filesystem::exists(actions_file_path))
    action_path = actions_file_path;
  else {
    cout << "Actions file not exists!\n";
    exit(1);
  }
}
