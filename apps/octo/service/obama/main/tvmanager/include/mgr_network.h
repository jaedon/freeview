/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  mgr_network.h
	@brief

	Description:			\n
	Module: 			 	\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	___MGR_NETWORK_H___
#define	___MGR_NETWORK_H___

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <db_svc.h>
#include <mgr_common.h>

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
enum
{
	eMEVT_NETWORK		= eMEVT_NETWORK_START,

	/* - Description : 접속하려고 하는 Access Point 정보를 검색하여 전달 받는 message
		   - Parameters Usage :
			   handle : N.A.
			   p1 : Net_Inet_IF_t
			   p2 : 0
			   p3 : 0 */
	eMEVT_NETWORK_INTERFACE_CONNECT,

	/* - Description : 접속하려고 하는 Access Point 정보를 검색하여 전달 받는 message
		   - Parameters Usage :
			   handle : N.A.
			   p1 : Net_Inet_IF_t
			   p2 : 0
			   p3 : 0 */
	eMEVT_NETWORK_INTERFACE_DISCONNECT,


};

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
HERROR	MGR_NETWORK_KeepNetworkingStart (void);
HERROR	MGR_NETWORK_KeepNetworkingSetEanbleStateOfIF (HINT32 nDevIdx, HBOOL bOn);


#endif /* !___MGR_NETWORK_H___ */

