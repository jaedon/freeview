/** **********************************************************************************************************
	@file 		corsair_fps_displayer.cpp
	
	@date		2014/12/30
	@author		Browser Part
	@breif		
	
	(c) 2014-2015 Humax Co., Ltd. 
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

#include "corsair_fps_displayer.h"
#include "corsair_log.h"
#include <iostream>
#include <sys/time.h>
#include <cstring>
#include <cstdio>

#include "logging/src/logging.h"

using namespace std;


/** **********************************************************************************************************
class CorsairFpsDisplayer
*********************************************************************************************************** */

CorsairFPSDisplayer::CorsairFPSDisplayer()
{
	m_log[0] = '\0';
	m_fps = 30.0;
	m_fps_filter = 30.0;
	m_now = 0;
	m_last_update = getCurrentTimeStamp();
}

CorsairFPSDisplayer::~CorsairFPSDisplayer()
{
}

void CorsairFPSDisplayer::update()
{
	m_now = getCurrentTimeStamp();
	double this_fps = (double)(1000 / (m_now - m_last_update));
	if ( m_now != m_last_update )
	{
		m_fps += (this_fps - m_fps) / m_fps_filter;
		m_last_update = m_now;
	}

	snprintf(m_log, sizeof(m_log), "[%s] FPS: %2.2f | Frequency: %2.2f", getDate(), m_fps, this_fps);
	LOG_DEBUG("%s \n",m_log);
}

long long CorsairFPSDisplayer::getCurrentTimeStamp(int mode)
{
	struct timeval te;

	gettimeofday(&te, NULL);

	switch(mode)
	{
	case FOR_FPS:
		return (te.tv_sec*1000LL + te.tv_usec/1000); 		// millisecond
	case FOR_TIMESTAMP:
		return (te.tv_sec*1000000LL + te.tv_usec);  // microsecond
	default:
		break;
	}

	return 0;
}

char *CorsairFPSDisplayer::getDate()
{
	static char szMsg[256] = {0,};
	struct tm *tm_data;
	time_t cur_time = time(NULL);
	tm_data = localtime(&cur_time);
	snprintf(szMsg, sizeof(szMsg), "--:%02d:%02d",
			//tm_data->tm_hour/*+13*/,
			tm_data->tm_min,
			tm_data->tm_sec );

	return szMsg;
}
