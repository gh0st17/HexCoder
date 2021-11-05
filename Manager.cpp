#include "Manager.hpp"

#define SLEEP_TIMEOUT 1500

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
		size_t start, size_t partStart, size_t partEnd, bool mode) {
	ifstream ifs(path, ifstream::binary);
	ifs.seekg(start);
	ifs.read(&file[partStart], (partEnd - partStart));
	ifs.close();
	if (!pass.empty() && insts.acts.empty())
		hc.code(file, pass, partStart, partEnd);
	else if (!pass.empty() && !insts.acts.empty())
		hc.code(file, pass, partStart, partEnd, insts, mode);
	else if(pass.empty() && !insts.acts.empty())
		hc.code(file, insts, partStart, partEnd, mode);
	m_locker.lock();
	cout << "Thread " << n_thread << " read from " << (partStart ? partStart + 1 : 0);
	cout << " to " << partEnd << " bytes\n";
	m_locker.unlock();
}

void Manager::byTyping(string& message) {
	cout << "Enter message: ";
	setlocale(LC_CTYPE, ".866");
	while (message.empty())
		getline(cin, message);
	setlocale(LC_CTYPE, ".1251");
	cout << '\n';
}

void Manager::setBlockSize() {
	cout << "Enter block size as power of 2.\nFor example 30 means 1GiB.\n";
	cout << "Default is 28 (256 MiB).\n>> ";
	uint16_t powerOfTwo = 64;
	while (powerOfTwo > 63) {
		cin >> powerOfTwo;
	}
	blockSize = 1 << powerOfTwo;
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
	Sleep(SLEEP_TIMEOUT);
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
			currentBlockSize = blockSize;
		if (blockSize > fileSize) {
			blockSize = fileSize;
			cout << "Warning: block size < file size, ";
			cout << "changing block size to file size.\n";
		}
		size_t threadCount = thread::hardware_concurrency(),
			partSize = blockSize / threadCount, blocksCount = 0;
		while (blocksCount * blockSize < fileSize)
			blocksCount++;
		ifs.close();

		try {
			string file = string(blockSize, '\0');
			if (!mode && path.substr(path.size() - 3) != "hcf")
				throw "File extension not 'hcf'";

			cout << "File size: " << fileSize << " bytes\n";
			cout << "Block size: " << blockSize << " bytes\n";
			cout << "Part size: " << partSize << " bytes\n";
			cout << "Please wait...\n";

			size_t start = 0, end, partStart = 0, partEnd, sum = 0, psum = 0;

			auto t1 = chrono::high_resolution_clock::now();
			string outPath = (mode ? path + ".hcf" : path.substr(0, path.size() - 4));
			filesystem::remove(outPath);
			ofstream ofs(outPath, ofstream::binary);
			for (size_t b = 0; b < blocksCount; b++) {
				vector<thread> t;
				for (size_t i = 0; i < threadCount; i++) {

					end = [i, b, start, threadCount, fileSize,
						blocksCount, partSize](size_t& bs) {

						if (b == blocksCount - 1) {
							if (i == threadCount - 1)
								return fileSize;
							else
								return start + partSize;
						}
						else if (i == threadCount - 1) 
								return bs + (bs * b);
						else
							return partSize * (i + 1) + (bs * b);
					}(blockSize);

					partEnd = [i, b, partStart, threadCount,
						blocksCount, partSize, start, end](size_t& bs) {

						if (b == blocksCount - 1) {
							if (i == threadCount - 1)
								return partStart + (end - start);
							else
								return partStart + partSize;
						}
						else if (i == threadCount - 1)
							return bs;
						else
							return partStart + partSize;
					}(blockSize);

					sum += end - start;
					psum += partEnd - partStart;
					t.push_back(thread(&Manager::readFilePth, this,
						i + 1, ref(file), path, start,
						partStart, partEnd, mode));
					//cout << "Thread " << i + 1 << " read from " << (start ? start + 1 : 0);
					//cout << " to " << end << " bytes pStart " << partStart << " pEnd " << partEnd << endl;
					
					start ^= end;
					end ^= start;
					start ^= end;

					partStart ^= partEnd;
					partEnd ^= partStart;
					partStart ^= partEnd;
				}
				partStart = 0;
				for_each(t.begin(), t.end(), mem_fn(&thread::join));

				cout << "Writing block " << b + 1 << " to file...\n";
				ofs << file;
			}
			ofs.close();

			auto t2 = std::chrono::high_resolution_clock::now();
			fsec duration = t2 - t1;

			if (currentBlockSize > fileSize) {
				blockSize = currentBlockSize;
				cout << "Warning: block size returned to ";
				cout << blockSize << " bytes.\n";
			}
			setlocale(LC_CTYPE, ".1251");
			cout << (mode ? "\nEncrypted " : "\nDecrypted ") << sum << " bytes for ";
			cout << setprecision(5) << duration.count() << " seconds!\nFile path is " << outPath << endl;
			setlocale(LC_CTYPE, ".866");
			file.clear();
			path.clear();
		}
		catch (bad_alloc const&) {
			cerr << "Can't allocate memory size " << blockSize << " bytes." << endl;
			cerr << "Try reduce block size.\n";
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
		hc.code(message, pass, 0, message.size());
	else if (!pass.empty() && !insts.acts.empty())
		hc.code(message, pass, 0, message.size(), insts, mode);
	else if(pass.empty() && !insts.acts.empty())
		hc.code(message, insts, 0, message.size(), mode);

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
		wm.setTitle(0, Dialog::actions);
		ch = _getch();
		if (ch == '1')
			insts.createInstructions();
		else if (ch == '2') {
			memset(&insts.acts, 0, sizeof insts.acts);
			cout << "\nActions was unset\n";
			Sleep(SLEEP_TIMEOUT);
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
				Sleep(SLEEP_TIMEOUT);
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
		wm.setTitle(0, Dialog::settings);
		ch = _getch();
		if (ch == '1')
			actionsMenu();
		else if (ch == '2')
			enterPass();
		else if (ch == '3') {
			memset(&pass, 0, sizeof pass);
			cout << "\nPassword was unset\n";
			Sleep(SLEEP_TIMEOUT);
		}
		else if (ch == '4') {
			setBlockSize();
			cout << "\nBlock size was set to " << blockSize << endl;
			Sleep(SLEEP_TIMEOUT);
		}
		else
			exit = 1;
	}
}

void Manager::mainMenu() {
	char ch;
	bool exit = 0;
	while (!exit) {
		wm.setTitle(0, Dialog::mainMenu);
		ch = _getch();

		if ((ch == '1' || ch == '2' || ch == '3') &&
					(pass.empty() && insts.acts.empty())) {
			cout << "Set password or actions first!\n";
			Sleep(SLEEP_TIMEOUT);
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