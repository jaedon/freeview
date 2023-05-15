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
#ifndef TRANSCODER_UTIL_H
#define TRANSCODER_UTIL_H

#if defined(__cplusplus)
extern "C"{
#endif

/*
#if !defined(BOOL)
#define 	BOOL	int
#define		TRUE	1
#define		FALSE	0
#endif
*/

#include <string.h>

#include "mxkernel.h"
#include "mxtypes.h"
#include "Transcoder_data_struct.h"

#ifndef WIN32

#include <sys/time.h>
#endif

#if !defined(WIN32)
#define isdigit(X)	( ( (X)>= '0' ) && ( (X) <= '9' ) )
#endif

MX_INT64 FileSize64( const char * szFileName );
#if defined(WIN32)
BOOL ReadLine(HANDLE hPipe, char *pBuffer, int BufSize, DWORD *dwRead);
#else
BOOL ReadLine(int hPipe, char *pBuffer, int BufSize, DWORD *dwRead);
#endif

char* ReadOutWord(char* dest, char* src);
char* ReadOutInteger(int* dest, char* src);
char* ReadOutUntilSeparator(char* dest, char* src, char separator);
char* ReadBackUntilSeparator(char* dest, char* src, char* org, char separator);

#if defined(WIN32)
char* transcoder_DecodeFromUTF8(const char* utf8Str, int codePage, int utfLen, int *dstLen);
char* transcoder_EncodeToUTF8(const void* uniStr, int codePage, int srcLen, int* dstLen);

char* getEscapedPathFromUTF8(char* src_utf8);
void getlogtimeNameW(wchar_t *buf, int nBuflen);
#endif

inline MX_UINT64 tcGetTickCount()
{
#ifdef WIN32
	return GetTickCount();
#else
	timeval tv;    
	MX_UINT64 CurrentTick = 0L;							// dypark : 2013-02-22 variable overflow
	gettimeofday(&tv, NULL);   

	CurrentTick = tv.tv_sec;
	CurrentTick = CurrentTick * 1000;
	CurrentTick = CurrentTick + (tv.tv_usec/1000);

	return CurrentTick;
#endif
}


//#if !defined(_strnicmp)
//int _strnicmp(const char *s1, const char *s2, size_t n);
//#endif

void utf8_remove(char* utf8_fullpathname);

typedef struct _transcoder_critical_section_tag
{
	MXKERNEL_CRITICALSECTION	CS;
	int							cRef;
	BOOL						terminating;
// --------------------------------------------------------//
} TRANSCODER_CRITICALSECTION, *PTRANSCODER_CRITICALSECTION;
// --------------------------------------------------------//

void transcoder_initializeCriticalSection(PTRANSCODER_CRITICALSECTION ptranscoder_criticalsection);
void transcoder_deleteCriticalSection(PTRANSCODER_CRITICALSECTION ptranscoder_criticalsection);
BOOL transcoder_enterCriticalSection(PTRANSCODER_CRITICALSECTION ptranscoder_criticalsection);
BOOL transcoder_leaveCriticalSection(PTRANSCODER_CRITICALSECTION ptranscoder_criticalsection);


int ExtractAlbumArt(char* srcImgName, char* resizedImgName, int width, int height, BOOL keep_aspect_ratio);
#ifdef __cplusplus
}
#endif


#endif//#ifndef TRANSCODER_UTIL_H

