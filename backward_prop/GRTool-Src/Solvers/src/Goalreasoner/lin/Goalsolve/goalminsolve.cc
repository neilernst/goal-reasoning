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
#include "weights.h"


// -------------------------- FUNCTIONS -------------------------- 

void printinputdefault() 
{
 cerr << "REMARKS:\n";
 cerr << "1) if no <inputnodefile> is provided, then  by default\n";
 cerr << "all goals with no incoming boolean relation are considered as input nodes\n";
 cerr << "2) if no <weightfile> is provided, then  by default\n";
 cerr << "all input goals are assigned the following default weights: ITS,ITD=2,IPS,IPD=1, all others = 0\n";
}

void printusage() 
{
  cerr << "Usage:\n";
  cerr << "goalminsolve -n|-w|-m|-s <graphfile> <desideratafile> [<inputnodefile> [<weightfile>]]\n\n";
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
  fstream weightfile;
  fstream wfffile;
  fstream minweightresultfile;
  fstream resultfile;
  
  goal * Goallist;
  relation * Rellist;
  cnf W;
  int goalnum,relnum;  
  int res;
  char c;
  goal g;
  conflicttype ct = NOCONFLICT;

//define paths and minweight execution script 
  strcat(minweightcommand," < ");
  strcat(minweightcommand,wfffilename);
  strcat(minweightcommand," > ");
  strcat(minweightcommand,minweightresultfilename);
// cerr << "Execution:" << minweightscript << endl;

// Checking command line
  if (argc<4 || argc>6) {
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
    if (argc>5) {
      weightfile.open(argv[5],ios::in);
      if(!weightfile) 
        errmsg("No file named: ",argv[5]);
    }
  }
  
// Read Graph File
  readheader(graphfile,goalnum,relnum);
  
  Goallist = readgoallist(graphfile,goalnum);

  Rellist = readrellist(graphfile,relnum);

  updategoallist(Rellist,relnum,Goallist);
  
  if (argc>4) {
    readinputgoals(inputnodesfile,Goallist,goalnum); 
    if (argc>5) {
      readweights(weightfile,Goallist,goalnum); 
    }
    else {
      setdefaultweights(Goallist,goalnum);
    }
  }
  else {
    setdefaultinputs(Goallist,goalnum);
    cerr << "No input goals specified, using default choices:\n";
    printinputdefault();
    setdefaultweights(Goallist,goalnum);
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
  prettyprintweights(wfffile,Goallist,goalnum);  
  printwff(wfffile,W);
  printweights(wfffile,Goallist,goalnum);
  
// CLOSE FILES
  graphfile.close();
  wfffile.close();
  inputnodesfile.close();
  desideratafile.close();

// INVOKING MIDWEIGHT
  cerr << "\nCalling the min-weight SAT solver MinWeight:\n" << minweightcommand << "\n...\n";
  res = system(minweightcommand);
  cerr << "done.\n";

// DECODING THE RESULT
  minweightresultfile.open(minweightresultfilename,ios::in);
  if (minweightresultfile==NULL)
    errmsg(minweightresultfilename,": no such file produced.");
  resultfile.open(resultfilename,ios::out);
  if (resultfile==NULL) 
    errmsg("Cannot open file: ",minweightresultfilename);

  cerr << "\nDecoding the result... ";
  res = minweightresdecode(minweightresultfile,Goallist,goalnum);
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

