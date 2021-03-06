#include "functions.h"


int main(void){

    int index;

    printf("Intitializing..\n");

    //run the init function
    initialize();

    printf("Done\n");

    while(1){

        //Prompt user for a command
        printf("Command: "); fflush(stdout);

        fgets(line, 128, stdin);

        //chop the tail off the line, set to null
        line[strlen(line)-1] = 0;

        //break the line up into commands and pathnames
        sscanf(line, "%s %s", command, pathname);

        //get the index from the command finder
        index = findCmd(command);

        //r will represent the return code.. see table
        int r = fptr[index](pathname);

        //reset line (name var) info.. cleanup
        for(int i = 0; i < 16; i++){
            name[i] = NULL;
            command[i] = NULL;
            pathname[i] = NULL;
        }

        for(int i = 16; i < 64; i++){
            pathname[i] = NULL;
        }
    }
    
    return 0;
}

