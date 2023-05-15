/*************************************************************************************************************
	File 		: SmoothStreamingAbrConfig.h
	author 		: STBComponent tstbcomp@humaxdigital.com
	comment		:
	date    	: 2013/10/21
	attention 	:

	(c) 2011-2012 Humax Co., Ltd.
	This program is produced by Humax Co., Ltd. ("Humax") and
	the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
	non-assignable, non-transferable and non-exclusive license to use this Software.
	You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
	you agree to the responsibility to take all reasonable efforts to protect the any information
	you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
	reverse engineer or extract the source code of this Software unless you have Humax's written permission
	to do so.
	If you have no authorized license, discontinue using this Software immediately.

	THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES,
	EITHER EXPRESS, IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
	IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR
	INDIRECT, SPECIAL, CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR
	INABILITY TO USE THE SOFTWARE.

	This License is effective until terminated. You may terminate this License at any time
	by destroying all copies of the Software including all documentation. This License will terminate
	immediately without notice from Humax to you if you fail to comply with any provision of this License.
	Upon termination, you must destroy all copies of the Software and all documentation.

	The laws of the Republic of Korea will apply to any disputes arising out of or relating to
	this Copyright Notice. All claims arising out of or relating to this Copyright Notice will be litigated
	in the Seoul Central District Court, in the Republic of Korea.
*************************************************************************************************************/

#ifndef __SMOOTHSTREAMING_ABR_CONFIG_H__
#define __SMOOTHSTREAMING_ABR_CONFIG_H__

#include "ObjectInstance.h"

#define ABR_TYPE "SMOOTH_STREAMING_ABR"

namespace Lovefilm
{
	class SmoothStreamingAbrConfig: public ObjectInstance {
	public:
		SmoothStreamingAbrConfig();
        virtual ~SmoothStreamingAbrConfig();

        char *getABRType() { return (char *)ABR_TYPE; }
        int getBandWidthMultiplier() { return m_bandwidthMultiplier; }
        int getMaxBandwidth() { return m_maxBandwidth; }
        int getBandwidthSampleCount() { return m_bandwidthSampleCount; }
        int getLookForwardDuration() { return m_lookForwardDuration; }
        int getStartingBitrate() { return m_startingBitrate; }
        int getLowestPlayableBitrate() { return m_lowestPlayableBitrate; }
        int getHighestPlayableBitrate() { return m_highestPlayableBitrate; }
        bool getAllowMultiBitrateSwitch() { return m_allowMultiBitrateSwitch; }
        int getNoDownshiftBufferDuration() { return m_noDownshiftBufferDuration; }
        int getNoUpshiftBufferDuration() { return m_noUpshiftBufferDuration; }
        int getStreamingSlowFragmentDownloadTimeout() { return m_streamingSlowFragmentDownloadTimeout; }
        int getStreamingFragmentRetryDownswitchFactor() { return m_streamingFragmentRetryDownswitchFactor; }
        int getFragmentRetryDelayBackoff() { return m_fragmentRetryDelayBackoff; }
        int getCdnMaxTotalConnections() { return m_cdnMaxTotalConnections; }
        int getCdnMaxConnectionsPerRoute() { return m_cdnMaxConnectionsPerRoute; }
        
	protected:
		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);

    protected:
        int m_bandwidthMultiplier;
        int m_maxBandwidth;
        int m_bandwidthSampleCount;
        int m_lookForwardDuration;
        int m_startingBitrate;
        int m_lowestPlayableBitrate;
        int m_highestPlayableBitrate;
        bool m_allowMultiBitrateSwitch;
        int m_noDownshiftBufferDuration;
        int m_noUpshiftBufferDuration;
        int m_streamingSlowFragmentDownloadTimeout;
        int m_streamingFragmentRetryDownswitchFactor;
        int m_fragmentRetryDelayBackoff;
        int m_cdnMaxTotalConnections;
        int m_cdnMaxConnectionsPerRoute;
        
	};	

} /* namespace Lovefilm */

#endif	/* __SMOOTHSTREAMING_ABR_CONFIG_H__ */




