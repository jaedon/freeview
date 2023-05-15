/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcWorkfile:$
 * $brcRevision:$
 * $brcDate:$
 *
 * File Description: Framework initialization file
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/appframework/src/common/appframework/framework.c $
 * 
 * Hydra_Software_Devel/21   9/19/12 4:33p katrep
 * SW7445-45:revise for 7445
 * 
 * Hydra_Software_Devel/20   9/14/12 4:04p katrep
 * SW7445-45:cacheflush not suppoorted on 7445
 * 
 * Hydra_Software_Devel/19   9/14/12 10:18a pblanco
 * SW7346-572: Added code to make framework interrupt setup and teardown
 * symmetrical.
 * 
 * Hydra_Software_Devel/18   9/4/12 1:39p darnstein
 * SW7346-572: roll back recent changes. They broke the check test
 * environment.
 * 
 * Hydra_Software_Devel/17   8/31/12 6:06p darnstein
 * SW7346-572: in AppFramework, destroy task before shutting down
 * interrupt callback that it might use.
 * 
 * Hydra_Software_Devel/16   1/10/12 2:37p tdo
 * SW7420-2211: Add timer exclusion for appframework
 *
 * Hydra_Software_Devel/15   5/3/11 10:40a jhaberf
 * SWDTV-6864: removed uncached memory kludge for 35233
 *
 * Hydra_Software_Devel/14   4/30/11 7:48a jgarrett
 * SWDTV-6864: Making uncached memory 35233-only
 *
 * Hydra_Software_Devel/13   4/29/11 9:25p zhang
 * SWDTV-6864: Use uncached memory in rockford appFramework.
 *
 * Hydra_Software_Devel/12   2/18/11 3:59p tdo
 * SW7420-1456: Fix compiling error for 3548 build
 *
 * Hydra_Software_Devel/11   2/18/11 1:48p tdo
 * SW7420-1456: Acquire pwr resource before BINT open to avoid bus error
 *
 * Hydra_Software_Devel/10   10/18/10 11:35a hongtaoz
 * SW7425-10: use BMEM_SAFETY_FASTEST to improve emulation performance;
 *
 * Hydra_Software_Devel/9   3/11/10 1:48p tdo
 * SW7400-2418: AppFramework: add calls to BPWR_Open/BPWR_Close before
 * calling BCHP_Open
 *
 * Hydra_Software_Devel/8   1/19/10 5:38p kaiz
 * SW35230-56: adding linuxpal support for Palladium emulation environment
 *
 * Hydra_Software_Devel/SW35230-56/1   1/18/10 7:11p kaiz
 * SW35230-56: adding linuxpal support for Palladium emulation environment
 *
 * Hydra_Software_Devel/7   7/15/09 5:39p tdo
 * PR56690: Undo previous check ins
 *
 * Hydra_Software_Devel/6   7/15/09 11:35a jhaberf
 * PR53796: Added infrastruction for 35130 build.
 *
 * Hydra_Software_Devel/5   7/14/09 3:57p tdo
 * PR56690: AppFramework: add calls to BPWR_Open/BPWR_Close before calling
 * BCHP_Open
 *
 * Hydra_Software_Devel/4   4/28/09 3:15p jessem
 * PR 54380: Changed all references to IKOS to EMULATION.
 *
 * Hydra_Software_Devel/3   10/16/07 3:52p jessem
 * PR 36129: Added IKOS support.
 *
 * Hydra_Software_Devel/2   9/21/06 1:42p syang
 * PR 23932: don't setup heap flush if it is not defined
 *
 * Hydra_Software_Devel/1   8/2/06 11:14a syang
 * PR 22212: split board specific frame work out to framework_board.c/h
 *
 * Hydra_Software_Devel/23   6/16/06 10:08a syang
 * PR 21689: don't refer to i2c stuff if i2c is not used
 *
 * Hydra_Software_Devel/22   5/19/06 11:43a pntruong
 * PR 20748:  Hush warnings.
 *
 * Hydra_Software_Devel/21   5/11/06 10:41a jessem
 * PR 17530: Added BTMR support.
 *
 * Hydra_Software_Devel/20   4/13/06 2:46p syang
 * PR 20748: 1). cleanup, 2). update i2c code for 7401 according to
 * bcmtm.c
 *
 * Hydra_Software_Devel/19   4/10/06 5:57p syang
 * PR 20748: fix 97398 compile problem
 *
 * Hydra_Software_Devel/18   4/10/06 2:20p syang
 * PR 20748:  added I2C enabling and fpga program
 *
 * Hydra_Software_Devel/17   3/9/06 5:00p maivu
 * PR 19599: Move OS dependent code to prepare for Vxworks support
 *
 * Hydra_Software_Devel/16   2/14/06 2:38p maivu
 * PR 19599: Use BCHP_7411_VER
 *
 * Hydra_Software_Devel/15   2/13/06 1:57p maivu
 * PR 19599: Fixed 97398 builts
 *
 * Hydra_Software_Devel/14   2/13/06 11:29a hongtaoz
 * PR19082: bring up dual xvd decode for 7400;
 *
 * Hydra_Software_Devel/13   2/10/06 4:59p maivu
 * PR 19599: Fixed errors with prevous check-in for non-97398 builts
 *
 * Hydra_Software_Devel/12   2/10/06 3:31p maivu
 * PR 19599: Add 7411 support for 97398
 *
 * Hydra_Software_Devel/11   2/8/06 8:10p hongtaoz
 * PR19082: bring up xvd_vdc_test for 7400;
 *
 * Hydra_Software_Devel/10   10/5/05 4:57p pntruong
 * PR17203: VDC timeouts because BMEM_FlushCache_idr is not working on
 * linux 2.6/97401.  Fixed with 2612-1.2 kernel.
 *
 * Hydra_Software_Devel/9   9/30/05 2:23p pntruong
 * PR17377, PR17203:  Settop API start up on 7401 will often have one of
 * two VDC timeouts.  This is due to the partial or corrupted rul due to
 * cached does not work properly.  Though, 7401 still need vec reset when
 * doing vdc open to removed previously vec running state and residue
 * interrupts.
 *
 * Hydra_Software_Devel/8   9/13/05 12:31p darnstein
 * PR16885: Program the pin mux so that I2C can be accessed.
 *
 * Hydra_Software_Devel/7   9/13/05 10:23a jasonh
 * PR 16967: Added support for cached memory.
 *
 * Hydra_Software_Devel/6   8/31/05 5:26p jasonh
 * PR 16943: Added I2c support. FPGA reg handle only needed for 7038 Bx.
 *
 * Hydra_Software_Devel/5   7/27/05 4:51p jasonh
 * PR 16397: Removed more chip dependencies.
 *
 * Hydra_Software_Devel/4   7/27/05 4:19p jasonh
 * PR 16397: Broke out code to initialize FPGA
 *
 * Hydra_Software_Devel/3   7/27/05 3:48p jasonh
 * PR 16397: Removed QAM. General clean-up
 *
 * Hydra_Software_Devel/2   2/24/05 2:00p jasonh
 * PR 13514: Fixed compile directive in register setting. Will cause
 * problems when BREG_Write32 is implemented as a macro.
 *
 * Hydra_Software_Devel/1   1/19/05 5:32p jasonh
 * PR 13863: Adding updated application framework.
 *
 * Hydra_Software_Devel/6   11/24/04 3:50p gmullen
 * PR13314: Don't close registers and interrupt from QAM if they weren't
 * opened to begin with.
 *
 * Hydra_Software_Devel/5   2/4/04 7:57a marcusk
 * PR9588: Added support for fpga, tested and working.
 *
 * Hydra_Software_Devel/4   12/30/03 10:11a marcusk
 * PR8916: Updated to properly support the 3250 QAM interrupt interface.
 *
 * Hydra_Software_Devel/3   12/29/03 3:58p marcusk
 * PR9117: Updated with changes required to support interrupt ids rather
 * than strings.
 *
 * Hydra_Software_Devel/2   12/11/03 8:30a frusso
 * PR8916: fixed some build errors
 *
 ***************************************************************************/

/* magnum includes */
#include "bstd.h"
#include "bkni.h"
#include "bchp.h"
#include "bmem.h"
#include "breg_mem.h"
#include "bint.h"
#include "bint_plat.h"

/* framework includes */
#include "systeminit.h"
#include "framework.h"
#include "framework_board.h"
#include "int1.h"
#include "intmgr.h"

/* chip specifics */
#include "platform.h"

#ifdef BCHP_PWR_SUPPORT
#include "bchp_pwr.h"
#endif

#ifdef LINUX
#if (defined(EMULATION) || defined(PALLADIUM_EMULATION))
/* cache support */
#else
	#if BCHP_CHIP == 7445
	#define cacheflush(addr, nbytes, cache) (0)
	#else
	#include <asm/cachectl.h>
	extern int cacheflush(char *addr, int nbytes, int cache);
	#endif
#endif
#endif

BDBG_MODULE( FRAMEWORK );

#define TestError(e, s) {\
    eStatus = e;\
    if (eStatus != BERR_SUCCESS)\
    {\
        BDBG_ERR(( s, __FILE__, __LINE__ ));\
        goto Error;\
    }\
}

#ifdef LINUX
/***************************************************************************/
#if (defined(EMULATION) || defined(PALLADIUM_EMULATION))
void b_cacheflush(const void *addr, size_t nbytes)

{
    BSTD_UNUSED(addr);
    BSTD_UNUSED(nbytes);
}
#else
void b_cacheflush(const void *addr, size_t nbytes)
{
    int rc;
	BSTD_UNUSED(addr);
    BSTD_UNUSED(nbytes);
    rc = cacheflush((void *)addr, nbytes, DCACHE);
    if (rc<0) {
        BDBG_ERR(("cacheflush has returned error %d, ignored", rc));
    }
    return;
}
#endif
#else
void b_cacheflush(const void *addr, size_t nbytes)
{
    BSTD_UNUSED(addr);
    BSTD_UNUSED(nbytes);
    /* TODO: implement for other OSs other than linux */
    return;
}
#endif

/***************************************************************************/
int BFramework_Init(
   const BSystem_Info *pSysInfo,
   BFramework_Info    *pFrameworkInfo )
{
   BERR_Code             eStatus = BERR_SUCCESS;
   const BINT_Settings  *pIntSetting;
   BTMR_DefaultSettings  stTimerSettings;
   BMEM_Settings mem_module_settings;
   BMEM_Heap_Settings mem_heap_settings;

   /* Clear all settings (uses KNI before init) */
   BKNI_Memset(pFrameworkInfo, 0x0, sizeof(BFramework_Info));

   /* KNI module */
   TestError( BKNI_Init(),
              "ERROR: BKNI_Init: %s:%d" );

   /* DBG module */
   TestError( BDBG_Init(),
              "ERROR: BDBG_Init: %s:%d" );

   /* REG handle */
   BREG_Open( &pFrameworkInfo->hReg,
              pSysInfo->bregBaseAddr, pSysInfo->bregMaxRegOffset );

#ifdef BPWR_SUPPORT
   /* PWR handle */
   TestError( BPWR_Open(&pFrameworkInfo->hPwr, NULL, pFrameworkInfo->hReg, NULL),
              "ERROR: BPWR_Open: %s:%d" );
#endif

   /* CHP handle */
   TestError( BCHP_OPEN( &pFrameworkInfo->hChp, pFrameworkInfo->hReg ),
              "ERROR: BCHP_OPEN: %s:%d" );

#ifdef BCHP_PWR_SUPPORT
   BCHP_PWR_InitAllHwResources(pFrameworkInfo->hChp);
#endif

#ifdef BCHP_PWR_RESOURCE_BINT_OPEN
   TestError( BCHP_PWR_AcquireResource(pFrameworkInfo->hChp, BCHP_PWR_RESOURCE_BINT_OPEN),
              "ERROR: BCHP_PWR_AcquireResource: %s:%d" );
#endif

   /* INT handle */
   pIntSetting = BINT_GET_SETTINGS();
   TestError( BINT_Open(&pFrameworkInfo->hInt, pFrameworkInfo->hReg, pIntSetting),
              "ERROR: BINT_Open: %s:%d" );

#ifdef BCHP_PWR_RESOURCE_BINT_OPEN
   TestError( BCHP_PWR_ReleaseResource(pFrameworkInfo->hChp, BCHP_PWR_RESOURCE_BINT_OPEN),
              "ERROR: BCHP_PWR_ReleaseResource: %s:%d" );
#endif

   /* Enable all used L1 bits */
   intmgr_Init();
   intmgr_EnableIsrs(pFrameworkInfo->hInt);

   /* Timer module */
   BTMR_GetDefaultSettings(&stTimerSettings);
#if BFramework_TMR_EXCLUSION_MASK
   /* Use default timer settings unless user sets BFramework_TMR_EXCLUSION_MASK in platform.h
      (similar to NEXUS_TMR_EXCLUSION_MASK in nexus_platform_features.h). Linux sometimes uses the last timer on certain chips.
      The timerMask (below) indicates which timers should be EXCLUDED from the list of available timers for nexus/magnum allocation and use.
      If 0 (default), no timers are using used externally. If 2, timer 1 is being used; if 6, timers 1&2 are being used, etc. */
   stTimerSettings.timerMask = BFramework_TMR_EXCLUSION_MASK;
#endif

   TestError( BTMR_Open(&pFrameworkInfo->hTmr, pFrameworkInfo->hChp, pFrameworkInfo->hReg,
                        pFrameworkInfo->hInt, &stTimerSettings),
              "ERROR: BTMR_Open: %s:%d" );

   /* Memory heap for modules */
   TestError( BMEM_GetDefaultSettings(&mem_module_settings),
              "ERROR: BMEM_GetDefaultSettings: %s:%d" );

   TestError( BMEM_Open(&pFrameworkInfo->hMemModule, &mem_module_settings),
              "ERROR: BMEM_Open: %s:%d" );

   TestError( BMEM_Heap_GetDefaultSettings(&mem_heap_settings),
              "ERROR: BMEM_Heap_GetDefaultSettings: %s:%d" );
   mem_heap_settings.uiAlignment = pSysInfo->bmemAlignment;
#ifdef LINUX
   mem_heap_settings.pCachedAddress = pSysInfo->bmemCachedAddress;
#if (!defined(EMULATION) && !defined(PALLADIUM_EMULATION))
   mem_heap_settings.flush = b_cacheflush;
   mem_heap_settings.flush_isr = b_cacheflush;
#else
   mem_heap_settings.eBookKeeping = BMEM_BOOKKEEPING_SYSTEM;
   mem_heap_settings.eSafetyConfig = BMEM_CONFIG_FASTEST;
   mem_heap_settings.flush = NULL;
   mem_heap_settings.flush_isr = NULL;
#endif
#endif

   TestError( BMEM_Heap_Create(pFrameworkInfo->hMemModule,
                               pSysInfo->bmemAddress, pSysInfo->bmemOffset, pSysInfo->bmemSize, &mem_heap_settings,
                               &pFrameworkInfo->hMem),
              "ERROR: BMEM_Heap_Create: %s:%d" );

   /* board specific frame work, such as I2C, FPGA, other chips suchas 7411 */
   TestError( BFrmWorkBoard_Init( pSysInfo, pFrameworkInfo ),
              "ERROR: Board specific frame work: %s:%d" );

   /* Success */
   return 0;

  Error:
   /* Free resources and return error */
   BFramework_Uninit(pFrameworkInfo);

   intmgr_DisableIsrs(pFrameworkInfo->hInt);    
   intmgr_Uninit();
   return eStatus;
}

void BFramework_Uninit( BFramework_Info *pFrameworkInfo )
{
   /* unint board specific first */
   BFrmWorkBoard_Uninit( pFrameworkInfo );

   if ( pFrameworkInfo->hTmr )
   {
      BTMR_Close(pFrameworkInfo->hTmr);
   }

   if ( pFrameworkInfo->hMem )
   {
      BMEM_Heap_Destroy(pFrameworkInfo->hMem);
   }
   BMEM_Close(pFrameworkInfo->hMemModule);

   if ( pFrameworkInfo->hInt )
   {
      intmgr_DisableIsrs(pFrameworkInfo->hInt);  
      intmgr_Uninit();

      BINT_Close(pFrameworkInfo->hInt);
   }

   if ( pFrameworkInfo->hChp )
   {
      BCHP_Close(pFrameworkInfo->hChp);
   }

#ifdef BPWR_SUPPORT
   if ( pFrameworkInfo->hPwr )
   {
      BPWR_Close(pFrameworkInfo->hPwr);
   }
#endif

   if ( pFrameworkInfo->hReg )
   {
      BREG_Close(pFrameworkInfo->hReg);
   }

   BDBG_Uninit();
   BKNI_Uninit();
}
