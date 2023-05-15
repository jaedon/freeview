#if defined OIPF && defined JLABS

#ifndef __JLABS_DMR_CONTROLLER__
#define __JLABS_DMR_CONTROLLER__

#include "jsplugin.h"
#include "ObjectInstance.h"
#include "EventTarget.h"
#include "jlabsCDSRecording.h"

namespace Ooif
{

	class jlabsDMRController: public EventTarget
	{
		private :
			static jlabsDMRController *m_pDmrInstance;

		public :
			jlabsDMRController();
			virtual ~jlabsDMRController();

			int getter( jsplugin_obj *obj, const char *name, jsplugin_value *result );
			int setter( jsplugin_obj *obj, const char *name, jsplugin_value *value );

			static int  isPlay(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
			static int  stop(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);

			static void callbackEventHandler(unsigned int event, unsigned int result, jlabsCDSRecordingHandle cdsItem );

		public :
			static const ClassType class_name = CLASS_JLABS_DMR_CONTROLLER;
	};

} /* namespace Ooif */

#endif 
#endif
