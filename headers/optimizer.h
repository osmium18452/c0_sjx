//
// Created by 17110 on 2020/5/5.
//

#ifndef C0_SJX_OPTIMIZER_H
#define C0_SJX_OPTIMIZER_H

#include "global.h"
#include "log.h"
#include "syntax.h"

class Optimizer {
public:
	Optimizer(Parser*, Log*);
	~Optimizer();
	Parser *p_parser_;
	Log *p_log_;
	std::ofstream ofstream_;
	std::vector<Tuple> intermediate_code_;
	void optimizeCode();
};

#endif //C0_SJX_OPTIMIZER_H
