/*************************************************************************************************************
	File 		: DrmConfig.cpp
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

#include "SmoothStreamingAbrConfig.h"
#include "macros.h"

#include <cstring>

using namespace Lovefilm;

SmoothStreamingAbrConfig::SmoothStreamingAbrConfig()
{
	classType = CLASS_SMOOTHSTREAMINGABRCONFIG;
}

int SmoothStreamingAbrConfig::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	GET_STRING("abrType", ABR_TYPE, JSP_GET_VALUE);
	GET_NUMBER("bandwidthMultiplier", m_bandwidthMultiplier, JSP_GET_VALUE);
	GET_NUMBER("maxBandwidth", m_maxBandwidth, JSP_GET_VALUE);
	GET_NUMBER("bandwidthSampleCount", m_bandwidthSampleCount, JSP_GET_VALUE);
	GET_NUMBER("lookForwardDuration", m_lookForwardDuration, JSP_GET_VALUE);
	GET_NUMBER("startingBitrate", m_startingBitrate, JSP_GET_VALUE);
	GET_NUMBER("lowestPlayableBitrate", m_lowestPlayableBitrate, JSP_GET_VALUE);
	GET_NUMBER("highestPlayableBitrate", m_highestPlayableBitrate, JSP_GET_VALUE);
	GET_BOOLEAN("allowMultiBitrateSwitch", m_allowMultiBitrateSwitch, JSP_GET_VALUE);
	GET_NUMBER("noDownshiftBufferDuration", m_noDownshiftBufferDuration, JSP_GET_VALUE);
	GET_NUMBER("noUpshiftBufferDuration", m_noUpshiftBufferDuration, JSP_GET_VALUE);
	GET_NUMBER("streamingSlowFragmentDownloadTimeout", m_streamingSlowFragmentDownloadTimeout, JSP_GET_VALUE);
	GET_NUMBER("streamingFragmentRetryDownswitchFactor", m_streamingFragmentRetryDownswitchFactor, JSP_GET_VALUE);
	GET_NUMBER("fragmentRetryDelayBackoff", m_fragmentRetryDelayBackoff, JSP_GET_VALUE);
	GET_NUMBER("cdnMaxTotalConnections", m_cdnMaxTotalConnections, JSP_GET_VALUE);
	GET_NUMBER("cdnMaxConnectionsPerRoute", m_cdnMaxConnectionsPerRoute, JSP_GET_VALUE);

	return JSP_GET_NOTFOUND;
}


int SmoothStreamingAbrConfig::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	SET_READONLY("abrType");
	SET_NUMBER("bandwidthMultiplier", m_bandwidthMultiplier, JSP_PUT_OK);
	SET_NUMBER("maxBandwidth", m_maxBandwidth, JSP_PUT_OK);
	SET_NUMBER("bandwidthSampleCount", m_bandwidthSampleCount, JSP_PUT_OK);
	SET_NUMBER("lookForwardDuration", m_lookForwardDuration, JSP_PUT_OK);
	SET_NUMBER("startingBitrate", m_startingBitrate, JSP_PUT_OK);
	SET_NUMBER("lowestPlayableBitrate", m_lowestPlayableBitrate, JSP_PUT_OK);
	SET_NUMBER("highestPlayableBitrate", m_highestPlayableBitrate, JSP_PUT_OK);
	SET_BOOLEAN("allowMultiBitrateSwitch", m_allowMultiBitrateSwitch, JSP_PUT_OK);
	SET_NUMBER("noDownshiftBufferDuration", m_noDownshiftBufferDuration, JSP_PUT_OK);
	SET_NUMBER("noUpshiftBufferDuration", m_noUpshiftBufferDuration, JSP_PUT_OK);
	SET_NUMBER("streamingSlowFragmentDownloadTimeout", m_streamingSlowFragmentDownloadTimeout, JSP_PUT_OK);
	SET_NUMBER("streamingFragmentRetryDownswitchFactor", m_streamingFragmentRetryDownswitchFactor, JSP_PUT_OK);
	SET_NUMBER("fragmentRetryDelayBackoff", m_fragmentRetryDelayBackoff, JSP_PUT_OK);
	SET_NUMBER("cdnMaxTotalConnections", m_cdnMaxTotalConnections, JSP_PUT_OK);
	SET_NUMBER("cdnMaxConnectionsPerRoute", m_cdnMaxConnectionsPerRoute, JSP_PUT_OK);

	return JSP_GET_NOTFOUND;
}

