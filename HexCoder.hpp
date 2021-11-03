#include "libs.hpp"

using namespace std;

class HexCoder
{
public:
	enum typeData { CHIP, PLAIN };
	HexCoder(){};
	~HexCoder(){};
	HexCoder(string inputString, typeData typeD);
	HexCoder(vector<char> &inputString, typeData typeD);
	string getString(typeData typeD);
	char * getCharPtrString(typeData typeD);
	vector<char> getCString(typeData typeD);

private:
	string chipString, plainString;
	vector<string> split(string str, char delimiter);
	vector<vector<char> > splitByKey(string str, size_t keyLength);
	vector<char> stringToCharArr(string &str);
	string charArrToString(vector<char> &charArr);
	vector<char> buildingKey(size_t size, bool mode);
	void code(string s, typeData typeD);
	void code(vector<char> &s, size_t s_size, typeData typeD);
};