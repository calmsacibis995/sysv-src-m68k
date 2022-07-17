/* @(#)sigcall.c	2.1	 */

/* signal()	- The interface to the system call _signal().
**
** _sigcall()	- Calls the user's system call handler.
**
** The system call as described in the manual is _signal().
**
** signal() maintains its own array (sigtab[]) of signal handlers,
** and passes the address of _sigcode() to the kernel.
** When a signal is sent to this process, the kernel forces the execution
** of _sigcode(), which in turn calls _sigcall().
** _sigcall() looks up the address of the user's handler in sigtab[],
** and calls it.
*/ 

#include "sys/signal.h"
#include "sys/errno.h"

typedef int (*fn)();

static fn sigtab [ NSIG ];
int errno;

fn signal( sig, func )
register int sig;
register fn func;
{
	register fn *p, old, tmp;
	extern int _signal();
	extern int _sigcode();

	if( sig <= 0 || sig > NSIG )
	{	errno = EINVAL;
		return( (fn) -1 );
	}
	tmp = _sigcode;
	if( func == SIG_DFL || ( (int) func )&1 )
		tmp = func;
	if( ( tmp = (fn) _signal( sig, tmp ) ) == (fn) -1 )
		return( (fn) -1 );
	p = &sigtab[ sig ] - 1;
	old = *p;
	*p = func;
	return( tmp == _sigcode ? old : tmp );
}

void _sigcall( sig, arg )
register int sig;
{
	register fn func;

	if( sig > 0 && sig <= NSIG )
	{	func = sigtab[ sig - 1 ];
		if( func != SIG_DFL && ( ( (int) func )&1 ) == 0 )
			(*func)( sig, arg );
	}
}
