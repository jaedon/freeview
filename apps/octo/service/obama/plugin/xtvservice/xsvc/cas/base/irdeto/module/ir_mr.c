/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <pal_sc.h>

#include <_svc_cas_sub_dev_api.h>

#include "s3_smartcard.h"
#include "s3_maturity_rating.h"
#include "s3_cadrv.h"
#include "s3_service.h"

#include "vd_humax.h"
#include "ir_mr.h"
#include "ir_platform.h"
#include "ir_svc.h"

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

extern IR_DRV_SC_INFO	g_IrDrvScInfo[];
extern IR_SERVICE_INFO	g_IrSvcInfo[];



/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/




/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
/*******************************************************************/
/********************     declare Functions	   *********************/
/*******************************************************************/

/*******************************************************************/
/********************     definition Functions    ******************/
/*******************************************************************/

/***************************************************************************/
#define ________Local_Functions________
/***************************************************************************/

/***************************************************************************/
#define _______Global_Functions________
/***************************************************************************/

HUINT32 XSVC_CAS_IR_DRV_MR_CapabilityQuery(HUINT32 resourceId)
{
	ia_result	ret;
	msg_smartcard_query_data_st	*scQueryData;

	scQueryData = (msg_smartcard_query_data_st *)OxCAS_Malloc(sizeof(msg_smartcard_query_data_st));
	scQueryData->wResourceId = resourceId;

	HxLOG_Print("[SCELL_Message] MSG_MATURITY_RATING_CAPABILITY_QUERY\n");
	ret = SCELL_Message(MSG_MATURITY_RATING_CAPABILITY_QUERY, 0, sizeof(msg_smartcard_query_data_st), (void *)scQueryData);

	return ret;
}

HUINT32 XSVC_CAS_IR_DRV_MR_ConfigurationQuery(HUINT32 resourceId, HUINT8 *aucSubscriberPin,msg_mr_config_action_e eAction, HUINT8 bEnableMREvaluation )
{
	ia_result						ret;
	HUINT8						*scQueryData= NULL;
	msg_mr_config_query_st		mrConfigQuery;
	msg_mr_config_data_st		mrConfigData;
	HUINT16 wLenthOfMSG =0;

	if(eAction == IA_MR_CONFIG_READ)
	{
		HxLOG_Print("[SCELL_Message] Action  = IA_MR_CONFIG_READ\n");
		wLenthOfMSG = sizeof(msg_mr_config_query_st);
	}
	else if(eAction == IA_MR_CONFIG_UPDATE)
	{
		/* when do update action, payload would be lenghten */
		HxLOG_Print("[SCELL_Message] Action  = IA_MR_CONFIG_UPDATE\n");
		wLenthOfMSG = sizeof(msg_mr_config_query_st) + sizeof(msg_mr_config_data_st) ;
	}
	else
	{
		HxLOG_Print("CAS_IR_DRV_MR_ConfigurationQuery: action is not defined!!!\n");
		return IA_FAIL;
	}

	scQueryData = OxCAS_Malloc( wLenthOfMSG );
	if(scQueryData == NULL)
	{
		HxLOG_Print("CAS_IR_DRV_MR_ConfigurationQuery: Memory allocation failed!!!\n");
		return IA_FAIL;
	}

	mrConfigQuery.wResourceId = resourceId;
	mrConfigQuery.abSubscriberPin[0]= aucSubscriberPin[0];
	mrConfigQuery.abSubscriberPin[1]= aucSubscriberPin[1];
	mrConfigQuery.eAction = eAction;

	HxLOG_Print("[SCELL_Message] MSG_MATURITY_RATING_CONFIG_QUERY\n");
	HxLOG_Print("[SCELL_Message] subscriber pin = [%X][%X][%X][%X], \n", \
		(mrConfigQuery.abSubscriberPin[0]>>4)	& 0x0F,\
		(mrConfigQuery.abSubscriberPin[0])		& 0x0F,\
		(mrConfigQuery.abSubscriberPin[1]>>4)	& 0x0F, \
		(mrConfigQuery.abSubscriberPin[1])		& 0x0F );

	HxSTD_memcpy(scQueryData, &mrConfigQuery, sizeof(msg_mr_config_query_st) );

	/* when do update action, payload would be lenghten */
	if(eAction == IA_MR_CONFIG_UPDATE)
	{
		msg_mr_config_data_st stMRconfigData;

		/*fill all member */
		/*translate member type into irdeto bool from temporal type */
		stMRconfigData.fEnable = (bEnableMREvaluation==1)? IA_TRUE: IA_FALSE;
		HxLOG_Print("CAS_IR_DRV_MR_ConfigurationQuery: Update:  = %d\n",stMRconfigData.fEnable);
		/*adjust target offset */
		HxSTD_memcpy(scQueryData+sizeof(msg_mr_config_query_st) , &stMRconfigData, sizeof(msg_mr_config_data_st) );
	}

	ret = SCELL_Message(MSG_MATURITY_RATING_CONFIG_QUERY, 0,wLenthOfMSG , (void *)scQueryData);

	return ret;
}

HUINT32 XSVC_CAS_IR_DRV_MR_UserProfileQuery(HUINT32 resourceId, HUINT8 *aucSubscriberPin,msg_mr_user_profile_action_e eAction, msg_mr_user_profile_data_st *pstUserProfile)
{
	ia_result	ret;
	msg_mr_user_profile_query_st	*scQueryData;

	HxLOG_Print("CAS_IR_DRV_MR_UserProfileQuery: Action = %d, UserProfileID= %d\n", eAction, pstUserProfile->bProfileID);

	scQueryData = (msg_mr_user_profile_query_st *)OxCAS_Malloc( sizeof(msg_mr_user_profile_query_st) );
	scQueryData->wResourceId = resourceId;
	HxSTD_memcpy(scQueryData->abSubscriberPin, aucSubscriberPin, IR_MR_SUBSCRIBER_PIN_LEN );
	scQueryData->eAction = eAction;

	if((scQueryData->eAction == IA_MR_USER_PROFILE_READ) ||(scQueryData->eAction == IA_MR_USER_PROFILE_DELETE) )
	{
		/*In READ or DELETE query, stUserProfile contains input data: only the bProfileID shall be filled in, the other members will be ignored. */
		scQueryData->stUserProfile.bProfileID = pstUserProfile->bProfileID;

		/* back up the MR key for reading whole pf */
		if(scQueryData->eAction == IA_MR_USER_PROFILE_READ)
		{
			g_IrDrvScInfo[0].stIrScApiInfo.stMRInfo.abSubscriberPin[0] =  scQueryData->abSubscriberPin[0];
			g_IrDrvScInfo[0].stIrScApiInfo.stMRInfo.abSubscriberPin[1] =  scQueryData->abSubscriberPin[1];
		}
	}
	else if(scQueryData->eAction == IA_MR_USER_PROFILE_SET)
	{
		/*In the SET query, stUserProfile contains input data: only the bProfileID shall be filled in, the other members will be ignored. */
		scQueryData->stUserProfile.bProfileID = pstUserProfile->bProfileID;
		scQueryData->stUserProfile.bUserAge= pstUserProfile->bUserAge;
	}
	else
	{
		HxLOG_Print("CAS_IR_DRV_MR_UserProfileQuery Action(%d) is not defined!!!\n", scQueryData->eAction);
		return IA_FAIL;
	}

	HxLOG_Print("[SCELL_Message] MSG_MATURITY_RATING_USER_PROFILE_QUERY\n");
	ret = SCELL_Message(MSG_MATURITY_RATING_USER_PROFILE_QUERY, 0, sizeof(msg_mr_user_profile_query_st), (void *)scQueryData);

	return ret;
}


HUINT32 XSVC_CAS_IR_DRV_MR_UpdateResource(Handle_t hAction, HUINT8 *aucPIN)
{
	ia_result					eIARst;

	msg_service_define_data_st	svcDefineData;

	msg_resource_st				MsgRsc;

	HUINT8						*pServiceDefinetData = NULL;
	HUINT8						*pMRRscData=NULL;
	HUINT8						i, *p;
	HUINT16 					wRscID;

	msg_resource_st				MsgMRRsc;
	msg_mr_define_st 			st_mr_define;

	HUINT16 					ServiceDefineLength;

	/*First, Find matched resource ID with serivce handle */
	for( i = 0; i < IR_HUMAX_MAX_SERVICE; i++ )
	{

		if( (g_IrSvcInfo[i].hAction!= HANDLE_NULL) && (g_IrSvcInfo[i].hAction == hAction) )
		{
			wRscID = g_IrSvcInfo[i].MaturityRatingRscStatus.usResourceID;

			HxLOG_Print("jichoi: CAS_IR_DRV_MR_UpdateResource: wRscID = %d\n", wRscID);
			break;
		}
	}

	if( i == IR_HUMAX_MAX_SERVICE )
	{
		HxLOG_Print("CAS_IR_DRV_MR_UpdateResource:ERROR: not registerd service. hAction=0x%x\n", hAction);
		return HIR_NOT_OK;
	}

	svcDefineData.wServiceHandle = g_IrSvcInfo[i].usSCELL_SvcHandle;
	svcDefineData.bResourceCount =1;

	/* Maturity rating resource */
	MsgRsc.wResourceType = RESOURCE_MATURITY_RATING;
	MsgRsc.wResourceId = wRscID;
	MsgRsc.wResourceMgnt = MANAGE_UPDATE;

	/* when the resource is initially defined, the value shall be 0xFFFF, but  Only users pin is used. */
	st_mr_define.abMRPin[0]= aucPIN[0];
	st_mr_define.abMRPin[1]= aucPIN[1];

	ServiceDefineLength = sizeof(msg_service_define_data_st) + sizeof(msg_resource_st) + sizeof(msg_mr_define_st);

	/* Allocation memory for resource updating. */
	p =	pServiceDefinetData = (HUINT8 *)OxCAS_Malloc(ServiceDefineLength);
	if (pServiceDefinetData == NULL)
	{
		HxLOG_Print("[CAS_IR_DRV_MR_UpdateResource] Error : Mem Alloc Error 3\n");
		return HIR_NOT_OK;
	}

	HxSTD_memcpy(p, &svcDefineData, sizeof(msg_service_define_data_st) );
	p+= sizeof(msg_service_define_data_st);

	HxSTD_memcpy(p, &MsgRsc, sizeof(msg_resource_st) );
	p+= sizeof(msg_resource_st);

	HxSTD_memcpy(p, &st_mr_define, sizeof(msg_mr_define_st) );

	eIARst = SCELL_Message(MSG_SERVICE_DEFINE, 0, ServiceDefineLength, (void *)pServiceDefinetData);
	if( eIARst != IA_SUCCESS )
	{
		HxLOG_Print("[CAS_IR_DRV_MR_UpdateResource : Error(0x%X) From SCELL_Message for live]\n", eIARst);
		return HIR_NOT_OK;
	}

	return HIR_OK;
}
