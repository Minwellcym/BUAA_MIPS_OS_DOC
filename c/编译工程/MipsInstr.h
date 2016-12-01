#ifndef __MIPSINSTR_H__
#define __MIPSINSTR_H__

#include <string>
#include "midCode.h"
#include "parser.h"
#include "symboltable.h"
#include "quater.h"
#include "Symbol.h"
using namespace std;
extern ofstream mipscodefile;


enum MipsCode {
	//load to the register
	lw,
	//store to the memory
	sw,
	//load the immdiate number.
	li,
	//load address
	la,
	//jump and link
	jal,
	//
	jr,
	//jumpd
	j,
	//add the ident
	add,
	addi,
	//sub
	sub,
	subi,
	//mult
	mul,
	//dive
	ddiv,
	//assign
	move,
	//branch
	bge,
	ble,
	bgt,
	blt,
	beq,
	bne,
	syscall,
	label,
	mflo,
	note
};
class Instruction 
{
private:
public:
	string op;
	string des;
	string src1;
	string src2;
	Instruction(MipsCode _op) 
	{
		op = getOpName(_op);
	}
	Instruction(MipsCode _op, string _des) 
	{
		op = getOpName(_op);
		des = _des;
	}
	Instruction(MipsCode _op, string _des, string _src1) 
	{
		op = getOpName(_op);
		des = _des;
		src1 = _src1;
	}
	Instruction(MipsCode _op, string _des, string _src1, string _src2)
	{
		op = getOpName(_op);
		des = _des;
		src1 = _src1;
		src2 = _src2;
	}
	string getOpName(MipsCode _opcode) 
	{
		switch (_opcode)
		{
		case lw:
			return "lw"; break;
		case sw:
			return "sw"; break;
		case li:
			return "li"; break;
		case la:
			return "la"; break;
		case jal:
			return "jal"; break;
		case jr:
			return "jr"; break;
		case add:
			return "add"; break;
		case addi:
			return "addi"; break;
		case sub:
			return "sub"; break;
		case subi:
			return "subi"; break;
		case mul:
			return "mul"; break;
		case MipsCode::ddiv:
			return "div"; break;
		case MipsCode::move:
			return "move"; break;
		case mflo:
			return "mflo"; break;
		case bge:
			return "bge"; break;
		case ble:
			return "ble"; break;
		case bgt:
			return "bgt"; break;
		case blt:
			return "blt"; break;
		case beq:
			return "beq"; break;
		case bne:
			return "bne"; break;
		case syscall:
			return "syscall"; break;
		case label:
			return ""; break;
		case j:
			return "j"; break;
		case note:
			return "#"; break;
		default:
			break;
		}
	}
	void printInstr() 
	{
		mipscodefile<<op<<des<<src1<<src2<<endl;
	}
};
class Dataseg 
{
private:
public:
	string label;
	string conststring;
	string setData() 
	{
		return label + ":" + "\t.asciiz \"" +conststring+"\"";
	}
	Dataseg(string _label, string _conststring)
	{
		label = _label;
		conststring = _conststring;
	}
};
class MipsInstr 
{
private:
public:
	void add(MipsCode _op) 
	{
		object_insrtuctions.push_back(Instruction(_op));
	}
	void add(MipsCode _op, string _des) 
	{
		object_insrtuctions.push_back(Instruction(_op," "+_des));
	}
	void add(MipsCode _op, string _des, string _src1) 
	{
		object_insrtuctions.push_back(Instruction(_op, " "+_des, ","+_src1));
	}
	void add(MipsCode _op, string _des, string _src1, string _src2) 
	{
		if (_op == MipsCode::sw || _op == MipsCode::lw)
			object_insrtuctions.push_back(Instruction(_op, " " + _des, "," + _src1, "(" + _src2 + ")"));
		else
			object_insrtuctions.push_back(Instruction(_op, " "+_des, ","+_src1, ","+_src2));
	}
	void addData(string _name, string _conststring) 
	{
		object_datas.push_back(Dataseg(_name, _conststring));
	}
	void translate();
	vector<Instruction> object_insrtuctions;
	vector<Dataseg> object_datas;
	void Handlequater(QuaterInstr cqt);

	void Handlescanf(QuaterInstr cqt);
	void Handleprintf(QuaterInstr cqt);

	void Handlecalc(QuaterInstr cqt);
	void Handleneg(QuaterInstr cqt);

	void Handlebranch(QuaterInstr cqt);
	void Handlejump(QuaterInstr cqt);
	void Handlereturn(QuaterInstr cqt);

	void Handleass(QuaterInstr cqt);

	void Handlearr(QuaterInstr cqt);

	void Handlelabel(QuaterInstr cqt);
	void Handlepush(QuaterInstr cqt);
	void Handlesave(QuaterInstr cqt);
	void Handlecall(QuaterInstr cqt);

	string genString();
	void loadReg(Operand opd,string reg);
	string loadaddr(Operand opd);
	void storeMemory(string reg,Operand des);
};
#endif