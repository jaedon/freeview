/*
 * teeMsgs.c
 *
 *  Created on: May 3, 2014
 *      Author: scott
 */
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include "CommonIncludes.h"
#include "StorageTypes.h"
#include "DebugTypes.h"
#include "TeeDebug.h"
#include "RPCUtils.h"
#include "TeeInternalDpi.h"
#include "AgentApi.h"
#include "StorageAgent.h"

#define nfTEE_MSG_WORKBUF_SIZE 1024
static uint8_t WorkBuf[nfTEE_MSG_WORKBUF_SIZE];

/*********************************************************************
 * @function teeStorageSimpleRead
 *
 * @brief do simple read from storage agent
 *
 * @param[in] RequestedKey - client key (db identifier) for requested data
 * @param[out] DataPtr - where you want the data
 * @param[in] MaxDataLength - maximum length of data
 * @param[out] DataLengthPtr - where the actual length is stored
 * @param[in] ClientId - ID for REE client who made request
 *
 * @returns 0 if successful, -1 otherwise
 *
 *********************************************************************/
nfTeeResult_t teeStorageSimpleRead( nfSecureStoreClientKey_t	RequestedKey,
									uint8_t 				 	*DataPtr,
									uint32_t					MaxDataLength,
									uint32_t					*DataLengthPtr,
									nfTeeClientId_t				ClientId )
{
	nfAgentApiParameter_t 	Params[nfTEE_SIMPLE_READ_REQ_PARAM_COUNT];
	nfTeeResult_t			Result;
	nfTeeDataBuffer_t		*DataBufPtr;

	MSGDBG("%s: called with %08x, %p, %d, %p, %p\n", __FUNCTION__, RequestedKey,
			DataPtr, MaxDataLength, DataLengthPtr, ClientId );

	/*
	 * First,make sure this will fit in our local WorkBuf...
	 */
	if ( MaxDataLength > (nfTEE_MSG_WORKBUF_SIZE - nfTEE_MIN_DATABUFFER_SIZE) )
	{
		MSGDBG("%s: requested data too large (%d/%d)\n", __FUNCTION__,
				MaxDataLength, (nfTEE_MSG_WORKBUF_SIZE - nfTEE_MIN_DATABUFFER_SIZE));
		return nfTGR_INTERNAL_ERROR;
	}
	/*
	 * now, build the params structure:
	 *
     * 	Params[0] = {nfPARAMETER_VALUE_TYPE, {RequestedKey, 0} }
     * 	Params[1] = {nfPARAMETER_MEMORY_REFERENCE_TYPE, {DstDataMemRef} }
	 */

	Params[0].Type = nfAAPT_VALUE;
	Params[0].data.Value.a = RequestedKey;
	Params[0].data.Value.b = 0;

	Params[1].Type = nfAAPT_MEMREF;
	Params[1].data.MemRef.memPtr = WorkBuf;
	Params[1].data.MemRef.memSize = MaxDataLength + nfTEE_MIN_DATABUFFER_SIZE;

	Result = storageAgentDispatchMsg( nfTO_STORAGE_OP_SIMPLE_READ, Params,
										nfTEE_SIMPLE_READ_REQ_PARAM_COUNT, ClientId );

	MSGDBG("%s: storageAgentDispatchMsg returned %08x\n", __FUNCTION__, Result );

	if ( Result == nfTGR_SUCCESS )
	{
		 DataBufPtr = (nfTeeDataBuffer_t *) WorkBuf;

		(void) memcpy( DataPtr, DataBufPtr->Data, teeNtohl(DataBufPtr->DataLength) );
		*DataLengthPtr = (uint32_t)teeNtohl(DataBufPtr->DataLength);
	}

	return Result;

}/* end teeStorageSimpleRead */
