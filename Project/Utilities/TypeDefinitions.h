#pragma once

// Include Linked Libraries
#include <Windows.h>
#include <iostream>

#define Mat cv::Mat
#define FSTREAM std::fstream
#define IFSTREAM std::ifstream
#define OFSTREAM std::ofstream
#define EXIT(_code) exit(_code)
#define ITOB(_int) (CAST(bool, _int))
#define CAST_S(type, var) ((type)var)
#define CSTR(_string) (_string.c_str())
#define CAST(type, var) static_cast<type>(var)
#define CAST_C(type, var) const_cast<type>(var)
#define CAST_D(type, var) dynamic_cast<type>(var)
#define CAST_R(type, var) reinterpret_cast<type>(var)