#include <nrdbase/Base64.h>
#include <nrdbase/Log.h>
#include "TeeApiCrypto.h"
#include "ScopedPtr.h"

#include <openssl/rsa.h>
#include <openssl/asn1.h>
#include <openssl/evp.h>
#include <openssl/objects.h>
#include <openssl/err.h>
#include <openssl/ssl.h>
#include <openssl/rand.h>

using namespace netflix;

TeeApiCrypto::TeeApiCrypto()
{
    teeApiComm = TeeApiComm::getInstance();
}

TeeApiCrypto::~TeeApiCrypto()
{
    teeApiComm.reset();
}

NFErr TeeApiCrypto::importKey(IWebCrypto::KeyFormat keyFormat,
        const DataBuffer& keyData, IWebCrypto::Algorithm algType, bool extractable,
        unsigned int keyUsage, nfCryptoKeyHandle_t &keyHandle, IWebCrypto::KeyType &keyType)
{
    if (!teeApiComm)
    {
        Log::error(TRACE_CRYPTO) << "TeeApiCrypto::importKey: teeApiComm is NULL";
        return NFErr_Uninitialized;
    }

    // verify input params
    if (keyData.empty())
    {
        Log::error(TRACE_CRYPTO) << "TeeApiCrypto::importKey: Argument keyData is empty";
        return NFErr_BadParameter;
    }
    if (keyFormat != IWebCrypto::RAW)
    {
        Log::error(TRACE_CRYPTO) << "TeeApiCrypto::importKey: Cannot import non-RAW keys into TEE";
        return NFErr_BadParameter;
    }

    /*
     * create a DataBuffer with the contents of nfTeeImportKeyRequest_t
     *
     * typedef struct
     * {
     *  nfCryptoAlgorithm_t		Algorithm;
     *  nfKeyUsageFlagsWord_t	KeyUsageFlags;
     *  nfKeyFormat_t			KeyFormat;
     *  uint32_t				DataLength;
     *  uint8_t	    			KeyData[DataLength];
     * } nfTeeImportKeyRequest_t;
     *
     */
    DataBuffer importKeyRequest;

    importKeyRequest.append<uint32_t>(htonl(convertAlgo(algType)));

    nfKeyUsageFlagsWord_t keyUsageFlags = convertWcUsage(keyUsage);
    if (extractable)
    {
        keyUsageFlags |= nfKUF_EXTRACTABLE;
    }
    importKeyRequest.append<uint32_t>(htonl(keyUsageFlags));

    importKeyRequest.append<uint32_t>(htonl(convertKeyFormat(keyFormat)));

    importKeyRequest.append<uint32_t>(htonl(keyData.size()));
    importKeyRequest.append(keyData);

    // process the message
    uint32_t rspCode;
    DataBuffer rspData;
    teeApiComm->processMessage (nfTO_CRYPTO_OP_IMPORT_KEY, importKeyRequest, rspCode, rspData);

    // handle error case
    if (rspCode != nfTGR_SUCCESS)
    {
        Log::error(TRACE_CRYPTO) << "TeeApiCrypto::importKey: Cannot import key, TEE returned error " << getTeeResultStr(rspCode);
        return translateTeeError((nfTeeResult_t)rspCode);
    }

    // ensure there is data to read
    if (rspData.size() < (int) sizeof(nfTeeImportKeyResponse_t))
    {
        Log::error(TRACE_CRYPTO) << "TeeApiCrypto::importKey: Cannot import key, TEE did not return correct response (handle and key type)";
        return NFErr_Internal;
    }

    //read the data
    unsigned char* rspDataStr = rspData.data();
    keyHandle = (nfCryptoKeyHandle_t)(ntohl(*((uint32_t*)rspDataStr)));
    nfKeyType_t nfKeyType = (nfKeyType_t) ntohl(*((uint32_t*)(rspDataStr+sizeof(uint32_t))));
    keyType = convertKeyType(nfKeyType);

    return NFErr_OK;
}

NFErr TeeApiCrypto::exportKey (nfCryptoKeyHandle_t keyHandle, IWebCrypto::KeyFormat keyFormat,
            /*out*/ DataBuffer& keyData)
{
    if (!teeApiComm)
    {
        Log::error(TRACE_CRYPTO) << "TeeApiCrypto::exportKey: teeApiComm is NULL";
        return NFErr_Uninitialized;
    }
    // verify input params
    if (keyFormat != IWebCrypto::RAW)
    {
        Log::error(TRACE_CRYPTO) << "TeeApiCrypto::exportKey: Cannot export non-RAW keys from TEE";
        return NFErr_BadParameter;
    }


    /*
     * create a DataBuffer with the contents of nfTeeExportKeyRequest_t
     *
     * typedef struct
     * {
     *  nfCryptoKeyHandle_t		KeyHandle;
     *  nfKeyFormat_t			KeyFormat;
     * } nfTeeExportKeyRequest_t;
     *
     */
    DataBuffer exportKeyRequest;
    exportKeyRequest.append<uint32_t>(htonl(keyHandle));
    exportKeyRequest.append<uint32_t>(htonl(convertKeyFormat(keyFormat)));

    // process the message
    uint32_t rspCode;
    DataBuffer rspData;
    teeApiComm->processMessage(nfTO_CRYPTO_OP_EXPORT_KEY, exportKeyRequest, rspCode, rspData);

    // handle error case
    if (rspCode != nfTGR_SUCCESS)
    {
        Log::error(TRACE_CRYPTO) << "TeeApiCrypto::exportKey: Cannot export key, TEE returned error " << getTeeResultStr(rspCode);
        return translateTeeError((nfTeeResult_t)rspCode);
    }

    // ensure there is data to read
    if (rspData.size() < (int) nfMIN_EXPORT_KEY_RSP_SIZE)
    {
        Log::error(TRACE_CRYPTO) << "TeeApiCrypto::exportKey: Cannot export key, TEE did not return correct response (handle and key type)";
        return NFErr_Internal;
    }

    //read the data
    unsigned char* rspDataStr = rspData.data();
    uint32_t dataLen = ntohl(*((uint32_t*)rspDataStr));
    keyData.clear();
    keyData.append((char *)(rspDataStr + sizeof(uint32_t)), dataLen);

    return NFErr_OK;
}

NFErr TeeApiCrypto::deleteKey (nfCryptoKeyHandle_t keyHandle)
{
    if (!teeApiComm)
    {
        Log::error(TRACE_CRYPTO) << "TeeApiCrypto::deleteKey: teeApiComm is NULL";
        return NFErr_Uninitialized;
    }

    /*
     * create a DataBuffer with the contents of nfTeeDeleteKeyRequest_t
     *  typedef struct
     *  {
     *      nfCryptoKeyHandle_t KeyHandle;
     *  } nfTeeDeleteKeyRequest_t;
     *
     */
    DataBuffer delKeyReq;
    delKeyReq.append<uint32_t>(htonl(keyHandle));

    NTRACE(TRACE_CRYPTO, "Calling delete key in TEE for handle %d", keyHandle);
    uint32_t rspCode;
    DataBuffer rspBuf;
    teeApiComm->processMessage(nfTO_CRYPTO_OP_DELETE_KEY, delKeyReq, rspCode, rspBuf);

    // handle error case
    if (rspCode != nfTGR_SUCCESS)
    {
        Log::error(TRACE_CRYPTO) << "TeeApiCrypto::deleteKey: TEE returned error " << getTeeResultStr(rspCode);
        return translateTeeError((nfTeeResult_t)rspCode);
    }

    return NFErr_OK;
}

NFErr TeeApiCrypto::getNamedKey (std::string KeyName, nfCryptoKeyHandle_t& keyHandle)
{
    if (!teeApiComm)
    {
        Log::error(TRACE_CRYPTO) << "TeeApiCrypto::getNamedKey: teeApiComm is NULL";
        return NFErr_Uninitialized;
    }

    /*
     * create a DataBuffer with the contents of nfTeeGetNamedKeyHandleRequest_t
     *  - uint32_t	KeyNameLength
     *  - uint8_t	    KeyName[];
     */
    DataBuffer getNamedKeyRequest;
    getNamedKeyRequest.append<uint32_t>(htonl(KeyName.length()));
    getNamedKeyRequest.append<const char>(KeyName.c_str(), KeyName.length());

    // process the message
    NTRACE(TRACE_CRYPTO, "Getting named key %s from TEE", KeyName.c_str());
    uint32_t rspCode;
    DataBuffer rspData;
    teeApiComm->processMessage(nfTO_CRYPTO_OP_GET_NAMED_KEY_HANDLE, getNamedKeyRequest, rspCode, rspData);

    // handle error case
    if (rspCode != nfTGR_SUCCESS)
    {
        Log::error(TRACE_CRYPTO) << "TeeApiCrypto::getNamedKey: Cannot get handle, TEE returned error " << getTeeResultStr(rspCode);
        return translateTeeError((nfTeeResult_t)rspCode);
    }

    // ensure there is data to read
    if (rspData.size() < (int) sizeof(nfTeeGetNamedKeyHandleResponse_t))
    {
        Log::error(TRACE_CRYPTO) << "TeeApiCrypto::getNamedKey: Cannot get handle, TEE did not return correct response";
        return NFErr_Internal;
    }

    //read the data
    unsigned char* rspDataStr = rspData.data();
    keyHandle = (nfCryptoKeyHandle_t)(ntohl(*((uint32_t*)rspDataStr)));

    return NFErr_OK;
}// end getNamedKey

NFErr TeeApiCrypto::getKeyInfo (nfCryptoKeyHandle_t keyHandle, IWebCrypto::KeyType &keyType,
                                bool &extractable, IWebCrypto::Algorithm &algo, unsigned int &usage)
{
    if (!teeApiComm)
    {
        Log::error(TRACE_CRYPTO) << "TeeApiCrypto::getKeyInfo: teeApiComm is NULL";
        return NFErr_Uninitialized;
    }

    /*
     * create a DataBuffer with the contents of nfTeeGetKeyInfoRequest_t
     *  typedef struct
     *  {
     *   nfCryptoKeyHandle_t	KeyHandle;
     *  } nfTeeGetKeyInfoRequest_t;
     *
     */
    DataBuffer getKeyInfoRequest;
    getKeyInfoRequest.append<uint32_t>(htonl(keyHandle));

    // process the message
    NTRACE(TRACE_CRYPTO, "Getting key Info from TEE for handle %d", keyHandle);
    uint32_t rspCode;
    DataBuffer rspData;
    teeApiComm->processMessage(nfTO_CRYPTO_OP_GET_KEY_INFO, getKeyInfoRequest, rspCode, rspData);

    // handle error case
    if (rspCode != nfTGR_SUCCESS)
    {
        Log::error(TRACE_CRYPTO) << "TeeApiCrypto::getKeyInfo: TEE returned error " << getTeeResultStr(rspCode);
        return translateTeeError((nfTeeResult_t)rspCode);
    }

    // ensure there is data to read
    if (rspData.size() < (int) sizeof(nfTeeGetKeyInfoResponse_t))
    {
        Log::error(TRACE_CRYPTO) << "TeeApiCrypto::getKeyInfo: TEE did not return correct response";
        return NFErr_Internal;
    }

    //read the data
    nfTeeGetKeyInfoResponse_t *keyInfoPtr = (nfTeeGetKeyInfoResponse_t*)(rspData.data());
    nfKeyType_t nfKeyType = (nfKeyType_t) ntohl(keyInfoPtr->KeyType);
    nfCryptoAlgorithm_t nfAlgo = (nfCryptoAlgorithm_t) ntohl(keyInfoPtr->Algorithm);
    nfKeyUsageFlagsWord_t nfUsage = (nfKeyUsageFlagsWord_t) ntohl(keyInfoPtr->KeyUsageFlags);
    keyType = convertKeyType(nfKeyType);
    algo = convertAlgo(nfAlgo);
    usage = convertTeeUsage(nfUsage);
    extractable = (nfUsage & nfKUF_EXTRACTABLE) ? true : false;

    return NFErr_OK;
} //end getKeyInfo

NFErr TeeApiCrypto::hmac (nfCryptoKeyHandle_t keyHandle, IWebCrypto::Algorithm shaAlg,
                const DataBuffer& data, DataBuffer& hmacBuf)
{
    if (!teeApiComm)
    {
        Log::error(TRACE_CRYPTO) << "TeeApiCrypto::hmac: teeApiComm is NULL";
        return NFErr_Uninitialized;
    }

    // verify input params
    int dataLen = data.size();
    if (dataLen == 0)
    {
        Log::error(TRACE_CRYPTO) << "TeeApiCrypto::hmac: data is empty";
        return NFErr_BadParameter;
    }

    /*
     * First, what algorithm are we using? TEE only supports HMAC-SHA256, so
     * anything else and we're finished.
     */
    nfTeeOperation_CryptoOp_t op = nfTO_CRYPTO_OP_INVALID;
    uint32_t hmacLen = 0;
    switch(shaAlg)
    {
        case IWebCrypto::SHA256:
            op = nfTO_CRYPTO_OP_HMAC_SHA256;
            hmacLen = nfHMAC_SHA256_OUTPUT_SIZE;
            break;
        case IWebCrypto::SHA384:
        case IWebCrypto::SHA512:
        case IWebCrypto::SHA1:
        case IWebCrypto::SHA224:
        default:
            //XXX - not defined yet
            break;
    }
    if ( op == nfTO_CRYPTO_OP_INVALID )
    {
        // need larger TeeApiComm buffers.
        Log::error(TRACE_CRYPTO) << "TeeApiCrypto::hmac: unsupported algorithm.";
        return NFErr_Internal;
    }

    /*
     * Before we do anything else, we need to decide if we're doing an atomic
     * hmac operation (single call to TEE) or an incremental hmac operation
     * (multiple calls to TEE). This depends on whether or not reqBuf
     * can hold entire request. If so, we can do atomic hmac operations.
     *
     * 	Otherwise, we need call the hmac-init function to create a context,
     * 	hmac the input buffer chunk by chunk (by calling hmac-update), and
     * 	then clean up, get the final hmac value, etc. by calling hmac-final.
     *
     * 	To answer the first question, we need to get the req buffer size from
     * 	TeeApiComm, and compare it to maxReqSize. If the date will fit,
     * 	we can do atomic. Otherwise, we must do incremental hmac
     */
    uint32_t reqBufSize = teeApiComm->getReqBufSize();
    uint32_t rspBufSize = teeApiComm->getRspBufSize();

    //printf("TeeApiCrypto::%s - op is hmac, reqBufSize is %d, rspBufSize is %d\n", __FUNCTION__,reqBufSize,rspBufSize);

    if ( ( reqBufSize < nfMIN_HMACSHA256_REQ_SIZE ) || ( rspBufSize < hmacLen ) )
    {
        // need larger TeeApiComm buffers.
        Log::error(TRACE_CRYPTO) << "TeeApiCrypto::hmac: comm buffer(s) too small.";
        return NFErr_Internal;
    }

    /*
     * Okay, so we've ascertained that we have the minimal necessary comm buffers (i.e. we can
     * send the hmac data at least one byte at a time (doh!), and there is enough room in
     * the rsp buffer for HMACSHA256. Now, let's see if we're doing atomic vs. incremental.
     */

    if ( reqBufSize >= nfMIN_HMACSHA256_REQ_SIZE + dataLen - 1 )
    {
        /*
         * This is an atomic hmac operation (one round trip to the TEE)
         *
         * create a DataBuffer with the contents of nfTeeHmacSha256Request_t
         *  typedef struct
         *  {
         *  	nfCryptoKeyHandle_t		KeyHandle;
         *  	uint32_t				DataLength;
         *  	uint8_t	    			Data[];
         *  } nfTeeHmacSha256Request_t;
         *
         */
        DataBuffer hmacRequest;
        hmacRequest.append<uint32_t>(htonl(keyHandle));
        hmacRequest.append<uint32_t>(htonl(dataLen));
        hmacRequest.append(data);

        //printf("TeeApiCrypto::%s - atomic hmac request\n", __FUNCTION__);

        // process the message
        NTRACE(TRACE_CRYPTO, "Calling HMAC in TEE using handle %d", keyHandle);
        uint32_t rspCode;
        hmacBuf.clear();
        teeApiComm->processMessage(op, hmacRequest, rspCode, hmacBuf);

        // handle error case
        if (rspCode != nfTGR_SUCCESS)
        {
            Log::error(TRACE_CRYPTO) << "TeeApiCrypto::hmac: Cannot compute HMAC, TEE returned error " << getTeeResultStr(rspCode);
            return translateTeeError((nfTeeResult_t)rspCode);
        }

        // ensure data is of right size
        if (hmacBuf.size() != (int) hmacLen)
        {
            Log::error(TRACE_CRYPTO) << "TeeApiCrypto::hmac: TEE did not return HMAC of correct size";
            return NFErr_Internal;
        }
        return NFErr_OK;
    }

    /**
     * If we get here, we have to do incremental hmac ops (init/update/final). We
     * want to do the following:
     *
     * 		- figure out the chunk size
     * 			- much simpler for hmac, it is simply reqSize - sizeof(hmac header)
     * 		- send {en,de}cryptInit to allocate context, set IV
     * 			- store context handle upon return
     * 		- for each chunk
     * 			- send {en,de}cryptUpdate with context handle, next chunk of data
     * 			- save result
     * 		- send {en,de}cryptFinal with context
     * 			- if encrypt, get final result
     */

    uint32_t chunkSize = reqBufSize - sizeof(nfTeeHmacSha256UpdateRequest_t);

    //printf("TeeApiCrypto::%s - chunkSize is %d\n", __FUNCTION__, chunkSize);

    // now, send hmac-init and get context handle
    /*
     * Create a DataBuffer with the contents for hmac-sha256 init.
     *
     * nfCryptoKeyHandle_t		KeyHandle;
     *
     */

    DataBuffer hmacReq;
    hmacReq.append<uint32_t>(htonl(keyHandle));

    // process the message
    uint32_t rspCode;
    DataBuffer rspData;
    op = nfTO_CRYPTO_OP_HMAC_SHA256_INIT;
    teeApiComm->processMessage(op, hmacReq, rspCode, rspData);

    // handle error case
    if (rspCode != nfTGR_SUCCESS)
    {
        Log::error(TRACE_CRYPTO) << "TeeApiCrypto::hmac-init: TEE returned error " << getTeeResultStr(rspCode);
        return translateTeeError((nfTeeResult_t)rspCode);
    }

    // ensure there is expected data
    if (rspData.size() < nfTEE_CRYPTO_CONTEXT_HANDLE_SIZE)
    {
        Log::error(TRACE_CRYPTO) << "TeeApiCrypto::hmac-init: TEE did not return response of correct size";
        return NFErr_Internal;
    }

    // save context handle
    nfTeeCryptoContextHandle_t contextHandle = *((nfTeeCryptoContextHandle_t *) rspData.data());

    //printf("TeeApiCrypto::%s - done with init, got context\n", __FUNCTION__);

    // init variables for update loop
    int bytesLeft = data.size();
    const unsigned char *dataPtr = data.constData();
    int offset = 0;

    // previously, op was xxx_INIT -- change to xxx_UPDATE
    op = nfTO_CRYPTO_OP_HMAC_SHA256_UPDATE;

    // now, loop through inBuf, chunkSize bytes at a time
    do
    {
        /*
         * Each time through the loop:
         *  - if last chunk, update chunkSize to match remaining bytes
         * 	- get next chunk
         * 		- use databuffer methods to return databuffer containing chunk
         * 	- update offset, bytesLeft
         * 	- clear req buffer
         * 	- add context handle to req buffer
         * 	- add data length to req buffer
         * 	- add data to req buffer
         * 	- send req buffer to TEE
         * 	- get response, verify success
         * 	- append response to outBuf
         */

        // decrease chunkSize if final chunk is smaller
        if ( bytesLeft < (int) chunkSize )
            chunkSize = (uint32_t) bytesLeft;

        //printf("TeeApiCrypto::%s - chunkSize is %d, bytesLeft is %d\n", __FUNCTION__, chunkSize, bytesLeft);

        /*
         * updateReq looks like this:
         *
         * typedef struct
         *	{
         *		nfTeeCryptoContextHandle_t	CryptoContextHandle;
         *		uint32_t					DataLength;
         *		uint8_t	    				Data[];
         *	} nfTeeHmacSha256UpdateRequest_t;
         *
         */
        hmacReq.clear();
        hmacReq.append((char *)&contextHandle, nfTEE_CRYPTO_CONTEXT_HANDLE_SIZE);
        hmacReq.append<uint32_t>(htonl(chunkSize));
        hmacReq.append(dataPtr + offset, chunkSize);

        // update housekeeping variables
        offset += chunkSize;
        bytesLeft -= chunkSize;

        // process the message
        rspCode = 0;
        rspData.clear();
        teeApiComm->processMessage(op, hmacReq, rspCode, rspData);

        // we don't expect any response data, ignore...

    } while ( bytesLeft > 0 );

    /*
     * When we get here, we're ready to send the final message. It looks like this:
     *
     *	typedef struct
     *	{
     *		nfTeeCryptoContextHandle_t	CryptoContextHandle;
     *	} nfTeeHmacSha256FinalRequest_t;
     *
     */
    hmacReq.clear();
    hmacReq.append((char *)&contextHandle, nfTEE_CRYPTO_CONTEXT_HANDLE_SIZE);
    op = nfTO_CRYPTO_OP_HMAC_SHA256_FINAL;

    //printf("TeeApiCrypto::%s - calling final\n", __FUNCTION__);

    // process the message
    rspCode = 0;
    rspData.clear();
    teeApiComm->processMessage(op, hmacReq, rspCode, hmacBuf);

    return NFErr_OK;

}

NFErr TeeApiCrypto::hmacVerify (nfCryptoKeyHandle_t keyHandle, IWebCrypto::Algorithm shaAlg,
                    const DataBuffer& data, const DataBuffer& hmac, /*out*/bool& verified)
{
    if (!teeApiComm)
    {
        Log::error(TRACE_CRYPTO) << "TeeApiCrypto::hmacVerify: teeApiComm is NULL";
        return NFErr_Uninitialized;
    }

    // verify input params
    int dataLen = data.size();
    int hmacLen = hmac.size();
    if (dataLen == 0)
    {
        Log::error(TRACE_CRYPTO) << "TeeApiCrypto::hmacVerify: data is empty";
        return NFErr_BadParameter;
    }
    if (hmacLen != nfHMAC_SHA256_OUTPUT_SIZE)
    {
        Log::error(TRACE_CRYPTO) << "TeeApiCrypto::hmacVerify: hmac size is wrong, expected " << nfHMAC_SHA256_OUTPUT_SIZE
                            << " but it is " << hmacLen;
        return NFErr_BadParameter;
    }

    /*
     * First, what algorithm are we using? TEE only supports HMAC-SHA256, so
     * anything else and we're finished.
     */
    nfTeeOperation_CryptoOp_t op = nfTO_CRYPTO_OP_INVALID;
    switch(shaAlg)
    {
        case IWebCrypto::SHA256:
            op = nfTO_CRYPTO_OP_HMAC_SHA256_VERIFY;
            hmacLen = nfHMAC_SHA256_OUTPUT_SIZE;
            break;
        case IWebCrypto::SHA384:
        case IWebCrypto::SHA512:
        case IWebCrypto::SHA1:
        case IWebCrypto::SHA224:
        default:
            //XXX - not defined yet
            break;
    }
    if ( op == nfTO_CRYPTO_OP_INVALID )
    {
        // need larger TeeApiComm buffers.
        Log::error(TRACE_CRYPTO) << "TeeApiCrypto::hmac: unsupported algorithm.";
        return NFErr_Internal;
    }

    /*
     * Before we do anything else, we need to decide if we're doing an atomic
     * hmac operation (single call to TEE) or an incremental hmac operation
     * (multiple calls to TEE). This depends on whether or not reqBuf
     * can hold entire request. If so, we can do atomic hmac operations.
     *
     * 	Otherwise, we need call the hmac-init function to create a context,
     * 	hmac the input buffer chunk by chunk (by calling hmac-update), and
     * 	then clean up, get the final hmac value, etc. by calling hmac-final.
     *
     * 	To answer the first question, we need to get the req buffer size from
     * 	TeeApiComm, and compare it to maxReqSize. If the date will fit,
     * 	we can do atomic. Otherwise, we must do incremental hmac
     */
    uint32_t reqBufSize = teeApiComm->getReqBufSize();

    //printf("TeeApiCrypto::%s - op is hmacverify, reqBufSize is %d\n", __FUNCTION__, reqBufSize);

    if ( reqBufSize < nfMIN_HMACSHA256_VERIFY_REQ_SIZE )
    {
        // need larger TeeApiComm buffers.
        Log::error(TRACE_CRYPTO) << "TeeApiCrypto::hmac: comm buffer too small.";
        return NFErr_Internal;
    }

    /*
     * Okay, so we've ascertained that we have the minimal necessary comm buffers (i.e. we can
     * send the hmac data at least one byte at a time (doh!). Now, let's see if we're doing
     * atomic vs. incremental hmac.
     */

    if ( reqBufSize >= nfMIN_HMACSHA256_VERIFY_REQ_SIZE + dataLen - 1 )
    {
        /*
         * This is an atomic hmac verify operation (one round trip to the TEE)
         *
         * create a DataBuffer with the contents of nfTeeHmacSha256VerifyRequest_t
         *  typedef struct
         *  {
         *    nfCryptoKeyHandle_t    KeyHandle;
         *    uint8_t                HmacValue[nfHMAC_SHA256_OUTPUT_SIZE];
         *    uint32_t               DataLength;
         *    uint8_t                Data[];
         *  } nfTeeHmacSha256VerifyRequest_t;
         *
         */
        //printf("TeeApiCrypto::%s - doing atomic verify\n", __FUNCTION__);
        DataBuffer hmacVerifyReq;
        hmacVerifyReq.append<uint32_t>(htonl(keyHandle));
        hmacVerifyReq.append<unsigned char>(hmac.data(), (int) nfHMAC_SHA256_OUTPUT_SIZE);
        hmacVerifyReq.append<uint32_t>(htonl(dataLen));
        hmacVerifyReq.append(data);

        NTRACE(TRACE_CRYPTO, "Calling HMAC Verify in TEE using handle %d", keyHandle);
        uint32_t rspCode;
        DataBuffer rspBuf;
        teeApiComm->processMessage(op, hmacVerifyReq, rspCode, rspBuf);

        // handle error case
        if (rspCode != nfTGR_SUCCESS)
        {
            verified = false;
            //any error other than nfTCR_HMAC_VERIFY_FAILED, return error
            if (rspCode != nfTCR_HMAC_VERIFY_FAILED)
            {
                Log::error(TRACE_CRYPTO) << "TeeApiCrypto::hmacVerify: TEE returned error " << getTeeResultStr(rspCode);
                return translateTeeError((nfTeeResult_t)rspCode);
            }
        }
        else
        {
            verified = true;
        }

        return NFErr_OK;
    }

    /**
     * If we get here, we have to do incremental hmac ops (init/update/final). We
     * want to do the following:
     *
     * 		- figure out the chunk size
     * 			- much simpler for hmac, it is simply reqSize - sizeof(hmac header)
     * 		- send {en,de}cryptInit to allocate context, set IV
     * 			- store context handle upon return
     * 		- for each chunk
     * 			- send {en,de}cryptUpdate with context handle, next chunk of data
     * 			- save result
     * 		- send {en,de}cryptFinal with context
     * 			- if encrypt, get final result
     */

    uint32_t chunkSize = reqBufSize - sizeof(nfTeeHmacSha256UpdateRequest_t);
    //printf("TeeApiCrypto::%s - op is hmacverify, chunkSize is %d\n", __FUNCTION__, chunkSize);

    // now, send hmac-init and get context handle
    /*
     * Create a DataBuffer with the contents for hmac-sha256-verify init.
     *
     * nfCryptoKeyHandle_t		KeyHandle;
     *
     */

    DataBuffer hmacReq;
    hmacReq.append<uint32_t>(htonl(keyHandle));

    // process the message
    uint32_t rspCode;
    DataBuffer rspData;
    op = nfTO_CRYPTO_OP_HMAC_SHA256_VERIFY_INIT;
    teeApiComm->processMessage(op, hmacReq, rspCode, rspData);

    // handle error case
    if (rspCode != nfTGR_SUCCESS)
    {
        Log::error(TRACE_CRYPTO) << "TeeApiCrypto::hmac-init: TEE returned error " << getTeeResultStr(rspCode);
        return translateTeeError((nfTeeResult_t)rspCode);
    }

    // ensure there is expected data
    if (rspData.size() < nfTEE_CRYPTO_CONTEXT_HANDLE_SIZE)
    {
        Log::error(TRACE_CRYPTO) << "TeeApiCrypto::hmac-init: TEE did not return response of correct size";
        return NFErr_Internal;
    }

    // save context handle
    nfTeeCryptoContextHandle_t contextHandle = *((nfTeeCryptoContextHandle_t *) rspData.data());
    //printf("TeeApiCrypto::%s - completed init, saving context\n", __FUNCTION__);

    // init variables for update loop
    int bytesLeft = data.size();
    const unsigned char *dataPtr = data.constData();
    int offset = 0;

    // previously, op was xxx_INIT -- change to xxx_UPDATE
    op = nfTO_CRYPTO_OP_HMAC_SHA256_VERIFY_UPDATE;

    // now, loop through inBuf, chunkSize bytes at a time
    do
    {
        /*
         * Each time through the loop:
         *  - if last chunk, update chunkSize to match remaining bytes
         * 	- get next chunk
         * 		- use databuffer methods to return databuffer containing chunk
         * 	- update offset, bytesLeft
         * 	- clear req buffer
         * 	- add context handle to req buffer
         * 	- add data length to req buffer
         * 	- add data to req buffer
         * 	- send req buffer to TEE
         * 	- get response, verify success
         * 	- append response to outBuf
         */

        // decrease chunkSize if final chunk is smaller
        if ( bytesLeft < (int) chunkSize )
            chunkSize = (uint32_t) bytesLeft;

        //printf("TeeApiCrypto::%s - op is hmacverify, chunkSize is %d, bytesLeft is %d\n", __FUNCTION__,chunkSize, bytesLeft );
        /*
         * updateReq looks like this:
         *
         * typedef struct
         *	{
         *		nfTeeCryptoContextHandle_t	CryptoContextHandle;
         *		uint32_t					DataLength;
         *		uint8_t	    				Data[];
         *	} nfTeeHmacSha256UpdateRequest_t;
         *
         */
        hmacReq.clear();
        hmacReq.append((char *)&contextHandle, nfTEE_CRYPTO_CONTEXT_HANDLE_SIZE);
        hmacReq.append<uint32_t>(htonl(chunkSize));
        hmacReq.append(dataPtr + offset, chunkSize);

        // update housekeeping variables
        offset += chunkSize;
        bytesLeft -= chunkSize;

        // process the message
        rspCode = 0;
        rspData.clear();
        teeApiComm->processMessage(op, hmacReq, rspCode, rspData);

        // we don't expect any response data, ignore...

    } while ( bytesLeft > 0 );

    /*
     * When we get here, we're ready to send the final message. It looks like this:
     *
     *	typedef struct
     *	{
     *		nfTeeCryptoContextHandle_t	CryptoContextHandle;
     *		uint8_t	    				HmacValue[nfHMAC_SHA256_OUTPUT_SIZE];
     *	} nfTeeHmacSha256VerifyFinalRequest_t;
     *
     */
    hmacReq.clear();
    hmacReq.append((char *)&contextHandle, nfTEE_CRYPTO_CONTEXT_HANDLE_SIZE);
    hmacReq.append<unsigned char>(hmac.data(), (int) nfHMAC_SHA256_OUTPUT_SIZE);
    op = nfTO_CRYPTO_OP_HMAC_SHA256_VERIFY_FINAL;

    //printf("TeeApiCrypto::%s - op is hmacverify, calling final\n", __FUNCTION__);

    // process the message
    rspCode = 0;
    rspData.clear();
    teeApiComm->processMessage(op, hmacReq, rspCode, rspData);

    // handle error case
    if (rspCode != nfTGR_SUCCESS)
    {
        verified = false;
        //any error other than nfTCR_HMAC_VERIFY_FAILED, return error
        if (rspCode != nfTCR_HMAC_VERIFY_FAILED)
        {
            Log::error(TRACE_CRYPTO) << "TeeApiCrypto::hmacVerify: TEE returned error " << getTeeResultStr(rspCode);
            return translateTeeError((nfTeeResult_t)rspCode);
        }
    }
    else
    {
        verified = true;
    }

    return NFErr_OK;

}

NFErr TeeApiCrypto::aescbc (nfCryptoKeyHandle_t keyHandle, IWebCrypto::CipherOp cipherOp,
                        const DataBuffer& ivBuf, const DataBuffer& inBuf, DataBuffer& outBuf)
{

    if (!teeApiComm)
    {
        Log::error(TRACE_CRYPTO) << "TeeApiCrypto::aescbc: teeApiComm is NULL";
        return NFErr_Uninitialized;
    }

    // verify input params
    int ivLen = ivBuf.size();
    int inLen = inBuf.size();

    if ((ivLen == 0) || (ivLen < nfTEE_AES_IV_MIN) || (ivLen > nfTEE_AES_IV_MAX))
    {
        Log::error(TRACE_CRYPTO) << "TeeApiCrypto::aescbc: arguments are not of right size";
        return NFErr_BadParameter;
    }

    // figure out operation specific details
    uint32_t minRspLen = 0;
    uint32_t maxRspLen = 0;

    nfTeeOperation_CryptoOp_t op;
    if (cipherOp == IWebCrypto::DOENCRYPT)
    {
        // op is encryption (not sure if atomic or incremental yet)
        op = nfTO_CRYPTO_OP_AES_CBC_ENCRYPT;
        // min size is message header + padding (e.g. for zero-byte message)
        minRspLen = nfMIN_AES_CBC_ENCR_RSP_SIZE;
        maxRspLen = nfMIN_AES_CBC_ENCR_RSP_SIZE + inLen; // already includes max padding
    }
    else if (cipherOp == IWebCrypto::DODECRYPT)
    {
        // op is decryption (not sure if atomic or incremental yet)
        op = nfTO_CRYPTO_OP_AES_CBC_DECRYPT;
        minRspLen = nfMIN_AES_CBC_DECRYPT_RSP_SIZE;
        maxRspLen = nfMIN_AES_CBC_DECRYPT_RSP_SIZE + inLen;
    }
    else
    {
        Log::error(TRACE_CRYPTO) << "TeeApiCrypto::aescbc: invalid cipherOp";
        return NFErr_BadParameter;
    }

    //printf("TeeApiCrypto::%s - maxRspLen is %d, minRspLen is %d\n", __FUNCTION__, maxRspLen, minRspLen);

    /*
     * Before we do anything else, we need to decide if we're doing an atomic
     * encrypt operation (single call to TEE) or an incremental encrypt operation
     * (multiple calls to TEE). This depends on 2 things:
     *
     * 	- can reqBuf hold entire cleartext/ciphertext request?
     * 	- can rspBuf hold entire ciphertext/cleartext response?
     *
     * 	Notice that both must be true in order to use an atomic operation.
     * 	Otherwise, we need call the encrypt-init function to create a context,
     * 	encrypt the input buffer chunk by chunk (by calling encrypt-update), and
     * 	then clean up, get the final padding block, etc. by calling encrypt-final.
     *
     * 	To answer the first question, we need to get the req/rsp buffer sizes from
     * 	TeeApiComm, and compare them to maxReqSize and maxRspSize. If both will fit,
     * 	we can do atomic. Otherwise, we must do incremental {en,de}cryption
     */
    uint32_t reqBufSize = teeApiComm->getReqBufSize();
    uint32_t rspBufSize = teeApiComm->getRspBufSize();

    //printf("TeeApiCrypto::%s - reqBufSize is %d, rspBufSize is %d\n", __FUNCTION__, reqBufSize, rspBufSize);

    if ( ( reqBufSize < nfMIN_AES_CBC_ENCR_REQ_SIZE ) || ( rspBufSize < nfMIN_AES_CBC_ENCR_RSP_SIZE) )
    {
        // need larger TeeApiComm buffers.
        Log::error(TRACE_CRYPTO) << "TeeApiCrypto::aescbc: comm buffer(s) too small.";
        return NFErr_Internal;
    }

    if ( ( reqBufSize >= (nfMIN_AES_CBC_ENCR_REQ_SIZE + inLen) ) && ( rspBufSize >= maxRspLen ) )
    {
        /**
         * Create a DataBuffer with the contents for atomic aes encrypt/decrypt. Both
         * nfTeeAesCbcEncryptRequest_t and nfTeeAesCbcDecryptRequest_t have the following
         * fields:
         *
         * nfCryptoKeyHandle_t		KeyHandle;
         * uint8_t	    			IV[nfTEE_AES_IV_MAX];
         * uint32_t					DataLength;
         * uint8_t	    			Data[];
         *
         */

        //printf("TeeApiCrypto::%s - doing atomic encrypt\n", __FUNCTION__);
        DataBuffer aescbcReq;
        aescbcReq.append<uint32_t>(htonl(keyHandle));
        aescbcReq.append(ivBuf);
        if (nfTEE_AES_IV_MAX > ivBuf.size())
        {
            DataBuffer buf((nfTEE_AES_IV_MAX - ivBuf.size()), '\0');
            aescbcReq.append(buf);
        }
        aescbcReq.append<uint32_t>(htonl(inLen));
        aescbcReq.append(inBuf);

        // process the message
        uint32_t rspCode;
        DataBuffer rspData;
        teeApiComm->processMessage(op, aescbcReq, rspCode, rspData);

        // handle error case
        if (rspCode != nfTGR_SUCCESS)
        {
            Log::error(TRACE_CRYPTO) << "TeeApiCrypto::aescbc: TEE returned error " << getTeeResultStr(rspCode);
            outBuf.clear();
            return translateTeeError((nfTeeResult_t)rspCode);
        }

        // ensure there is expected data
        if (rspData.size() < (int) minRspLen)
        {
            Log::error(TRACE_CRYPTO) << "TeeApiCrypto::aescbc: TEE did not return response of correct size";
            outBuf.clear();
            return NFErr_Internal;
        }

        // read data
        unsigned char* rspDataStr = rspData.data();
        uint32_t dataLen = ntohl(*((uint32_t*)rspDataStr));
        outBuf.clear();
        outBuf.append((char *)(rspDataStr + sizeof(uint32_t)), dataLen);

        return NFErr_OK;
    }/* end if atomic */

    /**
     * If we get here, we have to do incremental crypto ops (init/update/final). We
     * want to do the following:
     *
     * 		- figure out the chunk size
     * 			- We have 2 buffers (in vs. out). Chunk size has to be such that
     * 			  data we send will fit in reqBuf, and response will fit in rspBuf
     * 			  - for encrypt, rspSize >= reqSize, so if we compute
     * 			    max rspSize, this can be used for both buffers, and
     * 			    chunkSize = min(reqSize, rspSize - maxPaddingSize)
     * 			  - for decrypt, rspSize <= reqSize, so if we compute max
     * 			    reqSize, chunkSize = reqSize
     * 		- send {en,de}cryptInit to allocate context, set IV
     * 			- store context handle upon return
     * 		- for each chunk
     * 			- send {en,de}cryptUpdate with context handle, next chunk of data
     * 			- save result
     * 		- send {en,de}cryptFinal with context
     * 			- if encrypt, get final result
     */

    uint32_t chunkSize;
    uint32_t maxReqLen;
    if (cipherOp == IWebCrypto::DOENCRYPT)
    {
        op = nfTO_CRYPTO_OP_AES_CBC_ENCRYPT_INIT;
        maxRspLen = rspBufSize - nfMIN_AES_CBC_ENCRYPT_UPDATE_RSP_SIZE;
        maxReqLen = reqBufSize - nfMIN_AES_CBC_ENCRYPT_UPDATE_REQ_SIZE;
        if ( maxReqLen >= maxRspLen )
            chunkSize = maxRspLen;
        else
            chunkSize = maxReqLen;

        //printf("TeeApiCrypto::%s - op is encrypt, chunkSize is %d\n", __FUNCTION__, chunkSize);
    }
    else if (cipherOp == IWebCrypto::DODECRYPT)
    {
        op = nfTO_CRYPTO_OP_AES_CBC_DECRYPT_INIT;
        maxRspLen = rspBufSize - nfMIN_AES_CBC_DECRYPT_UPDATE_RSP_SIZE;
        maxReqLen = reqBufSize - nfMIN_AES_CBC_DECRYPT_UPDATE_REQ_SIZE;
        if ( maxReqLen >= maxRspLen )
            chunkSize = maxRspLen;
        else
            chunkSize = maxReqLen;

        //printf("TeeApiCrypto::%s - op is decrypt, chunkSize is %d\n", __FUNCTION__, chunkSize);
    }
    else
    {
        Log::error(TRACE_CRYPTO) << "TeeApiCrypto::aescbc: invalid cipherOp";
        return NFErr_BadParameter;
    }

    // now, send crypt-init and get context handle
    /*
     * Create a DataBuffer with the contents for aes encrypt/decrypt init. Both
     * nfTeeAesCbcEncryptInitRequest_t and nfTeeAesCbcDecryptInitRequest_t have the
     * following fields:
     *
     * nfCryptoKeyHandle_t		KeyHandle;
     * uint8_t	    			IV[nfTEE_AES_IV_MAX];
     *
     */

    DataBuffer aesReq;
    aesReq.append<uint32_t>(htonl(keyHandle));
    aesReq.append(ivBuf);
    if (nfTEE_AES_IV_MAX > ivBuf.size())
    {
        DataBuffer buf((nfTEE_AES_IV_MAX - ivBuf.size()), '\0');
        aesReq.append(buf);
    }

    // process the message
    uint32_t rspCode;
    DataBuffer rspData;
    teeApiComm->processMessage(op, aesReq, rspCode, rspData);

    // handle error case
    if (rspCode != nfTGR_SUCCESS)
    {
        Log::error(TRACE_CRYPTO) << "TeeApiCrypto::aescbc-init: TEE returned error " << getTeeResultStr(rspCode);
        outBuf.clear();
        return translateTeeError((nfTeeResult_t)rspCode);
    }

    // ensure there is expected data
    if (rspData.size() < nfTEE_CRYPTO_CONTEXT_HANDLE_SIZE)
    {
        Log::error(TRACE_CRYPTO) << "TeeApiCrypto::aescbc-init: TEE did not return response of correct size";
        outBuf.clear();
        return NFErr_Internal;
    }

    // save context handle
    nfTeeCryptoContextHandle_t contextHandle = *((nfTeeCryptoContextHandle_t *) rspData.data());

    //printf("TeeApiCrypto::%s - sent init, got handle\n", __FUNCTION__);

    // init variables for update loop
    int bytesLeft = inBuf.size();
    const unsigned char *dataPtr = inBuf.constData();
    int offset = 0;
    outBuf.clear();

    // previously, op was xxx_INIT -- change to xxx_UPDATE
    if ( op == nfTO_CRYPTO_OP_AES_CBC_ENCRYPT_INIT )
        op = nfTO_CRYPTO_OP_AES_CBC_ENCRYPT_UPDATE;
    else
        op = nfTO_CRYPTO_OP_AES_CBC_DECRYPT_UPDATE;


    // now, loop through inBuf, chunkSize bytes at a time
    do
    {
        /*
         * Each time through the loop:
         *  - if last chunk, update chunkSize to match remaining bytes
         * 	- get next chunk
         * 		- use databuffer methods to return databuffer containing chunk
         * 	- update offset, bytesLeft
         * 	- clear req buffer
         * 	- add context handle to req buffer
         * 	- add data length to req buffer
         * 	- add data to req buffer
         * 	- send req buffer to TEE
         * 	- get response, verify success
         * 	- append response to outBuf
         */

        // decrease chunkSize if final chunk is smaller
        if ( bytesLeft < (int) chunkSize )
            chunkSize = (uint32_t) bytesLeft;

        //printf("TeeApiCrypto::%s - chunkSize is %d, bytesLeft is %d\n", __FUNCTION__, chunkSize, bytesLeft);
        /*
         * updateReq looks like this:
         *
         * typedef struct
         *	{
         *		nfTeeCryptoContextHandle_t	CryptoContextHandle;
         *		uint32_t					DataLength;
         *		uint8_t	    				Data[];
         *	} nfTeeAesCbcEncryptUpdateRequest_t;
         *
         */
        aesReq.clear();
        aesReq.append((char *)&contextHandle, nfTEE_CRYPTO_CONTEXT_HANDLE_SIZE);
        aesReq.append<uint32_t>(htonl(chunkSize));
        aesReq.append(dataPtr + offset, chunkSize);

        // update housekeeping variables
        offset += chunkSize;
        bytesLeft -= chunkSize;

        // process the message
        rspCode = 0;
        rspData.clear();
        teeApiComm->processMessage(op, aesReq, rspCode, rspData);

        // get response into outbuf
        unsigned char* rspDataStr = rspData.data();
        uint32_t dataLen = ntohl(*((uint32_t*)rspDataStr));
        outBuf.append((char *)(rspDataStr + sizeof(uint32_t)), dataLen);

        //printf("TeeApiCrypto::%s - TEE returned %d bytes\n", __FUNCTION__, dataLen);

    } while ( bytesLeft > 0 );

    /*
     * When we get here, we're ready to send the final message. It looks like this:
     *
     * typedef struct
     *	{
     *		nfTeeCryptoContextHandle_t	CryptoContextHandle;
     *	} nfTeeAesCbcEncryptFinalRequest_t;
     *
     */
    aesReq.clear();
    aesReq.append((char *)&contextHandle, nfTEE_CRYPTO_CONTEXT_HANDLE_SIZE);

    // previously, op was xxx_UPDATE -- change to xxx_FINAL
    if ( op == nfTO_CRYPTO_OP_AES_CBC_ENCRYPT_UPDATE )
        op = nfTO_CRYPTO_OP_AES_CBC_ENCRYPT_FINAL;
    else
        op = nfTO_CRYPTO_OP_AES_CBC_DECRYPT_FINAL;

    // process the message
    rspCode = 0;
    rspData.clear();
    teeApiComm->processMessage(op, aesReq, rspCode, rspData);

    // get response into outbuf
    unsigned char* rspDataStr = rspData.data();
    uint32_t dataLen = ntohl(*((uint32_t*)rspDataStr));
    outBuf.append((char *)(rspDataStr + sizeof(uint32_t)), dataLen);

    //printf("TeeApiCrypto::%s - final returned %d bytes\n", __FUNCTION__, dataLen);

    return NFErr_OK;

}

NFErr TeeApiCrypto::dhKeyGen(DataBuffer generatorBuf,
                           DataBuffer primeBuf,
                           unsigned int keyUsage,
                           /*out*/uint32_t& pubKeyHandle,
                           /*out*/uint32_t& privKeyHandle)
{
    (void) keyUsage; // why is this here?

    if (!teeApiComm)
    {
        Log::error(TRACE_CRYPTO) << "TeeApiCrypto::dhKeyGen: teeApiComm is NULL";
        return NFErr_Uninitialized;
    }

  // get generator from databuf
    uint32_t	generatorValue;
    if ( generatorBuf.size() == 1 )
        generatorValue = (uint32_t) *((uint8_t *) generatorBuf.data());
    else if ( generatorBuf.size() == 2 )
        generatorValue = (uint32_t) *((uint16_t *) generatorBuf.data());
    else if ( generatorBuf.size() == 4 )
        generatorValue = *((uint32_t *) generatorBuf.data());
    else
    {
        Log::error(TRACE_CRYPTO) << "TeeApiCrypto::dhKeyGen: generator size is wrong (%d)" << generatorBuf.size();
        return NFErr_BadParameter;
    }

    /*
     * Create DataBuffer with nfTeeGenerateDHKeyRequest_t in it:
     *
     *	typedef struct
     *	{
     *		nfCryptoAlgorithm_t		Algorithm;
     *		nfKeyUsageFlagsWord_t	KeyUsageFlags;
     *		uint32_t				Generator;
     *		uint32_t				ModulusLength;
     *		uint8_t					Modulus[];
     *  } nfTeeGenerateDHKeyRequest_t;
     */

    DataBuffer dhKeyGenRequest;
    dhKeyGenRequest.append<uint32_t>(htonl(nfCA_NFLX_DH));
    dhKeyGenRequest.append<uint32_t>(htonl(nfKUF_DERIVE));
    dhKeyGenRequest.append<uint32_t>(htonl(generatorValue));
    dhKeyGenRequest.append<uint32_t>(htonl(primeBuf.size()));
    dhKeyGenRequest.append<const unsigned char>(primeBuf.data(), primeBuf.size());

    NTRACE(TRACE_CRYPTO, "TeeApiCrypto::dhKeyGen: calling dhKeyGen in TEE");
    uint32_t rspCode;
    DataBuffer rspData;
    teeApiComm->processMessage(nfTO_CRYPTO_OP_DH_GEN_KEYS, dhKeyGenRequest, rspCode, rspData);

    // handle error case
    if (rspCode != nfTGR_SUCCESS)
    {
        Log::error(TRACE_CRYPTO) << "TeeApiCrypto::dhKeyGen, TEE returned error " << getTeeResultStr(rspCode);
        return translateTeeError((nfTeeResult_t)rspCode);
    }

    // ensure there is data to read
    if (rspData.size() < (int) sizeof(nfTeeGenerateDHKeyResponse_t))
    {
        Log::error(TRACE_CRYPTO) << "TeeApiCrypto::dhKeyGen, TEE did not return the correct response";
        return NFErr_Internal;
    }

    pubKeyHandle = ntohl(((uint32_t*)(rspData.data()))[0]);
    privKeyHandle = ntohl(((uint32_t*)(rspData.data()))[1]);
    return NFErr_OK;
}

NFErr TeeApiCrypto::nflxDhDerive(uint32_t baseKeyHandle, const DataBuffer& peerPublicKeyData,
                               uint32_t derivationKeyHandle,
                               /*out*/uint32_t& encryptionKeyHandle,
                               /*out*/uint32_t& hmacKeyHandle,
                               /*out*/uint32_t& wrappingKeyHandle)
{
    if (!teeApiComm)
    {
        Log::error(TRACE_CRYPTO) << "TeeApiCrypto::nflxDhDerive: teeApiComm is NULL";
        return NFErr_Uninitialized;
    }

    /*
     * Create DataBuffer with nfTeeNflxDhDeriveKeyRequest_t in it:
     *
     * 	typedef struct
     * 	{
     *		nfCryptoKeyHandle_t	DHPrivKeyHandle;
     *		nfCryptoKeyHandle_t	KeyDerivationKeyHandle;
     *		uint32_t			PeerDHPubValueLength;
     *		uint8_t				PeerDHPubValue[];
     *	} nfTeeNflxDhDeriveKeyRequest_t;
     */

    uint32_t pubKeySize = peerPublicKeyData.size();
    DataBuffer dhDeriveRequest;
    dhDeriveRequest.append<nfCryptoKeyHandle_t>(htonl(baseKeyHandle));
    dhDeriveRequest.append<nfCryptoKeyHandle_t>(htonl(derivationKeyHandle));
    dhDeriveRequest.append<uint32_t>(htonl(pubKeySize));
    dhDeriveRequest.append<const unsigned char>(peerPublicKeyData.data(), pubKeySize);

    NTRACE(TRACE_CRYPTO, "TeeApiCrypto::nflxDhDerive: calling dhDerive in TEE");
    uint32_t rspCode;
    DataBuffer rspData;
    teeApiComm->processMessage(nfTO_CRYPTO_OP_NFLX_DH_DERIVE, dhDeriveRequest, rspCode, rspData);

    // handle error case
    if (rspCode != nfTGR_SUCCESS)
    {
        Log::error(TRACE_CRYPTO) << "TeeApiCrypto::nflxDhDerive, TEE returned error " << getTeeResultStr(rspCode);
        return translateTeeError((nfTeeResult_t)rspCode);
    }

    // ensure there is data to read
    if (rspData.size() < (int) sizeof(nfTeeNflxDhDeriveKeyResponse_t))
    {
        Log::error(TRACE_CRYPTO) << "TeeApiCrypto::nflxDhDerive, TEE did not return the correct response";
        return NFErr_Internal;
    }

    encryptionKeyHandle = ntohl(((uint32_t*)(rspData.data()))[0]);
    hmacKeyHandle = ntohl(((uint32_t*)(rspData.data()))[1]);
    wrappingKeyHandle = ntohl(((uint32_t*)(rspData.data()))[2]);

    return NFErr_OK;
}

netflix::NFErr TeeApiCrypto::translateTeeError (nfTeeResult_t teeResult)
{
    // just handle the success case first, this is the most likely value
    if (teeResult == nfTGR_SUCCESS)
        return NFErr_OK;

    NFErr nfErr = NFErr_Bad;
    if ((teeResult & nfMASK_RESULT_BASE) == nfCRYPTO_AGENT_RESULT_BASE)
    {
        nfTeeCryptoResult_t cryptoResult = (nfTeeCryptoResult_t) teeResult;
        switch (cryptoResult)
        {
            case  nfTCR_NOT_INITIALIZED:
                nfErr = NFErr_Uninitialized;
                break;
            case  nfTCR_KEY_STORE_FULL:
            case  nfTCR_OUT_OF_MEMORY:
                nfErr = NFErr_NoMemory;
                break;
            case  nfTCR_KEY_NOT_FOUND:
                nfErr = NFErr_NotFound;
                break;
            case  nfTCR_OPERATION_NOT_ALLOWED:
            case  nfTCR_ALREADY_INITIALIZED:
                nfErr = NFErr_NotAllowed;
                break;
            case  nfTCR_MALFORMED_DATA:
            case  nfTCR_BAD_PARAMETER:
            case  nfTCR_UNSUPPORTED_KEY_ENCODING:
            case  nfTCR_KEY_TYPE_ALG_MISMATCH:
            case  nfTCR_KEY_TYPE_FORMAT_MISMATCH:
            case  nfTCR_KEY_LENGTH_UNSUPPORTED:
            case  nfTCR_OPERAND_TOO_SHORT:
            case  nfTCR_UNKNOWN_KEY_TYPE:
            case  nfTCR_UNSUPPORTED_KEY_PARAMETER:
            case  nfTCR_UNKOWN_CLIENTID:
            case  nfTCR_INVALID_DH_GENERATOR:
            case  nfTCR_INVALID_KEY_HANDLE:
            case  nfTCR_SEALED_KEY_DATA_TOO_LARGE:
            case  nfTCR_SEALED_KEY_VERSION_UNSUPPORTED:
            case  nfTCR_BUFFER_TOO_SMALL:
                nfErr = NFErr_BadParameter;
                break;
            case  nfTCR_INTERNAL_ERROR:
            case  nfTCR_CIPHER_ERROR:
            case  nfTCR_HMAC_VERIFY_FAILED:
            case  nfTCR_IMPORT_ENC_KEY_FAILED:
            case  nfTCR_IMPORT_HMAC_KEY_FAILED:
            case  nfTCR_CRYPT_RANDOM_FAILURE:
            case  nfTCR_SEALED_KEY_VALIDATION_ERROR:
                nfErr = NFErr_Internal;
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

//TODO: assuming that the values of respective enums will not be same
//		if they are, then we can get rid of following helper functions that do the translation
nfCryptoAlgorithm_t TeeApiCrypto::convertAlgo(IWebCrypto::Algorithm algType)
{
    nfCryptoAlgorithm_t nfAlgType = nfCA_UNDEFINED;
    switch (algType)
    {
    case IWebCrypto::NFLX_DH:
        nfAlgType = nfCA_NFLX_DH;
        break;
    case IWebCrypto::HMAC:
        nfAlgType = nfCA_HMAC;
        break;
    case IWebCrypto::AES_CBC:
        nfAlgType = nfCA_AES_CBC;
        break;
    case IWebCrypto::AES_GCM:
        nfAlgType = nfCA_AES_GCM;
        break;
    case IWebCrypto::AES_CTR:
        nfAlgType = nfCA_AES_CTR;
        break;
    case IWebCrypto::RSAES_PKCS1_V1_5:
        nfAlgType = nfCA_RSAES_PKCS1_V1_5;
        break;
    case IWebCrypto::RSASSA_PKCS1_V1_5:
        nfAlgType = nfCA_RSASSA_PKCS1_V1_5;
        break;
    case IWebCrypto::RSA_OAEP:
        nfAlgType = nfCA_RSASSA_PKCS1_V1_5;
        break;
    case IWebCrypto::SHA1:
        nfAlgType = nfCA_SHA1;
        break;
    case IWebCrypto::SHA224:
        nfAlgType = nfCA_SHA224;
        break;
    case IWebCrypto::SHA256:
        nfAlgType = nfCA_SHA256;
        break;
    case IWebCrypto::SHA384:
        nfAlgType = nfCA_SHA384;
        break;
    case IWebCrypto::SHA512:
        nfAlgType = nfCA_SHA512;
        break;
    case IWebCrypto::AES_KW:
        nfAlgType = nfCA_AES_KW;
        break;
    case IWebCrypto::DH:
        nfAlgType = nfCA_DH_ALGO;
        break;
    case IWebCrypto::SYSTEM:
        nfAlgType = nfCA_SYSTEM;
        break;
    case IWebCrypto::ECC:
        // Invalid because we don't implement it here
        nfAlgType = nfCA_INVALID;
        break;
    case IWebCrypto::INVALID_ALGORITHM:
        nfAlgType = nfCA_INVALID;
        break;
    }
    return nfAlgType;
}

IWebCrypto::Algorithm TeeApiCrypto::convertAlgo(nfCryptoAlgorithm_t nfAlgType)
{
    IWebCrypto::Algorithm algType = IWebCrypto::INVALID_ALGORITHM;
    switch (nfAlgType)
    {
    case nfCA_NFLX_DH:
        algType= IWebCrypto::NFLX_DH;
        break;
    case nfCA_HMAC:
        algType = IWebCrypto::HMAC;
        break;
    case nfCA_AES_CBC:
        algType = IWebCrypto::AES_CBC;
        break;
    case nfCA_AES_GCM:
        algType = IWebCrypto::AES_GCM;
        break;
    case nfCA_AES_CTR:
        algType = IWebCrypto::AES_CTR;
        break;
    case nfCA_RSAES_PKCS1_V1_5:
        algType = IWebCrypto::RSAES_PKCS1_V1_5;
        break;
    case nfCA_RSASSA_PKCS1_V1_5:
        algType = IWebCrypto::RSASSA_PKCS1_V1_5;
        break;
    case nfCA_RSA_OAEP:
        algType = IWebCrypto::RSASSA_PKCS1_V1_5;
        break;
    case nfCA_SHA1:
        algType = IWebCrypto::SHA1;
        break;
    case nfCA_SHA224:
        algType = IWebCrypto::SHA224;
        break;
    case nfCA_SHA256:
        algType = IWebCrypto::SHA256;
        break;
    case nfCA_SHA384:
        algType = IWebCrypto::SHA384;
        break;
    case nfCA_SHA512:
        algType = IWebCrypto::SHA512;
        break;
    case nfCA_AES_KW:
        algType = IWebCrypto::AES_KW;
        break;
    case nfCA_DH_ALGO:
        algType = IWebCrypto::DH;
        break;
    case nfCA_SYSTEM:
        algType = IWebCrypto::SYSTEM;
        break;
    case nfCA_INVALID:
    case nfCA_UNDEFINED:
        algType = IWebCrypto::INVALID_ALGORITHM;
        break;
    }
    return algType;
}

nfKeyUsageFlagsWord_t TeeApiCrypto::convertWcUsage(unsigned int keyUsage)
{
    nfKeyUsageFlagsWord_t nfkeyUsage = 0;
    if (keyUsage & IWebCrypto::ENCRYPT)
    {
        nfkeyUsage |= nfKUF_ENCRYPT;
    }
    if (keyUsage & IWebCrypto::DECRYPT)
    {
        nfkeyUsage |= nfKUF_DECRYPT;
    }
    if (keyUsage & IWebCrypto::SIGN)
    {
        nfkeyUsage |= nfKUF_SIGN;
    }
    if (keyUsage & IWebCrypto::VERIFY)
    {
        nfkeyUsage |= nfKUF_VERIFY;
    }
    if (keyUsage & IWebCrypto::DERIVE)
    {
        nfkeyUsage |= nfKUF_DERIVE;
    }
    if (keyUsage & IWebCrypto::WRAP)
    {
        nfkeyUsage |= nfKUF_WRAP;
    }
    if (keyUsage & IWebCrypto::UNWRAP)
    {
        nfkeyUsage |= nfKUF_UNWRAP;
    }
    return nfkeyUsage;
}

unsigned int TeeApiCrypto::convertTeeUsage(nfKeyUsageFlagsWord_t nfKeyUsage)
{
    unsigned int keyUsage = 0;
    if (nfKeyUsage & nfKUF_ENCRYPT)
    {
        keyUsage |= IWebCrypto::ENCRYPT;
    }
    if (nfKeyUsage & nfKUF_DECRYPT)
    {
        keyUsage |= IWebCrypto::DECRYPT;
    }
    if (nfKeyUsage & nfKUF_SIGN)
    {
        keyUsage |= IWebCrypto::SIGN;
    }
    if (nfKeyUsage & nfKUF_VERIFY)
    {
        keyUsage |= IWebCrypto::VERIFY;
    }
    if (nfKeyUsage & nfKUF_DERIVE)
    {
        keyUsage |= IWebCrypto::DERIVE;
    }
    if (nfKeyUsage & nfKUF_WRAP)
    {
        keyUsage |= IWebCrypto::WRAP;
    }
    if (nfKeyUsage & nfKUF_UNWRAP)
    {
        keyUsage |= IWebCrypto::UNWRAP;
    }
    return keyUsage;
}

nfKeyFormat_t TeeApiCrypto::convertKeyFormat(IWebCrypto::KeyFormat keyFormat)
{
    nfKeyFormat_t nfkeyFormat = nfKF_UNDEFINED;

    switch (keyFormat)
    {
    case IWebCrypto::RAW:
        nfkeyFormat = nfKF_RAW;
        break;
    default:
        nfkeyFormat = nfKF_INVALID;
        break;
    }
    return nfkeyFormat;
}

IWebCrypto::KeyType TeeApiCrypto::convertKeyType(nfKeyType_t nfKeyType)
{
    IWebCrypto::KeyType keyType = IWebCrypto::SECRET; //TODO: the enum does not define a UNDEFINED or DEFAULT value

    switch (nfKeyType)
    {
    case nfKT_SECRET:
        keyType = IWebCrypto::SECRET;
        break;
    case nfKT_PUBLIC:
        keyType = IWebCrypto::PUBLIC;
        break;
    case nfKT_PRIVATE:
        keyType = IWebCrypto::PRIVATE;
        break;
    default:
        //todo: no invalid type defined in IWebCrypto::KeyType yet
        break;
    }
    return keyType;
}

void TeeApiCrypto::convertBnToB64(BIGNUM* bn, int maxBinStrLen, std::string &b64)
{
    if (bn != NULL)
    {
        std::vector<unsigned char> bin(maxBinStrLen);
        BN_bn2bin(bn, &bin[0]);
        char* binStr = reinterpret_cast<char*> (&bin[0]);
        b64.assign(Base64::encode(std::string(binStr), false));
    }
}

void TeeApiCrypto::convertB64ToBn(char* b64, BIGNUM* bn)
{
    if (NULL != b64)
    {
        std::string b64DecVal = Base64::decode(std::string(b64));
        const unsigned char* b64DecStr = reinterpret_cast<const unsigned char*> (b64DecVal.c_str());
        BN_bin2bn(b64DecStr, b64DecVal.length(), bn);
    }
}

std::string TeeApiCrypto::getEsn()
{
    if (!teeApiComm)
    {
        Log::error(TRACE_CRYPTO) << "TeeApiCrypto::getEsn: teeApiComm is NULL";
        return "";
    }

    // create empty DataBuffer since there is no data to pass for ESN
    DataBuffer getEsnRequest;

    // process the message
    NTRACE(TRACE_CRYPTO, "Getting ESN from TEE");
    uint32_t rspCode;
    DataBuffer rspData;
    teeApiComm->processMessage(nfTO_CRYPTO_OP_GET_ESN, getEsnRequest, rspCode, rspData);

    // handle error case
    if(rspCode != nfTGR_SUCCESS){
        Log::error(TRACE_CRYPTO) << "TeeApiCrypto::getEsn: Cannot get esn, TEE returned error " << getTeeResultStr(rspCode);
        return "";
    }

    // ensure there is data to read
    if (rspData.size() <= 0)
    {
        Log::error(TRACE_CRYPTO) << "TeeApiCrypto::getEsn: Cannot get esn, TEE returned NULL for response data";
        return "";
    }

    //read the data
    unsigned char* rspDataStr = rspData.data();
    uint32_t esnLength = (nfCryptoKeyHandle_t)(ntohl(*((uint32_t*)rspDataStr)));
    const char *tmpEsn=(const char *)(rspDataStr+sizeof(uint32_t));
    std::string esn(tmpEsn, esnLength);

    return esn;
}

NFErr TeeApiCrypto::exportSealedKey (nfCryptoKeyHandle_t keyHandle, /*out*/ DataBuffer& keyData)
{
    if (!teeApiComm)
    {
        Log::error(TRACE_CRYPTO) << "TeeApiCrypto::exportSealedKey: teeApiComm is NULL";
        return NFErr_Uninitialized;
    }

    /*
     * create DataBuffer for nfTeeExportSealedKeyRequest_t
     * {
     *   nfCryptoKeyHandle_t		KeyHandle;
     * } nfTeeExportSealedKeyRequest_t;
     */
    DataBuffer sealedKeyRequest;
    sealedKeyRequest.append<uint32_t>(htonl(keyHandle));

    // process the message
    NTRACE(TRACE_CRYPTO, "Exporting Sealed Key from TEE");
    uint32_t rspCode;
    DataBuffer rspData;
    teeApiComm->processMessage(nfTO_CRYPTO_OP_EXPORT_SEALED_KEY, sealedKeyRequest, rspCode, rspData);

    // handle error case
    if (rspCode != nfTGR_SUCCESS)
    {
        Log::error(TRACE_CRYPTO) << "TeeApiCrypto::exportSealedKey: Cannot export key, TEE returned error " << getTeeResultStr(rspCode);
        return translateTeeError((nfTeeResult_t)rspCode);
    }

    // ensure there is data to read
    if (rspData.size() < (int) nfMIN_EXPORT_SEALED_KEY_RSP_SIZE)
    {
        Log::error(TRACE_CRYPTO) << "TeeApiCrypto::exportSealedKey: Cannot export key, TEE did not return response of correct size";
        return NFErr_Internal;
    }

    //read the data
    unsigned char* rspDataStr = rspData.data();
    uint32_t dataLen = ntohl(*((uint32_t*)rspDataStr));
    keyData.clear();
    keyData.append((char *)(rspDataStr + sizeof(uint32_t)), dataLen);

    return NFErr_OK;

}// end exportSealedKey

NFErr TeeApiCrypto::importSealedKey (DataBuffer keyData, /*out*/ nfCryptoKeyHandle_t& keyHandle,
                                    /*out*/ nfKeyType_t& keyType, /*out*/nfCryptoAlgorithm_t& algorithm,
                                    /*out*/ nfKeyUsageFlagsWord_t& 	keyUsageFlags,
                                    /*out*/ uint32_t& keyBitCount )
{
    if (!teeApiComm)
    {
        Log::error(TRACE_CRYPTO) << "TeeApiCrypto::importSealedKey: teeApiComm is NULL";
        return NFErr_Uninitialized;
    }

    /*
     * create DataBuffer for nfTeeImportSealedKeyRequest_t
     * typedef struct
     * {
     *  uint32_t				DataLength;
     *  uint8_t	    			KeyData[//DataLength];
     * } nfTeeImportSealedKeyRequest_t;
     */
    DataBuffer sealedKeyRequest;
    sealedKeyRequest.append<uint32_t>(htonl(keyData.length()));
    sealedKeyRequest.append(keyData);

    // process the message
    NTRACE(TRACE_CRYPTO, "Importing Sealed Key from TEE");
    uint32_t rspCode;
    DataBuffer rspData;
    teeApiComm->processMessage(nfTO_CRYPTO_OP_IMPORT_SEALED_KEY, sealedKeyRequest, rspCode, rspData);

    // handle error case
    if (rspCode != nfTGR_SUCCESS)
    {
        Log::error(TRACE_CRYPTO) << "TeeApiCrypto::importSealedKey: Cannot import key, TEE returned error " << getTeeResultStr(rspCode);
        return translateTeeError((nfTeeResult_t)rspCode);
    }

    // ensure there is data to read
    if (rspData.size() < (int) nfMIN_IMPORT_SEALED_KEY_RSP_SIZE)
    {
        Log::error(TRACE_CRYPTO) << "TeeApiCrypto::importSealedKey: Cannot import key, TEE did not return response of correct size";
        return NFErr_Internal;
    }

    // copy the response values
    nfTeeImportSealedKeyResponse_t *sealedKey = (nfTeeImportSealedKeyResponse_t*) (rspData.data());
    keyHandle 		= (nfCryptoKeyHandle_t) ntohl(sealedKey->KeyHandle);
    keyType 		= (nfKeyType_t) ntohl(sealedKey->KeyType);
    algorithm 		= (nfCryptoAlgorithm_t)ntohl(sealedKey->Algorithm);
    keyUsageFlags 	= (nfKeyUsageFlagsWord_t)ntohl(sealedKey->KeyUsageFlags);
    keyBitCount 	= (uint32_t) ntohl(sealedKey->KeyBitCount);

    return NFErr_OK;
}// end importSealedKey

NFErr TeeApiCrypto::clearKeys(/*out*/std::set<uint32_t> & keyHandles)
{

    if (!teeApiComm)
    {
        Log::error(TRACE_CRYPTO) << "TeeApiCrypto::clearKeys: teeApiComm is NULL";
        return NFErr_Uninitialized;
    }

    /*
     * create DataBuffer for nfTeeClearKeysRequest_t
     *  (currently an empty message)
     */
    DataBuffer clearKeyRequest;

    // process the message
    NTRACE(TRACE_CRYPTO, "Importing Sealed Key from TEE");
    uint32_t rspCode;
    DataBuffer rspData;
    teeApiComm->processMessage(nfTO_CRYPTO_OP_CLEARKEYS, clearKeyRequest, rspCode, rspData);

    // handle error case
    if (rspCode != nfTGR_SUCCESS)
    {
        Log::error(TRACE_CRYPTO) << "TeeApiCrypto::clearKeys: Cannot clear keys, TEE returned error " << getTeeResultStr(rspCode);
        return translateTeeError((nfTeeResult_t)rspCode);
    }

    // ensure there is data to read
    if (rspData.size() < (int) nfMIN_CLEARKEYS_RSP_SIZE)
    {
        Log::error(TRACE_CRYPTO) << "TeeApiCrypto::clearKeys: Cannot clear keys, TEE did not return response of correct size";
        return NFErr_Internal;
    }

    // copy the response values
    nfTeeClearKeysResponse_t *clearedKeys = (nfTeeClearKeysResponse_t*) (rspData.data());

    const int handles = ntohl(clearedKeys->NumHandles);

    for (int i = 0; i < handles; ++i )
    {
        keyHandles.insert(ntohl(clearedKeys->DeletedHandles[i]));
    }

    return NFErr_OK;

}// end clearKeys
