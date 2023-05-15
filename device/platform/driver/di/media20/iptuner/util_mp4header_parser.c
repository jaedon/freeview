/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   util_mp4header_parser.c $
 * Version:			$Revision:   1.0  $
 * Original Author: Jeehune Park $
 * Current Author:	$Author: parkjh4@humaxdigital.com $
 * Date:			$Date: 2012.09.24
 * File Description:	managing pool
 * Module:
 * Remarks:
 */

/*******************************************************************/
/* Copyright (c) 2012 HUMAX Co., Ltd. 								*/
/* All rights reserved.												*/
/*******************************************************************/


/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
/* Start Including Header Files */
#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif

#include <inttypes.h>	 /** for 64bit MACRO ex>PRId64 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "di_iptuner_config.h"
#include "di_uart.h"
#include "util_dllist.h"
#include "util_mp4header_parser.h"
#include "htype.h"

#include "vkernel.h"
#include "taskdef.h"

//#define DASH_MEMORY_DEBUG
#ifdef DASH_MEMORY_DEBUG
#include "util_memtool.h"
#define VK_MEM_Alloc(X)		EIKI_MEM_Alloc((HUINT32)X,(HINT8*)__FUNCTION__,(HUINT32)__LINE__)
#define VK_MEM_Free(X)		EIKI_MEM_Free((void*)X,(HINT8*)__FUNCTION__,(HUINT32)__LINE__)
#endif

/* End Including Headers */
/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
/* Start #define */
//#define IPTUNER_ALL_DEBUG
//#define UTIL_MP4HEADER_PARSER_DEBUG
//#define DI_IPTUNER_CRITICAL_DEBUG

#ifdef IPTUNER_ALL_DEBUG
#ifdef UTIL_MP4HEADER_PARSER_DEBUG
#define PrintData		DI_UART_Print
#define PrintDebug		DI_UART_Print
#define PrintError		DI_UART_Print
#define PrintEnter		DI_UART_Print("(+)%s\n", __FUNCTION__)
#define PrintExit		DI_UART_Print("(-)%s\n", __FUNCTION__)
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
#endif

#ifdef DI_IPTUNER_CRITICAL_DEBUG
#define PrintCriticalDebug		DI_UART_Print
#else
#define PrintCriticalDebug		while (0) ((int (*)(char *, ...)) 0)
#endif

#define UNUSED_PARAM(x)  ((void)x)
/*----------------------------------------------------------------------------------------------
 *	MACRO
 *---------------------------------------------------------------------------------------------*/
#define UTIL_MP4_BOX_FTYP   "ftyp"
#define UTIL_MP4_BOX_MOOV	"moov"
#define UTIL_MP4_BOX_MVHD	"mvhd"
#define UTIL_MP4_BOX_TRAK	"trak"
#define UTIL_MP4_BOX_TKHD	"tkhd"
#define UTIL_MP4_BOX_TREF	"tref"
#define UTIL_MP4_BOX_EDTS	"edts"
#define UTIL_MP4_BOX_ELST	"elst"
#define UTIL_MP4_BOX_MDIA	"mdia"
#define UTIL_MP4_BOX_MDHD	"mdhd"
#define UTIL_MP4_BOX_HDLR	"hdlr"
#define UTIL_MP4_BOX_MINF	"minf"
#define UTIL_MP4_BOX_VMHD	"vmhd"
#define UTIL_MP4_BOX_SMHD	"smhd"
#define UTIL_MP4_BOX_HMHD	"hmhd"
#define UTIL_MP4_BOX_NMHD	"nmhd"
#define UTIL_MP4_BOX_DINF	"dinf"
#define UTIL_MP4_BOX_DREF	"dref"
#define UTIL_MP4_BOX_STBL	"stbl"
#define UTIL_MP4_BOX_STSD	"stsd"
#define UTIL_MP4_BOX_STTS	"stts"
#define UTIL_MP4_BOX_CTTS	"ctts"
#define UTIL_MP4_BOX_STSC	"stsc"
#define UTIL_MP4_BOX_STSZ	"stsz"
#define UTIL_MP4_BOX_STZ2	"stz2"
#define UTIL_MP4_BOX_STCO	"stco"
#define UTIL_MP4_BOX_CO64	"co64"
#define UTIL_MP4_BOX_STSS	"stss"
#define UTIL_MP4_BOX_STSH	"stsh"
#define UTIL_MP4_BOX_PADB	"padb"
#define UTIL_MP4_BOX_STDP	"stdp"
#define UTIL_MP4_BOX_SDTP	"sdtp"
#define UTIL_MP4_BOX_SBGP	"sbgp"
#define UTIL_MP4_BOX_SGPD	"sgpd"
#define UTIL_MP4_BOX_SUBS	"subs"
#define UTIL_MP4_BOX_MVEX	"mvex"
#define UTIL_MP4_BOX_MEHD	"mehd"
#define UTIL_MP4_BOX_TREX	"trex"
#define UTIL_MP4_BOX_IPMC	"ipmc"
#define UTIL_MP4_BOX_AVC1	"avc1"
#define UTIL_MP4_BOX_AVCC	"avcC"
#define UTIL_MP4_BOX_MP4A	"mp4a"

static inline UTIL_MP4_LE_VALUE __convertLE(char* pStr)
{
	return (UTIL_MP4_LE_VALUE)(pStr[3] << 24 | pStr[2] << 16 | pStr[1] << 8 | pStr[0]);
}

static inline HUINT32 UTIL_MP4_ConvertByteOrder(HUINT32 val)
{
	return	 (val & 0xFF000000) >> 24	/* 0XXX -> XXX0 */
		   | (val & 0x00FF0000) >> 8	/* X0XX -> XX0X */
		   | (val & 0x0000FF00) << 8	/* XX0X -> X0XX */
		   | (val & 0x000000FF) << 24;	/* XXX0 -> 0XXX */
}

static inline size_t UTIL_MP4_ReadData(void* dat, void *rcv, HINT32 *nPos, HUINT32 size, HBOOL changeOrder)
{
	HUINT32 idx = 0;
	if(size % 4 == 0)
	{
		for(idx = 0; idx < size; idx++)
		{
			((HUINT32*)rcv)[idx/4] = (((HUINT32*)rcv)[idx/4] << 8) | ((HUINT8*)dat)[*nPos+idx];
		}
	}
	else if(size % 2 == 0)
	{
		for(idx = 0; idx < size; idx++)
		{
			((HUINT16*)rcv)[idx/2] = (((HUINT16*)rcv)[idx/2] << 8) | ((HUINT8*)dat)[*nPos+idx];
		}
	}
	else
	{
		for(idx = 0; idx < size; idx++)
		{
			((HUINT8*)rcv)[idx] = ((HUINT8*)dat)[*nPos+idx];
		}
	}

	for(idx = 0; idx < size; idx+=4)
	{
		if(changeOrder == TRUE)
		{
			((HUINT32*)rcv)[idx/4] = UTIL_MP4_ConvertByteOrder(((HUINT32*)rcv)[idx/4]);
		}
	}
	*nPos = *nPos+size;
	return size;
}

static UTIL_MP4_BOX_INFO_t __find_box(HINT8* pBuf, HINT32 szBuf, char* str)
{
	HINT32 nPos = 0;
	HUINT8 len = 0;
	UTIL_MP4_BOX_INFO_t ibox;
	UTIL_MP4_LE_VALUE strCode = __convertLE(str);
	ibox.nBoxSize = 0;
	ibox.nStartOffset = 0;
	ibox.isValid = FALSE;

	if(szBuf == 0)
	{
		return ibox;
	}
	for(nPos = 0; nPos < szBuf - 3; ++nPos)
	{
		if(*(unsigned int*)(&pBuf[nPos]) == strCode)
			break;
	}
	if(nPos == szBuf -3)
	{
		PrintDebug("(%s)box is not found\n", str);
		return ibox;
	}
	ibox.nStartOffset = nPos - 4;
	for(nPos = 0; nPos < 4; nPos++)
	{
		len = pBuf[ibox.nStartOffset+nPos];
		ibox.nBoxSize = (ibox.nBoxSize << 8) + len;
	}
#if 0
	PrintDebug("\t[%s]\n", str);
	PrintDebug("\tStartOffset : %d\n", ibox.nStartOffset);
	PrintDebug("\tBox Size : %d\n", ibox.nBoxSize);
#endif
	ibox.isValid = TRUE;
	return ibox;
}

UTIL_MP4_ERRCODE UTIL_MP4_Parse_avcc(UTIL_MP4_PARSER_AVC1_AVCC_t* pavcc, HINT8* pBuf, UTIL_MP4_BOX_INFO_t iavcc)
{
	UNUSED_PARAM(iavcc);
	HINT8* pDat = pBuf + UTIL_MP4_BOXDATA_START;
	HINT32 nPos = 0;
	HINT32 loopcnt = 0;
	HUINT8 data_8b = 0;
	HUINT8* NAL = NULL;

	PrintDebug("\tavcc box parsing...\n");

	/* avcc data parsing */
	UTIL_MP4_ReadData(pDat, &pavcc->version, &nPos, UTIL_MP4_SIZE_B8, FALSE);
	UTIL_MP4_ReadData(pDat, &pavcc->h264_profile, &nPos, UTIL_MP4_SIZE_B8, FALSE);
	UTIL_MP4_ReadData(pDat, &pavcc->h264_compatible_profile, &nPos, UTIL_MP4_SIZE_B8, FALSE);
	UTIL_MP4_ReadData(pDat, &pavcc->h264_level, &nPos, UTIL_MP4_SIZE_B8, FALSE);
	UTIL_MP4_ReadData(pDat, &data_8b, &nPos, UTIL_MP4_SIZE_B8, FALSE);
	pavcc->reserved_6b = (data_8b & 0xfc) >> 2;
	pavcc->NAL_length_2b = data_8b & 0x03;
	UTIL_MP4_ReadData(pDat, &pavcc->nrSPS, &nPos, UTIL_MP4_SIZE_B8, FALSE);
	UTIL_MP4_ReadData(pDat, &data_8b, &nPos, UTIL_MP4_SIZE_B8, FALSE); // dummy
	UTIL_MP4_ReadData(pDat, &pavcc->SPSlen, &nPos, UTIL_MP4_SIZE_B8, FALSE);
	PrintDebug("%s(%d) SPSSlen : %d\n", __FUNCTION__, __LINE__, pavcc->SPSlen);

	for(loopcnt = 0; loopcnt < pavcc->SPSlen; loopcnt++)
	{
		NAL = VK_MEM_Alloc(sizeof(HUINT8));
		if(NAL == NULL)
		{
			PrintError("%s(%d) Not enough memory! \n", __FUNCTION__, __LINE__);
		}
		VK_MEM_Memset(NAL, 0x0, sizeof(HUINT8));
		UTIL_MP4_ReadData(pDat, NAL, &nPos, UTIL_MP4_SIZE_B8, FALSE);
		UTIL_DLLIST_Append(&pavcc->SPSNAL, NAL);
	}
	UTIL_MP4_ReadData(pDat, &pavcc->nrPPS, &nPos, UTIL_MP4_SIZE_B8, FALSE);
	UTIL_MP4_ReadData(pDat, &data_8b, &nPos, UTIL_MP4_SIZE_B8, FALSE); //dummy
	UTIL_MP4_ReadData(pDat, &pavcc->PPSlen, &nPos, UTIL_MP4_SIZE_B8, FALSE);
	PrintDebug("%s(%d) PPSlen : %d\n", __FUNCTION__, __LINE__, pavcc->PPSlen);

	for(loopcnt = 0; loopcnt < pavcc->PPSlen; loopcnt++)
	{
		NAL = VK_MEM_Alloc(sizeof(HUINT8));
		if(NAL == NULL)
		{
			PrintError("%s(%d) Not enough memory! \n", __FUNCTION__, __LINE__);
		}
		VK_MEM_Memset(NAL, 0x0, sizeof(HUINT8));
		UTIL_MP4_ReadData(pDat, NAL, &nPos, UTIL_MP4_SIZE_B8, FALSE);
		UTIL_DLLIST_Append(&pavcc->PPSNAL, NAL);
	}

	return UTIL_MP4_OK;
}

UTIL_MP4_ERRCODE UTIL_MP4_Release_avcc(UTIL_MP4_PARSER_AVC1_AVCC_t* pavcc)
{
	UTIL_MP4_ERRCODE errCode = UTIL_MP4_FAILED;
	UTIL_DLLNode_t* dllnode = NULL;
	HINT8* NAL = NULL;
	HINT32 szList = 0;
	HINT32 loopcnt = 0;

	szList = UTIL_DLLIST_Size(&pavcc->PPSNAL);
	for(loopcnt = 0; loopcnt < szList; loopcnt++)
	{
		dllnode = UTIL_DLLIST_Get_n_Node(&pavcc->PPSNAL, loopcnt);
		NAL = dllnode->pData;
		VK_MEM_Free(NAL);
		NAL = NULL;
		dllnode->pData = NULL;
	}
	UTIL_DLLIST_Reset(&pavcc->PPSNAL);

	szList = UTIL_DLLIST_Size(&pavcc->SPSNAL);
	for(loopcnt = 0; loopcnt < szList; loopcnt++)
	{
		dllnode = UTIL_DLLIST_Get_n_Node(&pavcc->SPSNAL, loopcnt);
		NAL = dllnode->pData;
		VK_MEM_Free(NAL);
		NAL = NULL;
		dllnode->pData = NULL;
	}
	UTIL_DLLIST_Reset(&pavcc->SPSNAL);

	return UTIL_MP4_OK;
}

UTIL_MP4_ERRCODE UTIL_MP4_Parse_avc1(UTIL_MP4_PARSER_AVC1_t* pavc1, HINT8* pBuf, UTIL_MP4_BOX_INFO_t iavc1)
{
	HINT8* pDat = pBuf + UTIL_MP4_BOXDATA_START;
	HINT32 nPos = 0;
	HINT32 loopcnt = 0;
	UTIL_MP4_BOX_INFO_t iavcc;
	UTIL_MP4_ERRCODE errCode = UTIL_MP4_OK;

	PrintDebug("\tavc1 box parsing...\n");

	/* get avcc box */
	iavcc = __find_box(pBuf, iavc1.nBoxSize, UTIL_MP4_BOX_AVCC);
	if(iavcc.isValid == TRUE)
	{
		PrintDebug("%s box found\n", UTIL_MP4_BOX_AVCC);
		VK_MEM_Memset(&pavc1->avcc, 0x0, sizeof(UTIL_MP4_PARSER_AVC1_AVCC_t));
		errCode = UTIL_MP4_Parse_avcc(&pavc1->avcc, pBuf+iavcc.nStartOffset, iavcc);
		if(errCode == UTIL_MP4_BOX_NOT_FOUND)
		{
			return errCode;
		}
	}

	/* avc1 data parsing */
	for(loopcnt = 0; loopcnt < 6; loopcnt++)
	{
		UTIL_MP4_ReadData(pDat, &pavc1->reserved_12b[loopcnt], &nPos, UTIL_MP4_SIZE_B8, FALSE);
	}
	UTIL_MP4_ReadData(pDat, &pavc1->data_ref_idx, &nPos, UTIL_MP4_SIZE_B16, FALSE);
	UTIL_MP4_ReadData(pDat, &pavc1->enc_version, &nPos, UTIL_MP4_SIZE_B16, FALSE);
	UTIL_MP4_ReadData(pDat, &pavc1->enc_level, &nPos, UTIL_MP4_SIZE_B16, FALSE);
	UTIL_MP4_ReadData(pDat, &pavc1->vendor, &nPos, UTIL_MP4_SIZE_B32, FALSE);
	UTIL_MP4_ReadData(pDat, &pavc1->temp_quality, &nPos, UTIL_MP4_SIZE_B32, FALSE);
	UTIL_MP4_ReadData(pDat, &pavc1->spatial_quality, &nPos, UTIL_MP4_SIZE_B32, FALSE);
	UTIL_MP4_ReadData(pDat, &pavc1->width, &nPos, UTIL_MP4_SIZE_B16, FALSE);
	UTIL_MP4_ReadData(pDat, &pavc1->height, &nPos, UTIL_MP4_SIZE_B16, FALSE);
	UTIL_MP4_ReadData(pDat, &pavc1->h_resol, &nPos, UTIL_MP4_SIZE_B32, FALSE);
	UTIL_MP4_ReadData(pDat, &pavc1->v_resol, &nPos, UTIL_MP4_SIZE_B32, FALSE);
	UTIL_MP4_ReadData(pDat, &pavc1->reserved_32b, &nPos, UTIL_MP4_SIZE_B32, FALSE);
	UTIL_MP4_ReadData(pDat, &pavc1->pre_defined1, &nPos, UTIL_MP4_SIZE_B16, FALSE);
	UTIL_MP4_ReadData(pDat, &pavc1->name_len, &nPos, UTIL_MP4_SIZE_B8, FALSE);
	UTIL_MP4_ReadData(pDat, &pavc1->compressor_name, &nPos, UTIL_MP4_SIZE_B8 * 31, FALSE);
	UTIL_MP4_ReadData(pDat, &pavc1->depth, &nPos, UTIL_MP4_SIZE_B16, FALSE);
	UTIL_MP4_ReadData(pDat, &pavc1->pre_defined2, &nPos, UTIL_MP4_SIZE_B16, FALSE);

	return UTIL_MP4_OK;
}

UTIL_MP4_ERRCODE UTIL_MP4_Release_avc1(UTIL_MP4_PARSER_AVC1_t* pavc1)
{
	UTIL_MP4_ERRCODE errCode = UTIL_MP4_FAILED;
	errCode = UTIL_MP4_Release_avcc(&pavc1->avcc);

	return errCode;
}


UTIL_MP4_ERRCODE UTIL_MP4_Parse_mp4a(UTIL_MP4_PARSER_MP4A_t* pmp4a, HINT8* pBuf, UTIL_MP4_BOX_INFO_t imp4a)
{
	UNUSED_PARAM(imp4a);

	HINT8* pDat = pBuf + UTIL_MP4_BOXDATA_START;
	HINT32 nPos = 0;

	/* mp4a data parsing */
	UTIL_MP4_ReadData(pDat, pmp4a->reserved, &nPos, UTIL_MP4_SIZE_B48, FALSE);
	UTIL_MP4_ReadData(pDat, &pmp4a->data_reference_index, &nPos, UTIL_MP4_SIZE_B16, FALSE);
	UTIL_MP4_ReadData(pDat, &pmp4a->reserved_b64, &nPos, UTIL_MP4_SIZE_B64, FALSE);
	UTIL_MP4_ReadData(pDat, &pmp4a->channel_count, &nPos, UTIL_MP4_SIZE_B16, FALSE);
	UTIL_MP4_ReadData(pDat, &pmp4a->sample_size, &nPos, UTIL_MP4_SIZE_B16, FALSE);
	UTIL_MP4_ReadData(pDat, &pmp4a->pre_defined, &nPos, UTIL_MP4_SIZE_B16, FALSE);
	UTIL_MP4_ReadData(pDat, &pmp4a->reserved_b16, &nPos, UTIL_MP4_SIZE_B16, FALSE);
	UTIL_MP4_ReadData(pDat, &pmp4a->time_scale, &nPos, UTIL_MP4_SIZE_B16, FALSE);
	UTIL_MP4_ReadData(pDat, &pmp4a->reserved_b16_2, &nPos, UTIL_MP4_SIZE_B16, FALSE);

	return UTIL_MP4_OK;
}

UTIL_MP4_ERRCODE UTIL_MP4_Release_mp4a(UTIL_MP4_PARSER_MP4A_t* pmp4a)
{
	return UTIL_MP4_OK;
}


UTIL_MP4_ERRCODE UTIL_MP4_Parse_mvhd(UTIL_MP4_PARSER_MVHD_t* pmvhd, HINT8* pBuf, UTIL_MP4_BOX_INFO_t imvhd)
{
	UNUSED_PARAM(imvhd);
	HINT8* pDat = pBuf + UTIL_MP4_BOXDATA_START;
	HINT32 nPos = 0;
	HINT8 ver = 0;
	HINT32 flags = 0;

	PrintDebug("\tmvhd box parsing...\n");

	/* mvhd data parsing */
	UTIL_MP4_ReadData(pDat, &ver, &nPos, UTIL_MP4_SIZE_VERSION, FALSE);
	UTIL_MP4_ReadData(pDat, &flags, &nPos, UTIL_MP4_SIZE_FLAGS, FALSE);
	PrintDebug("\tmvhd version : %d\n", ver);

	if(ver == 0)
	{
		UTIL_MP4_ReadData(pDat, &pmvhd->creation_time.b32, &nPos, 4, FALSE);
		UTIL_MP4_ReadData(pDat, &pmvhd->modification_time.b32, &nPos, 4, FALSE);
		UTIL_MP4_ReadData(pDat, &pmvhd->timescale, &nPos, 4, FALSE);
		UTIL_MP4_ReadData(pDat, &pmvhd->duration.b32, &nPos, 4, FALSE);
	}
	else if (ver == 1)
	{
		UTIL_MP4_ReadData(pDat, &pmvhd->creation_time.b64, &nPos, 8, FALSE);
		UTIL_MP4_ReadData(pDat, &pmvhd->modification_time.b64, &nPos, 4, FALSE);
		UTIL_MP4_ReadData(pDat, &pmvhd->timescale, &nPos, 8, FALSE);
		UTIL_MP4_ReadData(pDat, &pmvhd->duration.b64, &nPos, 8, FALSE);
	}
	UTIL_MP4_ReadData(pDat, &pmvhd->rate, &nPos, 4, FALSE);
	UTIL_MP4_ReadData(pDat, &pmvhd->volume, &nPos, 2, FALSE);
	UTIL_MP4_ReadData(pDat, &pmvhd->reserved, &nPos, 2, FALSE);
	UTIL_MP4_ReadData(pDat, &pmvhd->reserved_array, &nPos, 8, FALSE);
	UTIL_MP4_ReadData(pDat, &pmvhd->matrix, &nPos, 36, FALSE);
	UTIL_MP4_ReadData(pDat, &pmvhd->pre_defined, &nPos, 24, FALSE);
	UTIL_MP4_ReadData(pDat, &pmvhd->next_track_ID, &nPos, 4, FALSE);

	return UTIL_MP4_OK;
}

UTIL_MP4_ERRCODE UTIL_MP4_Parse_tkhd(UTIL_MP4_PARSER_TKHD_t* ptkhd, HINT8* pBuf, UTIL_MP4_BOX_INFO_t itkhd)
{
	UNUSED_PARAM(itkhd);
	HINT8* pDat = pBuf + UTIL_MP4_BOXDATA_START;
	HINT32 nPos = 0;
	HINT8 ver = 0;
	HINT32 flags = 0;

	PrintDebug("\ttkhd box parsing...\n");

	/* mvhd data parsing */
	UTIL_MP4_ReadData(pDat, &ver, &nPos, UTIL_MP4_SIZE_VERSION, FALSE);
	UTIL_MP4_ReadData(pDat, &flags, &nPos, UTIL_MP4_SIZE_FLAGS, FALSE);
	PrintDebug("\ttkhd version : %d\n", ver);
	if(ver == 0)
	{
		UTIL_MP4_ReadData(pDat, &ptkhd->creation_time.b32, &nPos, 4, FALSE);
		UTIL_MP4_ReadData(pDat, &ptkhd->modification_time.b32, &nPos, 4, FALSE);
		UTIL_MP4_ReadData(pDat, &ptkhd->track_ID, &nPos, 4, FALSE);
		UTIL_MP4_ReadData(pDat, &ptkhd->reserved, &nPos, 4, FALSE);
		UTIL_MP4_ReadData(pDat, &ptkhd->duration.b32, &nPos, 4, FALSE);
	}
	else if (ver == 1)
	{
		UTIL_MP4_ReadData(pDat, &ptkhd->creation_time.b64, &nPos, 8, FALSE);
		UTIL_MP4_ReadData(pDat, &ptkhd->modification_time.b64, &nPos, 8, FALSE);
		UTIL_MP4_ReadData(pDat, &ptkhd->track_ID, &nPos, 4, FALSE);
		UTIL_MP4_ReadData(pDat, &ptkhd->reserved, &nPos, 4, FALSE);
		UTIL_MP4_ReadData(pDat, &ptkhd->duration.b64, &nPos, 8, FALSE);
	}

	UTIL_MP4_ReadData(pDat, &ptkhd->reserved_2, &nPos, 8, FALSE);
	UTIL_MP4_ReadData(pDat, &ptkhd->layer, &nPos, 2, FALSE);
	UTIL_MP4_ReadData(pDat, &ptkhd->alternate_group, &nPos, 2, FALSE);
	UTIL_MP4_ReadData(pDat, &ptkhd->volume, &nPos, 2, FALSE);
	UTIL_MP4_ReadData(pDat, &ptkhd->reserved_3, &nPos, 2, FALSE);
	UTIL_MP4_ReadData(pDat, &ptkhd->matrix, &nPos, 36, FALSE);
	UTIL_MP4_ReadData(pDat, &ptkhd->width, &nPos, 4, FALSE);
	UTIL_MP4_ReadData(pDat, &ptkhd->height, &nPos, 4, FALSE);

	//adjust width and height value( >> 16)
	ptkhd->width = ptkhd->width >> 16;
	ptkhd->height = ptkhd->height >> 16;

	return UTIL_MP4_OK;
}

UTIL_MP4_ERRCODE UTIL_MP4_Parse_tref(UTIL_MP4_PARSER_TREF_t* ptref, HINT8* pBuf, UTIL_MP4_BOX_INFO_t itref)
{
	UNUSED_PARAM(ptref);
	UNUSED_PARAM(pBuf);
	UNUSED_PARAM(itref);
	return UTIL_MP4_OK;
}

UTIL_MP4_ERRCODE UTIL_MP4_Parse_hdlr(UTIL_MP4_PARSER_HDLR_t* phdlr, HINT8* pBuf, UTIL_MP4_BOX_INFO_t ihdlr)
{
	UNUSED_PARAM(ihdlr);
	HINT8* pDat = pBuf + UTIL_MP4_BOXDATA_START;
	HINT32 nPos = 0;
	HINT8 ver = 0;
	HINT32 flags = 0;
	HINT8  hdl_type[5];

	PrintDebug("\thdlr box parsing...\n");
	VK_MEM_Memset(hdl_type, 0x0, 5);
	/* hdlr data parsing */
	UTIL_MP4_ReadData(pDat, &ver, &nPos, UTIL_MP4_SIZE_VERSION, FALSE);
	UTIL_MP4_ReadData(pDat, &flags, &nPos, UTIL_MP4_SIZE_FLAGS, FALSE);
	PrintDebug("\thdlr version : %d\n", ver);
	UTIL_MP4_ReadData(pDat, &phdlr->pre_defined, &nPos, UTIL_MP4_SIZE_B32, FALSE);
	UTIL_MP4_ReadData(pDat, hdl_type, &nPos, UTIL_MP4_SIZE_B8*4, FALSE);
	UTIL_MP4_ReadData(pDat, &phdlr->reserved, &nPos, UTIL_MP4_SIZE_B32*3, FALSE);
	UTIL_MP4_ReadData(pDat, &phdlr->name, &nPos, UTIL_MP4_SIZE_B32, TRUE);

	if(!VK_strcmp(hdl_type, "ediv")) /*vide*/
	{
		phdlr->handler_type = UTIL_MP4_HDL_VIDE;
	}
	else if(!VK_strcmp(hdl_type, "nuos")) /*soun*/
	{
		phdlr->handler_type = UTIL_MP4_HDL_SOUN;
	}
	else if(!VK_strcmp(hdl_type, "tnih")) /*hint*/
	{
		phdlr->handler_type = UTIL_MP4_HDL_HINT;
	}
	else if(!VK_strcmp(hdl_type, "atem")) /*meta*/
	{
		phdlr->handler_type = UTIL_MP4_HDL_META;
	}
	return UTIL_MP4_OK;
}

UTIL_MP4_ERRCODE UTIL_MP4_Parse_mdhd(UTIL_MP4_PARSER_MDHD_t* pmdhd, HINT8* pBuf, UTIL_MP4_BOX_INFO_t imdhd)
{
	UNUSED_PARAM(imdhd);
	HINT8* pDat = pBuf + UTIL_MP4_BOXDATA_START;
	HINT32 nPos = 0;
	HINT8  ver = 0;
	HINT32 flags = 0;
	HUINT16 language = 0;
	PrintDebug("\tmdhd box parsing...\n");

	/* mvhd data parsing */
	UTIL_MP4_ReadData(pDat, &ver, &nPos, UTIL_MP4_SIZE_VERSION, FALSE);
	UTIL_MP4_ReadData(pDat, &flags, &nPos, UTIL_MP4_SIZE_FLAGS, FALSE);
	PrintDebug("\tmdhd version : %d\n", ver);
	if(ver == 0)
	{
		UTIL_MP4_ReadData(pDat, &pmdhd->creation_time.b32, &nPos, UTIL_MP4_SIZE_B32, FALSE);
		UTIL_MP4_ReadData(pDat, &pmdhd->modification_time.b32, &nPos, UTIL_MP4_SIZE_B32, FALSE);
		UTIL_MP4_ReadData(pDat, &pmdhd->timescale, &nPos, UTIL_MP4_SIZE_B32, FALSE);
		UTIL_MP4_ReadData(pDat, &pmdhd->duration.b32, &nPos, UTIL_MP4_SIZE_B32, FALSE);
	}
	else if (ver == 1)
	{
		UTIL_MP4_ReadData(pDat, &pmdhd->creation_time.b64, &nPos, UTIL_MP4_SIZE_B64, FALSE);
		UTIL_MP4_ReadData(pDat, &pmdhd->modification_time.b64, &nPos, UTIL_MP4_SIZE_B64, FALSE);
		UTIL_MP4_ReadData(pDat, &pmdhd->timescale, &nPos, UTIL_MP4_SIZE_B32, FALSE);
		UTIL_MP4_ReadData(pDat, &pmdhd->duration.b64, &nPos, UTIL_MP4_SIZE_B64, FALSE);
	}
	UTIL_MP4_ReadData(pDat, &language, &nPos, UTIL_MP4_SIZE_B16, FALSE);
	pmdhd->language0 = language & 0xF800 >> 11;
	pmdhd->language1 = language & 0x07C0 >> 6;
	pmdhd->language2 = language & 0x003E >> 1;

	UTIL_MP4_ReadData(pDat, &pmdhd->pre_defined, &nPos, UTIL_MP4_SIZE_B16, FALSE);

	return UTIL_MP4_OK;
}

UTIL_MP4_ERRCODE UTIL_MP4_Parse_vmhd(UTIL_MP4_PARSER_VMHD_t* pvmhd, HINT8* pBuf, UTIL_MP4_BOX_INFO_t ivmhd)
{
	UNUSED_PARAM(pvmhd);
	UNUSED_PARAM(pBuf);
	UNUSED_PARAM(ivmhd);
	return UTIL_MP4_OK;
}

UTIL_MP4_ERRCODE UTIL_MP4_Parse_smhd(UTIL_MP4_PARSER_SMHD_t* psmhd, HINT8* pBuf, UTIL_MP4_BOX_INFO_t ismhd)
{
	UNUSED_PARAM(psmhd);
	UNUSED_PARAM(pBuf);
	UNUSED_PARAM(ismhd);
	return UTIL_MP4_OK;
}

UTIL_MP4_ERRCODE UTIL_MP4_Parse_hmhd(UTIL_MP4_PARSER_HMHD_t* phmhd, HINT8* pBuf, UTIL_MP4_BOX_INFO_t ihmhd)
{
	UNUSED_PARAM(phmhd);
	UNUSED_PARAM(pBuf);
	UNUSED_PARAM(ihmhd);
	return UTIL_MP4_OK;
}

UTIL_MP4_ERRCODE UTIL_MP4_Parse_nmhd(UTIL_MP4_PARSER_NMHD_t* pnmhd, HINT8* pBuf, UTIL_MP4_BOX_INFO_t inmhd)
{
	UNUSED_PARAM(pnmhd);
	UNUSED_PARAM(pBuf);
	UNUSED_PARAM(inmhd);
	return UTIL_MP4_OK;
}

UTIL_MP4_ERRCODE UTIL_MP4_Parse_stsd(UTIL_MP4_PARSER_STSD_t* pstsd, HINT8* pBuf, UTIL_MP4_BOX_INFO_t istsd)
{
	HINT8* pDat = pBuf + UTIL_MP4_BOXDATA_START;
	HINT32 nPos = 0;
	HINT8  ver = 0;
	HINT32 flags = 0;
	HUINT32 entry_count = 0;
	UTIL_MP4_BOX_INFO_t entry;
	UTIL_MP4_ERRCODE errCode = UTIL_MP4_OK;
	UTIL_MP4_PARSER_AVC1_t* avc1= NULL;
	UTIL_MP4_PARSER_MP4A_t* mp4a= NULL;
	UTIL_MP4_CODEC_t* codecInfo = NULL;
	PrintDebug("\tstsd box parsing...\n");

	/* stsd data parsing */
	UTIL_MP4_ReadData(pDat, &ver, &nPos, UTIL_MP4_SIZE_VERSION, FALSE);
	UTIL_MP4_ReadData(pDat, &flags, &nPos, UTIL_MP4_SIZE_FLAGS, FALSE);
	PrintDebug("\tstsd version : %d\n", ver);
	UTIL_MP4_ReadData(pDat, &entry_count, &nPos, UTIL_MP4_SIZE_B32, FALSE);
	PrintDebug("\tstsd nr_entry (%d)\n", entry_count);
	while(entry_count > 0)
	{
		/* get avc1 box */
		entry = __find_box(pBuf, istsd.nBoxSize, UTIL_MP4_BOX_AVC1);
		if(entry.isValid == TRUE)
		{
			PrintDebug("%s box found\n", UTIL_MP4_BOX_AVC1);
			avc1 = VK_MEM_Alloc(sizeof(UTIL_MP4_PARSER_AVC1_t));
			VK_MEM_Memset(avc1, 0x0, sizeof(UTIL_MP4_PARSER_AVC1_t));
			errCode = UTIL_MP4_Parse_avc1(avc1, pBuf+entry.nStartOffset, entry);
			if(errCode == UTIL_MP4_BOX_NOT_FOUND)
			{
				return errCode;
			}
			UTIL_DLLIST_Append(&pstsd->entry, avc1);
			codecInfo = VK_MEM_Alloc(sizeof(UTIL_MP4_CODEC_t));
			*codecInfo = UTIL_MP4_CODEC_AVC1;
			UTIL_DLLIST_Append(&pstsd->codecInfo, codecInfo);
		}

		/* get mp4a box */
		entry = __find_box(pBuf, istsd.nBoxSize, UTIL_MP4_BOX_MP4A);
		if(entry.isValid == TRUE)
		{
			PrintDebug("%s box found\n", UTIL_MP4_BOX_MP4A);
			mp4a = VK_MEM_Alloc(sizeof(UTIL_MP4_PARSER_MP4A_t));
			VK_MEM_Memset(mp4a, 0x0, sizeof(UTIL_MP4_PARSER_MP4A_t));
			errCode = UTIL_MP4_Parse_mp4a(mp4a, pBuf+entry.nStartOffset, entry);
			if(errCode == UTIL_MP4_BOX_NOT_FOUND)
			{
				return errCode;
			}
			UTIL_DLLIST_Append(&pstsd->entry, mp4a);
			codecInfo = VK_MEM_Alloc(sizeof(UTIL_MP4_CODEC_t));
			*codecInfo = UTIL_MP4_CODEC_MP4A;
			UTIL_DLLIST_Append(&pstsd->codecInfo, codecInfo);
		}
		entry_count--;
	}

	return UTIL_MP4_OK;
}

UTIL_MP4_ERRCODE UTIL_MP4_Release_stsd(UTIL_MP4_PARSER_STSD_t* pstsd)
{
	UTIL_MP4_ERRCODE errCode = UTIL_MP4_FAILED;
	UTIL_DLLNode_t* dllnode = NULL;
	UTIL_DLLNode_t* dllnode_entry = NULL;
	UTIL_MP4_CODEC_t* codecInfo = NULL;
	UTIL_MP4_PARSER_AVC1_t* avc1= NULL;
	UTIL_MP4_PARSER_MP4A_t* mp4a= NULL;
	HINT32 szList = 0;
	HINT32 loopcnt = 0;

	szList = UTIL_DLLIST_Size(&pstsd->codecInfo);
	for(loopcnt = 0; loopcnt < szList; loopcnt++)
	{
		dllnode = UTIL_DLLIST_Get_n_Node(&pstsd->codecInfo, loopcnt);
		codecInfo = dllnode->pData;
		switch(*codecInfo)
		{
			case UTIL_MP4_CODEC_AVC1:
				dllnode_entry = UTIL_DLLIST_Get_n_Node(&pstsd->entry, loopcnt);
				avc1 = dllnode_entry->pData;
				errCode = UTIL_MP4_Release_avc1(avc1);
				if(errCode != UTIL_MP4_OK)
				{
					return errCode;
				}
				VK_MEM_Free(avc1);
				avc1 = NULL;
				dllnode_entry->pData = NULL;
				break;

			case UTIL_MP4_CODEC_MP4A:
				dllnode_entry = UTIL_DLLIST_Get_n_Node(&pstsd->entry, loopcnt);
				mp4a = dllnode_entry->pData;
				errCode = UTIL_MP4_Release_mp4a(mp4a);
				if(errCode != UTIL_MP4_OK)
				{
					return errCode;
				}
				VK_MEM_Free(mp4a);
				mp4a = NULL;
				dllnode_entry->pData = NULL;
				break;
			default:
				break;
		}

		VK_MEM_Free(codecInfo);
		codecInfo = NULL;
		dllnode->pData =NULL;
	}
	UTIL_DLLIST_Reset(&pstsd->entry);
	UTIL_DLLIST_Reset(&pstsd->codecInfo);

	return UTIL_MP4_OK;
}

UTIL_MP4_ERRCODE UTIL_MP4_Parse_stts(UTIL_MP4_PARSER_STTS_t* pstts, HINT8* pBuf, UTIL_MP4_BOX_INFO_t istts)
{
	UNUSED_PARAM(istts);
	HINT8* pDat = pBuf + UTIL_MP4_BOXDATA_START;
	HINT32 nPos = 0;
	HINT8  ver = 0;
	HINT32 flags = 0;
	HUINT32 entry_count = 0;
	UTIL_MP4_PARSER_STTS_ENTRY_t* entry = NULL;
	PrintDebug("\tstts box parsing...\n");

	/* stts data parsing */
	UTIL_MP4_ReadData(pDat, &ver, &nPos, UTIL_MP4_SIZE_VERSION, FALSE);
	UTIL_MP4_ReadData(pDat, &flags, &nPos, UTIL_MP4_SIZE_FLAGS, FALSE);
	PrintDebug("\tstts version : %d\n", ver);
	UTIL_MP4_ReadData(pDat, &entry_count, &nPos, UTIL_MP4_SIZE_B32, FALSE);
	PrintDebug("\tstts sample (%d)\n", entry_count);
	while(entry_count > 0)
	{
		entry = VK_MEM_Alloc(sizeof(UTIL_MP4_PARSER_STTS_ENTRY_t));
		UTIL_MP4_ReadData(pDat, &entry->sample_count, &nPos, UTIL_MP4_SIZE_B32, FALSE);
		UTIL_MP4_ReadData(pDat, &entry->sample_delta, &nPos, UTIL_MP4_SIZE_B32, FALSE);
		UTIL_DLLIST_Append(&pstts->sample, entry);
		entry_count--;
	}

	return UTIL_MP4_OK;
}

UTIL_MP4_ERRCODE UTIL_MP4_Release_stts(UTIL_MP4_PARSER_STTS_t* pstts)
{
	UTIL_MP4_ERRCODE errCode = UTIL_MP4_FAILED;
	UTIL_DLLNode_t* dllnode = NULL;
	UTIL_MP4_PARSER_STTS_ENTRY_t* pSample = NULL;
	HINT32 szList = 0;
	HINT32 loopcnt = 0;

	szList = UTIL_DLLIST_Size(&pstts->sample);
	for(loopcnt = 0; loopcnt < szList; loopcnt++)
	{
		dllnode = UTIL_DLLIST_Get_n_Node(&pstts->sample, loopcnt);
		pSample = dllnode->pData;
		VK_MEM_Free(pSample);
		pSample = NULL;
		dllnode->pData =NULL;
	}
	UTIL_DLLIST_Reset(&pstts->sample);

	return UTIL_MP4_OK;
}

UTIL_MP4_ERRCODE UTIL_MP4_Parse_stss(UTIL_MP4_PARSER_STSS_t* pstss, HINT8* pBuf, UTIL_MP4_BOX_INFO_t istss)
{
	UNUSED_PARAM(istss);
	HINT8* pDat = pBuf + UTIL_MP4_BOXDATA_START;
	HINT32 nPos = 0;
	HINT8  ver = 0;
	HINT32 flags = 0;
	HUINT32 entry_count = 0;
	PrintDebug("\tstss box parsing...\n");

	/* stss data parsing */
	UTIL_MP4_ReadData(pDat, &ver, &nPos, UTIL_MP4_SIZE_VERSION, FALSE);
	UTIL_MP4_ReadData(pDat, &flags, &nPos, UTIL_MP4_SIZE_FLAGS, FALSE);
	PrintDebug("\tstss version : %d\n", ver);
	UTIL_MP4_ReadData(pDat, &entry_count, &nPos, UTIL_MP4_SIZE_B32, FALSE);
	PrintDebug("\tstss sample (%d)\n", entry_count);

	while(entry_count > 0)
	{
		HUINT32* entry;
		entry = VK_MEM_Alloc(sizeof(HUINT32));
		UTIL_MP4_ReadData(pDat, &entry, &nPos, UTIL_MP4_SIZE_B32, FALSE);
		UTIL_DLLIST_Append(&pstss->sample, entry);
		entry_count--;
	}
	return UTIL_MP4_OK;
}

UTIL_MP4_ERRCODE UTIL_MP4_Release_stss(UTIL_MP4_PARSER_STSS_t* pstss)
{
	UTIL_MP4_ERRCODE errCode = UTIL_MP4_FAILED;
	UTIL_DLLNode_t* dllnode = NULL;
	HUINT32* pSample = NULL;
	HINT32 szList = 0;
	HINT32 loopcnt = 0;

	szList = UTIL_DLLIST_Size(&pstss->sample);
	for(loopcnt = 0; loopcnt < szList; loopcnt++)
	{
		dllnode = UTIL_DLLIST_Get_n_Node(&pstss->sample, loopcnt);
		pSample = dllnode->pData;
		VK_MEM_Free(pSample);
		pSample = NULL;
		dllnode->pData =NULL;
	}
	UTIL_DLLIST_Reset(&pstss->sample);

	return UTIL_MP4_OK;
}

UTIL_MP4_ERRCODE UTIL_MP4_Parse_stsc(UTIL_MP4_PARSER_STSC_t* pstsc, HINT8* pBuf, UTIL_MP4_BOX_INFO_t istsc)
{
	UNUSED_PARAM(istsc);
	HINT8* pDat = pBuf + UTIL_MP4_BOXDATA_START;
	HINT32 nPos = 0;
	HINT8  ver = 0;
	HINT32 flags = 0;
	HUINT32 entry_count = 0;
	PrintDebug("\tstsc box parsing...\n");

	/* stss data parsing */
	UTIL_MP4_ReadData(pDat, &ver, &nPos, UTIL_MP4_SIZE_VERSION, FALSE);
	UTIL_MP4_ReadData(pDat, &flags, &nPos, UTIL_MP4_SIZE_FLAGS, FALSE);
	PrintDebug("\tstsc version : %d\n", ver);
	UTIL_MP4_ReadData(pDat, &entry_count, &nPos, UTIL_MP4_SIZE_B32, FALSE);
	PrintDebug("\tstsc sample (%d)\n", entry_count);

	while(entry_count > 0)
	{
		UTIL_MP4_PARSER_STSC_ENTRY_s* entry;
		entry = VK_MEM_Alloc(sizeof(UTIL_MP4_PARSER_STSC_ENTRY_s));
		UTIL_MP4_ReadData(pDat, &entry->first_chunk, &nPos, UTIL_MP4_SIZE_B32, FALSE);
		UTIL_MP4_ReadData(pDat, &entry->samples_per_chunk, &nPos, UTIL_MP4_SIZE_B32, FALSE);
		UTIL_MP4_ReadData(pDat, &entry->sample_description_index, &nPos, UTIL_MP4_SIZE_B32, FALSE);
		UTIL_DLLIST_Append(&pstsc->sample, entry);
		entry_count--;
	}
	return UTIL_MP4_OK;
}

UTIL_MP4_ERRCODE UTIL_MP4_Release_stsc(UTIL_MP4_PARSER_STSC_t* pstsc)
{
	UTIL_MP4_ERRCODE errCode = UTIL_MP4_FAILED;
	UTIL_DLLNode_t* dllnode = NULL;
	UTIL_MP4_PARSER_STSC_ENTRY_s* pSample = NULL;
	HINT32 szList = 0;
	HINT32 loopcnt = 0;

	szList = UTIL_DLLIST_Size(&pstsc->sample);
	for(loopcnt = 0; loopcnt < szList; loopcnt++)
	{
		dllnode = UTIL_DLLIST_Get_n_Node(&pstsc->sample, loopcnt);
		pSample = dllnode->pData;
		VK_MEM_Free(pSample);
		pSample = NULL;
		dllnode->pData =NULL;
	}
	UTIL_DLLIST_Reset(&pstsc->sample);

	return UTIL_MP4_OK;
}

UTIL_MP4_ERRCODE UTIL_MP4_Parse_stsz(UTIL_MP4_PARSER_STSZ_t* pstsz, HINT8* pBuf, UTIL_MP4_BOX_INFO_t istsz)
{
	UNUSED_PARAM(istsz);
	HINT8* pDat = pBuf + UTIL_MP4_BOXDATA_START;
	HINT32 nPos = 0;
	HINT8  ver = 0;
	HINT32 flags = 0;
	HUINT32 entry_count = 0;
	PrintDebug("\tstsz box parsing...\n");

	/* stsz data parsing */
	UTIL_MP4_ReadData(pDat, &ver, &nPos, UTIL_MP4_SIZE_VERSION, FALSE);
	UTIL_MP4_ReadData(pDat, &flags, &nPos, UTIL_MP4_SIZE_FLAGS, FALSE);
	PrintDebug("\tstsz version : %d\n", ver);
	UTIL_MP4_ReadData(pDat, &pstsz->szSample, &nPos, UTIL_MP4_SIZE_B32, FALSE);
	UTIL_MP4_ReadData(pDat, &entry_count, &nPos, UTIL_MP4_SIZE_B32, FALSE);
	PrintDebug("\tstsz sample (%d)\n", pstsz->szSample);
	PrintDebug("\tstsz sample (%d)\n", entry_count);

	while(entry_count > 0)
	{
		HUINT32* entry;
		entry = VK_MEM_Alloc(sizeof(HUINT32));
		UTIL_MP4_ReadData(pDat, &entry, &nPos, UTIL_MP4_SIZE_B32, FALSE);
		UTIL_DLLIST_Append(&pstsz->sample, entry);
		entry_count--;
	}
	return UTIL_MP4_OK;
}

UTIL_MP4_ERRCODE UTIL_MP4_Release_stsz(UTIL_MP4_PARSER_STSZ_t* pstsz)
{
	UTIL_MP4_ERRCODE errCode = UTIL_MP4_FAILED;
	UTIL_DLLNode_t* dllnode = NULL;
	HUINT32* pSample = NULL;
	HINT32 szList = 0;
	HINT32 loopcnt = 0;

	szList = UTIL_DLLIST_Size(&pstsz->sample);
	for(loopcnt = 0; loopcnt < szList; loopcnt++)
	{
		dllnode = UTIL_DLLIST_Get_n_Node(&pstsz->sample, loopcnt);
		pSample = dllnode->pData;
		VK_MEM_Free(pSample);
		pSample = NULL;
		dllnode->pData =NULL;
	}
	UTIL_DLLIST_Reset(&pstsz->sample);

	return UTIL_MP4_OK;
}

UTIL_MP4_ERRCODE UTIL_MP4_Parse_stco(UTIL_MP4_PARSER_STCO_t* pstco, HINT8* pBuf, UTIL_MP4_BOX_INFO_t istco)
{
	UNUSED_PARAM(istco);
	HINT8* pDat = pBuf + UTIL_MP4_BOXDATA_START;
	HINT32 nPos = 0;
	HINT8  ver = 0;
	HINT32 flags = 0;
	HUINT32 entry_count = 0;
	PrintDebug("\tstco box parsing...\n");

	/* stsz data parsing */
	UTIL_MP4_ReadData(pDat, &ver, &nPos, UTIL_MP4_SIZE_VERSION, FALSE);
	UTIL_MP4_ReadData(pDat, &flags, &nPos, UTIL_MP4_SIZE_FLAGS, FALSE);
	PrintDebug("\tstco version : %d\n", ver);
	UTIL_MP4_ReadData(pDat, &entry_count, &nPos, UTIL_MP4_SIZE_B32, FALSE);
	PrintDebug("\tstco sample (%d)\n", entry_count);

	while(entry_count > 0)
	{
		HUINT32* entry;
		entry = VK_MEM_Alloc(sizeof(HUINT32));
		UTIL_MP4_ReadData(pDat, &entry, &nPos, UTIL_MP4_SIZE_B32, FALSE);
		UTIL_DLLIST_Append(&pstco->chunk, entry);
		entry_count--;
	}
	return UTIL_MP4_OK;
}

UTIL_MP4_ERRCODE UTIL_MP4_Release_stco(UTIL_MP4_PARSER_STCO_t* pstco)
{
	UTIL_MP4_ERRCODE errCode = UTIL_MP4_FAILED;
	UTIL_DLLNode_t* dllnode = NULL;
	HUINT32* pChunk = NULL;
	HINT32 szList = 0;
	HINT32 loopcnt = 0;

	szList = UTIL_DLLIST_Size(&pstco->chunk);
	for(loopcnt = 0; loopcnt < szList; loopcnt++)
	{
		dllnode = UTIL_DLLIST_Get_n_Node(&pstco->chunk, loopcnt);
		pChunk = dllnode->pData;
		VK_MEM_Free(pChunk);
		pChunk = NULL;
		dllnode->pData =NULL;
	}
	UTIL_DLLIST_Reset(&pstco->chunk);

	return UTIL_MP4_OK;
}


UTIL_MP4_ERRCODE UTIL_MP4_Parse_stbl(UTIL_MP4_PARSER_STBL_t* pstbl, HINT8* pBuf, UTIL_MP4_BOX_INFO_t istbl)
{
	UTIL_MP4_BOX_INFO_t istsd;
	UTIL_MP4_BOX_INFO_t istts;
	UTIL_MP4_BOX_INFO_t istss;
	UTIL_MP4_BOX_INFO_t istsc;
	UTIL_MP4_BOX_INFO_t istsz;
	UTIL_MP4_BOX_INFO_t istco;
	UTIL_MP4_ERRCODE errCode = UTIL_MP4_OK;

	/* get stsd */
	istsd = __find_box(pBuf, istbl.nBoxSize, UTIL_MP4_BOX_STSD);
	if(istsd.isValid == FALSE)
	{
		return UTIL_MP4_BOX_NOT_FOUND;
	}
	else
	{
		PrintDebug("%s box found\n", UTIL_MP4_BOX_STSD);
		errCode = UTIL_MP4_Parse_stsd(&pstbl->stsd, pBuf+istsd.nStartOffset, istsd);
		if(errCode == UTIL_MP4_BOX_NOT_FOUND)
		{
			return errCode;
		}
	}
	/* get stts */
	istts = __find_box(pBuf, istbl.nBoxSize, UTIL_MP4_BOX_STTS);
	if(istts.isValid == FALSE)
	{
		return UTIL_MP4_BOX_NOT_FOUND;
	}
	else
	{
		PrintDebug("%s box found\n", UTIL_MP4_BOX_STTS);
		errCode = UTIL_MP4_Parse_stts(&pstbl->stts, pBuf+istts.nStartOffset, istts);
		if(errCode == UTIL_MP4_BOX_NOT_FOUND)
		{
			return errCode;
		}
	}
	/* get stss */
	istss = __find_box(pBuf, istbl.nBoxSize, UTIL_MP4_BOX_STSS);
	if(istss.isValid == FALSE)
	{
		/* Could be not exist */
	}
	else
	{
		PrintDebug("%s box found\n", UTIL_MP4_BOX_STSS);
		errCode = UTIL_MP4_Parse_stss(&pstbl->stss, pBuf+istss.nStartOffset, istss);
		if(errCode == UTIL_MP4_BOX_NOT_FOUND)
		{
			return errCode;
		}
	}
	/* get stsc */
	istsc = __find_box(pBuf, istbl.nBoxSize, UTIL_MP4_BOX_STSC);
	if(istsc.isValid == FALSE)
	{
		return UTIL_MP4_BOX_NOT_FOUND;
	}
	else
	{
		PrintDebug("%s box found\n", UTIL_MP4_BOX_STSC);
		errCode = UTIL_MP4_Parse_stsc(&pstbl->stsc, pBuf+istsc.nStartOffset, istsc);
		if(errCode == UTIL_MP4_BOX_NOT_FOUND)
		{
			return errCode;
		}
	}
	/* get stsz */
	istsz = __find_box(pBuf, istbl.nBoxSize, UTIL_MP4_BOX_STSZ);
	if(istsz.isValid == FALSE)
	{
		return UTIL_MP4_BOX_NOT_FOUND;
	}
	else
	{
		PrintDebug("%s box found\n", UTIL_MP4_BOX_STSZ);
		errCode = UTIL_MP4_Parse_stsz(&pstbl->stsz, pBuf+istsz.nStartOffset, istsz);
		if(errCode == UTIL_MP4_BOX_NOT_FOUND)
		{
			return errCode;
		}
	}
	/* get stco */
	istco = __find_box(pBuf, istbl.nBoxSize, UTIL_MP4_BOX_STCO);
	if(istco.isValid == FALSE)
	{
		return UTIL_MP4_BOX_NOT_FOUND;
	}
	else
	{
		PrintDebug("%s box found\n", UTIL_MP4_BOX_STCO);
		errCode = UTIL_MP4_Parse_stco(&pstbl->stco, pBuf+istco.nStartOffset, istco);
		if(errCode == UTIL_MP4_BOX_NOT_FOUND)
		{
			return errCode;
		}
	}
	return UTIL_MP4_OK;
}

UTIL_MP4_ERRCODE UTIL_MP4_Release_stbl(UTIL_MP4_PARSER_STBL_t* pstbl)
{
	UTIL_MP4_ERRCODE errCode = UTIL_MP4_FAILED;

	errCode = UTIL_MP4_Release_stsd(&pstbl->stsd);
	if(errCode != UTIL_MP4_OK)
	{
		return errCode;
	}

	errCode = UTIL_MP4_Release_stts(&pstbl->stts);
	if(errCode != UTIL_MP4_OK)
	{
		return errCode;
	}

	errCode = UTIL_MP4_Release_stss(&pstbl->stss);
	if(errCode != UTIL_MP4_OK)
	{
		return errCode;
	}

	errCode = UTIL_MP4_Release_stsc(&pstbl->stsc);
	if(errCode != UTIL_MP4_OK)
	{
		return errCode;
	}

	errCode = UTIL_MP4_Release_stsz(&pstbl->stsz);
	if(errCode != UTIL_MP4_OK)
	{
		return errCode;
	}

	errCode = UTIL_MP4_Release_stco(&pstbl->stco);
	if(errCode != UTIL_MP4_OK)
	{
		return errCode;
	}

	return UTIL_MP4_OK;
}


UTIL_MP4_ERRCODE UTIL_MP4_Parse_minf(UTIL_MP4_PARSER_MINF_t* pminf, HINT8* pBuf, UTIL_MP4_BOX_INFO_t iminf)
{
	UTIL_MP4_BOX_INFO_t ivmhd;
	UTIL_MP4_BOX_INFO_t ismhd;
	UTIL_MP4_BOX_INFO_t ihmhd;
	UTIL_MP4_BOX_INFO_t inmhd;
	UTIL_MP4_BOX_INFO_t istbl;
	UTIL_MP4_ERRCODE errCode = UTIL_MP4_OK;
	HBOOL bExistence = FALSE;

	/* get vmhd */
	ivmhd = __find_box(pBuf, iminf.nBoxSize, UTIL_MP4_BOX_VMHD);
	if(ivmhd.isValid == FALSE)
	{
	   /* Could be not exist */
	}
	else
	{
		PrintDebug("%s box found\n", UTIL_MP4_BOX_VMHD);
		bExistence = TRUE;
		errCode = UTIL_MP4_Parse_vmhd(&pminf->vmhd, pBuf+ivmhd.nStartOffset, ivmhd);
		if(errCode == UTIL_MP4_BOX_NOT_FOUND)
		{
			return errCode;
		}
		else
		{
  			pminf->isVMHD = TRUE;
		}
	}
	/* get smhd */
	ismhd = __find_box(pBuf, iminf.nBoxSize, UTIL_MP4_BOX_SMHD);
	if(ismhd.isValid == FALSE)
	{
	   /* Could be not exist */
	}
	else
	{
		PrintDebug("%s box found\n", UTIL_MP4_BOX_SMHD);
		bExistence = TRUE;
		errCode = UTIL_MP4_Parse_smhd(&pminf->smhd, pBuf+ismhd.nStartOffset, ismhd);
		if(errCode == UTIL_MP4_BOX_NOT_FOUND)
		{
			return errCode;
		}
		pminf->isSMHD = TRUE;
	}
	/* get hmhd */
	ihmhd = __find_box(pBuf, iminf.nBoxSize, UTIL_MP4_BOX_HMHD);
	if(ihmhd.isValid == FALSE)
	{
	   /* Could be not exist */
	}
	else
	{
		PrintDebug("%s box found\n", UTIL_MP4_BOX_HMHD);
		bExistence = TRUE;
		errCode = UTIL_MP4_Parse_hmhd(&pminf->hmhd, pBuf+ihmhd.nStartOffset, ihmhd);
		if(errCode == UTIL_MP4_BOX_NOT_FOUND)
		{
			return errCode;
		}
		pminf->isHMHD = TRUE;
	}
	/* get nmhd */
	inmhd = __find_box(pBuf, iminf.nBoxSize, UTIL_MP4_BOX_NMHD);
	if(inmhd.isValid == FALSE)
	{
		/* Could be not exist */
	}
	else
	{
		PrintDebug("%s box found\n", UTIL_MP4_BOX_NMHD);
		bExistence = TRUE;
		errCode = UTIL_MP4_Parse_nmhd(&pminf->nmhd, pBuf+inmhd.nStartOffset, inmhd);
		if(errCode == UTIL_MP4_BOX_NOT_FOUND)
		{
			return errCode;
		}
		pminf->isNMHD = TRUE;
	}

	if(bExistence == FALSE)
	{
		return UTIL_MP4_BOX_NOT_FOUND;
	}

	/* get stbl */
	istbl = __find_box(pBuf, iminf.nBoxSize, UTIL_MP4_BOX_STBL);
	if(istbl.isValid == FALSE)
	{
		return UTIL_MP4_BOX_NOT_FOUND;
	}
	else
	{
		PrintDebug("%s box found\n", UTIL_MP4_BOX_STBL);
		errCode = UTIL_MP4_Parse_stbl(&pminf->stbl, pBuf+istbl.nStartOffset, istbl);
		if(errCode == UTIL_MP4_BOX_NOT_FOUND)
		{
			return errCode;
		}
	}

	return UTIL_MP4_OK;
}

UTIL_MP4_ERRCODE UTIL_MP4_Release_minf(UTIL_MP4_PARSER_MINF_t* pminf)
{
	UTIL_MP4_ERRCODE errCode = UTIL_MP4_FAILED;

	/* No need to handle VMHD, SMHD, HMHD, NMHD!  no dynamic allocated variables in them */

	errCode = UTIL_MP4_Release_stbl(&pminf->stbl);
	if(errCode != UTIL_MP4_OK)
	{
		return errCode;
	}

	return UTIL_MP4_OK;
}

UTIL_MP4_ERRCODE UTIL_MP4_Parse_mdia(UTIL_MP4_PARSER_MDIA_t* pmdia, HINT8* pBuf, UTIL_MP4_BOX_INFO_t imdia)
{
	UTIL_MP4_BOX_INFO_t ihdlr;
	UTIL_MP4_BOX_INFO_t iminf;
	UTIL_MP4_BOX_INFO_t imdhd;
	UTIL_MP4_ERRCODE errCode = UTIL_MP4_OK;

	/* get hldr */
	ihdlr = __find_box(pBuf, imdia.nBoxSize, UTIL_MP4_BOX_HDLR);
	if(ihdlr.isValid == FALSE)
	{
		return UTIL_MP4_BOX_NOT_FOUND;
	}
	else
	{
		PrintDebug("%s box found\n", UTIL_MP4_BOX_HDLR);
		errCode = UTIL_MP4_Parse_hdlr(&pmdia->hdlr, pBuf+ihdlr.nStartOffset, ihdlr);
		if(errCode == UTIL_MP4_BOX_NOT_FOUND)
		{
			return errCode;
		}
		pmdia->isHDLR = TRUE;
	}

	/* get minf */
	iminf = __find_box(pBuf, imdia.nBoxSize, UTIL_MP4_BOX_MINF);
	if(iminf.isValid == FALSE)
	{
		return UTIL_MP4_BOX_NOT_FOUND;
	}
	else
	{
		PrintDebug("%s box found\n", UTIL_MP4_BOX_MINF);
		errCode = UTIL_MP4_Parse_minf(&pmdia->minf, pBuf+iminf.nStartOffset, iminf);
		if(errCode == UTIL_MP4_BOX_NOT_FOUND)
		{
			return errCode;
		}
	}

	/* get mdhd */
	imdhd = __find_box(pBuf, imdia.nBoxSize, UTIL_MP4_BOX_MDHD);
	if(imdhd.isValid == FALSE)
	{
		return UTIL_MP4_BOX_NOT_FOUND;
	}
	else
	{
		PrintDebug("%s box found\n", UTIL_MP4_BOX_MDHD);
		errCode = UTIL_MP4_Parse_mdhd(&pmdia->mdhd, pBuf+imdhd.nStartOffset, imdhd);
		if(errCode == UTIL_MP4_BOX_NOT_FOUND)
		{
			return errCode;
		}
	}

	return UTIL_MP4_OK;
}

UTIL_MP4_ERRCODE UTIL_MP4_Release_mdia(UTIL_MP4_PARSER_MDIA_t* pmdia)
{
	UTIL_MP4_ERRCODE errCode = UTIL_MP4_FAILED;

/*
	errCode = UTIL_MP4_Release_mdhd(&pmdia->mdhd);
	if(errCode != UTIL_MP4_OK)
	{
		return errCode;
	}

	if(pmdia->hdlr == TRUE)
	{
		errCode = UTIL_MP4_Release_hdlr(&pmdia->hdlr);
		if(errCode != UTIL_MP4_OK)
		{
			return errCode;
		}
	}
*/

	errCode = UTIL_MP4_Release_minf(&pmdia->minf);
	if(errCode != UTIL_MP4_OK)
	{
		return errCode;
	}

	return UTIL_MP4_OK;
}

UTIL_MP4_ERRCODE UTIL_MP4_Parse_trak(UTIL_MP4_PARSER_TRAK_t* ptrak, HINT8* pBuf, UTIL_MP4_BOX_INFO_t itrak)
{
	UTIL_MP4_BOX_INFO_t itkhd;
	UTIL_MP4_BOX_INFO_t itref;
	UTIL_MP4_BOX_INFO_t imdia;
	UTIL_MP4_ERRCODE errCode = UTIL_MP4_OK;

	/* get tkhd */
	itkhd = __find_box(pBuf, itrak.nBoxSize, UTIL_MP4_BOX_TKHD);
	if(itkhd.isValid == FALSE)
	{
		return UTIL_MP4_BOX_NOT_FOUND;
	}
	else
	{
		PrintDebug("%s box found\n", UTIL_MP4_BOX_TKHD);
		errCode = UTIL_MP4_Parse_tkhd(&ptrak->tkhd, pBuf+itkhd.nStartOffset, itkhd);
		if(errCode == UTIL_MP4_BOX_NOT_FOUND)
		{
			return errCode;
		}
	}

	/* get tref */
	itref = __find_box(pBuf, itrak.nBoxSize, UTIL_MP4_BOX_TREF);
	if(itref.isValid == FALSE)
	{
		/* Optional : Could be not exist */
	}
	else
	{
		PrintDebug("%s box found\n", UTIL_MP4_BOX_TREF);
		errCode = UTIL_MP4_Parse_tref(&ptrak->tref, pBuf + itref.nStartOffset, itref);
		if(errCode == UTIL_MP4_BOX_NOT_FOUND)
		{
			return errCode;
		}
	 }

	/* get mdia */
	imdia = __find_box(pBuf, itrak.nBoxSize, UTIL_MP4_BOX_MDIA);
	if(imdia.isValid == FALSE)
	{
		return UTIL_MP4_BOX_NOT_FOUND;
	}
	else
	{
		PrintDebug("%s box found\n", UTIL_MP4_BOX_MDIA);
		errCode = UTIL_MP4_Parse_mdia(&ptrak->mdia, pBuf + imdia.nStartOffset, imdia);
		if(errCode == UTIL_MP4_BOX_NOT_FOUND)
		{
			return errCode;
		}
	}

	return UTIL_MP4_OK;
}

UTIL_MP4_ERRCODE UTIL_MP4_Release_trak(UTIL_MP4_PARSER_TRAK_t* ptrak)
{
	UTIL_MP4_ERRCODE errCode = UTIL_MP4_FAILED;

	errCode = UTIL_MP4_Release_mdia(&ptrak->mdia);
	if(errCode != UTIL_MP4_OK)
	{
		return errCode;
	}

	/*	Not used ... tref box is not parsed currently
	errCode = UTIL_MP4_Release_tkhd(ptrak->tkhd);
	if(errCode != UTIL_MP4_OK)
	{
		return errCode;
	}


	errCode = UTIL_MP4_Release_tref(ptrak->tref);
	if(errCode != UTIL_MP4_OK)
	{
		return errCode;
	}
	*/

	return UTIL_MP4_OK;
}

UTIL_MP4_ERRCODE UTIL_MP4_Parse_trex(UTIL_MP4_PARSER_TREX_t* ptrex, HINT8* pBuf, UTIL_MP4_BOX_INFO_t itrex)
{
	UNUSED_PARAM(itrex);
	HINT8* pDat = pBuf + UTIL_MP4_BOXDATA_START;
	HINT32 nPos = 0;
	HINT8  ver = 0;
	HINT32 flags = 0;
	PrintDebug("\ttrex box parsing...\n");

	/* trex data parsing */
	UTIL_MP4_ReadData(pDat, &ver, &nPos, UTIL_MP4_SIZE_VERSION, FALSE);
	UTIL_MP4_ReadData(pDat, &flags, &nPos, UTIL_MP4_SIZE_FLAGS, FALSE);
	PrintDebug("\ttrex version : %d\n", ver);
	UTIL_MP4_ReadData(pDat, &ptrex->trackID, &nPos, UTIL_MP4_SIZE_B32, FALSE);
	UTIL_MP4_ReadData(pDat, &ptrex->default_sample_description_index, &nPos, UTIL_MP4_SIZE_B32, FALSE);
	UTIL_MP4_ReadData(pDat, &ptrex->default_sample_duration, &nPos, UTIL_MP4_SIZE_B32, FALSE);
	UTIL_MP4_ReadData(pDat, &ptrex->default_sample_size, &nPos, UTIL_MP4_SIZE_B32, FALSE);
	UTIL_MP4_ReadData(pDat, &ptrex->default_sample_flags, &nPos, UTIL_MP4_SIZE_B32, FALSE);

	return UTIL_MP4_OK;
}

#if NOT_USED
UTIL_MP4_ERRCODE UTIL_MP4_Release_trex(UTIL_MP4_PARSER_TREX_t* ptrex, HINT8* pBuf, UTIL_MP4_BOX_INFO_t itrex)
{
	/* UTIL_MP4_Release_trex
		 : No need to release trex box (no dynamic allocated variables) */
}
#endif

UTIL_MP4_ERRCODE UTIL_MP4_Parse_mvex(UTIL_MP4_PARSER_MVEX_t* pmvex, HINT8* pBuf, UTIL_MP4_BOX_INFO_t imvex)
{
	UTIL_MP4_BOX_INFO_t itrex;
	UTIL_MP4_ERRCODE errCode = UTIL_MP4_OK;
	UTIL_MP4_PARSER_TREX_t* ptrex = NULL;
	HUINT32 nNextOffset = 0;
	HUINT32 nTrexCnt = 0;

	/* get trex(s) */
	do
	{
		itrex = __find_box(pBuf + nNextOffset, imvex.nBoxSize - nNextOffset, UTIL_MP4_BOX_TREX);
		if(itrex.isValid == FALSE)
		{
			break;
		}
		PrintDebug("%s box found\n", UTIL_MP4_BOX_TREX);
		PrintDebug("------ Trex #%d ------\n", nTrexCnt++);

		ptrex = VK_MEM_Alloc(sizeof(UTIL_MP4_PARSER_TREX_t));
		VK_MEM_Memset(ptrex, 0x0, sizeof(UTIL_MP4_PARSER_TREX_t));
		errCode = UTIL_MP4_Parse_trex(ptrex, pBuf + nNextOffset + itrex.nStartOffset, itrex);
		if(errCode == UTIL_MP4_BOX_NOT_FOUND)
		{
			VK_MEM_Free(ptrex);
			ptrex = NULL;
			break;
		}
		nNextOffset += itrex.nStartOffset+itrex.nBoxSize;
		UTIL_DLLIST_Append(&pmvex->trex, ptrex);
	}
	while(itrex.nBoxSize != 0);

	return UTIL_MP4_OK;
}

UTIL_MP4_ERRCODE UTIL_MP4_Release_mvex(UTIL_MP4_PARSER_MVEX_t* pmvex)
{
	UTIL_MP4_ERRCODE errCode = UTIL_MP4_FAILED;
	UTIL_DLLNode_t* dllnode = NULL;
	UTIL_MP4_PARSER_TREX_t* ptrex = NULL;
	HINT32 szList = 0;
	HINT32 loopcnt = 0;

	szList = UTIL_DLLIST_Size(&pmvex->trex);
	for(loopcnt = 0; loopcnt < szList; loopcnt++)
	{
		dllnode = UTIL_DLLIST_Get_n_Node(&pmvex->trex, loopcnt);
		ptrex = dllnode->pData;
		/* There are no dynamic allocated variables in trex box, so remove it immediately ! */
		VK_MEM_Free(ptrex);
		ptrex = NULL;
		dllnode->pData = NULL;
	}
	UTIL_DLLIST_Reset(&pmvex->trex);

	return UTIL_MP4_OK;
}

UTIL_MP4_ERRCODE UTIL_MP4_Parse_moov(UTIL_MP4_PARSER_MOOV_t* pmoov, HINT8* pBuf, UTIL_MP4_BOX_INFO_t imoov)
{
	UTIL_MP4_BOX_INFO_t imvhd;
	UTIL_MP4_BOX_INFO_t itrak;
	UTIL_MP4_BOX_INFO_t imvex;
	UTIL_MP4_PARSER_TRAK_t* ptrak;
	UTIL_MP4_ERRCODE errCode = UTIL_MP4_OK;
	HINT32 nNextOffset = 0;
	HINT32 nTrakCnt = 1;

	/* get mvhd */
	imvhd = __find_box(pBuf, imoov.nBoxSize, UTIL_MP4_BOX_MVHD);
	if(imvhd.isValid == FALSE)
	{
		return UTIL_MP4_BOX_NOT_FOUND;
	}
	PrintDebug("%s box found\n", UTIL_MP4_BOX_MVHD);
	errCode = UTIL_MP4_Parse_mvhd(&pmoov->mvhd, pBuf+imvhd.nStartOffset, imvhd);
	if(errCode == UTIL_MP4_BOX_NOT_FOUND)
	{
		return errCode;
	}
	/* get mvex */
	imvex = __find_box(pBuf, imoov.nBoxSize, UTIL_MP4_BOX_MVEX);
	if(imvex.isValid == TRUE)
	{
		PrintDebug("%s box found\n", UTIL_MP4_BOX_MVEX);
		errCode = UTIL_MP4_Parse_mvex(&pmoov->mvex, pBuf+imvex.nStartOffset, imvex);
		if(errCode == UTIL_MP4_FAILED)
		{
			return errCode;
		}
	}

	/* get trak(s) */
	do
	{
		itrak = __find_box(pBuf + nNextOffset, imoov.nBoxSize - nNextOffset, UTIL_MP4_BOX_TRAK);
		if(itrak.isValid == FALSE)
		{
			break;
		}
		PrintDebug("%s box found\n", UTIL_MP4_BOX_TRAK);
		PrintDebug("------ Track #%d ------\n", nTrakCnt++);

		ptrak = VK_MEM_Alloc(sizeof(UTIL_MP4_PARSER_TRAK_t));
		VK_MEM_Memset(ptrak, 0x0, sizeof(UTIL_MP4_PARSER_TRAK_t));
		errCode = UTIL_MP4_Parse_trak(ptrak, pBuf + nNextOffset + itrak.nStartOffset, itrak);
		if(errCode == UTIL_MP4_BOX_NOT_FOUND)
		{
			VK_MEM_Free(ptrak);
			ptrak = NULL;
			break;
		}
		nNextOffset += itrak.nStartOffset+itrak.nBoxSize;
		UTIL_DLLIST_Append(&pmoov->trak, ptrak);
	}
	while(itrak.nBoxSize != 0 /* || pBuf + nNextOffset < imoov.nBoxSize*/ );

	/* if track does not exist
	if(UTIL_DLLIST_Size(&pmoov->trak) == 0)
	{
		return UTIL_MP4_BOX_NOT_FOUND;
	}*/
	PrintDebug("There are no more tracks\n");
	return UTIL_MP4_OK;
}

UTIL_MP4_ERRCODE UTIL_MP4_Release_moov(UTIL_MP4_PARSER_MOOV_t* pmoov)
{
	UTIL_MP4_ERRCODE errCode = UTIL_MP4_FAILED;
	UTIL_DLLNode_t* dllnode = NULL;
	UTIL_MP4_PARSER_TRAK_t* ptrak = NULL;
	HINT32 szList = 0;
	HINT32 loopcnt = 0;

	errCode = UTIL_MP4_Release_mvex(&pmoov->mvex);
	if(errCode != UTIL_MP4_OK)
	{
		return errCode;
	}

/*
	errCode = UTIL_MP4_Release_mvhd(&pmoov->mvhd);
	if(errCode != UTIL_MP4_OK)
	{
		return errCode;
	}
*/
	szList = UTIL_DLLIST_Size(&pmoov->trak);
	for(loopcnt = 0; loopcnt < szList; loopcnt++)
	{
		dllnode = UTIL_DLLIST_Get_n_Node(&pmoov->trak, loopcnt);
		ptrak = dllnode->pData;
		errCode = UTIL_MP4_Release_trak(ptrak);
		if(errCode != UTIL_MP4_OK)
		{
			return errCode;
		}
		VK_MEM_Free(ptrak);
		ptrak = NULL;
		dllnode->pData = NULL;
	}
	UTIL_DLLIST_Reset(&pmoov->trak);

	return UTIL_MP4_OK;
}

UTIL_MP4_ERRCODE UTIL_MP4_Parse_ftyp(UTIL_MP4_PARSER_FTYP_t* pftyp, HINT8* pBuf, UTIL_MP4_BOX_INFO_t iftyp)
{
	HINT8* pDat = pBuf + UTIL_MP4_BOXDATA_START;
	HINT32 nPos = 0;
	HINT8* pBrand = NULL;
	PrintDebug("\tftyp box parsing...\n");

	/* ftyp data parsing */
	UTIL_MP4_ReadData(pDat, &pftyp->major_brand, &nPos, UTIL_MP4_SIZE_B32, FALSE);
	UTIL_MP4_ReadData(pDat, &pftyp->minor_version, &nPos, UTIL_MP4_SIZE_B32, FALSE);
	PrintDebug("\tcompatible brand : ");
	/* get the number of compatible_brand */
	while((HUINT32)nPos < iftyp.nBoxSize - (UTIL_MP4_SIZE_B32 * 2))
	{
		pBrand = VK_MEM_Alloc(UTIL_MP4_SIZE_BRAND);
		VK_MEM_Memset(pBrand, 0x0, UTIL_MP4_SIZE_BRAND);
		UTIL_MP4_ReadData(pDat, pBrand, &nPos, UTIL_MP4_SIZE_B32, TRUE);
		PrintDebug("%s, ", pBrand);
		UTIL_DLLIST_Append(&pftyp->compatible_brand, pBrand);
	}
	PrintDebug("\n");

	//PrintDebug("\tCompatible brand size (%d)\n", UTIL_DLLIST_Size(&pftyp->compatible_brand));
	return UTIL_MP4_OK;
}

UTIL_MP4_ERRCODE UTIL_MP4_Release_ftyp(UTIL_MP4_PARSER_FTYP_t* pftyp)
{
	UTIL_MP4_ERRCODE errCode = UTIL_MP4_FAILED;
	UTIL_DLLNode_t* dllnode = NULL;
	HINT8* pBrand = NULL;
	HINT32 szList = 0;
	HINT32 loopcnt = 0;

	szList = UTIL_DLLIST_Size(&pftyp->compatible_brand);
	for(loopcnt = 0; loopcnt < szList; loopcnt++)
	{
		dllnode = UTIL_DLLIST_Get_n_Node(&pftyp->compatible_brand, loopcnt);
		pBrand = dllnode->pData;
		VK_MEM_Free(pBrand);
		pBrand = NULL;
		dllnode->pData = NULL;
	}
	UTIL_DLLIST_Reset(&pftyp->compatible_brand);

	return UTIL_MP4_OK;
}

UTIL_MP4_ERRCODE UTIL_MP4_PARSER_Parse(UTIL_MP4_PARSER_t* pMP4, HINT8* pBuf, HINT32 size)
{
	UTIL_MP4_BOX_INFO_t iftyp;
	UTIL_MP4_BOX_INFO_t imoov;
	UTIL_MP4_ERRCODE errCode = UTIL_MP4_FAILED;

	iftyp = __find_box(pBuf, size, UTIL_MP4_BOX_FTYP);
	if(iftyp.nBoxSize != 0)
	{
		PrintDebug("%s box found\n", UTIL_MP4_BOX_FTYP);
		errCode = UTIL_MP4_Parse_ftyp(&pMP4->ftyp, pBuf+iftyp.nStartOffset, iftyp);
	}

	imoov = __find_box(pBuf, size, UTIL_MP4_BOX_MOOV);
	if(imoov.nBoxSize != 0)
	{
		PrintDebug("%s box found\n", UTIL_MP4_BOX_MOOV);
		errCode = UTIL_MP4_Parse_moov(&pMP4->moov, pBuf+imoov.nStartOffset, imoov);
	}

	if(errCode != UTIL_MP4_OK)
	{
		return UTIL_MP4_FAILED;
	}

	return UTIL_MP4_OK;
}

UTIL_MP4_ERRCODE UTIL_MP4_PARSER_Release(UTIL_MP4_PARSER_t* pMP4)
{
	UTIL_MP4_ERRCODE errCode = UTIL_MP4_FAILED;

	errCode = UTIL_MP4_Release_ftyp(&pMP4->ftyp);
	if(errCode != UTIL_MP4_OK)
	{
		return UTIL_MP4_FAILED;
	}
	errCode = UTIL_MP4_Release_moov(&pMP4->moov);
	if(errCode != UTIL_MP4_OK)
	{
		return UTIL_MP4_FAILED;
	}

	return UTIL_MP4_OK;
}

