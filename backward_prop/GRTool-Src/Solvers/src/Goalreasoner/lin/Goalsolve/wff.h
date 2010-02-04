#ifndef WFF_H
#define WFF_H

#include "goal.h"

typedef int boolvar;
typedef int literal;

enum vartype {NO,TS,TD,PS,PD,ITS,ITD,IPS,IPD};

struct clause 
{
  literal * cl;
  int dim;
  int litnum;
};

struct cnf 
{
  int varnum;
  int clausenum;
  int dim;
  clause * wff;
};


int sign (int n);
int abs (int n);


vartype vartype2inputvartype(vartype t); 
vartype inputvartype2vartype(vartype t); 


void prettyprintlit(fstream & f, literal l); 
void prettyprintlit(literal l);
void prettyprintclause(fstream & f, clause & c); 
void prettyprintclause(clause & c);
void prettyprintwff(fstream & f, cnf & w); 
void prettyprintwff(cnf & w);

void clauseinit(clause & c,int size);
void addliteral (clause & c, literal l);
void printclause(fstream & f, clause & c);


literal readliteral(fstream & f);  
clause readclause(fstream & f);  


void wffinit(cnf & w,int varnum,int clausenum);
void addclause (cnf & w, clause & c);
void addclauses (cnf & w, clause * cv, int clausenum);
void printwff(fstream & f, cnf & w);

boolvar goal2var (goalindex g,vartype v); 
goalindex var2goal (boolvar b); 
vartype var2vartype (boolvar b); 
literal goal2lit (goalindex g,vartype v, int sign); 
goalindex lit2goal (literal l);
int lit2sign (literal l);
vartype lit2vartype (literal l);

val vartype2val(vartype t);
sdval vartype2sdval(vartype t);
bool isinputvartype(vartype t);

void addmutex(literal l1, literal l2,cnf & w); 
void addsimpleimpl(literal l1, literal l2,cnf & w); 
void adddoubleimpl(literal l1, literal l2, literal l3, cnf & w); 
void addgenimpl(literal * l, int num, literal l0, cnf & w);


#endif
