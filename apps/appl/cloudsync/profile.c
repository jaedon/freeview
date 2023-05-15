/**************************************************************
*	http://www.humaxdigital.com
*	@author			humax - Europe/CIS team, hcyoo@humaxdigital.com
**************************************************************/
/*
* ¨Ï 2011-2012 Humax Co., Ltd.
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

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <vkernel.h>
#include <apk.h>
/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define PROFILE_MSGQ_SIZE			128
#define PROFILE_MSG_QUE_NAME		"ProfileMsgQ"
#define PROFILE_THREAD_STACK_SIZE 	SIZE_64K
/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef	enum
{
	PF_MSG_NONE = 0,
	PF_MSG_CHANGE_NETWORK_STATUS,
	PF_MSG_MAX
} Pf_MsgCommand_e;

typedef struct
{
	Pf_MsgCommand_e		eCmd;
	APKE_NET_STATUS_e 	eStatus;
} Pf_ChangeStatus_t;

typedef	union
{
	Pf_MsgCommand_e		eCmd;
	Pf_ChangeStatus_t	stStatusMsg;
} PfMsg_t;

/**@brief Cloud Sync Content structure. */
typedef struct tagPROFILE_CONTENT
{
	HULONG		ulProfileThreadId;		/**< Profile Thread ID. */
	HULONG		ulProfileMsgQId;		/**< Profile Thread Message Queue ID. */
	APKE_NET_STATUS_e 	eNetStatus;		/**< Net Status. */
} Pf_Content_t;

#define PF_CRITICAL_SESSION_IN	VK_SEM_Get( s_ulProfileSema );
#define PF_CRITICAL_SESSION_OUT	VK_SEM_Release( s_ulProfileSema );
/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
static Pf_Content_t		s_stPfContent;	/**< Profile Content Structure. */
static HLONG			s_ulProfileSema;

static HUINT8 * _pf_GetNetworkNotifyString( APKE_NET_STATUS_e eEvent )
{
	switch (eEvent)
	{
		ENUM_TO_STR(eNETWORK_STATUS_CON_SUCCESS);
		ENUM_TO_STR(eNETWORK_STATUS_CON_FAIL);
		ENUM_TO_STR(eNETWORK_STATUS_CON_AUTOIP_SUCCESS);
		ENUM_TO_STR(eNETWORK_STATUS_CON_AUTOIP_FAIL);
		ENUM_TO_STR(eNETWORK_STATUS_CABLE_LINK_CONNECTED);
		ENUM_TO_STR(eNETWORK_STATUS_CABLE_LINK_DISCONNECTED);
		ENUM_TO_STR(eNETWORK_STATUS_UNKNOWN);

		default :
			break;
	}

	return (HUINT8*)NULL;
}

static Pf_Content_t* _pf_GetContenxt( void )
{
	return &s_stPfContent ;
}

static HULONG _pf_GetMessageQId( void )
{
	return s_stPfContent.ulProfileMsgQId;
}

static void _pf_ChangeNetworkStatus( APKE_NET_STATUS_e status )
{
	Pf_Content_t	*pCt = NULL;

	HxLOG_Trace();

	PF_CRITICAL_SESSION_IN;

	pCt = _pf_GetContenxt();

	HxLOG_Debug( "_pf_ChangeNetworkStatus - (%s) -> (%s)\n", _pf_GetNetworkNotifyString( pCt->eNetStatus ), _pf_GetNetworkNotifyString( status ));

	pCt->eNetStatus = status;

	PF_CRITICAL_SESSION_OUT;
}

static void Profile_Thread( void )
{
	HERROR			hErr;
	PfMsg_t      	pfMsg;

	while ( 1 )
	{
		hErr = VK_MSG_Receive( _pf_GetMessageQId(), &pfMsg, sizeof( PfMsg_t ) );
		if ( hErr != VK_OK )
		{
			continue;
		}

		switch ( pfMsg.eCmd )
		{
			case PF_MSG_CHANGE_NETWORK_STATUS :
				_pf_ChangeNetworkStatus( pfMsg.stStatusMsg.eStatus );
				break;

			default :
				break;
		}
	}
}

HERROR PF_SetNetworkStatus( APKE_NET_STATUS_e eStatus )
{
	HERROR			hErr;
	Pf_Content_t	*pCt = NULL;
	PfMsg_t  		pfMsg;

	HxLOG_Trace();

	PF_CRITICAL_SESSION_IN;

	pCt = _pf_GetContenxt();

	if ( eStatus == pCt->eNetStatus )
	{
		HxLOG_Debug( "PF_SetNetworkStatus - Does not need to change - [%d], [%d]\n", eStatus, pCt->eNetStatus );
		PF_CRITICAL_SESSION_OUT;

		return ERR_OK;
	}

	PF_CRITICAL_SESSION_OUT;

	pfMsg.stStatusMsg.eCmd = PF_MSG_CHANGE_NETWORK_STATUS;
	pfMsg.stStatusMsg.eStatus = eStatus;

	hErr = VK_MSG_Send( _pf_GetMessageQId(), &pfMsg, sizeof( PfMsg_t ) );
	if ( hErr != VK_OK )
	{
	    return ERR_FAIL;
	}

	return ERR_OK;
}

void PF_InitProfile( void )
{
	HERROR	hErr;

	HxLOG_Trace();

	/* Create semaphore. */
	hErr = VK_SEM_Create( (unsigned long *)&s_ulProfileSema, "ProfileSema", VK_SUSPENDTYPE_PRIORITY );
	if ( hErr != VK_OK )
	{
		HxLOG_Error( "Profile Sema creation fail err = 0x%x\n", hErr );
		return;
	}

	PF_CRITICAL_SESSION_IN;

	/* Reset Context. */
	HxSTD_memset( &s_stPfContent, 0, sizeof(Pf_Content_t) );
	s_stPfContent.eNetStatus = eNETWORK_STATUS_UNKNOWN;

	PF_CRITICAL_SESSION_OUT;

	/* Create & Init message queue */
	hErr = VK_MSG_Create( PROFILE_MSGQ_SIZE, sizeof( PfMsg_t ), PROFILE_MSG_QUE_NAME, &s_stPfContent.ulProfileMsgQId, VK_SUSPENDTYPE_FIFO );
	if ( hErr != VK_OK )
	{
		HxLOG_Error( "Profile msgQ creation fail err = 0x%x\n", hErr );
		return;
	}

	/* Create and Start Profile Thread */
	hErr = VK_TASK_Create( (void *)Profile_Thread, VK_TASK_PRIORITY_MW_REF, PROFILE_THREAD_STACK_SIZE, (const char*)"ProfileThread", (void*)NULL, &s_stPfContent.ulProfileThreadId, 0 );
	if ( hErr != VK_OK )
	{
		HxLOG_Error( "Profile thread creation fail err = 0x%x\n", hErr );
		return;
	}

	hErr = VK_TASK_Start( s_stPfContent.ulProfileThreadId );
	if ( hErr != VK_OK )
	{
		HxLOG_Error( "Profile Thread start fail err = 0x%x\n", hErr );
		return;
	}
}

void PF_TermProfile( void )
{
	HERROR	hErr;

	HxLOG_Trace();

	/* Stop Profile Thread. */
	hErr = VK_TASK_Stop( s_stPfContent.ulProfileThreadId );
	if ( hErr != VK_OK )
	{
		HxLOG_Error( "Profile Thread stop fail err = 0x%x\n", hErr );
		return;
	}

	/* Destroy Profile Thread. */
	hErr = VK_TASK_Destroy( s_stPfContent.ulProfileThreadId );
	if ( hErr != VK_OK )
	{
		HxLOG_Error( "Profile Thread destroy fail err = 0x%x\n", hErr );
		return;
	}

	/* Destroy Message Queue. */
	hErr = VK_MSG_Destroy( s_stPfContent.ulProfileMsgQId );
	if ( hErr != VK_OK )
	{
		HxLOG_Error( "Profile Message Queue destroy fail err = 0x%x\n", hErr );
		return;
	}

	PF_CRITICAL_SESSION_IN;

	/* Reset Context. */
	HxSTD_memset( &s_stPfContent, 0, sizeof(Pf_Content_t) );

	PF_CRITICAL_SESSION_OUT;

	/* Destroy Message Sema. */
	hErr = VK_SEM_Destroy( s_ulProfileSema );
	if ( hErr != VK_OK )
	{
		HxLOG_Error( "Profile Sema destroy fail err = 0x%x\n", hErr );
		return;
	}
}


