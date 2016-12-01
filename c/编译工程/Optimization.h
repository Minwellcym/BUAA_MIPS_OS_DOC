#include <string>
#include "midCode.h"
#include "quater.h"
#include "symboltable.h"
#include "parser.h"
using namespace std;
class BaseBlock
{
private:
public:
	BaseBlock(){}
	Operand func; //基本块所属的函数
	int index;//基本块的编号
	vector<QuaterInstr> blockcodes; //基本块包含的中间代码
	vector<BaseBlock> beforeBlocks;//前继基本块
	vector<BaseBlock> afterBlocks;//后继基本块
};
class flowGraph
{
private:
public:
	flowGraph(){}
	vector<BaseBlock> baseblocks;
};
class FuncMidcode
{
private:
public:
	FuncMidcode(){}
	Operand func;
	int codenum;//记录的中间代码的数量
	vector<QuaterInstr> funccodes;   //这个函数所有的中间代码
	vector<int> entrycodes;  //入口语句
	vector<BaseBlock> baseblocks;    //这个函数对应的程序流图
	void enterentrycode(int i);
	void findentrycode();            //找到所有的入口语句
	void genflowgraph(Operand func);
	//flowGraph funcfg;  
};
class OPcode
{
private:
public:
	OPcode(){}
	vector<FuncMidcode> codes;
	void dividfunc();
	void printffunc();
};