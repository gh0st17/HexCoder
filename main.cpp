#include "Manager.hpp"

int main(int argc, const char* argv[]) {
  if (argc > 1) {
    Params params(argc, argv);
    if (params.isCreate)
      Manager m;
    else if (params.isView)
      Manager m(params.actionPath);
    else
      Manager m(params);
  }
  else
    Params::printHelp(argv[0]);
  return 0;
}