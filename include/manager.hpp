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
  Manager(const string& action_path);
  Manager();
  ~Manager();

private:
  Params params;
  mutex m_locker;
  string pass;
  Instructions insts;
  HexCoder hc;

  void read_file_pth(const size_t n_thread, string& file, const string& path,
    const size_t start, const size_t part_start, const size_t part_end, bool mode, size_t b);
  void enter_pass();
  void code_file(bool mode);
  void actions_menu();
};

