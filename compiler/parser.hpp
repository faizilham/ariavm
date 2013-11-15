#ifndef PARSER_HPP
#define PARSER_HPP

#include "tokenizer.hpp"
#include <map>
#include <sstream>

typedef struct {
	int line;
	string label;
	bool variable;
	int memory;
} ident_t;

static string to_str(int n){
	stringstream ss;
	ss << n;
	return ss.str();
}

static const string R_IF = "if";
static const string R_ELSEIF = "elseif";
static const string R_ELSE = "else";
static const string R_WHILE = "while";
static const string R_REPEAT = "repeat";
static const string R_UNTIL = "until";
static const string R_DEFINE = "define";
static const string R_RETURN = "return";
static const string R_END = "end";
static const string R_PRINT = "print";
static const string R_READ = "read";

class Parser{
	private:
		Tokenizer tokens;
		string module_name;
		vector<string> main, funcs, *target_code;
		map<string, ident_t> globals, locals, *symbol_table;
		int next; int label; bool infunc;
		int globalvars;
		
		void add_code(string);
		void add_code(int);
		void add_line();
		void settarget(vector<string>&);
		void settable(map<string, ident_t>&);
		string newlabel();
		
		Token& match(string);
		Token& match(int);
		
		void assignstat();
		void incrstat(ident_t);
		
		
		void funcexpr(ident_t);
		void opexpr();
		void expression();
		
		ident_t getVar(Token&);
		ident_t getFunc(Token&);
		int identExist(Token&);
		ident_t getParam(string, Token&);
		
		void idenstat();
		void whilestat();
		void statements();
		void printstat();
		void readstat();
		void ifstat();
		void definestat();
		void returnstat();
		void endstat();
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