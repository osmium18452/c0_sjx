//
// Created by 17110 on 2020/5/5.
//

/* syntax.cpp */

#include "../headers/syntax.h"

#define MAX_EXPR_LEVEL 15
#define SUB_EXPR_LEVEL 14

/// global variables

// TokenToTypeUnit for detecting type
std::unordered_map<TokenName, TypeUnit, std::hash<int> > TokenToTypeUnit ({
		                                                                          {TOK_CONST   , TPUNIT_CONST}   ,
		                                                                          {TOK_SIGNED  , TPUNIT_SIGNED}  , {TOK_UNSIGNED, TPUNIT_UNSIGNED},
		                                                                          {TOK_SHORT   , TPUNIT_SHORT}   , {TOK_LONG    , TPUNIT_LONG}    ,
		                                                                          {TOK_VOID    , TPUNIT_VOID}    ,
		                                                                          {TOK_CHAR    , TPUNIT_CHAR}    , {TOK_INT     , TPUNIT_INT}     ,
		                                                                          {TOK_FLOAT   , TPUNIT_FLOAT}   , {TOK_DOUBLE  , TPUNIT_DOUBLE}  ,
                                                                          });

std::unordered_map<Option, std::string, std::hash<int> > OptionToString ({
		                                                                         {OP_NULL       , ""}         ,
		                                                                         {OP_BITNOT     , "~"}        , {OP_LOGNOT    , "!"}         ,
		                                                                         {OP_DEREFERENCE, "*"}        , {OP_REFERENCE , "&"}         ,
		                                                                         {OP_MUL        , "*"}        , {OP_DIV       , "/"}         ,
		                                                                         {OP_MOD        , "%"}        ,
		                                                                         {OP_ADD        , "+"}        , {OP_SUB       , "-"}         ,
		                                                                         {OP_SAL        , "<<"}       , {OP_SAR       , ">>"}        ,
		                                                                         {OP_LT         , "<"}        , {OP_GT        , ">"}         ,
		                                                                         {OP_LE         , "<="}       , {OP_GE        , ">="}        ,
		                                                                         {OP_ET         , "=="}       , {OP_NE        , "!="}        ,
		                                                                         {OP_BITAND     , "&"}        , {OP_BITXOR    , "^"}         ,
		                                                                         {OP_BITOR      , "|"}        ,
		                                                                         {OP_LOGAND     , "&&"}       , {OP_LOGOR     , "||"}        ,
		                                                                         {OP_ASSIGN     , "="}        ,
		                                                                         {OP_VARDEF     , "vardef"}   , {OP_FUNCDEF   , "funcdef"}   ,
		                                                                         {OP_PARAM      , "param"}    ,
		                                                                         {OP_BEGIN      , "begin"}    , {OP_END       , "end"}       ,
		                                                                         {OP_JE         , "je"}       , {OP_JNE       , "jne"}       ,
		                                                                         {OP_JMP        , "jmp"}      ,
		                                                                         {OP_LABEL      , "label"}    ,
                                                                         });

/// public

Type::Type() {
	is_const_ = false;
	modifier_ = TPUNIT_NULL;
	range_ = TPUNIT_NULL;
	specifier_ = TPUNIT_NULL;
}

Type::Type(std::string str) {
	Type();
	std::stringstream t_ss(str);
	std::string t_unit;
	while (t_ss >> t_unit) {
		if (t_unit == "const") {
			is_const_ = true;
		}
		else if (t_unit == "signed") {
			modifier_ = TPUNIT_SIGNED;
		}
		else if (t_unit == "unsigned") {
			modifier_ = TPUNIT_UNSIGNED;
		}
		else if (t_unit == "short") {
			range_ = TPUNIT_SHORT;
		}
		else if (t_unit == "long") {
			if (range_ == TPUNIT_NULL) {
				range_ = TPUNIT_LONG;
			}
			else if (range_ == TPUNIT_LONG) {
				range_ = TPUNIT_LONGLONG;
			}
		}
		else if (t_unit == "void") {
			specifier_ = TPUNIT_VOID;
		}
		else if (t_unit == "char") {
			specifier_ = TPUNIT_CHAR;
		}
		else if (t_unit == "int") {
			specifier_ = TPUNIT_INT;
		}
		else if (t_unit == "float") {
			specifier_ = TPUNIT_FLOAT;
		}
		else if (t_unit == "double") {
			specifier_ = TPUNIT_DOUBLE;
		}
	}
}

// transfer type to string for intermediate code
std::string Type::toString() {
	std::string t_string;
	// qualifier
	if (is_const_) {
		t_string += "const ";
	}
	// modifier
	switch (modifier_) {
		case TPUNIT_SIGNED:
			t_string += "signed ";
			break;
		case TPUNIT_UNSIGNED:
			t_string += "unsigned ";
			break;
		default:
			break;
	}
	// range
	switch (range_) {
		case TPUNIT_SHORT:
			t_string += "short ";
			break;
		case TPUNIT_LONG:
			t_string += "long ";
			break;
		case TPUNIT_LONGLONG:
			t_string += "long long ";
			break;
		default:
			break;
	}
	// specifier
	switch (specifier_)
	{
		case TPUNIT_VOID:
			t_string += "void";
			break;
		case TPUNIT_CHAR:
			t_string += "char";
			break;
		case TPUNIT_INT:
			t_string += "int";
			break;
		case TPUNIT_FLOAT:
			t_string += "float";
			break;
		case TPUNIT_DOUBLE:
			t_string += "double";
			break;
		default:
			break;
	}
	return t_string;
}

Tuple::Tuple(Option op, const std::string& arg1,
             const std::string& arg2, const std::string& result) {
	op_ = op;
	arg1_ = arg1;
	arg2_ = arg2;
	result_ = result;
}

Tuple::Tuple(Option op, Type type, const std::string& result) {
	op_ = op;
	type_ = type;
	result_ = result;
}

Parser::Parser(Scanner* p_scanner, Log* p_log) {
	p_scanner_ = p_scanner;
	p_log_ = p_log;
	ofstream_.open(CONSOLE_NAME, std::ios::out);
	label_num_ = 0;
	temp_var_num_ = 0;
	param_num_ = 0;
	local_var_num_ = 0;
}

Parser::~Parser() {
	p_scanner_ = nullptr;
	p_log_ = nullptr;
	ofstream_.close();
}

// detect type
bool Parser::detectType() {
	type_units_.clear();
	while(TokenToTypeUnit.find(p_scanner_->token_now_.name_)
	      != TokenToTypeUnit.end()) {
		type_units_.push_back(TokenToTypeUnit[p_scanner_->token_now_.name_]);
		p_scanner_->readToken();
	}
	return type_units_.size() > 0;
}

// check type
void Parser::checkType() {
	type_now_.is_const_ = false;
	type_now_.modifier_ = TPUNIT_NULL;
	type_now_.range_ = TPUNIT_NULL;
	type_now_.specifier_ = TPUNIT_NULL;
	for (TypeUnit unit: type_units_) {
		switch (unit) {
			case TPUNIT_CONST:
				type_now_.is_const_ = true;
				break;
			case TPUNIT_SIGNED:
				switch (type_now_.modifier_) {
					case TPUNIT_NULL:
						type_now_.modifier_ = unit;
						break;
					case TPUNIT_SIGNED:
						p_log_->callError(ERR_DUPLICATE, "\'signed\'");
						break;
					case TPUNIT_UNSIGNED:
						p_log_->callError(ERR_OPPOSPEC,
						                  "\'signed\'", "\'unsigned\'", iden_now_.c_str());
						break;
					default:
						break;
				}
				break;
			case TPUNIT_UNSIGNED:
				switch (type_now_.modifier_) {
					case TPUNIT_NULL:
						type_now_.modifier_ = unit;
						break;
					case TPUNIT_SIGNED:
						p_log_->callError(ERR_OPPOSPEC,
						                  "\'signed\'", "\'unsigned\'", iden_now_.c_str());
						break;
					case TPUNIT_UNSIGNED:
						p_log_->callError(ERR_DUPLICATE, "\'unsigned\'");
						break;
					default:
						break;
				}
				break;
			case TPUNIT_SHORT:
				switch (type_now_.range_) {
					case TPUNIT_NULL:
						type_now_.range_ = unit;
						break;
					case TPUNIT_SHORT:
						p_log_->callError(ERR_DUPLICATE, "\'short\'");
						break;
					case TPUNIT_LONG:
					case TPUNIT_LONGLONG:
						p_log_->callError(ERR_OPPOSPEC,
						                  "\'long\'", "\'short\'", iden_now_.c_str());
						break;
					default:
						break;
				}
				break;
			case TPUNIT_LONG:
				switch (type_now_.range_) {
					case TPUNIT_NULL:
						type_now_.range_ = unit;
						break;
					case TPUNIT_SHORT:
						p_log_->callError(ERR_OPPOSPEC,
						                  "\'long\'", "\'short\'", iden_now_.c_str());
						break;
					case TPUNIT_LONG:
						type_now_.range_ = TPUNIT_LONGLONG;
						break;
					case TPUNIT_LONGLONG:
						p_log_->callError(ERR_TRIPLELONG);
						break;
					default:
						break;
				}
				break;
			case TPUNIT_VOID:
			case TPUNIT_CHAR:
			case TPUNIT_INT:
			case TPUNIT_FLOAT:
			case TPUNIT_DOUBLE:
				switch (type_now_.specifier_) {
					case TPUNIT_NULL:
						type_now_.specifier_ = unit;
						break;
					default:
						p_log_->callError(ERR_MULTITYPE, iden_now_.c_str());
						break;
				}
				break;
			default:
				break;
		}
	}
	// has no specifier
	if (type_now_.specifier_ == TPUNIT_NULL) {
		// has modifier or range
		if (type_now_.modifier_ != TPUNIT_NULL
		    || type_now_.range_ != TPUNIT_NULL) {
			type_now_.specifier_ = TPUNIT_INT;
		}
		else {
			type_now_.specifier_ = TPUNIT_INT;
			p_log_->callWarning(WARN_TYPEDFLTTOINT, iden_now_.c_str());
		}
	}
	// check if modifier is valid
	switch (type_now_.modifier_) {
		case TPUNIT_SIGNED:
		case TPUNIT_UNSIGNED:
			switch (type_now_.specifier_) {
				case TPUNIT_INT:
				case TPUNIT_CHAR:
					break;
				default:
					p_log_->callError(ERR_INVALIDSPEC,
					                  "\'signed\' or \'unsigned\'", iden_now_.c_str());
					break;
			}
			break;
		default:
			break;
	}
	// check if range is valid
	switch (type_now_.range_) {
		case TPUNIT_SHORT:
			switch (type_now_.specifier_) {
				case TPUNIT_INT:
					break;
				default:
					p_log_->callError(ERR_INVALIDSPEC,
					                  "\'short\'", iden_now_.c_str());
					break;
			}
			break;
		case TPUNIT_LONG:
			switch (type_now_.specifier_) {
				case TPUNIT_INT:
				case TPUNIT_DOUBLE:
					break;
				default:
					p_log_->callError(ERR_INVALIDSPEC,
					                  "\'long\'", iden_now_.c_str());
					break;
			}
			break;
		case TPUNIT_LONGLONG:
			switch (type_now_.specifier_) {
				case TPUNIT_INT:
					break;
				default:
					p_log_->callError(ERR_INVALIDSPEC,
					                  "\'long long\'", iden_now_.c_str());
					break;
			}
			break;
		default:
			break;
	}
}

// read a semicolon
bool Parser::readSemicolon() {
	if (p_scanner_->token_now_.name_ == TOK_SEMICOLON) {
		p_scanner_->readToken();
		return true;
	}
	else {
		p_log_->callError(ERR_EXPECTED, "\';\'");
		return false;
	}
}

// read an identifier
bool Parser::readIden() {
	if (p_scanner_->token_now_.name_ == TOK_IDEN) {
		iden_now_ = p_scanner_->token_now_.value_.iden_;
		p_scanner_->readToken();
		return true;
	}
	else {
		p_log_->callError(ERR_EXPECTED, "identifier");
		return false;
	}
}

// parse expression
bool Parser::parseExpr(int priority_level) {
	std::string t_arg1, t_arg2, t_result;
	std::string t_label1, t_label2;
	bool t_flag = true;
	switch (priority_level) {
		// factor
		case 0:
			switch (p_scanner_->token_now_.name_) {
				// (
				case TOK_LRBRACKET:
					p_scanner_->readToken();
					t_flag &= parseExpr(MAX_EXPR_LEVEL);
					// )
					if (p_scanner_->token_now_.name_ == TOK_RRBRACKET) {
						p_scanner_->readToken();
						t_result = expr_rst_now_;
					}
					else {
						p_log_->callError(ERR_EXPECTED, "\')\'");
						t_flag = false;
					}
					break;
				case TOK_IDEN:
					readIden();
					t_result = gb::to_string(p_scanner_->token_now_.value_.iden_);
					break;
				case TOK_INTCST:
#ifdef OPEN_OPTIMIZER
					t_result = gb::to_string(p_scanner_->token_now_.value_.int_);
#else
					t_arg1 = gb::to_string(p_scanner_->token_now_.value_.int_);
					t_result = nextTempVarName();
					generTuple(OP_ASSIGN, t_arg1, "", t_result);
#endif // OPEN_OPTIMIZER
					p_scanner_->readToken();
					break;
				case TOK_UINTCST:
#ifdef OPEN_OPTIMIZER
					t_result = gb::to_string(p_scanner_->token_now_.value_.uint_);
#else
					t_arg1 = gb::to_string(p_scanner_->token_now_.value_.uint_);
					t_result = nextTempVarName();
					generTuple(OP_ASSIGN, t_arg1, "", t_result);
#endif // OPEN_OPTIMIZER
					p_scanner_->readToken();
					break;
				case TOK_LONGCST:
#ifdef OPEN_OPTIMIZER
					t_result = gb::to_string(p_scanner_->token_now_.value_.long_);
#else
					t_arg1 = gb::to_string(p_scanner_->token_now_.value_.long_);
					t_result = nextTempVarName();
					generTuple(OP_ASSIGN, t_arg1, "", t_result);
#endif // OPEN_OPTIMIZER
					p_scanner_->readToken();
					break;
				case TOK_ULONGCST:
#ifdef OPEN_OPTIMIZER
					t_result = gb::to_string(p_scanner_->token_now_.value_.ulong_);
#else
					t_arg1 = gb::to_string(p_scanner_->token_now_.value_.ulong_);
					t_result = nextTempVarName();
					generTuple(OP_ASSIGN, t_arg1, "", t_result);
#endif // OPEN_OPTIMIZER
					p_scanner_->readToken();
					break;
				case TOK_FLOATCST:
#ifdef OPEN_OPTIMIZER
					t_result = gb::to_string(p_scanner_->token_now_.value_.float_);
#else
					t_arg1 = gb::to_string(p_scanner_->token_now_.value_.float_);
					t_result = nextTempVarName();
					generTuple(OP_ASSIGN, t_arg1, "", t_result);
#endif // OPEN_OPTIMIZER
					p_scanner_->readToken();
					break;
				case TOK_DOUBLECST:
#ifdef OPEN_OPTIMIZER
					t_result = gb::to_string(p_scanner_->token_now_.value_.double_);
#else
					t_arg1 = gb::to_string(p_scanner_->token_now_.value_.double_);
					t_result = nextTempVarName();
					generTuple(OP_ASSIGN, t_arg1, "", t_result);
#endif // OPEN_OPTIMIZER
					p_scanner_->readToken();
					break;
				case TOK_CHARCST:
#ifdef OPEN_OPTIMIZER
					t_result = gb::to_string(p_scanner_->token_now_.value_.char_);
#else
					t_arg1 = gb::to_string(p_scanner_->token_now_.value_.char_);
					t_result = nextTempVarName();
					generTuple(OP_ASSIGN, t_arg1, "", t_result);
#endif // OPEN_OPTIMIZER
					p_scanner_->readToken();
					break;
				case TOK_STRINGCST:
#ifdef OPEN_OPTIMIZER
					t_result = gb::to_string(p_scanner_->token_now_.value_.string_);
#else
					t_arg1 = gb::to_string(p_scanner_->token_now_.value_.string_);
					t_result = nextTempVarName();
					generTuple(OP_ASSIGN, t_arg1, "", t_result);
#endif // OPEN_OPTIMIZER
					p_scanner_->readToken();
					break;
				default:
					p_log_->callError(ERR_EXPECTED, "expression");
					t_flag = false;
					break;
			}
			break;
			// () [] ++ --(postpose)
		case 1:
			t_flag &= parseExpr(priority_level - 1);
			t_result = expr_rst_now_;
			while (p_scanner_->token_now_.name_ == TOK_LRBRACKET
			       || p_scanner_->token_now_.name_ == TOK_LSBRACKET
			       || p_scanner_->token_now_.name_ == TOK_INCREMENT
			       || p_scanner_->token_now_.name_ == TOK_DECREMENT) {
				// (
				if (p_scanner_->token_now_.name_ == TOK_LRBRACKET) {
					func_name_now_ = iden_now_;
					do {
						p_scanner_->readToken();
						t_flag &= parseExpr(SUB_EXPR_LEVEL);
					} while (p_scanner_->token_now_.name_ == TOK_COMMA);
					// )
					if (p_scanner_->token_now_.name_ == TOK_RRBRACKET) {
						generTuple(OP_CALL, "", "", func_name_now_);
						t_result = "@@";
						p_scanner_->readToken();
					}
				}
					// [
				else if (p_scanner_->token_now_.name_ == TOK_LSBRACKET) {
					p_scanner_->readToken();
					t_flag &= parseExpr(MAX_EXPR_LEVEL);
					// ]
					if (p_scanner_->token_now_.name_ == TOK_RSBRACKET) {
						// add pointer with offset
						t_arg1 = t_result;
						t_arg2 = expr_rst_now_;
						t_result = nextTempVarName();
						generTuple(OP_ADD, t_arg1, t_arg2, t_result);
						// dereference
						t_arg1 = t_result;
						t_result = nextTempVarName();
						generTuple(OP_DEREFERENCE, t_arg1, "", t_result);
						p_scanner_->readToken();
					}
					else {
						p_log_->callError(ERR_EXPECTED, "\']\'");
						t_flag = false;
					}
				}
					// ++
				else if (p_scanner_->token_now_.name_ == TOK_INCREMENT) {
					p_scanner_->readToken();
					t_arg1 = t_result;
					t_result = nextTempVarName();
					generTuple(OP_ADD, t_result, "1", t_result);
				}
					// --
				else if (p_scanner_->token_now_.name_ == TOK_DECREMENT) {
					p_scanner_->readToken();
					t_arg1 = t_result;
					t_result = nextTempVarName();
					generTuple(OP_SUB, t_arg1, "1", t_result);
				}
			}
			break;
			// ++ --(prepose) + -(unary) ! ~ * &
		case 2:
			switch (p_scanner_->token_now_.name_) {
				// ++
				case TOK_INCREMENT:
					p_scanner_->readToken();
					t_flag &= parseExpr(priority_level);
					t_arg1 = expr_rst_now_;
					t_result = nextTempVarName();
					generTuple(OP_ADD, t_arg1, "1", t_result);
					break;
					// --
				case TOK_DECREMENT:
					p_scanner_->readToken();
					t_flag &= parseExpr(priority_level);
					t_arg1 = expr_rst_now_;
					t_result = nextTempVarName();
					generTuple(OP_SUB, t_arg1, "1", t_result);
					break;
					// +
				case TOK_ADD:
					p_scanner_->readToken();
					t_flag &= parseExpr(priority_level);
					t_result = expr_rst_now_;
					break;
					// -
				case TOK_SUB:
					p_scanner_->readToken();
					t_flag &= parseExpr(priority_level);
					t_arg1 = expr_rst_now_;
					t_result = nextTempVarName();
					generTuple(OP_NEG, t_arg1, "", t_result);
					break;
					// !
				case TOK_LOGNOT:
					p_scanner_->readToken();
					t_flag &= parseExpr(priority_level);
					t_arg1 = expr_rst_now_;
					t_result = nextTempVarName();
					generTuple(OP_LOGNOT, t_arg1, "", t_result);
					break;
					// ~
				case TOK_BITNOT:
					p_scanner_->readToken();
					t_flag &= parseExpr(priority_level);
					t_arg1 = expr_rst_now_;
					t_result = nextTempVarName();
					generTuple(OP_BITNOT, t_arg1, "", t_result);
					break;
					// *
				case TOK_MUL:
					p_scanner_->readToken();
					t_flag &= parseExpr(priority_level);
					t_arg1 = expr_rst_now_;
					t_result = nextTempVarName();
					generTuple(OP_DEREFERENCE, t_arg1, "", t_result);
					break;
					// &
				case TOK_BITAND:
					p_scanner_->readToken();
					t_flag &= parseExpr(priority_level);
					t_arg1 = expr_rst_now_;
					t_result = nextTempVarName();
					generTuple(OP_REFERENCE, t_arg1, "", t_result);
					break;
				default:
					t_flag &= parseExpr(priority_level - 1);
					t_result = expr_rst_now_;
					break;
			}
			break;
			// * / %
		case 3:
			t_flag &= parseExpr(priority_level - 1);
			t_result = expr_rst_now_;
			while(p_scanner_->token_now_.name_ == TOK_MUL
			      || p_scanner_->token_now_.name_ == TOK_DIV
			      || p_scanner_->token_now_.name_ == TOK_MOD) {
				t_arg1 = t_result;
				switch (p_scanner_->token_now_.name_) {
					case TOK_MUL:
						p_scanner_->readToken();
						t_flag &= parseExpr(priority_level - 1);
						t_arg2 = expr_rst_now_;
						t_result = nextTempVarName();
						generTuple(OP_MUL, t_arg1, t_arg2, t_result);
						break;
					case TOK_DIV:
						p_scanner_->readToken();
						t_flag &= parseExpr(priority_level - 1);
						t_arg2 = expr_rst_now_;
						t_result = nextTempVarName();
						generTuple(OP_DIV, t_arg1, t_arg2, t_result);
						break;
					case TOK_MOD:
						p_scanner_->readToken();
						t_flag &= parseExpr(priority_level - 1);
						t_arg2 = expr_rst_now_;
						t_result = nextTempVarName();
						generTuple(OP_MOD, t_arg1, t_arg2, t_result);
						break;
					default:
						break;
				}
			}
			break;
			// + -(binary)
		case 4:
			t_flag &= parseExpr(priority_level - 1);
			t_result = expr_rst_now_;
			while(p_scanner_->token_now_.name_ == TOK_ADD
			      || p_scanner_->token_now_.name_ == TOK_SUB) {
				t_arg1 = t_result;
				switch (p_scanner_->token_now_.name_) {
					case TOK_ADD:
						p_scanner_->readToken();
						t_flag &= parseExpr(priority_level - 1);
						t_arg2 = expr_rst_now_;
						t_result = nextTempVarName();
						generTuple(OP_ADD, t_arg1, t_arg2, t_result);
						break;
					case TOK_SUB:
						p_scanner_->readToken();
						t_flag &= parseExpr(priority_level - 1);
						t_arg2 = expr_rst_now_;
						t_result = nextTempVarName();
						generTuple(OP_SUB, t_arg1, t_arg2, t_result);
						break;
					default:
						break;
				}
			}
			break;
			// << >>
		case 5:
			t_flag &= parseExpr(priority_level - 1);
			t_result = expr_rst_now_;
			while(p_scanner_->token_now_.name_ == TOK_SAL
			      || p_scanner_->token_now_.name_ == TOK_SAR) {
				t_arg1 = t_result;
				switch (p_scanner_->token_now_.name_) {
					case TOK_SAL:
						p_scanner_->readToken();
						t_flag &= parseExpr(priority_level - 1);
						t_arg2 = expr_rst_now_;
						t_result = nextTempVarName();
						generTuple(OP_SAL, t_arg1, t_arg2, t_result);
						break;
					case TOK_SAR:
						p_scanner_->readToken();
						t_flag &= parseExpr(priority_level - 1);
						t_arg2 = expr_rst_now_;
						t_result = nextTempVarName();
						generTuple(OP_SAR, t_arg1, t_arg2, t_result);
						break;
					default:
						break;
				}
			}
			break;
			// < > <= >=
		case 6:
			t_flag &= parseExpr(priority_level - 1);
			t_result = expr_rst_now_;
			while(p_scanner_->token_now_.name_ == TOK_LT
			      || p_scanner_->token_now_.name_ == TOK_GT
			      || p_scanner_->token_now_.name_ == TOK_LE
			      || p_scanner_->token_now_.name_ == TOK_GE) {
				t_arg1 = t_result;
				switch (p_scanner_->token_now_.name_) {
					case TOK_LT:
						p_scanner_->readToken();
						t_flag &= parseExpr(priority_level - 1);
						t_arg2 = expr_rst_now_;
						t_result = nextTempVarName();
						generTuple(OP_LT, t_arg1, t_arg2, t_result);
						break;
					case TOK_GT:
						p_scanner_->readToken();
						t_flag &= parseExpr(priority_level - 1);
						t_arg2 = expr_rst_now_;
						t_result = nextTempVarName();
						generTuple(OP_GT, t_arg1, t_arg2, t_result);
						break;
					case TOK_LE:
						p_scanner_->readToken();
						t_flag &= parseExpr(priority_level - 1);
						t_arg2 = expr_rst_now_;
						t_result = nextTempVarName();
						generTuple(OP_LE, t_arg1, t_arg2, t_result);
						break;
					case TOK_GE:
						p_scanner_->readToken();
						t_flag &= parseExpr(priority_level - 1);
						t_arg2 = expr_rst_now_;
						t_result = nextTempVarName();
						generTuple(OP_GE, t_arg1, t_arg2, t_result);
						break;
					default:
						break;
				}
			}
			break;
			// == !=
		case 7:
			t_flag &= parseExpr(priority_level - 1);
			t_result = expr_rst_now_;
			while(p_scanner_->token_now_.name_ == TOK_ET
			      || p_scanner_->token_now_.name_ == TOK_NE) {
				t_arg1 = t_result;
				switch (p_scanner_->token_now_.name_) {
					case TOK_ET:
						p_scanner_->readToken();
						t_flag &= parseExpr(priority_level - 1);
						t_arg2 = expr_rst_now_;
						t_result = nextTempVarName();
						generTuple(OP_ET, t_arg1, t_arg2, t_result);
						break;
					case TOK_NE:
						p_scanner_->readToken();
						t_flag &= parseExpr(priority_level - 1);
						t_arg2 = expr_rst_now_;
						t_result = nextTempVarName();
						generTuple(OP_NE, t_arg1, t_arg2, t_result);
						break;
					default:
						break;
				}
			}
			break;
			// &
		case 8:
			t_flag &= parseExpr(priority_level - 1);
			t_result = expr_rst_now_;
			while(p_scanner_->token_now_.name_ == TOK_BITAND) {
				p_scanner_->readToken();
				t_arg1 = t_result;
				t_flag &= parseExpr(priority_level - 1);
				t_arg2 = expr_rst_now_;
				t_result = nextTempVarName();
				generTuple(OP_BITAND, t_arg1, t_arg2, t_result);
			}
			break;
			// ^
		case 9:
			t_flag &= parseExpr(priority_level - 1);
			t_result = expr_rst_now_;
			while(p_scanner_->token_now_.name_ == TOK_BITXOR) {
				p_scanner_->readToken();
				t_arg1 = t_result;
				t_flag &= parseExpr(priority_level - 1);
				t_arg2 = expr_rst_now_;
				t_result = nextTempVarName();
				generTuple(OP_BITXOR, t_arg1, t_arg2, t_result);
			}
			break;
			// |
		case 10:
			t_flag &= parseExpr(priority_level - 1);
			t_result = expr_rst_now_;
			while(p_scanner_->token_now_.name_ == TOK_BITOR) {
				p_scanner_->readToken();
				t_arg1 = t_result;
				t_flag &= parseExpr(priority_level - 1);
				t_arg2 = expr_rst_now_;
				t_result = nextTempVarName();
				generTuple(OP_BITOR, t_arg1, t_arg2, t_result);
			}
			break;
			// &&
		case 11:
			t_flag &= parseExpr(priority_level - 1);
			t_result = expr_rst_now_;
			while(p_scanner_->token_now_.name_ == TOK_LOGAND) {
				p_scanner_->readToken();
				t_arg1 = t_result;
				t_flag &= parseExpr(priority_level - 1);
				t_arg2 = expr_rst_now_;
				t_result = nextTempVarName();
				generTuple(OP_LOGAND, t_arg1, t_arg2, t_result);
			}
			break;
			// ||
		case 12:
			t_flag &= parseExpr(priority_level - 1);
			t_result = expr_rst_now_;
			while(p_scanner_->token_now_.name_ == TOK_LOGOR) {
				p_scanner_->readToken();
				t_arg1 = t_result;
				t_flag &= parseExpr(priority_level - 1);
				t_arg2 = expr_rst_now_;
				t_result = nextTempVarName();
				generTuple(OP_LOGOR, t_arg1, t_arg2, t_result);
			}
			break;
			// ?:
		case 13:
			t_flag &= parseExpr(priority_level - 1);
			t_result = expr_rst_now_;
			// ?
			if (p_scanner_->token_now_.name_ == TOK_QUESTION) {
				p_scanner_->readToken();
				t_label1 = nextLabelName();
				t_label2 = nextLabelName();
				// expression 1
				generTuple(OP_JE, t_result, "0", t_label1);
				t_result = nextTempVarName();
				t_flag &= parseExpr(priority_level);
				generTuple(OP_ASSIGN, expr_rst_now_, "", t_result);
				generTuple(OP_JMP, "", "", t_label2);
				// :
				if (p_scanner_->token_now_.name_ == TOK_COLON) {
					p_scanner_->readToken();
					// expression 2
					generTuple(OP_LABEL, "", "", t_label1);
					t_flag &= parseExpr(priority_level);
					generTuple(OP_ASSIGN, expr_rst_now_, "", t_result);
					generTuple(OP_LABEL, "", "", t_label2);
				}
				else {
					p_log_->callError(ERR_EXPECTED, "\':\'");
				}
			}
			break;
			// =
		case 14:
			t_flag &= parseExpr(priority_level - 1);
			t_result = expr_rst_now_;
			if (p_scanner_->token_now_.name_ == TOK_ASSIGN) {
				p_scanner_->readToken();
				// ... judge if result is lvalue
				t_flag &= parseExpr(priority_level);
				t_arg1 = expr_rst_now_;
				generTuple(OP_ASSIGN, t_arg1, "", t_result);
			}
			break;
			// ,
		case 15:
			t_flag &= parseExpr(priority_level - 1);
			t_result = expr_rst_now_;
			while(p_scanner_->token_now_.name_ == TOK_COMMA) {
				p_scanner_->readToken();
				t_flag &= parseExpr(priority_level - 1);
				t_result = expr_rst_now_;
			}
			break;
		default:
			break;
	}
	expr_rst_now_ = t_result;
	return t_flag;
}

// parse if statement
void Parser::parseIfstmt() {
/*
<ifstmt> = <(> <expr_max> <)> <stmt>
         | <(> <expr_max> <)> <stmt> <else> <stmt>
*/
	std::string t_label1 = nextLabelName();
	std::string t_label2 = nextLabelName();
	// (
	if (p_scanner_->token_now_.name_ == TOK_LRBRACKET) {
		p_scanner_->readToken();
		parseExpr(MAX_EXPR_LEVEL);
		// )
		if (p_scanner_->token_now_.name_ == TOK_RRBRACKET) {
			generTuple(OP_JE, expr_rst_now_, "0", t_label1);
			p_scanner_->readToken();
			// already in a block
			if (p_scanner_->token_now_.name_ == TOK_LCBRACKET) {
				parseStmt();
			}
				// add into a block manually
			else {
				generTuple(OP_BEGIN, "", "", "");
				increaseSymbolLevel();
				parseStmt();
				generTuple(OP_END, "", "", "");
				decreaseSymbolLevel();
			}
			generTuple(OP_JMP, "", "", t_label2);
			generTuple(OP_LABEL, "", "", t_label1);
			// else
			if (p_scanner_->token_now_.name_ == TOK_ELSE) {
				p_scanner_->readToken();
				// already in a block
				if (p_scanner_->token_now_.name_ == TOK_LCBRACKET) {
					parseStmt();
				}
					// add into a block manually
				else {
					generTuple(OP_BEGIN, "", "", "");
					increaseSymbolLevel();
					parseStmt();
					generTuple(OP_END, "", "", "");
					decreaseSymbolLevel();
				}
			}
			generTuple(OP_LABEL, "", "", t_label2);
		}
		else {
			p_log_->callError(ERR_EXPECTED, "\')\'");
		}
	}
	else {
		p_log_->callError(ERR_EXPECTED, "\'(\'");
	}
}

// parse for statement
void Parser::parseForstmt() {
/*
<forstmt> = <(> <expression 1> <expression 2> <expression 3> <)> <stmt>
<expression 1> = <type> <iden> <vardef>
               | (<expr_max>) <;>
<expression 2> = (<expr_max>) <;>
<expression 3> = (<expr_max>)
*/
	std::string t_label1 = nextLabelName();
	std::string t_label2 = nextLabelName();
	std::string t_label3 = nextLabelName();
	std::string t_label4 = nextLabelName();
	generTuple(OP_BEGIN, "", "", "");
	increaseSymbolLevel();
	// (
	if (p_scanner_->token_now_.name_ == TOK_LRBRACKET) {
		p_scanner_->readToken();
		// <expression 1>
		if (detectType() == true) {
			readIden();
			checkType();
			parseVardef(false);
		}
		else {
			if (p_scanner_->token_now_.name_ != TOK_SEMICOLON) {
				parseExpr(MAX_EXPR_LEVEL);
			}
			// ;
			if (p_scanner_->token_now_.name_ == TOK_SEMICOLON) {
				p_scanner_->readToken();
			}
			else {
				p_log_->callError(ERR_EXPECTED, "\';\'");
			}
		}
		generTuple(OP_BEGIN, "", "", "");
		increaseSymbolLevel();
		// <expression 2>
		generTuple(OP_LABEL, "", "", t_label1);
		if (p_scanner_->token_now_.name_ != TOK_SEMICOLON) {
			parseExpr(MAX_EXPR_LEVEL);
			generTuple(OP_JE, expr_rst_now_, "0", t_label4);
		}
		generTuple(OP_JMP, "", "", t_label3);
		// ;
		if (p_scanner_->token_now_.name_ == TOK_SEMICOLON) {
			p_scanner_->readToken();
		}
		else {
			p_log_->callError(ERR_EXPECTED, "\';\'");
		}
		// <expression 3>
		generTuple(OP_LABEL, "", "", t_label2);
		if (p_scanner_->token_now_.name_ != TOK_RRBRACKET) {
			parseExpr(MAX_EXPR_LEVEL);
		}
		generTuple(OP_JMP, "", "", t_label1);
		// )
		if (p_scanner_->token_now_.name_ == TOK_RRBRACKET) {
			// statements
			generTuple(OP_LABEL, "", "", t_label3);
			p_scanner_->readToken();
			// already in a block
			if (p_scanner_->token_now_.name_ == TOK_LCBRACKET) {
				parseStmt();
			}
				// add into a block manually
			else {
				generTuple(OP_BEGIN, "", "", "");
				increaseSymbolLevel();
				parseStmt();
				generTuple(OP_END, "", "", "");
				decreaseSymbolLevel();
			}
			generTuple(OP_JMP, "", "", t_label2);
			generTuple(OP_LABEL, "", "", t_label4);
			generTuple(OP_END, "", "", "");
			decreaseSymbolLevel();
			generTuple(OP_END, "", "", "");
			decreaseSymbolLevel();
		}
		else {
			p_log_->callError(ERR_EXPECTED, ")");
		}
	}
	else {
		p_log_->callError(ERR_EXPECTED, "\'(\'");
	}
}

// parse while statement
void Parser::parseWhilestmt() {
/*
<whilestmt> = <(> <expr_max> <)> <stmt>
*/
	std::string t_label1 = nextLabelName();
	std::string t_label2 = nextLabelName();
	// (
	if (p_scanner_->token_now_.name_ == TOK_LRBRACKET) {
		generTuple(OP_LABEL, "", "", t_label1);
		p_scanner_->readToken();
		parseExpr(MAX_EXPR_LEVEL);
		generTuple(OP_JE, expr_rst_now_, "0", t_label2);
		// )
		if (p_scanner_->token_now_.name_ == TOK_RRBRACKET) {
			p_scanner_->readToken();
			// already in a block
			if (p_scanner_->token_now_.name_ == TOK_LCBRACKET) {
				parseStmt();
			}
				// add into a block manually
			else {
				generTuple(OP_BEGIN, "", "", "");
				increaseSymbolLevel();
				parseStmt();
				generTuple(OP_END, "", "", "");
				decreaseSymbolLevel();
			}
			generTuple(OP_JMP, "", "", t_label1);
			generTuple(OP_LABEL, "", "", t_label2);
		}
		else {
			p_log_->callError(ERR_EXPECTED, "\')\'");
		}
	}
	else {
		p_log_->callError(ERR_EXPECTED, "\'(\'");
	}
}

// parse do statement
void Parser::parseDostmt() {
/*
<dostmt> = <stmt> <while> <(> <expr_max> <)> <;>
*/
	std::string t_label1 = nextLabelName();
	generTuple(OP_LABEL, "", "", t_label1);
	// already in a block
	if (p_scanner_->token_now_.name_ == TOK_LCBRACKET) {
		parseStmt();
	}
		// add into a block manually
	else {
		generTuple(OP_BEGIN, "", "", "");
		increaseSymbolLevel();
		parseStmt();
		generTuple(OP_END, "", "", "");
		decreaseSymbolLevel();
	}
	// while
	if (p_scanner_->token_now_.name_ == TOK_WHILE) {
		p_scanner_->readToken();
		// (
		if (p_scanner_->token_now_.name_ == TOK_LRBRACKET) {
			p_scanner_->readToken();
			parseExpr(MAX_EXPR_LEVEL);
			// )
			if (p_scanner_->token_now_.name_ == TOK_RRBRACKET) {
				generTuple(OP_JNE, expr_rst_now_, "0", t_label1);
				p_scanner_->readToken();
				readSemicolon();
			}
			else {
				p_log_->callError(ERR_EXPECTED, "\')\'");
			}
		}
		else {
			p_log_->callError(ERR_EXPECTED, "\'(\'");
		}
	}
	else {
		p_log_->callError(ERR_EXPECTED, "\'while\'");
	}

}

// parse statement
void Parser::parseStmt() {
/*
<stmt> = <type> <iden> <vardef>
       | <block>
       | <if> <ifstmt>
       | <for> <forstmt>
       | <for> <whilestmt>
       | <do> <dostmt>
       | <;>
       | <return> <expr_max> <;>
       | <expr_max> <;>
*/
	// type
	if (detectType() == true) {
		readIden();
		checkType();
		parseVardef(false);
	}
	else {
		switch (p_scanner_->token_now_.name_) {
			// {
			case TOK_LCBRACKET:
				parseBlock();
				break;
				// if
			case TOK_IF:
				p_scanner_->readToken();
				parseIfstmt();
				break;
				// else
			case TOK_ELSE:
				p_scanner_->readToken();
				p_log_->callError(ERR_ELSENOIF);
				break;
				// for
			case TOK_FOR:
				p_scanner_->readToken();
				parseForstmt();
				break;
				// while
			case TOK_WHILE:
				p_scanner_->readToken();
				parseWhilestmt();
				break;
				// do
			case TOK_DO:
				p_scanner_->readToken();
				parseDostmt();
				break;
				// ;
			case TOK_SEMICOLON:
				p_scanner_->readToken();
				break;
				// return
			case TOK_RETURN:
				p_scanner_->readToken();
				parseExpr(MAX_EXPR_LEVEL);
				generTuple(OP_RETURN, "", "", expr_rst_now_);
				generTuple(OP_JMP, "", "", func_end_label_now_);
				readSemicolon();
				break;
				// expression
			default:
				if (!parseExpr(MAX_EXPR_LEVEL)) {
					p_scanner_->readToken();
					break;
				}
				readSemicolon();
				break;
		}
	}
}

// parse block
void Parser::parseBlock() {
/*
<block> = <{> (<stmt>)* <}>
*/
	// {
	if (p_scanner_->token_now_.name_ == TOK_LCBRACKET) {
		generTuple(OP_BEGIN, "", "", "");
		increaseSymbolLevel();
		p_scanner_->readToken();
		while(p_scanner_->token_now_.name_ != TOK_RCBRACKET) {
			if (p_scanner_->token_now_.name_ == TOK_EOF) {
				break;
			}
			parseStmt();
		}
		// }
		if (p_scanner_->token_now_.name_ == TOK_RCBRACKET) {
			generTuple(OP_END, "", "", "");
			decreaseSymbolLevel();
			p_scanner_->readToken();
		}
		else {
			p_log_->callError(ERR_EXPECTED, "\'}\'");
		}
	}
	else {
		p_log_->callError(ERR_EXPECTED, "\'{\'");
	}
}

// parse variable definition
void Parser::parseVardef(bool is_global) {
/*
<globdef> = (<type>) <iden> <vardef>
          | (<type>) <iden> <funcdef>
<vardef> = <[> <expr_sub> <]> <vardef>
         | <=> <expr_sub> <,> <iden> <vardef>
         | <=> <expr_sub> <;>
         | <,> <iden> <vardef>
         | <;>
*/
	std::string t_result;
	switch (p_scanner_->token_now_.name_) {
		// [
		case TOK_LSBRACKET:
			p_scanner_->readToken();
			parseExpr(SUB_EXPR_LEVEL);
			// ]
			if (p_scanner_->token_now_.name_ == TOK_RSBRACKET) {
				p_scanner_->readToken();
				parseVardef(is_global);
			}
			else {
				p_log_->callError(ERR_EXPECTED, "\']\'");
			}
			break;
			// =
		case TOK_ASSIGN:
			p_scanner_->readToken();
			// write down
			if (is_global) {
				addGlobalName(iden_now_);
			}
			else {
				addLocalVar(iden_now_);
			}
			generTuple(OP_VARDEF, type_now_, iden_now_);
			// assignment
			t_result = iden_now_;
			parseExpr(SUB_EXPR_LEVEL);
			generTuple(OP_ASSIGN, expr_rst_now_, "", t_result);
			// go on
			switch (p_scanner_->token_now_.name_) {
				// ,
				case TOK_COMMA:
					p_scanner_->readToken();
					readIden();
					parseVardef(is_global);
					break;
					// ;
				case TOK_SEMICOLON:
					p_scanner_->readToken();
					break;
				default:
					p_log_->callError(ERR_EXPECTED, "\',\', or \';\'");
					break;
			}
			break;
			// ,
		case TOK_COMMA:
			p_scanner_->readToken();
			// write down
			if (is_global) {
				addGlobalName(iden_now_);
			}
			else {
				addLocalVar(iden_now_);
			}
			generTuple(OP_VARDEF, type_now_, iden_now_);
			// go on
			readIden();
			parseVardef(is_global);
			break;
			// ;
		case TOK_SEMICOLON:
			// write down
			if (is_global) {
				addGlobalName(iden_now_);
			}
			else {
				addLocalVar(iden_now_);
			}
			generTuple(OP_VARDEF, type_now_, iden_now_);
			// go on
			p_scanner_->readToken();
			break;
		default:
			p_log_->callError(ERR_EXPECTED, "\'=\', \',\', or \';\'");
			break;
	}
}

// parse parameter definition
void Parser::parseParamdef() {
/*
<paramdef> = <[> <]> <paramdef>
           | <[> <expr_sub> <]> <paramdef>
           | <=> <expr_sub>
           | <=> <expr_sub> <,> (<type>) <iden> <paramdef>
           | <,> (<type>) <iden> <paramdef>
           | (null)
*/
	std::string t_result;
	switch (p_scanner_->token_now_.name_)
	{
		// [
		case TOK_LSBRACKET:
			p_scanner_->readToken();
			// ]
			if (p_scanner_->token_now_.name_ == TOK_RSBRACKET) {
				p_scanner_->readToken();
				parseParamdef();
			}
			else {
				parseExpr(SUB_EXPR_LEVEL);
				if (p_scanner_->token_now_.name_ == TOK_RSBRACKET) {
					p_scanner_->readToken();
					parseParamdef();
				}
				else {
					p_log_->callError(ERR_EXPECTED, "\']\'");
				}
			}
			break;
			// =
		case TOK_ASSIGN:
			p_scanner_->readToken();
			t_result = iden_now_;
			parseExpr(SUB_EXPR_LEVEL);
			generTuple(OP_ASSIGN, expr_rst_now_, "", t_result);
			if (p_scanner_->token_now_.name_ == TOK_COMMA) {
				p_scanner_->readToken();
				detectType();
				readIden();
				checkType();
				addParam(iden_now_);
				generTuple(OP_PARAM, type_now_, iden_now_);
				parseParamdef();
			}
			break;
			// ,
		case TOK_COMMA:
			p_scanner_->readToken();
			detectType();
			readIden();
			checkType();
			addParam(iden_now_);
			generTuple(OP_PARAM, type_now_, iden_now_);
			parseParamdef();
			break;
		default:
			break;
	}
}

// parse function definition
void Parser::parseFuncdef() {
/*
<globdef> = (<type>) <iden> <vardef>
          | (<type>) <iden> <funcdef>
<funcdef> = <(> <)> <block>
          | <(> (<type>) <iden> <paramdef> <)> <block>
*/
	std::string t_label1 = nextLabelName();
	func_end_label_now_ = t_label1;
	addGlobalName(iden_now_);
	generTuple(OP_FUNCDEF, type_now_, iden_now_);
	generTuple(OP_BEGIN, "", "", "");
	increaseSymbolLevel();
	// (
	if (p_scanner_->token_now_.name_ == TOK_LRBRACKET) {
		p_scanner_->readToken();
		// parameter list
		if (p_scanner_->token_now_.name_ != TOK_RRBRACKET) {
			detectType();
			readIden();
			checkType();
			addParam(iden_now_);
			generTuple(OP_PARAM, type_now_, iden_now_);
			parseParamdef();
		}
		// )
		if (p_scanner_->token_now_.name_ == TOK_RRBRACKET) {
			p_scanner_->readToken();
			parseBlock();
			generTuple(OP_LABEL, "", "", t_label1);
			generTuple(OP_END, "", "", "");
			decreaseSymbolLevel();
		}
		else {
			p_log_->callError(ERR_EXPECTED, "\')\'");
		}
	}
	else {
		p_log_->callError(ERR_EXPECTED, "\'(\'");
	}
}

// parse global definition
void Parser::parseGlobdef() {
/*
<globdef> = <;>
          | (<type>) <iden> <vardef>
          | (<type>) <iden> <funcdef>
*/
	switch (p_scanner_->token_now_.name_) {
		// ;
		case TOK_SEMICOLON:
			p_scanner_->readToken();
			break;
		default:
			detectType();
			if (!readIden()) {
				p_scanner_->readToken();
				break;
			}
			checkType();
			switch (p_scanner_->token_now_.name_) {
				// (
				case TOK_LRBRACKET:
					parseFuncdef();
					break;
					// [ = , ;
				case TOK_LSBRACKET:
				case TOK_ASSIGN:
				case TOK_COMMA:
				case TOK_SEMICOLON:
					parseVardef(true);
					break;
				default:
					p_log_->callError(ERR_EXPECTED, "\'=\', \',\', or \';\'");
					break;
			}
	}
}

// parse program
void Parser::parseProgram() {
/*
<program> = {<globdef>}*
*/
	increaseSymbolLevel();
	while(p_scanner_->token_now_.name_ != TOK_EOF) {
		parseGlobdef();
	}
}

// next label name for generating quadruple
std::string Parser::nextLabelName() {
	std::stringstream ss;
	ss << (++label_num_);
	return "L" + ss.str();
}

// next variable name for generating quadruple
std::string Parser::nextTempVarName() {
	std::stringstream ss;
	ss << (++temp_var_num_);
	std::string t_varname = ".V" + ss.str();
	addLocalVar(t_varname);
	return t_varname;
}

void Parser::increaseSymbolLevel() {
	std::unordered_map<std::string, int> sub_table;
	symbol_table_.push_back(sub_table);
}

void Parser::decreaseSymbolLevel() {
	symbol_table_.pop_back();
}

bool Parser::hasSymbol(const std::string& name) {
	bool t_has = false;
	for (int i = symbol_table_.size() - 1; i >= 0; i--) {
		if (symbol_table_[i].find(name) != symbol_table_[i].end()) {
			t_has = true;
			break;
		}
	}
	return t_has;
}

void Parser::addGlobalName(const std::string& name) {
	if (symbol_table_[0].find(name) == symbol_table_[0].end()) {
		symbol_table_[0][name] = 0;
	}
	else {
		p_log_->callError(ERR_REDEFINITION, name.c_str());
	}
}

void Parser::addParam(const std::string& name) {
	int t_back = symbol_table_.size() - 1;
	if (symbol_table_[t_back].find(name) == symbol_table_[t_back].end()) {
		symbol_table_[t_back][name] = ++param_num_;
	}
	else {
		p_log_->callError(ERR_REDEFINITION, name.c_str());
	}
}

void Parser::addLocalVar(const std::string& name) {
	int t_back = symbol_table_.size() - 1;
	if (symbol_table_[t_back].find(name) == symbol_table_[t_back].end()) {
		symbol_table_[t_back][name] = --local_var_num_;
	}
	else {
		p_log_->callError(ERR_REDEFINITION, name.c_str());
	}
}

int Parser::getVarOffset(const std::string& name) {
	int t_offset = 0;
	for (size_t i = symbol_table_.size() - 1; i >= 0; i--) {
		if (symbol_table_[i].find(name) != symbol_table_[i].end()) {
			t_offset = symbol_table_[i][name];
			break;
		}
	}
	return t_offset;
}

// generate quadruple
void Parser::generTuple(Option op, const std::string& arg1,
                        const std::string& arg2, const std::string& result) {
	std::string t_arg1 = arg1, t_arg2 = arg2, t_result = result;
	// tranform local variable name to offset
	if (hasSymbol(arg1)) {
		int t_offset = getVarOffset(arg1);
		t_arg1 =
				(t_offset != 0) ? ("@" + gb::to_string(t_offset)) : ("_" + arg1);
	}
	else if (arg1 == "@@") {
		t_arg1 = arg1;
	}
	if (hasSymbol(arg2)) {
		int t_offset = getVarOffset(arg2);
		t_arg2 =
				(t_offset != 0) ? ("@" + gb::to_string(t_offset)) : ("_" + arg2);
	}
	if (hasSymbol(result)) {
		int t_offset = getVarOffset(result);
		t_result =
				(t_offset != 0) ? ("@" + gb::to_string(t_offset)) : ("_" + result);
	}
	// push quadruple into vector
	intermediate_code_.push_back(Tuple(op, t_arg1, t_arg2, t_result));
	// output into the file
	ofstream_ << OptionToString[op] << "\t, " << t_arg1 << "\t, "
	          << t_arg2 << "\t, " << t_result << std::endl;
}

void Parser::generTuple(Option op, Type type, const std::string& result) {
	std::string t_result = result;
	// tranform local variable name to offset
	if (hasSymbol(result)) {
		int t_offset = getVarOffset(result);
		t_result =
				(t_offset != 0) ? ("@" + gb::to_string(t_offset)) : ("_" + result);
	}
	// push quadruple into vector
	intermediate_code_.push_back(Tuple(op, type, t_result));
	// output into the file
	ofstream_ << OptionToString[op] << "\t, "
	          << type.toString() << "\t, " << t_result << std::endl;
}
