#include <fstream>
#include <string>
#include "Symbol.h"
#include "Lexer.h"
#include "error.h"
#include "parser.h"
#include "symboltable.h"
#include "quater.h"
#include "MipsInstr.h"
#include "Optimization.h"

using namespace std;

//#define MAXReserved 30
//#define MAXR        20
int lev;       //程序层次
int lc;
int tempcount=0;
Symbol sym;    //当前的单词类型
fstream infile;
ofstream errmsgfile;
ofstream symboltablefile;
ofstream midcodefile;
ofstream mipscodefile;
ofstream  outputopfile;

Lexer lexer;  //词法分析
Parser parser;  //语法分析
Error error;
QCode midCode;
OPcode opQcode;
MipsInstr genmips;

tabItem current_item;
tabItem temp_item;
table toptable;
tableManager tab_set;
int errornum=0;