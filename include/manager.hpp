#pragma once
#include <hexcoder.hpp>
#include <params.hpp>
#include <md5.hpp>
#include <SHA256.hpp>
#include <sha512.hpp>

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

  void readFilePth(const size_t n_thread, string& file, const string& path,
    const size_t start, const size_t partStart, const size_t partEnd, bool mode, size_t b);
  void enterPass();
  void codeFile(bool mode);
  void actionsMenu();
};

