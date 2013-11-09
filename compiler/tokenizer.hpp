#ifndef TOKENIZER_HPP
#define TOKENIZER_HPP

#include <iostream>
#include <string>
#include <vector>

using namespace std;

typedef struct{
	string value;
	int type;
	int line;
} Token;

/******* char types **********/
static const int INVALID = -1;
static const int WHITESPACE = 2;
static const int OPERATOR = 3;
static const int ALPHANUMERIC = 4;
static const int NEWLINE = 5;

// single chars only
static const int PARENTHESES = 10;
static const int COMMA = 11;
static const int COMMENT = 12;

/****** token types ********/
static const int T_RESERVED = 1;
static const int T_IDENTIFIER = 2;
static const int T_INTEGER = 3;
static const int T_OPERATOR = 4;
static const int T_ASSIGNMENT = 5;
static const int T_INCDEC = 6;
static const int T_PAR_OPEN = 7;
static const int T_PAR_CLOSE = 8;
static const int T_COMMA = 9;
static const int T_NEWLINE = 10;

static const string math_op[] = {"+", "-", "*", "/", "%"}; static const int math_op_size = 5;

static const string logic_op[] = {"!", "&", "|"}; static const int logic_op_size = 3;

static const string test_op[] = {"==", "!=", "<", ">", "<=", ">=", "=>", "=<"}; static const int test_op_size = 8;

static const string assign_op[] = {"="}; static const int assign_op_size = 1;
static const string incdec_op[] = {"++", "--"}; static const int incdec_op_size = 1;

static const string reserved_words[] = {"if", "elseif", "else", "while", "repeat", "until", "define", "return", "end"};
static const int reserved_words_size = 9;

static bool exist(string value, const string array[], int size){
	for (int i = 0; i < size; i++)
		if (array[i] == value) return true;
	return false;
}

class Tokenizer{
	private:
		vector<Token> tokens; istream& input;
		
		char next; char buffer[64]; int line; bool comment;
		
		bool skip_whitespace();
		void skip_comment();
		void next_token();
		void init();
		void recognize(string);
		char consume();
		
	public:
		Tokenizer(istream&);
		~Tokenizer();
		void tokenize();
		void print();
		static int getType(char);
		int getline() const;
		
};

#endif