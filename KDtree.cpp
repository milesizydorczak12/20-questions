// KDtree.cpp
// By: Miles Izydorczak
// Date: 19 November 2019
// Purpose: Impementation of the KDtree class. There is procedural 
//          abstraction here because we keep the function declarations in a 
//          seperate file from their definitions
//

#include <iostream>
#include <string>
#include <cstdlib>
#include <vector>
#include <ctime>
#include "KDtree.h"
#include "questions_and_entries.h"
#include "Stack.h"
#include "userInput.h"

using namespace std;

// cosntructor KDtree
// Parameters: Two vectors passed by reference, one of the entries that we will
//             work with, the other of strings that are the questions
// Does:       Creates and initializes an instance of the KDtree class,
//             copying data from the given vectors into its own private data
//             members, then builing a tree of this information, and setting
//             both root and curr to the root of this tree.
KDtree::KDtree(vector<Entry> &eVec, vector<Question> &qVec, double vers)
{
    srand(time(NULL));
    entries = eVec;
    questions = qVec;
    root = build_tree(vers);
    curr = root;
    //for (size_t i = 0; i < eVec.size(); i++) {
    //    entries.push_back(eVec[i]);
    //}
    //for (size_t i = 0; i < qVec.size(); i++) {
    //    questions.push_back(qVec[i]);
    //}  
}

// Maybe store the version as a private data member, then move the build tree 
// calls fro the yes and no pools, might not work because of extra parameters

KDtree::Node *KDtree::build_tree(double version) 
{
    vector<int> starting_pool;
    for (size_t i = 0; i < entries.size(); i++) {
        starting_pool.push_back(static_cast<int>(i));
    }

    if (version == 0.0) {
        return build_tree_order(starting_pool, 0);
    } else if (version == 1.0) {
        return build_tree_best(starting_pool);
    } else if (version == 1.1) {
        return build_tree_sample(starting_pool);
    } else if (version == 1.5) {
        return build_tree_sums(starting_pool);
    } else if (version == 2.0) {
        return build_tree_best(starting_pool);
    } else if (version == 3.0) {
        return build_tree_sample(starting_pool);
    } else {
        return nullptr;
    }
}

// Destructor ~KDtree
// Does: Recycles all memory used to store this class's tree by calling the 
//       recursive "delete_tree" function at the tree's root
KDtree::~KDtree() 
{
    curr = nullptr;
    root = delete_tree(root);
}

// function delete_tree
// Parameters: A pointer to a Node, node
// Returns:    A pointer to a Node, node, which will always be the nullptr
// Does:       Recursive function that performs a post-order delete of this
//             binary tree
KDtree::Node *KDtree::delete_tree(Node *node)
{
    // Base case #1: the given node is the nullptr, in which case there is 
    // nothing to delete- just return nullptr back again
    if (node == nullptr) {
        return nullptr;
    } else if (node->yes == nullptr and node->no == nullptr) {
    // Base case #2: the given node is a leaf node, in which case, we delete 
    // the node safely without worrying about the node's yes and no pointers
        delete node;
        return nullptr;
    } else {
    // Recursive case: one or both of the next pointers have subtrees. In this
    // case, we delete those first, then delete the current node post order
        node->yes = delete_tree(node->yes);
        node->no = delete_tree(node->no);
        delete node;
        return nullptr;
    }
}

// function reset
// Parameters: Node
// Returns:    None
// Does:       Sets the current pointer to the root of the tree, in case I 
//             want to imlepent a "play again" feature without rebuilding the
//             tree
void KDtree::reset()
{
    curr = root;
}

// function advance
// Parameters: A bool, yes, which will determine to which node we should 
//             advance the curr pointer to
// Returns:    An integer, the size of the remaining pool of entries after
//             advancing the curr pointer
// Does:       Moves the current pointer to the appropriate next pointer (yes
//             if yes, no if !yes), then returns the number of entries at the
//             pool that we have narrowed it down to. The reason for this 
//             return is that the client can know when there is one entry left
//             that it could be so they know when to call the getter function
//             and make a guess
// int KDtree::advance(bool yes)
// {
//     if (curr == nullptr) {
//         return 0;
//     } else {
//         if (yes) {
//             curr = curr->yes;
//         } else {
//             curr = curr->no;
//         }

//         if (curr == nullptr) {
//             return 0;
//         } else {
//             return static_cast<int>(curr->correct_entries.size());
//         }
//     }
// }

void KDtree::advance(bool yes)
{
    if (curr == nullptr) {
        return;
    } else {
        if (yes) {
            curr = curr->yes;
        } else {
            curr = curr->no;
        }
    }
}

int KDtree::size_of_pool()
{
    if (curr == nullptr) {
        return 0;
    } else {
        return static_cast<int>(curr->correct_entries.size());
    }
}

void KDtree::advance_probably(userInput UI)
{
    if (curr == nullptr) {
        return;
    } else {
        set_question_truth(curr->question, UI);
        if (UI == yes) {
            curr = curr->yes;
        } else if (UI == no) {
            curr = curr->no;
        } else if (UI == prob) {
            add_probably(true);
            curr = curr->yes;
        } else if (UI == probnot) {
            add_probably(false);
            curr = curr->no;
        } else {
            // TODO pick randomly between yes and no
            set_question_truth(curr->question, probnot);
            add_unknown();
            curr = curr->no;
        }
        skip_answered_questions();
    }
}

// might want to consider hash table implementation
void KDtree::set_question_truth(string questionStr, userInput UI)
{
    for (size_t i = 0; i < questions.size(); i++) {
        if (questions[i].question == questionStr) {
            questions[i].truth = UI;
            return;
        }
    }
}

void KDtree::skip_answered_questions() 
{
    userInput current_truth = unknown;
    for (size_t i = 0; i < questions.size(); i++) {
        if (questions[i].question == curr->question) {
            current_truth = questions[i].truth;
        }
    }
    if (size_of_pool() <= 1) {
        return;
    } else if (current_truth != unknown) {
        if (current_truth == yes) {
            curr = curr->yes;
        } else if (current_truth == no) {
            curr = curr->no;
        } else if (current_truth == prob) {
            add_probably(true);
            curr = curr->yes;
        } else if (current_truth == probnot) {
            add_probably(false);
            curr = curr->no;
        }
        skip_answered_questions();
    }
} 

// int KDtree::advance_probably(userInput input)
// {
//     if (curr == nullptr) {
//         return 0;
//     } else {
//         if (input == yes) {
//             curr = curr->yes;
//         } else if (input == no) {
//             curr = curr->no;
//         } else if (input == probably) {
//             add_probably(true);
//             curr = curr->yes;
//         } else {
//             add_probably(false);
//             curr = curr->no;
//         }

//         if (curr == nullptr) {
//             return 0;
//         } else {
//             return static_cast<int>(curr->correct_entries.size());
//         }
//     }
//
bool KDtree::return_to_probably()
{
    if (not unknowns.is_empty()) {
        Unknown recent_unkwn = unknowns.top();
        unknowns.pop();
        if (recent_unkwn.yes) {
            curr = recent_unkwn.unkwn_ptr->no;
        } else {
            curr = recent_unkwn.unkwn_ptr->yes;
        }
        skip_answered_questions();
        return true;
    } else if (not probablies.is_empty()) {
        Probably recent_prob = probablies.top();
        probablies.pop();

        if (recent_prob.yes) {
            curr = recent_prob.prob_ptr->no;
        } else {
            curr = recent_prob.prob_ptr->yes;
        }
        skip_answered_questions();
        return true;
    } 
    return false;
}

void KDtree::add_probably(bool yes)
{
    Probably new_prob;
    new_prob.yes = yes;
    new_prob.prob_ptr = curr;
    probablies.push(new_prob);
}

void KDtree::add_unknown()
{
    Unknown new_unkwn;
    new_unkwn.yes = false;
    new_unkwn.unkwn_ptr = curr;
    unknowns.push(new_unkwn);
}


// function question_at_curr
// Parameters: None
// Returns:    A string
// Does:       Getter function that returns the question data at the current 
//             Node. Returns the empty string as a saftey measure in case the 
//             current Node was advanced all the way to the nullptr
string KDtree::question_at_curr() 
{
    if (curr == nullptr) {
        return " ";
    } else {
        return curr->question;
    }
}

// function entry_at_curr
// Parameters: None
// Returns:    A string
// Does:       Getter function that returns the first element in the pool of 
//             correct entries in a node. Only practical when making a guess
string KDtree::entry_at_curr() 
{
    if (curr == nullptr) {
        return "No entries that match these answers";
    } else {
        return entries[curr->correct_entries[0]].name; 
    }
}

// function build_tree_0
// Parameters: A vector of integers, the pool of integers that correspend to 
//             the indicies of entries that could still be the mystery entry,
//             as well as the number question that we are currently asking
// Returns:    A pointer to a node, the root of a constructed subtree of the 
//             kd-tree that is completed
// Does:       Recursive function that inserts the current pool of data and 
//             question string into the proper fields of a Node struct, then
//             divides the data according to its answer to the current question
//             and uses that data and the next index question of the questions
//             vector to recursively build the subtrees for its yes and no 
//             pointers. Stops when there is one entry left in the pool, 
//             becuase there is not a need to divide that data any further
KDtree::Node *KDtree::build_tree_order(vector<int> pool, int q_num)
{
    if (pool.size() < 1) {
        return nullptr;
    } else if (pool.size() == 1) {
        Node *new_node = make_node(pool[0]);
        return new_node;
    } else {
        Node *new_node = make_node(pool, q_num);

        vector<int> yes_pool, no_pool;
        for (size_t i = 0; i < pool.size(); i++) {
            if (entries[pool[i]].answers[q_num] == 0) {
                no_pool.push_back(pool[i]);
            } else {
                yes_pool.push_back(pool[i]);
            }
        }

        new_node->yes = build_tree_order(yes_pool, q_num + 1);
        new_node->no = build_tree_order(no_pool, q_num + 1);
        return new_node;
    }
}


// Build tree that looks at all entries and finds the most polarizing question 
// to ask
KDtree::Node *KDtree::build_tree_best(vector<int> pool) 
{
    if (pool.size() < 1) {
        return nullptr;
    } else if (pool.size() == 1) {
        Node *new_node = make_node(pool[0]);
        return new_node;
    } else {
        
        int best_index = select_best_question(pool);
        Node *new_node = make_node(pool, best_index);

        vector<int> yes_pool, no_pool;
        for (size_t i = 0; i < pool.size(); i++) {
            if (entries[pool[i]].answers[best_index] == 0) {
                no_pool.push_back(pool[i]);
            } else {
                yes_pool.push_back(pool[i]);
            }
        }

        new_node->yes = build_tree_best(yes_pool);
        new_node->no = build_tree_best(no_pool);
        return new_node;
    }
}

// build tree that looks at a randomly chosen subset of the current pool and 
// decides the most polarizing subset from that
KDtree::Node *KDtree::build_tree_sample(vector<int> pool) 
{
    if (pool.size() < 1) {
        return nullptr;
    } else if (pool.size() == 1) {
        Node *new_node = make_node(pool[0]);
        return new_node;
    } else if (pool.size() < 10) {
        return build_tree_best(pool);
    } else {
        vector<int> sample_set;
        make_sample_set(sample_set, pool);
        int best_index = select_best_question(sample_set);
        Node *new_node = make_node(pool, best_index);

        vector<int> yes_pool, no_pool;
        for (size_t i = 0; i < pool.size(); i++) {
            if (entries[pool[i]].answers[best_index] == 0) {
                no_pool.push_back(pool[i]);
            } else {
                yes_pool.push_back(pool[i]);
            }
        }

        new_node->yes = build_tree_sample(yes_pool);
        new_node->no = build_tree_sample(no_pool);
        return new_node;
    }
}


KDtree::Node *KDtree::build_tree_sums(vector<int> pool) 
{
    vector<int> sums;
    for (size_t i = 0; i < questions.size(); i++) {
        sums.push_back(0);
    }

    //int sums[static_cast<int>(questions.size())];
    //for (size_t i = 0; i < questions.size(); i++) {
        //sums[i] = 0;
    //}

    for (size_t i = 0; i < entries.size(); i++) {
        for (size_t j = 0; j < entries[i].answers.size(); j++) {
            sums[entries[i].answers[j]]++;
        }
    }

    return build_tree_sums(pool, sums);
}

KDtree::Node *KDtree::build_tree_sums(vector<int> pool, vector<int> q_sums) 
{
    if (pool.size() < 1) {
        return nullptr;
    } else if (pool.size() == 1) {
        Node *new_node = make_node(pool[0]);
        return new_node;
    } else {
    int best_index = select_best_sum(q_sums, static_cast<int>(pool.size()));
        Node *new_node = make_node(pool, best_index);

        vector<int> yes_pool, no_pool, yes_sums, no_sums;
        for (size_t i = 0; i < questions.size(); i++) {
            no_sums.push_back(0);
            yes_sums.push_back(0);
        }
        for (size_t i = 0; i < pool.size(); i++) {
            bool yes = false;
            for (size_t j = 0; j < entries[pool[i]].answers.size(); j++) {
                if (entries[pool[i]].answers[j] == best_index) {
                    yes = true;
                    break;
                } 
            }
            if (yes) {
                yes_pool.push_back(pool[i]);
                for (size_t j = 0; j < entries[pool[i]].answers.size(); j++) {
                    yes_sums[entries[pool[i]].answers[j]]++;
                }

            } else {
                no_pool.push_back(pool[i]);
                for (size_t j = 0; j < entries[pool[i]].answers.size(); j++) {
                    no_sums[entries[pool[i]].answers[j]]++;
                }
            }
        }
        new_node->yes = build_tree_sums(yes_pool, yes_sums);
        new_node->no = build_tree_sums(no_pool, no_sums);
        return new_node;
    }
}

int KDtree::select_best_sum(vector<int> q_sums, int pool_size) 
{
    double half_of_pool = pool_size / 2;
    int best_index = 0;
    double best_difference = pool_size;

    for (size_t i = 0; i < q_sums.size(); i++) {
        double current_difference = abs(q_sums[i] - half_of_pool);
        if (current_difference < best_difference) {
            best_index = i;
            best_difference = current_difference;
        }
    }
    return best_index;
}


void KDtree::make_sample_set(vector<int> &sample_set, vector<int> &pool)
{
    vector<int> clone;
    for (size_t i = 0; i < pool.size(); i++) {
        clone.push_back(pool[i]);
    }

    for (int i = 0; i < 10; i++) {
        size_t random_index = rand() % clone.size();
        sample_set.push_back(clone[random_index]);
        vector<int>::iterator to_remove;
        to_remove = clone.begin();
        to_remove += random_index;
        clone.erase(to_remove);
    }
}


int KDtree::select_best_question(vector<int> pool) 
{
    int best_question_index = 0;
    int best_question_yes = 0;
    double half_of_pool = static_cast<double>(pool.size()) / 2;

    for (size_t i = 0; i < questions.size(); i++) {
        int number_yes = 0;
        for (size_t j = 0; j < pool.size(); j++) {
            if (entries[pool[j]].answers[i] == 1) {
                number_yes++;
            }
        }

  if (abs(number_yes - half_of_pool) < abs(best_question_yes - half_of_pool)) {
            best_question_yes = number_yes;
            best_question_index = static_cast<int>(i);
        }
    }
    return best_question_index;
}


KDtree::Node *KDtree::make_node(int data)
{
    Node *new_node = new Node;
    new_node->yes = nullptr;
    new_node->no = nullptr;
    // Question new_question;
    // new_question.truth = unknown;
    // new_question.question = " ";
    // new_node->question = new_question;
    new_node->question = " ";
    new_node->correct_entries.push_back(data);
    return new_node;
}

KDtree::Node *KDtree::make_node(vector<int> pool, int q_index) 
{
    Node *new_node = new Node;
    new_node->question = questions[q_index].question;

    for (size_t i = 0; i < pool.size(); i++) {
        new_node->correct_entries.push_back(pool[i]);
    }

    new_node->yes = nullptr;
    new_node->no = nullptr;
    return new_node;
}


// function print_input
// Parameters: None
// Returns:    None
// Does:       Calls functions which print the entries and questions for
//             debugging purposes
void KDtree::print_input() 
{
    print_Es();
    print_Qs();
}

// function print_Es
// Parameters: None
// Returns:    None
// Does:       Prints each of the entries and their answers formatted in the 
//             same way as the input text file. 
void KDtree::print_Es()
{
    cout << "Printing Entries:" << endl;

    for (size_t i = 0; i < entries.size(); i++) {
        cout << entries[i].name << endl;
        for (size_t j = 0; j < entries[i].answers.size(); j++) {
            cout << entries[i].answers[j] << " ";
        }
        cout << endl;
    }
    cout << endl;
}

// function print_Qs 
// Parameters: None
// Returns:    None
// Does:       Prints each question to standard output in the order that they 
//             appear in the questions vector
void KDtree::print_Qs() 
{
    cout << "Printing Questions:" << endl;

    for (size_t i = 0; i < questions.size(); i++) {
        cout << questions[i].question << endl;
    }

    cout << endl;
}






