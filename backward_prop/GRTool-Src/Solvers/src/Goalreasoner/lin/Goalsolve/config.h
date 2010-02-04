#ifndef CONFIG_H
#define CONFIG_H

const int Pathlength = 1000;

// SET TO THE CURRENT CHAFF & MINWEIGHT EXECUTABLE, ABSOLUTE PATH
char chaffcommand[Pathlength] = "./asap";
char minweightcommand[Pathlength] = "./minweight";

// DO NOT MODIFY !!
char wfffilename[] = "wff";
char chaffresultfilename[] = "ChaffResult";
char minweightresultfilename[] = "MinweightResult";
char resultfilename[] = "Result";

#endif
