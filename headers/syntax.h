//
// Created by 17110 on 2020/5/5.
//

#ifndef C0_SJX_SYNTAX_H
#define C0_SJX_SYNTAX_H

#include "global.h"
#include "log.h"
#include "lexical.h"

enum TypeUnit {
	TPUNIT_NULL,
	// qualifier
	TPUNIT_CONST,
	// modifier
	TPUNIT_SIGNED,
	TPUNIT_UNSIGNED,
	// range
	TPUNIT_SHORT,
	TPUNIT_LONG,
	TPUNIT_LONGLONG,
	// specifier
	TPUNIT_VOID,
	TPUNIT_CHAR,
	TPUNIT_INT,
	TPUNIT_FLOAT,
	TPUNIT_DOUBLE,
};

enum Option {
	OP_NULL,
	// definition
	OP_VARDEF,
	OP_FUNCDEF,
	OP_PARAM,
	OP_RETURN,
	OP_CALL,
	// block
	OP_BEGIN,
	OP_END,
	// label
	OP_LABEL,
	// branch and jump
	OP_JE,
	OP_JNE,
	OP_JMP,
	// operators
	OP_NEG,         //  -   negate
	OP_BITNOT,      //  ~   bitwise not
	OP_LOGNOT,      //  !   logical not
	OP_DEREFERENCE, //  *   dereference
	OP_REFERENCE,   //  &   reference
	OP_MUL,         //  *   multiplication
	OP_DIV,         //  /   division
	OP_MOD,         //  %   modulo
	OP_ADD,         //  +   addition
	OP_SUB,         //  -   subtraction
	OP_SAL,         //  <<  shift arithmetic left
	OP_SAR,         //  >>  shift arithmetic right
	OP_LT,          //  <   less than
	OP_GT,          //  >   greater than
	OP_LE,          //  <=  less than or equal to
	OP_GE,          //  >=  greater than or equal to
	OP_ET,          //  ==  equal to
	OP_NE,          //  !=  not equal to
	OP_BITAND,      //  &   bitwise and
	OP_BITXOR,      //  ^   bitwise exclusive or
	OP_BITOR,       //  |   bitwise or
	OP_LOGAND,      //  &&  logical and
	OP_LOGOR,       //  ||  logical or
	OP_ASSIGN,      //  =   assignment
};

class Type {
public:
	Type();
	Type(std::string);
	bool is_const_;
	TypeUnit modifier_;
	TypeUnit range_;
	TypeUnit specifier_;
	std::string toString();
};

class Tuple {
public:
	Tuple(Option, const std::string&,
	      const std::string&, const std::string&);
	Tuple(Option, Type, const std::string&);
	Option op_;
	Type type_;
	std::string arg1_;
	std::string arg2_;
	std::string result_;
};

class Parser {
public:
	Parser(Scanner*, Log*);
	~Parser();
	Scanner *p_scanner_;
	Log *p_log_;
	std::ofstream ofstream_;
	std::vector<TypeUnit> type_units_;
	Type type_now_;
	std::string iden_now_;
	std::string expr_rst_now_;
	std::string func_name_now_;
	std::string func_end_label_now_;
	size_t label_num_;
	size_t temp_var_num_;
	int param_num_;
	int local_var_num_;
	std::vector<std::unordered_map<std::string, int> > symbol_table_;
	std::vector<Tuple> intermediate_code_;
	bool detectType();
	void checkType();
	bool readSemicolon();
	bool readIden();
	bool parseExpr(int);
	void parseIfstmt();
	void parseForstmt();
	void parseWhilestmt();
	void parseDostmt();
	void parseStmt();
	void parseBlock();
	void parseVardef(bool);
	void parseParamdef();
	void parseFuncdef();
	void parseGlobdef();
	void parseProgram();
	std::string nextLabelName();
	std::string nextTempVarName();
	void increaseSymbolLevel();
	void decreaseSymbolLevel();
	bool hasSymbol(const std::string&);
	void addGlobalName(const std::string&);
	void addParam(const std::string&);
	void addLocalVar(const std::string&);
	int getVarOffset(const std::string&);
	void generTuple(Option, const std::string&,
	                const std::string&, const std::string&);
	void generTuple(Option, Type, const std::string&);
};

#endif //C0_SJX_SYNTAX_H
