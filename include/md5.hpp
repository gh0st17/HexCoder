// Code from https://github.com/ulwanski/md5
#include <string>
#include <cstring>
#pragma once

using namespace std;

string md5(string dat);
string md5(const void* dat, size_t len);
