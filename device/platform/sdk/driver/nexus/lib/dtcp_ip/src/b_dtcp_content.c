/********************************************************************************************
*     (c)2004-2012 Broadcom Corporation                                                     *
*                                                                                           *
*  This program is the proprietary software of Broadcom Corporation and/or its licensors,   *
*  and may only be used, duplicated, modified or distributed pursuant to the terms and      *
*  conditions of a separate, written license agreement executed between you and Broadcom    *
*  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants*
*  no license (express or implied), right to use, or waiver of any kind with respect to the *
*  Software, and Broadcom expressly reserves all rights in and to the Software and all      *
*  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU       *
*  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY                    *
*  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.                                 *
*
*  Except as expressly set forth in the Authorized License,                                 *
*
*  1.     This program, including its structure, sequence and organization, constitutes     *
*  the valuable trade secrets of Broadcom, and you shall use all reasonable efforts to      *
*  protect the confidentiality thereof,and to use this information only in connection       *
*  with your use of Broadcom integrated circuit products.                                   *
*                                                                                           *
*  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"          *
*  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR                   *
*  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO            *
*  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES            *
*  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,            *
*  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION             *
*  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF              *
*  USE OR PERFORMANCE OF THE SOFTWARE.                                                      *
*                                                                                           *
*  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS         *
*  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR             *
*  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR               *
*  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF             *
*  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT              *
*  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE            *
*  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF              *
*  ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: b_dtcp_content.c $
 * $brcm_Revision: 32 $
 * $brcm_Date: 11/9/12 5:12p $
 * 
 * Module Description:
 *     DTCP-IP Content management component
 *
 * Revision History:
 * 
 * $brcm_Log: /nexus/lib/dtcp_ip/src/b_dtcp_content.c $
 * 
 * 32   11/9/12 5:12p cdisc
 * SWSECURITY-205: adding DMA buffer compliant code and cleanup
 * 
 * 31   10/4/12 11:29a cdisc
 * SW7425-3979: DTCP-IP streamlining
 * 
 * SW7425-3979/1   9/28/12 2:35p cdisc
 * SW7425-3979: making common DRM default path
 * 
 * 30   9/11/12 2:52p leisun
 * SWSECURITY-48: Auto-generating test keys, consolidate test/production
 *  build, make common DRM default ON
 * 
 * 29   5/23/12 2:39p celysee
 * SW7425-2894: Adding common drm and dtcp-ip
 * 
 * 28   4/18/12 6:16p celysee
 * SW7425-2894: Support for DTCP-IP with robustness rules
 * 
 * 27   4/2/12 4:17p leisun
 * SW7425-2751: coverity report fix
 * 
 * 26   2/28/12 5:50p ssood
 * SW7425-2514: get the default values of the key slots
 * 
 * 25   2/28/12 12:35p ssood
 * SW7425-2514: get the default values of the key slots
 * 
 * 24   2/10/12 4:20p leisun
 * SWSECURITY-107: DTCP-IP support for 7429
 * 
 * 23   11/8/11 2:01p leisun
 * SWSECURITY-85: DTCP-IP fail to decrypt when there is padding bytes
 * 
 * 22   3/10/11 10:32a leisun
 * SW7422-269: change final form of IV enum
 * 
 * 21   3/7/11 3:24p leisun
 * SW7422-269: nexus security API for newer chip
 * 
 * 19   2/11/11 5:39p leisun
 * SWSECURITY-32: Updated lib to V1SE1.3 conformance.
 * 
 * 18   12/1/10 4:35p leisun
 * SW7408-187: 7408 BE mode fix
 * 
 * 17   12/1/10 1:21p leisun
 * SW7408-187: DTCP-IP for BE platform fix
 * 
 * 16   6/1/10 2:33p leisun
 * SW7420-561: DTCP-IP, dumping correct debugging messages
 * 
 * 15   5/24/10 5:14p leisun
 * SW7420-561: Fix for coverity defects report and compiler warnings
 * 
 * 14   4/8/10 6:00p sgundime
 * SW7420-689: memset the pPacketHeader. This fixes the LPTT issue 7.7.1.2
 *  error, DTCP PCP Header Reserved bits were not Zero in PCP.
 * 
 * 13   3/23/10 6:57p leisun
 * SW7420-561: Fixes related to DMS with PS3 client
 * 
 * 12   12/11/09 2:39p leisun
 * SW7420-502: conditional compile for DTCP-IP
 * 
 * 11   10/22/09 1:07p ssood
 * SW7420-340: add bkni_multi.h header file for compiling on SMS platforms
 * 
 * 10   10/8/09 11:12a leisun
 * SW7405-3138: Fixes for live HTTP streaming
 * 
 * 9   10/7/09 11:46a leisun
 * SW7405-3138: Fixes for live HTTP streaming
 * 
 * 8   7/31/09 3:35p leisun
 * PR 55693: removed bcrypt dependency
 * 
 * 7   7/16/09 7:02p lwhite
 * PR55693: Fixed signedness
 * 
 * 6   7/13/09 11:19a leisun
 * PR 55693: Re-design interfaces, and other cleanups
 * 
 * 5   6/30/09 10:43p leisun
 * PR 55693: Added exchange key expiration timer, and other fixes
 * 
 * 4   6/25/09 3:47p ssood
 * PR53773: Allow app to specify Nexus M2M handle if available
 * 
 * 3   6/24/09 3:31p leisun
 * PR 55693: checking in DTCP-IP lib, fixed PCP handling error when stream
 *  larger then max PCP size
 * 
 * 2   6/11/09 10:54a leisun
 * PR 55907: Add HW M2M feature for DTCP-IP
 * 
 * 1   6/5/09 4:07p leisun
 * PR 55693: checking in DTCP-IP lib
 *********************************************************************************************/
#include <arpa/inet.h>
#include "bstd.h"
#include "bdbg.h"
#include "bkni.h"
#include "bkni_multi.h"
#include "b_dtcp_applib.h"
#include "b_dtcp_ake.h"
#include "b_dtcp_ip_ake.h"
#include "b_dtcp_stream.h"
#include "b_dtcp_content.h"
#include "b_dtcp_ip_constants.h"
#include "b_dtcp_status_codes.h"

#include "bcrypt_aescbc_sw.h"
#include "bcrypt_aesecb_sw.h"

#ifdef B_DTCP_IP_COMMON_DRM_CONTENT_SUPPORT
#include "drm_init.h"
#endif

BDBG_MODULE(b_dtcp_ip);
/* 
 * DTCP-IP content key constants, defined in b_ecc_wrappers.c
 */
extern unsigned char gCa0[];
extern unsigned char gCb1[];
extern unsigned char gCb0[];
extern unsigned char gCc1[];
extern unsigned char gCc0[];
extern unsigned char gCd0[];

/*! \brief Data associated with a protected content packet */
typedef struct DtcpPacketData
{
    BCRYPT_S_AESCBCSwCtrl_t *cipherContext;
	unsigned char NextIv[DTCP_AES_IV_SIZE];
    unsigned int    PayloadSize;
    unsigned int    BytesProcessed;
	B_ExEMI_T		ExtendedEmi;
} DtcpPacketData;
typedef struct {
	void * dma_in;
	void * dma_out;
	unsigned int  LastBufferLength;
} dma_t;
static dma_t  DtcpDma;

#if defined(B_DTCP_IP_HW_DECRYPTION) || defined(B_DTCP_IP_HW_ENCRYPTION)
#ifdef B_DTCP_IP_COMMON_DRM_CONTENT_SUPPORT

#include "drm_dtcp_ip.h"
#include "drm_dtcp_ip_vendor.h"
#include "nexus_memory.h"
static B_MutexHandle hSecurityMutex;

#else
#include "nexus_platform.h"
#include "nexus_memory.h"
#include "nexus_dma.h"
#include "nexus_security.h"
#include <unistd.h>

typedef struct {
	NEXUS_DmaJobHandle job;
	bool busy;
	NEXUS_DmaJobBlockSettings block;
	BKNI_EventHandle event;
	void * dma_in;
	void * dma_out;
	unsigned int  LastBufferLength;
} dma_t;

static dma_t  DtcpDma;

static NEXUS_KeySlotHandle DecKeyHandle;

#endif

void DtcpCleanupHwSecurityParams(void)
{
#ifdef B_DTCP_IP_COMMON_DRM_CONTENT_SUPPORT
        BDBG_MSG(("%s - Entered function", __FUNCTION__));
        DRM_DtcpIp_Vendor_Finalize();
#else
	if(DtcpDma.job != NULL)
		NEXUS_DmaJob_Destroy(DtcpDma.job);

	if(DecKeyHandle != NULL)
		NEXUS_Security_FreeKeySlot(DecKeyHandle);

    if (DtcpDma.event)
        BKNI_DestroyEvent(DtcpDma.event);

	if(hSecurityMutex != NULL) 
		B_Mutex_Destroy(hSecurityMutex);

	if(DtcpDma.dma_in != NULL)
		NEXUS_Memory_Free(DtcpDma.dma_in);
	
	if(DtcpDma.dma_out != NULL)
		NEXUS_Memory_Free(DtcpDma.dma_out);

#endif /*B_DTCP_IP_COMMON_DRM_CONTENT_SUPPORT*/
}

#ifndef B_DTCP_IP_COMMON_DRM_CONTENT_SUPPORT
static void DtcpEncCompleteCallback(void *pParam, int iParam)
{
    BSTD_UNUSED(iParam);
    BKNI_SetEvent(pParam);
}
#endif

/*
 * Must be called by app, if using hardware descryption.
 */
#ifdef B_DTCP_IP_COMMON_DRM_CONTENT_SUPPORT
BERR_Code DtcpInitHWSecurityParams(void *nexusDmaHandle)
{
    BERR_Code rc = BERR_SUCCESS;

    BDBG_MSG(("%s - Entered function", __FUNCTION__));

    if(DRM_DtcpIp_Vendor_Initialize() != Drm_Success) {
    	BDBG_ERR(("%s - Error initializing DRM DTCP-IP module", __FUNCTION__));
    	rc = BERR_UNKNOWN;
    	goto error;
    }

	if((hSecurityMutex = B_Mutex_Create(NULL)) == NULL)
	{
		BDBG_ERR(("Failed: B_Mutex_Create\n"));
		goto error;
	}


error:
	return rc;
}
#endif /* end of B_DTCP_IP_COMMON_DRM_CONTENT_SUPPORT */

#ifndef B_DTCP_IP_COMMON_DRM_CONTENT_SUPPORT
static BERR_Code LoadKeyIv(unsigned char * CipherKey, unsigned char * CipherIv)
{
	BERR_Code retValue = BERR_SUCCESS;
	NEXUS_SecurityClearKey key;
	BDBG_ENTER(LoadKeyIv);
	
    NEXUS_Security_GetDefaultClearKey(&key); 
	key.keyEntryType = NEXUS_SecurityKeyType_eOdd;
	key.keySize = DTCP_AES_KEY_SIZE;
#if (BCHP_CHIP == 7422 || BCHP_CHIP == 7425 || BCHP_CHIP == 7231 || \
		BCHP_CHIP == 7344 || BCHP_CHIP == 7346 || BCHP_CHIP == 7429)
    key.keyIVType = NEXUS_SecurityKeyIVType_eNoIV;
#endif
	BKNI_Memcpy(key.keyData, CipherKey, DTCP_AES_KEY_SIZE);
	
	if(NEXUS_Security_LoadClearKey(DecKeyHandle, &key) != 0)
	{
		BDBG_ERR(("Failed to load key\n"));
		retValue = BERR_HW_SECURITY_FAILURE; 
	}
	
	key.keySize = DTCP_AES_IV_SIZE;
#if (BCHP_CHIP == 7422 || BCHP_CHIP == 7425 || BCHP_CHIP == 7231 || \
		BCHP_CHIP == 7344 || BCHP_CHIP == 7346 || BCHP_CHIP == 7429)
    key.keyIVType = NEXUS_SecurityKeyIVType_eIV;
#else
	key.keyEntryType = NEXUS_SecurityKeyType_eIv;
#endif
	BKNI_Memcpy(key.keyData, CipherIv, DTCP_AES_IV_SIZE);
	
	if(NEXUS_Security_LoadClearKey(DecKeyHandle, &key) != 0)
	{
		BDBG_ERR(("Failed to load iv\n"));
		retValue = BERR_HW_SECURITY_FAILURE;
	}
	BDBG_LEAVE(LoadKeyIv);

	return retValue;
}
#endif /* end of B_DTCP_IP_COMMON_DRM_CONTENT_SUPPORT */

#endif /* B_DTCP_IP_HW_DECRYPTION */
static BERR_Code CreateContentKey(BCRYPT_Handle ghBcrypt, B_ExEMI_T aExtendedEmi,
                     unsigned char aExchangeKey[DTCP_EXCHANGE_KEY_SIZE],
                     unsigned char aNonce[DTCP_CONTENT_KEY_NONCE_SIZE],
                     unsigned char aCipherKey[DTCP_AES_KEY_SIZE],
                     unsigned char aCipherIv[DTCP_AES_IV_SIZE])
{
    BERR_Code retValue = BERR_SUCCESS;
	unsigned char contentKeyConstant[DTCP_CONTENT_KEY_CONSTANT_SIZE] = {0};
	unsigned char ivConstant[DTCP_IV_CONSTANT_SIZE] = {0};
	unsigned char keyGenBuffer[2 * DTCP_AES_BLOCK_SIZE] = {0};
	unsigned char ivGenBuffer[DTCP_CONTENT_KEY_NONCE_SIZE + DTCP_IV_CONSTANT_SIZE] = {0};
    BCRYPT_S_AESECBSwCtrl_t cipherContext ;
    int i;

	BDBG_ASSERT(aExchangeKey);
	BDBG_ASSERT(aNonce);
	BDBG_ASSERT(aCipherKey);

    if (B_ExtEmi_eCopyNever == aExtendedEmi)
    {
		BKNI_Memcpy(contentKeyConstant, gCa0, DTCP_CONTENT_KEY_CONSTANT_SIZE);
    }
    else if (B_ExtEmi_eCopyOneFC == aExtendedEmi)
    {
		BKNI_Memcpy(contentKeyConstant, gCb1, DTCP_CONTENT_KEY_CONSTANT_SIZE);
    }
    else if (B_ExtEmi_eCopyOneFN == aExtendedEmi)
    {
		BKNI_Memcpy(contentKeyConstant, gCb0, DTCP_CONTENT_KEY_CONSTANT_SIZE);
    }
    else if (B_ExtEmi_eMove == aExtendedEmi)
    {
		BKNI_Memcpy(contentKeyConstant, gCc1, DTCP_CONTENT_KEY_CONSTANT_SIZE);
    }
    else if (B_ExtEmi_eNoMoreCopy == aExtendedEmi)
    {
		BKNI_Memcpy(contentKeyConstant, gCc0, DTCP_CONTENT_KEY_CONSTANT_SIZE);
    }
    else if (B_ExtEmi_eCopyFreeEPN == aExtendedEmi)
    {
		BKNI_Memcpy(contentKeyConstant, gCd0, DTCP_CONTENT_KEY_CONSTANT_SIZE);
    }
    else if (B_ExtEmi_eCopyFree == aExtendedEmi)
    {
        /*retValue = SUCCESS_FALSE; */
    }
    else
    {
        retValue = BERR_INVALID_PARAMETER;
    }

    BKNI_Memcpy(keyGenBuffer,
           aExchangeKey,
           DTCP_EXCHANGE_KEY_SIZE);
    BKNI_Memcpy(keyGenBuffer + DTCP_EXCHANGE_KEY_SIZE,
           contentKeyConstant, 
           DTCP_CONTENT_KEY_CONSTANT_SIZE);
    BKNI_Memcpy(keyGenBuffer + DTCP_EXCHANGE_KEY_SIZE + DTCP_CONTENT_KEY_CONSTANT_SIZE,
           aNonce,
           DTCP_CONTENT_KEY_NONCE_SIZE);
	cipherContext.pIn = keyGenBuffer + DTCP_AES_BLOCK_SIZE;
	cipherContext.pkey = (unsigned char *)keyGenBuffer;
	cipherContext.len = DTCP_AES_BLOCK_SIZE;
	cipherContext.bEncFlag = true;
	cipherContext.pOut = aCipherKey;
	cipherContext.keybits = 128;
	
	retValue = BCrypt_AESECBSw(ghBcrypt, &cipherContext);
	if(retValue == BCRYPT_STATUS_eOK)
	{
		for (i = 0; i < DTCP_AES_KEY_SIZE; ++i)
		{
			unsigned char *y0 = keyGenBuffer + DTCP_AES_BLOCK_SIZE;
			aCipherKey[i] ^= y0[i];
		}
    	/* Create the IV payload buffer */
    	{
        	ivConstant[0] = 0x95;
        	ivConstant[1] = 0xDC;
        	ivConstant[2] = 0x3A;
        	ivConstant[3] = 0x44;
        	ivConstant[4] = 0x90;
        	ivConstant[5] = 0x28;
        	ivConstant[6] = 0xEB;
        	ivConstant[7] = 0x3C;
        	BKNI_Memcpy(ivGenBuffer, ivConstant, DTCP_IV_CONSTANT_SIZE);
        	BKNI_Memcpy(ivGenBuffer + DTCP_IV_CONSTANT_SIZE, aNonce, DTCP_CONTENT_KEY_NONCE_SIZE);
    	}
		cipherContext.pIn = ivGenBuffer;
		cipherContext.pkey = aCipherKey;
		cipherContext.len = DTCP_AES_BLOCK_SIZE;
		cipherContext.bEncFlag = true;
		cipherContext.pOut = aCipherIv;
		cipherContext.keybits = 128;

		if((retValue = BCrypt_AESECBSw(ghBcrypt, &cipherContext)) != BCRYPT_STATUS_eOK)
			retValue = BERR_CRYPT_FAILED;
		
	}else {
		retValue = BERR_CRYPT_FAILED;
	}
#ifdef DTCP_DEMO_MODE
	BDBG_MSG(("Creating content Key: exchange key:\n"));
	BDBG_BUFF(aExchangeKey, DTCP_EXCHANGE_KEY_SIZE);
	BDBG_MSG(("\n"));
	BDBG_MSG(("aNonce\n"));
	BDBG_BUFF(aNonce, DTCP_CONTENT_KEY_NONCE_SIZE);
	BDBG_MSG(("\n"));
	BDBG_MSG(("cipherKey\n"));
	BDBG_BUFF(aCipherKey, DTCP_IP_CONTENT_KEY_SIZE);
#endif
	return retValue;
}
void B_DTCP_Content_ClosePacket(B_PacketHandle_T pPacketHandle)
{
    DtcpPacketData *data = (DtcpPacketData *)pPacketHandle;
	BDBG_ASSERT(data);

	if(data->cipherContext)
	{
		BKNI_Free(data->cipherContext->pkey);
		BKNI_Free(data->cipherContext);
	}

	BKNI_Free(data);
}

/*! \brief B_DTCP_Content_CreatePacketHeader create pcp header, called by source device only.
 *  \param[in] hAkehandle handle to AKE session.
 *  \param[in] Transport Stream transport type.
 *  \param[in] StreamData dtcp_ip stream data pointer
 *  \param[in] ContentSize total size of the content.
 *  \param[in,out] pPacketHeader Buffer to hold created packet header
 */
BERR_Code B_DTCP_Content_CreatePacketHeader( B_AkeHandle_T hAkeHandle, B_StreamTransport_T Transport,
		struct __b_dtcp_stream_data *StreamData,
		int ContentSize,
		unsigned char pPacketHeader[DTCP_CONTENT_PACKET_HEADER_SIZE])
{
	BERR_Code retValue = BERR_SUCCESS;
	B_DTCP_IP_PCPHeader_T * pcph = NULL;
    DtcpPacketData *data = NULL;
    unsigned char exchangeKeyLabel = 0;
	unsigned char cipherKey[DTCP_AES_KEY_SIZE] = {0};
	unsigned char exchangeKey[DTCP_EXCHANGE_KEY_SIZE] = {0};
	unsigned char nonce[DTCP_CONTENT_KEY_NONCE_SIZE] = {0};
	unsigned char cipherIv[DTCP_AES_IV_SIZE] = {0};
	B_AkeCoreSessionData_T * pSession = (B_AkeCoreSessionData_T *)hAkeHandle;

	BDBG_ASSERT(pSession);
	BDBG_ASSERT(pPacketHeader);
	BDBG_ENTER(B_DTCP_Content_CreatePacketHeader);

	/* 
	 * Caller must make sure they size is less then max dtcp packet size
	 * before calling this function.
	 */
	if(ContentSize > DTCP_MAXIMUM_PROTECTED_PACKET_SIZE)
		return BERR_CONTENT_SIZE_TOO_LARGE;
	
	if( (data = BKNI_Malloc(sizeof(DtcpPacketData)) ) == NULL)
	{
		return B_ERROR_OUT_OF_MEMORY;
	}

	/* 
	 * Create cipher context for content encryption. 
	 * How do we do HW encrytion here ?
	 */
    data->cipherContext = BKNI_Malloc(sizeof(BCRYPT_S_AESCBCSwCtrl_t));
	if(data->cipherContext == NULL)
	{
		BKNI_Free(data);
		return B_ERROR_OUT_OF_MEMORY;
	}
	data->BytesProcessed = 0;
	data->PayloadSize = ContentSize;
	data->ExtendedEmi = StreamData->Emi;

	/* Padding bytes ?*/
	if(data->PayloadSize % DTCP_AES_BLOCK_SIZE != 0)
	{
		data->PayloadSize += DTCP_AES_BLOCK_SIZE - (data->PayloadSize % DTCP_AES_BLOCK_SIZE);
	}

	/* Get Exchange key, label and Nc */
	if((retValue = B_DTCP_GetExchKeyFromCore(pSession->pAkeCoreData, B_ExchKeyCipher_eAes,
					exchangeKey, &exchangeKeyLabel)) != BERR_SUCCESS)
	{
		retValue = BERR_UNKNOWN;
		goto ERR_FREE;
	}
#ifdef DTCP_DEMO_MODE 
	BDBG_MSG(("Source Exchange Key Label: %d\n", exchangeKeyLabel));
	BDBG_MSG(("Source Exchange Key:\n"));
	BDBG_BUFF(exchangeKey, DTCP_EXCHANGE_KEY_SIZE); 
	BDBG_MSG(("\n"));
#endif
	if (pSession->pcp_ur_cap == 1) {
		B_DTCP_IP_PCP_UR_T *pcp_ur = (B_DTCP_IP_PCP_UR_T *)nonce;
		pcp_ur->UR_Mode = StreamData->ur_mode;
		pcp_ur->Content_type = StreamData->content_type;
		pcp_ur->APS = StreamData->aps;
		pcp_ur->ICT = StreamData->ict;
		pcp_ur->Source = 1; 
		pcp_ur->AST = StreamData->ast;
		pcp_ur->Reserved = 0;
		BDBG_MSG(("stream attribute: pcp_ur: \nur_mode=%d\nContent_type=%d\nAPS=%d\nICT=%d\nSource=%d\nAST=%d\n",
				pcp_ur->UR_Mode, pcp_ur->Content_type, pcp_ur->APS, pcp_ur->ICT, pcp_ur->Source, pcp_ur->AST));
		BKNI_Memcpy(&nonce[2], &StreamData->HttpNonce[2], DTCP_CONTENT_KEY_NONCE_SIZE - 2);
	} else {
		BKNI_Memcpy(nonce, StreamData->HttpNonce, DTCP_CONTENT_KEY_NONCE_SIZE);
	}
	if(Transport == B_StreamTransport_eRtp ||
			Transport == B_StreamTransport_eUdp)
	{
		/* TODO: For RTP, Nc is updating periodically in a seperated thread.*/
		retValue = BERR_NOT_SUPPORTED;
		goto ERR_FREE;
	}
    if((retValue = CreateContentKey(pSession->pAkeCoreData->hBcrypt, StreamData->Emi, 
					exchangeKey, nonce, cipherKey, cipherIv))
			!= BERR_SUCCESS)
	{
		BDBG_ERR(("Failed to create content key\n"));
		retValue = BERR_UNKNOWN;
		goto ERR_FREE;
	}
	data->cipherContext->pkey = BKNI_Malloc(DTCP_AES_KEY_SIZE);
	BKNI_Memcpy(data->cipherContext->pkey, cipherKey, DTCP_IP_CONTENT_KEY_SIZE);
	BKNI_Memcpy(data->NextIv, cipherIv, DTCP_AES_IV_SIZE);
	data->cipherContext->piv = data->NextIv;
	data->cipherContext->len = DTCP_AES_BLOCK_SIZE;
	data->cipherContext->bEncFlag = true;
	data->cipherContext->keybits = 128;

	/* Popuplate packet header */
	pcph = (B_DTCP_IP_PCPHeader_T *)pPacketHeader;
	BKNI_Memset(pcph, 0, sizeof(B_DTCP_IP_PCPHeader_T));
	pcph->C_A = 0;	/* Cipher Algorithm, 0 AES128 */
	pcph->Emi = data->ExtendedEmi & 0x0f;
	pcph->ExchKeyLabel = exchangeKeyLabel;
	pcph->CL = htonl(ContentSize);
	BKNI_Memcpy(pcph->Nc, nonce, DTCP_CONTENT_KEY_NONCE_SIZE);

#ifdef DTCP_DEMO_MODE
	/* Dump packet raw header */
	BDBG_MSG(("pcp header:\n"));
	BDBG_BUFF(pPacketHeader, DTCP_CONTENT_PACKET_HEADER_SIZE);
#endif
	StreamData->hPacketHandle = data;

	BDBG_LEAVE(B_DTCP_Content_CreatePacketHeader);
	return retValue;
ERR_FREE:
	BKNI_Free(data->cipherContext);
	BKNI_Free(data);
    return retValue;
} 


/*! \brief B_DTCP_Content_ConsumePacketHeader consume pcp header, called by sink device only.
 *  \param[in] hakeHandle handle to active AKE session.
 *  \param[in] pPacketHeader Buffer to hold packet header
 *  \param[in] StreamData stream data pointer.
 */
BERR_Code B_DTCP_Content_ConsumePacketHeader(B_AkeHandle_T hAkeHandle,
		unsigned char * pPacketHeader,
		struct __b_dtcp_stream_data *StreamData)
{
	B_DTCP_IP_PCPHeader_T * pcph = NULL;
	B_DTCP_IP_PCPURHeader_T *pcpurh = NULL;
    BERR_Code retValue = BERR_SUCCESS;
    DtcpPacketData *data = NULL;
    unsigned char exchKeyLabel = 0;
    unsigned char packetExchKeyLabel = 0;
	unsigned char cipherKey[DTCP_AES_KEY_SIZE] = {0};
	unsigned char exchangeKey[DTCP_EXCHANGE_KEY_SIZE] = {0};
	unsigned char nonce[DTCP_CONTENT_KEY_NONCE_SIZE] = {0};
	unsigned char cipherIv[DTCP_AES_IV_SIZE]= {0};
	B_AkeCoreSessionData_T * pSession = (B_AkeCoreSessionData_T *)hAkeHandle;

	BDBG_ASSERT(pSession);
	BDBG_ASSERT(hAkeHandle);
	BDBG_ASSERT(pPacketHeader);
	BDBG_ENTER(B_DTCP_Content_ConsumePacketHeader);

	if(B_DTCP_IP_IsContKeyConfirmed(pSession) == false)
	{
		BDBG_ERR(("Content Key not confirmed! stopping decryption\n"));
		return BERR_CONT_KEY_CONF_FAILED;
	}

#ifdef DTCP_DEMO_MODE
	/* Dump packet raw header */
	BDBG_MSG(("pcp header:\n"));
	BDBG_BUFF(pPacketHeader, DTCP_CONTENT_PACKET_HEADER_SIZE);
#endif
	/* Allocate packet handle data. */
	if((data = (DtcpPacketData *)BKNI_Malloc(sizeof(DtcpPacketData))) == NULL)
	{
		return B_ERROR_OUT_OF_MEMORY;
	}
	if (pSession->pcp_ur_cap == 1) {
		pcpurh = (B_DTCP_IP_PCPURHeader_T *)pPacketHeader;
		StreamData->Emi = pcpurh->Emi & 0xF;
		StreamData->packet_content_length = ntohl(pcpurh->CL);
		packetExchKeyLabel = pcpurh->ExchKeyLabel;
		/* Extract PCP_UR from pcp header */
		StreamData->ur_mode = pcpurh->pcp_ur.UR_Mode;
		StreamData->content_type = pcpurh->pcp_ur.Content_type;
		StreamData->aps = pcpurh->pcp_ur.APS;
		StreamData->ict = pcpurh->pcp_ur.ICT;
		StreamData->ast = pcpurh->pcp_ur.AST;
		/* NC consit of PC_UR and 48 bits of SNC */
		BKNI_Memcpy(nonce, (void *)&pcpurh->pcp_ur, 2);
		BKNI_Memcpy(&nonce[2], pcpurh->SNc, DTCP_CONTENT_KEY_NONCE_SIZE - 2);
		BDBG_MSG(("stream attribute: pcp_ur:\nur_mode=%d\nContent_type=%d\nAPS=%d\nICT=%d\nAST=%d\n",
				StreamData->ur_mode, StreamData->content_type, StreamData->aps, StreamData->ict, StreamData->ast));

	}else {
		pcph = (B_DTCP_IP_PCPHeader_T *)pPacketHeader;
		StreamData->Emi = pcph->Emi & 0x0F;
		StreamData->packet_content_length = ntohl(pcph->CL);
		packetExchKeyLabel = pcph->ExchKeyLabel;
		BKNI_Memcpy(nonce, pcph->Nc, DTCP_CONTENT_KEY_NONCE_SIZE);
	}
	
	/* What about the Nc ? */
	/* Initialize packet handle data */

	data->BytesProcessed = 0;
	data->ExtendedEmi = StreamData->Emi;
	data->PayloadSize = StreamData->packet_content_length;
	BDBG_MSG(("Payloadsize=%d emi=%d\n", data->PayloadSize, data->ExtendedEmi));
	
#if 0
	/* Padding */
	if(data->PayloadSize % DTCP_AES_BLOCK_SIZE != 0)
	{
		data->PayloadSize += DTCP_AES_BLOCK_SIZE - (data->PayloadSize % DTCP_AES_BLOCK_SIZE);
	}
#endif
	if(data->PayloadSize > DTCP_MAXIMUM_PROTECTED_PACKET_SIZE)
	{
		retValue= BERR_INVALID_PACKET_HEADER;
		BDBG_ERR(("Payloadsize too large\n"));
		goto ERR_FREE;
	}
	if((retValue = B_DTCP_GetExchKeyFromSession(pSession, B_ExchKeyCipher_eAes,
					exchangeKey, &exchKeyLabel)) != BERR_SUCCESS)
	{
		BDBG_ERR(("Failed to get exch key from session\n"));
		retValue = BERR_UNKNOWN;
		goto ERR_FREE;
	}
	BDBG_MSG(("Sink Exchange Key Label: %d\n", exchKeyLabel));
	BDBG_MSG(("Sink Exchange Key:\n"));
#ifdef DTCP_DEMO_MODE
	BDBG_BUFF(exchangeKey, DTCP_EXCHANGE_KEY_SIZE);
#endif
	BDBG_MSG(("\n"));

	if(exchKeyLabel != packetExchKeyLabel)
	{
		BDBG_ERR(("Exchange Key label mis-match! Packet Label: %d / Session Label: %d\n",
				packetExchKeyLabel, exchKeyLabel));
		retValue = BERR_INVALID_EXCHANGE_KEY_LABEL;
		goto ERR_FREE;
	}
	
    data->cipherContext = BKNI_Malloc(sizeof(BCRYPT_S_AESCBCSwCtrl_t));
	if(data->cipherContext == NULL)
	{
		retValue =  B_ERROR_OUT_OF_MEMORY;
		goto ERR_FREE;
	}
    
    /* Initialze IV */
	/* Store nonce value contained in pcph into AKE Session . */
	B_DTCP_GetSetRealTimeNonce(pSession->hMutex, nonce, pSession->RealTimeNonce);

    /* Create cipher key */
	if( (retValue = CreateContentKey(pSession->pAkeCoreData->hBcrypt, data->ExtendedEmi, exchangeKey, nonce, cipherKey, cipherIv))
			!= BERR_SUCCESS)
	{
		BDBG_ERR(("Failed to create content cipher key and iv\n"));
		goto ERR_FREE;
	}
	BKNI_Memcpy(data->NextIv, cipherIv, DTCP_AES_IV_SIZE);

	data->cipherContext->pkey = BKNI_Malloc(DTCP_AES_KEY_SIZE);
	BKNI_Memcpy(data->cipherContext->pkey, cipherKey, DTCP_AES_KEY_SIZE);
#if !defined(B_DTCP_IP_HW_DECRYPTION)
	data->cipherContext->keybits = 128;
	data->cipherContext->bEncFlag = false;
	data->cipherContext->piv = data->NextIv;
#endif
	StreamData->hPacketHandle = data;
#ifdef DTCP_DEMO_MODE
	BDBG_MSG(("Exchange Key: \n"));
	BDBG_BUFF(exchangeKey, DTCP_EXCHANGE_KEY_SIZE);
	BDBG_MSG(("nonce:\n"));
	BDBG_BUFF(nonce, DTCP_CONTENT_KEY_NONCE_SIZE);
	BDBG_MSG(("Content Key:\n"));
	BDBG_BUFF(cipherKey, DTCP_IP_CONTENT_KEY_SIZE);
	BDBG_MSG(("IV:\n"));
	BDBG_BUFF(data->NextIv, DTCP_AES_IV_SIZE);
#endif
	BDBG_LEAVE(B_DTCP_Content_ConsumePacketHeader);
	return retValue;
ERR_FREE:
	BKNI_Free(data);
    return retValue;
} 

/*! \brief B_DTCP_Content_EncryptData encrypt protected content, called by source only.
 *  \param[in] aAkeHandle AKE session handle.
 *  \param[in] aPacketHandle handle to packet data (obtained from create packet header function).
 *  \param[in] aInBuffer input buffer.
 *  \param[in,out] aOutBuffer output buffer.
 *  \param[in] aBufferLength length of input buffer.
 *
 *  When B_DTCP_IP_HW_ENCRYPTION is defined, this function requires the aInBuffer and aOutBuffer
 *  allocated from heap (via NEXUS_Memory_Allocate() ).
 */
BERR_Code B_DTCP_Content_EncryptData(B_AkeHandle_T aAkeHandle, B_PacketHandle_T aPacketData, 
                            unsigned char    *aInBuffer,
                            unsigned char    *aOutBuffer,
                            unsigned int      aBufferLength)
{
    BERR_Code retValue = BERR_SUCCESS;
    DtcpPacketData *data = NULL;
	BCRYPT_S_AESCBCSwCtrl_t * cipherContext = NULL;
	B_AkeCoreSessionData_T * pSession = (B_AkeCoreSessionData_T *)aAkeHandle;

#ifdef B_DTCP_IP_HW_ENCRYPTION
#ifndef B_DTCP_IP_COMMON_DRM_CONTENT_SUPPORT
	NEXUS_DmaJobStatus jobStatus;
#endif
#endif

	BDBG_ASSERT(aInBuffer);
	BDBG_ASSERT(aOutBuffer);
	BDBG_ASSERT(aPacketData);
	BDBG_ENTER(B_DTCP_Content_EncryptData);
	if(!pSession)
		return BERR_NOT_INITIALIZED;

	if( aBufferLength % DTCP_AES_BLOCK_SIZE != 0)
	{
		BDBG_ERR(("Buffer length invalid: %d\n", aBufferLength));
		return BERR_INVALID_PARAMETER;
	}
	
	data = (DtcpPacketData*)aPacketData;
	cipherContext = data->cipherContext;
    /* 
	 * Check that buffer does not exceed length of payload for this packet
	 */
	if(data->PayloadSize < data->BytesProcessed + aBufferLength)
	{
		BDBG_ERR(("Content exceeds packet size\n"));
		return BERR_INVALID_PARAMETER;
	}
	if(data->ExtendedEmi == B_Emi_eCopyFree)
	{
		BKNI_Memcpy(aOutBuffer, aInBuffer, aBufferLength);
		return BERR_SUCCESS;
	}
	if (B_ExtEmi_eCopyFree == data->ExtendedEmi)
	{
		/* No encryption, clear buffer */
		BKNI_Memcpy(aOutBuffer, aInBuffer, aBufferLength);
	}
	else
	{
#ifdef B_DTCP_IP_HW_ENCRYPTION
		/* TODO: Verify aInBuffer and aOutBuffer, assert if it's not in the heap memory */
		B_Mutex_Lock(hSecurityMutex);

		if( aInBuffer && aOutBuffer)
		{

            if(DRM_DtcpIp_Encrypt(aInBuffer,
								aBufferLength,
								aOutBuffer,
								cipherContext->pkey,
								cipherContext->piv) != Drm_Success)
			{
				BDBG_ERR(("%s - Error encrypting", __FUNCTION__));
				retValue = BERR_UNKNOWN;
				goto ErrorExit;
			}
			/* Save next IV */
			BKNI_Memcpy(cipherContext->piv, &aOutBuffer[aBufferLength - DTCP_AES_IV_SIZE], DTCP_AES_IV_SIZE);
			data->BytesProcessed += aBufferLength;
		}

		B_Mutex_Unlock(hSecurityMutex);

	}
#else
		cipherContext->pIn = aInBuffer;
		cipherContext->pOut = aOutBuffer;
		cipherContext->len = aBufferLength;
		cipherContext->bEncFlag = true;
#ifdef DTCP_DEMO_MODE
		BDBG_MSG(("pkey:\n"));
		BDBG_BUFF(cipherContext->pkey, DTCP_IP_CONTENT_KEY_SIZE);
		BDBG_MSG(("piv: len=%d\n", aBufferLength));
		BDBG_BUFF(cipherContext->piv, DTCP_AES_IV_SIZE);  
#endif
		retValue = BCrypt_AESCBCSw(pSession->pAkeCoreData->hBcrypt, cipherContext);
		BKNI_Memcpy(cipherContext->piv, &aOutBuffer[aBufferLength - DTCP_AES_IV_SIZE], DTCP_AES_IV_SIZE);
		data->BytesProcessed += aBufferLength;
	}
#endif

ErrorExit:
	BDBG_LEAVE(B_DTCP_Content_EncryptData);
	return retValue;
}

/*! \brief B_DTCP_Content_DecryptData decrypt protected content, called by source only.
 *  \param[in] aAkeHandle AKE session handle.
 *  \param[in] aPacketHandle handle to packet data (obtained from create packet header function).
 *  \param[in] aInBuffer input buffer.
 *  \param[in,out] aOutBuffer output buffer.
 *  \param[in] aBufferLength length of input buffer.
 */
BERR_Code B_DTCP_Content_DecryptData(B_AkeHandle_T aAkeHandle, B_PacketHandle_T  aPacketHandle, 
                            unsigned char    *aInBuffer,
                            unsigned char    *aOutBuffer,
                            unsigned int      aBufferLength)
{
    BERR_Code retValue = BERR_SUCCESS;
    DtcpPacketData *data = (DtcpPacketData *)aPacketHandle;
	unsigned char tempIv[DTCP_AES_IV_SIZE] = {0};
	B_AkeCoreSessionData_T * pSession = (B_AkeCoreSessionData_T *)aAkeHandle;

#ifndef B_DTCP_IP_HW_DECRYPTION
	BCRYPT_S_AESCBCSwCtrl_t * cipherContext;
	cipherContext = data->cipherContext;
#endif

	BDBG_ASSERT(pSession);
	BDBG_ASSERT(aInBuffer);
	BDBG_ASSERT(aOutBuffer);
	BDBG_ASSERT(aBufferLength);
	BDBG_ASSERT(aPacketHandle);

	BDBG_ENTER(B_DTCP_Content_DecryptData);
    if (0 != aBufferLength % DTCP_AES_BLOCK_SIZE)
    {
        retValue = BERR_INVALID_PARAMETER;
    }

    if (BERR_SUCCESS == retValue)
    {
		if (B_ExtEmi_eCopyFree == data->ExtendedEmi)
		{
			BKNI_Memcpy(aOutBuffer, aInBuffer, aBufferLength);
		}
		else
		{
#ifdef  B_DTCP_IP_HW_DECRYPTION

			B_Mutex_Lock(hSecurityMutex);

			(void)pSession;
			BKNI_Memcpy(tempIv, &aInBuffer[aBufferLength - DTCP_AES_IV_SIZE], DTCP_AES_IV_SIZE);
			/* re-allocate DMA buffer if the buffer length changed.*/

			if( aInBuffer && aOutBuffer)
			{
                if(aBufferLength > DtcpDma.LastBufferLength)
                {
                    if(DtcpDma.dma_in != NULL)
                        NEXUS_Memory_Free(DtcpDma.dma_in);
                    if(DtcpDma.dma_out != NULL)
                        NEXUS_Memory_Free(DtcpDma.dma_out);

                    if(NEXUS_Memory_Allocate(aBufferLength, NULL, &DtcpDma.dma_in))
                    {
                        BDBG_ERR(("Failed to allocate DMA memory, len=%d\n", aBufferLength));
                        retValue = BERR_UNKNOWN;
                    }
                    if(NEXUS_Memory_Allocate(aBufferLength, NULL, &DtcpDma.dma_out))
                    {
                        BDBG_ERR(("Failed to allocate DMA memory, len=%d\n", aBufferLength));
                        retValue = BERR_UNKNOWN;
                    }

                    DtcpDma.LastBufferLength = aBufferLength;
                }

                BKNI_Memcpy(DtcpDma.dma_in, aInBuffer, aBufferLength);
                if(DRM_DtcpIp_Decrypt(DtcpDma.dma_in,
                            aBufferLength,
                            DtcpDma.dma_out,
                            data->cipherContext->pkey,
                            data->NextIv) != Drm_Success)
                {
                    BDBG_ERR(("%s - Error decrypting", __FUNCTION__));
                    retValue = BERR_UNKNOWN;
                    goto ErrorExit;
				}
				BKNI_Memcpy(aOutBuffer, DtcpDma.dma_out, aBufferLength);

				/* Save next IV */
				BKNI_Memcpy(data->NextIv, tempIv, DTCP_AES_IV_SIZE);
				data->BytesProcessed += aBufferLength;

				B_Mutex_Unlock(hSecurityMutex);
#endif
			}



#ifndef  B_DTCP_IP_HW_DECRYPTION /* !B_DTCP_IP_HW_DECRYPTION */
			BKNI_Memcpy(tempIv, &aInBuffer[aBufferLength - DTCP_AES_IV_SIZE], DTCP_AES_IV_SIZE);
			cipherContext->pIn = aInBuffer;
			cipherContext->pOut = aOutBuffer;
			cipherContext->bEncFlag = false;
			cipherContext->len = aBufferLength;
			cipherContext->piv = data->NextIv;
			retValue = BCrypt_AESCBCSw(pSession->pAkeCoreData->hBcrypt, cipherContext);

			if(retValue == BERR_SUCCESS)
			{
				data->BytesProcessed += aBufferLength;
				/* Save Next IV */
				BKNI_Memcpy(data->NextIv, tempIv, DTCP_AES_IV_SIZE);
			}
#ifdef DTCP_DEMO_MODE
			BDBG_MSG(("Sw Enc Buffer:\n"));
			BDBG_BUFF(aInBuffer, 16);
			BDBG_MSG(("Sw Dec Buffer:\n"));
			BDBG_BUFF(aOutBuffer, 16);
#endif
				
#endif
		}
    }

ErrorExit:
	BDBG_LEAVE(B_DTCP_Content_DecryptData);
    return retValue;
}
