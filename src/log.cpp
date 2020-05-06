//
// Created by 17110 on 2020/5/5.
//

/* log.cpp */

#include "../headers/log.h"
#include "../headers/lexical.h"

/// public

Log::Log() {
	line_num_ = 0;
	error_num_ = 0;
	warning_num_ = 0;
	ofstream_.open(CONSOLE_NAME, std::ios::out);
}

Log::~Log() {
	ofstream_.close();
}

void Log::callError(ErrorName error_name, ...) {
	error_num_++;
	if (error_num_ > 100) {
		exit(0);
		return;
	}
	// error in code
	if (line_num_ != 0) {
		ofstream_ << "Error in line " << line_num_ << ": ";
	}
		// error out of code
	else {
		ofstream_ << "Error: ";
	}
	// error information
	std::string t_info;
	va_list args;
	va_start(args, error_name);
	switch (error_name) {
		case ERR_NOSUCHFILE:
			t_info += "no such file";
			break;
		case ERR_UNRECOGCHAR:
			t_info += "unrecognizable character";
			break;
		case ERR_INVALIDSUFFIX:
			t_info += "invalid suffix ";
			t_info += va_arg(args, const char*);
			t_info += " on ";
			t_info += va_arg(args, const char*);
			t_info += " constant";
			break;
		case ERR_EMPTYCHARCST:
			t_info += "empty character constant";
			break;
		case ERR_MISSTRM:
			t_info += "missing terminating ";
			t_info += va_arg(args, const char*);
			t_info += " character";
			break;
		case ERR_OPPOSPEC:
			t_info += va_arg(args, const char*);
			t_info += " and ";
			t_info += va_arg(args, const char*);
			t_info += " specified together for \'";
			t_info += va_arg(args, const char*);
			t_info += "\'";
			break;
		case ERR_DUPLICATE:
			t_info += "duplicate ";
			t_info += va_arg(args, const char*);
			break;
		case ERR_TRIPLELONG:
			t_info += "\'long long long\' is too long";
			break;
		case ERR_MULTITYPE:
			t_info += "two or more data types in declaration of \'";
			t_info += va_arg(args, const char*);
			t_info += "\'";
			break;
		case ERR_INVALIDSPEC:
			t_info += va_arg(args, const char*);
			t_info += " invalid for \'";
			t_info += va_arg(args, const char*);
			t_info += "\'";
			break;
		case ERR_REDEFINITION:
			t_info += "redefinition of \'";
			t_info += va_arg(args, const char*);
			t_info += "\'";
			break;
		case ERR_EXPECTED:
			t_info += "expected ";
			t_info += va_arg(args, const char*);
			break;
		case ERR_ELSENOIF:
			t_info += "\'else\' without a previous \'if\'";
		default:
			break;
	}
	va_end(args);
	ofstream_ << t_info << std::endl;
}

void Log::callWarning(WarningName warning_name, ...) {
	warning_num_++;
	// warning in code
	if (line_num_ != 0) {
		ofstream_ << "Warning in line " << line_num_ << ": ";
	}
		// warning out of code
	else {
		ofstream_ << "Warning: ";
	}
	// waring information
	std::string t_info;
	va_list args;
	va_start(args, warning_name);
	switch (warning_name) {
		case WARN_MULTICHARCST:
			t_info += "multi-character character constant";
			break;
		case WARN_TYPEDFLTTOINT:
			t_info += "type defaults to \'int\' in declaration of \'";
			t_info += va_arg(args, const char*);
			t_info += "\'";
			break;
	}
	va_end(args);
	ofstream_ << t_info << std::endl;
}

void Log::printResult() {
	ofstream_ << error_num_ << " error(s), "
	          << warning_num_ << " warning(s)\n" << std::endl;
}
