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
// Does: Initializies an instance of the hammingTrie class, by creating a root
//       node whose name is "root" and next pointers are both set to nullptr
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
// Does: Frees memory that was used to store the hammingTrie
hammingTrie::~hammingTrie()
{
    root = post_order_delete(root);
}

// Function post_order_delete
// Parameters: A pointer to a Node, node, which we wish to delete
// Returns:    The nullptr
// Does:       Checks to see if the given node is nullptr. If it isn't,
//             it recursively calls itself on both of node's children then 
//             deletes node
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

// Function get_a_neighbor
// Parameters: None
// Returns:    A string
// Does:       Gets a string from the neighbors stack. If the neighbors stack
//             is empty, returns the string "no more neighbors"
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

// Function make_node
// Parameters: None
// Returns:    A pointer to a Node, space for which has been newly allocated on
//             the heap
// Does:       Allocates space for a Node struct, sets its name to the string
//             " " and sets both of its next pointers to the nullptr
hammingTrie::Node *hammingTrie::make_node()
{
    Node *one_node = new Node;
    one_node->name = " ";
    one_node->zero = nullptr;
    one_node->one = nullptr;
    return one_node;
}

// Function build_trie
// Parameters: None
// Returns:    None
// Does:       Iterates through each of the entries points, and adds those
//             points to the trie.
void hammingTrie::build_trie()
{
    for (size_t i = 0; i < entriesVec.size(); i++) {
        add_point(entriesVec[i].answers, entriesVec[i].name); 
    }
}

// Function add_point
// Parameters: A vector of integers (which is the binary sequence that defines 
//             a data point, and a string, point_name which is the name of that
//             entry)
// Returns:    None
// Does:       Adds a point to the trie by tracing along the nodes until it 
//             finds a nullptr where it needs a new branch to accomodate this 
//             new point. When this happens, it will call the make_node 
//             function before advancing to that node. Ends by setting the 
//             final node's name to the point_name which will overwrite that 
//             point's name if the new point's answer sequence is a duplicate 
//             of one already in the trie
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

// Function find_neighbors_of 
// Parameters: A string, point_name, the name of an entry whose neighbors we 
//             want to add to the neighbors stack
// Returns:    None
// Does:       Uses a for loop to find the binary answers sequence of the given
//             name, then calls adds its second degree neighbors followed by 
//             the first degree neighbors to the neighbors stack
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

// Function find_neighbors_of
// Parameters: An integer, the index of an entry
// Returns:    None
// Does:       Overloaded version of find_neighbors_of where we are given the 
//             index of the entry whose neighbors we want to add to the stack.
//             here we don't have to search through the entry's vector in order
//             to find the matching answers vector of the given string, we have
//             random access to that entry
void hammingTrie::find_neighbors_of(int entry_index)
{
    find_second_degree_of(entriesVec[entry_index].answers);
    find_first_degree_of(entriesVec[entry_index].answers);
}

// Function find_second_degree_of
// Parameters: A vector of integers, answers, the binary sequence from which
//             we want to find sequences with a hamming distance of two.
// Returns:    None
// Does:       Uses for-loops to produce sequences that have a hamming distance
//             of two, then searches the trie for those neighbors adding them
//             to the neighbors stack if they are found. Essentially we want to
//             check binary sequences that differ from the given at two of the 
//             elements. 
void hammingTrie::find_second_degree_of(vector<int> answers)
{
    for (int i = 0; i < static_cast<int>(answers.size()) - 1; i++) {
        answers[i] = switch_value(answers[i]);

        for (int j = i + 1; j < static_cast<int>(answers.size()); j++) {
            answers[j] = switch_value(answers[j]);
            // This combination of for-loops will change two of the elements in
            // the sequence in every way possible

            string name_in_trie = neighbor_name(answers);
            if (name_in_trie != " ") {
                neighbors.push(name_in_trie);
            } 

            answers[j] = switch_value(answers[j]);
        }
        answers[i] = switch_value(answers[i]);
        // We switch the elements in the sequence back to their original values
        // once finished so that we can check the next neighbor
    }
}

// Function find_first_degree_of
// Parameters: A vector of integers, answers, the binary sequence that
//             corresponds to a certain data point in the trie
// Returns:    None
// Does:       Checks the trie for sequences that differ from the given answers
//             sequence at a single element (hamming distance of one). If one
//             or more are found, they are added to the neighbors stack to be
//             guessed
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

// Function neighbor_name
// Parameters: A vector of integers- 0s and 1s
// Returns:    A string, could be the empty string if the binary string is not
//             present in the trie or the stored entry name if the string is in
//             the trie
// Does:       Traverses the trie in accordance with the given binary sequence
//             in order to determine if a neighbor exists in the tree. If it 
//             does, we return the neighbor's name as string. If not we return
//             the single-space string " ".
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

// Function switch_value
// Parameters: An integer x, which will be the binary value 1 or 0
// Returns:    An integer, either 1 or 0
// Does:       If the given entry is odd (1), returns 0, if the given entry is
//             even (0), returns 1
int hammingTrie::switch_value(int x) 
{
    return (x + 1) % 2;
}

