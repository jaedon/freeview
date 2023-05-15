/***************************************************************************
 *     (c)2008-2011 Broadcom Corporation
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
 *
 * Module Description:
 *  Initial Implementation of DAL functionality
 *	Compatibility NOCS3.0
 *
 * Revision History:
 *
 *  Initial version internally tested with DALTS Version 3.12.0
 * 	Based on DALAPI Version 7.1.0
 *	Based on Nagra header nocs_csd.h Revision 3.3.5
 *	Based on Nagra header ca_defs.h Revision 1.28
 *
 *
 **************************************************************************/

#include <string.h>

#include "bchp_sun_top_ctrl.h"

#include "nexus_platform.h"
#include "nexus_dma.h"
#include "nexus_types.h"
#include "nexus_memory.h"
#include "nexus_security.h"
#include "nexus_security_rawcommand.h"
#include "nexus_otpmsp.h"
#include "nexus_keyladder.h"
#include "nocs_csd_impl.h"
#include "nocs_csd.h"
#include "csd_brcm_extensions.h"

#include "bdbg.h"
#include "bchp_jtag_otp.h"

#include "ca_defs.h"
#include "emitools.h"

#include <stdio.h>


#define CSD_ASKM_CA_VENDOR_ID 0x97ed
#define CSD_ASKM_MODULE_ID 0xE0

#define INVALID_PID 0xffff
#define CSD_UNUSED(x) (void)(x)

#if (CSDAPI_VERSION_MEDIUM==1)
#elif (CSDAPI_VERSION_MEDIUM>1)

static TCsdStatus csdEncryptData
(
    TCsdUnsignedInt16       xEmi,
    TCsdR2RKeyPathHandle*  pxR2RKeyPathHandle,
    const TCsdUnsignedInt8*      pxInitializationVector,
    TCsdSize                xInitializationVectorSize,
    const TCsdUnsignedInt8*      pxInputData,
    TCsdUnsignedInt8*      pxOutputData,
    TCsdSize                xDataSize
);

static TCsdStatus csdDecryptData
(
    TCsdUnsignedInt16       xEmi,
    TCsdR2RKeyPathHandle*  pxR2RKeyPathHandle,
    const TCsdUnsignedInt8*      pxInitializationVector,
    TCsdSize                xInitializationVectorSize,
    const TCsdUnsignedInt8*      pxInputData,
    TCsdUnsignedInt8*      pxOutputData,
    TCsdSize                xDataSize
);

static TCsdStatus csdSetClearTextRam2RamHostKey
(
    TCsdUnsignedInt16              xEmi,
    TCsdR2RKeyPathHandle*         pxR2RKeyPathHandle,
    const TCsdUnsignedInt8*             pxClearTextR2RHostKey,
    TCsdSize                       xClearTextR2RHostKeySize
);

static TCsdStatus csdSetProtectedRam2RamContentKeys
(
    TCsdUnsignedInt16              xEmi,
    TCsdR2RKeyPathHandle*         pxR2RKeyPathHandle,
    const TCsdR2RCipheredProtectingKeys  xR2RCipheredProtectingKeys,
    const TCsdUnsignedInt8*             pxCipheredContentKey,
    TCsdSize                       xCipheredContentKeySize
);

#endif

BDBG_MODULE(NCSD);
bool CSD_INITIALIZED = FALSE;


TCsdInitParameters*  csdInitParameters[CSD_MAX_INDEX]= {NULL};

/**********************************************************************************************
						 CSD implementation specific structures
**********************************************************************************************/

/**********************************************************************************************
						 CSD implementation specific  variables
**********************************************************************************************/

static unsigned long 			gCrcTab [256];

#define BHSM_SLOT_NUM_INIT_VAL		0xFFFF

/**********************************************************************************************
						 CSD implementation specific internal functions/macros
**********************************************************************************************/

static unsigned long cmpCrc32 (unsigned char * xbufInp, int xbufLen)
{
    unsigned long crcVal;

    crcVal = 0xFFFFFFFF;
    while (xbufLen--) {
        crcVal = ((crcVal >> 8) & 0x00FFFFFF) ^ gCrcTab[(crcVal ^ *xbufInp++) & 0xFF];
    }
    return (crcVal ^ 0xFFFFFFFF);
}

static void genCrc32Tab (void)
{
    unsigned long crcVal, poly;
    register int i, j;

    poly = 0xEDB88320L;
    for (i = 0; i < 256; i++)
    {
        crcVal = i;
        for (j = 8; j > 0; j--)
        {
            if (crcVal & 1)
            {
                crcVal = (crcVal >> 1) ^ poly;
            } else {
                crcVal >>= 1;
            }
        }
        gCrcTab [i] = crcVal;
    }
}

#if(CFG_BIG_ENDIAN==1)
static void csdbyteSwap(
    unsigned char *word
)
{
    unsigned char temp;

    temp = word[0];
    word[0] = word[3];
    word[3] = temp;

    temp = word[1];
    word[1] = word[2];
    word[2] = temp;

    return;
}
#endif

static void CompleteCallback(void *pParam, int iParam)
{
    iParam=iParam;
    BKNI_SetEvent(pParam);
}


TCsdStatus csdR2RCryptoOperation(
    NEXUS_KeySlotHandle keyHandle,
    const TCsdUnsignedInt8* pxInputData,
    TCsdUnsignedInt8* pxOutputData,
    TCsdSize xDataSize
)
{
    NEXUS_DmaHandle dma;
    NEXUS_DmaJobSettings jobSettings;
    NEXUS_DmaJobHandle dmaJob;
    NEXUS_DmaJobBlockSettings blockSettings;
    unsigned char *input_data;
    unsigned char *output_data;
    TCsdStatus    retCode  =  CSD_NO_ERROR;
    BKNI_EventHandle event;
    NEXUS_Error rc;


#if(CFG_BIG_ENDIAN==1)
    TCsdSize i;
#endif
    BKNI_CreateEvent(&event);

    /* Need to allocate memory for M2M input and output buffers. */
    NEXUS_Memory_Allocate(sizeof(unsigned char) * xDataSize, NULL, (void *)&input_data);
    NEXUS_Memory_Allocate(sizeof(unsigned char) * xDataSize, NULL, (void *)&output_data);

    /* Copy the input data to M2M input buffer. */
    BKNI_Memcpy((void *)input_data, (void *)pxInputData, xDataSize );

#if(CFG_BIG_ENDIAN==1)
    for(i=0; i<xDataSize; i+=4)
        csdbyteSwap(&(input_data[i]));
#endif

    /* Open DMA handle */
    dma = NEXUS_Dma_Open(0, NULL);

    NEXUS_DmaJob_GetDefaultSettings(&jobSettings);
    jobSettings.numBlocks = 1;
    jobSettings.keySlot = keyHandle;
    jobSettings.dataFormat = NEXUS_DmaDataFormat_eBlock;
    jobSettings.completionCallback.callback = CompleteCallback;
    jobSettings.completionCallback.context = event;

    dmaJob = NEXUS_DmaJob_Create(dma, &jobSettings);

    NEXUS_DmaJob_GetDefaultBlockSettings(&blockSettings);
    blockSettings.pSrcAddr = input_data;
    blockSettings.pDestAddr = output_data;
    blockSettings.blockSize = xDataSize;
    blockSettings.resetCrypto = true;
    blockSettings.scatterGatherCryptoStart = true;
    blockSettings.scatterGatherCryptoEnd = true;
    blockSettings.cached = true;

    rc = NEXUS_DmaJob_ProcessBlocks(dmaJob, &blockSettings, 1);

    /* Wait for M2M Encryption to complete. */
    if (rc == NEXUS_DMA_QUEUED )
    {
        if (BKNI_WaitForEvent(event, BKNI_INFINITE) )
        {
        }
    }
    else if ( rc != NEXUS_SUCCESS )
    {
        BDBG_ERR(("DMA transfer failed "));

    }


#if(CFG_BIG_ENDIAN==1)
    for(i=0; i<xDataSize; i+=4)
        csdbyteSwap(&(output_data[i]));
#endif

    /* Copy the data to M2M output buffer. */
    BKNI_Memcpy((void *)pxOutputData, (void *)output_data, xDataSize );


    /* Done.  Close DMA. */
    NEXUS_DmaJob_Destroy(dmaJob);

    NEXUS_Dma_Close(dma);
    BKNI_DestroyEvent(event);

    NEXUS_Memory_Free(output_data);
    NEXUS_Memory_Free(input_data);

    return retCode;


}


/**********************************************************************************************
						 CSD API Brcm Enhancements
**********************************************************************************************/


TCsdStatus csdBrcmOpenR2RKeyPath(TCsdR2RKeyPathHandle** pR2RKeyPathOut,
                                 unsigned int    xTransportSessionId)
{
    NEXUS_SecurityKeySlotSettings keySlotSettings;
    TCsdR2RKeyPathHandle *pR2RKeyPath = 0;

    BDBG_MSG(("%s Enter", __FUNCTION__));

    /* make sure CSD is open */
    if(!CSD_INITIALIZED) {
        BDBG_ERR(("CSD not initialized!"));
        return CSD_ERROR;
    }

    /* allocate keypath */
    pR2RKeyPath=BKNI_Malloc(sizeof(TCsdR2RKeyPathHandle));
    if(pR2RKeyPath == NULL)
    {
        BDBG_ERR(("%s Fail - out of memory", __FUNCTION__));
        return CSD_ERROR;
    }

    BKNI_Memset(pR2RKeyPath, 0, sizeof(TCsdR2RKeyPathHandle));
    pR2RKeyPath->initHandle = csdInitParameters[0]; /* only ever one instance */
    pR2RKeyPath->sessionId = xTransportSessionId;

    /* get default keyslot settings */
    NEXUS_Security_GetDefaultKeySlotSettings(&keySlotSettings);
    keySlotSettings.keySlotEngine = NEXUS_SecurityEngine_eM2m;

    pR2RKeyPath->keyHandle = NEXUS_Security_AllocateKeySlot(&keySlotSettings);
    if ( !pR2RKeyPath->keyHandle )
    {
        BDBG_ERR(("Fail to allocate secure key slot\n"));
        BKNI_Free(pR2RKeyPath);
        return CSD_ERROR;
    }

    BDBG_MSG(("%s NEXUS_Security_AllocateKeySlot pDscKeyPath->keyHandle 0x%x", __FUNCTION__, pR2RKeyPath->keyHandle));

    *pR2RKeyPathOut = pR2RKeyPath;
    return CSD_NO_ERROR;
}

TCsdStatus csdBrcmCloseR2RKeyPath(TCsdR2RKeyPathHandle* pR2RKeyPath)
{
    BDBG_MSG(("%s Enter", __FUNCTION__));
    if(!pR2RKeyPath) {
        BDBG_ERR(("CSD invalid parameter"));
        return CSD_ERROR;
    }

    /* make sure CSD is open */
    if(!CSD_INITIALIZED) {
        BDBG_ERR(("CSD not initialized!"));
        return CSD_ERROR;
    }

    BDBG_MSG(("csdBrcmCloseR2RKeyPath pR2RKeyPath->keyHandle 0x%x", pR2RKeyPath->keyHandle));

    NEXUS_Security_FreeKeySlot(pR2RKeyPath->keyHandle);
    BKNI_Free(pR2RKeyPath);

    return CSD_NO_ERROR;
}

TCsdStatus csdBrcmOpenScrKeyPath(TCsdScrKeyPathHandle** pScrKeyPathOut,
                                 unsigned int    xTransportSessionId)
{
    NEXUS_SecurityKeySlotSettings keySlotSettings;
    TCsdScrKeyPathHandle *pScrKeyPath = 0;

    BDBG_MSG(("%s Enter", __FUNCTION__));

    /* make sure CSD is open */
    if(!CSD_INITIALIZED) {
        BDBG_ERR(("CSD not initialized!"));
        return CSD_ERROR;
    }

    /* allocate keypath */
    pScrKeyPath=BKNI_Malloc(sizeof(TCsdScrKeyPathHandle));
    if(pScrKeyPath == NULL)
    {
        BDBG_ERR(("%s Fail - out of memory", __FUNCTION__));
        return CSD_ERROR;
    }

    BKNI_Memset(pScrKeyPath, 0, sizeof(TCsdScrKeyPathHandle));
    pScrKeyPath->sessionId = xTransportSessionId;
    /* get default keyslot settings */
    NEXUS_Security_GetDefaultKeySlotSettings(&keySlotSettings);
    keySlotSettings.keySlotEngine = NEXUS_SecurityEngine_eM2m;

    pScrKeyPath->keyHandle = NEXUS_Security_AllocateKeySlot(&keySlotSettings);
    if ( !pScrKeyPath->keyHandle )
    {
        BDBG_ERR(("Fail to allocate secure key slot\n"));
        BKNI_Free(pScrKeyPath);
        return CSD_ERROR;
    }

    BDBG_MSG(("csdBrcmOpenScrKeyPath NEXUS_Security_AllocateKeySlot pScrKeyPath->keyHandle 0x%x", pScrKeyPath->keyHandle));

    *pScrKeyPathOut = pScrKeyPath;
    return CSD_NO_ERROR;
}

NEXUS_KeySlotHandle csdBrcmGetScrKeyHandle(TCsdScrKeyPathHandle* pScrKeyPath)
{
    BDBG_MSG(("%s Enter", __FUNCTION__));
    if(!pScrKeyPath) {
        BDBG_ERR(("CSD invalid parameter"));
        return (NEXUS_KeySlotHandle)0;
    }

    /* make sure CSD is open */
    if(!CSD_INITIALIZED) {
        BDBG_ERR(("CSD not initialized!"));
        return (NEXUS_KeySlotHandle)0;
    }

    BDBG_MSG(("csdBrcmGetScrKeyHandle pScrKeyPath->keyHandle 0x%x", pScrKeyPath->keyHandle));


    return pScrKeyPath->keyHandle;
}



TCsdStatus csdBrcmCloseScrKeyPath(TCsdScrKeyPathHandle* pScrKeyPath)
{
    BDBG_MSG(("%s Enter", __FUNCTION__));
    if(!pScrKeyPath) {
        BDBG_ERR(("CSD invalid parameter"));
        return CSD_ERROR;
    }

    /* make sure CSD is open */
    if(!CSD_INITIALIZED) {
        BDBG_ERR(("CSD not initialized!"));
        return CSD_ERROR;
    }

    BDBG_MSG(("csdBrcmCloseScrKeyPath pScrKeyPath->keyHandle 0x%x", pScrKeyPath->keyHandle));
    /* HUMAX_PLATFORM_BASE */
    {
        NEXUS_SecurityInvalidateKeySettings securityInvalidateKeySettings;
        NEXUS_Security_GetDefaultInvalidateKeySettings(&securityInvalidateKeySettings);
        securityInvalidateKeySettings.keyDestEntryType  = NEXUS_SecurityKeyType_eOddAndEven;
        securityInvalidateKeySettings.invalidateKeyType = NEXUS_SecurityInvalidateKeyFlag_eDestKeyOnly;
        securityInvalidateKeySettings.keyDestBlckType = NEXUS_SecurityAlgorithmConfigDestination_eCa;
        securityInvalidateKeySettings.virtualKeyLadderID = NEXUS_SecurityVirtualKeyladderID_eVKLDummy;
        if(NEXUS_Security_InvalidateKey(pScrKeyPath->keyHandle, &securityInvalidateKeySettings) != 0)
        {
            BDBG_ERR(("Invalidate Key setting error!"));
        }
    }

    NEXUS_Security_FreeKeySlot(pScrKeyPath->keyHandle);
    BKNI_Free(pScrKeyPath);

    return CSD_NO_ERROR;
}


TCsdStatus csdBrcmOpenDscKeyPath(TCsdDscKeyPathHandle** pDscKeyPathOut,
                                 unsigned int    xTransportSessionId,
                                 NEXUS_PlaypumpHandle playpumpHandle)
{
    NEXUS_SecurityKeySlotSettings keySlotSettings;
    TCsdDscKeyPathHandle *pDscKeyPath = 0;

    BDBG_MSG(("%s Enter", __FUNCTION__));

    /* make sure CSD is open */
    if(!CSD_INITIALIZED) {
        BDBG_ERR(("CSD not initialized!"));
        return CSD_ERROR;
    }

    /* allocate keypath */
    pDscKeyPath=BKNI_Malloc(sizeof(TCsdDscKeyPathHandle));
    if(pDscKeyPath == NULL)
    {
        BDBG_ERR(("%s Fail - out of memory", __FUNCTION__));
        return CSD_ERROR;
    }

    BKNI_Memset(pDscKeyPath, 0, sizeof(TCsdDscKeyPathHandle));
    pDscKeyPath->initHandle = csdInitParameters[0]; /* only ever one instance */
    pDscKeyPath->pidchannel = 0xFFFFFFFF;
    pDscKeyPath->sessionId = xTransportSessionId;
    pDscKeyPath->playpumpHandle = playpumpHandle;
    pDscKeyPath->xCurrentOddEmi = 0xcff0;
    pDscKeyPath->xCurrentEvenEmi = 0xcff0;
    pDscKeyPath->bAnyKeySet = false;

    /* get default keyslot settings */
    NEXUS_Security_GetDefaultKeySlotSettings(&keySlotSettings);
    keySlotSettings.keySlotEngine = NEXUS_SecurityEngine_eCa;

    pDscKeyPath->keyHandle = NEXUS_Security_AllocateKeySlot(&keySlotSettings);
    if ( !pDscKeyPath->keyHandle )
    {
        BDBG_ERR(("Fail to allocate secure key slot\n"));
        BKNI_Free(pDscKeyPath);
        return CSD_ERROR;
    }

    BDBG_MSG(("%s NEXUS_Security_AllocateKeySlot pDscKeyPath->keyHandle 0x%x", __FUNCTION__, pDscKeyPath->keyHandle));

    *pDscKeyPathOut = pDscKeyPath;
    return CSD_NO_ERROR;
}

NEXUS_KeySlotHandle csdBrcmGetDscKeyHandle(TCsdDscKeyPathHandle* pDscKeyPath)
{
    BDBG_MSG(("%s Enter", __FUNCTION__));
    if(!pDscKeyPath) {
        BDBG_ERR(("CSD invalid parameter"));
        return (NEXUS_KeySlotHandle)0;
    }

    /* make sure CSD is open */
    if(!CSD_INITIALIZED) {
        BDBG_ERR(("CSD not initialized!"));
        return (NEXUS_KeySlotHandle)0;
    }

    BDBG_MSG(("csdBrcmGetDscKeyHandle pDscKeyPath->keyHandle 0x%x", pDscKeyPath->keyHandle));

    return pDscKeyPath->keyHandle;
}

TCsdStatus csdBrcmCloseDscKeyPath(TCsdDscKeyPathHandle* pDscKeyPath)
{
    unsigned int i=0;
    BDBG_MSG(("%s Enter", __FUNCTION__));
    if(!pDscKeyPath) {
        BDBG_ERR(("CSD invalid parameter"));
        return CSD_ERROR;
    }

    /* make sure CSD is open */
    if(!CSD_INITIALIZED) {
        BDBG_ERR(("CSD not initialized!"));
        return CSD_ERROR;
    }

    BDBG_MSG(("csdBrcmCloseDscKeyPath pDscKeyPath->keyHandle 0x%x", pDscKeyPath->keyHandle));

    /* if there is a pid channel allocated free it */
    if(pDscKeyPath->numberOfPIDS) {
        for (i=0; i<pDscKeyPath->numberOfPIDS; i++) {
            if(!pDscKeyPath->pidList[i].pidChannelHandle) {
                BDBG_ERR(("expecting to be a valid pid handle here! %d",i));
            } else {
#if NEXUS_PLATFORM_97241 > NEXUS_PLATFORM_VERSION(12,4)
                NEXUS_KeySlot_RemovePidChannel(pDscKeyPath->keyHandle,pDscKeyPath->pidList[i].pidChannelHandle);
#else
                NEXUS_PidChannelStatus pidStatus;
                NEXUS_PidChannel_GetStatus(pDscKeyPath->pidList[i].pidChannelHandle, &pidStatus);
                NEXUS_Security_RemovePidChannelFromKeySlot(pDscKeyPath->keyHandle, pidStatus.pidChannelIndex);
#endif

                if(pDscKeyPath->playpumpHandle) {
                    BDBG_MSG((" removing from playpump"));
                    NEXUS_Playpump_ClosePidChannel(pDscKeyPath->playpumpHandle, pDscKeyPath->pidList[i].pidChannelHandle);
                } else {
                    NEXUS_PidChannel_Close(pDscKeyPath->pidList[i].pidChannelHandle);
                }
            }
        }
    }
    /* HUMAX_PLATFORM_BASE */
    {
        NEXUS_SecurityInvalidateKeySettings securityInvalidateKeySettings;
        NEXUS_Security_GetDefaultInvalidateKeySettings(&securityInvalidateKeySettings);
        securityInvalidateKeySettings.keyDestEntryType  = NEXUS_SecurityKeyType_eOddAndEven;
        securityInvalidateKeySettings.invalidateKeyType = NEXUS_SecurityInvalidateKeyFlag_eDestKeyOnly;
        securityInvalidateKeySettings.keyDestBlckType = NEXUS_SecurityAlgorithmConfigDestination_eCa;
        securityInvalidateKeySettings.virtualKeyLadderID = NEXUS_SecurityVirtualKeyladderID_eVKLDummy;
        if(NEXUS_Security_InvalidateKey(pDscKeyPath->keyHandle, &securityInvalidateKeySettings) != 0)
        {
	    BDBG_ERR(("Invalidate Key setting error!"));
        }
    }

    NEXUS_Security_FreeKeySlot(pDscKeyPath->keyHandle);
    BKNI_Free(pDscKeyPath);

    return CSD_NO_ERROR;
}

TCsdStatus csdBrcmSetDscPlaypumpHandle(TCsdDscKeyPathHandle* pDscKeyPath, NEXUS_PlaypumpHandle playpumpHandle)
{
    unsigned int i;
    NEXUS_PidChannelStatus pidStatus;

    if(!pDscKeyPath)
        return CSD_ERROR;

    /* Playpump hasn't changed? */
    if(pDscKeyPath->playpumpHandle == playpumpHandle)
        return CSD_NO_ERROR;

    /* Any PIDs? */
    if (pDscKeyPath->numberOfPIDS == 0)
    {
        pDscKeyPath->playpumpHandle = playpumpHandle;
        return CSD_NO_ERROR;
    }

    /* Free all the existing PIDs */
    for(i=0; i<pDscKeyPath->numberOfPIDS; i++) {
        if (pDscKeyPath->pidList[i].pidChannelHandle) {
#if NEXUS_PLATFORM_97241 > NEXUS_PLATFORM_VERSION(12,4)
            NEXUS_KeySlot_RemovePidChannel(pDscKeyPath->keyHandle, pDscKeyPath->pidList[i].pidChannelHandle);
#else
            NEXUS_PidChannelStatus pidStatus;
            NEXUS_PidChannel_GetStatus(pDscKeyPath->pidList[i].pidChannelHandle, &pidStatus);
            NEXUS_Security_RemovePidChannelFromKeySlot(pDscKeyPath->keyHandle, pidStatus.pidChannelIndex);
#endif
            if(pDscKeyPath->playpumpHandle) {
                NEXUS_Playpump_ClosePidChannel(pDscKeyPath->playpumpHandle, pDscKeyPath->pidList[i].pidChannelHandle);
            } else {
                NEXUS_PidChannel_Close(pDscKeyPath->pidList[i].pidChannelHandle);
            }
            pDscKeyPath->pidList[i].pidChannelHandle = NULL;
        }
    }

    /* Store new playpumpHandle */
    pDscKeyPath->playpumpHandle = playpumpHandle;

    /* Re-allocate the PID channel handles */
    for(i=0; i<pDscKeyPath->numberOfPIDS; i++) {
#if NEXUS_PLATFORM_97241 > NEXUS_PLATFORM_VERSION(12,4)
#else
        NEXUS_PidChannelStatus pidStatus;
#endif
        if(playpumpHandle) {
            pDscKeyPath->pidList[i].pidChannelHandle = NEXUS_Playpump_OpenPidChannel(pDscKeyPath->playpumpHandle, pDscKeyPath->pidList[i].PID, NULL);
        } else {
            pDscKeyPath->pidList[i].pidChannelHandle = NEXUS_PidChannel_Open(pDscKeyPath->sessionId, pDscKeyPath->pidList[i].PID, NULL);
        }

        if (!pDscKeyPath->pidList[i].pidChannelHandle)
            return CSD_ERROR;

#if NEXUS_PLATFORM_97241 > NEXUS_PLATFORM_VERSION(12,4)
        if (NEXUS_KeySlot_AddPidChannel(pDscKeyPath->keyHandle, pDscKeyPath->pidList[i].pidChannelHandle) != NEXUS_SUCCESS)
#else
        NEXUS_PidChannel_GetStatus(pDscKeyPath->pidList[i].pidChannelHandle, &pidStatus);
        if (NEXUS_Security_AddPidChannelToKeySlot(pDscKeyPath->keyHandle, pidStatus.pidChannelIndex))
#endif
        {
            BDBG_ERR(("Fail to add pid to keyslot 0x%x\n", pDscKeyPath->keyHandle));
            return CSD_ERROR;
        }
    }

    /* for legacy reasons also save the physical pid channel */
    NEXUS_PidChannel_GetStatus(pDscKeyPath->pidList[pDscKeyPath->numberOfPIDS-1].pidChannelHandle, &pidStatus);
    pDscKeyPath->pidchannel = pidStatus.pidChannelIndex;

    return CSD_NO_ERROR;
}

TCsdStatus csdBrcmSetDscPid(TCsdDscKeyPathHandle* pDscKeyPath,
                            unsigned int xPid)
{
    unsigned int i=0;
    /* unused variable NEXUS_PidChannelStatus pidStatus;*/

    BDBG_MSG(("%s Enter", __FUNCTION__));

    if(!pDscKeyPath) {
        BDBG_ERR(("CSD invalid parameter"));
        return CSD_ERROR;
    }

    /* make sure CSD is open */
    if(!CSD_INITIALIZED) {
        BDBG_ERR(("CSD not initialized!"));
        return CSD_ERROR;
    }

    /* check if pid is already set*/
    for(i=0; i<pDscKeyPath->numberOfPIDS; i++) {
        if(pDscKeyPath->pidList[i].pidChannelHandle && (pDscKeyPath->pidList[i].PID == xPid)) {
            /* pid is already in the list */
            BDBG_ERR(("Pid 0x%x already added to this keypath", xPid));
            return CSD_ERROR;
        }
    }

    /* allocate pid */
    BDBG_MSG(("csdSetPid: opening pid channel for pid xPid=0x%x \n",xPid));
    if(pDscKeyPath->playpumpHandle) {
        BDBG_MSG(("csdSetPid: opening pid channel for pid xPid=0x%x on Playpump handle %p",xPid, pDscKeyPath->playpumpHandle));
        pDscKeyPath->pidList[pDscKeyPath->numberOfPIDS].pidChannelHandle = NEXUS_Playpump_OpenPidChannel(pDscKeyPath->playpumpHandle, xPid, NULL);
    } else {
        pDscKeyPath->pidList[pDscKeyPath->numberOfPIDS].pidChannelHandle = NEXUS_PidChannel_Open(pDscKeyPath->sessionId, xPid, NULL);
    }
    if(pDscKeyPath->pidList[pDscKeyPath->numberOfPIDS].pidChannelHandle == NULL)
    {
        BDBG_ERR(("Fail to find a pid channel: parserband=0x%x, pid=0x%x\n", pDscKeyPath->sessionId, xPid));
        return CSD_ERROR;
    } else {
#if NEXUS_PLATFORM_97241 > NEXUS_PLATFORM_VERSION(12,4)
#else
        NEXUS_PidChannelStatus pidStatus;
#endif
        pDscKeyPath->pidList[pDscKeyPath->numberOfPIDS].PID = xPid;
        /* add to keyslot */
        BDBG_MSG(("csdSetPid: parserband=0x%x, xPid=0x%x", 0, xPid));
#if NEXUS_PLATFORM_97241 > NEXUS_PLATFORM_VERSION(12,4)
        if ( NEXUS_KeySlot_AddPidChannel(pDscKeyPath->keyHandle, pDscKeyPath->pidList[pDscKeyPath->numberOfPIDS].pidChannelHandle) != NEXUS_SUCCESS )
#else
        NEXUS_PidChannel_GetStatus(pDscKeyPath->pidList[pDscKeyPath->numberOfPIDS].pidChannelHandle, &pidStatus);
        if (NEXUS_Security_AddPidChannelToKeySlot(pDscKeyPath->keyHandle, pidStatus.pidChannelIndex))
#endif
        {
            BDBG_ERR(("Fail to add pid 0x%x to keyslot 0x%x\n", xPid, pDscKeyPath->keyHandle));
            return CSD_ERROR;
        }
        /* for legacy reasons also save the physical pid channel */
        NEXUS_PidChannel_GetStatus(pDscKeyPath->pidList[pDscKeyPath->numberOfPIDS].pidChannelHandle, &pidStatus);
        pDscKeyPath->pidchannel = pidStatus.pidChannelIndex;
        BDBG_MSG(("csdBrcmSetDscPid pDscKeyPath->keyHandle 0x%x pDscKeyPath->pidChannelHandle 0x%x", pDscKeyPath->keyHandle, pDscKeyPath->pidList[pDscKeyPath->numberOfPIDS].pidChannelHandle));
        pDscKeyPath->numberOfPIDS++;
    }

    BDBG_MSG(("Dumping %d Pids:", pDscKeyPath->numberOfPIDS));
    for(i=0; i<pDscKeyPath->numberOfPIDS; i++) {
        BDBG_MSG(("Pid 0x%x",pDscKeyPath->pidList[i].PID));
    }

    BDBG_MSG(("%s Leave", __FUNCTION__));
    return CSD_NO_ERROR;
}

TCsdStatus csdBrcmRemoveDscPid(TCsdDscKeyPathHandle* pDscKeyPath,
                               unsigned int xPid)
{
    unsigned int i=0,p=0;
    BDBG_MSG(("%s Enter", __FUNCTION__));

    if(!pDscKeyPath) {
        BDBG_ERR(("CSD invalid parameter"));
        return CSD_ERROR;
    }

    /* make sure CSD is open */
    if(!CSD_INITIALIZED) {
        BDBG_ERR(("CSD not initialized!"));
        return CSD_ERROR;
    }

    /* check if pid is set*/
    for(i=0; i<pDscKeyPath->numberOfPIDS; i++) {
        if(pDscKeyPath->pidList[i].pidChannelHandle && (pDscKeyPath->pidList[i].PID == xPid)) {
            /* pid is in the list */
            BDBG_MSG(("Pid 0x%x in this keypath so removing pDscKeyPath->numberOfPIDS %d i %d", xPid, pDscKeyPath->numberOfPIDS, i));
            break;
        }
    }

    if(i<pDscKeyPath->numberOfPIDS) {
        /*free pid channel and remove from keyslot*/
        BDBG_MSG(("Removing Pid channel"));
#if NEXUS_PLATFORM_97241 > NEXUS_PLATFORM_VERSION(12,4)
        NEXUS_KeySlot_RemovePidChannel(pDscKeyPath->keyHandle,pDscKeyPath->pidList[i].pidChannelHandle);
#else
        NEXUS_PidChannelStatus pidStatus;
        NEXUS_PidChannel_GetStatus(pDscKeyPath->pidList[i].pidChannelHandle, &pidStatus);
        NEXUS_Security_RemovePidChannelFromKeySlot(pDscKeyPath->keyHandle, pidStatus.pidChannelIndex);
#endif
        if(pDscKeyPath->playpumpHandle) {
            BDBG_MSG(("Pid 0x%x removing from playpump", xPid));
            NEXUS_Playpump_ClosePidChannel(pDscKeyPath->playpumpHandle, pDscKeyPath->pidList[i].pidChannelHandle);
        } else {
            NEXUS_PidChannel_Close(pDscKeyPath->pidList[i].pidChannelHandle);
        }
        BDBG_MSG(("%s Freed Pid channel and removed pid from descrambler", __FUNCTION__));
        /* remove from list and shuffle other pids down(if needed); decrement count*/
        if((i+1)== pDscKeyPath->numberOfPIDS) {
            pDscKeyPath->pidList[i].PID = INVALID_PID;
            pDscKeyPath->pidList[i].pidChannelHandle = 0;
            pDscKeyPath->numberOfPIDS--;
            BDBG_MSG(("%s Removed Pid 0x%x from end of list", __FUNCTION__,xPid ));
        } else {
            for(p=i; p<(pDscKeyPath->numberOfPIDS-1); p++) {
                pDscKeyPath->pidList[p].PID = pDscKeyPath->pidList[p+1].PID;
                pDscKeyPath->pidList[p].pidChannelHandle = pDscKeyPath->pidList[p+1].pidChannelHandle;
                BDBG_MSG(("%s moved Pid 0x%x to position %d", __FUNCTION__, pDscKeyPath->pidList[p].PID, p));
            }
            pDscKeyPath->numberOfPIDS--;
            BDBG_MSG(("%s Removed Pid 0x%x from list", __FUNCTION__, xPid));
        }
    }

    BDBG_MSG(("Dumping %d Pids:", pDscKeyPath->numberOfPIDS));
    for(i=0; i<pDscKeyPath->numberOfPIDS; i++) {
        BDBG_MSG(("Pid 0x%x",pDscKeyPath->pidList[i].PID));
    }


    BDBG_MSG(("%s Leave", __FUNCTION__));
    return CSD_NO_ERROR;
}

/**********************************************************************************************
						 CSD Public Functions
**********************************************************************************************/
TCsdStatus csdInitialize(
    TCsdInitParameters*  pxInitParameters)
{
    int csd_index;

    /* Make sure input parameter pointer is not NULL. */
    if(pxInitParameters==NULL)
        return CSD_ERROR_INVALID_PARAMETERS;

    /* Find an available CSD. */
    for(csd_index=0; csd_index<CSD_MAX_INDEX; csd_index++)
    {
        if(csdInitParameters[csd_index] == NULL)
            break;
    }

    /* If no CSD is available return error. */
    if(csd_index==CSD_MAX_INDEX)
        return CSD_ERROR_DRIVER_ALREADY_INITIALIZED;

    csdInitParameters[csd_index]=pxInitParameters;

    /* initialize linked list structure for storing key handle */
    pxInitParameters->handleNodeCount = 0;
    pxInitParameters->rootHandleNode = NULL;

    CSD_INITIALIZED = TRUE;

    return CSD_NO_ERROR;

}

static TCsdStatus csdFreeHandleNode(
    TCsdInitParameters*  pxInitParameters,
    NEXUS_KeySlotHandle handle)
{
    handleNode curNode, prevNode;

    /* always start at root */
    curNode  = pxInitParameters->rootHandleNode;
    prevNode = pxInitParameters->rootHandleNode;

    /* Search through all nodes for the key slot handle we want to free. */
    while (curNode)
    {
        if (handle == curNode->handle)
            break;
        prevNode = curNode;
        curNode  = curNode->nextNode;
    }


    if (curNode)
    {
        /* found the node to release */
        NEXUS_Security_FreeKeySlot(handle);

        /* remove the node from the link */
        prevNode->nextNode = curNode->nextNode;
        if (curNode == pxInitParameters->rootHandleNode)
            pxInitParameters->rootHandleNode = curNode->nextNode;

        pxInitParameters->handleNodeCount -= 1;

        /* release the node storage */
        NEXUS_Memory_Free((void *)curNode);

    }
    else
        /* The linked list does not contain the requested handle, return error. */
        return CSD_ERROR_INVALID_PARAMETERS;

    return CSD_NO_ERROR;

}

#if (CSDAPI_VERSION_MEDIUM < 3)
static void csdInsertHandleNode(
    TCsdInitParameters*  pxInitParameters,
    NEXUS_KeySlotHandle handle)
{
    handleNode node;

    /* allocate the memory for the node storage */
    NEXUS_Memory_Allocate(sizeof(struct keyHandleNode), NULL, (void *)&node);
    node->handle = handle;

    /* always insert to the front */
    node->nextNode = pxInitParameters->rootHandleNode;
    pxInitParameters->rootHandleNode   = node;
    pxInitParameters->handleNodeCount += 1;

    return;

}
#endif

TCsdStatus csdTerminate(
    TCsdTerminateParameters*  pxTerminateParameters)
{
    handleNode node;
    handleNode nextNode;
    int csd_index;
    NEXUS_KeySlotHandle keyHandle;

    if( CSD_INITIALIZED == FALSE )
        return CSD_ERROR;

    if(pxTerminateParameters == NULL)
        return CSD_ERROR_INVALID_PARAMETERS;

    if(pxTerminateParameters->initParam == NULL)
        return CSD_ERROR_INVALID_PARAMETERS;

    /* Find the CSD to terminate. */
    for(csd_index=0; csd_index<CSD_MAX_INDEX; csd_index++)
    {
        if(csdInitParameters[csd_index] == pxTerminateParameters->initParam )
            break;
    }

    /* If CSD does not exist, then return error. */
    if(csd_index==CSD_MAX_INDEX)
        return CSD_ERROR_INVALID_PARAMETERS;

    /* first check to see if the request is to free a single keyslot handle */
    if (pxTerminateParameters->keyHandleToRelease)
    {
        /* Make sure there is at least 1 node. */
        if (pxTerminateParameters->initParam->handleNodeCount)
        {
            /* Remove the association between the PID channel and key slot. */
            /* keyslot is managed externally to the csd - as the keyslot is being removed the pid channel should be closed externally */
            keyHandle = pxTerminateParameters->keyHandleToRelease;
            /* NEXUS_KeySlot_RemovePidChannel(keyHandle, keyHandle->dmaPidChannel); */
            /* NEXUS_Security_RemovePidChannelFromKeySlot(pxTerminateParameters->keyHandleToRelease, pxTerminateParameters->keyHandleToRelease->pidChannel); */

            /* Free the node. */
            return csdFreeHandleNode(
                       pxTerminateParameters->initParam,
                       pxTerminateParameters->keyHandleToRelease);
        }
        else
            /* No node to free. */
            return CSD_ERROR_INVALID_PARAMETERS;

        return CSD_NO_ERROR;
    }

    /* If the input doesn't specify a single node to release, then release all. */
    if (pxTerminateParameters->initParam->handleNodeCount)
    {
        node = pxTerminateParameters->initParam->rootHandleNode;

        while (node)
        {
            nextNode = node->nextNode;

            /* Remove the association between the PID channel and key slot. */
            /* keyslot is managed externally to the csd - as the keyslot is being removed the pid channel should be closed externally */
            keyHandle = node->handle;
            /* NEXUS_KeySlot_RemovePidChannel(keyHandle, keyHandle->dmaPidChannel); */

            if(csdFreeHandleNode(
                        pxTerminateParameters->initParam,
                        node->handle) != CSD_NO_ERROR)
                return CSD_ERROR_INVALID_PARAMETERS;

            node = nextNode;
        }
    }

    csdInitParameters[csd_index]=NULL;

    /* After terminating CSD, check if any CSD is intiailized. */
    for(csd_index=0; csd_index<CSD_MAX_INDEX; csd_index++)
    {
        if(csdInitParameters[csd_index] != NULL )
            break;
    }

    /* If CSD does not exist, set CSD_INITIALIZED to false. */
    if(csd_index==CSD_MAX_INDEX)
        CSD_INITIALIZED = FALSE;

    return CSD_NO_ERROR;

}

TCsdStatus csdGetSoftwareVersion(
    TCsd20CharsString xSoftwareVersion)
{
#if 0
    static char SoftwareVersion[] = "NOCS-3.0.0-7231-1.0";
#endif
    if( CSD_INITIALIZED == FALSE )
        return CSD_ERROR;

    if(xSoftwareVersion==NULL)
        return CSD_ERROR;

#if 0
    BKNI_Memcpy(xSoftwareVersion, SoftwareVersion, sizeof(SoftwareVersion));
#else
    sprintf((char*)xSoftwareVersion, "v%01x.%01x.%01x", CSDAPI_VERSION_MAJOR, CSDAPI_VERSION_MEDIUM, CSDAPI_VERSION_MINOR);
#endif

    return CSD_NO_ERROR;
}

TCsdStatus csdGetChipRevision(
    TCsd20CharsString xChipRevision)
{

    unsigned char	chipVersion[2];
    uint32_t uChipNum;

    if( CSD_INITIALIZED == FALSE )
        return CSD_ERROR;

    if(xChipRevision ==NULL)
        return CSD_ERROR;


    /* "chip rev=71250020" */
    NEXUS_Platform_ReadRegister( BCHP_SUN_TOP_CTRL_PRODUCT_ID, &uChipNum );
    if((uChipNum>>16) == 0x0000)
    {
        NEXUS_Platform_ReadRegister( BCHP_SUN_TOP_CTRL_CHIP_FAMILY_ID, &uChipNum );
    }

    chipVersion[0] = 'A';
    chipVersion[1] = '0';

    uChipNum &= 0xff;
    chipVersion[0] += (uChipNum>>4);
    chipVersion[1] += (uChipNum & 0x0f);

    xChipRevision[0] = chipVersion[0];
    xChipRevision[1] = chipVersion[1];
	xChipRevision[2] = '\0';

    return CSD_NO_ERROR;

}

TCsdStatus csdGetNuid (
    TCsd4BytesVector xNuid)
{
    BERR_Code errCode = BERR_SUCCESS;
    NEXUS_ReadOtpIO	otpIo;
    unsigned int i;

    if( CSD_INITIALIZED == FALSE )
        return CSD_ERROR;

    if(xNuid ==NULL)
        return CSD_ERROR;

    /* Read the ID from OTP. */
    errCode = NEXUS_Security_ReadOTP (
                  NEXUS_OtpCmdReadRegister_eKeyID,
                  NEXUS_OtpKeyType_eA,
                  &otpIo);

    if (errCode != BERR_SUCCESS)
        return CSD_ERROR;

    /* NUID is the second Dword */
    BKNI_Memcpy((void *)xNuid, (void *)&(otpIo.otpKeyIdBuf[4]), CSD_NUID32_LENGTH/8 );

    for (i=0; i<(CSD_NUID32_LENGTH/8); i++)
        BDBG_MSG((" %02X ", xNuid[i])) ;

    return CSD_NO_ERROR;
}

TCsdStatus csdGetNUIDCheckNumber(
    TCsd4BytesVector xCheckNumber
)
{

    TCsdStatus      retCode = CSD_NO_ERROR;

    NEXUS_ReadOtpIO readOtpIO;
    NEXUS_OtpCmdReadRegister opCode = NEXUS_OtpCmdReadRegister_eKeyHash;
    NEXUS_OtpKeyType keyType = NEXUS_OtpKeyType_eA ;

    if( CSD_INITIALIZED == FALSE )
        return CSD_ERROR;

    if( xCheckNumber == NULL )
        /* Should have returned CSD_ERROR_INVALID_PARAMETERS.
        But Nagra API does NOT define CSD_ERROR_INVALID_PARAMETERS
        for this particular function */
        return CSD_ERROR;

    if(NEXUS_Security_ReadOTP(opCode, keyType, &readOtpIO)) {
        BDBG_ERR(("NEXUS_Security_ReadOTP error!\n"));
        return CSD_ERROR;
    }

#if 0
    printf("read Otp IO\n");
    for( i=0; i<readOtpIO.otpKeyIdSize; i++ )
        printf("0x%02x ", readOtpIO.otpKeyIdBuf[i]);
    printf("\n");
#endif



    /*
    	xCheckNumber[0] = readOtpIO.otpKeyIdBuf[0] ;
    	xCheckNumber[1] = readOtpIO.otpKeyIdBuf[1] ;
    	xCheckNumber[2] = readOtpIO.otpKeyIdBuf[2] ;
    	xCheckNumber[3] = readOtpIO.otpKeyIdBuf[3] ;
    */
    BKNI_Memcpy(xCheckNumber, readOtpIO.otpKeyIdBuf, sizeof(xCheckNumber));

    return (retCode);
}

TCsdStatus csdGetNuid64
(
    TCsd8BytesVector xNuid64
)
{
    BERR_Code errCode = BERR_SUCCESS;
    NEXUS_ReadOtpIO	otpIo;
    unsigned int i;

    if( CSD_INITIALIZED == FALSE )
        return CSD_ERROR;

    if(xNuid64 ==NULL)
        return CSD_ERROR;

    /* Read the ID from OTP. */
    errCode = NEXUS_Security_ReadOTP (
                  NEXUS_OtpCmdReadRegister_eKeyID,
                  NEXUS_OtpKeyType_eA,
                  &otpIo);

    if (errCode != BERR_SUCCESS)
        return CSD_ERROR;

    /* NUID is the first Dword */
    BKNI_Memcpy((void *)xNuid64, (void *)&(otpIo.otpKeyIdBuf[0]), CSD_NUID_LENGTH/8 );

    for (i=0; i<(CSD_NUID_LENGTH/8); i++)
        BDBG_MSG((" %02X ", xNuid64[i])) ;

    return CSD_NO_ERROR;
}

TCsdStatus csdGetCSCDCheckNumber(
    const TCsdUnsignedInt8		xCSCData[16],
    TCsd4BytesVector xCheckNumber
)
{
    TCsdR2RKeyPathHandle *r2rHandle;
    unsigned char *crcInput;
    unsigned char *xCSCDataInput;
    unsigned long uCSCDcheckNum;
    TCsdStatus  retCode = CSD_NO_ERROR;
    TCsdUnsignedInt16      xEmi = 0x4041;
    const TCsdUnsignedInt8 CONSTANTs[2][16] =
    {   {   0xA9, 0x32, 0x30, 0x31, 0x31, 0x4E, 0x61, 0x67,
            0x72, 0x61, 0x76, 0x69, 0x73, 0x69, 0x6f, 0x6e
        },
        {   0xA9, 0x32, 0x30, 0x31, 0x31, 0x4E, 0x61, 0x67,
            0x72, 0x61, 0x76, 0x69, 0x73, 0x69, 0x6f, 0x6e
        },
    };

    const TCsdUnsignedInt8 CONSTANT_2[16] = {0xA9, 0x32, 0x30, 0x31, 0x31, 0x4E, 0x61, 0x67,
                                            0x72, 0x61, 0x76, 0x69, 0x73, 0x69, 0x6f, 0x6e
                                            };



    if( CSD_INITIALIZED == FALSE )
        return CSD_ERROR;

    if( (xCheckNumber == NULL) || (xCSCData==NULL) )
        return CSD_ERROR_INVALID_PARAMETERS;

    /* Need to allocate memory for M2M input and output buffers. */
    NEXUS_Memory_Allocate(sizeof(unsigned char) * 16, NULL, (void *)&xCSCDataInput);
    NEXUS_Memory_Allocate(sizeof(unsigned char) * 16, NULL, (void *)&crcInput);

    retCode = csdBrcmOpenR2RKeyPath(&r2rHandle, 0);
    if (CSD_NO_ERROR != retCode)
    {
        BDBG_ERR(("csdBrcmOpenR2RKeyPath failed"));
        goto CSD_P_DONE;
    }

    /* Copy the xCSCData to M2M input buffer. */
    BKNI_Memcpy((void *)xCSCDataInput, (void *)xCSCData, 16 );

#ifdef CSD_DEBUG
    printf("xCSCData[16]\n");
    for( i=0; i<16; i++ )
        printf("0x%02x ", xCSCDataInput[i]);
    printf("\n");
#endif

    /* HUMAX_PLATFORM_BASE */
    r2rHandle->bEncrypt = true;

    if(csdSetProtectedRam2RamContentKeys(xEmi,r2rHandle,CONSTANTs,CONSTANT_2, 16)) {
        BDBG_ERR(("R2R csdSetProtectedRam2RamContentKeys returns error"));
        retCode = CSD_ERROR;
        goto CSD_P_DONE;
    }

    if (CSD_NO_ERROR != csdEncryptData(xEmi, r2rHandle, NULL, 0, xCSCDataInput, crcInput, 16/*inputVector.sizeDataIn*/))
    {
        BDBG_ERR(("R2R csdEncryptData returns error"));
        retCode = CSD_ERROR;
        goto CSD_P_DONE;
    }

#ifdef CSD_DEBUG
    printf("crcInput\n");
    for( i=0; i<16; i++ )
        printf("0x%02x ", crcInput[i]);
    printf("\n");
#endif

    genCrc32Tab();
    uCSCDcheckNum= cmpCrc32(crcInput,16) ;

#ifdef CSD_DEBUG
    printf("CSC Data checkNum=0x%08x", uCSCDcheckNum);
#endif

    xCheckNumber[3] = uCSCDcheckNum&0xff;
    xCheckNumber[2] = (uCSCDcheckNum>>8) &0xff;
    xCheckNumber[1] = (uCSCDcheckNum>>16) &0xff;
    xCheckNumber[0] = (uCSCDcheckNum>>24) &0xff;

CSD_P_DONE:
    if (NULL != r2rHandle)
    {
        if (CSD_NO_ERROR != csdBrcmCloseR2RKeyPath(r2rHandle))
        {
            /* Don't lose a pre-existing error code */
            if (retCode == CSD_NO_ERROR)
              retCode = CSD_ERROR;
        }
    }

    NEXUS_Memory_Free(xCSCDataInput);
    NEXUS_Memory_Free(crcInput);

    return retCode;
}

TCsdStatus csdGetSTBCASNCheckNumber(
    const TCsdUnsignedInt8	 xSTBCASNData[4],
    TCsd4BytesVector xSTBCASNCheckNumber
)
{
    TCsdR2RKeyPathHandle * r2rHandle;
    unsigned char *crcInput;
    unsigned char *xSTBCASNDataInput;
    unsigned long uSTBCASNcheckNum;
    TCsdStatus  retCode = CSD_NO_ERROR;
    TCsdUnsignedInt16      xEmi = 0x4041;
    const TCsdUnsignedInt8 CONSTANTs[2][16] =
    {   {   0xA9, 0x32, 0x30, 0x31, 0x31, 0x4E, 0x61, 0x67,
            0x72, 0x61, 0x76, 0x69, 0x73, 0x69, 0x6f, 0x6e
        },
        {   0xA9, 0x32, 0x30, 0x31, 0x31, 0x4E, 0x61, 0x67,
            0x72, 0x61, 0x76, 0x69, 0x73, 0x69, 0x6f, 0x6e
        },
    };

    const TCsdUnsignedInt8 CONSTANT_2[16] = {0xA9, 0x32, 0x30, 0x31, 0x31, 0x4E, 0x61, 0x67,
                                            0x72, 0x61, 0x76, 0x69, 0x73, 0x69, 0x6f, 0x6e
                                            };


    if( CSD_INITIALIZED == FALSE )
        return CSD_ERROR;

    if( (xSTBCASNCheckNumber == NULL ) || (xSTBCASNData==NULL) )
        return CSD_ERROR_INVALID_PARAMETERS;

#ifdef CSD_DEBUG
    printf("xSTBCASNData[16]\n");
    for( i=0; i<4; i++ )
        printf("0x%02x ", xSTBCASNData[i]);
    printf("\n");
#endif


    /* Need to allocate memory for M2M input and output buffers. */
    NEXUS_Memory_Allocate(sizeof(unsigned char) * 16, NULL, (void *)&xSTBCASNDataInput);
    NEXUS_Memory_Allocate(sizeof(unsigned char) * 16, NULL, (void *)&crcInput);

    retCode = csdBrcmOpenR2RKeyPath(&r2rHandle, 0);
    if (CSD_NO_ERROR != retCode)
    {
        BDBG_ERR(("csdBrcmOpenR2RKeyPath failed"));
        goto CSD_P_DONE;
    }

    /* Initialize data buffer to zero. */
    memset(xSTBCASNDataInput, 0, sizeof(unsigned char) * 16 );

    /* Copy the xSTBCASNData to M2M input buffer. */
    BKNI_Memcpy((void *)&(xSTBCASNDataInput[12]), (void *)xSTBCASNData, 4 );

#ifdef CSD_EBGU
    printf("padded r2rDataIn[16]\n");
    for( i=0; i<16; i++ )
        printf("0x%02x ", xSTBCASNDataInput[i]);
    printf("\n");
#endif

    if(csdSetProtectedRam2RamContentKeys(xEmi,r2rHandle,CONSTANTs,CONSTANT_2, 16)) {
        BDBG_ERR(("R2R csdSetProtectedRam2RamContentKeys returns error "));
        goto CSD_P_DONE;
    }

    if (CSD_NO_ERROR != csdEncryptData(xEmi, r2rHandle, NULL, 0, xSTBCASNDataInput, crcInput, 16/*inputVector.sizeDataIn*/))
    {
        BDBG_ERR(("R2R csdEncryptData returns error"));
        retCode = CSD_ERROR;
        goto CSD_P_DONE;
    }


#ifdef CSD_DEBUG
    printf("crcInput\n");
    for( i=0; i<16; i++ )
        printf("0x%02x ", crcInput[i]);
    printf("\n");
#endif

    genCrc32Tab();
    uSTBCASNcheckNum= cmpCrc32(crcInput,16) ;

#ifdef CSD_DEBUG
    printf("STBCASN checkNum=0x%08x", uSTBCASNcheckNum);
#endif

    xSTBCASNCheckNumber[3] = uSTBCASNcheckNum&0xff;
    xSTBCASNCheckNumber[2] = (uSTBCASNcheckNum>>8) &0xff;
    xSTBCASNCheckNumber[1] = (uSTBCASNcheckNum>>16) &0xff;
    xSTBCASNCheckNumber[0] = (uSTBCASNcheckNum>>24) &0xff;

CSD_P_DONE:
    if (NULL != r2rHandle)
    {
        if (CSD_NO_ERROR != csdBrcmCloseR2RKeyPath(r2rHandle))
        {
            /* Don't lose a pre-existing error code */
            if (retCode == CSD_NO_ERROR)
              retCode = CSD_ERROR;
        }
    }

    NEXUS_Memory_Free(xSTBCASNDataInput);
    NEXUS_Memory_Free(crcInput);

    return retCode;
}

TCsdStatus csdGetDataIntegrityCheckNumber(
    const TCsdUnsignedInt8*		pxData,
    TCsdSize						xDataSize,
    TCsd4BytesVector 				xDataIntegrityCheckNumber
)
{
    TCsdR2RKeyPathHandle * r2rHandle;
    unsigned char *r2rDataIn;
    unsigned char *crcInput;
    unsigned long uDataIntegrityCheckNum;
    TCsdStatus  retCode = CSD_NO_ERROR;
    TCsdUnsignedInt16      xEmi = 0x4041;
    const TCsdUnsignedInt8 CONSTANTs[2][16] =
    {   {   0xA9, 0x32, 0x30, 0x31, 0x31, 0x4E, 0x61, 0x67,
            0x72, 0x61, 0x76, 0x69, 0x73, 0x69, 0x6f, 0x6e
        },
        {   0xA9, 0x32, 0x30, 0x31, 0x31, 0x4E, 0x61, 0x67,
            0x72, 0x61, 0x76, 0x69, 0x73, 0x69, 0x6f, 0x6e
        },
    };

    const TCsdUnsignedInt8 CONSTANT_2[16] = {0xA9, 0x32, 0x30, 0x31, 0x31, 0x4E, 0x61, 0x67,
                                            0x72, 0x61, 0x76, 0x69, 0x73, 0x69, 0x6f, 0x6e
                                            };

    if( CSD_INITIALIZED == FALSE )
        return CSD_ERROR;

    if( xDataSize >16 || xDataSize < 1 )
        return CSD_ERROR_INVALID_PARAMETERS;

    if( ( xDataIntegrityCheckNumber == NULL ) || (pxData==NULL) )
        return CSD_ERROR_INVALID_PARAMETERS;

#ifdef CSD_DEBUG
    printf("pxData\n");
    for( i=0; i<xDataSize; i++ )
        printf("0x%02x ", pxData[i]);
    printf("\n");
#endif

    /* Need to allocate memory for M2M input and output buffers. */
    NEXUS_Memory_Allocate(sizeof(unsigned char) * 16, NULL, (void *)&r2rDataIn);
    NEXUS_Memory_Allocate(sizeof(unsigned char) * 16, NULL, (void *)&crcInput);

    retCode = csdBrcmOpenR2RKeyPath(&r2rHandle, 0);
    if (CSD_NO_ERROR != retCode)
    {
        BDBG_ERR(("csdBrcmOpenR2RKeyPath failed"));
        goto CSD_P_DONE;
    }

    /* Initialize data buffer to zero. */
    memset(r2rDataIn, 0, sizeof(unsigned char) * 16 );

    /* Copy the xDataSize to M2M input buffer. */
    BKNI_Memcpy((void *)&(r2rDataIn[16-xDataSize]), (void *)pxData, xDataSize );

#ifdef CSD_DEBUG
    printf("padded r2rDataIn[16]\n");
    for( i=0; i<16; i++ )
        printf("0x%02x ", r2rDataIn[i]);
    printf("\n");
#endif

    if(csdSetProtectedRam2RamContentKeys(xEmi,r2rHandle,CONSTANTs,CONSTANT_2, 16)) {
        BDBG_ERR(("R2R csdSetProtectedRam2RamContentKeys returns error "));
        retCode = CSD_ERROR;
        goto CSD_P_DONE;
    }

    if (CSD_NO_ERROR != csdEncryptData(xEmi, r2rHandle, NULL, 0, r2rDataIn, crcInput, 16/*inputVector.sizeDataIn*/))
    {
        BDBG_ERR(("R2R csdEncryptData returns error"));
        retCode = CSD_ERROR;
        goto CSD_P_DONE;
    }


#ifdef CSD_DEBUG
    printf("crcInput\n");
    for( i=0; i<16; i++ )
        printf("0x%02x ", crcInput[i]);
    printf("\n");
#endif

    genCrc32Tab();
    uDataIntegrityCheckNum= cmpCrc32(crcInput,16) ;

#ifdef CSD_DEBUG
    printf("Data Integrity checkNum=0x%08x", uDataIntegrityCheckNum);
#endif
    xDataIntegrityCheckNumber[3] = uDataIntegrityCheckNum&0xff;
    xDataIntegrityCheckNumber[2] = (uDataIntegrityCheckNum>>8) &0xff;
    xDataIntegrityCheckNumber[1] = (uDataIntegrityCheckNum>>16) &0xff;
    xDataIntegrityCheckNumber[0] = (uDataIntegrityCheckNum>>24) &0xff;

CSD_P_DONE:
    if (NULL != r2rHandle)
    {
        if (CSD_NO_ERROR != csdBrcmCloseR2RKeyPath(r2rHandle))
        {
            /* Don't lose a pre-existing error code */
            if (retCode == CSD_NO_ERROR)
              retCode = CSD_ERROR;
        }
    }

    NEXUS_Memory_Free(r2rDataIn);
    NEXUS_Memory_Free(crcInput);


    return (retCode);
}


#if 1
TCsdStatus csdSetBootMode(
    TCsdBootMode xBootMode)
{
    CSD_UNUSED(xBootMode);
    return CSD_ERROR_OPERATION_NOT_SUPPORTED;
}
#else
TCsdStatus csdSetBootMode(
    TCsdBootMode xBootMode)
{
    uint32_t ubootStrapReg;
    TCsdBootMode strapBootMode;

    NEXUS_Platform_ReadRegister( BCHP_SUN_TOP_CTRL_STRAP_VALUE_0, &ubootStrapReg);

    ubootStrapReg &= BCHP_SUN_TOP_CTRL_STRAP_VALUE_0_strap_boot_shape_MASK;
    ubootStrapReg = ubootStrapReg>>BCHP_SUN_TOP_CTRL_STRAP_VALUE_0_strap_boot_shape_SHIFT;

    if(ubootStrapReg <7 )
    {
        strapBootMode = CSD_BOOT_FROM_NAND;
    }
    else if(ubootStrapReg <10 )
    {
        strapBootMode = CSD_BOOT_FROM_SPI;
    }
    else if(ubootStrapReg <12 )
    {
        strapBootMode = CSD_BOOT_FROM_NOR;
    }
    else
    {
        strapBootMode = CSD_BOOT_MODE_MAX;
    }

    if(xBootMode != strapBootMode)
        return CSD_ERROR_OPERATION_NOT_ALLOWED;


    return CSD_NO_ERROR;
}
#endif


TCsdStatus csdGetBootMode(
    TCsdBootMode* pxBootMode)
{
    uint32_t ubootStrapReg;
    TCsdBootMode strapBootMode;

    if ( pxBootMode==NULL )
        return CSD_ERROR_INVALID_PARAMETERS;

    NEXUS_Platform_ReadRegister( BCHP_SUN_TOP_CTRL_STRAP_VALUE_0, &ubootStrapReg);

    ubootStrapReg &= BCHP_SUN_TOP_CTRL_STRAP_VALUE_0_strap_boot_shape_MASK;
    ubootStrapReg = ubootStrapReg>>BCHP_SUN_TOP_CTRL_STRAP_VALUE_0_strap_boot_shape_SHIFT;

    if(ubootStrapReg <7 )
    {
        strapBootMode = CSD_BOOT_FROM_NAND;
    }
    else if(ubootStrapReg <10 )
    {
        strapBootMode = CSD_BOOT_FROM_SPI;
    }
    else if(ubootStrapReg <12 )
    {
        strapBootMode = CSD_BOOT_FROM_NOR;
    }
    else
    {
        *pxBootMode = CSD_BOOT_FROM_UNKNOWN;
        return CSD_ERROR;
    }

    /* Store boot mode. */
    *pxBootMode = strapBootMode;

    return CSD_NO_ERROR;
}

TCsdStatus csdEnableScs(void)
{
    NEXUS_ReadMspParms readMspParams;
    NEXUS_ProgramMspIO progMspIO;
    NEXUS_ReadMspIO readMspIO;

    if( CSD_INITIALIZED == FALSE )
        return CSD_ERROR;

    readMspParams.readMspEnum    = NEXUS_OtpCmdMsp_eSecureBootEnable;
    if(NEXUS_Security_ReadMSP (&readMspParams, &readMspIO))
        return CSD_ERROR;

    if(readMspIO.mspDataBuf[3]==0x0)
    {
        progMspIO.progMode       = NEXUS_OTP_DATASECTIONPROG_MODE;
        progMspIO.progMspEnum    = NEXUS_OtpCmdMsp_eSecureBootEnable;
        progMspIO.dataBitLen     = 1;
        progMspIO.dataBitMask[0] = 0x0;
        progMspIO.dataBitMask[1] = 0x0;
        progMspIO.dataBitMask[2] = 0x0;
        progMspIO.dataBitMask[3] = 0x01;
        progMspIO.mspData[0]     = 0x0;
        progMspIO.mspData[1]     = 0x0;
        progMspIO.mspData[2]     = 0x0;
        progMspIO.mspData[3]     = 0x1;

        /* Issue command to program the OTP. */
        if(NEXUS_Security_ProgramMSP (&progMspIO))
            return CSD_ERROR;

        readMspParams.readMspEnum =  NEXUS_OtpCmdMsp_eSecureBootEnable;

        if(NEXUS_Security_ReadMSP (&readMspParams, &readMspIO))
            return CSD_ERROR;

        if(readMspIO.mspDataBuf[3] != 0x1)
            return CSD_ERROR;
    }

    return CSD_NO_ERROR;

}

TCsdStatus csdGetScsActivationFlag(
    TCsdActivationFlag* pxScsActivationFlagState)
{
    NEXUS_ReadMspParms	readMspParams;
    NEXUS_ReadMspIO readMspIO;

    if( CSD_INITIALIZED == FALSE )
        return CSD_ERROR;

    if(pxScsActivationFlagState ==NULL)
        return CSD_ERROR;

    *pxScsActivationFlagState = LAST_CSD_ACTIVATION_FLAG;

    readMspParams.readMspEnum = NEXUS_OtpCmdMsp_eSecureBootEnable;

    if(NEXUS_Security_ReadMSP (&readMspParams, &readMspIO))
        return CSD_ERROR;

    if(readMspIO.mspDataBuf[3]==0x1)
        *pxScsActivationFlagState = CSD_ACTIVATION_FLAG_SET;
    else
        *pxScsActivationFlagState = CSD_ACTIVATION_FLAG_NOT_SET;

    return CSD_NO_ERROR;
}


TCsdStatus csdSetStbCaSn(
    const TCsd4BytesVector xStbCaSn
)
{
    NEXUS_ReadMspParms readMspParams;
    NEXUS_ProgramMspIO progMspIO;
    NEXUS_ReadMspIO readMspIO;
    unsigned char zeroArray[4] = {0, 0, 0, 0};

    if( CSD_INITIALIZED == FALSE )
        return CSD_ERROR;

    if( xStbCaSn == NULL )
        return CSD_ERROR;

    readMspParams.readMspEnum = NEXUS_OtpCmdMsp_eReserved104;
    if(NEXUS_Security_ReadMSP (&readMspParams, &readMspIO))
        return CSD_ERROR;

    BDBG_MSG(("STB CA Serial Number ID  = 0x%x%x%x%x\n",
              readMspIO.mspDataBuf[0],
              readMspIO.mspDataBuf[1],
              readMspIO.mspDataBuf[2],
              readMspIO.mspDataBuf[3]));

    if((memcmp(xStbCaSn, readMspIO.mspDataBuf, sizeof(unsigned char) * 4)) == 0 )
        return CSD_NO_ERROR;

    if((memcmp(zeroArray, readMspIO.mspDataBuf, sizeof(unsigned char) * 4) != 0) &&
            (memcmp(xStbCaSn, readMspIO.mspDataBuf, sizeof(unsigned char) * 4) != 0))
        return CSD_ERROR_OPERATION_NOT_ALLOWED;

    if(memcmp(zeroArray, readMspIO.mspDataBuf, sizeof(unsigned char) * 4) == 0)
    {

        progMspIO.progMode = NEXUS_OTP_DATASECTIONPROG_MODE;
        progMspIO.progMspEnum = NEXUS_OtpCmdMsp_eReserved104;
        progMspIO.dataBitLen = 32;
        progMspIO.dataBitMask[0] = 0xff;
        progMspIO.dataBitMask[1] = 0xff;
        progMspIO.dataBitMask[2] = 0xff;
        progMspIO.dataBitMask[3] = 0xff;

        progMspIO.mspData[0]     = xStbCaSn[0];
        progMspIO.mspData[1]     = xStbCaSn[1];
        progMspIO.mspData[2]     = xStbCaSn[2];
        progMspIO.mspData[3]     = xStbCaSn[3];

        /* Issue command to program the OTP. */
        if(NEXUS_Security_ProgramMSP (&progMspIO))
            return CSD_ERROR;

        readMspParams.readMspEnum    = NEXUS_OtpCmdMsp_eReserved104;
        if(NEXUS_Security_ReadMSP (&readMspParams, &readMspIO))
            return CSD_ERROR;

        BDBG_MSG(("STB CA Serial Number  = 0x%x%x%x%x\n",
                  readMspIO.mspDataBuf[0],
                  readMspIO.mspDataBuf[1],
                  readMspIO.mspDataBuf[2],
                  readMspIO.mspDataBuf[3]));

        if(memcmp(progMspIO.mspData, xStbCaSn, sizeof(unsigned char) * 4) != 0)
            return CSD_ERROR;
    }

    return CSD_NO_ERROR;
}


TCsdStatus csdGetStbCaSn(
    TCsd4BytesVector xStbCaSn
)
{

    NEXUS_ReadMspParms		readMspParams;
    NEXUS_ReadMspIO readMspIO;

    if( CSD_INITIALIZED == FALSE )
        return CSD_ERROR;

    if( xStbCaSn == NULL )
        return CSD_ERROR;

    readMspParams.readMspEnum = NEXUS_OtpCmdMsp_eReserved104;
    if(NEXUS_Security_ReadMSP (&readMspParams, &readMspIO))
        return CSD_ERROR;

    BDBG_MSG(("STB CA Serial Number  = 0x%x%x%x%x\n",
              readMspIO.mspDataBuf[0],
              readMspIO.mspDataBuf[1],
              readMspIO.mspDataBuf[2],
              readMspIO.mspDataBuf[3]));

    BKNI_Memcpy(xStbCaSn, readMspIO.mspDataBuf, sizeof(unsigned char) * 4);

    return CSD_NO_ERROR;
}

TCsdStatus csdSetMarketSegmentId(
    const TCsd4BytesVector xMarketSegmentId
)
{
    NEXUS_ReadMspParms		readMspParams;
    NEXUS_ProgramMspIO	progMspIO;
    NEXUS_ReadMspIO readMspIO;
    unsigned char zeroArray[4] = {0, 0, 0, 0};

    if( CSD_INITIALIZED == FALSE )
        return CSD_ERROR;

    if( xMarketSegmentId == NULL )
        return CSD_ERROR;

    readMspParams.readMspEnum = NEXUS_OtpCmdMsp_eMarketId;
    if(NEXUS_Security_ReadMSP (&readMspParams, &readMspIO))
        return CSD_ERROR;

    BDBG_MSG(("Market ID  = 0x%x%x%x%x\n",
              readMspIO.mspDataBuf[0],
              readMspIO.mspDataBuf[1],
              readMspIO.mspDataBuf[2],
              readMspIO.mspDataBuf[3]));

    if((memcmp(xMarketSegmentId, readMspIO.mspDataBuf, sizeof(unsigned char) * 4)) == 0 )
        return CSD_NO_ERROR;

    if((memcmp(zeroArray, readMspIO.mspDataBuf, sizeof(unsigned char) * 4) != 0) &&
            (memcmp(xMarketSegmentId, readMspIO.mspDataBuf, sizeof(unsigned char) * 4) != 0))
        return CSD_ERROR_OPERATION_NOT_ALLOWED;

    if(memcmp(zeroArray, readMspIO.mspDataBuf, sizeof(unsigned char) * 4) == 0)
    {

        progMspIO.progMode       = NEXUS_OTP_DATASECTIONPROG_MODE;
        progMspIO.progMspEnum    = NEXUS_OtpCmdMsp_eMarketId;
        progMspIO.dataBitLen     = 32;
        progMspIO.dataBitMask[0] = 0xff;
        progMspIO.dataBitMask[1] = 0xff;
        progMspIO.dataBitMask[2] = 0xff;
        progMspIO.dataBitMask[3] = 0xff;

        progMspIO.mspData[0]     = xMarketSegmentId[0];
        progMspIO.mspData[1]     = xMarketSegmentId[1];
        progMspIO.mspData[2]     = xMarketSegmentId[2];
        progMspIO.mspData[3]     = xMarketSegmentId[3];

        /* Issue command to program the OTP. */
        if(NEXUS_Security_ProgramMSP (&progMspIO))
            return CSD_ERROR;

        readMspParams.readMspEnum    = NEXUS_OtpCmdMsp_eMarketId;
        if(NEXUS_Security_ReadMSP (&readMspParams, &readMspIO))
            return CSD_ERROR;

        BDBG_MSG(("Market ID  = 0x%x%x%x%x\n",
                  readMspIO.mspDataBuf[0],
                  readMspIO.mspDataBuf[1],
                  readMspIO.mspDataBuf[2],
                  readMspIO.mspDataBuf[3]));

        if(memcmp(progMspIO.mspData, xMarketSegmentId, sizeof(unsigned char) * 4) != 0)
            return CSD_ERROR;
    }

    return CSD_NO_ERROR;
}


TCsdStatus csdGetMarketSegmentId(
    TCsd4BytesVector xMarketSegmentId
)
{
    NEXUS_ReadMspParms		readMspParams;
    NEXUS_ReadMspIO readMspIO;

    if( CSD_INITIALIZED == FALSE )
        return CSD_ERROR;

    if( xMarketSegmentId == NULL )
        return CSD_ERROR;

    readMspParams.readMspEnum    = NEXUS_OtpCmdMsp_eMarketId;
    if(NEXUS_Security_ReadMSP (&readMspParams, &readMspIO))
        return CSD_ERROR;

    BDBG_MSG(("Market ID  = 0x%x%x%x%x\n",
              readMspIO.mspDataBuf[0],
              readMspIO.mspDataBuf[1],
              readMspIO.mspDataBuf[2],
              readMspIO.mspDataBuf[3]));

    BKNI_Memcpy(xMarketSegmentId, readMspIO.mspDataBuf, sizeof(unsigned char) * 4);

    return CSD_NO_ERROR;
}

TCsdStatus csdGetDebugInterfaceProtectionLevel(
    TCsdDebugInterfaceAccessMode* pxDebugInterfaceProtectionLevel
)
{
    NEXUS_ReadMspParms	readMspParams;
    NEXUS_ReadMspIO readMspIO;

    if( CSD_INITIALIZED == FALSE )
        return CSD_ERROR;

    if( pxDebugInterfaceProtectionLevel == NULL )
        return CSD_ERROR;

    readMspParams.readMspEnum    = NEXUS_OtpCmdMsp_eCrDisable;

    if(NEXUS_Security_ReadMSP (&readMspParams, &readMspIO))
        return CSD_ERROR;

    BDBG_MSG(( "Cr bit 1  = %x\n", readMspIO.mspDataBuf[3]));
    if(readMspIO.mspDataBuf[3]==0x0)
    {
        readMspParams.readMspEnum = NEXUS_OtpCmdMsp_eCrLockEnable;
        if(NEXUS_Security_ReadMSP (&readMspParams, &readMspIO))
            return CSD_ERROR;

        BDBG_MSG(("Cr bit 0  = %x\n", readMspIO.mspDataBuf[3]));

        if(readMspIO.mspDataBuf[3]==0x1)
            *pxDebugInterfaceProtectionLevel = CSD_DEBUG_INTERFACE_ACCESS_MODE_PASSWORD_PROTECTED;
        else
            *pxDebugInterfaceProtectionLevel = CSD_DEBUG_INTERFACE_ACCESS_MODE_OPEN;
    }
    else
        *pxDebugInterfaceProtectionLevel = CSD_DEBUG_INTERFACE_ACCESS_MODE_CLOSED;

    return CSD_NO_ERROR;

}


TCsdStatus csdSelectDebugInterfaceProtectionLevel(
    TCsdDebugInterfaceAccessMode xDebugInterfaceProtectionLevel)
{

    NEXUS_ProgramMspIO	progMspIO;
    TCsdDebugInterfaceAccessMode xState;

    if( CSD_INITIALIZED == FALSE )
        return CSD_ERROR;

    if(csdGetDebugInterfaceProtectionLevel(&xState) != CSD_NO_ERROR)
    {
        BDBG_ERR(("Error getting JTAG state!\n"));
        return CSD_ERROR;
    }

    if(xState == xDebugInterfaceProtectionLevel)
    {
        BDBG_WRN(("Already in this mode\n"));
        return CSD_NO_ERROR;
    }

    else if(xState > xDebugInterfaceProtectionLevel)
    {
        BDBG_ERR(("Error: operation not allowed\n"));
        return CSD_ERROR_OPERATION_NOT_ALLOWED;
    }

    progMspIO.progMode       = NEXUS_OTP_DATASECTIONPROG_MODE;
    progMspIO.dataBitLen     = 1;
    progMspIO.dataBitMask[0] = 0x0;
    progMspIO.dataBitMask[1] = 0x0;
    progMspIO.dataBitMask[2] = 0x0;
    progMspIO.dataBitMask[3] = 0x01;
    progMspIO.mspData[0]     = 0x0;
    progMspIO.mspData[1]     = 0x0;
    progMspIO.mspData[2]     = 0x0;
    progMspIO.mspData[3]     = 0x1;

    switch(xDebugInterfaceProtectionLevel)
    {
        case CSD_DEBUG_INTERFACE_ACCESS_MODE_PASSWORD_PROTECTED:
            progMspIO.progMspEnum = NEXUS_OtpCmdMsp_eCrLockEnable;
            if(NEXUS_Security_ProgramMSP (&progMspIO))
                return CSD_ERROR;
            break;
        case CSD_DEBUG_INTERFACE_ACCESS_MODE_CLOSED:
#ifdef VER_ZEUS22
            progMspIO.progMode = NEXUS_OTP_DATASECTIONPROG_MODE;
            progMspIO.dataBitLen     = 4;
            progMspIO.dataBitMask[0] = 0x0;
            progMspIO.dataBitMask[1] = 0x0;
            progMspIO.dataBitMask[2] = 0x0;
            progMspIO.dataBitMask[3] = 0xF;
            progMspIO.mspData[0]     = 0x0;
            progMspIO.mspData[1]     = 0x0;
            progMspIO.mspData[2]     = 0x0;
            progMspIO.mspData[3]     = 0xF;
            progMspIO.progMspEnum = NEXUS_OtpCmdMsp_eReserved201;
            if(NEXUS_Security_ProgramMSP (&progMspIO))
                return CSD_ERROR;

            progMspIO.progMode       = NEXUS_OTP_DATASECTIONPROG_MODE;
            progMspIO.dataBitLen     = 1;
            progMspIO.dataBitMask[0] = 0x0;
            progMspIO.dataBitMask[1] = 0x0;
            progMspIO.dataBitMask[2] = 0x0;
            progMspIO.dataBitMask[3] = 0x01;
            progMspIO.mspData[0]     = 0x0;
            progMspIO.mspData[1]     = 0x0;
            progMspIO.mspData[2]     = 0x0;
            progMspIO.mspData[3]     = 0x1;

            progMspIO.progMspEnum = NEXUS_OtpCmdMsp_eCrDisable;
            if(NEXUS_Security_ProgramMSP (&progMspIO))
                return CSD_ERROR;
            /* Need to reset board to get the correct state */

            break;


#else
            progMspIO.progMspEnum = NEXUS_OtpCmdMsp_eCrDisable;

            if(NEXUS_Security_ProgramMSP (&progMspIO))
                return CSD_ERROR;

            /* need to reset board to get the correct state
            readMspIO.readMspEnum = BCMD_Otp_CmdMsp_eCrOtpBit1;
            BHSM_ReadMSP (hCsdInitParameters->hHsmCsd, &readMspIO);
            if(readMspIO.aucMspData[3]) result= CSD_NO_ERROR;
            else result = CSD_ERROR_OPERATION_FAILED;
            */
            break;
#endif

        default:
            return CSD_ERROR;
            break;

    }

    return CSD_NO_ERROR;

}

TCsdStatus csdDeactivateKeyLadder(void)
{
    NEXUS_ReadMspParms readMspParams;
    NEXUS_ProgramMspIO progMspIO;
    NEXUS_ReadMspIO readMspIO;

    if(CSD_INITIALIZED == FALSE )
        return CSD_ERROR;

#ifdef VER_ZEUS22
    readMspParams.readMspEnum = NEXUS_OtpCmdMsp_eReserved194;
    if(NEXUS_Security_ReadMSP (&readMspParams, &readMspIO))
        return CSD_ERROR;
    if(readMspIO.mspDataBuf[3] == 0)
    {
        progMspIO.progMode = NEXUS_OTP_DATASECTIONPROG_MODE;
        progMspIO.progMspEnum = NEXUS_OtpCmdMsp_eReserved194;
        progMspIO.dataBitLen = 1;
        progMspIO.dataBitMask[0] = 0x00;
        progMspIO.dataBitMask[1] = 0x00;
        progMspIO.dataBitMask[2] = 0x00;
        progMspIO.dataBitMask[3] = 0x01;
        progMspIO.mspData[0] = 0x00;
        progMspIO.mspData[1] = 0x00;
        progMspIO.mspData[2] = 0x00;
        progMspIO.mspData[3] = 0x01;
        if(NEXUS_Security_ProgramMSP (&progMspIO))
            return CSD_ERROR;
        readMspParams.readMspEnum = NEXUS_OtpCmdMsp_eReserved194;
        if(NEXUS_Security_ReadMSP (&readMspParams, &readMspIO))
            return CSD_ERROR;
        if(progMspIO.mspData[3] != 0x1)
            return CSD_ERROR;
    }
#else
    readMspParams.readMspEnum = NEXUS_OtpCmdMsp_eCaKeyLadderDisable;
    if(NEXUS_Security_ReadMSP (&readMspParams, &readMspIO))
        return CSD_ERROR;

    /* Is the CA key ladder disabled? */
    if(readMspIO.mspDataBuf[3] == 0)
    {

        progMspIO.progMode       = NEXUS_OTP_DATASECTIONPROG_MODE;
        progMspIO.progMspEnum = NEXUS_OtpCmdMsp_eCaKeyLadderDisable;
        progMspIO.dataBitLen     = 1;
        progMspIO.dataBitMask[0] = 0x00;
        progMspIO.dataBitMask[1] = 0x00;
        progMspIO.dataBitMask[2] = 0x00;
        progMspIO.dataBitMask[3] = 0x01;
        progMspIO.mspData[0]     = 0x00;
        progMspIO.mspData[1]     = 0x00;
        progMspIO.mspData[2]     = 0x00;
        progMspIO.mspData[3]     = 0x01;

        /* Issue command to program the OTP. */
        if(NEXUS_Security_ProgramMSP (&progMspIO))
            return CSD_ERROR;

        readMspParams.readMspEnum = NEXUS_OtpCmdMsp_eCaKeyLadderDisable;
        if(NEXUS_Security_ReadMSP (&readMspParams, &readMspIO))
            return CSD_ERROR;

        if(progMspIO.mspData[3] != 0x1)
            return CSD_ERROR;
    }

    readMspParams.readMspEnum = NEXUS_OtpCmdMsp_eCpKeyLadderDisable;
    if(NEXUS_Security_ReadMSP (&readMspParams, &readMspIO))
        return CSD_ERROR;

    /* Is the CP key ladder disabled? */
    if(readMspIO.mspDataBuf[3] == 0)
    {
        progMspIO.progMode       = NEXUS_OTP_DATASECTIONPROG_MODE;
        progMspIO.progMspEnum = NEXUS_OtpCmdMsp_eCpKeyLadderDisable;
        progMspIO.dataBitLen     = 1;
        progMspIO.dataBitMask[0] = 0x00;
        progMspIO.dataBitMask[1] = 0x00;
        progMspIO.dataBitMask[2] = 0x00;
        progMspIO.dataBitMask[3] = 0x01;
        progMspIO.mspData[0]     = 0x00;
        progMspIO.mspData[1]     = 0x00;
        progMspIO.mspData[2]     = 0x00;
        progMspIO.mspData[3]     = 0x01;

        /* Issue command to program the OTP. */
        if(NEXUS_Security_ProgramMSP (&progMspIO))
            return CSD_ERROR;

        readMspParams.readMspEnum = NEXUS_OtpCmdMsp_eCpKeyLadderDisable;
        if(NEXUS_Security_ReadMSP (&readMspParams, &readMspIO))
            return CSD_ERROR;

        if(progMspIO.mspData[3] != 0x1)
            return CSD_ERROR;
    }
#endif

    return CSD_NO_ERROR;
}


TCsdStatus csdGetKeyLadderDeactivationFlag(
    TCsdActivationFlag* pxKeyLadderDeactivationFlagState
)
{
    NEXUS_ReadMspParms readMspParams;
    NEXUS_ReadMspIO readMspIO;

    if(CSD_INITIALIZED == FALSE )
        return CSD_ERROR;

    if(pxKeyLadderDeactivationFlagState == NULL)
        return CSD_ERROR;

#ifdef VER_ZEUS22
    readMspParams.readMspEnum = NEXUS_OtpCmdMsp_eReserved194;

    if(NEXUS_Security_ReadMSP (&readMspParams, &readMspIO))
        return CSD_ERROR;

    BDBG_MSG(("Disable CA Key ladder  = %x\n", readMspIO.mspDataBuf[3]));

    if(readMspIO.mspDataBuf[3] == 0x1)
    {
        *pxKeyLadderDeactivationFlagState = CSD_ACTIVATION_FLAG_SET;
    }
    else
    {
        *pxKeyLadderDeactivationFlagState = CSD_ACTIVATION_FLAG_NOT_SET;
    }

    return CSD_NO_ERROR;
#else
    readMspParams.readMspEnum = NEXUS_OtpCmdMsp_eCaKeyLadderDisable;
    if(NEXUS_Security_ReadMSP (&readMspParams, &readMspIO))
        return CSD_ERROR;

    BDBG_MSG(("Disable CA Key ladder  = %x\n", readMspIO.mspDataBuf[3]));

    if(readMspIO.mspDataBuf[3] == 0x1)
    {
        readMspParams.readMspEnum = NEXUS_OtpCmdMsp_eCpKeyLadderDisable;
        if(NEXUS_Security_ReadMSP (&readMspParams, &readMspIO))
            return CSD_ERROR;

        BDBG_MSG(("Disable CP Key ladder  = %x\n", readMspIO.mspDataBuf[3]));

        if(readMspIO.mspDataBuf[3] == 0x1)
        {
            *pxKeyLadderDeactivationFlagState = CSD_ACTIVATION_FLAG_SET;
            return CSD_NO_ERROR;
        }
        else
            return CSD_ERROR;
    }
    else
    {
        readMspParams.readMspEnum = NEXUS_OtpCmdMsp_eCpKeyLadderDisable;
        if(NEXUS_Security_ReadMSP (&readMspParams, &readMspIO))
            return CSD_ERROR;

        BDBG_MSG(("Disable CP Key ladder  = %x\n", readMspIO.mspDataBuf[3]));

        if(readMspIO.mspDataBuf[3] == 0x1)
        {
            return CSD_ERROR;
        }
        else
        {
            *pxKeyLadderDeactivationFlagState = CSD_ACTIVATION_FLAG_NOT_SET;
            return CSD_NO_ERROR;
        }
    }
#endif

}

TCsdStatus csdGetDvrKey(
    TCsdUnsignedInt8 xDvrKey[16]
)
{
    TCsdR2RKeyPathHandle *r2rHandle = NULL;
    TCsdUnsignedInt8 *xDvrKeyR2ROutput;
    TCsdUnsignedInt8 *KeySafeR2RInput;
    TCsdSize	contKeySize = 16;
    TCsdStatus  retCode = CSD_NO_ERROR;
    const TCsdUnsignedInt8	 KeySafe[16] = {
        0xbf, 0x18, 0x6a, 0x73, 0xec, 0x86, 0x3f, 0x25,
        0x9b, 0xe8, 0x03, 0x52, 0x40, 0xd7, 0x37, 0x8c
    };

    const TCsdR2RCipheredProtectingKeys CONSTANT_1 =
    {
        {
            0xA9, 0x32, 0x30, 0x31, 0x31, 0x4E, 0x61, 0x67,
            0x72, 0x61, 0x76, 0x69, 0x73, 0x69, 0x6F, 0x6E

        },
        {
            0xA9, 0x32, 0x30, 0x31, 0x31, 0x4E, 0x61, 0x67,
            0x72, 0x61, 0x76, 0x69, 0x73, 0x69, 0x6F, 0x6E
        }
    };

    const TCsdUnsignedInt8 CONSTANT_2[16] = {
        0xA9, 0x32, 0x30, 0x31, 0x31, 0x4E, 0x61, 0x67,
        0x72, 0x61, 0x76, 0x69, 0x73, 0x69, 0x6F, 0x6E
    };

    if( CSD_INITIALIZED == FALSE )
        return CSD_ERROR;

    if( xDvrKey == NULL )
        return CSD_ERROR;

#ifdef CSD_DEBUG
    printf("KeySafe[16]\n");
    for( i=0; i<16; i++ )
        printf("0x%02x ", KeySafe[i]);
    printf("\n");
#endif


    /* Need to allocate memory for M2M input and output buffers. */
    NEXUS_Memory_Allocate(sizeof(unsigned char) * 16, NULL, (void *)&xDvrKeyR2ROutput);
    NEXUS_Memory_Allocate(sizeof(unsigned char) * 16, NULL, (void *)&KeySafeR2RInput);

    retCode = csdBrcmOpenR2RKeyPath(&r2rHandle, 0);
    if (CSD_NO_ERROR != retCode)
    {
        BDBG_ERR(("csdBrcmOpenR2RKeyPath failed"));
        goto CSD_P_DONE;
    }

    memset(xDvrKeyR2ROutput, 0, sizeof(unsigned char) * 16 );
    memset(KeySafeR2RInput, 0, sizeof(unsigned char) * 16 );

    /* Copy the KeySafeto M2M input buffer. */
    BKNI_Memcpy((void *)KeySafeR2RInput, (void *)KeySafe, 16 );

    if( csdSetProtectedRam2RamContentKeys(
                CSD_Algorithm_RAW_TDES_ABA_ECB_CLEAR,
                r2rHandle,
                CONSTANT_1,
                CONSTANT_2,
                contKeySize) != CSD_NO_ERROR)
    {
        retCode = CSD_ERROR;
        goto CSD_P_DONE;
    }

    if( csdDecryptData(
                CSD_Algorithm_RAW_TDES_ABA_ECB_CLEAR,
                r2rHandle,
                NULL,
                0,
                KeySafeR2RInput,
                xDvrKeyR2ROutput,
                16) != CSD_NO_ERROR)
    {
        retCode = CSD_ERROR;
        goto CSD_P_DONE;
    }

    /* Copy the M2M output buffer to xDvrKey. */
    BKNI_Memcpy((void *)xDvrKey, (void *)xDvrKeyR2ROutput, 16 );


#ifdef CSD_DEBUG
    printf("DvrKey[16]\n");
    for( i=0; i<16; i++ )
        printf("0x%02x ", xDvrKey[i]);
    printf("\n");
#endif

CSD_P_DONE:
    if (NULL != r2rHandle)
    {
        if (CSD_NO_ERROR != csdBrcmCloseR2RKeyPath(r2rHandle))
        {
            /* Don't lose a pre-existing error code */
            if (retCode == CSD_NO_ERROR)
              retCode = CSD_ERROR;
        }
    }

    NEXUS_Memory_Free(xDvrKeyR2ROutput);
    NEXUS_Memory_Free(KeySafeR2RInput);

    return (retCode);
}


static TCsdStatus csdBrcmLoadIv
(
    TCsdUnsignedInt16       xEmi,
    NEXUS_KeySlotHandle     keyHandle,
    bool                    bCaDest,
    const TCsdUnsignedInt8* pxInitializationVector,
    size_t                  xInitializationVectorSize,
    TCsdScrKeyPathHandle *  pxScrKeyPathHandle
)
{
    NEXUS_SecurityClearKey clearKey;
    const TCsdUnsignedInt8 xZeroVector[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    size_t xRequiredIvSize = 0, xIvSize = 0;
    const TCsdUnsignedInt8* pIv = NULL;
    TCsdStatus result = CSD_NO_ERROR;

    if (!brcmEmiIsValid(xEmi))
    {
        BDBG_ERR(("%s: Invalid EMI", __FUNCTION__));
        return CSD_ERROR;
    }

    xRequiredIvSize = brcmEmiGetIvSize(xEmi);
    if (0 == xRequiredIvSize)
        goto finish;

    if (brcmEmiIsZeroIv(xEmi))
    {
        pIv = xZeroVector;
        xIvSize = xRequiredIvSize;
    }
    else
    {
        if ((NULL == pxInitializationVector) ||
            (xInitializationVectorSize != xRequiredIvSize))
        {
            BDBG_ERR(("%s: Invalid IV", __FUNCTION__));
            result = CSD_ERROR_INVALID_PARAMETERS;
            goto finish;
        }

        pIv = pxInitializationVector;
        xIvSize = xInitializationVectorSize;
    }

    if (xIvSize != xRequiredIvSize)
    {
        xIvSize = 0;
        result = CSD_ERROR;
        goto finish;
    }

    NEXUS_Security_GetDefaultClearKey(&clearKey);
    clearKey.keyIVType    = NEXUS_SecurityKeyIVType_eIV;
    clearKey.keyEntryType = NEXUS_SecurityKeyType_eIv;
    clearKey.keySize      = 16;
    if (bCaDest)
        clearKey.dest = NEXUS_SecurityAlgorithmConfigDestination_eCa;
    memset(clearKey.keyData, 0, sizeof(TCsdUnsignedInt8)*16);

    if (xIvSize == 8)
        BKNI_Memcpy(&(clearKey.keyData[8]), pIv, sizeof(TCsdUnsignedInt8)*8);
    else
        BKNI_Memcpy(clearKey.keyData, pIv, sizeof(TCsdUnsignedInt8)*16);

    if (NEXUS_Security_LoadClearKey(keyHandle, &clearKey) != 0)
    {
        BDBG_ERR(("%s: NEXUS_Security_LoadClearKey failed", __FUNCTION__));
        result = CSD_ERROR;
        goto finish;
    }

finish:
    if (pxScrKeyPathHandle)
    {
        pxScrKeyPathHandle->ivSize = xIvSize;
        if (xIvSize)
        BKNI_Memcpy(pxScrKeyPathHandle->ivArray, pIv, xIvSize);
    }

    return result;
}

/* Clear CA */
TCsdStatus csdSetClearTextDscHostKeys
(
    TCsdUnsignedInt16            xEmi,
    const TCsdUnsignedInt8* pxClearTextDscOddHostKey,
    TCsdSize xClearTextDscOddHostKeySize,
    const TCsdUnsignedInt8* pxClearTextDscEvenHostKey,
    TCsdSize xClearTextDscEvenHostKeySize,
    TCsdDscKeyPathHandle* pxDscKeyPathHandle
)
{
    NEXUS_SecurityAlgorithmSettings	NexusConfig;
    NEXUS_SecurityClearKey clearKey;
    BERR_Code errCode=CSD_NO_ERROR;
    bool bSetOddKey = false;
    bool bSetEvenKey = false;

    BDBG_MSG(("Enter %s", __FUNCTION__));

    /* Basic validation */
    if (FALSE == CSD_INITIALIZED)
        return CSD_ERROR;

    if (NULL == pxDscKeyPathHandle)
        return CSD_ERROR_INVALID_KEY_PATH_HANDLE;

    if ((pxDscKeyPathHandle->pidchannel>256) ||
        (NULL == pxDscKeyPathHandle->keyHandle))
        return CSD_ERROR;

    if ((!brcmEmiIsValid(xEmi)) ||
        (!brcmEmiIsMpegTs(xEmi)))
        return CSD_ERROR_OPERATION_NOT_SUPPORTED;

    /* Check passed-in keys */
    if (xClearTextDscOddHostKeySize != 0)
    {
        if ((pxClearTextDscOddHostKey == NULL) ||
            (xClearTextDscOddHostKeySize != brcmEmiGetKeySize(xEmi)))
            return CSD_ERROR_INVALID_PARAMETERS;

        bSetOddKey = true;
    }

    if (xClearTextDscEvenHostKeySize != 0)
    {
        if ((pxClearTextDscEvenHostKey == NULL) ||
            (xClearTextDscEvenHostKeySize != brcmEmiGetKeySize(xEmi)))
            return CSD_ERROR_INVALID_PARAMETERS;

        bSetEvenKey = true;
    }

    if (!brcmEmiSetupAlgorithm(xEmi, &NexusConfig))
        return CSD_ERROR;

    NexusConfig.dest = NEXUS_SecurityAlgorithmConfigDestination_eCa;

    if ((bSetOddKey) || (!pxDscKeyPathHandle->bAnyKeySet))
    {
        if ((pxDscKeyPathHandle->xCurrentOddEmi != xEmi) ||
            (!pxDscKeyPathHandle->bOddFromClear))
        {
            NexusConfig.keyDestEntryType = NEXUS_SecurityKeyType_eOdd;
            errCode = NEXUS_Security_ConfigAlgorithm(pxDscKeyPathHandle->keyHandle, &NexusConfig);
            if (errCode != 0)
            {
                BDBG_WRN(("NEXUS_Security_ConfigAlgorithm(odd) errCode: %x\n", errCode ));
                return CSD_ERROR;
            }

            pxDscKeyPathHandle->xCurrentOddEmi = xEmi;
            pxDscKeyPathHandle->bOddFromClear = true;
        }
    }

    if (bSetOddKey)
    {
        NEXUS_Security_GetDefaultClearKey(&clearKey);
        clearKey.keyIVType= NEXUS_SecurityKeyIVType_eNoIV;
        clearKey.keySize = xClearTextDscOddHostKeySize;
        clearKey.keyEntryType = NEXUS_SecurityKeyType_eOdd;
        clearKey.dest=NEXUS_SecurityAlgorithmConfigDestination_eCa;
        BKNI_Memcpy(clearKey.keyData,  pxClearTextDscOddHostKey,clearKey.keySize) ;

        errCode = NEXUS_Security_LoadClearKey(pxDscKeyPathHandle->keyHandle, &clearKey);

        if (errCode != 0)
        {
            BDBG_WRN(("NEXUS_Security_LoadClearKey (odd) errCode: %x\n", errCode ));
            return CSD_ERROR;
        }
    }

    if ((bSetEvenKey) || (!pxDscKeyPathHandle->bAnyKeySet))
    {
        if ((pxDscKeyPathHandle->xCurrentEvenEmi != xEmi) ||
            (!pxDscKeyPathHandle->bEvenFromClear))
        {
            NexusConfig.keyDestEntryType = NEXUS_SecurityKeyType_eEven;
            errCode = NEXUS_Security_ConfigAlgorithm(pxDscKeyPathHandle->keyHandle, &NexusConfig);
            if (errCode != 0)
            {
                BDBG_WRN(("NEXUS_Security_ConfigAlgorithm(even) errCode: %x\n", errCode ));
                return CSD_ERROR;
            }

            pxDscKeyPathHandle->xCurrentEvenEmi = xEmi;
            pxDscKeyPathHandle->bEvenFromClear = true;
        }
    }

    if (bSetEvenKey)
    {
        NEXUS_Security_GetDefaultClearKey(&clearKey);
        clearKey.keyIVType= NEXUS_SecurityKeyIVType_eNoIV;
        clearKey.keySize = xClearTextDscEvenHostKeySize;
        clearKey.keyEntryType = NEXUS_SecurityKeyType_eEven;
        clearKey.dest=NEXUS_SecurityAlgorithmConfigDestination_eCa;

        BKNI_Memcpy(clearKey.keyData,  pxClearTextDscEvenHostKey,clearKey.keySize) ;

        errCode = NEXUS_Security_LoadClearKey( pxDscKeyPathHandle->keyHandle, &clearKey);

        if (errCode != 0)
        {
            BDBG_MSG(("NEXUS_Security_LoadClearKey (even) errCode: %x\n", errCode ));
            return CSD_ERROR;
        }
    }

    pxDscKeyPathHandle->bAnyKeySet = true;

    return csdBrcmLoadIv(
        xEmi,
        pxDscKeyPathHandle->keyHandle,
        true,
        pxDscKeyPathHandle->initializationVector,
        brcmEmiGetIvSize(xEmi),
        NULL);
}

/* CA key ladder */

#define CSD_DSC_GENERATE_ROUNTE_KEY(polarity, pKey, keySizeIn)\
	if ( keySizeIn > 0)		\
	{\
		SessionKey.keyladderID   = NEXUS_SecurityKeyladderID_eA; \
		SessionKey.keyGenCmdID   = NEXUS_SecurityKeyGenCmdID_eKeyGen; \
		SessionKey.keyladderType = NEXUS_SecurityKeyladderType_e3Des; \
	    SessionKey.rootKeySrc = NEXUS_SecurityRootKeySrc_eOtpKeyA; \
		SessionKey.swizzleType = NEXUS_SecuritySwizzleType_eNone; \
		SessionKey.keyEntryType = polarity; 		\
		SessionKey.keyDestIVType = NEXUS_SecurityKeyIVType_eNoIV;\
		SessionKey.operation = NEXUS_SecurityOperation_eDecrypt; \
		SessionKey.operationKey2 	= NEXUS_SecurityOperation_eEncrypt;\
		SessionKey.bASKMMode = true;\
		SessionKey.keyMode = NEXUS_SecurityKeyMode_eRegular;\
    	SessionKey.sessionKeyOp = NEXUS_SecuritySessionKeyOp_eNoProcess;\
    	SessionKey.bSwapAESKey = false;\
		SessionKey.cusKeyL = 0;\
		SessionKey.cusKeyH = 0;\
		SessionKey.cusKeyVarL = 0;\
		SessionKey.cusKeyVarH = 0;\
		SessionKey.bRouteKey = false;\
		SessionKey.keyLadderSelect = NEXUS_SecurityKeyLadderSelect_eHWKL;\
		SessionKey.hwklLen			= NEXUS_SecurityHWKLLenSelect_eLen3;\
		SessionKey.hwklDestAlg = NexusConfig.algorithm;\
		SessionKey.rootKeySwap	= false;\
		BKNI_Memcpy(&(proc_in3[0][0]), &(xDscCipheredProtectingKeys[0][8]), sizeof(TCsdUnsignedInt8)*8);\
		BKNI_Memcpy(&(proc_in3[0][8]), &(xDscCipheredProtectingKeys[0][0]), sizeof(TCsdUnsignedInt8)*8);\
		BKNI_Memcpy(SessionKey.keyData,&(proc_in3[0][0]), 16);\
		errCode = NEXUS_Security_GenerateSessionKey( pxDscKeyPathHandle->keyHandle, &SessionKey);\
		if (errCode != 0) \
		{\
			BDBG_MSG(("NEXUS_Security_GenerateSessionKey errCode: %x", errCode ));	\
			return CSD_ERROR;\
		}	\
        BDBG_MSG(("NEXUS_Security_GenerateSessionKey success: %x", errCode ));	\
		if(CSD_DSC_NB_OF_PROTECTING_KEYS == 2)\
		{\
			CW.keyladderID = NEXUS_SecurityKeyladderID_eA; \
			CW.keyGenCmdID   = NEXUS_SecurityKeyGenCmdID_eKeyGen; \
			CW.keyladderType = NEXUS_SecurityKeyladderType_e3Des; \
		    CW.rootKeySrc = NEXUS_SecurityRootKeySrc_eOtpKeyA; \
			CW.keyEntryType = polarity; \
			CW.keyDestIVType = NEXUS_SecurityKeyIVType_eNoIV;\
			CW.swizzleType = NEXUS_SecuritySwizzleType_eNone; \
			CW.bASKMMode = true;\
			CW.keyMode = NEXUS_SecurityKeyMode_eRegular;\
			CW.operation = NEXUS_SecurityOperation_eDecrypt; \
    		CW.bSwapAESKey = false;\
			CW.bRouteKey = false;\
			CW.keyLadderSelect = NEXUS_SecurityKeyLadderSelect_eHWKL;\
			CW.hwklLen			= NEXUS_SecurityHWKLLenSelect_eLen3;\
			CW.hwklDestAlg = NexusConfig.algorithm;\
			BKNI_Memcpy(&(proc_in3[1][0]), &(xDscCipheredProtectingKeys[1][8]), sizeof(TCsdUnsignedInt8)*8);	\
			BKNI_Memcpy(&(proc_in3[1][8]), &(xDscCipheredProtectingKeys[1][0]), sizeof(TCsdUnsignedInt8)*8);\
			BKNI_Memcpy(CW.keyData,&(proc_in3[1][0]), 16);\
			CW.keySize = 16;\
	       	errCode = NEXUS_Security_GenerateControlWord(pxDscKeyPathHandle->keyHandle , &CW);\
			if (errCode != 0) \
			{\
				BDBG_WRN(("NEXUS_Security_LoadClearKey (odd) errCode: %x", errCode ));	\
    			return CSD_ERROR;\
			}	\
            BDBG_MSG(("NEXUS_Security_GenerateControlWord (odd) success: %x", errCode ));	\
		}\
		else\
		{\
			CW.keyladderID = NEXUS_SecurityKeyladderID_eA; \
			CW.keyGenCmdID   = NEXUS_SecurityKeyGenCmdID_eKeyGen; \
			CW.keyladderType = NEXUS_SecurityKeyladderType_e3Des; \
		    CW.rootKeySrc = NEXUS_SecurityRootKeySrc_eOtpKeyA; \
			CW.keyEntryType = polarity; \
			CW.keyDestIVType = NEXUS_SecurityKeyIVType_eNoIV;\
			CW.swizzleType = NEXUS_SecuritySwizzleType_eNone; \
			CW.bASKMMode = true;\
			CW.keyMode = NEXUS_SecurityKeyMode_eRegular;\
			CW.operation = NEXUS_SecurityOperation_eDecrypt; \
    		CW.bSwapAESKey = false;\
			CW.bRouteKey = true;\
			CW.keyLadderSelect = NEXUS_SecurityKeyLadderSelect_eHWKL;\
			CW.hwklLen			= NEXUS_SecurityHWKLLenSelect_eLen3;\
			CW.hwklDestAlg = NexusConfig.algorithm;\
			BKNI_Memcpy(CW.keyData,pKey, keySizeIn);\
			CW.keySize = keySizeIn;\
	       	errCode = NEXUS_Security_GenerateControlWord(pxDscKeyPathHandle->keyHandle , &CW);\
			if (errCode != 0) \
			{\
				BDBG_MSG(("NEXUS_Security_LoadClearKey (odd) errCode: %x", errCode ));	\
    			return CSD_ERROR;\
			}			\
            BDBG_MSG(("NEXUS_Security_GenerateControlWord 1 success: %x", errCode ));	\
		}\
		if(CSD_DSC_NB_OF_PROTECTING_KEYS == 2)\
		{\
			encrytedCW.keyladderID = NEXUS_SecurityKeyladderID_eA; \
			encrytedCW.keyGenCmdID   = NEXUS_SecurityKeyGenCmdID_eKeyGen; \
			encrytedCW.keyladderType = NEXUS_SecurityKeyladderType_e3Des; \
		    encrytedCW.rootKeySrc = NEXUS_SecurityRootKeySrc_eOtpKeyA; \
			encrytedCW.keyEntryType = polarity; \
			encrytedCW.keyDestIVType = NEXUS_SecurityKeyIVType_eNoIV;\
			encrytedCW.swizzleType = NEXUS_SecuritySwizzleType_eNone; \
			encrytedCW.bASKMMode = true;\
			encrytedCW.keyMode = NEXUS_SecurityKeyMode_eRegular;\
			encrytedCW.operation = NEXUS_SecurityOperation_eDecrypt; \
    		encrytedCW.bSwapAESKey = false;\
			encrytedCW.bRouteKey = true;\
			encrytedCW.keyLadderSelect = NEXUS_SecurityKeyLadderSelect_eHWKL;\
			encrytedCW.hwklLen			= NEXUS_SecurityHWKLLenSelect_eLen3;\
			encrytedCW.hwklDestAlg = NexusConfig.algorithm;\
			BKNI_Memcpy(encrytedCW.keyData,pKey, keySizeIn);\
			encrytedCW.keySize = keySizeIn;\
	       	errCode = NEXUS_Security_GenerateKey5(pxDscKeyPathHandle->keyHandle , &encrytedCW);\
			if (errCode != 0) \
			{\
				BDBG_MSG(("NEXUS_Security_LoadClearKey (odd) errCode: %x", errCode ));\
    			return CSD_ERROR;\
			}\
             BDBG_MSG(("NEXUS_Security_GenerateKey5 success: %x", errCode ));	\
		}\
	}\

TCsdStatus csdSetProtectedDscContentKeys
(
    TCsdUnsignedInt16            xEmi,
    const TCsdDscCipheredProtectingKeys xDscCipheredProtectingKeys,
    const TCsdUnsignedInt8* pxCipheredDscOddContentKey,
    TCsdSize xCipheredDscOddContentKeySize,
    const TCsdUnsignedInt8* pxCipheredDscEvenContentKey,
    TCsdSize xCipheredDscEvenContentKeySize,
    TCsdDscKeyPathHandle* pxDscKeyPathHandle
)
{
    NEXUS_SecurityAlgorithmSettings	NexusConfig;
    NEXUS_SecurityEncryptedSessionKey	SessionKey;
    NEXUS_SecurityEncryptedControlWord	CW;
    NEXUS_SecurityEncryptedControlWord encrytedCW;
    TCsdDscCipheredProtectingKeys proc_in3;
    BERR_Code errCode=CSD_NO_ERROR;
    bool bSetOddKey = false;
    bool bSetEvenKey = false;

    BDBG_MSG(("Enter %s", __FUNCTION__));

    /* Basic validation */
    if (FALSE == CSD_INITIALIZED)
        return CSD_ERROR;

    if (NULL == pxDscKeyPathHandle)
        return CSD_ERROR_INVALID_KEY_PATH_HANDLE;

    if ((pxDscKeyPathHandle->pidchannel>256) ||
        (NULL == pxDscKeyPathHandle->keyHandle))
        return CSD_ERROR;

    if ((!brcmEmiIsValid(xEmi)) ||
        (!brcmEmiIsMpegTs(xEmi)))
        return CSD_ERROR_OPERATION_NOT_SUPPORTED;

    /* Check passed-in keys */
    if (xCipheredDscOddContentKeySize != 0)
    {
        if ((NULL == pxCipheredDscOddContentKey) ||
            (xCipheredDscOddContentKeySize != brcmEmiGetKeySize(xEmi)))
            return CSD_ERROR_INVALID_PARAMETERS;

        bSetOddKey = true;
    }

    if (xCipheredDscEvenContentKeySize != 0)
    {
        if ((NULL == pxCipheredDscEvenContentKey) ||
            (xCipheredDscEvenContentKeySize != brcmEmiGetKeySize(xEmi)))
            return CSD_ERROR_INVALID_PARAMETERS;

        bSetEvenKey = true;
    }

    /* Anything to do? */
    if ((!bSetEvenKey) && (!bSetOddKey))
        return CSD_NO_ERROR;

    /* Config Algorithm */
    if (!brcmEmiSetupAlgorithm(xEmi, &NexusConfig))
        return CSD_ERROR;

    NexusConfig.dest = NEXUS_SecurityAlgorithmConfigDestination_eCa;

    NexusConfig.caVendorID       = CSD_ASKM_CA_VENDOR_ID;
    NexusConfig.otpId            = NEXUS_SecurityOtpId_eOtpVal;
    NexusConfig.testKey2Select   = 0x0;

    NEXUS_Security_GetDefaultSessionKeySettings(&SessionKey);
    NEXUS_Security_GetDefaultControlWordSettings(&CW);
    NEXUS_Security_GetDefaultControlWordSettings(&encrytedCW);

    if(CSD_DSC_NB_OF_PROTECTING_KEYS == 2)
    {
        if(brcmEmiGetKeySize(xEmi) == 8)
        {
            SessionKey.custSubMode = NEXUS_SecurityCustomerSubMode_eGeneric_CA_64_5;
            CW.custSubMode = NEXUS_SecurityCustomerSubMode_eGeneric_CA_64_5;
            encrytedCW.custSubMode = NEXUS_SecurityCustomerSubMode_eGeneric_CA_64_5;
            /* NexusConfig.askmModuleID = NEXUS_SecurityAskmModuleID_eModuleID_5;*/
            SessionKey.virtualKeyLadderID = NEXUS_SecurityVirtualKeyladderID_eVKL2;
            CW.virtualKeyLadderID = NEXUS_SecurityVirtualKeyladderID_eVKL2;
            encrytedCW.virtualKeyLadderID = NEXUS_SecurityVirtualKeyladderID_eVKL2;
        }
        else
        {
            SessionKey.custSubMode = NEXUS_SecurityCustomerSubMode_eGeneric_CA_128_5;
            CW.custSubMode = NEXUS_SecurityCustomerSubMode_eGeneric_CA_128_5;
            encrytedCW.custSubMode = NEXUS_SecurityCustomerSubMode_eGeneric_CA_128_5;
            /* NexusConfig.askmModuleID = NEXUS_SecurityAskmModuleID_eModuleID_9;*/
            SessionKey.virtualKeyLadderID = NEXUS_SecurityVirtualKeyladderID_eVKL3;
            CW.virtualKeyLadderID = NEXUS_SecurityVirtualKeyladderID_eVKL3;
            encrytedCW.virtualKeyLadderID = NEXUS_SecurityVirtualKeyladderID_eVKL3;
        }
    }
    else
    {
        if(brcmEmiGetKeySize(xEmi) == 8)
        {
            SessionKey.custSubMode = NEXUS_SecurityCustomerSubMode_eGeneric_CA_64_4;
            CW.custSubMode = NEXUS_SecurityCustomerSubMode_eGeneric_CA_64_4;
            /*NexusConfig.askmModuleID = NEXUS_SecurityAskmModuleID_eModuleID_3;*/
            SessionKey.virtualKeyLadderID = NEXUS_SecurityVirtualKeyladderID_eVKL2;
            CW.virtualKeyLadderID = NEXUS_SecurityVirtualKeyladderID_eVKL2;
        }
        else
        {
            SessionKey.custSubMode = NEXUS_SecurityCustomerSubMode_eGeneric_CA_128_4;
            CW.custSubMode = NEXUS_SecurityCustomerSubMode_eGeneric_CA_128_4;
            /* NexusConfig.askmModuleID = NEXUS_SecurityAskmModuleID_eModuleID_7;*/
            SessionKey.virtualKeyLadderID = NEXUS_SecurityVirtualKeyladderID_eVKL3;
            CW.virtualKeyLadderID = NEXUS_SecurityVirtualKeyladderID_eVKL3;
        }
    }

    NexusConfig.key2Select   = NEXUS_SecurityKey2Select_eFixedKey;
    NexusConfig.askmModuleID = CSD_ASKM_MODULE_ID;

    if ((bSetOddKey) || (!pxDscKeyPathHandle->bAnyKeySet))
    {
        if ((pxDscKeyPathHandle->xCurrentOddEmi != xEmi) ||
            (pxDscKeyPathHandle->bOddFromClear))
        {
            NexusConfig.keyDestEntryType = NEXUS_SecurityKeyType_eOdd;
            errCode = NEXUS_Security_ConfigAlgorithm(pxDscKeyPathHandle->keyHandle, &NexusConfig);
            if (errCode != 0)
            {
                BDBG_ERR(("NEXUS_Security_ConfigAlgorithm(odd) errCode: %x", errCode ));
                return CSD_ERROR;
            }

            pxDscKeyPathHandle->xCurrentOddEmi = xEmi;
            pxDscKeyPathHandle->bOddFromClear  = false;
        }
    }

    if ((bSetEvenKey) || (!pxDscKeyPathHandle->bAnyKeySet))
    {
        if ((pxDscKeyPathHandle->xCurrentEvenEmi != xEmi) ||
            (pxDscKeyPathHandle->bEvenFromClear))
        {
            NexusConfig.keyDestEntryType = NEXUS_SecurityKeyType_eEven;
            errCode = NEXUS_Security_ConfigAlgorithm(pxDscKeyPathHandle->keyHandle, &NexusConfig);
            if (errCode != 0)
            {
                BDBG_ERR(("NEXUS_Security_ConfigAlgorithm(odd) errCode: %x", errCode ));
                return CSD_ERROR;
            }

            pxDscKeyPathHandle->xCurrentEvenEmi = xEmi;
            pxDscKeyPathHandle->bEvenFromClear  = false;
        }
    }

    pxDscKeyPathHandle->bAnyKeySet = true;

    /* Route ODD key */
    CSD_DSC_GENERATE_ROUNTE_KEY(NEXUS_SecurityKeyType_eOdd, pxCipheredDscOddContentKey, xCipheredDscOddContentKeySize);

    /* Route EVEN key */
    CSD_DSC_GENERATE_ROUNTE_KEY(NEXUS_SecurityKeyType_eEven, pxCipheredDscEvenContentKey, xCipheredDscEvenContentKeySize);

    return csdBrcmLoadIv(
        xEmi,
        pxDscKeyPathHandle->keyHandle,
        true,
        pxDscKeyPathHandle->initializationVector,
        brcmEmiGetIvSize(xEmi),
        NULL);
}


static TCsdStatus csdSetClearTextScrEncryptionDecryptionKey(
    TCsdUnsignedInt16            xEmi,
    const TCsdUnsignedInt8* pxClearTextScrHostKey,
    TCsdSize xClearTextScrHostKeySize,
    const TCsdUnsignedInt8* pxInitializationVector,
    TCsdSize xInitializationVectorSize,
    TCsdScrKeyPathHandle* pxScrKeyPathHandle,
    bool bEnc
)
{
    NEXUS_SecurityAlgorithmSettings algorithm;
    NEXUS_SecurityClearKey key;

    BDBG_MSG(("Enter %s", __FUNCTION__));

    /* Basic validation */
    if (FALSE == CSD_INITIALIZED)
    {
        BDBG_MSG(("Leave %s", __FUNCTION__));
        return CSD_ERROR;
    }

    if (NULL == pxScrKeyPathHandle)
    {
        BDBG_ERR(("%s: NULL pxScrKeyPathHandle", __FUNCTION__));
        return CSD_ERROR_INVALID_KEY_PATH_HANDLE;
    }

    if (NULL == pxScrKeyPathHandle->keyHandle)
    {
        BDBG_ERR(("%s: NULL pxScrKeyPathHandle", __FUNCTION__));
        return CSD_ERROR;
    }

    if (!brcmEmiIsValid(xEmi))
    {
        BDBG_ERR(("%s: Invalid EMI (0x%04x)", __FUNCTION__, xEmi));
        return CSD_ERROR_OPERATION_NOT_SUPPORTED;
    }

    /* Check passed-in key */
    if ((pxClearTextScrHostKey == NULL) ||
        (xClearTextScrHostKeySize != brcmEmiGetKeySize(xEmi)))
    {
        BDBG_ERR(("%s: Invalid key", __FUNCTION__));
        return CSD_ERROR_INVALID_PARAMETERS;
    }

    /* Config Algorithm */
    if (!brcmEmiSetupAlgorithm(xEmi, &algorithm))
    {
        BDBG_ERR(("%s: Algorithm config failed", __FUNCTION__));
        return CSD_ERROR;
    }

    /* csdBrcmLoadIv will do more complete validation, */
    /* but we just want to know that the correct IV size */
    /* was passed in here. */
    if ((brcmEmiGetIvSize(xEmi) > 0) &&
        (!brcmEmiIsZeroIv(xEmi)))
    {
        if (xInitializationVectorSize != brcmEmiGetIvSize(xEmi))
        {
            BDBG_ERR(("%s: Invalid IV", __FUNCTION__));
            return CSD_ERROR_INVALID_PARAMETERS;
        }
    }

    pxScrKeyPathHandle->dataFormat   = brcmEmiIsMpegTs(xEmi) ? NEXUS_DmaDataFormat_eMpeg : NEXUS_DmaDataFormat_eBlock;
    pxScrKeyPathHandle->bCounterMode = (algorithm.algorithmVar==NEXUS_SecurityAlgorithmVariant_eCounter);

	algorithm.operation        = (bEnc)?NEXUS_SecurityOperation_eEncrypt:NEXUS_SecurityOperation_eDecrypt;
	algorithm.keyDestEntryType = NEXUS_SecurityKeyType_eOdd;

    /* Load algorithm to key table.  Set it up to do encryption. */
    if(NEXUS_Security_ConfigAlgorithm(pxScrKeyPathHandle->keyHandle, &algorithm) != 0)
    {
        BDBG_WRN(("\nConfigAlg enc keyslot failed \n"));
        goto CSD_P_DONE;
    }

    NEXUS_Security_GetDefaultClearKey(&key);
    key.keyEntryType = NEXUS_SecurityKeyType_eOdd;
    key.keyIVType = NEXUS_SecurityKeyIVType_eNoIV;
    key.keySize = xClearTextScrHostKeySize;
    BKNI_Memcpy(key.keyData,  pxClearTextScrHostKey,sizeof(TCsdUnsignedInt8)*key.keySize) ;

    /* Load clear key to key table. */
    if (NEXUS_Security_LoadClearKey(pxScrKeyPathHandle->keyHandle, &key) != 0)
    {
        BDBG_WRN(("Load encryption key failed "));
        goto CSD_P_DONE;
    }

    return csdBrcmLoadIv(
        xEmi,
        pxScrKeyPathHandle->keyHandle,
        false,
        pxInitializationVector,
        xInitializationVectorSize,
        pxScrKeyPathHandle);

CSD_P_DONE:
    BDBG_ERR(("%s", __FUNCTION__));
    BDBG_MSG(("Leave %s", __FUNCTION__));

    return CSD_ERROR;

}

TCsdStatus csdSetClearTextScrEncryptionKey(
    TCsdUnsignedInt16            xEmi,
    const TCsdUnsignedInt8* pxClearTextScrHostKey,
    TCsdSize xClearTextScrHostKeySize,
    const TCsdUnsignedInt8* pxInitializationVector,
    TCsdSize xInitializationVectorSize,
    TCsdScrKeyPathHandle* pxScrKeyPathHandle
)
{
    BDBG_MSG(("Enter %s", __FUNCTION__));
    return csdSetClearTextScrEncryptionDecryptionKey(
               xEmi,
               pxClearTextScrHostKey,
               xClearTextScrHostKeySize,
               pxInitializationVector,
               xInitializationVectorSize,
               pxScrKeyPathHandle,
               true
           );
}

TCsdStatus csdSetClearTextScrDecryptionKey(
    TCsdUnsignedInt16            xEmi,
    const TCsdUnsignedInt8* pxClearTextScrHostKey,
    TCsdSize xClearTextScrHostKeySize,
    const TCsdUnsignedInt8* pxInitializationVector,
    TCsdSize xInitializationVectorSize,
    TCsdScrKeyPathHandle* pxScrKeyPathHandle
)
{
    BDBG_MSG(("Enter %s", __FUNCTION__));
    return csdSetClearTextScrEncryptionDecryptionKey(
               xEmi,
               pxClearTextScrHostKey,
               xClearTextScrHostKeySize,
               pxInitializationVector,
               xInitializationVectorSize,
               pxScrKeyPathHandle,
               false
           );

}


static TCsdStatus csdSetProtectedScrEncryptionDecryptionKey(
    TCsdUnsignedInt16 xEmi,
    const TCsdScrCipheredProtectingKeys xScrCipheredProtectingKeys,
    const TCsdUnsignedInt8* pxCipheredScrContentKey,
    TCsdSize xCipheredScrContentKeySize,
    const TCsdUnsignedInt8* pxInitializationVector,
    TCsdSize xInitializationVectorSize,
    TCsdScrKeyPathHandle* pxScrKeyPathHandle,
    bool	bEnc
)
{
    NEXUS_SecurityAlgorithmSettings algorithm;
    NEXUS_SecurityEncryptedSessionKey encryptedSessionkey;
    NEXUS_SecurityEncryptedControlWord encrytedCW, key4;
    TCsdR2RCipheredProtectingKeys proc_in3;

    BDBG_MSG(("Enter %s", __FUNCTION__));

    /* Basic validation */
    if (FALSE == CSD_INITIALIZED)
    {
        BDBG_MSG(("Leave %s", __FUNCTION__));
        return CSD_ERROR;
    }

    if (NULL == pxScrKeyPathHandle)
        return CSD_ERROR_INVALID_KEY_PATH_HANDLE;

    if (NULL == pxScrKeyPathHandle->keyHandle)
        return CSD_ERROR;

    if (!brcmEmiIsValid(xEmi))
        return CSD_ERROR_OPERATION_NOT_SUPPORTED;

    /* Check passed-in key */
    if ((pxCipheredScrContentKey == NULL) ||
        (xCipheredScrContentKeySize != brcmEmiGetKeySize(xEmi)))
        return CSD_ERROR_INVALID_PARAMETERS;

    /* Config Algorithm */
    if (!brcmEmiSetupAlgorithm(xEmi, &algorithm))
        return CSD_ERROR;

    /* csdBrcmLoadIv will do more complete validation, */
    /* but we just want to know that the correct IV size */
    /* was passed in here. */
    if ((brcmEmiGetIvSize(xEmi) > 0) &&
        (!brcmEmiIsZeroIv(xEmi)))
    {
        if (xInitializationVectorSize != brcmEmiGetIvSize(xEmi))
            return CSD_ERROR_INVALID_PARAMETERS;
    }

    pxScrKeyPathHandle->dataFormat   = brcmEmiIsMpegTs(xEmi) ? NEXUS_DmaDataFormat_eMpeg : NEXUS_DmaDataFormat_eBlock;
    pxScrKeyPathHandle->bCounterMode = (algorithm.algorithmVar==NEXUS_SecurityAlgorithmVariant_eCounter);
    pxScrKeyPathHandle->ivSize       = xInitializationVectorSize;
    BKNI_Memcpy(pxScrKeyPathHandle->ivArray, pxInitializationVector, xInitializationVectorSize);

	algorithm.operation        = (bEnc)?NEXUS_SecurityOperation_eEncrypt:NEXUS_SecurityOperation_eDecrypt;
	algorithm.keyDestEntryType = NEXUS_SecurityKeyType_eOdd;
    algorithm.caVendorID       = CSD_ASKM_CA_VENDOR_ID;

    if(CSD_SCR_NB_OF_PROTECTING_KEYS == 2)
        algorithm.askmModuleID = CSD_ASKM_MODULE_ID;
    else
        algorithm.askmModuleID = NEXUS_SecurityAskmModuleID_eModuleID_7;

    algorithm.otpId          = NEXUS_SecurityOtpId_eOtpVal;
    algorithm.testKey2Select = 0x0;
    algorithm.key2Select     = NEXUS_SecurityKey2Select_eFixedKey;

    /* Load algorithm to key table.  Set it up to do encryption. */
    if(NEXUS_Security_ConfigAlgorithm(pxScrKeyPathHandle->keyHandle, &algorithm) != 0)
    {
        BDBG_WRN(("ConfigAlg enc keyslot failed "));
        goto CSD_P_DONE;
    }

    /* Load session key3 */
    NEXUS_Security_GetDefaultSessionKeySettings (&encryptedSessionkey);
    encryptedSessionkey.keyladderID 	= NEXUS_SecurityKeyladderID_eA;
    encryptedSessionkey.keyGenCmdID 	= NEXUS_SecurityKeyGenCmdID_eKeyGen;
    encryptedSessionkey.keyLadderSelect = NEXUS_SecurityKeyLadderSelect_eHWKL;
    encryptedSessionkey.hwklLen			= NEXUS_SecurityHWKLLenSelect_eLen3;
    encryptedSessionkey.hwklDestAlg = algorithm.algorithm;
    encryptedSessionkey.keyladderType 	= NEXUS_SecurityKeyladderType_e3Des;
    encryptedSessionkey.rootKeySrc 		= NEXUS_SecurityRootKeySrc_eOtpKeyA;
    encryptedSessionkey.swizzleType		= NEXUS_SecuritySwizzleType_eNone;
    encryptedSessionkey.keyEntryType 	= NEXUS_SecurityKeyType_eOdd;
    encryptedSessionkey.keyDestIVType = NEXUS_SecurityKeyIVType_eNoIV;
    encryptedSessionkey.operation 		= NEXUS_SecurityOperation_eDecrypt;
    encryptedSessionkey.operationKey2 	= NEXUS_SecurityOperation_eEncrypt;
    encryptedSessionkey.bASKMMode = true;
    encryptedSessionkey.virtualKeyLadderID = NEXUS_SecurityVirtualKeyladderID_eVKL4;
    encryptedSessionkey.sessionKeyOp = NEXUS_SecuritySessionKeyOp_eNoProcess;
    encryptedSessionkey.cusKeyL 		= 0x00;
    encryptedSessionkey.cusKeyH 		= 0x00;
    encryptedSessionkey.cusKeyVarL 	= 0x00;
    encryptedSessionkey.cusKeyVarH 	= 0xFF;
    encryptedSessionkey.bSwapAESKey = false;
    encryptedSessionkey.bRouteKey 		= false;

    if(CSD_SCR_NB_OF_PROTECTING_KEYS == 2)
        encryptedSessionkey.custSubMode = NEXUS_SecurityCustomerSubMode_eReserved20;
    else
        encryptedSessionkey.custSubMode = NEXUS_SecurityCustomerSubMode_eGeneric_CP_128_4;

    encryptedSessionkey.keyMode = NEXUS_SecurityKeyMode_eRegular;

    /* Perform key swap for proc-in. */
    BKNI_Memcpy(&(proc_in3[0][0]), &(xScrCipheredProtectingKeys[0][8]), sizeof(TCsdUnsignedInt8)*8);
    BKNI_Memcpy(&(proc_in3[0][8]), &(xScrCipheredProtectingKeys[0][0]), sizeof(TCsdUnsignedInt8)*8);

    BKNI_Memcpy(encryptedSessionkey.keyData, &(proc_in3[0][0]), sizeof(TCsdUnsignedInt8)*16);

    if (NEXUS_Security_GenerateSessionKey(pxScrKeyPathHandle->keyHandle, &encryptedSessionkey) !=0)
    {

        BDBG_ERR(("Loading session key failed"));

        goto CSD_P_DONE;
    }

    /* Load key4 */
    NEXUS_Security_GetDefaultControlWordSettings (&key4);
    key4.keyladderID = NEXUS_SecurityKeyladderID_eA;
    key4.keyGenCmdID = NEXUS_SecurityKeyGenCmdID_eKeyGen;
    key4.hwklDestAlg = algorithm.algorithm;
    key4.keyladderType = NEXUS_SecurityKeyladderType_e3Des;
    /*key4.rootKeySrc = NEXUS_SecurityRootKeySrc_eOtpKeyA;*/
    key4.keyEntryType = NEXUS_SecurityKeyType_eOdd;
    key4.keyDestIVType = NEXUS_SecurityKeyIVType_eNoIV;
    key4.swizzleType = NEXUS_SecuritySwizzleType_eNone;
    key4.keyLadderSelect = NEXUS_SecurityKeyLadderSelect_eHWKL;
    key4.hwklLen = NEXUS_SecurityHWKLLenSelect_eLen3;
    key4.bASKMMode = true;
    key4.virtualKeyLadderID = NEXUS_SecurityVirtualKeyladderID_eVKL4;
    key4.operation = NEXUS_SecurityOperation_eDecrypt;
    key4.keyMode = NEXUS_SecurityKeyMode_eRegular;
    key4.bSwapAESKey = false;

    if(CSD_SCR_NB_OF_PROTECTING_KEYS == 2)
    {
        key4.bRouteKey = false;
        key4.custSubMode = NEXUS_SecurityCustomerSubMode_eReserved20;
        key4.keySize = 16;

        /* Perform key swap for proc-in. */
        BKNI_Memcpy(&(proc_in3[1][0]), &(xScrCipheredProtectingKeys[1][8]), sizeof(TCsdUnsignedInt8)*8);
        BKNI_Memcpy(&(proc_in3[1][8]), &(xScrCipheredProtectingKeys[1][0]), sizeof(TCsdUnsignedInt8)*8);

        BKNI_Memcpy(key4.keyData,  &(proc_in3[1][0]), sizeof(TCsdUnsignedInt8)*16);
    }
    else
    {
        key4.bRouteKey = true;
        key4.custSubMode = NEXUS_SecurityCustomerSubMode_eGeneric_CP_128_4;
        key4.keySize = xCipheredScrContentKeySize;
        BKNI_Memcpy(key4.keyData,  pxCipheredScrContentKey, sizeof(TCsdUnsignedInt8)*xCipheredScrContentKeySize);
    }


    if (NEXUS_Security_GenerateControlWord(pxScrKeyPathHandle->keyHandle, &key4))
    {

        BDBG_ERR(("Loading key4 failed for video ODD key"));

        goto CSD_P_DONE;
    }

    if(CSD_SCR_NB_OF_PROTECTING_KEYS == 2)
    {
        /* Load key5 */
        NEXUS_Security_GetDefaultControlWordSettings (&encrytedCW);
        encrytedCW.keyladderID = NEXUS_SecurityKeyladderID_eA;
        encrytedCW.keyGenCmdID = NEXUS_SecurityKeyGenCmdID_eKeyGen;
        encrytedCW.keyladderType = NEXUS_SecurityKeyladderType_e3Des;
        /*encrytedCW.rootKeySrc = NEXUS_SecurityRootKeySrc_eOtpKeyA;*/
        encrytedCW.keyEntryType = NEXUS_SecurityKeyType_eOdd;
        encrytedCW.keyLadderSelect = NEXUS_SecurityKeyLadderSelect_eHWKL;
        encrytedCW.hwklLen = NEXUS_SecurityHWKLLenSelect_eLen3;
        encrytedCW.hwklDestAlg = algorithm.algorithm;
        encrytedCW.keyDestIVType = NEXUS_SecurityKeyIVType_eNoIV;
        encrytedCW.swizzleType = NEXUS_SecuritySwizzleType_eNone;
        encrytedCW.bASKMMode = true;
        encrytedCW.custSubMode = NEXUS_SecurityCustomerSubMode_eReserved20;
        encrytedCW.virtualKeyLadderID = NEXUS_SecurityVirtualKeyladderID_eVKL4;
        encrytedCW.keyMode = NEXUS_SecurityKeyMode_eRegular;
        encrytedCW.keySize = xCipheredScrContentKeySize;
        BKNI_Memcpy(encrytedCW.keyData, pxCipheredScrContentKey, sizeof(TCsdUnsignedInt8)*xCipheredScrContentKeySize);
        encrytedCW.operation = NEXUS_SecurityOperation_eDecrypt;
        encrytedCW.bSwapAESKey = false;
        encrytedCW.bRouteKey = true;


        if (NEXUS_Security_GenerateKey5(pxScrKeyPathHandle->keyHandle, &encrytedCW))
        {

            BDBG_ERR(("Loading CW key failed for video ODD key"));

            goto CSD_P_DONE;
        }
    }

    return csdBrcmLoadIv(
        xEmi,
        pxScrKeyPathHandle->keyHandle,
        false,
        pxInitializationVector,
        xInitializationVectorSize,
        pxScrKeyPathHandle);

CSD_P_DONE:
    BDBG_ERR(("%s", __FUNCTION__));
    BDBG_MSG(("Leave %s", __FUNCTION__));

    return CSD_ERROR;

}


TCsdStatus csdSetProtectedScrDecryptionKey(
    TCsdUnsignedInt16 xEmi,
    const TCsdScrCipheredProtectingKeys xScrCipheredProtectingKeys,
    const TCsdUnsignedInt8* pxCipheredScrContentKey,
    TCsdSize xCipheredScrContentKeySize,
    const TCsdUnsignedInt8* pxInitializationVector,
    TCsdSize xInitializationVectorSize,
    TCsdScrKeyPathHandle* pxScrKeyPathHandle
)
{
    BDBG_MSG(("Enter %s", __FUNCTION__));
    return csdSetProtectedScrEncryptionDecryptionKey(
               xEmi,
               xScrCipheredProtectingKeys,
               pxCipheredScrContentKey,
               xCipheredScrContentKeySize,
               pxInitializationVector,
               xInitializationVectorSize,
               pxScrKeyPathHandle,
               false);

}



/*
'****************************************************************************************
'	read a word from OTP map
'****************************************************************************************
Function read_OTP_data(bit_address)
*/
TCsdStatus csd_P_ReadOtpData(
    unsigned int  row,
    unsigned int *pData
)
{
    unsigned int  regVal;

    NEXUS_Platform_ReadRegister(BCHP_JTAG_OTP_GENERAL_CTRL_1, &regVal);
    NEXUS_Platform_WriteRegister(BCHP_JTAG_OTP_GENERAL_CTRL_1, regVal | 0x00000001);

    NEXUS_Platform_WriteRegister(BCHP_JTAG_OTP_GENERAL_CTRL_3, 0x0000000f);
    NEXUS_Platform_WriteRegister(BCHP_JTAG_OTP_GENERAL_CTRL_0, 0x00200003);


    regVal = 0 ;
    while (( regVal & 0x1 ) == 0 )	{
        BKNI_Sleep(10);
        NEXUS_Platform_ReadRegister(BCHP_JTAG_OTP_GENERAL_STATUS_1, &regVal);
        BDBG_MSG(("Polling for status done status_reg = 0x%x\n",regVal));
    }
    NEXUS_Platform_WriteRegister(BCHP_JTAG_OTP_GENERAL_CTRL_0, 0);
    NEXUS_Platform_WriteRegister(BCHP_JTAG_OTP_GENERAL_CTRL_3, 0x00000004);

    NEXUS_Platform_WriteRegister(BCHP_JTAG_OTP_GENERAL_CTRL_0, 0x00200003);



    regVal = 0 ;
    while (( regVal & 0x1 ) == 0 )	{
        BKNI_Sleep(10);
        NEXUS_Platform_ReadRegister(BCHP_JTAG_OTP_GENERAL_STATUS_1, &regVal);
        BDBG_MSG(("Polling for status done status_reg = 0x%x\n",regVal));
    }


    NEXUS_Platform_WriteRegister(BCHP_JTAG_OTP_GENERAL_CTRL_0, 0);
    NEXUS_Platform_WriteRegister(BCHP_JTAG_OTP_GENERAL_CTRL_3, 0x00000008);

    NEXUS_Platform_WriteRegister(BCHP_JTAG_OTP_GENERAL_CTRL_0, 0x00200003);
    regVal = 0 ;
    while (( regVal & 0x1 ) == 0 )	{
        BKNI_Sleep(10);
        NEXUS_Platform_ReadRegister(BCHP_JTAG_OTP_GENERAL_STATUS_1, &regVal);
        BDBG_MSG(("Polling for status done status_reg = 0x%x\n",regVal));
    }

    NEXUS_Platform_WriteRegister(BCHP_JTAG_OTP_GENERAL_CTRL_0, 0);
    NEXUS_Platform_WriteRegister(BCHP_JTAG_OTP_GENERAL_CTRL_3, 0x0000000D);

    NEXUS_Platform_WriteRegister(BCHP_JTAG_OTP_GENERAL_CTRL_0, 0x00200003);
    regVal = 0 ;
    while (( regVal & 0x1 ) == 0 )	{
        BKNI_Sleep(10);
        NEXUS_Platform_ReadRegister(BCHP_JTAG_OTP_GENERAL_STATUS_1, &regVal);
        BDBG_MSG(("Polling for status done status_reg = 0x%x\n",regVal));
    }


    NEXUS_Platform_WriteRegister(BCHP_JTAG_OTP_GENERAL_CTRL_0, 0);
    NEXUS_Platform_WriteRegister(BCHP_JTAG_OTP_GENERAL_CTRL_3, row*32);

    NEXUS_Platform_WriteRegister(BCHP_JTAG_OTP_GENERAL_CTRL_0, 0x00000000);
    NEXUS_Platform_WriteRegister(BCHP_JTAG_OTP_GENERAL_CTRL_0, 0x00A00001);
    regVal = 0 ;
    while (( regVal & 0x1 ) == 0 )	{
        BKNI_Sleep(10);
        NEXUS_Platform_ReadRegister(BCHP_JTAG_OTP_GENERAL_STATUS_1, &regVal);
        BDBG_MSG(("Polling for status done status_reg = 0x%x\n",regVal));
    }
    NEXUS_Platform_ReadRegister(BCHP_JTAG_OTP_GENERAL_STATUS_0, &regVal);
    *pData = regVal;
    NEXUS_Platform_WriteRegister(BCHP_JTAG_OTP_GENERAL_CTRL_0, 0);

    return CSD_NO_ERROR;
}

/*#define DO_BOND_OPTION*/
/*#define TEST_ONLY*/
TCsdStatus csdGetChipExtension
(
    TCsd20CharsString xChipExtension
)
{
    unsigned int status_reg;
#if defined(DO_BOND_OPTION) && !defined(TEST_ONLY)
    unsigned int bond_option;
#endif


    if(xChipExtension == NULL)
        return CSD_ERROR;

    csd_P_ReadOtpData(13, &status_reg);
#ifndef TEST_ONLY
    xChipExtension[0] = (unsigned char)(((status_reg>>20)&0x3f)|0x40);
    xChipExtension[1] = (unsigned char)(((status_reg>>14)&0x3f)|0x40);
	xChipExtension[2] = 0;

#ifdef DO_BOND_OPTION
    /* 2012-04-27. Not required for now. */
    if((status_reg& 0xFF) < 0xA)
        /* CSA3 disabled */
        bond_option = 0x0;
    else if((status_reg& 0xFF) < 0x14)
        /* CSA3 disabled */
        bond_option = 0x1;
    else
        /* CSA3 enabled */
        bond_option = 0x2;


    xChipExtension[2] = 'K';
    xChipExtension[3] = 'F';
    xChipExtension[4] = 'E';
    xChipExtension[5] = 'B';
    xChipExtension[6] = 'A';
    xChipExtension[7] = '0'+bond_option;
    xChipExtension[8] = 'x';
    xChipExtension[9] = 'G';
	xChipExtension[10] = 0;
#endif

#else
    /* for test purpose before have a real part */
    xChipExtension[0] = 'N';
    xChipExtension[1] = 'P';
	xChipExtension[2] = 0;
#endif



    return CSD_NO_ERROR;
}

#define CSD_API_FIND_MSP(msb, value, bits)\
	for (msb=(bits-1);msb>0;msb-- )\
	{\
		if ( value >((1<<msb)-1) )\
			break;\
	}


static TCsdStatus csdProgramEpoch (unsigned char desired_epoch, bool bFwEpoch)
{
    NEXUS_ReadMspParms		readMspParams;
    NEXUS_ProgramMspIO	progMspIO;
    NEXUS_ReadMspIO readMspIO;
    NEXUS_OtpCmdMsp field = (bFwEpoch)? NEXUS_OtpCmdMsp_eReserved93:NEXUS_OtpCmdMsp_eSystemEpoch;
    unsigned char actual_epoch;
    unsigned char lock, desired_lock;

    readMspParams.readMspEnum = field;
    if(NEXUS_Security_ReadMSP (&readMspParams, &readMspIO))
        return CSD_ERROR;

    actual_epoch = readMspIO.mspDataBuf[3];
    lock = readMspIO.lockMspDataBuf[3];
#ifdef NOCS3_DEBUG_MESSAGE
    printf("actual_epoch %08x\n",actual_epoch);
    printf("lock %08x\n",lock);
#endif
    if ( (lock & actual_epoch)==desired_epoch )
    {
        /* Value in OTP is same as desired value */
        return CSD_NO_ERROR;
    }

    /* Make sure locked bits are masked out */
    desired_lock = desired_epoch ^ lock;
#ifdef NOCS3_DEBUG_MESSAGE
    printf("desired_epoch %08x\n",desired_epoch);
    printf("desired_lock %08x\n",desired_lock);
#endif

    /* Find bit location */
    progMspIO.progMode       = NEXUS_OTP_DATASECTIONPROG_MODE;
    progMspIO.progMspEnum    = field;
    progMspIO.dataBitLen     = 8;
    progMspIO.dataBitMask[0] = 0x00;
    progMspIO.dataBitMask[1] = 0x00;
    progMspIO.dataBitMask[2] = 0x00;
    progMspIO.dataBitMask[3] = desired_lock;

    progMspIO.mspData[0]     = 0;
    progMspIO.mspData[1]     = 0;
    progMspIO.mspData[2]     = 0;
    progMspIO.mspData[3]     = desired_epoch & desired_lock;

    /* Issue command to program the OTP. */
#if 1
    if(NEXUS_Security_ProgramMSP (&progMspIO))
        return CSD_ERROR;
#endif
    readMspParams.readMspEnum    = field;
    if(NEXUS_Security_ReadMSP (&readMspParams, &readMspIO))
        return CSD_ERROR;


    actual_epoch = readMspIO.mspDataBuf[3];
    lock = readMspIO.lockMspDataBuf[3];
#ifdef NOCS3_DEBUG_MESSAGE
    printf("After programming:actual_epoch %08x\n",actual_epoch);
    printf("After programming:lock %08x\n",lock);
#endif
    if(actual_epoch!= desired_epoch)
        return CSD_ERROR;

    return CSD_NO_ERROR;
}

TCsdStatus csdSetScsPv
(
    TCsdScsPvId               xScsPvParamsId,
    TCsdScsPvPathHandle*    pxPvPathHandle,
    TCsdUnsignedInt16         xPvLength,
    TCsdUnsignedInt8*        pxPvValue
)
{
    TCsdStatus status;

    CSD_UNUSED(pxPvPathHandle);

    if( CSD_INITIALIZED == FALSE )
        return CSD_ERROR;

    if  (/*(pxPvPathHandle==NULL) || */(xPvLength==0) || (pxPvValue==NULL))
    {
        return CSD_ERROR_INVALID_PARAMETERS;
    }

    switch (xScsPvParamsId)
    {
    case CSD_SCS_PV_VERSIONING_REF:
        /* Version is 8 bits for system epoch */
        if (xPvLength !=1 )
        {
            return CSD_ERROR_INVALID_PARAMETERS;
        }

        /* program system epoch */
        status = csdProgramEpoch(pxPvValue[0], false);
        if ( status != CSD_NO_ERROR )
        {
            return status;
        }
        break;
    default:
        return CSD_ERROR_OPERATION_NOT_SUPPORTED;
    }

    return CSD_NO_ERROR;
}

static TCsdStatus csdReadEpoch (unsigned char *actual_epoch, bool bFwEpoch)
{
    NEXUS_ReadMspParms		readMspParams;
    NEXUS_ReadMspIO readMspIO;
    NEXUS_OtpCmdMsp field = (bFwEpoch)? NEXUS_OtpCmdMsp_eReserved93:NEXUS_OtpCmdMsp_eSystemEpoch;

    readMspParams.readMspEnum = field;
    if(NEXUS_Security_ReadMSP (&readMspParams, &readMspIO))
        return CSD_ERROR;

    *actual_epoch = readMspIO.mspDataBuf[3];


    return CSD_NO_ERROR;
}

TCsdStatus csdGetScsPv
(
    TCsdScsPvId               xScsPvParamsId,
    TCsdScsPvPathHandle*    pxPvPathHandle,
    TCsdUnsignedInt16         xPvLength,
    TCsdUnsignedInt8*        pxPvValue
)
{
    TCsdStatus status;

    CSD_UNUSED(pxPvPathHandle);

    if( CSD_INITIALIZED == FALSE )
        return CSD_ERROR;

    if  (/*(pxPvPathHandle==NULL) || */(xPvLength==0) || (pxPvValue==NULL))
    {
        return CSD_ERROR_INVALID_PARAMETERS;
    }

    switch (xScsPvParamsId)
    {
    case CSD_SCS_PV_VERSIONING_REF:
        /* Version is 16 bits, 8 for BSECK FW epoch and 8 for system epoch */
        if (xPvLength !=1 )
        {
            return CSD_ERROR_INVALID_PARAMETERS;
        }

        /* Read system epoch */
        status = csdReadEpoch(&pxPvValue[0], false);
        if ( status != CSD_NO_ERROR )
        {
            return status;
        }
        break;
    default:
        return CSD_ERROR_OPERATION_NOT_SUPPORTED;
    }

    return CSD_NO_ERROR;
}


TCsdStatus csdSetRamUnscrambledStorageSize
(
    TCsdUnsignedInt8*        pxPvRamUnscrambledStorageSize,
    TCsdUnsignedInt16         xPvLength
)
{
    BSTD_UNUSED (pxPvRamUnscrambledStorageSize);
    BSTD_UNUSED (xPvLength);

    return CSD_ERROR_OPERATION_NOT_SUPPORTED;
}

TCsdStatus csdGetRamUnscrambledStorageSize
(
    TCsdUnsignedInt8*        pxPvRamUnscrambledStorageSize,
    TCsdUnsignedInt16         xPvLength
)
{
    BSTD_UNUSED (pxPvRamUnscrambledStorageSize);
    BSTD_UNUSED (xPvLength);

    return CSD_ERROR_OPERATION_NOT_SUPPORTED;
}


TCsdStatus csdEnableRamScrambling
(
    void
)
{
    NEXUS_ReadMspParms readMspParams;
    NEXUS_ProgramMspIO progMspIO;
    NEXUS_ReadMspIO readMspIO;

    if(CSD_INITIALIZED == FALSE )
        return CSD_ERROR;

    readMspParams.readMspEnum = NEXUS_OtpCmdMsp_eForceDramScrambler;
    if(NEXUS_Security_ReadMSP (&readMspParams, &readMspIO))
        return CSD_ERROR;

    /* Is the Dramscrambler enabled? */
    if(readMspIO.mspDataBuf[3] == 0)
    {

        progMspIO.progMode       = NEXUS_OTP_DATASECTIONPROG_MODE;
        progMspIO.progMspEnum = NEXUS_OtpCmdMsp_eForceDramScrambler;
        progMspIO.dataBitLen     = 1;
        progMspIO.dataBitMask[0] = 0x00;
        progMspIO.dataBitMask[1] = 0x00;
        progMspIO.dataBitMask[2] = 0x00;
        progMspIO.dataBitMask[3] = 0x01;
        progMspIO.mspData[0]     = 0x00;
        progMspIO.mspData[1]     = 0x00;
        progMspIO.mspData[2]     = 0x00;
        progMspIO.mspData[3]     = 0x01;

        /* Issue command to program the OTP. */
        if(NEXUS_Security_ProgramMSP (&progMspIO))
            return CSD_ERROR;

        readMspParams.readMspEnum = NEXUS_OtpCmdMsp_eForceDramScrambler;
        if(NEXUS_Security_ReadMSP (&readMspParams, &readMspIO))
            return CSD_ERROR;

        if(progMspIO.mspData[3] != 0x1)
            return CSD_ERROR;
    }


    return CSD_NO_ERROR;
}

TCsdStatus csdGetScramblingActivationFlag
(
    TCsdActivationFlag* pxScramblingActivationFlagState
)
{
    NEXUS_ReadMspParms readMspParams;
    /*NEXUS_ProgramMspIO progMspIO;*/
    NEXUS_ReadMspIO readMspIO;

    if(CSD_INITIALIZED == FALSE )
        return CSD_ERROR;

    if ( pxScramblingActivationFlagState == NULL )
        return CSD_ERROR_INVALID_PARAMETERS;

    readMspParams.readMspEnum = NEXUS_OtpCmdMsp_eForceDramScrambler;
    if(NEXUS_Security_ReadMSP (&readMspParams, &readMspIO))
        return CSD_ERROR;

    /* Is the Dramscrambler enabled? */
    if(readMspIO.mspDataBuf[3] == 1)
    {

        *pxScramblingActivationFlagState = CSD_ACTIVATION_FLAG_SET;
    } else *pxScramblingActivationFlagState = CSD_ACTIVATION_FLAG_NOT_SET;


    return CSD_NO_ERROR;
}

#if NEXUS_PLATFORM_97241==NEXUS_PLATFORM_VERSION(12,4)
#define NEXUS_OtpCmdMsp_eReserved155 NEXUS_OtpCmdMsp_eFlashDecryptionActivation
#endif
TCsdStatus csdEnableFlashProtection
(
    void
)
{
    NEXUS_ReadMspParms readMspParams;
    NEXUS_ProgramMspIO progMspIO;
    NEXUS_ReadMspIO readMspIO;

    if(CSD_INITIALIZED == FALSE )
        return CSD_ERROR;

		readMspParams.readMspEnum = NEXUS_OtpCmdMsp_eReserved155;
    /*	readMspParams.readMspEnum = NEXUS_OtpCmdMsp_eFlashDecryptionActivation; */
    if(NEXUS_Security_ReadMSP (&readMspParams, &readMspIO))
        return CSD_ERROR;

    /* Is the Dramscrambler enabled? */
    if(readMspIO.mspDataBuf[3] == 0)
    {

        progMspIO.progMode       = NEXUS_OTP_DATASECTIONPROG_MODE;
        progMspIO.progMspEnum = NEXUS_OtpCmdMsp_eReserved155;
        /*	progMspIO.progMspEnum = NEXUS_OtpCmdMsp_eFlashDecryptionActivation; */
        progMspIO.dataBitLen     = 1;
        progMspIO.dataBitMask[0] = 0x00;
        progMspIO.dataBitMask[1] = 0x00;
        progMspIO.dataBitMask[2] = 0x00;
        progMspIO.dataBitMask[3] = 0x01;
        progMspIO.mspData[0]     = 0x00;
        progMspIO.mspData[1]     = 0x00;
        progMspIO.mspData[2]     = 0x00;
        progMspIO.mspData[3]     = 0x01;

        /* Issue command to program the OTP. */
        if(NEXUS_Security_ProgramMSP (&progMspIO))
            return CSD_ERROR;

        readMspParams.readMspEnum = NEXUS_OtpCmdMsp_eReserved155;
        /*	readMspParams.readMspEnum = NEXUS_OtpCmdMsp_eFlashDecryptionActivation; */
        if(NEXUS_Security_ReadMSP (&readMspParams, &readMspIO))
            return CSD_ERROR;

        if(progMspIO.mspData[3] != 0x1)
            return CSD_ERROR;
    }



    return CSD_NO_ERROR;
}


TCsdStatus csdGetFlashProtectionActivationFlag
(
    TCsdActivationFlag* pxFlashProtectionActivationFlagState
)
{
    NEXUS_ReadMspParms readMspParams;
    /*NEXUS_ProgramMspIO progMspIO;*/
    NEXUS_ReadMspIO readMspIO;

    if(CSD_INITIALIZED == FALSE )
        return CSD_ERROR;

    if ( pxFlashProtectionActivationFlagState == NULL )
        return CSD_ERROR_INVALID_PARAMETERS;

    readMspParams.readMspEnum = NEXUS_OtpCmdMsp_eReserved155;
    /*	readMspParams.readMspEnum = NEXUS_OtpCmdMsp_eFlashDecryptionActivation; */
    if(NEXUS_Security_ReadMSP (&readMspParams, &readMspIO))
        return CSD_ERROR;

    /* Is the Dramscrambler enabled? */
    if(readMspIO.mspDataBuf[3] == 1)
    {

        *pxFlashProtectionActivationFlagState = CSD_ACTIVATION_FLAG_SET;
    } else *pxFlashProtectionActivationFlagState = CSD_ACTIVATION_FLAG_NOT_SET;


    return CSD_NO_ERROR;
}

TCsdStatus csdEncryptData
(
    TCsdUnsignedInt16       xEmi,
    TCsdR2RKeyPathHandle*  pxR2RKeyPathHandle,
    const TCsdUnsignedInt8*      pxInitializationVector,
    TCsdSize                xInitializationVectorSize,
    const TCsdUnsignedInt8*      pxInputData,
    TCsdUnsignedInt8*      pxOutputData,
    TCsdSize                xDataSize
)
{
    NEXUS_SecurityClearKey key;
    size_t xRequiredIvSize = 0;

    if (CSD_INITIALIZED == FALSE)
        return CSD_ERROR;

    if (pxR2RKeyPathHandle->keyHandle == NULL) {
        BDBG_ERR(("R2R key is not loaded!"));
        return CSD_ERROR;
    }

    if (!brcmEmiIsValid(xEmi))
        return CSD_ERROR_INVALID_PARAMETERS;

    /* load IV key if needed */
    xRequiredIvSize = brcmEmiGetIvSize(xEmi);
    if (xRequiredIvSize > 0)
    {
        if ((NULL == pxInitializationVector) ||
            (xInitializationVectorSize != xRequiredIvSize))
            return CSD_ERROR_INVALID_PARAMETERS;

        BKNI_Memset((void *)&key, 0, sizeof(key));
        NEXUS_Security_GetDefaultClearKey(&key);
        key.keyIVType 	= NEXUS_SecurityKeyIVType_eIV;
        key.keyEntryType = NEXUS_SecurityKeyType_eIv;
        key.keySize = 16;

        if(xInitializationVectorSize == 8)
            BKNI_Memcpy(&(key.keyData[8]), pxInitializationVector, xInitializationVectorSize);
        else
            BKNI_Memcpy(key.keyData, pxInitializationVector, xInitializationVectorSize);

        /* Load IV. */
        if (NEXUS_Security_LoadClearKey(pxR2RKeyPathHandle->keyHandle , &key) != 0)
        {
            BDBG_WRN(("\nLoad IV failed \n"));
            return CSD_ERROR;
        }
    }

    if(csdR2RCryptoOperation(pxR2RKeyPathHandle->keyHandle , pxInputData, pxOutputData, xDataSize))
        return CSD_ERROR;
    return CSD_NO_ERROR;
}

TCsdStatus csdDecryptData
(
    TCsdUnsignedInt16       xEmi,
    TCsdR2RKeyPathHandle*  pxR2RKeyPathHandle,
    const TCsdUnsignedInt8*      pxInitializationVector,
    TCsdSize                xInitializationVectorSize,
    const TCsdUnsignedInt8*      pxInputData,
    TCsdUnsignedInt8*      pxOutputData,
    TCsdSize                xDataSize
)
{
    NEXUS_SecurityClearKey key;
    size_t xRequiredIvSize = 0;

    if (CSD_INITIALIZED == FALSE)
        return CSD_ERROR;

    if (pxR2RKeyPathHandle->keyHandle == NULL) {
        BDBG_ERR(("R2R key is not loaded!"));
        return CSD_ERROR;
    }

    if (!brcmEmiIsValid(xEmi))
        return CSD_ERROR_INVALID_PARAMETERS;

    /* load IV key if needed */
    xRequiredIvSize = brcmEmiGetIvSize(xEmi);
    if (xRequiredIvSize > 0)
    {
        if ((NULL == pxInitializationVector) ||
            (xInitializationVectorSize != xRequiredIvSize))
            return CSD_ERROR_INVALID_PARAMETERS;

        BKNI_Memset((void *)&key, 0, sizeof(key));
        NEXUS_Security_GetDefaultClearKey(&key);
        key.keyIVType	= NEXUS_SecurityKeyIVType_eIV;
        key.keyEntryType = NEXUS_SecurityKeyType_eIv;
        key.keySize = 16; /*xInitializationVectorSize;*/

        if(xInitializationVectorSize == 8)
            BKNI_Memcpy(&(key.keyData[8]), pxInitializationVector, xInitializationVectorSize);
        else
            BKNI_Memcpy(key.keyData, pxInitializationVector, xInitializationVectorSize);

        /* Load IV. */
        if (NEXUS_Security_LoadClearKey(pxR2RKeyPathHandle->keyHandle , &key) != 0)
        {
            BDBG_WRN(("\nLoad IV failed \n"));
            return CSD_ERROR;
        }
    }
    if(csdR2RCryptoOperation(pxR2RKeyPathHandle->keyHandle , pxInputData, pxOutputData, xDataSize))
        return CSD_ERROR;

    return CSD_NO_ERROR;
}

#if (CSDAPI_VERSION_MEDIUM < 3)

TCsdStatus csdSetFlashProtRootKey
(
    TCsdFPKeyPathHandle*         pxFPKeyPathHandle
)
{
#if 1
    NEXUS_Error errCode;
    unsigned int inBuffer[16], outBuffer[16], actualOutSize;
    NEXUS_SecurityKeySlotInfo keyInfo;
    NEXUS_SecurityKeySlotSettings keySettings;
    NEXUS_KeySlotHandle keyHandle=NULL;

    if( CSD_INITIALIZED == FALSE )
        return CSD_ERROR;


    if(pxFPKeyPathHandle == NULL)
        return CSD_ERROR_INVALID_KEY_PATH_HANDLE;

    NEXUS_Security_GetDefaultKeySlotSettings(&keySettings);
    keySettings.keySlotEngine = NEXUS_SecurityEngine_eM2m;
    keyHandle = NEXUS_Security_AllocateKeySlot(&keySettings);


    if( keyHandle == NULL)
    {
        return	CSD_ERROR;	/* return if no keyslot is available*/
    }
    else
    {
        csdInsertHandleNode(pxFPKeyPathHandle->initHandle, keyHandle);

    }
    pxFPKeyPathHandle->keyHandle = keyHandle;
    NEXUS_Security_GetKeySlotInfo(keyHandle, &keyInfo);

    inBuffer[0] = 0x00000010;
    inBuffer[1] = 0x00000022;
    inBuffer[2] = 0xabcdef00;
    inBuffer[3] = 0xde55aa21;
    inBuffer[4] = 0x789a0008;
    inBuffer[5] = keyInfo.keySlotNumber;
    inBuffer[6] = (pxFPKeyPathHandle->bEncrypt)?1:0;


    errCode = NEXUS_Security_SubmitRawCommand (
                  inBuffer,    /* attr{nelem=sizeInBuffer;reserved=90} */
                  7,        /* size of pInBuffer in integers */
                  outBuffer,         /* [out] attr{nelem=sizeOutBuffer;reserved=90} */
                  6,        /* size of pOutBuffer in integers */
                  &actualOutSize      /* [out] number of integers written into pOutBuffer */
              );

    if ( errCode || (outBuffer[5]!=0 ))
    {
        return CSD_ERROR;
    }
#else
    NEXUS_SecurityKeySlotSettings keySettings;
    NEXUS_KeySlotHandle keyHandle=NULL;
    NEXUS_SecurityAlgorithmSettings algorithm;
    NEXUS_SecurityEncryptedSessionKey encryptedSessionkey;
    NEXUS_SecurityEncryptedControlWord encrytedCW, key4;
    TCsdR2RCipheredProtectingKeys proc_in3;
    TCsdStatus				   retCode = CSD_NO_ERROR;
    const TCsdUnsignedInt8 CONSTANTs[2][16] =
    {   {   0xDD, 0x35, 0x68, 0xB7, 0xC3, 0x54, 0xF8, 0xBC,
            0xCF, 0x8D, 0xFC, 0x95, 0xEA, 0x5C, 0xA2, 0xF6
        },
        {   0xDD, 0x35, 0x68, 0xB7, 0xC3, 0x54, 0xF8, 0xBC,
            0xCF, 0x8D, 0xFC, 0x95, 0xEA, 0x5C, 0xA2, 0xF6
        },
    };

    const TCsdUnsignedInt8 CONSTANT_2[16] = {0xDD, 0x35, 0x68, 0xB7, 0xC3, 0x54, 0xF8, 0xBC,
                                            0xCF, 0x8D, 0xFC, 0x95, 0xEA, 0x5C, 0xA2, 0xF6
                                            };

    bool bSwapAesKey = false;

    if( CSD_INITIALIZED == FALSE )
        return CSD_ERROR;


    if(pxFPKeyPathHandle == NULL)
        return CSD_ERROR_INVALID_KEY_PATH_HANDLE;
    /* Checks to make sure all input is valid. */
    /* Make sure clear key is 16 bytes. */
    /*	if(pxFPKexCipheredContentKeySize != 16)
    		return CSD_ERROR_INVALID_PARAMETERS;
    	 Make sure algorithm is AES-128 or 3DES-ABA. */


    NEXUS_Security_GetDefaultKeySlotSettings((const) &keySettings);

    keySettings.keySlotEngine = NEXUS_SecurityEngine_eM2m;
    keyHandle = NEXUS_Security_AllocateKeySlot(&keySettings);
    pxFPKeyPathHandle->keyHandle = keyHandle;


    if( keyHandle == NULL)
    {
        return	CSD_ERROR;	/* return if no keyslot is available*/
    }
    else
    {
        csdInsertHandleNode(pxFPKeyPathHandle->initHandle, keyHandle);

    }
    /* Set up encryption key */
    NEXUS_Security_GetDefaultAlgorithmSettings(&algorithm);


    algorithm.algorithm = NEXUS_SecurityAlgorithm_eAes128;
    algorithm.algorithmVar = NEXUS_SecurityAlgorithmVariant_eEcb;
    algorithm.terminationMode = NEXUS_SecurityTerminationMode_eClear;
    algorithm.solitarySelect = NEXUS_SecuritySolitarySelect_eClear;
    algorithm.ivMode = NEXUS_SecurityIVMode_eRegular;


    algorithm.keyDestEntryType = NEXUS_SecurityKeyType_eOdd;
    if(pxFPKeyPathHandle->bEncrypt)
        algorithm.operation = NEXUS_SecurityOperation_eEncrypt;
    else
        algorithm.operation = NEXUS_SecurityOperation_eDecrypt;

    algorithm.caVendorID=CSD_ASKM_CA_VENDOR_ID;

    if(CSD_FPK_NB_OF_PROTECTING_KEYS == 2)
        algorithm.askmModuleID = NEXUS_SecurityAskmModuleID_eModuleID_10;
    else
        algorithm.askmModuleID = NEXUS_SecurityAskmModuleID_eModuleID_8;
    algorithm.otpId = NEXUS_SecurityOtpId_eOtpVal;
    algorithm.testKey2Select = 0x0;
    algorithm.key2Select =	NEXUS_SecurityKey2Select_eFixedKey;
    /* Load algorithm to key table.  Set it up to do encryption. */
    if(NEXUS_Security_ConfigAlgorithm(keyHandle, &algorithm) != 0)
    {
        BDBG_WRN(("\nConfigAlg enc keyslot failed \n"));
        retCode =  CSD_ERROR;
        goto CSD_P_DONE;
    }


    /*BKNI_Memcpy((void *)&pxFPKeyPathHandle->algorithmConfig, (void *)&algorithm, sizeof(NEXUS_SecurityAlgorithmSettings));
    */
    BKNI_Memset((void *)&encryptedSessionkey, 0, sizeof(encryptedSessionkey));

    /* Load session key3 */
    encryptedSessionkey.keyladderID 	= NEXUS_SecurityKeyladderID_eA;

    encryptedSessionkey.keyGenCmdID 	= NEXUS_SecurityKeyGenCmdID_eKeyGen;
    encryptedSessionkey.keyLadderSelect = NEXUS_SecurityKeyLadderSelect_eFWKL;
    /*encryptedSessionkey.hwklLen 		= NEXUS_SecurityHWKLLenSelect_eLen3;*/
    encryptedSessionkey.keyladderType	= NEXUS_SecurityKeyladderType_eAes128;
    encryptedSessionkey.rootKeySrc		= NEXUS_SecurityRootKeySrc_eOtpKeyA;
    encryptedSessionkey.swizzleType 	= NEXUS_SecuritySwizzleType_eNone;
    encryptedSessionkey.keyEntryType	= NEXUS_SecurityKeyType_eOdd;
    encryptedSessionkey.keyDestIVType	= NEXUS_SecurityKeyIVType_eNoIV;
    encryptedSessionkey.operation		= NEXUS_SecurityOperation_eDecrypt;
    encryptedSessionkey.operationKey2	= NEXUS_SecurityOperation_eEncrypt;
    encryptedSessionkey.bASKMMode	= true;
    encryptedSessionkey.virtualKeyLadderID = NEXUS_SecurityVirtualKeyladderID_eVKL2;
    encryptedSessionkey.sessionKeyOp = NEXUS_SecuritySessionKeyOp_eNoProcess;
    encryptedSessionkey.bSwapAESKey = bSwapAesKey;

    encryptedSessionkey.cusKeyL 		= 0x00;
    encryptedSessionkey.cusKeyH 		= 0x00;
    encryptedSessionkey.cusKeyVarL	= 0x00;
    encryptedSessionkey.cusKeyVarH	= 0xFF;
    encryptedSessionkey.bRouteKey		= false;
    /*encryptedSessionkey.hwklDestAlg = algorithm.algorithm;*/
    encryptedSessionkey.rootKeySwap = false;

    if(CSD_FPK_NB_OF_PROTECTING_KEYS == 2)
        encryptedSessionkey.custSubMode =NEXUS_SecurityCustomerSubMode_eGeneric_CP_128_5;
    else
        encryptedSessionkey.custSubMode = NEXUS_SecurityCustomerSubMode_eGeneric_CP_128_4;

    encryptedSessionkey.keyMode = NEXUS_SecurityKeyMode_eRegular;

    /* Perform key swap for proc-in. */
#ifdef CSD_SWAP_PROCINS
    BKNI_Memcpy(&(proc_in3[0][0]), &(CONSTANTs[0][8]), sizeof(TCsdUnsignedInt8)*8);
    BKNI_Memcpy(&(proc_in3[0][8]), &(CONSTANTs[0][0]), sizeof(TCsdUnsignedInt8)*8);
#else
    BKNI_Memcpy(&(proc_in3[0][0]), &(CONSTANTs[0][0]), sizeof(TCsdUnsignedInt8)*16);
#endif

    BKNI_Memcpy(encryptedSessionkey.keyData, &(proc_in3[0][0]), sizeof(TCsdUnsignedInt8)*16);

    if (NEXUS_Security_GenerateSessionKey(keyHandle, &encryptedSessionkey) !=0)
    {
#ifdef CSD_DEBUG
        printf("\nLoading session key failed \n");
#endif
        retCode = CSD_ERROR;
        goto CSD_P_DONE;
    }
    BKNI_Memset((void *)&key4, 0, sizeof(key4));
    /* Load key4 */
    key4.keyladderID = NEXUS_SecurityKeyladderID_eA;
    key4.keyladderType = NEXUS_SecurityKeyladderType_eAes128;
    key4.keyLadderSelect = NEXUS_SecurityKeyLadderSelect_eFWKL;
    /*key4.hwklLen			= NEXUS_SecurityHWKLLenSelect_eLen3;*/
    key4.bASKMMode	= true;
    key4.keyEntryType = NEXUS_SecurityKeyType_eOdd;
    key4.operation = NEXUS_SecurityOperation_eDecrypt;
    key4.virtualKeyLadderID = NEXUS_SecurityVirtualKeyladderID_eVKL2;
    key4.bSwapAESKey = bSwapAesKey;
    /*key4.hwklDestAlg = algorithm.algorithm;*/
    if(CSD_FPK_NB_OF_PROTECTING_KEYS == 2)
    {
        key4.bRouteKey = false;
        key4.custSubMode = NEXUS_SecurityCustomerSubMode_eGeneric_CP_128_5;
        key4.keySize = 16;
        /* Perform key swap for proc-in. */
#ifdef CSD_SWAP_PROCINS
        BKNI_Memcpy(&(proc_in3[1][0]), &(CONSTANTs[1][8]), sizeof(TCsdUnsignedInt8)*8);
        BKNI_Memcpy(&(proc_in3[1][8]), &(CONSTANTs[1][0]), sizeof(TCsdUnsignedInt8)*8);
#else
        BKNI_Memcpy(&(proc_in3[1][0]), &(CONSTANTs[1][0]), sizeof(TCsdUnsignedInt8)*16);
#endif

        BKNI_Memcpy(key4.keyData,  &(proc_in3[1][0]), sizeof(TCsdUnsignedInt8)*16);

    }
    else
    {
        key4.bRouteKey = true;
        key4.custSubMode = NEXUS_SecurityCustomerSubMode_eGeneric_CP_128_4;
        key4.keySize = 16;
        BKNI_Memcpy(key4.keyData,  CONSTANT_2, key4.keySize);
    }

    key4.keyMode = NEXUS_SecurityKeyMode_eRegular;

    if (NEXUS_Security_GenerateControlWord(keyHandle, &key4))
    {
#ifdef CSD_DEBUG
        printf("\nLoading key4 failed for video ODD key\n");
#endif
        retCode = CSD_ERROR;
        goto CSD_P_DONE;
    }

    if(CSD_FPK_NB_OF_PROTECTING_KEYS == 2)
    {
        /* Load key5 */
        BKNI_Memset((void *)&encrytedCW, 0, sizeof(encrytedCW));
        encrytedCW.keyladderID = NEXUS_SecurityKeyladderID_eA;
        encrytedCW.keyladderType = NEXUS_SecurityKeyladderType_eAes128;
        encrytedCW.keyLadderSelect = NEXUS_SecurityKeyLadderSelect_eFWKL;
        /*encrytedCW.hwklLen			= NEXUS_SecurityHWKLLenSelect_eLen3;*/
        encrytedCW.bASKMMode	= true;
        encrytedCW.keySize = 16;
        encrytedCW.keyEntryType = NEXUS_SecurityKeyType_eOdd;

        BKNI_Memcpy(encrytedCW.keyData, CONSTANT_2, encrytedCW.keySize);
        /*	BKNI_Memcpy(&(encrytedCW.keyData[0]), &(pxCipheredContentKey[8]), sizeof(TCsdUnsignedInt8)*8);
        	BKNI_Memcpy(&(encrytedCW.keyData[8]), &(pxCipheredContentKey[0]), sizeof(TCsdUnsignedInt8)*8);
        */
        encrytedCW.operation = NEXUS_SecurityOperation_eDecrypt;
        encrytedCW.bRouteKey = true;
        encrytedCW.virtualKeyLadderID = NEXUS_SecurityVirtualKeyladderID_eVKL2;
        encrytedCW.custSubMode = NEXUS_SecurityCustomerSubMode_eGeneric_CP_128_5;
        encrytedCW.keyMode = NEXUS_SecurityKeyMode_eRegular;
        encrytedCW.bSwapAESKey = bSwapAesKey;
        /*	encrytedCW.hwklDestAlg = algorithm.algorithm;*/

        if (NEXUS_Security_GenerateKey5(keyHandle, &encrytedCW))
        {
#ifdef CSD_DEBUG
            printf("\nLoading CW key failed for video ODD key\n");
#endif
            retCode = CSD_ERROR;
            goto CSD_P_DONE;
        }
    }
#endif
    return CSD_NO_ERROR;

CSD_P_DONE:
    csdFreeHandleNode(pxFPKeyPathHandle->initHandle, keyHandle);
    return CSD_ERROR;
}

TCsdStatus csdSetClearTextFlashProtectionKey
(
    TCsdFPKeyPathHandle*         pxFPKeyPathHandle,
    const TCsdUnsignedInt8*            pxClearTextFlashProtectionKey
)
{

    NEXUS_SecurityKeySlotSettings keySettings;
    NEXUS_KeySlotHandle keyHandle;
    NEXUS_SecurityAlgorithmSettings algorithm;
    NEXUS_SecurityClearKey key;
    TCsdStatus				   retCode = CSD_NO_ERROR;

    if( CSD_INITIALIZED == FALSE )
        return CSD_ERROR;



    if(pxFPKeyPathHandle == NULL)
        return CSD_ERROR_INVALID_KEY_PATH_HANDLE;

    /* Checks to make sure all input is valid. */
    /* Make sure clear key is 16 bytes. */
    if(pxClearTextFlashProtectionKey == NULL)
        return CSD_ERROR_INVALID_PARAMETERS;

    NEXUS_Security_GetDefaultKeySlotSettings(&keySettings);
    keySettings.keySlotEngine = NEXUS_SecurityEngine_eM2m;
    if(pxFPKeyPathHandle->keyHandle == NULL) {
        keyHandle = NEXUS_Security_AllocateKeySlot(&keySettings);
        pxFPKeyPathHandle->keyHandle = keyHandle;

        if( keyHandle == NULL)
        {
            return	CSD_ERROR;	/* return if no keyslot is available*/
        }
        else
        {
            csdInsertHandleNode(pxFPKeyPathHandle->initHandle, keyHandle);
        }
    } else keyHandle = pxFPKeyPathHandle->keyHandle ;

    /* Set up encryption key */
    NEXUS_Security_GetDefaultAlgorithmSettings(&algorithm);


    algorithm.algorithm = NEXUS_SecurityAlgorithm_eAes128;
    algorithm.algorithmVar = NEXUS_SecurityAlgorithmVariant_eEcb;
    algorithm.terminationMode = NEXUS_SecurityTerminationMode_eClear;

    algorithm.enableExtIv = false;


    algorithm.keyDestEntryType = NEXUS_SecurityKeyType_eOdd;
    if(pxFPKeyPathHandle->bEncrypt)
        algorithm.operation = NEXUS_SecurityOperation_eEncrypt;
    else
        algorithm.operation = NEXUS_SecurityOperation_eDecrypt;
    /* Load algorithm to key table.  Set it up to do encryption. */
    if(NEXUS_Security_ConfigAlgorithm(keyHandle, &algorithm) != 0)
    {
        BDBG_WRN(("\nConfigAlg enc keyslot failed \n"));
        retCode =  CSD_ERROR;
        goto CSD_P_DONE;
    }



    NEXUS_Security_GetDefaultClearKey(&key);

    key.keyIVType	= NEXUS_SecurityKeyIVType_eNoIV;
    key.keyEntryType = NEXUS_SecurityKeyType_eOdd;
    key.keySize = 16;

    BKNI_Memcpy(key.keyData, pxClearTextFlashProtectionKey, 16);

    /* Load clear key to key table. */
    if (NEXUS_Security_LoadClearKey(keyHandle, &key) != 0)
    {
        BDBG_WRN(("\nLoad decryption key failed \n"));
        retCode = CSD_ERROR;
        goto CSD_P_DONE;
    }

    return CSD_NO_ERROR;

CSD_P_DONE:
    csdFreeHandleNode(pxFPKeyPathHandle->initHandle, keyHandle);
    return (retCode);

}

#if (CSDAPI_VERSION_MEDIUM==1)

TCsdStatus csdEncryptDataWithClearTextFlashProtectionKey
(
    TCsdFPKeyPathHandle*          pxFPKeyPathHandle,
    const TCsdUnsignedInt8*             pxInputData,
    TCsdUnsignedInt8*             pxOutputData,
    TCsdSize                       xDataSize
)
{

    if( CSD_INITIALIZED == FALSE )
        return CSD_ERROR;

    if(pxFPKeyPathHandle->keyHandle == NULL) {
        BDBG_ERR(("R2R key is not loaded!"));
        return CSD_ERROR;
    }

    if(csdR2RCryptoOperation(pxFPKeyPathHandle->keyHandle , pxInputData, pxOutputData, xDataSize))
        return CSD_ERROR;
    return CSD_NO_ERROR;
}
#endif

TCsdStatus csdDecryptDataWithClearTextFlashProtectionKey
(
    TCsdFPKeyPathHandle*          pxFPKeyPathHandle,
    const TCsdUnsignedInt8*             pxInputData,
    TCsdUnsignedInt8*             pxOutputData,
    TCsdSize                       xDataSize
)
{
    if( CSD_INITIALIZED == FALSE )
        return CSD_ERROR;

    if(pxFPKeyPathHandle->keyHandle == NULL) {
        BDBG_ERR(("R2R key is not loaded!"));
        return CSD_ERROR;
    }

    if(csdR2RCryptoOperation(pxFPKeyPathHandle->keyHandle , pxInputData, pxOutputData, xDataSize))
        return CSD_ERROR;
    return CSD_NO_ERROR;
}

TCsdStatus csdEncryptDataWithFlashProtRootKey
(
    TCsdFPKeyPathHandle*          pxFPKeyPathHandle,
    const TCsdUnsignedInt8*             pxInputData,
    TCsdUnsignedInt8*             pxOutputData,
    TCsdSize                       xDataSize
)
{
    if( CSD_INITIALIZED == FALSE )
        return CSD_ERROR;

    if(pxFPKeyPathHandle->keyHandle == NULL) {
        BDBG_ERR(("R2R key is not loaded!"));
        return CSD_ERROR;
    }

    if(csdR2RCryptoOperation(pxFPKeyPathHandle->keyHandle , pxInputData, pxOutputData, xDataSize))
        return CSD_ERROR;

    return CSD_NO_ERROR;
}


TCsdStatus csdDecryptDataWithFlashProtRootKey
(
    TCsdFPKeyPathHandle*          pxFPKeyPathHandle,
    const TCsdUnsignedInt8*             pxInputData,
    TCsdUnsignedInt8*             pxOutputData,
    TCsdSize                       xDataSize
)
{
    if( CSD_INITIALIZED == FALSE )
        return CSD_ERROR;

    if(pxFPKeyPathHandle->keyHandle == NULL) {
        BDBG_ERR(("R2R key is not loaded!"));
        return CSD_ERROR;
    }

    if(csdR2RCryptoOperation(pxFPKeyPathHandle->keyHandle , pxInputData, pxOutputData, xDataSize))
        return CSD_ERROR;

    return CSD_NO_ERROR;
}

#endif /* (CSDAPI_VERSION_MEDIUM < 3) */

#define CSD_R2R_PARSE_PARAMETERS(xKeySize) \
    if( CSD_INITIALIZED == FALSE )\
    {\
        BDBG_ERR(("%s: CSD not initialised", __FUNCTION__));\
        return CSD_ERROR;\
    }\
    if(pxR2RKeyPathHandle == NULL)\
    {\
        BDBG_ERR(("%s: NULL pxR2RKeyPathHandle", __FUNCTION__));\
        return CSD_ERROR_INVALID_KEY_PATH_HANDLE;\
    }\
    if(!brcmEmiIsValid(xEmi))\
    {\
        BDBG_ERR(("%s: invalid EMI (%04x)", __FUNCTION__, xEmi));\
        return CSD_ERROR_INVALID_PARAMETERS;\
    }\
    if(brcmEmiIsMpegTs(xEmi))\
    {\
        BDBG_ERR(("%s: MPEG-TS xEmi (%04x)", __FUNCTION__, xEmi));\
        return CSD_ERROR_INVALID_PARAMETERS;\
    }\
    if(xKeySize != brcmEmiGetKeySize(xEmi))\
    {\
        BDBG_ERR(("%s: Invalid key size (%d != %d)", __FUNCTION__, xKeySize, brcmEmiGetKeySize(xEmi)));\
        return CSD_ERROR_INVALID_PARAMETERS;\
    }
 
#define CSD_R2R_CONFIG_ALGORITHM \
    if(pxR2RKeyPathHandle->keyHandle)\
    {\
        keyHandle = pxR2RKeyPathHandle->keyHandle;\
    }\
    else\
    {\
        BDBG_ERR(("%s no keyslot allocated ", __FUNCTION__));   \
        return  CSD_ERROR;  \
    }\
    if(!brcmEmiSetupAlgorithm(xEmi, &algorithm))\
    {\
        BDBG_ERR(("%s brcmEmiSetupAlgorithm failed", __FUNCTION__));   \
        return CSD_ERROR;\
    }\
	algorithm.keyDestEntryType = NEXUS_SecurityKeyType_eOdd;\
	if(pxR2RKeyPathHandle->bEncrypt)\
		algorithm.operation = NEXUS_SecurityOperation_eEncrypt;\
	else \
		algorithm.operation = NEXUS_SecurityOperation_eDecrypt;\
 
TCsdStatus csdSetClearTextRam2RamHostKey
(
    TCsdUnsignedInt16              xEmi,
    TCsdR2RKeyPathHandle*         pxR2RKeyPathHandle,
    const TCsdUnsignedInt8*             pxClearTextR2RHostKey,
    TCsdSize                       xClearTextR2RHostKeySize
)
{
    NEXUS_KeySlotHandle keyHandle;
    NEXUS_SecurityAlgorithmSettings algorithm;
    NEXUS_SecurityClearKey key;
    TCsdStatus                 retCode = CSD_NO_ERROR;

    BDBG_MSG(("%s Enter", __FUNCTION__));

    CSD_R2R_PARSE_PARAMETERS(xClearTextR2RHostKeySize);

    CSD_R2R_CONFIG_ALGORITHM;

    if(NEXUS_Security_ConfigAlgorithm(keyHandle, &algorithm) != 0)
    {
        BDBG_WRN(("\nConfigAlg enc keyslot failed \n"));
        retCode =  CSD_ERROR;
        goto CSD_P_DONE;
    }

    BKNI_Memcpy((void *)&pxR2RKeyPathHandle->algorithmConfig, (void *)&algorithm, sizeof(NEXUS_SecurityAlgorithmSettings));
    \
    NEXUS_Security_GetDefaultClearKey(&key);

    key.keyIVType 	= NEXUS_SecurityKeyIVType_eNoIV;
    key.keyEntryType = NEXUS_SecurityKeyType_eOdd;
    key.keySize = xClearTextR2RHostKeySize;

    BKNI_Memcpy(key.keyData, pxClearTextR2RHostKey, sizeof(TCsdUnsignedInt8)*xClearTextR2RHostKeySize);

    /* Load clear key to key table. */
    if (NEXUS_Security_LoadClearKey(keyHandle, &key) != 0)
    {
        BDBG_WRN(("\nLoad decryption key failed \n"));
        retCode = CSD_ERROR;
        goto CSD_P_DONE;
    }

    return CSD_NO_ERROR;

CSD_P_DONE:

    return (retCode);



}



TCsdStatus csdSetProtectedRam2RamContentKeys
(
    TCsdUnsignedInt16              xEmi,
    TCsdR2RKeyPathHandle*         pxR2RKeyPathHandle,
    const TCsdR2RCipheredProtectingKeys  xR2RCipheredProtectingKeys,
    const TCsdUnsignedInt8*             pxCipheredContentKey,
    TCsdSize                       xCipheredContentKeySize
)
{
    NEXUS_KeySlotHandle keyHandle=NULL;
    NEXUS_SecurityAlgorithmSettings algorithm;
    NEXUS_SecurityEncryptedSessionKey encryptedSessionkey;
    NEXUS_SecurityEncryptedControlWord encrytedCW, key4;
    TCsdR2RCipheredProtectingKeys proc_in3;
    TCsdStatus                 retCode = CSD_NO_ERROR;

    BDBG_MSG(("%s Enter", __FUNCTION__));

    CSD_R2R_PARSE_PARAMETERS(xCipheredContentKeySize);

    CSD_R2R_CONFIG_ALGORITHM;

    algorithm.caVendorID=CSD_ASKM_CA_VENDOR_ID;

    if(CSD_R2R_NB_OF_PROTECTING_KEYS == 2)
        algorithm.askmModuleID = CSD_ASKM_MODULE_ID;
    else
        algorithm.askmModuleID = NEXUS_SecurityAskmModuleID_eModuleID_8;

    algorithm.otpId = NEXUS_SecurityOtpId_eOtpVal;
    algorithm.testKey2Select = 0x0;
    algorithm.key2Select =  NEXUS_SecurityKey2Select_eFixedKey;
    /* Load algorithm to key table.  Set it up to do encryption. */
    if(NEXUS_Security_ConfigAlgorithm(keyHandle, &algorithm) != 0)
    {
        BDBG_WRN(("\nConfigAlg enc keyslot failed \n"));
        retCode =  CSD_ERROR;
        goto CSD_P_DONE;
    }


    BKNI_Memcpy((void *)&pxR2RKeyPathHandle->algorithmConfig, (void *)&algorithm, sizeof(NEXUS_SecurityAlgorithmSettings));
    BKNI_Memset((void *)&encryptedSessionkey, 0, sizeof(encryptedSessionkey));

    /* Load session key3 */
    encryptedSessionkey.keyladderID 	= NEXUS_SecurityKeyladderID_eA;
    encryptedSessionkey.keyGenCmdID 	= NEXUS_SecurityKeyGenCmdID_eKeyGen;
    encryptedSessionkey.keyLadderSelect = NEXUS_SecurityKeyLadderSelect_eHWKL;
    encryptedSessionkey.hwklLen			= NEXUS_SecurityHWKLLenSelect_eLen3;
    encryptedSessionkey.keyladderType 	= NEXUS_SecurityKeyladderType_e3Des;
    encryptedSessionkey.rootKeySrc 		= NEXUS_SecurityRootKeySrc_eOtpKeyA;
    encryptedSessionkey.swizzleType		= NEXUS_SecuritySwizzleType_eNone;
    encryptedSessionkey.keyEntryType 	= NEXUS_SecurityKeyType_eOdd;
    encryptedSessionkey.keyDestIVType 	= NEXUS_SecurityKeyIVType_eNoIV;
    encryptedSessionkey.operation 		= NEXUS_SecurityOperation_eDecrypt;
    encryptedSessionkey.operationKey2 	= NEXUS_SecurityOperation_eEncrypt;
    encryptedSessionkey.bASKMMode 	= true;
#if 0 /* HUMAX_PLATFORM_BASE */
    encryptedSessionkey.virtualKeyLadderID = NEXUS_SecurityVirtualKeyladderID_eVKL1;
#else
    encryptedSessionkey.virtualKeyLadderID = NEXUS_SecurityVirtualKeyladderID_eVKL4;
#endif
    encryptedSessionkey.sessionKeyOp = NEXUS_SecuritySessionKeyOp_eNoProcess;
    encryptedSessionkey.bSwapAESKey = false;

    encryptedSessionkey.cusKeyL 		= 0x00;
    encryptedSessionkey.cusKeyH 		= 0x00;
    encryptedSessionkey.cusKeyVarL 	= 0x00;
    encryptedSessionkey.cusKeyVarH 	= 0xFF;
    encryptedSessionkey.bRouteKey 		= false;
    encryptedSessionkey.hwklDestAlg = algorithm.algorithm;
    encryptedSessionkey.rootKeySwap	= false;

    if(CSD_R2R_NB_OF_PROTECTING_KEYS == 2)
        encryptedSessionkey.custSubMode =NEXUS_SecurityCustomerSubMode_eReserved20;
    else
        encryptedSessionkey.custSubMode = NEXUS_SecurityCustomerSubMode_eGeneric_CP_128_4;

    encryptedSessionkey.keyMode = NEXUS_SecurityKeyMode_eRegular;

    /* Perform key swap for proc-in. */
    BKNI_Memcpy(&(proc_in3[0][0]), &(xR2RCipheredProtectingKeys[0][8]), sizeof(TCsdUnsignedInt8)*8);
    BKNI_Memcpy(&(proc_in3[0][8]), &(xR2RCipheredProtectingKeys[0][0]), sizeof(TCsdUnsignedInt8)*8);

    BKNI_Memcpy(encryptedSessionkey.keyData, &(proc_in3[0][0]), sizeof(TCsdUnsignedInt8)*16);

    if (NEXUS_Security_GenerateSessionKey(keyHandle, &encryptedSessionkey) !=0)
    {
#ifdef CSD_DEBUG
        printf("\nLoading session key failed \n");
#endif
        retCode = CSD_ERROR;
        goto CSD_P_DONE;
    }
    BKNI_Memset((void *)&key4, 0, sizeof(key4));
    /* Load key4 */
    key4.keyladderID = NEXUS_SecurityKeyladderID_eA;
    key4.keyladderType = NEXUS_SecurityKeyladderType_e3Des;
    key4.keyLadderSelect = NEXUS_SecurityKeyLadderSelect_eHWKL;
    key4.hwklLen			= NEXUS_SecurityHWKLLenSelect_eLen3;
    key4.bASKMMode 	= true;
    key4.keyEntryType = NEXUS_SecurityKeyType_eOdd;
    key4.operation = NEXUS_SecurityOperation_eDecrypt;
#if 0 /* HUMAX_PLATFORM_BASE */
    key4.virtualKeyLadderID = NEXUS_SecurityVirtualKeyladderID_eVKL1;
#else
    key4.virtualKeyLadderID = NEXUS_SecurityVirtualKeyladderID_eVKL4;
#endif
    key4.hwklDestAlg = algorithm.algorithm;
    if(CSD_R2R_NB_OF_PROTECTING_KEYS == 2)
    {
        key4.bRouteKey = false;
        key4.custSubMode = NEXUS_SecurityCustomerSubMode_eReserved20;
        key4.keySize = 16;
        /* Perform key swap for proc-in. */
        BKNI_Memcpy(&(proc_in3[1][0]), &(xR2RCipheredProtectingKeys[1][8]), sizeof(TCsdUnsignedInt8)*8);
        BKNI_Memcpy(&(proc_in3[1][8]), &(xR2RCipheredProtectingKeys[1][0]), sizeof(TCsdUnsignedInt8)*8);

        BKNI_Memcpy(key4.keyData,  &(proc_in3[1][0]), sizeof(TCsdUnsignedInt8)*16);

    }
    else
    {
        key4.bRouteKey = true;
        key4.custSubMode = NEXUS_SecurityCustomerSubMode_eGeneric_CP_128_4;
        key4.keySize = xCipheredContentKeySize;
        BKNI_Memcpy(key4.keyData,  pxCipheredContentKey, sizeof(TCsdUnsignedInt8)*xCipheredContentKeySize);
    }

    key4.keyMode = NEXUS_SecurityKeyMode_eRegular;

    if (NEXUS_Security_GenerateControlWord(keyHandle, &key4))
    {
#ifdef CSD_DEBUG
        printf("\nLoading key4 failed for video ODD key\n");
#endif
        retCode = CSD_ERROR;
        goto CSD_P_DONE;
    }

    if(CSD_R2R_NB_OF_PROTECTING_KEYS == 2)
    {
        /* Load key5 */
        BKNI_Memset((void *)&encrytedCW, 0, sizeof(encrytedCW));
        encrytedCW.keyladderID = NEXUS_SecurityKeyladderID_eA;
        encrytedCW.keyladderType = NEXUS_SecurityKeyladderType_e3Des;
        encrytedCW.keyLadderSelect = NEXUS_SecurityKeyLadderSelect_eHWKL;
        encrytedCW.hwklLen			= NEXUS_SecurityHWKLLenSelect_eLen3;
        encrytedCW.bASKMMode 	= true;
        encrytedCW.keySize = xCipheredContentKeySize;
        encrytedCW.keyEntryType = NEXUS_SecurityKeyType_eOdd;

        BKNI_Memcpy(encrytedCW.keyData, pxCipheredContentKey, sizeof(TCsdUnsignedInt8)*xCipheredContentKeySize);

        encrytedCW.operation = NEXUS_SecurityOperation_eDecrypt;
        encrytedCW.bRouteKey = true;
#if 0 /* HUMAX_PLATFORM_BASE */
        encrytedCW.virtualKeyLadderID = NEXUS_SecurityVirtualKeyladderID_eVKL1;
#else
        encrytedCW.virtualKeyLadderID = NEXUS_SecurityVirtualKeyladderID_eVKL4;
#endif
        encrytedCW.custSubMode = NEXUS_SecurityCustomerSubMode_eReserved20;
        encrytedCW.keyMode = NEXUS_SecurityKeyMode_eRegular;
        encrytedCW.hwklDestAlg = algorithm.algorithm;

        if (NEXUS_Security_GenerateKey5(keyHandle, &encrytedCW))
        {
#ifdef CSD_DEBUG
            printf("\nLoading CW key failed for video ODD key\n");
#endif
            retCode = CSD_ERROR;
            goto CSD_P_DONE;
        }
    }
#if 0
    /* Check if IV will be loaded. */
    if((xMode == CSD_R2R_CRYPTO_OPERATION_MODE_CBC) &&
            (xRefreshIv == TRUE))
    {
        key.keyIVType 	= NEXUS_SecurityKeyIVType_eIV;
        key.keyEntryType = NEXUS_SecurityKeyType_eIv;
        key.keySize = 16;

        memset (key.keyData, 0, sizeof(TCsdUnsignedInt8)*16);

        if(xAlgorithm == CSD_R2R_ALGORITHM_TDES_K1K2K1)
            BKNI_Memcpy(&(key.keyData[8]), pxInitializationVector, sizeof(TCsdUnsignedInt8)*xInitializationVectorSize);
        else
            BKNI_Memcpy(key.keyData, pxInitializationVector, sizeof(TCsdUnsignedInt8)*xInitializationVectorSize);

        /* Load IV. */
        if (NEXUS_Security_LoadClearKey(keyHandle, &key) != 0)
        {
            BDBG_WRN(("\nLoad IV failed \n"));
            retCode = CSD_ERROR;
            goto CSD_P_DONE;
        }
    }


#endif
    return CSD_NO_ERROR;

CSD_P_DONE:

    return (retCode);


}

TCsdStatus csdSetProtectedScrEncryptionKey
(
    TCsdUnsignedInt16               xEmi,
    const TCsdScrCipheredProtectingKeys   xScrCipheredProtectingKeys,
    const TCsdUnsignedInt8*               pxCipheredScrContentKey,
    TCsdSize                        xCipheredScrContentKeySize,
    const TCsdUnsignedInt8*               pxInitializationVector,
    TCsdSize                        xInitializationVectorSize,
    TCsdScrKeyPathHandle*           pxScrKeyPathHandle
)
{
    BDBG_MSG(("Enter %s", __FUNCTION__));
    return csdSetProtectedScrEncryptionDecryptionKey(
               xEmi,
               xScrCipheredProtectingKeys,
               pxCipheredScrContentKey,
               xCipheredScrContentKeySize,
               pxInitializationVector,
               xInitializationVectorSize,
               pxScrKeyPathHandle,
               true);
}


TCsdStatus csdGetApiVersion
(
    TCsdUnsignedInt32*  pxCsdApiVersion
)
{
#if 0
    if( CSD_INITIALIZED == FALSE )
        return CSD_ERROR;
#endif
    if(pxCsdApiVersion == NULL)
    {
        return CSD_ERROR_INVALID_PARAMETERS;
    }

    * pxCsdApiVersion = CSDAPI_VERSION_INT;

    return CSD_NO_ERROR;
}

#if (CSDAPI_VERSION_MEDIUM>1)

/***************************************************************************/
/*  The following version 2 API are for backward compatibility with NOCS1x */
/***************************************************************************/

TCsdStatus csdSetScsTotalAreaSize(
    TCsdScsTotalAreaSize xScsTotalAreaSize
)
{
    /* in Bseck mode, size is written in flash */
    BSTD_UNUSED(xScsTotalAreaSize);

    if( CSD_INITIALIZED == FALSE )
        return CSD_ERROR;

    return CSD_NO_ERROR;
}

TCsdStatus csdGetScsTotalAreaSize(
    TCsdScsTotalAreaSize* pxScsTotalAreaSize
)
{
    if( CSD_INITIALIZED == FALSE )
        return CSD_ERROR;

    if(pxScsTotalAreaSize ==	NULL)
        return CSD_ERROR;

    *pxScsTotalAreaSize = CSD_SCS_TOTAL_AREA_SIZE_NOT_SUPPORTED;

    return CSD_NO_ERROR;
}

#define CSD_R2R_NOCS_V1_CHECK_PARAMS_STATUS \
	if( CSD_INITIALIZED == FALSE )\
		return CSD_ERROR;\
	if(pxR2RKeyPathHandle == NULL)\
		return CSD_ERROR_INVALID_KEY_PATH_HANDLE;\
	if ( ( pxInputData==NULL) || (pxOutputData==NULL) || (xDataSize ==0 ))\
		return CSD_ERROR_INVALID_PARAMETERS;\
	if(xAlgorithm >= LAST_CSD_R2R_ALGORITHM)\
		return CSD_ERROR_INVALID_PARAMETERS;\
	if((xMode == CSD_R2R_CRYPTO_OPERATION_MODE_CBC) && \
		(xAlgorithm == CSD_R2R_ALGORITHM_AES_128) && \
		(xRefreshIv == TRUE) && \
		(xInitializationVectorSize != 16))\
		return CSD_ERROR_INVALID_PARAMETERS;\
	if((xMode == CSD_R2R_CRYPTO_OPERATION_MODE_CBC) && \
		(xAlgorithm == CSD_R2R_ALGORITHM_TDES_K1K2K1) && \
		(xRefreshIv == TRUE) && \
		(xInitializationVectorSize != 8))\
		return CSD_ERROR_INVALID_PARAMETERS;\
	if((xAlgorithm == CSD_R2R_ALGORITHM_AES_128) &&	\
		( (xDataSize % 16) != 0 ) )\
		return CSD_ERROR_INVALID_PARAMETERS;\
	if((xAlgorithm == CSD_R2R_ALGORITHM_TDES_K1K2K1) &&	\
		( (xDataSize % 8) != 0 ) )\
		return CSD_ERROR_INVALID_PARAMETERS;\
 
#define CSD_R2R_NOCS_V1_CHECK_PARAMS_CLEAR_KEY \
	if ( (pxClearTextHostKey==NULL) || (xClearTextHostKeySize != 16) )\
		return CSD_ERROR_INVALID_PARAMETERS;\
 
#define CSD_R2R_NOCS_V1_MAP_ALG_TO_EMI \
	switch(xAlgorithm)\
	{\
		case CSD_R2R_ALGORITHM_AES_128:\
			{\
				switch(xMode)\
				{\
					case CSD_R2R_CRYPTO_OPERATION_MODE_ECB:\
						xEmi = CSD_Algorithm_RAW_AES_128_ECB_CLEAR;\
						break;\
					case CSD_R2R_CRYPTO_OPERATION_MODE_CBC:\
						xEmi = CSD_Algorithm_RAW_AES_128_CBC_CLEAR;\
						break;\
					default:\
						status = CSD_ERROR_INVALID_PARAMETERS;\
						goto CSD_P_DONE;\
				}	\
			}\
			break;\
		case CSD_R2R_ALGORITHM_TDES_K1K2K1:\
				switch(xMode)\
				{\
					case CSD_R2R_CRYPTO_OPERATION_MODE_ECB:\
						xEmi = CSD_Algorithm_RAW_TDES_ABA_ECB_CLEAR;\
						break;\
					case CSD_R2R_CRYPTO_OPERATION_MODE_CBC:\
						xEmi = CSD_Algorithm_RAW_TDES_ABA_CBC_CLEAR;\
						break;\
					default:\
						status = CSD_ERROR_INVALID_PARAMETERS;\
						goto CSD_P_DONE;\
				}	\
			break;\
		default:\
			status = CSD_ERROR_INVALID_PARAMETERS;\
			goto CSD_P_DONE;\
	}\
 
 
TCsdStatus csdEncryptDataWithClearTextHostKey
(
    TCsdR2RAlgorithm            xAlgorithm,
    TCsdR2RCryptoOperationMode  xMode,
    const TCsdUnsignedInt8*          pxClearTextHostKey,
    TCsdSize                    xClearTextHostKeySize,
    const TCsdUnsignedInt8*          pxInitializationVector,
    TCsdSize                    xInitializationVectorSize,
    TCsdBoolean        	    xRefreshIv,
    TCsdR2RKeyPathHandle*      pxR2RKeyPathHandle,
    const TCsdUnsignedInt8*          pxInputData,
    TCsdUnsignedInt8*          pxOutputData,
    TCsdSize                    xDataSize
)
{
    TCsdStatus status = CSD_NO_ERROR;
    TCsdUnsignedInt16 xEmi;

    BDBG_MSG(("%s Enter", __FUNCTION__));

    CSD_R2R_NOCS_V1_CHECK_PARAMS_STATUS;
    CSD_R2R_NOCS_V1_CHECK_PARAMS_CLEAR_KEY;

    CSD_R2R_NOCS_V1_MAP_ALG_TO_EMI;

    pxR2RKeyPathHandle->bEncrypt = true;
    status =csdSetClearTextRam2RamHostKey(xEmi,pxR2RKeyPathHandle, pxClearTextHostKey, xClearTextHostKeySize);
    if (status!=CSD_NO_ERROR)
    {
        goto CSD_P_DONE;
    }

    if ( xRefreshIv && (xMode!=CSD_R2R_CRYPTO_OPERATION_MODE_ECB ) )
    {
        status =csdEncryptData(xEmi, \
                               pxR2RKeyPathHandle, \
                               pxInitializationVector, \
                               xInitializationVectorSize, \
                               pxInputData, \
                               pxOutputData, \
                               xDataSize);
    }
    else
    {
        status =csdEncryptData(xEmi, \
                               pxR2RKeyPathHandle, \
                               NULL, \
                               0, \
                               pxInputData, \
                               pxOutputData, \
                               xDataSize);
    }

CSD_P_DONE:
    return status;
}


TCsdStatus csdDecryptDataWithClearTextHostKey
(
    TCsdR2RAlgorithm            xAlgorithm,
    TCsdR2RCryptoOperationMode  xMode,
    const TCsdUnsignedInt8*          pxClearTextHostKey,
    TCsdSize                    xClearTextHostKeySize,
    const TCsdUnsignedInt8*          pxInitializationVector,
    TCsdSize                    xInitializationVectorSize,
    TCsdBoolean        	    xRefreshIv,
    TCsdR2RKeyPathHandle*      pxR2RKeyPathHandle,
    const TCsdUnsignedInt8*          pxInputData,
    TCsdUnsignedInt8*          pxOutputData,
    TCsdSize                    xDataSize
)
{
    TCsdStatus status = CSD_NO_ERROR;
    TCsdUnsignedInt16 xEmi;

    CSD_R2R_NOCS_V1_CHECK_PARAMS_STATUS;
    CSD_R2R_NOCS_V1_CHECK_PARAMS_CLEAR_KEY;

    CSD_R2R_NOCS_V1_MAP_ALG_TO_EMI;

    pxR2RKeyPathHandle->bEncrypt = false;
    status =csdSetClearTextRam2RamHostKey(xEmi,pxR2RKeyPathHandle, pxClearTextHostKey, xClearTextHostKeySize);
    if (status!=CSD_NO_ERROR)
    {
        goto CSD_P_DONE;
    }

    if ( xRefreshIv && (xMode!=CSD_R2R_CRYPTO_OPERATION_MODE_ECB ) )
    {
        status =csdDecryptData(xEmi, \
                               pxR2RKeyPathHandle, \
                               pxInitializationVector, \
                               xInitializationVectorSize, \
                               pxInputData, \
                               pxOutputData, \
                               xDataSize);
    }
    else
    {
        status =csdDecryptData(xEmi, \
                               pxR2RKeyPathHandle, \
                               NULL, \
                               0, \
                               pxInputData, \
                               pxOutputData, \
                               xDataSize);
    }

CSD_P_DONE:
    return status;
}

#define CSD_R2R_NOCS_V1_CHECK_PARAMS_ENCRYPTED_KEY \
	if ( (xR2RCipheredProtectingKeys==NULL) || (pxCipheredContentKey==NULL) || (xCipheredContentKeySize == 0) )\
		return CSD_ERROR_INVALID_PARAMETERS;\
 
TCsdStatus csdEncryptDataWithSecretContentKey
(
    TCsdR2RAlgorithm                xAlgorithm,
    TCsdR2RCryptoOperationMode      xMode,
    const TCsdR2RCipheredProtectingKeys   xR2RCipheredProtectingKeys,
    const TCsdUnsignedInt8*              pxCipheredContentKey,
    TCsdSize                        xCipheredContentKeySize,
    const TCsdUnsignedInt8*              pxInitializationVector,
    TCsdSize                        xInitializationVectorSize,
    TCsdBoolean                     xRefreshIv,
    TCsdR2RKeyPathHandle*          pxR2RKeyPathHandle,
    const TCsdUnsignedInt8*              pxInputData,
    TCsdUnsignedInt8*              pxOutputData,
    TCsdSize                        xDataSize
)
{
    TCsdStatus status = CSD_NO_ERROR;
    TCsdUnsignedInt16 xEmi;

    CSD_R2R_NOCS_V1_CHECK_PARAMS_STATUS;
    CSD_R2R_NOCS_V1_CHECK_PARAMS_ENCRYPTED_KEY;

    CSD_R2R_NOCS_V1_MAP_ALG_TO_EMI;

    pxR2RKeyPathHandle->bEncrypt = true;
    status =csdSetProtectedRam2RamContentKeys(xEmi,\
            pxR2RKeyPathHandle,\
            xR2RCipheredProtectingKeys,\
            pxCipheredContentKey,\
            xCipheredContentKeySize);
    if (status!=CSD_NO_ERROR)
    {
        goto CSD_P_DONE;
    }

    if ( xRefreshIv && (xMode!=CSD_R2R_CRYPTO_OPERATION_MODE_ECB ) )
    {
        status =csdEncryptData(xEmi, \
                               pxR2RKeyPathHandle, \
                               pxInitializationVector, \
                               xInitializationVectorSize, \
                               pxInputData, \
                               pxOutputData, \
                               xDataSize);
    }
    else
    {
        status =csdEncryptData(xEmi, \
                               pxR2RKeyPathHandle, \
                               NULL, \
                               0, \
                               pxInputData, \
                               pxOutputData, \
                               xDataSize);
    }

CSD_P_DONE:
    return status;
}

TCsdStatus csdDecryptDataWithSecretContentKey
(
    TCsdR2RAlgorithm                xAlgorithm,
    TCsdR2RCryptoOperationMode      xMode,
    const TCsdR2RCipheredProtectingKeys   xR2RCipheredProtectingKeys,
    const TCsdUnsignedInt8*              pxCipheredContentKey,
    TCsdSize                        xCipheredContentKeySize,
    const TCsdUnsignedInt8*              pxInitializationVector,
    TCsdSize                        xInitializationVectorSize,
    TCsdBoolean        		xRefreshIv,
    TCsdR2RKeyPathHandle*          pxR2RKeyPathHandle,
    const TCsdUnsignedInt8*              pxInputData,
    TCsdUnsignedInt8*              pxOutputData,
    TCsdSize                        xDataSize
)
{
    TCsdStatus status = CSD_NO_ERROR;
    TCsdUnsignedInt16 xEmi;

    CSD_R2R_NOCS_V1_CHECK_PARAMS_STATUS;
    CSD_R2R_NOCS_V1_CHECK_PARAMS_ENCRYPTED_KEY;

    CSD_R2R_NOCS_V1_MAP_ALG_TO_EMI;

    pxR2RKeyPathHandle->bEncrypt = false;
    status =csdSetProtectedRam2RamContentKeys(xEmi,\
            pxR2RKeyPathHandle,\
            xR2RCipheredProtectingKeys,\
            pxCipheredContentKey,\
            xCipheredContentKeySize);
    if (status!=CSD_NO_ERROR)
    {
        goto CSD_P_DONE;
    }

    if ( xRefreshIv && (xMode!=CSD_R2R_CRYPTO_OPERATION_MODE_ECB ) )
    {
        status =csdDecryptData(xEmi, \
                               pxR2RKeyPathHandle, \
                               pxInitializationVector, \
                               xInitializationVectorSize, \
                               pxInputData, \
                               pxOutputData, \
                               xDataSize);
    }
    else
    {
        status =csdDecryptData(xEmi, \
                               pxR2RKeyPathHandle, \
                               NULL, \
                               0, \
                               pxInputData, \
                               pxOutputData, \
                               xDataSize);
    }

CSD_P_DONE:
    return status;
}

#endif /* (CSDAPI_VERSION_MEDIUM>1) */
