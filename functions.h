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

//Globals
Node *root, *cwd; //root and cwd pointer
char *line[128];  //user input command line
char command[16], pathname[64];
char *dname, *bname;
char *name[16];

char *cmd[] = {"mkdir", "rmdir", "ls", "cd", "pwd", "creat",
               "rm", "reload", "save", "menu", "quit", NULL};

int tokenize(char *pathname);
Node *insertLocation(char *pathname[], char targetType);
Node *removeLocation(char *pathname[], char targetType);
void Insert(char *baseName, char type, Node *location);
bool doesExist(char *itemName, char itemType, Node *parentDirectory);

//function declaratoins
int mkdir(char *pathname);
int rmdir(char *pathname);

//holder of all that is hol-y
int (*fptr[])(char *) = {(int (*)(char *))mkdir, rmdir};

//Function defs
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

    //set the dname
    //strncpy(dname, name[n-1], sizeof(dname) - 1);

    //return the number of tokens
    return n;
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

        //necessary for proper loop termination
        loopCycles = numOfTokens;

        //increment i, we don't care about pathname[0] in this case
        i++;
    }
    else
    {
        //starting point
        currentDirectory = cwd;
        loopCycles = numOfTokens;
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

    printf("Insert directory %s successful\n", bname);
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

    //gross conditional block that checks for existence of itemName in/is
    //the parent directory
    if (((strcmp(child, itemName) == 0) && parentDirectory->child->type == itemType) ||
        (strcmp(sibling, itemName) == 0) && parentDirectory->child->sibling->type ==itemType)
    {

        return true;
    }

    //sorry fam, we don't got what you looking for
    return false;
}