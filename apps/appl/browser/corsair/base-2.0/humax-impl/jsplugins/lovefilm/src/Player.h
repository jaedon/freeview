/** **********************************************************************************************************
	@file 		player.h

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

#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "jsplugin.h"
#include "VisualObject.h"
#include "WrapperObject.h"
#include "NativePlayer.h"
#include "Collection.h"

namespace Lovefilm
{
	class PlayerPosition : public ObjectInstance
	{
	protected:
		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);

	public:
		PlayerPosition(int x, int y);
		virtual ~PlayerPosition();

	private:
		int m_x;
		int m_y;
	};

	class PlayerSize : public ObjectInstance
	{
	protected:
		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);

	public:
		PlayerSize(int width, int height);
		virtual ~PlayerSize();

	private:
		int m_width;
		int m_height;
	};

	class Player : public VisualObject, public WrapperObject<Player>
	{

	public:
		virtual NJSEReturnCode checkListenerAccessAllowed(const char *eventName);
		virtual NJSEReturnCode updateNativeWindowState(WindowState requested_state);		
		Player(PlayerHandle i);
		virtual ~Player();

		static void releaseNative(PlayerHandle id);		
		virtual void videoVisible(int visibility, int unloading);
		virtual void videoPosition(int x, int y, int w, int h);

		static int setAssetUrl(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int getAssetUrl(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int configureDrm(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int configureAbr(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int loadMetadata(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int getPlayReadyChallenge(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int setPlayReadyLicense(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int play(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int pause(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int seek(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int stop(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int unload(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int setPlaybackSpeed(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int setBufferTimeRequiredForInitialPlayback(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int setBufferTimeRequiredAfterExhaustion(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int setBufferTimeRequiredAfterStreamSwitch(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int setBufferTimeRequiredAfterSeeking(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int setFragmentsDownloadedEventFrequency(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int setActiveStream(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int getPosition(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int setPosition(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int getSize(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int setSize(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int getVolume(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int setVolume(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);

		static void onPlayerEvent(PlayerEvent event, PlayerEventInfo *info, PlayerHandle id);

		static const ClassType class_name = CLASS_PLAYER;

	protected:
		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
		int destructor(jsplugin_obj *obj);

	private:
		static list<Player*> instances;
		static Player* GetInstance(PlayerHandle id);		
		
		StreamTypesCollection *getAvailableStreams(int count, void* /*StreamTypesHandle*/ *handles);
		StreamTypesCollection *getActiveStreams(int count, void* /*StreamTypesHandle*/ *handles);
		StreamTypesCollection *getDownloadingStreams(int count, void* /*StreamTypesHandle*/ *handles);
		
	};
} /* namespace Lovefilm */

#endif // _PLAYER_H_

