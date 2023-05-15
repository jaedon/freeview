// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2010 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifdef OIPF
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jsplugin.h"
#include "macros.h"
#include "Exception.h"
#include "HCasUi.h"
#include "EmptyEvent.h"

#include "oplcas.h"

using namespace Ooif;

void* HCasUi::HCasUiIdentifier = NULL;

HCasUi::HCasUi() : EventTarget()
{
	ENTRY;

	HCasUiIdentifier = this;
	OPL_CasUi_RegisterListener((HUINT32)HCasUiIdentifier, HCasUi::onCasUiEvent);
}

HCasUi::~HCasUi()
{
	ENTRY;

	OPL_CasUi_UnRegisterListener((HUINT32)HCasUiIdentifier);
	HCasUiIdentifier = NULL;
}

int HCasUi::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;

	GET_FUNCTION(openSession, obj, "nnns", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(closeSession, obj, "nnnn", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(sendUiResponse, obj, "nnnns", JSP_GET_VALUE_CACHE);

	return JSP_GET_NOTFOUND;
}

int HCasUi::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
	INTRINSIC_EVENT(CasUiEvent);
	return JSP_PUT_NOTFOUND;
}

int HCasUi::openSession(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	Handle_t hSession;

	ENTRY;
	CHECK_ARGUMENT_COUNT_MIN_MAX(3, 4);

	const int nModule = (int)NUMBER_VALUE(argv[0]);
	const int nSlot = (int)NUMBER_VALUE(argv[1]);
	DxCAS_MmiSessionType_e eSessionType = (DxCAS_MmiSessionType_e)NUMBER_VALUE(argv[2]);
	const char* pcParamData = NULL;

	if(argc == 3)
	{
	          pcParamData = NULL;
	}
	else
	{
	          pcParamData = (char *)STRING_VALUE(argv[3]);
	}

	OOIF_LOG_DEBUG("[openSession] module %d slot %d sessionType %d\n", nModule, nSlot, eSessionType);
	hSession = OPL_CasUi_OpenSession(nModule, nSlot, eSessionType, pcParamData);

	RETURN_NUMBER((int)hSession, JSP_CALL_VALUE);
}


int HCasUi::closeSession(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(4);

	const int nModule = (int)NUMBER_VALUE(argv[0]);
	const int nSlot = (int)NUMBER_VALUE(argv[1]);
	DxCAS_MmiSessionType_e eSessionType = (DxCAS_MmiSessionType_e)NUMBER_VALUE(argv[2]);
	const Handle_t hSession = (int)NUMBER_VALUE(argv[3]);

	OOIF_LOG_DEBUG("[closeSession] module %d slot %d sessionType %d\n", nModule, nSlot, eSessionType);
	OPL_CasUi_CloseSession(nModule, nSlot, eSessionType, hSession);

	return JSP_CALL_NO_VALUE;
}

int HCasUi::sendUiResponse(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(5);

	const int nModule = (int)NUMBER_VALUE(argv[0]);
	const int nSlot = (int)NUMBER_VALUE(argv[1]);
	DxCAS_MmiSessionType_e eSessionType = (DxCAS_MmiSessionType_e)NUMBER_VALUE(argv[2]);
	Handle_t hSession = (int)NUMBER_VALUE(argv[3]);
	const char* pcInputData = (char *)STRING_VALUE(argv[4]);

	if(pcInputData != NULL)
	{
		OOIF_LOG_DEBUG("[sendUiResponse] module %d slot %d sessionType %d\n", nModule, nSlot, eSessionType);
		OPL_CasUi_SendUiResponse(nModule, nSlot, eSessionType, hSession, pcInputData);
	}

	return JSP_CALL_NO_VALUE;
}

//static
int HCasUi::onCasUiEvent(int module, int slot, int type, int session, int eventType, void *pvData)
{
	ENTRY;

	HCasUi *casUiHandle = NULL;
	if ( NULL == HCasUiIdentifier )
	{
		return -1;
	}
	casUiHandle = (HCasUi *)HCasUiIdentifier;

	jsplugin_obj *context = casUiHandle->getHost();
	const int nModule = (const int)module;
	const int nSlot = (const int)slot;
	const int nStype = (const int)type;
	const int nSession = (const int)session;
	const int nEventType = (const int)eventType;
	const char* pcJsonData = (const char *)pvData;
	HCasUiEvent *e = new HCasUiEvent(context, nModule, nSlot, nStype, nSession, nEventType, pcJsonData);
	casUiHandle->dispatchEvent(e);

	return 0;
}


/***********************************************************************************
 *  HCasUiEvent Class
 ***********************************************************************************
 */
HCasUiEvent::HCasUiEvent( jsplugin_obj *c, OOIFNumber _module, OOIFNumber _slot, OOIFNumber _sType, OOIFNumber _sSession, OOIFNumber _sEventType, const char* _uiData)
	: Event(c, "CasUiEvent"),
	module(_module),
	slot(_slot),
	sType(_sType),
	sSession(_sSession),
	sEventType(_sEventType)
{
	ENTRY;
	this->uiData = (_uiData!=NULL) ? HLIB_STD_StrDup(_uiData) : NULL;
}

HCasUiEvent::~HCasUiEvent()
{
	ENTRY;
	if(this->uiData != NULL)
	{
		HLIB_STD_MemFree(this->uiData);
	}
}

int HCasUiEvent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
	return JSP_GET_NOTFOUND;
}

int HCasUiEvent::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
	return JSP_GET_NOTFOUND;
}

//virtual
int HCasUiEvent::serialize(jsplugin_value **argv)
{
	ENTRY;
	*argv = new jsplugin_value[6];

	jsplugin_value *result = *argv;

	result[0].type = JSP_TYPE_NUMBER;
	result[0].u.number = module;

	result[1].type = JSP_TYPE_NUMBER;
	result[1].u.number = slot;

	result[2].type = JSP_TYPE_NUMBER;
	result[2].u.number = sType;

	result[3].type = JSP_TYPE_NUMBER;
	result[3].u.number = sSession;

	result[4].type = JSP_TYPE_NUMBER;
	result[4].u.number = sEventType;

	result[5].type = JSP_TYPE_STRING;
	result[5].u.string = uiData;

	return 6;
}

#endif // OIPF

/* EOF */

