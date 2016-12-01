#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "error.h"
#include "Lexer.h"
#include "parser.h"
#include "Symbol.h"

using namespace std;
extern int lc;       //当前行数
int lineoferr=1;
extern ofstream errmsgfile;  //错误信息输出的文件

extern Lexer lexer;
extern Parser parser;
extern Error error;

Error::Error()
{
	errornum=0;
}
void Error::initial()
{
	errmsgfile<<setw(10)<<"line"<<setw(20)<<"error id"<<setw(30)<<"error reason"
		<<setw(10)<<"msgnum"<<setw(20)<<"error line"<<endl;

	errfunc[0]="parser";    errfunc[1]="prog";
	errfunc[2]="constDec";    errfunc[3]="constDef";
	errfunc[4]="var_funcDec";    errfunc[5]="voidDec";
	errfunc[6]="varDef";    errfunc[7]="rtvaluefuncDec";
	errfunc[8]="nrvfuncDec";    errfunc[9]="parameterList";
	errfunc[10]="Mainfunc";    errfunc[11]="compoundStatement";
	errfunc[12]="Statement";    errfunc[13]="conditionStatement";
	errfunc[14]="Condition";    errfunc[15]="doloopStatement";
	errfunc[16]="forloopStatement";    errfunc[17]="call";
	errfunc[18]="callrtvalueFunc";    errfunc[19]="callnrtvalueFunc";
	errfunc[20]="valueparameterTable";    errfunc[21]="assignStatement";
	errfunc[22]="printfStatement";    errfunc[23]="scanfStatement";
	errfunc[24]="returnStatement";    errfunc[25]="expression";
	errfunc[26]="term";    errfunc[27]="factorpart1";
	errfunc[28]="factorpart3";    errfunc[29]="newvarDef";

	msg[0]="非法字符";              msg[1]="常量定义位置出错"; 
	msg[2]="应该是int/char";        msg[3]="应该是;";
	msg[4]="应该是ident";           msg[5]="应该是=";
	msg[6]="应该是number";          msg[7]="应该是constchar";
	msg[8]="变量定义位置出错";      msg[9]="应该是]";
	msg[10]="应该是{";              msg[11]="应该是 }";
	msg[12]="应该是 (";             msg[13]="应该是 )";
	msg[14]="应该是 函数类型";      msg[15]="错误的类型";
	msg[16]="应该是 关系运算符";    msg[17]="应该是 while";
	msg[18]="应该是 变量类型";      msg[19]="应该是 +/-";
	msg[20]="参数多余";             msg[21]="应该是数组类型";
	msg[22]="未定义的变量";         msg[23]="非法类型";
	msg[24]="错误数组下表";        msg[25]="for循环出错，应该是+ 或者-";
	msg[26]="for循环出错，应该是变量";        msg[27]="for循环出错，应该是=";
	msg[28]="for循环出错，初始值不是变量";        msg[29]="因子的类型不匹配";
	msg[30]="项的类型不匹配";        msg[31]="error in const def";
	msg[32]="";   msg[33]="";
	msg[34]="";   msg[35]="";
	msg[34]="";   msg[37]="";
	msg[34]="";   msg[39]="";
	msg[34]="";   msg[41]="";
	msg[34]="";   msg[43]="";
	msg[34]="";   msg[45]="";
}
void Error::adderrormsg(int efnum,int emnum)
{
	errornum++;
	printferrmsg(efnum,emnum,lc);
	switch (efnum)
	{
	case 0:
		parser.skip(parser.procbegsys);
		break;
	case 1:
		parser.skip(parser.procbegsys);
		break;
	case 2:
		parser.skip(parser.procbegsys);
		break;
	case 3:
		parser.skip(parser.procbegsys);
		break;
	case 4:
		parser.skip(parser.procbegsys);
		break;
	case 5:
		parser.skip(parser.procbegsys);
		break;
	case 6:
		parser.skip(parser.procbegsys);
		break;
	case 7:
		parser.skip(parser.procbegsys);
		break;
	case 8:
		parser.skip(parser.procbegsys);
		break;
	case 9:
		parser.skip(parser.procbegsys);
		break;
	case 10:
		parser.skip(parser.procbegsys);
		break;
	case 11:
		parser.skip(parser.procbegsys);
		break;
	case 12:
		parser.skip(parser.statbegsys);
		break;
	case 13:
		parser.skip(parser.statbegsys);
		break;
	case 14:
		parser.skip(parser.statbegsys);
		break;
	case 15:
		parser.skip(parser.statbegsys);
		break;
	case 16:
		parser.skip(parser.statbegsys);
		break;
	case 17:
		parser.skip(parser.statbegsys);
		break;
	case 18:
		parser.skip(parser.statbegsys);
		break;
	case 19:
		parser.skip(parser.statbegsys);
		break;
	case 20:
		parser.skip(parser.statbegsys);
		break;
	case 21:
		if(emnum==9)
		{
		}
		else
		{
			parser.skip(parser.statbegsys);
		}
		break;
	case 22:
		parser.skip(parser.statbegsys);
		break;
	case 23:
		if(emnum==22)
		{
			errmsgfile<<lexer.identsym<<endl;
		}
		parser.skip(parser.statbegsys);
		break;
	case 24:
		parser.skip(parser.statbegsys);
		break;
	case 25:
		parser.skip(parser.statbegsys);
		break;
	case 26:
		parser.skip(parser.statbegsys);
		break;
	case 27:
		if(emnum==9)
		{
		}
		else
		{
			parser.skip(parser.statbegsys);
		}
		break;
	case 28:
		parser.skip(parser.statbegsys);
		break;
	case 29:
		parser.skip(parser.statbegsys);
		break;
	}
}
void Error::printferrmsg(int efnum,int emnum,int line)
{
	errmsgfile<<setw(10)<<lineoferr++<<setw(20)<<errfunc[efnum]<<setw(30)<<msg[emnum]
		<<setw(10)<<emnum<<setw(20)<<line<<endl;
}
