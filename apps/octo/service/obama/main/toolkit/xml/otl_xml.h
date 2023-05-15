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

#ifndef	__PALXML_H__
#define	__PALXML_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/



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
	ERR_XML_OK 					= 0,
	ERR_XML_FAIL					= 1

}XML_ERR;

typedef	enum
{
	XML_VALIDATE_BUFFER 			= 0,
	XML_VALIDATE_FILE				= 1

}XML_VALIDATION;


typedef void*			HXMLDOC;
typedef void*			HXMLDTD;
typedef void*			HXMLELEMENT;


/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/


HERROR		PAL_XML_ParseFile(HUINT8 *pszFileName, HXMLDOC *phDocument);
HERROR		PAL_XML_ParseBuffer(HUINT8 *pucBuffer, HUINT32 ulBufferSize, HXMLDOC *phDocument);
HERROR		PAL_XML_Validate(HUINT32 ulMode, HUINT8 *pszDTD, HUINT8 *pucDtdBuffer, HUINT32 ulBufferSize, HXMLDOC hDocument) ;
HERROR		PAL_XML_InitParser(void);
HERROR		PAL_XML_GetRootElement(HXMLDOC hDocHandle, HXMLELEMENT *phRootHandle);
HERROR		PAL_XML_FindChildElementByName(HXMLELEMENT hParent, HUINT8 *pucName, POINTER_LIST_T **ppElementList);
HERROR		PAL_XML_GetChildElementList(HXMLELEMENT hParent, POINTER_LIST_T **ppElementList);
HERROR		PAL_XML_GetElementContents(HXMLDOC hDocHandle, HXMLELEMENT hElementHandle, HUINT8 **ppucContents);
HERROR		PAL_XML_GetNextElement(HXMLELEMENT hElementHandle, HXMLELEMENT *phNextHandle);
HERROR		PAL_XML_GetChildElement(HXMLELEMENT hElementHandle, HXMLELEMENT *phChildHandle);
HERROR		PAL_XML_GetElementName(HXMLELEMENT hElementHandle, HUINT8 **ppucName);
HERROR		PAL_XML_GetAttrContents(HXMLELEMENT hElementHandle, HUINT8* pszName, HUINT8 **ppucContents);
void		PAL_XML_FreeAttrContents (HUINT8 *pucContents);
HERROR		PAL_XML_FreeDocument(HXMLDOC hDocHandle);

#endif /* !__PALXML_H__ */
