#include "parser.hpp"
#include <stdexcept>

Parser::Parser(istream& input) : tokens(input){
	next = 0;
	label = 0; globalvars = 0;
	infunc = false;
	settarget(main);
	settable(globals);
	module_name = ".blah";
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

void Parser::settable(map<string, ident_t>& table){
	symbol_table = &table;
}

string Parser::newlabel(){
	return module_name + ".L" + to_str(label++);
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
	}else if (lookAhead(T_IDENTIFIER)){
		Token& ident = match(T_IDENTIFIER);
		if (!identExist(ident))
			throw runtime_error("Identifier " + ident.value + " is undeclared / unassigned");
		
		ident_t idt = getVar(ident);
		
		if(idt.variable){
			add_code("load"); add_code(idt.memory); add_line();
		}else{
			// funcexpr();
		}
		
	}else if(lookAhead(T_PAR_OPEN)){
		match(T_PAR_OPEN); expression(); match(T_PAR_CLOSE);
	}
	
	if (lookAhead(T_OPERATOR)){
		opexpr(); return;
	}
	else if (lookAhead(T_COMMA) || lookAhead(T_NEWLINE) || lookAhead(T_PAR_CLOSE)) return;
	
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

ident_t Parser::getVar(Token& ident){
	ident_t idt;
	
	int v = identExist(ident);
	
	if(v == 0){ // blom ada
		idt.line = ident.line;
		idt.variable = true;
		
		if (infunc)
			idt.memory = locals.size();
		else
			idt.memory = 16 + (globalvars++);
		
		symbol_table->insert(make_pair(ident.value, idt));
	}else{
		idt = v == 1 ? globals[ident.value] : locals[ident.value];
		if (!idt.variable) throw runtime_error(ident.value + " is not a variable");
	}
	
	return idt;
}

ident_t Parser::getFunc(Token& ident){
	int v = identExist(ident);
	if(!v) throw runtime_error("Function " + ident.value + " is undeclared");
	
	if (v==2) throw runtime_error("Identifier " + ident.value + " is not a function");
	
	ident_t idt = globals[ident.value];
	if (idt.variable) throw runtime_error("Identifier " + ident.value + " is not a function");
	
	return idt;	
}

int Parser::identExist(Token& ident){
	if(infunc && (locals.find(ident.value) != locals.end()))
		return 2;
	else if (globals.find(ident.value) != globals.end())
		return 1;
	else
		return 0;
}

void Parser::idenstat(){
	// var incr '\n'
	// ident = expression '\n'

	Token& ident = match(T_IDENTIFIER);
	
	if (lookAhead(T_ASSIGNMENT) || lookAhead(T_INCDEC)){
		ident_t idt = getVar(ident);
		
		if(lookAhead(T_INCDEC)){
			incrstat(idt);
		}else{
			assignstat();
		}
		
		match(T_NEWLINE);
		
		add_code("store"); add_code(idt.memory); add_line();
	}else if(lookAhead(T_PAR_OPEN)){
		ident_t idt = getFunc(ident);
		
		int n = 0;
		match(T_PAR_OPEN);
		
		while (!lookAhead(T_PAR_CLOSE)){
			n++;
			expression();
			if (!lookAhead(T_PAR_CLOSE))
				match(T_COMMA);
		}
		
		if (n != idt.memory) throw runtime_error("Wrong number of parameter");
		
		match(T_PAR_CLOSE); match(T_NEWLINE);
		add_code("call"); add_code(idt.label); add_line();
		add_code("pop"); add_line();
		
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
	ident_t idt = getVar(ident);
	
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

void Parser::definestat(){
	//read define

	match("define");
	if (infunc) throw runtime_error("Cannot define function inside function");
	
	infunc = true;
	locals.clear();
	settable(locals);
	settarget(funcs);
	
	// read function name
	Token& ident = match(T_IDENTIFIER);
	
	if (identExist(ident)) throw runtime_error("Identifier " + ident.value + " already declared");
	
	ident_t func;
	
	// make function label and put to global symbol table
	
	func.line = ident.line;
	func.variable = false;
	func.label = module_name + ".func." + ident.value;
	func.memory = 0; // serves as number of params
	
		
	//read (
	match(T_PAR_OPEN);
	
	// read parameters
	while(!lookAhead(T_PAR_CLOSE)){
		Token& param = match(T_IDENTIFIER);
		func.memory++;
		
		ident_t par;	
		par.line = param.line;
		par.variable = true;
		par.memory = locals.size();
		
		locals.insert(make_pair(param.value, par));
		
		if (!lookAhead(T_PAR_CLOSE)) match(T_COMMA);
	}
	
	// read ) and newline
	match(T_PAR_CLOSE); match(T_NEWLINE);
	
	// insert function to symbol table
	globals.insert(make_pair(ident.value, func));
	
	// make initial function code
	add_code(func.label); add_line();
	
	for (int i = func.memory - 1; i >= 0; i--){
		add_code("store"); add_code(i); add_line();
	}
	
	statements();
	
	match("end"); match(T_NEWLINE);
	
	if (funcs[funcs.size() - 2] != "return"){
		add_code("push"); add_code(0); add_line();
		add_code("return"); add_line();
	}
	
	// cleanup
	
	infunc = false;
	locals.clear();
	settable(globals);
	settarget(main);
}

void Parser::returnstat(){
	match("return");
	
	if(!infunc) throw runtime_error("Return statement outside of function definition");
	
	if (!lookAhead(T_NEWLINE)){
		expression();
	}
	
	match(T_NEWLINE);
	add_code("return"); add_line();
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
				definestat();
			}else if(lookAhead("return")){
				returnstat();
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
	
	for (int i = 0; i < funcs.size(); i++){
		main.push_back(funcs[i]);
	}
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

