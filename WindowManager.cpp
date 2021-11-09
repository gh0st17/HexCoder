#include "WindowManager.hpp"

WindowManager::WindowManager() {
	setlocale(LC_CTYPE, ".866");
	system("color 0a");
	moveWindow();
}

void WindowManager::copyText(string& s) {
	const size_t len = strlen(s.c_str()) + 1;
	HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, len);
	memcpy(GlobalLock(hMem), s.c_str(), len);
	GlobalUnlock(hMem);
	OpenClipboard(0);
	EmptyClipboard();
	SetClipboardData(CF_TEXT, hMem);
	CloseClipboard();
}

void WindowManager::copyDlg(string& s) {
	cout << "1. Copy to clipboard\nAny key - no\n\n";
	char ch = _getch();
	if (ch == '1') copyText(s);
}

void WindowManager::openMessage(string& path, int mode, const char* title) {
	OPENFILENAME ofn;

	const char filterSpec[4][MAX_PATH] = { "HexCoder File(*.hcf)\0*.hcf\0All files(*.*)\0*.*\0",
																				 "All files(*.*)\0*.*\0HexCoder File(*.hcf)\0*.hcf\0",
																				 "HexCoder Actions(*.hca)\0*.hca\0",
																				 "HexCoder Actions(*.hca)\0*.hca\0" };
	char fileName[MAX_PATH] = "";
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = NULL;
	ofn.lpstrFilter = filterSpec[mode];
	ofn.lpstrFile = fileName;
	ofn.lpstrTitle = title;
	ofn.nMaxFile = MAX_PATH;
	if (mode != 3)
		ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	else
		ofn.Flags = OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY;
	ofn.lpstrDefExt = "";
	char temp[_MAX_PATH + 1];
	GetModuleFileName(NULL, temp, _MAX_PATH);
	string appPath = temp;
	appPath = appPath.substr(0, appPath.find_last_of("\\/"));
	ofn.lpstrInitialDir = appPath.c_str();

	if (GetOpenFileName(&ofn))
		path = fileName;
	else
		cout << "File not open!\n";
}

string WindowManager::openBuffer() {
	HANDLE h;
	if (!OpenClipboard(NULL))
		return "Failed to open buffer!";
	else {
		h = GetClipboardData(CF_TEXT);
		CloseClipboard();
		return (char*)h;
	}
}

void WindowManager::setTitle(bool mode, Dialog d) {
	system("cls");
	if (d == Dialog::mainMenu) {
		SetConsoleTitleA((title + "Main menu").c_str());
		cout << "1. Encrypt text\n2. Decrypt text\n3. File encryption\n";
		cout << "4. Encryption settings\nAny key to exit\n\n";
	}
	else if (d == Dialog::file) {
		SetConsoleTitleA((title + "File encryption menu").c_str());
		cout << "1. Encrypt\n2. Decrypt\nAny key to return to previous menu\n\n";
	}
	else if (d == Dialog::fileEn) {
		if (mode)
			SetConsoleTitleA((title + "Encrypting...").c_str());
		else
			SetConsoleTitleA((title + "Decrypting...").c_str());
	}
	else if (d == Dialog::settings) {
		SetConsoleTitleA((title + "Settings menu").c_str());
		cout << "1. Actions\n2. Set password\n3. Unset password\n4. Set block size\n";
		cout << "5. Set hash algorithm\nAny key to return to previous menu\n\n";
	}
	else if (d == Dialog::text) {
		if (mode)
			SetConsoleTitleA((title + "Encrypt text menu").c_str());
		else
			SetConsoleTitleA((title + "Dectrypt text menu").c_str());

		cout << "1. Type on keyboard\n2. From Clipboard\nAny key to return to previous menu\n\n";
	}
	else if (d == Dialog::actions) {
		SetConsoleTitleA((title + "Actions menu").c_str());
		cout << "1. (Re-)Create actions\n2. Unset actions\n";
		cout << "3. Load actions\n4. Save actions\nAny key to return to previous menu\n\n";
	}
}

void WindowManager::moveWindow() {
	unsigned Width = GetSystemMetrics(SM_CXSCREEN);
	unsigned Height = GetSystemMetrics(SM_CYSCREEN);
	unsigned WindowWidth = 680;
	unsigned WindowHeight = 350;
	unsigned NewWidth = (Width - WindowWidth) / 2;
	unsigned NewHeight = (Height - WindowHeight) / 2;
	HWND hWnd = GetConsoleWindow();
	MoveWindow(hWnd, NewWidth, NewHeight, WindowWidth, WindowHeight, TRUE);
}