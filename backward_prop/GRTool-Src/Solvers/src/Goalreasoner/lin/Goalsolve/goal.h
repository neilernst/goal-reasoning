#ifndef GOAL_H
#define GOAL_H


// CONSTANTS: 

const int MAXINCOMING = 30;  // max num of incoming relations for a goal
const int MAXSOURCES = 20;  // max num of source goals for a relation
const int TKLENGTH = 100;

// TYPES: 

enum boolean {FALSE,TRUE}; // boolean values
enum retval {FAIL,OK};    // fail/success value
enum val {N,P,T};         // indifferent/partial/total
enum sdval {SAT,DEN};     // satisfiability/deniability
enum reltype {AND,OR,PLUSS,MINUSS,PLUSD,MINUSD,PLUSPLUSS,MINUSMINUSS,PLUSPLUSD,MINUSMINUSD,NOREL}; //type of relation

typedef int goalindex;

struct relation 
{
  reltype type;
  int target;              // index of target goal
  int sourcenum;
  goalindex sources[MAXSOURCES+1]; // indexes of source goals
};

struct goal 
{
  bool input;
  val sat;
  val den;
  val Isat;
  val Iden;
  int incomingnum;
  relation * incoming[MAXINCOMING];
  //weight of the values corresponding to {NO,TS,TD,PS,PD,ITS,ITD,IPS,IPD};
  int weights[9];
};




/* FUNCTIONS */
/* I/O: */

void errmsg(char msg[]);
void errmsg(char msg[],char msg1[]);

val readval (fstream & f);
goal readgoal (fstream & f);
void printgoal (fstream & f, goal & g);
void printgoal (goal & g);
void updategoal (goal & g,relation & r);

reltype readreltype (fstream & f);
relation readrelation (fstream & f);
void printrelation (fstream & f, relation & r);
void printrelation (relation & r);


 #endif

