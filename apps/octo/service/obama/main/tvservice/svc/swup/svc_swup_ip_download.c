/**
	@file     svc_swup_ip_download.c
	@brief    svc_swup_ip_download.c

	Description: \n
	Module: \n
	Remarks :\n

	Copyright (c) 2013 HUMAX Co., Ltd. \n
	All rights reserved. \n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

#include <pal_sys.h>

#include <svc_swup.h>
//#include <svc_sys.h>

#include "_svc_swup.h"

#include <libxml/tree.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define SVC_IP_UPGRADE_DOWNLOAD_TIMER	(5)
#define SVC_IP_UPGRADE_CHECK_BUFSIZE		(2048)
#if defined (CONFIG_NAGRA_SSD_OTA_TEST)
#define SVC_IP_UPGRADE_FTP_IDPW			"HUMAX:nLS9ahzh"
#else
#define SVC_IP_UPGRADE_FTP_IDPW			"ota:humax@!"
#endif
#define SVC_IP_UPGRADE_HTTP_PREFIX		"http://"
#define SVC_IP_UPGRADE_HTTPS_PREFIX		"https://"
#define SVC_IP_UPGRADE_FTP_PREFIX		"ftp://"
#define MAX_UPGRADE_BUF_SIZE 	(1024*100) //HxTransport_http 의 TRANSPORT_MAX_BUF_SIZE와 동기화 필요
#define MW_IPUP_ASSERT(x)		\
	if ( x == NULL )\
	{\
		HxLOG_Error("MW_MP: the param is NULL.\n");\
		return ERR_FAIL;\
	}\

/*******************************************************************/
/********************	   Typedef		   *************************/
/*******************************************************************/

typedef enum
{
	eIP_UPGRADE_HTTP,
	eIP_UPGRADE_FTP,
	eIP_UPGRADE_UNDEFINED
} SvcIpUpgrade_Type_e;

typedef enum
{
	eIP_UPGRADE_STATE_IDLE,
	eIP_UPGRADE_STATE_DOWNLOAD_START,
	eIP_UPGRADE_STATE_DOWNLOADING,
	eIP_UPGRADE_STATE_DOWNLOAD_FAILED,
	eIP_UPGRADE_STATE_DOWNLOAD_STOPPED,
	eIP_UPGRADE_STATE_DOWNLOAD_FINISHED,
} SvcIpUpgrade_State_e;

typedef struct _IP_Upgrade_t
{
	SvcIpUpgrade_State_e	 state;
	HxTRANSPORT_Http_t 		*pHttp;
	HxTRANSPORT_Ftp_t		*pFtp;
	SvcIpUpgrade_Type_e		 upgradeType;
	HINT8					*url;
	HBOOL					 bStop;
	HINT8					*downloadPath;
	HFLOAT64				 fDownloadSize;
} SvcIpUpgrade_Context_t;

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/


inline STATIC SvcIpUpgrade_Context_t *svc_swup_IpGetContext(SvcSwup_IpUpgradeHandle_t *hUpgrade)
{
	return (SvcIpUpgrade_Context_t *)hUpgrade;
}


STATIC void svc_swup_IpHttpUpgradeCallback(HxTRANSPORT_e event, HINT32 param1, HINT32 param2)
{
	STATIC	HINT32		__nCurrentHttpProgress = -1;
	SvcIpUpgrade_Context_t *handle = NULL;
	HINT32	nProgress;

	handle = (SvcIpUpgrade_Context_t *)param1;

	if(handle == NULL)
	{
		svc_swup_UtilPostMsgToMgr(eSEVT_SWUP_DOWNLOAD_FAIL, 0, (HINT32)handle, (HINT32)eSWUP_ERROR_IP_NO_DATA, 0);
		return;
	}

	switch(event)
	{
		case eHxTRANSPORT_EVENT_PROGRESS_CHANGED:
		{
			if(handle->fDownloadSize == 0)
			{
				if(handle->pHttp)
				{
					HLIB_TRANSPORT_HttpHttpGetFileSize(handle->pHttp, &(handle->fDownloadSize));
				}
			}
			{
				if (handle->fDownloadSize > 0)
					nProgress = ((HFLOAT64)param2 * 100) / (HFLOAT64)handle->fDownloadSize;
				else
					nProgress = 0;

				if (nProgress != __nCurrentHttpProgress)
				{
					HxLOG_Print("param2 [%d] handle->fDownloadSize [%d]\n", param2, (HINT32)handle->fDownloadSize);
					__nCurrentHttpProgress = nProgress;
					svc_swup_UtilPostMsgToMgr(eSEVT_SWUP_DOWNLOAD_PROGRESS, 0, param1, 100, (HINT32)__nCurrentHttpProgress);   // handle, full size , current size
					//svc_swup_UtilPostMsgToMgr(eSEVT_SWUP_DOWNLOAD_PROGRESS, NULL, param1, (HINT32)handle->fDownloadSize, param2);   // handle, full size , current size
				}
			}

			break;
		}

		case eHxTRANSPORT_EVENT_COMPLETED:
		case eHxTRANSPORT_EVENT_DOWNLOADSTOP:
		case eHxTRANSPORT_EVENT_UNDEFINED:
		default:
			break;
	}
}

STATIC void svc_swup_IpFtpUpgradeCallback(HxTRANSPORT_e event, HINT32 param1, HINT32 param2)
{
	STATIC	HINT32		__nCurrentFtpProgress = -1;
	SvcIpUpgrade_Context_t *handle = NULL;
	HINT32	nProgress;

	handle = (SvcIpUpgrade_Context_t *)param1;

	if(handle == NULL)
	{
		svc_swup_UtilPostMsgToMgr(eSEVT_SWUP_DOWNLOAD_FAIL, 0, (HINT32)handle, eSWUP_ERROR_IP_NO_DATA, 0);
		return;
	}

	switch(event)
	{
		case eHxTRANSPORT_EVENT_PROGRESS_CHANGED:
		{
			if(handle->fDownloadSize == 0)
			{
				if(handle->pHttp)
				{
					HLIB_TRANSPORT_FtpGetSize(handle->pFtp, &(handle->fDownloadSize));
				}
			}
//			svc_swup_UtilPostMsgToMgr(eSEVT_SWUP_DOWNLOAD_PROGRESS, NULL, param1, (HINT32)handle->fDownloadSize, param2);   // handle, full size , current size

			{
				if (handle->fDownloadSize > 0)
					nProgress = ((HFLOAT64)param2 * 100) / (HFLOAT64)handle->fDownloadSize;
				else
					nProgress = 0;

				if (nProgress != __nCurrentFtpProgress)
				{
					HxLOG_Print("param2 [%d] handle->fDownloadSize [%d]\n", param2, (HINT32)handle->fDownloadSize);
					__nCurrentFtpProgress = nProgress;
					svc_swup_UtilPostMsgToMgr(eSEVT_SWUP_DOWNLOAD_PROGRESS, 0, param1, 100, (HINT32)__nCurrentFtpProgress);   // handle, full size , current size
					//svc_swup_UtilPostMsgToMgr(eSEVT_SWUP_DOWNLOAD_PROGRESS, NULL, param1, (HINT32)handle->fDownloadSize, param2);   // handle, full size , current size
				}
			}


			break;
		}

		case eHxTRANSPORT_EVENT_COMPLETED:
		case eHxTRANSPORT_EVENT_DOWNLOADSTOP:
		case eHxTRANSPORT_EVENT_UNDEFINED:
		default:
			break;
	}
}

STATIC HERROR svc_swup_IpParseXml(const char *szData, SvcSwup_IpUpgradeInfo_t *pInfo)
{
	HINT8* szPath = NULL;
	HINT8 *szNewSWVersion = NULL;
	HINT8 szSystemID[256] = {0};
	xmlDocPtr doc;
	xmlNodePtr nodeLevel1;
	xmlNodePtr nodeLevel2;
	xmlNodePtr nodeLevel3;
	HINT8 *p = NULL;
	HINT8 szFullPath[MAX_FILE_PATH_LENGTH];
	HINT8 *szDirPath = NULL;
	HINT8 *szFileName = NULL;
	HINT8 *szID = NULL;
	HINT8 *szPassword = NULL;
	HINT8 *szIP = NULL;
	char *pToken1 = NULL;
	char *pToken2 = NULL;
	int i;
	int nVersion[3];

	if ( pInfo == NULL)
	{
		return ERR_FAIL;
	}

	pInfo->bAvailable = TRUE;
	pInfo->bEnable = FALSE;
	szPath = pInfo->szSoftwarePath;
	szNewSWVersion = pInfo->szSoftwareVersion;
	szDirPath= pInfo->aDirPath;
	szFileName = pInfo->aFileName;
	szID = pInfo->aUserName;
	szPassword = pInfo->aPasswd;
	szIP = pInfo->aServerIP;

	doc = xmlParseMemory(szData, MAX_UPGRADE_BUF_SIZE);

	if(doc == NULL)
	{
		return ERR_FAIL;
	}

	for( nodeLevel1 = doc->children->children; nodeLevel1 != NULL; nodeLevel1 = nodeLevel1->next)
	{
		for( nodeLevel2 = nodeLevel1->children; nodeLevel2 != NULL; nodeLevel2 = nodeLevel2->next)
		{
			if( strcmp((char *)(nodeLevel2->name), "available") == 0 )
			{
				if( strcmp((char *)xmlNodeGetContent(nodeLevel2), "0") == 0 )
				{
					pInfo->bAvailable = FALSE;
				}

				strncpy((char *)szSystemID, (char *)xmlNodeGetContent(nodeLevel2), 255);
				szSystemID[255] = '\0';
			}
			else if( strcmp((char *)(nodeLevel2->name), "forced") == 0 )
			{
				if(strcmp((char *)xmlNodeGetContent(nodeLevel2), "False") == 0)
				{
					pInfo->bForced= FALSE;
				}
				else
				{
					pInfo->bForced= TRUE;
				}
			}
			else if( strcmp((char *)(nodeLevel2->name), "systemid") == 0 )
			{
				strncpy((char *)szSystemID, (char *)xmlNodeGetContent(nodeLevel2), 255);
				szSystemID[255] = '\0';
			}
			else if( strcmp((char *)(nodeLevel2->name), "version") == 0 )
			{
				strncpy((char *)szNewSWVersion, (char *)xmlNodeGetContent(nodeLevel2), MAX_VERSION_LENGTH - 1);
				szNewSWVersion[MAX_VERSION_LENGTH - 1] = '\0';

				p = szNewSWVersion;
				pInfo->nSWVersion = 0;

				if( p == NULL )
				{
					pInfo->nSWVersion = 0;
					break;
				}

				pToken1 = p;
				for( i = 0; i < 3; i++ )
				{
					nVersion[i] = atoi( pToken1 );
					pToken2 = strchr(pToken1, '.');
					if( pToken2 == NULL )
						break;

					pToken1 = ++pToken2;
				}

				pInfo->nSWVersion = pInfo->nSWVersion | ( nVersion[0] << 16 );
				pInfo->nSWVersion = pInfo->nSWVersion | ( nVersion[1] << 8 );
				pInfo->nSWVersion = pInfo->nSWVersion | ( nVersion[2] << 0 );
			}

			else if( strcmp((char *)(nodeLevel2->name), "url") == 0 )
			{
				strncpy((char *)szPath, (char *)xmlNodeGetContent(nodeLevel2), MAX_PATH_LENGTH - 1);
				szPath[MAX_PATH_LENGTH - 1] = '\0';
			}
			else if( strcmp((char *)(nodeLevel2->name), "descriptions") == 0 )
			{
				int i = 0;
				for( nodeLevel3 = nodeLevel2->children; nodeLevel3 != NULL; nodeLevel3 = nodeLevel3->next)
				{
					if( strcmp((char *)(nodeLevel3->name), "desc") == 0 )
					{
						if( strcmp((char *)(nodeLevel3->properties->name), "lang") == 0 )
						{
							strncpy((char *)pInfo->aDescripLang[i], (char *)nodeLevel3->properties->children->content, MAX_DESCRIPTION_LANG_LENGTH - 1);
							pInfo->aDescripLang[i][MAX_DESCRIPTION_LANG_LENGTH - 1] = '\0';

							strncpy((char *)pInfo->aDescriptions[i], (char *)xmlNodeGetContent(nodeLevel3), MAX_DESCRIPTION_LENGTH - 1);
							pInfo->aDescriptions[i][MAX_DESCRIPTION_LENGTH - 1] = '\0';

							i++;
						}
					}
				}
				pInfo->nDescripCount = i;
			}
			else if( strcmp((char *)(nodeLevel2->name), "emergency_OTA") == 0 )
			{
				for( nodeLevel3 = nodeLevel2->children; nodeLevel3 != NULL; nodeLevel3 = nodeLevel3->next)
				{
					if( strcmp((char *)(nodeLevel3->name), "IP") == 0 )
					{
						strncpy((char *)szIP, (char *)xmlNodeGetContent(nodeLevel3), MAX_IP_ADDR_LENGTH - 1);
						szIP[MAX_IP_ADDR_LENGTH - 1] = '\0';
					}
					else if( strcmp((char *)(nodeLevel3->name), "enable") == 0 )
					{
						if(strcmp((char *)xmlNodeGetContent(nodeLevel3), "1") == 0)
						{
							pInfo->bEnable = TRUE;
						}
						else
						{
							pInfo->bEnable = FALSE;
						}
					}
					else if( strcmp((char *)(nodeLevel3->name), "ID") == 0 )
					{
						strncpy((char *)szID, (char *)xmlNodeGetContent(nodeLevel3), MAX_USER_LENGTH - 1);
						szID[MAX_USER_LENGTH - 1] = '\0';
					}
					else if( strcmp((char *)(nodeLevel3->name), "Password") == 0 )
					{
						strncpy((char *)szPassword, (char *)xmlNodeGetContent(nodeLevel3), MAX_PASSWD_LENGTH - 1);
						szPassword[MAX_PASSWD_LENGTH - 1] = '\0';
					}
					else if( strcmp((char *)(nodeLevel3->name), "FilePath") == 0 )
					{
						strncpy((char *)szFullPath, (char *)xmlNodeGetContent(nodeLevel3), MAX_FILE_PATH_LENGTH - 1);
						szFullPath[MAX_FILE_PATH_LENGTH - 1] = '\0';

						pToken1 = strchr(szFullPath, '/');
						if( NULL != pToken1 )
						{
							pToken2 = strchr(++pToken1, '/');
							if( NULL != pToken2 )
							{
								strncpy((char *)szDirPath, szFullPath, strlen(pToken1) - strlen(pToken2) + 1);
								szDirPath[strlen(pToken1) - strlen(pToken2) + 1] = '\0';

								pToken2 = pToken2 + 1;
								strncpy((char *)szFileName, pToken2, strlen(pToken2) + 1);
								szFileName[strlen(pToken2) + 1] = '\0';
							}
						}
					}
				}
			}

		}
	}

	xmlFreeDoc(doc);
	return ERR_OK;
}

STATIC HERROR svc_swup_IpDownload(SvcIpUpgrade_Context_t *up)
{
	HERROR eRet = ERR_OK;
	HFLOAT64 ullDownloadSize = 0;

	MW_IPUP_ASSERT(up);

	up->pHttp= HLIB_TRANSPORT_HttpOpen();
	if ( up->pHttp == NULL )
	{
		svc_swup_UtilPostMsgToMgr(eSEVT_SWUP_DOWNLOAD_FAIL, 0, (HINT32)(SvcSwup_IpUpgradeHandle_t *)up, eSWUP_ERROR_IP_NETWORK, 0);
		return ERR_FAIL;
	}

	HLIB_TRANSPORT_HttpSetListener(up->pHttp, svc_swup_IpHttpUpgradeCallback, (SvcSwup_IpUpgradeHandle_t *)up);

	if( up->upgradeType == eIP_UPGRADE_HTTP )
	{
		eRet = HLIB_TRANSPORT_HttpGetAndStore(up->pHttp, up->url, up->downloadPath, &ullDownloadSize);
		if ( eRet != ERR_OK )
		{
			if(up->bStop)
			{
				up->state = eIP_UPGRADE_STATE_DOWNLOAD_STOPPED;
				up->bStop = FALSE;
				HLIB_TRANSPORT_HttpClose(up->pHttp);
				return ERR_OK;
			}


			svc_swup_UtilPostMsgToMgr(eSEVT_SWUP_DOWNLOAD_FAIL, 0, (HINT32)(SvcSwup_IpUpgradeHandle_t *)up, eSWUP_ERROR_WRITE_FILE, 0);
			HLIB_TRANSPORT_HttpClose(up->pHttp);
			return ERR_FAIL;
		}
	}

	HLIB_TRANSPORT_HttpClose(up->pHttp);

	svc_swup_UtilPostMsgToMgr(eSEVT_SWUP_DOWNLOAD_SUCCESS, 0, (HINT32)(SvcSwup_IpUpgradeHandle_t *)up, 0, 0);

	return ERR_OK;
}

STATIC HERROR svc_swup_IpFtpDownload(SvcIpUpgrade_Context_t *up)
{
	HERROR hErr = ERR_OK;
	HFLOAT64 ullDownloadSize = 0;

	MW_IPUP_ASSERT(up);

	up->pFtp = HLIB_TRANSPORT_FtpOpen();
	if ( up->pFtp == NULL )
	{
		svc_swup_UtilPostMsgToMgr(eSEVT_SWUP_DOWNLOAD_FAIL, 0,  (HINT32)(SvcSwup_IpUpgradeHandle_t *)up, eSWUP_ERROR_IP_NETWORK, 0);
		return ERR_FAIL;
	}

	HLIB_TRANSPORT_FtpSetListener(up->pFtp, (HxFtp_callback)svc_swup_IpFtpUpgradeCallback, (void *)up);

	hErr = HLIB_TRANSPORT_FtpSetUserPwd(up->pFtp, SVC_IP_UPGRADE_FTP_IDPW);
	if ( hErr != ERR_OK )
	{
		svc_swup_UtilPostMsgToMgr(eSEVT_SWUP_DOWNLOAD_FAIL, 0, (HINT32)(SvcSwup_IpUpgradeHandle_t *)up, eSWUP_ERROR_IP_IDPW, 0);
		HLIB_TRANSPORT_FtpClose(up->pFtp);
		return ERR_FAIL;
	}

	hErr = HLIB_TRANSPORT_FtpDownload(up->pFtp, up->url, up->downloadPath, &ullDownloadSize);
	if ( hErr != ERR_OK )
	{
		if(up->bStop)
		{
			up->state = eIP_UPGRADE_STATE_DOWNLOAD_STOPPED;
			up->bStop = FALSE;
			HLIB_TRANSPORT_FtpClose(up->pFtp);
			return ERR_OK;
		}

		svc_swup_UtilPostMsgToMgr(eSEVT_SWUP_DOWNLOAD_FAIL, 0, (HINT32)(SvcSwup_IpUpgradeHandle_t *)up, eSWUP_ERROR_WRITE_FILE, 0);
		HLIB_TRANSPORT_FtpClose(up->pFtp);
		return ERR_FAIL;
	}

	HLIB_TRANSPORT_FtpClose(up->pFtp);
	svc_swup_UtilPostMsgToMgr(eSEVT_SWUP_DOWNLOAD_SUCCESS, 0, (HINT32)(SvcSwup_IpUpgradeHandle_t *)up, 0, 0);

	return ERR_OK;
}

STATIC HERROR svc_swup_IpFtpGetSize( HxTRANSPORT_Ftp_t *ptFtp, HFLOAT64 *pullSize )
{
	HERROR eRet = ERR_OK;

	MW_IPUP_ASSERT(pullSize);
	MW_IPUP_ASSERT(ptFtp);

	eRet = HLIB_TRANSPORT_FtpGetSize(ptFtp, pullSize);
	if ( eRet != ERR_OK )
	{
		HLIB_TRANSPORT_FtpClose(ptFtp);
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR svc_swup_IpHttpGetSize(HxTRANSPORT_Http_t *ptHttp, HFLOAT64 *pullSize)
{
	HERROR eRet = ERR_OK;

	MW_IPUP_ASSERT(pullSize);
	MW_IPUP_ASSERT(ptHttp);

	eRet = HLIB_TRANSPORT_HttpHttpGetFileSize(ptHttp, pullSize);
	if ( eRet != ERR_OK )
	{
		HLIB_TRANSPORT_HttpClose(ptHttp);
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC void svc_swup_IpDownloadTask(void *arg)
{
	SvcIpUpgrade_Context_t *up = (SvcIpUpgrade_Context_t *)arg;
	if ( up == NULL ) return;

	up->state = eIP_UPGRADE_STATE_DOWNLOADING;
	switch ( up->upgradeType )
	{
	case eIP_UPGRADE_HTTP:
		svc_swup_IpDownload(up);
		up->state = eIP_UPGRADE_STATE_DOWNLOAD_FINISHED;
		break;

	case eIP_UPGRADE_FTP:
		svc_swup_IpFtpDownload(up);
		up->state = eIP_UPGRADE_STATE_DOWNLOAD_FINISHED;
		break;
	case eIP_UPGRADE_UNDEFINED:
		up->state = eIP_UPGRADE_STATE_DOWNLOAD_FAILED;
	default:
		break;
	}

	return;
}

/*-------------------------------------------------------------------------------------
 *	Global function
 *-------------------------------------------------------------------------------------*/

SvcSwup_IpUpgradeHandle_t *svc_swup_IpUpgradeCreate(void)
{
	SvcIpUpgrade_Context_t * pUpgrade = NULL;

	pUpgrade = (SvcIpUpgrade_Context_t *)HLIB_STD_MemAlloc(sizeof(*pUpgrade));
	if ( pUpgrade == NULL )
	{
		svc_swup_UtilPostMsgToMgr(eSEVT_SWUP_DETECT_FAIL, 0, (HINT32)pUpgrade, eSWUP_ERROR_IP_NETWORK, 0);
		return NULL;
	}

	pUpgrade->state = eIP_UPGRADE_STATE_IDLE;
	pUpgrade->pFtp = NULL;
	pUpgrade->pHttp = NULL;
	pUpgrade->upgradeType = eIP_UPGRADE_UNDEFINED;
	pUpgrade->url = NULL;
	pUpgrade->downloadPath = NULL;
	pUpgrade->fDownloadSize = 0;

	return (SvcSwup_IpUpgradeHandle_t *)pUpgrade;

}

void svc_swup_IpUpgradeDestroy(SvcSwup_IpUpgradeHandle_t *hUpgrade)
{
	SvcIpUpgrade_Context_t *up = NULL;

	if ( hUpgrade == NULL )
		return;

	up = svc_swup_IpGetContext(hUpgrade);

	if ( up->pFtp ) HLIB_TRANSPORT_FtpClose(up->pFtp);
	if ( up->pHttp ) HLIB_TRANSPORT_HttpClose(up->pHttp);
	if ( up->url ) HLIB_STD_MemFree(up->url);
	if ( up->downloadPath ) HLIB_STD_MemFree(up->downloadPath);

	up->state = eIP_UPGRADE_STATE_IDLE;
	up->upgradeType = eIP_UPGRADE_UNDEFINED;

	HLIB_STD_MemFree(up);
	up = NULL;

}

HERROR svc_swup_IpUpgradeCheckSoftware(SvcSwup_IpUpgradeHandle_t *hUpgrade, const HINT8 *url, SvcSwup_IpUpgradeInfo_t *pInfo)
{
	HERROR eRet = ERR_OK;
	HxTRANSPORT_Http_t *pHttp = NULL;
	HUINT32 ulHttpCode 	= 0;
	HFLOAT64 ullDownloadSize = 0;
	HINT8 *checkBuf		= NULL;

	MW_IPUP_ASSERT(hUpgrade);
	MW_IPUP_ASSERT(url);
	MW_IPUP_ASSERT(pInfo);

	pHttp = HLIB_TRANSPORT_HttpOpen();
	if ( pHttp == NULL )
	{
		svc_swup_UtilPostMsgToMgr(eSEVT_SWUP_DETECT_FAIL, 0, (HINT32)hUpgrade, eSWUP_ERROR_IP_NETWORK, 0);
		return ERR_FAIL;
	}

	eRet = HLIB_TRANSPORT_HttpSetTimeout(pHttp, SVC_IP_UPGRADE_DOWNLOAD_TIMER);
	if ( eRet != ERR_OK )
	{
		svc_swup_UtilPostMsgToMgr(eSEVT_SWUP_DETECT_FAIL, 0, (HINT32)hUpgrade, eSWUP_ERROR_IP_TIMEOUT, 0);
		HLIB_TRANSPORT_HttpClose(pHttp);
		return ERR_FAIL;
	}

	eRet = HLIB_TRANSPORT_HttpGet(pHttp, url, &checkBuf, &ullDownloadSize);
	if ( eRet != ERR_OK )
	{
		svc_swup_UtilPostMsgToMgr(eSEVT_SWUP_DETECT_FAIL, 0, (HINT32)hUpgrade, eSWUP_ERROR_IP_NETWORK, 0);
		HLIB_TRANSPORT_HttpFreeBuffer(checkBuf);
		HLIB_TRANSPORT_HttpClose(pHttp);
		return ERR_FAIL;
	}

	eRet = HLIB_TRANSPORT_HttpGetHttpResponseCode(pHttp, &ulHttpCode);
	if( ulHttpCode != eHxTP_HTTP_RESCODE_OK )
	{
		svc_swup_UtilPostMsgToMgr(eSEVT_SWUP_DETECT_FAIL, 0, (HINT32)hUpgrade, eSWUP_ERROR_IP_NOT_FOUND_PAGE, 0);
		HLIB_TRANSPORT_HttpFreeBuffer(checkBuf);
		HLIB_TRANSPORT_HttpClose(pHttp);
		return ERR_FAIL;
	}

	eRet = svc_swup_IpParseXml(checkBuf, pInfo);
	if ( eRet != ERR_OK )
	{
		svc_swup_UtilPostMsgToMgr(eSEVT_SWUP_DETECT_FAIL, 0, (HINT32)hUpgrade, eSWUP_ERROR_IP_NO_DATA, 0);
		HLIB_TRANSPORT_HttpFreeBuffer(checkBuf);
		HLIB_TRANSPORT_HttpClose(pHttp);
		return ERR_FAIL;
	}

	HLIB_TRANSPORT_HttpFreeBuffer(checkBuf);
	HLIB_TRANSPORT_HttpClose(pHttp);

	return ERR_OK;
}

HERROR svc_swup_IpUpgradeDownloadStart(SvcSwup_IpUpgradeHandle_t *hUpgrade, const HINT8 *url, const HINT8 *downloadPath)
{
	HULONG	ulTaskId = 0;
	SvcIpUpgrade_Context_t *up = NULL;

	MW_IPUP_ASSERT(hUpgrade);
	MW_IPUP_ASSERT(url);
	MW_IPUP_ASSERT(downloadPath);

	up = svc_swup_IpGetContext(hUpgrade);

	if ( strstr(url, SVC_IP_UPGRADE_HTTP_PREFIX) )
	{
		up->upgradeType = eIP_UPGRADE_HTTP;
	}
	else if ( strstr(url, SVC_IP_UPGRADE_HTTPS_PREFIX) )
	{
		up->upgradeType = eIP_UPGRADE_HTTP;
	}
	else if ( strstr(url, SVC_IP_UPGRADE_FTP_PREFIX) )
	{
		up->upgradeType = eIP_UPGRADE_FTP;
	}
	else
	{
		up->upgradeType = eIP_UPGRADE_UNDEFINED;
		svc_swup_UtilPostMsgToMgr(eSEVT_SWUP_DOWNLOAD_FAIL, 0, (HINT32)hUpgrade, eSWUP_ERROR_UNDEFINED, 0);
		return ERR_FAIL;
	}

	up->state = eIP_UPGRADE_STATE_DOWNLOAD_START;
	up->url = VK_StrDup((void *)url);
	up->downloadPath = VK_StrDup((void *)downloadPath);
	up->bStop = FALSE;

	VK_TASK_Create(svc_swup_IpDownloadTask, 40, 0x4000, "DownloadTask", (void *)up, &ulTaskId, 0);
	VK_TASK_Start(ulTaskId);

//	SVC_SYS_DisplayPanelString (ePANEL_STR_PRIORITY_BG_ACTION, "D00%", eDxPANEL_ALIGN_CENTER);

	return ERR_OK;
}

HERROR svc_swup_IpUpgradeDownloadStop(SvcSwup_IpUpgradeHandle_t *hUpgrade)
{
	SvcIpUpgrade_Context_t *up = NULL;

	MW_IPUP_ASSERT(hUpgrade);
	up = svc_swup_IpGetContext(hUpgrade);

	switch(up->upgradeType)
	{
	case eIP_UPGRADE_HTTP:
		if(up->state == eIP_UPGRADE_STATE_DOWNLOADING)
			return HLIB_TRANSPORT_HttpDownloadStop(up->pHttp, &(up->bStop));
		break;

	case eIP_UPGRADE_FTP:
		if(up->state == eIP_UPGRADE_STATE_DOWNLOADING)
			return HLIB_TRANSPORT_FtpDownloadStop(up->pFtp, &(up->bStop));
		break;

	case eIP_UPGRADE_UNDEFINED:

	default:
		break;
	}

	return ERR_FAIL;

}

HERROR svc_swup_IpUpgradeGetSize(SvcSwup_IpUpgradeHandle_t *hUpgrade, HFLOAT64 *pullSize)
{
	SvcIpUpgrade_Context_t *up = NULL;

	MW_IPUP_ASSERT(hUpgrade);
	MW_IPUP_ASSERT(pullSize);

	up = svc_swup_IpGetContext(hUpgrade);

	switch(up->upgradeType)
	{
	case eIP_UPGRADE_HTTP:
		if(up->state == eIP_UPGRADE_STATE_DOWNLOADING)
			return svc_swup_IpHttpGetSize(up->pHttp, pullSize);
		break;

	case eIP_UPGRADE_FTP:
		if(up->state == eIP_UPGRADE_STATE_DOWNLOADING)
			return svc_swup_IpFtpGetSize(up->pFtp, pullSize);
		break;

	case eIP_UPGRADE_UNDEFINED:

	default:
		break;
	}

	return ERR_FAIL;
}

HERROR svc_swup_IpUpgradeGetDownloadedSize(SvcSwup_IpUpgradeHandle_t *hUpgrade, HFLOAT64 *pSize)
{
	SvcIpUpgrade_Context_t *up = NULL;

	MW_IPUP_ASSERT(hUpgrade);
	MW_IPUP_ASSERT(pSize);

	up = svc_swup_IpGetContext(hUpgrade);

	switch(up->upgradeType)
	{
	case eIP_UPGRADE_HTTP:
		if(up->state == eIP_UPGRADE_STATE_DOWNLOADING)
			return HLIB_TRANSPORT_HttpDownloadedSize(up->pHttp, pSize);
		break;

	case eIP_UPGRADE_FTP:
		if(up->state == eIP_UPGRADE_STATE_DOWNLOADING)
			return HLIB_TRANSPORT_FtpDownloadedSize(up->pFtp, pSize);
		break;

	case eIP_UPGRADE_UNDEFINED:

	default:
		break;
	}

	return ERR_FAIL;
}



