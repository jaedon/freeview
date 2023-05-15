/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "TeeApiMgmt.h"
#include <nrdbase/Log.h>

using namespace netflix;

TeeApiMgmt::TeeApiMgmt(uint32_t defaultInitFlags)
{
    this->defaultInitFlags = defaultInitFlags;
    teeApiComm = TeeApiComm::getInstance();
}

TeeApiMgmt::~TeeApiMgmt()
{
    teeApiComm.reset();
}

netflix::NFErr TeeApiMgmt::initTee (const DataBuffer &manufSS)
{
    if (!teeApiComm)
    {
        Log::fatal(TRACE_CRYPTO) << "TeeApiMgmt::initTee: teeApiComm instance is NULL";
        return NFErr_Uninitialized;
    }

    /*
     * create DataBuffer for nfTeeInitRequest_t
     * typedef struct
     * {
     * 	uint32_t 	DebugFlags;
     * 	uint32_t    ManufSSLength;
     *  uint8_t     ManufSS[];
     * } nfTeeInitRequest_t
     */

    DataBuffer initRequest;
    initRequest.append<uint32_t>(htonl(defaultInitFlags));
    initRequest.append<uint32_t> (htonl(manufSS.size()));
    initRequest.append(manufSS);

    // process the message
    NTRACE(TRACE_CRYPTO, "Initializing TEE");
    uint32_t rspCode;
    DataBuffer rspData;
    teeApiComm->processMessage(nfTO_MGMT_OP_INIT_TEE, initRequest, rspCode, rspData);

    // handle error case
    if (rspCode != nfTGR_SUCCESS)
    {
        Log::fatal(TRACE_CRYPTO) << "TeeApiMgmt::initTee: Cannot init TEE, TEE returned error " << getTeeResultStr(rspCode);
        return translateTeeError((nfTeeResult_t)rspCode);
    }

    // response to init has no data, so simply return success
    return NFErr_OK;
}

netflix::NFErr TeeApiMgmt::shutdownTee ()
{
    if (!teeApiComm)
    {
        Log::fatal(TRACE_CRYPTO) << "TeeApiMgmt::shutdownTee: teeApiComm instance is NULL";
        return NFErr_Uninitialized;
    }

    // create empty request
    DataBuffer shutRequest;

    // process the message
    uint32_t rspCode;
    DataBuffer rspData;
    teeApiComm->processMessage(nfTO_MGMT_OP_SHUTDOWN, shutRequest, rspCode, rspData);

    // handle error case
    if (rspCode != nfTGR_SUCCESS)
    {
        Log::fatal(TRACE_CRYPTO) << "TeeApiMgmt::shutdownTee: Cannot shutdown TEE, TEE returned error " << getTeeResultStr(rspCode);
        return translateTeeError((nfTeeResult_t)rspCode);
    }

    // response to shutdown has no data, so simply return success
    return NFErr_OK;
}

netflix::NFErr TeeApiMgmt::resetTeeDebugFlags(uint32_t debugFlags)
{
    if (!teeApiComm)
    {
        Log::fatal(TRACE_CRYPTO) << "TeeApiMgmt::resetTeeDebugFlags: teeApiComm instance is NULL";
        return NFErr_Uninitialized;
    }

    DataBuffer debugRequest;
    debugRequest.append<uint32_t>(htonl(debugFlags));

    // process the message
    NTRACE(TRACE_CRYPTO, "resetting TEE debug flags");
    uint32_t rspCode;
    DataBuffer rspData;
    teeApiComm->processMessage(nfTO_MGMT_OP_DEBUG, debugRequest, rspCode, rspData);

    // handle error case
    if (rspCode != nfTGR_SUCCESS)
    {
        Log::fatal(TRACE_CRYPTO) << "TeeApiMgmt::resetTeeDebugFlags: Cannot modify TEE debug flags, TEE returned error " << getTeeResultStr(rspCode);
        return translateTeeError((nfTeeResult_t)rspCode);
    }

    // response to init has no data, so simply return success
    return NFErr_OK;
}

netflix::NFErr TeeApiMgmt::translateTeeError (nfTeeResult_t teeResult)
{
    // just handle the success case first, this is the most likely value
    if (teeResult == nfTGR_SUCCESS)
        return NFErr_OK;

    NFErr nfErr = NFErr_Bad;
    if ((teeResult & nfMASK_RESULT_BASE) == nfDRM_AGENT_RESULT_BASE)
    {
        nfTeeMgmtResult_t mgmtResult = (nfTeeMgmtResult_t) teeResult;
        switch (mgmtResult)
        {
            case  nfTMR_NOT_INITIALIZED:
                nfErr = NFErr_Uninitialized;
                break;
            case  nfTMR_ALREADY_INITIALIZED:
                nfErr = NFErr_NotAllowed;
                break;
            case  nfTMR_PARAM_TOO_SMALL:
            case  nfTMR_PARAM_SIZE_MISMATCH:
                nfErr = NFErr_BadParameter;
                break;
        }
    }
    else
    {
        switch (teeResult)
        {
            case nfTGR_INMSG_INVALID:
            case nfTGR_INMSG_TOO_SHORT:
            case nfTGR_OUTMSG_INVALID:
            case nfTGR_OUTMSG_TOO_SHORT:
            case nfTGR_INVALID_OPERATION:
            case nfTGR_INVALID_PARAM_COUNT:
            case nfTGR_INVALID_PARM0_DESCRIPTION:
            case nfTGR_INVALID_PARM1_DESCRIPTION:
            case nfTGR_INVALID_PARM2_DESCRIPTION:
            case nfTGR_INVALID_PARM3_DESCRIPTION:
            case nfTGR_INVALID_PARM0:
            case nfTGR_INVALID_PARM1:
            case nfTGR_INVALID_PARM2:
            case nfTGR_INVALID_PARM3:
                nfErr = NFErr_BadParameter;
                break;
            case nfTGR_OPERATION_NOT_ALLOWED:
                nfErr = NFErr_NotAllowed;
                break;
            case nfTGR_INTERNAL_ERROR:
                nfErr = NFErr_Internal;
                break;
        }
    }

    return nfErr;
}
