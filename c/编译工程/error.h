#ifndef __ERROR_H__
#define __ERROR_H__

#include <string>
#include <string.h>
#include "Lexer.h"
#include "parser.h"
#include "Symbol.h"

using namespace std;

class Error
{
private:
public:
	int errornum;
	Error();
	string msg[100]; //��¼������Ϣ������
	string errfunc[50];
	bool is_success();
	void adderrormsg(int efnum,int emnum);
	void printferrmsg(int efnum,int emnum,int line);
	void initial();
};
#endif