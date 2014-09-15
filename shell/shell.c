#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <errno.h>

# define SLEN 255
# define TRUE (1 == 1)
# define FALSE (0 == 1)

struct node {
	char            path[SLEN];
	struct node     *next;
};

struct node *head = NULL;
struct node *crnt = NULL;
struct node *prev = NULL;

char cmdBuff[SLEN];
char *cmdPch[5];
int pchCount;
char *ppPch[5];
int ppPchCt;

char shellEnd = FALSE;
char ppDetected = FALSE;
char filePath[SLEN];
int cmdFirst;
int cmdCount;
char cmdChar;

int exitcmd(void)
{
	return TRUE;
}

int cdcmd(void)
{
	int cd = chdir(cmdPch[1]);

	if (cd < 0) {
		printf("error: %s\n", strerror(errno));
		return FALSE;
	}
	return FALSE;
}

int pathcmd(void)
{
	if (cmdPch[1] != NULL
	    && strcmp(cmdPch[1], "+") != 0
	    && strcmp(cmdPch[1], "-") != 0) {
		printf("error: invaild arrguments\n");
	} else if (cmdPch[1] != NULL && strcmp(cmdPch[1], "+") == 0) {
		if (cmdPch[2] == NULL) {
			printf("error: no indicated path\n");
			return FALSE;
		}
		crnt = (struct node *)malloc(sizeof(struct node));
		if (crnt == NULL) {
			printf("error: malloc failed\n");
			return FALSE;
			/*Avoid for the failure or malloc.*/
		} else {
			crnt->next = NULL;
			/*Setup a new node starts.*/
			strncpy(crnt->path, cmdPch[2], SLEN-1);
			crnt->path[SLEN-1] = '\0';
			if (head == NULL) {
				head = crnt;
				/*If first node than head is current.*/
			} else {
				prev->next = crnt;
				/*Point prev to current.*/
			}
			prev = crnt;
			/*Set prev to current to point next node.*/
		}
		/*Setup a new node ends.*/
		/*Add path.*/
	} else if (cmdPch[1] != NULL && strcmp(cmdPch[1], "-") == 0) {
		struct node *temp = NULL;

		if (cmdPch[2] == NULL) {
			printf("error: no indicated path\n");
			return FALSE;
		}
		crnt = head;
		if (strcmp(crnt->path, cmdPch[2]) == 0) {
			temp = crnt;
			head = crnt->next;
			/*If delete head.*/
		} else {
			while (crnt != NULL) {
				if (strcmp(crnt->path, cmdPch[2]) == 0) {
					temp = crnt;
					prev->next = crnt->next;
					if (prev->next == NULL)
						break;
				}
				prev = crnt;
				crnt = crnt->next;
			}
			/*If delete middle.*/
		}
		free(temp);
		return FALSE;
		/*If path isn't exits.*/
		/*Delete path.*/
	} else {
		crnt = head;
		while (crnt != NULL) {
			printf("%s", crnt->path);
			crnt = crnt->next;
			if (crnt != NULL)
				printf(":");
			else if (crnt == NULL)
				break;
			/*Show all path.*/
		}
		printf("\n");
	}
	return FALSE;
}

int fsearch(void)
{
	char fileFound = FALSE;

	if (head == NULL) {
		strcat(filePath, cmdPch[0]);
		FILE *fp = fopen(filePath, "r");

		if (fp) {
			fclose(fp);
			fileFound = TRUE;
		}
	} else {
		crnt = head;
		while (crnt != NULL) {
			strcpy(filePath, crnt->path);
			strcat(filePath, "/");
			strcat(filePath, cmdPch[0]);
			FILE *fp = fopen(filePath, "r");

			if (fp) {
				fclose(fp);
				fileFound = TRUE;
				return fileFound;
			}
			crnt = crnt->next;
			/*Search for ls file*/
		}
	}
	if (fileFound == FALSE)
		printf("error: file not found\n");
	return fileFound;
}

int execcmd(void)
{
	pid_t pid;
	int status;

	pid = fork();
	if (pid < 0)
		printf("error: %s\n", strerror(errno));
	else if (pid == 0)
		execl(filePath, cmdPch[0], cmdPch[1],
		      cmdPch[2], cmdPch[3], cmdPch[4], NULL);
	else
		waitpid(pid, &status, 0);
	return FALSE;
}

int ppfsearch(void)
{
	char fileFound = FALSE;

	if (head == NULL) {
		strcat(filePath, ppPch[0]);
		FILE *fp = fopen(filePath, "r");

		if (fp) {
			fclose(fp);
			fileFound = TRUE;
		}
	} else {
		crnt = head;
		while (crnt != NULL) {
			strcpy(filePath, crnt->path);
			strcat(filePath, "/");
			strcat(filePath, ppPch[0]);
			FILE *fp = fopen(filePath, "r");

			if (fp) {
				fclose(fp);
				fileFound = TRUE;
				return fileFound;
			}
			crnt = crnt->next;
			/*Search for ls file*/
		}
	}
	if (fileFound == FALSE)
		printf("error: file not found\n");
	return fileFound;
}

int ppexeccmd(void)
{
	execl(filePath, ppPch[0], ppPch[1], ppPch[2], ppPch[3], ppPch[4], NULL);
	return FALSE;
}

int dopipe(void)
{
	if (pchCount == 1) {
		printf("error: invalid pipe command\n");
		return FALSE;
	}
	if (strcmp(&cmdBuff[0], "|") == 0) {
		printf("error: invalid pipe command\n");
		return FALSE;
	}
	/*Number of fd needs.*/
	int pfd[(2 * pchCount)];
	int *tempPfd[2];
	int *cPfd[2];
	int *nPfd[2];
	int status;
	pid_t pid;
	int ppNo;

	for (ppNo = 0; ppNo < pchCount; ++ppNo) {
		tempPfd[0] = &pfd[2 * ppNo + 0];
		tempPfd[1] = &pfd[2 * ppNo + 1];
		if (pipe(*tempPfd) < 0)
			printf("error: %s\n", strerror(errno));
	}

	ppNo = 0;
	/*Times of pipe command.*/
	for (ppNo = 0; ppNo < pchCount; ++ppNo) {
		if (ppNo == 0) {
			cPfd[0] = &pfd[2 * ppNo + 0];
			cPfd[1] = &pfd[2 * ppNo + 1];
			/*Separate pipe command.*/
			ppPchCt = 0;
			ppPch[ppPchCt] = strtok(cmdPch[ppNo], " ");
			while (ppPch[ppPchCt] != NULL) {
				++ppPchCt;
				ppPch[ppPchCt] = strtok(NULL, " ");
			}
			/*Separate pipe command.*/
			if (ppPch[0] == NULL) {
				printf("error: invalid pipe command\n");
				break;
			}
			status = 0;
			pid = fork();
			if (pid < 0) {
				printf("error: %s\n", strerror(errno));
			} else if (pid == 0) {
				dup2(*cPfd[1], STDOUT_FILENO);
				close(*cPfd[0]);
				close(*cPfd[1]);
				if (ppfsearch() == TRUE)
					ppexeccmd();
				else
					exit(0);
			} else {
				waitpid(pid, &status, 0);
			}
		} else if (ppNo == (pchCount - 1)) {
			cPfd[0] = &pfd[2 * (ppNo - 1) + 0];
			cPfd[1] = &pfd[2 * (ppNo - 1) + 1];
			/*Separate pipe command.*/
			ppPchCt = 0;
			ppPch[ppPchCt] = strtok(cmdPch[ppNo], " ");
			while (ppPch[ppPchCt] != NULL) {
				++ppPchCt;
				ppPch[ppPchCt] = strtok(NULL, " ");
			}
			/*Separate pipe command.*/
			if (ppPch[0] == NULL) {
				printf("error: invalid pipe command\n");
				break;
			}
			status = 0;
			pid = fork();
			if (pid < 0) {
				printf("error: %s\n", strerror(errno));
			} else if (pid == 0) {
				dup2(*cPfd[0], STDIN_FILENO);
				close(*cPfd[1]);
				close(*cPfd[0]);
				if (ppfsearch() == TRUE)
					ppexeccmd();
				else
					exit(0);
			} else {
				close(*cPfd[0]);
				close(*cPfd[1]);
				waitpid(pid, &status, 0);
			}
		} else {
			cPfd[0] = &pfd[2 * (ppNo - 1) + 0];
			cPfd[1] = &pfd[2 * (ppNo - 1) + 1];
			nPfd[0] = &pfd[2 * ppNo + 0];
			nPfd[1] = &pfd[2 * ppNo + 1];
			/*Separate pipe command.*/
			ppPchCt = 0;
			ppPch[ppPchCt] = strtok(cmdPch[ppNo], " ");
			while (ppPch[ppPchCt] != NULL) {
				++ppPchCt;
				ppPch[ppPchCt] = strtok(NULL, " ");
			}
			/*Separate pipe command.*/
			if (ppPch[0] == NULL) {
				printf("error: invalid pipe command\n");
				break;
			}
			status = 0;
			pid = fork();
			if (pid < 0) {
				printf("error: %s\n", strerror(errno));
			} else if (pid == 0) {
				dup2(*cPfd[0], STDIN_FILENO);
				close(*cPfd[1]);
				close(*cPfd[0]);
				dup2(*nPfd[1], STDOUT_FILENO);
				close(*nPfd[1]);
				close(*nPfd[0]);
				if (ppfsearch() == TRUE)
					ppexeccmd();
				else
					exit(0);
			} else {
				close(*cPfd[1]);
				close(*cPfd[0]);
				waitpid(pid, &status, 0);
			}
		}
	}
	return FALSE;
}

int dupbarcheck(void)
{
	int ct;
	int isDup = FALSE;

	for (ct = 0; ct < cmdCount; ++ct) {
		if (&cmdBuff[ct] != NULL && ppDetected == TRUE) {
			if (cmdBuff[ct] == '|'
			    && cmdBuff[ct] ==  cmdBuff[ct + 1]) {
				printf("error: invalid pipe command\n");
				isDup = TRUE;
			}
		}
	}
	return isDup;
}

int main(int argc, const char *argv[])
{
	while (shellEnd == FALSE) {
		cmdCount = 0;
		cmdChar = 0;
		ppDetected = FALSE;
		printf("$");

		/*Input starts.*/
		cmdFirst = getchar();
		if (cmdFirst == '|')
			ppDetected = TRUE;
		if (cmdFirst == '\t')
			cmdCount = cmdCount - 1;

		if (cmdFirst == '\n') {
			continue;
		} else {
			cmdBuff[cmdCount] = cmdFirst;
			++cmdCount;
			while (cmdChar != '\n') {
				cmdChar = getchar();
				if (cmdChar == '|')
					ppDetected = TRUE;
				cmdBuff[cmdCount] = cmdChar;
				if (cmdChar == '\t')
					cmdCount = cmdCount - 1;
				++cmdCount;
			}
			cmdBuff[cmdCount-1] = '\0';
		}
		/*Input ends.*/

		if (dupbarcheck() == TRUE)
			continue;

		/*Separate by | starts.*/
		pchCount = 0;
		cmdPch[pchCount] = strtok(cmdBuff, "|");
		while (cmdPch[pchCount] != NULL) {
			++pchCount;
			cmdPch[pchCount] = strtok(NULL, "|");
		}
		/*Separate by | ends.*/

		/*Command recognition starts.*/
		if (ppDetected == TRUE) {
			shellEnd = dopipe();
		} else {
			/*No pipe commands.*/
			/*Separate by space starts.*/
			pchCount = 0;
			cmdPch[pchCount] = strtok(cmdBuff, " ");
			while (cmdPch[pchCount] != NULL) {
				++pchCount;
				cmdPch[pchCount] = strtok(NULL, " ");
			}
			/*Separate by space ends.*/

			if (cmdPch[0] != NULL) {
				if (strcmp(cmdPch[0], "exit") == 0) {
					shellEnd = exitcmd();
					/*exit function.*/
				} else if (strcmp(cmdPch[0], "cd") == 0) {
					shellEnd = cdcmd();
					/*cd function.*/
				} else if (strcmp(cmdPch[0], "path") == 0) {
					shellEnd = pathcmd();
					/*path function.*/
				} else if (strcmp(cmdPch[0], "ls") == 0) {
					if (fsearch() == TRUE)
						shellEnd = execcmd();
					/*ls function*/
				} else if (fsearch() == TRUE) {
					shellEnd = execcmd();
				} else
					printf("error: invalid command\n");
				/*Wrong command.*/
			}
		}
	}
	return 0;
}
