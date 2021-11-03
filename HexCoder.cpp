#include "HexCoder.hpp"

//#define debug

HexCoder::HexCoder(string inputString, typeData typeD)					//Конструктор кодера
{
	//ctor
	code(inputString, typeD);
	if (typeD == CHIP) chipString = inputString;
	else if (typeD == PLAIN) plainString = inputString;
}

HexCoder::HexCoder(vector<char> &inputString, typeData typeD)			//Конструктор кодера const char
{
	//ctor
	code(inputString, inputString.size(), typeD);
	if (typeD == CHIP) chipString = charArrToString(inputString);
	else if (typeD == PLAIN) plainString = charArrToString(inputString);
}

string HexCoder::getString(typeData typeD) {							//Получить hex или чистую строку
	if (typeD == CHIP) return chipString;
	else return plainString;
}

vector<char> HexCoder::getCString(typeData typeD){						//Получить hex или чистую строку
	if (typeD == CHIP) return stringToCharArr(chipString);
	else return stringToCharArr(plainString);
}

void HexCoder::code(string s, typeData typeD) {							//Кодируем строки
	string result = s;
	vector<char> key;
	if (typeD == PLAIN) key = buildingKey(s.size(), 1);
	else if (typeD == CHIP) key = buildingKey(s.size(), 0);
	for (size_t i = 0; i < s.size(); i++) result[i] ^= key[i];

	if (typeD == PLAIN) chipString = result;
	else if (typeD == CHIP) plainString = result;
}

void HexCoder::code(vector<char> &s, size_t s_size, typeData typeD){	//Кодируем строки
	string result = charArrToString(s);
	vector<char> key;
	if (typeD == PLAIN) key = buildingKey(s_size, 1);
	else if (typeD == CHIP) key = buildingKey(s_size, 0);
	for (size_t i = 0; i < s_size; i++) result[i] ^= key[i];

	if (typeD == PLAIN) chipString = result;
	else if (typeD == CHIP) plainString = result;
}

vector<string> HexCoder::split(string str, char delimiter) {			//Разбиение строки в вектор
	vector<string> internal;
	stringstream ss(str); // Turn the string into a stream.
	string tok;
	while (getline(ss, tok, delimiter)) {
		internal.push_back(tok);
	}
	return internal;
}

vector<vector<char> > HexCoder::splitByKey(string str, size_t keyLength) {
	vector<vector<char> > internal;
	internal.resize(keyLength);
	for (size_t i = 0; i < str.size(); i++){
		internal[i % keyLength].push_back(str[i]);
#ifdef Debug
		printf("%u %u %d\n", i % keyLength, i, str[i]);
	}
	system("pause");
#else
	}
#endif
	return internal;
}

vector<char> HexCoder::stringToCharArr(string &str){
	vector<char> charArr(str.size());
	for (size_t i = 0; i < charArr.size(); i++) charArr[i] = str[i];
	return charArr;
}

string HexCoder::charArrToString(vector<char> &charArr){
	string s;
	for (size_t i = 0; i < charArr.size(); i++) s.push_back(charArr[i]);
	return s;
}

vector<char> HexCoder::buildingKey(size_t size, bool mode){
	vector<char> key;
	size_t c;

	for (size_t i = 0; i < size + 1; i++){
		if (i < (size + 1) / 2){
			c = size & i;
			c |= (i + 1);
		}
		else{
			c = size | i;
			c &= (i + 1);
		}
		key.push_back((char)c);
		char k = key[key.size() - 1];
		if (mode) k = k >> 4 | k << 4;
		else k = k << 4 | k >> 4;
		key[key.size() - 1] = k;
	}
	return key;
}