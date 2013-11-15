all: ariac.o tokenizer.o parser.o assembler.o
	g++ -g -O0 -o ariac ariac.o tokenizer.o parser.o assembler.o
	
ariac.o: ariac.cpp
	g++ -g -O0 -c ariac.cpp
	
tokenizer.o: tokenizer.cpp
	g++ -g -O0 -c tokenizer.cpp
	
parser.o: parser.cpp
	g++ -g -O0 -c parser.cpp
	
assembler.o: assembler.cpp
	g++ -g -O0 -c assembler.cpp
	
clean:
	rm -rf *.exe *.o