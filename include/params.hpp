#pragma once
#include "libs.hpp"

enum class HashAlgorithm {
  None, MD5, SHA256, SHA512
};

enum class EncryptionMethod {
  Pass, Actions, Both
};

struct Params {

public:
  string path;
  string action_path;
  bool mode = true;
  bool is_create = false;
  bool is_view = false;
  bool verbose = false;
  size_t block_size = 1ULL << 28ULL;
  size_t threads_count = thread::hardware_concurrency();
  EncryptionMethod method = EncryptionMethod::Pass;
  HashAlgorithm hAlg = HashAlgorithm::SHA256;

  Params() {}
  Params(const size_t argc, const char* argv[]);
  static void print_help(string str);
  Params operator=(const Params& rhs);
  const string get_hash_name();
  const string get_enc_method_name();

private:
  const set<string> params_no_val {
    "-d","--decrypt",
    "-c","--create","--view",
    "-h","--help",
    "-v","--verbose"
  };

  const set<string> params_val {
    "-m","--method",
    "--hash","-b","-f","--file",
    "-a","--actions","-t"
  };

  void set_method(const string& method);
  void set_hash_alg(const string& hash_alg);
  void set_block_size(const string& block_size);
  void set_threads_count(const string& threads_count);
  void set_file_path(const string& file_path);
  void set_actions_file_path(const string& actions_file_path);
};