/**
	@file     	xmgr_clockrecover_base.c
	@brief

	Description:  			\n
	Module: 					 					\n
	Remarks : 										\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <hapi.h>
#include <db_svc.h>
#include <db_param.h>

#include <isosvc.h>

#include <bus.h>
#include <mgr_common.h>
#include <mgr_svclist.h>
#include <mgr_time.h>
#include <pmgr_time.h>
#include "../local_include/_xmgr_time.h"

#include <otl.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
#define MAX_REF_TP_LIST 30
#define MAX_DEFAULT_REF_TP_LIST 15
STATIC MgrTime_ClockRecoverRefTpInfo_t	s_stClockRefTp_Ukdtt[MAX_REF_TP_LIST];
#if defined(CONFIG_MW_CH_DVB_T2)

// T2 모델이기에 plp 값 들도 저장하기 위해 뒤에 7개의 인자를 더넣음.
STATIC MgrTime_ClockRecoverRefTpInfo_t	s_stClockDefaultTp_Ukdtt[] =
{ /* Clock time search will be done according to the order of the below TP info until the clock time is successfully set up.
	Modify the Antenna order and list according to the clock setup scenario */
	{eREF_WAKEUP_SVC, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{eREF_LAST_SVC, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{eREF_TER_TP, 0, 474000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{eREF_TER_TP, 0, 482000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{eREF_TER_TP, 0, 554000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{eREF_TER_TP, 0, 562000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{eREF_TER_TP, 0, 634000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{eREF_TER_TP, 0, 642000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{eREF_TER_TP, 0, 642000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{eREF_TER_TP, 0, 714000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{eREF_TER_TP, 0, 722000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{eREF_TER_TP, 0, 794000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{eREF_TER_TP, 0, 802000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{eREF_FIRST_SVC_TER, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{eREF_END, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} /* eREF_END will terminate the clock setup process */
};
#else
STATIC MgrTime_ClockRecoverRefTpInfo_t	s_stClockDefaultTp_Ukdtt[] =
{ /* Clock time search will be done according to the order of the below TP info until the clock time is successfully set up.
	Modify the Antenna order and list according to the clock setup scenario */
	{eREF_WAKEUP_SVC, 0, 0, 0, 0, 0, 0, 0, 0},
	{eREF_LAST_SVC, 0, 0, 0, 0, 0, 0, 0, 0},
	{eREF_TER_TP, 0, 474000, 0, 0, 0, 0, 0, 0, 0, 0},
	{eREF_TER_TP, 0, 482000, 0, 0, 0, 0, 0, 0, 0, 0},
	{eREF_TER_TP, 0, 554000, 0, 0, 0, 0, 0, 0, 0, 0},
	{eREF_TER_TP, 0, 562000, 0, 0, 0, 0, 0, 0, 0, 0},
	{eREF_TER_TP, 0, 634000, 0, 0, 0, 0, 0, 0, 0, 0},
	{eREF_TER_TP, 0, 642000, 0, 0, 0, 0, 0, 0, 0, 0},
	{eREF_TER_TP, 0, 642000, 0, 0, 0, 0, 0, 0, 0, 0},
	{eREF_TER_TP, 0, 714000, 0, 0, 0, 0, 0, 0, 0, 0},
	{eREF_TER_TP, 0, 722000, 0, 0, 0, 0, 0, 0, 0, 0},
	{eREF_TER_TP, 0, 794000, 0, 0, 0, 0, 0, 0, 0, 0},
	{eREF_TER_TP, 0, 802000, 0, 0, 0, 0, 0, 0, 0, 0},
	{eREF_FIRST_SVC_TER, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{eREF_END, 0, 0, 0, 0, 0, 0, 0, 0,0,0} /* eREF_END will terminate the clock setup process */
};

#endif
#define	XsvcSi_UKDTT_MAX_NCD_MESSAGE_LEN				(512)		/* 실제 message 와 date 정보를 포함하기에 512로 늘리자 */
#define	XsvcSi_UKDTT_MAX_NCD_MESSAGE_NUM				(4)			/* Suite 참조하여 1로 만들자 */

typedef struct
{
	HUINT8	iso639[4];
	HUINT8	ucText[XsvcSi_UKDTT_MAX_NCD_MESSAGE_LEN];
} XsvcSi_NcdInfoMsg_UkDtt_t;

typedef struct
{
	HUINT8			ucChangeId;
	HUINT8			ucChangeVer;
	HUINT32			ulStartTime;
	HUINT32			ulEndTime;
	HBOOL			bMajor;

	XsvcSi_NcdInfoMsg_UkDtt_t	stMessage[XsvcSi_UKDTT_MAX_NCD_MESSAGE_NUM];

	HUINT8			ucMsgId;
	HUINT16			usDemodCellId;
	HUINT8			ucChangeType;

	HUINT8			ucInvariantTsPresent;
	HUINT16			usInvariantTsId;
	HUINT16			usInvariantOnId;
} XsvcSi_NcdInfo_UkDtt_t;


/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

HERROR xmgr_time_ClockRecoverSearchByTerTpList_Ukdtt(void)
{

	HERROR				err;
	DbSvc_TsCondition_t stTsCond;
	HINT32				nCount,i = 0,j=0;
	DbSvc_TsInfo_t		*pstTsInfoList = NULL;
	XsvcSi_NcdInfo_UkDtt_t stPrevNcdInfo;
	DbSvc_TsInfo_t	stTsInfo;
	HUINT32			temp;
	HBOOL			bSame = FALSE;


	// TS Info가 OnId로 등록되어 있는지 확인한다.
	VK_memset(&stTsInfo, 0, sizeof(DbSvc_TsInfo_t));
	DB_SVC_InitTsFindCondition (&stTsCond);
	stTsCond.nDeliType	= eDxDELIVERY_TYPE_TER;
	HxSTD_memset(&stPrevNcdInfo, 0, sizeof(XsvcSi_NcdInfo_UkDtt_t));
	err= DB_PARAM_GetItem(eDB_PARAM_ITEM_ASO_NETWORK_CHANGE_INFO, (HUINT32 *)&stPrevNcdInfo, sizeof(XsvcSi_NcdInfo_UkDtt_t));
	if(err == ERR_OK)
	{
		if(stPrevNcdInfo.ulStartTime == 0 || stPrevNcdInfo.ulEndTime == 0)
		{

			VK_Print("[Clock Setup] GeteDB_PARAM_ITEM_ASO_NETWORK_CHANGE_INFO is None\n");
			return ERR_FAIL;
		}
		stTsCond.nOnId	= stPrevNcdInfo.usInvariantOnId;
		stTsCond.nTsId	= stPrevNcdInfo.usInvariantTsId;

	}
	else
	{
		VK_Print("[Clock Setup] GeteDB_PARAM_ITEM_ASO_NETWORK_CHANGE_INFO is FAILed\n");
		return ERR_FAIL;
	}
	VK_Print("[Clock Setup] local_clocksetup_SearchByTerTp(stTsCond.nOnId: %x)\n", stTsCond.nOnId);

	if((stTsCond.nOnId == -1 && stTsCond.nTsId == -1) || (stTsCond.nOnId == 0 && stTsCond.nTsId == 0))
	{
		stTsCond.nTsId = -1;
		stTsCond.nOnId = 0x233A;
		err = DB_SVC_FindTsInfoList(&stTsCond, &nCount, &pstTsInfoList);
		VK_Print("[Clock Setup] local_clocksetup_SearchByTerTp(nCount: %d)\n", nCount);
		if (err == ERR_OK && pstTsInfoList)

		{
			for (i =0 ; i<nCount ;i++)
			{
				HxSTD_MemSet(&s_stClockRefTp_Ukdtt[i],0x00,sizeof(MgrTime_ClockRecoverRefTpInfo_t));
				s_stClockRefTp_Ukdtt[i].eRefMethod = eREF_TER_TP;
				s_stClockRefTp_Ukdtt[i].ulTer_Frequency = pstTsInfoList[i].stTuningParam.ter.ulFreq;
				s_stClockRefTp_Ukdtt[i].eBandWidth = pstTsInfoList[i].stTuningParam.ter.ucBandwidth;
				s_stClockRefTp_Ukdtt[i].eTer_constellation = pstTsInfoList[i].stTuningParam.ter.ucConstellation;
				s_stClockRefTp_Ukdtt[i].eTransMode = pstTsInfoList[i].stTuningParam.ter.ucTransMode;
				s_stClockRefTp_Ukdtt[i].eStream = pstTsInfoList[i].stTuningParam.ter.ucStream;
				s_stClockRefTp_Ukdtt[i].eSystem = pstTsInfoList[i].stTuningParam.ter.ucSystem;
#if defined(CONFIG_MW_CH_DVB_T2)
				s_stClockRefTp_Ukdtt[i].ulPlpId = pstTsInfoList[i].stTuningParam.ter.stT2param.ulPlpId;
				s_stClockRefTp_Ukdtt[i].ucPreambleFormat= pstTsInfoList[i].stTuningParam.ter.stT2param.ucPreambleFormat;
				s_stClockRefTp_Ukdtt[i].ucMixed = pstTsInfoList[i].stTuningParam.ter.stT2param.ucMixed;
				s_stClockRefTp_Ukdtt[i].ucPilotPattern = pstTsInfoList[i].stTuningParam.ter.stT2param.ucPilotPattern;
				s_stClockRefTp_Ukdtt[i].ucExtenedCarrier = pstTsInfoList[i].stTuningParam.ter.stT2param.ucExtenedCarrier;
				s_stClockRefTp_Ukdtt[i].ucPAPRreduction = pstTsInfoList[i].stTuningParam.ter.stT2param.ucPAPRreduction;
				s_stClockRefTp_Ukdtt[i].ulNumPlpBlocks = pstTsInfoList[i].stTuningParam.ter.stT2param.ulNumPlpBlocks;
#endif
			}

			DB_SVC_FreeTsInfoList(pstTsInfoList);

		}

	}
	else
	{
		err = DB_SVC_FindTsInfoList(&stTsCond, &nCount, &pstTsInfoList);
		if (err == ERR_OK && pstTsInfoList)
		{ /* More than one TP set up. */
			VK_Print("[Clock Setup] 111local_clocksetup_SearchByTerTp(stTsCond.nOnId: %x nCount:%d)\n", stTsCond.nOnId,nCount);
			for (i =0 ; i<nCount ;i++)
			{
				HxSTD_MemSet(&s_stClockRefTp_Ukdtt[i],0x00,sizeof(MgrTime_ClockRecoverRefTpInfo_t));
				s_stClockRefTp_Ukdtt[i].eRefMethod = eREF_TER_TP;
				s_stClockRefTp_Ukdtt[i].ulTer_Frequency = pstTsInfoList[i].stTuningParam.ter.ulFreq;
				s_stClockRefTp_Ukdtt[i].eBandWidth = pstTsInfoList[i].stTuningParam.ter.ucBandwidth;
				s_stClockRefTp_Ukdtt[i].eTer_constellation = pstTsInfoList[i].stTuningParam.ter.ucConstellation;
				s_stClockRefTp_Ukdtt[i].eTransMode = pstTsInfoList[i].stTuningParam.ter.ucTransMode;
				s_stClockRefTp_Ukdtt[i].eStream = pstTsInfoList[i].stTuningParam.ter.ucStream;
				s_stClockRefTp_Ukdtt[i].eSystem = pstTsInfoList[i].stTuningParam.ter.ucSystem;
#if defined(CONFIG_MW_CH_DVB_T2)
				s_stClockRefTp_Ukdtt[i].ulPlpId = pstTsInfoList[i].stTuningParam.ter.stT2param.ulPlpId;
				s_stClockRefTp_Ukdtt[i].ucPreambleFormat= pstTsInfoList[i].stTuningParam.ter.stT2param.ucPreambleFormat;
				s_stClockRefTp_Ukdtt[i].ucMixed = pstTsInfoList[i].stTuningParam.ter.stT2param.ucMixed;
				s_stClockRefTp_Ukdtt[i].ucPilotPattern = pstTsInfoList[i].stTuningParam.ter.stT2param.ucPilotPattern;
				s_stClockRefTp_Ukdtt[i].ucExtenedCarrier = pstTsInfoList[i].stTuningParam.ter.stT2param.ucExtenedCarrier;
				s_stClockRefTp_Ukdtt[i].ucPAPRreduction = pstTsInfoList[i].stTuningParam.ter.stT2param.ucPAPRreduction;
				s_stClockRefTp_Ukdtt[i].ulNumPlpBlocks = pstTsInfoList[i].stTuningParam.ter.stT2param.ulNumPlpBlocks;
#endif

			}
			DB_SVC_FreeTsInfoList(pstTsInfoList);
		}
		else
		{
			stTsCond.nTsId = -1;
			err = DB_SVC_FindTsInfoList(&stTsCond, &nCount, &pstTsInfoList);
			if (err == ERR_OK && pstTsInfoList)
			{ /* More than one TP set up. */
				VK_Print("[Clock Setup] 111local_clocksetup_SearchByTerTp(stTsCond.nOnId: %x nCount:%d)\n", stTsCond.nOnId,nCount);
				for (i =0 ; i<nCount ;i++)
				{
					HxSTD_MemSet(&s_stClockRefTp_Ukdtt[i],0x00,sizeof(MgrTime_ClockRecoverRefTpInfo_t));
					s_stClockRefTp_Ukdtt[i].eRefMethod = eREF_TER_TP;
					s_stClockRefTp_Ukdtt[i].ulTer_Frequency = pstTsInfoList[i].stTuningParam.ter.ulFreq;
					s_stClockRefTp_Ukdtt[i].eBandWidth = pstTsInfoList[i].stTuningParam.ter.ucBandwidth;
					s_stClockRefTp_Ukdtt[i].eTer_constellation = pstTsInfoList[i].stTuningParam.ter.ucConstellation;
					s_stClockRefTp_Ukdtt[i].eTransMode = pstTsInfoList[i].stTuningParam.ter.ucTransMode;
					s_stClockRefTp_Ukdtt[i].eStream = pstTsInfoList[i].stTuningParam.ter.ucStream;
					s_stClockRefTp_Ukdtt[i].eSystem = pstTsInfoList[i].stTuningParam.ter.ucSystem;
#if defined(CONFIG_MW_CH_DVB_T2)
					s_stClockRefTp_Ukdtt[i].ulPlpId = pstTsInfoList[i].stTuningParam.ter.stT2param.ulPlpId;
					s_stClockRefTp_Ukdtt[i].ucPreambleFormat= pstTsInfoList[i].stTuningParam.ter.stT2param.ucPreambleFormat;
					s_stClockRefTp_Ukdtt[i].ucMixed = pstTsInfoList[i].stTuningParam.ter.stT2param.ucMixed;
					s_stClockRefTp_Ukdtt[i].ucPilotPattern = pstTsInfoList[i].stTuningParam.ter.stT2param.ucPilotPattern;
					s_stClockRefTp_Ukdtt[i].ucExtenedCarrier = pstTsInfoList[i].stTuningParam.ter.stT2param.ucExtenedCarrier;
					s_stClockRefTp_Ukdtt[i].ucPAPRreduction = pstTsInfoList[i].stTuningParam.ter.stT2param.ucPAPRreduction;
					s_stClockRefTp_Ukdtt[i].ulNumPlpBlocks = pstTsInfoList[i].stTuningParam.ter.stT2param.ulNumPlpBlocks;
#endif

				}
				DB_SVC_FreeTsInfoList(pstTsInfoList);
			}
			else
			{
				stTsCond.nTsId = -1;
				stTsCond.nOnId = 0x233A;
				err = DB_SVC_FindTsInfoList(&stTsCond, &nCount, &pstTsInfoList);
				if (err == ERR_OK && pstTsInfoList)
				{ /* More than one TP set up. */
					VK_Print("[Clock Setup] 111local_clocksetup_SearchByTerTp(stTsCond.nOnId: %x nCount:%d)\n", stTsCond.nOnId,nCount);
					for (i =0 ; i<nCount ;i++)
					{
						HxSTD_MemSet(&s_stClockRefTp_Ukdtt[i],0x00,sizeof(MgrTime_ClockRecoverRefTpInfo_t));
						s_stClockRefTp_Ukdtt[i].eRefMethod = eREF_TER_TP;
						s_stClockRefTp_Ukdtt[i].ulTer_Frequency = pstTsInfoList[i].stTuningParam.ter.ulFreq;
						s_stClockRefTp_Ukdtt[i].eBandWidth = pstTsInfoList[i].stTuningParam.ter.ucBandwidth;
						s_stClockRefTp_Ukdtt[i].eTer_constellation = pstTsInfoList[i].stTuningParam.ter.ucConstellation;
						s_stClockRefTp_Ukdtt[i].eTransMode = pstTsInfoList[i].stTuningParam.ter.ucTransMode;
						s_stClockRefTp_Ukdtt[i].eStream = pstTsInfoList[i].stTuningParam.ter.ucStream;
						s_stClockRefTp_Ukdtt[i].eSystem = pstTsInfoList[i].stTuningParam.ter.ucSystem;
#if defined(CONFIG_MW_CH_DVB_T2)
						s_stClockRefTp_Ukdtt[i].ulPlpId = pstTsInfoList[i].stTuningParam.ter.stT2param.ulPlpId;
						s_stClockRefTp_Ukdtt[i].ucPreambleFormat= pstTsInfoList[i].stTuningParam.ter.stT2param.ucPreambleFormat;
						s_stClockRefTp_Ukdtt[i].ucMixed = pstTsInfoList[i].stTuningParam.ter.stT2param.ucMixed;
						s_stClockRefTp_Ukdtt[i].ucPilotPattern = pstTsInfoList[i].stTuningParam.ter.stT2param.ucPilotPattern;
						s_stClockRefTp_Ukdtt[i].ucExtenedCarrier = pstTsInfoList[i].stTuningParam.ter.stT2param.ucExtenedCarrier;
						s_stClockRefTp_Ukdtt[i].ucPAPRreduction = pstTsInfoList[i].stTuningParam.ter.stT2param.ucPAPRreduction;
						s_stClockRefTp_Ukdtt[i].ulNumPlpBlocks = pstTsInfoList[i].stTuningParam.ter.stT2param.ulNumPlpBlocks;
#endif


					}

					DB_SVC_FreeTsInfoList(pstTsInfoList);
				}
			}

		}

	}


	for(j = 0;j< MAX_DEFAULT_REF_TP_LIST;j++)
	{
		bSame = FALSE;
		if(i >= MAX_REF_TP_LIST) // Max List 개수 넘으면
			break;
		for(temp = 0;temp<nCount;temp++)
		{
			if(s_stClockRefTp_Ukdtt[temp].ulTer_Frequency == s_stClockDefaultTp_Ukdtt[j].ulTer_Frequency)
			{
				bSame = TRUE;
				break;
			}
	}
		if(bSame == TRUE)
		{
			continue;
		}
		HxSTD_MemCopy(&s_stClockRefTp_Ukdtt[i++],&s_stClockDefaultTp_Ukdtt[j],sizeof(MgrTime_ClockRecoverRefTpInfo_t));

	}
#if 0//debug
	for( temp = 0;temp < MAX_REF_TP_LIST;temp++)
	{
		HxLOG_Print("s_stClockRefTp_Ukdtt frequ: %d\n",s_stClockRefTp_Ukdtt[temp].ulTer_Frequency);
	}
#endif
	return ERR_OK;
}

MgrTime_ClockRecoverRefTpInfo_t *xmgr_time_ClockRecoverGetRefTpList_Ukdtt (void)
{
	HERROR hErr = ERR_FAIL;
	hErr = xmgr_time_ClockRecoverSearchByTerTpList_Ukdtt();
	if (hErr != ERR_OK)
	{
		return xmgr_time_ClockRecoverGetRefTpList_Base();
	}
	return s_stClockRefTp_Ukdtt;
}

HERROR xmgr_time_ClockRecoverSearchByTerTp_Ukdtt(HUINT32 ulRefTpIdx, DxTuneParam_t	*tuningInfo)
{

	MgrTime_ClockRecoverRefTpInfo_t	*astTpList = NULL;

	astTpList = xmgr_time_ClockRecoverGetRefTpList_Ukdtt();
	if (NULL == astTpList)
	{
		return ERR_FAIL;
	}

	tuningInfo->ter.deliveryType	= eDxDELIVERY_TYPE_TER;
	tuningInfo->ter.ulTunerGroupId	= -1; /* 지상파는 DB parameter에서는 tuner ID를 지정하지 않는다. */
	tuningInfo->ter.ulFrequency 	= astTpList[ulRefTpIdx].ulTer_Frequency;
	tuningInfo->ter.eBandWidth		= astTpList[ulRefTpIdx].eBandWidth;
	tuningInfo->ter.eConstellation	= astTpList[ulRefTpIdx].eTer_constellation;
	tuningInfo->ter.eHierachy		= eDxTER_HIERACHY_AUTO;
	tuningInfo->ter.eStream 		= astTpList[ulRefTpIdx].eStream;
	tuningInfo->ter.eCodeRate		= eDxTER_CODERATE_AUTO;
	tuningInfo->ter.eGuardInterval	= eDxTER_GUARDINTERVAL_AUTO;
	tuningInfo->ter.eTransMode		= astTpList[ulRefTpIdx].eTransMode;
	tuningInfo->ter.eOffset 		= eDxTER_OFFSET_AUTO;
	tuningInfo->ter.eSystem 				= astTpList[ulRefTpIdx].eSystem;
#if defined(CONFIG_MW_CH_DVB_T2)
	tuningInfo->ter.stT2Param.ulPlpId =  astTpList[ulRefTpIdx].ulPlpId;
	tuningInfo->ter.stT2Param.preambleFormat	=  astTpList[ulRefTpIdx].ucPreambleFormat;
	tuningInfo->ter.stT2Param.mixed 		=  astTpList[ulRefTpIdx].ucMixed;
	tuningInfo->ter.stT2Param.pilotPattern	=  astTpList[ulRefTpIdx].ucPilotPattern;
	tuningInfo->ter.stT2Param.extenedCarrier	=  astTpList[ulRefTpIdx].ucExtenedCarrier;
	tuningInfo->ter.stT2Param.PAPRreduction =  astTpList[ulRefTpIdx].ucPAPRreduction;
	tuningInfo->ter.stT2Param.numPlpBlocks	=  astTpList[ulRefTpIdx].ulNumPlpBlocks;
#endif
	return ERR_OK;
}



/*********************** End of File ******************************/
