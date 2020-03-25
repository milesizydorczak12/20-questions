// KDtree.h
// By: Miles Izydorczak
// Date: 19 November 2019
// Purpose: Interface of the KDtree class. This class contains a tree with two
//          ways of accessing. One is the root Node and the other is a current
//          Node which can be advanced and reset as needed to traverse the tree
//          The rest of this class's functionality goes into different ways to
//          construct this tree.
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

    void advance(bool yes);
    void advance_probably(userInput input);
    bool return_to_probably();

    int size_of_pool();
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

    Node *root;
    Node *curr;

    struct Probably {
        bool yes;
        Node *prob_ptr;
    };

    struct Unknown {
        bool yes;
        Node *unkwn_ptr;
    };

    Stack<Probably> probablies;
    Stack<Unknown> unknowns;

    void add_probably(bool yes);
    void add_unknown();
    void set_question_truth(string questionStr, userInput UI);
    void skip_answered_questions();

    void make_sample_set(vector<int> &sample_set, vector<int> &pool);

    int select_best_question(vector<int> pool);
    int select_best_sum(vector<int> q_sums, size_t pool_size);

    Node *build_tree(double version);
    Node *delete_tree(Node *node);
    Node *build_tree_order(vector<int> pool, int q_num);
    Node *build_tree_best(vector<int> pool);
    Node *build_tree_sample(vector<int> pool);
    Node *build_tree_sums(vector<int> pool);
    Node *build_tree_sums(vector<int> pool, vector<int> q_sums);
    Node *make_node(vector<int> pool, int q_index);
    Node *make_node(int data);
};

#endif

