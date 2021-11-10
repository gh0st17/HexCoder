#include "Manager.hpp"

#define SLEEP_TIMEOUT 1500

Manager::Manager(Params& params) {
	path = params.path;

	if (params.method == EncryptionMetod::Pass)
		enterPass();
	else if (params.method == EncryptionMetod::Actions)
		params.actionPath.empty() ?
			actionsMenu() :
			insts.readInstructions(params.actionPath);
	else {
		enterPass();
		params.actionPath.empty() ?
			actionsMenu() :
			insts.readInstructions(params.actionPath);
	}

	if (params.type == OperationType::Text)
		codeText(params.mode);
	else if (params.type == OperationType::File && !path.empty())
		codeFile(params.mode);
	else
		cout << "File path did not set! Exiting.\n";
}

Manager::Manager() {
	mainMenu();
}

Manager::~Manager() {
	memset(&pass[0], 0, pass.size());
}

void Manager::readFilePth(const size_t n_thread, string& file, const string& path,
		const size_t start, const size_t partStart, const size_t partEnd, bool mode) {
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
	cout << "Thread " << n_thread << ": " << (partStart ? partStart + 1 : 0);
	cout << "->" << partEnd << " bytes\n";
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
	cout << "Default is 28 (256 MiB). Minimum block size is ";
	cout << threadCount << " bytes\n>>> ";
	uint16_t powerOfTwo = 64;
	while (powerOfTwo > 63 || (1Ui64 << powerOfTwo) < threadCount) {
		cin >> powerOfTwo;
	}
	blockSize = 1Ui64 << powerOfTwo;
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
	if (params.hAlg == HashAlgorithm::MD5)
		pass = md5(pass1);
	else if (params.hAlg == HashAlgorithm::SHA256) {
		SHA256 sha;
		sha.update(pass1);
		uint8_t* digest = sha.digest();
		pass = SHA256::toString(sha.digest());
		delete[] digest;
	}
	else if (params.hAlg == HashAlgorithm::SHA512)
		pass = sha512(pass1);
	else
		pass = pass1;
	memset(&pass1[0], 0, pass1.size());
	memset(&pass2[0], 0, pass2.size());
	cout << "\nPassword was set\n";
	Sleep(SLEEP_TIMEOUT);
}

void Manager::toHexString(string& str) {
	stringstream res;
	for (const auto& x : str)
		res << uppercase << hex << (unsigned short)(unsigned char)(x) << ' ';
	str = res.str();
	str.pop_back();
}

void Manager::codeFile(bool mode) {
	wm.setTitle(mode, Dialog::file);
	ifstream ifs(path, ifstream::binary);
	if (!ifs) {
		cerr << "Error opening file! Canceled.\n";
		_getch();
		return;
	}
	ifs.seekg(0, ifs.end);
	size_t fileSize = ifs.tellg();
	ifs.close();
	size_t partSize = blockSize / threadCount,
		currentBlockSize = blockSize, blocksCount = 0;
	if (blockSize > fileSize + threadCount) {
		blockSize = fileSize;
		cout << "Warning: block size < file size, ";
		cout << "changing block size to file size.\n";
	}
	while (blocksCount * blockSize < fileSize)
		blocksCount++;

	try {
		string file = string(blockSize, '\0');
		if (!mode && path.substr(path.size() - 3) != "hcf")
			throw "File extension not 'hcf'";

		cout << "   File size: " << fileSize << " bytes\n";
		cout << "  Block size: " << blockSize << " bytes\n";
		cout << "   Part size: " << partSize << " bytes\n";
		cout << "Blocks count: " << blocksCount << endl;
		cout << "Please wait...\n";

		size_t start = 0, end, partStart = 0, partEnd, sum = 0;

		auto t1 = chrono::high_resolution_clock::now();
		string outPath = (mode ? path + ".hcf" : path.substr(0, path.size() - 4));

		auto getEnd = [](size_t& i, size_t& b, size_t& start, size_t& threadCount,
			size_t& fileSize, size_t& blocksCount, size_t& partSize, size_t& bs) {
				if (b == blocksCount - 1) {
					if (i == threadCount - 1)
						return fileSize;
					else
						return start + (fileSize - (bs * b)) / threadCount;
				}
				else if (i == threadCount - 1)
					return bs + (bs * b);
				else
					return partSize * (i + 1) + (bs * b);
		};

		auto getPartEnd = [](size_t& i, size_t& b, size_t& partStart, size_t& threadCount,
			size_t& fileSize, size_t& blocksCount, size_t& partSize, size_t& bs) {
				if (b == blocksCount - 1) {
					if (i == threadCount - 1)
						return fileSize - (bs * b);
					else
						return partStart + (fileSize - (bs * b)) / threadCount;
				}
				else if (i == threadCount - 1)
					return bs;
				else
					return partStart + partSize;
		};

		ofstream ofs(outPath, ofstream::binary);
		vector<thread> t;
		for (size_t b = 0; b < blocksCount; b++) {
			for (size_t i = 0; i < threadCount; i++) {

				end = getEnd(i, b, start, threadCount, fileSize,
					blocksCount, partSize, blockSize);

				partEnd = getPartEnd(i, b, partStart, threadCount,
					fileSize, blocksCount, partSize, blockSize);

				sum += end - start;
				t.push_back(thread(&Manager::readFilePth, this,
					i + 1, ref(file), path, start, partStart,
					partEnd, mode));

				start ^= end;
				end ^= start;
				start ^= end;

				partStart ^= partEnd;
				partEnd ^= partStart;
				partStart ^= partEnd;
			}
			partStart = 0;
			m_locker.lock();
			cout << "Reading block " << b + 1 << "...\n";
			m_locker.unlock();
			for_each(t.begin(), t.end(), mem_fn(&thread::join));
			cout << "\r\b";
			t.clear();

			cout << "Writing block " << b + 1 << " to file...\n";
			if (b == blocksCount - 1)
				ofs.write(&file[0], fileSize - (blockSize * b));
			else
				ofs << file;
		}
		ofs.close();

		auto t2 = chrono::high_resolution_clock::now();
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
		memset(&file[0], 0, file.size());
		memset(&path[0], 0, path.size());
		file.clear();
		path.clear();
	}
	catch (bad_alloc const&) {
		cerr << "Can't allocate memory size " << blockSize << " bytes.\n";
		cerr << "Try reduce block size.\n";
	}
	catch (exception e) {
		cerr << e.what() << endl;
	}
	catch (char* c) {
		cerr << c << endl;;
	}
	_getch();
}

void Manager::codeText(bool mode) {
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
	wm.copyDlg(message.c_str());
}

void Manager::fromHexString(string& str) {
	unsigned short c;
	stringstream ss(str);
	string result = "";
	while (ss >> hex >> c)
		result.push_back((char)c);
	str = result;
}

void Manager::setOpenDlgTitle(bool mode){
		string title;
		if (mode) title = "Encrypt file...";
		else      title = "Decrypt file...";
		wm.openMessage(path, mode, title.c_str());
}

void Manager::fileMenu() {
	char ch;
	bool exit = 0;
	while (!exit) {
		wm.setTitle(0, Dialog::file);
		ch = _getch();
		if (ch == '1') {
			setOpenDlgTitle(true);
			codeFile(true);
		}
		else if (ch == '2') {
			setOpenDlgTitle(false);
			codeFile(false);
		}
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
			memset(&insts.acts[0], 0, sizeof(Action) * insts.acts.size());
			cout << "\nActions was unset\n";
			Sleep(SLEEP_TIMEOUT);
		}
		else if (ch == '3') {
			wm.openMessage(path, 2, "Load actions...");
			if (!path.empty()) {
				insts.readInstructions(path);
				memset(&path[0], 0, path.size());
				cout << "\nActions was read\n";
				Sleep(750);
			}
		}
		else if (ch == '4') {
			if (!insts.acts.empty()) {
				wm.openMessage(path, 3, "Save actions...");
				if (!path.empty()) {
					insts.writeInstructions(path);
					memset(&path[0], 0, path.size());
					cout << "\nActions written\n";
					Sleep(750);
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
			memset(&pass[0], 0, pass.size());
			cout << "\nPassword was unset\n";
			Sleep(SLEEP_TIMEOUT);
		}
		else if (ch == '4') {
			setBlockSize();
			cout << "\nBlock size was set to " << blockSize << endl;
			Sleep(SLEEP_TIMEOUT);
		}
		else if (ch == '5') {
			cout << "Available hash algorithms: None, MD5, SHA256, SHA512. ";
			cout << "Default is SHA256.\n>>> ";
			auto toString = [](HashAlgorithm hAlg) {
				if (hAlg == HashAlgorithm::MD5)
					return "MD5";
				else if (hAlg == HashAlgorithm::SHA256)
					return "SHA256";
				else if (hAlg == HashAlgorithm::SHA512)
					return "SHA512";
				else
					return "None";
			};
			string str = "";
			while (hAlgs.find(str) == hAlgs.end())
				cin >> str;
			if (str == "MD5")
				params.hAlg = HashAlgorithm::MD5;
			else if (str == "SHA256")
				params.hAlg = HashAlgorithm::SHA256;
			else if (str == "SHA512")
				params.hAlg = HashAlgorithm::SHA512;
			else if (str == "None")
				params.hAlg = HashAlgorithm::None;
			cout << "\nHash algorithm was set to " << toString(params.hAlg) << endl;
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
			codeText(true);
		else if (ch == '2')
			codeText(false);
		else if (ch == '3')
			fileMenu();
		else if (ch == '4')
			settingsMenu();
		else
			exit = 1;
	}
}