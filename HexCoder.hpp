#pragma once
#include "Instructions.hpp"

class HexCoder {

public:
	static void code(string& data, size_t start, size_t end, size_t dataSize);
	static void code(string& data, string& pass);
	static void code(string& data, string& pass, Instructions& insts);

private:
	static void applyActions(string& pass, Instructions& insts);
};