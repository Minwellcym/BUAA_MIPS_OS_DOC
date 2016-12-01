#include <iostream>
#include <fstream>
#include <sstream>
#include <cctype>
#include <ctype.h>
#include <string.h>
#include<iomanip>
#include "Symbol.h"
#include "Lexer.h"
#include "error.h"
#include "parser.h"
#include "quater.h"


using namespace std;

extern int lc;  //行计数器
extern Symbol sym;  //当前的单词类型
extern fstream infile;
extern ofstream midcodefile;

Lexer::Lexer()
{
    ll=strlen(buf);
    cc=0;
	temp=' ';
	lastword=0;
	curword=0;
	linenum=0;
	initial();
	num=0;
}
void Lexer::getch()
{
    if(cc==ll)
    {
		lc++;
        if(infile.eof())
        {
            temp= EOF;
            return ;
        }
        infile.getline(buf,1024);
        ll=strlen(buf);
        cc=0;
        temp=' ';
    }
    else if(cc==0)
    {
         linenum++;
         cc++;
         temp=buf[cc-1];
    }
    else
    {
        cc++;
        temp=buf[cc-1];
    }
	//cout<<temp;
}
void Lexer::initial()
{
	//保留字和符号对应的输出字符串
	keysym[0]="forsym";
	keysym[1]="whilesym";
	keysym[2]="dosym";
	keysym[3]="ifsym";
	keysym[4]="elsesym";
	keysym[5]="switchsym";
	keysym[6]="casesym";
	keysym[7]="defaultsym";
	keysym[8]="intsym";
	keysym[9]="charsym";
	keysym[10]="constsym";
	keysym[11]="printfsym";
	keysym[12]="scanfsym";
	keysym[13]="voidsym";
	keysym[14]="mainsym";
	keysym[15]="returnsym";
	keysym[16]="plus";
	keysym[17]="minus";
	keysym[18]="times";
	keysym[19]="divid";
	keysym[20]="eql";
	keysym[21]="neq";
	keysym[22]="gtr";
	keysym[23]="lss";
	keysym[24]="leq";
	keysym[25]="geq";
	keysym[26]="lparent";
	keysym[27]="rparent";
	keysym[28]="lbrack";
	keysym[29]="rbrack";
	keysym[30]="lbrace";
	keysym[31]="rbrace";
	keysym[32]="comma";
	keysym[33]="becomes";
	keysym[34]="ident";
	keysym[35]="number";
	keysym[36]="unsgnumber";
	keysym[37]="singlequote";
	keysym[38]="doublequote";
	keysym[39]="chcon";
	keysym[40]="strcon";
	keysym[41]="semicolon";

	//保留字符号集
	rsdmap["for"]=Symbol::forsym;
	rsdmap["while"]=Symbol::whilesym;
	rsdmap["do"]=Symbol::dosym;
	rsdmap["if"]=Symbol::ifsym;
	rsdmap["else"]=Symbol::elsesym;
	rsdmap["switch"]=Symbol::switchsym;
	rsdmap["case"]=Symbol::casesym;
	rsdmap["default"]=Symbol::defaultsym;
	rsdmap["int"]=Symbol::intsym;
	rsdmap["char"]=Symbol::charsym;
	rsdmap["const"]=Symbol::constsym;
	rsdmap["printf"]=Symbol::printfsym;
	rsdmap["scanf"]=Symbol::scanfsym;
	rsdmap["void"]=Symbol::voidsym;
	rsdmap["main"]=Symbol::mainsym;
	rsdmap["return"]=Symbol::returnsym;

	rsdmap["+"]=Symbol::plus;
	rsdmap["-"]=Symbol::minus;
	rsdmap["*"]=Symbol::times;
	rsdmap["/"]=Symbol::divid;
	rsdmap["("]=Symbol::lparent;
	rsdmap[")"]=Symbol::rparent;
	rsdmap["["]=Symbol::lbrack;
	rsdmap["]"]=Symbol::rbrack;
	rsdmap["{"]=Symbol::lbrace;
	rsdmap["}"]=Symbol::rbrace;
	rsdmap[";"]=Symbol::semicolon;
	rsdmap[","]=Symbol::comma;
}
void Lexer::skip()
{
	while((temp==' '||temp=='\t'||temp=='\n'))
		getch();
}
int Lexer::addop(char c)
{
	if(c=='+'||c=='-')
		return 1;
	else
		return 0;
}
int Lexer::mulop(char c)
{
	if(c=='*'||c=='/')
		return 1;
	else 
		return 0;
}
int Lexer::alpha(char c)
{
	if(isalpha(c)||c=='_')
		return 1;
	else
		return 0;
}
int Lexer::digit(char c)
{
	if(isdigit(c))
		return 1;
	else
		return 0;
}
int Lexer::nzdigit(char c)
{
	if(digit(c)&&c!=0)
		return 1;
	else
		return 0;
}
void Lexer::ident()
{
	int i;
	bool isarray=false;
	i=0;
	do
	{
		word[i++]=temp;
		getch();
	}
	while(alpha(temp)||digit(temp));
	word[i]='\0';
	identsym=string(word);
	map<string, Symbol>::const_iterator itr = rsdmap.find(identsym);
	if (itr!= rsdmap.end())
	{
		sym = itr->second;
	}
	else
	{
		sym = Symbol::ident;
	}
}
void Lexer::usint()
{
	int i;
	i=0;
	num=0;
	do
	{
		num=num*10+temp-'0';
		word[i++]=temp;
		getch();
	}
	while(digit(temp));
	word[i]='\0';
	identsym=string(word);
	sym=Symbol::unsgnumber;
}
void Lexer::sint()
{
	int i,k;
	i=1;
	k=0;
	if(temp=='0')
	{
		num=0;
		sym=Symbol::number;
	}
	else
	{
		word[k++]=temp;
		getch();
		if(temp=='-')
			i=-1;
	    if(nzdigit(temp))
	    {
		   num=0;
		   do
		   {
			   num=num*10+temp-'0';
			   word[k++]=temp;
			   getch();
		   }
		   while(digit(temp));
		}
		word[k]='\0';
		identsym=string(word);
		num=num*i;
		sym=Symbol::number;
	}
}
void Lexer::opsym()
{
	if(temp=='<')
	{
		getch();
		if(temp=='=')
		{
			sym=Symbol::leq;
			word[0]='<';
			word[1]=temp;
			word[2]='\0';
			identsym=string(word);
			getch();
		}
		else
		{
			sym=Symbol::lss;
			word[0]='<';
			word[1]='\0';
			identsym=string(word);
		}
	}
	else if(temp=='>')
	{
		getch();
		if(temp=='=')
		{
			sym=Symbol::geq;
			word[0]='>';
			word[1]=temp;
			word[2]='\0';
			identsym=string(word);
			getch();
		}
		else
		{
			sym=Symbol::gtr;
			word[0]='>';
			word[1]='\0';
			identsym=string(word);
		}
	}
	else if(temp=='!')
	{
		getch();
		if(temp=='=')
		{
			sym=Symbol::neq;
			word[0]='!';
			word[1]='=';
			word[2]='\0';
			identsym=string(word);
			getch();
		}
		else
		{
			//,errorcout<<"单独的！"<<endl;
		}
	}
	else if(temp=='=')
	{
		getch();
		if(temp=='=')
		{
			sym=Symbol::eql;
			word[0]='=';
			word[1]='=';
			word[2]='\0';
			identsym=string(word);
			getch();
		}
		else
		{
			sym=Symbol::becomes;
			word[0]='=';
			word[1]='\0';
			identsym=string(word);
		}
	}
}
void Lexer::getsym()
{
	int k;
	k=0;
	skip();
	if(alpha(temp))      // 是字母开头，判断是否是标识符，或者保留字
	{
		ident();
	}
	else if(nzdigit(temp))   //非零数字开头，判断是否是无符号数字
	{
		usint();
	}
	else if(temp=='0')   //如果是正负号开头，或者是0，是数字
	{
		sint();
	}
	else if(temp=='>'||temp=='<'||temp=='!'||temp=='=')
	{                       //判断二元符号
		opsym();
	}
	else if(temp=='\'')         //判断单引号，和字符
	{
		sym=Symbol::chcon;
		k=0;
		getch();
		while(temp!='\'')
		{
			if(addop(temp)||mulop(temp)||alpha(temp)||digit(temp))
			{
				word[k++]=temp;
			}
			else
			{
				cout<<"wrong ch in string!"<<endl;
			}
			getch();
		}
		//while(temp!='\'');//(temp==32||temp==33||(temp>=35&&temp<=126));
		if(k>1)
		{
			//error
		}
		word[1]='\0';
		constch=word[0];
		num=constch;
		identsym=string(word);
		getch();
	}
	else if(temp=='\"')
	{
		sym=Symbol::strcon;
		k=0;
		getch();
		while(temp!='\"')
		{
			if(temp==32||temp==33||(temp>=35&&temp<=126))
			{
				word[k++]=temp;
			}
			else
			{
				cout<<"wrong ch in string!"<<endl;
			}
			getch();
		}
		word[k]='\0';
		identsym=string(word);
		constr=identsym;
		getch();
	}
	else
	{
		word[0]=temp;
		word[1]='\0';
		identsym=string(word);
		map<string, Symbol>::const_iterator itr = rsdmap.find(word);
		if (itr == rsdmap.end())
		{
			cout<<"no such charactor"<<endl;
		}
		else 
		{
			sym = itr->second;	
		}
		getch();
	}
	//cout<<keysym[sym]<<identsym<<endl;
}
void Lexer::nextsym()
{
	if(temp!=EOF)
	{
		getsym();
		midcodefile<<identsym<<" "<<keysym[sym]<<endl;
	}
}
void Lexer::test()
{
	fstream fout;
	fout.open("14231052_result.txt");
	while(temp!=EOF)
	{
		getsym();
		fout<<std::left<<setw(10)<<word<<std::right<<setw(10)<<keysym[sym]<<endl;
		//cout<<word<<' '<<keysym[sym]<<endl;
	}
	fout.close();
}
