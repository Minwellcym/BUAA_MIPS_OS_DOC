#ifndef __PARSER_H__
#define __PARSER_H__

#include <set>
#include "Lexer.h"
#include "error.h"
#include <queue>
#include "symboltable.h"
#include "quater.h"


using namespace std;


typedef  set<Symbol>  symset;
extern QCode midCode;
extern int tempcount;


class Parser
{
private:
public:
	symset procbegsys;

    symset constDecbegsys;
	symset constDecendsys;
	symset constDefbegsys;

    symset varDecbegsys;

    symset statbegsys,statendsys;
    symset facbegsys;
	symset termbegsys;
	symset expressionbegsys;
	symset opsys;
	bool funcflag,mainfuncflag,compoundStatementflag;
	bool constDecflag,varDefflag;
	//bool skipflag;
	void initial();
	void skip(symset s);
	symset addsymset(symset s1,symset s2);
	void test(symset s1,symset s2,int n);
	void testsemicolon(int efnum,int emnum);
	void test_err(Symbol tsym);
	//void testaddsymset();
	Parser();
	void parser();
	void prog(symset progbegin);                //程序
	void constDec();            //处理常量说明
	void var_funcDec();
	void voidDec();
	void constDef();            //处理常量定义
	//void varDec();              //处理变量说明
	void varDef(string word,Symbol tsym);              //处理变量定义
	void rtvaluefuncDec(string word,Symbol tsym);      //有返回值函数定义
	void nrvfuncDec(string word);          //无返回值函数定义
	void funcdecHead();         //函数声明头部
	void Parameter();           //参数
	void parameterList();       //参数表
	void Mainfunc();            // 主函数
	void compoundStatement();    //复合语句

	void newvarDec();           //变量说明
	void newvarDef();

	void Statement();            //语句
	void Statementcolumn();       //语句列
	void conditionStatement();    //条件语句
	void Condition(Operand& des,bool flag);            //条件
	void doloopStatement();        //do while循环语句
	void forloopStatement();        //for 循环语句
	void forpart1();
	void forpart2();
	void forpart3();
	void Step();                //步长
	void call(tabItem func);
	void callrtvalueFunc(tabItem func,Operand& des);      //有返回值函数调用
	void callnrtvalueFunc(tabItem func);      //无返回值函数调用
	void valueparameterTable(tabItem func,Operand& des);   //值参列表
	void assignStatement(tabItem var_ident,Operand& opd);       //赋值语句
	void scanfStatement();        //读语句
	void printfStatement();       //写语句
	void returnStatement();       //返回语句
	void factor(Operand& des);                //因子处理
	void factorpart1(Operand& des);
	void factorpart2(Operand& des);
	void factorpart3(Operand& des);
	void term(Operand& des);                   //项
	void expression(Operand& src);            //表达式

	void completeaddr(int currenttab);
	void completetempaddr(int currenttab);
	void gensize();
};
#endif