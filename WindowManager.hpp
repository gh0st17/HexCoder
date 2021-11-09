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
  void copyText(const char* s);
  void copyDlg(const char* s);
  void openMessage(string& path, int mode, const char* title);
  string openBuffer();
  void setTitle(bool mode, Dialog d);

private:
  const string title = "HexCoder Processor v1.5.4 [By Ghost17] | ";

  void moveWindow();
};

