//
// Created by 17110 on 2020/5/5.
//

#ifndef C0_SJX_GENERATOR_H
#define C0_SJX_GENERATOR_H

#include "global.h"
#include "log.h"
#include "optimizer.h"

class Generator {
public:
	Generator(Optimizer*, Log*);
	~Generator();
	Optimizer *p_optimizer_;
	Log *p_log_;
	std::ofstream ofstream_;
	std::vector<Tuple>::iterator tuple_iter_;
	std::string newName(const std::string&);
	void printNewLine();
	void printDecl(const std::string&);
	void printDecl(const std::string&, const std::string&);
	void printLabel(const std::string&);
	void printInst(int paranum, ...);
	void generJeInst();
	void generJneInst();
	void generJmpInst();
	void generNegInst();
	void generBitnotInst();
	void generLognotInst();
	void generReferenceInst();
	void generDereferenceInst();
	void generMulInst();
	void generDivInst();
	void generModInst();
	void generAddInst();
	void generSubInst();
	void generSalInst();
	void generSarInst();
	void generLtInst();
	void generGtInst();
	void generLeInst();
	void generGeInst();
	void generEtInst();
	void generNeInst();
	void generBitandInst();
	void generBitxorInst();
	void generBitorInst();
	void generLogandInst();
	void generLogorInst();
	void generAssignInst();
	void generReturnInst();
	void generCallInst();
	void generLocalVardef();
	void generGlobalVardef();
	void generGlobalFuncBody();
	void generGlobalFuncdef();
	void generAsm();
};

#endif //C0_SJX_GENERATOR_H
