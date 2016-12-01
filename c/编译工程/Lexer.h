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
        int linenum;       //�м���
        char buf[MAXLINE];
		char word[MAXWORD];
		int lastword,curword;
        char temp;      //��¼��ǰ������ַ�
    public:
		string keysym[100];
		string identsym;
		int arraysize;  //��¼�������͵Ĵ�С
		char constch;
		string constr;
		map<string,Symbol> rsdmap;
		int num;
        Lexer();
        void getch();
        void getsym();
		void nextsym();
		void initial();
		void skip();     //�������÷���
		int addop(char c);     //�ӷ������
		int mulop(char c);     //�˷������
		int alpha(char c);     //��ĸ
		int nzdigit(char c);   //��������
		int digit(char c);     // ����
		void ident();      //��ʶ��
		void usint();      //�޷�������
		void sint();      //����
		void opsym();     //
		void test();
};

#endif