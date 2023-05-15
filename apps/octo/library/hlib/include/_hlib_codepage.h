/*************************************************************
 * @file		codepage.h
 * @date		2011/01/17
 * @author	hwang ji yeon(jyhwang@humaxdigital.com)
 * @brief		HUMAX common library header, related with codepage
 *
 * http://www.humaxdigital.com
 *************************************************************/
#ifndef	_HLIB_CP_H_
#define	_HLIB_CP_H_

/* *************************************************************************************************** */
#define ___HEADER________________________________________________________________________________________
/* *************************************************************************************************** */


/* *************************************************************************************************** */
#define ___Global_Variables______________________________________________________________________________
/* *************************************************************************************************** */



/* *************************************************************************************************** */
#define ___Macro_Definition______________________________________________________________________________
/* *************************************************************************************************** */

#define	CP_ENDIAN_BIG		0
#define	CP_ENDIAN_LITTLE	1


#define CP_MEM_Alloc				HLIB_MEM_Malloc
#define CP_MEM_Free					HLIB_MEM_Free
#define CP_Memset					memset
#define CP_Memcpy					memcpy



#define CP_Print(level, message)	Hx_Print message

typedef enum
{
	/* OutofNum 보다 작아지면 fail */
	eHxCP_ERROR_FAIL 	= -1,
	eHxCP_ERROR_OUT_OF_NUM	= 0,
	/* 0보다 큰 값은 True */

	eHxCP_ERROR_MAX
} HxCP_Error_e;


#define 	CHAR_EMPHASIS_ON		0x86
#define 	CHAR_EMPHASIS_OFF		0x87
#define 	CHAR_CR_LF				0x8A

#define 	UCS2_EMPHASIS_ON		0xE086
#define 	UCS2_EMPHASIS_OFF		0xE087
#define 	UCS2_CR_LF				0xE08A

#define 	UTF8_EMPHASIS_ON		0xEE8286
#define 	UTF8_EMPHASIS_OFF		0xEE8287
#define 	UTF8_CR_LF				0xEE828A


#define		EN300468_LOCALE_CODE_1BYTE_LEN			1
#define		EN300468_LOCALE_CODE_3BYTE_LEN			3

#define		EN300468_8859_3BTYE_FIRST_VALUE			0x10
#define		EN300468_8859_3BTYE_2ND_VALUE			0x00

#define		EN300468_8859_1_3BYTE_3TH_VALUE			0x01
#define		EN300468_8859_2_3BYTE_3TH_VALUE			0x02
#define		EN300468_8859_3_3BYTE_3TH_VALUE			0x03
#define		EN300468_8859_4_3BYTE_3TH_VALUE			0x04
#define		EN300468_8859_5_3BYTE_3TH_VALUE			0x05
#define		EN300468_8859_6_3BYTE_3TH_VALUE			0x06
#define		EN300468_8859_7_3BYTE_3TH_VALUE			0x07
#define		EN300468_8859_8_3BYTE_3TH_VALUE			0x08
#define		EN300468_8859_9_3BYTE_3TH_VALUE			0x09
#define		EN300468_8859_10_3BYTE_3TH_VALUE		0x0A
#define		EN300468_8859_11_3BYTE_3TH_VALUE		0x0B
#define		EN300468_8859_13_3BYTE_3TH_VALUE		0x0D
#define		EN300468_8859_14_3BYTE_3TH_VALUE		0x0E
#define		EN300468_8859_15_3BYTE_3TH_VALUE		0x0F

/* Humax에서 정의한 6937 Value임 - EN300468에서 정의한 값은 아니지만, 동일한 Naming 을 줌. */
#define		EN300468_6937_3BTYE_FIRST_VALUE			0x10
#define		EN300468_6937_3BTYE_2ND_VALUE			0x69
#define		EN300468_6937_3BYTE_3TH_VALUE			0x37

#define JIS_LOCALE_CODE_1BYTE 0x10
#define JIS_LOCALE_CODE_2BYTE 0x4A
#define JIS_LOCALE_CODE_3BYTE 0x50

#define SJIS_LOCALE_CODE_1BYTE 0x10
#define SJIS_LOCALE_CODE_2BYTE 0x4A
#define SJIS_LOCALE_CODE_3BYTE 0x53

#define EUCJP_LOCALE_CODE_1BYTE 0x10
#define EUCJP_LOCALE_CODE_2BYTE 0x4A
#define EUCJP_LOCALE_CODE_3BYTE 0x45

typedef enum
{
/** < ETSI EN 300 468 Annes A Table A.3 Character coding Tables */
	eHxCP_DVBLANG_VALUE_8859_05	= 0x01,
	eHxCP_DVBLANG_VALUE_8859_06	= 0x02,
	eHxCP_DVBLANG_VALUE_8859_07	= 0x03,
	eHxCP_DVBLANG_VALUE_8859_08	= 0x04,
	eHxCP_DVBLANG_VALUE_8859_09	= 0x05,
	eHxCP_DVBLANG_VALUE_8859_10	= 0x06,
	eHxCP_DVBLANG_VALUE_8859_11	= 0x07,
	eHxCP_DVBLANG_VALUE_8859_12	= 0x08,
	eHxCP_DVBLANG_VALUE_8859_13	= 0x09,
	eHxCP_DVBLANG_VALUE_8859_14	= 0x0A,
	eHxCP_DVBLANG_VALUE_8859_15	= 0x0B,
	eHxCP_DVBLANG_VALUE_8859_16	= 0x0C,

// ext DVBLANG
	eHxCP_DVBLANG_VALUE_8859		= 0x10,
	eHxCP_DVBLANG_VALUE_UCS2		= 0x11,

	eHxCP_DVBLANG_VALUE_KSX1001		= 0x12,
	eHxCP_DVBLANG_VALUE_GB_2312		= 0x13,
	eHxCP_DVBLANG_VALUE_BIG5		= 0x14,

	eHxCP_DVBLANG_VALUE_UTF_8		= 0x15,


	eHxCP_DVBLANG_VALUE_MAX
} HxCP_Tag_e;


typedef enum
{
	eHxCP_TELETEXT_G0_LATIN_ENG,		///< Latin G0 Primary Set - English
	eHxCP_TELETEXT_G0_LATIN_GER,		///< Latin G0 Primary Set - Germen
	eHxCP_TELETEXT_G0_LATIN_SWE_FIN,	///< Latin G0 Primary Set - Swedish/Finnish
	eHxCP_TELETEXT_G0_LATIN_ITA,		///< Latin G0 Primary Set - Italian
	eHxCP_TELETEXT_G0_LATIN_FRE,		///< Latin G0 Primary Set - French
	eHxCP_TELETEXT_G0_LATIN_POR_SPA,	///< Latin G0 Primary Set - Portuguese/Spanish
	eHxCP_TELETEXT_G0_LATIN_CZE_SLO,	///< Latin G0 Primary Set - Czech/Slovak
	eHxCP_TELETEXT_G0_LATIN_POL,		///< Latin G0 Primary Set - Polish
	eHxCP_TELETEXT_G0_LATIN_EST,		///< Latin G0 Primary Set - Estonian
	eHxCP_TELETEXT_G0_LATIN_LET_LIT,	///< Latin G0 Primary Set - Lettish/Lithuanian
	eHxCP_TELETEXT_G0_LATIN_RUM,		///< Latin G0 Primary Set - Rumanian
	eHxCP_TELETEXT_G0_LATIN_SCC_SCR,	///< Latin G0 Primary Set - Serbian/Croatian/Slovenian
	eHxCP_TELETEXT_G0_LATIN_TUR,		///< Latin G0 Primary Set - Turkish
	eHxCP_TELETEXT_G0_LATIN,			///< Latin G0 Primary Set
	eHxCP_TELETEXT_G0_CYRILLIC_1,		///< Cyrillic G0 Primary Set - Option 1 - Serbian/Croatian
	eHxCP_TELETEXT_G0_CYRILLIC_2,		///< Cyrillic G0 Primary Set - Option 2 - Russian/Bulgarian
	eHxCP_TELETEXT_G0_CYRILLIC_3,		///< Cyrillic G0 Primary Set - Option 3 - Ukrainian
	eHxCP_TELETEXT_G0_GREEK, 			///< Greek G0 Priamry Set
	eHxCP_TELETEXT_G0_ARBIC, 			///< Arbic G0 Primary Set
	eHxCP_TELETEXT_G0_HEBREW,			///< Hebrew G0 Primary Set
	eHxCP_TELETEXT_G1_MOSAIC_C,		///< G1 Block Mosaics Set - Contiguous
	eHxCP_TELETEXT_G1_MOSAIC_S,		///< G1 Block Mosaics Set - Separated
	eHxCP_TELETEXT_G2_LATIN, 			///< Latin G2 Supplementary Set
	eHxCP_TELETEXT_G2_CYRILLIC,		///< Cyrillic G2 Supplementary Set
	eHxCP_TELETEXT_G2_GREEK, 			///< Greek G2 Supplementary Set
	eHxCP_TELETEXT_G2_ARBIC, 			///< Arbic G2 Supplementary Set
	eHxCP_TELETEXT_G3_MOSAIC,			///< G3 line Drawing & Smooth Mosaic

	eHxCP_TELETEXT_MAX
} HxCP_TeletextTag_e;




typedef enum tagHCodepage_e{
/* single byte container */

/*************************************************************************/
/*	ISO CodePage														*/
/*************************************************************************/
	eHxCP_ISO_DEFAULT = 0,	// UTF8
	eHxCP_ISO_ASCII,
	eHxCP_ISO_6937,
	eHxCP_ISO_8859,
	eHxCP_ISO_8859_1,
	eHxCP_ISO_8859_2,
	eHxCP_ISO_8859_3,
	eHxCP_ISO_8859_4,
	eHxCP_ISO_8859_5,
	eHxCP_ISO_8859_6,
	eHxCP_ISO_8859_7,
	eHxCP_ISO_8859_8,
	eHxCP_ISO_8859_9,
	eHxCP_ISO_8859_10,
	eHxCP_ISO_8859_11,
	eHxCP_ISO_8859_12,
	eHxCP_ISO_8859_13,
	eHxCP_ISO_8859_14,
	eHxCP_ISO_8859_15,
	eHxCP_ISO_8859_16,
	eHxCP_ISO_300468,


	eHxCP_ISO_JAPAN_EUC,
	eHxCP_ISO_JAPAN_JIS,	// ISO-2022-JP-3
	eHxCP_ISO_JAPAN_SJIS,

/*************************************************************************/
/*	UNICODE CodePage 													 */
/*************************************************************************/
	eHxCP_UNICODE_UTF7,
	eHxCP_UNICODE_UTF8,
	eHxCP_UNICODE_UTF16,
	eHxCP_UNICODE_UTF32,

	eHxCP_UNICODE_UCS2,
	eHxCP_UNICODE_UCS4,

/*************************************************************************/
/*  Window CodePage                                                      */
/*************************************************************************/
	eHxCP_WIN_1251, //Russia 향에서 CP1251 (for Cyrillic) 에 대응하기 위해 추가



/*************************************************************************/
/*  Humax CodePage                                                      */
/*************************************************************************/
	eHxCP_UNICODE_DVB_UTF8,
	eHxCP_UNICODE_DVB_EMPHASYS_UTF8,
	eHxCP_UNICODE_DVB_UCS2,

	eHxCP_TELETEXT,

/*************************************************************************/
/*  추가는 위쪽에 해 주세요.						                     */
/*************************************************************************/
	eHCodepage_MAX
} eHxCP_CodePage_e;


typedef enum	tagHxCodeTable_e
{
	eHxCP_CODE_TABLE_Auto				= 0x00000000,

	/* generic codetable */
	eHxCP_CODE_TABLE_CTMask				= 0x00FFFFFF,

	eHxCP_CODE_TABLE_GenericCodetable	= 0x00000001,
	eHxCP_CODE_TABLE_Basic_Latin,
	eHxCP_CODE_TABLE_Latin_1_Supplement,
	eHxCP_CODE_TABLE_Latin_Extended_A,
	eHxCP_CODE_TABLE_Latin_Extended_B,
	eHxCP_CODE_TABLE_IPA_Extensions,
	eHxCP_CODE_TABLE_Spacing_Modifier_Letters,
	eHxCP_CODE_TABLE_Combining_Diacritical_Marks,
	eHxCP_CODE_TABLE_Greek_and_Coptic,
	eHxCP_CODE_TABLE_Cyrillic,
	eHxCP_CODE_TABLE_Cyrillic_Supplement,
	eHxCP_CODE_TABLE_Armenian,
	eHxCP_CODE_TABLE_Hebrew,
	eHxCP_CODE_TABLE_Arabic,
	eHxCP_CODE_TABLE_Syriac,
	eHxCP_CODE_TABLE_Arabic_Supplement,
	eHxCP_CODE_TABLE_Thaana,
	eHxCP_CODE_TABLE_NKo,
	eHxCP_CODE_TABLE_Samaritan,
	eHxCP_CODE_TABLE_Devanagari,
	eHxCP_CODE_TABLE_Bengali,
	eHxCP_CODE_TABLE_Gurmukhi,
	eHxCP_CODE_TABLE_Gujarati,
	eHxCP_CODE_TABLE_Oriya,
	eHxCP_CODE_TABLE_Tamil,
	eHxCP_CODE_TABLE_Telugu,
	eHxCP_CODE_TABLE_Kannada,
	eHxCP_CODE_TABLE_Malayalam,
	eHxCP_CODE_TABLE_Sinhala,
	eHxCP_CODE_TABLE_Thai,
	eHxCP_CODE_TABLE_Lao,
	eHxCP_CODE_TABLE_Tibetan,
	eHxCP_CODE_TABLE_Myanmar,
	eHxCP_CODE_TABLE_Georgian,
	eHxCP_CODE_TABLE_Hangul_Jamo,
	eHxCP_CODE_TABLE_Ethiopic,
	eHxCP_CODE_TABLE_Ethiopic_Supplement,
	eHxCP_CODE_TABLE_Cherokee,
	eHxCP_CODE_TABLE_Unified_Canadian_Aboriginal_Syllabics,
	eHxCP_CODE_TABLE_Ogham,
	eHxCP_CODE_TABLE_Runic,
	eHxCP_CODE_TABLE_Tagalog,
	eHxCP_CODE_TABLE_Hanunoo,
	eHxCP_CODE_TABLE_Buhid,
	eHxCP_CODE_TABLE_Tagbanwa,
	eHxCP_CODE_TABLE_Khmer,
	eHxCP_CODE_TABLE_Mongolian,
	eHxCP_CODE_TABLE_Unified_Canadian_Aboriginal_Syllabics_Extended,
	eHxCP_CODE_TABLE_Limbu,
	eHxCP_CODE_TABLE_Tai_Le,
	eHxCP_CODE_TABLE_New_Tai_Lue,
	eHxCP_CODE_TABLE_Khmer_Symbols,
	eHxCP_CODE_TABLE_Buginese,
	eHxCP_CODE_TABLE_Tai_Tham,
	eHxCP_CODE_TABLE_Balinese,
	eHxCP_CODE_TABLE_Sundanese,
	eHxCP_CODE_TABLE_Lepcha,
	eHxCP_CODE_TABLE_Ol_Chiki,
	eHxCP_CODE_TABLE_Vedic_Extensions,
	eHxCP_CODE_TABLE_Phonetic_Extensions,
	eHxCP_CODE_TABLE_Phonetic_Extensions_Supplement,

	eHxCP_CODE_TABLE_Combining_Diacritical_Marks_Supplement,
	eHxCP_CODE_TABLE_Latin_Extended_Additional,
	eHxCP_CODE_TABLE_Greek_Extended,
	eHxCP_CODE_TABLE_General_Punctuation,
	eHxCP_CODE_TABLE_Superscripts_and_Subscripts,
	eHxCP_CODE_TABLE_Currency_Symbols,
	eHxCP_CODE_TABLE_Combining_Diacritical_Marks_for_Symbols,
	eHxCP_CODE_TABLE_Letterlike_Symbols,
	eHxCP_CODE_TABLE_Number_Forms,
	eHxCP_CODE_TABLE_Arrows,
	eHxCP_CODE_TABLE_Mathematical_Operators,
	eHxCP_CODE_TABLE_Miscellaneous_Technical,
	eHxCP_CODE_TABLE_Control_Pictures,
	eHxCP_CODE_TABLE_Optical_Character_Recognition,
	eHxCP_CODE_TABLE_Enclosed_Alphanumerics,
	eHxCP_CODE_TABLE_Box_Drawing,
	eHxCP_CODE_TABLE_Block_Elements,
	eHxCP_CODE_TABLE_Geometric_Shapes,
	eHxCP_CODE_TABLE_Miscellaneous_Symbols,
	eHxCP_CODE_TABLE_Dingbats,
	eHxCP_CODE_TABLE_Miscellaneous_Mathematical_Symbols_A,
	eHxCP_CODE_TABLE_Supplemental_Arrows_A,
	eHxCP_CODE_TABLE_Braille_Patterns,
	eHxCP_CODE_TABLE_Supplemental_Arrows_B,
	eHxCP_CODE_TABLE_Miscellaneous_Mathematical_Symbols_B,
	eHxCP_CODE_TABLE_Supplemental_Mathematical_Operators,
	eHxCP_CODE_TABLE_Miscellaneous_Symbols_and_Arrows,
	eHxCP_CODE_TABLE_Glagolitic,
	eHxCP_CODE_TABLE_Latin_Extended_C,
	eHxCP_CODE_TABLE_Coptic,
	eHxCP_CODE_TABLE_Georgian_Supplement,
	eHxCP_CODE_TABLE_Tifinagh,
	eHxCP_CODE_TABLE_Ethiopic_Extended,
	eHxCP_CODE_TABLE_Cyrillic_Extended_A,
	eHxCP_CODE_TABLE_Supplemental_Punctuation,
	eHxCP_CODE_TABLE_CJK_Radicals_Supplement,
	eHxCP_CODE_TABLE_Kangxi_Radicals,
	eHxCP_CODE_TABLE_Ideographic_Description_Characters,
	eHxCP_CODE_TABLE_CJK_Symbols_and_Punctuation,
	eHxCP_CODE_TABLE_Hiragana,
	eHxCP_CODE_TABLE_Katakana,
	eHxCP_CODE_TABLE_Bopomofo,
	eHxCP_CODE_TABLE_Hangul_Compatibility_Jamo,

	eHxCP_CODE_TABLE_Kanbun,
	eHxCP_CODE_TABLE_Bopomofo_Extended,
	eHxCP_CODE_TABLE_CJK_Strokes,
	eHxCP_CODE_TABLE_Katakana_Phonetic_Extensions,
	eHxCP_CODE_TABLE_Enclosed_CJK_Letters_and_Months,
	eHxCP_CODE_TABLE_CJK_Compatibility,
	eHxCP_CODE_TABLE_CJK_Unified_Ideographs_Extension_A,
	eHxCP_CODE_TABLE_Yijing_Hexagram_Symbols,
	eHxCP_CODE_TABLE_CJK_Unified_Ideographs,
	eHxCP_CODE_TABLE_Yi_Syllables,
	eHxCP_CODE_TABLE_Yi_Radicals,
	eHxCP_CODE_TABLE_Lisu,
	eHxCP_CODE_TABLE_Vai,
	eHxCP_CODE_TABLE_Cyrillic_Extended_B,
	eHxCP_CODE_TABLE_Bamum,
	eHxCP_CODE_TABLE_Modifier_Tone_Letters,
	eHxCP_CODE_TABLE_Latin_Extended_D,
	eHxCP_CODE_TABLE_Syloti_Nagri,
	eHxCP_CODE_TABLE_Common_Indic_Number_Forms,
	eHxCP_CODE_TABLE_Phags_pa,
	eHxCP_CODE_TABLE_Saurashtra,
	eHxCP_CODE_TABLE_Devanagari_Extended,
	eHxCP_CODE_TABLE_Kayah_Li,
	eHxCP_CODE_TABLE_Rejang,
	eHxCP_CODE_TABLE_Hangul_Jamo_Extended_A,
	eHxCP_CODE_TABLE_Javanese,
	eHxCP_CODE_TABLE_Cham,
	eHxCP_CODE_TABLE_Myanmar_Extended_A,
	eHxCP_CODE_TABLE_Tai_Viet,
	eHxCP_CODE_TABLE_Meetei_Mayek,
	eHxCP_CODE_TABLE_Hangul_Syllables,
	eHxCP_CODE_TABLE_Hangul_Jamo_Extended_B,
	eHxCP_CODE_TABLE_High_Surrogates,
	eHxCP_CODE_TABLE_High_Private_Use_Surrogates,
	eHxCP_CODE_TABLE_Low_Surrogates,
	eHxCP_CODE_TABLE_Private_Use_Area,
	eHxCP_CODE_TABLE_CJK_Compatibility_Ideographs,
	eHxCP_CODE_TABLE_Alphabetic_Presentation_Forms,
	eHxCP_CODE_TABLE_Arabic_Presentation_Forms_A,
	eHxCP_CODE_TABLE_Variation_Selectors,
	eHxCP_CODE_TABLE_Vertical_Forms,
	eHxCP_CODE_TABLE_Combining_Half_Marks,
	eHxCP_CODE_TABLE_CJK_Compatibility_Forms,
	eHxCP_CODE_TABLE_Small_Form_Variants,
	eHxCP_CODE_TABLE_Arabic_Presentation_Forms_B,
	eHxCP_CODE_TABLE_Halfwidth_and_Fullwidth_Forms,
	eHxCP_CODE_TABLE_Specials,



#if 0 /* not support */
	eHxCP_CODE_TABLE_Linear_B_Syllabary,
	eHxCP_CODE_TABLE_Linear_B_Ideograms,
	eHxCP_CODE_TABLE_Aegean_Numbers,
	eHxCP_CODE_TABLE_Ancient_Greek_Numbers,
	eHxCP_CODE_TABLE_Ancient_Symbols,
	eHxCP_CODE_TABLE_Phaistos_Disc,
	eHxCP_CODE_TABLE_Lycian,
	eHxCP_CODE_TABLE_Carian,
	eHxCP_CODE_TABLE_Old_Italic,
	eHxCP_CODE_TABLE_Gothic,
	eHxCP_CODE_TABLE_Ugaritic,
	eHxCP_CODE_TABLE_Old_Persian,
	eHxCP_CODE_TABLE_Deseret,
	eHxCP_CODE_TABLE_Shavian,
	eHxCP_CODE_TABLE_Osmanya,
	eHxCP_CODE_TABLE_Cypriot_Syllabary,
	eHxCP_CODE_TABLE_Imperial_Aramaic,
	eHxCP_CODE_TABLE_Phoenician,
	eHxCP_CODE_TABLE_Lydian,
	eHxCP_CODE_TABLE_Kharoshthi,
	eHxCP_CODE_TABLE_Old_South_Arabian,
	eHxCP_CODE_TABLE_Avestan,
	eHxCP_CODE_TABLE_Inscriptional_Parthian,
	eHxCP_CODE_TABLE_Inscriptional_Pahlavi,
	eHxCP_CODE_TABLE_Old_Turkic,
	eHxCP_CODE_TABLE_Rumi_Numeral_Symbols,
	eHxCP_CODE_TABLE_Kaithi,
	eHxCP_CODE_TABLE_Cuneiform,
	eHxCP_CODE_TABLE_Cuneiform_Numbers_and_Punctuation,
	eHxCP_CODE_TABLE_Egyptian_Hieroglyphs,
	eHxCP_CODE_TABLE_Byzantine_Musical_Symbols,
	eHxCP_CODE_TABLE_Musical_Symbols,
	eHxCP_CODE_TABLE_Ancient_Greek_Musical_Notation,
	eHxCP_CODE_TABLE_Tai_Xuan_Jing_Symbols,
	eHxCP_CODE_TABLE_Counting_Rod_Numerals,
	eHxCP_CODE_TABLE_Mathematical_Alphanumeric_Symbols,
	eHxCP_CODE_TABLE_Mahjong_Tiles,
	eHxCP_CODE_TABLE_Domino_Tiles,

	eHxCP_CODE_TABLE_Enclosed_Alphanumeric_Supplement,
	eHxCP_CODE_TABLE_Enclosed_Ideographic_Supplement,
	eHxCP_CODE_TABLE_CJK_Unified_Ideographs_Extension_B,
	eHxCP_CODE_TABLE_CJK_Unified_Ideographs_Extension_C,
	eHxCP_CODE_TABLE_CJK_Compatibility_Ideographs_Supplement,
	eHxCP_CODE_TABLE_Tags,
	eHxCP_CODE_TABLE_Variation_Selectors_Supplement,
	eHxCP_CODE_TABLE_Supplementary_Private_Use_Area_A,
	eHxCP_CODE_TABLE_Supplementary_Private_Use_Area_B,
#endif
	eHxCP_CODE_TABLE_Max
} eHxCP_CodeTable_e;



typedef HINT32 (*CPNumOfBytes)(const void *pSrc);

typedef HINT32 (*CPNumOfChars)(const void *pSrc);
typedef HINT32 (*CPGetCharAt)(const void *pSrc, const HINT32 nCharNum,  void *pCharBuf);


typedef HINT32 (*ConvertCodepageToUCS2)(const void 	*pSrcStr, HUINT16 	*pDestUCS2Str, HINT32 nDestStrNumOfBytes);
typedef HINT32 (*ConvertUCS2ToCodepage)(const HUINT16 	*pSrcUCS2Str, void 	*pDestStr, HINT32 nDestStrNumOfBytes);

//for japan
typedef HINT32 (*ConvertCodepageToSJIS)(HUINT8 *pSrcStr, HUINT8 *pDstStr, HINT32 nSrcMaxLen, HINT32 nDstMaxLen, HUINT32 *pulUseStrLen);
typedef struct tagHCodepage_Japan
{
	ConvertCodepageToSJIS	cbCodepageToSJIS;
} HCodepage_Japan_t;


typedef struct tagCPTagSlot
{
	eHxCP_CodePage_e eCodepage;
	HUINT8		szCPTag[4];
	HINT32		nCPTagLen;

#if defined(TEST_CP)
	HUINT8		szCPTag2[4];
	HUINT32		nCPTagLen2;
#endif	
} CPTagSlot_t;

typedef struct tagHCodePageSlot
{
	CPNumOfChars			cbNumOfChars;
	CPNumOfBytes			cbNumOfBytes;
	CPGetCharAt				cbGetCharAt;

	ConvertCodepageToUCS2	cbCodeapgeToUCS2;
	ConvertUCS2ToCodepage	cbUCS2ToCodepage;

	CPTagSlot_t				stCPTagSlot;

	HUINT8					ucEndCharBytes;
	const HUINT16 			*pusUCS2Table;

	union {
		HCodepage_Japan_t		stJapanCp;
		void					*pExt;
	} uExt;
} eHxCP_Slot_t;






/* *************************************************************************************************** */
#define ___MACRO_FUCTION_Definition___________________________________________________________________________
/* *************************************************************************************************** */
#define		HxCP_IsValidReturnValue(x)		((x <= eHxCP_ERROR_OUT_OF_NUM) ? FALSE : TRUE)
#define		HxCP_IsErrorCheck(x)			((x < eHxCP_ERROR_OUT_OF_NUM) ? FALSE : TRUE)



/* *************************************************************************************************** */
#define ___Function_Definition___________________________________________________________________________
/* *************************************************************************************************** */

HERROR	HLIB_CP_Init(void);
HERROR	HLIB_CP_Destroy(void);

HERROR	HLIB_CP_GetSlot(eHxCP_CodePage_e eCodepage, eHxCP_Slot_t *pstCPSlot);
HERROR	HLIB_CP_AddCodepage(eHxCP_Slot_t *pstCPSlot);
HERROR	HLIB_CP_CPTagByCodepage(const HUINT8 *pcStr, eHxCP_CodePage_e *peCodepage, HINT32 *pnCPTagLen);
HERROR	HLIB_CP_CodepageByCPTag(eHxCP_CodePage_e eCodepage, HUINT8 *pszCPTagStr, HINT32 *pnCPTagLen);

HINT32	HLIB_CP_NumOfBytes(eHxCP_CodePage_e eCodepage, const void *pSrc);
HINT32	HLIB_CP_NumOfChars(eHxCP_CodePage_e eCodepage, const void *pSrc);
HINT32	HLIB_CP_CharAt(eHxCP_CodePage_e eCodepage, const void *pSrc, const HINT32 nCharNum,  void *pCharBuf);
HINT32	HLIB_CP_ConvertUCS2ToCodepage(eHxCP_CodePage_e eCodepage, const HUINT16 	*pSrcUCS2Str, void 	*pDestStr, HINT32 nDestStrNumOfBytes);
HINT32	HLIB_CP_ConvertCodepageToUCS2(eHxCP_CodePage_e eCodepage, const void 	*pSrcStr, HUINT16 	*pDestUCS2Str, HINT32 nDestStrNumOfBytes);

eHxCP_CodeTable_e	HLIB_CP_UCS2CodeTable(HUINT16 usUCS2);
eHxCP_CodeTable_e 	HLIB_CP_StringUCS2CodeTable(HUINT16 *pszUCS2, HUINT32	 nNumOfChars);


const HUINT16	*HLIB_CP_CodepageToUCS2Table(eHxCP_CodePage_e eCodepage);


HINT32 HLIB_CP_UTIL_ConvertCodepageToShiftJIS(eHxCP_CodePage_e eCodepage, HUINT8 *pSrcStr, HUINT8 *pDstStr, HINT32 nSrcMaxLen, HINT32 nDstMaxLen, HUINT32 *pulUseStrLen );
HINT32 HLIB_CP_UTIL_ConvertCodepageToUTF8(eHxCP_CodePage_e eCodepage, const void 	*pSrcStr, HUINT8 	*pDestUTF8Str, HINT32 nDestStrNumOfBytes);
HINT32 HLIB_CP_UTIL_StrUTF8CopyWithoutControlCode(HUINT8 *pszUTF8DstStr, HUINT8 *pszUTF8SrcStr, HINT32 nDstBufSize);


#endif /* _HLIB_CP_H_ */
