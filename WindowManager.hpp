#pragma once
#include "libs.hpp"

enum class Dialog {
  mainMenu,
  settings,
  fileEn,
  file,
  text,
  actions
};

class WindowManager {

public:
  WindowManager();
  ~WindowManager() {};
  void copyText(string& s);
  void copyDlg(string& s);
  void openMessage(string& path, int mode, const char* title);
  string openBuffer();
  void setTitle(bool mode, Dialog d);

private:
  const string title = "HexCoder Processor v1.5.3 [By Ghost17] | ";

  void moveWindow();
};

