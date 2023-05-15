/***************************************************************************
 *     Copyright (c) 2006-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bdsp_raaga_util.c $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 5/11/11 5:31p $
 *
 * Module Description: Raaga Util functions
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/dsp/raaga/api/bdsp_raaga_util.c $
 * 
 * Hydra_Software_Devel/2   5/11/11 5:31p srajapur
 * SW7422-374 : [7425] Incorporate changes to BDSP "details" structures
 * from code review feedback.
 * 
 * Hydra_Software_Devel/1   4/6/11 2:29p srajapur
 * SW7425-291: [7425]BDSP directory structure changes
 * 
 * Hydra_Software_Devel/6   3/24/11 11:03a jgarrett
 * SW7422-146: Removing erroneous definition codec details structure
 * 
 * Hydra_Software_Devel/5   2/5/11 4:25a srajapur
 * SW7422-247 : [7422] Adding Generic passthrul audio processing support.
 * 
 * Hydra_Software_Devel/4   1/25/11 2:24a gautamk
 * SW7422-191:Adding error recovery code for Raaga Open
 * 
 * Hydra_Software_Devel/3   1/19/11 2:16a gautamk
 * SW7422-191: putting all the Details of Audio codec in a const array and
 * then using it across the code.
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
 ***************************************************************************/

#include "bdsp_raaga_types.h"
#include "bdsp_raaga_priv.h"

BDBG_MODULE(bdsp_raaga_util);

bool    BDSP_Raaga_P_CheckIfPtrInvalid(
    void * ptr)
{
    if((ptr == NULL)
        ||(ptr == (void *)0)
        ||(ptr == (void *)BDSP_RAAGA_INVALID_DRAM_ADDRESS)
        ||(ptr == (void *)BDSP_AF_P_DRAM_ADDR_INVALID))
    {
        return true;
    }
    else
    {
        return  false;
    }
}


bool   BDSP_Raaga_P_IsPassthruSupportedWithoutLicense(
    BDSP_AudioType  eType)
{
    return BDSP_sAudioCodecDetails[eType]->passthruSupportedWithoutLicense;
}


