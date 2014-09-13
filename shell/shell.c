#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>

# define SLEN 255
# define TRUE (1 == 1)
# define FALSE (0 == 1)

struct node
{
    	int             number;
    	char            pathChar[SLEN];
    	struct node     * next;
};

typedef struct node node;

int main(int argc, char **argv) 
{
	printf("This is W4118 Homework 1.\n");
	char shellEnd = FALSE;
	char pipeDetected = FALSE;

	char cmdBuff[SLEN];
	char * pch[10];

	int cmdFirst;
	int cmdCount;
	int cmdChar;
	int pchCount;
    
  	node *current = NULL;
   	node *head = NULL;
	node *prev = NULL;

	while (shellEnd == FALSE)
	{
		cmdCount = 0;
		cmdChar = 0;
		pipeDetected = FALSE;
		printf("$ ");

		/*Input starts.*/
		cmdFirst = getchar();
		if (cmdFirst == '\n')
		{
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
				if (cmdChar == '|')
				{
					pipeDetected = TRUE;
				}
			}
			cmdBuff[cmdCount-1] = '\0';
		}
		/*Input ends.*/

		/*Seperate 1 starts.*/
		pchCount = 0;
		pch[pchCount] = strtok(cmdBuff, "|");
		while (pch[pchCount] != NULL)
		{
			++pchCount;
			pch[pchCount] = strtok(NULL, "|");
		}
		/*Seperate 1 ends.*/

		if (pipeDetected == TRUE)
		{
			printf("COMMAND: Pipe detected.\n");
			int pfd[2];
			int status = 0;
			pid_t pid1, pid2;

			if (pipe(pfd) < 0)
			{
				printf("ERROR: Pipe create failed.\n");
			}/*Create pipe.*/

			pid1 = fork();
			if (pid1 < 0)
			{
				printf("ERROR: Fork create failed.\n");
			}
			else if (pid1 == 0)/*Code exec by child.*/
			{
				close(STDOUT_FILENO);
				dup2(pfd[1], STDOUT_FILENO);
				close(pfd[0]);
				execl("/bin/ls", "ls", NULL);
				pause();
			}
			else/**Code exec by parent.*/
			{
				waitpid(pid1, &status, 0);
				pid2 = fork();

				if (pid2 < 0)
				{
					printf("ERROR: Fork create failed.\n");
				}
				else if (pid2 == 0)
				{
					close(STDIN_FILENO);
					dup2(pfd[0], STDIN_FILENO);
					close(pfd[1]);
					execlp("wc", "wc", NULL);
					pause();
				}
				else
				{
					close(pfd[0]);
					close(pfd[1]);
					waitpid(pid2, &status, 0);
				}
				continue;
			}/*Two process pipe.*/
		}	
		else	
		{
			/*Seperate 2 starts.*/
			pchCount = 0;
			pch[pchCount] = strtok(cmdBuff, " ");
			while (pch[pchCount] != NULL)
			{
				++pchCount;
				pch[pchCount] = strtok(NULL, " ");
			}
			/*Seperate 2 ends.*/
			
			if (pch[0] != NULL)
			{
				/*Command recognition starts.*/
				if (strcmp(pch[0], "exit") == 0)
				{
					printf("COMMAND: User terminate.\n");
					shellEnd = TRUE;
					continue;
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
				continue;
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
						continue;
            				}/*Add path.*/
            				else if (pch[1] != NULL && strcmp(pch[1], "-") == 0)
            				{
                				printf("COMMAND: Delete path: %s.\n", pch[2]);
                				
						node *temp = NULL;
						int isInList = FALSE;

						current = head;

						if (strcmp(current->pathChar, pch[2]) == 0)
               			 		{
							temp = current;
                    					head = current->next;
							isInList = TRUE;
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
         					continue;
			   		}/*Delete path.*/

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
                			}/*Show all path.*/
					printf("\n");
					continue;
		        	}/*path function.*/
		
				else if (strcmp(pch[0], "ls") == 0)
				{
					printf("COMMAND: List contents.\n");
			
					char filePath[SLEN];
					char fileFound = FALSE;
					
					/*Search for file starts*/
					current = head;
					while (current != NULL)
					{
						strcpy(filePath, current->pathChar);
						strcat(filePath, pch[0]);
						FILE * fp = fopen(filePath, "r");
						if (fp)
						{
							fclose(fp);
							fileFound = TRUE;
							break;
						}
						else
						{
							current = current->next;
						}
					}
					/*Search for file ends.*/

					if (fileFound == TRUE)
					{
						pid_t pid;
						int status;

						pid = fork();

						if (pid < 0)
						{
							exit(EXIT_FAILURE);
						}
						else if (pid == 0)/*Code exec by child.*/
						{
							execl(filePath, pch[0], pch[1], NULL);
							pause();
						}
						if (pid != 0)
						{
							waitpid(pid, &status, 0);
						}
					}
					else
					{
						printf("ERROR: Cannot find ls in directories stored in Path.\n");
					}
					continue;
				}/*ls function*/
				else
				{
					printf("ERROR: Invalid command.\n");	
				}/*Wrong commands.*/
			}
		}
	}
	return 0;
}
