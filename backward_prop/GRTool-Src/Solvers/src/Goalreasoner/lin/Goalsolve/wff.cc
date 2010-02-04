#include <iostream.h>
#include <fstream.h>
#include <stdlib.h>
#include <string.h>

#include "goal.h"
#include "graph.h"
#include "wff.h"


vartype vartypevalues[9] = {NO,TS,TD,PS,PD,ITS,ITD,IPS,IPD};
char * vartypeprintvalues[9] = {"NO","TS","TD","PS","PD","ITS","ITD","IPS","IPD"};

// ---------------------------- FUCTIONS --------------------

vartype vartype2inputvartype(vartype t) 
{
  switch (t) {
  case TS: return ITS;
  case TD: return ITD;
  case PS: return IPS;
  case PD: return IPD;
    errmsg("Wrong call to vartype2inputvartype");
  }
}

vartype inputvartype2vartype(vartype t) 
{
  switch (t) {
  case ITS: return TS;
  case ITD: return TD;
  case IPS: return PS;
  case IPD: return PD;
  default: 
    errmsg("Wrong call to inputvartype2vartype");
  }
}

val vartype2val(vartype t) 
{
  switch (t) {
    case ITS: case ITD: case TS: case TD: return T;
    case IPS: case IPD: case PS: case PD: return P;
    case NO: return N;
  }
}

sdval vartype2sdval(vartype t) 
{
  switch (t) {
    case ITS: case IPS: case TS: case PS: return SAT;
    case ITD: case IPD: case TD: case PD: return DEN;
  }
}

bool isinputvartype(vartype t) 
{
  return (t==ITS||t==ITD||t==IPS||t==IPD);
}


literal readliteral(fstream & f) 
{
  literal l;
  int sign=1;
  goalindex g;
  vartype t;
  char * buffer = new char[10];
  f >> buffer;
  if (buffer[0] == ';')
    return 0;
  if (strlen(buffer)== 0)
    return 0;
//cerr << "buffer: " << buffer << " ";
  if (buffer[0] == '-'){
    buffer++;
    sign=-1;
//cerr << "buffer: " << buffer << " ";
  }
//cerr << endl;  
  if      (strcmp(buffer,"ITS")==0) t=ITS;
  else if (strcmp(buffer,"IPS")==0) t=IPS;
  else if (strcmp(buffer,"ITD")==0) t=ITD;
  else if (strcmp(buffer,"IPD")==0) t=IPD;
  else if (strcmp(buffer,"TS")==0) t=TS;
  else if (strcmp(buffer,"PS")==0) t=PS;
  else if (strcmp(buffer,"TD")==0) t=TD;
  else if (strcmp(buffer,"PD")==0) t=PD;
  else errmsg("Wrong type of literal; ",buffer);
  f >> g;
  l = goal2lit(g,t,sign);
//cerr << "buffer: " << buffer << "\n";
//cerr << "Literal: " << l << endl;  
  return l;
}

clause readclause(fstream & f) 
{
  clause c;
  int flag = 0;
  int i=0;
  char buffer[10];
  literal constraint[MAXCONSTRAINLENGTH];
  constraint[i++] = readliteral(f);
  if (constraint[i-1]==0) {
   clauseinit(c,0);
   return c;
  }
  
//cerr <<  constraint[i-1] << " ";
  do {
    f >> buffer;
    if (buffer[0]==';')
      break;
    if (strcmp(buffer,"OR")!=0)
      errmsg("';' or 'OR' expected");
    constraint[i++] = readliteral(f);
//cerr <<  constraint[i-1] << " ";
  } while (1);
//cerr << "Clause size : " << i-1 << endl;
  clauseinit(c,i);
  for(int j=0;j<i;j++) {
    addliteral(c,constraint[j]);
  }
//cerr <<  endl;
  return c;
}

void prettyprintlit(fstream & f, literal l) 
{
  f << (sign(l)==-1 ? "-" : "") 
    << vartypeprintvalues[lit2vartype(l)] 
    << lit2goal(l);
}

void prettyprintlit(literal l) 
{
  cerr << (sign(l)==-1 ? "-" : "") 
    << vartypeprintvalues[lit2vartype(l)] 
    << lit2goal(l);
}

void prettyprintclause(fstream & f, clause & c) 
{
  f << "c ";
  prettyprintlit(f,c.cl[0]);
  for(int i = 1;i<c.dim;i++) {
    f << " v ";
    prettyprintlit(f,c.cl[i]);
  }
  f << "\n";
}

void prettyprintclause(clause & c) 
{
  cerr << "c ";
  prettyprintlit(c.cl[0]);
  for(int i = 1;i<c.dim;i++) {
    cerr << " v ";
    prettyprintlit(c.cl[i]);
  }
  cerr << "\n";
}

void prettyprintwff(fstream & f, cnf & w) 
{
  for(int i = 0;i<w.dim;i++)
    prettyprintclause(f,w.wff[i]);
  f.flush();
}

void prettyprintwff(cnf & w) 
{
  for(int i = 0;i<w.dim;i++)
    prettyprintclause(w.wff[i]);
}


// clause & wff manipulators

void clauseinit(clause & c,int size) 
{
  c.cl = new literal[size];
  c.dim = 0;
  c.litnum=size;
}

void addliteral (clause & c, literal l) 
{
  if(c.dim>=c.litnum)
    errmsg("Number of literals exceeded");
  c.cl[c.dim]=l;
  c.dim++;
}

void printclause(fstream & f, clause & c) 
{
  for(int i = 0;i<c.dim;i++)
    f << c.cl[i] << " ";
  f << "0\n";
}

void wffinit(cnf & w,int varnum,int clausenum) 
{
  w.varnum=varnum;
  w.clausenum=clausenum;
  w.dim=0;
  w.wff = new clause[clausenum];
}

void addclause (cnf & w, clause & c) 
{

  if (c.dim>0) {
    if (w.dim>=w.clausenum) {
      errmsg("\n++++++++++++++++++++++++++\nNumber of clauses exceeded\n++++++++++++++++++++++++++\n");
    }
    w.wff[w.dim]=c;
    w.dim++;
  }
}

void addclauses (cnf & w, clause * cv, int clausenum)
{
  for(int i=0;i<clausenum;i++)
    addclause(w,cv[i]);
}


void printwff(fstream & f, cnf & w) 
{
  f << "p cnf " << w.varnum << " " << w.dim << endl;
  for(int i = 0;i<w.dim;i++)
    printclause(f,w.wff[i]);
}

// ------------------------- ENCODING -----------------------------

int sign (int n) 
{
  return (n>=0) ? 1 : -1;
}

int abs (int n) 
{
  return (n>=0) ? n : -n;
}

boolvar goal2var (goalindex g,vartype v) 
{
  return (boolvar) g*8+v;
}

goalindex var2goal (boolvar b) 
{
  return (goalindex) (b-1)/8;
}

vartype var2vartype (boolvar b) 
{
  return vartypevalues[(b-1)%8+1];
}

literal goal2lit (goalindex g,vartype v, int sign) 
{
  return (literal) sign*goal2var(g,v);
}

goalindex lit2goal (literal l) 
{
  return var2goal(abs(int(l)));
}

int lit2sign (literal l)  
{
  return sign(int(l));
}

vartype lit2vartype (literal l) 
{
  return var2vartype(abs(int(l)));
}

// adds the clause -l1v-l2
void addmutex(literal l1, literal l2,cnf & w) 
{
  clause c;
  clauseinit(c,2);
  addliteral(c,-l1);
  addliteral(c,-l2);
  addclause(w,c);
}

// adds the clause l1->l2
void addsimpleimpl(literal l1, literal l2,cnf & w) 
{
  clause c;
  clauseinit(c,2);
  addliteral(c,-l1);
  addliteral(c,l2);
  addclause(w,c);
}


// adds the clause l1^l2->l3
void adddoubleimpl(literal l1, literal l2, literal l3, cnf & w) 
{
  clause c;
  clauseinit(c,3);
  addliteral(c,-l1);
  addliteral(c,-l2);
  addliteral(c,l3);
  addclause(w,c);
}

// adds the clause /\li->l0
void addgenimpl(literal * l, int num, literal l0, cnf & w) 
{
  clause c;
  clauseinit(c,num+1);
  for(int i=0;i<num;i++)
    addliteral(c,-l[i]);
  addliteral(c,l0);
  addclause(w,c);
}
