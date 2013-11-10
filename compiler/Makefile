all: ariac.o tokenizer.o parser.o assembler.o
	g++ -O2 -o ariac ariac.o tokenizer.o parser.o assembler.o
	
ariac.o: ariac.cpp
	g++ -O2 -c ariac.cpp
	
tokenizer.o: tokenizer.cpp
	g++ -O2 -c tokenizer.cpp
	
parser.o: parser.cpp
	g++ -O2 -c parser.cpp
	
assembler.o: assembler.cpp
	g++ -O2 -c assembler.cpp
	
clean:
	rm -rf *.exe *.o