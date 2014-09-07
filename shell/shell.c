#include <stdio.h>
#include <string.h>
#include <unistd.h>
int main(int argc, char **argv) 
{
	printf("This is W4118 Homework 1.\n$ ");
	
	int cmdEnd = 0;
	char * delim = " ";

	char cmdBuff[99];
	char * pch[5];

	int cmdFirst;
	int cmdCount;
	int cmdChar;
	int pchCount;

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
		else
		{
			printf("Invalid command.\n");
			printf("$ ");	
		}//Wrong commands.

	}while (cmdEnd == 0);
	
	return 0;
}
