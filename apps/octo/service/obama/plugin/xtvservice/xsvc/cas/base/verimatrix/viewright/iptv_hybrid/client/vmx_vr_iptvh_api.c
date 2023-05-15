/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  vmx_vr_iptvh_api.c
	@brief

	Description:  									\n
	Module: 	OBAMA / TvService				\n

	Copyright (c) 2014 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding 을 사용하는 source file 입니다.
* MS949 encodingを使用して source fileです。
* Quelldatei, die MS949-Codierung verwendet.
*
*/
/*
 * (c) 2011-2013 Humax Co., Ltd.
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

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include "_xsvc_cas_vmx_iptv_main.h"
#include "vmx_vr_iptvh_api.h"

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
/*
typedef enum
{
	eSvcXxx_Type,

} xsvcXxx_Type_e;

typedef  struct
{
	...
} xsvcXxx_Type_t;
*/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/
/*
// Local function
STATIC HERROR xsvc_xxx_DoSomething_Yyy( ... )
{
	...
}
*/

HERROR vmx_vr_iptvh_api_RegisterControlWordCallback( void *pVmxContext , tControlWordCallback pControlWordCallback , void *pUserParam , int nAlgorithmCount , int *nAlgorithmList)
{
	HxLOG_Print("IPTV API [Enter]\n");
	if(pVmxContext == NULL)
	{
		HxLOG_Print("IPTV API [Exit] pVmxContext is NULL... Check previous VmxContext...\n");
		return ERR_FAIL;
	}

	if(VMRegisterControlWordCallback(pVmxContext, pControlWordCallback, pUserParam, nAlgorithmCount, nAlgorithmList) != ERR_OK)
	{
		HxLOG_Print("IPTV API [Exit] Error\n");
		return ERR_FAIL;
	}
	HxLOG_Print("IPTV API [Exit]\n");
	return ERR_OK;
}

/*
// Global function


/**
* vmx_vr_iptvh_api_CreateContext
*	This function creates a new Decryption object context.
*
* @param[in] pucIniFileName	: INIFileName = NULL. the INI filename must be set to NULL because the INI filename is defined by the VMConfig or VMConfigOff API.
* @param[in] bUseIniFile	: UseIniFile = 1. Since Ithe NI file must be used, this parameter is required to set the non-zero value.
*							Non-zero = enable INI file processing.
*							0 = disable INI file processing.
* @param[out] pVmxContext	: Success = non-NULL pointer to a context object. Failure = NULL pointer.
*
* @return  #  returns ERR_OK or ERR_FAIL
*
* @attention
*/
HERROR vmx_vr_iptvh_api_CreateContext(HINT8 *pucIniFileName, HBOOL bUseIniFile, void** pVmxContext)
{
	HxLOG_Print("IPTV API [Enter]\n");
	if(*pVmxContext != NULL)
	{
		HxLOG_Print("IPTV API [Exit] pVmxContext is not NULL... Check previous VmxContext...\n");
		*pVmxContext = NULL;
	}
	if(pucIniFileName != NULL)
	{
		// ViewRight STB for IPTV v3.5를 기준으로, IniFileName은 항상 NULL로 전달 되어야 한다.
		// INI filename은 VMConfig나 VMConfigOffline API를 호출 시 설정하기 때문이다.
		// 추후 library 변경 사항이 있으면 수정이 필요 할 수 있음.
		HxLOG_Error("pVmxContext is not NULL... Check previous VmxContext...\n");
		pucIniFileName = NULL;
	}

	*pVmxContext = VMCreateContext(pucIniFileName, bUseIniFile);

	if ( NULL == *pVmxContext )
	{
		HxLOG_Error("IPTV API [Exit] Failed to create decryption context...\n");
		return ERR_FAIL;
	}
	HxLOG_Print("IPTV API [Exit]\n");
	return ERR_OK;
}

/**
* vmx_vr_iptvh_api_ResetStream
*	This function tells the libraries when the application has switched to a new and different stream.
*
* @param[in] pVmxContext	: Pointer to a decryption context.
*
* @return  #  returns ERR_OK or ERR_FAIL
*
* @attention
*/
HERROR vmx_vr_iptvh_api_ResetStream(void* pVmxContext)
{
	HxLOG_Print("IPTV API [Enter]\n");
	if(pVmxContext == NULL)
	{
		HxLOG_Error("IPTV API [Exit] pVmxContext is NULL!! Check VmxContext...\n");
		return ERR_FAIL;
	}

	if(VMResetStream(pVmxContext) == 0)
	{
		HxLOG_Print("IPTV API [Exit]\n");
		return ERR_OK;
	}

	HxLOG_Error("IPTV API [Exit] Reset FAILED!!\n");
	return ERR_FAIL;
}

/**
* vmx_vr_iptvh_api_DecryptStreamData
*
*
* @param[in]
*
* @return  #  returns ERR_OK or ERR_FAIL
*
* @attention
*/
HERROR vmx_vr_iptvh_api_DecryptStreamData(void* pVmxContext)
{
	HxLOG_Error("IPTV API [Enter] This function has to integrated!!\n");
	// Not yet implemented
	// call VMDecryptStreamData()
	return ERR_FAIL;
}

/**
* vmx_vr_iptvh_api_GetCaSystemId
*	This function returns the CA_System_ID that the DVB Consotium assigned to Verimatrix
*	Verimatrix CA system ID가 0x5601로 고정되어 있음. (본 API를 사용하지 않아도 됨)
*
* @param[in]
*
* @return  #  returns ERR_OK or ERR_FAIL
*
* @attention
*/
HERROR vmx_vr_iptvh_api_GetCaSystemId(HUINT16* pusCaSystemId)
{
	*pusCaSystemId = (HUINT16)VMGetCASystemID(NULL);
	HxLOG_Print("IPTV API [Enter] pusCaSystemId(0x%x)\n", *pusCaSystemId);
	return ERR_OK;
}

/**
* vmx_vr_iptvh_api_Client_SetEcm
*	For hardware-decryption systems, the Verimatrix client receives ECMs and sends keys(control words) to hardware. This function sends an ECM to the client for processing,
*	if successfully processed, the control words are sent to the target-specific hardware decryption interface.
*
* @param[in] pVmxContext	: pointer to a decryption context.
* @param[in] nIsSection		: isSection indicates whether the ECM is comprised of one or more transport packets.
*				0 = data is in an MPEG2 TS packet.
*				Non-zero = data is in section format. VMSetECM can handle ECMs received in TS packets when the ECM is carried by a single TS packet. If you are not sure whether
*				the size of the ECM is small enough to fit a TS packet, please send the ECM by section format.
* @param[in] usEcmPid		: used when isSection is non-zero. If ECM is in section format, the ECM pid is required.
* @param[in] pucBuffer		: pointer to the ECM data (or packets) that CA is to process.
* @param[in] usBufferLength	: total number of bytes to be processed.
*
*
* @return  #  returns ERR_OK or ERR_FAIL
*
* @attention
*/
HERROR vmx_vr_iptvh_api_Client_SetEcm(void* pVmxContext, HINT32 nIsSection, HUINT16 usEcmPid, HUINT8* pucBuffer, HUINT16 usBufferLength)
{
	HINT32	nReturn;

	HxLOG_Print("IPTV API [Enter]\n");
	if(pVmxContext == NULL)
	{
		HxLOG_Error("pVmxContext is NULL!! Check VmxContext...\n");
		return ERR_FAIL;
	}

	nReturn = VMSetECM(pVmxContext, nIsSection, usEcmPid, pucBuffer, usBufferLength);

#ifdef CONFIG_DEBUG
	switch(nReturn)
	{
		case -1:
			HxLOG_Error("ERROR... Input Parameters are invalid...\n");
			break;
		case -2:
			HxLOG_Error("ERROR... reject the ECM because there is no Verimatrix ECM	packet...\n");
			break;
		case 0:
			HxLOG_Info("ECM process succeeded...\n");
			break;
		default:
			HxLOG_Error("ERROR... Decode Result ENUM: [%d]...\n", nReturn);
			break;
	}
#endif

	if(nReturn == 0)
	{
		HxLOG_Print("IPTV API [Exit]\n");
		return ERR_OK;
	}
	HxLOG_Print("IPTV API [Exit] Fail\n");
	return ERR_FAIL;
}

/**
* vmx_vr_iptvh_api_UpdateChannelKeys
*	Calling this function forces an update of all of the available Broadcast (RTES) channel keys and starts the Key Manager thread if it is not already running.
*	Do not use this function in offline mode. In offline mode, an error is returned, without any requests made to the VCAS server.
*	ex) online mode로 연결 된 상태에서 decryption 실패시 key update 후 retry... offline mode에서 online mode로 전환시 key update...
*
* @param[in] pVmxContext	: pointer to a decryption context.
*
* @return  #  returns ERR_OK or ERR_FAIL
*
* @attention
*/
HERROR vmx_vr_iptvh_api_UpdateChannelKeys(void* pVmxContext)
{
	HxLOG_Print("IPTV API [Enter]\n");
	if(pVmxContext == NULL)
	{
		HxLOG_Error("IPTV API [Exit] pVmxContext is NULL!! Check VmxContext...\n");
		return ERR_FAIL;
	}

	if ( VMUpdateChannelKeys(pVmxContext) == 0 )
	{
		HxLOG_Print("IPTV API [Exit] Channel keys updated OK...\n");
		return ERR_OK;
	}

	HxLOG_Error("IPTV API [Exit] FAILED to update channel keys!!!\n");
	return ERR_FAIL;
}

/**
* vmx_vr_iptvh_api__RegisterSwDecryptionCb
*	This function sets the callback function to use for returning decrypted data to the application for Software decryption. This function is not needed for
*	hardware decryption.
*
* @param[in] pVmxContext	: pointer to a decryption context.
* @param[in] fnSwDecryptCb	: pointer to the callback function. If a NULL function pointer is used, the Context decrypts data in place.
* @param[in]
* @param[in]
*
* @return  #  returns ERR_OK or ERR_FAIL
*
* @attention	일반적으로는 callback을 등록하지 않고 in place 모드로 decryption 하는 것을 recommend 함
*				S/W Decryption을 아직 지원하지 않으므로 S/W decryption 동작 시키기 전 까지는 사용하지 않음
*				// TODO: 실제 사용 할 때 구조 생각해보고 추가 작업 필요.
*/
HERROR vmx_vr_iptvh_api_RegisterSwDecryptionCb(void* pVmxContext, RenderFunc fnSwDecryptionCb, HUINT32 ulSize, void *pArg)
{
	HxLOG_Print("IPTV API [Enter]\n");
	if(pVmxContext == NULL)
	{
		HxLOG_Error("IPTV API [Exit] pVmxContext is NULL!! Check VmxContext...\n");
		return ERR_FAIL;
	}

	if ( VMSetCallback(pVmxContext, fnSwDecryptionCb, ulSize, (void*)pArg) == 0 )
	{
		HxLOG_Print("IPTV API [Exit] S/W callback registration OK...\n");
		return ERR_OK;
	}

	HxLOG_Error("IPTV API [Exit] S/W callback registration FAILED!!!\n");
	return ERR_OK;
}

/**
* vmx_vr_iptvh_api_RegisterDecodeFailureCb
*	This function sets a callback into the STB application to be called if the Transport Stream decoding fails. If a decode failure occurs, the following pass to the callback:
*		User's parameter
*		Channel on which the decode failed
*		An eVmDecodeReason reason code
*
* @param[in] pVmxContext		: pointer to a decryption context.
* @param[in] fnDecodeFailureCb	: pointer to a function to be called on failure.
* @param[in] pUserParam			: value to be returned to the user when the callback function is called.
*
* @return  #  returns ERR_OK or ERR_FAIL
*
* @attention
*/
HERROR vmx_vr_iptvh_api_RegisterDecodeFailureCb(void* pVmxContext, tDecodeFailureCallback fnDecodeFailureCb, void *pUserParam)
{
	HxLOG_Print("IPTV API [Enter]\n");
	if(pVmxContext == NULL)
	{
		HxLOG_Error("IPTV API [Exit] pVmxContext is NULL!! Check VmxContext...\n");
		return ERR_FAIL;
	}

	VMRegisterDecodeFailureCallback(pVmxContext, fnDecodeFailureCb, pUserParam);
	HxLOG_Print("IPTV API [Exit]\n");
	return ERR_OK;
}

/**
* vmx_vr_iptvh_api_FlushBuffer
*
*
* @param[in]
*
* @return  #  returns ERR_OK or ERR_FAIL
*
* @attention	S/W Decryption 시나리오 동작 할 때 필요한 내용으로 S/W decryption 관련 구현 될 때 해당 부분도 구현 해야 함.
*/
HERROR vmx_vr_iptvh_api_FlushBuffer(void* pVmxContext, HBOOL bTriggerCallback)
{
	HxLOG_Print("IPTV API [Enter] This function has to integrated!!\n");
	return ERR_FAIL;
}

/**
* vmx_vr_iptvh_api_SetConfig
*	Calling this function initializes the client in online mode, or switches it from offline mode to online mode if the client was previously initialized in offline mode.
*	This function sets the file path of the .store file, VCI IP address, performs auto configuration, and provisions the client with the VCAS server. If failure is returned, the
*	client can still decrypt streams using persisted keys (if still valid) or degraded mode keys.
*	After failure, retries are left to the caller. The VMConfig function (or VMConfigOffline function) must be called before VMCreateContext.
*
* @param[in] pStoreFilePath = pointer to the string of the store file path.
* @param[in] pIpAddress = pointer to the string of VCI IP address.
* @param[in] nPort = number of VCI: Boot port.
*
* @return  #  returns ERR_OK or ERR_FAIL
*
* @attention
*/
HERROR vmx_vr_iptvh_api_SetConfig(HUINT8* szStoreFilePath, HUINT8* szIpAddress, HUINT16 usPort)
{
	// 아래 Timeout, Retry, Preperred IP ver은 setting 값을 변경할 일이 거의 없다고 봄.
	// model 별로 define해서 처리는 할 수 있음.

	//nTimeout = boot server connection timeout in seconds.
	HINT32 		nTimeout = 2;
	//	nRetries = number of retries if first connection attempt is not responded.
	HINT32 		nRetries = 2;
	//nIpVersionPreferred = determines which IP version is preferred
		//	0:No Preference connect using DNS resolutions in returned order regardless of protocol version
		//	4:IPv4 connect using IPv4 DNS resolutions before trying IPv6 Resolutions
		//	6:IPv6 connect using IPv6 DNS resolutions before trying IPv4 Resolutions
	HUINT16 	usIpVersionPreferred = 0;

	HxLOG_Print("IPTV API [Enter]\n");
	if ( VMConfig( szStoreFilePath, szIpAddress, usPort, nTimeout, nRetries, usIpVersionPreferred ) == 0 )
	{
		HxLOG_Info("IPTV API [Exit] Set Config OK...\n");
		return ERR_OK;
	}

	HxLOG_Info("IPTV API [Exit] FAILED to set config!!!\n");
	return ERR_FAIL;
}

/**
* vmx_vr_iptvh_api_SetConfigOffline
*
*
* @param[in]
*
* @return  #  returns ERR_OK or ERR_FAIL
*
* @attention
*/
HERROR vmx_vr_iptvh_api_SetConfigOffline(HUINT8* szStoreFilePath)
{
	HxLOG_Print("IPTV API [Enter]\n");
	if ( VMConfigOffline(szStoreFilePath) == 0 )
	{
		HxLOG_Info("IPTV API [Exit] Set Config OK...\n");
		return ERR_OK;
	}

	HxLOG_Error("IPTV API [Exit] FAILED to set config!!!\n");
	return ERR_FAIL;
}

/**
* vmx_vr_iptvh_api_DestroyClient
*	This function destroys the client and unallocates all memory allocated by the client. This method is the last method to be called before shutting down the client.
*
* @param[in]
*
* @return  #  returns ERR_OK or ERR_FAIL
*
* @attention 꼭 불러야 하는지는 의문.
*/
HERROR vmx_vr_iptvh_api_DestroyClient(void)
{
	// Client is removed from memory.
	// continue with STB shutdown or initialize and start the client again.
	HxLOG_Print("IPTV API [Enter]\n");
	VMDestroyClient();

	return ERR_OK;
}

/**
* vmx_vr_iptvh_api_SetEmm
*	This function allows the integrator to pass the Verimatrix EMMs found in the DVB transport stream. It is the integrator’s responsibility to filter out
*	Verimatrix EMMs if found in the DVB transport stream. The Client will allocate memory and copy the TS packets. The allocated memory is released after the
*	Client has processed the TS packets. This API places an EMM found in the DVB stream into the EMM Receiver’s EMM input queue. This function returns 0
*	if queued properly.
*	The Queued EMMs are discarded if the client does not have a valid EMM decryption key. The client will not have a valid EMM decryption key when
*	operating in offline mode, so any EMMs passed to the client are discarded. It is recommended that this function not be used in offline mode. The client logs
*	an error message for each EMM for which it does not have a valid decryption key.
*	Successfully initializing or switching the client to online mode (via VMConfig) does not indicate that the client has an EMM decryption key. The client will
*	attempt to obtain the EMM decryption key sometime after VMConfig returns.
*
* @param[in]
*	ppucTSPacketBuffer: pointer to array of pointers to 188 byte TS packets
*	pulNumTSPackets = pointer to the number of TS packet pointers found in ppTSPacketBuffer array.
*
* @return  #  returns ERR_OK or ERR_FAIL
*
* @attention
*/
HERROR vmx_vr_iptvh_api_SetEmm(HUINT8** ppucTSPacketBuffer, HUINT32* pulNumTSPackets)
{
	HxLOG_Print("IPTV API [Enter]\n");
	if ( VMSetEMM(ppucTSPacketBuffer, pulNumTSPackets) == 0 )
	{
		HxLOG_Info("IPTV API [Exit] OK...\n");
		return ERR_OK;
	}

	HxLOG_Error("IPTV API [Exit] FAILED!!!\n");
	return ERR_FAIL;
}

/**
* vmx_vr_iptvh_api_SetMwAuthenticator
*	The Middleware Authenticator (MEAB) provides a way for the STB middleware to pass data, along with the request, to the server-side entitlement system.
*	This data is opaque to the client. The data passed is provided to the entitlement system in the same format. Any transformations applied during
*	transmission are removed before providing the data to the entitlement system.
*	This function can be called at any time, but only has an effect when the client is in online mode.
*
* @param[in]
*
* @return  #  returns ERR_OK or ERR_FAIL
*
* @attention
*/
HERROR vmx_vr_iptvh_api_SetMwAuthenticator(HUINT8* pucData, HUINT32 ulLength)
{
	HxLOG_Print("IPTV API [Enter]\n");
	if ( VMSetMiddlewareAuthenticator(pucData, ulLength) == 0 )
	{
		return ERR_OK;
	}

	HxLOG_Error("IPTV API [Exit] VMSetMiddlewareAuthenticator() failed!!!\n");
	return ERR_FAIL;
}

/**
* vmx_vr_iptvh_api_SetVcasCommProtocol
*	This method is used to set the VCAS server version so that the software will use the message protocol compatible with that version.
*
* @param[in]
*
* @return  #  returns ERR_OK or ERR_FAIL
*
* @attention: Before VMConfig and VMConfigOffline. This method is optional.
*/
HERROR vmx_vr_iptvh_api_SetVcasCommProtocol(eVcasVersion_t eVersion)
{
	HxLOG_Print("IPTV API [Enter]\n");
	if ( VMSetVcasCommunicationProtocol(eVersion) == 0 )
	{
		HxLOG_Info("IPTV API [Exit] vmx_vr_iptvh_api_SetVcasCommProtocol() OK...\n");
		return ERR_OK;
	}

	HxLOG_Error("IPTV API [Exit] FAILED vmx_vr_iptvh_api_SetVcasCommProtocol()!!!\n");
	return ERR_FAIL;
}

/**
* vmx_vr_iptvh_api_StartService
*	The STB middleware sends the Sevice Index, PidCount and Pointer to StreamPid for a specific Context needed by the Advanced Security Library.
*
* @param[in]
*
* @return  #  returns ERR_OK or ERR_FAIL
*
* @attention
*/
HERROR vmx_vr_iptvh_api_StartService(void* pVmxContext, HUINT8 ucSvcIdx, HUINT8 ucPidCount, HUINT16 *pusStreamPid)
{
	HxLOG_Print("IPTV API [Enter]\n");
	if(pVmxContext == NULL)
	{
		HxLOG_Error("IPTV API [Exit] pVmxContext is NULL!! Check VmxContext...\n");
		return ERR_FAIL;
	}

	if ((ucSvcIdx < VMX_CAS_VALID_SVCIDX_START) || (ucSvcIdx > VMX_CAS_VALID_SVCIDX_END))
	{
		HxLOG_Error("Error... Invaild usSvcIdx!! SvcIdx:[0x%X]...\n", ucSvcIdx);
		return ERR_FAIL;
	}

	HxLOG_Print("\tSvcIdx(0x%x), usPidCount(%d)\n", ucSvcIdx, ucPidCount);

	if ( VMServiceStart(pVmxContext, ucSvcIdx, ucPidCount, pusStreamPid) == 0 )
	{
		HxLOG_Info("IPTV API [Exit] vmx_vr_iptvh_api_StartService() OK...\n");
		return ERR_OK;
	}

	HxLOG_Error("IPTV API [Exit] FAILED vmx_vr_iptvh_api_StartService()!!!\n");
	return ERR_FAIL;
}

/**
* vmx_vr_iptvh_api_StopService
*	The STB middleware stops the service for a specified Service Index.
*
* @param[in]
*
* @return  #  returns ERR_OK or ERR_FAIL
*
* @attention
*/
HERROR vmx_vr_iptvh_api_StopService(void* pVmxContext, HUINT16 usSvcIdx)
{
	HxLOG_Print("IPTV API [Enter]\n");
	if(pVmxContext == NULL)
	{
		HxLOG_Error("IPTV API [Exit] pVmxContext is NULL!! Check VmxContext...\n");
		return ERR_FAIL;
	}

	if ((usSvcIdx < VMX_CAS_VALID_SVCIDX_START) || (usSvcIdx > VMX_CAS_VALID_SVCIDX_END))
	{
		HxLOG_Error("Error... Invaild usSvcIdx!! SvcIdx:[0x%X]...\n", usSvcIdx);
		return ERR_FAIL;
	}

	VMServiceStop(pVmxContext, usSvcIdx);

	HxLOG_Info("IPTV API [Exit] vmx_vr_iptvh_api_StopService() OK...\n");
	return ERR_OK;
}

/**
* vmx_vr_iptvh_api_SetErrorLevel
*	This function sets the error level to control how much information is logged to the error log. The Error Level defaults to 0, which logs actual errors only. Higher error levels
*	activate more informational or debugging log messages. The highest error level (5) activates all of the logging statements in the libraries.
*	The Error Level can also be set by using the ERRORLEVEL= setting in the INI file.
*	Note: VMSetErrorLevel ignores the context parameter. Value is set globally.
*
* @param[in]
*
* @return  #  returns ERR_OK or ERR_FAIL
*
* @attention
*/
HERROR vmx_vr_iptvh_api_SetErrorLevel(void* pVmxContext, HINT32 nErrLevel)
{
	HxLOG_Print("IPTV API [Enter]\n");
	if(pVmxContext == NULL)
	{
		HxLOG_Error("IPTV API [Exit] pVmxContext is NULL!! Check VmxContext...\n");
		return ERR_FAIL;
	}

	VMSetErrorLevel(pVmxContext, nErrLevel);

	HxLOG_Info("IPTV API [Exit] vmx_vr_iptvh_api_SetErrorLevel() OK...\n");
	return ERR_OK;
}

/**
* vmx_vr_iptvh_api_GetVersionString
*	This function retrieves a pointer to the version string for the client library.
*
* @param[in]
*
* @return  #  returns ERR_OK or ERR_FAIL
*
* @attention
*/
const HCHAR* vmx_vr_iptvh_api_GetVersionString(void)
{
	HxLOG_Print("IPTV API [Enter]\n");
	return VMGetVersionString();
}

/**
* vmx_vr_iptvh_api_GetUniqueId
*	This function retrieves the client identification string into the provided buffer.
*
* @param[in]
*
* @return  #  returns ERR_OK or ERR_FAIL
*
* @attention
*/
HERROR vmx_vr_iptvh_api_GetUniqueId(HINT8* pcBuffer, HUINT32 ulMaxLength, HUINT32 *pulLength)
{
	HxLOG_Print("IPTV API [Enter]\n");
	if ( GetUniqueIdentifier(pcBuffer, ulMaxLength, pulLength) == 0 )
	{
		HxLOG_Info("IPTV API [Exit] vmx_vr_iptvh_api_GetUniqueId() OK...\n");
		return ERR_OK;
	}

	HxLOG_Error("IPTV API [Exit] FAILED vmx_vr_iptvh_api_GetUniqueId()!!!\n");
	return ERR_FAIL;
}

/**
* vmx_vr_iptvh_api_GetKeyUpdateWindow
*
*
* @param[in]
*
* @return  #  returns ERR_OK or ERR_FAIL
*
* @attention
*/
HERROR vmx_vr_iptvh_api_GetKeyUpdateWindow(void* pVmxContext, HINT32 *pnMinTime, HINT32 *pnMaxTime, HINT32 *pnSuggestedTime)
{
	HxLOG_Print("IPTV API [Enter]\n");
	if(pVmxContext == NULL)
	{
		HxLOG_Error("IPTV API [Exit] pVmxContext is NULL!! Check VmxContext...\n");
		return ERR_FAIL;
	}

	if ( VMGetKeyUpdateWindow(pVmxContext, pnMinTime, pnMaxTime, pnSuggestedTime) == 0 )
	{
		HxLOG_Info("IPTV API [Exit] vmx_vr_iptvh_api_GetKeyUpdateWindow() OK...\n");
		return ERR_OK;
	}

	HxLOG_Error("IPTV API [Exit] FAILED vmx_vr_iptvh_api_GetKeyUpdateWindow()!!!\n");
	return ERR_FAIL;
}

/**
* vmx_vr_iptvh_api_RemoveVodKey
*
*
* @param[in]
*
* @return  #  returns ERR_OK or ERR_FAIL
*
* @attention
*/
HERROR vmx_vr_iptvh_api_RemoveVodKey(void)
{
	HxLOG_Error("IPTV API [Enter] This function has to integrated!!\n");
	return ERR_FAIL;
}

/**
* vmx_vr_iptvh_api_PurgeVodKeyCache
*
*
* @param[in]
*
* @return  #  returns ERR_OK or ERR_FAIL
*
* @attention
*/
HERROR vmx_vr_iptvh_api_PurgeVodKeyCache(void)
{
	HxLOG_Error("IPTV API [Enter] This function has to integrated!!\n");
	return ERR_FAIL;
}

/**
* vmx_vr_iptvh_api_PurgeLastKeyCache
*	The context keeps a small cache of the last key requested/used for decryption. This function clears this cache manually.
*
* @param[in]
*
* @return  #  returns ERR_OK or ERR_FAIL
*
* @attention
*/
HERROR vmx_vr_iptvh_api_PurgeLastKeyCache(void* pVmxContext)
{
	HxLOG_Print("IPTV API [Enter]\n");
	if(pVmxContext == NULL)
	{
		HxLOG_Error("IPTV API [Exit] pVmxContext is NULL!! Check VmxContext...\n");
		return ERR_FAIL;
	}

	VMPurgeLastKeyCache(pVmxContext);
	HxLOG_Info("IPTV API [Exit] vmx_vr_iptvh_api_PurgeLastKeyCache() OK...\n");
	return ERR_OK;
}

/**
* vmx_vr_iptvh_api_PurgePvrKeyCache
*
*
* @param[in]
*
* @return  #  returns ERR_OK or ERR_FAIL
*
* @attention
*/
HERROR vmx_vr_iptvh_api_PurgePvrKeyCache(void* pVmxContext)
{
	HxLOG_Print("IPTV API [Enter]\n");
	if(pVmxContext == NULL)
	{
		HxLOG_Error("IPTV API [Exit] pVmxContext is NULL!! Check VmxContext...\n");
		return ERR_FAIL;
	}

	VMPurgePvrKeyCache(pVmxContext);
	HxLOG_Info("IPTV API [Exit] vmx_vr_iptvh_api_PurgePvrKeyCache() OK...\n");
	return ERR_OK;
}

/**
* vmx_vr_iptvh_api_RegisterStoreReadCb
*
*
* @param[in]
*
* @return  #  returns ERR_OK or ERR_FAIL
*
* @attention
*/
HERROR vmx_vr_iptvh_api_RegisterStoreReadCb(DataReadFunc fnReader, void *pArg)
{
	HxLOG_Print("IPTV API [Enter]\n");
	VMSetStoreReadCallback(fnReader,(void *)pArg);
	return ERR_OK;
}

/**
* vmx_vr_iptvh_api_RegisterStoreWriteCb
*
*
* @param[in]
*
* @return  #  returns ERR_OK or ERR_FAIL
*
* @attention
*/
HERROR vmx_vr_iptvh_api_RegisterStoreWriteCb(DataWriteFunc fnWriter, void *pArg)
{
	HxLOG_Print("IPTV API [Enter]\n");
	VMSetStoreWriteCallback(fnWriter,(void *)pArg);
	return ERR_OK;
}

/**
* vmx_vr_iptvh_api_RegisterUserDataCb
*
*
* @param[in]
*
* @return  #  returns ERR_OK or ERR_FAIL
*
* @attention
*/
HERROR vmx_vr_iptvh_api_RegisterUserDataCb(void* pVmxContext, tUserDataCallback fnUserDataCb, void *pUserParam)
{
	HxLOG_Print("IPTV API [Enter]\n");
	if(pVmxContext == NULL)
	{
		HxLOG_Error("IPTV API [Exit] pVmxContext is NULL!! Check VmxContext...\n");
		return ERR_FAIL;
	}


	VMRegisterUserdataCallback(pVmxContext, fnUserDataCb, pUserParam);
	HxLOG_Info("IPTV API [Exit] vmx_vr_iptvh_api_RegisterUserDataCb() OK...\n");
	return ERR_OK;
}

/**
* vmx_vr_iptvh_api_LogMessage
*
*
* @param[in]
*
* @return  #  returns ERR_OK or ERR_FAIL
*
* @attention
*/
HERROR vmx_vr_iptvh_api_LogMessage(HINT32 nLevel, const HCHAR *pzMsg)
{
	HxLOG_Print("IPTV API [Enter]\n");
	if ( VMLogMessage(nLevel, pzMsg) == 0 )
	{
		HxLOG_Info("IPTV API [Exit] vmx_vr_iptvh_api_LogMessage() OK...\n");
		return ERR_OK;
	}

	HxLOG_Error("IPTV API [Exit] FAILED vmx_vr_iptvh_api_LogMessage()!!!\n");
	return ERR_FAIL;
}

/**
* vmx_vr_iptvh_api_DestroyContext
*
*
* @param[in]
*
* @return  #  returns ERR_OK or ERR_FAIL
*
* @attention
*/
HERROR vmx_vr_iptvh_api_DestroyContext(void* pVmxContext)
{
	HxLOG_Print("IPTV API [Enter]\n");
	if(pVmxContext == NULL)
	{
		HxLOG_Error("IPTV API [Exit] pVmxContext is NULL!! Check VmxContext...\n");
		return ERR_FAIL;
	}

	VMDestroyContext(pVmxContext);

	HxLOG_Info("IPTV API [Exit] vmx_vr_iptvh_api_DestroyContext() OK...\n");
	return ERR_OK;

}

/**
* vmx_vr_iptvh_api_SetRatingLevel
*
*
* @param[in]
*
* @return  #  returns ERR_OK or ERR_FAIL
*
* @attention
*/
HERROR vmx_vr_iptvh_api_SetRatingLevel(void* pVmxContext, HUINT8 ucRating)
{
	HxLOG_Print("IPTV API [Enter]\n");
	if(pVmxContext == NULL)
	{
		HxLOG_Error("IPTV API [Exit] pVmxContext is NULL!! Check VmxContext...\n");
		return ERR_FAIL;
	}

	VMSetRatingLevel(pVmxContext, ucRating);

	HxLOG_Info("IPTV API [Exit] vmx_vr_iptvh_api_SetRatingLevel() OK...\n");
	return ERR_OK;
}

/**
* vmx_vr_iptvh_api_GetRatingLevel
*	This function returns the current STB Rating Level and optionally, the override state, and channel Rating Level.
*	If pbOverride is non-NULL, the override state (true or false) is stored in *pbOverride. If pucChannelRatingLevel is non-NULL,
*	the current channel's Rating Level is stored in *pucChannelRatingLevel.
*
* @param[in] pVmxContext = pointer to a decryption context.
* @param[in] pbOverride = pointer to location to store whether the Rating Level is currently overridden(see VMOverrideRatingLevel() on page 2-44). If NULL, override state is not
*	stored.
* @param[in] pucChannelRatingLevel = pointer to location to store the current Rating Level of the channel. If NULL, the current channel Rating Level is not stored.
* @param[in] pucCurrentStbRatingLevel = Current Rating Level of the STB as set by VMSetRatingLevel(). 0-255 = general viewing to XXX (default for STB is 255).
*
* @return  #  returns ERR_OK or ERR_FAIL
*
* @attention
*/
HERROR vmx_vr_iptvh_api_GetRatingLevel(void* pVmxContext, HUINT8 *pucOverride, HUINT8 *pucChannelRatingLevel, HUINT8 *pucCurrentStbRatingLevel)
{
	HxLOG_Print("IPTV API [Enter]\n");
	if(pVmxContext == NULL)
	{
		HxLOG_Error("IPTV API [Exit] pVmxContext is NULL!! Check VmxContext...\n");
		return ERR_FAIL;
	}

	*pucCurrentStbRatingLevel = VMGetRatingLevel(pVmxContext, pucOverride, pucChannelRatingLevel);

	HxLOG_Info("IPTV API [Exit] vmx_vr_iptvh_api_GetRatingLevel() OK...\n");
	return ERR_OK;
}

/**
* vmx_vr_iptvh_api_OverrideRatingLevel
*	This function temporarily overrides the Rating Level for the specified channel. Override is canceled when: the channel is changed,
*	the stream's Rating Level falls below or is equal to the STB's Rating Level, or a call is made to VMSetRatingLevel() . see page 2-42.
*
* @param[in] pContext = pointer to a decryption context.
* @param[in] pucCurrentStbRatingLevel = Current Rating Level of the STB as set by VMSetRatingLevel(). 0-255 = general viewing to XXX (default for STB is 255).
*
* @return  #  returns ERR_OK or ERR_FAIL
*
* @attention PC에 걸려 A/V가 막혀있는 content에 대해 다시 A/V 출력해주는 함수로 보임.
*/
HERROR vmx_vr_iptvh_api_OverrideRatingLevel(void* pVmxContext, HUINT8 *pucCurrentStbRatingLevel)
{
	HxLOG_Print("IPTV API [Enter]\n");
	if(pVmxContext == NULL)
	{
		HxLOG_Error("IPTV API [Exit] pVmxContext is NULL!! Check VmxContext...\n");
		return ERR_FAIL;
	}

	// TODO: Spec에서 정의한 VMOverrideRatingLevel 함수의 return 과 lib에서 정의한 return이 다름. verimatrix에 문의 필요.
	 *pucCurrentStbRatingLevel = VMOverrideRatingLevel(pVmxContext);

	HxLOG_Info("IPTV API [Exit] vmx_vr_iptvh_api_OverrideRatingLevel(%d) OK...\n", *pucCurrentStbRatingLevel );
	return ERR_OK;
}

/**
* vmx_vr_iptvh_api_RegisterFingerPrintingCb
*
*
* @param[in]
*
* @return  #  returns ERR_OK or ERR_FAIL
*
* @attention
*/
HERROR vmx_vr_iptvh_api_RegisterFingerPrintingCb(void* pVmxContext, tFingerPrintingCallback fnFingerPrintingCb, void *pUserParam)
{
	HxLOG_Print("IPTV API [Enter]\n");
	if(pVmxContext == NULL)
	{
		HxLOG_Error("IPTV API [Exit] pVmxContext is NULL!! Check VmxContext...\n");
		return ERR_FAIL;
	}

	if ( VMRegisterFingerPrintingCallback(pVmxContext, fnFingerPrintingCb, pUserParam) == 0 )
	{
		HxLOG_Info(" IPTV API [Exit]vmx_vr_iptvh_api_RegisterFingerPrintingCb() OK...\n");
		return ERR_OK;
	}

	HxLOG_Error("IPTV API [Exit]FAILED vmx_vr_iptvh_api_RegisterFingerPrintingCb()!!!\n");
	return ERR_FAIL;
}

/**
* vmx_vr_iptvh_api_RegisterCopyControlCb
*
*
* @param[in]
*
* @return  #  returns ERR_OK or ERR_FAIL
*
* @attention
*/
HERROR vmx_vr_iptvh_api_RegisterCopyControlCb(void* pVmxContext, tCopyControlCallback fnCopyCtrlCb, void *pUserParam)
{
	HxLOG_Print("IPTV API [Enter]\n");
	if(pVmxContext == NULL)
	{
		HxLOG_Error("IPTV API [Exit] pVmxContext is NULL!! Check VmxContext...\n");
		return ERR_FAIL;
	}

	if ( VMRegisterCopyControlCallback(pVmxContext, fnCopyCtrlCb, pUserParam) == 0 )
	{
		HxLOG_Info("IPTV API [Exit] vmx_vr_iptvh_api_RegisterCopyControlCb() OK...\n");
		return ERR_OK;
	}

	HxLOG_Error("IPTV API [Exit] FAILED vmx_vr_iptvh_api_RegisterCopyControlCb()!!!\n");
	return ERR_FAIL;
}

/**
* vmx_vr_iptvh_api_RegisterIrdOpaqueDataCb
*
*
* @param[in]
*
* @return  #  returns ERR_OK or ERR_FAIL
*
* @attention
*/
HERROR vmx_vr_iptvh_api_RegisterIrdOpaqueDataCb(tIrdOpaqueDataCallback fnIrdOpaqueDataCb, void *pUserParam)
{
	HxLOG_Print("IPTV API [Enter]\n");
	if ( VMRegisterIrdOpaqueCallback(fnIrdOpaqueDataCb, pUserParam) == 0 )
	{
		HxLOG_Info("IPTV API [Exit] vmx_vr_iptvh_api_RegisterIrdOpaqueDataCb() OK...\n");
		return ERR_OK;
	}

	HxLOG_Error("IPTV API [Exit] FAILED vmx_vr_iptvh_api_RegisterIrdOpaqueDataCb()!!!\n");
	return ERR_FAIL;
}

/**
* vmx_vr_iptvh_api_RegisterOsdMessageCb
*
*
* @param[in]
*
* @return  #  returns ERR_OK or ERR_FAIL
*
* @attention
*/
HERROR vmx_vr_iptvh_api_RegisterOsdMessageCb(tOsdMessageCallback fnOsdMsgCb, void *pUserParam)
{
	HxLOG_Print("IPTV API [Enter]\n");
	if ( VMRegisterOsdMessageCallback(fnOsdMsgCb, pUserParam) == 0 )
	{
		HxLOG_Info("IPTV API [Exit] vmx_vr_iptvh_api_RegisterOsdMessageCb() OK...\n");
		return ERR_OK;
	}

	HxLOG_Error("IPTV API [Exit] FAILED vmx_vr_iptvh_api_RegisterOsdMessageCb()!!!\n");
	return ERR_FAIL;
}
