/***************************************************************************
*     Copyright (c) 2004-2008, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: brap_spdiffm.h $
* $brcm_Revision: Hydra_Software_Devel/1 $
* $brcm_Date: 7/21/08 2:25p $
*
* Module Description:
*   Module name: SPDIFFM
*   This file lists all data structures, macros, enumerations and function 
*   prototypes for the SPDIF Formatter abstraction, which are exposed to the 
*   application developer.
*
* Revision History:
*
* $brcm_Log: /magnum/portinginterface/rap/base3/brap_spdiffm.h $
* 
* Hydra_Software_Devel/1   7/21/08 2:25p speter
* PR 44984: [3548,3556,7405] Adding files from base which have new
* Architecture specific changes and cannot be merged back in base
* 
* Hydra_Software_Devel/4   12/13/05 1:55p kagrawal
* PR 18578: Added support for dither and burst mode programming for
* SPDIFFM
* 
* Hydra_Software_Devel/3   6/2/05 10:48a rjain
* PR 14143: Implemented FMM Code review comments.
* 
* Hydra_Software_Devel/2   5/13/05 2:41p rjain
* PR 15160: Need ability to set various fields in the SPDIF channel
* status
* 
* Hydra_Software_Devel/1   1/11/05 11:40a rjain
* PR 13082: Moved to new directory structure
* 
* 
***************************************************************************/

#ifndef _BRAP_SPDIFFM_H_
#define _BRAP_SPDIFFM_H_

#include "brap.h"

#ifdef __cplusplus
extern "C" {
#endif


/*****************************************************************************
Summary:
    Enum describes various SPDIFFM Burst types.
    
Description:    

******************************************************************************/
typedef enum BRAP_SPDIFFM_BurstType
{
    BRAP_SPDIFFM_BurstType_eNull,   /* Insert NULL burst during compressed 
                                       data underflow */
    BRAP_SPDIFFM_BurstType_ePause,  /* Insert PAUSE burst during compressed 
                                       data underflow */
    BRAP_SPDIFFM_BurstType_eNone    /* Dont insert any bursts on an 
                                       underflow */
}BRAP_SPDIFFM_BurstType;

/***************************************************************************
Summary:
    Parameters to be passed by external user on Opening the SPDIF Formatter.
    
Description:
    

See Also:

***************************************************************************/
typedef struct BRAP_SPDIFFM_Settings
{
    bool                    bEnableDither; 
                                    /* If TRUE, dither signal will be sent 
                                       out on this when there is no data. 
                                       Every 192 samples, zero PCM data 
                                       will be overwritten with one. 
                                       Non-zero samples are left alone. 
                                       NOTE: Used only with uncompressed 
                                             PCM data */
   
    BRAP_SPDIFFM_BurstType  eBurstType;
                                    /* Burst type insertion for compressed 
                                       data out */

}BRAP_SPDIFFM_Settings;


/***************************************************************************
Summary:
    Parameters to be passed by external user on Starting the SPDIF Formatter.
    
Description:
    

See Also:

***************************************************************************/
typedef struct BRAP_SPDIFFM_Params
{
    bool         bSpdifFormat;   /* Selects whether data is to be formatted 
                                    for SPDIF or not
                                    TRUE: Spdif
                                    FALSE: PCM  */
}BRAP_SPDIFFM_Params;


#ifdef __cplusplus
}
#endif

#endif /* _BRAP_SPDIFFM_H_ */

/* End of File */
