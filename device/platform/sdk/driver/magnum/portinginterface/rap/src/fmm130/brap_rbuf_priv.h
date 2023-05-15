/***************************************************************************
*     Copyright (c) 2004-2008, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: brap_rbuf_priv.h $
* $brcm_Revision: Hydra_Software_Devel/39 $
* $brcm_Date: 11/12/08 7:32p $
*
* Module Description:
*   Module name: RBUF
*   This file lists all data structures, macros, enumerations and function 
*   prototypes for the RingBuffer abstraction, which are internal ie NOT
*   exposed to the application developer. These can be used only by the 
*   Audio Manager and other FMM submodules.
*
* Revision History:
*
* $brcm_Log: /magnum/portinginterface/rap/base/brap_rbuf_priv.h $
* 
* Hydra_Software_Devel/39   11/12/08 7:32p srajapur
* PR48893 : 74XX merged release for 7400,7401,7403 and 7118 [For more
* info see Description]
* PR45669: [7403] Adding support for DTS Certification.
* PR43467 : [7403] Implementation of DRA on 7403
* PR46472 : [7400] Implement Dolby Volume Leveler and Modeler per
* Motorola's Requirements
* PR42285 : [7401] Added Audio descriptor support
* PR44899 :[7401] Independent delay on spdif causes left and right
* channel to be out of phase
* PR44614 : [7400] [7401C1][NDS-CDI] Few seconds of noise on SPDIF/HDMI
* during PCM to Compressed & vice-a-versa transition
* PR42342: [7401] Added Downmix Method parameter in the AACHE user config
* structure for Sumitomo
* 
* Hydra_Software_Devel/38   9/9/08 7:16p sushmit
* PR 43454: [7405]Updating RBUF Sizes.
* 
* Hydra_Software_Devel/37   8/18/08 3:49p gdata
* PR43971: [7601] Fixing compilation error for 7601
* 
* Hydra_Software_Devel/36   7/21/08 12:50p speter
* PR 44984: [3548,3556,7405] Merging the changes from Rap_NewFwArch
* branch to Hydra_Software_Devel
* 
* Hydra_Software_Devel/Rap_NewFwArch/4   6/13/08 10:52p speter
* PR 38950: [3548,3556] Fixing memory corruption problem due to incorrect
* freeing of rbuf. Since 3548 uses static memory for rbuf it should not
* free them here
* 
* Hydra_Software_Devel/Rap_NewFwArch/3   5/5/08 11:22a speter
* PR 38950: [3548] Making 3548_FAMILY macro and using it for both 3548
* and 3556 cases.
* 
* Hydra_Software_Devel/Rap_NewFwArch/2   4/28/08 4:47p speter
* PR 38950: [3548] Fixing compilation for 3548/3556.
* 
* Hydra_Software_Devel/Rap_NewFwArch/1   4/8/08 10:10a nitinb
* PR 39989: [7405] Added support for DDP pass through
* 
* Hydra_Software_Devel/31   10/11/07 4:03p bselva
* PR35803:[7401,7118,7403,7400] The handling of Rbufsize given during
* Playback start is corrected in PI so that bLoopAroundEn flag is
* honoured properly.
* 
* Hydra_Software_Devel/30   9/30/07 2:00p sushmit
* PR35140: [7405] Develop 7325 Raptor PI for Bringup
* 
* Hydra_Software_Devel/29   9/19/07 11:34a sushmit
* PR 34355: [7405] Increasing Default RBUF Size
* PR 34969: [7405] Increasing Default RBUF Size
* 
* Hydra_Software_Devel/28   5/11/07 9:02p sushmit
* PR 27646: Getting 7405 PI compiled
* 
* Hydra_Software_Devel/27   4/17/07 4:41p nitinb
* PR27912 : [3563] Fixed allocation/freeing of ring buffers from pool of
* buffers for init time allocation
* 
* Hydra_Software_Devel/26   4/16/07 5:15p gautamk
* PR28472: [7401,7400] Use #if BRAP_SECURE_HEAP for the changes to
* BRAP_P_AllocAligned(); BRAP_P_Free() and other securtiy related
* changes
* 
* Hydra_Software_Devel/25   4/16/07 4:44p kagrawal
* PR 28561: [7440] Changing default RBuf size as per the worst case
* requirement for DTS-LBR
* 
* Hydra_Software_Devel/24   4/12/07 2:10p bselva
* PR 27768:[7400 b0]Increasing the rbuf size as per the new requirement
* for MPEG decoders
* 
* Hydra_Software_Devel/23   3/16/07 5:11p gautamk
* PR27912 :[7440]  Implementaion of Open time memory allocation for Ring
* Buffers
* 
* Hydra_Software_Devel/22   2/28/07 9:07a rjain
* PR 24247: Porting LSF (with SRC) for 7118 and 7403
* 
* Hydra_Software_Devel/21   2/15/07 11:32a rjain
* PR 24247: [7401] Increasing the default RBUF size for 7401 to 5 frames
* for LSF support.
* 
* Hydra_Software_Devel/20   9/27/06 5:50p rjain
* PR 22087: PI changes for independent output delay
* 
* Hydra_Software_Devel/19   9/7/06 11:33a rjain
* PR 22868: changing SECURE_HEAP to BRAP_SECURE_HEAP
* 
* Hydra_Software_Devel/18   8/31/06 12:31p rjain
* PR 22868: Moving the hSecureMem handle into BRAP_Settings()
* 
* Hydra_Software_Devel/17   5/25/06 7:54p nitinb
* PR 21794: Implement audio SRC for 7411D0
* 
* Hydra_Software_Devel/16   5/12/06 6:31p bhanus
* PR 21388 : Checking in PCM Playback related changes.
* 
* Hydra_Software_Devel/15   5/4/06 3:03p bhanus
* PR 21388 : Checking in Add/Remove Output Port related changes for 7400.
* 
* Hydra_Software_Devel/14   3/30/06 2:27p kagrawal
* PR 20318: Implemented reveiw comments on Audio Manager
* 
* Hydra_Software_Devel/13   3/1/06 9:39a kagrawal
* PR 18264: Implemented review comments for PCM playback
* 
* Hydra_Software_Devel/12   2/17/06 9:55a sushmit
* PR18604: Merging Encoder to latest.
* 
* Hydra_Software_Devel/RAP_AUD_ENC/2   2/6/06 4:17p sushmit
* PR18604: Merging latest Decoder PI to Encoder PI branch
* 
* Hydra_Software_Devel/11   1/23/06 4:28p kagrawal
* PR 19023: 1. Setting write pointer to the ring buffer start address
* during rbuf start irrespective of the channel type. This will ensure
* that starting of pcm playback 2nd time, there is no leftover from
* previous playback
* 2. Removed eChannelType passed to BRAP_RBUF_P_Start
* 
* Hydra_Software_Devel/10   1/19/06 2:10p kagrawal
* PR 18264: 1. Rearchitected SRC to accomodate capture channel
* 2. Implemented review comments
* 
* Hydra_Software_Devel/9   11/10/05 11:54a kagrawal
* PR 17590: Merging PCM Playback and Mixing changes to
* Hydra_Software_Devel
* 
* Hydra_Software_Devel/Kapil_PCM_Playback/2   11/1/05 9:44a kagrawal
* Review rework
* 
* Hydra_Software_Devel/Kapil_PCM_Playback/1   10/24/05 5:03p kagrawal
* PR 17590: Initial check in for PCM Playback and Mixing
* 
* Hydra_Software_Devel/8   10/2/05 11:28p nitinb
* PR 17415: Add support for advance codec for Raptor PI. Making default
* Rbuf size to 2048 * 3 *4  (size required for AACHE)
* 
* Hydra_Software_Devel/6   6/2/05 10:52a rjain
* PR 14143: Implemented FMM Code review comments.
* 
* Hydra_Software_Devel/5   3/16/05 3:14p nitinb
* PR 13082: Correcting default ring buffer size to 1536 * 4 * 3 (3 frames
* of AC3)
* 
* Hydra_Software_Devel/4   3/10/05 3:25p rjain
* PR 13082:
* - Implemented the new Start-Stop scheme where Hosts prgms all
* registers. DSP uses Start_WRPoint and RBUF_EndAddress to start decode.
* - removed bMixingEnable from BRAP_MIXER_Params
* - removed sRBufParams from BRAP_DEC_AudioParams.
* - Added BRAP_RM_P_GetDpStreamId
* - Corrected programming of Stream Id in DP registers
* - Added BRAP_SRCCH_P_WaitForRampDown()
* - Other minor changes
* 
* Hydra_Software_Devel/3   3/2/05 9:48p nitinb
* PR13082: Setting ring buffer default size to multiple of AC3 framesize
* 
* Hydra_Software_Devel/2   1/25/05 9:54a rjain
* PR13082: Modifying according to settings/bugs highlighted in emulation
* so far.
* 
* Hydra_Software_Devel/1   1/11/05 11:40a rjain
* PR 13082: Moved to new directory structure
* 
***************************************************************************/


#ifndef _BRAP_RBUF_PRIV_H_
#define _BRAP_RBUF_PRIV_H_

#include "brap_priv.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Free/Full byte mark percentage*/    
#define BRAP_RBUF_P_DEFAULT_WATER_MARK        100   
/* Free byte mark percentage: used for Src RBUFs */
#define BRAP_RBUF_P_DEFAULT_FREE_BYTE_MARK    BRAP_RBUF_P_DEFAULT_WATER_MARK 
/* Full byte mark percentage: used for DEST RBUFs */    
#define BRAP_RBUF_P_DEFAULT_FULL_BYTE_MARK    (100 - BRAP_RBUF_P_DEFAULT_WATER_MARK)  
/* Start WR Point percentage */     
/* TODO: START_WR_POINT = END_ADDR only for Decode channel. Will be different for
   Playback & Capture channels */
#define BRAP_RBUF_P_DEFAULT_START_WR_POINT    BRAP_RBUF_P_DEFAULT_SIZE 

/* Default size for RBUFs (without delay) ie these RBUFS cannot support independent output port delay*/
#if (BRAP_3548_FAMILY == 1) || (BRAP_7401_FAMILY == 1) || (BCHP_CHIP == 7400 && BCHP_VER !=BCHP_VER_A0) || (BRAP_7405_FAMILY == 1)
/* For LSF support, 7401 requires default RBUF size to be 5 frames. Keep 
default for all other chips as 3 frames so as to not increase memory consumption*/
/* As of now, Scheduler assumes these many buffers for algos as given below, 
#define MAX_FRAME_BUF_AC3                4   
#define MAX_FRAME_BUF_DDP                4   
#define MAX_FRAME_BUF_AAC                6   
#define MAX_FRAME_BUF_AAC_HE          3   
#define MAX_FRAME_BUF_MPEG_L1        9   
#define MAX_FRAME_BUF_MPEG_L2        6   
#define MAX_FRAME_BUF_MPEG_L3        6   
#define MAX_FRAME_BUF_WMA               5   
#define MAX_FRAME_BUF_WMA_PRO      5   
 Based in this info, make 7405 also use 5 frames so that we can support all algos */

#if (BCHP_CHIP == 7400)
/* DDP passthrough is the worst case so far*/
#define BRAP_RBUF_P_DEFAULT_SIZE              (98304)        
#elif (BRAP_7401_FAMILY == 1)
#define BRAP_RBUF_P_DEFAULT_SIZE              (73728 ) /* old (4096*4*4)    */
#else
#if ((BRAP_7405_FAMILY == 1)||(BRAP_3548_FAMILY == 1))
/* NEWFWARCH */
#define BRAP_RBUF_P_DEFAULT_SIZE    BRAP_AF_P_NON_DELAY_RBUF_SIZE
#define BRAP_P_HBR_RBUF_SIZE        BRAP_AF_P_NON_DELAY_RBUF_SIZE * 4
#else
#define BRAP_RBUF_P_DEFAULT_SIZE              (2048*5*4)
#define BRAP_P_HBR_RBUF_SIZE                        (2048*5*4*4)
#endif
#endif

#elif ( BRAP_DVD_FAMILY == 1)
/* DTS-LBR is the worst case so far */
#define BRAP_RBUF_P_DEFAULT_SIZE              (4096*3*4)
#else
#define BRAP_RBUF_P_DEFAULT_SIZE              (2048*3*4) 
#endif

/* Default size for RBUFs to be used for independent output port delay */
#if ((BCHP_CHIP == 7400) || (BRAP_7401_FAMILY == 1))
#define BRAP_RBUF_P_DEFAULT_DELAY_RBUF_SIZE              (147456) /* old (2048*17*4) */
#else
#if ((BRAP_7405_FAMILY == 1)||(BRAP_3548_FAMILY == 1))
/* NEWFWARCH */
#define BRAP_RBUF_P_DEFAULT_DELAY_RBUF_SIZE     BRAP_AF_P_DELAY_RBUF_SIZE 
#else
#define BRAP_RBUF_P_DEFAULT_DELAY_RBUF_SIZE              (2048*17*4) 
#endif
#endif

#define BRAP_RBUF_P_7411D_MULTICHANNEL_SRC_SIZE	(BRAP_RBUF_P_DEFAULT_SIZE * 8)
/* No delay - 0 */
#define BRAP_RBUF_P_NO_DELAY                  0
#define BRAP_RBUF_P_ALIGNMENT                 256

  
/***************************************************************************
Summary:
    Parameters to be passed by Audio Manager on Opening the Rbuf.

Description:
    This lists all the parameters required to open the Ring Buffer, 
    including both internal and external parameters.
    Note that the internal parameters are not exposed to the application.
    Currently no extra private settings are required.

See Also:

***************************************************************************/
typedef struct BRAP_RBUF_P_Settings
{
    BRAP_RBUF_Settings sExtSettings; /* Parameters provided by the application */
    bool               bProgRdWrRBufAddr; 
                                     /* TRUE: If Read-Write pointers to be 
                                        programmed during channel open.
                                        Required for PCM PB and CAP channels.
                                        FALSE: Other wise. */
    bool bRbufOfClonedPort;         /*  Program this to true for Add output
                                        ports in 7400 across the DP's. This makes
                                        sure that ring buffers are allocated for 
                                        the cloned port but no memory is allocated 
                                        to them.*/

#if (BRAP_SECURE_HEAP==1)
	bool 			bSecureMemory;		/* TRUE: RBUFs are to be allocated from Secure region.
										    FALSE: RBufs are to be allocated from normal memory */
#endif    	
										
} BRAP_RBUF_P_Settings;


/***************************************************************************
Summary:
    Parameters to be passed by Audio Manager on Starting the Ring Buffer.
    
Description:
    This lists all the parameters required to start the Ring Buffer, 
    including both internal and external parameters.
    Note that the internal parameters are not exposed to the application.
    Currently no extra private Parameters are required for the RBUF.

See Also:
    BRAP_FMM_Params

***************************************************************************/
typedef struct BRAP_RBUF_P_Params
{
    unsigned int uiStartWRPoint;
    unsigned int uiStartSize;
}BRAP_RBUF_P_Params; 



/***************************************************************************
Summary:
    Abstraction of a Ring Buffer 
    
Description:
    It contains all the information required to handle the RBUF.
    Particularly, it stores the RBUF index, handles for all required chip 
    related information, parent FMM handle, offset required to access 
    different RBUFs etc

See Also:
***************************************************************************/
typedef struct BRAP_RBUF_P_Object
{

    BRAP_RBUF_P_Settings sSettings; /* Ring buffer settings provided during
                                       Open() */

    BRAP_RBUF_P_Params   sParams;   /* Ring buffer settings for 
                                       the current instance provided 
                                       in Start() */
    BCHP_Handle          hChip;     /* Handle to chip object */
    BREG_Handle          hRegister; /* Handle to register object */
    BMEM_Handle          hHeap;     /* Handle to memory object */
    BINT_Handle          hInt;      /* Handle to interrupt object */
    BRAP_FMM_P_Handle    hFmm;      /* Parent FMM handle */

    unsigned int         uiIndex;   /* Ring buffer index */
    uint32_t             ui32Offset;  /* Offset of a register of current ring 
                                       buffer from the corresponding register
                                       of the first ring buffer */
#if (  BRAP_DVD_FAMILY == 1) || ( BCHP_CHIP == 3563) || ( BRAP_7405_FAMILY == 1) || ( BRAP_3548_FAMILY == 1)		                                       
	bool 				bAllocatedInternally; /* TRUE = If Ring Buffer is allocated internally
												False = If Ring Buffer is allocatedd Externally */
#endif													
}BRAP_RBUF_P_Object;

/***************************************************************************
Summary:
    Initializes the Ring Buffer and returns a Ring Buffer handle .

Description:
    This function must be called first to get a handle .  This
    handle is used for all other function calls.

Returns:
    BERR_SUCCESS on success
    Error code on failure

See Also:
    BRAP_RBUF_P_Close, , BRAP_RBUF_P_Start, BRAP_RBUF_P_Stop, 
    BRAP_RBUF_P_GetDefaultSettings.
**************************************************************************/
BERR_Code BRAP_RBUF_P_Open (
    BRAP_FMM_P_Handle      hFmm,           /* [in] FMM handle */
    BRAP_RBUF_P_Handle *   phRBuf,         /* [out] Pointer to RBUF handle */
    unsigned int           uiRbufIndex,    /* [in] RBUF index */           
    const BRAP_RBUF_P_Settings * pSettings /* [in] Open time settings */
);


/***************************************************************************
Summary:
    Starts the Ring Buffer.

Description:

Returns:
    BERR_SUCCESS on success
    Error code on failure

See Also:
    BRAP_RBUF_P_Open, BRAP_RBUF_P_Close, BRAP_RBUF_P_Stop, 
    BRAP_RBUF_P_GetDefaultSettings.
**************************************************************************/
BERR_Code BRAP_RBUF_P_Start (
    BRAP_RBUF_P_Handle   hRBuf,          /* [in] RBUF handle */
    const BRAP_RBUF_P_Params * pParams   /* [in] Start time settings */  
);


/***************************************************************************
Summary:
    Stops the Ring Buffer.

Description:

Returns:
    BERR_SUCCESS on success
    Error code on failure

See Also:
    BRAP_RBUF_P_Open, BRAP_RBUF_P_Close, BRAP_RBUF_P_Start, 
    BRAP_RBUF_P_GetDefaultSettings.
**************************************************************************/
BERR_Code BRAP_RBUF_P_Stop (
    BRAP_RBUF_P_Handle    hRBuf     /* [in] RBUF handle */
);



/***************************************************************************
Summary:
    Releases the Ring Buffer handle

Description:
    This function will undo what has been done in Open.  

Returns:
    BERR_SUCCESS on success
    Error code on failure

See Also:
    BRAP_RBUF_P_Open.
**************************************************************************/
BERR_Code BRAP_RBUF_P_Close ( 
    BRAP_RBUF_P_Handle      hRbuf   /* [in] RBUF handle */
);


/***************************************************************************
Summary:
    Returns default values for RBuf Open time settings.

Description:
    For settings that the system cannot assign default values to, 
    an invalid value is returned. Note that the default settings are common
    for all Rbufs ie different Rbufs do not have different default settings.

Returns:
    BERR_SUCCESS on success
    Error code on failure

See Also:
    BRAP_RBUF_P_GetDefaultParams.
**************************************************************************/
BERR_Code 
BRAP_RBUF_P_GetDefaultSettings ( 
    BRAP_RBUF_P_Settings   * pDefSettings  /* [out] Pointer to memory where default
                                              settings should be written */
);

/***************************************************************************
Summary:
    Returns default values for RBuf Start time parameters.

Description:
    For parameters that the system cannot assign default values to, 
    an invalid value is returned. Note that the default parameters are common
    for all Rbufs ie different Rbufs do not have different default parameters.

Returns:
    BERR_SUCCESS on success
    Error code on failure

See Also:
    BRAP_RBUF_P_GetDefaultSettings.
**************************************************************************/
BERR_Code 
BRAP_RBUF_P_GetDefaultParams ( 
    BRAP_RBUF_P_Params    *pDefParams   /* [out] Pointer to memory where default
                                           parameters should be written */    
);


BERR_Code BRAP_RBUF_P_GetFrameSize(
    unsigned int uiRBufSize, 
    unsigned int *pFrameSize
);

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
);

#ifdef DEBUG_RING_BUFF_CAPTURE
/*************************************************************************
Summary:
    These api's are defined to capture the Ring buffer
*************************************************************************/
void BRAP_RBUF_P_CapUninit(void);
void BRAP_RBUF_P_CapInit(BREG_Handle reg_handle, BMEM_Heap_Handle heap);
void BRAP_RBUF_P_CapChStopSM(uint32_t path,uint32_t channel);
void BRAP_RBUF_P_CapChStopEnc(uint32_t path,uint32_t channel);
void BRAP_RBUF_P_CapChStopMixer(uint32_t path,uint32_t channel);
void BRAP_RBUF_P_CapChStopTranscode(uint32_t path,uint32_t channel);
void BRAP_RBUF_P_CapChStopPassthru(uint32_t path,uint32_t channel);
void BRAP_RBUF_P_CapChStopDecoder(uint32_t path,uint32_t channel);
void BRAP_RBUF_P_CapChStartDecoder(uint32_t path,uint32_t channel,uint32_t rbuf_id);
void BRAP_RBUF_P_CapChStartPassthru(uint32_t path,uint32_t channel,uint32_t rbuf_id);
void BRAP_RBUF_P_CapChStartTranscode(uint32_t path,uint32_t channel,uint32_t rbuf_id);
void BRAP_RBUF_P_CapChStartMixer(uint32_t path,uint32_t channel,uint32_t rbuf_id);
void BRAP_RBUF_P_CapChStartEnc(uint32_t path,uint32_t channel,uint32_t rbuf_id);
void BRAP_RBUF_P_CapChStartSM(uint32_t path,uint32_t channel,uint32_t rbuf_id);
#endif

#ifdef __cplusplus
}
#endif


#endif /* _BRAP_RBUF_PRIV_H_ */

/* End of File */
