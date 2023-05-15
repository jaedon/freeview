/*************************************************************************************************************
	File 		: NativeHAirplay.h
	author 		: STB-Component@humaxdigital.com
	comment		: 
	date    	: 2013/09/05
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


#ifndef __NATIVE_HAIRPLAY__
#define __NATIVE_HAIRPLAY__

#include "NativeOOIFTypes.h"


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#if defined OIPF

typedef void	(*AirplayEventListener)(unsigned int event, unsigned long long decimal, char* pUrl, char* pPath, char* pTitle, char* pAlbum, char* pArtist);

typedef enum
{
	eNATIVE_AIRPLAY_REQ_PLAY,
	eNATIVE_AIRPLAY_REQ_STOP,
	eNATIVE_AIRPLAY_REQ_SHOWPHOTO,
	eNATIVE_AIRPLAY_REQ_SETPOSITION,
	eNATIVE_AIRPLAY_REQ_SETRATE,
	eNATIVE_AIRPLAY_REQ_SETVOLUME,
	eNATIVE_AIRTUNES_REQ_PLAY,
	eNATIVE_AIRTUNES_REQ_STOP,
	eNATIVE_AIRTUNES_REQ_FLUSH,
	eNATIVE_AIRTUNES_REQ_SETMETADATA,
	eNATIVE_AIRTUNES_REQ_SETALBUMART,
	eNATIVE_AIRPLAY_REQ_UNKNOWN
} eNATIVE_AIRPLAY_Request_e;

OOIFReturnCode 	native_Airplay_Start(OOIFBoolean *retval);
OOIFReturnCode 	native_Airplay_Stop(OOIFBoolean *retval);
bool 			native_Airplay_IsStarted();
bool			native_Airplay_IsActivated();
OOIFReturnCode 	native_Airplay_RegEventListener(AirplayEventListener listener);
OOIFReturnCode 	native_Airplay_UnRegEventListener();

/* Set playback status */
OOIFReturnCode  native_Airplay_SetPlaybackStatusInfo(const char* status);
OOIFReturnCode  native_Airplay_SetBufferedRangeInfo(long long int buffered_range);
OOIFReturnCode  native_Airplay_SetSeekableRangeInfo(long long int seekable_range);
OOIFReturnCode  native_Airplay_SetDurationInfo(long long int duration);
OOIFReturnCode  native_Airplay_SetPositionInfo(long long int position);
OOIFReturnCode  native_Airplay_SetStartPositionInfo(long long int start_position);
OOIFReturnCode  native_Airplay_SetRateInfo(int rate);

#endif /* OIPF */
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __NATIVE_HDLNA__ */
