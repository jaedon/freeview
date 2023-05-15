/* -*- Mode: c; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
 * Copyright (C) 1995-2010 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#if defined OIPF && defined JLABS

#ifndef __NATIVE_JLABS_EMMMESSAGEMANAGER
#define __NATIVE_JLABS_EMMMESSAGEMANAGER

#include "NativeOOIFTypes.h"
#include "NativeJlabsEmmMessage.h"
#include "NativeJlabsBoardMessage.h"
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef enum {
	JLABS_EMM_MESSAGE_EMM = 0,
	JLABS_EMM_MESSAGE_SELF = 1,
} JlabsEmmMessageType;

OOIFReturnCode native_jlabsEmmMessageMgrStopAutoDispMsg(int id, int *pResponse);
OOIFReturnCode native_jlabsEmmMessageMgrGetAllEmmMsg(int *count, jlabsEmmMessageHandle**handles);
OOIFReturnCode native_jlabsEmmMessageMgrGetAllSelfMsg(int *count, jlabsEmmMessageHandle**handles);
OOIFReturnCode native_jlabsEmmMessageMgrGetAllBoardMsg(int *count, jlabsBoardMessageHandle**handles);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __NATIVE_JLABS_EMMMESSAGECOLLECTION */

#endif

