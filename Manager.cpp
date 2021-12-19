#include "Manager.hpp"

Manager::Manager(const Params& params) {
	this->params = params;
	if (params.method == EncryptionMethod::Pass)
		enterPass();
	else {
		if (params.actionPath.empty()) {
			cout << "File path for actions did not set! Exiting.\n";
			return;
		}
		if (params.method == EncryptionMethod::Actions)
			insts.readInstructions(params.actionPath);
		else {
			enterPass();
			insts.readInstructions(params.actionPath);
		}
	}

	if (!params.path.empty())
		codeFile(params.mode);
	else
		cout << "File path did not set! Exiting.\n";
}

Manager::Manager(const string& actionPath) {
	if (actionPath.empty()) {
		cout << "File path for actions did not set! Exiting.\n";
		return;
	}
	insts.readInstructions(actionPath);
	insts.viewInstructions();
}

Manager::Manager() {
	actionsMenu();
}

Manager::~Manager() {
	fill(pass.begin(), pass.end(), 0);
}

void Manager::readFilePth(const size_t n_thread, string& file, const string& path,
		const size_t start, const size_t partStart, const size_t partEnd, bool mode, size_t b) {
	ifstream ifs(path, ifstream::binary);
	ifs.seekg(start);
	ifs.read(&file[partStart], (partEnd - partStart));
	ifs.close();
	if (!pass.empty() && !insts.getActionsCount())
		hc.code(file, pass, partStart, partEnd);
	else if (!pass.empty() && insts.getActionsCount())
		hc.code(file, pass, partStart, partEnd, insts, mode);
	else if(pass.empty() && insts.getActionsCount())
		hc.code(file, insts, partStart, partEnd, mode);
	if (params.verbose) {
		m_locker.lock();
		cout << "Thread " << n_thread << ": ";
		cout << (n_thread > 1 || b > 0 ? b * params.blockSize + partStart + 1 : 0);
		cout << "->" << b * params.blockSize + partEnd << " bytes\n";
		m_locker.unlock();
	}
}

void Manager::enterPass() {
	cout << "Enter password: ";
	cin >> pass;
	if (params.hAlg == HashAlgorithm::MD5)
		pass = md5(pass);
	else if (params.hAlg == HashAlgorithm::SHA256) {
		SHA256 sha;
		sha.update(pass);
		uint8_t* digest = sha.digest();
		pass = SHA256::toString(sha.digest());
		delete[] digest;
	}
	else if (params.hAlg == HashAlgorithm::SHA512)
		pass = sha512(pass);
}

void Manager::codeFile(bool mode) {
	ifstream ifs(params.path, ifstream::binary);
	if (!ifs) {
		cerr << "Error opening file! Canceled.\n";
		return;
	}
	ifs.seekg(0, ifs.end);
	size_t fileSize = ifs.tellg();
	ifs.close();
	size_t blocksCount = 0;
	if (params.blockSize > fileSize + params.threadsCount) {
		params.blockSize = fileSize;
		cout << "Warning: block size < file size, ";
		cout << "changing block size to file size.\n";
	}
	size_t partSize = params.blockSize / params.threadsCount;
	while (blocksCount * params.blockSize < fileSize)
		blocksCount++;

	try {
		string file = string(params.blockSize, '\0');
		string dt = params.path.substr(params.path.size() - 3);
		if (!mode && params.path.substr(params.path.size() - 3) != "hcf")
			throw "File extension not 'hcf'";

		cout << "         Mode: " << (params.mode ? "Encrypt\n" : "Decrypt\n");
		cout << "    Hash Alg.: " << params.getHashAlgorithmName() << endl;
		cout << "  Enc. method: " << params.getEncryptionMethodName() << endl;
		if (params.method != EncryptionMethod::Pass)
			cout << "   Acts count: " << insts.getActionsCount() << endl;
		cout << "    File size: " << fileSize << " bytes\n";
		cout << "   Block size: " << params.blockSize << " bytes\n";
		cout << "    Part size: " << partSize << " bytes\n";
		cout << " Blocks count: " << blocksCount << endl;
		cout << "Threads count: " << params.threadsCount << endl;
		cout << "Please wait...\n";

		string outPath = (mode ? params.path + ".hcf" :
			params.path.substr(0, params.path.size() - 4));

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

		size_t start = 0, end, partStart = 0, partEnd, sum = 0;

		auto t1 = chrono::high_resolution_clock::now();
		ofstream ofs(outPath, ofstream::binary);
		vector<thread> t;
		for (size_t b = 0; b < blocksCount; b++) {
			for (size_t i = 0; i < params.threadsCount; i++) {

				end = getEnd(i, b, start, params.threadsCount, fileSize,
					blocksCount, partSize, params.blockSize);

				partEnd = getPartEnd(i, b, partStart, params.threadsCount,
					fileSize, blocksCount, partSize, params.blockSize);

				sum += end - start;
				t.push_back(thread(&Manager::readFilePth, this,
					i + 1, ref(file), params.path, start, partStart,
					partEnd, mode, b));

				start ^= end;
				end ^= start;
				start ^= end;

				partStart ^= partEnd;
				partEnd ^= partStart;
				partStart ^= partEnd;
			}
			partStart = 0;
			m_locker.lock();
			cout << "Reading and processing block " << b + 1 << "...\n";
			m_locker.unlock();
			for_each(t.begin(), t.end(), mem_fn(&thread::join));
			t.clear();

			cout << "Writing block " << b + 1 << " to file...\n";
			if (b == blocksCount - 1)
				ofs.write(&file[0], fileSize - (params.blockSize * b));
			else
				ofs << file;
		}
		ofs.close();

		auto t2 = chrono::high_resolution_clock::now();
		fsec duration = t2 - t1;

		setlocale(LC_CTYPE, ".1251");
		cout << (mode ? "\nEncrypted " : "\nDecrypted ") << sum << " bytes for ";
		cout << setprecision(5) << duration.count() << " seconds!\nFile path is " << outPath << endl;
		fill(file.begin(), file.end(), 0);
		fill(params.path.begin(), params.path.end(), 0);
		file.clear();
		params.path.clear();
	}
	catch (bad_alloc const&) {
		cerr << "Can't allocate memory size " << params.blockSize << " bytes for block\n";
		cerr << "Try reduce block size";
	}
	catch (exception e) {
		cerr << e.what() << endl;
	}
	catch (char* c) {
		cerr << c << endl;
	}
}

void Manager::actionsMenu() {
	char ch;
	bool exit = 0;
	while (!exit) {
		cout << "1. (Re-)Create actions\n2. Save actions\nAny key to exit\n\n>>> ";
		cin >> ch;
		if (ch == '1')
			insts.createInstructions();
		else if (ch == '2') {
			if (!insts.getActionsCount()) {
				string filename;
				cout << "Enter file name: ";
				cin >> filename;
#ifdef _WIN64
				filename = filesystem::current_path().u8string() + '\\' + filename + ".hca";
#else
				filename = filesystem::current_path().u8string() + '/' + filename + ".hca";
#endif
				cout << filename;
				insts.writeInstructions(filename);
				cout << "\nActions written\n";
			}
			else {
				cout << "Actions not set\n";
			}
		}
		else
			exit = 1;
	}
}