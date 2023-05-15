#if defined OIPF && defined JLABS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jsplugin.h"

#include "macros.h"
#include "Exception.h"
#include "WrapperObject.h"
#include "jlabsPPVHistoryCollection.h"
#include "jlabsPPVHistory.h"
#include "UploadPPVViewlogDataEvent.h"
#include "NativeJlabsPPVHistoryCollection.h"

using namespace Ooif;

list<jlabsPPVHistoryCollection*> jlabsPPVHistoryCollection::instances;

void jlabsPPVHistoryCollection::init()
{
	classType = CLASS_JLABS_PPV_HISTORY_COLLECTION;
}

jlabsPPVHistory* jlabsPPVHistoryCollection::internalGetItem(int index)
{
	return items[index];
}

jlabsPPVHistoryCollection::jlabsPPVHistoryCollection()
{
	ENTRY;
	init();
	jlabsPPVHistoryCollection::instances.push_back(this);
}

jlabsPPVHistoryCollection::~jlabsPPVHistoryCollection()
{
	ENTRY;
	while (!this->items.empty()) {
		delete this->items.back();
		this->items.pop_back();
	}
	jlabsPPVHistoryCollection::instances.remove(this);
}


int jlabsPPVHistoryCollection::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
	GET_NUMBER("length", this->getSize(), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("totalPurchasedFee", native_jlabsPPVHistoryCollectionGetTotalPurchasedFee(&RETVAL), JSP_GET_VALUE);
	GET_FUNCTION(item, obj, "n", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(uploadPPVViewlogData, obj, "", JSP_GET_VALUE);
	GET_FUNCTION(deleteAllPPVPrivateData, obj, "", JSP_GET_VALUE);
	GET_FUNCTION(getPPVPurchaseHistory, obj, "n", JSP_GET_VALUE);
	GET_FUNCTION(cancelPPVViewlogDataUpload, obj, "", JSP_GET_VALUE);
	INTRINSIC_EVENT_GET(UploadPPVViewlogData);
	return JSP_GET_NOTFOUND;
}

int jlabsPPVHistoryCollection::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
	INTRINSIC_EVENT(UploadPPVViewlogData);
	return JSP_GET_NOTFOUND;
}

int jlabsPPVHistoryCollection::getSize()
{
	return items.size();
}

void jlabsPPVHistoryCollection::addItem(jlabsPPVHistory *item)
{
	items.push_back(item);
}

int jlabsPPVHistoryCollection::getItem(jsplugin_obj *this_obj, jsplugin_value *result, int index, int return_type)
{
	RETURN_OBJECT(this_obj, new jlabsPPVHistory(*(this->internalGetItem(index))), result, return_type);
}

int jlabsPPVHistoryCollection::item(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(1);
	CHECK_ARGUMENT(0, JSP_TYPE_NUMBER);
	jlabsPPVHistoryCollection *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, jlabsPPVHistoryCollection);
	if (instance->getSize() > NUMBER_VALUE(argv[0])  && NUMBER_VALUE(argv[0]) >= 0) {
		return instance->getItem(this_obj, result, (int)NUMBER_VALUE(argv[0]), JSP_CALL_VALUE);
	}
	else {
		RETURN_UNDEFINED(JSP_CALL_VALUE);
	}
}

int jlabsPPVHistoryCollection::uploadPPVViewlogData(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;

	CHECK_ARGUMENT_COUNT(0);

	OOIFNumber retval;

	NATIVE_CALL(native_jlabsPPVHistoryCollectionPPVViewlogData(&retval));
	RETURN_NUMBER(retval, JSP_CALL_VALUE);
}

int jlabsPPVHistoryCollection::deleteAllPPVPrivateData(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(0);

	OOIFNumber retval;

	NATIVE_CALL(native_jlabsPPVHistoryCollectionDeleteAllPPVPrivateData(&retval));
	RETURN_NUMBER(retval, JSP_CALL_VALUE);
}

int jlabsPPVHistoryCollection::getPPVPurchaseHistory(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;

	CHECK_ARGUMENT_COUNT(1);
	CHECK_ARGUMENT(0, JSP_TYPE_NUMBER);

	jlabsPPVHistoryCollection *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, jlabsPPVHistoryCollection);
	if (instance->getSize() > NUMBER_VALUE(argv[0])  && NUMBER_VALUE(argv[0]) >= 0) {
		return instance->getItem(this_obj, result, (int)NUMBER_VALUE(argv[0]), JSP_CALL_VALUE);
	}
	else {
		RETURN_UNDEFINED(JSP_CALL_VALUE);
	}
}

int jlabsPPVHistoryCollection::cancelPPVViewlogDataUpload(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;

	CHECK_ARGUMENT_COUNT(0);

	OOIFNumber retval;

	NATIVE_CALL(native_jlabsPPVHistoryCollectionCancelPPVViewlogDataUpload(&retval));
	RETURN_NUMBER(retval, JSP_CALL_VALUE);
}

void jlabsPPVHistoryCollection::onUploadPPVViewlogDataEvent(int result)
{
	for (list<jlabsPPVHistoryCollection*>::iterator it=instances.begin(); it != instances.end(); it++)
	{
		jlabsPPVHistoryCollection* ppvHistory = *it;
		Event *e = new UploadPPVViewlogDataEvent(ppvHistory->getHost(),result);
		ppvHistory->dispatchEvent(e);
	}
}

#endif //#if defined OIPF && defined JLABS
