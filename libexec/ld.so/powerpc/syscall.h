/*	$OpenBSD: syscall.h,v 1.1.1.1 2000/06/13 03:40:39 rahnds Exp $ */

/*
 * Copyright (c) 1998 Per Fogelstrom, Opsycon AB
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed under OpenBSD by
 *	Per Fogelstrom, Opsycon AB, Sweden.
 * 4. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

#ifdef USE_CACHE
#include <sys/stat.h>
#endif

#include <sys/syscall.h>

#ifndef _dl_MAX_ERRNO
#define _dl_MAX_ERRNO 4096
#endif
#define _dl_check_error(__res)	\
  ((int) __res < 0 && (int) __res >= -_dl_MAX_ERRNO)

/*
 *  Inlined system call functions that can be used before
 *  any dynamic address resoving has been done.
 */

void _dl_printf(const char *, ...);

static inline int
_dl_exit (int status)
{ 
  register int __status __asm__ ("3");
  __asm__ volatile ("mr  0,%1\n\t"
		    "mr  3,%2\n\t"
                    "sc"
                    : "=r" (__status)
                    : "r" (SYS_exit), "r" (status) : "0", "3");
  while(1);
} 

static inline int
_dl_open (const char* addr, unsigned int flags)
{ 
  register int status __asm__ ("3");
  __asm__ volatile ("mr    0,%1\n\t"
                    "mr    3,%2\n\t"
                    "mr    4,%3\n\t"
                    "sc\n\t"
		    "cmpwi   0, 0\n\t"
		    "beq   1f\n\t"
		    "li    3,-1\n\t"
		    "1:"
                    : "=r" (status)
                    : "r" (SYS_open), "r" (addr), "r" (flags)
                    : "0", "3", "4" );
  return status;
} 

static inline int
_dl_close (int fd)
{ 
  register int status __asm__ ("3");
  __asm__ volatile ("mr    0,%1\n\t"
                    "mr    3,%2\n\t"
                    "sc\n\t"
		    "cmpwi   0, 0\n\t"
		    "beq   1f\n\t"
		    "li    3,-1\n\t"
		    "1:"
                    : "=r" (status)
                    : "r" (SYS_close), "r" (fd)
                    : "0", "3");
  return status;
} 

static inline int
_dl_write (int fd, const char* buf, int len)
{ 
  register int status __asm__ ("3");
  __asm__ volatile ("mr    0,%1\n\t"
                    "mr    3,%2\n\t"
                    "mr    4,%3\n\t"
                    "mr    5,%4\n\t"
                    "sc\n\t"
		    "cmpwi   0, 0\n\t"
		    "beq   1f\n\t"
		    "li    3,-1\n\t"
		    "1:"
                    : "=r" (status)
                    : "r" (SYS_write), "r" (fd), "r" (buf), "r" (len)
                    : "0", "3", "4", "5" );
  return status;
} 

static inline int
_dl_read (int fd, const char* buf, int len)
{ 
  register int status __asm__ ("3");
  __asm__ volatile ("mr    0,%1\n\t"
                    "mr    3,%2\n\t"
                    "mr    4,%3\n\t"
                    "mr    5,%4\n\t"
                    "sc\n\t"
		    "cmpwi   0, 0\n\t"
		    "beq   1f\n\t"
		    "li    3,-1\n\t"
		    "1:"
                    : "=r" (status)
                    : "r" (SYS_read), "r" (fd), "r" (buf), "r" (len)
                    : "0", "3", "4", "5");
  return status;
} 
#if 0
static inline int
_dl__syscall(quad_t val, ...)
{
  register int status __asm__ ("3");
  __asm__ volatile ("mr    0,%1\n\t"
		    "mr	   3,%2\n\t"
                    "sc\n\t"
		    "cmpwi   0, 0\n\t"
		    "beq   1f\n\t"
		    "li    3,-1\n\t"
		    "1:"
                    : "=r" (status)
                    : "r" (SYS___syscall), "r" (val)
                    : "0");
	
}
#else

#define STRINGIFY(x)  #x
#define XSTRINGIFY(x) STRINGIFY(x)
int _dl__syscall(quad_t val, ...);
__asm__(".align 2\n\t"
	".type _dl__syscall,@function\n"
	"_dl__syscall:\n\t"
	"li 0, " XSTRINGIFY(SYS___syscall) "\n\t"
	"sc\n\t"
	"cmpwi	0, 0\n\t"
	"beq	1f\n\t"
	"li	3, -1\n\t"
	"1:\n\t"
	"blr");
#endif
static int
_dl_mmap (void *addr, unsigned int len, unsigned int prot,
          unsigned int flags, int fd, off_t offset)
{ 
	return((int)_dl__syscall((quad_t)SYS_mmap, addr, len, prot,
		flags, fd, 0, offset));
}

static inline int
_dl_munmap (const void* addr, unsigned int len)
{ 
  register int status __asm__ ("3");
  __asm__ volatile ("mr    0,%1\n\t"
                    "mr    3,%2\n\t"
                    "mr    4,%3\n\t"
                    "sc\n\t"
		    "cmpwi   0, 0\n\t"
		    "beq   1f\n\t"
		    "li    3,-1\n\t"
		    "1:"
                    : "=r" (status)
                    : "r" (SYS_read), "r" (addr), "r" (len)
                    : "0", "3", "4");
  return status;
} 

static inline int
_dl_mprotect (const void *addr, int size, int prot)
{ 
  register int status __asm__ ("3");
  __asm__ volatile ("mr    0,%1\n\t"
                    "mr    3,%2\n\t"
                    "mr    4,%3\n\t"
                    "mr    5,%4\n\t"
                    "sc\n\t"
		    "cmpwi   0, 0\n\t"
		    "beq   1f\n\t"
		    "li    3,-1\n\t"
		    "1:"
                    : "=r" (status)
                    : "r" (SYS_read), "r" (addr), "r" (size), "r" (prot)
                    : "0", "3", "4", "5");
  return status;
} 
#if 0

#ifdef USE_CACHE
static inline int
_dl_stat (const char *addr, struct stat *sb)
{ 
  register int status __asm__ ("3");
  __asm__ volatile ("mr    0,%1\n\t"
                    "mr    3,%2\n\t"
                    "mr    4,%3\n\t"
                    "sc\n\t"
		    "cmpwi   0, 0\n\t"
		    "beq   1f\n\t"
		    "li    3,-1\n\t"
		    "1:"
                    : "=r" (status)
                    : "r" (SYS_read), "r" (addr), "r" (sb)
                    : "0", "3", "4");
  return status;
} 

#endif

/* Not an actual syscall, but we need something in assembly to say
   whether this is OK or not.  */

static inline int
_dl_getuid (const void *addr, int size, int prot)
{ 
  register int status __asm__ ("3");
  __asm__ volatile ("mr    0,%1\n\t"
                    "sc\n\t"
		    "cmpwi   0, 0\n\t"
		    "beq   1f\n\t"
		    "li    3,-1\n\t"
		    "1:"
                    : "=r" (status)
                    : "r" (SYS_getuid)
                    : "0", "3");
  return status;
} 
static inline int
_dl_geteuid (const void *addr, int size, int prot)
{ 
  register int status __asm__ ("3");
  __asm__ volatile ("mr    0,%1\n\t"
                    "sc\n\t"
		    "cmpwi   0, 0\n\t"
		    "beq   1f\n\t"
		    "li    3,-1\n\t"
		    "1:"
                    : "=r" (status)
                    : "r" (SYS_geteuid)
                    : "0", "3");
  return status;
} 
static inline int
_dl_getgid (const void *addr, int size, int prot)
{ 
  register int status __asm__ ("3");
  __asm__ volatile ("mr    0,%1\n\t"
                    "sc\n\t"
		    "cmpwi   0, 0\n\t"
		    "beq   1f\n\t"
		    "li    3,-1\n\t"
		    "1:"
                    : "=r" (status)
                    : "r" (SYS_getgid)
                    : "0", "3");
  return status;
} 
static inline int
_dl_getuid (const void *addr, int size, int prot)
{ 
  register int status __asm__ ("3");
  __asm__ volatile ("mr    0,%1\n\t"
                    "sc\n\t"
		    "cmpwi   0, 0\n\t"
		    "beq   1f\n\t"
		    "li    3,-1\n\t"
		    "1:"
                    : "=r" (status)
                    : "r" (SYS_getegid),
                    : "0", "3");
  return status;
} 
static inline int
_dl_getuid (const void *addr, int size, int prot)
{ 
  register int status __asm__ ("3");
  __asm__ volatile ("mr    0,%1\n\t"
                    "sc\n\t"
		    "cmpwi   0, 0\n\t"
		    "beq   1f\n\t"
		    "li    3,-1\n\t"
		    "1:"
                    : "=r" (status)
                    : "r" (SYS_read), 
                    : "0", "3");
  return status;
} 
static inline int
_dl_suid_ok (void)
{
  unsigned int uid, euid, gid, egid;

	uid = _dl_getuid();
	euid = _dl_geteuid();
	gid = _dl_getgid();
	egid = _dl_getegid();
  	return (uid == euid && gid == egid);
}
#endif
static inline int
_dl_suid_ok(void)
{
	return 1;
}

#include <elf_abi.h>
