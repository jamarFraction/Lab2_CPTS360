#include <stdio.h>
#include <stdlib.h>

void initialize();

typedef struct node{

    char name[64];
    char type;
    struct node* child;
    struct node* sibling;
    struct node* parent;

} Node;

Node *root, *cwd;   //root and cwd pointer
char *line[128];     //user input command line
char command[16], pathname[64];
char dname[64], bname[64];

int main(void){

    //initialize the file system
    initialize();

    while(1){



        break;
    }
    printf("hey\n");

    

    return 0;
}

void initialize(){

    //Allocate space for a root node and init it's fields
    root = (Node *) malloc(sizeof(Node));

    *root->name = '/';

    root->type = 'D';

    root->parent = NULL;

    root->child = NULL;

    root->sibling = NULL; 

    //set the cwd to root
    cwd = root;
}