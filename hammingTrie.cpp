// hammingTrie.cpp
// By: Miles Izydorczak
// Date: 3 December 2019
// Purpose: Implementation of the hammingTrie class
//

#include <iostream>
#include <vector>
#include "questions_and_entries.h"
#include "Stack.h"
#include "hammingTrie.h"

using namespace std;

// Constructor hammingTrie
hammingTrie::hammingTrie(vector<Entry> &entries)
{
    entriesVec = entries;

    root = new Node;
    root->name = "root";
    root->one = nullptr;
    root->zero = nullptr;
    build_trie();
}

// Destructor hammingTrie
hammingTrie::~hammingTrie()
{
    root = post_order_delete(root);
}


hammingTrie::Node *hammingTrie::post_order_delete(Node *node)
{
    if (node == nullptr) {
        return nullptr;
    } else {
        node->zero = post_order_delete(node->zero);
        node->one = post_order_delete(node->one);
        delete node;
        return nullptr;
    }
}

string hammingTrie::get_a_neighbor()
{
    if (not neighbors.is_empty()) {
        string to_return = neighbors.top();
        neighbors.pop();
        return to_return;
    } else {
        return "No more neighbors";
    }
}

hammingTrie::Node *hammingTrie::make_node()
{
    Node *one_node = new Node;
    one_node->name = " ";
    one_node->zero = nullptr;
    one_node->one = nullptr;
    return one_node;
}


void hammingTrie::build_trie()
{
    for (size_t i = 0; i < entriesVec.size(); i++) {
        add_point(entriesVec[i].answers, entriesVec[i].name); 
    }
}

void hammingTrie::add_point(vector<int> answers, string point_name) 
{
    Node *curr = root;

    for (size_t i = 0; i < answers.size(); i++) {
        if (answers[i] == 0) {
            if (curr->zero == nullptr) {
                curr->zero = make_node();
            }
            curr = curr->zero;
        } else {
            if (curr->one == nullptr) {
                curr->one = make_node();
            }
            curr = curr->one;
        }
    }

    curr->name = point_name;
}

void hammingTrie::find_neighbors_of(string point_name) 
{
    for (size_t i = 0; i < entriesVec.size(); i++) {
        if (entriesVec[i].name == point_name) {
            // find neighbors of that point
            find_second_degree_of(entriesVec[i].answers);
            find_first_degree_of(entriesVec[i].answers);
        }
    }
}

void hammingTrie::find_neighbors_of(int entry_index)
{
    find_second_degree_of(entriesVec[entry_index].answers);
    find_first_degree_of(entriesVec[entry_index].answers);
}


//second degree
// void hammingTrie::find_neighbors_of(vector<int> answers)
// {
//     // put a -1 after size()
//     for (size_t i = 0; i < answers.size(); i++) {
//         answers[i] = switch_value(answers[i]);
//             string name_in_trie = neighbor_name(answers);
//             if (name_in_trie != " ") {
//                 neighbors.push(name_in_trie);
//             } 

//         for (size_t j = i + 1; j < answers.size(); j++) {
//              //if (i != j) {
//                  answers[j] = switch_value(answers[j]);
//              //}

//             name_in_trie = neighbor_name(answers);
//             if (name_in_trie != " ") {
//                 neighbors.push(name_in_trie);
//             } 
//              //if (i != j) {
//                  answers[j] = switch_value(answers[j]);
//              //}
//         }
//         answers[i] = switch_value(answers[i]);
//     }
// }

void hammingTrie::find_second_degree_of(vector<int> answers)
{
    for (int i = 0; i < static_cast<int>(answers.size()) - 1; i++) {
        answers[i] = switch_value(answers[i]);

        for (int j = i + 1; j < static_cast<int>(answers.size()); j++) {
            answers[j] = switch_value(answers[j]);

            string name_in_trie = neighbor_name(answers);
            if (name_in_trie != " ") {
                neighbors.push(name_in_trie);
            } 

            answers[j] = switch_value(answers[j]);
        }
        answers[i] = switch_value(answers[i]);
    }
}

void hammingTrie::find_first_degree_of(vector<int> answers) 
{
    for (size_t i = 0; i < answers.size(); i++) {
        answers[i] = switch_value(answers[i]);
        string name_in_trie = neighbor_name(answers);
        if (name_in_trie != " " ) {
            neighbors.push(name_in_trie);
        }
        answers[i] = switch_value(answers[i]);
    }
}


string hammingTrie::neighbor_name(vector<int> answers)
{
    Node *curr = root;
    if (curr == nullptr) {
        return " ";
    }
    
    for (size_t i = 0; i < answers.size(); i++) {
        if (answers[i] == 0) {
            curr = curr->zero;
        } else {
            curr = curr->one;
        }
        if (curr == nullptr) {
            return " ";
        }
    }

    return curr->name;
}


int hammingTrie::switch_value(int x) 
{
    return (x + 1) % 2;
}






























