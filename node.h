typedef struct node{

    char name[64];
    char type;
    Node *child;
    Node *sibling;
    Node *parent;

}Node;