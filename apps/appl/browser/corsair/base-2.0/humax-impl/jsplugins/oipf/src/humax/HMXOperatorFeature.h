/*
 * HMXOperatorFeature.h
 *
 *  Created on: 2012. 03. 15.
 *      Author: mspark
 */

#ifndef HMXOPERATORFEATURE_H_
#define HMXOPERATORFEATURE_H_

#include "ObjectInstance.h"
#include "humax/HDataTypes.h"

typedef void (*hOperatorFeatureListener)(void *pstOperatorFeature);
OOIFReturnCode native_HMXOperatorFeauture_GetNcdNetworkMsg(hOperatorFeatureListener listener,unsigned int param1);
OOIFReturnCode native_HMXOperatorFeautureSetListener(hOperatorFeatureListener listener);
void native_HMXOperatorFeautureNcdEventListener(void *pstNcdInfo);

namespace Ooif {

class HMXOperatorFeature : public EventTarget {
public:
	HMXOperatorFeature();
	~HMXOperatorFeature();
private: // from EventTarget
	static list<HMXOperatorFeature*>	instances; // for hadling events.
	static void onNcdInfoEventOPL(void *pEventData);

	virtual int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
	virtual int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
public:
	static int listenHomeTp(
		jsplugin_obj *this_obj,
		jsplugin_obj *function_obj,
		int argc,
		jsplugin_value *argv,
		jsplugin_value *result);
	static int getNcdNetworkMsg(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int setNcdNotifyData(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static void onNcdInfoEvent(humax::NcdInfoEventType_e event, humax::NcdNetworkInfo_t *info);
	static const ClassType class_name = CLASS_HMX_OPERATORFEATURE;
};

}
#endif /* HMXSTORAGEUTIL_H_ */


