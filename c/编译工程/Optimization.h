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
	Operand func; //�����������ĺ���
	int index;//������ı��
	vector<QuaterInstr> blockcodes; //������������м����
	vector<BaseBlock> beforeBlocks;//ǰ�̻�����
	vector<BaseBlock> afterBlocks;//��̻�����
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
	int codenum;//��¼���м���������
	vector<QuaterInstr> funccodes;   //����������е��м����
	vector<int> entrycodes;  //������
	vector<BaseBlock> baseblocks;    //���������Ӧ�ĳ�����ͼ
	void enterentrycode(int i);
	void findentrycode();            //�ҵ����е�������
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