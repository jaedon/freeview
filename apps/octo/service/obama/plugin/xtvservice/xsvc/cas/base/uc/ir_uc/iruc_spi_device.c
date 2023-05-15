/*
 * (c) 2011-2012 Humax Co., Ltd.
 * This program is produced by Humax Co., Ltd. ("Humax") and
 * the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
 * non-assignable, non-transferable and non-exclusive license to use this Software.
 * You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
 * you agree to the responsibility to take all reasonable efforts to protect the any information
 * you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
 * reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
 * If you have no authorized license, discontinue using this Software immediately.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
 * IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
 * IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies
 * of the Software including all documentation. This License will terminate immediately without notice from Humax
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
 * of the Software and all documentation.

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
 * in the Republic of Korea.
*/

#include <octo_common.h>
#include <_svc_cas_mgr_common.h>
#include <ir_api.h>

#include "UniversalClient_Common_API.h"
#include "UniversalClient_Common_SPI.h"

#include "iruc_drv.h"
#include "iruc_spi.h"

// hjlee3, test
//#define TEMP_SW_SECURED // only for debugging.
#ifdef TEMP_SW_SECURED
static HUINT8 temp_secure_id[IRUC_FLASH_SECURE_ID_SIZE]={0x00,0x00,0x30,0x04};
static HUINT8 temp_private_data[IRUC_FLASH_PRIVATEDATA_SIZE]={0};
#endif

#if defined(CONFIG_MW_CAS_IRDETO_SECURE)
static HBOOL	s_bEnabledCWMode = FALSE;
static HUINT8	s_aucCSSK[IRUC_SECURE_CSSK_NUM];
#else
static HUINT8	s_aucSecureId[IRUC_FLASH_SECURE_ID_SIZE];
#endif

static HUINT8	s_aucPrivateData[IRUC_FLASH_PRIVATEDATA_SIZE];

#if defined(CONFIG_MW_CAS_IRDETO_SECURE)
extern HIR_ERR CAS_IR_SECURECHIP_UpdateFusingInfo(void);
extern HIR_ERR CAS_IR_SECURECHIP_GetRsaBoot(HUINT32 *pulRsaBoot);
extern HIR_ERR CAS_IR_SECURECHIP_GetJtag(HUINT32 *pulJtag);
extern HIR_ERR CAS_IR_SECURECHIP_GetCwMode(HUINT32 *pulCwMode);
extern HIR_ERR CAS_IR_SECURECHIP_GetCrypto(HUINT8 *pucCrypto);
#endif
/** @defgroup devicespi Universal Client Device SPIs
 *  Universal Client Device SPIs
 *
 *  These SPIs are responsible for retrieving values that are provisioned onto a device at manufacture time,
 *  and for setting any hardware specific parameters in the device.
 *
 * \note It is the integrator's responsibility to implement these methods for a particular platform.
 *  @{
 */

/**
 * Gets the Secure Chipset Serial Number
 *
 * This function is called to retrieve the secure chipset serial number associated with the device.
 * This serial number is used to deliver EMMs addressed to this particular instance of a client.
 *
 * @param[in] pData Buffer to read serial number data into.
 *     On input, the uc_bytes::bytes member points to a buffer in memory, and the uc_bytes::length member
 *     to the maximum length of the buffer, in bytes.  The function will modify the contents of the buffer to include
 *     the secure chipset serial number, and set the uc_bytes::length member
 *     to the actual length of data returned.
 *     If the uc_bytes::length member is not large enough to hold the entire data, the uc_bytes::length member
 *     should be set to the minimum required buffer size, and the function should return ::UC_ERROR_INSUFFICIENT_BUFFER.
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     ::UC_ERROR_INSUFFICIENT_BUFFER
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List".
 */
uc_result UniversalClientSPI_Device_GetCSSN(uc_buffer_st * pData)
{
#if defined(CONFIG_MW_CAS_IRDETO_SECURE)
	HUINT32	ulCSSN = 0, i;
#endif

	if(pData == NULL)
	{
		HxLOG_Error("pData NULL\n");
		return UC_ERROR_NULL_PARAM;
	}

	HxLOG_Info("allocated cssn buffer size is (%d).\n", 	pData->length );

	if( IRUC_FLASH_SECURE_ID_SIZE > pData->length)
	{
		HxLOG_Print("insufficient buffer length\n");
		return UC_ERROR_INSUFFICIENT_BUFFER;
	}

#if defined(CONFIG_MW_CAS_IRDETO_SECURE)
	xsvc_cas_IrSecureChipGetCSSN(&ulCSSN);

	for(i = 0; i < IRUC_FLASH_SECURE_ID_SIZE ; i++)
	{
		pData->bytes[i] = (ulCSSN  >> (IRUC_FLASH_SECURE_ID_SIZE - (i+1))*8);
	}
#else
	HxSTD_memcpy(pData->bytes, s_aucSecureId, IRUC_FLASH_SECURE_ID_SIZE);
#endif

	HxLOG_Print("CSSN [%02X][%02X][%02X][%02X]\n", pData->bytes[0], pData->bytes[1], pData->bytes[2], pData->bytes[3]);

	return UC_ERROR_SUCCESS;
}

/**
 * Gets the device private data
 *
 * This returns the personalized device specific private data.  This is an optional SPI, the device may not implement it
 * if no such private data is available.
 *
 * @param[in] pData Buffer to read serial number data into.
 *     On input, the uc_bytes::bytes member points to a buffer in memory, and the uc_bytes::length member
 *     to the maximum length of the buffer, in bytes.  The function will modify the contents of the buffer to include
 *     the private data, and set the uc_bytes::length member to the actual length of data returned.
 *     If the uc_bytes::length member is not large enough to hold the entire data, the uc_bytes::length member
 *     should be set to the minimum required buffer size, and the function should return ::UC_ERROR_INSUFFICIENT_BUFFER.
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     ::UC_ERROR_INSUFFICIENT_BUFFER
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List".
 */
uc_result UniversalClientSPI_Device_GetPrivateData(uc_buffer_st * pData)
{
	if(pData == NULL)
	{
		return UC_ERROR_NULL_PARAM;
	}

	HxLOG_Info("allocated private data buffer size is (%d).\n",pData->length );

	if( IRUC_FLASH_PRIVATEDATA_SIZE > pData->length)
	{
		HxLOG_Print("insufficient buffer length\n");
		return UC_ERROR_INSUFFICIENT_BUFFER;
	}

	pData->length = IRUC_FLASH_PRIVATEDATA_SIZE;
	HxSTD_memcpy(pData->bytes, s_aucPrivateData, IRUC_FLASH_PRIVATEDATA_SIZE);

	HxLOG_Print("PrivateData [%02X][%02X][%02X][%02X][%02X][%02X][%02X][%02X][%02X][%02X][%02X][%02X][%02X][%02X][%02X][%02X]\n",
							s_aucPrivateData[0],s_aucPrivateData[1],s_aucPrivateData[2],s_aucPrivateData[3],
							s_aucPrivateData[4],s_aucPrivateData[5],s_aucPrivateData[6],s_aucPrivateData[7],
							s_aucPrivateData[8],s_aucPrivateData[9],s_aucPrivateData[10],s_aucPrivateData[11],
							s_aucPrivateData[12],s_aucPrivateData[13],s_aucPrivateData[14],s_aucPrivateData[15]);

	return UC_ERROR_SUCCESS;
}

/**
 * Sets the Secure Chipset Session Key
 *
 * This function is called to provision a secure descrambler chipset with a secure session key.  This session key is
 * used to protect descrambler keys that are delivered to the system by further encrypting them with another
 * key that only this device knows about.
 *
 * See ::uc_dk_protection for list of the types of protection that can be provided by a platform.
 *
 * @param[in] pKeyMaterial CSSK key material to provision into the device. This is normally protected by a secure chipset unique key
 *     (CSUK) that resides within the secure chipset such that an attacker gains no advantage by intercepting this method.
 *     The uc_bytes::bytes member points to a buffer containing the raw key material, and the uc_bytes::length member contains
 *     the number of bytes in the buffer.
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List".
 */
uc_result UniversalClientSPI_Device_SetCSSK(const uc_buffer_st * pKeyMaterial)
{
	HUINT8 i = 0;

	if(pKeyMaterial == NULL)
	{
		HxLOG_Error("pKeyMaterial NULL\n");
		return UC_ERROR_NULL_PARAM;
	}

	HxLOG_Info(" length (%d) byte (%x) \n",pKeyMaterial->length, pKeyMaterial->bytes[0]);

#if defined(CONFIG_MW_CAS_IRDETO_SECURE)
	if(pKeyMaterial->length > IRUC_SECURE_CSSK_NUM)
	{
		HxLOG_Error("CSSK length error. (%d, %d)\n", pKeyMaterial->length, IRUC_SECURE_CSSK_NUM);
		return UC_ERROR_INSUFFICIENT_BUFFER;
	}

	HxSTD_memset(s_aucCSSK, 0, IRUC_SECURE_CSSK_NUM);
	HxSTD_memcpy(s_aucCSSK, pKeyMaterial->bytes, pKeyMaterial->length);

	s_bEnabledCWMode = TRUE;

	/* Driver 단에서 SWAP이 되어 있어서 SWAP 필요. */
	for(i=0; i<8; i++)
	{
		s_aucCSSK[i+8]=pKeyMaterial->bytes[i];
		s_aucCSSK[i]=pKeyMaterial->bytes[i+8];
	}
#endif

	return UC_ERROR_SUCCESS;
}

/**
 * Sets the Secure Chipset Session Key With key Infomation to support multi key ladder.
 *
 * This is the extension of ::UniversalClientSPI_Device_SetCSSK function with additional key protection infomation provided,
 * in order to support differents key ladder.This session key is used to protect descrambler keys that are delivered to the
 * system by further encrypting them with another key that only this device knows about.
 *
 * See ::uc_dk_protection for list of the types of protection that can be provided by a platform.
 *
 * \note Some secure chipsets require to swap CSSK or/and CW. SoftCell3 will perform the swap internally so that the output meets the secure chipset's requirement.
 *    Cloaked CA Agent solution doesn't support to perform the swap internally. In order to run Cloaked CA Agent successfully, the swap function MUST be disabled.
 *    The client device manufacturers shall contact chipset vendor to figure out how to disable swap function in such case.
 *    If the descramble status of Cloaked CA Agent is D029, but no audio/video, it is possible that the chipset needs to be configured to disable swap function.
 *    The client device manufacturers may consult with Irdeto if necessary.
 *
 *    \n
 *
 * \note Cloaked CA Agent ONLY supports 2-TDES key ladder currently, i.e. CSSK is encrypted by chipset unique key and control word is encrypted by CSSK.
 *    Encryption algorithm is TDES.
 *
 * @param[in] pCSSKInfo Information about the CSSK being used to protected the descrambling key. The CSSK is also protected by CSUK.
 *     This contain CSSK protection and CSSK material. See ::uc_cssk_info.
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List".
 */
uc_result UniversalClientSPI_Device_SetExtendedCSSK(const uc_cssk_info * pCSSKInfo)
{
	// TODO: Secure chipset SPI
	HxLOG_Print("\n" );
	HxLOG_Print("주의 !!!!!!!!!!!\n" );
	HxLOG_Print("주의 !!!!!!!!!!!\n" );

	return	UC_ERROR_NOT_IMPLEMENTED;
//	return UC_ERROR_SUCCESS;
}

/**
 * Gets device's security state
 *
 * This function is called to retrieve security state information from a device.
 * Universal Client will combine the information from this method to generate a \ref glossary_tsc "TSC" CHIP record.
 *
 * @param[in,out] pDeviceSecurityState Secure chipset working information to be filled in by the platform.
 *
 * See ::uc_device_security_state for the detail information needed.
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List".
 */
uc_result UniversalClientSPI_Device_GetSecurityState(uc_device_security_state * pDeviceSecurityState)
{
#if defined(CONFIG_MW_CAS_IRDETO_SECURE)
	HUINT8	ucCrypto;
	HUINT32	ulRsaBoot, ulJtag, ulCwMode;
#endif

	if(pDeviceSecurityState == NULL)
	{
		HxLOG_Error("pDeviceSecurityState NULL\n");
		return UC_ERROR_NULL_PARAM;
	}

#if defined(CONFIG_MW_CAS_IRDETO_SECURE)
	CAS_IR_SECURECHIP_UpdateFusingInfo();

	CAS_IR_SECURECHIP_GetRsaBoot(&ulRsaBoot);
	CAS_IR_SECURECHIP_GetJtag(&ulJtag);
	CAS_IR_SECURECHIP_GetCwMode(&ulCwMode);
	CAS_IR_SECURECHIP_GetCrypto(&ucCrypto);

	pDeviceSecurityState->rsaMode = (uc_uint32)ulRsaBoot;
	pDeviceSecurityState->jtag = (uc_uint32)ulJtag;
	pDeviceSecurityState->cwMode = (uc_uint32)ulCwMode;
	pDeviceSecurityState->crypto = (uc_byte)ucCrypto;

	HxLOG_Print("rsaMode= 0x%X\n", pDeviceSecurityState->rsaMode);
	HxLOG_Print("jtag= 0x%X\n", pDeviceSecurityState->jtag);
	HxLOG_Print("cwMode= 0x%X\n",  pDeviceSecurityState->cwMode);
	HxLOG_Print("crypto= 0x%X\n",    pDeviceSecurityState->crypto);
#else
	HxSTD_memset(pDeviceSecurityState, 0, sizeof(uc_device_security_state));
#endif

	return UC_ERROR_SUCCESS;
}


/**
 * Gets device's platform identifiers
 *
 * This function is called to retrieve relevant identifiers from a device.
 * Universal Client will combine the information from this method to generate a \ref glossary_tsc "TSC" CHIP record.
 *
 * @param[in,out] pDevicePlatformIdentifiers Device's identifiers to be filled in by the platform.
 *
 * See ::uc_device_platform_identifiers for the detail information needed.
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List".
 */
 uc_result UniversalClientSPI_Device_GetPlatformIdentifiers(uc_device_platform_identifiers * pDevicePlatformIdentifiers)
{
	HUINT8 								ucTemp1,ucTemp2 ;
	IHL_STATUS							IHL_status;

	HxLOG_Print(" ==>start \n" );

	if(ERR_OK != xsvc_cas_IrLoaderGetHWCode((HUINT16*)&pDevicePlatformIdentifiers->hardwareVersion) )
	{// default.
		pDevicePlatformIdentifiers->hardwareVersion = 0xFFFF;
		HxLOG_Print("Get loader hw code failed\n" );
	}

	if(ERR_OK !=	xsvc_cas_IrLoaderGetManuCode((HUINT16*)&pDevicePlatformIdentifiers->manufacturerId) )
	{
		pDevicePlatformIdentifiers->manufacturerId 	= 0xFFFF;
		HxLOG_Print("Get loader man code failed\n" );
	}

	if(ERR_OK == xsvc_cas_IrLoaderGetLoaderMajor(&ucTemp1) )
	{
		if(ERR_OK == xsvc_cas_IrLoaderGetLoaderMinor(&ucTemp2) )
		{
			pDevicePlatformIdentifiers->loaderVersion = (ucTemp1<<8) | ucTemp2;
		}
		else
		{//default.
			pDevicePlatformIdentifiers->loaderVersion = 0xFFFF;
			HxLOG_Print("Get loader minor num,failed\n" );
		}
	}
	else
	{//default.
		pDevicePlatformIdentifiers->loaderVersion = 0xFFFF;
		HxLOG_Print("Get loader major num,failed\n" );
	}

	if(ERR_OK != xsvc_cas_IrLoaderGetStatusInfo(&IHL_status) )
	{// default.
		pDevicePlatformIdentifiers->systemId 	= 0x43;
		pDevicePlatformIdentifiers->keyVersion = 0xFFFF;
		pDevicePlatformIdentifiers->variant		= 0x0001;
		pDevicePlatformIdentifiers->signatureVersion = 0xFFFF;
	}

		pDevicePlatformIdentifiers->systemId 	= IHL_status.u16SysID;
		pDevicePlatformIdentifiers->keyVersion = IHL_status.u16KeyVer;
		pDevicePlatformIdentifiers->variant		= IHL_status.u16Variant;
		pDevicePlatformIdentifiers->signatureVersion = IHL_status.u16SigVer;

	HxLOG_Print("manufacturerId= 0x%X\n", pDevicePlatformIdentifiers->manufacturerId);
	HxLOG_Print("hardwareVersion= 0x%X\n", pDevicePlatformIdentifiers->hardwareVersion);
	HxLOG_Print("systemId= 0x%X\n",  pDevicePlatformIdentifiers->systemId);
	HxLOG_Print("variant= 0x%X\n",    pDevicePlatformIdentifiers->variant);
	HxLOG_Print("keyVersion= 0x%X\n",  pDevicePlatformIdentifiers->keyVersion);
	HxLOG_Print("signatureVersion= 0x%X\n",  pDevicePlatformIdentifiers->signatureVersion);
	HxLOG_Print("loaderVersion= 0x%X\n", pDevicePlatformIdentifiers->loaderVersion);

	return UC_ERROR_SUCCESS;
}


/**
 * Gets device's personalized data
 *
 * This function is called to retrieve personalized data in a device.
 * The personalized data was written to device on the product line.
 * Now this SPI is only used in Pre-Enablement, which is not a generally available feature. It can be ignored if
 * Pre-Enablement is not integrated and no personalized data is used.
 *
 * @param[in,out] pData The device's personalized data.
 *     On input, the uc_buffer_st::bytes member points to a buffer in memory, and the uc_buffer_st::length member
 *     to the maximum length of the buffer, in bytes.  The function will modify the contents of the buffer to include
 *     the personalized data, and set the uc_buffer_st::length member to the actual length of data returned.
 *     If the uc_buffer_st::length member is not large enough to hold the entire data, the uc_buffer_st::length member
 *     should be set to the minimum required buffer size, and the function should return ::UC_ERROR_INSUFFICIENT_BUFFER.
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     ::UC_ERROR_INSUFFICIENT_BUFFER
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List".
 */
uc_result UniversalClientSPI_Device_GetPersonalizedData(uc_buffer_st* pData )
{
	HxLOG_Info("allocated data buffer size is (%d)\n", pData->length );

	/* This SPI is only used in Pre-Enablement. */

	return UC_ERROR_NOT_IMPLEMENTED;
}


#define ______DEVICE_API____________

HERROR	CAS_IRUC_SPI_DEVICE_LoadAllUniqueIdentifers(void)
{
    HERROR	hErr;

	HxLOG_Print("\n");

#if !defined(CONFIG_MW_CAS_IRDETO_SECURE)
	hErr = svc_cas_DevSysGetField(eCAS_DEV_FLASH_FIELD_IRDETO_UC_SECURE_ID, 0, s_aucSecureId, IRUC_FLASH_SECURE_ID_SIZE);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("svc_cas_DevSysGetField, IRUC_FLASH_SECURE_ID_OFFSET, ret=0x%X\n", hErr);
		return	ERR_FAIL;
	}
#endif

#if 0// defined(CONFIG_MW_SYS_NAND_FLASH_ONLY)
// hjlee3 API 연결되어 잇나 확인 필요 하다.
//	di_ret  = PAL_SYS_GetUCPrivateData(s_aucPrivateData);
#else
	hErr = svc_cas_DevSysGetField(eCAS_DEV_FLASH_FIELD_IRDETO_UC_PRIVATE_DATA, 0, s_aucPrivateData, IRUC_FLASH_PRIVATEDATA_SIZE);
#endif

	if (ERR_OK != hErr )
	{
		HxLOG_Error("eCAS_DEV_FLASH_FIELD_IRDETO_UC_PRIVATE_DATA, ret=0x%X\n", hErr);
		return	ERR_FAIL;
	}

#if defined(TEMP_SW_SECURED)
	HxSTD_memcpy(s_aucSecureId, temp_secure_id, sizeof(s_aucSecureId) );
	HxSTD_memcpy(s_aucPrivateData, temp_private_data, sizeof(s_aucPrivateData) );
#endif

	HxLOG_Print("All identiferes are loaded. ok \n");

	return ERR_OK;
}

#if defined(CONFIG_MW_CAS_IRDETO_SECURE)
// SECURE CHIP
HUINT8 *CAS_IRUC_SPI_GetCSSK(void)
{
	return s_aucCSSK;
}

HBOOL CAS_IRUC_SPI_GetEnabledCSSK(void)
{
	return s_bEnabledCWMode;
}

void CAS_IRUC_SPI_ClearCSSK(void)
{
	s_bEnabledCWMode = FALSE;
	HxSTD_memset(s_aucCSSK, 0, IRUC_SECURE_CSSK_NUM);

	return;
}

#else
// SECURE ID
HINT32 CAS_IRUC_API_GetSecureID(HUINT8 *pSecureID)
{
	if(pSecureID == NULL)
	{
		return ERR_FAIL;
	}

	HxSTD_memcpy(pSecureID, s_aucSecureId, IRUC_FLASH_SECURE_ID_SIZE);

	return ERR_OK;
}
#endif

/** @} */ /* End of devicespi */
