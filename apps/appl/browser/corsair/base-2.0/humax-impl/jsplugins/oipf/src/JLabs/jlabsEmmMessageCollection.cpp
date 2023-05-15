/**************************************************************************************/
/**
 * @file jlabsEmmMessageCollection.cpp
 *
 * jlabsEmmMessageCollection module
 *
 * @author  sung-hoon Lee(shlee3@humaxdigital.com)
 * @date    2012/03/16
 * @attention   Copyright (c) 2011 Humax - All rights reserved.
 *
 **************************************************************************************
 **/

#if defined OIPF && defined JLABS

#include "jlabsEmmMessageCollection.h"
#include "NativeJlabsEmmMessageCollection.h"
#include "jlabsEmmMessage.h"
#include "Exception.h"

#include "jsplugin.h"
#include "macros.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace Ooif;

jlabsEmmMessageCollection::jlabsEmmMessageCollection(int count, jlabsEmmMessageHandle*handles)
{
	ENTRY;

	classType = CLASS_JLABS_EMM_MESSAGE_COLLECTION;
	for (int i=0; i<count; i++) {
		addItem(new jlabsEmmMessage(handles[i]));
	}
	HLIB_STD_MemFree(handles);
}

jlabsEmmMessageCollection::jlabsEmmMessageCollection()
{
	ENTRY;
	classType = CLASS_JLABS_EMM_MESSAGE_COLLECTION;
}

jlabsEmmMessageCollection::~jlabsEmmMessageCollection()
{
	ENTRY;
}

int jlabsEmmMessageCollection::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;

	int ret = Collection<jlabsEmmMessage>::getter(obj, name, result);

	if (ret != JSP_GET_NOTFOUND)
		return ret;

	GET_FUNCTION(deleteMsg, obj, "n", JSP_GET_VALUE);
	GET_FUNCTION(readDone, obj, "n", JSP_GET_VALUE);

	return JSP_GET_NOTFOUND;
}


//static
int jlabsEmmMessageCollection::deleteMsg(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
									int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;

	CHECK_ARGUMENT_COUNT(1);
	jlabsEmmMessageCollection *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, jlabsEmmMessageCollection);
	int msgId = NUMBER_VALUE(argv[0]);
	int idx = 0, retVal = -1;
	OOIFNumber tempId =0;

	for(idx = 0; idx < instance->getSize() ; idx++)
	{
		native_jlabsEmmMessageGetMsgId( (jlabsEmmMessageHandle)instance->internalGetItem(idx)->getIdentifier() , &tempId );

		if(msgId == (int)tempId)
		{
			break;
		}
	}

	if(idx <instance->getSize() )
	{
		retVal = native_jlabsEmmMessageDelete( msgId );

		delete instance->internalGetItem(idx);
		instance->internalRemoveIndex(idx);
	}

	RETURN_NUMBER(retVal, JSP_CALL_VALUE);
}

//static
int jlabsEmmMessageCollection::readDone(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	int idx = 0, retVal = -1;
	CHECK_ARGUMENT_COUNT(1);
	jlabsEmmMessageCollection *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, jlabsEmmMessageCollection);
	int msgId = NUMBER_VALUE(argv[0]);
	jlabsEmmMessageHandle handle = 0;
	OOIFNumber tempId =0;

	for(idx = 0; idx < instance->getSize() ; idx++)
	{
		handle = (jlabsEmmMessageHandle)instance->internalGetItem(idx)->getIdentifier();
		native_jlabsEmmMessageGetMsgId( handle , &tempId );

		if(msgId == (int)tempId)
		{
			break;
		}
	}
	if(idx <instance->getSize() )
	{
		retVal = native_jlabsEmmMessageReadDone(handle);
	}
	RETURN_NUMBER(retVal, JSP_CALL_VALUE);
}

#endif // OIPF && PVR_SUPPORT

