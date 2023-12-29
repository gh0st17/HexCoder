#include <manager.hpp>

int main(int argc, const char* argv[]) {
  if (argc > 1) {
    Params params(argc, argv);
    if (params.is_create)
      Manager m;
    else if (params.is_view)
      Manager m(params.action_path);
    else
      Manager m(params);
  }
  else
    Params::print_help(argv[0]);
  return 0;
}