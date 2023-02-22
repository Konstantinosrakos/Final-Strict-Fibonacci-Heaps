#include "iostream"
#include "strict-fibonacci-heap.hpp"
using namespace std;
// Printers
void print_state_of_rank_fixes(rank_list_record *rank)
{

    for (int i = 1; i < 6; i++)
    {
        if (rank->fixes[i] == NULL)
        {
            cout << "Rank " << i << " is empty" << endl;
        }
        else
        {
            cout << "Rank " << i << " is not empty" << endl;
        }
    }
}

void print_state_of_fix_list(FiboHeap *myHeap)
{
    for (int i = 1; i < 6; i++)
    {
        if (myHeap->fix_list[i] == NULL)
        {
            cout << "Fix " << i << " is empty" << endl;
        }
        else
        {
            cout << "Fix " << i << " is not empty" << endl;
        }
    }
}

void print_fibonode(FiboNode *node)
{
    cout << "item: " << node->item << endl;
    cout << "loss: " << node->loss << endl;
    cout << "left: ";
    if (node->left)
    {
        cout << node->left->item << endl;
    }
    else
    {
        cout << "NULL" << endl;
    }
    cout << "right: ";
    if (node->right)
    {
        cout << node->right->item << endl;
    }
    else
    {
        cout << "NULL" << endl;
    }
    cout << "parent: ";
    if (node->parent)
    {
        cout << node->parent->item << endl;
    }
    else
    {
        cout << "NULL" << endl;
    }
    cout << "left_child: ";
    if (node->left_child)
    {
        cout << node->left_child->item << endl;
    }
    else
    {
        cout << "NULL" << endl;
    }
    if (node->active != NULL && node->active->flag == true)
    {
        cout << "Is active " << endl;
    }
    else
    {
        cout << "It is not active" << endl;
    }

    cout << "q_prev: ";
    if (node->q_prev)
    {
        cout << node->q_prev->item << endl;
    }
    else
    {
        cout << "NULL" << endl;
    }
    cout << "q_next: ";
    if (node->q_next)
    {
        cout << node->q_next->item << endl;
    }
    else
    {
        cout << "NULL" << endl;
    }
    if (node->rank)
    {
        cout << "has rank" << node->rank->rank << endl;
    }
}