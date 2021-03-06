#ifndef INSTR_HANDLER_H
#define INSTR_HANDLER_H

#include "instr_aria.h"
#include <string.h>

void h_push(){ push(load_int()); }
void h_pop(){ pop(); }

void h_load(){ push(reg[pop()]);}
void h_store(){	int pop1 = pop(); reg[pop1] = pop();}

void h_jmp(){ ip = load_int();}
void h_jz(){ int target = load_int(); if (!pop()) ip = target;}
void h_jnz(){ int target = load_int(); if (pop()) ip = target;}

void h_add(){ push(pop() + pop());}
void h_sub(){ int val1 = pop(); push(pop() - val1);}
void h_mul(){ push(pop() * pop());}
void h_div(){ int val1 = pop(); push(pop() / val1);}
void h_mod(){ int val1 = pop(); push(pop() % val1);}

void h_and(){ push(pop() && pop());}
void h_or(){ push(pop() || pop());}
void h_not(){ push(!pop());}
void h_xor(){push(pop() ^ pop());}

void h_eq(){push(pop() == pop());}
void h_neq(){push(pop() != pop());}
void h_gt(){int val1 = pop(); push(pop() > val1);}
void h_lt(){int val1 = pop(); push(pop() < val1);}
void h_gte(){int val1 = pop(); push(pop() >= val1);}
void h_lte(){int val1 = pop(); push(pop() <= val1);}

void h_print(){printf("%d", pop());}
void h_printc(){printf("%c", pop());}
void h_halt(){running = 0;}
void h_nop(){}

void h_call(){
	int tmp = load_int();
	fstack[++fsp].old_ip = ip;
	memcpy(fstack[fsp].old_reg, reg, sizeof(int) * LOCAL_VAR);
	ip = tmp;
}

void h_return(){
	ip = fstack[fsp].old_ip;
	memcpy(reg, fstack[fsp--].old_reg, sizeof(int) * LOCAL_VAR);
}

void h_input(){
	scanf("%d", &stack[++sp]);
}

void h_inputc(){
	scanf("%c", &stack[++sp]);
}

void (*commands[30])();

void init_commands(){
		commands[NOP]=h_nop;
		commands[PUSH]=h_push;
		commands[POP]=h_pop;
		commands[LOAD]=h_load;
		commands[STORE]=h_store;
		
		commands[JMP]=h_jmp;
		commands[JZ]=h_jz;
		commands[JNZ]=h_jnz;
		
		commands[ADD]=h_add;
		commands[SUB]=h_sub;
		commands[MUL]=h_mul;
		commands[DIV]=h_div;
		commands[MOD]=h_mod;
		
		commands[AND]=h_and;
		commands[OR]=h_or;
		commands[NOT]=h_not;
		commands[XOR]=h_xor;
		
		commands[EQ]=h_eq;
		commands[NEQ]=h_neq;
		commands[GT]=h_gt;
		commands[LT]=h_lt;
		commands[GTE]=h_gte;
		commands[LTE]=h_lte;
		
		commands[PRINT]=h_print;
		commands[HALT]=h_halt;
		commands[PRINTC]=h_printc;
		commands[CALL]=h_call;
		commands[RETURN]=h_return;
		commands[INPUT]=h_input;
		commands[INPUTC]=h_inputc;
}

#endif