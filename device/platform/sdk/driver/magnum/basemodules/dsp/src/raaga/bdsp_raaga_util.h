/***************************************************************************
 *     Copyright (c) 2006-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bdsp_raaga_util.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 4/6/11 2:29p $
 *
 * Module Description: Host DSP Interface
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/dsp/raaga/api/bdsp_raaga_util.h $
 * 
 * Hydra_Software_Devel/1   4/6/11 2:29p srajapur
 * SW7425-291: [7425]BDSP directory structure changes
 * 
 * Hydra_Software_Devel/2   1/18/11 4:18a gautamk
 * SW7125-766: Added code for review comments.
 * 
 * Hydra_Software_Devel/1   1/13/11 5:29a gautamk
 * SW7422-191:[7422]Adding DSP support for Audio decode in Basemodule  It
 * includes - DSP Booting - Open time Download :  - Start Time Audio
 * Decode algorithm download  - Message buffer interaction with DSP:  -
 * Synchronized Interrupt:  - Async interrupts:  - Config Commands for
 * Decode/TSM/IDS: - Status Buffer for Decode/TSM/IDS:  - Start AC3 Audio
 * Decode:  - Pause/Resume Command:  - Frame Advance command:  - Device
 * level interrupt
 * 
 * Hydra_Software_Devel/1   12/15/10 6:47p jgarrett
 * SW7422-146: Initial compileable prototype
 * 
 * Hydra_Software_Devel/1   12/15/10 2:01p jgarrett
 * SW7422-146: Adding initial BDSP files
 * 
 ***************************************************************************/

#ifndef BDSP_RAAGA_UTIL_H_
#define BDSP_RAAGA_UTIL_H_
#include "bdsp_raaga_types.h"

bool   BDSP_Raaga_P_IsPassthruSupportedWithoutLicense(
        BDSP_AudioType  eType);

bool  BDSP_Raaga_P_CheckIfPtrInvalid(
    void * ptr);

#endif
