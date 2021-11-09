#include "HexCoder.hpp"
#include "WindowManager.hpp"
#include "HashAlgorithms/md5.hpp"
#include "HashAlgorithms/SHA256.hpp"
#pragma once

typedef chrono::duration<double> fsec;

enum class HashAlgorithm {
  None, MD5
};

enum class OperationType {
  Text, File
};

enum class EncryptionMetod {
  Pass, Actions, Both
};

struct Params {
  string path;
  string actionPath;
  bool mode = true;
  OperationType type = OperationType::Text;
  EncryptionMetod method = EncryptionMetod::Pass;
  HashAlgorithm hAlg = HashAlgorithm::MD5;
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
  WindowManager wm;
  HexCoder hc;
  size_t blockSize = 1Ui64 << 28Ui64,
    threadCount = thread::hardware_concurrency();

  void readFilePth(const size_t n_thread, string& file, const string& path,
    const size_t start, const size_t partStart, const size_t partEnd, bool mode);
  void byTyping(string& message);
  void setBlockSize();
  void enterPass();
  void toHexString(string& str);
  void codeFile(bool mode);
  void codeText(bool mode);
  void fromHexString(string& str);
  void setOpenDlgTitle(bool mode);
  void fileMenu();
  void actionsMenu();
  void settingsMenu();
  void mainMenu();
};

