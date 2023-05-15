/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include <nrdbase/Base64.h>
#include <nrdbase/Log.h>
#include "TeeApiStorage.h"
#include "ScopedPtr.h"

using namespace netflix;

namespace /* anonymous */ {
netflix::NFErr translateTeeError (nfTeeResult_t teeResult)
{
    using namespace netflix;

    // just handle the success case first, this is the most likely value
    if (teeResult == nfTGR_SUCCESS)
        return NFErr_OK;

    NFErr nfErr = NFErr_Bad;
    switch (teeResult)
    {
        case  nfTSR_NOT_FOUND:
            nfErr = NFErr_NotFound;
            break;
        case  nfTSR_MANUF_SSTORE_INVALID:
        case  nfTSR_CHUNK_SIZE_EXCEEDED:
        case  nfTSR_OPERAND_TOO_LONG:
        case  nfTSR_UNRECOGNIZED_VERSION:
        case  nfTSR_OPERAND_TOO_SHORT:
        case  nfTSR_BUFFER_TOO_SHORT:
        case  nfTSR_INVALID_PARM0:
        case  nfTSR_INVALID_PARM1:
        case  nfTSR_INVALID_PARM2:
            nfErr = NFErr_BadParameter;
            break;
        case  nfTSR_INTERNAL_ERROR:
        case  nfTSR_CRYPT_RANDOM_FAILURE:
        case  nfTSR_CIPHER_ERROR:
        case  nfTSR_KEY_DERIV_ERROR:
        case  nfTSR_INIT_FAILED:
            nfErr = NFErr_Internal;
            break;

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

        default:
            nfErr = NFErr_Internal;
            break;
    }

    return nfErr;
}
}


TeeApiStorage::TeeApiStorage() : m_isInited(false) { }

TeeApiStorage::~TeeApiStorage() {
    m_teeApiComm.reset();
}

netflix::NFErr TeeApiStorage::init(uint32_t maxChunkSize) {
    (void)maxChunkSize;

    if (m_isInited) {
        return NFErr_NotAllowed;
    }

    m_teeApiComm = TeeApiComm::getInstance();
    m_isInited = true;

    return NFErr_OK;
}

netflix::NFErr TeeApiStorage::storeProtected(DataBuffer& clearData, DataBuffer& protectedData)
{
    if (!m_isInited)
    {
        Log::fatal(TRACE_CRYPTO) << "TeeApiStorage::storeProtected: this instance is not initialized";
        return NFErr_Uninitialized;
    }

    if (clearData.empty())
    {
        Log::error(TRACE_CRYPTO) << "TeeApiStorage::storeProtected: Argument clearData is empty";
        return NFErr_BadParameter;
    }

    /*
     * create databuffer for nfTeeStorageProtectReq_t
     * typedef struct
     * {
     * 	uint32_t		DataLength;
     * 	uint8_t			Data[//DataLength];
     * } nfTeeStorageProtectReq_t;
     */
    DataBuffer storeRequest;
    storeRequest.append<uint32_t>(htonl(clearData.length()));
    storeRequest.append(clearData);

    // process the message
    uint32_t rspCode;
    DataBuffer rspData;
    m_teeApiComm->processMessage(nfTO_STORAGE_OP_PROTECT, storeRequest, rspCode, rspData);

    // handle error case
    if (rspCode != nfTGR_SUCCESS)
    {
        Log::fatal(TRACE_CRYPTO) << "TeeApiStorage::storeProtected: Cannot protect data, TEE returned error " << getTeeResultStr(rspCode);
        return translateTeeError((nfTeeResult_t)rspCode);
    }

    // make sure there is data to read
    if (rspData.size() < (int) nfMIN_STORAGE_PROTECT_RSP_SIZE)
    {
        Log::fatal(TRACE_CRYPTO) << "TeeApiStorage::storeProtected: Cannot protect data, TEE returned response of incorrect size";
        return NFErr_Internal;
    }

    // read the data
    unsigned char* rspDataStr = rspData.data();
    uint32_t dataLen =  ntohl( ((nfTeeStorageProtectRsp_t *)rspDataStr)->ProtectedDataLength );
    if ( dataLen > 0 )
    {
        // XXX - is append the right method here? Not sure...
        // copy data to protectedData
        protectedData.append(((nfTeeStorageProtectRsp_t *)rspDataStr)->ProtectedData, (int) dataLen );
    }

    return NFErr_OK;
}

netflix::NFErr TeeApiStorage::loadProtected(DataBuffer& protectedData, DataBuffer& clearData)
{
    if (!m_isInited)
    {
        Log::fatal(TRACE_CRYPTO) << "TeeApiStorage::loadProtected: this instance is not initialized";
        return NFErr_Uninitialized;
    }
    if (protectedData.empty())
    {
        Log::error(TRACE_CRYPTO) << "TeeApiCrypto::importKey: Argument protectedData is empty";
        return NFErr_BadParameter;
    }


    /*
     * create databuffer for nfTeeStorageUnprotectReq_t
     * typedef struct
     * {
     * 	uint32_t		DataLength;
     * 	uint8_t			Data[//DataLength];
     * } nfTeeStorageUnprotectReq_t;
     */
    DataBuffer loadRequest;
    loadRequest.append<uint32_t>(htonl(protectedData.length()));
    loadRequest.append(protectedData);

    // process the message
    uint32_t rspCode;
    DataBuffer rspData;
    m_teeApiComm->processMessage(nfTO_STORAGE_OP_UNPROTECT, loadRequest, rspCode, rspData);

    // handle error case
    if (rspCode != nfTGR_SUCCESS)
    {
        Log::fatal(TRACE_CRYPTO) << "TeeApiStorage::loadProtected: Cannot unprotect data, TEE returned error " << getTeeResultStr(rspCode);
        return translateTeeError((nfTeeResult_t)rspCode);
    }

    // make sure there is data to read
    if (rspData.size() < (int) nfMIN_STORAGE_UNPROTECT_RSP_SIZE)
    {
        Log::fatal(TRACE_CRYPTO) << "TeeApiStorage::loadProtected: Cannot unprotect data, TEE returned response of incorrect size";
        return NFErr_Internal;
    }

    // read the data
    unsigned char* rspDataStr = rspData.data();
    uint32_t dataLen =  ntohl( ((nfTeeStorageUnprotectRsp_t *)rspDataStr)->DataLength );
    // XXX make sure dataLen < rspLen
    if ( dataLen > 0 )
    {
        // XXX - is append the right method here? Not sure...
        clearData.append(((nfTeeStorageUnprotectRsp_t *)rspDataStr)->Data, (int) dataLen );
    }

    return NFErr_OK;
}

