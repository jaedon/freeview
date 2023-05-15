/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  layer_module.h File Name을 적습니다.
	@brief	  file_name & simple comment.

	Description: File에 대한 설명을 적습니다.		\n
	Module: Layer와 모듈 명을 적습니다.			 	\n
	Remarks : 										\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__XSVC_DOWNLOAD_INT_H__
#define	__XSVC_DOWNLOAD_INT_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

#include <xsvc_download.h>

//#include <download_hbbtv.h>
//#include <download_dvb_ssu.h>
//#include <download_freesat_meta.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

extern HERROR xsvc_download_InitDsmcc_Hbbtv(void);
extern HERROR xsvc_download_InitDvbssu(void);
extern HERROR xsvc_download_InitSatRecord(void);

#if defined(CONFIG_PROD_HMS1000S)
extern HERROR xsvc_download_Init_Hms1000s(void);
#elif defined(CONFIG_PROD_IR4000HD)  //
extern HERROR xsvc_download_Init_Ir4000hd(void);
#elif defined(CONFIG_PROD_ICORDPRO)
extern HERROR xsvc_download_Init_Icordpro(void);
#elif defined(CONFIG_PROD_HDNEO)
extern HERROR xsvc_download_Init_Hdneo(void);
#elif defined(CONFIG_PROD_HGS1000S)
extern HERROR xsvc_download_Init_hgs1000s(void);
#elif defined(CONFIG_PROD_HGS1000SIT)
extern HERROR xsvc_download_Init_hgs1000sit(void);
#elif defined(CONFIG_PROD_HD9000I)
extern HERROR  xsvc_download_Init_hd9000i(void);
#elif defined(CONFIG_PROD_HMS1000T)
extern HERROR xsvc_download_Init_Hms1000t(void);
#elif defined(CONFIG_PROD_IHDR6100C)
extern HERROR xsvc_download_Init_Ihdr6100c(void);
#elif defined(CONFIG_PROD_FVP4000T)
extern HERROR xsvc_download_Init_fvp4000t(void);
#elif defined(CONFIG_PROD_DIGIMAXT2)
extern HERROR xsvc_download_Init_Digimaxt2(void);
#elif defined(CONFIG_PROD_HDFOXIRCONNECT)
extern HERROR xsvc_download_Init_HdFoxIrConnect(void);
#elif defined(CONFIG_PROD_HA7100S)  //
extern HERROR xsvc_download_Init_Ha7100s(void);
#elif defined(CONFIG_PROD_YSR2000)
extern HERROR xsvc_download_Init_ysr2000(void);
#elif defined(CONFIG_PROD_YS1000)
extern HERROR xsvc_download_Init_ys1000(void);
#elif defined(CONFIG_PROD_M1)
extern HERROR xsvc_download_Init_M1(void);
#elif defined(CONFIG_PROD_CDIG2000C)
extern HERROR xsvc_download_Init_cdig2000c(void);
#endif

extern HERROR xsvc_download_DmxStopOneSection(HUINT16 usFilterId);
extern HERROR xsvc_download_DmxRetrieveDsiSection(SiRetrieveParam_t *pstTable,
												void *pfnParse,
												void *pfnFree,
												HUINT16 *pusFilterId);
extern HERROR xsvc_download_DmxRetrieveDdbSection(SiRetrieveParam_t *pstTable,
												void *pfnParse,
												void *pfnFree,
												HUINT16 *pusFilterId);
extern HERROR xsvc_download_DmxRetrieveDescSection(SiRetrieveParam_t *pstTable,
												void *pfnParse,
												void *pfnFree,
												HUINT16 *pusFilterId);

extern HERROR xsvc_download_DmxGetTimeout(Handle_t hAction, SiTableType_e eTableType, HUINT32 *pulTimeout);

#endif /* !__XSVC_DOWNLOAD_INT_H__ */
