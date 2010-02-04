#ifndef DECODER_H
#define DECODER_H


const int bufferlength = 1000;

int chaffresdecode(fstream & f,goal * Goallist,int goalnum);
int minweightresdecode(fstream & f,goal * Goallist,int goalnum); 
void printgoalvalues(fstream & resultfile,goal * Goallist,int goalnum);


#endif
