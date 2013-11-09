#include <fstream>
#include <stdexcept>
#include "parser.hpp"

int main(){
	fstream in("test.txt");	
	Parser parser(in);
	
	try{
		parser.parse();
		parser.printCode();
	}catch(runtime_error& e){
		cout<<e.what()<<endl;
		Token& tok = parser.getLastProcessed();
		cout<<tok.value<<" "<<tok.line<<endl;
	}
	
	in.close();
	
	return 0;
}