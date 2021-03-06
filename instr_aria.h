/* INSTRUCTION SET DEFINITION FOR ARIA VM */
#ifndef INSTR_ARIA_H
#define INSTR_ARIA_H

/* 	SPESIFIKASI
		PUSH x	: push x ke array
		POP		: buang top
		LOAD	: push isi alamat dari pop() ke stack
		STORE	: pop 2 elemen, lalu simpan pop2 ke pop1
		JMP x	: jump ke instruksi ke-x
		JZ x	: jump ke instruksi ke-x jika top == 0
		JNZ x	: jump ke instruksi ke-x jika top != 0
		ADD		: pop 2 elemen, tambahkan, lalu push hasilnya
		SUB		: pop 2 elemen, kurangkan: pop2 - pop1, lalu push hasilnya
		MUL		: pop 2 elemen, kalikan, lalu push hasilnya
		DIV		: pop 2 elemen, bagi: pop2 div pop1, lalu push hasilnya
		MOD		: pop 2 elemen, mod: pop2 mod pop1, lalu push hasilnya
		AND		: pop 2 elemen, operasi &, lalu push hasilnya
		OR		: pop 2 elemen, operasi |, lalu push hasilnya
		XOR		: pop 2 elemen, operasi ^, lalu push hasilnya
		NOT		: pop 1 elemen, operasi !, lalu push hasilnya
		EQ		: pop 2 elemen, operasi ==, lalu push hasilnya
		NEQ		: pop 2 elemen, operasi !=, lalu push hasilnya
		GT		: pop 2 elemen, operasi pop2 > pop1, lalu push hasilnya
		LT		: pop 2 elemen, operasi pop2 < pop1, lalu push hasilnya
		GTE		: pop 2 elemen, operasi pop2 >= pop1, lalu push hasilnya
		LTE		: pop 2 elemen, operasi pop2 <= pop1, lalu push hasilnya
		PRINT	: pop 1 elemen, print ke layar (integer)
		HALT	: hentikan program
		PRINTC	: pop 1 elemen, print ke layar (karakter)
		CALL x	: simpan ip & 8 register awal, jalankan fungsi dengan label x
		RETURN	: kembalikan ip & 8 register awal seperti sebelum fungsi dipanggil
		INPUT	: baca nilai integer dari layar, push ke stack
		INPUTC	: baca nilai char dari layar, push ke stack
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
#define MOD 0x0C

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
#define CALL 0x1A
#define RETURN 0x1B
#define INPUT 0x1C
#define INPUTC 0x1D

#endif