/***************************************************************************
 *     (c)2007-2011 Broadcom Corporation
 *
 *  This program is the proprietary software of Broadcom Corporation and/or its licensors,
 *  and may only be used, duplicated, modified or distributed pursuant to the terms and
 *  conditions of a separate, written license agreement executed between you and Broadcom
 *  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 *  no license (express or implied), right to use, or waiver of any kind with respect to the
 *  Software, and Broadcom expressly reserves all rights in and to the Software and all
 *  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 *  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 *  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 *
 *  Except as expressly set forth in the Authorized License,
 *
 *  1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 *  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 *  and to use this information only in connection with your use of Broadcom integrated circuit products.
 *
 *  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 *  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 *  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
 *  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
 *  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
 *  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
 *  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
 *  USE OR PERFORMANCE OF THE SOFTWARE.
 *
 *  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
 *  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
 *  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 *  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 *  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
 *  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 *  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 *  ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: nexus_hdmi_input_edid_ram.c $
 * $brcm_Revision: 9 $
 * $brcm_Date: 10/4/11 2:50p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: Q:/projects/7425/latest/nexus/modules/hdmi_input/7425/src/nexus_hdmi_input_edid_ram.c $
 * 
 * 9   10/4/11 2:50p rgreen
 * SW7425-1390,SW7425-1391: Fix coverity issue; prevent possible
 *  dereference of NULL pointer
 * 
 * 8   4/22/11 3:50p erickson
 * SW3556-1233: 93556 build
 * 
 * 7   2/16/11 6:18p rgreen
 * SW7422-129: Merge Changes
 * 
 * SW7422-129/1   2/2/11 6:26p rgreen
 * SW7422-129: Fix memory leak for unloading of EDID data; Add API
 *  NEXUS_HdmiInput_UnloadEdidData to remove EDID resources
 * 
 * 6   1/19/11 1:12p rgreen
 * SW35125-12: Fix compilation for 35125
 * 
 * SW35230-517/2   7/13/10 9:02p rgreen
 * SW35230-517: Update naming convention
 * 
 * SW35230-517/1   6/30/10 11:20a adtran
 * SW35230-517: add support for hotplug and edid controls
 * 
 * 4   4/13/09 3:58p rgreen
 * PR52879: Fix macro name for EDID RAM Support
 * 
 * 3   4/9/09 4:52p rgreen
 * PR52879: Add stub for EDID RAM Support
 * 
 * 2   4/9/09 4:02p rgreen
 * PR52879: Add eon-chip EDID RAM support
 * 
 * PR52879/1   4/9/09 3:32p rgreen
 * PR52879: Add eon-chip EDID RAM support
 * 
 * 1   3/23/09 3:07p rgreen
 * PR52879: Add EDID RAM support
 * 
 *
 **************************************************************************/
#include "nexus_hdmi_input_module.h"


BDBG_MODULE(nexus_hdmi_input_edid_ram);


NEXUS_Error NEXUS_HdmiInput_LoadEdidData(NEXUS_HdmiInputHandle hdmiInput,  
    const uint8_t *dataBytes, 
    uint16_t numEdidBytes)
{

#if (HDMI_RX_GEN != 35230)

#if BHDR_CONFIG_EDID_RAM
    BERR_Code rc;
    BDBG_OBJECT_ASSERT(hdmiInput, NEXUS_HdmiInput);
	
    if (hdmiInput->hdr == NULL)
    {
        BDBG_ERR(("Invalid HDR Pointer @ Line: %d \n ", __LINE__));
        return NEXUS_NOT_INITIALIZED ;
    }

    if (dataBytes == NULL)
    {
        BDBG_ERR(("EDID dataBytes (=NULL) are not specified")) ;
        return NEXUS_INVALID_PARAMETER ;
    }

    /* always make sure EDID is enabled */
    BHDR_EDID_RAM_Enable(hdmiInput->hdr) ;

    /* load the data passed in */
    if (hdmiInput->stEdidInfo.pDataBytes)
    {
		/* free any previously allocated EDID storage */
		BKNI_Free(hdmiInput->stEdidInfo.pDataBytes) ;
		BDBG_MSG(("Freeing previously allocted EDID...")) ;
    }

    hdmiInput->stEdidInfo.uiNumBlocks = numEdidBytes / 128  ;

    /* allocate the required memory for the new EDID  */	
    hdmiInput->stEdidInfo.pDataBytes = 
		(uint8_t *) BKNI_Malloc (sizeof (uint8_t)  * numEdidBytes) ;

    /* now copy the EDID to our hdmiInput handle */
    BKNI_Memcpy(hdmiInput->stEdidInfo.pDataBytes, dataBytes, numEdidBytes) ;

    /* load the EDID RAM */
    rc = BHDR_EDID_RAM_LoadData(hdmiInput->hdr,  &hdmiInput->stEdidInfo) ;
    if (rc) {rc = BERR_TRACE(rc); return rc ;}

    return NEXUS_SUCCESS ;
#else
    BSTD_UNUSED(hdmiInput) ;
    BSTD_UNUSED(dataBytes) ; 
    BSTD_UNUSED(numEdidBytes) ;
    return BERR_TRACE(NEXUS_NOT_SUPPORTED) ;
#endif	

#else /* 35230 */
    BSTD_UNUSED(hdmiInput) ;

    NEXUS_HdmiInputTvm_P_EdidRam_Enable();

    NEXUS_HdmiInputTvm_P_EdidRam_LoadData(dataBytes, numEdidBytes);
        
    return BERR_SUCCESS;
    
#endif

}




NEXUS_Error NEXUS_HdmiInput_UnloadEdidData(NEXUS_HdmiInputHandle hdmiInput)   
{

#if (HDMI_RX_GEN != 35230)

#if BHDR_CONFIG_EDID_RAM
    BDBG_OBJECT_ASSERT(hdmiInput, NEXUS_HdmiInput);
	
    if (hdmiInput->hdr == NULL)
    {
        BDBG_ERR(("Invalid HDR Pointer @ Line: %d \n ", __LINE__));
        return NEXUS_NOT_INITIALIZED ;
    }

	/* free any previously allocated EDID storage */
    if (hdmiInput->stEdidInfo.pDataBytes)
    {
		BKNI_Free(hdmiInput->stEdidInfo.pDataBytes) ;
		BDBG_MSG(("Freeing previously allocted EDID...")) ;
    }

	hdmiInput->stEdidInfo.pDataBytes = NULL ;

    return NEXUS_SUCCESS ;
#else
    BSTD_UNUSED(hdmiInput) ;
    return BERR_TRACE(NEXUS_NOT_SUPPORTED) ;
#endif	

#else /* 35230 */
    BSTD_UNUSED(hdmiInput) ;
        
    return NEXUS_SUCCESS ;
    
#endif

}


