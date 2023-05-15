/**
	@file     file_name.c File Name을 적습니다.
	@brief    file_name & simple comment.

	Description: File에 대한 설명을 적습니다.		\n
	Module: Layer와 모듈 명을 적습니다.			 	\n
	Remarks : 										\n

	Copyright (c) 2011 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
/** @brief global header file  */
#include <octo_common.h>

#include <svc_download.h>

/** @brief local header file  */
#include "local_include/_xsvc_download.h"

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

/**
  @brief     LAYER_MODULE_DoSomething 함수.

  simple comment \n

  @return    return 값.
*/


// DOWNLOAD Main에서 호출하는 함수...
HERROR psvc_download_InitProduct(void)
{
#if defined(CONFIG_MW_DOWNLOAD)
 	HERROR hErr = ERR_FAIL;

#if defined(CONFIG_PROD_HMS1000S) || defined(CONFIG_PROD_HMSLITE)
	hErr = xsvc_download_Init_Hms1000s();
#elif defined(CONFIG_PROD_IR4000HD)
	hErr = xsvc_download_Init_Ir4000hd();
#elif defined(CONFIG_PROD_ICORDPRO)
	hErr = xsvc_download_Init_Icordpro();
#elif defined(CONFIG_PROD_HDNEO)
	hErr = xsvc_download_Init_Hdneo();
#elif defined(CONFIG_PROD_HGS1000S)
	hErr = xsvc_download_Init_hgs1000s();
#elif defined(CONFIG_PROD_HGS1000SIT)
	hErr = xsvc_download_Init_hgs1000sit();
#elif defined(CONFIG_PROD_HD9000I)
	hErr = xsvc_download_Init_hd9000i();
#elif defined(CONFIG_PROD_HMS1000T)
	hErr = xsvc_download_Init_Hms1000t();
#elif defined(CONFIG_PROD_IHDR6100C)
	hErr = xsvc_download_Init_Ihdr6100c();
#elif defined(CONFIG_PROD_FVP4000T)
	hErr = xsvc_download_Init_fvp4000t();
#elif defined(CONFIG_PROD_HMX4KBBC)
	hErr = xsvc_download_Init_Hmx4kbbc();
#elif defined(CONFIG_PROD_DIGIMAXT2)
	hErr = xsvc_download_Init_Digimaxt2();
#elif defined(CONFIG_PROD_HDFOXIRCONNECT)
	hErr = xsvc_download_Init_HdFoxIrConnect();
#elif defined(CONFIG_PROD_YSR2000)
	hErr = xsvc_download_Init_ysr2000();
#elif defined(CONFIG_PROD_YS1000)
	hErr = xsvc_download_Init_ys1000();
#elif defined(CONFIG_PROD_CDIG2000C)
	hErr = xsvc_download_Init_cdig2000c();
#elif defined(CONFIG_PROD_ROCKYPOM) || defined(CONFIG_PROD_ROCKYWPOM)
	hErr = xsvc_download_Init_rockypom();
#elif defined(CONFIG_PROD_HA7100S)
	hErr = xsvc_download_Init_Ha7100s();
#elif defined(CONFIG_PROD_M1)
	hErr = xsvc_download_Init_M1();
#elif defined(CONFIG_PROD_DT95420UHD)
	hErr = xsvc_download_Init_DT95420UHD();
#else
	#error ("xsvc_download_Init_[Product_Name] for a Product is not activated!\n")
#endif

	return hErr;
#else
	return ERR_FAIL;
#endif
}

HBOOL	psvc_download_IsValidOui(HUINT32 ulOui)
{
#if defined(CONFIG_MW_DOWNLOAD)
	return ((ulOui == eSWUP_OUI_HUMAX) || (ulOui == eSWUP_OUI_DVB)) ? TRUE : FALSE;
#else
	return FALSE;
#endif
}

/*********************** End of File ******************************/
