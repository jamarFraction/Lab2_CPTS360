#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


typedef struct node{

    char name[64];
    char type;
    struct node* child;
    struct node* sibling;
    struct node* parent;

} Node;

//Globals
Node *root, *cwd;   //root and cwd pointer
char *line[128];     //user input command line
char command[16], pathname[64];
char dname[64], bname[64];

char *cmd[] = {"mkdir", "rmdir", "ls", "cd", "pwd", "creat",
        "rm", "reload", "save", "menu", "quit", NULL};

int processPathname(char pathname[]);

//function declaratoins
int mkdir(char *pathname);
int rmdir(char *pathname);

//holder of all that is hol-y
int (*fptr[])(char *) = {(int (*)(char *))mkdir};

//Function defs
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

//returns the index of the given command in "char *cmd[]"
int findCmd(char *command){

    int i = 0;

    while(cmd[i]){

        if(!strcmp(command, cmd[i])){

            //found command, return index i
            return i;
        }
        
        i++;
    }

    //not found, return -1
    return -1;
}

int mkdir(char *pathname){

    processPathname(pathname);

    return 1;
}

int processPathname(char *pathname){

    char *test;
    char *curLevel, *prevLevel;
    bool absolute = (pathname[0] == '/');

    //toss a / at index 0 for absolute directories
    if (absolute == true){

        strcat(dname, "/");   
    }

    //first pull of strtok to get things started
    curLevel = strtok(pathname, "/");

    //outcome is the dname (pathname sans /baseName)
    while(curLevel != NULL){

        prevLevel = curLevel;

        curLevel = strtok(NULL, "/");
        
        if(curLevel != NULL){

            strcat(dname, prevLevel);
            strcat(dname, "/");
        }
    }

    //pop the last char of the dname (a /)
    dname[strlen(dname)-1] = 0;

    //set the bname
    strcat(bname, prevLevel);
}