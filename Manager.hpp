#include "HexCoder.hpp"
#include "WindowManager.hpp"
#pragma once

typedef chrono::duration<double> fsec;

class Manager {

public:
  Manager(int argc, char* argv[]);
  ~Manager();

private:
  mutex m_locker;
  string path, pass;
  Instructions insts;
  WindowManager wm;
  HexCoder hc;
  size_t blockSize = 1 << 28;

  void readFilePth(size_t n_thread, string& file, string path,
    size_t start, size_t partStart, size_t partEnd, bool mode);
  void byTyping(string& message);
  void setBlockSize();
  void enterPass();
	void toHexString(string& str);
	void code(bool mode, bool isDrag);
  void code(bool mode);
	void fromHexString(string& str);
	void fileMenu();
  void actionsMenu();
  void settingsMenu();
	void mainMenu();
};

