/**
	@file     otl_xml.c
	@brief   libxml interface function file

	Description: libxml interface function file		\n
	Module: XML API			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <ctype.h>
#include <octo_common.h>
#include <vkernel.h>

#include <libxml/xmlmemory.h>

#include "linkedlist.h"
#include "otl_xml.h"

#if defined(CONFIG_TEMP_OS_EMULATOR)
#define xmlFree	free
#endif

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/
HUINT32 			g_XML_Level = 0x0000;



/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/




#define			XML_ELEMENT_NAME_MAX		64


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
#if asdfas
/** @brief struct에 대한 간단한 설명  */
typedef struct
{
    HUINT32        member1; 		/**< member1에 대한 comment      */
	HUINT32        member2;  		/**< member2에 대한 comment      */
}MODULE_DoWhat_t ;

/** @brief enummeration에 대한 간단한 설명. */
typedef enum
{
    eModule_DoWhat0=0, 				/**< Value 설명	*/
    eModule_DoWhat1 				/**< Value 설명	*/
}AV_DualMonoMode_;
#endif


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      extern functions     ********************/
/*******************************************************************/
extern	xmlParserCtxtPtr	xmlCreateMemoryParserCtxt	(const char * buffer, int size);


/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

#if 0
static HUINT8		local_xml_UpperCase(HUINT8 ch);
static void		local_xml_ConvertStringUpper(HUINT8 *pszString, HUINT32 ulMaxSize);
static HERROR		local_xml_CopyByteUntilNull(HUINT8 *pucDest, HUINT8 *pucSrc, HUINT32 ulMaxSize);
#endif // #if 0
static HERROR		local_xml_InitParser(void);
static HERROR		local_xml_ParseBuffer(HUINT8 *pucBuffer, HUINT32 ulBufferSize, HXMLDOC *phDocument);
static HERROR		local_xml_ParseFile(HUINT8 *pszFileName, HXMLDOC *phDocument);
static HERROR		local_xml_Validate(HUINT32 ulMode, HUINT8 *pszDTDFileName, HUINT8 *pucDTDStream, HUINT32 ulStreamSize, HXMLDOC hDocument) ;
static HERROR		local_xml_ParseDTDFile(HUINT8 *pszDtdFileName, HXMLDTD *phDtdHandle);
static HERROR		local_xml_ParseDTDBuffer(HUINT8 *pucDtdBuffer, HUINT32 ulBufferSize, HXMLDTD *phDtdHandle);
static HERROR		local_xml_FindChildElementByName(HXMLELEMENT hParent, HUINT8 *pucName, POINTER_LIST_T **ppElementList);
static HERROR		local_xml_GetChildElementList(HXMLELEMENT hParent, POINTER_LIST_T **ppElementList);
static HERROR		local_xml_GetRootElement(HXMLDOC hDocHandle, HXMLELEMENT *phRootHandle);
static HERROR		local_xml_GetChildElement(HXMLELEMENT hElementHandle, HXMLELEMENT *phChildHandle);
static HERROR		local_xml_GetNextElement(HXMLELEMENT hElementHandle, HXMLELEMENT *phNextHandle);
static HERROR		local_xml_GetElementName(HXMLELEMENT hElementHandle, HUINT8 **ppucName);
static HERROR		local_xml_GetElementContents(HXMLDOC hDocHandle, HXMLELEMENT hElementHandle, HUINT8 **ppucContents);
static HERROR		local_xml_GetAttrContents(HXMLELEMENT hElementHandle, HUINT8* pszName, HUINT8 **ppucContents);
static void			local_xml_FreeAttrContents (HUINT8 *pucContents);
static HERROR		local_xml_FreeDocument(HXMLDOC hDocHandle);






#define ______GLOBAL_XML_FUNC______

HERROR		PAL_XML_ParseFile(HUINT8 *pszFileName, HXMLDOC *phDocument)
{
	HERROR			hError = ERR_XML_FAIL;

	if(pszFileName == NULL || phDocument == NULL)
	{
		HxLOG_Print("[PAL_XML_ParseFile] Param NULL!!\n");
		goto EXIT_XML_FUNC;
	}

	hError = local_xml_ParseFile(pszFileName, phDocument);
	if (hError != ERR_XML_OK)
	{
		HxLOG_Print("[PAL_XML_ParseFile] ParseFile error!!\n");
		goto EXIT_XML_FUNC;
	}

    	hError = ERR_XML_OK;

EXIT_XML_FUNC :

	return hError;
}



HERROR		PAL_XML_ParseBuffer(HUINT8 *pucBuffer, HUINT32 ulBufferSize, HXMLDOC *phDocument)
{
	HERROR			hError = ERR_XML_FAIL;

	if(pucBuffer == NULL || phDocument == NULL)
	{
		HxLOG_Print("[PAL_XML_ParseBuffer] Param NULL!!\n");
		goto EXIT_XML_FUNC;
	}

	hError = local_xml_ParseBuffer(pucBuffer, ulBufferSize, phDocument);
	if (hError != ERR_XML_OK)
	{
		HxLOG_Print("[PAL_XML_ParseBuffer] ParseBuffer error!!\n");
		goto EXIT_XML_FUNC;
	}

    	hError = ERR_XML_OK;

EXIT_XML_FUNC :

	return hError;
}




HERROR		PAL_XML_Validate(HUINT32 ulMode, HUINT8 *pszDTD, HUINT8 *pucDtdBuffer, HUINT32 ulBufferSize, HXMLDOC hDocument)
{
	HERROR			hError = ERR_XML_FAIL;

	hError = local_xml_Validate(ulMode, pszDTD, pucDtdBuffer, ulBufferSize, hDocument);
	if (hError != ERR_XML_OK)
	{
		HxLOG_Print("[PAL_XML_ValidateFile] ValidateFile error!!\n");
		goto EXIT_XML_FUNC;
	}

    	hError = ERR_XML_OK;

EXIT_XML_FUNC :

	return hError;
}



HERROR		PAL_XML_InitParser(void)
{
	HERROR			hError = ERR_XML_FAIL;

	hError = local_xml_InitParser();
	if (hError != ERR_XML_OK)
	{
		HxLOG_Print("[PAL_XML_InitParser] InitParser error!!\n");
		goto EXIT_XML_FUNC;
	}

    	hError = ERR_XML_OK;

EXIT_XML_FUNC :

	return hError;
}


HERROR		PAL_XML_GetRootElement(HXMLDOC hDocHandle, HXMLELEMENT *phRootHandle)
{
	HERROR			hError = ERR_XML_FAIL;

	if(hDocHandle == NULL || phRootHandle == NULL)
	{
		HxLOG_Print("[PAL_XML_GetRootElement] Param NULL!!\n");
		goto EXIT_XML_FUNC;
	}

	hError = local_xml_GetRootElement(hDocHandle, phRootHandle);
	if (hError != ERR_XML_OK)
	{
		HxLOG_Print("[PAL_XML_GetRootElement] GetRootElement error!!\n");
		goto EXIT_XML_FUNC;
	}

    	hError = ERR_XML_OK;

EXIT_XML_FUNC :

	return hError;
}



HERROR		PAL_XML_FindChildElementByName(HXMLELEMENT hParent, HUINT8 *pucName, POINTER_LIST_T **ppElementList)
{
	HERROR			hError = ERR_XML_FAIL;

	if(hParent == NULL || pucName == NULL || ppElementList == NULL)
	{
		HxLOG_Print("[PAL_XML_FindChildElementByName] Param NULL!!\n");
		goto EXIT_XML_FUNC;
	}

	hError = local_xml_FindChildElementByName(hParent, pucName, ppElementList);
	if (hError != ERR_XML_OK)
	{
		HxLOG_Print("[PAL_XML_FindChildElementByName] FindChildElementByName error!!\n");
		goto EXIT_XML_FUNC;
	}

    	hError = ERR_XML_OK;

EXIT_XML_FUNC :

	return hError;

}


HERROR		PAL_XML_GetChildElementList(HXMLELEMENT hParent, POINTER_LIST_T **ppElementList)
{
	HERROR			hError = ERR_XML_FAIL;

	if(hParent == NULL || ppElementList == NULL)
	{
		HxLOG_Print("[PAL_XML_GetChildElementList] Param NULL!!\n");
		goto EXIT_XML_FUNC;
	}

	hError = local_xml_GetChildElementList(hParent, ppElementList);
	if (hError != ERR_XML_OK)
	{
		HxLOG_Print("[PAL_XML_GetChildElementList] local_xml_GetChildElementList error!!\n");
		goto EXIT_XML_FUNC;
	}

    	hError = ERR_XML_OK;

EXIT_XML_FUNC :

	return hError;

}



HERROR		PAL_XML_GetElementContents(HXMLDOC hDocHandle, HXMLELEMENT hElementHandle, HUINT8 **ppucContents)
{
	HERROR			hError = ERR_XML_FAIL;

	if(hDocHandle == NULL || hElementHandle == NULL || ppucContents == NULL)
	{
		HxLOG_Print("[PAL_XML_GetElementContents] Param NULL!!\n");
		goto EXIT_XML_FUNC;
	}

	// alloc & assign
	hError = local_xml_GetElementContents(hDocHandle, hElementHandle, ppucContents);
	if (hError != ERR_XML_OK)
	{
		HxLOG_Print("[PAL_XML_GetElementContents] GetElementContents error!!\n");
		goto EXIT_XML_FUNC;
	}

    	hError = ERR_XML_OK;

EXIT_XML_FUNC :

	return hError;
}



HERROR		PAL_XML_GetNextElement(HXMLELEMENT hElementHandle, HXMLELEMENT *phNextHandle)
{
	HERROR			hError = ERR_XML_FAIL;


	if(hElementHandle == NULL || phNextHandle == NULL)
	{
		HxLOG_Print("[PAL_XML_GetNextElement] Param NULL!!\n");
		goto EXIT_XML_FUNC;
	}

	hError = local_xml_GetNextElement(hElementHandle, phNextHandle);
	if (hError != ERR_XML_OK)
	{
		HxLOG_Print("[PAL_XML_GetNextElement] GetNextElement error!!\n");
		goto EXIT_XML_FUNC;
	}

    	hError = ERR_XML_OK;

EXIT_XML_FUNC :

	return hError;
}


HERROR		PAL_XML_GetChildElement(HXMLELEMENT hElementHandle, HXMLELEMENT *phChildHandle)
{
	HERROR			hError = ERR_XML_FAIL;


	if(hElementHandle == NULL || phChildHandle == NULL)
	{
		HxLOG_Print("[PAL_XML_GetChildElement] Param NULL!!\n");
		goto EXIT_XML_FUNC;
	}

	hError = local_xml_GetChildElement(hElementHandle, phChildHandle);
	if (hError != ERR_XML_OK)
	{
		HxLOG_Print("[PAL_XML_GetChildElement] GetChildElement error!!\n");
		goto EXIT_XML_FUNC;
	}

    	hError = ERR_XML_OK;

EXIT_XML_FUNC :

	return hError;
}


HERROR		PAL_XML_GetElementName(HXMLELEMENT hElementHandle, HUINT8 **ppucName)
{
	HERROR			hError = ERR_XML_FAIL;


	if(hElementHandle == NULL || ppucName == NULL)
	{
		HxLOG_Print("[PAL_XML_GetElementName] Param NULL!!\n");
		goto EXIT_XML_FUNC;
	}

	hError = local_xml_GetElementName(hElementHandle, ppucName);
	if (hError != ERR_XML_OK)
	{
		HxLOG_Print("[PAL_XML_GetElementName] local_xml_GetElementName error!!\n");
		goto EXIT_XML_FUNC;
	}

    	hError = ERR_XML_OK;

EXIT_XML_FUNC :

	return hError;
}



HERROR		PAL_XML_GetAttrContents(HXMLELEMENT hElementHandle, HUINT8* pszName, HUINT8 **ppucContents)
{
	HERROR			hError = ERR_XML_FAIL;

	if(hElementHandle == NULL || pszName == NULL)
	{
		HxLOG_Print("[PAL_XML_GetAttrContents] Param NULL!!\n");
		goto EXIT_XML_FUNC;
	}

	// alloc & assign
	hError = local_xml_GetAttrContents(hElementHandle, pszName, ppucContents);
	if (hError != ERR_XML_OK)
	{
		HxLOG_Print("[PAL_XML_GetAttrContents] GetAttrContents fail!!\n");
		goto EXIT_XML_FUNC;
	}

    	hError = ERR_XML_OK;

EXIT_XML_FUNC :

	return hError;
}

void		PAL_XML_FreeAttrContents (HUINT8 *pucContents)
{
	local_xml_FreeAttrContents (pucContents);
}


HERROR		PAL_XML_FreeDocument(HXMLDOC hDocHandle)
{
	HERROR			hError = ERR_XML_FAIL;

	if(hDocHandle == NULL)
	{
		HxLOG_Print("[PAL_XML_FreeDocument] Param NULL!!\n");
		goto EXIT_XML_FUNC;
	}

	hError = local_xml_FreeDocument(hDocHandle);
	if(hError != ERR_XML_OK)
	{
		HxLOG_Print("[PAL_XML_FreeDocument] FreeDocument fail!!\n");
		goto EXIT_XML_FUNC;
	}

    	hError = ERR_XML_OK;

EXIT_XML_FUNC :

	return hError;

}




#define ______LOCAL_XML_FUNC______


#if 0
static HUINT8		local_xml_UpperCase(HUINT8 ch)
{
	if ( isalpha(ch) )
	{
		if ( ch >= 'a' && ch <= 'z' )
		{
			ch += (HUINT8)('A' - 'a') ;
		}
	}

	return ch ;
}

static void		local_xml_ConvertStringUpper(HUINT8 *pszString, HUINT32 ulMaxSize)
{
	HINT32	lLen = 0;
	HINT32	lIndex = 0 ;

	for (lIndex = 0 ; lIndex <  ulMaxSize; lIndex++ )
	{
		if(pszString[lIndex] == '\0')
		{
			break;
		}
	}
	lLen = lIndex;

	for ( lIndex = 0 ; lIndex < lLen ; lIndex++ )
	{
		pszString[lIndex] = local_xml_UpperCase(pszString[lIndex]) ;
	}
}


static HERROR		local_xml_CopyByteUntilNull(HUINT8 *pucDest, HUINT8 *pucSrc, HUINT32 ulMaxSize)
{
	HERROR			hError = ERR_XML_FAIL;
	HUINT16			usIndex = 0;

	if(pucDest == NULL || pucSrc == NULL)
	{
		HxLOG_Print("[local_xml_CopyByteUntilNull] Param NULL!!\n");
		goto EXIT_XML_FUNC;
	}


	for(usIndex = 0; usIndex < ulMaxSize-1; usIndex++)
	{
		pucDest[usIndex] = pucSrc[usIndex];

		if(pucSrc[usIndex] == '\0')
		{
			break;
		}
	}
	pucDest[usIndex] = '\0';

	hError = ERR_XML_OK;

EXIT_XML_FUNC :

	return hError;
}
#endif



/**
  @brief     LAYER_MODULE_DoSomething 함수.

  simple comment \n

  @return    return 값.
  */
static HERROR		local_xml_InitParser(void)
{
 	xmlInitParser();

	return ERR_XML_OK;
}



static HERROR		local_xml_ParseBuffer(HUINT8 *pucBuffer, HUINT32 ulBufferSize, HXMLDOC *phDocument)
{
	xmlDocPtr		pstDocument = (xmlDocPtr)NULL;
	HERROR			hError = ERR_XML_FAIL;

	if(pucBuffer == NULL || phDocument == NULL)
	{
		HxLOG_Print("[local_xml_ParseBuffer] Param NULL!!\n");
		goto EXIT_XML_FUNC;
	}

	pstDocument = xmlParseMemory((char*)pucBuffer, (int)ulBufferSize);
	if(pstDocument == NULL)
	{
		HxLOG_Print("[local_xml_ParseBuffer] pstDocument NULL!!\n");
		goto EXIT_XML_FUNC;
	}
	*phDocument = (HXMLDOC)pstDocument;
    	hError = ERR_XML_OK;

EXIT_XML_FUNC :

	return hError;
}




static HERROR		local_xml_ParseFile(HUINT8 *pszFileName, HXMLDOC *phDocument)
{
	xmlDocPtr		pstDocument = (xmlDocPtr)NULL;
	HERROR			hError = ERR_XML_FAIL;

	if(pszFileName == NULL || phDocument == NULL)
	{
		HxLOG_Print("[local_xml_ParseFile] Param NULL!!\n");
		goto EXIT_XML_FUNC;
	}

	pstDocument = xmlParseFile((const char*)pszFileName);
	if(pstDocument == NULL)
	{
		HxLOG_Print("[local_xml_ParseFile] pstDocument NULL!!\n");
		goto EXIT_XML_FUNC;
	}
	*phDocument = (HXMLDOC)pstDocument;
    	hError = ERR_XML_OK;

EXIT_XML_FUNC :

	return hError;
}






static HERROR		local_xml_Validate(HUINT32 ulMode, HUINT8 *pszDTDFileName, HUINT8 *pucDTDStream, HUINT32 ulStreamSize, HXMLDOC hDocument)
{
	xmlValidCtxt		vctxt;
	HERROR			hError = ERR_XML_FAIL;
	xmlDocPtr		pstDocument = (xmlDocPtr)NULL;
	HINT32			ulRet = 0;
	HXMLDTD		hDtdHandle = NULL;


	if(ulMode & XML_VALIDATE_BUFFER)
	{
		if(pucDTDStream == NULL || hDocument == NULL)
		{
			HxLOG_Print("[local_xml_Validate] Param NULL!!\n");
			goto EXIT_XML_FUNC;
		}

		hError = local_xml_ParseDTDBuffer(pucDTDStream, ulStreamSize, &hDtdHandle);
		if (hError != ERR_XML_OK)
		{
			HxLOG_Print("[local_xml_Validate] ParseDTDBuffer error!!\n");
			goto EXIT_XML_FUNC;
		}
	}
	else
	{

		if(pszDTDFileName == NULL || hDocument == NULL)
		{
			HxLOG_Print("[local_xml_Validate] Param NULL!!\n");
			goto EXIT_XML_FUNC;
		}

		hError = local_xml_ParseDTDFile(pszDTDFileName, &hDtdHandle);
		if (hError != ERR_XML_OK)
		{
			HxLOG_Print("[local_xml_Validate] ParseDTDFile error!!\n");
			goto EXIT_XML_FUNC;
		}
	}


	pstDocument = (xmlDocPtr)hDocument;

	vctxt.userData = stderr;
	vctxt.error = (xmlValidityErrorFunc)fprintf;
	vctxt.warning = (xmlValidityWarningFunc)fprintf;

	//if 0, it is not valid
    	hError = ERR_XML_OK;
	ulRet = (int)xmlValidateDtd(&vctxt, pstDocument, (xmlDtdPtr)hDtdHandle);
	if(ulRet == 0)
	{
		HxLOG_Print("[local_xml_Validate] DTD result error!!\n");
		hError = ERR_XML_FAIL;
	}

EXIT_XML_FUNC :

	xmlFreeDtd(hDtdHandle);

	return hError;
}





static HERROR		local_xml_ParseDTDFile(HUINT8 *pszDtdFileName, HXMLDTD *phDtdHandle)
{
	HERROR			hError = ERR_XML_FAIL;
	xmlDtdPtr		pDtd = NULL;


	if(pszDtdFileName == NULL || phDtdHandle == NULL)
	{
		HxLOG_Print("[local_xml_ParseDTDFile] Param NULL!!\n");
		goto EXIT_XML_FUNC;
	}

	pDtd = xmlParseDTD(NULL, (xmlChar *)pszDtdFileName);
	if (pDtd == NULL)
	{
		HxLOG_Print("[local_xml_ParseDTDFile] DTD Parsing fail!!\n");
		goto EXIT_XML_FUNC;
	}

	*phDtdHandle =(HXMLDTD)pDtd;

    	hError = ERR_XML_OK;

EXIT_XML_FUNC :

	return hError;

}



static HERROR		local_xml_ParseDTDBuffer(HUINT8 *pucDtdBuffer, HUINT32 ulBufferSize, HXMLDTD *phDtdHandle)
{
	xmlParserCtxtPtr	ctxt = NULL;
	HERROR			hError = ERR_XML_FAIL;
	HINT32			lRet = 0;


	if(pucDtdBuffer == NULL || phDtdHandle == NULL)
	{
		HxLOG_Print("[local_xml_ParseDTDBuffer] Param NULL!!\n");
		goto EXIT_XML_FUNC;
	}

	ctxt = xmlCreateMemoryParserCtxt((const char*)pucDtdBuffer, (int)ulBufferSize);
	if (ctxt == NULL)
	{
		HxLOG_Print("[local_xml_ParseDTDBuffer] CreateMemoryParserCtxtt error!!\n");
		goto EXIT_XML_FUNC;
	}

	lRet = (int)xmlParseDocument(ctxt);
	if(lRet != ERR_XML_OK)
	{
		HxLOG_Print("[local_xml_ParseDTDBuffer] ParseDocument fail!!\n");
		goto EXIT_XML_FUNC;
	}
	if (!ctxt->wellFormed)
	{
		HxLOG_Print("[local_xml_ParseDTDBuffer] not wellFormed!!\n");
		goto EXIT_XML_FUNC;
	}

	*phDtdHandle = (HXMLDTD)ctxt->myDoc->intSubset;
	ctxt->myDoc->intSubset = NULL;

	xmlFreeDoc(ctxt->myDoc);
	ctxt->myDoc = NULL;

	hError = ERR_XML_OK;

EXIT_XML_FUNC :

	xmlFreeParserCtxt(ctxt);
	ctxt = NULL;

	return hError;

}

#if 0
static HERROR		local_xml_FindChildElementByName(HXMLELEMENT hParent, HUINT8 *pucName, POINTER_LIST_T **ppElementList)
{
	HERROR			hError = ERR_XML_FAIL;
	HXMLELEMENT		hTempNode = NULL;
	HUINT8			*pucTempName = NULL;
	HUINT8			szTempUpperName[XML_ELEMENT_NAME_MAX];
	HUINT8			szUpperName[XML_ELEMENT_NAME_MAX];
	POINTER_LIST_T	*pElementList = NULL;

	if(hParent == NULL || pucName == NULL || ppElementList == NULL)
	{
		HxLOG_Print("[local_xml_FindElementNodeByName] Param NULL!!\n");
		goto EXIT_XML_FUNC;
	}
	*ppElementList = NULL;

	HxSTD_memset(szTempUpperName, 0x00, sizeof(HUINT8)*XML_ELEMENT_NAME_MAX);
	HxSTD_memset(szUpperName, 0x00, sizeof(HUINT8)*XML_ELEMENT_NAME_MAX);

	hError = local_xml_GetChildElement(hParent, &hTempNode);
	if(hError != ERR_XML_OK)
	{
		HxLOG_Print("[local_xml_FindElementNodeByName] GetChildElement fail!!\n");
		goto EXIT_XML_FUNC;
	}

	hError = local_xml_CopyByteUntilNull(szUpperName, pucName, XML_ELEMENT_NAME_MAX);
	if(hError != ERR_XML_OK)
	{
		HxLOG_Print("[local_xml_FindElementNodeByName] CopyByteUntilNull fail!!\n");
		goto EXIT_XML_FUNC;
	}
	local_xml_ConvertStringUpper(szUpperName, XML_ELEMENT_NAME_MAX);

	while(hTempNode != NULL)
	{
		hError = local_xml_GetElementName(hTempNode, &pucTempName);
		if(hError != ERR_XML_OK)
		{
			HxLOG_Print("[local_xml_FindElementNodeByName] GetElementName fail!!\n");
			continue;
		}

		hError = local_xml_CopyByteUntilNull(szTempUpperName, pucTempName, XML_ELEMENT_NAME_MAX);
		if(hError != ERR_XML_OK)
		{
			HxLOG_Print("[local_xml_FindElementNodeByName] CopyByteUntilNull fail!!\n");
			goto EXIT_XML_FUNC;
		}
		local_xml_ConvertStringUpper(szTempUpperName, XML_ELEMENT_NAME_MAX);
		pucTempName = NULL;

		if(!strcmp(szTempUpperName, szUpperName) )
		{
			pElementList = UTIL_LINKEDLIST_AppendListItemToPointerList(pElementList, (void*)hTempNode);
			if(pElementList == NULL)
			{
				HxLOG_Print("[local_xml_FindElementNodeByName] AppendListItem fail!!\n");
			}
		}

		hError = local_xml_GetNextElement(hTempNode, &hTempNode);
		if(hError != ERR_XML_OK)
		{
			HxLOG_Print("[local_xml_FindElementNodeByName] GetNextElement NULL!!\n");
		}
	}

	if(pElementList != NULL)
	{
		*ppElementList = pElementList;
		hError = ERR_XML_OK;
	}


EXIT_XML_FUNC :

	return hError;


}

#else
static HERROR		local_xml_FindChildElementByName(HXMLELEMENT hParent, HUINT8 *pucName, POINTER_LIST_T **ppElementList)
{
	HERROR			hError = ERR_XML_FAIL;
	HXMLELEMENT		hTempNode = NULL;
	HUINT8			*pucTempName = NULL;
	POINTER_LIST_T	*pElementList = NULL;

	if(hParent == NULL || pucName == NULL || ppElementList == NULL)
	{
		HxLOG_Print("[local_xml_FindElementNodeByName] Param NULL!!\n");
		goto EXIT_XML_FUNC;
	}
	*ppElementList = NULL;

	hError = local_xml_GetChildElement(hParent, &hTempNode);
	if(hError != ERR_XML_OK)
	{
		HxLOG_Print("[local_xml_FindElementNodeByName] GetChildElement fail!!\n");
		goto EXIT_XML_FUNC;
	}

	while(hTempNode != NULL)
	{
		pucTempName = NULL;
		hError = local_xml_GetElementName(hTempNode, &pucTempName);
		if(hError != ERR_XML_OK || pucTempName == NULL)
		{
			HxLOG_Print("[local_xml_FindElementNodeByName] GetElementName fail!!\n");

			hError = local_xml_GetNextElement(hTempNode, &hTempNode);
			if(hError != ERR_XML_OK)
			{
				HxLOG_Print("[local_xml_FindElementNodeByName] GetNextElement NULL!!\n");
			}
			continue;
		}

		if(!strcmp(pucTempName, pucName) )
		{
			pElementList = UTIL_LINKEDLIST_AppendListItemToPointerList(pElementList, (void*)hTempNode);
			if(pElementList == NULL)
			{
				HxLOG_Print("[local_xml_FindElementNodeByName] AppendListItem fail!!\n");
			}
		}

		hError = local_xml_GetNextElement(hTempNode, &hTempNode);
		if(hError != ERR_XML_OK)
		{
			HxLOG_Print("[local_xml_FindElementNodeByName] GetNextElement NULL!!\n");
		}
	}

	if(pElementList != NULL)
	{
		*ppElementList = pElementList;
		hError = ERR_XML_OK;
	}


EXIT_XML_FUNC :

	return hError;
}
#endif




static HERROR		local_xml_GetChildElementList(HXMLELEMENT hParent, POINTER_LIST_T **ppElementList)
{
	HERROR			hError = ERR_XML_FAIL;
	HXMLELEMENT		hTempNode = NULL;
	POINTER_LIST_T	*pElementList = NULL;

	if(hParent == NULL || ppElementList == NULL)
	{
		HxLOG_Print("[local_xml_FindElementNodeByName] Param NULL!!\n");
		goto EXIT_XML_FUNC;
	}
	*ppElementList = NULL;

	hError = local_xml_GetChildElement(hParent, &hTempNode);
	if(hError != ERR_XML_OK)
	{
		HxLOG_Print("[local_xml_FindElementNodeByName] GetChildElement fail!!\n");
		goto EXIT_XML_FUNC;
	}

	while(hTempNode != NULL)
	{
		pElementList = UTIL_LINKEDLIST_AppendListItemToPointerList(pElementList, (void*)hTempNode);
		if(pElementList == NULL)
		{
			HxLOG_Print("[local_xml_FindElementNodeByName] AppendListItem fail!!\n");
		}
		hError = local_xml_GetNextElement(hTempNode, &hTempNode);
		if(hError != ERR_XML_OK)
		{
			HxLOG_Print("[local_xml_FindElementNodeByName] GetNextElement NULL!!\n");
		}
	}

	if(pElementList != NULL)
	{
		*ppElementList = pElementList;
		hError = ERR_XML_OK;
	}


EXIT_XML_FUNC :

	return hError;
}








static HERROR		local_xml_GetRootElement(HXMLDOC hDocHandle, HXMLELEMENT *phRootHandle)
{
	HERROR			hError = ERR_XML_FAIL;
	xmlDocPtr		pstDocument = (xmlDocPtr)NULL;
	xmlNodePtr		pstRootNode = NULL;


	if(hDocHandle == NULL || phRootHandle == NULL)
	{
		HxLOG_Print("[local_xml_GetRootElement] Param NULL!!\n");
		goto EXIT_XML_FUNC;
	}
	*phRootHandle = NULL;

	pstDocument = (xmlDocPtr)hDocHandle;
	pstRootNode = xmlDocGetRootElement(pstDocument);
	if(pstRootNode == NULL)
	{
		HxLOG_Print("[local_xml_GetRootElement] RootElement NULL!!\n");
		goto EXIT_XML_FUNC;

	}

	*phRootHandle = (HXMLELEMENT)pstRootNode;
    	hError = ERR_XML_OK;

EXIT_XML_FUNC :


	return hError;
}


static HERROR		local_xml_GetChildElement(HXMLELEMENT hElementHandle, HXMLELEMENT *phChildHandle)
{
	HERROR			hError = ERR_XML_FAIL;
	xmlNodePtr		pstParent = NULL;


	if(hElementHandle == NULL || phChildHandle == NULL)
	{
		HxLOG_Print("[local_xml_GetChildElement] Param NULL!!\n");
		goto EXIT_XML_FUNC;
	}
	*phChildHandle = NULL;

	pstParent = (xmlNodePtr)hElementHandle;
	if(pstParent->children == NULL)
	{
		HxLOG_Print("[local_xml_GetChildElement] ChildElement NULL!!\n");
		goto EXIT_XML_FUNC;
	}

	*phChildHandle = (HXMLELEMENT)pstParent->children;
    	hError = ERR_XML_OK;

EXIT_XML_FUNC :


	return hError;
}




static HERROR		local_xml_GetNextElement(HXMLELEMENT hElementHandle, HXMLELEMENT *phNextHandle)
{
	HERROR			hError = ERR_XML_FAIL;
	xmlNodePtr		pstCurrent = NULL;


	if(hElementHandle == NULL || phNextHandle == NULL)
	{
		HxLOG_Print("[local_xml_GetNextElement] Param NULL!!\n");
		goto EXIT_XML_FUNC;
	}
	*phNextHandle = NULL;

	pstCurrent = (xmlNodePtr)hElementHandle;
	if(pstCurrent->next == NULL)
	{
		HxLOG_Print("[local_xml_GetNextElement] ChildElement NULL!!\n");
		goto EXIT_XML_FUNC;
	}

	*phNextHandle = (HXMLELEMENT)pstCurrent->next;
    	hError = ERR_XML_OK;

EXIT_XML_FUNC :

	return hError;
}



static HERROR		local_xml_GetElementName(HXMLELEMENT hElementHandle, HUINT8 **ppucName)
{
	HERROR			hError = ERR_XML_FAIL;
	xmlNodePtr		pstCurrent = NULL;


	if(hElementHandle == NULL || ppucName == NULL)
	{
		HxLOG_Print("[local_xml_GetElementName] Param NULL!!\n");
		goto EXIT_XML_FUNC;
	}
	*ppucName = NULL;

	pstCurrent = (xmlNodePtr)hElementHandle;
	if(pstCurrent->name == NULL)
	{
		HxLOG_Print("[local_xml_GetElementName] name NULL!!\n");
		goto EXIT_XML_FUNC;
	}

	*ppucName = (HUINT8*)pstCurrent->name;
    	hError = ERR_XML_OK;

EXIT_XML_FUNC :

	return hError;
}




static HERROR		local_xml_GetElementContents(HXMLDOC hDocHandle, HXMLELEMENT hElementHandle, HUINT8 **ppucContents)
{
	HERROR			hError = ERR_XML_FAIL;
	xmlDocPtr		pstDocument = (xmlDocPtr)NULL;
	xmlNodePtr		pstCurrent = NULL;


	if(hDocHandle == NULL || hElementHandle == NULL)
	{
		HxLOG_Print("[local_xml_GetElementContents] Param NULL!!\n");
		goto EXIT_XML_FUNC;
	}
	*ppucContents = NULL;


	pstDocument = (xmlDocPtr)hDocHandle;
	pstCurrent = (xmlNodePtr)hElementHandle;

	// alloc & assign
	*ppucContents = xmlNodeListGetString(pstDocument, pstCurrent->children, 1);
	if(*ppucContents == NULL)
	{
		HxLOG_Print("[local_xml_GetElementContents] pucContents NULL!!\n");
		goto EXIT_XML_FUNC;
	}

    	hError = ERR_XML_OK;

EXIT_XML_FUNC :

	return hError;
}



static HERROR		local_xml_GetAttrContents(HXMLELEMENT hElementHandle, HUINT8* pszName, HUINT8 **ppucContents)
{
	HERROR			hError = ERR_XML_FAIL;
	xmlNodePtr		pstCurrent = NULL;


	if(hElementHandle == NULL || pszName == NULL)
	{
		HxLOG_Print("[local_xml_GetAttrContents] Param NULL!!\n");
		goto EXIT_XML_FUNC;
	}
	*ppucContents = NULL;

	pstCurrent = (xmlNodePtr)hElementHandle;

	// alloc & assign
	*ppucContents = xmlGetProp(pstCurrent, pszName);
	if(*ppucContents == NULL)
	{
		HxLOG_Print("[local_xml_GetAttrContents] pucContents NULL!!\n");
		goto EXIT_XML_FUNC;
	}

    	hError = ERR_XML_OK;

EXIT_XML_FUNC :

	return hError;
}


static void			local_xml_FreeAttrContents (HUINT8 *pucContents)
{
	if (pucContents != NULL)
	{
		xmlFree (pucContents);
	}
}


static HERROR		local_xml_FreeDocument(HXMLDOC hDocHandle)
{
	HERROR			hError = ERR_XML_FAIL;

	if(hDocHandle == NULL)
	{
		HxLOG_Print("[local_xml_FreeDocument] Param NULL!!\n");
		goto EXIT_XML_FUNC;
	}

	xmlFreeDoc((xmlDocPtr)hDocHandle);

    	hError = ERR_XML_OK;

EXIT_XML_FUNC :

	return hError;
}





/*********************** End of File ******************************/
