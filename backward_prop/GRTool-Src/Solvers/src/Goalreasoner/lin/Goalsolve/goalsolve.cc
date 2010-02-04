#include <iostream.h>
#include <fstream.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "config.h"
#include "goal.h"
#include "graph.h"
#include "wff.h"
#include "axioms.h"
#include "decoder.h"


// -------------------------- FUNCTIONS -------------------------- 

void printinputdefault() 
{
 cerr << "REMARK: if no <inputnodefile> is provided, then  by default\n";
 cerr << "all goals with no incoming boolean relation are considered as input nodes\n";
}

void printusage() 
{
  cerr << "Usage:\n";
  cerr << "goalsolve -n|-w|-m|-s <graphfile> <desideratafile> [<inputnodefile>]\n\n";
  cerr << "-n :         no conflict avoinding\n";
  cerr << "-w (weak):   avoinding conflicts in the form  'TSg and TDg'\n";
  cerr << "-m (medium): avoinding conflicts in the forms 'TSg and TDg', 'TSg and PDg', 'PSg and TDg'\n";
  cerr << "-s (strong): avoinding conflicts in the forms 'TSg and TDg', 'TSg and PDg', 'PSg and TDg', 'PSg and PDg'\n\n";
  printinputdefault();
}

// -------------------------- MAIN -------------------------- 

int main (int argc, char * argv[]) 
{

  extern char **environ; // needed by execl

  fstream graphfile;
  fstream inputnodesfile;
  fstream desideratafile;
  fstream wfffile;
  fstream chaffresultfile;
  fstream resultfile;
  
  goal * Goallist;
  relation * Rellist;
  cnf W;
  int goalnum,relnum;  
  int res;
  char c;
  goal g;
  conflicttype ct = NOCONFLICT;

//define paths and chaff execution script 
  strcat(chaffcommand," ");
  strcat(chaffcommand,wfffilename);
  strcat(chaffcommand," > ");
  strcat(chaffcommand,chaffresultfilename);
// cerr << "Execution:" << chaffscript << endl;

// Checking command line
  if (argc<4 || argc>5) {
    printusage();
    errmsg("");
  }
  if (strcmp(argv[1],"-n")==0) ct = NOCONFLICT; 
  else if (strcmp(argv[1],"-w")==0) ct = WEAK;
  else if (strcmp(argv[1],"-m")==0) ct = MEDIUM;
  else if (strcmp(argv[1],"-s")==0) ct = STRONG;
  else {
    cerr << "Wrong '-' option\n";
    printusage();
    errmsg("");
  }
 
// Open files
  graphfile.open(argv[2],ios::in);
  if(!graphfile) 
    errmsg("No file named: ",argv[2]);
  wfffile.open("wff",ios::out);
  if(!wfffile) 
    errmsg("No file named: wff");
  desideratafile.open(argv[3],ios::in);
  if(!desideratafile) 
    errmsg("No file named: ",argv[3]);
  if (argc>4) {
    inputnodesfile.open(argv[4],ios::in);
    if(!inputnodesfile) 
      errmsg("No file named: ",argv[4]);
  }

// Read Graph File
  readheader(graphfile,goalnum,relnum);
  
  Goallist = readgoallist(graphfile,goalnum);

  Rellist = readrellist(graphfile,relnum);

  updategoallist(Rellist,relnum,Goallist);
  
  if (argc>4) 
    readinputgoals(inputnodesfile,Goallist,goalnum); 
  else {
    setdefaultinputs(Goallist,goalnum);
    cerr << "No input goals specified, using default choices:\n";
    printinputdefault();
  }
  cerr << "\nInput goals are: \n";
  for(int i=0;i<goalnum;i++) 
    if(Goallist[i].input)
      cerr << "G" << i << " ";
  cerr << endl;

  
// GENERATING WFF
  wffinit(W, 
          8*goalnum,            // variable number
          10*goalnum+           // invariants axioms
          2*relnum*MAXSOURCES+  // relation axioms
          2*relnum+              // relation axioms
          2*goalnum+             // conflict avoid axioms
          4*goalnum*(MAXSOURCES+MAXINCOMING)+ // backward propagation axioms
          2*goalnum             //desiderata
          );
  wffmake(W,goalnum,Goallist,relnum,Rellist,desideratafile,ct);

// PRINT WFF FILE
  prettyprintwff(wfffile,W);
  printwff(wfffile,W);
  
  
// CLOSE FILES
  graphfile.close();
  wfffile.close();
  inputnodesfile.close();
  desideratafile.close();

// INVOKING ZCHAFF
  cerr << "\nCalling the SAT solver Chaff:\n" << chaffcommand << "\n...\n";
  res = system(chaffcommand);
  cerr << "done.\n";

// DECODING THE RESULT
  chaffresultfile.open(chaffresultfilename,ios::in);
  if (chaffresultfile==NULL)
    errmsg(chaffresultfilename,": no such file produced.");
  resultfile.open(resultfilename,ios::out);
  if (resultfile==NULL) 
    errmsg("Cannot open file: ",chaffresultfilename);

  cerr << "\nDecoding the result... ";
  res = chaffresdecode(chaffresultfile,Goallist,goalnum);
  cerr << "done\n\n";
  if (res==0) { // UNSATISFIABLE
    cerr << "++++++++++++++++++++++++++++++++++\n";
    cerr << "+ The problem admits no solution +\n";
    cerr << "++++++++++++++++++++++++++++++++++\n";
  }
  else {
    cerr << "++++++++++++++++++++++++++++++++++\n";
    cerr << "+ The problem admits solutions   +\n";
    cerr << "++++++++++++++++++++++++++++++++++\n";
    printgoalvalues(resultfile,Goallist,goalnum);
    cerr << "File '" << resultfilename << "' produced\n\n";
  }
  return 0;
}

