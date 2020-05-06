//
// Created by 17110 on 2020/5/5.
//

#ifndef C0_SJX_GLOBAL_H
#define C0_SJX_GLOBAL_H

#include <algorithm>
#include <cctype>
#include <cstdarg>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#define CONSOLE_NAME "con"  // windows
//#define CONSOLE_NAME "/dev/tty"  // ubuntu

typedef unsigned int uint;
typedef unsigned long ulong;
typedef long long llong;
typedef unsigned long long ullong;
typedef long double ldouble;

namespace gb {

	// convert constant value to string
	template<typename Tp>
	inline std::string to_string(const Tp &value) {
		std::stringstream t_ss;
		//t_ss.setf(std::ios::fixed, std::ios::floatfield);
		t_ss << std::setprecision(16) << value;
		return t_ss.str();
	}

	// convert letters in string to lowercase
	inline std::string to_lower(const std::string &str) {
		std::string t_str(str);
		for (char &ch : t_str) {
			ch = tolower(ch);
		}
		return t_str;
	}
}

#endif //C0_SJX_GLOBAL_H
