//
// Created by 17110 on 2020/5/5.
//

#ifndef C0_SJX_LEXICAL_H
#define C0_SJX_LEXICAL_H

#include "global.h"
#include "log.h"

enum TokenName {
	TOK_EOF = -1,   //      (end of file)
	TOK_NULL,       //      (null)
	TOK_ERROR,      //      (token with error)
	TOK_IDEN,       //      (identifier)
	// keywords
	TOK_CONST,      //      const
	TOK_SIGNED,     //      signed
	TOK_UNSIGNED,   //      unsigned
	TOK_VOID,       //      void
	TOK_CHAR,       //      char
	TOK_INT,        //      int
	TOK_SHORT,      //      short
	TOK_LONG,       //      long
	TOK_FLOAT,      //      float
	TOK_DOUBLE,     //      double
	TOK_IF,         //      if
	TOK_ELSE,       //      else
	TOK_FOR,        //      for
	TOK_WHILE,      //      while
	TOK_DO,         //      do
	TOK_RETURN,     //      return
	// literal
	TOK_INTCST,     //      integer constant
	TOK_UINTCST,    //      unsigned integer constant
	TOK_LONGCST,    //      long integer constant
	TOK_ULONGCST,   //      unsigned long integer constant
	TOK_LLONGCST,   //      long long integer constant
	TOK_ULLONGCST,  //      unsigned long long integer constant
	TOK_FLOATCST,   //      single floating constant
	TOK_DOUBLECST,  //      double floating constant
	TOK_LDOUBLECST, //      long double floating constant
	TOK_CHARCST,    //      character constant
	TOK_STRINGCST,  //      string constant
	// delimiter
	TOK_BLANK,      //      (blank characters)
	TOK_COMMA,      //  ,   comma
	TOK_SEMICOLON,  //  ;   Semicolon
	// operators
	TOK_LRBRACKET,  //  (   left round bracket
	TOK_RRBRACKET,  //  )   right round bracket
	TOK_LSBRACKET,  //  [   left square bracket
	TOK_RSBRACKET,  //  ]   right square bracket
	TOK_LCBRACKET,  //  {   left curly bracket
	TOK_RCBRACKET,  //  }   right curly bracket
	TOK_INCREMENT,  //  ++  increment
	TOK_DECREMENT,  //  --  decrement
	TOK_BITNOT,     //  ~   bitwise not
	TOK_LOGNOT,     //  !   logical not
	TOK_MUL,        //  *   multiplication / dereference
	TOK_DIV,        //  /   division
	TOK_MOD,        //  %   modulo
	TOK_ADD,        //  +   addition
	TOK_SUB,        //  -   subtraction
	TOK_SAL,        //  <<  shift arithmetic left
	TOK_SAR,        //  >>  shift arithmetic right
	TOK_LT,         //  <   less than
	TOK_GT,         //  >   greater than
	TOK_LE,         //  <=  less than or equal to
	TOK_GE,         //  >=  greater than or equal to
	TOK_ET,         //  ==  equal to
	TOK_NE,         //  !=  not equal to
	TOK_BITAND,     //  &   bitwise and / reference
	TOK_BITXOR,     //  ^   bitwise exclusive or
	TOK_BITOR,      //  |   bitwise or
	TOK_LOGAND,     //  &&  logical and
	TOK_LOGOR,      //  ||  logical or
	TOK_QUESTION,   //  ?   question mark
	TOK_COLON,      //  :   colon
	TOK_ASSIGN,     //  =   assignment
};

class TokenValue {
public:
	std::string iden_;
	int int_;
	uint uint_;
	long long_;
	ulong ulong_;
	llong llong_;
	ullong ullong_;
	float float_;
	double double_;
	ldouble ldouble_;
	char char_;
	std::string string_;
};

class Token {
public:
	TokenName name_;
	TokenValue value_;
};

class Scanner {
public:
	Scanner(Log*);
	~Scanner();
	Log *p_log_;
	std::ifstream ifstream_;
	char char_now_;
	Token token_now_;
	std::string buf_line_;
	size_t index_in_line_;  // index of buffer character in buffer line
	size_t row_of_line_;  // row of buffer line in source file
	void readChar();
	void readToken();
};

#endif //C0_SJX_LEXICAL_H
