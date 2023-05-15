/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   di_iptuner_sstr.c  $
 * Version:			1.5 (for OCTO 1.0 with 40nm)
 * Original Author:		YoungKi Kim $
 * Current Author:	$Author: ykkim3@humaxdigital.com $
 * Date:			$Date: 2012.02.28
 * File Description:	SSTR VFIO Controller
 * Module:
 * Remarks:
 */

/**
 * @defgroup SSTR_VFIO_CONTROLLER
 * @brief SSTR VFIO Controller 이며 SSTR 과 Nexus Playback의 연동에 필요한 기능을 총괄한다.
 */

/**
 * @author YoungKi Kim (ykkim3@humaxdigital.com)
 * @date 28 Feb 2012
 */

/**
 * @brief SSTR VFIO Controller 이며 SSTR 과 Nexus Playback의 연동에 필요한 기능을 총괄한다.
 */

/**
 * @note
 * Copyright (C) 2012 Humax Corporation. All Rights Reserved. <br>
 * This software is the confidential and proprietary information
 * of Humax Corporation. You may not use or distribute this software
 * except in compliance with the terms and conditions of any applicable license
 * agreement in writing between Humax Corporation and you.
 */

 /*@{*/

/**
 * @file di_iptuner_sstr.c
 */

/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
/* Start Including Header Files */

#include "di_iptuner_config.h"

#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif
#include <inttypes.h>	 /** for 64bit MACRO ex>PRId64 */
#include <sys/time.h> 	 /** for checking delay */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "htype.h"

#include "vkernel.h"
#include "taskdef.h"
#include "di_uart.h"

#include "di_iptuner.h"
#include "di_iptuner_http.h"
#include "di_iptuner_sstr.h"

#include "sstr_controller.h"
#include "stream_http.h"

#include "di_iptuner_stream.h"

#include "vfio.h"

/* End Including Headers */
/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
/* Start #define */
//#define DI_IPTUNER_SSTR_DEBUG
//#define SSTR_CONFIG_DUMP_TEXTSTREAM
//#define SSTR_CONFIG_WRITE_FRAGMENT_FOR_TEST
//#define SSTR_CONFIG_WRITE_ONE_FILE_FOR_TEST
//#define SSTR_CONFIG_WRITE_BOX_FOR_TEST

#define SSTR_CONFIG_CHECK_UNDERFLOW		1

/* DEBUG DEFINE */
#ifdef IPTUNER_ALL_DEBUG
#ifdef DI_IPTUNER_SSTR_DEBUG
#define PrintData		DI_UART_Print
#define PrintDebug		DI_UART_Print
#define PrintEnter		DI_UART_Print("(+)%s\n", __FUNCTION__)
#define PrintExit		DI_UART_Print("(-)%s\n", __FUNCTION__)
#define PrintError		DI_UART_Print
#else
#define PrintData		while (0) ((int (*)(char *, ...)) 0)
#define PrintDebug		while (0) ((int (*)(char *, ...)) 0)
#define PrintEnter		while (0) ((int (*)(char *, ...)) 0)
#define PrintExit		while (0) ((int (*)(char *, ...)) 0)
#define PrintError		DI_UART_Print
#endif

#else

#define PrintData		while (0) ((int (*)(char *, ...)) 0)
#define PrintDebug		while (0) ((int (*)(char *, ...)) 0)
#define PrintError		while (0) ((int (*)(char *, ...)) 0)
#define PrintEnter
#define PrintExit
#endif /* END DEBUG DEFINE */

#define UNUSED_PARAM(x)  ((void)x)

/* End #define */
/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
/* Start typedef */

#define SSTR_MP4BOX_BMP4 "bmp4"

/**
 * Broadcom fragmented mp4 지원을 위한 bmp4 box 관리 Structure
 */
typedef struct mem_file {
    unsigned char *buf; /**< bmp4 BOX Buffer */
    size_t buf_len; /**< bmp4 BOX Buffer Size */
    size_t length; /**< bmp4 BOX Length */
    size_t offset; /**< bmp4 BOX Buffer 내 현재 Offset */
    HBOOL bBoxing; /**< bmp4 BOX Complete Flag */
} mem_file;

typedef struct SSTR_REQ_TEXT_s
{
	HUINT64 ullStartTime;
} SSTR_REQ_TEXT_t;

typedef struct SSTR_STREAM_s
{
	HBOOL bEnabled;
	HINT32 nIndex;
} SSTR_STREAM_t;

#define DOWNLOAD_CHECK_PERIOD		3

/*
* bandwidth 등 속도
*/
typedef struct tDownloadInfo
{
	bool IsNotFirst;

	HFLOAT64 fMaxBandwidth;
	HFLOAT64 fBandwidth;			/* Byte per second, average time with downloading packet */
	HULONG ulDownloadTime;		/* ms , average download time */
	HULONG ulStartingTime;			/* ms */
	HULONG ulLoadingTime;			/* downloading manifest time and parsing manifest itme */

	HINT32 nDownloadCount;		/* To check lastest three packet */

	HULONG ulStartTime;			/* temporary time value to store start time */
	HULONG ulEndTime;			/* temporary time value to store end time */

	HULONG ulTimeArray[DOWNLOAD_CHECK_PERIOD];		/* temporary time array to calculate average time with latest three packet */
	HFLOAT64 fBandwidthArray[DOWNLOAD_CHECK_PERIOD];	/* temporary bandwidth array to calculate average bandwidth with latest three packet */

	HUINT32 uCurFragmentIndex;
	HUINT64 ullCurFragmentStartTime;

	HUINT32 uBufferingCount;
	HULONG ulBufferingStartTime;
	HULONG ulBufferingEndTime;
	HUINT64 ullTotalBufferingTime;
}tDownloadInfo;
/**
 * SSTR 운용을 위한 최상위 SSTR Handle Structure
 */
typedef struct SSTR_s
{

#ifdef SSTR_CONFIG_WRITE_FRAGMENT_FOR_TEST
	/* for Test */
	HINT32 fd;
#endif
	SSTR_CTRL hCtrl; /**< SSTR CONTROLLER Handle */
	HINT8 *pManifestUrl;
#if 1
	DI_IPTUNER_SSTR_Info_t* pManifestInfo;
#endif
	mem_file mp4Box; /**< bmp4 box */

	HUINT8 *pFragBuff; /**< Fragment Buffer */
	HUINT32 ulBuffSize; /**< Fragment Buffer Size */
	size_t sz_FragSize; /**< Fragment Size */

	SSTR_URL_t tAVUrl;
	SSTR_URL_t tTextUrl;
	SSTR_URL_t tUpdateUrl;

	HUINT64 ullTimeForSeek; /**< 요청된 Seek Time offset */
	HBOOL	bRequestedSeek; /**< Seek 요청 Flag */
	HUINT32 ulSeekSema;	/**< Seek Semaphore */
	HUINT32 ulLiveSema;	/**< Live Streaming 지원을 위한 Semaphore */
	HBOOL bImmediatelyExit; /**< VFIO 의 Exit를 위한 Flag */
	off_t ullVirtureOffset; /**< Fake Offset */

	SSTR_CONTENTS_TYPE_e eAVType;

//	HUINT32 ulMaxVideo; /**< Max Video Streams */
//	SSTR_STREAM_t	tVideoStreams[DI_IPTUNER_SSTR_CONST_MAX_STREAMS];

	HUINT32 ulMaxText; /**< Max Text Streams */
	SSTR_STREAM_t	tTextStreams[DI_IPTUNER_SSTR_CONST_MAX_STREAMS];

	HBOOL bTextStreamTaskInit; /**< TextStream Task Init */
	unsigned long ulTextStreamTaskId; /**< TextStream Update Task ID */
	DI_IPTUNER_SSTR_TEXT_TASK_STATE_e eTextTaskState;
	UTIL_DLL_t tReqTextStream;
	HBOOL bReqTextFlush;

	/* for Control */
//	HUINT32 ulCurVideo; /**< Current Video Stream  */
	HUINT32 ulCurAudio;	/**< Current Audio Stream  */
	HUINT32 ulCurText; /**< Current Text Stream  */

	/* Live Update */
	HBOOL bLiveUpdateTaskInit; /**< Live Update Task Init */
	unsigned long ulLiveUpdateTaskId; /**< Live Update Task ID */
	DI_IPTUNER_SSTR_LIVE_UPDATE_e eLiveUpdateStatus; /**< Live Update Status */
	HUINT64 ullLiveStartTime; /**< Live Start Time */
	HUINT64 ullLiveStartSystemTime; /**< Live Start Clock */
	HBOOL	bPaused; /**< Paused Flag */

	STREAM_ACCESS_t *pAccess;

	tDownloadInfo stDInfo;	/* download bandwidth information */
} SSTR_t;
/* End typedef */

/*******************************************************************/
/************************ global variables *************************/
/*******************************************************************/
/* Start global variable */
/* End global variable */

/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/
/* Start static variable */
static SSTR_HANDLE g_hSstr = NULL;
static DI_IPTUNER_SSTR_BUFFERING_TASK_STATE_e g_ePreBufferingTaskState = DI_IPTUNER_SSTR_BUFFERING_TASK_STATE_DONE;
static DI_IPTUNER_SSTR_BUFFERING_TASK_STATE_e g_ePreBufferingTaskChangeState = DI_IPTUNER_SSTR_BUFFERING_TASK_STATE_DONE;

#ifdef __SSTR_NOT_SUPPORTED_YET__ /* not supported yet */
static DI_IPTUNER_AdaptiveCallback_t stAdaptiveCallback;
#endif

/* ISO/IEC 13818-7:2005(E) */
/* Table 35 . Sampling frequency dependent on sampling_frequency_index */
static const unsigned b_aac_adts_sample_rate[]={
	96000,
	88200,
	64000,
	48000,
	44100,
	32000,
	24000,
	22050,
	16000,
	12000,
	11025,
	8000
};

/* Table 42 . Channel Configuration */
static const uint8_t b_aac_adts_channels[]={
	0,
	1,
	2,
	3,
	4,
	5,
	5+1,
	7+1
};

/* End static variable */
/*******************************************************************/
/************************ static funtions **************************/
/*******************************************************************/
void P_STREAM_SSTR_TimerStart(tDownloadInfo* pInfo);
void P_STREAM_SSTR_TimerStop(tDownloadInfo* pInfo);
void P_STREAM_SSTR_TimerSetStartingTime(tDownloadInfo* pInfo);
void P_STREAM_SSTR_TimerSetLoadingTime(tDownloadInfo* pInfo);
void P_STREAM_SSTR_TimerSetDownloadTime(tDownloadInfo* pInfo, HINT32 nSize);

#if SSTR_CONFIG_CHECK_UNDERFLOW
HBOOL P_STREAM_SSTR_IsBuffering(HUINT32 uPrvBufferDepth, HUINT32* pCurBufferDepth);
void P_STREAM_SSTR_BufferingTimerStart(tDownloadInfo* pInfo);
void P_STREAM_SSTR_BufferingTimerStop(tDownloadInfo* pInfo);
void P_STREAM_SSTR_SetBufferingTime(tDownloadInfo* pInfo);
#endif
#if __________p__________
#endif

/**
 * @b Function @b Description <br>
 * bmp4 box buffer에 대한 Virture File I/O Function ( Write ) <br>
 * bmp4 box buffer에 Write 한다. <br>
 *
 * @param[in] mem_file	: bmp4 box의 mem_file I/O handle
 * @param[in] ptr	: bmp4 box buffer에 복사할 sourc buffer
 * @param[in] size : 복사할 양
 *
 * @return  #  void <br>
 */
static void mem_file_write(mem_file *file,const void *ptr, size_t size)
{
	// BDBG_ASSERT(file->offset + size<= file->buf_len);
	VK_memcpy((uint8_t *)file->buf + file->offset, ptr, size);
	file->offset += size;
	if(file->length<file->offset) {
		file->length = file->offset;
	}
	return;
}

/**
 * @b Function @b Description <br>
 * bmp4 box buffer에 대한 Virture File I/O Function ( Seek ) <br>
 * bmp4 box buffer내의 현재 offset을 반환한다.  <br>
 *
 * @param[in] mem_file	: bmp4 box의 mem_file I/O handle
 *
 * @todo Offset Range 체크 하도록 수정 해야함
 *
 * @return  #  이동한 offset (size_t) <br>
 */
static size_t mem_file_tell(mem_file *file)
{
	return file->offset;
}

/**
 * @b Function @b Description <br>
 * bmp4 box buffer에 대한 Virture File I/O Function ( Seek ) <br>
 * bmp4 box buffer내의 offset을 이동한다.  <br>
 *
 * @param[in] mem_file	: bmp4 box의 mem_file I/O handle
 * @param[in] offset	: 이동할 offset

 * @return  #  현재 offset (size_t) <br>
 */
static void mem_file_seek(mem_file *file, size_t offset)
{
	file->offset = offset;
	return;
}

/**
 * @b Function @b Description <br>
 * bmp4 box buffer에 대한 Virture File I/O Sub Function ( uint8 ) <br>
 * bmp4 box buffer에 uint8 값을 Write 한다.  <br>
 *
 * @param[in] mem_file	: bmp4 box의 mem_file I/O handle
 * @param[in] d	: Write 할 uint8 value

 * @return  #  void <br>
 */
static void
write_uint8(mem_file *fout, uint8_t d)
{
	mem_file_write(fout, &d, sizeof(d));
}

#if 0
/**
 * @b Function @b Description <br>
 * bmp4 box buffer에 대한 Virture File I/O Sub Function ( uint16_big_endian ) <br>
 * bmp4 box buffer에 uint16(big-endian) 값을 Write 한다.  <br>
 *
 * @param[in] mem_file	: bmp4 box의 mem_file I/O handle
 * @param[in] data	: Write 할 uint16 value

 * @return  #  void <br>
 */

static void
write_uint16_be(FILE *fout, uint16_t data)
{
	uint8_t d[2];
	d[0] = data >> 8;
	d[1] = data;
	fwrite(d, sizeof(d), 1, fout);
}
#endif

/**
 * @b Function @b Description <br>
 * bmp4 box buffer에 대한 Virture File I/O Sub Function ( uint24_big_endian ) <br>
 * bmp4 box buffer에 uint24(big-endian) 값을 Write 한다.  <br>
 *
 * @param[in] mem_file	: bmp4 box의 mem_file I/O handle
 * @param[in] data	: Write 할 uint24 value

 * @return  #  void <br>
 */
static void
write_uint24_be(mem_file *fout, uint32_t data)
{
	uint8_t d[3];
	d[0] = data >> 16;
	d[1] = data >> 8;
	d[2] = data;
	mem_file_write(fout, d, sizeof(d));
}

/**
 * @b Function @b Description <br>
 * bmp4 box buffer에 대한 Virture File I/O Sub Function ( uint32_big_endian ) <br>
 * bmp4 box buffer에 uint32(big-endian) 값을 Write 한다.  <br>
 *
 * @param[in] mem_file	: bmp4 box의 mem_file I/O handle
 * @param[in] data	: Write 할 uint32 value
 * @return  #  void <br>
 */
static void
write_uint32_be(mem_file *fout, uint32_t data)
{
	uint8_t d[4];
	d[0] = data >> 24;
	d[1] = data >> 16;
	d[2] = data >> 8;
	d[3] = data;
	mem_file_write(fout, d, sizeof(d));
}

/**
 * @b Function @b Description <br>
 * bmp4 box buffer에 대한 Virture File I/O Sub Function ( uint64_big_endian ) <br>
 * bmp4 box buffer에 uint64(big-endian) 값을 Write 한다.  <br>
 *
 * @param[in] mem_file	: bmp4 box의 mem_file I/O handle
 * @param[in] data	: Write 할 uint64 value
 * @return  #  void <br>
 */
static void
write_uint64_be(mem_file *fout, uint64_t data)
{
	uint8_t d[8];
	d[0] = data >> 56;
	d[1] = data >> 48;
	d[2] = data >> 40;
	d[3] = data >> 32;
	d[4] = data >> 24;
	d[5] = data >> 16;
	d[6] = data >> 8;
	d[7] = data;
	mem_file_write(fout, d, sizeof(d));
}

/**
 * @b Function @b Description <br>
 * bmp4 box buffer에 대한 Virture File I/O Sub Function ( *uint8 ) <br>
 * bmp4 box buffer에 uint8 pointer의 값을 Write 한다.  <br>
 *
 * @param[in] mem_file	: bmp4 box의 mem_file I/O handle
 * @param[in] data	: Write 할 uint8 pointer value
 * @param[in] len	: Data Buffer의 크기
 *
 * @return  #  void <br>
 */
static void
write_data(mem_file *fout, const uint8_t *data, size_t len)
{
	mem_file_write(fout, data, len);
}

/**
 * @b Function @b Description <br>
 * bmp4 box buffer에 대한 Virture File I/O Sub Function ( FourCC ) <br>
 * bmp4 box buffer에 FourCC 의 값을 Write 한다.  <br>
 *
 * @param[in] mem_file	: bmp4 box의 mem_file I/O handle
 * @param[in] fourcc	: Write 할 FourCC value
 *
 * @return  #  void <br>
 */
static void
write_fourcc(mem_file *fout, const char *fourcc)
{
	write_data(fout, (const uint8_t *)fourcc, 4);
}

/**
 * @b Function @b Description <br>
 * bmp4 box buffer에 대한 Virture File I/O Sub Function ( box type ) <br>
 * MP4 Box 시작시 사용하며 Box Size를 초기화하고 Box Type 의 값을 Write 한다.  <br>
 * MP4 FullBox의 구조는 | Box Size (32bits) | BoxType (32bits) | Version (8bits) | Flags (24bits) | Value | 로 구성되고 <br>
 * bmp4 Box의 구조는 | Box Size (32bits) | BoxType "bmp4" (32bits) | MP4 Full Boxes ... | 로 구성된다. <br>
 *
 * @param[in] mem_file	: bmp4 box의 mem_file I/O handle
 * @param[in] type	: Write 할 type value
 *
 * @return  #  Write 후 이동한 offset <br>
 */
static int
start_mp4_box(mem_file *fout, const char *type)
{
	int offset = mem_file_tell(fout);
	write_uint32_be(fout, 0); /* size */
	write_fourcc(fout, type); /* type */
	return offset;
}

/**
 * @b Function @b Description <br>
 * bmp4 box buffer에 대한 Virture File I/O Sub Function ( box type ) <br>
 * MP4 box 종료시 사용하며 Box Size를 Write 한다.  <br>
 * Box Size는 Value 의 크기만 나타낸다.
 * 인자에 사용되는 offset은 Box의 Value 의 Start Offset이다.  <br>
 *
 * @param[in] mem_file	: bmp4 box의 mem_file I/O handle
 * @param[in] offset	: Box의 Value 의 Start Offset
 *
 * @return  # void <br>
 */
static void
finish_mp4_box(mem_file *fout, int offset)
{
	int current = mem_file_tell(fout);
	mem_file_seek(fout, offset);
	write_uint32_be(fout, current-offset); /* size */
	mem_file_seek(fout, current);
	return;
}

/**
 * @b Function @b Description <br>
 * bmp4 box buffer에 대한 Virture File I/O Sub Function ( box type ) <br>
 * bmp4 Box 종료시 사용하며 bmp4 box buffer의 0 offset에 Box Size를 Write 한다.  <br>
 * 인자에 사용되는 Size는 bmp4 box의 MP4 Fullboxes 의 전체 크기이다.  <br>
 *
 * @param[in] mem_file	: bmp4 box의 mem_file I/O handle
 * @param[in] size	: MP4 Fullboxes 의 전체 크기
 *
 * @return  # void <br>
 */
static void
close_mp4_box(mem_file *fout, int size)
{
	int current = mem_file_tell(fout);
	mem_file_seek(fout, 0);
	write_uint32_be(fout, size); /* size */
	mem_file_seek(fout, current);
	return;
}

/**
 * @b Function @b Description <br>
 * bmp4 box buffer에 대한 Virture File I/O Sub Function ( box type ) <br>
 * MP4 Full Box 시작시 사용하며 Box Size를 초기화하고 Box Type, Version, Flags 의 값을 Write 한다.  <br>
 * MP4 FullBox의 구조는 | Box Size (32bits) | BoxType (32bits) | Version (8bits) | Flags (24bits) | Value | 로 구성된다. <br>
 *
 * @param[in] mem_file	: bmp4 box의 mem_file I/O handle
 * @param[in] type	: Write 할 type value
 * @param[in] version	: Write 할 version value
 * @param[in] flags	: Write 할 flags value
 *
 * @return  #  Write 후 이동한 offset <br>
 */
static int
start_mp4_fullbox(mem_file *fout, const char *type, unsigned version, uint32_t flags)
{
	int offset = start_mp4_box(fout, type);
	write_uint8(fout, version);
	write_uint24_be(fout, flags);
	return offset;
}

/**
 * @b Function @b Description <br>
 * bmp4 box buffer에 대한 Virture File I/O Sub Function ( TREX BOX ) <br>
 * trex Box(full box) 를 생성한다. <br>
 * Track ID는 Setup 시 사용되는 Stream ID와 같아야한다.
 *
 * @param[in] mem_file	: bmp4 box의 mem_file I/O handle
 * @param[in] track_ID : Write 할 Track ID (Stream ID)
 * @param[in] default_sample_description_index : Write 할 default_sample_description_index value
 * @param[in] default_sample_duration : Write 할 default_sample_duration value
 * @param[in] default_sample_size : Write 할 default_sample_size value
 * @param[in] default_sample_flags : Write 할 default_sample_flags value
 *
 * @return  #  void <br>
 */
static void
write_trex_box(mem_file *fout,	uint32_t track_ID, uint32_t default_sample_description_index, uint32_t default_sample_duration, uint32_t default_sample_size, uint32_t default_sample_flags)
{
	int offset = start_mp4_fullbox(fout, "trex", 0, 0);
	write_uint32_be(fout, track_ID);
	write_uint32_be(fout, default_sample_description_index);
	write_uint32_be(fout, default_sample_duration);
	write_uint32_be(fout, default_sample_size);
	write_uint32_be(fout, default_sample_flags);
	finish_mp4_box(fout, offset);
	return;
}

/**
 * @b Function @b Description <br>
 * bmp4 box buffer에 대한 Virture File I/O Sub Function ( BDAT BOX ) <br>
 * bdat Box(full box) 를 생성한다. <br>
 * 인자로 사용되는 data는 CodecPrivateData를 나타낸다.
 *
 * @param[in] mem_file	: bmp4 box의 mem_file I/O handle
 * @param[in] data : Write 할 CodecPrivateData
 * @param[in] data_len : CodecPrivateData 의 크기
 *
 * @return  #  void <br>
 */
static void
write_bdat_box(mem_file *fout,	const uint8_t *data, size_t data_len)
{
	int offset = start_mp4_box(fout, "bdat");
	write_data(fout, data, data_len);
	finish_mp4_box(fout, offset);
	return;
}

/**
 * @b Function @b Description <br>
 * bmp4 box buffer에 대한 Virture File I/O Sub Function ( BHED BOX ) <br>
 * bhed Box(full box) 를 생성한다. <br>
 *
 * @param[in] mem_file	: bmp4 box의 mem_file I/O handle
 * @param[in] timescale : timescale
 * @param[in] start_time : 해당 fragment의 start_time
 * @param[in] fourcc : 해당 fragment의 fourcc
 *
 * @return  #  void <br>
 */
static void
write_bhed_box(mem_file *fout, uint64_t timescale, uint64_t start_time, const char *fourcc)
{
	int offset = start_mp4_box(fout, "bhed");
	write_uint64_be(fout, timescale);
	write_uint64_be(fout, start_time);
	write_fourcc(fout, fourcc);
	finish_mp4_box(fout, offset);
	return;
}

static int
h2b(char code)
{
	if(code>='0' && code<='9') {
		return code - '0';
	} else if (code>='A' && code<='F') {
		return code - 'A' + 0x0A;
	} else if (code>='a' && code<='f') {
		return code - 'a' + 0x0a;
	}
	return -1;
}

/**
 * @b Function @b Description <br>
 * HEX to BIN <br>
 *
 * @param[in] data : HEX Data
 * @param[in] buf : Binary로 저장될 Buffer
 * @param[in] max_len : Buffer의 크기
 *
 * @return  #  저장된 Binary 크기 <br>
 */
static int
hex2bin(const char *data, uint8_t *buf, size_t max_len)
{
	unsigned i;
	for(i=0;;i++) {

		char ch1 = data[2*i];
		char ch2 = data[2*i+1];
		int b1, b2;
		if(i>=max_len) {
			return -1;
		}
		if(ch1==0 || ch2==0) {
			break;
		}
		b1 = h2b(ch1);
		b2 = h2b(ch2);
		if(b1<0 || b2<0) {
			return -1;
		}
		buf[i]= (b1<<4) | b2;

	}
	return i;
}

/**
 * @b Function @b Description <br>
 * Unsigned Integer to Array <br>
 *
 * @param[in] dst : Array로 저장될 Buffer
 * @param[in] src : source Unsigned Integer
 *
 * @return  #  void <br>
 */
static void
uint2array(char *dst, unsigned int src)
{
	int mask = 0xff;
	dst[0] = mask & (src >> 24);
	dst[1] = mask & (src >> 16);
	dst[2] = mask & (src >> 8);
	dst[3] = mask & src;
}


static inline SSTR_t* __getSSTR(SSTR_HANDLE hSSTR) { return (SSTR_t*)hSSTR; }

/**
 * @b Function @b Description <br>
 * Seek이 요청 여부 확인 <br>
 *
 * @param[in] pSstr : SSTR HANDLE
 *
 * @return  #  Boolean <br>
 */
HBOOL P_STREAM_SSTR_IsSeekRequested(SSTR_t* pSstr)
{
	if(pSstr == NULL)
	{
		PrintError("%s(%d) Err> SSTR HANDLE is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return 0;
	}
	return pSstr->bRequestedSeek;
}

/**
 * @b Function @b Description <br>
 * Seek 가능한 Time Offset인지 확인 <br>
 *
 * @param[in] pSstr : SSTR HANDLE
 * @param[in] ullTime : Time Offset
 *
 * @return  #  Boolean <br>
 */
HBOOL P_STREAM_SSTR_IsValidTimeForSeek(SSTR_t* pSstr, HUINT64 ullTime)
{
	HUINT32 unFragment = 0;
	if(pSstr == NULL)
	{
		PrintError("%s(%d) Err> SSTR HANDLE is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return FALSE;
	}
#if 1
	if(SSTR_CONTROLLER_GetFragmentByTime(pSstr->hCtrl, SSTR_CONTROLLER_GetCurStreamType(pSstr->hCtrl), ullTime, &unFragment) != SSTR_ERR_OK)
	{
		return FALSE;
	}
#else
	if( SSTR_CONTROLLER_GetMediaDuration(pSstr->hCtrl) < ullTime )
	{
		return FALSE;
	}
#endif
	return TRUE;
}

HUINT32 P_STREAM_SSTR_bytetouint24_be(const HUINT8 *data, size_t sz_data)
{
	uint32_t res = 0;
	if(data == NULL || sz_data <= 0)
	{
		return 0;
	}
	if (sz_data>=4) {
		res =
			(((HUINT32)data[0])<<16) |
			(((HUINT32)data[1])<<8) |
			((HUINT32)data[2]);
	}
	return res;
}

HUINT32 P_STREAM_SSTR_bytetouint32_be(const HUINT8 *data, size_t sz_data)
{
	HUINT32 res = 0;
	if(data == NULL || sz_data <= 0)
	{
		return 0;
	}
	if (sz_data>=4) {
		res =
			(((HUINT32)data[0])<<24) |
			(((HUINT32)data[1])<<16) |
			(((HUINT32)data[2])<<8) |
			((HUINT32)data[3]);
	}
	return res;
}

#define MP4_TYPE(c1,c2,c3,c4) (((uint32_t)c4) | ((uint32_t)c3<<8) | ((uint32_t)c2<<16) | ((uint32_t)c1<<24))

typedef struct MP4_BOX {
	HUINT64 size;
	HUINT32 type;
	HUINT8 version;
	HUINT32 flags;
} MP4_BOX_t;


HUINT64 P_STREAM_SSTR_bytetouint64_be(const HUINT8 *data, size_t sz_data)
{
	HUINT64 res = 0;
	if(data == NULL || sz_data <= 0)
	{
		return 0;
	}
	if(sz_data >= 8)
	{
		res = ((uint64_t)P_STREAM_SSTR_bytetouint32_be(data,sz_data)) << 32;
		res |= P_STREAM_SSTR_bytetouint32_be(data+4,sz_data-4);
	}
	return res;
}

size_t P_STREAM_SSTR_ParseBox(const HUINT8 *data, size_t sz_data, MP4_BOX_t *box)
{
	HUINT32 size = 0, pos=0;

	if(data == NULL || sz_data <= 0 || box == NULL)
	{
		return 0;
	}
	size = P_STREAM_SSTR_bytetouint32_be(data,sz_data);
	if(size == 0)
	{
		return 0;
	}
	pos += 4;
	box->type = P_STREAM_SSTR_bytetouint32_be(data+pos,sz_data-pos);
	if(box->type == 0)
	{
		return 0;
	}
	pos += 4;
	if(sz_data - pos > 0) {
		if(size!=1) {
			box->size = size;
			return sizeof(HUINT32)+sizeof(HUINT32);
		}
		box->size = P_STREAM_SSTR_bytetouint64_be(data+pos,sz_data-pos);
		if(box->size == 0)
		{
			return 0;
		}
		pos += 8;
		if(sz_data - pos > 0) {
			return sizeof(HUINT32)+sizeof(HUINT32)+sizeof(HUINT64);
		}
	}
	return 0;
}

HBOOL P_STREAM_SSTR_ParseFullBox(const HUINT8 *data, size_t sz_data, MP4_BOX_t *box)
{
	if(data == NULL || sz_data <= 0 || box == NULL)
	{
		return FALSE;
	}
	if(sz_data >= 4)
	{
		box->version = data[0];
		box->flags = P_STREAM_SSTR_bytetouint24_be(data+1,sz_data-1);
	}
	return TRUE;
}
void P_STREAM_SSTR_LiveRecovery(SSTR_HANDLE hSstr)
{
	HUINT32 ulFragmentForSeek = 0;
	HUINT64 ullCurrentTime = 0;
	HUINT64 ullMediaStartTime = 0;
	SSTR_t* pSstr = NULL;
	if(NULL == hSstr)
	{
		PrintError("%s(%d) Error> SSTR HANDLE is NULL. Check This !!\n"
		, __FUNCTION__, __LINE__);
		return;
	}
	pSstr = __getSSTR(hSstr);
	if(pSstr->hCtrl == NULL)
	{
		PrintError("%s(%d) Error> SSTR CONTROLLER is NULL. Check This !!\n"
		, __FUNCTION__, __LINE__);
		return;
	}
	ullCurrentTime = VK_TIMER_GetSystemTick();
	ullMediaStartTime = SSTR_CONTROLLER_GetMediaStartTime(pSstr->hCtrl);
	ullCurrentTime = ullCurrentTime - pSstr->ullLiveStartSystemTime;
	ullCurrentTime *= 10000;
	ullCurrentTime += pSstr->ullLiveStartTime;
	if(ullMediaStartTime > ullCurrentTime)
	{
		ullCurrentTime = ullMediaStartTime;
	}
#ifdef DI_IPTUNER_SSTR_CONFIG_AUDIO_FIRST
	if( pSstr->eAVType != SSTR_CONTENTS_AUDIO )
	{
		SSTR_CONTROLLER_SetCurStreamType(pSstr->hCtrl,SSTR_TYPE_VIDEO);
	}
#else
	if( pSstr->eAVType != SSTR_CONTENTS_VIDEO )
	{
		SSTR_CONTROLLER_SetCurStreamType(pSstr->hCtrl,SSTR_TYPE_AUDIO);
	}
#endif
	PrintError("%s(%d) Try to Next Fragment (Time : %llu)\n",__FUNCTION__,__LINE__,ullCurrentTime);
	if( pSstr->eAVType != SSTR_CONTENTS_AUDIO )
	{
		if(SSTR_CONTROLLER_GetFragmentByTime(pSstr->hCtrl, SSTR_CONTROLLER_GetCurAVStream(pSstr->hCtrl, SSTR_TYPE_VIDEO),ullCurrentTime,&ulFragmentForSeek) == SSTR_ERR_OK)
		{
			(void)SSTR_CONTROLLER_SetFragment(pSstr->hCtrl,SSTR_CONTROLLER_GetCurAVStream(pSstr->hCtrl, SSTR_TYPE_VIDEO), ulFragmentForSeek);
		}
		else
		{
			ulFragmentForSeek = SSTR_CONTROLLER_GetTotalFragment(pSstr->hCtrl,SSTR_CONTROLLER_GetCurAVStream(pSstr->hCtrl, SSTR_TYPE_VIDEO));
			if(ulFragmentForSeek >= 2)
			{
				ulFragmentForSeek -= 2;
			}
			else
			{
				ulFragmentForSeek--;
			}
			(void)SSTR_CONTROLLER_SetFragment(pSstr->hCtrl,SSTR_CONTROLLER_GetCurAVStream(pSstr->hCtrl, SSTR_TYPE_VIDEO), ulFragmentForSeek);
		}
	}
	if( pSstr->eAVType != SSTR_CONTENTS_VIDEO )
	{
		if(SSTR_CONTROLLER_GetFragmentByTime(pSstr->hCtrl, SSTR_CONTROLLER_GetCurAVStream(pSstr->hCtrl, SSTR_TYPE_AUDIO),ullCurrentTime,&ulFragmentForSeek) == SSTR_ERR_OK)
		{
			(void)SSTR_CONTROLLER_SetFragment(pSstr->hCtrl,SSTR_CONTROLLER_GetCurAVStream(pSstr->hCtrl, SSTR_TYPE_AUDIO), ulFragmentForSeek);
		}
		else
		{
			ulFragmentForSeek = SSTR_CONTROLLER_GetTotalFragment(pSstr->hCtrl,SSTR_CONTROLLER_GetCurAVStream(pSstr->hCtrl, SSTR_TYPE_AUDIO));
			if(ulFragmentForSeek >= 2)
			{
				ulFragmentForSeek -= 2;
			}
			else
			{
				ulFragmentForSeek--;
			}
			(void)SSTR_CONTROLLER_SetFragment(pSstr->hCtrl,SSTR_CONTROLLER_GetCurAVStream(pSstr->hCtrl, SSTR_TYPE_AUDIO), ulFragmentForSeek);
		}
	}
}

void P_STREAM_SSTR_LiveUpdate(SSTR_t* pSstr, HUINT8 *pBuf, size_t unBufSize, HUINT32 unStream)
{
	HUINT32 i = 0, j = 0, compare = 0;
	HUINT32 header_size = 0;
	HBOOL bTraf = FALSE;
	HBOOL bMoof = FALSE;
	HBOOL bUpdate = FALSE;
	HUINT32 unEntries = 0;
	MP4_BOX_t tBox;
	HUINT64 ullDuration = 0;
	HUINT64 ullTimeStamp = 0;
	HUINT8 lookahead_ext[16] = {0xd4, 0x80, 0x7e, 0xf2, 0xca, 0x39, 0x46, 0x95, 0x8e, 0x54, 0x26, 0xcb, 0x9e, 0x46, 0xa7, 0x9f};
	HUINT32 unIndex = 0;
	if(pSstr == NULL || pBuf == NULL || unBufSize <= 0)
	{
		return;
	}
	unIndex = unStream;
	/* TODO : Compare Extended Type */
	for(i = 0 ; i < unBufSize;)
	{
		VK_memset(&tBox,0x00,sizeof(MP4_BOX_t));
		header_size = P_STREAM_SSTR_ParseBox(pBuf + i, unBufSize - i,&tBox);
		if(header_size > 0)
		{
			if(bMoof == FALSE)
			{
				if(MP4_TYPE('m','o','o','f') == tBox.type)
	        	{
					i += header_size;
					bMoof = TRUE;
	        	}
	        	else
				{
					i += tBox.size;
				}
				continue;
			}
			if(bTraf == FALSE)
	        {
				if(MP4_TYPE('t','r','a','f') == tBox.type)
				{
					i += header_size;
					bTraf = TRUE;
				}
				else
				{
					i += tBox.size;
				}
				continue;
			}

			if(MP4_TYPE('u','u','i','d') == tBox.type)
			{
				i += header_size;
				if((i + 16) >= unBufSize)
				{
					break;
				}
				for(j = 0; j < 16 ; j++)
				{
					if(pBuf[i+j] == lookahead_ext[j])
					{
						compare++;
						if(compare == 16)
						{
							bUpdate = TRUE;
							i += 16;
							break;
						}
					}
					else
					{
						i += (tBox.size - header_size);
						break;
					}
				}
			}
			else
			{
				i += tBox.size;
				continue;
			}

			if(bUpdate == TRUE)
			{
				break;
			}
		}
		else
		{
			break;
		}
    }
    if(bUpdate == TRUE)
    {
    	if(P_STREAM_SSTR_ParseFullBox(pBuf + i, unBufSize - i,&tBox) == TRUE)
		{
			i += 4;
			if(unBufSize - i > 0)
			{
				unEntries = pBuf[i];
				i += 1;
			}
			VK_SEM_Get(pSstr->ulLiveSema);
			if( (unBufSize - i) - (unEntries * 16) > 0 )
			{
				for(j = 0 ; j < unEntries ; j++)
				{
					ullTimeStamp = P_STREAM_SSTR_bytetouint64_be(pBuf + i, unBufSize - i);
					i+=8;
					ullDuration = P_STREAM_SSTR_bytetouint64_be(pBuf + i, unBufSize - i);
					i+=8;
					if(SSTR_CONTROLLER_AddFragment(pSstr->hCtrl,unIndex,ullTimeStamp,ullDuration) == SSTR_ERR_OK)
					{
						PrintDebug("%s(%d) (+) Fragment Added\n",__FUNCTION__,__LINE__);
						PrintDebug("%s(%d) [TIMESTAMP : %"PRId64"]\n",__FUNCTION__,__LINE__,ullTimeStamp);
						PrintDebug("%s(%d) [DURATION : %"PRId64"]\n",__FUNCTION__,__LINE__,ullDuration);
						if( SSTR_CONTROLLER_GetTotalFragment(pSstr->hCtrl,unIndex) > 1
						&& ( SSTR_CONTROLLER_GetCurFragment(pSstr->hCtrl, unIndex) != 0 || pSstr->bPaused ))
						{
							if(SSTR_CONTROLLER_DiscardFragments(pSstr->hCtrl,unIndex,0) == SSTR_ERR_OK)
							{
								PrintDebug("%s(%d) (-) Fragment Discarded\n",__FUNCTION__,__LINE__);
							}
							PrintDebug("%s(%d) Total Fragments %d\n",__FUNCTION__,__LINE__,SSTR_CONTROLLER_GetTotalFragment(pSstr->hCtrl,unIndex));
						}
					}
					else
					{
						PrintDebug("%s(%d) (*) Fragment Detected\n",__FUNCTION__,__LINE__);
						PrintDebug("%s(%d) [TIMESTAMP : %"PRId64"]\n",__FUNCTION__,__LINE__,ullTimeStamp);
						PrintDebug("%s(%d) [DURATION : %"PRId64"]\n",__FUNCTION__,__LINE__,ullDuration);
					}
				}
			}
			VK_SEM_Release(pSstr->ulLiveSema);
		}
    }
    return;
}

/**
 * @b Function @b Description <br>
 * FourCC to DI Codec <br>
 *
 * @param[in] pFourCC : FourCC Value
 *
 * @return  #  DI CODEC <br>
 */
HINT32 P_STREAM_SSTR_FourCCToDICodec(HINT8* pFourCC)
{
	if(!VK_strncmp(pFourCC,SSTR_CONST_FOURCC_H264,4))
	{
		return DI_MEDIA_VIDEO_FORMAT_H264;
	}
	else if(!VK_strncmp(pFourCC,SSTR_CONST_FOURCC_WVC1,4))
	{
		return DI_MEDIA_VIDEO_FORMAT_VC1;
	}
	else if(!VK_strncmp(pFourCC,SSTR_CONST_FOURCC_AVC1,4))
	{
		return DI_MEDIA_VIDEO_FORMAT_H264;
	}
	else if(!VK_strncmp(pFourCC,SSTR_CONST_FOURCC_AACL,4))
	{
		return DI_MEDIA_AUDIO_FORMAT_AAC;
	}
	else if(!VK_strncmp(pFourCC,SSTR_CONST_FOURCC_AC_3,4))
	{
		return DI_MEDIA_AUDIO_FORMAT_AC3;
	}
	else if(!VK_strncmp(pFourCC,SSTR_CONST_FOURCC_EC_3,4))
	{
		return DI_MEDIA_AUDIO_FORMAT_AC3_PLUS;
	}
	else if(!VK_strncmp(pFourCC,SSTR_CONST_FOURCC_WMAP,4))
	{
		return DI_MEDIA_AUDIO_FORMAT_WMA_STD;
	}
	else
	{
		return 0;
	}
}

#ifdef __SSTR_NOT_SUPPORTED_YET__
static DI_IPTUNER_SSTR_TEXT_e P_STREAM_SSTR_ConvertSubtype(HINT8* pSubType)
{
	if(pSubType == NULL)
	{
		return DI_IPTUNER_SSTR_TEXT_UNKNOWN;
	}
	if(!VK_strncmp(pSubType,SSTR_CONST_FOURCC_SCMD,4))
	{
		return DI_IPTUNER_SSTR_TEXT_SCMD;
	}
	else if(!VK_strncmp(pSubType,SSTR_CONST_FOURCC_CHAP,4))
	{
		return DI_IPTUNER_SSTR_TEXT_CHAP;
	}
	else if(!VK_strncmp(pSubType,SSTR_CONST_FOURCC_SUBT,4))
	{
		return DI_IPTUNER_SSTR_TEXT_SUBT;
	}
	else if(!VK_strncmp(pSubType,SSTR_CONST_FOURCC_CAPT,4))
	{
		return DI_IPTUNER_SSTR_TEXT_CAPT;
	}
	else if(!VK_strncmp(pSubType,SSTR_CONST_FOURCC_DESC,4))
	{
		return DI_IPTUNER_SSTR_TEXT_DESC;
	}
	else if(!VK_strncmp(pSubType,SSTR_CONST_FOURCC_CTRL,4))
	{
		return DI_IPTUNER_SSTR_TEXT_CTRL;
	}
	else if(!VK_strncmp(pSubType,SSTR_CONST_FOURCC_DATA,4))
	{
		return DI_IPTUNER_SSTR_TEXT_DATA;
	}
	else
	{
		return DI_IPTUNER_SSTR_TEXT_UNKNOWN;
	}
}
#endif

static DI_ERR_CODE P_STREAM_SSTR_ReqTextStream(SSTR_HANDLE hSstr, HUINT64 ullStartTime)
{
	SSTR_t* pSstr = NULL;
	SSTR_REQ_TEXT_t* pReq = NULL;
	if(NULL == hSstr)
	{
		PrintError("%s(%d) Error> SSTR HANDLE is NULL. Check This !!\n"
		, __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}
#if defined(DI_IPTUNER_SSTR_CONFIG_TEXTSTREAM)
	pSstr = __getSSTR(hSstr);
	if(pSstr->hCtrl == NULL)
	{
		PrintError("%s(%d) Error> SSTR CONTROLLER is NULL. Check This !!\n"
		, __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}
	if(pSstr->eTextTaskState != DI_IPTUNER_SSTR_TEXT_TASK_STATE_STARTED)
	{
		PrintError("%s(%d) Error> Text Task is busy or done. Check This !!\n"
		, __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}
	pReq = DD_MEM_Alloc(sizeof(SSTR_REQ_TEXT_t));
	if(pReq == NULL)
	{
		PrintError("%s(%d) Error> SSTR_REQ_TEXT_t Allocation Fail. Check This !!\n"
		, __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}
//	PrintDebug("Req %"PRId64"\n\n",ullStartTime);
	VK_MEM_Memset(pReq,0x00,sizeof(SSTR_REQ_TEXT_t));
	pReq->ullStartTime = ullStartTime;
	UTIL_DLLIST_Append(&pSstr->tReqTextStream,pReq);
#else
	UNUSED_PARAM(pReq);
	UNUSED_PARAM(hSstr);
	UNUSED_PARAM(pSstr);
	UNUSED_PARAM(ullStartTime);
#endif
	return DI_ERR_OK;
}

#if defined(DI_IPTUNER_SSTR_CONFIG_TEXTSTREAM)
static SSTR_REQ_TEXT_t* P_STREAM_SSTR_GetReqTextStream(SSTR_HANDLE hSstr)
{
	SSTR_t* pSstr = NULL;
	SSTR_REQ_TEXT_t* pReq = NULL;
	UTIL_DLLNode_t* pNode = NULL;
	if(NULL == hSstr)
	{
		PrintError("%s(%d) Error> SSTR HANDLE is NULL. Check This !!\n"
		, __FUNCTION__, __LINE__);
		return NULL;
	}
	pSstr = __getSSTR(hSstr);
	if(pSstr->hCtrl == NULL)
	{
		PrintError("%s(%d) Error> SSTR CONTROLLER is NULL. Check This !!\n"
		, __FUNCTION__, __LINE__);
		return NULL;
	}
	// VK_SEM_Get(pSstr->ulQueueSema);
	pNode = UTIL_DLLIST_GetHeadNode(&pSstr->tReqTextStream);
	if(pNode != NULL && pNode->pData != NULL)
	{
		pNode = UTIL_DLLIST_ExtractHead(&pSstr->tReqTextStream);
		if(pNode != NULL && pNode->pData != NULL)
		{
			pReq = (SSTR_REQ_TEXT_t*)pNode->pData;
			if(pReq != NULL)
			{
				DD_MEM_Free(pNode);
				// VK_SEM_Release(pSstr->ulQueueSema);
				return pReq;
			}
			else
			{
				if(pNode != NULL)
				{
					DD_MEM_Free(pNode);
				}
			}
		}
	}
	// VK_SEM_Release(pSstr->ulQueueSema);
	return NULL;
}
#endif

static SSTR_ERR P_STREAM_SSTR_DownloadFragment(SSTR_HANDLE hSstr, STREAM_ACCESS_t *pAccess, HUINT32 unStream, HUINT32 unTrack, HUINT32 unFragment, SSTR_URL_t *pUrl, HUINT8 **pBuf, HUINT32 *unBufSize, HUINT64 *pullRTD)
{
	HINT32 nRet = SSTR_ERR_OK;
//	STREAM_ACCESS_t *pAccess = NULL;
//	STREAM_ACCESS_t tAccess;
	size_t sz_RemainSize=0;
	size_t sz_Real=0;
	size_t sz_RealTotal=0;
	size_t sz_FragSize = 0;
	size_t sz_Try = 0;
	HINT32 n_ReturnedValue = 0;
	HUINT32 unStartTick = 0;
	HUINT32 unTick = 0;
	HUINT32 unEndTick = 0;
	SSTR_t* pSstr = NULL;

	PrintEnter;
	if(hSstr == NULL)
	{
		PrintError("%s(%d) Error> SSTR HANDLE is NULL \n", __FUNCTION__, __LINE__);
		PrintExit;
		return SSTR_ERR_FAIL;
	}
	pSstr = __getSSTR(hSstr);
	if(pSstr == NULL)
	{
		PrintError("%s(%d) Error> SSTR HANDLE is NULL \n", __FUNCTION__, __LINE__);
		PrintExit;
		return SSTR_ERR_FAIL;
	}
	if(pSstr->hCtrl == NULL)
	{
		PrintError("%s(%d) Error> SSTR Controller is NULL \n", __FUNCTION__, __LINE__);
		PrintExit;
		return SSTR_ERR_FAIL;
	}
	if(pUrl == NULL)
	{
		PrintError("%s(%d) Error> SSTR Url is NULL \n", __FUNCTION__, __LINE__);
		PrintExit;
		return SSTR_ERR_FAIL;
	}
	if(unBufSize == NULL)
	{
		PrintError("%s(%d) Error> Buffer Size is NULL \n", __FUNCTION__, __LINE__);
		PrintExit;
		return SSTR_ERR_FAIL;
	}
	if(pAccess == NULL)
	{
		PrintError("%s(%d) Error> STREAM_ACCESS HANDLE is NULL \n", __FUNCTION__, __LINE__);
		PrintExit;
		return SSTR_ERR_FAIL;
	}
	VK_SEM_Get(pSstr->ulLiveSema);
	unStartTick = VK_TIMER_GetSystemTick(); /* 1000 tick == 1 sec */
#if 0
	VK_memset(&tAccess,0x00,sizeof(STREAM_ACCESS_t));
	pAccess = &tAccess;
#endif
	nRet = SSTR_CONTROLLER_Get_N_FragmentUrl(pSstr->hCtrl,unStream,unTrack,unFragment,pUrl);
	if(nRet != SSTR_ERR_OK)
	{
		PrintError("%s(%d) : Error> SSTR_CONTROLLER_GetFragmentUrl.\n",__FUNCTION__,__LINE__);
		PrintExit;
		VK_SEM_Release(pSstr->ulLiveSema);
		return nRet;
	}
#if 0
	pAccess->psz_path = VK_StrDup(pUrl->url);
	if(pAccess->psz_path == NULL)
	{
		PrintError("%s(%d) : Error> Fail Mem alloc.\n",__FUNCTION__,__LINE__);
		PrintExit;
		VK_SEM_Release(pSstr->ulLiveSema);
		return SSTR_ERR_FAIL;
	}
	VK_SEM_Release(pSstr->ulLiveSema);

	/* */
	nRet = STREAM_HTTP_Open (pAccess);
#else
	if(pAccess->psz_path != NULL)
	{
		DD_MEM_Free(pAccess->psz_path);
	}
	pAccess->psz_path = VK_StrDup(pUrl->url);
	if(pAccess->psz_path == NULL)
	{
		PrintError("%s(%d) : Error> Fail Mem alloc.\n",__FUNCTION__,__LINE__);
		PrintExit;
		VK_SEM_Release(pSstr->ulLiveSema);
		return SSTR_ERR_FAIL;
	}
	nRet = STREAM_HTTP_RequestWithPersist(pAccess,0);
	if(nRet != DI_IPT_SUCCESS)
	{
		STREAM_HTTP_Close(pAccess);
		nRet = STREAM_HTTP_Open(pAccess);
	}
	VK_SEM_Release(pSstr->ulLiveSema);
#endif
	if(nRet != DI_IPT_SUCCESS)
	{
		PrintError("%s(%d) : Error> HTTP_Open Fail : %s\n",__FUNCTION__,__LINE__,pAccess->psz_path);
		nRet = SSTR_ERR_CONNECT_FAIL;
	}
	else
	{
		PrintDebug("%s(%d) Download URL : %s\n",__FUNCTION__,__LINE__,pAccess->psz_path);

		if( pAccess->info.i_size > *unBufSize )
		{
			PrintError("%s(%d) : Error> Memory Reallocation(%u -> %"PRId64").\n",__FUNCTION__,__LINE__, *unBufSize, pAccess->info.i_size + SSTR_CONST_PADDING);
			if(*pBuf != NULL)
			{
				DD_MEM_Free(*pBuf);
			}
			*pBuf = DD_MEM_Alloc(pAccess->info.i_size + SSTR_CONST_PADDING);
			if(*pBuf == NULL)
			{
				PrintError("%s(%d) : Error> Fail Mem alloc.\n",__FUNCTION__,__LINE__);
				PrintExit;
				return SSTR_ERR_MEMORY_ALLOC;
			}
			*unBufSize = pAccess->info.i_size + SSTR_CONST_PADDING;
		}
		/* Reset Buffer */
		VK_MEM_Memset(*pBuf , 0x00, *unBufSize);
		/* Download 시작*/
		sz_FragSize = pAccess->info.i_size;
		/* 다운로드 받을 남은 사이즈를 입력*/
		sz_RemainSize = pAccess->info.i_size;
		do
		{
			/* 60KB 단위로 다운로드*/
			if(sz_RemainSize > DI_IPTUNER_SSTR_CONST_RECV_BLOCKSIZE)
			{
				sz_Try = DI_IPTUNER_SSTR_CONST_RECV_BLOCKSIZE;
			}
			else
			{
				sz_Try = sz_RemainSize;
			}

			sz_Real=0;

			P_STREAM_SSTR_TimerStart(&pSstr->stDInfo);

			do
			{
				n_ReturnedValue = STREAM_HTTP_Read ( pAccess, *pBuf + sz_RealTotal, sz_Try - sz_Real );
				if(n_ReturnedValue > 0)
				{
					sz_Real += n_ReturnedValue;
					sz_RealTotal += n_ReturnedValue;
				}
				else if (n_ReturnedValue == 0 && pAccess->info.b_eof )
				{
					PrintDebug ("%s(%d) : Debug > End of File. and recv zero \n",__FUNCTION__,__LINE__);
					break;
				}
				else
				{
					PrintError ("%s(%d) Stop!! STREAM_HTTP_Read Fail !!!\n",__FUNCTION__,__LINE__);
					nRet = SSTR_ERR_DOWNLOAD_FAIL;
					break;
				}

				if(sz_FragSize != pAccess->info.i_size)
				{
					PrintError ("%s(%d) STREAM_HTTP_Read Fail(Contents Size is Changed)!!!\n",__FUNCTION__,__LINE__);
					nRet = SSTR_ERR_DOWNLOAD_FAIL;
					break;
				}

				if(g_ePreBufferingTaskChangeState >= DI_IPTUNER_SSTR_BUFFERING_TASK_STATE_KILLING)
				{
					PrintDebug("%s(%d) Break Download Loop\n",__FUNCTION__,__LINE__);
					nRet = SSTR_ERR_DOWNLOAD_FORCED_EXIT;
					break;
				}

				unTick = VK_TIMER_GetSystemTick(); /* 1000 tick == 1 sec */
			}
			while (sz_Real < sz_Try);

			P_STREAM_SSTR_TimerSetDownloadTime(&pSstr->stDInfo, sz_Real);

			if(nRet != SSTR_ERR_OK)
			{
				break;
			}
			else
			{
				sz_RemainSize = sz_RemainSize - sz_Real;
			}

		}
		while(sz_RemainSize > 0);
	}
#if 0
	if(pAccess != NULL)
	{
		(void)STREAM_HTTP_Close (pAccess);
		if ( pAccess->psz_path != NULL )
		{
			VK_MEM_Free(pAccess->psz_path);
		}
	}
#endif
	unEndTick = VK_TIMER_GetSystemTick(); /* 1000 tick == 1 sec */
	if(pullRTD != NULL)
	{
		*pullRTD = unEndTick - unStartTick;
	}
	PrintExit;
	return nRet;
}

/**
 * @b Function @b Description <br>
 * 현재 설정된 Video or Audio Fragment 다운로드 <br>
 *
 * @param[in] pHTuner : TUNER HANDLE
 * @param[in] pullRTD : Download Delay
 *
 * @return  #  SSTR_ERR <br>
 */

static SSTR_ERR P_STREAM_SSTR_DownloadCurFragment(SSTR_HANDLE hSstr, STREAM_ACCESS_t *pAccess, HUINT64 *pullRTD)
{
	HINT32 nRet = SSTR_ERR_OK;
//	STREAM_ACCESS_t *pAccess = NULL;
//	STREAM_ACCESS_t tAccess;
	size_t sz_RemainSize=0;
	size_t sz_Real=0;
	size_t sz_RealTotal=0;
	size_t sz_Try = 0;
	HINT32 n_ReturnedValue = 0;
	SSTR_STREAM_TYPE_e eType;

	HUINT32 unStartTick = 0;
	HUINT32 unTick = 0;
	HUINT32 unEndTick = 0;
	SSTR_t* pSstr = NULL;

	PrintEnter;
	if(hSstr == NULL)
	{
		PrintError("%s(%d) Error> SSTR HANDLE is NULL \n", __FUNCTION__, __LINE__);
		PrintExit;
		return SSTR_ERR_FAIL;
	}
	if(pAccess == NULL)
	{
		PrintError("%s(%d) Error> STREAM_ACCESS HANDLE is NULL \n", __FUNCTION__, __LINE__);
		PrintExit;
		return SSTR_ERR_FAIL;
	}
	pSstr = __getSSTR(hSstr);
	if(pSstr == NULL)
	{
		PrintError("%s(%d) Error> SSTR HANDLE is NULL \n", __FUNCTION__, __LINE__);
		PrintExit;
		return SSTR_ERR_FAIL;
	}
	if(pSstr->hCtrl == NULL)
	{
		PrintError("%s(%d) Error> SSTR Controller is NULL \n", __FUNCTION__, __LINE__);
		PrintExit;
		return SSTR_ERR_FAIL;
	}
	VK_SEM_Get(pSstr->ulLiveSema);
	unStartTick = VK_TIMER_GetSystemTick(); /* 1000 tick == 1 sec */
#if 0
	VK_memset(&tAccess,0x00,sizeof(STREAM_ACCESS_t));
	pAccess = &tAccess;
#endif
	eType = SSTR_CONTROLLER_GetCurStreamType(pSstr->hCtrl);
	nRet = SSTR_CONTROLLER_GetFragmentUrl(pSstr->hCtrl,eType,&pSstr->tAVUrl);
	if(nRet != SSTR_ERR_OK)
	{
		PrintError("%s(%d) : Error> SSTR_CONTROLLER_GetFragmentUrl.\n",__FUNCTION__,__LINE__);
		PrintExit;
		VK_SEM_Release(pSstr->ulLiveSema);
		return nRet;
	}
#if 0
	pAccess->psz_path = VK_StrDup(pSstr->tAVUrl.url);
	if(pAccess->psz_path == NULL)
	{
		PrintError("%s(%d) : Error> Fail Mem alloc.\n",__FUNCTION__,__LINE__);
		PrintExit;
		VK_SEM_Release(pSstr->ulLiveSema);
		return SSTR_ERR_FAIL;
	}
	VK_SEM_Release(pSstr->ulLiveSema);

	/* */
	nRet = STREAM_HTTP_Open (pAccess);
#else
	if(pAccess->psz_path != NULL)
	{
		VK_free(pAccess->psz_path);
	}
	pAccess->psz_path = VK_StrDup(pSstr->tAVUrl.url);
	if(pAccess->psz_path == NULL)
	{
		PrintError("%s(%d) : Error> Fail Mem alloc.\n",__FUNCTION__,__LINE__);
		PrintExit;
		VK_SEM_Release(pSstr->ulLiveSema);
		return SSTR_ERR_FAIL;
	}
	nRet = STREAM_HTTP_RequestWithPersist(pAccess,0);
	if(nRet != DI_IPT_SUCCESS)
	{
		STREAM_HTTP_Close(pAccess);
		nRet = STREAM_HTTP_Open(pAccess);
	}
	VK_SEM_Release(pSstr->ulLiveSema);
#endif
	if(nRet != DI_IPT_SUCCESS)
	{
		PrintError("%s(%d) : Error> HTTP_Open Fail : %s\n",__FUNCTION__,__LINE__,pAccess->psz_path);
		nRet = SSTR_ERR_CONNECT_FAIL;
	}
	else
	{
		PrintDebug("%s(%d) Download URL : %s\n",__FUNCTION__,__LINE__,pAccess->psz_path);

		if( pAccess->info.i_size > pSstr->ulBuffSize )
		{
			PrintError("%s(%d) : Error> Memory Reallocation(%u -> %"PRId64").\n",__FUNCTION__,__LINE__, pSstr->ulBuffSize, pAccess->info.i_size + SSTR_CONST_PADDING);
			if(pSstr->pFragBuff != NULL)
			{
				DD_MEM_Free(pSstr->pFragBuff);
			}
			pSstr->pFragBuff = DD_MEM_Alloc(pAccess->info.i_size + SSTR_CONST_PADDING);
			if(pSstr->pFragBuff == NULL)
			{
				PrintError("%s(%d) : Error> Fail Mem alloc.\n",__FUNCTION__,__LINE__);
				PrintExit;
				return SSTR_ERR_MEMORY_ALLOC;
			}
			pSstr->ulBuffSize = pAccess->info.i_size + SSTR_CONST_PADDING;
		}

		/* Reset Buffer */
		VK_MEM_Memset(pSstr->pFragBuff, 0x00, pSstr->ulBuffSize);
		/* Download 시작*/
		pSstr->sz_FragSize = pAccess->info.i_size;
		/* 다운로드 받을 남은 사이즈를 입력*/
		sz_RemainSize = pAccess->info.i_size;
		do
		{
			/* 60KB 단위로 다운로드*/
			if(sz_RemainSize > DI_IPTUNER_SSTR_CONST_RECV_BLOCKSIZE)
			{
				sz_Try = DI_IPTUNER_SSTR_CONST_RECV_BLOCKSIZE;
			}
			else
			{
				sz_Try = sz_RemainSize;
			}

			sz_Real=0;

			P_STREAM_SSTR_TimerStart(&pSstr->stDInfo);

			do
			{

				n_ReturnedValue = STREAM_HTTP_Read ( pAccess, pSstr->pFragBuff+sz_RealTotal, sz_Try - sz_Real );
				if(n_ReturnedValue > 0)
				{
					sz_Real += n_ReturnedValue;
					sz_RealTotal += n_ReturnedValue;
				}
				else if (n_ReturnedValue == 0 && pAccess->info.b_eof )
				{
					PrintDebug ("%s(%d) : Debug > End of File. and recv zero \n",__FUNCTION__,__LINE__);
					break;
				}
				else
				{
					PrintError ("%s(%d) Stop!! STREAM_HTTP_Read Fail !!!\n",__FUNCTION__,__LINE__);
					nRet = SSTR_ERR_DOWNLOAD_FAIL;
					break;
				}

				if(pSstr->sz_FragSize != pAccess->info.i_size)
				{
					PrintError ("%s(%d) STREAM_HTTP_Read Fail(Contents Size is Changed)!!!\n",__FUNCTION__,__LINE__);
					nRet = SSTR_ERR_DOWNLOAD_FAIL;
					break;
				}

				if(g_ePreBufferingTaskChangeState >= DI_IPTUNER_SSTR_BUFFERING_TASK_STATE_KILLING)
				{
					PrintDebug("%s(%d) Break Download Loop\n",__FUNCTION__,__LINE__);
					nRet = SSTR_ERR_DOWNLOAD_FORCED_EXIT;
					break;
				}
				if(P_STREAM_SSTR_IsSeekRequested(pSstr))
				{
					PrintDebug("%s(%d) Exit Download (Seek Requested)\n",__FUNCTION__,__LINE__);
					nRet = SSTR_ERR_DOWNLOAD_FORCED_EXIT;
					break;
				}
				unTick = VK_TIMER_GetSystemTick(); /* 1000 tick == 1 sec */
				if( (unTick - unStartTick) >= ( SSTR_CONST_DEFAULT_DURATION * SSTR_CONST_MILLISEC * 2))
				{
					if(SSTR_CONTROLLER_GetCurStreamType(pSstr->hCtrl) == SSTR_TYPE_VIDEO)
					{
						if(SSTR_CONTROLLER_GetCurBitrate(pSstr->hCtrl) != SSTR_CONTROLLER_GetMinBitrate(pSstr->hCtrl))
						{
							if(SSTR_CONTROLLER_CheckHeuristicsPanic(pSstr->hCtrl,unTick - unStartTick))
							{
								PrintDebug("%s(%d) Exit Download (Network Bandwidth Panic)\n",__FUNCTION__,__LINE__);
								nRet = SSTR_ERR_DOWNLOAD_PANIC;
								break;
							}
						}
					}
				}
			}
			while (sz_Real < sz_Try);

			P_STREAM_SSTR_TimerSetDownloadTime(&pSstr->stDInfo, sz_Real);

			if(nRet != SSTR_ERR_OK)
			{
				break;
			}
			else
			{
				sz_RemainSize = sz_RemainSize - sz_Real;
			}

		}
		while(sz_RemainSize > 0);
	}
#if 0
	if(pAccess != NULL)
	{
		(void)STREAM_HTTP_Close (pAccess);
		if ( pAccess->psz_path != NULL )
		{
			VK_MEM_Free(pAccess->psz_path);
		}
	}
#endif
	unEndTick = VK_TIMER_GetSystemTick(); /* 1000 tick == 1 sec */
	if(pullRTD != NULL)
	{
		*pullRTD = unEndTick - unStartTick;
	}
	PrintExit;
	return nRet;
}

/**
 * @b Function @b Description <br>
 * Seek 요청 여부 Flag Setting <br>
 *
 * @param[in] pSstr : SSTR HANDLE
 * @param[in] bPause : Pause 요청 여부 Flag (Boolean)
 *
 * @return  #  SSTR_ERR <br>
 */
SSTR_ERR P_STREAM_SSTR_SetPause(SSTR_t* pSstr, HBOOL bPause)
{
//	SSTR_CONTROLLER_t* pController = NULL;
	PrintEnter;
	if(pSstr == NULL)
	{
		PrintError("%s(%d) Err> SSTR HANDLE is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return SSTR_ERR_FAIL;
	}
//	pController = __controller(hCtrl);
	pSstr->bPaused = bPause;
	PrintExit;
	return SSTR_ERR_OK;
}


/**
 * @b Function @b Description <br>
 * Seek 요청 여부 Flag Setting <br>
 *
 * @param[in] pSstr : SSTR HANDLE
 * @param[in] bRequest : Seek 요청 여부 Flag (Boolean)
 *
 * @return  #  SSTR_ERR <br>
 */
SSTR_ERR P_STREAM_SSTR_SetRequestSeek(SSTR_t* pSstr, HBOOL bRequest)
{
//	SSTR_CONTROLLER_t* pController = NULL;
	PrintEnter;
	if(pSstr == NULL)
	{
		PrintError("%s(%d) Err> SSTR HANDLE is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return SSTR_ERR_FAIL;
	}
//	pController = __controller(hCtrl);
	VK_SEM_Get(pSstr->ulSeekSema);
	pSstr->bRequestedSeek = bRequest;
	VK_SEM_Release(pSstr->ulSeekSema);
	PrintExit;
	return SSTR_ERR_OK;
}

/**
 * @b Function @b Description <br>
 * Seek 요청 Time Offset <br>
 *
 * @param[in] pSstr : SSTR HANDLE
 * @param[in] ullTime : Seek 요청 Time Offset
 *
 * @return  #  SSTR_ERR <br>
 */
SSTR_ERR P_STREAM_SSTR_SetTimeForSeek(SSTR_t* pSstr, HUINT64 ullTime)
{
//	SSTR_CONTROLLER_t* pController = NULL;
	PrintEnter;
	if(pSstr == NULL)
	{
		PrintError("%s(%d) Err> SSTR HANDLE is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return SSTR_ERR_FAIL;
	}
//	pController = __controller(hCtrl);
	if(pSstr->bRequestedSeek == TRUE)
	{
		PrintExit;
		return SSTR_ERR_FAIL;
	}
	VK_SEM_Get(pSstr->ulSeekSema);
	pSstr->ullTimeForSeek = ullTime;
	VK_SEM_Release(pSstr->ulSeekSema);
	PrintExit;
	return SSTR_ERR_OK;
}

/**
 * @b Function @b Description <br>
 * Seek 요청 중인 Time Offset 반환 <br>
 *
 * @param[in] pSstr : SSTR HANDLE
 *
 * @return  #  Time Offset (HUINT64) <br>
 */
HUINT64 P_STREAM_SSTR_GetTimeForSeek(SSTR_t* pSstr)
{
	PrintEnter;
	if(pSstr == NULL)
	{
		PrintError("%s(%d) Err> SSTR HANDLE is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return 0;
	}
	PrintExit;
	return pSstr->ullTimeForSeek;
}
#if 1
int P_STREAM_SSTR_CreateManifestInfo(SSTR_t* pSstr)
{
	DI_IPTUNER_SSTR_Info_t* pSSTRInfo = NULL;
	PrintEnter;
	if(pSstr == NULL)
	{
		PrintError("%s(%d) : Error> SSTR HANDLE is NULL.\n",__FUNCTION__,__LINE__);
		return VFIO_ERROR_IN_DEVICE;
	}
	if(pSstr->hCtrl == NULL)
	{
		PrintError("%s(%d) Error> SSTR CONTROLLER is NULL. Check This !!\n"
		, __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}
	pSSTRInfo = (DI_IPTUNER_SSTR_Info_t*)DD_MEM_Alloc(sizeof(DI_IPTUNER_SSTR_Info_t));
	if(pSSTRInfo == NULL)
	{
		return DI_ERR_ERROR;
	}

	pSSTRInfo->nStreamCnt = SSTR_CONTROLLER_GetTotalStreams(pSstr->hCtrl);
	for(HINT32 i = 0 ; i < pSSTRInfo->nStreamCnt && i < DI_IPTUNER_SSTR_MAX_STREAM ; i++)
	{
		pSSTRInfo->stStream[i].eType = SSTR_CONTROLLER_GetStreamType(pSstr->hCtrl, i);
		pSSTRInfo->stStream[i].ulIndex = SSTR_CONTROLLER_GetStreamIndex(pSstr->hCtrl, i);
		pSSTRInfo->stStream[i].nNameLen = SSTR_CONTROLLER_GetStreamNameLen(pSstr->hCtrl, i);
		pSSTRInfo->stStream[i].pName = SSTR_CONTROLLER_GetStreamName(pSstr->hCtrl, i);
		pSSTRInfo->stStream[i].nLangLen = SSTR_CONTROLLER_GetStreamLangLen(pSstr->hCtrl, i);
		pSSTRInfo->stStream[i].pLanguage = SSTR_CONTROLLER_GetStreamLang(pSstr->hCtrl, i);
		pSSTRInfo->stStream[i].nTrackCnt = SSTR_CONTROLLER_GetTotalTracks(pSstr->hCtrl, i);
		for(HINT32 j = 0 ; j < pSSTRInfo->stStream[i].nTrackCnt && j < DI_IPTUNER_SSTR_MAX_TRACK ; j++)
		{
			pSSTRInfo->stStream[i].stTrack[j].ulIndex = SSTR_CONTROLLER_GetTrackIndex(pSstr->hCtrl, i, j);
			pSSTRInfo->stStream[i].stTrack[j].ulBitrate = SSTR_CONTROLLER_GetTrackBitrate(pSstr->hCtrl, i, j);
			pSSTRInfo->stStream[i].stTrack[j].pFourCC = SSTR_CONTROLLER_GetTrackFourCC(pSstr->hCtrl, i, j);
#if 0
			pSSTRInfo->stStream[i].stTrack[j].nCustomAttrCnt = ((pManifest->pStream + i)->pTrack+j)->nCustomAttrCnt;
			for(HINT32 k = 0 ; k < pSSTRInfo->stStream[i].stTrack[j].nCustomAttrCnt && k < DI_IPTUNER_SSTR_MAX_CATTR; k++)
			{
				pSSTRInfo->stStream[i].stTrack[j].stCustomAttr[k].nNameLen = (((pManifest->pStream + i)->pTrack+j)->pCustomAttr+k)->nNameLen;
				pSSTRInfo->stStream[i].stTrack[j].stCustomAttr[k].pName = (((pManifest->pStream + i)->pTrack+j)->pCustomAttr+k)->pName;
				pSSTRInfo->stStream[i].stTrack[j].stCustomAttr[k].nValueLen = (((pManifest->pStream + i)->pTrack+j)->pCustomAttr+k)->nValueLen;
				pSSTRInfo->stStream[i].stTrack[j].stCustomAttr[k].pValue = (((pManifest->pStream + i)->pTrack+j)->pCustomAttr+k)->pValue;
			}
#endif
		}
	}
	pSstr->pManifestInfo = pSSTRInfo;
	return DI_ERR_OK;
}

int P_STREAM_SSTR_DestroyManifestInfo(SSTR_t* pSstr)
{
	PrintEnter;
	if(pSstr == NULL)
	{
		PrintError("%s(%d) : Error> SSTR HANDLE is NULL.\n",__FUNCTION__,__LINE__);
		return VFIO_ERROR_IN_DEVICE;
	}
	if(pSstr->hCtrl == NULL)
	{
		PrintError("%s(%d) Error> SSTR CONTROLLER is NULL. Check This !!\n"
		, __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}
	if(pSstr->pManifestInfo == NULL)
	{
		PrintError("%s(%d) Error> SSTR Manifest Info is NULL. Check This !!\n"
		, __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}
	DD_MEM_Free(pSstr->pManifestInfo);
	pSstr->pManifestInfo = NULL;
	return DI_ERR_OK;
}
#endif


/**
 * @b Function @b Description <br>
 * Manifest Download <br>
 *
 * @param[in] pUrl : Manifest Url
 * @param[out] pMetaBuff : Manifest 를 저장할 Buffer
 * @param[out] pBaseUrl : Manifest 의 위치를 나타내는 URL (Fragment 요청에 사용될 Base URL)
 *
 * @return	#  Manifest 크기 (HINT64) <br>
 */
HINT64  P_STREAM_SSTR_GetManifest(STREAM_ACCESS_t *pAccess, HINT8 *pUrl, HINT8 **pMetaBuff,HINT8 **pBaseUrl)
{
	HINT64 	nSumLength=0;
	HINT32	nPos = 0;
	HINT32	nRet = DI_ERR_OK;
	HINT64  nContentLength = 0;
	HINT8 	*pBuffer=NULL;
	HINT32 	nRecvLength=0;
#if !SSTR_CONFIG_MEMORY_PARSE
	HINT32 	nRc = -1;
	HINT32	flags = O_CREAT|O_TRUNC|O_WRONLY|O_SYNC;
	HINT32	nFd = -1;
#endif
	HTTP_ACCESS_SYS_t *pstHttpSys=NULL;
//	STREAM_ACCESS_t tAccess;
	DI_IPTUNER_Msg_t s_Msg;
#if 0
	char *psCookie = NULL;
	URL_t tUrl;
#endif
#if !SSTR_CONFIG_MEMORY_PARSE
	UNUSED_PARAM(pMetaBuff);
#endif
	/*******************************************
	 * Open Protocol Controller
	 * 	: After the pVfioHandle created,
	 * 			IPTUNER handle has to link with it
	 *******************************************/
	if(pAccess == NULL)
	{
		nRet = DI_ERR_ERROR;
		/* TODO : Resource check */
		return 0;
	}
	(void) VK_memset (&s_Msg, 0x0, sizeof(s_Msg));
	if(NULL == VK_strstr((char*)pUrl,".ism/Manifest") && NULL== VK_strstr((char*)pUrl,".ism/manifest")
	&& NULL == VK_strstr((char*)pUrl,".isml/Manifest") && NULL== VK_strstr((char*)pUrl,".isml/manifest")
	&& NULL == VK_strstr((char*)pUrl,".smil/Manifest") && NULL== VK_strstr((char*)pUrl,".smil/manifest"))
	{
		PrintError ("%s (%d) Error>  Invalid Url for SSTR \n",__FUNCTION__,__LINE__);
		nRet = DI_ERR_ERROR;
		/* TODO : Resource check */
		return 0;
	}
	else
	{
//		VK_memset(&tAccess,0,sizeof(STREAM_ACCESS_t));
		VK_memset(pAccess,0,sizeof(STREAM_ACCESS_t));
		pAccess->b_use_persist = TRUE;
#if 0
		VK_MEM_Memset(&tUrl, 0x0, sizeof(tUrl));
		UTIL_UrlParse(&tUrl, (const char *)pUrl, 0);
		if(tUrl.psz_cookie != NULL)
		{
			psCookie = VK_StrDup(tUrl.psz_cookie);
		}
		UTIL_UrlClean(&tUrl);
#endif

		/** psz_path must be free */
		if(pAccess->psz_path != NULL)
		{
			VK_MEM_Free(pAccess->psz_path);
		}
		pAccess->psz_path = VK_StrDup ( (void *) UTIL_GetOriginalURL ((const char *)pUrl) );
#if 0
		if(psCookie != NULL)
		{
			tAccess.psz_cookie = psCookie;
		}
#endif
		if(pAccess->psz_path == NULL)
		{
			PrintError("%s(%d) : Error> Fail Mem alloc.\n",__FUNCTION__,__LINE__);

			if(pAccess->psz_path != NULL)
			{
				DD_MEM_Free(pAccess->psz_path);
				pAccess->psz_path =NULL;
			}
			/* else { continue; } */
#if 0
			if(tAccess.psz_cookie != NULL)
			{
				DD_MEM_Free(tAccess.psz_cookie);
				tAccess.psz_cookie =NULL;
			}
			/* else { continue; } */
#endif
			PrintExit;
			return 0;
		}

		/* Try to open the STREAM */
		if (DI_IPT_SUCCESS != STREAM_HTTP_Open (pAccess) )
		{
			if(pAccess->psz_path != NULL)
			{
				DD_MEM_Free(pAccess->psz_path);
				pAccess->psz_path =NULL;
			}
			/* else { continue; } */
#if 0
			if(tAccess.psz_cookie != NULL)
			{
				DD_MEM_Free(tAccess.psz_cookie);
				tAccess.psz_cookie =NULL;
			}
			/* else { continue; } */
#endif
			PrintError("%s (%d) : Error> STREAM_HTTP_Open Fail.\n",__FUNCTION__,__LINE__);
			PrintExit;
			return 0;
		}
		else
		{
			pstHttpSys = (HTTP_ACCESS_SYS_t *)pAccess->p_sys;
			*pBaseUrl = (HINT8*)DD_MEM_Alloc( VK_strlen( pAccess->psz_path )+SSTR_CONST_PADDING);
			VK_memset(*pBaseUrl,0x00,VK_strlen( pAccess->psz_path )+SSTR_CONST_PADDING);
			VK_MEM_Memcpy(*pBaseUrl, pAccess->psz_path, VK_strlen( pAccess->psz_path ));
		}
	}
	nContentLength = pAccess->info.i_size;
	PrintDebug ("%s(%d): Open %s\n",__FUNCTION__,__LINE__,pUrl);
	PrintDebug ("%s(%d): nContentLength (%lld)bytes \n",__FUNCTION__,__LINE__,nContentLength);
#if SSTR_CONFIG_MEMORY_PARSE
	*pMetaBuff = (HINT8*) DD_MEM_Alloc (nContentLength+SSTR_CONST_PADDING);
	if(*pMetaBuff==NULL)
	{
		PrintError ("%s(%d) Error> Heap Memory Allocation Fail. size (%"PRId64") !!\n",__FUNCTION__,__LINE__,nContentLength+SSTR_CONST_PADDING);
		return 0;
	}
	VK_memset(*pMetaBuff,0x00,nContentLength+SSTR_CONST_PADDING);
#else
	nFd = open (SSTR_CONFIG_TEMP_CACHE, flags, 0666);
	if(nFd <0)
	{
		PrintError ("%s(%d) : Error> Manifest Cache File Open Fail %s\n",__FUNCTION__,__LINE__,SSTR_CONFIG_TEMP_CACHE);
		return 0;
	}
#endif
	/* heap allocation */
	pBuffer = (HINT8*) DD_MEM_Alloc (SSTR_CONST_HTTP_RECV_BUFFSIZE);
	if(pBuffer==NULL)
	{
		PrintError ("%s(%d) Error> Heap Memory Allocation Fail. size (%d) !!\n",__FUNCTION__,__LINE__,SSTR_CONST_HTTP_RECV_BUFFSIZE);
		return 0;
	}

	do
	{
		VK_memset(pBuffer,0x00,SSTR_CONST_HTTP_RECV_BUFFSIZE);
		nRecvLength = 0;
		nRecvLength = STREAM_HTTP_Read ( pAccess, (HUINT8 *)pBuffer, SSTR_CONST_HTTP_RECV_BUFFSIZE );
		if(nRecvLength > 0) /* normal case */
		{
			if(pAccess->info.i_size != nContentLength)
			{
				nSumLength = -1;
				break;
			}
#if SSTR_CONFIG_MEMORY_PARSE
			if(nContentLength < nPos + nRecvLength)
			{
				PrintError ("%s(%d) : Error > BufferOverFlow !!Content : %lld, Pos : %d, RecvLength : %d \n",__FUNCTION__,__LINE__,nContentLength,nPos,nRecvLength);
				nSumLength = -1;
				break;
			}
			else
			{
				VK_MEM_Memcpy(*pMetaBuff+nPos, pBuffer, nRecvLength);
				nPos+=nRecvLength;
				nSumLength+=nRecvLength;
			}
#else
			nRc = write (nFd, pBuffer, nRecvLength);
			nPos+=nRecvLength;
			nSumLength+=nRecvLength;
#endif
			usleep(0);
		}
		else if (nRecvLength == 0 && pAccess->info.b_eof )
		{
			PrintDebug ("%s(%d) : Debug > End of File. and recv zero \n",__FUNCTION__,__LINE__);
			break;
		}
		else
		{
			PrintError ("%s(%d) : Error > Read Fail. \n",__FUNCTION__,__LINE__);
			nSumLength = -1;
			break;
		}
	}
	while (nSumLength < nContentLength );

	if(pBuffer != NULL)
	{
		VK_MEM_Free(pBuffer);
	}
#if SSTR_CONFIG_MEMORY_PARSE
#else
	close(nFd);
#endif
//	PrintDebug ("%s(%d) : Content : %lld, Pos : %d, RecvLength : %d \n",__FUNCTION__,__LINE__,nContentLength,nPos,nSumLength);
#if 0
	(void) STREAM_HTTP_Close ( &tAccess );
	tAccess.b_alive = FALSE;
	if(tAccess.psz_path != NULL)
	{
		DD_MEM_Free  (tAccess.psz_path);
		tAccess.psz_path =NULL;
	}
#endif
	/* else { continue; } */
#if 0
	if(tAccess.psz_cookie != NULL)
	{
		DD_MEM_Free(tAccess.psz_cookie);
		tAccess.psz_cookie =NULL;
	}
	/* else { continue; } */
#endif
	nContentLength = nSumLength;
	return nContentLength;
}

/**
 * @b Function @b Description <br>
 * bmp4 Box 생성  <br>
 *
 * @param[in] pSstr : SSTR HANDLE
 *
 * @return	#  생성된 bmp4 box의 크기 (HUINT32) <br>
 */
HUINT32	P_STREAM_SSTR_MakeBox (SSTR_t* pSstr)
{
#ifdef SSTR_CONFIG_WRITE_FRAGMENT_FOR_TEST
	HINT8 strBuf[512]= {0};
#endif
	SSTR_STREAM_TYPE_e eType;
	HUINT32 ulCodecPrivateData = 0;
	HUINT32 ulIndex = 0;
	HUINT64 ullTimeScale = 0;
	HUINT64 ullStartTime = 0;
	size_t offset = 0;
	HINT8 *pFourCC = NULL;
	HINT8 *pCodecPrivate = NULL;
	HINT8 szCodecPrivData[SSTR_CONST_CODECPRIVDATA_BUFFER] = {0};
	int nCodecPrivateDataLen = 0;
	PrintEnter;
	if(pSstr == NULL || pSstr->hCtrl == NULL)
	{
		PrintError("%s(%d) Err> SSTR HANDLE is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return 0;
	}
	if(pSstr->mp4Box.bBoxing == TRUE)
	{
		PrintExit;
		return 0;
	}
	else
	{
		eType = SSTR_CONTROLLER_GetCurStreamType(pSstr->hCtrl);
		ullStartTime = SSTR_CONTROLLER_GetCurFragmentStartTime(pSstr->hCtrl, eType);
		pCodecPrivate = SSTR_CONTROLLER_GetCurTrackCodecPrivate(pSstr->hCtrl, eType);
		pFourCC = SSTR_CONTROLLER_GetCurTrackFourCC(pSstr->hCtrl, eType);

		if(eType == SSTR_TYPE_VIDEO)
		{
			ulIndex = SSTR_CONST_VIDEO_STREAM_ID;
			//ulIndex = SSTR_CONST_VIDEO_STREAM_ID + SSTR_CONTROLLER_GetCurAVStream(pSstr->hCtrl, SSTR_TYPE_VIDEO);
		}
		else if(eType == SSTR_TYPE_AUDIO)
		{
			ulIndex = SSTR_CONST_AUDIO_STREAM_ID;
		}
		if(P_STREAM_SSTR_FourCCToDICodec(pFourCC) == 0) /* AV Stream Codec Check with FourCC */
		{
			pFourCC = SSTR_CONTROLLER_GetCurStreamSubtype(pSstr->hCtrl, eType);
		}
		if(pFourCC == NULL)
		{
			PrintError("%s(%d) Err> FourCC is NULL\n",__FUNCTION__,__LINE__);
			PrintExit;
			return 0;
		}
		ullTimeScale = SSTR_CONST_DEFAULT_TIMESCALE;
		offset = start_mp4_box(&pSstr->mp4Box, SSTR_MP4BOX_BMP4);
		write_bhed_box(&pSstr->mp4Box, ullTimeScale, ullStartTime, (char*)pFourCC);
		if(pCodecPrivate == NULL/* || SSTR_CONTROLLER_GetCurStreamType(pSstr->hCtrl) == SSTR_TYPE_AUDIO*/)
		{
			if(!VK_strncmp(pFourCC,SSTR_CONST_FOURCC_AACL,VK_strlen(SSTR_CONST_FOURCC_AACL)))
			{
				HINT32 nProfile = 2; /* Actually 2 */
				HINT32 nSamplingRateIndex = -1;
				HINT32 nChannelsIndex = -1;
				HUINT32 ulSamplingRate = SSTR_CONTROLLER_GetCurAudioSamplingRate(pSstr->hCtrl);
				HUINT32 ulChannels = SSTR_CONTROLLER_GetCurAudioChannels(pSstr->hCtrl);
				for(unsigned int i = 0; i < sizeof(b_aac_adts_sample_rate)/sizeof(*b_aac_adts_sample_rate); i++)
				{
					if (b_aac_adts_sample_rate[i] == ulSamplingRate)
					{
						nSamplingRateIndex = i;
						break;
					}
				}
				for(unsigned int i = 0; i < sizeof(b_aac_adts_channels)/sizeof(*b_aac_adts_channels); i++)
				{
					if (b_aac_adts_channels[i] == ulChannels)
					{
						nChannelsIndex = i;
						break;
					}
				}
				if(nSamplingRateIndex >= 0 && nChannelsIndex >= 0)
				{
					ulCodecPrivateData = ((nProfile << 28) | (nSamplingRateIndex << 24) | (nChannelsIndex << 20));
					ulCodecPrivateData = ulCodecPrivateData >> 1;
					uint2array((char*)szCodecPrivData,ulCodecPrivateData);
				}
				nCodecPrivateDataLen = 3;
			}

		}
		else
		{
			nCodecPrivateDataLen = hex2bin((char*)pCodecPrivate, (unsigned char*)szCodecPrivData, SSTR_CONST_CODECPRIVDATA_BUFFER);
			if(nCodecPrivateDataLen < 0 )
			{
				PrintError("%s(%d) Err> Invalid CodecPrivateData\n",__FUNCTION__,__LINE__);
				nCodecPrivateDataLen = 0;
			}
		}
		write_bdat_box(&pSstr->mp4Box, (unsigned char*)szCodecPrivData, nCodecPrivateDataLen);
		write_trex_box(&pSstr->mp4Box, ulIndex, 0, 0, 0, 0);
		close_mp4_box(&pSstr->mp4Box, pSstr->mp4Box.length + pSstr->sz_FragSize);
	}
	pSstr->mp4Box.bBoxing = TRUE;

#ifdef SSTR_CONFIG_WRITE_FRAGMENT_FOR_TEST
#ifdef SSTR_CONFIG_WRITE_ONE_FILE_FOR_TEST
#else
	if(eType == SSTR_TYPE_VIDEO)
	{
		VK_sprintf(strBuf,"/home/video_%llu",ullStartTime);
	}
	else
	{
		VK_sprintf(strBuf,"/home/audio_%llu",ullStartTime);
	}
	pSstr->fd = open(strBuf,O_CREAT|O_TRUNC|O_WRONLY|O_SYNC,0666);
#endif
#endif
//	VK_SEM_Release(pSstr->ulSema);
	PrintExit;
	return pSstr->mp4Box.length;
}

#ifdef SSTR_CONFIG_WRITE_FRAGMENT_FOR_TEST
SSTR_ERR	P_STREAM_SSTR_WriteFrag (SSTR_t* pSstr,HINT8* pBuf, HINT32 nBufPos, HINT32 size)
{
	PrintEnter;
	// P_SSTR_CONTROLLER_Lock(hCtrl);
	if(pSstr == NULL)
	{
		PrintError("%s(%d) Err> SSTR HANDLE is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return 0;
	}
	write(pSstr->fd,pBuf + nBufPos, size);
}
#endif

/**
 * @b Function @b Description <br>
 * bmp4 Box 를 비움 <br>
 *
 * @param[in] pSstr : SSTR HANDLE
 *
 * @return	# SSTR_ERR <br>
 */
SSTR_ERR	P_STREAM_SSTR_ClearBox (SSTR_t* pSstr)
{
	PrintEnter;
	if(pSstr == NULL)
	{
		PrintError("%s(%d) Err> SSTR HANDLE is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return 0;
	}
	pSstr->mp4Box.length = 0;
	pSstr->mp4Box.offset = 0;
	pSstr->mp4Box.bBoxing = FALSE;
	VK_MEM_Memset(pSstr->mp4Box.buf,0x00,pSstr->mp4Box.buf_len);
#ifdef SSTR_CONFIG_WRITE_FRAGMENT_FOR_TEST
#ifdef SSTR_CONFIG_WRITE_ONE_FILE_FOR_TEST
#else
	close(pSstr->fd);
#endif
#endif
	PrintExit;
	return SSTR_ERR_OK;
}

/**
 * @b Function @b Description <br>
 * Access Handle의 URL을 변경 <br>
 *
 * @param[in] pAccess : ACCESS HANDLE
 * @param[in] pUrl : 변경할 URL
 *
 * @return	# DI_ERR_CODE <br>
 */
DI_ERR_CODE P_STREAM_SSTR_ChangeUrl(STREAM_ACCESS_t *pAccess, HINT8 *pUrl)
{
	HTTP_ACCESS_SYS_t	*pHttpSys = NULL;
	if(pUrl == NULL)
	{
		PrintError("%s(%d) : Error> Url is NULL.\n",__FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}
	if(pAccess == NULL)
	{
		PrintError("%s(%d) : Error> STREAM ACCESS is NULL.\n",__FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}
	pHttpSys = (HTTP_ACCESS_SYS_t*)pAccess->p_sys;
	if(pHttpSys == NULL)
	{
		PrintError("%s(%d) : Error> HTTP_ACCESS_SYS_t is NULL.\n",__FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}
	UTIL_UrlClean( &pHttpSys->url );
	UTIL_UrlParse( &pHttpSys->url, (const char *)pUrl, 0 );

	/* close current connection */
	(void) STREAM_HTTP_Close ( pAccess );

	pAccess->b_alive = FALSE;

	if(pAccess->psz_path != NULL)
	{
		DD_MEM_Free  (pAccess->psz_path);
		pAccess->psz_path =NULL;
	}
	/* else { continue; } */

#if SUPPORT_HUMAX_COOKIE
	if(pAccess->psz_cookie != NULL)
	{
		DD_MEM_Free(pAccess->psz_cookie);
		pAccess->psz_cookie =NULL;
	}
	/* else { continue; } */
#endif

	/* open new connection with next segment */

#if SUPPORT_HUMAX_COOKIE
	if(pHttpSys->url.psz_cookie != NULL)
	{
		if(pAccess->psz_cookie != NULL)
		{
			VK_MEM_Free(pAccess->psz_cookie);
		}
		pAccess->psz_cookie = VK_StrDup(pHttpSys->url.psz_cookie);
	}
#endif

	/* open for player */
	/** psz_path must be free */
	if(pAccess->psz_path != NULL)
	{
		VK_MEM_Free(pAccess->psz_path);
	}
	pAccess->psz_path = VK_StrDup (pUrl);

	/* Try to open the STREAM */
	if (DI_IPT_SUCCESS != STREAM_HTTP_Open (pAccess) )
	{
		usleep(10); /* for test */
		if(DI_IPT_SUCCESS != STREAM_HTTP_Open (pAccess))
		{
			if(pAccess->psz_path != NULL)
			{
				DD_MEM_Free(pAccess->psz_path);
				pAccess->psz_path =NULL;
			}
			/* else { continue; } */
#if SUPPORT_HUMAX_COOKIE
			if(pAccess->psz_cookie != NULL)
			{
				DD_MEM_Free(pAccess->psz_cookie);
				pAccess->psz_cookie =NULL;
			}
			/* else { continue; } */
#endif
			if(pAccess->psz_path != NULL)
			{
				DD_MEM_Free  (pAccess->psz_path);
				pAccess->psz_path =NULL;
			}
			/* else { continue; } */
		}

		PrintError("%s (%d) : Error> STREAM_HTTP_Open Fail.\n",__FUNCTION__,__LINE__);
		PrintExit;
		return DI_ERR_ERROR;
	}
	return DI_ERR_OK;
}

/**
 * @b Function @b Description <br>
 * 다음 Fragment를 Setting <br>
 *
 * @param[in] hSstr : SSTR HANDLE
 *
 * @return	# DI_ERR_CODE <br>
 */
DI_ERR_CODE P_STREAM_SSTR_NextFragment(SSTR_HANDLE hSstr)
{
	SSTR_STREAM_TYPE_e eType;
	SSTR_STREAM_TYPE_e eNextType;
	HUINT32 unCurAVIndex = 0;
	HUINT32 unNextAVIndex = 0;
	HUINT32 unFragmentId = 0;
	HUINT32 unNextFragmentId = 0;
	HUINT64 ullStartTime = 0;
	HUINT64 ullNextStartTime = 0;
	HINT32 nRet = DI_ERR_OK;
	SSTR_t* pSstr = NULL;
	if(NULL == hSstr)
	{
		PrintError("%s(%d) Error> SSTR HANDLE is NULL. Check This !!\n"
		, __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}
	pSstr = __getSSTR(hSstr);
	if(pSstr->hCtrl == NULL)
	{
		PrintError("%s(%d) Error> SSTR CONTROLLER is NULL. Check This !!\n"
		, __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}
	VK_SEM_Get(pSstr->ulLiveSema);
	eType = SSTR_CONTROLLER_GetCurStreamType(pSstr->hCtrl);
	if(eType == SSTR_TYPE_VIDEO)
	{
		eNextType = SSTR_TYPE_AUDIO;
	}
	else if (eType == SSTR_TYPE_AUDIO)
	{
		eNextType = SSTR_TYPE_VIDEO;
	}
	else
	{
		VK_SEM_Release(pSstr->ulLiveSema);
//		PrintExit;
		return DI_ERR_ERROR;
	}

	unCurAVIndex = SSTR_CONTROLLER_GetCurAVStream(pSstr->hCtrl,eType);
	unNextAVIndex = SSTR_CONTROLLER_GetCurAVStream(pSstr->hCtrl,eNextType);

	nRet = SSTR_CONTROLLER_NextFragment(pSstr->hCtrl,unCurAVIndex);
	if(nRet != DI_ERR_OK)
	{
		VK_SEM_Release(pSstr->ulLiveSema);
//		PrintExit;
		return nRet;
	}
	if( pSstr->eAVType == SSTR_CONTENTS_ALL)
	{
		unFragmentId = SSTR_CONTROLLER_GetCurFragment(pSstr->hCtrl, unCurAVIndex);
		unNextFragmentId = SSTR_CONTROLLER_GetCurFragment(pSstr->hCtrl, unNextAVIndex);
		SSTR_CONTROLLER_GetFragmentStartTime(pSstr->hCtrl, unCurAVIndex, unFragmentId,&ullStartTime);
		SSTR_CONTROLLER_GetFragmentStartTime(pSstr->hCtrl, unNextAVIndex, unNextFragmentId,&ullNextStartTime);
		if(ullStartTime < ullNextStartTime)
		{
			eNextType = eType;
			ullNextStartTime = ullStartTime;
		}
		SSTR_CONTROLLER_SetCurStreamType(pSstr->hCtrl,eNextType);

#if !defined(DI_IPTUNER_SSTR_CONFIG_AUDIO_FIRST)
		if(pSstr->ulMaxText > 0 && SSTR_CONTROLLER_GetCurStreamType(pSstr->hCtrl) == SSTR_TYPE_VIDEO)
#else
		if(pSstr->ulMaxText > 0 && SSTR_CONTROLLER_GetCurStreamType(pSstr->hCtrl) == SSTR_TYPE_AUDIO)
#endif
		{
			P_STREAM_SSTR_ReqTextStream(pSstr, SSTR_CONTROLLER_GetCurFragmentStartTime(pSstr->hCtrl, SSTR_CONTROLLER_GetCurStreamType(pSstr->hCtrl)));
		}

		if(eType == SSTR_TYPE_VIDEO)
		{
			pSstr->stDInfo.uCurFragmentIndex = unFragmentId;
			pSstr->stDInfo.ullCurFragmentStartTime = ullStartTime;
		}
	}
	else
	{
		if(pSstr->ulMaxText > 0)
		{
			P_STREAM_SSTR_ReqTextStream(pSstr, SSTR_CONTROLLER_GetCurFragmentStartTime(pSstr->hCtrl, SSTR_CONTROLLER_GetCurStreamType(pSstr->hCtrl)));
		}
	}
	VK_SEM_Release(pSstr->ulLiveSema);
//	PrintExit;
	return nRet;
}

#if 0 /* P_STREAM_SSTR_PrevSegment is not used */
DI_ERR_CODE P_STREAM_SSTR_PrevSegment(VFIO_Handle_t tHandle)
{
	HINT32 nRet = DI_ERR_OK;
	DI_IPTUNER_t *pTunerHandle = NULL;
	size_t sz_File=0;
	HINT8 szUrl[SSTR_CONST_URL_LENGTH]={0};
	pTunerHandle = tHandle.pTunerHandle;
	nRet = SSTR_PrevSegment(pTunerHandle->hAbr);
	if(nRet != DI_ERR_OK)
	{
		// nRet = -1;
		PrintExit;
		return nRet;
	}
	nRet = SSTR_GetSegmentUrl(pTunerHandle->hAbr,szUrl,SSTR_CONST_URL_LENGTH);
	if(nRet != DI_ERR_OK)
	{
		// nRet = -1;
		PrintExit;
		return nRet;
	}
	nRet = DI_STREAM_HTTP_ChangeUrl(tHandle, szUrl);
	if(nRet != DI_ERR_OK)
	{
		// nRet = -1;
		PrintExit;
		return nRet;
	}
	DI_STREAM_HTTP_BoundsForNexus(tHandle,&sz_File);
	nRet = SSTR_Queue_PushFront(pTunerHandle->hAbr,SSTR_GetCurrentSegmentId(pTunerHandle->hAbr),sz_File,SSTR_GetCurrentBitrate(pTunerHandle->hAbr));
	if(nRet != DI_ERR_OK)
	{
		// nRet = -1;
		PrintExit;
		return nRet;
	}
	PrintExit;
	return nRet;
}
#endif

#if 0 /* P_STREAM_SSTR_N_Segment is not used */
DI_ERR_CODE P_STREAM_SSTR_N_Segment(VFIO_Handle_t tHandle, HINT32 nSegment)
{
	HINT32 nRet = DI_ERR_OK;
	DI_IPTUNER_t *pTunerHandle = NULL;
	size_t sz_File=0;
	HINT8 szUrl[SSTR_CONST_URL_LENGTH]={0};
	pTunerHandle = tHandle.pTunerHandle;
	nRet = SSTR_SetSegment(pTunerHandle->hAbr,nSegment);
	if(nRet != DI_ERR_OK)
	{
		// nRet = -1;
		PrintExit;
		return nRet;
	}
	nRet = SSTR_GetSegmentUrl(pTunerHandle->hAbr,szUrl,SSTR_CONST_URL_LENGTH);
	if(nRet != DI_ERR_OK)
	{
		// nRet = -1;
		PrintExit;
		return nRet;
	}
	nRet = DI_STREAM_HTTP_ChangeUrl(tHandle, szUrl);
	if(nRet != DI_ERR_OK)
	{
		// nRet = -1;
		PrintExit;
		return nRet;
	}
	//DI_STREAM_HTTP_BoundsForNexus(tHandle,&sz_File);
	// SSTR_Queue_Reset(pTunerHandle->hAbr);
	// SSTR_Queue_PushBack(pTunerHandle->hAbr,SSTR_GetCurrentSegmentId(pTunerHandle->hAbr),sz_File,SSTR_GetCurrentBitrate(pTunerHandle->hAbr));
	PrintExit;
	return nRet;
}
#endif

/**
 * @b Function @b Description <br>
 * Live Manifest Update를 위한 Task <br>
 * 현재 미완성 <br>
 *
 * @param[in] pParam : Session Handle : Session Handle
 * @todo 추가 작업 필요
 *
 * @return	# void <br>
 */
void P_STREAM_SSTR_LiveUpdate_Task(void *pParam)
{
	SSTR_t* pSstr;
	SSTR_STREAM_TYPE_e eType = SSTR_TYPE_VIDEO;
	HUINT32 i = 0;
	HUINT32 unFragment = 0;
	HUINT32 unTrack = 0;
	HUINT32 unTotalFragments = 0;
	HUINT32 unTotalStreams = 0;
//	HUINT64 ullDVRWindow = 0;

	STREAM_ACCESS_t *pAccess = NULL;

	HUINT64 ullLastUpdateTime = 0;
	HUINT64 ullCurrentTime = 0;
	HUINT64 ullMinUpdateDelay = 0;
	HUINT8 *pBuf = NULL;
	HUINT32 unBufSize = 0;

	UNUSED_PARAM(pParam);
	PrintEnter;
	if(g_hSstr == NULL)
	{
		PrintError("%s(%d) Error> SSTR HANDLE is NULL \n", __FUNCTION__, __LINE__);
		return;
	}
	pSstr = __getSSTR(g_hSstr);
	if(pSstr->hCtrl == NULL)
	{
		PrintError("%s(%d) Error> SSTR CONTROLLER is NULL. Check This !!\n"
		, __FUNCTION__, __LINE__);
		return;
	}
	if(pSstr->eAVType == SSTR_CONTENTS_AUDIO)
	{
		eType = SSTR_TYPE_AUDIO;
	}
	pAccess = DD_MEM_Alloc(sizeof(STREAM_ACCESS_t));
	if(pAccess == NULL)
	{
		PrintError("Buffer Allocation Fail\n");
		return;
	}
	VK_memset(pAccess,0x00,sizeof(STREAM_ACCESS_t));
	pAccess->b_use_persist = TRUE;
	if(pSstr->pManifestUrl == NULL)
	{
		PrintError("%s(%d) Error> Manifest URL is NULL\n"
				, __FUNCTION__, __LINE__);
		return;
	}
	pSstr->eLiveUpdateStatus = DI_IPTUNER_SSTR_LIVE_UPDATE_STARTED;
	ullMinUpdateDelay = SSTR_CONVERT_TO_MILLISEC( SSTR_CONTROLLER_GetCurFragmentDuration(pSstr->hCtrl,eType) );
	if(ullMinUpdateDelay <= 0 || ullMinUpdateDelay > 5000)
	{
		ullMinUpdateDelay = 5000;
	}
	ullMinUpdateDelay /= 2;
	/* Task Started Flag */
	do
	{
		ullCurrentTime = VK_TIMER_GetSystemTick();
		if(!SSTR_CONTROLLER_IsLive(pSstr->hCtrl))
		{
			PrintError ("%s(%d) Debug> Live Streaming -> VoD Streaming\n",__FUNCTION__,__LINE__);
			break;
		}
		if(g_ePreBufferingTaskChangeState >= DI_IPTUNER_SSTR_BUFFERING_TASK_STATE_KILLING)
		{
			break;
		}
		if(ullCurrentTime - ullLastUpdateTime < ullMinUpdateDelay && (ullLastUpdateTime < ullCurrentTime))
		{
			PrintDebug ("%s(%d) Debug> Try To Update Delay %"PRId64" - %"PRId64"(%"PRId64") : %"PRId64"\n",__FUNCTION__,__LINE__,ullCurrentTime,ullLastUpdateTime,ullCurrentTime - ullLastUpdateTime, ullMinUpdateDelay);
//			PrintDebug ("%s(%d) Debug> Try To Update Delay %u( TF : %u, CF : %u )\n",__FUNCTION__,__LINE__,unDelay,unTotalFragments,unFragment);
			VK_TASK_Sleep(100); /* 시간 확인 */
			continue;
		}
		unTotalStreams = SSTR_CONTROLLER_GetTotalStreams(pSstr->hCtrl);
		for(i = 0 ; i < unTotalStreams ; i++)
		{
			if(g_ePreBufferingTaskChangeState >= DI_IPTUNER_SSTR_BUFFERING_TASK_STATE_KILLING)
			{
				break;
			}
			unFragment = SSTR_CONTROLLER_GetCurFragment(pSstr->hCtrl,i);
			unTotalFragments = SSTR_CONTROLLER_GetTotalFragment(pSstr->hCtrl,i);
			eType = SSTR_CONTROLLER_GetStreamType(pSstr->hCtrl, i);
			if(eType != SSTR_TYPE_AUDIO && eType != SSTR_TYPE_VIDEO)
			{
				continue;
			}
			if(unTotalFragments == 0)
			{
				continue;
			}
			if((unTotalFragments - 1) == unFragment
			&& i == SSTR_CONTROLLER_GetCurAVStream(pSstr->hCtrl, eType))
			{
				continue;
			}
			unTrack = SSTR_CONTROLLER_GetMinBitrateTrack(pSstr->hCtrl, i);
			if(P_STREAM_SSTR_DownloadFragment(pSstr,pAccess,i,unTrack,(unTotalFragments - 1),&pSstr->tUpdateUrl,&pBuf,&unBufSize,NULL) == SSTR_ERR_OK)
			{
//				VK_printf(">> Live Update \n");
				(void)P_STREAM_SSTR_LiveUpdate(pSstr,pBuf,unBufSize,i);
			}
			else
			{
//				VK_printf(">> Live Update Fail\n");
				continue;
			}
		}
		ullLastUpdateTime = VK_TIMER_GetSystemTick();
		VK_TASK_Sleep(100); /* 시간 확인 */
	}
	while (g_ePreBufferingTaskChangeState < DI_IPTUNER_SSTR_BUFFERING_TASK_STATE_KILLING);
	/* Task Terminate Flag */
	PrintError ("%s(%d) Debug> Live Task is Termnated\n",__FUNCTION__,__LINE__);
	if(pAccess != NULL)
	{
		(void)STREAM_HTTP_Close (pAccess);
		if ( pAccess->psz_path != NULL )
		{
			VK_MEM_Free(pAccess->psz_path);
			pAccess->psz_path = NULL;
		}
		DD_MEM_Free(pAccess);
		pAccess = NULL;
	}
	PrintExit;
	pSstr->eLiveUpdateStatus = DI_IPTUNER_SSTR_LIVE_UPDATE_TERMINATED;
	return;
}

#if defined(DI_IPTUNER_SSTR_CONFIG_TEXTSTREAM)
static void P_STREAM_SSTR_TextStream_Task(void *pParam)
{
	STREAM_ACCESS_t *pAccess = NULL;
#ifdef SSTR_CONFIG_DISKDUMP_TEXTSTREAM
	HINT8 strBuf[512]= {0};
	HINT32 flags = O_CREAT|O_TRUNC|O_WRONLY|O_SYNC;
	HINT32 nFd = -1;
#endif
	HUINT32 nTextFragmentId = 0;
	HINT8 FourCC[5]={0};
	HINT64 nSize = 0;
	HUINT32 nBufSize = SSTR_CONST_TEXTSTREAM_DEFAULTSIZE;
	HUINT8 *pBuffer = NULL;
	ssize_t sz_Read = 0;
	SSTR_REQ_TEXT_t* pReq = NULL;
	SSTR_t* pSstr = NULL;
	HINT64 ullPrevFragStartTime[DI_IPTUNER_SSTR_CONST_MAX_STREAMS] = {-1,};
	HUINT64 ullStartTime = 0;
	UNUSED_PARAM(pParam);
	PrintEnter;
	if(g_hSstr == NULL)
	{
		PrintError("%s(%d) Error> SSTR HANDLE is NULL \n", __FUNCTION__, __LINE__);
		goto error_exit;
	}
	pSstr = __getSSTR(g_hSstr);
	if(pSstr->hCtrl == NULL)
	{
		PrintError("%s(%d) Error> SSTR CONTROLLER is NULL. Check This !!\n"
		, __FUNCTION__, __LINE__);
		goto error_exit;
	}
	pBuffer = DD_MEM_Alloc(nBufSize);
	if(pBuffer == NULL)
	{
		PrintError("%s(%d) Error> Buffer Allocation Fail\n"
		, __FUNCTION__, __LINE__);
		goto error_exit;
	}
	pAccess = DD_MEM_Alloc(sizeof(STREAM_ACCESS_t));
	if(pAccess == NULL)
	{
		PrintError("%s(%d) Error> Buffer Allocation Fail\n"
		, __FUNCTION__, __LINE__);
		goto error_exit;
	}
	VK_MEM_Memset(pAccess,0x00,sizeof(STREAM_ACCESS_t));
	pAccess->b_use_persist = TRUE;
	pSstr->eTextTaskState = DI_IPTUNER_SSTR_TEXT_TASK_STATE_STARTED;
	while(TRUE)
	{
		if(pSstr->bReqTextFlush == TRUE || g_ePreBufferingTaskChangeState >= DI_IPTUNER_SSTR_BUFFERING_TASK_STATE_KILLING)
		{
			pSstr->eTextTaskState = DI_IPTUNER_SSTR_TEXT_TASK_STATE_FLUSHING;
			while((pReq = P_STREAM_SSTR_GetReqTextStream(pSstr)) != NULL)
			{
				if(pReq != NULL)
				{
					VK_MEM_Free(pReq);
					pReq = NULL;
				}
			}
			if(g_ePreBufferingTaskChangeState >= DI_IPTUNER_SSTR_BUFFERING_TASK_STATE_KILLING)
			{
				break;
			}
			else
			{
				for(HUINT32 i = 0; i < DI_IPTUNER_SSTR_CONST_MAX_STREAMS ; i++)
				{
					ullPrevFragStartTime[i] = -1;
				}
				for(HUINT32 i = 0; i < pSstr->ulMaxText ; i++)
				{
					SSTR_CONTROLLER_SetFragment(pSstr->hCtrl,pSstr->tTextStreams[i].nIndex,0);
				}
				pSstr->eTextTaskState = DI_IPTUNER_SSTR_TEXT_TASK_STATE_STARTED;
				pSstr->bReqTextFlush = FALSE;
			}
		}
		pReq = P_STREAM_SSTR_GetReqTextStream(pSstr);
		if(pReq == NULL)
		{
			continue;
		}
		else
		{
			for(HUINT32 i = 0; i < pSstr->ulMaxText ; i++)
			{
				ssize_t nCur = 0;
				size_t sz_Try = 0;
				VK_MEM_Memset(pBuffer,0x00,nBufSize);
				VK_MEM_Memset(FourCC,0x00,sizeof(FourCC));
				if(g_ePreBufferingTaskChangeState >= DI_IPTUNER_SSTR_BUFFERING_TASK_STATE_KILLING)
				{
					break;
				}
				if(pSstr->bReqTextFlush == TRUE)
				{
					break;
				}
				if(SSTR_CONTROLLER_GetStreamType(pSstr->hCtrl,pSstr->tTextStreams[i].nIndex) != SSTR_TYPE_TEXT)
				{
					continue;
				}
				VK_SEM_Get(pSstr->ulLiveSema);
				if(SSTR_CONTROLLER_GetFragment(pSstr->hCtrl,pSstr->tTextStreams[i].nIndex,pReq->ullStartTime,&nTextFragmentId) != SSTR_ERR_OK)
				{
					VK_SEM_Release(pSstr->ulLiveSema);
					continue;
				}
				if(SSTR_CONTROLLER_GetFragmentStartTime(pSstr->hCtrl,pSstr->tTextStreams[i].nIndex,nTextFragmentId,&ullStartTime) != SSTR_ERR_OK)
				{
					VK_SEM_Release(pSstr->ulLiveSema);
					continue;
				}
				if(ullPrevFragStartTime[i] == (HINT64)ullStartTime)
				{
					VK_SEM_Release(pSstr->ulLiveSema);
					continue;
				}
				else
				{
					ullPrevFragStartTime[i] = ullStartTime;
				}
				if(	SSTR_CONTROLLER_Get_N_FragmentUrl(pSstr->hCtrl,pSstr->tTextStreams[i].nIndex,SSTR_CONTROLLER_GetTrack(pSstr->hCtrl,pSstr->tTextStreams[i].nIndex),nTextFragmentId,&pSstr->tTextUrl) != SSTR_ERR_OK)
				{
					VK_SEM_Release(pSstr->ulLiveSema);
					continue;
				}
				if(SSTR_CONTROLLER_GetTotalFragment(pSstr->hCtrl,pSstr->tTextStreams[i].nIndex) > nTextFragmentId+1)
				{
					SSTR_CONTROLLER_SetFragment(pSstr->hCtrl,pSstr->tTextStreams[i].nIndex,nTextFragmentId+1);
				}
				VK_SEM_Release(pSstr->ulLiveSema);
				if(g_ePreBufferingTaskChangeState >= DI_IPTUNER_SSTR_BUFFERING_TASK_STATE_KILLING)
				{
					break;
				}
				if(pSstr->bReqTextFlush == TRUE)
				{
					break;
				}
				if(pSstr->tTextStreams[i].bEnabled == FALSE)
				{
#ifdef SSTR_CONFIG_DUMP_TEXTSTREAM
					PrintDebug("Skip TEXT([S:%d]:StartTime[%d] = %"PRId64")\n",i,nTextFragmentId,ullStartTime);
#endif
					continue;
				}
				if(pAccess->p_sys != NULL)
				{
					STREAM_HTTP_Close(pAccess);
				}
				if(pAccess->psz_path != NULL)
				{
					DD_MEM_Free(pAccess->psz_path);
					pAccess->psz_path = NULL;
				}
				pAccess->psz_path = VK_StrDup(pSstr->tTextUrl.url);
				if( STREAM_HTTP_Open(pAccess) != DI_ERR_OK )
				{
					continue;
				}

				nSize = pAccess->info.i_size;
				if(nSize > nBufSize)
				{
					if(pBuffer != NULL)
					{
						VK_MEM_Free(pBuffer);
						pBuffer = VK_MEM_Alloc(nSize);
						if(pBuffer == NULL)
						{
							goto error_exit;
						}
						nBufSize = nSize;
					}
				}
				while(nCur < nSize)
				{
					if(g_ePreBufferingTaskChangeState >= DI_IPTUNER_SSTR_BUFFERING_TASK_STATE_KILLING)
					{
						break;
					}
					if(pSstr->bReqTextFlush == TRUE)
					{
						break;
					}
					if((nSize - nCur) > DI_IPTUNER_SSTR_CONST_RECV_BLOCKSIZE)
					{
						sz_Try = DI_IPTUNER_SSTR_CONST_RECV_BLOCKSIZE;
					}
					else
					{
						sz_Try = nSize - nCur;
					}
					sz_Read = STREAM_HTTP_Read ( pAccess, pBuffer + nCur, sz_Try);
					if( sz_Read > 0 )
					{
						nCur += sz_Read;
					}
					else if (sz_Read == 0 && pAccess->info.b_eof )
					{
						PrintDebug ("%s(%d) : Debug > End of File. and recv zero \n",__FUNCTION__,__LINE__);
						break;
					}
					else
					{
						break;
					}
				}
				if(nCur == nSize && (pSstr->bReqTextFlush != TRUE || g_ePreBufferingTaskChangeState < DI_IPTUNER_SSTR_BUFFERING_TASK_STATE_KILLING))
				{
					if(SSTR_CONTROLLER_GetSubType(pSstr->hCtrl, pSstr->tTextStreams[i].nIndex, FourCC) != SSTR_ERR_OK)
					{
						continue;
					}
#ifdef SSTR_CONFIG_DUMP_TEXTSTREAM
					PrintDebug("TEXT(%s[%d]:%"PRId64") : %s\n",FourCC,nTextFragmentId,nSize,pBuffer);
#endif
#ifdef SSTR_CONFIG_DISKDUMP_TEXTSTREAM
					VK_sprintf(strBuf,"/home/text_%d_%s_Fragment_%d",pSstr->tTextStreams[i].nIndex,FourCC,nTextFragmentId);
					nFd = open(strBuf,O_CREAT|O_TRUNC|O_WRONLY|O_SYNC,0666);
					write(nFd,pBuffer,nSize);
					close(nFd);
#endif
#if __SSTR_NOT_SUPPORTED_YET__
					if(stAdaptiveCallback.cbTextStreamCallback != NULL)
					{
						SSTR_CONTROLLER_GetFragmentStartTime(pSstr->hCtrl,nIndex,nTextFragmentId, &ullTextStartTime);
						SSTR_CONTROLLER_GetFragmentEndTime(pSstr->hCtrl,nIndex,nTextFragmentId, &ullTextEndTime);
						stAdaptiveCallback.cbTextStreamCallback(nTunerIndex,P_STREAM_SSTR_ConvertSubtype(FourCC),ullTextStartTime,ullTextEndTime,pBuffer,nSize);
					}
#endif
				}
			}
			if(pReq != NULL)
			{
				VK_MEM_Free(pReq);
				pReq = NULL;
			}
		}
		VK_TASK_Sleep(50);
	}
error_exit:
	if(pAccess->p_sys != NULL)
	{
		STREAM_HTTP_Close(pAccess);
	}
	if(pAccess->psz_path != NULL)
	{
		DD_MEM_Free(pAccess->psz_path);
	}
	if(pAccess != NULL)
	{
		DD_MEM_Free(pAccess);
	}
	if(pBuffer != NULL)
	{
		DD_MEM_Free(pBuffer);
	}
	if(pReq != NULL)
	{
		VK_MEM_Free(pReq);
		pReq = NULL;
	}
	if(pSstr != NULL)
	{
		pSstr->eTextTaskState = DI_IPTUNER_SSTR_TEXT_TASK_STATE_DONE;
	}
	PrintExit;
	return;
}
#endif

HUINT32 DI_STREAM_SSTR_GetBufferingState()
{
	return g_ePreBufferingTaskState;
}
void DI_STREAM_SSTR_SetBufferingState(HUINT32 nState)
{
	g_ePreBufferingTaskState = nState;
	return;
}
void DI_STREAM_SSTR_StopBufferingTask()
{
	g_ePreBufferingTaskChangeState = DI_IPTUNER_SSTR_BUFFERING_TASK_STATE_KILLING;
	return;
}

int DI_STREAM_SSTR_CreateLiveUpdateTask ()
{
	SSTR_t* pSstr;
	VK_ERR_CODE nVKRet = VK_OK;
//	HINT8 task_name[128]={0};
	PrintEnter;
	if(g_hSstr == NULL)
	{
		PrintError("%s(%d) Error> SSTR HANDLE is NULL \n", __FUNCTION__, __LINE__);
		return -1;
	}
	pSstr = __getSSTR(g_hSstr);
	if(pSstr->hCtrl == NULL)
	{
		PrintError("%s(%d) Error> SSTR CONTROLLER is NULL. Check This !!\n"
		, __FUNCTION__, __LINE__);
		return -1;
	}
	if(pSstr->bLiveUpdateTaskInit == TRUE)
	{
		return 0;
	}
	if(SSTR_CONTROLLER_IsLive (pSstr->hCtrl) != TRUE)
	{
		return -1;
	}
//	VK_snprintf((char*)task_name,VK_strlen("SSTR_LiveUpdateTask_")+10, "SSTR_LiveUpdateTask_%p", (void*)pHSession);
	nVKRet = VK_TASK_Create ( P_STREAM_SSTR_LiveUpdate_Task
									, PLAYBACK_LOAD_TASK_PRIORITY	// Buffering TASK Priority 는 Playback 만큼 또는 그 이상 높아야 한다.
									, TUNER_STREAM_STACK_SIZE
									, (char*)"SSTR_LiveUpdateTask"
									, NULL
									, &pSstr->ulLiveUpdateTaskId
									, 0);
	if ( VK_OK != nVKRet )
	{
		PrintError ("%s (%d) Error> STREAM Tuner task creation fail err = 0x%x\n",__FUNCTION__,__LINE__, nVKRet);
		return -1;
	}
	pSstr->bLiveUpdateTaskInit = TRUE;
	pSstr->eLiveUpdateStatus = DI_IPTUNER_SSTR_LIVE_UPDATE_READY;
	VK_TASK_Start(pSstr->ulLiveUpdateTaskId);
	PrintExit;
	return 0;
}

int DI_STREAM_SSTR_DestroyLiveUpdateTask ()
{
	SSTR_t* pSstr;
	PrintEnter;
	if(g_hSstr == NULL)
	{
		PrintError("%s(%d) Error> SSTR HANDLE is NULL \n", __FUNCTION__, __LINE__);
		return -1;
	}
	pSstr = __getSSTR(g_hSstr);
	if(pSstr->hCtrl == NULL)
	{
		PrintError("%s(%d) Error> SSTR CONTROLLER is NULL. Check This !!\n"
		, __FUNCTION__, __LINE__);
		return -1;
	}
	if(SSTR_CONTROLLER_IsLive (pSstr->hCtrl) != TRUE)
	{
		return -1;
	}
	if(pSstr->bLiveUpdateTaskInit == FALSE)
	{
		return 0;
	}
	while(pSstr->eLiveUpdateStatus != DI_IPTUNER_SSTR_LIVE_UPDATE_TERMINATED)
	{
		VK_TASK_Sleep(100);
	}
	(void) VK_TASK_Destroy (pSstr->ulLiveUpdateTaskId);
	pSstr->bLiveUpdateTaskInit = FALSE;
	PrintExit;
	return 0;
}

#if defined(DI_IPTUNER_SSTR_CONFIG_TEXTSTREAM)
int DI_STREAM_SSTR_CreateTextStreamTask ()
{
	SSTR_t* pSstr;
	VK_ERR_CODE nVKRet = VK_OK;
//	HINT8 task_name[128]={0};
	PrintEnter;
	if(g_hSstr == NULL)
	{
		PrintError("%s(%d) Error> SSTR HANDLE is NULL \n", __FUNCTION__, __LINE__);
		return -1;
	}
	pSstr = __getSSTR(g_hSstr);
	if(pSstr->hCtrl == NULL)
	{
		PrintError("%s(%d) Error> SSTR CONTROLLER is NULL. Check This !!\n"
		, __FUNCTION__, __LINE__);
		return -1;
	}
	if(pSstr->bTextStreamTaskInit == TRUE)
	{
		return 0;
	}
	if(pSstr->ulMaxText == 0)
	{
		return 0;
	}
//	VK_snprintf((char*)task_name,VK_strlen("SSTR_TextStreamTask_")+10, "SSTR_TextStreamTask_%p", (void*)pHSession);
	nVKRet = VK_TASK_Create ( P_STREAM_SSTR_TextStream_Task
									, PLAYBACK_LOAD_TASK_PRIORITY	// Buffering TASK Priority 는 Playback 만큼 또는 그 이상 높아야 한다.
									, TUNER_STREAM_STACK_SIZE
									, (char*)"SSTR_TextStreamTask"
									, NULL
									, &pSstr->ulTextStreamTaskId
									, 0);
	if ( VK_OK != nVKRet )
	{
		PrintError ("%s (%d) Error> STREAM Tuner task creation fail err = 0x%x\n",__FUNCTION__,__LINE__, nVKRet);
		return -1;
	}
	pSstr->bTextStreamTaskInit = TRUE;
	VK_TASK_Start(pSstr->ulTextStreamTaskId);
	PrintExit;
	return 0;
}

int DI_STREAM_SSTR_DestroyTextStreamTask ()
{
	SSTR_t* pSstr;
	PrintEnter;
	if(g_hSstr == NULL)
	{
		PrintError("%s(%d) Error> SSTR HANDLE is NULL \n", __FUNCTION__, __LINE__);
		return -1;
	}
	pSstr = __getSSTR(g_hSstr);
	if(pSstr->hCtrl == NULL)
	{
		PrintError("%s(%d) Error> SSTR CONTROLLER is NULL. Check This !!\n"
		, __FUNCTION__, __LINE__);
		return -1;
	}
	if(pSstr->bTextStreamTaskInit == FALSE || pSstr->ulMaxText == 0)
	{
		return 0;
	}
	while(pSstr->eTextTaskState != DI_IPTUNER_SSTR_TEXT_TASK_STATE_DONE)
	{
		VK_TASK_Sleep(100);
	}
	(void) VK_TASK_Destroy (pSstr->ulTextStreamTaskId);
	pSstr->bTextStreamTaskInit = FALSE;
	PrintExit;
	return 0;
}
#endif

SSTR_ERR DI_STREAM_SSTR_SetSettings(SSTR_HANDLE hSstr, DI_IPTUNER_AdaptiveSettings_t *pSettings)
{
	SSTR_t* pSstr = NULL;
	SSTR_HEURISTICS_Config_t tHeurConfig;
	SSTR_HEURISTICS_t *pHeuristics = NULL;
	SSTR_BUFFERMGR_t *pBufferMgr = NULL;
	DI_IPTUNER_ADAPTIVE_CONTROL_e eControl = DI_IPTUNER_ADAPTIVE_NONE;
	PrintEnter;
	if(hSstr == NULL)
	{
		PrintError("%s(%d) Err> SSTR HANDLE is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return SSTR_ERR_FAIL;
	}
	pSstr = __getSSTR(hSstr);
	pBufferMgr = SSTR_CONTROLLER_GetBuffer(pSstr->hCtrl);
	if(pBufferMgr == NULL)
	{
		PrintError ("%s(%d) Fail! GetBufferMgr was failed. \n",__FUNCTION__,__LINE__);
		PrintExit;
		return SSTR_ERR_FAIL;
	}
	pHeuristics = SSTR_CONTROLLER_GetHeuristics(pSstr->hCtrl);
	if(pHeuristics == NULL)
	{
		PrintError ("%s(%d) Fail! GetHeuristicsHandle was failed. \n",__FUNCTION__,__LINE__);
		PrintExit;
		return SSTR_ERR_FAIL;
	}
	if(pSettings != NULL)
	{
		eControl = pSettings->eControl;
	}
	else
	{
		PrintDebug ("%s(%d) Warning> Adaptive Setting is NULL. Change to Initialize\n",__FUNCTION__,__LINE__);
		eControl = DI_IPTUNER_ADAPTIVE_INIT;
	}
	switch(eControl)
	{
		case DI_IPTUNER_ADAPTIVE_NONE :
			break;
		case DI_IPTUNER_ADAPTIVE_INIT : /* Default Setting */
			if( pHeuristics->bInit != TRUE )
			{
				if(SSTR_HEURISTICS_GetDefaultConfig(pHeuristics, SSTR_HEUR_CONFIG_PRESET_SSTR, &tHeurConfig))
				{
					PrintError ("%s(%d) Fail! GetDefaultConfig was failed. \n",__FUNCTION__,__LINE__);
					PrintExit;
					return SSTR_ERR_FAIL;
				}
				tHeurConfig.hCtrl = pSstr->hCtrl;
				tHeurConfig.hBufferMgr = pBufferMgr;
				tHeurConfig.cbCurBitrate = SSTR_CONTROLLER_GetCurBitrate;
				tHeurConfig.cbCurDuration = SSTR_CONTROLLER_GetCurFragmentDurationForHeur;
				tHeurConfig.cbIsProtected = SSTR_CONTROLLER_IsProtected;
				tHeurConfig.cbGearUp = SSTR_CONTROLLER_BitrateUp;
				tHeurConfig.cbGearDown = SSTR_CONTROLLER_BitrateDown;
				tHeurConfig.cbGearSet = SSTR_CONTROLLER_SetClosestBitrate;
				tHeurConfig.cbGearLowest = SSTR_CONTROLLER_SetLowestBitrate;
				tHeurConfig.cbGearHighest = SSTR_CONTROLLER_SetHighestBitrate;
				tHeurConfig.eStartup = DI_IPTUNER_ADAPTIVE_STARTUP_LOWEST;
				tHeurConfig.eDecision= DI_IPTUNER_ADAPTIVE_DECISION_AUTO;

				if(SSTR_HEURISTICS_Init(pHeuristics, &tHeurConfig))
				{
					PrintError ("%s(%d) Fail! Heuristics Init was failed. \n",__FUNCTION__,__LINE__);
					PrintExit;
					return SSTR_ERR_FAIL;
				}
			}
			break;
		case DI_IPTUNER_ADAPTIVE_SSTR_ALL :
			{
				SSTR_HEURISTICS_GetConfig(pHeuristics, &tHeurConfig);
				tHeurConfig.eStartup = pSettings->eStartup;
				tHeurConfig.eDecision= pSettings->eDecision;

				SSTR_CONTROLLER_SetStreamByArrayIndex(pSstr->hCtrl , SSTR_TYPE_AUDIO, pSettings->ulAudioStreamIndex);
				SSTR_CONTROLLER_SetStreamByArrayIndex(pSstr->hCtrl , SSTR_TYPE_VIDEO, pSettings->ulVideoStreamIndex);
				SSTR_CONTROLLER_SetTrackByIndex(pSstr->hCtrl , SSTR_TYPE_VIDEO, pSettings->ulVideoTrackIndex);
				SSTR_CONTROLLER_SetTrackByIndex(pSstr->hCtrl , SSTR_TYPE_AUDIO, pSettings->ulAudioTrackIndex);

				if(SSTR_HEURISTICS_SetConfig(pHeuristics, &tHeurConfig))
				{
					PrintError ("%s(%d) Fail! Heuristics Init was failed. \n",__FUNCTION__,__LINE__);
					PrintExit;
					return SSTR_ERR_FAIL;
				}
			}
			break;
		case DI_IPTUNER_ADAPTIVE_SSTR_VIDEO_STREAM :
			SSTR_CONTROLLER_SetStreamByArrayIndex(pSstr->hCtrl , SSTR_TYPE_VIDEO, pSettings->ulVideoStreamIndex);
			break;
		case DI_IPTUNER_ADAPTIVE_SSTR_AUDIO_STREAM :
			SSTR_CONTROLLER_SetStreamByArrayIndex(pSstr->hCtrl , SSTR_TYPE_AUDIO, pSettings->ulAudioStreamIndex);
			break;
		case DI_IPTUNER_ADAPTIVE_SSTR_UNSET_AUDIO_STREAM :
			break;
		case DI_IPTUNER_ADAPTIVE_SSTR_VIDEO_TRACK :
			SSTR_CONTROLLER_SetTrackByIndex(pSstr->hCtrl , SSTR_TYPE_VIDEO, pSettings->ulVideoTrackIndex);
			break;
		case DI_IPTUNER_ADAPTIVE_SSTR_AUDIO_TRACK :
			SSTR_CONTROLLER_SetTrackByIndex(pSstr->hCtrl , SSTR_TYPE_AUDIO, pSettings->ulAudioTrackIndex);
			break;
		case DI_IPTUNER_ADAPTIVE_SSTR_TEXT_STREAM :
		case DI_IPTUNER_ADAPTIVE_SSTR_TEXT_TRACK :
		default :
			PrintError ("%s(%d) Not Supported Adaptive Control (%d) \n",__FUNCTION__,__LINE__,pSettings->eControl);
			PrintExit;
			return SSTR_ERR_FAIL;
	}
	PrintExit;
	return SSTR_ERR_OK;
}

SSTR_ERR DI_STREAM_SSTR_GetSettings(SSTR_HANDLE hSstr, DI_IPTUNER_AdaptiveSettings_t *pSettings)
{
	SSTR_t* pSstr = NULL;
	SSTR_HEURISTICS_Config_t tHeurConfig;
	SSTR_HEURISTICS_t *pHeuristics = NULL;
	PrintEnter;
	if(hSstr == NULL)
	{
		PrintError("%s(%d) Err> SSTR HANDLE is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return SSTR_ERR_FAIL;
	}
	pSstr = __getSSTR(hSstr);
	pHeuristics = SSTR_CONTROLLER_GetHeuristics(pSstr->hCtrl);
	if(pHeuristics == NULL)
	{
		PrintError ("%s(%d) Fail! GetHeuristicsHandle was failed. \n",__FUNCTION__,__LINE__);
		PrintExit;
		return SSTR_ERR_FAIL;
	}
#if 1
	SSTR_HEURISTICS_GetConfig(pHeuristics, &tHeurConfig);
	pSettings->eStartup = tHeurConfig.eStartup;
	pSettings->eDecision = tHeurConfig.eDecision;
	pSettings->ulVideoStreamIndex = SSTR_CONTROLLER_GetCurAVStream(pSstr->hCtrl , SSTR_TYPE_VIDEO);
	pSettings->ulAudioStreamIndex = SSTR_CONTROLLER_GetCurAVStream(pSstr->hCtrl , SSTR_TYPE_AUDIO);
	pSettings->ulVideoTrackIndex = SSTR_CONTROLLER_GetCurAVTrack(pSstr->hCtrl , SSTR_TYPE_VIDEO);
	pSettings->ulAudioTrackIndex = SSTR_CONTROLLER_GetCurAVTrack(pSstr->hCtrl , SSTR_TYPE_AUDIO);
#else
	switch(pSettings->eControl)
	{
		case DI_IPTUNER_ADAPTIVE_NONE :
			if(SSTR_HEURISTICS_GetDefaultConfig(pHeuristics, SSTR_HEUR_CONFIG_PRESET_SSTR, &tHeurConfig))
			{
				PrintError ("%s(%d) Fail! GetDefaultConfig was failed. \n",__FUNCTION__,__LINE__);
				PrintExit;
				return SSTR_ERR_FAIL;
			}
			pSettings->eStartup = tHeurConfig.eStartup;
			pSettings->eDecision = tHeurConfig.eDecision;
			pSettings->ulVideoStreamIndex = SSTR_CONTROLLER_GetCurAVStream(pSstr->hCtrl , SSTR_TYPE_VIDEO);
			pSettings->ulAudioStreamIndex = SSTR_CONTROLLER_GetCurAVStream(pSstr->hCtrl , SSTR_TYPE_AUDIO);
			pSettings->ulVideoTrackIndex = SSTR_CONTROLLER_GetCurAVTrack(pSstr->hCtrl , SSTR_TYPE_VIDEO);
			pSettings->ulAudioTrackIndex = SSTR_CONTROLLER_GetCurAVTrack(pSstr->hCtrl , SSTR_TYPE_AUDIO);
			break;
		case DI_IPTUNER_ADAPTIVE_SSTR_ALL :
			SSTR_HEURISTICS_GetConfig(pHeuristics, &tHeurConfig);
			pSettings->eStartup = tHeurConfig.eStartup;
			pSettings->eDecision = tHeurConfig.eDecision;
			pSettings->ulVideoStreamIndex = SSTR_CONTROLLER_GetCurAVStream(pSstr->hCtrl , SSTR_TYPE_VIDEO);
			pSettings->ulAudioStreamIndex = SSTR_CONTROLLER_GetCurAVStream(pSstr->hCtrl , SSTR_TYPE_AUDIO);
			pSettings->ulVideoTrackIndex = SSTR_CONTROLLER_GetCurAVTrack(pSstr->hCtrl , SSTR_TYPE_VIDEO);
			pSettings->ulAudioTrackIndex = SSTR_CONTROLLER_GetCurAVTrack(pSstr->hCtrl , SSTR_TYPE_AUDIO);
			break;
		case DI_IPTUNER_ADAPTIVE_SSTR_VIDEO_STREAM :
			pSettings->ulVideoStreamIndex = SSTR_CONTROLLER_GetCurAVStream(pSstr->hCtrl , SSTR_TYPE_VIDEO);
		case DI_IPTUNER_ADAPTIVE_SSTR_AUDIO_STREAM :
			pSettings->ulAudioStreamIndex = SSTR_CONTROLLER_GetCurAVStream(pSstr->hCtrl , SSTR_TYPE_AUDIO);
		case DI_IPTUNER_ADAPTIVE_SSTR_VIDEO_TRACK :
			pSettings->ulVideoTrackIndex = SSTR_CONTROLLER_GetCurAVTrack(pSstr->hCtrl , SSTR_TYPE_VIDEO);
		case DI_IPTUNER_ADAPTIVE_SSTR_AUDIO_TRACK :
			pSettings->ulAudioTrackIndex = SSTR_CONTROLLER_GetCurAVTrack(pSstr->hCtrl , SSTR_TYPE_AUDIO);
			break;
		case DI_IPTUNER_ADAPTIVE_SSTR_TEXT_STREAM :
		case DI_IPTUNER_ADAPTIVE_SSTR_TEXT_TRACK :
		default :
			PrintError ("%s(%d) UNKNOWN ADAPTIVE CONTROL (%d) \n",__FUNCTION__,__LINE__,pSettings->eControl);
			PrintExit;
			return SSTR_ERR_FAIL;

	}
#endif
	PrintExit;
	return SSTR_ERR_OK;
}

SSTR_ERR	DI_STREAM_SSTR_GetMediaInfo (DI_MEDIA_INFO_t *pstMediaInfo)
{
	HINT32 nVideoIndex = 0;
	HINT32 nAudioIndex = 0;
//	SSTR_CONTROLLER_t* pController = NULL;
//	sstrMedia_t *pMedia = NULL;
	SSTR_t *pSstr = NULL;
//	HUINT16 usMaxWidth = 0;
//	HUINT16 usMaxHeight = 0;
	PrintEnter;
	//P_SSTR_CONTROLLER_Lock(hCtrl);
	if(g_hSstr == NULL )
	{
		PrintError("%s(%d) : Error> SSTR HANDLE is NULL\n",__FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}
	pSstr = __getSSTR(g_hSstr);
	if(pSstr->hCtrl == NULL)
	{
		PrintError("%s(%d) Error> SSTR CONTROLLER is NULL. Check This !!\n"
		, __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}
	pstMediaInfo->max_bitrate = SSTR_CONTROLLER_GetMaxBitrate(pSstr->hCtrl);
	pstMediaInfo->duration	= SSTR_CONTROLLER_GetMediaDuration(pSstr->hCtrl) / 10000; /* convert to Millisecs */
	if(pstMediaInfo->duration == 0 || SSTR_CONTROLLER_IsLive(pSstr->hCtrl))
	{
		pstMediaInfo->duration = 0xFFFFFFFF;
	}
	pstMediaInfo->nprograms	= 1;  /* Check */
	pstMediaInfo->ntracks = SSTR_CONTROLLER_GetTotalStreams(pSstr->hCtrl);
	pstMediaInfo->stream_type = DI_MEDIA_MPEG_TYPE_MP4FRAGMENT;
	pstMediaInfo->pcr_pid	= 0x1fff; /* NULL_PID;*/

	for(HUINT32 i = 0 ; i < pstMediaInfo->ntracks ; i++)
	{
		HINT8* pLang = NULL;
		HINT32 nLangLen = 0;
		switch(SSTR_CONTROLLER_GetStreamType(pSstr->hCtrl, i))
		{
			case SSTR_TYPE_VIDEO :
				pstMediaInfo->stVideoTrack[nVideoIndex].video_codec = P_STREAM_SSTR_FourCCToDICodec(SSTR_CONTROLLER_GetTrackFourCC(pSstr->hCtrl, i, 0));
				if(pstMediaInfo->stVideoTrack[nVideoIndex].video_codec == 0)
				{
					pstMediaInfo->stVideoTrack[nVideoIndex].video_codec = P_STREAM_SSTR_FourCCToDICodec(SSTR_CONTROLLER_GetStreamSubtype(pSstr->hCtrl, i));
				}
				if(pstMediaInfo->stVideoTrack[nVideoIndex].video_codec != 0)
				{
					// pstMediaInfo->stVideoTrack[nVideoIndex].video_stream_id = SSTR_CONST_VIDEO_STREAM_ID + i;
					pstMediaInfo->stVideoTrack[nVideoIndex].video_stream_id = SSTR_CONST_VIDEO_STREAM_ID;
					pstMediaInfo->stVideoTrack[nVideoIndex].video_pes_id = DI_MEDIA_VIDEO_PES_ID;
					nVideoIndex++;
				}
				break;
			case SSTR_TYPE_AUDIO :
				pLang = SSTR_CONTROLLER_GetStreamLang(pSstr->hCtrl, i);
				nLangLen = SSTR_CONTROLLER_GetStreamLangLen(pSstr->hCtrl, i);
				if(pLang != NULL && nLangLen > 0)
				{
					nLangLen = nLangLen >= 256 ? 256 : nLangLen;
					VK_strncpy(pstMediaInfo->stAudioTrack[nAudioIndex].uacAudioLang,pLang,nLangLen);
				}
				pstMediaInfo->stAudioTrack[nAudioIndex].audio_codec =  P_STREAM_SSTR_FourCCToDICodec(SSTR_CONTROLLER_GetTrackFourCC(pSstr->hCtrl, i, 0));
				if(pstMediaInfo->stAudioTrack[nAudioIndex].audio_codec == 0)
				{
					pstMediaInfo->stAudioTrack[nAudioIndex].audio_codec = P_STREAM_SSTR_FourCCToDICodec(SSTR_CONTROLLER_GetStreamSubtype(pSstr->hCtrl, i));
				}
				if(pstMediaInfo->stAudioTrack[nAudioIndex].audio_codec != 0)
				{
					pstMediaInfo->stAudioTrack[nAudioIndex].audio_stream_id = SSTR_CONST_AUDIO_STREAM_ID;
					pstMediaInfo->stAudioTrack[nAudioIndex].audio_pes_id = DI_MEDIA_AUDIO_PES_ID;
					nAudioIndex++;
				}
				break;
			default :
				break;
		}
	}
	pstMediaInfo->ucTotalAudioTrack = nAudioIndex;
	pstMediaInfo->ucTotalVideoTrack = nVideoIndex;
	pstMediaInfo->ntracks = nVideoIndex + nAudioIndex;
#if 1
	pstMediaInfo->nWrmHeaderLen = SSTR_CONTROLLER_GetDecodedProtectionHeaderLen(pSstr->hCtrl);
	pstMediaInfo->pucWrmHeader = SSTR_CONTROLLER_GetDecodedProtectionHeader(pSstr->hCtrl);
//	PrintDebug ("pstMediaInfo->nWrmHeaderLen %d\n",pstMediaInfo->nWrmHeaderLen);
	if(pstMediaInfo->nWrmHeaderLen > 0 && pstMediaInfo->pucWrmHeader != NULL)
	{
#if 0
		pstMediaInfo->nWrmHeaderLen = nProtectionHeaderLen+1;
		pstMediaInfo->pucWrmHeader = DD_MEM_Alloc(nProtectionHeaderLen+1);
		if(pstMediaInfo->pucWrmHeader != NULL)
		{
			VK_MEM_Memset(pstMediaInfo->pucWrmHeader,0x00,nProtectionHeaderLen+1);
			VK_MEM_Memcpy(pstMediaInfo->pucWrmHeader, pProtectionHeader, nProtectionHeaderLen);
			pstMediaInfo->nWrmHeaderLen = nProtectionHeaderLen;
			pstMediaInfo->is_drm = TRUE;
		}
#else
		pstMediaInfo->is_drm = TRUE;
		pstMediaInfo->eDrmType = DI_MEDIA_DRM_TYPE_PLAYREADY;
#endif
	}
#endif

	pstMediaInfo->usVideoWidth = SSTR_CONTROLLER_GetMaxWidth(pSstr->hCtrl);
	pstMediaInfo->usVideoHeight = SSTR_CONTROLLER_GetMaxHeight(pSstr->hCtrl);
	pstMediaInfo->audio_codec = pstMediaInfo->stAudioTrack[0].audio_codec;
	pstMediaInfo->video_codec = pstMediaInfo->stVideoTrack[0].video_codec;
	pstMediaInfo->audiodecodertype = DI_MEDIA_AUDIO_DECODER_MAIN;
	pstMediaInfo->videodecodertype = DI_MEDIA_VIDEO_DECODER_MAIN;
	pstMediaInfo->audio_stream_id = pstMediaInfo->stAudioTrack[0].audio_stream_id;
	pstMediaInfo->video_stream_id = pstMediaInfo->stVideoTrack[0].video_stream_id;
	pstMediaInfo->audio_pes_id = pstMediaInfo->stAudioTrack[0].audio_pes_id;
	pstMediaInfo->video_pes_id = pstMediaInfo->stVideoTrack[0].video_pes_id;
	pstMediaInfo->support_seek = 1;
	pstMediaInfo->support_trick = 0; /* not supported */
	PrintExit;
	return SSTR_ERR_OK;
}

int DI_STREAM_SSTR_OpenController (HINT8 *pUrl)
{
	HINT64 nContentLength = 0;
	HINT8 *pMetaBuff = NULL;
	SSTR_t *pSstr = NULL;
	HINT32 nRet = DI_ERR_OK;
	HINT8 *pBaseUrl=NULL;
	HUINT32 unBufferSize = 0;
	HINT8 sem_name[128]={0};
	HUINT32 nNetworkRetryCount = 0;
	HUINT32 unTextStreams = 0;
	STREAM_ACCESS_t *pAccess = NULL;
	PrintEnter;
	if(pUrl == NULL)
	{
		PrintError("%s(%d) : Error> invalid pUrl\n",__FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}
	if(g_hSstr != NULL)
	{
		PrintError("%s(%d) : Error> SSTR HANDLE is already opened\n",__FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}
	pSstr = (SSTR_t*)DD_MEM_Alloc(sizeof(SSTR_t));
	if(pSstr == NULL)
	{
		PrintError ("%s(%d) Error> Memory Allocation Fail !!\n",__FUNCTION__,__LINE__);
		PrintExit;
		return DI_ERR_ERROR;
	}
	VK_MEM_Memset(pSstr,0x00,sizeof(SSTR_t));
	pAccess = (STREAM_ACCESS_t*)DD_MEM_Alloc(sizeof(STREAM_ACCESS_t));
	if(pAccess == NULL)
	{
		PrintError ("%s(%d) Error> Memory Allocation Fail !!\n",__FUNCTION__,__LINE__);
		if(pSstr != NULL)
		{
			DD_MEM_Free(pSstr);
		}
		PrintExit;
		return DI_ERR_ERROR;
	}
	VK_MEM_Memset(pAccess,0x00,sizeof(STREAM_ACCESS_t));
	pAccess->b_use_persist = TRUE;

	P_STREAM_SSTR_TimerStart(&pSstr->stDInfo);
	pSstr->stDInfo.ulStartingTime = pSstr->stDInfo.ulStartTime;

	while((nContentLength = P_STREAM_SSTR_GetManifest(pAccess,pUrl,&pMetaBuff,&pBaseUrl)) <= 0)
	{
		nNetworkRetryCount++;
		STREAM_HTTP_Close(pAccess);
		PrintError ("%s(%d) Error> Get Manifest Fail Retry (%d)!!\n",__FUNCTION__,__LINE__,nNetworkRetryCount);
		if(nNetworkRetryCount > DI_IPTUNER_SSTR_CONST_RETRY_COUNT_ON_NET_ERROR)
		{
			PrintError("%s(%d) Manifest Download Fail.\n",__FUNCTION__,__LINE__);
			if(pSstr != NULL)
			{
				DD_MEM_Free(pSstr);
			}
			if(pAccess != NULL)
			{
				if ( pAccess->psz_path != NULL )
				{
					VK_MEM_Free(pAccess->psz_path);
					pAccess->psz_path = NULL;
				}
				DD_MEM_Free(pAccess);
			}
			PrintExit;
			return DI_ERR_ERROR;
		}
	}
	pSstr->hCtrl = SSTR_CONTROLLER_Open(pMetaBuff,nContentLength);
	if( pSstr->hCtrl == NULL)
	{
		PrintError ("%s(%d) Error> Manifest Open Fail !!\n",__FUNCTION__,__LINE__);
		if(pSstr != NULL)
		{
			DD_MEM_Free(pSstr);
		}
		if(pMetaBuff != NULL)
		{
			VK_MEM_Free(pMetaBuff);
		}
		if(pBaseUrl != NULL)
		{
			VK_MEM_Free(pBaseUrl);
		}
		if(pAccess != NULL)
		{
			(void)STREAM_HTTP_Close (pAccess);
			if ( pAccess->psz_path != NULL )
			{
				VK_MEM_Free(pAccess->psz_path);
				pAccess->psz_path = NULL;
			}
			DD_MEM_Free(pAccess);
		}
		PrintExit;
		return DI_ERR_ERROR;
	}
	nRet = SSTR_CONTROLLER_SetBaseUrl(pSstr->hCtrl, pBaseUrl);
	if(pMetaBuff != NULL)
	{
		VK_MEM_Free(pMetaBuff);
	}
	if(pBaseUrl != NULL)
	{
		VK_MEM_Free(pBaseUrl);
	}
	for(HINT32 i = 0 ; i < SSTR_CONTROLLER_GetTotalStreams(pSstr->hCtrl) ; i++)
	{
		switch(SSTR_CONTROLLER_GetStreamType(pSstr->hCtrl, i))
		{
			/*
			case SSTR_TYPE_VIDEO :
				if(unVideoStreams < DI_IPTUNER_SSTR_CONST_MAX_STREAMS)
				{
					pSstr->tVideoStreams[unVideoStreams].bEnabled = FALSE;
					pSstr->tVideoStreams[unVideoStreams].nIndex = i;
					unVideoStreams++;
				}
				break;*/
			case SSTR_TYPE_TEXT :
				if(unTextStreams < DI_IPTUNER_SSTR_CONST_MAX_STREAMS)
				{
					pSstr->tTextStreams[unTextStreams].bEnabled = FALSE;
					pSstr->tTextStreams[unTextStreams].nIndex = i;
					unTextStreams++;
				}
				break;
			default:
				break;
		}
	}

//	pSstr->ulMaxVideo = unVideoStreams;
	pSstr->ulMaxText = unTextStreams;

#if 1 /* hard coding for Default Setting by ykkim */
	pSstr->eAVType = SSTR_CONTROLLER_SetDefaultAVStream(pSstr->hCtrl);
	if( pSstr->eAVType == SSTR_CONTENTS_NONE)
	{
		goto _openctrl_error;
	}
	if(pSstr->eAVType == SSTR_CONTENTS_ALL
	|| pSstr->eAVType == SSTR_CONTENTS_AUDIO)
	{
		if(SSTR_CONTROLLER_SetTrackByIndex(pSstr->hCtrl , SSTR_TYPE_AUDIO, 0))
		{
			PrintError ("%s(%d) Fail! Default Setting(no audio)\n",__FUNCTION__,__LINE__);
			goto _openctrl_error;
		}
		pSstr->ulCurAudio = 0;
	}
	if(pSstr->eAVType == SSTR_CONTENTS_ALL
	|| pSstr->eAVType == SSTR_CONTENTS_VIDEO)
	{
		if(SSTR_CONTROLLER_SetTrackByIndex(pSstr->hCtrl , SSTR_TYPE_VIDEO, 0))
		{
			PrintError ("%s(%d) Fail! Default Setting(no video)\n",__FUNCTION__,__LINE__);
			goto _openctrl_error;
		}
	}
//	pSstr->tTextStreams[0].bEnabled = TRUE; /* For Test */
#else
	if(SSTR_CONTROLLER_SetStreamByIndex(pSstr->hCtrl , SSTR_TYPE_VIDEO, 0)
	|| SSTR_CONTROLLER_SetTrackByIndex(pSstr->hCtrl , SSTR_TYPE_VIDEO, 0))
	{
		PrintError ("%s(%d) Fail! Default Setting(no video)\n",__FUNCTION__,__LINE__);
		goto _openctrl_error;
	}
	if(SSTR_CONTROLLER_SetStreamByIndex(pSstr->hCtrl , SSTR_TYPE_AUDIO, 0)
	|| SSTR_CONTROLLER_SetTrackByIndex(pSstr->hCtrl , SSTR_TYPE_AUDIO, 0))
	{
		PrintError ("%s(%d) Fail! Default Setting(no audio)\n",__FUNCTION__,__LINE__);
		goto _openctrl_error;
	}
//	pSstr->tVideoStreams[0].bEnabled = TRUE;
	pSstr->tAudioStreams[0].bEnabled = TRUE;
#endif

	pSstr->ulBuffSize = SSTR_CONTROLLER_GetMaxBitrate(pSstr->hCtrl);
	if( pSstr->ulBuffSize <= 0 )
	{
		pSstr->ulBuffSize = DI_IPTUNER_SSTR_CONST_DEFAULT_FRAGBUFF;
	}
	else
	{
		pSstr->ulBuffSize = pSstr->ulBuffSize / 8 * (SSTR_CONST_DEFAULT_DURATION + SSTR_CONST_PADDING );
		if( pSstr->ulBuffSize <= 0 )
		{
			pSstr->ulBuffSize = DI_IPTUNER_SSTR_CONST_DEFAULT_FRAGBUFF;
		}
	}
	PrintError("%s(%d) : Debug> Fragment Buffer : %u\n",__FUNCTION__,__LINE__,pSstr->ulBuffSize );
	pSstr->pFragBuff = DD_MEM_Alloc(pSstr->ulBuffSize);
	if(pSstr->pFragBuff == NULL)
	{
		PrintError("%s(%d) : Error> Fragment Buffer Allocation Fail\n",__FUNCTION__,__LINE__);
		goto _openctrl_error;
	}
	VK_MEM_Memset(&pSstr->mp4Box,0x00,sizeof(mem_file));
	pSstr->mp4Box.buf = VK_MEM_Alloc(DI_IPTUNER_SSTR_CONST_MP4BOX_BUFFERSIZE);
	if(pSstr->mp4Box.buf == NULL)
	{
		PrintError("%s(%d) Error> mp4Box Buffer Open Fail\n",__FUNCTION__,__LINE__);
		goto _openctrl_error;
	}
	VK_MEM_Memset(pSstr->mp4Box.buf,0x00,DI_IPTUNER_SSTR_CONST_MP4BOX_BUFFERSIZE);
	pSstr->mp4Box.buf_len = DI_IPTUNER_SSTR_CONST_MP4BOX_BUFFERSIZE;
	VK_snprintf((char*)sem_name,VK_strlen("SSTR_SeekSema_")+10, "SSTR_SeekSema_%p", (void*)pSstr);
	nRet = VK_SEM_Create((unsigned long*)&pSstr->ulSeekSema, (const char*)sem_name, DEFAULT_SUSPEND_TYPE);
	if( nRet != VK_OK )
	{
		PrintError("%s(%d) Error(0x%x) \n\r",__FUNCTION__,__LINE__, nRet);
		goto _openctrl_error;
	}
	VK_snprintf((char*)sem_name,VK_strlen("SSTR_LiveSema_")+10, "SSTR_LiveSema_%p", (void*)pSstr);
	nRet = VK_SEM_Create((unsigned long*)&pSstr->ulLiveSema, (const char*)sem_name, DEFAULT_SUSPEND_TYPE);
	if( nRet != VK_OK )
	{
		PrintError("%s(%d) Error(0x%x) \n\r",__FUNCTION__,__LINE__, nRet);
		goto _openctrl_error;
	}
	unBufferSize = SSTR_CONTROLLER_GetMaxBitrate(pSstr->hCtrl);
	if(unBufferSize <= 0)
	{
		unBufferSize = SSTR_CONFIG_MAX_BUFFER;
	}
	else
	{
		unBufferSize = (unBufferSize / 8) * SSTR_CONFIG_BUFFERING_SECS;
	}
	if(unBufferSize > SSTR_CONFIG_MAX_BUFFER )
	{
		unBufferSize = SSTR_CONFIG_MAX_BUFFER;
	}
	else if( unBufferSize < SSTR_CONFIG_MIN_BUFFER)
	{
		unBufferSize = SSTR_CONFIG_MIN_BUFFER;
	}
	nRet = SSTR_CONTROLLER_OpenBuffer(pSstr->hCtrl ,unBufferSize, SSTR_CONST_MEMORY_BUFFER);
	if (nRet != DI_ERR_OK)
	{
		PrintError ("%s(%d) Fail! CreateBuffer\n",__FUNCTION__,__LINE__);
		goto _openctrl_error;
	}
	if(DI_STREAM_SSTR_SetSettings(pSstr,NULL))
	{
		PrintError ("%s(%d) Fail! Default Setting(Heuristics Initialize)\n",__FUNCTION__,__LINE__);
		goto _openctrl_error;
	}
	P_STREAM_SSTR_CreateManifestInfo(pSstr);
	pSstr->pManifestUrl = VK_StrDup(pUrl);
	if(pSstr->pManifestUrl == NULL)
	{
		PrintError ("%s(%d) URL Duplication Fail\n",__FUNCTION__,__LINE__);
		goto _openctrl_error;
	}

	P_STREAM_SSTR_TimerSetLoadingTime(&pSstr->stDInfo);

	pSstr->pAccess = pAccess;
	g_hSstr = pSstr;

	g_ePreBufferingTaskState = DI_IPTUNER_SSTR_BUFFERING_TASK_STATE_READY;
	g_ePreBufferingTaskChangeState = DI_IPTUNER_SSTR_BUFFERING_TASK_STATE_READY;
#ifdef SSTR_CONFIG_WRITE_FRAGMENT_FOR_TEST
#ifdef SSTR_CONFIG_WRITE_ONE_FILE_FOR_TEST
	pSstr->fd = open("/home/fullcontents",O_CREAT|O_TRUNC|O_WRONLY|O_SYNC,0666);
#else
#endif
#endif
	PrintExit;
	return DI_ERR_OK;

_openctrl_error:
	if(pSstr->pFragBuff != NULL)
	{
		DD_MEM_Free(pSstr->pFragBuff);
	}
	if(pSstr->mp4Box.buf != NULL)
	{
		DD_MEM_Free(pSstr->mp4Box.buf);
	}
	if(pAccess != NULL)
	{
		(void)STREAM_HTTP_Close (pAccess);
		if ( pAccess->psz_path != NULL )
		{
			VK_MEM_Free(pAccess->psz_path);
			pAccess->psz_path = NULL;
		}
		DD_MEM_Free(pAccess);
	}
	(void)SSTR_CONTROLLER_Close(pSstr->hCtrl);
	if(pSstr != NULL)
	{
		DD_MEM_Free(pSstr);
	}
	PrintExit;
	return DI_ERR_ERROR;
}

int DI_STREAM_SSTR_CloseController ()
{
	HINT32 nRet = DI_ERR_OK;
	SSTR_t* pSstr = NULL;
	PrintEnter;
	if( g_hSstr == NULL )
	{
		PrintError("%s(%d) : Error> SSTR HANDLE is not opened\n",__FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}
	pSstr = __getSSTR(g_hSstr);
	if(pSstr == NULL)
	{
		return DI_ERR_ERROR;
	}
	if(pSstr->hCtrl == NULL)
	{
		PrintError("%s(%d) Error> SSTR CONTROLLER is NULL. Check This !!\n"
		, __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}
	if(pSstr->mp4Box.buf != NULL)
	{
		DD_MEM_Free(pSstr->mp4Box.buf);
	}
	if(pSstr->pFragBuff != NULL)
	{
		DD_MEM_Free(pSstr->pFragBuff);
	}
	if(pSstr->tAVUrl.url != NULL)
	{
		VK_MEM_Free(pSstr->tAVUrl.url);
		pSstr->tAVUrl.url = NULL;
	}
	if(pSstr->tAVUrl.swap != NULL)
	{
		VK_MEM_Free(pSstr->tAVUrl.swap);
		pSstr->tAVUrl.swap = NULL;
	}
	if(pSstr->tTextUrl.url != NULL)
	{
		VK_MEM_Free(pSstr->tTextUrl.url);
		pSstr->tTextUrl.url = NULL;
	}
	if(pSstr->tTextUrl.swap != NULL)
	{
		VK_MEM_Free(pSstr->tTextUrl.swap);
		pSstr->tTextUrl.swap = NULL;
	}
	if(pSstr->tUpdateUrl.swap != NULL)
	{
		VK_MEM_Free(pSstr->tUpdateUrl.swap);
		pSstr->tUpdateUrl.swap = NULL;
	}
	if(pSstr->pManifestUrl != NULL)
	{
		VK_MEM_Free(pSstr->pManifestUrl );
		pSstr->pManifestUrl = NULL;
	}
	(void)VK_SEM_Destroy(pSstr->ulSeekSema);
	(void)VK_SEM_Destroy(pSstr->ulLiveSema);
	SSTR_CONTROLLER_CloseBuffer(pSstr->hCtrl);
	P_STREAM_SSTR_DestroyManifestInfo(pSstr);
	(void)SSTR_CONTROLLER_Close(pSstr->hCtrl);
#ifdef SSTR_CONFIG_WRITE_FRAGMENT_FOR_TEST
#ifdef SSTR_CONFIG_WRITE_ONE_FILE_FOR_TEST
	close(pSstr->fd);
	pSstr->fd = -1;
#else
#endif
#endif
	if(pSstr->pAccess != NULL)
	{
		(void)STREAM_HTTP_Close (pSstr->pAccess);
		if ( pSstr->pAccess->psz_path != NULL )
		{
			VK_MEM_Free(pSstr->pAccess->psz_path);
			pSstr->pAccess->psz_path = NULL;
		}
		DD_MEM_Free(pSstr->pAccess);
		pSstr->pAccess = NULL;
	}
	if(pSstr != NULL)
	{
		DD_MEM_Free(pSstr);
	}
	g_hSstr = NULL;
	PrintExit;
	return nRet;
}

int DI_STREAM_SSTR_PreBuffering (DI_IPTUNER_Index_e nTunerIndex)
{
	HINT32 nRet = SSTR_ERR_OK;
	HINT32 nWriteSize=0;
	size_t sz_BoxSize = 0;
	ssize_t sz_RemainSize = 0;
	off_t cur = 0;
	HUINT64 ullRequestedSeekPoint;
	HUINT32 ulFragmentForSeek;
	HINT64 nSumLength=0;
	HUINT64 ullRTD = 0;
//	HUINT32 unStartTick = 0;
//	HUINT32 unEndTick = 0;
	SSTR_BUFFERMGR_t* pBufferMgr = NULL;
	HUINT32 nNetworkRetryCount = 0;
	HUINT32 nNoTrackRecovery = 0;
	HUINT32 nLiveRecovery = 0;
	SSTR_t *pSstr = NULL;

	HUINT64 ullAudioRTD = 0;
	HUINT64 ullAudioSize = 0;

	PrintEnter;
	if(nTunerIndex != DI_IPTUNER_STREAM_SSTR)
	{
		PrintError("%s(%d) : Error> invalid Tuner Type(%d)\n",__FUNCTION__,__LINE__,nTunerIndex);
		return DI_ERR_ERROR;
	}
	if(g_hSstr == NULL)
	{
		PrintError("%s(%d) Error> SSTR HANDLE is NULL \n", __FUNCTION__, __LINE__);
		return -1;
	}
	pSstr = __getSSTR(g_hSstr);
	if(pSstr->hCtrl == NULL)
	{
		PrintError("%s(%d) Error> SSTR CONTROLLER is NULL. Check This !!\n"
		, __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}
	pBufferMgr = SSTR_CONTROLLER_GetBuffer(pSstr->hCtrl );
	if(pBufferMgr == NULL)
	{
		PrintError("%s(%d) Error> pBufferMgr is NULL\n", __FUNCTION__, __LINE__);
	}
	g_ePreBufferingTaskState = DI_IPTUNER_SSTR_BUFFERING_TASK_STATE_STARTED;
//	g_ePreBufferingTaskChangeState = DI_IPTUNER_SSTR_BUFFERING_TASK_STATE_STARTED;
	do
	{
		//VK_SEM_Get (s_ulPreBufferingLock); 	//--------------------------------	Critical Section
		if(pBufferMgr == NULL)
		{
			PrintError("pBufferMgr is NULL\n");
			break;
		}
		if(g_ePreBufferingTaskChangeState >= DI_IPTUNER_SSTR_BUFFERING_TASK_STATE_KILLING)
		{
			PrintDebug("%s(%d) Break Buffering\n",__FUNCTION__,__LINE__);
			break;
		}
		/***********************************************************************
		 *  1. Seek 요청에 대한 처리
		 *     Seek Time이 포함된 Fragment 를 찾아서 Current Fragment 로 세팅
		 ***********************************************************************/
		if(P_STREAM_SSTR_IsSeekRequested(pSstr))
		{
#if defined(DI_IPTUNER_SSTR_CONFIG_TEXTSTREAM)
			pSstr->bReqTextFlush = TRUE;
			while(pSstr->eTextTaskState == DI_IPTUNER_SSTR_TEXT_TASK_STATE_FLUSHING || pSstr->bReqTextFlush == TRUE)
			{
				if(pSstr->bTextStreamTaskInit == FALSE ||
				pSstr->eTextTaskState == DI_IPTUNER_SSTR_TEXT_TASK_STATE_READY ||
				pSstr->eTextTaskState == DI_IPTUNER_SSTR_TEXT_TASK_STATE_DONE)
				{
					break;
				}
				VK_TASK_Sleep(DI_IPTUNER_SSTR_CONST_SLEEP_FOR_WFD);
			}
#endif
			VK_SEM_Get(pSstr->ulLiveSema);
			PrintDebug("%s(%d) : SSTR_CONTROLLER_IsSeekRequested SEEKING >>\n",__FUNCTION__,__LINE__);
			ullRequestedSeekPoint = P_STREAM_SSTR_GetTimeForSeek(pSstr);
			if(P_STREAM_SSTR_IsValidTimeForSeek(pSstr,ullRequestedSeekPoint))
			{
				PrintDebug("%s(%d) : SSTR_CONTROLLER_IsSeekRequested SEEKING >> %"PRId64"\n",__FUNCTION__,__LINE__,ullRequestedSeekPoint);
				if(	pSstr->eAVType != SSTR_CONTENTS_VIDEO )
				{
					(void)SSTR_CONTROLLER_GetFragmentByTime(pSstr->hCtrl, SSTR_CONTROLLER_GetCurAVStream(pSstr->hCtrl, SSTR_TYPE_AUDIO),ullRequestedSeekPoint,&ulFragmentForSeek);
					(void)SSTR_CONTROLLER_SetFragment(pSstr->hCtrl,SSTR_CONTROLLER_GetCurAVStream(pSstr->hCtrl, SSTR_TYPE_AUDIO), ulFragmentForSeek);
				}
				if(	pSstr->eAVType != SSTR_CONTENTS_AUDIO )
				{
					(void)SSTR_CONTROLLER_GetFragmentByTime(pSstr->hCtrl, SSTR_CONTROLLER_GetCurAVStream(pSstr->hCtrl, SSTR_TYPE_VIDEO),ullRequestedSeekPoint,&ulFragmentForSeek);
					(void)SSTR_CONTROLLER_SetFragment(pSstr->hCtrl,SSTR_CONTROLLER_GetCurAVStream(pSstr->hCtrl, SSTR_TYPE_VIDEO), ulFragmentForSeek);
				}
#ifdef DI_IPTUNER_SSTR_CONFIG_AUDIO_FIRST
				if(	pSstr->eAVType != SSTR_CONTENTS_VIDEO )
				{
					SSTR_CONTROLLER_SetCurStreamType(pSstr->hCtrl,SSTR_TYPE_AUDIO);
				}
#else
				if(	pSstr->eAVType != SSTR_CONTENTS_AUDIO )
				{
					SSTR_CONTROLLER_SetCurStreamType(pSstr->hCtrl,SSTR_TYPE_VIDEO);
				}
#endif
				if(pSstr->ulMaxText > 0)
				{
					P_STREAM_SSTR_ReqTextStream(pSstr, ullRequestedSeekPoint);
				}

				(void)SSTR_CONTROLLER_BitrateDown(pSstr->hCtrl);

				SSTR_CONTROLLER_ResetHeuristics(pSstr->hCtrl);
//			cleanup_buffer:
				ullAudioSize = 0;
				ullAudioRTD = 0;
				g_ePreBufferingTaskState = DI_IPTUNER_SSTR_BUFFERING_TASK_STATE_STARTED;
				(void)SSTR_BUFFERMGR_Clear(pBufferMgr);
			}
			P_STREAM_SSTR_SetRequestSeek(pSstr,FALSE);
			SSTR_BUFFERMGR_SetReadLock(pBufferMgr, FALSE);
			VK_SEM_Release(pSstr->ulLiveSema);
		}
		if(g_ePreBufferingTaskState == DI_IPTUNER_SSTR_BUFFERING_TASK_STATE_WAITING)
		{
			if(P_STREAM_SSTR_NextFragment(pSstr) != DI_ERR_OK)
			{
				g_ePreBufferingTaskState = DI_IPTUNER_SSTR_BUFFERING_TASK_STATE_WAITING;
//				PrintDebug("[PAUSE] P_STREAM_SSTR_NextFragment \n");
				VK_TASK_Sleep(DI_IPTUNER_SSTR_CONST_SLEEP_FOR_WFD);
				continue;
			}
			else
			{
				g_ePreBufferingTaskState = DI_IPTUNER_SSTR_BUFFERING_TASK_STATE_STARTED;
//				PrintDebug("[PLAY] P_STREAM_SSTR_NextFragment \n");
			}
		}
		/***********************************************************************
		 *	2. Download Current Fragment
		 *	   현재의 Fragment 를 Download 한다.
		 ***********************************************************************/
		ullRTD = 0;
		nRet = P_STREAM_SSTR_DownloadCurFragment(pSstr,pSstr->pAccess,&ullRTD);
		if(nRet == SSTR_ERR_DOWNLOAD_FAIL)
		{
			nNetworkRetryCount++;
			PrintError("%s(%d) Fragment Download Fail. Retry (%d)\n",__FUNCTION__,__LINE__,nNetworkRetryCount);
			if(nNetworkRetryCount > DI_IPTUNER_SSTR_CONST_RETRY_COUNT_ON_NET_ERROR)
			{
				PrintError("%s(%d) Fragment Download Fail. Break Main Loop\n",__FUNCTION__,__LINE__);
				break;
			}
			else
			{
				continue;
			}
		}
		else if(nRet == SSTR_ERR_CONNECT_FAIL)
		{
			if(SSTR_CONTROLLER_IsLive(pSstr->hCtrl))
			{
				nNetworkRetryCount++;
				PrintError("%s(%d) Fragment Connection Fail. Retry (%d)\n",__FUNCTION__,__LINE__,nNetworkRetryCount);
				if(nNetworkRetryCount > DI_IPTUNER_SSTR_CONST_RETRY_COUNT_ON_NET_ERROR)
				{
					nLiveRecovery++;
					if(nLiveRecovery <= DI_IPTUNER_SSTR_CONST_RETRY_COUNT_ON_NET_ERROR)
					{
						(void)P_STREAM_SSTR_LiveRecovery(pSstr);
					}
					else
					{
						PrintError("%s(%d) Fragment Download Fail. Break Main Loop\n",__FUNCTION__,__LINE__);
						break;
					}
				}
				else
				{
					VK_TASK_Sleep(DI_IPTUNER_SSTR_CONST_SLEEP_FOR_WFD);
					continue;
				}
			}
			else
			{
				if(nNoTrackRecovery)
				{
					if(SSTR_CONTROLLER_GetCurBitrate(pSstr->hCtrl) != SSTR_CONTROLLER_GetMaxBitrate(pSstr->hCtrl))
					{
						PrintError("%s(%d) Fragment Connection Fail. Try to Another Track\n",__FUNCTION__,__LINE__);
						SSTR_CONTROLLER_BitrateUp(pSstr->hCtrl);
						continue;
					}
					else
					{
						nNoTrackRecovery = 0;
					}
				}
				else
				{
					if(SSTR_CONTROLLER_GetCurBitrate(pSstr->hCtrl) != SSTR_CONTROLLER_GetMinBitrate(pSstr->hCtrl))
					{
						PrintError("%s(%d) Fragment Connection Fail. Try to Another Track\n",__FUNCTION__,__LINE__);
						SSTR_CONTROLLER_BitrateDown(pSstr->hCtrl);
						continue;
					}
					else
					{
						nNoTrackRecovery = 1;
					}
				}
				nNetworkRetryCount++;
				PrintError("%s(%d) Fragment Connection Fail. Retry (%d)\n",__FUNCTION__,__LINE__,nNetworkRetryCount);
				if(nNetworkRetryCount > DI_IPTUNER_SSTR_CONST_RETRY_COUNT_ON_NET_ERROR)
				{
					PrintError("%s(%d) Fragment Download Fail. Break Main Loop\n",__FUNCTION__,__LINE__);
					break;
				}
				PrintError("%s(%d) Fragment Connection Fail. Try to Next Fragment\n",__FUNCTION__,__LINE__);
#ifdef DI_IPTUNER_SSTR_CONFIG_AUDIO_FIRST
				if( pSstr->eAVType != SSTR_CONTENTS_AUDIO )
				{
					SSTR_CONTROLLER_SetCurStreamType(pSstr->hCtrl,SSTR_TYPE_VIDEO);
				}
#else
				if( pSstr->eAVType != SSTR_CONTENTS_VIDEO )
				{
					SSTR_CONTROLLER_SetCurStreamType(pSstr->hCtrl,SSTR_TYPE_AUDIO);
				}
#endif
				if(P_STREAM_SSTR_NextFragment(pSstr) != DI_ERR_OK)
				{
					g_ePreBufferingTaskState = DI_IPTUNER_SSTR_BUFFERING_TASK_STATE_WAITING;
					VK_TASK_Sleep(DI_IPTUNER_SSTR_CONST_SLEEP_FOR_WFD);
				}
			}
			continue;
		}
		else if(nRet == SSTR_ERR_DOWNLOAD_FORCED_EXIT)
		{
			/***********************************************************************
			 *	Check Status Change (After Loop)
			 ***********************************************************************/
			if(g_ePreBufferingTaskChangeState >= DI_IPTUNER_SSTR_BUFFERING_TASK_STATE_KILLING)
			{
				PrintDebug("%s(%d) Break Main Loop\n",__FUNCTION__,__LINE__);
				break;
			}
			if(P_STREAM_SSTR_IsSeekRequested(pSstr))
			{
				continue;
			}
		}
		else if(nRet == SSTR_ERR_DOWNLOAD_PANIC)
		{
			continue;
		}
		else if(nRet == SSTR_ERR_OK)
		{
			nLiveRecovery = 0;
			nNetworkRetryCount = 0;
		}
		else
		{
			PrintError("%s(%d) Fragment Download Fail. UNKNOWN ERROR (%d)\n",__FUNCTION__,__LINE__,nRet);
			break;
		}
		if(SSTR_CONTROLLER_IsLive(pSstr->hCtrl))
		{
			HUINT64 ullDelay = SSTR_CONTROLLER_GetCurFragmentDuration(pSstr->hCtrl, SSTR_CONTROLLER_GetCurStreamType(pSstr->hCtrl));
			HUINT64 ullTime = 0;
			ullDelay = SSTR_CONVERT_TO_MILLISEC(ullDelay) / 2;
			while(pSstr->bPaused)
			{
				// SSTR_BUFFERMGR_Clear(pBufferMgr);
				if(g_ePreBufferingTaskChangeState >= DI_IPTUNER_SSTR_BUFFERING_TASK_STATE_KILLING)
				{
					PrintDebug("%s(%d) Break Main Loop\n",__FUNCTION__,__LINE__);
					break;
				}
				VK_TASK_Sleep(100);
				ullTime += 100;
				if(ullTime >= ullDelay)
				{
					break;
				}
			}
			(void)P_STREAM_SSTR_LiveUpdate(pSstr,pSstr->pFragBuff,pSstr->sz_FragSize,SSTR_CONTROLLER_GetCurAVStream(pSstr->hCtrl, SSTR_CONTROLLER_GetCurStreamType(pSstr->hCtrl)));
		}
		/***********************************************************************
		 *  3. Make Box & Buffering
		 *     Current Fragment의 BRCM Specific Fragmented Mp4 Box를 생성하여 Buffer에 Writing 한다.
		 ***********************************************************************/
		{
			cur = 0;
			P_STREAM_SSTR_ClearBox (pSstr);
			sz_BoxSize = P_STREAM_SSTR_MakeBox(pSstr);
#ifdef SSTR_CONFIG_WRITE_FRAGMENT_FOR_TEST
#ifdef SSTR_CONFIG_WRITE_BOX_FOR_TEST
			P_STREAM_SSTR_WriteFrag(pSstr , pSstr->mp4Box.buf , 0, sz_BoxSize);
#endif
#endif
		    if(sz_BoxSize > 0)
		    {
				while(sz_BoxSize > 0)
				{
					if(g_ePreBufferingTaskChangeState >= DI_IPTUNER_SSTR_BUFFERING_TASK_STATE_KILLING)
					{
						PrintDebug("%s(%d) Break Loop(Box Write)\n",__FUNCTION__,__LINE__);
						break;
					}
					nWriteSize = SSTR_BUFFERMGR_Write(pBufferMgr, pSstr->mp4Box.buf + cur , sz_BoxSize);
					if(nWriteSize == 0)
					{
						if(SSTR_CONTROLLER_IsLive(pSstr->hCtrl) && pSstr->bPaused)
						{
							SSTR_BUFFERMGR_Clear(pBufferMgr);
						}
						else
						{
							VK_TASK_Sleep(DI_IPTUNER_SSTR_CONST_SLEEP_FOR_WFD);
							continue;
						}
					}
					cur += nWriteSize;
					sz_BoxSize -= nWriteSize;
				}
		    }
		    else
		    {
		    	PrintError("%s(%d) Make HEADER BOX Fail\n",__FUNCTION__,__LINE__);
		    	break;
	    	}
		}

		/***********************************************************************
		 *	Check Status Change (After Loop)
		 ***********************************************************************/
		if(g_ePreBufferingTaskChangeState >= DI_IPTUNER_SSTR_BUFFERING_TASK_STATE_KILLING)
		{
			PrintDebug("%s(%d) Break Main Loop\n",__FUNCTION__,__LINE__);
			break;
		}
		if(P_STREAM_SSTR_IsSeekRequested(pSstr))
		{
			continue;
		}

		/***********************************************************************
		 *  4. Buffering Fragment
		 *     Download 받은 Fragment를 Buffer에 Writing 한다.
		 ***********************************************************************/
		nSumLength=0;
		cur = 0;
#ifdef SSTR_CONFIG_WRITE_FRAGMENT_FOR_TEST
		P_STREAM_SSTR_WriteFrag(pSstr , pSstr->pFragBuff, 0, pSstr->sz_FragSize);
#endif
		sz_RemainSize = pSstr->sz_FragSize;
		while(sz_RemainSize > 0)
		{
			if(g_ePreBufferingTaskChangeState >= DI_IPTUNER_SSTR_BUFFERING_TASK_STATE_KILLING)
			{
				PrintDebug("%s(%d) Break Loop(Fragment Write)\n",__FUNCTION__,__LINE__);
				break;
			}
			if(P_STREAM_SSTR_IsSeekRequested(pSstr))
			{
				break;
			}
			nWriteSize = SSTR_BUFFERMGR_Write(pBufferMgr,pSstr->pFragBuff + cur, sz_RemainSize);
			if(nWriteSize == 0)
			{
				if(SSTR_CONTROLLER_IsLive(pSstr->hCtrl) && pSstr->bPaused)
				{
					SSTR_BUFFERMGR_Clear(pBufferMgr);
				}
				else
				{
					VK_TASK_Sleep(DI_IPTUNER_SSTR_CONST_SLEEP_FOR_WFD);
					continue;
				}
			}
			cur += nWriteSize;
			sz_RemainSize -= nWriteSize;
		}

		P_STREAM_SSTR_TimerSetStartingTime(&pSstr->stDInfo);

		/***********************************************************************
		 *	Check Status Change (After Loop)
		 ***********************************************************************/
		if(g_ePreBufferingTaskChangeState >= DI_IPTUNER_SSTR_BUFFERING_TASK_STATE_KILLING)
		{
			PrintDebug("%s(%d) Break Main Loop\n",__FUNCTION__,__LINE__);
			break;
		}
		if(P_STREAM_SSTR_IsSeekRequested(pSstr))
		{
			continue;
		}
		/***********************************************************************
		 *	5. Bitrate Change
		 *	   Heuristics Module을 이용하여 Bitrate Changing 수행 (Auto Mode)
		 ***********************************************************************/
//		if(stConfig.eDecision == SSTR_DECISION_AUTO)
		{
//			PrintDebug ("%s(%d) Enter SSTR_DECISION_AUTO Mode. \n",__FUNCTION__,__LINE__);
			if(ullRTD == 0) /** avoid divide zero */
			{
				ullRTD = 10;
			}
			if(SSTR_CONTROLLER_GetCurStreamType(pSstr->hCtrl) == SSTR_TYPE_VIDEO)
			{
				VK_SEM_Get(pSstr->ulLiveSema);
				SSTR_HEURISTICS_Run(SSTR_CONTROLLER_GetHeuristics(pSstr->hCtrl),pSstr->sz_FragSize + ullAudioSize,ullRTD + ullAudioRTD);
				ullAudioSize = 0;
				ullAudioRTD = 0;
				VK_SEM_Release(pSstr->ulLiveSema);
			}
			else
			{
				ullAudioSize += pSstr->sz_FragSize;
				ullAudioRTD += (ullRTD);
			}
//			unStartTick = VK_TIMER_GetSystemTick(); /* 1000 tick == 1 sec */
		}
#if 0
		else if(stConfig.eDecision == SSTR_DECISION_MANUAL)
		{
			PrintDebug ("%s(%d) SSTR_DECISION_MANUAL\n",__FUNCTION__,__LINE__);
			while(!SSTR_CONTROLLER_GetDecision(pSstr->hCtrl ))
			{
				if(g_ePreBufferingTaskChangeState >= DI_IPTUNER_SSTR_BUFFERING_TASK_STATE_KILLING)
				{
					PrintDebug("%s(%d) Break Buffering\n",__FUNCTION__,__LINE__);
					goto exit_stop;
				}
				VK_TASK_Sleep(DI_IPTUNER_SSTR_CONST_SLEEP_FOR_WFD);
			}
			SSTR_CONTROLLER_SetDecision(pSstr->hCtrl ,FALSE);
		}
#endif

#ifdef __SSTR_NOT_SUPPORTED_YET__
		if(stAdaptiveCallback.cbAVStreamCallback != NULL)
		{
			stAdaptiveCallback.cbAVStreamCallback(nTunerIndex,SSTR_CONTROLLER_GetCurStreamType(pSstr->hCtrl ), SSTR_CONTROLLER_GetCurBitrate(pSstr->hCtrl ),SSTR_CONTROLLER_GetCurFragment(pSstr->hCtrl ));
		}
#endif
		/***********************************************************************
		 *	5. Setting Next Fragment
		 *	   다음 Fragment를 Current Fragment로 지정한다.
		 *	   Next Fragment가 없을 경우 EOS 처리 한다.
		 ***********************************************************************/
		if(P_STREAM_SSTR_NextFragment(pSstr) != DI_ERR_OK)
		{
			g_ePreBufferingTaskState = DI_IPTUNER_SSTR_BUFFERING_TASK_STATE_WAITING;
//			PrintDebug("%s(%d) Waiting For Update (or next fragment)\n",__FUNCTION__,__LINE__);
			VK_TASK_Sleep(DI_IPTUNER_SSTR_CONST_SLEEP_FOR_WFD);
			continue;
		}
	}while (g_ePreBufferingTaskChangeState != DI_IPTUNER_SSTR_BUFFERING_TASK_STATE_KILLING); /* End Main Loop*/
	SSTR_BUFFERMGR_SetReadLock(pBufferMgr, FALSE);
	g_ePreBufferingTaskState = DI_IPTUNER_SSTR_BUFFERING_TASK_STATE_DONE;
	g_ePreBufferingTaskChangeState = DI_IPTUNER_SSTR_BUFFERING_TASK_STATE_DONE;
	PrintExit;
	return 0;
}

/**
 * @brief       "OpenFromNexus" VFIO
 * @param	path : path of file
 * @param       flags :
 * @param	ptHandle : VFIO Handle
 * @return     	VFIO err code
 * @date        2011/02/19
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/
/* Open From Nexus */
int P_STREAM_SSTR_OpenFromNexus (const HINT8 *pUrl, const unsigned int flags, VFIO_Handle_t *ptHandle)
{
	SSTR_t* pSstr = NULL;
	HINT32 nRet = DI_ERR_OK;
	UNUSED_PARAM (flags);
	SSTR_STREAM_TYPE_e eType = SSTR_TYPE_UNKNOWN;
#if STREAM_BUFFERING_SSTR
	DI_IPTUNER_Msg_t s_Msg;
#endif

	PrintEnter;
	if(pUrl==NULL)
	{
		ptHandle->ucAvailable = 0;
		PrintError("%s(%d) : Error> invalid pUrl(%s)\n",__FUNCTION__,__LINE__,pUrl);
		return VFIO_ERROR_IN_DEVICE;
	}
	/* assert check */
	if (ptHandle == NULL || pUrl == NULL)
	{
		PrintError("pVfioHandle is NULL\n");
		return VFIO_ERROR_IN_DEVICE;
	}
	pSstr = __getSSTR(g_hSstr);
	if(pSstr->hCtrl == NULL)
	{
		PrintError("%s(%d) Error> SSTR CONTROLLER is NULL. Check This !!\n"
		, __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}
	/*
	 * We must store a STREAM Handler in iptuner handle .
	 * If not so, we can't know correct stream handle in vfio function set.(VFIO_Read/VFIO_xxx etc)
	 *
	 * iptuner_handler --  VFIO Handler (include file and socket descriptor)
	 *				   --  STREAM Handler ( include protocol controller)
	 */
/*
	VK_sprintf(tmpString,"httpSstrSema_%p",(void*)hSstr);
	if( VK_OK != VK_SEM_Create(&s_ulSstrSemId,tmpString, DEFAULT_SUSPEND_TYPE))
	{
		PrintError ("%s(%d) Error> can't create semaphore httpSema",__FUNCTION__,__LINE__);
		PrintExit;
		return VFIO_ERROR_IN_DEVICE;
	}
*/

#ifdef DI_IPTUNER_SSTR_CONFIG_AUDIO_FIRST
	SSTR_CONTROLLER_SetCurStreamType(pSstr->hCtrl,SSTR_TYPE_AUDIO);
	if(pSstr->eAVType == SSTR_CONTENTS_ALL ||
	pSstr->eAVType == SSTR_CONTENTS_AUDIO)
	{
		SSTR_CONTROLLER_SetCurStreamType(pSstr->hCtrl,SSTR_TYPE_AUDIO);
	}
	else if(pSstr->eAVType == SSTR_CONTENTS_VIDEO)
	{
		SSTR_CONTROLLER_SetCurStreamType(pSstr->hCtrl,SSTR_TYPE_VIDEO);
	}
#else
	if(pSstr->eAVType == SSTR_CONTENTS_ALL ||
	pSstr->eAVType == SSTR_CONTENTS_VIDEO)
	{
		SSTR_CONTROLLER_SetCurStreamType(pSstr->hCtrl,SSTR_TYPE_VIDEO);
	}
	else if(pSstr->eAVType == SSTR_CONTENTS_AUDIO)
	{
		SSTR_CONTROLLER_SetCurStreamType(pSstr->hCtrl,SSTR_TYPE_AUDIO);
	}
#endif
	if(SSTR_CONTROLLER_IsLive(pSstr->hCtrl))
	{
		SSTR_STREAM_TYPE_e eType = SSTR_CONTROLLER_GetCurStreamType(pSstr->hCtrl);
		HUINT32 unStream = SSTR_CONTROLLER_GetCurAVStream(pSstr->hCtrl,eType);
		HUINT32 unFragment = SSTR_CONTROLLER_GetTotalFragment(pSstr->hCtrl, unStream);
		HUINT64 ullTime = 0;
		HUINT64 ullDuration = SSTR_CONTROLLER_GetCurFragmentDuration(pSstr->hCtrl, eType);
		if(ullDuration != 0)
		{
			if(unFragment > DI_IPTUNER_SSTR_CONST_LIVE_STARTDELAY / ullDuration)
			{
				unFragment -= DI_IPTUNER_SSTR_CONST_LIVE_STARTDELAY / ullDuration;
				if(SSTR_CONTROLLER_SetFragment(pSstr->hCtrl, unStream, unFragment) == SSTR_ERR_OK)
				{
					ullTime = SSTR_CONTROLLER_GetCurFragmentStartTime(pSstr->hCtrl, eType);
					if(pSstr->eAVType == SSTR_CONTENTS_ALL)
					{
						unStream = SSTR_CONTROLLER_GetCurAVStream(pSstr->hCtrl,SSTR_TYPE_AUDIO);
						if(SSTR_CONTROLLER_GetFragmentByTime(pSstr->hCtrl, unStream, ullTime, &unFragment) != SSTR_ERR_OK
						|| SSTR_CONTROLLER_SetFragment(pSstr->hCtrl, unStream, unFragment) != SSTR_ERR_OK)
						{
							PrintError("%s (%d) Live Setting Fail. Rollback and Continue.\n",__FUNCTION__,__LINE__);
							eType = SSTR_CONTROLLER_GetCurStreamType(pSstr->hCtrl);
							unStream = SSTR_CONTROLLER_GetCurAVStream(pSstr->hCtrl,eType);
							if(SSTR_CONTROLLER_SetFragment(pSstr->hCtrl, unStream, 0) != SSTR_ERR_OK)
							{
								PrintError("%s (%d) SSTR_CONTROLLER_SetFragment Fail\n",__FUNCTION__,__LINE__);
							}
						}
					}
				}
			}
		}
	}

	eType = SSTR_CONTROLLER_GetCurStreamType(pSstr->hCtrl);
	pSstr->ullLiveStartTime = SSTR_CONTROLLER_GetCurFragmentStartTime(pSstr->hCtrl,eType);
	pSstr->ullLiveStartSystemTime = VK_TIMER_GetSystemTick();

	if(pSstr->ulMaxText > 0 )
	{
		P_STREAM_SSTR_ReqTextStream(pSstr, SSTR_CONTROLLER_GetCurFragmentStartTime(pSstr->hCtrl, SSTR_CONTROLLER_GetCurStreamType(pSstr->hCtrl)));
	}
#if STREAM_BUFFERING_SSTR
	PrintData ("%s (%d) Buffering Start from OpenNexusCallback\n",__FUNCTION__,__LINE__);
	(void) VK_memset (&s_Msg, 0x0, sizeof(s_Msg));
	s_Msg.unMsg = TUNER_STREAM_MSG_SSTR_BUFFERING;
	s_Msg.unParam1 = DI_IPTUNER_STREAM_SSTR;
	g_ePreBufferingTaskState = DI_IPTUNER_SSTR_BUFFERING_TASK_STATE_READY;
	g_ePreBufferingTaskChangeState = DI_IPTUNER_SSTR_BUFFERING_TASK_STATE_READY;
	(void) VK_MSG_Send ( s_ulStreamMsgQId, (HUINT32 *)&s_Msg, sizeof(DI_IPTUNER_Msg_t) ) ;
#endif
	PrintExit;
	return nRet;
}

/**
 * @brief       "SeekFromNexus" VFIO
 * @param	tHandle : VFIO Handle
 * @param       offset : offset for seeking
 * @param	whence : whence
 * @return     	VFIO err code
 * @date        2011/02/19
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/
/* Seek From Nexus : Long jump 인 경우 어느정도는 block 되어도 된다.
 * 단 Read 함수에서는 block 되면 waiting IO error, 0 return 시 endofStream, -1 return시 errorofstream 이 뜬다.*/
off_t P_STREAM_SSTR_SeekForNexus (VFIO_Handle_t tHandle, off_t offset, int whence)
{
	// HINT32 nSegment = 0;
	// HUINT64 ullBitrate = 0;
	SSTR_t* pSstr = NULL;
	UNUSED_PARAM(tHandle);
	UNUSED_PARAM(whence);
	PrintEnter;
	if(g_hSstr == NULL)
	{
		PrintError("%s(%d) : Error> SSTR Controller is NULL.\n",__FUNCTION__,__LINE__);
		return VFIO_ERROR_IN_DEVICE;
	}
	pSstr = __getSSTR(g_hSstr);
	if(pSstr->hCtrl == NULL)
	{
		PrintError("%s(%d) Error> SSTR CONTROLLER is NULL. Check This !!\n"
		, __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}
	pSstr->ullVirtureOffset = offset;
	PrintExit;
	return offset;
}

/**
 * @brief       "BoundsFromNexus" VFIO
 * @param	tHandle : VFIO Handle
 * @param       file_size : size of content
 * @return     	VFIO err code
 * @date        2011/02/19
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/
/* Bounds From Nexus */
int P_STREAM_SSTR_BoundsForNexus (VFIO_Handle_t tHandle, off_t *file_size)
{
	HINT32 nRet = VFIO_ERROR_SUCCESS;
	UNUSED_PARAM(tHandle);
//	PrintEnter;
	*file_size = 0xFFFFFFFFFFFFFF;
//	PrintExit;
	return nRet;
}

#if 0
/**
 * @brief       "Open" From User Application and bmedia VFIO
 * @param	path : path of file
 * @param       mode :
 * @param	ptHandle : VFIO Handle
 * @return     	VFIO err code
 * @date        2011/02/19
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/
/** Open From User Application and bmedia*/
int DI_STREAM_SSTR_Open  (const HINT8 *path, const HINT8 *mode, VFIO_Handle_t *ptHandle )
{
	SSTR_t* pSstr = NULL;
	HINT32 nRet = DI_ERR_OK;
//	HINT8 tmpString[256]; /* for naming */
	DI_IPTUNER_t *pHTuner=NULL;
	UNUSED_PARAM(mode);
	if(path==NULL)
	{
		ptHandle->ucAvailable = 0;
		PrintError("%s(%d) : Error> invalid path(%s)\n",__FUNCTION__,__LINE__,path);
		return VFIO_ERROR_IN_DEVICE;
	}
	pHTuner = DI_IPTUNER_POOL_FindByUrl ((HINT8*)path);
	if(pHTuner == NULL)
	{
		ptHandle->ucAvailable = 0;
		PrintError ("%s(%d) : Error> can't find iptuner handler by this path(%s)\n",__FUNCTION__,__LINE__,path);
		return VFIO_ERROR_IN_DEVICE;
	}
	if(pHTuner->hAbr == NULL)
	{
		PrintError("%s(%d) : Error> SSTR Controller is NULL.\n",__FUNCTION__,__LINE__);
		PrintExit;
		return DI_ERR_ERROR;
	}
	pSstr = __getSSTR(pHTuner->hAbr);
	if(pSstr->hCtrl == NULL)
	{
		PrintError("%s(%d) Error> SSTR CONTROLLER is NULL. Check This !!\n"
		, __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}

	/*
	 * We must store a STREAM Handler in iptuner handle .
	 * If not so, we can't know correct stream handle in vfio function set.(VFIO_Read/VFIO_xxx etc)
	 *
	 * iptuner_handler --  VFIO Handler (include file and socket descriptor)
	 *				   --  STREAM Handler ( include protocol controller)
	 */
/*
	VK_sprintf(tmpString,"httpSstrSema_%p",hSstr);
	if( VK_OK != VK_SEM_Create(&s_ulSstrSemId,tmpString, DEFAULT_SUSPEND_TYPE))
	{
		PrintError ("%s(%d) Error> can't create semaphore httpSema",__FUNCTION__,__LINE__);
		PrintExit;
		return VFIO_ERROR_IN_DEVICE;
	}
*/
#ifdef DI_IPTUNER_SSTR_CONFIG_AUDIO_FIRST
	SSTR_CONTROLLER_SetCurStreamType(pSstr->hCtrl,SSTR_TYPE_AUDIO);
#else
	SSTR_CONTROLLER_SetCurStreamType(pSstr->hCtrl,SSTR_TYPE_VIDEO);
#endif
	PrintExit;
	return nRet;
}

int DI_STREAM_SSTR_Close (VFIO_Handle_t tHandle)
{
	HINT32 vRet = VFIO_ERROR_SUCCESS;
	SSTR_t* pSstr = NULL;
	DI_IPTUNER_t *pHTuner = NULL;
	PrintEnter;
	/* Getting IPTUNER handle from VFIO Handle */
	pHTuner = tHandle.pTunerHandle;
	if (pHTuner == NULL)
	{
		tHandle.ucAvailable = 0;
		PrintError ("%s (%d) Error>  Invalid Tuner Handle in VFIO Handle\n",__FUNCTION__,__LINE__);
		return VFIO_ERROR_IN_DEVICE;
	}

	if(pHTuner->hAbr== NULL)
	{
		PrintError("%s(%d) : Error> SSTR HANDLE is NULL.\n",__FUNCTION__,__LINE__);
		return VFIO_ERROR_IN_DEVICE;
	}
	pSstr = __getSSTR(pHTuner->hAbr);
	if(pSstr->hCtrl == NULL)
	{
		PrintError("%s(%d) Error> SSTR CONTROLLER is NULL. Check This !!\n"
		, __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}
	/* Now we just get the Stream handler */
//	(void)VK_SEM_Get(s_ulSstrSemId);
	pSstr->bImmediatelyExit = TRUE;
//	(void)VK_SEM_Release (s_ulSstrSemId);
//	(void)VK_SEM_Destroy(s_ulSstrSemId);
//	s_ulSstrSemId = 0;
	PrintExit;
	return vRet;
}
#endif

#if 1
DI_IPTUNER_SSTR_Info_t* DI_STREAM_SSTR_GetManifestInfo()
{
	SSTR_t* pSstr = NULL;
	PrintEnter;
	if(g_hSstr == NULL)
	{
		PrintError("%s(%d) : Error> SSTR HANDLE is NULL.\n",__FUNCTION__,__LINE__);
		return NULL;
	}
	pSstr = __getSSTR(g_hSstr);
	if(pSstr->hCtrl == NULL)
	{
		PrintError("%s(%d) Error> SSTR CONTROLLER is NULL. Check This !!\n"
		, __FUNCTION__, __LINE__);
		return NULL;
	}
	if(pSstr->pManifestInfo == NULL)
	{
		PrintError("%s(%d) Error> SSTR Manifest Info is NULL. Check This !!\n"
		, __FUNCTION__, __LINE__);
		return NULL;
	}
	return pSstr->pManifestInfo;
}

int DI_STREAM_SSTR_SetProperty(DI_IPTUNER_ADAPTIVE_PROPERTY_e eCmd, DI_IPTUNER_ADAPTIVE_PROPERTY_DATA_t *pData)
{
	int nRet = DI_ERR_OK;
	SSTR_t* pSstr = NULL;
	PrintEnter;
	if(g_hSstr == NULL)
	{
		PrintError("%s(%d) : Error> SSTR HANDLE is NULL.\n",__FUNCTION__,__LINE__);
		return VFIO_ERROR_IN_DEVICE;
	}
	pSstr = __getSSTR(g_hSstr);
	if(pSstr->hCtrl == NULL)
	{
		PrintError("%s(%d) Error> SSTR CONTROLLER is NULL. Check This !!\n"
		, __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}
	switch ( eCmd )
	{
		case DI_IPTUNER_PROPERTY_SSTR_VIDEO_STREAM:
			SSTR_CONTROLLER_SetStreamByArrayIndex(pSstr->hCtrl, SSTR_TYPE_VIDEO, pData->uiIndex);
			break;
		case DI_IPTUNER_PROPERTY_SSTR_AUDIO_STREAM:
			SSTR_CONTROLLER_SetStreamByArrayIndex(pSstr->hCtrl, SSTR_TYPE_AUDIO, pData->uiIndex);
			break;
		case DI_IPTUNER_PROPERTY_SSTR_TEXT_STREAM:
			/* not supported yet */
			PrintError ("[%s] [%d] DI_IPTUNER_ADAPTIVE_SSTR_SET_TEXT_STREAM is not supported\n",__FUNCTION__,__LINE__);
			nRet = DI_ERR_ERROR;
			break;
		case DI_IPTUNER_PROPERTY_SSTR_VIDEO_TRACK:
			SSTR_CONTROLLER_SetTrackByIndex(pSstr->hCtrl, SSTR_TYPE_VIDEO, pData->uiIndex);
			break;
		case DI_IPTUNER_PROPERTY_SSTR_AUDIO_TRACK:
			SSTR_CONTROLLER_SetTrackByIndex(pSstr->hCtrl, SSTR_TYPE_AUDIO, pData->uiIndex);
			break;
		case DI_IPTUNER_PROPERTY_SSTR_TEXT_TRACK:
			/* not supported yet */
			PrintError ("[%s] [%d] DI_IPTUNER_ADAPTIVE_SSTR_SET_TEXT_TRACK is not supported\n",__FUNCTION__,__LINE__);
			nRet = DI_ERR_ERROR;
			break;
		default  :
			PrintError ("[%s] [%d] Cmd %d is not supported\n",__FUNCTION__,__LINE__, eCmd);
			nRet = DI_ERR_ERROR;
			break;
	}
	return nRet;
}

int DI_STREAM_SSTR_GetProperty(DI_IPTUNER_ADAPTIVE_PROPERTY_e eCmd, DI_IPTUNER_ADAPTIVE_PROPERTY_DATA_t *pData)
{
	int nRet = DI_ERR_OK;
	SSTR_t* pSstr = NULL;
	SSTR_SUMMARY_t tSummary;
	PrintEnter;
	if(g_hSstr == NULL)
	{
		PrintError("%s(%d) : Error> SSTR HANDLE is NULL.\n",__FUNCTION__,__LINE__);
		return VFIO_ERROR_IN_DEVICE;
	}
	pSstr = __getSSTR(g_hSstr);
	if(pSstr->hCtrl == NULL)
	{
		PrintError("%s(%d) Error> SSTR CONTROLLER is NULL. Check This !!\n"
		, __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}
	if(SSTR_CONTROLLER_GetSummary(pSstr->hCtrl, &tSummary) != SSTR_ERR_OK)
	{
		PrintError("%s(%d) : Error> Get Summary Failed.\n",__FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}
	switch ( eCmd )
	{
		case DI_IPTUNER_PROPERTY_SSTR_VIDEO_STREAM:
			pData->uiIndex = tSummary.nCurVideoIndex;
//			SSTR_CONTROLLER_SetStreamByArrayIndex(pSstr->hCtrl, SSTR_TYPE_VIDEO, tData.uiIndex);
			break;
		case DI_IPTUNER_PROPERTY_SSTR_AUDIO_STREAM:
			pData->uiIndex = tSummary.nCurAudioIndex;
//			SSTR_CONTROLLER_SetStreamByArrayIndex(pSstr->hCtrl, SSTR_TYPE_AUDIO, tData.uiIndex);
			break;
		case DI_IPTUNER_PROPERTY_SSTR_TEXT_STREAM:
			/* not supported yet */
			PrintError ("[%s] [%d] DI_IPTUNER_ADAPTIVE_SSTR_SET_TEXT_STREAM is not supported\n",__FUNCTION__,__LINE__);
			nRet = DI_ERR_ERROR;
			break;
		case DI_IPTUNER_PROPERTY_SSTR_VIDEO_TRACK:
			pData->uiIndex = tSummary.nCurVideoTrack;
//			SSTR_CONTROLLER_SetTrackByIndex(pSstr->hCtrl, SSTR_TYPE_VIDEO, tData.uiIndex);
			break;
		case DI_IPTUNER_PROPERTY_SSTR_AUDIO_TRACK:
			pData->uiIndex = tSummary.nCurAudioTrack;
//			SSTR_CONTROLLER_SetTrackByIndex(pSstr->hCtrl, SSTR_TYPE_AUDIO, tData.uiIndex);
			break;
		case DI_IPTUNER_PROPERTY_SSTR_TEXT_TRACK:
			/* not supported yet */
			PrintError ("[%s] [%d] DI_IPTUNER_ADAPTIVE_SSTR_SET_TEXT_TRACK is not supported\n",__FUNCTION__,__LINE__);
			nRet = DI_ERR_ERROR;
			break;
		default  :
			PrintError ("[%s] [%d] Cmd %d is not supported\n",__FUNCTION__,__LINE__, eCmd);
			nRet = DI_ERR_ERROR;
			break;
	}
	return nRet;
}
#endif

DI_ERR_CODE DI_STREAM_SSTR_Control (DI_IPTUNER_CMD_e cmd, HUINT64 val, HUINT64 val2 )
{
	DI_ERR_CODE nRet=DI_ERR_OK;
	SSTR_t* pSstr = NULL;
	SSTR_BUFFERMGR_t* pBufferMgr = NULL;
	PrintEnter;
	UNUSED_PARAM(val2);
	if(g_hSstr == NULL)
	{
		PrintError("%s(%d) : Error> SSTR HANDLE is NULL.\n",__FUNCTION__,__LINE__);
		return VFIO_ERROR_IN_DEVICE;
	}
	pSstr = __getSSTR(g_hSstr);
	if(pSstr->hCtrl == NULL)
	{
		PrintError("%s(%d) Error> SSTR CONTROLLER is NULL. Check This !!\n"
		, __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}
	if ( cmd == CMD_CONTROL_SEEK )
	{
		val *= 10000000; /* micro seconds? */
		VK_SEM_Get(pSstr->ulLiveSema);
		if(P_STREAM_SSTR_IsValidTimeForSeek(pSstr, val))
		{
			pBufferMgr = SSTR_CONTROLLER_GetBuffer(pSstr->hCtrl);
			if(pBufferMgr == NULL)
			{
				PrintError("%s(%d) Error> pBufferMgr is NULL\n", __FUNCTION__, __LINE__);
				VK_SEM_Release(pSstr->ulLiveSema);
				return DI_ERR_ERROR;
			}
			SSTR_BUFFERMGR_SetReadLock(pBufferMgr, TRUE);
			nRet = P_STREAM_SSTR_SetTimeForSeek(pSstr, val);
			if(nRet != DI_ERR_OK)
			{
				VK_SEM_Release(pSstr->ulLiveSema);
				return nRet;
			}
			nRet = P_STREAM_SSTR_SetRequestSeek(pSstr, TRUE);
		}
		else
		{
			VK_SEM_Release(pSstr->ulLiveSema);
			return DI_ERR_ERROR;
		}
		VK_SEM_Release(pSstr->ulLiveSema);
	}
	else if ( cmd == CMD_CONTROL_PAUSE )
	{
		P_STREAM_SSTR_SetPause(pSstr,TRUE);
	}
	else if ( cmd == CMD_CONTROL_RESUME )
	{
		P_STREAM_SSTR_SetPause(pSstr,FALSE);
	}
	else if ( cmd == CMD_CONTROL_SPEED )
	{
		switch(val)
		{
			case DI_MEDIA_TRICK_PAUSE :
				P_STREAM_SSTR_SetPause(pSstr,TRUE);
				break;
			case DI_MEDIA_TRICK_1FFSTEP :
			default :
				P_STREAM_SSTR_SetPause(pSstr,FALSE);
				break;
		}
		/* not supported yet */
	}
	PrintExit;
	return nRet;
}

/**
 * @brief       "Open" VFIO
 * @param	tHandle : VFIO Handle
 * @return     	VFIO err code
 * @date        2011/02/19
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/
int P_STREAM_SSTR_Open  (const HINT8 *path, const HINT8 *mode, VFIO_Handle_t *ptHandle )
{
	HINT32 vRet = VFIO_ERROR_SUCCESS;
	PrintEnter;
	P_STREAM_SSTR_OpenFromNexus(path,*mode,ptHandle);
	PrintExit;
	return vRet;
}

/**
 * @brief       "Close" VFIO
 * @param	tHandle : VFIO Handle
 * @return     	VFIO err code
 * @date        2011/02/19
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/
int P_STREAM_SSTR_Close (VFIO_Handle_t tHandle)
{
	HINT32 vRet = VFIO_ERROR_SUCCESS;
	SSTR_t* pSstr = NULL;
	UNUSED_PARAM(tHandle);
	PrintEnter;
	/* Getting IPTUNER handle from VFIO Handle */
	if(g_hSstr == NULL)
	{
		PrintError("%s(%d) : Error> SSTR Controller is NULL.\n",__FUNCTION__,__LINE__);
		return VFIO_ERROR_IN_DEVICE;
	}
	pSstr = __getSSTR(g_hSstr);
	if(pSstr->hCtrl == NULL)
	{
		PrintError("%s(%d) Error> SSTR CONTROLLER is NULL. Check This !!\n"
		, __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}
	/* Now we just get the Stream handler */
//	(void)VK_SEM_Get(s_ulSstrSemId);
	pSstr->bImmediatelyExit = TRUE;
//	(void)VK_SEM_Release (s_ulSstrSemId);
//	(void)VK_SEM_Destroy(s_ulSstrSemId);
//	s_ulSstrSemId = 0;
	PrintExit;
	return vRet;
}

/**
 * @brief       "Read" From User Application and bmedia
 * @param	tHandle : VFIO Handle
 * @param       ptr : pointer of buffer
 * @param	size : size of memory for read
 * @param	nmemb : size of memory for read
 * @param	ptReadSize : size of byte
 * @return     	VFIO err code
 * @date        2011/02/19
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/
int P_STREAM_SSTR_Read (VFIO_Handle_t tHandle, void *ptr, size_t size, size_t nmemb, ssize_t *ptReadSize)
{
	HBOOL bIsEOS = FALSE;
	size_t sz_Try = size*nmemb;
	size_t cur=0;
	size_t sz_Read=0;
	size_t sz_Temp=0;
	int nRead=0;
	int ret = VFIO_ERROR_SUCCESS;
	SSTR_t* pSstr = NULL;
	SSTR_BUFFERMGR_t* pBufferMgr = NULL;
#if SSTR_CONFIG_CHECK_UNDERFLOW
	HUINT32 uPrvBufferDepth = 0;
	HUINT32 uCurBufferDepth = 0;
	HUINT32 uCheckCount = 0;
	HBOOL IsTimerStarted = FALSE;
#endif

	UNUSED_PARAM(tHandle);
	PrintEnter;
	if(g_hSstr == NULL)
	{
		PrintError("%s(%d) : Error> SSTR Controller is NULL.\n",__FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}
	pSstr = __getSSTR(g_hSstr);
	if(pSstr->hCtrl == NULL)
	{
		PrintError("%s(%d) Error> SSTR CONTROLLER is NULL. Check This !!\n"
		, __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}
	/* Now we just get the Stream handler */
//	PrintDebug("%s(%d) : Debug> Try to Read\n",__FUNCTION__,__LINE__);
	pBufferMgr = SSTR_CONTROLLER_GetBuffer(pSstr->hCtrl);
	VK_MEM_Memset(ptr,0x00,sz_Try);
	sz_Read = sz_Try;
/*
	if(pSstr->bImmediatelyExit)
	{
		pSstr->bImmediatelyExit = FALSE;
	}
*/
	while(sz_Read > 0)
	{
		if(pSstr->bImmediatelyExit)
		{
			pSstr->bImmediatelyExit = FALSE;
			IsTimerStarted = FALSE;
			PrintDebug("%s(%d) : Debug> Forced Exit.\n",__FUNCTION__,__LINE__);
			break;
		}
		if(P_STREAM_SSTR_IsSeekRequested(pSstr))
		{
			cur = 0;
		}
		if(SSTR_BUFFERMGR_GetReadableSize(pBufferMgr) < sz_Read)
		{
			if(g_ePreBufferingTaskState > DI_IPTUNER_SSTR_BUFFERING_TASK_STATE_STARTED) /* || SSTR_BUFFERMGR_GetWritableSize(pBufferMgr) <= 0)*/
			{
				sz_Temp = SSTR_BUFFERMGR_GetReadableSize(pBufferMgr);

				if(sz_Temp == 0)
				{
					if(g_ePreBufferingTaskState == DI_IPTUNER_SSTR_BUFFERING_TASK_STATE_WAITING)
					{
						if(SSTR_CONTROLLER_IsLive(pSstr->hCtrl) != TRUE)
						{
							break;
						}
						else
						{
							VK_TASK_Sleep(10);
							// PrintDebug("%s(%d) : Debug> BufferUnderrun : %"PRId64"\n",__FUNCTION__,__LINE__,SSTR_BUFFERMGR_GetReadableSize(pBufferMgr) );
							continue;
						}
					}
					else if(g_ePreBufferingTaskState >= DI_IPTUNER_SSTR_BUFFERING_TASK_STATE_KILLING)
					{
						break;
					}
				}

				if(sz_Temp < sz_Read)
				{
					sz_Read = sz_Temp;
					bIsEOS = TRUE;
				}
			}
			else
			{
#if SSTR_CONFIG_CHECK_UNDERFLOW
				uPrvBufferDepth = uCurBufferDepth;
				if(P_STREAM_SSTR_IsBuffering(uPrvBufferDepth, &uCurBufferDepth) && (uCheckCount > 5))
				{
					if(!IsTimerStarted)
					{
						// To increase buffering count and start buffering timer.
						pSstr->stDInfo.uBufferingCount++;
						P_STREAM_SSTR_BufferingTimerStart(&pSstr->stDInfo);
#ifdef DEBUG_TIMESTAMP
						PrintDebug("\t\033[01m\033[32m START Timer\033[0m\n");
#endif
					}
					IsTimerStarted = TRUE;
				}
				else
				{
					uCheckCount++;
				}
#endif
				VK_TASK_Sleep(10);
//				PrintDebug("%s(%d) : Debug> BufferUnderrun : %"PRId64"\n",__FUNCTION__,__LINE__,SSTR_BUFFERMGR_GetReadableSize(pBufferMgr) );
				continue;
			}
		}

#if SSTR_CONFIG_CHECK_UNDERFLOW
		uPrvBufferDepth = uCurBufferDepth = 0;
		uCheckCount = 0;
#endif
		nRead = SSTR_BUFFERMGR_Read(pBufferMgr, (HUINT8 *)ptr + cur, sz_Read);
		if(nRead == 0)
		{
			if(g_ePreBufferingTaskState > DI_IPTUNER_SSTR_BUFFERING_TASK_STATE_STARTED)
			{
				break;
			}
			else
			{
				VK_TASK_Sleep(10);
				continue;
			}
		}
		cur+=nRead;
		sz_Read-=nRead;
		// TODO: must add Break Condition ex) End of Stream, Stop to play and so on
	}
	if(!bIsEOS)
	{
		*ptReadSize = cur;
	}
	else
	{
		*ptReadSize = sz_Try;
	}

#if SSTR_CONFIG_CHECK_UNDERFLOW
	if(IsTimerStarted)
	{
		// To stop buffering timer.
		P_STREAM_SSTR_SetBufferingTime(&pSstr->stDInfo);
		IsTimerStarted = FALSE;
	}
#endif
	PrintDebug("%s(%d) : Debug> Readsize : %d \n",__FUNCTION__,__LINE__,cur);
	PrintExit;
	return ret;
}

/**
 * @brief       "Seek" From User Application and bmedia
 * @param	tHandle : VFIO Handle
 * @param       offset : offset for seeking
 * @param	whence : whence
 * @return     	VFIO err code
 * @date        2011/02/19
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/
/** This Function called by bmedia probe only.*/
int P_STREAM_SSTR_Seek (VFIO_Handle_t tHandle, off_t offset, int whence)
{
	HINT32 nRet = VFIO_ERROR_SUCCESS;
	nRet = P_STREAM_SSTR_SeekForNexus(tHandle,offset,whence);
	//PrintExit;
	return nRet;
}

/**
 * @brief       "Tell" From User Application and bmedia
 * @param	tHandle : VFIO Handle
 * @param       plCurOffset : current offset
 * @return     	VFIO err code
 * @date        2011/02/19
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/
int P_STREAM_SSTR_Tell (VFIO_Handle_t tHandle, off_t *plCurOffset)
{
	SSTR_t* pSstr = NULL;
	PrintEnter;
	if(g_hSstr == NULL)
	{
		tHandle.ucAvailable = 0;
		PrintError("%s(%d) : Error> SSTR HANDLE is NULL.\n",__FUNCTION__,__LINE__);
		return VFIO_ERROR_IN_DEVICE;
	}
	pSstr = __getSSTR(g_hSstr);
	if(pSstr->hCtrl == NULL)
	{
		PrintError("%s(%d) Error> SSTR CONTROLLER is NULL. Check This !!\n"
		, __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}
	// TODO: must implement for ABR
	*plCurOffset = pSstr->ullVirtureOffset;
	return VFIO_ERROR_SUCCESS;
}

/**
 * @brief       "IsEOF" From User Application and bmedia
 * @param	tHandle : VFIO Handle
 * @return     	bool
 * @date        2011/02/19
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/

/** if EOF is true, must be return -1 like feof*/
int P_STREAM_SSTR_IsEOF (VFIO_Handle_t tHandle)
{
	HINT32 nRet = VFIO_ERROR_SUCCESS;
	PrintEnter;
	if(g_hSstr == NULL)
	{
		tHandle.ucAvailable = 0;
		PrintError("%s(%d) : Error> SSTR HANDLE is NULL.\n",__FUNCTION__,__LINE__);
		return VFIO_ERROR_IN_DEVICE;
	}
	/* Now we just get the Stream handler */
	return nRet;
}

/**
 * @brief       register callback
 * @return      void
 * @date        2011/02/19
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/
void DI_STREAM_SSTR_RegisterCallback ()
{
	VFIO_Callback_t stCBFunction;

	/* 1. VFIO에 callback 함수 등록 */
	(void)VK_memset (&stCBFunction, 0x0, sizeof(VFIO_Callback_t));

	stCBFunction.Open 			= (VFIOOpenCB)	P_STREAM_SSTR_Open;
	stCBFunction.Close			= (VFIOCloseCB)	P_STREAM_SSTR_Close;
	stCBFunction.Read			= (VFIOReadCB)	P_STREAM_SSTR_Read;
	stCBFunction.Seek			= (VFIOSeekCB) 	P_STREAM_SSTR_Seek;
	stCBFunction.Tell			= (VFIOTellCB) 	P_STREAM_SSTR_Tell;
	stCBFunction.IsEOF			= (VFIOIsEOFCB) P_STREAM_SSTR_IsEOF;
	stCBFunction.OpenNexus		= (VFIOOpenFromNexusCB)	 P_STREAM_SSTR_OpenFromNexus;
	stCBFunction.SeekNexus		= (VFIOSeekForNexusCB) 	 P_STREAM_SSTR_SeekForNexus;
	stCBFunction.BoundsNexus	= (VFIOBoundsForNexusCB) P_STREAM_SSTR_BoundsForNexus;

	(void) VFIO_RegisterCallback (VFIO_CALLBACK_TYPE_STREAMING_SSTR, stCBFunction);

	PrintExit;
	return;
}

/**
 * @brief       unregister callback
 * @return      void
 * @date        2011/02/19
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/
void DI_STREAM_SSTR_UnregisterCallback ()
{
	PrintEnter;
	(void)VFIO_UnregisterCallback (VFIO_CALLBACK_TYPE_STREAMING_SSTR);
	PrintExit;
	return;
}

#ifdef __SSTR_NOT_SUPPORTED_YET__ /* not supported yet */
void DI_STREAM_SSTR_RegisterEventCallback (DI_IPTUNER_AdaptiveCallback_t *pCallback)
{
	VK_MEM_Memcpy(&stAdaptiveCallback,pCallback,sizeof(DI_IPTUNER_AdaptiveCallback_t));
	PrintExit;
	return;
}

/**
 * @brief       unregister callback
 * @return      void
 * @date        2011/02/19
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/
void DI_STREAM_SSTR_UnregisterEventCallback ()
{
	VK_MEM_Memset(&stAdaptiveCallback,0x00,sizeof(DI_IPTUNER_AdaptiveCallback_t));
	PrintExit;
	return;
}
#endif

void P_STREAM_SSTR_TimerStart(tDownloadInfo* pInfo)
{
	if(pInfo == NULL)
	{
		PrintError("[%s:%d]Info structure is NULL\n", __FUNCTION__, __LINE__);
		return;
	}

	pInfo->ulStartTime = VK_TIMER_GetSystemTick();
}

void P_STREAM_SSTR_TimerStop(tDownloadInfo* pInfo)
{
	if(pInfo == NULL)
	{
		PrintError("[%s:%d]Info structure is NULL\n", __FUNCTION__, __LINE__);
		return;
	}

	pInfo->ulEndTime = VK_TIMER_GetSystemTick();
}

void P_STREAM_SSTR_TimerSetStartingTime(tDownloadInfo* pInfo)
{
	HULONG ulTime = 0;

	if(pInfo == NULL)
	{
		PrintError("[%s:%d]Info structure is NULL\n", __FUNCTION__, __LINE__);
		return;
	}

	if(pInfo->IsNotFirst)
	{
		return;
	}

	pInfo->ulEndTime = VK_TIMER_GetSystemTick();

	if(pInfo->ulStartingTime == 0 || pInfo->ulEndTime == 0)
	{
		PrintError("[%s:%d]]Start or End Time is Zero\n", __FUNCTION__, __LINE__);
		return;
	}

	// To calculate download time for current packet
	ulTime = pInfo->ulEndTime - pInfo->ulStartingTime;
	if(ulTime == 0)
		ulTime = 1;

	// To calculate average time and bandwidth with latest three packet.
	pInfo->ulStartingTime = ulTime;
	pInfo->IsNotFirst = TRUE;
#ifdef DEBUG_TIMESTAMP
	PrintDebug("Starting Time : %lu ms\n", ulTime);
#endif
}

void P_STREAM_SSTR_TimerSetLoadingTime(tDownloadInfo* pInfo)
{
	HULONG ulTime = 0;

	if(pInfo == NULL)
	{
		PrintError("[%s:%d]Info structure is NULL\n", __FUNCTION__, __LINE__);
		return;
	}

	pInfo->ulEndTime = VK_TIMER_GetSystemTick();

	if(pInfo->ulStartTime == 0 || pInfo->ulEndTime == 0)
	{
		PrintError("[%s:%d]]Start or End Time is Zero\n", __FUNCTION__, __LINE__);
		return;
	}

	// To calculate download time for current packet
	ulTime = pInfo->ulEndTime - pInfo->ulStartTime;
	if(ulTime == 0)
		ulTime = 1;

	// To calculate average time and bandwidth with latest three packet.
	pInfo->ulLoadingTime = ulTime;
#ifdef DEBUG_TIMESTAMP
	PrintDebug("Loading Time : %lu ms\n", ulTime);
#endif
}

void P_STREAM_SSTR_TimerSetDownloadTime(tDownloadInfo* pInfo, HINT32 nSize)
{
	HULONG ulTime = 0;
	HFLOAT64 fSize = (HFLOAT64)nSize;
	HFLOAT64 fTimeToSec = 0;
	HFLOAT64 fBps = 0;
	HINT32 i = 0, nTimeCount = 0, nBandwidthCount = 0;
	HFLOAT64 fAveBps = 0;
	HULONG ulAveTime = 0;

	if(pInfo == NULL)
	{
		PrintError("[%s:%d]Info structure is NULL\n", __FUNCTION__, __LINE__);
		return;
	}

	pInfo->ulEndTime = VK_TIMER_GetSystemTick();

	if(pInfo->ulStartTime == 0 || pInfo->ulEndTime == 0)
	{
		PrintError("[%s:%d]]Start or End Time is Zero\n", __FUNCTION__, __LINE__);
		return;
	}

	// To calculate download time for current packet
	ulTime = pInfo->ulEndTime - pInfo->ulStartTime;
	if(ulTime == 0)
		ulTime = 1;

	// ms -> sec
	fTimeToSec = (HFLOAT64)((HFLOAT64)ulTime / (HFLOAT64)1000);
	// bandwidth for current packet, Byte per sec
	fBps = fSize / fTimeToSec;
	if(fBps == 0 || fBps > 10000000.00)
	{
		fBps  = 10000000.00;
	}

	pInfo->ulTimeArray[pInfo->nDownloadCount] = ulTime;
	pInfo->fBandwidthArray[pInfo->nDownloadCount] = fBps;


	for(i=0 ; i<DOWNLOAD_CHECK_PERIOD ; i++)
	{
		if(pInfo->ulTimeArray[i] != 0)
		{
			ulAveTime += pInfo->ulTimeArray[i];
			nTimeCount++;
		}
		if(pInfo->fBandwidthArray[i] != 0)
		{
			fAveBps += pInfo->fBandwidthArray[i];
			nBandwidthCount++;
		}
	}

	// To calculate average time and bandwidth with latest three packet.
	pInfo->ulDownloadTime = (HULONG)((HULONG)ulAveTime / (HULONG)nTimeCount);
	pInfo->fBandwidth = (HFLOAT64)((HFLOAT64)fAveBps / (HFLOAT64)nBandwidthCount);
	if(pInfo->fMaxBandwidth < pInfo->fBandwidth)
		pInfo->fMaxBandwidth = pInfo->fBandwidth;

	// To check packet count only for latest three packet
	pInfo->nDownloadCount++;
	if(pInfo->nDownloadCount >= DOWNLOAD_CHECK_PERIOD)
		pInfo->nDownloadCount = 0;
#ifdef DEBUG_TIMESTAMP
	PrintDebug("%d. %6lu ms|%10.2lf Bps(%6.2lf KBps)|AVE%6lu ms|%10.2lf Bps(%6.2lf KBps)\n", pInfo->nDownloadCount, ulTime, fBps, fBps/ 1024, pInfo->ulDownloadTime, pInfo->fBandwidth, pInfo->fBandwidth/ 1024);
#endif
}

HULONG DI_STREAM_SSTR_TimerGetStartingTime(void)
{
	SSTR_t* pSstr = NULL;

	if(g_hSstr == NULL)
	{
		PrintError("%s(%d) : Error> SSTR Controller is NULL.\n",__FUNCTION__,__LINE__);
		return 0;
	}
	pSstr = __getSSTR(g_hSstr);

	if(!pSstr->stDInfo.IsNotFirst)
		return 0;

	return pSstr->stDInfo.ulStartingTime;
}

HULONG DI_STREAM_SSTR_TimerGetLoadingTime(void)
{
	SSTR_t* pSstr = NULL;

	if(g_hSstr == NULL)
	{
		PrintError("%s(%d) : Error> SSTR Controller is NULL.\n",__FUNCTION__,__LINE__);
		return 0;
	}
	pSstr = __getSSTR(g_hSstr);

	return pSstr->stDInfo.ulLoadingTime;
}

HUINT32 DI_STREAM_SSTR_TimerGetBandwidth(void)
{
	SSTR_t* pSstr = NULL;
	HUINT32 uBandwidth = 0;

	if(g_hSstr == NULL)
	{
		PrintError("%s(%d) : Error> SSTR Controller is NULL.\n",__FUNCTION__,__LINE__);
		return 0;
	}
	pSstr = __getSSTR(g_hSstr);

	uBandwidth = (HUINT32)(pSstr->stDInfo.fBandwidth / 1024);

	return uBandwidth;
}

HUINT32 DI_STREAM_SSTR_TimerGetMaxBandwidth(void)
{
	SSTR_t* pSstr = NULL;
	HUINT32 uBandwidth = 0;

	if(g_hSstr == NULL)
	{
		PrintError("%s(%d) : Error> SSTR Controller is NULL.\n",__FUNCTION__,__LINE__);
		return 0;
	}
	pSstr = __getSSTR(g_hSstr);

	uBandwidth = (HUINT32)(pSstr->stDInfo.fMaxBandwidth / 1024);

	return uBandwidth;
}

HUINT32 DI_STREAM_SSTR_GetCurFragmentIndex(void)
{
	SSTR_t* pSstr = NULL;

	if(g_hSstr == NULL)
	{
		PrintError("%s(%d) : Error> SSTR Controller is NULL.\n",__FUNCTION__,__LINE__);
		return 0;
	}
	pSstr = __getSSTR(g_hSstr);

	return pSstr->stDInfo.uCurFragmentIndex;
}

HUINT64 DI_STREAM_SSTR_GetCurFragmentTime(void)
{
	SSTR_t* pSstr = NULL;

	if(g_hSstr == NULL)
	{
		PrintError("%s(%d) : Error> SSTR Controller is NULL.\n",__FUNCTION__,__LINE__);
		return 0;
	}
	pSstr = __getSSTR(g_hSstr);

	return pSstr->stDInfo.ullCurFragmentStartTime / 10000;
}

HUINT32 DI_STREAM_SSTR_GetCurBitrate(void)
{
	SSTR_t* pSstr = NULL;

	if(g_hSstr == NULL)
	{
		PrintError("%s(%d) : Error> SSTR Controller is NULL.\n",__FUNCTION__,__LINE__);
		return 0;
	}
	pSstr = __getSSTR(g_hSstr);

	if(pSstr->hCtrl == NULL)
	{
		PrintError("%s(%d) : Error> SSTR Controller is NULL.\n",__FUNCTION__,__LINE__);
		return 0;
	}

	return SSTR_CONTROLLER_GetCurBitrate(pSstr->hCtrl);
}

DI_ERR_CODE DI_STREAM_SSTR_GetCurFragmentUrl(char** pUrl)
{
	SSTR_t* pSstr = NULL;
	SSTR_STREAM_TYPE_e eType;
	SSTR_ERR nRet = 0;

	if(g_hSstr == NULL)
	{
		PrintError("%s(%d) : Error> SSTR Controller is NULL.\n",__FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}
	pSstr = __getSSTR(g_hSstr);

	if(pSstr->hCtrl == NULL)
	{
		PrintError("%s(%d) : Error> SSTR Controller is NULL.\n",__FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}

	eType = SSTR_CONTROLLER_GetCurStreamType(pSstr->hCtrl);
	nRet = SSTR_CONTROLLER_GetFragmentUrl(pSstr->hCtrl,eType,&pSstr->tAVUrl);
	if(nRet != SSTR_ERR_OK)
	{
		PrintError("%s(%d) : Error> DI_STREAM_SSTR_GetCurFragmentUrl.\n",__FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}

	*pUrl = pSstr->tAVUrl.url;

	return DI_ERR_OK;
}

#define _____SSTR_CONFIG_CHECK_UNDERFLOW_____
#if SSTR_CONFIG_CHECK_UNDERFLOW
HBOOL P_STREAM_SSTR_IsBuffering(HUINT32 uPrvBufferDepth, HUINT32* pCurBufferDepth)
{
	DI_ERR_CODE eErr = 0;
	SSTR_t* pSstr = NULL;
	SSTR_STREAM_TYPE_e eType;

	HUINT32 uCurAVIndex = 0;
	HUINT32 uFragmentId = 0;
	HUINT32 uTotalFragmentCount =0 ;

	HUINT32 uVideoBufferDepth = 0;
//	HUINT32 uAudioBufferDepth = 0;
//	HUINT32 uPumpBufferDepth = 0;
	HUINT32 uBufferDepth = 0;

	if(pCurBufferDepth == NULL)
	{
		PrintError("%s(%d) : Error> Parameter is NULL.\n",__FUNCTION__,__LINE__);
		return FALSE;
	}

	if(g_hSstr == NULL)
	{
		PrintError("%s(%d) : Error> SSTR Controller is NULL.\n",__FUNCTION__,__LINE__);
		return FALSE;
	}
	pSstr = __getSSTR(g_hSstr);

	if(pSstr->hCtrl == NULL)
	{
		PrintError("%s(%d) : Error> SSTR Controller is NULL.\n",__FUNCTION__,__LINE__);
		return FALSE;
	}
	eType = SSTR_CONTROLLER_GetCurStreamType(pSstr->hCtrl);
	uCurAVIndex = SSTR_CONTROLLER_GetCurAVStream(pSstr->hCtrl,eType);

	// 5초 이후의 fragment에 대해서만 체크.
	uFragmentId = SSTR_CONTROLLER_GetCurFragment(pSstr->hCtrl, uCurAVIndex);
	if(uFragmentId < 5)
	{
		PrintDebug("%s(%d) : Error> SSTR : Fragment ID is lower than 2.\n",__FUNCTION__,__LINE__);
		return FALSE;
	}

	uTotalFragmentCount = SSTR_CONTROLLER_GetTotalFragment(pSstr->hCtrl, uCurAVIndex);
	if (uFragmentId >= uTotalFragmentCount - 1)
	{
		PrintDebug("%s(%d) : Error > SSTR : Skip the last Fragment (id=%d, total=%d).\n", __FUNCTION__, __LINE__, uFragmentId, uTotalFragmentCount);
		return FALSE;
	}

	eErr = DI_MEDIA_GetVideoBufferDepth(&uVideoBufferDepth);
	if(eErr != DI_ERR_OK)
	{
		PrintError("%s(%d) : Error> DI_MEDIA_GetVideoBufferDepth.\n",__FUNCTION__,__LINE__);
		return FALSE;
	}
#if 0
	eErr = DI_MEDIA_GetAudioBufferDepth(&uAudioBufferDepth);
	if(eErr != DI_ERR_OK)
	{
		PrintError("%s(%d) : Error> DI_MEDIA_GetAudioBufferDepth.\n",__FUNCTION__,__LINE__);
		return FALSE;
	}

	eErr = DI_MEDIA_GetPumpBufferDepth(&uPumpBufferDepth);
	if(eErr != DI_ERR_OK)
	{
		PrintError("%s(%d) : Error> DI_MEDIA_GetPumpBufferDepth.\n",__FUNCTION__,__LINE__);
		return FALSE;
	}

	//uBufferDepth = uVideoBufferDepth + uAudioBufferDepth;
#endif
	uBufferDepth = uVideoBufferDepth;
#ifdef DEBUG_TIMESTAMP
	PrintDebug("\t\033[01m\033[33m FID(%d) decoder buffer status..V(%d) A(%d) P(%d)\033[0m\n", uFragmentId, uVideoBufferDepth, uAudioBufferDepth, uPumpBufferDepth);PrintDebug("\t\033[01m\033[33m Check for decoder buffer status..V(%d) A(%d) P(%d)\033[0m\n", uVideoBufferDepth, uAudioBufferDepth, uPumpBufferDepth);
#endif
	*pCurBufferDepth = uBufferDepth;
	if(uPrvBufferDepth == uBufferDepth)
	{
#ifdef DEBUG_TIMESTAMP
		PrintDebug("\t\033[01m\033[31m Now Buffering...\033[0m\n");
#endif
		return TRUE;
	}

	return FALSE;

}

void P_STREAM_SSTR_BufferingTimerStart(tDownloadInfo* pInfo)
{
	if(pInfo == NULL)
	{
		PrintError("[%s:%d]Info structure is NULL\n", __FUNCTION__, __LINE__);
		return;
	}

	pInfo->ulBufferingStartTime = VK_TIMER_GetSystemTick();
}

void P_STREAM_SSTR_BufferingTimerStop(tDownloadInfo* pInfo)
{
	if(pInfo == NULL)
	{
		PrintError("[%s:%d]Info structure is NULL\n", __FUNCTION__, __LINE__);
		return;
	}

	pInfo->ulBufferingEndTime = VK_TIMER_GetSystemTick();
}

void P_STREAM_SSTR_SetBufferingTime(tDownloadInfo* pInfo)
{
	HULONG ulTime = 0;

	if(pInfo == NULL)
	{
		PrintError("[%s:%d]Info structure is NULL\n", __FUNCTION__, __LINE__);
		return;
	}

	pInfo->ulBufferingEndTime = VK_TIMER_GetSystemTick();

	if(pInfo->ulBufferingStartTime == 0 || pInfo->ulBufferingEndTime == 0)
	{
		PrintError("[%s:%d]]Start or End Time is Zero\n", __FUNCTION__, __LINE__);
		return;
	}

	// To calculate download time for current packet
	ulTime = pInfo->ulBufferingEndTime - pInfo->ulBufferingStartTime;
	if(ulTime == 0)
		ulTime = 1;

	pInfo->ullTotalBufferingTime = pInfo->ullTotalBufferingTime + (HUINT64)ulTime;
#ifdef DEBUG_TIMESTAMP
	PrintDebug("\t\033[01m\033[32m SET Timer Count:(%d) Time:(%lu)ms, Total: (%llu)ms\033[0m\n", pInfo->uBufferingCount, ulTime, pInfo->ullTotalBufferingTime);
#endif
}

HUINT32 DI_STREAM_SSTR_GetBufferingCount()
{
	SSTR_t* pSstr = NULL;

	if(g_hSstr == NULL)
	{
		PrintError("%s(%d) : Error> SSTR Controller is NULL.\n",__FUNCTION__,__LINE__);
		return 0;
	}
	pSstr = __getSSTR(g_hSstr);

	return pSstr->stDInfo.uBufferingCount;
}

HUINT64 DI_STREAM_SSTR_GetBufferingTime()
{
	SSTR_t* pSstr = NULL;

	if(g_hSstr == NULL)
	{
		PrintError("%s(%d) : Error> SSTR Controller is NULL.\n",__FUNCTION__,__LINE__);
		return 0;
	}
	pSstr = __getSSTR(g_hSstr);

	return pSstr->stDInfo.ullTotalBufferingTime;
}
#endif

/*@}*/
