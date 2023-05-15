/**************************************************************************************/
/**
 * @file oplcasbc.h
 *
 * Oplcasbc
 *
 * @author  sunghoon Lee(shlee3@humaxdigital.com)
 * @date    2012/08/13
 * @attention   Copyright (c) 2011 Humax - All rights reserved.
 *
 **************************************************************************************
 **/

#ifndef __OPLCASBC_H__
#define __OPLCASBC_H__

#include "bpltype.h"
#include "oplbroadcast.h"
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/******************************************************************
	Typedef
******************************************************************/


BPLStatus	OPL_CAS_BC_ReleaseAutoDispMsg(OPL_AutoDispMsg_t *pDispMsg);
BPLStatus	OPL_CAS_BC_GetAutoDispMsgId(OPL_AutoDispMsg_t *pDispMsg, HUINT32 *pMsgId);
BPLStatus	OPL_CAS_BC_GetAutoDispMsgType(OPL_AutoDispMsg_t *pDispMsg, HUINT32 *pMsgType);
BPLStatus	OPL_CAS_BC_StopAutoDispMsg(HINT32 msgId, HINT32 *pResponse);

#ifdef __cplusplus
};
#endif // __cplusplus

#endif // __OPLCASBC_H__

/* EOF */


