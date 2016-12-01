#ifndef __LEXER_H__
#define __LEXER_H__

#define MAXLINE 1024
#define MAXWORD 30

#include <string>
#include "Symbol.h"

using namespace std;
class Lexer 
{
    private:
        int ll,cc;
        int linenum;       //行计数
        char buf[MAXLINE];
		char word[MAXWORD];
		int lastword,curword;
        char temp;      //记录当前读入的字符
    public:
		string keysym[100];
		string identsym;
		int arraysize;  //记录数组类型的大小
		char constch;
		string constr;
		map<string,Symbol> rsdmap;
		int num;
        Lexer();
        void getch();
        void getsym();
		void nextsym();
		void initial();
		void skip();     //跳读无用符号
		int addop(char c);     //加法运算符
		int mulop(char c);     //乘法运算符
		int alpha(char c);     //字母
		int nzdigit(char c);   //非零数字
		int digit(char c);     // 数字
		void ident();      //标识符
		void usint();      //无符号整数
		void sint();      //整数
		void opsym();     //
		void test();
};

#endif