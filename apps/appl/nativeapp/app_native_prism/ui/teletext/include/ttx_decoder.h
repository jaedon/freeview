
/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  layer_module.h File Name을 적습니다.
	@brief	  file_name & simple comment.

	Description: File에 대한 설명을 적습니다.		\n
	Module: Layer와 모듈 명을 적습니다.			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__TTX_DECODER_H__
#define	__TTX_DECODER_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <stdarg.h>
#include	<ondk.h>
//#include <octo_common.h>

#include <ttx_top.h>


//1 WORKAROUND!!

#define	TTX_ENABLE			1
#define	TTX_DISABLE			0

/**
 * Astra의 Arte채널에서 100, 101, 102페이지가 보이지 않는 문제
 *
 * 원인: 페이지에서 28번 패킷에 Page Function을 POP, 그리고 Page Coding은 0x7로
 *		잘못된 정보를 보내주고 있었음.
 * 수정: 28번 패킷을 통해 Page Function과 Coding을 지정할 수는 있지만, 페이지를 그리는 루틴 등이
 *		Odd parity에 맞춰져 있기 때문에 일반적이지 않다고 판단됨.
 *		X/28의 Page Function / Coding Parsing을 Disable한다.
 *
 * @date	2009-??-??
 * @author	jhlee4
 */
#define	TTX_WORKAROUND_01	TTX_DISABLE

/**
 * Packet Demuxing/Decoding 과정 중 디코딩 에러로 인한 Packet loss가 발생
 *
 * 페이지가 업데이트(내용이 바뀜)되지 않고 재전송될 경우, 이전에 존재하던 페이지와 Merging하여
 * Packet loss에 의한 불완전한 페이지를 보다 완전하게 복구해 나간다.
 *
 * 이 Workaround를 Enable 하면, Merging 과정에의해 보다 많은 리소스!!! 를 사용하게 될 것이다.
 *
 * !!! 시그널이 좋지 않은 경우 Enable하면 페이지가 더 이쁘게(?) 나올 것이다. !!!
 *
 * @date	2009-03-22
 * @author	jhlee4
 *
 * 우연찮게도 이 구현내용이
 *     "Page Overwrite :ERAGE_PAGE or UPDATE_INDICATOR 로 인해 Memory내의 페이지를 지우지 않고, 덮어쓰는 경우"
 * 에 딱 드러 맞는다. 아직 긴가민가 하는 내용이지만, 정상 동작하는 것을 확인 하였고, 이것은 SPEC 사양이므로,
 * 이 Workaround는 더이상 workaround로 처리하지 않는다(이제 기본 포함 사항).
 *
 * --기존 위치--
 * local_ttx_ReceiveLOP () {
 *     ...
 *   <TTX_WORKAROUND_02>
 *     if (!(pHeader->ucControlBit & eTTX_CX_PAGE_UPDATE)) {
 *         ...
 *     }
 *   </TTX_WORKAROUND_02>
 *     ...
 * }
 *
 * @date	2009-05-14
 * @author	jhlee4
 */
#define	TTX_WORKAROUND_02


/**
 * Parallel Transmission 중 같은 메거진에 하나의 페이지만 들어올 경우,
 * Page Terminating 조건을 만족시키지 못해 패킷이 무한정 Pipe에  쌓여버린다.
 * 이 Define을 Enable하면 0.3초 내에 하나의 페이지를 받지 못한 경우, 페이지를 강제 종료 시켜,
 * 메모리를 정상 유지 시킨다.
 *
 * @date	2009-??-??
 *
 * 위의 사항은 스펙에 명시되어 있는
 * "For all types of pages, the maximum time intrval betweeen the transmission
 *   of successive packets of the same page shall be 100 ms"
 * 을 기준으로 적용하였으나, 영국 ATV 채널에서 페이지가 완성되는 시간이 0.3초보다
 * 더 걸리는 경우가 발생해 페이지가 잘리는 현상이 생겼다.
 *
 * 이에 페이지 강제 종료를 시간을 기준으로 하지 않고, 한 페이지에 가장 많이 들어올 수 있는 패킷의
 * 수를 넘어가면 종료하도록 수정한다.
 *
 * @date	2009-03-24
 *
 * >>> Parallel Transmission에서 하나의 페이지만 들어오는 것이 정상적이지 않은 것이나,
 *     위와 같이 '정상적이지 않게' 데이터가 들어올 때 Disable할 경우,
 *     MEMPOOL(Memory) Underflow에 의한 Cache Reset(Page를 처음부터 다시 받아야 함)이
 *     일어날 수 있다.  <<<
 *
 * @author	jhlee4
 */
#define	TTX_WORKAROUND_03	TTX_ENABLE


/**
 * Packet 8/25에 의한 패킷 손실(?)
 * [Comment]
 * - 8/25에 대한 내용은 Dynamic packet allocation [teletext service improvement] (Weitzel, P. BBC Projects, London)
 * 에 소개된다. 그러나 구할 수 없다.... ㅜㅠ, 구하면!!!!!!!!!! 공부하고 적용하자!!!!!
 * - 현재 ETSI 스펙(기본 TTX Spec및 Code of practice)에는 8/25에 대한 어떠한 언급도 없어 어떻게 처리를 해야 할지 모르는 상태
 *
 * [증상]
 * - JVC Stream(Doku Kika 등)에서 페이지가 주기적으로 한줄씩 비어서 나옴
 *
 * [효과]
 * - 이 Workaround를 Enable하면 페이지 전송 과정 중간에 8/25 패킷이 들어오면 마지막 메거진 파이프에
 * 디먹싱되고 있는 모든 패킷을 초기화 하여, 페이지 중간이 비는 것을 방지한다.
 *
 * [발생 가능한 Side Effect]
 * - 디먹싱을 한번식 건너뛰는 것이기 때문에, 페이지 업데이트 타이밍이 다를 수 있음
 * - 최악의 경우, (아마 그럴일은 없겠지만) 페이지 중간에 항상 8/25가 딸려 들어오면 페이지를 받을 수 없다.
 *
 * @date	2009-03-26
 * @author	jhlee4
 */
#if defined(CONFIG_DB1) || defined(CONFIG_HB1)
#define TTX_WORKAROUND_04	TTX_ENABLE
#else
#define TTX_WORKAROUND_04	TTX_DISABLE
#endif

/**
 * Page Termination 조건
 *
 * spec에 명시된 "different page number"를 지키지 않는 일부 서비스의 경우,
 * same page number가 하나의 magazine에 연속해서 들어온 경우 page를 종료하지 않고,
 * 연속해서 하나의 페이지로 처리하였었으나, 그 역시 정상적으로 동작하지 않으므로
 * "Different Page Number" 조건은 무시한다.
 *
 * @date	2010-01-07
 * @author	jhlee4
 */
#define TTX_WORKAROUND_05	TTX_ENABLE

/**
 * Signal을 제거하면 시간이 가지 않는다.
 *
 * if Enable {
 *    Signal을 제거해도 Tick을 기준으로 시간이 간다.
 * } else {
 *    Signal을 제거하면 시간이 가지 않는다.
 * }
 */
#define TTX_WORKAROUND_06	TTX_DISABLE

#define TTX_WORKAROUND_07
#define TTX_WORKAROUND_08




/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/


/**
 * 컴파일 warning을 제거하기 위한 매크로
 *
 * @param param
 */
#define	TTX_UNUSED_PARAM(param)	(void)(param)


/**
 * Page number가 적합한지 여부
 * @param X	 Page number
 */
#define	TTX_IS_VALID_PAGE(X)		((((X) >> 4) <= 0x9) && (((X) & 0xF) <= 0x9))

/**
 * Magazine이 적합한지 여부
 * @param M	 Magazine
 */
#define	TTX_IS_VALID_MAGAZINE(M)	((M) <= 0x08)

/**
 * Magazine, Page number가 적합한지 여부
 * @param M	 Magazine
 * @param X	 Page number
 */
#define	TTX_IS_VALID_ADDR(M,X)		(TTX_IS_VALID_MAGAZINE((M)) && TTX_IS_VALID_PAGE((X)))


/**
 * Hex로 되어 있는 Page Address를 계산의 편의를 위해 Dec으로 변환
 * @param M	 Magazine
 * @param X	 Page number
 */
#define	TTX_HEX_ADDR_TO_DEC(M,X)	(((M) % 8) * 100 + ((X) >> 4) * 10 + ((X) & 0xF))
#define  TTX_HEX_ADDR_TO_DEC_TNTSAT(M,X) (((M) % 8) * 100 + X)			/* in case of TNTSAT Canal Ready, page number 88 is encoded 0x58 instead of 0x88 */

/**
 * DEC로 되어 있는 Page Address를 HEX Magazine 으로 변환
 */
#define	TTX_DEC_ADDR_TO_HEX_M(addr)	((HUINT8)((addr) / 100))

/**
 * DEC로 되어 있는 Page Address를 HEX Page Num 으로 변환
 */
#define	TTX_DEC_ADDR_TO_HEX_X(addr)	((HUINT8)(((((addr) % 100) / 10) << 4) | ((addr) % 10)))


#define TTX_ELAPSED_TICK(prev,cur)	(((prev) > (cur)) ? ((0xFFFFFFFFUL - (prev)) + (cur)) : ((cur) - (prev)))

#define	TTX_IS_ROTATING_PAGE(subcode)	(((subcode) & 0xFF80) || (((subcode) & 0x00F0) > 0x0070) || (((subcode) & 0x000F) > 0x0009))

/**
 * Bit Operation
 */
#define TTX_BITSET(byte,bit)	((byte) |= (bit))
#define TTX_BITCLR(byte,bit)	((byte) &= ~(bit))
#define	TTX_BITFLP(byte,bit)	((byte) ^= (bit))

#define TTX_PACKET_LENGTH		43		/* Framing code부터 나머지 byte (Digital ttx & Analog ttx 공통 부분) */
#define	TTX_MAX_ROW				25		// 0 ~ 24
#define	TTX_MAX_COLUMN			40		// 0 ~ 39, 만약 Side Panel을 적용하면 56이 된다.
#define	TTX_CHAR_SPACE			0x20	// ' '
#define	TTX_TIME_FIELD_LENGTH	8		// Time 정보가 들어가는 Cell의 크기
#define	TTX_IGNORE_SUBCODE		0x3F7F	// Subcode의 최대값으로 서브코드를 무시

#define	TTX_MAX_CLUT			4
#define	TTX_MAX_COLOR_ENTRY		8
#define	TTX_MAX_SUBCODE_LIST	12		// 화면에 보여줄 수 있는 최대 Subcode 개수
#define	TTX_PTU_LENGTH			20

/**
 * Default Teletext Color
 *
 */
enum tagTTX_Color
{
	eTTX_COLOR_BLACK,
	eTTX_COLOR_RED,
	eTTX_COLOR_GREEN,
	eTTX_COLOR_YELLOW,
	eTTX_COLOR_BLUE,
	eTTX_COLOR_MAGENTA,
	eTTX_COLOR_CYAN,
	eTTX_COLOR_WHITE,
	eTTX_COLOR_LAST
};


/**
 * National Option Character Subset
 */
enum tagTTX_NationalOptionSubset
{
	eTTX_NOS_ENG,
	eTTX_NOS_GER,
	eTTX_NOS_SWE_FIN_HUN,
	eTTX_NOS_ITA,
	eTTX_NOS_FRE,
	eTTX_NOS_POR_SPA,
	eTTX_NOS_CZE_SLO,
	eTTX_NOS_POL,
	eTTX_NOS_EST,
	eTTX_NOS_LET_LIT,
	eTTX_NOS_RUM,
	eTTX_NOS_SCC_SCR_SLV,
	eTTX_NOS_TUR,
	eTTX_NOS_EMPTY,

	eTTX_NOS_LAST,

	eTTX_NOS_MAX	= 0x7
};


/**
 * Teletext Character code
 */
enum tagTTX_CharCode
{
	eTTX_CODE_G0_LATIN,			///< Latin G0 Primary Set
	eTTX_CODE_G0_CYRILLIC_1,	///< Cyrillic G0 Primary Set - Option 1 - Serbian/Croatian
	eTTX_CODE_G0_CYRILLIC_2,	///< Cyrillic G0 Primary Set - Option 2 - Russian/Bulgarian
	eTTX_CODE_G0_CYRILLIC_3,	///< Cyrillic G0 Primary Set - Option 3 - Ukrainian
	eTTX_CODE_G0_GREEK,			///< Greek G0 Priamry Set
	eTTX_CODE_G0_ARBIC,			///< Arbic G0 Primary Set
	eTTX_CODE_G0_HEBREW,		///< Hebrew G0 Primary Set

	eTTX_CODE_G1_MOSAIC,		///< G1 Block Mosaics Set

	eTTX_CODE_G2_LATIN,			///< Latin G2 Supplementary Set
	eTTX_CODE_G2_CYRILLIC,		///< Cyrillic G2 Supplementary Set
	eTTX_CODE_G2_GREEK,			///< Greek G2 Supplementary Set
	eTTX_CODE_G2_ARBIC,			///< Arbic G2 Supplementary Set

	eTTX_CODE_G3_MOSAIC,		///< G3 line Drawing & Smooth Mosaic

	eTTX_CODE_USER_DEFINED,

	eTTX_CODE_LAST
};



/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/**
 * 텔레텍스트 API 결과
 *
 */
typedef enum
{
	ERR_TTX_OK		= ERR_OK,		///< OK
	ERR_TTX_FAIL,					///< Fail
	ERR_TTX_NO_PAGE,				///< Cache에 저장된 페이지가 없다.
	ERR_TTX_NO_TELETEXT,			///< Decoder에 Teletext 데이터가 없다.
	ERR_TTX_NO_RECEIVER,			///< Decoder에 현재 Page Receiver가 물려있지 않다.
	ERR_TTX_DECODE_FAIL,			///< Decode Fail
	ERR_TTX_INVALID_ARGUMENT,		///< Invalid argument
	ERR_TTX_DISABLED,				///< Decoder가 Disable되어 있다.
	ERR_TTX_DELETE,
	ERR_TTX_WRONG_FRAMING_CODE,		///< Wrong framing code
	ERR_TTX_NOT_IMPLEMENTED_YET,
	ERR_TTX_LAST
} TTX_Result_t;


/**
 * 디코더에 페이지를 요청할 때 현재 페이지를 기준으로 검색하는 방법
 */
typedef enum
{
	eTTX_SEARCH_NULL,

	eTTX_SEARCH_NORMAL		= 0,	///< PageNum만 고려하여 검색
	eTTX_SEARCH_NEXT_PAGE,			///< 현재 페이지를 기준으로 다음 페이지 검색
	eTTX_SEARCH_PREV_PAGE,			///< 현재 페이지를 기준으로 이전 페이지 검색
	eTTX_SEARCH_NEXT_SUBPAGE,		///< 현재 페이지를 기준으로 다음 서브 페이지 검색
	eTTX_SEARCH_PREV_SUBPAGE,		///< 현재 페이지를 기준으로 이전 서브 페이지 검색
	eTTX_SEARCH_FIRST_SUBPAGE,
	eTTX_SEARCH_LAST_SUBPAGE,

	eTTX_SEARCH_LAST
} TTX_Search_t;


typedef struct
{
	HBOOL	bEnabled;
	HUINT16	usPageNum;
	HUINT16	usSubcode;
} TTX_Link_t;


/**
 * Full Level One Facilities
 *
 */
typedef struct
{
	HBOOL		bEnabled;		///< Is FLOF Enabled??
	HBOOL		bLinkControl;	///< Row 24를 그릴지 여부를 나타낸다.
	TTX_Link_t	aColorKey[4];	///< prompt mode
	TTX_Link_t	indexKey;		///< This link associated with INDEX key.
	TTX_Link_t	addtion;		///< maybe used for additional automatic page acquisition.
} TTX_FLOF_t;


/**
 * 화면을 가로 40 X 세로 25의 격자로 나누었을 때,
 * 각 격자 하나하나에 해당 하는 부분을 Cell이라 명명하고,
 * 하나의 Cell에 대한 정보를 아래 구조체에서 나타낸다.
 *
 * @struct	TTX_Cell_t
 */
typedef struct
{
	HUINT8	ucCode;				///< Teletext 문자 코드
	HUINT8	ucNationalOption;	///< Teletext 문자 코드 (National Option character subsets)
	HUINT8	ucChar;				///< Character

	HUINT8	ucFgColor;			///< Foreground color
	HUINT8	ucBgColor;			///< Background color

	HUINT8	bDoubleWidth;		///< Cell의 Width를 오른쪽 칸 까지 늘린다.
	HUINT8	bDoubleHeight;		///< Cell의 Height를 아래 칸 까지 늘린다.

	union {
		HUINT8	ucDiacriticalMark;	///< 구분 기호
		HUINT8	bSeparatedMosaic;	///< Separated Mosaics
		HUINT8	ucReserved;			///< reserved for future use??
	}Opt;						///< Optional member

	HUINT8	bConceal;			///< Conceal
	HUINT8	bFlash;				///< Flash
	HUINT8	bCompositeChar;		///< Composite Characer
	HUINT8	bDisplayable;		///< Cell이 Displayable한지 여부
} TTX_Cell_t;


/**
 * 텔레텍스트 페이지 객체
 */
typedef struct
{
	HBOOL		bEnabled;			///< Enabled
	HUINT16		usPageNum;			///< Page number
	HUINT16		usSubcode;			///< Subcode

	HUINT8		bSubtitle;			///< Subtitle page
	HUINT8		bNewsflash;			///< Newsflash page

	TTX_FLOF_t	FLOF;				///< FLOF Page Navigation
	struct {
		HBOOL			bEnabled;
		TTX_TOP_Link_t	aLink[4];
		HUINT32			ulVersion;
	} TOP;							///< TOP Page Navigation
	struct {
		HBOOL			bEnabled;	// TODO: 필요 없는 부분으로, 삭제 하자!!
		HUINT16			ausTrace[4];
		HUINT32			ulVersion;
	} TRACE;						///< TRACE Page Navigation
	struct {
		HUINT32			ulCount;
		HUINT8			aucList[TTX_MAX_SUBCODE_LIST];	///< 0x00~0x79까지만 허용
		HUINT32			ulVersion;
	} SubcodeBar;					///< Subcode 리스트로 Row25에 그려지며, 총 12개를 표시할 수 있음

	HUINT8		ucFgCLUT;			///< Foreground CLUT
	HUINT8		ucBgCLUT;			///< Background CLUT
	HUINT8		ucDefaultCode;		///< Default Character Code
	HUINT8		ucDefaultNOS;		///< Default national option
	HUINT8		ucScreenColor;		///< Screen Color
	HUINT8		ucRowColor;			///< Row Color
	HUINT8		bInhibitDisplay;	///< Row 1부터 24 까지 그리지 않는다.
	HUINT8		bSuppressHeader;	///< 헤더를 그리지 않는다.
	HUINT8		bLinkControl;		///< Row 24를 그릴지 여부를 나타낸다.

	/**
	 * 화면을 각 셀로 나누었을 때의 정보
	 */
	TTX_Cell_t		aCellMap[TTX_MAX_ROW][TTX_MAX_COLUMN];
} TTX_Page_t;


/**
 * 페이지 리시버 콜백
 *
 * @param	pPage		필터링된 페이지(리시버 함수가 리턴되면 소멸되므로 복사를 하던지...)
 * @param	bUpdated	새로 Update된 페이지
 */
typedef void (*TTX_PageReceiver_t)	(TTX_Page_t *pPage);


/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/


//1 Teletext Decoder APIs


/**
 * 디코더를 생성하여 핸들을 리턴한다.
 *
 * @param 	pvMemory	디코더에서 사용할 메모리 영역
 * @param 	ulLength	메모리 영역의 크기(in bytes)
 * @return 	Handle of Decoder
 */
Handle_t		TTX_DECODER_Create (
								void *			pvMemory,
								const HUINT32 	ulLength
								);


/**
 * 디코더를 소멸한다.
 *
 * @param 	hDecoder	Decoder handle
 */
void			TTX_DECODER_Destroy (
								Handle_t 		hDecoder
								);

/**
 * 디코더 스위치
 *
 * @param	hwindow	FBWindow handle
 * @param	hDecoder	Decoder handle
 * @param	bOn			Switch On/Off
 * @param	ERR_TTX_OK / ERR_TTX_FAIL / ERR_TTX_INVALID_ARGUMENT
 */
TTX_Result_t	TTX_DECODER_Switch (
								ONDKWindowHandle	hwindow,
								Handle_t 		hDecoder,
								HBOOL 			bOn
								);

/**
 * Clock run-in을 제외한 나머지 43바이트의 데이터를 디먹싱하여 메모리에 저장한다.
 *
 * @param	hDecoder	Decoder handle
 * @param	pucPacket	43 Bytes(Framing code + 42 Data bytes) TTX Packet
 * @return	ERR_TTX_OK / ERR_TTX_FAIL / ERR_TTX_DISABLED
 */
TTX_Result_t	TTX_DECODER_Demux	(
								Handle_t 		hDecoder,
								HUINT8 *		pucPacket
								);

/**
 *
 */
TTX_Result_t	TTX_DECODER_SetFilter	(
								Handle_t		hDecoder,
								const HUINT16	usPageNum,
								const HUINT16	usSubcode,
								const HBOOL		bRecvCached,
								TTX_PageReceiver_t pfnReceiver
								);

HUINT32			TTX_DECODER_GetPage (
								Handle_t 		hDecoder,
								HUINT16 		pageno,
								HUINT16 		subcode,
								HUINT8 *		buf,
								HUINT32 		size
								);

/**
 *
 */
TTX_Result_t	TTX_DECODER_Search (
								Handle_t		hDecoder,
								const HUINT16	usPageNum,
								const HUINT16	usSubcode,
								const TTX_Search_t search,
								HUINT16		*	pusResPageNum,
								HUINT16		*	pusResSubcode
								);


/**
 * 페이지를 화면에 렌더링 한다.
 * ttx_decoder모듈은 플랫폼에 독립적이다. 그렇기 때문에 내부적으로 그래픽스 렌더링 함수를 호출하며,
 * 그래픽스 랜더링 함수는 각 플랫폼 종속적으로 구현되어 있어야 한다.
 *
 * @param 	hDecoder		Decoder Handle
 * @param 	pPage			화면에 그려질 페이지 객체
 * @param	bRollingHeader	서치 중에 사용한다. 헤더가 계속 변함
 * @param 	...				TTX_GFX_RenderingPage (in ttx_graphics.h)에 전달될 가변인자
 * @return 	ERR_TTX_OK / ERR_TTX_FAIL / ERR_TTX_INVALID_ARGUMENT / ERR_TTX_DISABLED
 */
TTX_Result_t	TTX_DECODER_Rendering (
								Handle_t 		hDecoder,
								TTX_Page_t * 	pPage,
								HBOOL			bRollingHeader,
								...
								);


/**
 * 현재 들어오고 있는 페이지가 제한시간 100ms이 지났는데도, 다음 헤더를 기다리고 있는경우,
 * Flushing을 하여 Update를 해야한다.
 * 이 함수는 적어도 100ms에 한번씩 호출되어야 한다.
 * ==> @see EN300706v010201 B.9 Maximum time between successive packets of a page
 *
 * @param	hDecoder	Decoder Handle
 * @return	ERR_TTX_OK / ERR_TTX_FAIL / ERR_TTX_INVALID_ARGUMENT / ERR_TTX_DISABLED
 */
TTX_Result_t	TTX_DECODER_Flush (
								Handle_t		hDecoder
								);

TTX_Result_t	TTX_DECODER_InitPage (
								Handle_t		hDecoder,
								HUINT16		*	pusPageNum,
								HUINT16		*	pusSubcode
								);

TTX_Result_t	TTX_DECODER_SetLanguage (
								Handle_t 		hDecoder,
								const HUINT8	ucNosIndex,
								const HUINT8	ucG0,
								const HUINT8	ucG2,
								const HUINT8	ucNOS
								);

#if defined(CONFIG_DEBUG)
/**
 * 스트림을 켑쳐한다.
 *
 * @param	hDecoder	Decoder handle
 * @param	szFileName	Capture한 스트림의 파일명
 */
TTX_Result_t	TTX_DECODER_StreamCapture (
								Handle_t 		hDecoder,
								const HINT8 *	szFileName
								);

#endif

#endif /* !__TTX_DECODER_H__ */


