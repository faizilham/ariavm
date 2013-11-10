#include <fstream>
#include <stdexcept>
#include "parser.hpp"
#include "assembler.hpp"

void help(){

}

int main(int argc, char** argv){
	string fname; bool assembly = false;
	
	if (argc == 1){
		help();
		return 1;
	}else if (argc == 2){
		fname = string(argv[1]);
	}else if (argc == 3){
		string s1 = string(argv[1]), s2 = string(argv[2]);
		assembly = true;
		if (s1 == "-s"){
			fname = s2;
		}else if(s2 == "-s"){
			fname = s1;
		}else{
			help(); return 1;
		}	
	}
	
	if (fname.rfind(".aria") == -1){
		help(); return 1;
	}
	
	string namepart = fname.substr(0, fname.length() - 5);

	ifstream in(fname.c_str());	
	Parser parser(in);
	
	try{
		parser.parse();
		if (assembly){
			string fout = namepart + ".avc";
			ofstream out(fout.c_str());
			parser.printCode(out);
			out.close();
		}else{
			string fout = namepart + ".arx";
			ofstream out(fout.c_str(), ofstream::binary);
			try{
				Assembler asmb(parser.getCode(), out);
				asmb.assemble();
			}catch(...){
				cout<<"Assembler error"<<endl;
			}
			out.close();
		}
	}catch(runtime_error& e){
		cout<<e.what()<<endl;
		Token& tok = parser.getLastProcessed();
		cout<<tok.value<<" "<<tok.line<<endl;
	}
	
	in.close();
	
	return 0;
}