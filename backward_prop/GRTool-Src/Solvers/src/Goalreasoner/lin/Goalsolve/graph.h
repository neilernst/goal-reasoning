#ifndef GRAPH_H
#define GRAPH_H


const int MAXCONSTRAINLENGTH = 1000;

void readheader (fstream & f, int & goalnum, int & relnum); 
goal * readgoallist (fstream & f, int goalnum); 
void printgoallist (fstream & f, goal * Goallist, int goalnum);
void updategoallist(relation * Rellist,int relnum, goal * Goallist); 
relation * readrellist (fstream & f, int relnum); 
void printrellist (fstream & f, relation * Rellist, int relnum);
void readinputgoals(fstream & f, goal * Goallist,int goalnum); 


#endif
