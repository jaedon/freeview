#include <unistd.h>
#include <hlib.h>
#include "_dlib_port_humax_epg.h"

#define USE_CRC_CHECK

#ifdef CONFIG_OP_UK_DTT
#define CONFIG_FUNC_IPEPG_USE_KEYWORDSEARCH
#endif

#define	MAX_IPEPG_SERVICE_NUM			16
/* 현재 TVTV EPG 구성
    MAX 5000개 채널까지만 수신 - 그런데 size 문제로인해 현재 130개의 채널만 수신되도록 fix되어 있다.
    14일을 송출하고 있지만 size 문제로인해 7일만 수신하고 있다.
    1day , 1 channel event number : max 30분당 1개 : 24*2 = 48 개
    total event info : 5000 * 14 * 48 = 3360000
*/
#define MAX_EVENT_NUMBER				3360000

// File structure
//
//	{	4 byte		|	4 byte						|	(event count * dlib_epgevt_t) byte	|
//	{	event count	|	CRC of dlib_epgevt_tdata	|	dlib_epgevt_t data					|	raw data
//

#define IPEPG_MAKE_KEY(key)	((HUINT32)(key&0x00000000FFFFFFFF))

#define KEYWORD_CRID_PROGRAM 			"pCRID:"
#define KEYWORD_CRID_SERIES 			"sCRID:"
#define KEYWORD_CRID_DLIMITER			","

#if defined(CONFIG_IPEPG_REPOSITORY_HDD)
#define IPEPG_REPO_PATH_LEN				128
#endif
#define IPEPG_REPO_EMPTY_PATH			"NOT_VALID"

/* Defines of Debug Options */
//#define DLIB_PORT_HUMAX_EPG_SEE_PARAMETERS
//#define DLIB_PORT_HUMAX_EPG_SEE_DUMPDATA

typedef	struct
{
	HUID	uid;
	HINT32	size;
	HUINT8	data[];
} dlib_epgevt_t;

typedef	struct
{
	HCHAR			*pszUid;

	HBOOL			bIsChanged;
	HUINT32			ulLastUsed;
	HCHAR			*pszRepositoryName;
	HxHASH_t	*pstEventTable;
} dlib_epgsvc_t;

static	HxHASH_t	*s_pstServiceEventTable = NULL;

typedef struct
{
	DxField_t		field;
	HBOOL			(* getData) (DxField_t *field, const void *src, HUINT32 size);
} DxEPG_Field_t;


#define	RETURN_IF_VOID(expr)	do{if(expr)return;}while(0)
#define	RETURN_IF(expr, err)	do{if(expr)return (err);}while(0)
#define	GOTO_IF(expr, pos)		do{if(expr)goto pos;}while(0)

#define	D_RETURN_IF_VOID(expr)	do{if(expr){HxLOG_Debug("%s failed!!\n", #expr);return;}}while(0)
#define	D_RETURN_IF(expr, err)	do{if(expr){HxLOG_Debug("%s failed!!\n", #expr);return (err);}}while(0)
#define	D_GOTO_IF(expr, pos)	do{if(expr){HxLOG_Debug("%s failed!!\n", #expr);goto pos;}}while(0)

#define	W_RETURN_IF_VOID(expr)	do{if(expr){HxLOG_Warning("%s failed!!\n", #expr);return;}}while(0)
#define	W_RETURN_IF(expr, err)	do{if(expr){HxLOG_Warning("%s failed!!\n", #expr);return (err);}}while(0)
#define	W_GOTO_IF(expr, pos)	do{if(expr){HxLOG_Warning("%s failed!!\n", #expr);goto pos;}}while(0)

#define	E_RETURN_IF_VOID(expr)	do{if(expr){HxLOG_Error("%s failed!!\n", #expr);return;}}while(0)
#define	E_RETURN_IF(expr, err)	do{if(expr){HxLOG_Error("%s failed!!\n", #expr);return (err);}}while(0)
#define	E_GOTO_IF(expr, pos)	do{if(expr){HxLOG_Error("%s failed!!\n", #expr);goto pos;}}while(0)
#define	IPEPG_Get8Bit(a) 		(HUINT8)((((*((HUINT8*)(a))))))
#define	IPEPG_Get16Bit(a) 		(HUINT16)((((*((HUINT8 *)(a+1)) << 8) | (*((HUINT8*)(a))))))
#define	IPEPG_Get32Bit(a)		(HUINT32)((((*((HUINT8 *)(a)+3) << 24) | (*((HUINT8*)(a)+2) << 16) |(*((HUINT8*)(a)+1) << 8) |(*((HUINT8*)(a))))))

#if defined(CONFIG_IPEPG_REPOSITORY_HDD) || defined(CONFIG_IPEPG_REPOSITORY_EMMC)
#define IPEPG_REPO_PATH_LEN						128

static HBOOL		 s_bDlibEpgHmx_RepoAvail = FALSE;
static HCHAR		 s_szDlibEpgHmx_RepoPath[IPEPG_REPO_PATH_LEN];
#endif

// for registering the Storage Device
#if defined(CONFIG_IPEPG_REPOSITORY_HDD)
static HBOOL		 s_bDlibEpgHmx_DeviceRegistered = FALSE;
static HCHAR		 s_szDlibEpgHmx_RegisteredDevice[DxSTORAGE_NAME_LEN];
#endif

#define ___FUNCTIONS___

#define _____HUMAX_EPG_REPOSITORY_____
static HBOOL dlib_epg_humax_IsRepositoryAvailable (void)
{
#if defined(CONFIG_IPEPG_REPOSITORY_HDD)
	return s_bDlibEpgHmx_RepoAvail;
#elif defined(CONFIG_IPEPG_REPOSITORY_EMMC)
	return TRUE;
#else
	return FALSE;
#endif
}

static const HCHAR *dlib_epg_humax_GetRepositoryPath (void)
{
#if defined(CONFIG_IPEPG_REPOSITORY_HDD)
	if (TRUE == s_bDlibEpgHmx_RepoAvail)
	{
		return (const HCHAR *)s_szDlibEpgHmx_RepoPath;
	}
	else
	{
		return (const HCHAR *)IPEPG_REPO_EMPTY_PATH;
	}
#elif defined(CONFIG_IPEPG_REPOSITORY_EMMC)
	if (TRUE != s_bDlibEpgHmx_RepoAvail)
	{
		s_bDlibEpgHmx_RepoAvail = TRUE;
		HxSTD_PrintToStrN(s_szDlibEpgHmx_RepoPath, IPEPG_REPO_PATH_LEN, "%s/%s", CONFIG_IPEPG_REPOSITORY_EMMC_PATH, "repository");
	}

	return (const HCHAR *)s_szDlibEpgHmx_RepoPath;
#else
	return (const HCHAR *)IPEPG_REPO_EMPTY_PATH;
#endif
}

#if defined(CONFIG_IPEPG_REPOSITORY_HDD)
static HERROR dlib_epg_humax_SetRepositoryPath (HCHAR *szPath)
{
	if ((NULL == szPath) || ('0' == *szPath))
	{
		HxLOG_Error("szPath is Null...\n");
		return ERR_FAIL;
	}

	s_bDlibEpgHmx_RepoAvail = TRUE;
	HxSTD_PrintToStrN(s_szDlibEpgHmx_RepoPath, IPEPG_REPO_PATH_LEN - 1, "%s/%s", szPath, "repository");
	s_szDlibEpgHmx_RepoPath[IPEPG_REPO_PATH_LEN - 1] = '\0';

	return ERR_OK;
}
static void dlib_epg_humax_ResetRepositoryPath (void)
{
	s_bDlibEpgHmx_RepoAvail = FALSE;
	HxSTD_MemSet(s_szDlibEpgHmx_RepoPath, 0, IPEPG_REPO_PATH_LEN);
}
#endif


#define ____________________

static HxHASH_t	*	dlib_epg_humax_MakeSvcHashTable(void)
{
	s_pstServiceEventTable = HLIB_HASH_NewFull(HLIB_HASH_StrHash, HLIB_HASH_StrEqual, (_HxDestroyNotify)DLIB_Free_CB, (_HxDestroyNotify)DLIB_Free_CB);
	return s_pstServiceEventTable;
}

static void	dlib_epg_humax_Print_Event(DxEvent_t *event);
#if defined(DLIB_PORT_HUMAX_EPG_SEE_DUMPDATA)
void 	dlib_epg_humax_dumpdata(HUINT8 *buf, HUINT32 size)
{
	HUINT32 i = 0;
	HUINT32 index = 0;
	HUINT8 *tempBuf = buf;
	HUINT16	usSize = 0;

	HxLOG_Debug("\ndlib_epg_humax_dumpdata src:%x size[%d]\n", buf, size);
	// DxIpEventBase_t
	HxLOG_Debug("DxIpEventBase_t\n");
	for(i = 0; i < sizeof(DxIpEventBase_t); i++)
	{
		if(i%16 == 0)
		{
			HxLOG_Print("\n");
		}
		HxLOG_Print("%02X ", buf[i]);
	}

	// DxIpString
	HxLOG_Debug("\n\nDxIpString\n");
	tempBuf = tempBuf + sizeof(DxIpEventBase_t);

	for(;i<size; i+=2)
	{
		usSize = IPEPG_Get16Bit(&buf[i]);
		HxLOG_Debug("\n\tINDEX[%d] Size [%d][0x%x]", index++, usSize, usSize);

		if(usSize != 0)
		{
			HUINT32 k = 0;
			for(k = 0;k<usSize + 2; k++)
			{
				if(k%16 == 0)
				{
					HxLOG_Print("\n\t\t");
				}
				HxLOG_Print("%02X ", buf[i+k]);
			}
		}
		i += usSize;
	}
	HxLOG_Debug("\n");
	HxLOG_Debug("\n");
}
#endif
static void dlib_epg_humaxrf_FreeEventString(DxEventString_t *string)
{
	if (string)
	{
		if (string->string)
			DLIB_Free(string->string);
		DLIB_Free(string);
	}
}

static void	dlib_epg_humax_FreeCRID (DxEventCRID_t *item)
{
	if (item)
	{
		if (item->crid)
			DLIB_Free(item->crid);
		DLIB_Free(item);
	}
}

static void	dlib_epg_humax_FreeLink (DxEventLink_t *item)
{
	if (item)
	{
		if (item->pcSubName)	DLIB_Free(item->pcSubName);
		DLIB_Free(item);
	}
}


static void dlib_epg_humax_FreeMediaImage (DxEventMediaImage_t * item)
{
	if(item)
	{
		if(item->pucImageUrl)	DLIB_Free(item->pucImageUrl);
		if(item->pucUrlType)	DLIB_Free(item->pucUrlType);
		DLIB_Free(item);
	}
}
static void dlib_epg_humax_FreeMediaUrl (DxEventMediaUrl_t * item)
{
	if(item)
	{
		if(item->pucTarget)		DLIB_Free(item->pucTarget);
		if(item->pucType)		DLIB_Free(item->pucType);
		if(item->pucUrl)		DLIB_Free(item->pucUrl);
		DLIB_Free(item);
	}
}

static void	dlib_epg_humax_FreeMedia (DxEventMedia_t *item)
{
	if (item)
	{
		if (item->pcMediaId)		DLIB_Free(item->pcMediaId);
		if (item->mediaUrlList)		HLIB_VECTOR_Delete(item->mediaUrlList);
		if (item->mediaImageList)	HLIB_VECTOR_Delete(item->mediaImageList);
		DLIB_Free(item);
	}
}

static void dlib_epg_humax_FreeOnDemandString (DxEventOnDemandString_t * item)
{
	if(item)
	{
		if(item->pucString)		DLIB_Free(item->pucString);
		DLIB_Free(item);
	}
}

static void	dlib_epg_humax_FreeOnDemand (DxEventOnDemand_t *item)
{
	if (item)
	{
		if (item->onDemandStringList)		HLIB_VECTOR_Delete(item->onDemandStringList);
		DLIB_Free(item);
	}
}

static HUINT8	* dlib_epg_humax_Get_ExtBuf(HUINT8 *buf, HUINT32 size)
{
	HUINT32 i = 0, j = 0;
	HUINT8 *tempBuf = buf;
	HUINT16	usSize = 0;
	HUINT16	usCount = 0;
	HUINT16	usCountOfUrl = 0, usCountOfImage = 0;

	tempBuf += sizeof(DxIpEventBase_t);
	usCount = IPEPG_Get16Bit(tempBuf);
	tempBuf += sizeof(HUINT16);

	for(i = 0; i< usCount; i++)
	{
		usSize = IPEPG_Get16Bit(tempBuf);
		tempBuf += (sizeof(HUINT16) + usSize);
		if(usSize != 0) tempBuf += (sizeof(DxIPEPG_Text_e) + 3);
	}

/////////////////////////////////////////// End of STRING

	usCount = IPEPG_Get16Bit(tempBuf);	// Link Count
	tempBuf += sizeof(HUINT16);
	if(usCount)
	{
		for(i = 0; i< usCount; i++)
		{
			tempBuf += (sizeof(HUINT32)*5);		// ulProgId,ulStartTime,ulEndTime,ulEpisodeNum,ulSeason
			usSize = IPEPG_Get16Bit(tempBuf);
			tempBuf += (sizeof(HUINT16) + usSize);			// subName size + string size

			if(usSize != 0)	tempBuf += (sizeof(DxIPEPG_Text_e) + 3);
		}
	}
/////////////////////////////////////////// End of Link

	usCount = IPEPG_Get16Bit(tempBuf);	// Media Count
	tempBuf += sizeof(HUINT16);
	if(usCount)
	{
		for(i = 0; i< usCount; i++)
		{
			tempBuf += (sizeof(HUINT32)*4);	// ulPubDate, ulExpireDate, ulTransmitDate, ulPrice

			// stMediaId
			usSize = IPEPG_Get16Bit(tempBuf);
			tempBuf += (sizeof(HUINT16) + usSize);			// stMediaId size + string size
			if(usSize != 0) { tempBuf += (sizeof(DxIPEPG_Text_e)+3);}

			// ulMediaUrlCount
			usCountOfUrl = IPEPG_Get16Bit(tempBuf);
			tempBuf += sizeof(HUINT16);
			if(usCountOfUrl > 0)
			{
				for (j = 0 ; j < usCountOfUrl; j++)
				{
					// TARGET
					usSize = IPEPG_Get16Bit(tempBuf);
					tempBuf += (sizeof(HUINT16)+usSize);	// TARGET size + string size
					if(usSize != 0) { tempBuf += (sizeof(DxIPEPG_Text_e)+3);}

					// TYPE
					usSize = IPEPG_Get16Bit(tempBuf);
					tempBuf += (sizeof(HUINT16)+usSize);	// TYPE size + string size
					if(usSize != 0) { tempBuf += (sizeof(DxIPEPG_Text_e)+3);}

					// URL
					usSize = IPEPG_Get16Bit(tempBuf);
					tempBuf += (sizeof(HUINT16)+usSize);	// URL size + string size
					if(usSize != 0) { tempBuf += (sizeof(DxIPEPG_Text_e)+3);}
				}
			}
			//------------------------------------

			// ulMediaImageCount
			usCountOfImage= IPEPG_Get16Bit(tempBuf);
			tempBuf += sizeof(HUINT16);
			if(usCountOfImage > 0)
			{
				for (j = 0 ; j < usCountOfImage; j++)
				{
					// Size
					tempBuf += sizeof(HUINT32);

					// TYPE
					usSize = IPEPG_Get16Bit(tempBuf);
					tempBuf += (sizeof(HUINT16)+usSize);	// TYPE size + string size
					if(usSize != 0) { tempBuf += (sizeof(DxIPEPG_Text_e)+3);}

					// URL
					usSize = IPEPG_Get16Bit(tempBuf);
					tempBuf += (sizeof(HUINT16)+usSize);	// URL size + string size
					if(usSize != 0) { tempBuf += (sizeof(DxIPEPG_Text_e)+3);}
				}
			}
		}
	}
/////////////////////////////////////////// End of Media data

	return tempBuf;

}
static HxVector_t * dlib_epg_humax_Get_LinkData(HUINT8 *buf, HUINT32 size)
{
	HUINT32 i = 0;
	HUINT8 *tempBuf = buf;
	HUINT16	usSize = 0;
	HUINT16	usCount = 0;
	HxVector_t * linkList = NULL;
	tempBuf += sizeof(DxIpEventBase_t);

	usCount = IPEPG_Get16Bit(tempBuf);
	tempBuf += sizeof(HUINT16);

	for(i = 0; i< usCount; i++)
	{
		usSize = IPEPG_Get16Bit(tempBuf);
		tempBuf += (sizeof(HUINT16) + usSize);
		if(usSize != 0) tempBuf += (sizeof(DxIPEPG_Text_e) + 3);
	}
/////////////////////////////////////////// End of STRING
	usCount = IPEPG_Get16Bit(tempBuf);	// Link Count
	tempBuf += sizeof(HUINT16);

#if defined(DLIB_PORT_HUMAX_EPG_SEE_PARAMETERS)
	HxLOG_Debug("LINK count[%d]\n", usCount);
#endif
	if(usCount)
	{
		linkList = HLIB_VECTOR_NewEx(NULL, 0, NULL, (void *(*)(void *))dlib_epg_humax_FreeLink, NULL);


		for(i = 0; i< usCount; i++)
		{
			DxEventLink_t	* pLinkInfo = DLIB_Malloc(sizeof(DxEventLink_t));

			pLinkInfo->ulProgId = IPEPG_Get32Bit(tempBuf);
			tempBuf += sizeof(pLinkInfo->ulProgId);
			pLinkInfo->ulStartTime = IPEPG_Get32Bit(tempBuf);
			tempBuf += sizeof(pLinkInfo->ulStartTime);
			pLinkInfo->ulEndTime = IPEPG_Get32Bit(tempBuf);
			tempBuf += sizeof(pLinkInfo->ulEndTime);
			pLinkInfo->ulEpisodeNum = IPEPG_Get32Bit(tempBuf);
			tempBuf += sizeof(pLinkInfo->ulEpisodeNum);
			pLinkInfo->ulSeason = IPEPG_Get32Bit(tempBuf);
			tempBuf += sizeof(pLinkInfo->ulSeason);
#if defined(DLIB_PORT_HUMAX_EPG_SEE_PARAMETERS)
			HxLOG_Debug("LINK[%d:%d] prog[%x] stime[%x] etime[%x] episode[%x] season[%x]\n", usCount, i, pLinkInfo->ulProgId, pLinkInfo->ulStartTime, pLinkInfo->ulEndTime, pLinkInfo->ulEpisodeNum, pLinkInfo->ulSeason);
#endif
			usSize = IPEPG_Get16Bit(tempBuf);
			tempBuf += sizeof(HUINT16);

			if(usSize != 0)
			{
				tempBuf += sizeof(DxIPEPG_Text_e);
				tempBuf += 3;
				pLinkInfo->pcSubName = DLIB_StrDup(tempBuf);
#if defined(DLIB_PORT_HUMAX_EPG_SEE_PARAMETERS)
				HxLOG_Debug("LINK Name size:%d name:%s\n", usSize, pLinkInfo->pcSubName);
#endif
				tempBuf += usSize;
			}
			else
			{
				pLinkInfo->pcSubName = NULL;
			}

			HLIB_VECTOR_Add(linkList,pLinkInfo);
		}
	}
#if defined(DLIB_PORT_HUMAX_EPG_SEE_PARAMETERS)
	HxLOG_Debug("link pointer[%x]\n", tempBuf);
#endif
	return linkList;
}

static HxVector_t * dlib_epg_humax_Get_MediaData(HUINT8 *buf, HUINT32 size)
{
	HUINT32 i = 0, j = 0;
	HUINT8 *tempBuf = buf;
	HUINT16	usSize = 0;
	HUINT16	usCount = 0;
	HUINT16	usCountOfUrl = 0, usCountOfImage = 0;
	HxVector_t * mediaList = NULL;
	HxVector_t * mediaUrlList = NULL;
	HxVector_t * mediaImageList = NULL;

	tempBuf += sizeof(DxIpEventBase_t);
	usCount = IPEPG_Get16Bit(tempBuf);
	tempBuf += sizeof(HUINT16);

	for(i = 0; i< usCount; i++)
	{
		usSize = IPEPG_Get16Bit(tempBuf);
		tempBuf += (sizeof(HUINT16) + usSize);
		if(usSize != 0) tempBuf += (sizeof(DxIPEPG_Text_e) + 3);
	}

/////////////////////////////////////////// End of STRING
	usCount = IPEPG_Get16Bit(tempBuf);	// Link Count
	tempBuf += sizeof(HUINT16);
	if(usCount)
	{
		for(i = 0; i< usCount; i++)
		{
			tempBuf += (sizeof(HUINT32)*5);		// ulProgId,ulStartTime,ulEndTime,ulEpisodeNum,ulSeason
			usSize = IPEPG_Get16Bit(tempBuf);
			tempBuf += (sizeof(HUINT16) + usSize);			// subName size + string size

			if(usSize != 0)	tempBuf += (sizeof(DxIPEPG_Text_e) + 3);
		}
	}
/////////////////////////////////////////// End of Link
#if defined(DLIB_PORT_HUMAX_EPG_SEE_PARAMETERS)
	HxLOG_Debug("link pointer for media[%x]\n", tempBuf);
#endif
	usCount = IPEPG_Get16Bit(tempBuf);	// Media Count
	tempBuf += sizeof(HUINT16);

	if(usCount)
	{
		mediaList = HLIB_VECTOR_NewEx(NULL, 0, NULL, (void *(*)(void *))dlib_epg_humax_FreeMedia, NULL);
		for(i = 0; i< usCount; i++)
		{
			DxEventMedia_t	* pMediaInfo = DLIB_Calloc(sizeof(DxEventMedia_t));

			pMediaInfo->ulPubDate = IPEPG_Get32Bit(tempBuf);
			tempBuf += sizeof(HUINT32);
			pMediaInfo->ulExpireDate 	= IPEPG_Get32Bit(tempBuf);
			tempBuf += sizeof(HUINT32);
			pMediaInfo->ulTransmitDate = IPEPG_Get32Bit(tempBuf);
			tempBuf += sizeof(HUINT32);
			pMediaInfo->ulPrice = IPEPG_Get32Bit(tempBuf);
			tempBuf += sizeof(HUINT32);

			// stMediaId
			usSize = IPEPG_Get16Bit(tempBuf);
			tempBuf += sizeof(HUINT16);
			if(usSize != 0)
			{
				tempBuf += sizeof(DxIPEPG_Text_e);
				tempBuf += 3;
				pMediaInfo->pcMediaId= DLIB_StrDup(tempBuf);
				tempBuf += usSize;
			}

			// ulMediaUrlCount
			usCountOfUrl = IPEPG_Get16Bit(tempBuf);
			tempBuf += sizeof(HUINT16);
			if(usCountOfUrl > 0)
			{
				mediaUrlList = HLIB_VECTOR_NewEx(NULL, 0, NULL, (void *(*)(void*))dlib_epg_humax_FreeMediaUrl,NULL);
				for (j = 0 ; j < usCountOfUrl; j++)
				{
					DxEventMediaUrl_t * pMediaUrlInfo = DLIB_Calloc(sizeof(DxEventMediaUrl_t));

					// TARGET
					usSize = IPEPG_Get16Bit(tempBuf);
					tempBuf += sizeof(HUINT16);
					if(usSize != 0)
					{
						tempBuf += sizeof(DxIPEPG_Text_e);
						tempBuf += 3;
						pMediaUrlInfo->pucTarget = DLIB_StrDup(tempBuf);
						tempBuf += usSize;
					}
					// TYPE
					usSize = IPEPG_Get16Bit(tempBuf);
					tempBuf += sizeof(HUINT16);
					if(usSize != 0)
					{
						tempBuf += sizeof(DxIPEPG_Text_e);
						tempBuf += 3;
						pMediaUrlInfo->pucType= DLIB_StrDup(tempBuf);
						tempBuf += usSize;
					}
					// URL
					usSize = IPEPG_Get16Bit(tempBuf);
					tempBuf += sizeof(HUINT16);
					if(usSize != 0)
					{
						tempBuf += sizeof(DxIPEPG_Text_e);
						tempBuf += 3;
						pMediaUrlInfo->pucUrl= DLIB_StrDup(tempBuf);
						tempBuf += usSize;
					}
					HLIB_VECTOR_Add(mediaUrlList,pMediaUrlInfo);
				}
				pMediaInfo->mediaUrlList = mediaUrlList;
			}
			//------------------------------------

			// ulMediaImageCount
			usCountOfImage= IPEPG_Get16Bit(tempBuf);
			tempBuf += sizeof(HUINT16);
			if(usCountOfImage > 0)
			{
				mediaImageList= HLIB_VECTOR_NewEx(NULL, 0, NULL, (void *(*)(void*))dlib_epg_humax_FreeMediaImage,NULL);
				for (j = 0 ; j < usCountOfImage; j++)
				{
					DxEventMediaImage_t* pMediaImageInfo = DLIB_Calloc(sizeof(DxEventMediaImage_t));

					// Size
					pMediaImageInfo->ulSize = IPEPG_Get32Bit(tempBuf);
					tempBuf += sizeof(HUINT32);

					// TYPE
					usSize = IPEPG_Get16Bit(tempBuf);
					tempBuf += sizeof(HUINT16);
					if(usSize != 0)
					{
						tempBuf += sizeof(DxIPEPG_Text_e);
						tempBuf += 3;
						pMediaImageInfo->pucUrlType = DLIB_StrDup(tempBuf);
						tempBuf += usSize;
					}
					// URL
					usSize = IPEPG_Get16Bit(tempBuf);
					tempBuf += sizeof(HUINT16);
					if(usSize != 0)
					{
						tempBuf += sizeof(DxIPEPG_Text_e);
						tempBuf += 3;
						pMediaImageInfo->pucImageUrl = DLIB_StrDup(tempBuf);
						tempBuf += usSize;
					}
					HLIB_VECTOR_Add(mediaImageList,pMediaImageInfo);
				}
				pMediaInfo->mediaImageList = mediaImageList;
			}

			HLIB_VECTOR_Add(mediaList,pMediaInfo);
		}
	}
	return mediaList;
}

static HxVector_t * dlib_epg_humax_Get_CRID(HUINT8 *buf, HUINT32 size)
{
#if !defined(CONFIG_PROD_FVP4000T)
	return NULL;
#else
	HUINT32 i = 0;
	HUINT8 *tempBuf = buf;
	HUINT16	usCount = 0;
	HxVector_t * cridList = NULL;
	HUINT16	usStrSize;

	tempBuf = dlib_epg_humax_Get_ExtBuf(buf, size);

	usCount = IPEPG_Get16Bit(tempBuf);	// DxIpEvent_t.ext.ukdtt.usCRIDCount
	tempBuf += sizeof(HUINT16);
	if (usCount)
	{
		DxIPEPG_UKDTT_TEXT_e eType = 0;
		HUINT8	Lang[3] = "";

		cridList = HLIB_VECTOR_NewEx(NULL, 0, NULL, (void *(*)(void *))dlib_epg_humax_FreeCRID, NULL);
		for (i=0; i<usCount; i++)
		{
			DxEventCRID_t	*crid = DLIB_Calloc(sizeof(DxEventCRID_t));
			usStrSize = IPEPG_Get16Bit(tempBuf);
			tempBuf += sizeof(HUINT16);
			eType = IPEPG_Get32Bit(tempBuf);
			tempBuf += sizeof(DxIPEPG_Text_e);

			Lang[0] = IPEPG_Get8Bit(tempBuf);
			Lang[1] = IPEPG_Get8Bit(tempBuf+1);
			Lang[2] = IPEPG_Get8Bit(tempBuf+2);
			tempBuf += 3;

			switch (eType)
			{
				case eDxIPEPG_UKDTT_TEXT_PROGRAMME_CRID:
					crid->type = DxCRID_TYPE_PROG;
					break;
				case eDxIPEPG_UKDTT_TEXT_SERIES_CRID:
					crid->type = DxCRID_TYPE_SERIES;
					break;
				case eDxIPEPG_UKDTT_TEXT_RECOMMENDATION_CRID:
					crid->type = DxCRID_TYPE_RECOMMEND;
					break;
				default:
					HxLOG_Error("Unknown type! size=(%d)", size);
					crid->type = 0;
					break;
			}

			if (HLIB_STD_StrNCaseCmp(tempBuf, "crid://", 7) == 0)
			{
				// ignore crid://
				crid->crid = DLIB_StrDup(tempBuf+7);
			}
			else
			{
				crid->crid = DLIB_StrDup(tempBuf);
			}
			tempBuf += usStrSize;

			HLIB_VECTOR_Add(cridList,crid);
		}
	}
	return cridList;
#endif
}

static HUINT32 dlib_epg_humax_Get_GuidanceType(HUINT8 *buf, HUINT32 size)
{
#if !defined(CONFIG_PROD_FVP4000T)
	return 0;
#else
	HUINT32 i = 0;
	HUINT8 *tempBuf = buf;
	HUINT16	usCount = 0;
	HUINT16	usStrSize;
	HUINT32	ulValue = 0;

	tempBuf = dlib_epg_humax_Get_ExtBuf(buf, size);

	usCount = IPEPG_Get16Bit(tempBuf);	// DxIpEvent_t.ext.ukdtt.usCRIDCount
	tempBuf += sizeof(HUINT16);
	if (usCount)
	{
		for (i=0; i<usCount; i++)
		{
			// CRID
			usStrSize = IPEPG_Get16Bit(tempBuf);
			tempBuf += (sizeof(HUINT16)+usStrSize);
			if (usStrSize != 0)	{tempBuf += (sizeof(DxIPEPG_Text_e) +3 ); }
		}
	}

	ulValue = IPEPG_Get32Bit(tempBuf);

	return ulValue;
#endif
}

static HCHAR * dlib_epg_humax_Get_Guidance(HUINT8 *buf, HUINT32 size)
{
#if !defined(CONFIG_PROD_FVP4000T)
	return NULL;
#else
	HUINT32 i = 0;
	HUINT8 *tempBuf = buf;
	HUINT16	usCount = 0;
	HUINT16	usStrSize;
	HCHAR	*pszString = NULL;

	tempBuf = dlib_epg_humax_Get_ExtBuf(buf, size);

	usCount = IPEPG_Get16Bit(tempBuf);	// DxIpEvent_t.ext.ukdtt.usCRIDCount
	tempBuf += sizeof(HUINT16);
	if (usCount)
	{
		for (i=0; i<usCount; i++)
		{
			// CRID
			usStrSize = IPEPG_Get16Bit(tempBuf);
			tempBuf += (sizeof(HUINT16)+usStrSize);
			if (usStrSize != 0)	{tempBuf += (sizeof(DxIPEPG_Text_e) +3 ); }
		}
	}

	tempBuf += sizeof(HUINT32);	// DxIpEvent_t.ext.ukdtt.ulGuidanceType

	usCount = IPEPG_Get16Bit(tempBuf);	// DxIpEvent_t.ext.ukdtt.usGuidanceCount
	tempBuf += sizeof(HUINT16);
	if (usCount)
	{
		usStrSize = IPEPG_Get16Bit(tempBuf);
		tempBuf += sizeof(HUINT16);
		if(usStrSize != 0)
		{
			tempBuf += sizeof(DxIPEPG_Text_e);
			tempBuf += 3;
			pszString = DLIB_StrDup(tempBuf);
		}
	}

	return pszString;
#endif
}

static HUINT32 dlib_epg_humax_Get_Component(HUINT8 *buf, HUINT32 size)
{
#if !defined(CONFIG_PROD_FVP4000T)
	return 0;
#else
	HUINT32 i = 0;
	HUINT8 *tempBuf = buf;
	HUINT16	usCount = 0;
	HUINT16	usStrSize;

	HUINT32	ulValue;


	tempBuf = dlib_epg_humax_Get_ExtBuf(buf, size);

	usCount = IPEPG_Get16Bit(tempBuf);	// DxIpEvent_t.ext.ukdtt.usCRIDCount
	tempBuf += sizeof(HUINT16);
	if (usCount)
	{
		for (i=0; i<usCount; i++)
		{
			// CRID
			usStrSize = IPEPG_Get16Bit(tempBuf);
			tempBuf += (sizeof(HUINT16)+usStrSize);
			if (usStrSize != 0)	{tempBuf += (sizeof(DxIPEPG_Text_e) +3 ); }
		}
	}

	tempBuf += sizeof(HUINT32);	// DxIpEvent_t.ext.ukdtt.ulGuidanceType

	usCount = IPEPG_Get16Bit(tempBuf);	// DxIpEvent_t.ext.ukdtt.usGuidanceCount
	tempBuf += sizeof(HUINT16);
	if (usCount)
	{
		usStrSize = IPEPG_Get16Bit(tempBuf);
		tempBuf += (sizeof(HUINT16) + usStrSize);
		if(usStrSize != 0)	{ tempBuf += (sizeof(DxIPEPG_Text_e) + 3);}
	}

	tempBuf += (sizeof(HUINT16)*4);	// HdAlternative*4

	ulValue = IPEPG_Get32Bit(tempBuf);

	return ulValue;
#endif
}

static DxEventOnDemand_t * dlib_epg_humax_Get_OnDemand(HUINT8 *buf, HUINT32 size)
{
#if !defined(CONFIG_PROD_FVP4000T)
	return NULL;
#else
	HUINT32 i = 0,j=0;
	HUINT8 *tempBuf = buf;
	HUINT16 usCount = 0;
	HUINT16 usStrSize;
	HUINT16 usMandatoryStrCount;
	DxEventOnDemand_t	*pOnDemandInfo =  NULL;
	HxVector_t * mandatoryStringList = NULL;

	tempBuf = dlib_epg_humax_Get_ExtBuf(buf, size);

	usCount = IPEPG_Get16Bit(tempBuf);	// DxIpEvent_t.ext.ukdtt.usCRIDCount
	tempBuf += sizeof(HUINT16);
	if (usCount)
	{
		for (i=0; i<usCount; i++)
		{
			// CRID
			usStrSize = IPEPG_Get16Bit(tempBuf);
			tempBuf += (sizeof(HUINT16)+usStrSize);
			if (usStrSize != 0) {tempBuf += (sizeof(DxIPEPG_Text_e) +3 ); }
		}
	}

	tempBuf += sizeof(HUINT32);	// DxIpEvent_t.ext.ukdtt.ulGuidanceType

	usCount = IPEPG_Get16Bit(tempBuf);	// DxIpEvent_t.ext.ukdtt.usGuidanceCount
	tempBuf += sizeof(HUINT16);
	if (usCount)
	{
		usStrSize = IPEPG_Get16Bit(tempBuf);
		tempBuf += sizeof(HUINT16) + usStrSize;
		if(usStrSize != 0)	{ tempBuf += (sizeof(DxIPEPG_Text_e) + 3);}
	}

	tempBuf += (sizeof(HUINT16)*4);	// HdAlternative*4
	tempBuf += (sizeof(HUINT32)); // DxIpEvent_t.ext.ukdtt.eComponent

	usCount = IPEPG_Get16Bit(tempBuf);	// OnDemand Count
	tempBuf += sizeof(HUINT16);
	if (usCount)
	{
		pOnDemandInfo = DLIB_Calloc(sizeof(DxEventOnDemand_t));

		pOnDemandInfo->ulStartOfAvailability = IPEPG_Get32Bit(tempBuf);
		tempBuf += sizeof(HUINT32);
		pOnDemandInfo->ulEndOfAvailability = IPEPG_Get32Bit(tempBuf);
		tempBuf += sizeof(HUINT32);
		pOnDemandInfo->ulPublishedDuration = IPEPG_Get32Bit(tempBuf);
		tempBuf += sizeof(HUINT32);
		pOnDemandInfo->ucFree = IPEPG_Get8Bit(tempBuf);
		tempBuf += sizeof(HUINT8);
		pOnDemandInfo->ucIsHD = IPEPG_Get8Bit(tempBuf);
		tempBuf += sizeof(HUINT8);
		pOnDemandInfo->ulOnDemandGenre = IPEPG_Get32Bit(tempBuf);
		tempBuf += sizeof(HUINT32);

		// stMandatory
		usMandatoryStrCount = IPEPG_Get16Bit(tempBuf);
		tempBuf += sizeof(HUINT16);
		if (usMandatoryStrCount != 0)
		{
			mandatoryStringList = HLIB_VECTOR_NewEx(NULL, 0, NULL, (void *(*)(void*))dlib_epg_humax_FreeOnDemandString,NULL);
			for (j=0; j<usMandatoryStrCount; j++)
			{
				DxEventOnDemandString_t * pOnDemandString = DLIB_Calloc(sizeof(DxEventOnDemandString_t));

				usStrSize = IPEPG_Get16Bit(tempBuf);
				tempBuf += sizeof(HUINT16);
				if (usStrSize != 0)
				{
					pOnDemandString->ulStringType = IPEPG_Get32Bit(tempBuf);
					tempBuf += sizeof(DxIPEPG_UKDTT_TEXT_e);
					tempBuf += 3;
					pOnDemandString->pucString= DLIB_StrDup(tempBuf);
					tempBuf += usStrSize;
				}

				HLIB_VECTOR_Add(mandatoryStringList,pOnDemandString);
			}

			pOnDemandInfo->onDemandStringList = mandatoryStringList;
		}
	}
	return pOnDemandInfo;
#endif
}

static HUINT8 * dlib_epg_humax_Get_data(HUINT8 *buf, DxIPEPG_Text_e eIndex, HUINT32 size, HxLANG_Id_e * pLangId)
{
	HUINT32 i = 0;
	HUINT8 *tempBuf = buf;
	HUINT16	usSize = 0;
	HUINT16	usStrCount = 0;

	tempBuf += sizeof(DxIpEventBase_t);

	usStrCount = IPEPG_Get16Bit(tempBuf);
#if defined(DLIB_PORT_HUMAX_EPG_SEE_PARAMETERS)
	HxLOG_Debug("STR COUNT %d, %x eIndex:%d size:%d\n", usStrCount, usStrCount, eIndex, size);
#endif
	tempBuf += sizeof(HUINT16);

	for(i = 0; i< usStrCount; i++)
	{
		DxIPEPG_Text_e eType = 0;
		HUINT8	Lang[4] = "";
		usSize = IPEPG_Get16Bit(tempBuf);
		tempBuf += sizeof(HUINT16);
		if(usSize > size)	HxLOG_Error("dlib_epg_humax_Get_data FAIL[%d] [%x]\n", size, usSize);


		if(usSize != 0)
		{
			eType = IPEPG_Get32Bit(tempBuf);
			tempBuf += sizeof(DxIPEPG_Text_e);

#if defined(DLIB_PORT_HUMAX_EPG_SEE_PARAMETERS)
			HxLOG_Debug("STR size [%d, %x] type [%d, %x]\n", usSize,  usSize,eType, eType);
#endif

			Lang[0] = IPEPG_Get8Bit(tempBuf);
			Lang[1] = IPEPG_Get8Bit(tempBuf+1);
			Lang[2] = IPEPG_Get8Bit(tempBuf+2);
			if(pLangId) *pLangId = HLIB_LANG_639ToId(Lang);
			tempBuf += 3;
			if(eIndex == eType)
			{
#if defined(DLIB_PORT_HUMAX_EPG_SEE_PARAMETERS)
				HxLOG_Debug("tempBuf[%s]\n", tempBuf);
#endif
				return tempBuf;
			}
		}
		tempBuf += usSize;
	}

	return NULL;
}
static HBOOL	dlib_epg_humax_GetName (DxField_t *field, const void *src, HUINT32 size)
{
	HUINT8 *pEvent = (HUINT8 *)src;
	RETURN_IF(field == NULL || src == NULL || size == 0, FALSE);
#if defined(DLIB_PORT_HUMAX_EPG_SEE_DUMPDATA)
	dlib_epg_humax_dumpdata((HUINT8 *)src, size);
#endif
	field->u.string = DLIB_StrDup(dlib_epg_humax_Get_data(pEvent, eDxIPEPG_TEXT_NAME, size, NULL));
	return TRUE;
}

static HBOOL	dlib_epg_humax_GetStartTime (DxField_t *field, const void *src, HUINT32 size)
{
	DxIpEvent_t * pEvent = (DxIpEvent_t *)src;
	RETURN_IF(field == NULL || src == NULL || size == 0, FALSE);
	field->u.integer = pEvent->stBase.ulStartTime;
	return TRUE;
}

static HBOOL	dlib_epg_humax_GetEndTime (DxField_t *field, const void *src, HUINT32 size)
{
	DxIpEvent_t * pEvent = (DxIpEvent_t *)src;
	RETURN_IF(field == NULL || src == NULL || size == 0, FALSE);
	field->u.integer = pEvent->stBase.ulEndTime;
	return TRUE;
}

static HBOOL	dlib_epg_humax_GetSvcUID(DxField_t *field, const void *src, HUINT32 size)
{
	DxIpEvent_t * pEvent = (DxIpEvent_t *)src;
	RETURN_IF(field == NULL || src == NULL || size == 0, FALSE);
	field->u.integer = (HINT32)pEvent->stBase.ulSvcUid;
	return TRUE;
}

static HBOOL	dlib_epg_humax_GetEventID (DxField_t *field, const void *src, HUINT32 size)
{
	DxIpEvent_t * pEvent = (DxIpEvent_t *)src;
	RETURN_IF(field == NULL || src == NULL || size == 0, FALSE);
	field->u.integer = pEvent->stBase.ulUniqueId;
	return TRUE;
}


static HBOOL	dlib_epg_humax_GetUniqueID (DxField_t *field, const void *src, HUINT32 size)
{
	DxIpEvent_t * pEvent = (DxIpEvent_t *)src;
	RETURN_IF(field == NULL || src == NULL || size == 0, FALSE);
	field->u.integer = pEvent->stBase.ulUniqueId;
	return TRUE;
}


static HBOOL	dlib_epg_humax_GetSeriesID (DxField_t *field, const void *src, HUINT32 size)
{
	DxIpEvent_t * pEvent = (DxIpEvent_t *)src;
	RETURN_IF(field == NULL || src == NULL || size == 0, FALSE);
	field->u.integer = pEvent->stBase.ulSeriesId;
	return TRUE;
}

static HBOOL	dlib_epg_humax_GetTriplet (DxField_t *field, const void *src, HUINT32 size)
{
	DxIpEvent_t * pEvent = (DxIpEvent_t *)src;
	HCHAR	buf[32];

	RETURN_IF(field == NULL || src == NULL || size == 0, FALSE);

	HxSTD_PrintToStrN(buf, 32, "%x.%x.%x"
					, pEvent->stBase.usOnId
					, pEvent->stBase.usTsId
					, pEvent->stBase.usSvcId
				);
	field->u.string = DLIB_StrDup(buf);
	if (field->u.string == NULL)
		return FALSE;
	return TRUE;
}

static HxVector_t * dlib_epg_humax_GetNameVector (const void *src, HUINT32 size)
{
	HUINT8 *pEvent = (HUINT8 *)src;
	HCHAR * pString = NULL;
	HxLANG_Id_e eLangID = eLangID_NONE;
	HxVector_t * nameText	 = NULL;
	RETURN_IF(src == NULL || size == 0, FALSE);
	pString = dlib_epg_humax_Get_data(pEvent, eDxIPEPG_TEXT_NAME, size, &eLangID);

	if(pString)
	{
		nameText = HLIB_VECTOR_NewEx(NULL, 0, NULL, (void *(*)(void *))dlib_epg_humaxrf_FreeEventString, NULL);

		if(nameText != NULL)
		{
			DxEventString_t	* eventShortString = DLIB_Calloc(sizeof(DxEventString_t));
			eventShortString->string = DLIB_StrDup(pString);
			eventShortString->language = eLangID;
			HLIB_VECTOR_Add(nameText, eventShortString);
		}
	}
	return nameText;
}

static HxVector_t * dlib_epg_humax_GetShortVector (const void *src, HUINT32 size)
{
	HUINT8 *pEvent = (HUINT8 *)src;
	HCHAR * pString = NULL;
	HxLANG_Id_e eLangID = eLangID_NONE;
	HxVector_t * shortText	 = NULL;
	RETURN_IF(src == NULL || size == 0, FALSE);
	pString = dlib_epg_humax_Get_data(pEvent, eDxIPEPG_TEXT_SHORT, size, &eLangID);

	if(pString)
	{
		shortText = HLIB_VECTOR_NewEx(NULL, 0, NULL, (void *(*)(void *))dlib_epg_humaxrf_FreeEventString, NULL);

		if(shortText != NULL)
		{
			DxEventString_t	* eventShortString = DLIB_Calloc(sizeof(DxEventString_t));
			eventShortString->string = DLIB_StrDup(pString);
			eventShortString->language = eLangID;
			HLIB_VECTOR_Add(shortText, eventShortString);
		}
	}
	return shortText;
}

static HxVector_t * dlib_epg_humax_GetDetailVector (const void *src, HUINT32 size)
{
	HUINT8 *pEvent = (HUINT8 *)src;
	HCHAR * pString = NULL;
	HxLANG_Id_e eLangID = eLangID_NONE;
	HxVector_t * detailText = NULL;
	RETURN_IF(src == NULL || size == 0, FALSE);
	pString = dlib_epg_humax_Get_data(pEvent, eDxIPEPG_TEXT_DETAIL, size, &eLangID);

	if(pString)
	{
		detailText = HLIB_VECTOR_NewEx(NULL, 0, NULL, (void *(*)(void *))dlib_epg_humaxrf_FreeEventString, NULL);

		if(detailText != NULL)
		{
			DxEventString_t	* eventDetailString = DLIB_Calloc(sizeof(DxEventString_t));
			eventDetailString->string = DLIB_StrDup(pString);
			eventDetailString->language = eLangID;
			HLIB_VECTOR_Add(detailText, eventDetailString);
		}
	}
	return detailText;
}

#if defined(CONFIG_PROD_HMS1000S) || defined(CONFIG_PROD_HMSLITE)
static HBOOL	dlib_epg_humax_GetActor (DxField_t *field, const void *src, HUINT32 size)
{
	HUINT8 *pEvent = (HUINT8 *)src;
	RETURN_IF(field == NULL || src == NULL || size == 0, FALSE);
	field->u.string = DLIB_StrDup(dlib_epg_humax_Get_data(pEvent, eDxIPEPG_TEXT_ACTOR, size, NULL));
	return TRUE;
}

static HBOOL	dlib_epg_humax_GetActress (DxField_t *field, const void *src, HUINT32 size)
{
	HUINT8 *pEvent = (HUINT8 *)src;
	RETURN_IF(field == NULL || src == NULL || size == 0, FALSE);
	field->u.string = DLIB_StrDup(dlib_epg_humax_Get_data(pEvent, eDxIPEPG_TEXT_ACTRESS, size, NULL));
	return TRUE;
}

static HBOOL	dlib_epg_humax_GetDirector (DxField_t *field, const void *src, HUINT32 size)
{
	HUINT8 *pEvent = (HUINT8 *)src;
	RETURN_IF(field == NULL || src == NULL || size == 0, FALSE);
	field->u.string = DLIB_StrDup(dlib_epg_humax_Get_data(pEvent, eDxIPEPG_TEXT_DIRECTOR, size, NULL));
	return TRUE;
}
#endif
static HBOOL	dlib_epg_humax_GetThumbnail (DxField_t *field, const void *src, HUINT32 size)
{
	HUINT8 *pEvent = (HUINT8 *)src;
	RETURN_IF(field == NULL || src == NULL || size == 0, FALSE);
	field->u.string = DLIB_StrDup(dlib_epg_humax_Get_data(pEvent, eDxIPEPG_TEXT_THUMBNAIL, size, NULL));
	return TRUE;
}

static HBOOL	dlib_epg_humax_GetPerson (DxField_t *field, const void *src, HUINT32 size)
{
	HUINT8 *pEvent = (HUINT8 *)src;
	RETURN_IF(field == NULL || src == NULL || size == 0, FALSE);
	field->u.string = DLIB_StrDup(dlib_epg_humax_Get_data(pEvent, eDxIPEPG_TEXT_PERSON, size, NULL));
	return TRUE;
}

static HBOOL	dlib_epg_humax_GetEpisode (DxField_t *field, const void *src, HUINT32 size)
{
	DxIpEvent_t * pEvent = (DxIpEvent_t *)src;
	RETURN_IF(field == NULL || src == NULL || size == 0, FALSE);
	field->u.integer = pEvent->stBase.ulEpisodeNum;
	return TRUE;
}

static HxVector_t * dlib_epg_humax_GetLinkList (const void *src, HUINT32 size)
{
	HUINT8 *pEvent = (HUINT8 *)src;
	RETURN_IF(src == NULL || size == 0, NULL);
	return dlib_epg_humax_Get_LinkData(pEvent, size);
}

static HxVector_t * dlib_epg_humax_GetMediaList (const void *src, HUINT32 size)
{
	HUINT8 *pEvent = (HUINT8 *)src;
	RETURN_IF(src == NULL || size == 0, NULL);
	return dlib_epg_humax_Get_MediaData(pEvent, size);
}

static HxVector_t * dlib_epg_humax_GetCRID(const void *src, HUINT32 size)
{
	HUINT8 *pEvent = (HUINT8 *)src;
	RETURN_IF(src == NULL || size == 0, NULL);
	return dlib_epg_humax_Get_CRID(pEvent, size);
}

static HUINT32 dlib_epg_humax_GetGuidanceType(const void *src, HUINT32 size)
{
	HUINT8 *pEvent = (HUINT8 *)src;
	RETURN_IF(src == NULL || size == 0, 0);
	return dlib_epg_humax_Get_GuidanceType(pEvent, size);
}

static HCHAR	* dlib_epg_humax_GetGuidance(const void *src, HUINT32 size)
{
	HUINT8 *pEvent = (HUINT8 *)src;
	RETURN_IF(src == NULL || size == 0, NULL);
	return dlib_epg_humax_Get_Guidance(pEvent, size);
}

static HUINT32		dlib_epg_humax_GetComponent(const void *src, HUINT32 size)
{
	HUINT8 *pEvent = (HUINT8 *)src;
	RETURN_IF(src == NULL || size == 0, 0);
	return dlib_epg_humax_Get_Component(pEvent, size);
}

static DxEventOnDemand_t * dlib_epg_humax_GetOnDemand (const void *src, HUINT32 size)
{
	HUINT8 *pEvent = (HUINT8 *)src;
	RETURN_IF(src == NULL || size == 0, NULL);
	return dlib_epg_humax_Get_OnDemand(pEvent, size);
}

static HUINT32 _dlib_epg_humax_Get_ulMediaCount(HUINT8 *buf, HUINT32 size)
{
	HUINT32 i = 0;
	HUINT8 *tempBuf = buf;
	HUINT16	usSize = 0;
	HUINT16	usCount = 0;
	HUINT32 ulMediaCount = 0;

	tempBuf += sizeof(DxIpEventBase_t);
	usCount = IPEPG_Get16Bit(tempBuf);
	tempBuf += sizeof(HUINT16);

	for(i = 0; i< usCount; i++)
	{
		usSize = IPEPG_Get16Bit(tempBuf);
		tempBuf += (sizeof(HUINT16) + usSize);
		if(usSize != 0) tempBuf += (sizeof(DxIPEPG_Text_e) + 3);
	}

/////////////////////////////////////////// End of STRING
	usCount = IPEPG_Get16Bit(tempBuf);	// Link Count
	tempBuf += sizeof(HUINT16);
	if(usCount)
	{
		for(i = 0; i< usCount; i++)
		{
			tempBuf += (sizeof(HUINT32)*5); 				// ulProgId,ulStartTime,ulEndTime,ulEpisodeNum,ulSeason
			usSize = IPEPG_Get16Bit(tempBuf);
			tempBuf += (sizeof(HUINT16) + usSize);			// subName size + string size

			if(usSize != 0) tempBuf += (sizeof(DxIPEPG_Text_e) + 3);
		}
	}
/////////////////////////////////////////// End of Link
#if defined(DLIB_PORT_HUMAX_EPG_SEE_PARAMETERS)
	HxLOG_Debug("link pointer for media[%x]\n", tempBuf);
#endif
	usCount = IPEPG_Get16Bit(tempBuf);	// Media Count
	tempBuf += sizeof(HUINT16);

	ulMediaCount = usCount;

	return ulMediaCount;
}

static HBOOL	dlib_epg_humax_GetMediaLinkType (DxField_t *field, const void *src, HUINT32 size)
{
	HUINT8 * pEvent = (HUINT8 *)src;
	HUINT32 ulMediaCount = 0;
	RETURN_IF(field == NULL || src == NULL || size == 0, FALSE);

	field->u.integer = 0;

	ulMediaCount = _dlib_epg_humax_Get_ulMediaCount(pEvent, size);
	if(ulMediaCount > 0)
		field->u.integer = 1;

	return TRUE;
}

#ifdef CONFIG_FUNC_IPEPG_USE_KEYWORDSEARCH
static HCHAR * dlib_epg_humax_GetCRIDKeyword( HxVector_t * pCRIDList)
{
	HUINT32 		uCRIDLen,i;
	HCHAR			szKeyword[1024] = {0,};
	HCHAR			*pRet;
	DxEventCRID_t	*pCRID 			= NULL;

	if(pCRIDList == NULL)
		return NULL;

	uCRIDLen = HLIB_VECTOR_Length(pCRIDList);
	if(uCRIDLen <= 0)
		return NULL;

	for(i=0; i< uCRIDLen; i++)
	{
		pCRID = (DxEventCRID_t *)HLIB_VECTOR_ItemAt(pCRIDList, i);

		if(i>0)
			HxSTD_StrNCat(szKeyword ,KEYWORD_CRID_DLIMITER,HxSTD_StrLen(KEYWORD_CRID_DLIMITER));

		if(pCRID->type == DxCRID_TYPE_SERIES)
			HxSTD_StrNCat(szKeyword ,KEYWORD_CRID_SERIES,HxSTD_StrLen(KEYWORD_CRID_SERIES));
		else if(pCRID->type == DxCRID_TYPE_PROG)
			HxSTD_StrNCat(szKeyword ,KEYWORD_CRID_PROGRAM,HxSTD_StrLen(KEYWORD_CRID_PROGRAM));
		else
			continue;

		HxSTD_StrNCat(szKeyword ,pCRID->crid,HLIB_STD_StrNLen(pCRID->crid,DxCRID_LEN));
		HxSTD_StrNCat(szKeyword ,KEYWORD_CRID_DLIMITER,HxSTD_StrLen(KEYWORD_CRID_DLIMITER));
	}

	HxLOG_Debug("[%s][%d] CRID Keyword: %s\n", __FUNCTION__,__LINE__,szKeyword);

	pRet = DLIB_StrDup(szKeyword);
	return pRet;
}

static HBOOL	dlib_epg_humax_GetKeyword (DxField_t *field, const void *src, HUINT32 size)
{
	HCHAR *pKeyword = NULL;

	RETURN_IF(field == NULL || src == NULL || size == 0, FALSE);

#if defined(CONFIG_FUNC_IPEPG_USE_KEYWORDSEARCH)
	{
		HxVector_t * pCRIDs = dlib_epg_humax_GetCRID(src,size);
		if(pCRIDs)
		{
			pKeyword = dlib_epg_humax_GetCRIDKeyword(pCRIDs);
			HLIB_VECTOR_Delete(pCRIDs);
		}
	}
#endif

	if(pKeyword)
		field->u.string = pKeyword;
	else
		field->u.string = DLIB_StrDup("NULL");

	return TRUE;
}
#endif

static const DxEPG_Field_t	s_EPG_HUMAX_FIELDS[] =
{
	  {{eDxTYPE_STRING	, "name"			, TRUE , {0}	}, dlib_epg_humax_GetName}
	, {{eDxTYPE_INTEGER	, "start_time"		, FALSE, {0}	}, dlib_epg_humax_GetStartTime}
	, {{eDxTYPE_INTEGER	, "end_time"		, FALSE, {0}	}, dlib_epg_humax_GetEndTime}
	, {{eDxTYPE_INTEGER	, "svcuid"			, TRUE , {0}	}, dlib_epg_humax_GetSvcUID}
	, {{eDxTYPE_INTEGER	, "eventid"			, TRUE,  {0}	}, dlib_epg_humax_GetEventID}
	, {{eDxTYPE_STRING  , "tripleid"  		, TRUE,  {0}	}, dlib_epg_humax_GetTriplet}
	, {{eDxTYPE_INTEGER	, "seriesid"		, TRUE,  {0}	}, dlib_epg_humax_GetSeriesID}
	, {{eDxTYPE_STRING	, "person"			, TRUE,  {0}	}, dlib_epg_humax_GetPerson}
	, {{eDxTYPE_INTEGER	, "uniqueid"		, FALSE, {0}	}, dlib_epg_humax_GetUniqueID}		// IpEpg UID from SERVER
	, {{eDxTYPE_INTEGER	, "episode"			, FALSE, {0}	}, dlib_epg_humax_GetEpisode}
	, {{eDxTYPE_INTEGER	, "mediaType"		, FALSE, {0}	}, dlib_epg_humax_GetMediaLinkType}
#ifdef CONFIG_FUNC_IPEPG_USE_KEYWORDSEARCH
	, {{eDxTYPE_STRING,  "keyword"	 		, TRUE , {0}	}, dlib_epg_humax_GetKeyword}
#endif
};

HBOOL dlib_epg_linkPrint(DxEventLink_t	* linkData, void * udata)
{
#if defined(DLIB_PORT_HUMAX_EPG_SEE_PARAMETERS)
	if(linkData)
	{
		HxLOG_Debug("LINK prog[%x] stime[%x] etime[%x] episode[%x] season[%x]\n", linkData->ulProgId, linkData->ulStartTime, linkData->ulEndTime, linkData->ulEpisodeNum, linkData->ulSeason);
		if(linkData->pcSubName) HxLOG_Debug("  name [%s]\n",linkData->pcSubName);
	}
#endif
	return FALSE;
}

#define DBG_PRINT_DLIB_EPG_MEDIA
HBOOL dlib_epg_mediaImagePrint(void * pData, void *udata)
{
	DxEventMediaImage_t * imageData = (DxEventMediaImage_t *)pData;
#if defined (DBG_PRINT_DLIB_EPG_MEDIA)
	if(imageData)
	{
		HxLOG_Debug("Image size[%d] type[%s] url[%s] \n",
				imageData->ulSize,
				(NULL!=imageData->pucUrlType)	?	imageData->pucUrlType 	: (HUINT8*)"empty",
				(NULL!=imageData->pucImageUrl)	?	imageData->pucImageUrl	: (HUINT8*)"empty"
			);
	}
#endif
	return FALSE;
}

HBOOL dlib_epg_mediaUrlPrint(void * pData, void *udata)
{
	DxEventMediaUrl_t * urlData = (DxEventMediaUrl_t *)pData;

#if defined (DBG_PRINT_DLIB_EPG_MEDIA)
	if(urlData)
	{
		HxLOG_Debug("URL target[%s] type[%s] url[%s] \n",
			(NULL!=urlData->pucTarget)	? urlData->pucTarget 	: (HUINT8*)"empty",
			(NULL!=urlData->pucType) 	? urlData->pucType 		: (HUINT8*)"empty",
			(NULL!=urlData->pucUrl) 	? urlData->pucUrl 		: (HUINT8*)"empty"
			);
	}
#endif
	return FALSE;
}

HBOOL dlib_epg_mediaPrint(void * pData, void * udata)
{
	DxEventMedia_t	* mediaData = (DxEventMedia_t *)pData;

#if defined (DBG_PRINT_DLIB_EPG_MEDIA)
	if(mediaData)
	{
		HxLOG_Debug("MEDIA id[%s] pdate[%x] edate[%x] tdate[%x] price[%x]\n",mediaData->pcMediaId, mediaData->ulPubDate, mediaData->ulExpireDate, mediaData->ulTransmitDate, mediaData->ulPrice);
		if(mediaData->mediaUrlList) HLIB_VECTOR_ForEach	(mediaData->mediaUrlList, dlib_epg_mediaUrlPrint, NULL);
		if(mediaData->mediaImageList) HLIB_VECTOR_ForEach (mediaData->mediaImageList, dlib_epg_mediaImagePrint, NULL);
	}
#endif
	return FALSE;
}

DxEvent_t *	dlib_epg_humax_NewEvent (HUID uid, const HUINT8 *src, HUINT32 size)
{
	DxIpEvent_t		*ip		= NULL;
	DxEvent_t		*event 	= NULL;
	DxField_t field;

	HxLOG_Trace();
	ip = (DxIpEvent_t *)src;
#if defined(DLIB_PORT_HUMAX_EPG_SEE_PARAMETERS)
	HxLOG_Debug("dlib_port_humax_epg[%d] size[%d]\n", __LINE__, size);
#endif

	RETURN_IF(ip == NULL || size == 0 , NULL);
	RETURN_IF(ip->stBase.type != eDxEPG_TYPE_IP, NULL);
	event = (DxEvent_t *)DLIB_Malloc(sizeof(DxEvent_t));
	RETURN_IF(event == NULL, NULL);
	HxSTD_MemSet(event,0,sizeof(DxEvent_t));

	event->type 		= ip->stBase.type;
	event->refCount 	= 1;
	event->uid          = uid;
	event->svcuid 		= ip->stBase.ulSvcUid;
	event->tsid 		= ip->stBase.usTsId;
	event->onid 		= ip->stBase.usOnId;
	event->svcid 		= ip->stBase.usSvcId;

	event->eventId		= ip->stBase.ulUniqueId;
	event->startTime 	= ip->stBase.ulStartTime;
	event->duration 	= ip->stBase.ulEndTime - ip->stBase.ulStartTime;
	event->runStatus 	= 0;
	event->freeCaMode 	= TRUE;
	event->parentalRating = (HUINT32)ip->stBase.ucRating;

	HxSTD_MemSet(event->content,0,sizeof(HUINT16)*DxEVENT_CONTENT); //temp
	event->content[0] = (ip->stBase.ucGenre<<8) & 0xFF00;

	event->crid = dlib_epg_humax_GetCRID(src,size);

	event->extension.ipEpg.drm 			= ip->stBase.ucDrm;
	event->extension.ipEpg.seriesId 	= ip->stBase.ulSeriesId;
	event->extension.ipEpg.episodeNum 	= ip->stBase.ulEpisodeNum;
	event->extension.ipEpg.totalNum 	= ip->stBase.ulTotalNum;
	event->extension.ipEpg.season 		= ip->stBase.ulSeason;
	event->extension.ipEpg.seriesType 	= ip->stBase.ucSeriesType;
	HxSTD_MemCopy(event->extension.ipEpg.eventLink, ip->stBase.astEventLink,sizeof(EVENT_LINK_HUMAX_t)*EPG_HUMAX_LINK_MAX);

	event->extension.ipEpg.linkList = dlib_epg_humax_GetLinkList(src,size);
	event->extension.ipEpg.mediaList = dlib_epg_humax_GetMediaList(src,size);
	//if(event->extension.ipEpg.linkList) HLIB_VECTOR_ForEach	(event->extension.ipEpg.linkList, dlib_epg_linkPrint, NULL);
	//if(event->extension.ipEpg.mediaList) HLIB_VECTOR_ForEach	(event->extension.ipEpg.mediaList, dlib_epg_mediaPrint, NULL);

	dlib_epg_humax_GetPerson (&field, src, size);
	event->extension.ipEpg.person = field.u.string;
	dlib_epg_humax_GetThumbnail (&field, src, size);
	event->extension.ipEpg.thumbnailURL = field.u.string;

	event->name = dlib_epg_humax_GetNameVector (src, size);
	event->text = dlib_epg_humax_GetShortVector(src, size);
	event->longText = dlib_epg_humax_GetDetailVector(src, size);

#if defined(CONFIG_OP_UK_DTT)
	event->extension.ipEpg.guidanceType = dlib_epg_humax_GetGuidanceType(src, size);
	event->extension.ipEpg.guidance = dlib_epg_humax_GetGuidance(src, size);
	event->extension.ipEpg.component = dlib_epg_humax_GetComponent(src,size);
	event->extension.ipEpg.onDemand = dlib_epg_humax_GetOnDemand(src, size);
#else
	event->extension.ipEpg.guidanceType = 0;
	event->extension.ipEpg.guidance = NULL;
	event->extension.ipEpg.component = 0;
	event->extension.ipEpg.onDemand = NULL;
#endif

	dlib_epg_humax_Print_Event(event);
	return event;
}

static DxEventString_t *dlib_epg_humax_CloneString (DxEventString_t *string)
{
	DxEventString_t	*clone;
	if (string == NULL)
		return NULL;

	clone = (DxEventString_t *)DLIB_MemDup(string, sizeof(DxEventString_t));
	if (clone)
	{
		if(string->string) clone->string = DLIB_StrDup(string->string);
	}
	return clone;
}

static DxEventLink_t *dlib_epg_humax_CloneLinkItem (DxEventLink_t *item)
{
	DxEventLink_t	*clone;
	if (item == NULL)
		return NULL;

	clone = (DxEventLink_t *)DLIB_MemDup(item, sizeof(DxEventLink_t));
	if (clone)
	{
		if(item->pcSubName) clone->pcSubName = DLIB_StrDup(item->pcSubName);
	}
	return clone;
}

static DxEventMediaImage_t *dlib_epg_humax_CloneMediaImageList (DxEventMediaImage_t *item)
{
	DxEventMediaImage_t *clone;
	if (NULL == item)
		return NULL;
	clone = (DxEventMediaImage_t *)DLIB_MemDup(item, sizeof(DxEventMediaImage_t));
	if(clone)
	{
		if(item->pucImageUrl)
			clone->pucImageUrl	= DLIB_StrDup(item->pucImageUrl);
		if(item->pucUrlType)
			clone->pucUrlType	= DLIB_StrDup(item->pucUrlType);
	}
	return clone;
}

static DxEventMediaUrl_t *dlib_epg_humax_CloneMediaUrlList (DxEventMediaUrl_t *item)
{
	DxEventMediaUrl_t *clone;
	if (NULL == item)
		return NULL;

	clone = (DxEventMediaUrl_t *)DLIB_MemDup(item, sizeof(DxEventMediaUrl_t));
	if(clone)
	{
		if(item->pucTarget)
			clone->pucTarget	= DLIB_StrDup(item->pucTarget);
		if(item->pucType)
			clone->pucType		= DLIB_StrDup(item->pucType);
		if(item->pucUrl)
			clone->pucUrl		= DLIB_StrDup(item->pucUrl);
	}
	return clone;
}

static DxEventMedia_t *dlib_epg_humax_CloneMediaItem (DxEventMedia_t *item)
{
	DxEventMedia_t	*clone;
	if (NULL == item)
		return NULL;

	clone = (DxEventMedia_t *)DLIB_MemDup(item, sizeof(DxEventMedia_t));
	if (clone)
	{
		if(item->pcMediaId)
			clone->pcMediaId 		= DLIB_StrDup(item->pcMediaId);
		if(item->mediaUrlList)
			clone->mediaUrlList 	= HLIB_VECTOR_CloneEx((HxVector_t *)item->mediaUrlList, (void *(*)(void *))dlib_epg_humax_CloneMediaUrlList );
		if(item->mediaImageList)
			clone->mediaImageList 	= HLIB_VECTOR_CloneEx((HxVector_t *)item->mediaImageList, (void *(*)(void *))dlib_epg_humax_CloneMediaImageList );
	}
	return clone;
}

static DxEventCRID_t *dlib_epg_humax_CloneCRID (DxEventCRID_t *item)
{
	DxEventCRID_t *clone;

	if (item == NULL)
		return NULL;

	clone =  (DxEventCRID_t *)DLIB_MemDup(item, sizeof(DxEventCRID_t));
	if (clone)
	{
		clone->type = item->type;
		clone->crid = DLIB_StrDup(item->crid);
	}
	return clone;
}

static DxEventOnDemandString_t *dlib_epg_humax_CloneOnDemandString (DxEventOnDemandString_t *string)
{
	DxEventOnDemandString_t	*clone;
	if (string == NULL)
		return NULL;

	clone = (DxEventOnDemandString_t *)DLIB_MemDup(string, sizeof(DxEventOnDemandString_t));
	if (clone)
	{
		if(string->pucString) clone->pucString = DLIB_StrDup(string->pucString);
	}
	return clone;
}

static DxEventOnDemand_t *dlib_epg_humax_CloneOnDemand (DxEventOnDemand_t *item)
{
	DxEventOnDemand_t *clone;

	if (item == NULL)
		return NULL;

	clone =  (DxEventOnDemand_t *)DLIB_MemDup(item, sizeof(DxEventOnDemand_t));
	if (clone)
	{
		clone->onDemandStringList = HLIB_VECTOR_CloneEx((HxVector_t *)item->onDemandStringList, (void *(*)(void *))dlib_epg_humax_CloneOnDemandString);
	}
	return clone;
}

DxEvent_t *	dlib_epg_humax_CloneEvent (const DxEvent_t *event)
{
	DxEvent_t *clone = NULL;
	HxLOG_Trace();
	clone = (DxEvent_t *)DLIB_MemDup(event, sizeof(DxEvent_t));
	if (clone)
	{
		clone->refCount = 1;
		if (event->name)
			clone->name = HLIB_VECTOR_CloneEx((HxVector_t *)clone->name, (void *(*)(void *))dlib_epg_humax_CloneString);
		if (event->text)
			clone->text = HLIB_VECTOR_CloneEx((HxVector_t *)clone->text, (void *(*)(void *))dlib_epg_humax_CloneString);
		if (event->longText)
			clone->longText = HLIB_VECTOR_CloneEx((HxVector_t *)clone->longText, (void *(*)(void *))dlib_epg_humax_CloneString);
		if (event->crid)
			clone->crid = HLIB_VECTOR_CloneEx((HxVector_t *)event->crid, (void *(*)(void *))dlib_epg_humax_CloneCRID);
		if(event->extension.ipEpg.actor)
			clone->extension.ipEpg.actor = DLIB_StrDup(event->extension.ipEpg.actor);
		if(event->extension.ipEpg.actress)
			clone->extension.ipEpg.actress = DLIB_StrDup(event->extension.ipEpg.actress);
		if(event->extension.ipEpg.director)
			clone->extension.ipEpg.director = DLIB_StrDup(event->extension.ipEpg.director);
		if(event->extension.ipEpg.person)
			clone->extension.ipEpg.person = DLIB_StrDup(event->extension.ipEpg.person);
		if(event->extension.ipEpg.thumbnailURL)
			clone->extension.ipEpg.thumbnailURL = DLIB_StrDup(event->extension.ipEpg.thumbnailURL);
		if(event->extension.ipEpg.linkList)
			clone->extension.ipEpg.linkList = HLIB_VECTOR_CloneEx((HxVector_t *)event->extension.ipEpg.linkList, (void *(*)(void *))dlib_epg_humax_CloneLinkItem);
		if(event->extension.ipEpg.mediaList)
			clone->extension.ipEpg.mediaList = HLIB_VECTOR_CloneEx((HxVector_t *)event->extension.ipEpg.mediaList, (void *(*)(void *))dlib_epg_humax_CloneMediaItem);
#if defined(CONFIG_OP_UK_DTT)
		if (event->extension.ipEpg.guidance)
			clone->extension.ipEpg.guidance = DLIB_StrDup(event->extension.ipEpg.guidance);
		if (event->extension.ipEpg.onDemand)
			clone->extension.ipEpg.onDemand = dlib_epg_humax_CloneOnDemand(event->extension.ipEpg.onDemand);
#endif
	}
	return clone;
}

void dlib_epg_humax_FreeEvent (DxEvent_t *event)
{
	HxLOG_Trace();

	RETURN_IF_VOID(event == NULL);
	RETURN_IF_VOID(event->type != eDxEPG_TYPE_IP);

	if (event->name)			HLIB_VECTOR_Delete(event->name);
	if (event->text)			HLIB_VECTOR_Delete(event->text);
	if (event->longText)		HLIB_VECTOR_Delete(event->longText);
	if (event->item)			HLIB_VECTOR_Delete(event->item);
	if (event->component)		HLIB_VECTOR_Delete(event->component);
	if (event->descriptor)		HLIB_VECTOR_Delete(event->descriptor);
	if (event->linkage)			HLIB_VECTOR_Delete(event->linkage);
	if (event->crid)			HLIB_VECTOR_Delete(event->crid);

	if (event->extension.ipEpg.actor)		 DLIB_Free(event->extension.ipEpg.actor);
	if (event->extension.ipEpg.actress)		 DLIB_Free(event->extension.ipEpg.actress);
	if (event->extension.ipEpg.director)	 DLIB_Free(event->extension.ipEpg.director);
	if (event->extension.ipEpg.person)		 DLIB_Free(event->extension.ipEpg.person);
	if (event->extension.ipEpg.thumbnailURL) DLIB_Free(event->extension.ipEpg.thumbnailURL);
	if (event->extension.ipEpg.linkList)	 HLIB_VECTOR_Delete(event->extension.ipEpg.linkList);
	if (event->extension.ipEpg.mediaList)	 HLIB_VECTOR_Delete(event->extension.ipEpg.mediaList);

#if defined(CONFIG_OP_UK_DTT)
	if (event->extension.ipEpg.guidance) DLIB_Free(event->extension.ipEpg.guidance);
	if (event->extension.ipEpg.onDemand) dlib_epg_humax_FreeOnDemand(event->extension.ipEpg.onDemand);
#endif
	DLIB_Free(event);
}

static HUINT32	dlib_epg_humax_hash(const void *key)
{
	return (HUINT32)key;
}

static HBOOL	dlib_epg_humax_hashvalue_equal(const void *a, const void *b)
{
	if (a == b)
		return TRUE;
	return FALSE;
}

void			DLIB_EPG_HUMAX_Init(void)
{
	HxLOG_Debug("HUMAX EPG Init..\n");

	dlib_epg_humax_MakeSvcHashTable();
}

#define DLIB_IPEPG_DBFILE CONFIG_PRODUCT_USERDATA_DIR "/ipepg/eDxEPG_TYPE_IP.db"
const HCHAR *	DLIB_EPG_HUMAX_GetDBFullName (HINT32 dupIndex)
{
	if(dupIndex == 0)
		return DLIB_IPEPG_DBFILE;
	else
		return NULL;
}

HUINT32			DLIB_EPG_HUMAX_GetSyncTime (void)
{
	return 10;
}

HINT32			DLIB_EPG_HUMAX_GetFieldCount (void)
{
	return sizeof(s_EPG_HUMAX_FIELDS) / sizeof(s_EPG_HUMAX_FIELDS[0]);
}

DxField_t *		DLIB_EPG_HUMAX_GetField (HINT32 index, const void *src, HUINT32 srcSize)
{
	DxField_t field;

	if (DLIB_EPG_HUMAX_GetFieldCount() <= index)
	{
		HxLOG_Warning("%s(index) index(index) out of range~!\n", __FUNCTION__, index);
		return NULL;
	}
	memcpy(&field, &(s_EPG_HUMAX_FIELDS[index].field), sizeof(DxField_t));

	if (src && srcSize && s_EPG_HUMAX_FIELDS[index].getData)
	{
		if (!s_EPG_HUMAX_FIELDS[index].getData(&field, src, srcSize))
		{
			HxLOG_Warning("%s(%s) Cannot get data\n", __FUNCTION__, s_EPG_HUMAX_FIELDS[index].field.name);
			return NULL;
		}
	}
	return (DxField_t *)DLIB_MemDup(&field, sizeof(DxField_t));
}

HERROR			DLIB_EPG_HUMAX_Encode (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	return ERR_FAIL;
}

HERROR			DLIB_EPG_HUMAX_Decode (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	return ERR_FAIL;
}

HERROR			DLIB_EPG_HUMAX_Compress (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	return ERR_FAIL;
}

HERROR			DLIB_EPG_HUMAX_Extract (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	return ERR_FAIL;
}

HERROR			DLIB_EPG_HUMAX_MakeType (HUID uid, const void *src, HUINT32 srcSize, void **type)
{
	DxEvent_t	*event = NULL;

	if (srcSize == sizeof(DxEvent_t) && uid == DxEPG_CLONE_UID)
		event = dlib_epg_humax_CloneEvent((const DxEvent_t *)src);
	else
		event = dlib_epg_humax_NewEvent(uid, (const HUINT8 *) src, srcSize);

	if (event == NULL)
		return ERR_FAIL;

	*type = event;
	return ERR_OK;
}

void			DLIB_EPG_HUMAX_FreeType (void *type)
{
	RETURN_IF_VOID(type == NULL);
	dlib_epg_humax_FreeEvent(type);
}

const HCHAR*	DLIB_EPG_HUMAX_GetRepositoryPath(void)
{
	return dlib_epg_humax_GetRepositoryPath();
}

HUINT32		DLIB_EPG_HUMAX_GetThreshold (void)
{
	return 130000;
}

HUINT64		DLIB_EPG_HUMAX_GetThresholdSize (void)
{
	return 45000000;
}

static HCHAR * dlib_epg_humax_findRemovableCache(void)
{
	static HCHAR pszLink[512] = "";
	HCHAR * psLastLink = NULL;
	HUINT32	ulLast;
	dlib_epgsvc_t	*dlibsvc;
	HxHASH_TableIter_t	iter;


	ulLast = HLIB_STD_GetSystemTick();
	HLIB_HASH_IterInit(&iter, s_pstServiceEventTable);
	while (HLIB_HASH_IterNext(&iter, NULL, (void**)&dlibsvc))
	{
		if (ulLast > dlibsvc->ulLastUsed)
		{
			ulLast = dlibsvc->ulLastUsed;
			psLastLink = dlibsvc->pszUid;
		}
	}

	HxSTD_MemSet(pszLink,0,512); //temp

	if(psLastLink)
	{
		HLIB_STD_StrNCpySafe(pszLink, psLastLink, 512);
	}
	return pszLink;
}

static HERROR	dlib_epg_humax_DeleteAllFile(void)
{
	// Remove Event Info
	dlib_epgsvc_t	*dlibsvc;
	HxHASH_TableIter_t	iter;

	HLIB_HASH_IterInit(&iter, s_pstServiceEventTable);
	while (HLIB_HASH_IterNext(&iter, NULL, (void**)&dlibsvc))
	{
		HLIB_HASH_RemoveAll(dlibsvc->pstEventTable);

		if(NULL != dlibsvc->pszRepositoryName)
		{
			DLIB_Free(dlibsvc->pszRepositoryName);
			dlibsvc->pszRepositoryName = NULL;
		}

		if(NULL != dlibsvc->pszUid)
		{
			DLIB_Free(dlibsvc->pszUid);
			dlibsvc->pszUid = NULL;
		}
	}

	HLIB_HASH_RemoveAll(s_pstServiceEventTable);
	s_pstServiceEventTable = NULL;

	if (dlib_epg_humax_IsRepositoryAvailable() == TRUE)
	{
		const HCHAR		*szRepoPath = dlib_epg_humax_GetRepositoryPath();

		if(HLIB_DIR_Delete(szRepoPath) == ERR_FAIL)
		{
			HxLOG_Critical("IPEPG File delete Fail\n");
		}

		if (access(szRepoPath, R_OK | W_OK) != 0)
		{
			HLIB_DIR_Make(szRepoPath);
		}
	}

	dlib_epg_humax_MakeSvcHashTable();
	return ERR_OK;
	// Remove Service Info
	// Delete All File
}

static HERROR	dlib_epg_humax_closeFile(dlib_epgsvc_t	*svcInfo)
{
	HxLOG_Trace();
	RETURN_IF(svcInfo == NULL, ERR_FAIL);

	// Write files...
	if (dlib_epg_humax_IsRepositoryAvailable() == TRUE)
	{
		const HCHAR			*szRepoPath = dlib_epg_humax_GetRepositoryPath();

		if (svcInfo->bIsChanged && HLIB_HASH_Size(svcInfo->pstEventTable))
		{
			//	Write data to file.
			FILE				*file = NULL;
			HxHASH_TableIter_t	iter;
			dlib_epgevt_t		*dlibEvt;
			HINT32				evtKey, length;
			HUINT32				ulCRC;
			HUINT8				*p, *pBuffer;

			length = HLIB_HASH_Size(svcInfo->pstEventTable);
			p = pBuffer = (HUINT8*)DLIB_Malloc(sizeof(dlib_epgevt_t) * length);
			HLIB_HASH_IterInit(&iter, svcInfo->pstEventTable);
			while (HLIB_HASH_IterNext(&iter, (void**)&evtKey, (void**)&dlibEvt))
			{
				HxSTD_MemCopy(p, dlibEvt, sizeof(dlib_epgevt_t));
				p += sizeof(dlib_epgevt_t);
			}
#if defined(USE_CRC_CHECK)
			ulCRC = HLIB_MATH_GetCrc32(pBuffer, sizeof(dlib_epgevt_t) * length);
			HxLOG_Debug("data is changed --> write it number(%d) ulCRC(%x) Name[%s]\n", length, ulCRC, svcInfo->pszRepositoryName);
#endif
			if (access(szRepoPath, R_OK | W_OK) != 0)
				HLIB_DIR_Make(szRepoPath);

			file = fopen(svcInfo->pszRepositoryName, "wb");
			HxLOG_Debug("pszRepositoryName %d name[%s]\n", file, svcInfo->pszRepositoryName);

			if(file) fwrite(&length, 4, 1, file);

#if defined(USE_CRC_CHECK)
			if(file) (void)fseek(file, 8, SEEK_SET);
#endif

			if(file) fwrite(pBuffer, sizeof(dlib_epgevt_t) * length, 1, file);
			DLIB_Free(pBuffer);

			HLIB_HASH_IterInit(&iter, svcInfo->pstEventTable);
			while (HLIB_HASH_IterNext(&iter, (void*)&evtKey, (void**)&dlibEvt))
			{
				if(file) fwrite(dlibEvt, sizeof(dlib_epgevt_t) + dlibEvt->size, 1, file);
			}
#if defined(USE_CRC_CHECK)
			if(file) (void)fseek(file, 4, SEEK_SET);
			if(file) fwrite(&ulCRC, 4, 1, file);
#endif
			if(file) fclose(file);

		}
		else if(HLIB_HASH_Size(svcInfo->pstEventTable) == 0)
		{
			if ( access(svcInfo->pszRepositoryName, F_OK) != -1 )
			{
				HxLOG_Debug("Data is NULL delete file[%s]\n", svcInfo->pszRepositoryName);
				unlink(svcInfo->pszRepositoryName);
			}
		}
	}

	HLIB_HASH_RemoveAll(svcInfo->pstEventTable);
	if(NULL != svcInfo->pszRepositoryName)
	{
		DLIB_Free(svcInfo->pszRepositoryName);
		svcInfo->pszRepositoryName = NULL;
	}
	if(NULL != svcInfo->pszUid)
	{
		DLIB_Free(svcInfo->pszUid);
		svcInfo->pszUid = NULL;
	}
	return ERR_OK;
}

static void		dlib_epg_humax_closeOldFile(void)
{
	dlib_epgsvc_t	*svcInfo;
	HCHAR *	pszLink = NULL;

	HxLOG_Trace();

	pszLink = dlib_epg_humax_findRemovableCache();
	svcInfo = (dlib_epgsvc_t*)HLIB_HASH_Lookup(s_pstServiceEventTable, pszLink);
	if(svcInfo)
	{
		HxLOG_Debug("oldest service event info : [%s]\n", pszLink);
		dlib_epg_humax_closeFile(svcInfo);
		HLIB_HASH_Remove(s_pstServiceEventTable, (void*)pszLink);
	}
}

static void		dlib_epg_humax_closeAllFile(void)
{
	dlib_epgsvc_t	*dlibsvc;
	HxHASH_TableIter_t	iter;
	HxLOG_Trace();

	HLIB_HASH_IterInit(&iter, s_pstServiceEventTable);

	while (HLIB_HASH_IterNext(&iter, NULL, (void**)&dlibsvc))
	{
		dlib_epg_humax_closeFile(dlibsvc);
	}
	HLIB_HASH_RemoveAll(s_pstServiceEventTable);
	s_pstServiceEventTable = NULL;
	dlib_epg_humax_MakeSvcHashTable();
}

static dlib_epgsvc_t*	dlib_epg_humax_openFile(const HCHAR *pszLink)
{
	FILE	*file;
	HINT32	length, i;
	HUINT32	ulCRC, ulOldCRC;
	dlib_epgsvc_t	*svcInfo;
	dlib_epgevt_t	*evtInfo, *e, *evt;
	HINT32	nRead;

	HxLOG_Trace();

	if (dlib_epg_humax_IsRepositoryAvailable() != TRUE)
	{
		return NULL;
	}

	svcInfo = (dlib_epgsvc_t*)DLIB_Calloc(sizeof(dlib_epgsvc_t));
	svcInfo->pszRepositoryName = (HCHAR*)DLIB_Calloc(512);
	svcInfo->pszUid = (HCHAR*)DLIB_Calloc(512);

	HxSTD_snprintf(svcInfo->pszRepositoryName, 511, "%s/%s.dat", dlib_epg_humax_GetRepositoryPath(), pszLink);
	HxSTD_snprintf(svcInfo->pszUid, 511, "%s", pszLink);

#if defined(DLIB_PORT_HUMAX_EPG_SEE_PARAMETERS)
	HxLOG_Debug("[%s:%d] pszRepositoryName[%s] [%s]\n", __FUNCTION__, __LINE__, svcInfo->pszRepositoryName, pszLink);
#endif

	svcInfo->ulLastUsed = HLIB_STD_GetSystemTick();

	svcInfo->pstEventTable = HLIB_HASH_NewFull(dlib_epg_humax_hash, dlib_epg_humax_hashvalue_equal, NULL, (_HxDestroyNotify)DLIB_Free_CB);

	HLIB_HASH_Insert(s_pstServiceEventTable, (void*)DLIB_StrDup(pszLink), (void*)svcInfo);

	HxLOG_Debug("svcInfo RepositoryName : [%s]\n", svcInfo->pszRepositoryName);
	file = fopen(svcInfo->pszRepositoryName, "rb");
	if (file == NULL)
		return svcInfo;

	nRead = fread(&length, 4, 1, file);
	if(nRead < 1)
	{
		HxLOG_Error("Error : Get Length\n");
		fclose(file);
		return svcInfo;
	}

#if defined(USE_CRC_CHECK)
	nRead = fread(&ulOldCRC, 4, 1, file);
	if(nRead < 1)
	{
		HxLOG_Error("Error : Get Old Crc\n");
		fclose(file);
		return svcInfo;
	}
#endif

	if (length < 0)
	{
		fclose(file);
		return svcInfo;
	}

	if( length > MAX_EVENT_NUMBER )
	{
		fclose(file);
		return svcInfo;
	}

	evtInfo = (dlib_epgevt_t*)DLIB_Malloc(sizeof(dlib_epgevt_t) * length);
	nRead = fread(evtInfo, (HUINT32)(sizeof(dlib_epgevt_t) * length), 1, file);
	if(nRead < 1)
	{
		HxLOG_Error("Error : Get Event Info\n");
		DLIB_Free(evtInfo);
		fclose(file);
		return svcInfo;
	}
	e = evtInfo;

#if defined(USE_CRC_CHECK)
	ulCRC = HLIB_MATH_GetCrc32((HUINT8 *)evtInfo, sizeof(dlib_epgevt_t) * length);

	if(ulOldCRC != ulCRC)
	{
		HxLOG_Critical("CRC FAIL OLDCRC[%x] CRC[%x]\n", ulOldCRC, ulCRC);
		DLIB_Free(evtInfo);
		fclose(file);
	}
	else
#endif
	{
		HxLOG_Debug("link[%s] number of event [%d] ulCRC[%x]\n", pszLink, length, ulCRC);
		for (i = 0; i < length; i++)
		{
			HINT32	size = sizeof(dlib_epgevt_t) + e->size;

			nRead = 0;
			evt = (dlib_epgevt_t*)DLIB_Calloc(size);
			nRead = fread(evt, 1, size, file);
			e++;

			if (nRead == size)
			{
				HLIB_HASH_Insert(svcInfo->pstEventTable, (void*)IPEPG_MAKE_KEY(evt->uid), (void*)evt);
			}
			else
			{
				HxLOG_Error("fail to get humax epg info [%d/%d]\n", nRead, size);
				DLIB_Free(evt);
			}
		}
		DLIB_Free(evtInfo);
		fclose(file);
	}

	(void)nRead;
	return svcInfo;
}

static dlib_epgsvc_t*	dlib_epg_humax_getSvcInfo(const HCHAR *pszLink)
{
	dlib_epgsvc_t	*pstSvcInfo;

	pstSvcInfo = (dlib_epgsvc_t*)HLIB_HASH_Lookup(s_pstServiceEventTable, (void*)pszLink);

	if (pstSvcInfo == NULL)
	{
		//	check cache count and close it if needs
		HxLOG_Debug("dlib_epg_humax_getSvcInfo pstSvcInfo NULL size[%d]\n", HLIB_HASH_Size(s_pstServiceEventTable));
		if (HLIB_HASH_Size(s_pstServiceEventTable) > MAX_IPEPG_SERVICE_NUM)
			dlib_epg_humax_closeOldFile();

		pstSvcInfo = dlib_epg_humax_openFile(pszLink);
	}
	else
	{
		HxLOG_Debug("dlib_epg_humax_getSvcInfo It is a pstSvcInfo size[%d]\n", HLIB_HASH_Size(s_pstServiceEventTable));
	}

	return pstSvcInfo;
}

HERROR	dlib_epg_humax_read_file(dlib_epgsvc_t *pstSvcInfo, HUID evtUid, HINT32 *pnSize, void **ppvData)
{
	HUINT8			*pucData;
	dlib_epgevt_t	*evtInfo;

	if(pstSvcInfo == NULL)
		return ERR_FAIL;

	evtInfo = HLIB_HASH_Lookup(pstSvcInfo->pstEventTable, (void*)IPEPG_MAKE_KEY(evtUid));
	if (evtInfo == NULL)
	{
		return ERR_FAIL;
	}

	*pnSize = evtInfo->size;
	pucData = (HUINT8*)DLIB_Malloc(*pnSize);
	HxSTD_MemCopy(pucData, evtInfo->data, *pnSize);
	*ppvData = (void*)pucData;

	pstSvcInfo->ulLastUsed = HLIB_STD_GetSystemTick();
	return ERR_OK;
}

HERROR	dlib_epg_humax_write_file(dlib_epgsvc_t *pstSvcInfo, HUID evtUid, HINT32 nSize, const void *pvData)
{
	dlib_epgevt_t	*evtInfo;

	if(pstSvcInfo == NULL)
		return ERR_FAIL;

	evtInfo = HLIB_HASH_Lookup(pstSvcInfo->pstEventTable, (void*)IPEPG_MAKE_KEY(evtUid));
	if(nSize == 0)
	{
		//HxLOG_Debug("remove Event UID[%lld].\n", evtUid);
		if(evtInfo) HLIB_HASH_Remove(pstSvcInfo->pstEventTable, (void*)IPEPG_MAKE_KEY(evtUid));
		pstSvcInfo->bIsChanged = TRUE;
		pstSvcInfo->ulLastUsed = HLIB_STD_GetSystemTick() - 2*60*60;
	}
	else
	{
		if (evtInfo)
		{
			if ((evtInfo->size == nSize) && (HxSTD_MemCmp(evtInfo->data, pvData, nSize) == 0))
			{
				//HxLOG_Debug("same data comming...\n");
				return ERR_OK;
			}
			HLIB_HASH_Remove(pstSvcInfo->pstEventTable, (void*)IPEPG_MAKE_KEY(evtUid));
		}

		evtInfo = (dlib_epgevt_t*)DLIB_Malloc(sizeof(dlib_epgevt_t) + nSize);
		evtInfo->size = nSize;
		evtInfo->uid = evtUid;
		HxSTD_MemCopy(evtInfo->data, pvData, nSize);
		HLIB_HASH_Insert(pstSvcInfo->pstEventTable, (void*)IPEPG_MAKE_KEY(evtUid), (void*)evtInfo);

		pstSvcInfo->bIsChanged = TRUE;
		pstSvcInfo->ulLastUsed = HLIB_STD_GetSystemTick() - 2*60*60;
	}

	return ERR_OK;
}

HERROR			DLIB_EPG_HUMAX_ReadData(HUID uid, const HCHAR *pszLink, void **ppvData, HUINT32 *pnSize)
{
	HUID			evtUid;
	dlib_epgsvc_t	*pstSvcInfo;
	const HCHAR 	*pszTempLink = pszLink;

	evtUid = uid;

	pstSvcInfo = dlib_epg_humax_getSvcInfo(pszTempLink);
	HxLOG_Debug("readData : uid[%lld] pszLink[%s]\n", evtUid, pszTempLink);
	return dlib_epg_humax_read_file(pstSvcInfo, evtUid, pnSize, ppvData);
}

HERROR			DLIB_EPG_HUMAX_WriteData(HUID uid, const HCHAR *pszLink, const void *pvData, HUINT32 size)
{
	HUID			evtUid;
	dlib_epgsvc_t	*pstSvcInfo;

	evtUid = uid;

	if(uid == -1 && size == 0)
	{
		return dlib_epg_humax_DeleteAllFile();
	}
	else
	{
		const HCHAR 	*pszTempLink = pszLink;

		HxLOG_Debug("writeData : uid[%lld] pvData[%08x] size[%d] pszLink[%s]\n", evtUid, pvData, size, pszTempLink);

		pstSvcInfo = dlib_epg_humax_getSvcInfo(pszTempLink);
		return dlib_epg_humax_write_file(pstSvcInfo, evtUid, size, pvData);
	}
}

HERROR			DLIB_EPG_HUMAX_LinkName(HUID uid, const void *pvData, HUINT32 size, void *name, HUINT32 nameSize)
{
	DxIpEvent_t * pEvent = (DxIpEvent_t *)pvData;

	RETURN_IF(pvData == NULL || size == 0 || name == NULL || nameSize == 0, FALSE);
#ifdef CONFIG_FUNC_EPG_USE_SVCUID
	HxSTD_PrintToStrN(name, nameSize, "%d"
					, pEvent->stBase.ulSvcUid
				);
#else
	HxSTD_PrintToStrN(name, nameSize, "%x.%x.%x"
					, pEvent->stBase.usOnId
					, pEvent->stBase.usTsId
					, pEvent->stBase.usSvcId
				);
#endif
	return TRUE;
}

HERROR DLIB_EPG_HUMAX_SyncData(void)
{
	dlib_epg_humax_closeAllFile();
	return ERR_OK;
}

HERROR DLIB_EPG_HUMAX_OnStorageAdded (DxStorage_Info_t *pstStorage)
{
#if defined(CONFIG_IPEPG_REPOSITORY_HDD)
	HINT32				 nCnt;

	if ((pstStorage->ulFlags & eDxSTORAGE_FLAG_SYSTEM) && (TRUE != s_bDlibEpgHmx_DeviceRegistered))
	{
		for (nCnt = 0; nCnt < pstStorage->nPartitionNum; nCnt++)
		{
			DxStorage_PartitionInfo_t	*pstPartition = &(pstStorage->astPartitionInfos[nCnt]);

			if (pstPartition->ulFlags & eDxSTORAGE_FLAG_SYSTEM)
			{
				s_bDlibEpgHmx_DeviceRegistered = TRUE;
				HxSTD_StrNCpy(s_szDlibEpgHmx_RegisteredDevice, pstStorage->szDevName, DxSTORAGE_NAME_LEN - 1);
				s_szDlibEpgHmx_RegisteredDevice[DxSTORAGE_NAME_LEN - 1] = '\0';

				dlib_epg_humax_SetRepositoryPath(pstPartition->szMountPath);
				return ERR_OK;
			}
		}
	}
#endif

	return ERR_OK;
}

HERROR DLIB_EPG_HUMAX_OnStorageRemoved (DxStorage_Info_t *pstStorage)
{
#if defined(CONFIG_IPEPG_REPOSITORY_HDD)
	if (TRUE == s_bDlibEpgHmx_DeviceRegistered)
	{
		if (HxSTD_StrNCmp(pstStorage->szDevName, s_szDlibEpgHmx_RegisteredDevice, DxSTORAGE_NAME_LEN - 1) == 0)
		{
			s_bDlibEpgHmx_DeviceRegistered = FALSE;
			HxSTD_MemSet(s_szDlibEpgHmx_RegisteredDevice, 0, DxSTORAGE_NAME_LEN);

			dlib_epg_humax_ResetRepositoryPath();
		}
	}
#endif

	return ERR_OK;
}

static void	dlib_epg_humax_Print_Event(DxEvent_t *event)
{
#if defined(DLIB_PORT_HUMAX_EPG_SEE_PARAMETERS)
	HINT32		i;
#endif

	if(event == NULL)
		return;

#if defined(DLIB_PORT_HUMAX_EPG_SEE_PARAMETERS)
	HxLOG_Print(
			"\nDxEvent_t(0x%X) refCount:%d\n"
			"\ttype:%s\n"
			"\tsvcuid:%d"
			"\teventId:0x%X\n"
			"\tstartTime:%s, duration:%usec\n"
			"\trunning status:0x%X, freeCAmode:%d\n"
			"\tname:\'%s\'\n"
			"\ttext:\'%s\'\n"
			"\tlongText:\'%s\'\n"
			"\tparantal rating:%d\n"
			, (int)event, event->refCount
			, DxEPG_NAMEOF(event->type)
			, event->svcuid
			, event->eventId
			, HLIB_DATETIME_UnixTimeToString((UNIXTIME)event->startTime, NULL, 0), event->duration
			, event->runStatus, event->freeCaMode
			, DxEPG_EVENT_GET_STRING(event->name)
			, DxEPG_EVENT_GET_STRING(event->text)
			, DxEPG_EVENT_GET_STRING(event->longText)
			, event->parentalRating
		);

	HxLOG_Print("\tcontent:");
	for (i = 0 ; i < DxEVENT_CONTENT ; i++)
		HxLOG_Print(" %04X", event->content[i]);
	HxLOG_Print("\n");

	HxLOG_Print("\tdrm : %d\n"
			"\tSeriesId : %d, Season : %d\n"
			"\tEpisode : %d / %d\n"
			"\tseries type : %s\n"
			"\tlink[0] %s  id : %d\n"
			"\tlink[1] %s  id : %d\n"
			"\tthumbnail : %s\n"
			"\tactor      : %s\n"
			"\tactress 	 : %s\n"
			"\tdirector   : %s\n"
			"\tperson     : %s\n"
			,event->extension.ipEpg.drm
			,event->extension.ipEpg.seriesId
			,event->extension.ipEpg.season
			,event->extension.ipEpg.episodeNum
			,event->extension.ipEpg.totalNum
			,(event->extension.ipEpg.seriesType == 1)?"using Series ID":"using CRID"
			,(event->extension.ipEpg.eventLink[0].ucType == 1)?"Same":"Next", event->extension.ipEpg.eventLink[0].ulProgId
			,(event->extension.ipEpg.eventLink[1].ucType == 1)?"Same":"Next", event->extension.ipEpg.eventLink[1].ulProgId
			,event->extension.ipEpg.thumbnailURL
			,event->extension.ipEpg.actor
			,event->extension.ipEpg.actress
			,event->extension.ipEpg.director
			,event->extension.ipEpg.person
		);
#else
	return;
#endif
}

void 			DLIB_EPG_HUMAX_Print_Event(FILE *fp, const void *t)
{
	DxEvent_t	*event = (DxEvent_t *)t;
	HINT32		i;

	if(event == NULL || fp == NULL)
		return;

	HLIB_FILE_Printf(fp,
			"\nDxEvent_t(0x%X) refCount:%d\n"
			"\ttype:%s\n"
			"\tsvcuid:%d"
			"\teventId:0x%X\n"
			"\tstartTime:%s, duration:%usec\n"
			"\trunning status:0x%X, freeCAmode:%d\n"
			"\tname:\'%s\'\n"
			"\ttext:\'%s\'\n"
			"\tlongText:\'%s\'\n"
			"\tparantal rating:%d\n"
			, (int)event, event->refCount
			, DxEPG_NAMEOF(event->type)
			, event->svcuid
			, event->eventId
			, HLIB_DATETIME_UnixTimeToString((UNIXTIME)event->startTime, NULL, 0), event->duration
			, event->runStatus, event->freeCaMode
			, DxEPG_EVENT_GET_STRING(event->name)
			, DxEPG_EVENT_GET_STRING(event->text)
			, DxEPG_EVENT_GET_STRING(event->longText)
			, event->parentalRating
		);

	HLIB_FILE_Printf(fp, "\tcontent:");
	for (i = 0 ; i < DxEVENT_CONTENT ; i++)
		HLIB_FILE_Printf(fp, " %04X", event->content[i]);
	HLIB_FILE_Printf(fp, "\n");
	HLIB_FILE_Printf(fp,
			"\tdrm : %d\n"
			"\tSeriesId : %d, Season : %d\n"
			"\tEpisode : %d / %d\n"
			"\tseries type : %s\n"
			"\tlink[0] %s  id : %d\n"
			"\tlink[1] %s  id : %d\n"
			"\tthumbnail : %s\n"
			"\tactor      : %s\n"
			"\tactress 	 : %s\n"
			"\tdirector   : %s\n\n"
			,event->extension.ipEpg.drm
			,event->extension.ipEpg.seriesId
			,event->extension.ipEpg.season
			,event->extension.ipEpg.episodeNum
			,event->extension.ipEpg.totalNum
			,(event->extension.ipEpg.seriesType == 1)?"using Series ID":"using CRID"
			,(event->extension.ipEpg.eventLink[0].ucType == 1)?"Same":"Next", event->extension.ipEpg.eventLink[0].ulProgId
			,(event->extension.ipEpg.eventLink[1].ucType == 1)?"Same":"Next", event->extension.ipEpg.eventLink[1].ulProgId
			,event->extension.ipEpg.thumbnailURL
			,event->extension.ipEpg.actor
			,event->extension.ipEpg.actress
			,event->extension.ipEpg.director
		);
}

