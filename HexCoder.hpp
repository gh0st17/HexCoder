#include "Instructions.hpp"
#pragma once

class HexCoder {

public:
	static void code(string& data, size_t start, size_t end, size_t dataSize);
	static void code(string& data, string& pass, size_t start, size_t end);
	static void code(string& data, Instructions& insts,
									 size_t start, size_t end, bool mode);
	static void code(string& data, string& pass, size_t start, size_t end,
									 Instructions& insts, bool mode);
};