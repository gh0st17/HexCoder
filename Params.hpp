#include "libs.hpp"
#pragma once

enum class HashAlgorithm {
  None, MD5, SHA256, SHA512
};

enum class EncryptionMethod {
  Pass, Actions, Both
};

struct Params {

public:
  string path;
  string actionPath;
  bool mode = true;
  bool isCreate = false;
  bool isView = false;
  bool verbose = false;
  size_t blockSize = 1ULL << 28ULL;
  size_t threadsCount = thread::hardware_concurrency();
  EncryptionMethod method = EncryptionMethod::Pass;
  HashAlgorithm hAlg = HashAlgorithm::SHA256;

  Params() {}
  Params(const int argc, const char* argv[]);
  static void printHelp(string str);
  Params operator=(const Params& rhs);
  const string getHashAlgorithmName();
  const string getEncryptionMethodName();

private:
  const set<string> paramsWithoutVal{
    "-d","--decrypt",
    "-c","--create","--view",
    "-h","--help",
    "-v","--verbose"
  };

  const set<string> paramsWithVal{
    "-m","--method",
    "--hash","-b","-f","--file",
    "-a","--actions","-t"
  };

  void setMethod(const string& method);
  void setHashAlg(const string& hashAlg);
  void setBlockSize(const string& blockSize);
  void setThreadsCount(const string& threadsCount);
  void setFilePath(const string& filePath);
  void setActionsFilePath(const string& actionsFilePath);
};