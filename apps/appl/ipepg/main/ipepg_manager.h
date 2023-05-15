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
#ifndef _IPEPG_MANAGER_H_
#define _IPEPG_MANAGER_H_
#include	<hlib.h>
HERROR IPEPG_GetMsgQId(HUINT32 * pMsgQId);
HERROR IPEPG_SendCommand(IPEPG_MSG_e message);
HERROR IPEPG_SendCommandWithParam(IpEpgMsg_t	stMsg);
void IPEPG_StartProc(IPEPG_DATAMGR_t * pData);
void IPEPG_CancelTimer(void);
void IPEPG_StartProc(IPEPG_DATAMGR_t * pData);
#if defined(CONFIG_DEBUG)
#define IPEPG_SetTimer(x) 	IPEPG_SetTimerDebug(x, __FUNCTION__, __LINE__)
HERROR IPEPG_SetTimerDebug(HUINT32 time, const HCHAR *function, HUINT32 line);
#else
HERROR IPEPG_SetTimer(HUINT32 time);
#endif
void IPEPG_CheckVersionNRemoveDB(void);
#endif

HERROR IPEPG_UploadSvc_SendCommand(IPEPG_MSG_e message);
HERROR IPEPG_DownloadData_SendCommand(IPEPG_MSG_e message);
HERROR RP_SendCommand(IPEPG_MSG_e message);

/*********************** End of File ******************************/

