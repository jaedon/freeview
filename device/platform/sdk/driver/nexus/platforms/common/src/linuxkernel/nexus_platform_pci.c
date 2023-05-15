/******************************************************************************
 *    (c)2008 Broadcom Corporation
 * 
 * This program is the proprietary software of Broadcom Corporation and/or its licensors,
 * and may only be used, duplicated, modified or distributed pursuant to the terms and
 * conditions of a separate, written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 * no license (express or implied), right to use, or waiver of any kind with respect to the
 * Software, and Broadcom expressly reserves all rights in and to the Software and all
 * intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 * HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 * NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.  
 *  
 * Except as expressly set forth in the Authorized License,
 *  
 * 1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 * secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of Broadcom integrated circuit products.
 *  
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS" 
 * AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR 
 * WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO 
 * THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES 
 * OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, 
 * LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION 
 * OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF 
 * USE OR PERFORMANCE OF THE SOFTWARE.
 * 
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS 
 * LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR 
 * EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR 
 * USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF 
 * THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT 
 * ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE 
 * LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF 
 * ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: nexus_platform_pci.c $
 * $brcm_Revision: 1 $
 * $brcm_Date: 4/8/08 11:19a $
 * 
 * Module Description:
 * 
 * Revision History:
 * 
 * $brcm_Log: /nexus/platforms/97400/src/linuxkernel/nexus_platform_pci.c $
 * 
 * 1   4/8/08 11:19a vsilyaev
 * PR 39899: Fixed linuxkernel build
 * 
 *****************************************************************************/

#include "nexus_base.h"
#include "nexus_platform_priv.h"


/***************************************************************************
Summary:
	Detect a PCI device with specified VendorId-DeviceId and make it 
	current
***************************************************************************/
NEXUS_Error NEXUS_Platform_PCI_P_Detect(uint32_t venDevId)
{
    BSTD_UNUSED(venDevId);
    return BERR_TRACE(NEXUS_NOT_SUPPORTED);
}


/***************************************************************************
Summary:
	Return the BAR information from the current PCI device.
***************************************************************************/
NEXUS_Error NEXUS_Platform_PCI_P_GetBarInfo(uint32_t venDevId, unsigned int barNumber, uint32_t *pBarBaseAddr, uint32_t *pBarSize)
{
    BSTD_UNUSED(venDevId);
    BSTD_UNUSED(barNumber);
    BSTD_UNUSED(pBarBaseAddr);
    BSTD_UNUSED(pBarSize);
    return BERR_TRACE(NEXUS_NOT_SUPPORTED);
}

