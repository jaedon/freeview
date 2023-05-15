/***************************************************************************
*     Copyright (c) 2004-2005, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: brap_pcm.h $
* $brcm_Revision: Hydra_Software_Devel/10 $
* $brcm_Date: 12/9/05 4:40p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /magnum/portinginterface/rap/base/brap_pcm.h $
* 
* Hydra_Software_Devel/10   12/9/05 4:40p rjain
* PR 18264: Capture PI Development
* 
* Hydra_Software_Devel/9   12/8/05 2:41p kagrawal
* PR 18312: Added comments on uiWaterMark
* 
* Hydra_Software_Devel/8   12/6/05 7:20p kagrawal
* PR 18312: Added following ISR version of routines:
* 1. BRAP_PB_GetBufInfo_isr
* 2. BRAP_PB_UpdateBufUsg_isr
* 
* Hydra_Software_Devel/7   12/3/05 7:07p kagrawal
* PR 18312: Removed ePcmOutputMode from the PCM playback channel open
* interface
* 
* Hydra_Software_Devel/6   12/3/05 6:04p kagrawal
* PR 18312: Added eBufDataMode and ePcmOutputMode to start params
* 
* Hydra_Software_Devel/5   11/30/05 4:23p kagrawal
* PR 18279: Rearchitected output sampling rate change for PCM channel
* 
* Hydra_Software_Devel/4   11/29/05 1:58p bmishra
* PR 18264: Adding Data Structures for Capture
* 
* Hydra_Software_Devel/3   11/16/05 9:16p kagrawal
* PR 17590: Removed dead code
* 
* Hydra_Software_Devel/2   11/12/05 7:24p kagrawal
* PR 17590: Changed BRAP_PB_AudioParams structure to have common delay
* and size for the left and right ring buffers
* 
* Hydra_Software_Devel/1   11/10/05 10:45a kagrawal
* PR 17590: Merging PCM Playback and Mixing to Hydra_Software_Devel
* 
* Kapil_PCM_Playback/1   10/24/05 5:53p kagrawal
* PR 17590: Initial check in for PCM Playback and Mixing
* 
***************************************************************************/
#ifndef _BRAP_PCM_H__
#define _BRAP_PCM_H__

/*{{{ Includes */

#include "brap.h"

/*}}}*/

#ifdef __cplusplus
extern "C" {
#endif

/*{{{ Defines */


/*}}}*/

/*{{{ Typedefs */







/*}}}*/

/*{{{ Function prototypes */

/*}}}*/

#ifdef __cplusplus
}
#endif

#endif /*_BRAP_PCM_H__*/

