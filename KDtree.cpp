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
}

// Function build_tree
// Parameters: A double, the version of the program which we want to build a 
//             tree for
// Returns:    A pointer to a Node, the root of a tree that we have built
// Does:       Given the version number as a double, makes a starting pool 
//             which contains the indices of every entry in the data set, then
//             calls the appropriate build tree function passing in that vector
//             of integers as the starting pool
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
// Returns:    None
// Does:       Moves the current pointer to the appropriate next pointer (yes
//             if yes is true, no if yes is false.
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

// Function size_of_pool
// Parameters: None
// Returns:    An integer
// Does:       Getter function that returns the size of the pool of correct
//             entries at the node that curr points to. The reason for this 
//             return is that the client can know when there is one entry left
//             that it could be so they know when to make a guess
int KDtree::size_of_pool()
{
    if (curr == nullptr) {
        return 0;
    } else {
        return static_cast<int>(curr->correct_entries.size());
    }
}

// Function advance_probably
// Parameters: A value of type userInput, UI
// Returns:    None 
// Does:       Similar functionality to "advance", but here, we have more 
//             options for what the player could have entered in the form of 
//             unknown, probably, and probably not. Whenever a value of 
//             probably or probably-not is given by the player, we add the 
//             current pointer to the stack of probablies and probably-nots so
//             that we can return to those nodes if we exhaust one of the 
//             branches and have yet to find the correct answer. If the given 
//             value is unknown, we treat it like probably not with the 
//             exception that the pointer to curr is added to the unknown stack
//             instead of the probably stack which has a higher priority in 
//             terms of when we return to it, for if a player answers probably
//             or probably-not, they are more sure of their answer than a 
//             player who answers unknown, and therefore there is a greater 
//             chance that unknown was answered incorrectly
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
            // We set question truth to probnot instead of unknown, because
            // we don't want to ask this question again per "skip_answered_
            // questions"
            set_question_truth(curr->question, probnot);
            add_unknown();
            curr = curr->no;
        }
        skip_answered_questions();
    }
}

// Function set_question_truth
// Parameters: A string, questionStr, and a userInput UI
// Returns:    None
// Does:       Finds the question in the questions vertex to find the question
//             with the same name as the one given as the questionStr. Then it
//             sets that question's truth variable to the given userInput
void KDtree::set_question_truth(string questionStr, userInput UI)
{
    for (size_t i = 0; i < questions.size(); i++) {
        if (questions[i].question == questionStr) {
            questions[i].truth = UI;
            return;
        }
    }
}

// Function skip_answered_questions
// Parameters: None
// Returns:    None
// Does:       If the question at curr has already been answered, advances to 
//             the next unanswered node in accorance with the answers to those
//             questions. This ensures that after we return to a 
//             probably/probably-not/unknwon node, we don't end up asking 
//             questions that had been answered in that previous branch, and as
//             a consequence, wasting some of our precious 20 opportunities to
//             narrow down the pool
void KDtree::skip_answered_questions() 
{
    userInput current_truth = unknown;
    for (size_t i = 0; i < questions.size(); i++) {
        if (questions[i].question == curr->question) {
            current_truth = questions[i].truth;
        }
    }
    // If the pool is less than or equal to one in size, then there are no 
    // quesions to be answered
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
        // Recursively calls itself if a question was skipped in order to skip 
        // other subsequent unanswered questions if they exist
        skip_answered_questions();
    }
} 

// Function return_to_probably
// Parameters: None
// Returns:    A bool, true if there was a probably node to return to, and 
//             false if we have exhausted the probablies and unknowns stacks
// Does:       First checks to see if the stack of unknowns is empty. If it 
//             isn't, it changes the curr pointer to the node pointer stored at
//             the top element of the stack. If the unknown pointer had been 
//             answered yes, we move to its no pointer, and if it had been 
//             answered no, we move to its yes pointer. Then we skip any 
//             answered questions that might be in this path. We repeat this 
//             process for the probalies and probably-nots if the unknowns 
//             was empty.
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

// Function add_probably
// Parameters: A boolean, yes
// Returns:    None
// Does:       Adds an element to the probablies stack whose yes value is set
//             to the given boolean and whose node pointer is set to the curr
//             pointer
void KDtree::add_probably(bool yes)
{
    Probably new_prob;
    new_prob.yes = yes;
    new_prob.prob_ptr = curr;
    probablies.push(new_prob);
}

// Function add_unknown
// Parameters: None
// Returns:    None
// Does:       Adds an element to the unknowns stack whose yes value is set to
//             false and whose node pointer value is set to the curr pointer
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

// function build_tree_order
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

// Function build_tree_best 
// Parameters: A vector of integers, the indices of the entries that, given the
//             current order of questions asked already, could be the correct 
//             one which we are trying to guess
// Returns:    A pointer to a node, the root of the subtree which we have just 
//             built
// Does:       Similar function to build_tree_order, but here, rather than just
//             calling the next question in the order that they appear in the 
//             questions vector, we call the select best question function 
//             which actively searches for the question that could best split
//             the pool of entries in half and asks that question. This is 
//             how a human plays twenty questions, if we already know a fact
//             about the entry which we are trying to guess, there is no need
//             to ask a question which only divides the entries for which that
//             fact is untrue.
KDtree::Node *KDtree::build_tree_best(vector<int> pool) 
{
    if (pool.size() < 1) {
        return nullptr;
    } else if (pool.size() == 1) {
        Node *new_node = make_node(pool[0]);
        return new_node;
    } else {
        // Here we find the best (most polarizing question) and use that to
        // divide the pool into a yes-pool and  a no-pool
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

// Function build_tree_sample
// Parameters: A vector of integers, the indices of the entries that, given the
//             current order of questions asked already, could be the correct 
//             one which we are trying to guess
// Returns:    A pointer to a Node, the root of the subtree which we have just 
//             built
// Does:       Builds a subtree in the same way as build_tree_best, but here 
//             we attempt to have better runtime because rather than searching
//             through the entire remaining pool of entries, we take a random
//             sample set of size 10, and look through those to find the best 
//             question. 
KDtree::Node *KDtree::build_tree_sample(vector<int> pool) 
{
    if (pool.size() < 1) {
        return nullptr;
    } else if (pool.size() == 1) {
        Node *new_node = make_node(pool[0]);
        return new_node;
    } else if (pool.size() < 10) {
        // If the size is less than 10, we can't make a sample set of size 10
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

// Function build_tree_sums
// Parameters: A vector of integers, the indices of the entries that, given the
//             current order of questions asked, could be the entry which we 
//             are trying to guess
// Returns:    A Node pointer to the root of the tree we have just built
// Does:       Alternative way to reduce the runtime of the build_tree_best
//             function, here we track the sums of the number of entries for
//             which the answer is yes and update these sums as entries are 
//             taken out of the pool so that we know which sum is closest to
//             half of the pool. This function is split into an overloaded 
//             recursive function, and this function which calls it at the 
//             starting pool and staring sums vectors
KDtree::Node *KDtree::build_tree_sums(vector<int> pool) 
{
    // Make a second vector parallel to that of the questions, this vector is
    // of type integer and each index contains the total number of entries in 
    // the pool for which that question is true.
    vector<int> sums(questions.size(), 0);

    for (size_t i = 0; i < entries.size(); i++) {
        for (size_t j = 0; j < entries[i].answers.size(); j++) {
            sums[j] += entries[i].answers[j];
        }
    }

    // Now we pass the starting pool and the parallel vector of sums to the 
    // overloaded recursive function build_tree_sums
    return build_tree_sums(pool, sums);
}

// Function build_tree_sums
// Parameters: Two vectors of integers: pool and q_sums
// Returns:    A pointer to a Node
// Does:       Overloaded recursive version of build_tree_sums that tracks
//             the sums of entries for which each question is correct as well 
//             as the pool of entries themselves. We no longer have to interate
//             through each of the entries' answers vector, we can just iterate
//             through the q_sums vector to find the most polarizing question
KDtree::Node *KDtree::build_tree_sums(vector<int> pool, vector<int> q_sums)
{
    if (pool.size() < 1) {
        return nullptr;
    } else if (pool.size() == 1) {
        Node *new_node = make_node(pool[0]);
        return new_node; 
    } else {
        int best_index = select_best_sum(q_sums, pool.size());
        Node *new_node = make_node(pool, best_index);

        vector<int> yes_pool, no_pool, yes_sums(questions.size(), 0);
        for (size_t i = 0; i < pool.size(); i++) {
            if (entries[pool[i]].answers[best_index] == 1) {
                yes_pool.push_back(pool[i]);
                for (size_t j = 0; j < entries[pool[i]].answers.size(); j++) {
                    // Remove the entry's contribution to the q_sums and add 
                    // one to the yes_sums
                    yes_sums[j] += entries[pool[i]].answers[j];
                    q_sums[j] -= entries[pool[i]].answers[j];
                }
            } else {
                no_pool.push_back(pool[i]);
            }
        }
        new_node->yes = build_tree_sums(yes_pool, yes_sums);
        new_node->no = build_tree_sums(no_pool, q_sums);
        return new_node;
    }
}

// Function select_best_sums
// Parameters: A vector of integers, q_sums, whose elements are the sums of 
//             entries for which the answer is correct for each question, and
//             the size of the current pool of entries, as an unsigned 
// Returns:    An integer, the idex of the most polarizing question
// Does:       Returns the index of the most polarizing question which is the
//             index in the q_sums vector whose value is closest to half of the
//             pool_size   
int KDtree::select_best_sum(vector<int> q_sums, size_t pool_size) 
{
    double half_of_pool = pool_size / 2;
    int best_index = 0;
    double best_difference = pool_size;

    // Similar procedure to finding the maximum or minimum element in a vector
    // But the exception is that here, we are trying to minimize the difference
    // between half of the pool size and the number of correct entries at a 
    // question
    for (size_t i = 0; i < q_sums.size(); i++) {
        double current_difference = abs(q_sums[i] - half_of_pool);
        if (current_difference < best_difference) {
            best_index = i;
            best_difference = current_difference;
        }
    }
    return best_index;
}

// Function make_sample_set
// Parameters: Two vectors of integers, passed by reference. The given pool of
//             indices of entries and the sample set of that pool
// Returns:    None
// Does:       Randomly chooses a sample set of 10 from the given pool of 
//             indices, assumes that the size of the pool is 10 or greater
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

// Function select_best_question
// Parameters: A vector of integers, pool, the indices of entries
// Returns:    An integer, the index of the best question to ask in order to
//             divide the pool roughly in half
// Does:       Iterates through all of the questions and all of the entries,
//             adding up the number of entries that are correct for each
//             question and tracking the index of the question that has a sum
//             of indices closest to half of the pool's size. Then returns that
//             index
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

        double current_difference = abs(number_yes - half_of_pool);
        double target_difference = abs(best_question_yes - half_of_pool);
        // Want to track the minimum difference between the number of entries
        // for which the value is yes and half the size of the pool
        if (current_difference < target_difference) {
            best_question_yes = number_yes;
            best_question_index = static_cast<int>(i);
        }
    }
    return best_question_index;
}

// Function make_node
// Parameters: An integer, data, an index of the single remaining correct entry
//             in the pool
// Returns:    A pointer to a Node, newly initialized 
// Does:       Overloaded make node function for making a leaf node where there
//             is only one index in the correct entries vector, and the 
//             question member variable is set to the string " ", signifying 
//             that there is no question to ask here, for there is no need to
//             divide the pool any further
KDtree::Node *KDtree::make_node(int data)
{
    Node *new_node = new Node;
    new_node->yes = nullptr;
    new_node->no = nullptr;
    new_node->question = " ";
    new_node->correct_entries.push_back(data);
    return new_node;
}

// Function make_node
// Parameters: A vector of integers, pool, the indices which the correct entry
//             could be, and an index of a question 
// Returns:    A pointer to a Node, newly allocated on the heap
// Does:       An overloaded make_node function that stores a vector of indices
//             as the correct entries vector and sets the question member 
//             variable to the question at the given index. Initializes both
//             yes and no pointers to the nullptr
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
