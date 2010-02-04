#ifndef AXIOMS_H
#define AXIOMS_H


// Default weights 
// (goalminsolve only)
const int defaultTSweight=2;
const int defaultTDweight=2;
const int defaultPSweight=1;
const int defaultPDweight=1;


// WEAK:   -TSg v -TDg
// MEDIUM: -TSg v -PDg and -PSg v -TDg
// STRONG: -PSg v -PDg
enum conflicttype {NOCONFLICT,WEAK,MEDIUM,STRONG};

// adds invariant axioms
void addinvariant(cnf & w,goalindex g, goal * Goallist); 
void addinvariants(cnf & w,int goalnum, goal * Goallist); 

// adds relation axioms
void addrelaxiom_bool(cnf & w,int target, int * sources, int sourcenum, reltype t);
void addrelaxiom_contr(cnf & w,int target, int source, reltype t); 
void addrelaxiom_fullcontr(cnf & w,int target, int source, reltype t); 
void addrelaxiom(cnf & w,relation r); 
void addrellistaxioms(cnf & w,relation * Rellist, int relnum);

// add constraints & desiderata axioms
void read_add_constraints(fstream & f, cnf & w); 

// add "at least one source" axioms
boolean isthereanincomingbooleanrelation(goal g, reltype & t, int & index); 
void atleastclause_setup(clause & c, goal g, goalindex gi, vartype t, int extralits = 0); 
void add_relsource_lit(clause & c, relation r, vartype t,int sourceindex=0); 
void simple_add_atleastonesource_axiom(cnf & w, goal g, goalindex gi); 
void AND_add_atleastonesource_axiom(cnf & w, goal g, goalindex gi,int boolindex); 
void OR_add_atleastonesource_axiom(cnf & w, goal g, goalindex gi,int boolindex); 
void add_atleastonesource_axiom(cnf & w, goal g, goalindex gi); 
void add_atleastonesource_axioms(cnf & w, goal * Goallist, int goalnum); 

// add conflict avoiding axioms
void conflictavoid_add_constraint(cnf & w,goalindex gi,conflicttype ct); 
void conflictavoid_add_constraints(cnf & w, goal * Goallist, int goalnum,conflicttype ct); 

// handle the default choice for input goals
void setdefaultinputs(goal * Goallist,int goalnum); 
// handle the default choice for weights to the input goals (goalminsolve only )
void setdefaultweights(goal * Goallist,int goalnum);

// general wff-make
void wffmake (cnf & w,int goalnum, goal * Goallist, int relnum, relation * Rellist,fstream & desideratafile,conflicttype ct); 

#endif


