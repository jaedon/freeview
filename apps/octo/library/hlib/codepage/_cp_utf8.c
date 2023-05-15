
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


/* *************************************************************************************************** */
#define ___STATIC_VARIABLES______________________________________________________________________________
/* *************************************************************************************************** */



/* *************************************************************************************************** */
#define ___Private_FUNCTION______________________________________________________________________________
/* *************************************************************************************************** */
STATIC 	HUINT8			*_hcodepage_utf8_ToUCS2(HUINT16 *pDestStr, HUINT8 *pSrcUTF8, HINT32	nConvCharCnt)
{
	HUINT8 	cDvbCh = *pSrcUTF8;


	/* 0x01 ~ 0x7F */
	if(cDvbCh < 0x80)
	{
		pDestStr[nConvCharCnt] = cDvbCh;

		pSrcUTF8++;
	}
	/* 0x20 ~ 0xC1 */
	else if(cDvbCh < 0xc2)
	{
		//is utf8??
		pSrcUTF8++;
	}
	else if(cDvbCh < 0xe0)
	{
		if (!((pSrcUTF8[1] ^ 0x80) < 0x40))
		{
			//is utf8??
			pSrcUTF8++;
		}
		else
		{
			pDestStr[nConvCharCnt]  = ((HUINT16) (cDvbCh & 0x1f) << 6) | (HUINT16) (pSrcUTF8[1] ^ 0x80);
			pSrcUTF8 += 2;
		}
	}
	else if(cDvbCh < 0xf0)
	{
		if (!((pSrcUTF8[1] ^ 0x80) < 0x40 && (pSrcUTF8[2] ^ 0x80) < 0x40 && (cDvbCh >= 0xe1 || pSrcUTF8[1] >= 0xa0)))
		{
			//is utf8??
			pSrcUTF8++;
		}
		else
		{
			pDestStr[nConvCharCnt] = ((HUINT16) (cDvbCh & 0x0f) << 12) | ((HUINT16) (pSrcUTF8[1] ^ 0x80) << 6) | (HUINT16) (pSrcUTF8[2] ^ 0x80);
			pSrcUTF8 += 3;
		}
	}
	else
	{
		//is utf8??
		pSrcUTF8++;
	}

	return pSrcUTF8;
}


STATIC 	HUINT8			*_hcodepage_utf8_ToUCS4(HUINT32 *pDestStr, HUINT8 *pSrcUTF8, HINT32	nConvCharCnt)
{
	HUINT8 	cDvbCh = *pSrcUTF8;

	/* 0x01 ~ 0x7F */
	if(cDvbCh < 0x80)
	{
		pDestStr[nConvCharCnt] = cDvbCh;
		pSrcUTF8++;
	}
	/* 0x20 ~ 0xC1 */
	else if(cDvbCh < 0xc2)
	{
		//is utf8??
		pSrcUTF8++;
		return pSrcUTF8;
	}
	else if(cDvbCh < 0xe0)
	{
		if (!((pSrcUTF8[1] ^ 0x80) < 0x40))
		{
			//is utf8??
			pSrcUTF8++;
		}
		else
		{
			pDestStr[nConvCharCnt]  = ((HUINT32) (cDvbCh & 0x1f) << 6) | (HUINT32) (pSrcUTF8[1] ^ 0x80);
			pSrcUTF8 += 2;
		}
	}
	else if(cDvbCh < 0xf0)
	{
		if (!((pSrcUTF8[1] ^ 0x80) < 0x40 && (pSrcUTF8[2] ^ 0x80) < 0x40 && (cDvbCh >= 0xe1 || pSrcUTF8[1] >= 0xa0)))
		{
			//is utf8??
			pSrcUTF8++;
		}
		else
		{
			pDestStr[nConvCharCnt] = ((HUINT32) (cDvbCh & 0x0f) << 12) | ((HUINT32) (pSrcUTF8[1] ^ 0x80) << 6) | (HUINT32) (pSrcUTF8[2] ^ 0x80);
			pSrcUTF8 += 3;
		}
	}
	else if(cDvbCh < 0xf8)
	{
		if (!((pSrcUTF8[1] ^ 0x80) < 0x40 && (pSrcUTF8[2] ^ 0x80) < 0x40
			&& (pSrcUTF8[3] ^ 0x80) < 0x40
			&& (cDvbCh >= 0xf1 || pSrcUTF8[1] >= 0x90)))
		{
			//is utf8??
			pSrcUTF8++;
		}
		else
		{
			pDestStr[nConvCharCnt] = ((HUINT32) (cDvbCh & 0x07) << 18) | ((HUINT32) (pSrcUTF8[1] ^ 0x80) << 12) | ((HUINT32) (pSrcUTF8[2] ^ 0x80) << 6) | (HUINT32) (pSrcUTF8[3] ^ 0x80);
			pSrcUTF8 += 4;
		}
	}
	else if(cDvbCh < 0xfc)
	{
		if (!((pSrcUTF8[1] ^ 0x80) < 0x40 && (pSrcUTF8[2] ^ 0x80) < 0x40
			&& (pSrcUTF8[3] ^ 0x80) < 0x40 && (pSrcUTF8[4] ^ 0x80) < 0x40
			&& (cDvbCh >= 0xf9 || pSrcUTF8[1] >= 0x88)))
		{
			//is utf8??
			pSrcUTF8++;
		}
		else
		{
			pDestStr[nConvCharCnt] = ((HUINT32) (cDvbCh & 0x03) << 24) | ((HUINT32) (pSrcUTF8[1] ^ 0x80) << 18) | ((HUINT32) (pSrcUTF8[2] ^ 0x80) << 12) | ((HUINT32) (pSrcUTF8[3] ^ 0x80) << 6) | (HUINT32) (pSrcUTF8[4] ^ 0x80);
			pSrcUTF8 += 5;
		}
	}
	else if(cDvbCh < 0xfe)
	{
		if (!((pSrcUTF8[1] ^ 0x80) < 0x40 && (pSrcUTF8[2] ^ 0x80) < 0x40
			&& (pSrcUTF8[3] ^ 0x80) < 0x40 && (pSrcUTF8[4] ^ 0x80) < 0x40
			&& (pSrcUTF8[5] ^ 0x80) < 0x40
			&& (cDvbCh >= 0xfd || pSrcUTF8[1] >= 0x84)))
		{
			//is utf8??
			pSrcUTF8++;
		}
		else
		{
			pDestStr[nConvCharCnt] = ((HUINT32) (cDvbCh & 0x01) << 30)  | ((HUINT32) (pSrcUTF8[1] ^ 0x80) << 24)	| ((HUINT32) (pSrcUTF8[2] ^ 0x80) << 18)  | ((HUINT32) (pSrcUTF8[3] ^ 0x80) << 12)	| ((HUINT32) (pSrcUTF8[4] ^ 0x80) << 6)  | (HUINT32) (pSrcUTF8[5] ^ 0x80);
			pSrcUTF8 += 6;
		}
	}
	else
	{
		//is utf8??
		pSrcUTF8++;
	}

	return pSrcUTF8;
}



STATIC	HINT32			_hcodepage_utf8_GetCountOneCharByte(HUINT8 chChar)
{
	if (chChar < 0x80)
	{
		return 1;
	}
	else if (chChar < 0xc2)
	{
		// is utf8 ??
		return 1;
	}
	else if (chChar < 0xe0)
	{
		return 2;
	}
	else if (chChar < 0xf0)
	{
		return 3;
	}
	else if (chChar < 0xf8)
	{
		return 4;
	}
	else if (chChar < 0xfc)
	{
		return 5;
	}
	else if (chChar < 0xfe)
	{
		return 6;
	}

	return 0;
}

STATIC	HINT32			_hcodepage_utf8_ChangeUCS4toUTF8( HUINT32 ulUCS2Char, HUINT8 *pszStr)
{
	HINT32 nOneCharCnt = 0;

	if (ulUCS2Char < 0x80)				nOneCharCnt = 1;
	else if (ulUCS2Char < 0x800)		nOneCharCnt = 2;
	else if (ulUCS2Char < 0x10000)		nOneCharCnt = 3;
	else if (ulUCS2Char < 0x200000)		nOneCharCnt = 4;
	else if (ulUCS2Char < 0x4000000)	nOneCharCnt = 5;
	else if (ulUCS2Char <= 0x7fffffff)	nOneCharCnt = 6;
	else
		return 0;

	switch(nOneCharCnt)
	{
	case 6: pszStr[5] = 0x80 | (ulUCS2Char & 0x3f); ulUCS2Char = ulUCS2Char >> 6; ulUCS2Char |= 0x4000000;
	case 5: pszStr[4] = 0x80 | (ulUCS2Char & 0x3f); ulUCS2Char = ulUCS2Char >> 6; ulUCS2Char |= 0x200000;
	case 4: pszStr[3] = 0x80 | (ulUCS2Char & 0x3f); ulUCS2Char = ulUCS2Char >> 6; ulUCS2Char |= 0x10000;
	case 3: pszStr[2] = 0x80 | (ulUCS2Char & 0x3f); ulUCS2Char = ulUCS2Char >> 6; ulUCS2Char |= 0x800;
	case 2: pszStr[1] = 0x80 | (ulUCS2Char & 0x3f); ulUCS2Char = ulUCS2Char >> 6; ulUCS2Char |= 0xc0;
	case 1: pszStr[0] = ulUCS2Char & 0xff;

	default:
		break;
	}

	return nOneCharCnt;
}


STATIC	HINT32			_hcodepage_utf8_ChangeUCS2toUTF8( HUINT16 usUCS2Char, HUINT8 *pszStr)
{
	HINT32 nOneCharCnt = 0;

	/**
	 * @see ETSI EN 300 468 Annex A, A.1 Control codes
 	*/
	if (0xE080 <= usUCS2Char && usUCS2Char <= 0xE09F)
	{
		switch (usUCS2Char)
		{
		case UCS2_CR_LF:
			pszStr[0] = '\n';
			return 1;
			break;

		case UCS2_EMPHASIS_ON:		// character emphasis on
		case UCS2_EMPHASIS_OFF: 	// character emphasis off
		default:					// reserved for future use
			pszStr[0] = 0xEE;
			pszStr[1] = 0x80;
			pszStr[2] = (usUCS2Char & 0xFF);
			pszStr[3] = '\0';
			break;
		}
		return 0;
	}


	if (usUCS2Char < 0x80)				nOneCharCnt = 1;
	else if (usUCS2Char < 0x800)		nOneCharCnt = 2;
	else								nOneCharCnt = 3;

	switch(nOneCharCnt)
	{
	case 3:
		pszStr[0] = ((usUCS2Char >> 12) ) | 0xE0;
		pszStr[1] = ((usUCS2Char >> 6) & 0x3F) | 0x80;
		pszStr[2] = (usUCS2Char & 0x3f) | 0x80;
		pszStr[3] = '\0';
		break;
	case 2:
		pszStr[0] = (usUCS2Char >> 6) | 0xC0;
		pszStr[1] = (usUCS2Char & 0x3f) | 0x80;
		pszStr[2] = '\0';
		break;

	case 1:
	default:
		pszStr[0] = usUCS2Char;
		pszStr[1] = '\0';
		break;
	}

	return nOneCharCnt;
}


STATIC	HINT32			_hcodepage_dvbutf8_ChangeUCS2toUTF8( HUINT16 usUCS2Char, HUINT8 *pszStr)
{
	HINT32 nOneCharCnt = 0;

	/**
	 * @see ETSI EN 300 468 Annex A, A.1 Control codes
	 */
	if (0xE080 <= usUCS2Char && usUCS2Char <= 0xE09F)
	{
		switch (usUCS2Char)
		{
		case UCS2_CR_LF:
			pszStr[0] = '\n';
			return 1;
			break;

		case UCS2_EMPHASIS_ON:		// character emphasis on
		case UCS2_EMPHASIS_OFF:		// character emphasis off
		default:					// reserved for future use
			pszStr[0] = 0xEE;
			pszStr[1] = 0x80;
			pszStr[2] = (usUCS2Char & 0xFF);
			pszStr[3] = '\0';
			break;
		}
		return 0;
	}

	if (usUCS2Char < 0x80)				nOneCharCnt = 1;
	else if (usUCS2Char < 0x800)		nOneCharCnt = 2;
	else								nOneCharCnt = 3;

	switch(nOneCharCnt)
	{
	case 3:
		pszStr[0] = ((usUCS2Char >> 12) ) | 0xE0;
		pszStr[1] = ((usUCS2Char >> 6) & 0x3F) | 0x80;
		pszStr[2] = (usUCS2Char & 0x3f) | 0x80;
		pszStr[3] = '\0';
		break;
	case 2:
		pszStr[0] = (usUCS2Char >> 6) | 0xC0;
		pszStr[1] = (usUCS2Char & 0x3f) | 0x80;
		pszStr[2] = '\0';
		break;

	case 1:
	default:
		pszStr[0] = usUCS2Char;
		pszStr[1] = '\0';
		break;
	}

	return nOneCharCnt;
}




STATIC	HINT32			_hcodepage_utf8_NumOfBytes(const void 	*pSrc)
{
	return strlen((const char*)pSrc);
}

STATIC	HINT32			_hcodepage_utf8_NumOfChars(const void	*pSrc)
{
	HINT32	nNumOfBytes = 0;
	HINT32 	nNumOfChars = 0;
	HINT32 	nLoop = 0;
	HINT32 	nTempChar = 0;
	HUINT8	*pszStr = (HUINT8 *)pSrc;

	nNumOfBytes = _hcodepage_utf8_NumOfBytes(pSrc);

	for(nLoop=0; nLoop < nNumOfBytes && pszStr[nLoop] != 0; )
	{
		nTempChar = _hcodepage_utf8_GetCountOneCharByte(pszStr[nLoop]);
		if(nTempChar == 0)
		{
			nLoop++;
			continue;
		}

		nNumOfChars++;
		nLoop+=nTempChar;
	}

	return nNumOfChars;
}

STATIC	HINT32			_hcodepage_utf8_GetCharAt(const void 	*pSrc, const HINT32 nCharIdx,  void *pCharBuf)
{
	HINT32	nOneCharByte 	= 0;
	HUINT8	*pszStr 		= (HUINT8 *)pSrc;
	HINT32	nCharNum		= nCharIdx;

	while(*pszStr && nCharNum)
	{
		nOneCharByte = _hcodepage_utf8_GetCountOneCharByte(*pszStr);
		if(nOneCharByte)
		{
			pszStr+=nOneCharByte;
		}
		else
		{
			pszStr++;
		}
		nCharNum--;
	}

	if(*pszStr)
	{
		nOneCharByte = _hcodepage_utf8_GetCountOneCharByte(*pszStr);
		if(pCharBuf)
		{
			HxSTD_MemCopy(pCharBuf, pszStr, nOneCharByte);
		}
	}

	return nOneCharByte;
}

STATIC	HINT32 			_hcodepage_utf8Common_ConvertUCS2ToCodepage(const HUINT16 	*pSrcUCS2Str,
																		void 	*pDestStr,
																		HINT32 	nDestStrNumOfBytes,
																		HINT32	(*ChangeUCS2toUTF8)( HUINT16, HUINT8 *))
{
	HINT32		nLoop = 0;
	HINT32 		nSrcChars = 0;
	HINT32		nConvCharByte = 0;
	HINT32		nOneCharCnt = 0;
	HUINT8		szBuf[8] = {0, };
	HUINT8		*pDestString = (HUINT8	*)pDestStr;

	nSrcChars = HLIB_CP_NumOfChars(eHxCP_UNICODE_UCS2, pSrcUCS2Str);
	if(HxCP_IsValidReturnValue(nSrcChars) == FALSE)
	{
		return eHxCP_ERROR_FAIL;
	}

	nDestStrNumOfBytes -= sizeof(HUINT8);

	for(nLoop=0; nLoop < nSrcChars; nLoop++)
	{
		nOneCharCnt = ChangeUCS2toUTF8(pSrcUCS2Str[nLoop], szBuf);
		if(nConvCharByte + nOneCharCnt > nDestStrNumOfBytes)
		{
			break;
		}
		HxSTD_MemCopy(pDestString+nConvCharByte, szBuf, nOneCharCnt);
		nConvCharByte += nOneCharCnt;
	}
	pDestString[nConvCharByte] = 0;

	return nConvCharByte;
}


STATIC	HINT32 			_hcodepage_utf8Common_ConvertCodepageToUCS2(const 	void 		*pSrcStr,
																			HUINT16 	*pDestStr,
																			HINT32 		nDestStrNumOfBytes,
																			HUINT8*		(*cbUtf8ToUCS2)(HUINT16 *, HUINT8 *, HINT32	))
{
	HINT32	nConvCharCnt = 0;
	HUINT8	*pSrcUTF8 = (HUINT8	*)pSrcStr;

	HxLOG_Assert(pSrcUTF8);

	HxSTD_MemSet(pDestStr, 0, nDestStrNumOfBytes);

	nDestStrNumOfBytes -= sizeof(HUINT16);
	nDestStrNumOfBytes /= sizeof(HUINT16);

	while(*pSrcUTF8 && nConvCharCnt < nDestStrNumOfBytes)
	{
		pSrcUTF8 = cbUtf8ToUCS2(pDestStr, pSrcUTF8, nConvCharCnt);
		if(pSrcUTF8 == NULL)
		{
			break;
		}
		if(pDestStr[nConvCharCnt])
		{
			nConvCharCnt++;
		}
	}
	pDestStr[nConvCharCnt] = 0x0000;

	return (nConvCharCnt*sizeof(HUINT16));
}

STATIC	HINT32 			_hcodepage_utf8_ConvertCodepageToUCS2(const void 	*pSrcStr, HUINT16 	*pDestStr, HINT32 nDestStrNumOfBytes)
{
	return _hcodepage_utf8Common_ConvertCodepageToUCS2(pSrcStr, pDestStr, nDestStrNumOfBytes, _hcodepage_utf8_ToUCS2);
}

STATIC	HINT32 			_hcodepage_dvbutf8_ConvertCodepageToUCS2(const void 	*pSrcStr, HUINT16 	*pDestStr, HINT32 nDestStrNumOfBytes)
{
	return _hcodepage_utf8Common_ConvertCodepageToUCS2(pSrcStr, pDestStr, nDestStrNumOfBytes, _hcodepage_utf8_ToUCS2);
}

STATIC	HINT32 			_hcodepage_emphasisutf8_ConvertCodepageToUCS2(const void 	*pSrcStr, HUINT16 	*pDestStr, HINT32 nDestStrNumOfBytes)
{
	return _hcodepage_utf8Common_ConvertCodepageToUCS2(pSrcStr, pDestStr, nDestStrNumOfBytes, _hcodepage_utf8_ToUCS2);
}


STATIC	HINT32 			_hcodepage_dvbutf8_ConvertUCS2ToCodepage(const HUINT16 	*pSrcUCS2Str, void 	*pDestStr, HINT32 nDestStrNumOfBytes)
{
	return _hcodepage_utf8Common_ConvertUCS2ToCodepage(pSrcUCS2Str, pDestStr, nDestStrNumOfBytes, _hcodepage_dvbutf8_ChangeUCS2toUTF8);
}

STATIC	HINT32 			_hcodepage_utf8_ConvertUCS2ToCodepage(const HUINT16 	*pSrcUCS2Str, void 	*pDestStr, HINT32 nDestStrNumOfBytes)
{
	return _hcodepage_utf8Common_ConvertUCS2ToCodepage(pSrcUCS2Str, pDestStr, nDestStrNumOfBytes, _hcodepage_utf8_ChangeUCS2toUTF8);
}

STATIC	HINT32 			_hcodepage_emphasisutf8_ConvertUCS2ToCodepage(const HUINT16 	*pSrcUCS2Str,
																		void 	*pDestStr,
																		HINT32 	nDestStrNumOfBytes,
																		HINT32	(*ChangeUCS2toUTF8)( HUINT16, HUINT8 *))
{
	HBOOL		bStartMakeEmphasizedCode = FALSE;
	HBOOL		bEndMakeEmphasizedCode = FALSE;
	HBOOL		bEmphasis = FALSE;
	HINT32		nLoop = 0;
	HINT32 		nSrcChars = 0;
	HINT32		nConvCharByte = 0;
	HINT32		nOneCharCnt = 0;
	HUINT8		szBuf[8] = {0, };
	HUINT8		*pDestString = (HUINT8	*)pDestStr;

	nSrcChars = HLIB_CP_NumOfChars(eHxCP_UNICODE_UCS2, pSrcUCS2Str);
	if(HxCP_IsValidReturnValue(nSrcChars) == FALSE)
	{
		return eHxCP_ERROR_FAIL;
	}

	nDestStrNumOfBytes -= sizeof(HUINT8);

	for(nLoop=0; nLoop < nSrcChars; nLoop++)
	{
		if(FALSE == bEmphasis)
		{
			if( UCS2_EMPHASIS_ON == pSrcUCS2Str[nLoop])
			{
				bEmphasis = TRUE;
				bStartMakeEmphasizedCode = TRUE;
			}
		}
		else
		{
			if( UCS2_EMPHASIS_OFF == pSrcUCS2Str[nLoop])
			{
				bEmphasis = FALSE;
				bEndMakeEmphasizedCode = TRUE;
				continue;
			}

			nOneCharCnt = ChangeUCS2toUTF8(pSrcUCS2Str[nLoop], szBuf);
			if(nConvCharByte + nOneCharCnt > nDestStrNumOfBytes)
			{
				break;
			}
			HxSTD_MemCopy(pDestString+nConvCharByte, szBuf, nOneCharCnt);
			nConvCharByte += nOneCharCnt;
		}

	}
	pDestString[nConvCharByte] = 0;

	return nConvCharByte;
}


STATIC	HERROR			_hcodepage_utf8_GetInfo(eHxCP_Slot_t *pCPSlot)
{
	if(pCPSlot == NULL)
	{
		return ERR_FAIL;
	}

	pCPSlot->cbNumOfBytes	= _hcodepage_utf8_NumOfBytes;
	pCPSlot->cbNumOfChars	= _hcodepage_utf8_NumOfChars;
	pCPSlot->cbGetCharAt	= _hcodepage_utf8_GetCharAt;

	pCPSlot->cbUCS2ToCodepage	= _hcodepage_utf8_ConvertUCS2ToCodepage;
	pCPSlot->cbCodeapgeToUCS2	= _hcodepage_utf8_ConvertCodepageToUCS2;

	pCPSlot->stCPTagSlot.eCodepage = eHxCP_UNICODE_UTF8;
	pCPSlot->stCPTagSlot.nCPTagLen = 1;
	pCPSlot->stCPTagSlot.szCPTag[0] = eHxCP_DVBLANG_VALUE_UTF_8;
	pCPSlot->stCPTagSlot.szCPTag[1] = 0;

#if defined(TEST_CP)
	pCPSlot->stCPTagSlot.szCPTag2[0] = 0;
	pCPSlot->stCPTagSlot.nCPTagLen2 = 0;
#endif


	pCPSlot->ucEndCharBytes	= 1;

	return ERR_OK;
}

STATIC	HERROR			_hcodepage_dvbutf8_GetInfo(eHxCP_Slot_t *pCPSlot)
{
	if(pCPSlot == NULL)
	{
		return ERR_FAIL;
	}

	pCPSlot->cbNumOfBytes	= _hcodepage_utf8_NumOfBytes;
	pCPSlot->cbNumOfChars	= _hcodepage_utf8_NumOfChars;
	pCPSlot->cbGetCharAt	= _hcodepage_utf8_GetCharAt;

	pCPSlot->cbUCS2ToCodepage	= _hcodepage_dvbutf8_ConvertUCS2ToCodepage;
	pCPSlot->cbCodeapgeToUCS2	= _hcodepage_dvbutf8_ConvertCodepageToUCS2;

	pCPSlot->stCPTagSlot.eCodepage = eHxCP_UNICODE_DVB_UTF8;
	pCPSlot->ucEndCharBytes	= 1;
	pCPSlot->stCPTagSlot.szCPTag[0] = eHxCP_DVBLANG_VALUE_UTF_8;
	pCPSlot->stCPTagSlot.szCPTag[1] = 0;

#if defined(TEST_CP)
	pCPSlot->stCPTagSlot.szCPTag2[0] = 0;
	pCPSlot->stCPTagSlot.nCPTagLen2 = 0;
#endif


	return ERR_OK;
}

STATIC	HERROR			_hcodepage_emphasisutf8_GetInfo(eHxCP_Slot_t *pCPSlot)
{
	if(pCPSlot == NULL)
	{
		return ERR_FAIL;
	}

	pCPSlot->cbNumOfBytes	= _hcodepage_utf8_NumOfBytes;
	pCPSlot->cbNumOfChars	= _hcodepage_utf8_NumOfChars;
	pCPSlot->cbGetCharAt	= _hcodepage_utf8_GetCharAt;

	pCPSlot->cbUCS2ToCodepage	= (ConvertUCS2ToCodepage)_hcodepage_emphasisutf8_ConvertUCS2ToCodepage;
	pCPSlot->cbCodeapgeToUCS2	= _hcodepage_emphasisutf8_ConvertCodepageToUCS2;

	pCPSlot->stCPTagSlot.eCodepage = eHxCP_UNICODE_DVB_EMPHASYS_UTF8;
	pCPSlot->ucEndCharBytes	= 1;
	pCPSlot->stCPTagSlot.szCPTag[0] = eHxCP_DVBLANG_VALUE_UTF_8;
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


HERROR	INT_HCodepage_UTF8(void)
{
	HERROR		hErr = ERR_FAIL;
	eHxCP_Slot_t	stCPSlot;

	HxSTD_MemSet(&stCPSlot, 0, sizeof(eHxCP_Slot_t));
	hErr = _hcodepage_utf8_GetInfo(&stCPSlot);
	if(hErr == ERR_OK)
	{
		hErr = HLIB_CP_AddCodepage(&stCPSlot);
	}

	HxSTD_MemSet(&stCPSlot, 0, sizeof(eHxCP_Slot_t));
	hErr = _hcodepage_dvbutf8_GetInfo(&stCPSlot);
	if(hErr == ERR_OK)
	{
		hErr = HLIB_CP_AddCodepage(&stCPSlot);
	}


	HxSTD_MemSet(&stCPSlot, 0, sizeof(eHxCP_Slot_t));
	hErr = _hcodepage_emphasisutf8_GetInfo(&stCPSlot);
	if(hErr == ERR_OK)
	{
		hErr = HLIB_CP_AddCodepage(&stCPSlot);
	}

	(void)_hcodepage_utf8_ToUCS4;
	(void)_hcodepage_utf8_ChangeUCS4toUTF8;
	return ERR_OK;
}


/*********************** End of File ******************************/

