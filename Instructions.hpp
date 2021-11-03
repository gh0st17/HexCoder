#pragma once
#include "libs.hpp"

typedef vector< pair<uint8_t, size_t>> Actions;

class Instructions {
public:
	Actions acts;

	Instructions();
	~Instructions();
	void readInstructions();
	void writeInstructions();

private:
	bool validateSequence(string& seq);
};

