#include "Manager.hpp"

void printHelp(string str) {
  cout << "Usage: " << str.substr(str.find_last_of('\\') + 1);
  cout << " {-m {p|a|b}} {-d {e|d}} [--hash {None|MD5|SHA256|SHA512}] ";
  cout << "[-b blockSize] {-f filePath} [-a actionsFilePath] [-h] | -c" << endl;
  cout << "-m, --method\t\tp - Password, a - Actions, b - Both. Default Password\n";
  cout << "-d, --direction\t\te - Encrypt, d - Decrypt. Default Encrypt\n";
  cout << "    --hash\t\tHash algorithm for password. Default is SHA256\n";
  cout << "-b,       \t\tBlock size as power of two. Default 28 (256Mb)\n";
  cout << "-f, --file\t\tFile path\n";
  cout << "-a, --actions\t\tActions file path\n";
  cout << "-c, --create\t\tOpen menu for creating and save actions\n";
  cout << "            \t\tOther parameters will be ignore\n";
  cout << "-h, --help\t\tPrint this message\n";
}

Params getParams(const int argc, const char* argv[]) {
  Params params;
  string str;
  auto check = [argc](size_t& i) {
    return argc > i;
  };
  for (size_t i = 1; i < argc; i++) {
    str = string(argv[i]);
    if (str == "-m" || str == "--method") {
      if (check(i)) {
        str = argv[++i];
        params.setMethod(str);
      }
    }
    else if (str == "-d" || str == "--direction") {
      if (check(i)) {
        str = argv[++i];
        params.setDirection(str);
      }
    }
    else if (str == "--hash") {
      if (check(i)) {
        str = argv[++i];
        params.setHashAlg(str);
      }
    }
    else if (str == "-b") {
      if (check(i)) {
        str = argv[++i];
        params.setBlockSize(str);
      }
      else {
        cout << "Block size did not set! Exiting.\n";
        exit(1);
      }
    }
    else if (str == "-f" || str == "--file") {
      if (check(i)) {
        str = argv[++i];
        params.setFilePath(str);
      }
      else {
        cout << "File path did not set! Exiting.\n";
        exit(1);
      }
    }
    else if (str == "-a" || str == "--actions") {
      if (check(i)) {
        str = argv[++i];
        params.setActionsFilePath(str);
      }
      else {
        cout << "Actions file path did not set! Exiting.\n";
        exit(1);
      }
    }
    else if (str == "-c" || str == "--create") {
      params.isCreate = true;
      break;
    }
    else if (str == "-h" || str == "--help")
      printHelp(argv[0]);
    else {
      cout << "Unknown parameter " << str << endl;
      exit(1);
    }
  }
  return params;
}

int main(int argc, const char* argv[]) {
  if (argc > 1) {
    Params params = getParams(argc, argv);
    if (params.isCreate)
      Manager m;
    else
      Manager m(params);
  }
  else {
    printHelp(argv[0]);
    getchar();
  }
  return 0;
}