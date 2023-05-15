/* ************************************************************************************************************
	File 		: oplvideoutil.h
	author 		: BrowserPart
	comment		:
	date    	: 2016/01/30
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
************************************************************************************************************ */
#ifndef __OPLVIDEO_UTIL_H__
#define __OPLVIDEO_UTIL_H__

#include "autoconf.h"

#define OSD_WIDTH (CONFIG_MWC_OSD_HORIZONTAL)
#define OSD_HEIGHT (CONFIG_MWC_OSD_VERTICAL)

#ifdef __cplusplus
extern "C" {
#endif	//__cplusplus

/* **
    this function will get the fully fitted rect.
    we have to get rect size from the requested rect position, because we have to present video with the fitted rect size as keeping origianal ratio 
*/
void OplVideoUtil_getFittedPosition( int requested_x, int requested_y, int requested_w, int requested_h, \
                                     float stream_aspect_ratio, \
                                     int video_x, int video_y, int video_w, int video_h, \
                                     int *fitted_x, int *fitted_y, int *fitted_w, int *fitted_h);
/**
    get the rect position for video output, from requested position 
*/
void OplVideoUtil_getVideoOutputWindow( int requested_x, int requested_y, int requested_w, int requested_h, \
                                        int *video_out_x, int *video_out_y, int *video_out_w, int *video_out_h);
                                  
/**
    get the rect position for video intput, from video size position 
*/
void OplVideoUtil_getVideoInputWindow( int requested_x, int requested_y, int requested_w, int requested_h, \
                                       int video_x, int video_y, int video_w, int video_h, \
                                       int *video_in_x, int *video_in_y, int *video_in_w, int *video_in_h);

#ifdef __cplusplus
};
#endif	//__cplusplus

#endif //__OPLVIDEO_UTIL_H__
