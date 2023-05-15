/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef MGMTTYPES_H_
#define MGMTTYPES_H_

#include "CommTypes.h"

/*
 * Tee mgmt messages
 */

typedef enum
{
	nfTO_MGMT_OP_INIT_TEE = nfTOB_TEE_MGMT + 1,
	nfTO_MGMT_OP_SHUTDOWN,
	nfTO_MGMT_OP_DEBUG,

	// insert new message types here...

    nfTO_MGMT_OP_INVALID // leave this last!!
} nfTeeOperation_MgmtOp_t;

#define nfTee_IS_MGMT_OP(x) (((x) > nfTOB_TEE_MGMT) && ((x) < nfTO_MGMT_OP_INVALID))


/*
 * nfTO_MGMT_OP_INIT_TEE may contain some additional data;
 * that data is defined in nfTeeInitRequest_t. Currently,
 * the only data is debug string, which allows enabling
 * specific types of debug trace.
 */
typedef struct
{
	uint32_t 	DebugFlags;
	uint32_t	ManufSSLength;
	uint8_t		ManufSS[];
} nfTeeInitRequest_t;

// XXX current init request must have path, but it may be only 1 char (/)
#define nfMIN_INIT_REQ_SIZE sizeof(nfTeeInitRequest_t)

// new init request
/*
 * New API: nfTeeInitRequest_t replacement:
 *
 * 		param[0] = {nfPARAMTER_VALUE_TYPE, {value.a=defaultInitFlags,value.b=0}
 * 		param[1] = {nfPARAMETER_MEMORY_REFERENCE_TYPE, {manufSS} }
 *
 */

#define nfTEE_INIT_REQ_PARAM_COUNT 2


typedef struct
{
	uint32_t 	DebugFlags;
} nfTeeDebugRequest_t;

#define nfMIN_DEBUG_REQ_SIZE sizeof(nfTeeDebugRequest_t)

/*
 typedef struct
 {
  <empty>
  } nfTeeDebugRsp_t;
 */

#endif /* MGMTTYPES_H_ */
