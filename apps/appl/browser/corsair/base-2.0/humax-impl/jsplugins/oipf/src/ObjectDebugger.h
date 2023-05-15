/*
 *	OIPF Object Debugger
 *
 *	Written by Jongwon Kim, April 2 2013
 *
 */

#ifdef CONFIG_DEBUG

#ifndef __OBJECT_DEBUGGER_H__
#define __OBJECT_DEBUGGER_H__

#include <map>

using namespace std;

/*--------------------------------------------------------------------------------
 * Object debugger definitions
 *--------------------------------------------------------------------------------*/
#define CONFIG_OBJDBG_PROGRAMME
#define CONFIG_OBJDBG_QUERY
#define CONFIG_OBJDBG_CHANNEL
#define CONFIG_OBJDBG_METADATASEARCH
#define CONFIG_OBJDBG_RECORDING
#define CONFIG_OBJDBG_SCHEDULEDRECORDING
#define CONFIG_OBJDBG_PARENTAL_RATING
#define CONFIG_OBJDBG_WIDGETDESCRIPTOR
#define CONFIG_OBJDBG_VIDEO_BROADCAS
#define CONFIG_OBJDBG_VIDEO_ON_DEMAND
#define CONFIG_OBJDBG_FAVOURITELIST

/* Humax Objects */
#define CONFIG_OBJDBG_HNETWORK
#define CONFIG_OBJDBG_HCONTENT
#define CONFIG_OBJDBG_HDLNACDS
#define CONFIG_OBJDBG_HSAMBASERVER
#define CONFIG_OBJDBG_HFTPSERVER
#define CONFIG_OBJDBG_HSTORAGE


/*--------------------------------------------------------------------------------
 * Includes Headder files
 *--------------------------------------------------------------------------------*/
#ifdef CONFIG_OBJDBG_PROGRAMME
#include "Programme.h"
#endif
#ifdef CONFIG_OBJDBG_QUERY
#include "Query.h"
#endif
#ifdef CONFIG_OBJDBG_CHANNEL
#include "Channel.h"
#endif
#ifdef CONFIG_OBJDBG_METADATASEARCH
#include "MetadataSearch.h"
#include "SearchManager.h"
#include "SearchResults.h"
#endif
#ifdef CONFIG_OBJDBG_RECORDING
#include "Recording.h"
#endif
#ifdef CONFIG_OBJDBG_SCHEDULEDRECORDING
#include "ScheduledRecording.h"
#endif
#ifdef CONFIG_OBJDBG_PARENTAL_RATING
#include "ParentalRating.h"
#endif
#ifdef CONFIG_OBJDBG_WIDGETDESCRIPTOR
#include "WidgetDescriptor.h"
#endif
#ifdef CONFIG_OBJDBG_VIDEO_BROADCAS
#include "VideoBroadcast.h"
#endif
#ifdef CONFIG_OBJDBG_VIDEO_ON_DEMAND
#include "VideoOnDemand.h"
#endif
#ifdef CONFIG_OBJDBG_FAVOURITELIST
#include "FavouriteList.h"
//#include "FavouriteListCollection.h"
#endif

#ifdef CONFIG_OBJDBG_HNETWORK
#include "humax/HNetworkInterface.h"
#endif
#ifdef CONFIG_OBJDBG_HCONTENT
#include "humax/HContentManager.h"
#endif
#ifdef CONFIG_OBJDBG_HDLNACDS
#include "humax/HDlna.h"
#endif
#ifdef CONFIG_OBJDBG_HSAMBASERVER
#include "humax/HSamba.h"
#endif
#ifdef CONFIG_OBJDBG_HFTPSERVER
#include "humax/HFtpManager.h"
#endif
#ifdef CONFIG_OBJDBG_HSTORAGE
#include "humax/HMXLogicalStorage.h"
#include "humax/HMXPhysicalStorage.h"
#endif


/*--------------------------------------------------------------------------------
 * Class OIPFObjectInfo
 *--------------------------------------------------------------------------------*/
namespace Ooif
{
template<class T>
class OIPFObjectInfo {
public:
	inline static int __handle() { return T::refs.size(); }
	inline static int __ref()
	{
		int count = 0;
		map<void*,unsigned int>::iterator it;
		for ( it = T::refs.begin(); it != T::refs.end(); ++it )
			count += it->second;
		return count;
	}
};

#define OBJECT_DEBUGGER(T) \
	do { \
		OOIF_LOG_FPRINT("|\toipf_obj_dbg:\t ["#T"][handle:%d][ref:%d]\n",\
			OIPFObjectInfo<T>::__handle(),\
			OIPFObjectInfo<T>::__ref());	\
	} while (0)

/*--------------------------------------------------------------------------------
 * Class OIPFObjectDebugger
 *--------------------------------------------------------------------------------*/
class OIPFObjectDebugger {
public:
	static void now()
	{
		__head();
#ifdef CONFIG_OBJDBG_PROGRAMME
		OBJECT_DEBUGGER(Programme);
#endif
#ifdef CONFIG_OBJDBG_QUERY
		OBJECT_DEBUGGER(Query);
#endif
#ifdef CONFIG_OBJDBG_CHANNEL
		OBJECT_DEBUGGER(Channel);
#endif
#ifdef CONFIG_OBJDBG_METADATASEARCH
		OBJECT_DEBUGGER(MetadataSearch);
		OBJECT_DEBUGGER(SearchManager);
		OBJECT_DEBUGGER(SearchResults);
#endif
#ifdef CONFIG_OBJDBG_RECORDING
		OBJECT_DEBUGGER(Recording);
#endif
#ifdef CONFIG_OBJDBG_SCHEDULEDRECORDING
		OBJECT_DEBUGGER(ScheduledRecording);
#endif
#ifdef CONFIG_OBJDBG_PARENTAL_RATING
		OBJECT_DEBUGGER(ParentalRating);
#endif
#ifdef CONFIG_OBJDBG_WIDGETDESCRIPTOR
		OBJECT_DEBUGGER(WidgetDescriptor);
#endif
#ifdef CONFIG_OBJDBG_VIDEO_BROADCAS
		OBJECT_DEBUGGER(VideoBroadcast);
#endif
#ifdef CONFIG_OBJDBG_VIDEO_ON_DEMAND
		OBJECT_DEBUGGER(VideoOnDemand);
#endif
#ifdef CONFIG_OBJDBG_FAVOURITELIST
		OBJECT_DEBUGGER(FavouriteList);
//		OBJECT_DEBUGGER(FavouriteListCollection);
#endif

#ifdef CONFIG_OBJDBG_HNETWORK
		OBJECT_DEBUGGER(HNetworkInterface);
#endif

#ifdef CONFIG_OBJDBG_HCONTENT
		OBJECT_DEBUGGER(HContent);
#endif

#ifdef CONFIG_OBJDBG_HDLNACDS
		OBJECT_DEBUGGER(HDlnaCds);
		OBJECT_DEBUGGER(HDlnaCdsContent);
#endif

#ifdef CONFIG_OBJDBG_HSAMBASERVER
		OBJECT_DEBUGGER(HSambaSearchedServer);
		OBJECT_DEBUGGER(HSmbsShareFolder);
#endif
#ifdef CONFIG_OBJDBG_HFTPSERVER
		OBJECT_DEBUGGER(HFtpsShareFolder);
#endif

#ifdef CONFIG_OBJDBG_HSTORAGE
		OBJECT_DEBUGGER(HMXLogicalStorage);
		OBJECT_DEBUGGER(HMXPhysicalStorage);
#endif

		__tail();
	}

	inline static void __head()
	{ OOIF_LOG_FPRINT( "------------------------- OIPF Objects ---------------------------\n"); }

	inline static void __tail()
	{ OOIF_LOG_FPRINT( "------------------------------------------------------------------\n"); }
};

}	/* namespace Ooif */

#endif	/* __OBJECT_MONITOR_H__ */

#endif	/* CONFIG_DEBUG */

