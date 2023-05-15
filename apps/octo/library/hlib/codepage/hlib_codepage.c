
/* *************************************************************************************************** */
#define ___HEADER________________________________________________________________________________________
/* *************************************************************************************************** */
#include <vkernel.h>

#include <hlib.h>


#include "_cp_internal.h"


/* *************************************************************************************************** */
#define ___GLOBAL_HEADER_________________________________________________________________________________
/* *************************************************************************************************** */


/* *************************************************************************************************** */
#define ___DEFINE________________________________________________________________________________________
/* *************************************************************************************************** */

/* *************************************************************************************************** */
// BOM (Byte order mark) parser
// http://en.wikipedia.org/wiki/Byte_order_mark
/* *************************************************************************************************** */

const HUINT8 HxCP_BOM_UTF8[]		= { 0xef, 0xbb, 0xbf };
const HUINT8 HxCP_BOM_UTF16_BE[]	= { 0xfe, 0xff };
const HUINT8 HxCP_BOM_UTF16_LE[]	= { 0xff, 0xfe };
const HUINT8 HxCP_BOM_UTF32_BE[]	= { 0x00, 0x00, 0xfe, 0xff };
const HUINT8 HxCP_BOM_UTF32_LE[]	= { 0xff, 0xfe, 0x00, 0x00 };
const HUINT8 HxCP_BOM_SCSU[]		= { 0x0e, 0xfe, 0xff };
const HUINT8 HxCP_BOM_UTF7[]		= { 0x2b, 0x2f, 0x76};
const HUINT8 HxCP_BOM_UTF7_0[]		= { 0x2b, 0x2f, 0x76, 0x38};
const HUINT8 HxCP_BOM_UTF7_1[]		= { 0x2b, 0x2f, 0x76, 0x39 };
const HUINT8 HxCP_BOM_UTF7_2[]		= { 0x2b, 0x2f, 0x76, 0x2b };
const HUINT8 HxCP_BOM_UTF7_3[]		= { 0x2b, 0x2f, 0x76, 0x2f };
const HUINT8 HxCP_BOM_UTF7_4[]		= { 0x2b, 0x2f, 0x76, 0x38, 0x2d };
const HUINT8 HxCP_BOM_UTFEBCDIC[]		= { 0xdd, 0x73, 0x66, 0x73 };
const HUINT8 HxCP_BOM_BOCU_1[]		= { 0xfb, 0xee, 0x28 };


/* *************************************************************************************************** */
#define ___TYPEDEF_______________________________________________________________________________________
/* *************************************************************************************************** */


typedef struct tagHCodepageMgr
{
	HUINT8			ucEndian;	/* ENDIAN_LITTLE or ENDIAN_BIG */


	eHxCP_Slot_t	stCodepage[eHCodepage_MAX];
} HCodepageMgr_t;



/* *************************************************************************************************** */
#define ___STATIC_VARIABLES______________________________________________________________________________
/* *************************************************************************************************** */
STATIC	HCodepageMgr_t		*s_pCodepageMgr = NULL;


/* *************************************************************************************************** */
#define ___Private_FUNCTION______________________________________________________________________________
/* *************************************************************************************************** */

STATIC	HERROR			_hcodepage_DestroyMgr(void)
{
	CP_MEM_Free(s_pCodepageMgr);
	s_pCodepageMgr = NULL;
	return ERR_OK;
}


STATIC	HERROR			_hcodeapge_InitMgr(void)
{
	while(s_pCodepageMgr ==NULL)
	{
		s_pCodepageMgr = (HCodepageMgr_t *)CP_MEM_Alloc(sizeof(HCodepageMgr_t));
		if(s_pCodepageMgr == NULL)
		{
			VK_TASK_Sleep(10);
		}
	}

	CP_Memset(s_pCodepageMgr, 0, sizeof(HCodepageMgr_t));

	return ERR_OK;
}


STATIC	HERROR			_hcodepage_InitPage(HCodepageMgr_t *pCPMgr)
{
	HERROR	hErr = ERR_FAIL;

	hErr = INT_HCodepage_ASCII();
	if(hErr != ERR_OK)
	{
		//??
	}

	hErr = INT_HCodepage_8859();
	if(hErr != ERR_OK)
	{
		//??
	}

	hErr = INT_HCodepage_6937();
	if(hErr != ERR_OK)
	{
		//??
	}

	hErr = INT_HCodepage_UTF8();
	if(hErr != ERR_OK)
	{
		//??
	}

	hErr = INT_HCodepage_UCS2();
	if(hErr != ERR_OK)
	{
		//??
	}

	hErr = INT_HCodepage_TELETEXT();
	if(hErr != ERR_OK)
	{
		//??
	}

	hErr = INT_HCodepage_JIS();
	if(hErr != ERR_OK)
	{
		//??
	}
	hErr = INT_HCodepage_EUCJP();
	if(hErr != ERR_OK)
	{
		//??
	}

	(void)pCPMgr;
	return ERR_OK;
}

STATIC HERROR			_hcodepage_GetMgr(HCodepageMgr_t **ppCPMgr)
{
	if (s_pCodepageMgr == NULL)
	{
		s_pCodepageMgr = (HCodepageMgr_t*)CP_MEM_Alloc(sizeof(HCodepageMgr_t));

		_hcodepage_InitPage(s_pCodepageMgr);
	}

	if (ppCPMgr)
		*ppCPMgr = s_pCodepageMgr;

	return ERR_OK;
}

#if	0
STATIC	HERROR			_hcodepage_GetMgr(HCodepageMgr_t **ppCPMgr)
{
	if(ppCPMgr == NULL || s_pCodepageMgr == NULL)
	{
		return ERR_FAIL;
	}

	*ppCPMgr = s_pCodepageMgr;
	return ERR_OK;
}
#endif

#if 0
STATIC	HERROR			_hcodepage_GetCurrentEndian(HCodepageMgr_t *pCPMgr)
{
	//	HUINT32		ulCheckEndian = 0;

	if(pCPMgr == NULL)
	{
		return ERR_FAIL;
	}

	// ::TODO
	pCPMgr->ucEndian = CP_ENDIAN_LITTLE;

	return ERR_OK;
}
#endif


STATIC	HERROR			_hcodepage_CopySlot(eHxCP_Slot_t *pstDstSlot, eHxCP_Slot_t *pstSrcSlot)
{
	if(pstDstSlot == NULL || pstSrcSlot == NULL)
	{
		return ERR_FAIL;
	}

	HxSTD_MemCopy(pstDstSlot, pstSrcSlot, sizeof(eHxCP_Slot_t));

	return ERR_OK;
}



/* *************************************************************************************************** */
#define ___PUBLIC_FUNCTION_______________________________________________________________________________
/* *************************************************************************************************** */
HERROR	HLIB_CP_Init(void)
{
	return _hcodepage_GetMgr(NULL);
}
#if 0
HERROR	HLIB_CP_Init(void)
{
	HERROR			hErr = ERR_FAIL;
	HCodepageMgr_t	*pCPMgr = NULL;

	if(bIsInit == TRUE)
	{
		return ERR_OK;
	}

	hErr = _hcodeapge_InitMgr();
	if(hErr != ERR_OK)
	{
		return hErr;
	}

	hErr = _hcodepage_GetMgr(&pCPMgr);
	if(hErr != ERR_OK)
	{
		return hErr;
	}

	hErr = _hcodepage_InitPage(pCPMgr);
	if(hErr != ERR_OK)
	{
		return hErr;
	}

	bIsInit = TRUE;

	return ERR_OK;
}
#endif

HERROR	HLIB_CP_Destroy(void)
{
	HERROR 	hErr = ERR_FAIL;

	hErr = _hcodepage_DestroyMgr();
	if(hErr != ERR_OK)
	{
		return hErr;
	}

	return ERR_OK;
}


HERROR	HLIB_CP_AddCodepage(eHxCP_Slot_t *pstCPSlot)
{
	HERROR			hErr = ERR_FAIL;
	HCodepageMgr_t	*pCPMgr = NULL;
	eHxCP_CodePage_e 	eCodepage = eHCodepage_MAX;

	if(pstCPSlot == NULL)
	{
		return hErr;
	}

	eCodepage = pstCPSlot->stCPTagSlot.eCodepage;
	if(eCodepage >= eHCodepage_MAX)
	{
		return hErr;
	}

	hErr = _hcodepage_GetMgr(&pCPMgr);
	if(hErr != ERR_OK)
	{
		return hErr;
	}

	hErr = _hcodepage_CopySlot(&(pCPMgr->stCodepage[eCodepage]), pstCPSlot);
	if(hErr != ERR_OK)
	{
		return hErr;
	}

	return ERR_OK;
}

HERROR	HLIB_CP_GetSlot(eHxCP_CodePage_e eCodepage, eHxCP_Slot_t *pstCPSlot)
{
	HERROR			hErr = ERR_FAIL;
	HCodepageMgr_t	*pCPMgr = NULL;

	if(eCodepage >= eHCodepage_MAX)
	{
		return hErr;
	}

	if(pstCPSlot == NULL)
	{
		return hErr;
	}

	hErr = _hcodepage_GetMgr(&pCPMgr);
	if(hErr != ERR_OK)
	{
		return hErr;
	}

	hErr = _hcodepage_CopySlot(pstCPSlot, &(pCPMgr->stCodepage[eCodepage]));
	if(hErr != ERR_OK)
	{
		return hErr;
	}

	return ERR_OK;
}

HINT32	HLIB_CP_ConvertCodepageToUCS2(eHxCP_CodePage_e eCodepage, const void 	*pSrcStr, HUINT16 	*pDestUCS2Str, HINT32 nDestStrNumOfBytes)
{
	HERROR			hErr = ERR_FAIL;
	HCodepageMgr_t	*pCPMgr = NULL;

	if(eCodepage >= eHCodepage_MAX)
	{
		return eHxCP_ERROR_FAIL;
	}

	hErr = _hcodepage_GetMgr(&pCPMgr);
	if(hErr != ERR_OK)
	{
		return eHxCP_ERROR_FAIL;
	}

	if(pCPMgr->stCodepage[eCodepage].cbCodeapgeToUCS2 == NULL)
	{
		return eHxCP_ERROR_FAIL;
	}

	return pCPMgr->stCodepage[eCodepage].cbCodeapgeToUCS2(pSrcStr, pDestUCS2Str, nDestStrNumOfBytes);
}

HINT32	HLIB_CP_ConvertUCS2ToCodepage(eHxCP_CodePage_e eCodepage, const HUINT16 	*pSrcUCS2Str, void 	*pDestStr, HINT32 nDestStrNumOfBytes)
{
	HERROR			hErr = ERR_FAIL;
	HCodepageMgr_t	*pCPMgr = NULL;

	if(eCodepage >= eHCodepage_MAX)
	{
		return eHxCP_ERROR_FAIL;
	}

	hErr = _hcodepage_GetMgr(&pCPMgr);
	if(hErr != ERR_OK)
	{
		return eHxCP_ERROR_FAIL;
	}

	if(pCPMgr->stCodepage[eCodepage].cbUCS2ToCodepage == NULL)
	{
		return eHxCP_ERROR_FAIL;
	}

	return pCPMgr->stCodepage[eCodepage].cbUCS2ToCodepage(pSrcUCS2Str, pDestStr, nDestStrNumOfBytes);
}



/**
*
* 차지하고 있는 Byte 수
*
* @param	codeapge enum value (view eHxCP_CodePage_e table)
* @param	String Pointer
* @return	Byte - HxCP_IsValidReturnValue(return byte) 확인 할 것.
*/
HINT32	HLIB_CP_NumOfBytes(eHxCP_CodePage_e eCodepage, const void *pSrc)
{
	HERROR			hErr = ERR_FAIL;
	HCodepageMgr_t	*pCPMgr = NULL;

	if(eCodepage >= eHCodepage_MAX)
	{
		return eHxCP_ERROR_FAIL;
	}

	hErr = _hcodepage_GetMgr(&pCPMgr);
	if(hErr != ERR_OK)
	{
		return eHxCP_ERROR_FAIL;
	}

	if(pCPMgr->stCodepage[eCodepage].cbNumOfBytes == NULL)
	{
		return eHxCP_ERROR_FAIL;
	}

	return pCPMgr->stCodepage[eCodepage].cbNumOfBytes(pSrc);
}



/**
*
* 실제 문자의 개수
*
* @param	codeapge enum value (view eHxCP_CodePage_e table)
* @param	String Pointer
* @return	Byte - HxCP_IsValidReturnValue(return byte) 확인 할 것.
*/
HINT32	HLIB_CP_NumOfChars(eHxCP_CodePage_e eCodepage, const void *pSrc)
{
	HERROR			hErr = ERR_FAIL;
	HCodepageMgr_t	*pCPMgr = NULL;

	if(eCodepage >= eHCodepage_MAX)
	{
		return eHxCP_ERROR_FAIL;
	}

	hErr = _hcodepage_GetMgr(&pCPMgr);
	if(hErr != ERR_OK)
	{
		return eHxCP_ERROR_FAIL;
	}

	if(pCPMgr->stCodepage[eCodepage].cbNumOfChars == NULL)
	{
		return eHxCP_ERROR_FAIL;
	}

	return pCPMgr->stCodepage[eCodepage].cbNumOfChars(pSrc);
}


HINT32	HLIB_CP_CharAt(eHxCP_CodePage_e eCodepage, const void *pSrc, const HINT32 nCharNum, void *pCharBuf)
{
	HERROR			hErr = ERR_FAIL;
	HCodepageMgr_t	*pCPMgr = NULL;

	if(eCodepage >= eHCodepage_MAX)
	{
		return eHxCP_ERROR_FAIL;
	}

	hErr = _hcodepage_GetMgr(&pCPMgr);
	if(hErr != ERR_OK)
	{
		return eHxCP_ERROR_FAIL;
	}

	if(pCPMgr->stCodepage[eCodepage].cbGetCharAt == NULL)
	{
		return eHxCP_ERROR_FAIL;
	}

	return pCPMgr->stCodepage[eCodepage].cbGetCharAt(pSrc,nCharNum, pCharBuf);
}

const HUINT16	*HLIB_CP_CodepageToUCS2Table(eHxCP_CodePage_e eCodepage)
{
	HERROR			hErr = ERR_FAIL;
	HCodepageMgr_t	*pCPMgr = NULL;

	if(eCodepage >= eHCodepage_MAX)
	{
		return NULL;
	}

	hErr = _hcodepage_GetMgr(&pCPMgr);
	if(hErr != ERR_OK)
	{
		return NULL;
	}

	if(pCPMgr->stCodepage[eCodepage].pusUCS2Table == NULL)
	{
		return NULL;
	}

	return pCPMgr->stCodepage[eCodepage].pusUCS2Table;
}

HERROR		HLIB_CP_CPTagByCodepage(const HUINT8 *pcStr, eHxCP_CodePage_e *peCodepage, HINT32 *pnCPTagLen)
{
	HINT32			nLoop = 0;
	HINT32			nMaxLoop = (HINT32)eHCodepage_MAX;
	HERROR			hErr = ERR_FAIL;
	HCodepageMgr_t	*pCPMgr = NULL;
	CPTagSlot_t		*pstCPTagSlot = NULL;

#if defined(TEST_CP)
	HBOOL		bFindCPTag1 = FALSE, bFindCPTag2 = FALSE;
#endif

	if(peCodepage == NULL || pnCPTagLen == NULL)
	{
		return ERR_FAIL;
	}

	hErr = _hcodepage_GetMgr(&pCPMgr);
	if(hErr != ERR_OK)
	{
		return ERR_FAIL;
	}

	*peCodepage = eHCodepage_MAX;
	*pnCPTagLen = 0;

	for(nLoop=0; nLoop < nMaxLoop; nLoop++)
	{
#if defined(TEST_CP)
		bFindCPTag1 = FALSE;
		bFindCPTag2 = FALSE;

		pstCPTagSlot = &pCPMgr->stCodepage[nLoop].stCPTagSlot;
		if(pstCPTagSlot->nCPTagLen <= 0)
		{
			bFindCPTag1 = FALSE;
		}
		else
		{
			if(memcmp(pcStr, pstCPTagSlot->szCPTag, pstCPTagSlot->nCPTagLen) == 0)
				bFindCPTag1 =TRUE;
		}

		if(pstCPTagSlot->nCPTagLen2 <=0)
		{
			bFindCPTag2 = FALSE;
		}
		else
		{
			if(memcmp(pcStr, pstCPTagSlot->szCPTag2, pstCPTagSlot->nCPTagLen2) == 0)
				bFindCPTag2 =TRUE;
		}

		if(bFindCPTag1 == FALSE && bFindCPTag2 == FALSE)
			continue;

		if(bFindCPTag1 == TRUE)
		{
			*pnCPTagLen = pstCPTagSlot->nCPTagLen;
			*peCodepage = pstCPTagSlot->eCodepage;
		}

		if(bFindCPTag2 == TRUE)
		{
			*pnCPTagLen = pstCPTagSlot->nCPTagLen2;
			*peCodepage = pstCPTagSlot->eCodepage;
		}
#else
		pstCPTagSlot = &pCPMgr->stCodepage[nLoop].stCPTagSlot;
		if(pstCPTagSlot->nCPTagLen <= 0)
		{
			continue;
		}

		if(pcStr[0] != pstCPTagSlot->szCPTag[0])
		{
			continue;
		}

		if(pstCPTagSlot->nCPTagLen > 1 &&
			memcmp(pcStr, pstCPTagSlot->szCPTag, pstCPTagSlot->nCPTagLen) != 0)
		{
			continue;
		}
		*pnCPTagLen = pstCPTagSlot->nCPTagLen;
		*peCodepage = pstCPTagSlot->eCodepage;
#endif
		break;
	}

	if(nLoop < eHCodepage_MAX)
	{
		return ERR_OK;
	}

	return ERR_FAIL;
}

HERROR		HLIB_CP_CodepageByCPTag(eHxCP_CodePage_e eCodepage, HUINT8 *pszCPTagStr, HINT32 *pnCPTagLen)
{
	HERROR			hErr = ERR_FAIL;
	HCodepageMgr_t	*pCPMgr = NULL;
	CPTagSlot_t		*pstCPTagSlot = NULL;

	if(eCodepage >= eHCodepage_MAX)
	{
		return ERR_FAIL;
	}

	if(pszCPTagStr == NULL ||
		pnCPTagLen == NULL)
	{
		return ERR_FAIL;
	}

	hErr = _hcodepage_GetMgr(&pCPMgr);
	if(hErr != ERR_OK)
	{
		return ERR_FAIL;
	}

	pstCPTagSlot = &pCPMgr->stCodepage[eCodepage].stCPTagSlot;
	HxSTD_StrNCpy((HCHAR*)pszCPTagStr, (const HCHAR*)pstCPTagSlot->szCPTag, pstCPTagSlot->nCPTagLen);
	*pnCPTagLen = pstCPTagSlot->nCPTagLen;

	return ERR_OK;
}

eHxCP_CodeTable_e	HLIB_CP_UCS2CodeTable(HUINT16 usUCS2)
{
	eHxCP_CodeTable_e eCT =	eHxCP_CODE_TABLE_Auto;

	eCT = INT_HCodepage_GetUCS2CodeTable(usUCS2);

	return eCT;
}


eHxCP_CodeTable_e HLIB_CP_StringUCS2CodeTable(HUINT16 *pszUCS2, HUINT32	 nNumOfChars)
{
	HINT32 			i = 0;
	eHxCP_CodeTable_e 	eCT		= eHxCP_CODE_TABLE_Auto;

	for(i=0; i < nNumOfChars; i++)
	{
		eCT = HLIB_CP_UCS2CodeTable(pszUCS2[i]);
		switch(eCT)
		{
			/* left to right code table */
		case eHxCP_CODE_TABLE_Basic_Latin:
		case eHxCP_CODE_TABLE_Latin_1_Supplement:
		case eHxCP_CODE_TABLE_Latin_Extended_A:
		case eHxCP_CODE_TABLE_Latin_Extended_B:
		case eHxCP_CODE_TABLE_IPA_Extensions:
			break;

			/* right to left code table */
		case eHxCP_CODE_TABLE_Arabic:
			return eCT;

		default:
			/* unkown */
			return eCT;
		}
	}

	return eCT;
}

HERROR	HLIB_CP_PrintMgrInfo(void)
{
	HERROR			hErr = ERR_FAIL;
	HCodepageMgr_t	*pCPMgr = NULL;

	hErr = _hcodepage_GetMgr(&pCPMgr);
	if(hErr != ERR_OK)
	{
		return hErr;
	}

	// ::TODO


	return ERR_OK;
}

/* *************************************************************************************************** */
#define ___PUBLIC_UTIL_FUNCTION_______________________________________________________________________________
/* *************************************************************************************************** */
HINT32	HLIB_CP_UTIL_ConvertCodepageToUTF8(eHxCP_CodePage_e eCodepage, const void 	*pSrcStr, HUINT8 	*pDestUTF8Str, HINT32 nDestStrNumOfBytes)
{
	HERROR			hErr = ERR_FAIL;
	HCodepageMgr_t	*pCPMgr = NULL;

	HINT32			nNumOfChars = 0;
	HINT32			nRetUseBytes = 0;
	HINT32			nTempNumOfBytes = 0;
	HUINT16			*pTempStr = NULL;

	if(eCodepage >= eHCodepage_MAX)
	{
		return eHxCP_ERROR_FAIL;
	}

	hErr = _hcodepage_GetMgr(&pCPMgr);
	if(hErr != ERR_OK)
	{
		return eHxCP_ERROR_FAIL;
	}

	switch(eCodepage)
	{
	case eHxCP_UNICODE_UTF8:
	case eHxCP_UNICODE_DVB_UTF8:
		if(NULL == pCPMgr->stCodepage[eCodepage].cbNumOfBytes)
		{
			return eHxCP_ERROR_FAIL;
		}
		nRetUseBytes = pCPMgr->stCodepage[eCodepage].cbNumOfBytes(pSrcStr);

		nRetUseBytes = (nRetUseBytes < nDestStrNumOfBytes) ? nRetUseBytes : (nDestStrNumOfBytes-1);

		CP_Memcpy(pDestUTF8Str, pSrcStr, nRetUseBytes);
		pDestUTF8Str[nRetUseBytes] = 0;
		break;

	case eHxCP_UNICODE_UCS2:
	case eHxCP_UNICODE_DVB_UCS2:
		if(pCPMgr->stCodepage[eHxCP_UNICODE_DVB_UTF8].cbUCS2ToCodepage== NULL)
		{
			return eHxCP_ERROR_FAIL;
		}

		nRetUseBytes = pCPMgr->stCodepage[eHxCP_UNICODE_DVB_UTF8].cbUCS2ToCodepage(pTempStr, pDestUTF8Str, nDestStrNumOfBytes);
		break;

	default:
		if(NULL == pCPMgr->stCodepage[eCodepage].cbNumOfBytes)
		{
			return eHxCP_ERROR_FAIL;
		}
		nNumOfChars = pCPMgr->stCodepage[eCodepage].cbNumOfChars(pSrcStr);

		if(pCPMgr->stCodepage[eCodepage].cbCodeapgeToUCS2 == NULL)
		{
			return eHxCP_ERROR_FAIL;
		}

		nTempNumOfBytes = (nNumOfChars+2) * 2;
		pTempStr = CP_MEM_Alloc(nTempNumOfBytes);
		CP_Memset(pTempStr, 0, nTempNumOfBytes);
		if(NULL == pTempStr)
		{
			return eHxCP_ERROR_FAIL;
		}
		pCPMgr->stCodepage[eCodepage].cbCodeapgeToUCS2(pSrcStr, pTempStr, nTempNumOfBytes);


		if(pCPMgr->stCodepage[eHxCP_UNICODE_DVB_UTF8].cbUCS2ToCodepage== NULL)
		{
			CP_MEM_Free(pTempStr);
			return eHxCP_ERROR_FAIL;
		}

		nRetUseBytes = pCPMgr->stCodepage[eHxCP_UNICODE_DVB_UTF8].cbUCS2ToCodepage(pTempStr, pDestUTF8Str, nDestStrNumOfBytes);
		CP_MEM_Free(pTempStr);
	}


	return nRetUseBytes;
}

HINT32 HLIB_CP_UTIL_ConvertCodepageToShiftJIS(eHxCP_CodePage_e eCodepage, HUINT8 *pSrcStr, HUINT8 *pDstStr, HINT32 nSrcMaxLen, HINT32 nDstMaxLen, HUINT32 *pulUseStrLen )
{
	HERROR			hErr = ERR_FAIL;
	HCodepageMgr_t	*pCPMgr = NULL;

	hErr = _hcodepage_GetMgr(&pCPMgr);
	if(hErr != ERR_OK)
	{
		return eHxCP_ERROR_FAIL;
	}

	if(NULL == pCPMgr->stCodepage[eCodepage].uExt.stJapanCp.cbCodepageToSJIS)
	{
		return eHxCP_ERROR_FAIL;
	}

	return pCPMgr->stCodepage[eCodepage].uExt.stJapanCp.cbCodepageToSJIS(pSrcStr,pDstStr,nSrcMaxLen,nDstMaxLen,pulUseStrLen);

}


/*********************** End of File ******************************/
