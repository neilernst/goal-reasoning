#include <iostream.h>
#include <fstream.h>
#include <stdlib.h>
#include <string.h>

#include "goal.h"
#include "graph.h"
#include "wff.h"

// -------------------------- FUNCTIONS -------------------------- 

// I/O: 

void readheader (fstream & f, int & goalnum, int & relnum) 
{
  char token [TKLENGTH];
  f >> token;
  if (strcmp(token,"D")!=0) 
    errmsg("D expected");
  f >> goalnum;
  f >> relnum;
}

goal * readgoallist (fstream & f, int goalnum) 
{
  goal * Goallist = new goal[goalnum];
  for(int i=0;i<goalnum;i++) 
    Goallist[i]=readgoal(f);
  return Goallist;
}

void printgoallist (fstream & f, goal * Goallist, int goalnum)
{
  for(int i=0;i<goalnum;i++) {
    f << "N" << i << " ";
    printgoal(f,Goallist[i]);
  }
}

void updategoallist(relation * Rellist,int relnum, goal * Goallist) 
{
  for(int i=0;i<relnum;i++) 
    updategoal(Goallist[Rellist[i].target],Rellist[i]);
}
  
relation * readrellist (fstream & f, int relnum) 
{
  relation * Rellist = new relation[relnum];
  for(int i=0;i<relnum;i++) {
    Rellist[i]=readrelation(f);
  }  
  return Rellist;
}

void printrellist (fstream & f, relation * Rellist, int relnum)
{
  for(int i=0;i<relnum;i++) {
    f << "R" << " ";
    printrelation(f,Rellist[i]);
  }
}

void readinputgoals(fstream & f, goal * Goallist,int goalnum) 
{
  int index;
  while(!f.eof()) {
    f >> index;
    if (f.fail())
      break;
    if (index >= goalnum) 
      errmsg("Input goal file: wrong input goal");
    Goallist[index].input = TRUE;
  }
}

