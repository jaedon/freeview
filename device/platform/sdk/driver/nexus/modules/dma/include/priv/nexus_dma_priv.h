/***************************************************************************
*     (c)2004-2012 Broadcom Corporation
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
* $brcm_Workfile: nexus_dma_priv.h $
* $brcm_Revision: 5 $
* $brcm_Date: 6/19/12 12:42p $
*
* API Description:
*   API name: Dma
*    Specific APIs related to memory to memory DMA.
*
* Revision History:
*
* $brcm_Log: /nexus/modules/dma/7400/include/priv/nexus_dma_priv.h $
* 
* 5   6/19/12 12:42p vsilyaev
* SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
* 
* 4   6/4/12 4:31p vsilyaev
* SW7420-2313: Added reference counting for Nexus_Dma and NEXUS_DmaJob
*  objects
* 
* 3   5/7/08 10:19p vsilyaev
* PR 41869: Swapped pointer and nblocks to follow usual Nexus order
* 
* 2   5/1/08 4:09p vsilyaev
* PR 42392: Added control to disable cache flush
* 
* 1   4/24/08 5:38p vsilyaev
* PR 42158: Private interface for DMA
* 
*
***************************************************************************/

#ifndef NEXUS_DMA_PRIV_H__
#define NEXUS_DMA_PRIV_H__

#ifdef __cplusplus
extern "C" {
#endif


/***************************************************************************
Summary:
Prepares and run DMA transaction

Description:
This function preapers and starts a DMA job operation.  If function returns NEXUS_SUCCESS, then
DMA transaction is completed and new transaction could be started right the way. 
However if function returns NEXUS_DMA_QUEUED, then caller should wait 
for the completion event or poll the current status with NEXUS_DmaJob_GetStatus
to ensure the transfer is complete before using the same job a second
time.

See Also:
    NEXUS_DmaJob_ProcessBlocks_priv
Returns:
  On Success
    NEXUS_SUCCESS - DMA completed
    NEXUS_DMA_QUEUED - DMA job queued for processing

***************************************************************************/
NEXUS_Error NEXUS_DmaJob_ProcessBlocks_priv(
    NEXUS_DmaJobHandle handle,
    const NEXUS_DmaJobBlockSettings *pDmaBlocks, /* pointer to array of DMA blocks */
    unsigned nBlocks,            /* Must be < JobSettings.numBlocks */
    BKNI_EventHandle event        /* event that is fired when queued DMA transaction is completed */
    );

NEXUS_OBJECT_CLASS_DECLARE(NEXUS_Dma);
NEXUS_OBJECT_CLASS_DECLARE(NEXUS_DmaJob);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef NEXUS_DMA_PRIV_H__ */

