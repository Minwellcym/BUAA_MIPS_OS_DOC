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



extern int lev;  //程序层次
extern int tempcount;
extern Symbol sym;  //当前的单词类型
extern Lexer lexer;
extern Error error;
extern QCode midCode;
extern int lc;

extern tabItem temp_item;
extern ofstream mipscodefile;
extern ofstream midcodefile;

extern table toptable;        //顶层符号表
extern tableManager tab_set;  //符号表管理
extern int errornum;
int labelcount=0;

void Parser::initial()  //初始化一些头符号集
{
	procbegsys.insert(constsym);
	procbegsys.insert(intsym);
	procbegsys.insert(charsym);
	procbegsys.insert(voidsym);

	constDecbegsys.insert(constsym);  //进入constDec 的条件
	constDecendsys.insert(semicolon);
	constDefbegsys.insert(intsym);    //进入constDef 的条件
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
	while(procbegsys.find(sym)!=procbegsys.end()) //当始终是合法头符号集
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
void Parser::constDec() //  ＜常量说明＞   ::=  const＜常量定义＞;{ const＜常量定义＞;}
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
//入口 const
//出口 ;后的合法字符
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
				temp_item.name=lexer.identsym; //暂时存起来当前读入的标识符单词
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
// 入口 int / char 
// 出口 
void Parser::var_funcDec()
{
	Symbol tsym=sym;
	lexer.nextsym();
	if(sym==ident) //必须是标志符
	{
		string word=lexer.identsym;
		lexer.nextsym();//标志符后面分成三类
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
		else if(sym==lparent) //证明是有返回值的函数说明
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
// 入口 void 
// 出口 
void Parser::voidDec()
{
	lexer.nextsym();
	if(sym==ident) //此时是无返回值的函数
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
//＜常量定义＞   ::=   int＜标识符＞＝＜整数＞{,＜标识符＞＝＜整数＞}| char＜标识符＞＝＜字符＞{,＜标识符＞＝＜字符＞}
//          ＜整数＞        ::= ［＋｜－］＜无符号整数＞｜０

//＜变量定义＞   ::= ＜类型标识符＞(＜标识符＞|＜标识符＞‘[’＜无符号整数＞‘]’){,(＜标识符＞|＜标识符＞‘[’＜无符号整数＞‘]’) }

// 入口 sym==comma || sym==semicolon || sym==lbrack
// 出口 
void Parser::varDef(string word,Symbol tsym) //tsym 保存的是这些常量的类型
{
	//cout<<"enter varDef"<<endl;
	if(tsym==intsym)
		temp_item.type=inttyp;
	else 
		temp_item.type=chartyp;
	if(sym==lbrack)  //左中括号证明是数组
	{
		lexer.nextsym();//读完左中括号后，应该读入无符号整数
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
			if(sym==lbrack) //证明是数组
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
			else //不是数组的话，就是普通常量
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
//＜有返回值函数定义＞  ::=  ＜声明头部＞‘(’＜参数＞‘)’ ‘{’＜复合语句＞‘}’

// 入口 (
// 出口 
void Parser::rtvaluefuncDec(string word,Symbol tsym) //word存入的函数名，tsym是函数的返回值类型
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
		lexer.nextsym();  //现在读入的应该是参量表的头字符集，或者右括号
		if(sym==intsym||sym==charsym) //读入类型标志符时，进入参数列表处理
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
			if(sym==intsym||sym==charsym) //读入类型标志符时，进入参数列表处理
			{
				parameterList();
			}
		}
		if(sym==rparent)  //读入右括号，证明无参数列表,或者参数列表处理完
		{
			lexer.nextsym();  //当前读入的字符应该是大括号
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
			}//不是大括号报错
			else
			{
				error.adderrormsg(8,10);
			}
			tab_set.exit_func(tab_set.pCurrent_tab);
			cout<<"This is a no return function!"<<endl;
		}
	}
}
//＜参数表＞     ::=  ＜类型标识符＞＜标识符＞{,＜类型标识符＞＜标识符＞}| ＜空＞
// 入口为  int /char 
//出口为  )
void Parser::parameterList()
{
	Symbol tsym; //用于保存当前的参数类型
	if(sym==intsym)
		temp_item.type=inttyp;
	else
		temp_item.type=chartyp;
	tsym=sym;//记录当前参数的类型
	lexer.nextsym();//现在读入的应该是标识符了
	if(sym==ident)
	{
		temp_item.name=lexer.identsym;
		tab_set.enterFPA(temp_item.type,temp_item.name,tab_set.pCurrent_tab);
		lexer.nextsym();        //现在读入的应该是逗号或者右括号
		if(sym==comma||sym==rparent)
		{
			while(sym==comma)
			{
				lexer.nextsym();
				if(sym==intsym||sym==charsym) //参数列表逗号后面应该是类型标志符
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
//＜主函数＞ ::=  void main‘(’‘)’ ‘{’＜复合语句＞‘}’
// 入口为  main 
//出口为 
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
				}//不是右大括号报错
				else
				{
					error.adderrormsg(10,11);
				}
			}//不是左大括号报错
			else
			{
				error.adderrormsg(10,10);
			}
		}//不是右括号报错
		else
		{
			error.adderrormsg(10,13);
		}
	}//不是左括号报错
	else
	{
		error.adderrormsg(10,12);
	}
	mainfuncflag=true;
	gensize();
	cout<<"leave Mainfunc()"<<endl;
}
//＜复合语句＞::=［＜常量说明＞］［＜变量说明＞］＜语句列＞
//复合语句的出口部分应该是  语句列的出口  下一个语句的开头 ，或者是 }
//读入大括号，进入复合语句
void Parser::compoundStatement()     
{
	cout<<"start compoundStatement "<<endl;
	compoundStatementflag=true;
	lexer.nextsym();  //大括号后面可能是 const int char 语句列
	if(sym==constsym)
	{
		constDec();
	}
	if(sym==intsym||sym==charsym)
	{
		newvarDec();
	}
	completeaddr(tab_set.pCurrent_tab);
	if(statbegsys.find(sym)!=statbegsys.end()) //如果有语句
	{
		Statementcolumn();
	}
	else if(sym==rbrace)                    //语句列中没语句
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
//＜变量说明＞   ::= ＜变量定义＞;{＜变量定义＞;}
//＜变量定义＞   ::= ＜类型标识符＞(＜标识符＞|＜标识符＞‘[’＜无符号整数＞‘]’){,(＜标识符＞|＜标识符＞‘[’＜无符号整数＞‘]’) }
//入口  int char
//出口   }
void Parser::newvarDec()
{
	while(sym==intsym||sym==charsym)
	{
		newvarDef();
	}
}
//入口  int char
//出口 
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
//＜语句＞       ::= ＜条件语句＞｜＜循环语句＞｜‘{’＜语句列＞‘}’｜＜有返回值函数调用语句＞; 
//                   |＜无返回值函数调用语句＞;｜＜赋值语句＞;｜＜读语句＞;｜＜写语句＞;｜＜空＞;｜＜返回语句＞;
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
	else if(sym==ident)  //可能是赋值语句，或者函数调用语句
	{
		string name=lexer.identsym;  //记录一下赋值的对象
		lexer.nextsym();
		if(sym==becomes||sym==lbrack)//是赋值语句
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
		else if(sym==lparent)    //调用函数
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
	else if(sym==scanfsym) //进入读入语句的处理
	{
		scanfStatement();
	}
	else if(sym==printfsym)  //进入输出语句处理
	{
		printfStatement();
	}
	else if(sym==returnsym)  //返回语句
	{
		returnStatement();
	}
	else if(sym==lbrace)//进入语句列判断
	{
		lexer.nextsym();
		if(statbegsys.find(sym)!=statbegsys.end()) //语句列非空
		{
			Statementcolumn();
		}
		if(sym==rbrace)             //语句列为空
		{
			lexer.nextsym();
		}
		else
		{
			error.adderrormsg(12,10);
		}
		return ;
	}
	else if(sym==semicolon) //直接是分号，空语句，直接处理结束
	{
		return ;
	}
}
//＜语句列＞     ::=｛＜语句＞｝ 我们只考虑语句列中有语句的情况 
//语句 列的出口 ，是语句的出口
// 进入语句列之前，已经进行了 入口合法性检查测
//语句列的出口 是  不是语句列的首字符  
void Parser::Statementcolumn()
{
	while(statbegsys.find(sym)!=statbegsys.end())
	{
		Statement();
	}
}
//＜条件语句＞   ::=  if ‘(’＜条件＞‘)’＜语句＞［else＜语句＞］
//＜条件＞   ::=  ＜表达式＞＜关系运算符＞＜表达式＞｜＜表达式＞ //表达式为0条件为假，否则为真
// 入口 ：if
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
//＜条件＞   ::=  ＜表达式＞＜关系运算符＞＜表达式＞｜＜表达式＞ //表达式为0条件为假，否则为真
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
//＜循环语句＞   ::=  do＜语句＞while ‘(’＜条件＞‘)’
   //                 |for‘(’＜标识符＞＝＜表达式＞;＜条件＞;＜标识符＞＝＜标识符＞(+|-)＜步长＞‘)’＜语句＞
//入口 ： dosym
//出口 ： } /statbegsys
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
//入口 ： (
//出口 ： ;的下一个符号
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
//＜无返回值函数调用语句＞ ::= ＜标识符＞‘(’＜值参数表＞‘)’
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
//＜值参数表＞   ::= ＜表达式＞{,＜表达式＞}｜＜空＞
// 入口  ： 合法的表达式开头
//出口   ： 参数列表后一个
// 可能出现的错误：  参数类型不匹配 ，参数个数不匹配，
void Parser::valueparameterTable(tabItem func,Operand& des)
{
	cout<<"start valueparameterTable"<<endl;
	tabItem par_temp;
	int i=0; //这个函数的参数的指针
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
//＜赋值语句＞::=＜标识符＞＝＜表达式＞|＜标识符＞‘[’＜表达式＞‘]’=＜表达式＞
//出口  分号后 一个
// 入口 标志符后一个
void Parser::assignStatement(tabItem var_ident,Operand& des)
{
	cout<<"start assignstatement"<<endl;
	Operand src,opd1,opd2;
	if(sym==lbrack)//赋值的对象是数组元素
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
//＜写语句＞::= printf‘(’＜字符串＞,＜表达式＞‘)’|printf ‘(’＜字符串＞‘)’|printf ‘(’＜表达式＞‘)’
//出口 ： 下一个合法开头，或者 }
//入口 ： printf 
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
//＜读语句＞::=scanf ‘(’＜标识符＞{,＜标识符＞}‘)’
//返回时 的当前字符应该是 右大括号，或者 下一个合法的语句开头
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
//＜返回语句＞   ::=  return[‘(’＜表达式＞‘)’]
//返回时 的当前字符应该是 右大括号，或者 下一个合法的语句开头
// 入口 ： return 
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
//＜表达式＞::= ［＋｜－］＜项＞{＜加法运算符＞＜项＞}
//处理完表达式后，当前的sym应该是  ) , ; , ] ,
//进入expression 之前 已经检查是否是当前字符是否在 它的合法否符号集
//离开expression 时，读到的表达式后面的一个字符；
void Parser::expression(Operand& src1)
{
	midcodefile<<"进入表达式处理"<<endl;
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
					//如果两个都是常量，就不用生成中间变量了，也不用生成代码，直接转化成常量即可
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
	midcodefile<<"离开表达式处理"<<endl;
}
//＜项＞::= ＜因子＞{＜乘法运算符＞＜因子＞}
//再进入 项处理之前，先检测当前的字符是否合法
//离开  项处理时， 读到的是 因子后面的一个字符
void Parser::term(Operand& src1)
{
	midcodefile<<"进入项处理"<<endl;
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
					//如果两个都是常量，就不用生成中间变量了
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
	midcodefile<<"离开项处理"<<endl;
}
//＜因子＞ ::= ＜标识符＞｜＜标识符＞‘[’＜表达式＞‘]’｜
//        ＜整数＞|＜字符＞｜＜有返回值函数调用语句＞|‘(’＜表达式＞‘)’
// 进入因子处理之前，先判断是否合法
// 离开因子处理   读到的是因子后面一个字符
void Parser::factor(Operand& des)
{
	midcodefile<<"进入因子处理"<<endl;
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
	midcodefile<<"离开因子处理"<<endl;
}
// 入口 ：ident
// 出口 ： 项后面的一个字符
//这一部分，对于因子是函数调用，或者是变量（变量或者数组变量）
void Parser::factorpart1(Operand& des)
{
	Operand ident;
	Symbol tsym=sym;      //先记录当前的变量的类型和名字
	string name=lexer.identsym;
	tabItem C_item;
	C_item=*tab_set.find_item(tab_set.pCurrent_tab,name);

	lexer.nextsym();

	des.opd_name=C_item.name;
	des.t_typ=C_item.type;
	des.t_kind=C_item.kind;
	des.offset=C_item.addr;
	des.tabx=tab_set.pCurrent_tab;
	
	if(sym==lbrack)  //读到左中括号，应该是数组类型
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
	else if(sym==lparent)  // 读到左括号，应该是有返回值函数类型   ＜有返回值函数调用语句＞ ::= ＜标识符＞‘(’＜值参数表＞‘)’＜值参数表＞   ::= ＜表达式＞{,＜表达式＞}｜＜空＞
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
//这一部分是对于因子是整数 ，做出处理
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
//递归调用 表达式处理 处理因子是表达式的情况
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
//在声明部分结束后，对声明部分的地址进行分配
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
//在处理完语句后，对于生成的中间临时变量进行地址分配
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
