/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

/* global header file */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <mwc_codepage.h>

#include <octo_common.h>

/* local header file */
#include <isosvc.h>


/*******************************************************************/
/********************      Debug     ********************/
/*******************************************************************/




/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/



/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/


STATIC CHARMAPITEM_t ISO_8859_PAGE00_TO_UNICODE[] =
{
	{0x20,	0x0020},	//SPACE
	{0x21,	0x0021},	//EXCLAMATION MARK
	{0x22,	0x0022},	//QUOTATION MARK
	{0x23,	0x0023},	//NUMBER SIGN
	{0x24,	0x0024},	//DOLLAR SIGN
	{0x25,	0x0025},	//PERCENT SIGN
	{0x26,	0x0026},	//AMPERSAND
	{0x27,	0x0027},	//APOSTROPHE
	{0x28,	0x0028},	//LEFT PARENTHESIS
	{0x29,	0x0029},	//RIGHT PARENTHESIS
	{0x2A,	0x002A},	//ASTERISK
	{0x2B,	0x002B},	//PLUS SIGN
	{0x2C,	0x002C},	//COMMA
	{0x2D,	0x002D},	//HYPHEN-MINUS
	{0x2E,	0x002E},	//FULL STOP
	{0x2F,	0x002F},	//SOLIDUS
	{0x30,	0x0030},	//DIGIT ZERO
	{0x31,	0x0031},	//DIGIT ONE
	{0x32,	0x0032},	//DIGIT TWO
	{0x33,	0x0033},	//DIGIT THREE
	{0x34,	0x0034},	//DIGIT FOUR
	{0x35,	0x0035},	//DIGIT FIVE
	{0x36,	0x0036},	//DIGIT SIX
	{0x37,	0x0037},	//DIGIT SEVEN
	{0x38,	0x0038},	//DIGIT EIGHT
	{0x39,	0x0039},	//DIGIT NINE
	{0x3A,	0x003A},	//COLON
	{0x3B,	0x003B},	//SEMICOLON
	{0x3C,	0x003C},	//LESS-THAN SIGN
	{0x3D,	0x003D},	//EQUALS SIGN
	{0x3E,	0x003E},	//GREATER-THAN SIGN
	{0x3F,	0x003F},	//QUESTION MARK
	{0x40,	0x0040},	//COMMERCIAL AT
	{0x41,	0x0041},	//LATIN CAPITAL LETTER A
	{0x42,	0x0042},	//LATIN CAPITAL LETTER B
	{0x43,	0x0043},	//LATIN CAPITAL LETTER C
	{0x44,	0x0044},	//LATIN CAPITAL LETTER D
	{0x45,	0x0045},	//LATIN CAPITAL LETTER E
	{0x46,	0x0046},	//LATIN CAPITAL LETTER F
	{0x47,	0x0047},	//LATIN CAPITAL LETTER G
	{0x48,	0x0048},	//LATIN CAPITAL LETTER H
	{0x49,	0x0049},	//LATIN CAPITAL LETTER I
	{0x4A,	0x004A},	//LATIN CAPITAL LETTER J
	{0x4B,	0x004B},	//LATIN CAPITAL LETTER K
	{0x4C,	0x004C},	//LATIN CAPITAL LETTER L
	{0x4D,	0x004D},	//LATIN CAPITAL LETTER M
	{0x4E,	0x004E},	//LATIN CAPITAL LETTER N
	{0x4F,	0x004F},	//LATIN CAPITAL LETTER O
	{0x50,	0x0050},	//LATIN CAPITAL LETTER P
	{0x51,	0x0051},	//LATIN CAPITAL LETTER Q
	{0x52,	0x0052},	//LATIN CAPITAL LETTER R
	{0x53,	0x0053},	//LATIN CAPITAL LETTER S
	{0x54,	0x0054},	//LATIN CAPITAL LETTER T
	{0x55,	0x0055},	//LATIN CAPITAL LETTER U
	{0x56,	0x0056},	//LATIN CAPITAL LETTER V
	{0x57,	0x0057},	//LATIN CAPITAL LETTER W
	{0x58,	0x0058},	//LATIN CAPITAL LETTER X
	{0x59,	0x0059},	//LATIN CAPITAL LETTER Y
	{0x5A,	0x005A},	//LATIN CAPITAL LETTER Z
	{0x5B,	0x005B},	//LEFT SQUARE BRACKET
	{0x5C,	0x005C},	//REVERSE SOLIDUS
	{0x5D,	0x005D},	//RIGHT SQUARE BRACKET
	{0x5E,	0x005E},	//CIRCUMFLEX ACCENT
	{0x5F,	0x005F},	//LOW LINE
	{0x60,	0x0060},	//GRAVE ACCENT
	{0x61,	0x0061},	//LATIN SMALL LETTER A
	{0x62,	0x0062},	//LATIN SMALL LETTER B
	{0x63,	0x0063},	//LATIN SMALL LETTER C
	{0x64,	0x0064},	//LATIN SMALL LETTER D
	{0x65,	0x0065},	//LATIN SMALL LETTER E
	{0x66,	0x0066},	//LATIN SMALL LETTER F
	{0x67,	0x0067},	//LATIN SMALL LETTER G
	{0x68,	0x0068},	//LATIN SMALL LETTER H
	{0x69,	0x0069},	//LATIN SMALL LETTER I
	{0x6A,	0x006A},	//LATIN SMALL LETTER J
	{0x6B,	0x006B},	//LATIN SMALL LETTER K
	{0x6C,	0x006C},	//LATIN SMALL LETTER L
	{0x6D,	0x006D},	//LATIN SMALL LETTER M
	{0x6E,	0x006E},	//LATIN SMALL LETTER N
	{0x6F,	0x006F},	//LATIN SMALL LETTER O
	{0x70,	0x0070},	//LATIN SMALL LETTER P
	{0x71,	0x0071},	//LATIN SMALL LETTER Q
	{0x72,	0x0072},	//LATIN SMALL LETTER R
	{0x73,	0x0073},	//LATIN SMALL LETTER S
	{0x74,	0x0074},	//LATIN SMALL LETTER T
	{0x75,	0x0075},	//LATIN SMALL LETTER U
	{0x76,	0x0076},	//LATIN SMALL LETTER V
	{0x77,	0x0077},	//LATIN SMALL LETTER W
	{0x78,	0x0078},	//LATIN SMALL LETTER X
	{0x79,	0x0079},	//LATIN SMALL LETTER Y
	{0x7A,	0x007A},	//LATIN SMALL LETTER Z
	{0x7B,	0x007B},	//LEFT CURLY BRACKET
	{0x7C,	0x007C},	//VERTICAL LINE
	{0x7D,	0x007D},	//RIGHT CURLY BRACKET
	{0x7E,	0x007E},	//TILDE
	{0x7F,	0x007F},		//DELETE
};

#define __________________________CODEPAGE_TO_UCS2__________________________________________


STATIC HERROR	local_mw_cp_ConvertAsciiToUCS2(const CH_ASCII * pAsciiStr, const HINT32 nAsciiStrLen, HINT32 *pnUsedByte, CH_UCS2 *ucs2Str, HINT32 *pnUcs2StrLen)
{
	HUINT16		i=0;
	HUINT16		convertedCharacterCount=0;
	ConvEncoding_Err_t	resultErr=ERR_FAIL;


	if(nAsciiStrLen==0)
	{
		/* check number to converte */
		*pnUcs2StrLen=0;

		return ERR_OK;
	}

	if(pAsciiStr==NULL||ucs2Str==NULL)
	{
		/* pAsciiStr storage check */
		*pnUcs2StrLen=0;

		return ERR_FAIL;
	}

	/* ascii encoding ���� �� dvb ��Ʈ�� ���̿� �Ǿ��ڸ� ��Ʈ�� ����Ʈ�� white space�� ���� �Ǿ��� ���õȴ�.  */
	for(i=0; i<nAsciiStrLen; i++)
	{

		if(pAsciiStr[i]==0x00)
		{
			resultErr=ERR_NULL_CH;

			break;
		}
		else if(pAsciiStr[i]<0x20)
		{
			/* its white space character */
			resultErr=ERR_WHITE_CH;
		}
		else if(pAsciiStr[i]>0x7f)
		{
			/* out of ascii code page */
			resultErr=ERR_OUTOF_CODEPAGE;

			continue;
		}
		else	/* ~ 0x7f */
		{
			/* ascii code */
			resultErr=ERR_OK;
		}

		ucs2Str[convertedCharacterCount++]=0x0000|pAsciiStr[i];
	}

	*pnUsedByte=i;
	*pnUcs2StrLen=convertedCharacterCount;


	return resultErr;
}

#define __________________________UCS2_TO_CODEPAGE___________________________________________
STATIC HERROR	local_mw_cp_ConvertUCS2ToAscii(const CH_UCS2 			*pSrcUCS2Str,
												const HINT32 			nSrcUCS2StrLen,
												const HINT32			nBufferLen,
												HINT32 					*pnUsedByte,
												CH_1BYTE 				*pDest8859Str,
												HINT32 					*pnDest8859StrLen
												)
{
	HINT32			iter = 0;
	HINT32			nNumOfConvertedCh = 0;
	HERROR			hResErr = ERR_OK;

	HxLOG_Assert(pSrcUCS2Str);
	HxLOG_Assert(pDest8859Str);


	nNumOfConvertedCh = 1;
	pDest8859Str[0] = eHxCP_ISO_ASCII;


	for(iter = 0; iter < nSrcUCS2StrLen; iter++)
	{
		if(pSrcUCS2Str[iter] >= 0x00A0)
		{
			continue;
		}
		else
		{
			pDest8859Str[nNumOfConvertedCh] = (pSrcUCS2Str[iter] & 0x00ff);
			nNumOfConvertedCh++;
			(*pnUsedByte)++;
		}

		if(nBufferLen <= *pnUsedByte)
		{
			break;
		}
	}

	*pnDest8859StrLen = nNumOfConvertedCh;


	return hResErr;

}



#define __________________________Drawable___________________________________________

STATIC HERROR local_mw_cp_CountDrawableCharNumSingleByte (HUINT8 *szString, HUINT32 *pulCharNum)
{
	*pulCharNum = MWC_UTIL_DvbStrlen (szString);
	return ERR_OK;
}

STATIC HERROR local_mw_cp_GetDrawableCharSingleByte (HUINT8 *szString, HUINT32 ulIndex,
													   HUINT8 *szCharWithCode, HUINT32 *pulCharLenWithCode,
													   HUINT8 *szCharOnly, HUINT32 *pulCharLenOnly)
{
	HUINT8		*szPos;

	szPos = szString;

	if (MWC_UTIL_DvbStrlen (szString) <= ulIndex)			{ return ERR_FAIL; }

	if (szCharWithCode != NULL)
	{
		szCharWithCode[0] = szString[ulIndex];
	}

	if (pulCharLenWithCode != NULL)
	{
		*pulCharLenWithCode = 1;
	}

	if (szCharOnly != NULL)
	{
		szCharOnly[0] = szString[ulIndex];
	}

	if (pulCharLenOnly != NULL)
	{
		*pulCharLenOnly = 1;
	}

	return ERR_OK;
}


#define _________________________EmphasizedString________________________________________


#define _________________________PUBLIC_FUNCTION__________________________________

HERROR MWC_UTIL_CodePage_RegisterASCII(void)
{
	MWC_UTIL_CodePage_Register(eHxCP_ISO_ASCII,
		local_mw_cp_CountDrawableCharNumSingleByte, local_mw_cp_GetDrawableCharSingleByte,
		local_mw_cp_ConvertAsciiToUCS2,
		local_mw_cp_ConvertUCS2ToAscii,
		NULL,
		ISO_8859_PAGE00_TO_UNICODE);

#if 0
	MWC_UTIL_CodePage_Register(eHxCP_ISO_SINGLEBYTE,
		local_mw_cp_CountDrawableCharNumSingleByte, local_mw_cp_GetDrawableCharSingleByte,
		NULL,	// single byte�� Ư�� codepage�� �ƴϴ�.
		NULL,
		NULL,
		ISO_8859_PAGE00_TO_UNICODE);
#endif

	return ERR_OK;
}
