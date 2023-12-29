#include <manager.hpp>

Manager::Manager(const Params& params) {
	this->params = params;
	if (params.method == EncryptionMethod::Pass)
		enter_pass();
	else {
		if (params.action_path.empty()) {
			cout << "File path for actions did not set! Exiting.\n";
			return;
		}
		if (params.method == EncryptionMethod::Actions)
			insts.read_instructions(params.action_path);
		else {
			enter_pass();
			insts.read_instructions(params.action_path);
		}
	}

	if (!params.path.empty())
		code_file(params.mode);
	else
		cout << "File path did not set! Exiting.\n";
}

Manager::Manager(const string& actionPath) {
	if (actionPath.empty()) {
		cout << "File path for actions did not set! Exiting.\n";
		return;
	}
	insts.read_instructions(actionPath);
	insts.view_instructions();
}

Manager::Manager() {
	actions_menu();
}

Manager::~Manager() {
	fill(pass.begin(), pass.end(), 0);
}

void Manager::read_file_pth(const size_t n_thread, string& file, const string& path,
		const size_t start, const size_t part_start, const size_t part_end, bool mode, size_t b) {
	ifstream ifs(path, ifstream::binary);
	ifs.seekg(start);
	ifs.read(&file[part_start], (part_end - part_start));
	ifs.close();
	if (!pass.empty() && !insts.get_actions_count())
		hc.code(file, pass, part_start, part_end);
	else if (!pass.empty() && insts.get_actions_count())
		hc.code(file, pass, part_start, part_end, insts, mode);
	else if(pass.empty() && insts.get_actions_count())
		hc.code(file, insts, part_start, part_end, mode);
	if (params.verbose) {
		m_locker.lock();
		cout << "Thread " << n_thread << ": ";
		cout << (n_thread > 1 || b > 0 ? b * params.block_size + part_start + 1 : 0);
		cout << "->" << b * params.block_size + part_end << " bytes\n";
		m_locker.unlock();
	}
}

void Manager::enter_pass() {
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

void Manager::code_file(bool mode) {
	ifstream ifs(params.path, ifstream::binary);
	if (!ifs) {
		cerr << "Error opening file! Canceled.\n";
		return;
	}
	ifs.seekg(0, ifs.end);
	size_t file_size = ifs.tellg();
	ifs.close();
	size_t blocks_count = 0;
	if (params.block_size > file_size + params.threads_count) {
		params.block_size = file_size;
		cout << "Warning: block size < file size, ";
		cout << "changing block size to file size.\n";
	}
	size_t part_size = params.block_size / params.threads_count;
	while (blocks_count * params.block_size < file_size)
		blocks_count++;

	try {
		string file = string(params.block_size, '\0');
		string dt = params.path.substr(params.path.size() - 3);
		if (!mode && params.path.substr(params.path.size() - 3) != "hcf")
			throw "File extension is not 'hcf'";

		cout << "         Mode: " << (params.mode ? "Encrypt\n" : "Decrypt\n");
		if (params.method != EncryptionMethod::Actions)
			cout << "    Hash Alg.: " << params.get_hash_name() << endl;
		cout << "  Enc. method: " << params.get_enc_method_name() << endl;
		if (params.method != EncryptionMethod::Pass)
			cout << "   Acts count: " << insts.get_actions_count() << endl;
		cout << "    File size: " << file_size << " bytes\n";
		cout << "   Block size: " << params.block_size << " bytes\n";
		cout << "    Part size: " << part_size << " bytes\n";
		cout << " Blocks count: " << blocks_count << endl;
		cout << "Threads count: " << params.threads_count << endl;
		cout << "Please wait...\n";

		string out_path = (mode ? params.path + ".hcf" :
			params.path.substr(0, params.path.size() - 4));

		auto get_end = [](size_t& i, size_t& b, size_t& start, size_t& thread_count,
			size_t& file_size, size_t& blocks_count, size_t& part_size, size_t& bs) {
				if (b == blocks_count - 1) {
					if (i == thread_count - 1)
						return file_size;
					else
						return start + (file_size - (bs * b)) / thread_count;
				}
				else if (i == thread_count - 1)
					return bs + (bs * b);
				else
					return part_size * (i + 1) + (bs * b);
		};

		auto get_part_end = [](size_t& i, size_t& b, size_t& part_start, size_t& thread_count,
			size_t& file_size, size_t& blocks_count, size_t& part_size, size_t& bs) {
				if (b == blocks_count - 1) {
					if (i == thread_count - 1)
						return file_size - (bs * b);
					else
						return part_start + (file_size - (bs * b)) / thread_count;
				}
				else if (i == thread_count - 1)
					return bs;
				else
					return part_start + part_size;
		};

		size_t start = 0, end, part_start = 0, part_end, sum = 0;

		auto t1 = chrono::high_resolution_clock::now();
		ofstream ofs(out_path, ofstream::binary);
		vector<thread> t;
		for (size_t b = 0; b < blocks_count; b++) {
			for (size_t i = 0; i < params.threads_count; i++) {

				end = get_end(i, b, start, params.threads_count, file_size,
					blocks_count, part_size, params.block_size);

				part_end = get_part_end(i, b, part_start, params.threads_count,
					file_size, blocks_count, part_size, params.block_size);

				sum += end - start;
				t.push_back(thread(&Manager::read_file_pth, this,
					i + 1, ref(file), params.path, start, part_start,
					part_end, mode, b));

				start ^= end;
				end ^= start;
				start ^= end;

				part_start ^= part_end;
				part_end ^= part_start;
				part_start ^= part_end;
			}
			part_start = 0;
			m_locker.lock();
			cout << "Reading and processing block " << b + 1 << "...\n";
			m_locker.unlock();
			for_each(t.begin(), t.end(), mem_fn(&thread::join));
			t.clear();

			cout << "Writing block " << b + 1 << " to file...\n";
			if (b == blocks_count - 1)
				ofs.write(&file[0], file_size - (params.block_size * b));
			else
				ofs << file;
		}
		ofs.close();

		auto t2 = chrono::high_resolution_clock::now();
		fsec duration = t2 - t1;

		setlocale(LC_CTYPE, ".1251");
		cout << (mode ? "\nEncrypted " : "\nDecrypted ") << sum << " bytes for ";
		cout << setprecision(5) << duration.count() << " seconds!\nFile path is " << out_path << endl;
		fill(file.begin(), file.end(), 0);
		fill(params.path.begin(), params.path.end(), 0);
		file.clear();
		params.path.clear();
	}
	catch (bad_alloc const&) {
		cerr << "Can't allocate memory size " << params.block_size << " bytes for block\n";
		cerr << "Try reduce block size";
	}
	catch (exception const& e) {
		cerr << e.what() << endl;
	}
	catch (const char* c) {
		cerr << c << endl;
	}
}

void Manager::actions_menu() {
	char ch;
	bool exit = 0;
	while (!exit) {
		cout << "1. (Re-)Create actions\n2. Save actions\nAny key to exit\n\n>>> ";
		cin >> ch;
		if (ch == '1')
			insts.create_instructions();
		else if (ch == '2') {
			if (insts.get_actions_count()) {
				string filename;
				cout << "Enter file name: ";
				cin >> filename;
#ifdef _WIN64
				filename = filesystem::current_path().string() + '\\' + filename + ".hca";
#else
				filename = filesystem::current_path().string() + '/' + filename + ".hca";
#endif
				cout << filename;
				insts.write_instructions(filename);
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