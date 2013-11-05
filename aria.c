#include <stdio.h>
#include <stdlib.h>

typedef unsigned char byte;
typedef unsigned int ptr_t;

int stack[1024]; 	// stack
ptr_t sp = -1;  	// stack pointer

byte instr[65536]; 	// instruction
ptr_t ip = 0;		// instruction pointer

int memory[256];	// virtual memory
int running = 1;	// running flag

void push(int value){
	stack[++sp] = value;
}

int pop(){
	return stack[sp--];
}

int load_int(){
	// representasi little endian
	int sum = (instr[ip] & 0xFF) | ((instr[ip+1] & 0xFF) << 8) | ((instr[ip+2] & 0xFF) << 16) | ((instr[ip+3] & 0xFF) << 24);
	ip += 4;
	return sum;
}

#include "instr_handler.h"

void die(char* info){
	printf("%s\n", info);
	exit(1);
}

void load_program(char* filename){	
	FILE* file = fopen(filename, "rb");
	
	if (!file){
		printf("Couldn't load file %s\n", filename);
		exit(1);
	}
	
	fread(instr, sizeof(byte), 65536, file);
	fclose(file);
}

void print_help(){
	exit(1);
}

int main(int argc, char** argv){
	if (argc != 2)
		print_help();
	else
		load_program(argv[1]);
		
	init_commands();
	
	while(running){
		commands[instr[ip++]](); //execute		
	}
	
	return 0;
}