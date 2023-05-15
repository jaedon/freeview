/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  ttx_decoder.c
	@brief    Teletext decoder

	Description:
	Module:

	Copyright (c) 2008 HUMAX Co., Ltd.			\n
	All rights reserved.						\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
/** @brief global header file  */
#include <stdarg.h>
#include <stdio.h>
#if defined(CONFIG_DEBUG)
#include <ctype.h>
#endif
#include <stdlib.h>

//#include <octo_common.h>
#include <hlib.h>
#include <timeconv.h>
#include <nx_common.h>
#include <ttx_decoder.h>
#include <ttx_errchk.h>
#include <ttx_graphics.h>
#include <ttx_drcs.h>
#include <mempool.h>
/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/
typedef enum
{
	eTTX_DBG_LV_NONE		= 0x00000000,

	eTTX_DBG_LV_TRACE		= HxLOG_PRINT,
	eTTX_DBG_LV_PRINT		= HxLOG_PRINT,
	eTTX_DBG_LV_DATA		= HxLOG_INFO,
	eTTX_DBG_LV_ASSERT		= HxLOG_ERROR,

	eTTX_DBG_LV_LOG			= 0x0000000F,

	eTTX_DBG_LV_CURPAGE		= 0x00001000,
	eTTX_DBG_LV_TOP			= 0x00002000,
	eTTX_DBG_LV_PKTMONITOR	= 0x00004000,
	eTTX_DBG_LV_MEMORY		= 0x00008000,

	eTTX_DBG_LV_ALL			= 0xFFFFFFFF
} TTX_DBG_Level_t;


#if defined(CONFIG_DEBUG)
HUINT32		g_MwTtxDec_Level;
#endif
/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/



/**@brief Teletext Decoder's predefined contants */
#define TTX_MAX_MAGAZINE				8

#define TTX_MAX_PAGE_NUM				800		// 899가 최대 넘버이지만, 800~899는 0~99로 매핑되기 때문에, 800이 최대값
#define TTX_MAX_WEIGHT					10
#define	TTX_MAX_PACKET_OF_PAGE			74

#define TTX_PACKET_DATA_BYTES			40
#define TTX_HEADER_DATA_BYTES			32

#define	TTX_HEADER_ADDR_BYTES			8

#define TTX_LSB_FRAMING_CODE			0x27
#define TTX_FRAMING_CODE				0xE4

#define TTX_MIN_MEMORY					10000

/**
 * For all types of pages, the maximum time interval between the transmission of successive packets of the same page shall be 100ms.
 */
#define	TTX_MAX_TX_INTERVAL				100

#define	TTX_SUBCODE(header)				(HUINT16)((header[2] & 0xF)|((header[3] & 0x7) << 4)|((header[4] & 0xF) << 8)|((header[5] & 0x3) << 12))
#define ISASCII(c)						(c >= 0x00 && c <= 0x7F)

/**
 * @see ETS 300 706, Table 2: Control bits in the page header
 */
enum tagTTX_ControlBits
{
	eTTX_C4_ERASE_PAGE				= 0x01,
	eTTX_C5_NEWSFLASH				= 0x02,
	eTTX_C6_SUBTITLE				= 0x04,
	eTTX_C7_SUPPRESS_HEADER			= 0x08,
	eTTX_C8_UPDATE_INDICATOR		= 0x10,
	eTTX_C9_INTERRUPTED_SEQUENCE	= 0x20,
	eTTX_C10_INHIBIT_DISPLAY		= 0x40,
	eTTX_C11_MAGAZINE_SERIAL		= 0x80
};


/**
 * TTX Packet Optional Field
 * +--------------------------+------------+-----------------------------------+
 * |  Syntax                  | No.of Bits |   Description                     |
 * +--------------------------+------------+-----------------------------------+
 * | if (header_packet) {     |            |                                   |
 * |     reserved             |     3      | Reserved for future use           |
 * |     page_decoded         |     1      | 페이지 디코딩(On demand) 여부     |
 * |     data_page            |     1      | Data Page인 경우                  |
 * |     packet_decoded       |     1      | Packet 디코딩 여부                |
 * |     reserved             |     2      | Reserved for future use           |
 * | }                        |            |                                   |
 * | else {                   |            |                                   |
 * |     reserved             |     5      | Reserved for future use           |
 * |     packet_decoded       |     1      | Packet 디코딩 여부                |
 * |     reserved             |     2      | Reserved for future use           |
 * | }                        |            |                                   |
 * +--------------------------+------------+-----------------------------------+
 */
enum tagTTX_OptionalField
{
	eTTX_OPT_RESERVED				= 0x03,
	eTTX_OPT_PACKET_DECODED			= 0x04,
	eTTX_OPT_DATA_PAGE				= 0x08,
	eTTX_OPT_PAGE_DECODED			= 0x10
};


/**
 * TTX Spacing Attributes
 */
enum tagTTX_SpacingAttributes
{
	eTTX_SA_NULL					= 0x00000000,

	eTTX_SA_ALPHA_COLOR_CODES		= 0x000000FF,
	eTTX_SA_ALPHA_BLACK				= 0x00000001,
	eTTX_SA_ALPHA_RED				= 0x00000002,
	eTTX_SA_ALPHA_GREEN				= 0x00000004,
	eTTX_SA_ALPHA_YELLOW			= 0x00000008,
	eTTX_SA_ALPHA_BLUE				= 0x00000010,
	eTTX_SA_ALPHA_MAGENTA			= 0x00000020,
	eTTX_SA_ALPHA_CYAN				= 0x00000040,
	eTTX_SA_ALPHA_WHITE				= 0x00000080,
	eTTX_SA_FLASH					= 0x00000100,
	eTTX_SA_STEADY					= 0x00000200,
	eTTX_SA_END_BOX					= 0x00000400,
	eTTX_SA_START_BOX				= 0x00000800,
	eTTX_SA_NORMAL_SIZE				= 0x00001000,
	eTTX_SA_DOUBLE_HEIGHT			= 0x00002000,
	eTTX_SA_DOUBLE_WIDTH			= 0x00004000,
	eTTX_SA_DOUBLE_SIZE				= 0x00008000,
	eTTX_SA_MOSAIC_COLOR_CODES		= 0x00FF0000,
	eTTX_SA_MOSAIC_BLACK			= 0x00010000,
	eTTX_SA_MOSAIC_RED				= 0x00020000,
	eTTX_SA_MOSAIC_GREEN			= 0x00040000,
	eTTX_SA_MOSAIC_YELLOW			= 0x00080000,
	eTTX_SA_MOSAIC_BLUE				= 0x00100000,
	eTTX_SA_MOSAIC_MAGENTA			= 0x00200000,
	eTTX_SA_MOSAIC_CYAN				= 0x00400000,
	eTTX_SA_MOSAIC_WHITE			= 0x00800000,
	eTTX_SA_CONCEAL					= 0x01000000,
	eTTX_SA_CONTIGUOUSE_MOSAIC		= 0x02000000,
	eTTX_SA_SEPARATED_MOSAIC		= 0x04000000,
	eTTX_SA_ESC						= 0x08000000,
	eTTX_SA_BLACK_BACKGROUND		= 0x10000000,
	eTTX_SA_NEW_BACKGROUND			= 0x20000000,
	eTTX_SA_HOLD_MOSAICS			= 0x40000000,
	eTTX_SA_RELEASE_MOSAICS			= 0x80000000,

	eTTX_SA_SET_AT					= eTTX_SA_STEADY
									| eTTX_SA_NORMAL_SIZE
									| eTTX_SA_CONCEAL
									| eTTX_SA_CONTIGUOUSE_MOSAIC
									| eTTX_SA_SEPARATED_MOSAIC
									| eTTX_SA_BLACK_BACKGROUND
									| eTTX_SA_NEW_BACKGROUND
									| eTTX_SA_HOLD_MOSAICS,
	eTTX_SA_SET_AFTER				= ~eTTX_SA_SET_AT
};

/**
 * @see ETS 300 706, Table 3: Page function and page coding bits
 */
typedef enum
{
	eTTX_FUNCTION_LOP,
	eTTX_FUNCTION_DATA_BROADCASTING,
	eTTX_FUNCTION_GPOP,
	eTTX_FUNCTION_POP,
	eTTX_FUNCTION_GDRCS,
	eTTX_FUNCTION_DRCS,
	eTTX_FUNCTION_MOT,
	eTTX_FUNCTION_MIP,
	eTTX_FUNCTION_TOP_BTT,
	eTTX_FUNCTION_TOP_AIT,
	eTTX_FUNCTION_TOP_MPT,
	eTTX_FUNCTION_TOP_MPT_EX,
	eTTX_FUNCTION_TRIGGER_MSG,

	eTTX_FUNCTION_LAST,

	eTTX_FUNCTION_UNKNOWN		= 0x000000FF
} TTX_PageFunction_t;


/**
 * @see ETS 300 706, Table 3: Page function and page coding bits
 */
typedef enum
{
	eTTX_CODING_ODDPARITY,
	eTTX_CODING_8_DATA_BIT,
	eTTX_CODING_HAMMING2418,
	eTTX_CODING_HAMMING84,
	eTTX_CODING_TOP_AIT,
	eTTX_CODING_VARIABLE,

	eTTX_CODING_LAST,

	eTTX_CODING_UNKNOWN			= 0x000000FF
} TTX_PageCoding_t;


typedef enum
{
	eTTX_TIME_BY_NONE,
	eTTX_TIME_BY_HEADER,
	eTTX_TIME_BY_TIME_HEADER,
	eTTX_TIME_BY_MJD_UTC,

	eTTX_TIME_BY_SYSTEM,

	eTTX_TIME_LAST
} TTX_TimeSetBy_t;
/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

//1 LOCAL TYPE DEFINITIONS
#define ___TTX_LOCAL_TYPE_DEFINITIONS___

/**
 * Teletext Character Set
 */
typedef struct
{
	HUINT8	ucG0Set;			///< G0 Primary Character set
	HUINT8	ucG2Set;			///< G2 Supplementary Character set
	HUINT8	ucNationalOption;	///< National Option Character
}TTX_CharSet_t;


/**
 * TTX Page를 그릴 때 사용하는 디폴트 정보
 */
typedef struct
{
	HBOOL			bUpdated;		///< X/28, M/29에 의해 update되었는지 여부
	TTX_CharSet_t	charSet;		///< default Character set
	TTX_CharSet_t	secondCharSet;	///< Second Character set
	HUINT8			ucCharSetDesg;
	HUINT8			uc2ndCharSetDesg;
	HUINT8			ucRowColor;		///< default row color
	HUINT8			ucScreenColor;	///< default screen color
}TTX_DefaultInfo_t;


/**
 * Spacing Attributes
 *
 * @see	Table 26: Spacing Attributes, ETS 300 706
 */
typedef struct
{
	HUINT8	ucFgColor;			///< Foreground color
	HUINT8	ucBgColor;			///< Background color
	HUINT8	ucHeldMosaic;		///< held mosaic character
	HUINT8	bMosaicMode;		///< Mosaic mode
	HUINT8	bBox;				///< Box mode (start box/end box)
	HUINT8	bESC;				///< Switch default & 2nd character set
	HUINT8	bHoldMosaic;		///< Hold mosaic mode
	HUINT8	bConceal;			///< conceal
	HUINT8	bFlash;				///< flash
	HUINT8	bDoubleHeight;		///< double height
	HUINT8	bDoubleWidth;		///< double width
	HUINT8	bSeparatedMosaic;	///< separated mosaic
	HUINT8	ucG0;				///< default G0 character set
	HUINT8	ucNationalOpt;		///< national option character subset
	HUINT8	uc2ndG0;			///< second G0 character set
	HUINT8	uc2ndNationalOpt;	///< second national option character subset
}TTX_SpacingAttr_t;


/**
 * TTX Packet
 *
 * total 48 bytes
 */
typedef struct tagTTX_Packet
{
	/**
	 * TTX Packet Data
	 */
	HUINT8	aucData[TTX_PACKET_DATA_BYTES];
	HUINT8	ucY;						///< Packet number
	HUINT8	ucDesgCode;					///< Designation Code
	HUINT8	ucReserved;					///< Packed되지 않은 struct의 남은 용량을 체우기 위한 필드
	HUINT8	ucOptionalField;			///< see: enum tagTTX_OptionalField
	struct tagTTX_Packet *pNextPacket;	///< Next Packet
}TTX_Packet_t;


/**
 * TTX Header Packet
 *
 * total 48 bytes
 */
typedef struct tagTTX_Header
{
	HUINT16	usPageNum;		///< Page number
	HUINT16	usSubcode;		///< Subcode
	/**
	 * Next Sub-page
	 * 각 서브페이지는 usSubcode를 기준으로 하여 오름차순으로 정렬되어 있다.
	 */
	struct tagTTX_Header	*pSubPage;
	/**
	 * TTX Header Packet Data
	 */
	HUINT8	aucData[TTX_HEADER_DATA_BYTES];
	HUINT8	ucY;				///< Packet number
	HUINT8	ucNationalOption;	///< National Option
	HUINT8	ucControlBit;		///< see: enum tagTTX_ControlBits
	HUINT8	ucOptionalField;	///< see: enum tagTTX_OptionalField
	struct tagTTX_Packet *pNextPacket;	///< Next Packet
}TTX_Header_t;


//1 >>> README !!
/*
 * 1. TTX_Packet_t과 TTX_Header_t는 서로 케스팅을 통해 다형성을 유지한다.
 * 2. 다형성을 유지하기 위해서 구조체 내의 멤버들의 순서를 적절히 맞춰 놓았으며, 만약 바꾸길
 *    원한다면 신중을 기하길 바란다.
 * 3. 메모리에 민감한 자료구조. 1메가를 기준으로 했을 때, 최대 패킷의 개수는 약 2만개로 구조체를
 *    1바이트만 증가 시켜도 20kbyte의 메모리를 더 필요로 한다. 이것은 곧 케시에 저장되는 텔레텍스트
 *    페이지의 감소로 이어질 것이며, 그렇게 되면 페이지 전환이 더 느려지는 결과를 초래한다.
 * 즉, !!! 왠만하면 구조체를 변경하지 말 것 !!!
 *
 * @author	이종혁2(jhlee4@humaxdigital.com)
 */
//1 <<< README !!


typedef struct
{
	HUINT16	usPageNum;
	HUINT32	ulElapTick;
}TTX_TimeStamp_t;

typedef struct
{
	TTX_TimeSetBy_t	eSetBy;
	TIME_T			time;
	HUINT32			ulLastTick;
}TTX_Time_t;


/**
 * Teletext Cache
 *
 * 디코더 내부에서 페이지를 저장하여, 외부로부터의 페이지 요청에
 * 빠르게 응답하기 위한 케시 자료구조
 */
typedef struct
{
	/**
	 * 처음 디코더가 시작된 후, 한번 케시가 꽉 찰때까지 헝그리 모드로 동작하며,
	 * 새로들어오는 페이지를 모두 받아들인다. (hit rate을 최대한 빠른 시간 내에 높이기 위한 동작)
	 */
	HBOOL			bHungry;
	HUINT32			ulCount;	///< 케시 내의 페이지 개수
	HUINT32			ulMaxCount;	///< 케시 내의 최대 페이지 개수

	HUINT32			ulStartTick;

	/**
	 * 페이지가 들어오거나, 요청된 시간에 대한 Time Stamp. 이는 Cache를 Arrange할 때 쓰인다.
	 */
	TTX_TimeStamp_t	aTimeStamp[TTX_MAX_PAGE_NUM];

	/**
	 * 케시 버퍼
	 */
	TTX_Header_t	*apBuf[TTX_MAX_PAGE_NUM];
}TTX_Cache_t;


typedef struct
{
	/**
	 * 현재 디먹싱 되고 있는 페이지의 헤더를 받은 시간
	 * Flush 함수에서 이 시간을 참조하여, Flushing 여부를 결정한다.
	 */
	HUINT32			ulRecvTick;
	TTX_Header_t	*pHeader;	///< 현재 디먹싱 되고 있는 페이지의 헤더 패킷
	TTX_Packet_t	*pLast;		///< 현재 디먹싱 되고 있는 페이지의 마지막 패킷
	HUINT32			ulCount;	///< 현재 디먹싱 되고 있는 페이지의 패킷 개수
}TTX_DemuxPipe_t;


/**
 * TTX Decoder
 *
 * 텔레텍스트 디코더 자료구조
 * 내부적으로 메모리 풀, 케시, 텔레텍스트 그래픽스 핸들 등 디코딩에 필요한 자료구조를 포함
 */
typedef struct
{
	MemPool_t			*pMemory;			///< Teletext Memory
	TTX_Cache_t			cache;				///< TTX Page Cache
	HBOOL				bEnabled;			///< Decoder Enable/Disable
	HUINT16				usFilterPageNum;	///< Page number filter
	HUINT16				usFilterSubcode;	///< Subcode filter
	TTX_PageReceiver_t	pfnReceiver;		///< Page Receiver Callback
	void 				*userdata;			///< Userdata

	Handle_t			hTOP;				///< TTX TOP handle
	Handle_t			hDrcs;				///< TTX DRCS handle
	/**
	 * Rolling Header(Search 중)를 위해 가장 최근에 들어온 헤더를 모니터 한다.
	 */
	HUINT8				aucRollingHeader[TTX_HEADER_DATA_BYTES];

	TTX_Time_t			ttxTime;			///< TTX Time

	/**
	 * Header를 통해 들어온 최신 타임
	 */
	HUINT8				*pucTimeFill;

	/**
	 * TTX Demux Pipe
	 */
	TTX_DemuxPipe_t		aPipe[TTX_MAX_MAGAZINE];
	TTX_DemuxPipe_t		*pCurPipe;

	/**
	 * 각 메거진 별로 유지되는 디폴트 정보
	 */
	TTX_DefaultInfo_t	aMagazineInfo[TTX_MAX_MAGAZINE];

	HUINT16				usInitPageNum;
	HUINT16				usInitSubcode;

	HUINT8				aucNI[2];		///< Network Identification Code, 8/30 format 1

	HUINT8				aucHeaderDupCheck[TTX_HEADER_ADDR_BYTES];

	TTX_Page_t			pageBuffer;		///< Page Buffer

	/**
	 * X/28이나 M/29가 들어오지 않고, National Option만으로 다양한 언어를
	 * 선택할 수는 없지만, 사용자가 정의한 경우, 그게 가능하도록 기능을 열어둔다.
	 * 0x0 ~ 0x7까지의 C12-14 값으로 Character Set을 선택한다.
	 */
	struct
	{
		HBOOL			bDefined;
		TTX_CharSet_t	set;
	} aUserDefinedChar[eTTX_NOS_MAX + 1];

#if defined(CONFIG_DEBUG)
	/**
	 * TTX Stream Capture용 파일 포인터
	 */
	FILE				*fpCapture;
	FILE				*fpPacket;	///< Packet Monitor용
#endif
}TTX_Decoder_t;



//1 LOCAL FUNCTION PROTOTYPES
#define ____TTX_LOCAL_FUNC_PROTOTYPES____

#define	__decl__ttx_demux____
static HERROR			local_ttx_DemuxTTXPacket		(TTX_Decoder_t *pDec, HUINT8 *pucPacket);
static HERROR			local_ttx_DemuxHeaderPacket		(TTX_Decoder_t *pDec, const HUINT8 ucM, HUINT8 *pucHeader);
static HERROR			local_ttx_DemuxY1toY28 			(TTX_Decoder_t *pDec, const HUINT8 ucM, const HUINT8 ucY, HUINT8 *pucPacket);
static HERROR 			local_ttx_DemuxServiceDataPacket (TTX_Decoder_t *pDec, HUINT8 *pucPacket);
static HBOOL 			local_ttx_IsHeaderDuplicated 	(TTX_Decoder_t *pDec, const HUINT8 ucM, const HUINT8 *pucNewHeader);
static HERROR 			local_ttx_SetTeletextTime 		(TTX_Decoder_t *pDec, const HUINT8 *pucHeader, const TTX_TimeSetBy_t eTimeBy);
static HBOOL 			local_ttx_IsNearPageAddr 		(const HUINT16 usBase, const HUINT16 usAddr, const HUINT32 ulRange);
static HBOOL 			local_ttx_IsAcceptablePage 		(TTX_Decoder_t *pDec, const TTX_Header_t *pPage);
#if TTX_WORKAROUND_05 != TTX_ENABLE
static HBOOL 			local_ttx_IsSamePageNumber 		(const TTX_Header_t *pHeader, const HUINT8 ucX);
#endif
static HERROR			local_ttx_TerminatePage 		(TTX_Decoder_t *pDec, const HUINT8 ucM, const HUINT8 ucX, const HBOOL bSerial);
static TTX_Result_t		local_ttx_NotifyToReceiver 		(TTX_Decoder_t *pDec, TTX_Header_t *pHeader);


#define	__decl__ttx_memory_management____
static TTX_Result_t		local_ttx_ReceivePage			(TTX_Decoder_t *pDec, TTX_Header_t *pHeader);
static TTX_Result_t		local_ttx_ReceiveLOP			(TTX_Decoder_t *pDec, TTX_Header_t *pHeader);
static TTX_Result_t		local_ttx_ReceiveTOP 			(TTX_Decoder_t *pDec, TTX_Header_t *pHeader);
static HINT32			local_ttx_TimeStampCompare 		(const TTX_TimeStamp_t *lhs, const TTX_TimeStamp_t *rhs);
static TTX_Header_t *	local_ttx_FetchFromCache		(TTX_Decoder_t *pDec, const HUINT16 usPageNum, const HUINT16 usSubcode);
static int	 			local_ttx_PageAddrCompare 		(const void *pvArg1, const void *pvArg2);
static HERROR 			local_ttx_MergePage				(TTX_Header_t *pH1, TTX_Header_t *pH2);
static HERROR 			local_ttx_WriteToCache			(TTX_Decoder_t *pDec, TTX_Header_t *pPkt);
static HERROR 			local_ttx_EraseFromCache 		(TTX_Decoder_t *pDec, const HUINT16 usPageNum, const HUINT16 usSubcode);
static void 			local_ttx_ArrangeCache			(TTX_Decoder_t *pDec);
static void				local_ttx_ResetCache 			(TTX_Decoder_t *pDec);
static void *			local_ttx_NewPacket 			(TTX_Decoder_t *pDec);
static TTX_Packet_t * 	local_ttx_DeletePacket 			(TTX_Decoder_t *pDec, TTX_Packet_t *pPacket);
static TTX_Header_t *	local_ttx_DeleteSubPage 		(TTX_Decoder_t *pDec, TTX_Header_t *pHeader);
static void				local_ttx_DeletePage			(TTX_Decoder_t *pDec, TTX_Header_t *pHeader);

#define __decl__ttx_decode____
static HERROR 			local_ttx_DecodePage 			(TTX_Decoder_t *pDec, TTX_Header_t *pHeader);
static HINT32			local_ttx_DecodePacket			(HUINT8	*pucPacket, const TTX_PageCoding_t eCoding);
static HERROR			local_ttx_ParseFuncAndCoding 	(HUINT8 *pucY28, TTX_PageFunction_t *peFunc, TTX_PageCoding_t *peCoding);
static TTX_PageFunction_t	local_ttx_GetPageFunction		(TTX_Decoder_t *pDec, const TTX_Header_t *pHeader);
static TTX_PageCoding_t		local_ttx_GetDefaultPageCoding	(const TTX_PageFunction_t eFunction);

#define	__decl__ttx_char_set_designation____
static HERROR			local_ttx_MakeDefaultInfo		(TTX_DefaultInfo_t *pInfo, const HUINT8 *pucPktData);
static HERROR 			local_ttx_CharSetDesignation 	(TTX_CharSet_t *pSet, const HUINT8 ucCharSetDesg);
static HERROR			local_ttx_CharSetDesignationByNI (TTX_CharSet_t *pSet, const HUINT8 *pucNI);
static HERROR			local_ttx_ChooseLanguage 		(TTX_Decoder_t *pDec, TTX_CharSet_t *pSet, const HUINT8 ucNOS);

#define	__decl__ttx_make_teletext_page____
#if 1
static void				local_ttx_MakeTeletextPage		(TTX_Decoder_t *pDec, TTX_Page_t *pPage, TTX_Header_t *pHeader);
static void 			local_ttx_SpacingAttrSetAt		(TTX_SpacingAttr_t *pAttr, const HUINT8 ucAttr);
static void 			local_ttx_SpacingAttrSetAfter	(TTX_SpacingAttr_t *pAttr, const HUINT8 ucAttr);
static void 			local_ttx_MakeTeletextCell		(TTX_Cell_t *pCell, TTX_SpacingAttr_t *pAttr, const HUINT8 ucAttr, const HUINT8 bBoxMode);
static TTX_Packet_t *	local_ttx_MakeSpacingAttr 		(TTX_Page_t *pPage, TTX_Packet_t *pPkt, const TTX_DefaultInfo_t *pDefaultInfo);
static TTX_Packet_t *	local_ttx_MakeNonSpacingAttr 	(TTX_Page_t *pPage, TTX_Packet_t *pPkt, const TTX_DefaultInfo_t *pDefaultInfo);
static TTX_Packet_t *	local_ttx_MakePageRelatedInfo	(TTX_DefaultInfo_t *pDefaultInfo, TTX_Packet_t *pPkt);
static void				local_ttx_ApplySizeAttributes	(TTX_Page_t *pPage);
static void				local_ttx_MakeRollingHeader 	(TTX_Decoder_t *pDec, TTX_Page_t *pPage);
static void				local_ttx_MakeTeletextTime		(TTX_Decoder_t *pDec, TTX_Page_t *pPage);
static HBOOL 			local_ttx_IsTimeFormat 			(HUINT8 *pucTime);
#endif
static void				local_ttx_SearchCTT 			(TTX_Decoder_t *pDec, HUINT16 usPageNum, HUINT16 ausCTT[]);
static HERROR			local_ttx_MakeCTT				(TTX_Decoder_t *pDec, TTX_Page_t *pPage);
static HERROR			local_ttx_MakeSubcodeBar 		(TTX_Decoder_t *pDec, TTX_Page_t *pPage);

#define	__decl__ttx_searching_functions____
static TTX_Header_t *	local_ttx_SearchPage			(TTX_Decoder_t *pDec, const HUINT16 usPageNum, const HUINT16 usSubcode, const TTX_Search_t search);
static TTX_Header_t *	local_ttx_SearchNextPage 		(TTX_Decoder_t *pDec, const HUINT16 usPageNum);
static TTX_Header_t *	local_ttx_SearchPrevPage 		(TTX_Decoder_t *pDec, const HUINT16 usPageNum);
static TTX_Header_t *	local_ttx_SearchNextSubPage 	(TTX_Decoder_t *pDec, const HUINT16 usPageNum, const HUINT16 usSubcode);
static TTX_Header_t *	local_ttx_SearchPrevSubPage 	(TTX_Decoder_t *pDec, const HUINT16 usPageNum, const HUINT16 usSubcode);

#define	__decl__ttx_FLOF____
static HERROR			local_ttx_DecodeFLOF 			(HUINT8 *pucPacket);
static TTX_Packet_t * 	local_ttx_ParseFLOF 			(HUINT16 usPageNum, TTX_FLOF_t *pFLOF, TTX_Packet_t *pY27);

#define	__decl__ttx_debug____
static void				local_ttx_DebugPage 			(TTX_Header_t *pHeader, TTX_Page_t *pPage);
static void				local_ttx_DebugY26 				(FILE *fp, TTX_Packet_t *pY26);
static void				local_ttx_DebugPacketMonitor 	(TTX_Decoder_t *pDec, const HUINT8 ucM, const HUINT8 ucY, const HUINT8 aucPacket[]);



#define ___UTIL__FUNCTION________________________
unsigned long sys_tick_init_val = 0;
#define VK_INVALID_CLOCK	15

#if !defined(WIN32)
unsigned long VK_TIMER_GetSystemTick(void)
{
	int rc;
#if defined (WIN32)
	struct timespec {
			long		tv_sec;
			long		tv_nsec;
	};
#endif
	struct timespec humax_time;
	unsigned long	msTick;

#if defined (WIN32)
	rc = 0;
#else
	rc = clock_gettime(CLOCK_MONOTONIC, &humax_time);
#endif
    if (rc!=0) {
        HxLOG_Critical("clock_gettime returned %d, ignored", rc);
		return 0;
    }
	msTick = (humax_time.tv_sec*1000 + humax_time.tv_nsec/1000000);
	return (unsigned long)(msTick-sys_tick_init_val);
}
#endif

#if !defined(WIN32)
#define VALID_UNIXTIME					(60 * 60 * 24 * 366)			// until 1971/01/02
unsigned long VK_CLOCK_GetTime (unsigned long *pulUnixTime)
{
	if (!pulUnixTime)
	{
		return ERR_FAIL;
	}

	*pulUnixTime = (unsigned long)time(NULL);
	if (*pulUnixTime < VALID_UNIXTIME) {
		*pulUnixTime = 0;
		return VK_INVALID_CLOCK;
	}

	return ERR_OK;
}
#endif





//1 GLOBAL FUNCTION DEFINITIONS
#define ____TTX_GLOBAL_FUNC_DEFINITIONS____

//2 Create Decoder
Handle_t	TTX_DECODER_Create	(void *pvMemory, const HUINT32 ulLength)
{
	TTX_Decoder_t *pDec	= NULL;

	HxLOG_Print("Create New Decoder\n");
	if (pvMemory == NULL || ulLength < TTX_MIN_MEMORY)
	{
		return HANDLE_NULL;
	}

	pDec = (TTX_Decoder_t *)NX_APP_Calloc(sizeof(TTX_Decoder_t));
	if (pDec == NULL)
	{
		return HANDLE_NULL;
	}

	// create memory pool
	pDec->pMemory	= MEMPOOL_Create(
						pvMemory, 				// memory area
						ulLength, 				// length of memory area in bytes
						sizeof(TTX_Packet_t), 	// size of an element
						NULL,					// packet constructor
						NULL					// packet destructor
					);

	if (pDec->pMemory == NULL)
	{
		NX_APP_Free(pDec);

		return HANDLE_NULL;
	}

	pDec->cache.bHungry = TRUE; // Decoder가 시작되면, 우선 캐시를 체워야 hit rate이 올라가므로,
								// 캐시를 Hungry 모드로 작동시킨다.
	pDec->cache.ulStartTick	= VK_TIMER_GetSystemTick();
	pDec->pucTimeFill		= &(pDec->aucRollingHeader[24]);
	pDec->usInitPageNum		= 100;
	pDec->usInitSubcode		= TTX_IGNORE_SUBCODE;

	return (Handle_t)pDec;
}


//2 Destroy Decoder
void	TTX_DECODER_Destroy	(Handle_t hDecoder)
{
	TTX_Decoder_t *pDec = (TTX_Decoder_t *)hDecoder;

	if (hDecoder == HANDLE_NULL || pDec == NULL)
	{
		return;
	}

	HxLOG_Print("Destroy Decoder\n");

#ifdef CONFIG_DEBUG
	if (pDec->fpCapture)
	{
		HxLOG_Print("[TTX_DECODER_StreamCapture] End\n");
		fclose(pDec->fpCapture);
	}

	if (g_MwTtxDec_Level & eTTX_DBG_LV_MEMORY)	// memory leak test
	{
		int i;
		for (i = 0 ; i < TTX_MAX_PAGE_NUM ; i++)
			local_ttx_DeletePage(pDec, pDec->cache.apBuf[i]);

		for (i = 0 ; i < TTX_MAX_MAGAZINE ; i++)
			if (pDec->aPipe[i].pHeader)
				local_ttx_DeleteSubPage(pDec, pDec->aPipe[i].pHeader);
	}
	MEMPOOL_Print(pDec->pMemory, g_MwTtxDec_Level);

	if (pDec->fpPacket)
		fclose(pDec->fpPacket);
#endif
	MEMPOOL_Destroy(pDec->pMemory);

	NX_APP_Free(pDec);

	// warning 제거
	//(void)local_ttx_DebugPage;
}

//2 Switch
TTX_Result_t	TTX_DECODER_Switch (ONDKWindowHandle	hwindow,Handle_t hDecoder, HBOOL bOn)
{
	TTX_Decoder_t *pDec = (TTX_Decoder_t *)hDecoder;
	if (hDecoder == HANDLE_NULL || pDec == NULL)
	{
		return ERR_TTX_INVALID_ARGUMENT;
	}
	if (bOn == pDec->bEnabled)
	{
		return ERR_TTX_FAIL;
	}

	if (bOn == TRUE)
	{
		pDec->hDrcs	= TTX_DRCS_Create();
		if (pDec->hDrcs == HANDLE_NULL)
		{
			return ERR_TTX_FAIL;
		}
		TTX_GFX_Create(hwindow,pDec->hDrcs);
		pDec->hTOP	= TTX_TOP_Create();
	}
	else
	{
		TTX_GFX_Destroy();
		TTX_DRCS_Destroy(pDec->hDrcs);
		pDec->hDrcs = HANDLE_NULL;
		TTX_TOP_Destroy(pDec->hTOP);
		pDec->hTOP = HANDLE_NULL;
	}

	pDec->bEnabled	= bOn;
	return ERR_TTX_OK;
}


//2 Demux
TTX_Result_t	TTX_DECODER_Demux	(Handle_t hDecoder, HUINT8 *pucPacket)
{
	TTX_Decoder_t	*pDec = (TTX_Decoder_t *)hDecoder;

	if (hDecoder == HANDLE_NULL || pDec == NULL || pucPacket == NULL)
	{
		return ERR_TTX_INVALID_ARGUMENT;
	}

	if (pDec->bEnabled == FALSE)
	{
		return ERR_TTX_DISABLED;
	}
	// if LSB first (Framing Code)
	if (pucPacket[0] == TTX_LSB_FRAMING_CODE)
	{
		TTX_ERRCHK_ReverseBit(pucPacket, pucPacket, TTX_PACKET_LENGTH);
	}

#if defined(CONFIG_DEBUG)
	if (pDec->fpCapture)
	{
		fwrite (pucPacket, sizeof(HUINT8), TTX_PACKET_LENGTH, pDec->fpCapture);
	}
#endif

	// CHECK Framing Code
	if (pucPacket[0] == TTX_FRAMING_CODE)
	{
		return local_ttx_DemuxTTXPacket(pDec, pucPacket + 1);
	}

	return ERR_TTX_WRONG_FRAMING_CODE;
}


//2 Set Page Filter
TTX_Result_t	TTX_DECODER_SetFilter	(Handle_t hDecoder, const HUINT16 usPageNum, const HUINT16 usSubcode,
										const HBOOL bRecvCached, TTX_PageReceiver_t pfnReceiver)
{
	TTX_Decoder_t 	*pDec 		= (TTX_Decoder_t *)hDecoder;
	TTX_Header_t	*pHeader	= NULL;
	HUINT32			ulTick		= 0;

	if (hDecoder == HANDLE_NULL || pDec == NULL || usPageNum >= TTX_MAX_PAGE_NUM)
	{
		return ERR_TTX_INVALID_ARGUMENT;
	}

	if (pDec->bEnabled == FALSE)
	{
		return ERR_TTX_DISABLED;
	}
#if 0
	if (pDec->usFilterPageNum == usPageNum
		&& pDec->usFilterSubcode == usSubcode
		&& pDec->pfnReceiver == pfnReceiver)
	{
		return ERR_TTX_FAIL;
	}
#endif
	pDec->usFilterPageNum	= usPageNum;
	pDec->usFilterSubcode	= usSubcode;
	pDec->pfnReceiver		= pfnReceiver;


	pHeader = local_ttx_SearchPage(pDec, usPageNum, usSubcode, eTTX_SEARCH_NORMAL);
	if (pHeader == NULL)
	{
		return ERR_TTX_NO_PAGE;	// 현재 페이지가 케시에 저장되어 있지 않아도 OK
	}

	// 현재 필터가 걸려 있는 페이지의 TimeStamp를 다시 찍는다.
	ulTick	= VK_TIMER_GetSystemTick();
	pDec->cache.aTimeStamp[usPageNum].usPageNum		= usPageNum;
	pDec->cache.aTimeStamp[usPageNum].ulElapTick	= TTX_ELAPSED_TICK(pDec->cache.ulStartTick, ulTick);

	if (bRecvCached == FALSE)
	{
		return ERR_TTX_OK;
	}

	if (pfnReceiver == NULL)
	{
		return ERR_TTX_NO_RECEIVER;	// Receiver 가 없어도 OK
	}

	if (pHeader->ucOptionalField & eTTX_OPT_PAGE_DECODED		|| local_ttx_DecodePage(pDec, pHeader) == ERR_OK)
	{
		HxSTD_MemSet(&pDec->pageBuffer, 0, sizeof(TTX_Page_t));
		local_ttx_MakeTeletextPage(pDec, &pDec->pageBuffer, pHeader);

		pfnReceiver(&pDec->pageBuffer);
	}
	else	// Decoding FAIL!!
	{
		(void)local_ttx_EraseFromCache(pDec, pHeader->usPageNum, pHeader->usSubcode);
		return ERR_TTX_NO_PAGE;
	}

	return ERR_TTX_OK;
}

/**
    +---------------------------------+-----------+-----+----------------------------------------+
    |             Syntax              |No. of bits|Index|               Description              |
    +---------------------------------+-----------+-----+----------------------------------------+
    |teletext_page() {                |           |     |                                        |
    |    magic_code                   |     8     |  0  | 'M'                                    |
    |    version                      |     8     |  1  | reserved                               |
    |    number_of_packet             |     8     |  2  | number of packets                      |
    |    magazine_number              |     8     |  3  | magazine number(0~7, 0=8)              |
    |    page_number                  |     8     |  4  | page number (hex)                      |
    |    subcode                      |    16     |  5  | subcode                                |
    |    national_option              |     8     |  7  | latin national option subset           |
    |    control_bit                  |     8     |  8  | control bit(C11|C10|C9|C8|C7|C6|C5|C4) |
    |    has_magazine_default         |     8     |  9  | has magazine(M/29) default info        |
    |    if (has_magazine_default==1){|           |     |                                        |
    |        character_set_code       |     8     | 10  | @see ETSI EN 300 706 Table 32.         |
    |        character_set_code_2nd   |     8     | 11  |                                        |
    |        row color                |     8     | 12  | default row color                      |
    |        screen color             |     8     | 13  | default screen color                   |
    |    } else {                     |           |     |                                        |
    |        reserved                 |    32     | 10  | reserved                               |
    |    }                            |           |     |                                        |
    |    reserved_for_future_use      |    16     | 14  | reserved                               |
    |    CRC32                        |    32     | 16  | reserved                               |
    |    link_type                    |           | 20  | 'T': TOP, 'F': FLOF, else: default     |
    |    for (i = 0 ; i < 5 ; i++) {  |           |     |                                        |
    |        linked_magazine_number   |     8     | 21* | linked magazine number(0xff means NULL)|
    |        linked_page_number       |     8     | 22* | linked page number(hex)                |
    |        linked_subcode           |    16     | 23* | linked subcode                         |
    |    }                            |           |     |                                        |
    |                                 |           |     |                                        |
    |    for (i = 0 ; i < N ; i++) {  |           |     |                                        |
    |        packet_number            |     8     | 41* | packet number (Y)                      |
    |        data                     |   320     | 42* | packet data                            |
    |    }                            |           |     |                                        |
    |}                                |           |     |                                        |
    +---------------------------------+-----------+-----+----------------------------------------+
 */
HUINT32		TTX_DECODER_GetPage (Handle_t hDecoder, HUINT16 pageno, HUINT16 subcode, HUINT8 *buf, HUINT32 size)
{
	TTX_Decoder_t 	*pDec = (TTX_Decoder_t *)hDecoder;
	TTX_Header_t	*pHeader;
	TTX_Packet_t	*pPacket, *pRow24 = NULL;
	HINT32			i, j;
	HINT32			len = (HINT32)size;
	HUINT8			*meta, *count, *link;
	HINT32			offset;
	TTX_TOP_Link_t	aTop[4];
	TTX_FLOF_t		flof;
	TTX_DefaultInfo_t	info;

	if (hDecoder == HANDLE_NULL || pDec == NULL || pageno >= TTX_MAX_PAGE_NUM || buf == NULL)
	{
		return 0;
	}

	offset	= 1/*packet number*/ + TTX_PACKET_DATA_BYTES;
	if (len < offset)
	{
		return 0;	// need more memory
	}

	if (pDec->bEnabled == FALSE)
	{
		return 0;
	}

	pHeader = local_ttx_SearchPage(pDec, pageno, subcode, eTTX_SEARCH_NORMAL);
	if (pHeader == NULL)
	{
		return 0;
	}

	if (!(pHeader->ucOptionalField & eTTX_OPT_PAGE_DECODED))
	{
		if (local_ttx_DecodePage(pDec, pHeader) != ERR_OK)
			return 0;
	}

	//2 'M' meta info packet
	meta	= buf;
	link    = buf + 20;
	*meta++	= 'M';		// [0] meta packet designation
	HxSTD_MemSet(meta, 0, TTX_PACKET_DATA_BYTES);
	*meta++	= 0;		// [1] version (reserved for future use)
	count	= meta++;	// [2] packet count
	*meta++	= TTX_DEC_ADDR_TO_HEX_M(pHeader->usPageNum);	// [3]
	*meta++	= TTX_DEC_ADDR_TO_HEX_X(pHeader->usPageNum);	// [4]
	*meta++	= (pHeader->usSubcode >> 8) & 0xFF;	// [5]
	*meta++	= (pHeader->usSubcode >> 0) & 0xFF;	// [6]
	*meta++	= pHeader->ucNationalOption;		// [7] national option
	*meta++	= pHeader->ucControlBit;			// [8] control bits
	info	= pDec->aMagazineInfo[TTX_DEC_ADDR_TO_HEX_M(pHeader->usPageNum)];
	if (info.bUpdated)
	{
		*meta++	= 1;
		*meta++	= info.ucCharSetDesg;
		*meta++	= info.uc2ndCharSetDesg;
		*meta++	= info.ucRowColor;
		*meta++	= info.ucScreenColor;
	}

	buf += offset;
	len -= offset;
	*count  = 0;

	//2 Y=0~23, 26, 27, 28
	HxSTD_MemSet(&flof, 0, sizeof(TTX_FLOF_t));
	for (pPacket = (TTX_Packet_t *)pHeader ; pPacket ; )
	{
		if (len < offset)
			break;

		if (pPacket->ucY == 0)
		{
			*buf++ = pPacket->ucY;
			HxSTD_MemSet(buf, 0x20, TTX_HEADER_ADDR_BYTES);
			buf[3]	= (pHeader->usPageNum < 100) ? '8' : pHeader->usPageNum / 100 + '0';
			buf[4]	= (pHeader->usPageNum % 100) / 10 + '0';
			buf[5]	= (pHeader->usPageNum % 10 ) + '0';
			buf += TTX_HEADER_ADDR_BYTES;
			HxSTD_MemCopy(buf, pHeader->aucData, TTX_HEADER_DATA_BYTES);
			buf += TTX_HEADER_DATA_BYTES;
			len -= offset;
			(*count)++;
		}
		else if (pPacket->ucY < 24 || pPacket->ucY == 26 || pPacket->ucY == 28)
		{
			*buf = pPacket->ucY;
			HxSTD_MemCopy(buf + 1, pPacket->aucData, TTX_PACKET_DATA_BYTES);

			buf += offset, len -= offset;
			(*count)++;
		}
		else if (pPacket->ucY == 24)
		{
			pRow24 = pPacket;
		}
		else if (pPacket->ucY == 27)
		{
			pPacket = local_ttx_ParseFLOF(pHeader->usPageNum, &flof, pPacket);
			continue;
		}
		pPacket = pPacket->pNextPacket;
	}

	//2 Y=24 (Link)
	HxSTD_MemSet(aTop, 0, sizeof(TTX_TOP_Link_t) * 4);

	// check TOP
	if (pDec->hTOP != HANDLE_NULL
		&& TTX_TOP_GetTOPLink(pDec->hTOP, pHeader->usPageNum, aTop) == ERR_OK)
	{
		*link++	= 'T';
		*link++	= 1;	*link++	= 0;	*link++	= 0x3F;	*link++ = 0x7F;

		for (i = 0 ; i < 4 ; i++)
		{
			*link++ = TTX_DEC_ADDR_TO_HEX_M(aTop[i].usPageNum);
			*link++ = TTX_DEC_ADDR_TO_HEX_X(aTop[i].usPageNum);
			*link++	= (aTop[i].usSubcode >> 8) & 0xFF;	// [5]
			*link++	= (aTop[i].usSubcode >> 0) & 0xFF;	// [6]
		}
		if (len >= offset)
		{
			static const HUINT8	s_row24TOP[TTX_PACKET_DATA_BYTES] = {
				// 0,    1,    2,    3,    4,    5,    6,    7,    8,    9,
				0x01, 0x1D, 0x00,  '-', 0x20, 0x02, 0x1D, 0x00,  '+', 0x20,
				0x03, 0x1D, 0x00, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
				0x20, 0x20, 0x20, 0x20, 0x20, 0x06, 0x1D, 0x00, 0x20, 0x20,
				0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20
			};
			*buf++	= 24;	// Y/24
			HxSTD_MemCopy(buf, s_row24TOP, TTX_PACKET_DATA_BYTES);
			HxSTD_MemCopy(buf + 13, aTop[2].aucTitle, TOP_AIT_TITLE_LEN);
			HxSTD_MemCopy(buf + 28, aTop[3].aucTitle, TOP_AIT_TITLE_LEN);
			buf += TTX_PACKET_DATA_BYTES;
			len -= offset;
			(*count)++;
		}
	}
	// check FLOF
	else if (flof.bEnabled && flof.bLinkControl && pRow24)
	{
		*link++ = 'F';
		*link++	= TTX_DEC_ADDR_TO_HEX_M(flof.indexKey.usPageNum);
		*link++	= TTX_DEC_ADDR_TO_HEX_X(flof.indexKey.usPageNum);
		*link++	= (flof.indexKey.usSubcode >> 8) & 0xFF;
		*link++	= (flof.indexKey.usSubcode >> 0) & 0xFF;

		for (i = 0 ; i < 4 ; i++)
		{
			*link++ = TTX_DEC_ADDR_TO_HEX_M(flof.aColorKey[i].usPageNum);
			*link++ = TTX_DEC_ADDR_TO_HEX_X(flof.aColorKey[i].usPageNum);
			*link++	= (flof.aColorKey[i].usSubcode >> 8) & 0xFF;	// [5]
			*link++	= (flof.aColorKey[i].usSubcode >> 0) & 0xFF;	// [6]
		}
		if (len >= offset)
		{
			*buf	= 24;
			HxSTD_MemCopy(buf + 1, pRow24->aucData, TTX_PACKET_DATA_BYTES);
			buf += offset, len -= offset;
			(*count)++;
		}
	}
	// default
	else
	{
		HUINT16	ctt[4];

		local_ttx_SearchCTT(pDec, pHeader->usPageNum, ctt);
		*link++ = 'C';
		*link++	= 1;
		*link++	= 0;
		*link++	= 0x3F;
		*link++ = 0x7F;

		for (i = 0 ; i < 4 ; i++)
		{
			*link++ = TTX_DEC_ADDR_TO_HEX_M(ctt[i]);
			*link++ = TTX_DEC_ADDR_TO_HEX_X(ctt[i]);
			*link++	= 0x3F;
			*link++ = 0x7F;
		}
		if (len >= offset)
		{
			static const HUINT8	s_row24CTT[TTX_PACKET_DATA_BYTES] = {
				// 0,    1,    2,    3,    4,    5,    6,    7,    8,    9,
				0x01, 0x1D, 0x00, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x02,
				0x1C, 0x1D, 0x00, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x03,
				0x1C, 0x1D, 0x00, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x06,
				0x1C, 0x1D, 0x00, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20
			};

			*buf++	= 24;	// Y/24
			HxSTD_MemCopy(buf, s_row24CTT, TTX_PACKET_DATA_BYTES);

			for (i = 0, j = 4 ; i < 4 ; i++, j += 10)
			{
				buf[j + 0]	= (ctt[i] < 100) ? '8' : ctt[i] / 100 + '0';
				buf[j + 1]	= (ctt[i] % 100) / 10 + '0';
				buf[j + 2]	= (ctt[i] % 10 ) + '0';
			}
			buf += TTX_PACKET_DATA_BYTES;
			len -= offset;
			(*count)++;
		}
	}
	return size - (HUINT32)len;
}


//2 Search
TTX_Result_t	TTX_DECODER_Search (Handle_t hDecoder, const HUINT16 usPageNum, const HUINT16 usSubcode,
									const TTX_Search_t search, HUINT16 *pusResPageNum, HUINT16 *pusResSubcode)
{
	TTX_Decoder_t	*pDec		= (TTX_Decoder_t *)hDecoder;
	TTX_Header_t	*pHeader	= NULL;

	if (hDecoder == HANDLE_NULL || pDec == NULL || usPageNum >= TTX_MAX_PAGE_NUM
		|| pusResPageNum == NULL || pusResSubcode == NULL)
	{
		return ERR_TTX_INVALID_ARGUMENT;
	}

	if (pDec->bEnabled == FALSE)
	{
		return ERR_TTX_DISABLED;
	}

	pHeader = local_ttx_SearchPage(pDec, usPageNum, usSubcode, search);
	if (pHeader == NULL)
	{
		return ERR_TTX_NO_PAGE;
	}

	*pusResPageNum	= pHeader->usPageNum;
	*pusResSubcode	= pHeader->usSubcode;

	return ERR_TTX_OK;
}


//2 Rendering Page
TTX_Result_t	TTX_DECODER_Rendering	(Handle_t hDecoder, TTX_Page_t *pPage, HBOOL bRollingHeader, ...)
{

	TTX_Decoder_t 	*pDec 	= (TTX_Decoder_t *)hDecoder;
	TTX_Result_t	result;
	HUINT32			ulVersion;
	va_list			va;

	if (hDecoder == HANDLE_NULL || pDec == NULL || pPage == NULL)
	{
		return ERR_TTX_INVALID_ARGUMENT;
	}

	if (pDec->bEnabled == FALSE)
	{
		return ERR_TTX_DISABLED;
	}

	(void)local_ttx_MakeCTT(pDec, pPage);
	(void)local_ttx_MakeSubcodeBar(pDec, pPage);
	ulVersion	= TTX_TOP_GetCurVersion(pDec->hTOP);
	if (ulVersion != 0)
	{
		// Update TOP Info
		if (pPage->TOP.bEnabled == TRUE && pPage->TOP.ulVersion != ulVersion)
		{
			pPage->TOP.ulVersion	= ulVersion;
			TTX_TOP_GetTOPLink(pDec->hTOP, pPage->usPageNum, pPage->TOP.aLink);
		}
	}
	HxSTD_MemCopy(&pDec->pageBuffer, pPage, sizeof(TTX_Page_t));

	// Update Header
	if (bRollingHeader == TRUE)
	{
		local_ttx_MakeRollingHeader(pDec, &pDec->pageBuffer);
	}

	local_ttx_MakeTeletextTime(pDec, &pDec->pageBuffer);

	va_start(va, bRollingHeader);
	result = TTX_GFX_RenderingPage(&pDec->pageBuffer, va);
	va_end(va);

	return result;
}


//2 TTX Flush
TTX_Result_t	TTX_DECODER_Flush (Handle_t hDecoder)
{
	TTX_Decoder_t	*pDec		= (TTX_Decoder_t *)hDecoder;
	HUINT32			ulTick;
	HUINT32			ulRecvTick;
	HUINT32			i;
	TTX_Header_t	*pHeader	= NULL;

	if (hDecoder == HANDLE_NULL || pDec == NULL)
	{
		return ERR_TTX_INVALID_ARGUMENT;
	}
#if 0
	if (pDec->hGFX == HANDLE_NULL)
	{
		return ERR_TTX_DISABLED;
	}
#endif
	ulTick	= VK_TIMER_GetSystemTick();

	for (i = 0 ; i < TTX_MAX_MAGAZINE ; i++)
	{
		pHeader		= pDec->aPipe[i].pHeader;
		ulRecvTick	= pDec->aPipe[i].ulRecvTick;

		if (	pHeader == NULL
			||	pHeader->pNextPacket == NULL
			||	!(pHeader->ucControlBit & (eTTX_C5_NEWSFLASH | eTTX_C6_SUBTITLE))
			||	ulRecvTick == 0
			||	pDec->pfnReceiver == NULL
			||	TTX_ELAPSED_TICK(ulRecvTick, ulTick) < TTX_MAX_TX_INTERVAL * 2
			||	pDec->usFilterPageNum != pHeader->usPageNum
			|| (pDec->usFilterSubcode != TTX_IGNORE_SUBCODE
				&& pDec->usFilterSubcode != pHeader->usSubcode)
			)
		{
			continue;
		}

		HxLOG_Print("[TTX_DECODER_Flush] Flush:%03d-%04X\n", pHeader->usPageNum, pHeader->usSubcode);

		if (pHeader->ucOptionalField & eTTX_OPT_PAGE_DECODED || local_ttx_DecodePage(pDec, pHeader) == ERR_OK)
		{
			HxSTD_MemSet(&pDec->pageBuffer, 0, sizeof(TTX_Page_t));
			local_ttx_MakeTeletextPage(pDec, &pDec->pageBuffer, pHeader);

			pDec->pfnReceiver(&pDec->pageBuffer);

			TTX_BITCLR(pHeader->ucOptionalField, eTTX_OPT_PAGE_DECODED);
		}
		else	// Decoding FAIL!!
		{
			(void)local_ttx_DeleteSubPage(pDec, pHeader);
			HxSTD_MemSet(&(pDec->aPipe[i]), 0, sizeof(TTX_DemuxPipe_t));
		}
	}

	return ERR_TTX_OK;
}


//2 Get Initial Page
TTX_Result_t	TTX_DECODER_InitPage (Handle_t hDecoder, HUINT16 *pusPageNum, HUINT16 *pusSubcode)
{
	TTX_Decoder_t	*pDec		= (TTX_Decoder_t *)hDecoder;

	if (hDecoder == HANDLE_NULL || pDec == NULL || pusPageNum == NULL || pusSubcode == NULL)
	{
		return ERR_TTX_INVALID_ARGUMENT;
	}

	*pusPageNum	= pDec->usInitPageNum;
	*pusSubcode	= pDec->usInitSubcode;

	return ERR_TTX_OK;
}


//2 Set User Defined Language Set
TTX_Result_t	TTX_DECODER_SetLanguage (Handle_t hDecoder, const HUINT8 ucNosIndex,
										const HUINT8 ucG0, const HUINT8 ucG2, const HUINT8 ucNOS)
{
	TTX_Decoder_t	*pDec		= (TTX_Decoder_t *)hDecoder;

	if (hDecoder == HANDLE_NULL || pDec == NULL || ucNosIndex > eTTX_NOS_MAX)
	{
		return ERR_TTX_INVALID_ARGUMENT;
	}

	if ( (/*ucG0 < eTTX_CODE_G0_LATIN ||*/ eTTX_CODE_G0_HEBREW < ucG0)
		|| (ucG2 < eTTX_CODE_G2_LATIN || eTTX_CODE_G2_ARBIC < ucG2)
		|| (eTTX_NOS_LAST < ucNOS) )
	{
		return ERR_TTX_INVALID_ARGUMENT;
	}

	pDec->aUserDefinedChar[ucNosIndex].bDefined	= TRUE;
	pDec->aUserDefinedChar[ucNosIndex].set.ucG0Set	= ucG0;
	pDec->aUserDefinedChar[ucNosIndex].set.ucG2Set	= ucG2;
	pDec->aUserDefinedChar[ucNosIndex].set.ucNationalOption	= ucNOS;

	return ERR_TTX_OK;
}


//2 TTX Stream Capture
#if defined(CONFIG_DEBUG)
TTX_Result_t	TTX_DECODER_StreamCapture (Handle_t hDecoder, const HINT8 *szFileName)
{
	TTX_Decoder_t 	*pDec 	= (TTX_Decoder_t *)hDecoder;

	HxLOG_Print("[TTX_DECODER_StreamCapture] Start\n");

	if (hDecoder == HANDLE_NULL || pDec == NULL || szFileName == NULL)
	{
		return ERR_TTX_INVALID_ARGUMENT;
	}

	if (pDec->fpCapture)	// 이미 켑쳐 중..
	{
		HxLOG_Print("[TTX_DECODER_StreamCapture] Decoder already capture the stream!!\n");
		return ERR_TTX_FAIL;
	}

	pDec->fpCapture	= fopen(szFileName, "wb");
	if (pDec->fpCapture == NULL)
	{
		HxLOG_Print("[TTX_DECODER_StreamCapture] File(%s) Open Fail!!\n", szFileName);
		return ERR_TTX_FAIL;
	}

	return ERR_TTX_OK;
}
#endif


//1 LOCAL FUNCTION DEFINITIONS
#define ____TTX_LOCAL_FUNC_DEFINITIONS____


#define	__def__ttx_demux____


/**
 * 총 45byte의 Teletext 패킷 중 Clock run-in(2 bytes)과 Framing code(1 byte)를 제외한
 * 나머지 42bytes 데이터를 디먹싱!!
 *
 * @param 	pDec		Instance of 'TTX Decoder'
 * @param 	pucPacket	TTX Packet (total 42bytes)
 * @return 	ERR_OK / ERR_FAIL
 */
static HERROR local_ttx_DemuxTTXPacket (TTX_Decoder_t *pDec, HUINT8 *pucPacket)
{
	HUINT8			ucM;	// Magazine
	HUINT8			ucY;	// Packet number

	if (TTX_ERRCHK_Hamming84(pucPacket, pucPacket, 2) != 0)
	{
		return ERR_FAIL;
	}
	// parse magazine and packet number
	ucM = pucPacket[0] & 0x07;
	ucY = (pucPacket[0] >> 3) | (pucPacket[1] << 1);

	// Skip Teletext Packet Address(Magazine, Packet number).
	pucPacket += 2;
#if defined(CONFIG_DEBUG)
	if (g_MwTtxDec_Level & eTTX_DBG_LV_PKTMONITOR)
	{
		local_ttx_DebugPacketMonitor(pDec, ucM, ucY, pucPacket);
	}
#endif

#if TTX_WORKAROUND_04 == TTX_ENABLE
	if (ucM == 0 && ucY == 25 && pDec->pCurPipe && pDec->pCurPipe->pHeader)
	{
		if (!(pDec->pCurPipe->pHeader->ucOptionalField & eTTX_OPT_DATA_PAGE))
			pDec->cache.ulCount++;	// 아래 함수에서 cache count를 줄이기 때문에, 미리 증가시키자...(좋지 않은 코드다ㅠㅠ)

		local_ttx_DeleteSubPage(pDec, pDec->pCurPipe->pHeader);
		HxSTD_MemSet(pDec->pCurPipe, 0, sizeof(TTX_DemuxPipe_t));

		return ERR_FAIL;
	}
	pDec->pCurPipe	= &(pDec->aPipe[ucM]);
#endif
	if (ucY == 0)		// header packet
	{
		return local_ttx_DemuxHeaderPacket(pDec, ucM, pucPacket);
	}
	else if (ucY <= 28)	// Page related packets
	{
		return local_ttx_DemuxY1toY28(pDec, ucM, ucY, pucPacket);
	}
	else if (ucY == 29)	// Magazine related packets
	{
		if (local_ttx_DecodePacket(pucPacket, eTTX_CODING_HAMMING2418) != 0)
		{
			return ERR_FAIL;
		}
		HxLOG_Info("[%s] Recieve M(%d)/29(%X)!!\n", __FUNCTION__, ucM, pucPacket[0]);

		if (pucPacket[0] != 0 && pucPacket[0] != 4) // designation code will be 1 or 4
		{
			return ERR_FAIL;
		}
		return local_ttx_MakeDefaultInfo(&pDec->aMagazineInfo[ucM], pucPacket);
	}
	else if (ucY == 30)
	{
		return local_ttx_DemuxServiceDataPacket(pDec, pucPacket);
	}
	else
	{
		return ERR_FAIL;
	}
}


/**
 * Header 패킷을 디먹싱
 *
 * @param 	pDec		Instance of 'TTX Decoder'
 * @param	ucM			Current Magazine
 * @param 	pucHeader	TTX Header Packet (total 40bytes)
 * @return 	ERR_OK / ERR_FAIL
 */
static HERROR local_ttx_DemuxHeaderPacket (TTX_Decoder_t *pDec, const HUINT8 ucM, HUINT8 *pucHeader)
{
	TTX_Header_t	*pHeader	= NULL;
	HUINT8			ucNationalOpt;
	if (TTX_ERRCHK_Hamming84(pucHeader, pucHeader, TTX_HEADER_ADDR_BYTES) != 0)
	{
		return ERR_FAIL;
	}

	if (local_ttx_IsHeaderDuplicated(pDec, ucM, pucHeader))
	{
		return ERR_FAIL;
	}
	HxSTD_MemCopy(pDec->aucHeaderDupCheck, pucHeader, TTX_HEADER_ADDR_BYTES);

	// Parity 에러는 발생하더라도 무시한다. 최신 헤더는 항상 주시한다.
	if (TTX_ERRCHK_OddParity(&pucHeader[8], &pucHeader[8], TTX_HEADER_DATA_BYTES) == 0)
	{
		if (pucHeader[0] <= 0x09 && pucHeader[1] <= 0x09 		// Data Page또는 Subtitle, Newsflash, Suppress Header,
			&& !(pucHeader[5] & 0x0C || pucHeader[6] & 0x05) )	// Interrupted Sequence는 Rolling Header에서 제외시킨다.
		{
			HxSTD_MemCopy(pDec->aucRollingHeader, &pucHeader[8], TTX_HEADER_DATA_BYTES);
		}

		(void)local_ttx_SetTeletextTime(pDec, pucHeader,
			(pucHeader[0] == 0x0F && pucHeader[1] == 0x0F) ? eTTX_TIME_BY_TIME_HEADER : eTTX_TIME_BY_HEADER);
	}

	if (local_ttx_TerminatePage(pDec, ucM,
								(pucHeader[1] << 4) | pucHeader[0], 	// ucX
								(pucHeader[7] & 0x01) ? TRUE : FALSE	// Magazine Serial
							) != ERR_OK)
	{
		return ERR_FAIL;
	}

	// Time Filling Header
	if (pucHeader[0] == 0x0F && pucHeader[1] == 0x0F)
	{
		return ERR_OK;
	}

	pHeader	= (TTX_Header_t *)local_ttx_NewPacket(pDec);
	if (pHeader == NULL)
	{
		return ERR_FAIL;
	}

	pHeader->ucY		= 0;
	// Basic Level 1 Teletext page (LOP)
	if (pucHeader[0] <= 0x09 && pucHeader[1] <= 0x09)
	{
		pHeader->usPageNum	= ucM * 100 + pucHeader[1] * 10 + pucHeader[0];
	}
	// The other page (Data pages..., GPOP, POP, GDRCS, DRCS, MOT, MIP, BTT, AIT, MPT, MPT-EX)
	else
	{
		pHeader->usPageNum	= ((ucM & 0x0F) << 8) | ((pucHeader[1] & 0x0F) << 4) | (pucHeader[0] & 0x0F);
		TTX_BITSET(pHeader->ucOptionalField, eTTX_OPT_DATA_PAGE);
	}
	pHeader->usSubcode		=  TTX_SUBCODE(pucHeader);
	pHeader->ucControlBit	= ((pucHeader[3] & 0x08) >> 3)	// C4
							| ((pucHeader[5] & 0x0C) >> 1)	// C6, C5
							| ((pucHeader[6] & 0x0F) << 3)	// C10, C9, C8, C7
							| ((pucHeader[7] & 0x01) << 7); // C11

	// National Option Character Subset(C14, C13, C12)
	TTX_ERRCHK_ReverseBit(&ucNationalOpt, &pucHeader[7], 1);
	pHeader->ucNationalOption	= (ucNationalOpt >> 4) & 0x07;

	HxSTD_MemCopy(pHeader->aucData, &pucHeader[8], TTX_HEADER_DATA_BYTES);

	TTX_BITSET(pHeader->ucOptionalField, eTTX_OPT_PACKET_DECODED);

	pDec->aPipe[ucM].pHeader	= pHeader;
	pDec->aPipe[ucM].pLast		= (TTX_Packet_t *)pHeader;
	pDec->aPipe[ucM].ulRecvTick	= VK_TIMER_GetSystemTick();
	pDec->aPipe[ucM].ulCount	= 1;

	return ERR_OK;
}


/**
 * Page related packet
 * +--------+-----------------------------------------------------------------------+
 * |    Y   |     Field                                                             |
 * +--------+-----------------------------------------------------------------------+
 * | 1 - 25 | 40 Data bytes                                                         |
 * | 26, 28 | 1 Designation code + 13 Triplets(39 bytes)                            |
 * | 27     | 1 Designation code + 6 Link(36 bytes) + 1 Link Control + CRC(2 bytes) |
 * +--------+-----------------------------------------------------------------------+
 *
 * @param pDec		Instance of 'TTX Decoder'
 * @param ucM		Magazine number
 * @param ucY		Packet number
 * @param pucPacket	Decoded Teletext Packet (total 40 bytes)
 * @see Figure 10, 11, 12 (ETS 300 706)
 */
static HERROR local_ttx_DemuxY1toY28 (TTX_Decoder_t *pDec, const HUINT8 ucM, const HUINT8 ucY, HUINT8 *pucPacket)
{
	TTX_Packet_t	*pPkt = NULL;

	if (pDec->aPipe[ucM].pHeader == NULL)
	{
		return ERR_FAIL;
	}

	pPkt = (TTX_Packet_t *)local_ttx_NewPacket(pDec);
	if (pPkt == NULL)
	{
		return ERR_FAIL;
	}

	if (26 <= ucY && ucY <= 28)	// has designation code
	{
		if (TTX_ERRCHK_Hamming84(&(pPkt->ucDesgCode), pucPacket, 1) != 0)
		{
			local_ttx_DeletePacket(pDec, pPkt);
			return ERR_FAIL;
		}
	}

	pPkt->ucY	= ucY;
	pPkt->pNextPacket	= NULL;
	HxSTD_MemCopy (pPkt->aucData, pucPacket, TTX_PACKET_DATA_BYTES);

	pDec->aPipe[ucM].pLast->pNextPacket	= pPkt;
	pDec->aPipe[ucM].pLast		= pPkt;
	pDec->aPipe[ucM].ulCount++;

	return ERR_OK;
}


/**
 * Broadcast Service Data Packets
 *
 * @param pDec		Instance of 'TTX Decoder'
 * @param pucPacket	Decoded Teletext Packet (total 40 bytes)
 * @see Figure 15, 16 (ETS 300 706)
 */
static HERROR local_ttx_DemuxServiceDataPacket (TTX_Decoder_t *pDec, HUINT8 *pucPacket)
{
	HUINT8	ucM;

	if (TTX_ERRCHK_Hamming84(pucPacket, pucPacket, 7) != 0
		|| TTX_ERRCHK_OddParity(pucPacket + 20, pucPacket + 20, 20) != 0)
	{
		return ERR_FAIL;
	}

	ucM		= (HUINT8)(((pucPacket[4] & 0x8) >> 3) | ((pucPacket[6] & 0xC) >> 1));
	pDec->usInitPageNum	= ucM * 100 + pucPacket[2] * 10 + pucPacket[1];
	pDec->usInitSubcode	= (HUINT16)(((pucPacket[3] & 0xF) << 0)
								| ((pucPacket[4] & 0x7) << 4)
								| ((pucPacket[5] & 0xF) << 8)
								| ((pucPacket[6] & 0x3) << 12));


	switch (pucPacket[0])	// designation code
	{
	case 0:	case 1:	// Format 1
		TTX_ERRCHK_ReverseBit(pucPacket + 9, pucPacket + 9, 11);
		/**
		 * ETSI TR 100 287에 보면, 10.14 Date and currTime setting에
		 * 8/30/1을 통해 들어오는 MJD/UTC 를 강추하고 있다.....
		 * 그러나...!!! 보내주는 사람 마음이다. ㅡㅡ;;
		 */
		// TODO: 이 시간은 문제가 많았다... 우선 SKIP!!
		//(void)local_ttx_SetTeletextTime(pDec, pucPacket, eTTX_TIME_BY_MJD_UTC);

		// Set Network Identification Code
		pDec->aucNI[0]	= pucPacket[7];
		pDec->aucNI[1]	= pucPacket[8];
		break;

	case 2:	case 3:	// Format 2
		return ERR_FAIL;	// TODO: Not implemented yet...

	default:
		return ERR_FAIL;
	}
	return ERR_OK;
}


/**
 * 연속해서 들어온 두 개의 같은 헤더에 대한 중복 여부를 검사
 *
 * @param	pDec		Instance of 'TTX Decoder'
 * @param	ucM			Magazine number
 * @param	pucHeader	Decoded Teletext New Header Packet
 * @return	TRUE / FALSE
 */
static HBOOL local_ttx_IsHeaderDuplicated (TTX_Decoder_t *pDec, const HUINT8 ucM, const HUINT8 *pucNewHeader)
{
	HUINT32			ulTick;
	TTX_DemuxPipe_t *pPipe;

	pPipe	= &pDec->aPipe[ucM];

	if (pPipe->pHeader == NULL || (TTX_Packet_t *)pPipe->pHeader != pPipe->pLast)	// 헤더만 들어왔고,
	{
		return FALSE;
	}

	ulTick	= VK_TIMER_GetSystemTick();
	if (TTX_ELAPSED_TICK(pPipe->ulRecvTick, ulTick) > TTX_MAX_TX_INTERVAL)	// 시간이 0.1초 이내에,
	{
		return FALSE;
	}

	if (HxSTD_MemCmp(pDec->aucHeaderDupCheck, pucNewHeader, TTX_HEADER_ADDR_BYTES) != 0)	// 중복된다면?
	{
		return FALSE;
	}
	return TRUE;
}

/**
 * Header Packet으로 부터 시간을 얻어온다.
 *
 * @param	pDec		Instance of 'TTX Decoder'
 * @param	pucHeader	Decoded Teletext Header Packet
 * @return	ERR_OK/ERR_FAIL
 */
static HERROR local_ttx_SetTeletextTime (TTX_Decoder_t *pDec, const HUINT8 *pucPacket, const TTX_TimeSetBy_t eTimeSetBy)
{
	UNIXTIME		unixTime	= 0;
	TIME_T			currTime;
	const HUINT8	*pucTime	= NULL;
	TTX_Time_t		*pTtxTime	= &pDec->ttxTime;

	if (eTimeSetBy == eTTX_TIME_BY_NONE || eTimeSetBy < pTtxTime->eSetBy)
	{
		return ERR_FAIL;
	}

	HxSTD_MemSet(&currTime, 0, sizeof(TIME_T));

	switch (eTimeSetBy)
	{
	case eTTX_TIME_BY_HEADER:
	case eTTX_TIME_BY_TIME_HEADER:
		pucTime	= &pucPacket[32];
		if (!isdigit(pucTime[0]) || !isdigit(pucTime[1]) 		// HH
			|| !isdigit(pucTime[3]) || !isdigit(pucTime[4]) 	// MM
			|| !isdigit(pucTime[6]) || !isdigit(pucTime[7]))	// SS
		{
			return ERR_FAIL;
		}
		currTime.ucHour		= (pucTime[0] - '0') * 10 + (pucTime[1] - '0');
		currTime.ucMinute	= (pucTime[3] - '0') * 10 + (pucTime[4] - '0');
		currTime.ucSecond	= (pucTime[6] - '0') * 10 + (pucTime[7] - '0');
		break;

	case eTTX_TIME_BY_MJD_UTC:
		if (UTIL_TCONV_ConvertBcdTimeToDecimalTime((HUINT8 *)&pucPacket[13], 3, &currTime) != ERR_OK)
		{
			return ERR_FAIL;
		}
		break;
	case eTTX_TIME_BY_SYSTEM:
		if (VK_CLOCK_GetTime((void *)&unixTime) != ERR_OK
			|| UTIL_TCONV_ConvertUnixTimeToTime(unixTime, &currTime) != ERR_OK)
		{
			return ERR_FAIL;
		}
		break;

	default:
		return ERR_FAIL;
	}

	if (pTtxTime->time.ucHour < currTime.ucHour
		|| pTtxTime->time.ucMinute < currTime.ucMinute
		|| pTtxTime->time.ucSecond < currTime.ucSecond)
	{
		pTtxTime->eSetBy		= eTimeSetBy;
		pTtxTime->time			= currTime;
		pTtxTime->ulLastTick	= VK_TIMER_GetSystemTick();
		return ERR_OK;
	}
	return ERR_FAIL;
}


/**
 * 페이지 넘버가 +-ulRange인지 여부 판단
 *
 * @param	usBase	base addr
 * @param	usAddr	addr
 * @param	ulRange	range
 * @return	TRUE / FALSE
 */
static HBOOL local_ttx_IsNearPageAddr (const HUINT16 usBase, const HUINT16 usAddr, const HUINT32 ulRange)
{
	HINT32	nMxxA	= (HINT32)((usBase < 100) ? usBase + 800 : usBase);
	HINT32	nMxxB	= (HINT32)((usAddr < 100) ? usAddr + 800 : usAddr);
	HINT32	nAbs	= nMxxA - nMxxB;

	if (nAbs < 0)
		nAbs	= -nAbs;

	if (nAbs <= (HINT32)ulRange || TTX_MAX_PAGE_NUM - (HINT32)ulRange <= nAbs)
	{
		return TRUE;
	}
	return FALSE;
}


/**
 * 조건에 따라 pPage가 수용 가능한지 체크
 *
 * @param 	pDec	Instance of 'TTX Decoder'
 * @param 	pHeader	TTX_HeaderPacket
 * @return	TRUE / FALSE
 */
static HBOOL local_ttx_IsAcceptablePage (TTX_Decoder_t *pDec, const TTX_Header_t *pHeader)
{
	HERROR	err;
	TTX_Header_t	*pCached = NULL;

	// Cache에 저장된 페이지가 존재
	pCached	= local_ttx_FetchFromCache(pDec, pHeader->usPageNum, pHeader->usSubcode);
	if (pCached != NULL)
	{
		return TRUE;
	}

	if (pDec->cache.bHungry == TRUE
		|| local_ttx_IsNearPageAddr(pDec->usFilterPageNum, pHeader->usPageNum, 5) == TRUE)
	{
		return TRUE;
	}

	// TOP BTT내에 존재하는 페이지를 받는다.
	err	= TTX_TOP_IsPageExist(pDec->hTOP, pHeader->usPageNum, pHeader->usSubcode);
	if (err != ERR_TOP_INVALID_ARGUMENT && err != ERR_TOP_NO_BTT)	// if (TOP is not ready)
	{
		return (err == ERR_OK) ? TRUE : FALSE;
	}

	if (pHeader->usPageNum % 10 == 0)	// M00 or Mx0
	{
		return TRUE;
	}

	// 마지막에 외부로부터 요청된 페이지 넘버의 +-15의 오차 범위 내로 들어오면 받아들인다.
	if (local_ttx_IsNearPageAddr(pDec->usFilterPageNum, pHeader->usPageNum, 15) == TRUE)
	{
		return TRUE;
	}

	return FALSE;
}


#if TTX_WORKAROUND_05 != TTX_ENABLE
/**
 * 페이지의 타입에 따라 같은 페이지 번호(Magazine 제외)인지 검사
 *
 * @param	pHeader	페이지 헤더
 * @param	ucX		HEX로 구성된 페이지 넘버
 * @return 	TRUE/FALSE
 */
static HBOOL local_ttx_IsSamePageNumber (const TTX_Header_t *pHeader, const HUINT8 ucX)
{
	// Header == Data Page, Data Page의 Header는 usPageNum을 HEX로 갖고 있는다.
	if (pHeader->ucOptionalField & eTTX_OPT_DATA_PAGE)
	{
		return ((HUINT8)(pHeader->usPageNum & 0x00FF) == ucX) ? TRUE : FALSE;
	}

	// ucX == Data Page
	if ((ucX >> 4) > 0x09 || (ucX & 0x0F) > 0x09)
	{
		return FALSE;
	}

	if ((pHeader->usPageNum % 100) != ((ucX >> 4) * 10 + (ucX & 0x0F)))
	{
		return FALSE;
	}

	return TRUE;
}
#endif

/**
 * 새로 들어온 헤더의 MXX(ucM,ucX)를 통해 Magazine Pipe에 걸려 있는 페이지의 종료를 체크한다.
 *
 * @param	pDec	Instance of 'TTX Decoder'
 * @param	ucM		새로 받은 메거진 넘버
 * @param	ucX		새로 받은 페이지 넘버
 * @param	bSerial	Magazine Serial
 * @return	ERR_OK / ERR_FAIL (어떠한 페이지도 종료시키지 못했다.)
 */
static HERROR	local_ttx_TerminatePage (TTX_Decoder_t *pDec, const HUINT8 ucM, const HUINT8 ucX, const HBOOL bSerial)
{
	HUINT8		ucMagazine		= 0;
	HUINT32		ulTick			= 0;
	HBOOL		bSamePageNo		= FALSE;
	HBOOL		bPageTerminated	= FALSE;
	TTX_Header_t	*pHeader	= NULL;

	ulTick	= VK_TIMER_GetSystemTick();

	for (ucMagazine = 0 ; ucMagazine < TTX_MAX_MAGAZINE ; ucMagazine++)
	{
		pHeader	= pDec->aPipe[ucMagazine].pHeader;
		if (pHeader == NULL)
		{
			continue;
		}

#if TTX_WORKAROUND_05 == TTX_ENABLE
		bSamePageNo	= FALSE;
#else
		/* +-----------------------------------------------------------------------+
		 * | The transmission of a page is terminated by the next page header with |
		 * |   Serial : different page number                                      |
		 * | Parallel : different page number but the same magazine number         |
		 * +-----------------------------------------------------------------------+ */
		bSamePageNo	= local_ttx_IsSamePageNumber(pHeader, ucX);
#endif
		if (pHeader->ucControlBit & eTTX_C11_MAGAZINE_SERIAL && bSerial == TRUE)
		{
			bPageTerminated	= !bSamePageNo;
		}
		else
		{
			bPageTerminated	= (ucMagazine == ucM) ? !bSamePageNo : FALSE;
		}

		if (bPageTerminated == FALSE
#if TTX_WORKAROUND_03 == TTX_ENABLE
			&& pDec->aPipe[ucMagazine].ulCount < TTX_MAX_PACKET_OF_PAGE
#endif
			&& TTX_ELAPSED_TICK(pDec->aPipe[ucMagazine].ulRecvTick, ulTick) < TTX_MAX_TX_INTERVAL * 3
		) {
			continue;
		}

		if (local_ttx_ReceivePage(pDec, pHeader) == ERR_TTX_DELETE)
		{
			local_ttx_DeleteSubPage(pDec, pHeader);
			pDec->cache.ulCount++;
		}

		HxSTD_MemSet(&(pDec->aPipe[ucMagazine]), 0, sizeof(TTX_DemuxPipe_t));
	}

	// Receive same header(same Page number)
	if (pDec->aPipe[ucM].pHeader != NULL)
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}


/**
 * 등록된 Receiver에 새로 들어온 페이지를 Notify
 *
 * @param	pDec		Instance of 'TTX Decoder'
 * @param	pHeader		TTX Header Packet
 * @return	ERR_TTX_OK/ERR_TTX_FAIL/ERR_TTX_DECODE_FAIL
 */
static TTX_Result_t	local_ttx_NotifyToReceiver (TTX_Decoder_t *pDec, TTX_Header_t *pHeader)
{
	HERROR	err = ERR_OK;

	if (pDec->pfnReceiver == NULL
		|| pDec->usFilterPageNum != pHeader->usPageNum
		|| (pDec->usFilterSubcode != TTX_IGNORE_SUBCODE
			&& pDec->usFilterSubcode != pHeader->usSubcode))
	{
		return ERR_TTX_FAIL;
	}

	if (!(pHeader->ucOptionalField & eTTX_OPT_PAGE_DECODED))
	{
		err = local_ttx_DecodePage(pDec, pHeader);
	}

	if (err == ERR_FAIL)	// Decode Fail!!
	{
		return ERR_TTX_DECODE_FAIL;
	}

	HxSTD_MemSet(&pDec->pageBuffer, 0, sizeof(TTX_Page_t));
	local_ttx_MakeTeletextPage(pDec, &pDec->pageBuffer, pHeader);

	pDec->pfnReceiver(&pDec->pageBuffer);
	return ERR_TTX_OK;
}

#define	__def__ttx_memory_management____


/**
 * 새로 페이지를 받았을 때 처리 루틴
 *
 * @param 	pDec	Instance of 'TTX Decoder'
 * @param 	pHeader	방금 만들어진 따끈따끈한 Header 및 그에 딸려온 패킷들~
 * @return	ERR_TTX_DELETE / ERR_TTX_OK
 */
static TTX_Result_t local_ttx_ReceivePage (TTX_Decoder_t *pDec, TTX_Header_t *pHeader)
{
	TTX_PageFunction_t	eFunc;

	eFunc	= local_ttx_GetPageFunction(pDec, pHeader);
	switch (eFunc)
	{
	case eTTX_FUNCTION_LOP:					return local_ttx_ReceiveLOP(pDec, pHeader);
	case eTTX_FUNCTION_DATA_BROADCASTING:	break;
	case eTTX_FUNCTION_GPOP:				break;
	case eTTX_FUNCTION_POP:					break;
	case eTTX_FUNCTION_GDRCS:				break;
	case eTTX_FUNCTION_DRCS:				break;
	case eTTX_FUNCTION_MOT:					break;
	case eTTX_FUNCTION_MIP:					break;
	case eTTX_FUNCTION_TOP_BTT:
	case eTTX_FUNCTION_TOP_AIT:
	case eTTX_FUNCTION_TOP_MPT:
	case eTTX_FUNCTION_TOP_MPT_EX:			return local_ttx_ReceiveTOP(pDec, pHeader);
	default:	break;
	}
	return ERR_TTX_DELETE;
}


/**
 * 새로 LOP 페이지를 받았을 때 처리 루틴
 *
 * @param 	pDec	Instance of 'TTX Decoder'
 * @param 	pHeader	방금 만들어진 따끈따끈한 Header 및 그에 딸려온 패킷들~
 * @return	ERR_TTX_DELETE / ERR_TTX_OK
 */
static TTX_Result_t local_ttx_ReceiveLOP (TTX_Decoder_t *pDec, TTX_Header_t *pHeader)
{
	HBOOL			bAcceptable;
	HERROR			err			= ERR_OK;
	TTX_Result_t	ttxErr		= ERR_TTX_FAIL;
	TTX_Header_t	*pCached	= NULL;

#if defined(CONFIG_DEBUG)
	if (pDec->usFilterPageNum == pHeader->usPageNum && g_MwTtxDec_Level & eTTX_DBG_LV_CURPAGE)
	{
		(void) local_ttx_DecodePage(pDec, pHeader);
		(void) local_ttx_DebugPage(pHeader, NULL);
	}
#endif

	bAcceptable	= local_ttx_IsAcceptablePage(pDec, pHeader);
	if (bAcceptable == FALSE)
	{
		return ERR_TTX_DELETE;
	}

	if (pHeader->ucControlBit & eTTX_C4_ERASE_PAGE)
	{
		(void) local_ttx_EraseFromCache (pDec, pHeader->usPageNum, pHeader->usSubcode);
	}

	pCached	= local_ttx_FetchFromCache(pDec, pHeader->usPageNum, pHeader->usSubcode);
	if (pCached)
	{
		(void) local_ttx_MergePage(pHeader, pCached);	// pHeader에 Cache된 페이지를 머지
	}

	ttxErr = local_ttx_NotifyToReceiver(pDec, pHeader);
	if (ttxErr == ERR_TTX_DECODE_FAIL)
	{
		return ERR_TTX_DELETE;
	}

	/*
	 * Subcode 범위가 (0x0~0x79)를 넘는 경우, Rotating Page로 간주하여,
	 * Cache에 하나의 페이지만 유지한다.
	 * @see EN 300 706, Annex A.1 NOTE1.
	 */
	if (TTX_IS_ROTATING_PAGE(pHeader->usSubcode))
	{
		local_ttx_DeletePage(pDec, pDec->cache.apBuf[pHeader->usPageNum]);
		pDec->cache.apBuf[pHeader->usPageNum]	= NULL;
	}

	err = local_ttx_WriteToCache(pDec, pHeader);
	if (err != ERR_OK)	// Caching에 실패한 경우 지우자!!
	{
		return ERR_TTX_DELETE;
	}

	return ERR_TTX_OK;
}


/**
 * 새로 TOP 페이지를 받았을 때 처리 루틴
 *
 * @param 	pDec	Instance of 'TTX Decoder'
 * @param 	pHeader	방금 만들어진 따끈따끈한 Header 및 그에 딸려온 패킷들~
 * @return	ERR_TTX_DELETE / ERR_TTX_OK
 */
static TTX_Result_t local_ttx_ReceiveTOP (TTX_Decoder_t *pDec, TTX_Header_t *pHeader)
{
	HERROR			err;
	TTX_Packet_t	*pPkt;
	TTX_TOP_Table_t topTable;

	if (pDec->hTOP == HANDLE_NULL)
	{
		return ERR_TTX_DELETE;
	}

	if (local_ttx_DecodePage(pDec, pHeader) != ERR_OK)
	{
		return ERR_TTX_DELETE;
	}

	HxSTD_MemSet(&topTable, 0, sizeof(TTX_TOP_Table_t));

	topTable.bUpdated	= (pHeader->ucControlBit & eTTX_C8_UPDATE_INDICATOR) ? TRUE : FALSE;
	topTable.usPageNum	= pHeader->usPageNum;
	topTable.usSubcode	= pHeader->usSubcode;

	for (pPkt = pHeader->pNextPacket ; pPkt != NULL ; pPkt = pPkt->pNextPacket)
	{
		if (pPkt->ucY > 23)
		{
			continue;
		}
		topTable.apData[pPkt->ucY - 1]	= pPkt->aucData;
	}

	err = TTX_TOP_Receive(pDec->hTOP, &topTable);
	if (err == ERR_TOP_UPDATED)
	{
		HxLOG_Info("TOP Table[MXX:%03X-%04X, type(%d)] is updated!!\n", topTable.usPageNum, topTable.usSubcode, topTable.type);

		//local_ttx_DebugPage(pHeader, NULL);
	}

	return ERR_TTX_DELETE;
}


/**
 * qsort, bsearch에서 사용할 비교 함수
 *
 * @param	lhs	left hand side
 * @param	rhs	right hand side
 * @return	< 0: lhs less than rhs, 0: equal, > 0: lhs greater then rhs
 */
static HINT32		local_ttx_TimeStampCompare (const TTX_TimeStamp_t *lhs, const TTX_TimeStamp_t *rhs)
{
	return (HINT32)(lhs->ulElapTick - rhs->ulElapTick);
}


/**
 * 캐시에 저장된 페이지를 인출(Fetch)한다.
 *
 * @param 	pDec		Instance of 'TTX Decoder'
 * @param	usPageNum	page number
 * @param	usSubcode	subcode
 * @return	페이지 헤더 / NULL
 */
static TTX_Header_t *local_ttx_FetchFromCache	(TTX_Decoder_t *pDec, const HUINT16 usPageNum, const HUINT16 usSubcode)
{
	TTX_Header_t *pHeader	= NULL;

	if (usPageNum >= TTX_MAX_PAGE_NUM)
	{
		return NULL;
	}

	pHeader = pDec->cache.apBuf[usPageNum];
	if (usSubcode == TTX_IGNORE_SUBCODE)
	{
		return pHeader;
	}

	while (pHeader)
	{
		if (pHeader->usSubcode == usSubcode)
		{
			return pHeader;
		}

		pHeader = pHeader->pSubPage;
	}
	return NULL;
}


/**
 * Packet Address값의 우선순위 비교
 * - 우선순위: 0 > 27 > 28 > 26 > 1 > 2 > ... > 25
 *
 * @param	pvArg1	packet 1
 * @param	pvArg2	packet 2
 * @return	< 0: pkt1 < pkt2, > 0: pkt1 > pkt2, 0: pkt1 == pkt2
 */
static int local_ttx_PageAddrCompare (const void *pvArg1, const void *pvArg2)
{
	static const HINT32	c_aIdxTable[29] = {		// 0: Y == 0
		0, 1, 1, 1, 1, 1, 1, 1, 1, 1,			// 1: Y == 1 ~ 25
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,			// 2: Y == 26
		1, 1, 1, 1, 1, 1, 2, 3, 4				// 3: Y == 27
	};											// 4: Y == 28
	static const HINT32	c_aCmpTable[5][5] = {
		{'Y',  1,  1,  1,  1},	// 패킷 우선순위에 따른 비교 테이블
		{ -1,'Y', -1, -1, -1},	//  'Y': Compare 'Y'
		{ -1,  1,'D',  1, -1},	//  'D': Compare 'Designation Code'
		{ -1,  1, -1,'D', -1},	//  < 0: pPkt1 less than pPkt2
		{ -1,  1,  1,  1,'D'}	//  > 0: pPkt1 greater than pPkt2
	};

	TTX_Packet_t	*pPkt1	= *(TTX_Packet_t **)pvArg1;
	TTX_Packet_t	*pPkt2	= *(TTX_Packet_t **)pvArg2;
	HINT32			nCmpType;
	HINT32			nCmp1, nCmp2;
	HINT32			nIdx1, nIdx2;

	nIdx1		= c_aIdxTable[pPkt1->ucY % 29];
	nIdx2		= c_aIdxTable[pPkt2->ucY % 29];
	nCmpType	= c_aCmpTable[nIdx1][nIdx2];

	if (nCmpType == 'Y')
	{
		nCmp1	= (HINT32)pPkt1->ucY;
		nCmp2	= (HINT32)pPkt2->ucY;
	}
	else if (nCmpType == 'D')
	{
		nCmp1	= (HINT32)pPkt1->ucDesgCode;
		nCmp2	= (HINT32)pPkt2->ucDesgCode;
	}
	else
	{
		return nCmpType;
	}
	return nCmp1 - nCmp2;
}


/**
 * 두 Packet리스트를 머지, 아... 복잡하구나..
 *
 * @param 	pH1		One..		이름 붙이기 참 어렵다..
 * @param	pH2		Onother...
 * @return	ERR_OK / ERR_FAIL
 */
static HERROR local_ttx_MergePage	(TTX_Header_t *pH1, TTX_Header_t *pH2)
{
	HINT32			i;
	HINT32			nMgCount	= 0;	// Merge된 패킷 개수
	HINT32			nH1Size		= 0;	// pH1의 패킷 개수
	HINT32			nH1Count	= 0;	// pH1의 총 패킷 개수(Merge된 후)
	HINT32			nH2Count	= 0;	// pH2의 총 패킷 개수(Merge된 후)
	TTX_Packet_t	*pPkt		= NULL;
	TTX_Packet_t	*apH1List[TTX_MAX_PACKET_OF_PAGE];
	TTX_Packet_t	*apH2List[TTX_MAX_PACKET_OF_PAGE];

	if (pH1 == NULL || pH2 == NULL)
		return ERR_FAIL;

	// 1. H1의 Packet List를 생성한 후 정렬
	for (pPkt = pH1->pNextPacket ; pPkt && nH1Size < TTX_MAX_PACKET_OF_PAGE ; pPkt = pPkt->pNextPacket)
	{
		apH1List[nH1Size++]	= pPkt;
	}
	if (nH1Size == TTX_MAX_PACKET_OF_PAGE)
		return ERR_FAIL;

	if (nH1Size > 1)
		qsort(apH1List, nH1Size, sizeof(TTX_Packet_t *), local_ttx_PageAddrCompare);

	// 2. H2을 H1에 머지
	nH1Count	= nH1Size;
	for (pPkt = pH2->pNextPacket ; pPkt ; pPkt = pPkt->pNextPacket)
	{
		// found it!!
		if (nH1Size > 0
			&& bsearch(&pPkt, apH1List, nH1Size, sizeof(TTX_Packet_t *), local_ttx_PageAddrCompare))
		{
			apH2List[nH2Count++]	= pPkt;
		}
		else
		{
			apH1List[nH1Count++]	= pPkt;
			nMgCount++;
		}

		if (nH1Count == TTX_MAX_PACKET_OF_PAGE || nH2Count == TTX_MAX_PACKET_OF_PAGE)
			return ERR_FAIL;
	}

	// 3. 머지된것이 없으면 FAIL
	if (nMgCount == 0)
		return ERR_FAIL;

	// 4. 머지된 H1리스트를 다시 정렬
	if (nH1Count > 1)
		qsort(apH1List, nH1Count, sizeof(TTX_Packet_t *), local_ttx_PageAddrCompare);

	// 5. H1리스트를 다시 패킷 리스트로 재구성
	if (nH1Count == 0)
	{
		pH1->pNextPacket	= NULL;
	}
	else
	{
		pH1->pNextPacket	= apH1List[0];
		for (i = 0 ; i < nH1Count - 1; i++)
			apH1List[i]->pNextPacket	= apH1List[i + 1];
		apH1List[i]->pNextPacket	= NULL;
	}

	// 6. H2리스트를 다시 패킷 리스트로 재구성
	if (nH2Count == 0)
	{
		pH2->pNextPacket	= NULL;
	}
	else
	{
		pH2->pNextPacket	= apH2List[0];
		for (i = 0 ; i < nH2Count - 1 ; i++)
			apH2List[i]->pNextPacket	= apH2List[i + 1];
		apH2List[i]->pNextPacket	= NULL;
	}

	HxLOG_Print("[%s]The Page(%03d-%04X) is Merged!! (+%d)\n", __FUNCTION__, pH1->usPageNum, pH1->usSubcode, nMgCount);

	return ERR_OK;
}


/**
 * 캐시에 새로운 페이지를 Write한다.
 *
 * @param 	pDec	Instance of 'TTX Decoder'
 * @param	pHeader	페이지 헤더
 * @return 	ERR_OK / ERR_FAIL
 */
static HERROR local_ttx_WriteToCache	(TTX_Decoder_t *pDec, TTX_Header_t *pHeader)
{
	TTX_TimeStamp_t	*pTimeStamp	= NULL;
	TTX_Header_t	*pCached	= NULL;
	TTX_Header_t	*pTmp		= NULL;
	HUINT32			ulCurrTick	= 0;

/**************************************************************************************
 * header만 있는 페이지의 경우 케싱을 하지 않기 때문에, Page Up/Down으로 페이지 전환을
 * 할 수 없다. 우선 아래와 같이 임시로 케싱을 막는 부분을 Block하고, 많은 테스트를
 * 거친 후, 코드를 지우자!
 * @date	2009-06-29
 * @author	jhlee4@humaxdigital.com
 **************************************************************************************/
#if 0
	// Header만 있는 페이지는 케싱을 해도 의미가 없다.
	if (pHeader->pNextPacket == NULL)
	{
		return ERR_FAIL;
	}
#endif
	ulCurrTick	= VK_TIMER_GetSystemTick();
	pTimeStamp	= &pDec->cache.aTimeStamp[pHeader->usPageNum];

	pCached		= pDec->cache.apBuf[pHeader->usPageNum];
	if (pCached == NULL)
	{
		pDec->cache.apBuf[pHeader->usPageNum]	= pHeader;
		pDec->cache.ulCount++;

		if (pDec->cache.ulCount > pDec->cache.ulMaxCount)
			pDec->cache.ulMaxCount	= pDec->cache.ulCount;

		pTimeStamp->usPageNum	= pHeader->usPageNum;
		pTimeStamp->ulElapTick	= TTX_ELAPSED_TICK(pDec->cache.ulStartTick, ulCurrTick);
		return ERR_OK;
	}

	while (pCached)
	{
		if (pHeader->usSubcode < pCached->usSubcode)		// insert
		{
			if (pTmp == NULL)
			{
				pDec->cache.apBuf[pHeader->usPageNum]	= pHeader;
			}
			else
			{
				pTmp->pSubPage	= pHeader;
			}
			pHeader->pSubPage	= pCached;

			break;
		}
		else if (pHeader->usSubcode == pCached->usSubcode)	// replace
		{
			if (pTmp == NULL)
			{
				pDec->cache.apBuf[pHeader->usPageNum]	= pHeader;
			}
			else
			{
				pTmp->pSubPage	= pHeader;
			}
			pHeader->pSubPage	= local_ttx_DeleteSubPage(pDec, pCached);

			break;
		}

		pTmp	= pCached;
		pCached = pCached->pSubPage;
	}

	if (pCached == NULL)	// append
	{
		pTmp->pSubPage	= pHeader;
	}

	pDec->cache.ulCount++;

	if (pDec->cache.ulCount > pDec->cache.ulMaxCount)
		pDec->cache.ulMaxCount	= pDec->cache.ulCount;

	pTimeStamp->usPageNum	= pHeader->usPageNum;
	pTimeStamp->ulElapTick	= TTX_ELAPSED_TICK(pDec->cache.ulStartTick, ulCurrTick);

	return ERR_OK;
}


/**
 * 캐시에 저장된 페이지를 삭제
 *
 * @param 	pDec		Instance of 'TTX Decoder'
 * @param	usPageNum	page number
 * @param	usSubcode	subcode
 * @return 	ERR_OK / ERR_FAIL
 */
static HERROR local_ttx_EraseFromCache (TTX_Decoder_t *pDec, const HUINT16 usPageNum, const HUINT16 usSubcode)
{
	TTX_Header_t *pCached	= NULL;
	TTX_Header_t *pTmp		= NULL;

	pCached	= pDec->cache.apBuf[usPageNum];

	while (pCached)
	{
		if (pCached->usSubcode == usSubcode)
		{
			if (pTmp == NULL)
			{
				pDec->cache.apBuf[usPageNum] = pCached->pSubPage;
			}
			else
			{
				pTmp->pSubPage	= pCached->pSubPage;
			}
			local_ttx_DeleteSubPage(pDec, pCached);

			return ERR_OK;
		}

		pTmp	= pCached;
		pCached	= pCached->pSubPage;
	}

	return ERR_FAIL;
}


/**
 * 메모리(메모리풀)가 꽉 찬경우, 캐시내에 저장된 페이지들을 가중치를 기준으로
 * Arrange하여, 메모리를 확보한다.
 *
 * @param 	pDec	Instance of 'TTX Decoder'
 */
static void local_ttx_ArrangeCache	(TTX_Decoder_t *pDec)
{
	HUINT32			i			= 0;
	HUINT32			ulTick		= 0;
	HUINT32			ulPrevCount	= 0;
	HUINT32			ulDeleted	= 0;
	HUINT32			ulMaxDelete	= 0;
	HBOOL			bProtectTOP	= TRUE;
	TTX_Cache_t		*pCache		= NULL;
	TTX_TimeStamp_t	*pTimeStamp;

	pCache	= &(pDec->cache);

	HxLOG_Print("[%s](BEFORE:%d/MAX:%d/Alloc:%d)\n", __FUNCTION__, pCache->ulCount, pCache->ulMaxCount, pDec->pMemory->ulAllocated);

	pTimeStamp	= (TTX_TimeStamp_t *)NX_APP_Malloc(sizeof(TTX_TimeStamp_t) * TTX_MAX_PAGE_NUM);
	if (pTimeStamp == NULL)
	{
		return;
	}
	HxSTD_MemCopy(pTimeStamp, pCache->aTimeStamp, sizeof(TTX_TimeStamp_t) * TTX_MAX_PAGE_NUM);

	qsort(pTimeStamp,
		TTX_MAX_PAGE_NUM,
		sizeof(TTX_TimeStamp_t),
		(int (*)(const void *, const void *))local_ttx_TimeStampCompare);

	ulTick		= VK_TIMER_GetSystemTick();
	ulMaxDelete	= pCache->ulMaxCount / TTX_MAX_WEIGHT;

	do
	{
		for (i = 0 ; i < TTX_MAX_PAGE_NUM ; i++)
		{
			if (pTimeStamp[i].ulElapTick == 0)	// 빈 페이지 skip
				continue;

			if (ulDeleted >= ulMaxDelete)		// done?? break!!
				break;

			// Block or Group은 한번은 봐주고... 그 다음에 지우자!!
			if (bProtectTOP == TRUE && TTX_TOP_IsBlockOrGroup(pDec->hTOP, pTimeStamp[i].usPageNum, TTX_IGNORE_SUBCODE) == ERR_OK)
			{
				pCache->aTimeStamp[pTimeStamp[i].usPageNum].ulElapTick	= TTX_ELAPSED_TICK(pCache->ulStartTick, ulTick);
				continue;
			}

			ulPrevCount	= pCache->ulCount;

			local_ttx_DeletePage(pDec, pCache->apBuf[pTimeStamp[i].usPageNum]);
			pCache->apBuf[pTimeStamp[i].usPageNum]	= NULL;

			ulDeleted	+= ulPrevCount - pCache->ulCount;
		}

		if (bProtectTOP == FALSE)	// 혹시 모르는 무한루프 방지용...
			break;

		bProtectTOP = FALSE;
	} while (ulDeleted == 0);

	HxLOG_Print("[%s](AFTER:%d/MAX:%d/Alloc:%d)\n", __FUNCTION__, pCache->ulCount, pCache->ulMaxCount, pDec->pMemory->ulAllocated);

	NX_APP_Free(pTimeStamp);

	if (ulDeleted == 0)
	{
		// 여긴 오면 안되지만... 모든 메모리를 클리어해 아래와 같이 보험 들자!!
		local_ttx_ResetCache(pDec);
		return;
	}

	pCache->bHungry = FALSE;
}


/**
 * Reset Cache
 *
 * @param 	pDec	Instance of 'TTX Decoder'
 */
static void		local_ttx_ResetCache (TTX_Decoder_t *pDec)
{
	HINT32		i		= TTX_MAX_MAGAZINE;
	TTX_Cache_t	*pCache	= &pDec->cache;

	while (i--)
	{
		if (pDec->aPipe[i].pHeader)
			local_ttx_DeleteSubPage(pDec, pDec->aPipe[i].pHeader);
	}
	HxSTD_MemSet(pDec->aPipe, 0, sizeof(TTX_DemuxPipe_t) * TTX_MAX_MAGAZINE);

	MEMPOOL_Clear(pDec->pMemory);

	pCache->ulCount	= 0;
	pCache->bHungry = TRUE;

	HxSTD_MemSet(pCache->apBuf, 0, sizeof(TTX_Header_t *) * TTX_MAX_PAGE_NUM);
	HxSTD_MemSet(pCache->aTimeStamp, 0, sizeof(TTX_TimeStamp_t) * TTX_MAX_PAGE_NUM);

	HxLOG_Error("[%s](ASSERT:%d/MAX:%d/Alloc:%d)\n", __FUNCTION__, pDec->cache.ulCount, pDec->cache.ulMaxCount, pDec->pMemory->ulAllocated);
}


/**
 * Allocate New TTX Packet
 *
 * @param 	pDec	Instance of 'TTX Decoder'
 * @return	allocated packet
 */
static void *	local_ttx_NewPacket (TTX_Decoder_t *pDec)
{
	void* pvNew	= MEMPOOL_New(pDec->pMemory);
	if (pvNew == NULL)
	{
		local_ttx_ArrangeCache(pDec);
		return NULL;
	}

	HxSTD_MemSet(pvNew, 0, pDec->pMemory->ulSize);

	return pvNew;
}


/**
 * Delete a packet
 *
 * @param 	pDec	Instance of 'TTX Decoder'
 * @param	pPacket	to be deleted..
 * @return	next packet
 */
static TTX_Packet_t * local_ttx_DeletePacket (TTX_Decoder_t *pDec, TTX_Packet_t *pPacket)
{
	TTX_Packet_t	*pNext = pPacket->pNextPacket;

	MEMPOOL_Delete(pDec->pMemory, pPacket);

	return pNext;
}


/**
 * Delete a sub-page (packet list)
 *
 * @param 	pDec	Instance of 'TTX Decoder'
 * @param	pHeader	header of page
 * @return	next subpage
 */
static TTX_Header_t * local_ttx_DeleteSubPage (TTX_Decoder_t *pDec, TTX_Header_t *pHeader)
{
	TTX_Packet_t	*pPacket		= (TTX_Packet_t *)pHeader;
	TTX_Header_t	*pNextSubPage	= pHeader->pSubPage;
	HUINT16			usPageNum		= pHeader->usPageNum;

	while (pPacket != NULL)
	{
		pPacket	= local_ttx_DeletePacket(pDec, pPacket);
	}

	if (pHeader->ucOptionalField & eTTX_OPT_DATA_PAGE)
	{
		return pNextSubPage;
	}

	pDec->cache.ulCount--;

	if (pNextSubPage == NULL)
	{
		pDec->cache.aTimeStamp[usPageNum].ulElapTick	= 0;
	}

	return pNextSubPage;
}


/**
 * Delete Page(with all subpages)
 *
 * @param 	pDec	Instance of 'TTX Decoder'
 * @param	pHeader	header of page
 */
static void		local_ttx_DeletePage (TTX_Decoder_t *pDec, TTX_Header_t *pHeader)
{
	TTX_Header_t	*pSubPage	= pHeader;

	while (pSubPage != NULL)
	{
		pSubPage = local_ttx_DeleteSubPage(pDec, pSubPage);
	}
}

#define	__def__ttx_decode____

/**
 * 페이지를 디코딩 한다.
 *
 * @param 	pDec	Instance of 'TTX Decoder'
 * @param	pHeader	페이지의 Header
 * @return	ERR_OK / ERR_FAIL
 */
static HERROR local_ttx_DecodePage (TTX_Decoder_t *pDec, TTX_Header_t *pHeader)
{
	TTX_Packet_t	*pPacket		= NULL;
	TTX_Packet_t	*pPrevPacket	= NULL;
	TTX_Packet_t	*pErrPacket		= NULL;
	HUINT8			ucY				= 0;
	HERROR			err				= ERR_FAIL;
	HINT32			nDecodeErr		= 0;
	HUINT32			ulErrCount		= 0;
	HUINT32			ulDupCheck		= 0;
	TTX_PageFunction_t	eFunction	= local_ttx_GetPageFunction(pDec, pHeader);
	TTX_PageCoding_t	eCoding		= local_ttx_GetDefaultPageCoding(eFunction);

	if (pHeader->ucY != 0
		|| pHeader->ucOptionalField & eTTX_OPT_PAGE_DECODED
		|| eFunction == eTTX_FUNCTION_UNKNOWN
		|| eCoding == eTTX_CODING_UNKNOWN)
	{
		return ERR_FAIL;
	}

	pPacket		= pHeader->pNextPacket;
	pPrevPacket	= (TTX_Packet_t *)pHeader;

	while (pPacket)
	{
		ucY	= pPacket->ucY;

		if (pPacket->ucOptionalField & eTTX_OPT_PACKET_DECODED)
		{
			if (1 <= ucY && ucY <= 25)
				TTX_BITSET(ulDupCheck, 1 << ucY);

			pPrevPacket	= pPacket;
			pPacket		= pPacket->pNextPacket;
			continue;
		}

		err = ERR_FAIL;
		if (1 <= ucY && ucY <= 25)
		{
			nDecodeErr = local_ttx_DecodePacket(pPacket->aucData, eCoding);
			if (nDecodeErr == 0)	// Decode Success
			{
				if (!(ulDupCheck & (1 << ucY)))		// 디코딩에 성공했지만, 이전에 성공한게 있으면 그냥 무시
				{
					TTX_BITSET(ulDupCheck, 1 << ucY);
					err = ERR_OK;
				}
			}
			else
			{
				// 중복된게 없고, Page coding이 Odd Parity면...
				if (!(ulDupCheck & (1 << ucY)) && eCoding == eTTX_CODING_ODDPARITY)
				{
					err = ERR_OK;
				}
			}
		}
		else if (ucY == 26 || ucY == 28)
		{
			nDecodeErr = local_ttx_DecodePacket(pPacket->aucData, eTTX_CODING_HAMMING2418);
#if TTX_WORKAROUND_01 == TTX_ENABLE
			if (nDecodeErr == 0 && ucY == 28)
			{
 				(void) local_ttx_ParseFuncAndCoding(pPacket->aucData, &eFunction, &eCoding);
			}
#else
			(void) local_ttx_ParseFuncAndCoding;
#endif
			err	= (nDecodeErr == 0) ? ERR_OK : ERR_FAIL;
		}
		else if (ucY == 27)
		{
			err = local_ttx_DecodeFLOF(pPacket->aucData);
		}

		if (err == ERR_FAIL)
		{
			pErrPacket	= pPacket;
			pPacket		= pPacket->pNextPacket;
			pPrevPacket->pNextPacket	= local_ttx_DeletePacket(pDec, pErrPacket);
			ulErrCount++;

			HxLOG_Print("[%s] Y=%02d Error\n", __FUNCTION__,ucY);
		}
		else
		{
			TTX_BITSET(pPacket->ucOptionalField, eTTX_OPT_PACKET_DECODED);
			pPrevPacket	= pPacket;
			pPacket		= pPacket->pNextPacket;
		}
	}

	HxLOG_Print("[%s] Total Err: %d\n", __FUNCTION__, ulErrCount);
	pHeader->ucOptionalField	|= eTTX_OPT_PAGE_DECODED;

	return ERR_OK;
}


/**
 * 각기 다르게 코딩(Error Protection) 된 패킷을 디코딩
 *
 * @param	pucPacket	패킷
 * @param	eCoding		데이터의 코딩
 * @return	ERR 개수
 * @see Page coding(Table 3), ETS 300 706: May 1997
 */
static HINT32	local_ttx_DecodePacket		(HUINT8	*pucPacket, const TTX_PageCoding_t eCoding)
{
	HINT32	i;
	HINT32	aErr[4];

	aErr[0] = aErr[1] = aErr[2] = aErr[3] = 0;

	switch (eCoding)
	{
	case eTTX_CODING_ODDPARITY:
		/*
		 * All 8-bit bytes, each comprising 7 data bits and 1 odd parity bit.
		 * Parity Check을 하는 데이터는 에러가 나도 크리티컬하지 않기 때문에, 에러를 무시한다.
		 */
		aErr[0]	= TTX_ERRCHK_OddParity (pucPacket, pucPacket, TTX_PACKET_DATA_BYTES);
		break;

	case eTTX_CODING_8_DATA_BIT:
		/*
		 * All 8-bit bytes, each comprising 8 data bits.
		 */
		aErr[0]	= TTX_ERRCHK_ReverseBit(pucPacket, pucPacket, TTX_PACKET_DATA_BYTES);
		break;

	case eTTX_CODING_HAMMING2418:
		/*
		 * Per packet: One 8-bit byte coded Hamming 8/4, followed by thirteen groups of three 8-bit
		 *             bytes coded Hamming 24/18.
		 */
		aErr[0]	= TTX_ERRCHK_Hamming84(pucPacket, pucPacket, 1);
		aErr[1]	= TTX_ERRCHK_Hamming2418(pucPacket + 1, pucPacket + 1, 39);
		break;

	case eTTX_CODING_HAMMING84:
		/*
		 * All 8-bit bytes, each code Hamming 8/4.
		 */
		aErr[0] = TTX_ERRCHK_Hamming84 (pucPacket, pucPacket, TTX_PACKET_DATA_BYTES);
		break;

	case eTTX_CODING_TOP_AIT:
		/*
		 * Per packet: Eight 8-bit bytes coded Hamming 8/4, followed by twelve 8-bit bytes coded
		 *             7 data bits and 1 odd parity bit. This sequence is then repeated for the
		 *             remaining 20 bytes.
		 */
		aErr[0]	= TTX_ERRCHK_Hamming84 (pucPacket, pucPacket, 8);
		aErr[1]	= TTX_ERRCHK_OddParity (pucPacket +  8, pucPacket +  8, 12);
		aErr[2]	= TTX_ERRCHK_Hamming84 (pucPacket + 20, pucPacket + 20,  8);
		aErr[3]	= TTX_ERRCHK_OddParity (pucPacket + 28, pucPacket + 28, 12);
		break;

	case eTTX_CODING_VARIABLE:
		/*
		 * Per packet: First 8-bit byte coded Hamming 8/4. The data bits from this byte define the
		 *             coding of the remaining 39 bytes of this packet only, according to the first
		 *             five entries(000~100).
		 */
		if (TTX_ERRCHK_Hamming84(pucPacket, pucPacket, 1) == 0)
		{
			HxLOG_Print("WARNNING!!! <PacketDecoding 101>\n");
			return local_ttx_DecodePacket(pucPacket, *pucPacket);
		}
		return -1;

	default:
		return	-1;
	}

	for (i = 0 ; i < 4 ; i++)
	{
		if (aErr[i] < 0)
			return -1;
	}

	return aErr[0] + aErr[1] + aErr[2] + aErr[3];
}


/**
 * X/28로부터 Page Function과 Page Coding 정보를 가져온다.
 *
 * @param	pucY28		X/28 Data
 * @param	peFunc		Page Function
 * @param	peCoding	Page Coding
 */
static HERROR	local_ttx_ParseFuncAndCoding (HUINT8 *pucY28, TTX_PageFunction_t *peFunc, TTX_PageCoding_t *peCoding)
{
	HUINT8		ucDesgCode	= pucY28[0];
	TTX_PageFunction_t	eFunc;
	TTX_PageCoding_t	eCoding;

	if (peFunc == NULL || peCoding == NULL)
	{
		return ERR_FAIL;
	}

	switch (ucDesgCode)
	{
	case 0x00:
		if ((pucY28[2] & 0x70) || (pucY28[3] & 0x07))	// Format 1
		{
			eFunc	= (TTX_PageFunction_t)(pucY28[1] & 0x0F);
			eCoding	= (TTX_PageCoding_t)(pucY28[2] & 0x07);
		}
		else	// Format 2 ...Not Support
		{
			return ERR_FAIL;
		}
		break;

	case 0x01:
	case 0x02:
		return ERR_FAIL;

	case 0x03:
	case 0x04:
		eFunc	= (TTX_PageFunction_t)(pucY28[1] & 0x0F);
		eCoding	= (TTX_PageCoding_t)(pucY28[2] & 0x07);
		break;

	default:
		return ERR_FAIL;
	}

	if (eTTX_FUNCTION_LAST <= eFunc)
	{
		return ERR_FAIL;
	}

	if (eTTX_CODING_LAST <= eCoding)
	{
		eCoding = local_ttx_GetDefaultPageCoding(eFunc);
	}

	if (eCoding == eTTX_CODING_UNKNOWN)
	{
		return ERR_FAIL;
	}

	*peFunc		= eFunc;
	*peCoding	= eCoding;

	return ERR_OK;
}


/**
 * Page 번호를 기준으로 Page Function을 리턴한다.
 *
 * @param	pDec
 * @param	pHeader
 * @return	Page Function
 */
static TTX_PageFunction_t local_ttx_GetPageFunction (TTX_Decoder_t *pDec, const TTX_Header_t *pHeader)
{
	HUINT8				ucX;
	TTX_TOP_Type_t		topType;

	if (!(pHeader->ucOptionalField & eTTX_OPT_DATA_PAGE))
	{
		return eTTX_FUNCTION_LOP;	// Basic Level 1 Teletext page
	}

	ucX	= (HUINT8)(pHeader->usPageNum & 0x00FF);
	if (ucX == 0xFE)	// MOT
	{
		return eTTX_FUNCTION_MOT;
	}

	if (ucX == 0xFD)	// MIP
	{
		return eTTX_FUNCTION_MIP;
	}

	if (pDec->hTOP == HANDLE_NULL)
	{
		return eTTX_FUNCTION_UNKNOWN;
	}

	topType	= TTX_TOP_GetType(pDec->hTOP, pHeader->usPageNum);
	switch (topType)
	{
	case eTTX_TOP_BTT:		return eTTX_FUNCTION_TOP_BTT;
	case eTTX_TOP_AIT:		return eTTX_FUNCTION_TOP_AIT;
	case eTTX_TOP_MPT:		return eTTX_FUNCTION_TOP_MPT;
	case eTTX_TOP_MPT_EX:	return eTTX_FUNCTION_TOP_MPT_EX;
	default :				return eTTX_FUNCTION_UNKNOWN;
	}
}


/**
 * Page Function에 대응되는 Default Page coding을 리턴한다.
 *
 * @param	eFunction	Page Function
 * @return	Page Coding
 */
static TTX_PageCoding_t		local_ttx_GetDefaultPageCoding	(const TTX_PageFunction_t eFunction)
{
	switch (eFunction)
	{
	case eTTX_FUNCTION_LOP: 		return eTTX_CODING_ODDPARITY;
	case eTTX_FUNCTION_GPOP:		return eTTX_CODING_HAMMING2418;
	case eTTX_FUNCTION_POP: 		return eTTX_CODING_HAMMING2418;
	case eTTX_FUNCTION_GDRCS:
	case eTTX_FUNCTION_DRCS: 		return eTTX_CODING_ODDPARITY;
	case eTTX_FUNCTION_MOT: 		return eTTX_CODING_HAMMING84;
	case eTTX_FUNCTION_MIP: 		return eTTX_CODING_HAMMING84;
	case eTTX_FUNCTION_TOP_BTT: 	return eTTX_CODING_HAMMING84;
	case eTTX_FUNCTION_TOP_AIT: 	return eTTX_CODING_TOP_AIT;
	case eTTX_FUNCTION_TOP_MPT: 	return eTTX_CODING_HAMMING84;
	case eTTX_FUNCTION_TOP_MPT_EX:	return eTTX_CODING_HAMMING84;
	case eTTX_FUNCTION_TRIGGER_MSG:	break;
	default:	break;
	}

	return eTTX_CODING_UNKNOWN;
}



#define	__def__ttx_char_set_designation____

/**
 * Make default information
 *
 * @param	pInfo	DefaultInfo
 * @param	pucPkt	X/28/0 Format1 or X/28/4, M/29/0 or M/29/4
 * @return	ERR_OK / ERR_FAIL
 */
static HERROR local_ttx_MakeDefaultInfo (TTX_DefaultInfo_t *pInfo, const HUINT8 *pucPkt)
{
	HUINT8	ucCharSetDesg		= (pucPkt[2] >> 3) | ((pucPkt[3] & 0x07) << 4);
	HUINT8	uc2ndCharSetDesg	= (pucPkt[3] >> 3) | ((pucPkt[4] & 0x07) << 4);

	pInfo->ucCharSetDesg	= ucCharSetDesg;
	pInfo->uc2ndCharSetDesg	= uc2ndCharSetDesg;
	pInfo->ucScreenColor	= (pucPkt[38] & 0x1F);
	pInfo->ucRowColor 		= (pucPkt[38] >> 5) | ((pucPkt[39] & 0x07) << 2);

	if (local_ttx_CharSetDesignation(&pInfo->charSet, ucCharSetDesg) == ERR_OK
		&& local_ttx_CharSetDesignation(&pInfo->secondCharSet, uc2ndCharSetDesg) == ERR_OK)
	{
		pInfo->bUpdated	= TRUE;
	}

	return ERR_OK;
}


/**
 * Character Set designation!!
 *
 * @param
 * @param
 * @return	ERR_OK / ERR_FAIL
 */
static HERROR local_ttx_CharSetDesignation (TTX_CharSet_t *pSet, const HUINT8 ucCharSetDesg)
{
	pSet->ucG0Set 			= eTTX_CODE_G0_LATIN;
	pSet->ucG2Set 			= eTTX_CODE_G2_LATIN;
	pSet->ucNationalOption	= eTTX_NOS_EMPTY;

	switch (ucCharSetDesg)
	{
	case 0x00:	pSet->ucNationalOption	= eTTX_NOS_ENG;				break;
	case 0x01:	pSet->ucNationalOption	= eTTX_NOS_GER;				break;
	case 0x02:	pSet->ucNationalOption	= eTTX_NOS_SWE_FIN_HUN;		break;
	case 0x03:	pSet->ucNationalOption	= eTTX_NOS_ITA;				break;
	case 0x04:	pSet->ucNationalOption	= eTTX_NOS_FRE;				break;
	case 0x05:	pSet->ucNationalOption	= eTTX_NOS_POR_SPA;			break;
	case 0x06:	pSet->ucNationalOption	= eTTX_NOS_CZE_SLO;			break;
	case 0x08:	pSet->ucNationalOption	= eTTX_NOS_POL;				break;
	case 0x09:	pSet->ucNationalOption	= eTTX_NOS_GER;				break;
	case 0x0A:	pSet->ucNationalOption	= eTTX_NOS_SWE_FIN_HUN;		break;
	case 0x0B:	pSet->ucNationalOption	= eTTX_NOS_ITA;				break;
	case 0x0C:	pSet->ucNationalOption	= eTTX_NOS_FRE;				break;
	case 0x0E:	pSet->ucNationalOption	= eTTX_NOS_CZE_SLO;			break;
	case 0x10:	pSet->ucNationalOption	= eTTX_NOS_ENG;				break;
	case 0x11:	pSet->ucNationalOption	= eTTX_NOS_GER;				break;
	case 0x12:	pSet->ucNationalOption	= eTTX_NOS_SWE_FIN_HUN;		break;
	case 0x13:	pSet->ucNationalOption	= eTTX_NOS_ITA;				break;
	case 0x14:	pSet->ucNationalOption	= eTTX_NOS_FRE;				break;
	case 0x15:	pSet->ucNationalOption	= eTTX_NOS_POR_SPA;			break;
	case 0x16:	pSet->ucNationalOption	= eTTX_NOS_TUR;				break;
	case 0x1D:	pSet->ucNationalOption	= eTTX_NOS_SCC_SCR_SLV;		break;
	case 0x1F:	pSet->ucNationalOption	= eTTX_NOS_RUM;				break;
	case 0x20:	pSet->ucG0Set			= eTTX_CODE_G0_CYRILLIC_1;
				pSet->ucG2Set			= eTTX_CODE_G2_CYRILLIC;	break;
	case 0x21:	pSet->ucNationalOption	= eTTX_NOS_GER;				break;
	case 0x22:	pSet->ucNationalOption	= eTTX_NOS_EST;				break;
	case 0x23:	pSet->ucNationalOption	= eTTX_NOS_LET_LIT;			break;
	case 0x24:	pSet->ucG0Set			= eTTX_CODE_G0_CYRILLIC_2;
				pSet->ucG2Set			= eTTX_CODE_G2_CYRILLIC;	break;
	case 0x25:	pSet->ucG0Set			= eTTX_CODE_G0_CYRILLIC_3;
				pSet->ucG2Set			= eTTX_CODE_G2_CYRILLIC;	break;
	case 0x26:	pSet->ucNationalOption	= eTTX_NOS_CZE_SLO;			break;
	case 0x36:	pSet->ucNationalOption	= eTTX_NOS_TUR;				break;
	case 0x37:	pSet->ucG0Set			= eTTX_CODE_G0_GREEK;
				pSet->ucG2Set			= eTTX_CODE_G2_GREEK;		break;
	case 0x40:	pSet->ucG2Set			= eTTX_CODE_G2_ARBIC;
				pSet->ucNationalOption	= eTTX_NOS_ENG;				break;
	case 0x44:	pSet->ucG2Set			= eTTX_CODE_G2_ARBIC;
				pSet->ucNationalOption	= eTTX_NOS_FRE;				break;
	case 0x47:	pSet->ucG0Set			= eTTX_CODE_G0_ARBIC;
				pSet->ucG2Set			= eTTX_CODE_G2_ARBIC;		break;
	case 0x55:	pSet->ucG0Set			= eTTX_CODE_G0_HEBREW;
				pSet->ucG2Set			= eTTX_CODE_G2_ARBIC;		break;
	case 0x57:	pSet->ucG0Set			= eTTX_CODE_G0_ARBIC;
				pSet->ucG2Set			= eTTX_CODE_G2_ARBIC;		break;
	case 0x7F:	return ERR_OK;
	default:	return ERR_FAIL;
	}
	return ERR_OK;
}


/**
 * 8/30 Format 1 Packet으로 부터 Characeter Set을 얻는다.
 *
 * @param
 * @param
 * @return	ERR_OK / ERR_FAIL
 */
static HERROR	local_ttx_CharSetDesignationByNI (TTX_CharSet_t *pSet, const HUINT8 *pucNI)
{
	if (pucNI[0] == 0x03)	// Croatia
	{
		pSet->ucG0Set	= eTTX_CODE_G0_CYRILLIC_1;
		pSet->ucG2Set	= eTTX_CODE_G2_CYRILLIC;
		pSet->ucNationalOption	= eTTX_NOS_EMPTY;

		return ERR_OK;
	}

	if (pucNI[0] == 0x30)	// Greece
	{
		pSet->ucG0Set	= eTTX_CODE_G0_GREEK;
		pSet->ucG2Set	= eTTX_CODE_G2_GREEK;
		pSet->ucNationalOption	= eTTX_NOS_EMPTY;

		return ERR_OK;
	}

	pSet->ucG0Set 			= eTTX_CODE_G0_LATIN;
	pSet->ucG2Set 			= eTTX_CODE_G2_LATIN;
	pSet->ucNationalOption	= eTTX_NOS_EMPTY;

	switch (pucNI[0])
	{
	case 0x31:	// Netherlands
	case 0x5C:	// German
		pSet->ucNationalOption	= eTTX_NOS_GER;
		break;

	case 0x33:
		if (pucNI[1] == 0x33)	// Ireland
			pSet->ucNationalOption	= eTTX_NOS_ENG;
		else	// France
			pSet->ucNationalOption	= eTTX_NOS_FRE;
		break;

	case 0x35:
		if (pucNI[1] >= 0x80)		// Finland
			pSet->ucNationalOption	= eTTX_NOS_SWE_FIN_HUN;
		else if (pucNI[1] == 0x41)	// Iceland
			pSet->ucNationalOption	= eTTX_NOS_SWE_FIN_HUN;
		else if (pucNI[1] >= 0x30)	// Ireland
			pSet->ucNationalOption	= eTTX_NOS_ENG;
		else						// Portugal
			pSet->ucNationalOption	= eTTX_NOS_POR_SPA;
		break;

	case 0x39:	// Itary
		pSet->ucNationalOption	= eTTX_NOS_ITA;
		break;

	case 0x41:
	case 0x42:	// Czech Republic, Slovakia
		pSet->ucNationalOption	= eTTX_NOS_CZE_SLO;
		break;


	case 0x36:	// Hungary
	case 0x45:	// Denmark
	case 0x46:
	case 0x47:	// Norway
	case 0x73:	// Denmark
		pSet->ucNationalOption	= eTTX_NOS_SWE_FIN_HUN;
		break;

	case 0x48:
		pSet->ucNationalOption	= eTTX_NOS_POL;
		break;

	case 0x49:
		if (pucNI[1] == 0xCF)
			pSet->ucNationalOption	= eTTX_NOS_SWE_FIN_HUN;
		else
			pSet->ucNationalOption	= eTTX_NOS_GER;
		break;

	case 0xFA:
		switch (pucNI[1])
		{
		case 0x04:	// Itary
		case 0x05:
		case 0x06:
		case 0x08:	pSet->ucNationalOption	= eTTX_NOS_ITA;	break;
		default:	pSet->ucNationalOption	= eTTX_NOS_ENG;	break;	// UK
		}
		break;

	case 0xF1:
	case 0xF5:
	case 0xFE:	// France
		pSet->ucNationalOption	= eTTX_NOS_FRE;
		break;

	case 0x04:	// Belgium
	case 0x32:
	default:
		return ERR_FAIL;
	}

	return ERR_OK;
}


/**
 *
 *
 */
static HERROR	local_ttx_ChooseLanguage (TTX_Decoder_t *pDec, TTX_CharSet_t *pSet, const HUINT8 ucNOS)
{
	if (ucNOS > eTTX_NOS_MAX)
		return ERR_FAIL;

	if (pDec->aUserDefinedChar[ucNOS].bDefined)
	{
		*pSet	= pDec->aUserDefinedChar[ucNOS].set;

		return ERR_OK;
	}

	if (local_ttx_CharSetDesignation(pSet, ucNOS) == ERR_OK)
		return ERR_OK;

	if (local_ttx_CharSetDesignationByNI(pSet, pDec->aucNI) == ERR_OK)
		return ERR_OK;

	return ERR_FAIL;
}


#define __def__ttx_make_teletext_page____

#if 1
/**
 * 패킷들로부터 TTX Page를 만든다.
 *
 * @param 	pDec	Instance of 'TTX Decoder'
 * @param 	pPage	TTX Page
 * @param 	pHeader	TTX packet header
 */
static void	local_ttx_MakeTeletextPage	(TTX_Decoder_t *pDec, TTX_Page_t *pPage, TTX_Header_t *pHeader)
{
	TTX_Packet_t		*pPkt		= (TTX_Packet_t *)pHeader;
	TTX_DefaultInfo_t	defaultInfo	= pDec->aMagazineInfo[pHeader->usPageNum / 100];

	pPage->usPageNum	= pHeader->usPageNum;
	pPage->usSubcode	= pHeader->usSubcode;

	// EN 300 706의 Annex E.9 Selection of national option sub-sets의 두번 째 룰에 따라.
	// M/29가 세팅되어 있으면, C12-14를 사용하지 않는다.
	if (defaultInfo.bUpdated == FALSE)
	{
		if (local_ttx_ChooseLanguage(pDec, &defaultInfo.charSet, pHeader->ucNationalOption) != ERR_OK)
		{
			defaultInfo.charSet.ucG0Set	= eTTX_CODE_G0_LATIN;
			defaultInfo.charSet.ucG2Set	= eTTX_CODE_G2_LATIN;
			defaultInfo.charSet.ucNationalOption	= eTTX_NOS_ENG;
		}
		defaultInfo.secondCharSet.ucG0Set	= eTTX_CODE_G0_LATIN;
		defaultInfo.secondCharSet.ucG2Set	= eTTX_CODE_G2_LATIN;
		defaultInfo.secondCharSet.ucNationalOption	= eTTX_NOS_ENG;
	}

	/**
	 * 원래 사양상 X/28은 다른 패킷들보다 먼저 들어와야 하나... 너무 늦게 패킷이 들어와서 처리가 안되고 있는 경우가 있음
	 * 이에 따라 미리 28번 패킷을 처리할 수 있도록 아래와 같이 28번 패킷을 먼저 모니터링 한다.
	 */
	while (pPkt)
	{
		if (pPkt->ucY == 28)
		{
			pPkt = local_ttx_MakePageRelatedInfo(&defaultInfo, pPkt);
		}
		else
		{
			pPkt = pPkt->pNextPacket;
		}
	}
	pPkt = (TTX_Packet_t *)pHeader;

	pPage->ucDefaultCode	= defaultInfo.charSet.ucG0Set;
	pPage->ucDefaultNOS		= defaultInfo.charSet.ucNationalOption;
	pPage->bSuppressHeader	= (pHeader->ucControlBit & eTTX_C7_SUPPRESS_HEADER	) ? TRUE : FALSE;
	pPage->bInhibitDisplay	= (pHeader->ucControlBit & eTTX_C10_INHIBIT_DISPLAY	) ? TRUE : FALSE;
	pPage->bSubtitle		= (pHeader->ucControlBit & eTTX_C6_SUBTITLE			) ? TRUE : FALSE;
	pPage->bNewsflash		= (pHeader->ucControlBit & eTTX_C5_NEWSFLASH		) ? TRUE : FALSE;

	while (pPkt)
	{
		if (pPkt->ucY <= 24)
		{
			pPkt = local_ttx_MakeSpacingAttr (pPage, pPkt, &defaultInfo);
		}
		else if (pPkt->ucY == 27)
		{
			pPkt = local_ttx_ParseFLOF(pPage->usPageNum, &pPage->FLOF, pPkt);
		}
#if 0	// 위 수정 땜시 Block
		else if (pPkt->ucY == 28)
		{
			pPkt = local_ttx_MakePageRelatedInfo(&defaultInfo, pPkt);
		}
#endif
		else	// Y == 25, and.. other
		{
			pPkt = pPkt->pNextPacket;
		}
	}

	pPkt = pHeader->pNextPacket;
	while (pPkt)	// Cell이 spacing attribute들로 인해 overwrite될 수 있기 때문에
	{				// non-spacing attribute는 가장 마지막에 처리한다.
		if (pPkt->ucY == 26)
		{
			pPkt = local_ttx_MakeNonSpacingAttr (pPage, pPkt, &defaultInfo);
		}
		else
		{
			pPkt = pPkt->pNextPacket;
		}
	}

	// set default screen color
	pPage->ucScreenColor	= defaultInfo.ucScreenColor;
	pPage->ucRowColor		= defaultInfo.ucRowColor;

	local_ttx_ApplySizeAttributes(pPage);

	pPage->bEnabled	= TRUE;

	(void)local_ttx_MakeCTT(pDec, pPage);
	(void)local_ttx_MakeSubcodeBar(pDec, pPage);

	if (TTX_TOP_GetTOPLink(pDec->hTOP, pPage->usPageNum, pPage->TOP.aLink) == ERR_OK)
	{
		pPage->TOP.bEnabled		= TRUE;
		pPage->TOP.ulVersion	= TTX_TOP_GetCurVersion(pDec->hTOP);
	}
}

/**
 * Spacing Attribute를 TTX Page에 적용한다.
 *
 * @param 	pPage			TTX Page
 * @param  	pPkt			TTX Packet list
 * @param 	pDefaultInfo	디폴트 정보
 * @return 	TTX_Packet
 */
static TTX_Packet_t *	local_ttx_MakeSpacingAttr (TTX_Page_t *pPage, TTX_Packet_t *pPkt, const TTX_DefaultInfo_t *pDefaultInfo)
{
	HUINT8	ucRow	= 0;
	HUINT8	ucCol	= 0;
	HUINT8	ucAttr	= 0;
	TTX_Cell_t	*pCell	= NULL;
	HUINT8	bBoxMode	= (pPage->bNewsflash || pPage->bSubtitle) ? TRUE : FALSE;

	TTX_SpacingAttr_t	spacingAttr;
	TTX_SpacingAttr_t	defaultAttr;

	HxSTD_MemSet(&spacingAttr, 0, sizeof(TTX_SpacingAttr_t));
	HxSTD_MemSet(&defaultAttr, 0, sizeof(TTX_SpacingAttr_t));

	// init default spacing attribute
	defaultAttr.ucHeldMosaic		= TTX_CHAR_SPACE;
	defaultAttr.ucFgColor			= eTTX_COLOR_WHITE;
	defaultAttr.ucBgColor			= pDefaultInfo->ucRowColor;
	defaultAttr.ucG0				= pDefaultInfo->charSet.ucG0Set;
	defaultAttr.ucNationalOpt		= pDefaultInfo->charSet.ucNationalOption;
	defaultAttr.uc2ndG0				= pDefaultInfo->secondCharSet.ucG0Set;
	defaultAttr.uc2ndNationalOpt	= pDefaultInfo->secondCharSet.ucNationalOption;

	while (pPkt && pPkt->ucY <= 24)
	{
		ucRow 			= pPkt->ucY;
		// Spacing attributes are initialized by the start of a new row.
		spacingAttr		= defaultAttr;

		for (ucCol = 0 ; ucCol < TTX_MAX_COLUMN ; ucCol++)
		{
			pCell 	= &pPage->aCellMap[ucRow][ucCol];
			ucAttr	= (ucRow == 0 && ucCol < 8) ? TTX_CHAR_SPACE : pPkt->aucData[ucCol];

			spacingAttr.bDoubleHeight	= (ucRow < 23) ? spacingAttr.bDoubleHeight : FALSE;
			spacingAttr.bDoubleWidth	= (ucCol < 39) ? spacingAttr.bDoubleWidth  : FALSE;

			if (ucAttr < TTX_CHAR_SPACE)	// Spacing Attributes
			{
				local_ttx_SpacingAttrSetAt (&spacingAttr, ucAttr);

				local_ttx_MakeTeletextCell(pCell, &spacingAttr, ucAttr, bBoxMode);

				local_ttx_SpacingAttrSetAfter (&spacingAttr, ucAttr);
			}
			else if (ucAttr < 0x80)	// Character codes
			{
				local_ttx_MakeTeletextCell(pCell, &spacingAttr, ucAttr, bBoxMode);
			}
			else	// error...
			{
				local_ttx_MakeTeletextCell(pCell, &spacingAttr, TTX_CHAR_SPACE, bBoxMode);
			}
		}

		pPkt	= pPkt->pNextPacket;
	}

	return pPkt;
}

/**
 * Spacing Attribute들 중 "Set-At"에 해당되는 attribute들을 처리한다.
 *
 * @param 	pAttr	현재 까지의 TTX Spacing Attribute 정보
 * @param  	ucAttr	새로 들어온 하나의 Attribute
 */
static void local_ttx_SpacingAttrSetAt	(TTX_SpacingAttr_t *pAttr, const HUINT8 ucAttr)
{
	if (!((1 << ucAttr) & eTTX_SA_SET_AT))
	{
		return;
	}

	switch ((1 << ucAttr))
	{
	case eTTX_SA_STEADY:
		pAttr->bFlash	= FALSE;
		break;

	case eTTX_SA_NORMAL_SIZE:
		pAttr->bDoubleHeight	= FALSE;
		pAttr->bDoubleWidth		= FALSE;
		break;

	case eTTX_SA_CONCEAL:
		pAttr->bConceal	= TRUE;
		break;

	case eTTX_SA_CONTIGUOUSE_MOSAIC:
		pAttr->bSeparatedMosaic	= FALSE;
		break;

	case eTTX_SA_SEPARATED_MOSAIC:
		pAttr->bSeparatedMosaic	= TRUE;
		break;

	case eTTX_SA_BLACK_BACKGROUND:
		pAttr->ucBgColor	= eTTX_COLOR_BLACK;
		break;

	case eTTX_SA_NEW_BACKGROUND:
		pAttr->ucBgColor	= pAttr->ucFgColor;
		break;

	case eTTX_SA_HOLD_MOSAICS:
		pAttr->bHoldMosaic	= TRUE;
		break;

	default:
		HxLOG_Critical("\n[%s:%d]\n",__FUNCTION__,__LINE__);
		break;
	}
}

/**
 * Spacing Attribute들 중 "Set-After"에 해당되는 attribute들을 처리한다.
 *
 * @param 	pAttr	현재 까지의 TTX Spacing Attribute 정보
 * @param  	ucAttr	새로 들어온 하나의 Attribute
 */
static void local_ttx_SpacingAttrSetAfter	(TTX_SpacingAttr_t *pAttr, const HUINT8 ucAttr)
{
	if (!((1 << ucAttr) & eTTX_SA_SET_AFTER))
	{
		return;
	}

	switch ((1 << ucAttr))
	{
	case eTTX_SA_FLASH:
		pAttr->bFlash	= TRUE;
		break;

	case eTTX_SA_END_BOX:
		pAttr->bBox		= FALSE;
		break;

	case eTTX_SA_START_BOX:
		pAttr->bBox		= TRUE;
		break;

	case eTTX_SA_DOUBLE_HEIGHT:
		pAttr->bDoubleHeight	= TRUE;
		pAttr->bDoubleWidth		= FALSE;
		break;

	case eTTX_SA_DOUBLE_WIDTH:
		pAttr->bDoubleWidth		= TRUE;
		pAttr->bDoubleHeight	= FALSE;
		break;

	case eTTX_SA_DOUBLE_SIZE:
		pAttr->bDoubleHeight	= TRUE;
		pAttr->bDoubleWidth		= TRUE;
		break;

	case eTTX_SA_ESC:
		pAttr->bESC		= !(pAttr->bESC);
		break;

	case eTTX_SA_RELEASE_MOSAICS:
		pAttr->bHoldMosaic	= FALSE;
		break;

	default:
		pAttr->bConceal	= FALSE;

		if ((1 << ucAttr) & eTTX_SA_ALPHA_COLOR_CODES)
		{
			pAttr->ucFgColor	= ucAttr;
			pAttr->bMosaicMode	= FALSE;
		}
		else if ((1 << ucAttr) & eTTX_SA_MOSAIC_COLOR_CODES)
		{
			pAttr->ucFgColor	= ucAttr - 0x10;
			pAttr->bMosaicMode	= TRUE;
		}
		else
		{
			HxLOG_Critical("\n[%s:%d]\n",__FUNCTION__,__LINE__);
		}
		break;
	}
}

/**
 * 지금까지의 Spacing Attribute를 적용하여 하나의 Cell을 만든다.
 *
 * @param 	pCell		TTX Cell
 * @param  	pAttr		지금까지 적용된 Spacing Attribute들
 * @param  	ucAttr		새로 들어온 하나의 Attribute
 * @param	bBoxMode	subtitle or newsflash
 */
static void local_ttx_MakeTeletextCell	(TTX_Cell_t *pCell, TTX_SpacingAttr_t *pAttr, const HUINT8 ucAttr, const HUINT8 bBoxMode)
{
	HUINT8	ucChar	= (ucAttr < TTX_CHAR_SPACE) ? TTX_CHAR_SPACE : ucAttr;

	pCell->bConceal 		= pAttr->bConceal;
	pCell->bFlash			= pAttr->bFlash;
	pCell->ucFgColor		= pAttr->ucFgColor;
	pCell->ucBgColor		= pAttr->ucBgColor;
	pCell->bDoubleHeight	= pAttr->bDoubleHeight;
	pCell->bDoubleWidth 	= pAttr->bDoubleWidth;

	// Mosaic Graphics
	if (pAttr->bMosaicMode == TRUE && (ucChar & 0x20))
	{
		if (ucAttr < TTX_CHAR_SPACE)
		{
			ucChar	= (pAttr->bHoldMosaic) ? pAttr->ucHeldMosaic : ucChar;
		}

		pAttr->ucHeldMosaic	= ucChar;	// The "Held-Mosaic" is most recent mosaics character.
		pCell->ucCode		= eTTX_CODE_G1_MOSAIC;
		pCell->Opt.bSeparatedMosaic = pAttr->bSeparatedMosaic;
	}
	// Alphanumeric Characters
	else
	{
		if (pAttr->bESC == TRUE)	// Toggle between the first and second G0 sets
		{
			pCell->ucCode			= pAttr->uc2ndG0;
			pCell->ucNationalOption	= pAttr->uc2ndNationalOpt;
		}
		else
		{
			pCell->ucCode			= pAttr->ucG0;
			pCell->ucNationalOption	= pAttr->ucNationalOpt;
		}
	}

	pCell->ucChar = ucChar;

	//On pages with Subtitle or Newsflash, Characters outside this box are not displayed.
	if (bBoxMode == TRUE)
	{
		pCell->bDisplayable = pAttr->bBox;
	}
	else
	{
		pCell->bDisplayable = TRUE;
	}
}

/**
 * Y=26 을 파싱하여 Non-spacing attribute들을 TTX Page에 적용한다.
 *
 * @param 	pPage			TTX Page
 * @param  	pPkt			TTX Packet list (Y=26)
 * @param 	pDefaultInfo	디폴트 정보
 * @return 	TTX Packet (next)
 */
static TTX_Packet_t * local_ttx_MakeNonSpacingAttr (TTX_Page_t *pPage, TTX_Packet_t *pY26, const TTX_DefaultInfo_t *pDefaultInfo)
{
	HUINT8	ucTriplet	= 0;
	HUINT8	ucAddr		= 0;
	HUINT8	ucMode		= 0;
	HUINT8	ucData		= 0;
	HUINT8	ucRow		= 0;
	HUINT8	*pucY26 		= NULL;
	HINT32	lActivePosition = -1;
	TTX_Cell_t	*pCell		= NULL;

	while (pY26 && pY26->ucY == 26)
	{
		pucY26	= pY26->aucData;
		pucY26++;	// Skip Designation Code

		for (ucTriplet = 0 ; ucTriplet < 13 ; ucTriplet++, pucY26 += 3)
		{
			ucAddr	= pucY26[0] | ((pucY26[1] & 0x3) << 4);
			ucMode	= pucY26[1] >> 2;
			ucData	= pucY26[2];

			if (40 <= ucAddr && ucMode == 0x1F)	// Row Address Group && Mode == TERMINATION MARKER
			{
				break;
			}

			// Column Address Group (0 ~ 39)
			if (ucAddr <= 39)
			{
				if (lActivePosition < 0)
				{
					continue;
				}

				ucRow = (HUINT8)lActivePosition;

				pCell	= &pPage->aCellMap[ucRow][ucAddr];
				if (ucMode == 0x01)			// Block Mosaic Character from the G1 Set
				{
					pCell->ucChar	= ucData;
					pCell->ucCode	= eTTX_CODE_G1_MOSAIC;
				}
				else if (ucMode == 0x02 || ucMode == 0x0B) 	// Line Drawing or Smoothed Mosaic Character
				{
					pCell->ucChar	= ucData;
					pCell->ucCode	= eTTX_CODE_G3_MOSAIC;
				}
				else if (ucMode == 0x0F)	// Characters from the G2 supplementary set
				{
					pCell->ucChar	= ucData;
					pCell->ucCode	= pDefaultInfo->charSet.ucG2Set;
				}
				else if (0x10 <= ucMode && ucMode <= 0x1F)	// Characters including diacritical marks
				{
					pCell->ucChar	= ucData;
					pCell->ucCode	= pDefaultInfo->charSet.ucG0Set;
					pCell->bCompositeChar	= TRUE;
					pCell->Opt.ucDiacriticalMark = ucMode & 0x0F;
				}
				else
				{
					// do nothing.. (Over presentation level)
				}
			}
			// Row Address Group (40 ~ 63)
			else if (ucAddr <= 63)
			{
				if (ucMode == 0x04) 		// Set Active Position
				{
					lActivePosition = (HINT32)((ucAddr == 40) ? 24 : ucAddr - 40);
				}
				else if (ucMode == 0x07)	// Address Display Row 0
				{
					lActivePosition = 0;
				}
			}
			else
			{
				HxLOG_Print("Invalid Address of Y=26 Triplet!!\n");
			}
		}

		pY26 = pY26->pNextPacket;
	}

	return pY26;

}

/**
 * Y=28 을 파싱하여 디폴트 정보를 생성한다.
 *
 * @param 	pDefaultInfo	디폴터 정보
 * @param  	pPkt			TTX Packet list (Y=28)
 * @return 	TTX Packet (next)
 */
static TTX_Packet_t * local_ttx_MakePageRelatedInfo (TTX_DefaultInfo_t *pDefaultInfo, TTX_Packet_t *pY28)
{
	HUINT8	*pucY28		= NULL;

	while (pY28 && pY28->ucY == 28)
	{
		pucY28	= pY28->aucData;

		HxLOG_Info("[%s] Make X/28(%X)!!\n", __FUNCTION__, pucY28[0]);

		if (pucY28[0] == 0	// Designation code == 0 and X/28/0 Format 1 only
			&& ((pucY28[2] >> 4) != 0 || (pucY28[3] & 0x3F) != 0))
		{
			(void)local_ttx_MakeDefaultInfo(pDefaultInfo, pY28->aucData);
		}

		pY28 = pY28->pNextPacket;
	}

	return pY28;
}

/**
 * Double Height / Double Width / Double Size Attribute 속성으로 인한
 * Cell의 겹침 현상을 방지하기 위해 근접 Cell들을 Not Displayable로 마킹한다.
 *
 * @param pPage	TTX Page
 */
static void local_ttx_ApplySizeAttributes (TTX_Page_t *pPage)
{
	HUINT8	ucRow	= 0;
	HUINT8	ucCol	= 0;
	HBOOL	bFoundDoubleHeight	= FALSE;

	TTX_Cell_t	*pCurrRow	= NULL;
	TTX_Cell_t	*pBelowRow	= NULL;

	// "The broadcaster should not insert double height control characters in rows 23 or 24.", EN 300 706 Table 26
	// 위의 사양으로 인해 22까지만 체크한다.
	for (ucRow = 0 ; ucRow < 23 ; ucRow++)
	{
		pCurrRow	= pPage->aCellMap[ucRow];
		pBelowRow	= pPage->aCellMap[ucRow + 1];

		for (ucCol = 0 ; ucCol < TTX_MAX_COLUMN ; ucCol++)
		{
			if (pCurrRow[ucCol].bDoubleHeight == TRUE)
				break;
		}

		bFoundDoubleHeight = (ucCol != TTX_MAX_COLUMN) ? TRUE : FALSE;

		for (ucCol = 0 ; ucCol < TTX_MAX_COLUMN ; ucCol++)
		{
			if (pCurrRow[ucCol].bDisplayable == FALSE)
				continue;

			if (bFoundDoubleHeight == TRUE && pCurrRow[ucCol].bDoubleHeight == FALSE)
			{
				pBelowRow[ucCol].ucBgColor	= pCurrRow[ucCol].ucBgColor;
				pBelowRow[ucCol].ucFgColor	= pBelowRow[ucCol].ucBgColor;
				pBelowRow[ucCol].bDisplayable	= TRUE;
			}

			// "DOUBLE HEIGHT"
			if (pCurrRow[ucCol].bDoubleHeight == TRUE)
			{
				pBelowRow[ucCol].bDisplayable	= FALSE;
			}

			// "DOUBLE WIDTH"
			if (pCurrRow[ucCol].bDoubleWidth == TRUE && ucCol < TTX_MAX_COLUMN - 1)
			{
				pCurrRow[ucCol + 1].bDisplayable = FALSE;
			}

			// "DOUBLE SIZE"
			if (pCurrRow[ucCol].bDoubleHeight == TRUE && pCurrRow[ucCol].bDoubleWidth == TRUE && ucCol < TTX_MAX_COLUMN - 1)
			{
				pBelowRow[ucCol + 1].bDisplayable	= FALSE;
			}
		}
	}
}

/**
 * Search 중에 Header만 Rolling 시키기 위해 Rolling Header를 만든다.
 *
 * @param 	pDec	Instance of 'TTX Decoder'
 * @param	pPage	TTX Page
 */
static void	local_ttx_MakeRollingHeader (TTX_Decoder_t *pDec, TTX_Page_t *pPage)
{
	HINT32				i;
	HUINT8				ucColor;
	TTX_DefaultInfo_t	defaultInfo;
	TTX_Packet_t		packet;

	HxSTD_MemSet(&defaultInfo, 0, sizeof(TTX_DefaultInfo_t));
	HxSTD_MemSet(&packet, 0, sizeof(TTX_Packet_t));

	defaultInfo.charSet.ucG0Set	= eTTX_CODE_G0_LATIN;
	defaultInfo.charSet.ucG2Set	= eTTX_CODE_G2_LATIN;
	defaultInfo.charSet.ucNationalOption	= eTTX_NOS_EMPTY;

	HxSTD_MemCopy(packet.aucData + 8, pDec->aucRollingHeader, TTX_HEADER_DATA_BYTES);

	local_ttx_MakeSpacingAttr(pPage, &packet, &defaultInfo);

	ucColor	= pPage->aCellMap[0][8].ucFgColor;
	for (i = 8 ; i < TTX_MAX_COLUMN ; i++)
	{
		if (ucColor == pPage->aCellMap[0][i].ucFgColor)
		{
			pPage->aCellMap[0][i].ucFgColor	= eTTX_COLOR_GREEN;
		}
	}
}

/**
 * TTX Time을 만든다.
 *
 * @param 	pDec	Instance of 'TTX Decoder'
 * @param	pPage	TTX Page
 */
static void local_ttx_MakeTeletextTime (TTX_Decoder_t *pDec, TTX_Page_t *pPage)
{
	HINT32		i;
	TTX_Time_t	*pTime		= NULL;
	HUINT32		ulSpaceCellCount= 0;
	HUINT8		*pucTime	= pDec->pucTimeFill;
	TTX_Cell_t	*pTimeCell	= &(pPage->aCellMap[0][32]);

	for (i = 0 ; i < TTX_TIME_FIELD_LENGTH ; i++)
	{
		if (pTimeCell[i].ucChar <= 0x20)
			ulSpaceCellCount++;
	}

	pTime		= &pDec->ttxTime;

	// Header에 시간정보를 갖지 않는다.
	if (pTime->eSetBy == eTTX_TIME_BY_NONE || ulSpaceCellCount == TTX_TIME_FIELD_LENGTH)
	{
		return;
	}

	for (i = 0 ; i < TTX_TIME_FIELD_LENGTH ; i++)
	{
		pTimeCell[i].ucChar = pDec->pucTimeFill[i];
	}

	// 시간 외에 문자 Slide 등의 정보가 표시됨
	if (local_ttx_IsTimeFormat(pucTime) == FALSE)
	{
		for (i = 0 ; i < TTX_TIME_FIELD_LENGTH ; i++)
		{
			pTimeCell[i].ucChar = pDec->pucTimeFill[i];
		}
	}
#if TTX_WORKAROUND_06 == TTX_ENABLE
	// 시간이 표시됨 (HH:MM:SS)
	else
	{
		TIME_T		currTime;
		HUINT32		ulElapSec	= 0;
		HUINT32		ulCurrSec	= 0;
		HUINT32		ulCurrTick	= 0;
		ulCurrTick	= VK_TIMER_GetSystemTick();
		ulElapSec	= TTX_ELAPSED_TICK(pTime->ulLastTick, ulCurrTick) / 1000;

		if (ulElapSec > 0)
		{
			ulCurrSec	= pTime->time.ucHour * 3600 + pTime->time.ucMinute * 60 + pTime->time.ucSecond;
			ulCurrSec	+= ulElapSec;
			currTime.ucHour		= (ulCurrSec / 3600) % 24;
			currTime.ucMinute	= (ulCurrSec / 60  ) % 60;
			currTime.ucSecond	= ulCurrSec % 60;
			currTime.usMillisecond = 0;
			pTime->time			= currTime;
			pTime->ulLastTick	= pTime->ulLastTick + ulElapSec * 1000;
		}
		else
			currTime	= pTime->time;

		pTimeCell[0].ucChar	= currTime.ucHour	/ 10 + '0';
		pTimeCell[1].ucChar	= currTime.ucHour   % 10 + '0';
		pTimeCell[2].ucChar	= (isalnum(pTimeCell[2].ucChar)) ? ':' : pTimeCell[2].ucChar;
		pTimeCell[3].ucChar	= currTime.ucMinute / 10 + '0';
		pTimeCell[4].ucChar	= currTime.ucMinute % 10 + '0';
		pTimeCell[5].ucChar	= (isalnum(pTimeCell[5].ucChar)) ? ':' : pTimeCell[2].ucChar;
		pTimeCell[6].ucChar	= currTime.ucSecond / 10 + '0';
		pTimeCell[7].ucChar	= currTime.ucSecond % 10 + '0';
	}
#endif
}


/**
 * Header의 Time Field가 시간인지 문자인지 판단
 *
 * @param	pucTime	Header의 시간 필드
 * @return	TRUE / FALSE
 */
static HBOOL local_ttx_IsTimeFormat (HUINT8 *pucTime)
{
	HINT32	nDigit	= 0;

	// 시간 중간에 alphabet, digit이 있으면 시간이 아니다
	if (isalnum(pucTime[2]) || isalnum(pucTime[5]))
	{
		return FALSE;
	}

	nDigit	= (isdigit(pucTime[0])) ? nDigit + 1 : nDigit;
	nDigit	= (isdigit(pucTime[1])) ? nDigit + 1 : nDigit;
	nDigit	= (isdigit(pucTime[3])) ? nDigit + 1 : nDigit;
	nDigit	= (isdigit(pucTime[4])) ? nDigit + 1 : nDigit;
	nDigit	= (isdigit(pucTime[6])) ? nDigit + 1 : nDigit;
	nDigit	= (isdigit(pucTime[7])) ? nDigit + 1 : nDigit;

	if (nDigit < 5)
	{
		return FALSE;
	}
	return TRUE;
}
#endif

static void		local_ttx_SearchCTT (TTX_Decoder_t *pDec, HUINT16 usPageNum, HUINT16 ausCTT[])
{
	TTX_Header_t	*pHeader;

	pHeader		= local_ttx_SearchPrevPage(pDec, usPageNum);	// 이전
	ausCTT[0]	= (pHeader) ? pHeader->usPageNum : usPageNum;
	pHeader		= local_ttx_SearchNextPage(pDec, usPageNum);	// 다음
	ausCTT[1]	= (pHeader) ? pHeader->usPageNum : usPageNum;
	pHeader		= local_ttx_SearchNextPage(pDec, ausCTT[1]);	// 다다음
	ausCTT[2]	= (pHeader) ? pHeader->usPageNum : ausCTT[1];
	pHeader 	= local_ttx_SearchNextPage(pDec, ausCTT[2]);	// 다다다음
	ausCTT[3]	= (pHeader) ? pHeader->usPageNum : ausCTT[2];
}

/**
 * Cycle Trace를 만든다.
 *
 * @param 	pDec	Instance of 'TTX Decoder'
 * @param	pPage	TTX Page
 * @return	ERR_OK / ERR_FAIL
 */
static HERROR local_ttx_MakeCTT (TTX_Decoder_t *pDec, TTX_Page_t *pPage)
{
	HUINT16	ausCTT[4];

	local_ttx_SearchCTT(pDec, pPage->usPageNum, ausCTT);

	if (pPage->TRACE.ulVersion > 0
		&& HxSTD_MemCmp(pPage->TRACE.ausTrace, ausCTT, sizeof(HUINT16) * 4) == 0)
	{
		return ERR_FAIL;
	}

	HxSTD_MemCopy(pPage->TRACE.ausTrace, ausCTT, sizeof(HUINT16) * 4);
	pPage->TRACE.ulVersion++;
	return ERR_OK;
}


/**
 * pagenum과 subcode를 기준으로, 해당 페이지의 서브페이지 리스트를 수집한다.
 * - 한 화면에 총 12개의 subcode를 보여줄 수 있어, 현재 subcode를 기준으로 12개의 subcode만을 찾아
 *   리스트를 구축한다.
 *
 * @param 	pDec	Instance of 'TTX Decoder'
 * @param	pPage	TTX Page
 * @return	ERR_OK / ERR_FAIL
 */
static HERROR	local_ttx_MakeSubcodeBar (TTX_Decoder_t *pDec, TTX_Page_t *pPage)
{
	HUINT8			aucList[TTX_MAX_SUBCODE_LIST];
	HUINT8			aucSort[TTX_MAX_SUBCODE_LIST];
	HUINT8			*pucList	= NULL;
	HUINT32			ulCount		= 0;
	HUINT32			ulMod		= 0;
	TTX_Header_t	*pHeader	= pDec->cache.apBuf[pPage->usPageNum];
	HUINT16			usLastSubcode	= 0;

	if (pHeader == NULL)
	{
		return ERR_FAIL;
	}

	while (pHeader)
	{
		if (TTX_IS_ROTATING_PAGE(pHeader->usSubcode))
		{
			return ERR_FAIL;
		}

		if (TTX_MAX_SUBCODE_LIST <= ulCount && pPage->usSubcode <= usLastSubcode)
		{
			break;
		}

		// 0x00~0x0079까지 이므로, 아래 한 바이트만 추출
		aucList[ulCount++ % TTX_MAX_SUBCODE_LIST]	= (HUINT8)(pHeader->usSubcode & 0x00FF);

		usLastSubcode	= pHeader->usSubcode;
		pHeader	= pHeader->pSubPage;
	}

	if (ulCount <= TTX_MAX_SUBCODE_LIST || ulCount % TTX_MAX_SUBCODE_LIST == 0)
	{
		pucList	= aucList;
		if (ulCount > TTX_MAX_SUBCODE_LIST)
			ulCount %= TTX_MAX_SUBCODE_LIST;
	}
	else
	{
		ulMod	= ulCount % TTX_MAX_SUBCODE_LIST;
		HxSTD_MemCopy(aucSort, aucList + ulMod, TTX_MAX_SUBCODE_LIST - ulMod);
		HxSTD_MemCopy(aucSort + TTX_MAX_SUBCODE_LIST - ulMod, aucList, ulMod);
		pucList	= aucSort;
		ulCount	= TTX_MAX_SUBCODE_LIST;
	}

	if (pPage->SubcodeBar.ulVersion > 0
		&& pPage->SubcodeBar.ulCount == ulCount
		&& HxSTD_MemCmp(pPage->SubcodeBar.aucList, pucList, ulCount) == 0)
	{
		return ERR_FAIL;
	}

	if (ulCount == 1 && pucList[0] == 0x00)	// subcode 0이고, 하나만 있으면, 서브페이지가 없다고 간주
	{
		return ERR_FAIL;
	}

	HxSTD_MemCopy(pPage->SubcodeBar.aucList, pucList, ulCount);
	pPage->SubcodeBar.ulCount	= ulCount;
	pPage->SubcodeBar.ulVersion++;

	return ERR_OK;
}


#define __def__ttx_searching_functions____


/**
 * search 타입에 따라 페이지를 케시에서 검색한 후 그 결과를 리턴
 *
 * @param 	pDec		Instance of 'TTX Decoder'
 * @param	usPageNum	Page number (0 ~ 799)
 * @param	usSubcode	Subcode
 * @param	search		Search type
 * @return	TTX_HeaderPacket_t의 인스턴스
 */
static TTX_Header_t *local_ttx_SearchPage (TTX_Decoder_t *pDec,
											const HUINT16 usPageNum, const HUINT16 usSubcode,
											const TTX_Search_t search)
{
	TTX_Header_t *pHeader	= NULL;

	switch (search)
	{
	case eTTX_SEARCH_NORMAL:
		pHeader = local_ttx_FetchFromCache(pDec, usPageNum, usSubcode);
		if (pHeader == NULL && usSubcode == TTX_IGNORE_SUBCODE)
			pHeader = pDec->cache.apBuf[usPageNum];
		break;

	case eTTX_SEARCH_NEXT_PAGE:
		pHeader = local_ttx_SearchNextPage(pDec, usPageNum);
		break;

	case eTTX_SEARCH_PREV_PAGE:
		pHeader = local_ttx_SearchPrevPage(pDec, usPageNum);
		break;

	case eTTX_SEARCH_NEXT_SUBPAGE:
		pHeader = local_ttx_SearchNextSubPage(pDec, usPageNum, usSubcode);
		break;

	case eTTX_SEARCH_PREV_SUBPAGE:
		pHeader = local_ttx_SearchPrevSubPage(pDec, usPageNum, usSubcode);
		break;

	case eTTX_SEARCH_FIRST_SUBPAGE:
		pHeader = pDec->cache.apBuf[usPageNum];
		break;

	case eTTX_SEARCH_LAST_SUBPAGE:
		pHeader = pDec->cache.apBuf[usPageNum];
		if (pHeader)
		{
			pHeader	= local_ttx_SearchPrevSubPage(pDec, pHeader->usPageNum, pHeader->usSubcode);
		}
		break;

	default:
		pHeader	= NULL;
		break;
	}
	return pHeader;
}


/**
 * usPageNum을 기준으로 다음 페이지를 검색한다.
 *
 * @param 	pDec		Instance of 'TTX Decoder'
 * @param	usPageNum	Page number (0 ~ 799)
 * @return	TTX_HeaderPacket_t의 인스턴스
 */
static TTX_Header_t *local_ttx_SearchNextPage (TTX_Decoder_t *pDec, const HUINT16 usPageNum)
{
	HUINT16			usNext		= usPageNum;
	TTX_Header_t	*pHeader	= NULL;

	while (pHeader == NULL)
	{
		usNext	= (usNext == 799) ? 0 : usNext + 1;

		if (usNext == usPageNum)	// 한바퀴 돌아도 없으면?
		{
			return local_ttx_FetchFromCache(pDec, usPageNum, TTX_IGNORE_SUBCODE);
		}

		pHeader = pDec->cache.apBuf[usNext];
	}

	return pHeader;
}


/**
 * usPageNum을 기준으로 이전 페이지를 검색한다.
 *
 * @param 	pDec		Instance of 'TTX Decoder'
 * @param	usPageNum	Page number (0 ~ 799)
 * @return	TTX_HeaderPacket_t의 인스턴스
 */
static TTX_Header_t *local_ttx_SearchPrevPage (TTX_Decoder_t *pDec, const HUINT16 usPageNum)
{
	HUINT16			usPrev		= usPageNum;
	TTX_Header_t	*pHeader	= NULL;

	while (pHeader == NULL)
	{
		usPrev	= (usPrev == 0) ? 799 : usPrev - 1;

		if (usPrev == usPageNum)	// 한바퀴 돌아도 없으면?
		{
			return local_ttx_FetchFromCache(pDec, usPageNum, TTX_IGNORE_SUBCODE);
		}

		pHeader = pDec->cache.apBuf[usPrev];
	}

	return pHeader;
}


/**
 * usPageNum와 usSubcode를 기준으로 다음 서브 페이지를 검색한다.
 *
 * @param 	pDec		Instance of 'TTX Decoder'
 * @param	usPageNum	Page number (0 ~ 799)
 * @param	usSubcode	Subcode
 * @return	TTX_HeaderPacket_t의 인스턴스
 */
static TTX_Header_t *local_ttx_SearchNextSubPage (TTX_Decoder_t *pDec,
												const HUINT16 usPageNum, const HUINT16 usSubcode)
{
	TTX_Header_t	*pHead	= pDec->cache.apBuf[usPageNum];
	TTX_Header_t	*pPkt	= NULL;

	if (pHead == NULL)
	{
		return NULL;
	}

	if (pHead->pSubPage == NULL)	// 페이지가 하나뿐!!
	{
		return pHead;
	}

	pPkt	= pHead;
	while (pPkt)
	{
		if (usSubcode < pPkt->usSubcode)
		{
			return pPkt;
		}

		pPkt	= pPkt->pSubPage;
	}

	return pHead;
}


/**
 * usPageNum와 usSubcode를 기준으로 이전 서브 페이지를 검색한다.
 *
 * @param 	pDec		Instance of 'TTX Decoder'
 * @param	usPageNum	Page number (0 ~ 799)
 * @param	usSubcode	Subcode
 * @return	TTX_HeaderPacket_t의 인스턴스
 */

static TTX_Header_t *local_ttx_SearchPrevSubPage (TTX_Decoder_t *pDec,
														const HUINT16 usPageNum, const HUINT16 usSubcode)
{
	TTX_Header_t	*pHead	= pDec->cache.apBuf[usPageNum];
	TTX_Header_t	*pPkt	= NULL;
	TTX_Header_t	*pTail	= NULL;

	if (pHead == NULL)
	{
		return NULL;
	}

	if (pHead->pSubPage == NULL)	// 페이지가 하나??
	{
		return pHead;
	}

	if (pHead->usSubcode == usSubcode)	// 기준페이지가 가장 앞 서브페이지면 Tail페이지를 찾는다.
	{
		pTail	= pHead;

		while (pTail->pSubPage)
		{
			pTail = pTail->pSubPage;
		}

		return pTail;
	}

	pPkt	= pHead->pSubPage;		// Scan...
	while (pPkt)
	{
		if (pPkt->usSubcode >= usSubcode)
		{
			return pHead;
		}

		pHead	= pHead->pSubPage;
		pPkt	= pPkt->pSubPage;
	}

	// 만약 여기까지 왔다면, usSubcode가 Cache에 있는 어떤 서브페이지보다 크다는 것이므로,
	// Tail을 리턴
	pTail	= pHead;
	return pTail;
}


#define	__def__TTX_FLOF____

static HERROR	local_ttx_DecodeFLOF (HUINT8 *pucPacket)
{
	HINT32	err;

	// Byte '0' is the designation code, coded Hamming 8/4
	if (TTX_ERRCHK_Hamming84(pucPacket, pucPacket, 1) != 0)
	{
		return ERR_FAIL;
	}

	switch (pucPacket[0])
	{
	case 0x0:	case 0x1:	case 0x2:	case 0x3:
		// Bytes 1 to 37 are coded Hamming 8/4 and are arranged as 6 groups of 6 bytes.
		err = TTX_ERRCHK_Hamming84(pucPacket + 1, pucPacket + 1, 37);
		break;

	case 0x4:	case 0x5:	case 0x6:	case 0x7:
		// Bytes 1 to 29 area arranged as 6 groups of 3 + 3 bytes,
		// each sub-group of 3 bytes being one Hamming 24/18 coded triplet.
		err = TTX_ERRCHK_Hamming2418(pucPacket + 1, pucPacket + 1, 39);
		break;

	default:
		err = -1;
	}

	return (err == 0) ? ERR_OK : ERR_FAIL;
}

static TTX_Packet_t * local_ttx_ParseFLOF (HUINT16 usPageNum, TTX_FLOF_t *pFLOF, TTX_Packet_t *pY27)
{
	HUINT8		*pucY27		= NULL;
	HUINT32		i			= 0;
	HUINT8		ucCurM		= 0;
	HUINT8		ucM			= 0;
	HUINT8		ucX			= 0;
	HUINT8		ucRelativeM	= 0;
	HUINT16		usSubcode	= 0;
	TTX_Link_t	*pLink		= NULL;

	while (pY27 && pY27->ucY == 27)
	{
		// if Designation code == 0
		if (pY27->aucData[0] == 0 && pucY27 == NULL)
		{
			pucY27 = pY27->aucData + 1;	// skip designation code...
		}

		pY27 = pY27->pNextPacket;
	}

	if (pucY27 == NULL)
	{
		return pY27;
	}

	pFLOF->bEnabled		= TRUE;
	pFLOF->bLinkControl	= (pucY27[36] & 0x08) ? TRUE : FALSE;

	// Dec to Hex
	ucCurM	= (HUINT8)(usPageNum / 100);

	for (i = 0 ; i < 6 ; i++, pucY27 += 6)
	{
		ucRelativeM = ((pucY27[3] & 0x08) >> 3) | ((pucY27[5] & 0x0C) >> 1);
		ucM			= (ucCurM ^ ucRelativeM);
		ucX			= ((pucY27[0] & 0x0F) | ((pucY27[1] & 0x0F) << 4));
		usSubcode	= (pucY27[2] & 0x0F)
					| ((pucY27[3] & 0x07) << 4)
					| ((pucY27[4] & 0x0F) << 8)
					| ((pucY27[5] & 0x03) << 12);

		if (!TTX_IS_VALID_ADDR(ucM, ucX))
		{
			continue;
		}

		if (i <= 3)	// Color Key
		{
			pLink	= &pFLOF->aColorKey[i];
		}
		else if (i == 4)	// Addtion...
		{
			pLink	= &pFLOF->addtion;
		}
		else	// Index Key
		{
			pLink	= &pFLOF->indexKey;
		}

		pLink->bEnabled		= TRUE;
		pLink->usPageNum	= TTX_HEX_ADDR_TO_DEC(ucM, ucX);
		pLink->usSubcode	= usSubcode;
	}

	return pY27;
}

#define	__def__ttx_debug____

#if defined(CONFIG_DEBUG)
static void	local_ttx_DebugPage (TTX_Header_t *pHeader, TTX_Page_t *pPage)
{
	FILE			*fpOut;
	TTX_Packet_t	*pPacket	= NULL;
	HUINT8			ucY;
	HUINT8			ucCode;
	HUINT32			i;
	HUINT16			usPageNum;
	HUINT8			szFileName[100];

	static	HUINT32		s_ulNumber;

	if (pHeader == NULL)
	{
		return;
	}

	if (!(pHeader->ucOptionalField & eTTX_OPT_DATA_PAGE))
	{
		usPageNum	= ( (pHeader->usPageNum / 100) << 8)
					| (((pHeader->usPageNum % 100) / 10) << 4)
					| ( (pHeader->usPageNum % 10));
	}
	else
		usPageNum	= pHeader->usPageNum;

#if defined(WIN32)
	sprintf_s((char *)szFileName, 100,
		"ttx\\%04d_ttx_debug_%08u(%03X_%04X).txt",
#else
	snprintf((char *)szFileName, 100,
		"/opt/%04d_ttx_debug_%08u(%03X_%04X).txt",
#endif
		(int)++s_ulNumber,
		(unsigned int)VK_TIMER_GetSystemTick(),
		(int)usPageNum,
		(int)pHeader->usSubcode
		);

	fpOut	= fopen((char *)szFileName, "w");
	if (fpOut == NULL)
	{
		HxLOG_Print("[local_ttx_DebugPage] File Open Fail:%s\n", szFileName);
		return;
	}

	fprintf(fpOut,
			"[TTX_Packet(s) Dump]\r\n"
			"                                MXX-SSSS: %03d-%04X\r\n"
			"                          C4(Erage Page): %s\r\n"
			"                           C5(Newsflash): %s\r\n"
			"                            C6(Subtitle): %s\r\n"
			"                     C7(Suppress Header): %s\r\n"
			"                    C8(Update Indicator): %s\r\n"
			"                C9(Interrupted Sequence): %s\r\n"
			"                    C10(Inhibit Display): %s\r\n"
			"                    C11(Magazine Serial): %s\r\n"
			"        National option character subset: 0x%X\r\n",
			pHeader->usPageNum, pHeader->usSubcode,
			(pHeader->ucControlBit & eTTX_C4_ERASE_PAGE				) ? "TRUE" : "FALSE",
			(pHeader->ucControlBit & eTTX_C5_NEWSFLASH				) ? "TRUE" : "FALSE",
			(pHeader->ucControlBit & eTTX_C6_SUBTITLE				) ? "TRUE" : "FALSE",
			(pHeader->ucControlBit & eTTX_C7_SUPPRESS_HEADER		) ? "TRUE" : "FALSE",
			(pHeader->ucControlBit & eTTX_C8_UPDATE_INDICATOR		) ? "TRUE" : "FALSE",
			(pHeader->ucControlBit & eTTX_C9_INTERRUPTED_SEQUENCE	) ? "TRUE" : "FALSE",
			(pHeader->ucControlBit & eTTX_C10_INHIBIT_DISPLAY		) ? "TRUE" : "FALSE",
			(pHeader->ucControlBit & eTTX_C11_MAGAZINE_SERIAL		) ? "TRUE" : "FALSE",
			pHeader->ucNationalOption
			);
	pPacket	= (TTX_Packet_t *)pHeader;
	while (pPacket)
	{
		ucY	= pPacket->ucY;

		if (ucY <= 25)
		{
			fprintf(fpOut, "%8d: ", ucY);
			for (i = 0 ; i < 40 ; i++)
			{
				ucCode	= pPacket->aucData[i];
				if (0x20 <= ucCode && ucCode < 0x80)
					fprintf(fpOut, "%c", ucCode);
				else
					fprintf(fpOut, ".");
			}

			fprintf(fpOut, "\t");

			for (i = 0 ; i < 40 ; i++)
			{
				ucCode	= pPacket->aucData[i];
				if (i % 4 == 0)
					fprintf(fpOut, " ");
				if (isalpha(ucCode) || isdigit(ucCode))
					fprintf(fpOut, "%c_", ucCode);
				else
					fprintf(fpOut, "%02X", ucCode);
			}
			fprintf(fpOut, "\r\n");
		}
		else if (ucY == 26)
		{
			local_ttx_DebugY26(fpOut, pPacket);
		}
		else
		{
			fprintf(fpOut, "(%2d)%4d: ", pPacket->aucData[0], ucY);

			for (i = 0 ; i < 40 ; i++)
			{
				fprintf(fpOut, ".");
			}

			fprintf(fpOut, "\t");

			for (i = 0 ; i < 40 ; i++)
			{
				if (i % 4 == 0)
					fprintf(fpOut, " ");
				fprintf(fpOut, "%02X", pPacket->aucData[i]);
			}
			fprintf(fpOut, "\r\n");
		}

		pPacket = pPacket->pNextPacket;
	}

	if (pPage)
	{
		HUINT8	ucRow, ucCol;
		TTX_Cell_t	*pCell;

		fprintf(fpOut,
				"\r\n\r\n[TTX_Page Dump]\r\n"
				"\t");

		for (ucRow = 0 ; ucRow < TTX_MAX_ROW ; ucRow++)
		{
			for (ucCol = 0 ; ucCol < TTX_MAX_COLUMN ; ucCol++)
			{
				pCell	= &pPage->aCellMap[ucRow][ucCol];
				fprintf(fpOut,
					"[%02d/%02d]: "
					"Code(%02d), NOS(%02d), Char(%c), FG(%02d), BG(%02d), Double(W:%d,H:%d)"
					", Opt(%02d), Conceal(%1d), Flash(%1d), Composite Char(%1d), Displayable(%1d)"
					"\r\n",
					ucRow, ucCol,
					pCell->ucCode, pCell->ucNationalOption, pCell->ucChar, pCell->ucFgColor, pCell->ucBgColor, pCell->bDoubleWidth, pCell->bDoubleHeight,
					pCell->Opt.ucReserved, pCell->bConceal, pCell->bFlash, pCell->bCompositeChar, pCell->bDisplayable
				);
			}
		}
	}
	fclose(fpOut);
}

static void	local_ttx_DebugY26 (FILE *fp, TTX_Packet_t *pY26)
{
	HUINT32	i;
	HUINT8	ucAddr, ucMode, ucData;
	HUINT8	ucDesgCode	= pY26->aucData[0];
	HUINT8	*pucTriplet	= &(pY26->aucData[1]);

	fprintf(fp, "(%2d)%4d: >>>\r\n", ucDesgCode, pY26->ucY);

	for (i = 0 ; i< 13 ; i++, pucTriplet += 3)
	{
		ucAddr	= pucTriplet[0] | ((pucTriplet[1] & 0x3) << 4);
		ucMode	= pucTriplet[1] >> 2;
		ucData	= pucTriplet[2];

		if (ucAddr <= 39)	// Column Address Group
		{
			switch (ucMode)
			{
			case 0x00:	fprintf(fp, "\t::[%02d,%02X,%02X] Foreground Colour\r\n", ucAddr, ucMode, ucData);	break;
			case 0x01:	fprintf(fp, "\t::[%02d,%02X,%02X] Block Mosaic Character from the G1 set\r\n", ucAddr, ucMode, ucData);	break;
			case 0x02:	fprintf(fp, "\t::[%02d,%02X,%02X] Line Drawing or Smoothed Mosaic Character from the G3 set (Level 1.5)\r\n", ucAddr, ucMode, ucData);	break;
			case 0x03:	fprintf(fp, "\t::[%02d,%02X,%02X] Background Colour\r\n", ucAddr, ucMode, ucData);	break;
			case 0x04:	fprintf(fp, "\t::[%02d,%02X,%02X] Reserved\r\n", ucAddr, ucMode, ucData);	break;
			case 0x05:	fprintf(fp, "\t::[%02d,%02X,%02X] Reserved\r\n", ucAddr, ucMode, ucData);	break;
			case 0x06:	fprintf(fp, "\t::[%02d,%02X,%02X] PDC - Cursor Column & Announced Starting & Finishing Time Minutes\r\n", ucAddr, ucMode, ucData);	break;
			case 0x07:	fprintf(fp, "\t::[%02d,%02X,%02X] Additional Flash Functions\r\n", ucAddr, ucMode, ucData);	break;
			case 0x08:	fprintf(fp, "\t::[%02d,%02X,%02X] Modified G0 and G2 Character Set Design.\r\n", ucAddr, ucMode, ucData);	break;
			case 0x09:	fprintf(fp, "\t::[%02d,%02X,%02X] Character from the G0 set (Levels 2.5 & 3.5)\r\n", ucAddr, ucMode, ucData);	break;
			case 0x0A:	fprintf(fp, "\t::[%02d,%02X,%02X] Reserved\r\n", ucAddr, ucMode, ucData);	break;
			case 0x0B:	fprintf(fp, "\t::[%02d,%02X,%02X] Line Drawing or Smoothed Mosaic Character from the G3 set (Levels 2.5 & 3.5)\r\n", ucAddr, ucMode, ucData);	break;
			case 0x0C:	fprintf(fp, "\t::[%02d,%02X,%02X] Display Attributes\r\n", ucAddr, ucMode, ucData);	break;
			case 0x0D:	fprintf(fp, "\t::[%02d,%02X,%02X] DRCS Character Invocation\r\n", ucAddr, ucMode, ucData);	break;
			case 0x0E:	fprintf(fp, "\t::[%02d,%02X,%02X] Font Style\r\n", ucAddr, ucMode, ucData);	break;
			case 0x0F:	fprintf(fp, "\t::[%02d,%02X,%02X] Character from the G2 set\r\n", ucAddr, ucMode, ucData);	break;
			case 0x10:	fprintf(fp, "\t::[%02d,%02X,%02X] G0 character without diacritical mark\r\n", ucAddr, ucMode, ucData);	break;
			case 0x11:	case 0x12:	case 0x13:	case 0x14:	case 0x15:
			case 0x16:	case 0x17:	case 0x18:	case 0x19:	case 0x1A:
			case 0x1B:	case 0x1C:	case 0x1D:	case 0x1E:
			case 0x1F:	fprintf(fp, "\t::[%02d,%02X,%02X] G0 character with diacritical mark\r\n", ucAddr, ucMode, ucData);	break;
			default:	fprintf(fp, "\t::[%02d,%02X,%02X] ERROR!!\r\n", ucAddr, ucMode, ucData);	break;
			}
		}
		else	// Row Address Group
		{
			switch (ucMode)
			{
			case 0x00:	fprintf(fp, "\t::[%02d,%02X,%02X] Full Screen Colour\r\n", ucAddr, ucMode, ucData);	break;
			case 0x01:	fprintf(fp, "\t::[%02d,%02X,%02X] Full Row Colour\r\n", ucAddr, ucMode, ucData);	break;
			case 0x02:	fprintf(fp, "\t::[%02d,%02X,%02X] Reserved\r\n", ucAddr, ucMode, ucData);	break;
			case 0x03:	fprintf(fp, "\t::[%02d,%02X,%02X] Reserved\r\n", ucAddr, ucMode, ucData);	break;
			case 0x04:	fprintf(fp, "\t::[%02d,%02X,%02X] Set Active Position\r\n", ucAddr, ucMode, ucData);	break;
			case 0x05:	fprintf(fp, "\t::[%02d,%02X,%02X] Reserved\r\n", ucAddr, ucMode, ucData);	break;
			case 0x06:	fprintf(fp, "\t::[%02d,%02X,%02X] Reserved\r\n", ucAddr, ucMode, ucData);	break;
			case 0x07:	fprintf(fp, "\t::[%02d,%02X,%02X] Address Display Row 0 \r\n", ucAddr, ucMode, ucData);	break;
			case 0x08:	fprintf(fp, "\t::[%02d,%02X,%02X] PDC - Country of Origin and Programme Source\r\n", ucAddr, ucMode, ucData);	break;
			case 0x09:	fprintf(fp, "\t::[%02d,%02X,%02X] PDC - Month & Day \r\n", ucAddr, ucMode, ucData);	break;
			case 0x0A:	fprintf(fp, "\t::[%02d,%02X,%02X] PDC - Cursor Row & Announced Starting Time Hours\r\n", ucAddr, ucMode, ucData);	break;
			case 0x0B:	fprintf(fp, "\t::[%02d,%02X,%02X] PDC - Cursor Row & Announce Finishing Time Hours\r\n", ucAddr, ucMode, ucData);	break;
			case 0x0C:	fprintf(fp, "\t::[%02d,%02X,%02X] PDC - Cursor Row & Local Time Offset \r\n", ucAddr, ucMode, ucData);	break;
			case 0x0D:	fprintf(fp, "\t::[%02d,%02X,%02X] PDC - Series Identifier and Series Code\r\n", ucAddr, ucMode, ucData);	break;
			case 0x0E:	fprintf(fp, "\t::[%02d,%02X,%02X] Reserved\r\n", ucAddr, ucMode, ucData);	break;
			case 0x0F:	fprintf(fp, "\t::[%02d,%02X,%02X] Reserved\r\n", ucAddr, ucMode, ucData);	break;
			case 0x10:	fprintf(fp, "\t::[%02d,%02X,%02X] Origin Modifier\r\n", ucAddr, ucMode, ucData);	break;
			case 0x11:	fprintf(fp, "\t::[%02d,%02X,%02X] Active Object Invocation\r\n", ucAddr, ucMode, ucData);	break;
			case 0x12:	fprintf(fp, "\t::[%02d,%02X,%02X] Adaptive Object Invocation\r\n", ucAddr, ucMode, ucData);	break;
			case 0x13:	fprintf(fp, "\t::[%02d,%02X,%02X] Passive Object Invocation\r\n", ucAddr, ucMode, ucData);	break;
			case 0x14:	fprintf(fp, "\t::[%02d,%02X,%02X] Reserved\r\n", ucAddr, ucMode, ucData);	break;
			case 0x15:	fprintf(fp, "\t::[%02d,%02X,%02X] Active Object Definition\r\n", ucAddr, ucMode, ucData);	break;
			case 0x16:	fprintf(fp, "\t::[%02d,%02X,%02X] Adaptive Object Definition\r\n", ucAddr, ucMode, ucData);	break;
			case 0x17:	fprintf(fp, "\t::[%02d,%02X,%02X] Passive Object Definition\r\n", ucAddr, ucMode, ucData);	break;
			case 0x18:	fprintf(fp, "\t::[%02d,%02X,%02X] DRCS Mode\r\n", ucAddr, ucMode, ucData);	break;
			case 0x19:	case 0x1A:	case 0x1B:	case 0x1C:	case 0x1D:
			case 0x1E:	fprintf(fp, "\t::[%02d,%02X,%02X] Reserved\r\n", ucAddr, ucMode, ucData);	break;
			case 0x1F:	fprintf(fp, "\t::[%02d,%02X,%02X] Termination Marker\r\n", ucAddr, ucMode, ucData);	break;
			default:	fprintf(fp, "\t::[%02d,%02X,%02X] ERROR!!\r\n", ucAddr, ucMode, ucData);	break;
			}
		}
	}

	fprintf(fp, "        <<<\r\n");
}
#endif

#if defined(CONFIG_DEBUG)
static void	local_ttx_DebugPacketMonitor (TTX_Decoder_t *pDec, const HUINT8 ucM, const HUINT8 ucY, const HUINT8 aucPacket[])
{
	HUINT32	i;
	HUINT8	aucBuf[TTX_PACKET_DATA_BYTES];
	FILE	*fp;

	if (pDec->fpPacket == NULL)
	{
		pDec->fpPacket = fopen("/opt/packet_monitor.txt", "w");
	}
	if (pDec->fpPacket == NULL)
		return;
	fp	= pDec->fpPacket;

	if (ucY == 0)
	{
		(void)TTX_ERRCHK_Hamming84(aucBuf + 0, aucPacket + 0, 8);
		(void)TTX_ERRCHK_OddParity(aucBuf + 8, aucPacket + 8, TTX_HEADER_DATA_BYTES);
		fprintf(fp,	"[%08u][%X%X%X-%04X]",
					(unsigned int)VK_TIMER_GetSystemTick(),
					ucM,
					aucBuf[1],
					aucBuf[0],
					TTX_SUBCODE(aucBuf));
		fprintf(fp, "[C4-11:");
		fprintf(fp, "%01d", (aucBuf[3] & 0x8) ? 1 : 0);	// C4
		fprintf(fp, "%01d", (aucBuf[5] & 0x4) ? 1 : 0);	// C5
		fprintf(fp, "%01d", (aucBuf[5] & 0x8) ? 1 : 0);	// C6
		fprintf(fp, "%01d", (aucBuf[6] & 0x1) ? 1 : 0);	// C7
		fprintf(fp, "%01d", (aucBuf[6] & 0x2) ? 1 : 0);	// C8
		fprintf(fp, "%01d", (aucBuf[6] & 0x4) ? 1 : 0);	// C9
		fprintf(fp, "%01d", (aucBuf[6] & 0x8) ? 1 : 0);	// C10
		fprintf(fp, "%01d", (aucBuf[7] & 0x1) ? 1 : 0);	// C11
		fprintf(fp, "][NOS:");
		fprintf(fp, "%01d", (aucBuf[7] & 0x8) ? 1 : 0);	// C14
		fprintf(fp, "%01d", (aucBuf[7] & 0x4) ? 1 : 0);	// C13
		fprintf(fp, "%01d", (aucBuf[7] & 0x2) ? 1 : 0);	// C12
		fprintf(fp, "][");
		for (i = 0 ; i < TTX_HEADER_DATA_BYTES ; i++)
		{
				fprintf(fp, "%c", aucBuf[i + 8]);
		}
		fprintf(fp, "]\r\n");
	}
	else if (ucY <= 25)
	{
		fprintf(fp, "          [%X/%02d]\r\n", ucM, ucY);
	}
	else if (ucY == 28)
	{
		HxSTD_MemCopy(aucBuf, aucPacket, TTX_PACKET_DATA_BYTES);
		i = local_ttx_DecodePacket(aucBuf, eTTX_CODING_HAMMING2418);
		fprintf(fp, "          [%X/%02d-%X][err:%d] ", ucM, ucY, aucBuf[0], i);
		for (i = 1 ; i < TTX_PACKET_DATA_BYTES ; i++)
		{
			fprintf(fp, "%02X", aucBuf[i]);
		}
		fprintf(fp, "\r\n");
	}
	else
	{
		(void)TTX_ERRCHK_Hamming84(aucBuf, aucPacket, 1);	// Designation Code
		fprintf(fp, "          [%X/%02d-%X]\r\n", ucM, ucY, aucBuf[0]);
	}

}
#endif


