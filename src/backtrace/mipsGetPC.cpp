/*
 * mipsGetPC.cpp
 *
 *  Created on: 2015-1-1
 *      Author: hong
 */
#include "../config.h"
#if MIPSGETPC_CPP

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <dlfcn.h>
#include <ucontext.h>

struct StackTrace
{
	unsigned int *bfp;
	unsigned int *fp;
};

void RegisterSignal(int signo);
void SigHandler(int signo, siginfo_t *info, void *dummy);
unsigned int *GetPC(void);
unsigned int *GetSP(void);

void PrintFuncName(unsigned int *ra);
void PrintStackTrace(void);
void Myfunc_01(int cnt);
void Myfunc_02(int cnt);
void Myfunc_03(void);
void Myfunc_04(void);
void FuncSegfault(void);

int MipsGetPC_PrintStackWithSignal(int argc, char *argv[]);
int MipsGetPC_PrintStackNoSignal(int argc, char *argv[]);

/*==============main===============*/
int MipsGetPC(int argc, char *argv[])
{
	int ret;

	/* use asm get ra, fp, sp registers */
	unsigned int *__ra = NULL;
	unsigned int *__sp = NULL;
	unsigned int *__fp = NULL;

	__asm__ volatile("move %0, $29" : "=r"(__sp));	/* sp */
	__asm__ volatile("move %0, $30" : "=r"(__fp));	/* fp */
	__asm__ volatile("move %0, $31" : "=r"(__ra));	/* ra */

	printf("MipsGetPC sp=[0x%08X] *sp=[0x%08X]\n", __sp, *__sp);
	printf("MipsGetPC fp=[0x%08X] *fp=[0x%08X]\n", __fp, *__fp);
	printf("MipsGetPC ra=[0x%08X] ", __ra);
	PrintFuncName(__ra);
	printf("MipsGetPC addr [0x%08X]\n", MipsGetPC);
	printf("\n");

	ret = MipsGetPC_PrintStackWithSignal(argc, argv);
//	ret = MipsGetPC_PrintStackNoSignal(argc, argv);

	return ret;
}


int MipsGetPC_PrintStackWithSignal(int argc, char *argv[])
{
	if (2 != argc)
	{
		printf("usage:[loop times]\n"), exit(-1);
	}

	int loopTimes = atoi(argv[1]);

	RegisterSignal(SIGSEGV);

	Myfunc_01(loopTimes);

	return 0;
}


int MipsGetPC_PrintStackNoSignal(int argc, char *argv[])
{
#if 0
	if (2 != argc)
	{
		printf("usage:[loop times]\n"), exit(-1);
	}
	int loopTimes = atoi(argv[1]);
#endif
	/* use asm get ra, fp, sp registers */
	unsigned int *__ra = NULL;
	unsigned int *__sp = NULL;
	unsigned int *__fp = NULL;

	__asm__ volatile("move %0, $29" : "=r"(__sp));	/* sp */
	__asm__ volatile("move %0, $30" : "=r"(__fp));	/* fp */
	__asm__ volatile("move %0, $31" : "=r"(__ra));	/* ra */

	printf("MipsGetPC_PrintStackNoSignal sp=[0x%08X] *sp=[0x%08X]\n", __sp, *__sp);
	printf("MipsGetPC_PrintStackNoSignal fp=[0x%08X] *fp=[0x%08X]\n", __fp, *__fp);
	printf("MipsGetPC_PrintStackNoSignal ra=[0x%08X] ", __ra);
	PrintFuncName(__ra);
	printf("MipsGetPC_PrintStackNoSignal addr [0x%08X]\n", MipsGetPC_PrintStackNoSignal);
	printf("\n");

	Myfunc_03();

	return 0;
}


void RegisterSignal(int signo)
{
	struct sigaction act;
	sigemptyset( &act.sa_mask );
	act.sa_flags = SA_SIGINFO;
	act.sa_sigaction = SigHandler;

	if (sigaction(signo, &act, NULL) < 0)
	{
		printf("install signal error.\n"), exit(-1);
	}
}

void SigHandler(int signo, siginfo_t *info, void *context_ptr)
{
	/* use asm get ra, fp, sp registers */
	unsigned int *__ra = NULL;
	unsigned int *__sp = NULL;

	__asm__ volatile("move %0, $29" : "=r"(__sp));	/* sp */
	__asm__ volatile("move %0, $31" : "=r"(__ra));	/* ra */

	int cnt = 0;
	while ((unsigned int)__sp < 0x7FFFFFFF)
	{
		while ((unsigned int)__sp != (*__sp - 8))
		{
			++__sp;
		}
		__ra = (unsigned int *)*(++__sp);
		++__sp;
		printf("<%03d> ra=[0x%08X] \n", cnt, __ra);
		PrintFuncName(__ra);
		++cnt;
	}

	/* return signo to system */
	signal(signo, SIG_DFL);
	raise(signo);
}

unsigned int *GetPC(void)
{
	static unsigned int *pc_ra = NULL;
	__asm__ volatile ("move %0, $31" : "=r"(pc_ra));

	return pc_ra;
}

unsigned int *GetSP(void)
{
	static unsigned int *__sp = NULL;
	__asm__ volatile ("move %0, $29" : "=r"(__sp));

	return __sp;
}

void PrintFuncName(unsigned int *ra)
{
	Dl_info dlip;

	dladdr(ra, &dlip);
	printf("[%s]\n", dlip.dli_sname);
}

void PrintStackTrace(void)
{
#if 0
	/* get regs */
	int index;
	unsigned int regs[32] = {0};
	char command[32] = {0};
	for (index = 0; index < 32; index++)
	{
		sprintf(command, "move %0, $%d", index);
		__asm__ volatile(command : "=r"(regs[index]));	/* sp */
		printf("<reg %d> [0x%08X]\n", index, regs[index]);
	}
	printf("+++++++++++++++++++++\n");
#endif

	int i;
	/* use asm get ra, fp, sp registers */
	unsigned int *__ra = NULL;
	unsigned int *__sp = NULL;
	unsigned int *__fpCopy = NULL;

	__asm__ volatile("move %0, $29" : "=r"(__sp));	/* sp */
	__asm__ volatile("move %0, $31" : "=r"(__ra));	/* ra */

	printf("PrintStackTrace sp=[0x%08X] *sp=[0x%08X]\n", __sp, *__sp);
	printf("PrintStackTrace ra=[0x%08X] ", __ra);
	PrintFuncName(__ra);
	printf("PrintStackTrace addr [0x%08X]\n", PrintStackTrace);
	printf("\n");

#if 0
	printf("===============Stack data1===========\n");
	__fpCopy = __sp;
	int i;
	for (i = 0; i < 100; i++)
	{
		/* print function name */
		printf("<%03d> sp=[0x%08X] *sp=[0x%08X]  ", i, __fpCopy, *__fpCopy);
		PrintFuncName((unsigned int *)*__fpCopy);
		__fpCopy++;
	}
#endif

#if 1
	int cnt = 0;
	while ((unsigned int)__sp <= 0x7FFFFFFF)
	{
		while ((unsigned int)__sp != (*__sp - 8))
		{
			++__sp;
		}
		__ra = (unsigned int *)*(++__sp);
		++__sp;
		printf("<%03d> ra=[0x%08X] \n", i, __ra);
	}
#endif
}


void Myfunc_01(int cnt)
{
	if (cnt > 0)
		Myfunc_01(--cnt);
	else
		FuncSegfault();
}

void Myfunc_02(int cnt)
{
	if (cnt > 0)
		Myfunc_02(--cnt);
	else
		PrintStackTrace();
}

void Myfunc_03(void)
{
	/* use asm get ra, fp, sp registers */
	unsigned int *__ra = NULL;
	unsigned int *__sp = NULL;
	unsigned int *__fp = NULL;

	__asm__ volatile("move %0, $29" : "=r"(__sp));	/* sp */
	__asm__ volatile("move %0, $30" : "=r"(__fp));	/* fp */
	__asm__ volatile("move %0, $31" : "=r"(__ra));	/* ra */

	printf("Myfunc_03 sp=[0x%08X] *sp=[0x%08X]\n", __sp, *__sp);
	printf("Myfunc_03 fp=[0x%08X] *fp=[0x%08X]\n", __fp, *__fp);
	printf("Myfunc_03 ra=[0x%08X] ", __ra);
	PrintFuncName(__ra);
	printf("Myfunc_03 addr [0x%08X]\n", Myfunc_03);
	printf("\n");

	Myfunc_04();
}

void Myfunc_04(void)
{
	/* use asm get ra, fp, sp registers */
	unsigned int *__ra = NULL;
	unsigned int *__sp = NULL;
	unsigned int *__fp = NULL;

	__asm__ volatile("move %0, $29" : "=r"(__sp));	/* sp */
	__asm__ volatile("move %0, $30" : "=r"(__fp));	/* fp */
	__asm__ volatile("move %0, $31" : "=r"(__ra));	/* ra */

	printf("Myfunc_04 sp=[0x%08X] *sp=[0x%08X]\n", __sp, *__sp);
	printf("Myfunc_04 fp=[0x%08X] *fp=[0x%08X]\n", __fp, *__fp);
	printf("Myfunc_04 ra=[0x%08X] ", __ra);
	PrintFuncName(__ra);
	printf("Myfunc_04 addr [0x%08X]\n", Myfunc_04);
	printf("\n");

	PrintStackTrace();
}

void FuncSegfault(void)
{
	int *p = NULL;
	*p = 100;

	p = (int *)12345;
}

#endif /* MIPSGETPC_CPP */
/* end file */
