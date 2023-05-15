/**************************************************************
*
*	http://www.humaxdigital.com
*
*	@data			2012/10/24
*	@author			humax
**************************************************************/


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


/*
* ⓒ 2011-2012 Humax Co., Ltd.
* This program is produced by Humax Co., Ltd. ("Humax") and
* the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
* non-assignable, non-transferable and non-exclusive license to use this Software.
* You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
* you agree to the responsibility to take all reasonable efforts to protect the any information
* you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
* reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
* If you have no authorized license, discontinue using this Software immediately.
*
* THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
* IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
* IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
*  CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.
*
* This License is effective until terminated. You may terminate this License at any time by destroying all copies
* of the Software including all documentation. This License will terminate immediately without notice from Humax
* to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
* of the Software and all documentation.
*
* The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
* All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
* in the Republic of Korea.
*/


#ifndef __NOVA_CAS__H__
#define __NOVA_CAS__H__

#include <nova_port.h>
#include <stdbool.h>
#include "nova_errcode.h"
#include "nova_types.h"

#include "hlib.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TEMP_NOVA_CAS


//-
#define NOVA_BOOL(x)							(x==ERR_OK ? NOVASPREAD_TRUE : NOVASPREAD_FALSE)
#define NOVA_MAX_NUM_OF_TS					(10)				// Obama에서 올라오는 모든 transportSessionInfo임, 직접 obama에서 transportSessionId를 가지고오는 방법으로 변경하자 .
#define NOVA_TRANSPORTSESSIONID_SUM		(100)//(15)				// multiscreen과 다른 action과의 transportsessionId가 같으면 안되어서 10을 더해줌...확인 필요.
#define NOVA_INVALID_TSID						(0xFFFFFFFF)	// 다른곳에서 정의한것이 존재하면 대신 사용하자 
#define NOVA_INVALID_TRIPLEID					(0xFFFFFFFF)
#define NOVA_INVALID_KEYHANDLE				(0xFFFFFFFF)
#define NOVA_INVALID_PID						(0xFFFF)
#define NOVA_INVALID_INDEX						(-1)
#define NOVA_MAX_CAINFO_STRING_LENGTH 		(50)

//-
#define NOVA_MEM_Alloc(a)				HLIB_STD_MemAlloc(a)
#define NOVA_MEM_Free(a)				HLIB_STD_MemFree(a)

#define NOVA_memset(a,b,c)				HxSTD_memset(a,b,c)
#define NOVA_memcpy(a,b,c)				HxSTD_memcpy(a,b,c)
#define NOVA_memcmp(a,b,c)			HxSTD_memcpy(a,b,c)
#define NOVA_strcmp(a,b)				HxSTD_StrCmp(a,b)
#define NOVA_strcpy(a,b)				HxSTD_StrNCpy(a,b)


//-
#if defined(CONFIG_DEBUG)
/* DEBUG : Novaspread-S : DRM */
#define NOVA_PORT_DRM_ERR			"NOVA_PORT_DRM_ERR"
#define NOVA_PORT_DRM_INFO			"NOVA_PORT_DRM_INFO"
#define NOVA_CAS_DRM_ERR			"NOVA_CAS_DRM_ERR"
#define NOVA_CAS_DRM_INFO			"NOVA_CAS_DRM_INFO"
/* DEBUG : Novaspread-S : CA */
#define NOVA_PORT_CA_ERR				"NOVA_PORT_CA_ERR"
#define NOVA_PORT_CA_INFO			"NOVA_PORT_CA_INFO"
#define NOVA_CAS_CA_ERR				"NOVA_CAS_CA_ERR"
#define NOVA_CAS_CA_INFO				"NOVA_CAS_CA_INFO"
#define NOVA_CAS_MAIN_ERR			"NOVA_CAS_MAIN_ERR"
#define NOVA_CAS_MAIN_INFO			"NOVA_CAS_MAIN_INFO"
#else
/* DEBUG : Novaspread-S : DRM */
#define NOVA_PORT_DRM_ERR			"NOVA_PORT_DRM_ERR"
#define NOVA_PORT_DRM_INFO			"NOVA_PORT_DRM_INFO"
#define NOVA_CAS_DRM_ERR			"NOVA_CAS_DRM_ERR"
#define NOVA_CAS_DRM_INFO			"NOVA_CAS_DRM_INFO"
/* DEBUG : Novaspread-S : CA */
#define NOVA_PORT_CA_ERR				"NOVA_PORT_CA_ERR"
#define NOVA_PORT_CA_INFO			"NOVA_PORT_CA_INFO"
#define NOVA_CAS_CA_ERR				"NOVA_CAS_CA_ERR"
#define NOVA_CAS_CA_INFO				"NOVA_CAS_CA_INFO"
#define NOVA_CAS_MAIN_ERR			"NOVA_CAS_MAIN_ERR"
#define NOVA_CAS_MAIN_INFO			"NOVA_CAS_MAIN_INFO"
#endif

#define MAX_CAS_INFO_STRING_LEGTH			(50)
#define MAX_DATA_LENGTH						(1024)
#define	MAX_NOVA_DATA_SIZE					(sizeof(NOVA_CAS_Item_t))

typedef struct
{
	HUINT32 NovaCasType;
	HUINT8 ucData[MAX_DATA_LENGTH];
} NOVA_CAS_Item_t;

typedef struct
{
	HUINT8 	ucChipsetUid[MAX_CAS_INFO_STRING_LEGTH];
	HUINT8 	ucCaVendor[MAX_CAS_INFO_STRING_LEGTH];
	HUINT8 	ucChipsetType[MAX_CAS_INFO_STRING_LEGTH];
	HUINT8 	ucChipsetRevision[MAX_CAS_INFO_STRING_LEGTH];
	HUINT8 	ucCaVersion[MAX_CAS_INFO_STRING_LEGTH];
	HUINT8 	ucCaNumber[MAX_CAS_INFO_STRING_LEGTH];
	HUINT8  ucSmartcardInserted;
	HUINT8 	ucSmartcardSuitable;
	HUINT8 	ucSmartcardType[MAX_CAS_INFO_STRING_LEGTH];
	HUINT8 	ucSmartcardNumber[MAX_CAS_INFO_STRING_LEGTH];
	HUINT8 	ucSmartcardStatus;
	HUINT32	ulExpirationDate;
}NOVA_CAS_Info_t;

typedef struct
{
	HUINT32 ulTransportSessionId;
	void *pContext;
	NovaspreadTDrmLicenseChangeListener DrmLicenseChangeListener;

	HUINT8 *pLicense;
	HUINT32 ulSize;
	HUINT32 ulRecordHandle;	// record stop시 xmgr_cas_NadvlStopRecord(ulRecordHandle) 로 handle을 넘겨주기위해 사용된다. 아니면 다른 tuner 단에서 관리해야하나? ts id로 handle 관리하자.
} NOVA_CAS_DrmLicenseNotifier_t;

typedef struct
{
	void *pContext;
	NovaspreadTCaPlatformUsageRulesReceivedListener CaPlatformUsageRulesReceivedListener;

	HUINT8 *pUsageRules;
	HUINT32 ulSize;
} NOVA_CAS_PlatformUsageRulesNotifier_t;

typedef struct
{
	void *pContext;
	HUINT32 ulTransportSessionId;
	NovaspreadTCaServiceUsageRulesReceivedListener	CaServiceUsageRulesReceivedListener;

	HUINT8 *pUsageRules;
	HUINT32 ulSize;	
} NOVA_CAS_ServiceUsageRulesNofitier_t;

typedef struct
{
	Handle_t hAction;
	HUINT32 ulTransportSessionId;

	HINT32 usOnId;
	HINT32 usTsId;
	HINT32 usSvcId;

	HUINT16 usAudioPid;
	HUINT16 usVideoPid;
} NOVA_CAS_TransportSessionInfo_t;

typedef struct
{
	HUINT32 ulTransportSessionId;
	HUINT32 ulKeyHandler;
	HUINT8	ucKey[128];	// max key size == 16 byte
	HUINT32	ulKeyLength;	
} NOVA_CAS_KeyHandlerInfo_t;

typedef struct
{
	NOVA_CAS_TransportSessionInfo_t		stTsIdInfo;
	NOVA_CAS_DrmLicenseNotifier_t			stDrmLicense;
	NOVA_CAS_KeyHandlerInfo_t				stKeyHandler;
	NOVA_CAS_ServiceUsageRulesNofitier_t 	stServiceUsageRules;
} NOVA_CAS_TsNotifier_t;

typedef struct
{
	NOVA_CAS_PlatformUsageRulesNotifier_t stPlatformUsageRules;
	NOVA_CAS_TsNotifier_t	stTs[NOVA_MAX_NUM_OF_TS];
} NOVA_CAS_Notifier_t;

typedef struct
{
	NOVA_CAS_Info_t stCaInfo;
	NOVA_CAS_Notifier_t stNotifier;
} NOVA_CAS_t;

NOVA_CAS_t stNovaCas;





void NOVA_CAS_PrintHexDump(HUINT8 *pusTitle, HUINT8 *pusData, HUINT32 ulSize);
NOVA_CAS_t * NOVA_CAS_GetCas(void);
void NOVA_CAS_Init(void);

void NOVA_CMDRegister_CAS(const HCHAR *hCmdHandle);;
void	NOVA_CAS_Cmd(const HCHAR *hCmdHandle);

/*****************************************************************************************************
*	Novaspread-S : TransportSessionId
*****************************************************************************************************/
void	NOVA_CAS_TransportSessionInfoInit(void);
void		NOVA_CAS_ReleaseTransportSessionInfo(HUINT32  ulTransportSessionId);
HUINT32 NOVA_CAS_GetTransportSessionIdFromTripleId(HUINT32 hAction, HINT32 usOnId, HINT32 usTsId, HINT32 usSvcId);
void	NOVA_CAS_PrintTransportSessionInfo(void);
NOVA_CAS_TransportSessionInfo_t * NOVA_CAS_GetTransportSessionInfoFromTripleId(HUINT32 hAction, HINT32 usOnId, HINT32 usTsId, HINT32 usSvcId);

/*****************************************************************************************************
*	Novaspread-S : DRM
*****************************************************************************************************/
int NOVA_CAS_DRM_DrmSetParameters (HUINT32 ulTransportSessionId, NovaspreadTDrmLicenseParameters * pLicenseParameters);
int  NOVA_CAS_DRM_DrmSetLicenseChangeListener (HUINT32  ulTransportSessionId, NovaspreadTDrmLicenseChangeListener Listener,  void * pContext);
void NOVA_CAS_DRM_init(void);
void NOVA_CAS_DRM_PrintLicenseParameters(NovaspreadTDrmLicenseParameters *pDrmLicenseParameters);
int NOVA_CAS_DRM_ReleaseDrmLisntener(HUINT32  ulTransportSessionId);
HINT32 NOVA_CAS_DRM_GetDrmListenerIndexFromTsId(HUINT32  ulTransportSessionId);
HUINT32 NOVA_CAS_DRM_GetKeyHandlerFromTsId(HUINT32 ulTransportSessionId);
NOVA_CAS_KeyHandlerInfo_t *  NOVA_CAS_DRM_GetKeyInfoFromTsId(HUINT32 ulTransportSessionId);
// PORT
void NOVA_PORT_LicenseNotifier(NOVA_CAS_DrmLicenseNotifier_t *pDrmListener, HBOOL bRequestLicenseAgain);

/*****************************************************************************************************
*	Novaspread-S : CA
*****************************************************************************************************/
void NOVA_CAS_CA_Init(void);
void NOVA_CAS_CaSetPlatformUsageRulesReceivedListener (NovaspreadTCaPlatformUsageRulesReceivedListener *pListener, void * pContext);
int NOVA_CAS_CA_SetServiceUsageRulesReceivedListener ( HUINT32 ulTransportSessionId, NovaspreadTCaServiceUsageRulesReceivedListener Listener, void * pContext );
HERROR NOVA_CAS_CA_GetNuid(HUINT32 *pChipsetNuid);
void NOVA_CAS_CA_InitCaInfo(void);
NOVA_CAS_Info_t * NOVA_CAS_CA_GetCaInfo(void);
// PORT
void NOVA_PORT_ServiceUsageRulesNotifier(NOVA_CAS_ServiceUsageRulesNofitier_t *pServiceUsageRules);
void NOVA_PORT_PlatformUsageRulesNotifier(NOVA_CAS_PlatformUsageRulesNotifier_t *pPlatformUsageRules);
extern int DI_MI_DECRYPTOR_SEC_SessionEncrypt(unsigned int *pKeyHandle, unsigned char *pucInput, unsigned char *pucOutput, unsigned int ulSize, unsigned char *pKey);
extern void *DI_MI_DECRYPTOR_SOC_Malloc(unsigned int unBytes);
extern void DI_MI_DECRYPTOR_SOC_Free(void *pMemory);

#ifdef __cplusplus
}
#endif
#endif
