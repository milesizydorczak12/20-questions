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

main.o: main.cpp TwentyQgame.h 
TwentyQgame.o: TwentyQgame.cpp TwentyQgame.h hammingTrie.h
KDtree.o: KDtree.cpp KDtree.h 
hammingTrie.o: hammingTrie.cpp hammingTrie.h

clean: 
	rm -rf 20Q *.o 


