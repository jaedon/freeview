
/* *************************************************************************************************** */
#define ___HEADER________________________________________________________________________________________
/* *************************************************************************************************** */

#include <hlib.h>


#include "_cp_internal.h"

/* *************************************************************************************************** */
#define ___GLOBAL_HEADER_________________________________________________________________________________
/* *************************************************************************************************** */


/* *************************************************************************************************** */
#define ___DEFINE________________________________________________________________________________________
/* *************************************************************************************************** */


/* *************************************************************************************************** */
#define ___TYPEDEF_______________________________________________________________________________________
/* *************************************************************************************************** */
typedef struct tagHxUCS2Table_t
{
	HUINT32			ulStartNum;
	HUINT32			ulEndNum;
	eHxCP_CodeTable_e	eTableEnum;	/* enum */
} HxUCS2Table_t;

/* *************************************************************************************************** */
#define ___STATIC_VARIABLES______________________________________________________________________________
/* *************************************************************************************************** */

STATIC HxUCS2Table_t s_stUCS2CodeTable[eHxCP_CODE_TABLE_Max] =
{
	{0x0000, 0x007F, eHxCP_CODE_TABLE_Basic_Latin},
	{0x0080, 0x00FF, eHxCP_CODE_TABLE_Latin_1_Supplement},
	{0x0100, 0x017F, eHxCP_CODE_TABLE_Latin_Extended_A},
	{0x0180, 0x024F, eHxCP_CODE_TABLE_Latin_Extended_B},
	{0x0250, 0x02AF, eHxCP_CODE_TABLE_IPA_Extensions},
	{0x02B0, 0x02FF, eHxCP_CODE_TABLE_Spacing_Modifier_Letters},
	{0x0300, 0x036F, eHxCP_CODE_TABLE_Combining_Diacritical_Marks},
	{0x0370, 0x03FF, eHxCP_CODE_TABLE_Greek_and_Coptic},
	{0x0400, 0x04FF, eHxCP_CODE_TABLE_Cyrillic},
	{0x0500, 0x052F, eHxCP_CODE_TABLE_Cyrillic_Supplement},
	{0x0530, 0x058F, eHxCP_CODE_TABLE_Armenian},
	{0x0590, 0x05FF, eHxCP_CODE_TABLE_Hebrew},
	{0x0600, 0x06FF, eHxCP_CODE_TABLE_Arabic},
	{0x0700, 0x074F, eHxCP_CODE_TABLE_Syriac},
	{0x0750, 0x077F, eHxCP_CODE_TABLE_Arabic_Supplement},
	{0x0780, 0x07BF, eHxCP_CODE_TABLE_Thaana},
	{0x07C0, 0x07FF, eHxCP_CODE_TABLE_NKo},
	{0x0800, 0x083F, eHxCP_CODE_TABLE_Samaritan},
	{0x0900, 0x097F, eHxCP_CODE_TABLE_Devanagari},
	{0x0980, 0x09FF, eHxCP_CODE_TABLE_Bengali},
	{0x0A00, 0x0A7F, eHxCP_CODE_TABLE_Gurmukhi},
	{0x0A80, 0x0AFF, eHxCP_CODE_TABLE_Gujarati},
	{0x0B00, 0x0B7F, eHxCP_CODE_TABLE_Oriya},
	{0x0B80, 0x0BFF, eHxCP_CODE_TABLE_Tamil},
	{0x0C00, 0x0C7F, eHxCP_CODE_TABLE_Telugu},
	{0x0C80, 0x0CFF, eHxCP_CODE_TABLE_Kannada},
	{0x0D00, 0x0D7F, eHxCP_CODE_TABLE_Malayalam},
	{0x0D80, 0x0DFF, eHxCP_CODE_TABLE_Sinhala},
	{0x0E00, 0x0E7F, eHxCP_CODE_TABLE_Thai},
	{0x0E80, 0x0EFF, eHxCP_CODE_TABLE_Lao},
	{0x0F00, 0x0FFF, eHxCP_CODE_TABLE_Tibetan},
	{0x1000, 0x109F, eHxCP_CODE_TABLE_Myanmar},
	{0x10A0, 0x10FF, eHxCP_CODE_TABLE_Georgian},
	{0x1100, 0x11FF, eHxCP_CODE_TABLE_Hangul_Jamo},
	{0x1200, 0x137F, eHxCP_CODE_TABLE_Ethiopic},
	{0x1380, 0x139F, eHxCP_CODE_TABLE_Ethiopic_Supplement},
	{0x13A0, 0x13FF, eHxCP_CODE_TABLE_Cherokee},
	{0x1400, 0x167F, eHxCP_CODE_TABLE_Unified_Canadian_Aboriginal_Syllabics},
	{0x1680, 0x169F, eHxCP_CODE_TABLE_Ogham},
	{0x16A0, 0x16FF, eHxCP_CODE_TABLE_Runic},
	{0x1700, 0x171F, eHxCP_CODE_TABLE_Tagalog},
	{0x1720, 0x173F, eHxCP_CODE_TABLE_Hanunoo},
	{0x1740, 0x175F, eHxCP_CODE_TABLE_Buhid},
	{0x1760, 0x177F, eHxCP_CODE_TABLE_Tagbanwa},
	{0x1780, 0x17FF, eHxCP_CODE_TABLE_Khmer},
	{0x1800, 0x18AF, eHxCP_CODE_TABLE_Mongolian},
	{0x18B0, 0x18FF, eHxCP_CODE_TABLE_Unified_Canadian_Aboriginal_Syllabics_Extended,},
	{0x1900, 0x194F, eHxCP_CODE_TABLE_Limbu},
	{0x1950, 0x197F, eHxCP_CODE_TABLE_Tai_Le},
	{0x1980, 0x19DF, eHxCP_CODE_TABLE_New_Tai_Lue},
	{0x19E0, 0x19FF, eHxCP_CODE_TABLE_Khmer_Symbols},
	{0x1A00, 0x1A1F, eHxCP_CODE_TABLE_Buginese},
	{0x1A20, 0x1AAF, eHxCP_CODE_TABLE_Tai_Tham},
	{0x1B00, 0x1B7F, eHxCP_CODE_TABLE_Balinese},
	{0x1B80, 0x1BBF, eHxCP_CODE_TABLE_Sundanese},
	{0x1C00, 0x1C4F, eHxCP_CODE_TABLE_Lepcha},
	{0x1C50, 0x1C7F, eHxCP_CODE_TABLE_Ol_Chiki},
	{0x1CD0, 0x1CFF, eHxCP_CODE_TABLE_Vedic_Extensions},
	{0x1D00, 0x1D7F, eHxCP_CODE_TABLE_Phonetic_Extensions},
	{0x1D80, 0x1DBF, eHxCP_CODE_TABLE_Phonetic_Extensions_Supplement},
	{0x1DC0, 0x1DFF, eHxCP_CODE_TABLE_Combining_Diacritical_Marks_Supplement},
	{0x1E00, 0x1EFF, eHxCP_CODE_TABLE_Latin_Extended_Additional},
	{0x1F00, 0x1FFF, eHxCP_CODE_TABLE_Greek_Extended},
	{0x2000, 0x206F, eHxCP_CODE_TABLE_General_Punctuation},
	{0x2070, 0x209F, eHxCP_CODE_TABLE_Superscripts_and_Subscripts},
	{0x20A0, 0x20CF, eHxCP_CODE_TABLE_Currency_Symbols},
	{0x20D0, 0x20FF, eHxCP_CODE_TABLE_Combining_Diacritical_Marks_for_Symbols},
	{0x2100, 0x214F, eHxCP_CODE_TABLE_Letterlike_Symbols},
	{0x2150, 0x218F, eHxCP_CODE_TABLE_Number_Forms},
	{0x2190, 0x21FF, eHxCP_CODE_TABLE_Arrows},
	{0x2200, 0x22FF, eHxCP_CODE_TABLE_Mathematical_Operators},
	{0x2300, 0x23FF, eHxCP_CODE_TABLE_Miscellaneous_Technical},
	{0x2400, 0x243F, eHxCP_CODE_TABLE_Control_Pictures},
	{0x2440, 0x245F, eHxCP_CODE_TABLE_Optical_Character_Recognition},
	{0x2460, 0x24FF, eHxCP_CODE_TABLE_Enclosed_Alphanumerics},
	{0x2500, 0x257F, eHxCP_CODE_TABLE_Box_Drawing},
	{0x2580, 0x259F, eHxCP_CODE_TABLE_Block_Elements},
	{0x25A0, 0x25FF, eHxCP_CODE_TABLE_Geometric_Shapes},
	{0x2600, 0x26FF, eHxCP_CODE_TABLE_Miscellaneous_Symbols},
	{0x2700, 0x27BF, eHxCP_CODE_TABLE_Dingbats},
	{0x27C0, 0x27EF, eHxCP_CODE_TABLE_Miscellaneous_Mathematical_Symbols_A},
	{0x27F0, 0x27FF, eHxCP_CODE_TABLE_Supplemental_Arrows_A},
	{0x2800, 0x28FF, eHxCP_CODE_TABLE_Braille_Patterns},
	{0x2900, 0x297F, eHxCP_CODE_TABLE_Supplemental_Arrows_B},
	{0x2980, 0x29FF, eHxCP_CODE_TABLE_Miscellaneous_Mathematical_Symbols_B},
	{0x2A00, 0x2AFF, eHxCP_CODE_TABLE_Supplemental_Mathematical_Operators},
	{0x2B00, 0x2BFF, eHxCP_CODE_TABLE_Miscellaneous_Symbols_and_Arrows},
	{0x2C00, 0x2C5F, eHxCP_CODE_TABLE_Glagolitic},
	{0x2C60, 0x2C7F, eHxCP_CODE_TABLE_Latin_Extended_C},
	{0x2C80, 0x2CFF, eHxCP_CODE_TABLE_Coptic},
	{0x2D00, 0x2D2F, eHxCP_CODE_TABLE_Georgian_Supplement},
	{0x2D30, 0x2D7F, eHxCP_CODE_TABLE_Tifinagh},
	{0x2D80, 0x2DDF, eHxCP_CODE_TABLE_Ethiopic_Extended},
	{0x2DE0, 0x2DFF, eHxCP_CODE_TABLE_Cyrillic_Extended_A},
	{0x2E00, 0x2E7F, eHxCP_CODE_TABLE_Supplemental_Punctuation},
	{0x2E80, 0x2EFF, eHxCP_CODE_TABLE_CJK_Radicals_Supplement},
	{0x2F00, 0x2FDF, eHxCP_CODE_TABLE_Kangxi_Radicals},
	{0x2FF0, 0x2FFF, eHxCP_CODE_TABLE_Ideographic_Description_Characters},
	{0x3000, 0x303F, eHxCP_CODE_TABLE_CJK_Symbols_and_Punctuation},
	{0x3040, 0x309F, eHxCP_CODE_TABLE_Hiragana},
	{0x30A0, 0x30FF, eHxCP_CODE_TABLE_Katakana},
	{0x3100, 0x312F, eHxCP_CODE_TABLE_Bopomofo},
	{0x3130, 0x318F, eHxCP_CODE_TABLE_Hangul_Compatibility_Jamo},
	{0x3190, 0x319F, eHxCP_CODE_TABLE_Kanbun},
	{0x31A0, 0x31BF, eHxCP_CODE_TABLE_Bopomofo_Extended},
	{0x31C0, 0x31EF, eHxCP_CODE_TABLE_CJK_Strokes},
	{0x31F0, 0x31FF, eHxCP_CODE_TABLE_Katakana_Phonetic_Extensions},
	{0x3200, 0x32FF, eHxCP_CODE_TABLE_Enclosed_CJK_Letters_and_Months},
	{0x3300, 0x33FF, eHxCP_CODE_TABLE_CJK_Compatibility},
	{0x3400, 0x4DBF, eHxCP_CODE_TABLE_CJK_Unified_Ideographs_Extension_A},
	{0x4DC0, 0x4DFF, eHxCP_CODE_TABLE_Yijing_Hexagram_Symbols},
	{0x4E00, 0x9FFF, eHxCP_CODE_TABLE_CJK_Unified_Ideographs},
	{0xA000, 0xA48F, eHxCP_CODE_TABLE_Yi_Syllables},
	{0xA490, 0xA4CF, eHxCP_CODE_TABLE_Yi_Radicals},
	{0xA4D0, 0xA4FF, eHxCP_CODE_TABLE_Lisu},
	{0xA500, 0xA63F, eHxCP_CODE_TABLE_Vai},
	{0xA640, 0xA69F, eHxCP_CODE_TABLE_Cyrillic_Extended_B},
	{0xA6A0, 0xA6FF, eHxCP_CODE_TABLE_Bamum},
	{0xA700, 0xA71F, eHxCP_CODE_TABLE_Modifier_Tone_Letters},
	{0xA720, 0xA7FF, eHxCP_CODE_TABLE_Latin_Extended_D},
	{0xA800, 0xA82F, eHxCP_CODE_TABLE_Syloti_Nagri},
	{0xA830, 0xA83F, eHxCP_CODE_TABLE_Common_Indic_Number_Forms},
	{0xA840, 0xA87F, eHxCP_CODE_TABLE_Phags_pa},
	{0xA880, 0xA8DF, eHxCP_CODE_TABLE_Saurashtra},
	{0xA8E0, 0xA8FF, eHxCP_CODE_TABLE_Devanagari_Extended},
	{0xA900, 0xA92F, eHxCP_CODE_TABLE_Kayah_Li},
	{0xA930, 0xA95F, eHxCP_CODE_TABLE_Rejang},
	{0xA960, 0xA97F, eHxCP_CODE_TABLE_Hangul_Jamo_Extended_A},
	{0xA980, 0xA9DF, eHxCP_CODE_TABLE_Javanese},
	{0xAA00, 0xAA5F, eHxCP_CODE_TABLE_Cham},
	{0xAA60, 0xAA7F, eHxCP_CODE_TABLE_Myanmar_Extended_A},
	{0xAA80, 0xAADF, eHxCP_CODE_TABLE_Tai_Viet},
	{0xABC0, 0xABFF, eHxCP_CODE_TABLE_Meetei_Mayek},
	{0xAC00, 0xD7AF, eHxCP_CODE_TABLE_Hangul_Syllables},
	{0xD7B0, 0xD7FF, eHxCP_CODE_TABLE_Hangul_Jamo_Extended_B},
	{0xD800, 0xDB7F, eHxCP_CODE_TABLE_High_Surrogates},
	{0xDB80, 0xDBFF, eHxCP_CODE_TABLE_High_Private_Use_Surrogates},
	{0xDC00, 0xDFFF, eHxCP_CODE_TABLE_Low_Surrogates},
	{0xE000, 0xF8FF, eHxCP_CODE_TABLE_Private_Use_Area},
	{0xF900, 0xFAFF, eHxCP_CODE_TABLE_CJK_Compatibility_Ideographs},
	{0xFB00, 0xFB4F, eHxCP_CODE_TABLE_Alphabetic_Presentation_Forms},
	{0xFB50, 0xFDFF, eHxCP_CODE_TABLE_Arabic_Presentation_Forms_A},
	{0xFE00, 0xFE0F, eHxCP_CODE_TABLE_Variation_Selectors},
	{0xFE10, 0xFE1F, eHxCP_CODE_TABLE_Vertical_Forms},
	{0xFE20, 0xFE2F, eHxCP_CODE_TABLE_Combining_Half_Marks},
	{0xFE30, 0xFE4F, eHxCP_CODE_TABLE_CJK_Compatibility_Forms},
	{0xFE50, 0xFE6F, eHxCP_CODE_TABLE_Small_Form_Variants},
	{0xFE70, 0xFEFF, eHxCP_CODE_TABLE_Arabic_Presentation_Forms_B},
	{0xFF00, 0xFFEF, eHxCP_CODE_TABLE_Halfwidth_and_Fullwidth_Forms},
	{0xFFF0, 0xFFFF, eHxCP_CODE_TABLE_Specials},

#if 0 /* not support */
	{0x10000, 0x1007F, eHxCP_CODE_TABLE_Linear_B_Syllabary},
	{0x10080, 0x100FF, eHxCP_CODE_TABLE_Linear_B_Ideograms},
	{0x10100, 0x1013F, eHxCP_CODE_TABLE_Aegean_Numbers},
	{0x10140, 0x1018F, eHxCP_CODE_TABLE_Ancient_Greek_Numbers},
	{0x10190, 0x101CF, eHxCP_CODE_TABLE_Ancient_Symbols},
	{0x101D0, 0x101FF, eHxCP_CODE_TABLE_Phaistos_Disc},
	{0x10280, 0x1029F, eHxCP_CODE_TABLE_Lycian},
	{0x102A0, 0x102DF, eHxCP_CODE_TABLE_Carian},
	{0x10300, 0x1032F, eHxCP_CODE_TABLE_Old_Italic},
	{0x10330, 0x1034F, eHxCP_CODE_TABLE_Gothic},
	{0x10380, 0x1039F, eHxCP_CODE_TABLE_Ugaritic},
	{0x103A0, 0x103DF, eHxCP_CODE_TABLE_Old_Persian},
	{0x10400, 0x1044F, eHxCP_CODE_TABLE_Deseret},
	{0x10450, 0x1047F, eHxCP_CODE_TABLE_Shavian},
	{0x10480, 0x104AF, eHxCP_CODE_TABLE_Osmanya},
	{0x10800, 0x1083F, eHxCP_CODE_TABLE_Cypriot_Syllabary},
	{0x10840, 0x1085F, eHxCP_CODE_TABLE_Imperial_Aramaic},
	{0x10900, 0x1091F, eHxCP_CODE_TABLE_Phoenician},
	{0x10920, 0x1093F, eHxCP_CODE_TABLE_Lydian},
	{0x10A00, 0x10A5F, eHxCP_CODE_TABLE_Kharoshthi},
	{0x10A60, 0x10A7F, eHxCP_CODE_TABLE_Old_South_Arabian},
	{0x10B00, 0x10B3F, eHxCP_CODE_TABLE_Avestan},
	{0x10B40, 0x10B5F, eHxCP_CODE_TABLE_Inscriptional_Parthian},
	{0x10B60, 0x10B7F, eHxCP_CODE_TABLE_Inscriptional_Pahlavi},
	{0x10C00, 0x10C4F, eHxCP_CODE_TABLE_Old_Turkic},
	{0x10E60, 0x10E7F, eHxCP_CODE_TABLE_Rumi_Numeral_Symbols},
	{0x11080, 0x110CF, eHxCP_CODE_TABLE_Kaithi},
	{0x12000, 0x123FF, eHxCP_CODE_TABLE_Cuneiform},
	{0x12400, 0x1247F, eHxCP_CODE_TABLE_Cuneiform_Numbers_and_Punctuation},
	{0x13000, 0x1342F, eHxCP_CODE_TABLE_Egyptian_Hieroglyphs},
	{0x1D000, 0x1D0FF, eHxCP_CODE_TABLE_Byzantine_Musical_Symbols},
	{0x1D100, 0x1D1FF, eHxCP_CODE_TABLE_Musical_Symbols},
	{0x1D200, 0x1D24F, eHxCP_CODE_TABLE_Ancient_Greek_Musical_Notation},
	{0x1D300, 0x1D35F, eHxCP_CODE_TABLE_Tai_Xuan_Jing_Symbols},
	{0x1D360, 0x1D37F, eHxCP_CODE_TABLE_Counting_Rod_Numerals},
	{0x1D400, 0x1D7FF, eHxCP_CODE_TABLE_Mathematical_Alphanumeric_Symbols},
	{0x1F000, 0x1F02F, eHxCP_CODE_TABLE_Mahjong_Tiles},
	{0x1F030, 0x1F09F, eHxCP_CODE_TABLE_Domino_Tiles},
	{0x1F100, 0x1F1FF, eHxCP_CODE_TABLE_Enclosed_Alphanumeric_Supplement},
	{0x1F200, 0x1F2FF, eHxCP_CODE_TABLE_Enclosed_Ideographic_Supplement},
	{0x20000, 0x2A6DF, eHxCP_CODE_TABLE_CJK_Unified_Ideographs_Extension_B},
	{0x2A700, 0x2B73F, eHxCP_CODE_TABLE_CJK_Unified_Ideographs_Extension_C},
	{0x2F800, 0x2FA1F, eHxCP_CODE_TABLE_CJK_Compatibility_Ideographs_Supplement},
	{0xE0000, 0xE007F, eHxCP_CODE_TABLE_Tags},
	{0xE0100, 0xE01EF, eHxCP_CODE_TABLE_Variation_Selectors_Supplement},
	{0xF0000, 0xFFFFF, eHxCP_CODE_TABLE_Supplementary_Private_Use_Area_A},
	{0x100000, 0x10FFFF, eHxCP_CODE_TABLE_Supplementary_Private_Use_Area_B},
#endif
};


/* *************************************************************************************************** */
#define ___Private_FUNCTION______________________________________________________________________________
/* *************************************************************************************************** */
STATIC	HINT32			_hcodepage_ucs2_NumOfChars(const void 	*pSrc)
{
	HUINT8	*pcIdx = (HUINT8	*)pSrc;
	HUINT32 ulCnt = 0;

	while (*pcIdx || *(pcIdx + 1))
	{
		pcIdx += 2;
		ulCnt += 1;
	}

	return ulCnt;
}

STATIC	HINT32			_hcodepage_ucs2_NumOfBytes(const void 	*pSrc)
{
	HUINT32 ulCnt = 0;

	ulCnt = _hcodepage_ucs2_NumOfChars(pSrc);

	return ulCnt * sizeof(HUINT16);
}


STATIC	HINT32			_hcodepage_ucs2_GetCharAt(const void *pSrc, const HINT32 nCharNum, void *pCharBuf)
{
	HUINT16 *pszUCS2 	= (HUINT16 *)pSrc;
	HUINT16	*pusCharBuf = (HUINT16 *)pCharBuf;

	*pusCharBuf = pszUCS2[nCharNum];
	return 2;
}

STATIC	HINT32 			_hcodepage_ucs2_ConvertUCS2ToCodepage(const HUINT16 	*pSrcUCS2Str, void 	*pDestStr, HINT32 nDestStrNumOfBytes)
{
	HINT32 		nSrcBytes = 0;
	HINT32 		nMaxBytes = 0;
	HUINT16		*pDestString = (HUINT16	*)pDestStr;

	nSrcBytes = _hcodepage_ucs2_NumOfBytes(pDestString);
	if(HxCP_IsValidReturnValue(nSrcBytes) == FALSE)
	{
		return eHxCP_ERROR_FAIL;
	}

	nDestStrNumOfBytes -= sizeof(HUINT16);
	nMaxBytes = (nSrcBytes > nDestStrNumOfBytes) ? nDestStrNumOfBytes : nSrcBytes;

	HxSTD_MemCopy(pDestString, pSrcUCS2Str, nMaxBytes);
	pDestString[nMaxBytes>>2] = 0;

	return nMaxBytes;
}

STATIC	HINT32 			_hcodepage_ucs2_ConvertCodepageToUCS2(const void 	*pSrcStr, HUINT16 	*pDestUCS2Str, HINT32 nDestStrNumOfBytes)
{
	HINT32 		nSrcBytes = 0;
	HINT32 		nMaxBytes = 0;
	HUINT16		*pSrcString = (HUINT16	*)pSrcStr;

	nSrcBytes = _hcodepage_ucs2_NumOfBytes(pSrcString);
	if(HxCP_IsValidReturnValue(nSrcBytes) == FALSE)
	{
		return eHxCP_ERROR_FAIL;
	}


	nDestStrNumOfBytes -= sizeof(HUINT16);
	nMaxBytes = (nSrcBytes > nDestStrNumOfBytes) ? nDestStrNumOfBytes : nSrcBytes;

	HxSTD_MemCopy(pDestUCS2Str, pSrcString, nMaxBytes);
	pDestUCS2Str[nMaxBytes>>2] = 0;

	return nMaxBytes;
}


STATIC	HERROR			_hcodepage_ucs2_GeIinfo(eHxCP_Slot_t *pCPSlot)
{
	if(pCPSlot == NULL)
	{
		return ERR_FAIL;
	}

	pCPSlot->cbNumOfBytes	= _hcodepage_ucs2_NumOfBytes;
	pCPSlot->cbNumOfChars	= _hcodepage_ucs2_NumOfChars;
	pCPSlot->cbGetCharAt	= _hcodepage_ucs2_GetCharAt;

	pCPSlot->cbUCS2ToCodepage	= _hcodepage_ucs2_ConvertUCS2ToCodepage;
	pCPSlot->cbCodeapgeToUCS2	= _hcodepage_ucs2_ConvertCodepageToUCS2;

	pCPSlot->stCPTagSlot.eCodepage = eHxCP_UNICODE_UCS2;
	pCPSlot->ucEndCharBytes	= 2;

	pCPSlot->stCPTagSlot.szCPTag[0] = eHxCP_DVBLANG_VALUE_UCS2;
	pCPSlot->stCPTagSlot.szCPTag[1] = 0;

#if defined(TEST_CP)
	pCPSlot->stCPTagSlot.szCPTag2[0] = 0;
	pCPSlot->stCPTagSlot.nCPTagLen2 = 0;
#endif
	

	return ERR_OK;
}

STATIC	HERROR			_hcodepage_dvbucs2_getinfo(eHxCP_Slot_t *pCPSlot)
{
	if(pCPSlot == NULL)
	{
		return ERR_FAIL;
	}

	pCPSlot->cbNumOfBytes	= _hcodepage_ucs2_NumOfBytes;
	pCPSlot->cbNumOfChars	= _hcodepage_ucs2_NumOfChars;
	pCPSlot->cbGetCharAt	= _hcodepage_ucs2_GetCharAt;

	pCPSlot->cbUCS2ToCodepage	= _hcodepage_ucs2_ConvertUCS2ToCodepage;
	pCPSlot->cbCodeapgeToUCS2	= _hcodepage_ucs2_ConvertCodepageToUCS2;

	pCPSlot->stCPTagSlot.eCodepage = eHxCP_UNICODE_DVB_UCS2;
	pCPSlot->ucEndCharBytes	= 2;

	pCPSlot->stCPTagSlot.szCPTag[0] = eHxCP_DVBLANG_VALUE_UCS2;
	pCPSlot->stCPTagSlot.szCPTag[1] = 0;

#if defined(TEST_CP)
	pCPSlot->stCPTagSlot.szCPTag2[0] = 0;
	pCPSlot->stCPTagSlot.nCPTagLen2 = 0;
#endif
	

	return ERR_OK;
}


/* *************************************************************************************************** */
#define ___PUBLIC_FUNCTION_______________________________________________________________________________
/* *************************************************************************************************** */


HERROR	INT_HCodepage_UCS2(void)
{
	HERROR		hErr = ERR_FAIL;
	eHxCP_Slot_t	stCPSlot;

	HxSTD_MemSet(&stCPSlot, 0, sizeof(eHxCP_Slot_t));
	hErr = _hcodepage_ucs2_GeIinfo(&stCPSlot);
	if(hErr == ERR_OK)
	{
		hErr = HLIB_CP_AddCodepage(&stCPSlot);
	}

	HxSTD_MemSet(&stCPSlot, 0, sizeof(eHxCP_Slot_t));
	hErr = _hcodepage_dvbucs2_getinfo(&stCPSlot);
	if(hErr == ERR_OK)
	{
		hErr = HLIB_CP_AddCodepage(&stCPSlot);
	}

	return ERR_OK;
}

STATIC	HxUCS2Table_t *_hcodepage_ucs2_GetCodeTable(void)
{
	return s_stUCS2CodeTable;
}

eHxCP_CodeTable_e	INT_HCodepage_GetUCS2CodeTable(HUINT16 ucUCS2)
{
	HINT32			i = 0;
	HINT32			nMid = 0;
	HINT32			nDataCnt = eHxCP_CODE_TABLE_Max - 1;
	HxUCS2Table_t 	*pstCodeTable = _hcodepage_ucs2_GetCodeTable();

	if(pstCodeTable[0].ulEndNum >= ucUCS2)
	{
		return pstCodeTable[0].eTableEnum;
	}

	while(i <= nDataCnt)
	{
		nMid = (i + nDataCnt) >> 1;
		if(pstCodeTable[nMid].ulEndNum >= ucUCS2)
		{
			if(pstCodeTable[nMid].ulStartNum <= ucUCS2)
			{
				break;
			}
			i = nMid + 1;
		}
		else
		{
			nDataCnt = nMid - 1;
		}
	}

	return pstCodeTable[nMid].eTableEnum;
}
/*********************** End of File ******************************/

