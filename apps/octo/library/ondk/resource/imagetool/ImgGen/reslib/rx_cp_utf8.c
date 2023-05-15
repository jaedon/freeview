
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


/* *************************************************************************************************** */
#define ___STATIC_VARIABLES______________________________________________________________________________
/* *************************************************************************************************** */



/* *************************************************************************************************** */
#define ___Private_FUNCTION______________________________________________________________________________
/* *************************************************************************************************** */
STATIC 	HUINT8			*_rxcp_utf8_ToUCS2(HUINT16 *pDestStr, HUINT8 *pSrcUTF8, HINT32	nConvCharCnt)
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


STATIC 	HUINT8			*_rxcp_utf8_ToUCS4(HUINT32 *pDestStr, HUINT8 *pSrcUTF8, HINT32	nConvCharCnt)
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



STATIC	HINT32			_rxcp_utf8_GetCountOneCharByte(HUINT8 chChar)
{
	if (chChar < 0x80)
	{
		return 1;
	}
	else if (chChar < 0xc2)
	{
		// is utf8 ??
		return 0;
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

STATIC	HINT32			_rxcp_utf8_ChangeUCS4toUTF8( HUINT32 ulUCS2Char, HUINT8 *pszStr)
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


STATIC	HINT32			_rxcp_utf8_ChangeUCS2toUTF8( HUINT16 usUCS2Char, HUINT8 *pszStr)
{
	HINT32 nOneCharCnt = 0;

	if (usUCS2Char < 0x80)				nOneCharCnt = 1;
	else if (usUCS2Char < 0x800)		nOneCharCnt = 2;
	else								nOneCharCnt = 3;

	switch(nOneCharCnt)
	{
	case 3: pszStr[2] = 0x80 | (usUCS2Char & 0x3f); usUCS2Char = usUCS2Char >> 6; usUCS2Char |= 0x800;
	case 2: pszStr[1] = 0x80 | (usUCS2Char & 0x3f); usUCS2Char = usUCS2Char >> 6; usUCS2Char |= 0xc0;
	case 1: pszStr[0] = usUCS2Char & 0xff;

	default:
		break;
	}

	return nOneCharCnt;
}


STATIC	HINT32			_rxcp_utf8_NumOfBytes(const void 	*pSrc)
{
	return strlen((HUINT8 *)pSrc);
}

STATIC	HINT32			_rxcp_utf8_NumOfChars(const void	*pSrc)
{
	HINT32	nNumOfBytes = 0;
	HINT32 	nNumOfChars = 0;
	HINT32 	nLoop = 0;
	HINT32 	nTempChar = 0;
	HUINT8	*pszStr = (HUINT8 *)pSrc;

	nNumOfBytes = _rxcp_utf8_NumOfBytes(pSrc);

	for(nLoop=0; nLoop < nNumOfBytes && pszStr[nLoop] != 0; )
	{
		nTempChar = _rxcp_utf8_GetCountOneCharByte(pszStr[nLoop]);
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

STATIC	HINT32			_rxcp_utf8_GetCharAt(const void 	*pSrc, const HINT32 nCharIdx,  void *pCharBuf)
{
	HINT32	nOneCharByte 	= 0;
	HUINT8	*pszStr 		= (HUINT8 *)pSrc;
	HINT32	nCharNum		= nCharIdx;

	while(*pszStr && nCharNum)
	{
		nOneCharByte = _rxcp_utf8_GetCountOneCharByte(*pszStr);
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
		nOneCharByte = _rxcp_utf8_GetCountOneCharByte(*pszStr);
		RxSTD_MemCopy(pCharBuf, pszStr, nOneCharByte);
	}

	return nOneCharByte;
}

STATIC	HINT32 			_rxcp_utf8_ConvertUCS2ToCodepage(const HUINT16 	*pSrcUCS2Str, void 	*pDestStr, HINT32 nDestStrNumOfBytes)
{
	HINT32		nLoop = 0;
	HINT32 		nSrcChars = 0;
	HINT32		nConvCharByte = 0;
	HINT32		nOneCharCnt = 0;
	HUINT8		szBuf[8] = {0, };
	HUINT8		*pDestString = (HUINT8	*)pDestStr;

	nSrcChars = RxCP_NumOfChars(eRxCP_UNICODE_UCS2, pSrcUCS2Str);
	if(RxCP_IsValidReturnValue(nSrcChars) == FALSE)
	{
		return ERR_FAIL;
	}

	nDestStrNumOfBytes -= sizeof(HUINT8);

	for(nLoop=0; nLoop < nSrcChars; nLoop++)
	{
		nOneCharCnt = _rxcp_utf8_ChangeUCS2toUTF8(pSrcUCS2Str[nLoop], szBuf);
		if(nConvCharByte + nOneCharCnt > nDestStrNumOfBytes)
		{
			break;
		}
		RxSTD_MemCopy(pDestString+nConvCharByte, szBuf, nOneCharCnt);
		nConvCharByte += nOneCharCnt;
	}
	pDestString[nConvCharByte] = 0;

	return nConvCharByte;
}


STATIC	HINT32 			_rxcp_utf8_ConvertCodepageToUCS2(const void 	*pSrcStr, HUINT16 	*pDestStr, HINT32 nDestStrNumOfBytes)
{
	HINT32	nConvCharCnt = 0;
	HUINT8	*pSrcUTF8 = (HUINT8	*)pSrcStr;

	RxLOG_Assert(pSrcUTF8);

	RxSTD_MemSet(pDestStr, 0, nDestStrNumOfBytes);

	nDestStrNumOfBytes -= sizeof(HUINT16);
	nDestStrNumOfBytes /= sizeof(HUINT16);

	while(*pSrcUTF8 && nConvCharCnt < nDestStrNumOfBytes)
	{
		pSrcUTF8 = _rxcp_utf8_ToUCS2(pDestStr, pSrcUTF8, nConvCharCnt);
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

STATIC	HINT32 			_rxcp_dvbutf8_ConvertCodepageToUCS2(const void 	*pSrcStr, HUINT16 	*pDestStr, HINT32 nDestStrNumOfBytes)
{
	return _rxcp_utf8_ConvertCodepageToUCS2(pSrcStr, pDestStr, nDestStrNumOfBytes);
}

STATIC	HINT32 			_rxcp_dvbutf8_ConvertUCS2ToCodepage(const HUINT16 	*pSrcUCS2Str, void 	*pDestStr, HINT32 nDestStrNumOfBytes)
{
	return _rxcp_utf8_ConvertUCS2ToCodepage(pSrcUCS2Str, pDestStr, nDestStrNumOfBytes);
}


STATIC	HERROR			_rxcp_utf8_GetInfo(RxCPSlot_t *pCPSlot)
{
	if(pCPSlot == NULL)
	{
		return ERR_FAIL;
	}

	pCPSlot->cbNumOfBytes	= _rxcp_utf8_NumOfBytes;
	pCPSlot->cbNumOfChars	= _rxcp_utf8_NumOfChars;
	pCPSlot->cbGetCharAt	= _rxcp_utf8_GetCharAt;

	pCPSlot->cbUCS2ToCodepage	= _rxcp_utf8_ConvertUCS2ToCodepage;
	pCPSlot->cbCodeapgeToUCS2	= _rxcp_utf8_ConvertCodepageToUCS2;

	pCPSlot->stCPTagSlot.eCodepage = eRxCP_UNICODE_UTF8;
	pCPSlot->stCPTagSlot.nCPTagLen = 1;
	pCPSlot->stCPTagSlot.szCPTag[0] = RxCPTAG_UTF_8;
	pCPSlot->stCPTagSlot.szCPTag[1] = 0;

	pCPSlot->ucEndCharBytes	= 1;

	return ERR_OK;
}

STATIC	HERROR			_rxcp_dvbutf8_GetInfo(RxCPSlot_t *pCPSlot)
{
	if(pCPSlot == NULL)
	{
		return ERR_FAIL;
	}

	pCPSlot->cbNumOfBytes	= _rxcp_utf8_NumOfBytes;
	pCPSlot->cbNumOfChars	= _rxcp_utf8_NumOfChars;
	pCPSlot->cbGetCharAt	= _rxcp_utf8_GetCharAt;

	pCPSlot->cbUCS2ToCodepage	= _rxcp_dvbutf8_ConvertUCS2ToCodepage;
	pCPSlot->cbCodeapgeToUCS2	= _rxcp_dvbutf8_ConvertCodepageToUCS2;

	pCPSlot->stCPTagSlot.eCodepage = eRxCP_UNICODE_DVB_UTF8;
	pCPSlot->ucEndCharBytes	= 1;
	pCPSlot->stCPTagSlot.szCPTag[0] = RxCPTAG_UTF_8;
	pCPSlot->stCPTagSlot.szCPTag[1] = 0;

	return ERR_OK;
}


/* *************************************************************************************************** */
#define ___PUBLIC_FUNCTION_______________________________________________________________________________
/* *************************************************************************************************** */


HERROR	INT_MHCp_UTF8(void)
{
	HERROR		hErr = ERR_FAIL;
	RxCPSlot_t	stCPSlot;

	RxSTD_MemSet(&stCPSlot, 0, sizeof(RxCPSlot_t));
	hErr = _rxcp_utf8_GetInfo(&stCPSlot);
	if(hErr == ERR_OK)
	{
		hErr = RxCP_AddCodepage(&stCPSlot);
	}

	RxSTD_MemSet(&stCPSlot, 0, sizeof(RxCPSlot_t));
	hErr = _rxcp_dvbutf8_GetInfo(&stCPSlot);
	if(hErr != ERR_OK)
	{
		hErr = RxCP_AddCodepage(&stCPSlot);
	}

	return ERR_OK;
}


/*********************** End of File ******************************/

