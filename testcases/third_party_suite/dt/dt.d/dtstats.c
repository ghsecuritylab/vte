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
 * Module:	dtstats.c
 * Author:	Robin T. Miller
 *
 * Description:
 *	Display statistics information for generic data test program.
 *
 * Modification History:
 *
 * June 24th, 2004 by Robin Miller.
 *      Improve data pattern display when multiple passes specified.
 * Previously, insufficent/misleading information was displayed when
 * IOT or lbdata was enabled.
 *
 * December 6th, 2003 by Robin Miller.
 *      Conditionalize to exclude tty code.
 *
 * November 17th, 2003 by Robin Miller.
 *	Breakup output to stdout or stderr, rather than writing
 * all output to stderr.  If output file is stdout ('-') or a log
 * file is specified, then all output reverts to stderr.
 *
 * January 29th, 2002 by Robin Miller.
 *	Display the start and end date/time for per pass statistics.
 *
 * January 24th, 2001 by Robin Miller.
 *	If variable block sizes enabled, display the random seed used.
 *	When using IOT test pattern, display the blocking factor in the
 * total statistics, now that "lbs=" can alter the default size.
 *
 * January 15th, 2001 by Robin Miller.
 *	On Windows, enable display of the system and user times, since
 * this is reported properly by the latest Cygnus DLL (1.1.7-1 used).
 * When testing multiple volumes, display the total volumes completed.
 *
 * January 14th, 2001 by Robin Miller.
 *	Added support for multiple volumes option.
 *
 * January 11th, 2001 by Robin Miller.
 *	Fix problem with displaying 64-bit total tape files count.
 *
 * December 30th, 2000 by Robin Miller.
 *	Make changes to build using MKS/NuTCracker product.
 *
 * November 19th, 2000 by Robin Miller.
 *	When testing with the lbdata option, display both the starting
 * lba and the size used.  On Tru64 Unix, device size should be default.
 *
 * May 5th, 2000 by Robin Miller.
 *	When displaying total statistics, if the total errors is zero,
 * and the exit status is not SUCCESS, then display the exit status to
 * help diagnose improper exit status problems.
 *
 * April 25th, 2000 by Robin Miller.
 *	During multiple passes, display the current pattern read/written,
 * unless using a pattern file.  We used to work this way before we put our
 * data pattern into the pattern buffer (regression).
 *
 * April 18th, 2000 by Robin Miller.
 *	Modified statistics, so each set of stats has a newline prepended
 * which makes the formatting more consistent, and matchs 'scu's output.
 *
 * February 17th, 2000 by Robin Miller.
 *	Reset end of file/logical/media flags at end of pass.
 *
 * December 31st, 1999 by Robin Miller.
 *	Display the type of I/O performed, and if random also display the
 * random seed so the user has the option of specifying this same seed.
 *
 * July 22nd, 1999 by Robin Miller.
 *	Added support for IOT (DJ's) test pattern.
 * 
 * May 3, 1999 by Robin Miller.
 *	Added table wait time for DUNIX systems.
 *
 * April 8, 1999 by Robin Miller.
 *	Merge in Jeff Detjen's changes for table()/sysinfo timing.
 *
 * December 19, 1998 by Robin Miller.
 *	Fix problem reporting total files processed count when we have
 * not completed a pass (exiting prematurely due to error or signal).
 *
 * November 16, 1998 by Robin Miller.
 *	Moved location of where Asynchonous I/O's (AIO's) got reported.
 *
 * November 3, 1998 by Robin Miller.
 *	If asynchronous I/O (AIO) is enabled, display the AIO count used
 * in the total statistics (needed to distiguish from normal sync I/O).
 *
 * May 13, 1997 by Robin Miller.
 *	Report number of I/O's per second (some people desire this).
 *
 * February 18, 1996 by Robin Miller.
 *	Added support for 'copy' and 'verify' statistics.
 *
 * October 27, 1995 by Robin Miller.
 *	Correctly use "hz" variable instead of "HZ" define for clock ticks.
 *	[ NOTE: This could have resulted in wrong performance statistics. ]
 *
 * August 12, 1994 by Robin Miller.
 *	Removed extra "%" character in "Total passes..." printf, which
 *	prevents this line from being formatted and displayed on Gold.
 *
 * October 28, 1993 by Robin Miller.
 *	For multiple processes, display the PID to differentiate output.
 *
 * Septemeber 1, 1993 by Robin Miller.
 *	Display variable record size parameters when enabled.
 *	Properly display unsigned decimal values to prevent negative
 *	values from being displayed.
 *
 * August 13, 1993 by Robin Miller.
 *	Display data pattern for per pass statistics when multiple passes
 *	are selected, since a different pattern is used for each pass.
 *	Also added field width (08) when printing the hex pattern value.
 *
 * August 17, 1993 by Robin Miller.
 *	Display the program start & end times for total statistics.
 *
 * August 5, 1993 by Robin Miller.
 *	Added displaying of tape files processed (if enabled by user).
 *
 * August 4, 1993 by Robin Miller.
 *	Use buffering printing functions to ensure statistics get written
 *	contiguously to the log file (necessary for multiple processes).
 *
 */
#include "dt.h"

static char *stats_names[] = {
	"Copy",			/* COPY_STATS */
	"Read",			/* READ_STATS */
	"Read After Write",	/* RAW_STATS */
	"Write",		/* WRITE_STATS */
	"Total",		/* TOTAL_STATS */
	"Verify"		/* VERIFY_STATS */
};
static char *data_op_str = "Data operation performed: ";

/*
 * Functions to Process Statistics:
 */
void
gather_stats(struct dinfo *dip)
{
    /*
     * Gather per pass statistics.
     */
    total_files_read += dip->di_files_read;
    total_files_written += dip->di_files_written;
    total_bytes_read += dip->di_dbytes_read;
    total_bytes_written += dip->di_dbytes_written;
    total_partial_reads += dip->di_partial_reads;
    total_partial_writes += dip->di_partial_writes;
    total_records_read += dip->di_full_reads;
    total_records_written += dip->di_full_writes;
    /* TODO: Cleanup these two variables. */
    records_processed = (dip->di_full_reads + dip->di_full_writes);
    partial_records = (dip->di_partial_reads + dip->di_partial_writes);
}

void
gather_totals(struct dinfo *dip)
{
    /*
     * Gather total (accumulated) statistics:
     */
    /* TODO: Move total statistics to dip structure! */
    total_bytes = (total_bytes_read + total_bytes_written);
    total_files = (total_files_read + total_files_written);
    total_records += records_processed;
    total_partial += partial_records;
    total_errors += error_count;
}

void
init_stats(struct dinfo *dip)
{
    /*
     * Initial fields in preparation for the next pass.
     */
    end_of_file = FALSE;
    error_count = (vu_long) 0;
    dip->di_end_of_file = FALSE;
    dip->di_end_of_media = FALSE;
    dip->di_end_of_logical = FALSE;
    dip->di_files_read = (vu_long) 0;
    dip->di_dbytes_read = (v_large) 0;
    dip->di_vbytes_read = (v_large) 0;
    dip->di_records_read = (vu_long) 0;
    dip->di_files_written = (vu_long) 0;
    dip->di_dbytes_written = (v_large) 0;
    dip->di_vbytes_written = (v_large) 0;
    dip->di_records_written = (vu_long) 0;
    dip->di_volume_records = (vu_long) 0;
    dip->di_full_reads = (u_long) 0;
    dip->di_full_writes = (u_long) 0;
    dip->di_partial_reads = (u_long) 0;
    dip->di_partial_writes = (u_long) 0;
    records_processed = (u_long) 0;
}

/************************************************************************
 *									*
 * report_pass() - Report end of pass information.			*
 *									*
 * Inputs:								*
 *	stats_type = Type of statistics to display (read/write/total).	*
 *									*
 ************************************************************************/
void
report_pass(struct dinfo *dip, enum stats stats_type)
{
    gather_stats(dip);		/* Gather the total statistics.	*/
    gather_totals(dip);		/* Update the total statistics. */

    if (pstats_flag && (stats_level == STATS_FULL) ) {
        if (stats_flag) {
	    report_stats(dip, stats_type);
	}
    } else if ( (!pstats_flag && verbose_flag) || /* compatability */
                (pstats_flag && (stats_level == STATS_BRIEF)) ) {
#if defined(DEC)
	table(TBL_SYSINFO,0,(char *)&e_table,1,sizeof(struct tbl_sysinfo));
#endif /* defined(DEC) */
        if ( user_keepalive && !user_pkeepalive &&
             (time((time_t *)0) > lastalarm) ) {
          char alivemsg[STRING_BUFFER_SIZE];
          if (FmtKeepAlive(dip, keepalive, alivemsg) == SUCCESS) {
              LogMsg (ofp, logLevelLog, PRT_NOIDENT, "%s\n", alivemsg);
          }
        }
        if ( pkeepalive && strlen(pkeepalive) ) {
            /* TODO: Make stats type available in FmtKeepAlive()! */
	    Lprintf ("End of %s ", stats_names[(int)stats_type]);
            FmtKeepAlive(dip, pkeepalive, log_bufptr);
            LogMsg (ofp, logLevelLog, PRT_NOIDENT, "%s\n", log_buffer);
	    log_bufptr = log_buffer;
        }
    }
    /*
     * Re-initialize the per pass counters and flags.
     */
    init_stats(dip);
    return;
}

/************************************************************************
 *									*
 * report_stats() - Report statistics at end of pass or program.	*
 *									*
 * Inputs:								*
 *	stats_type = Type of statistics to display (read/write/total).	*
 *									*
 ************************************************************************/
void
report_stats(struct dinfo *dip, enum stats stats_type)
{
	double bytes_sec, kbytes_sec, ios_sec, Kbytes, Mbytes;
	large_t xfer_bytes, xfer_records;
	unsigned long xfer_partial;
	clock_t at, et;
	struct tms *stms, *etms = &etimes;
	char pbuf[50];
	char *pinfo = pbuf;
#if defined(DEC)
	long table_user, table_sys, table_idle, table_wait;
	struct tbl_sysinfo *stbl, *etbl = &e_table;
#endif /* defined(DEC) */

        if (!stats_flag || (stats_level == STATS_NONE) ) return;

        if ( (stats_type == TOTAL_STATS) && (stats_level == STATS_BRIEF) ) {
            if ( user_keepalive && !user_tkeepalive &&
                 (time((time_t *)0) > lastalarm) ) {
                if (FmtKeepAlive(dip, keepalive, log_buffer) == SUCCESS) {
                    LogMsg (ofp, logLevelLog, PRT_NOIDENT, "%s\n", log_buffer);
                }
            }
            if ( tkeepalive && strlen(tkeepalive) ) {
                if (FmtKeepAlive(dip, tkeepalive, log_buffer) == SUCCESS) {
                    LogMsg (ofp, logLevelLog, PRT_NOIDENT, "%s\n", log_buffer);
                }
            }
            return;
        }
	end_time = times (etms);
#if defined(DEC)
	table(TBL_SYSINFO,0,(char *)etbl,1,sizeof(struct tbl_sysinfo));
#endif /* defined(DEC) */
	if (num_procs || num_slices || forked_flag) {
	    Lprintf ("\n%s Statistics (%d):\n",
				stats_names[(int)stats_type], getpid());
	} else {
	    Lprintf ("\n%s Statistics:\n", stats_names[(int)stats_type]);
	}

	if (stats_type == TOTAL_STATS) {
	    et = end_time - start_time;		/* Test elapsed time.	*/
	    stms = &stimes;			/* Test start times.	*/
#if defined(DEC)
	    stbl = &s_table;			/* Test start table.	*/
#endif /* defined(DEC) */
	    xfer_bytes = total_bytes;		/* Total bytes xferred.	*/
	    xfer_records = total_records;	/* Total records xfered	*/
	    xfer_partial = total_partial;	/* Total partial records*/
	} else {
	    et = end_time - pass_time;		/* Pass elapsed time.	*/
	    stms = &ptimes;			/* Test pass times.	*/
#if defined(DEC)
	    stbl = &p_table;			/* Test pass table.	*/
#endif /* defined(DEC) */
	    xfer_bytes = dip->di_dbytes_read;	/* Data bytes per pass.	*/
	    xfer_bytes += dip->di_dbytes_written;/* Data bytes written.	*/
	    xfer_records = records_processed;	/* Data records/pass.	*/
	    xfer_partial = partial_records;	/* Partial records.	*/
	}

	if (stats_type == TOTAL_STATS) {
	    /*
	     * Display device tested & other device information.
	     */
	    if (input_file) {
		struct dinfo *dip = input_dinfo;
		struct dtype *dtp = dip->di_dtype;
		Lprintf ("%30.30s%s",
				"Input device/file name: ", input_file);
		if (input_dtype->dt_dtype != DT_UNKNOWN) {
		    if (dip->di_device != NULL) {
			Lprintf (" (Device: %s, type=%s)\n",
					dip->di_device, dtp->dt_type);
		    } else {
			Lprintf (" (device type=%s)\n", dtp->dt_type);
		    }
		} else {
		    Lprintf ("\n");
		}
	    } else {
		struct dinfo *dip = output_dinfo;
		struct dtype *dtp = dip->di_dtype;
		Lprintf ("%30.30s%s",
				"Output device/file name: ", output_file);
		if (output_dtype->dt_dtype != DT_UNKNOWN) {
		    if (dip->di_device != NULL) {
			Lprintf (" (Device: %s, type=%s)\n",
					dip->di_device, dtp->dt_type);
		    } else {
			Lprintf (" (device type=%s)\n", dtp->dt_type);
		    }
		} else {
		    Lprintf ("\n");
		}
	    }

	    Lprintf ("%30.30s", "Type of I/O's performed: ");
	    if (io_type == RANDOM_IO) {
		Lprintf ("random (rseed=%#x", random_seed);
	    } else {
		Lprintf ("sequential (%s",
			(io_dir == FORWARD) ? "forward" : "reverse");
		if (variable_flag) {
		    Lprintf (", rseed=%#x", random_seed);
		}
	    }
	    if (raw_flag) {
		Lprintf (", read-after-write)\n");
	    } else {
		Lprintf(")\n");
	    }
	    if (io_type == RANDOM_IO) {
		Lprintf ("%30.30s", "Random I/O Parameters: ");
		Lprintf ("position=" FUF ", ralign=%ld, rlimit=" LUF "\n",
			file_position, random_align, rdata_limit);
	    } else if (num_slices) {
		Lprintf ("%30.30s", "Slice Range Parameters: ");
		Lprintf ("position=" FUF " (lba %u), limit=" LUF "\n",
		file_position, (u_int32)(file_position / dip->di_dsize), data_limit);
	    }

	    if (align_offset || rotate_flag) {
		Lprintf ("%30.30s", "Buffer alignment options: ");
		if (align_offset) {
		    Lprintf ("alignment offset = %d bytes\n", align_offset);
		} else {
		    Lprintf ("rotating through 1st %d bytes\n", ROTATE_SIZE);
		}
	    }
	}

	if (num_procs || num_slices) {
	    Lprintf ("%30.30s%d/%d\n",
			(num_slices) ? "Current Slice Reported: "
				     : "Current Process Reported: ",
					cur_proc,
			(num_procs) ? num_procs : num_slices);
	}

#if defined(TTY)
	if (ttyport_flag && (stats_type == TOTAL_STATS)) {
	    Lprintf ("%30.30sflow=%s, parity=%s, speed=%s\n",
				"Terminal characteristics: ",
				flow_str, parity_str, speed_str);
	}
#endif /* defined(TTY) */

    if (io_mode == TEST_MODE) {
	/*
	 * Extra information regarding pattern verification.
	 */
	bzero (pinfo, sizeof(*pinfo));
	if (output_file && !verify_flag) {
	    pinfo = " (read verify disabled)";
	} else if (!compare_flag) {
	    pinfo = " (data compare disabled)";
	} else if (incr_pattern) {
	    sprintf(pinfo, " (incrementing 0-255)");
	} else if (iot_pattern) {
	    sprintf(pinfo, " (blocking is %u bytes)", lbdata_size);
	} else if (pattern_file || pattern_string) {
	    sprintf(pinfo, " (first %u bytes)", sizeof(pattern));
	} else if (lbdata_flag) {
	    sprintf(pinfo, " (w/lbdata, lba %u, size %u bytes)",
					 lbdata_addr, lbdata_size);
	}
	if ( (stats_type == TOTAL_STATS) || (pass_limit > 1L) ) {
	    if (prefix_string) {
		Lprintf ("%30.30s'%s'\n",
			"Data pattern prefix used: ", prefix_string);
	    }
	    if (pattern_file) {
		Lprintf ("%30.30s%s (%lu bytes)\n",
			"Data pattern file used: ", pattern_file, patbuf_size);
	    } else if (pattern_string) {
		Lprintf ("%30.30s'%s'%s\n",
				"Data pattern string used: ", pattern_string,
				(iot_pattern) ? pinfo : "");
	    }
	    if (!iot_pattern) {
	      if (output_file && verify_flag) {
		Lprintf ("%30.30s0x%08x%s\n",
				"Data pattern read/written: ", pattern, pinfo);
	      } else if (output_file && !verify_flag) {
		Lprintf ("%30.30s0x%08x%s\n",
				"Data pattern written: ", pattern, pinfo);
	      } else {
		Lprintf ("%30.30s0x%08x%s\n",
				"Data pattern read: ", pattern, pinfo);
	      }
	    }
	}
    } else { /* !TEST_MODE */
	if ( (stats_type == COPY_STATS) ||
	     ((stats_type == TOTAL_STATS) && !verify_flag) ) {
	    Lprintf ("%30.30sCopied '%s' to '%s'.\n",
				data_op_str,
				input_dinfo->di_dname,
				output_dinfo->di_dname);
	} else if ( (stats_type == VERIFY_STATS) ||
		    ((stats_type == TOTAL_STATS) && verify_only) ) {
	    Lprintf ("%30.30sVerified '%s' with '%s'.\n",
				data_op_str,
				input_dinfo->di_dname,
				output_dinfo->di_dname);
	}
    } /* end 'if (io_mode == TEST_MDOE)' */
	if (min_size) {
	    Lprintf ("%30.30s" LUF " with min=%ld, max=%ld, incr=",
				"Total records processed: ",
				(xfer_records + xfer_partial),
				min_size, max_size);
	    if (variable_flag) {
		Lprintf ("variable\n");
	    } else {
		Lprintf ("%ld\n", incr_count);
	    }
	} else {
	    Lprintf ("%30.30s" LUF " @ %ld bytes/record",
				"Total records processed: ",
				xfer_records, block_size);
	    if (xfer_partial) {
		Lprintf (", %lu partial\n", xfer_partial);
	    } else {
		Lprintf (" (%.3f Kbytes)\n",
				((double)block_size / (double) KBYTE_SIZE));
	    }
	}
    
#if !defined(__GNUC__) && ( defined(_NT_SOURCE) || defined(WIN32) )
    /* Avoid:  error C2520: conversion from unsigned __int64 to double not implemented, use signed __int64 */
	Kbytes = (double) ( (double)(slarge_t) xfer_bytes / (double) KBYTE_SIZE);
	Mbytes = (double) ( (double)(slarge_t) xfer_bytes / (double) MBYTE_SIZE);
#else /* !defined(_NT_SOURCE) */
	Kbytes = (double) ( (double) xfer_bytes / (double) KBYTE_SIZE);
	Mbytes = (double) ( (double) xfer_bytes / (double) MBYTE_SIZE);
#endif /* !defined(__GNUC__) ( defined(_NT_SOURCE) || defined(WIN32) ) */
	    
	Lprintf ("%30.30s" LUF " (%.3f Kbytes, %.3f Mbytes)\n",
				"Total bytes transferred: ",
				xfer_bytes, Kbytes, Mbytes);

	/*
	 * Calculate the total clock ticks (hz = ticks/second).
	 */
	if (et) {
#if !defined(__GNUC__) &&  defined(_NT_SOURCE) 
	    bytes_sec = ((double)(slarge_t)xfer_bytes / ((double)et / (double)hz));
#elif defined(WIN32)
	    bytes_sec = ((double)(slarge_t)xfer_bytes / (double)et );
#else /* !defined(_NT_SOURCE) */
	    bytes_sec = ((double)xfer_bytes / ((double)et / (double)hz));
#endif /* !defined(__GNUC__) && defined(_NT_SOURCE) */
	} else {
	    bytes_sec = 0.0;
	}
#if !defined(__GNUC__) && ( defined(_NT_SOURCE) || defined(WIN32) )
	kbytes_sec = (double) ( (double) bytes_sec / (double) KBYTE_SIZE);
#else /* !defined(_NT_SOURCE) */
        kbytes_sec = (double) ( (double) bytes_sec / (double) KBYTE_SIZE);
#endif /* !defined(__GNUC__) && defined(_NT_SOURCE) */
	Lprintf ("%30.30s%.0f bytes/sec, %.3f Kbytes/sec\n",
				"Average transfer rates: ",
				bytes_sec, kbytes_sec);
#if defined(AIO)
	if (stats_type == TOTAL_STATS) {
	    if (aio_flag) {
		Lprintf("%30.30s%d\n", "Asynchronous I/O's used: ", aio_bufs);
	    }
	}
#endif /* defined(AIO) */

	if (et && xfer_records) {
#if !defined(__GNUC__) && ( defined(_NT_SOURCE) || defined(WIN32) )
	    double records = (double)(slarge_t)(xfer_records + xfer_partial);
#else /* !defined(_NT_SOURCE) */
	    double records = (double)(xfer_records + xfer_partial);
#endif /* !defined(__GNUC__) && defined(_NT_SOURCE) */
	    ios_sec = (records / ((double)et / (double)hz));
	} else {
	    ios_sec = 0.0;
	}
	Lprintf ("%30.30s%.3f\n", "Number I/O's per second: ", ios_sec);

	if (multi_flag || volumes_flag) {
	    Lprintf ("%30.30s", "Total volumes completed: ");
	    if (volumes_flag) {
		Lprintf ("%d/%d\n", multi_volume, volume_limit);
	    } else {
		Lprintf ("%d\n", multi_volume);
	    }
	}

	Lprintf ("%30.30s", "Total passes completed: ");
	if (runtime) {
	    Lprintf ("%lu\n", pass_count);
	} else {
	    Lprintf ("%lu/%lu\n", pass_count, pass_limit);
	}

	if (file_limit) {
	    if ((stats_type == TOTAL_STATS) && output_file && verify_flag) {
		/*
		 * When writing/reading multiple tape files, we'll actually
		 * process double the specified number of file marks.
		 */
		if (total_files > file_limit) {
		    file_limit *= 2;	/* Make stats look kosher... */
		}
	    }
	    Lprintf ("%30.30s", "Total files processed: ");
	    if ((stats_type == TOTAL_STATS) && pass_count && total_files) {
		Lprintf (LUF "/" LUF "\n", total_files,
				((large_t)pass_count * (large_t)file_limit));
	    } else {
		u_long files = (dip->di_files_read + dip->di_files_written);
		Lprintf ("%lu/%lu\n", files, file_limit);
	    }
	}

	Lprintf ("%30.30s%lu/%lu",
				"Total errors detected: ",
	    (stats_type == TOTAL_STATS) ? total_errors : error_count,
						error_limit);
	if (stats_type == TOTAL_STATS) {
	    if ( (total_errors == 0) &&
		 (exit_status != SUCCESS) && (exit_status != END_OF_FILE) ) {
		Lprintf (" (exit status is %u!)");
	    }
	}
	Lprintf("\n");

	/*
	 * Report elapsed, user, and system times.
	 */
	Lprintf ("%30.30s", "Total elapsed time: ");
	format_time (et);
#if !defined(WIN32)
	/*
	 * More ugliness for Windows, since the system and
	 * user times are always zero, don't display them.
	 */
	Lprintf ("%30.30s", "Total system time: ");
	at = etms->tms_stime - stms->tms_stime;
	at += etms->tms_cstime - stms->tms_cstime;
	format_time (at);

	Lprintf ("%30.30s", "Total user time: ");
	at = etms->tms_utime - stms->tms_utime;
	at += etms->tms_cutime - stms->tms_cutime;
	format_time (at);
#endif /* !defined(WIN32) */

#if defined(DEC)
	if (table_flag) {
	    if ( (etbl->si_hz != stbl->si_hz) ) {
		Fprintf ("Inconsistency! etbl->si_hz = %ld, stbl->si_hz = %ld\n",
								etbl->si_hz, stbl->si_hz);
	    }
	    table_user = (etbl->si_user - stbl->si_user);
	    table_sys = (etbl->si_sys - stbl->si_sys);
	    table_idle = (etbl->si_idle - stbl->si_idle);
	    table_wait = (etbl->wait - stbl->wait);

	    Lprintf ("%30.30s", "Table system time: ");
	    format_ltime(table_sys, (int)stbl->si_hz);

	    Lprintf ("%30.30s", "Table user time: ");
	    format_ltime (table_user, (int)stbl->si_hz);

	    Lprintf ("%30.30s", "Table idle time: ");
	    format_ltime (table_idle, (int)stbl->si_hz);

	    Lprintf ("%30.30s", "Table wait time: ");
	    format_ltime (table_wait, (int)stbl->si_hz);
	}
#endif /* defined(DEC) */

	Lprintf ("%30.30s", "Starting time: ");
	Ctime(program_start);
	program_end = time((time_t) 0);
	Lprintf ("%30.30s", "Ending time: ");
	Ctime(program_end);
	Lprintf ("\n");
	Lflush();
}
