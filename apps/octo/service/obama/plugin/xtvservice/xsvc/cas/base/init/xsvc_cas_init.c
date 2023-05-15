/**
	@file     file_name.c File Name을 적습니다.
	@brief    file_name & simple comment.

	Description: File에 대한 설명을 적습니다.		\n
	Module: Layer와 모듈 명을 적습니다.			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
/** @brief global header file  */
#include <octo_common.h>

#include <svc_cas.h>
#include <xsvc_cas.h>

/** @brief local header file  */
#include "../../local_include/_xsvc_cas.h"

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


HERROR xsvc_cas_InitCi(void)
{
#if defined(CONFIG_MW_CI)
	HERROR	hErr;

	hErr = xsvc_cas_CiInit();

	return hErr;
#else
	return ERR_FAIL;
#endif
}

HERROR xsvc_cas_InitNagra(void)
{
#if defined(CONFIG_MW_CAS_NAGRA)
	HERROR	hErr;

	hErr = xsvc_cas_NaMgrInit();

	return hErr;
#else
	return ERR_FAIL;
#endif
}

HERROR xsvc_cas_InitIrdeto(void)
{
#if defined(CONFIG_MW_CAS_IRDETO) && defined(CONFIG_MW_CAS_IRDETO_SCELL)
	HERROR	hErr;

	hErr = xsvc_cas_IrMgrInit();

	return hErr;
#else
	return ERR_FAIL;
#endif
}

HERROR xsvc_cas_InitIrdetoUc(void)
{
#if defined(CONFIG_MW_CAS_IRDETO) && defined(CONFIG_MW_CAS_IRDETO_UCAS)
	HERROR	hErr;

	hErr = xsvc_cas_IrUcMgrInit();

	return hErr;
#else
	return ERR_FAIL;
#endif

}

HERROR xsvc_cas_InitConax(void)
{
#if defined(CONFIG_MW_CAS_CONAX)
	HERROR	hErr;

	hErr = xsvc_cas_CxMgrInit();

	return hErr;
#else
	return ERR_FAIL;
#endif
}


HERROR xsvc_cas_InitViaccess(void)
{
#if defined(CONFIG_MW_CAS_VIACCESS)
	HERROR	hErr;

	hErr = xsvc_cas_VaMgrInit();

	return hErr;
#else
	return ERR_FAIL;
#endif
}


HERROR xsvc_cas_InitSatsa(void)
{
#if defined(CONFIG_MW_CAS_SATSA)
	HERROR	hErr = ERR_OK;

#if !defined(WIN32)
	hErr = xsvc_cas_SatsaInit();
#endif

	return hErr;
#else
	return ERR_FAIL;
#endif
}


HERROR xsvc_cas_InitCiMgr(void)
{
#if defined(CONFIG_MW_CI)
	HERROR	hErr = ERR_OK;

	hErr = xsvc_cas_CiMgrInit();

	return hErr;
#else
	return ERR_FAIL;
#endif
}

HERROR xsvc_cas_InitBCas(void)
{
	return ERR_FAIL;
}

HERROR xsvc_cas_InitVmxIptv(void)
{
#if defined(CONFIG_MW_CAS_VERIMATRIX_IPTV)
	HERROR	hErr = ERR_OK;

	hErr = xsvc_cas_VmxMainInit();

	return hErr;
#else
	return ERR_FAIL;
#endif
}

/*********************** End of File ******************************/
