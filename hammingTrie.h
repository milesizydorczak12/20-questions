// hammingTrie.h
// By: Miles Izydorczak
// Date: 3 December 2019
// Purpose: Trie of the binary answers to each question of each answer. Used to
//          determine K nearest neighbors of an entry: if we guess an entry and
//          the player says that is not what they were thinking of, we 
//          determine all other entries that have a hamming distance of one 
//          from that tree, to ask other entries that are similar enough to the
//          entry that they guessed such that there is only a single question
//          that separates the two entries
//

#include <iostream>
#include <vector>
#include "questions_and_entries.h"
#include "Stack.h"

using namespace std;

#ifndef _HAMMINGTRIE_H_
#define _HAMMINGTRIE_H_

class hammingTrie {
public:

    hammingTrie(vector<Entry> &entries);

    ~hammingTrie();

    void find_neighbors_of(string point_name);
    void find_neighbors_of(int entry_index);

    string get_a_neighbor();



private:

    vector<Entry> entriesVec;

    struct Node {
        string name;
        Node *one;
        Node *zero;
    };

    Node *root;

    Node *make_node();

    Node *post_order_delete(Node *node);

    void add_point(vector<int> answers, string point_name);
    void build_trie();
    void find_second_degree_of(vector<int> answers);
    void find_first_degree_of(vector<int> answers);
    int switch_value(int x);
    string neighbor_name(vector<int> answers);

    Stack<string> neighbors;


};

#endif
