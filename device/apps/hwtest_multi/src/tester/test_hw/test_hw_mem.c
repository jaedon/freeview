/*
 * memtester version 4
 *
 * Very simple but very effective user-space memory tester.
 * Originally by Simon Kirby <sim@stormix.com> <sim@neato.org>
 * Version 2 by Charles Cazabon <memtest@discworld.dyndns.org>
 * Version 3 not publicly released.
 * Version 4 rewrite:
 * Copyright (C) 2004 Charles Cazabon <memtest@discworld.dyndns.org>
 * Licensed under the terms of the GNU General Public License version 2 (only).
 * See the file COPYING for details.
 *
 */

#define __version__ "4.0.4"

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>

/*
#include <sys/mman.h>
*/
#include <unistd.h>
#include <errno.h>

#include "mem_types.h"
#include "mem_sizes.h"
#include "mem_tests.h"

#define EXIT_FAIL_NONSTARTER    0x01
#define EXIT_FAIL_ADDRESSLINES  0x02
#define EXIT_FAIL_OTHERTEST     0x04



/* shai : */
#define BSS_ARRAY_SIZE 32*1024*1024
#define MEMORY_GUARD 1024
//unsigned char g_bss_byte_array[BSS_ARRAY_SIZE + MEMORY_GUARD];

/* end of shai */


int mem_tester_main(void);


struct test tests[] = {
    { "Random Value", test_random_value },
	{ "Compare XOR", test_xor_comparison },
    { "Compare SUB", test_sub_comparison },
    { "Compare MUL", test_mul_comparison },
    { "Compare DIV",test_div_comparison },
    { "Compare OR", test_or_comparison },
    { "Compare AND", test_and_comparison },
    { "Sequential Increment", test_seqinc_comparison },
    { "Solid Bits", test_solidbits_comparison },
    { "Block Sequential", test_blockseq_comparison },
    { "Checkerboard", test_checkerboard_comparison },
    { "Bit Spread", test_bitspread_comparison },
    { "Bit Flip", test_bitflip_comparison },
    { "Walking Ones", test_walkbits1_comparison },
    { "Walking Zeroes", test_walkbits0_comparison },
    { NULL, NULL }
};

void FATAL_ERROR(int line);

#ifdef _SC_VERSION
void check_posix_system(void) {
    if (sysconf(_SC_VERSION) < 198808L) {
        DBG_Print("A POSIX system is required.  Don't be surprised if "
            "this craps out.\n");
        DBG_Print("_SC_VERSION is %d\n", sysconf(_SC_VERSION));
    }
}
#else
#define check_posix_system()
#endif

#ifdef _SC_PAGE_SIZE
int memtester_pagesize(void) {
    int pagesize = sysconf(_SC_PAGE_SIZE);
    if (pagesize == -1) {
        perror("get page size failed");
        exit(EXIT_FAIL_NONSTARTER);
    }
    DBG_Print("pagesize is %d\n", pagesize);
    return pagesize;
}
#else
int memtester_pagesize(void) {
    DBG_Print("sysconf(_SC_PAGE_SIZE) not supported; using pagesize of 8192\n");
    return 8192;
}
#endif

int mem_tester_main(void) {

    ul loops, loop, i;
    size_t pagesize, wantmb, wantbytes, bufsize, halflen, count;
    ptrdiff_t pagesizemask;
    void volatile *buf, *aligned;
    ulv *bufa, *bufb;
    int do_mlock = 0, done_mem = 0;
    int exit_code = 0;

	/*DBG_Print_Init();*/

    DBG_Print("memtester version " __version__ " (%d-bit)\n", UL_LEN);
    DBG_Print("memtester version " __version__ " (%d-bit)\n", UL_LEN);
    DBG_Print("Copyright (C) 2004 Charles Cazabon.\n");
    DBG_Print("Licensed under the GNU General Public License version 2 (only).\n");
    DBG_Print("\n");
    check_posix_system();
    pagesize = memtester_pagesize();
    pagesizemask = (ptrdiff_t) ~(pagesize - 1);
    DBG_Print("pagesizemask is 0x%x\n", pagesizemask);

	/*
    if (argc < 2) {
		
        DBG_Print("need memory argument, in MB\n");
        exit(EXIT_FAIL_NONSTARTER);
	
    }
    */
    /* wantmb = (size_t) strtoul(argv[1], NULL, 0); */
	wantmb = (size_t) (BSS_ARRAY_SIZE / (1024 * 1024));

    wantbytes = (size_t) (wantmb << 20);
    if (wantbytes < pagesize) {
        DBG_Print("bytes < pagesize -- memory argument too large?\n");
        exit(EXIT_FAIL_NONSTARTER);
    }

	/*
    if (argc < 3) {
        loops = 0;
    } else {
        loops = strtoul(argv[2], NULL, 0);
    }
	*/
	
	loops = 0;

    DBG_Print("want %dMB (%d bytes)\n", (ul) wantmb, (ul) wantbytes);
    buf = NULL;

    while (!done_mem) {
        while (!buf && wantbytes) {

			/* shai: no malloc just take the static array - g_bss_byte_array */
            buf = (void volatile *) malloc(wantbytes);
			
			/*buf = (void volatile *) g_bss_byte_array; */
            if (!buf) wantbytes -= pagesize;
        }
        bufsize = wantbytes;
        DBG_Print("got  %dMB (%d bytes), trying mlock ...", (ul) wantbytes >> 20,
            (ul) wantbytes);
		DBG_Flush();

		/*
        if ((size_t) buf % pagesize) {
          
            aligned = (void volatile *) ((size_t) buf & pagesizemask);
            bufsize -= ((size_t) aligned - (size_t) buf);
        } else {
            aligned = buf;
        }
		*/
		aligned = (void volatile *)buf ;
        /* Try memlock */

		/* 
		shai: no mlock in ST20 
        if (mlock((void *) aligned, bufsize) < 0) {
		*/
		if (do_mlock) {
            switch(errno) {
                case ENOMEM:
                    DBG_Print("too many pages, reducing...\n");
                    free((void *) buf);
                    buf = NULL;
                    wantbytes -= pagesize;
                    break;
                case EPERM:
                    DBG_Print("insufficient permission.\n");
                    do_mlock = 0;
                    done_mem = 1;
                    break;
                default:
                    DBG_Print("failed for unknown reason.\n");
                    do_mlock = 0;
                    done_mem = 1;
            }
        } else {
            DBG_Print("locked.\n");
            done_mem = 1;
        }
    }

	/*
    if (!do_mlock) DBG_Print("Continuing with unlocked memory; testing "
        "will be slower and less reliable.\n");
	*/

    halflen = bufsize / 2;
    count = halflen / sizeof(ul);
    bufa = (ulv *) aligned;
    bufb = (ulv *) ((size_t) aligned + halflen);

	DBG_Print ("\n using bufa = 0x%x and bufb = 0x%x count is %d \n", bufa, bufb, count);

    for(loop=1; ((!loops) || loop <= loops); loop++) {
        DBG_Print("Loop %d", loop);
        if (loops) {
            DBG_Print("/%d", loops);
        }
        DBG_Print(":\n");
        DBG_Print("  %-20s: \n", "Stuck Address");


		if (!test_stuck_address(aligned, bufsize / sizeof(ul))) {
             DBG_Print("ok\n");
        } else {
            exit_code |= EXIT_FAIL_ADDRESSLINES;
            FATAL_ERROR(__LINE__);
        }
		DBG_Flush();

        for (i=0;;i++) {
            if (!tests[i].name) break;
            DBG_Print("  %-20s: \n", tests[i].name);
            if (!tests[i].fp(bufa, bufb, count)) {
                DBG_Print("ok\n");
            } else {
                exit_code |= EXIT_FAIL_OTHERTEST;
                FATAL_ERROR(__LINE__);
            }
			DBG_Flush();
        }
        DBG_Print("\n");
		DBG_Flush();
    }
    /* if (do_mlock) munlock((void *) aligned, bufsize); */
    DBG_Print("Done.\n");
    exit(exit_code);
}

void FATAL_ERROR(int line)
{
	while(1)
	{
		DBG_Print("FATAL_ERROR!!! (Line:%d)\n", line);
	}
}
