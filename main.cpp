#include "Manager.hpp"

void printHelp(string str) {
  cout << "Usage: " << str.substr(str.find_last_of('\\') + 1);
  cout << " {-m {p|a|b}} {-t {t|f}} {-d {e|d}} [--hash {None|MD5|SHA256|SHA512}] ";
  cout << "[-b blockSize] [-f filePath] [-a actionsFilePath] [-h]" << endl;
  cout << "-m, --method\t\tp - Password, a - Actions, b - Both\n";
  cout << "-t, --type\t\tt - Text, f - File\n";
  cout << "-d, --direction\t\te - Encrypt, d - Decrypt\n";
  cout << "    --hash\t\tHash algorithm for password. Default is MD5.\n";
  cout << "-b,       \t\tBlock size as power of two.\n";
  cout << "-f, --file\t\tAbsolute file path\n";
  cout << "-a, --actions\t\tAbsolute actions file path\n";
  cout << "-h, --help\t\tPrint this message\n";
  exit(0);
  return;
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
        if (str == "p")
          params.method = EncryptionMetod::Pass;
        else if (str == "a")
          params.method = EncryptionMetod::Actions;
        else if (str == "b")
          params.method = EncryptionMetod::Both;
        else {
          cout << "Unknown method " << str << endl;
          exit(1);
        }
      }
    }
    else if (str == "-t" || str == "--type") {
      if (check(i)) {
        str = argv[++i];
        if (str == "t")
          params.type = OperationType::Text;
        else if (str == "f")
          params.type = OperationType::File;
        else {
          cout << "Unknown operation type " << str << endl;
          exit(1);
        }
      }
    }
    else if (str == "-d" || str == "--direction") {
      if (check(i)) {
        str = argv[++i];
        if (str == "e")
          params.mode = true;
        else if (str == "d")
          params.mode = false;
        else {
          cout << "Unknown direction " << str << endl;
          exit(1);
        }
      }
    }
    else if (str == "--hash") {
      if (check(i)) {
        str = argv[++i];
        if (str == "None")
          params.hAlg = HashAlgorithm::None;
        else if (str == "MD5")
          params.hAlg = HashAlgorithm::MD5;
        else if (str == "SHA256")
          params.hAlg = HashAlgorithm::SHA256;
        else if (str == "SHA512")
          params.hAlg = HashAlgorithm::SHA512;
        else {
          cout << "Unknown hash algorithm " << str << endl;
          exit(1);
        }
      }
    }
    else if (str == "-b") {
      if (check(i)) {
        str = argv[++i];
        uint16_t powerOfTwo;
        try {
          powerOfTwo = stoul(str);
          if ((1Ui64 << powerOfTwo) < thread::hardware_concurrency())
            throw "Small block size\n";
        }
        catch (exception e) {
          cerr << "Entered not a number.";
          exit(1);
        }
        catch (const char * c) {
          cerr << c << endl;
          exit(1);
        }
      }
      else {
        cout << "Block size did not set! Exiting.\n";
        exit(1);
      }
    }
    else if (str == "-f" || str == "--file") {
      if (check(i)) {
        str = argv[++i];
        if (filesystem::exists(str))
          params.path = str;
        else {
          cout << "File not exists!\n";
          exit(1);
        }
      }
      else {
        cout << "File path did not set! Exiting.\n";
        exit(1);
      }
    }
    else if (str == "-a" || str == "--actions") {
      if (check(i)) {
        str = argv[++i];
        if (filesystem::exists(str))
          params.actionPath = str;
        else {
          cout << "Actions file not exists!\n";
          exit(1);
        }
      }
      else {
        cout << "Actions file path did not set! Exiting.\n";
        exit(1);
      }
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
    Manager m(params);
	}
	else
    Manager m;
	return 0;
}