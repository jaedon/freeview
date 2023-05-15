/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  layer_module.h File Name을 적습니다.
	@brief	  file_name & simple comment.

	Description: File에 대한 설명을 적습니다.		\n
	Module: Layer와 모듈 명을 적습니다.			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__HX_XML_H__
#define	__HX_XML_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include "_hlib_list.h"


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/



/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef	enum
{
	_XML_VALIDATE_BUFFER 			= 0,
	_XML_VALIDATE_FILE				= 1

}_XML_VALIDATION;


typedef void*			HxXMLDOC;
typedef void*			HxXMLDTD;
typedef void*			HxXMLELEMENT;


/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/


HERROR		HLIB_XML_ParseFile(HUINT8 *pszFileName, HxXMLDOC *phDocument);
HERROR		HLIB_XML_ParseBuffer(HUINT8 *pucBuffer, HUINT32 ulBufferSize, HxXMLDOC *phDocument);
HERROR		HLIB_XML_Validate(HUINT32 ulMode, HUINT8 *pszDTD, HUINT8 *pucDtdBuffer, HUINT32 ulBufferSize, HxXMLDOC hDocument) ;
HERROR		HLIB_XML_InitParser(void);
HERROR		HLIB_XML_GetRootElement(HxXMLDOC hDocHandle, HxXMLELEMENT *phRootHandle);
HERROR		HLIB_XML_GetElementContents(HxXMLDOC hDocHandle, HxXMLELEMENT hElementHandle, HUINT8 **ppucContents);
HERROR		HLIB_XML_GetNextElement(HxXMLELEMENT hElementHandle, HxXMLELEMENT *phNextHandle);
HERROR		HLIB_XML_GetChildElement(HxXMLELEMENT hElementHandle, HxXMLELEMENT *phChildHandle);
HERROR		HLIB_XML_GetElementName(HxXMLELEMENT hElementHandle, HUINT8 **ppucName);
HERROR		HLIB_XML_GetAttrContents(HxXMLELEMENT hElementHandle, HUINT8* pszName, HUINT8 **ppucContents);
void		HLIB_XML_FreeAttrContents (HUINT8 *pucContents);
HERROR		HLIB_XML_FreeDocument(HxXMLDOC hDocHandle);
HERROR		HLIB_XML_FindChildElementByName(HxXMLELEMENT hParent, HUINT8 *pucName, HxList_t **ppElementList);
HERROR		HLIB_XML_GetChildElementList(HxXMLELEMENT hParent, HxList_t **ppElementList);
#endif /* !__PALXML_H__ */
