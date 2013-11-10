#include "parser.hpp"
#include <stdexcept>

Parser::Parser(istream& input) : tokens(input){
	next = 0;
	label = 0; globalvars = 0;
	infunc = false;
	settarget(main);
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
	else throw runtime_error("Expected " + value);
}
Token& Parser::match(int type){
	if(!eof() && lookAhead(type)) return consume();
	else throw runtime_error("Expected type " + to_str(type));
}

void Parser::add_code(string code){
	target_code->push_back(code);
}

void Parser::add_code(int code){
	add_code(to_str(code));
}

void Parser::add_line(){
	add_code("\n");
}

void Parser::settarget(vector<string>& target){
	target_code = &target;
}

string Parser::newlabel(){
	return ".L" + to_str(label++);
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
	
	// logic operator, % not yet
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
		if (globals.find(ident) == globals.end())
			throw runtime_error("Identifier " + ident + " is undeclared / unassigned");
		
		ident_t idt = globals[ident];
		
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

ident_t Parser::getIdent(Token& ident){
	ident_t idt;
	if(globals.find(ident.value) == globals.end()){
		idt.line = ident.line;
		idt.declared = true;
		idt.variable = true;
		idt.memory = 16 + (globalvars++); // asumsi global var dulu
		
		globals.insert(make_pair(ident.value, idt));
	}else{
		idt = globals[ident.value];
		if (!idt.variable) throw ("Cannot do assignment to function " + ident.value);
	}
	
	return idt;
}

void Parser::idenstat(){
	// var incr '\n'
	// ident  expression '\n'

	Token& ident = match(T_IDENTIFIER);
	
	if (lookAhead(T_ASSIGNMENT) || lookAhead(T_INCDEC)){
		ident_t idt = getIdent(ident);
		
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
	string start = newlabel();
	string end = newlabel();
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

void Parser::readstat(){
	match("read");
	
	Token& ident = match(T_IDENTIFIER);
	ident_t idt = getIdent(ident);
	
	match(T_NEWLINE);
	
	add_code("input"); add_line();
	add_code("store"); add_code(idt.memory); add_line();
}


void Parser::__readcond(string __match, string actlabel, vector<string>& jumptable){
	settarget(jumptable);
	match(__match); expression(); match(T_NEWLINE);
	add_code("jnz"); add_code(actlabel); add_line();
}

void Parser::__readact(string actlabel, string endlabel, vector<string>& st_part){
	settarget(st_part); 
	add_code(actlabel); add_line();
	statements();
	add_code("jmp"); add_code(endlabel); add_line();
}

void Parser::ifstat(){
	vector<string> jumptable, st_part, *actual;
	actual = target_code;
	string actlabel, endlabel = newlabel();
	
	/* read if */
	actlabel = newlabel();
	__readcond("if", actlabel, jumptable);
	__readact(actlabel, endlabel, st_part);
	
	
	while(lookAhead("elseif")){
		/* read elseif */
		actlabel = newlabel();
		__readcond("elseif", actlabel, jumptable);
		__readact(actlabel, endlabel, st_part);
	}
	
	if (lookAhead("else")){
		/* read condition */
		actlabel = newlabel();
		match("else"); match(T_NEWLINE);
		settarget(jumptable);
		add_code("jmp"); add_code(actlabel); add_line();
		
		__readact(actlabel, endlabel, st_part);
	}else{
		settarget(jumptable);
		add_code("jmp"); add_code(endlabel); add_line();
	}
	
	match("end"); match(T_NEWLINE);

	/* add jumptable, statement part and endlabel*/
	target_code = actual;
	
	for(int i = 0; i < jumptable.size(); i++)
		add_code(jumptable[i]);
		
	for(int i = 0; i < st_part.size(); i++)
		add_code(st_part[i]);
	
	add_code(endlabel); add_line();
	
}

void Parser::repeatstat(){
	match("repeat"); match(T_NEWLINE);
	string rep = newlabel();
	add_code(rep); add_line();
	
	statements();
	
	match("until");
	expression();
	match(T_NEWLINE);
	add_code("jz"); add_code(rep); add_line();
}

void Parser::statements(){
	if (eof()) return;

	switch(tokens.at(next).type){
		case T_IDENTIFIER:
			idenstat();
		break;
		case T_RESERVED:
		{
			if (lookAhead("end") || lookAhead("else") || lookAhead("elseif") || lookAhead("until")){
				return;
			}else if(lookAhead("while")){
				whilestat();
			}else if(lookAhead("print")){
				printstat();
			}else if(lookAhead("read")){
				readstat();
			}else if(lookAhead("if")){
				ifstat();
			}else if(lookAhead("repeat")){
				repeatstat();
			}else if(lookAhead("define")){
				// define
			}else if(lookAhead("return")){
				// return
			}else{
				return;
			}
		}
		break;
		case T_NEWLINE:
			match(T_NEWLINE);
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

vector<string>& Parser::getCode(){
	return main;
}

void Parser::printCode(ostream& out){
	int i = 0;
	
	while (i < main.size()){
		if(main[i]!="\n"){
			out<<main[i++];
		
			if((i == main.size()) || (main[i]=="\n")){
				out<<endl;
			}else{
				out<<" ";
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

