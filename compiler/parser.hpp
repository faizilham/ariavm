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
		vector<string> main, funcs, *target_code;
		map<string, ident_t> globals;
		int next; int label; bool infunc;
		int globalvars;
		
		void add_code(string);
		void add_code(int);
		void add_line();
		void settarget(vector<string>&);
		string newlabel();
		
		Token& match(string);
		Token& match(int);
		
		void assignstat();
		void incrstat(ident_t);
		
		
		// void funcexpr();
		void opexpr();
		void expression();
		
		ident_t getIdent(Token&);
		
		void idenstat();
		void whilestat();
		void statements();
		void printstat();
		void readstat();
		void ifstat();
		void __readcond(string, string, vector<string>&);
		void __readact(string, string, vector<string>&);
		
		void repeatstat();
		
		bool eof();
		
		Token& consume();
		bool lookAhead(string);
		bool lookAhead(int);
		
		
	public:
		Parser(istream&);
		~Parser();
		
		Token& getLastProcessed();
		vector<string>& getCode();
		void parse();
		void printCode(ostream&);
};

#endif