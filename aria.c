#include <stdio.h>
#include <stdlib.h>

#define MAX_STACK 4096
#define MAX_FUNCSTACK 256
#define INSTR_LEN 65536
#define LOCAL_VAR 16

typedef unsigned char byte;
typedef unsigned int ptr_t;

typedef struct{
	ptr_t old_ip;
	int old_reg[LOCAL_VAR]; // hanya sejumlah LOCAL_VAR register pertama yg dijamin aman
} func_t;

int stack[MAX_STACK]; 	// stack
ptr_t sp = -1;  	// stack pointer

func_t fstack[MAX_FUNCSTACK];
ptr_t fsp = -1;

byte instr[INSTR_LEN]; 	// instruction
ptr_t ip = 0;		// instruction pointer

int reg[MAX_STACK];	// virtual register / memori
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

void die(char* info){
	printf("%s\n", info);
	exit(1);
}

#include "instr_handler.h"

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
	printf("Usage: aria arx_file\n");
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