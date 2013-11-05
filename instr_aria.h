/* INSTRUCTION SET DEFINITION FOR ARIA VM */
#ifndef INSTR_ARIA_H
#define INSTR_ARIA_H

/* 	SPESIFIKASI
		PUSH x	: push x ke array
		POP		: buang top
		LOAD x	: push isi alamat x ke array
		STORE x	: pop, lalu simpan ke x
		JMP x	: jump ke instruksi ke-x
		JZ x	: jump ke instruksi ke-x jika top == 0
		JNZ x	: jump ke instruksi ke-x jika top != 0
		ADD		: pop 2 elemen, tambahkan, lalu push hasilnya
		SUB		: pop 2 elemen, kurangkan: *(top-1) - *top, lalu push hasilnya
		MUL		: pop 2 elemen, kalikan, lalu push hasilnya
		DIV		: pop 2 elemen, bagi integer: *(top-1) div *top, lalu push hasilnya
		NEG		: pop 1 elemen, negasikan, lalu push hasilnya
		AND		: pop 2 elemen, operasi &, lalu push hasilnya
		OR		: pop 2 elemen, operasi |, lalu push hasilnya
		XOR		: pop 2 elemen, operasi ^, lalu push hasilnya
		NOT		: pop 1 elemen, operasi !, lalu push hasilnya
		EQ		: pop 2 elemen, operasi ==, lalu push hasilnya
		NEQ		: pop 2 elemen, operasi !=, lalu push hasilnya
		GT		: pop 2 elemen, operasi >, lalu push hasilnya
		LT		: pop 2 elemen, operasi <, lalu push hasilnya
		GTE		: pop 2 elemen, operasi >=, lalu push hasilnya
		LTE		: pop 2 elemen, operasi <=, lalu push hasilnya
		PRINT	: pop 1 elemen, print ke layar (integer)
		HALT	: hentikan program
		PRINTC	: pop 1 elemen, print ke layar (karakter)
*/

// memory access
#define NOP 0x00
#define PUSH 0x01
#define POP 0x02
#define LOAD 0x03
#define STORE 0x04

// flow control
#define JMP 0x05
#define JZ 0x06
#define JNZ 0x07

// arithmetic
#define ADD 0x08
#define SUB 0x09
#define MUL 0x0A
#define DIV 0x0B
#define NEG 0x0C

// logic
#define AND 0x0D
#define OR 0x0E
#define NOT 0x0F
#define XOR 0x10

// tests
#define EQ 0x11
#define NEQ 0x12
#define GT 0x13
#define LT 0x14
#define GTE 0x15
#define LTE 0x16

// other
#define PRINT 0x17
#define HALT 0x18
#define PRINTC 0x19

#endif