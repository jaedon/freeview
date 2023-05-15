/**************************************************************************************/
/**
 * @file jlabsEmmMessageManager.cpp
 *
 * jlabsEmmMessageManager module
 *
 * @author  sung-hoon Lee(shlee3@humaxdigital.com)
 * @date    2012/03/16
 * @attention   Copyright (c) 2011 Humax - All rights reserved.
 *
 **************************************************************************************
 **/


#if defined OIPF && defined JLABS

#include "jlabsEmmMessageManager.h"
#include "jlabsEmmMessageCollection.h"
#include "jlabsBoardMessage.h"
#include "NativeJlabsEmmMessageManager.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jsplugin.h"

#include "macros.h"
#include "Exception.h"
#include "WrapperObject.h"
#include "Collection.h"


using namespace Ooif;

jlabsEmmMessageManager::jlabsEmmMessageManager()
{
	ENTRY;
	classType = CLASS_JLABS_EMM_MESSAGE_MANAGER;
}

jlabsEmmMessageManager::~jlabsEmmMessageManager()
{
	ENTRY;
}

int jlabsEmmMessageManager::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;

	GET_FUNCTION(stopAutoDisplayMessage, obj, "n", JSP_GET_VALUE);
	GET_FUNCTION(getAllEmmMessage, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(getAllSelfMessage, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(getAllBoardMessage, obj, "", JSP_GET_VALUE_CACHE);

	return JSP_GET_NOTFOUND;
}

int jlabsEmmMessageManager::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
	return JSP_PUT_NOTFOUND;
}


int jlabsEmmMessageManager::stopAutoDisplayMessage(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(1);
	jlabsEmmMessageManager *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, jlabsEmmMessageManager);

	int retval = -1;
	NATIVE_CALL(native_jlabsEmmMessageMgrStopAutoDispMsg(NUMBER_VALUE(argv[0]), &retval));
	RETURN_NUMBER(retval, JSP_CALL_VALUE);
}

int jlabsEmmMessageManager::getAllEmmMessage(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	int count = 0;
	jlabsEmmMessageHandle *handles = NULL;
	native_jlabsEmmMessageMgrGetAllEmmMsg(&count, &handles);
	if( count > 0 )
	{
		RETURN_OBJECT(this_obj, new jlabsEmmMessageCollection(count, handles), result, JSP_CALL_VALUE);
	}
	else
	{
		RETURN_OBJECT(this_obj, new jlabsEmmMessageCollection(), result, JSP_CALL_VALUE);
	}
}

int jlabsEmmMessageManager::getAllSelfMessage(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	int count = 0;
	jlabsEmmMessageHandle *handles = NULL;
	NATIVE_CALL(native_jlabsEmmMessageMgrGetAllSelfMsg(&count, &handles));
	if( count > 0 )
	{
		RETURN_OBJECT(this_obj, new jlabsEmmMessageCollection(count, handles), result, JSP_CALL_VALUE);
	}
	else
	{
		RETURN_OBJECT(this_obj, new jlabsEmmMessageCollection(), result, JSP_CALL_VALUE);
	}
}

int jlabsEmmMessageManager::getAllBoardMessage(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	jlabsBoardMessageHandle *handles = NULL;
	int count	= 0;
	NATIVE_CALL(native_jlabsEmmMessageMgrGetAllBoardMsg(&count, &handles));
	jlabsBoardMessageCollection *collection = new jlabsBoardMessageCollection();

	for (int i=0; i<count; i++)
	{
		collection->addItem(new jlabsBoardMessage(handles[i]));
	}
	HLIB_STD_MemFree(handles);
	handles = NULL;
	RETURN_OBJECT(this_obj, collection, result, JSP_CALL_VALUE);
}




#endif // OIPF || OPT_HBBTV_EXT_CONFIG


