/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef TEEAPICOMM_H_
#define TEEAPICOMM_H_

#include <pthread.h>

#include <nrdbase/tr1.h>
#include <nrdbase/Mutex.h>
#include <nrdbase/DataBuffer.h>
#include <nrddpi/CommTypes.h>
#include <nrddpi/IMessage.h>

#define MAX_BUFFER_SIZE 1024*512	//each buffer can be half an MB in size
#define MAX_REQ_DATA_SIZE (MAX_BUFFER_SIZE - nfTEE_MIN_REQ_SIZE)
#define MAX_RSP_DATA_SIZE (MAX_BUFFER_SIZE - nfTEE_MIN_RSP_SIZE)

//TODO: Error handling of functions
/**
 * @class 	TeeApiComm
 * @brief 	a singleton class with member functions to ProcessMessage to/from TEE
 * 			It also maintains a mutex for send and receive buffers
 */
class TeeApiComm
{
public:

    /**
     * static accessor function to get shared pointer to instance of TeeApiComm
     */
    static shared_ptr<TeeApiComm> getInstance();

    /**
     * destructor
     */
    ~TeeApiComm();

    /*
     * @function  processMessage
     * @brief 	  sends message to TEE, gets the response and passes it back up
     *
     * @param 	  (out) rspCode, error code returned by Tee
     * @param 	  (out) rspData, DataBuffer containing response data from TEE.
     * @param 	  (in) op, operation to be performed by Tee
     * @param 	  (in) opData, DataBuffer containing relevant data for the operation.
     *
     * @return 	  0 on success, <0 on error
     */
    int processMessage (nfTeeOperation_t op, netflix::DataBuffer &opData,
                        uint32_t &rspCode, netflix::DataBuffer &rspData);

	/**
	 * req/rsp buffer size getters
	 */
	uint32_t getReqBufSize();
	uint32_t getRspBufSize();

	/*
	 * @function  allocateMemory
	 * @brief 	  allocates shared mem for use in current session
	 *
	 * @param 	  (out) memRefPtr, memory
	 * @param 	  (in) memSize, size of memory to allocation
	 * @param	  (in) memType, memory type
	 *
	 * @return 	  0 on success, <0 on error
	 */
	int allocateMemory (nfMemoryReference_t *memRefPtr, uint32_t memSize,
			nfMemoryReferenceReadWriteType_t memType );

	/*
	 * @function  freeMemory
	 * @brief 	  dereferences memory
	 *
	 * @param 	  (in) memRefPtr, memory
	 *
	 * @return 	  0 on success, <0 on error
	 */
	int freeMemory (nfMemoryReference_t *memRefPtr);


private:
    /**
     * static mutex for getInstance() function
     */
    static netflix::Mutex instanceMutex;

    /**
     * private constructor since TeeApiComm is a singleton
     */
    TeeApiComm();

    /**
     * private copy operation since TeeApiComm is a singleton
     * Not implemented
     */
    TeeApiComm(TeeApiComm const& teeApiComm);

    /**
     * private assignment since TeeApiComm is a singleton
     * Not implemented
     */
    TeeApiComm& operator=(TeeApiComm const& teeApiComm);

    /*
     * TeeApiComm is a singleton, therefore maintain a static instance of self
     */
    static shared_ptr<TeeApiComm> teeApiComm_;

    /**
     * Request and Response buffer pointers used to pass messages around
     * Each pointer will be allocated MAX_BUFFER_SIZE size of
     */
    uint8_t * reqBufPtr;
    uint8_t * rspBufPtr;

    /**
     * Request/Response buffer sizes
     */
    uint32_t reqBufSize;
    uint32_t	rspBufSize;

    nfConnection_t conn;
    nfSession_t session;

	// mutex to serialize access to TEE
    mutable netflix::Mutex mutex_;
};

#endif /* TEEAPICOMM_H_ */



