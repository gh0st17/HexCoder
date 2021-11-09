#include "Instructions.hpp"
#pragma once

class HexCoder {

public:
	void code(string& data, const string& pass,
		const size_t start, const size_t end);
	void code(string& data, Instructions& insts,
		const size_t start, const size_t end, bool mode);
	void code(string& data, const string& pass,
		const size_t start, const size_t end,
		Instructions& insts, bool mode);
};