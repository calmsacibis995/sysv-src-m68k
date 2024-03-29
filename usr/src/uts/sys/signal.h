/* @(#)signal.h	2.4	 */
#define	SIGHUP	1	/* hangup */
#define	SIGINT	2	/* interrupt (rubout) */
#define	SIGQUIT	3	/* quit (ASCII FS) */
#define	SIGILL	4	/* illegal instruction (not reset when caught)*/
#define	SIGTRAP	5	/* trace trap (not reset when caught) */
#define	SIGIOT	6	/* IOT instruction */
#define	SIGEMT	7	/* EMT instruction */
#define	SIGFPE	8	/* floating point exception */
#define	SIGKILL	9	/* kill (cannot be caught or ignored) */
#define	SIGBUS	10	/* bus error */
#define	SIGSEGV	11	/* segmentation violation */
#define	SIGSYS	12	/* bad argument to system call */
#define	SIGPIPE	13	/* write on a pipe with no one to read it */
#define	SIGALRM	14	/* alarm clock */
#define	SIGTERM	15	/* software termination signal from kill */
#define	SIGUSR1	16	/* user defined signal 1 */
#define	SIGUSR2	17	/* user defined signal 2 */
#define	SIGCLD	18	/* death of a child */
#define	SIGPWR	19	/* power-fail restart */

#define	NSIG	20

#define	SIG_DFL	(int (*)())0
#if lint
#define	SIG_IGN	(int (*)())0
#else
#define	SIG_IGN	(int (*)())1
#endif

/* SIGFPE codes */

#define	KINTOVF	1	/* integer overflow */
#define	KINTDIV	2	/* integer divide by zero */
#define	KFLTOVF	3	/* floating overflow */
#define	KFLTDIV	4	/* floating divide by zero */
#define	KDECDIV	KFLTDIV	/* decimal divide by zero */
#define	KFLTUND	5	/* floating underflow */
#define	KDECOVF	6	/* decimal overflow */
#define	KSUBRNG	7	/* subscript range */
