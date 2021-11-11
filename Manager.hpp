#include "HexCoder.hpp"
#include "Params.hpp"
#include "HashAlgorithms/md5.hpp"
#include "HashAlgorithms/SHA256.hpp"
#include "HashAlgorithms/sha512.hpp"
#pragma once

typedef chrono::duration<double> fsec;

class Manager {

public:
  Manager(const Params& params);
  Manager(const string& actionPath);
  Manager();
  ~Manager();

private:
  Params params;
  mutex m_locker;
  string pass;
  Instructions insts;
  HexCoder hc;
  size_t threadCount = thread::hardware_concurrency();

  void readFilePth(const size_t n_thread, string& file, const string& path,
    const size_t start, const size_t partStart, const size_t partEnd, bool mode);
  //void setBlockSize();
  void enterPass();
  void codeFile(bool mode);
  void actionsMenu();
};

