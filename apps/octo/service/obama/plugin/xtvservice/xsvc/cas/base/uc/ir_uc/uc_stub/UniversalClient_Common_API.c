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
#include "UniversalClient_Common_API.h"

/**
 * Initialize the Cloaked CA Agent.
 *
 * This function initializes the Cloaked CA Agent for use with an application.
 *
 * An application must call this function before calling any other Cloaked CA Agent API methods.
 * This function is responsible for creating any internal data structures, threads, global data, and other resources.
 *
 * Calling this function more than once will cause the error ::UC_ERROR_ALREADY_STARTED to be returned.
 *
 * When this function is called, ::UniversalClientSPI_GetVersion is called to check the version of
 * the SPI implementation being used. If the version is not compatible, ::UC_ERROR_INCOMPATIBLE_SPI_VERSION
 * is returned.
 *
 * After this, ::UniversalClientSPI_GetImplementation is called to fill a structure with
 * function pointers to the implementation of SPI methods. This structure is checked
 * to make sure that the minimum required SPI methods are present. If not, ::UC_ERROR_MISSING_SPI_METHOD
 * is returned.
 *
 * @param[in] MessageProc Callback function to be used for notifying the application of
 *     global events. For example, ::UC_GLOBAL_RAW_IRD_MESSAGE is used to notify IRD messages(text, mail, etc) to the application.
 *     For more details on IRD messages, please refer to 705410_CCP_IRD_Messages,_a.k.a._Decoder_messages.doc provided by Irdeto.
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *      ::UC_ERROR_ALREADY_STARTED
 * @retval
 *      ::UC_ERROR_MISSING_SPI_METHOD
 * @retval
 *      ::UC_ERROR_INCOMPATIBLE_SPI_VERSION
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result  UniversalClient_StartCaClient(uc_global_message_proc MessageProc)
{
	return UC_ERROR_SUCCESS;
}


/**
 * Shut down the Cloaked CA Agent.
 *
 * This function shuts down the Cloaked CA Agent, freeing all internal data structures, threads, global data, and other resources.
 * An application that has called ::UniversalClient_Stop must call this function before exiting to ensure proper cleanup.
 *
 * This function will close all the services that are sill running.
 *
 * Calling this function more than once will cause the error ::UC_ERROR_ALREADY_STOPPED to be returned.
 *
 * After this function returns, all internal threads are terminated, and it is guaranteed
 * that any \a MessageProc callback function passed to ::UniversalClient_StartCaClient will no longer
 * be called.
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_ALREADY_STOPPED
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result  UniversalClient_StopCaClient(void)
{
	return UC_ERROR_SUCCESS;
}

/**
 * Open a new service
 *
 * A 'service' is a collection of connections associated with each of the ECM
 * streams OR EMM streams(In DVB, referenced by a DVB PMT or DVB CAT).
 *
 * This function sets up the service handle and informational callbacks. After opening the service,
 * the application should call ::UniversalClient_DVB_NotifyPMT or ::UniversalClient_DVB_NotifyCAT in order
 * to begin processing ECMs or EMMs on those streams.
 *
 * When a service is opened for special purpose, such as PPV VOD or Push VOD, the service handle can't be used by normal live descrambling service.
 * In other words, the service handle can't be reused among different kinds of services. If the service type is unchanged, the service handle can
 * be reused. Suppose there are two services running in parallel, one is VOD service and the other is live descrambling service, the suggested
 * behaviors are as below: \n
 *   1. For VOD service, open one service and get the service handle A, then use service handle A all the time for VOD service.\n
 *   2. For live descrambling service, open a second service and get service handle B, then use service handle B all the time for live descrambling service.
 * At channel switch, the application can just notify the new PMT to service handle B.\n
 *
 * Mixing-up service handles among different kinds of services may cause unexpected behavior inside Cloaked CA Agent.
 *
 * Service status can be retrieved via ::UniversalClient_GetServiceStatus once it has been opened.
 * Some advanced features can be achieved via ::UniversalClient_ConfigService.
 * A service can be closed via ::UniversalClient_CloseService.
 *
 * Generally, Digital TV Application can open one descramble service and a EMM service at startup,
 * then notify the whole PMT or CAT to this service via ::UniversalClient_DVB_NotifyPMT or ::UniversalClient_DVB_NotifyCAT.
 *
 * @param[in] pServiceContext Parameter for this service.
 *     This parameter will be passed to ::UniversalClientSPI_Stream_Open, so that Stream SPI Implementation may know the operation source.
 *     In case of multiple tunners, this parameter can be used to indicate tunner info.
 *     For those devices that integrated SoftCell3 before,this parameter can be used to carry resource ID.
 * @param[in] messageProc Callback function to receive notification of events related to
 *     the service. Whenever the status of a service is changed,  this callback function will be called
 *     with message type #UC_SERVICE_STATUS.
 * @param[in] pMessageProcData Callback-specific value to pass to the function specified
 *     by \a messageProc when events related to the service are sent.
 * @param[out] pServiceHandle Receives a handle to the newly opened service. When the service is
 *     no longer needed, the caller should close it using ::UniversalClient_CloseService.
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClient_OpenService(
    void *pServiceContext,
    uc_service_message_proc messageProc,
    void *pMessageProcData,
    uc_service_handle *pServiceHandle)
{
	return UC_ERROR_SUCCESS;
}

/**
 * Configure a service
 *
 * An application can call this method to send a list of parameters to the service.
 *
 * This function will make a copy of the TLV parameters, post it to the internal task of Cloaked CA Agent,
 * and then return.
 *
 * This function is used to achieve some advanced features like PVR. No need to invoke this function
 * if relevant features are not used.
 *
 *     \note
 *     Refer to \ref basicpvr_record "Basic PVR Record Flow" and \ref pvrrecord "PVR Record"
 *     to get an overview on how to config a PVR service.
 *
 * @param[in] serviceHandle Service handle previously opened by a call to ::UniversalClient_OpenService.
 *
 * @param[in] TLVLength Must be set to the number of the \a pTLV member.
 *
 * @param[in] pTLV Buffer containing a list of parameters in TLV format.
 *     Please refer to \ref servicetlvtags "Cloaked CA Agent TLV Tags for Service"
 *     for a list of TVL tags and its values.
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *    ::UC_ERROR_NULL_PARAM
  * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
*/
uc_result UniversalClient_ConfigService(
    uc_service_handle serviceHandle,
    uc_uint32 TLVLength,
    const void *pTLV)
{
	return UC_ERROR_SUCCESS;
}

/**
 * Get the status of a service.
 *
 * This function retrieves the service status.
 *
 * The method is similar with UniversalClient_GetConvergentServiceStatus.
 * when the client is configured to support Unified Client, use ::UniversalClient_GetConvergentServiceStatus,
 * otherwise use ::UniversalClient_GetServiceStatus.
 *
 * Once a service is opened, Digital TV Application can query the status of the service
 * via this function.
 *
 * Refer to \ref descramblingservicestatus_dvb "Descramblig Service Status Transition" to get an
 * overview on the service status transition.
 *
 * @param[in] serviceHandle Service handle previously opened by a call to ::UniversalClient_OpenService.
 * @param[out] serviceStatus CA client will fill this input buffer with service status string.
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClient_GetServiceStatus(
    uc_service_handle serviceHandle,
    uc_byte serviceStatus[UC_CONNECTION_STATUS_STRING_SIZE])
{
	return UC_ERROR_SUCCESS;
}

/**
 * Close a service handle
 *
 * This function closes a service handle previously opened by a call to
 * ::UniversalClient_OpenService.
 *
 * @param[in,out] pServiceHandle On input, points to the service handle to be closed.
 *     When the function returns, this will be set to INVALID_SERVICE_HANDLE.
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *    ::UC_ERROR_INVALID_HANDLE
 * @retval
 *    ::UC_ERROR_WRONG_HANDLE_TYPE
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClient_CloseService(
    uc_service_handle *pServiceHandle)
{
	return UC_ERROR_SUCCESS;
}


/**
 * Configure client
 *
 * An application can call this method to configure parameters to client for some special requirement,
 * for example, the application can set the client's priority through this method.
 *
 * @param[in] pConfigInfo Structure containing configuration parameters.
 * When filling in the parameter pConfigInfo, the content of pointer pConfigData must be filled according to the parameter configType.
 * For instance, if application wants to enable auto switching service, the value of configType must be set to ::UC_CONFIG_TYPE_FOR_UNIFIED_CLIENT,
 * the pConfigInfo must point to a ::uc_unified_client_config parameter, set the value of isAutoClientSwitching to ::UC_TRUE,
 * and the rest of the members in ::uc_unified_client_config must also be set to the valid value.
 *
 * \note After calling this method to configure client's priority or auto switching service, the newly setting will not take effect immediately
 * until the user changes the channel. For more details for integration code, please refer to \ref configclient "Config Client".
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
  * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
*/
uc_result UniversalClient_ConfigClient(
    uc_ca_client_config* pConfigInfo)
{
	return UC_ERROR_SUCCESS;
}


/**
 * Obtain the status of all ECM/EMM streams belonged to the service.
 *
 * This function can be used to obtain a list of ECM/EMM stream status belonged to the service.
 * The service can be an EMM service or descramble service.
 *
 * \note The information from this method must be displayed in a GUI dialog.
 *
 * @param[in] serviceHandle Service handle previously opened by a call to ::UniversalClient_OpenService.
 * @param[out] pStreamCount If the function succeeds, this parameter receives the number of
 *     elements returned in the ppStreamStatusList array.
 * @param[out] ppStreamStatusList If the function succeeds, this parameter receives a pointer to a buffer containing a
 *     array of ::uc_service_stream_status_st structures. Cloaked CA Agent will allocate the memory, and if the client device application
 *     no longer needs the information in the array, it must call ::UniversalClient_FreeStreamStatus to free the buffer.
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClient_GetStreamStatus(
    uc_service_handle serviceHandle,
    uc_uint32 *pStreamCount,
    uc_service_stream_status_st **ppStreamStatusList)
{
	return UC_ERROR_SUCCESS;
}

/**
 * Free buffer allocated in ::UniversalClient_GetStreamStatus.
 *
 * This function will free the memory allocated by a previous call to ::UniversalClient_GetStreamStatus.
 *
 * @param[in,out] ppStreamStatusList On input, this is the pointer used in the previous call to ::UniversalClient_GetStreamStatus.
 *       On output, the function sets this to NULL.
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClient_FreeStreamStatus(uc_service_stream_status_st **ppStreamStatusList)
{
	return UC_ERROR_SUCCESS;
}


/** @} */ /* End of groups */

/** @defgroup pvrapis Cloaked CA Agent PVR APIs
 *  Cloaked CA Agent PVR APIs
 *
 *  These API methods provide the necessary functionality for \ref glossary_pvr "PVR".
 *
 *  Irdeto provides CA based solution on content protection and management for \ref glossary_pvr "PVR".
 *  In this solution, the content is first descrambled, before re-scrambling
 *  with the same or different algorithm to disk. Playback is achieved by retrieving
 *  the session information to create the keys to decrypt the file on disk.
 *
 *  In order to use PVR, \ref pvrspi "Cloaked CA Agent PVR SPIs" must be implemented.
 *
 *  Refer to \ref basicpvr_record "Basic PVR Record Flow" to get an
 *  overview on how to record a service.
 *
 *  Refer to \ref basicpvr_playback "Basic PVR Playback Flow" to get an
 *  overview on how to playback a service.
 *

 *  On recording, the application shall configure a service or connection to Cloaked CA Agent via ::UniversalClient_ConfigService or :: UniversalClient_ConfigConnection.
 *  The status of record request will be notified to the application via service message uc_service_message_type::UC_SERVICE_PVR_RECORD_STATUS_REPLY.
 *  If the client is entitled to record the content, at least one PVR Session Metadata will be sent to the application via service message uc_service_message_type::UC_SERVICE_PVR_SESSION_METADATA_REPLY,
 *  and a PVR Session Key will be sent to SPI ::UniversalClientSPI_PVR_SetSessionKey. The application should use the session key to encrypt the descrambled content. When the record finishs, the application
 *  should call ::UniversalClient_ConfigService or ::UniversalClient_ConfigConnection to stop the recording.
 *
 *
 *  On playback, the application shall open a service for playback and send the PVR Session Metadata to this service. Cloaked CA Agent will parse the metadata. Then If the client is entitled to playback the content,
 *  a PVR Session Key will be sent to SPI ::UniversalClientSPI_PVR_SetSessionKey. The application should use the session key to decrypt the encrypted content. The status of playback request will be notified to
 *  the application via service message uc_service_message_type::UC_SERVICE_PVR_PLAYBACK_STATUS_REPLY. If there are more than one PVR Session Metadata associated with the content, the application should submit
 *  these data in time and order that match the time and order when these data are generated. For each submitted PVR Session Metadata there will be a service message to tell application the status. The application
 *  shall call ::UniversalClient_CloseService to stop playback.
 *
 *  \note Cloaked CA Agent only provides \ref glossary_pvr "PVR" feature for platform with secure chipset support.
 *
 *  @{
 */

/**
 * Submit a PVR Session Metadata.
 *
 * An application can call this method to submit PVR Session Metadata to Cloaked CA Agent.
 * A PVR Session Metadata is generated by Cloaked CA Agent in response to a PVR record request via
 * ::UniversalClient_ConfigService or ::UniversalClient_ConfigConnection with a tag ::UC_TLV_TAG_FOR_PVR_RECORD.
 *
 * When the application wants to playback a recorded content, it should open a service and submit the PVR Session Metadata
 * to the service.
 *
 * Cloaked CA Agent will parse the PVR Session Metadata and check the entitlement. If the client is entitled to
 * playback the content, the session key for the content will be sent out via ::UniversalClientSPI_PVR_SetSessionKey.
 *
 * Cloaked CA Agent replies the result of processing the PVR Session Metadata via uc_service_message_type::UC_SERVICE_PVR_PLAYBACK_STATUS_REPLY.
 *
 * @param[in] playbackServiceHandle service handle of the playback service.
 *
 * @param[in] pPVRSessionMetadata Buffer containing the PVR Session Metadata.
 *     The uc_buffer_st::bytes member must point to a buffer containing the PVR Session Metadata, and
 *     the uc_buffer_st::length must be set to the number of bytes in the buffer.
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClient_SubmitPVRSessionMetadata(
    uc_service_handle playbackServiceHandle,
    const uc_buffer_st *pPVRSessionMetadata )
{
	return UC_ERROR_SUCCESS;
}

/** @} */ /* End of pvrapis groups */

/** @} */ /* End of Univeral Client APIs */


/** @defgroup convergentclient_api Unified Client Specific APIs

 *  Unified Client APIs
 *
 *  These API methods provide the necessary interfaces for the Unified Client device. When
 *  Unified Client is integrated, the following APIs shall be used in replace of corresponding
 *  Universal Client APIs that are only for Cloaked CA Agent. However, not all APIs have its Unified
 *  Client specific version. Some of them can be used without caring whether Cloaked CA Agent or Unified Client is integrated.
 *  , such as ::UniversalClient_OpenService.
 *
 *  @{
 */


/**
 * Get the status of a service.
 *
 * This function retrieves the service status.
 *
 * The method is similar with UniversalClient_GetServiceStatus.
 * When Unified Client is supported, use ::UniversalClient_GetConvergentServiceStatus,
 * otherwise use ::UniversalClient_GetServiceStatus.
 *
 * Once a service is opened, Digital TV Application can query the status of the service
 * via this function.
 *
 * Refer to \ref descramblingservicestatus_dvb "Descramblig Service Status Transition" to get an
 * overview on the service status transition.
 *
 * @param[in] serviceHandle Service handle previously opened by a call to ::UniversalClient_OpenService.
 * @param[out] pServiceStatusInfo Unified Client will fill this service status infomation.
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClient_GetConvergentServiceStatus(
    uc_service_handle serviceHandle,
    uc_convergent_client_service_status_st* pServiceStatusInfo)
{
	return UC_ERROR_SUCCESS;
}


/** @} */ /* End of Unified Client Common APIs */


