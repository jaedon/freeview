/**
	@file     file_name.c File Name을 적습니다.
	@brief    file_name & simple comment.

	Description: File에 대한 설명을 적습니다.		\n
	Module: Layer와 모듈 명을 적습니다.			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <stdio.h>
#include <octo_common.h>
#include <vkernel.h>

#include "util.h"
#include "linkedlist.h"

#include <otl.h>

#include <db_svc.h>

#include <pal_crypt.h>
#include <pal_dsmcc.h>
#include <pal_pipe.h>
#include <pal_pb.h>

#include "isosvc.h"

#include <svc_epg.h>

#include "_svc_epg_common.h"
#include "_svc_epg_context_mgr.h"
#include "_svc_epg_common_lib.h"

#include "_xsvc_epg_tvtv_data.h"
#include "_xsvc_epg_tvtv_lib.h"

#include "_svc_epg_db_raw_lib.h"

#include "_xsvc_epg_tvtv_db_lib.h"
#include "_xsvc_epg_tvtv.h"

#define TVTV_NEW_ONE
#if defined(TVTV_NEW_ONE)
#include "dlib_epg.h"
#endif

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/



/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
//#define		CONV_JPEG_TO_BMP

#define		NEW_DES							1


#define		TVTV_DES_ENC						0
#define		TVTV_DES_DEC						1
#define		TVTV_DES_SIZE_BLOCK				8
#define		TVTV_MAX_LEN_COMPDATA			(50*1024)							//(50 * 1024)
#define		TVTV_MAX_LEN_DECOMPDATA			(1024 * 1024)
#if 0
// DVR key
#define		TVTV_PRIMARY_KEY					"4t28hzip4t28hzip4t28hzip"
#else
// ICORD key
#define		TVTV_PRIMARY_KEY					"taratry3taratry2taratry1"
#endif
#define		TVTV_PRIMARY_KEYLEN				24
#define		TVTV_FLT_SIZE						4096
#define		TVTV_FLT_NAME_LENGTH				20
#define		TVTV_XML_NAME_LENGTH				20
#define		TVTV_MAX_EPG_DAY					14
#define		TVTV_MAX_FILE_FULLPATH_LENGTH	255
#if defined(WIN32)
#define		TVTV_BASIC_SVCNAME				"tvtv DIGITAL"		// Emulator에서 CD로 제공된 stream file로 테스트 시...
#else
#define		TVTV_BASIC_SVCNAME				"EPG Digital"			// Live에서 사용할 이름. 'tvtv EPG A_KDG1.pdf'에서 이름이 변경됨 ("tvtv Digital" -> "EPG Digital")
#define		TVTV_BASIC_SVCNUM					0x1FFF				// 항상 고정된 번호를 사용할 수 있도록 강제 할당한다. EEPROM에 13bit 할당됨.
#endif
#define		TVTV_FILES_PER_FOLDER				0x3E0				//0x3F0

#define		TVTV_LANG_DEFAULT				"ger"

 /************************************************************************
 * DOWNTVTV
 *************************************************************************/
#define		TVTV_FOLDER_DOWN_HOME			"DOWNTVTV"
#define		TVTV_FOLDER_DOWN_SYS			"DOWNTVTV/SYS"
#define		TVTV_FOLDER_DOWN_LOGO			"DOWNTVTV/LOGO"
#define		TVTV_FOLDER_DOWN_XML			"DOWNTVTV/XML"		// folder from 'TVTVXML00'. A folder has 1000 xml files.


 /************************************************************************
 * DOWNTVTV -> POSTTVTV로 rename을 한다.
 *************************************************************************/
#define		TVTV_FOLDER_POST_HOME			"POSTTVTV"
#define		TVTV_FOLDER_POST_SYS				"POSTTVTV/SYS"
#define		TVTV_FOLDER_POST_LOGO			"POSTTVTV/LOGO"
#define		TVTV_FOLDER_POST_XML				"POSTTVTV/XML"		// folder from 'TVTVXML00'. A folder has 1000 xml files.


 /************************************************************************
 * POSTTVTV 폴더를 가지고 post processing을 이 폴더로 한다.
 *************************************************************************/
#define		TVTV_FOLDER_PREP_HOME			"PREPTVTV"
#define		TVTV_FOLDER_PREP_SYS				"PREPTVTV/SYS"
#define		TVTV_FOLDER_PREP_LOGO			"PREPTVTV/LOGO"
#define		TVTV_FOLDER_PREP_XML				"PREPTVTV/XML"		// folder from 'TVTVXML00'. A folder has 1000 xml files.
#define		TVTV_FOLDER_PREP_FLT				"PREPTVTV/FILELIST"
#define		TVTV_FOLDER_PREP_JPG				"PREPTVTV/JPG"


 /************************************************************************
 * Post processing 완료 후 PREPTVTV -> TVTV로 rename 한다
 *************************************************************************/
#define		TVTV_FOLDER_HOME					"TVTV"
#define		TVTV_FOLDER_SYS					"TVTV/SYS"
#define		TVTV_FOLDER_LOGO					"TVTV/LOGO"
#define		TVTV_FOLDER_XML					"TVTV/XML"			// folder from 'TVTVXML00'. A folder has 1000 xml files.
#define		TVTV_FOLDER_FLT					"TVTV/FILELIST"
#define		TVTV_FOLDER_JPG					"TVTV/JPG"


 /************************************************************************
 * TVTV file name define
 *************************************************************************/
#define		TVTV_FILENAME_LENGTH				12
#define		TVTV_FILENAME_DGKT				"00000333.dkt"
#define		TVTV_FILENAME_TVST				"00000000.tvs"
#define		TVTV_FILENAME_LTOT				"LTOT.xml"
#define		TVTV_FILENAME_XMLEXTENSION		".xml"
#define		TVTV_FILENAME_JPGEXTENSION		".jpg"
#define		TVTV_FILENAME_DGKTEXTENSION		".dkt"
#define		TVTV_FILENAME_TVSTEXTENSION		".tvs"
#define		TVTV_FILENAME_TSMT_MIDDLE		"FF"
#define		TVTV_FILENAME_LTOT_MIDDLE		"LTOT"




/************************************************************************
 * TVTV TVST(service table) size define
 *************************************************************************/
#define		TVTV_TVST_SIZE_LEN				1
#define		TVTV_TVST_SIZE_TVSID				2
#define		TVTV_TVST_SIZE_TYPE				1
#define		TVTV_TVST_SIZE_VER				4
#define		TVTV_TVST_SIZE_FILECNT			2
#define		TVTV_TVST_SIZE_START				4
#define		TVTV_TVST_SIZE_END				4
#define		TVTV_TVST_SIZE_NAMELEN			1
#define		TVTV_TVST_SIZE_RESERVED			1


/************************************************************************
 * TVTV DGKT(device group key table) size define
 *************************************************************************/
#define		TVTV_DGKT_SIZE_TVSID				2
#define		TVTV_DGKT_SIZE_ENCTYPE			1
#define		TVTV_DGKT_SIZE_ENCLEN			2
#define		TVTV_DGKT_SIZE_COMPTYPE			1
#define		TVTV_DGKT_SIZE_RESERVED			2
#define		TVTV_DGKT_SIZE_SSK				16				// tvtv service session key


/************************************************************************
 * TVTV max count
 *************************************************************************/
#define		TVTV_COUNT_MAX_TSMT				0xff
#define		TVTV_COUNT_MAX_EPG				0x7fff
#define		TVTV_COUNT_MIN_LOGO				0xd000
#define		TVTV_COUNT_MAX_LOGO				0xffff
#define		TVTV_IMG_MAX_COPYRIGHT			(EPG_LANG_ISO_639_LEN + 50)
#define		TVTV_IMG_MAX_FILENAME			(EPG_LANG_ISO_639_LEN + 12)
#define		TVTV_MAX_SHORTNAME				(EPG_LANG_ISO_639_LEN + 8)
#define		TVTV_MAX_LONGNAME				(EPG_LANG_ISO_639_LEN + 32)
#define		TVTV_MAX_LANG						(EPG_LANG_ISO_639_LEN + 3)
#define		TVTV_MAX_COC						(EPG_LANG_ISO_639_LEN + 3)
#define		TVTV_MAX_TIME						(EPG_LANG_ISO_639_LEN + 16)
#define		TVTV_MAX_TITLE					(EPG_LANG_ISO_639_LEN + 50)
#define		TVTV_MAX_LONGTEXT				(EPG_LANG_ISO_639_LEN + 40)
#define		TVTV_MAX_DTEXT					(EPG_LANG_ISO_639_LEN + 800)
#define		TVTV_MAX_SNR						(EPG_LANG_ISO_639_LEN + 9)
#define		TVTV_MAX_COUNTRY					(EPG_LANG_ISO_639_LEN + 15)
#define		TVTV_MAX_YEAR						(EPG_LANG_ISO_639_LEN + 9)
#define		TVTV_MAX_NAME						(EPG_LANG_ISO_639_LEN + 35)


/************************************************************************
 * TVTV time format
 *************************************************************************/
 #define		TVTV_TIME_MAX_LEN			16
#define		TVTV_TIME_SEP_YM			4
#define		TVTV_TIME_SEP_MD			7
#define		TVTV_TIME_SEP_DH			10
#define		TVTV_TIME_SEP_HM			13


#define		TVTV_CMD_ON				1


#define	ENTER_CRITICAL_TVTVBUF								\
{															\
	VK_SEM_Get(s_ulTvtvBufSemaphore);							\
}

#define	LEAVE_CRITICAL_TVTVBUF								\
{															\
	VK_SEM_Release(s_ulTvtvBufSemaphore);						\
}


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/


typedef struct
{
	HUINT16			usTvSid;			// Unique service ID.
	HUINT8			ucType;
	HUINT32			ulVersion;			// ex: Ver2.1.3 = 213
	HUINT16			usFileCnt;			// file count
	HUINT32			ulStart;				// not used
	HUINT32			ulEnd;				// not used
	HUINT8			ucNameLen;			// name length
	HUINT8			*pszName;			// until ucNameLen
} XsvcEpg_TvtvServiceTable_t;


typedef struct
{
	HUINT16			usTvSid;
	HUINT8			ucEncType;
	HUINT16			usEncLen;
	HUINT8			ucCompType;
	HUINT16			usReserved;
	HUINT8			aucSsk[TVTV_DGKT_SIZE_SSK];		// if a key smaller than 128 bits is used, the key is stored in the least significant bits
} XsvcEpg_TvtvDgkt_t;


typedef struct
{
	HUINT16			usTvSid;
	HUINT8			ucType;
	HUINT32			ulVersion;
	HUINT16			usFileCnt;

	HUINT8			ucEncType;
	HUINT16			usEncLen;
	HUINT8			ucCompType;
	HUINT8			*pszName;
	HUINT8			aucSsk[TVTV_DGKT_SIZE_SSK];		// if a key smaller than 128 bits is used, the key is stored in the least significant bits
} XsvcEpg_TvtvSystemInfo_t;




typedef struct
{
	HUINT8		szCopyright[TVTV_IMG_MAX_COPYRIGHT + 1];
	HUINT8		szFilename[TVTV_IMG_MAX_FILENAME + 1];
} XsvcEpg_TvtvImage_t;


typedef struct
{
	HUINT16			usTvtvId;								// mandatory
	HUINT16			usOnId;
	HUINT16			usSId;
	HUINT16			usTsId;
	HUINT16			usF1830;
	HUINT16			usF2830;
	HUINT16			usX26;
	HUINT16			usVps;
	HUINT8			szSname[TVTV_MAX_SHORTNAME + 1];		// mandatory
	HUINT8			szLname[TVTV_MAX_LONGNAME + 1];		// mandatory
	HUINT8			szLang[TVTV_MAX_LANG + 1];				// mandatory
	HUINT8			ucCat;
	HUINT16			usOrder;
	HUINT16			usCaType;
	HUINT16			ausCaId[4];								// 64 bits.
	XsvcEpg_TvtvImage_t	*pstLogoInfo;
} XsvcEpg_TvtvTsmt_t;


typedef struct
{
	HUINT8			szCoc[TVTV_MAX_COC + 1];			// mandatory
	HUINT8			ucCor;								// mandatory
	UNIXTIME		ulToc;
	HUINT8			szToc[TVTV_MAX_TIME + 1];			// mandatory. UTC : <YYYY>-<MM>-<DD>T<HH>:<MM>
	HUINT32			ulLtoBefore;							// mandatory
	HUINT32			ulLtoAfter;							// mandatory
} XsvcEpg_TvtvLtot_t;



typedef struct tagTVTV_NAME
{
	HUINT8					ucType;
	HUINT8					szName[TVTV_MAX_NAME + 1];
	struct tagTVTV_NAME		*pstNext;
} XsvcEpg_TvtvName_t;


typedef struct tagTVTV_LINK
{
	HUINT8					ucType;
	HUINT32					ulProgId;
	struct tagTVTV_LINK		*pstNext;
} XsvcEpg_TvtvLink_t;


typedef struct tagTVTV_CONTENT
{
	HUINT8					ucContent;
	struct tagTVTV_CONTENT	*pstNext;
} XsvcEpg_TvtvContent_t;


typedef struct tagTVTV_SUBTITLE
{
	HUINT8					ucSubtitle;
	struct tagTVTV_SUBTITLE	*pstNext;
} XsvcEpg_TvtvSubtitle_t;


typedef struct tagTVTV_AUDIO
{
	HUINT8					ucAudio;
	struct tagTVTV_AUDIO		*pstNext;
} XsvcEpg_TvtvAudio_t;


typedef struct tagTVTV_VIDEO
{
	HUINT8					ucVideo;
	struct tagTVTV_VIDEO		*pstNext;
} XsvcEpg_TvtvVideo_t;


typedef struct tagTVTV_TIP
{
	HUINT8					ucTip;
	struct tagTVTV_TIP		*pstNext;
} XsvcEpg_TvtvTip_t;


typedef struct tagTVTV_CAT
{
	HUINT32					ulCat;
	struct tagTVTV_CAT		*pstNext;
} XsvcEpg_TvtvCat_t;



typedef struct
{
	HUINT32					ulProgId;								// mandatory. Unique.
	HUINT16					ulTvtvId;								// mandatory. Unique. Range 1~32767
	UNIXTIME				ulStart;
	UNIXTIME				ulEnd;
//	HUINT8					szStart[TVTV_TIME_MAX_LEN + 1]; 			// ?. UTC : <YYYY>-<MM>-<DD>T<HH>:<MM>
//	HUINT8					szEnd[TVTV_TIME_MAX_LEN + 1];			// ?. UTC : <YYYY>-<MM>-<DD>T<HH>:<MM>
	HUINT32					ulPil;									// ?
	HUINT8					szTitle[TVTV_MAX_TITLE + 1];				// ?. If over 50, truncated.
	HUINT8					*pcOtitle;								// ?. If over 50, truncated. MAX 50.
	HUINT8					szLtext[TVTV_MAX_LONGTEXT + 1];			// ?
	HUINT8					*pcDtext;								// ?. MAX 400.
	HUINT32					ulSId;									// ?. Unique.
	HUINT8					szSnr[TVTV_MAX_SNR + 1];				// ?
	HUINT32					ulSct;									// ?
	HUINT8					szCountry[TVTV_MAX_COUNTRY + 1];		// ?
	HUINT8					szYear[TVTV_MAX_YEAR + 1];				// ?
	HUINT8					ucFsk;									// ?
	HUINT8					ucFormat;								// ?
	XsvcEpg_TvtvName_t				*pstName;								// *
	XsvcEpg_TvtvContent_t			*pstContent;								// *
	XsvcEpg_TvtvImage_t			*pstImageInfo;							// ?
	HUINT32					ulVer;									// ?
	XsvcEpg_TvtvLink_t				*pstLinkInfo;							// *
	XsvcEpg_TvtvSubtitle_t			*pstSubtitle;								// *
	XsvcEpg_TvtvAudio_t			*pstAudio;								// *
	XsvcEpg_TvtvVideo_t			*pstVideo;								// *
	XsvcEpg_TvtvTip_t				*pstTip;									// *
	HINT32	nEventId;								// ?
	XsvcEpg_TvtvCat_t				*pstCat;									// *
	XsvcEpg_TvtvName_t				*pstNameExt;							// *
} XsvcEpg_TvtvEpgData_t;



typedef struct
{
	HUINT8		ucOrgStrLen;
	HUINT8		ucFlagReplace;
	HUINT8		szOrgStr[8];
	HUINT8		ucReplaceCode;
} XsvcEpg_TvtvReplaceStr_t;



/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

STATIC XsvcEpg_TvtvReplaceStr_t		s_astTvtvReplaceString[] =
{
		{	5,		FALSE,	{'&', 'a', 'm', 'p', ';', '\0', 0, },		0x00	},
		{	6,		TRUE,	{'&', 'q', 'u', 'o', 't', ';', '\0', 0, },	0x22	},
		{	4,		TRUE,	{'&', 'g', 't', ';', '\0', 0, },			0x3C	},
		{	4,		TRUE,	{'&', 'l', 't', ';', '\0', 0, },			0x3E	},

		// system specification 2.4.2
		// 5.3 programme data
		{	2, 		TRUE, 	{'\\', 'n', '\0', 0, }, 				0x0A	},
		{	2, 		TRUE, 	{'\\', 't', '\0', 0, }, 				0x09	},
		{	1, 		TRUE, 	{'\\', '\0', 0, },		 			0x5C	},
		{	0, 		FALSE, 	{0, }, 						0x00	}
};


STATIC mwUtIso639Font	s_astSvcEpg_TvtvISO639EncodingTable[] =
{
	{	"sqi",		"alb",		"alb",		"\x05",			1	},		//"Albanian"
	{	"cat",		"cat",		"cat",		"\x05",			1	},		//"Catalan"
	{	"dan",		"dan",		"dan",		"\x05",			1	},		//"Danish"
	{	"nla",		"dut",		"nld",		"\x05",			1	},		//"Dutch"
	{	"eng",		"eng",		"eng",		"\x05",			1	},		//"English"
	{	"fao",		"fao",		"fao",		"\x05",			1	},		//"Faroese"
	{	"fin",		"fin",		"fin",		"\x05",			1	},		//"Finnish"
	{	"fra",		"fre",		"fre",		"\x05",			1	},		//"French"
	{	"deu",		"ger",		"ger",		"\x05",			1	},		//"German"
	{	"gai",		"iri",		"gle",		"\x05",			1	},		//"Irish"
	{	"isl",		"ice",		"ice",		"\x05",			1	},		//"Icelandic"
	{	"ita",		"ita",		"ita",		"\x05",			1	},		//"Italian"
	{	"nor",		"nor",		"nor",		"\x05",			1	},		//"Norwegian"
	{	"por",		"por",		"por",		"\x05",			1	},		//"Portuguese"
	{	"esl",		"spa",		"esp",		"\x05",			1	},		//"Spanish"
	{	"sve",		"swe",		"swe",		"\x05",			1	},		//"Swedish"
	{	"tur",		"tur",		"tur",		"\x05",			1	},		//"Turkish"
	{	"rus",		"rus",		"rus",		"\x01", 		1	},		//"rusia"
	{	"ces",		"cze",		"cze",		"\x10\x00\x02",	3	},		//"Czech"  : 체코는 6937로 처리하고자 한다.
	{	"hrv",		"scr",		"scr",		"\x10\x00\x02",	3	},		//"Croatian"
	{	"hun",		"hun",		"hun",		"\x10\x00\x02",	3	},		//"Hungrian"
	{	"pol",		"pol",		"pol",		"\x10\x00\x02",	3	},		//"Polish"
	{	"ron",		"rum",		"rum",		"\x10\x00\x02",	3	},		//"Romanian"
	{	"slk",		"slo",		"slo",		"\x10\x00\x02",	3	},		//"Slovak"
	{	"slv",		"slv",		"slv",		"\x10\x00\x02",	3	},		//"Slovenian"

	{	"ell",		"gre",		"gre",		"\x10\x00\x07",	3	},		//"Greek"
	{	"eur",		"eur",		"eur",		"\x10\x00\x0F",	3	},		//"8859-15"
	{	"\0\0\0",	"\0\0\0",	"\0\0\0",	"",				0	},		// no matching iso code
};




STATIC HUINT8						s_szTvtvFileName[TVTV_MAX_FILE_FULLPATH_LENGTH];
STATIC HUINT8*						s_pucTvtvCompBuffer = NULL;
STATIC HUINT8*						s_pucTvtvDecompBuffer = NULL;

STATIC POINTER_LIST_T*				s_pstTvtvTvstList = NULL;
STATIC POINTER_LIST_T*				s_pstTvtvDgktList = NULL;
STATIC XsvcEpg_TvtvSystemInfo_t		s_stTvtvSystemInfo;


//STATIC POINTER_LIST_T*			s_pTvtvTsmtList = NULL;
STATIC POINTER_LIST_T*				s_pstTvtvLtotList = NULL;
//STATIC POINTER_LIST_T*			s_pTvtvEpgList = NULL;


STATIC HULONG						s_ulTvtvBufSemaphore = 0;


/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/



STATIC HERROR		xsvc_epglib_GetFileNameBuffer_Tvtv(HUINT8 **ppszFileName);
STATIC HERROR		xsvc_epglib_GetCompBuffer_Tvtv(HUINT8 **ppBuffer);
STATIC HERROR		xsvc_epglib_GetDecompBuffer_Tvtv(HUINT8 **ppBuffer);
STATIC HERROR		xsvc_epglib_GetSystemInfo_Tvtv(XsvcEpg_TvtvSystemInfo_t *pSystemInfo);
STATIC HERROR		xsvc_epglib_SetSystemInfo_Tvtv(XsvcEpg_TvtvSystemInfo_t *pSystemInfo);
STATIC HERROR		xsvc_epglib_GetTvstInfo_Tvtv(	XsvcEpg_TvtvServiceTable_t *pTvstInfo);
STATIC HERROR		xsvc_epglib_SetTvstList_Tvtv(POINTER_LIST_T *pTvstList);
STATIC HERROR		xsvc_epglib_GetDgktInfo_Tvtv(HUINT16 usTvSid, XsvcEpg_TvtvDgkt_t *pDgktInfo);
STATIC HERROR		xsvc_epglib_SetDgktList_Tvtv(POINTER_LIST_T *pDgktList);
STATIC HERROR		xsvc_epglib_GetLtotList_Tvtv(POINTER_LIST_T **ppLtotList);
STATIC HERROR		xsvc_epglib_SetLtotList_Tvtv(POINTER_LIST_T *pLtotList);
#if 0
STATIC HERROR		xsvc_epglib_SetDataByType_Tvtv(	HUINT32 ulType,
													HUINT8 *pucSrc,
													HUINT8 *pucLandCode,
													HUINT32 ulLandCodeLen,
													HUINT32 ulMaxLen,
													HUINT8 *pszString,
													HUINT32 *pulDecimal);
#endif
STATIC HERROR		xsvc_epglib_ConvertStringTimeToUnixTime_Tvtv (HUINT8 *pucDateTime, UNIXTIME *pUnixtime);
STATIC HUINT32	xsvc_epglib_Hextoi_Tvtv (HUINT8 *pucPtr);
STATIC HERROR		xsvc_epglib_IsReplaceString_Tvtv(HUINT8 *pucPtr, HBOOL *pbIsReplace, HUINT8 *pucSize, HUINT8 *puReplaceCode);
STATIC HERROR		xsvc_epglib_SelectIso639LanguageEncoding_Tvtv(HUINT8 *pucISO639Code, HUINT8 *pucLangCode, HUINT32 *pulLen);
STATIC HERROR		xsvc_epglib_ParseTvst_Tvtv(HUINT8 *pRawBuffer, HUINT32 ulBufferSize, POINTER_LIST_T **ppTvstList);
STATIC HERROR		xsvc_epglib_ParseDgkt_Tvtv(HUINT8 *pRawBuffer, HUINT32 ulBufferSize, POINTER_LIST_T **ppDgktList);
STATIC HBOOL		xsvc_epglib_FilterDgktByTvSid_Tvtv(void *pvItem, void *pvFilter);
STATIC void		xsvc_epglib_FreeTvstInfo_Tvtv(void* pvItem);
STATIC void		xsvc_epglib_FreeDgktInfo_Tvtv(void* pvItem);
STATIC void		xsvc_epglib_FreeTsmtInfo_Tvtv(void* pvItem);
STATIC void		xsvc_epglib_FreeLtotInfo_Tvtv(void* pvItem);
STATIC void		xsvc_epglib_FreeEpgDataInfo_Tvtv(void* pvItem);
STATIC HERROR		xsvc_epglib_TrimString_Tvtv(HUINT8 *pucSource, HUINT32 ulStartIndex, HUINT32 ulNumOfCh, HUINT8 *pucTrimString);

#if 1 // 다시 확인
// Compile 문데로인해 : 임시로 배치
#include <stdio.h>

#define	TVTV_FILEIO_READONLY			"rb"
#define	TVTV_FILEIO_WRITEONLY			"wb"


STATIC HERROR		local_tvtvlib_fopen(HUINT8 *pszFileName, HUINT8 *ucMode, HFILEHANDLE *phFileHandle);
STATIC HERROR		local_tvtvlib_fread(void *pBuffer, HUINT32 ulUnitSize, HUINT32 ulUnitNum, HFILEHANDLE hFileHandle, HUINT32 *pulReadSize);
#if 		defined(TEMP_SAVE_FILE)
STATIC HERROR		local_tvtvlib_fwrite(void *pBuffer, HUINT32 ulUnitSize, HUINT32 ulUnitNum, HFILEHANDLE hFileHandle, HUINT32 *pulWriteSize);
#endif
STATIC HERROR		local_tvtvlib_fclose(HFILEHANDLE hFileHandle);
STATIC HERROR		local_tvtvlib_filelength(HFILEHANDLE hFileHandle, HUINT32 *pulFileLength);
STATIC HERROR		local_tvtvlib_ReadFileToBuffer(HUINT8 *pszFileName, HUINT8 *pBuffer, HUINT32 *pulReadSize);
#endif

#define	______GLOBAL_FUNC_START______


HERROR		xsvc_epglib_InitBuffer_Tvtv(void)
{
	HERROR					hError = ERR_TVTV_FAIL;

	ENTER_CRITICAL_TVTVBUF;

	if (s_pucTvtvCompBuffer != NULL)
	{
		OxEPG_Free(s_pucTvtvCompBuffer);
		s_pucTvtvCompBuffer = NULL;
	}

	if (s_pucTvtvDecompBuffer != NULL)
	{
		OxEPG_Free(s_pucTvtvDecompBuffer);
		s_pucTvtvDecompBuffer = NULL;
	}

	if(s_stTvtvSystemInfo.pszName)
	{
		OxEPG_Free(s_stTvtvSystemInfo.pszName);
	}

	HxSTD_memset(&s_stTvtvSystemInfo, 0x00, sizeof(XsvcEpg_TvtvSystemInfo_t));

	hError = ERR_TVTV_OK;


	LEAVE_CRITICAL_TVTVBUF;

	return hError;
}




HERROR		xsvc_epglib_CreateBuffer_Tvtv(void)
{
	HERROR					hError = ERR_TVTV_FAIL;

	if(s_ulTvtvBufSemaphore == 0)
	{
		HINT32		lResult = VK_ERROR;
		lResult = VK_SEM_Create((unsigned long*)&s_ulTvtvBufSemaphore, (const char*)"epg_tvtv_semid", VK_SUSPENDTYPE_PRIORITY);
		if(lResult != VK_OK)
		{
			HxLOG_Print("[xsvc_epglib_CreateBuffer_Tvtv] Comp VK_SEM_Create alloc fail !!\n");
			return hError;
		}
	}

	ENTER_CRITICAL_TVTVBUF;

	s_pucTvtvCompBuffer = (HUINT8 *)OxEPG_Malloc (TVTV_MAX_LEN_COMPDATA * sizeof(HUINT8));
	if (s_pucTvtvCompBuffer == NULL)
	{
		HxLOG_Print("[xsvc_epglib_CreateBuffer_Tvtv] Comp buffer alloc fail !!\n");
		goto EXIT_TVTVLIB_FUNC;
	}
	VK_memset32(s_pucTvtvCompBuffer, 0x00, TVTV_MAX_LEN_COMPDATA * sizeof(HUINT8));

	s_pucTvtvDecompBuffer = (HUINT8 *)OxEPG_Malloc (TVTV_MAX_LEN_DECOMPDATA * sizeof(HUINT8));
	if (s_pucTvtvDecompBuffer == NULL)
	{
		HxLOG_Print("[xsvc_epglib_CreateBuffer_Tvtv] Decomp buffer alloc fail !!\n");
		goto EXIT_TVTVLIB_FUNC;
	}
	VK_memset32(s_pucTvtvDecompBuffer, 0x00, TVTV_MAX_LEN_DECOMPDATA * sizeof(HUINT8));

	VK_memset32(&s_stTvtvSystemInfo, 0x00, sizeof(XsvcEpg_TvtvSystemInfo_t));

	hError = ERR_TVTV_OK;

EXIT_TVTVLIB_FUNC :

	LEAVE_CRITICAL_TVTVBUF;

	return hError;
}



HERROR		xsvc_epglib_CreateEpgInfo_Tvtv(HBOOL bNeedConverting)
{
	HERROR					hError = ERR_TVTV_FAIL;
	HUINT32					ulReadSize = 0;
	HUINT32					ulBufferSize = 0;
	HUINT32					ulIndex = 0;
	HUINT32					ulDecompLen = 0;
	HUINT8					*pCompBuffer = NULL;
	HUINT8					*pDecompBuffer = NULL;
	HUINT8					*pszFileName = NULL;
	HUINT16					usFileIndex = 0;
	POINTER_LIST_T			*pEpgList = NULL;
	HBOOL					bJpeg = FALSE;
	HUINT8					szLang[TVTV_MAX_LANG + 1];
	HUINT8					aucOrgFileName[128] = {0, };

	ENTER_CRITICAL_TVTVBUF;

	HxSTD_memset(szLang, 0x00, sizeof(HUINT8)*(TVTV_MAX_LANG + 1));

	// epg files loop
	for(usFileIndex = 0; usFileIndex < s_stTvtvSystemInfo.usFileCnt; usFileIndex++)
	{
		bJpeg = FALSE;

		hError = xsvc_epglib_GetFileNameBuffer_Tvtv(&pszFileName);
		if(hError != ERR_TVTV_OK)
		{
			HxLOG_Print("[xsvc_epglib_CreateEpgInfo_Tvtv] pszFileName NULL!!\n");
			continue;
		}

		snprintf ((char *)aucOrgFileName, 128, "%04X%04X%s",	s_stTvtvSystemInfo.usTvSid,
													usFileIndex,
													TVTV_FILENAME_XMLEXTENSION);

		snprintf ((char *)pszFileName, 256, "%s%s", EPG_FS_TVTV_DATA_PATH, aucOrgFileName);

		if(bNeedConverting == TRUE)
		{
			hError = xsvc_epglib_GetCompBuffer_Tvtv(&pCompBuffer);
			if(hError != ERR_TVTV_OK || pCompBuffer == NULL)
			{
				HxLOG_Print("[xsvc_epglib_CreateEpgInfo_Tvtv] GetCompBuffer fail!!\n");
				goto EXIT_TVTVLIB_FUNC;
			}

			hError = local_tvtvlib_ReadFileToBuffer(pszFileName, pCompBuffer, &ulReadSize);
			if(hError != ERR_TVTV_OK)
			{
				HxLOG_Print("[xsvc_epglib_CreateEpgInfo_Tvtv] ReadFileToBuffer (%s)fail!!\n", pszFileName);

				snprintf ((char *)aucOrgFileName, 128, "%04X%04X%s",	s_stTvtvSystemInfo.usTvSid,
															usFileIndex,
															TVTV_FILENAME_JPGEXTENSION);

				snprintf ((char *)pszFileName, 256, "%s%s", EPG_FS_TVTV_IMAGE_PATH, aucOrgFileName);


				hError = local_tvtvlib_ReadFileToBuffer(pszFileName, pCompBuffer, &ulReadSize);
				if(hError != ERR_TVTV_OK)
				{
					HxLOG_Print("[xsvc_epglib_CreateEpgInfo_Tvtv] ReadFileToBuffer (%s)fail!!\n", pszFileName);
					continue;
				}
				bJpeg = TRUE;
			}
			ulBufferSize = ulReadSize;

			///////////////////////////////////////////////////////
			// decryption
			///////////////////////////////////////////////////////
			if(s_stTvtvSystemInfo.ucEncType != 0)
			{
				// s_stTvtvSystemInfo.aucSsk[TVTV_DES_SIZE_BLOCK] : because of LBS
				hError = PAL_CRYPT_Desinit(&s_stTvtvSystemInfo.aucSsk[TVTV_DES_SIZE_BLOCK]);
				if(hError != ERR_OK)
				{
					HxLOG_Print("[xsvc_epglib_CreateEpgInfo_Tvtv] PAL_CRYPT_Desinit fail!!\n");
					continue;
				}

				for(ulIndex = 0; ulIndex < s_stTvtvSystemInfo.usEncLen; ulIndex++)
				{
					hError = PAL_CRYPT_DesDecryptProcess(&pCompBuffer[ulIndex*TVTV_DES_SIZE_BLOCK], &pCompBuffer[ulIndex*TVTV_DES_SIZE_BLOCK]);
					if(hError != ERR_OK)
					{
						HxLOG_Print("[xsvc_epglib_CreateEpgInfo_Tvtv] PAL_CRYPT_DesDecryptProcess fail!!\n");
						break;
					}
				}
				if(hError != ERR_OK)
				{
					continue;
				}
			}

			///////////////////////////////////////////////////////
			// decompression
			///////////////////////////////////////////////////////
			if(s_stTvtvSystemInfo.ucCompType != 0 && bJpeg == FALSE)
			{
				ulDecompLen = TVTV_MAX_LEN_DECOMPDATA;
				hError = xsvc_epglib_GetDecompBuffer_Tvtv(&pDecompBuffer);
				if(hError != ERR_TVTV_OK || pDecompBuffer == NULL)
				{
					HxLOG_Print("[xsvc_epglib_CreateEpgInfo_Tvtv] GetDecompBuffer fail!!\n");
					goto EXIT_TVTVLIB_FUNC;
				}

				hError = HLIB_ZLIB_Uncompress(pDecompBuffer, &ulDecompLen, pCompBuffer, ulReadSize);

				if(hError != ERR_TVTV_OK)
				{
					HxLOG_Print("[xsvc_epglib_CreateEpgInfo_Tvtv] Uncompress fail!!\n");
					continue;
				}
				ulBufferSize = ulDecompLen;
			}
		}
		else
		{
			hError = xsvc_epglib_GetDecompBuffer_Tvtv(&pDecompBuffer);
			if(hError != ERR_TVTV_OK || pDecompBuffer == NULL)
			{
				HxLOG_Print("[xsvc_epglib_CreateEpgInfo_Tvtv] GetDecompBuffer fail!!\n");
				goto EXIT_TVTVLIB_FUNC;
			}

			hError = local_tvtvlib_ReadFileToBuffer(pszFileName, pDecompBuffer, &ulReadSize);
			if(hError != ERR_TVTV_OK)
			{
				snprintf ((char *)aucOrgFileName, 128, "%04X%04X%s",	s_stTvtvSystemInfo.usTvSid,
															usFileIndex,
															TVTV_FILENAME_JPGEXTENSION);

				snprintf ((char *)pszFileName, 256, "%s%s", EPG_FS_TVTV_IMAGE_PATH, aucOrgFileName);

				hError = local_tvtvlib_ReadFileToBuffer(pszFileName, pDecompBuffer, &ulReadSize);
				if(hError != ERR_TVTV_OK)
				{
					HxLOG_Print("[xsvc_epglib_CreateEpgInfo_Tvtv] ReadFileToBuffer (%s)fail!!\n", pszFileName);
					continue;
				}
				bJpeg = TRUE;
			}
			ulBufferSize = ulReadSize;
		}

#if 		defined(TEMP_SAVE_FILE)
{
	HUINT8					szFileName[256] = {0, };
	HFILEHANDLE				hFileHandle = NULL;
	HUINT32					ulWriteSize = 0;
	HUINT8					*pucTempBuffer = NULL;

	pucTempBuffer = pDecompBuffer;
	if(bJpeg)
	{
		pucTempBuffer = pCompBuffer;
	}

	if(pucTempBuffer == NULL || ulBufferSize == 0)
	{
		continue;
	}

	{
		sprintf ((char *)szFileName, "%s%s", ".\\mnt\\hd2\\POST_TVTV\\", aucOrgFileName);
		hError = local_tvtvlib_fopen(szFileName, TVTV_FILEIO_WRITEONLY, &hFileHandle);
		if(hError != ERR_OK)
		{
			continue;
		}

		hError = local_tvtvlib_fwrite(pucTempBuffer, ulBufferSize, 1, hFileHandle, &ulWriteSize);
		if(hError != ERR_OK || ulBufferSize != ulWriteSize)
		{
			continue;
		}

		hError = local_tvtvlib_fclose(hFileHandle);
		if(hError != ERR_TVTV_OK)
		{
			continue;
		}
		HxLOG_Print("[xsvc_epglib_CreateEpgInfo_Tvtv] file name(%s)!!\n", szFileName);
	}
	continue;
}
#endif

		if(bJpeg == FALSE)
		{
			hError = xsvc_epglib_ParseEpg_Tvtv(pDecompBuffer, ulBufferSize, &pEpgList, szLang);
			if(hError != ERR_TVTV_OK)
			{
				HxLOG_Print("[xsvc_epglib_CreateEpgInfo_Tvtv] ParseEpg fail!!\n");
				continue;
			}
		}
		else
		{
			// jpep => bitmap or jpeg save
			continue;

		}

		//epginfo converting

#if		defined(TVTV_CMD_ON)
{
	POINTER_LIST_T		*pTempList = NULL;
	XsvcEpg_TvtvEpgData_t		*pEpgData = NULL;

	pTempList = pEpgList;

	while(pTempList)
	{
		pEpgData = UTIL_LINKEDLIST_GetContents(pTempList);
		HxLOG_Print("[TVTV] EPGDATA : ulProgId(0x%04X), ulStart(0x%04X), ulEnd(0x%04X), szTitle(%s), szYear(%s)\n",
								pEpgData->ulProgId,
								pEpgData->ulStart,
								pEpgData->ulEnd,
								pEpgData->szTitle,
								pEpgData->szYear);

		pTempList = UTIL_LINKEDLIST_GetNextPointer(pTempList);
	}
}
#endif


		pEpgList = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList(pEpgList, xsvc_epglib_FreeEpgDataInfo_Tvtv);
		if(pEpgList != NULL)
		{
			HxLOG_Print("[xsvc_epglib_CreateEpgInfo_Tvtv] FreeWholeListItemsFromPointerList fail!!\n");
		}
	}

	//hdd save routine


	hError = ERR_TVTV_OK;

EXIT_TVTVLIB_FUNC :

	LEAVE_CRITICAL_TVTVBUF;

	return hError;
}




HERROR		xsvc_epglib_CreateLogoImage_Tvtv(HBOOL bNeedConverting)
{
	HERROR					hError = ERR_TVTV_FAIL;
	HUINT32					ulReadSize = 0;
	HUINT32					ulBufferSize = 0;
	HUINT32					ulIndex = 0;
	HUINT8					*pDecompBuffer = NULL;
	HUINT8					*pszFileName = NULL;
	HUINT16					usFileIndex = 0;
	HUINT8					aucOrgFileName[128] = {0, };

	ENTER_CRITICAL_TVTVBUF;

	// epg files loop
	for(usFileIndex = TVTV_COUNT_MIN_LOGO; usFileIndex < TVTV_COUNT_MAX_LOGO; usFileIndex++)
	{
		hError = xsvc_epglib_GetFileNameBuffer_Tvtv(&pszFileName);
		if(hError != ERR_TVTV_OK)
		{
			HxLOG_Print("[xsvc_epglib_CreateLogoImage_Tvtv] pszFileName NULL!!\n");
			continue;
		}
		snprintf((char *)aucOrgFileName, 128, "%04X%04X%s",	s_stTvtvSystemInfo.usTvSid,
													usFileIndex,
													TVTV_FILENAME_JPGEXTENSION);

		snprintf ((char *)pszFileName, 256, "%s%s",	EPG_FS_TVTV_IMAGE_PATH,aucOrgFileName);

		if(bNeedConverting == TRUE)
		{
			hError = xsvc_epglib_GetDecompBuffer_Tvtv(&pDecompBuffer);
			if(hError != ERR_TVTV_OK || pDecompBuffer == NULL)
			{
				HxLOG_Print("[xsvc_epglib_CreateLogoImage_Tvtv] GetCompBuffer fail!!\n");
				goto EXIT_TVTVLIB_FUNC;
			}

			hError = local_tvtvlib_ReadFileToBuffer(pszFileName, pDecompBuffer, &ulReadSize);
			if(hError != ERR_TVTV_OK)
			{
				HxLOG_Print("[xsvc_epglib_CreateLogoImage_Tvtv] ReadFileToBuffer (%s)fail!!\n", pszFileName);
				continue;
			}
			ulBufferSize = ulReadSize;

			///////////////////////////////////////////////////////
			// decryption
			///////////////////////////////////////////////////////
			if(s_stTvtvSystemInfo.ucEncType != 0)
			{
				// s_stTvtvSystemInfo.aucSsk[TVTV_DES_SIZE_BLOCK] : because of LBS
				hError = PAL_CRYPT_Desinit(&s_stTvtvSystemInfo.aucSsk[TVTV_DES_SIZE_BLOCK]);
				if(hError != ERR_OK)
				{
					HxLOG_Print("[xsvc_epglib_CreateLogoImage_Tvtv] PAL_CRYPT_Desinit fail!!\n");
					continue;
				}

				for(ulIndex = 0; ulIndex < s_stTvtvSystemInfo.usEncLen; ulIndex++)
				{
					hError = PAL_CRYPT_DesDecryptProcess(&pDecompBuffer[ulIndex*TVTV_DES_SIZE_BLOCK], &pDecompBuffer[ulIndex*TVTV_DES_SIZE_BLOCK]);
					if(hError != ERR_OK)
					{
						HxLOG_Print("[xsvc_epglib_CreateLogoImage_Tvtv] PAL_CRYPT_DesDecryptProcess fail!!\n");
						break;
					}
				}
				if(hError != ERR_OK)
				{
					continue;
				}
			}
		}
		else
		{
			hError = xsvc_epglib_GetDecompBuffer_Tvtv(&pDecompBuffer);
			if(hError != ERR_TVTV_OK || pDecompBuffer == NULL)
			{
				HxLOG_Print("[xsvc_epglib_CreateLogoImage_Tvtv] GetDecompBuffer fail!!\n");
				goto EXIT_TVTVLIB_FUNC;
			}

			hError = local_tvtvlib_ReadFileToBuffer(pszFileName, pDecompBuffer, &ulReadSize);
			if(hError != ERR_TVTV_OK)
			{
				HxLOG_Print("[xsvc_epglib_CreateLogoImage_Tvtv] ReadFileToBuffer (%s)fail!!\n", pszFileName);
				continue;
			}
			ulBufferSize = ulReadSize;
		}
		NOT_USED_PARAM(ulBufferSize);
#if 		defined(TEMP_SAVE_FILE)
{
	HUINT8					szFileName[256] = {0, };
	HFILEHANDLE				hFileHandle = NULL;
	HUINT32					ulWriteSize = 0;

	if(pDecompBuffer == NULL || ulBufferSize == 0)
	{
		continue;
	}

	{
		sprintf ((char *)szFileName, "%s%s", ".\\mnt\\hd2\\POST_TVTV\\", aucOrgFileName);
		hError = local_tvtvlib_fopen(szFileName, TVTV_FILEIO_WRITEONLY, &hFileHandle);
		if(hError != ERR_OK)
		{
			continue;
		}

		hError = local_tvtvlib_fwrite(pDecompBuffer, ulBufferSize, 1, hFileHandle, &ulWriteSize);
		if(hError != ERR_OK || ulBufferSize != ulWriteSize)
		{
			continue;
		}

		hError = local_tvtvlib_fclose(hFileHandle);
		if(hError != ERR_TVTV_OK)
		{
			continue;
		}
		HxLOG_Print("[xsvc_epglib_CreateEpgInfo_Tvtv] file name(%s)!!\n", szFileName);
	}
	continue;
}
#endif
		//bitmap or jpeg save

	}

	//hdd save routine

	hError = ERR_TVTV_OK;

EXIT_TVTVLIB_FUNC :

	LEAVE_CRITICAL_TVTVBUF;

	return hError;
}



HERROR		xsvc_epglib_CreateTsmtInfo_Tvtv(HBOOL bNeedConverting)
{
	HERROR					hError = ERR_TVTV_FAIL;
	HUINT32					ulReadSize = 0;
	HUINT32					ulBufferSize = 0;
	HUINT32					ulIndex = 0;
	HUINT32					ulDecompLen = 0;
	HUINT8					*pCompBuffer = NULL;
	HUINT8					*pDecompBuffer = NULL;
	HUINT8					*pszFileName = NULL;
	HUINT8					ucFileIndex = 0;
	POINTER_LIST_T			*pRootList = NULL;
	POINTER_LIST_T			*pTsmtList = NULL;
	POINTER_LIST_T			*pRest = NULL;
	HUINT8					aucOrgFileName[128] = {0, };

	ENTER_CRITICAL_TVTVBUF;

	// TSMT files loop
	for(ucFileIndex = 0; ucFileIndex < TVTV_COUNT_MAX_TSMT; ucFileIndex++)
	{
		hError = xsvc_epglib_GetFileNameBuffer_Tvtv(&pszFileName);
		if(hError != ERR_TVTV_OK)
		{
			HxLOG_Print("[xsvc_epglib_CreateTsmtInfo_Tvtv] pszFileName NULL!!\n");
			continue;
		}

		snprintf ((char *)aucOrgFileName, 128, "%04X%s%02X%s",	s_stTvtvSystemInfo.usTvSid,
														TVTV_FILENAME_TSMT_MIDDLE,
														ucFileIndex,
														TVTV_FILENAME_XMLEXTENSION);

		snprintf ((char *)pszFileName, 256, "%s%s",	EPG_FS_TVTV_DATA_PATH, aucOrgFileName);

		if(bNeedConverting == TRUE)
		{
			hError = xsvc_epglib_GetCompBuffer_Tvtv(&pCompBuffer);
			if(hError != ERR_TVTV_OK || pCompBuffer == NULL)
			{
				HxLOG_Print("[xsvc_epglib_CreateTsmtInfo_Tvtv] GetCompBuffer fail!!\n");
				goto EXIT_TVTVLIB_FUNC;
			}

			hError = local_tvtvlib_ReadFileToBuffer(pszFileName, pCompBuffer, &ulReadSize);
			if(hError != ERR_TVTV_OK)
			{
				HxLOG_Print("[xsvc_epglib_CreateTsmtInfo_Tvtv] ReadFileToBuffer fail!!\n");
				continue;
			}
			ulBufferSize = ulReadSize;

			///////////////////////////////////////////////////////
			// decryption
			///////////////////////////////////////////////////////
			if(s_stTvtvSystemInfo.ucEncType != 0)
			{
				// s_stTvtvSystemInfo.aucSsk[TVTV_DES_SIZE_BLOCK] : because of LBS
				hError = PAL_CRYPT_Desinit(&s_stTvtvSystemInfo.aucSsk[TVTV_DES_SIZE_BLOCK]);
				if(hError != ERR_OK)
				{
					HxLOG_Print("[xsvc_epglib_CreateTsmtInfo_Tvtv] PAL_CRYPT_Desinit fail!!\n");
					continue;
				}

				for(ulIndex = 0; ulIndex < s_stTvtvSystemInfo.usEncLen; ulIndex++)
				{
					hError = PAL_CRYPT_DesDecryptProcess(&pCompBuffer[ulIndex*TVTV_DES_SIZE_BLOCK], &pCompBuffer[ulIndex*TVTV_DES_SIZE_BLOCK]);
					if(hError != ERR_OK)
					{
						HxLOG_Print("[xsvc_epglib_CreateTsmtInfo_Tvtv] PAL_CRYPT_DesDecryptProcess fail!!\n");
						break;
					}
				}
				if(hError != ERR_OK)
				{
					continue;
				}
			}

			///////////////////////////////////////////////////////
			// decompression
			///////////////////////////////////////////////////////
			if(s_stTvtvSystemInfo.ucCompType != 0)
			{
				ulDecompLen = TVTV_MAX_LEN_DECOMPDATA;
				hError = xsvc_epglib_GetDecompBuffer_Tvtv(&pDecompBuffer);
				if(hError != ERR_TVTV_OK || pDecompBuffer == NULL)
				{
					HxLOG_Print("[xsvc_epglib_CreateTsmtInfo_Tvtv] GetDecompBuffer fail!!\n");
					goto EXIT_TVTVLIB_FUNC;
				}

				hError = HLIB_ZLIB_Uncompress(pDecompBuffer, &ulDecompLen, pCompBuffer, ulReadSize);

				if(hError != ERR_TVTV_OK)
				{
					HxLOG_Print("[xsvc_epglib_CreateTsmtInfo_Tvtv] Uncompress fail!!\n");
					continue;
				}
				ulBufferSize = ulDecompLen;
			}
		}
		else
		{
			hError = xsvc_epglib_GetDecompBuffer_Tvtv(&pDecompBuffer);
			if(hError != ERR_TVTV_OK || pDecompBuffer == NULL)
			{
				HxLOG_Print("[xsvc_epglib_CreateTsmtInfo_Tvtv] GetDecompBuffer fail!!\n");
				goto EXIT_TVTVLIB_FUNC;
			}

			hError = local_tvtvlib_ReadFileToBuffer(pszFileName, pDecompBuffer, &ulReadSize);
			if(hError != ERR_TVTV_OK)
			{
				HxLOG_Print("[xsvc_epglib_CreateTsmtInfo_Tvtv] ReadFileToBuffer fail!!\n");
				continue;
			}
			ulBufferSize = ulReadSize;
		}
#if 		defined(TEMP_SAVE_FILE)
{
	HUINT8					szFileName[256] = {0, };
	HFILEHANDLE				hFileHandle = NULL;
	HUINT32					ulWriteSize = 0;

	if(pDecompBuffer == NULL || ulBufferSize == 0)
	{
		continue;
	}

	{
		sprintf ((char *)szFileName, "%s%s", ".\\mnt\\hd2\\POST_TVTV\\", aucOrgFileName);
		hError = local_tvtvlib_fopen(szFileName, TVTV_FILEIO_WRITEONLY, &hFileHandle);
		if(hError != ERR_OK)
		{
			continue;
		}

		hError = local_tvtvlib_fwrite(pDecompBuffer, ulBufferSize, 1, hFileHandle, &ulWriteSize);
		if(hError != ERR_OK || ulBufferSize != ulWriteSize)
		{
			continue;
		}

		hError = local_tvtvlib_fclose(hFileHandle);
		if(hError != ERR_TVTV_OK)
		{
			continue;
		}
		HxLOG_Print("[xsvc_epglib_CreateEpgInfo_Tvtv] file name(%s)!!\n", szFileName);
	}
	continue;
}
#endif


		hError = xsvc_epglib_ParseTsmt_Tvtv(pDecompBuffer, ulBufferSize, &pTsmtList);
		if(hError != ERR_TVTV_OK)
		{
			HxLOG_Print("[xsvc_epglib_CreateTsmtInfo_Tvtv] ParseTsmt fail!!\n");
			continue;
		}

		pRootList = UTIL_LINKEDLIST_CombineTwoPointerLists(pRootList, pTsmtList, NULL, NULL, &pRest);
		if(pRootList == NULL)
		{
			HxLOG_Print("[xsvc_epglib_CreateTsmtInfo_Tvtv] Combine fail!!\n");
			continue;
		}
	}

#if	defined(TVTV_CMD_ON)
{
	XsvcEpg_TvtvTsmt_t		*pTsmt = NULL;

	while(pRootList)
	{
		pTsmt = UTIL_LINKEDLIST_GetContents(pRootList);

		HxLOG_Print("[TVTV] TSMT : usTvtvId(%08d), usOnId(%08d), usSId(%08d), usTsId(%08d), szSname(%s), szLang(%s)\n",
														pTsmt->usTvtvId,
														pTsmt->usOnId,
														pTsmt->usSId,
														pTsmt->usTsId,
														pTsmt->szSname,
														pTsmt->szLang);
		pRootList = UTIL_LINKEDLIST_GetNextPointer(pRootList);
	}
}
#endif

	hError = ERR_TVTV_OK;

EXIT_TVTVLIB_FUNC :

	LEAVE_CRITICAL_TVTVBUF;

	return hError;
}


HERROR		xsvc_epglib_DestroyTsmtInfo_Tvtv(POINTER_LIST_T *pTsmtList)
{
	HERROR					hError = ERR_TVTV_FAIL;


	pTsmtList = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList(pTsmtList, xsvc_epglib_FreeTsmtInfo_Tvtv);
	if(pTsmtList != NULL)
	{
		HxLOG_Print("[xsvc_epglib_DestroyTsmtInfo_Tvtv] Tsmt list destory fail!!\n");
		goto EXIT_TVTVLIB_FUNC;
	}

	hError = ERR_TVTV_OK;

EXIT_TVTVLIB_FUNC :

	return hError;
}


HERROR		xsvc_epglib_CreateLtotInfo_Tvtv(HBOOL bNeedConverting)
{
	HERROR					hError = ERR_TVTV_FAIL;
	HUINT32					ulReadSize = 0;
	HUINT32					ulBufferSize = 0;
	HUINT32					ulIndex = 0;
	HUINT32					ulDecompLen = 0;
	HUINT8					*pCompBuffer = NULL;
	HUINT8					*pDecompBuffer = NULL;
	HUINT8					*pszFileName = NULL;
	POINTER_LIST_T			*pLtotList = NULL;
	HUINT8					aucOrgFileName[128] = {0, };

	ENTER_CRITICAL_TVTVBUF;

	// LTOT files loop
	hError = xsvc_epglib_GetFileNameBuffer_Tvtv(&pszFileName);
	if(hError != ERR_TVTV_OK)
	{
		HxLOG_Print("[xsvc_epglib_CreateLtotInfo_Tvtv] pszFileName NULL!!\n");
		goto EXIT_TVTVLIB_FUNC;
	}

	snprintf ((char *)aucOrgFileName, 128, "%04X%s",	s_stTvtvSystemInfo.usTvSid,
											TVTV_FILENAME_LTOT);

	snprintf ((char *)pszFileName, 256, "%s%s",			EPG_FS_TVTV_DATA_PATH,
											aucOrgFileName);


	if(bNeedConverting == TRUE)
	{
		hError = xsvc_epglib_GetCompBuffer_Tvtv(&pCompBuffer);
		if(hError != ERR_TVTV_OK || pCompBuffer == NULL)
		{
			HxLOG_Print("[xsvc_epglib_CreateLtotInfo_Tvtv] GetCompBuffer fail!!\n");
			goto EXIT_TVTVLIB_FUNC;
		}

		hError = local_tvtvlib_ReadFileToBuffer(pszFileName, pCompBuffer, &ulReadSize);
		if(hError != ERR_TVTV_OK)
		{
			HxLOG_Print("[xsvc_epglib_CreateLtotInfo_Tvtv] ReadFileToBuffer fail!!\n");
			goto EXIT_TVTVLIB_FUNC;
		}
		ulBufferSize = ulReadSize;

		///////////////////////////////////////////////////////
		// decryption
		///////////////////////////////////////////////////////
		if(s_stTvtvSystemInfo.ucEncType != 0)
		{
			// s_stTvtvSystemInfo.aucSsk[TVTV_DES_SIZE_BLOCK] : because of LBS
			hError = PAL_CRYPT_Desinit(&s_stTvtvSystemInfo.aucSsk[TVTV_DES_SIZE_BLOCK]);
			if(hError != ERR_OK)
			{
				HxLOG_Print("[xsvc_epglib_CreateLtotInfo_Tvtv] PAL_CRYPT_Desinit fail!!\n");
				goto EXIT_TVTVLIB_FUNC;
			}

			for(ulIndex = 0; ulIndex < s_stTvtvSystemInfo.usEncLen; ulIndex++)
			{
				hError = PAL_CRYPT_DesDecryptProcess(&pCompBuffer[ulIndex*TVTV_DES_SIZE_BLOCK], &pCompBuffer[ulIndex*TVTV_DES_SIZE_BLOCK]);
				if(hError != ERR_OK)
				{
					HxLOG_Print("[xsvc_epglib_CreateLtotInfo_Tvtv] PAL_CRYPT_DesDecryptProcess fail!!\n");
					goto EXIT_TVTVLIB_FUNC;
				}
			}
		}

		///////////////////////////////////////////////////////
		// decompression
		///////////////////////////////////////////////////////
		if(s_stTvtvSystemInfo.ucCompType != 0)
		{
			ulDecompLen = TVTV_MAX_LEN_DECOMPDATA;
			hError = xsvc_epglib_GetDecompBuffer_Tvtv(&pDecompBuffer);
			if(hError != ERR_TVTV_OK || pDecompBuffer == NULL)
			{
				HxLOG_Print("[xsvc_epglib_CreateLtotInfo_Tvtv] GetDecompBuffer fail!!\n");
				goto EXIT_TVTVLIB_FUNC;
			}

			hError = HLIB_ZLIB_Uncompress(pDecompBuffer, &ulDecompLen, pCompBuffer, ulReadSize);

			if(hError != ERR_TVTV_OK)
			{
				HxLOG_Print("[xsvc_epglib_CreateLtotInfo_Tvtv] Uncompress fail!!\n");
				goto EXIT_TVTVLIB_FUNC;
			}
			ulBufferSize = ulDecompLen;
		}
	}
	else
	{
		hError = xsvc_epglib_GetDecompBuffer_Tvtv(&pDecompBuffer);
		if(hError != ERR_TVTV_OK || pDecompBuffer == NULL)
		{
			HxLOG_Print("[xsvc_epglib_CreateLtotInfo_Tvtv] GetDecompBuffer fail!!\n");
			goto EXIT_TVTVLIB_FUNC;
		}

		hError = local_tvtvlib_ReadFileToBuffer(pszFileName, pDecompBuffer, &ulReadSize);
		if(hError != ERR_TVTV_OK)
		{
			HxLOG_Print("[xsvc_epglib_CreateLtotInfo_Tvtv] ReadFileToBuffer fail!!\n");
			goto EXIT_TVTVLIB_FUNC;
		}
		ulBufferSize = ulReadSize;
	}
#if 		defined(TEMP_SAVE_FILE)
{
	HUINT8					szFileName[256] = {0, };
	HFILEHANDLE				hFileHandle = NULL;
	HUINT32					ulWriteSize = 0;

	if(pDecompBuffer == NULL || ulBufferSize == 0)
	{
		goto EXIT_TVTVLIB_FUNC;
	}

	{
		sprintf ((char *)szFileName, "%s%s", EPG_FS_TVTV_DATA_PATH, aucOrgFileName);
		hError = local_tvtvlib_fopen(szFileName, TVTV_FILEIO_WRITEONLY, &hFileHandle);
		if(hError != ERR_OK)
		{
			goto EXIT_TVTVLIB_FUNC;
		}

		hError = local_tvtvlib_fwrite(pDecompBuffer, ulBufferSize, 1, hFileHandle, &ulWriteSize);
		if(hError != ERR_OK || ulBufferSize != ulWriteSize)
		{
			goto EXIT_TVTVLIB_FUNC;
		}

		hError = local_tvtvlib_fclose(hFileHandle);
		if(hError != ERR_TVTV_OK)
		{
			goto EXIT_TVTVLIB_FUNC;
		}
		HxLOG_Print("[xsvc_epglib_CreateLtotInfo_Tvtv] file name(%s)!!\n", szFileName);
	}
	hError = ERR_TVTV_OK;

	goto EXIT_TVTVLIB_FUNC;
}
#endif


	hError = xsvc_epglib_ParseLtot_Tvtv(pDecompBuffer, ulBufferSize, &pLtotList);
	if(hError != ERR_TVTV_OK)
	{
		HxLOG_Print("[xsvc_epglib_CreateLtotInfo_Tvtv] ParseLtot fail!!\n");
		goto EXIT_TVTVLIB_FUNC;
	}

	hError = xsvc_epglib_SetLtotList_Tvtv(pLtotList);
	if(hError != ERR_TVTV_OK)
	{
		HxLOG_Print("[xsvc_epglib_CreateLtotInfo_Tvtv] SetTsmtList fail!!\n");
		goto EXIT_TVTVLIB_FUNC;
	}


#if	defined(TVTV_CMD_ON)
{
	XsvcEpg_TvtvLtot_t		*pLtot = NULL;

	while(pLtotList)
	{
		pLtot = UTIL_LINKEDLIST_GetContents(pLtotList);

		HxLOG_Print("[TVTV] LTOT : szCoc(%s), ucCor(0x%04X), ulToc(0x%04X), szToc(%s)\n",
																					pLtot->szCoc,
																					pLtot->ucCor,
																					pLtot->ulToc,
																					pLtot->szToc);
		pLtotList = UTIL_LINKEDLIST_GetNextPointer(pLtotList);
	}
}
#endif

	hError = ERR_TVTV_OK;

EXIT_TVTVLIB_FUNC :

	LEAVE_CRITICAL_TVTVBUF;

	return hError;
}

HERROR		xsvc_epglib_DestroyLtotInfo_Tvtv(void)
{
	HERROR					hError = ERR_TVTV_FAIL;
	POINTER_LIST_T			*pRoot = NULL;

	hError = xsvc_epglib_GetLtotList_Tvtv(&pRoot);
	if(hError != ERR_TVTV_OK)
	{
		HxLOG_Print("[xsvc_epglib_DestroyLtotInfo_Tvtv] GetLtotList fail!!\n");
		goto EXIT_TVTVLIB_FUNC;
	}

	pRoot = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList(pRoot, xsvc_epglib_FreeLtotInfo_Tvtv);
	if(pRoot != NULL)
	{
		HxLOG_Print("[xsvc_epglib_DestroyLtotInfo_Tvtv] Ltot list destory fail!!\n");
		goto EXIT_TVTVLIB_FUNC;
	}

	s_pstTvtvLtotList = NULL;

	hError = ERR_TVTV_OK;

EXIT_TVTVLIB_FUNC :

	return hError;
}





HERROR		xsvc_epglib_CreateTvtvSystemInfo_Tvtv(HBOOL bNeedConverting)
{
	HERROR					hError = ERR_TVTV_FAIL;
	XsvcEpg_TvtvServiceTable_t		stTvst;
	XsvcEpg_TvtvDgkt_t				stDgkt;
	XsvcEpg_TvtvSystemInfo_t 		stSystemInfo;

	HxSTD_memset(&stTvst, 0x00, sizeof(XsvcEpg_TvtvServiceTable_t));
	HxSTD_memset(&stDgkt, 0x00, sizeof(XsvcEpg_TvtvDgkt_t));
	HxSTD_memset(&stSystemInfo, 0x00, sizeof(XsvcEpg_TvtvSystemInfo_t));


	hError = xsvc_epglib_CreateTvstInfo_Tvtv();
	if(hError != ERR_TVTV_OK)
	{
		HxLOG_Print("[xsvc_epglib_CreateTvtvSystemInfo_Tvtv] CreateTvstInfo fail!!\n");
		goto EXIT_TVTVLIB_FUNC;
	}

	hError = xsvc_epglib_CreateDgktInfo_Tvtv(bNeedConverting);
	if(hError != ERR_TVTV_OK)
	{
		HxLOG_Print("[xsvc_epglib_CreateTvtvSystemInfo_Tvtv] CreateDgktInfo fail!!\n");
		goto EXIT_TVTVLIB_FUNC;
	}

	hError = xsvc_epglib_GetTvstInfo_Tvtv(&stTvst);
	if(hError != ERR_TVTV_OK)
	{
		HxLOG_Print("[xsvc_epglib_CreateTvtvSystemInfo_Tvtv] GetTvstInfo fail!!\n");
		goto EXIT_TVTVLIB_FUNC;
	}

	hError = xsvc_epglib_GetDgktInfo_Tvtv(stTvst.usTvSid, &stDgkt);
	if(hError != ERR_TVTV_OK)
	{
		HxLOG_Print("[xsvc_epglib_CreateTvtvSystemInfo_Tvtv] GetTvstInfo fail!!\n");
		goto EXIT_TVTVLIB_FUNC;
	}


	stSystemInfo.usTvSid = stTvst.usTvSid;
	stSystemInfo.ucType = stTvst.ucType;
	stSystemInfo.ulVersion = stTvst.ulVersion;
	stSystemInfo.usFileCnt = stTvst.usFileCnt;
	stSystemInfo.ucEncType = stDgkt.ucEncType;
	stSystemInfo.usEncLen = stDgkt.usEncLen;
	stSystemInfo.ucCompType = stDgkt.ucCompType;
	VK_memcpy32(stSystemInfo.aucSsk, stDgkt.aucSsk, sizeof(HUINT8)*TVTV_DGKT_SIZE_SSK);
	stSystemInfo.pszName = OxEPG_Malloc(sizeof(HUINT8)*(stTvst.ucNameLen+1));
	if(stSystemInfo.pszName == NULL)
	{
		HxLOG_Print("[xsvc_epglib_CreateTvtvSystemInfo_Tvtv] name alloc fail!!\n");
		goto EXIT_TVTVLIB_FUNC;
	}
	HxSTD_memcpy(stSystemInfo.pszName, stTvst.pszName, sizeof(HUINT8)*(stTvst.ucNameLen+1));


	hError = xsvc_epglib_SetSystemInfo_Tvtv(&stSystemInfo);
	if(hError != ERR_TVTV_OK)
	{
		HxLOG_Print("[xsvc_epglib_CreateTvtvSystemInfo_Tvtv] SetSystemInfo fail!!\n");
		goto EXIT_TVTVLIB_FUNC;
	}

	hError = xsvc_epglib_DestroyTvstInfo_Tvtv();
	if(hError != ERR_TVTV_OK)
	{
		HxLOG_Print("[xsvc_epglib_CreateTvtvSystemInfo_Tvtv] DestoryTvstInfo fail!!\n");
	}

	hError = xsvc_epglib_DestroyDgktInfo_Tvtv();
	if(hError != ERR_TVTV_OK)
	{
		HxLOG_Print("[xsvc_epglib_CreateTvtvSystemInfo_Tvtv] DestoryDgktInfo fail!!\n");
	}

	hError = ERR_TVTV_OK;

EXIT_TVTVLIB_FUNC :

	return hError;

}




/**
  @brief     function creating tVtV service table file.

  simple comment \n

  @return    humax error.
  */
HERROR		xsvc_epglib_CreateTvstInfo_Tvtv(void)
{
	HERROR					hError = ERR_TVTV_FAIL;
	HUINT32					ulReadSize = 0;

	POINTER_LIST_T			*pTvstList = NULL;
	HUINT8					*pDecompBuffer = NULL;
	HUINT8					*pszFileName = NULL;
	HUINT8					aucOrgFileName[128] = {0, };

	ENTER_CRITICAL_TVTVBUF;

	hError = xsvc_epglib_GetFileNameBuffer_Tvtv(&pszFileName);
	if(hError != ERR_TVTV_OK)
	{
		HxLOG_Print("[xsvc_epglib_CreateTvstInfo_Tvtv] pszFileName NULL!!\n");
		goto EXIT_TVTVLIB_FUNC;
	}

	snprintf ((char *)aucOrgFileName, 128, "%s", "00000000.tvs");
	snprintf ((char *)pszFileName, 256, "%s%s", EPG_FS_TVTV_DATA_PATH, aucOrgFileName);

	hError = xsvc_epglib_GetDecompBuffer_Tvtv(&pDecompBuffer);
	if(hError != ERR_TVTV_OK || pDecompBuffer == NULL)
	{
		HxLOG_Print("[xsvc_epglib_CreateTvstInfo_Tvtv] pDecompBuffer NULL!!\n");
		goto EXIT_TVTVLIB_FUNC;
	}

	hError = local_tvtvlib_ReadFileToBuffer(pszFileName, pDecompBuffer, &ulReadSize);
	if(hError != ERR_TVTV_OK)
	{
		HxLOG_Print("[xsvc_epglib_CreateTvstInfo_Tvtv] file read fail !!\n");
		goto EXIT_TVTVLIB_FUNC;
	}

	hError = xsvc_epglib_ParseTvst_Tvtv(pDecompBuffer, ulReadSize, &pTvstList);
	if(hError != ERR_TVTV_OK)
	{
		HxLOG_Print("[xsvc_epglib_CreateTvstInfo_Tvtv] Tvst parsing fail !!\n");
		goto EXIT_TVTVLIB_FUNC;
	}

	hError = xsvc_epglib_SetTvstList_Tvtv(pTvstList);
	if(hError != ERR_TVTV_OK)
	{
		HxLOG_Print("[xsvc_epglib_CreateTvstInfo_Tvtv] Tvst list setting fail !!\n");
		goto EXIT_TVTVLIB_FUNC;
	}


#if	defined(TVTV_CMD_ON)
{
	XsvcEpg_TvtvServiceTable_t		*pServiceTable = NULL;

	while(pTvstList)
	{
		pServiceTable = UTIL_LINKEDLIST_GetContents(pTvstList);

		HxLOG_Print("[TVTV] SvcTbl : Id(0x%04X), Type(%d), Ver(%d), FileCnt(%d), Start(%d), End(%d), Name(%s)\n",	pServiceTable->usTvSid,
																													pServiceTable->ucType,
																													pServiceTable->ulVersion,
																													pServiceTable->usFileCnt,
																													pServiceTable->ulStart,
																													pServiceTable->ulEnd,
																													pServiceTable->pszName );
		pTvstList = UTIL_LINKEDLIST_GetNextPointer(pTvstList);
	}
}
#endif

	hError = ERR_TVTV_OK;

EXIT_TVTVLIB_FUNC :

	LEAVE_CRITICAL_TVTVBUF;

	return hError;
}




#define	______TEMP_FILEIO_START______
STATIC HERROR		local_tvtvlib_fopen(HUINT8 *pszFileName, HUINT8 *ucMode, HFILEHANDLE *phFileHandle)
{
	FILE				*pFile = NULL;
	HERROR			hError = ERR_TVTV_FAIL;

	if(pszFileName == NULL || ucMode == NULL || phFileHandle == NULL)
	{
		HxLOG_Print("[local_tvtvlib_fopen] Param NULL!!\n");
		goto EXIT_TVTVLIB_FUNC;
	}

	*phFileHandle = NULL;

	pFile = fopen((const char*)pszFileName, (const char*)ucMode);
	if(pFile == NULL)
	{
		HxLOG_Print("[local_tvtvlib_fopen] fopen fail!!\n");
		goto EXIT_TVTVLIB_FUNC;
	}

	*phFileHandle =(HFILEHANDLE)pFile ;
	hError = ERR_TVTV_OK;

EXIT_TVTVLIB_FUNC :

	return hError;

}


STATIC HERROR		local_tvtvlib_fread(void *pBuffer, HUINT32 ulUnitSize, HUINT32 ulUnitNum, HFILEHANDLE hFileHandle, HUINT32 *pulReadSize)
{
	HERROR			hError = ERR_TVTV_FAIL;
	HUINT32			ulReadUnit = 0;

	if(pBuffer == NULL || hFileHandle == NULL || pulReadSize == NULL)
	{
		HxLOG_Print("[local_tvtvlib_fread] Param NULL!!\n");
		goto EXIT_TVTVLIB_FUNC;
	}

	*pulReadSize = 0;

	//ulReadUnit = (HUINT32)fread((const void *)pBuffer, (size_t)ulUnitSize, (size_t)ulUnitNum, (FILE*)hFileHandle);
	ulReadUnit = (HUINT32)fread(pBuffer, ulUnitSize, ulUnitNum, hFileHandle);
	if(ulReadUnit == 0)
	{
		HxLOG_Print("[local_tvtvlib_fread] reading fail 1!!\n");
		goto EXIT_TVTVLIB_FUNC;
	}
	else if(ulReadUnit != ulUnitNum)
	{
		HxLOG_Print("[local_tvtvlib_fread] reading fail 2!!\n");
	}

	*pulReadSize = ulUnitSize*ulReadUnit;
	hError = ERR_TVTV_OK;

EXIT_TVTVLIB_FUNC :

	return hError;

}

#if 		defined(TEMP_SAVE_FILE)
STATIC HERROR		local_tvtvlib_fwrite(void *pBuffer, HUINT32 ulUnitSize, HUINT32 ulUnitNum, HFILEHANDLE hFileHandle, HUINT32 *pulWriteSize)
{
	HERROR			hError = ERR_TVTV_FAIL;
	HUINT32			ulWriteNum = 0;

	if(pBuffer == NULL || hFileHandle == NULL || pulWriteSize == NULL)
	{
		HxLOG_Print("[local_tvtvlib_fwrite] Param NULL!!\n");
		goto EXIT_TVTVLIB_FUNC;
	}

	*pulWriteSize = 0;

	ulWriteNum = (HUINT32)fwrite((void *)pBuffer, (size_t)ulUnitSize, (size_t)ulUnitNum, (FILE*)hFileHandle);
	if(ulWriteNum == 0)
	{
		HxLOG_Print("[local_tvtvlib_fwrite] writing fail 1!!\n");
		goto EXIT_TVTVLIB_FUNC;
	}
	else if(ulWriteNum != ulUnitNum)
	{
		HxLOG_Print("[local_tvtvlib_fwrite] writing fail 2!!\n");
	}

	*pulWriteSize = ulUnitSize*ulWriteNum;
	hError = ERR_TVTV_OK;

EXIT_TVTVLIB_FUNC :

	return hError;

}
#endif

STATIC HERROR		local_tvtvlib_fclose(HFILEHANDLE hFileHandle)
{
	HERROR			hError = ERR_TVTV_FAIL;

	if( hFileHandle == NULL)
	{
		HxLOG_Print("[local_tvtvlib_fclose] Param NULL!!\n");
		goto EXIT_TVTVLIB_FUNC;
	}

	fclose((FILE*)hFileHandle);

	hError = ERR_TVTV_OK;

EXIT_TVTVLIB_FUNC :

	return hError;
}


STATIC HERROR		local_tvtvlib_filelength(HFILEHANDLE hFileHandle, HUINT32 *pulFileLength)
{
	HERROR			hError = ERR_TVTV_FAIL;
	HINT32			ulRet = 0;
	HINT32			ulLength = 0;

	if( hFileHandle == NULL || pulFileLength == NULL)
	{
		HxLOG_Print("[local_tvtvlib_filelength] Param NULL!!\n");
		goto EXIT_TVTVLIB_FUNC;
	}

	*pulFileLength = 0;

	ulRet = (HINT32)fseek((FILE*)hFileHandle, 0, SEEK_END);
	if(ulRet != 0)
	{
		HxLOG_Print("[local_tvtvlib_filelength] fseek SEEK_END fail!!\n");
		goto EXIT_TVTVLIB_FUNC;
	}

	ulLength = (HINT32)ftell((FILE*)hFileHandle);
	if(ulLength == -1)
	{
		HxLOG_Print("[local_tvtvlib_filelength] ftell fail!!\n");
		goto EXIT_TVTVLIB_FUNC;
	}

	ulRet = (HINT32)fseek((FILE*)hFileHandle, 0, SEEK_SET);
	if(ulRet != 0)
	{
		HxLOG_Print("[local_tvtvlib_filelength] fseek SEEK_SET fail!!\n");
		goto EXIT_TVTVLIB_FUNC;
	}

	*pulFileLength = ulLength;
	hError = ERR_TVTV_OK;

EXIT_TVTVLIB_FUNC :

	return hError;

}


STATIC HERROR		local_tvtvlib_ReadFileToBuffer(HUINT8 *pszFileName, HUINT8 *pBuffer, HUINT32 *pulReadSize)
{
	HERROR			hError = ERR_TVTV_FAIL;
	HFILEHANDLE		hFileHandle = NULL;
	HUINT32			ulFileSize = 0;
	HUINT32			ulSizeToRead = 0;


	if( pszFileName == NULL || pBuffer == NULL || pulReadSize == NULL)
	{
		HxLOG_Print("[local_tvtvlib_ReadFileToBuffer] Param NULL!!\n");
		goto EXIT_TVTVLIB_FUNC;
	}

	*pulReadSize = 0;

	hError = local_tvtvlib_fopen(pszFileName, TVTV_FILEIO_READONLY, &hFileHandle);
	if(hError != ERR_TVTV_OK)
	{
		HxLOG_Print("[local_tvtvlib_ReadFileToBuffer] file open fail!!\n");
		goto EXIT_TVTVLIB_FUNC;
	}

	hError = local_tvtvlib_filelength(hFileHandle, &ulFileSize);
	if(hError != ERR_TVTV_OK)
	{
		HxLOG_Print("[local_tvtvlib_ReadFileToBuffer] filelength read fail!!\n");
		goto EXIT_TVTVLIB_FUNC;
	}

	hError = local_tvtvlib_fread(pBuffer, ulFileSize, 1, hFileHandle, &ulSizeToRead);
	if(hError != ERR_TVTV_OK)
	{
		HxLOG_Print("[local_tvtvlib_ReadFileToBuffer] file read fail !!\n");
		goto EXIT_TVTVLIB_FUNC;
	}


	*pulReadSize = ulSizeToRead;
	hError = ERR_TVTV_OK;

EXIT_TVTVLIB_FUNC :
	if(hFileHandle != NULL)
	{
		hError = local_tvtvlib_fclose(hFileHandle);
		if(hError != ERR_TVTV_OK)
		{
			HxLOG_Print("[local_tvtvlib_ReadFileToBuffer] file close fail !!\n");
		}
	}

	return hError;

}


#define	______TEMP_FILEIO_END______




#define	______LOCAL_FUNC_START______



#define	______LOCAL_FUNC_POSTPROCESSING______
HERROR		xsvc_epglib_GetTvtvFileType_Tvtv(HUINT8 *pucFileName, xsvcEpg_TvtvFileType_e *pTvtvFileType)
{
	HUINT8				szTrimString[TVTV_FILENAME_LENGTH + 1] = {0, };
	HERROR				hError = ERR_FAIL;
	xsvcEpg_TvtvFileType_e		eType = TVTV_FileType_MAX;
	HUINT32				ulFileNumber = 0;

	if(pucFileName == NULL || pTvtvFileType == NULL)
	{
		HxLOG_Print("[xsvc_epglib_GetTvtvFileType_Tvtv] param fail!!\n");
		goto EXIT_TVTVLIB_FUNC;
	}
	*pTvtvFileType = eType;

	if(!MWC_UTIL_DvbStrcmp(pucFileName, TVTV_FILENAME_LTOT))
	{
		*pTvtvFileType = TVTV_FileType_LTOT;
		hError = ERR_OK;

		goto EXIT_TVTVLIB_FUNC;
	}
	else if(!MWC_UTIL_DvbStrcmp(pucFileName, TVTV_FILENAME_DSI_TABLE))
	{
		*pTvtvFileType = TVTV_FileType_EPG_DSI;
		hError = ERR_OK;

		goto EXIT_TVTVLIB_FUNC;
	}

/*************************************************************************************
 *
 * trim extension
 *
 *************************************************************************************/
	hError = xsvc_epglib_TrimString_Tvtv(pucFileName, 8, 4, szTrimString);
	if(hError != ERR_OK)
	{
		HxLOG_Print("[xsvc_epglib_GetTvtvFileType_Tvtv] xsvc_epglib_TrimString_Tvtv fail!!\n");
		goto EXIT_TVTVLIB_FUNC;
	}

	if(!MWC_UTIL_DvbStrcmp(szTrimString, TVTV_FILENAME_TABLEEXTENSION))
	{
		eType = TVTV_FileType_EPG_TABLE;
	}
	else if(!MWC_UTIL_DvbStrcmp(szTrimString, TVTV_FILENAME_TVSTEXTENSION))
	{
		if(!MWC_UTIL_DvbStrcmp(pucFileName, TVTV_FILENAME_TVST))
		{
			eType = TVTV_FileType_TVST;
		}
		else
		{
			HxLOG_Print("[xsvc_epglib_GetTvtvFileType_Tvtv] TVTV_FILENAME_TVSTEXTENSION else!!\n");
			goto EXIT_TVTVLIB_FUNC;
		}

	}
	else if(!MWC_UTIL_DvbStrcmp(szTrimString, TVTV_FILENAME_DGKTEXTENSION))
	{
		if(!MWC_UTIL_DvbStrcmp(pucFileName, TVTV_FILENAME_DGKT))
		{
			eType = TVTV_FileType_DGKT;
		}
		else
		{
			HxLOG_Print("[xsvc_epglib_GetTvtvFileType_Tvtv] TVTV_FILENAME_DGKTEXTENSION else!!\n");
			goto EXIT_TVTVLIB_FUNC;
		}
	}
	else if(!MWC_UTIL_DvbStrcmp(szTrimString, TVTV_FILENAME_XMLEXTENSION))
	{
		hError = xsvc_epglib_TrimString_Tvtv(pucFileName, 4, 2, szTrimString);
		if(hError != ERR_OK)
		{
			HxLOG_Print("[xsvc_epglib_GetTvtvFileType_Tvtv] xsvc_epglib_TrimString_Tvtv fail!!\n");
			goto EXIT_TVTVLIB_FUNC;
		}

		if(!MWC_UTIL_DvbStrcmp(szTrimString, TVTV_FILENAME_TSMT_MIDDLE))
		{
			eType = TVTV_FileType_TSMT;
		}
		else
		{
			eType = TVTV_FileType_EPG_EVENT;
		}
	}
	else if(!MWC_UTIL_DvbStrcmp(szTrimString, TVTV_FILENAME_JPGEXTENSION))
	{
		hError = xsvc_epglib_TrimString_Tvtv(pucFileName, 4, 4, szTrimString);
		if(hError != ERR_OK)
		{
			HxLOG_Print("[xsvc_epglib_GetTvtvFileType_Tvtv] xsvc_epglib_TrimString_Tvtv fail!!\n");
			goto EXIT_TVTVLIB_FUNC;
		}
		ulFileNumber = xsvc_epglib_Hextoi_Tvtv(szTrimString);
		if(ulFileNumber >= TVTV_COUNT_MIN_LOGO && ulFileNumber <= TVTV_COUNT_MAX_LOGO)
		{
			eType = TVTV_FileType_LOGO;
		}
		else
		{
			eType = TVTV_FileType_EPG_IMAGE;
		}

	}
	else
	{
		HxLOG_Print("[xsvc_epglib_GetTvtvFileType_Tvtv] else!!\n");
		goto EXIT_TVTVLIB_FUNC;
	}

	*pTvtvFileType = eType;

	hError = ERR_OK;

EXIT_TVTVLIB_FUNC :

	return hError;
}






#define	______LOCAL_FUNC_INIT______




#define	______TVTV_GET_FUNC______
STATIC HERROR		xsvc_epglib_GetFileNameBuffer_Tvtv(HUINT8 **ppszFileName)
{
	HERROR			hError = ERR_TVTV_FAIL;


	if(ppszFileName == NULL)
	{
		HxLOG_Print("[xsvc_epglib_GetFileNameBuffer_Tvtv] Param NULL!!\n");
		goto EXIT_TVTVLIB_FUNC;
	}

	HxSTD_memset(s_szTvtvFileName, 0x00, sizeof(HUINT8)*TVTV_MAX_FILE_FULLPATH_LENGTH);
	*ppszFileName = s_szTvtvFileName;
	hError = ERR_TVTV_OK;

EXIT_TVTVLIB_FUNC :

	return hError;
}


STATIC HERROR		xsvc_epglib_GetCompBuffer_Tvtv(HUINT8 **ppBuffer)
{
	HERROR					hError = ERR_TVTV_FAIL;

	if( ppBuffer == NULL)
	{
		HxLOG_Error("[xsvc_epglib_GetCompBuffer_Tvtv] Param NULL!!\n");
		goto EXIT_TVTVLIB_FUNC;
	}
	*ppBuffer = NULL;

	if(s_pucTvtvCompBuffer == NULL)
	{
		HxLOG_Error("[xsvc_epglib_GetCompBuffer_Tvtv] s_pucTvtvCompBuffer NULL!!\n");
		goto EXIT_TVTVLIB_FUNC;
	}

	*ppBuffer = s_pucTvtvCompBuffer;

	hError = ERR_TVTV_OK;

EXIT_TVTVLIB_FUNC :

	return hError;
}



STATIC HERROR		xsvc_epglib_GetDecompBuffer_Tvtv(HUINT8 **ppBuffer)
{
	HERROR					hError = ERR_TVTV_FAIL;

	if( ppBuffer == NULL)
	{
		HxLOG_Print("[xsvc_epglib_GetDecompBuffer_Tvtv] Param NULL!!\n");
		goto EXIT_TVTVLIB_FUNC;
	}
	*ppBuffer = NULL;

	if(s_pucTvtvDecompBuffer == NULL)
	{
		HxLOG_Error("[xsvc_epglib_GetDecompBuffer_Tvtv] s_pucTvtvDecompBuffer NULL!!\n");
		goto EXIT_TVTVLIB_FUNC;
	}

	*ppBuffer = s_pucTvtvDecompBuffer;

	hError = ERR_TVTV_OK;

EXIT_TVTVLIB_FUNC :

	return hError;
}



STATIC HERROR		xsvc_epglib_GetSystemInfo_Tvtv(XsvcEpg_TvtvSystemInfo_t *pSystemInfo)
{
	HERROR					hError = ERR_TVTV_FAIL;

	ENTER_CRITICAL_TVTVBUF;

	if( pSystemInfo == NULL)
	{
		HxLOG_Print("[xsvc_epglib_GetSystemInfo_Tvtv] Param NULL!!\n");
		goto EXIT_TVTVLIB_FUNC;
	}

	if( s_stTvtvSystemInfo.ulVersion == 0)
	{
		HxLOG_Print("[xsvc_epglib_GetSystemInfo_Tvtv] invalid version!!\n");
		goto EXIT_TVTVLIB_FUNC;
	}

	HxSTD_memcpy(pSystemInfo, &s_stTvtvSystemInfo, sizeof(XsvcEpg_TvtvSystemInfo_t));

	hError = ERR_TVTV_OK;

EXIT_TVTVLIB_FUNC :
	LEAVE_CRITICAL_TVTVBUF;

	return hError;
}


STATIC HERROR		xsvc_epglib_GetTvstInfo_Tvtv(	XsvcEpg_TvtvServiceTable_t *pTvstInfo)
{
	HERROR					hError = ERR_TVTV_FAIL;
	POINTER_LIST_T			*pRoot = NULL;
	XsvcEpg_TvtvServiceTable_t 	*pTempInfo = NULL;

	if( pTvstInfo == NULL)
	{
		HxLOG_Print("[xsvc_epglib_GetTvstInfo_Tvtv] Param NULL!!\n");
		goto EXIT_TVTVLIB_FUNC;
	}

	pRoot = s_pstTvtvTvstList;
	pTempInfo = (XsvcEpg_TvtvServiceTable_t*)UTIL_LINKEDLIST_GetContents(pRoot);
	if(pTempInfo == NULL)
	{
		HxLOG_Print("[xsvc_epglib_GetTvstInfo_Tvtv] Info NULL!!\n");
		goto EXIT_TVTVLIB_FUNC;
	}

	HxSTD_memcpy(pTvstInfo, pTempInfo, sizeof(XsvcEpg_TvtvServiceTable_t));

	hError = ERR_TVTV_OK;

EXIT_TVTVLIB_FUNC :

	return hError;
}



STATIC HERROR		xsvc_epglib_GetDgktInfo_Tvtv(HUINT16 usTvSid, XsvcEpg_TvtvDgkt_t *pDgktInfo)
{
	HERROR					hError = ERR_TVTV_FAIL;
	POINTER_LIST_T			*pFound = NULL;
	POINTER_LIST_T			*pRootNode = NULL;
	XsvcEpg_TvtvDgkt_t 			*pTempInfo = NULL;

	if( pDgktInfo == NULL)
	{
		HxLOG_Print("[xsvc_epglib_GetDgktInfo_Tvtv] Param NULL!!\n");
		goto EXIT_TVTVLIB_FUNC;
	}

	pRootNode = s_pstTvtvDgktList;

	pFound = UTIL_LINKEDLIST_IsListItemInPointerList(pRootNode, NULL, NULL, xsvc_epglib_FilterDgktByTvSid_Tvtv, (void*)&usTvSid);
	if(pFound == NULL)
	{
		HxLOG_Print("[xsvc_epglib_GetDgktInfo_Tvtv] not found !!\n");
		goto EXIT_TVTVLIB_FUNC;
	}

	pTempInfo = (XsvcEpg_TvtvDgkt_t*)UTIL_LINKEDLIST_GetContents(pFound);
	if(pTempInfo == NULL)
	{
		HxLOG_Print("[xsvc_epglib_GetDgktInfo_Tvtv] info is NULL!!\n");
		goto EXIT_TVTVLIB_FUNC;
	}

	HxSTD_memcpy(pDgktInfo, pTempInfo, sizeof(XsvcEpg_TvtvDgkt_t));

	hError = ERR_TVTV_OK;

EXIT_TVTVLIB_FUNC :

	return hError;
}




STATIC HERROR		xsvc_epglib_GetLtotList_Tvtv(POINTER_LIST_T **ppLtotList)
{
	HERROR					hError = ERR_TVTV_FAIL;

	if( ppLtotList == NULL)
	{
		HxLOG_Print("[xsvc_epglib_GetLtotList_Tvtv] Param NULL!!\n");
		goto EXIT_TVTVLIB_FUNC;
	}

	*ppLtotList = s_pstTvtvLtotList;

	hError = ERR_TVTV_OK;

EXIT_TVTVLIB_FUNC :

	return hError;
}




#define	______TVTV_SET_FUNC______
STATIC HERROR		xsvc_epglib_SetSystemInfo_Tvtv(XsvcEpg_TvtvSystemInfo_t *pSystemInfo)
{
	HERROR					hError = ERR_TVTV_FAIL;

	ENTER_CRITICAL_TVTVBUF;

	if( pSystemInfo == NULL)
	{
		HxLOG_Print("[xsvc_epglib_SetSystemInfo_Tvtv] Param NULL!!\n");
		goto EXIT_TVTVLIB_FUNC;
	}

	HxSTD_memcpy(&s_stTvtvSystemInfo, pSystemInfo, sizeof(XsvcEpg_TvtvSystemInfo_t));

	hError = ERR_TVTV_OK;

EXIT_TVTVLIB_FUNC :
	LEAVE_CRITICAL_TVTVBUF;

	return hError;

}


STATIC HERROR		xsvc_epglib_SetTvstList_Tvtv(POINTER_LIST_T *pTvstList)
{
	HERROR					hError = ERR_TVTV_FAIL;

	if( pTvstList == NULL)
	{
		HxLOG_Print("[xsvc_epglib_SetTvstList_Tvtv] Param NULL!!\n");
		goto EXIT_TVTVLIB_FUNC;
	}

	s_pstTvtvTvstList = pTvstList;

	hError = ERR_TVTV_OK;

EXIT_TVTVLIB_FUNC :

	return hError;

}

STATIC HERROR		xsvc_epglib_SetDgktList_Tvtv(POINTER_LIST_T *pDgktList)
{
	HERROR					hError = ERR_TVTV_FAIL;

	if( pDgktList == NULL)
	{
		HxLOG_Print("[xsvc_epglib_SetDgktList_Tvtv] Param NULL!!\n");
		goto EXIT_TVTVLIB_FUNC;
	}

	s_pstTvtvDgktList = pDgktList;

	hError = ERR_TVTV_OK;

EXIT_TVTVLIB_FUNC :

	return hError;

}



STATIC HERROR		xsvc_epglib_SetLtotList_Tvtv(POINTER_LIST_T *pLtotList)
{
	HERROR					hError = ERR_TVTV_FAIL;

	if(pLtotList == NULL)
	{
		HxLOG_Print("[xsvc_epglib_SetLtotList_Tvtv] Param NULL!!\n");
		goto EXIT_TVTVLIB_FUNC;
	}

	s_pstTvtvLtotList = pLtotList;

	hError = ERR_TVTV_OK;

EXIT_TVTVLIB_FUNC :

	return hError;

}


HERROR		xsvc_epglib_SetDataByType_Tvtv(	HUINT32 ulType,
													HUINT8 *pucSrc,
													HUINT8 *pucLandCode,
													HUINT32 ulLandCodeLen,
													HUINT32 ulMaxLen,
													HUINT8 *pszString,
													HUINT32 *pulDecimal)
{
	HERROR					hError = ERR_TVTV_FAIL;
	HINT16					sIndex = 0;
	HINT16					sCount = 0;
	HBOOL					bReplace = FALSE;
	HUINT8					ucSize = 0;
	HUINT8					ucReplaceCode = 0;
	HUINT8					*pucTempString = NULL;

	if(pucSrc == NULL)
	{
		HxLOG_Print("[xsvc_epglib_SetDataByType_Tvtv] Param NULL!!\n");
		goto EXIT_TVTVLIB_FUNC;
	}

	switch(ulType)
	{
		case TVTV_TYPE_STRING :
#if 1
			// IP EPG가 ulLandCodeLen를 0로 사용함. 추후 다시 확인 필요
			ulLandCodeLen = 0;
//			HxSTD_memcpy(pszString, pucLandCode, sizeof(HUINT8)*ulLandCodeLen);
#else
			HxSTD_memcpy(pszString, pucLandCode, sizeof(HUINT8)*ulLandCodeLen);
#endif
			pucTempString = &(pszString[ulLandCodeLen]);

			for(sIndex = 0 ; sIndex<  (ulMaxLen - ulLandCodeLen) - 1 ; sIndex++)
			{
				if(pucSrc[sIndex] == '\0')
				{
					break;
				}

				if(sCount >= (ulMaxLen - ulLandCodeLen))
				{
					break;
				}

				if(pucSrc[sIndex] == '\\' || pucSrc[sIndex] == '&')
				{
					bReplace = FALSE;
					ucSize = 0;
					ucReplaceCode = 0;
					hError = xsvc_epglib_IsReplaceString_Tvtv(&pucSrc[sIndex], &bReplace, &ucSize, &ucReplaceCode);
					if(hError != ERR_TVTV_OK)
					{
						HxLOG_Print("[xsvc_epglib_SetDataByType_Tvtv] IsReplaceString fail!!\n");
					}

					if(bReplace == TRUE)
					{
						sIndex += ucSize;
						pucTempString[sCount++] = ucReplaceCode;
						continue;
					}
				}
				pucTempString[sCount++] = pucSrc[sIndex];

			}
			pucTempString[sCount]	= '\0';

			break;

		case TVTV_TYPE_DECIMAL :
			*pulDecimal = atoi((const char *)pucSrc);
			break;

		case TVTV_TYPE_DECIMAL64 :
			*pulDecimal = 0;
			break;

		case TVTV_TYPE_HEXA:
			*pulDecimal = xsvc_epglib_Hextoi_Tvtv(pucSrc);
			break;

		default :
			HxLOG_Print("[xsvc_epglib_SetDataByType_Tvtv] Invalid type(%d)!!\n", ulType);
			goto EXIT_TVTVLIB_FUNC;
	}

	hError = ERR_TVTV_OK;

EXIT_TVTVLIB_FUNC :

	return hError;
}


#define	______TVTV_CREATE_FUNC______





/**
  @brief     function creating tVtV device group key table

  simple comment \n

  @return    humax error.
  */
HERROR		xsvc_epglib_CreateDgktInfo_Tvtv(HBOOL bNeedConverting)
{
	HERROR					hError = ERR_TVTV_FAIL;
	HUINT32					ulReadSize = 0;
	POINTER_LIST_T			*pDgktList = NULL;
	HUINT8					*pDecompBuffer = NULL;
	HUINT8					*pszFileName = NULL;
	HUINT8					aucPrimaryKey[TVTV_PRIMARY_KEYLEN + 1];
	HUINT32					ulIndex = 0;
	HUINT8					aucOrgFileName[128] = {0, };

	ENTER_CRITICAL_TVTVBUF;

	HxSTD_memset(aucPrimaryKey, 0x00, sizeof(HUINT8)*(TVTV_PRIMARY_KEYLEN + 1));

	hError = xsvc_epglib_GetFileNameBuffer_Tvtv(&pszFileName);
	if(hError != ERR_TVTV_OK)
	{
		HxLOG_Print("[xsvc_epglib_CreateDgktInfo_Tvtv] pszFileName NULL!!\n");
		goto EXIT_TVTVLIB_FUNC;
	}

	snprintf ((char *)aucOrgFileName, 128, "%s", "00000333.dkt");
	snprintf ((char *)pszFileName, 256, "%s%s", EPG_FS_TVTV_DATA_PATH, aucOrgFileName);

	hError = xsvc_epglib_GetDecompBuffer_Tvtv(&pDecompBuffer);
	if(hError != ERR_TVTV_OK || pDecompBuffer == NULL)
	{
		HxLOG_Print("[xsvc_epglib_CreateDgktInfo_Tvtv] pDecompBuffer NULL!!\n");
		goto EXIT_TVTVLIB_FUNC;
	}

	hError = local_tvtvlib_ReadFileToBuffer(pszFileName, pDecompBuffer, &ulReadSize);
	if(hError != ERR_TVTV_OK)
	{
		HxLOG_Print("[xsvc_epglib_CreateDgktInfo_Tvtv] file read fail !!\n");
		goto EXIT_TVTVLIB_FUNC;
	}

	if(bNeedConverting == TRUE)
	{
		///////////////////////////////////////////////////////////////////
		// TVTV PLAYOUT System Specification v 2.1.3
		// 5.2 Device group key table
		///////////////////////////////////////////////////////////////////
		HxSTD_memcpy(aucPrimaryKey, TVTV_PRIMARY_KEY, sizeof(HUINT8)*(TVTV_PRIMARY_KEYLEN + 1));
		hError = PAL_CRYPT_Des3init(aucPrimaryKey);
		if(hError != ERR_OK)
		{
			HxLOG_Print("[xsvc_epglib_CreateDgktInfo_Tvtv] PAL_CRYPT_Des3init fail !!\n");
			goto EXIT_TVTVLIB_FUNC;
		}

		for(ulIndex = 0; ulIndex < ulReadSize/TVTV_DES_SIZE_BLOCK; ulIndex++)
		{
			hError = PAL_CRYPT_Des3DecryptProcess(&pDecompBuffer[ulIndex*TVTV_DES_SIZE_BLOCK], &pDecompBuffer[ulIndex*TVTV_DES_SIZE_BLOCK]);
			if(hError != ERR_OK)
			{
				HxLOG_Print("[xsvc_epglib_CreateDgktInfo_Tvtv] PAL_CRYPT_Des3DecryptProcess fail !!\n");
				goto EXIT_TVTVLIB_FUNC;
			}
		}
	}


	hError = xsvc_epglib_ParseDgkt_Tvtv(pDecompBuffer, ulReadSize, &pDgktList);
	if(hError != ERR_TVTV_OK)
	{
		HxLOG_Print("[xsvc_epglib_CreateDgktInfo_Tvtv] Parsing fail !!\n");
		goto EXIT_TVTVLIB_FUNC;
	}

	hError = xsvc_epglib_SetDgktList_Tvtv(pDgktList);
	if(hError != ERR_TVTV_OK)
	{
		HxLOG_Print("[xsvc_epglib_CreateDgktInfo_Tvtv] Tvst list setting fail !!\n");
		goto EXIT_TVTVLIB_FUNC;
	}

#if	defined(TVTV_CMD_ON)
{
	XsvcEpg_TvtvDgkt_t		*pDgkt = NULL;

	while(pDgktList)
	{
		pDgkt = UTIL_LINKEDLIST_GetContents(pDgktList);


		HxLOG_Print("[TVTV] DGKT : usTvSid(0x%04X), ucEncType(%d), usEncLen(%d), ucCompType(%d)\n",	pDgkt->usTvSid,
																											pDgkt->ucEncType,
																											pDgkt->usEncLen,
																											pDgkt->ucCompType );
		pDgktList = UTIL_LINKEDLIST_GetNextPointer(pDgktList);
	}
}
#endif



	hError = ERR_TVTV_OK;

EXIT_TVTVLIB_FUNC :

	LEAVE_CRITICAL_TVTVBUF;

	return hError;

}


#define	______TVTV_DESTROY_FUNC______





HERROR		xsvc_epglib_DestroyTvstInfo_Tvtv(void)
{
	HERROR					hError = ERR_TVTV_FAIL;
	POINTER_LIST_T			*pRoot = NULL;


	pRoot = s_pstTvtvTvstList;

	pRoot = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList(pRoot, xsvc_epglib_FreeTvstInfo_Tvtv);
	if(pRoot != NULL)
	{
		HxLOG_Print("[xsvc_epglib_DestroyTvstInfo_Tvtv] Tvst list destory fail!!\n");
		goto EXIT_TVTVLIB_FUNC;
	}

	s_pstTvtvTvstList = NULL;

	hError = ERR_TVTV_OK;

EXIT_TVTVLIB_FUNC :

	return hError;
}




HERROR		xsvc_epglib_DestroyDgktInfo_Tvtv(void)
{
	HERROR					hError = ERR_TVTV_FAIL;
	POINTER_LIST_T			*pRoot = NULL;


	pRoot = s_pstTvtvDgktList;

	pRoot = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList(pRoot, xsvc_epglib_FreeDgktInfo_Tvtv);
	if(pRoot != NULL)
	{
		HxLOG_Print("[xsvc_epglib_DestroyDgktInfo_Tvtv] Dgkt list destory fail!!\n");
		goto EXIT_TVTVLIB_FUNC;
	}

	s_pstTvtvDgktList = NULL;

	hError = ERR_TVTV_OK;

EXIT_TVTVLIB_FUNC :

	return hError;
}




#define	______TVTV_UTIL_FUNC______


STATIC HERROR		xsvc_epglib_ConvertStringTimeToUnixTime_Tvtv (HUINT8 *pucDateTime, UNIXTIME *pUnixtime)
{
	HxDATETIME_t			stDateTime;
	HINT32					lIndex = 0;
	HERROR					hError = ERR_TVTV_FAIL;
	HUINT8					szDataTime[TVTV_TIME_MAX_LEN + 1];
	HINT16					sCount = 0;

	if(pucDateTime == NULL || pUnixtime == NULL)
	{
		HxLOG_Print("[xsvc_epglib_ConvertStringTimeToUnixTime_Tvtv] Param NULL!!\n");
		goto EXIT_TVTVLIB_FUNC;
	}
	*pUnixtime = 0;

	HxSTD_memset(szDataTime, 0x00, sizeof(HUINT8)*(TVTV_TIME_MAX_LEN+1));
	HxSTD_memset(&stDateTime, 0x00, sizeof(HxDATETIME_t));

	for(lIndex = 0; lIndex < TVTV_TIME_MAX_LEN; lIndex ++)
	{
		if(lIndex == TVTV_TIME_SEP_YM)
		{
			if(pucDateTime[TVTV_TIME_SEP_YM] != '-' )
			{
				HxLOG_Print("[xsvc_epglib_ConvertStringTimeToUnixTime_Tvtv] TVTV_TIME_SEP_YM invalid!!\n");
				goto EXIT_TVTVLIB_FUNC;
			}
			stDateTime.stDate.usYear = (HUINT16)atoi(szDataTime);
			HxSTD_memset(szDataTime, 0x00, sizeof(HUINT8)*(TVTV_TIME_MAX_LEN+1));
			sCount = 0;
			continue;
		}

		if(lIndex == TVTV_TIME_SEP_MD)
		{
			if(pucDateTime[TVTV_TIME_SEP_MD] != '-' )
			{
				HxLOG_Print("[xsvc_epglib_ConvertStringTimeToUnixTime_Tvtv] TVTV_TIME_SEP_MD invalid!!\n");
				goto EXIT_TVTVLIB_FUNC;
			}
			stDateTime.stDate.ucMonth = (HUINT8)atoi(szDataTime);
			HxSTD_memset(szDataTime, 0x00, sizeof(HUINT8)*(TVTV_TIME_MAX_LEN+1));
			sCount = 0;
			continue;
		}

		if(lIndex == TVTV_TIME_SEP_DH)
		{
			if(pucDateTime[TVTV_TIME_SEP_DH] == 'T' || pucDateTime[TVTV_TIME_SEP_DH] == 't')
			{
				;
			}
			else
			{
				HxLOG_Print("[xsvc_epglib_ConvertStringTimeToUnixTime_Tvtv] TVTV_TIME_SEP_DH invalid!!\n");
				goto EXIT_TVTVLIB_FUNC;
			}
			stDateTime.stDate.ucDay = (HUINT8)atoi(szDataTime);
			HxSTD_memset(szDataTime, 0x00, sizeof(HUINT8)*(TVTV_TIME_MAX_LEN+1));
			sCount = 0;
			continue;
		}

		if(lIndex == TVTV_TIME_SEP_HM)
		{
			if(pucDateTime[TVTV_TIME_SEP_HM] != ':')
			{
				HxLOG_Print("[xsvc_epglib_ConvertStringTimeToUnixTime_Tvtv] TVTV_TIME_SEP_HM invalid!!\n");
				goto EXIT_TVTVLIB_FUNC;
			}
			stDateTime.stTime.ucHour = (HUINT8)atoi(szDataTime);
			HxSTD_memset(szDataTime, 0x00, sizeof(HUINT8)*(TVTV_TIME_MAX_LEN+1));
			sCount = 0;
			continue;
		}

		szDataTime[sCount++] = pucDateTime[lIndex];

	}

	stDateTime.stTime.ucMinute = (HUINT8)atoi(szDataTime);

	hError = HLIB_DATETIME_ConvertDateTimeToUnixTimeWithOutOffset(&stDateTime, pUnixtime);
	if(hError != ERR_TVTV_OK)
	{
		HxLOG_Print("[xsvc_epglib_ConvertStringTimeToUnixTime_Tvtv] ConvertDateTimeToUnixTime fail!!\n");
		goto EXIT_TVTVLIB_FUNC;
	}

	hError = ERR_TVTV_OK;

EXIT_TVTVLIB_FUNC :

	return hError;

}


STATIC HERROR		xsvc_epglib_TrimString_Tvtv(HUINT8 *pucSource, HUINT32 ulStartIndex, HUINT32 ulNumOfCh, HUINT8 *pucTrimString)
{
	HERROR		hError = ERR_FAIL;
	HUINT32		ulSourceLen = 0;
	HUINT32		ulIndex = 0;
	HUINT32		ulTrimIndex = 0;

	if(pucSource == NULL || pucTrimString == NULL)
	{
		HxLOG_Print("[xsvc_epglib_TrimString_Tvtv] param fail!!\n");
		goto EXIT_TVTVLIB_FUNC;
	}

	ulSourceLen = MWC_UTIL_DvbStrlen(pucSource);
	if(ulStartIndex + ulNumOfCh > ulSourceLen)
	{
		HxLOG_Print("[xsvc_epglib_TrimString_Tvtv] param fail!!\n");
		goto EXIT_TVTVLIB_FUNC;
	}

	for(ulIndex = ulStartIndex; ulIndex < ulStartIndex + ulNumOfCh; ulIndex++)
	{
		pucTrimString[ulTrimIndex] = pucSource[ulIndex];
		ulTrimIndex++;
	}
	pucTrimString[ulTrimIndex] = '\0';

	hError = ERR_OK;

EXIT_TVTVLIB_FUNC :

	return hError;
}



STATIC HUINT32		xsvc_epglib_Hextoi_Tvtv (HUINT8 *pucPtr)
{
	HUINT32	ulVal = 0 ;


	if (pucPtr == NULL)
	{
		return 0 ;
	}

	while (1)
	{
		if (*pucPtr=='\0')
		{
			break ;
		}
		else if (*pucPtr=='0' && ( pucPtr[1]=='x' || pucPtr[1]=='X' ))
		{
			pucPtr++ ;
		}
		else if (*pucPtr>='0' && *pucPtr<='9')
		{
			ulVal *= 16 ;
			ulVal += *pucPtr - '0' ;
		}
		else if (*pucPtr>='a' && *pucPtr<='f')
		{
			ulVal *= 16 ;
			ulVal += *pucPtr - 'a' + 10 ;
		}
		else if (*pucPtr>='A' && *pucPtr<='F')
		{
			ulVal *= 16 ;
			ulVal += *pucPtr - 'A' + 10 ;
		}
		pucPtr++ ;
	}

	return ulVal ;
}


STATIC HERROR		xsvc_epglib_IsReplaceString_Tvtv(HUINT8 *pucPtr, HBOOL *pbIsReplace, HUINT8 *pucSize, HUINT8 *puReplaceCode)
{
	HERROR					hError = ERR_TVTV_FAIL;
	HINT16					sIndex = 0;
	HINT32					lRet = 0;

	if(pucPtr == NULL || pbIsReplace == NULL || pucSize == NULL || puReplaceCode == NULL)
	{
		HxLOG_Print("[xsvc_epglib_IsReplaceString_Tvtv] Param NULL!!\n");
		goto EXIT_TVTVLIB_FUNC;
	}
	*pbIsReplace = FALSE;
	*pucSize = 0;
	*puReplaceCode = 0;

	for( ; ; sIndex++)
	{
		if(s_astTvtvReplaceString[sIndex].szOrgStr[0] == 0)
		{
			HxLOG_Print("[xsvc_epglib_IsReplaceString_Tvtv] Loop end!!\n");
			break;
		}

		lRet = (HINT32)memcmp(pucPtr, s_astTvtvReplaceString[sIndex].szOrgStr, s_astTvtvReplaceString[sIndex].ucOrgStrLen);
		if(lRet == 0)
		{
			*pbIsReplace = TRUE;
			*pucSize = s_astTvtvReplaceString[sIndex].ucOrgStrLen;
			*puReplaceCode = s_astTvtvReplaceString[sIndex].ucReplaceCode;
			break;
		}
	}
	hError = ERR_TVTV_OK;

EXIT_TVTVLIB_FUNC :

	return hError;
}


STATIC HERROR		xsvc_epglib_SelectIso639LanguageEncoding_Tvtv(HUINT8 *pucISO639Code, HUINT8 *pucLangCode, HUINT32 *pulLen)
{
	HINT32		lIndex = 0;

	if(pucISO639Code[0] <= 'Z') { pucISO639Code[0] += 32; }	// 'a' - 'A' = 32
	if(pucISO639Code[1] <= 'Z') { pucISO639Code[1] += 32; }
	if(pucISO639Code[2] <= 'Z') { pucISO639Code[2] += 32; }


	while ( s_astSvcEpg_TvtvISO639EncodingTable[lIndex].acCode1[0] != 0)
	{
		if(	!strncmp( (char *)pucISO639Code, (char *)s_astSvcEpg_TvtvISO639EncodingTable[lIndex].acCode1, 3)
			|| !strncmp( (char *)pucISO639Code, (char *)s_astSvcEpg_TvtvISO639EncodingTable[lIndex].acCode2, 3)
			|| !strncmp( (char *)pucISO639Code, (char *)s_astSvcEpg_TvtvISO639EncodingTable[lIndex].acCode3, 3)
		)
		{
			*pulLen = s_astSvcEpg_TvtvISO639EncodingTable[lIndex].unFontSelLen;
			if(*pulLen != 0)
			{
				HxSTD_memcpy(pucLangCode, s_astSvcEpg_TvtvISO639EncodingTable[lIndex].acFontSel, *pulLen );
			}
			return ERR_OK;
		}
		lIndex++;
	}
	*pulLen = 0;

	return ERR_OK;
}




#define	______TVTV_SETSYSTEMINFO_FUNC______

HERROR		xsvc_epglib_SetTvstInfo_Tvtv(HUINT8 *pucTvst, HUINT32 ulSize)
{
	HERROR					hError = ERR_TVTV_FAIL;
	POINTER_LIST_T			*pTvstList = NULL;

	if(pucTvst == NULL)
	{
		HxLOG_Print("[xsvc_epglib_SetTvstInfo_Tvtv] Tvst parsing fail !!\n");
		goto EXIT_TVTVLIB_FUNC;
	}

	hError = xsvc_epglib_ParseTvst_Tvtv(pucTvst, ulSize, &pTvstList);
	if(hError != ERR_TVTV_OK)
	{
		HxLOG_Print("[xsvc_epglib_SetTvstInfo_Tvtv] Tvst parsing fail !!\n");
		goto EXIT_TVTVLIB_FUNC;
	}

	hError = xsvc_epglib_SetTvstList_Tvtv(pTvstList);
	if(hError != ERR_TVTV_OK)
	{
		HxLOG_Print("[xsvc_epglib_SetTvstInfo_Tvtv] Tvst list setting fail !!\n");
		goto EXIT_TVTVLIB_FUNC;
	}


#if	defined(TVTV_CMD_ON)
{
	XsvcEpg_TvtvServiceTable_t		*pServiceTable = NULL;

	while(pTvstList)
	{
		pServiceTable = UTIL_LINKEDLIST_GetContents(pTvstList);

		HxLOG_Print("[TVTV] SvcTbl : Id(0x%04X), Type(%d), Ver(%d), FileCnt(%d), Start(%d), End(%d), Name(%s)\n",	pServiceTable->usTvSid,
																													pServiceTable->ucType,
																													pServiceTable->ulVersion,
																													pServiceTable->usFileCnt,
																													pServiceTable->ulStart,
																													pServiceTable->ulEnd,
																													pServiceTable->pszName );
		pTvstList = UTIL_LINKEDLIST_GetNextPointer(pTvstList);
	}
}
#endif

	hError = ERR_TVTV_OK;

EXIT_TVTVLIB_FUNC :

	return hError;
}



HERROR		xsvc_epglib_SetDgktInfo_Tvtv(HUINT8 *pucDgkt, HUINT32 ulSize, HBOOL bNeedConverting)
{
	HERROR					hError = ERR_TVTV_FAIL;
	POINTER_LIST_T			*pDgktList = NULL;
	HUINT32					ulIndex = 0;
	HUINT8					aucPrimaryKey[TVTV_PRIMARY_KEYLEN + 1] = {0, };


	if(pucDgkt == NULL)
	{
		HxLOG_Print("[xsvc_epglib_SetDgktInfo_Tvtv] param fail !!\n");
		goto EXIT_TVTVLIB_FUNC;
	}

	if(bNeedConverting == TRUE)
	{
		///////////////////////////////////////////////////////////////////
		// TVTV PLAYOUT System Specification v 2.1.3
		// 5.2 Device group key table
		///////////////////////////////////////////////////////////////////
		HxSTD_memcpy(aucPrimaryKey, TVTV_PRIMARY_KEY, sizeof(HUINT8)*(TVTV_PRIMARY_KEYLEN + 1));
		hError = PAL_CRYPT_Des3init(aucPrimaryKey);
		if(hError != ERR_OK)
		{
			HxLOG_Error("[xsvc_epglib_SetDgktInfo_Tvtv] PAL_CRYPT_Des3init fail !!\n");
			goto EXIT_TVTVLIB_FUNC;
		}

		for(ulIndex = 0; ulIndex < ulSize/TVTV_DES_SIZE_BLOCK; ulIndex++)
		{
			hError = PAL_CRYPT_Des3DecryptProcess(&pucDgkt[ulIndex*TVTV_DES_SIZE_BLOCK], &pucDgkt[ulIndex*TVTV_DES_SIZE_BLOCK]);
			if(hError != ERR_OK)
			{
				HxLOG_Error("[xsvc_epglib_SetDgktInfo_Tvtv] PAL_CRYPT_Des3DecryptProcess fail !!\n");
				goto EXIT_TVTVLIB_FUNC;
			}
		}

	}

	hError = xsvc_epglib_ParseDgkt_Tvtv(pucDgkt, ulSize, &pDgktList);
	if(hError != ERR_TVTV_OK)
	{
		HxLOG_Error("[xsvc_epglib_SetDgktInfo_Tvtv] Parsing fail !!\n");
		goto EXIT_TVTVLIB_FUNC;
	}

	hError = xsvc_epglib_SetDgktList_Tvtv(pDgktList);
	if(hError != ERR_TVTV_OK)
	{
		HxLOG_Error("[xsvc_epglib_SetDgktInfo_Tvtv] Tvst list setting fail !!\n");
		goto EXIT_TVTVLIB_FUNC;
	}

#if	defined(TVTV_CMD_ON)
{
	XsvcEpg_TvtvDgkt_t		*pDgkt = NULL;

	while(pDgktList)
	{
		pDgkt = UTIL_LINKEDLIST_GetContents(pDgktList);


		HxLOG_Print("[TVTV] DGKT : usTvSid(0x%04X), ucEncType(%d), usEncLen(%d), ucCompType(%d)\n",	pDgkt->usTvSid,
																											pDgkt->ucEncType,
																											pDgkt->usEncLen,
																											pDgkt->ucCompType );
		pDgktList = UTIL_LINKEDLIST_GetNextPointer(pDgktList);
	}
}
#endif

	hError = ERR_TVTV_OK;

EXIT_TVTVLIB_FUNC :

	return hError;

}


HERROR		xsvc_epglib_SetTvtvSystemInfo_Tvtv(void)
{
	HERROR					hError = ERR_TVTV_FAIL;
	XsvcEpg_TvtvServiceTable_t		stTvst;
	XsvcEpg_TvtvDgkt_t				stDgkt;
	XsvcEpg_TvtvSystemInfo_t 		stSystemInfo;

	HxSTD_memset(&stTvst, 0x00, sizeof(XsvcEpg_TvtvServiceTable_t));
	HxSTD_memset(&stDgkt, 0x00, sizeof(XsvcEpg_TvtvDgkt_t));
	HxSTD_memset(&stSystemInfo, 0x00, sizeof(XsvcEpg_TvtvSystemInfo_t));

	hError = xsvc_epglib_GetTvstInfo_Tvtv(&stTvst);
	if(hError != ERR_TVTV_OK)
	{
		HxLOG_Print("[xsvc_epglib_CreateTvtvSystemInfo_Tvtv] GetTvstInfo fail!!\n");
		goto EXIT_TVTVLIB_FUNC;
	}

	hError = xsvc_epglib_GetDgktInfo_Tvtv(stTvst.usTvSid, &stDgkt);
	if(hError != ERR_TVTV_OK)
	{
		HxLOG_Print("[xsvc_epglib_CreateTvtvSystemInfo_Tvtv] GetTvstInfo fail!!\n");
		goto EXIT_TVTVLIB_FUNC;
	}

	stSystemInfo.usTvSid = stTvst.usTvSid;
	stSystemInfo.ucType = stTvst.ucType;
	stSystemInfo.ulVersion = stTvst.ulVersion;
	stSystemInfo.usFileCnt = stTvst.usFileCnt;
	stSystemInfo.ucEncType = stDgkt.ucEncType;
	stSystemInfo.usEncLen = stDgkt.usEncLen;
	stSystemInfo.ucCompType = stDgkt.ucCompType;
	VK_memcpy32(stSystemInfo.aucSsk, stDgkt.aucSsk, sizeof(HUINT8)*TVTV_DGKT_SIZE_SSK);
	stSystemInfo.pszName = OxEPG_Malloc(sizeof(HUINT8)*(stTvst.ucNameLen+1));
	if(stSystemInfo.pszName == NULL)
	{
		HxLOG_Print("[xsvc_epglib_CreateTvtvSystemInfo_Tvtv] name alloc fail!!\n");
		goto EXIT_TVTVLIB_FUNC;
	}
	HxSTD_memcpy(stSystemInfo.pszName, stTvst.pszName, sizeof(HUINT8)*(stTvst.ucNameLen+1));


	hError = xsvc_epglib_SetSystemInfo_Tvtv(&stSystemInfo);
	if(hError != ERR_TVTV_OK)
	{
		HxLOG_Print("[xsvc_epglib_CreateTvtvSystemInfo_Tvtv] SetSystemInfo fail!!\n");
		goto EXIT_TVTVLIB_FUNC;
	}

	hError = xsvc_epglib_DestroyTvstInfo_Tvtv();
	if(hError != ERR_TVTV_OK)
	{
		HxLOG_Print("[xsvc_epglib_CreateTvtvSystemInfo_Tvtv] DestoryTvstInfo fail!!\n");
	}

	hError = xsvc_epglib_DestroyDgktInfo_Tvtv();
	if(hError != ERR_TVTV_OK)
	{
		HxLOG_Print("[xsvc_epglib_CreateTvtvSystemInfo_Tvtv] DestoryDgktInfo fail!!\n");
	}

	hError = ERR_TVTV_OK;

EXIT_TVTVLIB_FUNC :

	return hError;

}



HERROR		xsvc_epglib_Postprocess_Tvtv(PalDsmcc_File_t *pDsmccFile, xsvcEpg_TvtvFileType_e eFileType, HBOOL bNeedConverting, xsvcEpg_PostprocFile_t **ppPostprocessedFile)
{
	HERROR					hError = ERR_TVTV_FAIL;
	HUINT32					ulIndex = 0;
	HUINT32					ulDecompLen = 0;
	HUINT8					*pDecompBuffer = NULL;
	HUINT8					*pucDecompFile = NULL;
	xsvcEpg_PostprocFile_t			*pPostFile = NULL;
	HUINT32					*pBitmap = NULL;
	HINT32					lWidth = 0;
	HINT32					lHeight = 0;

	ENTER_CRITICAL_TVTVBUF;

	if(pDsmccFile == NULL || ppPostprocessedFile == NULL)
	{
		HxLOG_Print("[local_tvtvlib_PostprocessInfo] param fail!!\n");
		goto EXIT_TVTVLIB_FUNC;
	}
	*ppPostprocessedFile = NULL;

	if(bNeedConverting == TRUE)
	{
		///////////////////////////////////////////////////////
		// decryption
		///////////////////////////////////////////////////////
		if(s_stTvtvSystemInfo.ucEncType != 0)
		{
			// s_stTvtvSystemInfo.aucSsk[TVTV_DES_SIZE_BLOCK] : because of LBS
			hError = PAL_CRYPT_Desinit(&s_stTvtvSystemInfo.aucSsk[TVTV_DES_SIZE_BLOCK]);
			if(hError != ERR_OK)
			{
				HxLOG_Print("[local_tvtvlib_PostprocessInfo] PAL_CRYPT_Desinit fail!!\n");
				goto EXIT_TVTVLIB_FUNC;
			}

			for(ulIndex = 0; ulIndex < s_stTvtvSystemInfo.usEncLen; ulIndex++)
			{
				hError = PAL_CRYPT_DesDecryptProcess(	&pDsmccFile->pucFilePointer[ulIndex*TVTV_DES_SIZE_BLOCK],
														&pDsmccFile->pucFilePointer[ulIndex*TVTV_DES_SIZE_BLOCK]);
				if(hError != ERR_OK)
				{
					HxLOG_Print("[local_tvtvlib_PostprocessInfo] PAL_CRYPT_DesDecryptProcess fail!!\n");
					break;
				}
			}
			if(hError != ERR_OK)
			{
				goto EXIT_TVTVLIB_FUNC;
			}
		}

		///////////////////////////////////////////////////////
		// decompression
		///////////////////////////////////////////////////////
		if(	s_stTvtvSystemInfo.ucCompType != 0
			&& (eFileType == TVTV_FileType_LTOT || eFileType == TVTV_FileType_TSMT || eFileType == TVTV_FileType_EPG_EVENT))
		{
			ulDecompLen = TVTV_MAX_LEN_DECOMPDATA;
			hError = xsvc_epglib_GetDecompBuffer_Tvtv(&pDecompBuffer);
			if(hError != ERR_TVTV_OK || pDecompBuffer == NULL)
			{
				HxLOG_Print("[local_tvtvlib_PostprocessInfo] GetDecompBuffer fail!!\n");
				goto EXIT_TVTVLIB_FUNC;
			}

			hError = HLIB_ZLIB_Uncompress(pDecompBuffer, &ulDecompLen, pDsmccFile->pucFilePointer, pDsmccFile->ulModuleSize);

			if(hError != ERR_TVTV_OK)
			{
				HxLOG_Print("[local_tvtvlib_PostprocessInfo] Uncompress fail!!\n");
				goto EXIT_TVTVLIB_FUNC;
			}

			pucDecompFile = OxEPG_Malloc(sizeof(HUINT8)*(ulDecompLen+1));
			if(pucDecompFile == NULL)
			{
				HxLOG_Error("[local_tvtvlib_PostprocessInfo] OxEPG_Malloc fail!!\n");
				goto EXIT_TVTVLIB_FUNC;
			}
			VK_memcpy32(pucDecompFile, pDecompBuffer, sizeof(HUINT8)*ulDecompLen);

			///////////////////////////////////////////////////////////////////
			//
			// for LIBXML parser
			//
			///////////////////////////////////////////////////////////////////
			pucDecompFile[ulDecompLen] = '\0';

			pPostFile = OxEPG_Malloc(sizeof(xsvcEpg_PostprocFile_t));
			if(pPostFile == NULL)
			{
				HxLOG_Error("[local_tvtvlib_PostprocessInfo] OxEPG_Malloc fail!!\n");
				goto EXIT_TVTVLIB_FUNC;
			}
			HxSTD_memset(pPostFile, 0x00, sizeof(xsvcEpg_PostprocFile_t));

			pPostFile->eFileType = eFileType;
			pPostFile->pucFileName = pDsmccFile->pucModuleName;
			pDsmccFile->pucModuleName = NULL;
			pPostFile->pucFilePointer = pucDecompFile;
			pucDecompFile = NULL;
			pPostFile->ulFileSize = ulDecompLen;

			pPostFile->pNext = NULL;
			pDsmccFile->pucModuleName = NULL;
		}
		///////////////////////////////////////////////////////
		// JPEG => Bitmap
		///////////////////////////////////////////////////////
		else if(eFileType == TVTV_FileType_LOGO || eFileType == TVTV_FileType_EPG_IMAGE)
		{

#if			defined(CONV_JPEG_TO_BMP)
			hError = PAL_JPEG_DecodeBuffer(	pDsmccFile->pucFilePointer,
											pDsmccFile->ulModuleSize,
											&pBitmap,
											&lWidth,
											&lHeight);
			if(hError != ERR_OK)
			{
				HxLOG_Print("[local_tvtvlib_PostprocessInfo] PAL_JPEG_DecodeBuffer fail!!\n");
				goto EXIT_TVTVLIB_FUNC;
			}
#endif
			pPostFile = OxEPG_Malloc(sizeof(xsvcEpg_PostprocFile_t));
			if(pPostFile == NULL)
			{
				HxLOG_Error("[local_tvtvlib_PostprocessInfo] OxEPG_Malloc fail!!\n");
				goto EXIT_TVTVLIB_FUNC;
			}
			HxSTD_memset(pPostFile, 0x00, sizeof(xsvcEpg_PostprocFile_t));

			pPostFile->eFileType = eFileType;
			pPostFile->pucFileName = pDsmccFile->pucModuleName;
			pDsmccFile->pucModuleName = NULL;
#if			defined(CONV_JPEG_TO_BMP)
			pPostFile->pucFilePointer = (HUINT8 *)pBitmap;
			pBitmap = NULL;
			pPostFile->ulFileSize = lWidth*lHeight*sizeof(HUINT32) + (2*sizeof(HINT32));
#else
			pPostFile->pucFilePointer = pDsmccFile->pucFilePointer;
			pPostFile->ulFileSize = pDsmccFile->ulModuleSize;
			pDsmccFile->pucFilePointer = NULL;
#endif
			pPostFile->lWidth = lWidth;
			pPostFile->lHeight = lHeight;

			pPostFile->pNext = NULL;
			pDsmccFile->pucModuleName = NULL;
		}
	}

	*ppPostprocessedFile = pPostFile;

	hError = ERR_TVTV_OK;

EXIT_TVTVLIB_FUNC :
	if(pBitmap)
	{
		OxMW_Free(pBitmap);
		pBitmap = NULL;
	}

	if(pucDecompFile)
	{
		OxEPG_Free(pucDecompFile);
		pucDecompFile = NULL;
	}

	LEAVE_CRITICAL_TVTVBUF;

	return hError;
}



#define	______TVTV_PARSING_FUNC______

STATIC HERROR		xsvc_epglib_ParseTvst_Tvtv(HUINT8 *pRawBuffer, HUINT32 ulBufferSize, POINTER_LIST_T **ppTvstList)
{
	HERROR					hError = ERR_TVTV_FAIL;
	XsvcEpg_TvtvServiceTable_t		*pServiceTable = NULL;
	HUINT8					*pucTempBuffer = NULL;
	POINTER_LIST_T			*pTvstList = NULL;
	HUINT32					ucTempLen = 0;
	HINT32					ulTotalRead = 0;

	HUINT8					ucTvstLen = 0;
	HUINT16					usTvsid = 0;
	HUINT8					ucType = 0;
	HUINT32					ulVer = 0;
	HUINT16					usFileCnt = 0;
	HUINT32					ulStart = 0;
	HUINT32					ulEnd = 0;
	HUINT8					ucNameLen = 0;
	HUINT8					*pucName = NULL;

	if( pRawBuffer == NULL || ppTvstList == NULL ||ulBufferSize == 0)
	{
		HxLOG_Print("[local_tvtvlib_ParseTvstInfo] Param NULL!!\n");
		goto EXIT_TVTVLIB_FUNC;
	}

	*ppTvstList = NULL;
	ulTotalRead = (HINT32)ulBufferSize;
	pucTempBuffer = pRawBuffer;

	while(ulTotalRead > 0)
	{

		ucTvstLen = *pucTempBuffer;
		pucTempBuffer += TVTV_TVST_SIZE_LEN;
		ulTotalRead -= TVTV_TVST_SIZE_LEN;
		if(ulTotalRead <= 0)
		{
			HxLOG_Print("[local_tvtvlib_ParseTvstInfo] raw data read fail 1 !!\n");
			goto EXIT_TVTVLIB_FUNC;
		}
		ucTempLen = 0;

		usTvsid = get16bit ((HUINT8 *)pucTempBuffer);
		pucTempBuffer += TVTV_TVST_SIZE_TVSID;
		ucTempLen += TVTV_TVST_SIZE_TVSID;
		ulTotalRead -= TVTV_TVST_SIZE_TVSID;
		if(ulTotalRead <= 0)
		{
			HxLOG_Print("[local_tvtvlib_ParseTvstInfo] raw data read fail 2 !!\n");
			goto EXIT_TVTVLIB_FUNC;
		}

		ucType = *pucTempBuffer;
		pucTempBuffer += TVTV_TVST_SIZE_TYPE;
		ucTempLen += TVTV_TVST_SIZE_TYPE;
		ulTotalRead -= TVTV_TVST_SIZE_TYPE;
		if(ulTotalRead < 0)
		{
			HxLOG_Print("[local_tvtvlib_ParseTvstInfo] raw data read fail 3 !!\n");
			goto EXIT_TVTVLIB_FUNC;
		}


		ulVer =  get32bit ((HUINT8 *)pucTempBuffer);
		pucTempBuffer += TVTV_TVST_SIZE_VER;
		ucTempLen += TVTV_TVST_SIZE_VER;
		ulTotalRead -= TVTV_TVST_SIZE_VER;
		if(ulTotalRead < 0)
		{
			HxLOG_Print("[local_tvtvlib_ParseTvstInfo] raw data read fail 4 !!\n");
			goto EXIT_TVTVLIB_FUNC;
		}


		usFileCnt = get16bit ((HUINT8 *)pucTempBuffer);
		pucTempBuffer += TVTV_TVST_SIZE_FILECNT;
		ucTempLen += TVTV_TVST_SIZE_FILECNT;
		ulTotalRead -= TVTV_TVST_SIZE_FILECNT;
		if(ulTotalRead < 0)
		{
			HxLOG_Print("[local_tvtvlib_ParseTvstInfo] raw data read fail 5 !!\n");
			goto EXIT_TVTVLIB_FUNC;
		}


		ulStart =  get32bit ((HUINT8 *)pucTempBuffer);
		pucTempBuffer += TVTV_TVST_SIZE_START;
		ucTempLen += TVTV_TVST_SIZE_START;
		ulTotalRead -= TVTV_TVST_SIZE_START;
		if(ulTotalRead < 0)
		{
			HxLOG_Print("[local_tvtvlib_ParseTvstInfo] raw data read fail 6 !!\n");
			goto EXIT_TVTVLIB_FUNC;
		}


		ulEnd =  get32bit ((HUINT8 *)pucTempBuffer);
		pucTempBuffer += TVTV_TVST_SIZE_END;
		ucTempLen += TVTV_TVST_SIZE_END;
		ulTotalRead -= TVTV_TVST_SIZE_END;
		if(ulTotalRead < 0)
		{
			HxLOG_Print("[local_tvtvlib_ParseTvstInfo] raw data read fail 7 !!\n");
			goto EXIT_TVTVLIB_FUNC;
		}


		ucNameLen =  *pucTempBuffer;
		pucTempBuffer += TVTV_TVST_SIZE_NAMELEN;
		ucTempLen += TVTV_TVST_SIZE_NAMELEN;
		ulTotalRead -= TVTV_TVST_SIZE_NAMELEN;
		if(ulTotalRead <= 0)
		{
			HxLOG_Print("[local_tvtvlib_ParseTvstInfo] raw data read fail 7 !!\n");
			goto EXIT_TVTVLIB_FUNC;
		}

		pucName = (HUINT8*)OxEPG_Malloc((ucNameLen+1)*sizeof(HUINT8));
		if(pucName == NULL)
		{
			HxLOG_Print("[local_tvtvlib_ParseTvstInfo] name alloc fail !!\n");
			goto EXIT_TVTVLIB_FUNC;
		}
		HxSTD_memset(pucName, 0x00, ucNameLen+1);

		HxSTD_memcpy(pucName, pucTempBuffer, ucNameLen);
		pucTempBuffer += ucNameLen;
		ucTempLen += ucNameLen;
		ulTotalRead -= ucNameLen;
		if(ulTotalRead < 0)
		{
			HxLOG_Print("[local_tvtvlib_ParseTvstInfo] raw data read fail 8 ucNameLen(%d)!!\n", ucNameLen);
			goto EXIT_TVTVLIB_FUNC;
		}

#if		0
		ucType = *pucTempBuffer;
		pucTempBuffer += TVTV_TVST_SIZE_RESERVED;
		ucTempLen += TVTV_TVST_SIZE_RESERVED;
		ulTotalRead -= TVTV_TVST_SIZE_RESERVED;
		if(ulTotalRead < 0)
		{
			HxLOG_Print("[local_tvtvlib_ParseTvstInfo] raw data read fail 9 !!\n");
			goto EXIT_TVTVLIB_FUNC;
		}
#endif
		if(ucTempLen !=  ucTvstLen)
		{
			HxLOG_Print("[local_tvtvlib_ParseTvstInfo] raw data read fail 10 !!\n");
			goto EXIT_TVTVLIB_FUNC;
		}


		pServiceTable = (XsvcEpg_TvtvServiceTable_t*)OxEPG_Malloc(sizeof(XsvcEpg_TvtvServiceTable_t));
		if(pServiceTable == NULL)
		{
			HxLOG_Print("[local_tvtvlib_ParseTvstInfo] memory alloc fail NULL!!\n");
			goto EXIT_TVTVLIB_FUNC;
		}
		HxSTD_memset(pServiceTable, 0x00, sizeof(XsvcEpg_TvtvServiceTable_t));

		pServiceTable->usTvSid = usTvsid;
		pServiceTable->ucType = ucType;
		pServiceTable->ulVersion = ulVer;
		pServiceTable->usFileCnt = usFileCnt;
		pServiceTable->ulStart = ulStart;
		pServiceTable->ulEnd = ulEnd;
		pServiceTable->ucNameLen = ucNameLen;
		pServiceTable->pszName = pucName;

#if		0
		HxLOG_Print("[TVTV] SvcTbl : Id(0x%04X), Type(%d), Ver(%d), FileCnt(%d), Start(%d), End(%d), Name(%s)\n",	pServiceTable->usTvSid,
																													pServiceTable->ucType,
																													pServiceTable->ulVersion,
																													pServiceTable->usFileCnt,
																													pServiceTable->ulStart,
																													pServiceTable->ulEnd,
																													pServiceTable->pszName) );
#endif

		pTvstList = UTIL_LINKEDLIST_AppendListItemToPointerList(pTvstList, (void*) pServiceTable);
		if(pTvstList == NULL)
		{
			HxLOG_Print("[local_tvtvlib_ParseTvstInfo] Append list fail !!\n");
			goto EXIT_TVTVLIB_FUNC;
		}
		pServiceTable = NULL;

	}

	*ppTvstList = pTvstList;
	hError = ERR_TVTV_OK;

	return hError;

EXIT_TVTVLIB_FUNC :

	if(pucName)
	{
		HLIB_STD_MemFree(pucName);
	}

	return hError;

}


STATIC HERROR		xsvc_epglib_ParseDgkt_Tvtv(HUINT8 *pRawBuffer, HUINT32 ulBufferSize, POINTER_LIST_T **ppDgktList)
{
	HERROR					hError = ERR_TVTV_FAIL;
	HINT32					lTotalRead = 0;
	HUINT8					*pucTempBuffer = NULL;
	XsvcEpg_TvtvDgkt_t				*pTempDgkt;
	POINTER_LIST_T			*pDgktList = NULL;

	HUINT16					usTvsid = 0;
	HUINT8					ucEncType = 0;
	HUINT16					usEncLen = 0;
	HUINT8					ucCompType = 0;
	HUINT16					usReserved = 0;
	HUINT8					aucSsk[TVTV_DGKT_SIZE_SSK];

	if( pRawBuffer == NULL || ppDgktList == NULL ||ulBufferSize == 0)
	{
		HxLOG_Print("[xsvc_epglib_ParseDgkt_Tvtv] Param NULL!!\n");
		goto EXIT_TVTVLIB_FUNC;
	}

	*ppDgktList = NULL;
	lTotalRead = (HINT32)ulBufferSize;
	pucTempBuffer = pRawBuffer;

	while(lTotalRead > 0)
	{
		usTvsid = get16bit ((HUINT8 *)pucTempBuffer);
		pucTempBuffer += TVTV_DGKT_SIZE_TVSID;
		lTotalRead -= TVTV_DGKT_SIZE_TVSID;
		if(lTotalRead <= 0)
		{
			HxLOG_Print("[xsvc_epglib_ParseDgkt_Tvtv] raw data read fail 1 !!\n");
			goto EXIT_TVTVLIB_FUNC;
		}

		ucEncType = *pucTempBuffer;
		pucTempBuffer += TVTV_DGKT_SIZE_ENCTYPE;
		lTotalRead -= TVTV_DGKT_SIZE_ENCTYPE;
		if(lTotalRead <= 0)
		{
			HxLOG_Print("[xsvc_epglib_ParseDgkt_Tvtv] raw data read fail 2 !!\n");
			goto EXIT_TVTVLIB_FUNC;
		}

		usEncLen = get16bit ((HUINT8 *)pucTempBuffer);
		pucTempBuffer += TVTV_DGKT_SIZE_ENCLEN;
		lTotalRead -= TVTV_DGKT_SIZE_ENCLEN;
		if(lTotalRead <= 0)
		{
			HxLOG_Print("[xsvc_epglib_ParseDgkt_Tvtv] raw data read fail 3 !!\n");
			goto EXIT_TVTVLIB_FUNC;
		}


		ucCompType= *pucTempBuffer;
		pucTempBuffer += TVTV_DGKT_SIZE_COMPTYPE;
		lTotalRead -= TVTV_DGKT_SIZE_COMPTYPE;
		if(lTotalRead <= 0)
		{
			HxLOG_Print("[xsvc_epglib_ParseDgkt_Tvtv] raw data read fail 4 !!\n");
			goto EXIT_TVTVLIB_FUNC;
		}


		usReserved = get16bit ((HUINT8 *)pucTempBuffer);
		pucTempBuffer += TVTV_DGKT_SIZE_RESERVED;
		lTotalRead -= TVTV_DGKT_SIZE_RESERVED;
		if(lTotalRead <= 0)
		{
			HxLOG_Print("[xsvc_epglib_ParseDgkt_Tvtv] raw data read fail 5 !!\n");
			goto EXIT_TVTVLIB_FUNC;
		}

		HxSTD_memset(aucSsk, 0x00, (TVTV_DGKT_SIZE_SSK*sizeof(HUINT8)));
		if (lTotalRead < TVTV_DGKT_SIZE_SSK)
		{
			HxSTD_memcpy(&aucSsk[TVTV_DGKT_SIZE_SSK - lTotalRead], pucTempBuffer, lTotalRead*sizeof(HUINT8));
			pucTempBuffer += lTotalRead;
			lTotalRead = 0;
		}
		else
		{
			HxSTD_memcpy(aucSsk, pucTempBuffer, TVTV_DGKT_SIZE_SSK*sizeof(HUINT8));
			pucTempBuffer += TVTV_DGKT_SIZE_SSK;
			lTotalRead -= TVTV_DGKT_SIZE_SSK;
		}
		if(lTotalRead < 0)
		{
			HxLOG_Print("[xsvc_epglib_ParseDgkt_Tvtv] raw data read fail 5 !!\n");
			goto EXIT_TVTVLIB_FUNC;
		}


		pTempDgkt = (XsvcEpg_TvtvDgkt_t*)OxEPG_Malloc(sizeof(XsvcEpg_TvtvDgkt_t));
		if(pTempDgkt == NULL)
		{
			HxLOG_Print("[xsvc_epglib_ParseDgkt_Tvtv] memory alloc fail NULL!!\n");
			goto EXIT_TVTVLIB_FUNC;
		}
		HxSTD_memset(pTempDgkt, 0x00, sizeof(XsvcEpg_TvtvDgkt_t));

		pTempDgkt->usTvSid = usTvsid;
		pTempDgkt->ucEncType = ucEncType;
		pTempDgkt->usEncLen = usEncLen;
		pTempDgkt->ucCompType = ucCompType;
		pTempDgkt->usReserved = usReserved;
		HxSTD_memcpy(pTempDgkt->aucSsk, aucSsk, sizeof(HUINT8)*TVTV_DGKT_SIZE_SSK) ;

#if		0
		HxLOG_Print("[TVTV] DGKT : usTvSid(0x%04X), ucEncType(%d), usEncLen(%d), ucCompType(%d)\n",	pTempDgkt->usTvSid,
																											pTempDgkt->ucEncType,
																											pTempDgkt->usEncLen,
																											pTempDgkt->ucCompType) );
#endif

		pDgktList = UTIL_LINKEDLIST_AppendListItemToPointerList(pDgktList, (void*) pTempDgkt);
		if(pDgktList == NULL)
		{
			HxLOG_Print("[xsvc_epglib_ParseDgkt_Tvtv] Append list fail !!\n");
			goto EXIT_TVTVLIB_FUNC;
		}
		pTempDgkt = NULL;

	}

	*ppDgktList = pDgktList;
	hError = ERR_TVTV_OK;

EXIT_TVTVLIB_FUNC :

	return hError;
}



HERROR		xsvc_epglib_ParseTsmt_Tvtv(HUINT8 *pRawBuffer, HUINT32 ulBufferSize, POINTER_LIST_T **ppTsmtList)
{
	HERROR					hError = ERR_TVTV_FAIL;
	HXMLDOC				hXmlDocHandle = NULL;
	HXMLELEMENT				hRootElement = NULL;
	HXMLELEMENT				hTempElement1 = NULL;
	HXMLELEMENT				hTempElement2 = NULL;
	HUINT32					ulIndex = 0;
	HUINT8					*pszTempContents= NULL;
	HUINT8					*pszAttrContents= NULL;
	XsvcEpg_TvtvElementList_t 		*pstElementList = NULL;
	XsvcEpg_TvtvElement_t			*pstElement = NULL;
	XsvcEpg_TvtvTsmt_t				*pstTempTsmt = NULL;
	XsvcEpg_TvtvImage_t			*pstImage = NULL;
	POINTER_LIST_T			*pSameChildList = NULL;
	POINTER_LIST_T			*pChildTemp = NULL;
	POINTER_LIST_T			*pTsList = NULL;
	POINTER_LIST_T			*pTsTemp = NULL;
	POINTER_LIST_T			*pTsmtList = NULL;
	HUINT32					ulValue = 0;

	if( pRawBuffer == NULL || ppTsmtList == NULL ||ulBufferSize == 0)
	{
		HxLOG_Print("[xsvc_epglib_ParseTsmt_Tvtv] Param NULL!!\n");
		goto EXIT_TVTVLIB_FUNC;
	}


	hError = PAL_XML_InitParser();
	if(hError != ERR_XML_OK)
	{
		HxLOG_Print("[xsvc_epglib_ParseTsmt_Tvtv] XML_InitParser fail !!\n");
		goto EXIT_TVTVLIB_FUNC;
	}

	hError = PAL_XML_ParseBuffer(pRawBuffer, ulBufferSize, &hXmlDocHandle);
	if(hError != ERR_XML_OK)
	{
		HxLOG_Print("[xsvc_epglib_ParseTsmt_Tvtv] ParseBuffer fail !!\n");
		goto EXIT_TVTVLIB_FUNC;
	}

	// Todo validation check
#if 0
	hError = PAL_XML_Validate(XML_VALIDATE_BUFFER, NULL, /*dtd buffer variable */, /* buffer size*/, hXmlDocHandle);
	if(hError != ERR_XML_OK)
	{
		HxLOG_Print("[xsvc_epglib_ParseTsmt_Tvtv] XML_Validate fail !!\n");
		goto EXIT_TVTVLIB_FUNC;
	}
#endif

	hError = xsvc_epgdata_LoadElementList_Tvtv(TVTV_ELEMENT_LIST_TSMT, &pstElementList);
	if(hError != ERR_XML_OK)
	{
		HxLOG_Print("[xsvc_epglib_ParseTsmt_Tvtv] LoadElementList fail !!\n");
		goto EXIT_TVTVLIB_FUNC;
	}
	pstElement = pstElementList->pstElement;
	if(pstElement == NULL)
	{
		HxLOG_Print("[xsvc_epglib_ParseTsmt_Tvtv] pstElement NULL !!\n");
		goto EXIT_TVTVLIB_FUNC;
	}

	hError = PAL_XML_GetRootElement(hXmlDocHandle, &hRootElement);
	if(hError != ERR_XML_OK)
	{
		HxLOG_Print("[xsvc_epglib_ParseTsmt_Tvtv] GetRootElement fail !!\n");
		goto EXIT_TVTVLIB_FUNC;
	}

	hError = PAL_XML_FindChildElementByName(hRootElement, pstElement[TVTV_TSMT_TS].pucTagName, &pTsList);
	if(hError != ERR_XML_OK)
	{
		HxLOG_Print("[xsvc_epglib_ParseTsmt_Tvtv] GetRootElement fail !!\n");
		goto EXIT_TVTVLIB_FUNC;
	}
	pTsTemp = pTsList;

	while(pTsTemp != NULL)
	{
		hTempElement1 = (HXMLELEMENT)UTIL_LINKEDLIST_GetContents(pTsTemp);
		if(hTempElement1 == NULL)
		{
			HxLOG_Print("[xsvc_epglib_ParseTsmt_Tvtv] GetContents fail!!\n");
			goto EXIT_TVTVLIB_TS_LOOP;
		}

		pstTempTsmt = (XsvcEpg_TvtvTsmt_t*)OxEPG_Malloc(sizeof(XsvcEpg_TvtvTsmt_t));
		if(pstTempTsmt == NULL)
		{
			HxLOG_Print("[xsvc_epglib_ParseTsmt_Tvtv] Alloc fail!!\n");
			goto EXIT_TVTVLIB_TS_LOOP;
		}
		HxSTD_memset(pstTempTsmt, 0x00, sizeof(XsvcEpg_TvtvTsmt_t));

		for(ulIndex = TVTV_TSMT_TVTVID; ulIndex < TVTV_TSMT_MAX; ulIndex++)
		{

			hError = PAL_XML_FindChildElementByName(hTempElement1, pstElement[ulIndex].pucTagName, &pSameChildList);
			if(hError != ERR_XML_OK)
			{
				HxLOG_Print("[xsvc_epglib_ParseTsmt_Tvtv] FindChildElementByName fail (%s)!!\n", pstElement[ulIndex].pucTagName);
				continue;
			}
			pChildTemp = pSameChildList;

			while(pChildTemp != NULL)
			{
				hTempElement2 = (HXMLELEMENT)UTIL_LINKEDLIST_GetContents(pChildTemp);
				if(hTempElement2 == NULL)
				{
					HxLOG_Print("[xsvc_epglib_ParseTsmt_Tvtv] GetContents fail (%s)!!\n", pstElement[ulIndex].pucTagName);
					goto EXIT_TVTVLIB_SAMECHILD_LOOP;
				}

				hError = PAL_XML_GetElementContents(hXmlDocHandle, hTempElement2, &pszTempContents);
				if(hError != ERR_XML_OK)
				{
					HxLOG_Print("[xsvc_epglib_ParseTsmt_Tvtv] GetElementContents fail (%s)!!\n", pstElement[ulIndex].pucTagName);
					goto EXIT_TVTVLIB_SAMECHILD_LOOP;
				}

				switch(ulIndex)
				{

					case TVTV_TSMT_TVTVID :
						hError = xsvc_epglib_SetDataByType_Tvtv(pstElement[ulIndex].ulTagDataType, pszTempContents, NULL, 0, 0, NULL, &ulValue);
						if(hError != ERR_TVTV_OK)
						{
							HxLOG_Print("[xsvc_epglib_ParseTsmt_Tvtv] SetDataByType usTvtvId fail!!\n");
						}
						pstTempTsmt->usTvtvId = (HUINT16)ulValue;
						break;
					case TVTV_TSMT_OID :
						hError = xsvc_epglib_SetDataByType_Tvtv(pstElement[ulIndex].ulTagDataType, pszTempContents, NULL, 0, 0, NULL, &ulValue);
						if(hError != ERR_TVTV_OK)
						{
							HxLOG_Print("[xsvc_epglib_ParseTsmt_Tvtv] SetDataByType usOnId fail!!\n");
						}
						pstTempTsmt->usOnId = (HUINT16)ulValue;
						break;
					case TVTV_TSMT_SID :
						hError = xsvc_epglib_SetDataByType_Tvtv(pstElement[ulIndex].ulTagDataType, pszTempContents, NULL, 0, 0, NULL, &ulValue);
						if(hError != ERR_TVTV_OK)
						{
							HxLOG_Print("[xsvc_epglib_ParseTsmt_Tvtv] SetDataByType usSId fail!!\n");
						}
						pstTempTsmt->usSId = (HUINT16)ulValue;
						break;
					case TVTV_TSMT_TSID :
						hError = xsvc_epglib_SetDataByType_Tvtv(pstElement[ulIndex].ulTagDataType, pszTempContents, NULL, 0, 0, NULL, &ulValue);
						if(hError != ERR_TVTV_OK)
						{
							HxLOG_Print("[xsvc_epglib_ParseTsmt_Tvtv] SetDataByType usTsId fail!!\n");
						}
						pstTempTsmt->usTsId = (HUINT16)ulValue;
						break;
					case TVTV_TSMT_F1830 :
						hError = xsvc_epglib_SetDataByType_Tvtv(pstElement[ulIndex].ulTagDataType, pszTempContents, NULL, 0, 0, NULL, &ulValue);
						if(hError != ERR_TVTV_OK)
						{
							HxLOG_Print("[xsvc_epglib_ParseTsmt_Tvtv] SetDataByType usF1830 fail!!\n");
						}
						pstTempTsmt->usF1830 = (HUINT16)ulValue;
						break;
					case TVTV_TSMT_F2830 :
						hError = xsvc_epglib_SetDataByType_Tvtv(pstElement[ulIndex].ulTagDataType, pszTempContents, NULL, 0, 0, NULL, &ulValue);
						if(hError != ERR_TVTV_OK)
						{
							HxLOG_Print("[xsvc_epglib_ParseTsmt_Tvtv] SetDataByType usF2830 fail!!\n");
						}
						pstTempTsmt->usF2830 = (HUINT16)ulValue;
						break;
					case TVTV_TSMT_X26 :
						hError = xsvc_epglib_SetDataByType_Tvtv(pstElement[ulIndex].ulTagDataType, pszTempContents, NULL, 0, 0, NULL, &ulValue);
						if(hError != ERR_TVTV_OK)
						{
							HxLOG_Print("[xsvc_epglib_ParseTsmt_Tvtv] SetDataByType usX26 fail!!\n");
						}
						pstTempTsmt->usX26 = (HUINT16)ulValue;
						break;
					case TVTV_TSMT_VPS :
						hError = xsvc_epglib_SetDataByType_Tvtv(pstElement[ulIndex].ulTagDataType, pszTempContents, NULL, 0, 0, NULL, &ulValue);
						if(hError != ERR_TVTV_OK)
						{
							HxLOG_Print("[xsvc_epglib_ParseTsmt_Tvtv] SetDataByType usVps fail!!\n");
						}
						pstTempTsmt->usVps = (HUINT16)ulValue;
						break;
					case TVTV_TSMT_SNAME :
						hError = xsvc_epglib_SetDataByType_Tvtv(pstElement[ulIndex].ulTagDataType, pszTempContents, NULL, 0, TVTV_MAX_SHORTNAME + 1, pstTempTsmt->szSname, NULL);
						if(hError != ERR_TVTV_OK)
						{
							HxLOG_Print("[xsvc_epglib_ParseTsmt_Tvtv] SetDataByType szSname fail!!\n");
						}
						break;
					case TVTV_TSMT_LNAME :
						hError = xsvc_epglib_SetDataByType_Tvtv(pstElement[ulIndex].ulTagDataType, pszTempContents, NULL, 0, TVTV_MAX_LONGNAME + 1, pstTempTsmt->szLname, NULL);
						if(hError != ERR_TVTV_OK)
						{
							HxLOG_Print("[xsvc_epglib_ParseTsmt_Tvtv] SetDataByType szSname fail!!\n");
						}
						break;
					case TVTV_TSMT_LANG :
						hError = xsvc_epglib_SetDataByType_Tvtv(pstElement[ulIndex].ulTagDataType, pszTempContents, NULL, 0, TVTV_MAX_LANG + 1, pstTempTsmt->szLang, NULL);
						if(hError != ERR_TVTV_OK)
						{
							HxLOG_Print("[xsvc_epglib_ParseTsmt_Tvtv] SetDataByType szLang fail!!\n");
						}
						break;
					case TVTV_TSMT_CAT :
						hError = xsvc_epglib_SetDataByType_Tvtv(pstElement[ulIndex].ulTagDataType, pszTempContents, NULL, 0, 0, NULL, &ulValue);
						if(hError != ERR_TVTV_OK)
						{
							HxLOG_Print("[xsvc_epglib_ParseTsmt_Tvtv] SetDataByType ucCat fail!!\n");
						}
						pstTempTsmt->ucCat = (HUINT8)ulValue;
						break;
					case TVTV_TSMT_LOGO :
						pstImage = (XsvcEpg_TvtvImage_t*)OxEPG_Malloc(sizeof(XsvcEpg_TvtvImage_t));
						if(pstImage == NULL)
						{
							HxLOG_Print("[xsvc_epglib_ParseTsmt_Tvtv] pstImage alloc fail!!\n");
							break;
						}
						HxSTD_memset(pstImage, 0x00, sizeof(XsvcEpg_TvtvImage_t));

						hError = xsvc_epglib_SetDataByType_Tvtv(pstElement[ulIndex].ulTagDataType, pszTempContents, NULL, 0, TVTV_IMG_MAX_FILENAME + 1, pstImage->szFilename, NULL);
						if(hError != ERR_TVTV_OK)
						{
							HxLOG_Print("[xsvc_epglib_ParseTsmt_Tvtv] SetDataByType szFilename fail!!\n");
							OxEPG_Free(pstImage);
							break;
						}

						hError = PAL_XML_GetAttrContents(hTempElement2, pstElement[ulIndex].pucAttrName, &pszAttrContents);
						if(hError != ERR_XML_OK)
						{
							HxLOG_Print("[xsvc_epglib_ParseTsmt_Tvtv] GetAttrContents fail!!\n");
							OxEPG_Free(pstImage);
							pstImage = NULL;
							break;
						}

						hError = xsvc_epglib_SetDataByType_Tvtv(pstElement[ulIndex].ulAttrDataType, pszAttrContents, NULL, 0, TVTV_IMG_MAX_COPYRIGHT + 1, pstImage->szCopyright, NULL);
						if(hError != ERR_TVTV_OK)
						{
							HxLOG_Print("[xsvc_epglib_ParseTsmt_Tvtv] SetDataByType szCopyright fail!!\n");
							OxEPG_Free(pstImage);
							pstImage = NULL;
							PAL_XML_FreeAttrContents(pszAttrContents);
							pszAttrContents = NULL;
							break;
						}

						pstTempTsmt->pstLogoInfo = pstImage;
						PAL_XML_FreeAttrContents(pszAttrContents);
						pszAttrContents = NULL;
						break;
					case TVTV_TSMT_ORDER :
						hError = xsvc_epglib_SetDataByType_Tvtv(pstElement[ulIndex].ulTagDataType, pszTempContents, NULL, 0, 0, NULL, &ulValue);
						if(hError != ERR_TVTV_OK)
						{
							HxLOG_Print("[xsvc_epglib_ParseTsmt_Tvtv] SetDataByType usOrder fail!!\n");
						}
						pstTempTsmt->usOrder = (HUINT16)ulValue;
						break;
					case TVTV_TSMT_CATYPE :
						hError = xsvc_epglib_SetDataByType_Tvtv(pstElement[ulIndex].ulTagDataType, pszTempContents, NULL, 0, 0, NULL, &ulValue);
						if(hError != ERR_TVTV_OK)
						{
							HxLOG_Print("[xsvc_epglib_ParseTsmt_Tvtv] SetDataByType usCaType fail!!\n");
						}
						pstTempTsmt->usCaType = (HUINT16)ulValue;
						break;
					case TVTV_TSMT_CAID :
						HxLOG_Print("[xsvc_epglib_ParseTsmt_Tvtv] SetDataByType TVTV_TSMT_CAID Skip!!\n");
						break;
					default :
						break;
				}

				PAL_XML_FreeAttrContents(pszTempContents);
				pszTempContents = NULL;

EXIT_TVTVLIB_SAMECHILD_LOOP :

				pChildTemp = UTIL_LINKEDLIST_GetNextPointer(pChildTemp);
				if(pChildTemp == NULL || pChildTemp == pSameChildList)
				{
					break;
				}
			}

			pSameChildList = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList(pSameChildList, NULL);
			if(pSameChildList != NULL)
			{
				HxLOG_Print("[xsvc_epglib_ParseTsmt_Tvtv] FreeWholeListItemsFromPointerList Fail!!\n");
				pSameChildList = NULL;
			}

		}

		pTsmtList = UTIL_LINKEDLIST_AppendListItemToPointerList(pTsmtList, (void*)pstTempTsmt);
		if(pTsmtList == NULL)
		{
			HxLOG_Print("[xsvc_epglib_ParseTsmt_Tvtv] AppendListItemToPointerList Fail!!\n");
		}

EXIT_TVTVLIB_TS_LOOP :

		pTsTemp = UTIL_LINKEDLIST_GetNextPointer(pTsTemp);
		if(pTsTemp == NULL || pTsTemp == pTsList)
		{
			HxLOG_Print("[xsvc_epglib_ParseTsmt_Tvtv] Loop end !!\n");
			break;
		}
	}

	pTsList = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList(pTsList, NULL);
	if(pTsList != NULL)
	{
		HxLOG_Print("[xsvc_epglib_ParseTsmt_Tvtv] pTsList Fail!!\n");
		pTsList = NULL;
	}

	hError = PAL_XML_FreeDocument(hXmlDocHandle);
	if(hError != ERR_XML_OK)
	{
		HxLOG_Print("[xsvc_epglib_ParseTsmt_Tvtv] FreeDocument Fail!!\n");
	}

	*ppTsmtList = pTsmtList;

	hError = ERR_TVTV_OK;

EXIT_TVTVLIB_FUNC :

	return hError;

}





HERROR		xsvc_epglib_ParseEpg_Tvtv(HUINT8 *pRawBuffer, HUINT32 ulBufferSize, POINTER_LIST_T **ppEpgList, HUINT8 *pszLang)
{
	HERROR					hError = ERR_TVTV_FAIL;
	HXMLDOC				hXmlDocHandle = NULL;
	HXMLELEMENT				hRootElement = NULL;
	HXMLELEMENT				hTempElement1 = NULL;
	HXMLELEMENT				hTempElement2 = NULL;
	HUINT32					ulIndex = 0;
	HUINT8					*pucXmlLang = NULL;
	HUINT8					*pszTempContents= NULL;
	HUINT8					*pszAttrContents= NULL;
	XsvcEpg_TvtvElementList_t 		*pstElementList = NULL;
	XsvcEpg_TvtvElement_t			*pstElement = NULL;
	XsvcEpg_TvtvEpgData_t			*pstTempEpg = NULL;
	XsvcEpg_TvtvName_t				*pstTempName = NULL;
	XsvcEpg_TvtvName_t				*pstLastName = NULL;
	XsvcEpg_TvtvContent_t			*pstTempContent = NULL;
	XsvcEpg_TvtvContent_t			*pstLastContent = NULL;
	XsvcEpg_TvtvImage_t			*pstTempImage = NULL;
//	XsvcEpg_TvtvImage_t			*pstLastImage = NULL;
	XsvcEpg_TvtvLink_t				*pstTempLink = NULL;
	XsvcEpg_TvtvLink_t				*pstLastLink = NULL;
	XsvcEpg_TvtvSubtitle_t			*pstTempSubtitle = NULL;
	XsvcEpg_TvtvSubtitle_t			*pstLastSubtitle = NULL;
	XsvcEpg_TvtvAudio_t			*pstTempAudio = NULL;
	XsvcEpg_TvtvAudio_t			*pstLastAudio = NULL;
	XsvcEpg_TvtvVideo_t			*pstTempVideo = NULL;
	XsvcEpg_TvtvVideo_t			*pstLastVideo = NULL;
	XsvcEpg_TvtvTip_t				*pstTempTip = NULL;
	XsvcEpg_TvtvTip_t				*pstLastTip = NULL;
	XsvcEpg_TvtvCat_t				*pstTempCat = NULL;
	XsvcEpg_TvtvCat_t				*pstLastCat = NULL;
	POINTER_LIST_T			*pSameChildList = NULL;
	POINTER_LIST_T			*pChildTemp = NULL;
	POINTER_LIST_T			*pProgList = NULL;
	POINTER_LIST_T			*pProgTemp = NULL;
	POINTER_LIST_T			*pEpgList = NULL;
	HUINT8					szStart[TVTV_TIME_MAX_LEN + 1];
	HUINT8					szEnd[TVTV_TIME_MAX_LEN + 1];
	HUINT8					*pszOtitle = NULL;
	HUINT8					*pszDtext = NULL;

	if( pRawBuffer == NULL || ppEpgList == NULL ||ulBufferSize == 0 || pszLang == NULL)
	{
		HxLOG_Print("[xsvc_epglib_ParseEpg_Tvtv] Param NULL!!\n");
		goto EXIT_TVTVLIB_FUNC;
	}

	hError = PAL_XML_InitParser();
	if(hError != ERR_XML_OK)
	{
		HxLOG_Print("[xsvc_epglib_ParseEpg_Tvtv] XML_InitParser fail !!\n");
		goto EXIT_TVTVLIB_FUNC;
	}

	hError = PAL_XML_ParseBuffer(pRawBuffer, ulBufferSize, &hXmlDocHandle);
	if(hError != ERR_XML_OK)
	{
		HxLOG_Print("[xsvc_epglib_ParseEpg_Tvtv] ParseBuffer fail !!\n");
		goto EXIT_TVTVLIB_FUNC;
	}

	//validation check
#if 0
	hError = PAL_XML_Validate(XML_VALIDATE_BUFFER, NULL, /*dtd buffer variable */, /* buffer size*/, hXmlDocHandle);
	if(hError != ERR_XML_OK)
	{
		HxLOG_Print("[xsvc_epglib_ParseEpg_Tvtv] XML_Validate fail !!\n");
		goto EXIT_TVTVLIB_FUNC;
	}
#endif

	hError = xsvc_epgdata_LoadElementList_Tvtv(TVTV_ELEMENT_LIST_EPG, &pstElementList);
	if(hError != ERR_XML_OK)
	{
		HxLOG_Print("[xsvc_epglib_ParseEpg_Tvtv] LoadElementList fail !!\n");
		goto EXIT_TVTVLIB_FUNC;
	}
	pstElement = pstElementList->pstElement;
	if(pstElement == NULL)
	{
		HxLOG_Print("[xsvc_epglib_ParseEpg_Tvtv] pstElement NULL !!\n");
		goto EXIT_TVTVLIB_FUNC;
	}

	hError = PAL_XML_GetRootElement(hXmlDocHandle, &hRootElement);
	if(hError != ERR_XML_OK)
	{
		HxLOG_Print("[xsvc_epglib_ParseEpg_Tvtv] GetRootElement fail !!\n");
		goto EXIT_TVTVLIB_FUNC;
	}

	hError = PAL_XML_GetAttrContents(hRootElement, pstElement[TVTV_EPG_EPG].pucAttrName, &pucXmlLang);
	if(hError != ERR_XML_OK)
	{
		HxLOG_Print("[xsvc_epglib_ParseEpg_Tvtv] GetAttrContents fail (%s)!!\n", pstElement[TVTV_EPG_EPG].pucAttrName);
		strncpy(pszLang, TVTV_LANG_DEFAULT, TVTV_MAX_LANG+1);
	}
	strncpy(pszLang, pucXmlLang, TVTV_MAX_LANG+1);
	PAL_XML_FreeAttrContents(pucXmlLang);
	pucXmlLang = NULL;

	hError = PAL_XML_FindChildElementByName(hRootElement, pstElement[TVTV_EPG_PROG].pucTagName, &pProgList);
	if(hError != ERR_XML_OK)
	{
		HxLOG_Print("[xsvc_epglib_ParseEpg_Tvtv] FindChildElementByName fail !!\n");
		goto EXIT_TVTVLIB_FUNC;
	}
	pProgTemp = pProgList;

	while(pProgTemp != NULL)
	{
		hTempElement1 = (HXMLELEMENT)UTIL_LINKEDLIST_GetContents(pProgTemp);
		if(hTempElement1 == NULL)
		{
			HxLOG_Print("[xsvc_epglib_ParseEpg_Tvtv] GetContents fail!!\n");
			goto EXIT_TVTVLIB_PROG_LOOP;
		}

		pstTempEpg = (XsvcEpg_TvtvEpgData_t*)OxEPG_Malloc(sizeof(XsvcEpg_TvtvEpgData_t));
		if(pstTempEpg == NULL)
		{
			HxLOG_Print("[xsvc_epglib_ParseEpg_Tvtv] Alloc fail!!\n");
			goto EXIT_TVTVLIB_PROG_LOOP;
		}
		HxSTD_memset(pstTempEpg, 0x00, sizeof(XsvcEpg_TvtvEpgData_t));

		for(ulIndex = TVTV_EPG_PROGID; ulIndex < TVTV_EPG_MAX; ulIndex++)
		{

			hError = PAL_XML_FindChildElementByName(hTempElement1, pstElement[ulIndex].pucTagName, &pSameChildList);
			if(hError != ERR_XML_OK)
			{
				HxLOG_Print("[xsvc_epglib_ParseEpg_Tvtv] FindChildElementByName fail (%s)!!\n", pstElement[ulIndex].pucTagName);
				continue;
			}
			pChildTemp = pSameChildList;

			while(pChildTemp != NULL)
			{
				hTempElement2 = (HXMLELEMENT)UTIL_LINKEDLIST_GetContents(pChildTemp);
				if(hTempElement2 == NULL)
				{
					HxLOG_Print("[xsvc_epglib_ParseEpg_Tvtv] GetContents fail (%s)!!\n", pstElement[ulIndex].pucTagName);
					goto EXIT_TVTVLIB_SAMECHILD_LOOP;
				}

				hError = PAL_XML_GetElementContents(hXmlDocHandle, hTempElement2, &pszTempContents);
				if(hError != ERR_XML_OK)
				{
					HxLOG_Print("[xsvc_epglib_ParseEpg_Tvtv] GetElementContents fail (%s)!!\n", pstElement[ulIndex].pucTagName);
					goto EXIT_TVTVLIB_SAMECHILD_LOOP;
				}

				switch(ulIndex)
				{

					case TVTV_EPG_PROGID :
						hError = xsvc_epglib_SetDataByType_Tvtv(pstElement[ulIndex].ulTagDataType, pszTempContents, NULL, 0, 0, NULL, &pstTempEpg->ulProgId);
						if(hError != ERR_TVTV_OK)
						{
							HxLOG_Print("[xsvc_epglib_ParseEpg_Tvtv] SetDataByType ulProgId fail!!\n");
						}
						break;
					case TVTV_EPG_TVTVID:
						hError = xsvc_epglib_SetDataByType_Tvtv(pstElement[ulIndex].ulTagDataType, pszTempContents, NULL, 0, 0, NULL, &pstTempEpg->ulTvtvId);
						if(hError != ERR_TVTV_OK)
						{
							HxLOG_Print("[xsvc_epglib_ParseEpg_Tvtv] SetDataByType usTvtvId fail!!\n");
						}
						break;
					case TVTV_EPG_START:
						HxSTD_memset(szStart, 0x00, sizeof(HUINT8)*(TVTV_TIME_MAX_LEN+1));
						hError = xsvc_epglib_SetDataByType_Tvtv(pstElement[ulIndex].ulTagDataType, pszTempContents, NULL, 0, TVTV_TIME_MAX_LEN + 1, szStart, NULL);
						if(hError != ERR_TVTV_OK)
						{
							HxLOG_Print("[xsvc_epglib_ParseEpg_Tvtv] SetDataByType szStart fail!!\n");
						}

						hError = xsvc_epglib_ConvertStringTimeToUnixTime_Tvtv(szStart, (UNIXTIME*)&pstTempEpg->ulStart);
						if(hError != ERR_TVTV_OK)
						{
							HxLOG_Print("[xsvc_epglib_ParseLtot_Tvtv] SetDataByType ulStart fail!!\n");
						}
						break;
					case TVTV_EPG_END :
						HxSTD_memset(szEnd, 0x00, sizeof(HUINT8)*(TVTV_TIME_MAX_LEN+1));
						hError = xsvc_epglib_SetDataByType_Tvtv(pstElement[ulIndex].ulTagDataType, pszTempContents, NULL, 0, TVTV_TIME_MAX_LEN + 1, szEnd, NULL);
						if(hError != ERR_TVTV_OK)
						{
							HxLOG_Print("[xsvc_epglib_ParseEpg_Tvtv] SetDataByType szEnd fail!!\n");
						}

						hError = xsvc_epglib_ConvertStringTimeToUnixTime_Tvtv(szStart, (UNIXTIME*)&pstTempEpg->ulEnd);
						if(hError != ERR_TVTV_OK)
						{
							HxLOG_Print("[xsvc_epglib_ParseLtot_Tvtv] SetDataByType ulEnd fail!!\n");
						}
						break;
					case TVTV_EPG_PIL:
						hError = xsvc_epglib_SetDataByType_Tvtv(pstElement[ulIndex].ulTagDataType, pszTempContents, NULL, 0, 0, NULL, (HUINT32*)&pstTempEpg->ulPil);
						if(hError != ERR_TVTV_OK)
						{
							HxLOG_Print("[xsvc_epglib_ParseEpg_Tvtv] SetDataByType u20Pil fail!!\n");
						}
						break;
					case TVTV_EPG_TITLE :
						hError = xsvc_epglib_SetDataByType_Tvtv(pstElement[ulIndex].ulTagDataType, pszTempContents, NULL, 0, TVTV_MAX_TITLE + 1, pstTempEpg->szTitle, NULL);
						if(hError != ERR_TVTV_OK)
						{
							HxLOG_Print("[xsvc_epglib_ParseEpg_Tvtv] SetDataByType szTitle fail!!\n");
						}
						break;
					case TVTV_EPG_OTITLE :
						pszOtitle = (HUINT8*)OxEPG_Malloc(sizeof(HUINT8)*(TVTV_MAX_TITLE+1));
						if(pszOtitle == NULL)
						{
							HxLOG_Print("[xsvc_epglib_ParseEpg_Tvtv] pszOtitle Alloc fail!!\n");
							break;
						}
						HxSTD_memset(pszOtitle, 0x00, sizeof(HUINT8)*(TVTV_MAX_TITLE+1));

						hError = xsvc_epglib_SetDataByType_Tvtv(pstElement[ulIndex].ulTagDataType, pszTempContents, NULL, 0, TVTV_MAX_TITLE + 1, pszOtitle, NULL);
						if(hError != ERR_TVTV_OK)
						{
							HxLOG_Print("[xsvc_epglib_ParseEpg_Tvtv] SetDataByType pszOtitle fail!!\n");
							OxEPG_Free(pszOtitle);
							pszOtitle = NULL;
							break;
						}
						pstTempEpg->pcOtitle = pszOtitle;
						pszOtitle = NULL;
						break;

					case TVTV_EPG_LTEXT :
						hError = xsvc_epglib_SetDataByType_Tvtv(pstElement[ulIndex].ulTagDataType, pszTempContents, NULL, 0, TVTV_MAX_LONGTEXT + 1, pstTempEpg->szLtext, NULL);
						if(hError != ERR_TVTV_OK)
						{
							HxLOG_Print("[xsvc_epglib_ParseEpg_Tvtv] SetDataByType szLtext fail!!\n");
						}
						break;

					case TVTV_EPG_DTEXT :
						pszDtext = (HUINT8*)OxEPG_Malloc(sizeof(HUINT8)*(TVTV_MAX_DTEXT+1));
						if(pszDtext == NULL)
						{
							HxLOG_Print("[xsvc_epglib_ParseEpg_Tvtv] pszDtext Alloc fail!!\n");
							break;
						}
						HxSTD_memset(pszDtext, 0x00, sizeof(HUINT8)*(TVTV_MAX_DTEXT+1));

						hError = xsvc_epglib_SetDataByType_Tvtv(pstElement[ulIndex].ulTagDataType, pszTempContents, NULL, 0, TVTV_MAX_DTEXT + 1, pszDtext, NULL);
						if(hError != ERR_TVTV_OK)
						{
							HxLOG_Print("[xsvc_epglib_ParseEpg_Tvtv] SetDataByType pszDtext fail!!\n");
							OxEPG_Free(pszDtext);
							pszDtext  =NULL;
							break;
						}
						pstTempEpg->pcDtext = pszDtext;
						pszDtext = NULL;
						break;

					case TVTV_EPG_SID :
						hError = xsvc_epglib_SetDataByType_Tvtv(pstElement[ulIndex].ulTagDataType, pszTempContents, NULL, 0, 0, NULL, (HUINT32*)&pstTempEpg->ulSId);
						if(hError != ERR_TVTV_OK)
						{
							HxLOG_Print("[xsvc_epglib_ParseEpg_Tvtv] SetDataByType ulSId fail!!\n");
						}
						break;

					case TVTV_EPG_SNR :
						hError = xsvc_epglib_SetDataByType_Tvtv(pstElement[ulIndex].ulTagDataType, pszTempContents, NULL, 0, TVTV_MAX_SNR + 1, pstTempEpg->szSnr, NULL);
						if(hError != ERR_TVTV_OK)
						{
							HxLOG_Print("[xsvc_epglib_ParseEpg_Tvtv] SetDataByType szSnr fail!!\n");
						}
						break;

					case TVTV_EPG_SCT :
						hError = xsvc_epglib_SetDataByType_Tvtv(pstElement[ulIndex].ulTagDataType, pszTempContents, NULL, 0, 0, NULL, &pstTempEpg->ulSct);
						if(hError != ERR_TVTV_OK)
						{
							HxLOG_Print("[xsvc_epglib_ParseEpg_Tvtv] SetDataByType usSct fail!!\n");
						}
						break;

					case TVTV_EPG_COUNTRY :
						hError = xsvc_epglib_SetDataByType_Tvtv(pstElement[ulIndex].ulTagDataType, pszTempContents, NULL, 0, TVTV_MAX_COUNTRY + 1, pstTempEpg->szCountry, NULL);
						if(hError != ERR_TVTV_OK)
						{
							HxLOG_Print("[xsvc_epglib_ParseEpg_Tvtv] SetDataByType szCountry fail!!\n");
						}
						break;

					case TVTV_EPG_YEAR :
						hError = xsvc_epglib_SetDataByType_Tvtv(pstElement[ulIndex].ulTagDataType, pszTempContents, NULL, 0, TVTV_MAX_YEAR + 1, pstTempEpg->szYear, NULL);
						if(hError != ERR_TVTV_OK)
						{
							HxLOG_Print("[xsvc_epglib_ParseEpg_Tvtv] SetDataByType szYear fail!!\n");
						}
						break;

					case TVTV_EPG_FSK :
						hError = xsvc_epglib_SetDataByType_Tvtv(pstElement[ulIndex].ulTagDataType, pszTempContents, NULL, 0, 0, NULL, (HUINT32*)&pstTempEpg->ucFsk);
						if(hError != ERR_TVTV_OK)
						{
							HxLOG_Print("[xsvc_epglib_ParseEpg_Tvtv] SetDataByType ucFsk fail!!\n");
						}
						break;

					case TVTV_EPG_FORMAT :
						hError = xsvc_epglib_SetDataByType_Tvtv(pstElement[ulIndex].ulTagDataType, pszTempContents, NULL, 0, 0, NULL, (HUINT32*)&pstTempEpg->ucFormat);
						if(hError != ERR_TVTV_OK)
						{
							HxLOG_Print("[xsvc_epglib_ParseEpg_Tvtv] SetDataByType ucFormat fail!!\n");
						}
						break;

					case TVTV_EPG_NAME :
						pstTempName = (XsvcEpg_TvtvName_t*)OxEPG_Malloc(sizeof(XsvcEpg_TvtvName_t));
						if(pstTempName == NULL)
						{
							HxLOG_Print("[xsvc_epglib_ParseEpg_Tvtv] pstTempName alloc fail!!\n");
							break;
						}
						HxSTD_memset(pstTempName, 0x00, sizeof(XsvcEpg_TvtvName_t));

						hError = xsvc_epglib_SetDataByType_Tvtv(pstElement[ulIndex].ulTagDataType, pszTempContents, NULL, 0, TVTV_MAX_NAME + 1, pstTempName->szName, NULL);
						if(hError != ERR_TVTV_OK)
						{
							HxLOG_Print("[xsvc_epglib_ParseEpg_Tvtv] SetDataByType szName fail!!\n");
							OxEPG_Free(pstTempName);
							pstTempName = NULL;
							break;
						}

						hError = PAL_XML_GetAttrContents(hTempElement2, pstElement[ulIndex].pucAttrName, &pszAttrContents);
						if(hError != ERR_XML_OK)
						{
							HxLOG_Print("[xsvc_epglib_ParseEpg_Tvtv] pstTempName GetAttrContents fail!!\n");
							OxEPG_Free(pstTempName);
							pstTempName = NULL;
							break;
						}

						hError = xsvc_epglib_SetDataByType_Tvtv(pstElement[ulIndex].ulAttrDataType, pszAttrContents, NULL, 0, 0, NULL, (HUINT32*)& pstTempName->ucType);
						if(hError != ERR_TVTV_OK)
						{
							HxLOG_Print("[xsvc_epglib_ParseEpg_Tvtv] SetDataByType pstTempName ucType fail!!\n");
							OxEPG_Free(pstTempName);
							PAL_XML_FreeAttrContents(pszAttrContents);
							pszAttrContents = NULL;
							pstTempName = NULL;
							break;
						}

						pstLastName = pstTempEpg->pstName;
						// if root is null
						if(pstLastName == NULL)
						{
							pstTempEpg->pstName = pstTempName;
						}
						else
						{
							while(pstLastName->pstNext != NULL)
							{
								pstLastName = pstLastName->pstNext ;
							}
							pstLastName->pstNext = pstTempName;
						}

						PAL_XML_FreeAttrContents(pszAttrContents);
						pszAttrContents = NULL;
						pstTempName = NULL;
						break;
					case TVTV_EPG_CONTENT :
						pstTempContent = (XsvcEpg_TvtvContent_t*)OxEPG_Malloc(sizeof(XsvcEpg_TvtvContent_t));
						if(pstTempContent == NULL)
						{
							HxLOG_Print("[xsvc_epglib_ParseEpg_Tvtv] pstTempContent alloc fail!!\n");
							break;
						}
						HxSTD_memset(pstTempContent, 0x00, sizeof(XsvcEpg_TvtvContent_t));

						hError = xsvc_epglib_SetDataByType_Tvtv(pstElement[ulIndex].ulTagDataType, pszTempContents, NULL, 0, 0, NULL, (HUINT32*)&pstTempContent->ucContent);
						if(hError != ERR_TVTV_OK)
						{
							HxLOG_Print("[xsvc_epglib_ParseEpg_Tvtv] SetDataByType ucContent fail!!\n");
							OxEPG_Free(pstTempContent);
							pstTempContent = NULL;
							break;
						}

						pstLastContent = pstTempEpg->pstContent;
						// if root is null
						if(pstLastContent == NULL)
						{
							pstTempEpg->pstContent = pstTempContent;
						}
						else
						{
							while(pstLastContent->pstNext != NULL)
							{
								pstLastContent = pstLastContent->pstNext ;
							}
							pstLastContent->pstNext = pstTempContent;
						}
						pstTempContent = NULL;
						break;
					case TVTV_EPG_IMAGE :
						pstTempImage = (XsvcEpg_TvtvImage_t*)OxEPG_Malloc(sizeof(XsvcEpg_TvtvImage_t));
						if(pstTempImage == NULL)
						{
							HxLOG_Print("[xsvc_epglib_ParseEpg_Tvtv] pstTempImage alloc fail!!\n");
							break;
						}
						HxSTD_memset(pstTempImage, 0x00, sizeof(XsvcEpg_TvtvImage_t));

						hError = xsvc_epglib_SetDataByType_Tvtv(pstElement[ulIndex].ulTagDataType, pszTempContents, NULL, 0, TVTV_IMG_MAX_FILENAME + 1, pstTempImage->szFilename, NULL);
						if(hError != ERR_TVTV_OK)
						{
							HxLOG_Print("[xsvc_epglib_ParseEpg_Tvtv] SetDataByType szImage fail!!\n");
							OxEPG_Free(pstTempImage);
							pstTempImage = NULL;
							break;
						}

						hError = PAL_XML_GetAttrContents(hTempElement2, pstElement[ulIndex].pucAttrName, &pszAttrContents);
						if(hError != ERR_XML_OK)
						{
							HxLOG_Print("[xsvc_epglib_ParseEpg_Tvtv] pstTempImage GetAttrContents fail!!\n");
							OxEPG_Free(pstTempImage);
							pstTempImage = NULL;
							break;
						}

						hError = xsvc_epglib_SetDataByType_Tvtv(pstElement[ulIndex].ulAttrDataType, pszAttrContents, NULL, 0, TVTV_IMG_MAX_COPYRIGHT + 1, pstTempImage->szCopyright, NULL);
						if(hError != ERR_TVTV_OK)
						{
							HxLOG_Print("[xsvc_epglib_ParseEpg_Tvtv] SetDataByType szCopyright fail!!\n");
						}

						pstTempEpg->pstImageInfo = pstTempImage;

						PAL_XML_FreeAttrContents(pszAttrContents);
						pszAttrContents = NULL;
						pstTempImage = NULL;
						break;

					case TVTV_EPG_VER :
						hError = xsvc_epglib_SetDataByType_Tvtv(pstElement[ulIndex].ulTagDataType, pszTempContents, NULL, 0, 0, NULL, &pstTempEpg->ulVer);
						if(hError != ERR_TVTV_OK)
						{
							HxLOG_Print("[xsvc_epglib_ParseEpg_Tvtv] SetDataByType usVer fail!!\n");
						}
						break;

					case TVTV_EPG_LINK :
						pstTempLink = (XsvcEpg_TvtvLink_t*)OxEPG_Malloc(sizeof(XsvcEpg_TvtvLink_t));
						if(pstTempLink == NULL)
						{
							HxLOG_Print("[xsvc_epglib_ParseEpg_Tvtv] pstTempLink alloc fail!!\n");
							break;
						}
						HxSTD_memset(pstTempLink, 0x00, sizeof(XsvcEpg_TvtvLink_t));

						hError = xsvc_epglib_SetDataByType_Tvtv(pstElement[ulIndex].ulTagDataType, pszTempContents, NULL, 0, 0, NULL, &pstTempLink->ulProgId);
						if(hError != ERR_TVTV_OK)
						{
							HxLOG_Print("[xsvc_epglib_ParseEpg_Tvtv] SetDataByType ulProgId fail!!\n");
							OxEPG_Free(pstTempLink);
							pstTempLink = NULL;
							break;
						}

						hError = PAL_XML_GetAttrContents(hTempElement2, pstElement[ulIndex].pucAttrName, &pszAttrContents);
						if(hError != ERR_XML_OK)
						{
							HxLOG_Print("[xsvc_epglib_ParseEpg_Tvtv] pstTempLink GetAttrContents fail!!\n");
							OxEPG_Free(pstTempLink);
							pstTempLink = NULL;
							break;
						}

						hError = xsvc_epglib_SetDataByType_Tvtv(pstElement[ulIndex].ulTagDataType, pszAttrContents, NULL, 0, 0, NULL, (HUINT32*)&pstTempLink->ucType);
						if(hError != ERR_TVTV_OK)
						{
							HxLOG_Print("[xsvc_epglib_ParseEpg_Tvtv] SetDataByType pstTempLink ucType fail!!\n");
						}

						pstLastLink = pstTempEpg->pstLinkInfo;
						// if root is null
						if(pstLastLink == NULL)
						{
							pstTempEpg->pstLinkInfo = pstTempLink;
						}
						else
						{
							while(pstLastLink->pstNext != NULL)
							{
								pstLastLink = pstLastLink->pstNext ;
							}
							pstLastLink->pstNext = pstTempLink;
						}
						PAL_XML_FreeAttrContents(pszAttrContents);
						pszAttrContents = NULL;
						pstTempLink = NULL;
						break;

					case TVTV_EPG_SUBTITLE :
						pstTempSubtitle = (XsvcEpg_TvtvSubtitle_t*)OxEPG_Malloc(sizeof(XsvcEpg_TvtvSubtitle_t));
						if(pstTempSubtitle == NULL)
						{
							HxLOG_Print("[xsvc_epglib_ParseEpg_Tvtv] pstTempSubtitle alloc fail!!\n");
							break;
						}
						HxSTD_memset(pstTempSubtitle, 0x00, sizeof(XsvcEpg_TvtvSubtitle_t));

						hError = xsvc_epglib_SetDataByType_Tvtv(pstElement[ulIndex].ulTagDataType, pszTempContents, NULL, 0, 0, NULL, (HUINT32*)&pstTempSubtitle->ucSubtitle);
						if(hError != ERR_TVTV_OK)
						{
							HxLOG_Print("[xsvc_epglib_ParseEpg_Tvtv] SetDataByType ucSubtitle fail!!\n");
							OxEPG_Free(pstTempSubtitle);
							pstTempSubtitle = NULL;
							break;
						}

						pstLastSubtitle = pstTempEpg->pstSubtitle;
						// if root is null
						if(pstLastSubtitle == NULL)
						{
							pstTempEpg->pstSubtitle = pstTempSubtitle;
						}
						else
						{
							while(pstLastSubtitle->pstNext != NULL)
							{
								pstLastSubtitle = pstLastSubtitle->pstNext ;
							}
							pstLastSubtitle->pstNext = pstTempSubtitle;
						}
						pstTempSubtitle = NULL;
						break;

					case TVTV_EPG_AUDIO :
						pstTempAudio = (XsvcEpg_TvtvAudio_t*)OxEPG_Malloc(sizeof(XsvcEpg_TvtvAudio_t));
						if(pstTempAudio == NULL)
						{
							HxLOG_Print("[xsvc_epglib_ParseEpg_Tvtv] pstTempAudio alloc fail!!\n");
							break;
						}
						HxSTD_memset(pstTempAudio, 0x00, sizeof(XsvcEpg_TvtvAudio_t));

						hError = xsvc_epglib_SetDataByType_Tvtv(pstElement[ulIndex].ulTagDataType, pszTempContents, NULL, 0, 0, NULL, (HUINT32*)&pstTempAudio->ucAudio);
						if(hError != ERR_TVTV_OK)
						{
							HxLOG_Print("[xsvc_epglib_ParseEpg_Tvtv] SetDataByType ucAudio fail!!\n");
							OxEPG_Free(pstTempAudio);
							pstTempAudio = NULL;
							break;
						}

						pstLastAudio = pstTempEpg->pstAudio;
						// if root is null
						if(pstLastAudio == NULL)
						{
							pstTempEpg->pstAudio = pstTempAudio;
						}
						else
						{
							while(pstLastAudio->pstNext != NULL)
							{
								pstLastAudio = pstLastAudio->pstNext ;
							}
							pstLastAudio->pstNext = pstTempAudio;
						}
						pstTempAudio = NULL;
						break;

					case TVTV_EPG_VIDEO :
						pstTempVideo = (XsvcEpg_TvtvVideo_t*)OxEPG_Malloc(sizeof(XsvcEpg_TvtvVideo_t));
						if(pstTempVideo == NULL)
						{
							HxLOG_Print("[xsvc_epglib_ParseEpg_Tvtv] pstTempVideo alloc fail!!\n");
							break;
						}
						HxSTD_memset(pstTempVideo, 0x00, sizeof(XsvcEpg_TvtvVideo_t));

						hError = xsvc_epglib_SetDataByType_Tvtv(pstElement[ulIndex].ulTagDataType, pszTempContents, NULL, 0, 0, NULL, (HUINT32*)&pstTempVideo->ucVideo);
						if(hError != ERR_TVTV_OK)
						{
							HxLOG_Print("[xsvc_epglib_ParseEpg_Tvtv] SetDataByType ucVideo fail!!\n");
							OxEPG_Free(pstTempVideo);
							pstTempVideo = NULL;
							break;
						}

						pstLastVideo = pstTempEpg->pstVideo;
						// if root is null
						if(pstLastVideo == NULL)
						{
							pstTempEpg->pstVideo = pstTempVideo;
						}
						else
						{
							while(pstLastVideo->pstNext != NULL)
							{
								pstLastVideo = pstLastVideo->pstNext ;
							}
							pstLastVideo->pstNext = pstTempVideo;
						}
						pstTempVideo = NULL;
						break;

					case TVTV_EPG_TIP :
						pstTempTip = (XsvcEpg_TvtvTip_t*)OxEPG_Malloc(sizeof(XsvcEpg_TvtvTip_t));
						if(pstTempTip == NULL)
						{
							HxLOG_Print("[xsvc_epglib_ParseEpg_Tvtv] pstTempTip alloc fail!!\n");
							break;
						}
						HxSTD_memset(pstTempTip, 0x00, sizeof(XsvcEpg_TvtvTip_t));

						hError = xsvc_epglib_SetDataByType_Tvtv(pstElement[ulIndex].ulTagDataType, pszTempContents, NULL, 0, 0, NULL, (HUINT32*)&pstTempTip->ucTip);
						if(hError != ERR_TVTV_OK)
						{
							HxLOG_Print("[xsvc_epglib_ParseEpg_Tvtv] SetDataByType ucTip fail!!\n");
							OxEPG_Free(pstTempTip);
							pstTempTip = NULL;
							break;
						}

						pstLastTip = pstTempEpg->pstTip;
						// if root is null
						if(pstLastTip == NULL)
						{
							pstTempEpg->pstTip = pstTempTip;
						}
						else
						{
							while(pstLastTip->pstNext != NULL)
							{
								pstLastTip = pstLastTip->pstNext ;
							}
							pstLastTip->pstNext = pstTempTip;
						}
						pstTempTip= NULL;
						break;

					case TVTV_EPG_EVENTID :
						hError = xsvc_epglib_SetDataByType_Tvtv(pstElement[ulIndex].ulTagDataType, pszTempContents, NULL, 0, 0, NULL, (HUINT32*)&pstTempEpg->nEventId);
						if(hError != ERR_TVTV_OK)
						{
							HxLOG_Print("[xsvc_epglib_ParseEpg_Tvtv] SetDataByType nEventId fail!!\n");
						}
						break;

					case TVTV_EPG_CAT :
						pstTempCat = (XsvcEpg_TvtvCat_t*)OxEPG_Malloc(sizeof(XsvcEpg_TvtvCat_t));
						if(pstTempCat == NULL)
						{
							HxLOG_Print("[xsvc_epglib_ParseEpg_Tvtv] pstTempCat alloc fail!!\n");
							break;
						}
						HxSTD_memset(pstTempCat, 0x00, sizeof(XsvcEpg_TvtvCat_t));

						hError = xsvc_epglib_SetDataByType_Tvtv(pstElement[ulIndex].ulTagDataType, pszTempContents, NULL, 0, 0, NULL, &pstTempCat->ulCat);
						if(hError != ERR_TVTV_OK)
						{
							HxLOG_Print("[xsvc_epglib_ParseEpg_Tvtv] SetDataByType usCat fail!!\n");
							OxEPG_Free(pstTempCat);
							pstTempCat = NULL;
							break;
						}

						pstLastCat = pstTempEpg->pstCat;
						// if root is null
						if(pstLastCat == NULL)
						{
							pstTempEpg->pstCat = pstTempCat;
						}
						else
						{
							while(pstLastCat->pstNext != NULL)
							{
								pstLastCat = pstLastCat->pstNext ;
							}
							pstLastCat->pstNext = pstTempCat;
						}
						pstTempCat = NULL;
						break;

					case TVTV_EPG_NAME_EXT :
						pstTempName = (XsvcEpg_TvtvName_t*)OxEPG_Malloc(sizeof(XsvcEpg_TvtvName_t));
						if(pstTempName == NULL)
						{
							HxLOG_Print("[xsvc_epglib_ParseEpg_Tvtv] TVTV_EPG_NAME_EXT alloc fail!!\n");
							break;
						}
						HxSTD_memset(pstTempName, 0x00, sizeof(XsvcEpg_TvtvName_t));

						hError = xsvc_epglib_SetDataByType_Tvtv(pstElement[ulIndex].ulTagDataType, pszTempContents, NULL, 0, TVTV_MAX_NAME + 1, pstTempName->szName, NULL);
						if(hError != ERR_TVTV_OK)
						{
							HxLOG_Print("[xsvc_epglib_ParseEpg_Tvtv] SetDataByType TVTV_EPG_NAME_EXT szName fail!!\n");
							OxEPG_Free(pstTempName);
							pstTempName = NULL;
							break;
						}

						hError = PAL_XML_GetAttrContents(hTempElement2, pstElement[ulIndex].pucAttrName, &pszAttrContents);
						if(hError != ERR_XML_OK)
						{
							HxLOG_Print("[xsvc_epglib_ParseEpg_Tvtv] pstTempName  TVTV_EPG_NAME_EXT GetAttrContents fail!!\n");
							OxEPG_Free(pstTempName);
							pstTempName = NULL;
							break;
						}

						hError = xsvc_epglib_SetDataByType_Tvtv(pstElement[ulIndex].ulAttrDataType, pszAttrContents, NULL, 0, 0, NULL, (HUINT32*)& pstTempName->ucType);
						if(hError != ERR_TVTV_OK)
						{
							HxLOG_Print("[xsvc_epglib_ParseEpg_Tvtv] SetDataByType TVTV_EPG_NAME_EXT pstTempName ucType fail!!\n");
							OxEPG_Free(pstTempName);
							PAL_XML_FreeAttrContents(pszAttrContents);
							pszAttrContents = NULL;
							pstTempName  = NULL;
							break;
						}

						pstLastName = pstTempEpg->pstName;
						// if root is null
						if(pstLastName == NULL)
						{
							pstTempEpg->pstName = pstTempName;
						}
						else
						{
							while(pstLastName->pstNext != NULL)
							{
								pstLastName = pstLastName->pstNext ;
							}
							pstLastName->pstNext = pstTempName;
						}

						PAL_XML_FreeAttrContents(pszAttrContents);
						pszAttrContents = NULL;
						pstTempName = NULL;
						break;
					default :
						break;
				}

				PAL_XML_FreeAttrContents(pszTempContents);
				pszTempContents = NULL;

EXIT_TVTVLIB_SAMECHILD_LOOP :
				pChildTemp = UTIL_LINKEDLIST_GetNextPointer(pChildTemp);
				if(pChildTemp == NULL || pChildTemp == pSameChildList)
				{
					break;
				}
			}

			pSameChildList = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList(pSameChildList, NULL);
			if(pSameChildList != NULL)
			{
				HxLOG_Print("[xsvc_epglib_ParseEpg_Tvtv] FreeWholeListItemsFromPointerList Fail!!\n");
				pSameChildList = NULL;
			}

		}

		pEpgList = UTIL_LINKEDLIST_AppendListItemToPointerList(pEpgList, (void*)pstTempEpg);
		if(pEpgList == NULL)
		{
			HxLOG_Print("[xsvc_epglib_ParseEpg_Tvtv] AppendListItemToPointerList Fail!!\n");
		}

EXIT_TVTVLIB_PROG_LOOP :

		pProgTemp = UTIL_LINKEDLIST_GetNextPointer(pProgTemp);
		if(pProgTemp == NULL || pProgTemp == pProgList)
		{
			HxLOG_Print("[xsvc_epglib_ParseEpg_Tvtv] Loop end !!\n");
			break;
		}
	}

	pProgList = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList(pProgList, NULL);
	if(pProgList != NULL)
	{
		HxLOG_Print("[xsvc_epglib_ParseEpg_Tvtv] pProgList Fail!!\n");
		pProgList = NULL;
	}

	hError = PAL_XML_FreeDocument(hXmlDocHandle);
	if(hError != ERR_XML_OK)
	{
		HxLOG_Print("[xsvc_epglib_ParseEpg_Tvtv] FreeDocument Fail!!\n");
	}

	*ppEpgList = pEpgList;
	hError = ERR_TVTV_OK;

EXIT_TVTVLIB_FUNC :

	return hError;

}







HERROR		xsvc_epglib_ParseLtot_Tvtv(HUINT8 *pRawBuffer, HUINT32 ulBufferSize, POINTER_LIST_T **ppLtotList)
{
	HERROR					hError = ERR_TVTV_FAIL;
	HUINT32					ulIndex = 0;
	HXMLDOC				hXmlDocHandle = NULL;
	HXMLELEMENT				hRootElement = NULL;
	HXMLELEMENT				hTempElement1 = NULL;
	HXMLELEMENT				hTempElement2 = NULL;
	HUINT8					*pszTempContents= NULL;
	XsvcEpg_TvtvElementList_t 		*pstElementList = NULL;
	XsvcEpg_TvtvElement_t			*pstElement = NULL;
	XsvcEpg_TvtvLtot_t				*pstTempLtot = NULL;
	POINTER_LIST_T			*pSameChildList = NULL;
	POINTER_LIST_T			*pChildTemp = NULL;
	POINTER_LIST_T			*pLtoList = NULL;
	POINTER_LIST_T			*pLtoTemp = NULL;
	POINTER_LIST_T			*pLtotList = NULL;

	if( pRawBuffer == NULL || ppLtotList == NULL ||ulBufferSize == 0)
	{
		HxLOG_Print("[xsvc_epglib_ParseLtot_Tvtv] Param NULL!!\n");
		goto EXIT_TVTVLIB_FUNC;
	}


	hError = PAL_XML_InitParser();
	if(hError != ERR_XML_OK)
	{
		HxLOG_Print("[xsvc_epglib_ParseLtot_Tvtv] XML_InitParser fail !!\n");
		goto EXIT_TVTVLIB_FUNC;
	}

	hError = PAL_XML_ParseBuffer(pRawBuffer, ulBufferSize, &hXmlDocHandle);
	if(hError != ERR_XML_OK)
	{
		HxLOG_Print("[xsvc_epglib_ParseLtot_Tvtv] ParseBuffer fail !!\n");
		goto EXIT_TVTVLIB_FUNC;
	}

	// Todo validation check
#if 0
	hError = PAL_XML_Validate(XML_VALIDATE_BUFFER, NULL, /*dtd buffer variable */, /* buffer size*/, hXmlDocHandle);
	if(hError != ERR_XML_OK)
	{
		HxLOG_Print("[xsvc_epglib_ParseLtot_Tvtv] XML_Validate fail !!\n");
		goto EXIT_TVTVLIB_FUNC;
	}
#endif

	hError = xsvc_epgdata_LoadElementList_Tvtv(TVTV_ELEMENT_LIST_LTOT, &pstElementList);
	if(hError != ERR_XML_OK)
	{
		HxLOG_Print("[xsvc_epglib_ParseLtot_Tvtv] LoadElementList fail !!\n");
		goto EXIT_TVTVLIB_FUNC;
	}
	pstElement = pstElementList->pstElement;
	if(pstElement == NULL)
	{
		HxLOG_Print("[xsvc_epglib_ParseLtot_Tvtv] pstElement NULL !!\n");
		goto EXIT_TVTVLIB_FUNC;
	}

	hError = PAL_XML_GetRootElement(hXmlDocHandle, &hRootElement);
	if(hError != ERR_XML_OK)
	{
		HxLOG_Print("[xsvc_epglib_ParseLtot_Tvtv] GetRootElement fail !!\n");
		goto EXIT_TVTVLIB_FUNC;
	}

	hError = PAL_XML_FindChildElementByName(hRootElement, pstElement[TVTV_LTOT_LTO].pucTagName, &pLtoList);
	if(hError != ERR_XML_OK)
	{
		HxLOG_Print("[xsvc_epglib_ParseLtot_Tvtv] GetRootElement fail !!\n");
		goto EXIT_TVTVLIB_FUNC;
	}
	pLtoTemp = pLtoList;

	while(pLtoTemp != NULL)
	{
		hTempElement1 = (HXMLELEMENT)UTIL_LINKEDLIST_GetContents(pLtoTemp);
		if(hTempElement1 == NULL)
		{
			HxLOG_Print("[xsvc_epglib_ParseLtot_Tvtv] GetContents fail!!\n");
			goto EXIT_TVTVLIB_LTO_LOOP;
		}

		pstTempLtot = (XsvcEpg_TvtvLtot_t*)OxEPG_Malloc(sizeof(XsvcEpg_TvtvLtot_t));
		if(pstTempLtot == NULL)
		{
			HxLOG_Print("[xsvc_epglib_ParseLtot_Tvtv] Alloc fail!!\n");
			goto EXIT_TVTVLIB_LTO_LOOP;
		}
		HxSTD_memset(pstTempLtot, 0x00, sizeof(XsvcEpg_TvtvLtot_t));

		for(ulIndex = TVTV_LTOT_COC; ulIndex < TVTV_LTOT_MAX; ulIndex++)
		{

			hError = PAL_XML_FindChildElementByName(hTempElement1, pstElement[ulIndex].pucTagName, &pSameChildList);
			if(hError != ERR_XML_OK)
			{
				HxLOG_Print("[xsvc_epglib_ParseLtot_Tvtv] FindChildElementByName fail (%s)!!\n", pstElement[ulIndex].pucTagName);
				continue;
			}
			pChildTemp = pSameChildList;

			while(pChildTemp != NULL)
			{
				hTempElement2 = (HXMLELEMENT)UTIL_LINKEDLIST_GetContents(pChildTemp);
				if(hTempElement2 == NULL)
				{
					HxLOG_Print("[xsvc_epglib_ParseLtot_Tvtv] GetContents fail (%s)!!\n", pstElement[ulIndex].pucTagName);
					goto EXIT_TVTVLIB_SAMECHILD_LOOP;
				}

				hError = PAL_XML_GetElementContents(hXmlDocHandle, hTempElement2, &pszTempContents);
				if(hError != ERR_XML_OK)
				{
					HxLOG_Print("[xsvc_epglib_ParseLtot_Tvtv] GetElementContents fail (%s)!!\n", pstElement[ulIndex].pucTagName);
					goto EXIT_TVTVLIB_SAMECHILD_LOOP;
				}

				switch(ulIndex)
				{

					case TVTV_LTOT_COC :
						hError = xsvc_epglib_SetDataByType_Tvtv(pstElement[ulIndex].ulTagDataType, pszTempContents, NULL, 0, TVTV_MAX_COC + 1, pstTempLtot->szCoc, NULL);
						if(hError != ERR_TVTV_OK)
						{
							HxLOG_Print("[xsvc_epglib_ParseLtot_Tvtv] SetDataByType szCoc fail!!\n");
						}
						break;
					case TVTV_LTOT_COR :
						hError = xsvc_epglib_SetDataByType_Tvtv(pstElement[ulIndex].ulTagDataType, pszTempContents, NULL, 0, 0, NULL, (HUINT32*)&pstTempLtot->ucCor);
						if(hError != ERR_TVTV_OK)
						{
							HxLOG_Print("[xsvc_epglib_ParseLtot_Tvtv] SetDataByType ucCor fail!!\n");
						}
						break;
					case TVTV_LTOT_TOC:
						hError = xsvc_epglib_SetDataByType_Tvtv(pstElement[ulIndex].ulTagDataType, pszTempContents, NULL, 0, TVTV_MAX_TIME + 1, pstTempLtot->szToc, NULL);
						if(hError != ERR_TVTV_OK)
						{
							HxLOG_Print("[xsvc_epglib_ParseLtot_Tvtv] SetDataByType szCoc fail!!\n");
						}

						hError = xsvc_epglib_ConvertStringTimeToUnixTime_Tvtv(pstTempLtot->szToc, (UNIXTIME*)&pstTempLtot->ulToc);
						if(hError != ERR_TVTV_OK)
						{
							HxLOG_Print("[xsvc_epglib_ParseLtot_Tvtv] SetDataByType ulToc fail!!\n");
						}
						break;
					case TVTV_LTOT_LTOBEFORE:
						hError = xsvc_epglib_SetDataByType_Tvtv(pstElement[ulIndex].ulTagDataType, pszTempContents, NULL, 0, 0, NULL, &pstTempLtot->ulLtoBefore);
						if(hError != ERR_TVTV_OK)
						{
							HxLOG_Print("[xsvc_epglib_ParseLtot_Tvtv] SetDataByType usLtoBefore fail!!\n");
						}
						break;
					case TVTV_LTOT_LTOAFTER:
						hError = xsvc_epglib_SetDataByType_Tvtv(pstElement[ulIndex].ulTagDataType, pszTempContents, NULL, 0, 0, NULL, &pstTempLtot->ulLtoAfter);
						if(hError != ERR_TVTV_OK)
						{
							HxLOG_Print("[xsvc_epglib_ParseLtot_Tvtv] SetDataByType usLtoAfter fail!!\n");
						}
						break;
					default :
						break;
				}

				PAL_XML_FreeAttrContents(pszTempContents);
				pszTempContents = NULL;

EXIT_TVTVLIB_SAMECHILD_LOOP :

				pChildTemp = UTIL_LINKEDLIST_GetNextPointer(pChildTemp);
				if(pChildTemp == NULL || pChildTemp == pSameChildList)
				{
					break;
				}
			}

			pSameChildList = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList(pSameChildList, NULL);
			if(pSameChildList != NULL)
			{
				HxLOG_Print("[xsvc_epglib_ParseLtot_Tvtv] FreeWholeListItemsFromPointerList Fail!!\n");
				pSameChildList = NULL;
			}

		}

		pLtotList = UTIL_LINKEDLIST_AppendListItemToPointerList(pLtotList, (void*)pstTempLtot);
		if(pLtotList == NULL)
		{
			HxLOG_Print("[xsvc_epglib_ParseLtot_Tvtv] AppendListItemToPointerList Fail!!\n");
		}

EXIT_TVTVLIB_LTO_LOOP :

		pLtoTemp = UTIL_LINKEDLIST_GetNextPointer(pLtoTemp);
		if(pLtoTemp == NULL || pLtoTemp == pLtoList)
		{
			HxLOG_Print("[xsvc_epglib_ParseLtot_Tvtv] Loop end !!\n");
			break;
		}
	}

	pLtoList = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList(pLtoList, NULL);
	if(pLtoList != NULL)
	{
		HxLOG_Print("[xsvc_epglib_ParseLtot_Tvtv] pTsList Fail!!\n");
		pLtoList = NULL;
	}

	hError = PAL_XML_FreeDocument(hXmlDocHandle);
	if(hError != ERR_XML_OK)
	{
		HxLOG_Print("[xsvc_epglib_ParseLtot_Tvtv] FreeDocument Fail!!\n");
	}

	*ppLtotList = pLtotList;
	hError = ERR_TVTV_OK;

EXIT_TVTVLIB_FUNC :

	return hError;

}


#define	______TVTV_HUMAXPARSING_FUNC______
#if defined(TVTV_NEW_ONE)
HERROR		xsvc_epglib_ParseToHumaxServiceList_Tvtv(HUINT8 *pRawBuffer, HUINT32 ulBufferSize)
{
	HERROR					hError = ERR_TVTV_FAIL;
	HXMLDOC				hXmlDocHandle = NULL;
	HXMLELEMENT				hRootElement = NULL;
	HXMLELEMENT				hTempElement1 = NULL;
	HXMLELEMENT				hTempElement2 = NULL;
	HUINT32					ulIndex = 0;
	HUINT8					*pszTempContents= NULL;
	XsvcEpg_TvtvElementList_t 		*pstElementList = NULL;
	XsvcEpg_TvtvElement_t			*pstElement = NULL;
	svcEpg_TvtvMap_t				*pstTempTvtvMap = NULL;
	POINTER_LIST_T			*pSameChildList = NULL;
	POINTER_LIST_T			*pChildTemp = NULL;
	POINTER_LIST_T			*pTsList = NULL;
	POINTER_LIST_T			*pTsTemp = NULL;
	HUINT32					ulValue = 0;
	DxTvtvService_t			stSvcInfo = {0, };

	if( pRawBuffer == NULL||ulBufferSize == 0)
	{
		HxLOG_Print(" Param NULL!!\n");
		goto EXIT_TVTVLIB_FUNC;
	}


	hError = PAL_XML_InitParser();
	if(hError != ERR_XML_OK)
	{
		HxLOG_Print("XML_InitParser fail !!\n");
		goto EXIT_TVTVLIB_FUNC;
	}

	hError = PAL_XML_ParseBuffer(pRawBuffer, ulBufferSize, &hXmlDocHandle);
	if(hError != ERR_XML_OK)
	{
		HxLOG_Print("ParseBuffer fail !!\n");
		goto EXIT_TVTVLIB_FUNC;
	}

	// Todo validation check
#if 0
	hError = PAL_XML_Validate(XML_VALIDATE_BUFFER, NULL, /*dtd buffer variable */, /* buffer size*/, hXmlDocHandle);
	if(hError != ERR_XML_OK)
	{
		HxLOG_Print("[xsvc_epglib_ParseToHumaxServiceList_Tvtv] XML_Validate fail !!\n");
		goto EXIT_TVTVLIB_FUNC;
	}
#endif

	hError = xsvc_epgdata_LoadElementList_Tvtv(TVTV_ELEMENT_LIST_TSMT, &pstElementList);
	if(hError != ERR_XML_OK)
	{
		HxLOG_Print("LoadElementList fail !!\n");
		goto EXIT_TVTVLIB_FUNC;
	}
	pstElement = pstElementList->pstElement;
	if(pstElement == NULL)
	{
		HxLOG_Print("pstElement NULL !!\n");
		goto EXIT_TVTVLIB_FUNC;
	}

	hError = PAL_XML_GetRootElement(hXmlDocHandle, &hRootElement);
	if(hError != ERR_XML_OK)
	{
		HxLOG_Print("GetRootElement fail !!\n");
		goto EXIT_TVTVLIB_FUNC;
	}

	hError = PAL_XML_FindChildElementByName(hRootElement, pstElement[TVTV_TSMT_TS].pucTagName, &pTsList);
	if(hError != ERR_XML_OK)
	{
		HxLOG_Print("GetRootElement fail !!\n");
		goto EXIT_TVTVLIB_FUNC;
	}
	pTsTemp = pTsList;


	DAPI_BatchBegin(DxNAMEOF(DxTvtvService_t));

	while(pTsTemp != NULL)
	{
		hTempElement1 = (HXMLELEMENT)UTIL_LINKEDLIST_GetContents(pTsTemp);
		if(hTempElement1 == NULL)
		{
			HxLOG_Print("GetContents fail!!\n");
			goto EXIT_TVTVLIB_TS_LOOP;
		}

//		pstTempTvtvMap = NULL;
		if(pstTempTvtvMap == NULL)
			pstTempTvtvMap = (svcEpg_TvtvMap_t*)OxEPG_Malloc(sizeof(svcEpg_TvtvMap_t));

		if(pstTempTvtvMap == NULL)
		{
			HxLOG_Print("Alloc fail!!\n");
			goto EXIT_TVTVLIB_TS_LOOP;
		}
		HxSTD_memset(pstTempTvtvMap, 0x00, sizeof(svcEpg_TvtvMap_t));

		for(ulIndex = TVTV_TSMT_TVTVID; ulIndex < TVTV_TSMT_MAX; ulIndex++)
		{

			switch(ulIndex)
			{
				case TVTV_TSMT_SNAME :
				case TVTV_TSMT_LNAME :
				case TVTV_TSMT_LANG :
				case TVTV_TSMT_CAT :
				case TVTV_TSMT_LOGO :
				case TVTV_TSMT_ORDER :
				case TVTV_TSMT_CATYPE :
				case TVTV_TSMT_CAID :
					HxLOG_Print("SetDataByType skip(%d)!!\n", ulIndex);
					continue;
				default :
					break;
			}

			hError = PAL_XML_FindChildElementByName(hTempElement1, pstElement[ulIndex].pucTagName, &pSameChildList);
			if(hError != ERR_XML_OK)
			{
				HxLOG_Print("FindChildElementByName fail (%s)!!\n", pstElement[ulIndex].pucTagName);
				continue;
			}
			pChildTemp = pSameChildList;

			while(pChildTemp != NULL)
			{
				hTempElement2 = (HXMLELEMENT)UTIL_LINKEDLIST_GetContents(pChildTemp);
				if(hTempElement2 == NULL)
				{
					HxLOG_Print("GetContents fail (%s)!!\n", pstElement[ulIndex].pucTagName);
					goto EXIT_TVTVLIB_SAMECHILD_LOOP;
				}

				hError = PAL_XML_GetElementContents(hXmlDocHandle, hTempElement2, &pszTempContents);
				if(hError != ERR_XML_OK)
				{
					HxLOG_Print("GetElementContents fail (%s)!!\n", pstElement[ulIndex].pucTagName);
					goto EXIT_TVTVLIB_SAMECHILD_LOOP;
				}

				switch(ulIndex)
				{

					case TVTV_TSMT_TVTVID :
						ulValue = 0;
						hError = xsvc_epglib_SetDataByType_Tvtv(	pstElement[ulIndex].ulTagDataType,
															pszTempContents,
															NULL, 0, 0, NULL,
															&ulValue);
						if(hError != ERR_TVTV_OK)
						{
							HxLOG_Print("[xsvc_epglib_ParseToHumaxServiceList_Tvtv] SetDataByType usTvtvId fail!!\n");
						}
						pstTempTvtvMap->usTvtvId = (HUINT16)ulValue;
						break;
					case TVTV_TSMT_OID :
						ulValue = 0;
						hError = xsvc_epglib_SetDataByType_Tvtv(	pstElement[ulIndex].ulTagDataType,
															pszTempContents,
															NULL, 0, 0, NULL,
															&ulValue);
						if(hError != ERR_TVTV_OK)
						{
							HxLOG_Print("SetDataByType usOnId fail!!\n");
						}
						pstTempTvtvMap->stTripleId.usOnId = (HUINT16)ulValue;
						break;

					case TVTV_TSMT_TSID :
						ulValue = 0;
						hError = xsvc_epglib_SetDataByType_Tvtv(	pstElement[ulIndex].ulTagDataType,
															pszTempContents,
															NULL, 0, 0, NULL,
															&ulValue);
						if(hError != ERR_TVTV_OK)
						{
							HxLOG_Print("SetDataByType usTsId fail!!\n");
						}
						pstTempTvtvMap->stTripleId.usTsId = (HUINT16)ulValue;
						break;

					case TVTV_TSMT_SID :
						ulValue = 0;
						hError = xsvc_epglib_SetDataByType_Tvtv(	pstElement[ulIndex].ulTagDataType,
															pszTempContents,
															NULL, 0, 0, NULL,
															&ulValue);
						if(hError != ERR_TVTV_OK)
						{
							HxLOG_Print("SetDataByType usSId fail!!\n");
						}
						pstTempTvtvMap->stTripleId.usSvcId = (HUINT16)ulValue;
						break;

/**********************************************************************************************
 *
 * analog info area
 *
 **********************************************************************************************/
					case TVTV_TSMT_F1830 :
					case TVTV_TSMT_F2830 :
					case TVTV_TSMT_X26 :
					case TVTV_TSMT_VPS :
						if(pstTempTvtvMap != NULL)
						{
							OxEPG_Free(pstTempTvtvMap);
							pstTempTvtvMap = NULL;
						}
						goto EXIT_TVTVLIB_TS_LOOP;

#if					0
					case TVTV_TSMT_SNAME :
					case TVTV_TSMT_LNAME :
					case TVTV_TSMT_LANG :
					case TVTV_TSMT_CAT :
					case TVTV_TSMT_LOGO :
					case TVTV_TSMT_ORDER :
					case TVTV_TSMT_CATYPE :
					case TVTV_TSMT_CAID :
						HxLOG_Print("[xsvc_epglib_ParseToHumaxServiceList_Tvtv] SetDataByType skip(%d)!!\n", ulIndex);
						break;
#endif
					default :
						break;
				}

				PAL_XML_FreeAttrContents(pszTempContents);
				pszTempContents = NULL;

EXIT_TVTVLIB_SAMECHILD_LOOP :

				pChildTemp = UTIL_LINKEDLIST_GetNextPointer(pChildTemp);
				if(pChildTemp == NULL || pChildTemp == pSameChildList)
				{
					break;
				}
			}

			pSameChildList = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList(pSameChildList, NULL);
			if(pSameChildList != NULL)
			{
				HxLOG_Print("FreeWholeListItemsFromPointerList Fail!!\n");
				pSameChildList = NULL;
			}
		}

#if defined(TEMP_DBG_RPINT)
		xsvc_epgcontext_PrintTvtvMapInfo(eTVTVMAP_PARSE, pstTempTvtvMap);
#endif

		HxSTD_memset(&stSvcInfo, 0x00, sizeof(DxTvtvService_t));

		if(xsvc_epgcontext_GetSvcUidFromTripleId(pstTempTvtvMap->usTvtvId, pstTempTvtvMap->stTripleId.usOnId, pstTempTvtvMap->stTripleId.usTsId, pstTempTvtvMap->stTripleId.usSvcId, &pstTempTvtvMap->ulSvcUid) == TRUE)
		{
			stSvcInfo.usTvtvId = pstTempTvtvMap->usTvtvId;
			stSvcInfo.usOnId = pstTempTvtvMap->stTripleId.usOnId;
			stSvcInfo.usTsId = pstTempTvtvMap->stTripleId.usTsId;
			stSvcInfo.usSvcId = pstTempTvtvMap->stTripleId.usSvcId;
			stSvcInfo.ulSvcUid = pstTempTvtvMap->ulSvcUid;
#if 0 // test only
			{
				static int cnt = 0;
				HLIB_CMD_Printf("++++ TSMT Send To Dama :  cnt[%d] : uid (%d), tvtvid(%d), onId(0x%04x)(%d), tsid(0x%04x)(%d), svcId(0x%04x)(%d)\n", ++cnt,
					pstTempTvtvMap->ulSvcUid, pstTempTvtvMap->usTvtvId,
					pstTempTvtvMap->stTripleId.usOnId, pstTempTvtvMap->stTripleId.usOnId,
					pstTempTvtvMap->stTripleId.usTsId, pstTempTvtvMap->stTripleId.usTsId,
					pstTempTvtvMap->stTripleId.usSvcId,pstTempTvtvMap->stTripleId.usSvcId);
			}
#endif

			if(DAPI_Set(DxNAMEOF(DxTvtvService_t), stSvcInfo.ulSvcUid, &stSvcInfo, sizeof(DxTvtvService_t)) != ERR_OK)
			{
				HxLOG_Debug("DAPI SET FAIL[xsvc_epgcontext_SvcInfoSendToDama_tvtv]~~~~~~~~~~~~~~~~~~~~~~~~\n");
			}
		}

EXIT_TVTVLIB_TS_LOOP :

		pTsTemp = UTIL_LINKEDLIST_GetNextPointer(pTsTemp);
		if(pTsTemp == NULL || pTsTemp == pTsList)
		{
			HxLOG_Print("Loop end !!\n");
			break;
		}
	}

	DAPI_BatchEnd(DxNAMEOF(DxTvtvService_t));
	DAPI_Sync(DxNAMEOF(DxTvtvService_t));

	pTsList = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList(pTsList, NULL);
	if(pTsList != NULL)
	{
		HxLOG_Print("pTsList Fail!!\n");
		pTsList = NULL;
	}

	hError = PAL_XML_FreeDocument(hXmlDocHandle);
	if(hError != ERR_XML_OK)
	{
		HxLOG_Print("FreeDocument Fail!!\n");
	}

	if(pstTempTvtvMap)
	{
		HLIB_STD_MemFree(pstTempTvtvMap);
		pstTempTvtvMap = NULL;
	}

	hError = ERR_TVTV_OK;

EXIT_TVTVLIB_FUNC :

	return hError;

}

HERROR		xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv(HUINT8 *pRawBuffer, HUINT32 ulBufferSize, POINTER_LIST_T **ppEventList)
{
	HERROR					hError = ERR_TVTV_FAIL;
	HXMLDOC				hXmlDocHandle = NULL;
	HXMLELEMENT				hRootElement = NULL;
	HXMLELEMENT				hTempElement1 = NULL;
	HXMLELEMENT				hTempElement2 = NULL;
	HUINT32					ulIndex = 0;
	HUINT8					*pszTempContents= NULL;
	HUINT8					*pszAttrContents= NULL;
	XsvcEpg_TvtvElementList_t 		*pstElementDataList = NULL;
	XsvcEpg_TvtvElement_t			*pstElementData = NULL;

	SvcEpg_Event_t				*pNewEvent = NULL;
	SvcEpg_TvtvEvent_t		*pNewTvtvEvent = NULL;
//	SvcEpg_ExtText_t			*pEpgText = NULL;
//	SvcEpg_ExtDText_t			*pEpgDText = NULL;

	POINTER_LIST_T			*pElementList = NULL;
	POINTER_LIST_T			*pTempElementList = NULL;
	POINTER_LIST_T			*pProgList = NULL;
	POINTER_LIST_T			*pProgTemp = NULL;
	POINTER_LIST_T			*pEventList = NULL;
//	POINTER_LIST_T			*pExtEventList = NULL;

	HUINT8					szStart[TVTV_TIME_MAX_LEN + 1] = {0, };
	HUINT8					szEnd[TVTV_TIME_MAX_LEN + 1] = {0, };
//	HUINT8					szTitle[TVTV_MAX_TITLE + 1] = {0, };				// ?. If over 50, truncated.
//	HUINT8					szLtext[TVTV_MAX_LONGTEXT + 1] = {0, };
	///////////////////////////////////////////////////////////////////
	// TVTV DIGITAL BASIC EPG
	// CODE OF PRACTICE V 1.0.1
	//
	// 10.1 TV STATION MAPPING TABLE
	// 10.1.6 LANG : coded according to ISO 639-2
	///////////////////////////////////////////////////////////////////
	HUINT8					*pucISO639Code = NULL;
	HUINT8					szISO639Code[EPG_LANG_ISO_639_LEN + 1] = {0, };
	HUINT8					szLangCode[EPG_LANG_ISO_639_LEN + 1] = {0, };
	UNIXTIME				ulEndTime = 0;
	HUINT32					ulLangCodeLen = 0;
	HUINT8					ucLinkCount = 0;
	HUINT32					ulCategory = 0;
	HUINT8					ucCategoryCount = 0;
	HUINT32					ulProgId = 0;
	HUINT32					ulType = 0;
	HUINT8					*pucElementName = NULL;
	HBOOL					bFound = FALSE;
	HUINT32					ulValue = 0;
#if 1 //humandks_tvtv
	HBOOL					bUpdateFlag = FALSE;
	HLONG 					sysTime;
//	HxDATETIME_t	 		startTime;
	UNIXTIME 				ulCmpTimeAfter7Day = 60*60*24*7; // 7일
#endif


	if( pRawBuffer == NULL || ppEventList == NULL || ulBufferSize == 0)
	{
		HxLOG_Print("[xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv] Param NULL!!\n");
		goto EXIT_TVTVLIB_FUNC;
	}
	*ppEventList = NULL;

	hError = PAL_XML_InitParser();
	if(hError != ERR_XML_OK)
	{
		HxLOG_Print("[xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv] XML_InitParser fail !!\n");
		goto EXIT_TVTVLIB_FUNC;
	}

	hError = PAL_XML_ParseBuffer(pRawBuffer, ulBufferSize, &hXmlDocHandle);
	if(hError != ERR_XML_OK)
	{
		HxLOG_Print("[xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv] ParseBuffer fail !!\n");
		goto EXIT_TVTVLIB_FUNC;
	}

	//validation check
#if 0
	hError = PAL_XML_Validate(XML_VALIDATE_BUFFER, NULL, /*dtd buffer variable */, /* buffer size*/, hXmlDocHandle);
	if(hError != ERR_XML_OK)
	{
		HxLOG_Print("[xsvc_epglib_ParseEpg_Tvtv] XML_Validate fail !!\n");
		goto EXIT_TVTVLIB_FUNC;
	}
#endif

	hError = xsvc_epgdata_LoadElementList_Tvtv(TVTV_ELEMENT_LIST_EPG, &pstElementDataList);
	if(hError != ERR_XML_OK)
	{
		HxLOG_Print("[xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv] LoadElementList fail !!\n");
		goto EXIT_TVTVLIB_FUNC;
	}
	pstElementData = pstElementDataList->pstElement;
	if(pstElementData == NULL)
	{
		HxLOG_Print("[xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv] pstElementData NULL !!\n");
		goto EXIT_TVTVLIB_FUNC;
	}

	hError = PAL_XML_GetRootElement(hXmlDocHandle, &hRootElement);
	if(hError != ERR_XML_OK)
	{
		HxLOG_Print("[xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv] GetRootElement fail !!\n");
		goto EXIT_TVTVLIB_FUNC;
	}

	hError = PAL_XML_GetAttrContents(hRootElement, pstElementData[TVTV_EPG_EPG].pucAttrName, (HUINT8 **)&pucISO639Code);
	if(hError == ERR_XML_OK && pucISO639Code != NULL)
	{
		HxLOG_Print("[PAL_XML_GetAttrContents] GetAttrContents fail (%s)!!\n", pstElementData[TVTV_EPG_EPG].pucAttrName);
		MWC_UTIL_DvbStrcpy(szISO639Code, pucISO639Code);

		PAL_XML_FreeAttrContents(pucISO639Code);
	}
	else
	{
		MWC_UTIL_DvbStrcpy(szISO639Code, TVTV_LANG_DEFAULT);
	}
	hError = xsvc_epglib_SelectIso639LanguageEncoding_Tvtv(szISO639Code, szLangCode, &ulLangCodeLen);
	if(hError != ERR_OK)
	{
		HxLOG_Print("[xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv] szLangCode fail (%s)!!\n", pstElementData[TVTV_EPG_EPG].pucAttrName);
	}
#if	0
	HxLOG_Info("[Event] szISO639Code(%c%c%c) szLangCode[%x] szLangCode[%x] szLangCode[%x]!!\n",
																					szISO639Code[0],
																					szISO639Code[1],
																					szISO639Code[2],
																					szLangCode[0],
																					szLangCode[1],
																					szLangCode[2]));
#endif
	hError = PAL_XML_FindChildElementByName(hRootElement, pstElementData[TVTV_EPG_PROG].pucTagName, &pProgList);
	if(hError != ERR_XML_OK)
	{
		HxLOG_Print("[xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv] FindChildElementByName fail !!\n");
		goto EXIT_TVTVLIB_FUNC;
	}
	pProgTemp = pProgList;

#if defined(TVTV_NEW_ONE)
	DAPI_BatchBegin(DxEPG_NAMEOF(eDxEPG_TYPE_TVTV));
#endif

#if 1 //humandks_tvtv
	sysTime = HLIB_STD_GetSystemTime();
#endif

	while(pProgTemp != NULL)
	{
		hTempElement1 = (HXMLELEMENT)UTIL_LINKEDLIST_GetContents(pProgTemp);
		if(hTempElement1 == NULL)
		{
			HxLOG_Print("[xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv] GetContents fail!!\n");
			goto EXIT_TVTVLIB_PROG_LOOP;
		}

		pNewEvent = NULL;

		hError = xsvc_epgdblib_AllocEvent(&pNewEvent);
		if(pNewEvent == NULL || hError != ERR_OK)
		{
			HxLOG_Print("[xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv] Alloc fail!!\n");
			goto EXIT_TVTVLIB_PROG_LOOP;
		}
		pNewEvent->stEventInfo.ucEpgDataType = (HUINT8)EPG_DATA_TYPE_TVTV;
		pNewTvtvEvent = &(pNewEvent->stEventInfo.stTvtvEventInfo);

		pElementList = NULL;
		hError = PAL_XML_GetChildElementList(hTempElement1, &pElementList);
		if(hError != ERR_XML_OK)
		{
			HxLOG_Print("[xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv] PAL_XML_GetChildElementList fail!!\n");
			goto EXIT_TVTVLIB_PROG_LOOP;
		}

		HxSTD_memcpy(pNewTvtvEvent->aucIsoCountryCode, szISO639Code, EPG_LANG_ISO_639_LEN);
/**********************************************************************************************
*
* program count init
*
**********************************************************************************************/
		ucLinkCount = 0;
		ucCategoryCount = 0;

		pTempElementList = pElementList;

#if 1 //humandks_tvtv
		bUpdateFlag = FALSE;
#endif
		while(pTempElementList)
		{
			hTempElement2 = (HXMLELEMENT)UTIL_LINKEDLIST_GetContents(pTempElementList);
			if(hTempElement2 == NULL)
			{
				HxLOG_Print("[xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv] GetContents fail!!\n");
				goto EXIT_TVTVLIB_ELEMENT_LOOP;
			}

			pucElementName = NULL;
			hError = PAL_XML_GetElementName(hTempElement2, &pucElementName);
			if(hError != ERR_XML_OK || pucElementName == NULL)
			{
				HxLOG_Print("[xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv] PAL_XML_GetElementName fail!!\n");
				goto EXIT_TVTVLIB_ELEMENT_LOOP;
			}

			bFound = FALSE;
			for(ulIndex = TVTV_EPG_PROGID; ulIndex < TVTV_EPG_CAT; ulIndex++)
			{
				if(!strcmp(pstElementData[ulIndex].pucTagName, pucElementName))
				{
					bFound = TRUE;
					break;
				}
			}
			if(!bFound)
			{
				HxLOG_Print("[xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv] !bFound!!\n");
				goto EXIT_TVTVLIB_ELEMENT_LOOP;
			}

			hError = PAL_XML_GetElementContents(hXmlDocHandle, hTempElement2, &pszTempContents);
			if(hError != ERR_XML_OK)
			{
				HxLOG_Print("[xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv] PAL_XML_GetElementContents fail!!\n");
				goto EXIT_TVTVLIB_ELEMENT_LOOP;
			}

			switch(ulIndex)
			{
/**********************************************************************************************
*
* event info area
*
**********************************************************************************************/
				case TVTV_EPG_PROGID :
					hError = xsvc_epglib_SetDataByType_Tvtv(	pstElementData[ulIndex].ulTagDataType,
														pszTempContents,
														NULL, 0, 0, NULL,
														&ulValue);
					if(hError != ERR_TVTV_OK)
					{
						HxLOG_Print("[xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv] SetDataByType ulProgId fail!!\n");
					}
					pNewTvtvEvent->ulProgId = ulValue;
					break;

				case TVTV_EPG_TVTVID :
					hError = xsvc_epglib_SetDataByType_Tvtv(	pstElementData[ulIndex].ulTagDataType,
														pszTempContents,
														NULL, 0, 0, NULL,
														&ulValue);
					if(hError != ERR_TVTV_OK)
					{
						HxLOG_Print("[xsvc_epglib_ParseEpg_Tvtv] SetDataByType usTvtvId fail!!\n");
					}
					pNewTvtvEvent->usTvtvId = (HUINT16)ulValue;
					break;

				case TVTV_EPG_START :
					HxSTD_memset(szStart, 0x00, sizeof(HUINT8)*(TVTV_TIME_MAX_LEN+1));
					hError = xsvc_epglib_SetDataByType_Tvtv(	pstElementData[ulIndex].ulTagDataType,
														pszTempContents,
														NULL, 0,
														TVTV_TIME_MAX_LEN + 1,
														szStart,
														NULL);
					if(hError != ERR_TVTV_OK)
					{
						HxLOG_Print("[xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv] SetDataByType szStart fail!!\n");
					}

					hError = xsvc_epglib_ConvertStringTimeToUnixTime_Tvtv(szStart, (UNIXTIME*)&pNewEvent->ulStartTime);
					if(hError != ERR_TVTV_OK)
					{
						HxLOG_Print("[xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv] SetDataByType ulStart fail!!\n");
					}

#if 1 //humandks_tvtv
					if(pNewEvent->ulStartTime <= (sysTime+ulCmpTimeAfter7Day))
					{
						bUpdateFlag = TRUE;
					}
#endif
					break;

				case TVTV_EPG_END :
					HxSTD_memset(szEnd, 0x00, sizeof(HUINT8)*(TVTV_TIME_MAX_LEN+1));
					hError = xsvc_epglib_SetDataByType_Tvtv(	pstElementData[ulIndex].ulTagDataType,
														pszTempContents,
														NULL, 0,
														TVTV_TIME_MAX_LEN + 1,
														szEnd,
														NULL);
					if(hError != ERR_TVTV_OK)
					{
						HxLOG_Print("[xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv] SetDataByType szEnd fail!!\n");
					}
					ulEndTime = 0;
					hError = xsvc_epglib_ConvertStringTimeToUnixTime_Tvtv(szEnd, (UNIXTIME*)&ulEndTime);
					if(hError != ERR_TVTV_OK)
					{
						HxLOG_Print("[xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv] SetDataByType ulEnd fail!!\n");
					}
					pNewEvent->ulDuration = ulEndTime - pNewEvent->ulStartTime;
					break;

				case TVTV_EPG_TITLE :
#if 1 // tvtv epg에 의한 memory leak 수정사항.w
					HxSTD_memset(&pNewEvent->ucEventName, 0, sizeof(pNewEvent->ucEventName));

					hError = xsvc_epglib_SetDataByType_Tvtv(	pstElementData[ulIndex].ulTagDataType,
														pszTempContents,
														szLangCode,
														ulLangCodeLen,
														TVTV_MAX_TITLE + 1,
														pNewEvent->ucEventName,
														NULL);
					if(hError != ERR_TVTV_OK)
					{
						HxLOG_Print("[xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv] SetDataByType szTitle fail!!\n");
					}
#else
					szTitle[0] = (HUINT8)0;
					hError = xsvc_epglib_SetDataByType_Tvtv(	pstElementData[ulIndex].ulTagDataType,
														pszTempContents,
														szLangCode,
														ulLangCodeLen,
														TVTV_MAX_TITLE + 1,
														szTitle,
														NULL);
					if(hError != ERR_TVTV_OK)
					{
						HxLOG_Print("[xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv] SetDataByType szTitle fail!!\n");
					}
					pEpgText = NULL;

					hError = xsvc_epgdblib_MakeEpgTextList(szTitle, pNewEvent->astShortText[0].pEventName, &pEpgText);
					if(hError != ERR_OK)
					{
						HxLOG_Print("[xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv] xsvc_epgdblib_MakeEpgTextList fail!!\n");
					}
					HxSTD_memcpy(pNewEvent->astShortText[0].aucIsoCountryCode, szISO639Code, EPG_LANG_ISO_639_LEN);
					pNewEvent->astShortText[0].pEventName = pEpgText;
#endif
					pNewTvtvEvent->usEventInfoMask |= eEPG_DES_SHORT_EVENT;
					break;

				case TVTV_EPG_LTEXT :
#if 1 // tvtv epg에 의한 memory leak 수정사항.
					HxSTD_memset(&pNewEvent->ucEventText, 0, sizeof(pNewEvent->ucEventText));
					hError = xsvc_epglib_SetDataByType_Tvtv(	pstElementData[ulIndex].ulTagDataType,
														pszTempContents,
														szLangCode,
														ulLangCodeLen,
														TVTV_MAX_LONGTEXT + 1,
														pNewEvent->ucEventText,
														NULL);

					if(hError != ERR_TVTV_OK)
					{
						HxLOG_Print("[xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv] SetDataByType szLtext fail!!\n");
					}
#else
					szLtext[0] = (HUINT8)0;
					hError = xsvc_epglib_SetDataByType_Tvtv(	pstElementData[ulIndex].ulTagDataType,
														pszTempContents,
														szLangCode,
														ulLangCodeLen,
														TVTV_MAX_LONGTEXT + 1,
														szLtext,
														NULL);
					if(hError != ERR_TVTV_OK)
					{
						HxLOG_Print("[xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv] SetDataByType szLtext fail!!\n");
					}
					pEpgText = NULL;

					hError = xsvc_epgdblib_MakeEpgTextList(szLtext, pNewEvent->astShortText[0].pEventText, &pEpgText);
					if(hError != ERR_OK)
					{
						HxLOG_Print("[xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv] xsvc_epgdblib_MakeEpgTextList fail!!\n");
					}
					HxSTD_memcpy(pNewEvent->astShortText[0].aucIsoCountryCode, szISO639Code, EPG_LANG_ISO_639_LEN);
					pNewEvent->astShortText[0].pEventText = pEpgText;
#endif
					pNewTvtvEvent->usEventInfoMask |= eEPG_DES_SHORT_EVENT;
					break;

				case TVTV_EPG_DTEXT :
#if 1 // tvtv epg에 의한 memory leak 수정사항.
					HxSTD_memset(&pNewEvent->ucEventDText, 0, sizeof(pNewEvent->ucEventDText));
					hError = xsvc_epglib_SetDataByType_Tvtv(pstElementData[ulIndex].ulTagDataType, pszTempContents, NULL, 0, TVTV_MAX_DTEXT + 1, pNewEvent->ucEventDText, NULL);
					if(hError != ERR_TVTV_OK)
					{
						HxLOG_Print("[xsvc_epglib_ParseEpg_Tvtv] SetDataByType pEpgDText fail!!\n");
					}
#else
					pEpgDText = (HUINT8*)OxEPG_Malloc(sizeof(HUINT8)*(TVTV_MAX_DTEXT+1));
					if(pEpgDText == NULL)
					{
						HxLOG_Print("[xsvc_epglib_ParseEpg_Tvtv] pEpgDText Alloc fail!!\n");
						break;
					}
					HxSTD_memset(pEpgDText, 0x00, sizeof(HUINT8)*(TVTV_MAX_DTEXT+1));

					hError = xsvc_epglib_SetDataByType_Tvtv(pstElementData[ulIndex].ulTagDataType, pszTempContents, NULL, 0, TVTV_MAX_DTEXT + 1, pEpgDText, NULL);
					if(hError != ERR_TVTV_OK)
					{
						HxLOG_Print("[xsvc_epglib_ParseEpg_Tvtv] SetDataByType pEpgDText fail!!\n");
						OxEPG_Free(pEpgDText);
						pEpgDText  =NULL;
						break;
					}
					pNewEvent->astShortText[0].pEventDText = pEpgDText;
#endif
					pNewTvtvEvent->usEventInfoMask |= eEPG_DES_EXTENDED_EVENT;
					break;

				case TVTV_EPG_SID :
					hError = xsvc_epglib_SetDataByType_Tvtv(	pstElementData[ulIndex].ulTagDataType,
														pszTempContents,
														NULL, 0, 0, NULL,
														&ulValue);
					if(hError != ERR_TVTV_OK)
					{
						HxLOG_Print("[xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv] SetDataByType ulSId fail!!\n");
					}
					pNewTvtvEvent->ulSeriesId = ulValue;
					break;

				case TVTV_EPG_SNR :
					hError = xsvc_epglib_SetDataByType_Tvtv(	pstElementData[ulIndex].ulTagDataType,
														pszTempContents,
														NULL, 0,
														TVTV_MAX_SNR + 1,
														pNewTvtvEvent->aucSeriesNumber,
														NULL);
					if(hError != ERR_TVTV_OK)
					{
						HxLOG_Print("[xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv] SetDataByType szSnr fail!!\n");
					}
					break;

				case TVTV_EPG_FSK :
					hError = xsvc_epglib_SetDataByType_Tvtv(	pstElementData[ulIndex].ulTagDataType,
														pszTempContents,
														NULL, 0, 0, NULL,
														&ulValue);
					if(hError != ERR_TVTV_OK)
					{
						HxLOG_Print("[xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv] SetDataByType ucFsk fail!!\n");
					}
					// for DVB spec
					HxSTD_memcpy(pNewTvtvEvent->astParentalRating[0].aucIsoCountryCode, szISO639Code, sizeof(HUINT8)*(EPG_LANG_ISO_639_LEN+1));
					pNewTvtvEvent->astParentalRating[0].ucParentalRating = (HUINT8)ulValue - 3;
					break;

				case TVTV_EPG_CONTENT :
					if(pNewEvent->aucContent[0] != 0)
					{
						break;
					}
					hError = xsvc_epglib_SetDataByType_Tvtv(	pstElementData[ulIndex].ulTagDataType,
														pszTempContents,
														NULL, 0, 0, NULL,
														(HUINT32*)&pNewEvent->aucContent[0]);
					if(hError != ERR_TVTV_OK)
					{
						HxLOG_Print("[xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv] SetDataByType ucContent fail!!\n");
					}
#if					0
					{
						HUINT8	ucCode = 0;
						ucCode = pNewEvent->ucContent & 0x4f;
						if(ucCode & 0x40)
						{
							HxLOG_Info("[xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv] TVTV_EPG_CONTENT (%s)!!\n",
													pNewEvent->astShortText[0].pEventName->aucTextChar));
						}
					}
#endif
					break;

				case TVTV_EPG_IMAGE :
					hError = xsvc_epglib_SetDataByType_Tvtv(	pstElementData[ulIndex].ulTagDataType,
														pszTempContents,
														NULL, 0,
														TVTV_IMG_MAX_FILENAME + 1,
														pNewTvtvEvent->aucImageName,
														NULL);
					if(hError != ERR_TVTV_OK)
					{
						HxLOG_Print("[xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv] SetDataByType szImage fail!!\n");
					}
					pNewTvtvEvent->ucIsImage = 1;
					break;

				case TVTV_EPG_VER :
					hError = xsvc_epglib_SetDataByType_Tvtv(	pstElementData[ulIndex].ulTagDataType,
														pszTempContents,
														NULL, 0, 0, NULL,
														&ulValue);
					if(hError != ERR_TVTV_OK)
					{
						HxLOG_Print("[xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv] SetDataByType usVer fail!!\n");
					}
					pNewTvtvEvent->usVersion = (HUINT16)ulValue;
					break;

				case TVTV_EPG_LINK :
					if(ucLinkCount >= EPG_TVTV_LINK_MAX)
					{
						break;
					}
					ulProgId = 0;
					hError = xsvc_epglib_SetDataByType_Tvtv(	pstElementData[ulIndex].ulTagDataType,
														pszTempContents,
														NULL, 0, 0, NULL,
														(HUINT32*)&ulProgId);
					if(hError != ERR_TVTV_OK)
					{
						HxLOG_Print("[xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv] SetDataByType ulProgId fail!!\n");
						break;
					}

					hError = PAL_XML_GetAttrContents(hTempElement2, pstElementData[ulIndex].pucAttrName, &pszAttrContents);
					if(hError != ERR_XML_OK)
					{
						HxLOG_Print("[xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv] pstTempLink GetAttrContents fail!!\n");
						break;
					}
					ulType = 0;
					hError = xsvc_epglib_SetDataByType_Tvtv(	pstElementData[ulIndex].ulAttrDataType,
														pszAttrContents,
														NULL, 0, 0, NULL,
														(HUINT32*)&ulType);
					if(hError != ERR_TVTV_OK)
					{
						HxLOG_Print("[xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv] SetDataByType pstTempLink ulType fail!!\n");
						PAL_XML_FreeAttrContents(pszAttrContents);
						pszAttrContents = NULL;
						break;
					}
					pNewTvtvEvent->astEventLink[ucLinkCount].ulProgId = ulProgId;
					pNewTvtvEvent->astEventLink[ucLinkCount].ucType = (HUINT8)ulType;
					ucLinkCount++;
					/** TVTV EPG SYSTEM SPEC V 2.4.2,  5.3.2.21 Link type table*/
					if(ulType == 2)
					{
						pNewTvtvEvent->usEventInfoMask |= eEPG_DES_SERIESLINK;
					}
					else if(ulType == 1)
					{
						pNewTvtvEvent->usEventInfoMask |= eEPG_DES_SAMELINK;
					}

					PAL_XML_FreeAttrContents(pszAttrContents);
					pszAttrContents = NULL;
					break;

				case TVTV_EPG_TIP :
					if(pNewTvtvEvent->ucTip != 0)
					{
						break;
					}
					hError = xsvc_epglib_SetDataByType_Tvtv(	pstElementData[ulIndex].ulTagDataType,
														pszTempContents,
														NULL, 0, 0, NULL,
														&ulValue);
					if(hError != ERR_TVTV_OK)
					{
						HxLOG_Print("[xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv] SetDataByType ucTip fail!!\n");
					}
					pNewTvtvEvent->ucTip = (HUINT8)ulValue;
					{
						HxDATETIME_t		stDateTime = {{0, }, };

						HLIB_DATETIME_ConvertUnixTimeToDateTime(pNewEvent->ulStartTime, &stDateTime);

						HxLOG_Info("[xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv] TVTV_EPG_TIP usTvtvId(%d)ulProgId(%d)ucTip(%d)(%02d.%02d  %02d:%02d)!!\n",
																					pNewTvtvEvent->usTvtvId,
																					pNewTvtvEvent->ulProgId,
																					pNewTvtvEvent->ucTip,
																					stDateTime.stDate.ucMonth,
																					stDateTime.stDate.ucDay,
																					stDateTime.stTime.ucHour,
																					stDateTime.stTime.ucMinute);
					}
					break;

				case TVTV_EPG_EVENTID :
					hError = xsvc_epglib_SetDataByType_Tvtv(	pstElementData[ulIndex].ulTagDataType,
														pszTempContents,
														NULL, 0, 0, NULL,
														&ulValue);
					if(hError != ERR_TVTV_OK)
					{
						HxLOG_Print("[xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv] SetDataByType nEventId fail!!\n");
					}
					pNewTvtvEvent->nEventId = ulValue;
					break;

				case TVTV_EPG_CAT :
					if(ucCategoryCount >= EPG_TVTV_CATEGORY_LEN)
					{
						break;
					}
					ulCategory = 0;
					hError = xsvc_epglib_SetDataByType_Tvtv(	pstElementData[ulIndex].ulTagDataType,
														pszTempContents,
														NULL, 0, 0, NULL,
														(HUINT32*)&ulCategory);
					if(hError != ERR_TVTV_OK)
					{
						HxLOG_Print("[xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv] SetDataByType usCat fail!!\n");
						break;
					}
					pNewTvtvEvent->ausCategory[ucCategoryCount] = (HUINT16)ulCategory;
					ucCategoryCount++;
					break;
				case TVTV_EPG_SEASON :
					hError = xsvc_epglib_SetDataByType_Tvtv(	pstElementData[ulIndex].ulTagDataType,
														pszTempContents,
														NULL, 0, 0, NULL,
														&ulValue);
					if(hError != ERR_TVTV_OK)
					{
						HxLOG_Print("[xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv] SetDataByType nEventId fail!!\n");
					}
					pNewTvtvEvent->ulSeason = ulValue;
					break;
				case TVTV_EPG_SCT :
					hError = xsvc_epglib_SetDataByType_Tvtv(	pstElementData[ulIndex].ulTagDataType,
														pszTempContents,
														NULL, 0, 0, NULL,
														&ulValue);
					if(hError != ERR_TVTV_OK)
					{
						HxLOG_Print("[xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv] SetDataByType nEventId fail!!\n");
					}
					pNewTvtvEvent->ulTotalNum = ulValue;
					break;

/**********************************************************************************************
*
* extended info area
*
**********************************************************************************************/
				case TVTV_EPG_NAME :
				case TVTV_EPG_NAME_EXT :
				case TVTV_EPG_YEAR :
				case TVTV_EPG_COUNTRY :
					pNewTvtvEvent->usEventInfoMask |= eEPG_DES_EXTENDED_EVENT;
					break;

#if				0
/**********************************************************************************************
*
* skip info area
*
**********************************************************************************************/
				case TVTV_EPG_PIL:
				case TVTV_EPG_OTITLE :
				case TVTV_EPG_SCT :
				case TVTV_EPG_FORMAT :
				case TVTV_EPG_SUBTITLE :
				case TVTV_EPG_AUDIO :
				case TVTV_EPG_VIDEO :
					HxLOG_Print("[xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv] skip element type(%d)!!\n", ulIndex);
					break;
#endif
				default :
					break;
			}

			PAL_XML_FreeAttrContents(pszTempContents);
			pszTempContents = NULL;
EXIT_TVTVLIB_ELEMENT_LOOP :
			pTempElementList = UTIL_LINKEDLIST_GetNextPointer(pTempElementList);
			if(pTempElementList == NULL || pElementList == pTempElementList)
			{
				break;
			}
		}

#if defined(TVTV_NEW_ONE)
#if 1 //humandks_tvtv
		if(bUpdateFlag == TRUE)
		{
#endif
			if(pNewEvent)
			{
				xsvc_epgcontext_SendToDamaEventIfno(pNewEvent);
			}
#if 1 //humandks_tvtv
			bUpdateFlag = FALSE;
		}
#endif
#endif

EXIT_TVTVLIB_PROG_LOOP :
		pEventList = UTIL_LINKEDLIST_AppendListItemToPointerList(pEventList, (void*)pNewEvent);
		if(pEventList == NULL)
		{
			HxLOG_Print("[xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv] AppendListItemToPointerList Fail!!\n");
		}
		pProgTemp = UTIL_LINKEDLIST_GetNextPointer(pProgTemp);
		if(pProgTemp == NULL || pProgTemp == pProgList)
		{
			HxLOG_Print("[xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv] Loop end !!\n");
			break;
		}
		pElementList = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList(pElementList, NULL);
		if(pElementList != NULL)
		{
			HxLOG_Print("[xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv] pElementList Fail!!\n");
			pElementList = NULL;
		}
	}

#if defined(TVTV_NEW_ONE)
	DAPI_BatchEnd(DxEPG_NAMEOF(eDxEPG_TYPE_TVTV));
//	HLIB_STD_TaskSleep(1000);
	HLIB_STD_TaskSleep(10);
#endif


	pProgList = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList(pProgList, NULL);
	if(pProgList != NULL)
	{
		HxLOG_Print("[xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv] pProgList Fail!!\n");
		pProgList = NULL;
	}

	hError = PAL_XML_FreeDocument(hXmlDocHandle);
	if(hError != ERR_XML_OK)
	{
		HxLOG_Print("[xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv] FreeDocument Fail!!\n");
	}

	*ppEventList = pEventList;

	hError = ERR_TVTV_OK;

EXIT_TVTVLIB_FUNC :

	return hError;

}
#else
HERROR		xsvc_epglib_ParseToHumaxServiceList_Tvtv(HUINT8 *pRawBuffer, HUINT32 ulBufferSize, POINTER_LIST_T **ppTvtvMapList)
{
	HERROR					hError = ERR_TVTV_FAIL;
	HXMLDOC				hXmlDocHandle = NULL;
	HXMLELEMENT				hRootElement = NULL;
	HXMLELEMENT				hTempElement1 = NULL;
	HXMLELEMENT				hTempElement2 = NULL;
	HUINT32					ulIndex = 0;
	HUINT8					*pszTempContents= NULL;
	XsvcEpg_TvtvElementList_t 		*pstElementList = NULL;
	XsvcEpg_TvtvElement_t			*pstElement = NULL;
	svcEpg_TvtvMap_t				*pstTempTvtvMap = NULL;
	POINTER_LIST_T			*pSameChildList = NULL;
	POINTER_LIST_T			*pChildTemp = NULL;
	POINTER_LIST_T			*pTsList = NULL;
	POINTER_LIST_T			*pTsTemp = NULL;
	POINTER_LIST_T			*pTvtvMapList = NULL;
	HUINT32					ulValue = 0;


	if( pRawBuffer == NULL || ppTvtvMapList == NULL ||ulBufferSize == 0)
	{
		HxLOG_Print("[xsvc_epglib_ParseToHumaxServiceList_Tvtv] Param NULL!!\n");
		goto EXIT_TVTVLIB_FUNC;
	}


	hError = PAL_XML_InitParser();
	if(hError != ERR_XML_OK)
	{
		HxLOG_Print("[xsvc_epglib_ParseToHumaxServiceList_Tvtv] XML_InitParser fail !!\n");
		goto EXIT_TVTVLIB_FUNC;
	}

	hError = PAL_XML_ParseBuffer(pRawBuffer, ulBufferSize, &hXmlDocHandle);
	if(hError != ERR_XML_OK)
	{
		HxLOG_Print("[xsvc_epglib_ParseToHumaxServiceList_Tvtv] ParseBuffer fail !!\n");
		goto EXIT_TVTVLIB_FUNC;
	}

	// Todo validation check
#if 0
	hError = PAL_XML_Validate(XML_VALIDATE_BUFFER, NULL, /*dtd buffer variable */, /* buffer size*/, hXmlDocHandle);
	if(hError != ERR_XML_OK)
	{
		HxLOG_Print("[xsvc_epglib_ParseToHumaxServiceList_Tvtv] XML_Validate fail !!\n");
		goto EXIT_TVTVLIB_FUNC;
	}
#endif

	hError = xsvc_epgdata_LoadElementList_Tvtv(TVTV_ELEMENT_LIST_TSMT, &pstElementList);
	if(hError != ERR_XML_OK)
	{
		HxLOG_Print("[xsvc_epglib_ParseToHumaxServiceList_Tvtv] LoadElementList fail !!\n");
		goto EXIT_TVTVLIB_FUNC;
	}
	pstElement = pstElementList->pstElement;
	if(pstElement == NULL)
	{
		HxLOG_Print("[xsvc_epglib_ParseToHumaxServiceList_Tvtv] pstElement NULL !!\n");
		goto EXIT_TVTVLIB_FUNC;
	}

	hError = PAL_XML_GetRootElement(hXmlDocHandle, &hRootElement);
	if(hError != ERR_XML_OK)
	{
		HxLOG_Print("[xsvc_epglib_ParseToHumaxServiceList_Tvtv] GetRootElement fail !!\n");
		goto EXIT_TVTVLIB_FUNC;
	}

	hError = PAL_XML_FindChildElementByName(hRootElement, pstElement[TVTV_TSMT_TS].pucTagName, &pTsList);
	if(hError != ERR_XML_OK)
	{
		HxLOG_Print("[xsvc_epglib_ParseToHumaxServiceList_Tvtv] GetRootElement fail !!\n");
		goto EXIT_TVTVLIB_FUNC;
	}
	pTsTemp = pTsList;

#if defined(TVTV_NEW_ONE)
	DAPI_BatchBegin(DxNAMEOF(DxTvtvService_t));
#endif

	while(pTsTemp != NULL)
	{
		hTempElement1 = (HXMLELEMENT)UTIL_LINKEDLIST_GetContents(pTsTemp);
		if(hTempElement1 == NULL)
		{
			HxLOG_Print("[xsvc_epglib_ParseToHumaxServiceList_Tvtv] GetContents fail!!\n");
			goto EXIT_TVTVLIB_TS_LOOP;
		}

		pstTempTvtvMap = NULL;
		pstTempTvtvMap = (svcEpg_TvtvMap_t*)OxEPG_Malloc(sizeof(svcEpg_TvtvMap_t));
		if(pstTempTvtvMap == NULL)
		{
			HxLOG_Print("[xsvc_epglib_ParseToHumaxServiceList_Tvtv] Alloc fail!!\n");
			goto EXIT_TVTVLIB_TS_LOOP;
		}
		HxSTD_memset(pstTempTvtvMap, 0x00, sizeof(svcEpg_TvtvMap_t));

		for(ulIndex = TVTV_TSMT_TVTVID; ulIndex < TVTV_TSMT_MAX; ulIndex++)
		{

			switch(ulIndex)
			{
				case TVTV_TSMT_SNAME :
				case TVTV_TSMT_LNAME :
				case TVTV_TSMT_LANG :
				case TVTV_TSMT_CAT :
				case TVTV_TSMT_LOGO :
				case TVTV_TSMT_ORDER :
				case TVTV_TSMT_CATYPE :
				case TVTV_TSMT_CAID :
					HxLOG_Print("[xsvc_epglib_ParseToHumaxServiceList_Tvtv] SetDataByType skip(%d)!!\n", ulIndex);
					continue;
				default :
					break;
			}

			hError = PAL_XML_FindChildElementByName(hTempElement1, pstElement[ulIndex].pucTagName, &pSameChildList);
			if(hError != ERR_XML_OK)
			{
				HxLOG_Print("[xsvc_epglib_ParseToHumaxServiceList_Tvtv] FindChildElementByName fail (%s)!!\n", pstElement[ulIndex].pucTagName);
				continue;
			}
			pChildTemp = pSameChildList;

			while(pChildTemp != NULL)
			{
				hTempElement2 = (HXMLELEMENT)UTIL_LINKEDLIST_GetContents(pChildTemp);
				if(hTempElement2 == NULL)
				{
					HxLOG_Print("[xsvc_epglib_ParseToHumaxServiceList_Tvtv] GetContents fail (%s)!!\n", pstElement[ulIndex].pucTagName);
					goto EXIT_TVTVLIB_SAMECHILD_LOOP;
				}

				hError = PAL_XML_GetElementContents(hXmlDocHandle, hTempElement2, &pszTempContents);
				if(hError != ERR_XML_OK)
				{
					HxLOG_Print("[xsvc_epglib_ParseToHumaxServiceList_Tvtv] GetElementContents fail (%s)!!\n", pstElement[ulIndex].pucTagName);
					goto EXIT_TVTVLIB_SAMECHILD_LOOP;
				}

				switch(ulIndex)
				{

					case TVTV_TSMT_TVTVID :
						ulValue = 0;
						hError = xsvc_epglib_SetDataByType_Tvtv(	pstElement[ulIndex].ulTagDataType,
															pszTempContents,
															NULL, 0, 0, NULL,
															&ulValue);
						if(hError != ERR_TVTV_OK)
						{
							HxLOG_Print("[xsvc_epglib_ParseToHumaxServiceList_Tvtv] SetDataByType usTvtvId fail!!\n");
						}
						pstTempTvtvMap->usTvtvId = (HUINT16)ulValue;
						break;
					case TVTV_TSMT_OID :
						ulValue = 0;
						hError = xsvc_epglib_SetDataByType_Tvtv(	pstElement[ulIndex].ulTagDataType,
															pszTempContents,
															NULL, 0, 0, NULL,
															&ulValue);
						if(hError != ERR_TVTV_OK)
						{
							HxLOG_Print("[xsvc_epglib_ParseToHumaxServiceList_Tvtv] SetDataByType usOnId fail!!\n");
						}
						pstTempTvtvMap->stTripleId.usOnId = (HUINT16)ulValue;
						break;

					case TVTV_TSMT_TSID :
						ulValue = 0;
						hError = xsvc_epglib_SetDataByType_Tvtv(	pstElement[ulIndex].ulTagDataType,
															pszTempContents,
															NULL, 0, 0, NULL,
															&ulValue);
						if(hError != ERR_TVTV_OK)
						{
							HxLOG_Print("[xsvc_epglib_ParseToHumaxServiceList_Tvtv] SetDataByType usTsId fail!!\n");
						}
						pstTempTvtvMap->stTripleId.usTsId = (HUINT16)ulValue;
						break;

					case TVTV_TSMT_SID :
						ulValue = 0;
						hError = xsvc_epglib_SetDataByType_Tvtv(	pstElement[ulIndex].ulTagDataType,
															pszTempContents,
															NULL, 0, 0, NULL,
															&ulValue);
						if(hError != ERR_TVTV_OK)
						{
							HxLOG_Print("[xsvc_epglib_ParseToHumaxServiceList_Tvtv] SetDataByType usSId fail!!\n");
						}
						pstTempTvtvMap->stTripleId.usSvcId = (HUINT16)ulValue;
						break;

/**********************************************************************************************
 *
 * analog info area
 *
 **********************************************************************************************/
					case TVTV_TSMT_F1830 :
					case TVTV_TSMT_F2830 :
					case TVTV_TSMT_X26 :
					case TVTV_TSMT_VPS :
						if(pstTempTvtvMap != NULL)
						{
							OxEPG_Free(pstTempTvtvMap);
							pstTempTvtvMap = NULL;
						}
						goto EXIT_TVTVLIB_TS_LOOP;

#if					0
					case TVTV_TSMT_SNAME :
					case TVTV_TSMT_LNAME :
					case TVTV_TSMT_LANG :
					case TVTV_TSMT_CAT :
					case TVTV_TSMT_LOGO :
					case TVTV_TSMT_ORDER :
					case TVTV_TSMT_CATYPE :
					case TVTV_TSMT_CAID :
						HxLOG_Print("[xsvc_epglib_ParseToHumaxServiceList_Tvtv] SetDataByType skip(%d)!!\n", ulIndex);
						break;
#endif
					default :
						break;
				}

				PAL_XML_FreeAttrContents(pszTempContents);
				pszTempContents = NULL;

EXIT_TVTVLIB_SAMECHILD_LOOP :

				pChildTemp = UTIL_LINKEDLIST_GetNextPointer(pChildTemp);
				if(pChildTemp == NULL || pChildTemp == pSameChildList)
				{
					break;
				}
			}

			pSameChildList = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList(pSameChildList, NULL);
			if(pSameChildList != NULL)
			{
				HxLOG_Print("[xsvc_epglib_ParseToHumaxServiceList_Tvtv] FreeWholeListItemsFromPointerList Fail!!\n");
				pSameChildList = NULL;
			}

		}

#if defined(TEMP_DBG_RPINT)
		xsvc_epgcontext_PrintTvtvMapInfo(eTVTVMAP_PARSE, pstTempTvtvMap);
#endif

#if defined(TVTV_NEW_ONE)
		{
			DxTvtvService_t				stSvcInfo = {0, };

			HxSTD_memset(&stSvcInfo, 0x00, sizeof(DxTvtvService_t));

			if(xsvc_epgcontext_GetSvcUidFromTripleId(pstTempTvtvMap->usTvtvId, pstTempTvtvMap->stTripleId.usOnId, pstTempTvtvMap->stTripleId.usTsId, pstTempTvtvMap->stTripleId.usSvcId, &pstTempTvtvMap->ulSvcUid) == TRUE)
			{
				stSvcInfo.usTvtvId = pstTempTvtvMap->usTvtvId;
				stSvcInfo.usOnId = pstTempTvtvMap->stTripleId.usOnId;
				stSvcInfo.usTsId = pstTempTvtvMap->stTripleId.usTsId;
				stSvcInfo.usSvcId = pstTempTvtvMap->stTripleId.usSvcId;
				stSvcInfo.ulSvcUid = pstTempTvtvMap->ulSvcUid;
#if 0
				{
					static int cnt = 0;
					HLIB_CMD_Printf("++++ TSMT Send To Dama :  cnt[%d] : uid (%d), tvtvid(%d), onId(0x%04x)(%d), tsid(0x%04x)(%d), svcId(0x%04x)(%d)\n", ++cnt,
						pstTempTvtvMap->ulSvcUid, pstTempTvtvMap->usTvtvId,
						pstTempTvtvMap->stTripleId.usOnId, pstTempTvtvMap->stTripleId.usOnId,
						pstTempTvtvMap->stTripleId.usTsId, pstTempTvtvMap->stTripleId.usTsId,
						pstTempTvtvMap->stTripleId.usSvcId,pstTempTvtvMap->stTripleId.usSvcId);
				}
#endif

				if(DAPI_Set(DxNAMEOF(DxTvtvService_t), stSvcInfo.ulSvcUid, &stSvcInfo, sizeof(DxTvtvService_t)) != ERR_OK)
				{
					HxLOG_Debug("DAPI SET FAIL[xsvc_epgcontext_SvcInfoSendToDama_tvtv]~~~~~~~~~~~~~~~~~~~~~~~~\n");
				}
			}
		}
#endif

		pTvtvMapList = UTIL_LINKEDLIST_AppendListItemToPointerList(pTvtvMapList, (void*)pstTempTvtvMap);
		if(pTvtvMapList == NULL)
		{
			HxLOG_Print("[xsvc_epglib_ParseToHumaxServiceList_Tvtv] AppendListItemToPointerList Fail!!\n");
		}

EXIT_TVTVLIB_TS_LOOP :

		pTsTemp = UTIL_LINKEDLIST_GetNextPointer(pTsTemp);
		if(pTsTemp == NULL || pTsTemp == pTsList)
		{
			HxLOG_Print("[xsvc_epglib_ParseToHumaxServiceList_Tvtv] Loop end !!\n");
			break;
		}
	}

#if defined(TVTV_NEW_ONE)
	DAPI_BatchEnd(DxNAMEOF(DxTvtvService_t));
	DAPI_Sync(DxNAMEOF(DxTvtvService_t));
#endif

	pTsList = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList(pTsList, NULL);
	if(pTsList != NULL)
	{
		HxLOG_Print("[xsvc_epglib_ParseToHumaxServiceList_Tvtv] pTsList Fail!!\n");
		pTsList = NULL;
	}

	hError = PAL_XML_FreeDocument(hXmlDocHandle);
	if(hError != ERR_XML_OK)
	{
		HxLOG_Print("[xsvc_epglib_ParseToHumaxServiceList_Tvtv] FreeDocument Fail!!\n");
	}

	*ppTvtvMapList = pTvtvMapList;

	hError = ERR_TVTV_OK;

EXIT_TVTVLIB_FUNC :

	return hError;

}

HERROR		xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv(HUINT8 *pRawBuffer, HUINT32 ulBufferSize, POINTER_LIST_T **ppEventList)
{
	HERROR					hError = ERR_TVTV_FAIL;
	HXMLDOC				hXmlDocHandle = NULL;
	HXMLELEMENT				hRootElement = NULL;
	HXMLELEMENT				hTempElement1 = NULL;
	HXMLELEMENT				hTempElement2 = NULL;
	HUINT32					ulIndex = 0;
	HUINT8					*pszTempContents= NULL;
	HUINT8					*pszAttrContents= NULL;
	XsvcEpg_TvtvElementList_t 		*pstElementDataList = NULL;
	XsvcEpg_TvtvElement_t			*pstElementData = NULL;

	SvcEpg_Event_t				*pNewEvent = NULL;
	SvcEpg_TvtvEvent_t		*pNewTvtvEvent = NULL;
	SvcEpg_ExtEvent_t			*pNewExtEvent = NULL;
	SvcEpg_ExtText_t			*pEpgText = NULL;

	POINTER_LIST_T			*pElementList = NULL;
	POINTER_LIST_T			*pTempElementList = NULL;
	POINTER_LIST_T			*pProgList = NULL;
	POINTER_LIST_T			*pProgTemp = NULL;
	POINTER_LIST_T			*pEventList = NULL;
//	POINTER_LIST_T			*pExtEventList = NULL;

	HUINT8					szStart[TVTV_TIME_MAX_LEN + 1] = {0, };
	HUINT8					szEnd[TVTV_TIME_MAX_LEN + 1] = {0, };
	HUINT8					szTitle[TVTV_MAX_TITLE + 1] = {0, };				// ?. If over 50, truncated.
	HUINT8					szLtext[TVTV_MAX_LONGTEXT + 1] = {0, };
	///////////////////////////////////////////////////////////////////
	// TVTV DIGITAL BASIC EPG
	// CODE OF PRACTICE V 1.0.1
	//
	// 10.1 TV STATION MAPPING TABLE
	// 10.1.6 LANG : coded according to ISO 639-2
	///////////////////////////////////////////////////////////////////
	HUINT8					*pucISO639Code = NULL;
	HUINT8					szISO639Code[EPG_LANG_ISO_639_LEN + 1] = {0, };
	HUINT8					szLangCode[EPG_LANG_ISO_639_LEN + 1] = {0, };
	UNIXTIME				ulEndTime = 0;
	HUINT32					ulLangCodeLen = 0;
	HUINT8					ucLinkCount = 0;
	HUINT32					ulCategory = 0;
	HUINT8					ucCategoryCount = 0;
	HUINT32					ulProgId = 0;
	HUINT32					ulType = 0;
	HUINT8					ucNameCount = 0;
	HUINT8					*pucElementName = NULL;
	HBOOL					bFound = FALSE;
	HUINT32					ulValue = 0;

	if( pRawBuffer == NULL || ppEventList == NULL || ulBufferSize == 0)
	{
		HxLOG_Print("[xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv] Param NULL!!\n");
		goto EXIT_TVTVLIB_FUNC;
	}
	*ppEventList = NULL;

	hError = PAL_XML_InitParser();
	if(hError != ERR_XML_OK)
	{
		HxLOG_Print("[xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv] XML_InitParser fail !!\n");
		goto EXIT_TVTVLIB_FUNC;
	}

	hError = PAL_XML_ParseBuffer(pRawBuffer, ulBufferSize, &hXmlDocHandle);
	if(hError != ERR_XML_OK)
	{
		HxLOG_Print("[xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv] ParseBuffer fail !!\n");
		goto EXIT_TVTVLIB_FUNC;
	}

	//validation check
#if 0
	hError = PAL_XML_Validate(XML_VALIDATE_BUFFER, NULL, /*dtd buffer variable */, /* buffer size*/, hXmlDocHandle);
	if(hError != ERR_XML_OK)
	{
		HxLOG_Print("[xsvc_epglib_ParseEpg_Tvtv] XML_Validate fail !!\n");
		goto EXIT_TVTVLIB_FUNC;
	}
#endif

	hError = xsvc_epgdata_LoadElementList_Tvtv(TVTV_ELEMENT_LIST_EPG, &pstElementDataList);
	if(hError != ERR_XML_OK)
	{
		HxLOG_Print("[xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv] LoadElementList fail !!\n");
		goto EXIT_TVTVLIB_FUNC;
	}
	pstElementData = pstElementDataList->pstElement;
	if(pstElementData == NULL)
	{
		HxLOG_Print("[xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv] pstElementData NULL !!\n");
		goto EXIT_TVTVLIB_FUNC;
	}

	hError = PAL_XML_GetRootElement(hXmlDocHandle, &hRootElement);
	if(hError != ERR_XML_OK)
	{
		HxLOG_Print("[xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv] GetRootElement fail !!\n");
		goto EXIT_TVTVLIB_FUNC;
	}

	hError = PAL_XML_GetAttrContents(hRootElement, pstElementData[TVTV_EPG_EPG].pucAttrName, (HUINT8 **)&pucISO639Code);
	if(hError == ERR_XML_OK && pucISO639Code != NULL)
	{
		HxLOG_Print("[PAL_XML_GetAttrContents] GetAttrContents fail (%s)!!\n", pstElementData[TVTV_EPG_EPG].pucAttrName);
		MWC_UTIL_DvbStrcpy(szISO639Code, pucISO639Code);

		PAL_XML_FreeAttrContents(pucISO639Code);
	}
	else
	{
		MWC_UTIL_DvbStrcpy(szISO639Code, TVTV_LANG_DEFAULT);
	}
	hError = xsvc_epglib_SelectIso639LanguageEncoding_Tvtv(szISO639Code, szLangCode, &ulLangCodeLen);
	if(hError != ERR_OK)
	{
		HxLOG_Print("[xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv] szLangCode fail (%s)!!\n", pstElementData[TVTV_EPG_EPG].pucAttrName);
	}
#if	0
	HxLOG_Info("[Event] szISO639Code(%c%c%c) szLangCode[%x] szLangCode[%x] szLangCode[%x]!!\n",
																					szISO639Code[0],
																					szISO639Code[1],
																					szISO639Code[2],
																					szLangCode[0],
																					szLangCode[1],
																					szLangCode[2]));
#endif
	hError = PAL_XML_FindChildElementByName(hRootElement, pstElementData[TVTV_EPG_PROG].pucTagName, &pProgList);
	if(hError != ERR_XML_OK)
	{
		HxLOG_Print("[xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv] FindChildElementByName fail !!\n");
		goto EXIT_TVTVLIB_FUNC;
	}
	pProgTemp = pProgList;

#if defined(TVTV_NEW_ONE)
	DAPI_BatchBegin(DxEPG_NAMEOF(eDxEPG_TYPE_TVTV));
#endif

	while(pProgTemp != NULL)
	{
		hTempElement1 = (HXMLELEMENT)UTIL_LINKEDLIST_GetContents(pProgTemp);
		if(hTempElement1 == NULL)
		{
			HxLOG_Print("[xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv] GetContents fail!!\n");
			goto EXIT_TVTVLIB_PROG_LOOP;
		}

		pNewEvent = NULL;
		pNewExtEvent = NULL;

		hError = xsvc_epgdblib_AllocEvent(&pNewEvent);
		if(pNewEvent == NULL || hError != ERR_OK)
		{
			HxLOG_Print("[xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv] Alloc fail!!\n");
			goto EXIT_TVTVLIB_PROG_LOOP;
		}
		pNewEvent->stEventInfo.ucEpgDataType = (HUINT8)EPG_DATA_TYPE_TVTV;
		pNewTvtvEvent = &(pNewEvent->stEventInfo.stTvtvEventInfo);

		pElementList = NULL;
		hError = PAL_XML_GetChildElementList(hTempElement1, &pElementList);
		if(hError != ERR_XML_OK)
		{
			HxLOG_Print("[xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv] PAL_XML_GetChildElementList fail!!\n");
			goto EXIT_TVTVLIB_PROG_LOOP;
		}

		HxSTD_memcpy(pNewTvtvEvent->aucIsoCountryCode, szISO639Code, EPG_LANG_ISO_639_LEN);
/**********************************************************************************************
*
* program count init
*
**********************************************************************************************/
		ucLinkCount = 0;
		ucNameCount = 0;
		ucCategoryCount = 0;

		pTempElementList = pElementList;

		while(pTempElementList)
		{
			hTempElement2 = (HXMLELEMENT)UTIL_LINKEDLIST_GetContents(pTempElementList);
			if(hTempElement2 == NULL)
			{
				HxLOG_Print("[xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv] GetContents fail!!\n");
				goto EXIT_TVTVLIB_ELEMENT_LOOP;
			}

			pucElementName = NULL;
			hError = PAL_XML_GetElementName(hTempElement2, &pucElementName);
			if(hError != ERR_XML_OK || pucElementName == NULL)
			{
				HxLOG_Print("[xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv] PAL_XML_GetElementName fail!!\n");
				goto EXIT_TVTVLIB_ELEMENT_LOOP;
			}

			bFound = FALSE;
			for(ulIndex = TVTV_EPG_PROGID; ulIndex < TVTV_EPG_CAT; ulIndex++)
			{
				if(!strcmp(pstElementData[ulIndex].pucTagName, pucElementName))
				{
					bFound = TRUE;
					break;
				}
			}
			if(!bFound)
			{
				HxLOG_Print("[xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv] !bFound!!\n");
				goto EXIT_TVTVLIB_ELEMENT_LOOP;
			}

			hError = PAL_XML_GetElementContents(hXmlDocHandle, hTempElement2, &pszTempContents);
			if(hError != ERR_XML_OK)
			{
				HxLOG_Print("[xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv] PAL_XML_GetElementContents fail!!\n");
				goto EXIT_TVTVLIB_ELEMENT_LOOP;
			}

			switch(ulIndex)
			{
/**********************************************************************************************
*
* event info area
*
**********************************************************************************************/
				case TVTV_EPG_PROGID :
					hError = xsvc_epglib_SetDataByType_Tvtv(	pstElementData[ulIndex].ulTagDataType,
														pszTempContents,
														NULL, 0, 0, NULL,
														&ulValue);
					if(hError != ERR_TVTV_OK)
					{
						HxLOG_Print("[xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv] SetDataByType ulProgId fail!!\n");
					}
					pNewTvtvEvent->ulProgId = ulValue;
					break;

				case TVTV_EPG_TVTVID :
					hError = xsvc_epglib_SetDataByType_Tvtv(	pstElementData[ulIndex].ulTagDataType,
														pszTempContents,
														NULL, 0, 0, NULL,
														&ulValue);
					if(hError != ERR_TVTV_OK)
					{
						HxLOG_Print("[xsvc_epglib_ParseEpg_Tvtv] SetDataByType usTvtvId fail!!\n");
					}
					pNewTvtvEvent->usTvtvId = (HUINT16)ulValue;
					break;

				case TVTV_EPG_START :
					HxSTD_memset(szStart, 0x00, sizeof(HUINT8)*(TVTV_TIME_MAX_LEN+1));
					hError = xsvc_epglib_SetDataByType_Tvtv(	pstElementData[ulIndex].ulTagDataType,
														pszTempContents,
														NULL, 0,
														TVTV_TIME_MAX_LEN + 1,
														szStart,
														NULL);
					if(hError != ERR_TVTV_OK)
					{
						HxLOG_Print("[xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv] SetDataByType szStart fail!!\n");
					}

					hError = xsvc_epglib_ConvertStringTimeToUnixTime_Tvtv(szStart, (UNIXTIME*)&pNewEvent->ulStartTime);
					if(hError != ERR_TVTV_OK)
					{
						HxLOG_Print("[xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv] SetDataByType ulStart fail!!\n");
					}
					break;

				case TVTV_EPG_END :
					HxSTD_memset(szEnd, 0x00, sizeof(HUINT8)*(TVTV_TIME_MAX_LEN+1));
					hError = xsvc_epglib_SetDataByType_Tvtv(	pstElementData[ulIndex].ulTagDataType,
														pszTempContents,
														NULL, 0,
														TVTV_TIME_MAX_LEN + 1,
														szEnd,
														NULL);
					if(hError != ERR_TVTV_OK)
					{
						HxLOG_Print("[xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv] SetDataByType szEnd fail!!\n");
					}
					ulEndTime = 0;
					hError = xsvc_epglib_ConvertStringTimeToUnixTime_Tvtv(szEnd, (UNIXTIME*)&ulEndTime);
					if(hError != ERR_TVTV_OK)
					{
						HxLOG_Print("[xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv] SetDataByType ulEnd fail!!\n");
					}
					pNewEvent->ulDuration = ulEndTime - pNewEvent->ulStartTime;
					break;

				case TVTV_EPG_TITLE :
					szTitle[0] = (HUINT8)0;
					hError = xsvc_epglib_SetDataByType_Tvtv(	pstElementData[ulIndex].ulTagDataType,
														pszTempContents,
														szLangCode,
														ulLangCodeLen,
														TVTV_MAX_TITLE + 1,
														szTitle,
														NULL);
					if(hError != ERR_TVTV_OK)
					{
						HxLOG_Print("[xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv] SetDataByType szTitle fail!!\n");
					}
					pEpgText = NULL;

					hError = xsvc_epgdblib_MakeEpgTextList(szTitle, pNewEvent->astShortText[0].pEventName, &pEpgText);
					if(hError != ERR_OK)
					{
						HxLOG_Print("[xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv] xsvc_epgdblib_MakeEpgTextList fail!!\n");
					}
					HxSTD_memcpy(pNewEvent->astShortText[0].aucIsoCountryCode, szISO639Code, EPG_LANG_ISO_639_LEN);
					pNewEvent->astShortText[0].pEventName = pEpgText;

					pNewTvtvEvent->usEventInfoMask |= eEPG_DES_SHORT_EVENT;
					break;

				case TVTV_EPG_LTEXT :
					szLtext[0] = (HUINT8)0;
					hError = xsvc_epglib_SetDataByType_Tvtv(	pstElementData[ulIndex].ulTagDataType,
														pszTempContents,
														szLangCode,
														ulLangCodeLen,
														TVTV_MAX_LONGTEXT + 1,
														szLtext,
														NULL);
					if(hError != ERR_TVTV_OK)
					{
						HxLOG_Print("[xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv] SetDataByType szLtext fail!!\n");
					}
					pEpgText = NULL;

					hError = xsvc_epgdblib_MakeEpgTextList(szLtext, pNewEvent->astShortText[0].pEventText, &pEpgText);
					if(hError != ERR_OK)
					{
						HxLOG_Print("[xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv] xsvc_epgdblib_MakeEpgTextList fail!!\n");
					}
					HxSTD_memcpy(pNewEvent->astShortText[0].aucIsoCountryCode, szISO639Code, EPG_LANG_ISO_639_LEN);
					pNewEvent->astShortText[0].pEventText = pEpgText;

					pNewTvtvEvent->usEventInfoMask |= eEPG_DES_SHORT_EVENT;
					break;

				case TVTV_EPG_SID :
					hError = xsvc_epglib_SetDataByType_Tvtv(	pstElementData[ulIndex].ulTagDataType,
														pszTempContents,
														NULL, 0, 0, NULL,
														&ulValue);
					if(hError != ERR_TVTV_OK)
					{
						HxLOG_Print("[xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv] SetDataByType ulSId fail!!\n");
					}
					pNewTvtvEvent->ulSeriesId = ulValue;
					break;

				case TVTV_EPG_SNR :
					hError = xsvc_epglib_SetDataByType_Tvtv(	pstElementData[ulIndex].ulTagDataType,
														pszTempContents,
														NULL, 0,
														TVTV_MAX_SNR + 1,
														pNewTvtvEvent->aucSeriesNumber,
														NULL);
					if(hError != ERR_TVTV_OK)
					{
						HxLOG_Print("[xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv] SetDataByType szSnr fail!!\n");
					}
					break;

				case TVTV_EPG_FSK :
					hError = xsvc_epglib_SetDataByType_Tvtv(	pstElementData[ulIndex].ulTagDataType,
														pszTempContents,
														NULL, 0, 0, NULL,
														&ulValue);
					if(hError != ERR_TVTV_OK)
					{
						HxLOG_Print("[xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv] SetDataByType ucFsk fail!!\n");
					}
					// for DVB spec
					HxSTD_memcpy(pNewTvtvEvent->astParentalRating[0].aucIsoCountryCode, szISO639Code, sizeof(HUINT8)*(EPG_LANG_ISO_639_LEN+1));
					pNewTvtvEvent->astParentalRating[0].ucParentalRating = (HUINT8)ulValue - 3;
					break;

				case TVTV_EPG_CONTENT :
					if(pNewEvent->aucContent[0] != 0)
					{
						break;
					}
					hError = xsvc_epglib_SetDataByType_Tvtv(	pstElementData[ulIndex].ulTagDataType,
														pszTempContents,
														NULL, 0, 0, NULL,
														(HUINT32*)&pNewEvent->aucContent[0]);
					if(hError != ERR_TVTV_OK)
					{
						HxLOG_Print("[xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv] SetDataByType ucContent fail!!\n");
					}
#if					0
					{
						HUINT8	ucCode = 0;
						ucCode = pNewEvent->ucContent & 0x4f;
						if(ucCode & 0x40)
						{
							HxLOG_Info("[xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv] TVTV_EPG_CONTENT (%s)!!\n",
													pNewEvent->astShortText[0].pEventName->aucTextChar));
						}
					}
#endif
					break;

				case TVTV_EPG_IMAGE :
					hError = xsvc_epglib_SetDataByType_Tvtv(	pstElementData[ulIndex].ulTagDataType,
														pszTempContents,
														NULL, 0,
														TVTV_IMG_MAX_FILENAME + 1,
														pNewTvtvEvent->aucImageName,
														NULL);
					if(hError != ERR_TVTV_OK)
					{
						HxLOG_Print("[xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv] SetDataByType szImage fail!!\n");
					}
					pNewTvtvEvent->ucIsImage = 1;
					break;

				case TVTV_EPG_VER :
					hError = xsvc_epglib_SetDataByType_Tvtv(	pstElementData[ulIndex].ulTagDataType,
														pszTempContents,
														NULL, 0, 0, NULL,
														&ulValue);
					if(hError != ERR_TVTV_OK)
					{
						HxLOG_Print("[xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv] SetDataByType usVer fail!!\n");
					}
					pNewTvtvEvent->usVersion = (HUINT16)ulValue;
					break;

				case TVTV_EPG_LINK :
					if(ucLinkCount >= EPG_TVTV_LINK_MAX)
					{
						break;
					}
					ulProgId = 0;
					hError = xsvc_epglib_SetDataByType_Tvtv(	pstElementData[ulIndex].ulTagDataType,
														pszTempContents,
														NULL, 0, 0, NULL,
														(HUINT32*)&ulProgId);
					if(hError != ERR_TVTV_OK)
					{
						HxLOG_Print("[xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv] SetDataByType ulProgId fail!!\n");
						break;
					}

					hError = PAL_XML_GetAttrContents(hTempElement2, pstElementData[ulIndex].pucAttrName, &pszAttrContents);
					if(hError != ERR_XML_OK)
					{
						HxLOG_Print("[xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv] pstTempLink GetAttrContents fail!!\n");
						break;
					}
					ulType = 0;
					hError = xsvc_epglib_SetDataByType_Tvtv(	pstElementData[ulIndex].ulAttrDataType,
														pszAttrContents,
														NULL, 0, 0, NULL,
														(HUINT32*)&ulType);
					if(hError != ERR_TVTV_OK)
					{
						HxLOG_Print("[xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv] SetDataByType pstTempLink ulType fail!!\n");
						PAL_XML_FreeAttrContents(pszAttrContents);
						pszAttrContents = NULL;
						break;
					}
					pNewTvtvEvent->astEventLink[ucLinkCount].ulProgId = ulProgId;
					pNewTvtvEvent->astEventLink[ucLinkCount].ucType = (HUINT8)ulType;
					ucLinkCount++;
					/** TVTV EPG SYSTEM SPEC V 2.4.2,  5.3.2.21 Link type table*/
					if(ulType == 2)
					{
						pNewTvtvEvent->usEventInfoMask |= eEPG_DES_SERIESLINK;
					}
					else if(ulType == 1)
					{
						pNewTvtvEvent->usEventInfoMask |= eEPG_DES_SAMELINK;
					}

					PAL_XML_FreeAttrContents(pszAttrContents);
					pszAttrContents = NULL;
					break;

				case TVTV_EPG_TIP :
					if(pNewTvtvEvent->ucTip != 0)
					{
						break;
					}
					hError = xsvc_epglib_SetDataByType_Tvtv(	pstElementData[ulIndex].ulTagDataType,
														pszTempContents,
														NULL, 0, 0, NULL,
														&ulValue);
					if(hError != ERR_TVTV_OK)
					{
						HxLOG_Print("[xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv] SetDataByType ucTip fail!!\n");
					}
					pNewTvtvEvent->ucTip = (HUINT8)ulValue;
					{
						HxDATETIME_t		stDateTime = {{0, }, };

						HLIB_DATETIME_ConvertUnixTimeToDateTime(pNewEvent->ulStartTime, &stDateTime);

						HxLOG_Info("[xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv] TVTV_EPG_TIP usTvtvId(%d)ulProgId(%d)ucTip(%d)(%02d.%02d  %02d:%02d)!!\n",
																					pNewTvtvEvent->usTvtvId,
																					pNewTvtvEvent->ulProgId,
																					pNewTvtvEvent->ucTip,
																					stDateTime.stDate.ucMonth,
																					stDateTime.stDate.ucDay,
																					stDateTime.stTime.ucHour,
																					stDateTime.stTime.ucMinute);
					}
					break;

				case TVTV_EPG_EVENTID :
					hError = xsvc_epglib_SetDataByType_Tvtv(	pstElementData[ulIndex].ulTagDataType,
														pszTempContents,
														NULL, 0, 0, NULL,
														&ulValue);
					if(hError != ERR_TVTV_OK)
					{
						HxLOG_Print("[xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv] SetDataByType nEventId fail!!\n");
					}
					pNewTvtvEvent->nEventId = ulValue;
					break;

				case TVTV_EPG_CAT :
					if(ucCategoryCount >= EPG_TVTV_CATEGORY_LEN)
					{
						break;
					}
					ulCategory = 0;
					hError = xsvc_epglib_SetDataByType_Tvtv(	pstElementData[ulIndex].ulTagDataType,
														pszTempContents,
														NULL, 0, 0, NULL,
														(HUINT32*)&ulCategory);
					if(hError != ERR_TVTV_OK)
					{
						HxLOG_Print("[xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv] SetDataByType usCat fail!!\n");
						break;
					}
					pNewTvtvEvent->ausCategory[ucCategoryCount] = (HUINT16)ulCategory;
					ucCategoryCount++;
					break;
				case TVTV_EPG_SEASON :
					hError = xsvc_epglib_SetDataByType_Tvtv(	pstElementData[ulIndex].ulTagDataType,
														pszTempContents,
														NULL, 0, 0, NULL,
														&ulValue);
					if(hError != ERR_TVTV_OK)
					{
						HxLOG_Print("[xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv] SetDataByType nEventId fail!!\n");
					}
					pNewTvtvEvent->ulSeason = ulValue;
					break;
				case TVTV_EPG_SCT :
					hError = xsvc_epglib_SetDataByType_Tvtv(	pstElementData[ulIndex].ulTagDataType,
														pszTempContents,
														NULL, 0, 0, NULL,
														&ulValue);
					if(hError != ERR_TVTV_OK)
					{
						HxLOG_Print("[xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv] SetDataByType nEventId fail!!\n");
					}
					pNewTvtvEvent->ulTotalNum = ulValue;
					break;

/**********************************************************************************************
*
* extended info area
*
**********************************************************************************************/
				case TVTV_EPG_NAME :
				case TVTV_EPG_NAME_EXT :
				case TVTV_EPG_YEAR :
				case TVTV_EPG_DTEXT :
				case TVTV_EPG_COUNTRY :
					pNewTvtvEvent->usEventInfoMask |= eEPG_DES_EXTENDED_EVENT;
					break;

#if				0
/**********************************************************************************************
*
* skip info area
*
**********************************************************************************************/
				case TVTV_EPG_PIL:
				case TVTV_EPG_OTITLE :
				case TVTV_EPG_SCT :
				case TVTV_EPG_FORMAT :
				case TVTV_EPG_SUBTITLE :
				case TVTV_EPG_AUDIO :
				case TVTV_EPG_VIDEO :
					HxLOG_Print("[xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv] skip element type(%d)!!\n", ulIndex);
					break;
#endif
				default :
					break;
			}

			PAL_XML_FreeAttrContents(pszTempContents);
			pszTempContents = NULL;
EXIT_TVTVLIB_ELEMENT_LOOP :
			pTempElementList = UTIL_LINKEDLIST_GetNextPointer(pTempElementList);
			if(pTempElementList == NULL || pElementList == pTempElementList)
			{
				break;
			}
		}

#if defined(TVTV_NEW_ONE)
		if(pNewEvent)
		{
			xsvc_epgcontext_SendToDamaEventIfno(pNewEvent);
		}
#endif

EXIT_TVTVLIB_PROG_LOOP :
		pEventList = UTIL_LINKEDLIST_AppendListItemToPointerList(pEventList, (void*)pNewEvent);
		if(pEventList == NULL)
		{
			HxLOG_Print("[xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv] AppendListItemToPointerList Fail!!\n");
		}
		pProgTemp = UTIL_LINKEDLIST_GetNextPointer(pProgTemp);
		if(pProgTemp == NULL || pProgTemp == pProgList)
		{
			HxLOG_Print("[xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv] Loop end !!\n");
			break;
		}
		pElementList = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList(pElementList, NULL);
		if(pElementList != NULL)
		{
			HxLOG_Print("[xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv] pElementList Fail!!\n");
			pElementList = NULL;
		}
	}

#if defined(TVTV_NEW_ONE)
	DAPI_BatchEnd(DxEPG_NAMEOF(eDxEPG_TYPE_TVTV));
	HLIB_STD_TaskSleep(1000);
#endif


	pProgList = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList(pProgList, NULL);
	if(pProgList != NULL)
	{
		HxLOG_Print("[xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv] pProgList Fail!!\n");
		pProgList = NULL;
	}

	hError = PAL_XML_FreeDocument(hXmlDocHandle);
	if(hError != ERR_XML_OK)
	{
		HxLOG_Print("[xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv] FreeDocument Fail!!\n");
	}

	*ppEventList = pEventList;

	hError = ERR_TVTV_OK;

EXIT_TVTVLIB_FUNC :

	return hError;

}
#endif

HERROR		xsvc_epglib_GetFileStartTime_Tvtv(HUINT8 *pRawBuffer, HUINT32 ulBufferSize, HUINT32 *pulStartTime)
{
	HERROR					hError = ERR_TVTV_FAIL;
	HXMLDOC				hXmlDocHandle = NULL;
	HXMLELEMENT				hRootElement = NULL;
	HXMLELEMENT				hTempElement1 = NULL;
	HXMLELEMENT				hTempElement2 = NULL;
	HUINT8					*pszTempContents= NULL;
	XsvcEpg_TvtvElementList_t 		*pstElementList = NULL;
	XsvcEpg_TvtvElement_t			*pstElement = NULL;


	POINTER_LIST_T			*pSameChildList = NULL;
	POINTER_LIST_T			*pProgList = NULL;
	POINTER_LIST_T			*pProgTemp = NULL;

	HUINT8					szStart[TVTV_TIME_MAX_LEN + 1] = {0, };
	UNIXTIME				ulStartTime = 0;
	HBOOL					bFound = FALSE;


	if( pRawBuffer == NULL  || ulBufferSize == 0 || pulStartTime == NULL)
	{
		HxLOG_Print("[xsvc_epglib_GetFileStartTime_Tvtv] Param NULL!!\n");
		goto EXIT_TVTVLIB_FUNC;
	}
	*pulStartTime = 0;

	hError = PAL_XML_InitParser();
	if(hError != ERR_XML_OK)
	{
		HxLOG_Print("[xsvc_epglib_GetFileStartTime_Tvtv] XML_InitParser fail !!\n");
		goto EXIT_TVTVLIB_FUNC;
	}

	hError = PAL_XML_ParseBuffer(pRawBuffer, ulBufferSize, &hXmlDocHandle);
	if(hError != ERR_XML_OK)
	{
		HxLOG_Print("[xsvc_epglib_GetFileStartTime_Tvtv] ParseBuffer fail !!\n");
		goto EXIT_TVTVLIB_FUNC;
	}

	//validation check
#if 0
	hError = PAL_XML_Validate(XML_VALIDATE_BUFFER, NULL, /*dtd buffer variable */, /* buffer size*/, hXmlDocHandle);
	if(hError != ERR_XML_OK)
	{
		HxLOG_Print("[xsvc_epglib_ParseEpg_Tvtv] XML_Validate fail !!\n");
		goto EXIT_TVTVLIB_FUNC;
	}
#endif

	hError = xsvc_epgdata_LoadElementList_Tvtv(TVTV_ELEMENT_LIST_EPG, &pstElementList);
	if(hError != ERR_XML_OK)
	{
		HxLOG_Print("[xsvc_epglib_GetFileStartTime_Tvtv] LoadElementList fail !!\n");
		goto EXIT_TVTVLIB_FUNC;
	}
	pstElement = pstElementList->pstElement;
	if(pstElement == NULL)
	{
		HxLOG_Print("[xsvc_epglib_GetFileStartTime_Tvtv] pstElement NULL !!\n");
		goto EXIT_TVTVLIB_FUNC;
	}

	hError = PAL_XML_GetRootElement(hXmlDocHandle, &hRootElement);
	if(hError != ERR_XML_OK)
	{
		HxLOG_Print("[xsvc_epglib_GetFileStartTime_Tvtv] GetRootElement fail !!\n");
		goto EXIT_TVTVLIB_FUNC;
	}

	hError = PAL_XML_FindChildElementByName(hRootElement, pstElement[TVTV_EPG_PROG].pucTagName, &pProgList);
	if(hError != ERR_XML_OK)
	{
		HxLOG_Print("[xsvc_epglib_GetFileStartTime_Tvtv] FindChildElementByName fail !!\n");
		goto EXIT_TVTVLIB_FUNC;
	}
	pProgTemp = pProgList;
	while(pProgTemp != NULL)
	{
		hTempElement1 = (HXMLELEMENT)UTIL_LINKEDLIST_GetContents(pProgTemp);
		if(hTempElement1 == NULL)
		{
			HxLOG_Print("[xsvc_epglib_GetFileStartTime_Tvtv] GetContents fail!!\n");
			goto EXIT_TVTVLIB_PROG_LOOP;
		}

		hError = PAL_XML_FindChildElementByName(hTempElement1, pstElement[TVTV_EPG_START].pucTagName, &pSameChildList);
		if(hError != ERR_XML_OK)
		{
			HxLOG_Print("[xsvc_epglib_GetFileStartTime_Tvtv] FindChildElementByName fail (%s)!!\n", pstElement[TVTV_EPG_START].pucTagName);
			goto EXIT_TVTVLIB_PROG_LOOP;
		}

		hTempElement2 = (HXMLELEMENT)UTIL_LINKEDLIST_GetContents(pSameChildList);
		if(hTempElement2 == NULL)
		{
			HxLOG_Print("[xsvc_epglib_GetFileStartTime_Tvtv] GetContents fail (%s)!!\n", pstElement[TVTV_EPG_START].pucTagName);
			goto EXIT_TVTVLIB_PROG_LOOP;
		}
		hError = PAL_XML_GetElementContents(hXmlDocHandle, hTempElement2, &pszTempContents);
		if(hError != ERR_XML_OK)
		{
			HxLOG_Print("[xsvc_epglib_GetFileStartTime_Tvtv] GetElementContents fail (%s)!!\n", pstElement[TVTV_EPG_START].pucTagName);
			goto EXIT_TVTVLIB_PROG_LOOP;
		}


		HxSTD_memset(szStart, 0x00, sizeof(HUINT8)*(TVTV_TIME_MAX_LEN+1));
		hError = xsvc_epglib_SetDataByType_Tvtv(	pstElement[TVTV_EPG_START].ulTagDataType,
											pszTempContents,
											NULL, 0,
											TVTV_TIME_MAX_LEN + 1,
											szStart,
											NULL);
		if(hError != ERR_TVTV_OK)
		{
			HxLOG_Print("[xsvc_epglib_GetFileStartTime_Tvtv] SetDataByType szStart fail!!\n");
			goto EXIT_TVTVLIB_PROG_LOOP;
		}

		hError = xsvc_epglib_ConvertStringTimeToUnixTime_Tvtv(szStart, (UNIXTIME*)&ulStartTime);
		if(hError != ERR_TVTV_OK)
		{
			HxLOG_Print("[xsvc_epglib_GetFileStartTime_Tvtv] SetDataByType ulStart fail!!\n");
			goto EXIT_TVTVLIB_PROG_LOOP;
		}
		bFound = TRUE;

EXIT_TVTVLIB_PROG_LOOP :

		pSameChildList = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList(pSameChildList, NULL);
		if(pSameChildList != NULL)
		{
			HxLOG_Print("[xsvc_epglib_GetFileStartTime_Tvtv] FreeWholeListItemsFromPointerList Fail!!\n");
			pSameChildList = NULL;
		}

		pProgTemp = UTIL_LINKEDLIST_GetNextPointer(pProgTemp);
		if(pProgTemp == NULL || pProgTemp == pProgList)
		{
			HxLOG_Print("[xsvc_epglib_GetFileStartTime_Tvtv] Loop end !!\n");
			break;
		}
		if(bFound == TRUE)
		{
			*pulStartTime = ulStartTime;
			break;
		}
	}

	pProgList = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList(pProgList, NULL);
	if(pProgList != NULL)
	{
		HxLOG_Print("[xsvc_epglib_GetFileStartTime_Tvtv] pProgList Fail!!\n");
		pProgList = NULL;
	}

	hError = PAL_XML_FreeDocument(hXmlDocHandle);
	if(hError != ERR_XML_OK)
	{
		HxLOG_Print("[xsvc_epglib_GetFileStartTime_Tvtv] FreeDocument Fail!!\n");
	}

	hError = ERR_TVTV_OK;

EXIT_TVTVLIB_FUNC :

	return hError;

}






#define	______TVTV_FILTER_FUNC______

STATIC HBOOL		xsvc_epglib_FilterDgktByTvSid_Tvtv(void *pvItem, void *pvFilter)
{
	XsvcEpg_TvtvDgkt_t					*pTempDgkt = (XsvcEpg_TvtvDgkt_t*)pvItem;
	HUINT16 						usTvSid = *(HUINT16 *)pvFilter;

	if(pTempDgkt == NULL)
	{
		return FALSE;
	}

	if (pTempDgkt->usTvSid == usTvSid)
	{
		return TRUE;
	}
	return FALSE;
}

#define	______TVTV_FREE_FUNC______

STATIC void		xsvc_epglib_FreeTvstInfo_Tvtv(void* pvItem)
{
	XsvcEpg_TvtvServiceTable_t			*pTvstInfo = (XsvcEpg_TvtvServiceTable_t*)pvItem;

	if(pTvstInfo == NULL)
	{
		return;
	}

	if(pTvstInfo->pszName != NULL)
	{
		OxEPG_Free(pTvstInfo->pszName);
	}

	OxEPG_Free(pTvstInfo);

	return;
}



STATIC void		xsvc_epglib_FreeDgktInfo_Tvtv(void* pvItem)
{
	XsvcEpg_TvtvDgkt_t			*pDgktInfo = (XsvcEpg_TvtvDgkt_t*)pvItem;

	if(pDgktInfo == NULL)
	{
		return;
	}

	OxEPG_Free(pDgktInfo);

	return;
}


STATIC void		xsvc_epglib_FreeTsmtInfo_Tvtv(void* pvItem)
{
	XsvcEpg_TvtvTsmt_t			*pTsmtInfo = (XsvcEpg_TvtvTsmt_t*)pvItem;

	if(pTsmtInfo == NULL)
	{
		return;
	}

	if(pTsmtInfo->pstLogoInfo != NULL)
	{
		OxEPG_Free(pTsmtInfo->pstLogoInfo);
	}

	OxEPG_Free(pTsmtInfo);

	return;
}



STATIC void		xsvc_epglib_FreeLtotInfo_Tvtv(void* pvItem)
{
	XsvcEpg_TvtvLtot_t			*pLtotInfo = (XsvcEpg_TvtvLtot_t*)pvItem;

	if(pLtotInfo == NULL)
	{
		return;
	}

	OxEPG_Free(pLtotInfo);

	return;
}



STATIC void		xsvc_epglib_FreeEpgDataInfo_Tvtv(void* pvItem)
{
	XsvcEpg_TvtvEpgData_t			*pEpgInfo = (XsvcEpg_TvtvEpgData_t*)pvItem;

	if(pEpgInfo == NULL)
	{
		return;
	}

	if(pEpgInfo->pcOtitle != NULL)
	{
		OxEPG_Free(pEpgInfo->pcOtitle);
	}

	if(pEpgInfo->pcDtext != NULL)
	{
		OxEPG_Free(pEpgInfo->pcDtext);
	}

	if(pEpgInfo->pstName != NULL)
	{
		XsvcEpg_TvtvName_t	*pCurTemp = NULL;
		XsvcEpg_TvtvName_t	*pNextTemp = NULL;

		pCurTemp = pEpgInfo->pstName;
		while(pCurTemp)
		{
			pNextTemp = pCurTemp->pstNext;
			OxEPG_Free(pCurTemp);

			pCurTemp = pNextTemp;
		}
	}

	if(pEpgInfo->pstContent != NULL)
	{
		XsvcEpg_TvtvContent_t	*pCurTemp = NULL;
		XsvcEpg_TvtvContent_t	*pNextTemp = NULL;

		pCurTemp = pEpgInfo->pstContent;
		while(pCurTemp)
		{
			pNextTemp = pCurTemp->pstNext;
			OxEPG_Free(pCurTemp);

			pCurTemp = pNextTemp;
		}
	}

	if(pEpgInfo->pstImageInfo != NULL)
	{
		OxEPG_Free(pEpgInfo->pstImageInfo);
	}

	if(pEpgInfo->pstLinkInfo != NULL)
	{
		XsvcEpg_TvtvLink_t	*pCurTemp = NULL;
		XsvcEpg_TvtvLink_t	*pNextTemp = NULL;

		pCurTemp = pEpgInfo->pstLinkInfo;
		while(pCurTemp)
		{
			pNextTemp = pCurTemp->pstNext;
			OxEPG_Free(pCurTemp);

			pCurTemp = pNextTemp;
		}
	}

	if(pEpgInfo->pstSubtitle != NULL)
	{
		XsvcEpg_TvtvSubtitle_t	*pCurTemp = NULL;
		XsvcEpg_TvtvSubtitle_t	*pNextTemp = NULL;

		pCurTemp = pEpgInfo->pstSubtitle;
		while(pCurTemp)
		{
			pNextTemp = pCurTemp->pstNext;
			OxEPG_Free(pCurTemp);

			pCurTemp = pNextTemp;
		}
	}

	if(pEpgInfo->pstAudio != NULL)
	{
		XsvcEpg_TvtvAudio_t	*pCurTemp = NULL;
		XsvcEpg_TvtvAudio_t	*pNextTemp = NULL;

		pCurTemp = pEpgInfo->pstAudio;
		while(pCurTemp)
		{
			pNextTemp = pCurTemp->pstNext;
			OxEPG_Free(pCurTemp);

			pCurTemp = pNextTemp;
		}
	}

	if(pEpgInfo->pstVideo != NULL)
	{
		XsvcEpg_TvtvVideo_t	*pCurTemp = NULL;
		XsvcEpg_TvtvVideo_t	*pNextTemp = NULL;

		pCurTemp = pEpgInfo->pstVideo;
		while(pCurTemp)
		{
			pNextTemp = pCurTemp->pstNext;
			OxEPG_Free(pCurTemp);

			pCurTemp = pNextTemp;
		}
	}

	if(pEpgInfo->pstTip != NULL)
	{
		XsvcEpg_TvtvTip_t	*pCurTemp = NULL;
		XsvcEpg_TvtvTip_t	*pNextTemp = NULL;

		pCurTemp = pEpgInfo->pstTip;
		while(pCurTemp)
		{
			pNextTemp = pCurTemp->pstNext;
			OxEPG_Free(pCurTemp);

			pCurTemp = pNextTemp;
		}
	}

	if(pEpgInfo->pstCat != NULL)
	{
		XsvcEpg_TvtvCat_t	*pCurTemp = NULL;
		XsvcEpg_TvtvCat_t	*pNextTemp = NULL;

		pCurTemp = pEpgInfo->pstCat;
		while(pCurTemp)
		{
			pNextTemp = pCurTemp->pstNext;
			OxEPG_Free(pCurTemp);

			pCurTemp = pNextTemp;
		}
	}

	if(pEpgInfo->pstNameExt != NULL)
	{

		XsvcEpg_TvtvName_t	*pCurTemp = NULL;
		XsvcEpg_TvtvName_t	*pNextTemp = NULL;

		pCurTemp = pEpgInfo->pstNameExt;
		while(pCurTemp)
		{
			pNextTemp = pCurTemp->pstNext;
			OxEPG_Free(pCurTemp);

			pCurTemp = pNextTemp;
		}
	}

	OxEPG_Free(pEpgInfo);

	return;
}

#if 1	// DB

STATIC HERROR		local_epgdb_fopen(HUINT8 *pszFileName, Handle_t *phFileHandle);
STATIC HERROR		local_epgdb_fread(void *pBuffer, HUINT32 ulUnitSize, Handle_t hFileHandle, PalPb_SeekPosition_e eSeekMode, HUINT32 *pulReadSize);
STATIC HERROR		local_epgdb_fseek(Handle_t hFileHandle, HUINT32 ulFileOffset, PalPb_SeekPosition_e eSeekMode);
STATIC HERROR		local_epgdb_currentpos_fread(void *pBuffer, HUINT32 ulUnitSize, Handle_t hFileHandle, HUINT32 *pulReadSize);
STATIC HERROR		local_epgdb_fclose(Handle_t hFileHandle);
STATIC HERROR		local_epgdb_filelength(HUINT8 *pucFileName, HUINT32 *pulFileLength);
STATIC HERROR		local_epgdb_ReadFileToBuffer(Handle_t hFileHandle, HUINT8 **ppBuffer, HUINT32 ulReadSize, HUINT32 *pulReadSize);


STATIC HERROR		local_epgdb_fopen(HUINT8 *pszFileName, Handle_t *phFileHandle)
{
	HERROR			hError = ERR_FAIL;
	HxFILE_t	hFile = NULL;

	if(pszFileName == NULL || phFileHandle == NULL)
	{
		HxLOG_Error("[local_epgdb_fopen] Param NULL!!\n");
		goto EXIT_EPGDB_FUNC;
	}

	*phFileHandle = (Handle_t)NULL;

	if (HLIB_FILE_Exist(pszFileName) == TRUE)
	{
		hFile = HLIB_FILE_Open(pszFileName, "r+");
	}
	else
	{
		hFile = HLIB_FILE_Open(pszFileName, "w+");
	}
	if (hFile == NULL)
	{
		HxLOG_Error("[local_epgdb_fopen] HLIB_FILE_Open fail!!\n");
		goto EXIT_EPGDB_FUNC;
	}
	*phFileHandle =(Handle_t)hFile ;
	hError = ERR_OK;

EXIT_EPGDB_FUNC :

	return hError;

}


STATIC HERROR		local_epgdb_fread(void *pBuffer, HUINT32 ulUnitSize, Handle_t hFileHandle, PalPb_SeekPosition_e eSeekMode, HUINT32 *pulReadSize)
{
	HERROR			hError = ERR_FAIL;
	HUINT32 		ulReadSize;
	HxFILE_t	hFile = NULL;
	HxFILE_Seek_e	eMode;
	HUINT64 	ullPosOffset;

	if(pBuffer == NULL || hFileHandle == (Handle_t)NULL || pulReadSize == NULL)
	{
		HxLOG_Error("[local_epgdb_fread] Param NULL!!\n");
		goto EXIT_EPGDB_FUNC;
	}
	*pulReadSize = 0;

	hFile = (HxFILE_t)hFileHandle;

	switch(eSeekMode)
	{
		case	ePAL_PB_SEEKPOSITION_START:
			eMode = eHxFILE_SEEK_SET;
			break;

		case	ePAL_PB_SEEKPOSITION_CURRENT:
			eMode = eHxFILE_SEEK_CUR;
			break;

		case	ePAL_PB_SEEKPOSITION_END:
			eMode = eHxFILE_SEEK_END;
			break;

		default:
			eMode = eHxFILE_SEEK_SET;
			break;
	}

	hError = HLIB_FILE_Seek(hFile, 0, eMode, &ullPosOffset);
	if (hError != ERR_OK )
	{
		HxLOG_Error("[local_epgdb_fread] HLIB_FILE_Seek fail!!\n");
		goto EXIT_EPGDB_FUNC;
	}

	ulReadSize = HLIB_FILE_Read(hFile, (void *)pBuffer, 1, ulUnitSize);
	if (ulReadSize <= 0)
	{
		HxLOG_Error("[local_epgdb_fread] HLIB_FILE_Read fail!!\n");
		goto EXIT_EPGDB_FUNC;
	}

	*pulReadSize = ulReadSize;

	hError = ERR_OK;

EXIT_EPGDB_FUNC :

	return hError;

}


STATIC HERROR		local_epgdb_fseek(Handle_t hFileHandle, HUINT32 ulFileOffset, PalPb_SeekPosition_e eSeekMode)
{
	HERROR			hError = ERR_FAIL;
	HxFILE_t	hFile = NULL;
	HxFILE_Seek_e	eMode;
	HUINT64 	ullPosOffset;

	hFile = (HxFILE_t)hFileHandle;

	if(hFileHandle == (Handle_t)NULL)
	{
		HxLOG_Print("[local_epgdb_fseek] Param NULL!!\n");
		goto EXIT_EPGDB_FUNC;
	}

	switch(eSeekMode)
	{
		case	ePAL_PB_SEEKPOSITION_START:
			eMode = eHxFILE_SEEK_SET;
			break;

		case	ePAL_PB_SEEKPOSITION_CURRENT:
			eMode = eHxFILE_SEEK_CUR;
			break;

		case	ePAL_PB_SEEKPOSITION_END:
			eMode = eHxFILE_SEEK_END;
			break;

		default:
			eMode = eHxFILE_SEEK_SET;
			break;
	}

	hError = HLIB_FILE_Seek(hFile, ulFileOffset, eMode, &ullPosOffset);
	if (hError != ERR_OK )
	{
		HxLOG_Error("[local_epgdb_fread] HLIB_FILE_Seek fail!!\n");
		goto EXIT_EPGDB_FUNC;
	}

	hError = ERR_OK;

EXIT_EPGDB_FUNC :

	return hError;

}



STATIC HERROR		local_epgdb_currentpos_fread(void *pBuffer, HUINT32 ulUnitSize, Handle_t hFileHandle, HUINT32 *pulReadSize)
{
	HERROR			hError = ERR_FAIL;
	HUINT32 		ulReadSize;
	HxFILE_t			hFile = NULL;

	if(pBuffer == NULL || hFileHandle == (Handle_t)NULL || pulReadSize == NULL)
	{
		HxLOG_Error("[local_epgdb_currentpos_fread] Param NULL!!\n");
		goto EXIT_EPGDB_FUNC;
	}
	*pulReadSize = 0;

	hFile = (HxFILE_t)hFileHandle;

	ulReadSize = HLIB_FILE_Read(hFile, (void *)pBuffer, 1, ulUnitSize);
	if (ulReadSize <= 0)
	{
		HxLOG_Error("[local_epgdb_currentpos_fread] HLIB_FILE_Read fail!!\n");
		goto EXIT_EPGDB_FUNC;
	}

	*pulReadSize = ulReadSize;

	hError = ERR_OK;

EXIT_EPGDB_FUNC :

	return hError;

}

STATIC HERROR		local_epgdb_fclose(Handle_t hFileHandle)
{
	HERROR			hError = ERR_FAIL;

	if( hFileHandle == (Handle_t)0)
	{
		HxLOG_Print("[local_epgdb_fclose] Param NULL!!\n");
		goto EXIT_EPGDB_FUNC;
	}

	HLIB_FILE_Close((HxFILE_t)hFileHandle);

	hError = ERR_OK;

EXIT_EPGDB_FUNC :

	return hError;
}


STATIC HERROR		local_epgdb_filelength(HUINT8 *pucFileName, HUINT32 *pulFileLength)
{
	HxFILE_t		hFile;
	HERROR		hError = ERR_FAIL;
	HUINT32 	ulFileSize;

	if( pucFileName == NULL || pulFileLength == NULL)
	{
		HxLOG_Print("[local_epgdb_filelength] Param NULL!!\n");
		goto EXIT_EPGDB_FUNC;
	}

	*pulFileLength = 0;

	hFile = HLIB_FILE_Open(pucFileName,"r");
	if (hFile == NULL)
	{
		HxLOG_Error("[local_epgdb_fread] HLIB_FILE_Open fail!!\n");
		goto EXIT_EPGDB_FUNC;
	}

	hError = HLIB_FILE_Size32(hFile, &ulFileSize);
	if (hError != ERR_OK)
	{
		HxLOG_Error("[local_epgdb_fread] HLIB_FILE_Size32 fail!!\n");
		HLIB_FILE_Close(hFile);
		goto EXIT_EPGDB_FUNC;
	}
	*pulFileLength = ulFileSize;

	hError = ERR_OK;
	HLIB_FILE_Close(hFile);

EXIT_EPGDB_FUNC :

	return hError;

}



STATIC HERROR		local_epgdb_ReadFileToBuffer(Handle_t hFileHandle, HUINT8 **ppBuffer, HUINT32 ulReadSize, HUINT32 *pulReadSize)
{
	HERROR			hError = ERR_FAIL;
	HUINT32			ulSizeToRead = 0;
	HUINT8			*pucFileBuffer = NULL;
	HUINT8			*pucTempBuffer = NULL;
	HUINT32			ulIndex = 0;
	HUINT32			ulBlockCount = 0;
	HUINT32			ulRestSize = 0;

	if( hFileHandle == 0 || ppBuffer == NULL || pulReadSize == NULL)
	{
		HxLOG_Print("[local_epgdb_ReadFileToBuffer] Param NULL!!\n");
		goto EXIT_EPGDB_FUNC;
	}
	*ppBuffer = NULL;
	*pulReadSize = 0;

	pucFileBuffer = (HUINT8*)OxMW_Malloc(sizeof(HUINT8)*(ulReadSize+1));
	if(pucFileBuffer == NULL)
	{
		HxLOG_Print("[local_epgdb_ReadFileToBuffer] OxMW_Malloc fail!!\n");
		goto EXIT_EPGDB_FUNC;
	}

	ulBlockCount = ulReadSize / TVTVEPG_FILE_READ_UNIT ;
	ulRestSize = ulReadSize % TVTVEPG_FILE_READ_UNIT;

	pucTempBuffer = pucFileBuffer;

	for(ulIndex = 0; ulIndex < ulBlockCount; ulIndex++)
	{
		hError = local_epgdb_currentpos_fread(pucTempBuffer, TVTVEPG_FILE_READ_UNIT, hFileHandle, &ulSizeToRead);
		if(hError != ERR_OK || ulSizeToRead == 0)
		{
			break;
		}
		pucTempBuffer += ulSizeToRead;
	}
	if(ulRestSize)
	{
		hError = local_epgdb_currentpos_fread(pucTempBuffer, ulRestSize, hFileHandle, &ulSizeToRead);
		if(hError != ERR_OK)
		{
			HxLOG_Print("[local_epgdb_ReadFileToBuffer] local_epgdb_fread fail!!\n");
		}
		pucTempBuffer += ulSizeToRead;
	}
	if(pucTempBuffer - pucFileBuffer != ulReadSize)
	{
		OxMW_Free(pucFileBuffer);
		pucFileBuffer = NULL;
		pucTempBuffer = NULL;

		HxLOG_Print("[local_epgdb_ReadFileToBuffer] file read fail !!\n");
		goto EXIT_EPGDB_FUNC;
	}

	///////////////////////////////////////////////////////////////////
	//
	// for LIBXML parser
	//
	///////////////////////////////////////////////////////////////////
	pucFileBuffer[ulReadSize] = '\0';

	*ppBuffer = pucFileBuffer;
	*pulReadSize = ulReadSize;
	hError = ERR_OK;

EXIT_EPGDB_FUNC :

	return hError;

}


#if 0
HERROR				xsvc_epg_GetTvtvExtEventByProgId(SvcEpg_FindParam_t *pParam, POINTER_LIST_T **ppExtList)
{
	HERROR						hError = ERR_EPG_FAIL;
	Handle_t						hFileHandle = (Handle_t)0;
	HUINT8						aucFileNumber[32] = {0, };
	HUINT8						aucFileName[128] = {0, };
	SvcEpg_TvtvEventFileTable_t		*pstFileTable = NULL;
	HUINT32						ulReadSize = 0;
	HUINT32						ulIndex = 0;
	HUINT32						ulXMLSize = 0;
	HUINT32						ulTotalFileOffset = 0;
	HUINT8						*pucFileBuffer = NULL;
	POINTER_LIST_T				*pExtList = NULL;


	if(pParam == NULL || ppExtList == NULL)
	{
		HxLOG_Print("[local_epgdb_GetTvtvExtEvent] param fail!!\n");
		goto EXIT_EPGDB_FUNC;
	}
	*ppExtList = NULL;

	pstFileTable = (SvcEpg_TvtvEventFileTable_t*)OxEPG_Malloc(sizeof(SvcEpg_TvtvEventFileTable_t));
	if(pstFileTable == NULL)
	{
		HxLOG_Print("[local_epgdb_GetTvtvExtEvent] OxEPG_Malloc fail!!\n");
		goto EXIT_EPGDB_FUNC;
	}
	HxSTD_memset(pstFileTable, 0x00, sizeof(SvcEpg_TvtvEventFileTable_t));

	snprintf((char*)aucFileNumber, 32, "%08d%s", pParam->ulTableId, TVTV_FILENAME_TABLEEXTENSION); // prevent fix. [2011-08-25]
	snprintf((char*)aucFileName, 128, "%s%s", EPG_FS_TVTV_DATA_PATH, aucFileNumber); // prevent fix. [2011-08-25]
	hError = local_epgdb_fopen(aucFileName, &hFileHandle);
	if(hError != ERR_OK)
	{
		HxLOG_Print("[local_epgdb_GetTvtvExtEvent] local_epgdb_fopen fail!!\n");
		goto EXIT_EPGDB_FUNC;
	}

	hError = local_epgdb_fread((void*)pstFileTable, sizeof(SvcEpg_TvtvEventFileTable_t), hFileHandle, ePAL_PB_SEEKPOSITION_CURRENT, &ulReadSize);
	if(hError != ERR_OK)
	{
		HxLOG_Print("[local_epgdb_GetTvtvExtEvent] local_epgdb_fread fail!!\n");
		goto EXIT_EPGDB_FUNC;
	}
	if(pParam->usFileIndex >= pstFileTable->ulNumOfFile)
	{
		HxLOG_Print("[local_epgdb_GetTvtvExtEvent] pParam->usFileIndex >= stFileTable.ulNumOfFile!!\n");
		goto EXIT_EPGDB_FUNC;
	}

	for(ulIndex = 0; ulIndex < pParam->usFileIndex && ulIndex < pstFileTable->ulNumOfFile; ulIndex++)
	{
		ulTotalFileOffset += pstFileTable->aulFileSizeTable[ulIndex];
	}

	hError = local_epgdb_fseek(hFileHandle, ulTotalFileOffset, ePAL_PB_SEEKPOSITION_CURRENT);
	if(hError != ERR_OK)
	{
		HxLOG_Print("[local_epgdb_GetTvtvExtEvent] local_epgdb_fread fail!!\n");
		goto EXIT_EPGDB_FUNC;
	}

	ulXMLSize = pstFileTable->aulFileSizeTable[pParam->usFileIndex];
	hError = local_epgdb_ReadFileToBuffer(hFileHandle, &pucFileBuffer, ulXMLSize, &ulReadSize);
	if(hError != ERR_OK)
	{
		HxLOG_Print("[local_epgdb_GetTvtvExtEvent] local_epgdb_fread fail!!\n");
		goto EXIT_EPGDB_FUNC;
	}

	hError = xsvc_epglib_ParseParseToHumaxExtEvent_Tvtv(pucFileBuffer, ulXMLSize, pParam->ulProgId, &pExtList);
	if(hError != ERR_OK)
	{
		HxLOG_Print("[local_epgdb_GetTvtvExtEvent] xsvc_epglib_ParseParseToHumaxExtEvent_Tvtv fail!!\n");
		goto EXIT_EPGDB_FUNC;
	}

	*ppExtList = pExtList;

	hError = ERR_EPG_OK;

EXIT_EPGDB_FUNC :

	if(pstFileTable)
	{
		OxEPG_Free(pstFileTable);
		pstFileTable = NULL;
	}

	if(hFileHandle != (Handle_t)0)
	{
		hError = local_epgdb_fclose(hFileHandle);
		if(hError != ERR_OK)
		{
			HxLOG_Print("[local_epgdb_GetTvtvExtEvent] local_epgdb_fclose fail!!\n");
		}
	}

	return hError;
}
#endif

HERROR	xsvc_epg_FreeTvtvSchEvent(void)
{
	HERROR						hError = ERR_EPG_FAIL;
	HUINT16						usSvcId = 0;
	SvcEpg_SvcEventList_t			*pSvcEventList = NULL;
	void							*pData = NULL;
	POINTER_LIST_T				*pEventList = NULL;
	POINTER_LIST_T				*pExtList = NULL;
	HTREEITEM					hOrgDeliItem = NULL;
	HTREEITEM					hTsItem = NULL;
	HTREEITEM					hServiceItem = NULL;
	HTREEMGR					hTree = NULL;
	HUINT32						ulOrgDeliType = eDxDELIVERY_TYPE_SAT;
	HUINT8						ucAntId = 0;
	SvcEpg_ServiceTsInfo_t		stEpgTsInfo = {0, };
#if	defined(CONFIG_MW_CH_SATELLITE)
{
	HINT32						lTotalAntNum = 0;
	HINT32						lAntCnt = 0;
	HINT32						*plAntIdxArray = NULL;
	HBOOL						bFoundAstra = FALSE;
	DbSvc_AntInfo_t					stAntInfo = {0, };

	hError = DB_SVC_FindAllAntIdxList (&lTotalAntNum, &plAntIdxArray);
	if (hError == ERR_OK)
	{
		for(lAntCnt = 0; lAntCnt < lTotalAntNum; lAntCnt++)
		{
			hError = DB_SVC_GetAntInfo((HUINT16)plAntIdxArray[lAntCnt], &stAntInfo);
			if (hError == ERR_OK)
			{
				if (stAntInfo.eSatType == eSatType_ASTRA_1)
				{
					bFoundAstra = TRUE;
					ucAntId = (HUINT32)plAntIdxArray[lAntCnt];
					break;
				}
			}
		}
		DB_SVC_FreeAntList (plAntIdxArray);
	}

	if(!bFoundAstra)
	{
		HxLOG_Info("[local_epgdb_FreeTvtvSchEvent] param fail!!\n");
		return ERR_FAIL;
	}
}
#endif

	ENTER_CRITICAL_TVTVBUF

	hError = xsvc_epgdblib_GetTreeMgr(&hTree);
	if(hError != ERR_TREEUTIL_OK)
	{
		HxLOG_Print("[local_epgdb_FreeTvtvSchEvent] xsvc_epgdblib_GetTreeMgr fail!!\n");
		goto EXIT_EPGDB_FUNC;
	}
	if(hTree == NULL)
	{
		HxLOG_Print("[local_epgdb_FreeTvtvSchEvent] hTree fail!!\n");
		goto EXIT_EPGDB_FUNC;
	}

	hOrgDeliItem = NULL;
	hTsItem = NULL;
	hServiceItem = NULL;

	hError = xsvc_epgdblib_FindDelieveryItem(hTree, ulOrgDeliType, &hOrgDeliItem);
	if(hOrgDeliItem == NULL)
	{
		HxLOG_Print("[local_epgdb_FreeTvtvSchEvent] xsvc_epgdblib_FindDelieveryItem fail!!\n");
		goto EXIT_EPGDB_FUNC;
	}

	hTsItem = NULL;
	hError = UTIL_TREEUTIL_GetChildItem(hOrgDeliItem, &hTsItem);
	while(hTsItem)
	{
#if		defined(CONFIG_MW_CH_SATELLITE)
		HxSTD_memset(&stEpgTsInfo, 0x00, sizeof(SvcEpg_ServiceTsInfo_t));
		hError = UTIL_TREEUTIL_GetItemUserInfo(hTsItem, sizeof(SvcEpg_ServiceTsInfo_t), (void*)&stEpgTsInfo);
		if(hError != ERR_OK)
		{
			HxLOG_Print("[local_epgdb_FreeTvtvSchEvent] GetItemUserInfo fail!!\n");
			hError = UTIL_TREEUTIL_GetNextItem(hTsItem, &hTsItem);
			if(hError != ERR_OK || hTsItem == NULL)
			{
				HxLOG_Print("[local_epgdb_FreeTvtvSchEvent] hTsItem, GetNextItem fail!!\n");
			}
			continue;
		}
		if(stEpgTsInfo.ucAntId != ucAntId)
		{
			hError = UTIL_TREEUTIL_GetNextItem(hTsItem, &hTsItem);
			if(hError != ERR_OK || hTsItem == NULL)
			{
				HxLOG_Print("[local_epgdb_FreeTvtvSchEvent] hTsItem, GetNextItem fail!!\n");
			}
			continue;
		}
#endif

		hServiceItem = NULL;
		hError = UTIL_TREEUTIL_GetChildItem(hTsItem, &hServiceItem);
		if(hServiceItem == NULL)
		{
			hError = UTIL_TREEUTIL_GetNextItem(hTsItem, &hTsItem);
			if(hError != ERR_OK || hTsItem == NULL)
			{
				HxLOG_Print("[local_epgdb_FreeTvtvSchEvent] hTsItem, GetNextItem fail!!\n");
			}
			continue;
		}
		while(hServiceItem)
		{
			pData= NULL;
			hError = UTIL_TREEUTIL_GetItemData(hServiceItem, &pData);
			if(hError != ERR_TREEUTIL_OK)
			{
				HxLOG_Print("[local_epgdb_FreeTvtvSchEvent] UTIL_TREEUTIL_GetItemData fail!!\n");
				break;
			}
			hError = UTIL_TREEUTIL_GetItemUserInfo(hServiceItem, sizeof(HUINT16), (void*)&usSvcId);
			if(hError != ERR_TREEUTIL_OK)
			{
				HxLOG_Print("[local_epgdb_FreeTvtvSchEvent] UTIL_TREEUTIL_GetItemData fail!!\n");
				break;
			}

			pSvcEventList = (SvcEpg_SvcEventList_t*)pData;
			if(pSvcEventList->ulEpgServiceTuneType != eEPG_SPEC_TVTV)
			{
				hError = UTIL_TREEUTIL_GetNextItem(hServiceItem, &hServiceItem);
				if(hError != ERR_OK || hTsItem == NULL)
				{
					HxLOG_Print("[local_epgdb_FreeTvtvSchEvent] hTsItem, GetNextItem fail!!\n");
				}
				continue;
			}
			pSvcEventList->ulEpgServiceTuneType = eEPG_SPEC_BASE;

			pEventList = NULL;
			pExtList = NULL;

			if(pSvcEventList->pSchExtList)
			{
				pExtList = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList(	pSvcEventList->pSchExtList,
																				xsvc_epgdblib_FreeExtEventListContents);
				pSvcEventList->pSchExtList = pExtList;
			}

			if(pSvcEventList->pSchList)
			{
				pEventList = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList(	pSvcEventList->pSchList,
																				xsvc_epgdblib_FreeEventListContents);
				pSvcEventList->pSchList = pEventList;
			}

			hError = UTIL_TREEUTIL_GetNextItem(hServiceItem, &hServiceItem);
			if(hError != ERR_OK || hServiceItem == NULL)
			{
				HxLOG_Print("[local_epgdb_FreeTvtvSchEvent] hServiceItem, GetNextItem fail!!\n");
			}
		}

		hError = UTIL_TREEUTIL_GetNextItem(hTsItem, &hTsItem);
		if(hError != ERR_OK || hTsItem == NULL)
		{
			HxLOG_Print("[local_epgdb_FreeTvtvSchEvent] hTsItem, GetNextItem fail!!\n");
		}
	}

	hError = ERR_EPG_OK;

EXIT_EPGDB_FUNC :
	LEAVE_CRITICAL_TVTVBUF;

	return hError;
}
#endif




/*********************** End of File ******************************/
