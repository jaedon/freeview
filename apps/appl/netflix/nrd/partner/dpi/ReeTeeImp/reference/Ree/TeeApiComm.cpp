/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "TeeApiComm.h"

#include <iostream>
#include <cstring>
#include <stdlib.h>
#include <stdio.h>

#include <nrdbase/Log.h>
#include <nrdbase/ScopedMutex.h>
#include <nrddpi/IConnection.h>
#include <nrddpi/IMemoryReference.h>
#include <nrddpi/IMessage.h>
#include <nrddpi/ISession.h>
#include <nrddpi/IValue.h>
#include <nrddpi/MemoryReference.h>
#include <nrddpi/RPCErrorTypes.h>
#include <nrddpi/TeeApiErrorTypes.h>

using namespace netflix;

shared_ptr<TeeApiComm> TeeApiComm::teeApiComm_;
Mutex TeeApiComm::instanceMutex (ZERO_MUTEX, "TeeApiCommMutex");

shared_ptr<TeeApiComm> TeeApiComm::getInstance()
{
    if (0 == teeApiComm_.use_count())
    {
        //lock mutex, ensuring only one instantiation
        ScopedMutex lock(instanceMutex);
        if (0 == teeApiComm_.use_count()) {
            teeApiComm_.reset(new TeeApiComm());
        }
    }
    return teeApiComm_;
}

TeeApiComm::TeeApiComm()
    : mutex_(TEEAPICOMM_MUTEX, "TeeApiCommProcessMsgMutex")
{

    // no asserts in prod code, so make sure these are initialized.
    reqBufSize = rspBufSize = 0;

    reqBufPtr = (uint8_t *) malloc(MAX_BUFFER_SIZE);
    if(reqBufPtr == NULL)
    {
        Log::fatal(TRACE_CRYPTO) << "TeeApiComm::TeeApiComm: Failed to malloc request buffer";
        assert(0);
    }
    else
        reqBufSize = MAX_BUFFER_SIZE;

    rspBufPtr = (uint8_t *) malloc(MAX_BUFFER_SIZE);
    if(rspBufPtr == NULL)
    {
        Log::fatal(TRACE_CRYPTO) << "TeeApiComm::TeeApiComm: Failed to malloc response buffer";
        assert(0);
    }
    else
        rspBufSize = MAX_BUFFER_SIZE;

    nfTeeApiError_t err;
    nfRPCResult_t result;

    err = nfOpenConnection(nfCONTAINER_ID_TEE, &conn);
    CHECK_FOR_ERROR(err, "Failed to open connection.");

    err = nfOpenSession(conn, nfSERVICE_NETFLIX_TASK_GROUP, &session, &result);
    CHECK_FOR_ERROR(err, "Failed to open session.");

}

TeeApiComm::~TeeApiComm()
{
    if (reqBufPtr != NULL)
    {
        free(reqBufPtr);
        reqBufPtr = NULL;
    }
    if (rspBufPtr != NULL)
    {
        free(rspBufPtr);
        rspBufPtr = NULL;
    }

    nfTeeApiError_t err;
    nfRPCResult_t result;

    err = nfCloseSession(session, &result);
    CHECK_FOR_ERROR(err, "Failed to close session.");

    err = nfCloseConnection(conn);
    CHECK_FOR_ERROR(err, "Failed to close connection.");
}

int TeeApiComm::processMessage (nfTeeOperation_t op, DataBuffer &opData,
                                uint32_t &rspCode, DataBuffer &rspData)
{
    //check that opData size is valid (i.e. less than max allowed)
    int opDataLen = opData.size();
    if (opDataLen > (int) MAX_REQ_DATA_SIZE)
    {
        Log::error(TRACE_CRYPTO) << "TeeApiComm::processMessage: request size is " << opDataLen
                                 << " where as max allowed is " << MAX_REQ_DATA_SIZE;
        return -1;
    }

    // NRDP-3240
    if(reqBufPtr == NULL)
    {
        Log::fatal(TRACE_CRYPTO) << "TeeApiComm::TeeApiComm: Failed to malloc request buffer";
        return -1;
    }
    if(rspBufPtr == NULL)
    {
        Log::fatal(TRACE_CRYPTO) << "TeeApiComm::TeeApiComm: Failed to malloc response buffer";
        return -1;
    }

    //lock mutex, ensuring synchronous and serialized communication with TEE
    ScopedMutex lock(mutex_);

    //clear the request and response buffers
    memset(reqBufPtr, '\0', MAX_BUFFER_SIZE);
    memset(rspBufPtr, '\0', MAX_BUFFER_SIZE);

    // add nfTeeApiReqHdr_t at reqBufPtr i.e. op and data length
    nfTeeApiReqHdr_t* reqHdrPtr = (nfTeeApiReqHdr_t*) reqBufPtr;
    reqHdrPtr->Operation = htonl(op);
    reqHdrPtr->OperandLength = htonl(opDataLen);
    // if opData exists, copy it over to reqBufPtr
    if (opDataLen > 0)
    {
        memcpy((reqBufPtr + sizeof(nfTeeApiReqHdr_t)), opData.data(), opDataLen);
    }

    nfTeeApiError_t err;
    nfParameter_t params[2];

    memset(&params[0], 0, sizeof(nfParameter_t));
    params[0].type = nfPARAMETER_MEMORY_REFERENCE_TYPE;
    nfMemoryReference_t *registeredMemRef = (nfMemoryReference_t *)(&(params[0].data));
    registeredMemRef->addr = (void *)reqBufPtr;
    registeredMemRef->size = nfTEE_MIN_REQ_SIZE + opDataLen;
    registeredMemRef->type = nfMEMORY_REFERENCE_READ_ONLY;
    err = nfMemoryReferenceRegister(session, registeredMemRef);
    CHECK_FOR_ERROR(err, "Failed to allocate a memory reference.");

    memset(&params[1], 0, sizeof(nfParameter_t));
    params[1].type = nfPARAMETER_MEMORY_REFERENCE_TYPE;
    registeredMemRef = (nfMemoryReference_t *)(&(params[1].data));
    registeredMemRef->addr = (void *)rspBufPtr;
    registeredMemRef->size = MAX_BUFFER_SIZE;
    registeredMemRef->type = nfMEMORY_REFERENCE_WRITE_ONLY;
    err = nfMemoryReferenceRegister(session, registeredMemRef);
    CHECK_FOR_ERROR(err, "Failed to register a memory reference.");


    uint32_t returnCode = 0;
    err = nfSendMessage(session, op, params, 2, &returnCode);
    CHECK_FOR_ERROR(err, "Failed to send message.");

    err = nfMemoryReferenceRelease((nfMemoryReference_t *)&(params[0].data));
    CHECK_FOR_ERROR(err, "Failed to release registered memory reference for InMsg");

    err = nfMemoryReferenceRelease((nfMemoryReference_t *)&(params[1].data));
    CHECK_FOR_ERROR(err, "Failed to release allocated memory reference for OutMsg");

    //get values of rspCode and rspLen from the response header
    rspCode = ntohl (((nfTeeApiRspHdr_t*) rspBufPtr)->ResponseCode);
    int rspLen = ntohl (((nfTeeApiRspHdr_t*) rspBufPtr)->ResponseLength);

    //copy the response (if it exists) to data buffer rspData
    if(rspLen > 0)
    {
        rspData.clear();
        rspData.append<uint8_t>((rspBufPtr+sizeof(nfTeeApiRspHdr_t)), rspLen);
    }

    return 0;
}

int TeeApiComm::allocateMemory(nfMemoryReference_t *memRefPtr, uint32_t memSize,
                               nfMemoryReferenceReadWriteType_t memType )
{
    nfTeeApiError_t err;
    memRefPtr->size = memSize;
    memRefPtr->type = memType;
    err = nfMemoryReferenceAllocate(session, memRefPtr);
    if (err != nfTAE_SUCCESS)
    {
        printf("Failed to allocate a memory reference.\n");
        return -1;
    }

    return 0;

}

int TeeApiComm::freeMemory (nfMemoryReference_t *memRefPtr)
{
    nfTeeApiError_t err;
    err = nfMemoryReferenceRelease(memRefPtr);
    if (err != nfTAE_SUCCESS)
    {
        printf("Failed to release memory reference.\n");
        return -1;
    }

    return 0;
}

uint32_t TeeApiComm::getReqBufSize()
{
    return reqBufSize - sizeof(nfTeeApiReqHdr_t);
}

uint32_t TeeApiComm::getRspBufSize()
{
    return rspBufSize - sizeof(nfTeeApiRspHdr_t);
}

