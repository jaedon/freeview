/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef COMMTYPES_H_
#define COMMTYPES_H_

#include <arpa/inet.h> // for htonl, etc.
#include "CommonIncludes.h"

/*
 * REE application ID, currently used only internally in TEE
 */
#define nfTEE_CLIENT_ID_SIZE	16
typedef uint8_t nfTeeClientId_t[nfTEE_CLIENT_ID_SIZE];

/*
 * TEE services are exposed as "operations". Operation definitions are below.
 */

typedef uint32_t nfTeeOperation_t;

/*
 * For each operation, there are zero or more "operands". Operands
 * may be passed by reference, or as immediate data. In our initial
 * implementation, we will always pass operands as immediate data,
 * in a message.
 *
 * Messages between the TEE and REE are protocol messages, and
 * resemble network protocol messages. The outermost layer of
 * these messages represent the TEE IPC/IVC (inter-process/
 * inter-virtual machine communications) API.
 *
 * Since the TEE is implemented as a library in this initial
 * implementation, our IPC/IVC mechanism is very simple:
 *
 * 		- every transaction consists of a request (from the REE)
 * 		  and a reply (from the TEE)
 * 		  -  you can think of this as a reliable datagram service
 * 		  -  if your application requires multiple messages for some
 * 		     transactions, it is up to you to implement fragmentation/
 * 		     reassembly and session management on top of this request/
 * 		     response exchange
 *
 * 		- The outer header of the request contains the operation and the message
 * 		  length
 *
 *  	- reply contents depend on the operation. Because we are explicitly linking
 *  	   the request and response, there is no need to include the operation in
 *  	   the reply (just look at the associated request buffer). A "real" TEE would
 *  	   have to include a session header for this purpose.
 *
 * 		- The outer header of the reply contains only the ResponseCode and
 * 		  message length. Any additional data following this header is
 * 		  operation-specific, i.e. each TA will define its own operation headers
 * 		  and data.
 *
 */


typedef struct
{
	nfTeeOperation_t	Operation; // operation definitions are below
	uint32_t			OperandLength; // length of message following this header
} nfTeeApiReqHdr_t;

// generic response header, all responses begin with ResponseCode
typedef struct
{
	uint32_t	ResponseCode;
	uint32_t	ResponseLength;
} nfTeeApiRspHdr_t;

// minimum req/rsp buffer sizes
#define nfTEE_MIN_REQ_SIZE (sizeof(nfTeeApiReqHdr_t))
#define nfTEE_MIN_RSP_SIZE (sizeof(nfTeeApiRspHdr_t))

typedef struct
{
	uint32_t	DataLength;
	uint8_t		Data[];
} nfTeeDataBuffer_t;

#define nfTEE_MIN_DATABUFFER_SIZE	(sizeof(nfTeeDataBuffer_t))

/*
 * For TEE safety, we pass serialized messages for most operations. In that
 * case, the first two parms in the vector point to InMsg/OutMsg
 *
 * Serialized message params:
 *
 * 		param[0] = {nfPARAMETER_MEMORY_REFERENCE_TYPE, {InMsgPtr,size}}
 * 		param[1] = {nfPARAMETER_MEMORY_REFERENCE_TYPE, {OutMsgPtr,size} }
 */

#define nfTEE_SERIALIZEDMSG_REQ_PARAM_COUNT 2

/*
 * TEE Operation definitions: TEE applications (TAs) each expose their own APIs
 * in terms of "operations". We want to be able to quickly dispatch operation
 * messages to the appropriate TA, so we'll divide up the 32-bit operation space,
 * allocating some number of "slots" to each TA. We're arbitrarily choosing 2048
 * per application, but we can always grow/shrink this in the future if that
 * makes sense.
 */

// operations for each module (cryptoAgent, storageAgent, etc.) are >= base for that module
typedef enum
{
	nfTOB_UNDEFINED = 0,

	/*
	 * Some messages are for TEE (or VM) management. Those start at offset 0
	 */
	nfTOB_TEE_MGMT = 1,

	/*
	 * We're not implementing REE/TEE session management in the first revision, but we'll
	 * need it later, so let's allocate a slot for that.
	 */
    nfTOB_TEE_SESSION = 2048,

    /*
     * The first TA we'll implement is the crypto agent. This agent is used for all
     * webcrypto/msl operations, and also can provide secure store crypto services.
     */
	nfTOB_CRYPTO_AGENT = 4096,

	/*
	 * PlayReady integration is a stretch goal for the first revision, so let's
	 * allocate a slot for it.
	 */
	nfTOB_DRM_AGENT = 6144,

	/*
	 * The storage agent only manages the manufacturing secure store
	 * in the first release.
	 */
	nfTOB_STORAGE_AGENT = 8192,

} nfTeeOperationBase_t;

// TEE mgmt operations (e.g. teeInit() are nrdTOB_TEE_MGMT + offset)
// TEE session operations are (nrdTOB_TEE_SESSION + offset)
// crypto operations are (nrdTOB_CRYPTO_AGENT + offset)
// playready operations are (nrdTOB_PLAYREADY_AGENT + offset)

/*
 * TEE API communications are in network byte order, because not all TEEs
 * and REEs share the same processor architecture type. In addition, some
 * processors do not allow unaligned accesses to memory locations, so anywhere
 * we need to read/write unaligned data, we need to deal with this. The
 * following macros are for this purpose, but you should define assembly
 * functions for this.
 */

// TBD: these should be inline functions; also, define ntohl, htonl
#define nfPUTWORD32(ADDR, VALUE) \
	do { *((uint8_t *)(ADDR)    ) = (((VALUE) & 0xFF000000) >> 24); \
	     *((uint8_t *)(ADDR) + 1) = (((VALUE) & 0x00FF0000) >> 16); \
	     *((uint8_t *)(ADDR) + 2) = (((VALUE) & 0x0000FF00) >>  8); \
	     *((uint8_t *)(ADDR) + 3) = (((VALUE) & 0x000000FF)      ); } while(0)

#define nfPUTWORD16(ADDR, VALUE) \
	do { (*((uint8_t *)(ADDR)) = ((VALUE) & 0xFF00)) >> 8; \
	     (*((uint8_t *)(ADDR) + 1) = ((VALUE) & 0x00FF)); } while(0)

#define nfGETWORD32(ADDR, VALUE) \
	do { (VALUE)  = *((uint8_t *)(ADDR)    ) << 24; \
	     (VALUE) |= *((uint8_t *)(ADDR) + 1) << 16; \
	     (VALUE) |= *((uint8_t *)(ADDR) + 2) <<  8; \
	     (VALUE) |= *((uint8_t *)(ADDR) + 3)     ; } while(0)

#define nfGETWORD16(ADDR, VALUE) \
	do { VALUE = *((uint8_t *)(ADDR)) << 8; \
		 VALUE |= *((uint8_t *)(ADDR) + 1); } while(0)

#endif /* COMMTYPES_H_ */



