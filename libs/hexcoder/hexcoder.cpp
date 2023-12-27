#include <hexcoder.hpp>

void HexCoder::code(string& data, const string& pass,
		const size_t start, const size_t end) {
	for (size_t i = start; i < end; i++)
		data[i] ^= pass[(i % pass.size())];
}

void HexCoder::code(string& data, Instructions& insts,
		const size_t start, const size_t end, bool mode) {
	if (mode)
		insts.applyActions(data, start, end);
	else
		insts.reverseActions(data, start, end);
}

void HexCoder::code(string& data, const string& pass,
		const size_t start, const size_t end,
		Instructions& insts, bool mode) {
	if (mode)
		insts.applyActions(data, start, end);
	for (size_t i = start; i < end; i++) {
		data[i] ^= pass[(i % pass.size())];
	}
	if (!mode)
		insts.reverseActions(data, start, end);
}