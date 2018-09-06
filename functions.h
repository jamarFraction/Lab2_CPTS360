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
char *name[16];

char *cmd[] = {"mkdir", "rmdir", "ls", "cd", "pwd", "creat",
        "rm", "reload", "save", "menu", "quit", NULL};

int tokenize(char *pathname);

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

    //processPathname(pathname);
    int test = tokenize(pathname);

    printf("First name: %s \n", name[0]);

    return 1;
}

int tokenize(char *pathname)
{
    int n = 0;
    char *curDirectory;
    char *pathCopy = (char*)malloc(sizeof(pathname));
    bool absolute = (pathname[0] == '/');

    //make a copy of the pathname as to not destroy the original
    strcpy(pathCopy, pathname);
    
    //logic block for absolute pathnames
    if(absolute){
        curDirectory = strtok(pathCopy, "/") - 1;
    }else{
        curDirectory = strtok(pathCopy, "/");
    }
    
    while(curDirectory != NULL){

        name[n] = curDirectory;

        curDirectory = strtok(NULL, "/");

        n++;
    }

    //set the dname
    //strncpy(dname, name[n-1], sizeof(dname) - 1);

    //return the number of tokens
    return n;
}