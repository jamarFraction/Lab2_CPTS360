#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


typedef struct node{

    char *name;
    char type;
    struct node* child;
    struct node* sibling;
    struct node* parent;

} Node;

//Globals
Node *root, *cwd;   //root and cwd pointer
char *line[128];     //user input command line
char command[16], pathname[64];
char *dname, *bname;
char *name[16];

char *cmd[] = {"mkdir", "rmdir", "ls", "cd", "pwd", "creat",
        "rm", "reload", "save", "menu", "quit", NULL};

int tokenize(char *pathname);
Node *canInsert(char *pathname[], char targetType);
void Insert(char *baseName, char type, Node *location);
bool doesExist(char *itemName, char itemType, Node *parentDirectory);

//function declaratoins
int mkdir(char *pathname);
int rmdir(char *pathname);

//holder of all that is hol-y
int (*fptr[])(char *) = {(int (*)(char *))mkdir};

//Function defs
void initialize(){
    //Allocate space for a root node and init it's fields
    root = (Node *) malloc(sizeof(Node));

    root->name = "/";

    root->type = 'D';

    root->parent = root;

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

    //results exist in global name[]
    int count = tokenize(pathname);

    //set basename
    bname = name[count - 1];

    //find dirName
    //create Node pointer for potential insertion location
    Node *insertLocation = canInsert(name, 'D');

    if(insertLocation != NULL){

        Insert(bname, 'D', insertLocation);
    }

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
    
    //add the root to it's own space in the array.. this will help
    //later in execution
    if(absolute){

        name[n] = "/";
        n++;
    }

    //pull the first strtok for the lowest directory
    curDirectory = strtok(pathCopy, "/");
    
    
    while(curDirectory != NULL){

        //add curDirectory to the token array
        name[n] = curDirectory;

        //advance to the next instance of "..../"
        curDirectory = strtok(NULL, "/");

        n++;
    }

    //set the dname
    //strncpy(dname, name[n-1], sizeof(dname) - 1);

    //return the number of tokens
    return n;
}

Node *canInsert(char *pathname[], char targetType){

    int i = 0;
    int numOfTokens = getPathnameCount(pathname);
    Node *currentDirectory;

    //set the starting point of the search
    if(pathname[i] == "/"){
        currentDirectory = root;
    }
    else{
        currentDirectory = cwd;
    }

    //directory name comparisson loop
    //end target is the last name in the name[] array - 1
    for(; i < numOfTokens - 1; i++){

        if(currentDirectory != NULL &&
            strcmp(pathname[i], currentDirectory->name) == 0){// pathname[i] == currentDirectory->name){
            
            //the loop will iterate again, go 1 level deeper
            if(i + 1 != numOfTokens -1){
                currentDirectory = currentDirectory->child;
            }

        }else if(currentDirectory != NULL &&
            strcmp(pathname[i], currentDirectory->sibling->name) == 0){
            
            currentDirectory = currentDirectory->sibling;

            //the loop will iterate again, go 1 level deeper
            if((i + 1) != (numOfTokens -1)){

                currentDirectory = currentDirectory->child;
            }
        }else{

            //Non-existent directory
            printf("Directory %s does not exist! Operation failed\n", pathname[i]);
            return NULL;
        }
    }


    //Final check is for space in the directory's child + sibling nodes
    if(currentDirectory->child == NULL ||
        currentDirectory->child->sibling == NULL){

            return currentDirectory;
    }

    //No space in either node
    printf("%s does not have have room for insertion! Operation failed!\n", currentDirectory->name);
    return NULL;
}

int getPathnameCount(char *name[]){
    int i = 0, count = 0;

    while(name[i] != NULL){
        count += 1;
        i++;
    }

    return count;
}

void Insert(char *baseName, char type, Node *parentDirectory){

    bool exists = doesExist(baseName, type, parentDirectory);

    //case for trying to insert an already existing directory
    if(exists){
        printf("Directory %s already exists. Operation failed\n", baseName);
        return;
    }
        
    //allocate memory for the node
    Node *newNode = (Node *) malloc(sizeof(Node));

    //assign the argument values to the new node
    newNode->name = baseName;
    newNode->type = type;
    newNode->parent = parentDirectory;

    //check was already done to see if there is space
    //now check to insert in child or child->sibling location
    if(parentDirectory->child == NULL){
        parentDirectory->child = newNode;
    }else{
        parentDirectory->child->sibling = newNode;
    }

    printf("Insert directory %s successful\n", bname);
}

bool doesExist(char *itemName, char itemType, Node *parentDirectory){

    char *parent = parentDirectory->name;
    char *child = "", *sibling = "";
    
    if(parentDirectory->child != NULL){

        child = parentDirectory->child->name;

        if(parentDirectory->child->sibling != NULL){

            sibling = parentDirectory->child->sibling->name;
        }
    }

    //gross conditional block that checks for existence of itemName in/is
    //the parent directory
    if((strcmp(parent, itemName) == 0 ||
        strcmp(child, itemName) == 0 ) ||
        strcmp(sibling, itemName) == 0){

            return true;

    }

    
    return false;
}