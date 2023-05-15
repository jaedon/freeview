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

#include "octo_common.h"

#include "UniversalClient_Common_SPI.h"
#include "UniversalClient_Common_API.h"

#include "iruc_spi.h"



STATIC uc_spi_implementation_st s_stUcasSpiObj =
{
	UniversalClientSPI_Memory_Malloc,                      //	void* (*Memory_Malloc)(uc_uint32 bytes);
	UniversalClientSPI_Memory_Free,                        //	void (*Memory_Free)(void * lpVoid);
	UniversalClientSPI_Semaphore_Open,                     //	uc_result (*Semaphore_Open)(uc_uint32 initialValue, uc_semaphore_handle * pSemaphoreHandle);
	UniversalClientSPI_Semaphore_Post,                     //	uc_result (*Semaphore_Post)(uc_semaphore_handle semaphoreHandle);
	UniversalClientSPI_Semaphore_Wait,                     //	uc_result (*Semaphore_Wait)(uc_semaphore_handle semaphoreHandle);
	UniversalClientSPI_Semaphore_Close,                    //	uc_result (*Semaphore_Close)(uc_semaphore_handle * pSemaphoreHandle);
	UniversalClientSPI_Mutex_Open,                         //	uc_result (*Mutex_Open)(uc_mutex_handle * pMutexHandle);
	UniversalClientSPI_Mutex_Lock,                         //	uc_result (*Mutex_Lock)(uc_mutex_handle mutexHandle);
	UniversalClientSPI_Mutex_Unlock,                       //	uc_result (*Mutex_Unlock)(uc_mutex_handle mutexHandle);
	UniversalClientSPI_Mutex_Close,                        //	uc_result (*Mutex_Close)(uc_mutex_handle * pMutexHandle);
	UniversalClientSPI_Thread_Open,                        //	uc_result (*Thread_Open)(uc_threadproc threadProc, void* lpVoid, uc_thread_handle * pThreadHandle);
	UniversalClientSPI_Thread_Sleep,                       //	uc_result (*Thread_Sleep)(uc_thread_handle hThreadHandle, uc_uint16 wDelay);
	UniversalClientSPI_Thread_Close,                       //	uc_result (*Thread_Close)(uc_thread_handle * pThreadHandle);
	UniversalClientSPI_PS_Delete,                          //	uc_result (*PS_Delete)(uc_uint32 index);
	UniversalClientSPI_PS_Write,                           //	uc_result (*PS_Write)(uc_uint32 index, uc_uint32 offset, const uc_bytes *pData);
	UniversalClientSPI_PS_Read,                            //	uc_result (*PS_Read)(uc_uint32 index, uc_uint32 offset, uc_bytes *pData);
	UniversalClientSPI_PS_Initialize,                      //	uc_result (*PS_Initialize)(void);
	UniversalClientSPI_PS_Terminate,                       //	uc_result (*PS_Terminate)(void);
	UniversalClientSPI_Device_GetCSSN,                     //	uc_result (*Device_GetCSSN)(uc_bytes * pData);
	UniversalClientSPI_Device_SetCSSK,                     //	uc_result (*Device_SetCSSK)(const uc_bytes * pKeyMaterial);
	UniversalClientSPI_Device_SetExtendedCSSK,		//	uc_result (*Device_SetExtendedCSSK)(const uc_cssk_info * pCSSKInfo);
	UniversalClientSPI_Stream_Open,                       	 //    uc_result (*Stream_Open)( uc_connection_stream_type streamType, void *pStreamOpenParams,  uc_stream_handle *pStreamHandle);
	UniversalClientSPI_Stream_AddComponent,                //	uc_result (*Stream_AddComponent)(        uc_stream_handle streamHandle,        const uc_component *pComponent);
	UniversalClientSPI_Stream_RemoveComponent,             //	uc_result (*Stream_RemoveComponent)(        uc_stream_handle streamHandle,        const uc_component *pComponent);
	UniversalClientSPI_Stream_Start,                       //	uc_result (*Stream_Start)(uc_stream_handle streamHandle);
	UniversalClientSPI_Stream_Stop,                        //	uc_result (*Stream_Stop)(uc_stream_handle streamHandle);
	UniversalClientSPI_Stream_OpenFilter,                  //	uc_result (*Stream_OpenFilter)(        uc_stream_handle streamHandle,        uc_filter_handle *pFilterHandle);
	UniversalClientSPI_Stream_SetFilter,                   //	uc_result (*Stream_SetFilter)(        uc_filter_handle filterHandle,        const uc_filter *pFilterRules);
	UniversalClientSPI_Stream_CloseFilter,                 //	uc_result (*Stream_CloseFilter)(        uc_stream_handle streamHandle,        uc_filter_handle *pFilterHandle);
	UniversalClientSPI_Stream_Connect,                     //	uc_result (*Stream_Connect)(        uc_stream_handle streamHandle,        uc_connection_handle connectionHandle,        uc_notify_callback notifyCallback);
#if (UCCA_MAJOR_VERSION >= 4)
	NULL,	//  uc_result (*Stream_Connect)(uc_stream_handle streamHandle, uc_connection_handle connectionHandle, uc_notify_callback notifyCallback);
#endif
	UniversalClientSPI_Stream_Disconnect,                  //	uc_result (*Stream_Disconnect)(        uc_stream_handle streamHandle,        uc_connection_handle connectionHandle);
	UniversalClientSPI_Stream_SetDescramblingKey,          //	uc_result (*Stream_SetDescramblingKey)(        uc_stream_handle streamHandle,        const uc_key_info *pKeyInfo,        uc_uint32 keyVersion);
	UniversalClientSPI_Stream_CleanDescramblingKey,        //	uc_result (*Stream_CleanDescramblingKey)(        uc_stream_handle streamHandle);
	UniversalClientSPI_Stream_Close,                       //	uc_result (*Stream_Close)(        uc_stream_handle *pStreamHandle);
	UniversalClientSPI_Device_GetPrivateData,              //	uc_result (*Device_GetPrivateData)(uc_bytes * pData);
	UniversalClientSPI_Device_GetSecurityState,            //	uc_result (*Device_GetSecurityState)(uc_device_security_state * pDeviceSecurityState);
	UniversalClientSPI_Device_GetPlatformIdentifiers,      //	uc_result (*Device_GetPlatformIdentifiers)(uc_device_platform_identifiers * pDevicePlatformIdentifiers);
	UniversalClientSPI_PVR_SetSessionKey,                  //	uc_result (*PVR_SetSessionKey)(uc_uint16 recordId, const uc_bytes *pPVRSK);
	UniversalClientSPI_PVR_SetExtendedSessionKey,	//	uc_result (*PVR_SetExtendedSessionKey)(uc_stream_handle streamHandle, const uc_pvrsk_info *pPVRSKInfo);
	UniversalClientSPI_CopyControl_Macrovision_SetConfig,  //	uc_result (*CopyControl_Macrovision_SetConfig)(uc_macrovision_config mac_config_data);
	UniversalClientSPI_CopyControl_Macrovision_SetMode,    //	uc_result  (*CopyControl_Macrovision_SetMode)(uc_cc_content_handle handle,uc_macrovision_mode mac_mode);
	UniversalClientSPI_CopyControl_SetCCI,                 //	uc_result  (*CopyControl_SetCCI)(uc_cc_content_handle handle,uc_cc_info cci_info);
	UniversalClientSPI_FatalError,                          //	void  (*FatalError)(uc_fatal_error_type type,void* lpVoid);
	UniversalClientSPI_Smartcard_Open,			 // uc_result  (*Smartcard_Open)(uc_uint32 *pSmartcardID, uc_sc_open_parameter *pSCOpenData),
	UniversalClientSPI_Smartcard_Close,			// uc_result (*Smartcard_Close)(uc_uint32 smartcardID)
	UniversalClientSPI_Smartcard_Reset,			// uc_result (*Smartcard_Reset)(uc_uint32 smartcardID)
	UniversalClientSPI_Smartcard_Communicate,	//uc_result (*Smartcard_Communicate)(uc_uint32 smartcardID, uc_uint32 headerLen, uc_uint32 payloadLen,uc_byte *pSendBuffer, uc_uint32 *pRecvDataLen, uc_byte *pRecvBuffer )
	UniversalClientSPI_DateTime_GetTimeOfDay,	//uc_result  (*DateTime_GetTimeOfDay)(uc_time *pCurrentTime);
	UniversalClientSPI_Crypto_Verify,				//uc_result  (*Crypto_Verify)(uc_crypto_info *pCryptoInfo);
	UniversalClientSPI_Device_GetPersonalizedData,		//uc_result (*Device_GetPersonalizedData)(uc_buffer_st* pData);
	UniversalClientSPI_Timer_Open,				//uc_result (*Timer_Open)(uc_uint32* pTimerId,uc_timer_info* pTimerInfo);
	UniversalClientSPI_Timer_Close,				//uc_result (*Timer_Close)(uc_uint32 timerId);
	UniversalClientSPI_Timer_Start,				//uc_result (*Timer_Start)(uc_uint32 timerId);
	UniversalClientSPI_Timer_Stop,				//uc_result (*Timer_Stop)(uc_uint32 timerId);
#if (UCCA_MAJOR_VERSION >= 4)
	UniversalClientSPI_Semaphore_WaitTimeout,	//uc_result (*Semaphore_WaitTimeout)(uc_semaphore_handle semaphoreHandle, uc_uint32 milliseconds);
	NULL,	//uc_result (*Device_GetDeviceID)(uc_buffer_st * pData);
	NULL,	//uc_result (*Device_GetIPAddress)(uc_buffer_st * pData);
	NULL,	//uc_result (*Stream_Send)(uc_stream_handle streamHandle, const uc_stream_send_payload *pBytes);
	NULL,	//uc_result (*Device_GetPVRSecurityState)(uc_pvr_security_state * pPVRSecurityState);
	NULL,	//uc_result (*Device_GetChipConfigurationCheck)(uc_chip_configuration_request chipConfigurationRequest, uc_chip_configuration_response *pChipConfigurationResponse);
	NULL,	//uc_result (*Device_SetCSSN)(const uc_buffer_st * pData);
	NULL,	//uc_result (*Device_GetPINCode)(uc_buffer_st * pData);
	NULL,	//uc_result (*Device_SetMulti2Parameter)(uc_stream_handle streamHandle, uc_device_multi2_parameter *pMulti2Parameter);
	NULL,	//uc_result (*IFCP_Communicate)(uc_IFCP_input *pInput, uc_IFCP_output *pOutput);
	NULL,	//uc_result (*IFCP_LoadImage)(uc_IFCP_image *pImage);
	NULL,	//uc_result (*SetExtraTrickModeControl)(uc_stream_handle streamHandle,uc_extra_trick_mode_control *pExtraTrickModeControl);
#endif
};


/**
 * Get the current version of the Universal Client SPI.
 *
 * This function is called when the client is started is called in order to check
 * that the version of the SPI implementation is compatible with the version of the client.
 *
 * This function MUST return #UNIVERSALCLIENTSPI_VERSION. Recompiling an existing SPI implementation
 * will update this version automatically.
 */
uc_uint32 UniversalClientSPI_GetVersion(void)
{
	HxLOG_Print("%s\n", __FUNCTION__ );

	return UNIVERSALCLIENTSPI_VERSION;
}


/**
 * Get the list of implemented methods
 *
 * This method is called when the client is started in order to
 * get the list of SPI methods currently available on the platform.
 *
 * @param [in,out] pImpl On input, this structure is zeroed out.
 *    The implementation MUST populate this structure in order to successfully
 *    integrate with the platform.
 *
 */


void UniversalClientSPI_GetImplementation(uc_spi_implementation_st *pImpl)
{
	HxLOG_Print("in pImpl(0x%X) \n", (HUINT32) pImpl);
	*pImpl = s_stUcasSpiObj;
	return ;
}

/** @} */ /* End of toplevelspi*/


/** @defgroup pvrspi Universal Client PVR SPIs
 *  Universal Client PVR SPIs
 *
 *  Univesal Client will output PVR relevant keys/records via these SPIs.
 *  To access PVR functionality, the application should also invoke \ref pvrapis "Universal Client PVR APIs"
 *  to cooperate with these SPIs.
 *
 *
 *  @{
 */

/**
 * Set PVR session key
 *
 * This function is called to set a session key in response to the following requests:
 * -# Request for a PVR record via ::UniversalClient_ConfigService or ::UniversalClient_ConfigConnection
 * -# Request for a PVR playback via ::UniversalClient_SubmitPvrSkr
 *
 * SPI implementation should use this PVR session key to encrypt the descrambled content before saving it to storage.
 *
 * @param[in] recordId Record Id previously used by the call to ::UniversalClient_ConfigService
 *     or ::UniversalClient_ConfigConnection.Each session is uniquely identified by a record id.
 *
 * @param[in] pPVRSK Buffer containing a PVR session key.
 *     The uc_bytes::bytes member points to a buffer containing the session key, and
 *     the uc_bytes::length member is set to the length of the session key.Session key length currently is 16 bytes long.
 *
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
uc_result UniversalClientSPI_PVR_SetSessionKey(
		   uc_stream_handle streamHandle,
		   const uc_buffer_st *pPVRSessionKey)
{
	HxLOG_Print("林狼 !!!!!!!!!!!\n");
	return UC_ERROR_NOT_IMPLEMENTED;
}

/**
 * Set PVR session key with key infomation to support multi key ladder
 *
 * This is the extension of ::UniversalClientSPI_PVR_SetSessionKey function with additional key protection infomation provided,
 * in order to support differents key ladder.This function is called to set a session key in response to the following requests:
 * -# Request for a PVR record via ::UniversalClient_ConfigService or ::UniversalClient_ConfigConnection
 * -# Request for a PVR playback via ::UniversalClient_SubmitPVRSessionMetadata
 *
 * At a PVR record session, SPI implementation should use this PVR Session Key to encrypt the descrambled content before saving it to storage.
 * At a PVR plackback session, SPI implementation should use this PVR Session Key to decrypt the encrypted content before viewing the content.
 *
 * @param[in] streamHandle The handle of the stream returned by ::UniversalClientSPI_Stream_Open for PVR record.
 * @param[in] pPVRSKInfo Information about the PVRSK, contains CSSK protection and CSSK material.See ::uc_pvrsk_info.
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
uc_result UniversalClientSPI_PVR_SetExtendedSessionKey(
                        uc_stream_handle streamHandle,
                        const uc_pvrsk_info * pPVRSKInfo)
{
	HxLOG_Print("林狼 !!!!!!!!!!!\n");
	return UC_ERROR_NOT_IMPLEMENTED;
}

/** @defgroup copycontrolspi Universal Client Copy Control SPIs
 *  Universal Client Copy Control SPIs
 *
 *  Univesal Client will output copy control information via these SPIs.
 *  To access copy control functionality, the application should also invoke \ref copycontrolapis "Universal Client Copy Control APIs"
 *  to cooperate with these SPIs.
 *
 *
 *  @{
 */

/**
 * Sets the macrovision configuration data
 *
 * This method is called during the initialization of Universal Client, or when the IUC received Macrovision CAM emm.
 *
 * @param[in] mac_config_data macrovsion configuration data structure. this contains MacrovisionVersion and
    Macrovision configuration data.
 *
 * See ::uc_macrovision_config for the detail information needed.
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result  UniversalClientSPI_CopyControl_Macrovision_SetConfig(uc_macrovision_config mac_config_data)
{
	HxLOG_Print("林狼 !!!!!!!!!!!\n");
	return UC_ERROR_NOT_IMPLEMENTED;
}

/**
 * CopyControl method to set the Macrovision mode
 *
 * This method is called when the received stream contained Macrovision mode info or submitted SKY contained Macrovision mode.
 *
 * @param[in] handle Information about the whether the service is a live one or recorded one.
 *            for live one the handle should be streamhandle returned from the stream's ::UniversalClientSPI_Stream_Open method.
 *            for recorded one the handle should be the recordId from ::UniversalClient_SubmitPvrSkr.
 * @param[in] mac_mode The mode of macrovision to be set
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result  UniversalClientSPI_CopyControl_Macrovision_SetMode(uc_stream_handle streamHandle,uc_macrovision_mode mac_mode)
{
	HxLOG_Print("林狼 !!!!!!!!!!!\n" );
	return UC_ERROR_NOT_IMPLEMENTED;
}

/**
 * CopyControl method to set the CCI bits info [cci_info] to client
 *
 * This method is called when the received stream contained the CCI bits or the submitted SKY contained the CCI bits
 *
 * @param[in] handle Information about whether the service is a live one or recorded one.
 *            for live one the handle should be streamhandle returned from ::UniversalClientSPI_Stream_Open method.
 *            for recorded one the handle should be the recordId from ::UniversalClient_SubmitPvrSkr.
 * @param[in] cci_info detail copy control info.
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result  UniversalClientSPI_CopyControl_SetCCI(uc_stream_handle streamHandle, uc_copy_control_info* pCopyControlInfo)
{
	HxLOG_Print("林狼 !!!!!!!!!!!\n" );
	return UC_ERROR_NOT_IMPLEMENTED;
}

/** @} */ /* End of copycontrolspi */


/**
 * Method to get current date time.
 *
 * This SPI is optional currently.
 *
 * This method is called to retrieve second and microsecond of current time info.
 * In Linux-like system, gettimeofday can be used to get the result(but the result needs to be converted to millisecond).
 * In MS Windows-like system, GetTickCount, GetLocalTime and GetSystemTime can be used to get the result.
 *
 * \note Cloaked CA Agent only uses the time info to measure the performance. So the time can be time elapsed since reboot, or
 * it can also be the real day time.
 *
 * @param[out] pCurrentTime To save current time info.
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */

/* return ms */
uc_result UniversalClientSPI_DateTime_GetTimeOfDay(uc_time *pCurrentTime)
{
    unsigned long currentTickCount;
	struct timeval tv;

	gettimeofday(&tv, NULL);

	currentTickCount = (unsigned long)(tv.tv_sec * 1000 + tv.tv_usec / 1000);

    pCurrentTime->second = currentTickCount/1000;
    pCurrentTime->millisecond = currentTickCount%1000;

	return UC_ERROR_SUCCESS;
}

/** @} */ /* End of datetimespi */

/** @defgroup manufacturercryptospi Cloaked CA Agent Crypto SPIs
 *  Cloaked CA Agent Crypto SPIs
 *
 *  Cloaked CA Agent will do the verification step for digital signature via these SPIs.
 *
 *  @{
 */


/**
 * Method to do the verification for digital signature.
 *
 * This SPI is optional currently.
 *
 * This functional modules are implemented by both Cloaked CA Agent and STB manufacturer.
 * Generally speaking, the SPI form is faster than the Cloaked CA Agent's implementation.
 * Cloaked CA Agent will decide which implementation should be used in digital signature process.
 *
 * @param[in] pCryptoInfo send relative info indicating the deciphering algorithm type and the parameter related to it.
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClientSPI_Crypto_Verify(uc_crypto_info *pCryptoInfo)
{
	HxLOG_Print("林狼 !!!!!!!!!!!\n" );
	return UC_ERROR_NOT_IMPLEMENTED;
}

/** @} */ /* End of manufacturercryptospi */

#if defined(CONFIG_MW_CAS_IRDETO_UCAS_VM_VAILDATOR)
uc_uint32 UniversalClientSPI_File_LoadImage(uc_char* imageName, uc_buffer_st* outbuf)
{
	uc_bool success = UC_TRUE;
	FILE *pFile = NULL;
	uc_uint32 mysize = 0;
	uc_uint32 len = 0;

	pFile = fopen (imageName, "rb");
	if (pFile == NULL)
	{
		success = UC_FALSE;
	}

	if(success == UC_TRUE)
	{
		fseek (pFile , 0 , SEEK_END);
		mysize = ftell (pFile);
		rewind (pFile);

		outbuf->bytes = (uc_byte *)OxCAS_Malloc(mysize);
		if (outbuf->bytes == NULL)
		{
			success = UC_FALSE;
		}
	}

	if(success == UC_TRUE)
	{
		len = fread (outbuf->bytes, 1, mysize, pFile);
		if (len != mysize)
		{
			success = UC_FALSE;
		}
		else
	  	{
	  		outbuf->length = len;
	  	}
	}

	/* Error happened in above operation, so perform the clean-up operation */
	if(success == UC_FALSE)
	{
		if (outbuf->bytes != NULL)
		{
			OxCAS_Free(outbuf->bytes);
			outbuf->bytes = NULL;
		}

		outbuf->length = 0;
		mysize = 0;
	}

	if(NULL != pFile)
	{
		fclose(pFile);  /* Close file handle */
	}

	return mysize;
}
#endif

/** @} */ /* End of Cloaked CA Agent SPIs*/


