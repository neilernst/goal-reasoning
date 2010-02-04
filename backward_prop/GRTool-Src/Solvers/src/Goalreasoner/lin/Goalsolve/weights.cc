#include <iostream.h>
#include <fstream.h>
#include <stdlib.h>
#include <string.h>

#include "goal.h"
#include "graph.h"
#include "wff.h"
#include "axioms.h"
#include "weights.h"

extern char * vartypeprintvalues[];

void readweights(fstream & f,goal * Goallist,int goalnum) 
{
  goalindex i;
  char buffer[10];
  int tsw,tdw,psw,pdw;
cerr << "readweights:";
cerr.flush(); 
  while(!f.eof()) {
    f >> buffer;
cerr << buffer << " ";
    if (strlen(buffer)==0)
      break;
    i = atoi(buffer);
    if (i==0&&buffer[0]!=0) 
      errmsg("Error in reading the weight file: goal index is expected");
    f >> buffer;
cerr << buffer << " ";
    if (buffer[0] != ':')
      errmsg("Error in reading the weight file: ':' expected");
    f >> tsw >> tdw >> psw >> pdw;
cerr << tsw << " " <<  tdw << " " <<  psw << " " <<  pdw;
    f >> buffer;
cerr << buffer << "\n";
    if (buffer[0] != ';')
      errmsg("Error in reading the weight file: ';' expected");
    Goallist[i].weights[TS]=tsw-psw;
    Goallist[i].weights[TD]=tdw-pdw;
    Goallist[i].weights[PS]=psw;
    Goallist[i].weights[PD]=pdw;
  }
}

void printweights(fstream & f,goal * Goallist,int goalnum) 
{
  f << ":\n";
  for (int i = 0;i<goalnum;i++) {
    for (int j = 1;j<=8;j++) {
      f << Goallist[i].weights[j] << endl;
    }
  }
  f << "%\n";
}

void prettyprintweights(fstream & f,goal * Goallist,int goalnum) 
{
  f << "c WEIGHTS:\nc ";
  for (int i = 0;i<goalnum;i++) {
  bool found=FALSE;
    for (int j = 1;j<=8;j++) {
      if (Goallist[i].weights[j]!=0) {
        f << "" << vartypeprintvalues[j] << i << ": " 
          << Goallist[i].weights[j] << "  ";
	found = TRUE;
      }
    }
  if (found) 
     f << "\nc ";
  }
  f << endl;
}
