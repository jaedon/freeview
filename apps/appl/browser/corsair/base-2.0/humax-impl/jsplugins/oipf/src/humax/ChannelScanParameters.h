#ifdef OIPF
#ifndef __CHANNELSCANPARAMETERS
#define __CHANNELSCANPARAMETERS

#include "jsplugin.h"
#include "macros.h"
#include "ObjectInstance.h"
#include "WrapperObject.h"
#include "Collection.h"

#include "NativeChannel.h"
#include "humax/HDataTypes.h"
#if defined JLABS
#include "NativeJlabsScanning.h"
#endif

namespace Ooif
{
	class DVBTChannelScanParameters
	{
		private:
		public:
			int		startFrequency;
			int		endFrequency;
			char	*transmission;
			int		raster;
			int		modulationModes;

			char	*ofdm;
			char	*bandwidth;

			DVBTChannelScanParameters();
			DVBTChannelScanParameters(const DVBTChannelScanParameters &original);
			DVBTChannelScanParameters(humax::DVBT_TunedData *_DVBTtunedData);
			~DVBTChannelScanParameters();

			int		getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
			int		setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
	};

	class DVBCChannelScanParameters
	{
		private:
		public:
			int		startFrequency;
			int		endFrequency;
			int		raster;
			int		modulationModes;
			int		networkId;

			bool	startNoetworkScanOnNIT;

			char	*symbolRate;

			DVBCChannelScanParameters();
			DVBCChannelScanParameters(const DVBCChannelScanParameters &original);
			DVBCChannelScanParameters(humax::DVBC_TunedData *_DVBCtunedData);
			~DVBCChannelScanParameters();

			int		getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
			int		setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
	};

	class DVBSChannelScanParameters
	{
		private:
		public:
			int		antennaId;
			char	*antennaName;

			int		startFrequency;
			int		endFrequency;
			int		modulationModes;
			int		polarisation;
			int		networkId;

			char	*symbolRate;
			char	*codeRate;

			float	orbitalPosition;

			DVBSChannelScanParameters();
			DVBSChannelScanParameters(const DVBSChannelScanParameters &original);
			DVBSChannelScanParameters(humax::DVBS_TunedData *_DVBStunedData);
			~DVBSChannelScanParameters();

			int		getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
			int		setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
	};

	class ChannelScanParameters : public ObjectInstance, public WrapperObject<ChannelScanParameters>
	{
	private:

	public:
		//		ChannelScanParameters Structures.
		int		idType;

		DVBTChannelScanParameters	*dvbt_scan_parameters;
		DVBSChannelScanParameters	*dvbs_scan_parameters;
		DVBCChannelScanParameters	*dvbc_scan_parameters;

		ChannelScanParameters(int id_type);
		ChannelScanParameters(const ChannelScanParameters &original);
		ChannelScanParameters(humax::ChannelTuned_Data *_tunedData);
#if defined JLABS
		ChannelScanParameters(jlabsScanningHandle handle);
#endif
		~ChannelScanParameters();

		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
		{
			switch (idType)
			{
				case	CHANNEL_ID_DVB_S:
				case	CHANNEL_ID_DVB_S2:
					return dvbs_scan_parameters->setter(obj, name, value);
				case	CHANNEL_ID_DVB_C:
				case	CHANNEL_ID_DVB_C2:
					return dvbc_scan_parameters->setter(obj, name, value);
				case	CHANNEL_ID_DVB_T:
				case	CHANNEL_ID_DVB_T2:
					return dvbt_scan_parameters->setter(obj, name, value);
				default:
					OOIF_LOG_ERROR("Error : not supported idType [%d]\n", idType);
					break;
			}

			return JSP_PUT_NOTFOUND;
		}

		static void releaseNative(void * id)
		{
		}

		static const ClassType class_name = CLASS_CHANNELSCANPARAMS;
	};

	class ChannelScanParametersCollection : public Collection<ChannelScanParameters>
	{
		public:
			ChannelScanParametersCollection();
			~ChannelScanParametersCollection();
			int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
			int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);

			static	int	addChannelScanParam(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
			static	int	removeChannelScanParam(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
			static const ClassType class_name = CLASS_CHANNELSCANPARAMSCOLLECTION;
	};

} /* namespace Ooif */

#endif
#endif // OIPF



