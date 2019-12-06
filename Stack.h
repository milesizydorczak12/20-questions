// Stack.h
// By: Miles Izydorczak
// Date: 28 November 2019
// Purpose: Header file for template class Stack. Can be a stack of any data 
//          type with basic stack functionality such as push, pop, top, and 
//          is_empty. The representation of this Stack is a linked list.
//

#include <exception>

#ifndef _STACK_H_
#define _STACK_H_

template<class T> class Stack {
private:

    struct Node {
        T data;
        Node *next;
    };

public:

    // Constructor Stack
    // Purpose: Intitializes an instance of the Stack class by setting its 
    //          front pointer to NULL
    Stack() 
    {
        front = nullptr;
    }

    // Destructor ~Stack
    // Purpose: Recycles all memory used to store the Stack by repeatedly 
    //          calling the pop function
    ~Stack() 
    {
        while(not is_empty()) {
            pop();
        }
    }

    // Copy Constructor Stack
    // Parameters: Another instance of the Stack class, passed by reference
    // Purpose: Initializes an instance of the Stack class to be a clone of 
    //          a pre-existing Stack. Performs a deep copy by calling the 
    //          recursive function copy_first at the front pointer of the Stack
    //          which we are copying
    Stack(const Stack &to_copy) 
    {
        front = copy_first(to_copy.front);
    }

    // Function copy_first
    // Parameters: A pointer to a Node, the Node which we will copy
    // Returns:    A pointer to a Node, a copy which is also stored on the heap
    // Does:       Recursive function that allocates space for a new Node on 
    //             the heap, then initializes that Node's data member to the 
    //             data of the given source node, and its next member to the 
    //             result of calling copy_first on the source's next member
    Node *copy_first(Node *source_first)
    {
        if (source_first == nullptr) {
            return nullptr;
        } else {
            Node *clone_node = new Node;
            clone_node->data = source_first->data;
            clone_node->next = copy_first(source_first->next);
        }
    }

    // Assignment Operator
    // Purpose: Redefines the = operator for use with two instances of a Stack
    //          class so that assigning one pre-existing class to another 
    //          recycles all memory used by the first and the makes a deep copy
    //          of the second
    Stack &operator=(const Stack &to_copy) 
    {
        if (this == &to_copy) {
            return *this;
        }

        while (not is_empty()) {
            pop();
        }

        front = copy_first(to_copy.front);

        return *this;
    }

    // Function push
    // Parameters: A variable of any type T, called new_data 
    // Returns:    None
    // Does:       Adds another element to the top of the Stack, by inserting a 
    //             Node at the front of the linked list
    void push(T new_data)
    {
        Node *new_node = new Node;
        new_node->data = new_data;
        new_node->next = front;
        front = new_node;
    }

    // Function pop
    // Parameters: None
    // Returns:    None
    // Does:       Removes the first element in the Stack by removing the first
    //             Node in the linked list. Assumes proper usage (not used on 
    //             an empty Stack)
    void pop()
    {
        Node *temp = front->next;
        delete front;
        front = temp;
    }

    // Function top
    // Parameters: None
    // Returns:    A variable of any type T, whatever type we are storing in 
    //             the Stack
    // Does:       Returns the data element of the first Node in the stack.
    //             Throws an exception if the Stack is empty.
    T top()
    {
        if (not is_empty()) {
            return front->data;
        } else {
            throw 42;
        }
    }

    // Function is_empty
    // Parameters: None
    // Returns:    A boolean
    // Does:       Returns true if the Stack is empty (which happens when the 
    //             front member variable is NULL) and false otherwise
    bool is_empty()
    {
        if (front == nullptr) {
            return true;
        } else {
            return false;
        }
    }

private:

    Node *front;
};

#endif
