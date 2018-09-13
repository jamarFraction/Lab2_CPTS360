#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct node
{

    char *name;
    char type;
    struct node *child;
    struct node *sibling;
    struct node *parent;

} Node;

//Globals/////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
Node *root, *cwd; //root and cwd pointer
char *line[128];  //user input command line
char command[16], pathname[64];
char *dname, *bname;
char *name[16];

char *cmd[] = {"mkdir", "rmdir", "ls", "cd", "pwd", "creat",
               "rm", "reload", "save", "menu", "quit", NULL};


//Function declarations///////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
int tokenize(char *pathname);
Node *insertLocation(char *pathname[], char targetType);
Node *removeLocation(char *pathname[], char targetType);
void Insert(char *baseName, char type, Node *location);
void Remove(char *baseName, char type, Node *target);
bool find(char *itemName, Node *parentDirectory);   //C sucks, can't overload doesExits
bool doesExist(char *itemName, char itemType, Node *parentDirectory);
Node *findDirectory(char *pathname[]);
int printDirectory(Node *directory);
int mkdir(char *pathname);
int rmdir(char *pathname);
int ls(char *pathname);
int cd(char *pathname);
int pwd(void);
void pwdRecurs(Node *location);
int creat(char *pathname);
void printNode(Node *ptr);

//holder of all that is hol-y
int (*fptr[])(char *) = {(int (*)(char *))mkdir, rmdir, ls, cd, pwd,
creat};

//Function defs///////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
void initialize()
{
    //Allocate space for a root node and init it's fields
    root = (Node *)malloc(sizeof(Node));

    root->name = "/";

    root->type = 'D';

    root->parent = root;

    root->child = NULL;

    root->sibling = NULL;

    //set the cwd to root
    cwd = root;
}

//returns the index of the given command in "char *cmd[]"
int findCmd(char *command)
{

    int i = 0;

    while (cmd[i])
    {

        if (!strcmp(command, cmd[i]))
        {

            //found command, return index i
            return i;
        }

        i++;
    }

    //not found, return -1
    return -1;
}

int mkdir(char *pathname)
{

    //results exist in global name[]
    int count = tokenize(pathname);

    //set basename
    bname = name[count - 1];

    //find dirName
    //create Node pointer for potential insertion location
    Node *location = insertLocation(name, 'D');

    if (location != NULL)
    {

        Insert(bname, 'D', location);
    }else{

        return -1;
    }

    return 1;
}

int rmdir(char *pathname){

    //results exist in global name[]
    int count = tokenize(pathname);

    //set basename
    bname = name[count - 1];

    //find dirName
    //create Node pointer for potential insertion location
    Node *location = removeLocation(name, 'D');

    if(location != NULL){

        //silly rabbit
        if(strcmp("/", location->name) == 0){

            printf("Cannot remove root. Operation failed.\n");
            return -1;
        }

        if(location->child == NULL)
        {
            Remove(bname, 'D', location);
            printf("Directory %s removed successfully.\n", bname);
            return 1;
        }else{
            printf("Directory %s is not empty. Operation failed.\n", bname);
            return -1;
        }   
    }
}

int ls(char *pathname){

    //results exist in global name[]
    int count = tokenize(pathname);

    //set basename
    bname = name[count - 1];

    //find dirName
    //create Node pointer for potential insertion location
    Node *location = findDirectory(name);

    //Error message already printed, item does not exist
    if(location == NULL){
        return -1;
    }

    //returns -1 if the passed location is not a directory
    //1 if completed successfully
    return printDirectory(location);
}

int cd(char *pathname){

    //results exist in global name[]
    int count = tokenize(pathname);

    //set basename
    bname = name[count - 1];

    //find dirName
    //create Node pointer for potential insertion location
    Node *location = findDirectory(name);

    //Error message already printed, item does not exist
    if(location == NULL){
        return -1;
    }else if(location->type != 'D'){

        //is this even a directory?
        printf("%s is not a directory. Operation failed.", location->name);
        return -1;
    }
    
    //change the working directory and print
    cwd = location;

    printf("cd to Directory : %s successful.\n", location->name);

    return 1;

}

int pwd(void){


    //start from the working directory
    Node *location = cwd;

    
    if(location->parent != location){

        printf("/");
        pwdRecurs(location);
        printf("\n");

        return 1;
    }

    //hey, its the root!
    printNode(location);

    return 1;

}

void pwdRecurs(Node *location){

    if(strcmp(location->parent->name, "/") != 0){

        pwdRecurs(location->parent);
    }
    
    printf("%s/", location->name);
    fflush;

    return;


}

int creat(char *pathname){

    //results exist in global name[]
    int count = tokenize(pathname);

    //set basename
    bname = name[count - 1];

    //find dirName
    //create Node pointer for potential insertion location
    Node *location = insertLocation(name, 'F');

    if (location != NULL)
    {

        Insert(bname, 'F', location);
    }else{

        return -1;
    }

    return 1;

}

int tokenize(char *pathname)
{
    int n = 0;
    char *curDirectory;
    char *pathCopy = (char *)malloc(sizeof(pathname));
    bool absolute = (pathname[0] == '/');

    //make a copy of the pathname as to not destroy the original
    strcpy(pathCopy, pathname);

    //add the root to it's own space in the array.. this will help
    //later in execution
    if (absolute)
    {

        name[n] = "/";
        n++;
    }

    //pull the first strtok for the lowest directory
    curDirectory = strtok(pathCopy, "/");

    while (curDirectory != NULL)
    {

        //add curDirectory to the token array
        name[n] = curDirectory;

        //advance to the next instance of "..../"
        curDirectory = strtok(NULL, "/");

        n++;
    }

    //return the number of tokens
    return n;
}

int printDirectory(Node *directory){

    if(directory->type != 'D'){
        printf("%s is not a directory. Operation failed.", directory->name);
        return -1;
    }

    //header
    printf("Printing directory : %s\n", directory->name);

    //Print the parent
    printNode(directory);

    //check for child
    if(directory->child != NULL){

        printNode(directory->child);

        //check for sibling
        if(directory->child->sibling != NULL){

            printNode(directory->child->sibling);
        }
    }

    return 1;

}

Node *findDirectory(char *pathname[]){

    int i = 0;
    int numOfTokens = getPathnameCount(name);
    int loopCycles = 0;
    Node *currentDirectory;

    //set the starting point of the search
    if (pathname[i] == "/")
    {
        //starting point
        currentDirectory = root;

        //increment i, we don't care about pathname[0] in this case
        i++;
    }
    else
    {
        //starting point
        currentDirectory = cwd;
    }

    //set the loop cycle 
    loopCycles = numOfTokens;


    //directory name comparisson loop
    //end target is the last name in the name[] array - 1
    for (; i < loopCycles; i++)
    {

        if (find(pathname[i], currentDirectory))
        {

            //check for if the pathname is the child's sibling
            //and not the child
            if (currentDirectory->child->sibling != NULL &&
                (strcmp(pathname[i], currentDirectory->child->sibling->name) == 0))
            {
                currentDirectory = currentDirectory->child->sibling;
            }
            else
            {
                currentDirectory = currentDirectory->child;
            }
        }
        else
        {
            printf("%c %s Does not exist. Operation failed.\n", currentDirectory->type, pathname[i]);
            return NULL;
        }
    }

    return currentDirectory; 
}

void printNode(Node *ptr){

    //easy peasy
    printf("%c %s\n", ptr->type, ptr->name);
}

Node *removeLocation(char *pathname[], char targetType){

    int i = 0;
    int numOfTokens = getPathnameCount(pathname);
    int loopCycles = 0;
    Node *currentDirectory;

    //set the starting point of the search
    if (pathname[i] == "/")
    {
        //starting point
        currentDirectory = root;

        //increment i, we don't care about pathname[0] in this case
        i++;
    }
    else
    {
        //starting point
        currentDirectory = cwd;
    }

    //set the loop cycle 
    loopCycles = numOfTokens;


    //directory name comparisson loop
    //end target is the last name in the name[] array
    for (; i < loopCycles; i++)
    {

        if (doesExist(pathname[i], targetType, currentDirectory))
        {

            //check for if the pathname is the child's sibling
            //and not the child
            if (currentDirectory->child->sibling != NULL &&
                (strcmp(pathname[i], currentDirectory->child->sibling->name) == 0))
            {
                currentDirectory = currentDirectory->child->sibling;
            }
            else
            {
                currentDirectory = currentDirectory->child;
            }
        }
        else
        {
            printf("%c %s Does not exist. Operation failed.\n", targetType, pathname[i]);
            return NULL;
        }
    }

    return currentDirectory;

}

Node *insertLocation(char *pathname[], char targetType)
{

    int i = 0;
    int numOfTokens = getPathnameCount(pathname);
    int loopCycles = 0;
    Node *currentDirectory;

    //set the starting point of the search
    if (pathname[i] == "/")
    {
        //starting point
        currentDirectory = root;

        //necessary for proper loop termination
        loopCycles = numOfTokens - 1;

        //increment i, we don't care about pathname[0] in this case
        i++;
    }
    else
    {
        //starting point
        currentDirectory = cwd;

        //necessary for proper loop termination
        if (numOfTokens > 2)
        {
            loopCycles = numOfTokens;
        }
        else
        {   
            //stop at the parent node, not the exact node itself
            loopCycles = numOfTokens - 1;
        }
    }

    //directory name comparisson loop
    //end target is the last name in the name[] array - 1
    for (; i < loopCycles; i++)
    {

        if (doesExist(pathname[i], targetType, currentDirectory))
        {

            //check for if the pathname is the child's sibling
            //and not the child
            if (currentDirectory->child->sibling != NULL &&
                (strcmp(pathname[i], currentDirectory->child->sibling->name) == 0))
            {

                currentDirectory = currentDirectory->child->sibling;
            }
            else
            {

                currentDirectory = currentDirectory->child;
            }
        }
        else
        {
            printf("%c %s Does not exist. Operation failed.\n", targetType, pathname[i]);
            return NULL;
        }
    }

    return currentDirectory;
}

int getPathnameCount(char *name[])
{
    int i = 0, count = 0;

    while (name[i] != NULL)
    {
        count += 1;
        i++;
    }

    return count;
}

void Remove(char *baseName, char type, Node *target)
{
    //can't lose track of your parents.. 
    //especially at the grocery store
    Node *parent = target->parent;

    //the node being deleted is the youngest child
    if(parent->child != target){
        
        parent->child->sibling = NULL;
        free(target);
        printf("Delete %s successful.\n", target->name);

        return;
    }else if(target->sibling != NULL){
        
        //solidify the link between parent and newly appointed oldest child
        parent->child = target->sibling;
    }else{

        //node to be removed is an only child
        parent->child = NULL;
    }

    //you gotta go
    free(target);
    printf("Delete %s successful.\n", target->name);

}
void Insert(char *baseName, char type, Node *parentDirectory)
{

    bool exists = doesExist(baseName, type, parentDirectory);

    //case for trying to insert an already existing directory
    if (exists)
    {
        printf("%c %s already exists. Operation failed.\n", type, baseName);
        return;
    }
    else if (parentDirectory->child != NULL &&
             parentDirectory->child->sibling != NULL)
    {

        //No space in either node
        printf("%s does not have have room for insertion. Operation failed.\n", parentDirectory->name);
        return;
    }

    //allocate memory for the node
    Node *newNode = (Node *)malloc(sizeof(Node));

    //assign the argument values to the new node
    newNode->name = baseName;
    newNode->type = type;
    newNode->parent = parentDirectory;

    //check was already done to see if there is space
    //now check to insert in child or child->sibling location
    if (parentDirectory->child == NULL)
    {
        parentDirectory->child = newNode;
    }
    else
    {
        parentDirectory->child->sibling = newNode;
    }

    printf("Insert %s successful\n", bname);
}

bool doesExist(char *itemName, char itemType, Node *parentDirectory)
{

    char *parent = parentDirectory->name;
    char *child = "", *sibling = "";

    //make sure the child dir is not null
    if (parentDirectory->child != NULL)
    {
        //set the child name
        child = parentDirectory->child->name;

        //check for the sibling existence
        if (parentDirectory->child->sibling != NULL)
        {
            //set the sibling name
            sibling = parentDirectory->child->sibling->name;
        }
    }

    //gross conditional block that checks for existence of itemName + type
    if (((strcmp(child, itemName) == 0) && parentDirectory->child->type == itemType) ||
        (strcmp(sibling, itemName) == 0) && parentDirectory->child->sibling->type == itemType)
    {

        return true;
    }

    //sorry fam, we don't got what you looking for
    return false;
}

bool find(char *itemName, Node *parentDirectory){

    char *parent = parentDirectory->name;
    char *child = "", *sibling = "";

    //make sure the child dir is not null
    if (parentDirectory->child != NULL)
    {
        //set the child name
        child = parentDirectory->child->name;

        //check for the sibling existence
        if (parentDirectory->child->sibling != NULL)
        {
            //set the sibling name
            sibling = parentDirectory->child->sibling->name;
        }
    }

    //gross conditional block that checks for existence of itemName + type
    if ((strcmp(child, itemName) == 0) ||
        (strcmp(sibling, itemName) == 0))
    {

        return true;
    }

    //sorry fam, we don't got what you looking for
    return false;
}