#include "HexCoder.hpp"
#pragma once

typedef chrono::duration<double> fsec;

class Manager {

public:
  Manager(int argc, char* argv[]);
  ~Manager();

private:
  string path, pass;
  Instructions insts;
  void copyText(string& s);
  void copyDlg(string& s);
  void moveWindow();
  void openMessage(string& path, int mode, const char* title);
  string openBuffer();
  void readFilePth(size_t n_thread, string& file, string path,
                   size_t start, size_t end, size_t fileSize, bool mode);
  void byTyping(string& message);
  void enterPass();
	void toHexString(string& str);
	void code(bool mode, bool isDrag);
  void code(bool mode);
	void fromHexString(string& str);
	void fileMenu();
	void mainMenu();
};

