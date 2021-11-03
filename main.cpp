#include "process.hpp"

using namespace std;

int main(int argc, char* argv[]) {
	setlocale(LC_CTYPE, ".866");
	omp_set_dynamic(0);
	omp_set_num_threads(omp_get_max_threads());
	system("color 0a");
	moveWindow();
	if (argc == 2){
		string p(argv[1]);
		path = p;
		p = p.substr(p.size() - 3);
		if (p == "hcf") code(false, true);
		else code(true, true);	
	}
	else mainMenu();
	return 0;
}