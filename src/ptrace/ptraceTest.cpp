/*
 * ptraceTest.cpp
 *
 *  Created on: 2014-10-30
 *      Author: hong
 */
#include "../config.h"
#if PTRACETEST_CPP

#include <sys/ptrace.h>
#include <sys/reg.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <sys/syscall.h>
#include <sys/user.h>	/* define struct user_regs_struct */

/* define global value */
const int long_size = sizeof(long);

/* inner function */
void reverse(char *str);
void getdata(pid_t child, long addr, char *str, int len);
void putdata(pid_t child, long addr, char *str, int len);


int PtraceTest_1( int agrc, char *argv[] );
int PtraceTest_PeekUser( int agrc, char *argv[] );
int PtraceTest_GetRegs( int agrc, char *argv[] );
int PtraceTest_PokeData( int agrc, char *argv[] );
int PtraceTest_SingleStep(int argc, char *argv[]);
int PtraceTest_3( int agrc, char *argv[] );
int PtraceTest_4( int agrc, char *argv[] );
int PtraceTest_5( int agrc, char *argv[] );

int PtraceTest( int argc,char *argv[])	/* equal to main */
{
//	PtraceTest_1( argc, argv );
//	PtraceTest_PeekUser( argc, argv );
//	PtraceTest_GetRegs( argc, argv );
//	PtraceTest_PokeData( argc, argv );
	PtraceTest_SingleStep(argc, argv );
	return 0;
}

int PtraceTest_1( int agrc, char *argv[] )
{
	printf("father pid is %d\n", getpid() );
	pid_t child;
	long orig_eax;
	child = fork();
	if( child < 0 )
		printf("fork fail.\n");
	if( 0 == child )
	{
		printf("if pid is %d\n", getpid() );
		printf("if father pid is %d\n", getppid() );
		ptrace( PTRACE_TRACEME, 0, NULL, NULL);
		execl( "/bin/ls", "ls", NULL);
	}
	else
	{
		printf("else father pid is %d\n", getppid());
		wait(NULL);
		orig_eax = ptrace(PTRACE_PEEKUSER, child, 4*ORIG_EAX, NULL);
		printf("The child made a system call %ld.\n", orig_eax);
		ptrace(PTRACE_CONT, child, NULL, NULL);
	}
	return 0;
}

int PtraceTest_PeekUser( int agrc, char *argv[] )
{
	pid_t child;
	long orig_eax, eax;
	long params[3];
	int status;
	int insyscall = 0;
	child = fork();
	if(child == 0)
	{
		ptrace(PTRACE_TRACEME, 0, NULL, NULL);
		execl("/bin/ls", "ls", NULL);
	}
	else
	{
		while(1)
	   {
		  wait(&status);
		  if( WIFEXITED(status) )	break;
		  orig_eax = ptrace(PTRACE_PEEKUSER, child, 4 * ORIG_EAX, NULL);
		  static int cnt = 0;
		  std::cout<<" wait "<<++cnt <<std::endl;
		  if(orig_eax == SYS_write)
		  {
			  static int write_cnt = 0;
			  std::cout<<"SYS_write "<<++write_cnt <<std::endl;
			  if(insyscall == 0)	/* Syscall entry */
			  {
					insyscall = 1;
					params[0] = ptrace(PTRACE_PEEKUSER, child, 4 * EBX, NULL);
					params[1] = ptrace(PTRACE_PEEKUSER, child, 4 * ECX, NULL);
					params[2] = ptrace(PTRACE_PEEKUSER, child, 4 * EDX, NULL);
					printf("Write called with %ld, %ld, %ld \n",params[0], params[1],params[2]);
			  }
			  else	/* Syscall exit */
			  {
					eax = ptrace(PTRACE_PEEKUSER, child, 4 * EAX, NULL);
					printf("Write returned with %ld \n", eax);
					insyscall = 0;
			  }
		  }
		  ptrace(PTRACE_SYSCALL, child, NULL, NULL);
	   }/* while(1) */
	}
	return 0;
}

int PtraceTest_GetRegs( int agrc, char *argv[] )
{
	pid_t child;
	long orig_eax, eax;
	int status;
	int insyscall = 0;
	struct user_regs_struct regs;
	child = fork();
	if(child == 0)
	{
		ptrace(PTRACE_TRACEME, 0, NULL, NULL);
		execl("/bin/ls", "ls", NULL);
	}
	else
	{
		while(1)
		{
			wait(&status);
			if(WIFEXITED(status))	break;
			orig_eax = ptrace(PTRACE_PEEKUSER, child, 4 * ORIG_EAX, NULL);
			if(orig_eax == SYS_write)
			{
				if(insyscall == 0)	/* Syscall entry */
				{
					insyscall = 1;
					ptrace(PTRACE_GETREGS, child, NULL, &regs);
					printf("Write called with %ld, %ld, %ld \n",regs.ebx, regs.ecx,regs.edx);
				}
				else	/* Syscall exit */
				{
					eax = ptrace(PTRACE_PEEKUSER, child, 4 * EAX, NULL);
					printf("Write returned with %ld \n", eax);
					insyscall = 0;
				}
			}
          ptrace(PTRACE_SYSCALL, child, NULL, NULL);
		}/* while(1) */
	}/* if(child == 0) */
   return 0;
}

int PtraceTest_SingleStep(int argc, char *argv[])
{
	pid_t child;
	child = fork();
	if(child == 0)
    {
		std::cout<< "start child......"<< std::endl;
		ptrace(PTRACE_TRACEME, 0, NULL, NULL);
		execl("./hello", "hello", NULL);
    }
	else
    {
		std::cout<< "start father......"<< std::endl;
    	int status;
    	struct user_regs_struct regs;
    	int start = 0;
    	long ins;

    	while(1)
    	{
    		wait(&status);
    		if(WIFEXITED(status))	break;
    		ptrace(PTRACE_GETREGS, child, NULL, &regs);
    		if(start == 1)
    		{
    			ins = ptrace(PTRACE_PEEKTEXT, child, regs.eip, NULL);
    			printf("EIP: %lx Instruction executed: %lx \n", regs.eip, ins);
    		}
    		if(regs.orig_eax == SYS_write)
    		{
    			start = 1;
    			ptrace(PTRACE_SINGLESTEP, child, NULL, NULL);
            }
    		else
    		{
    			ptrace(PTRACE_SYSCALL, child, NULL, NULL);
    		}
    	}/* while(1) */
	}
	return 0;
}

int PtraceTest_PokeData(int argc, char *argv[])
{
	pid_t child;
	child = fork();
	if(child == 0)	/* into child process */
	{
		ptrace(PTRACE_TRACEME, 0, NULL, NULL);
		execl("/bin/ls", "ls", NULL);
	}
	else	/* into father process */
	{
		long orig_eax;
		long params[3];
		int status;
		char *str;
		int toggle = 0;
		while(1)
		{
			wait(&status);
			if(WIFEXITED(status))	break;
			orig_eax = ptrace(PTRACE_PEEKUSER, child, 4 * ORIG_EAX, NULL);
			if(orig_eax == SYS_write)
			{
				if(toggle == 0)
            	{
					toggle = 1;
					params[0] = ptrace(PTRACE_PEEKUSER, child, 4 * EBX, NULL);
					params[1] = ptrace(PTRACE_PEEKUSER, child, 4 * ECX, NULL);
					params[2] = ptrace(PTRACE_PEEKUSER, child, 4 * EDX, NULL);
					str = (char *)calloc( (params[2]+1),sizeof(char) );
					getdata(child, params[1], str, params[2]);
					reverse(str);
					putdata(child, params[1], str, params[2]);
            	}
				else
				{
					toggle = 0;
				}
			}
			ptrace(PTRACE_SYSCALL, child, NULL, NULL);
		}/* while(1) */
	}
	return 0;
}

void reverse(char *str)
{
	int i, j;
	char temp;
	for(i=0,j=strlen(str)-2; i<=j; ++i,--j)
	{
		temp = str[i];
		str[i] = str[j];
		str[j] = temp;
	}
}

void getdata(pid_t child, long addr, char *str, int len)
{
	char *laddr;
	int i, j;
	union u
    {
		long val;
		char chars[long_size];
	}data;

	i = 0;
	j = len / long_size;
	laddr = str;
	while(i < j)
	{
		data.val = ptrace(PTRACE_PEEKDATA, child, addr+i*4, NULL);
		memcpy(laddr, data.chars, long_size);
		++i;
		laddr += long_size;
	}
	j = len % long_size;
	if(j != 0)
	{
		data.val = ptrace(PTRACE_PEEKDATA, child, addr + i * 4, NULL);
		memcpy(laddr, data.chars, j);
    }
	str[len] = 0;
}

void putdata(pid_t child, long addr, char *str, int len)
{
	char *laddr;
	int i, j;
	union u
    {
    	long val;
    	char chars[long_size];
	}data;

	i = 0;
	j = len / long_size;
	laddr = str;
	while(i < j)
	{
		memcpy(data.chars, laddr, long_size);
		ptrace(PTRACE_POKEDATA, child, addr + i * 4, data.val);
		++i;
		laddr += long_size;
	}
	j = len % long_size;
	if(j != 0)
	{
		memcpy(data.chars, laddr, j);
		ptrace(PTRACE_POKEDATA, child, addr + i * 4, data.val);
    }
}


#endif /* PTRACETEST_CPP */
/* end file */
