#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
# define SLEN 255

struct node
{
    	int             number;
    	char            pathChar[SLEN];
    	struct node     * next;
};

typedef struct node node;

int main(int argc, char **argv) 
{
		printf("This is W4118 Homework 1.\n$ ");
	
		int cmdEnd = 0;
		char delim[1] = " ";
		int listCount = 0;

		char cmdBuff[SLEN];
		char * pch[5];

		int cmdFirst;
		int cmdCount;
		int cmdChar;
		int pchCount;
    
   		node *current = NULL;
   		node *head = NULL;
		node *prev = NULL;

	do
	{
		cmdCount = 0;
		cmdChar = 0;

		//Input starts.
		cmdFirst = getchar();
		if (cmdFirst == '\n')
		{
			printf("$ ");
			continue;
		}//For allways starts from $.
		else
		{
			cmdBuff[cmdCount] = cmdFirst;
			++cmdCount;
			//Add first character back.

			while (cmdChar != '\n')
			{
				cmdChar = getchar();
				cmdBuff[cmdCount] = cmdChar;
				++cmdCount;
			}
			cmdBuff[cmdCount-1] = '\0';
		}
		//Input ends.

		//Seperate starts.
		pchCount = 0;
		pch[pchCount] = strtok(cmdBuff, delim);
		while (pch[pchCount] != NULL)
		{
			++pchCount;
			pch[pchCount] = strtok(NULL, delim);
		}
		//Seperate ends.

		//Command recognition starts.
		if (strcmp(pch[0], "exit") == 0)
		{
			printf("User terminate.\n");
			cmdEnd = 1;
		}//exit function.
        
		else if (strcmp(pch[0], "cd") == 0)
		{
			printf("Change directory.\n");
			printf("Target directory is %s.\n", pch[1]);
			if (chdir(pch[1]) < 0)
			{
				printf("Invalid directory.\n");
				printf("$ ");
			}
		}//cd function.
        
        	else if (strcmp(pch[0], "path") == 0)
        	{
        		if (strcmp(pch[1], "+") == 0)
			{
                		printf("Add path %s.\n", pch[2]);
                		
				current = (node *)malloc(sizeof(node));
                		if (current == NULL)
                		{
                   			exit(EXIT_FAILURE);
                		}//Avoid for the failure or malloc.
                
                		current->next = NULL;
                
                		//Set-up a new node.
                		current->number = listCount;
                		strncpy(current->pathChar, pch[2], SLEN-1);
				current->pathChar[SLEN-1] = '\0';
                
                		if (head == NULL)
                		{
                    			head = current;
                		}//If first node than head is current.
                		else
                		{
                    			prev->next =current;
                		}//Point prev to current.
                		prev = current;//Set prev to current to point next node.
                		++listCount;
                		//End of set-up a new node.
            		}//Add path.
            		else if (strcmp(pch[1], "-") == 0)
            		{
                		printf("Delete path %s.\n", pch[2]);
                		
				node *temp = NULL;
				int isInList = 0;

				current = head;

				if (strcmp(current->pathChar, pch[2]) == 0)
                		{
					temp = current;
                    			head = current->next;
					//temp = (node *)realloc(temp, sizeof(node));
                    			isInList = 1;
                		}//If delete head.
                		else
                		{
                    			while (current != NULL)
                    			{
						if (strcmp(current->pathChar, pch[2]) == 0)
                        			{			
							temp = current;
                            				prev->next = current->next;
							//temp = (node *)realloc(temp, sizeof(node));
                            				if (prev->next == NULL)
							{
								isInList = 1;
								break;
							}//End loop if delete the last node.
							if (prev->next != NULL)
							{
								isInList = 1;
							}
						}
                        			prev = current;
                        			current = current->next;
                    			}
                		}//If delete middle.
                		
				free(temp);

                		if (isInList == 0)
                		{
                    			printf("Path not found.\n");
                		}//If path isn't exits.
            		}//Delete path.
			else if (strcmp(pch[1], "list") == 0)
            		{
                		current = head;
                		while (current != NULL)
                		{
                    			printf("Path No.%d = %s\n", current->number+1, current->pathChar);
                    			current = current->next;
                		}
            		}//Show all path.
            		printf("$ ");
        	}//path function.
        
		else
		{
			printf("Invalid command.\n");
			printf("$ ");	
		}//Wrong commands.

	}while (cmdEnd == 0);
	return 0;
}
