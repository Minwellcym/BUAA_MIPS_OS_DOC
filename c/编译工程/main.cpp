#include <iostream>
#include <fstream>
#include <cctype>
#include <string>
#include <vector>
#include "Symbol.h"
#include "Lexer.h"
#include "parser.h"
#include "error.h"
#include "symboltable.h"
#include "quater.h"
#include "MipsInstr.h"
#include "Optimization.h"
extern fstream infile;
extern ofstream errmsgfile;
extern ofstream symboltablefile;
extern ofstream midcodefile;
extern ofstream mipscodefile;
extern ofstream  outputopfile;
extern int lc;  //行计数器
extern Lexer lexer;
extern Parser parser;
extern Error error;
extern QCode midCode;
extern OPcode opQcode;
extern MipsInstr genmips;
extern table toptable;
extern tableManager tab_set;
extern int errornum;

int main()
{
    string filename;
	int i=0;
    infile.open("14231052_test.txt");
	errmsgfile.open("errmsg.txt");
	symboltablefile.open("symtab.txt");
	midcodefile.open("midcode.txt");
	mipscodefile.open("mipscode.txt");
	outputopfile.open("outputofOPTIMIZATION.txt");
	if (!infile.is_open())
    {
		cout << "source file open fail!" << endl;
	}
	lc=0;
	error.initial();
	tab_set.Tab.push_back(toptable);
	tab_set.pCurrent_tab=0;
	tab_set.pTop_tab=0;
	parser.parser();
	cout<<errornum<<endl;
	tab_set.printf_tab();
	tab_set.printftab(tab_set.pTop_tab);
	if(error.errornum==0)
	{
		midCode.printf_Qcode();
		//opQcode.dividfunc();
		genmips.translate();
		cout<<"no error"<<endl;
	}
	else
	{
		cout<<"||||||||||出错了，编译不通过||||||"<<endl;
	}

    infile.close();
	errmsgfile.close();
	symboltablefile.close();
	midcodefile.close();
	mipscodefile.close();
	outputopfile.close();

	system("pause");
    return 0;
}