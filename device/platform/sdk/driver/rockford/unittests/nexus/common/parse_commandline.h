/******************************************************************************
 *    (c)2008 Broadcom Corporation
 *
 * This program is the proprietary software of Broadcom Corporation and/or its licensors,
 * and may only be used, duplicated, modified or distributed pursuant to the terms and
 * conditions of a separate, written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 * no license (express or implied), right to use, or waiver of any kind with respect to the
 * Software, and Broadcom expressly reserves all rights in and to the Software and all
 * intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 * HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 * NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 *
 * Except as expressly set forth in the Authorized License,
 *
 * 1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 * secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of Broadcom integrated circuit products.
 *
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 * AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 * WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
 * THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
 * OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
 * LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
 * OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
 * USE OR PERFORMANCE OF THE SOFTWARE.
 *
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
 * LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
 * EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 * USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
 * ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 * LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 * ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: parse_commandline.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 6/7/12 6:17p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/common/parse_commandline.h $
 * 
 * Hydra_Software_Devel/1   6/7/12 6:17p mphillip
 * SW7425-2907: Add command-line support for extended testing
 *
 *
 *
******************************************************************************/


/* parse_commandline.h
 *
 * This file provides a set of functions to allow flexible looping for torture test
 * versions of unit tests.
 *
 * The functions are:
 *
 * channels_file_name(argc, argv), which returns a string matching -channels [filename]
 * if it's provided on the commandline, or the default "channels.txt" otherwise.  It is
 * intended to be used in conjunction with parse_channels.h's parse_channels function:
 *     entries = parse_channels(channels_file_name(argc,argv),&num_entries);
 *
 * loop_from_commandline(argc, argv, default_iterations) will return a value usable in
 * while().  If no command-line arguments overrule the default, it will assume the caller
 * wants to iterate default_iterations times.  To do this a single time (but allow extended
 * running), wrap the test in:
 *
 *     while (loop_from_commandline(argc, argv, 1) {
 *         ...test code goes here...
 *     }
 *
 * The supported command-line parameters are:
 *     -count #
 *             iterate # number of times
 *     -for HH[:MM[:SS]]
 *             iterate for HH hours, MM minutes, and SS seconds.
 *             (neither MM nor SS are required)
 *
 * This allows any unit test to, in three lines (an include, and a surrounding while loop)
 * to support extended testing while still defaulting to a unittest-friendly version.
 *
 * (Note: this header can only be included once, and it includes code -- this allows it to
 * be dropped into existing unittests without modifying makefiles to link libraries or add
 * other source files.)
 */


#ifndef __PARSE_COMMANDLINE_H__
#define __PARSE_COMMANDLINE_H__

#include <string.h>
#include <stdlib.h>
#include <sys/time.h>

#define is_str(x,y) (!strcasecmp(x,y))

const char *channels_file_name(int argc, char **argv) {
    const char *rv = "channels.txt";
    int i;

    for (i=0; i < argc-1; i++) {
        if (is_str(argv[i],"-channels")) {
            rv = argv[i+1];
        }
    }

    return rv;
}

bool loop_from_commandline(int argc, char **argv, int default_iterations) {
    static int count = 0;
    static int total_count = 0;
    static struct timeval target_time;
    static bool configured = false;
    static bool loop_on_time = false;

    bool done = false;

    if (!configured) {
        /* only parse the args once */
        int i;

        total_count = default_iterations;
        gettimeofday(&target_time, NULL);
        for (i=0; i < argc-1; i++) {
            if (is_str(argv[i],"-count")) {
                total_count = atoi(argv[i+1]);
            } else if (is_str(argv[i],"-for")) {
                const char *delim = ":";
                bool hours_parsed = false, minutes_parsed = false;
                int hours = 0, minutes = 0, seconds = 0;
                char *s = strdup(argv[i+1]);
                char *t;
                t = strtok(s, delim);
                while (t && *t) {
                    if (!hours_parsed) {
                        hours_parsed = true;
                        hours = atoi(t);
                    } else if (!minutes_parsed) {
                        minutes_parsed = true;
                        minutes = atoi(t);
                    } else
                        seconds = atoi(t);
                    t = strtok(NULL, delim);
                }
                free(s);
                target_time.tv_sec = target_time.tv_sec + seconds + 60*minutes + 60*60*hours + 1/* to cover configuration time*/;
                loop_on_time = true;
            }
                /* -until [time] was considered, but became too complicated due to clock-wrapping and dates.
                 * It's much more likely an extended test run wants "for an hour" than "until 11pm", and the latter
                 * can be duplicated with a little arithmetic.
                 */
        }
        configured = true;
    }

    if (loop_on_time) {
        struct timeval t;
        gettimeofday(&t, NULL);
        if (t.tv_sec >= target_time.tv_sec) {
            done = true;
        }
    } else {
        count++;
        if (count > total_count)
            done = true;
    }

    return !done;
}

#undef is_str

#endif
