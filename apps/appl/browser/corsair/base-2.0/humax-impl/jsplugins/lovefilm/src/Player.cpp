/** **********************************************************************************************************
	@file 		player.cpp

	@date		2013/10/21
	@author		STB-Component
	@breif

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
*********************************************************************************************************** */

#include "Player.h"
#include "DrmConfig.h"
#include "AbrConfig.h"
#include "BufferingTypes.h"
#include "MetadataTypes.h"
#include "StreamTypes.h"
#include "EmptyEvent.h"
#include "PlayerEvents.h"

#include <macros.h>

#include <cstring>

using namespace Lovefilm;

list<Player*> Player::instances;

PlayerPosition::PlayerPosition(int x, int y)
:m_x(x), m_y(y)
{
}

PlayerPosition::~PlayerPosition()
{
}

int PlayerPosition::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	GET_NUMBER("x", m_x, JSP_GET_VALUE);
	GET_NUMBER("y", m_y, JSP_GET_VALUE);
	
	return JSP_PUT_NOTFOUND;
}

PlayerSize::PlayerSize(int width, int height)
:m_width(width), m_height(height)
{
}

PlayerSize::~PlayerSize()
{
}

int PlayerSize::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	GET_NUMBER("width", m_width, JSP_GET_VALUE);
	GET_NUMBER("height", m_height, JSP_GET_VALUE);
	
	return JSP_PUT_NOTFOUND;
}

Player::Player(PlayerHandle i) : WrapperObject<Player>(i)
{
	classType = CLASS_PLAYER;
	Player::instances.push_back(this);
	NATIVE_WARN_NOK(native_PlayerSetListener(Player::onPlayerEvent));
}

Player::~Player()
{
	Player::instances.remove(this);
}

Player* Player::GetInstance(PlayerHandle id)
{
	for (list<Player*>::iterator it=instances.begin(); it!=instances.end(); it++)
	{
		if ((*it)->getIdentifier() == id)
			return *it;
	}
	return NULL;
}

int Player::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	GET_OBJECT("bufferingTypes", obj, new BufferingTypes(), JSP_GET_VALUE);
	GET_OBJECT("drmConfig", obj, new DrmConfig(), JSP_GET_VALUE);
	GET_OBJECT("abrConfig", obj, new AbrConfig(), JSP_GET_VALUE);
	NATIVE_GET_OBJECT("metadataTypes", obj, native_MetadataTypesCreate(&RETVAL), new MetadataTypes(RETVAL), JSP_GET_VALUE);
	NATIVE_GET_OBJECT("streamTypes", obj, native_StreamTypesCreate(&RETVAL), new StreamTypes(RETVAL), JSP_GET_VALUE);

	INTRINSIC_EVENT_GET(player_connection_attempted);
	INTRINSIC_EVENT_GET(player_connection_established);
	INTRINSIC_EVENT_GET(player_metadata);
	INTRINSIC_EVENT_GET(player_stream_started_downloading);
	INTRINSIC_EVENT_GET(yer_stream_stopped_downloading);
	INTRINSIC_EVENT_GET(player_stream_became_active);
	INTRINSIC_EVENT_GET(player_buffer_started);
	INTRINSIC_EVENT_GET(player_buffer_completed);
	INTRINSIC_EVENT_GET(player_fragments_downloaded);
	INTRINSIC_EVENT_GET(player_ready);
	INTRINSIC_EVENT_GET(player_stream_switch_started);
	INTRINSIC_EVENT_GET(player_stream_switch_completed);
	INTRINSIC_EVENT_GET(player_playing);
	INTRINSIC_EVENT_GET(player_paused);
	INTRINSIC_EVENT_GET(player_seek_started);
	INTRINSIC_EVENT_GET(player_seek_completed);
	INTRINSIC_EVENT_GET(player_playback_speed_changed);
	INTRINSIC_EVENT_GET(player_volume_changed);
	INTRINSIC_EVENT_GET(player_stopped);
	INTRINSIC_EVENT_GET(player_ended);
	INTRINSIC_EVENT_GET(player_unloaded);

	NATIVE_GET_VOIDP_COLLECTION("availableStreams", obj, 
								native_PlayerGetAvailableStreams(&SIZE, &NATIVE_ARRAY),
								getAvailableStreams(SIZE, NATIVE_ARRAY), JSP_GET_VALUE);

	NATIVE_GET_VOIDP_COLLECTION("activeStreams", obj, 
								native_PlayerGetActiveStreams(&SIZE, &NATIVE_ARRAY),
								getActiveStreams(SIZE, NATIVE_ARRAY), JSP_GET_VALUE);

	NATIVE_GET_VOIDP_COLLECTION("downloadingStreams", obj, 
								native_PlayerGetDownloadingStreams(&SIZE, &NATIVE_ARRAY),
								getDownloadingStreams(SIZE, NATIVE_ARRAY), JSP_GET_VALUE);

	NATIVE_GET_NUMBER("currentTime", native_PlayerGetCurrentTime(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("currentBufferSize", native_PlayerGetCurrentBufferSize(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("currentBufferTime", native_PlayerGetCurrentBufferTime(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("maxBufferSize", native_PlayerGetMaxBufferSize(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("droppedFrames", native_PlayerGetDroppedFrames(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("averageFrameRate", native_PlayerGetAverageFrameRate(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("averageBandwidth", native_PlayerGetAverageBandwidth(&RETVAL), JSP_GET_VALUE);

	GET_FUNCTION(setAssetUrl, obj, "s", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(getAssetUrl, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(configureDrm, obj, "o", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(configureAbr, obj, "o", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(loadMetadata, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(getPlayReadyChallenge, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(setPlayReadyLicense, obj, "s", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(play, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(pause, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(seek, obj, "n", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(stop, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(unload, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(setPlaybackSpeed, obj, "n", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(setBufferTimeRequiredForInitialPlayback, obj, "n", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(setBufferTimeRequiredAfterExhaustion, obj, "n", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(setBufferTimeRequiredAfterStreamSwitch, obj, "n", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(setBufferTimeRequiredAfterSeeking, obj, "n", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(setFragmentsDownloadedEventFrequency, obj, "n", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(setActiveStream, obj, "so", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(getPosition, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(setPosition, obj, "nn", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(getSize, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(setSize, obj, "nn", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(getVolume, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(setVolume, obj, "n", JSP_GET_VALUE_CACHE);

	return EventTarget::getter(obj, name, result);
}

int Player::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	INTRINSIC_EVENT(player_connection_attempted);
	INTRINSIC_EVENT(player_connection_established);
	INTRINSIC_EVENT(player_metadata);
	INTRINSIC_EVENT(player_stream_started_downloading);
	INTRINSIC_EVENT(yer_stream_stopped_downloading);
	INTRINSIC_EVENT(player_stream_became_active);
	INTRINSIC_EVENT(player_buffer_started);
	INTRINSIC_EVENT(player_buffer_completed);
	INTRINSIC_EVENT(player_fragments_downloaded);
	INTRINSIC_EVENT(player_ready);
	INTRINSIC_EVENT(player_stream_switch_started);
	INTRINSIC_EVENT(player_stream_switch_completed);
	INTRINSIC_EVENT(player_playing);
	INTRINSIC_EVENT(player_paused);
	INTRINSIC_EVENT(player_seek_started);
	INTRINSIC_EVENT(player_seek_completed);
	INTRINSIC_EVENT(player_playback_speed_changed);
	INTRINSIC_EVENT(player_volume_changed);
	INTRINSIC_EVENT(player_stopped);
	INTRINSIC_EVENT(player_ended);
	INTRINSIC_EVENT(player_unloaded);

	return JSP_PUT_NOTFOUND;
}

int Player::destructor(jsplugin_obj *obj)
{
	return JSP_DESTROY_OK;
}

void Player::releaseNative(PlayerHandle id)
{
	ENTRY;

	NATIVE_WARN_NOK(native_PlayerReleaseHandle(id));
}

NJSEReturnCode Player::updateNativeWindowState(WindowState requested_state)
{
	return native_PlayerSetFullScreen(getIdentifier(), requested_state == FULLSCREEN);
}

NJSEReturnCode Player::checkListenerAccessAllowed(const char *eventName)
{
	return native_PlayerOnListenerAccessed(identifier, eventName);
}

void Player::videoVisible(int visibility, int unloading)
{
	if (toggleVisibility(visibility))
	{
		NATIVE_WARN_NOK(native_PlayerSetVisibility(getIdentifier(), visibility, unloading));
	}
}

void Player::videoPosition(int x, int y, int w, int h)
{
	if (resize(x, y, w, h))
	{
		NATIVE_WARN_NOK(native_PlayerSetVideoWindow(getIdentifier(), x, y, w, h));
	}
}

StreamTypesCollection *Player::getAvailableStreams(int count, StreamTypesHandle *handles)
{
	ENTRY;
	StreamTypesCollection *collection = new StreamTypesCollection;
	for (int i=0; i<count; i++) {
		collection->addItem(new StreamTypes(handles[i]));
	}

	if( handles )
	{
		delete [] handles;
	}
	return collection;
}

StreamTypesCollection *Player::getActiveStreams(int count, StreamTypesHandle *handles)
{
	ENTRY;
	StreamTypesCollection *collection = new StreamTypesCollection;
	for (int i=0; i<count; i++) {
		collection->addItem(new StreamTypes(handles[i]));
	}

	if( handles )
	{
		delete [] handles;
	}
	return collection;
}

StreamTypesCollection *Player::getDownloadingStreams(int count, StreamTypesHandle *handles)
{
	ENTRY;
	StreamTypesCollection *collection = new StreamTypesCollection;
	for (int i=0; i<count; i++) {
		collection->addItem(new StreamTypes(handles[i]));
	}

	if( handles )
	{
		delete [] handles;
	}
	return collection;
}

int	Player::setAssetUrl(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(1);
	
	Player *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, Player);

	const char *url;
	if (TYPE_STRING(argv[0]))
	{
		url = STRING_VALUE(argv[0]);
	}
	else
	{
		url = NULL;
	}

	NJSEBoolean retval;

	NATIVE_CALL(native_PlayerSetAssetUrl(instance->getIdentifier(), url, &retval));

	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}

int	Player::getAssetUrl(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(0);

	Player *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, Player);
	
	char* retval = NULL;

	NATIVE_CALL(native_PlayerGetAssetUrl(instance->getIdentifier(), &retval));
	
	RETURN_STRING(retval, JSP_CALL_VALUE);
}

int	Player::configureDrm(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(1);

	Player *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, Player);
	DrmConfig *config = OBJECT_VALUE_OR_THROW(config, argv[0], DrmConfig);

	NJSEBoolean retval = TRUE;

	NATIVE_CALL(native_PlayerConfigureDrm(instance->getIdentifier(), (void *)config, &retval));

	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);	
}

int	Player::configureAbr(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(1);

	Player *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, Player);
	AbrConfig *config = OBJECT_VALUE_OR_THROW(config, argv[0], AbrConfig);

	NJSEBoolean retval = TRUE;

	NATIVE_CALL(native_PlayerConfigureAbr(instance->getIdentifier(), (void *)config, &retval));

	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);	
}

int	Player::loadMetadata(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(0);
	Player *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, Player);

	NJSEBoolean retval = TRUE;

	NATIVE_CALL(native_PlayerLoadMetadata(instance->getIdentifier(), &retval));
	/*
		Device.player.player_connection_attempted
		Device.player.player_connection_established
		Device.player.player_metadata 
	*/

	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);	
}

int	Player::getPlayReadyChallenge(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(0);
	Player *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, Player);

	char * retval = NULL;
	
	NATIVE_CALL(native_PlayerGetPalyReadyChallenge(instance->getIdentifier(), &retval));

	RETURN_STRING(retval, JSP_CALL_VALUE);
}

int	Player::setPlayReadyLicense(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(1);
	Player *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, Player);

	const char *license;
	if (TYPE_STRING(argv[0]))
		license = STRING_VALUE(argv[0]);
	else
		license = NULL;

	NJSEBoolean retval = TRUE;

	if(license)
		NATIVE_CALL(native_PlayerSetPlayReadyLicense(instance->getIdentifier(), license, &retval));
	else
		retval = FALSE;

	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);

}

int	Player::play(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(0);
	Player *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, Player);

	NJSEBoolean retval = TRUE;

	NATIVE_CALL(native_PlayerPlay(instance->getIdentifier(), &retval));

	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);	
}

int	Player::pause(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(0);
	Player *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, Player);

	NJSEBoolean retval = TRUE;

	NATIVE_CALL(native_PlayerPause(instance->getIdentifier(), &retval));

	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);	
}

int	Player::seek(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(1);
	Player *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, Player);

	NJSEBoolean retval = FALSE;
	NJSENumber time = 0;
	if (TYPE_NUMBER(argv[0]))
	{
		time = NUMBER_VALUE(argv[0]);
	}
	else
		RETURN_BOOLEAN(retval, JSP_CALL_VALUE);

	NATIVE_CALL(native_PlayerSeek(instance->getIdentifier(), time, &retval));

	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);	
}

int	Player::stop(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(0);
	Player *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, Player);

	NJSEBoolean retval = TRUE;

	NATIVE_CALL(native_PlayerStop(instance->getIdentifier(), &retval));

	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);	
}

int	Player::unload(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(0);
	Player *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, Player);

	NJSEBoolean retval	= TRUE;

	NATIVE_CALL(native_PlayerUnload(instance->getIdentifier(), &retval));

	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);	
}

int	Player::setPlaybackSpeed(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(1);
	Player *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, Player);

	NJSEBoolean retval = FALSE;
	NJSENumber speed = 0;
	if (TYPE_NUMBER(argv[0]))
	{
		speed = NUMBER_VALUE(argv[0]);
	}
	else
		RETURN_BOOLEAN(retval, JSP_CALL_VALUE);


	NATIVE_CALL(native_PlayerSetPlaybackSpeed(instance->getIdentifier(), speed, &retval));

	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);	
}

int	Player::setBufferTimeRequiredForInitialPlayback(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(1);
	Player *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, Player);

	NJSEBoolean retval = FALSE;
	NJSENumber time = 0;
	if (TYPE_NUMBER(argv[0]))
	{
		time = NUMBER_VALUE(argv[0]);
	}
	else
		RETURN_BOOLEAN(retval, JSP_CALL_VALUE);


	NATIVE_CALL(native_PlayerSetBufferTimeRequiredForInitialPlayback(instance->getIdentifier(), time, &retval));

	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);	
}

int	Player::setBufferTimeRequiredAfterExhaustion(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(1);
	Player *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, Player);

	NJSEBoolean retval	= FALSE;
	NJSENumber time = 0;
	if (TYPE_NUMBER(argv[0]))
	{
		time = NUMBER_VALUE(argv[0]);
	}
	else
		RETURN_BOOLEAN(retval, JSP_CALL_VALUE);


	NATIVE_CALL(native_PlayerSetBufferTimeRequiredAfterExhaustion(instance->getIdentifier(), time, &retval));

	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);	
}

int	Player::setBufferTimeRequiredAfterStreamSwitch(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(1);
	Player *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, Player);

	NJSEBoolean retval = FALSE;
	NJSENumber time = 0;
	if (TYPE_NUMBER(argv[0]))
	{
		time = NUMBER_VALUE(argv[0]);
	}
	else
		RETURN_BOOLEAN(retval, JSP_CALL_VALUE);


	NATIVE_CALL(native_PlayerSetBufferTimeRequiredAfterStreamSwitch(instance->getIdentifier(), time, &retval));

	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);	
}

int	Player::setBufferTimeRequiredAfterSeeking(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(1);
	Player *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, Player);

	NJSEBoolean retval = FALSE;
	NJSENumber time = 0;
	if (TYPE_NUMBER(argv[0]))
	{
		time = NUMBER_VALUE(argv[0]);
	}
	else
		RETURN_BOOLEAN(retval, JSP_CALL_VALUE);


	NATIVE_CALL(native_PlayerSetBufferTimeRequiredAfterSeeking(instance->getIdentifier(), time, &retval));

	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);	
}

int	Player::setFragmentsDownloadedEventFrequency(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(1);
	Player *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, Player);

	NJSEBoolean retval = FALSE;
	NJSENumber frequency = 0;
	if (TYPE_NUMBER(argv[0]))
	{
		frequency = NUMBER_VALUE(argv[0]);
	}
	else
		RETURN_BOOLEAN(retval, JSP_CALL_VALUE);


	NATIVE_CALL(native_PlayerSetFragmentsDownloadedEventFrequency(instance->getIdentifier(), frequency, &retval));

	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);	
}

int	Player::setActiveStream(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(2);
	Player *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, Player);

	NJSEBoolean retval = FALSE;
	const char * type = NULL;
	
	if (TYPE_STRING(argv[0]))
	{
		type = STRING_VALUE(argv[0]);
	}
	else
		RETURN_BOOLEAN(retval, JSP_CALL_VALUE);

	StreamTypes *stream = OBJECT_VALUE_OR_THROW(stream, argv[1], StreamTypes);

	NATIVE_CALL(native_PlayerSetActiveStream(instance->getIdentifier(), type, stream->getIdentifier(), &retval));

	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);	
}

int	Player::getPosition(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(0);
	Player *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, Player);

	NJSENumber x = 0, y = 0;
	
	NATIVE_CALL(native_PlayerGetPosition(instance->getIdentifier(), &x, &y));

	RETURN_OBJECT(this_obj, new PlayerPosition(x, y), result, JSP_CALL_VALUE);	
}

int	Player::setPosition(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(2);
	Player *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, Player);

	NJSEBoolean retval = FALSE;
	NJSENumber x, y;
	
	if (TYPE_NUMBER(argv[0]) && TYPE_NUMBER(argv[1]))
	{
		x = NUMBER_VALUE(argv[0]);
		y = NUMBER_VALUE(argv[1]);
	}
	else
		RETURN_BOOLEAN(retval, JSP_CALL_VALUE);

	NATIVE_CALL(native_PlayerSetPosition(instance->getIdentifier(),  x,  y, &retval));

	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);	
}

int	Player::getSize(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(0);
	Player *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, Player);
	NJSENumber width = 0, height = 0;

	NATIVE_CALL(native_PlayerGetSize(instance->getIdentifier(), &width, &height));

	RETURN_OBJECT(this_obj, new PlayerPosition(width, height), result, JSP_CALL_VALUE);	
}

int	Player::setSize(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(2);
	Player *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, Player);

	NJSEBoolean retval = FALSE;
	NJSENumber width, height;
	
	if (TYPE_NUMBER(argv[0]) && TYPE_NUMBER(argv[1]))
	{
		width = NUMBER_VALUE(argv[0]);
		height = NUMBER_VALUE(argv[1]);
	}
	else
		RETURN_BOOLEAN(retval, JSP_CALL_VALUE);

	NATIVE_CALL(native_PlayerSetSize(instance->getIdentifier(), width, height, &retval));

	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);	
}

int	Player::getVolume(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(0);
	Player *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, Player);

	NJSENumber volume;

	NATIVE_CALL(native_PlayerGetVolume(instance->getIdentifier(), &volume));

	RETURN_NUMBER(volume, JSP_CALL_VALUE);	
}

int	Player::setVolume(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(1);
	Player *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, Player);

	NJSEBoolean retval = FALSE;
	NJSENumber volume;
	
	if (TYPE_NUMBER(argv[0]))
	{
		volume = NUMBER_VALUE(argv[0]);
	}
	else
		RETURN_BOOLEAN(retval, JSP_CALL_VALUE);

	NATIVE_CALL(native_PlayerSetVolume(instance->getIdentifier(), volume, &retval));
	
	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);	
}

void Player::onPlayerEvent(PlayerEvent event, PlayerEventInfo *info, PlayerHandle id)
{
	Player* player = GetInstance(id);
	if (!player)
	{
		NJSE_LOG_ERROR("Unknown PlayerHandle received");
		return;
	}

	Event *e = NULL;
	jsplugin_obj *context = player->getHost();

	switch(event)
	{
		case PLAYER_EVENT_CONNECTION_ATTEMPTED:			
			e = new EmptyEvent(context, "player_connection_attempted");
			break;
			
		case PLAYER_EVENT_CONNECTION_ESTABLISHED:
			e = new EmptyEvent(context, "player_connection_established");
			break;
			
		case PLAYER_EVENT_METADATA:
			e = new EmptyEvent(context, "player_metadata");			
			break;
			
		case PLAYER_EVENT_STREAM_STARTED_DOWNLOADING:
			e = new PlayerStreamStartedDownloadingEvent(context, info->player_stream_started_downloading.stream);
			break;
			
		case PLAYER_EVENT_STREAM_STOPPED_DOWNLOADING:
			e = new PlayerStreamStoppedDownloadingEvent(context, info->player_stream_stopped_downloading.stream);
			break;
			
		case PLAYER_EVENT_STREAM_BECAME_ACTIVE:
			e = new PlayerStreamBecameActiveEvent(context, info->player_stream_became_active.stream);
			break;
			
		case PLAYER_EVENT_BUFFER_STARTED:
			e = new PlayerBufferStartedEvent(context, info->player_buffer_started.type, info->player_buffer_started.stream);
			break;
			
		case PLAYER_EVENT_BUFFER_COMPLETED:
			e = new PlayerBufferCompletedEvent(context, info->player_buffer_completed.type, info->player_buffer_completed.stream);
			break;
			
		case PLAYER_EVENT_FRAGMENTS_DOWNLOADED:
			e = new PlayerFragmentsDownloadedEvent(context);
			break;
			
		case PLAYER_EVENT_READY:			
			e = new EmptyEvent(context, "player_ready");
			break;
			
		case PLAYER_EVENT_STREAM_SWITCH_STARTED:
			break;
			
		case PLAYER_EVENT_STREAM_SWITCH_COMPLETED:
			break;
			
		case PLAYER_EVENT_PLAYING:
			e = new EmptyEvent(context, "player_playing");
			break;
			
		case PLAYER_EVENT_PAUSED:
			e = new EmptyEvent(context, "player_paused");
			break;
			
		case PLAYER_EVENT_SEEK_STARTED:
			break;
			
		case PLAYER_EVENT_SEEK_COMPLETED:
			break;
			
		case PLAYER_EVENT_PLAYBACK_SPEED_CHANGED:
			break;
			
		case PLAYER_EVENT_VOLUME_CHANGED:
			break;
			
		case PLAYER_EVENT_STOPPED:
			e = new EmptyEvent(context, "player_stopped");			
			break;
			
		case PLAYER_EVENT_ENDED:
			e = new EmptyEvent(context, "player_ended");
			break;
			
		case PLAYER_EVENT_UNLOADED:
			e = new EmptyEvent(context, "player_unloaded");			
			break;

		default:
			break;
	}
	player->dispatchEvent(e);
}

