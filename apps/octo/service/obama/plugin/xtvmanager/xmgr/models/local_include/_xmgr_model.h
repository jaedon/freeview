/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  _xmgr_model.h
	@brief

	Description:  									\n
	Module: Portinig Layer / Initialization / Model		\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding 을 사용하는 source file 입니다.
* MS949 encodingを使用して source fileです。
* Quelldatei, die MS949-Codierung verwendet.
*
*/


#ifndef	___INT_XMGR_MODEL_H___
#define	___INT_XMGR_MODEL_H___

#define ___INSIDE_INT_XMGR_MODEL_H_FILE___

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#if defined(CONFIG_PROD_HDR1000S)
	#include "model/_xmgr_hdr1000s.h"
#elif defined(CONFIG_PROD_HMS1000S) || defined(CONFIG_PROD_HMSLITE)
	#include "model/_xmgr_hms1000s.h"
#elif defined(CONFIG_PROD_IR4000HD) // TODO: shko 임시 검토필요
	#include "model/_xmgr_ir4000hd.h"
#elif defined(CONFIG_PROD_ICORDPRO)
	#include "model/_xmgr_icordpro.h"
#elif defined(CONFIG_PROD_HDNEO)
	#include "model/_xmgr_hdneo.h"
#elif defined(CONFIG_PROD_HGS1000S)
	#include "model/_xmgr_hgs1000s.h"
#elif defined(CONFIG_PROD_HD9000I)
	#include "model/_xmgr_hd9000i.h"
#elif defined(CONFIG_PROD_HMS1000T)
	#include "model/_xmgr_hms1000t.h"
#elif defined(CONFIG_PROD_IHDR6100C)
	#include "model/_xmgr_ihdr6100c.h"
#elif defined(CONFIG_PROD_FVP4000T)
	#include "model/_xmgr_fvp4000t.h"
#elif defined(CONFIG_PROD_HMX4KBBC)
	#include "model/_xmgr_hmx4kbbc.h"
#elif defined(CONFIG_PROD_DIGIMAXT2)
	#include "model/_xmgr_digimaxt2.h"
#elif defined(CONFIG_PROD_HDFOXIRCONNECT)
	#include "model/_xmgr_hdfoxirconnect.h"
#elif defined(CONFIG_PROD_YSR2000)
	#include "model/_xmgr_ysr2000.h"
#elif defined(CONFIG_PROD_YS1000)
	#include "model/_xmgr_ys1000.h"
#elif defined(CONFIG_PROD_CDIG2000C)
	#include "model/_xmgr_cdig2000c.h"
#elif defined(CONFIG_PROD_ROCKYPOM) || defined(CONFIG_PROD_ROCKYWPOM)
	#include "model/_xmgr_rockypom.h"
#elif defined(CONFIG_PROD_HA7100S)
	#include "model/_xmgr_ha7100s.h"
#elif defined(CONFIG_PROD_M1)
	#include "model/_xmgr_m1.h"
#else
	#error ("model/_xmgr_[modelname].h Shall be included!!!\n")
#endif

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
/********************      Functions       *************************/
/*******************************************************************/
HERROR xmgr_model_GetSiEsPriorityTable_Base (SvcSi_EsPriorityTable_t *pstTable);

#undef ___INSIDE_INT_XMGR_MODEL_H_FILE___
#endif	/*	!___INT_XMGR_MODEL_H___	*/
