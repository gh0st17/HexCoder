#include "HexCoder.hpp"
#include "HashAlgorithms/md5.hpp"
#include "HashAlgorithms/SHA256.hpp"
#include "HashAlgorithms/sha512.hpp"
#pragma once

typedef chrono::duration<double> fsec;

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
  EncryptionMetod method = EncryptionMetod::Pass;
  HashAlgorithm hAlg = HashAlgorithm::SHA256;
};

class Manager {

public:
  Manager(Params& params);
  Manager();
  ~Manager();

private:
  Params params;
  mutex m_locker;
  string path, pass;
  Instructions insts;
  HexCoder hc;
  size_t blockSize = 1Ui64 << 28Ui64,
    threadCount = thread::hardware_concurrency();
  const set<string> hAlgs {
    "None", "MD5", "SHA256", "SHA512"
  };

  void readFilePth(const size_t n_thread, string& file, const string& path,
    const size_t start, const size_t partStart, const size_t partEnd, bool mode);
  void setBlockSize();
  void enterPass();
  void codeFile(bool mode);
};

