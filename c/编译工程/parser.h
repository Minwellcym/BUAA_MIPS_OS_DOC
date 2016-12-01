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
	void prog(symset progbegin);                //����
	void constDec();            //������˵��
	void var_funcDec();
	void voidDec();
	void constDef();            //����������
	//void varDec();              //�������˵��
	void varDef(string word,Symbol tsym);              //�����������
	void rtvaluefuncDec(string word,Symbol tsym);      //�з���ֵ��������
	void nrvfuncDec(string word);          //�޷���ֵ��������
	void funcdecHead();         //��������ͷ��
	void Parameter();           //����
	void parameterList();       //������
	void Mainfunc();            // ������
	void compoundStatement();    //�������

	void newvarDec();           //����˵��
	void newvarDef();

	void Statement();            //���
	void Statementcolumn();       //�����
	void conditionStatement();    //�������
	void Condition(Operand& des,bool flag);            //����
	void doloopStatement();        //do whileѭ�����
	void forloopStatement();        //for ѭ�����
	void forpart1();
	void forpart2();
	void forpart3();
	void Step();                //����
	void call(tabItem func);
	void callrtvalueFunc(tabItem func,Operand& des);      //�з���ֵ��������
	void callnrtvalueFunc(tabItem func);      //�޷���ֵ��������
	void valueparameterTable(tabItem func,Operand& des);   //ֵ���б�
	void assignStatement(tabItem var_ident,Operand& opd);       //��ֵ���
	void scanfStatement();        //�����
	void printfStatement();       //д���
	void returnStatement();       //�������
	void factor(Operand& des);                //���Ӵ���
	void factorpart1(Operand& des);
	void factorpart2(Operand& des);
	void factorpart3(Operand& des);
	void term(Operand& des);                   //��
	void expression(Operand& src);            //���ʽ

	void completeaddr(int currenttab);
	void completetempaddr(int currenttab);
	void gensize();
};
#endif