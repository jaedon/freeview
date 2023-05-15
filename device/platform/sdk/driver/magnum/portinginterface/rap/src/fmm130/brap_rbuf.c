/***************************************************************************
*     Copyright (c) 2004-2008, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: brap_rbuf.c $
* $brcm_Revision: Hydra_Software_Devel/86 $
* $brcm_Date: 10/8/08 11:29p $
*
* Module Description:
*   Module name: RBUF
*   This file contains the implementation of all PIs for the Ring
*   Buffer abstraction. 
*
* Revision History:
*
* $brcm_Log: /magnum/portinginterface/rap/base/brap_rbuf.c $
* 
* Hydra_Software_Devel/86   10/8/08 11:29p gdata
* PR42597: [7440] Adding support for ring buffer capture utility.
* 
* Hydra_Software_Devel/85   8/18/08 3:49p gdata
* PR43971: [7601] Fixing compilation error for 7601
* 
* Hydra_Software_Devel/84   8/1/08 6:30p gdata
* PR42597: [7440] Fixing the bug for capture ring buffer
* 
* Hydra_Software_Devel/83   7/29/08 6:27p gdata
* PR42597: [7440] Fixing the bug for capture ring buffer
* 
* Hydra_Software_Devel/82   6/25/08 1:59p gdata
* PR42597: [7440] Fixing the bug for capture ring buffer
* 
* Hydra_Software_Devel/81   6/24/08 6:39p gdata
* PR42597: [7440] Removing compilation warning
* 
* Hydra_Software_Devel/80   6/24/08 6:20p gdata
* PR42597: [7440] Merging to main branch
* 
* Hydra_Software_Devel/Ring_Buffer_Capture/4   6/24/08 4:07p gdata
* PR42597: [7440] Added Command File Generation
* 
* Hydra_Software_Devel/Ring_Buffer_Capture/3   5/23/08 11:53a gdata
* PR42597: [7440] Adding additional check before copying the data in
* buffer
* 
* Hydra_Software_Devel/Ring_Buffer_Capture/2   5/12/08 5:34p gdata
* PR42597: [7440] Adding support for ring buffer capture utility.
* 
* Hydra_Software_Devel/Ring_Buffer_Capture/1   5/12/08 11:12a gdata
* PR42597: [7440] Adding initial support for ring buffer capture utility.
* 
* Hydra_Software_Devel/78   10/12/07 12:19p sushmit
* PR 36003: [7405] Adding support for memory leak debug
* 
* Hydra_Software_Devel/77   10/11/07 4:02p bselva
* PR 35803:[7401,7118,7403,7400] The handling of Rbufsize given during
* Playback start is corrected in PI so that bLoopAroundEn flag is
* honoured properly.
* 
* Hydra_Software_Devel/76   9/30/07 2:01p sushmit
* PR35140: [7405] Develop 7325 Raptor PI for Bringup
* 
* Hydra_Software_Devel/75   8/24/07 5:27p gautamk
* PR23960 :[7401] Ring Buffers allocated/accessed by host should not be
* in the secure region. it should be in global region. Setting the flag
* in Brap_p_allocalign() function.
* 
* Hydra_Software_Devel/74   6/5/07 11:17a speter
* PR 31827: [3563] Merging from Phase 2.0 release branch to main
* 
* Hydra_Software_Devel/RAP_3563_PH_2_0/2   6/5/07 11:15a speter
* PR 31246: [3563] Making the code specific to 3563 as there might be
* problems in the case of playback and capture
* 
* Hydra_Software_Devel/RAP_3563_PH_2_0/1   5/29/07 10:55a speter
* PR 31246: [3563] Noise on first time reboot has been fixed. Warnings
* also supressed.
* 
* Hydra_Software_Devel/73   5/11/07 9:02p sushmit
* PR 27646: Getting 7405 PI compiled
* 
* Hydra_Software_Devel/72   4/25/07 10:14a kagrawal
* PR 30070: [7440, 3563] Removed the zeroing of RBuf hack from start and
* stop added in the last check-in
* 
* Hydra_Software_Devel/71   4/23/07 12:07p kagrawal
* PR 30070: [7440,3563] Zeroing Rbufs during RBuf start and stop
* 
* Hydra_Software_Devel/70   4/17/07 4:41p nitinb
* PR27912 : [3563] Fixed allocation/freeing of ring buffers from pool of
* buffers for init time allocation
* 
* Hydra_Software_Devel/69   4/16/07 5:15p gautamk
* PR28472: [7401,7400] Use #if BRAP_SECURE_HEAP for the changes to
* BRAP_P_AllocAligned(); BRAP_P_Free() and other securtiy related
* changes
* 
* Hydra_Software_Devel/68   3/16/07 5:11p gautamk
* PR27912 :[7440]  Implementaion of Open time memory allocation for Ring
* Buffers
* 
* Hydra_Software_Devel/67   3/5/07 5:09p rjain
* PR 22585: Fixing some issues reported by Coverity
* 
* Hydra_Software_Devel/66   2/8/07 2:46p gautamk
* PR23960 : RAP ZSP FW must reside in a single block of read-only memory
* to support NDS SVP -- Removing Compilation error for other platforms
* not supporting NDS/SVP
* 
* Hydra_Software_Devel/65   2/7/07 3:17p gautamk
* PR23960 : RAP ZSP FW must reside in a single block of read-only memory
* to support NDS SVP
* 
* Hydra_Software_Devel/64   10/12/06 4:15p bselva
* PR 24645: Fixed the compilation warning while compiling in release
* mode.
* 
* Hydra_Software_Devel/63   9/25/06 3:05p kagrawal
* PR 20654: Merged from 7440 bring up branch
* 
* Hydra_Software_Devel/62   9/7/06 11:33a rjain
* PR 22868: changing SECURE_HEAP to BRAP_SECURE_HEAP
* 
* Hydra_Software_Devel/61   8/31/06 2:29p rjain
* PR 22868: adding a check for NULL handle
* 
* Hydra_Software_Devel/60   8/31/06 12:31p rjain
* PR 22868: Moving the hSecureMem handle into BRAP_Settings()
* 
* Hydra_Software_Devel/59   8/23/06 4:31p rjain
* PR 22868: added code to pass a 2nd memory handle (for secure region).
* this 2nd handle is used for RBUF memory allocation. Note: application
* has to take care to pass correct secure memory handle
* 
* Hydra_Software_Devel/58   8/11/06 4:01p bselva
* PR 23192: Fixed the compilation warning
* 
* Hydra_Software_Devel/57   5/25/06 7:54p nitinb
* PR 21794: Implement audio SRC for 7411D0
* 
* Hydra_Software_Devel/56   5/24/06 7:19p bhanus
* PR 21388 : Removing unused variable uiFrameSize.
* 
* Hydra_Software_Devel/55   5/24/06 5:58p bhanus
* PR 21388 : Checking in PCM Playback related changes for 7400.
* 
* Hydra_Software_Devel/54   5/17/06 11:45a rjain
* PR 21588: added some debug msgs
* 
* Hydra_Software_Devel/53   5/13/06 5:13p bhanus
* PR 21388 : Checking in PCM Playback related changes.
* 
* Hydra_Software_Devel/52   5/12/06 6:30p bhanus
* PR 21388 : Checking in PCM Playback related changes.
* 
* Hydra_Software_Devel/51   5/9/06 11:24a rjain
* PR 21481: adding support for 7118 to RAP PI
* 
* Hydra_Software_Devel/50   5/4/06 3:04p bhanus
* PR 21388 : Checking in Add/Remove Output Port related changes for 7400.
* 
* Hydra_Software_Devel/49   4/11/06 5:34p rjain
* PR 20318: changing code like "#if 7401 || 7400" to "#ifndef
* BCHP_7411_VER  "
* 
* Hydra_Software_Devel/48   3/30/06 2:26p kagrawal
* PR 20318: Implemented reveiw comments on Audio Manager
* 
* Hydra_Software_Devel/47   3/1/06 9:38a kagrawal
* PR 18264: Implemented review comments for PCM playback
* 
* Hydra_Software_Devel/46   2/17/06 9:55a sushmit
* PR18604: Merging Encoder to latest.
* 
* Hydra_Software_Devel/RAP_AUD_ENC/3   2/6/06 4:17p sushmit
* PR18604: Merging latest Decoder PI to Encoder PI branch
* 
* Hydra_Software_Devel/45   1/23/06 4:28p kagrawal
* PR 19023: 1. Setting write pointer to the ring buffer start address
* during rbuf start irrespective of the channel type. This will ensure
* that starting of pcm playback 2nd time, there is no leftover from
* previous playback
* 2. Removed eChannelType passed to BRAP_RBUF_P_Start
* 
* Hydra_Software_Devel/44   1/19/06 7:24p bselva
* PR 19168: Checking in the changes required for 7400 bringup
* 
* Hydra_Software_Devel/43   12/13/05 4:40p nitinb
* PR 18163: RAP BMEM leak on system uninit; calls to BMEM_AllocAligned
* but no calls to BMEM_Free
* 
* Hydra_Software_Devel/42   12/5/05 11:40a kagrawal
* PR 18312: Removed obsolete code - BRAP_RBUF_GetReadPointer () and
* BRAP_RBUF_GetWritePointer()
* 
* Hydra_Software_Devel/41   12/1/05 5:48p kagrawal
* PR 18312: Corrected FREEFULL MARK programming
* 
* Hydra_Software_Devel/40   11/10/05 5:00p kagrawal
* PR 17590: Fixed compilation error for 7411
* 
* Hydra_Software_Devel/39   11/10/05 11:54a kagrawal
* PR 17590: Merging PCM Playback and Mixing changes to
* Hydra_Software_Devel
* 
* Hydra_Software_Devel/Kapil_PCM_Playback/2   11/1/05 9:44a kagrawal
* Added eChannelType param in BRAP_RBUF_P_HWConfig
* 
* Hydra_Software_Devel/Kapil_PCM_Playback/1   10/24/05 5:06p kagrawal
* PR 17590: Initial check in for PCM Playback and Mixing
* 
* Hydra_Software_Devel/38   10/27/05 7:11p rjain
* PR 17796: To reduce timing, we dont clear Ring buffer memory on
* _Start() and _Stop() now.
* 
* Hydra_Software_Devel/37   10/17/05 2:17p rjain
* PR 17651: Making changes for 7411 D0
* 
* Hydra_Software_Devel/36   9/19/05 4:35p rjain
* PR 16899: Making changes to the FMM modules for watchdog recovery.
* 
* Hydra_Software_Devel/35   9/19/05 7:14p bmishra
* PR 16148: Removed chip specific code from interface files
* 
* Hydra_Software_Devel/34   9/13/05 7:42p bmishra
* PR 16148: Merged 7401 related changes
* 
* Hydra_Software_Devel/BLR_RAP_7401_Bringup/3   8/19/05 12:03p bmishra
* PR 16148: Merged latest changes from the main branch
* 
* Hydra_Software_Devel/33   8/2/05 11:06a rjain
* PR 16148: removing warning for unused variable
* 
* Hydra_Software_Devel/32   8/1/05 5:08p rjain
* PR 16148: Shifting ring buffer base address before
* programming into ring buffer base address register for 7401 also.
* 
* Hydra_Software_Devel/31   7/29/05 10:28p nitinb
* PR 16148: Shifting ring buffer addresses appropriately before
* programming into ring buffer registers
*  
* Hydra_Software_Devel/BLR_RAP_7401_Bringup/1   7/12/05 8:16p bmishra
* PR 16148: Modified the file for 7401 compatibility
* 
* Hydra_Software_Devel/29   7/11/05 6:24p rjain
* PR 16148: For chips other than 7411, converting virtual address of
* device memory to physical address before programming into
* hardware/firmware registers.
* 
* Hydra_Software_Devel/28   7/11/05 3:18p rjain
* PR 16171: Misc FMM PI changes
* 
* Hydra_Software_Devel/27   7/8/05 2:05p rjain
* PR 16148: Modifiying for 7401
* 
* Hydra_Software_Devel/26   6/27/05 6:08p bmishra
* PR 15976: Added Wrap Arround flag to functions for Get Ring buffer Rd
* and Wr pointers
* 
* Hydra_Software_Devel/25   6/27/05 1:44a bmishra
* PR 15976: Added Get Ring buffer pointers
* 
* Hydra_Software_Devel/24   6/22/05 9:55p bselva
* PR15887: Programmed the endaddress of Ring buffer at start time
* 
* Hydra_Software_Devel/23   6/10/05 3:26p rjain
* PR 15837: Moving RBUF memory allocation from FMM_P_Open() to
* RBUF_P_Open() => only memory for opened RBUFs will be allocated, not
* for all 12.
* 
* Hydra_Software_Devel/21   6/2/05 10:47a rjain
* PR 14143: Implemented FMM Code review comments.
* 
* Hydra_Software_Devel/20   5/23/05 3:52p bmishra
* PR 15496: Implemented a DRAM read followed by one or more DRAM writes,
* which is required to fix the memory coherency problem of Stream Arc
* 
* Hydra_Software_Devel/19   5/13/05 2:59p rjain
* PR 15271: To workaround current memory crunch for MPEG, temproraily
* allocating memory only for 9 ring buffers instead of 12.
* 
* Hydra_Software_Devel/18   5/4/05 11:09a rjain
* PR 14507:- Added proper States, open/start counts for modules.
* - Added appropriate checks to handle multiple calls to _Open(),
* _Start(), _Stop(), _Close() for modules.
* - Corrected SPDIFFM handle stored in hFmm / channel handle to be
* SPDIFFM Stream handle.
* - Some clean up, mroe comments/debug messages.
* - Added BRAP_RM_P_GetSpdifFmId() to RM
* 
* Hydra_Software_Devel/17   4/11/05 2:42p rjain
* PR 13082: modifiying debug msgs and module names
* 
* Hydra_Software_Devel/16   2/17/05 7:35p bmishra
* PR 14143: Removed calls to BMEM_ConvertAddressToOffset
* 
* Hydra_Software_Devel/15   2/16/05 1:45p rjain
* PR 14143: Implementing some comments from Phase 1 Code review done till
* date
* 
* Hydra_Software_Devel/14   2/9/05 2:11p rjain
* PR13082: Correcting Rbuf end address
* 
* Hydra_Software_Devel/13   2/4/05 6:29p rjain
* PR13082: correcting hrbuf typo
* 
* Hydra_Software_Devel/12   2/4/05 6:28p rjain
* PR13082: Fixing compilation warnings
* 
* Hydra_Software_Devel/11   2/3/05 3:27p rjain
* PR13082: Replacing calls to BKNI_Memset()  for device memory with
* BRAP_P_DRAMWRITE
* 
* Hydra_Software_Devel/10   2/3/05 2:46p rjain
* PR13082: Fixing compilation warnings
* 
* Hydra_Software_Devel/9   2/2/05 7:14p nitinb
* PR 13082: Fixing compilation warnings
* 
* Hydra_Software_Devel/8   2/2/05 9:59p bmishra
* PR13082: Fixed compilation error
* 
* Hydra_Software_Devel/7   2/2/05 5:34p rjain
* PR13082:Converted BREG_Read32 and BRAP_Read32 and BREG_Write32 to
* BRAP_Write32
* 
* Hydra_Software_Devel/6   2/2/05 4:49p rjain
* PR13082: Modifying according to settings/bugs highlighted in emulation
* 
* Hydra_Software_Devel/5   1/27/05 6:10p bmishra
* PR 13082: Fixed compilation errors
* 
* Hydra_Software_Devel/4   1/25/05 9:53a rjain
* PR13082: Modifying according to settings/bugs highlighted in emulation
* so far.
* 
* Hydra_Software_Devel/3   1/12/05 5:58p rjain
* PR13082: Converted BRAP_Read32 and BRAP_Write32 to BREG_XXX
* 
* Hydra_Software_Devel/2   1/12/05 5:03p rjain
* PR13082: Updating with changes after compilation of FMM, AM and RM
* together.
* 
* Hydra_Software_Devel/1   1/11/05 11:39a rjain
* PR 13082: Moved to new directory structure
* 
***************************************************************************/
#include "brap.h"
#include "brap_priv.h"

BDBG_MODULE (rap_rbuf);

static BERR_Code BRAP_RBUF_P_HWConfig (
    BRAP_RBUF_P_Handle  hRBuf  		/* [in] RBUF handle */
);

/* Default Settings and Parameters */
static const BRAP_RBUF_P_Settings defRbufSettings =
{
    {
       0,                                   /* sExtSettings.pBufferStart */
       BRAP_RBUF_P_DEFAULT_SIZE,            /* sExtSettings.uiSize */     
       BRAP_RBUF_P_DEFAULT_FREE_BYTE_MARK   /* sExtSettings.uiWaterMark */
    },
    false,                                   /* bProgRdWrRBufAddr */ 
    false                                   /* bRbufOfClonedPort*/
#if (BRAP_SECURE_HEAP==1)
	,false,						/* No secure memory by default */
#endif    
};
static const BRAP_RBUF_P_Params defRbufParams =
{
    BRAP_RBUF_P_DEFAULT_START_WR_POINT,   /* uiStartWRPoint */
    BRAP_RBUF_P_DEFAULT_SIZE
};



BERR_Code 
BRAP_RBUF_P_GetDefaultSettings ( 
    BRAP_RBUF_P_Settings   * pDefSettings  /* Pointer to memory where default
                                              settings should be written */
)
{
    BERR_Code ret = BERR_SUCCESS;
    BDBG_ENTER (BRAP_RBUF_P_GetDefaultSettings);

    BDBG_ASSERT (pDefSettings);            
    
    *pDefSettings = defRbufSettings;
    BDBG_LEAVE (BRAP_RBUF_P_GetDefaultSettings);
    return ret;
}

BERR_Code 
BRAP_RBUF_P_GetDefaultParams ( 
    BRAP_RBUF_P_Params    *pDefParams   /* Pointer to memory where default
                                           settings should be written */    
)
{
    BERR_Code ret = BERR_SUCCESS;
    BDBG_ENTER (BRAP_RBUF_P_GetDefaultParams);

    BDBG_ASSERT (pDefParams);            
    
    *pDefParams = defRbufParams;
    BDBG_LEAVE (BRAP_RBUF_P_GetDefaultParams);
    return ret;
}


BERR_Code BRAP_RBUF_P_Open (
    BRAP_FMM_P_Handle      		hFmm,       /* [in] FMM handle */
    BRAP_RBUF_P_Handle 	   		*phRBuf,    /* [out] Pointer to RBUF handle */
    unsigned int           		uiRbufIndex,/* [in] RBUF index */           
    const BRAP_RBUF_P_Settings 	*pSettings 	/* [in] Open time settings */
)
{

    BERR_Code ret = BERR_SUCCESS;
    BRAP_RBUF_P_Handle hRBuf;
    uint32_t ui32RegVal = 0;
    unsigned int i=0;
    uint32_t ui32DummyVar;
    uint32_t ui32PhyAddr =0;
	unsigned int uiSize = 0;
	unsigned int uiWaterMark = 0;
#if BCHP_7411_VER > BCHP_VER_C0
	BRAP_AudioMemAllocation eMemAllocation= BRAP_AudioMemAllocation_eStereo;
#endif
	
    BDBG_ENTER (BRAP_RBUF_P_Open);
    /* 1. Check all input parameters to the function. Return error if
     * - FMM handle is NULL
     * - Given index exceeds maximum no. of RBufs
     * - Pointer to Settings structure is NULL
     * - Pointer to RBuf handle is NULL     */
    BDBG_ASSERT (hFmm);
    BDBG_ASSERT (phRBuf);
    
    if ( uiRbufIndex > (BRAP_RM_P_MAX_RBUFS -1))
    {
        return BERR_TRACE (BERR_NOT_SUPPORTED);
    }      

    if (BRAP_FMM_P_GetWatchdogRecoveryFlag (hFmm) == false)
    {   /* If not in WatchDog recovery */   
        BDBG_ASSERT (pSettings);
    
        if (hFmm->hRBuf[uiRbufIndex] != NULL )
        {
            BDBG_ERR(("BRAP_RBUF_P_Open: RBUF %d is already open", uiRbufIndex));
            return BERR_TRACE(BERR_INVALID_PARAMETER);
        }
       
        BDBG_MSG(("BRAP_RBUF_P_Open: Given input parameters"
               "uiRbufIndex=%d, " 
               "pSettings->bProgRdWrRBufAddr=%d,"
               "pSettings->bRbufOfClonedPort=%d,"
               "pSettings->sExtSettings.pBufferStart=0x%x,"
               "pSettings->sExtSettings.uiSize=%d,"
               "pSettings->sExtSettings.uiWaterMark=%d ",
               uiRbufIndex,
               pSettings->bProgRdWrRBufAddr,
               pSettings->bRbufOfClonedPort,
               pSettings->sExtSettings.pBufferStart,
               pSettings->sExtSettings.uiSize,
               pSettings->sExtSettings.uiWaterMark)); 


        if (pSettings->sExtSettings.pBufferStart != NULL)
        { 
            /* Check Start address and size. Should be a multiple of 256 bytes */
            if ((((int)(pSettings->sExtSettings.pBufferStart)%BRAP_RBUF_P_ALIGNMENT) != 0) 
                || (((pSettings->sExtSettings.uiSize % BRAP_RBUF_P_ALIGNMENT)) != 0) )
            {
                BDBG_ERR (("BRAP_RBUF_P_Open: Ringbuffer start address or size is not properly aligned."));
                return BERR_TRACE (BERR_INVALID_PARAMETER);
            }  
        }

    
        /* 2. Allocate memory for the RBUF handle. Fill in parameters in the RBUF
           handle. */
    
        /* Allocate RBuf handle */
        hRBuf = (BRAP_RBUF_P_Handle) BKNI_Malloc (sizeof(BRAP_RBUF_P_Object));
        if (hRBuf == NULL)
        {
            return BERR_TRACE (BERR_OUT_OF_SYSTEM_MEMORY);
        }
        
        /* Clear the handle memory */
        BKNI_Memset (hRBuf, 0, sizeof(BRAP_RBUF_P_Object));

        /* Initialise known elements in RBUF handle */
        hRBuf->hChip = hFmm->hChip;
        hRBuf->hRegister = hFmm->hRegister;
        hRBuf->hInt = hFmm->hInt;
        hRBuf->hFmm = hFmm;
        hRBuf->uiIndex = uiRbufIndex;
        hRBuf->ui32Offset = (BCHP_AUD_FMM_BF_CTRL_RINGBUF_1_RDADDR  
                       - BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_RDADDR) 
                       * uiRbufIndex; 
		hRBuf->hHeap = hFmm->hHeap;
#if ( BRAP_DVD_FAMILY == 1) || ( BCHP_CHIP == 3563) || ( BRAP_7405_FAMILY == 1)		
		hRBuf->bAllocatedInternally=false;
#endif
#if (BRAP_SECURE_HEAP==1)	
	/* If BRAP_SECURE_HEAP is defined, RBUFS should be allocated from Secure 
	Memory Region for Decode channels, and normal region for PB and 
	CAP channels. */
	if ((pSettings->bSecureMemory == true) && (hFmm->hRap->sSettings.hSecureMemory != NULL))	
	{
		hRBuf->hHeap =  hFmm->hRap->sSettings.hSecureMemory;
	}
#endif

		/* If caller has supplied size and watermark, use them, else use default */
	   	uiSize = (pSettings->sExtSettings.uiSize == 0) ?  
				BRAP_RBUF_P_DEFAULT_SIZE : pSettings->sExtSettings.uiSize;
		uiWaterMark = (pSettings->sExtSettings.uiWaterMark == 0) ?
				BRAP_RBUF_P_DEFAULT_WATER_MARK : pSettings->sExtSettings.uiWaterMark;

#if BCHP_7411_VER > BCHP_VER_C0
	eMemAllocation = BRAP_P_GetMemoryAllocationType(hFmm->hRap);
	if (eMemAllocation==BRAP_AudioMemAllocation_eMultiChannelSrc)
	{
		uiSize = BRAP_RBUF_P_7411D_MULTICHANNEL_SRC_SIZE;
	}
#endif

        /* Fill up the RBUF Settings Structure */
        if (pSettings->sExtSettings.pBufferStart == NULL)
        {
            /* Use internally allocated RBUF memory */
            hRBuf->sSettings.sExtSettings.pBufferStart 
		            = (void *)BRAP_P_AllocAligned (hFmm->hRap, uiSize, 8, 0
#if (BRAP_SECURE_HEAP==1)
					,false
#endif												
					);
            if (hRBuf->sSettings.sExtSettings.pBufferStart == (void *) BRAP_P_INVALID_DRAM_ADDRESS)
            {
                /* Free the RBUF Handle memory*/
                BKNI_Free (hRBuf); 
                return BERR_TRACE (BERR_OUT_OF_DEVICE_MEMORY);
            }

	        for (i=0; i<uiSize; i=i+4)
	        {
	            BRAP_P_DRAMWRITE ((BARC_Handle) hRBuf->hRegister, 
	                ((unsigned int)(hRBuf->sSettings.sExtSettings.pBufferStart) + i), 
	                0);
	        }			
  
      	    /* Have a DRAM read following DRAM writes, 
  	        in order to fix the Stream Arc memory coherency issue. */
       	    ui32DummyVar = BRAP_P_DRAMREAD((BARC_Handle) hRBuf->hRegister, 
                (unsigned int)(hRBuf->sSettings.sExtSettings.pBufferStart));
#if ( BRAP_DVD_FAMILY == 1)  || ( BCHP_CHIP == 3563) || ( BRAP_7405_FAMILY == 1)		
			hRBuf->bAllocatedInternally=true;
#endif
			
            BDBG_MSG (("BRAP_RBUF_P_Open: Device memory starting at 0x%x reserved RBUF[%d]", 
                     (hRBuf->sSettings.sExtSettings.pBufferStart), uiRbufIndex));
       	}
        else 
        {
            /* External memory has been allocated for this RBUF */
            hRBuf->sSettings.sExtSettings.pBufferStart 
                   =  pSettings->sExtSettings.pBufferStart;
#if (  BRAP_DVD_FAMILY == 1) || ( BCHP_CHIP == 3563) || ( BRAP_7405_FAMILY == 1)			
			hRBuf->bAllocatedInternally=false;
#endif
            BDBG_MSG (("BRAP_RBUF_P_Open: For RBUF[%d] External RBUF memory is used.",
                        uiRbufIndex));
#if ( BCHP_CHIP == 3563)
			for (i=0; i<uiSize; i=i+4)
			{
				BRAP_P_DRAMWRITE ((BARC_Handle) hRBuf->hRegister, 
					((unsigned int)(hRBuf->sSettings.sExtSettings.pBufferStart) + i), 
					0);
			}
#endif
        }
		
        hRBuf->sSettings.bRbufOfClonedPort = pSettings->bRbufOfClonedPort;
		hRBuf->sSettings.sExtSettings.uiSize = uiSize;
        hRBuf->sSettings.sExtSettings.uiWaterMark = uiWaterMark;
        hRBuf->sSettings.bProgRdWrRBufAddr = pSettings->bProgRdWrRBufAddr;
    }   /* End: If not in WatchDog recovery */
    else
    {
        hRBuf = *phRBuf;
    }
    ret = BRAP_P_ConvertAddressToOffset (hRBuf->hHeap, 
            hRBuf->sSettings.sExtSettings.pBufferStart, &ui32PhyAddr);
    if (ret != BERR_SUCCESS)
    {
        BDBG_ERR(("BRAP_RBUF_P_Open: Unable to convert RBUF %d start address to physical address", uiRbufIndex));
        /* Free the RBUF Handle memory*/
        BKNI_Free (hRBuf);         
        return BERR_TRACE(ret);
    }

    BDBG_MSG (("BRAP_RBUF_P_Open: For RBUF[%d], StartAddr(virtual)=0x%x, StartAddr(Physical)=0x%x, Size(in bytes)=0x%x, WaterMark(percentage)=%d", 
                    uiRbufIndex, 
                    hRBuf->sSettings.sExtSettings.pBufferStart,
                    ui32PhyAddr,
                    hRBuf->sSettings.sExtSettings.uiSize, 
                    hRBuf->sSettings.sExtSettings.uiWaterMark));
    /* program the base address */
    /* Note: JWORD address has to be programmed into the BASEADDR register. 
     * so dont use BCHP_FIELD_DATA */
    BRAP_Write32 (hRBuf->hRegister,  
                 (BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_BASEADDR + hRBuf->ui32Offset), 
                  ui32PhyAddr);    
    
#if (BCHP_7411_VER == BCHP_VER_C0)  /* for 7411 C0 */   
    /* program the end address */
    ui32RegVal = (BCHP_FIELD_DATA (
                        AUD_FMM_BF_CTRL_RINGBUF_0_ENDADDR, 
                        SOURCE_RINGBUF_ENDADDR, 
                        ui32PhyAddr + hRBuf->sSettings.sExtSettings.uiSize )); 

    BDBG_MSG(("BRAP_RBUF_P_Open: End addr (physical)=0x%x", ui32RegVal));
    BRAP_Write32 (hRBuf->hRegister,  
                 (BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_ENDADDR + hRBuf->ui32Offset), 
                  ui32RegVal);
#else
    /* program the end address */
    ui32RegVal = (BCHP_FIELD_DATA (
                        AUD_FMM_BF_CTRL_RINGBUF_0_ENDADDR, 
                        RINGBUF_ENDADDR, 
                        (ui32PhyAddr + hRBuf->sSettings.sExtSettings.uiSize - 1))); 
    BDBG_MSG(("BRAP_RBUF_P_Open: End addr (physical)=0x%x", ui32RegVal));
    BRAP_Write32 (hRBuf->hRegister,  
                 (BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_ENDADDR + hRBuf->ui32Offset), 
                  ui32RegVal);
#endif

	/* Read - Write addresses are required in PCM after channel open. */
	/* Application may chose to write PCM data before calling a channel start */
	if (true == hRBuf->sSettings.bProgRdWrRBufAddr)
	{
#if (BCHP_7411_VER == BCHP_VER_C0)  /* for 7411 C0 */   
		ui32RegVal = (BCHP_FIELD_DATA (
	                  AUD_FMM_BF_CTRL_RINGBUF_0_RDADDR, 
	                  SRING_RDADDR, 
	                  ui32PhyAddr));       
		BRAP_Write32 (hRBuf->hRegister,  
	                  BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_RDADDR + hRBuf->ui32Offset, 
	                  ui32RegVal);


		ui32RegVal = (BCHP_FIELD_DATA (
	                  AUD_FMM_BF_CTRL_RINGBUF_0_WRADDR, 
	                  SOURCE_RING_WRADDR, 
	                  ui32PhyAddr));       
		
	    BRAP_Write32 (hRBuf->hRegister,  
	                  BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_WRADDR + hRBuf->ui32Offset, 
	                  ui32RegVal);
#else /* 7401 & 7411 D0 & 7400 & 7118*/
    	ui32RegVal = (BCHP_FIELD_DATA (
	                  AUD_FMM_BF_CTRL_RINGBUF_0_RDADDR, 
	                  RINGBUF_RDADDR, 
	                  ui32PhyAddr));       
		BRAP_Write32 (hRBuf->hRegister,  
	                  BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_RDADDR + hRBuf->ui32Offset, 
	                  ui32RegVal);


		ui32RegVal = (BCHP_FIELD_DATA (
	                  AUD_FMM_BF_CTRL_RINGBUF_0_WRADDR, 
	                  RINGBUF_WRADDR, 
	                  ui32PhyAddr));       
		
	    BRAP_Write32 (hRBuf->hRegister,  
	                  BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_WRADDR + hRBuf->ui32Offset, 
	                  ui32RegVal);
#endif
	}
   
    if (BRAP_FMM_P_GetWatchdogRecoveryFlag (hFmm) == false)
    {   /* If not in WatchDog recovery */  
        
        /*4. Store Ring buffer handle inside the FMM handle */
        hFmm->hRBuf[uiRbufIndex] = hRBuf;
    
        *phRBuf = hRBuf;
    }     
    
    BDBG_MSG (("BRAP_RBUF_P_Open: New RBUF Handle = 0x%x", hRBuf));
    
    BDBG_LEAVE (BRAP_RBUF_P_Open);
    return ret;
}


BERR_Code BRAP_RBUF_P_Start (
    BRAP_RBUF_P_Handle   hRBuf,          /* [in] RBUF handle */
    const BRAP_RBUF_P_Params * pParams   /* [in] Start time settings */  
)
{
    BERR_Code ret = BERR_SUCCESS;

    BDBG_ENTER (BRAP_RBUF_P_Start);
    
    BDBG_ASSERT (hRBuf);
   
    if (BRAP_FMM_P_GetWatchdogRecoveryFlag (hRBuf->hFmm) == false)
    {   /* If not in WatchDog recovery */  
        
        BDBG_ASSERT (pParams);
    
        BDBG_MSG (("BRAP_RBUF_P_Start:"
               "hRBuf=0x%x, RBuf index=%d," 
               "pParams.uiStartWRPoint=%d",
                hRBuf, hRBuf->uiIndex, pParams->uiStartWRPoint));
    
        /* Store the start parameters inside the handle */
        hRBuf->sParams = *pParams; 
    }
   
    /* Configure RBuf Hardware */
    ret = BRAP_RBUF_P_HWConfig (hRBuf);

    BDBG_LEAVE (BRAP_RBUF_P_Start);
    return ret;
}

BERR_Code BRAP_RBUF_P_Stop (
    BRAP_RBUF_P_Handle    hRBuf     /* [in] RBUF handle */
)
{
    BERR_Code ret = BERR_SUCCESS;

    BDBG_ENTER (BRAP_RBUF_P_Stop);
    BDBG_ASSERT (hRBuf);
    BSTD_UNUSED (hRBuf);
    
    BDBG_MSG (("BRAP_RBUF_P_Stop:"
               "hRBuf=0x%x, RBuf index=%d ",
                hRBuf, hRBuf->uiIndex));

    BDBG_LEAVE (BRAP_RBUF_P_Stop);
    return ret;
}

BERR_Code BRAP_RBUF_P_Close ( 
    BRAP_RBUF_P_Handle      hRBuf   /* [in] RBUF handle */
)
{
    BERR_Code ret = BERR_SUCCESS;

    BDBG_ENTER (BRAP_RBUF_P_Close);
    BDBG_ASSERT (hRBuf);
    
    BDBG_MSG (("BRAP_RBUF_P_Close:"
               "hRBuf=0x%x, RBuf index=%d ",
                hRBuf, hRBuf->uiIndex));
    
    /* Remove referrence to this Rbuf from the parent FMM */ 
    hRBuf->hFmm->hRBuf[hRBuf->uiIndex] = NULL;

    /* For cloned port Rbuf does not have any memory allocated to it.*/
    if (hRBuf->sSettings.bRbufOfClonedPort == false)
    {
	    /* Free the ring buffer memory */
#if (BRAP_SECURE_HEAP==1)
		BRAP_P_Free(hRBuf->hFmm->hRap, (void *) hRBuf->sSettings.sExtSettings.pBufferStart,true);
#else
#if (  BRAP_DVD_FAMILY == 1) || ( BCHP_CHIP == 3563) || ( BRAP_7405_FAMILY == 1)		
		if(true == hRBuf->bAllocatedInternally) 
		{
			BDBG_MSG(("Freeing the Mem %#x",hRBuf->sSettings.sExtSettings.pBufferStart));
	    	BRAP_P_Free(hRBuf->hFmm->hRap, (void *) hRBuf->sSettings.sExtSettings.pBufferStart);
		}
		else
			BDBG_MSG(("Not Freeing the Mem.%#x allocated externally",hRBuf->sSettings.sExtSettings.pBufferStart));
#else	
	   	BRAP_P_Free(hRBuf->hFmm->hRap, (void *) hRBuf->sSettings.sExtSettings.pBufferStart);
#endif		
#endif

    }
	
    /* Free the RBUF Handle memory*/
    BKNI_Free (hRBuf); 
                 
    BDBG_LEAVE (BRAP_RBUF_P_Close);
    return ret;
}


/***************************************************************************
Summary:
    Configures the HW registers for the Ring Buffer

Description:


Returns:
    BERR_SUCCESS on success
    Error code on failure

See Also:
    None.
**************************************************************************/
static BERR_Code BRAP_RBUF_P_HWConfig (
    BRAP_RBUF_P_Handle  hRBuf  		/* [in] RBUF handle */
)
{
    BERR_Code ret = BERR_SUCCESS;
    uint32_t  ui32RegVal = 0;
    uint32_t  ui32BaseAddr; /* Physical address of RBUF base */
    BREG_Handle hRegister;
    uint32_t   ui32WaterMark;

#if (BCHP_7411_VER == BCHP_VER_C0)  /* for 7411 C0 */   
    BSTD_UNUSED(ui32WaterMark);
#endif

    BDBG_ENTER (BRAP_RBUF_P_HWConfig);
    BDBG_ASSERT (hRBuf);
    
    BDBG_MSG (("BRAP_RBUF_P_HWConfig:"
               "hRBuf=0x%x, RBuf index=%d ",
                hRBuf, hRBuf->uiIndex));
   
    hRegister = hRBuf->hRegister; 
    
    ui32BaseAddr = BRAP_Read32 (hRegister,
                            BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_BASEADDR + 
                            hRBuf->ui32Offset);
#if (BCHP_7411_VER == BCHP_VER_C0)  /* for 7411 C0 */   

    /* Set RDADDR and WRADDR equal to BASEADDR Only for a decode channel */ 
    ui32RegVal = (BCHP_FIELD_DATA (
                        AUD_FMM_BF_CTRL_RINGBUF_0_RDADDR, 
                        SRING_RDADDR, 
                        ui32BaseAddr));       
   	BRAP_Write32 (hRegister,  
                  BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_RDADDR + hRBuf->ui32Offset, 
                  ui32RegVal);

	    ui32RegVal = (BCHP_FIELD_DATA (
                        AUD_FMM_BF_CTRL_RINGBUF_0_WRADDR, 
                        SOURCE_RING_WRADDR, 
                        ui32BaseAddr));       
    	BRAP_Write32 (hRegister,  
                  BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_WRADDR + hRBuf->ui32Offset, 
                  ui32RegVal);
	
    /* Workaround for PR15887: program the end address */
	 ui32RegVal = (BCHP_FIELD_DATA (
                        AUD_FMM_BF_CTRL_RINGBUF_0_ENDADDR, 
                        SOURCE_RINGBUF_ENDADDR, 
                        ui32BaseAddr + hRBuf->sSettings.sExtSettings.uiSize )); 
    BDBG_MSG(("BRAP_RBUF_P_HWConfig: Base addr (physical)=0x%x End addr (physical)=0x%x", ui32BaseAddr, ui32RegVal));

    BRAP_Write32 (hRBuf->hRegister,  
                 (BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_ENDADDR + hRBuf->ui32Offset), 
                  ui32RegVal);
    
    /* Configure water mark : note:uiWaterMark is %  */
    ui32RegVal = (BCHP_FIELD_DATA (
                        AUD_FMM_BF_CTRL_RINGBUF_0_FREEBYTE_MARK, 
                        SOURCE_RINGBUF_FREEBYTE_MARK, 
                        (hRBuf->sSettings.sExtSettings.uiWaterMark
                         * hRBuf->sSettings.sExtSettings.uiSize)/100));       
    BRAP_Write32 (hRegister,  
                 BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_FREEBYTE_MARK + hRBuf->ui32Offset , 
                 ui32RegVal);

    ui32RegVal = (BCHP_FIELD_DATA (
                        AUD_FMM_BF_CTRL_RINGBUF_0_START_WRPOINT, 
                        SOURCE_RINGBUF_START_WRPOINT, 
                        (uint32_t)(hRBuf->sParams.uiStartWRPoint)
                        + ui32BaseAddr));       
	
    BRAP_Write32 (hRegister,  
                   (BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_START_WRPOINT 
                   + hRBuf->ui32Offset), 
                  ui32RegVal);
#else
    /* for 7401,7411 D0 etc*/
    ui32RegVal = (BCHP_FIELD_DATA (
                        AUD_FMM_BF_CTRL_RINGBUF_0_RDADDR, 
                        RINGBUF_RDADDR, 
                        ui32BaseAddr));       
   	BRAP_Write32 (hRegister,  
                  BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_RDADDR + hRBuf->ui32Offset, 
                  ui32RegVal);

	    ui32RegVal = (BCHP_FIELD_DATA (
                        AUD_FMM_BF_CTRL_RINGBUF_0_WRADDR, 
                        RINGBUF_WRADDR, 
                        ui32BaseAddr));       
	    BRAP_Write32 (hRegister,  
                  BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_WRADDR + hRBuf->ui32Offset, 
                  ui32RegVal);

    /* Workaround for PR15887: program the end address */
    ui32RegVal = (BCHP_FIELD_DATA (
                        AUD_FMM_BF_CTRL_RINGBUF_0_ENDADDR, 
                        RINGBUF_ENDADDR, 
                        ui32BaseAddr + hRBuf->sSettings.sExtSettings.uiSize - 1)); 
    BDBG_MSG(("BRAP_RBUF_P_HWConfig: Base addr (physical)=0x%x End addr (physical)=0x%x", ui32BaseAddr, ui32RegVal));

    /* Calculate the Water Mark Level depending on the type of the System 
       If Edge Triggered system, the application provided Water Mark is Ignored 
       and internallyforced to be (100-FrameSize)%, where FrameSize is also in 
       percentage. Currently the FrameSize used is 25%.
       If Level Triggered System, the user provided Water Mark is Used.*/
#if 0       
#if (BRAP_P_EDGE_TRIG_INTRPT == 0)
    BRAP_RBUF_P_GetFrameSize(hRBuf->sSettings.sExtSettings.uiSize, &uiFrameSize);
    ui32WaterMark = hRBuf->sSettings.sExtSettings.uiSize - uiFrameSize + 1;
#else
#endif
#endif
    ui32WaterMark = (hRBuf->sSettings.sExtSettings.uiWaterMark
                         * hRBuf->sSettings.sExtSettings.uiSize)/100;


    BRAP_Write32 (hRBuf->hRegister,  
                 (BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_ENDADDR + hRBuf->ui32Offset), 
                  ui32RegVal);
  
    /* Configure water mark : note:uiWaterMark is %  */
    ui32RegVal = (BCHP_FIELD_DATA (
                        AUD_FMM_BF_CTRL_RINGBUF_0_FREEFULL_MARK, 
                        RINGBUF_FREEFULL_MARK,
                        ui32WaterMark));
                            
    BRAP_Write32 (hRegister,  
                 BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_FREEFULL_MARK + hRBuf->ui32Offset , 
                  ui32RegVal);

    ui32RegVal = (BCHP_FIELD_DATA (
                        AUD_FMM_BF_CTRL_RINGBUF_0_START_WRPOINT, 
                        RINGBUF_START_WRPOINT, 
                        (uint32_t)(hRBuf->sParams.uiStartWRPoint)
                          + ui32BaseAddr));       

    BRAP_Write32 (hRegister,  
                   (BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_START_WRPOINT 
                   + hRBuf->ui32Offset), 
                  ui32RegVal);    
      
#endif 
    BDBG_LEAVE (BRAP_RBUF_P_HWConfig);
    return ret;
}

/***************************************************************************
Summary:
	Gets the base and end pointers of the ring buffer.

Description:
	Gets the base and end pointers of the ring buffer associated with an 
	output audio channel for a RAP Channel. This is a debug API.

Returns:
	BERR_SUCCESS 

See Also:
	

****************************************************************************/
BERR_Code 
BRAP_RBUF_GetBaseAndEndAddress( 
            BRAP_ChannelHandle hRapCh,        /* [in] Audio Device handle */
            BRAP_OutputChannel eOpCh,    /* [in] Output channel type */
            uint32_t         *pBaseAddr,      /* [Out] Ring buffer base address */   
            uint32_t         *pEndAddr        /* [Out] Ring buffer end address */ 
            )
{

#if (BCHP_7411_VER != BCHP_VER_C0) 
	BSTD_UNUSED(hRapCh);
	BSTD_UNUSED(eOpCh);
	BSTD_UNUSED(pBaseAddr);
	BSTD_UNUSED(pEndAddr);
	return BERR_TRACE (BERR_NOT_SUPPORTED);
#else

    BRAP_RBUF_P_Handle    hRBuf;

	BDBG_ENTER (BRAP_RBUF_GetReadPointer);
    BDBG_ASSERT (hRapCh);
	BDBG_ASSERT (pBaseAddr);
	BDBG_ASSERT (pEndAddr);

    /* Get the appropriate ring buffer handle */
    hRBuf = hRapCh->sModuleHandles.hRBuf[eOpCh];

    /* Assert on hRBuf */
	BDBG_ASSERT (hRBuf);

    /* Read the base and end addresses from the registers */
    *pBaseAddr = BRAP_Read32(hRBuf->hRegister, 
                            BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_BASEADDR + 
                            hRBuf->ui32Offset);
	*pEndAddr = BRAP_Read32(hRBuf->hRegister, 
                            BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_ENDADDR + 
                            hRBuf->ui32Offset);
	
    BDBG_LEAVE(BRAP_RBUF_GetBaseAndEndAddress);

    return BERR_SUCCESS;    
#endif /* 7401 etc*/
}

BERR_Code BRAP_RBUF_P_GetFrameSize(
    unsigned int uiRBufSize, 
    unsigned int *pFrameSize
    )
{
    BDBG_ASSERT(pFrameSize);

    /* Each Ring buffer is logically divided into 4 frames. 
       This is also the max free contiguous ring buffer size 
       in bytes that is informed to the user */       
    *pFrameSize = uiRBufSize >> 2;
    
    return BERR_SUCCESS;
}

/***************************************************************************
Summary:
    Configures the End address of the the Ring Buffer

Description:


Returns:
    BERR_SUCCESS on success
    Error code on failure

See Also:
    None.
**************************************************************************/
BERR_Code BRAP_RBUF_P_ProgramEndAddress (
    BRAP_RBUF_P_Handle  hRBuf  		/* [in] RBUF handle */
)
{
	BERR_Code ret = BERR_SUCCESS;
	uint32_t  ui32RegVal = 0;
	uint32_t  ui32BaseAddr; /* Physical address of RBUF base */	
	BREG_Handle hRegister;
	uint32_t   ui32WaterMark;

	BDBG_ENTER (BRAP_RBUF_P_ProgramEndAddress);
	BDBG_ASSERT (hRBuf);

	hRegister = hRBuf->hRegister; 

	ui32BaseAddr = BRAP_Read32 (hRegister,
					BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_BASEADDR + 
					hRBuf->ui32Offset);

#if (BCHP_7411_VER == BCHP_VER_C0)  /* for 7411 C0 */   
	ui32RegVal = (BCHP_FIELD_DATA (
					AUD_FMM_BF_CTRL_RINGBUF_0_ENDADDR, 
					SOURCE_RINGBUF_ENDADDR, 
					ui32BaseAddr + hRBuf->sParams.uiStartSize)); 

	BDBG_MSG(("BRAP_RBUF_P_HWConfig: Base addr (physical)=0x%x End addr (physical)=0x%x", ui32BaseAddr, ui32RegVal));

	BRAP_Write32 (hRBuf->hRegister,  
		(BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_ENDADDR + hRBuf->ui32Offset), 
		ui32RegVal);
    
	/* Configure water mark : note:uiWaterMark is %  */
	ui32RegVal = (BCHP_FIELD_DATA (
					AUD_FMM_BF_CTRL_RINGBUF_0_FREEBYTE_MARK, 
					SOURCE_RINGBUF_FREEBYTE_MARK, 
					(hRBuf->sSettings.sExtSettings.uiWaterMark
					* hRBuf->sParams.uiStartSize)/100));       

	BRAP_Write32 (hRegister,  
		BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_FREEBYTE_MARK + hRBuf->ui32Offset , 
		ui32RegVal);
#else    /* for 7401,7411 D0 etc*/
	ui32RegVal = (BCHP_FIELD_DATA (
	                    AUD_FMM_BF_CTRL_RINGBUF_0_ENDADDR, 
	                    RINGBUF_ENDADDR, 
	                    ui32BaseAddr +  hRBuf->sParams.uiStartSize - 1)); 
	BDBG_MSG(("BRAP_RBUF_P_HWConfig: Base addr (physical)=0x%x End addr (physical)=0x%x", ui32BaseAddr, ui32RegVal));

	ui32WaterMark = (hRBuf->sSettings.sExtSettings.uiWaterMark
	                     * hRBuf->sParams.uiStartSize)/100;


	BRAP_Write32 (hRBuf->hRegister,  
	 			(BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_ENDADDR + hRBuf->ui32Offset), 
	 			ui32RegVal);
  
	/* Configure water mark : note:uiWaterMark is %  */
	ui32RegVal = (BCHP_FIELD_DATA (
	                    AUD_FMM_BF_CTRL_RINGBUF_0_FREEFULL_MARK, 
	                    RINGBUF_FREEFULL_MARK,
	                    ui32WaterMark));
                            
	BRAP_Write32 (hRegister,  
	             BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_FREEFULL_MARK + hRBuf->ui32Offset , 
	              ui32RegVal);
#endif

	BDBG_LEAVE (BRAP_RBUF_P_ProgramEndAddress);
	return ret;

}

/* End of File */
