//
// Created by 17110 on 2020/5/5.
//

/* optimizer.cpp */

#include "../headers/optimizer.h"

/// public

Optimizer::Optimizer(Parser* p_parser, Log* p_log) {
	p_parser_ = p_parser;
	p_log_ = p_log;
	ofstream_.open(CONSOLE_NAME, std::ios::out);
}

Optimizer::~Optimizer() {
	p_parser_ = nullptr;
	p_log_ = nullptr;
	ofstream_.close();
}

void Optimizer::optimizeCode() {
	for (Tuple t_tuple : p_parser_->intermediate_code_) {
		intermediate_code_.push_back(t_tuple);
	}
}
