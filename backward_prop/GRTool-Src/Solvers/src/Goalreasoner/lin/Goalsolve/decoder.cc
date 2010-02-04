#include <iostream.h>
#include <fstream.h>
#include <stdlib.h>
#include <string.h>

#include "goal.h"
#include "graph.h"
#include "wff.h"
#include "axioms.h"
#include "decoder.h"

extern char * valname [];
extern char * reltypename[];
extern char * vartypeprintvalues[];

void updatevalue(val & v1, val v2) 
{
  if (v2>v1) v1=v2;
}

void updategoalvalue(literal l,goal & g) 
{
  
// cerr << "Previous goal:\n";
// printgoal(g);
  if (lit2sign(l)>0) {
    vartype vt = lit2vartype(l);
    val v = vartype2val(vt);
    sdval sdv = vartype2sdval(vt);
    if (!isinputvartype(vt)) {
      if (sdv==SAT) 
	updatevalue(g.sat,v);
      else 
	updatevalue(g.den,v);
    }	
    else if (isinputvartype(vt)&&(g.input==TRUE)) {
      if (sdv==SAT) 
	updatevalue(g.Isat,v);
      else 
	updatevalue(g.Iden,v);
    }
  }
// cerr << "updated literal: " << l << endl;
// cerr << "resulted goal:\n";
// printgoal(g);
// cerr << endl; 
}

int chaffresdecode(fstream & f,goal * Goallist,int goalnum)
{
  char buffer[bufferlength];
  literal l;
  do {
    f >> buffer;
  } while ((strcmp(buffer,"satisfiable")!=0) and 
           (strcmp(buffer,"unsatisfiable")!=0) and 
           !f.eof());
  if (f.eof()) 
     errmsg("Wrong output from chaff");
  if (strcmp(buffer,"unsatisfiable")==0) {
    return 0;
  }
  else {
    for (int i =0;i<goalnum*8;i++) {
      f >> l;
      updategoalvalue(l,Goallist[lit2goal(l)]);
    }
    return 1;
  }
// for(int i=0;i<goalnum;i++)
// printgoal(Goallist[i]);
}

int minweightresdecode(fstream & f,goal * Goallist,int goalnum) 
{
  char buffer[bufferlength];
  literal l;
  do {
    f >> buffer;
  } while ((strcmp(buffer,"satisfiable")!=0) and 
           (strcmp(buffer,"unsatisfiable")!=0) and 
           !f.eof());
  if (f.eof()) 
     errmsg("Wrong output from MinWeight");
  if (strcmp(buffer,"unsatisfiable")==0) {
    return 0;
  }
  else {
    for (int i =0;i<goalnum*8;i++) {
      f >> l;
      updategoalvalue(l,Goallist[lit2goal(l)]);
    }
    return 1;
  }
}


void printgoalvalue (fstream & f, goal & g, goalindex gi) 
{ 
if (g.sat!=N||g.den!=N) {
  if (g.sat!=N)
     f << valname[g.sat] << "S " << gi << " ; ";
  else
     f << "        ";
  if (g.den!=N)
     f << valname[g.den] << "D " << gi << " ; ";
  else
     f << "        ";
//   if (g.input) {
//     if (g.Isat!=N)
//       f << "I" << valname[g.Isat] << "S " << gi << " ; ";
//     else
//        f << "         ";
//     if (g.Iden!=N)
//        f << "I" << valname[g.Iden] << "D " << gi << " ; ";
//     else
//        f << "         ";
//   }
  if (g.input==TRUE) 
    f << "INPUT";
  f << endl;
} 
}

void printgoalvalues(fstream & f,goal * Goallist,int goalnum)
{
  for(int i=0;i<goalnum;i++) {
    printgoalvalue(f,Goallist[i],i);
  }
}


