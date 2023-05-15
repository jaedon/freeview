/**************************************************************************************/
/**
 * @file pl_op.c
 * defines APIs's body
 *
 * Other Process interface
 *
 * @author  Sung-Su Yun(ssyun@humaxdigital.com)
 * @date    2012/10/10
 * @attention   Copyright (c) 2012 Humax - All rights reserved.
 *
**************************************************************************************
**/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include "pl_op.h"

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define	__MODULE__	"PL:OP"

#define NON_IPCMODE	0
#if NON_IPCMODE
//#include "db_svc.h"
#endif

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

typedef struct
{
	HBOOL	bInit;
} PL_OP_Context_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC PL_OP_Context_t s_PL_OP_Context = { 0, };


/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/
#define __________Internal_Function__________


PL_OP_Context_t *__otherprocess_GetContext( void )
{
	return &s_PL_OP_Context;
}



void __otherprocess_initContext( void )
{
	PL_OP_Context_t	*pContext = __otherprocess_GetContext();
	//HINT32				i = 0;
	
	if( pContext == NULL )
		return;

	HxSTD_MemSet( pContext, 0x00, sizeof( PL_OP_Context_t ) );
	
	( void )pContext;
}

#define __________Public_Function__________

HOM_Result_e	PL_OP_GetStandbyMode( PL_OP_STANDBYMODE_e *pStandbyMode )
{
	HOM_Result_e 			eRet = eHOM_Result_Ok;
	//HINT32					nValue = -1;

	HOM_FUNC_ENTER;

#if NON_IPCMODE
	( void )OxDB_SETUP_GetMenuItemValue( eDxSETUP_USER_LAST_STANDBY, &nValue );
	if(nValue == 0)
	{
		*pStandbyMode = ePL_OP_STADNBYMODE_OPERATION;
	}
	else
	{
		*pStandbyMode = ePL_OP_STADNBYMODE_ACTIVE;
	}
#else
	/* Not Implement */
#endif

	HOM_FUNC_LEAVE;

	return eRet;
}

HOM_Result_e	PL_OP_GetChannelServiceInfo( HINT32 nSvcUid, CO_CHANNEL_Info_t *pChannelinfo )
{
	HOM_Result_e 		eRet = eHOM_Result_Ok;

	HOM_FUNC_ENTER;

#if NON_IPCMODE
	SVC_Info_t			stSvcInfo;	
	int 				hError = 0;

	HOM_MEMSET( &stSvcInfo, 0x00, sizeof( SVC_Info_t ) );
	( void )OxDB_SVC_GetServiceInfo( nSvcUid, &stSvcInfo );

	pChannelinfo->usAudioPid = stSvcInfo.usAudioPid;
	pChannelinfo->usOnId = stSvcInfo.usOnId;
	pChannelinfo->usPcrPid = stSvcInfo.usPcrPid;
	pChannelinfo->usPmtPid = stSvcInfo.usPmtPid;
	pChannelinfo->usPrvIdx = stSvcInfo.usPrvIdx;
	pChannelinfo->usSvcId = stSvcInfo.usSvcId;
	pChannelinfo->usTsId = stSvcInfo.usTsId;
	pChannelinfo->usTsIdx = stSvcInfo.usTsIdx;
	pChannelinfo->usTtxPid = stSvcInfo.usTtxPid;
	pChannelinfo->usVideoPid = stSvcInfo.usVideoPid;
#else
	/* Not Implement */
#endif

	HOM_FUNC_LEAVE;

	return eRet;
}


/* end od file */
