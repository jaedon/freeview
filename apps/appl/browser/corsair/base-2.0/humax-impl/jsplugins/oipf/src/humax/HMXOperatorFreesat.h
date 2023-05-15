/**************************************************************
 *	@file		HMXOperatorFreesat.h
 *
 *	http://www.humaxdigital.com
 *
 *	@data			2012/04/10
 *	@author			humax
 *	@brief			appkit for MAYA/WEB Application
 **************************************************************/

#ifdef	CONFIG_OP_FREESAT

#ifndef HMXOPERATORFREESAT_H_
#define HMXOPERATORFREESAT_H_

#include "ObjectInstance.h"
#include "jsplugin.h"
#include "EventTarget.h"

#include "NativeHOperatorFreesat.h"

namespace Ooif {

class HMXOperatorFreesat: public EventTarget {
public:
	HMXOperatorFreesat();
	~HMXOperatorFreesat();
private: // from EventTarget
	static list<HMXOperatorFreesat*> 	instances;

	virtual int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
	virtual int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
public:
	static int listenHomeTp(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int setDeepLinkForITV(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static void onFreesatMetaFileUpdateEventOPL(int eType, int subParam);
	static void onFreesatMhegFileUpdateEventOPL(int eType, int subParam);
static const ClassType class_name = CLASS_HMX_OPERATOR_FREESAT;
private:


};

}
#endif /* HMXOPERATORFREESAT_H_ */
#endif


