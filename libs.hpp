#include <functional>
#include <filesystem>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <map>
#include <set>

#ifdef _MSC_VER
#include <intrin.h>
#define ROL8 _rotl8
#define ROR8 _rotr8
#else
#include <x86intrin.h>
#define ROL8 _rolb
#define ROR8 _rorb
#endif

using namespace std;