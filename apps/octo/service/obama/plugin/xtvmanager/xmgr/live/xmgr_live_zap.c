/**
	@file     xmgr_zapping.c
	@brief    file_name & simple comment.

	Description: Applicatuion for searching Services\n
	Module: Application(Layer), Menu(module) \n
	Remarks : \n
	 Date			Modification				Name\n
	-----------	-------------------------------------	------------------\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/**
  * Character encoding.
  *
  * MS949
  *
  * This source file that uses MS949 encoding.
  * MS949 encoding �� ����ϴ� source file �Դϴ�.
  * MS949 encoding�����Ī��� source file�Ǫ���
  * Quelldatei, die MS949-Codierung verwendet.
  *
**/


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <db_svc.h>
#include <mgr_common.h>
#include <mgr_action.h>
#include <svc_av.h>
#include <svc_transcoder.h>

#include <mgr_live.h>
#include <xmgr_zapping.h>
#include "./local_include/_xmgr_live.h"
#include "./local_include/_xmgr_live_zap.h"

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
//	#define	CONFIG_ZAPPING_WITHIN_NOCONFLICTION

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

typedef struct
{
	/*
	- Setup DB�� ����Ǵ� last channel ����.
	- Last key�� ���ؼ��� �Ʒ� prev... ������ ��������, ���ٰ� ������ ��쿡�� ���� ���� �ִ� ä���� ���;� �ϹǷ� �¾����� current�� ����Ǿ�� �Ѵ�.
	- Context�� �ִ� handle�� group ������ �ӽ� �����̰� Ʃ���� Ȯ���� ���Ŀ� �� ���� ����Ǹ鼭 EEPROM�� ����ȴ�.
	- ���� handle�� ����� ���� �����Ƿ� �ݵ�� ���غ��� �ٸ� handle�� ��츸 update�Ǿ�� �Ѵ�.
	*/
	Handle_t			hCurSvc;

	DxSvcType_e			curSvcType; /* TV/Radio */
	DxDeliveryType_e	curDeliType; /* ����/������/���̺�/�Ƴ��α� */

	/*
	- Last key ������ ���Ǵ� �ٷ� �� ä���� service handle�� group.
	- ���� cur... �� �ٲ� ��쿡�� �� ������ cur... �� ��ü�Ǹ�, cur...�� channel type�� ���Ͽ� prevTv... �� prevRadio... ���� ī�ǵȴ�.
	*/
	Handle_t			hPrevSvc;

	/*
	- TV/Radio key�� ���� ä�� ������ ��� ���ȴ�.
	- TV/Radio ��ȯ�ÿ��� group�� �ش� group���� ������.
	*/
	Handle_t			hLastSvc_TV; /* ANA ���Ե� TV ��ü last service */
	Handle_t			hLastSvc_RADIO; /* Radio�� last service */

	/*
	- Delivery ��ȯ�� ���� ä�� ������ ��� ���ȴ�.
	- Delivery ��ȯ�ÿ��� group�� all�� ������.
	*/
	Handle_t			hLastSvc_SAT; /* ������ last service */
	Handle_t			hLastSvc_TER; /* �������� last service */
	Handle_t			hLastSvc_CAB; /* ���̺��� last service */
	Handle_t			hLastSvc_DTV; /* Digital channel�� ���յ� last service */
	Handle_t			hLastSvc_ANA; /* Analog channel�� last service */
} XmgrLastSvcInfo_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/
STATIC XmgrZapInfo_t		s_astZapInfo[NUM_VIEW_ACTION];	 /* �� action �� zapping ���� ����. */
STATIC XmgrLastSvcInfo_t	s_stLastMainSvcInfo;			 /* �� action �� zapping ���� �� main action���� ����Ǵ� last channel ����.
																������ ������ ������ ������ �ȵǰ� �Ѿ �� �ִ�. */

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/******************* Static Function Prototypes  *************************/
/*******************************************************************/

/*******************************************************************/
/************************	API Functions  *************************/
/*******************************************************************/

void XMGR_ZAP_ResetZappingInfo(HBOOL bLoadSetup)
{
	HERROR			err;
	Handle_t		hCurSvc = HANDLE_NULL;
	DbSvc_TsInfo_t 	tsInfo;
	DbSvc_Info_t		svcInfo;
	HINT32			i;

/* 1. Last service info �ʱ�ȭ */
	/* 1.1 Current channel list group �ʱ�ȭ */
	/*  >> ch list group ������� ���� */

	/* 1.2 Current service handle �ʱ�ȭ */
	err = DB_PARAM_GetItem(eDB_PARAM_ITEM_CUR_SVC, (HUINT32 *)&hCurSvc, 0);
	if( (bLoadSetup == TRUE) && (err == ERR_OK) )
	{
		err = DB_SVC_CheckValidServiceHandle(hCurSvc);
		if(err == ERR_OK)
		{
			s_stLastMainSvcInfo.hCurSvc = hCurSvc;
		}
		else
		{
			s_stLastMainSvcInfo.hCurSvc = HANDLE_NULL;
		}
	}
	else
	{
		s_stLastMainSvcInfo.hCurSvc = HANDLE_NULL;
	}

	HxSTD_memset(&svcInfo, 0, sizeof(DbSvc_Info_t));
	err = DB_SVC_GetServiceInfo(s_stLastMainSvcInfo.hCurSvc, &svcInfo);
	if(err == ERR_OK)
	{
		/* 1.3 Current service type �ʱ�ȭ �ϰ� TV/Radio Item ���ε� ����Ѵ�. */
		s_stLastMainSvcInfo.curSvcType = DbSvc_GetSvcType(&svcInfo);

		s_stLastMainSvcInfo.hLastSvc_TV = HANDLE_NULL;
		s_stLastMainSvcInfo.hLastSvc_RADIO = HANDLE_NULL;

		//	last svc�� ��ϵ� ��� �ش� svc�� default�� ������ش�.
#if	defined(CONFIG_APCORE_SAVE_LAST_TVRADIO_SVC)
		{
			HINT32	hTypeSvcHandle;

			err = DB_PARAM_GetItem(eDB_PARAM_ITEM_LAST_TV_SVC, (HUINT32*)&hTypeSvcHandle, sizeof(Handle_t));
			s_stLastMainSvcInfo.hLastSvc_TV = (err == ERR_OK) ? hTypeSvcHandle : HANDLE_NULL;
			err = DB_PARAM_GetItem(eDB_PARAM_ITEM_LAST_RADIO_SVC, (HUINT32*)&hTypeSvcHandle, sizeof(Handle_t));
			s_stLastMainSvcInfo.hLastSvc_RADIO = (err == ERR_OK) ? hTypeSvcHandle : HANDLE_NULL;
		}
#endif

		switch(DbSvc_GetSvcType(&svcInfo))
		{
			case eDxSVC_TYPE_TV :
				HxLOG_Print("\t\t=> Save to TV service.\n");
				s_stLastMainSvcInfo.hLastSvc_TV = s_stLastMainSvcInfo.hCurSvc;
				break;

			case eDxSVC_TYPE_RADIO :
				HxLOG_Print("\t\t=> Save to RADIO service.\n");
				s_stLastMainSvcInfo.hLastSvc_RADIO = s_stLastMainSvcInfo.hCurSvc;
				break;

			default :
				break;
		}


		/* 1.4 Current channel�� delivery type �ʱ�ȭ�ϰ� delivery type item���ε� ����Ѵ�. */
		HxSTD_memset(&tsInfo, 0, sizeof(DbSvc_TsInfo_t));
		err = DB_SVC_GetTsInfo(DbSvc_GetTsIdx(&svcInfo), &tsInfo);
		if (err == ERR_OK)
		{
			s_stLastMainSvcInfo.curDeliType = tsInfo.eDeliType;

			s_stLastMainSvcInfo.hLastSvc_DTV = HANDLE_NULL;
			s_stLastMainSvcInfo.hLastSvc_TER = HANDLE_NULL;
			s_stLastMainSvcInfo.hLastSvc_SAT = HANDLE_NULL;
			s_stLastMainSvcInfo.hLastSvc_CAB = HANDLE_NULL;
			s_stLastMainSvcInfo.hLastSvc_ANA = HANDLE_NULL;

			switch(tsInfo.eDeliType)
			{
				case eDxDELIVERY_TYPE_SAT :
					HxLOG_Print("\t\t=> Save to SAT & DTV service.\n");
					s_stLastMainSvcInfo.hLastSvc_SAT = s_stLastMainSvcInfo.hCurSvc;
					s_stLastMainSvcInfo.hLastSvc_DTV = s_stLastMainSvcInfo.hCurSvc;
					break;

				case eDxDELIVERY_TYPE_TER :
					HxLOG_Print("\t\t=> Save to TER & DTV service.\n");
					s_stLastMainSvcInfo.hLastSvc_TER = s_stLastMainSvcInfo.hCurSvc;
					s_stLastMainSvcInfo.hLastSvc_DTV = s_stLastMainSvcInfo.hCurSvc;
					break;

				case eDxDELIVERY_TYPE_CAB :
					HxLOG_Print("\t\t=> Save to CAB & DTV service.\n");
					s_stLastMainSvcInfo.hLastSvc_CAB = s_stLastMainSvcInfo.hCurSvc;
					s_stLastMainSvcInfo.hLastSvc_DTV = s_stLastMainSvcInfo.hCurSvc;
					break;

				default :
					break;
			}
		}
		else
		{
			s_stLastMainSvcInfo.curDeliType = eDxDELIVERY_TYPE_ALL;

			s_stLastMainSvcInfo.hLastSvc_DTV = HANDLE_NULL;
			s_stLastMainSvcInfo.hLastSvc_TER = HANDLE_NULL;
			s_stLastMainSvcInfo.hLastSvc_SAT = HANDLE_NULL;
			s_stLastMainSvcInfo.hLastSvc_CAB = HANDLE_NULL;
			s_stLastMainSvcInfo.hLastSvc_ANA = HANDLE_NULL;
		}
	}
	else
	{
		s_stLastMainSvcInfo.curDeliType = eDxDELIVERY_TYPE_ALL;

		s_stLastMainSvcInfo.hLastSvc_TV = HANDLE_NULL;
		s_stLastMainSvcInfo.hLastSvc_RADIO = HANDLE_NULL;
		s_stLastMainSvcInfo.hLastSvc_DTV = HANDLE_NULL;
		s_stLastMainSvcInfo.hLastSvc_TER = HANDLE_NULL;
		s_stLastMainSvcInfo.hLastSvc_SAT = HANDLE_NULL;
		s_stLastMainSvcInfo.hLastSvc_CAB = HANDLE_NULL;
		s_stLastMainSvcInfo.hLastSvc_ANA = HANDLE_NULL;
	}

	/* 1.5 Last service�� �ʱ�ȭ ���Ŀ��� �������� �ʱ� ���Ͽ� HANDLE_NULL�� �Ѵ�. */
	s_stLastMainSvcInfo.hPrevSvc = HANDLE_NULL;

/* 2. Current zapping info �ʱ�ȭ */
	for(i = 0; i < NUM_VIEW_ACTION; i++)
	{
		s_astZapInfo[i].hSvc = s_stLastMainSvcInfo.hCurSvc;
		s_astZapInfo[i].bSvcLockPassed = FALSE;
	}
}

void XMGR_ZAP_RegisterLastSvcInfo(Handle_t hSvc)
{
	HERROR				err;
	DbSvc_Info_t			stSvcInfo;
	DxSvcType_e			svcType;
	DxDeliveryType_e	deliveryType;
	HBOOL				bIsNeedToUpdateLastSvcInfo = FALSE;

	HxSTD_memset(&stSvcInfo, 0, sizeof(DbSvc_Info_t));
	err = DB_SVC_GetServiceInfo(hSvc, &stSvcInfo);
	if(err != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		return;
	}
	else
	{
		svcType = DbSvc_GetSvcType(&stSvcInfo);
		deliveryType = DbSvc_GetDeliType(stSvcInfo);
	}

	if (DbSvc_GetVolatileSvcFlag(&stSvcInfo))
	{	// �ֹ߼� Service�� �������� �ʴ´�.
		HxLOG_Print("\t=> Backup ignored ... Volatile Service(hSvc:0x%X)\n", (unsigned int)hSvc);
		return;
	}

	if( (hSvc != HANDLE_NULL) && (s_stLastMainSvcInfo.hCurSvc != hSvc) )
	{
		bIsNeedToUpdateLastSvcInfo = TRUE;
	}

	if(TRUE == bIsNeedToUpdateLastSvcInfo)
	{
/* ���� service�� ���� service�� ����ϰ�... */
		HxLOG_Print("\t=> Backup current service (0x%08X)\n", hSvc);
		s_stLastMainSvcInfo.hPrevSvc = s_stLastMainSvcInfo.hCurSvc;

/* �� service�� TV/Radio Item ���ε� ����Ѵ�. */
		switch(svcType)
		{
			case eDxSVC_TYPE_TV :
				HxLOG_Print("\t\t=> Save to TV service.\n");

#if	defined(CONFIG_APCORE_SAVE_LAST_TVRADIO_SVC)
				if(hSvc != s_stLastMainSvcInfo.hLastSvc_TV)
				{
					DB_PARAM_SetItem(eDB_PARAM_ITEM_LAST_TV_SVC, (HUINT32)&hSvc, sizeof(Handle_t));
					DB_PARAM_Sync();
				}
#endif

				s_stLastMainSvcInfo.hLastSvc_TV = hSvc;

				break;

			case eDxSVC_TYPE_RADIO :
				HxLOG_Print("\t\t=> Save to RADIO service.\n");

#if	defined(CONFIG_APCORE_SAVE_LAST_TVRADIO_SVC)
				if(hSvc != s_stLastMainSvcInfo.hLastSvc_RADIO)
				{
					DB_PARAM_SetItem(eDB_PARAM_ITEM_LAST_RADIO_SVC, (HUINT32)&hSvc, sizeof(Handle_t));
					DB_PARAM_Sync();
				}
#endif

				s_stLastMainSvcInfo.hLastSvc_RADIO = hSvc;

				break;

			default :
				HxLOG_Critical("\n\n");
				break;
		}

/* �� delivery type item ���ε� ����Ѵ�. */
		switch(deliveryType)
		{
			case eDxDELIVERY_TYPE_SAT :
				HxLOG_Print("\t\t=> Save to SAT & DTV service.\n");
				s_stLastMainSvcInfo.hLastSvc_SAT = hSvc;
				s_stLastMainSvcInfo.hLastSvc_DTV = hSvc;
				break;

			case eDxDELIVERY_TYPE_TER :
				HxLOG_Print("\t\t=> Save to TER & DTV service.\n");
				s_stLastMainSvcInfo.hLastSvc_TER = hSvc;
				s_stLastMainSvcInfo.hLastSvc_DTV = hSvc;
				break;

			case eDxDELIVERY_TYPE_CAB :
				HxLOG_Print("\t\t=> Save to CAB & DTV service.\n");
				s_stLastMainSvcInfo.hLastSvc_CAB = hSvc;
				s_stLastMainSvcInfo.hLastSvc_DTV = hSvc;
				break;

			default :
				HxLOG_Critical("\n\n");
				break;
		}

/* ���� ä�� ������ ����Ѵ�. */
		s_stLastMainSvcInfo.hCurSvc = hSvc;
		s_stLastMainSvcInfo.curSvcType = svcType;
		s_stLastMainSvcInfo.curDeliType = deliveryType;

		/* 4. ���� ä�� ������ DB���� ����Ѵ� */
		DB_PARAM_SetItem(eDB_PARAM_ITEM_CUR_SVC, (HUINT32)s_stLastMainSvcInfo.hCurSvc, 0); 		/* Service handle ��� */
		DB_PARAM_Sync();
	}
	else
	{
		HxLOG_Print("\t=> Backup ignored ... same ch#\n");
	}
}

HERROR XMGR_ZAP_GetCurrentSvcInfo(HUINT32 ulActionId, Handle_t *phCurSvc)
{
	if(DB_SVC_CheckValidServiceHandle(s_astZapInfo[ulActionId].hSvc) != ERR_OK)
	{
		return ERR_FAIL;
	}
	else
	{
		if(phCurSvc != NULL)
		{
			*phCurSvc = s_astZapInfo[ulActionId].hSvc;
		}
	}

	return ERR_OK;
}

HERROR XMGR_ZAP_SetCurrentSvcInfo(HUINT32 ulActionId, Handle_t hCurSvc)
{
	if((ulActionId < eActionId_VIEW_FIRST) || (ulActionId > eActionId_VIEW_LAST))
	{
		HxLOG_Error ("invalid ulActionId(%d) \n", ulActionId);
		return ERR_FAIL;
	}

	if(DB_SVC_CheckValidServiceHandle(hCurSvc) != ERR_OK)
	{
		HxLOG_Error ("invalid svcHandle(0x%x) \n", hCurSvc);
		return ERR_FAIL;
	}
	else if (s_astZapInfo[ulActionId].hSvc != hCurSvc)
	{
		HxSTD_MemSet(&(s_astZapInfo[ulActionId]), 0, sizeof(XmgrZapInfo_t));
		s_astZapInfo[ulActionId].hSvc = hCurSvc;
	}

	return ERR_OK;
}

HERROR XMGR_ZAP_GetPrevSvcHandle(Handle_t *phPrevSvc)
{
	if(DB_SVC_CheckValidServiceHandle(s_stLastMainSvcInfo.hPrevSvc) != ERR_OK)
	{
		return ERR_FAIL;
	}
	else
	{
		if(phPrevSvc != NULL)
		{
			*phPrevSvc = s_stLastMainSvcInfo.hPrevSvc;
		}
	}

	return ERR_OK;
}

HERROR XMGR_ZAP_GetLastSvcHandle(MgrLive_LastSvcItem_t eLastSvcItem, Handle_t *phLastSvc)
{
	if(phLastSvc == NULL)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	switch(eLastSvcItem)
	{
		case eLastSvcItem_TV:
			*phLastSvc = s_stLastMainSvcInfo.hLastSvc_TV;
			break;

		case eLastSvcItem_RADIO:
			*phLastSvc = s_stLastMainSvcInfo.hLastSvc_RADIO;
			break;

		case eLastSvcItem_SAT:
			*phLastSvc = s_stLastMainSvcInfo.hLastSvc_SAT;
			break;

		case eLastSvcItem_TER:
			*phLastSvc = s_stLastMainSvcInfo.hLastSvc_TER;
			break;

		case eLastSvcItem_CAB:
			*phLastSvc = s_stLastMainSvcInfo.hLastSvc_CAB;
			break;

		case eLastSvcItem_DTV:
			*phLastSvc = s_stLastMainSvcInfo.hLastSvc_DTV;
			break;

		case eLastSvcItem_ANA:
			*phLastSvc = s_stLastMainSvcInfo.hLastSvc_ANA;
			break;

		default:
			HxLOG_Critical("\n\n");
			*phLastSvc = HANDLE_NULL;
			return ERR_FAIL;
	}

	return ERR_OK;
}

HBOOL XMGR_ZAP_IsSvcLockPassed(HUINT32 ulActionId, Handle_t hSvc)
{
	if(s_astZapInfo[ulActionId].hSvc == hSvc)
	{
		HxLOG_Info("[AP:ZAP(%d)]  \033[35mAP_ZAP_IsServiceLockPassed(0x%x) == %d\n",ulActionId, hSvc, s_astZapInfo[ulActionId].bSvcLockPassed);
		return s_astZapInfo[ulActionId].bSvcLockPassed;
	}
	else
	{
		HxLOG_Info("[AP:ZAP(%d)] \033[35m\tAP_ZAP_IsServiceLockPassed(0x%x) ==> SVC NOT FOUND\n",ulActionId, hSvc);
		return FALSE;
	}
}

HERROR XMGR_ZAP_SetSvcLockPassed(HUINT32 ulActionId, Handle_t hSvc)
{
	if(s_astZapInfo[ulActionId].hSvc == hSvc)
	{
		HxLOG_Info("[AP:ZAP(%d)] \033[35mAP_ZAP_SetSvcLockPassed(0x%x) ==> OK\n",ulActionId, hSvc);
		s_astZapInfo[ulActionId].bSvcLockPassed = TRUE;

		return ERR_OK;
	}
	else
	{
		HxLOG_Info("[AP:ZAP(%d)] \033[35m\tAP_ZAP_IsServiceLockPassed(0x%x) ==> SVC NOT FOUND\n",ulActionId, hSvc);
		return ERR_FAIL;
	}
}

HERROR XMGR_ZAP_ResetSvcLockPassed(HUINT32 ulActionId)
{
	HxLOG_Info("[AP:ZAP(%d)] \033[35mAP_ZAP_ResetSvcLockPassed\n",ulActionId);
	if(ulActionId >= NUM_VIEW_ACTION)
	{
		HxLOG_Error("[AP:ZAP] \033[35mInvalid ActionId(%d)\n",ulActionId);
		return ERR_FAIL;
	}
	s_astZapInfo[ulActionId].bSvcLockPassed = FALSE;
	return ERR_OK;
}


HBOOL XMGR_ZAP_GetSkipPassword (Handle_t hSvc)
{
	return FALSE;
}

void XMGR_ZAP_SetSkipPassword (Handle_t hSvc, HBOOL bSkipFlag)
{
}

HBOOL XMGR_ZAP_GetPasswordOk (Handle_t hSvc)
{
	return FALSE;
}

void XMGR_ZAP_SetPasswordOk (Handle_t hSvc, HBOOL bPasswordOk)
{
}

HBOOL XMGR_ZAP_IsPasswordOk (Handle_t hSvc)
{
	return FALSE;
}

XmgrZapInfo_t *XMGR_ZAP_GetZapInfo (HUINT32 ulActionId)
{
	// view action id�� ���
	if (ulActionId >= NUM_VIEW_ACTION)
	{
		return NULL;
	}

	return &s_astZapInfo[ulActionId];
}


/* EOF */
