#file: makefile
# Hodaya Koslowsky 313377673

a.out: ex2.o jobList.o
	gcc ex2.o jobList.o

ex2.o: ex2.c ex2.h jobList.h
	gcc -c ex2.c

jobList.o: jobList.c ex2.h jobList.h
	gcc -c jobList.c

