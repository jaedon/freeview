/**
	@file     pal_xml.c
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
#include <stdio.h>
#include <stdlib.h>
#include <hlib.h>
#include <assert.h>

#include <dlfcn.h>

#include <vkernel.h>

#include <libxml/xmlmemory.h>

#include "_hlib_xml.h"
#include "_hlib_stdlib.h"
#include "_hlib_log.h"

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#undef	HxLOG_DOMAIN
#define HxLOG_DOMAIN	DOMAIN_TVSVC_HXXML


#define			HxXML_ELEMENT_NAME_MAX		64
#if defined(CONFIG_DEBUG)
#define	XML_RETURN_IF(expr, err)	do{if(expr){HxLOG_Debug("%s failed!!\n", #expr);return (err);}}while(0)
#define	XML_GOTO_IF(expr, pos)		do{if(expr){HxLOG_Debug("%s failed!!\n", #expr);goto pos;}}while(0)

#else
#define	XML_RETURN_IF(expr, err)	do{if(expr){return (err);}}while(0)
#define	XML_GOTO_IF(expr, pos)		do{if(expr){goto pos;}}while(0)
#endif

#define	xml2(func)		s_pstXml2Lib->_##func

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct
{
	HUINT32		ulSemId;			//	Semaphore가 필요없을것 같지만 나중을 대비해서 우선 만들어 놓음.

	void		*pvXml2So;

	void				(*_xmlInitParser)(void);
	xmlDocPtr			(*_xmlParseMemory)(const char *buffer, int size);
	xmlDocPtr			(*_xmlParseFile)(const char *filename);
	int					(*_xmlValidateDtd)(xmlValidCtxtPtr ctxt, xmlDocPtr doc, xmlDtdPtr dtd);
	xmlDtdPtr			(*_xmlParseDTD)(const xmlChar *ExternalID, const xmlChar *SystemID);
	xmlParserCtxtPtr	(*_xmlCreateMemoryParserCtxt)(const char *buffer, int size);
	void				(*_xmlFreeDoc)(xmlDocPtr cur);
	void				(*_xmlFreeParserCtxt)(xmlParserCtxtPtr ctxt);
	xmlNodePtr			(*_xmlDocGetRootElement)(xmlDocPtr doc);
	xmlChar*			(*_xmlNodeListGetString)(xmlDocPtr doc, xmlNodePtr list, int inLine);
	xmlChar*			(*_xmlGetProp)(xmlNodePtr node, const xmlChar *name);
	int					(*_xmlParseDocument)(xmlParserCtxtPtr ctxt);
	void				(*_xmlFreeDtd)(xmlDtdPtr cur);
	void				(*_xmlFree)(void *ptr);
} xml2_t;

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
static xml2_t		*s_pstXml2Lib = NULL;

/*******************************************************************/
/********************      extern functions     ********************/
/*******************************************************************/
extern	xmlParserCtxtPtr	xmlCreateMemoryParserCtxt	(const char * buffer, int size);


/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

static HERROR		__xml_InitParser(void);
static HERROR		__xml_ParseBuffer(HUINT8 *pucBuffer, HUINT32 ulBufferSize, HxXMLDOC *phDocument);
static HERROR		__xml_ParseFile(HUINT8 *pszFileName, HxXMLDOC *phDocument);
static HERROR		__xml_Validate(HUINT32 ulMode, HUINT8 *pszDTDFileName, HUINT8 *pucDTDStream, HUINT32 ulStreamSize, HxXMLDOC hDocument);
static HERROR		__xml_ParseDTDFile(HUINT8 *pszDtdFileName, HxXMLDTD *phDtdHandle);
static HERROR		__xml_ParseDTDBuffer(HUINT8 *pucDtdBuffer, HUINT32 ulBufferSize, HxXMLDTD *phDtdHandle);
static HERROR		__xml_FindChildElementByName(HxXMLELEMENT hParent, HUINT8 *pucName, HxList_t **ppElementList);
static HERROR		__xml_GetChildElementList(HxXMLELEMENT hParent, HxList_t **ppElementList);
static HERROR		__xml_GetRootElement(HxXMLDOC hDocHandle, HxXMLELEMENT *phRootHandle);
static HERROR		__xml_GetChildElement(HxXMLELEMENT hElementHandle, HxXMLELEMENT *phChildHandle);
static HERROR		__xml_GetNextElement(HxXMLELEMENT hElementHandle, HxXMLELEMENT *phNextHandle);
static HERROR		__xml_GetElementName(HxXMLELEMENT hElementHandle, HUINT8 **ppucName);
static HERROR		__xml_GetElementContents(HxXMLDOC hDocHandle, HxXMLELEMENT hElementHandle, HUINT8 **ppucContents);
static HERROR		__xml_GetAttrContents(HxXMLELEMENT hElementHandle, HUINT8* pszName, HUINT8 **ppucContents);
static void			__xml_FreeAttrContents (HUINT8 *pucContents);
static HERROR		__xml_FreeDocument(HxXMLDOC hDocHandle);

/*******************************************************************************************
 * Static
 ******************************************************************************************/
static HERROR		__xml_InitLibrary(void)
{
	if (s_pstXml2Lib == NULL)
	{
		s_pstXml2Lib = HLIB_MEM_Calloc(sizeof(xml2_t));

		s_pstXml2Lib->pvXml2So = dlopen("/usr/lib/libxml2.so", RTLD_LAZY);
		if (s_pstXml2Lib->pvXml2So == NULL)
		{
			HxLOG_Critical("Can't find /usr/lib/libxml2.so\n");
			HxLOG_Assert(0);
			return ERR_FAIL;
		}

		s_pstXml2Lib->_xmlInitParser = dlsym(s_pstXml2Lib->pvXml2So, "xmlInitParser");
		s_pstXml2Lib->_xmlParseMemory = dlsym(s_pstXml2Lib->pvXml2So, "xmlParseMemory");
		s_pstXml2Lib->_xmlParseFile = dlsym(s_pstXml2Lib->pvXml2So, "xmlParseFile");
		s_pstXml2Lib->_xmlValidateDtd = dlsym(s_pstXml2Lib->pvXml2So, "xmlValidateDtd");
		s_pstXml2Lib->_xmlParseDTD = dlsym(s_pstXml2Lib->pvXml2So, "xmlParseDTD");

		s_pstXml2Lib->_xmlCreateMemoryParserCtxt = dlsym(s_pstXml2Lib->pvXml2So, "xmlCreateMemoryParserCtxt");
		s_pstXml2Lib->_xmlFreeDoc = dlsym(s_pstXml2Lib->pvXml2So, "xmlFreeDoc");
		s_pstXml2Lib->_xmlFreeParserCtxt = dlsym(s_pstXml2Lib->pvXml2So, "xmlFreeParserCtxt");
		s_pstXml2Lib->_xmlDocGetRootElement = dlsym(s_pstXml2Lib->pvXml2So, "xmlDocGetRootElement");
		s_pstXml2Lib->_xmlNodeListGetString = dlsym(s_pstXml2Lib->pvXml2So, "xmlNodeListGetString");
		s_pstXml2Lib->_xmlGetProp = dlsym(s_pstXml2Lib->pvXml2So, "xmlGetProp");
		s_pstXml2Lib->_xmlParseDocument = dlsym(s_pstXml2Lib->pvXml2So, "xmlParseDocument");
		s_pstXml2Lib->_xmlFreeDtd = dlsym(s_pstXml2Lib->pvXml2So, "xmlFreeDtd");
		s_pstXml2Lib->_xmlFree = dlsym(s_pstXml2Lib->pvXml2So, "xmlFree");

		HxLOG_Assert(s_pstXml2Lib->_xmlFree && s_pstXml2Lib->_xmlFreeDtd && s_pstXml2Lib->_xmlParseDocument);
		HxLOG_Assert(s_pstXml2Lib->_xmlGetProp && s_pstXml2Lib->_xmlNodeListGetString && s_pstXml2Lib->_xmlDocGetRootElement);
		HxLOG_Assert(s_pstXml2Lib->_xmlFreeParserCtxt && s_pstXml2Lib->_xmlFreeDoc && s_pstXml2Lib->_xmlCreateMemoryParserCtxt);
		HxLOG_Assert(s_pstXml2Lib->_xmlInitParser && s_pstXml2Lib->_xmlParseMemory && s_pstXml2Lib->_xmlParseFile);
		HxLOG_Assert(s_pstXml2Lib->_xmlValidateDtd && s_pstXml2Lib->_xmlParseDTD);

		HxLOG_Warning("XML2 Dynamic loading is done...\n");
	}

	return ERR_OK;
}

static HERROR		__xml_InitParser(void)
{
	xml2(xmlInitParser());
	return ERR_OK;
}

static HERROR		__xml_ParseBuffer(HUINT8 *pucBuffer, HUINT32 ulBufferSize, HxXMLDOC *phDocument)
{
	xmlDocPtr		pstDocument = (xmlDocPtr)NULL;

	XML_RETURN_IF(pucBuffer == NULL || phDocument == NULL, ERR_FAIL);
	pstDocument = xml2(xmlParseMemory((char*)pucBuffer, (int)ulBufferSize));
	XML_RETURN_IF(pstDocument == NULL, ERR_FAIL);

	*phDocument = (HxXMLDOC)pstDocument;

	return ERR_OK;
}

static HERROR		__xml_ParseFile(HUINT8 *pszFileName, HxXMLDOC *phDocument)
{
	xmlDocPtr		pstDocument = (xmlDocPtr)NULL;

	XML_RETURN_IF(pszFileName == NULL || phDocument == NULL, ERR_FAIL);
	pstDocument = xml2(xmlParseFile((const char*)pszFileName));
	XML_RETURN_IF(pstDocument == NULL, ERR_FAIL);
	*phDocument = (HxXMLDOC)pstDocument;

	return ERR_OK;
}

static HERROR		__xml_Validate(HUINT32 ulMode, HUINT8 *pszDTDFileName, HUINT8 *pucDTDStream, HUINT32 ulStreamSize, HxXMLDOC hDocument)
{
	xmlValidCtxt		vctxt;
	HERROR			hError = ERR_FAIL;
	xmlDocPtr		pstDocument = (xmlDocPtr)NULL;
	HxXMLDTD		hDtdHandle = NULL;


	if(ulMode & _XML_VALIDATE_BUFFER)
	{
		XML_GOTO_IF(pucDTDStream == NULL || hDocument == NULL, EXIT_XML_FUNC);
		XML_GOTO_IF (__xml_ParseDTDBuffer(pucDTDStream, ulStreamSize, &hDtdHandle) != ERR_OK, EXIT_XML_FUNC);
	}
	else
	{
		XML_GOTO_IF(pszDTDFileName == NULL || hDocument == NULL, EXIT_XML_FUNC);
		XML_GOTO_IF(__xml_ParseDTDFile(pszDTDFileName, &hDtdHandle) != ERR_OK, EXIT_XML_FUNC);
	}

	pstDocument = (xmlDocPtr)hDocument;
	vctxt.userData = stderr;
	vctxt.error = (xmlValidityErrorFunc)fprintf;
	vctxt.warning = (xmlValidityWarningFunc)fprintf;

	//if 0, it is not valid
	XML_RETURN_IF((int)xml2(xmlValidateDtd(&vctxt, pstDocument, (xmlDtdPtr)hDtdHandle)) == 0, ERR_FAIL);
	hError = ERR_OK;

EXIT_XML_FUNC :

	xml2(xmlFreeDtd(hDtdHandle));
	return hError;
}

static HERROR		__xml_ParseDTDFile(HUINT8 *pszDtdFileName, HxXMLDTD *phDtdHandle)
{
	xmlDtdPtr		pDtd = NULL;

	XML_RETURN_IF(pszDtdFileName == NULL || phDtdHandle == NULL, ERR_FAIL);

	pDtd = xml2(xmlParseDTD(NULL, (xmlChar *)pszDtdFileName));
	XML_RETURN_IF(pDtd == NULL, ERR_FAIL);

	*phDtdHandle =(HxXMLDTD)pDtd;
	return ERR_OK;

}



static HERROR		__xml_ParseDTDBuffer(HUINT8 *pucDtdBuffer, HUINT32 ulBufferSize, HxXMLDTD *phDtdHandle)
{
	xmlParserCtxtPtr	ctxt = NULL;
	HERROR			hError = ERR_FAIL;

	XML_GOTO_IF(pucDtdBuffer == NULL || phDtdHandle == NULL,EXIT_XML_FUNC);

	ctxt = xml2(xmlCreateMemoryParserCtxt((const char*)pucDtdBuffer, (int)ulBufferSize));
	XML_GOTO_IF(ctxt == NULL, EXIT_XML_FUNC);
	XML_GOTO_IF((int)xml2(xmlParseDocument(ctxt))!= ERR_OK, EXIT_XML_FUNC);
	XML_GOTO_IF(!ctxt->wellFormed, EXIT_XML_FUNC);

	*phDtdHandle = (HxXMLDTD)ctxt->myDoc->intSubset;
	ctxt->myDoc->intSubset = NULL;

	xml2(xmlFreeDoc(ctxt->myDoc));
	ctxt->myDoc = NULL;
	hError = ERR_OK;

EXIT_XML_FUNC :

	xml2(xmlFreeParserCtxt(ctxt));
	ctxt = NULL;

	return hError;

}

static HERROR		__xml_FindChildElementByName(HxXMLELEMENT hParent, HUINT8 *pucName, HxList_t **ppElementList)
{
	HERROR			hError = ERR_FAIL;
	HxXMLELEMENT		hTempNode = NULL;
	HUINT8			*pucTempName = NULL;
	HxList_t		*pElementList = NULL;

	XML_RETURN_IF(hParent == NULL || pucName == NULL || ppElementList == NULL, ERR_FAIL);
	*ppElementList = NULL;

	XML_RETURN_IF(__xml_GetChildElement(hParent, &hTempNode) != ERR_OK, ERR_FAIL);

	while(hTempNode != NULL)
	{
		pucTempName = NULL;
		hError = __xml_GetElementName(hTempNode, &pucTempName);
		if(hError != ERR_OK || pucTempName == NULL)
		{
			HxLOG_Print("[__xml_FindChildElementByName] GetElementName fail!!\n");
			hError = __xml_GetNextElement(hTempNode, &hTempNode);
			if(hError != ERR_OK)
			{
				HxLOG_Print("[__xml_FindChildElementByName] GetNextElement NULL!!\n");
			}
			continue;
		}

		if(!HxSTD_StrCmp(pucTempName, pucName) )
		{
			//pElementList = UTIL_LINKEDLIST_AppendListItemToPointerList(pElementList, (void*)hTempNode);
			pElementList = HLIB_LIST_Append(pElementList,(void*)hTempNode);

			if(pElementList == NULL)
			{
				HxLOG_Print("[__xml_FindChildElementByName] AppendListItem fail!!\n");
			}
		}

		hError = __xml_GetNextElement(hTempNode, &hTempNode);
		if(hError != ERR_OK)
		{
			HxLOG_Print("[__xml_FindChildElementByName] GetNextElement NULL!!\n");
		}
	}

	if(pElementList != NULL)
	{
		*ppElementList = pElementList;
		hError = ERR_OK;
	}

	return hError;
}

static HERROR		__xml_GetChildElementList(HxXMLELEMENT hParent, HxList_t **ppElementList)
{
	HERROR			hError = ERR_FAIL;
	HxXMLELEMENT		hTempNode = NULL;
	HxList_t	*pElementList = NULL;

	XML_RETURN_IF(hParent == NULL || ppElementList == NULL, ERR_FAIL);
	*ppElementList = NULL;

	XML_RETURN_IF(__xml_GetChildElement(hParent, &hTempNode) != ERR_OK, ERR_FAIL);

	while(hTempNode != NULL)
	{
		pElementList = HLIB_LIST_Append(pElementList,(void*)hTempNode);
		if(pElementList == NULL)
		{
			HxLOG_Print("[__xml_GetChildElementList] AppendListItem fail!!\n");
		}
		hError = __xml_GetNextElement(hTempNode, &hTempNode);
		if(hError != ERR_OK)
		{
			HxLOG_Print("[__xml_GetChildElementList] GetNextElement NULL!!\n");
		}
	}

	if(pElementList != NULL)
	{
		*ppElementList = pElementList;
		hError = ERR_OK;
	}

	return hError;
}


static HERROR		__xml_GetRootElement(HxXMLDOC hDocHandle, HxXMLELEMENT *phRootHandle)
{
	xmlDocPtr		pstDocument = (xmlDocPtr)NULL;
	xmlNodePtr		pstRootNode = NULL;


	XML_RETURN_IF(hDocHandle == NULL || phRootHandle == NULL, ERR_FAIL);
	*phRootHandle = NULL;

	pstDocument = (xmlDocPtr)hDocHandle;
	pstRootNode = xml2(xmlDocGetRootElement(pstDocument));
	XML_RETURN_IF(pstRootNode == NULL, ERR_FAIL);
	*phRootHandle = (HxXMLELEMENT)pstRootNode;

	return ERR_OK;
}


static HERROR		__xml_GetChildElement(HxXMLELEMENT hElementHandle, HxXMLELEMENT *phChildHandle)
{
	xmlNodePtr		pstParent = NULL;

	XML_RETURN_IF(hElementHandle == NULL || phChildHandle == NULL, ERR_FAIL);
	*phChildHandle = NULL;

	pstParent = (xmlNodePtr)hElementHandle;
	XML_RETURN_IF(pstParent->children == NULL, ERR_FAIL);

	*phChildHandle = (HxXMLELEMENT)pstParent->children;

	return ERR_OK;
}

static HERROR		__xml_GetNextElement(HxXMLELEMENT hElementHandle, HxXMLELEMENT *phNextHandle)
{
	xmlNodePtr		pstCurrent = NULL;

	XML_RETURN_IF(hElementHandle == NULL || phNextHandle == NULL, ERR_FAIL);
	*phNextHandle = NULL;

	pstCurrent = (xmlNodePtr)hElementHandle;
	XML_RETURN_IF(pstCurrent->next == NULL, ERR_FAIL);

	*phNextHandle = (HxXMLELEMENT)pstCurrent->next;

	return ERR_OK;
}

static HERROR		__xml_GetElementName(HxXMLELEMENT hElementHandle, HUINT8 **ppucName)
{
	xmlNodePtr		pstCurrent = NULL;

	XML_RETURN_IF(hElementHandle == NULL || ppucName == NULL, ERR_FAIL);
	*ppucName = NULL;

	pstCurrent = (xmlNodePtr)hElementHandle;
	XML_RETURN_IF(pstCurrent->name == NULL, ERR_FAIL);

	*ppucName = (HUINT8*)pstCurrent->name;

	return ERR_OK;
}

static HERROR		__xml_GetElementContents(HxXMLDOC hDocHandle, HxXMLELEMENT hElementHandle, HUINT8 **ppucContents)
{
	xmlDocPtr		pstDocument = (xmlDocPtr)NULL;
	xmlNodePtr		pstCurrent = NULL;

	XML_RETURN_IF(hDocHandle == NULL || hElementHandle == NULL, ERR_FAIL);
	*ppucContents = NULL;

	pstDocument = (xmlDocPtr)hDocHandle;
	pstCurrent = (xmlNodePtr)hElementHandle;

	// alloc & assign
	*ppucContents = xml2(xmlNodeListGetString(pstDocument, pstCurrent->children, 1));
	XML_RETURN_IF(*ppucContents == NULL, ERR_FAIL);

	return ERR_OK;
}



static HERROR		__xml_GetAttrContents(HxXMLELEMENT hElementHandle, HUINT8* pszName, HUINT8 **ppucContents)
{
	xmlNodePtr		pstCurrent = NULL;

	XML_RETURN_IF(hElementHandle == NULL || pszName == NULL, ERR_FAIL);
	*ppucContents = NULL;

	pstCurrent = (xmlNodePtr)hElementHandle;

	// alloc & assign
	*ppucContents = xml2(xmlGetProp(pstCurrent, pszName));
	XML_RETURN_IF(*ppucContents == NULL, ERR_FAIL);

	return ERR_OK;
}


static void			__xml_FreeAttrContents (HUINT8 *pucContents)
{
	if (pucContents != NULL)
	{
		//xml2(xmlFree(pucContents));
		free(pucContents);	// xmlFree is unknown function.
	}
}


static HERROR		__xml_FreeDocument(HxXMLDOC hDocHandle)
{
	XML_RETURN_IF(hDocHandle == NULL, ERR_FAIL);
	xml2(xmlFreeDoc((xmlDocPtr)hDocHandle));
	return ERR_OK;
}



/*******************************************************************************************
 * Interfaces
 ******************************************************************************************/
HERROR	HLIB_XML_ParseFile(HUINT8 *pszFileName, HxXMLDOC *phDocument)
{
	XML_RETURN_IF(pszFileName == NULL || phDocument == NULL, ERR_FAIL);
	XML_RETURN_IF(__xml_ParseFile(pszFileName, phDocument) != ERR_OK, ERR_FAIL);

	return ERR_OK;
}

HERROR	HLIB_XML_ParseBuffer(HUINT8 *pucBuffer, HUINT32 ulBufferSize, HxXMLDOC *phDocument)
{
	XML_RETURN_IF(pucBuffer == NULL || phDocument == NULL, ERR_FAIL);
	XML_RETURN_IF(__xml_ParseBuffer(pucBuffer, ulBufferSize, phDocument)!= ERR_OK, ERR_FAIL);

	return ERR_OK;
}

HERROR	HLIB_XML_Validate(HUINT32 ulMode, HUINT8 *pszDTD, HUINT8 *pucDtdBuffer, HUINT32 ulBufferSize, HxXMLDOC hDocument)
{
	XML_RETURN_IF(__xml_Validate(ulMode, pszDTD, pucDtdBuffer, ulBufferSize, hDocument) != ERR_OK, ERR_FAIL);
	return ERR_OK;
}

HERROR	HLIB_XML_InitParser(void)
{
	XML_RETURN_IF(__xml_InitLibrary() != ERR_OK, ERR_FAIL);
	XML_RETURN_IF(__xml_InitParser() != ERR_OK, ERR_FAIL);

	return ERR_OK;
}

HERROR	HLIB_XML_GetRootElement(HxXMLDOC hDocHandle, HxXMLELEMENT *phRootHandle)
{
	XML_RETURN_IF(hDocHandle == NULL || phRootHandle == NULL, ERR_FAIL);
	XML_RETURN_IF(__xml_GetRootElement(hDocHandle, phRootHandle) != ERR_OK, ERR_FAIL);
	return ERR_OK;
}

HERROR	HLIB_XML_FindChildElementByName(HxXMLELEMENT hParent, HUINT8 *pucName, HxList_t **ppElementList)
{
	XML_RETURN_IF(hParent == NULL || pucName == NULL || ppElementList == NULL, ERR_FAIL);
	XML_RETURN_IF(__xml_FindChildElementByName(hParent, pucName, ppElementList) != ERR_OK, ERR_FAIL);
	return ERR_OK;
}

HERROR	HLIB_XML_GetChildElementList(HxXMLELEMENT hParent, HxList_t **ppElementList)
{
	XML_RETURN_IF(hParent == NULL || ppElementList == NULL, ERR_FAIL);
	XML_RETURN_IF(__xml_GetChildElementList(hParent, ppElementList) != ERR_OK, ERR_FAIL);
	return ERR_OK;
}

HERROR	HLIB_XML_GetElementContents(HxXMLDOC hDocHandle, HxXMLELEMENT hElementHandle, HUINT8 **ppucContents)
{
	XML_RETURN_IF(hDocHandle == NULL || hElementHandle == NULL || ppucContents == NULL, ERR_FAIL);
	XML_RETURN_IF(__xml_GetElementContents(hDocHandle, hElementHandle, ppucContents) != ERR_OK, ERR_FAIL);
	return ERR_OK;
}

HERROR	HLIB_XML_GetNextElement(HxXMLELEMENT hElementHandle, HxXMLELEMENT *phNextHandle)
{
	XML_RETURN_IF(hElementHandle == NULL || phNextHandle == NULL, ERR_FAIL);
	XML_RETURN_IF(__xml_GetNextElement(hElementHandle, phNextHandle)!= ERR_OK, ERR_FAIL);
	return ERR_OK;
}

HERROR	HLIB_XML_GetChildElement(HxXMLELEMENT hElementHandle, HxXMLELEMENT *phChildHandle)
{
	XML_RETURN_IF(hElementHandle == NULL || phChildHandle == NULL, ERR_FAIL);
	XML_RETURN_IF(__xml_GetChildElement(hElementHandle, phChildHandle) != ERR_OK, ERR_FAIL);
	return ERR_OK;
}

HERROR	HLIB_XML_GetElementName(HxXMLELEMENT hElementHandle, HUINT8 **ppucName)
{
	XML_RETURN_IF(hElementHandle == NULL || ppucName == NULL, ERR_FAIL);
	XML_RETURN_IF(__xml_GetElementName(hElementHandle, ppucName) != ERR_OK, ERR_FAIL);
	return ERR_OK;
}

HERROR	HLIB_XML_GetAttrContents(HxXMLELEMENT hElementHandle, HUINT8* pszName, HUINT8 **ppucContents)
{
 	XML_RETURN_IF(hElementHandle == NULL || pszName == NULL, ERR_FAIL);
 	XML_RETURN_IF(__xml_GetAttrContents(hElementHandle, pszName, ppucContents) != ERR_OK, ERR_FAIL);
 	return ERR_OK;
}

void	HLIB_XML_FreeAttrContents (HUINT8 *pucContents)
{
	__xml_FreeAttrContents(pucContents);
}


HERROR	HLIB_XML_FreeDocument(HxXMLDOC hDocHandle)
{
	XML_RETURN_IF(hDocHandle == NULL, ERR_FAIL);
	XML_RETURN_IF(__xml_FreeDocument(hDocHandle) != ERR_OK, ERR_FAIL);
	return ERR_FAIL;
}



/*********************** End of File ******************************/
