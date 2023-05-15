/*************************************************************************************************************
	File 		: OplVideoUtil.cpp
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
*************************************************************************************************************/
#include <stdlib.h>
#include <stdio.h>

#include "oplvideoutil.h"
#include "oplvodhandle_def.h"

void OplVideoUtil_getFittedPosition( int requested_x, int requested_y, int requested_w, int requested_h, \
                                     float stream_aspect_ratio,
                                     int video_x, int video_y, int video_w, int video_h, \
                                     int *fitted_x, int *fitted_y, int *fitted_w, int *fitted_h)
{
    if ( video_w == 0 || video_h == 0 || requested_w == 0 || requested_h == 0 )
        return;
        
    /* sometimes, the stream ratio information is different with which get from stream resolution. 
       but at this time, I respect the stream ratio information more than stream resolution. */
    float video_ratio = ( stream_aspect_ratio != (float) 0 ) ? stream_aspect_ratio : ( (float)video_w / (float)video_h );
    float requested_ratio = ( (float)requested_w / (float)requested_h );
    
    /* we will compare only first digit after dot. maybe some floading value can not be equal by many digitals like 1.0000000000001 and 1.0*/
    int first_digit_video_ratio = (int)( video_ratio * 10 );
    int first_digit_requested_ratio = (int)( requested_ratio * 10 );
    
    /* in the case that each ratio is same, we do not need to adjust position */    
    if ( first_digit_video_ratio == first_digit_requested_ratio )
    {
        *fitted_x = requested_x;
        *fitted_y = requested_y;
        *fitted_w = requested_w;
        *fitted_h = requested_h;
    }
    /* if video_ratio is bigger than requested_ratio, it means that the video shall be fitted by requested_w */
    else if ( first_digit_video_ratio > first_digit_requested_ratio )
    {
        *fitted_w = requested_w;
        *fitted_h = (int)((*fitted_w) * ( 1 / video_ratio ));
        *fitted_x = requested_x;
        *fitted_y = requested_y +  ( requested_h >> 1 ) - ((*fitted_h) >> 1);
    }
    /* if video_ratio is bigger than requested_ratio, it means that the video shall be fitted by requested_w */
    else if ( first_digit_requested_ratio > first_digit_video_ratio )
    {
        *fitted_h = requested_h;
        *fitted_w = (*fitted_h) * ( video_ratio );
        *fitted_x = requested_x +  ( requested_w >> 1 ) - ((*fitted_w) >> 1);
        *fitted_y = requested_y;
    }
    return;    
}
 
void OplVideoUtil_getVideoOutputWindow( int requested_x, int requested_y, int requested_w, int requested_h, \
                                        int *video_out_x, int *video_out_y, int *video_out_w, int *video_out_h)
{
    /* adjusted request postion because */
    /* set out window */
    if ( requested_x < 0 )
    {
        *video_out_x = 0;
        if ( requested_x + requested_w < OSD_WIDTH )
        {
            *video_out_w = ( requested_x + requested_w );
        }
        else
        {
            *video_out_w = OSD_WIDTH;
        }
    }
    else 
    {
        *video_out_x = requested_x;
        if ( requested_x + requested_w < OSD_WIDTH )
        {
            *video_out_w = requested_w;
        }
        else
        {
            *video_out_w = OSD_WIDTH - requested_x;
        }
    }
    
    if ( requested_y < 0 )
    {
        *video_out_y = 0;
        if ( requested_y + requested_h < OSD_HEIGHT )
        {
            *video_out_h = ( requested_y + requested_h );
        }
        else
        {
            *video_out_h = OSD_HEIGHT;
        }
    }
    else
    {
        *video_out_y = requested_y;
        if ( requested_y + requested_h < OSD_HEIGHT )
        {
            *video_out_h = requested_h;
        }
        else
        {
            *video_out_h = OSD_HEIGHT - requested_y;
        }
    }
}

void OplVideoUtil_getVideoInputWindow( int requested_x, int requested_y, int requested_w, int requested_h, \
                                       int video_x, int video_y, int video_w, int video_h, \
                                       int *video_in_x, int *video_in_y, int *video_in_w, int *video_in_h)
{
    if ( requested_w == 0 || requested_h == 0 || video_w == 0 || video_h == 0 )
    {
        *video_in_x = video_x;
        *video_in_y = video_y;
        *video_in_w = video_w;
        *video_in_h = video_h;
        return;
    }
    
    int presenting_size = 0;  
    int presenting_start_point = 0;
    int presenting_full_size = 0;
    int video_full_size = 0;
    
    /* 
        how to get video_in_x or video_in_y
            presenting_full_size : presenting_start_point == video_full_size : video_in_x/y
            is equal with next .
            video_in_x/y = presenting_start_point * video_full_size / presenting_full_size
            
        how to get video_in_w or video_in_h
            presenting_full_size : presenting_size == video_full_size : video_in_w/h
            is equal with next .
            video_in_w/h = presenting_size * video_full_size / presenting_full_size;
            
        rqs  : requested_x/y == presenting_start_point;
        rqe  : requested_x/y + requested_w/h == presenting_full_size
        OSD_S: OSD start pixel position
        0SD_E: OSD end pixel position
        psp  : presenting_start_point
        pss  : presenting_size
    */
    
    if ( requested_x < 0 )
    {
        //  |---------------|=======================|----------------------------|
        //  rqs             OSD_S                   rqe                          0SD_E 
        //                  psp                     pss
        if ( requested_x + requested_w < OSD_WIDTH )
        {
            presenting_start_point = abs(requested_x);
            presenting_size = requested_w - abs(requested_x);
        }
        //  |---------------|====================================================|----------------------------|
        //  rqs             OSD_S                                                0SD_E                        rqe
        //                  psp                                                  pss
        else
        {
            presenting_start_point = abs(requested_x);
            presenting_size = OSD_WIDTH;
        }
    }
    else
    {
        //                  |---------------|=======================|------------|
        //                  OSD_S           rqs                     rqe          0SD_E 
        //                                  psp                     pss
        if ( requested_x + requested_w < OSD_WIDTH )
        {
            presenting_start_point = 0;
            presenting_size = requested_w;
        }
        //                  |---------------|====================================|---------------------|
        //                  OSD_S           rqs                                  0SD_E                 rqe
        //                                  psp                                  pss
        else
        {
            presenting_start_point = 0;
            presenting_size = OSD_WIDTH - abs(requested_x);
        }
    }
    video_full_size = video_w;
    presenting_full_size = requested_w;
    *video_in_x = presenting_start_point * video_full_size / presenting_full_size;
    *video_in_w = presenting_size * video_full_size / presenting_full_size;
    
    /* for y / h , just change variable's name from x/h to y/h */
    if ( requested_y < 0 )
    {
        if ( requested_y + requested_h < OSD_HEIGHT )
        {
            presenting_start_point = abs(requested_y);
            presenting_size = requested_h - abs(requested_y);
        }
        else
        {
            presenting_start_point = abs(requested_y);
            presenting_size = OSD_HEIGHT;
        }
    }
    else
    {
        if ( requested_y + requested_h < OSD_HEIGHT )
        {
            presenting_start_point = 0;
            presenting_size = requested_h;
        }
        else
        {
            presenting_start_point = 0;
            presenting_size = OSD_HEIGHT - abs(requested_y);
        }
    }
    video_full_size = video_h;
    presenting_full_size = requested_h;
    *video_in_y = presenting_start_point * video_full_size / presenting_full_size;
    *video_in_h = presenting_size * video_full_size / presenting_full_size;
    
    return;
}
