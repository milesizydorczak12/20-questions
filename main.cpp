// main.cpp
// By: Miles Izydorczak
// Date: 18 November 2019
// Purpose: Main function for the 20 questions game. Reads in command line
//          arguments and uses them to create an instance of the TwentyQgame 
//          class, then runs the game. 
//

#include <iostream>
#include "TwentyQgame.h"

using namespace std;

int main(int argc, char *argv[])
{
    if (argc != 4) {
        cerr << "Usage: ./20Q version entriesFile questionsFile" << endl;
        exit(1);
    } 

    TwentyQgame game(argv[1], argv[2], argv[3]);
    game.run();

    return 0;
}
