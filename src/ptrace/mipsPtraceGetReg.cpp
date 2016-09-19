/*
 * mipsPtraceGetReg.cpp
 *
 *  Created on: 2014-12-30
 *      Author: hong
 */
#include "../config.h"
#if MIPSPTRACEGETREG_CPP

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ptrace.h>

#include "include/mipsReg.h"

static void supply_gregset(struct regcache *regcache, const elf_gregset_t * gregsetp);
static void fetch_regs(struct regcache *regcache, int tid);

int MipsPtraceGetReg_GetReg(int argc, char *argv[]);

/*==================main=================*/
int MipsPtraceGetReg(int argc, char *argv[])
{
	int ret;

	ret = MipsPtraceGetReg_GetReg(argc, argv);

	return ret;
}

int MipsPtraceGetReg_GetReg(int argc, char *argv[])
{
	struct pt_regs regs;

	if (-1 == ptrace(PTRACE_GETREGS, tid, 0, (int)&regs)0)
		perror("Couldn't get registers");
	return 0;
}

#if 0
/* Transfering the general-purpose registers between GDB, inferiors
 and core files.  */

/* Fill GDB's register array with the general-purpose register values
 in *GREGSETP.  */

void supply_gregset(struct regcache *regcache, const elf_gregset_t * gregsetp)
{
	const struct pt_regs *regp = (const elf_greg_t *) gregsetp;
	int i;
	unsigned long psw, bbpsw;

	psw = *(regp + PSW_REGMAP);
	bbpsw = *(regp + BBPSW_REGMAP);

	for (i = 0; i < M32R_LINUX_NUM_REGS; i++)
	{
		elf_greg_t regval;

		switch (i)
		{
			case PSW_REGNUM:
			regval = ((0x00c1 & bbpsw) << 8) | ((0xc100 & psw) >> 8);
			break;
			case CBR_REGNUM:
			regval = ((psw >> 8) & 1);
			break;
			default:
			regval = *(regp + regmap[i]);
			break;
		}

		if (i != M32R_SP_REGNUM)
		regcache_raw_supply(regcache, i, &regval);
		else if (psw & 0x8000)
		regcache_raw_supply(regcache, i, regp + SPU_REGMAP);
		else
		regcache_raw_supply(regcache, i, regp + SPI_REGMAP);
	}
}
#endif


#endif /* MIPSPTRACEGETREG_CPP */
/* end file */
