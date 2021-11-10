#include "Manager.hpp"

Manager::Manager(Params& params) {
	if (params.method == EncryptionMetod::Pass)
		enterPass();
	else if (params.method == EncryptionMetod::Actions)
		insts.readInstructions(params.actionPath);
	else {
		enterPass();
		insts.readInstructions(params.actionPath);
	}

	if (!params.path.empty())
		codeFile(params.mode);
	else
		cout << "File path did not set! Exiting.\n";
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
		char c = getchar();
		while (c != 13) {
			pass1.push_back(c);
			cout << '*';
			c = getchar();
		}

		cout << "\nRe-enter password: ";
		c = getchar();
		while (c != 13) {
			pass2.push_back(c);
			cout << '*';
			c = getchar();
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
}

void Manager::codeFile(bool mode) {
	ifstream ifs(path, ifstream::binary);
	if (!ifs) {
		cerr << "Error opening file! Canceled.\n";
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
		cerr << c << endl;
	}
}