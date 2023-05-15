#ifdef	OIPF

#ifndef	__ANTENNAINFOCOLLECTION_H__
#define	__ANTENNAINFOCOLLECTION_H__

#include	"NativeHConfiguration.h"

#include "jsplugin.h"
#include "macros.h"
#include "ObjectInstance.h"
#include "WrapperObject.h"
#include "Collection.h"
#include "Bookmark.h"

#include "oplantinfo.h"

namespace Ooif
{
	class AntennaInfo : public ObjectInstance, public WrapperObject<AntennaInfo>
	{
		private:
			void	init(AntennaInfoHandle id);
		public:
			int					antennaId;
			OPL_AntennaInfo_t	antennaInfo;

			AntennaInfo(AntennaInfoHandle i);
			AntennaInfo(const AntennaInfo &original);
			~AntennaInfo();

			int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
			int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
			static void releaseNative(AntennaInfoHandle id);
			static const ClassType class_name = CLASS_ANTENNAINFO;

			static	int	commit(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
			static	int	refresh(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	};

	class AntennaInfoCollection : public Collection<AntennaInfo>
	{
		protected:

		public:
			AntennaInfoCollection();
			~AntennaInfoCollection();
			int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);

			// Javascript functions
			static	int	commit(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
			static	int	refresh(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
			static	int	createAntennaInfo(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
			static	int	getAntennaInfo(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
			static	int	getConnectionType(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
			static	int	setConnectionType(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
			static	int	removeAll(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);

			static	int	gotoPosition(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
			static	int	storePosition(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
			static	int	recalculate(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
			static	int	stop(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
			static	int	setLimit(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
			static	int	enableLimit(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
			static	int	disableLimit(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
			static	int	gotoDegree(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
			static	int	gotoSatellite(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
			static	int	drive(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
			static	int	getMyLatitude(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
			static	int	getMyLongitude(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
			static	int	setMyLatitude(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
			static	int	setMyLongitude(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);

			static const ClassType class_name = CLASS_ANTENNAINFOCOLLECTION;
	};

}

#endif
#endif

