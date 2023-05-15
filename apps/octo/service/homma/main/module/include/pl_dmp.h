/*************************************************************************************************************
	File            : pl_dmp.h
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

#ifndef __PL_DMP_H__
#define __PL_DMP_H__

#define	________________Header_Files_______________________________ 
#include <common.h>
#include <pl_common.h>

#define ________________Typedef____________________________________

#define  PAL_DMP_SEARCH_ITEMTYPE_MASK 0x0000FF00
typedef enum
{
	Dmp_SearchMode_Title   = 0x00000000,
	Dmp_SearchMode_Author  = 0x00000001,
	Dmp_SearchMode_Video   = 0x00000100 << 0,
	Dmp_SearchMode_Music   = 0x00000100 << 1,
	Dmp_SearchMode_Photo   = 0x00000100 << 2,
	//ePAL_DMP_SEARCHFLAG_ALL       = 0x00000100 << 3,
} PL_DMP_SearchMode_e;

typedef enum
{
	Dmp_BrowseMode_Cache = 0,    
	Dmp_BrowseMode_Direct,
} PL_DMP_BrowseMode_t;

typedef enum
{
	Dmp_BrowseFlag_Metadata = 0,    
	Dmp_BrowseFlag_Children,
} PL_DMP_BrowseFlag_t;

typedef enum {
	Dmp_ServiceEvent_Undefiend,

	Dmp_ServiceEvent_Mscp_DirectBrowseResult,
	Dmp_ServiceEvent_Mscp_DirectBrowseResult_Failed,

	Dmp_ServiceEvent_Max,
} PL_DMP_ServiceEvent_t;

typedef	struct	{
	HCHAR	szFriendlyName[ TITLE_SIZE ];
	HCHAR	szUDN[ UDN_MAX ];
	HCHAR	szIp[ IP_SIZE ];
} PL_DMP_DmsInfo_t;

typedef struct {
	HINT32 nTotalMatch;
	HINT32 nReturnCnt;

	HBOOL bAllDMS; /* searchItem(udn) or searchAll() */
	HINT32 nError;
	HINT32 nReqId;        

	HCHAR szUDN[ UDN_MAX ];  ///< CDS UDN
	HCHAR szCID[ TITLE_SIZE ]; ///< Object ID
} PL_DMP_CDSResultInfo_t;

typedef void (*PL_DMP_BrowseResultCallback)		( HCHAR *pUdn, CO_MediaInstence_t *pList, HINT32 nCount, HINT32 nErrCode); 
typedef void (*PL_DMP_DeviceCallback) 			( PL_DMP_DmsInfo_t *pDMSInfo, HBOOL bAdded, HBOOL bCurrent );
typedef void (*PL_DMP_DeviceResponseSink)		( HCHAR *pUdn, HINT32 action, HINT32 error, void *data);
typedef void (*PL_DMP_SearchResultCallback)		( HCHAR *pUdn, PL_DMP_CDSResultInfo_t *pResult, CO_MediaInstence_t *pList, HINT32 nErr );

#define ________________Public_Functions___________________________

/* *
 * @brief 
 *
 * @param void  
 *
 * @return 
 **/ 
HERROR	PL_DMP_Init(void);


/* *
 * @brief 
 *
 * @param void  
 *
 * @return 
 **/ 
HERROR	PL_DMP_Start(void);


/* *
 * @brief 
 *
 * @param void  
 *
 * @return 
 **/ 
HERROR	PL_DMP_DeInit(void);


/* *
 * @brief 
 *
 * @param cbDevice  
 * @param cbRequestResult  
 *
 * @return 
 **/ 
HERROR	PL_DMP_RegisterEventCB(PL_DMP_DeviceCallback cbDevice, PL_DMP_BrowseResultCallback cbRequestResult, PL_DMP_SearchResultCallback cbSearchResult);

/* *
 * @brief 
 *
 * @param void  
 *
 * @return 
 **/ 
HERROR	PL_DMP_UnRegisterEventCB(void);


/* *
 * @brief 
 *
 * @param PL_DMP_IsDMPStarted  
 *
 * @return 
 **/ 
HBOOL	PL_DMP_IsDMPStarted(void);

/* *
 * @brief 
 *
 * @param void  
 *
 * @return 
 **/ 
HERROR	PL_DMP_M_Search(void);

/* *
 * @brief 
 *
 * @param pUdn  
 *
 * @return 
 **/ 
HERROR	PL_DMP_RefreshDms(HCHAR *pUdn);


/* *
 * @brief 
 *
 * @param nMode  
 * @param szUDN  
 * @param szCID  
 * @param Browse_Mode  
 * @param nSortOrder  
 * @param nStartIdx  
 * @param nReqNum  
 *
 * @return 
 **/ 
HERROR	PL_DMP_Browse(PL_DMP_BrowseMode_t nMode,
	   					HCHAR *szUDN, HCHAR *szCID, 
						PL_DMP_BrowseFlag_t Browse_Mode, 
						HUINT32 nSortOrder, 
						HUINT32 nStartIdx, 
						HUINT32 nReqNum);

/* *
 * @brief 
 *
 * @param pUdn  
 *
 * @return 
 **/
HERROR	PL_DMP_GetCurrentDms(HCHAR **pUdn);

/* *
 * @brief 
 *
 * @param pulCount  
 * @param pList  
 *
 * @return 
 **/
HERROR  PL_DMP_GetDmsList(HUINT32 *pulCount, PL_DMP_DmsInfo_t **pList);

/* *
 * @brief 
 *
 * @param pCount  
 * @param pList  
 *
 * @return 
 **/ 
HERROR  PL_DMP_GetCurrentItemList(HUINT32 *pCount, CO_MediaInstence_t **pList);

/*  ?�장 준�? */
HERROR	PL_DMP_RegisterUserDeviceEventResponseSink(PL_DMP_DeviceResponseSink cb);
HERROR	PL_DMP_MakeUserDeviceCaps(HCHAR *attribute, HCHAR *name, HCHAR *prefix);
void*	PL_DMP_GetMSCPHandle(void);
void*	PL_DMP_GetMSCPAHandle(void);

/*  ?�일 분리  */
typedef enum
{
	/*  hdrl */
	eRecord_Destinations,
	eRecord_StorageInfo,
	eRecord_ContainerID,

	/*	xsrs */
	eRecord_ScheduleList,
	eRecord_ScheduleConflict,
	eRecord_ScheduleCreate,
	eRecord_ScheduleDelete,
	eRecord_ScheduleUpdate,

	/* upload */
	eRecord_CreateObject,
	eRecord_DestroyObject,
} eRecordEvent;

typedef void (*PL_DMP_JP_ResultCallback)		( HINT32 nService, HCHAR *pUdn, HINT32 p1,  HINT32 p2, HINT32 p3);


HERROR    PL_DMP_Search( PL_DMP_SearchMode_e eMode, 
					  	HCHAR *szUDN, 
						HCHAR *szCID, 
						HCHAR *szKeyword, 
						HUINT32 nSortOrder, 
						HUINT32 nStartIdx, 
						HUINT32 nRequestedNum, 
						HINT32 nReqId );

HERROR    PL_DMP_SearchAll( PL_DMP_SearchMode_e eMode, 
					  	HCHAR *szCID, 
						HCHAR *szKeyword, 
						HUINT32 nSortOrder, 
						HINT32 nReqId );


#endif	//__PL_DLNA_DMP_H__
/* end od file */
