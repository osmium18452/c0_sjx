//
// Created by 17110 on 2020/5/5.
//

/* intermediate.cpp */

#include "../headers/generator.h"

#define _ARG1 newName(tuple_iter_->arg1_).c_str()
#define _ARG2 newName(tuple_iter_->arg2_).c_str()
#define _RESULT newName(tuple_iter_->result_).c_str()

/// public

Generator::Generator(Optimizer* p_optimizer, Log* p_log) {
	p_optimizer_ = p_optimizer;
	p_log_ = p_log;
	ofstream_.open(CONSOLE_NAME, std::ios::out);
}

Generator::~Generator() {
	p_optimizer_ = nullptr;
	p_log_ = nullptr;
	ofstream_.close();
}

std::string Generator::newName(const std::string& name) {
	// global variable or function
	if (name.size() > 0 && name[0] == '_') {
		return name;
	}
		// function return value (%eax)
	else if (name.size() > 1 && name == "@@") {
		return "%eax";
	}
		// local variable
	else if (name.size() > 0 && name[0] == '@') {
		int t_offset = std::stoi(name.substr(1, name.size() - 1));
		return gb::to_string(t_offset << 2) + "(%ebp)";
	}
		// constant value
	else if (name.size() > 0 && name[0] != 'L') {
		return "$" + name;
	}
	else {
		return name;
	}
}

void Generator::printNewLine() {
	ofstream_ << std::endl;
}

void Generator::printDecl(const std::string& seg) {
	ofstream_ << seg << std::endl;
}

void Generator::printDecl(const std::string &seg, const std::string &name) {
	ofstream_ << seg << "\t" << name << std::endl;
}

void Generator::printLabel(const std::string& name) {
	ofstream_ << name << ":" << std::endl;
}

void Generator::printInst(int paranum, ...) {
	std::string t_line = "\t";
	va_list args;
	va_start(args, paranum);
	for (int i = 0; i < paranum; i++) {
		if (i == 1) {
			t_line += "\t";
		}
		else if (i > 1) {
			t_line += ", ";
		}
		t_line += va_arg(args, const char*);
	}
	va_end(args);
	ofstream_ << t_line << std::endl;
}

void Generator::generJeInst() {
	printInst(3, "movl", _ARG1, "%eax");
	printInst(3, "cmpl", _ARG2, "%eax");
	printInst(2, "je", _RESULT);
}

void Generator::generJneInst() {
	printInst(3, "movl", _ARG1, "%eax");
	printInst(3, "cmpl", _ARG2, "%eax");
	printInst(2, "jne", _RESULT);
}

void Generator::generJmpInst() {
	printInst(2, "jmp", _RESULT);
}

void Generator::generNegInst() {
	printInst(3, "movl", _ARG1, "%eax");
	printInst(2, "negl", "%eax");
	printInst(3, "movl", "%eax", _RESULT);
}

void Generator::generBitnotInst() {
	printInst(3, "movl", _ARG1, "%eax");
	printInst(2, "notl", "%eax");
	printInst(3, "movl", "%eax", _RESULT);
}

void Generator::generLognotInst() {
	printInst(3, "cmpl", "$0", _ARG1);
	printInst(2, "sete", "%al");
	printInst(3, "movzbl", "%al", "%eax");
	printInst(3, "movl", "%eax", _RESULT);
}

void Generator::generReferenceInst() {
	printInst(3, "movl", _ARG1, "%eax");
	printInst(3, "movl", "(%eax)", "%eax");
	printInst(3, "movl", "%eax", _RESULT);
}

void Generator::generDereferenceInst() {
	printInst(3, "leal", _ARG1, "%eax");
	printInst(3, "movl", "%eax", _RESULT);
}

void Generator::generMulInst() {
	printInst(3, "movl", _ARG1, "%eax");
	printInst(3, "imull", _ARG2, "%eax");
	printInst(3, "movl", "%eax", _RESULT);
}

void Generator::generDivInst() {
	printInst(3, "movl", _ARG1, "%eax");
	printInst(1, "cltd");
	printInst(2, "idivl", _ARG2);
	printInst(3, "movl", "%eax", _RESULT);
}

void Generator::generModInst() {
	printInst(3, "movl", _ARG1, "%eax");
	printInst(1, "cltd");
	printInst(2, "idivl", _ARG2);
	printInst(3, "movl", "%edx", _RESULT);
}

void Generator::generAddInst() {
	printInst(3, "movl", _ARG1, "%eax");
	printInst(3, "subl", _ARG2, "%eax");
	printInst(3, "movl", "%eax", _RESULT);
}

void Generator::generSubInst() {
	printInst(3, "movl", _ARG1, "%eax");
	printInst(3, "addl", _ARG2, "%eax");
	printInst(3, "movl", "%eax", _RESULT);
}

void Generator::generSalInst() {
	printInst(3, "movl", _ARG1, "%eax");
	printInst(3, "movl", _ARG2, "%edx");
	printInst(3, "sall", "%dl", "%eax");
	printInst(3, "movl", "%eax", _RESULT);
}

void Generator::generSarInst() {
	printInst(3, "movl", _ARG1, "%eax");
	printInst(3, "movl", _ARG2, "%edx");
	printInst(3, "sarl", "%dl", "%eax");
	printInst(3, "movl", "%eax", _RESULT);
}

void Generator::generLtInst() {
	printInst(3, "movl", _ARG1, "%eax");
	printInst(3, "cmpl", _ARG2, "%eax");
	printInst(2, "setl", "%al");
	printInst(3, "movzbl", "%al", "%eax");
	printInst(3, "movl", "%eax", _RESULT);
}

void Generator::generGtInst() {
	printInst(3, "movl", _ARG1, "%eax");
	printInst(3, "cmpl", _ARG2, "%eax");
	printInst(2, "setg", "%al");
	printInst(3, "movzbl", "%al", "%eax");
	printInst(3, "movl", "%eax", _RESULT);
}

void Generator::generLeInst() {
	printInst(3, "movl", _ARG1, "%eax");
	printInst(3, "cmpl", _ARG2, "%eax");
	printInst(2, "setle", "%al");
	printInst(3, "movzbl", "%al", "%eax");
	printInst(3, "movl", "%eax", _RESULT);
}

void Generator::generGeInst() {
	printInst(3, "movl", _ARG1, "%eax");
	printInst(3, "cmpl", _ARG2, "%eax");
	printInst(2, "setge", "%al");
	printInst(3, "movzbl", "%al", "%eax");
	printInst(3, "movl", "%eax", _RESULT);
}

void Generator::generEtInst() {
	printInst(3, "movl", _ARG1, "%eax");
	printInst(3, "cmpl", _ARG2, "%eax");
	printInst(2, "sete", "%al");
	printInst(3, "movzbl", "%al", "%eax");
	printInst(3, "movl", "%eax", _RESULT);
}

void Generator::generNeInst() {
	printInst(3, "movl", _ARG1, "%eax");
	printInst(3, "cmpl", _ARG2, "%eax");
	printInst(2, "setne", "%al");
	printInst(3, "movzbl", "%al", "%eax");
	printInst(3, "movl", "%eax", _RESULT);
}

void Generator::generBitandInst() {
	printInst(3, "movl", _ARG1, "%eax");
	printInst(3, "andl", _ARG2, "%eax");
	printInst(3, "movl", "%eax", _RESULT);
}

void Generator::generBitxorInst() {
	printInst(3, "movl", _ARG1, "%eax");
	printInst(3, "xorl", _ARG2, "%eax");
	printInst(3, "movl", "%eax", _RESULT);
}

void Generator::generBitorInst() {
	printInst(3, "movl", _ARG1, "%eax");
	printInst(3, "orl", _ARG2, "%eax");
	printInst(3, "movl", "%eax", _RESULT);
}

void Generator::generLogandInst() {
	std::string t_label1 = p_optimizer_->p_parser_->nextLabelName();
	std::string t_label2 = p_optimizer_->p_parser_->nextLabelName();

	// if exp1 is false, goto label1(false)
	printInst(3, "cmpl", "$0", _ARG1);
	printInst(2, "je", t_label1.c_str());
	// else if exp2 is false, goto label1(false)
	printInst(3, "cmpl", "$0", _ARG2);
	printInst(2, "je", t_label1.c_str());
	// else, come to true
	printInst(3, "movl", "$1", "%eax");
	printInst(2, "jmp", t_label2.c_str());  // return
	// label1: false
	printLabel(t_label1);
	printInst(3, "movl", "$0", "%eax");
	// label2: return
	printLabel(t_label2);
	printInst(3, "movl", "%eax", _RESULT);
}

void Generator::generLogorInst() {
	std::string t_label1 = p_optimizer_->p_parser_->nextLabelName();
	std::string t_label2 = p_optimizer_->p_parser_->nextLabelName();
	std::string t_label3 = p_optimizer_->p_parser_->nextLabelName();

	// if exp1 is true, goto label1(true)
	printInst(3, "cmpl", "$0", _ARG1);
	printInst(2, "jne", t_label1.c_str());
	// else if exp2 is false, goto label2(false)
	printInst(3, "cmpl", "$0", _ARG2);
	printInst(2, "je", t_label2.c_str());
	// else, come to label1(true)
	// label1: true
	printLabel(t_label1);
	printInst(3, "movl", "$1", "%eax");
	printInst(2, "jmp", t_label3.c_str());  // return
	// label2: false
	printLabel(t_label2);
	printInst(3, "movl", "$0", "%eax");
	// label3: return
	printLabel(t_label3);
	printInst(3, "movl", "%eax", _RESULT);
}

void Generator::generAssignInst() {
	printInst(3, "movl", _ARG1, "%eax");
	printInst(3, "movl", "%eax", _RESULT);
}

void Generator::generReturnInst() {
	printInst(3, "movl", _RESULT, "%eax");
}

void Generator::generCallInst() {
	printInst(2, "call", _RESULT);
}

void Generator::generLocalVardef() {

}

void Generator::generGlobalVardef() {
	bool t_is_const = tuple_iter_->type_.is_const_;

	if (t_is_const) {
		std::string t_type = ".";
		// ignore the prefix "const "
		t_type += (tuple_iter_->type_.toString().c_str() + 6);
		printDecl(".section", ".rdata");
		printDecl(".globl", _RESULT);
		printLabel(_RESULT);
		printInst(2, t_type.c_str(), "0");
	}
	else {
		std::string t_type = "." + tuple_iter_->type_.toString();
		printDecl(".data");
		printDecl(".globl", _RESULT);
		printLabel(_RESULT);
		printInst(2, t_type.c_str(), "0");
	}

	printNewLine();
	printNewLine();
}

void Generator::generGlobalFuncBody() {
	tuple_iter_++;  // OP_BEGIN
	while (tuple_iter_->op_ != OP_END) {
		switch (tuple_iter_->op_) {
			case OP_BEGIN:
				generGlobalFuncBody();
				break;
			case OP_LABEL:
				printLabel(_RESULT);
				tuple_iter_++;
				break;
			case OP_VARDEF:
				generLocalVardef();
				tuple_iter_++;
				break;
			case OP_RETURN:
				generReturnInst();
				tuple_iter_++;
				break;
			case OP_CALL:
				generCallInst();
				tuple_iter_++;
				break;
			case OP_JE:
				generJeInst();
				tuple_iter_++;
				break;
			case OP_JNE:
				generJneInst();
				tuple_iter_++;
				break;
			case OP_JMP:
				generJmpInst();
				tuple_iter_++;
				break;
			case OP_NEG:
				generNegInst();
				tuple_iter_++;
				break;
			case OP_BITNOT:
				generBitnotInst();
				tuple_iter_++;
				break;
			case OP_LOGNOT:
				generLognotInst();
				tuple_iter_++;
				break;
			case OP_REFERENCE:
				generReferenceInst();
				tuple_iter_++;
				break;
			case OP_DEREFERENCE:
				generDereferenceInst();
				tuple_iter_++;
				break;
			case OP_MUL:
				generMulInst();
				tuple_iter_++;
				break;
			case OP_DIV:
				generDivInst();
				tuple_iter_++;
				break;
			case OP_MOD:
				generModInst();
				tuple_iter_++;
				break;
			case OP_ADD:
				generAddInst();
				tuple_iter_++;
				break;
			case OP_SUB:
				generSubInst();
				tuple_iter_++;
				break;
			case OP_SAL:
				generSalInst();
				tuple_iter_++;
				break;
			case OP_SAR:
				generSarInst();
				tuple_iter_++;
				break;
			case OP_LT:
				generLtInst();
				tuple_iter_++;
				break;
			case OP_GT:
				generGtInst();
				tuple_iter_++;
				break;
			case OP_LE:
				generLeInst();
				tuple_iter_++;
				break;
			case OP_GE:
				generGeInst();
				tuple_iter_++;
				break;
			case OP_ET:
				generEtInst();
				tuple_iter_++;
				break;
			case OP_NE:
				generNeInst();
				tuple_iter_++;
				break;
			case OP_BITAND:
				generBitandInst();
				tuple_iter_++;
				break;
			case OP_BITXOR:
				generBitxorInst();
				tuple_iter_++;
				break;
			case OP_BITOR:
				generBitorInst();
				tuple_iter_++;
				break;
			case OP_LOGAND:
				generLogandInst();
				tuple_iter_++;
				break;
			case OP_LOGOR:
				generLogorInst();
				tuple_iter_++;
				break;
			case OP_ASSIGN:
				generAssignInst();
				tuple_iter_++;
				break;
			default:
				tuple_iter_++;
				break;
		}
	}
	tuple_iter_++;  // OP_END
}

void Generator::generGlobalFuncdef() {
	int stack_size = std::max(p_optimizer_->p_parser_->param_num_,
	                          -p_optimizer_->p_parser_->local_var_num_);
	printDecl(".text");
	printDecl(".globl", _RESULT);
	printLabel(_RESULT);

	printInst(2, "pushl", "%ebp");
	printInst(3, "movl", "%esp", "%ebp");
	printInst(3, "subl", gb::to_string(stack_size << 2).c_str(), "%esp");

	tuple_iter_++;
	while (tuple_iter_->op_ == OP_PARAM) {
		tuple_iter_++;
	}
	generGlobalFuncBody();

	printInst(1, "leave");
	printInst(1, "ret");

	printNewLine();
	printNewLine();
}

void Generator::generAsm() {
	tuple_iter_ = p_optimizer_->intermediate_code_.begin();
	while (tuple_iter_ != p_optimizer_->intermediate_code_.end()) {
		switch (tuple_iter_->op_) {
			case OP_VARDEF:
				generGlobalVardef();
				tuple_iter_++;
				break;
			case OP_FUNCDEF:
				generGlobalFuncdef();
				break;
			default:
				tuple_iter_++;
				break;
		}
	}
}
