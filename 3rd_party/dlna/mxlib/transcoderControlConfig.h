/*******************************************************************************
 * HUMAX R&D Lab., HUMAX INC., SEOUL, KOREA
 * COPYRIGHT(c) 2011 by HUMAX Inc.
 * 
 * Author : Kang, Yonghee.
 *
 * All rights reserved. No part of this work may be reproduced, stored in a
 * retrieval system, or transmitted by any means without prior written
 * permission of HUMAX Inc.
 ******************************************************************************/

#ifndef TRANSCODER_CONFIG_H
#define TRANSCODER_CONFIG_H


#include "mxlib_config.h"

#ifdef __cplusplus
extern "C" {
#endif

//#define KT_HES

#if defined(KT_HES)

#	define TRANSCODER_AS_DLL
#else
	//..\mxlib\mxkernel;..\mxlib\mxmedia;..\mxlib\mxfile;..\mxlib
#endif

#if defined(WIN32)

#else
	#define USE_MBCS_FILENAME
#endif 
//#define USE_MBCS_FILENAME
//#	define USE_UTF8_FILENAME
//#define USE_UNICODE_FILENAME
//



#if defined(_DEBUG)
#define SAVE_FFPROBE_TOO_SMALL_FILE	
#endif

#define USE_VARIANT_BITRATE
#define USE_FAAC

//#define USE_AIRVIDEO_FFMPEG
//#define USE_MENCODER
#if !defined(USE_MENCODER)
#define USE_FFMPEG  
#endif

#define	USE_MEMORY_SEGMENT
#if !defined(USE_MEMORY_SEGMENT)
#define	USE_FILE_SEGMENT // now broken. 20110901
#endif 

#if defined(TRANSCODER_FFMPEG)
#define	TEST_TUNER
#endif





#ifdef __cplusplus
}
#endif

#endif