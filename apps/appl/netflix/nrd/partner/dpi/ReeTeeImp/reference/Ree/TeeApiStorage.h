/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef TEEAPISTORAGE_H_
#define TEEAPISTORAGE_H_

#include <nrdbase/DataBuffer.h>
#include <nrddpi/TeeApiComm.h>
#include <nrddpi/StorageTypes.h>
#include <pthread.h>
#include <nrdbase/tr1.h>
#include <nrdbase/NFErr.h>
#include <string>
#include <memory>

/**
 * @class 	TeeApiStorage
 * @brief 	a singleton class with member functions that provide TEE storage functionality
 *          like loadProtected, storeProtected
 */
namespace netflix {
class TeeApiStorage
{
public:
    /**
     * constructor
     *
     */
    TeeApiStorage();

    /**
     * destructor
     */
    ~TeeApiStorage();

    /**
     * @function init
     * @brief    initializes the TeeApiStorage object. This allows for error handling at the calling
     *           site in case of an initialization error. This method *must* be called before any other
     *           method can be called.
     *
     * @param    (in)maxChunkSize, maximum size expected to be sent to that instance for storage and loading.
     * @return   netflix::NFerr error, 0 on success, <0 on failure
     */
    netflix::NFErr init(uint32_t maxChunkSize);

    /*
     * @function  storeProtected
     * @brief 	  encrypts/integrity protects data (returns b64-encoded string)
     *
     * @param	  clearData, data to protect
     * @param	  (out) protectedData, protected data will be stored here
     * @return 	  int error, 0 on success, <0 on failure
     *
     */
    netflix::NFErr storeProtected(DataBuffer& ClearData, DataBuffer& protectedData);

    /*
     * @function  loadProtected
     * @brief 	  decrypts/verifies protected data (returns b64-encoded string)
     *
     * @param	  protectedData, protected data (b64-encoded string)
     * @param	  (out) clearData, unprotected data
     * @return 	  int error, 0 on success, <0 on failure
     *
     */
    netflix::NFErr loadProtected(DataBuffer& protectedData, DataBuffer& clearData);

private:
    shared_ptr<TeeApiComm> m_teeApiComm;
    bool                        m_isInited;
};
}

#endif /* TEEAPISTORAGE_H_ */
