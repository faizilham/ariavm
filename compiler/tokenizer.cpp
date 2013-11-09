#include "tokenizer.hpp"
#include <stdexcept>

Tokenizer::Tokenizer(istream& in) : input(in){}

Tokenizer::~Tokenizer(){
	tokens.clear();
}

void Tokenizer::init(){
	line = 1; comment = false;
	if (input.good()) next = input.get();
}

char Tokenizer::consume(){
	char current = next;
	if (input.good()) next = input.get();
	return current;
}

int Tokenizer::getline() const{
	return line;
}

void Tokenizer::tokenize(){
	init();
	while(input.good()){
		if(!skip_whitespace()) break;
		
		if (comment)
			skip_comment();
		else
			next_token();
	}
}

bool Tokenizer::skip_whitespace(){
	while(input.good() && ((next == ' ') || (next == '\t') || (next == '\r' ))){
		consume();
	}
	
	return input.good();
}

void Tokenizer::skip_comment(){
	do{
		consume();
	}while (input.good() && (getType(next)!=NEWLINE));
	
	comment = false;
}

void Tokenizer::next_token(){
	int n = 0;
	char current;
	do{
		current = consume();
		buffer[n++] = current;
	}while(input.good() && (getType(current) == getType(next)) && (getType(current) < PARENTHESES));
	
	buffer[n] = '\0';
	
	if(getType(current) == NEWLINE){
		line++;
	}else if(getType(current) == COMMENT){
		comment = true;
		return;
	}
	
	recognize(string(buffer));
	
}

void Tokenizer::recognize(string value){
	Token token; token.value = value; token.line = line;
	switch(getType(value[0])){
		case INVALID:
			throw runtime_error("Invalid character " + value);
		break;
		case NEWLINE:
			token.type = T_NEWLINE;
		break;
		case COMMA:
			token.type = T_COMMA;
		break;
		case PARENTHESES:
			token.type = value[0] == '(' ? T_PAR_OPEN : T_PAR_CLOSE;
		break;
		case OPERATOR:
		{
			if (exist(value, math_op, math_op_size) || exist(value, logic_op, logic_op_size) || exist(value, test_op, test_op_size)){
				token.type = T_OPERATOR;
			}else if(exist(value, assign_op, assign_op_size)){
				token.type = T_ASSIGNMENT;
			}else if(exist(value, incdec_op, incdec_op_size)){
				token.type = T_INCDEC;
			}else{
				throw runtime_error("Invalid operator " + value);
			}
		}
		break;
		case ALPHANUMERIC:
		{
			char c = value[0];
			
			if (c >= '0' && c <= '9'){
				for (int i = 1; i < value.length(); i++){
					c = value[i];
					if (c < '0' || c > '9') throw runtime_error("Invalid integer " + value);
				}
				
				token.type = T_INTEGER;
			}else{
				token.type = exist(value, reserved_words, reserved_words_size) ? T_RESERVED : T_IDENTIFIER;
			}
		}
		break;
	}
	
	tokens.push_back(token);
}

void Tokenizer::print(){
	int n = tokens.size();
	for (int i = 0; i < n; i++)
		cout<<":"<<tokens[i].value<<endl;
	
}

int Tokenizer::getType(char c){
	switch(c){
		case ' ':
		case '\t':
		case '\r':
			return WHITESPACE;
		break;
		case '\n':
			return NEWLINE;
		case '=':
		case '+':
		case '-':
		case '*':
		case '/':
		case '<':
		case '>':
			return OPERATOR;
		break;
		case '#':
			return COMMENT;
		break;
		case '(':
		case ')':
			return PARENTHESES;
		break;
		case ',':
			return COMMA;
		break;
		default:
			if ((c >= 'a' && c <='z') || (c >='A' && c <='Z') || (c >= '0' && c <= '9'))
				return ALPHANUMERIC;
			else
				return INVALID;
	}
}

