#include "Instructions.hpp"
#pragma once

class HexCoder {

public:
	void code(string& data, string& pass, size_t start, size_t end);
	void code(string& data, Instructions& insts,
									 size_t start, size_t end, bool mode);
	void code(string& data, string& pass, size_t start, size_t end,
									 Instructions& insts, bool mode);
};