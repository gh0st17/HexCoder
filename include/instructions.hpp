#pragma once
#include <libs.hpp>

typedef pair<char, uint8_t> Action;
typedef vector<Action> Actions;

class Instructions {

public:
	Instructions() {}
	~Instructions();
	void createInstructions();
	void readInstructions(const string& path);
	void writeInstructions(const string& path);
	void viewInstructions();
	void applyActions(string& data, const size_t start, const size_t end);
	void reverseActions(string& data, const size_t start, const size_t end);
	const size_t getActionsCount();

private:
	Actions acts;
	const map<char, char (*)(char, uint8_t)> Ops {
		{'^', [](char ch, uint8_t val) { return (char)(ch ^ val); }},
		{'<', [](char ch, uint8_t val) { return (char)(ROL8(ch, val)); }},
		{'>', [](char ch, uint8_t val) { return (char)(ROR8(ch, val)); }},
		{'~', [](char ch, uint8_t val) { return (char)(~ch); }},
		{'+', [](char ch, uint8_t val) { return (char)(ch + val); }},
	};
	const map<char, char (*)(char, uint8_t)> revOps {
		{'^', [](char ch, uint8_t val) { return (char)(ch ^ val); }},
		{'<', [](char ch, uint8_t val) { return (char)(ROR8(ch, val)); }},
		{'>', [](char ch, uint8_t val) { return (char)(ROL8(ch, val)); }},
		{'~', [](char ch, uint8_t val) { return (char)(~ch); }},
		{'+', [](char ch, uint8_t val) { return (char)(ch - val); }},
	};
	bool validateOp(const char& op);
	void zero();
};