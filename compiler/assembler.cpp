#include <cstdlib>
#include <algorithm>
#include <cstdio>
#include "assembler.hpp"

Assembler::Assembler(const vector<string>& c, ostream& o): code(c), out(o){
	initmap();
}

void Assembler::itob(char* byte, int n){
	for (int i = 0; i < 4; i++)
		byte[i] = (n >> (i * 8)) & 0xFF;
}

string Assembler::strc(char c){
	string s = " ";
	s[0] = c;
	return s;
}

void Assembler::assemble(){
	build_symbol();
	
	writefile();
}

void Assembler::add_int(int n){
	char buf[4]; itob(buf, n);
	for (int i = 0; i < 4; i++)
		symbol.push_back(strc(buf[i]));
}

static char upcase(char c){
	if((c >= 'a') && ( c <= 'z')){
		return c - 'a' + 'A';
	}else{
		return c;
	}
}

string Assembler::uppercase(string s){
	transform(s.begin(), s.end(), s.begin(), upcase);
	return s;
}

void Assembler::build_symbol(){
	int i = 0;
	
	while (i < code.size()){
		string first = code[i++];
		
		if (first[0] == '.'){
			label.insert(make_pair(first, symbol.size()));
		}else{
			symbol.push_back(strc(instruction_map[uppercase(first)]));
			
		}
		
		if (i == code.size()) break;
		string second = code[i++];
		
		if (second[0] == '.'){
			symbol.push_back(second);
			
			for (int j = 0; j < 3; j++)
				symbol.push_back(strc('\0'));
				
			i++;
		}else if(second != "\n"){
			add_int(atoi(second.c_str()));
			
			i++;
		}
		
	}
}

void Assembler::writefile(){
	int i = 0;
	
	while (i < symbol.size()){
		string sym = symbol[i++];
		if ((sym[0] == '.') && (label.find(sym) != label.end())){
			char buf[4]; itob(buf, label[sym]);
			
			for (int j = 0; j < 4; j++)
				out<<buf[j];
			
			i+=3;
		}else{	
			out<<sym[0];
		}
	}
}
