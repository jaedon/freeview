
/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  layer_module.h File Name�� �����ϴ�.
	@brief	  file_name & simple comment.

	Description: File�� ���� ������ �����ϴ�.		\n
	Module: Layer�� ��� ���� �����ϴ�.			 	\n
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
 * Astra�� Arteä�ο��� 100, 101, 102�������� ������ �ʴ� ����
 *
 * ����: ���������� 28�� ��Ŷ�� Page Function�� POP, �׸��� Page Coding�� 0x7��
 *		�߸��� ������ �����ְ� �־���.
 * ����: 28�� ��Ŷ�� ���� Page Function�� Coding�� ������ ���� ������, �������� �׸��� ��ƾ ����
 *		Odd parity�� ������ �ֱ� ������ �Ϲ������� �ʴٰ� �Ǵܵ�.
 *		X/28�� Page Function / Coding Parsing�� Disable�Ѵ�.
 *
 * @date	2009-??-??
 * @author	jhlee4
 */
#define	TTX_WORKAROUND_01	TTX_DISABLE

/**
 * Packet Demuxing/Decoding ���� �� ���ڵ� ������ ���� Packet loss�� �߻�
 *
 * �������� ������Ʈ(������ �ٲ�)���� �ʰ� �����۵� ���, ������ �����ϴ� �������� Merging�Ͽ�
 * Packet loss�� ���� �ҿ����� �������� ���� �����ϰ� ������ ������.
 *
 * �� Workaround�� Enable �ϸ�, Merging ���������� ���� ���� ���ҽ�!!! �� ����ϰ� �� ���̴�.
 *
 * !!! �ñ׳��� ���� ���� ��� Enable�ϸ� �������� �� �̻ڰ�(?) ���� ���̴�. !!!
 *
 * @date	2009-03-22
 * @author	jhlee4
 *
 * �쿬���Ե� �� ����������
 *     "Page Overwrite :ERAGE_PAGE or UPDATE_INDICATOR �� ���� Memory���� �������� ������ �ʰ�, ����� ���"
 * �� �� �巯 �´´�. ���� �䰡�ΰ� �ϴ� ����������, ���� �����ϴ� ���� Ȯ�� �Ͽ���, �̰��� SPEC ����̹Ƿ�,
 * �� Workaround�� ���̻� workaround�� ó������ �ʴ´�(���� �⺻ ���� ����).
 *
 * --���� ��ġ--
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
 * Parallel Transmission �� ���� �ް����� �ϳ��� �������� ���� ���,
 * Page Terminating ������ ������Ű�� ���� ��Ŷ�� ������ Pipe��  �׿�������.
 * �� Define�� Enable�ϸ� 0.3�� ���� �ϳ��� �������� ���� ���� ���, �������� ���� ���� ����,
 * �޸𸮸� ���� ���� ��Ų��.
 *
 * @date	2009-??-??
 *
 * ���� ������ ���忡 ��õǾ� �ִ�
 * "For all types of pages, the maximum time intrval betweeen the transmission
 *   of successive packets of the same page shall be 100 ms"
 * �� �������� �����Ͽ�����, ���� ATV ä�ο��� �������� �ϼ��Ǵ� �ð��� 0.3�ʺ���
 * �� �ɸ��� ��찡 �߻��� �������� �߸��� ������ �����.
 *
 * �̿� ������ ���� ���Ḧ �ð��� �������� ���� �ʰ�, �� �������� ���� ���� ���� �� �ִ� ��Ŷ��
 * ���� �Ѿ�� �����ϵ��� �����Ѵ�.
 *
 * @date	2009-03-24
 *
 * >>> Parallel Transmission���� �ϳ��� �������� ������ ���� ���������� ���� ���̳�,
 *     ���� ���� '���������� �ʰ�' �����Ͱ� ���� �� Disable�� ���,
 *     MEMPOOL(Memory) Underflow�� ���� Cache Reset(Page�� ó������ �ٽ� �޾ƾ� ��)��
 *     �Ͼ �� �ִ�.  <<<
 *
 * @author	jhlee4
 */
#define	TTX_WORKAROUND_03	TTX_ENABLE


/**
 * Packet 8/25�� ���� ��Ŷ �ս�(?)
 * [Comment]
 * - 8/25�� ���� ������ Dynamic packet allocation [teletext service improvement] (Weitzel, P. BBC Projects, London)
 * �� �Ұ��ȴ�. �׷��� ���� �� ����.... �̤�, ���ϸ�!!!!!!!!!! �����ϰ� ��������!!!!!
 * - ���� ETSI ����(�⺻ TTX Spec�� Code of practice)���� 8/25�� ���� ��� ��޵� ���� ��� ó���� �ؾ� ���� �𸣴� ����
 *
 * [����]
 * - JVC Stream(Doku Kika ��)���� �������� �ֱ������� ���پ� �� ����
 *
 * [ȿ��]
 * - �� Workaround�� Enable�ϸ� ������ ���� ���� �߰��� 8/25 ��Ŷ�� ������ ������ �ް��� ��������
 * ��Խ̵ǰ� �ִ� ��� ��Ŷ�� �ʱ�ȭ �Ͽ�, ������ �߰��� ��� ���� �����Ѵ�.
 *
 * [�߻� ������ Side Effect]
 * - ��Խ��� �ѹ��� �ǳʶٴ� ���̱� ������, ������ ������Ʈ Ÿ�̹��� �ٸ� �� ����
 * - �־��� ���, (�Ƹ� �׷����� ��������) ������ �߰��� �׻� 8/25�� ���� ������ �������� ���� �� ����.
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
 * Page Termination ����
 *
 * spec�� ��õ� "different page number"�� ��Ű�� �ʴ� �Ϻ� ������ ���,
 * same page number�� �ϳ��� magazine�� �����ؼ� ���� ��� page�� �������� �ʰ�,
 * �����ؼ� �ϳ��� �������� ó���Ͽ�������, �� ���� ���������� �������� �����Ƿ�
 * "Different Page Number" ������ �����Ѵ�.
 *
 * @date	2010-01-07
 * @author	jhlee4
 */
#define TTX_WORKAROUND_05	TTX_ENABLE

/**
 * Signal�� �����ϸ� �ð��� ���� �ʴ´�.
 *
 * if Enable {
 *    Signal�� �����ص� Tick�� �������� �ð��� ����.
 * } else {
 *    Signal�� �����ϸ� �ð��� ���� �ʴ´�.
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
 * ������ warning�� �����ϱ� ���� ��ũ��
 *
 * @param param
 */
#define	TTX_UNUSED_PARAM(param)	(void)(param)


/**
 * Page number�� �������� ����
 * @param X	 Page number
 */
#define	TTX_IS_VALID_PAGE(X)		((((X) >> 4) <= 0x9) && (((X) & 0xF) <= 0x9))

/**
 * Magazine�� �������� ����
 * @param M	 Magazine
 */
#define	TTX_IS_VALID_MAGAZINE(M)	((M) <= 0x08)

/**
 * Magazine, Page number�� �������� ����
 * @param M	 Magazine
 * @param X	 Page number
 */
#define	TTX_IS_VALID_ADDR(M,X)		(TTX_IS_VALID_MAGAZINE((M)) && TTX_IS_VALID_PAGE((X)))


/**
 * Hex�� �Ǿ� �ִ� Page Address�� ����� ���Ǹ� ���� Dec���� ��ȯ
 * @param M	 Magazine
 * @param X	 Page number
 */
#define	TTX_HEX_ADDR_TO_DEC(M,X)	(((M) % 8) * 100 + ((X) >> 4) * 10 + ((X) & 0xF))
#define  TTX_HEX_ADDR_TO_DEC_TNTSAT(M,X) (((M) % 8) * 100 + X)			/* in case of TNTSAT Canal Ready, page number 88 is encoded 0x58 instead of 0x88 */

/**
 * DEC�� �Ǿ� �ִ� Page Address�� HEX Magazine ���� ��ȯ
 */
#define	TTX_DEC_ADDR_TO_HEX_M(addr)	((HUINT8)((addr) / 100))

/**
 * DEC�� �Ǿ� �ִ� Page Address�� HEX Page Num ���� ��ȯ
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

#define TTX_PACKET_LENGTH		43		/* Framing code���� ������ byte (Digital ttx & Analog ttx ���� �κ�) */
#define	TTX_MAX_ROW				25		// 0 ~ 24
#define	TTX_MAX_COLUMN			40		// 0 ~ 39, ���� Side Panel�� �����ϸ� 56�� �ȴ�.
#define	TTX_CHAR_SPACE			0x20	// ' '
#define	TTX_TIME_FIELD_LENGTH	8		// Time ������ ���� Cell�� ũ��
#define	TTX_IGNORE_SUBCODE		0x3F7F	// Subcode�� �ִ밪���� �����ڵ带 ����

#define	TTX_MAX_CLUT			4
#define	TTX_MAX_COLOR_ENTRY		8
#define	TTX_MAX_SUBCODE_LIST	12		// ȭ�鿡 ������ �� �ִ� �ִ� Subcode ����
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
 * �ڷ��ؽ�Ʈ API ���
 *
 */
typedef enum
{
	ERR_TTX_OK		= ERR_OK,		///< OK
	ERR_TTX_FAIL,					///< Fail
	ERR_TTX_NO_PAGE,				///< Cache�� ����� �������� ����.
	ERR_TTX_NO_TELETEXT,			///< Decoder�� Teletext �����Ͱ� ����.
	ERR_TTX_NO_RECEIVER,			///< Decoder�� ���� Page Receiver�� �������� �ʴ�.
	ERR_TTX_DECODE_FAIL,			///< Decode Fail
	ERR_TTX_INVALID_ARGUMENT,		///< Invalid argument
	ERR_TTX_DISABLED,				///< Decoder�� Disable�Ǿ� �ִ�.
	ERR_TTX_DELETE,
	ERR_TTX_WRONG_FRAMING_CODE,		///< Wrong framing code
	ERR_TTX_NOT_IMPLEMENTED_YET,
	ERR_TTX_LAST
} TTX_Result_t;


/**
 * ���ڴ��� �������� ��û�� �� ���� �������� �������� �˻��ϴ� ���
 */
typedef enum
{
	eTTX_SEARCH_NULL,

	eTTX_SEARCH_NORMAL		= 0,	///< PageNum�� ����Ͽ� �˻�
	eTTX_SEARCH_NEXT_PAGE,			///< ���� �������� �������� ���� ������ �˻�
	eTTX_SEARCH_PREV_PAGE,			///< ���� �������� �������� ���� ������ �˻�
	eTTX_SEARCH_NEXT_SUBPAGE,		///< ���� �������� �������� ���� ���� ������ �˻�
	eTTX_SEARCH_PREV_SUBPAGE,		///< ���� �������� �������� ���� ���� ������ �˻�
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
	HBOOL		bLinkControl;	///< Row 24�� �׸��� ���θ� ��Ÿ����.
	TTX_Link_t	aColorKey[4];	///< prompt mode
	TTX_Link_t	indexKey;		///< This link associated with INDEX key.
	TTX_Link_t	addtion;		///< maybe used for additional automatic page acquisition.
} TTX_FLOF_t;


/**
 * ȭ���� ���� 40 X ���� 25�� ���ڷ� �������� ��,
 * �� ���� �ϳ��ϳ��� �ش� �ϴ� �κ��� Cell�̶� ����ϰ�,
 * �ϳ��� Cell�� ���� ������ �Ʒ� ����ü���� ��Ÿ����.
 *
 * @struct	TTX_Cell_t
 */
typedef struct
{
	HUINT8	ucCode;				///< Teletext ���� �ڵ�
	HUINT8	ucNationalOption;	///< Teletext ���� �ڵ� (National Option character subsets)
	HUINT8	ucChar;				///< Character

	HUINT8	ucFgColor;			///< Foreground color
	HUINT8	ucBgColor;			///< Background color

	HUINT8	bDoubleWidth;		///< Cell�� Width�� ������ ĭ ���� �ø���.
	HUINT8	bDoubleHeight;		///< Cell�� Height�� �Ʒ� ĭ ���� �ø���.

	union {
		HUINT8	ucDiacriticalMark;	///< ���� ��ȣ
		HUINT8	bSeparatedMosaic;	///< Separated Mosaics
		HUINT8	ucReserved;			///< reserved for future use??
	}Opt;						///< Optional member

	HUINT8	bConceal;			///< Conceal
	HUINT8	bFlash;				///< Flash
	HUINT8	bCompositeChar;		///< Composite Characer
	HUINT8	bDisplayable;		///< Cell�� Displayable���� ����
} TTX_Cell_t;


/**
 * �ڷ��ؽ�Ʈ ������ ��ü
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
		HBOOL			bEnabled;	// TODO: �ʿ� ���� �κ�����, ���� ����!!
		HUINT16			ausTrace[4];
		HUINT32			ulVersion;
	} TRACE;						///< TRACE Page Navigation
	struct {
		HUINT32			ulCount;
		HUINT8			aucList[TTX_MAX_SUBCODE_LIST];	///< 0x00~0x79������ ���
		HUINT32			ulVersion;
	} SubcodeBar;					///< Subcode ����Ʈ�� Row25�� �׷�����, �� 12���� ǥ���� �� ����

	HUINT8		ucFgCLUT;			///< Foreground CLUT
	HUINT8		ucBgCLUT;			///< Background CLUT
	HUINT8		ucDefaultCode;		///< Default Character Code
	HUINT8		ucDefaultNOS;		///< Default national option
	HUINT8		ucScreenColor;		///< Screen Color
	HUINT8		ucRowColor;			///< Row Color
	HUINT8		bInhibitDisplay;	///< Row 1���� 24 ���� �׸��� �ʴ´�.
	HUINT8		bSuppressHeader;	///< ����� �׸��� �ʴ´�.
	HUINT8		bLinkControl;		///< Row 24�� �׸��� ���θ� ��Ÿ����.

	/**
	 * ȭ���� �� ���� �������� ���� ����
	 */
	TTX_Cell_t		aCellMap[TTX_MAX_ROW][TTX_MAX_COLUMN];
} TTX_Page_t;


/**
 * ������ ���ù� �ݹ�
 *
 * @param	pPage		���͸��� ������(���ù� �Լ��� ���ϵǸ� �Ҹ�ǹǷ� ���縦 �ϴ���...)
 * @param	bUpdated	���� Update�� ������
 */
typedef void (*TTX_PageReceiver_t)	(TTX_Page_t *pPage);


/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/


//1 Teletext Decoder APIs


/**
 * ���ڴ��� �����Ͽ� �ڵ��� �����Ѵ�.
 *
 * @param 	pvMemory	���ڴ����� ����� �޸� ����
 * @param 	ulLength	�޸� ������ ũ��(in bytes)
 * @return 	Handle of Decoder
 */
Handle_t		TTX_DECODER_Create (
								void *			pvMemory,
								const HUINT32 	ulLength
								);


/**
 * ���ڴ��� �Ҹ��Ѵ�.
 *
 * @param 	hDecoder	Decoder handle
 */
void			TTX_DECODER_Destroy (
								Handle_t 		hDecoder
								);

/**
 * ���ڴ� ����ġ
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
 * Clock run-in�� ������ ������ 43����Ʈ�� �����͸� ��Խ��Ͽ� �޸𸮿� �����Ѵ�.
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
 * �������� ȭ�鿡 ������ �Ѵ�.
 * ttx_decoder����� �÷����� �������̴�. �׷��� ������ ���������� �׷��Ƚ� ������ �Լ��� ȣ���ϸ�,
 * �׷��Ƚ� ������ �Լ��� �� �÷��� ���������� �����Ǿ� �־�� �Ѵ�.
 *
 * @param 	hDecoder		Decoder Handle
 * @param 	pPage			ȭ�鿡 �׷��� ������ ��ü
 * @param	bRollingHeader	��ġ �߿� ����Ѵ�. ����� ��� ����
 * @param 	...				TTX_GFX_RenderingPage (in ttx_graphics.h)�� ���޵� ��������
 * @return 	ERR_TTX_OK / ERR_TTX_FAIL / ERR_TTX_INVALID_ARGUMENT / ERR_TTX_DISABLED
 */
TTX_Result_t	TTX_DECODER_Rendering (
								Handle_t 		hDecoder,
								TTX_Page_t * 	pPage,
								HBOOL			bRollingHeader,
								...
								);


/**
 * ���� ������ �ִ� �������� ���ѽð� 100ms�� �����µ���, ���� ����� ��ٸ��� �ִ°��,
 * Flushing�� �Ͽ� Update�� �ؾ��Ѵ�.
 * �� �Լ��� ��� 100ms�� �ѹ��� ȣ��Ǿ�� �Ѵ�.
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
 * ��Ʈ���� �����Ѵ�.
 *
 * @param	hDecoder	Decoder handle
 * @param	szFileName	Capture�� ��Ʈ���� ���ϸ�
 */
TTX_Result_t	TTX_DECODER_StreamCapture (
								Handle_t 		hDecoder,
								const HINT8 *	szFileName
								);

#endif

#endif /* !__TTX_DECODER_H__ */


