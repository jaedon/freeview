/**
	@file     _xmgr_epg.h
	@brief    file_name & simple comment.

	Module: Application(Layer), Menu(module) \n
	Remarks : \n
	 Date			Modification				Name\n
	-----------	-------------------------------------	------------------\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n

Default Installation :

*/


#ifndef __XMGR_EPG_INT_H__
#define __XMGR_EPG_INT_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <db_svc.h>
#include <svc_ch.h>
#include <bus.h>
#include <mgr_common.h>
#include <mgr_epg.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
enum humaxepg_cmd
{
	eHMXEPGCMD_IPSERVICE_UPDATED = 18,
	eHMXEPGCMD_RF_OVERFLOW,
	eHMXEPGCMD_LAST
};

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/


/*******************************************************************/
/********************      static variables         ********************/
/*******************************************************************/


/*******************************************************************/
/********************      static Functions         *******************/
/*******************************************************************/

/*******************************************************************/
/********************      external Functions    **********************/
/*******************************************************************/

/*		Proto Action Releated Function
 */

HERROR 			xmgr_epg_RfStart_Base (void);

#if defined(CONFIG_APCORE_IP_EPG)
HERROR			xmgr_epg_IpHumaxStart_Base (void);
#if defined(CONFIG_OP_UK_DTT)
HERROR			xmgr_epg_IpHumaxStart_Ukdtt (void);
#endif
#endif

#if defined(CONFIG_MW_EPG_TVTV)
HERROR			xmgr_epg_TvtvStart_Base (MgrEpg_ProcessType_e eProcessType, MgrEpg_CallType_e eActionType);
HERROR			xmgr_epg_TvtvStop_Base (void);
#endif


HERROR xmgr_epg_BgTrackStart_Base(void);


#endif

