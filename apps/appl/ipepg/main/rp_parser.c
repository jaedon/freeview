/**************************************************************
 *	@file		rp_parser.c
 *	Remote Programming parser for OCTO 2.0
 *
 *	http://www.humaxdigital.com
 *
 *	@data			2012/08/01
 *	@author			humax
 *	@brief			Remote Programming parser for OCTO 2.0 Project
 **************************************************************/
#include <unistd.h>
#include <signal.h>

#include <vkernel.h>
#include <hlib.h>


#include "../include/ipepg.h"
#include "rp_common.h"

//////////////////////////////////////////////////////////////////////////////////// XML


typedef	enum
{
	RPHUMAX_ELEMENT_LIST_RP 					= 0,

}RPHUMAX_DATA_LISTTYPE;


typedef	enum
{
	RPHUMAX_TYPE_NONE 							= 0,
	RPHUMAX_TYPE_STRING,
	RPHUMAX_TYPE_DECIMAL,
	RPHUMAX_TYPE_DECIMAL64,
	RPHUMAX_TYPE_HEXA

}RPHUMAX_ELEMENTDATA_TYPE_e;



typedef	enum
{
	RPHUMAX_ELEMENT_DEPTH_ROOT 					= 0,
	RPHUMAX_ELEMENT_DEPTH_1,
	RPHUMAX_ELEMENT_DEPTH_2,
	RPHUMAX_ELEMENT_DEPTH_3,
	RPHUMAX_ELEMENT_DEPTH_4,
	RPHUMAX_ELEMENT_DEPTH_5,
	RPHUMAX_ELEMENT_DEPTH_6,
	RPHUMAX_ELEMENT_DEPTH_7,
	RPHUMAX_ELEMENT_DEPTH_8
}RPHUMAX_ELEMENT_DEPTH_e;



typedef enum
{
	RPHUMAX_RP_RESPONSE = 0,
	RPHUMAX_RP_RPLIST,
	RPHUMAX_RP_RPDATA,
	RPHUMAX_RP_MAX
}RPHUMAX_RP_e;

typedef enum
{
	RPHUMAX_ELEMENT_RPINDEX = 0,
#if 0 // to be deleted
	RPHUMAX_ELEMENT_ONID,
	RPHUMAX_ELEMENT_TSID,
	RPHUMAX_ELEMENT_SVCID,
#endif
	RPHUMAX_ELEMENT_SVCHANDLE,
	RPHUMAX_ELEMENT_UNIQUEID,
	RPHUMAX_ELEMENT_START,
	RPHUMAX_ELEMENT_END,
	RPHUMAX_ELEMENT_RSVTYPE,
	RPHUMAX_ELEMENT_REPEAT,
	RPHUMAX_ELEMENT_MAX
}RPHUMAX_ELEMENT_e;

typedef enum
{
	RPHUMAX_RPLIST_ATTRIBUTE_VERSION = 0,
	RPHUMAX_RPLIST_ATTRIBUTE_COUNTRY,
	RPHUMAX_RPLIST_ATTRIBUTE_COUNT,
	RPHUMAX_RPLIST_ATTRIBUTE_MAX
} RPHUMAX_RPLIST_ATTRIBUTE_e;


typedef enum
{
	RPHUMAX_RPDATA_ATTRIBUTE_KIND = 0,
	RPHUMAX_RPDATA_ATTRIBUTE_ACTION,
	RPHUMAX_RPDATA_ATTRIBUTE_MAX
} RPHUMAX_RPDATA_ATTRIBUTE_e;



typedef struct
{
	HUINT32					ulAttrDataType;
	HUINT8					*pucAttrName;
} RP_HUMAX_ATTRIBUTE_t;


typedef struct RPHumaxElement
{
	HBOOL					bFlagEnd;
	HUINT32					ulDepth;

	HUINT32					ulTagDataType;
	HUINT8					*pucTagName;

	struct RPHumaxElement	*ElementList;

	RP_HUMAX_ATTRIBUTE_t	*AttributeList;

} RP_HUMAX_ELEMENT_t;


typedef struct
{
	HUINT32					ulListType;
	HUINT32					ulElementNum;
	RP_HUMAX_ELEMENT_t			*pstElement;
} RP_HUMAX_ELEMENTLIST_t;


typedef struct
{
	HUINT8		ucOrgStrLen;
	HUINT8		ucFlagReplace;
	HUINT8		szOrgStr[8];
	HUINT8		ucReplaceCode;
} RP_HUMAX_REPLACESTR_t;


////////////////////////////////////////////////////////////////////////////////////////

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

const static RP_HUMAX_ATTRIBUTE_t	s_astAttributeRpList[] =
{
	{RPHUMAX_TYPE_DECIMAL, (HUINT8 *)"version"},
	{RPHUMAX_TYPE_DECIMAL, (HUINT8 *)"country"},
	{RPHUMAX_TYPE_DECIMAL, (HUINT8 *)"count"},
};

const static RP_HUMAX_ATTRIBUTE_t	s_astAttributeRpData[] =
{
	{RPHUMAX_TYPE_DECIMAL, (HUINT8 *)"kind"},
	{RPHUMAX_TYPE_DECIMAL, (HUINT8 *)"action"}
};



const static RP_HUMAX_ELEMENT_t		s_astElementRpData[] =
{
	{FALSE,		RPHUMAX_ELEMENT_DEPTH_3,		RPHUMAX_TYPE_DECIMAL,		(HUINT8 *)"rpindex",		NULL,	NULL	},
	{FALSE, 	RPHUMAX_ELEMENT_DEPTH_3,		RPHUMAX_TYPE_DECIMAL, 		(HUINT8 *)"hsvc",			NULL,	NULL	},
	{FALSE, 	RPHUMAX_ELEMENT_DEPTH_3,		RPHUMAX_TYPE_DECIMAL,		(HUINT8 *)"uniqueid",		NULL,	NULL	},
	{FALSE, 	RPHUMAX_ELEMENT_DEPTH_3,		RPHUMAX_TYPE_DECIMAL,		(HUINT8 *)"start",			NULL,	NULL	},
	{FALSE, 	RPHUMAX_ELEMENT_DEPTH_3,		RPHUMAX_TYPE_DECIMAL,		(HUINT8 *)"end",			NULL,	NULL	},
	{FALSE,		RPHUMAX_ELEMENT_DEPTH_3,		RPHUMAX_TYPE_DECIMAL, 		(HUINT8 *)"rsvtype",		NULL,	NULL	},
	{TRUE,		RPHUMAX_ELEMENT_DEPTH_3,		RPHUMAX_TYPE_DECIMAL,		(HUINT8 *)"repeat", 		NULL,	NULL	}
};


const static RP_HUMAX_ELEMENT_t		s_astElementRpList[] =
{
	{FALSE,		RPHUMAX_ELEMENT_DEPTH_ROOT,		RPHUMAX_TYPE_NONE,			(HUINT8 *)"response",		NULL,	NULL	},
	{FALSE,		RPHUMAX_ELEMENT_DEPTH_1,		RPHUMAX_TYPE_NONE, 			(HUINT8 *)"rplist",			NULL,	(RP_HUMAX_ATTRIBUTE_t *)s_astAttributeRpList	},
	{TRUE,		RPHUMAX_ELEMENT_DEPTH_2,		RPHUMAX_TYPE_NONE,			(HUINT8 *)"rpdata", 	(RP_HUMAX_ELEMENT_t *)s_astElementRpData,	(RP_HUMAX_ATTRIBUTE_t *)s_astAttributeRpData	},
};



const static RP_HUMAX_ELEMENTLIST_t		s_stElementListRp =	{		RPHUMAX_ELEMENT_LIST_RP,		RPHUMAX_ELEMENT_MAX,		(RP_HUMAX_ELEMENT_t *)s_astElementRpList	};


#if defined(CONFIG_XML_DTD)
const static HUINT8*				s_pucSvcDtdStream =
"\
\
\
\
";

const static HUINT8*				s_pucEpgDtdStream =
"\
\
\
\
";
#endif


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

static RP_HUMAX_REPLACESTR_t		s_astIpepgHumaxReplaceString[] =
{
		{	5,		FALSE,	{'&', 'a', 'm', 'p', ';', '\0', 0, },		0x00	},
		{	6,		TRUE,	{'&', 'q', 'u', 'o', 't', ';', '\0', 0, },	0x22	},
		{	4,		TRUE,	{'&', 'g', 't', ';', '\0', 0, },			0x3C	},
		{	4,		TRUE,	{'&', 'l', 't', ';', '\0', 0, },			0x3E	},

		// system specification 2.4.2
		// 5.3 programme data
		{	2, 		TRUE, 	{'\\', 'n', '\0', 0, }, 				0x0A	},
		{	2, 		TRUE, 	{'\\', 't', '\0', 0, }, 				0x09	},
		{	1, 		TRUE, 	{'\\', '\0', 0, },		 			0x5C	},
		{	0, 		FALSE, 	{0, }, 						0x00	}
};

static HUINT32		local_Hextoi (HUINT8 *pucPtr)
{
	HUINT32	ulVal = 0 ;

	if (pucPtr == NULL)
	{
		return 0 ;
	}

	while (1)
	{
		if (*pucPtr=='\0')
		{
			break ;
		}
		else if (*pucPtr=='0' && ( pucPtr[1]=='x' || pucPtr[1]=='X' ))
		{
			pucPtr++ ;
		}
		else if (*pucPtr>='0' && *pucPtr<='9')
		{
			ulVal *= 16 ;
			ulVal += *pucPtr - '0' ;
		}
		else if (*pucPtr>='a' && *pucPtr<='f')
		{
			ulVal *= 16 ;
			ulVal += *pucPtr - 'a' + 10 ;
		}
		else if (*pucPtr>='A' && *pucPtr<='F')
		{
			ulVal *= 16 ;
			ulVal += *pucPtr - 'A' + 10 ;
		}
		pucPtr++ ;
	}

	return ulVal ;
}


static HERROR		local_IsReplaceString(HUINT8 *pucPtr, HBOOL *pbIsReplace, HUINT8 *pucSize, HUINT8 *puReplaceCode)
{
	HINT16					sIndex = 0;
	HINT32					lRet = 0;


	IPEPG_RETURN_IF(pucPtr == NULL || pbIsReplace == NULL || pucSize == NULL || puReplaceCode == NULL, ERR_FAIL);

	*pbIsReplace = FALSE;
	*pucSize = 0;
	*puReplaceCode = 0;

	for( ; ; sIndex++)
	{
		if(s_astIpepgHumaxReplaceString[sIndex].szOrgStr[0] == 0)
		{
			break;
		}

		lRet = (HINT32)memcmp(pucPtr, s_astIpepgHumaxReplaceString[sIndex].szOrgStr, s_astIpepgHumaxReplaceString[sIndex].ucOrgStrLen);
		if(lRet == 0)
		{
			*pbIsReplace = TRUE;
			*pucSize = s_astIpepgHumaxReplaceString[sIndex].ucOrgStrLen;
			*puReplaceCode = s_astIpepgHumaxReplaceString[sIndex].ucReplaceCode;
			break;
		}
	}

	return ERR_OK;
}

static HERROR		local_SetDataByType(	HUINT32 ulType,
													HUINT8 *pucSrc,
													HUINT8 *pucLandCode,
													HUINT32 ulLandCodeLen,
													HUINT32 ulMaxLen,
													HUINT8 *pszString,
													HUINT32 *pulDecimal
 													)
{
	HERROR					hError = ERR_FAIL;
	HINT16					sIndex = 0;
	HINT16					sCount = 0;
	HBOOL					bReplace = FALSE;
	HUINT8					ucSize = 0;
	HUINT8					ucReplaceCode = 0;
	HUINT8					*pucTempString = NULL;


	IPEPG_RETURN_IF(pucSrc == NULL, ERR_FAIL);

	switch(ulType)
	{
		case RPHUMAX_TYPE_STRING :
			IPEPG_RETURN_IF(pszString == NULL, ERR_FAIL);

			if(pucLandCode != NULL) // it is type of UTF8 that is received data from RP SERVER.
			{
				pszString[0] = 0x15;
				ulLandCodeLen = 1; // FOR UTF8 TYPE
			}
			else
			{
				ulLandCodeLen = 0;
			}
			pucTempString = &(pszString[ulLandCodeLen]);

			for(sIndex = 0 ; sIndex<  (ulMaxLen - ulLandCodeLen) - 1 ; sIndex++)
			{
				if(pucSrc[sIndex] == '\0' || sCount >= (ulMaxLen - ulLandCodeLen))
				{
					break;
				}

				if(pucSrc[sIndex] == '\\' || pucSrc[sIndex] == '&')
				{
					bReplace = FALSE;
					ucSize = 0;
					ucReplaceCode = 0;
					hError = local_IsReplaceString(&pucSrc[sIndex], &bReplace, &ucSize, &ucReplaceCode);
					if(hError != ERR_OK)
					{
					}

					if(bReplace == TRUE)
					{
						sIndex += ucSize;
						pucTempString[sCount++] = ucReplaceCode;
						continue;
					}
				}
				pucTempString[sCount++] = pucSrc[sIndex];

			}
			pucTempString[sCount]	= '\0';
 			break;

		case RPHUMAX_TYPE_DECIMAL :
			*pulDecimal = atoi((const char *)pucSrc);
			break;

		case RPHUMAX_TYPE_DECIMAL64 :
			*pulDecimal = 0;
			break;

		case RPHUMAX_TYPE_HEXA:
			*pulDecimal = local_Hextoi(pucSrc);
			break;

		default :
			break;
	}

	return ERR_OK;
}


static HERROR	local_LoadElementList(HUINT32 ulMode, RP_HUMAX_ELEMENTLIST_t **ppstElementList)
{

	IPEPG_RETURN_IF(ppstElementList == NULL, ERR_FAIL);

	switch(ulMode)
	{
/*		case RPHUMAX_ELEMENT_LIST_SVC :
			*ppstElementList = (RPHUMAX_ELEMENTLIST_t *)&s_stElementListSvc;
			break;

		case RPHUMAX_ELEMENT_LIST_LTOT :
			*ppstElementList = (RPHUMAX_ELEMENTLIST_t *)&s_stElementListSvc;
			break;
*/
		case RPHUMAX_ELEMENT_LIST_RP :
			*ppstElementList = (RP_HUMAX_ELEMENTLIST_t *)&s_stElementListRp;
			break;

		default :
			*ppstElementList = NULL;
	}

	return ERR_OK;
}


#define	______GLOBAL_FUNC_START______


HERROR Rp_ParseRsvList(HUINT8 *pRawBuffer, HUINT32 ulBufferSize, HxList_t **ppRpListAttr, HxList_t **ppRpList)
{
	HERROR					hError = ERR_FAIL;
	HxXMLDOC 				hXmlDocHandle = NULL;
	HxXMLELEMENT 			hRootElement = NULL;
	HxXMLELEMENT 			hTempElement0 = NULL;
	HxXMLELEMENT 			hTempElement1 = NULL;
	HxXMLELEMENT 			hTempElement2 = NULL;
	HUINT32 				ulIndex = 0;
	HUINT32 				ulAttrIndex = 0;

	HUINT8					*pszTempContents= NULL;
	HUINT8					*pszAttrContents= NULL;
	RP_HUMAX_ELEMENTLIST_t	*pstElementDataList = NULL;
	RP_HUMAX_ELEMENT_t		*pstRsvListData = NULL;
	RP_HUMAX_ELEMENT_t		*pstElementData = NULL;
	RP_HUMAX_ATTRIBUTE_t		*pstAttribute = NULL;
	RP_Data_t				*pNewRpData= NULL;
	HxList_t				*pElementList1 = NULL;
	HxList_t				*pTempElementList1 = NULL;
	HxList_t				*pRpList = NULL;
	HxList_t				*pRpDataList = NULL;
	HxList_t				*pRpDataTemp = NULL;
	HxList_t				*pResponse = NULL;
	HUINT8					*pucElementName = NULL;
	HBOOL					bFound = FALSE;
	HUINT32 				ulValue = 0;
	RPListAttr_t			*pstRpListAttr = NULL;
	HxList_t				*pRpAttr = NULL;

	IPEPG_GOTO_IF( (pRawBuffer == NULL || ppRpListAttr == NULL || ppRpList == NULL || ulBufferSize == 0) , EXIT_FUNC);
	IPEPG_GOTO_IF(HLIB_XML_InitParser() != ERR_OK, EXIT_FUNC);
	IPEPG_GOTO_IF(HLIB_XML_ParseBuffer(pRawBuffer, ulBufferSize, &hXmlDocHandle), EXIT_FUNC);

	IPEPG_GOTO_IF(local_LoadElementList(RPHUMAX_ELEMENT_LIST_RP, &pstElementDataList) != ERR_OK, EXIT_FUNC);
	pstRsvListData = pstElementDataList->pstElement;
	IPEPG_GOTO_IF(pstRsvListData == NULL, EXIT_FUNC);

//**************** Getting a Request or Response *****************//
	IPEPG_GOTO_IF(HLIB_XML_GetRootElement(hXmlDocHandle, &hRootElement) != ERR_OK, EXIT_FUNC);
	if(ERR_OK != HLIB_XML_FindChildElementByName(hRootElement, pstRsvListData[RPHUMAX_RP_RPLIST].pucTagName, &pResponse))
	{
		goto EXIT_FUNC;
	}

	hTempElement0 = (HxXMLELEMENT)HLIB_LIST_Data(pResponse);
	IPEPG_GOTO_IF(hTempElement0 == NULL, EXIT_FUNC);

/***********************************************************
Adding attributes in RPLIST (version, country, count - START
***********************************************************/
	ulValue = 0;
	pstAttribute = pstRsvListData[RPHUMAX_RP_RPLIST].AttributeList;
	pstRpListAttr= (RPListAttr_t *)IPEPG_MemAlloc(sizeof(RPListAttr_t));
	IPEPG_GOTO_IF(pstRpListAttr == NULL, EXIT_FUNC);

	for(ulAttrIndex = RPHUMAX_RPLIST_ATTRIBUTE_VERSION; ulAttrIndex < RPHUMAX_RPLIST_ATTRIBUTE_MAX; ulAttrIndex++)
	{
		hError = HLIB_XML_GetAttrContents(hTempElement0, pstAttribute[ulAttrIndex].pucAttrName, &pszAttrContents);
		if(hError != ERR_OK)
		{
			 HxLOG_Debug("[PAL_XML_GetAttrContents] GetAttrContents fail (%s)!!\n", pstAttribute[ulAttrIndex].pucAttrName);
			 HLIB_XML_FreeAttrContents(pszAttrContents);
			 pszAttrContents = NULL;
			 continue;
		}
		switch(ulAttrIndex)
		{
			case RPHUMAX_RPLIST_ATTRIBUTE_VERSION :
				ulValue = 0;
				hError = local_SetDataByType(	pstAttribute[ulAttrIndex].ulAttrDataType,
													pszAttrContents,
													NULL, 0, 0, NULL,
													&ulValue);
				if(hError != ERR_OK)
				{
					HxLOG_Debug("[local_rphumaxlib_ParseToRpList] SetDataByType pstAttribute ucVersion fail!!\n");
					break;
				}
				pstRpListAttr->ucVersion = (HUINT8)ulValue;
				HxLOG_Debug("Version : [%d]\n", pstRpListAttr->ucVersion);
				break;
			case RPHUMAX_RPLIST_ATTRIBUTE_COUNTRY :
				ulValue = 0;
				hError = local_SetDataByType(	pstAttribute[ulAttrIndex].ulAttrDataType,
													pszAttrContents,
													NULL, 0, 0, NULL,
													&ulValue);
				if(hError != ERR_OK)
				{
					HxLOG_Debug("[local_rphumaxlib_ParseToRpList] SetDataByType pstAttribute ucCountry fail!!\n");
					break;
				}
				pstRpListAttr->ucCountry = (HUINT8)ulValue;
				HxLOG_Debug("Country : [%d]\n", pstRpListAttr->ucCountry);
				break;
			case RPHUMAX_RPLIST_ATTRIBUTE_COUNT :
				ulValue = 0;
				hError = local_SetDataByType(	pstAttribute[ulAttrIndex].ulAttrDataType,
													pszAttrContents,
													NULL, 0, 0, NULL,
													&ulValue);
				if(hError != ERR_OK)
				{
					HxLOG_Debug("[local_rphumaxlib_ParseToRpList] SetDataByType pstAttribute ucCount fail!!\n");
					break;
				}
				pstRpListAttr->ucCount = (HUINT8)ulValue;
				HxLOG_Debug("Count : [%d]\n", pstRpListAttr->ucCount);
				break;
			default :
				break;
		}
		HLIB_XML_FreeAttrContents(pszAttrContents);
		pszAttrContents = NULL;
	}

	pRpAttr = HLIB_LIST_Append(pRpAttr, (void*)pstRpListAttr);
	if(pRpAttr == NULL)
	{
		HxLOG_Debug("[local_rphumaxlib_ParseToRpList] pRpAttr Fail!!\n");
	}
	*ppRpListAttr = pRpAttr;

/***********************************************************
Adding attributes in RPLIST (version, country, count - FINISH
***********************************************************/
	hError = HLIB_XML_FindChildElementByName(hTempElement0, pstRsvListData[RPHUMAX_RP_RPDATA].pucTagName, &pRpDataList);
	IPEPG_GOTO_IF(hError != ERR_OK, EXIT_FUNC);
	pRpDataTemp = pRpDataList;

// RPHUMAX_RP_RPDATA
	while(pRpDataTemp != NULL)
	{
		hTempElement1 = (HxXMLELEMENT)HLIB_LIST_Data(pRpDataTemp);
		if(hTempElement1 == NULL)
		{
			HxLOG_Debug("[local_rphumaxlib_ParseToRpList] GetContents fail!!\n");
			goto EXIT_RPHUMAXLIB_PROG_LOOP;
		}

		pNewRpData = IPEPG_MemAlloc(sizeof(RP_Data_t));
		if(pNewRpData == NULL)
		{
			HxLOG_Debug("[local_rphumaxlib_ParseToRpList] pNewEvent Alloc fail!!\n");
			goto EXIT_RPHUMAXLIB_PROG_LOOP;
		}

		/************************************************
		Adding attributes in RPDATA (kind, action - START
		************************************************/
		ulValue = 0;
		pstAttribute = pstRsvListData[RPHUMAX_RP_RPDATA].AttributeList;
		for(ulAttrIndex = RPHUMAX_RPDATA_ATTRIBUTE_KIND; ulAttrIndex < RPHUMAX_RPDATA_ATTRIBUTE_MAX; ulAttrIndex++)
		{
			hError = HLIB_XML_GetAttrContents(hTempElement1, pstAttribute[ulAttrIndex].pucAttrName, &pszAttrContents);
			if(hError != ERR_OK)
			{
				HxLOG_Debug("[PAL_XML_GetAttrContents] GetAttrContents fail (%s)!!\n", pstAttribute[ulAttrIndex].pucAttrName);
				HLIB_XML_FreeAttrContents(pszAttrContents);
				pszAttrContents = NULL;
				continue;
			}
			switch(ulAttrIndex)
			{
				case RPHUMAX_RPDATA_ATTRIBUTE_KIND :
					ulValue = 0;
					hError = local_SetDataByType(	pstAttribute[ulAttrIndex].ulAttrDataType,
														pszAttrContents,
														NULL, 0, 0, NULL,
														&ulValue);
					if(hError != ERR_OK)
					{
						HxLOG_Debug("[local_rphumaxlib_ParseToRpList] SetDataByType pstAttribute usKind fail!!\n");
						break;
					}
					pNewRpData->usKind = (HUINT16)ulValue;
					break;
				case RPHUMAX_RPDATA_ATTRIBUTE_ACTION :
					ulValue = 0;
					hError = local_SetDataByType(	pstAttribute[ulAttrIndex].ulAttrDataType,
														pszAttrContents,
														NULL, 0, 0, NULL,
														&ulValue);
					if(hError != ERR_OK)
					{
						HxLOG_Debug("[local_rphumaxlib_ParseToRpList] SetDataByType pstAttribute usAction fail!!\n");
						break;
					}
					pNewRpData->usAction = (HUINT16)ulValue;
					break;
				default :
					break;
			}
			HLIB_XML_FreeAttrContents(pszAttrContents);
			pszAttrContents = NULL;
		}
		/************************************************
		Adding attributes in RPDATA (kind, action - FINISH
		************************************************/

		if(NULL != pstRsvListData[RPHUMAX_RP_RPDATA].ElementList)
		{
			pstElementData = pstRsvListData[RPHUMAX_RP_RPDATA].ElementList;
		}
		else
		{
			HxLOG_Debug("[local_rphumaxlib_ParseToRpList] Get ChildElementList fail!! \n");
			goto EXIT_RPHUMAXLIB_PROG_LOOP;
		}

		pElementList1 = NULL;
		hError = HLIB_XML_GetChildElementList(hTempElement1, &pElementList1);
		if(hError != ERR_OK)
		{
			HxLOG_Debug("[local_rphumaxlib_ParseToRpList] PAL_XML_GetChildElementList fail!!\n");
			goto EXIT_RPHUMAXLIB_PROG_LOOP;
		}

/**********************************************************************************************
*
* program count init
*
**********************************************************************************************/
//		ucLinkCount = 0;
//		ucNameCount = 0;
//		ucCategoryCount = 0;

		pTempElementList1 = pElementList1;

		while(pTempElementList1)			// Elements
		{
			hTempElement2 = (HxXMLELEMENT)HLIB_LIST_Data(pTempElementList1);
			if(hTempElement2 == NULL)
			{
				HxLOG_Debug("[local_rphumaxlib_ParseToRpList] GetContents fail!!\n");
				goto EXIT_RPHUMAXLIB_ELEMENT_LOOP1;
			}

			pucElementName = NULL;
			hError = HLIB_XML_GetElementName(hTempElement2, &pucElementName);
			if(hError != ERR_OK || pucElementName == NULL)
			{
				HxLOG_Debug("[local_rphumaxlib_ParseToRpList] PAL_XML_GetElementName fail!!\n");
				goto EXIT_RPHUMAXLIB_ELEMENT_LOOP1;
			}
			bFound = FALSE;
			ulIndex = 0;
			do{
				if(!strcmp(pstElementData[ulIndex].pucTagName, pucElementName))
				{
					bFound = TRUE;
					break;
				}
			}while(!(pstElementData[ulIndex++].bFlagEnd));

			if(!bFound)
			{
				HxLOG_Debug("[local_rphumaxlib_ParseToRpList] 1st !bFound!!\n");
				goto EXIT_RPHUMAXLIB_ELEMENT_LOOP1;
			}

			hError = HLIB_XML_GetElementContents(hXmlDocHandle, hTempElement2, &pszTempContents);
			if(hError != ERR_OK)
			{
				HxLOG_Debug("[local_rphumaxlib_ParseToRpList] PAL_XML_GetElementContents fail!!\n");
				goto EXIT_RPHUMAXLIB_ELEMENT_LOOP1;
			}

			switch(ulIndex)
			{
				case RPHUMAX_ELEMENT_RPINDEX :
					ulValue = 0;
					hError = local_SetDataByType(	pstElementData[ulIndex].ulTagDataType,
														pszTempContents,
														NULL, 0, 0, NULL,
														&ulValue);
					if(hError != ERR_OK)
					{
						HxLOG_Debug("[local_rphumaxlib_ParseToRpList] SetDataByType usRpIndex fail!!\n");
					}
					pNewRpData->usRpindex = (HUINT16)ulValue;
					break;

				case RPHUMAX_ELEMENT_SVCHANDLE :
					ulValue = 0;
					hError = local_SetDataByType(	pstElementData[ulIndex].ulTagDataType,
														pszTempContents,
														NULL, 0, 0, NULL,
														&ulValue);
					if(hError != ERR_OK)
					{
						HxLOG_Debug("[local_rphumaxlib_ParseToRpList] SetDataByType hSvc fail!!\n");
					}
					pNewRpData->hSvc = ulValue;
					break;

				case RPHUMAX_ELEMENT_UNIQUEID :
					ulValue = 0;
					hError = local_SetDataByType(	pstElementData[ulIndex].ulTagDataType,
														pszTempContents,
														NULL, 0, 0, NULL,
														&ulValue);
					if(hError != ERR_OK)
					{
						HxLOG_Debug("[local_rphumaxlib_ParseToRpList] SetDataByType usUniqueId fail!!\n");
					}
					pNewRpData->usUniqueId = ulValue;
/*
Data type - HUINT32
Event의 unique한 ID이다.
EPG kind가 TVTV인 경우에는 server에서 가지고 있는 TVTV data에서 progid의 값을 사용한다.
EPG kind가 TVTV가 아닌 경우에는 event id 값을 사용한다.
*/
					break;

				case RPHUMAX_ELEMENT_START :
					ulValue = 0;
					hError = local_SetDataByType(	pstElementData[ulIndex].ulTagDataType,
														pszTempContents,
														NULL, 0, 0, NULL,
														&ulValue);
					if(hError != ERR_OK)
					{
						HxLOG_Debug("[local_rphumaxlib_ParseToRpList] SetDataByType utStartTime fail!!\n");
					}
					pNewRpData->utStartTime= (UNIXTIME)ulValue;
					break;

				case RPHUMAX_ELEMENT_END :
					ulValue = 0;
					hError = local_SetDataByType(	pstElementData[ulIndex].ulTagDataType,
														pszTempContents,
														NULL, 0, 0, NULL,
														&ulValue);
					if(hError != ERR_OK)
					{
						HxLOG_Debug("[local_rphumaxlib_ParseToRpList] SetDataByType utEndTime fail!!\n");
					}
					pNewRpData->utEndTime = (UNIXTIME)ulValue;
					break;

				case RPHUMAX_ELEMENT_RSVTYPE :
					ulValue = 0;
					hError = local_SetDataByType(	pstElementData[ulIndex].ulTagDataType,
														pszTempContents,
														NULL, 0, 0, NULL,
														&ulValue);
					if(hError != ERR_OK)
					{
						HxLOG_Debug("[local_rphumaxlib_ParseToRpList] SetDataByType usRsvType fail!!\n");
					}


					pNewRpData->usRsvType = (HUINT16)ulValue;
					break;

				case RPHUMAX_ELEMENT_REPEAT :
					ulValue = 0;
					hError = local_SetDataByType(	pstElementData[ulIndex].ulTagDataType,
														pszTempContents,
														NULL, 0, 0, NULL,
														&ulValue);
					if(hError != ERR_OK)
					{
						HxLOG_Debug("[local_rphumaxlib_ParseToRpList] SetDataByType ucRepeat fail!!\n");
					}

					pNewRpData->ucRepeat = (HUINT8)ulValue; 	// XsvcEpg_EpgHumaxDrm_e
					break;

				default :
					break;
			}
EXIT_RPHUMAXLIB_ELEMENT_LOOP1 :
			pTempElementList1 = pTempElementList1->next;
			if(pTempElementList1 == NULL || pElementList1 == pTempElementList1)
			{
				break;
			}
		}

EXIT_RPHUMAXLIB_PROG_LOOP :
		pRpList = HLIB_LIST_Append(pRpList, (void*)pNewRpData);
		if(pRpList == NULL)
		{
			HxLOG_Debug("[local_rphumaxlib_ParseToRpList] AppendListItemToPointerList Fail!!\n");
		}

		pRpDataTemp = pRpDataTemp->next;
		if(pRpDataTemp == NULL || pRpDataTemp == pRpDataList)
		{
			HxLOG_Debug("[local_rphumaxlib_ParseToRpList] Loop end !!\n");
			break;
		}
		pElementList1 = HLIB_LIST_RemoveAll(pElementList1);

	}
	pRpDataList = HLIB_LIST_RemoveAll(pRpDataList);
	(void)pRpDataList;
	hError = ERR_OK;
	*ppRpList = pRpList;

EXIT_FUNC :

	HLIB_LIST_RemoveAll(pElementList1);
	HLIB_LIST_RemoveAll(pResponse);

	if(hXmlDocHandle != NULL)
	{
		HLIB_XML_FreeDocument(hXmlDocHandle);
	}
	return hError;
}



/*********************** End of File ******************************/


