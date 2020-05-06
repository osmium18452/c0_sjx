//
// Created by 17110 on 2020/5/5.
//

#ifndef C0_SJX_LOG_H
#define C0_SJX_LOG_H

#include "global.h"

enum ErrorName {
	// lexical
	ERR_NOSUCHFILE,     // no such file
	ERR_UNRECOGCHAR,    // unrecognizable character
	ERR_INVALIDSUFFIX,  // invalid suffix ... on ... constant
	ERR_EMPTYCHARCST,   // empty character constant
	ERR_MISSTRM,        // missing terminating ... character
	// type
	ERR_OPPOSPEC,       // ... and ... specified together
	ERR_DUPLICATE,      // duplicate ...
	ERR_TRIPLELONG,     // long long long is too long
	ERR_MULTITYPE,      // two or more data type
	ERR_INVALIDSPEC,    // ... invalid
	// declaration or definition
	ERR_REDEFINITION,   // redefinition of ...
	ERR_EXPECTED,       // expected ...
	ERR_ELSENOIF,       // else without a previous if
};

enum WarningName {
	WARN_MULTICHARCST,    // multi-character character constant
	WARN_TYPEDFLTTOINT,  // type default to int
};

class Log {
public:
	Log();
	~Log();
	std::ofstream ofstream_;  // error file output stream
	size_t line_num_;  // line number of error or warning
	size_t error_num_;
	size_t warning_num_;
	void callError(ErrorName, ...);
	void callWarning(WarningName, ...);
	void printResult();
};


#endif //C0_SJX_LOG_H
