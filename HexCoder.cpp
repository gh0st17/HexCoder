#include "HexCoder.hpp"

void HexCoder::code(string& data, size_t start, size_t end, size_t dataSize) {
	char c;
	for (size_t i = start; i < end; i++) {
		if (i < dataSize >> 1) {
			c = dataSize & i;
			c |= (i + 1);
		}
		else {
			c = dataSize | i;
			c &= (i + 1);
		}
		c = c >> 4 | c << 4;
		data[i] ^= c;
	}
}

void HexCoder::code(string& data, string& pass) {
	for (size_t i = 0; i < data.size(); i++)
		data[i] ^= pass[(i % pass.size())];
}

void HexCoder::code(string& data, string& pass, Instructions& insts) {
	for (size_t i = 0; i < data.size(); i++) {
		data[i] ^= pass[(i % pass.size())];
	}
}

void HexCoder::applyActions(string& pass, Instructions& insts) {
	size_t t = pass.size();
	while (t < (2 << 16))
		t = t << 2;
	char c;
	for (size_t i = 0; i < pass.size(); i++) {
		c = t;
		for (const auto& operand : insts.acts)
			if (operand.first == '|')
				c |= operand.second;
			else if (operand.first == '&')
				c &= operand.second;
		pass[i] ^= c;
	}
}