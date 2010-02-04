#include <iostream.h>
#include <fstream.h>
#include <stdlib.h>
#include <string.h>

#include "goal.h"
#include "graph.h"
#include "wff.h"
#include "axioms.h"

extern char * reltypename[];
extern char * vartypeprintvalues[];


// --------------------- INVARIANT AXIOMS --------------------- 
void addinvariant(cnf & w,goalindex g, goal * Goallist) 
{
  literal ts = goal2lit(g,TS,1);
  literal ps = goal2lit(g,PS,1);
  literal td = goal2lit(g,TD,1);
  literal pd = goal2lit(g,PD,1);

  addsimpleimpl(ts,ps,w);
  addsimpleimpl(td,pd,w);

}

void addinvariants(cnf & w,int goalnum, goal * Goallist) 
{
  for (int i=0;i<goalnum;i++) 
    addinvariant(w,i,Goallist);
}

//  --------------------- RELATION AXIOMS --------------------- 
void addrelaxiom_bool(cnf & w,int target, int * sources, int sourcenum, reltype t)
{
  literal ts0 = goal2lit(target,TS,1);
  literal ps0 = goal2lit(target,PS,1);
  literal td0 = goal2lit(target,TD,1);
  literal pd0 = goal2lit(target,PD,1);
  literal * tsall = new literal[sourcenum];
  literal * psall = new literal[sourcenum];
  literal * tdall = new literal[sourcenum];
  literal * pdall = new literal[sourcenum];

  for(int i=0;i<sourcenum;i++) {
    tsall[i] = goal2lit(sources[i],TS,1);
    psall[i] = goal2lit(sources[i],PS,1);
    tdall[i] = goal2lit(sources[i],TD,1);
    pdall[i] = goal2lit(sources[i],PD,1);
  }

  if (t==AND) {
    addgenimpl(tsall,sourcenum,ts0,w);
    addgenimpl(psall,sourcenum,ps0,w);
    for(int i=0;i<sourcenum;i++) {
      addsimpleimpl(tdall[i],td0,w);
      addsimpleimpl(pdall[i],pd0,w);
    }
  }
  else if (t==OR) {
    addgenimpl(tdall,sourcenum,td0,w);
    addgenimpl(pdall,sourcenum,pd0,w);
    for(int i=0;i<sourcenum;i++) {
      addsimpleimpl(tsall[i],ts0,w);
      addsimpleimpl(psall[i],ps0,w);
    }
  }
  else errmsg("Not a boolean relation type");
}

void addrelaxiom_contr(cnf & w,int target, int source, reltype t) 
{
  literal ps1 = goal2lit(target,PS,1);
  literal pd1 = goal2lit(target,PD,1);
  literal ps2 = goal2lit(source,PS,1);
  literal pd2 = goal2lit(source,PD,1);
  switch (t) {
    case PLUSS:  addsimpleimpl(ps2,ps1,w);break;
    case MINUSS: addsimpleimpl(ps2,pd1,w);break;
    case PLUSD:  addsimpleimpl(pd2,pd1,w);break;
    case MINUSD: addsimpleimpl(pd2,ps1,w);break;
  default: errmsg("Not a contribution relation type");
  }
}

void addrelaxiom_fullcontr(cnf & w,int target, int source, reltype t) 
{
  literal ts1 = goal2lit(target,TS,1);
  literal ps1 = goal2lit(target,PS,1);
  literal td1 = goal2lit(target,TD,1);
  literal pd1 = goal2lit(target,PD,1);
  literal ts2 = goal2lit(source,TS,1);
  literal ps2 = goal2lit(source,PS,1);
  literal td2 = goal2lit(source,TD,1);
  literal pd2 = goal2lit(source,PD,1);
  switch (t) {
    case PLUSPLUSS:  
       addsimpleimpl(ts2,ts1,w);
       addsimpleimpl(ps2,ps1,w);break;
    case MINUSMINUSS: 
       addsimpleimpl(ts2,td1,w); 
       addsimpleimpl(ps2,pd1,w);break;
    case PLUSPLUSD:   
       addsimpleimpl(td2,td1,w);
       addsimpleimpl(pd2,pd1,w);break;
    case MINUSMINUSD:  
       addsimpleimpl(td2,ts1,w);  
       addsimpleimpl(pd2,ps1,w);break;
  default: errmsg("Not a full contribution relation type");
  }
}

void addrelaxiom(cnf & w,relation r) 
{
  switch (r.type) {
  case AND: 
  case OR: 
     addrelaxiom_bool(w,r.target,r.sources,r.sourcenum,r.type);
     break;
  case PLUSS:
  case MINUSS:
  case PLUSD:
  case MINUSD:
     addrelaxiom_contr(w,r.target,r.sources[0],r.type);
     break;
  case PLUSPLUSS:
  case MINUSMINUSS:
  case PLUSPLUSD:
  case MINUSMINUSD:
     addrelaxiom_fullcontr(w,r.target,r.sources[0],r.type);
     break;
  default:
    errmsg("Unknown relation type");
  }
}

void addrellistaxioms(cnf & w,relation * Rellist, int relnum) 
{
  for(int i=0;i<relnum;i++) 
    addrelaxiom(w,Rellist[i]);
}

// --------------------- CONSTRAINTS & DESIDERATA AXIOMS --------------------- 
void read_add_constraints(fstream & f, cnf & w) 
{
  clause c;
  while (!f.eof()) {
// cerr << "\nTrying to read clause ...";
    c = readclause(f);
// cerr << "clause read: ";
// prettyprintclause(c);
    addclause(w,c);
  }
}

// --------------------- "AT LEAST ONE SOURCE" AXIOMS ---------------------

//            TS       TD       PS       PD                                 
// --------------------------------------------
// target    -TS0     -TD0     -PS0     -PD0                                         
// input     [ITS0]   [ITD0]   [PS0]    [IPD0]                                               
// AND       /\TSi    \/TDi    /\PSi    \/PDi                                             
// OR        \/TSi    /\TDi    \/PSi    /\PDi                                                  
// ++        TSs               PSs                                             
// --                 TSs               PSs                                      
// **                 TDs               PDs                                      
// //        TDs               PDs                                               
// +                           PSs                                            
// -                                    PSs                                   
// *                                    PDs                                   
// /                           PDs                                            

// given the value Vt of the target and a relation R, 
// returns the value Vs of the source(s) causing Vt via R
// "NO" for none
vartype sourcevartype[10][9] =
{// NO  TS  TD  PS  PD ITS ITD IPS IPD 
   {NO, TS, TD, PS, PD, NO, NO, NO, NO}, // AND
   {NO, TS, TD, PS, PD, NO, NO, NO, NO}, // OR
   {NO, NO, NO, PS, NO, NO, NO, NO, NO}, // +  PLUSS
   {NO, NO, NO, NO, PS, NO, NO, NO, NO}, // -  MINUSS
   {NO, NO, NO, NO, PD, NO, NO, NO, NO}, // *  PLUSD,
   {NO, NO, NO, PD, NO, NO, NO, NO, NO}, // /  MINUSD
   {NO, TS, NO, PS, NO, NO, NO, NO, NO}, // ++ PLUSPLUSS
   {NO, NO, TS, NO, PS, NO, NO, NO, NO}, // -- MINUSMINUSS
   {NO, NO, TD, NO, PD, NO, NO, NO, NO}, // ** PLUSPLUSD
   {NO, TD, NO, PD, NO, NO, NO, NO, NO}  // // MINUSMINUSD
};
                                               
boolean isthereanincomingbooleanrelation(goal g, reltype & t, int & index) 
{
  boolean res=FALSE;
  index = -1;
  for(int i=0;i<g.incomingnum;i++) {
    if (g.incoming[i]->type==AND || g.incoming[i]->type==OR) {
      if (res==TRUE) 
	errmsg("More than one incoming boolean relations in a goal");
      res = TRUE;
      index = i;
      t = g.incoming[i]->type;
    }
  }
  return res;
}

void atleastclause_setup(clause & c, goal g, goalindex gi, vartype t, int extralits)
{
  clauseinit(c,g.incomingnum+1+extralits);
  addliteral(c,goal2lit(gi,t,-1));
}

void add_relsource_lit(clause & c, relation r, vartype t,int sourceindex) 
{
  vartype sourcetype = sourcevartype[int(r.type)][int(t)];
  if (sourcetype!=NO) {
    addliteral(c,goal2lit(r.sources[sourceindex],sourcetype,1));
  }
}

void simple_add_atleastonesource_axiom(cnf & w, goal g, goalindex gi)
{
   clause cts,ctd,cps,cpd;
   atleastclause_setup(cts,g,gi,TS);
   atleastclause_setup(ctd,g,gi,TD);
   atleastclause_setup(cps,g,gi,PS);
   atleastclause_setup(cpd,g,gi,PD);

// cerr << "\nExecuting simple_add_atleastonesource_axiom\n";
// cerr << "G" << gi << ": ";
// printgoal(g);

   for(int i=0;i<g.incomingnum;i++) {
     add_relsource_lit(cts,*(g.incoming[i]),TS);
     add_relsource_lit(cps,*(g.incoming[i]),PS);
     add_relsource_lit(ctd,*(g.incoming[i]),TD);
     add_relsource_lit(cpd,*(g.incoming[i]),PD);
   }

// cerr << "CTS: ";prettyprintclause(cts);
// cerr << "CPS: ";prettyprintclause(cps);
// cerr << "CTD: ";prettyprintclause(ctd);
// cerr << "CPD: ";prettyprintclause(cpd);

  addclause(w,cts);
  addclause(w,cps);
  addclause(w,ctd);
  addclause(w,cpd);
}

void AND_add_atleastonesource_axiom(cnf & w, goal g, goalindex gi,int boolindex)
{
// cerr << "\nExecuting AND_add_atleastonesource_axiom\n";
// cerr << "G" << gi << ": ";
// printgoal(g);
   relation andrel = *(g.incoming[boolindex]);
   int arity =andrel.sourcenum;
   clause * cts = new clause[arity];
   clause * cps = new clause[arity];
   clause ctd,cpd;

   for (int i=0;i<arity;i++) {
     atleastclause_setup(cts[i],g,gi,TS);
     atleastclause_setup(cps[i],g,gi,PS);
   }
   atleastclause_setup(ctd,g,gi,TD,arity-1);
   atleastclause_setup(cpd,g,gi,PD,arity-1);
 
   //DEFINING TS[i] AND PS[i] CLAUSES
  for(int j=0;j<arity;j++){
    for(int i=0;i<g.incomingnum;i++) {
      if (i!=boolindex) {
        add_relsource_lit(cts[j],*(g.incoming[i]),TS);
        add_relsource_lit(cps[j],*(g.incoming[i]),PS);
      }
      else { // i == boolindex
        add_relsource_lit(cts[j],*(g.incoming[i]),TS,j);
        add_relsource_lit(cps[j],*(g.incoming[i]),PS,j);
      }
    }
  }
    
   //DEFINING TD AND PD CLAUSES
   for(int i=0;i<g.incomingnum;i++) {
     if (i!=boolindex) {
       add_relsource_lit(ctd,*(g.incoming[i]),TD);
       add_relsource_lit(cpd,*(g.incoming[i]),PD);
     }
     else { // i == boolindex
       for(int j=0;j<arity;j++){
         add_relsource_lit(ctd,*(g.incoming[i]),TD,j);
         add_relsource_lit(cpd,*(g.incoming[i]),PD,j);
       }
     }
   }

// for(int k=0;k<arity;k++) {
// cerr << "CTS[ " << k << "]: ";prettyprintclause(cts[k]);}
// for(int r=0;r<arity;r++)  {
// cerr << "CPS[ " << r << "]: ";prettyprintclause(cps[r]);}
// cerr << "CTD: ";prettyprintclause(ctd);
// cerr << "CPD: ";prettyprintclause(cpd);

  addclauses(w,cts,arity);
  addclauses(w,cps,arity);
  addclause(w,ctd);
  addclause(w,cpd);
}



void OR_add_atleastonesource_axiom(cnf & w, goal g, goalindex gi,int boolindex) 
{
// cerr << "\nExecuting OR_add_atleastonesource_axiom\n";
// cerr << "G" << gi << ": ";
// printgoal(g);
   relation orrel = *(g.incoming[boolindex]);
   int arity =orrel.sourcenum;
   clause * ctd = new clause[arity];
   clause * cpd = new clause[arity];
   clause cts,cps;

   for (int i=0;i<arity;i++) {
     atleastclause_setup(ctd[i],g,gi,TD);
     atleastclause_setup(cpd[i],g,gi,PD);
   }
   atleastclause_setup(cts,g,gi,TS,arity-1);
   atleastclause_setup(cps,g,gi,PS,arity-1);
 
   //DEFINING TD[i] AND PD[i] CLAUSES
  for(int j=0;j<arity;j++){
    for(int i=0;i<g.incomingnum;i++) {
      if (i!=boolindex) {
        add_relsource_lit(ctd[j],*(g.incoming[i]),TD);
        add_relsource_lit(cpd[j],*(g.incoming[i]),PD);
      }
      else { // i == boolindex
        add_relsource_lit(ctd[j],*(g.incoming[i]),TD,j);
        add_relsource_lit(cpd[j],*(g.incoming[i]),PD,j);
      }
    }
  }
    
   //DEFINING TS AND PS CLAUSES
   for(int i=0;i<g.incomingnum;i++) {
     if (i!=boolindex) {
       add_relsource_lit(cts,*(g.incoming[i]),TS);
       add_relsource_lit(cps,*(g.incoming[i]),PS);
     }
     else { // i == boolindex
       for(int j=0;j<arity;j++){
         add_relsource_lit(cts,*(g.incoming[i]),TS,j);
         add_relsource_lit(cps,*(g.incoming[i]),PS,j);
       }
     }
   }

// for(int k=0;k<arity;k++) {
// cerr << "ctd[ " << k << "]: ";prettyprintclause(ctd[k]);}
// for(int r=0;r<arity;r++)  {
// cerr << "cpd[ " << r << "]: ";prettyprintclause(cpd[r]);}
// cerr << "CTS: ";prettyprintclause(cts);
// cerr << "CPS: ";prettyprintclause(cps);

  addclauses(w,ctd,arity);
  addclauses(w,cpd,arity);
  addclause(w,cts);
  addclause(w,cps);
}
                        
void add_atleastonesource_axiom(cnf & w, goal g, goalindex gi) 
{
   reltype booltype;\
   int boolindex;
   if (isthereanincomingbooleanrelation(g,booltype,boolindex)) {
     if (booltype == AND) 
       AND_add_atleastonesource_axiom(w,g,gi,boolindex);
     else if  (booltype == OR) 
       OR_add_atleastonesource_axiom(w,g,gi,boolindex);
     else
       errmsg("Error: Problem within add_atleastonesource_axiom");
   }
   else 
     simple_add_atleastonesource_axiom(w,g,gi);
}

void add_atleastonesource_axioms(cnf & w, goal * Goallist, int goalnum) 
{
  for (int i=0;i<goalnum;i++) 
    if (Goallist[i].input==FALSE)
       add_atleastonesource_axiom(w,Goallist[i],i);
}


// --------------------- CONFLICT AVOIDING AXIOMS --------------------- 
// WEAK:   -TSg v -TDg
// MEDIUM: -TSg v -PDg and -PSg v -TDg
// STRONG: -PSg v -PDg

void conflictavoid_add_constraint(cnf & w,goalindex gi,conflicttype ct) 
{
  literal ts = goal2lit(gi,TS,1);
  literal ps = goal2lit(gi,PS,1);
  literal td = goal2lit(gi,TD,1);
  literal pd = goal2lit(gi,PD,1);
  switch (ct) {
  case WEAK: 
    addmutex(ts,td,w);
    break;
  case MEDIUM:
    addmutex(ts,pd,w);
    addmutex(ps,td,w);
    break;
  case STRONG:
    addmutex(ps,pd,w);
    break;
  default : errmsg("ERROR: Wrong type of conflict");
  }
}

void conflictavoid_add_constraints(cnf & w, goal * Goallist, int goalnum,conflicttype ct) 
{
  if (ct!=NOCONFLICT) 
    for (int i=0;i<goalnum;i++) 
      conflictavoid_add_constraint(w,i,ct);
}

// --------------------- DEFAULT SETTING OF INPUTS --------------------- 

void setdefaultinputs(goal * Goallist,int goalnum) 
{
  reltype booltype;
  int boolindex;
  for (int i=0;i<goalnum;i++){
    if (!isthereanincomingbooleanrelation(Goallist[i],booltype,boolindex)) {
      Goallist[i].input = TRUE;
    }
  }
}

//USEFUL ONLY FOR MIN SAT
void setdefaultweights(goal * Goallist,int goalnum) 
{
   for (int i=0;i<goalnum;i++){
     if (Goallist[i].input == TRUE) { 
      Goallist[i].weights[TS]=defaultTSweight-defaultPSweight; // NOTICE: TS->PS 
      Goallist[i].weights[PS]=defaultPSweight;       
      Goallist[i].weights[TD]=defaultTDweight-defaultPDweight; // NOTICE: TD->PD
      Goallist[i].weights[PD]=defaultPDweight;
     }
   }
}



// --------------------- WFF MAKE --------------------- 
void wffmake (cnf & w,int goalnum, goal * Goallist, int relnum, relation * Rellist,fstream & desideratafile,conflicttype ct) 
{
  cerr << "\nBuilding the SAT Formula... \n";
  cerr << "- adding invariants... ";
  addinvariants(w,goalnum,Goallist);
  cerr << "done\n- adding relation axioms... ";
  addrellistaxioms(w,Rellist,relnum);
  cerr << "done\n- adding backward propagation axioms... ";
  add_atleastonesource_axioms(w,Goallist,goalnum);
  cerr << "done\n- adding users' desiderata and constraint axioms... ";
  read_add_constraints(desideratafile,w);
  cerr << "done\n- adding conflict avoiding axioms... ";
  conflictavoid_add_constraints(w,Goallist,goalnum,ct);
  cerr << "done\ndone\n";
}

                                                                        
                                                                        
   
