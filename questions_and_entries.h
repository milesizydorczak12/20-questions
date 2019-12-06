// questions_and_entries.h
// By: Miles Izydorczak
// Date: 18 November 2019
// Purpose: Contains struct definition for an Entry which will be used by 
//          numerous other objects and files
//
#include <vector>
#include "userInput.h"

using namespace std;

#ifndef _QUESTIONS_AND_ENTRIES_H_
#define _QUESTIONS_AND_ENTRIES_H_

struct Entry {
    string name;
    vector<int> answers;
};

struct Question {
    userInput truth;
    string question;
};

#endif
