// TwentyQgame.h
// By: Miles Izydorczak
// Date: 18 November 2019
// Purpose: Implementation barrier of the TwentyQGame class. Provides 
//          procedural abstraction by seperating the functions that the client 
//          is allowed to call from the helper functions and private data 
//          members used to run them. Populates two vectors, one of the entries
//          Things that we guess) and the other of the questions that the 
//          computer will ask. Depending on the version of the program 
//          requested, we will call the correct version, passing in a reference
//          to these two vectors
//
//
#include <vector>
#include "KDtree.h"
#include "questions_and_entries.h"
#include "userInput.h"
#include "hammingTrie.h"

using namespace std;

const string ACKNOWLEDGEMENTS = "This project is the independent project "
                                "designed and written by Miles Izydorczak"
                                "for his data structures class at Tufts "
                                "University. It was made possible thanks \nto "
                                "the mentoring done by TAs Lawrence Chan and "
                                "Kevin Destin. Special thanks to \nProfessors "
                                "Mark Sheldon and Matias Korman and the rest "
                                "of the Tufts computer \nscience department.";

#ifndef _20Q_GAME_H_
#define _20Q_GAME_H_

class TwentyQgame {
public:

	TwentyQgame(string version, string entriesFile, string qsFile);

    void run();

private:

    vector<Question> questionsVec;
    vector<Entry> entriesVec;

    double version_num;
    string prompt;

    void read_entries(string entriesFile);
    void read_questions(string questionsFile);
    void set_version_num(string version);

    void run_welcome();

    void run_proto();
    void run_prob();
    void run_neighbors();
    void run_hybrid();
    void run_neural();

    void guess_neighbors(string point, int guesses_so_far);

    userInput string_to_UI(string s);

};


#endif

