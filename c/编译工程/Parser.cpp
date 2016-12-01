#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <set>
#include "Symbol.h"
#include "error.h"
#include "Lexer.h"
#include "parser.h"
#include "symboltable.h"
#include "quater.h"

using namespace std;



extern int lev;  //������
extern int tempcount;
extern Symbol sym;  //��ǰ�ĵ�������
extern Lexer lexer;
extern Error error;
extern QCode midCode;
extern int lc;

extern tabItem temp_item;
extern ofstream mipscodefile;
extern ofstream midcodefile;

extern table toptable;        //������ű�
extern tableManager tab_set;  //���ű����
extern int errornum;
int labelcount=0;

void Parser::initial()  //��ʼ��һЩͷ���ż�
{
	procbegsys.insert(constsym);
	procbegsys.insert(intsym);
	procbegsys.insert(charsym);
	procbegsys.insert(voidsym);

	constDecbegsys.insert(constsym);  //����constDec ������
	constDecendsys.insert(semicolon);
	constDefbegsys.insert(intsym);    //����constDef ������
	constDefbegsys.insert(charsym);

	statbegsys.insert(ifsym);
	statbegsys.insert(dosym);
	statbegsys.insert(forsym);
	statbegsys.insert(lbrace);
	statbegsys.insert(ident);
	statbegsys.insert(scanfsym);
	statbegsys.insert(printfsym);
	statbegsys.insert(returnsym);
	//statbegsys.insert(semicolon); 

	statendsys=statbegsys;
	statendsys.insert(rbrace);
	statendsys.insert(elsesym);

	facbegsys.insert(ident);
	facbegsys.insert(number);
	facbegsys.insert(unsgnumber);
	facbegsys.insert(chcon);
	facbegsys.insert(lparent);

	expressionbegsys=facbegsys;
	expressionbegsys.insert(Symbol::plus);
	expressionbegsys.insert(Symbol::minus);

	termbegsys=facbegsys;

	opsys.insert(eql);
	opsys.insert(neq);
	opsys.insert(gtr);
	opsys.insert(lss);
	opsys.insert(leq);
	opsys.insert(geq);
}
symset Parser::addsymset(symset s1,symset s2)
{
	symset s3;
	s3=s1;
	symset::iterator cp;
	cp=s2.begin();
	while(cp!=s2.end())
	{
		if(s3.find(*cp)==s3.end())
		{
			s3.insert(*cp);
		}
		cp++;
	}
	return s3;
}
void Parser::skip(symset s)
{
	while(s.find(sym)==s.end())
	{
		lexer.nextsym();
	}
}
void Parser::testsemicolon(int efnum,int emnum)
{
	if(sym==semicolon)
	{
		lexer.nextsym();
	}
	else
	{
		error.adderrormsg(efnum,emnum);
	}
}



Parser::Parser()
{
	funcflag=false;
	mainfuncflag=false;
	compoundStatementflag=false;
	constDecflag=false;
	varDefflag=false;
	initial();
}
void Parser::parser()
{
	cout<<"start parser!"<<endl;
	lexer.nextsym();
	if(procbegsys.find(sym)!=procbegsys.end())
	{
		prog(procbegsys);
	}
	else
	{
		error.adderrormsg(0,0);
		prog(procbegsys);
	}
	cout<<"end parser!"<<endl;
}

void Parser::prog(symset progbegin)
{
	cout<<"enter prog"<<endl;
	while(procbegsys.find(sym)!=procbegsys.end()) //��ʼ���ǺϷ�ͷ���ż�
	{
		if(sym==constsym)
		{
			if(!constDecflag)
			{
				constDec();
			}
			else
			{
				error.adderrormsg(1,1);
			}
		}
		else if(sym==intsym||sym==charsym)
		{
			var_funcDec();
		}
		else if(sym==voidsym)
		{
			constDecflag=true;
			voidDec();
		}
	}
	cout<<"leave prog"<<endl;
}
void Parser::constDec() //  ������˵����   ::=  const���������壾;{ const���������壾;}
{
	if(!compoundStatementflag && (( funcflag && !mainfuncflag) || ( funcflag && mainfuncflag )) )
	{
		error.adderrormsg(2,1);
	}
	else
	{
		do
		{
			lexer.nextsym();
			if( sym==intsym||sym==charsym )
			{
				constDef();
			} 
			else
			{
				error.adderrormsg(2,2);
			}
		}
		while(sym==constsym);
	}
	cout<<"This is a Const Dec!"<<endl;
}
//��� const
//���� ;��ĺϷ��ַ�
void Parser::constDef()
{                       
	temp_item.kind=CONST;
	if(sym==intsym)
	{
		do
		{
			temp_item.type=inttyp;
			lexer.nextsym();
			if(sym==ident)
			{
				temp_item.name=lexer.identsym; //��ʱ��������ǰ����ı�ʶ������
				int num,k ;
				k=1;
				lexer.nextsym();
				if(sym==becomes)
				{
					lexer.nextsym();
					if(sym==Symbol::minus||sym==Symbol::plus)
					{
						lexer.nextsym();
						if(sym==Symbol::minus)
						{
							k=-1;
						}
					}
					if(sym==unsgnumber||sym==number)
					{
						num=k*lexer.num;
						temp_item.value=num;
						tab_set.enterConstint(temp_item.type,temp_item.name,temp_item.value,tab_set.pCurrent_tab);
						lexer.nextsym();
					}
					else
					{
						error.adderrormsg(3,6);
					}
				}
				else
				{
					error.adderrormsg(3,5);
				}
			}
			else
			{
				error.adderrormsg(3,4);
			}
		}
		while(sym==comma);
	}
	else
	{
		do
		{
			temp_item.type=chartyp;
			lexer.nextsym();
			if(sym==ident)
			{
				temp_item.name=lexer.identsym;
				lexer.nextsym();
				if(sym==becomes)
				{
					lexer.nextsym();
					if(sym==chcon)
					{
						temp_item.conch=lexer.constch;
						tab_set.enterConstchar(temp_item.type,temp_item.name,temp_item.conch,tab_set.pCurrent_tab);
						lexer.nextsym();
					}
					else
					{
						error.adderrormsg(3,7);
					}
				}
				else
				{
					error.adderrormsg(3,5);
				}
			}
			else
			{
				error.adderrormsg(3,4);
			}
		}
		while(sym==comma);
	}
	testsemicolon(3,3);
	cout<<"This is a const Def!"<<endl;
}
// ��� int / char 
// ���� 
void Parser::var_funcDec()
{
	Symbol tsym=sym;
	lexer.nextsym();
	if(sym==ident) //�����Ǳ�־��
	{
		string word=lexer.identsym;
		lexer.nextsym();//��־������ֳ�����
		if(sym==comma || sym==semicolon || sym==lbrack)
		{
			if(!varDefflag)
			{
				varDef(word,tsym);
			}
			else
			{
				error.adderrormsg(4,8);
			}
		}
		else if(sym==lparent) //֤�����з���ֵ�ĺ���˵��
		{
			rtvaluefuncDec(word,tsym);
		}
		else
		{
			error.adderrormsg(4,0);
		}
	}
	else
	{
		error.adderrormsg(4,4);
	}
}
// ��� void 
// ���� 
void Parser::voidDec()
{
	lexer.nextsym();
	if(sym==ident) //��ʱ���޷���ֵ�ĺ���
	{
		nrvfuncDec(lexer.identsym);
	}
	else if(sym==mainsym)
	{
		completeaddr(tab_set.pCurrent_tab);
		Mainfunc();
	}
	else
	{
		error.adderrormsg(5,0);
	}
}
//���������壾   ::=   int����ʶ��������������{,����ʶ��������������}| char����ʶ���������ַ���{,����ʶ���������ַ���}
//          ��������        ::= �ۣ������ݣ��޷�������������

//���������壾   ::= �����ͱ�ʶ����(����ʶ����|����ʶ������[�����޷�����������]��){,(����ʶ����|����ʶ������[�����޷�����������]��) }

// ��� sym==comma || sym==semicolon || sym==lbrack
// ���� 
void Parser::varDef(string word,Symbol tsym) //tsym ���������Щ����������
{
	//cout<<"enter varDef"<<endl;
	if(tsym==intsym)
		temp_item.type=inttyp;
	else 
		temp_item.type=chartyp;
	if(sym==lbrack)  //��������֤��������
	{
		lexer.nextsym();//�����������ź�Ӧ�ö����޷�������
		if(sym==unsgnumber)
		{
			temp_item.size=lexer.num;
			lexer.nextsym();
			if(sym==rbrack)
			{
				lexer.nextsym();
				temp_item.name=word;
				tab_set.enterArray(temp_item.type,temp_item.name,temp_item.size,tab_set.pCurrent_tab);
			}
			else
			{
				error.adderrormsg(6,9);
			}
		}
		else
		{
			error.adderrormsg(6,6);
		}
	}
	else
	{
		temp_item.name=word;
		tab_set.enterVar(temp_item.type,temp_item.name,tab_set.pCurrent_tab);
	}
	while(sym==comma)
	{
		lexer.nextsym();
		if(sym==ident)
		{
			temp_item.name=lexer.identsym;
			lexer.nextsym();
			if(sym==lbrack) //֤��������
			{
				lexer.nextsym();
				if(sym==unsgnumber)
				{
					temp_item.size=lexer.num;
					lexer.nextsym();
					if(sym==rbrack)
					{
						lexer.nextsym();
						tab_set.enterArray(temp_item.type,temp_item.name,temp_item.size,tab_set.pCurrent_tab);
					}
					else
					{
						error.adderrormsg(6,9);
					}
				}
				else
				{
					error.adderrormsg(6,6);
				}
			}
			else //��������Ļ���������ͨ����
			{
				tab_set.enterVar(temp_item.type,temp_item.name,tab_set.pCurrent_tab);
			}
		}
		else
		{
			error.adderrormsg(6,4);
		}
	}
	testsemicolon(6,3);
	cout<<"This is a var  Dec!"<<endl;
}
//���з���ֵ�������壾  ::=  ������ͷ������(������������)�� ��{����������䣾��}��

// ��� (
// ���� 
void Parser::rtvaluefuncDec(string word,Symbol tsym) //word����ĺ�������tsym�Ǻ����ķ���ֵ����
{
	funcflag=true;
	if(mainfuncflag)
	{
		error.adderrormsg(7,9);
	}
	else
	{
		if(tsym==intsym)
			temp_item.type=inttyp;
		else
			temp_item.type=chartyp;
		temp_item.name=word;
		tab_set.enterFunc(temp_item.type,temp_item.name,labelcount+1,tab_set.pCurrent_tab);
		Operand funclabel;
		funclabel.opd_type=Opdtype::OPLABEL;
		funclabel.value.label=++labelcount;
		funclabel.opd_name=word;
		funclabel.t_kind=Tokenkind::FUNC;
		midCode.Qcode.push_back(QuaterInstr(lev,tab_set.pCurrent_tab,Opcode::LAB,funclabel));
		midCode.Qcode.push_back(QuaterInstr(lev,tab_set.pCurrent_tab,Opcode::SAVE));
		lexer.nextsym();  //���ڶ����Ӧ���ǲ������ͷ�ַ���������������
		if(sym==intsym||sym==charsym) //�������ͱ�־��ʱ����������б���
		{
			parameterList();
		}
		if(sym==rparent)
		{
			lexer.nextsym();
			if(sym==lbrace)
			{
				compoundStatement();
				if(sym==rbrace)
				{
					lexer.nextsym();
				}
				else
				{
					error.adderrormsg(7,11);
				}
			}
			else
			{
				error.adderrormsg(7,10);
			}
		}
		else
		{
			error.adderrormsg(7,12);
		}
		tab_set.exit_func(tab_set.pCurrent_tab);
		cout<<"This is a  return function!"<<endl;
	}
}
void Parser::nrvfuncDec(string word)
{
	funcflag=true;
	if(mainfuncflag)
	{
		error.adderrormsg(8,9);
	}
	else
	{
		temp_item.type=voidtyp;
		temp_item.name=word;
		tab_set.enterFunc(temp_item.type,temp_item.name,labelcount+1,tab_set.pCurrent_tab);
		Operand funclabel;
		funclabel.opd_type=Opdtype::OPLABEL;
		funclabel.value.label=++labelcount;
		funclabel.opd_name=word;
		funclabel.t_kind=Tokenkind::FUNC;
		midCode.Qcode.push_back(QuaterInstr(lev,tab_set.pCurrent_tab,Opcode::LAB,funclabel));
		midCode.Qcode.push_back(QuaterInstr(lev,tab_set.pCurrent_tab,Opcode::SAVE));
		lexer.nextsym();
		if(sym==lparent)
		{
			lexer.nextsym();
			if(sym==intsym||sym==charsym) //�������ͱ�־��ʱ����������б���
			{
				parameterList();
			}
		}
		if(sym==rparent)  //���������ţ�֤���޲����б�,���߲����б�����
		{
			lexer.nextsym();  //��ǰ������ַ�Ӧ���Ǵ�����
			if(sym==lbrace) 
			{
				compoundStatement();
				if(sym==rbrace)
				{
					lexer.nextsym();
				}
				else
				{
					error.adderrormsg(8,11);
				}
			}//���Ǵ����ű���
			else
			{
				error.adderrormsg(8,10);
			}
			tab_set.exit_func(tab_set.pCurrent_tab);
			cout<<"This is a no return function!"<<endl;
		}
	}
}
//��������     ::=  �����ͱ�ʶ��������ʶ����{,�����ͱ�ʶ��������ʶ����}| ���գ�
// ���Ϊ  int /char 
//����Ϊ  )
void Parser::parameterList()
{
	Symbol tsym; //���ڱ��浱ǰ�Ĳ�������
	if(sym==intsym)
		temp_item.type=inttyp;
	else
		temp_item.type=chartyp;
	tsym=sym;//��¼��ǰ����������
	lexer.nextsym();//���ڶ����Ӧ���Ǳ�ʶ����
	if(sym==ident)
	{
		temp_item.name=lexer.identsym;
		tab_set.enterFPA(temp_item.type,temp_item.name,tab_set.pCurrent_tab);
		lexer.nextsym();        //���ڶ����Ӧ���Ƕ��Ż���������
		if(sym==comma||sym==rparent)
		{
			while(sym==comma)
			{
				lexer.nextsym();
				if(sym==intsym||sym==charsym) //�����б��ź���Ӧ�������ͱ�־��
				{
					tsym=sym;
					if(sym==intsym)
						temp_item.type=inttyp;
					else
						temp_item.type=chartyp;
					lexer.nextsym();
					if(sym==ident)
					{
						temp_item.name=lexer.identsym;
						tab_set.enterFPA(temp_item.type,temp_item.name,tab_set.pCurrent_tab);
						lexer.nextsym();
					}
					else
					{
						error.adderrormsg(9,4);
					}
				}
				else
				{
					error.adderrormsg(9,2);
				}
			}
		} 
		else
		{
			error.adderrormsg(9,2);
		}
	}          
}
//���������� ::=  void main��(����)�� ��{����������䣾��}��
// ���Ϊ  main 
//����Ϊ 
void Parser::Mainfunc()
{
	cout<<"enter Mainfunc()"<<endl;
	Operand funclabel;
	funclabel.opd_type=Opdtype::OPLABEL;
	funclabel.value.label=++labelcount;
	funclabel.opd_name="_main";
	funclabel.t_kind=Tokenkind::FUNC;
	midCode.Qcode.push_back(QuaterInstr(lev,tab_set.pCurrent_tab,Opcode::LAB,funclabel));
	temp_item.type=voidtyp;
	temp_item.name="_main";
	temp_item.value=funclabel.value.label;
	tab_set.enterFunc(temp_item.type,temp_item.name,labelcount,tab_set.pCurrent_tab);
	lexer.nextsym();
	if(sym==lparent)
	{
		lexer.nextsym();
		if(sym==rparent)
		{
			lexer.nextsym();
			if(sym==lbrace)
			{
				compoundStatement();
				if(sym==rbrace)
				{
					lexer.nextsym();
				}//�����Ҵ����ű���
				else
				{
					error.adderrormsg(10,11);
				}
			}//����������ű���
			else
			{
				error.adderrormsg(10,10);
			}
		}//���������ű���
		else
		{
			error.adderrormsg(10,13);
		}
	}//���������ű���
	else
	{
		error.adderrormsg(10,12);
	}
	mainfuncflag=true;
	gensize();
	cout<<"leave Mainfunc()"<<endl;
}
//��������䣾::=�ۣ�����˵�����ݣۣ�����˵�����ݣ�����У�
//�������ĳ��ڲ���Ӧ����  ����еĳ���  ��һ�����Ŀ�ͷ �������� }
//��������ţ����븴�����
void Parser::compoundStatement()     
{
	cout<<"start compoundStatement "<<endl;
	compoundStatementflag=true;
	lexer.nextsym();  //�����ź�������� const int char �����
	if(sym==constsym)
	{
		constDec();
	}
	if(sym==intsym||sym==charsym)
	{
		newvarDec();
	}
	completeaddr(tab_set.pCurrent_tab);
	if(statbegsys.find(sym)!=statbegsys.end()) //��������
	{
		Statementcolumn();
	}
	else if(sym==rbrace)                    //�������û���
	{
	}
	else 
	{
		error.adderrormsg(11,2);
	}
	completetempaddr(tab_set.pCurrent_tab);
	compoundStatementflag=false;
	cout<<"leave compoundStatement() "<<endl;
}
//������˵����   ::= ���������壾;{���������壾;}
//���������壾   ::= �����ͱ�ʶ����(����ʶ����|����ʶ������[�����޷�����������]��){,(����ʶ����|����ʶ������[�����޷�����������]��) }
//���  int char
//����   }
void Parser::newvarDec()
{
	while(sym==intsym||sym==charsym)
	{
		newvarDef();
	}
}
//���  int char
//���� 
void Parser::newvarDef()
{
	Symbol tsym=sym;
	if(tsym==intsym)
		temp_item.type=inttyp;
	else 
		temp_item.type=chartyp;
	lexer.nextsym();
	if(sym==ident)
	{
		string name=lexer.identsym;
		temp_item.name=lexer.identsym;
		lexer.nextsym();
		if(sym==lbrack)
		{
			lexer.nextsym();
			if(sym==unsgnumber)
			{
				int size=lexer.num;
				temp_item.size=lexer.num;
				lexer.nextsym();
				if(sym==rbrack)
				{
					tab_set.enterArray(temp_item.type,temp_item.name,temp_item.size,tab_set.pCurrent_tab);
					lexer.nextsym();
				}
				else
				{
					error.adderrormsg(29,9);
				}
			}
			else
			{
				error.adderrormsg(29,6);
			}
		}
		else
		{
			tab_set.enterVar(temp_item.type,temp_item.name,tab_set.pCurrent_tab);
		}
		while(sym==comma)
		{
			lexer.nextsym();
			if(sym==ident)
			{
				temp_item.name=lexer.identsym;
				lexer.nextsym();
				if(sym==lbrack)
				{
					lexer.nextsym();
					if(sym==unsgnumber)
					{
						temp_item.size=lexer.num;
						lexer.nextsym();
						if(sym==rbrack)
						{
							tab_set.enterArray(temp_item.type,temp_item.name,temp_item.size,tab_set.pCurrent_tab);
							lexer.nextsym();
						}
						else
						{
							error.adderrormsg(29,9);
						}
					}
					else
					{
						error.adderrormsg(29,6);
					}
				}
				else
				{
					tab_set.enterVar(temp_item.type,temp_item.name,tab_set.pCurrent_tab);
				}
			}
		}
	}
	else
	{
		error.adderrormsg(29,2);
	}
	testsemicolon(29,3);
	cout<<"This is a var  Dec!"<<endl;
}
//����䣾       ::= ��������䣾����ѭ����䣾����{��������У���}�������з���ֵ����������䣾; 
//                   |���޷���ֵ����������䣾;������ֵ��䣾;��������䣾;����д��䣾;�����գ�;����������䣾;
void Parser::Statement()
{
	if(sym==ifsym)
	{
		conditionStatement();
	}
	else if(sym==dosym)
	{
		doloopStatement();
	}
	else if(sym==forsym)
	{
		forloopStatement();
	}
	else if(sym==ident)  //�����Ǹ�ֵ��䣬���ߺ����������
	{
		string name=lexer.identsym;  //��¼һ�¸�ֵ�Ķ���
		lexer.nextsym();
		if(sym==becomes||sym==lbrack)//�Ǹ�ֵ���
		{
			tabItem var_ident=*tab_set.find_item(tab_set.pCurrent_tab,name);
			if(var_ident.kind==Tokenkind::VAR||var_ident.kind==Tokenkind::ARRAY||var_ident.kind==Tokenkind::FPA)
			{
				Operand des;
				des.opd_type=Opdtype::OPIDENT;
				des.t_kind=var_ident.kind;
				des.tabx=tab_set.pCurrent_tab;
				des.opd_name=var_ident.name;
				des.t_typ=var_ident.type;
				assignStatement(var_ident,des);
			}
			else
			{
				error.adderrormsg(12,15);
			}
		}
		else if(sym==lparent)    //���ú���
		{
			tabItem func;
			func=*tab_set.find_item(tab_set.pCurrent_tab,name);
			if(func.kind==Tokenkind::FUNC)
			{
				call(func);
			}
			else
			{
				error.adderrormsg(12,14);
			}
		}
		else
		{
			error.adderrormsg(12,2);
		}
	}
	else if(sym==scanfsym) //����������Ĵ���
	{
		scanfStatement();
	}
	else if(sym==printfsym)  //���������䴦��
	{
		printfStatement();
	}
	else if(sym==returnsym)  //�������
	{
		returnStatement();
	}
	else if(sym==lbrace)//����������ж�
	{
		lexer.nextsym();
		if(statbegsys.find(sym)!=statbegsys.end()) //����зǿ�
		{
			Statementcolumn();
		}
		if(sym==rbrace)             //�����Ϊ��
		{
			lexer.nextsym();
		}
		else
		{
			error.adderrormsg(12,10);
		}
		return ;
	}
	else if(sym==semicolon) //ֱ���Ƿֺţ�����䣬ֱ�Ӵ������
	{
		return ;
	}
}
//������У�     ::=������䣾�� ����ֻ�������������������� 
//��� �еĳ��� �������ĳ���
// ���������֮ǰ���Ѿ������� ��ںϷ��Լ���
//����еĳ��� ��  ��������е����ַ�  
void Parser::Statementcolumn()
{
	while(statbegsys.find(sym)!=statbegsys.end())
	{
		Statement();
	}
}
//��������䣾   ::=  if ��(������������)������䣾��else����䣾��
//��������   ::=  �����ʽ������ϵ������������ʽ���������ʽ�� //���ʽΪ0����Ϊ�٣�����Ϊ��
// ��� ��if
void Parser::conditionStatement()
{
	Operand else_label,endif_label;
	lexer.nextsym();
	if(sym==lparent)
	{
		lexer.nextsym();
		if(expressionbegsys.find(sym)!=expressionbegsys.end())
	    {
			else_label.opd_type=Opdtype::OPLABEL;
			else_label.value.label=++labelcount;
		    Condition(else_label,true);
			if(sym==rparent)
			{
				lexer.nextsym();
				if(statbegsys.find(sym)!=statbegsys.end())
				{
					Statement();
				}
				if(sym==elsesym)
				{
					endif_label.opd_type=Opdtype::OPLABEL;
					endif_label.value.label=++labelcount;
					midCode.Qcode.push_back(QuaterInstr(lev,tab_set.pCurrent_tab,Opcode::JMP,endif_label));
					midCode.Qcode.push_back(QuaterInstr(lev,tab_set.pCurrent_tab,Opcode::LAB,else_label));
					lexer.nextsym();
					if(statbegsys.find(sym)!=statbegsys.end())
					{
					    Statement();
					}
					midCode.Qcode.push_back(QuaterInstr(lev,tab_set.pCurrent_tab,Opcode::LAB,endif_label));
				}
				else
				{
					midCode.Qcode.push_back(QuaterInstr(lev,tab_set.pCurrent_tab,Opcode::LAB,else_label));
				}
			}
			else
			{
				error.adderrormsg(13,13);
			}
		}
		else
		{
			error.adderrormsg(13,2);
		}
	}
	else
	{
		error.adderrormsg(13,12);
	}
	cout<<"This is a conditionStatement!"<<endl;
}
//��������   ::=  �����ʽ������ϵ������������ʽ���������ʽ�� //���ʽΪ0����Ϊ�٣�����Ϊ��
void Parser::Condition(Operand& opd,bool flag)
{
	Operand op1,op2;
	Symbol temp_sym;
	expression(op1);
	if(opsys.find(sym)!=opsys.end())
	{
		temp_sym=sym;
		lexer.nextsym();
		if(expressionbegsys.find(sym)!=expressionbegsys.end())
		{
			expression(op2);
			if(flag)
			{
				switch (temp_sym)
				{
				case Symbol::eql:
					midCode.Qcode.push_back(QuaterInstr(lev,tab_set.pCurrent_tab,Opcode::BNE,opd,op1,op2));
					break;
				case Symbol::neq:
					midCode.Qcode.push_back(QuaterInstr(lev,tab_set.pCurrent_tab,Opcode::BEQ,opd,op1,op2));
					break;
				case Symbol::lss:
					midCode.Qcode.push_back(QuaterInstr(lev,tab_set.pCurrent_tab,Opcode::BGE,opd,op1,op2));
					break;
				case Symbol::gtr:
					midCode.Qcode.push_back(QuaterInstr(lev,tab_set.pCurrent_tab,Opcode::BLE,opd,op1,op2));
					break;
				case Symbol::geq:
					midCode.Qcode.push_back(QuaterInstr(lev,tab_set.pCurrent_tab,Opcode::BLT,opd,op1,op2));
					break;
				case Symbol::leq:
					midCode.Qcode.push_back(QuaterInstr(lev,tab_set.pCurrent_tab,Opcode::BGT,opd,op1,op2));
					break;
				}
			}
			else
			{
				switch (temp_sym)
				{
				case Symbol::eql:
					midCode.Qcode.push_back(QuaterInstr(lev,tab_set.pCurrent_tab,Opcode::BEQ,opd,op1,op2));
					break;
				case Symbol::neq:
					midCode.Qcode.push_back(QuaterInstr(lev,tab_set.pCurrent_tab,Opcode::BNE,opd,op1,op2));
					break;
				case Symbol::lss:
					midCode.Qcode.push_back(QuaterInstr(lev,tab_set.pCurrent_tab,Opcode::BLT,opd,op1,op2));
					break;
				case Symbol::gtr:
					midCode.Qcode.push_back(QuaterInstr(lev,tab_set.pCurrent_tab,Opcode::BGT,opd,op1,op2));
					break;
				case Symbol::geq:
					midCode.Qcode.push_back(QuaterInstr(lev,tab_set.pCurrent_tab,Opcode::BGE,opd,op1,op2));
					break;
				case Symbol::leq:
					midCode.Qcode.push_back(QuaterInstr(lev,tab_set.pCurrent_tab,Opcode::BLE,opd,op1,op2));
					break;
				}
			}
		}
	}
	else
	{
		error.adderrormsg(14,16);
	}
}
//��ѭ����䣾   ::=  do����䣾while ��(������������)��
   //                 |for��(������ʶ�����������ʽ��;��������;����ʶ����������ʶ����(+|-)����������)������䣾
//��� �� dosym
//���� �� } /statbegsys
void Parser::doloopStatement()
{
	Operand loop_label;
	lexer.nextsym();
	loop_label.opd_type=Opdtype::OPLABEL;
	loop_label.value.label=++labelcount;
	midCode.Qcode.push_back(QuaterInstr(lev,tab_set.pCurrent_tab,Opcode::LAB,loop_label));
	if(statbegsys.find(sym)!=statbegsys.end())
	{
		Statement();
	}
	if(sym==whilesym)
	{	
		lexer.nextsym();
		if(sym==lparent)
		{
			lexer.nextsym();
			if(expressionbegsys.find(sym)!=expressionbegsys.end())
			{
				Condition(loop_label,false);
			}
			if(sym==rparent)
			{
				lexer.nextsym();
			}
			else
			{
				error.adderrormsg(15,13);
			}
		}
		else
		{
			error.adderrormsg(15,12);
		}
	}
	else
	{
		error.adderrormsg(15,17);
	}
	cout<<"This is a doloopStatement!"<<endl;
}
void Parser::forloopStatement()
{
	lexer.nextsym();
	Opcode op;
	if(sym==lparent)
	{
		Operand initial_value,temp_init;
		tabItem first_ident,second_ident;
		lexer.nextsym();
		if(sym==ident)
		{
			first_ident=*tab_set.find_item(tab_set.pCurrent_tab,lexer.identsym);
			if(first_ident.kind!=Tokenkind::VAR)
			{
				error.adderrormsg(16,18);
			}
			else
			{
				temp_init.opd_name=first_ident.name;
				temp_init.opd_type=Opdtype::OPIDENT;
				temp_init.t_typ=Tokentype::inttyp;
				temp_init.tabx=tab_set.pCurrent_tab;
				lexer.nextsym();
				if(sym==becomes)
				{
					lexer.nextsym();
					if(expressionbegsys.find(sym)!=expressionbegsys.end())
					{
						expression(initial_value);
						temp_init.value.cont=initial_value.value.cont;
						midCode.Qcode.push_back(QuaterInstr(lev,tab_set.pCurrent_tab,Opcode::ASS,temp_init,initial_value));
					}
					else
					{
						error.adderrormsg(16,2);
					}
				}
				else
				{
					error.adderrormsg(16,5);
				}
			}
		}
		else
		{
			error.adderrormsg(16,2);
		}
		testsemicolon(16,3);
		Operand loop_label,end_loop_label,stepopd;
		loop_label.value.label=++labelcount;
		loop_label.opd_type=Opdtype::OPLABEL;
		end_loop_label.value.label=++labelcount;
		end_loop_label.opd_type=Opdtype::OPLABEL;
		midCode.Qcode.push_back(QuaterInstr(lev,tab_set.pCurrent_tab,Opcode::LAB,loop_label));
		if(expressionbegsys.find(sym)!=expressionbegsys.end())
		{
			Condition(end_loop_label,true);
			testsemicolon(17,6);
		}
		else
		{
			error.adderrormsg(16,2);
		}	
		if(sym==ident)
		{
			lexer.nextsym();
			if(sym==becomes)
			{
				lexer.nextsym();
				if(sym==ident)
				{
					lexer.nextsym();
					if(sym==Symbol::plus||sym==Symbol::minus)
					{
						if(sym==Symbol::plus)
							op=Opcode::ADD;
						else
							op=Opcode::SUB;
						lexer.nextsym();
						if(sym==unsgnumber)
						{
							stepopd.value.cont=lexer.num;
							stepopd.opd_type=Opdtype::OPCONST;
							stepopd.t_typ=Tokentype::inttyp;
							lexer.nextsym();
						}
						else
						{
							error.adderrormsg(16,6);
						}
					}
					else
					{
						error.adderrormsg(16,19);
					}
				}
				else
				{
					error.adderrormsg(16,4);
				}
			}
			else
			{
				error.adderrormsg(16,5);
			}
		}
		if(sym==rparent)
		{
			lexer.nextsym();
			if(statbegsys.find(sym)!=statbegsys.end())
			{
				Statement();
				midCode.Qcode.push_back(QuaterInstr(lev,tab_set.pCurrent_tab,op,temp_init,temp_init,stepopd));
				midCode.Qcode.push_back(QuaterInstr(lev,tab_set.pCurrent_tab,Opcode::JMP,loop_label));
				midCode.Qcode.push_back(QuaterInstr(lev,tab_set.pCurrent_tab,Opcode::LAB,end_loop_label));
			}
			else
			{
				error.adderrormsg(16,2);
			}
		}
		else
		{
			error.adderrormsg(16,13);
		}
	}
	cout<<"This is a forloopStatement!"<<endl;
}
//��� �� (
//���� �� ;����һ������
void Parser::call(tabItem func)
{
	Operand des;
	if(func.type==inttyp||func.type==chartyp)
	{
		callrtvalueFunc(func,des);
	    cout<<"This is a callrtvaluefunc Statment!"<<endl;
	}
	else if(func.type==voidtyp)
	{
		callnrtvalueFunc(func);
	    cout<<"This is a callnrtvaluefunc Statment!"<<endl;
	}
	if(sym==rparent)
	{
		lexer.nextsym();
	}
	testsemicolon(17,6);
}
void Parser::callrtvalueFunc(tabItem func,Operand& des)
{
	cout<<"start callrtvalueFunc"<<endl;
	Operand funcopd;
	lexer.nextsym();
	if(expressionbegsys.find(sym)!=expressionbegsys.end())
	{
		valueparameterTable(func,des);
	}
	funcopd.opd_name=func.name;
	funcopd.tabx=tab_set.pCurrent_tab;
	des.opd_type=Opdtype::OPIDENT;
	des.tabx=tab_set.pCurrent_tab;
	funcopd.retflag=1;
	stringstream temp_name;
	temp_name.str("");
	temp_name<<"[temp"<<++tempcount<<"]";
	des.opd_name=temp_name.str();
	des.t_typ=func.type;
	tab_set.enterTEMP(des.t_typ,des.opd_name,0,tab_set.pCurrent_tab);

	midCode.Qcode.push_back(QuaterInstr(lev,tab_set.pCurrent_tab,Opcode::CAL,funcopd,des));
	cout<<"leave callrtvalueFunc"<<endl;
}
//���޷���ֵ����������䣾 ::= ����ʶ������(����ֵ��������)��
void Parser::callnrtvalueFunc(tabItem func)
{
	cout<<"start callnrtvalueFunc"<<endl;
	Operand des;
	lexer.nextsym();
	if(expressionbegsys.find(sym)!=expressionbegsys.end())
	{
		valueparameterTable(func,des);
	}
	des.opd_name=func.name;
	des.t_typ=func.type;
	des.tabx=tab_set.pCurrent_tab;
	des.retflag=0;
	midCode.Qcode.push_back(QuaterInstr(lev,tab_set.pCurrent_tab,Opcode::CAL,des));
	cout<<"leave callnrtvalueFunc"<<endl;
}
//��ֵ������   ::= �����ʽ��{,�����ʽ��}�����գ�
// ���  �� �Ϸ��ı��ʽ��ͷ
//����   �� �����б��һ��
// ���ܳ��ֵĴ���  �������Ͳ�ƥ�� ������������ƥ�䣬
void Parser::valueparameterTable(tabItem func,Operand& des)
{
	cout<<"start valueparameterTable"<<endl;
	tabItem par_temp;
	int i=0; //��������Ĳ�����ָ��
	Opcode op;
	op=Opcode::PUSH;
	Operand src1,src2;
	int func_tab=func.ptfunctab;
	par_temp=tab_set.Tab[func_tab].tab[i];
	i=i+1;
	if(sym!=rparent)
	{
		if(par_temp.kind==Tokenkind::FPA)
		{
			expression(src1);
			src2.opd_name=func.name;
			src2.tabx=func.ptfunctab;
			src2.value.cont=i;
			i=i+1;
			midCode.Qcode.push_back(QuaterInstr(lev,tab_set.pCurrent_tab,op,src1,src2));
			while(sym==comma)
			{
				lexer.nextsym();
				par_temp=tab_set.Tab[func_tab].tab[i-1];
				if(par_temp.kind==Tokenkind::FPA)
				{
					if(expressionbegsys.find(sym)!=expressionbegsys.end())
					{
						expression(src1);
						src2.opd_name=func.name;
						src2.tabx=func.ptfunctab;
						src2.value.cont=i;
						midCode.Qcode.push_back(QuaterInstr(lev,tab_set.pCurrent_tab,op,src1,src2));
						i=i+1;
					}
					else
					{
						error.adderrormsg(20,2);
					}
				}
				else
				{
					error.adderrormsg(20,20);
				}
			}
		}
		else
		{
			error.adderrormsg(20,20);
		}
	}
	cout<<"leave valueparameterTable"<<endl;
}
//����ֵ��䣾::=����ʶ�����������ʽ��|����ʶ������[�������ʽ����]��=�����ʽ��
//����  �ֺź� һ��
// ��� ��־����һ��
void Parser::assignStatement(tabItem var_ident,Operand& des)
{
	cout<<"start assignstatement"<<endl;
	Operand src,opd1,opd2;
	if(sym==lbrack)//��ֵ�Ķ���������Ԫ��
	{
		if(var_ident.kind==Tokenkind::ARRAY)
		{
			opd1=des;
			lexer.nextsym();
			if(expressionbegsys.find(sym)!=expressionbegsys.end())
			{
				expression(opd2);
				des.opd_type=Opdtype::OPTEMP;
				des.t_kind=Tokenkind::ARRAY;
				stringstream temp_name;
				temp_name<<"[temp"<<++tempcount<<"]";
				des.opd_name=temp_name.str();
				des.tabx=tab_set.pCurrent_tab;
				tab_set.enterTEMP(des.t_typ,des.opd_name,0,tab_set.pCurrent_tab);
				midCode.Qcode.push_back(QuaterInstr(lev,tab_set.pCurrent_tab,Opcode::OFF,des,opd1,opd2));
				if(sym==rbrack)
				{
					lexer.nextsym();
				}
				else
				{
					error.adderrormsg(21,9);
				}
			}
			else
			{
				error.adderrormsg(21,2);
			}
		}
		else
		{
			error.adderrormsg(21,21);
		}
	}
	if(sym==becomes)
	{
		lexer.nextsym();
		if(expressionbegsys.find(sym)!=expressionbegsys.end())
		{
			expression(src);
			midCode.Qcode.push_back(QuaterInstr(lev,tab_set.pCurrent_tab,Opcode::ASS,des,src));
		}
		else
		{
			error.adderrormsg(21,2);
		}
	}
	else
	{
		error.adderrormsg(21,5);
	}
	testsemicolon(21,3);
	midcodefile<<"This is a assignStatement Statement"<<endl;
}
//��д��䣾::= printf��(�����ַ�����,�����ʽ����)��|printf ��(�����ַ�������)��|printf ��(�������ʽ����)��
//���� �� ��һ���Ϸ���ͷ������ }
//��� �� printf 
void Parser::printfStatement()
{
	Operand src1,src2;
	Opcode pf_op=Opcode::PRF;
	lexer.nextsym();
	if(sym==lparent)
	{
		lexer.nextsym();
		if(sym==strcon)
		{
			src1.str=lexer.constr;
			src1.opd_type=Opdtype::OPSTRING;
			midCode.Qcode.push_back(QuaterInstr(lev,tab_set.pCurrent_tab,pf_op,src1));
			lexer.nextsym();
			if(sym==comma)
			{
				lexer.nextsym();
				if (expressionbegsys.find(sym)!=expressionbegsys.end())
				{
					expression(src2);
					midCode.Qcode.push_back(QuaterInstr(lev,tab_set.pCurrent_tab,pf_op,src2));
					if(sym==rparent)
					{
						lexer.nextsym();
						testsemicolon(22,6);
					}
					else
					{
						error.adderrormsg(22,13);
					}
				}
				else
				{
					error.adderrormsg(22,2);
				}
			}
			else if(sym==rparent)
			{
				lexer.nextsym();
				testsemicolon(22,3);
			}
			else
			{
				error.adderrormsg(22,13);
			}
		}
		else if (expressionbegsys.find(sym)!=expressionbegsys.end())
		{
			expression(src2);
			midCode.Qcode.push_back(QuaterInstr(lev,tab_set.pCurrent_tab,pf_op,src2));
			if(sym==rparent)
			{
				lexer.nextsym();
				testsemicolon(22,3);
			}
			else
			{
				error.adderrormsg(22,13);
			}
		}
		else
		{
			error.adderrormsg(22,2);
		}
	}
	else
	{
		error.adderrormsg(22,12);
	}
	midcodefile<<"This is a printf Statement"<<endl;
}
//������䣾::=scanf ��(������ʶ����{,����ʶ����}��)��
//����ʱ �ĵ�ǰ�ַ�Ӧ���� �Ҵ����ţ����� ��һ���Ϸ�����俪ͷ
void Parser::scanfStatement()
{
	lexer.nextsym();
	tabItem temp;
	Operand src;
	Opcode sf_op=Opcode::SCF;
	if(sym==lparent)
	{
		lexer.nextsym();
		if(sym==ident)
		{
			if(tab_set.find_item(tab_set.pCurrent_tab,lexer.identsym)!=NULL)
			{
				temp=*tab_set.find_item(tab_set.pCurrent_tab,lexer.identsym);
				src.opd_name=temp.name;
				src.t_typ=temp.type;
				src.tabx=temp.ptfunctab;
				src.t_kind=temp.kind;
				midCode.Qcode.push_back(QuaterInstr(lev,tab_set.pCurrent_tab,sf_op,src));
				lexer.nextsym();
				while(sym==comma)
				{
					lexer.nextsym();
					if(sym==ident)
					{
						if(tab_set.find_item(tab_set.pCurrent_tab,lexer.identsym)!=NULL)
						{
							temp=*tab_set.find_item(tab_set.pCurrent_tab,lexer.identsym);
							src.opd_name=temp.name;
							midCode.Qcode.push_back(QuaterInstr(lev,tab_set.pCurrent_tab,sf_op,src));
							lexer.nextsym();
						}
						else
						{
							error.adderrormsg(23,22);
						}
					}
					else
					{
						error.adderrormsg(23,4);
					}
				}
				if(sym==rparent)
				{
					lexer.nextsym();
					testsemicolon(23,3);
				}
				else
				{
					error.adderrormsg(23,13);
				}
			}
			else
			{
				error.adderrormsg(23,22);
			}
		}
		else
		{
			error.adderrormsg(23,13);
		}
	}
	midcodefile<<"This is a scanf Statement"<<endl;
}
//��������䣾   ::=  return[��(�������ʽ����)��]
//����ʱ �ĵ�ǰ�ַ�Ӧ���� �Ҵ����ţ����� ��һ���Ϸ�����俪ͷ
// ��� �� return 
void Parser::returnStatement()
{
	Operand src;
	Opcode ret_op=Opcode::RET;
	lexer.nextsym();
	if(sym==lparent)
	{
		lexer.nextsym();
		if(expressionbegsys.find(sym)!=expressionbegsys.end())
		{
			expression(src);
			src.retflag=1;
			midCode.Qcode.push_back(QuaterInstr(lev,tab_set.pCurrent_tab,ret_op,src));
			if(sym==rparent)
			{
				lexer.nextsym();
			}
			else
			{
				error.adderrormsg(24,13);
			}
		}
	}
	else
	{
		src.retflag=0;
		midCode.Qcode.push_back(QuaterInstr(lev,tab_set.pCurrent_tab,ret_op,src));
	}
	testsemicolon(24,3);
	cout<<"This is a return Statement"<<endl;
} 
//�����ʽ��::= �ۣ������ݣ��{���ӷ�����������}
//��������ʽ�󣬵�ǰ��symӦ����  ) , ; , ] ,
//����expression ֮ǰ �Ѿ�����Ƿ��ǵ�ǰ�ַ��Ƿ��� ���ĺϷ�����ż�
//�뿪expression ʱ�������ı��ʽ�����һ���ַ���
void Parser::expression(Operand& src1)
{
	midcodefile<<"������ʽ����"<<endl;
	int i=1;
	Operand src2,temp,ntemp;
	Opcode exp_op;
	int value;
	if(sym==Symbol::plus||sym==Symbol::minus)
	{
		if(sym==Symbol::minus)
			i=-1;
		lexer.nextsym();
	}
	if(termbegsys.find(sym)!=termbegsys.end())
	{
		term(src1);
		if(i==-1)
		{
			if(src1.opd_type==Opdtype::OPIDENT||src1.opd_type==Opdtype::OPTEMP)
			{
			   ntemp.opd_type=src1.opd_type;
			   ntemp.t_typ=src1.t_typ;
			   ntemp.tabx=tab_set.pCurrent_tab;
			   ntemp.offset=src1.offset;
			   stringstream ntemp_name;
			   ntemp_name<<"[temp"<<++tempcount<<"]";
			   ntemp.opd_name=ntemp_name.str();
			   tab_set.enterTEMP(ntemp.t_typ,ntemp.opd_name,ntemp.value.cont,tab_set.pCurrent_tab);
			   midCode.Qcode.push_back(QuaterInstr(lev,tab_set.pCurrent_tab,Opcode::NEG,ntemp,src1));
			   src1=ntemp;
			}
			else if(src1.opd_type==Opdtype::OPCONST)
			{
				src1.value.cont=-src1.value.cont;
			}
		}
		while(sym==Symbol::plus||sym==Symbol::minus)
		{
			if(sym==Symbol::minus)
				exp_op=Opcode::SUB;
			else
				exp_op=Opcode::ADD;
			lexer.nextsym();
			if(termbegsys.find(sym)!=termbegsys.end())
			{
				term(src2);
				if(src1.t_typ==src2.t_typ||
					(src1.t_typ==Tokentype::chartyp&&src2.t_typ==Tokentype::inttyp)||
					(src2.t_typ==Tokentype::chartyp&&src1.t_typ==Tokentype::inttyp)
					)
				{
					if(exp_op==Opcode::ADD)
						temp.value.cont=src1.value.cont+src2.value.cont;
					else
						temp.value.cont=src1.value.cont-src2.value.cont;
					//����������ǳ������Ͳ��������м�����ˣ�Ҳ�������ɴ��룬ֱ��ת���ɳ�������
					if(src1.opd_type==Opdtype::OPCONST &&
						src2.opd_type==Opdtype::OPCONST)
					{
						temp.opd_type=Opdtype::OPCONST;
						temp.t_typ=src1.t_typ;
						temp.tabx=tab_set.pCurrent_tab;
						src1=temp;
					}
					else
					{
						temp.opd_type=Opdtype::OPIDENT;
						temp.t_typ=Tokentype::inttyp;
						temp.tabx=tab_set.pCurrent_tab;
						stringstream temp_name;
						temp_name<<"[temp"<<++tempcount<<"]";
						temp.opd_name=temp_name.str();
						tab_set.enterTEMP(temp.t_typ,temp.opd_name,temp.value.cont,tab_set.pCurrent_tab);
						midCode.Qcode.push_back(QuaterInstr(lev,tab_set.pCurrent_tab,exp_op,temp,src1,src2));
						src1=temp;
					}
				}
				else
				{
					error.adderrormsg(25,23);
				}
			}
			else
			{
				error.adderrormsg(25,2);
			}
		}
	}
	else
	{
		error.adderrormsg(25,2);
	}
	midcodefile<<"�뿪���ʽ����"<<endl;
}
//���::= �����ӣ�{���˷�������������ӣ�}
//�ٽ��� ���֮ǰ���ȼ�⵱ǰ���ַ��Ƿ�Ϸ�
//�뿪  ���ʱ�� �������� ���Ӻ����һ���ַ�
void Parser::term(Operand& src1)
{
	midcodefile<<"�������"<<endl;
	Operand src2,temp;
	Opcode term_op;
	int value;
	if(facbegsys.find(sym)!=facbegsys.end())
	{
		factor(src1);
		while(sym==times||sym==divid)
		{
			if(sym==times)
				term_op=Opcode::MUL;
			else
				term_op=Opcode::DIV;
			lexer.nextsym();
			if(facbegsys.find(sym)!=facbegsys.end())
			{
				factor(src2);
				if(src1.t_typ==src2.t_typ||
					(src1.t_typ==Tokentype::chartyp&&src2.t_typ==Tokentype::inttyp)||
					(src2.t_typ==Tokentype::chartyp&&src1.t_typ==Tokentype::inttyp)
					)
				{
					//����������ǳ������Ͳ��������м������
					if(src1.opd_type==Opdtype::OPCONST &&
						src2.opd_type==Opdtype::OPCONST)
					{
						if(term_op==Opcode::MUL)
							temp.value.cont=src1.value.cont*src2.value.cont;
						else
							temp.value.cont=src1.value.cont/src2.value.cont;
						temp.opd_type=Opdtype::OPCONST;
						temp.t_typ=src1.t_typ;
						temp.tabx=tab_set.pCurrent_tab;
					    src1=temp;
					}
					else
					{
						temp.opd_type=Opdtype::OPIDENT;
						temp.t_typ=Tokentype::inttyp;
						temp.tabx=tab_set.pCurrent_tab;
						stringstream temp_name;
						temp_name<<"[temp"<<++tempcount<<"]";
						temp.opd_name=temp_name.str();
						tab_set.enterTEMP(temp.t_typ,temp.opd_name,temp.value.cont,tab_set.pCurrent_tab);
						midCode.Qcode.push_back(QuaterInstr(lev,tab_set.pCurrent_tab,term_op,temp,src1,src2));
						src1=temp;
					}
				}
				else
				{
					error.adderrormsg(26,23);
				}
			}
			else
			{
				error.adderrormsg(26,23);
			}
		}
	}
	else
	{
		error.adderrormsg(26,2);
	}
	midcodefile<<"�뿪���"<<endl;
}
//�����ӣ� ::= ����ʶ����������ʶ������[�������ʽ����]����
//        ��������|���ַ��������з���ֵ����������䣾|��(�������ʽ����)��
// �������Ӵ���֮ǰ�����ж��Ƿ�Ϸ�
// �뿪���Ӵ���   �����������Ӻ���һ���ַ�
void Parser::factor(Operand& des)
{
	midcodefile<<"�������Ӵ���"<<endl;
	if(sym==ident)
	{
		factorpart1(des);
	}
	else if(sym==Symbol::plus||sym==Symbol::minus||sym==number||sym==unsgnumber)
	{
		factorpart2(des);
	}
	else if(sym==lparent)
	{
		factorpart3(des);
	}
	else if(sym==chcon)
	{
		 lexer.nextsym();
	}
	midcodefile<<"�뿪���Ӵ���"<<endl;
}
// ��� ��ident
// ���� �� ������һ���ַ�
//��һ���֣����������Ǻ������ã������Ǳ����������������������
void Parser::factorpart1(Operand& des)
{
	Operand ident;
	Symbol tsym=sym;      //�ȼ�¼��ǰ�ı��������ͺ�����
	string name=lexer.identsym;
	tabItem C_item;
	C_item=*tab_set.find_item(tab_set.pCurrent_tab,name);

	lexer.nextsym();

	des.opd_name=C_item.name;
	des.t_typ=C_item.type;
	des.t_kind=C_item.kind;
	des.offset=C_item.addr;
	des.tabx=tab_set.pCurrent_tab;
	
	if(sym==lbrack)  //�����������ţ�Ӧ������������
	{
		if(C_item.kind==Tokenkind::ARRAY)
		{
			des.opd_type=Opdtype::OPIDENT;
			des.t_kind==Tokenkind::ARRAY;
			Operand opd1,opd2;
			opd1=des;
			lexer.nextsym();
			if(expressionbegsys.find(sym)!=expressionbegsys.end())
			{
				expression(opd2);
				if(opd2.t_typ!=Tokentype::inttyp)
				{
				    error.adderrormsg(27,24);
				}
				else
				{
					des.opd_type=Opdtype::OPTEMP;
					stringstream temp_name;
					temp_name<<"[temp"<<++tempcount<<"]";
					des.opd_name=temp_name.str();
					tab_set.enterTEMP(des.t_typ,des.opd_name,des.value.cont,tab_set.pCurrent_tab);
					midCode.Qcode.push_back(QuaterInstr(lev,tab_set.pCurrent_tab,Opcode::SEL,des,opd1,opd2));
				}
				if(sym==rbrack)
				{
					lexer.nextsym();
				}
				else
				{
					error.adderrormsg(27,9);
				}
			}
			else
			{
				error.adderrormsg(27,2);
			}
		}
		else
		{
			error.adderrormsg(27,21);
		}
	}
	else if(sym==lparent)  // ���������ţ�Ӧ�����з���ֵ��������   ���з���ֵ����������䣾 ::= ����ʶ������(����ֵ��������)����ֵ������   ::= �����ʽ��{,�����ʽ��}�����գ�
	{
		callrtvalueFunc(C_item,des);
		if(sym==rparent)
		{
			lexer.nextsym();
		}
		else
		{
			error.adderrormsg(27,13);
		}
	}
	else
	{
		des.value.cont=C_item.value;
		if(C_item.kind==Tokenkind::VAR||C_item.kind==Tokenkind::FPA)
		{
			des.opd_type=Opdtype::OPIDENT;
		}
		else if(C_item.kind==Tokenkind::CONST)
		{
			des.opd_type=Opdtype::OPCONST;
		}
	}
}
//��һ�����Ƕ������������� ����������
void Parser::factorpart2(Operand& des)
{
	Operand temp;
	int i=1;
	int num;
	if(sym==Symbol::plus||sym==Symbol::minus)
	{
		if(sym==Symbol::minus)
		{
			i=-1;
		}
		lexer.nextsym();
	}
	num=lexer.num*i;
	des.tabx=tab_set.pCurrent_tab;
	des.t_typ=Tokentype::inttyp;
	des.value.cont=num;
	des.opd_type=OPCONST;
	lexer.nextsym();
}
//�ݹ���� ���ʽ���� ���������Ǳ��ʽ�����
void Parser::factorpart3(Operand& des)
{
	lexer.nextsym();
	if(expressionbegsys.find(sym)!=expressionbegsys.end())
	{
		expression(des);
		if(sym==rparent)
		{
			lexer.nextsym();
		}
		else
		{
			error.adderrormsg(28,13);
		}
	}
	else
	{
		error.adderrormsg(28,2);
	}
}
//���������ֽ����󣬶��������ֵĵ�ַ���з���
void Parser::completeaddr(int Current_Tab)
{
	int i;
	int addr=0;
	for(i=0;i<tab_set.Tab[Current_Tab].tab.size();i++)
	{
		if(tab_set.Tab[Current_Tab].tab[i].kind==CONST)
		{
			tab_set.Tab[Current_Tab].tab[i].addr=addr;
			addr=addr+4;
		}
		else if(tab_set.Tab[Current_Tab].tab[i].kind==VAR)
		{
			tab_set.Tab[Current_Tab].tab[i].addr=addr;
			addr=addr+4;
		}
		else if(tab_set.Tab[Current_Tab].tab[i].kind==ARRAY)
		{
			tab_set.Tab[Current_Tab].tab[i].addr=addr;
			addr=addr+4*tab_set.Tab[Current_Tab].tab[i].size;
		}
		else if(tab_set.Tab[Current_Tab].tab[i].kind==FPA)
		{
			tab_set.Tab[Current_Tab].tab[i].addr=addr;
			addr=addr+4;
		}
	}
}
//�ڴ��������󣬶������ɵ��м���ʱ�������е�ַ����
void Parser::completetempaddr(int Current_Tab)
{
	int i,addr=0;
	for(i=0;i<tab_set.Tab[Current_Tab].tab.size();i++)
	{
		if(tab_set.Tab[Current_Tab].tab[i].kind==Tokenkind::TEMP)
		{
		    tab_set.Tab[Current_Tab].tab[i].addr=addr;
			addr=addr+4;
		}
		else
		{
			addr=tab_set.Tab[Current_Tab].tab[i].addr+4*tab_set.Tab[Current_Tab].tab[i].size;
		}
	}
}
void Parser::gensize()
{
	//cout<<"enter gensize"<<endl;
	int i,j;
	int size,num;
	num=0;
	//cout<<tab_set.Tab.size()<<endl;
	for(i=0;i<tab_set.Tab.size();i++)
	{
		//cout<<tab_set.Tab[i].tab.size()<<endl;
		for(j=0,num=0;j<tab_set.Tab[i].tab.size()&&tab_set.Tab[i].tab[j].kind!=Tokenkind::FUNC;j++)
		{
			if(tab_set.Tab[i].tab[j].kind==Tokenkind::FPA)
			{
				num++;
			}
			size=tab_set.Tab[i].tab[j].addr;
		}
		if(j>=1)
		{
			if(tab_set.Tab[i].tab[j-1].kind==Tokenkind::ARRAY)
			{
				size=size+tab_set.Tab[i].tab[j-1].size*4;
			}
			else
			{
				size=size+4;
			}
		}
		else
		{
			size=0;
			num=0;
		}
		tab_set.Tab[i].size=size;
		tab_set.Tab[i].noforpar=num;
	}
	//cout<<"leave gensize"<<endl;
}
