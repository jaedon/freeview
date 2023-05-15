/*************************************************************************************************************
	File 		: NativeCombinedStreamMetadata.cpp
	author 		: STBComponent tstbcomp@humaxdigital.com
	comment		:
	date    	: 2013/10/21
	attention 	:

	(c) 2011-2012 Humax Co., Ltd.
	This program is produced by Humax Co., Ltd. (¡°Humax¡±) and
	the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
	non-assignable, non-transferable and non-exclusive license to use this Software.
	You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
	you agree to the responsibility to take all reasonable efforts to protect the any information
	you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
	reverse engineer or extract the source code of this Software unless you have Humax's written permission
	to do so.
	If you have no authorized license, discontinue using this Software immediately.

	THE SOFTWARE IS PROVIDED ¡°AS IS¡± AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES,
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

#include "NativeCombinedStreamMetadata.h"
#include "Log.h"

NJSEReturnCode native_CombinedStreamMetadataCreate(CombinedStreamMetadataHandle *id)
{
	*id = NULL;

	return NJSE_RETURN_OK;
}

NJSEReturnCode native_CombinedStreamMetadataReleaseHandle(CombinedStreamMetadataHandle id)
{
	if ( id )
	{
		free(id);
		id = NULL;
	}
	return NJSE_RETURN_OK;
}

NJSEReturnCode native_CombinedStreamMetadataGetStreamType(CombinedStreamMetadataHandle id, const char **retval)
{
	const char *stream_type = "CombinedStream";

	NJSE_LOG_DEBUG("Stream Type: %s\n", stream_type);

	*retval = stream_type;

	return NJSE_RETURN_OK;
}

NJSEReturnCode native_CombinedStreamMetadataGetAudioBitrate(CombinedStreamMetadataHandle id, NJSENumber *retval)
{
	NJSENumber bitrate = 10;

	NJSE_LOG_DEBUG("Audio Bitrate: %d\n", bitrate);

	*retval = bitrate;

	return NJSE_RETURN_OK;
}

NJSEReturnCode native_CombinedStreamMetadataGetVideoBitrate(CombinedStreamMetadataHandle id, NJSENumber *retval)
{
	NJSENumber bitrate = 10;

	NJSE_LOG_DEBUG("Video Bitrate: %d\n", bitrate);

	*retval = bitrate;

	return NJSE_RETURN_OK;
}

NJSEReturnCode native_CombinedStreamMetadataGetChannels(CombinedStreamMetadataHandle id, const char **retval)
{
	const char *channel = "1.0";

	NJSE_LOG_DEBUG("Channels: %s\n", channel);

	*retval = channel;

	return NJSE_RETURN_OK;
}

NJSEReturnCode native_CombinedStreamMetadataGetDrmType(CombinedStreamMetadataHandle id, const char **retval)
{
	const char *drm_type = "PLAY_READY_DRM";

	NJSE_LOG_DEBUG("DRM Type: %s\n", drm_type);

	*retval = drm_type;

	return NJSE_RETURN_OK;
}

NJSEReturnCode native_CombinedStreamMetadataGetDuration(CombinedStreamMetadataHandle id, NJSENumber *retval)
{
	NJSENumber duration = 120;

	NJSE_LOG_DEBUG("Duration: %d msecs\n", duration);

	*retval = duration;

	return NJSE_RETURN_OK;
}

NJSEReturnCode native_CombinedStreamMetadataGetLanguage(CombinedStreamMetadataHandle id, const char **retval)
{
	const char *language = "en";

	NJSE_LOG_DEBUG("Language: %s\n", language);

	*retval = language;

	return NJSE_RETURN_OK;
}

NJSEReturnCode native_CombinedStreamMetadataGetWidth(CombinedStreamMetadataHandle id, NJSENumber *retval)
{
	NJSENumber width = 120;

	NJSE_LOG_DEBUG("Width: %d\n", width);

	*retval = width;

	return NJSE_RETURN_OK;
}

NJSEReturnCode native_CombinedStreamMetadataGetHeight(CombinedStreamMetadataHandle id, NJSENumber *retval)
{
	NJSENumber height = 120;

	NJSE_LOG_DEBUG("Height: %d\n", height);

	*retval = height;

	return NJSE_RETURN_OK;
}
