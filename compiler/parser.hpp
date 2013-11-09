#ifndef PARSER_HPP
#define PARSER_HPP

#include "tokenizer.hpp"
#include <map>
#include <sstream>

typedef struct {
	int line;
	bool declared;
	bool variable;
	int memory;
} ident_t;

static string to_str(int n){
	stringstream ss;
	ss << n;
	return ss.str();
}

class Parser{
	private:
		Tokenizer tokens;
		vector<string> main, funcs;
		map<string, ident_t> identifiers;
		int next; int label; bool infunc;
		int vars;
		
		void add_code(string);
		void add_code(int);
		void add_line();
		
		Token& match(string);
		Token& match(int);
		
		void assignstat();
		void incrstat(ident_t);
		
		// void funcexpr();
		void opexpr();
		void expression();
		
		void idenstat();
		void whilestat();
		void statements();
		void printstat();
		
		bool eof();
		
		Token& consume();
		bool lookAhead(string);
		bool lookAhead(int);
		
		
	public:
		Parser(istream&);
		~Parser();
		
		Token& getLastProcessed();
		void parse();
		void printCode();
};

#endif