/***************************************************************************
 *     Copyright (c) 2002-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: vdc_test.c $
 * $brcm_Revision: Hydra_Software_Devel/34 $
 * $brcm_Date:
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/portinginterface/vdc/vdc_test.c $
 * 
 * Hydra_Software_Devel/34   8/16/12 5:47p vanessah
 * SW7425-2501: clean up unnecessary include file
 * 
 * Hydra_Software_Devel/33   5/23/12 11:33a vanessah
 * SW7360-23: add 7360 a0 support
 * 
 * Hydra_Software_Devel/32   5/3/12 1:32p tdo
 * SW7584-21: Add PI/vdc support for 7584
 *
 * Hydra_Software_Devel/31   10/12/11 11:01a pntruong
 * SW7429-16: Initial port.
 *
 * Hydra_Software_Devel/30   5/4/11 11:29a tdo
 * SW7422-417: Add cmd to toggle 3d cap for decode channel open
 *
 * Hydra_Software_Devel/29   4/27/11 4:14p tdo
 * SW7231-133, SW7422-417: Add support for multiple channel decode within
 * a decoder with streamer input and playback  Add option for users to
 * manually force SVD to only decode AVC, this is needed to do multi
 * channel decode with SVD due to memory allocation issue.  Cleanup old
 * code.
 *
 * Hydra_Software_Devel/28   1/18/11 2:01p yuxiaz
 * SW7552-17: Merge from Branch for 7552 support.
 *
 * Hydra_Software_Devel/SW7552-17/1   12/20/10 7:31p xhuang
 * SW7552-17: add 7552 support
 *
 * Hydra_Software_Devel/27   12/2/10 3:09p katrep
 * SW7231-16:add more support for 7231
 *
 * Hydra_Software_Devel/26   11/18/10 5:05p darnstein
 * SW7231-16: port to 7344 and 7346.
 *
 * Hydra_Software_Devel/25   11/1/10 2:03p yuxiaz
 * SW7358-17: Added 7358 support to vdc_test.
 *
 * Hydra_Software_Devel/24   10/26/10 6:40p hongtaoz
 * SW7425-32: added XPT compile support for 7425;
 *
 * Hydra_Software_Devel/23   9/27/10 4:08p hongtaoz
 * SW7425-32: added VDC_TEST_ONLY compile option;
 *
 * Hydra_Software_Devel/22   12/15/09 5:50p yuxiaz
 * SW7408-13: Get vdc_test compile for 7408. Clean up compile flags.
 *
 * Hydra_Software_Devel/21   12/4/09 2:51p syang
 * SW7550-70: add mem alloc msg
 *
 * Hydra_Software_Devel/20   12/1/09 4:46p rpan
 * SW7468-22: Got vdc_test compiled for 7468.
 *
 * Hydra_Software_Devel/19   7/31/09 6:49p syang
 * PR 55812:  add 7550 support
 *
 * Hydra_Software_Devel/18   5/18/09 11:10a tdo
 * PR53239: Remove usage of private defintions in vdc unittest.
 *
 * Hydra_Software_Devel/17   6/8/08 10:40a maivu
 * PR 43417: Remove vcxo and stripe width from app, moved to Appframework
 * and bchp_open
 *
 * Hydra_Software_Devel/16   6/6/08 4:28p maivu
 * PR 43417: Temporary check in vcxo and stripe width settings. Striped
 * width should go to bchp_open.
 *
 * Hydra_Software_Devel/15   3/20/08 1:40p yuxiaz
 * PR39807: Static buffer bring up on 3548.
 *
 * Hydra_Software_Devel/14   2/21/08 4:05p rpan
 * PR39807: Added compile flag for IKOS.
 *
 * Hydra_Software_Devel/13   11/9/07 11:58a yuxiaz
 * PR34540: 7325 bringup.
 *
 * Hydra_Software_Devel/12   10/29/07 2:59p yuxiaz
 * PR34540: Add support for 7325.
 *
 * Hydra_Software_Devel/11   10/16/07 2:46p rpan
 * PR34857: First version after merging the latest xvd_vdc_test and
 * mvd_vdc_test as of Oct. 16, 2007, 12pm.
 *
 * Hydra_Software_Devel/1   9/17/07 4:36p rpan
 * PR34857: Initial revision. Ported from xvd_vdc_test.
 *
 *
 ***************************************************************************/

#include <stdio.h>          /* for printf */
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

#include "config.h"
#include "platform.h"
#include "framework.h"
#include "framework_board.h"

/* base modules */
#include "bstd.h"           /* standard types */
#include "berr.h"           /* error code */
#include "bkni.h"           /* kernel interface */
#include "bkni_multi.h"     /* kernel interface */
#include "btst_kni.h"       /* to handle task */

/* format information */
#include "bfmt.h"

/* audio video common */
#include "bavc.h"

/* top level interrupts */
#include "int1.h"
#include "bint_plat.h"

/* test include */
#include "interactive.h"
#include "test_util.h"
#include "bmem_debug.h"

#if !BTST_VDC_ONLY
#ifndef IKOS_EMULATION
#include "bxpt.h"
#include "bxpt_pcr.h"
#include "bxpt_pcr_offset.h"
#include "bxpt_rave.h"
#include "bxpt_rsbuf.h"
#endif
#endif

#include "btmr.h"
#ifdef VXWORKS
#include "bcmmips.h"
#endif

BDBG_MODULE(vdc_test);

/***************************************************************************
 * Global variables (pointers) for keyboard handler.
 *
 */
BFramework_Info g_frmInfo;


/***************************************************************************/

static BSystem_Info         sysInfo;

static bool      s_bInited = false;
static uintptr_t s_BaseAddr = 0;
static size_t    s_MaxRegOffset = 0;
static uint32_t  s_MemOffset = 0;
static uint32_t  s_uiMemSize = 0;
bool             g_bIntercept = false;

BRDC_Handle       g_hRdc;
BTMR_Handle       g_hTmr;

extern bool     g_bInterceptFrame;
void           *g_pvLumaStartAddress, *g_pvChromaStartAddress;
uint32_t        g_uiLumaBufSize, g_uiChromaBufSize;

/***************************************************************************
 * TerminateTestApp
 ***************************************************************************/
uint32_t TerminateTestApp
(
    const BFramework_Info *pFrmInfo
)
{
    if( !s_bInited ) return 0;

    printf("\nTEST: Clean Up!\n");

    /* This is where anything that needs to get freed or terminated
     * gets done. (i.e. things allocated in the INIT function.) */
    /* release all the opened resources */
    BINT_Close(pFrmInfo->hInt);
    BMEM_Dbg_DumpHeap(pFrmInfo->hMem);
    BMEM_DestroyHeap(pFrmInfo->hMem);
    BCHP_Close(pFrmInfo->hChp);
    BREG_Close(pFrmInfo->hReg);
    BDBG_Uninit();
    BKNI_Uninit();
#ifdef LINUX
    BSystem_Uninit(&sysInfo);
#endif
    s_bInited = false;
    return ( 1 );
}

/***************************************************************************
 * InitializeTestApp
 ***************************************************************************/
uint32_t InitializeTestApp
(
    const BFramework_Info *pFrmInfo
)
{
    BERR_Code            err;

    /* call TerminateTestApp first to free previous allocated resource */
    TerminateTestApp(pFrmInfo);

    /* This is where anything that needs to get initialized for the
     * application to work gets done. */
    printf("\nTEST: Initializing TEST APP!\n");

    /* platform sysinit code: init memmgr and intmgr */
    TestError( BSystem_Init( 0, NULL, &sysInfo ), "ERROR: BSystem_Init" );
        s_BaseAddr     = (uintptr_t)sysInfo.bregBaseAddr;
        s_MaxRegOffset = sysInfo.bregMaxRegOffset;
        s_uiMemSize    = (uint32_t)sysInfo.bmemSize;
        s_MemOffset    = sysInfo.bmemOffset;


#if 0
    #ifndef VXWORKS
        BMEM_SetCache(g_hMemory, sysInfo.bmemCachedAddress, b_cacheflush, b_cacheflush);
    #else
        /* TODO: vxworks cache flush */
        /* BMEM_SetCache(g_hMemory, (void*)BCM_PHYS_TO_K0(s_MemOffset), NULL, NULL); */
    #endif
#endif

    s_bInited = true;
Done:
    return ( err == BERR_SUCCESS );
}

#ifndef IKOS_EMULATION
static void print_usage(void)
{
    printf("\nUsage: vdc_test [-422] [-avc] [-i] [-sh]\n\n");
    printf("Command line arguments list:\n");
    printf("        -422        - 422 frame image\n"
           "        -i          - Enable frmae buffer image interception debug feature (only supported on 93563 as of now)\n"
           "        -sh         - Enable shared memory between MVD/XVD and VDC (only supported on 93563 as of now)\n"
          );

}
#endif

/***************************************************************************
 * main
 ***************************************************************************/
int app_main( int argc, char **argv )
{
    int iErr;
    BSystem_Info sysInfo;
    BFramework_Info frmInfo;
    BERR_Code  err          = BERR_SUCCESS;
    bool b422 = false;
    bool bSharedMem         = false;
    unsigned long ulSharedMemSize = 0;
#ifndef EMULATION
    int argIndex;
#endif

    BSTD_UNUSED(argc);
    BSTD_UNUSED(argv);

#ifndef EMULATION
    /* Process all the command options */
    for (argIndex = 1; argIndex < argc; argIndex++)
    {
        if (!strcmp(argv[argIndex], "-i"))
        {
            g_bIntercept = true;
        }
        else if (!strcmp(argv[argIndex], "-422"))
        {
            b422 = true;
        }
        else if (!strcmp(argv[argIndex], "-sh"))
        {
            bSharedMem = true;
            argIndex++;
            ulSharedMemSize = strtol(argv[argIndex], NULL, 0);
        }
        else
        {
            print_usage();
            return -1;
        }
    }
#endif

    iErr = BSystem_Init( argc, argv, &sysInfo );
    if ( iErr )
    {
        printf( "System init FAILED!\n" );
        return iErr;
    }

    iErr = BFramework_Init( &sysInfo, &frmInfo );
    if ( iErr )
    {
        printf( "Framework init FAILED!\n" );
        return iErr;
    }
    g_frmInfo = frmInfo;

#ifndef VXWORKS
#if BDBG_DEBUG_BUILD
    BDBG_SetLevel(BDBG_eErr);
#endif

#if (BMEM_SAFETY_CONFIG==BMEM_CONFIG_TRACK)
    /* turn on BMEM msg before XVD and XPT are initiated */
    BDBG_SetModuleLevel("BMEM", BDBG_eMsg);
#endif

    /* allocate interception frame buffer */
    if(g_bIntercept)
    {
        AllocateInterceptFrameBuffer(4, b422,
            &g_uiLumaBufSize, &g_pvLumaStartAddress,
            &g_uiChromaBufSize, &g_pvChromaStartAddress);
    }

    BTST_Interactive(&frmInfo, &sysInfo, bSharedMem, ulSharedMemSize, g_bIntercept, b422);

    if (g_bInterceptFrame)
    {
        /* dump the intercepted frame */
        DumpInterceptFrameBuffer(g_pvLumaStartAddress, g_pvChromaStartAddress, b422);
    }

#else   /* VXWORKS */
    InitializeTestApp(&frmInfo);
    BTST_Interactive(&frmInfo.hReg, NULL);
#endif

    /* terminate */
    TerminateTestApp(&frmInfo);

    BFramework_Uninit( &frmInfo );

    BSystem_Uninit(&sysInfo);

    /* return status */
    return err;
}

/* end of file */
