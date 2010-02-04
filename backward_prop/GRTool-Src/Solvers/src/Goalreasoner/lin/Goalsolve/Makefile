all: goalsolve

goalsolve: axioms.o decoder.o goal.o goalsolve.o graph.o  wff.o
	g++ axioms.o decoder.o goal.o goalsolve.o graph.o  wff.o -o goalsolve

goalminsolve: axioms.o decoder.o goal.o goalminsolve.o weights.o graph.o  wff.o
	g++ axioms.o decoder.o goal.o goalminsolve.o graph.o weights.o wff.o -o goalminsolve

%.o : %.c
	g++ -c  $< -o $@

clean: 
	rm *.o goalsolve goalminsolve