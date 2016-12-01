#include <sstream>
#include "quater.h"

extern tableManager tab_set;
extern ofstream midcodefile;
string Operand::tostring() 
{
	stringstream ss;
	switch (opd_type) 
	{
	case OPCONST:
		ss << value.cont;
		break;

	case OPIDENT:
		ss << tab_set.find(tabx, value.ident).name;
		break;

	case OPTEMP:
		ss << tab_set.find(tabx, value.temp).name;
		break;

	case OPLABEL:
		if (value.label == 0) {
			ss << "Main";
		}
		else 
		{
			ss << "Label_" << value.label;
		}
		break;

	case OPSTRING:
		//ss << "\"" << table.findString(value.str) << "\"";
		break;
	}
	return ss.str();
}
QuaterInstr::QuaterInstr(){}
QuaterInstr::QuaterInstr(int lev,int current_tab,Opcode opt,Operand dest,Operand src1,Operand src2)
{
	this->lev=lev;
	this->current_tab=current_tab;
	this->op=opt;
	this->des=dest;
	this->src1=src1;
	this->src2=src2;
}
void QCode::printf_Qcode()
{
	//cout<<"enter printf_Qcode()"<<endl;
	int i=0;
	stringstream ss;
	midcodefile<<Qcode.size()<<endl;
	for(i=0;i<Qcode.size();i++)
	{
		switch (Qcode[i].op)
		{
		case Opcode::ADD:
			midcodefile<<setw(10)<<" ADD "; 
			midcodefile<<setw(10)<<Qcode[i].des.opd_name;
			if(Qcode[i].src1.opd_type==Opdtype::OPIDENT||Qcode[i].src1.opd_type==Opdtype::OPTEMP)
				midcodefile<<setw(10)<<Qcode[i].src1.opd_name;
			else if(Qcode[i].src1.opd_type==Opdtype::OPCONST)
				midcodefile<<setw(10)<<Qcode[i].src1.value.cont;
			if(Qcode[i].src2.opd_type==Opdtype::OPIDENT||Qcode[i].src2.opd_type==Opdtype::OPTEMP)
				midcodefile<<setw(10)<<Qcode[i].src2.opd_name<<endl;
			else if(Qcode[i].src2.opd_type==Opdtype::OPCONST)
				midcodefile<<setw(10)<<Qcode[i].src2.value.cont<<endl;
			break;	
		case Opcode::MUL:
			midcodefile<<setw(10)<<" MUL "; 
			midcodefile<<setw(10)<<Qcode[i].des.opd_name;
			if(Qcode[i].src1.opd_type==Opdtype::OPIDENT||Qcode[i].src1.opd_type==Opdtype::OPTEMP)
				midcodefile<<setw(10)<<Qcode[i].src1.opd_name;
			else if(Qcode[i].src1.opd_type==Opdtype::OPCONST)
				midcodefile<<setw(10)<<Qcode[i].src1.value.cont;
			if(Qcode[i].src2.opd_type==Opdtype::OPIDENT||Qcode[i].src2.opd_type==Opdtype::OPTEMP)
				midcodefile<<setw(10)<<Qcode[i].src2.opd_name<<endl;
			else if(Qcode[i].src2.opd_type==Opdtype::OPCONST)
				midcodefile<<setw(10)<<Qcode[i].src2.value.cont<<endl;
			break;	
		case Opcode::SUB:
			midcodefile<<setw(10)<<" SUB "; 
			midcodefile<<setw(10)<<Qcode[i].des.opd_name;
			if(Qcode[i].src1.opd_type==Opdtype::OPIDENT||Qcode[i].src1.opd_type==Opdtype::OPTEMP)
				midcodefile<<setw(10)<<Qcode[i].src1.opd_name;
			else if(Qcode[i].src1.opd_type==Opdtype::OPCONST)
				midcodefile<<setw(10)<<Qcode[i].src1.value.cont;
			if(Qcode[i].src2.opd_type==Opdtype::OPIDENT||Qcode[i].src2.opd_type==Opdtype::OPTEMP)
				midcodefile<<setw(10)<<Qcode[i].src2.opd_name<<endl;
			else if(Qcode[i].src2.opd_type==Opdtype::OPCONST)
				midcodefile<<setw(10)<<Qcode[i].src2.value.cont<<endl;
			break;	
		case Opcode::DIV:
			midcodefile<<setw(10)<<" DIV "; 
			midcodefile<<setw(10)<<Qcode[i].des.opd_name;
			if(Qcode[i].src1.opd_type==Opdtype::OPIDENT||Qcode[i].src1.opd_type==Opdtype::OPTEMP)
				midcodefile<<setw(10)<<Qcode[i].src1.opd_name;
			else if(Qcode[i].src1.opd_type==Opdtype::OPCONST)
				midcodefile<<setw(10)<<Qcode[i].src1.value.cont;
			if(Qcode[i].src2.opd_type==Opdtype::OPIDENT||Qcode[i].src2.opd_type==Opdtype::OPTEMP)
				midcodefile<<setw(10)<<Qcode[i].src2.opd_name<<endl;
			else if(Qcode[i].src2.opd_type==Opdtype::OPCONST)
				midcodefile<<setw(10)<<Qcode[i].src2.value.cont<<endl;
			break;	
		case Opcode::SCF:
			midcodefile<<setw(10)<<" SCF "; 
			midcodefile<<setw(10)<<Qcode[i].des.opd_name;
			midcodefile<<endl;
			break;	
		case Opcode::PRF:
			midcodefile<<setw(10)<<" PRF "; 
			if(Qcode[i].des.opd_type==Opdtype::OPSTRING)
				midcodefile<<setw(20)<<Qcode[i].des.str;
			else if(Qcode[i].des.opd_type==Opdtype::OPCONST)
				midcodefile<<setw(10)<<Qcode[i].des.value.cont;
			else if(Qcode[i].des.opd_type==Opdtype::OPIDENT||
				Qcode[i].des.opd_type==Opdtype::OPTEMP)
				midcodefile<<setw(10)<<Qcode[i].des.opd_name;
			midcodefile<<endl;
			break;	
		case Opcode::RET:
			midcodefile<<setw(10)<<" RET "; 
			if(Qcode[i].des.opd_type==Opdtype::OPCONST)
				midcodefile<<setw(10)<<Qcode[i].des.value.cont;
			else if(Qcode[i].des.opd_type==Opdtype::OPIDENT||
			    Qcode[i].des.opd_type==Opdtype::OPTEMP)
			    midcodefile<<setw(10)<<Qcode[i].des.opd_name;
			midcodefile<<endl;
			break;	
		case Opcode::PUSH:
			midcodefile<<setw(10)<<" PUSH "; 
			if(Qcode[i].des.opd_type==Opdtype::OPIDENT||Qcode[i].des.opd_type==Opdtype::OPTEMP)
			{
				midcodefile<<setw(10)<<Qcode[i].des.opd_name;
			}
			else if(Qcode[i].des.opd_type==Opdtype::OPCONST)
				midcodefile<<setw(10)<<Qcode[i].des.value.cont;
			midcodefile<<setw(10)<<Qcode[i].src1.opd_name<<endl;
			break;
		case Opcode::CAL:
			midcodefile<<setw(10)<<" CAL "; 
			midcodefile<<setw(10)<<Qcode[i].des.opd_name;
			midcodefile<<setw(10)<<Qcode[i].src1.opd_name<<endl;
			break;
		case Opcode::SAVE:
			midcodefile<<setw(10)<<" SAVE "<<endl; 
			break;
		case Opcode::ASS:
			midcodefile<<setw(10)<<" ASS "; 
			midcodefile<<setw(10)<<Qcode[i].des.opd_name;
			if(Qcode[i].src1.opd_type==Opdtype::OPIDENT||Qcode[i].src1.opd_type==Opdtype::OPTEMP)
				midcodefile<<setw(10)<<Qcode[i].src1.opd_name<<endl;
			else if(Qcode[i].src1.opd_type==Opdtype::OPCONST)
				midcodefile<<setw(10)<<Qcode[i].src1.value.cont<<endl;
			break;
		case Opcode::LAB:
			//midcodefile<<setw(10)<<" LAB "; 
			if(Qcode[i].des.t_kind==Tokenkind::FUNC)
			{
				ss<<Qcode[i].des.opd_name<<":";
			}
			else
			{
				ss<<"[LABEL_"<<Qcode[i].des.value.label<<"]:";
			}
			midcodefile<<setw(10)<<ss.str()<<endl;
			ss.str("");
			break;
		case Opcode::BEQ:
		case Opcode::BGE:
		case Opcode::BGT:
		case Opcode::BLE:
		case Opcode::BLT:
		case Opcode::BNE:
			switch (Qcode[i].op)
			{
				case Opcode::BEQ:
					midcodefile<<setw(10)<<" LAB ";
					break;
				case Opcode::BGE:
					midcodefile<<setw(10)<<" BGE ";
					break;
				case Opcode::BGT:
					midcodefile<<setw(10)<<" BGT ";
					break;
				case Opcode::BLE:
					midcodefile<<setw(10)<<" BLE ";
					break;
				case Opcode::BLT:
					midcodefile<<setw(10)<<" BLT ";
					break;
				case Opcode::BNE:
					midcodefile<<setw(10)<<" BNE ";
					break;
			}
			ss<<"[LABEL_"<<Qcode[i].des.value.label<<"]";
			midcodefile<<setw(10)<<ss.str();
			ss.str("");
			if(Qcode[i].src1.opd_type==Opdtype::OPIDENT||Qcode[i].src1.opd_type==Opdtype::OPTEMP)
				midcodefile<<setw(10)<<Qcode[i].src1.opd_name;
			else if(Qcode[i].src1.opd_type==Opdtype::OPCONST)
				midcodefile<<setw(10)<<Qcode[i].src1.value.cont;
			if(Qcode[i].src2.opd_type==Opdtype::OPIDENT||Qcode[i].src2.opd_type==Opdtype::OPTEMP)
				midcodefile<<setw(10)<<Qcode[i].src2.opd_name<<endl;
			else if(Qcode[i].src2.opd_type==Opdtype::OPCONST)
				midcodefile<<setw(10)<<Qcode[i].src2.value.cont<<endl;
			break;
		case Opcode::NEG:
			midcodefile<<setw(10)<<" NEG ";
			midcodefile<<setw(10)<<Qcode[i].des.opd_name;
			if(Qcode[i].src1.opd_type==Opdtype::OPIDENT||Qcode[i].src1.opd_type==Opdtype::OPTEMP)
				midcodefile<<setw(10)<<Qcode[i].src1.opd_name<<endl;
			else if(Qcode[i].src1.opd_type==Opdtype::OPCONST)
				midcodefile<<setw(10)<<Qcode[i].src1.value.cont<<endl;
			break;
		case Opcode::JMP:
			ss<<"[LABEL_"<<Qcode[i].des.value.label<<"]";
			midcodefile<<setw(10)<<" JMP "; 
			midcodefile<<setw(10)<<ss.str()<<endl;
			ss.str("");
			break;
		case Opcode::SEL:
			midcodefile<<setw(10)<<" SEL "; 
			midcodefile<<setw(10)<<Qcode[i].des.opd_name<<setw(10)<<Qcode[i].src1.opd_name;
			if(Qcode[i].src2.opd_type==Opdtype::OPIDENT||Qcode[i].src2.opd_type==Opdtype::OPTEMP)
				midcodefile<<setw(10)<<Qcode[i].src2.opd_name<<endl;
			else if(Qcode[i].src2.opd_type==Opdtype::OPCONST)
				midcodefile<<setw(10)<<Qcode[i].src2.value.cont<<endl;
			break;
		case Opcode::OFF:
			midcodefile<<setw(10)<<" OFF "; 
			midcodefile<<setw(10)<<Qcode[i].des.opd_name<<setw(10)<<Qcode[i].src1.opd_name;
			if(Qcode[i].src2.opd_type==Opdtype::OPIDENT||Qcode[i].src2.opd_type==Opdtype::OPTEMP)
				midcodefile<<setw(10)<<Qcode[i].src2.opd_name<<endl;
			else if(Qcode[i].src2.opd_type==Opdtype::OPCONST)
				midcodefile<<setw(10)<<Qcode[i].src2.value.cont<<endl;
			break;
		}
	}
	//cout<<"leave printf_Qcode()"<<endl;
}