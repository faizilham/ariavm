#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <iostream>
#include <string>
#include <vector>
#include <map>

using namespace std;

static map<string, char> instruction_map;
static bool initialized = false;

static void initmap(){
	if(!initialized){
		instruction_map["NOP"] = 0x00;
		instruction_map["PUSH"] = 0x01;
		instruction_map["POP"] = 0x02;
		instruction_map["LOAD"] = 0x03;
		instruction_map["STORE"] = 0x04;
		instruction_map["JMP"] = 0x05;
		instruction_map["JZ"] = 0x06;
		instruction_map["JNZ"] = 0x07;
		instruction_map["ADD"] = 0x08;
		instruction_map["SUB"] = 0x09;
		instruction_map["MUL"] = 0x0A;
		instruction_map["DIV"] = 0x0B;
		instruction_map["NEG"] = 0x0C;
		instruction_map["AND"] = 0x0D;
		instruction_map["OR"] = 0x0E;
		instruction_map["NOT"] = 0x0F;
		instruction_map["XOR"] = 0x10;
		instruction_map["EQ"] = 0x11;
		instruction_map["NEQ"] = 0x12;
		instruction_map["GT"] = 0x13;
		instruction_map["LT"] = 0x14;
		instruction_map["GTE"] = 0x15;
		instruction_map["LTE"] = 0x16;
		instruction_map["PRINT"] = 0x17;
		instruction_map["HALT"] = 0x18;
		instruction_map["PRINTC"] = 0x19;
		instruction_map["CALL"] = 0x1A;
		instruction_map["RETURN"] = 0x1B;
		instruction_map["INPUT"] = 0x1C;
		initialized = true;
	}
}

class Assembler{
	private:
		ostream& out;
		vector<string> code, symbol;
		map<string, int> label;
		
		static string uppercase(string);
		
		void build_symbol();
		void writefile();
		void add_int(int);
		static void itob(char*, int);
		static string strc(char);
	public:
		Assembler(const vector<string>&, ostream&);
		void assemble();
};

#endif