#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <fstream>
#include <string>
#include "symboltable.h"
#include "error.h"
#include "Symbol.h"
using namespace std;

//extern tabItem current_item;
extern int lc;  //�м�����
extern int tempcount;
extern tabItem temp_item;
extern ofstream symboltablefile;
extern ofstream errmsgfile;  //������Ϣ������ļ�
int lineoftab=1;
int addr=0;
extern Error error;

void tableManager::initial() 
{
	strkind[CONST]=" const ";
	strkind[VAR]=" var ";
	strkind[FUNC]=" func "; 
	strkind[ARRAY]=" array ";
	strkind[FPA]=" for par ";
	strkind[TEMP]=" Temp ";
	strkind[UNDEFINE]=" Undefine ";
	strtype[inttyp]=" int ";
	strtype[chartyp]=" char ";
	strtype[stringtyp]=" string ";
	strtype[voidtyp]=" void ";
	strtype[notyp]=" notyp ";
}
//����ǰ�ķ��ű����½��ǰ�ķ��ű�  
bool tableManager::enter(tabItem Current_Item,int Current_Tab)
{
	if(find_item(Current_Tab,Current_Item.name)==NULL||(find_item(Current_Tab,Current_Item.name)->ptfunctab!=Current_Tab))
	{
		Tab[Current_Tab].tab.push_back(Current_Item);
		return true;
	}
	else
	{
		error.printferrmsg(0,0,lc);
		return false;
	}
}
//���ͳ��� ����
void tableManager::enterConstint(Tokentype c_type,string c_name,int c_value,int Current_Tab)
{
	current_item.item_clear();
	current_item.name=c_name;
	current_item.type=c_type;
	current_item.value=c_value;
	current_item.kind=CONST;
	current_item.ptfunctab=Current_Tab;
	current_item.size=1;
	enter(current_item,Current_Tab);
}
//�ַ����� ����
void tableManager::enterConstchar(Tokentype c_type,string c_name,char c_conch,int Current_Tab)
{
	current_item.item_clear();
	current_item.name=c_name;
	current_item.type=c_type;
	current_item.conch=c_conch;
	current_item.kind=CONST;
	current_item.value=int(c_conch);
	current_item.ptfunctab=Current_Tab;
	current_item.size=1;
	enter(current_item,Current_Tab);
}
//��������
void tableManager::enterVar(Tokentype c_type,string c_name,int Current_Tab)
{
	current_item.item_clear();
	current_item.name=c_name;
	current_item.type=c_type;
	current_item.kind=VAR;
	current_item.ptfunctab=Current_Tab;
	current_item.size=1;
	enter(current_item,Current_Tab);
}   
//�����������
void tableManager::enterArray(Tokentype c_type,string c_name,int c_size,int Current_Tab)
{
	current_item.item_clear();
	current_item.name=c_name;
	current_item.type=c_type;
	current_item.kind=ARRAY;
	current_item.size=c_size;
	current_item.ptfunctab=Current_Tab;
	enter(current_item,Current_Tab);
}
//���뺯������  �Ƚ��������浽���ű�  Ȼ����뺯���ķ��ű� ��ǰ�ķ��ű�תΪ�����ķ��ű�
void tableManager::enterFunc(Tokentype c_type,string c_name,int c_value,int Current_Tab)
{
	current_item.item_clear();
	pCurrent_tab=new_table(Current_Tab);
	current_item.name=c_name;
	current_item.type=c_type;
	current_item.kind=FUNC;
	current_item.ptfunctab=pCurrent_tab;
	current_item.addr=c_value;
	current_item.value=0;
	enter(current_item,Current_Tab);
}
void tableManager::enterFPA(Tokentype c_type,string c_name,int Current_Tab)
{
	current_item.item_clear();
	current_item.name=c_name;
	current_item.type=c_type;
	current_item.kind=FPA;
	current_item.ptfunctab=Current_Tab;
	current_item.size=1;
	enter(current_item,Current_Tab);
}
void tableManager::enterTEMP(Tokentype c_type,string c_name,int c_value,int Current_Tab)
{
	current_item.item_clear();
	current_item.name=c_name;
	current_item.type=c_type;
	current_item.value=c_value;
	current_item.kind=TEMP;
	current_item.ptfunctab=Current_Tab;
	current_item.value=0;
	current_item.size=1;
	enter(current_item,Current_Tab);
}
//���뺯�� �½����ű� Ȼ��ǰ�ķ��ű��л����½��ĺ����ķ��ű�
int tableManager::new_table(int Current_Tab)
{
	table n_tab;
	n_tab.pre_tab=pTop_tab;
	Tab.push_back(n_tab);
	return Tab.size()-1;
}

//�������� ��ǰ�ķ��ű�ص�����
void tableManager::exit_func(int Current_Tab)
{
	pCurrent_tab=Tab[Current_Tab].pre_tab;
}
void tabItem::item_clear()
{
		name="";
		kind=UNDEFINE;
		type=notyp;
		conch='p';
		constring="";
		value=0;
		lev=0;
		addr=0;
		size=0;
		ptfunctab=0;
}
table::table()
{
	size=0;
}

tabItem* tableManager::find_item(int Current_Tab,string name)
{
	int i=0;
	tabItem* temp=NULL;
	for(i=0;i<Tab[Current_Tab].tab.size();i++)
	{
		if(name==Tab[Current_Tab].tab[i].name)
		{
			temp=&Tab[Current_Tab].tab[i];
			i=Tab[Current_Tab].tab.size();
		}
	}
	if(temp==NULL&&Current_Tab!=pTop_tab)
		temp=find_item(Tab[Current_Tab].pre_tab,name);
	return temp;
}
tabItem tableManager::find(int Tabindex,int tabitemindex)
{
	return Tab[Tabindex].tab[tabitemindex];
}
void tableManager::printftab(int Current_Tab)
{
	cout<<"enter printftab"<<endl;
	//cout<<Current_Tab<<endl;
	symboltablefile<<setw(10)<<Tab[Current_Tab].size<<endl;
	symboltablefile<<setw(10)<<Tab[Current_Tab].noforpar<<endl;
	int i;
	for(i=0;i<Tab[Current_Tab].tab.size();i++)
	{
		if(Tab[Current_Tab].tab[i].kind==CONST)
		{
			symboltablefile<<setw(10)<<lineoftab++<<setw(10)<<Tab[Current_Tab].tab[i].name<<setw(10)<<strtype[Tab[Current_Tab].tab[i].type]
			<<setw(10)<<strkind[Tab[Current_Tab].tab[i].kind]<<setw(10)<<Tab[Current_Tab].tab[i].value
				<<setw(10)<<Tab[Current_Tab].tab[i].size<<setw(10)<<Tab[Current_Tab].tab[i].ptfunctab
				<<setw(10)<<Tab[Current_Tab].tab[i].addr<<endl;
		}
		else if(Tab[Current_Tab].tab[i].kind==VAR)
		{
			symboltablefile<<setw(10)<<lineoftab++<<setw(10)<<Tab[Current_Tab].tab[i].name<<setw(10)<<strtype[Tab[Current_Tab].tab[i].type]
			<<setw(10)<<strkind[Tab[Current_Tab].tab[i].kind]<<setw(10)<<Tab[Current_Tab].tab[i].value
				<<setw(10)<<Tab[Current_Tab].tab[i].size<<setw(10)<<Tab[Current_Tab].tab[i].ptfunctab
				<<setw(10)<<Tab[Current_Tab].tab[i].addr<<endl;
		}
		else if(Tab[Current_Tab].tab[i].kind==ARRAY)
		{
			symboltablefile<<setw(10)<<lineoftab++<<setw(10)<<Tab[Current_Tab].tab[i].name<<setw(10)<<strtype[Tab[Current_Tab].tab[i].type]
			<<setw(10)<<strkind[Tab[Current_Tab].tab[i].kind]<<setw(10)<<Tab[Current_Tab].tab[i].value
				<<setw(10)<<Tab[Current_Tab].tab[i].size<<setw(10)<<Tab[Current_Tab].tab[i].ptfunctab
				<<setw(10)<<Tab[Current_Tab].tab[i].addr<<endl;
		}
		else if(Tab[Current_Tab].tab[i].kind==FUNC)
		{
			symboltablefile<<setw(10)<<lineoftab++<<setw(10)<<Tab[Current_Tab].tab[i].name<<setw(10)<<strtype[Tab[Current_Tab].tab[i].type]
			<<setw(10)<<strkind[Tab[Current_Tab].tab[i].kind]<<setw(10)<<Tab[Current_Tab].tab[i].value
				<<setw(10)<<Tab[Current_Tab].tab[i].size<<setw(10)<<Tab[Current_Tab].tab[i].ptfunctab
				<<setw(10)<<Tab[Current_Tab].tab[i].addr<<endl;
			printftab(Tab[Current_Tab].tab[i].ptfunctab);
		}
		else if(Tab[Current_Tab].tab[i].kind==FPA)
		{
			symboltablefile<<setw(10)<<lineoftab++<<setw(10)<<Tab[Current_Tab].tab[i].name<<setw(10)<<strtype[Tab[Current_Tab].tab[i].type]
			<<setw(10)<<strkind[Tab[Current_Tab].tab[i].kind]<<setw(10)<<Tab[Current_Tab].tab[i].value
				<<setw(10)<<Tab[Current_Tab].tab[i].size<<setw(10)<<Tab[Current_Tab].tab[i].ptfunctab
				<<setw(10)<<Tab[Current_Tab].tab[i].addr<<endl;
		}
		else if(Tab[Current_Tab].tab[i].kind==TEMP)
		{
			symboltablefile<<setw(10)<<lineoftab++<<setw(10)<<Tab[Current_Tab].tab[i].name<<setw(10)<<strtype[Tab[Current_Tab].tab[i].type]
			<<setw(10)<<strkind[Tab[Current_Tab].tab[i].kind]<<setw(10)<<Tab[Current_Tab].tab[i].value
				<<setw(10)<<Tab[Current_Tab].tab[i].size<<setw(10)<<Tab[Current_Tab].tab[i].ptfunctab
				<<setw(10)<<Tab[Current_Tab].tab[i].addr<<endl;
		}
	}
	cout<<"leave printftab"<<endl;
	return ;
}
