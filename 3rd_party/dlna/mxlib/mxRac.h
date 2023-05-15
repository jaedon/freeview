
/*******************************************************************************
 * HUMAX R&D Lab., HUMAX INC., SEOUL, KOREA
 * COPYRIGHT(c) 2010 by HUMAX Inc.
 * 
 * All rights reserved. No part of this work may be reproduced, stored in a
 * retrieval system, or transmitted by any means without prior written
 * permission of HUMAX Inc.
 ******************************************************************************/



#ifndef ____RACLIENT__H____ 
#define ____RACLIENT__H____ 

#ifdef __cplusplus
extern "C"{
#endif

#include "mxkernel.h"
#include "mxRa.h"

#define RAC_VERSION_LENGTH	12
#define RAC_VERSION			"V2.0"

//////////////////////////////////////////////////////////////////
// STRUCT
//////////////////////////////////////////////////////////////////
// mxRac의 전체 구조체 이다.
typedef struct _MXRAC_DATA {
	void *hWidMgr;
//	MXKERNEL_CRITICALSECTION cs;
	MXRA_USER_EVENT_CALLBACK rac_user_callback;
} MXRAC_DATA, *MXRAC_HANDLE;


// RAC를 생성한다.
__MXAPI 
MXRAC_HANDLE mxRac_init(
	char *connection_ip, int connection_port, 
	MXRA_USER_EVENT_CALLBACK rac_user_callback
);

// RAC를 정리한다 
__MXAPI 
int mxRac_deinit(MXRAC_HANDLE handle);

// WID 정보를 가져온다.
__MXAPI 
int mxRac_invokeQueryWid(MXRAC_HANDLE handle, char *pairingID, char *wid, void *user);

#ifdef __cplusplus
}
#endif


#endif /* ____RACLIENT__H____ */
