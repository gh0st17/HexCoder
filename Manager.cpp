#include "Manager.hpp"

Manager::Manager(int argc, char* argv[]) {
	/*string s = "C:\\eded.hfc";
	string fileName = [s]() {
		auto p1 = s.find('\\');
		while (p1 != string::npos) {
			if (s.find('\\', ++p1) == string::npos) {
				break;
			}
			p1 = s.find('\\', ++p1);
		}
		return s.substr(p1);
	}();
	cout << fileName;*/
	if (argc == 2) {
		string p(argv[1]);
		path = p;
		p = p.substr(p.size() - 3);
		if (p == "hcf") code(false, true);
		else code(true, true);
	}
	else mainMenu();
}

Manager::~Manager() {
	memset(&pass, 0, sizeof pass);
}

void Manager::readFilePth(size_t n_thread, string& file, string path,
		size_t start, size_t end, size_t fileSize, bool mode) {
	ifstream ifs(path, ifstream::binary);
	ifs.seekg(start);
	ifs.read(&file[start], end - start);
	ifs.close();
	if (!pass.empty() && insts.acts.empty())
		HexCoder::code(file, pass, start, end);
	else if (!pass.empty() && !insts.acts.empty())
		HexCoder::code(file, pass, start, end, insts, mode);
	else if(pass.empty() && !insts.acts.empty())
		HexCoder::code(file, insts, start, end, mode);
	cout << "Thread " << n_thread << " read from " << (start ? start + 1 : 0);
	cout << " to " << end << " bytes\n";
}

void Manager::byTyping(string& message) {
	cout << "Enter message: ";
	setlocale(LC_CTYPE, ".866");
	while (message.empty())
		getline(cin, message);
	setlocale(LC_CTYPE, ".1251");
	cout << '\n';
}

void Manager::enterPass() {
	string pass1, pass2;
	while (pass1 != pass2 || pass1.empty() || pass2.empty()) {
		cout << "Enter password: ";
		char c = _getch();
		while (c != 13) {
			pass1.push_back(c);
			cout << '*';
			c = _getch();
		}

		cout << "\nRe-enter password: ";
		c = _getch();
		while (c != 13) {
			pass2.push_back(c);
			cout << '*';
			c = _getch();
		}
		if (pass1 != pass2) {
			cout << "\nPassword mismatch!\n";
			pass1 = pass2 = string();
		}
	}
	pass = pass1;
	memset(&pass1, 0, sizeof pass1);
	memset(&pass2, 0, sizeof pass2);
	cout << "\nPassword was set\n";
	Sleep(750);
}

void Manager::toHexString(string& str) {
	stringstream res;
	for (const auto& x : str)
		res << uppercase << hex << (unsigned)(unsigned char)(x) << ' ';
	str = res.str();
	str.pop_back();
}

void Manager::code(bool mode, bool isDrag) {
	wm.setTitle(mode, Dialog::file);
	if (!isDrag) {
		string title;
		if (mode) title = "Encrypt file...";
		else      title = "Decrypt file...";
		wm.openMessage(path, mode, title.c_str());
	}
	if (!path.empty()) {

		ifstream ifs(path, ifstream::binary);
		if (!ifs) {
			cerr << "Error opening file! Exit\n";
			_getch();
			return; 
		}
		ifs.seekg(0, ifs.end);
		size_t fileSize = ifs.tellg(),
			partsCount = thread::hardware_concurrency(),
			partSize = fileSize / partsCount, done = 0;
		ifs.close();

		try {
			if (!mode && path.substr(path.size() - 3) != "hcf")
				throw "File extension not 'hcf'";
			string file(fileSize, '\0');

			cout << "File size: " << fileSize << " bytes\n";
			cout << "Part size: " << partSize << " bytes\n";
			cout << "Please wait...\n";

			size_t start = 0, end, sum = 0;

			auto t1 = chrono::high_resolution_clock::now();
			vector<thread> t;
			for (size_t i = 0; i < partsCount; i++) {
				end = (i == partsCount - 1 ? fileSize : partSize * (i + 1));
				sum += end - start;
				t.push_back(thread(&Manager::readFilePth, this,
					i + 1, ref(file), path, start, end, fileSize, mode));
				start ^= end;
				end ^= start;
				start ^= end;
			}
			for_each(t.begin(), t.end(), mem_fn(&thread::join));

			cout << "Start writing to file...\n";
			if (mode) path += ".hcf";
			else path.erase(path.size() - 4, 4);
			ofstream ofs(path, ofstream::binary);
			ofs << file;
			ofs.close();

			auto t2 = std::chrono::high_resolution_clock::now();
			fsec duration = t2 - t1;

			setlocale(LC_CTYPE, ".1251");
			cout << (mode ? "\nEncrypted " : "\nDecrypted ") << sum << " bytes for ";
			cout << setprecision(5) << duration.count() << " seconds!\nFile path is " << path << endl;
			setlocale(LC_CTYPE, ".866");
			memset(&file, 0, sizeof file);
			memset(&path, 0, sizeof path);
		}
		catch (bad_alloc const&) {
			cerr << "Can't allocate memory size " << fileSize << " bytes" << endl;
		}
		catch (exception e) {
			cerr << e.what() << endl;
		}
		catch (char* c) {
			cout << c << endl;;
		}
	}
	else
		cout << "Path not defined\n";
	_getch();
}

void Manager::code(bool mode) {
	system("cls");
	wm.setTitle(mode, Dialog::text);

	char ch = _getch();
	string message;
	if (ch == '1')
		byTyping(message);
	else if (ch == '2')
		message = wm.openBuffer();
	else
		return;

	if (!mode)
		fromHexString(message);

	if (!pass.empty() && insts.acts.empty())
		HexCoder::code(message, pass, 0, message.size());
	else if (!pass.empty() && !insts.acts.empty())
		HexCoder::code(message, pass, 0, message.size(), insts, mode);
	else if(pass.empty() && !insts.acts.empty())
		HexCoder::code(message, insts, 0, message.size(), mode);

	if (mode)
		toHexString(message);

	setlocale(LC_CTYPE, ".866");
	cout << (mode ? "Encrypted message:\n" : "Message:\n") << message << "\n\n";
	setlocale(LC_CTYPE, ".1251");
	wm.copyDlg(message);
}

void Manager::fromHexString(string& str) {
	unsigned short c;
	stringstream ss(str);
	string result = "";
	while (ss >> hex >> c)
		result.push_back((char)c);
	str = result;
}

void Manager::fileMenu() {
	char ch;
	bool exit = 0;
	while (!exit) {
		system("cls");
		wm.setTitle(0, Dialog::file);
		ch = _getch();
		if (ch == '1')
			code(true, false);
		else if (ch == '2')
			code(false, false);
		else
			exit = 1;
	}
}

void Manager::actionsMenu() {
	char ch;
	bool exit = 0;
	while (!exit) {
		system("cls");
		wm.setTitle(0, Dialog::actions);
		ch = _getch();
		if (ch == '1')
			insts.createInstructions();
		else if (ch == '2') {
			memset(&insts.acts, 0, sizeof insts.acts);
			cout << "\nActions was unset\n";
			Sleep(750);
		}
		else if (ch == '3') {
			wm.openMessage(path, 2, "Load actions...");
			if (!path.empty()) {
				insts.readInstructions(path);
				memset(&path, 0, sizeof path);
			}
		}
		else if (ch == '4') {
			if (!insts.acts.empty()) {
				wm.openMessage(path, 3, "Save actions...");
				if (!path.empty()) {
					insts.writeInstructions(path);
					memset(&path, 0, sizeof path);
				}
			}
			else {
				cout << "Actions not set\n";
				Sleep(750);
			}
		}
		else
			exit = 1;
	}
}

void Manager::settingsMenu() {
	char ch;
	bool exit = 0;
	while (!exit) {
		system("cls");
		wm.setTitle(0, Dialog::settings);
		ch = _getch();
		if (ch == '1')
			actionsMenu();
		else if (ch == '2')
			enterPass();
		else if (ch == '3') {
			memset(&pass, 0, sizeof pass);
			cout << "\nPassword was unset\n";
			Sleep(750);
		}
		else
			exit = 1;
	}
}

void Manager::mainMenu() {
	char ch;
	bool exit = 0;
	while (!exit) {
		system("cls");
		wm.setTitle(0, Dialog::mainMenu);
		ch = _getch();

		if ((ch == '1' || ch == '2' || ch == '3') &&
					(pass.empty() && insts.acts.empty())) {
			cout << "Set password or actions first!\n";
			Sleep(750);
			continue;
		}

		if (ch == '1')
			code(true);
		else if (ch == '2')
			code(false);
		else if (ch == '3')
			fileMenu();
		else if (ch == '4')
			settingsMenu();
		else
			exit = 1;
	}
}