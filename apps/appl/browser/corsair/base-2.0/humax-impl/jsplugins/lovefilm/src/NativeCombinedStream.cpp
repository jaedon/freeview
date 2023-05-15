/*************************************************************************************************************
	File 		: NativeCombinedStream.cpp
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

#include "NativeCombinedStream.h"
#include "Log.h"

NJSEReturnCode native_CombinedStreamCreate(CombinedStreamHandle *id)
{
	*id = NULL;

	return NJSE_RETURN_OK;
}

NJSEReturnCode native_CombinedStreamReleaseHandle(CombinedStreamHandle id)
{
	if ( id )
	{
		free(id);
		id = NULL;
	}

	return NJSE_RETURN_OK;
}

NJSEReturnCode native_CombinedStreamGetStreamType(CombinedStreamHandle id, const char **retval)
{
	const char *stream_type = "CombinedStream";

	NJSE_LOG_DEBUG("Stream Type: %s\n", stream_type);

	*retval = stream_type;

	return NJSE_RETURN_OK;
}

NJSEReturnCode native_CombinedStreamIsDownloading(CombinedStreamHandle id, NJSEBoolean *retval)
{
	NJSEBoolean is_downloading = 1;

	NJSE_LOG_DEBUG("IsDownloading: %d\n", is_downloading);

	*retval = is_downloading;

	return NJSE_RETURN_OK;
}

NJSEReturnCode native_CombinedStreamIsActive(CombinedStreamHandle id, NJSEBoolean *retval)
{
	NJSEBoolean is_active = 1;

	NJSE_LOG_DEBUG("IsActive: %d\n", is_active);

	*retval = is_active;

	return NJSE_RETURN_OK;
}

NJSEReturnCode native_CombinedStreamGetDownloadingFragmentIndex(CombinedStreamHandle id, NJSENumber *retval)
{
	NJSENumber index = 0;

	NJSE_LOG_DEBUG("Downloading Fragment Index: %d\n", index);

	*retval = index;

	return NJSE_RETURN_OK;
}

NJSEReturnCode native_CombinedStreamGetPlayingFragmentIndex(CombinedStreamHandle id, NJSENumber *retval)
{
	NJSENumber index = 0;

	NJSE_LOG_DEBUG("Playing Fragment Index: %d\n", index);

	*retval = index;

	return NJSE_RETURN_OK;
}

NJSEReturnCode native_CombinedStreamGetServerIpAddr(CombinedStreamHandle id, const char **retval)
{
	const char *ipaddr = "10.0.13.35";

	NJSE_LOG_DEBUG("Server IP Address: %s\n", ipaddr);

	*retval = ipaddr;

	return NJSE_RETURN_OK;
}

NJSEReturnCode native_CombinedStreamGetBytesStreamed(CombinedStreamHandle id, NJSENumber *retval)
{
	NJSENumber bytes = 0;

	NJSE_LOG_DEBUG("Bytes Streamed: %d\n", bytes);

	*retval = bytes;

	return NJSE_RETURN_OK;
}
