/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: channelscan.h $
 * $brcm_Revision: 7 $
 * $brcm_Date: 5/18/12 7:42p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/channelmgr/channelscan.h $
 * 
 * 7   5/18/12 7:42p bselva
 * SW7360-19: Add support for 7360 platform
 * 
 * 6   3/21/12 12:12p erickson
 * SW7425-2664: consolidate force_sds BCHP_CHIP list
 * 
 * 5   9/19/07 12:31p jrubio
 * PR34613: new argument for read, NETACCEL
 * 
 * 4   1/29/07 11:18a ssood
 * PR24918: Channel Map Redesign to make its access Thread-safe
 * 
 * 3   11/21/06 8:52a erickson
 * PR23575: add DEMUX_PROGRAMS feature (defaulted off) to create separate
 * channels for streams with multiple audio/video pids.
 * 
 * 2   4/9/05 12:21a erickson
 * PR14701: added func to scan one channel
 * 
 * 1   2/7/05 7:59p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/10   7/21/04 12:10p erickson
 * PR11682: adapted to tspsimgr changes to better support PMT capture
 * without hack
 * 
 * Irvine_BSEAVSW_Devel/9   7/21/04 10:10a erickson
 * PR11682: move channels.txt support out of ChannelMap and into
 * PSIChannelScan
 * 
 * Irvine_BSEAVSW_Devel/8   4/30/04 12:06p erickson
 * PR10832: rework of brutus to support single AVManager and
 * ChannelManager for system
 * 
 * Irvine_BSEAVSW_Devel/7   3/4/04 1:16p erickson
 * PR9465: updated more comments
 * 
 * Irvine_BSEAVSW_Devel/6   2/13/04 5:34p erickson
 * PR9465: updated comments
 * 
 * Irvine_BSEAVSW_Devel/5   2/13/04 12:05p erickson
 * PR9465: updated channelmgr naming and comments
 * 
 * Irvine_BSEAVSW_Devel/4   2/12/04 7:35p erickson
 * PR9465: reworked ChannelManager to support multiple tuners per
 * downstream
 * 
 * Irvine_BSEAVSW_Devel/3   10/10/03 1:43p erickson
 * converted brutus frontend code so that it can support all frontend
 * types simultaneously. this
 * is in preperation for 97038 and others.
 *
 *************************************************************/
#ifndef CHANNELSCAN_H__
#define CHANNELSCAN_H__

#include "channelmap.h"
#include "bsettop.h"

/**
Summary:
Tune callback function passed into ChannelScan constructor.
Description:
This allows ChannelScan to be written in an abstract way.
**/
typedef bband_t (*TUNE_FUNC_T)(ChannelMap::ChannelMapEntry *stream, void *data);
typedef void (*UNTUNE_FUNC_T)(bband_t band, void *data);

/**
Summary:
ChannelScan is an abstract class which helps to populate a ChannelMap object.

Description:
Ancestors of ChannelScan can override the read/write functions to manage
a file-based channel map.
They can also override the scan function to dynamically read program information
for a PSI channel scan, POD channel scan or other means.
**/
class ChannelScan {
public:
	struct Settings {
		bool filterOutEncrypted;
		bool filterOutDuplicatePids;
		bool filterOutAudioOnly;
		bool filterOutDataPids;
		bool demuxPrograms; /* if there are multiple audio/video pids in a program, setting this
			option to true causes individual programs to be created for every audio[N]/video[N] pair. */
	};

	/**
	Summary:
	Required to initialize the Settings structure.
	**/
	static void initialize(Settings *settings);

	ChannelScan(
		ChannelMap *map, /* source of the channel map and destination of the channel list */
		const Settings *settings);	/* settings for the scan */

	void setTuneCallback(
		TUNE_FUNC_T tuneFunc,	/* callback function into the application to tune */
		UNTUNE_FUNC_T untuneFunc,	/* callback function into the application to untune */
		void *tuneFuncData	/* data to pass to the callback function */
		);

	/**
	Summary:
	Scan all channels.
	Description:
	This implements the generic algorithm, which depends on the pure virtual
	function readProgramInfo to be implemented in a subclass.
	**/
	int scan();
	
	/**
	Summary:
	Scan for channels on an entry in the ChannelMap.
	**/
	int scan(int entryIndex, bband_t band = NULL);

	/**
	Summary:
	Write the channel map to disk, if supported.
	**/
	virtual int write(const char *filename) const {return -1;}

	/**
	Summary:
	Read the channel map from disk, if supported.
	**/
	virtual int read(const char *filename, const char * ipv4 = NULL) {return -1;}

	ChannelMap *map() const {return _map;}

protected:
	ChannelMap *_map;
	TUNE_FUNC_T _tuneFunc;
	UNTUNE_FUNC_T _untuneFunc;
	void *_tuneFuncData;
	Settings _settings;

	/* pure virtual function call be scan(). */
	virtual int readProgramInfo(bband_t band, unsigned stream) = 0;
	
	bool includeProgram(unsigned stream, const PROGRAM_INFO_T *program);
};

#if (BCHP_CHIP==7325) || (BCHP_CHIP==7335) || (BCHP_CHIP==7340) || \
	(BCHP_CHIP==7342) || (BCHP_CHIP==7420) || (BCHP_CHIP==7422) || \
	(BCHP_CHIP==7425) || (BCHP_CHIP==7346) || (BCHP_CHIP==7344) || \
	(BCHP_CHIP==7358) || (BCHP_CHIP==7435) || (BCHP_CHIP==7360)
/* extern the force_sds global to control scan */
#define B_CHANNELMGR_FORCE_SDS 1
#endif

#endif //CHANNELSCAN_H__
