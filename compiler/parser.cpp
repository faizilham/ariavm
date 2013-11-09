#include "parser.hpp"
#include <stdexcept>

Parser::Parser(istream& input) : tokens(input){
	next = 0;
	label = 0; vars = 0;
	infunc = false;
	try{
		tokens.tokenize();
	}catch(runtime_error& e){
		cout<<"Error on line "<<tokens.getline()<<": "<<e.what()<<endl;
		throw runtime_error("Compile aborted");
	}
}

Parser::~Parser(){}
Token& Parser::consume(){
	return tokens.at(next++);
}
bool Parser::lookAhead(string value){
	return tokens.at(next).value == value;
}
bool Parser::lookAhead(int type){
	return tokens.at(next).type == type;
}
Token& Parser::match(string value){
	if (!eof() && lookAhead(value)) return consume();
	else throw runtime_error("no match " + value);
}
Token& Parser::match(int type){
	if(!eof() && lookAhead(type)) return consume();
	else throw runtime_error("no match type " + to_str(type));
}

void Parser::add_code(string code){
	if(infunc)
		funcs.push_back(code);
	else
		main.push_back(code);
}

void Parser::add_code(int code){
	add_code(to_str(code));
}

void Parser::add_line(){
	add_code("\n");
}

bool Parser::eof(){
	return next == tokens.size();
}

void Parser::opexpr(){
	string val = match(T_OPERATOR).value;
	expression();
	
	if (val == "+"){
		add_code("add"); add_line();
	}else if (val == "-"){
		add_code("sub"); add_line();
	}else if (val == "*"){
		add_code("mul"); add_line();
	}else if (val == "/"){
		add_code("div"); add_line();
	}else if (val == "<"){
		add_code("lt"); add_line();
	}else if (val == ">"){
		add_code("gt"); add_line();
	}else if(val == "=="){
		add_code("eq"); add_line();
	}else if(val == "!="){
		add_code("neq"); add_line();
	}else if((val == ">=") || (val == "=>")){
		add_code("gte"); add_line();
	}else if((val == "<=") || (val == "=<")){
		add_code("lte"); add_line();
	}
}

void Parser::expression(){
	if (lookAhead(T_INTEGER)){
		add_code("push"); add_code(match(T_INTEGER).value); add_line();
		
		if (lookAhead(T_OPERATOR)){
			opexpr(); return;
		}
		else if (lookAhead(T_NEWLINE) || lookAhead(T_PAR_CLOSE)) return;

	}else if (lookAhead(T_IDENTIFIER)){
		string ident = match(T_IDENTIFIER).value;
		if (identifiers.find(ident) == identifiers.end())
			throw runtime_error("Identifier " + ident + " is undeclared / unassigned");
		
		ident_t idt = identifiers[ident];
		
		if(idt.variable){
			add_code("load"); add_code(idt.memory); add_line();
		}else{
			// funcexpr();
		}
		
		if (lookAhead(T_OPERATOR)){
			opexpr(); return;
		}
		else if (lookAhead(T_NEWLINE) || lookAhead(T_PAR_CLOSE)) return;
	}else if(lookAhead(T_PAR_OPEN)){
		match(T_PAR_OPEN); expression(); match(T_PAR_CLOSE);
		
		if (lookAhead(T_OPERATOR)){
			opexpr(); return;
		}
		else if (lookAhead(T_NEWLINE) || lookAhead(T_PAR_CLOSE)) return;
	}
	
	throw runtime_error("Invalid expression");
}

void Parser::assignstat(){
	match(T_ASSIGNMENT);
	
	expression();
}

void Parser::incrstat(ident_t idt){
	string inc = match(T_INCDEC).value;
	
	add_code("load"); add_code(idt.memory); add_line();
	add_code("push"); add_code(1); add_line();
	
	if (inc == "++"){
		add_code("add"); add_line();
	}else{
		add_code("sub"); add_line();
	}
}

void Parser::idenstat(){
	// var incr '\n'
	// ident  expression '\n'

	Token& ident = match(T_IDENTIFIER);
	
	if (lookAhead(T_ASSIGNMENT) || lookAhead(T_INCDEC)){
		ident_t idt;
		if(identifiers.find(ident.value) == identifiers.end()){
			idt.line = ident.line;
			idt.declared = true;
			idt.variable = true;
			idt.memory = 16 + (vars++); // asumsi global var dulu
			
			identifiers.insert(make_pair(ident.value, idt));
		}else{
			idt = identifiers[ident.value];
			if (!idt.variable) throw ("Cannot do assignment to function " + ident.value);
		}
		
		if(lookAhead(T_INCDEC)){
			incrstat(idt);
		}else{
			assignstat();
		}
		
		match(T_NEWLINE);
		
		add_code("store"); add_code(idt.memory); add_line();
	}else{
		throw runtime_error("Expecting = or ++ or --");
	}
}

void Parser::whilestat(){
	match("while");
	string start = ".L" + to_str(label++);
	string end = ".L" + to_str(label++);
	add_code(start); add_line();
	expression();
	add_code("jz"); add_code(end); add_line();
	
	match(T_NEWLINE);
	
	statements();
	
	match("end"); match(T_NEWLINE);
	add_code("jmp"); add_code(start); add_line();
	add_code(end); add_line();
}

void Parser::printstat(){
	// 'print' '\n'
	// 'print' expression '\n'

	match("print");
	
	if (!lookAhead(T_NEWLINE)){
		expression();
		add_code("print"); add_line();
	}
	
	match(T_NEWLINE);
	
	add_code("push"); add_code(10); add_line();
	add_code("printc"); add_line();	
}

void Parser::statements(){
	if (eof()) return;

	switch(tokens.at(next).type){
		case T_IDENTIFIER:
			idenstat();
		break;
		case T_RESERVED:
		{
			if (lookAhead("end")){
				return;
			}else if(lookAhead("while")){
				whilestat();
			}else if(lookAhead("print")){
				printstat();
			}else{
				return;
			}
		}
		break;
		default:
			throw runtime_error("Invalid token " + tokens.at(next).value);
		break;
	}
	
	statements();
}

void Parser::parse(){
	statements();
	add_code("halt"); add_line();
}

void Parser::printCode(){
	int i = 0;
	
	while (i < main.size()){
		if(main[i]!="\n"){
			cout<<main[i++];
		
			if((i == main.size()) || (main[i]=="\n")){
				cout<<endl;
			}else{
				cout<<" ";
			}
		}else{
			i++;
		}
	}
}

Token& Parser::getLastProcessed(){
	if(!eof())
		return tokens.at(next);
	else
		return tokens.at(next-1);
}

