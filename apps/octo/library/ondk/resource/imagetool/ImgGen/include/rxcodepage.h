/*************************************************************
 * @file		codepage.h
 * @date		2011/01/17
 * @author	hwang ji yeon(jyhwang@humaxdigital.com)
 * @brief		HUMAX common library header, related with codepage
 *
 * http://www.humaxdigital.com
 *************************************************************/

#ifndef	_MH_CODEPAGE_H_
#define	_MH_CODEPAGE_H_

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


#define CP_MEM_Alloc				RxSTD_Alloc
#define CP_MEM_Free					RxSTD_Free
#define CP_Memset					RxSTD_MemSet


#define CP_Print(level, message)	Rx_Print message


typedef enum tagCPTag
{
/** < ETSI EN 300 468 Annes A Table A.3 Character coding Tables */
	RxCPTAG_8859_05	= 0x01,
	RxCPTAG_8859_06	= 0x02,
	RxCPTAG_8859_07	= 0x03,
	RxCPTAG_8859_08	= 0x04,
	RxCPTAG_8859_09	= 0x05,
	RxCPTAG_8859_10	= 0x06,
	RxCPTAG_8859_11	= 0x07,
	RxCPTAG_8859_12	= 0x08,
	RxCPTAG_8859_13	= 0x09,
	RxCPTAG_8859_14	= 0x0A,
	RxCPTAG_8859_15	= 0x0B,
	RxCPTAG_8859_16	= 0x0C,

// ext DVBLANG
	RxCPTAG_8859		= 0x10,
	RxCPTAG_UCS2		= 0x11,
	RxCPTAG_UTF_8		= 0x15,

	RxCPTAG_MAX
} RxCPTag_e;


enum tagCPTeletextTag
{
	eRxCP_TELETEXT_G0_LATIN_ENG,		///< Latin G0 Primary Set - English
	eRxCP_TELETEXT_G0_LATIN_GER,		///< Latin G0 Primary Set - Germen
	eRxCP_TELETEXT_G0_LATIN_SWE_FIN,	///< Latin G0 Primary Set - Swedish/Finnish
	eRxCP_TELETEXT_G0_LATIN_ITA,		///< Latin G0 Primary Set - Italian
	eRxCP_TELETEXT_G0_LATIN_FRE,		///< Latin G0 Primary Set - French
	eRxCP_TELETEXT_G0_LATIN_POR_SPA,	///< Latin G0 Primary Set - Portuguese/Spanish
	eRxCP_TELETEXT_G0_LATIN_CZE_SLO,	///< Latin G0 Primary Set - Czech/Slovak
	eRxCP_TELETEXT_G0_LATIN_POL,		///< Latin G0 Primary Set - Polish
	eRxCP_TELETEXT_G0_LATIN_EST,		///< Latin G0 Primary Set - Estonian
	eRxCP_TELETEXT_G0_LATIN_LET_LIT,	///< Latin G0 Primary Set - Lettish/Lithuanian
	eRxCP_TELETEXT_G0_LATIN_RUM,		///< Latin G0 Primary Set - Rumanian
	eRxCP_TELETEXT_G0_LATIN_SCC_SCR,	///< Latin G0 Primary Set - Serbian/Croatian/Slovenian
	eRxCP_TELETEXT_G0_LATIN_TUR,		///< Latin G0 Primary Set - Turkish
	eRxCP_TELETEXT_G0_LATIN,			///< Latin G0 Primary Set
	eRxCP_TELETEXT_G0_CYRILLIC_1,		///< Cyrillic G0 Primary Set - Option 1 - Serbian/Croatian
	eRxCP_TELETEXT_G0_CYRILLIC_2,		///< Cyrillic G0 Primary Set - Option 2 - Russian/Bulgarian
	eRxCP_TELETEXT_G0_CYRILLIC_3,		///< Cyrillic G0 Primary Set - Option 3 - Ukrainian
	eRxCP_TELETEXT_G0_GREEK, 			///< Greek G0 Priamry Set
	eRxCP_TELETEXT_G0_ARBIC, 			///< Arbic G0 Primary Set
	eRxCP_TELETEXT_G0_HEBREW,			///< Hebrew G0 Primary Set
	eRxCP_TELETEXT_G1_MOSAIC_C,		///< G1 Block Mosaics Set - Contiguous
	eRxCP_TELETEXT_G1_MOSAIC_S,		///< G1 Block Mosaics Set - Separated
	eRxCP_TELETEXT_G2_LATIN, 			///< Latin G2 Supplementary Set
	eRxCP_TELETEXT_G2_CYRILLIC,		///< Cyrillic G2 Supplementary Set
	eRxCP_TELETEXT_G2_GREEK, 			///< Greek G2 Supplementary Set
	eRxCP_TELETEXT_G2_ARBIC, 			///< Arbic G2 Supplementary Set
	eRxCP_TELETEXT_G3_MOSAIC,			///< G3 line Drawing & Smooth Mosaic

	eRxCP_TELETEXT_MAX
};




typedef enum tagMHCp_e{
/* single byte container */

/*************************************************************************/
/*	ISO CodePage														*/
/*************************************************************************/
	eRxCP_ISO_DEFAULT = 0,	// UTF8
	eRxCP_ISO_ASCII,
	eRxCP_ISO_6937,
	eRxCP_ISO_8859,
	eRxCP_ISO_8859_1,
	eRxCP_ISO_8859_2,
	eRxCP_ISO_8859_3,
	eRxCP_ISO_8859_4,
	eRxCP_ISO_8859_5,
	eRxCP_ISO_8859_6,
	eRxCP_ISO_8859_7,
	eRxCP_ISO_8859_8,
	eRxCP_ISO_8859_9,
	eRxCP_ISO_8859_10,
	eRxCP_ISO_8859_11,
	eRxCP_ISO_8859_12,
	eRxCP_ISO_8859_13,
	eRxCP_ISO_8859_14,
	eRxCP_ISO_8859_15,
	eRxCP_ISO_8859_16,
	eRxCP_ISO_300468,


	eRxCP_ISO_JAPAN_EUC,
	eRxCP_ISO_JAPAN_JIS,	// ISO-2022-JP-3
	eRxCP_ISO_JAPAN_SJIS,

/*************************************************************************/
/*	UNICODE CodePage 													 */
/*************************************************************************/
	eRxCP_UNICODE_UTF7,
	eRxCP_UNICODE_UTF8,
	eRxCP_UNICODE_UTF16,
	eRxCP_UNICODE_UTF32,

	eRxCP_UNICODE_UCS2,
	eRxCP_UNICODE_UCS4,

/*************************************************************************/
/*  Window CodePage                                                      */
/*************************************************************************/
	eRxCP_WIN_1251, //Russia 향에서 CP1251 (for Cyrillic) 에 대응하기 위해 추가



/*************************************************************************/
/*  Humax CodePage                                                      */
/*************************************************************************/
	eRxCP_UNICODE_DVB_UTF8,
	eRxCP_UNICODE_DVB_UCS2,

	eRxCP_TELETEXT,

/*************************************************************************/
/*  추가는 위쪽에 해 주세요.						                     */
/*************************************************************************/
	eMHCp_MAX
} MHCp_e;


typedef enum	tagRxCT_e
{
	eRxCT_Auto				= 0x00000000,

	/* str 종류 */
	eRxCT_StrNormal			= 0x00000000,
	eRxCT_StrML				= 0x10000000,
	eRxCT_StrMask			= 0xF0000000,

	/* generic codetable */
	eRxCT_CTMask				= 0x00FFFFFF,

	eRxCT_GenericCodetable	= 0x00000001,
	eRxCT_Basic_Latin,
	eRxCT_Latin_1_Supplement,
	eRxCT_Latin_Extended_A,
	eRxCT_Latin_Extended_B,
	eRxCT_IPA_Extensions,
	eRxCT_Spacing_Modifier_Letters,
	eRxCT_Combining_Diacritical_Marks,
	eRxCT_Greek_and_Coptic,
	eRxCT_Cyrillic,
	eRxCT_Cyrillic_Supplement,
	eRxCT_Armenian,
	eRxCT_Hebrew,
	eRxCT_Arabic,
	eRxCT_Syriac,
	eRxCT_Arabic_Supplement,
	eRxCT_Thaana,
	eRxCT_NKo,
	eRxCT_Samaritan,
	eRxCT_Devanagari,
	eRxCT_Bengali,
	eRxCT_Gurmukhi,
	eRxCT_Gujarati,
	eRxCT_Oriya,
	eRxCT_Tamil,
	eRxCT_Telugu,
	eRxCT_Kannada,
	eRxCT_Malayalam,
	eRxCT_Sinhala,
	eRxCT_Thai,
	eRxCT_Lao,
	eRxCT_Tibetan,
	eRxCT_Myanmar,
	eRxCT_Georgian,
	eRxCT_Hangul_Jamo,
	eRxCT_Ethiopic,
	eRxCT_Ethiopic_Supplement,
	eRxCT_Cherokee,
	eRxCT_Unified_Canadian_Aboriginal_Syllabics,
	eRxCT_Ogham,
	eRxCT_Runic,
	eRxCT_Tagalog,
	eRxCT_Hanunoo,
	eRxCT_Buhid,
	eRxCT_Tagbanwa,
	eRxCT_Khmer,
	eRxCT_Mongolian,
	eRxCT_Unified_Canadian_Aboriginal_Syllabics_Extended,
	eRxCT_Limbu,
	eRxCT_Tai_Le,
	eRxCT_New_Tai_Lue,
	eRxCT_Khmer_Symbols,
	eRxCT_Buginese,
	eRxCT_Tai_Tham,
	eRxCT_Balinese,
	eRxCT_Sundanese,
	eRxCT_Lepcha,
	eRxCT_Ol_Chiki,
	eRxCT_Vedic_Extensions,
	eRxCT_Phonetic_Extensions,
	eRxCT_Phonetic_Extensions_Supplement,

	eRxCT_Combining_Diacritical_Marks_Supplement,
	eRxCT_Latin_Extended_Additional,
	eRxCT_Greek_Extended,
	eRxCT_General_Punctuation,
	eRxCT_Superscripts_and_Subscripts,
	eRxCT_Currency_Symbols,
	eRxCT_Combining_Diacritical_Marks_for_Symbols,
	eRxCT_Letterlike_Symbols,
	eRxCT_Number_Forms,
	eRxCT_Arrows,
	eRxCT_Mathematical_Operators,
	eRxCT_Miscellaneous_Technical,
	eRxCT_Control_Pictures,
	eRxCT_Optical_Character_Recognition,
	eRxCT_Enclosed_Alphanumerics,
	eRxCT_Box_Drawing,
	eRxCT_Block_Elements,
	eRxCT_Geometric_Shapes,
	eRxCT_Miscellaneous_Symbols,
	eRxCT_Dingbats,
	eRxCT_Miscellaneous_Mathematical_Symbols_A,
	eRxCT_Supplemental_Arrows_A,
	eRxCT_Braille_Patterns,
	eRxCT_Supplemental_Arrows_B,
	eRxCT_Miscellaneous_Mathematical_Symbols_B,
	eRxCT_Supplemental_Mathematical_Operators,
	eRxCT_Miscellaneous_Symbols_and_Arrows,
	eRxCT_Glagolitic,
	eRxCT_Latin_Extended_C,
	eRxCT_Coptic,
	eRxCT_Georgian_Supplement,
	eRxCT_Tifinagh,
	eRxCT_Ethiopic_Extended,
	eRxCT_Cyrillic_Extended_A,
	eRxCT_Supplemental_Punctuation,
	eRxCT_CJK_Radicals_Supplement,
	eRxCT_Kangxi_Radicals,
	eRxCT_Ideographic_Description_Characters,
	eRxCT_CJK_Symbols_and_Punctuation,
	eRxCT_Hiragana,
	eRxCT_Katakana,
	eRxCT_Bopomofo,
	eRxCT_Hangul_Compatibility_Jamo,

	eRxCT_Kanbun,
	eRxCT_Bopomofo_Extended,
	eRxCT_CJK_Strokes,
	eRxCT_Katakana_Phonetic_Extensions,
	eRxCT_Enclosed_CJK_Letters_and_Months,
	eRxCT_CJK_Compatibility,
	eRxCT_CJK_Unified_Ideographs_Extension_A,
	eRxCT_Yijing_Hexagram_Symbols,
	eRxCT_CJK_Unified_Ideographs,
	eRxCT_Yi_Syllables,
	eRxCT_Yi_Radicals,
	eRxCT_Lisu,
	eRxCT_Vai,
	eRxCT_Cyrillic_Extended_B,
	eRxCT_Bamum,
	eRxCT_Modifier_Tone_Letters,
	eRxCT_Latin_Extended_D,
	eRxCT_Syloti_Nagri,
	eRxCT_Common_Indic_Number_Forms,
	eRxCT_Phags_pa,
	eRxCT_Saurashtra,
	eRxCT_Devanagari_Extended,
	eRxCT_Kayah_Li,
	eRxCT_Rejang,
	eRxCT_Hangul_Jamo_Extended_A,
	eRxCT_Javanese,
	eRxCT_Cham,
	eRxCT_Myanmar_Extended_A,
	eRxCT_Tai_Viet,
	eRxCT_Meetei_Mayek,
	eRxCT_Hangul_Syllables,
	eRxCT_Hangul_Jamo_Extended_B,
	eRxCT_High_Surrogates,
	eRxCT_High_Private_Use_Surrogates,
	eRxCT_Low_Surrogates,
	eRxCT_Private_Use_Area,
	eRxCT_CJK_Compatibility_Ideographs,
	eRxCT_Alphabetic_Presentation_Forms,
	eRxCT_Arabic_Presentation_Forms_A,
	eRxCT_Variation_Selectors,
	eRxCT_Vertical_Forms,
	eRxCT_Combining_Half_Marks,
	eRxCT_CJK_Compatibility_Forms,
	eRxCT_Small_Form_Variants,
	eRxCT_Arabic_Presentation_Forms_B,
	eRxCT_Halfwidth_and_Fullwidth_Forms,
	eRxCT_Specials,



#if 0 /* not support */
	eRxCT_Linear_B_Syllabary,
	eRxCT_Linear_B_Ideograms,
	eRxCT_Aegean_Numbers,
	eRxCT_Ancient_Greek_Numbers,
	eRxCT_Ancient_Symbols,
	eRxCT_Phaistos_Disc,
	eRxCT_Lycian,
	eRxCT_Carian,
	eRxCT_Old_Italic,
	eRxCT_Gothic,
	eRxCT_Ugaritic,
	eRxCT_Old_Persian,
	eRxCT_Deseret,
	eRxCT_Shavian,
	eRxCT_Osmanya,
	eRxCT_Cypriot_Syllabary,
	eRxCT_Imperial_Aramaic,
	eRxCT_Phoenician,
	eRxCT_Lydian,
	eRxCT_Kharoshthi,
	eRxCT_Old_South_Arabian,
	eRxCT_Avestan,
	eRxCT_Inscriptional_Parthian,
	eRxCT_Inscriptional_Pahlavi,
	eRxCT_Old_Turkic,
	eRxCT_Rumi_Numeral_Symbols,
	eRxCT_Kaithi,
	eRxCT_Cuneiform,
	eRxCT_Cuneiform_Numbers_and_Punctuation,
	eRxCT_Egyptian_Hieroglyphs,
	eRxCT_Byzantine_Musical_Symbols,
	eRxCT_Musical_Symbols,
	eRxCT_Ancient_Greek_Musical_Notation,
	eRxCT_Tai_Xuan_Jing_Symbols,
	eRxCT_Counting_Rod_Numerals,
	eRxCT_Mathematical_Alphanumeric_Symbols,
	eRxCT_Mahjong_Tiles,
	eRxCT_Domino_Tiles,

	eRxCT_Enclosed_Alphanumeric_Supplement,
	eRxCT_Enclosed_Ideographic_Supplement,
	eRxCT_CJK_Unified_Ideographs_Extension_B,
	eRxCT_CJK_Unified_Ideographs_Extension_C,
	eRxCT_CJK_Compatibility_Ideographs_Supplement,
	eRxCT_Tags,
	eRxCT_Variation_Selectors_Supplement,
	eRxCT_Supplementary_Private_Use_Area_A,
	eRxCT_Supplementary_Private_Use_Area_B,
#endif
	eRxCT_Max
} RxCT_e;



typedef HINT32 (*RxCPNumOfBytes)(const void *pSrc);

typedef HINT32 (*RxCPNumOfChars)(const void *pSrc);
typedef HINT32 (*RxCPGetCharAt)(const void *pSrc, const HINT32 nCharNum,  void *pCharBuf);


typedef HINT32 (*MHConvCodepageToUCS2)(const void 	*pSrcStr, HUINT16 	*pDestUCS2Str, HINT32 nDestStrNumOfBytes);
typedef HINT32 (*MHConvUCS2ToCodepage)(const HUINT16 	*pSrcUCS2Str, void 	*pDestStr, HINT32 nDestStrNumOfBytes);

typedef struct _RxCPTagSlot_t
{
	MHCp_e eCodepage;
	HUINT8		szCPTag[4];
	HINT32		nCPTagLen;
} RxCPTagSlot_t;

typedef struct _RxCPSlot
{
	RxCPNumOfChars			cbNumOfChars;
	RxCPNumOfBytes			cbNumOfBytes;
	RxCPGetCharAt				cbGetCharAt;

	MHConvCodepageToUCS2	cbCodeapgeToUCS2;
	MHConvUCS2ToCodepage	cbUCS2ToCodepage;

	RxCPTagSlot_t				stCPTagSlot;

	HUINT8					ucEndCharBytes;
	const HUINT16 			*pusUCS2Table;
} RxCPSlot_t;






/* *************************************************************************************************** */
#define ___MACRO_FUCTION_Definition___________________________________________________________________________
/* *************************************************************************************************** */
#define		RxCP_IsValidReturnValue(x)		((x <= 0) ? FALSE : TRUE)
#define		RxCP_IsErrorCheck(x)				((x < 0) ? FALSE : TRUE)



/* *************************************************************************************************** */
#define ___Function_Definition___________________________________________________________________________
/* *************************************************************************************************** */

HERROR	RxCP_Init(void);
HERROR	RxCP_Destroy(void);

HERROR	RxCP_GetSlot(MHCp_e eCodepage, RxCPSlot_t *pstCPSlot);
HERROR	RxCP_AddCodepage(RxCPSlot_t *pstCPSlot);
HERROR	RxCP_CPTagByCodepage(const HUINT8 *pcStr, MHCp_e *peCodepage, HINT32 *pnCPTagLen);
HERROR	RxCP_CodepageByCPTag(MHCp_e eCodepage, HUINT8 *pszCPTagStr, HINT32 *pnCPTagLen);

HINT32	RxCP_NumOfBytes(MHCp_e eCodepage, const void *pSrc);
HINT32	RxCP_NumOfChars(MHCp_e eCodepage, const void *pSrc);
HINT32	RxCP_CharAt(MHCp_e eCodepage, const void *pSrc, const HINT32 nCharNum,  void *pCharBuf);
HINT32	RxCP_ConvertUCS2ToCodepage(MHCp_e eCodepage, const HUINT16 	*pSrcUCS2Str, void 	*pDestStr, HINT32 nDestStrNumOfBytes);
HINT32	RxCP_ConvertCodepageToUCS2(MHCp_e eCodepage, const void 	*pSrcStr, HUINT16 	*pDestUCS2Str, HINT32 nDestStrNumOfBytes);

RxCT_e	RxCP_UCS2CodeTable(HUINT16 usUCS2);
RxCT_e 	RxCP_StringUCS2CodeTable(HUINT16 *pszUCS2, HUINT32	 nNumOfChars);


const HUINT16	*RxCP_CodepageToUCS2Table(MHCp_e eCodepage);











#endif /* _MH_CODEPAGE_H_ */

