#include "error.h"
#include "MipsInstr.h"
#include "quater.h"
#include "parser.h"
#include "symboltable.h"


using namespace std;

extern QCode midCode;
extern ofstream mipscodefile;
extern tableManager tab_set;  //符号表管理


string tostring(int value) 
{
	stringstream ss;
	ss << value;
	return ss.str();
}

string MipsInstr::genString() {
	static int count = 1;
	stringstream s;
	s << "_string" << count ;
	count++;
	return s.str();
}
void MipsInstr::translate()
{
	int i;
	int addr=0;
	stringstream ss;
	
	for(i=0;i<midCode.Qcode.size();i++)
	{
		if(midCode.Qcode[i].op==Opcode::PRF&&midCode.Qcode[i].des.opd_type==Opdtype::OPSTRING)
		{
			addr=addr+midCode.Qcode[i].des.str.length();
		}
	}
	if(addr!=0)
	{
		addr=(addr/4+1)*4;
		ss.str("");
		ss<<addr;
		add(MipsCode::li,"$fp",ss.str());
		add(MipsCode::li,"$sp",ss.str());
	}
	else
	{
		ss.str("");
		ss<<addr;
		add(MipsCode::li,"$fp",ss.str());
		add(MipsCode::li,"$sp",ss.str());
	}
	//为全局变量分配空间
	ss.str("");
	ss<<tab_set.Tab[0].size;
	add(MipsCode::addi,"$sp","$sp",ss.str());

	//返回值预留空间  -16($fp)
	add(MipsCode::addi,"$sp","$sp","4");
	
	//存入上层函数的基地址 -12($fp)
	add(MipsCode::sw,"$fp","0($sp)");
	add(MipsCode::addi,"$sp","$sp","4");
	//存入全局变量的基地址  -8($fp)
	add(MipsCode::sw,"$fp","0($sp)");
	add(MipsCode::addi,"$sp","$sp","4");

	//预留返回地址
	add(MipsCode::addi,"$sp","$sp","4");
	//移动fp
	add(MipsCode::addi,"$fp","$sp","0");

	
	tabItem C_item;
	C_item=*tab_set.find_item(0,"_main");
	//为main函数分配空间
	ss.str("");
	ss<<tab_set.Tab[C_item.ptfunctab].size;
	add(MipsCode::addi,"$sp","$sp",ss.str());

	ss.str("");
	ss<<"Label_"<<C_item.addr;
	//add(MipsCode::j,ss.str());
	add(MipsCode::j,"_main");
	//cout<<midCode.Qcode.size()<<endl;
	for(i=0;i<midCode.Qcode.size();i++)
	{
		//cout<<"code  "<<i<<endl;
		Handlequater(midCode.Qcode[i]);
	}
	mipscodefile<<".data"<<endl;
	for(i=0;i<object_datas.size();i++)
	{
		mipscodefile<<object_datas[i].setData()<<endl;
	}
	mipscodefile<<".text"<<endl;
	for(i=0;i<object_insrtuctions.size();i++)
	{
		object_insrtuctions[i].printInstr();
	}
}
void MipsInstr::Handlequater(QuaterInstr cqt)
{
	//cout<<"Handlequater in "<<endl;
	switch (cqt.op)
	{
	case Opcode::ADD:
	case Opcode::SUB:
	case Opcode::MUL:
	case Opcode::DIV:
		Handlecalc(cqt);
		break;
	case Opcode::NEG:
		Handleneg(cqt);
		break;
	case Opcode::SCF:
		Handlescanf(cqt);
		break;
	case Opcode::PRF:
		Handleprintf(cqt);
		break;
	case Opcode::BEQ:
	case Opcode::BNE:
	case Opcode::BGT:
	case Opcode::BGE:
	case Opcode::BLT:
	case Opcode::BLE:
		Handlebranch(cqt);
		break;
	case Opcode::JMP:
		Handlejump(cqt);
		break;
	case Opcode::ASS:
		Handleass(cqt);
		break;
	case Opcode::SEL:
	case Opcode::OFF:
		Handlearr(cqt);
		break ;
	case Opcode::LAB:
		Handlelabel(cqt);
		break;
	case Opcode::PUSH:
		Handlepush(cqt);
		break;
	case Opcode::CAL:
		Handlecall(cqt);
		break;
	case Opcode::SAVE:
		Handlesave(cqt);
		break;
	case Opcode::RET:
		Handlereturn(cqt);
		break;
	}
	return ;
}
void MipsInstr::Handlescanf(QuaterInstr cqt)
{
	//cout<<"Handlescanf in "<<endl;
	add(MipsCode::note,"scanf");
	if(cqt.des.t_typ==Tokentype::inttyp)
	{
		add(MipsCode::li, "$v0", "5");
		add(MipsCode::syscall);
	}
	else if(cqt.des.t_typ==Tokentype::chartyp)
	{
	    add(MipsCode::li, "$v0", "12");
		add(MipsCode::syscall);
	}
	storeMemory("$v0",cqt.des);
	return ;
}
void MipsInstr::Handleprintf(QuaterInstr cqt)
{
	//cout<<"Handlecall in "<<endl;
	add(MipsCode::note, "printf");
	if(cqt.des.opd_type==Opdtype::OPCONST)
	{
		if(cqt.des.t_typ==Tokentype::inttyp)
		{
			add(MipsCode::addi, "$a0","$0", tostring(cqt.des.value.cont));
			add(MipsCode::li, "$v0", "1");
			add(MipsCode::syscall);
		}
		else if(cqt.des.t_typ==Tokentype::chartyp)
		{
			add(MipsCode::addi, "$a0","$0", tostring(cqt.des.value.cont));
			add(MipsCode::li, "$v0", "11");
			add(MipsCode::syscall);
		}
	}
	else if(cqt.des.opd_type==Opdtype::OPSTRING)
	{
		string label_string = genString();
		addData(label_string,cqt.des.str);
		add(MipsCode::la," $a0",label_string);
		add(MipsCode::li," $v0", "4");
		add(MipsCode::syscall);
	}
	else if(cqt.des.opd_type==Opdtype::OPIDENT||
		cqt.des.opd_type==Opdtype::OPTEMP)
	{
		if(cqt.des.t_typ==Tokentype::inttyp)
		{
			loadReg(cqt.des,"$t0");
			add(MipsCode::add, " $a0", "$t0"," $0");
			add(MipsCode::li," $v0", "1");
			add(MipsCode::syscall);
		}
		else if(cqt.des.t_typ==Tokentype::chartyp)
		{
			loadReg(cqt.des,"$t0");
			add(MipsCode::add, "$a0", "$t0","$0");
			add(MipsCode::li, "$v0", "11");
			add(MipsCode::syscall);
		}
	}
}

void MipsInstr::Handlecalc(QuaterInstr cqt)
{
	//cout<<"Handlecalc in "<<endl;
	add(MipsCode::note, "calc"); 
	loadReg(cqt.src1, "$t0");
	loadReg(cqt.src2, "$t1");
	switch (cqt.op)
	{
	case Opcode::ADD:
		add(MipsCode::add, "$s0", "$t0"," $t1");
		break ;
	case Opcode::SUB:
		add(MipsCode::sub, "$s0", "$t0"," $t1");
		break ;
	case Opcode::MUL:
		add(MipsCode::mul, "$s0", "$t0"," $t1");
		break ;
	case Opcode::DIV:
		add(MipsCode::ddiv, "$t0"," $t1");
		add(MipsCode::mflo, "$s0");
		break ;
	}
	storeMemory("$s0",cqt.des);
	//cout<<"Handlecalc out "<<endl;
}
void MipsInstr::Handleneg(QuaterInstr cqt)
{
	add(MipsCode::note, "neg");
	loadReg(cqt.src1, "$t0");
	add(MipsCode::sub," $s0", "$0", "$t0");
	storeMemory("$s0", cqt.des);
}

void MipsInstr::Handlebranch(QuaterInstr cqt)
{
	add(MipsCode::note, "branch");
	loadReg(cqt.src1, "$t0");
	loadReg(cqt.src2, "$t1");
	stringstream ss;
	ss.str("");
	ss<<"Label_"<<cqt.des.value.label;
	cqt.des.opd_name=ss.str();
	switch (cqt.op)
	{
	case Opcode::BEQ:
		add(MipsCode::beq, "$t0", "$t1", cqt.des.opd_name);
		break;
	case Opcode::BNE:
		add(MipsCode::bne, "$t0", "$t1", cqt.des.opd_name);
		break;
	case Opcode::BGT:
		add(MipsCode::bgt, "$t0", "$t1", cqt.des.opd_name);
		break;
	case Opcode::BGE:
		add(MipsCode::bge, "$t0", "$t1", cqt.des.opd_name);
		break;
	case Opcode::BLT:
		add(MipsCode::blt, "$t0", "$t1", cqt.des.opd_name);
		break;
	case Opcode::BLE:
		add(MipsCode::ble, "$t0", "$t1", cqt.des.opd_name);
		break;
	}
}
void MipsInstr::Handlejump(QuaterInstr cqt)
{
	stringstream ss;
	ss.str("");
	ss<<"Label_"<<cqt.des.value.label;
	cqt.des.opd_name=ss.str();
	add(MipsCode::note, "Jump");
	add(MipsCode::j,cqt.des.opd_name);
}

void MipsInstr::Handleass(QuaterInstr cqt)
{
	//cout<<"Handleass in"<<endl;
	add(MipsCode::note, "assign");
	loadReg(cqt.src1,"$t0");
	if(cqt.des.opd_type==Opdtype::OPTEMP)
	{ 
		add(MipsCode::lw,"$t1",loadaddr(cqt.des)); 
		add(MipsCode::sw,"$t0","0($t1)");  
	}
	else
	{
		storeMemory("$t0",cqt.des);
	}
	//cout<<"Handleass out "<<endl;
}
void MipsInstr::Handlearr(QuaterInstr cqt)
{
	add(MipsCode::note, "arrayaddr");
	add(MipsCode::la,"$t1",loadaddr(cqt.src1));  
	loadReg(cqt.src2,"$t2");    
	add(MipsCode::mul,"$t2","$t2","4");     //计算出数组内的偏移量
	add(MipsCode::add,"$t1","$t1","$t2");  //此时t0存入数组这个元素的地址
	if(cqt.op==Opcode::SEL)
	{
		add(MipsCode::lw,"$t1","0($t1)");  //将数组的这个元素的值加载到$t1中
	}
	storeMemory("$t1",cqt.des); //将地址存入了temp里
}

void MipsInstr::Handlelabel(QuaterInstr cqt)
{
	stringstream ss;
	//cout<<cqt.des.opd_name<<"||||||"<<endl;
	if(cqt.des.t_kind!=Tokenkind::FUNC)
	{
		ss.str("");
		ss<<"Label_"<<cqt.des.value.label<<":";
		cqt.des.opd_name=ss.str();
	}
	else
	{
		//cout<<cqt.des.t_kind<<"||||||"<<endl;
		ss.str("");
		ss<<cqt.des.opd_name<<":";
		cqt.des.opd_name=ss.str();
	}
	add(MipsCode::label,cqt.des.opd_name);
}
void MipsInstr::Handlepush(QuaterInstr cqt)
{
	//cout<<"Handlepush in "<<endl;
	add(MipsCode::note, "push");
	stringstream ss;
	loadReg(cqt.des,"$t0");
	ss<<12+cqt.src1.value.cont*4;
	add(MipsCode::sw, "$t0",ss.str(),"$sp");
	//add(MipsCode::addi,"$sp", "$sp", "4");
	//cout<<"Handlepush out "<<endl;
}
void MipsInstr::Handlesave(QuaterInstr cqt)
{
	add(MipsCode::sw,"$ra","-4($fp)");
}
void MipsInstr::Handlecall(QuaterInstr cqt)
{
	//cout<<"Handlecall in "<<endl;
	add(MipsCode::note, "call");
	tabItem C_item;
	stringstream ss;
	int numoffp;
	//cout<<cqt.des.opd_name<<endl;
	//cout<<cqt.des.tabx<<endl;
	C_item=*tab_set.find_item(cqt.des.tabx,cqt.des.opd_name);
	
	//cout<<C_item.ptfunctab<<endl;
	//预留12
	//返回值 -16($fp)
	add(MipsCode::addi, "$sp", "$sp", "4");
	//存入上层函数的基地址，栈指针上移4 -12($fp)
	add(MipsCode::sw, "$fp", "0", "$sp");
	add(MipsCode::addi, "$sp", "$sp", "4");
	//存入全局变量的基地址，栈指针上移4 -8($fp)
	add(MipsCode::lw,"$t0","-8($fp)");
	add(MipsCode::sw, "$t0", "0", "$sp");
	add(MipsCode::addi, "$sp", "$sp", "4");
	//返回地址 -4($fp)
	add(MipsCode::addi, "$sp", "$sp", "4");


	//move $fp
	add(MipsCode::addi,"$fp","$sp","0");

	//ss.str("");
	//numoffp=tab_set.Tab[C_item.ptfunctab].noforpar;
	//ss<<"-"<<numoffp*4;
	//add(MipsCode::addi,"$fp","($sp)",ss.str());
	//为函数的局部变量分配空间
	ss.str("");
	ss<<tab_set.Tab[C_item.ptfunctab].size;
	//cout<<tab_set.Tab[C_item.ptfunctab].size;
	add(MipsCode::addi,"$sp","$sp",ss.str());
	
	add(MipsCode::addi,"$sp","$sp","4");

	//ss.str("");
	//ss<<"Label_"<<C_item.addr;
	//add(MipsCode::jal,ss.str());
	add(MipsCode::jal,cqt.des.opd_name);

	if(C_item.type!=Tokentype::voidtyp)
	{
		storeMemory("$v0",cqt.src1);
	}
	//cout<<"Handlecall out "<<endl;
}
void MipsInstr::Handlereturn(QuaterInstr cqt)
{
	//cout<<"Handleretrun in "<<endl;
	add(MipsCode::note, "return");
	add(MipsCode::lw,"$ra","-4($fp)");
	if(cqt.des.retflag==1)
	{
		//获取返回值
		loadReg(cqt.des,"$t1");
		add(MipsCode::move,"$v0","$t1");
	}
	else
	{
		//获取返回值
		add(MipsCode::lw,"$v0","-16($fp)");
	}
	add(MipsCode::lw,"$t0","-12($fp)");
	//move $sp
	add(MipsCode::addi,"$sp","$fp","-16");
	//move $fp
	add(MipsCode::move,"$fp","$t0");
	add(MipsCode::jr,"$ra");
	//cout<<"Handlereturn out "<<endl;
}

void MipsInstr::loadReg(Operand opd,string reg)
{
	//cout<<"loadReg in"<<endl;
	tabItem C_item;
	stringstream ss;
	if(opd.opd_type==Opdtype::OPCONST)
	{
		add(MipsCode::li, reg, tostring(opd.value.cont));
	}
	else if(opd.opd_type==Opdtype::OPIDENT||opd.opd_type==Opdtype::OPTEMP)
	{
		C_item=*tab_set.find_item(opd.tabx,opd.opd_name);
		if(C_item.ptfunctab==opd.tabx)
		{
			ss.str("");
			ss<<C_item.addr<<"($fp)";
			//add(MipsCode::lw,reg,ss.str());
			add(MipsCode::lw,reg,loadaddr(opd));
		}
		else if(C_item.ptfunctab==0)
		{
			add(MipsCode::lw,"$t2","-8($fp)");
			ss.str("");
			ss<<C_item.addr<<"($t2)";
			//add(MipsCode::lw,reg,ss.str());
			add(MipsCode::lw,reg,loadaddr(opd));
		}
		else
		{
			ss.str("");
			ss<<C_item.addr<<"($fp)";
			add(MipsCode::lw,reg,ss.str());
		}
	}
	//cout<<"loadReg out"<<endl;
}
string MipsInstr::loadaddr(Operand opd)
{
	tabItem C_item;
	stringstream ss;
	if(opd.opd_type==Opdtype::OPIDENT||opd.opd_type==Opdtype::OPTEMP)
	{
		C_item=*tab_set.find_item(opd.tabx,opd.opd_name);
		if(C_item.ptfunctab==opd.tabx)
		{
			ss.str("");
			ss<<C_item.addr<<"($fp)";
		}
		else if(C_item.ptfunctab==0)
		{
			add(MipsCode::lw,"$t2","-8($fp)");
			ss.str("");
			ss<<C_item.addr<<"($t2)";
		}
		else
		{
			ss.str("");
			ss<<C_item.addr<<"($fp)";
		}
	}
	return ss.str();
}
void MipsInstr::storeMemory(string reg,Operand des)
{
	//cout<<"storeMemory in"<<endl;
	tabItem C_item;
	stringstream ss;
	C_item=*tab_set.find_item(des.tabx,des.opd_name);
	//存入的地址只可能是全局 或者这个函数自己的变量
	if(C_item.ptfunctab==des.tabx)
	{
		ss.str("");
		ss<<C_item.addr<<"($fp)";
		add(MipsCode::sw,reg,ss.str());
	}
	else if(C_item.ptfunctab==0)
	{
		//取出全局变量的基地址 在($t2)中
		add(MipsCode::lw,"$t2","-8($fp)");
		ss.str("");
		ss<<C_item.addr<<"($t2)";
		//找到全局变量的地址，将该值存入这个地址
		add(MipsCode::sw,reg,ss.str());
	} 
	//cout<<"storeMemory out"<<endl;
}