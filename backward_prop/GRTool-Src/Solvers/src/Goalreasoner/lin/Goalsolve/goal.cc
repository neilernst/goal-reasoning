#include <iostream.h>
#include <fstream.h>
#include <stdlib.h>
#include <string.h>

#include "goal.h"

// -------------------------- GLOBAL VARS -------------------------- 

char * valname [] = {"N","P","T"};
char * reltypename[] = {"AND","OR ","+  ","-  ","*  ","/  ","++ ","-- ","** ","// "};
// char * reltypename[] = {"AND","OR","+S","-S","+D","-D","++S","--S","++D","--D"};

// -------------------------- FUNCTIONS -------------------------- 

// ------------- I/O -------------

// Goals:

void errmsg(char msg[]) 
{
  cerr << msg << endl;
  exit(0);
}

void errmsg(char msg[],char msg1[]) 
{
  cerr << msg << msg1 << endl;
  exit(0);
}

val readval (fstream & f) 
{
  char token [TKLENGTH];
  f >> token;
  if (strcmp(token,"NO")==0) 
    return N;
  else if (strcmp(token,"PS")==0 || strcmp(token,"PD")==0) 
    return P;
  else if (strcmp(token,"TS")==0 || strcmp(token,"TD")==0) 
    return T;
  else 
    errmsg("Wrong value");
  return N;
}

goal readgoal (fstream & f) 
{
  char token [TKLENGTH];
  goal g;
  g.incomingnum=0;
  g.input=FALSE;
  f >> token;
  if (strcmp(token,"N")!=0)
    errmsg("N expected");
  g.sat = readval(f);
  g.den = readval(f);
  g.Isat = g.Iden = N;
  for(int i=0;i<9;i++)
    g.weights[i]=0;
  return g;
}

void printgoal (fstream & f, goal & g) 
{
  f << valname[g.sat] << ((g.sat==N)? "O " : "S ");
  f << valname[g.den] << ((g.den==N)? "O " : "D ");
  f << valname[g.Isat] << ((g.Isat==N)? "O " : "S ");
  f << valname[g.Iden] << ((g.Iden==N)? "O " : "D ");
  f << (g.input ? " INPUT " : "");
  f << endl;
  for (int i=0;i<g.incomingnum;i++) {
    f << "  ";
    printrelation(f,*(g.incoming[i]));
  }
}

void printgoal (goal & g) 
{
  cerr << valname[g.sat] << ((g.sat==N)? "O " : "S ");
  cerr << valname[g.den] << ((g.den==N)? "O " : "D ");
  cerr << valname[g.Isat] << ((g.Isat==N)? "O " : "S ");
  cerr << valname[g.Iden] << ((g.Iden==N)? "O " : "D ");
  cerr << (g.input ? " INPUT " : "");
  cerr << endl;
  for (int i=0;i<g.incomingnum;i++) {
    cerr << "  ";
    printrelation(*(g.incoming[i]));
  }
}

void updategoal (goal & g,relation & r) 
{
  if (g.incomingnum>=MAXINCOMING)
    errmsg("Max number of incoming relations exceeded!");
  g.incoming[g.incomingnum++]=&r;
}


// Relations:

reltype readreltype (fstream & f) 
{
  char token [TKLENGTH];
  f >> token;
  if (strcmp(token,"AND")==0) return AND;
  if (strcmp(token,"OR")==0)  return OR;
  if (strcmp(token,"+")==0)   return PLUSS;
  if (strcmp(token,"-")==0)   return MINUSS;
  if (strcmp(token,"*")==0)   return PLUSD;
  if (strcmp(token,"/")==0)   return MINUSD;
  if (strcmp(token,"++")==0)  return PLUSPLUSS;
  if (strcmp(token,"--")==0)  return MINUSMINUSS;
  if (strcmp(token,"**")==0)  return PLUSPLUSD;
  if (strcmp(token,"//")==0)  return MINUSMINUSD;
  
  errmsg("Wrong value");
  return AND;
}

relation readrelation (fstream & f) 
{
  char token [TKLENGTH];
  relation r;
  f >> token;
  if (strcmp(token,"R")!=0)
    errmsg("R expected");
  r.type = readreltype(f);
  f >> r.target;
  r.sourcenum=0;
  do {
    f >> r.sources[r.sourcenum++];
  } while (!f.fail());
  f.clear();
  r.sourcenum--;
  return r;
}

void printrelation (fstream & f, relation & r) 
{
  f << reltypename[r.type] << " ";
  f << r.target << " ";
  for(int i=0;i<r.sourcenum;i++) 
    f << r.sources[i] << " ";
  f << endl;
}
    
void printrelation (relation & r) 
{
  cerr << reltypename[r.type] << " ";
  cerr << r.target << " ";
  for(int i=0;i<r.sourcenum;i++) 
    cerr << r.sources[i] << " ";
  cerr << endl;
}
    


    
  
