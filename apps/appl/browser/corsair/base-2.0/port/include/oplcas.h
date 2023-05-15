/**************************************************************************************/
/**
 * @file oplcas.h
 *
 * Oplcas
 *
 * @author  Kyungkoo Kim(kkkim@humaxdigital.com)
 * @date    2012/01/04
 * @attention   Copyright (c) 2011 Humax - All rights reserved.
 *
 **************************************************************************************
 **/

#ifndef __OPLCAS_H__
#define __OPLCAS_H__

#include "bpltype.h"
#include "dlib_cas.h"
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/******************************************************************
	Typedef
******************************************************************/

typedef int (*oplCasUiEventCallback)(int module, int slot, int type, int session, int eventType, void *pstData);

BPLStatus	OPL_CasUi_SendUiResponse(const int nModule, const int nSlot, DxCAS_MmiSessionType_e eType, Handle_t hSession, const char *pcInputData);
BPLStatus	OPL_CasUi_CloseSession(const int nModule, const int nSlot, DxCAS_MmiSessionType_e eType, Handle_t hSession);
Handle_t	OPL_CasUi_OpenSession(const int nModule, const int nSlot, DxCAS_MmiSessionType_e eType, const char *pcParamData);
BPLStatus	OPL_CasUi_UnRegisterListener(HUINT32 objId);
BPLStatus	OPL_CasUi_RegisterListener(HUINT32 objId, oplCasUiEventCallback cbFunc);

#ifdef __cplusplus
};
#endif // __cplusplus

#endif // __OPLCAS_H__

/* EOF */

