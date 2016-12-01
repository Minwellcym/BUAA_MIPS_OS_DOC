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
//ÿ�����ŵ����� �����������߱��� ���������� ���ߺ���
enum Tokenkind
{
	CONST,      //����
	VAR,        //����
	FUNC,       //����
	FPA,
	ARRAY,       //����
	TEMP,
	UNDEFINE
};
//ÿ�����ŵ����� ����  �ַ� �ַ��� �޷���ֵ 
enum Tokentype {
	inttyp,   //���� ����������������������
	chartyp,  // ���� ����������������������
	stringtyp,  // �ַ���
	voidtyp,  // ����  ���޷���ֵ����
	notyp    // ������
};
//��������� ���ͻ����ַ�
enum Arrtype {
	arr_inttyp,  
	arr_chartyp,  
};
//���ű���� ���е�Ԫ��
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
	string constring;  //�ַ�������
	int value ;       // ���ͻ����ַ�������ֵ
	int size;          // size of array
	int lev;           //�������û�в����ʵ�ò���
	int addr;         //ƫ����
	int ptfunctab;    //������Ӧ�ķ��ű�
	void item_clear(); //���������Ϣ
};
//���ű���������
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
//���з��ű����
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
	bool enter(tabItem Current_Item,int Current_Tab);  //��ǰ�ķ��ű������һ������
	void enterConstint(Tokentype c_type,string c_name,int c_value,int Current_Tab);  // �������뵱ǰ�ķ��ű�
	void enterConstchar(Tokentype c_type,string c_name,char c_conch,int Current_Tab); // �������뵱ǰ�ķ��ű�
	void enterArray(Tokentype c_type,string c_name,int c_size,int Current_Tab); // ���������Ϣ���뵱ǰ�ķ��ű�
	void enterFunc(Tokentype c_type,string c_name,int c_value,int Current_Tab);              //��Ӻ�����Ϣ
	void enterVar(Tokentype c_type,string c_name,int Current_Tab);              //��Ӻ�����Ϣ
	void enterFPA(Tokentype c_type,string c_name,int Current_Tab);               //����β���Ϣ
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
	void printftab(int Current_Tab);                         //�����ǰ���ű�����
};
#endif