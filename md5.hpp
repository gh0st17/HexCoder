// Code from https://github.com/ulwanski/md5
#include <string>
#include <cstring>
#pragma once

std::string md5(std::string dat);
std::string md5(const void* dat, size_t len);
