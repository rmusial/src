/*	$OpenBSD: netbsd_exec.c,v 1.1 1999/09/12 14:15:16 kstailey Exp $	 */
/*	$NetBSD: svr4_exec.c,v 1.16 1995/10/14 20:24:20 christos Exp $	 */

/*
 * Copyright (c) 1994 Christos Zoulas
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/proc.h>
#include <sys/signalvar.h>
#include <sys/malloc.h>
#include <sys/exec.h>
#include <sys/exec_elf.h>
#include <sys/exec_olf.h>

#include <sys/mman.h>
#include <vm/vm.h>
#include <vm/vm_param.h>
#include <vm/vm_map.h>

#include <machine/cpu.h>
#include <machine/reg.h>
#if 0
#include <machine/netbsd_machdep.h>
#endif

#include <compat/netbsd/netbsd_util.h>
#include <compat/netbsd/netbsd_syscall.h>
#include <compat/netbsd/netbsd_exec.h>

static void *netbsd_copyargs __P((struct exec_package *, struct ps_strings *,
			       void *, void *));

const char netbsd_emul_path[] = "/emul/netbsd";
extern char sigcode[], esigcode[];
extern struct sysent netbsd_sysent[];
extern char *netbsd_syscallnames[];

struct emul emul_elf64_netbsd = {
	"netbsd",
	NULL,
	sendsig,
	NETBSD_SYS_syscall,
	NETBSD_SYS_MAXSYSCALL,
	netbsd_sysent,
	netbsd_syscallnames,
	0,
	netbsd_copyargs,
	setregs,
	exec_elf64_fixup,
	sigcode,
	esigcode,
};

static void *
netbsd_copyargs(pack, arginfo, stack, argp)
	struct exec_package *pack;
	struct ps_strings *arginfo;
	void *stack;
	void *argp;
{
	AuxInfo *a;

	if (!(a = (AuxInfo *)elf64_copyargs(pack, arginfo, stack, argp)))
		return (NULL);
	return (a);
}

int
netbsd_elf64_probe(p, epp, itp, pos, os)
	struct proc *p;
	struct exec_package *epp;
	char *itp;
	u_long *pos;
	u_int8_t *os;
{
	char *bp;
	int error;
	size_t len;

	if (itp[0]) {
		if ((error = emul_find(p, NULL, netbsd_emul_path, itp, &bp, 0)))
			return (error);
		if ((error = copystr(bp, itp, MAXPATHLEN, &len)))
			return (error);
		free(bp, M_TEMP);
	}
	epp->ep_emul = &emul_elf64_netbsd;
	*pos = ELF64_NO_ADDR;
	if (*os == OOS_NULL)
		*os = OOS_NETBSD;
	return (0);
}
