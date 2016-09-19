/*
 * fatherForkAndWait.cpp
 *
 *  Created on: 2015-6-25
 *      Author: hongjianan
 */
#include "../config.h"
#if FATHERFORKANDWAIT_CPP

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <errno.h>


int FatherForkAndWait_NoCacheChild(int argc, char *argv[]);
int FatherForkAndWait_CacheChild(int argc, char *argv[]);

/*====================main=================*/
int FatherForkAndWait(int argc, char *argv[])
{
	int ret;

//	ret = FatherForkAndWait_CacheChild(argc, argv);
	ret = FatherForkAndWait_NoCacheChild(argc, argv);

	return ret;
}

int FatherForkAndWait_CacheChild(int argc, char *argv[])
{
	if (2 != argc)
	{
		printf("usage: [%s] [program name]\n", argv[0]);
		exit(-1);
	}

	int status;
	pid_t childPid, exitPid;

	childPid = fork();
	if (childPid < 0)
	{
		perror("fork error");
		exit(errno);
	}
	if (childPid == 0)
	{
		execl(argv[1], argv[1], NULL);
	}
	else
	{
		time_t timeNow;
		char fileName[64];

		sprintf(fileName, "%s.log", argv[1]);
		FILE *fileFd = fopen(fileName, "wb+");

		time(&timeNow);
		fprintf(fileFd, "The child start time: %s", ctime(&timeNow));
		fflush(fileFd);

		while (1)
		{
			exitPid = waitpid(childPid, &status, 0);

			if (exitPid == childPid)	/* child process exit */
			{
				time(&timeNow);
				fprintf(fileFd, "The child end   time: %s\n", ctime(&timeNow));
				fprintf(fileFd, "The child process exit, pid=%d status=%d\n", exitPid, status);

				if (WIFEXITED(status))
				{
					fprintf(fileFd, "The child exit normally.\n");
					fprintf(fileFd, "The exit code is %d.\n", WEXITSTATUS(status));
					break;
				}

				else if (WIFSIGNALED(status))
				{
					fprintf(fileFd, "The child killed by signal.\n");
					fprintf(fileFd, "The signal number is %d\n", WTERMSIG(status));
					if (WCOREDUMP(status))
					{
						fprintf(fileFd, "The signal is coredump.\n");
					}
					break;
				}

				else if (WIFSTOPPED(status))
				{
					fprintf(fileFd, "The child stopped by signal.\n");
					fprintf(fileFd, "The signal number is %d.\n", WSTOPSIG(status));
					break;
				}

				else if (WIFCONTINUED(status))
				{
					fprintf(fileFd, "The child continued by signal.\n", exitPid);
					fprintf(fileFd, "The signal is SIGCONT.\n");
					break;
				}

				else
				{
					fprintf(fileFd, "unkown reason.\n");
					break;
				}
			}
		}/* while(1) */

		fflush(fileFd);
		fclose(fileFd);
	}

	return 0;
}

int FatherForkAndWait_NoCacheChild(int argc, char *argv[])
{
	if (2 != argc)
	{
		printf("usage: [%s] [program name]\n", argv[0]);
		exit(-1);
	}

	pid_t childPid;

	childPid = fork();
	if (childPid < 0)
	{
		perror("fork error");
		exit(errno);
	}
	if (childPid == 0)
	{
		execl(argv[1], argv[1], NULL);
	}

	while (1)
	{
		sleep(100);
	}

	return 0;
}


#endif /* FATHERFORKANDWAIT_CPP */
/* end file */
