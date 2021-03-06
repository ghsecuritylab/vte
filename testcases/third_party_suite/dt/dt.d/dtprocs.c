static char *whatHeader = "WHATSTRING";
/****************************************************************************
 *									    *
 *			  COPYRIGHT (c) 1990 - 2004			    *
 *			   This Software Provided			    *
 *				     By					    *
 *			  Robin's Nest Software Inc.			    *
 *									    *
 * Permission to use, copy, modify, distribute and sell this software and   *
 * its documentation for any purpose and without fee is hereby granted,	    *
 * provided that the above copyright notice appear in all copies and that   *
 * both that copyright notice and this permission notice appear in the	    *
 * supporting documentation, and that the name of the author not be used    *
 * in advertising or publicity pertaining to distribution of the software   *
 * without specific, written prior permission.				    *
 *									    *
 * THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, 	    *
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN	    *
 * NO EVENT SHALL HE BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL   *
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR    *
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS  *
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF   *
 * THIS SOFTWARE.							    *
 *									    *
 ****************************************************************************/
/*
 * Module:	dtprocs.c
 * Author:	Robin T. Miller
 * Date:	August 7, 1993
 *
 * Description:
 *	Functions to handle multiple processes for 'dt' program.
 *
 * Modification History:
 *
 * January 5th, 2007 by Robin T. Miller.
 *	When displaying slice information w/debug enabled, format lba's
 * and block length values as 64-bits (32-bits is limited to 2TB!).
 *
 * August 3rd, 2005 by Robin T. Miller.
 *      Fixed bug in init_slice_info() where the position was not taken
 * into account when calculating the slice resid. We would end up writing
 * too much, and in the case of Hazard, we were destroying the GDID!.
 *
 * April 12th, 2005 by Robin T. Miller
 *	Fixed debug output in setup_slice(), when there was a mismatch
 * between 32 and 64 bit values and format control characters.  Also
 * removed WINDOWS conditionalization, which shouldn't be needed, now
 * that the root cause has been identified and fixed.
 *
 * November 17th, 2003 by Robin Miller.
 *	Breakup output to stdout or stderr, rather than writing
 * all output to stderr.  If output file is stdout ('-') or a log
 * file is specified, then all output reverts to stderr.
 *
 * March 14th, 2003 by Robin Miller.
 *	Added support for testing an individual slice.
 *
 * May 31st, 2001 by Robin Miller.
 *	Update abort_procs() to use max_procs, valid for both multiple
 * processes and slices, and loop through entire process table since
 * some processes may have finished already (use to break out early).
 *	When starting slices with multiple processes, prime the data
 * pattern to ensure each process uses the same pattern in a slice.
 *
 * February 6th, 2001 by Robin Miller.
 *	If doing multiple slices in reverse direction, ensure the data
 * limit gets tot he slice length, or we'll overflow into previous slice.
 *
 * February 1st, 2001 by Robin Miller.
 *	Fix dumb problem starting multiple procs with multiple slices.
 *
 * January 28th, 2001 by Robin Miller.
 *	When aborting processes, send SIGINT instead of SIGTERM, so
 * statistics gets reported (important on long running commands :-).
 *	Added support for multiple slices option.  This sets up each
 * process exercising a different range of blocks (slice) on the disk.
 *
 * January 2nd, 2001 by Robin Miller.
 *      Make changes to build using MKS/NuTCracker product.
 *
 * March 28th, 2000 by Robin Miller.
 *	When creating multiple processes, do a better job determining
 * when a unique device name should be constructed.  Was broken for
 * named piped (FIFO's).
 *
 * May 3, 1999 by Robin Miller.
 *	Allocate more space for unique file names, since the size
 * of pid_t is now 31 bits in Steel.
 *
 * April 8, 1999 by Robin Miller.
 *	Merge in Jeff Detjen's changes for current process count.
 *
 * April 29, 1998 by Robin Miller.
 *	Add support for an alternate device directory.
 *
 * July 17, 1995 by Robin Miller.
 *	Apply a severity priority to child exit status.
 *	Added flag to ensure process abortion occurs only once.
 *
 * July 5, 1995 by Robin Miller.
 *	Properly check for child process exiting as result of a signal.
 *
 * March 28, 1995 by Robin Miller.
 *	Report specific error for "no processes started", and exit with
 *	error status if the system process limit has been exceeded.
 *
 * November 4, 1994 by Robin Miller.
 *	Don't set SIGCHLD signal to SIG_IGN (set to SIG_DFL) or else
 *	waitpid() won't detect any child processes (OSF R1.3 and QNX).
 *	[ Unfortunately, the POSIX standard states "The specification
 *	  of the effects of SIG_IGN on SIGCHLD as implementation defined
 *	  and permits, but does NOT require, the System V effect of
 *	  causing terminating children to be ignored by wait(). Yuck!!! ]
 */
#include "dt.h"
#if !defined(WIN32)
#  include <signal.h>
#  include <sys/stat.h>
#  include <sys/wait.h>
#endif /* !defined(WIN32) */

#define PROC_ALLOC (sizeof(pid_t) * 3)	/* Extra allocation for PID.	*/

/*
 * Structure to track multiple processes.
 */
struct dt_procs {
	pid_t	dt_pid;			/* The child process ID.	*/
	int	dt_status;		/* The child exit status.	*/
	bool	dt_active;		/* The process active flag.	*/
};

/*
 * Slice Range Definition:
 */
typedef struct slice_info {
	int	slice;			/* Slice number.		*/
	large_t	slice_position;		/* Starting slice position.	*/
	large_t	slice_length;		/* The slice data length.	*/
} slice_info_t;

/*
 * Forward References:
 */
static void init_slice_info(struct dinfo *dip, slice_info_t *sip, large_t *data_resid);
static void setup_slice(struct dinfo *dip, slice_info_t *sip);

struct dt_procs *ptable;		/* Multiple 'dt' procs table.	*/
int num_procs = 0;			/* Number of procs to create.	*/
int cur_proc = 0;			/* Current count of processes.	*/
int max_procs = 0;			/* Maximum processes started.	*/
int procs_active = 0;			/* Number of active processes.	*/
int num_slices = 0;			/* Number of slices to create.	*/
int slice_num = 0;			/* Slice number to operate on.	*/
#if !defined(WIN32)

/*
 * abort_procs - Abort processes started by the parent.
 */
void
abort_procs(void)
{
    static int aborted_processes = FALSE;
    struct dt_procs *dtp;
    int procs;
    pid_t pid;

    if ((ptable == NULL) || aborted_processes)  return;
    /*
     * Force all processes to terminate.
     */
    for (dtp = ptable, procs=0; procs < max_procs; procs++, dtp++) {
	if ((pid = dtp->dt_pid) == (pid_t) 0) continue;
	if (debug_flag) {
	    Printf("Aborting child process %d via a SIGINT (%d)...\n",
								pid, SIGINT);
	}
	if (dtp->dt_active) (void) kill (pid, SIGINT);
    }
    aborted_processes = TRUE;
}

void
await_procs(void)
{
    pid_t wpid;
    struct dt_procs *dtp;
    int procs, status;

    if (debug_flag) {
	Printf ("Waiting for %d child processes to complete...\n", procs_active);
    }
    while (1) {
	if ((wpid = waitpid ((pid_t) -1, &child_status, 0)) == FAILURE) {
	    if (errno == ECHILD) {
		if (procs_active) abort();	/* Programming error... */
		break;				/* No more children... */
	    } else if (errno == EINTR) {
		abort_procs();
		continue;
	    } else {
		report_error ("waitpid", FALSE);
		exit (FATAL_ERROR);
	    }
	}
	/*
	 * Examine the child process status.
	 */
	if ( WIFSTOPPED(child_status) ) {
	    Printf ("Child process %d, stopped by signal %d.\n",
					wpid, WSTOPSIG(child_status));
	    continue; /* Maybe attached from debugger... */
	} else if ( WIFSIGNALED(child_status) ) {
	    status = WTERMSIG(child_status);
	    Fprintf ("Child process %d, exiting because of signal %d\n",
							wpid, status);
	} else { /* Process must be exiting... */
	    status = WEXITSTATUS (child_status);
	    if (debug_flag) {
		Printf ("Child process %d, exiting with status %d\n",
							wpid, status);
	    }
	}
	if ( (exit_status == SUCCESS) && (status != SUCCESS) ) {
	    if ( (oncerr_action == ABORT) &&
		 (status != WARNING) && (status != END_OF_FILE) ) {
		abort_procs();		/* Abort procs on error. */
	    }
	    /*
	     * Save the most sever error for parent exit status.
	     *
	     * Severity Priorities:	WARNING		(lowest)
	     *				END_OF_FILE
	     *				Signal Number
	     *				FATAL_ERROR	(highest)
	     */
	    if ( ((exit_status == SUCCESS) || (status == FATAL_ERROR)) ||
		 ((exit_status == WARNING) && (status > WARNING))      ||
		 ((exit_status == END_OF_FILE) && (status > WARNING)) ) {
		exit_status = status;	/* Set error code for exit. */
	    }
	}
	/*
	 * House keeping... (mostly sanity check, not really necessary).
	 */
	for (dtp = ptable, procs = 0; procs < max_procs; procs++, dtp++) {
	    if (dtp->dt_pid == wpid) {
		dtp->dt_active = FALSE;
		dtp->dt_status = status;
		procs_active--;
	    }
	}
    } /* End of while(1)... */
}

pid_t
fork_process(void)
{
    pid_t pid;

    if ((pid = fork()) == (pid_t) -1) {
	if (errno == EAGAIN) {
	    if (procs_active == 0) {
		LogMsg (efp, logLevelCrit, 0,
	"ERROR: could NOT start any processes, please check your system...\n");
		exit (FATAL_ERROR);
	    } else {
		Printf (
	"WARNING: system imposed process limit reached, only %d procs started...\n",
								procs_active);
	    }
	} else {
	    report_error ("fork", FALSE);
	    abort_procs();
	}
    }
    return (pid);
}

pid_t
start_procs(void)
{
    struct dt_procs *dtp;
    size_t psize;
    int procs;

    max_procs = num_procs;
    psize = (max_procs * sizeof(*dtp));

    if ((ptable = (struct dt_procs *)malloc(psize)) == NULL) {
	report_error ("No memory for proc table", FALSE);
	exit (FATAL_ERROR);
    }
    bzero((char *)ptable, psize);
#if !defined(__MSDOS__) || defined(__NUTC__)
    (void) signal (SIGCHLD, SIG_DFL);
#endif
    (void) signal (SIGHUP, terminate);
    (void) signal (SIGINT, terminate);
    (void) signal (SIGTERM, terminate);

    cur_proc = 1;
    procs_active = 0;

    for (dtp = ptable, procs = 0; procs < max_procs; procs++, dtp++) {
	if ((child_pid = fork_process()) == (pid_t) -1) {
	    break;
	} else if (child_pid) {		/* Parent process gets the PID. */
	    cur_proc++;
	    dtp->dt_pid = child_pid;
	    dtp->dt_active = TRUE;
	    procs_active++;
	    if (debug_flag) {
		Printf ("Started Process %d...\n", child_pid);
	    }
	} else {			/* Child process... */
	    struct stat sb;
	    bool make_unique = FALSE;
	    int error;

	    if (!output_file) break;
	    error = stat (output_file, &sb);
	    if (!error) {
		if ( S_ISREG(sb.st_mode) ) {
		    make_unique = TRUE;
		}
	    /* Leave all other types alone! */
	    } else if ( (NEL (output_file, DEV_PREFIX, DEV_LEN)) &&
			(NEL (output_file, ADEV_PREFIX, ADEV_LEN)) ) {
		make_unique = TRUE;	/* Ok, not a device directory. */
	    }
	    /*
	     * Construct unique file name for file system I/O.
	     */
	    if (make_unique) {
		char *bp;
		bp = (char *)malloc(strlen(output_file) + PROC_ALLOC);
		(void)sprintf(bp, "%s-%d", output_file, getpid());
		output_dinfo->di_dname = output_file = bp;
	    }
	    break;			/* Child process, continue... */
	}
    }
    return (child_pid);
}

pid_t
start_slices(void)
{
    struct dinfo *dip = active_dinfo;
    struct dt_procs *dtp;
    size_t psize;
    struct slice_info slice_info;
    slice_info_t *sip = &slice_info;
    large_t data_resid;
    int procs;

    max_procs = num_slices;
    psize = (max_procs * sizeof(*dtp));

    if ((ptable = (struct dt_procs *)malloc(psize)) == NULL) {
	report_error ("No memory for proc table", FALSE);
	exit (FATAL_ERROR);
    }
    bzero((char *)ptable, psize);
#if !defined(__MSDOS__) || defined(__NUTC__)
    (void) signal (SIGCHLD, SIG_DFL);
#endif
    (void) signal (SIGHUP, terminate);
    (void) signal (SIGINT, terminate);
    (void) signal (SIGTERM, terminate);

    init_slice_info(dip, sip, &data_resid);

    cur_proc = 1;
    procs_active = 0;

    for (dtp = ptable, procs = 0; procs < max_procs; procs++, dtp++) {
	sip->slice++;
	if ((child_pid = fork_process()) == (pid_t) -1) {
	    break;
	} else if (child_pid) {		/* Parent process gets the PID. */
	    cur_proc++;
	    dtp->dt_pid = child_pid;
	    dtp->dt_active = TRUE;
	    procs_active++;
	    if (debug_flag) {
		Printf ("Started Slice %d, PID %d...\n", sip->slice, child_pid);
	    }
	    if (procs < max_procs) {
		sip->slice_position += sip->slice_length;
		if (procs == max_procs) {
		    sip->slice_length += data_resid;
		}
	    }
	} else {			/* Child process... */
	    /*
	     * Initialize the starting data pattern for each slice.
	     */
	    if (unique_pattern) {
		pattern = data_patterns[(cur_proc - 1) % npatterns];
	    }
	    setup_slice(dip, sip);
	    break;			/* Child process, continue... */
	}
    }
    return (child_pid);
}
#endif /* !defined(WIN32) */

void
init_slice(struct dinfo *dip, int slice)
{
    struct slice_info slice_info;
    slice_info_t *sip = &slice_info;
    large_t data_resid;

    init_slice_info(dip, sip, &data_resid);
    sip->slice_position += (sip->slice_length * (slice - 1));
    /*
     * Any residual goes to the last slice.
     */
    if (slice == num_slices) {
	sip->slice_length += data_resid;
    }
    sip->slice = slice;
    setup_slice(dip, sip);

    /*
     * Initialize the starting data pattern for each slice.
     */
    if (unique_pattern) {
	pattern = data_patterns[(slice - 1) % npatterns];
    }
    return;
}

static void
init_slice_info(struct dinfo *dip, slice_info_t *sip, large_t *data_resid)
{
    large_t dlimit = (dip->di_data_limit - file_position);
    large_t slice_length;

    sip->slice = 0;
    sip->slice_position = file_position;
    slice_length = (dlimit / num_slices);
    sip->slice_length = rounddown(slice_length, dip->di_dsize);
    if (sip->slice_length < dip->di_dsize) {
	LogMsg (efp, logLevelCrit, 0,
		"Slice length of " LUF " bytes is too small!\n",
						sip->slice_length);
	exit (FATAL_ERROR);
    }
    *data_resid = (dlimit - (sip->slice_length * num_slices));
    *data_resid = rounddown(*data_resid, dip->di_dsize);
    return;
}

static void
setup_slice(struct dinfo *dip, slice_info_t *sip)
{
    file_position = sip->slice_position;
    if (dip->di_random_io) {
	rdata_limit = (file_position + sip->slice_length);
    }
    /*
     * Restrict data limit to slice length or user set limit.
     */
    data_limit = MIN(data_limit, sip->slice_length);
    if (debug_flag || Debug_flag) {
	large_t dlimit = (dip->di_random_io) ? rdata_limit : data_limit;
	Printf("Slice %d Information:\n"
		"\t\t Start: " FUF " offset (lba " LUF ")\n"
		"\t\t   End: " FUF " offset (lba " LUF ")\n"
		"\t\tLength: " FUF " bytes (" LUF " blocks)\n"
		"\t\t Limit: " FUF " bytes (" LUF " blocks)\n",
		sip->slice,
		file_position, (large_t)(file_position / dip->di_dsize),
		(file_position + sip->slice_length),
		(large_t)((file_position + sip->slice_length) / dip->di_dsize),
		sip->slice_length, (large_t)(sip->slice_length / dip->di_dsize),
		dlimit, (large_t)(dlimit / dip->di_dsize));
    }
    return;
}
