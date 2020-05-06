//
// Created by 17110 on 2020/5/5.
//

#ifndef C0_SJX_COMPILER_H
#define C0_SJX_COMPILER_H

#include "global.h"
#include "log.h"
#include "lexical.h"
#include "syntax.h"
#include "optimizer.h"
#include "generator.h"

class Compiler {
public:
	Compiler();
	~Compiler();
	void setLogOutputFile(const std::string&);
	void setScannerInputFile(const std::string&);
	void setParserOutputFile(const std::string&);
	void setOptimizerOutputFile(const std::string&);
	void setGeneratorOutputFile(const std::string&);
	bool compile();
private:
	Log *p_log_;
	Scanner *p_scanner_;
	Parser *p_parser_;
	Optimizer *p_optimizer_;
	Generator *p_generator_;
};

#endif //C0_SJX_COMPILER_H
