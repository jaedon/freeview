/**************************************************************
 *	@file		rsv_downloader.h
 *	IP EPG downloader for OCTO 2.0
 *
 *	http://www.humaxdigital.com
 *
 *	@data			2012/08/01
 *	@author			humax
 *	@brief			IP EPG Downloader for OCTO 2.0 Project
 **************************************************************/
#ifndef _IPEPG_PARSER_H_
#define _IPEPG_PARSER_H_
#include	<hlib.h>

#if defined(CONFIG_OP_UK_DTT)
#include	<dlib.h>
extern HINT32	IPEPG_ParseHeaderFieldValue (HCHAR *szValue, HCHAR *szHeader, HCHAR *szName, HINT32 nLimit);
extern HERROR	IPEPG_FvcParseHeader(HUINT8 *pRawBuffer, DxFvcExpiry_t *pstFvcExpiry);
extern HERROR	IPEPG_FvcParseServiceList(HUINT8 *pRawBuffer, HUINT32 ulBufferSize, HxList_t **ListOfEventURL);
extern HERROR	IPEPG_FvcParseEventList(HUINT8 *pRawBuffer, HUINT32 ulBufferSize, HxList_t **ppEventList, HINT32 ulSvcUid);
#endif

extern HERROR	IPEPG_HumaxParseServiceList(HUINT8 *pRawBuffer, HUINT32 ulBufferSize, HxList_t **ListOfEventURL);
extern HERROR	IPEPG_HumaxParseEventList(HUINT8 *pRawBuffer, HUINT32 ulBufferSize, HxList_t **ppEventList, HINT32 ulSvcUid);


#endif

/*********************** End of File ******************************/

