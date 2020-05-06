//
// Created by 17110 on 2020/5/5.
//

#include "../headers/compiler.h"

Compiler::Compiler() {
	p_log_ = new Log();
	p_scanner_ = new Scanner(p_log_);
	p_parser_ = new Parser(p_scanner_, p_log_);
	p_optimizer_ = new Optimizer(p_parser_, p_log_);
	p_generator_ = new Generator(p_optimizer_, p_log_);
}

Compiler::~Compiler() {
	delete p_log_;
	delete p_scanner_;
	delete p_parser_;
	delete p_optimizer_;
	delete p_generator_;
}

void Compiler::setLogOutputFile(const std::string& filename) {
	p_log_->ofstream_.close();
	p_log_->ofstream_.open(filename, std::ios::out);
}

void Compiler::setScannerInputFile(const std::string& filename) {
	p_scanner_->ifstream_.close();
	p_scanner_->ifstream_.open(filename, std::ios::in);
}

void Compiler::setParserOutputFile(const std::string& filename) {
	p_parser_->ofstream_.close();
	p_parser_->ofstream_.open(filename, std::ios::out);
}

void Compiler::setOptimizerOutputFile(const std::string& filename) {
	p_optimizer_->ofstream_.close();
	p_optimizer_->ofstream_.open(filename, std::ios::out);
}

void Compiler::setGeneratorOutputFile(const std::string& filename) {
	p_generator_->ofstream_.close();
	p_generator_->ofstream_.open(filename, std::ios::out);
}

bool Compiler::compile() {
	if (p_scanner_->ifstream_.is_open() == false) {
		p_log_->callError(ERR_NOSUCHFILE);
	}
	// LL(1)
	p_scanner_->readChar();
	p_scanner_->readToken();
	p_parser_->parseProgram();
	// success
	if (p_log_->error_num_ == 0) {
		p_optimizer_->optimizeCode();
		p_generator_->generAsm();
	}
	p_log_->printResult();
	return p_log_->error_num_ == 0;
}
