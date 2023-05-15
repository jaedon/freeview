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
#include "UniversalClient_DVB_API.h"

/**
 * Updates a service with a new PMT associated with a program.
 *
 * This function updates a service handle that was previously opened using ::UniversalClient_OpenService
 * by passing it a new PMT.  A single service handle may only be associated with one PMT. Multiple
 * service handles may be opened to deal with multiple programs at once.
 *
 * If the PMT has not changed since the service was updated, this function
 * does nothing. If it is new or has changed, it propagates the changes through the system,
 * which may result in changes to active components for stream instances,
 * and the opening and closing of streams.
 *
 * When an application tunes to a DVB program, it should pass the PMT to this function
 * to listen for ECMs from all ECM streams in the PMT, process them, and send control
 * words back to the streams.
 *
 * This function performs the following:
 *
 * - The PMT is parsed to obtain a list of ECM stream PIDs, and for each ECM stream,
 *   the PIDs of the elementary streams that are scrambled by the ECMs.
 * - The streams in the list is compared against the streams in a previous call to this function.
 * - Each of the new streams in the list is opened (see ::UniversalClientSPI_Stream_Open),
 *   and the client processing thread begins communicating with each stream instance.
 * - Each of the streams removed from the list are closed
 * - Any streams associated with ECM PIDs whose elementary stream PIDs have changed will result
 *   in components on those streams changing (See ::UniversalClientSPI_Stream_AddComponent
 *   and ::UniversalClientSPI_Stream_RemoveComponent).
 *
 * @param[in] serviceHandle Service handle previously opened by a call to ::UniversalClient_OpenService.
 * @param[in] pPmt Buffer containing the raw contents of the PMT table associated with a
 *     program. The uc_buffer_st::bytes member must point to a buffer containing the PMT, and
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
uc_result UniversalClient_DVB_NotifyPMT(
    uc_service_handle serviceHandle,
    const uc_buffer_st *pPmt)
{
	return UC_ERROR_SUCCESS;
}

/**
 * Updates a service with a new CAT associated with a transport
 *
 * This function updates a service handle that was previously opened using ::UniversalClient_OpenService
 * by passing it a new CAT.
 * Typically, there is one CAT that refers to one EMM stream associated with a transport.
 * However, over time, the CAT may change.
 *
 * A single service handle may only be associated with one CAT.
 *
 * If the CAT has not changed since the service was updated, this function
 * does nothing. If it is new or has changed, it propagates the changes through the system, which may result in
 * the closing and opening of streams.
 *
 * This function performs the following:
 *
 * - The CAT is parsed to obtain the EMM stream PID.
 * - If the PID is different from the previous CAT passed to this service, the previous stream
 *   will be closed and a new stream opened. (See ::UniversalClientSPI_Stream_Close and ::UniversalClientSPI_Stream_Open)
 * - A service handle is allocated.
 * - A stream is opened for the EMM PID, and associated with the service handle.
 *
 * @param[in] serviceHandle Service handle previously opened by a call to ::UniversalClient_OpenService.
 * @param[in] pCat Buffer containing the raw contents of the CAT table associated with a
 *     transport. The uc_buffer_st::bytes member must point to a buffer containing the CAT, and
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
uc_result UniversalClient_DVB_NotifyCAT(
    uc_service_handle serviceHandle,
    const uc_buffer_st *pCat)
{
	return UC_ERROR_SUCCESS;
}

/** @} */ /* End of groups */

/**
 * @page stream_interface_call_sequence Stream Interface Call Sequence
 *
 * The general sequence in which the methods of the stream interface methods
 * defined in \ref streamspi "Cloaked CA Agent Stream SPIs"
 * is called by the client during normal operation is provided in this section. Integrators
 * should be aware of this sequence and plan accordingly when implementing the stream interface methods.
 * ECM streams:
 * - Application opens a service using ::UniversalClient_OpenService
 * - Application notifies service of a new PMT of a program to be decrypted by passing it to ::UniversalClient_DVB_NotifyPMT.
 * - ::UniversalClientSPI_Stream_Open is called from the same thread to create a new stream instance, for each of the
 *   ECM PIDs in the PMT.
 * - ::UniversalClientSPI_Stream_Connect is called from the client processing thread to associate each stream
 *   with a connection.
 * - ::UniversalClientSPI_Stream_AddComponent is called for each elementary stream PID associated with the ECM PID.
 * - ::UniversalClientSPI_Stream_OpenFilter and ::UniversalClientSPI_Stream_SetFilter is called from the client processing
 *   thread to set up filters on the stream.
 * - ::UniversalClientSPI_Stream_Start is called, and the stream implementation begins listening for sections that match the filter.
 * - When sections arrive on a stream, stream implementation calls ::uc_notify_callback.
 * - ::UniversalClientSPI_Stream_SetFilter is called from the client processing thread to update the filter.
 *   This gives the stream implementation an
 *   opportunity to filter out repeated ECMs. This process is repeated whenever a new non-duplicate
 *   ECM arrives.
 * - ::UniversalClientSPI_Stream_SetDescramblingKey is called from the client processing thread to notify the stream of a successfully processed descrambling key.
 * - Eventually, the application closes the service using ::UniversalClient_CloseService.
 * - ::UniversalClientSPI_Stream_Stop is called to stop all filters.
 * - ::UniversalClientSPI_Stream_CloseFilter is called from the client processing thread to clean up any existing filters.
 * - ::UniversalClientSPI_Stream_RemoveComponent is called for each elementary stream PID associated with the ECM PID.
 * - ::UniversalClientSPI_Stream_Disconnect is called from the client processing thread to disassociate the stream from the connection.
 * - ::UniversalClient_CloseService waits for client processing thread to completely disconnect.
 * - ::UniversalClientSPI_Stream_Close is called from the same thread as ::UniversalClient_CloseService to free the stream instance.
 *
 * EMM streams:
 * - Application opens a service using ::UniversalClient_OpenService
 * - Application notifies service of a new CAT by passing it to ::UniversalClient_DVB_NotifyCAT.
 * - ::UniversalClientSPI_Stream_Open is called from the same thread to create a new stream instance for the EMM PID extracted from the CAT.
 * - ::UniversalClientSPI_Stream_Connect is called from the client processing thread to associate the stream
 *   with a connection.
 * - ::UniversalClientSPI_Stream_OpenFilter and ::UniversalClientSPI_Stream_SetFilter is called from the client processing thread to set up filters on the stream.
 * - ::UniversalClientSPI_Stream_Start is called, and the stream implementation begins listening for sections that match the filter.
 * - When sections arrive, stream implementation calls ::uc_notify_callback.
 * - If device information changes as a result of the EMM, ::UniversalClientSPI_Stream_SetFilter is called
 *   to update the filter. This gives the implementation an opportunity to continue receiving EMMs addressed to
 *   it when device information changes.
 * - Eventually, the application closes the service using ::UniversalClient_CloseService.
 * - ::UniversalClientSPI_Stream_Stop is called to stop all filters.
 * - ::UniversalClientSPI_Stream_CloseFilter is called from the client processing thread to clean up any existing filters.
 * - ::UniversalClientSPI_Stream_Disconnect is called from the client processing thread to disassociate the stream from the connection.
 * - ::UniversalClient_CloseService waits for client processing thread to completely disconnect.
 * - ::UniversalClientSPI_Stream_Close is called from the same thread as ::UniversalClient_CloseService to free the stream instance.
 */

