struct FiboNode
{
    int item, loss;
    struct FiboNode *left;
    struct FiboNode *right;
    struct FiboNode *parent;
    struct FiboNode *left_child;
    struct active_record *active;
    struct FiboNode *q_prev;
    struct FiboNode *q_next;
    struct rank_list_record *rank;
    struct fix_list_record *fix;

    FiboNode(int item = -1) : item(item), fix(nullptr), loss(0), left(this), right(this), parent(nullptr), left_child(nullptr),
                              active(nullptr), q_prev(nullptr), q_next(nullptr), rank(nullptr) {}
};

/**
 * Fix list record struct holding the node in case, it's siblings as for each part it is a double linked
 * list. A rank record to the corresponding rank list record.
 */
struct fix_list_record
{
    struct FiboNode *node;
    struct fix_list_record *left;
    struct fix_list_record *right;
    struct rank_list_record *rank;
    int type;

    fix_list_record() : node(nullptr), left(nullptr), right(nullptr), rank(nullptr), type(-1) {}
};

/**
 * Rank list record that holds an integer to the rank of the record
 * a pointer to the next and previous rank if that exists, otherwise null
 * A fix list record, showing to the fix list record of the rank in case that is a loss state
 * A fix list record that points to the active roots of the fix list record of that rank.
 */
struct rank_list_record
{
    int rank;
    struct rank_list_record *inc;
    struct rank_list_record *dec;
    struct fix_list_record *fixes[6];

    int ref_count;

    rank_list_record() : rank(0), inc(nullptr), dec(nullptr), ref_count(0) {}
};

struct active_record
{
    bool flag;
    int ref_count;
    active_record() : flag(false), ref_count(0) {}
};

struct FiboHeap
{
    int size;
    struct FiboNode *root;
    struct active_record *active;
    struct FiboNode *non_linkable_child;
    struct FiboNode *linkable_child;
    struct FiboNode *Q_head;
    struct rank_list_record *rank_list;
    struct fix_list_record *fix_list[6];

    FiboHeap() : size(0), root(nullptr), active(nullptr),
                 non_linkable_child(nullptr), linkable_child(nullptr), Q_head(nullptr), rank_list(nullptr) {}
};
