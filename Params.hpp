#include "libs.hpp"
#pragma once

enum class HashAlgorithm {
  None, MD5, SHA256, SHA512
};

enum class EncryptionMetod {
  Pass, Actions, Both
};

struct Params {

public:
  string path;
  string actionPath;
  bool mode = true;
  bool isCreate = false;
  bool isView = false;
  size_t blockSize = 1ULL << 28ULL;
  EncryptionMetod method = EncryptionMetod::Pass;
  HashAlgorithm hAlg = HashAlgorithm::SHA256;

  Params() {}
  Params(const int argc, const char* argv[]);
  static void printHelp(string str);
  Params operator=(const Params& rhs);
  const string getHashAlgorithmName();
  const string getEncryptionMethodName();

private:
  const set<string> paramsWithoutVal{
    "-c","--create","--view",
    "-h","--help"
  };

  const set<string> paramsWithVal{
    "-m","--method","-d","--direction",
    "--hash","-b","-f","--file",
    "-a","--actions"
  };

  void setMethod(const string& method);
  void setDirection(const string& direction);
  void setHashAlg(const string& hashAlg);
  void setBlockSize(const string& blockSize);
  void setFilePath(const string& filePath);
  void setActionsFilePath(const string& actionsFilePath);
};