
/* *************************************************************************************************** */
#define ___HEADER________________________________________________________________________________________
/* *************************************************************************************************** */

#include <rxlib.h>


#include "rx_cp_internal.h"

/* *************************************************************************************************** */
#define ___GLOBAL_HEADER_________________________________________________________________________________
/* *************************************************************************************************** */


/* *************************************************************************************************** */
#define ___DEFINE________________________________________________________________________________________
/* *************************************************************************************************** */


/* *************************************************************************************************** */
#define ___TYPEDEF_______________________________________________________________________________________
/* *************************************************************************************************** */
typedef struct tagRxUCS2Table_t
{
	HUINT32			ulStartNum;
	HUINT32			ulEndNum;
	RxCT_e	eTableEnum;	/* enum */
} RxUCS2Table_t;

/* *************************************************************************************************** */
#define ___STATIC_VARIABLES______________________________________________________________________________
/* *************************************************************************************************** */

STATIC RxUCS2Table_t s_stUCS2CodeTable[eRxCT_Max] =
{
	{0x0000, 0x007F, eRxCT_Basic_Latin},
	{0x0080, 0x00FF, eRxCT_Latin_1_Supplement},
	{0x0100, 0x017F, eRxCT_Latin_Extended_A},
	{0x0180, 0x024F, eRxCT_Latin_Extended_B},
	{0x0250, 0x02AF, eRxCT_IPA_Extensions},
	{0x02B0, 0x02FF, eRxCT_Spacing_Modifier_Letters},
	{0x0300, 0x036F, eRxCT_Combining_Diacritical_Marks},
	{0x0370, 0x03FF, eRxCT_Greek_and_Coptic},
	{0x0400, 0x04FF, eRxCT_Cyrillic},
	{0x0500, 0x052F, eRxCT_Cyrillic_Supplement},
	{0x0530, 0x058F, eRxCT_Armenian},
	{0x0590, 0x05FF, eRxCT_Hebrew},
	{0x0600, 0x06FF, eRxCT_Arabic},
	{0x0700, 0x074F, eRxCT_Syriac},
	{0x0750, 0x077F, eRxCT_Arabic_Supplement},
	{0x0780, 0x07BF, eRxCT_Thaana},
	{0x07C0, 0x07FF, eRxCT_NKo},
	{0x0800, 0x083F, eRxCT_Samaritan},
	{0x0900, 0x097F, eRxCT_Devanagari},
	{0x0980, 0x09FF, eRxCT_Bengali},
	{0x0A00, 0x0A7F, eRxCT_Gurmukhi},
	{0x0A80, 0x0AFF, eRxCT_Gujarati},
	{0x0B00, 0x0B7F, eRxCT_Oriya},
	{0x0B80, 0x0BFF, eRxCT_Tamil},
	{0x0C00, 0x0C7F, eRxCT_Telugu},
	{0x0C80, 0x0CFF, eRxCT_Kannada},
	{0x0D00, 0x0D7F, eRxCT_Malayalam},
	{0x0D80, 0x0DFF, eRxCT_Sinhala},
	{0x0E00, 0x0E7F, eRxCT_Thai},
	{0x0E80, 0x0EFF, eRxCT_Lao},
	{0x0F00, 0x0FFF, eRxCT_Tibetan},
	{0x1000, 0x109F, eRxCT_Myanmar},
	{0x10A0, 0x10FF, eRxCT_Georgian},
	{0x1100, 0x11FF, eRxCT_Hangul_Jamo},
	{0x1200, 0x137F, eRxCT_Ethiopic},
	{0x1380, 0x139F, eRxCT_Ethiopic_Supplement},
	{0x13A0, 0x13FF, eRxCT_Cherokee},
	{0x1400, 0x167F, eRxCT_Unified_Canadian_Aboriginal_Syllabics},
	{0x1680, 0x169F, eRxCT_Ogham},
	{0x16A0, 0x16FF, eRxCT_Runic},
	{0x1700, 0x171F, eRxCT_Tagalog},
	{0x1720, 0x173F, eRxCT_Hanunoo},
	{0x1740, 0x175F, eRxCT_Buhid},
	{0x1760, 0x177F, eRxCT_Tagbanwa},
	{0x1780, 0x17FF, eRxCT_Khmer},
	{0x1800, 0x18AF, eRxCT_Mongolian},
	{0x18B0, 0x18FF, eRxCT_Unified_Canadian_Aboriginal_Syllabics_Extended,},
	{0x1900, 0x194F, eRxCT_Limbu},
	{0x1950, 0x197F, eRxCT_Tai_Le},
	{0x1980, 0x19DF, eRxCT_New_Tai_Lue},
	{0x19E0, 0x19FF, eRxCT_Khmer_Symbols},
	{0x1A00, 0x1A1F, eRxCT_Buginese},
	{0x1A20, 0x1AAF, eRxCT_Tai_Tham},
	{0x1B00, 0x1B7F, eRxCT_Balinese},
	{0x1B80, 0x1BBF, eRxCT_Sundanese},
	{0x1C00, 0x1C4F, eRxCT_Lepcha},
	{0x1C50, 0x1C7F, eRxCT_Ol_Chiki},
	{0x1CD0, 0x1CFF, eRxCT_Vedic_Extensions},
	{0x1D00, 0x1D7F, eRxCT_Phonetic_Extensions},
	{0x1D80, 0x1DBF, eRxCT_Phonetic_Extensions_Supplement},
	{0x1DC0, 0x1DFF, eRxCT_Combining_Diacritical_Marks_Supplement},
	{0x1E00, 0x1EFF, eRxCT_Latin_Extended_Additional},
	{0x1F00, 0x1FFF, eRxCT_Greek_Extended},
	{0x2000, 0x206F, eRxCT_General_Punctuation},
	{0x2070, 0x209F, eRxCT_Superscripts_and_Subscripts},
	{0x20A0, 0x20CF, eRxCT_Currency_Symbols},
	{0x20D0, 0x20FF, eRxCT_Combining_Diacritical_Marks_for_Symbols},
	{0x2100, 0x214F, eRxCT_Letterlike_Symbols},
	{0x2150, 0x218F, eRxCT_Number_Forms},
	{0x2190, 0x21FF, eRxCT_Arrows},
	{0x2200, 0x22FF, eRxCT_Mathematical_Operators},
	{0x2300, 0x23FF, eRxCT_Miscellaneous_Technical},
	{0x2400, 0x243F, eRxCT_Control_Pictures},
	{0x2440, 0x245F, eRxCT_Optical_Character_Recognition},
	{0x2460, 0x24FF, eRxCT_Enclosed_Alphanumerics},
	{0x2500, 0x257F, eRxCT_Box_Drawing},
	{0x2580, 0x259F, eRxCT_Block_Elements},
	{0x25A0, 0x25FF, eRxCT_Geometric_Shapes},
	{0x2600, 0x26FF, eRxCT_Miscellaneous_Symbols},
	{0x2700, 0x27BF, eRxCT_Dingbats},
	{0x27C0, 0x27EF, eRxCT_Miscellaneous_Mathematical_Symbols_A},
	{0x27F0, 0x27FF, eRxCT_Supplemental_Arrows_A},
	{0x2800, 0x28FF, eRxCT_Braille_Patterns},
	{0x2900, 0x297F, eRxCT_Supplemental_Arrows_B},
	{0x2980, 0x29FF, eRxCT_Miscellaneous_Mathematical_Symbols_B},
	{0x2A00, 0x2AFF, eRxCT_Supplemental_Mathematical_Operators},
	{0x2B00, 0x2BFF, eRxCT_Miscellaneous_Symbols_and_Arrows},
	{0x2C00, 0x2C5F, eRxCT_Glagolitic},
	{0x2C60, 0x2C7F, eRxCT_Latin_Extended_C},
	{0x2C80, 0x2CFF, eRxCT_Coptic},
	{0x2D00, 0x2D2F, eRxCT_Georgian_Supplement},
	{0x2D30, 0x2D7F, eRxCT_Tifinagh},
	{0x2D80, 0x2DDF, eRxCT_Ethiopic_Extended},
	{0x2DE0, 0x2DFF, eRxCT_Cyrillic_Extended_A},
	{0x2E00, 0x2E7F, eRxCT_Supplemental_Punctuation},
	{0x2E80, 0x2EFF, eRxCT_CJK_Radicals_Supplement},
	{0x2F00, 0x2FDF, eRxCT_Kangxi_Radicals},
	{0x2FF0, 0x2FFF, eRxCT_Ideographic_Description_Characters},
	{0x3000, 0x303F, eRxCT_CJK_Symbols_and_Punctuation},
	{0x3040, 0x309F, eRxCT_Hiragana},
	{0x30A0, 0x30FF, eRxCT_Katakana},
	{0x3100, 0x312F, eRxCT_Bopomofo},
	{0x3130, 0x318F, eRxCT_Hangul_Compatibility_Jamo},
	{0x3190, 0x319F, eRxCT_Kanbun},
	{0x31A0, 0x31BF, eRxCT_Bopomofo_Extended},
	{0x31C0, 0x31EF, eRxCT_CJK_Strokes},
	{0x31F0, 0x31FF, eRxCT_Katakana_Phonetic_Extensions},
	{0x3200, 0x32FF, eRxCT_Enclosed_CJK_Letters_and_Months},
	{0x3300, 0x33FF, eRxCT_CJK_Compatibility},
	{0x3400, 0x4DBF, eRxCT_CJK_Unified_Ideographs_Extension_A},
	{0x4DC0, 0x4DFF, eRxCT_Yijing_Hexagram_Symbols},
	{0x4E00, 0x9FFF, eRxCT_CJK_Unified_Ideographs},
	{0xA000, 0xA48F, eRxCT_Yi_Syllables},
	{0xA490, 0xA4CF, eRxCT_Yi_Radicals},
	{0xA4D0, 0xA4FF, eRxCT_Lisu},
	{0xA500, 0xA63F, eRxCT_Vai},
	{0xA640, 0xA69F, eRxCT_Cyrillic_Extended_B},
	{0xA6A0, 0xA6FF, eRxCT_Bamum},
	{0xA700, 0xA71F, eRxCT_Modifier_Tone_Letters},
	{0xA720, 0xA7FF, eRxCT_Latin_Extended_D},
	{0xA800, 0xA82F, eRxCT_Syloti_Nagri},
	{0xA830, 0xA83F, eRxCT_Common_Indic_Number_Forms},
	{0xA840, 0xA87F, eRxCT_Phags_pa},
	{0xA880, 0xA8DF, eRxCT_Saurashtra},
	{0xA8E0, 0xA8FF, eRxCT_Devanagari_Extended},
	{0xA900, 0xA92F, eRxCT_Kayah_Li},
	{0xA930, 0xA95F, eRxCT_Rejang},
	{0xA960, 0xA97F, eRxCT_Hangul_Jamo_Extended_A},
	{0xA980, 0xA9DF, eRxCT_Javanese},
	{0xAA00, 0xAA5F, eRxCT_Cham},
	{0xAA60, 0xAA7F, eRxCT_Myanmar_Extended_A},
	{0xAA80, 0xAADF, eRxCT_Tai_Viet},
	{0xABC0, 0xABFF, eRxCT_Meetei_Mayek},
	{0xAC00, 0xD7AF, eRxCT_Hangul_Syllables},
	{0xD7B0, 0xD7FF, eRxCT_Hangul_Jamo_Extended_B},
	{0xD800, 0xDB7F, eRxCT_High_Surrogates},
	{0xDB80, 0xDBFF, eRxCT_High_Private_Use_Surrogates},
	{0xDC00, 0xDFFF, eRxCT_Low_Surrogates},
	{0xE000, 0xF8FF, eRxCT_Private_Use_Area},
	{0xF900, 0xFAFF, eRxCT_CJK_Compatibility_Ideographs},
	{0xFB00, 0xFB4F, eRxCT_Alphabetic_Presentation_Forms},
	{0xFB50, 0xFDFF, eRxCT_Arabic_Presentation_Forms_A},
	{0xFE00, 0xFE0F, eRxCT_Variation_Selectors},
	{0xFE10, 0xFE1F, eRxCT_Vertical_Forms},
	{0xFE20, 0xFE2F, eRxCT_Combining_Half_Marks},
	{0xFE30, 0xFE4F, eRxCT_CJK_Compatibility_Forms},
	{0xFE50, 0xFE6F, eRxCT_Small_Form_Variants},
	{0xFE70, 0xFEFF, eRxCT_Arabic_Presentation_Forms_B},
	{0xFF00, 0xFFEF, eRxCT_Halfwidth_and_Fullwidth_Forms},
	{0xFFF0, 0xFFFF, eRxCT_Specials},

#if 0 /* not support */
	{0x10000, 0x1007F, eRxCT_Linear_B_Syllabary},
	{0x10080, 0x100FF, eRxCT_Linear_B_Ideograms},
	{0x10100, 0x1013F, eRxCT_Aegean_Numbers},
	{0x10140, 0x1018F, eRxCT_Ancient_Greek_Numbers},
	{0x10190, 0x101CF, eRxCT_Ancient_Symbols},
	{0x101D0, 0x101FF, eRxCT_Phaistos_Disc},
	{0x10280, 0x1029F, eRxCT_Lycian},
	{0x102A0, 0x102DF, eRxCT_Carian},
	{0x10300, 0x1032F, eRxCT_Old_Italic},
	{0x10330, 0x1034F, eRxCT_Gothic},
	{0x10380, 0x1039F, eRxCT_Ugaritic},
	{0x103A0, 0x103DF, eRxCT_Old_Persian},
	{0x10400, 0x1044F, eRxCT_Deseret},
	{0x10450, 0x1047F, eRxCT_Shavian},
	{0x10480, 0x104AF, eRxCT_Osmanya},
	{0x10800, 0x1083F, eRxCT_Cypriot_Syllabary},
	{0x10840, 0x1085F, eRxCT_Imperial_Aramaic},
	{0x10900, 0x1091F, eRxCT_Phoenician},
	{0x10920, 0x1093F, eRxCT_Lydian},
	{0x10A00, 0x10A5F, eRxCT_Kharoshthi},
	{0x10A60, 0x10A7F, eRxCT_Old_South_Arabian},
	{0x10B00, 0x10B3F, eRxCT_Avestan},
	{0x10B40, 0x10B5F, eRxCT_Inscriptional_Parthian},
	{0x10B60, 0x10B7F, eRxCT_Inscriptional_Pahlavi},
	{0x10C00, 0x10C4F, eRxCT_Old_Turkic},
	{0x10E60, 0x10E7F, eRxCT_Rumi_Numeral_Symbols},
	{0x11080, 0x110CF, eRxCT_Kaithi},
	{0x12000, 0x123FF, eRxCT_Cuneiform},
	{0x12400, 0x1247F, eRxCT_Cuneiform_Numbers_and_Punctuation},
	{0x13000, 0x1342F, eRxCT_Egyptian_Hieroglyphs},
	{0x1D000, 0x1D0FF, eRxCT_Byzantine_Musical_Symbols},
	{0x1D100, 0x1D1FF, eRxCT_Musical_Symbols},
	{0x1D200, 0x1D24F, eRxCT_Ancient_Greek_Musical_Notation},
	{0x1D300, 0x1D35F, eRxCT_Tai_Xuan_Jing_Symbols},
	{0x1D360, 0x1D37F, eRxCT_Counting_Rod_Numerals},
	{0x1D400, 0x1D7FF, eRxCT_Mathematical_Alphanumeric_Symbols},
	{0x1F000, 0x1F02F, eRxCT_Mahjong_Tiles},
	{0x1F030, 0x1F09F, eRxCT_Domino_Tiles},
	{0x1F100, 0x1F1FF, eRxCT_Enclosed_Alphanumeric_Supplement},
	{0x1F200, 0x1F2FF, eRxCT_Enclosed_Ideographic_Supplement},
	{0x20000, 0x2A6DF, eRxCT_CJK_Unified_Ideographs_Extension_B},
	{0x2A700, 0x2B73F, eRxCT_CJK_Unified_Ideographs_Extension_C},
	{0x2F800, 0x2FA1F, eRxCT_CJK_Compatibility_Ideographs_Supplement},
	{0xE0000, 0xE007F, eRxCT_Tags},
	{0xE0100, 0xE01EF, eRxCT_Variation_Selectors_Supplement},
	{0xF0000, 0xFFFFF, eRxCT_Supplementary_Private_Use_Area_A},
	{0x100000, 0x10FFFF, eRxCT_Supplementary_Private_Use_Area_B},
#endif
};


/* *************************************************************************************************** */
#define ___Private_FUNCTION______________________________________________________________________________
/* *************************************************************************************************** */
STATIC	HINT32			_rxcp_ucs2_NumOfChars(const void 	*pSrc)
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

STATIC	HINT32			_rxcp_ucs2_NumOfBytes(const void 	*pSrc)
{
	HUINT32 ulCnt = 0;

	ulCnt = _rxcp_ucs2_NumOfChars(pSrc);

	return ulCnt * sizeof(HUINT16);
}


STATIC	HINT32			_rxcp_ucs2_GetCharAt(const void *pSrc, const HINT32 nCharNum, void *pCharBuf)
{
	HUINT16 *pszUCS2 	= (HUINT16 *)pSrc;
	HUINT16	*pusCharBuf = (HUINT16 *)pCharBuf;

	*pusCharBuf = pszUCS2[nCharNum];
	return 2;
}

STATIC	HINT32 			_rxcp_ucs2_ConvertUCS2ToCodepage(const HUINT16 	*pSrcUCS2Str, void 	*pDestStr, HINT32 nDestStrNumOfBytes)
{
	HINT32 		nSrcBytes = 0;
	HINT32 		nMaxBytes = 0;
	HUINT16		*pDestString = (HUINT16	*)pDestStr;

	nSrcBytes = _rxcp_ucs2_NumOfBytes(pDestString);
	if(RxCP_IsValidReturnValue(nSrcBytes) == FALSE)
	{
		return ERR_FAIL;
	}

	nDestStrNumOfBytes -= sizeof(HUINT16);
	nMaxBytes = (nSrcBytes > nDestStrNumOfBytes) ? nDestStrNumOfBytes : nSrcBytes;

	RxSTD_MemCopy(pDestString, pSrcUCS2Str, nMaxBytes);
	pDestString[nMaxBytes>>2] = 0;

	return nMaxBytes;
}

STATIC	HINT32 			_rxcp_ucs2_ConvertCodepageToUCS2(const void 	*pSrcStr, HUINT16 	*pDestUCS2Str, HINT32 nDestStrNumOfBytes)
{
	HINT32 		nSrcBytes = 0;
	HINT32 		nMaxBytes = 0;
	HUINT16		*pSrcString = (HUINT16	*)pSrcStr;

	nSrcBytes = _rxcp_ucs2_NumOfBytes(pSrcString);
	if(RxCP_IsValidReturnValue(nSrcBytes) == FALSE)
	{
		return ERR_FAIL;
	}


	nDestStrNumOfBytes -= sizeof(HUINT16);
	nMaxBytes = (nSrcBytes > nDestStrNumOfBytes) ? nDestStrNumOfBytes : nSrcBytes;

	RxSTD_MemCopy(pDestUCS2Str, pSrcString, nMaxBytes);
	pDestUCS2Str[nMaxBytes>>2] = 0;

	return nMaxBytes;
}


STATIC	HERROR			_rxcp_ucs2_GeIinfo(RxCPSlot_t *pCPSlot)
{
	if(pCPSlot == NULL)
	{
		return ERR_FAIL;
	}

	pCPSlot->cbNumOfBytes	= _rxcp_ucs2_NumOfBytes;
	pCPSlot->cbNumOfChars	= _rxcp_ucs2_NumOfChars;
	pCPSlot->cbGetCharAt	= _rxcp_ucs2_GetCharAt;

	pCPSlot->cbUCS2ToCodepage	= _rxcp_ucs2_ConvertUCS2ToCodepage;
	pCPSlot->cbCodeapgeToUCS2	= _rxcp_ucs2_ConvertCodepageToUCS2;

	pCPSlot->stCPTagSlot.eCodepage = eRxCP_UNICODE_UCS2;
	pCPSlot->ucEndCharBytes	= 2;

	pCPSlot->stCPTagSlot.szCPTag[0] = RxCPTAG_UCS2;
	pCPSlot->stCPTagSlot.szCPTag[1] = 0;

	return ERR_OK;
}

STATIC	HERROR			_rxcp_dvbucs2_getinfo(RxCPSlot_t *pCPSlot)
{
	if(pCPSlot == NULL)
	{
		return ERR_FAIL;
	}

	pCPSlot->cbNumOfBytes	= _rxcp_ucs2_NumOfBytes;
	pCPSlot->cbNumOfChars	= _rxcp_ucs2_NumOfChars;
	pCPSlot->cbGetCharAt	= _rxcp_ucs2_GetCharAt;

	pCPSlot->cbUCS2ToCodepage	= _rxcp_ucs2_ConvertUCS2ToCodepage;
	pCPSlot->cbCodeapgeToUCS2	= _rxcp_ucs2_ConvertCodepageToUCS2;

	pCPSlot->stCPTagSlot.eCodepage = eRxCP_UNICODE_DVB_UCS2;
	pCPSlot->ucEndCharBytes	= 2;

	pCPSlot->stCPTagSlot.szCPTag[0] = RxCPTAG_UCS2;
	pCPSlot->stCPTagSlot.szCPTag[1] = 0;

	return ERR_OK;
}


/* *************************************************************************************************** */
#define ___PUBLIC_FUNCTION_______________________________________________________________________________
/* *************************************************************************************************** */


HERROR	INT_MHCp_UCS2(void)
{
	HERROR		hErr = ERR_FAIL;
	RxCPSlot_t	stCPSlot;

	RxSTD_MemSet(&stCPSlot, 0, sizeof(RxCPSlot_t));
	hErr = _rxcp_ucs2_GeIinfo(&stCPSlot);
	if(hErr == ERR_OK)
	{
		hErr = RxCP_AddCodepage(&stCPSlot);
	}

	RxSTD_MemSet(&stCPSlot, 0, sizeof(RxCPSlot_t));
	hErr = _rxcp_dvbucs2_getinfo(&stCPSlot);
	if(hErr == ERR_OK)
	{
		hErr = RxCP_AddCodepage(&stCPSlot);
	}

	return ERR_OK;
}

STATIC	RxUCS2Table_t *_rxcp_ucs2_GetCodeTable(void)
{
	return s_stUCS2CodeTable;
}

RxCT_e	INT_MHCp_GetUCS2CodeTable(HUINT16 ucUCS2)
{
	HINT32			i = 0;
	HINT32			nMid = 0;
	HINT32			nDataCnt = eRxCT_Max - 1;
	RxUCS2Table_t 	*pstCodeTable = _rxcp_ucs2_GetCodeTable();

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

