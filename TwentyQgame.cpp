// TwentyQgame.cpp
// By: Miles Izydorczak
// Date: 18 November 2019
// Purpose: Run the 20 questions game given the files containing entries and 
//          questions data and the desired version number. This is done in its
//          own class rather than the main file to increase modularity, in case
//          someone wanted to add onto the program (developing a way to create 
//          questions and entries, etc.).
//

#include <vector>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <sstream>
#include "KDtree.h"
#include "TwentyQgame.h"
#include "questions_and_entries.h"
#include "userInput.h"
#include "hammingTrie.h"

using namespace std;

// Constructor TwentyQgame
// Does: Sets up a game of twenty questions by calling functions that will read
//       in all of the questions and entry names so that the program knows 
//       from which set of information it is guessing from and what questions
//       it can use to guess them. It sets the version number as well,
//       in accordance to the given version parameter, so that for the duration
//       of the program, we know which guessing structure to make and which
//       functions to use to play the actual game.
TwentyQgame::TwentyQgame(string version, string entriesFile, string qsFile)
{
    read_entries(entriesFile);
    read_questions(qsFile);
    set_version_num(version);
}

// Function run
// Parameters: None
// Returns:    None
// Does:       Using the version_num data member, calls the appropriate run
//             function.
void TwentyQgame::run() 
{
    run_welcome();
    if (version_num < 2.0) {
        run_proto();   
    } else if (version_num == 2.0) {
        run_prob();
    } else if (version_num == 3.0) {
        run_neighbors();
    } else if (version_num == 4.0) {
        run_hybrid();
    } else {
        run_neural();
    }
}

// Function run_welcome
// Parameters: None
// Returns:    None
// Does:       Prints an instructional message to standard output, the contents
//             of which vary in accordance to the version which we are running
//             Then waits for the user to enter B to begin as they think of 
//             an entry for the program to guess, printing acknowledgements if
//             they are asked for.
void TwentyQgame::run_welcome() 
{
    if (version_num != 2.0) {
        cout << "Welcome to 20 questions! Please think of a(n)\n" 
             << prompt << endl 
             << "Respond to each of the following questions with (y/n)\n";
    } else if (version_num == 2.0) {
        cout << "Welcome to 20 questions! Please think of a(n)\n"
             << prompt << endl
             << "Respond to each of the following questions with "
                "(y/n/prob/probnot/unknown)\n";
    }

    char command;
    bool repeat = true;
    while (repeat) {
        cout << "Type B to begin or I for more information" << endl;
        cin >> command;
        if (command == 'B') {
            return;
        } else if (command == 'I') {
            cout << ACKNOWLEDGEMENTS << endl;
        }
    }
}

// Function run_proto()
// Parameters: None
// Returns:    None
// Does:       Navegates the tree structure made from an instance of the KDtree
//             class by asking for a character from standard input, either 'y'
//             or 'n', then advances in that direction until there is just one
//             entry that it has narrowed the pool to. It asks if the user was
//             thinking of that entry or not. This prototype version does
//             nothing to account for the player making a mistake or possible
//             dissent among what a question entails
void TwentyQgame::run_proto()
{
    KDtree proto(entriesVec, questionsVec, version_num);
    char command;

    while (proto.size_of_pool() > 1) {
        cout << proto.question_at_curr() << endl;
        cin >> command;
        if (command == 'y' or command == 'n') {
            proto.advance(command == 'y');
        }
    }

    cout << "Are you thinking of " << proto.entry_at_curr() << "?" << endl;
    cin >> command;
    if (command == 'y') {
        cout << "I win!!!" << endl;
    } else {
        cout << "You win, I am stumped..." << endl;
    }
}

// Function run_neighbors
// Parameters: None
// Returns:    None
// Does:       Runs in an identical fashion to run_prob, execept here it tracks
//             the number of questions that it has asked so it doesn't go over
//             the maximum of 20. When it reaches a single entry that it could
//             be, it guesses it and if the user tells the program that that 
//             was an incorrect guess, it calls the guess_neighbors function
//             on that guess, in order to guess similar data points to that 
//             point
void TwentyQgame::run_neighbors()
{
    KDtree neigh(entriesVec, questionsVec, version_num);
    char command;
    int guesses_made = 0;

    // Stop before question 20 because we want to make a guess and that counts
    while (neigh.size_of_pool() > 1 and guesses_made < 19) {
        cout << (guesses_made + 1) << ". " << neigh.question_at_curr() << endl;
        cin >> command;
        if (command == 'y' or command == 'n') {
            neigh.advance(command == 'y');
            guesses_made++;
        }
    }

    cout << (guesses_made + 1) << ". Are you thinking of " 
         << neigh.entry_at_curr() << "?" << endl;
    cin >> command;
    guesses_made++;
    if (command == 'y') {
        cout << "I win!!!" << endl;
    } else if (guesses_made < 20) {
        // if that was our last guess left, there is no need to ask the 
        // neighbors
        guess_neighbors(neigh.entry_at_curr(), guesses_made);
    }
}

// Function guess_neighbors
// Parameters: A string, the name of an entry that we will attempt to find the
//             neighbors of and an integer guesses_so_far so that we know how
//             many of the neighbors we can guess
// Returns:    None
// Does:       Uses an instance of the hammingTrie class to find all entries 
//             that have a hamming distance of 1 from the binary list that is 
//             the given entry's answers vector. Essentially we give the player
//             a little slack and allow them to have made one error or dissent
//             of judgement when answering the questions. If it hasn't guessed
//             it yet and there is still guesses left to be made, it will begin
//             to guess entries that are two responses off.
void TwentyQgame::guess_neighbors(string point, int guesses_so_far)
{
    char command;
    hammingTrie neighborhood(entriesVec);
    // find_neighbors_of makes a stack in which the highest priority elements 
    // are the one degree separated data points and the lower priority elements
    // are the two degree separated data points
    neighborhood.find_neighbors_of(point);
    string guess = neighborhood.get_a_neighbor();

    while (guesses_so_far < 20 and guess != "No more neighbors") {
        cout << (guesses_so_far + 1) << ". Are you thinking of " << guess
             << "?" << endl;
        cin >> command;
        if (command == 'y') {
            cout << "I win!!!" << endl;
            return;
        }
        guess = neighborhood.get_a_neighbor();
        guesses_so_far++;
    }
    cout << "You win, I am stumped..." << endl;
}

// Function run_prob
// Parameters: None
// Returns:    None
// Does:       Gameplay loop of the 2.0 version of the program where instead of
//             merely yes or no, the user can enter probably, probably not, or
//             unknown to give the program a better perspective on how 
//             polarizing that question was. If it reaches a dead end, where it
//             has narrowed it down to just one entry, and the human player is
//             telling it "no, thats not what I'm thinking of", the program
//             can return to the state it was before a question was answered
//             unknown or probably and try the opposite side of that question
//             while rettaining the knowledge that it gained about definite yes
//             and no answers, so that it won't re-ask the same question
void TwentyQgame::run_prob()
{
    KDtree prob(entriesVec, questionsVec, version_num);
    string command;
    int guesses_made = 0;
    while (guesses_made < 20) {
        cout << (guesses_made + 1) << ". " << prob.question_at_curr() << endl;
        guesses_made++;
        cin >> command;
        prob.advance_probably(string_to_UI(command));
        // We need a second while loop so when we have a probably that divides
        // two pools of size 1, there is no question to ask when we return 
        while (prob.size_of_pool() <= 1 and guesses_made < 20) {
            cout << (guesses_made + 1) << ". " 
                 << "Are you thinking of " << prob.entry_at_curr() << "?\n";
            guesses_made++;
            cin >> command;
            if (command == "y") {
                cout << "I win!!!" << endl;
                return;
            } else if (not prob.return_to_probably()) {
                // If there is no probably or unknown to return to and we 
                // didn't guess correctly, then the player wins
                cout << "You win! I'm stumped." << endl;
                return;
            } 
        }
    } 
    cout << "You win! I'm stumped." << endl;
}


// I want to continue adding on to this project over winter break, so I left 
// room for two more version ideas: one could be a hybrid between versions 2.0
// and 3.0, and the other could be a sort of nueral network that changes the 
// text files as it learns how people answer questions

void TwentyQgame::run_hybrid()
{
    // TODO
}
    
void TwentyQgame::run_neural()
{
    // TODO
}

// function read_entries
// Parameters: A string, which should be the name of a file in the current
//             directory, this is the text file from which we will read the 
//             entries and their answers to each question. 
// Returns:    None
// Does:       Opens the file (exiting if the file does not exist), reads one
//             pair of lines at a time, the first is stored as the name of the
//             entry (a string), the second line is read in one integer at a 
//             time. These will be stored as the answers in the answers vector
//             of the entry.
void TwentyQgame::read_entries(string entriesFile)
{
    ifstream entries_in;
    entries_in.open(entriesFile);
    if (not entries_in.is_open()) {
        cerr << "Error opening entriesFile: " << entriesFile << endl;
        exit(1);
    }

    int answer;
    string name, answers;
    while (getline(entries_in, name)) {
        Entry new_entry;
        new_entry.name = name;
        // Make a string stream from the second line so that we can parse it 
        // for one integer at a time and store them individually in a vector
        getline(entries_in, answers);
        stringstream s(answers);
        while (s >> answer) {
            new_entry.answers.push_back(answer);
        }
        entriesVec.push_back(new_entry);
    }

    entries_in.close();
}

// function read_questions
// Parameters: A string, questionsFile, which is the name of the text file that
//             we will open and read from
// Returns:    None (only modifies the questionsVec vector)
// Does:       First opens the given file as an input file stream, printing an
//             error message and crashing if the given file could not be 
//             opened. Then uses getline to read each line in the text file in 
//             storing that line as the string element of a Question in the 
//             questions vector. Initializes the truth element of each 
//             Question that it adds to the vector as "Unknown"
void TwentyQgame::read_questions(string questionsFile)
{
    ifstream questions_in;
    questions_in.open(questionsFile);
    if (not questions_in.is_open()) {
        cerr << "Error opening questionsFile: " << questionsFile << endl;
        exit(1); 
    }

    getline(questions_in, prompt);
    string questionStr;
    while (getline(questions_in, questionStr)) {
        Question new_question;
        new_question.question = questionStr;
        new_question.truth = unknown;
        questionsVec.push_back(new_question);
    }

    questions_in.close();
}

// Function set_version_num
// Parameters: a string, version, this was one of the three additional command
//             line arguments that were given with the executable
// Returns:    None
// Does:       Sets the TwentyQgame's version_num private value to the 
//             appropriate double given a string that tells us which version
//             of the game we want to play or test
void TwentyQgame::set_version_num(string version)
{
    if (version == "-v0.0") {
        version_num = 0.0;
    } else if (version == "-v1.0") {
        version_num = 1.0;
    } else if (version == "-v1.1") {
        version_num = 1.1;
    } else if (version == "-v1.5") {
        version_num = 1.5;
    } else if (version == "-v2.0") {
        version_num = 2.0;
    } else if (version == "-v3.0") {
        version_num = 3.0;
    } else if (version == "-v4.0") {
        version_num = 4.0;
    } else if (version == "-v5.0") {
        version_num = 5.0;
    } else {
        cerr << "Version number: " << version << " not supported" << endl;
        exit(1);
    }
}

// Function string_to_UI
// Parameters: A string, s, which we will convert to type userInput, an enum 
//             declared and defined in the userInput header file
// Returns:    A value of type userInput 
// Does:       Uses conditional statements to turn the given type string (from
//             standard input/cin usually) into the more manageable enum type
//             userInput
userInput TwentyQgame::string_to_UI(string s)
{
    if (s == "y") {
        return yes;
    } else if (s == "n") {
        return no;
    } else if (s == "prob") {
        return prob;
    } else if (s == "probnot") {
        return probnot;
    } else {
        return unknown;
    }
}
