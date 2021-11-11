#include "libs.hpp"
#pragma once

enum class HashAlgorithm {
  None, MD5, SHA256, SHA512
};

enum class EncryptionMetod {
  Pass, Actions, Both
};

struct Params {
  string path;
  string actionPath;
  bool mode = true;
  bool isCreate = false;
  bool isView = false;
  size_t blockSize = 1Ui64 << 28Ui64;
  EncryptionMetod method = EncryptionMetod::Pass;
  HashAlgorithm hAlg = HashAlgorithm::SHA256;

  void setMethod(const string& method);
  void setDirection(const string& direction);
  void setHashAlg(const string& hashAlg);
  void setBlockSize(const string& blockSize);
  void setFilePath(const string& filePath);
  void setActionsFilePath(const string& actionsFilePath);
};