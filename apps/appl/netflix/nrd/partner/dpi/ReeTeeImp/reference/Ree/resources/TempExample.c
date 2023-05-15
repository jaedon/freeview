/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "IConnection.h"
#include "ISession.h"
#include "IMemoryReference.h"
#include "IValue.h"
#include "IMessage.h"

#include "RPCErrorTypes.h"
#include "TeeApiErrorTypes.h"

#include "TestUtils.h"


int main(void)
{
    nfTeeApiError_t err;
    nfConnection_t conn;
    nfSession_t session;
    nfRPCResult_t result;

    err = nfOpenConnection(nfCONTAINER_ID_TEE, &conn);
    CHECK_FOR_ERROR(err, "Failed to open connection.")

    err = nfOpenSession(conn, nfSERVICE_NETFLIX, &session, &result);
    CHECK_FOR_ERROR(err, "Failed to open session.");

    nfParameter_t params[3];
    nfParameter_t copyParams[3];

    memset(&params[0], 0, sizeof(nfParameter_t));
    params[0].type = nfPARAMETER_MEMORY_REFERENCE_TYPE;
    nfMemoryReference_t *allocatedMemRef = (nfMemoryReference_t *)(&(params[0].data));
    allocatedMemRef->size = 32;
    allocatedMemRef->type = nfMEMORY_REFERENCE_READ_ONLY;
    err = nfMemoryReferenceAllocate(session, allocatedMemRef);
    CHECK_FOR_ERROR(err, "Failed to allocate a memory reference.");

    memset(&params[1], 0, sizeof(nfParameter_t));
    params[1].type = nfPARAMETER_MEMORY_REFERENCE_TYPE;
    uint8_t buf[32] = { 0x0 };
    nfMemoryReference_t *registeredMemRef = (nfMemoryReference_t *)(&(params[1].data));
    registeredMemRef->addr = (void *)buf;
    registeredMemRef->size = 32;
    registeredMemRef->type = nfMEMORY_REFERENCE_WRITE_ONLY;
    err = nfMemoryReferenceRegister(session, registeredMemRef);
    CHECK_FOR_ERROR(err, "Failed to register a memory reference.");

    memset(&params[2], 0, sizeof(nfParameter_t));
    params[2].type = nfPARAMETER_VALUE_TYPE;
    nfValue_t *value = (nfValue_t *)(&(params[2].data));
    value->a = 1;
    value->b = 2;
    value->type = nfVALUE_READ_WRITE;


    uint32_t ctr = 0;
    for (ctr = 0; ctr < sizeof(params) / sizeof(*params); ctr++) {
        copyParam(&params[ctr], &copyParams[ctr]);
    }

    uint32_t opCode = 0;
    uint32_t returnCode = 0;
    err = nfSendMessage(session, opCode, params, 3, &returnCode);
    CHECK_FOR_ERROR(err, "Failed to send message.");

    err = nfMemoryReferenceRelease(session, registeredMemRef);
    CHECK_FOR_ERROR(err, "Failed to release registered memory reference");

    err = nfMemoryReferenceRelease(session, allocatedMemRef);
    CHECK_FOR_ERROR(err, "Failed to release allocated memory reference");

    err = nfCloseSession(session, &result);
    CHECK_FOR_ERROR(err, "Failed to close session.");

    err = nfCloseConnection(conn);
    CHECK_FOR_ERROR(err, "Failed to close connection.");

    printf("SUCCESS\n");
    return 0;
}


