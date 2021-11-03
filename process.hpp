#include "HexCoder.hpp"

using namespace std;

/*********** COMMON ***********/

inline void copyText(string &s) {
	const size_t len = strlen(s.c_str()) + 1;
	HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, len);
	memcpy(GlobalLock(hMem), s.c_str(), len);
	GlobalUnlock(hMem);
	OpenClipboard(0);
	EmptyClipboard();
	SetClipboardData(CF_TEXT, hMem);
	CloseClipboard();
}

inline void copyDlg(string &s) {
	printf("1. Copy to clipboard\nAny key - no\n\n");
	char ch = _getch();
	if (ch == '1') copyText(s);
}

inline void moveWindow() {
	unsigned Width = GetSystemMetrics(SM_CXSCREEN);
	unsigned Height = GetSystemMetrics(SM_CYSCREEN);
	unsigned WindowWidth = 680;
	unsigned WindowHeight = 350;
	unsigned NewWidth = (Width - WindowWidth) / 2;
	unsigned NewHeight = (Height - WindowHeight) / 2;
	HWND hWnd = GetConsoleWindow();
	MoveWindow(hWnd, NewWidth, NewHeight, WindowWidth, WindowHeight, TRUE);
}

inline vector<string> split(string str, char delimiter) {
	vector<string> internal;
	stringstream ss(str);
	string tok;
	while (getline(ss, tok, delimiter)) internal.push_back(tok);
	return internal;
}

void openMessage(string &path, int mode, const char * title) {
	OPENFILENAME ofn;
	char filterSpec[2][MAX_PATH] = { "HexCoder File(*.hcf)\0*.hcf\0All files(*.*)\0*.*\0",
									"All files(*.*)\0*.*\0HexCoder File(*.hcf)\0*.hcf\0"};
	char fileName[MAX_PATH] = "";
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = NULL;
	ofn.lpstrFilter = filterSpec[mode];
	ofn.lpstrFile = fileName;
	ofn.lpstrTitle = title;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	ofn.lpstrDefExt = "";
	char temp[_MAX_PATH + 1];
	GetModuleFileName(NULL, temp, _MAX_PATH);
	string appPath = temp;
	appPath = appPath.substr(0, appPath.find_last_of("\\/"));
	ofn.lpstrInitialDir = appPath.c_str();

	if (GetOpenFileName(&ofn))
		path  = fileName;
	else
		cout << "File not open!" << '\n';
}

inline string openBuffer() {
	HANDLE h;
	if (!OpenClipboard(NULL))
		return "Failed to open buffer!";
	else {
		h = GetClipboardData(CF_TEXT);
		CloseClipboard();
		return (char *)h;
	}
}

void writeFile(ofstream & fout, string &data) {
	for (size_t i = 0; i < data.size(); i++){
		fout.write(reinterpret_cast<const char*>(&data.c_str()[i]), sizeof(char));
	}
}

void readFile(FILE * ifs, ofstream & fout, size_t oldPartEnd, size_t partEnd) {
	fseek(ifs, oldPartEnd, SEEK_CUR);
	char * read = (char *)malloc(partEnd - oldPartEnd);
	fread(read, partEnd - oldPartEnd, 1, ifs);
	string s(read, partEnd - oldPartEnd);
	free(read);
	HexCoder * hc = new HexCoder(s, HexCoder::PLAIN);
	s = hc->getString(HexCoder::CHIP);
	delete hc;
	writeFile(fout, s);
}

inline void byTyping(string &message){
	printf("Enter message: ");
	getline(cin, message);
	cout << '\n';
}

inline size_t getPartEnd(	size_t fileSize, 
							size_t partSize,
							size_t targetPart,
							size_t lastPart){

	if (targetPart != lastPart) return partSize * targetPart;
	else return fileSize;
}

inline size_t getPartsCount(size_t fileSize){
	//1048576 - 1mb | 4194304 - 4mb | 8388608 - 8mb | 16777216 - 16mb | 33554432 - 32mb | 67108864 - 64mb
	if (fileSize <= 16777216) return 1;
	else return (size_t)ceil((float)fileSize / 16777216.0f);
}

string path;

/*********** ENCODE ***********/

inline void toHexString(string &str){
	stringstream res;
	for (size_t i = 0; i < str.size(); i++)
		res << hex << uppercase << bitset<8>(str[i]).to_ulong() << (i != str.size() - 1 ? " " : "");
	str = res.str();
}

void code(bool mode, bool isDrag = false){
	SetConsoleTitleA(mode ? "HexCoder Processor v1.3[By Ghost - 17] | Encrypting..." : "HexCoder Processor v1.3 [By Ghost-17] | Decrypting...");
	if (!isDrag) {
		string title;
		if (mode) title = "Encrypt file...";
		else      title = "Decrypt file...";
		openMessage(path, 1, title.c_str());
	}
	if (!path.empty()) {
		clock_t t1 = clock(), t2;
		FILE * ifs;
		fopen_s(&ifs, path.c_str(), "rb");
		if (!ifs) { printf("Error opening file! Exit\n"); _getch(); return; }
		if (mode) path += ".hcf";
		else path.erase(path.size() - 4, 4);
		ofstream fout(path, ios::binary);
		if (!fout){ printf("File not created! Exit\n"); return; }
		double oldProgress = 20.0;
		fseek(ifs, 0, SEEK_END);
		size_t	fileSize = ftell(ifs),
				partsCount = getPartsCount(fileSize),
				partSize = fileSize / partsCount;
		rewind(ifs);
		
		cout << "Please wait...";
		
#pragma omp parallel for ordered schedule(static, omp_get_num_threads()*2)
		for (long i = 1; i <= partsCount; i++){
#pragma omp ordered
			{
				readFile(ifs, fout, partSize * (i - 1), getPartEnd(fileSize, partSize, i, partsCount));
     		}
			double progress = 100.0 / (double)partsCount * (double)i;
			if (abs(progress - oldProgress) <= 5.0){
				cout << (unsigned)oldProgress << (oldProgress != 100.0 ? "%..." : "%");
				oldProgress += 20.0;
			}
		}

		fclose(ifs);
		fout.close();
		t2 = clock();
		setlocale(LC_CTYPE, ".1251");
		printf("\n%s %llu bytes for %.2f seconds!\nFile path is %s\n", (mode ? "Encrypted" : "Decrypted"), fileSize, (double)(t2 - t1) / CLOCKS_PER_SEC, path.c_str());
		setlocale(LC_CTYPE, ".866");
		path = "";
	}
	_getch();
}

void encode(){
	system("cls");
	SetConsoleTitleA("HexCoder Processor v1.3 [By Ghost-17] | Encrypt text menu");
	printf("1. Type on keyboard\n2. From Clipboard\nAny key to return to previous menu\n\n");
	char ch = getchar();
	string message;
	switch (ch){
	case '1':
		//byTyping(message);
		message = "lol lol lol";
		break;
	case '2':
		message = openBuffer();
		break;
	default:
		return;
		break;
	}
	HexCoder * hc = new HexCoder(message, HexCoder::PLAIN);
	message = hc->getString(HexCoder::CHIP);
	toHexString(message);
	printf("Encrypted message:\n%s\n\n", message.c_str());
	copyDlg(message);
}

/*********** DECODE ***********/

inline void fromHexString(string &str){
	vector<string> splitted = split(str, ' ');
	int ch;
	string result;
	for (size_t i = 0; i < splitted.size(); i++){
		stringstream ss;
		ss << hex << splitted[i];
		ss >> ch;
		result.push_back(ch);
	}
	str = result;
}

void decode(){
	system("cls");
	SetConsoleTitleA("HexCoder Processor v1.3 [By Ghost-17] | Decrypt text menu");
	printf("1. Type on keyboard\n2. From Clipboard\nAny key to return to previous menu\n\n");
	char ch = _getch();
	string message;
	switch (ch){
	case '1':
		byTyping(message);
		break;
	case '2':
		message = openBuffer();
		break;
	default:
		return;
		break;
	}
	fromHexString(message);
	HexCoder * hc = new HexCoder(message, HexCoder::CHIP);
	message = hc->getString(HexCoder::PLAIN);
	printf("Message:\n%s\n\n", message.c_str());
	copyDlg(message);
}

/*********** OTHER ***********/

void fileMenu(){
	char ch = _getch();
	bool exit = 0;
	while (!exit){
		SetConsoleTitleA("HexCoder Processor v1.3 [By Ghost-17] | File encryption menu");
		system("cls");
		printf("1. Encrypt\n2. Decrypt\nAny key to return to previous menu\n\n");
		ch = _getch();
		switch (ch){
		case '1':
			code(true);
			break;
		case '2':
			code(false);
			break;
		default:
			exit = 1;
			break;
		}
	}
}

void mainMenu(){
	char ch;
	bool exit = 0;
	while (!exit){
		SetConsoleTitleA("HexCoder Processor v1.3 [By Ghost-17] | Main menu");
		system("cls");
		printf("1. Encrypt text\n2. Decrypt text\n3. File encryption\nAny key to exit\n\n");
		ch = _getch();
		switch (ch){
		case '1':
			encode();
			break;
		case '2':
			decode();
			break;
		case '3':
			fileMenu();
			break;
		default:
			exit = 1;
			break;
		}
	}
}