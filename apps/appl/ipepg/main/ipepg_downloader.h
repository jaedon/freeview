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
#ifndef _IPEPG_DOWNLOADER_H_
#define _IPEPG_DOWNLOADER_H_
#include	<hlib.h>

void	IPEPG_DownloadData(void *arg);
void	IPEPG_DownloadDataTask (void * pstDownMgr);
void 	IPEPG_ChLogoCreateTask(IPEPG_TASK_MGR_t* pstTaskMgr);
void	IPEPG_DeleteAllData(void);
void 	IPEPG_MemFreeEvent(void *mem);
void 	IPEPG_MemFreeIpEvent(void *mem);

#if defined (CONFIG_OP_UK_DTT)
void	IPEPG_DownloadFvcSvc (void *arg);
void	IPEPG_DownloadFvcEvt(void *arg);
void	IPEPG_DownloadFvcSeriesEvt(void *arg);
HERROR	IPEPG_HeadFvcPrefixUrl (HCHAR *szUriPrefix);

#endif

#endif

/*********************** End of File ******************************/

