/**************************************************************
 *	@file		HMXOperatorFreesat.cpp
 *
 *	http://www.humaxdigital.com
 *
 *	@data			2012/04/10
 *	@author			humax
 *	@brief			appkit for MAYA/WEB Application
 **************************************************************/

#ifdef	CONFIG_OP_FREESAT


#include <string.h>

#include "macros.h"
#include "OOIFUtils.h"
#include "HMXOperatorFreesat.h"
#include "HMXOperatorFreesatMetaUpdateEvent.h"
#include "HMXOperatorFreesatMhegUpdateEvent.h"

#include "oploperator_freesat.h"
//#include "oplsqcinfo_freesat.h"
#include "opllocalsystem.h"


// OPL
using namespace Ooif;
//static

list<HMXOperatorFreesat*> HMXOperatorFreesat::instances;

HMXOperatorFreesat::HMXOperatorFreesat() : EventTarget()
{
	ENTRY;

#ifdef	CONFIG_OP_FREESAT
	classType = class_name;
	HMXOperatorFreesat::instances.push_back(this);

	OPL_Operator_FSAT_InitMetaEventListner();
	OPL_Operator_FSAT_RegisterMetaEventListener(onFreesatMetaFileUpdateEventOPL);
	OPL_Operator_FSAT_RegisterMhegEventListener(onFreesatMhegFileUpdateEventOPL);
#else
	//#error	"the class will be deleted!!"
#endif
}

HMXOperatorFreesat::~HMXOperatorFreesat()
{
	ENTRY;
#ifdef	CONFIG_OP_FREESAT
	HMXOperatorFreesat::instances.remove(this);
#endif
}

void HMXOperatorFreesat::onFreesatMetaFileUpdateEventOPL(int eType, int subParam)
{
	Event *e = NULL;

	ENTRY;
	OOIF_LOG_DEBUG(" Freesat Meta File Update  Message : 0X%x ", eType);

	// Iterate over all instances and dispatch the event
	for (list<HMXOperatorFreesat*>::iterator it=instances.begin(); it!=instances.end(); it++) {
		jsplugin_obj *context = (*it)->getHost();

		if(eType != eOpFSAT_MetaEventType_Xml)
			continue;

		e = new HMXOperatorFreesatMetaUpdateEvent(context, eType, (const char *)subParam);
		// do something to notify ... file name is pFilename

		(*it)->dispatchEvent(e);
	}
}

void HMXOperatorFreesat::onFreesatMhegFileUpdateEventOPL(int eType, int subParam)
{
	Event *e = NULL;

	ENTRY;
	OOIF_LOG_DEBUG(" Freesat Mheg : 0X%x ", eType);

	// Iterate over all instances and dispatch the event
	for (list<HMXOperatorFreesat*>::iterator it=instances.begin(); it!=instances.end(); it++) {
		jsplugin_obj *context = (*it)->getHost();

		e = new HMXOperatorFreesatMhegUpdateEvent(context, eType, (const char *)subParam);
		// do something to notify ... file name is pFilename

		(*it)->dispatchEvent(e);
	}
}

int HMXOperatorFreesat::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;

	OOIF_LOG_DEBUG("HMXOperatorFreesat::getter [%s]", name);

	INTRINSIC_EVENT_GET(FreesatMetaFileUpdate);
	INTRINSIC_EVENT_GET(FreesatMhegEvent);
	GET_STRING("name", "Freesat", JSP_GET_VALUE);
	GET_FUNCTION(listenHomeTp, obj, "n", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(setDeepLinkForITV, obj, "sn", JSP_GET_VALUE_CACHE);
	NATIVE_GET_STRING("postcode", native_HGetPostCode(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("bouquetname", native_HGetBouquetName(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("simode", native_HGetSiMode(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("bouquetid", native_HGetBouquetId(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("regioncode", native_HGetRegionCode(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("adultchannels", native_HGetAdultChannels(&RETVAL), JSP_GET_VALUE);

	return JSP_GET_NOTFOUND;
	//return ObjectInstance::getter(obj, name, result);
}

int HMXOperatorFreesat::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;

	OOIF_LOG_DEBUG("HMXOperatorFreesat::setter [%s]", name);

	NATIVE_SET("postcode", native_HSetPostCode(STRING_VALUE(value[0])));
	NATIVE_SET("bouquetname", native_HSetBouquetName(STRING_VALUE(value[0])));
	NATIVE_SET("simode", native_HSetSiMode(NUMBER_VALUE(value[0])));
	NATIVE_SET("adultchannels", native_HSetAdultChannels(BOOLEAN_VALUE(value[0])));
	NATIVE_SET("mhegoperation", native_HSetMHEGOperationControl(BOOLEAN_VALUE(value[0])));
	INTRINSIC_EVENT(FreesatMetaFileUpdate);
	INTRINSIC_EVENT(FreesatMhegEvent);

	return JSP_GET_NOTFOUND;
	//return ObjectInstance::setter(obj, name, value);
}

int HMXOperatorFreesat::listenHomeTp(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;

	CHECK_ARGUMENT_COUNT(1);
	int listen = NUMBER_VALUE(argv[0]);

	OOIF_LOG_PRINT(">>>>> HMXOperatorFreesat::listenHomeTp set = %d\n", listen);
	if(listen != 0)
	{
		OPL_LocalSystem_ExecuteCommand(0);
	}

	RETURN_BOOLEAN(listen, JSP_GET_VALUE);	
}

int HMXOperatorFreesat::setDeepLinkForITV(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
							   int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(2);
	CHECK_ARGUMENT(0, JSP_TYPE_STRING);
	CHECK_ARGUMENT(1, JSP_TYPE_NUMBER);
#ifdef CONFIG_OP_FREESAT
	OPL_SqcInfo_Freesat_SetDeepLinkForITV(NULL, STRING_VALUE(argv[0]), NUMBER_VALUE(argv[1]));
#endif

	return JSP_CALL_NO_VALUE;
}

#endif

