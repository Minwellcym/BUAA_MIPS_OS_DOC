#ifndef __QUATER_H__
#define __QUATER_H__
#include <iostream>
#include <fstream>
#include <string>
#include "symboltable.h"

using namespace std;

extern ofstream midcodefile;

enum Opcode
{
	ADD, SUB, MUL, DIV, NEG,
	BEQ, BNE, BGT, BGE, BLT, BLE,
	JMP, SCF, PRF, ASS, SEL, OFF,
	CAL, RET, VAL, PUSH, SAVE, LAB
};
enum Opdtype
{
	OPCONST, 
	OPIDENT, 
	OPTEMP, 
	OPLABEL, 
	OPSTRING,
};
class Operand
{
private:
public:
	int lev;
	int tabx;
	int offset;
	int retflag;
	Opdtype opd_type;
	Tokentype t_typ;
	Tokenkind t_kind;
	string opd_name;
	string str;
	union value
	{
		int cont;
		int ident;
		int temp;
		int label;
		int str;
	}value;
	Operand()
	{
	}
	string getname();
	string tostring();
};
class QuaterInstr
{
private:
public:
	QuaterInstr();
	QuaterInstr(int lev,int current_tab,Opcode opt,Operand dest=Operand(),Operand src1=Operand(),Operand src2=Operand());
	Opcode op;
	Operand des;
	Operand src1;
	Operand src2;
	int current_tab;
	int lev;
};
class QCode
{
private:
public:
	vector<QuaterInstr> Qcode;
	QCode(){}
	void printf_Qcode();
};





#endif