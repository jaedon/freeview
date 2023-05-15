
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


typedef struct tagRxCpMgr
{
	HUINT8			ucEndian;	/* ENDIAN_LITTLE or ENDIAN_BIG */


	RxCPSlot_t	stCodepage[eMHCp_MAX];
} RxCpMgr_t;



/* *************************************************************************************************** */
#define ___STATIC_VARIABLES______________________________________________________________________________
/* *************************************************************************************************** */
STATIC	RxCpMgr_t		*s_pCodepageMgr = NULL;



/* *************************************************************************************************** */
#define ___Private_FUNCTION______________________________________________________________________________
/* *************************************************************************************************** */

STATIC	HERROR			_rxcp_mgr_InitMgr(void)
{
	s_pCodepageMgr = (RxCpMgr_t *)CP_MEM_Alloc(sizeof(RxCpMgr_t));


	CP_Memset(s_pCodepageMgr, 0, sizeof(RxCpMgr_t));

	return ERR_OK;
}


STATIC	HERROR			_rxcp_DestroyMgr(void)
{
	CP_MEM_Free(s_pCodepageMgr);
	s_pCodepageMgr = NULL;
	return ERR_OK;
}



STATIC	HERROR			_rxcp_GetMgr(RxCpMgr_t **ppCPMgr)
{
	if(ppCPMgr == NULL || s_pCodepageMgr == NULL)
	{
		return ERR_FAIL;
	}

	*ppCPMgr = s_pCodepageMgr;
	return ERR_OK;
}

#if 0
STATIC	HERROR			_rxcp_GetCurrentEndian(RxCpMgr_t *pCPMgr)
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

STATIC	HERROR			_rxcp_InitPage(RxCpMgr_t *pCPMgr)
{
	HERROR	hErr = ERR_FAIL;
/*
	hErr = INT_MHCp_ASCII();
	if(hErr != ERR_OK)
	{
		//??
	}

	hErr = INT_MHCp_8859();
	if(hErr != ERR_OK)
	{
		//??
	}

	hErr = INT_MHCp_6937();
	if(hErr != ERR_OK)
	{
		//??
	}
*/
	hErr = INT_MHCp_UTF8();
	if(hErr != ERR_OK)
	{
		//??
	}

	hErr = INT_MHCp_UCS2();
	if(hErr != ERR_OK)
	{
		//??
	}
/*
	hErr = INT_MHCp_TELETEXT();
	if(hErr != ERR_OK)
	{
		//??
	}
*/
	(void)pCPMgr;
	return ERR_OK;
}

STATIC	HERROR			_rxcp_CopySlot(RxCPSlot_t *pstDstSlot, RxCPSlot_t *pstSrcSlot)
{
	if(pstDstSlot == NULL || pstSrcSlot == NULL)
	{
		return ERR_FAIL;
	}

	RxSTD_MemCopy(pstDstSlot, pstSrcSlot, sizeof(RxCPSlot_t));

	return ERR_OK;
}



/* *************************************************************************************************** */
#define ___PUBLIC_FUNCTION_______________________________________________________________________________
/* *************************************************************************************************** */

HERROR	RxCP_Init(void)
{
	HERROR			hErr = ERR_FAIL;
	RxCpMgr_t	*pCPMgr = NULL;

	hErr = _rxcp_mgr_InitMgr();
	if(hErr != ERR_OK)
	{
		return hErr;
	}

	hErr = _rxcp_GetMgr(&pCPMgr);
	if(hErr != ERR_OK)
	{
		return hErr;
	}

	hErr = _rxcp_InitPage(pCPMgr);
	if(hErr != ERR_OK)
	{
		return hErr;
	}

	return ERR_OK;
}

HERROR	RxCP_Destroy(void)
{
	HERROR 	hErr = ERR_FAIL;

	hErr = _rxcp_DestroyMgr();
	if(hErr != ERR_OK)
	{
		return hErr;
	}

	return ERR_OK;
}


HERROR	RxCP_AddCodepage(RxCPSlot_t *pstCPSlot)
{
	HERROR			hErr = ERR_FAIL;
	RxCpMgr_t	*pCPMgr = NULL;
	MHCp_e 	eCodepage = eMHCp_MAX;

	if(pstCPSlot == NULL)
	{
		return hErr;
	}

	eCodepage = pstCPSlot->stCPTagSlot.eCodepage;
	if(eCodepage >= eMHCp_MAX)
	{
		return hErr;
	}

	hErr = _rxcp_GetMgr(&pCPMgr);
	if(hErr != ERR_OK)
	{
		return hErr;
	}

	hErr = _rxcp_CopySlot(&(pCPMgr->stCodepage[eCodepage]), pstCPSlot);
	if(hErr != ERR_OK)
	{
		return hErr;
	}

	return ERR_OK;
}

HERROR	RxCP_GetSlot(MHCp_e eCodepage, RxCPSlot_t *pstCPSlot)
{
	HERROR			hErr = ERR_FAIL;
	RxCpMgr_t	*pCPMgr = NULL;

	if(eCodepage >= eMHCp_MAX)
	{
		return hErr;
	}

	if(pstCPSlot == NULL)
	{
		return hErr;
	}

	hErr = _rxcp_GetMgr(&pCPMgr);
	if(hErr != ERR_OK)
	{
		return hErr;
	}

	hErr = _rxcp_CopySlot(pstCPSlot, &(pCPMgr->stCodepage[eCodepage]));
	if(hErr != ERR_OK)
	{
		return hErr;
	}

	return ERR_OK;
}


HINT32	RxCP_ConvertCodepageToUCS2(MHCp_e eCodepage, const void 	*pSrcStr, HUINT16 	*pDestUCS2Str, HINT32 nDestStrNumOfBytes)
{
	HERROR			hErr = ERR_FAIL;
	RxCpMgr_t	*pCPMgr = NULL;

	if(eCodepage >= eMHCp_MAX)
	{
		return ERR_FAIL;
	}

	hErr = _rxcp_GetMgr(&pCPMgr);
	if(hErr != ERR_OK)
	{
		return ERR_FAIL;
	}

	if(pCPMgr->stCodepage[eCodepage].cbCodeapgeToUCS2 == NULL)
	{
		return ERR_FAIL;
	}

	return pCPMgr->stCodepage[eCodepage].cbCodeapgeToUCS2(pSrcStr, pDestUCS2Str, nDestStrNumOfBytes);
}

HINT32	RxCP_ConvertUCS2ToCodepage(MHCp_e eCodepage, const HUINT16 	*pSrcUCS2Str, void 	*pDestStr, HINT32 nDestStrNumOfBytes)
{
	HERROR			hErr = ERR_FAIL;
	RxCpMgr_t	*pCPMgr = NULL;

	if(eCodepage >= eMHCp_MAX)
	{
		return ERR_FAIL;
	}

	hErr = _rxcp_GetMgr(&pCPMgr);
	if(hErr != ERR_OK)
	{
		return ERR_FAIL;
	}

	if(pCPMgr->stCodepage[eCodepage].cbUCS2ToCodepage == NULL)
	{
		return ERR_FAIL;
	}

	return pCPMgr->stCodepage[eCodepage].cbUCS2ToCodepage(pSrcUCS2Str, pDestStr, nDestStrNumOfBytes);
}



/**
*
* 차지하고 있는 Byte 수
*
* @param	codeapge enum value (view MHCp_e table)
* @param	String Pointer
* @return	Byte - RxCP_IsValidReturnValue(return byte) 확인 할 것.
*/
HINT32	RxCP_NumOfBytes(MHCp_e eCodepage, const void *pSrc)
{
	HERROR			hErr = ERR_FAIL;
	RxCpMgr_t	*pCPMgr = NULL;

	if(eCodepage >= eMHCp_MAX)
	{
		return ERR_FAIL;
	}

	hErr = _rxcp_GetMgr(&pCPMgr);
	if(hErr != ERR_OK)
	{
		return ERR_FAIL;
	}

	if(pCPMgr->stCodepage[eCodepage].cbNumOfBytes == NULL)
	{
		return ERR_FAIL;
	}

	return pCPMgr->stCodepage[eCodepage].cbNumOfBytes(pSrc);
}



/**
*
* 실제 문자의 개수
*
* @param	codeapge enum value (view MHCp_e table)
* @param	String Pointer
* @return	Byte - RxCP_IsValidReturnValue(return byte) 확인 할 것.
*/
HINT32	RxCP_NumOfChars(MHCp_e eCodepage, const void *pSrc)
{
	HERROR			hErr = ERR_FAIL;
	RxCpMgr_t	*pCPMgr = NULL;

	if(eCodepage >= eMHCp_MAX)
	{
		return ERR_FAIL;
	}

	hErr = _rxcp_GetMgr(&pCPMgr);
	if(hErr != ERR_OK)
	{
		return ERR_FAIL;
	}

	if(pCPMgr->stCodepage[eCodepage].cbNumOfChars == NULL)
	{
		return ERR_FAIL;
	}

	return pCPMgr->stCodepage[eCodepage].cbNumOfChars(pSrc);
}


HINT32	RxCP_CharAt(MHCp_e eCodepage, const void *pSrc, const HINT32 nCharNum, void *pCharBuf)
{
	HERROR			hErr = ERR_FAIL;
	RxCpMgr_t	*pCPMgr = NULL;

	if(eCodepage >= eMHCp_MAX)
	{
		return ERR_FAIL;
	}

	hErr = _rxcp_GetMgr(&pCPMgr);
	if(hErr != ERR_OK)
	{
		return ERR_FAIL;
	}

	if(pCPMgr->stCodepage[eCodepage].cbGetCharAt == NULL)
	{
		return ERR_FAIL;
	}

	return pCPMgr->stCodepage[eCodepage].cbGetCharAt(pSrc,nCharNum, pCharBuf);
}

const HUINT16	*RxCP_CodepageToUCS2Table(MHCp_e eCodepage)
{
	HERROR			hErr = ERR_FAIL;
	RxCpMgr_t	*pCPMgr = NULL;

	if(eCodepage >= eMHCp_MAX)
	{
		return NULL;
	}

	hErr = _rxcp_GetMgr(&pCPMgr);
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

HERROR		RxCP_CPTagByCodepage(const HUINT8 *pcStr, MHCp_e *peCodepage, HINT32 *pnCPTagLen)
{
	HINT32			nLoop = 0;
	HINT32			nMaxLoop = (HINT32)eMHCp_MAX;
	HERROR			hErr = ERR_FAIL;
	RxCpMgr_t	*pCPMgr = NULL;
	RxCPTagSlot_t		*pstCPTagSlot = NULL;

	if(peCodepage == NULL || pnCPTagLen == NULL)
	{
		return ERR_FAIL;
	}

	hErr = _rxcp_GetMgr(&pCPMgr);
	if(hErr != ERR_OK)
	{
		return ERR_FAIL;
	}

	*peCodepage = eMHCp_MAX;
	*pnCPTagLen = 0;

	for(nLoop=0; nLoop < nMaxLoop; nLoop++)
	{
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
			RxSTD_StrNCmp(pcStr, pstCPTagSlot->szCPTag, pstCPTagSlot->nCPTagLen) != 0)
		{
			continue;
		}

		*pnCPTagLen = pstCPTagSlot->nCPTagLen;
		*peCodepage = pstCPTagSlot->eCodepage;
		break;
	}

	if(nLoop < eMHCp_MAX)
	{
		return ERR_OK;
	}

	return ERR_FAIL;
}

HERROR		RxCP_CodepageByCPTag(MHCp_e eCodepage, HUINT8 *pszCPTagStr, HINT32 *pnCPTagLen)
{
	HERROR			hErr = ERR_FAIL;
	RxCpMgr_t	*pCPMgr = NULL;
	RxCPTagSlot_t		*pstCPTagSlot = NULL;

	if(eCodepage >= eMHCp_MAX)
	{
		return ERR_FAIL;
	}

	if(pszCPTagStr == NULL ||
		pnCPTagLen == NULL)
	{
		return ERR_FAIL;
	}

	hErr = _rxcp_GetMgr(&pCPMgr);
	if(hErr != ERR_OK)
	{
		return ERR_FAIL;
	}

	pstCPTagSlot = &pCPMgr->stCodepage[eCodepage].stCPTagSlot;
	RxSTD_StrNCpy(pszCPTagStr, pstCPTagSlot->szCPTag, pstCPTagSlot->nCPTagLen);
	*pnCPTagLen = pstCPTagSlot->nCPTagLen;

	return ERR_OK;
}

RxCT_e	RxCP_UCS2CodeTable(HUINT16 usUCS2)
{
	RxCT_e eCT =	eRxCT_Auto;


#if 1//defined(CONFIG_CODEPAGE_UCS2)
	eCT = INT_MHCp_GetUCS2CodeTable(usUCS2);
#endif

	return eCT;
}


RxCT_e RxCP_StringUCS2CodeTable(HUINT16 *pszUCS2, HUINT32	 nNumOfChars)
{
	HINT32 			i = 0;
	RxCT_e 	eCT		= eRxCT_Auto;

	for(i=0; i < (HINT32)nNumOfChars; i++)
	{
		eCT = RxCP_UCS2CodeTable(pszUCS2[i]);
		switch(eCT)
		{
			/* left to right code table */
		case eRxCT_Basic_Latin:
		case eRxCT_Latin_1_Supplement:
		case eRxCT_Latin_Extended_A:
		case eRxCT_Latin_Extended_B:
		case eRxCT_IPA_Extensions:
			break;

			/* right to left code table */
		case eRxCT_Arabic:
			return eCT;

		default:
			/* unkown */
			return eCT;
		}
	}

	return eCT;
}



#if defined(CONFIG_DEBUG)
HERROR	RxCP_PrintMgrInfo()
{
	HERROR			hErr = ERR_FAIL;
	RxCpMgr_t	*pCPMgr = NULL;

	hErr = _rxcp_GetMgr(&pCPMgr);
	if(hErr != ERR_OK)
	{
		return hErr;
	}

	// ::TODO


	return ERR_OK;
}
#endif

/*********************** End of File ******************************/
