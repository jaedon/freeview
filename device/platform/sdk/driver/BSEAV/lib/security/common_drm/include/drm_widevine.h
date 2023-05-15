/******************************************************************************
 *    (c)2010-2012 Broadcom Corporation
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
 * $brcm_Workfile: drm_widevine.h $
 * $brcm_Revision: 6 $
 * $brcm_Date: 11/21/12 3:42p $
 * 
 * Module Description:
 * 
 * Revision History:
 * 
 *****************************************************************************/
#ifndef DRM_WIDEVINE_H_
#define DRM_WIDEVINE_H_

#include "drm_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum DRM_WidevineDecryptMode_e
{
	DRM_WidevineDecryptMode_eVod = 0,
	DRM_WidevineDecryptMode_eTs_Iptv = 1,
	DRM_WidevineDecryptMode_eMax
}DRM_WidevineDecryptMode_e;

typedef struct DRM_WidevineInit_t
{
	char * key_file;
	DRM_WidevineDecryptMode_e mode;
	DrmCommonInit_t drmCommonInit;
}DRM_Widevine_Init_t;


/******************************************************************************
** FUNCTION:
**  DRM_Widevine_GetDefaultParamSettings
**
** DESCRIPTION:
**   Retrieve the default settings
**
** PARAMETERS:
** pWidevineParamSettings - pointer to settings structure
**
** RETURNS:
**   void.
**
******************************************************************************/
void DRM_Widevine_GetDefaultParamSettings(
		DRM_Widevine_Init_t *pWidevineParamSettings);

/******************************************************************************
** FUNCTION:
**  DRM_Widevine_SetParamSettings
**
** DESCRIPTION:
**   Set param settings
**
** PARAMETERS:
** pWidevineParamSettings - settings structure
**
** RETURNS:
**   void.
**
******************************************************************************/
void DRM_Widevine_SetParamSettings(
		DRM_Widevine_Init_t widevineParamSettings);

/**********************************************************
 * FUNCTION:
 *  DRM_Widevine_Initialize
 *
 * DESCRIPTION:
 *   Fetches the encrypted keybox information and sets the
 *   decryption mode in either TS mode or regular mode
 *
 * RETURN:
 * 	Drm_Success or other
***********************************************************/
DrmRC DRM_Widevine_Initialize(
		DRM_Widevine_Init_t inputWvInitStruct);

/*
// Frees the key slots for widevine decryption
*/
void DRM_Widevine_Finalize(void);

/*
/// Retrieves the Device ID.
/// @param[in/out]  pDevID  Pointer to the buffer where the DevID Data will be copied into.
/// @return drm_wv_ok if the operation is successful or an error.
*/
DrmRC DRM_Widevine_GetDeviceId(uint8_t* pDevID);

/*
/// Retrieves the Root Key Data.
/// @param[in/out]  pKeyID      Pointer to the buffer where the Root Key Data will be copied into.
/// @return drm_wv_ok if the operation is successful or an error.
*/
DrmRC DRM_Widevine_GetKeyId(uint8_t* pKeyID);

/*
/// Decrypts the stream received in parameter.
/// @param[in]      pEnc        Pointer to the buffer of encrypted data
/// @param[in]          uiSize      Size of the data to decrypt.
/// @param[in/out]  pDec        Pointer to the buffer where the decrypted data will be copied into.
/// @param[in]          pIv         IV for CBC decryption
/// @param[in]      dest_type            Decryption type: in place or to destination. 
/// @param[in]      keyslot         Use Odd/Even Key
/// @return drm_wv_ok if the operation is successful or an error.
*/
DrmRC DRM_Widevine_Decrypt(uint8_t* pEnc,
                    uint32_t uiSize, 
                    uint8_t* pDec, 
                    uint8_t* pIv, 
                    DrmDestinationType dest_type, 
                    DrmSecurityKeyType keyslot_type);


/*
/// Encrypts the stream received in parameter.
/// @param[in]      pBuf        Pointer to the buffer to be encrypted data
/// @param[in]          uiSize      Size of the data to decrypt.
/// @param[in/out]  pEnc        Pointer to the buffer where the encrypted data will be copied into.
/// @param[in]          pIv         IV for CBC encryption
/// @param[in]      dest_type            Encyption typee: in place or to destination. 
/// @param[in]      keyslot         Use Odd/Even Key
/// @return drm_wv_ok if the operation is successful or an error.
*/
DrmRC DRM_Widevine_Encrypt(uint8_t* pBuf,
                    uint32_t uiSize, 
                    uint8_t* pEnc, 
                    uint8_t* pIv, 
                    DrmDestinationType dest_type, 
                    DrmSecurityKeyType keyslot_type);

/*
/// Passes through  the stream received in parameter thru M2M.
/// @param[in]      pSrcBuf        Pointer to the buffer to be passed thru
/// @param[in]          uiSize      Size of the data to decrypt.
/// @param[in/out]  pDstBuf        Pointer to the destination buffer where the passed thru data will be copied into.
/// @param[in]          pIv         IV for CBC encryption , not used
/// @param[in]      dest_type            Encyption typee: in place or to destination. 
/// @param[in]      keyslot         Use Odd/Even Key
/// @return drm_wv_ok if the operation is successful or an error.
*/

DrmRC DRM_Widevine_Passthru(uint8_t* pSrcBuf,
                    uint32_t uiSize, 
                    uint8_t* pDstBuf, 
                    uint8_t* pIv, 
                    DrmDestinationType dest_type, 
                    DrmSecurityKeyType keyslot_type);

/*//////////////////////////////////////////////////////////////////////////////
/// Loads the CW using dynamic keys located in the DRM binary file.
/// @param[in]      pCommonKey  Pointer to the buffer of encrypted Common Key.
/// @param[in]      pEcm        Pointer to the buffer of encrypted ECM.
/// @param[in/out]  pId         Pointer to the buffer where the 4-byte ID of the CW is copied into.
/// @param[in]      keyslot     Keyslot Type(odd or even key slot)
/// @return drm_wv_ok if the operation is successful or an error.
//////////////////////////////////////////////////////////////////////////////*/
DrmRC DRM_Widevine_LoadCw(
        uint8_t* pCommonKey, 
        uint8_t* pEcm, 
        uint8_t* pId, 
        DrmSecurityKeyType keyslot_type
);


/*//////////////////////////////////////////////////////////////////////////////
/// Loads the CW using fixed test keys.
/// @param[in]      pFixedKey   Pointer to the buffer of the fixed value of CW.
/// @param[in]      pEcm        Pointer to the buffer of encrypted ECM.
/// @param[in/out]  pId         Pointer to the buffer where the 4-byte ID of the CW is copied into.
/// @param[in]      keyslot     Keyslot Type (odd or even key slot)
/// @return drm_wv_ok if the operation is successful or an error.
//////////////////////////////////////////////////////////////////////////////*/
DrmRC DRM_Widevine_LoadCwFixed(
        uint8_t* pFixedKey,
        uint8_t* pEcm,
        uint8_t* pId, 
        DrmSecurityKeyType keyslot_type
);


/**********************************************************
** FUNCTION:
**  DRM_Widevine_AttachPidChannel
**
** DESCRIPTION:
**   Attach a pid channel to the content key
**
** RETURN:
** 	Drm_Success or other
***********************************************************/
DrmRC DRM_Widevine_AttachPidChannel(unsigned int pidChannel);


/******************************************************************************
** FUNCTION:
**  DRM_Widevine_DetachPidChannel
**
** DESCRIPTION:
**  Detach the pid channel from the content key keyslot
******************************************************************************/
DrmRC DRM_Widevine_DetachPidChannel(unsigned int pidChannel);

/******************************************************************************
** FUNCTION:
**  DRM_Widevine_SetDrmOp
**
** DESCRIPTION:
**  Overrides the default operation(which is decrypt). 
******************************************************************************/
DrmRC DRM_Widevine_SetDrmOp(DrmCryptoOperation Op);

#include "nexus_security_datatypes.h"


/******************************************************************************
** FUNCTION:
**  DRM_Widevine_GetKeySlot
**
** DESCRIPTION:
**  Returns the keyslot handle that is configured
******************************************************************************/
DrmRC DRM_Widevine_GetKeySlot(NEXUS_SecurityKeyType keyslot_type, NEXUS_KeySlotHandle*pKeySlot);


#ifdef __cplusplus
}
#endif

#endif /*DRM_WIDEVINE_H_*/
