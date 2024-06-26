#include <iostream>
#include "strict-fibonacci-heap.hpp"
#include <chrono>

using namespace std;

// Declarations
void printFiboTree(FiboNode *root);
void add_to_fix_list(FiboHeap *myHeap, fix_list_record *fix_record, int part);
void add_to_rank_list(FiboHeap *myHeap, fix_list_record *fix_record, int part);
void remove_from_rank_list(FiboHeap *myHeap, fix_list_record *fix_record, int part);
rank_list_record *create_rank_record(rank_list_record *previous, int rank);
bool root_degree_reduction(FiboHeap *myHeap);
bool active_root_reduction(FiboHeap *myHeap);
bool check_fix_records(fix_list_record *fix_record_1, fix_list_record *fix_record_2);
bool is_active(FiboNode *node);
bool is_passive_linkable(FiboNode *node);
void sort_3_nodes(FiboNode *&x, FiboNode *&y, FiboNode *&z);
void x_tree_is_bigger(FiboHeap *&x, FiboHeap *&y);
void x_is_smaller(FiboNode *&x, FiboNode *&y);
FiboNode *move_in_queue(FiboHeap *myHeap);
void remove_from_siblings(FiboNode *child);
void remove_from_queue(FiboHeap *myHeap, FiboNode *node);
void link(FiboNode *child, FiboNode *parent);
void mark_as_active(FiboHeap *myHeap, FiboNode *node);
fix_list_record *remove_from_fix_list(FiboHeap *myHeap, fix_list_record *fix_record, int part);
void check_for_fix_promotions(FiboHeap *myHeap, rank_list_record *rank, int part);
fix_list_record *create_active_root(FiboHeap *myHeap, FiboNode *node);
fix_list_record *promote_active_node_to_active_root(FiboHeap *myHeap, FiboNode *node);
fix_list_record *create_loss_record(FiboHeap *myHeap, FiboNode *node);
rank_list_record *create_rank_record(rank_list_record *previous, int rank);
void promote_active_root(FiboHeap *myHeap, fix_list_record *fix_record);
struct FiboHeap *meld(FiboHeap *x, FiboHeap *y);
struct FiboHeap *insert_first_node(int value);
struct FiboHeap *insert_node(FiboHeap *myHeap, int value);
void loss_to_active_root(FiboHeap *myHeap, fix_list_record *fix_record);
void decrease_rank(FiboHeap *myHeap, FiboNode *node);
void increase_rank(FiboHeap *myHeap, FiboNode *node);
void add_loss(FiboHeap *myHeap, FiboNode *node);
void one_loss_reduction(FiboHeap *myHeap);
void two_loss_reduction(FiboHeap *myHeap);
bool check_fix_records(fix_list_record *fix_record_1, fix_list_record *fix_record_2);
FiboNode *find_new_root(FiboNode *node);
void make_passive(FiboHeap *myHeap, FiboNode *node);
void loss_reduction(FiboHeap *myHeap);
void delete_minumum(FiboHeap *myHeap);
void decrease_key(FiboHeap *myHeap, FiboNode *node, int value);
void delete_item(FiboHeap *myHeap, FiboNode *node);
void printFiboTree(const std::string &prefix, FiboNode *node, bool isLeft);
void printFiboTree(FiboNode *root);

// Printer
void printFiboTree(const std::string &prefix, FiboNode *node, bool isLeft)
{
    if (node != nullptr)
    {
        std::cout << prefix;

        // Check if this is the root node, in which case we don't want to print any arrows
        if (node->parent == nullptr)
        {
            std::cout << node->item << std::endl;
        }
        else
        {
            if (is_active(node))
                std::cout << (isLeft ? "├── " : "└── ") << node->item << "*" << std::endl;
            else
                std::cout << (isLeft ? "├── " : "└── ") << node->item << std::endl;
        }

        // Recursively print the left child's siblings
        printFiboTree(prefix + (isLeft ? "│   " : "    "), node->left_child, true);

        // Recursively print the rest of the tree
        if (node->left_child != nullptr)
        {
            FiboNode *sibling = node->left_child->right;
            while (sibling != node->left_child)
            {
                printFiboTree(prefix + (isLeft ? "│   " : "    "), sibling, false);
                sibling = sibling->right;
            }
        }
    }
}

void printFiboTree(FiboNode *root)
{
    printFiboTree("", root, false);
}
// Utilties

bool is_active(FiboNode *node)
{
    if (node->active != NULL && node->active->flag == true)
        return true;
    else
        return false;
}

bool is_passive_linkable(FiboNode *node)
{
    if (!is_active(node))
    {
        if (node->left_child != NULL)
        {
            if (!is_active(node->left_child))
            {
                return true;
            }
        }
        else
        {
            return true;
        }
    }
    return false;
}

void sort_3_nodes(FiboNode *&x, FiboNode *&y, FiboNode *&z)
{
    if (x->item > z->item)
    {
        swap(x, z);
    }
    if (x->item > y->item)
    {
        swap(x, y);
    }
    if (y->item > z->item)
    {
        swap(y, z);
    }
}

void x_tree_is_bigger(FiboHeap *&x, FiboHeap *&y)
{
    if (x->size < y->size)
    {
        swap(x, y);
    }
    return;
}

void x_is_smaller(FiboNode *&x, FiboNode *&y)
{
    if (x->item > y->item)
    {
        swap(x, y);
    }
}

void make_passive(FiboHeap *myHeap, FiboNode *node)
{
    node->active->ref_count--;
    node->active = NULL;

    if (node->fix != NULL)
    {
        free(remove_from_fix_list(myHeap, node->fix, node->fix->type));
    }

    return;
}

void remove_from_siblings(FiboNode *child)
{
    FiboNode *next = child->right;
    FiboNode *previous;

    if (child->parent != NULL)
    {
        if (child == next)
        {
            child->parent->left_child = NULL;
        }
        else
        {
            previous = child->left;
            next->left = previous;
            previous->right = next;
            if (child->parent->left_child == child)
            {
                child->parent->left_child = next;
            }
        }
    }

    child->right = child;
    child->left = child;
    child->parent = NULL;
}

void remove_from_siblings_root(FiboHeap *myHeap, FiboNode *child)
{
    if (myHeap->non_linkable_child == child)
    {
        if (child != child->right && !is_active(child) && !is_passive_linkable(child))
        {
            myHeap->non_linkable_child = child->right;
        }
        else if (child != child->left && is_active(child->left))
        {
            myHeap->non_linkable_child = child->left;
        }
        else
        {
            myHeap->non_linkable_child = NULL;
        }
    }
    remove_from_siblings(child);
}

void link_to_root(FiboHeap *myHeap, FiboNode *child, FiboNode *parent)
{
    if (parent->left_child == NULL)
    {
        parent->left_child = child;
        child->parent = parent;
        if (!is_passive_linkable(child))
        {
            myHeap->non_linkable_child = child;
        }
        return;
    }

    FiboNode *next, *previous;
    if (is_active(child))
    {
        if (myHeap->non_linkable_child == NULL)
        {
            myHeap->non_linkable_child = child;
            next = parent->left_child;
        }
        else
        {
            if (is_active(parent->left_child))
            {
                next = parent->left_child;
            }
            else
            {
                next = myHeap->non_linkable_child;
            }
        }
    }
    else if (is_passive_linkable(child))
    {
        next = parent->left_child;
        cout << child->item << endl;
    }
    else
    {
        cout << "hi";
        if (myHeap->non_linkable_child == NULL)
        {
            cout << "uo";
            myHeap->non_linkable_child = child;
            next = parent->left_child;
        }
        else
        {
            if (is_active(myHeap->non_linkable_child))
            {
                next = myHeap->non_linkable_child->right;
                myHeap->non_linkable_child = child;
            }
            else
            {
                next = myHeap->non_linkable_child;
                myHeap->non_linkable_child = child;
            }
        }
    }

    previous = next->left;
    child->right = next;
    child->left = previous;
    previous->right = child;
    next->left = child;

    if (child->active != NULL && child->active->flag == true)
    {
        parent->left_child = child;
    }

    child->parent = parent;
}

void link(FiboHeap *myHeap, FiboNode *child, FiboNode *parent)
{

    remove_from_siblings(child);
    FiboNode *next = parent->left_child;
    FiboNode *previous;

    if (next == NULL)
    {
        parent->left_child = child;
    }
    else
    {
        previous = next->left;
        child->right = next;
        child->left = previous;
        previous->right = child;
        next->left = child;

        if (child->active != NULL && child->active->flag == true)
        {
            parent->left_child = child;
        }
    }
    child->parent = parent;
}

void mark_as_active(FiboHeap *myHeap, FiboNode *node)
{
    if (myHeap->active == NULL)
    {
        myHeap->active = new active_record();
        myHeap->active->flag = true;
    }
    else
    {
        myHeap->active->flag = true;
    }

    node->active = myHeap->active;
    myHeap->active->ref_count++;

    rank_list_record *current_rank = myHeap->rank_list;
    if (current_rank == NULL)
    {
        myHeap->rank_list = create_rank_record(myHeap->rank_list, 0);
        current_rank = myHeap->rank_list;
    }
    node->rank = current_rank;
    node->loss = 0;
    current_rank->ref_count++;
}

/*          Fix list utilties   */

void add_to_fix_list(FiboHeap *myHeap, fix_list_record *fix_record, int part)
{
    fix_record->left = fix_record;
    fix_record->right = fix_record;

    rank_list_record *rank = fix_record->rank;

    if (rank->fixes[part] == NULL)
    {
        if (myHeap->fix_list[part] == NULL)
        {
            myHeap->fix_list[part] = fix_record;
        }
        else
        {
            myHeap->fix_list[part]->right->left = fix_record->left;
            fix_record->left->right = myHeap->fix_list[part]->right;
            myHeap->fix_list[part]->right = fix_record;
            fix_record->left = myHeap->fix_list[part];
        }
    }
    else
    {
        rank->fixes[part]->right->left = fix_record->left;
        fix_record->left->right = rank->fixes[part]->right;
        rank->fixes[part]->right = fix_record;
        fix_record->left = rank->fixes[part];
    }

    myHeap->fix_list[part] = fix_record->left;
    fix_record->type = part;
    fix_record->node->fix = fix_record;
    add_to_rank_list(myHeap, fix_record, part);

    check_for_fix_promotions(myHeap, fix_record->rank, part);
}

fix_list_record *remove_from_fix_list(FiboHeap *myHeap, fix_list_record *fix_record, int part)
{

    if (fix_record->right == fix_record)
    {
        myHeap->fix_list[part] = NULL;
    }
    else
    {
        fix_record->left->right = fix_record->right;
        fix_record->right->left = fix_record->left;
        if (fix_record == myHeap->fix_list[part])
        {
            myHeap->fix_list[part] = fix_record->right;
        }
    }

    remove_from_rank_list(myHeap, fix_record, part);
    fix_record->node->fix = NULL;
    return fix_record;
}

void check_for_fix_promotions(FiboHeap *myHeap, rank_list_record *rank, int part)
{

    fix_list_record *temp1;
    fix_list_record *temp2;

    if (part == 2)
    {

        if (rank->fixes[part] != rank->fixes[part]->right && rank->fixes[part]->rank == rank->fixes[part]->right->rank)
        {
            temp1 = remove_from_fix_list(myHeap, rank->fixes[part], part);

            temp2 = remove_from_fix_list(myHeap, rank->fixes[part], part);

            add_to_fix_list(myHeap, temp1, 1);
            add_to_fix_list(myHeap, temp2, 1);
        }
    }
    else if (part == 3)
    {
        if (rank->fixes[part] != rank->fixes[part]->right && rank->fixes[part]->rank == rank->fixes[part]->right->rank)
        {
            temp1 = remove_from_fix_list(myHeap, rank->fixes[part], part);
            temp2 = remove_from_fix_list(myHeap, rank->fixes[part], part);
            add_to_fix_list(myHeap, temp1, 4);
            add_to_fix_list(myHeap, temp2, 4);
        }
    }
}

bool check_fix_records(fix_list_record *fix_record_1, fix_list_record *fix_record_2)
{
    if (fix_record_1 == fix_record_2)
    {
        return false;
    }

    if (fix_record_1->rank != fix_record_2->rank)
    {
        return false;
    }

    return true;
}

void add_to_rank_list(FiboHeap *myHeap, fix_list_record *fix_record, int part)
{

    if (fix_record->rank->fixes[part] == NULL)
    {
        fix_record->rank->fixes[part] = fix_record;
    }

    fix_record->rank->ref_count++;
}

void remove_from_rank_list(FiboHeap *myHeap, fix_list_record *fix_record, int part)
{

    if (fix_record->right->rank == fix_record->rank && fix_record->right != fix_record && fix_record->rank->fixes[part] == fix_record)
    {
        fix_record->rank->fixes[part] = fix_record->right;
    }
    else
    {
        fix_record->rank->fixes[part] = NULL;
    }
    fix_record->rank->ref_count--;
    /*

    if (rank->fixes[part] != rank->fixes[part]->right)
    {
        if (rank->fixes[part]->rank == rank->fixes[part]->right->rank)
        {
            rank->fixes[part] = rank->fixes[part]->right;
        }
        else
        {
            rank->fixes[part] = NULL;
        }
    }
    else
    {
        rank->fixes[part] = NULL;
    }
    */
}

fix_list_record *create_active_root(FiboHeap *myHeap, FiboNode *node)
{
    fix_list_record *new_fix = new fix_list_record();

    new_fix->node = node;
    new_fix->type = 2;
    new_fix->rank = create_rank_record(myHeap->rank_list, 0);
    node->rank = new_fix->rank;
    add_to_fix_list(myHeap, new_fix, 2);

    return new_fix;
}

fix_list_record *promote_active_node_to_active_root(FiboHeap *myHeap, FiboNode *node)
{

    fix_list_record *new_fix = new fix_list_record();

    node->loss = 0;

    new_fix->node = node;
    new_fix->type = 2;
    new_fix->rank = node->rank;
    node->fix = new_fix;

    add_to_fix_list(myHeap, new_fix, 2);
    return new_fix;
}

fix_list_record *create_loss_record(FiboHeap *myHeap, FiboNode *node)
{
    fix_list_record *new_fix = new fix_list_record();

    new_fix->node = node;
    if (node->loss == 1)
        new_fix->type = 3;
    else
        new_fix->type = 5;

    new_fix->rank = node->rank;
    add_to_fix_list(myHeap, new_fix, new_fix->type);

    return new_fix;
}

void promote_active_root(FiboHeap *myHeap, fix_list_record *fix_record)
{

    rank_list_record *new_rank = fix_record->rank->inc;
    if (new_rank == NULL)
    {
        new_rank = create_rank_record(fix_record->rank, 1);
    }

    fix_record->rank = new_rank;

    fix_record->node->rank = new_rank;

    add_to_fix_list(myHeap, fix_record, 2);
}

/* Rank list utilities*/

rank_list_record *create_rank_record(rank_list_record *previous, int rank)
{
    if (previous == NULL && rank == 0)
    {

        previous = new rank_list_record();
        previous->rank = 0;
        return previous;
    }

    if (rank > 0)
    {
        if (previous->inc == NULL)
        {

            previous->inc = new rank_list_record();
            previous->inc->rank = previous->rank + 1;
            previous->inc->dec = previous;
        }
        return create_rank_record(previous->inc, rank - 1);
    }
    else
    {
        return previous;
    }

    return previous;
    if (previous == NULL)
    {
        previous = new rank_list_record();
        previous->rank = rank;

        return previous;
    }
    else if (previous->rank == rank)
    {
        return previous;
    }
    else
    {
        previous->inc = create_rank_record(previous->inc, rank - 1);
        return previous->inc;
    }
    if (previous != NULL && previous->inc != NULL)
    {

        return previous->inc;
    }

    if (previous != NULL && previous->rank == rank)
    {
        return previous;
    }
    if (previous != NULL)
    {
        previous->inc = new rank_list_record();
        previous->inc->rank = rank;
        previous->inc->dec = previous;
        return previous->inc;
    }
    rank_list_record *new_rank = new rank_list_record();
    new_rank->rank = rank;

    if (previous != NULL)
    {
        new_rank->dec = previous;
        previous->inc = new_rank;
    }

    return new_rank;
}

void decrease_rank(FiboHeap *myHeap, FiboNode *node)
{
    fix_list_record *to_decrease;

    if (node->fix != NULL)
    {
        to_decrease = remove_from_fix_list(myHeap, node->fix, node->fix->type);
        to_decrease->rank = to_decrease->rank->dec;

        if (to_decrease->type < 2)
            to_decrease->type = 2;
        else if (to_decrease->type == 3 || to_decrease->type == 4)
            to_decrease->type = 3;
        else if (to_decrease->type == 5)
            to_decrease->type = 5;

        add_to_fix_list(myHeap, to_decrease, to_decrease->type);
    }
    else
    {
        node->rank->ref_count--;
    }

    node->rank = node->rank->dec;
}

void increase_rank(FiboHeap *myHeap, FiboNode *node)
{
    rank_list_record *new_rank = node->rank->inc;

    if (new_rank == NULL)
    {
        new_rank = create_rank_record(node->rank, 1);
    }

    node->rank = new_rank;
    node->rank->ref_count++;
    cout << " HIIIIIIIIII" << endl;
}

/* Heap utilities*/
FiboNode *move_in_queue(FiboHeap *myHeap)
{
    if (myHeap->Q_head == NULL)
    {
        return NULL;
    }
    FiboNode *q_node = myHeap->Q_head;
    myHeap->Q_head = q_node->q_next;
    return q_node;
}

void remove_from_queue(FiboHeap *myHeap, FiboNode *node)
{
    if (node->q_next == node)
    {
        cout << "Stop imddietealy";
        myHeap->Q_head = NULL;
        return;
    }

    node->q_prev->q_next = node->q_next;
    node->q_next->q_prev = node->q_prev;

    if (myHeap->Q_head == node)
    {
        myHeap->Q_head = node->q_next;
    }
    node->q_next = NULL;
    node->q_prev = NULL;
}

struct FiboHeap *meld(FiboHeap *x, FiboHeap *y)
{
    x_tree_is_bigger(x, y);

    FiboHeap *new_heap = new FiboHeap();
    new_heap->size = x->size + y->size;
    new_heap->Q_head = x->Q_head;
    new_heap->active = x->active;
    new_heap->rank_list = x->rank_list;
    new_heap->non_linkable_child = x->non_linkable_child;
    new_heap->fix_list[1] = x->fix_list[1];
    new_heap->fix_list[2] = x->fix_list[2];
    new_heap->fix_list[3] = x->fix_list[3];
    new_heap->fix_list[4] = x->fix_list[4];
    new_heap->fix_list[5] = x->fix_list[5];

    if (y->active != NULL)
    {
        y->active->flag = false;
    }

    FiboNode *u = x->root;
    FiboNode *v = y->root;

    x_is_smaller(u, v);
    link(new_heap, v, u);

    new_heap->root = u;
    if (new_heap->Q_head != NULL)
    {
        new_heap->Q_head->q_prev->q_next = v;
        v->q_prev = new_heap->Q_head->q_prev;
        v->q_next = new_heap->Q_head;
        new_heap->Q_head->q_prev = v;
        new_heap->Q_head = v;
    }
    else
    {
        new_heap->Q_head = v;
        v->q_next = v;
        v->q_prev = v;
    }

    if (y->Q_head != NULL)
    {
        new_heap->Q_head->q_prev->q_next = y->Q_head;
        y->Q_head->q_prev = new_heap->Q_head->q_prev;
        y->Q_head->q_next = new_heap->Q_head;
        new_heap->Q_head->q_prev = y->Q_head;
    }
    bool result = false;
    loss_reduction(new_heap);

    int count_root = 0;
    int count_degree = 0;

    while (count_root < 1 || count_degree < 1)
    {
        if (count_root < 1 && active_root_reduction(new_heap))
            count_root++;
        else if (count_degree < 1 && root_degree_reduction(new_heap))
            count_degree++;
        else
            break;
    }

    return new_heap;
}

struct FiboHeap *insert_first_node(int value)
{
    FiboHeap *new_heap = new FiboHeap();
    new_heap->root = new FiboNode(value);
    new_heap->size++;
    return new_heap;
}

struct FiboHeap *insert_node(FiboHeap *myHeap, int value)
{
    FiboHeap *new_heap = insert_first_node(value);
    cout << "Insert " << value << endl;
    myHeap = meld(myHeap, new_heap);

    return myHeap;
}

void loss_to_active_root(FiboHeap *myHeap, fix_list_record *fix_record)
{
    fix_record->node->loss = 0;
    fix_record->node->active = myHeap->active;
    add_to_fix_list(myHeap, fix_record, 2);
}

void add_loss(FiboHeap *myHeap, FiboNode *node)
{
    if (node->fix != NULL && node->fix->type < 3)
    {
        return;
    }

    if (!is_active(node))
    {
        return;
    }
    node->loss++;
    if (!is_active(node))
    {
        return;
    }
    else if (node->fix != NULL && node->fix->type > 2)
    {
        fix_list_record *fix_record = remove_from_fix_list(myHeap, node->fix, node->fix->type);
        add_to_fix_list(myHeap, fix_record, 5);
    }
    else
    {
        create_loss_record(myHeap, node);
    }
    return;
}

FiboNode *find_new_root(FiboNode *node)
{
    FiboNode *new_root = node;
    FiboNode *step = node->right;

    while (node != step)
    {
        if (new_root->item > step->item)
        {
            new_root = step;
        }
        step = step->right;
    }

    cout << "This is the new root: " << new_root->item << endl;
    return new_root;
}

void loss_reduction(FiboHeap *myHeap)
{
    if (myHeap->fix_list[4] != NULL)
    {
        two_loss_reduction(myHeap);
    }
    else if (myHeap->fix_list[5] != NULL)
        one_loss_reduction(myHeap);
}

// Transformations

bool active_root_reduction(FiboHeap *myHeap)
{
    if (myHeap->fix_list[1] == NULL)
    {

        return false;
    }

    fix_list_record *active_fix_1;
    fix_list_record *active_fix_2;

    if (!check_fix_records(myHeap->fix_list[1], myHeap->fix_list[1]->right))
    {
        cout << "el problemo" << endl;
        active_fix_1 = remove_from_fix_list(myHeap, myHeap->fix_list[1], 1);
        add_to_fix_list(myHeap, active_fix_1, 2);
        active_root_reduction(myHeap);
        return false;
    }

    active_fix_1 = remove_from_fix_list(myHeap, myHeap->fix_list[1], 1);
    active_fix_2 = remove_from_fix_list(myHeap, myHeap->fix_list[1], 1);

    cout << "Active root reduction on " << active_fix_1->node->item << " and " << active_fix_2->node->item << " rank " << active_fix_1->rank->rank << endl;

    FiboNode *z;
    if (active_fix_1->node->item < active_fix_2->node->item)
    {

        link(myHeap, active_fix_2->node, active_fix_1->node);
        promote_active_root(myHeap, active_fix_1);

        if (active_fix_1->node->left_child != NULL)
        {

            z = active_fix_1->node->left_child->left;
            if (!is_active(z))
            {
                link(myHeap, z, myHeap->root);
            }
        }
    }
    else
    {
        link(myHeap, active_fix_1->node, active_fix_2->node);
        promote_active_root(myHeap, active_fix_2);

        if (active_fix_2->node->left_child != NULL)
        {
            z = active_fix_2->node->left_child->left;
            if (!is_active(z))
            {
                link(myHeap, z, myHeap->root);
            }
        }
    }

    return true;
}

bool root_degree_reduction(FiboHeap *myHeap)
{
    if (myHeap->root == NULL || myHeap->root->left_child == NULL)
    {
        return false;
    }

    FiboNode *x = myHeap->root->left_child->left;
    if (!is_passive_linkable(x))
        return false;

    FiboNode *y = x->left;

    if (!is_passive_linkable(y))
        return false;

    FiboNode *z = y->left;
    if (!is_passive_linkable(z))
        return false;

    if (x == y || x == z || y == z)
        return false;

    cout << "Root degree Reduction performed " << x->item << "," << y->item << "," << z->item << endl;
    sort_3_nodes(x, y, z);
    mark_as_active(myHeap, x);
    mark_as_active(myHeap, y);

    fix_list_record *new_fix = create_active_root(myHeap, x);
    new_fix = remove_from_fix_list(myHeap, new_fix, new_fix->type);
    promote_active_root(myHeap, new_fix);
    myHeap->root->left_child = x;

    link(myHeap, z, y);
    link(myHeap, y, x);
    link(myHeap, x, myHeap->root);

    return true;
}

void one_loss_reduction(FiboHeap *myHeap)
{
    fix_list_record *toRemove;

    toRemove = remove_from_fix_list(myHeap, myHeap->fix_list[5], 5);

    FiboNode *x = toRemove->node;
    FiboNode *y = x->parent;

    loss_to_active_root(myHeap, toRemove);

    decrease_rank(myHeap, y);
    add_loss(myHeap, y);

    link(myHeap, x, myHeap->root);

    cout << "One loss reduction on " << x->item << endl;
}

void two_loss_reduction(FiboHeap *myHeap)
{

    fix_list_record *fix_record_1;
    fix_list_record *fix_record_2;
    if (!check_fix_records(myHeap->fix_list[4], myHeap->fix_list[4]->right))
    {
        fix_record_1 = remove_from_fix_list(myHeap, myHeap->fix_list[4], 4);
        add_to_fix_list(myHeap, fix_record_1, 3);
        return;
    }

    fix_record_1 = remove_from_fix_list(myHeap, myHeap->fix_list[4], 4);
    fix_record_2 = remove_from_fix_list(myHeap, myHeap->fix_list[4], 4);

    FiboNode *x;
    FiboNode *y;
    FiboNode *z;

    if (fix_record_1->node->item < fix_record_2->node->item)
    {
        x = fix_record_1->node;
        y = fix_record_2->node;
        z = y->parent;
        link(myHeap, y, x);
        increase_rank(myHeap, x);
        x->loss = 0;
        y->loss = 0;
        decrease_rank(myHeap, z);
        add_loss(myHeap, z);
    }
    else
    {
        x = fix_record_2->node;
        y = fix_record_1->node;
        z = y->parent;
        link(myHeap, y, x);
        increase_rank(myHeap, x);
        x->loss = 0;
        y->loss = 0;
        decrease_rank(myHeap, z);
        add_loss(myHeap, z);
    }

    cout << "Two node loss on " << fix_record_1->node->item << " and " << fix_record_2->node->item << endl;
    return;
}

/*Heap Operations */

void delete_minumum(FiboHeap *myHeap)
{
    if (myHeap->root == NULL)
    {
        cout << "Heap is empty\n";
        return;
    }

    if (myHeap->root->left_child == NULL)
    {
        cout << "Heap is now empty as there are no more nodes\n";
        return;
    }

    FiboNode *new_root = myHeap->root->left_child;
    new_root = find_new_root(new_root);

    remove_from_siblings(new_root);
    remove_from_queue(myHeap, new_root);

    FiboNode *old_root = myHeap->root;

    myHeap->root = new_root;

    if (is_active(new_root))
    {
        make_passive(myHeap, new_root);
    }

    FiboNode *step;

    step = new_root->left_child;
    if (step != NULL)
    {
        do
        {
            if (is_active(step))
            {
                if (step->fix == NULL)
                {
                    promote_active_node_to_active_root(myHeap, step);
                }
                else
                {
                    step->fix = remove_from_fix_list(myHeap, step->fix, step->fix->type);
                    add_to_fix_list(myHeap, step->fix, 2);
                }
            }
            step = step->right;
        } while (step != new_root->left_child);
    }
    step = old_root->left_child;
    while (old_root->left_child != NULL)
    {
        link(myHeap, old_root->left_child, new_root);
    }

    FiboNode *q_node;
    FiboNode *q_child;
    for (int i = 0; i < 2; i++)
    {
        q_node = move_in_queue(myHeap);
        if (q_node != NULL)
        {
            cout << "Move to back " << q_node->item << endl;
            for (int j = 0; j < 2; j++)
            {
                if (q_node->left_child != NULL && !is_active(q_node->left_child->left))
                {
                    cout << "Link to root:" << q_node->left_child->item << endl;

                    link(myHeap, q_node->left_child->left, new_root);
                    add_loss(myHeap, q_node);
                }
                else
                {
                    break;
                }
            }
        }
    }

    loss_reduction(myHeap);
    FiboHeap *heap3 = myHeap;

    while (1)
    {
        if (active_root_reduction(myHeap))
            continue;
        else if (root_degree_reduction(myHeap))
            continue;
        else
            break;
    }
    // active_root_reduction(myHeap);
    myHeap->size--;
}

void decrease_key(FiboHeap *myHeap, FiboNode *node, int value)
{
    cout << "Decreasing node with key:" << node->item << endl;
    node->item = value;
    if (myHeap->root == node)
    {
        cout << "Decrease completed" << endl;
        return;
    }
    if (myHeap->root->item > node->item)
    {
        swap(myHeap->root->item, node->item);
    }

    FiboNode *y = node->parent;
    fix_list_record *fix_record;
    if (node->fix != NULL)
    {
        fix_record = remove_from_fix_list(myHeap, node->fix, node->fix->type);
    }

    link(myHeap, node, myHeap->root);

    if (is_active(node))
    {
        if (fix_record != NULL && (fix_record->type != 1 && fix_record->type != 2))
        {
            promote_active_node_to_active_root(myHeap, node);
            decrease_rank(myHeap, y);
        }
    }

    add_loss(myHeap, y);
    for (int i = 0; i < 6; i++)
    {
        if (active_root_reduction(myHeap))
            continue;
        else
            break;
    }

    for (int i = 0; i < 4; i++)
    {
        if (root_degree_reduction(myHeap))
            continue;
        else
            break;
    }
}

void delete_item(FiboHeap *myHeap, FiboNode *node)
{
    decrease_key(myHeap, node, -1);
    delete_minumum(myHeap);
    return;
}

int main()
{
    FiboNode *node;
    FiboHeap *heap3 = insert_first_node(9);

    auto start_time = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 10000000; i++)
    {
        heap3 = insert_node(heap3, i);
    }

    delete_minumum(heap3);
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    std::cout << "Time taken by program: " << duration.count() / 1000000.0 << " seconds" << std::endl;
    for (int i = 1; i < 6; i++)
    {
        if (heap3->fix_list[i] != NULL)
        {
            fix_list_record *temp = heap3->fix_list[i];
            cout << "part " << i << endl;
            do
            {
                cout << temp->node->item << "(" << temp->rank->rank << ") ";

                temp = temp->right;
            } while (temp != heap3->fix_list[i]);
        }
        cout << endl;
    }
    return 1;
    heap3 = insert_node(heap3, 1);
    heap3 = insert_node(heap3, 17);
    heap3 = insert_node(heap3, 12);
    heap3 = insert_node(heap3, 16);
    heap3 = insert_node(heap3, 6);
    heap3 = insert_node(heap3, 19);
    heap3 = insert_node(heap3, 5);
    heap3 = insert_node(heap3, 14);
    heap3 = insert_node(heap3, 0);
    heap3 = insert_node(heap3, 18);
    heap3 = insert_node(heap3, 10);
    heap3 = insert_node(heap3, 8);
    heap3 = insert_node(heap3, 3);
    heap3 = insert_node(heap3, 15);
    heap3 = insert_node(heap3, 11);
    heap3 = insert_node(heap3, 2);
    heap3 = insert_node(heap3, 7);
    heap3 = insert_node(heap3, 4);

    node = heap3->root->left_child->right->left_child->left_child;
    cout << node->item << endl;
    decrease_key(heap3, node, -3);
    delete_minumum(heap3);
    increase_rank(heap3, heap3->root->left_child);

    cout << heap3->root->left_child->rank->rank << endl;

    printFiboTree(heap3->root);
    return 1;
}
