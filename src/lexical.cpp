//
// Created by 17110 on 2020/5/5.
//

/* lexical.cpp */

#include "../headers/lexical.h"

/// global variables

std::unordered_map<std::string, TokenName> StringToToken ({
		                                                          {"const"   , TOK_CONST}    ,
		                                                          {"signed"  , TOK_SIGNED}   , {"unsigned", TOK_UNSIGNED} ,
		                                                          {"void"    , TOK_VOID}     ,
		                                                          {"char"    , TOK_CHAR}     , {"int"     , TOK_INT}      ,
		                                                          {"short"   , TOK_SHORT}    , {"long"    , TOK_LONG}     ,
		                                                          {"float"   , TOK_FLOAT}    , {"double"  , TOK_DOUBLE}   ,
		                                                          {"if"      , TOK_IF}       , {"else"    , TOK_ELSE}     ,
		                                                          {"for"     , TOK_FOR}      , {"while"   , TOK_WHILE}    ,
		                                                          {"do"      , TOK_DO}       , {"return"  , TOK_RETURN}   ,
		                                                          {","       , TOK_COMMA}    , {";"       , TOK_SEMICOLON},
		                                                          {"("       , TOK_LRBRACKET}, {")"       , TOK_RRBRACKET},
		                                                          {"["       , TOK_LSBRACKET}, {"]"       , TOK_RSBRACKET},
		                                                          {"{"       , TOK_LCBRACKET}, {"}"       , TOK_RCBRACKET},
		                                                          {"++"      , TOK_INCREMENT}, {"--"      , TOK_DECREMENT},
		                                                          {"~"       , TOK_BITNOT}   , {"!"       , TOK_LOGNOT}   ,
		                                                          {"*"       , TOK_MUL}      , {"/"       , TOK_DIV}      ,
		                                                          {"%"       , TOK_MOD}      ,
		                                                          {"+"       , TOK_ADD}      , {"-"       , TOK_SUB}      ,
		                                                          {"<<"      , TOK_SAL}      , {">>"      , TOK_SAR}      ,
		                                                          {"<"       , TOK_LT}       , {">"       , TOK_GT}       ,
		                                                          {"<="      , TOK_LE}       , {">="      , TOK_GE}       ,
		                                                          {"=="      , TOK_ET}       , {"!="      , TOK_NE}       ,
		                                                          {"&"       , TOK_BITAND}   , {"^"       , TOK_BITXOR}   ,
		                                                          {"|"       , TOK_BITOR}    ,
		                                                          {"&&"      , TOK_LOGAND}   , {"||"      , TOK_LOGOR}    ,
		                                                          {"?"       , TOK_QUESTION} , {":"       , TOK_COLON}    ,
		                                                          {"="       , TOK_ASSIGN}   ,
                                                          });

std::unordered_set<char> BlankSet ({
		                                   ' ', '\t', '\v', '\f', '\n', '\r',
                                   });

std::unordered_map<char, char> CharToEscape ({
		                                             {'a', '\a'}, {'b', '\b'}, {'t', '\t'}, {'v', '\v'},
		                                             {'f', '\f'}, {'n', '\n'}, {'r', '\r'},
                                             });

/// public

Scanner::Scanner(Log* p_log) {
	p_log_ = p_log;
	index_in_line_ = 0;
	row_of_line_ = 0;
	ifstream_.open(CONSOLE_NAME, std::ios::in);
}

Scanner::~Scanner() {
	p_log_ = nullptr;
	ifstream_.close();
}

void Scanner::readChar() {
	/* If index reach the end of buffer line, read the next line.
	   Otherwise, read the next char of current index. */
	if (row_of_line_ == 0 || index_in_line_ == buf_line_.length()) {
		if (std::getline(ifstream_, buf_line_)) {
			char_now_ = '\n';
			row_of_line_++;
			index_in_line_ = 0;
		}
		else {
			char_now_ = EOF;
		}
	}
	else {
		char_now_ = buf_line_[index_in_line_];
		index_in_line_++;
	}
}

void Scanner::readToken() {
	// In LL(1), line number should be the row of previous token.
	p_log_->line_num_ = row_of_line_;

	// end of file
	if (char_now_ == EOF) {
		token_now_.name_ = TOK_EOF;
	}
		// blank character(s)
	else if (BlankSet.find(char_now_) != BlankSet.end()) {
		while(BlankSet.find(char_now_) != BlankSet.end()) {
			readChar();
		}
		token_now_.name_ = TOK_BLANK;
	}
		// identifier or keyword
	else if (char_now_ == '_' || isalpha(char_now_)) {
		std::string t_word;
		while(char_now_ == '_' || isalnum(char_now_)) {
			t_word += char_now_;
			readChar();
		}
		// keyword
		if (StringToToken.find(t_word) != StringToToken.end()) {
			token_now_.name_ = StringToToken[t_word];
		}
			// identifier
		else {
			token_now_.name_ = TOK_IDEN;
			token_now_.value_.iden_ = t_word;
		}
	}
		// integer constant or float constant
	else if (isdigit(char_now_)) {
		// handle as unsigned long long first
		typedef ullong integer_t;
		integer_t t_integer_val = 0;
		while(isdigit(char_now_)) {
			t_integer_val *= 10;
			t_integer_val += (integer_t)(char_now_ - '0');
			readChar();
		}
		// floating constant
		if (char_now_ == '.') {
			// handle as long double first
			typedef ldouble floating_t;
			floating_t t_floating_val = (floating_t)t_integer_val;
			floating_t t_decimal = 0.1;
			readChar();
			while (isdigit(char_now_)) {
				t_floating_val += (integer_t)(char_now_ - '0') * t_decimal;
				t_decimal /= 10;
				readChar();
			}
			// suppose have no suffix
			token_now_.name_ = TOK_DOUBLECST;
			token_now_.value_.double_ = (double)t_floating_val;
			// have suffix
			if (isalpha(char_now_)) {
				std::string t_suffix = "";
				while (isalpha(char_now_)) {
					t_suffix += char_now_;
					readChar();
				}
				std::string t_lowersuffix = gb::to_lower(t_suffix);
				if (t_lowersuffix == "f") {
					token_now_.name_ = TOK_FLOATCST;
					token_now_.value_.float_ = (float)(t_floating_val);
				}
				else if (t_lowersuffix == "d") {
					token_now_.name_ = TOK_DOUBLECST;
					token_now_.value_.double_ = (double)(t_floating_val);
				}
				else if (t_lowersuffix == "l") {
					token_now_.name_ = TOK_LDOUBLECST;
					token_now_.value_.ldouble_ = (ldouble)(t_floating_val);
				}
				else {
					p_log_->callError(ERR_INVALIDSUFFIX,
					                  t_suffix.c_str(), "floating");
				}
			}
		}
			// integer constant
		else {
			// suppose have no suffix
			// convert to fit type
			if (t_integer_val < (1LL << 31)) {
				token_now_.name_ = TOK_INTCST;
				token_now_.value_.int_ = (int)t_integer_val;
			}
			else if (t_integer_val < (1LL << 32)) {
				token_now_.name_ = TOK_UINTCST;
				token_now_.value_.uint_ = (uint)t_integer_val;
			}
			else if (t_integer_val < (1LL << 63)) {
				token_now_.name_ = TOK_LLONGCST;
				token_now_.value_.llong_ = (llong)t_integer_val;
			}
			else {
				token_now_.name_ = TOK_ULLONGCST;
				token_now_.value_.ullong_ = (ullong)t_integer_val;
			}
			// have suffix
			if (isalpha(char_now_)) {
				std::string t_suffix = "";
				while (isalpha(char_now_)) {
					t_suffix += char_now_;
					readChar();
				}
				std::string t_lowersuffix = gb::to_lower(t_suffix);
				std::sort(t_lowersuffix.begin(), t_lowersuffix.end());
				if (t_lowersuffix == "l") {
					if (token_now_.name_ == TOK_INTCST) {
						token_now_.name_ = TOK_LONGCST;
						token_now_.value_.long_ = (long)(t_integer_val);
					}
					else if (token_now_.name_ == TOK_UINTCST) {
						token_now_.name_ = TOK_ULONGCST;
						token_now_.value_.ulong_ = (ulong)(t_integer_val);
					}
				}
				else if (t_lowersuffix == "u") {
					if (token_now_.name_ == TOK_INTCST) {
						token_now_.name_ = TOK_UINTCST;
						token_now_.value_.uint_ = (uint)(t_integer_val);
					}
					else if (token_now_.name_ == TOK_LLONGCST) {
						token_now_.name_ = TOK_ULLONGCST;
						token_now_.value_.ullong_ = (ullong)(t_integer_val);
					}
				}
				else if (t_lowersuffix == "ll") {
					if (token_now_.name_ == TOK_INTCST) {
						token_now_.name_ = TOK_LLONGCST;
						token_now_.value_.llong_ = (llong)(t_integer_val);
					}
					else if (token_now_.name_ == TOK_UINTCST) {
						token_now_.name_ = TOK_ULLONGCST;
						token_now_.value_.ullong_ = (ullong)(t_integer_val);
					}
				}
				else if (t_lowersuffix == "lu") {
					if (token_now_.name_ == TOK_INTCST
					    || token_now_.name_ == TOK_UINTCST) {
						token_now_.name_ = TOK_ULONGCST;
						token_now_.value_.ulong_ = (ulong)(t_integer_val);
					}
					else if (token_now_.name_ == TOK_LLONGCST) {
						token_now_.name_ = TOK_ULLONGCST;
						token_now_.value_.ullong_ = (ullong)(t_integer_val);
					}
				}
				else if (t_lowersuffix == "llu") {
					token_now_.name_ = TOK_ULLONGCST;
					token_now_.value_.ullong_ = (ullong)(t_integer_val);
				}
				else {
					p_log_->callError(ERR_INVALIDSUFFIX,
					                  t_suffix.c_str(), "floating");
				}
			}
		}
	}
		// character constant or string constant
	else if (char_now_ == '\'' || char_now_ == '\"') {
		// Handle char as string first
		char t_quote = char_now_;
		std::string t_string_val = "";
		readChar();
		while(char_now_ != t_quote && char_now_ != '\n'
		      && char_now_ != EOF) {
			// newline or escape character
			if (char_now_ == '\\') {
				readChar();
				// end of file
				if (char_now_ == EOF) {
					break;
				}
					// newline
				else if (char_now_ == '\n') {
					readChar();
					continue;
				}
					// escape character
				else if (CharToEscape.find(char_now_) != CharToEscape.end()) {
					t_string_val += CharToEscape[char_now_];
				}
					// non-escape character
				else {
					t_string_val += char_now_;
				}
			}
			else {
				t_string_val += char_now_;
			}
			readChar();
		}
		// char
		if (t_quote == '\'') {
			// missing terminating single quote mark
			if (char_now_ != '\''){
				token_now_.name_ = TOK_ERROR;
				p_log_->callError(ERR_MISSTRM, "\'");
			}
			else {
				// empty
				if (t_string_val.length() == 0) {
					token_now_.name_ = TOK_ERROR;
					p_log_->callError(ERR_EMPTYCHARCST);
				}
					// multi-character
				else if (t_string_val.length() > 1) {
					token_now_.name_ = TOK_CHARCST;
					token_now_.value_.char_ = t_string_val.back();
					p_log_->callWarning(WARN_MULTICHARCST);
				}
				else {
					token_now_.value_.char_ = t_string_val.back();
					token_now_.name_ = TOK_CHARCST;
				}
				readChar();
			}
		}
			// double
		else if (t_quote == '\"') {
			// missing terminating double quote mark
			if (char_now_ != '\"') {
				token_now_.name_ = TOK_ERROR;
				p_log_->callError(ERR_MISSTRM, "\"");
			}
			else {
				token_now_.name_ = TOK_STRINGCST;
				token_now_.value_.string_ = t_string_val;
				readChar();
			}
		}
	}
		// other tokens
	else {
		std::string t_symbol(1, char_now_);  // convert char to string
		// recognizable character
		/* It should be ensured that each prefix of each
		   multi-character token is also a token. */
		if (StringToToken.find(t_symbol) != StringToToken.end()) {
			readChar();
			while(StringToToken.find(t_symbol + char_now_) != StringToToken.end()) {
				t_symbol += char_now_;
				readChar();
			}
			token_now_.name_ = StringToToken[t_symbol];
		}
			// unrecognizable character
		else {
			token_now_.name_ = TOK_ERROR;
			p_log_->callError(ERR_UNRECOGCHAR);
			readChar();
		}
	}
	// pass blank token and token with error
	if (token_now_.name_ == TOK_BLANK || token_now_.name_ == TOK_ERROR) {
		readToken();
	}
}
