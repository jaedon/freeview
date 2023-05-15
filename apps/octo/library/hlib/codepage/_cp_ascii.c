
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
STATIC	HINT32			_hcodepage_ascii_NumOfBytes(const void 	*pSrc)
{
	char *pSrcString = (char *)pSrc;

	return strlen(pSrcString);
}

STATIC	HINT32			_hcodepage_ascii_NumOfChars(const void 	*pSrc)
{
	char *pSrcString = (char *)pSrc;

	return strlen(pSrcString);
}

STATIC	HINT32			_hcodepage_ascii_GetCharAt(const void 	*pSrc, const HINT32 nCharNum, void *pCharBuf)
{
	HUINT8	*pSrcString = (HUINT8	*)pSrc;
	HUINT8	*pcCharBuf	= (HUINT8	*)pCharBuf;

	*pcCharBuf = pSrcString[nCharNum];
	return 1;
}

STATIC	HINT32 			_hcodepage_ascii_ConvertCodepageToUCS2(const void 	*pSrcStr, HUINT16 	*pDestUCS2Str, HINT32 nDestStrNumOfBytes)
{
	HINT32	nLoop = 0;
	HINT32 	nSrcChars = 0;
	HINT32 	nMaxChars = 0;
	HUINT8	*pSrcString = (HUINT8	*)pSrcStr;

	nSrcChars = _hcodepage_ascii_NumOfChars(pSrcString);
	if(HxCP_IsValidReturnValue(nSrcChars) == FALSE)
	{
		return eHxCP_ERROR_FAIL;
	}

	nDestStrNumOfBytes -= sizeof(HUINT16);
	nDestStrNumOfBytes /= sizeof(HUINT16);
	nMaxChars = (nSrcChars > nDestStrNumOfBytes) ? nDestStrNumOfBytes : nSrcChars;

	for(nLoop=0; nLoop < nMaxChars; nLoop++)
	{
		pDestUCS2Str[nLoop] = pSrcString[nLoop];
	}
	pDestUCS2Str[nLoop] = 0x0000;

	return (nMaxChars*sizeof(HUINT16));
}

STATIC	HINT32 			_hcodepage_ascii_ConvertUCS2ToCodepage(const HUINT16 	*pSrcUCS2Str, void 	*pDestStr, HINT32 nDestStrNumOfBytes)
{
	HINT32		nLoop = 0;
	HINT32 		nSrcChars = 0;
	HINT32 		nMaxChars = 0;
	HUINT8		*pDestString = (HUINT8	*)pDestStr;

	nSrcChars = HLIB_CP_NumOfChars(eHxCP_UNICODE_UCS2, pSrcUCS2Str);
	if(HxCP_IsValidReturnValue(nSrcChars) == FALSE)
	{
		return eHxCP_ERROR_FAIL;
	}

	nDestStrNumOfBytes -= sizeof(HUINT8);
	nMaxChars = (nSrcChars > nDestStrNumOfBytes) ? nDestStrNumOfBytes : nSrcChars;

	for(nLoop=0; nLoop < nMaxChars; nLoop++)
	{
		if(pSrcUCS2Str[nLoop] < 0x00A0)
		{
			pDestString[nLoop] = pSrcUCS2Str[nLoop] & 0xFF;
		}
		else
		{
			pDestString[nLoop] = ' ';
		}
	}
	pDestString[nLoop] = 0x00;

	return nMaxChars;
}




STATIC	HERROR			_hcodepage_ascii_getinfo(eHxCP_Slot_t *pCPSlot)
{
	if(pCPSlot == NULL)
	{
		return ERR_FAIL;
	}

	pCPSlot->cbNumOfBytes	= _hcodepage_ascii_NumOfBytes;
	pCPSlot->cbNumOfChars	= _hcodepage_ascii_NumOfChars;
	pCPSlot->cbGetCharAt	= _hcodepage_ascii_GetCharAt;

	pCPSlot->cbUCS2ToCodepage	= _hcodepage_ascii_ConvertUCS2ToCodepage;
	pCPSlot->cbCodeapgeToUCS2	= _hcodepage_ascii_ConvertCodepageToUCS2;

	pCPSlot->ucEndCharBytes	= 1;

	pCPSlot->stCPTagSlot.eCodepage = eHxCP_ISO_ASCII;
	pCPSlot->stCPTagSlot.nCPTagLen = 0;

	return ERR_OK;
}


/* *************************************************************************************************** */
#define ___PUBLIC_FUNCTION_______________________________________________________________________________
/* *************************************************************************************************** */


HERROR	INT_HCodepage_ASCII(void)
{
	HERROR		hErr = ERR_FAIL;
	eHxCP_Slot_t	stCPSlot;

	HxSTD_MemSet(&stCPSlot, 0, sizeof(eHxCP_Slot_t));

	hErr = _hcodepage_ascii_getinfo(&stCPSlot);
	if(hErr != ERR_OK)
	{
		return hErr;
	}

	hErr = HLIB_CP_AddCodepage(&stCPSlot);
	if(hErr != ERR_OK)
	{
		return hErr;
	}

	return ERR_OK;
}


/*********************** End of File ******************************/

