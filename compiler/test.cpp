#include <fstream>
#include <stdexcept>
#include "tokenizer.hpp"

int main(){
	fstream in("test.txt");
	Tokenizer tok(in);
	try{
		tok.tokenize();
	}catch(runtime_error& e){
		cout<<"Error on line "<<tok.getline()<<": "<<e.what()<<endl;
		return 1;
	}
	tok.print();
	in.close();
	
	return 0;
}