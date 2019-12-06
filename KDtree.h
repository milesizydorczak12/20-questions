// KDtree.h
// By: Miles Izydorczak
// Date: 19 November 2019
// Purpose: Proof of concept of the 20 questions game, header file. Same as the
//			eventual final version with the exception that there is a very
//			small data set, and we are optimizing each question so that we 
//          don't need to find the most polarizing questioncto divide the data
//			with.
//

#include <vector>
#include "questions_and_entries.h"
#include "Stack.h"
#include "userInput.h"

using namespace std;

#ifndef _KDTREE_H_
#define _KDTREE_H_

class KDtree {
public:

    KDtree(vector<Entry> &eVec, vector<Question> &qVec, double vers);

    ~KDtree();

    void print_input();

    void reset();

    // int advance(bool yes);
    void advance(bool yes);
    int size_of_pool();


    void advance_probably(userInput input);
    bool return_to_probably();
    string entry_at_curr();
    string question_at_curr();


private:

    vector<Question> questions;
    vector<Entry> entries;

    void print_Qs();
    void print_Es();

    struct Node {
        string question;
        vector<int> correct_entries;
        Node *yes;
        Node *no;
    };

    struct Probably {
        bool yes;
        Node *prob_ptr;
    };

    struct Unknown {
        bool yes;
        Node *unkwn_ptr;
    };

    void add_probably(bool yes);
    void add_unknown();
    void set_question_truth(string questionStr, userInput UI);
    void skip_answered_questions();

    Node *root;
    Node *curr;

    void make_sample_set(vector<int> &sample_set, vector<int> &pool);

    int select_best_question(vector<int> pool);
    int select_best_sum(vector<int> q_sums, int pool_size);

    Node *make_node(vector<int> pool, int q_index);
    Node *make_node(int data);

    Stack<Probably> probablies;
    Stack<Unknown> unknowns;

    Node *build_tree(double version);

    Node *delete_tree(Node *node);
    Node *build_tree_order(vector<int> pool, int q_num);
    Node *build_tree_best(vector<int> pool);
    Node *build_tree_sample(vector<int> pool);
    Node *build_tree_sums(vector<int> pool);
    Node *build_tree_sums(vector<int> pool, vector<int> q_sums);
    //Node *build_tree_4(vector<int> pool);
};

#endif




























