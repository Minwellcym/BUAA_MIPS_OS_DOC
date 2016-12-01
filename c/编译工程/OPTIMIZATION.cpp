#include "error.h"
#include "MipsInstr.h"
#include "quater.h"
#include "parser.h"
#include "symboltable.h"
#include "Optimization.h"

using namespace std;

extern QCode midCode;
extern ofstream mipscodefile;
extern ofstream  outputopfile;
int blockcount=0;



void OPcode::dividfunc()
{
	int i=0;
	while(i<midCode.Qcode.size())
	{
		if(midCode.Qcode[i].op==Opcode::LAB
			&&midCode.Qcode[i].des.t_kind==Tokenkind::FUNC)
		{
			FuncMidcode temp;
			temp.func=midCode.Qcode[i].des;
			outputopfile<<"当前函数是："<<midCode.Qcode[i].des.opd_name<<endl;
			blockcount=0;
			temp.funccodes.push_back(midCode.Qcode[i]);
			i++;
			while( (i<midCode.Qcode.size()) &&
				!(midCode.Qcode[i].op==Opcode::LAB && 
				midCode.Qcode[i].des.t_kind==Tokenkind::FUNC) )
			{
				temp.funccodes.push_back(midCode.Qcode[i]);
				i++;
			}
			temp.genflowgraph(temp.func);
			codes.push_back(temp);
		}
	}
	outputopfile<<"结束划分函数"<<endl;
}
void FuncMidcode::enterentrycode(int index)
{
	int i;
	bool exist=false;
	for(i=0;i<entrycodes.size();i++)
	{
		if(entrycodes[i]==index)
		{
			exist=true;
		}
	}
	if(!exist)
	{
		entrycodes.push_back(index); 
		outputopfile<<"第"<<index<<"条语句是入口语句"<<endl;
	}
}
void FuncMidcode::findentrycode()
{
	//outputopfile<<"开始确定入口语句！"<<endl;
	int i;
	enterentrycode(0);     //规则一:第一条语句是入口语句
	for(i=0;i<funccodes.size();i++)
	{
		//规则二：跳转语句跳转到的是入口语句
		if(funccodes[i].op==Opcode::LAB)
		{
			enterentrycode(i);
		}
		//规则三：跳转语句后的是入口语句
		else if(funccodes[i].op==Opcode::BEQ ||
			funccodes[i].op==Opcode::BGE ||
			funccodes[i].op==Opcode::BGT ||
			funccodes[i].op==Opcode::BLE ||
			funccodes[i].op==Opcode::BLT ||
			funccodes[i].op==Opcode::BNE ||
			funccodes[i].op==Opcode::JMP)
		{
			i++;
			enterentrycode(i);
		}
	}
	//outputopfile<<"入口语句确定完毕！"<<endl;
}
void FuncMidcode::genflowgraph(Operand func)
{
	outputopfile<<func.opd_name<<"函数的流图生成"<<endl;
	findentrycode();
	outputopfile<<entrycodes.size()<<endl;
	int i,j;
	for(i=1;i<entrycodes.size();i++)
	{
		BaseBlock temp;
		temp.func=func;
		temp.index=++blockcount;
		outputopfile<<"[Block"<<blockcount<<"]:"<<entrycodes[i-1]<<
			"至"<<entrycodes[i]-1<<endl;
		for(j=entrycodes[i-1];j<entrycodes[i];j++)
		{
			temp.blockcodes.push_back(funccodes[j]);
		}
		baseblocks.push_back(temp);
	}
}











