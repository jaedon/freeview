/***************************************************************************
*     Copyright (c) 2004-2006, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: brap_rbuf.h $
* $brcm_Revision: Hydra_Software_Devel/15 $
* $brcm_Date: 5/12/06 6:30p $
*
* Module Description:
*   Module name: RBUF
*   This file lists all data structures, macros, enumerations and function 
*   prototypes for the RingBuffer abstraction, which are exposed to the 
*   application developer.
*
* Revision History:
*
* $brcm_Log: /magnum/portinginterface/rap/base/brap_rbuf.h $
* 
* Hydra_Software_Devel/15   5/12/06 6:30p bhanus
* PR 21388 : Checking in PCM Playback related changes.
* 
* Hydra_Software_Devel/14   12/13/05 3:24p kagrawal
* PR 18312: Added comments on uiWaterMark
* 
* Hydra_Software_Devel/13   12/8/05 2:44p kagrawal
* PR 18312: Updated comments on uiWaterMark
* 
* Hydra_Software_Devel/12   12/5/05 11:43a kagrawal
* PR 18312: Removed obsolete code - BRAP_RBUF_GetReadPointer () and
* BRAP_RBUF_GetWritePointer()
* 
* Hydra_Software_Devel/11   11/10/05 11:54a kagrawal
* PR 17590: Merging PCM Playback and Mixing changes to
* Hydra_Software_Devel
* 
* Hydra_Software_Devel/Kapil_PCM_Playback/1   10/24/05 5:03p kagrawal
* PR 17590: Initial check in for PCM Playback and Mixing
* 
* Hydra_Software_Devel/10   9/19/05 7:14p bmishra
* PR 16148: Removed chip specific code from interface files
* 
* Hydra_Software_Devel/9   9/13/05 7:43p bmishra
* PR 16148: Merged 7401 related changes
* 
* Hydra_Software_Devel/BLR_RAP_7401_Bringup/1   7/12/05 8:16p bmishra
* PR 16148: Modified for 7401 compatibility
* 
* Hydra_Software_Devel/7   6/30/05 8:38p bmishra
* PR 15976: Added comment for debug functions
* 
* Hydra_Software_Devel/6   6/27/05 6:08p bmishra
* PR 15976: Added Wrap Arround flag to functions for Get Ring buffer Rd
* and Wr pointers
* 
* Hydra_Software_Devel/5   6/27/05 1:44a bmishra
* PR 15976: Added Get Ring buffer pointers
* 
* Hydra_Software_Devel/4   6/2/05 10:47a rjain
* PR 14143: Implemented FMM Code review comments.
* 
* Hydra_Software_Devel/3   3/10/05 3:25p rjain
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
* Hydra_Software_Devel/2   2/16/05 1:46p rjain
* PR 14143: Implementing some comments from Phase 1 Code review done till
* date
* 
* Hydra_Software_Devel/1   1/11/05 11:39a rjain
* PR 13082: Moved to new directory structure
* 
***************************************************************************/

#ifndef _BRAP_RBUF_H_
#define _BRAP_RBUF_H_     

#include "brap.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
Summary:
    Parameters to be passed by application on Opening a Ring Buffer.

Description:
    Note: Free watermark percentage, say x, implies that if free number of 
          bytes in the associated ring buffer(s) exceeds x% of the total 
          buffer size, a ring buffer free water mark interrupt is generated.
See Also:

***************************************************************************/
typedef struct BRAP_RBUF_Settings
{
    void *          pBufferStart; /* Pointer to the start of this RBUF.
                                     If it is NULL, internally allocated memory
                                     is used. It has to be 256 byte aligned. */
    size_t          uiSize;       /* Ring Buffer size must be multiple of 
                                     256 bytes. If passed as 0, default value 
                                     indicated by BRAP_RBUF_P_DEFAULT_SIZE 
                                     will be used. 
                                     Note: Even if pBufferStart is NULL,
                                     uiSize can be passed for internal use */
    unsigned int    uiWaterMark;  /* Water Mark: Percentage of Free/Full Bytes.
                                     If passed as 0, default value indicated 
                                     by BRAP_RBUF_P_DEFAULT_WATER_MARK will 
                                     be used. The Water Mark Level depends on 
                                     the type of the System. 
                                     If Edge Triggered system, the application 
                                     provided Water Mark is Ignored and internally
                                     forced to be (RBuf Size-FrameSize), where 
                                     FrameSize is one-fourth of the Rbuf Size.
                                     If Level Triggered System, the user provided 
                                     Water Mark is Used.
                                     Note: Even if pBufferStart is NULL,
                                     uiWaterMark can be passed for internal use */
} BRAP_RBUF_Settings;

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
            );
#ifdef __cplusplus
}
#endif

#endif /* _BRAP_RBUF_H_ */

/* End of File */
