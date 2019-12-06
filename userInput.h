// userInput.h 
// By: Miles Izydorczak
// Date: 30 November 2019
// Purpose: Header file that defines the enum "userInput" which has five values
//          of truth that a question could have depending on what the user 
//          thinks about the question and their secret thing
//

#ifndef _USERINPUT_H_
#define _USERINPUT_H_

enum userInput
{
    yes,
    no,
    prob,
    probnot,
    unknown
};

#endif