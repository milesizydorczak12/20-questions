# Makefile for 20 questions project
# By: Miles Izydorczak
# Date: 18 November 2019
# Purpose: Makes the executables that are needed for my program to function. 
#          Contains target executable and object files, with the dependencies
#          and recipies that they need in order to compile
#

CXX = clang++
CXXFLAGS = -g -Wall -Wextra -std=c++11
LDFLAGS = -g

20Q: main.o TwentyQgame.o KDtree.o hammingTrie.o
	${CXX} ${LDFLAGS} -o 20Q main.o TwentyQgame.o KDtree.o hammingTrie.o

#Prototype: main.o Prototype.o 
#	${CXX} ${LDFLAGS} -o Prototype main.o Prototype.o 

main.o: main.cpp TwentyQgame.h # questions_and_entries.h Stack.h
TwentyQgame.o: TwentyQgame.cpp TwentyQgame.h hammingTrie.h#questions_and_entries.h userInput.h KDtree.h
# KDtree.h 
KDtree.o: KDtree.cpp KDtree.h #questions_and_entries.h Stack.h userInput.h
hammingTrie.o: hammingTrie.cpp hammingTrie.h
#Prototype.o: Prototype.cpp Prototype.h questions_and_entries.h Stack.h

clean: 
	rm -rf 20Q *.o 


