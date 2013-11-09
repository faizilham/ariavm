all: test.o tokenizer.o parser.o
	g++ -O2 -o test test.o tokenizer.o parser.o
	
test.o: test.cpp
	g++ -O2 -c test.cpp
	
tokenizer.o: tokenizer.cpp
	g++ -O2 -c tokenizer.cpp
	
parser.o: parser.cpp
	g++ -O2 -c parser.cpp
	
clean:
	rm -rf test.exe *.o