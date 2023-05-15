/*************************************************************************************************************
	File            : pl_dmr.h
	author          : IPM tipmsw1@humaxdigital.com
	comment         :
	date            :
	attention       :

	(c) 2011-2012 Humax Co., Ltd.
	This program is produced by Humax Co., Ltd. (Humax) and
	the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
	non-assignable, non-transferable and non-exclusive license to use this Software.
	You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
	you agree to the responsibility to take all reasonable efforts to protect the any information
	you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
	reverse engineer or extract the source code of this Software unless you have Humax's written permission
	to do so.
	If you have no authorized license, discontinue using this Software immediately.

	THE SOFTWARE IS PROVIDED ¡°AS IS¡± AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES,
	EITHER EXPRESS, IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
	IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR
	INDIRECT, SPECIAL, CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR
	INABILITY TO USE THE SOFTWARE.

	This License is effective until terminated. You may terminate this License at any time
	by destroying all copies of the Software including all documentation. This License will terminate
	immediately without notice from Humax to you if you fail to comply with any provision of this License.
	Upon termination, you must destroy all copies of the Software and all documentation.

	The laws of the Republic of Korea will apply to any disputes arising out of or relating to
	this Copyright Notice. All claims arising out of or relating to this Copyright Notice will be litigated
	in the Seoul Central District Court, in the Republic of Korea.
*************************************************************************************************************/

#define	________________Header_Files_______________________________

#include "pl_common.h"
#include "pl_dmr.h"

#include "mxkernel.h"
#include "mxdlna.h"
#include "mxdlna_dmr.h"

#define ________________Private_Mecro_and_Defines__________________
/* Default Service Info */
#define DMR_DEFAULT_SERVICE_PORT		55200
#define	DMR_DEFAULT_UDN_PREFIX			"1E96FF1A-6771-2F44-B0C5-"

/* PrepareForConnection */
#define DMR_SESSIONID_AV			1
#define DMR_SESSIONID_IMAGE			2

#define CHECK_MODULE_START()      	(s_PL_DMR_Context.isStarted? TRUE:FALSE)
#define PRINT_MXDLNA_RET_VALUE()  	{HxLOG_Print("[%s:%d] RetValue :: %d \n", __FUNCTION__, __LINE__, nRet);}
#define	PL_DMR_SNPRINTF(x, v)		HxSTD_snprintf(x, sizeof(x), "%s", v)

#define ________________Typedef____________________________________
typedef struct
{
	HBOOL						is_init;
	HBOOL						isStarted;

	PL_DMR_ServiceInfo_t		stServiceInfo;
	PL_COMMON_ProductInfo_t		stProductInfo;

	PL_DMR_AVT_EventCallback	stAVT_cb;

	/* Media Platying Info */
	CO_MediaInstence_t			*pstCurrentItem;
	PL_DMR_UPnPState_e			ePlayState;

	HUINT64						ullPlayPostion;
	HUINT64						ullDuration;
} PL_DMR_Context_t;

#define ________________Static_Variables___________________________
static PL_DMR_Context_t s_PL_DMR_Context = { 0 };

const HCHAR* s_PL_DMR_DEFAULT_PROTOCOLINFO[] = {
	/* Video Profile List */
	"http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=MPEG_TS_SD_NA;DLNA.ORG_FLAGS=8D300000000000000000000000000000",
	"http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=MPEG_TS_SD_NA_T;DLNA.ORG_FLAGS=8D300000000000000000000000000000",
	"http-get:*:video/mpeg:DLNA.ORG_PN=MPEG_TS_SD_NA_ISO;DLNA.ORG_FLAGS=8D300000000000000000000000000000",
	"http-get:*:video/mpeg:DLNA.ORG_PN=MPEG_PS_NTSC;DLNA.ORG_FLAGS=8D300000000000000000000000000000",

	"http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=MPEG_TS_SD_EU;DLNA.ORG_FLAGS=8D300000000000000000000000000000",
	"http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=MPEG_TS_SD_EU_T;DLNA.ORG_FLAGS=8D300000000000000000000000000000",
	"http-get:*:video/mpeg:DLNA.ORG_PN=MPEG_TS_SD_EU_ISO;DLNA.ORG_FLAGS=8D300000000000000000000000000000",
	"http-get:*:video/mpeg:DLNA.ORG_PN=MPEG_PS_PAL;DLNA.ORG_FLAGS=8D300000000000000000000000000000",

	"http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=MPEG_TS_JP_T;DLNA.ORG_FLAGS=8D300000000000000000000000000000",
	"http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=AVC_TS_JP_AAC_T;DLNA.ORG_FLAGS=8D300000000000000000000000000000",
	"http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=AVC_TS_HD_60_AC3_T;DLNA.ORG_FLAGS=8D300000000000000000000000000000",
	"http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=AVC_TS_HD_24_AC3_T;DLNA.ORG_FLAGS=8D300000000000000000000000000000",
	"http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=MPEG_TS_SD_JP_MPEG1_L2_T;DLNA.ORG_FLAGS=8D300000000000000000000000000000",
	"http-get:*:application/x-dtcp1;video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=DTCP_MPEG_TS_JP_T;DLNA.ORG_FLAGS=8D310000000000000000000000000000",
	"http-get:*:application/x-dtcp1;video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=DTCP_AVC_TS_JP_AAC_T;DLNA.ORG_FLAGS=8D310000000000000000000000000000",
	"http-get:*:application/x-dtcp1;video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=DTCP_AVC_TS_HD_60_AC3_T;DLNA.ORG_FLAGS=8D310000000000000000000000000000",
	"http-get:*:application/x-dtcp1;video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=DTCP_MPEG_TS_SD_JP_MPEG1_L2_T;DLNA.ORG_FLAGS=8D310000000000000000000000000000",

	"http-get:*:video/ts:*",
	"http-get:*:video/x-msvideo:*",
	"http-get:*:video/mp4:*",
	"http-get:*:video/avi:*",
	"http-get:*:video/mpeg:*",
	"http-get:*:video/fli:*",
	"http-get:*:video/flv:*",
	"http-get:*:video/x-ms-wmv:*",
	"http-get:*:video/x-ms-avi:*",
	"http-get:*:video/x-flv:*",
	"http-get:*:video/x-fli:*",
	"http-get:*:video/x-ms-asf:*",
	"http-get:*:video/x-ms-asx:*",
	"http-get:*:video/x-ms-wmx:*",
	"http-get:*:video/x-ms-wvx:*",
	"http-get:*:video/x-msvideo:*",

	/* Audio Profile List */
	"http-get:*:audio/mpeg3:*",
	"http-get:*:audio/mpeg:*",
	"http-get:*:audio/mp3:*",
	"http-get:*:audio/L16;channels=1;rate=44100:DLNA.ORG_PN=LPCM",
	"http-get:*:audio/L16;channels=2;rate=44100:DLNA.ORG_PN=LPCM",
	"http-get:*:audio/L16;channels=1;rate=48000:DLNA.ORG_PN=LPCM",
	"http-get:*:audio/L16;channels=2;rate=48000:DLNA.ORG_PN=LPCM",

	/* Photo Profile List */
	"http-get:*:image/jpeg:DLNA.ORG_PN=JPEG_SM;DLNA.ORG_FLAGS=8C900000000000000000000000000000",
	"http-get:*:image/jpeg:DLNA.ORG_PN=JPEG_MED;DLNA.ORG_FLAGS=8C900000000000000000000000000000",
	"http-get:*:image/jpeg:DLNA.ORG_PN=JPEG_LRG;DLNA.ORG_FLAGS=8C900000000000000000000000000000",
	"http-get:*:image/jpeg:DLNA.ORG_PN=JPEG_TN;DLNA.ORG_FLAGS=8C900000000000000000000000000000",
	"http-get:*:image/jpeg:DLNA.ORG_PN=JPEG_SM_ISO;DLNA.ORG_FLAGS=8C900000000000000000000000000000",
	"http-get:*:image/jpeg:DLNA.ORG_PN=JPEG_LRG_ISO;DLNA.ORG_FLAGS=8C900000000000000000000000000000",

	"\0"
};

#define ________________Internal_APIs_Prototype____________________
static PL_DMR_Context_t *__get_context( void );

static HINT32 __cms_CbPrepareConnection( void *hDmr, \
										HCHAR *pRemoteProtocolInfo, \
										HCHAR *pPeerConnectionManager, \
										HINT32 nPeerConnectionID, \
										HCHAR *pDirection );

static HINT32 __avt_CbSetURI( void *hDmr, HINT32 nInstanceID, HCHAR *pUri, void *pData );
static HINT32 __avt_CbPlay( void *hDmr, HINT32 nInstanceID, HCHAR *pPlayspeed );
static HINT32 __avt_CbStop( void *hDmr, HINT32 nInstanceID );
static HINT32 __avt_CbPause( void *hDmr, HINT32 nInstanceID );

static HINT32 __rcs_CbSetVolume( void *hDmr, HINT32 nInstanceID, HCHAR *pChannel, HUINT16 nVolume );
static HINT32 __rcs_CbSetMute( void *hDmr, HINT32 nInstanceID, HCHAR *pChannel, HINT32 bMute );

static HINT32 __avt_CbSeekAbsPos( void *hDmr, HINT32 nInstanceID, HLONG lPosition );
static HINT32 __avt_CbSeekRelPos( void *hDmr, HINT32 nInstanceID, HLONG lPosition );

static HINT32 __avt_CbGetRelTimePos( void *hDmr, HINT32 nInstanceID, HLONG *pTime );
static HINT32 __avt_CbGetAbsTimePos( void *hDmr, HINT32 nInstanceID, HLONG *pTime );
static HINT32 __rcs_CbGetVolume( void *hDmr, HINT32 nInstanceID, HCHAR *pChannel, HUINT16 *pVolume );

MX_INT32 PL_USERDATA_Make( MXDLNA_CDSOBJECT * object, void * appData );

#define ________________Internal_Functions_________________________
/* DMR을 위한 Context를 초기화. */
static PL_DMR_Context_t *__get_context( void )
{
	return &s_PL_DMR_Context;
}

#if defined(CONFIG_ENHANCED_SECURITY)

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <sys/utsname.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>

int	__security_get_localhost_ip(int **pp_int)
{
	char szBuffer[16 * sizeof(struct ifreq)] = {0};

	struct ifconf ifConf = {0};
	struct ifreq ifReq = {0};
	int nResult = 0;
	int LocalSock = 0;
	struct sockaddr_in LocalAddr = {0};
	int tempresults[16] = {0};
	int ctr=0;
	int i = 0;

	if ((LocalSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
		return -1;
	}

	/* Get the interface configuration information... */
	ifConf.ifc_len = sizeof szBuffer;
	ifConf.ifc_ifcu.ifcu_buf = (caddr_t)szBuffer;
	nResult = ioctl(LocalSock, SIOCGIFCONF, &ifConf);
	if (nResult < 0) {
		close(LocalSock);
		return -1;
	}
	/* Cycle through the list of interfaces looking for IP addresses. */
	for (i = 0;(i < ifConf.ifc_len);)
	{
		struct ifreq *pifReq = (struct ifreq *)((caddr_t)ifConf.ifc_req + i);
		i += sizeof *pifReq;

		/* See if this is the sort of interface we want to deal with. */
		strcpy (ifReq.ifr_name, pifReq -> ifr_name);
		if (ioctl (LocalSock, SIOCGIFFLAGS, &ifReq) < 0)
		{
			close(LocalSock);
			return -1;
		}
		/* Skip loopback, point-to-point and down interfaces, */
		/* except don't skip down interfaces */
		/* if we're trying to get a list of configurable interfaces. */
		if ((ifReq.ifr_flags & IFF_LOOPBACK) || (!(ifReq.ifr_flags & IFF_UP)))
		{
			continue;
		}
		if (pifReq -> ifr_addr.sa_family == AF_INET)
		{
			/* Get a pointer to the address... */
			memcpy (&LocalAddr, &pifReq -> ifr_addr, sizeof pifReq -> ifr_addr);
			if (LocalAddr.sin_addr.s_addr != htonl (INADDR_LOOPBACK))
			{
				tempresults[ctr] = LocalAddr.sin_addr.s_addr;
				++ctr;
			}
		}
	}

	close(LocalSock);
	*pp_int = (int*)HOM_MEMALLOC(sizeof(int)*(ctr));
	memcpy(*pp_int,tempresults,sizeof(int)*ctr);
	return(ctr);
}

static HINT32 __security_change_localhost_ip(HCHAR *pUri)
{
	HCHAR *pStrCur = NULL;
	HCHAR *pStrPos = NULL;
	HINT32 szStrPos = 0;
	HINT32 szUrl = HOM_STRLEN((void*)pUri);

	if(szUrl > 7)
	{
		pStrCur = pUri + 7;
		pStrPos = pStrCur;

		if( NULL == pStrPos) {
			HxLOG_Error( "[ PL_DMR ] Error> Change LXC localhost URL \n");
			return -1;
		}

		while( *pStrPos != '\0' && *pStrPos != ':') {
			szStrPos++;
			pStrPos++;
		}

		if(*pStrPos != ':') {
			HxLOG_Error( "[ PL_DMR ] Error> Change LXC localhost URL \n");
			return -1;
		}

		{
			PL_COMMON_NetworkInfo_t	net_info;

			if (PL_COMMON_GetDefaultNetwork(&net_info) != ERR_OK) {
				HxLOG_Error( "[ PL_DMR ] Error> Change LXC localhost URL, IP Network\n");
				return -1;
			}

			if(HxSTD_StrNCmp( pStrCur, net_info.szIp, szStrPos) == 0)
			{
				int *pip_list = NULL;

				if(__security_get_localhost_ip(&pip_list) != -1 && pip_list != NULL)
				{
					struct in_addr __inaddr = {0};
					HCHAR URI[URL_MAX] = "\0";

					HxLOG_Debug("__security_change_localhost_ip if 2\n");
					__inaddr.s_addr = *pip_list;
					HxSTD_sprintf(&URI, "http://%s%s", (char *)inet_ntoa((struct in_addr)__inaddr), pStrPos);
					szUrl = HOM_STRLEN((void*)URI);

					HxSTD_MemCopy(pUri, &URI, szUrl);
				} else {
					HxLOG_Error( "[ PL_DMR ] Error> Change LXC localhost URL, HOMMA Network\n");
				}

				HOM_MEMFREE(pip_list);
			}
		}
	}

	return 0;
}
#endif



/* -------------------------------------------------------------*/
/* DMC -------> DMR 로 요청 되는 Event 							*/
/*--------------------------------------------------------------*/

/*  DMC가 URI를 설정시 호출 되는 콜백 - AVTransport:SetAVTransportURI() */

static HINT32 __avt_CbSetURI( void *hDmr, HINT32 nInstanceID, HCHAR *pUri, void *pData )
{
	PL_DMR_Context_t 	*pDmr = __get_context();
	CO_MediaInstence_t  *item = NULL;
	HINT32				nRet = 0;

	HOM_FUNC_ENTER;

	if( pUri == NULL )
		return -1;

	HxLOG_Print( "[ PL_DMR:%d ] Set URI( %s ) pData(%p)\n", __LINE__, pUri, pData );

	if (pDmr->pstCurrentItem != NULL)
	{
		HOM_MEMFREE( pDmr->pstCurrentItem );
		pDmr->pstCurrentItem = NULL;
	}
	item = HLIB_STD_MemCalloc(sizeof(CO_MediaInstence_t));
	pDmr->pstCurrentItem = item;

	if(pData != NULL)
	{
		PL_USERDATA_Make( (MXDLNA_CDSOBJECT *) pData, (void *)pDmr->pstCurrentItem );
		PL_USERDATA_Cds_Dump(pDmr->pstCurrentItem);
	}
	else
	{
		/*  Live Streaming ?  */
		HxSTD_snprintf(item->URI, sizeof(item->URI), "%s", pUri);
	}

#if defined(CONFIG_LXC_ONLY) || defined(CONFIG_CHROOT_AND_LXC)
	if(item != NULL)
	{
		if(item->URI != '\0') {
			__security_change_localhost_ip(&item->URI);
		}

		if(item->ThumbnailURI != '\0') {
			__security_change_localhost_ip(&item->ThumbnailURI);
		}
	}
#endif

	if (pDmr->stAVT_cb == NULL)
	{
		HxLOG_Error( "[ PL_DMR:%d ] Error> No registered event handler! [URI:%s]\n", __LINE__, pUri);
	}
	else
	{
		 pDmr->stAVT_cb(ePL_DMR_AVT_EVT_SETURI, 0, 0);
	}

	HOM_FUNC_LEAVE;

	return nRet;
}

/* DMC가 Play 요청시 호출 되는 콜백 - AVTransport:Play() */
static HINT32 __avt_CbPlay( void *hDmr, HINT32 nInstanceID, HCHAR *pPlayspeed )
{
	PL_DMR_Context_t	*pDmr = __get_context();

	HxLOG_Print("[%s:%d] DMR CB ### PLAY [%s] ### \n", __FUNCTION__, __LINE__, pPlayspeed);

	if(pDmr->stAVT_cb != NULL)
		 pDmr->stAVT_cb(ePL_DMR_AVT_EVT_PLAY, atoi(pPlayspeed), 0);

	return ERR_OK;
}

/* DMC가 Stop 요청시 호출 되는 콜백 - AVTransport:Stop() */
static HINT32 __avt_CbStop( void *hDmr, HINT32 nInstanceID )
{
	PL_DMR_Context_t	*pDmr = __get_context();

	HxLOG_Print("[%s:%d] DMR CB ### STOP ### \n", __FUNCTION__, __LINE__);

	if(pDmr->stAVT_cb != NULL)
		pDmr->stAVT_cb(ePL_DMR_AVT_EVT_STOP, 0, 0);

	return ERR_OK;
}

/* DMC가 Pause 요청시 호출 되는 콜백 - AVTransport:Pause() */
static HINT32 __avt_CbPause( void *hDmr, HINT32 nInstanceID )
{
	PL_DMR_Context_t	*pDmr = __get_context();

	HxLOG_Print("[%s:%d] DMR CB ### PAUSE ### \n", __FUNCTION__, __LINE__);

	if(pDmr->stAVT_cb != NULL)
		pDmr->stAVT_cb(ePL_DMR_AVT_EVT_PAUSE,0,0);

	return ERR_OK;
}

/* DMC가 Seek 요청시 호출 되는 콜백 - AVTransport:Seek() */
static HINT32 __avt_CbSeekAbsPos( void *hDmr, HINT32 nInstanceID, HLONG lPosition )
{
	PL_DMR_Context_t	*pDmr = __get_context();

	HxLOG_Print("[%s:%d] DMR CB ### SEEK [%ld] ### \n", __FUNCTION__, __LINE__, lPosition);

	if(pDmr->stAVT_cb != NULL)
		pDmr->stAVT_cb(ePL_DMR_AVT_EVT_SEEK, lPosition, 0);

	return ERR_OK;
}

/* DMC가 Seek 요청시 호출 되는 콜백 - AVTransport:Seek() */
static HINT32 __avt_CbSeekRelPos( void *hDmr, HINT32 nInstanceID, HLONG lPosition )
{
	PL_DMR_Context_t	*pDmr = __get_context();

	HxLOG_Print("[%s:%d] DMR CB ### SEEK [%ld] ### \n", __FUNCTION__, __LINE__, lPosition);

	if(pDmr->stAVT_cb != NULL)
		pDmr->stAVT_cb(ePL_DMR_AVT_EVT_SEEK, lPosition, 0);

	return ERR_OK;
}

/* DMC가 Volume 정보 설정시 호출 되는 콜백 - RenderingControl:SetVolume() */
static HINT32 __rcs_CbSetVolume( void *hDmr, HINT32 nInstanceID, HCHAR *pChannel, HUINT16 nVolume )
{
	HINT32  ulVolume = 0, ulVolumeRange = 0;
	PL_DMR_Context_t	*pDmr = __get_context();

	HxLOG_Print("[%s:%d] DMR CB ### Set Volume[%d] ### \n", __FUNCTION__, __LINE__, nVolume);

	/* 볼륨 최대값 확인 */
	HOMMA_INTEROP_OUTPUT_AUDIO_GetVolumeMax(&ulVolumeRange);

	/* 현재 볼륨값 확인 */
	HOMMA_INTEROP_OUTPUT_AUDIO_GetVolume(&ulVolume);

	if( ( HUINT16 )ulVolume != nVolume)
	{
		HxLOG_Print( "[%s:%d] Set Volume( %d ) Current( %d/%d )\n", \
				__FUNCTION__, __LINE__, 	nVolume, ulVolume, ulVolumeRange );
		HOMMA_INTEROP_OUTPUT_AUDIO_SetVolume( nVolume );
		mxdlna_dmr_setstate( 0, ePL_DMR_UPnPState_Volume, &nVolume);
	}

	if(pDmr->stAVT_cb != NULL)
		pDmr->stAVT_cb(ePL_DMR_AVT_EVT_VOLUME, nVolume, 0);

	return ERR_OK;
}


/* DMC가 Mute 설정시 호출 되는 콜백 - RenderingControl:SetMute() */
static HINT32 __rcs_CbSetMute( void *hDmr, HINT32 nInstanceID, HCHAR *pChannel, HINT32 bMute )
{
	PL_DMR_Context_t	*pDmr = __get_context();
	HBOOL   bSysMute = FALSE;

	HxLOG_Print("[%s:%d] DMR CB ### MUTE [%d] ### \n", __FUNCTION__, __LINE__, bMute);

	HOMMA_INTEROP_OUTPUT_AUDIO_GetMasterMute(&bSysMute);
	if( bSysMute != bMute )
	{
		HOMMA_INTEROP_OUTPUT_AUDIO_SetMasterMute( ( HBOOL )bMute );
		mxdlna_dmr_setstate( 0, ePL_DMR_UPnPState_Mute, &bMute);
	}

	if(pDmr->stAVT_cb != NULL)
		pDmr->stAVT_cb(ePL_DMR_AVT_EVT_MUTE, bMute, 0);


	return ERR_OK;
}

/* -------------------------------------------------------------*/
/* DMC 가 참조하는 값들을 위한 콜백 							*/
/*--------------------------------------------------------------*/

/* DMC가 Position 정보 요청시 호출 되는 콜백  - AVTransport:GetPositionInfo() */
/* Rel Position :: Platying Postion */
static HINT32 __avt_CbGetRelTimePos( void *hDmr, HINT32 nInstanceID, HLONG *pTime )
{
	PL_DMR_Context_t	*pDmr = __get_context();
	HxLOG_Print("[%s:%d] DMR CB ### (Position)Get Rel Time Pos - %lld ### \n", __FUNCTION__, __LINE__, pDmr->ullPlayPostion);
	*pTime = pDmr->ullPlayPostion;
	return ERR_OK;
}

/* DMC가 Position 정보 요청시 호출 되는 콜백  - AVTransport:GetPositionInfo() */
/* Abs Position :: Duration */
static HINT32 __avt_CbGetAbsTimePos( void *hDmr, HINT32 nInstanceID, HLONG *pTime )
{
	PL_DMR_Context_t	*pDmr = __get_context();
	HxLOG_Print("[%s:%d] DMR CB ### (Duration)Get Abs Time Pos -  %lld ### \n",	__FUNCTION__, __LINE__, pDmr->ullDuration);
	*pTime = pDmr->ullDuration;
	return ERR_OK;
}

/* DMC가 Volume 정보 요청시 호출 되는 콜백 - RenderingControl:GetVolume()*/
static HINT32 __rcs_CbGetVolume( void *hDmr, HINT32 nInstanceID, HCHAR *pChannel, HUINT16 *pVolume )
{
	HUINT32 ulVolume = 0;
	HOMMA_INTEROP_OUTPUT_AUDIO_GetVolume(&ulVolume);

	HxLOG_Print("[%s:%d] DMR CB ### Get Volume(%d)  ### \n", __FUNCTION__, __LINE__, ulVolume);

	*pVolume = ulVolume;
	return ERR_OK;
}

/* -------------------------------------------------------------*/
/* DMC Connection 관련 함수. 윗단에서 사용되지 않음. 			*/
/*--------------------------------------------------------------*/

/*  PrepareForConnection Action 요청시 호출 되는 콜백 - ConnectionManager:PrepareForConnection() */
static HINT32 __cms_CbPrepareConnection( void *hDmr, HCHAR *pRemoteProtocolInfo,
										HCHAR *pPeerConnectionManager,
										HINT32 nPeerConnectionID, HCHAR *pDirection )
{
	//PL_DMR_Context_t *pDmr = __get_context();

	HOM_FUNC_ENTER;

	if( pRemoteProtocolInfo != NULL )
	{
		MXDLNA_PROTOCOLINFO *pInfo = mxDlna_protocolinfoParser( pRemoteProtocolInfo , HxSTD_StrLen( pRemoteProtocolInfo ) );

		if( pInfo == NULL )
		{
			HxLOG_Error( "[ %s ] Error> protocolinfo is null.\n", __FUNCTION__);
			HOM_FUNC_LEAVE;
			return MXDLNA_DMR_RET_ERR;
		}

		HxLOG_Print( "[ PL_DMR ] Prepare for connection( rp=%s, pcm=%s, conID=%d )\n", \
				pRemoteProtocolInfo, pPeerConnectionManager, nPeerConnectionID );

		/* 재검토가 필요한 부분 */
		if( pInfo != NULL && pInfo->Profile != NULL )
		{
			if((HxSTD_StrNCmp( pInfo->Profile , "WMV" , 3 ) == 0)
				|| (HxSTD_StrNCmp( pInfo->Profile , "MPEG" , 4 ) == 0)
				|| (HxSTD_StrNCmp( pInfo->Profile , "MP3" , 3 ) == 0)
				|| (HxSTD_StrNCmp( pInfo->Profile , "LPCM" , 4 ) == 0))
			{
				HxLOG_Print("[ PL_DMR ] AV Session open( %s )\n", pInfo->Profile );
				return DMR_SESSIONID_AV;
			}
			else
			{
				HxLOG_Print("[ PL_DMR ] IMAGE Session open( %s )\n", pInfo->Profile );
				return DMR_SESSIONID_IMAGE;
			}
		}
		else
		{
			HxLOG_Print("[ PL_DMR ] Unknown Session open\n");
			return DMR_SESSIONID_IMAGE;
		}
	}

	/*  윗단에서 별도로 처리 하는 일 없음. - 제거 */

	HOM_FUNC_LEAVE;

	return 0;
}

/* library Callback 설정 */
HOM_Result_e	__avt_regist_handler(void)
{
	HINT32 nRet = 0;

	nRet = mxdlna_dmr_avt_setCallBack(	__avt_CbSetURI,
										__avt_CbPlay,
										__avt_CbStop,
										__avt_CbPause,
										__avt_CbGetRelTimePos,
										__avt_CbGetAbsTimePos );
	PRINT_MXDLNA_RET_VALUE();

	nRet = mxdlna_dmr_rcs_setCallBack( 	__rcs_CbGetVolume,
										__rcs_CbSetVolume,
										__rcs_CbSetMute );
	PRINT_MXDLNA_RET_VALUE();

	nRet = mxdlna_dmr_avt_setCallBack2( __avt_CbSeekAbsPos,
										__avt_CbSeekRelPos);

	PRINT_MXDLNA_RET_VALUE();

	/* ConnectionManager Callback 설정 */
	nRet = mxdlna_dmr_con_setCallBack( __cms_CbPrepareConnection );

	PRINT_MXDLNA_RET_VALUE();

	return eHOM_Result_Ok;
}

#define ________________Public_Functions___________________________
/*  DMR 초기화, 관련 정보 설정 */
HERROR	PL_DMR_InitDmr(HCHAR *pszFriendlyName, HUINT32 nPort, PL_DMR_Scope_e eScope) /* Initialized once */
{
	HERROR hErr;
	PL_DMR_Context_t	*pDmr = __get_context();
	PL_COMMON_MacInfo_t mac_info;

	HOM_FUNC_ENTER;

	/* TODO: where is initializing for DMR context */
	if (pDmr->is_init == TRUE) /* TODO: initialize() is needed while booting because this is initialzing before start() */
	{
		HxLOG_Error( "[ PL_DMR ] already inited PL_DMR(inited once)\n" );
		return ERR_OK;
	}

	if ((pszFriendlyName == NULL) || (HxSTD_StrEmpty(pszFriendlyName))) /* Naming is already done by caller */
	{
		HxLOG_Error( "[ PL_DMR ] Error> NULL friendlyName!\n" );
		return ERR_FAIL;
	}

	HxSTD_MemSet(pDmr, 0x00, sizeof(PL_DMR_Context_t));
	{
		pDmr->isStarted      = FALSE;
		pDmr->stAVT_cb       = NULL;
		pDmr->pstCurrentItem = NULL;
		pDmr->ePlayState     = ePL_DMR_UPnPState_Stopped;
		pDmr->ullPlayPostion = 0;
		pDmr->ullDuration    = 0;
		HxSTD_MemSet(&pDmr->stServiceInfo, 0x0, sizeof(PL_DMR_ServiceInfo_t));
		HxSTD_MemSet(&pDmr->stProductInfo, 0x0, sizeof(PL_COMMON_ProductInfo_t));
	}

	if( nPort < 1024 || nPort > 65535 )
		pDmr->stServiceInfo.nPortNumber = DMR_DEFAULT_SERVICE_PORT;
	else
		pDmr->stServiceInfo.nPortNumber = nPort;

	/* Set service information */
	HxSTD_MemSet(&mac_info, 0x0, sizeof(PL_COMMON_MacInfo_t));
	hErr = PL_COMMON_GetEthMac(&mac_info);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("Critical/Error> Cant get ethernet mac!\n" );
		/* It's situation that should not occur absolutely.  */
		if (1)
		{
			/* Dont use this; It can't be ensured that the "Default" is same with "ethernet". */
			HxSTD_MemSet(&mac_info, 0x0, sizeof(PL_COMMON_MacInfo_t));
			hErr = PL_COMMON_GetDefaultMac(&mac_info);
			if (hErr != ERR_OK)
			{
				HxLOG_Error("Error> Cant get default mac!\n" );
			}
		}
	}
	HxSTD_snprintf(pDmr->stServiceInfo.szUDN, sizeof(pDmr->stServiceInfo.szUDN), "%s%s", DMR_DEFAULT_UDN_PREFIX, mac_info.szMacAddr);
	HxSTD_snprintf(pDmr->stServiceInfo.szFriendlyName, sizeof(pDmr->stServiceInfo.szFriendlyName), "%s", pszFriendlyName);
	HxLOG_Print("MAC:%s, UDN:%s\n", mac_info.szMacAddr, pDmr->stServiceInfo.szUDN);

	HxSTD_MemSet(&pDmr->stProductInfo, 0x00, sizeof(PL_COMMON_ProductInfo_t));
	PL_COMMON_GetProductInfo(&pDmr->stProductInfo);
	HxSTD_snprintf(pDmr->stServiceInfo.szSerialNumber, sizeof(pDmr->stServiceInfo.szSerialNumber), "%s", pDmr->stProductInfo.szSerialNumber);
	pDmr->stServiceInfo.eScope = eScope;

	pDmr->is_init = TRUE;
	pDmr->isStarted = FALSE; /* Initialize because following DMR_Start() will set it */

	__avt_regist_handler();

	HOM_FUNC_LEAVE;

	return ERR_OK;
}

/* DMR 서비스 생성 - 시작 */
HERROR	PL_DMR_Start( void  )
{
	PL_DMR_Context_t		*pDmr = __get_context();
	HINT32					nRet = 0;
	HOM_FUNC_ENTER;

	nRet = mxDlnaDmr_SetScope( pDmr->stServiceInfo.eScope );
	HxLOG_Print( "[ PL_DMR ] mxDlnaDmr_SetScope( %d ) Ret( %d )\n", pDmr->stServiceInfo.eScope, nRet );

	mxdlna_dmr_setNames( pDmr->stProductInfo.szManufacturer,
						pDmr->stProductInfo.szManufacturerUrl,
						pDmr->stProductInfo. szModelName,
						pDmr->stProductInfo. szModelDescription,
						pDmr->stProductInfo. szModelNumber,
						pDmr->stProductInfo. szModelUrl );

	HxLOG_Print("[%s:%d] DMR Info :: Port(%d) Udn(%s) Name(%s) Serial(%s) \n", __FUNCTION__, __LINE__, \
			pDmr->stServiceInfo.nPortNumber, pDmr->stServiceInfo.szUDN, pDmr->stServiceInfo.szFriendlyName, pDmr->stServiceInfo.szSerialNumber);

	nRet = mxdlna_CreateDMR( 	pDmr->stServiceInfo.nPortNumber,
								pDmr->stServiceInfo.szUDN,
								pDmr->stServiceInfo.szFriendlyName,
								pDmr->stServiceInfo.szSerialNumber,
								s_PL_DMR_DEFAULT_PROTOCOLINFO );
	PRINT_MXDLNA_RET_VALUE();

	pDmr->isStarted = TRUE;

	if (pDmr->stAVT_cb != NULL)
	{
		pDmr->stAVT_cb(ePL_DMR_AVT_EVT_BRINGIN, 0, 0);
	}

	HOM_FUNC_LEAVE;
	return ERR_OK;
}

/* DMR 서비스 삭제 - 중단 */
HERROR	PL_DMR_Stop( void )
{
	PL_DMR_Context_t	*pDmr = __get_context();
	HINT32				nRet = 0;

	HOM_FUNC_ENTER;

	if (1)
	{
		pDmr->ullPlayPostion = 0;
		pDmr->ullDuration    = 0;
		if (pDmr->ePlayState != ePL_DMR_UPnPState_Stopped)
		{
			(void)PL_DMR_SetUPnPState( ePL_DMR_UPnPState_Stopped, NULL );
		}

		if (0) /* Dont initialize following vars because these are initialized once */
		{
			pDmr->stAVT_cb       = NULL;
			pDmr->pstCurrentItem = NULL;
			HxSTD_MemSet(&pDmr->stServiceInfo, 0x0, sizeof(PL_DMR_ServiceInfo_t));
			HxSTD_MemSet(&pDmr->stProductInfo, 0x0, sizeof(PL_COMMON_ProductInfo_t));
		}
	}

	nRet = mxdlna_DestroyDMR();
	PRINT_MXDLNA_RET_VALUE();

	pDmr->isStarted = FALSE;
	pDmr->is_init = FALSE;

	HOM_FUNC_LEAVE;

	return ERR_OK;
}

/* DMR Friendly Name 변경 */
HERROR	PL_DMR_SetName( HCHAR *pDmrName )
{
	HINT32				nRet = 0;
	if(!CHECK_MODULE_START()) 	return ERR_FAIL;

	/* FriendlyName 변경 후 Advertisement */
	nRet = mxDlnaDmr_renameFriendlyName( pDmrName );
	PRINT_MXDLNA_RET_VALUE();

	return ERR_OK;
}

/* DMR 서비스 범위 설정 */
HERROR	PL_DMR_SetServiceScope( PL_DMR_Scope_e	eScope )
{
	mxDlnaDmr_SetScope( eScope );
	return ERR_OK;
}

/* 현재 DMR의 PB State를 설정 - DMC에 전달 */
HERROR	PL_DMR_SetUPnPState( PL_DMR_UPnPState_e	eUPnPState, void *value )
{
	PL_DMR_Context_t	*pDmr = __get_context();

	switch(eUPnPState)
	{
		case ePL_DMR_UPnPState_Playing:
		case ePL_DMR_UPnPState_Stopped:
		case ePL_DMR_UPnPState_Paused:
			pDmr->ePlayState = eUPnPState;
			break;

		default :
			break;
	}

	/* Instance ID는 지원하지 않으므로 0 고정 */
	mxdlna_dmr_setstate( 0, eUPnPState, value );
	return ERR_OK;
}

HERROR	PL_DMR_GetUpnpState( PL_DMR_UPnPState_e *eUPnPState)
{
	PL_DMR_Context_t	*pDmr = __get_context();

	*eUPnPState = pDmr->ePlayState;
	return ERR_OK;
}

HERROR	PL_DMR_RegisterAVTEventCallback(PL_DMR_AVT_EventCallback cb)
{
	PL_DMR_Context_t	*pDmr = __get_context();

	pDmr->stAVT_cb = cb;

	return ERR_OK;
}

HERROR	PL_DMR_GetCurrentItem(CO_MediaInstence_t **item)
{
	PL_DMR_Context_t	*pDmr = __get_context();

	*item = pDmr->pstCurrentItem;

	return ERR_OK;
}

HERROR	PL_DMR_SetMediaDuration(HUINT64 ullTime)
{
	PL_DMR_Context_t	*pDmr = __get_context();
	pDmr->ullDuration = ullTime;

	return ERR_OK;
}

HERROR	PL_DMR_SetMediaPlayingPosition(HUINT64 ullTime)
{
	PL_DMR_Context_t	*pDmr = __get_context();
	pDmr->ullPlayPostion = ullTime;

	return ERR_OK;
}

/* -------------------------------------------------------------*/
/* Spec에 존재하는 내용은 아닌 부분으로 DMC에서 DMR의 마지막 재생관련하여 정보를 얻음.	*/
/*--------------------------------------------------------------*/

/* Fling용 URI 및 MetaData를 Set 할수 있는 API */
HERROR	PL_DMR_SetFlingInfo( HCHAR *pUri, HCHAR *pCurrentMeta )
{
	if(!CHECK_MODULE_START())
	{
		HxLOG_Error("DMR is not Inited. \n");
		return ERR_FAIL;
	}

	/* mklim Fling info를 비워줄 수 있도록 하기 위해 NULL check를 제거 */
	if( (pUri != NULL && pCurrentMeta == NULL) || (pUri == NULL && pCurrentMeta != NULL) )
	{
		return ERR_FAIL;
	}

	HxLOG_Print("[%s:%d] URI(%s) META(%s) \n", __FUNCTION__, __LINE__, pUri, pCurrentMeta);

	/* Instance ID는 지원하지 않으므로 0 고정 */
	mxdlna_dmr_setFlingInfo( 0, pUri, pCurrentMeta );

	HxLOG_Print("[%s:%d] URI(%s) META(%s) \n", __FUNCTION__, __LINE__, pUri, pCurrentMeta);

	return ERR_OK;
}

/* 현재 설정된 Fling용 URI를 확인하기 위한 함수. 호출부에서 메모리 해제 해야 함 */
HCHAR* PL_DMR_GetFlingURI( void )
{
	if(!CHECK_MODULE_START()) 	return NULL;

	return  mxdlna_dmr_getFlingInfoURI( 0 );
}

/* 현재 설정된 Fling용 Meta를 확인하기 위한 함수. 호출부에서 메모리 해제 해야 함 */
HCHAR* PL_DMR_GetFlingMeta( void )
{
	if(!CHECK_MODULE_START()) 	return NULL;

	 return  mxdlna_dmr_getFlingInfoMetadata( 0 );
}

HERROR	PL_DMR_RequestBringInURL(void)
{
	PL_DMR_Context_t		*pDmr = __get_context();
	HOM_FUNC_ENTER;

	if ((pDmr->isStarted != TRUE) || (pDmr->stAVT_cb == NULL))
	{
		HxLOG_Error("Error> Inactivated DMR status(%d)!\n", pDmr->isStarted);
	}

	if (pDmr->stAVT_cb != NULL)
	{
		pDmr->stAVT_cb(ePL_DMR_AVT_EVT_BRINGIN, 0, 0);
	}

	HOM_FUNC_LEAVE;
	return ERR_OK;
}
/* end od file */
