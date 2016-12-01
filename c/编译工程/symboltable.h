#ifndef __SYMBOLTABLE_H__
#define __SYMBOLTABLE_H__
#include <vector>
#include <sstream>
#include <iomanip>
#include <string>
#include <vector>
#include <fstream>
using namespace std;
extern ofstream symboltablefile;
//每个符号的类型 ，常量，或者变量 或者是数组 或者函数
enum Tokenkind
{
	CONST,      //常量
	VAR,        //变量
	FUNC,       //函数
	FPA,
	ARRAY,       //数组
	TEMP,
	UNDEFINE
};
//每个符号的类型 整型  字符 字符串 无返回值 
enum Tokentype {
	inttyp,   //对于 函数，常量，变量，数组
	chartyp,  // 对于 函数，常量，变量，数组
	stringtyp,  // 字符串
	voidtyp,  // 函数  有无返回值函数
	notyp    // 无类型
};
//数组的类型 整型或者字符
enum Arrtype {
	arr_inttyp,  
	arr_chartyp,  
};
//符号表表项 具有的元素
class tabItem
{
private:
public:
	tabItem()
	{
		//item_clear();
	}
	string name;
	Tokenkind kind;
	Tokentype type;
	char conch;
	string constring;  //字符串常量
	int value ;       // 整型或者字符常量的值
	int size;          // size of array
	int lev;           //层深，好像并没有层深，其实用不上
	int addr;         //偏移量
	int ptfunctab;    //函数对应的符号表
	void item_clear(); //清除表项信息
};
//符号表数据类型
class table
{
public:
	table();
	vector<tabItem> tab;
	int noforpar;
	int size;
	int pre_tab;
	int nex_tab;
};
//进行符号表管理
class tableManager
{
public:
	vector<table> Tab;
	int pCurrent_tab;
	int pTop_tab;
	tabItem current_item;
	//tabItem* Current_tabItem;
	tableManager(){initial();}
	string strtype[20];
	string strkind[10];
	void initial();
	bool enter(tabItem Current_Item,int Current_Tab);  //向当前的符号表中添加一个表项
	void enterConstint(Tokentype c_type,string c_name,int c_value,int Current_Tab);  // 常量进入当前的符号表
	void enterConstchar(Tokentype c_type,string c_name,char c_conch,int Current_Tab); // 常量进入当前的符号表
	void enterArray(Tokentype c_type,string c_name,int c_size,int Current_Tab); // 添加数组信息进入当前的符号表
	void enterFunc(Tokentype c_type,string c_name,int c_value,int Current_Tab);              //添加函数信息
	void enterVar(Tokentype c_type,string c_name,int Current_Tab);              //添加函数信息
	void enterFPA(Tokentype c_type,string c_name,int Current_Tab);               //添加形参信息
	void enterTEMP(Tokentype c_type,string c_name,int c_value,int Current_Tab);
	int new_table(int Current_Tab);
	void exit_func(int Current_Tab);
	tabItem* find_item(int Current_Tab,string name);
	tabItem find(int Tabindex,int tabitemindex);
	void printf_tab()
	{
		symboltablefile<<setw(10)<<"linenum"<<setw(10)<<"name"<<setw(10)<<"type"<<setw(10)
		<<"kind"<<setw(10)<<"value"<<setw(10)<<"size"<<setw(10)<<"tableNo"<<setw(10)
		<<"addr"<<endl;
	}
	void printftab(int Current_Tab);                         //输出当前符号表内容
};
#endif