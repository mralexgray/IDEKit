/*
 * Copyright (c) 1998, 1999 Henry Spencer.  All rights reserved.
 * 
 * Development of this software was funded, in part, by Cray Research Inc.,
 * UUNET Communications Services Inc., Sun Microsystems Inc., and Scriptics
 * Corporation, none of whom are responsible for the results.  The author
 * thanks all of them. 
 * 
 * Redistribution and use in source and binary forms -- with or without
 * modification -- are permitted for any purpose, provided that
 * redistributions in source form retain this entire copyright notice and
 * indicate the origin and nature of any modifications.
 * 
 * I'd appreciate being given credit for this package in the documentation
 * of software which uses it, but that is not a requirement.
 * 
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL
 * HENRY SPENCER BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/* headers if any */
//#include "tclInt.h"
#include "FakeTCLBridge.h"

/* overrides for regguts.h definitions, if any */
#define	FUNCPTR(name, args)	(*name) _ANSI_ARGS_(args)
#ifdef nodef
#define	MALLOC(n)		ckalloc(n)
#define	FREE(p)			ckfree(VS(p))
#define	REALLOC(p,n)		ckrealloc(VS(p),n)
#endif


/*
 * Do not insert extras between the "begin" and "end" lines -- this
 * chunk is automatically extracted to be fitted into regex.h.
 */
/* --- begin --- */
/* ensure certain things don't sneak in from system headers */
#ifdef __REG_WIDE_T
#undef __REG_WIDE_T
#endif
#ifdef __REG_WIDE_COMPILE
#undef __REG_WIDE_COMPILE
#endif
#ifdef __REG_WIDE_EXEC
#undef __REG_WIDE_EXEC
#endif
#ifdef __REG_REGOFF_T
#undef __REG_REGOFF_T
#endif
#ifdef __REG_VOID_T
#undef __REG_VOID_T
#endif
#ifdef __REG_CONST
#undef __REG_CONST
#endif
#ifdef __REG_NOFRONT
#undef __REG_NOFRONT
#endif
#ifdef __REG_NOCHAR
#undef __REG_NOCHAR
#endif
/* interface types */
//#define	__REG_WIDE_T	Tcl_UniChar
#define __REG_WIDE_T    unichar
#define	__REG_REGOFF_T	long	/* not really right, but good enough... */
#define	__REG_VOID_T	void
#define	__REG_CONST	const
/* names and declarations */
#define	__REG_WIDE_COMPILE	re_unicomp  //TclReComp
#define	__REG_WIDE_EXEC		re_uniexec  //TclReExec
#define	__REG_NOFRONT		/* don't want regcomp() and regexec() */
#define	__REG_NOCHAR		/* or the char versions */
#define	regfree		re_free //TclReFree
#define	regerror	re_err  //TclReError
/* --- end --- */



/* internal character type and related */
typedef Tcl_UniChar chr;	/* the type itself */
typedef int pchr;		/* what it promotes to */
typedef NSUInteger uchr;		/* NSUInteger type that will hold a chr */
typedef int celt;		/* type to hold chr, MCCE number, or NOCELT */
#define	NOCELT	(-1)		/* celt value which is not valid chr or MCCE */
#define	CHR(c)	(UCHAR(c))	/* turn char literal into chr literal */
#define	DIGITVAL(c)	((c)-'0')	/* turn chr digit into its value */
#if TCL_UTF_MAX > 3
#define	CHRBITS	32		/* bits in a chr; must not use sizeof */
#define	CHR_MIN	0x00000000	/* smallest and largest chr; the value */
#define	CHR_MAX	0xffffffff	/*  CHR_MAX-CHR_MIN+1 should fit in uchr */
#else
#define	CHRBITS	16		/* bits in a chr; must not use sizeof */
#define	CHR_MIN	0x0000		/* smallest and largest chr; the value */
#define	CHR_MAX	0xffff		/*  CHR_MAX-CHR_MIN+1 should fit in uchr */
#endif

/* functions operating on chr */
#define	iscalnum(x)	Tcl_UniCharIsAlnum(x)
#define	iscalpha(x)	Tcl_UniCharIsAlpha(x)
#define	iscdigit(x)	Tcl_UniCharIsDigit(x)
#define	iscspace(x)	Tcl_UniCharIsSpace(x)

/* name the external functions */
#define	compile		TclReComp
#define	exec		TclReExec

/* enable/disable debugging code (by whether REG_DEBUG is defined or not) */
#if 0		/* no debug unless requested by makefile */
#define	REG_DEBUG	/* */
#endif

/* and pick up the standard header */
#include "regex.h"
