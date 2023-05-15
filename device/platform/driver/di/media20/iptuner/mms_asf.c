/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   mms_asf.c  $
 * Version:		2.0 (for Media 2.0 with 40nm)
 * Original Author:	Jin Lee
 * Current Author:	$Author: jlee@humaxdigital.com $
 * Date:			$Date: 2012.07.30
 * File Description:	     MMS - ASF Controller
 * Module:
 * Remarks:
 */

/**
 * @defgroup MMS_ASF_CONTROLLER
 * @brief MMS ASF Controller.
 */

/**
 * @author Jin Lee (jlee@humaxdigital.com)
 * @date 30 Jul 2012
 */

/**
 * @brief MMS ASF Controller.
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
 * @file mms_asf.c
 */

/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
/* Start Including Header Files */
#include "di_iptuner_config.h"
#include "di_iptuner_common.h"

#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/types.h>

#include "htype.h"
#include "di_uart.h"
#include "mms_asf.h"
#include "mms_buffer.h"

#include "rtsp_lib.h"
/* End Including Headers */

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
/* Start #define */
//#define MMS_ASF_DEBUG
//#define MMS_ASF_CRITICAL_DEBUG
//
#ifdef IPTUNER_ALL_DEBUG

#ifdef MMS_ASF_DEBUG
#define PrintData		DI_UART_Print
#define PrintDebug		DI_UART_Print
#define PrintEnter		DI_UART_Print("(+)%s\n", __FUNCTION__)
#define PrintExit		DI_UART_Print("(-)%s\n", __FUNCTION__)
#define PrintError		DI_UART_Print
#else
#define PrintData		while (0) ((int (*)(char *, ...)) 0)
#define PrintDebug		while (0) ((int (*)(char *, ...)) 0)
#define PrintEnter
#define PrintExit
#define PrintError		DI_UART_Print
#endif

#else

#define PrintData		while (0) ((int (*)(char *, ...)) 0)
#define PrintDebug		while (0) ((int (*)(char *, ...)) 0)
#define PrintError		while (0) ((int (*)(char *, ...)) 0)
#define PrintEnter
#define PrintExit
#endif /* END DEBUG DEFINE */


#ifdef MMS_ASF_CRITICAL_DEBUG
#define PrintCriticalDebug	  DI_UART_Print
#else
#define PrintCriticalDebug	  while (0) ((int (*)(char *, ...)) 0)
#endif

#define UNUSED_PARAM(x)  ((void)x)

#define _MMS_ASF_RAND_BLOCK_SIZE 16
#define SUPPORT_ANALYZE_ASF_AUDIO 0
#define SUPPORT_ANALYZE_ASF_VIDEO 0
/* End #define */

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
/* Start typedef */

#if SUPPORT_ANALYZE_ASF_VIDEO
typedef struct
{
	HINT32 headerSize;
	HINT32 biWidth;		/* initialize to avoid compiler warning */
	HINT32 biHeight;
	HINT16 biPlanes;
	HINT16 biBitCount;
	HINT32 biCompression;
	HINT32 biSizeImage;
	HINT32 biXPelsPerMeter;
	HINT32 biYPelsPerMeter;
	HINT32 biClrUsed;
	HINT32 biClrImportant;
} ASF_BMPINFOHEADER_t;
#endif
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
/* End static variable */


/*******************************************************************/
/************************ static funtions **************************/
/*******************************************************************/
/* Start static function prototype */
/* End static function prototype */
/*
  Asf Guid
*/
const HUINT8 Asf_Guid_Stream_Header[MMS_ASF_GUID_LENGTH] =
{
 0x91, 0x07, 0xdc, 0xb7, /* Guid Check1 */
 0xb7, 0xa9, /* Guid Check2 */
 0xcf, 0x11, /* Guid Check3 */
 0x8e, 0xe6, /* Guid Check4 */
 0x00, 0xc0, 0x0c, 0x20, 0x53, 0x65 /* Guid Check5 */
};

const HUINT8 Asf_Guid_Stream_Group[MMS_ASF_GUID_LENGTH] =
{
 0xce, 0x75, 0xf8, 0x7b, /* Guid Check1 */
 0x8d, 0x46, /* Guid Check2 */
 0xd1, 0x11, /* Guid Check3 */
 0x8d, 0x82, /* Guid Check4 */
 0x00, 0x60, 0x97, 0xc9, 0xa2, 0xb2 /* Guid Check5 */
};

const HUINT8 Asf_Guid_Ext_Stream_EmbedStream_Header[MMS_ASF_GUID_LENGTH] =
{
 0xe2, 0x65, 0xfb, 0x3a, /* Guid Check1 */
 0xef, 0x47, /* Guid Check2 */
 0xf2, 0x40, /* Guid Check3 */
 0xac, 0x2c, /* Guid Check4 */
 0x70, 0xa9, 0x0d, 0x71, 0xd3, 0x43 /* Guid Check5 */
};

const HUINT8 Asf_Guid_Ext_Stream_Header[MMS_ASF_GUID_LENGTH] =
{
 0xCB, 0xA5, 0xE6, 0x14, /* Guid Check1 */
 0x72, 0xC6, /* Guid Check2 */
 0x32, 0x43, /* Guid Check3 */
 0x83, 0x99, /* Guid Check4 */
 0xA9, 0x69, 0x52, 0x06, 0x5B, 0x5A /* Guid Check5 */
};

const HUINT8 Asf_Guid_Ext_Stream_Audio[MMS_ASF_GUID_LENGTH] =
{
 0x9d, 0x8c, 0x17, 0x31, /* Guid Check1 */
 0xe1, 0x03, /* Guid Check2 */
 0x28, 0x45, /* Guid Check3 */
 0xb5, 0x82, /* Guid Check4 */
 0x3d, 0xf9, 0xdb, 0x22, 0xf5, 0x03 /* Guid Check5 */
};

const HUINT8 Asf_Guid_File_Header[MMS_ASF_GUID_LENGTH] =
{
 0xa1, 0xdc, 0xab, 0x8c, /* Guid Check1 */
 0x47, 0xa9, /* Guid Check2 */
 0xcf, 0x11, /* Guid Check3 */
 0x8e, 0xe4, /* Guid Check4 */
 0x00, 0xc0, 0x0c, 0x20, 0x53, 0x65 /* Guid Check5 */
};

const HUINT8 Asf_Guid_Content_Desc[MMS_ASF_GUID_LENGTH] =
{
 0x33, 0x26, 0xb2, 0x75, /* Guid Check1 */
 0x8e, 0x66, /* Guid Check2 */
 0xcf, 0x11, /* Guid Check3 */
 0xa6, 0xd9, /* Guid Check4 */
 0x00, 0xaa, 0x00, 0x62, 0xce, 0x6c /* Guid Check5 */
};

const HUINT8 Asf_Guid_Header_Object[MMS_ASF_GUID_LENGTH] =
{
 0x30, 0x26, 0xb2, 0x75, /* Guid Check1 */
 0x8e, 0x66, /* Guid Check2 */
 0xcf, 0x11, /* Guid Check3 */
 0xa6, 0xd9, /* Guid Check4 */
 0x00, 0xaa, 0x00, 0x62, 0xce, 0x6c /* Guid Check5 */
};

const HUINT8 Asf_Guid_Data_Object[MMS_ASF_GUID_LENGTH] =
{
 0x36, 0x26, 0xb2, 0x75, /* Guid Check1 */
 0x8e, 0x66, /* Guid Check2 */
 0xcf, 0x11, /* Guid Check3 */
 0xa6, 0xd9, /* Guid Check4 */
 0x00, 0xaa, 0x00, 0x62, 0xce, 0x6c /* Guid Check5 */
};

static int max_idx(int s_count,int *s_rates,int bound);

static struct asf_streams_t *New_Asf_Streams(void);

static void Free_Asf_Streams(struct asf_streams_t *ast);

static int INT_ASF_CompareGUID ( const ASF_GUID_t *p_asfguid1, const ASF_GUID_t *p_asfguid2 )
{
	return((p_asfguid1->v1 == p_asfguid2->v1 &&	p_asfguid1->v2 == p_asfguid2->v2 &&
				p_asfguid1->v3 == p_asfguid2->v3 &&	p_asfguid1->v4[0] == p_asfguid2->v4[0] &&
				p_asfguid1->v4[1] == p_asfguid2->v4[1] &&	p_asfguid1->v4[2] == p_asfguid2->v4[2] &&
				p_asfguid1->v4[3] == p_asfguid2->v4[3] &&	p_asfguid1->v4[4] == p_asfguid2->v4[4] &&
				p_asfguid1->v4[5] == p_asfguid2->v4[5] &&	p_asfguid1->v4[6] == p_asfguid2->v4[6] &&
				p_asfguid1->v4[7] == p_asfguid2->v4[7] ) ? 1 : 0 );
}

HBOOL INT_ASF_GetRandomHex(char *pTarget, HUINT32 i_maxLen)
{
	FILE *fp = NULL;
	char *m_charset = "0123456789abcdef";
	HUINT32 idx = 0;
	char *pTmp = pTarget;
	HUINT32 i_tmpLen = i_maxLen;
	struct timeval tvTime;
	HINT32 ret = 0;

	fp = fopen("/dev/urandom","r");
	if (fp == NULL)
	{
		PrintError("%s(%d) Error> Can't open /dev/urandom\n",__FUNCTION__,__LINE__);
		/* Don't retry to /dev/random; This is unconfirmed */
		/* fp = fopen("/dev/random","r"); */
		goto func_error;
	}

	if (fp == NULL || (ret = fread(pTarget,i_maxLen,1,fp)) == 0)
	{
		PrintError("%s(%d) Error> Can't read /dev/urandom\n",__FUNCTION__,__LINE__);

		/* Use another random generation: time + pid + ... */
		pid_t tPid = getpid();

		gettimeofday(&tvTime,NULL);
		if (i_tmpLen >= sizeof(tvTime.tv_usec)) {
			VK_memcpy(pTmp,&tvTime.tv_usec,sizeof(tvTime.tv_usec));
			i_tmpLen -= sizeof(tvTime.tv_usec);
			pTmp += sizeof(tvTime.tv_usec);
		}
		if (i_tmpLen >= sizeof(tvTime.tv_sec)) {
			VK_memcpy(pTmp,&tvTime.tv_sec,sizeof(tvTime.tv_sec));
			i_tmpLen -= sizeof(tvTime.tv_sec);
			pTmp += sizeof(tvTime.tv_sec);
		}
		if (i_tmpLen >= sizeof(tPid)) {
			VK_memcpy(pTmp,&tPid,sizeof(tPid));
			i_tmpLen -= sizeof(tPid);
			pTmp += sizeof(tPid);
		}

		/* Because of CID #46692, can't use random(); Use fixed value if /dev/urandom is unavailable */
		/* xor with random() output */
		/*
		 * for (idx = 0; idx < i_maxLen; idx++)
		 * {
		 * 	pTarget[idx] ^= random();
		 * }
		 */
	}

	/* hex digits taking just 4 bits out of 8 */
	for (idx = 0; idx < i_maxLen; idx++)
	{
		pTarget[idx] = m_charset[pTarget[idx] & 0x0F];
	}

	if (fp != NULL)
	{
		fclose(fp);
		fp = NULL;
	}
	return TRUE;

func_error:
	if (fp != NULL)
	{
		fclose(fp);
		fp = NULL;
	}
	return FALSE;
}

void  ASF_GenerateGuid ( ASF_GUID_t *p_asfguid )
{
	int CheckNum=0;
	HUINT32 i_seed = 0;
	struct timeval tvTime;
	HBOOL bDevRandom = FALSE;
	unsigned char m_randBlock[_MMS_ASF_RAND_BLOCK_SIZE];
	HUINT64 i_rand = 0;

	if (bDevRandom == TRUE) /* Don't use this */
	{
		VK_memset(m_randBlock, 0x0, _MMS_ASF_RAND_BLOCK_SIZE);
		if (INT_ASF_GetRandomHex(m_randBlock, _MMS_ASF_RAND_BLOCK_SIZE) == TRUE)
		{
			i_rand = strtoll(m_randBlock, NULL, 16);
			p_asfguid->v1 = 0xbabac001;

			/* CID #12169: Reference following: */
			p_asfguid->v2 = ( i_rand << 16 ) / RAND_MAX;
			p_asfguid->v3 = ( i_rand << 16 ) / RAND_MAX;

			for( CheckNum = 0; CheckNum < 8; CheckNum++ )
			{
				p_asfguid->v4[CheckNum] = ( i_rand * 256 ) / RAND_MAX;
			}
			return;
		}
	}

	/* Getting seed number */
	if (gettimeofday(&tvTime, NULL) == 0)
	{
		i_seed = (HUINT32)(tvTime.tv_usec + tvTime.tv_sec);
	}
	else
	{
		i_seed = (HUINT32)time(NULL);
	}

#ifdef __DONT_USE_THIS_RAND_FUNCTION__ /* Because of CID #12169 issue, can't use rand() or random() */
  #if 0 // TODO : junsy TEST
	srand( 34 & 0xffffffff );
  #else
	srandom((unsigned int)i_seed); /* CID #12169 */
  #endif

	p_asfguid->v1 = 0xbabac001;

	/* CID #12169: Reference following: */
	/* https://www.securecoding.cert.org/confluence/display/seccode/MSC30-C.+Do+not+use+the+rand()+function+for+generating+pseudorandom+numbers */
	p_asfguid->v2 = ( (HUINT64)random() << 16 ) / RAND_MAX; /* CID #12169 */
	p_asfguid->v3 = ( (HUINT64)random() << 16 ) / RAND_MAX; /* RAND_MAX: Get upper 2 bytes */

	for( CheckNum = 0; CheckNum < 8; CheckNum++ )
	{
		p_asfguid->v4[CheckNum] = ( (HUINT64)random() * 256 ) / RAND_MAX; /* RAND_MAX: Get upper 1 bytes */
	}
#else
	/*
	 * But, because the blocking issue is reported at usage of /dev/urandom or /dev/random,
	 * We will use fixed value before the this confirmation.
	 * Fixed value is OK; Reference the FFMPEG:send_startup_packet(MMSTContext *mmst)
     *   - SubscriberName is defined in MS specification linked below.
     *   - The guid value can be any valid value
     */
	p_asfguid->v1 = 0xbabac001;
	p_asfguid->v2 = 0xA661; /* 7E667F5D-A661-495E-A512-F55686DDA178 */
	p_asfguid->v3 = 0x495E;
	for( CheckNum = 0; CheckNum < 8; CheckNum++ )
	{
		p_asfguid->v4[CheckNum] = CheckNum;
	}
#endif

	return;
}

void  ASF_ParseHeader ( ASF_HEADER_t *hdr, HUINT8 *p_header, int i_header )
{
	var_buffer_t	tBuffer;
	ASF_GUID_t		tAsfGuid;
	HUINT64	i_size=0;
	int		 CheckNum=0;
#if SUPPORT_ANALYZE_ASF_VIDEO	/* Video Stream Info */
	HINT16 EncodedImageWidth=0;
	HINT16 EncodedImageHeight=0;
	HINT32 ReservedFlags=0;
	HINT32 FormatDataSize=0;
	ASF_BMPINFOHEADER_t FormatData;
#endif

#if SUPPORT_ANALYZE_ASF_AUDIO	/* Audio Stream Info */
	HINT16 AudioCodecId=0;
	HINT16 ChannelNumber=0;
	HINT32 SampleRate=0;
	HINT32 Bitrate=0;
	HINT16 BlockAlignment=0;
	HINT16 BitperSample=0;
	HINT16 CodecDataSize=0;
#endif

	PrintEnter;

#if SUPPORT_ANALYZE_ASF_VIDEO	/* Video Stream Info */
	VK_MEM_Memset(&FormatData, 0x0, sizeof(ASF_BMPINFOHEADER_t));
#endif

	hdr->i_file_size = 0;
	hdr->i_data_packets_count = 0;
	hdr->i_min_data_packet_size = 0;
	hdr->i_encrypted = 0;

	for( CheckNum = 0; CheckNum < 128; CheckNum++ )
	{
		hdr->stream[CheckNum].i_cat = MMS_ASF_STREAM_UNKNOWN;
		hdr->stream[CheckNum].i_selected = 0;
		hdr->stream[CheckNum].i_bitrate = -1;
	}

	MMS_BUF_Initread( &tBuffer, p_header, i_header );
	MMS_BUF_Getguid( &tBuffer, &tAsfGuid );

	if( !INT_ASF_CompareGUID ( &tAsfGuid, &asf_object_header_guid ) )
	{
		/* ERROR: */
		PrintError("%s(%d) Error> Invalid ASF Header Packet \n",__FUNCTION__,__LINE__);
		return;
	}
	MMS_BUF_Getmemory( &tBuffer, NULL, 30 - 16 );

	for( ;; )
	{
		MMS_BUF_Getguid( &tBuffer, &tAsfGuid );
		i_size = MMS_BUF_Get64( &tBuffer );

		if( INT_ASF_CompareGUID ( &tAsfGuid, &asf_object_file_properties_guid ) )
		{
#if 0
	/* http://msdn.microsoft.com/en-us/library/bb970440(VS.85).aspx */
	HUINT8  stream_id[16];  /* stream GUID 						16 */

	HUINT64 file_size; 								/* 			8 */
	HUINT64 creation_time;  /* File creation time FILETIME 				8						*/
	HUINT64 data_packets_count;	/* Number of packets UINT64 		8						   */
	HUINT64 play_duration;  /* Timestamp of the end position UINT64 	8 , 100-nanosecond units   (includes the preroll time)		   */
	HUINT64 send_duration;  /* Duration of the playback UINT64 		8					*/

	HUINT64 preroll;		/* Time to bufferize before playing UINT64 					8			*/
	HUINT32 flags;		  /* Unknown, maybe flags (usually contains 2) UINT32 		4   */
	HUINT32 min_data_packet_size;/* Min size of the packet, in bytes UINT32 		4			*/
	HUINT32 max_data_packet_size;/* Max size of the packet  UINT32 				4					 */
	HUINT32 max_bitrate;	/* Maximum bitrate of the media (sum of all the stream) 	4 */
#endif
			MMS_BUF_Getmemory( &tBuffer, NULL, 16 );				/* skip GUID 16 */
			hdr->i_file_size			= MMS_BUF_Get64( &tBuffer);		/* read file_size 8 */

			MMS_BUF_Getmemory( &tBuffer, NULL, 8 );				/* skip creation_time 8 */
			hdr->i_data_packets_count   = MMS_BUF_Get64( &tBuffer );	/* read data_packet_count 8 */

			hdr->i_play_duration= MMS_BUF_Get64( &tBuffer );			/* read playduration 8 */
			hdr->i_send_duration= MMS_BUF_Get64( &tBuffer );			/* read sendduration 8 */

			MMS_BUF_Getmemory( &tBuffer, NULL, 8+4); 			/* send_duration 8, preroll 8, flags 4 */
			hdr->i_min_data_packet_size = MMS_BUF_Get32( &tBuffer );	/* read min_data_packet_size 4 */
			hdr->i_max_data_packet_size = MMS_BUF_Get32( &tBuffer );/* read max_data_packet_size 4 */
			hdr->i_max_bitrate= MMS_BUF_Get32( &tBuffer );			/* read max_bitrate 4 */

			MMS_BUF_Getmemory( &tBuffer, NULL, i_size - 24 - 16 - 8 - 8 - 8 - 8-8-8-4 - 4 -4 -4); /* sub read byte */
		}
		else if( INT_ASF_CompareGUID ( &tAsfGuid, &asf_object_header_extension_guid ) )
		{
			MMS_BUF_Getmemory( &tBuffer, NULL, 46 - 24 );
		}
		else if( INT_ASF_CompareGUID ( &tAsfGuid, &asf_object_extended_stream_properties_guid ) )
		{
			HINT16 nCount1, nCount2;
			HINT32 nSubsize=0;

			MMS_BUF_Getmemory( &tBuffer, NULL, 84 - 24 );

			nCount1 = MMS_BUF_Get16( &tBuffer );
			nCount2 = MMS_BUF_Get16( &tBuffer );

			nSubsize = 88;

			for( CheckNum = 0; CheckNum < nCount1; CheckNum++ )
			{
				int i_len;

				MMS_BUF_Get16( &tBuffer );
				i_len = MMS_BUF_Get16( &tBuffer );
				MMS_BUF_Getmemory( &tBuffer, NULL, i_len );

				nSubsize = 4 + i_len;
			}

			for( CheckNum = 0; CheckNum < nCount2; CheckNum++ )
			{
				int i_len;
				MMS_BUF_Getmemory( &tBuffer, NULL, 16 + 2 );
				i_len = MMS_BUF_Get32( &tBuffer );
				MMS_BUF_Getmemory( &tBuffer, NULL, i_len );

				nSubsize += 16 + 6 + i_len;
			}

			if( i_size - nSubsize <= 24 )
			{
				MMS_BUF_Getmemory( &tBuffer, NULL, i_size - nSubsize );
			}
		}
		else if( INT_ASF_CompareGUID ( &tAsfGuid, &asf_object_stream_properties_guid ) )
		{
			/*
			Object ID	GUID	128 16
			Object Size	QWORD	64 8
			Stream Type	GUID	128 16
			Error Correction Type	GUID	128 16
			Time Offset	QWORD	64 8
			Type-Specific Data Length	DWORD	32 4
			Error Correction Data Length	DWORD	32 4
			Flags	WORD	16 2
			Reserved	DWORD	32 4
			Type-Specific Data	BYTE	varies
			Error Correction Data	BYTE	varies
			*/

			HINT32		nStream_id=0;
			HUINT16		nEncrypted=0;
			HUINT16		flags=0;
			ASF_GUID_t	tStream_type;
			HUINT32		uTypeSpecificDataLength = 0;
			HUINT32		uErrorCorrectionDataLength = 0;

			/*already read Object ID 16, Object Size 8 = 24*/
			MMS_BUF_Getguid( &tBuffer, &tStream_type ); /* Stream Type 16 */
			MMS_BUF_Getmemory( &tBuffer, NULL, 16 ); /* Error Correction Type 16*/
			MMS_BUF_Getmemory( &tBuffer, NULL, 8 ); /* Time Offset 8*/
			uTypeSpecificDataLength = MMS_BUF_Get32( &tBuffer); /* Type-Specific Data Length 4*/
			//PrintDebug ("Type Specific Data Length = %u\n", uTypeSpecificDataLength);
			uErrorCorrectionDataLength = MMS_BUF_Get32( &tBuffer); /* Error Correction Data Length 4*/
			//PrintDebug ("Error Correction Data Length = %u\n", uErrorCorrectionDataLength);


			flags = MMS_BUF_Get16 ( &tBuffer ); /* Flags 2 */
			/* Stream ID Detection */
			nStream_id = flags & 0x7f;
			/* Encrypted Contents Detection*/
			nEncrypted = flags & 0x8000;

			if( INT_ASF_CompareGUID ( &tStream_type, &asf_object_stream_type_video ) )
			{
				hdr->stream[nStream_id].i_cat = MMS_ASF_STREAM_VIDEO;
				PrintDebug("Type Specific Data Length = %u\n", uTypeSpecificDataLength);
				PrintDebug("Error Correction Data Length = %u\n", uErrorCorrectionDataLength);

				MMS_BUF_Getmemory( &tBuffer, NULL, 4 ); /* Reserved 4*/
#if SUPPORT_ANALYZE_ASF_VIDEO			/* start of type specific data */
				EncodedImageWidth = MMS_BUF_Get32 ( &tBuffer ); /* Encoded Image Width 4 */
				PrintDebug("EncodedImageWidth = %d\n", EncodedImageWidth);
				EncodedImageHeight = MMS_BUF_Get32 ( &tBuffer ); /* Encoded Image Height 4 */
				PrintDebug("EncodedImageHeight = %d\n", EncodedImageHeight);
				ReservedFlags = MMS_BUF_Get8 ( &tBuffer ); /* Reserved Flags 1 */
				PrintDebug("ReservedFlags = %d\n", ReservedFlags);
				FormatDataSize = MMS_BUF_Get16 ( &tBuffer ); /* Format Data Size 2 */
				PrintDebug("FormatDataSize = %d\n", FormatDataSize);
				FormatData.headerSize = MMS_BUF_Get32 ( &tBuffer ); /* Format Data Size 4 */
				PrintDebug("FormatData.headerSize = %d\n", FormatData.headerSize);
				FormatData.biWidth = MMS_BUF_Get32 ( &tBuffer ); /* Image Width 4 */
				PrintDebug("FormatData.biWidth = %d\n", FormatData.biWidth);
				FormatData.biHeight = MMS_BUF_Get32 ( &tBuffer ); /* Image Height 4 */
				PrintDebug("FormatData.biHeight = %d\n", FormatData.biHeight);
				FormatData.biPlanes = MMS_BUF_Get16 ( &tBuffer ); /* Reserved 2 */
				PrintDebug("FormatData.biPlanes = %d\n", FormatData.biPlanes);
				FormatData.biBitCount = MMS_BUF_Get16 ( &tBuffer ); /* Bits Per Pixel Count 2 */
				PrintDebug("FormatData.biBitCount = %d\n", FormatData.biBitCount);

				char CompressedID[5];

				CompressedID[0] = MMS_BUF_Get8(&tBuffer);
				CompressedID[1] = MMS_BUF_Get8(&tBuffer);
				CompressedID[2] = MMS_BUF_Get8(&tBuffer);
				CompressedID[3] = MMS_BUF_Get8(&tBuffer);
				CompressedID[4] = 0;
				PrintDebug("CompressedID = %s\n", CompressedID);
				if(VK_strcmp(CompressedID, "WVC1") == 0)
				{
					hdr->eVidCodec = MMS_ASF_CODEC_MAIN;
				}
				else
				{
					hdr->eVidCodec = MMS_ASF_CODEC_SIMPLE;
				}

				FormatData.biSizeImage = MMS_BUF_Get32 ( &tBuffer ); /* Image Size 4 */
				PrintDebug("FormatData.biSizeImage = %d\n", FormatData.biSizeImage);
				FormatData.biXPelsPerMeter = MMS_BUF_Get32 ( &tBuffer ); /* Horizontal Pixels Per Meter 4 */
				PrintDebug("FormatData.biXPelsPerMeter = %d\n", FormatData.biXPelsPerMeter);
				FormatData.biYPelsPerMeter = MMS_BUF_Get32 ( &tBuffer ); /* Vertical Pixels Per Meter 4 */
				PrintDebug("FormatData.biYPelsPerMeter = %d\n", FormatData.biYPelsPerMeter);
				FormatData.biClrUsed = MMS_BUF_Get32 ( &tBuffer ); /* Colors Used Count 4 */
				PrintDebug("FormatData.biClrUsed = %d\n", FormatData.biClrUsed);
				FormatData.biClrImportant = MMS_BUF_Get32 ( &tBuffer ); /* Important Colors Count 4 */
				PrintDebug("FormatData.biClrImportant = %d\n", FormatData.biClrImportant);
				MMS_BUF_Getmemory( &tBuffer, NULL, i_size - 20 - 24 - 32 - 16 - 2 - 35);
#else
				/* start of type specific data */
				MMS_BUF_Getmemory( &tBuffer, NULL, 11 );

				MMS_BUF_Getmemory( &tBuffer, NULL, 16 );
				{
					char CompressedID[5];
					CompressedID[0] = MMS_BUF_Get8(&tBuffer);
					CompressedID[1] = MMS_BUF_Get8(&tBuffer);
					CompressedID[2] = MMS_BUF_Get8(&tBuffer);
					CompressedID[3] = MMS_BUF_Get8(&tBuffer);
					CompressedID[4] = 0;

					PrintDebug("CompressedID = %s\n", CompressedID);

					if(VK_strcmp(CompressedID, "WVC1") == 0)
					{
						hdr->eVidCodec = MMS_ASF_CODEC_MAIN;
					}
					else
					{
						hdr->eVidCodec = MMS_ASF_CODEC_SIMPLE;
					}
				}

				MMS_BUF_Getmemory( &tBuffer, NULL, i_size - 24 - 32 - 16 - 2 - 35);
#endif
			}
			else
			{
				if( INT_ASF_CompareGUID ( &tStream_type, &asf_object_stream_type_audio ) )
				{
#if SUPPORT_ANALYZE_ASF_AUDIO
					MMS_BUF_Getmemory( &tBuffer, NULL, 4 ); /* Reserved 4*/
					AudioCodecId = MMS_BUF_Get16 ( &tBuffer ); /* Codec ID/Format Tag 2 */
					switch(AudioCodecId)
					{
						case 0x161:
							PrintDebug("AudioCodecId: WINDOWS MEDIA AUDIO(Versions 7,8, and 9 Series)\n");
							break;
						case 0x162:
							PrintDebug("AudioCodecId: WINDOWS MEDIA AUDIO 9 Professional\n");
							break;
						case 0x163:
							PrintDebug("AudioCodecId: WINDOWS MEDIA AUDIO 9 Lossless\n");
							break;
						case 0x7A21:
							PrintDebug("AudioCodecId: GSM-AMR(Fixed bit rate, no SID)\n");
							break;
						case 0x7A22:
							PrintDebug("AudioCodecId: GSM-AMR(Variable bit rate, including SID)\n");
							break;
						default:
							PrintDebug("AudioCodecId: Unknown Codec\n");
							break;
					}

					ChannelNumber = MMS_BUF_Get16 ( &tBuffer ); /* Number of Channels 2 */
					PrintDebug("ChannelNumber = %d\n", ChannelNumber);
					SampleRate = MMS_BUF_Get32 ( &tBuffer ); /* Samples Per Second 4 */
					PrintDebug("SampleRate = %d\n", SampleRate);
					Bitrate = MMS_BUF_Get32 ( &tBuffer ); /* Average Number of Bytes Per Second 4 */
					PrintDebug("Bitrate = %d\n", Bitrate);
					BlockAlignment = MMS_BUF_Get16 ( &tBuffer ); /* Block Alignment 2 */
					PrintDebug("BlockAlignment = %d\n", BlockAlignment);
					BitperSample = MMS_BUF_Get16 ( &tBuffer ); /* Codec Specific Data Size 2 */
					PrintDebug("BitperSample = %d\n", BitperSample);
					CodecDataSize = MMS_BUF_Get16 ( &tBuffer ); /* Codec Specific Data Size 2 */
					PrintDebug("CodecDataSize = %d\n", CodecDataSize);
					hdr->stream[nStream_id].i_cat = MMS_ASF_STREAM_AUDIO;
					MMS_BUF_Getmemory( &tBuffer, NULL, i_size - 22 - 24 - 32 - 16 - 2 );
#else
					hdr->stream[nStream_id].i_cat = MMS_ASF_STREAM_AUDIO;
					MMS_BUF_Getmemory( &tBuffer, NULL, i_size - 24 - 32 - 16 - 2 );
#endif
				}
				else
				{
					hdr->stream[nStream_id].i_cat = MMS_ASF_STREAM_UNKNOWN;
					MMS_BUF_Getmemory( &tBuffer, NULL, i_size - 24 - 32 - 16 - 2 );
				}
			}

#if 0 /* Encrypted Contents Detection moved */
			/** Get Encrypted Content Flags */
			if(i_encrypted)
			{
				PrintDebug ("%s(%d) Protected Content. is_drm is true flags(0x%x)!!!!\n",__FUNCTION__,__LINE__,flags);
				hdr->i_encrypted = 1;
			}
			else
			{
				PrintDebug ("%s(%d) Not Protected Content. is_drm is false flags(0x%x)!!!!\n",__FUNCTION__,__LINE__,flags);
				hdr->i_encrypted = 0;
			}
#endif

		}
		else if ( INT_ASF_CompareGUID ( &tAsfGuid, &asf_object_bitrate_properties_guid ) )
		{
			int	 i_count;
			HUINT8 nStream_id;

			i_count = MMS_BUF_Get16( &tBuffer );
			i_size -= 2;
			while( i_count > 0 )
			{
				nStream_id = MMS_BUF_Get16( &tBuffer )&0x7f;
				hdr->stream[nStream_id].i_bitrate =  MMS_BUF_Get32( &tBuffer );
				i_count--;
				i_size -= 6;
			}
			MMS_BUF_Getmemory( &tBuffer, NULL, i_size - 24 );
		}
#if 0	/* XXX: is support MS DRM Version 7 ??*/
		/* 3.15	Extended Content Encryption Object (optional, 0 or 1) in ASF Spec WMDRM Version 7 */
		else if ( INT_ASF_CompareGUID ( &guid, &asf_object_extended_content_encryption_guid ) )
		{
			HUINT32	signType=0;
			HUINT32 signLength=0;

			signType = MMS_BUF_Get32(&buffer);
			signLength = MMS_BUF_Get32(&buffer);

			PrintDebug ("%s(%d) Detection asf_object_digital_signature_guid signType(0x%x) signLength(0x%x)\n"
					,__FUNCTION__
					,__LINE__
					, signType
					, signLength );
			/* protected contents type is 2 in ASF Spec*/
			if(signType == 2)
			{
				PrintDebug ("%s(%d) DIGITAL SIGNATURE is TRUE !!!!!!\n" ,__FUNCTION__ ,__LINE__);
				hdr->i_encrypted = 1;
			}
			else
			{
				PrintDebug ("%s(%d) DIGITAL SIGNATURE is FALSE !!!!!!\n" ,__FUNCTION__ ,__LINE__);
				hdr->i_encrypted = 0;
			}
			MMS_BUF_Getmemory ( &buffer, NULL, i_size - 24 - 4 - 4);
		}
#endif
		/* 3.16	Digital Signature Object (optional, 0 or 1) in ASF Sped */
		else if ( INT_ASF_CompareGUID ( &tAsfGuid, &asf_object_digital_signature_guid ) )
		{
			HUINT32	signType=0;
			HUINT32 signLength=0;

			signType = MMS_BUF_Get32(&tBuffer);
			i_size-=4;
			signLength = MMS_BUF_Get32(&tBuffer);
			i_size-=4;

			PrintDebug ("%s(%d) Detection asf_object_digital_signature_guid signType(0x%x) signLength(0x%x)\n"
					,__FUNCTION__
					,__LINE__
					, signType
					, signLength );
			/* protected contents type is 2 in ASF Spec*/
			if(signType == 2)
			{
				PrintDebug ("%s(%d) DIGITAL SIGNATURE is TRUE !!!!!!\n" ,__FUNCTION__ ,__LINE__);
				hdr->i_encrypted = 1;
			}
			else
			{
				PrintDebug ("%s(%d) DIGITAL SIGNATURE is FALSE !!!!!!\n" ,__FUNCTION__ ,__LINE__);
				hdr->i_encrypted = 0;
			}

			MMS_BUF_Getmemory ( &tBuffer, NULL, i_size - 24 );
		}
		else
		{
			/** skip unknown guid */
			MMS_BUF_Getmemory( &tBuffer, NULL, i_size - 24 );
		}

		if( MMS_BUF_Readempty( &tBuffer ) )
		{
			PrintExit;
			return;
		}
	}
	PrintExit;
	return;
}

/** param[in]	hdr				ASF Stream Header
 *  			i_bitrate_max	MAXIMUM Bitrate
 *  			b_all			Select ALL STREAM
 *  			b_audio			Select AUDIO Stream
 *  			b_video			Select VIDEO Stream
 */
void  ASF_SelectStream  ( ASF_HEADER_t *hdr, int i_bitrate_max, HBOOL b_all, HBOOL b_audio, HBOOL b_video )
{
	/* XXX FIXME use mututal eclusion information */
	HINT32	CheckNum=0;
	HINT32	nAudio=0;
	HINT32	nVideo=0;
	HINT32	nBitrate_total=0;

	nAudio = 0;
	nVideo = 0;
	nBitrate_total = 0;
	if( b_all )
	{
		/* Check all valid stream */
		for( CheckNum = 1; CheckNum < 128; CheckNum++ )
		{
			if( hdr->stream[CheckNum].i_cat != MMS_ASF_STREAM_UNKNOWN )
			{
				hdr->stream[CheckNum].i_selected = 1;
			}
		}
		return;
	}
	else
	{
		for( CheckNum = 0; CheckNum < 128; CheckNum++ )
		{
			/* by default, not selected */
			hdr->stream[CheckNum].i_selected = 0;
		}
	}

	for( CheckNum = 1; CheckNum < 128; CheckNum++ )
	{
		if( hdr->stream[CheckNum].i_cat == MMS_ASF_STREAM_UNKNOWN )
		{
			continue;
		}
		else if( hdr->stream[CheckNum].i_cat == MMS_ASF_STREAM_AUDIO && b_audio &&
				( nAudio <= 0 ||
				  ( ( ( hdr->stream[CheckNum].i_bitrate > hdr->stream[nAudio].i_bitrate &&
						( nBitrate_total + hdr->stream[CheckNum].i_bitrate - hdr->stream[nAudio].i_bitrate
						  < i_bitrate_max || !i_bitrate_max) ) ||
					  ( hdr->stream[CheckNum].i_bitrate < hdr->stream[nAudio].i_bitrate &&
						i_bitrate_max != 0 && nBitrate_total > i_bitrate_max )
					) )  ) )
		{
			/* unselect old stream */
			if( nAudio > 0 )
			{
				hdr->stream[nAudio].i_selected = 0;
				if( hdr->stream[nAudio].i_bitrate> 0 )
				{
					nBitrate_total -= hdr->stream[nAudio].i_bitrate;
				}
			}

			hdr->stream[CheckNum].i_selected = 1;
			if( hdr->stream[CheckNum].i_bitrate> 0 )
			{
				nBitrate_total += hdr->stream[CheckNum].i_bitrate;
			}

			nAudio = CheckNum;
		}
		else if( hdr->stream[CheckNum].i_cat == MMS_ASF_STREAM_VIDEO && b_video &&
				( nVideo <= 0 ||
				  (
				   ( ( hdr->stream[CheckNum].i_bitrate > hdr->stream[nVideo].i_bitrate &&
					   ( nBitrate_total + hdr->stream[CheckNum].i_bitrate - hdr->stream[nVideo].i_bitrate
						 < i_bitrate_max || !i_bitrate_max) ) ||
					 ( hdr->stream[CheckNum].i_bitrate < hdr->stream[nVideo].i_bitrate &&
					   i_bitrate_max != 0 && nBitrate_total > i_bitrate_max )
				   ) ) )  )
		{
			/* unselect old stream */
			if( nVideo > 0 )
			{
				hdr->stream[nVideo].i_selected = 0;
				if( hdr->stream[nVideo].i_bitrate> 0 )
				{
					nBitrate_total -= hdr->stream[nVideo].i_bitrate;
				}
			}

			hdr->stream[CheckNum].i_selected = 1;
			if( hdr->stream[CheckNum].i_bitrate> 0 )
			{
				nBitrate_total += hdr->stream[CheckNum].i_bitrate;
			}

			nVideo = CheckNum;
		}

	}
	return;
}


static struct asf_streams_t *New_Asf_Streams(void)
{
	struct asf_streams_t *ast = (struct asf_streams_t *)DD_MEM_Alloc(sizeof(struct asf_streams_t));
	if (ast == NULL)
	{
		PrintError("[%s:%d] Error> Memory Alloc Fail\n", __FUNCTION__, __LINE__);
		return NULL;
	}
	VK_memset(ast ,0, sizeof(struct asf_streams_t));
	return ast;
}


static void Free_Asf_Streams(struct asf_streams_t *ast)
{
	if(ast->audio_streams)
	{
		DD_MEM_Free(ast->audio_streams);
		ast->audio_streams = NULL;
	}

	if(ast->video_streams)
	{
		DD_MEM_Free(ast->video_streams);
		ast->video_streams = NULL;
	}

	DD_MEM_Free(ast);
	ast = NULL;
}



ASF_HEADER_t *new_pAsfHeaderInfo_t(void)
{
	ASF_HEADER_t *info = (ASF_HEADER_t *)DD_MEM_Alloc(sizeof(ASF_HEADER_t));
	if (info == NULL)
	{
		PrintError("[%s:%d] Error> Memory Alloc Fail\n", __FUNCTION__, __LINE__);
		return NULL;
	}
	VK_memset(info ,0, sizeof(ASF_HEADER_t));

	info->pRawAsfHeader = NULL;
	info->nRawAsfHeaderLen = 0;

	/* warning: assignment from incompatible pointer type            */
	/* Added (ASF_STREAMS_t *) casting from (struct asf_streams_t *) */
	info->streams = (ASF_STREAMS_t *)New_Asf_Streams();

	return info;
}



void free_pAsfHeaderInfo_t(ASF_HEADER_t *info)
{
	if(!info)
	{
		return;
	}

	if(info->pRawAsfHeader)
	{
		DD_MEM_Free(info->pRawAsfHeader);
		info->pRawAsfHeader = NULL;
	}

	if(info->streams)
	{
		/* Caution to warning: passing argument 1 of 'Free_Asf_Streams' from incompatible pointer type */
		/* Added (struct asf_streams_t *) casting from (ASF_STREAMS_t *)                                 */
		Free_Asf_Streams((struct asf_streams_t *)info->streams);
	}

	DD_MEM_Free(info);
	info = NULL;
}



/*
 * buffer is starting of asf header, and asf_header_size is its size.
 * in Find_Main_Asf_Streams, choose highest streams which works fine
 * with specified bandwidth(bw).
 * all parsed result will be available in asf_http_ctrl;
 *			  *** THIS IS A WRAPPER FUNCTION ***
 *				 return value:	  1 ... success
 *								   -1 ... error
 */
int asf_interpret_header(ASF_HEADER_t *hinfo,const int bw,
			 HUINT8 *buffer, const size_t asf_header_size) /* (const HUINT8 *buffer) */
{
	int ret = 0;

	/* warning: passing argument 2 of 'ASF_ParseHeader' discards qualifiers from pointer target type */
	/* ASF_ParseHeader ( ASF_HEADER_t *hdr, HUINT8 *p_header, int i_header ) */
	ASF_ParseHeader(hinfo, buffer, asf_header_size);
	ret = hinfo->i_max_data_packet_size;

	/* Caution to warning: passing argument 4 of 'Find_Main_Asf_Streams' from incompatible pointer type */
	/* Added (struct asf_streams_t *) casting from (ASF_STREAMS_t *)                                      */
	ret = Find_Main_Asf_Streams(buffer, asf_header_size,
				  bw, /* choose maximum bandwidth */
				  (struct asf_streams_t *)hinfo->streams);
	return ret;
}



int find_asf_guid(const HUINT8 *buffer,int cur_pos,const HUINT8 *guid,int buffer_len)
{
	int i = 0;
	for(i = cur_pos; i < buffer_len - 19 ; i++)
	{
		if((HUINT8)guid[0] == (HUINT8)buffer[i])
		{
			if(VK_memcmp(buffer + i,guid, MMS_ASF_GUID_LENGTH) == 0)
			return i + 16 + 8; /* point after guid + length */
		}
	}
	return -1;
}



/*
 * find best idx
 */
static int max_idx(int s_count,int *s_rates,int bound)
{
	int i = 0, best = -1, rate = -1;
	for(i = 0; i< s_count; i++) {
	if(s_rates[i] > rate && s_rates[i] <= bound) {
		rate = s_rates[i];
		best = i;
	}
	}
	return best;
}

/*
 * Select Audio and Video Main(fastest) Streams
 */
int Find_Main_Asf_Streams(HUINT8 *header, int asf_header_size,	int bw, struct asf_streams_t *asf_streams)
{
	HINT32	CheckNum = 0;
	HINT32	Position = 0;
	HINT32	StartPos = 0;
	HINT32	*pVideo_Rates = NULL;
	HINT32	*pAudio_Rates = NULL;
	HINT32	v_rate = 0;
	HINT32	a_rate = 0;
	HINT32	v_idx = -1;
	HINT32	a_idx = -1;

	Position = sizeof(struct asf_header_t);
	StartPos = Position;

	/*
	  select Main(fastest) streams
	*/
	while((Position = find_asf_guid(header,Position,Asf_Guid_Stream_Header,asf_header_size)) >= 0)
	{
		struct asf_stream_header_t *streamh = (struct asf_stream_header_t *)(header + Position);

		Position += sizeof(struct asf_stream_header_t);
		if(Position > asf_header_size)
		{
			PrintError("[%s] Find Position Error\n", __FUNCTION__);
			return -1;
		}

		/* get ASF GUID PREFIX (first 4 byte) */
		switch(get32_le(streamh->type))
		{
			case MMS_ASF_AUDIO_STREAM: /* audio stream */
				PrintDebug("audio stream detected!!!!\n");
				if(asf_streams->audio_streams == NULL) /* no audio stream registerd yet */
				{
					asf_streams->audio_streams = (int *)DD_MEM_Alloc(sizeof(int));
					if(asf_streams->audio_streams == NULL)
					{
						PrintError("[%s] Memory Alloc Fail\n", __FUNCTION__);
						return -1;
					}

					VK_MEM_Memset(asf_streams->audio_streams ,0, sizeof(int));
					asf_streams->n_audio = 1;
				}
				else  /* more audio streams.!! */
				{
					asf_streams->n_audio++;
					asf_streams->audio_streams = (int *)DD_MEM_Realloc(asf_streams->audio_streams, asf_streams->n_audio * sizeof(int));
					if (asf_streams->audio_streams == NULL)
					{
						PrintError("%s(%d) Error> Out of memory\n", __FUNCTION__, __LINE__);
						return -1;
					}
				}

				asf_streams->audio_streams[asf_streams->n_audio - 1] = le2me_16(streamh->stream_no);
				break;

			case MMS_ASF_VIDEO_STREAM: /* video streams */
				PrintDebug("video stream detected!!!!\n");
				if(asf_streams->video_streams == NULL) /* no video stream registerd yet */
				{
					asf_streams->video_streams = (int *)DD_MEM_Alloc(sizeof(int));
					if(asf_streams->video_streams == NULL)
					{
						PrintError("[%s] Memory Alloc Fail\n", __FUNCTION__);
						return -1;
					}

					VK_MEM_Memset(asf_streams->video_streams ,0, sizeof(int));
					asf_streams->n_video = 1;
				}
				else  /* more video streams.!! */
				{
					asf_streams->n_video++;
					asf_streams->video_streams = (int *)DD_MEM_Realloc(asf_streams->video_streams, asf_streams->n_video * sizeof(int));
					if (asf_streams->video_streams == NULL)
					{
						PrintError("%s(%d) Error> Out of memory\n", __FUNCTION__, __LINE__);
						return -1;
					}
				}

				asf_streams->video_streams[asf_streams->n_video - 1] = le2me_16(streamh->stream_no);
				break;

			case ASF_COMMAND_MEDIA_STREAM: /* Command media stream... whatever*/
				PrintDebug("Command media stream detected, but ignore this.\n");
				break;

			case ASF_FILE_TRANSFER_MEDIA_STREAM: /* File transfer media stream... I don't know what the heck this is.*/
				PrintDebug("File transfer stream detected, but ignore this.\n");
				break;

		}
	}

	pAudio_Rates = DD_MEM_Alloc(asf_streams->n_audio * sizeof(int));
	if (pAudio_Rates == NULL)
	{
		PrintError("[%s:%d] Error> Memory Alloc Fail\n", __FUNCTION__, __LINE__);
		goto Find_Failed;
	}
	VK_MEM_Memset(pAudio_Rates ,0, asf_streams->n_audio * sizeof(int));

	pVideo_Rates = DD_MEM_Alloc(asf_streams->n_video * sizeof(int));
	if (pVideo_Rates == NULL)
	{
		PrintError("[%s:%d] Error> Memory Alloc Fail\n", __FUNCTION__, __LINE__);
		goto Find_Failed;
	}
	VK_MEM_Memset(pVideo_Rates ,0, asf_streams->n_video * sizeof(int));

	Position = find_asf_guid(header, StartPos, Asf_Guid_Stream_Group, asf_header_size);
	if(Position >= 0)
	{
		/* stream bitrate proterties object */
		int stream_count = 0;
		HUINT8 *ptr = (HUINT8 *)header + Position;

		PrintDebug("stream bitrate properties object\n");

		stream_count = get16_le(ptr); /* little endian. */
		ptr += sizeof(HUINT16);
		if(ptr > header + asf_header_size)
		{
			PrintError("[%s] Find Position Error\n", __FUNCTION__);
			goto Find_Failed;
		}

		PrintDebug("stream count = [0x%x] [%u]\n", stream_count, stream_count);
		PrintDebug("audio streams: %d, video streams: %d\n", asf_streams->n_audio,asf_streams->n_video);

		for(CheckNum = 0; CheckNum < stream_count; CheckNum++)
		{
			HUINT32 rate = 0;
			int id = 0;
			int j = 0;

			id = get16_le(ptr);
			ptr += sizeof(HUINT16);
			if(ptr > header + asf_header_size)
			{
				PrintError("[%s] Find Position Error\n", __FUNCTION__);
				goto Find_Failed;
			}

			rate = get32_le(ptr);
			ptr += sizeof(HUINT32);
			if(ptr > header + asf_header_size)
			{
				PrintError("[%s] Find Position Error\n", __FUNCTION__);
				goto Find_Failed;
			}

			PrintDebug("stream_id   = [0x%x] [%u]\n max bitrate = [0x%x] [%u]\n", id, id, rate,rate);

			for(j = 0; j < asf_streams->n_audio; j++)
			{
				if(id == asf_streams->audio_streams[j])
				{
					PrintDebug("is audio stream\n");
					pAudio_Rates[j] = rate;
					break;
				}
			}

			for(j = 0; j < asf_streams->n_video; j++)
			{
				if(id == asf_streams->video_streams[j])
				{
					PrintDebug("is video stream\n");
					pVideo_Rates[j] = rate;
					break;
				}
			}
		}
	}

	/* just to make sure bw is not Zero! */
	if(bw == 0)
	{
		bw = 0x7fffffff; /* INT MAX as maximum bandwidth */
	}

	if(asf_streams->n_audio)
	{
		/* find lowest-bitrate audio stream */
		a_rate = pAudio_Rates[0];
		a_idx = 0;
		for(CheckNum = 0; CheckNum < asf_streams->n_audio; CheckNum++)
		{
			if(pAudio_Rates[CheckNum] < a_rate)
			{
				a_rate = pAudio_Rates[CheckNum];
				a_idx = CheckNum;
			}
		}

		if(max_idx(asf_streams->n_video, pVideo_Rates, bw - a_rate) < 0)
		{
			/* both audio and video are not possible, try video only next */
			a_idx = -1;
			a_rate = 0;
		}
	}

	/* find best video stream */
	v_idx = max_idx(asf_streams->n_video, pVideo_Rates, bw - a_rate);
	if(v_idx >= 0)
	{
		v_rate = pVideo_Rates[v_idx];
	}

	/* find best auido stream */
	a_idx = max_idx(asf_streams->n_audio, pAudio_Rates, bw - v_rate);

	if(pVideo_Rates != NULL)
	{
		DD_MEM_Free(pVideo_Rates);
		pVideo_Rates = NULL;
	}

	if(pAudio_Rates != NULL)
	{
		DD_MEM_Free(pAudio_Rates);
		pAudio_Rates = NULL;
	}

	if(a_idx < 0 && v_idx < 0)
	{
		PrintCriticalDebug("bandwidth too low ... cannot get streams.");
		return -1;
	}


	if(a_idx >= 0)
	{
		asf_streams->audio_id = asf_streams->audio_streams[a_idx];
	}
	else if(asf_streams->n_audio)
	{
		PrintCriticalDebug("Bandwidth too too small so deselected audio....sad.\n");
	}

	if(v_idx >= 0)
	{
		asf_streams->video_id = asf_streams->video_streams[v_idx];
	}
	else if(asf_streams->n_video)
	{
		PrintCriticalDebug("Bandwidth too too small so deselected video....sad.\n");
	}

	return 1;

  Find_Failed:
	if(pVideo_Rates != NULL)
	{
		DD_MEM_Free(pVideo_Rates);
		pVideo_Rates = NULL;
	}

	if(pAudio_Rates != NULL)
	{
		DD_MEM_Free(pAudio_Rates);
		pAudio_Rates = NULL;
	}

	return -1;

}


