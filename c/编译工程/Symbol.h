#ifndef __SYMBOL_H__
#define __SYMBOL_H__

#include <iostream>
#include <set>
#include <string>
#include <map>

using namespace std;
enum Symbol
{ 
	forsym,whilesym,dosym,
	ifsym,elsesym,switchsym,casesym,defaultsym,
	intsym,charsym,constsym,
	printfsym,scanfsym,voidsym,mainsym,returnsym,
	plus,minus,times,divid,eql,neq,gtr,lss,leq,geq,
	lparent,rparent,lbrack,rbrack,lbrace,rbrace,comma,becomes,
	ident,number,unsgnumber,
	singlequote,doublequote,
	chcon,strcon,semicolon
};
#endif