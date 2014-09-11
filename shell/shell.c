#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

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
	
	int shellEnd = 0;
	char delim[1] = " ";

	char cmdBuff[SLEN];
	char * pch[5];

	int cmdFirst;
	int cmdCount;
	int cmdChar;
	int pchCount;
    
  	node *current = NULL;
   	node *head = NULL;
	node *prev = NULL;

	while (shellEnd == 0)
	{
		cmdCount = 0;
		cmdChar = 0;

		/*Input starts.*/
		cmdFirst = getchar();
		if (cmdFirst == '\n')
		{
			printf("$ ");
			continue;
		}/*For allways starts from $.*/
		else
		{
			cmdBuff[cmdCount] = cmdFirst;
			++cmdCount;
			/*Add first character back.*/

			while (cmdChar != '\n')
			{
				cmdChar = getchar();
				cmdBuff[cmdCount] = cmdChar;
				++cmdCount;
			}
			cmdBuff[cmdCount-1] = '\0';
		}
		/*Input ends.*/

		/*Seperate starts.*/
		pchCount = 0;
		pch[pchCount] = strtok(cmdBuff, delim);
		while (pch[pchCount] != NULL)
		{
			++pchCount;
			pch[pchCount] = strtok(NULL, delim);
		}
		/*Seperate ends.*/

		/*Command recognition starts.*/
		if (strcmp(pch[0], "exit") == 0)
		{
			printf("COMMAND: User terminate.\n");
			shellEnd = 1;
			break;
		}/*exit function.*/
        
		else if (strcmp(pch[0], "cd") == 0)
		{
			printf("COMMAND: Change to the directory: %s.\n", pch[1]);
			
			int cd = chdir(pch[1]);

			if (cd == 0)
			{
				printf("Current working directory: %s.\n", getcwd(NULL, 0));
			}	
			else if (cd < 0)
			{
				printf("ERROR: Invalid directory.\n");
			}
		}/*cd function.*/
        
        	else if (strcmp(pch[0], "path") == 0)
        	{
        		if (pch[1] != NULL && strcmp(pch[1], "+") == 0)
			{
                		printf("COMMAND: Add path: %s.\n", pch[2]);
                		
				current = (node *)malloc(sizeof(node));
                		if (current == NULL)
                		{
                   			exit(EXIT_FAILURE);
                		}/*Avoid for the failure for malloc.*/
                
                		current->next = NULL;
                
                		/*Setup a new node starts.*/
                		strncpy(current->pathChar, pch[2], SLEN-1);
				current->pathChar[SLEN-1] = '\0';
                
                		if (head == NULL)
                		{
                    			head = current;
                		}/*If first node than head is current.*/
                		else
                		{
                    			prev->next =current;
                		}/*Point prev to current.*/
                		prev = current;/*Set prev to current to point next node.*/
                		/*Setup a new node ends.*/
            		}/*Add path.*/
            		else if (pch[1] != NULL && strcmp(pch[1], "-") == 0)
            		{
                		printf("COMMAND: Delete path: %s.\n", pch[2]);
                		
				node *temp = NULL;
				int isInList = 0;

				current = head;

				if (strcmp(current->pathChar, pch[2]) == 0)
                		{
					temp = current;
                    			head = current->next;
					isInList = 1;
                		}/*If delete head.*/
                		else
                		{
                    			while (current != NULL)
                    			{
						if (strcmp(current->pathChar, pch[2]) == 0)
                        			{			
							temp = current;
                            				prev->next = current->next;
					                if (prev->next == NULL)
							{
								isInList = 1;
								break;
							}/*Ends loop if delete the last node.*/
							if (prev->next != NULL)
							{
								isInList = 1;
							}
						}
                        			prev = current;
                        			current = current->next;
                    			}
                		}/*If delete middle.*/
                		
				free(temp);

                		if (isInList == 0)
                		{
                    			printf("ERROR: Path not found.\n");
                		}/*If path isn't exits.*/
            		}/*Delete path.*/
			else
            		{
                		printf("COMMAND: Show all pathes.\n");
				current = head;
                		while (current != NULL)
                		{
                    			printf("%s", current->pathChar);
                    			current = current->next;
					if (current != NULL)
					{
						printf(" : ");
					}
					else if (current == NULL) 
					{
						break;
					}
                		}
				printf("\n");
            		}/*Show all path.*/
        	}/*path function.*/
		
		else if (strcmp(pch[0], "ls") == 0)
		{
			printf("COMMAND: List contents.\n");
			
			char filePath[SLEN];
			int fileFound = 0;
			FILE * fOpen = NULL;

			//printf("Breaker");
			/*Search for file starts*/
			current = head;
			while (current != NULL)
			{
				strcpy(filePath, current->pathChar);
				strcat(filePath, pch[0]);
				fOpen = fopen(filePath, "r");
				if (fOpen)
				{
					fclose(fOpen);
					fileFound = 1;
					break;
				}
				else
				{
					current = current->next;
				}
			}
			/*Search for file ends.*/

			if (fileFound)
			{
				pid_t pid;
				int status;

				pid = fork();

				if (pid < 0)
				{
					exit(EXIT_FAILURE);
				}
				if (pid == 0)
				{
					execl(filePath, pch[0], pch[1], NULL);
					pause();
				}
				if (pid != 0)
				{
					waitpid(pid, &status, WUNTRACED);
				}
			}
			else
			{
				printf("ERROR: Cannot find ls in directories stored in Path.\n");
			}
		}/*ls function*/

		else
		{
			printf("ERROR: Invalid command.\n");	
		}/*Wrong commands.*/
		
		printf("$ ");
	}
	return 0;
}
