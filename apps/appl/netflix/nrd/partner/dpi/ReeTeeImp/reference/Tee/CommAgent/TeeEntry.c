/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

/*
 *      This file implements the TEE entry point(s). Right now, there is exactly
 *      one functions exposed to the outside world:
 *
 *      - nfTeeDispatchMsg(): you communicate with the TEE by passing it a message, and waiting
 *        for the response. This is the function you call to do that.
 */

#include "TeeTypes.h"
#include "CommTypes.h"
#include "CryptoTypes.h"
#include "StorageTypes.h"
#include "MgmtTypes.h"
#include "TeeDrmTypes.h"
#include "MgmtAgent.h"
#include "DrmAgent.h"
#include "StorageAgent.h"
#include "CryptoAgent.h"
#include "TeeDebug.h"
#include "TeeInternalDpi.h"

#include "RPCTypes.h"
#include "RPCUtils.h"
#include "AgentApi.h"

/*
 * XXX - hard-coding NRD client ID for this library TEE implementation; this is
 * defined in the manufacturing secure store, and in StorageAgentInternals.h
 */
static nfTeeClientId_t NrdClientId =
	{0x15,0xe1,0x25,0xe9,0x11,0x00,0xF1,0x99,0x03,0x33,0xd1,0xfd,0x90,0x87,0xbb,0x40};

/*
 * Comm agent needs reference addresses in order to validate pointers in
 * messages. These values should be set during initialization.
 */
// XXX - for now, we are setting these to dummy values for the ref app, but this must be fixed.
static uint8_t *caSharedMemoryBaseAddr = (uint8_t *)0x00000000;
static uint8_t *caSharedMemoryMaxAddr = (uint8_t *) 0xFFFFFFFF;

/*********************************************************************
 * @function caValidateInboundMsgPtr
 *
 * @brief verify that passed pointer is within allowed range for incoming msg
 *
 * @param[in] InMsgPtr - pointer to incoming message
 * @param[in] InMsgLength - length of incoming message
 *
 * @return nfTGR_SUCCESS if okay, nfTGR_INMSG_INVALID otherwise
 *
 * NB: need to look at optimization for this function. This should
 *     probably be inline-able.
 *
 *********************************************************************/
nfTeeResult_t caValidateInboundMsgPtr( uint8_t 	*InMsgPtr,
									   uint32_t	InMsgLength )
{

	/*
	 * We want to ensure the following:
	 *
	 * 	- message pointer is within allowable range
	 * 		- Not in TEE memory
	 * 		- Not overlapping with InMsg
	 * 		- Not in MMIO (or other inappropriate) memory range
	 *
	 * 	- length does not cause message to exceed allowable range
	 *
	 * 	Because this is an x86 reference implementation, we know that
	 * 	InMsgPtr and OutMsgPtr are on the heap. We could add checks here
	 * 	to ensure that this is the case (to illustrate best practices),
	 * 	but this would be an academic exercise.
	 *
	 * 	What you check here will vary, depending on your implementation.
	 * 	The important thing to keep in mind is that the REE may be
	 * 	compromised, so you cannot trust these pointers or lengths. You
	 * 	need to check EVERY TIME to make sure they are "reasonable".
	 */

	if ( ( InMsgPtr > caSharedMemoryBaseAddr )
	  && ( InMsgPtr < caSharedMemoryMaxAddr )
	  && ( InMsgLength < (uint32_t) (caSharedMemoryMaxAddr - InMsgPtr)))
		return nfTGR_SUCCESS;

	// if we get here, something is wrong.
	COMMDBG("%s: invalid ptr/length (%p/%p, %p/%p, %p/%p)\n", __FUNCTION__,
				InMsgPtr, caSharedMemoryBaseAddr, InMsgPtr, caSharedMemoryMaxAddr,
				InMsgPtr + InMsgLength, caSharedMemoryMaxAddr );

	return nfTGR_INMSG_INVALID;
}/* end caValidateInboundMsgPtr */

/*********************************************************************
 * @function caValidateOutboundMsgPtr
 *
 * @brief verify that passed pointer is within allowed range for Outgoing msg
 *
 * @param[in] OutMsgPtr - pointer to incoming message
 * @param[in] OutMsgMax - max length of outgoing message
 *
 * @return nfTGR_SUCCESS if okay, nfTGR_OUTMSG_INVALID otherwise
 *
 * NB: need to look at optimization for this function. This should
 *     probably be inline-able.
 *
 *********************************************************************/
nfTeeResult_t caValidateOutboundMsgPtr( uint8_t 	*OutMsgPtr,
									    uint32_t	OutMsgMax )
{
	/*
	 * We want to ensure the following:
	 *
	 * 	- message pointer is within allowable range
	 * 		- Not in TEE memory
	 * 		- Not overlapping with InMsg
	 * 		- Not in MMIO (or other inappropriate) memory range
	 *
	 * 	- length does not cause message to exceed allowable range
	 *
	 * 	Because this is an x86 reference implementation, we know that
	 * 	InMsgPtr and OutMsgPtr are on the heap. We could add checks here
	 * 	to ensure that this is the case (to illustrate best practices),
	 * 	but this would be an academic exercise.
	 *
	 * 	What you check here will vary, depending on your implementation.
	 * 	The important thing to keep in mind is that the REE may be
	 * 	compromised, so you cannot trust these pointers or lengths. You
	 * 	need to check EVERY TIME to make sure they are "reasonable".
	 */

	if ( ( OutMsgPtr > caSharedMemoryBaseAddr )
	  && ( OutMsgPtr < caSharedMemoryMaxAddr )
	  && ( OutMsgMax < (uint32_t) (caSharedMemoryMaxAddr - OutMsgPtr) ))
		return nfTGR_SUCCESS;

	// if we get here, something is wrong.
	COMMDBG("%s: invalid ptr/length (%p/%p, %p/%p, %p/%p)\n", __FUNCTION__,
				OutMsgPtr, caSharedMemoryBaseAddr, OutMsgPtr, caSharedMemoryMaxAddr,
				OutMsgPtr + OutMsgMax, caSharedMemoryMaxAddr );

	return nfTGR_OUTMSG_INVALID;
}/* end caValidateOutboundMsgPtr */

/*********************************************************************
 * @function nfTeeDispatchSerializedMsg
 *
 * @brief internal TEE message dispatcher/entry point
 *
 * @param[in] InMsgPtr - pointer to incoming message
 * @param[in] InMsgBufLength - actual size of input buffer
 * @param[out] OutMsgPtr - pointer to buffer for response
 * @param[in] OutMsgBufLength - maximum size of response
 *
 * @return void
 *
 *********************************************************************/
static nfTeeResult_t
nfTeeDispatchSerializedMsg( uint8_t 	*InMsgPtr,
							uint32_t 	InMsgBufLength,
							uint8_t 	*OutMsgPtr,
							uint32_t 	OutMsgBufLength )
{
	nfTeeResult_t 		Result = nfTGR_SUCCESS;
    nfTeeOperation_t 	Operation = nfTOB_UNDEFINED;
    uint32_t			OutMsgLength = 0;
    uint8_t 			*ReqPtr = NULL;
    uint32_t 			ReqBufLength = 0;
    uint8_t 			*RspPtr = NULL;
    uint32_t 			RspBufLength = 0;

    /*
     *  This is the TEE library entry point function. We use messages for
     *  communications between the REE and the TEE. The current communications
     *  model is very simple:
     *
     *	- The TEE is passive, and only responds to requests. It never
     *	  initiates action.
     *
     *	- This function requires two buffers, one containing the inbound
     *	  request (the "operation" and "operands"), and one which will
     *	  contain the response. The REE must allocate these buffers.
     *
     *	- This function is also the first line of defense against a
     *	  potentially hostile REE. So, we should do as much argument
     *	  validation as we can in this function, before invoking the
     *	  target trusted application (TA).
     *
     */

    // if OutMsg is invalid, we are finished.
    if ( OutMsgPtr == NULL )
	{
        COMMDBG("%s: OutMsgPtr == NULL\n", __FUNCTION__);
    	return nfTGR_OUTMSG_INVALID;
	}
    else if ( OutMsgBufLength < sizeof(nfTeeApiRspHdr_t) )
    {
        COMMDBG("%s: OutMsgBufLength < sizeof(nfTeeApiRspHdr_t) (%lu|%zu(\n", __FUNCTION__, OutMsgBufLength, sizeof(nfTeeApiRspHdr_t));
    	return nfTGR_OUTMSG_TOO_SHORT;
    }
    else if (( Result = caValidateOutboundMsgPtr( OutMsgPtr, OutMsgBufLength ) ) != nfTGR_SUCCESS )
    {
    	// can't send a response; puke
    	COMMDBG("%s: must have valid outmsg (pointer: %p/length: %d)", __FUNCTION__, OutMsgPtr, OutMsgBufLength );
    	return Result;
    }

    // validate InMsgPtr/length
    if (InMsgBufLength < sizeof(nfTeeApiReqHdr_t))
    {
        COMMDBG("%s: InMsgBufLength < sizeof(nfTeeApiReqHdr_t) (%lu|%zu(\n", __FUNCTION__, InMsgBufLength, sizeof(nfTeeApiReqHdr_t));

        // Store result and length in outmsg
        ((nfTeeApiRspHdr_t *)OutMsgPtr)->ResponseCode = teeHtonl(nfTGR_INMSG_TOO_SHORT);
        ((nfTeeApiRspHdr_t *)OutMsgPtr)->ResponseLength = 0;

        // if we get here, OutMsg contains response, just return
        return nfTGR_INMSG_TOO_SHORT;
    }
    if (InMsgPtr == NULL )
    {
        COMMDBG("%s: InMsgPtr == NULL\n", __FUNCTION__);

        // Store result and length in outmsg
        ((nfTeeApiRspHdr_t *)OutMsgPtr)->ResponseCode = teeHtonl(nfTGR_INMSG_INVALID);
        ((nfTeeApiRspHdr_t *)OutMsgPtr)->ResponseLength = 0;

        // if we get here, OutMsg contains response, just return
        return nfTGR_INMSG_INVALID;

    }

    // Verify that InMsg is in valid shared memory range
    if ( ( ( Result = caValidateInboundMsgPtr( InMsgPtr, InMsgBufLength ) ) != nfTGR_SUCCESS ) )
    {
    	COMMDBG("%s: message buffer(s) failed validation.\n", __FUNCTION__);

		// Store result and length in outmsg
		((nfTeeApiRspHdr_t *)OutMsgPtr)->ResponseCode = teeHtonl(Result);
		((nfTeeApiRspHdr_t *)OutMsgPtr)->ResponseLength = 0;
		return Result;
    }

    // okay, got past first line of obstacles. Now, check operation
    Operation = teeNtohl(((nfTeeApiReqHdr_t *) InMsgPtr)->Operation);

    /*
     * Because we are implementing the TEE as a library (instead of a VM, or
     * TZ, or a HW TEE), we have to deal with library artifacts (like function
     * call behavior, return values, etc.).
     *
     * In typical (non-libary) TEE implementation, we would expect that
     * communications with modules would be asynchronous (we send and receive
     * messages, all message ops are non-blocking), but that's not the case here.
     * So, we have to deal with this.
     *
     * We adopt the following conventions:
     *
     *  - this dispatcher GUARANTEES that InMsgPtr, InMsgBufLength, OutMsgPtr,
     *    OutMsgBuflength, and Operation are all valid and/or reasonable. The
     *    module dispatch function need not repeat basic argument validation.
     *
     *	- dispatch functions MUST return message length and a
     *	  valid/informative ResponseCode.
     *
     */

    // fix up lengths and buffer pointers for agent dispatch functions
    if ( ( ReqBufLength = InMsgBufLength - sizeof(nfTeeApiReqHdr_t) ) > 0 )
    	ReqPtr = InMsgPtr + sizeof(nfTeeApiReqHdr_t);

    if ( ( RspBufLength = OutMsgBufLength - sizeof(nfTeeApiRspHdr_t) ) > 0 )
    	RspPtr = OutMsgPtr + sizeof(nfTeeApiRspHdr_t);

    /*
     * We haven't implemented session management yet, so we use the operation
     * macros to determine which agent to route requests to. When we implement
     * session management, this will change.
     */
    if ( nfTeeIsInitialized() == nfBT_TRUE )
    {
        if ( nfTee_IS_MGMT_OP(Operation) )
        {
            COMMDBG("%s: calling mgmtAgentDispatchMsg()\n", __FUNCTION__ );
            Result = mgmtAgentDispatchMsg( Operation, ReqPtr, ReqBufLength,
            							   RspPtr, RspBufLength,
                                           &OutMsgLength, NrdClientId );

        }
        else if ( nfTEE_IS_CRYPTO_OP(Operation) )
        {
            COMMDBG("%s: calling cryptoAgentDispatchMsg()\n", __FUNCTION__ );
            Result = cryptoAgentDispatchMsg( Operation, ReqPtr, ReqBufLength,
            							   	 RspPtr, RspBufLength,
            							   	 &OutMsgLength, NrdClientId );
        }
        else if ( nfTEE_IS_STORAGE_OP(Operation) )
        {
            COMMDBG("%s: calling storageAgentDispatchMsg()\n", __FUNCTION__ );
        	Result = storageAgentDispatchMsg( Operation, ReqPtr, ReqBufLength,
        									  RspPtr, RspBufLength,
        									  &OutMsgLength, NrdClientId );

        }
        else
        {
            COMMDBG("%s: invalid operation 0x%08x (case TEE initialized)\n", __FUNCTION__, Operation );
            Result = nfTGR_INVALID_OPERATION;
        }

    }
    else
    {
    	// if we are not initialized, the only message allowed is an INIT message
		if ( nfTee_IS_MGMT_OP(Operation) )
		{
			COMMDBG("%s: calling mgmtAgentDispatchMsg()\n", __FUNCTION__ );
			Result = mgmtAgentDispatchMsg( Operation, ReqPtr, ReqBufLength,
				   	 	 	 	 	 	   RspPtr, RspBufLength,
				   	 	 	 	 	 	   &OutMsgLength, NrdClientId );

		}
		else
		{
			// not initialized and not a mgmt message --> this is an error
            COMMDBG("%s: invalid operation 0x%08x (case TEE NOT initialized)\n", __FUNCTION__, Operation );
			Result = nfTMR_NOT_INITIALIZED;
		}
    }

    // Store result and length in outmsg
    ((nfTeeApiRspHdr_t *)OutMsgPtr)->ResponseCode = teeHtonl(Result);
    ((nfTeeApiRspHdr_t *)OutMsgPtr)->ResponseLength = teeHtonl(OutMsgLength);

    // if we get here, OutMsg contains response, just return
    return Result;

}/* end nfTeeDispatchSerializedMsg */

/*********************************************************************
 * @function nfTeeDeserializeRPCParams
 *
 * @brief TEE message deserializer, copies from RPC msg to internal parms array
 *
 * @param[in] payload - buffer containing transport payload
 * @param[in] params - array where you want deserialized params
 * @param[in/out] NumParamsPtr - size of array on entry, number of params on exit
 *
 * @return nfTGR_SUCCESS if successful
 *
 *********************************************************************/
static nfTeeResult_t
nfTeeDeserializeRPCParams( nfRPCTransportPayload_t 	*PayloadPtr,
						   nfAgentApiParameter_t 	*Params,
						   uint32_t 				*NumParamsPtr)
{
    // Return RPC out values
    uint32_t 		paramDescription = PayloadPtr->paramDescription;
    uint32_t 		offset = sizeof(PayloadPtr->code) + sizeof(paramDescription);
    uint32_t 		paramSizeInBytes = sizeof(PayloadPtr->param[0]);
    uint32_t 		i;
    uint8_t			*BytePtr = (uint8_t *) PayloadPtr;
    uint32_t 		ParamCount = 0;
    uint32_t		LoopMax = *NumParamsPtr;
    nfTeeResult_t	Result;

    // deserialize RPC payload into params, count 'em as we go
    MSGDBG("%s: LoopMax is %d\n", __FUNCTION__, LoopMax );

    for ( i = 0; i < LoopMax; i++ )
    {
        nfRPCParameter_t *param = &(PayloadPtr->param[i]);
        uint32_t paramOffset = offset + i * paramSizeInBytes;
        if (nfIS_ITH_RPC_PARAMETER_PASSED_BY_VALUE(i, paramDescription))
        {
            nfRPCValue_t rpcValue;
            nfGetWord32(BytePtr + paramOffset, &rpcValue.a);
            nfGetWord32(BytePtr + paramOffset + sizeof(rpcValue.a), &rpcValue.b);

            nfRPCValue_t *value = (nfRPCValue_t *)(&(Params[i].data));
            value->a = rpcValue.a;
            value->b = rpcValue.b;
            Params[i].Type = nfAAPT_VALUE;
            MSGDBG("%s: extracting value in slot %d (%x/%x)\n", __FUNCTION__, i, value->a, value->b);

            int canRead = nfIS_ITH_RPC_PARAMETER_PASSED_BY_VALUE_AND_CAN_READ(i, paramDescription);
            int canWrite = nfIS_ITH_RPC_PARAMETER_PASSED_BY_VALUE_AND_CAN_WRITE(i, paramDescription);
            if (canRead && canWrite)
            {
                Params[i].Permission = nfREAD_WRITE;
            }
            else if (canWrite)
            {
                Params[i].Permission = nfWRITE;
            }
            else if (canRead)
            {
                Params[i].Permission = nfREAD;
            }
            else
            {
                MSGDBG("%s: Permission not set for param %d passed by value\n", __FUNCTION__, i);
                // A param passsed by value must have a permission description
                return nfTGR_INVALID_PARM0_DESCRIPTION + i;
            } // permissions setting
        }
       else if(nfIS_ITH_RPC_PARAMETER_PASSED_BY_REFERENCE(i, paramDescription))
        {
            nfRPCMemoryReference_t *rpcMemRef = (nfRPCMemoryReference_t *)param;
            nfRPCMemoryReference_t *MemRefPtr = (nfRPCMemoryReference_t *)(&(Params[i].data));

            uint32_t memPtrValue = 0;

            nfGetWord32(BytePtr + paramOffset, &(memPtrValue));
            nfGetWord32(BytePtr + paramOffset + sizeof(memPtrValue), &(rpcMemRef->memSize));

            // make sure pointer is not into TEE memory...
            if ( ( Result = caValidateInboundMsgPtr( (uint8_t *)(uintptr_t) memPtrValue, rpcMemRef->memSize ) ) != nfTGR_SUCCESS )
            {
            	MSGDBG("%s: Invalid pointer (%p/%d)\n", __FUNCTION__, memPtrValue, rpcMemRef->memSize );
            	return Result;
            }

            MemRefPtr->memPtr = (void *)(uintptr_t)memPtrValue;
            MemRefPtr->memSize = rpcMemRef->memSize;
            Params[i].Type = nfAAPT_MEMREF;
            MSGDBG("%s: extracting ptr in slot %d (%p/%d)\n", __FUNCTION__, i, memPtrValue, rpcMemRef->memSize );

            int canRead = nfIS_ITH_RPC_PARAMETER_PASSED_BY_REFERENCE_AND_CAN_READ(i, paramDescription);
            int canWrite = nfIS_ITH_RPC_PARAMETER_PASSED_BY_REFERENCE_AND_CAN_WRITE(i, paramDescription);
            if (canRead && canWrite)
            {
                Params[i].Permission = nfREAD_WRITE;
            }
            else if (canWrite)
            {
                Params[i].Permission = nfWRITE;
            }
            else if (canRead)
            {
                Params[i].Permission = nfREAD;
            }
            else
            {
                MSGDBG("%s: Permission not set for param %d passed by reference\n", __FUNCTION__, i);
                // A param passsed by reference must have a permission description
                return nfTGR_INVALID_PARM0_DESCRIPTION + i;
            } // permissions setting
        }
        else if(nfIS_ITH_RPC_PARAMETER_NRDP_UNUSED(i, paramDescription))
        {
            /* Parameter is of type NONE: nothing more to do. */
            MSGDBG("%s: extracting unused param in slot %d\n", __FUNCTION__, i);

            // Parameters must be packed and subsequent params must be unused.
            uint32_t ctr = 0;
            for (ctr = i + 1; ctr < LoopMax; ctr++)
            {
                if (!nfIS_ITH_RPC_PARAMETER_NRDP_UNUSED(i, paramDescription))
                {
                    MSGDBG("%s: expecting an unused param in slot %d, param description = %x\n", __FUNCTION__, ctr, paramDescription);
                    return nfTGR_INVALID_PARM0_DESCRIPTION + ctr;
                }
            }
            break;
        }
        else
        {
            MSGDBG("%s: extracting invalid param in slot %d, param description = %x\n", __FUNCTION__, i, paramDescription);
            return nfTGR_INVALID_PARM0_DESCRIPTION + i;
        }

        ++ParamCount;

    }

     MSGDBG("%s: setting *NumParamsPtr to %d\n", __FUNCTION__, ParamCount );
    *NumParamsPtr = ParamCount;

    return nfTGR_SUCCESS;

}/* end nfTeeDeserializeRPCParams */

/*********************************************************************
 * @function nfTeeSerializeRPCParams
 *
 * @brief TEE message serializer, copies from internal parms array to RPC msg
 *
 * @param[in] payload - buffer where you want transport payload
 * @param[in] params - array containing deserialized params
 * @param[in/out] NumParams - size of params array
 *
 * @return nfTGR_SUCCESS if successful
 *
 *********************************************************************/
#ifdef PLAYREADY2_5_TEE
static nfTeeResult_t nfTeeSerializeRPCParams( nfRPCTransportPayload_t 	*PayloadPtr,
                         	 	 	 	 	  nfAgentApiParameter_t   	Params[],
                         	 	 	 	 	  uint32_t 					NumParams)
{
    // Return RPC out values
    uint32_t 	paramDescription = PayloadPtr->paramDescription;
    uint32_t 	offset = sizeof(PayloadPtr->code) + sizeof(paramDescription);
    uint32_t 	paramSizeInBytes = sizeof(PayloadPtr->param[0]);
    uint32_t 	i;
    uint8_t	*BytePtr = (uint8_t *) PayloadPtr;

    // deserialize RPC payload into params, count 'em as we go
    MSGDBG("%s: LoopMax is %d\n", __FUNCTION__, NumParams );

    for ( i = 0; i < NumParams; i++ )
    {
        uint32_t paramOffset = offset + i * paramSizeInBytes;
        if (nfIS_ITH_RPC_PARAMETER_PASSED_BY_VALUE(i, paramDescription))
        {
            nfRPCValue_t *value = (nfRPCValue_t *)(&(Params[i].data));
            nfPutWord32(BytePtr + paramOffset, value->a);
            nfPutWord32(BytePtr + paramOffset + sizeof(value->a), value->b);
        }
    }

    return nfTGR_SUCCESS;

}/* end nfTeeSerializeRPCParams */
#endif // #ifdef PLAYREADY2_5_TEE


/*********************************************************************
 * @function nfTeeDispatchRPCMessage
 *
 * @brief TEE message dispatcher/entry point
 *
 * @param[in] headerBuf - buffer containing transport header
 * @param[in] payloadBuf - buffer containing transport payload
 *
 * @return void
 *
 *********************************************************************/
void nfTeeDispatchRPCMessage(uint8_t *headerBuf, uint8_t *payloadBuf)
{
	nfRPCTransportHeader_t 		*hdrPtr = (nfRPCTransportHeader_t *)headerBuf;
    nfRPCTransportPayload_t 	*payloadPtr = (nfRPCTransportPayload_t *)payloadBuf;
    nfTeeResult_t				Result;
    nfAgentApiParameter_t		params[nfTEE_MAX_PARAMS];
    nfTeeOperation_t 			Operation = nfTOB_UNDEFINED;
    uint32_t					ParamCount = nfTEE_MAX_PARAMS;

    /*
     * It is not practical for THIS reference implementation to do memory
     * bounds check for the headerBuf and payloadBuf pointers. The REE owns
     * the memory containing the contents of the transport header and payload.
     * The REE would have to pass the size of the transport header and payload
     * to the TEE. Memory bound checks implemented by the TEE are superficial
     * because the REE cannot be trusted. The REE can lie about the memory
     * size and trick the TEE to reference an incorrect memory address. This
     * is a limitation of THIS reference implementation, which is based on
     * syncronous run-to-completion execution of RPCs between the REE and the
     * TEE.
     *
     * Note that a TrustZone Tee-based and Virtual TEE-based implementations do
     * not suffer from this limitation. In the TrustZone case, the RPC
     * information is passed via registers and not a memory buffer owned by the
     * REE. In the virtual case, the RPC information is passed via shared memory
     * managed by the hypervisero, a trusted entity.
     */

    /*
     * transport payload has operation (code), parameter description,
     * and array of params. We need to do the following:
     * 	- validate transport header (NB: no session/connection mgmt yet)
     * 		- version
     * 		- xid
     * 		- uuid
     *  - extract (deserialize) parameters
     *  - dispatch message based on operation (code)
     *  - store result in payloadBuf (overwrite code)
     *  - return
     */

    // validate transport header
    if ( hdrPtr->version != nfRPC_TRANSPORT_HEADER_VERSION )
    {
    	MSGDBG("%s: header version mismatch...\n", __FUNCTION__);

    	// overwrite "code" with result
    	if ( payloadBuf != NULL )
    		nfPutWord32(payloadBuf, nfTGR_INTERNAL_ERROR);
    	return;
    }

    // dispatch message based on operation
    nfGetWord32(payloadBuf, &Operation);

	// deserialize params
	MSGDBG("%s: calling nfTeeDeserialize with max of %d params\n", __FUNCTION__, ParamCount );
	Result = nfTeeDeserializeRPCParams( payloadPtr, params, &ParamCount);
	MSGDBG("%s: nfTeeDeserializeRPCParams() says there are %d params.\n", __FUNCTION__, ParamCount);

	if ( Result != nfTGR_SUCCESS )
	{
		MSGDBG("%s: nfTeeDeserialize failed.\n", __FUNCTION__ );
        /*  Return the error code to the REE by overwritting the op code
         * with the resulting error code.
         */
        nfPutWord32(payloadBuf, Result);
		return;
	}

	MSGDBG("%s: operation is %08x\n", __FUNCTION__, Operation );
    if ( nfTEE_IS_DRM_OP(Operation) )
	{
        if ( nfTeeIsInitialized() == nfBT_TRUE )
        {
#ifdef PLAYREADY2_5_TEE
		COMMDBG("%s: calling drmAgentDispatchRPCMsg()\n", __FUNCTION__ );
		Result = drmAgentDispatchRPCMsg( Operation, params, ParamCount, NrdClientId );
		COMMDBG("%s: calling nfTeeSerializeRPCParams()\n", __FUNCTION__ );
	        Result = nfTeeSerializeRPCParams( payloadPtr, params, ParamCount);
#else
        COMMDBG("%s: rec'd DRM op when not supposed to be used...\n", __FUNCTION__);
    	Result = nfTGR_OPERATION_NOT_ALLOWED;
#endif
        }
        else
        {
        	COMMDBG("%s: rec'd DRM op when not initialized...\n", __FUNCTION__);
            Result = (nfTeeResult_t) nfTMR_NOT_INITIALIZED;
        }

	}
    else
    {
    	/*
		 * First, validate the params:
		 *
		 * 		param[0] = {nfPARAMTER_VALUE_TYPE, {value.a=defaultInitFlags,value.b=0}
		 * 		param[1] = {nfPARAMETER_MEMORY_REFERENCE_TYPE, {manufSS,size} }
		 */

		if ( ParamCount != nfTEE_SERIALIZEDMSG_REQ_PARAM_COUNT )
		{
			MGMTDBG("%s: Invalid parameter count (%d/%d)\n", __FUNCTION__,
						ParamCount, nfTEE_INIT_REQ_PARAM_COUNT);
			Result = nfTGR_INVALID_PARAM_COUNT;
		}
		else if ( (  params[0].Type != nfAAPT_MEMREF )
			   || (  params[0].data.MemRef.memPtr == NULL )
			   || (  params[0].data.MemRef.memSize == 0 ) )
		{
			MGMTDBG("%s: Invalid params[0] (Type: %d; val: %d/%d)\n", __FUNCTION__,
					 params[0].Type,  params[0].data.Value.a,
					 params[0].data.Value.b );
			Result = nfTGR_INVALID_PARM0;
		}
		else if ( ( params[1].Type != nfAAPT_MEMREF )
			   || ( params[1].data.MemRef.memPtr == NULL )
			   || ( params[1].data.MemRef.memSize == 0 ) )
		{
			MGMTDBG("%s: Invalid params[1] (Type: %d; val: %p/%d)\n", __FUNCTION__,
					params[1].Type, params[1].data.MemRef.memPtr,
					params[1].data.MemRef.memSize );
			Result = nfTGR_INVALID_PARM1;
		}
		else if ( ( caValidateInboundMsgPtr( params[0].data.MemRef.memPtr, params[0].data.MemRef.memSize) != nfTGR_SUCCESS )
			   || ( teeNtohl(((nfTeeApiReqHdr_t *) params[0].data.MemRef.memPtr)->Operation) != Operation ) )
		{
			COMMDBG("%s: operation mismatch (%d/%d)\n", __FUNCTION__,
					teeNtohl(((nfTeeApiReqHdr_t *) params[0].data.MemRef.memPtr)->Operation),
					Operation );
			Result = nfTGR_INVALID_PARM0;
		}
		else
		{
			// call old nfTeeDispatchSerializedMsg with MsgPtr's and sizes
			COMMDBG("%s: calling nfTeeDispatchSerializedMsg()\n", __FUNCTION__);
			Result = nfTeeDispatchSerializedMsg( params[0].data.MemRef.memPtr,
												 params[0].data.MemRef.memSize,
												 params[1].data.MemRef.memPtr,
												 params[1].data.MemRef.memSize );
		}
    }/* end else not a DRM operation */

    // overwrite "code" with result
    nfPutWord32(payloadBuf, Result);

    // we're outta here
    return;

}/* end nfTeeDispatchRPCMsg */

// vim: expandtab tabstop=4 shiftwidth=4
