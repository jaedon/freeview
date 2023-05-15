/** **********************************************************************************************************
	@file 		hmp_backend.h
	
	@date		2012/11/12
	@author		IPM-SW1T (tipmsw1@humaxdigital.com)
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

#ifndef __HMP_BACKEND_H_
#define __HMP_BACKEND_H_

#include <uva_backend.h>

class OplVodAvcHandle;

typedef struct _hmp_winrect_t {
	int x;
	int y;
	int w;
	int h;
} hmp_winrect_t;

/***************************************************************************************************
 * HmpMediaBackend
 ***************************************************************************************************/
class HmpMediaBackend : public UVAMediaBackend {
public:
	HmpMediaBackend(UVABackendType type, UVAClient *client);
	virtual ~HmpMediaBackend();

	virtual UVA_STATUS initBackend(const char *origin, SourceDescription **sources,
						bool automatic_key_system_selection);
	virtual UVA_STATUS attachSource(SourceDescription *source);
	virtual UVA_STATUS removeSource(uintptr_t source_id);
	virtual UVA_STATUS setVideoWindow(int x, int y, int w, int h);
	virtual UVA_STATUS setClipRect(int x, int y, int w, int h);
	virtual UVA_STATUS getNativeSize(int &w, int &h, double &pixel_aspect);
	virtual UVA_STATUS getSeekableTimeRanges(ByteRange **input_byte_ranges,
						TimeRange **&output_time_ranges);
	virtual UVA_STATUS getBufferedTimeRanges(ByteRange **input_byte_ranges,
						TimeRange **&output_time_ranges);
	virtual UVA_STATUS canPlayURL(const char *url);
	virtual UVA_STATUS play();
	virtual UVA_STATUS stop();
	virtual UVA_STATUS pause(bool get_resources);
	virtual UVA_STATUS setPlaybackRate(double rate);
	virtual UVA_STATUS setVolume(double volume);
	virtual UVA_STATUS getDuration(double &duration);
	virtual UVA_STATUS getPosition(double &position);
	virtual UVA_STATUS setPosition(double position);
	virtual UVA_STATUS setVisibility(bool visible, bool unloaded);
	virtual UVA_STATUS getSizeFromDuration(double duration, size_t &size);
	virtual UVA_STATUS writeData(UVAEvent *need_data_event, size_t bytes_written,
						WriteDataStatus status);
	virtual UVA_STATUS canPlayType(const char *type, const char **codecs,
						const char *drmID);
	virtual UVA_STATUS setFullScreen(bool fullscreen);
	virtual UVA_STATUS getSupportedMimetypes(char **&mime_types);
	
	/* for OIPF */
	virtual UVA_STATUS getComponents(AVComponent **&components);
	virtual UVA_STATUS unselectComponent(AVComponent *component);
	virtual UVA_STATUS selectComponent(AVComponent *component);

	UVAClient *getClient() { return m_uva_client; }
	OplVodAvcHandle *getPlayer() { return m_player; }

public:
	static HmpMediaBackend *s_theApp;
	static hmp_winrect_t	s_latest_window_rect;

	static void onEventCallback(void *handle, int type);
	static void onPlayStateChanged(void *handle);
	static void onBufferStateChanged(void *handle);
	static void onPlayPositionChanged(void *handle);

private:
	UVABackendType 		m_uva_type;
	UVAClient			*m_uva_client;
	OplVodAvcHandle		*m_player;
};

static inline HmpMediaBackend *getBackend() { return HmpMediaBackend::s_theApp; }

#endif //__HMP_BACKEND_H_
