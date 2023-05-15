/***************************************************************************
 *     Copyright (c) 2003-2007, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: channelscan.cpp $
 * $brcm_Revision: 8 $
 * $brcm_Date: 6/29/07 4:48p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/channelmgr/channelscan.cpp $
 * 
 * 8   6/29/07 4:48p erickson
 * PR21941: fix includeProgram DBG
 * 
 * 7   1/29/07 11:18a ssood
 * PR24918: Channel Map Redesign to make its access Thread-safe
 * 
 * 6   11/27/06 1:57p erickson
 * PR24374: fix WRN
 * 
 * 5   11/21/06 8:52a erickson
 * PR23575: add DEMUX_PROGRAMS feature (defaulted off) to create separate
 * channels for streams with multiple audio/video pids.
 * 
 * 4   7/31/06 3:50p erickson
 * PR21941: give feedback if program or pid is excluded along with bypass.
 * fixed encrypted channels bypass.
 * 
 * 3   6/19/06 3:47p erickson
 * PR19526: added ChannelMapEntry::setType to force initialization of
 * Settop API frontend params structures when type changes
 * 
 * 2   4/9/05 12:21a erickson
 * PR14701: added func to scan one channel
 * 
 * 1   2/7/05 7:59p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/15   8/17/04 4:41p erickson
 * PR11770: removed brutus inputband support. added streamer as a
 * registered frontend resource, with a mapping between index number and
 * settop api id.
 * 
 * Irvine_BSEAVSW_Devel/14   8/9/04 3:48p erickson
 * PR12104: extended SDS to handle DSS_PES and DSS_ES, along with STREAMER
 * and INPUTBAND
 * 
 * Irvine_BSEAVSW_Devel/13   7/21/04 10:10a erickson
 * PR11682: move channels.txt support out of ChannelMap and into
 * PSIChannelScan
 * 
 * Irvine_BSEAVSW_Devel/12   5/28/04 8:45a erickson
 * PR11243: amodified STREAMER and STREAMER_DSS to support streamer
 * indexes, not just INPUTBAND synonym
 *
 * Irvine_BSEAVSW_Devel/11   4/30/04 12:06p erickson
 * PR10832: rework of brutus to support single AVManager and
 * ChannelManager for system
 *
 * Irvine_BSEAVSW_Devel/10   3/2/04 5:26p erickson
 * PR9217: improved debug output on channel scan
 *
 * Irvine_BSEAVSW_Devel/9   2/13/04 12:05p erickson
 * PR9465: updated channelmgr naming and comments
 *
 * Irvine_BSEAVSW_Devel/8   2/12/04 7:34p erickson
 * PR9465: reworked ChannelManager to support multiple tuners per
 * downstream
 *
 * Irvine_BSEAVSW_Devel/7   10/10/03 1:43p erickson
 * converted brutus frontend code so that it can support all frontend
 * types simultaneously. this
 * is in preperation for 97038 and others.
 *
 *************************************************************/
#include "channelscan.h"
#include "bkni.h"
BDBG_MODULE(channelscan);

ChannelScan::ChannelScan(ChannelMap *map, const Settings *settings)
{
	_map = map;
	_settings = *settings;
	_tuneFunc = NULL;
	_untuneFunc = NULL;
	_tuneFuncData = NULL;
}

void ChannelScan::setTuneCallback(TUNE_FUNC_T tuneFunc, UNTUNE_FUNC_T untuneFunc, void *tuneFuncData)
{
	_tuneFunc = tuneFunc;
	_untuneFunc = untuneFunc;
	_tuneFuncData = tuneFuncData;
}

int ChannelScan::scan(int entryIndex, bband_t band)
{
	ChannelMap::ChannelMapEntry str;
	bband_t temp_band = NULL;
	
	if (_map->getEntry(entryIndex, &str) == false ||
	    str.getType() == btuner_analog_stream)
		return 0;
		 
	switch (str.getType()) {
	case btuner_streamer_stream:
		printf("Scanning streamer %d (TS%d_IN)...\n", str.streamer.index,
			str.streamer.index+1); break;
	default:
		printf("Scanning %0.2f...\n", str.freq); break;
	}

	if (!band) {	
		/* We if weren't given a band to scan on, go get one */
		temp_band = (*_tuneFunc)(&str, _tuneFuncData);
		if (!temp_band)
			return -1;
		band = temp_band;
	}
		
	/* call virtual function to get program info from the band and populate
	the ChannelMap */
	readProgramInfo(band, entryIndex);
	
	if (temp_band)
		(*_untuneFunc)(temp_band, _tuneFuncData);
	return 0;
}

int ChannelScan::scan()
{
	_map->clearPrograms();
	BDBG_MSG(("Scanning %d entries", _map->total()));
	for (int i=0;i < _map->total(); i++) {
		if (!_map->isNoScanChannelMapEntry(i))
			scan(i);
	}
	return 0;
}

// 0 is a valid pid, but never in this context, therefore it's excluded.
#define INVALID_PID(pid) ((pid)==0 || (pid)>=0x1FFF)

bool ChannelScan::includeProgram(unsigned stream, const PROGRAM_INFO_T *program)
{
//TODO: multiple video/audio programs
	int vidpid = program->num_video_pids?program->video_pids[0].pid:0x1FFF;
	int audpid = program->num_audio_pids?program->audio_pids[0].pid:0x1FFF;
	if (_settings.filterOutEncrypted) {
		if (program->ca_pid) {
			BDBG_WRN(("program with vidpid 0x%x excluded because program ca_pid set. use ENCRYPTED_CHANNELS_ENABLED=1 to bypass.",
				program->video_pids[0].pid));
			return false;
		}
		if (program->video_pids[0].ca_pid) {
			BDBG_WRN(("vidpid 0x%x excluded because ca_pid set. use ENCRYPTED_CHANNELS_ENABLED=1 to bypass.",
				program->video_pids[0].pid));
			vidpid = 0x1FFF;
		}
		if (program->audio_pids[0].ca_pid) {
			BDBG_WRN(("audpid 0x%x excluded because ca_pid set. use ENCRYPTED_CHANNELS_ENABLED=1 to bypass.",
				program->audio_pids[0].pid));
			audpid = 0x1FFF;
		}
	}
	if (INVALID_PID(audpid) && INVALID_PID(vidpid)) {
		/* we've eliminated our content */
		return false;
	}

	/* must always have valid pcr pid for live */
	if (INVALID_PID(program->pcr_pid)) {
		return false;
	}

	if (_settings.filterOutDuplicatePids) {
		for (int i=0;i<_map->totalPrograms(stream);i++) {
			PROGRAM_INFO_T other;
                        if (_map->getProgram(stream, i, &other) == false)
                                return false;
			if (vidpid == other.video_pids[0].pid &&
				other.num_video_pids &&
				audpid == other.audio_pids[0].pid &&
				other.num_audio_pids)
			{
				BDBG_WRN(("duplicate vidpid 0x%x/audpid 0x%x excluded. use DUPLICATE_CHANNELS_ENABLED=1 to bypass.",
					vidpid, audpid));
				return false;
			}
		}
	}

	if (_settings.filterOutAudioOnly && INVALID_PID(vidpid) && !INVALID_PID(audpid)) {
		BDBG_WRN(("audpid 0x%x excluded because it had no video. use AUDIO_ONLY_CHANNELS_ENABLED=1 to bypass.",
			program->audio_pids[0].pid));
		return false;
	}

#if 0
/* brutus does nothing with "other" pids anyway */
	if (_settings.filterOutDataPids && INVALID_PID(vidpid) && INVALID_PID(audpid)) {
		BDBG_WRN(("no audio/video pids. program filtered out. use DATA_CHANNELS_ENABLED=1 to bypass."));
		return false;
	}
#endif

	return true;
}

void ChannelScan::initialize(Settings *settings)
{
	BKNI_Memset(settings, 0, sizeof(*settings));
	settings->filterOutEncrypted = true;
	settings->filterOutDuplicatePids = true;
	settings->filterOutAudioOnly = true;
	settings->filterOutDataPids = true;
}
