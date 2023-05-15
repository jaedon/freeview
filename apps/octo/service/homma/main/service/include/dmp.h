/*************************************************************************************************************
	File            : dmp.h
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

#ifndef __DMP_H__
#define __DMP_H__

#define	________________Header_Files_______________________________ 
#include "pl_dmp.h"

#define ________________Typedef____________________________________
typedef enum
{
	eDMP_ADD_DMS = 0,
	eDMP_REMOVE_DMS,

	eDMP_ADD_ITEM,
	eDMP_SEARCH_ITEM,
} DMP_Event_e;

typedef void (*DMP_EventCallback_t)(HINT32 param1, HINT32 param2, HINT32 param3);

#define ________________Public_Functions___________________________

/**
 * @brief 
 *
 * @param void  
 *
 * @return 
 **/ 
HERROR	DMP_Start(void);

/**
 * @brief 
 *
 * @param void  
 *
 * @return 
 **/ 
HERROR	DMP_Stop(void);


/**
 * @brief 
 *
 * @param pUdn  
 *
 * @return 
 **/ 
HERROR  DMP_GetCurrentDms(HCHAR **pUdn);

/**
 * @brief 
 *
 * @param pUdn  
 * @param pCid  
 * @param nSortOrder  
 * @param nStartIdx  
 * @param nReqIdx  
 *
 * @return 
 **/ 
HERROR 	DMP_Browse(const HCHAR *pUdn, const HCHAR *pCid, HUINT32 nSortOrder, HUINT32 nStartIdx, HUINT32 nReqIdx);

/**
 * @brief 
 *
 * @param pUdn  
 * @param pCid  
 * @param nSortOrder  
 * @param nStartIdx  
 * @param nReqIdx  
 *
 * @return 
 **/ 
HERROR 	DMP_Search(const HCHAR *pUdn, const HCHAR *pCid, const HCHAR *pKeyword, HUINT32 nSortOrder, HUINT32 nStartIdx, HUINT32 nReqNum, HINT32 nReqId);

/**
 * @brief 
 *
 * @param pCid  
 * @param nSortOrder  
 *
 * @return 
 **/ 
HERROR 	DMP_SearchAll(const HCHAR *pCid, const HCHAR *pKeyword, HUINT32 nSortOrder, HINT32 nReqId);

/**
 * @brief 
 *
 * @param ulCount  
 * @param ppDmsList  
 *
 * @return 
 **/ 
HERROR	DMP_GetDmsList(HUINT32 *ulCount, PL_DMP_DmsInfo_t **ppDmsList);

/**
 * @brief 
 *
 * @param ulCount  
 * @param ppDmsList  
 *
 * @return 
 **/ 
HERROR	DMP_GetItemList(HUINT32 *ulCount, CO_MediaInstence_t **ppDmsList);


/**
 * @brief 
 *
 * @param cb  
 *
 * @return 
 **/ 
HERROR	DMP_RegisterEventCallback(DMP_EventCallback_t cb);

/**
 * @brief 
 *
 * @param void  
 *
 * @return 
 **/ 
HERROR	DMP_UnregisterEventCallback(void);

/**
 * @brief 
 *
 * @param void  
 *
 * @return 
 **/ 
HBOOL	DMP_IsStart(void);

/**
 * @brief 
 *
 * @param pUdn  
 *
 * @return 
 **/ 
HERROR	DMP_RefreshDms(HCHAR *pUdn);

#endif /* __DMP_H__ */
/* end od file */
