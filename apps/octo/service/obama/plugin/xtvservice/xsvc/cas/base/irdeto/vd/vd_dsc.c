
#include "octo_common.h"

#include <pal_sc.h>

#include <_svc_cas_sub_dev_api.h>

#include "vd_humax.h"

#include "s3_vddd.h"
#include "pal_sys.h"
#include "pal_dsc.h"






typedef struct DSC_RefId_t
{
	HBOOL bUsed;
	HUINT32	ulRefId;

}DSC_RefId;

// ewlee 20100825 - PID list for each reference ID
#if defined(CONFIG_WA_2SC_SAME_ENTITLEMENT)
#define	HMX_VD_REF_PID_MAX		10	// 10 is temporary value
typedef struct RefId_PIDS_t
{
	HUINT32		ulSCellRefId;
	HUINT8		ucRemovePID[HMX_VD_REF_PID_MAX];
	HUINT32		ulHmxRefId[HMX_VD_REF_PID_MAX];
	HUINT16		usPID[HMX_VD_REF_PID_MAX];
#if defined(WA_2SC_SAME_ENTITLEMENT_GRAP_KEY_DATA)
	HBOOL		bKeySetEven[HMX_VD_REF_PID_MAX];
	HBOOL		bKeySetOdd[HMX_VD_REF_PID_MAX];
	vd_dd_key_st	ddkeyst;
	HUINT8		*p;
#endif
}RefId_PIDS;
// ewlee 20100830 - for PVR
typedef struct Demux_Info_t
{
	vd_rsc_id	DmxId;	// usDmxId is same as resource id.
	RefId_PIDS	RefIdPID_List[HMX_VD_DSC_REFID_MAX];
}Demux_Info;
//static RefId_PIDS	RefId_HMXInfoList[HMX_VD_DSC_REFID_MAX];
static Demux_Info	DemuxInfo[HMX_VD_DMX_MAX_NUM];
#endif

#if defined(IRPLAT_SYS_LIMITED_DESC_SLOT)
typedef struct DSC_RefCount_t
{
	HUINT32 			ulCount;
	HUINT32			ulPalDscIdx;

}DSC_RefCount;
#endif

static DSC_RefId	DSC_RefIdList[HMX_VD_DSC_REFID_MAX];
static HVD_DSC_RSC s_stHVD_DSC_Info[HMX_VD_DMX_MAX_NUM];

HULONG	s_ulDESCAPISem;

static HUINT32 s_EnabledCWMode = 0;
static HUINT8 *pCSSK = NULL;

#define DESC_SEM_GET()			VK_SEM_Get(s_ulDESCAPISem)
#define DESC_SEM_RELEASE()		VK_SEM_Release(s_ulDESCAPISem)

#if defined(IRPLAT_SYS_LIMITED_DESC_SLOT)
#define MAX_DESC_SLOT	5
static DSC_RefCount s_DescRefCount[HMX_VD_DMX_MAX_NUM];
#endif

//#define PRINT_CW

static vd_ref_id	local_VdDsc_GetAvailableRefID(void);
static ia_result	local_VdDsc_SetPID(vd_ref_id wRefId, vd_dd_pid_st DdPid);
static void local_VdDsc_ClearReferenceID(vd_ref_id ReferenceID);
static void local_VdDsc_ClearRefInfo(HVD_DSC_RSC_INFO *pDscRefInfo);
static HERROR local_VdDsc_SetKEY(vd_ref_id wRefId, vd_dd_key_st DdKey, HUINT8 *p);

static vd_ref_id local_VdDsc_GetAvailableRefID(void)
{
	HUINT32	idx;

	for( idx = 0; idx < HMX_VD_DSC_REFID_MAX; idx++ )
	{
		if( DSC_RefIdList[idx].bUsed == FALSE )
		{
			DSC_RefIdList[idx].ulRefId = idx;
			return (vd_ref_id) idx;
		}
	}
	HxLOG_Print("ERROR : no more available reference ID\n" );
	return VD_REF_ID_ERROR;
}


static void local_VdDsc_ClearRefInfo(HVD_DSC_RSC_INFO *pDscRefInfo)
{
	pDscRefInfo->ucDemuxId = HMX_VD_DMX_MAX_NUM;
	pDscRefInfo->ReferenceID = VD_REF_ID_ERROR;
	pDscRefInfo->ulPalDscIdx = HMX_DSC_INVALID_PAL_IDX;
	pDscRefInfo->usPid = HMX_INVALID_PID;
	pDscRefInfo->bNeedToClose = FALSE;
	pDscRefInfo->usUsedCount = 0;	// ewlee 20100825

	VK_MEM_Memset(pDscRefInfo->EvenKey, 0xFF, sizeof(pDscRefInfo->EvenKey) );
	VK_MEM_Memset(pDscRefInfo->OddKey, 0xFF, sizeof(pDscRefInfo->OddKey) );
}

static void local_VdDsc_ClearReferenceID(vd_ref_id ReferenceID)
{
	if( (ReferenceID >= 0) && (ReferenceID < HMX_VD_DSC_REFID_MAX) )
	{
		DSC_RefIdList[ReferenceID].bUsed= FALSE;
		DSC_RefIdList[ReferenceID].ulRefId= VD_REF_ID_ERROR;
	}

	return;
}

// ewlee 20100825
#if defined(CONFIG_WA_2SC_SAME_ENTITLEMENT)
// ewlee 20100830 - start

#if defined(CONFIG_DEBUG)
static void local_VdDsc_PrintDemuxInfo(void)
{
	HUINT32		i, j, k;
	HxLOG_Debug("Demux Info \n");
	for (i = 0; i < HMX_VD_DMX_MAX_NUM; i++)
	{
		if(DemuxInfo[i].DmxId != VD_RSC_ID_INVALID)
		{
			HxLOG_Debug("DemuxInfo[%d].DmxId: 0x%x \n", i, DemuxInfo[i].DmxId);
			for (j = 0; j < HMX_VD_DSC_REFID_MAX; j++)
			{
				if(DemuxInfo[i].RefIdPID_List[j].ulSCellRefId != VD_REF_ID_ERROR)
				{
					HxLOG_Debug("DemuxInfo[%d].RefIdPID_List[%d].ulSCellRefId: 0x%x \n", i, j, DemuxInfo[i].RefIdPID_List[j].ulSCellRefId);


					for (k = 0; k < HMX_VD_REF_PID_MAX; k++)
					{
#if defined(WA_2SC_SAME_ENTITLEMENT_GRAP_KEY_DATA)
						HxLOG_Debug("\t, [%d] ucRemovePID: 0x%x, ulHmxRefId: 0x%x, usPID: 0x%x, bKeySetEven: %d, bKeySetOdd: %d \n", k,  DemuxInfo[i].RefIdPID_List[j].ucRemovePID[k], DemuxInfo[i].RefIdPID_List[j].ulHmxRefId[k], DemuxInfo[i].RefIdPID_List[j].usPID[k], DemuxInfo[i].RefIdPID_List[j].bKeySetEven[k], DemuxInfo[i].RefIdPID_List[j].bKeySetOdd[k]);
#else
						HxLOG_Debug("\t, [%d] ucRemovePID: 0x%x, ulHmxRefId: 0x%x, usPID: 0x%x \n", k,  DemuxInfo[i].RefIdPID_List[j].ucRemovePID[k], DemuxInfo[i].RefIdPID_List[j].ulHmxRefId[k], DemuxInfo[i].RefIdPID_List[j].usPID[k]);
#endif
					}

				}
			}
		}
	}

}
#endif

static void local_VdDsc_InitDemuxInfo(void)
{
	HUINT32		i, j, k;

	for (i = 0; i < HMX_VD_DMX_MAX_NUM; i++)
	{
		DemuxInfo[i].DmxId = VD_RSC_ID_INVALID;

		for (j = 0; j < HMX_VD_DSC_REFID_MAX; j++)
		{
			DemuxInfo[i].RefIdPID_List[j].ulSCellRefId = VD_REF_ID_ERROR;
#if defined(WA_2SC_SAME_ENTITLEMENT_GRAP_KEY_DATA)
			HxSTD_MemSet(&(DemuxInfo[i].RefIdPID_List[j].ddkeyst), 0x00, sizeof(vd_dd_key_st));
			if(DemuxInfo[i].RefIdPID_List[j].p != NULL)
			{
				HLIB_STD_MemFree(DemuxInfo[i].RefIdPID_List[j].p);
				DemuxInfo[i].RefIdPID_List[j].p = NULL;
			}
#endif
			for (k = 0; k < HMX_VD_REF_PID_MAX; k++)
			{
				DemuxInfo[i].RefIdPID_List[j].ucRemovePID[k] = 0;
				DemuxInfo[i].RefIdPID_List[j].ulHmxRefId[k] = VD_REF_ID_ERROR;
				DemuxInfo[i].RefIdPID_List[j].usPID[k] = HMX_INVALID_PID;
#if defined(WA_2SC_SAME_ENTITLEMENT_GRAP_KEY_DATA)
				DemuxInfo[i].RefIdPID_List[j].bKeySetEven[k] = FALSE;
				DemuxInfo[i].RefIdPID_List[j].bKeySetOdd[k] = FALSE;
#endif


			}
		}
	}
}

RefId_PIDS* local_VdDsc_GetRefPIDInfo(vd_ref_id ReferenceID)
{
	HUINT32		i, j;

	for (i = 0; i < HMX_VD_DMX_MAX_NUM; i++)
	{
		if (DemuxInfo[i].DmxId == VD_RSC_ID_INVALID)
			continue;

		for (j = 0; j < HMX_VD_DSC_REFID_MAX; j++)
		{
			if (DemuxInfo[i].RefIdPID_List[j].ulSCellRefId == ReferenceID)
				return &(DemuxInfo[i].RefIdPID_List[j]);
		}
	}

	return NULL;
}

vd_rsc_id local_VdDsc_GetDemuxIdOfScRefId(vd_ref_id ReferenceID)
{
	HUINT32		i, j;

	for (i = 0; i < HMX_VD_DMX_MAX_NUM; i++)
	{
		if (DemuxInfo[i].DmxId == VD_RSC_ID_INVALID)
			continue;

		for (j = 0; j < HMX_VD_DSC_REFID_MAX; j++)
		{
			if (DemuxInfo[i].RefIdPID_List[j].ulSCellRefId == ReferenceID)
				return DemuxInfo[i].DmxId;
		}
	}

	return VD_RSC_ID_INVALID;
}

vd_rsc_id local_VdDsc_GetDemuxIdOfHmxRefId(vd_ref_id ReferenceID)
{
	HUINT32		i, j, k;

	for (i = 0; i < HMX_VD_DMX_MAX_NUM; i++)
	{
		if (DemuxInfo[i].DmxId == VD_RSC_ID_INVALID)
			continue;

		for (j = 0; j < HMX_VD_DSC_REFID_MAX; j++)
		{
			if (DemuxInfo[i].RefIdPID_List[j].ulSCellRefId == VD_REF_ID_ERROR)
				continue;

			for (k = 0; k < HMX_VD_REF_PID_MAX; k++)
			{
				if (DemuxInfo[i].RefIdPID_List[j].ulHmxRefId[k] == ReferenceID)
					return DemuxInfo[i].DmxId;
			}
		}
	}

	return VD_RSC_ID_INVALID;
}

static void local_VdDsc_SetDemuxRefId(vd_rsc_id wResourceID, vd_ref_id ReferenceID)
{
	HUINT32		i;

	if( wResourceID >= HMX_VD_DMX_MAX_NUM )
		return;

	DemuxInfo[wResourceID].DmxId = wResourceID;

	for (i = 0; i < HMX_VD_DSC_REFID_MAX; i++)
	{
		if (DemuxInfo[wResourceID].RefIdPID_List[i].ulSCellRefId == ReferenceID)
			break;

		if (DemuxInfo[wResourceID].RefIdPID_List[i].ulSCellRefId != VD_REF_ID_ERROR)
			continue;

		DemuxInfo[wResourceID].RefIdPID_List[i].ulSCellRefId = ReferenceID;
		break;
	}
}
// ewlee 20100830 - end

static void local_VdDsc_ClearSecureReferenceID(vd_ref_id ReferenceID)
{
	HUINT32		i;
	RefId_PIDS	*pRefPIDInfo = NULL;

	pRefPIDInfo = local_VdDsc_GetRefPIDInfo(ReferenceID);

	if (pRefPIDInfo == NULL)
		return;

	pRefPIDInfo->ulSCellRefId = VD_REF_ID_ERROR;

	for (i = 0; i < HMX_VD_REF_PID_MAX; i++)
	{
		pRefPIDInfo->ucRemovePID[i] = 0;
		pRefPIDInfo->ulHmxRefId[i] = VD_REF_ID_ERROR;
		pRefPIDInfo->usPID[i] = HMX_INVALID_PID;
#if defined(WA_2SC_SAME_ENTITLEMENT_GRAP_KEY_DATA)
		pRefPIDInfo->bKeySetEven[i] = FALSE;
		pRefPIDInfo->bKeySetOdd[i] = FALSE;
#endif
	}

#if defined(WA_2SC_SAME_ENTITLEMENT_GRAP_KEY_DATA)
	HxSTD_MemSet(&(pRefPIDInfo->ddkeyst), 0x00, sizeof(vd_dd_key_st));
	if(pRefPIDInfo->p != NULL)
	{
		HLIB_STD_MemFree(pRefPIDInfo->p);
		pRefPIDInfo->p = NULL;
	}
#endif

	return;
}

static void local_VdDsc_SetHMXInfo(vd_ref_id wSCRefId, HUINT32 ulHmxRefId, HUINT16 usSCPID)
{
	HUINT32		i;
	HUINT32		SCRefIdx = 0;
	RefId_PIDS	*pRefPIDInfo = NULL;

	pRefPIDInfo = local_VdDsc_GetRefPIDInfo(wSCRefId);

	if (pRefPIDInfo == NULL)
		return;

	for (i = 0; i < HMX_VD_REF_PID_MAX; i++)
	{
		if (pRefPIDInfo->usPID[i] == usSCPID)	// ewlee 20100826 - it already has the same PID
		{
			pRefPIDInfo->ucRemovePID[i] = 0;
			break;
		}

		if (pRefPIDInfo->ulHmxRefId[i] != VD_REF_ID_ERROR)
			continue;

		pRefPIDInfo->ulHmxRefId[i] = ulHmxRefId;
		pRefPIDInfo->usPID[i] = usSCPID;
		pRefPIDInfo->ucRemovePID[i] = 0;
#if defined(WA_2SC_SAME_ENTITLEMENT_GRAP_KEY_DATA)
		pRefPIDInfo->bKeySetEven[i] = FALSE;
		pRefPIDInfo->bKeySetOdd[i] = FALSE;
#endif
		HxLOG_Info("[%d - %d] Set HUMAX Ref ID:%d\n", SCRefIdx, i, pRefPIDInfo->ulHmxRefId[i]);
                HxLOG_Info("[ - %d] Set HUMAX Ref ID:%d\n", i, pRefPIDInfo->ulHmxRefId[i]);
		break;
	}
}

// ewlee 20100826 - This API does not have locking routine. Use carefully.
vd_ref_id local_VdDsc_Open( vd_rsc_id wResourceID, void *pvData )
{
	vd_ref_id	ReferenceID;
	HUINT32		ulPalDscIdx;
	HUINT32		i;

	HxLOG_Info("\n\t(+)local_VdDsc_Open wRefId(0x%X)\n\n", (unsigned int)wResourceID);

	if( wResourceID >= HMX_VD_DMX_MAX_NUM )
	{
		HxLOG_Print("Invalid wResourceID = %d\n", wResourceID);
		ReferenceID = VD_REF_ID_ERROR;
	}
	else
	{
		s_stHVD_DSC_Info[wResourceID].ResourceID = wResourceID;

		/* platform의 모든 descrambler slot 에 대한 unique 한 reference ID 를 만들어 낸다. */
		ReferenceID = local_VdDsc_GetAvailableRefID();

		HxLOG_Info("Ref ID = 0x%X\n", (unsigned int) ReferenceID);

		if( ReferenceID == VD_REF_ID_ERROR )
		{
			HxLOG_Print(" No more valid referenceID\n");
			ReferenceID = VD_REF_ID_ERROR;
		}
		else
		{
			/* link ref id list & vd dsc info table */
			for( i = 0; i < HMX_VD_DSC_SLOT_MAX_IN_ONE_DEMUX; i++ )
			{
				if( s_stHVD_DSC_Info[wResourceID].RefInfo[i].ReferenceID == VD_REF_ID_ERROR )
					break;
			}

			if( i == HMX_VD_DSC_SLOT_MAX_IN_ONE_DEMUX )
			{
				HxLOG_Print("RefInfo was full\n");
				ReferenceID = VD_REF_ID_ERROR;
			}
			else
			{
				/* Demux resource id 와 동일한 값으로 보이는 wResourceID 로 open 시킨다. */
				//Current only DVB-CSA is supported. 11 Jan 2010

				if(ERR_OK != svc_cas_DevDscOpen(wResourceID,  ePAL_DSC_DSCTYPE_DVB, ePAL_DSC_DATATYPE_PES, &ulPalDscIdx ) )
				{
					// pal layer에서 가용 dsc 가 없는 것임.
					ReferenceID = VD_REF_ID_ERROR;
					HxLOG_Print(" : No more valid referenceID\n");
				}
				else
				{
					HxLOG_Debug("%s dsc open, idx: 0x%x \n", __FUNCTION__, ulPalDscIdx);
					s_stHVD_DSC_Info[wResourceID].RefInfo[i].ReferenceID = ReferenceID;
					s_stHVD_DSC_Info[wResourceID].RefInfo[i].ulPalDscIdx = ulPalDscIdx;
					s_stHVD_DSC_Info[wResourceID].RefInfo[i].bNeedToClose = FALSE;
					DSC_RefIdList[ReferenceID].bUsed = TRUE;
				}
			}
		}
	}

	HxLOG_Info("\n\t(-)local_VdDsc_Open\n\n");
	return ReferenceID;
}

// ewlee 20100826 - This API does not have locking routine. Use carefully.
ia_result local_VdDsc_Close( vd_ref_id wRefId, HUINT8 ucHmxRefId )
{
	HUINT32	i, j;
	HUINT16	usTempPID;
	HVD_DSC_RSC_INFO *pDscRscInfo;
	vd_rsc_id	DemuxId = VD_RSC_ID_INVALID;

	HxLOG_Info("\n\t(+)local_VdDsc_Close wRefId(0x%X)\n\n", (unsigned int)wRefId);

	if (ucHmxRefId == 1)
		DemuxId = local_VdDsc_GetDemuxIdOfHmxRefId(wRefId);
	else
		DemuxId = local_VdDsc_GetDemuxIdOfScRefId(wRefId);

	if (DemuxId == VD_RSC_ID_INVALID)
	{
		HxLOG_Info("Demux ID is invalid\n");
		return IA_FAIL;
	}

	for( i = 0; i < HMX_VD_DMX_MAX_NUM; i++ )
	{
		HxLOG_Info("Scan ResoruceID(0x%X)\n", s_stHVD_DSC_Info[i].ResourceID);
		if( s_stHVD_DSC_Info[i].ResourceID != VD_RSC_ID_INVALID && s_stHVD_DSC_Info[i].ResourceID == DemuxId)
		{
			for( j = 0; j < HMX_VD_DSC_SLOT_MAX_IN_ONE_DEMUX; j++ )
			{
				pDscRscInfo  = &(s_stHVD_DSC_Info[i].RefInfo[j]);
				if( pDscRscInfo->ReferenceID == wRefId )
				{
					HxLOG_Info(" (%d:%d), (0x%X:0x%X)\n", i, j, pDscRscInfo->ReferenceID, wRefId) ;

					usTempPID = s_stHVD_DSC_Info[i].RefInfo[j].usPid;

					if (pDscRscInfo->usUsedCount > 0)
					{
						HxLOG_Info(" This descrambler is still being used(%d) PID : 0x%04X\n", pDscRscInfo->usUsedCount, pDscRscInfo->usPid) ;
						return IA_SUCCESS;
					}

					if(ERR_OK != svc_cas_DevDscClose(pDscRscInfo->ulPalDscIdx) )
					{
						HxLOG_Print(" svc_cas_DevDscClose: FAIL!!!(pal idx=0x%X)\n", pDscRscInfo->ulPalDscIdx);
						return IA_FAIL;
					}
					else
					{
						local_VdDsc_ClearReferenceID(wRefId);
						local_VdDsc_ClearRefInfo(pDscRscInfo);
						break;
					}
				}
			}

			/* Step: when there are no valid ref id, it makes resouce id goes to invalid value. */
			for( j = 0; j < HMX_VD_DSC_SLOT_MAX_IN_ONE_DEMUX; j++ )
			{
				if( s_stHVD_DSC_Info[i].RefInfo[j].ReferenceID != VD_REF_ID_ERROR )
					break;
			}

			if( j == HMX_VD_DSC_SLOT_MAX_IN_ONE_DEMUX )
			{
				// s_stHVD_DSC_Info[i]에 더이상 valid한 reference ID를 가진 RefInfo가 없으면 s_stHVD_DSC_Info[i]를 reset.
				s_stHVD_DSC_Info[i].ResourceID = VD_RSC_ID_INVALID;
			}
		}
	}

	HxLOG_Info("\n\t(-)local_VdDsc_Close\n\n");
	return IA_SUCCESS;
}


void local_VdDsc_ClearProtectionKey( vd_ref_id wRefId )
{
	HUINT32		i, j;
	vd_rsc_id	DemuxId = VD_RSC_ID_INVALID;

	DemuxId = local_VdDsc_GetDemuxIdOfScRefId(wRefId);

	for( i = 0; i < HMX_VD_DMX_MAX_NUM; i++ )
	{
		if( s_stHVD_DSC_Info[i].ResourceID != VD_RSC_ID_INVALID && s_stHVD_DSC_Info[i].ResourceID == DemuxId)
		{
			for( j = 0; j < HMX_VD_DSC_SLOT_MAX_IN_ONE_DEMUX; j++ )
			{
				if( s_stHVD_DSC_Info[i].RefInfo[j].ReferenceID == wRefId )
				{
					/* SESSION KEY RESET */
					svc_cas_DevDscClearProtectionKey(s_stHVD_DSC_Info[i].RefInfo[j].ulPalDscIdx);

					HxLOG_Info("CLEAR PROTECTION KEY OK!\n");
					return;
				}
			}
		}
	}

	HxLOG_Info("CLEAR PROTECTION KEY FAIL!!!! \n");
	return;
}

static ia_result local_VdDsc_SetPID(vd_ref_id wRefId, vd_dd_pid_st DdPid)
{
	HUINT32			i, j;
	HUINT32			DscInfoIdx = VD_RSC_ID_INVALID;
	HUINT32			RefInfoIdx;
	HUINT32			TempRefId;
	vd_rsc_id	DemuxId = VD_RSC_ID_INVALID;

	DemuxId = local_VdDsc_GetDemuxIdOfScRefId(wRefId);

	HxLOG_Trace();
	if (DemuxId == VD_RSC_ID_INVALID)
		return IA_FAIL;

	RefInfoIdx = 0;
	HxLOG_Debug("[%s] wRefId(%d) DdPid(0x%x)\n", __FUNCTION__, wRefId, DdPid.wPid);
	for( i = 0; i < HMX_VD_DMX_MAX_NUM; i++ )
	{
		if( s_stHVD_DSC_Info[i].ResourceID != VD_RSC_ID_INVALID && s_stHVD_DSC_Info[i].ResourceID == DemuxId)
		{
			for( j = 0; j < HMX_VD_DSC_SLOT_MAX_IN_ONE_DEMUX; j++ )
			{
				if (s_stHVD_DSC_Info[i].RefInfo[j].usPid == HMX_INVALID_PID)
				{
					HxLOG_Debug("s_stHVD_DSC_Info[i].RefInfo[j].ReferenceID: 0x%x \n", s_stHVD_DSC_Info[i].RefInfo[j].ReferenceID);
					if (s_stHVD_DSC_Info[i].RefInfo[j].ReferenceID == VD_REF_ID_ERROR)
					{
						TempRefId = local_VdDsc_Open(s_stHVD_DSC_Info[i].ResourceID, NULL);
						local_VdDsc_SetHMXInfo(wRefId, TempRefId, DdPid.wPid);
					}
					else
					{
						local_VdDsc_SetHMXInfo(wRefId, s_stHVD_DSC_Info[i].RefInfo[j].ReferenceID, DdPid.wPid);
					}
					DscInfoIdx = i;
					RefInfoIdx = j;
					s_stHVD_DSC_Info[i].RefInfo[j].usPid = DdPid.wPid;
					s_stHVD_DSC_Info[i].RefInfo[j].usUsedCount++;
					break;
				}
				else if (s_stHVD_DSC_Info[i].RefInfo[j].usPid == DdPid.wPid)
				{
					HxLOG_Debug("s_stHVD_DSC_Info[i].RefInfo[j].usPid == DdPid.wPid \n");
					local_VdDsc_SetHMXInfo(wRefId, s_stHVD_DSC_Info[i].RefInfo[j].ReferenceID, DdPid.wPid);
					s_stHVD_DSC_Info[i].RefInfo[j].usUsedCount++;
					if (s_stHVD_DSC_Info[i].RefInfo[j].usUsedCount > 1)
						return IA_SUCCESS;
					// ewlee 20100826
					DscInfoIdx = i;
					RefInfoIdx = j;
					break;
				}
			}

			if( DscInfoIdx != VD_RSC_ID_INVALID )
				break;
		}
	}

	if( (DscInfoIdx == VD_RSC_ID_INVALID) ||(RefInfoIdx == VD_REF_ID_ERROR) )
	{
		HxLOG_Print("RscID(0x%X), Reference ID(0x%X)\n", DscInfoIdx, RefInfoIdx);
		return VD_WRONG_REF_ID;
	}
	HxLOG_Debug("DSC RSC:%d(%d:%d)] ulPalDscIdx: %d, SetPID(0x%X) => ", s_stHVD_DSC_Info[DscInfoIdx].ResourceID, DscInfoIdx, RefInfoIdx, s_stHVD_DSC_Info[DscInfoIdx].RefInfo[RefInfoIdx].ulPalDscIdx, DdPid.wPid);

	if(ERR_OK !=svc_cas_DevDscSetPid(s_stHVD_DSC_Info[DscInfoIdx].RefInfo[RefInfoIdx].ulPalDscIdx, DdPid.wPid ) )
	{
		HxLOG_Print("RscID(0x%X), Reference ID(0x%X)\n", DscInfoIdx, RefInfoIdx);
		return IA_FAIL;
	}

	return IA_SUCCESS;
}

static void local_VdDsc_SetKEY_MultipleDSC(vd_ref_id wRefId, vd_dd_key_st DdKey, HUINT8 *p)
{
	HUINT32			j = 0;
#if defined(WA_2SC_SAME_ENTITLEMENT_GRAP_KEY_DATA)
	HUINT32			k = 0;
#endif
	RefId_PIDS	*pRefPIDInfo = NULL;

	HxLOG_Trace();
	HxLOG_Debug("[%s] wRefId: 0x%x \n", __FUNCTION__, wRefId);

	pRefPIDInfo = local_VdDsc_GetRefPIDInfo(wRefId);

	if (pRefPIDInfo == NULL)
		return;

#if defined(CONFIG_DEBUG)
	local_VdDsc_PrintDemuxInfo();
#endif


#if defined(WA_2SC_SAME_ENTITLEMENT_GRAP_KEY_DATA)

	if(pRefPIDInfo->p != NULL)
	{
		HxLOG_Debug("pRefPIDInfo->p \n");

		for(k=0; k<HMX_VD_REF_PID_MAX;k++)
		{
			HxLOG_Debug("pRefPIDInfo->ulHmxRefId[k]: 0x%x \n", pRefPIDInfo->ulHmxRefId[k]);
			if(pRefPIDInfo->ulHmxRefId[k] != VD_REF_ID_ERROR)// && (pRefPIDInfo->bKeySetEven[k] == FALSE || pRefPIDInfo->bKeySetOdd[k] == FALSE))
			{
				HxLOG_Debug("pRefPIDInfo->ddkeyst.eKeyType: %d, pRefPIDInfo->bKeySetEven[k]: %d \n", pRefPIDInfo->ddkeyst.eKeyType, pRefPIDInfo->bKeySetEven[k]);
				if((pRefPIDInfo->ddkeyst.eKeyType == VD_DD_KEY_EVEN && pRefPIDInfo->bKeySetEven[k] == FALSE)
					|| (pRefPIDInfo->ddkeyst.eKeyType == VD_DD_KEY_ODD && pRefPIDInfo->bKeySetOdd[k] == FALSE))
				{
					local_VdDsc_SetKEY(pRefPIDInfo->ulHmxRefId[k], pRefPIDInfo->ddkeyst, pRefPIDInfo->p);
				}

			}
		}

		HxLOG_Debug("free pRefPIDInfo->p \n");
		HLIB_STD_MemFree(pRefPIDInfo->p);
		pRefPIDInfo->p = NULL;

	}
#endif

	HxLOG_Debug("Softcell Ref ID:%d\n", pRefPIDInfo->ulSCellRefId);
	for (j = 0; j < HMX_VD_REF_PID_MAX; j++)
	{
		if (pRefPIDInfo->ulHmxRefId[j] == VD_REF_ID_ERROR)
			continue;

		HxLOG_Debug("HUMAX Ref ID:%d\n", pRefPIDInfo->ulHmxRefId[j]);



		if(local_VdDsc_SetKEY(pRefPIDInfo->ulHmxRefId[j], DdKey, p) == ERR_OK)
		{
			HxLOG_Debug("local_VdDsc_SetKEY() is OK! \n");
#if defined(WA_2SC_SAME_ENTITLEMENT_GRAP_KEY_DATA)

			if(DdKey.eKeyType == VD_DD_KEY_EVEN)
			{
				pRefPIDInfo->bKeySetEven[j] = TRUE;
			}
			if(DdKey.eKeyType == VD_DD_KEY_ODD)
			{
				pRefPIDInfo->bKeySetOdd[j] = TRUE;
			}
			HxSTD_MemSet(&(pRefPIDInfo->ddkeyst), 0x00, sizeof(vd_dd_key_st));
			HxSTD_memcpy(&(pRefPIDInfo->ddkeyst), &DdKey, sizeof(vd_dd_key_st));
			if(pRefPIDInfo->p != NULL)
			{
				HxLOG_Debug("free pRefPIDInfo->p \n");
				HLIB_STD_MemFree(pRefPIDInfo->p);
				pRefPIDInfo->p = NULL;
			}
			if(pRefPIDInfo->p == NULL)
			{
				HxLOG_Debug("alloc pRefPIDInfo->p, DdKey.wKeyLength: %d\n", DdKey.wKeyLength);
				pRefPIDInfo->p = (HUINT8*)HLIB_STD_MemCalloc(sizeof(HUINT8) * (DdKey.wKeyLength+1));
				if (pRefPIDInfo->p)
				{
					HxSTD_memcpy(pRefPIDInfo->p, p, DdKey.wKeyLength);
				} else
					HxLOG_Error("pRefPIDInfo->p, alloc fail \n");
			}
#endif
		}

	}
}

#else

void local_VdDsc_ClearProtectionKey( vd_ref_id wRefId )
{
	HUINT32		i, j;

	for( i = 0; i < HMX_VD_DMX_MAX_NUM; i++ )
	{
		if( s_stHVD_DSC_Info[i].ResourceID != VD_RSC_ID_INVALID )
		{
			for( j = 0; j < HMX_VD_DSC_SLOT_MAX_IN_ONE_DEMUX; j++ )
			{
 #if defined(CONFIG_OP_SKYLINK)
				if (wRefId == VD_REF_ID_GLOBAL )
				 {

					svc_cas_DevDscClearProtectionKey(s_stHVD_DSC_Info[i].RefInfo[j].ulPalDscIdx);

					HxLOG_Info("CLEAR PROTECTION KEY OK!\n");
				 }
				 else
#endif
				{
					if( s_stHVD_DSC_Info[i].RefInfo[j].ReferenceID == wRefId )
					{
						/* SESSION KEY RESET */
						svc_cas_DevDscClearProtectionKey(s_stHVD_DSC_Info[i].RefInfo[j].ulPalDscIdx);

						HxLOG_Info("CLEAR PROTECTION KEY OK!\n");
						return;
					}
				}
			}
		}
	}

	HxLOG_Info("CLEAR PROTECTION KEY FAIL!!!! \n");
	return;
}

static ia_result local_VdDsc_SetPID(vd_ref_id wRefId, vd_dd_pid_st DdPid)
{
	HUINT32			i, j;
	HUINT32			DscInfoIdx = VD_RSC_ID_INVALID;
	HUINT32			RefInfoIdx;

	RefInfoIdx = 0;
	HxLOG_Debug(" wRefId(%d) DdPid(0x%x)\n", wRefId, DdPid.wPid);
	for( i = 0; i < HMX_VD_DMX_MAX_NUM; i++ )
	{
		if( s_stHVD_DSC_Info[i].ResourceID != VD_RSC_ID_INVALID )
		{
			for( j = 0; j < HMX_VD_DSC_SLOT_MAX_IN_ONE_DEMUX; j++ )
			{
				if( s_stHVD_DSC_Info[i].RefInfo[j].ReferenceID == wRefId )
				{
					DscInfoIdx = i;
					RefInfoIdx = j;
					s_stHVD_DSC_Info[i].RefInfo[j].usPid = DdPid.wPid;
					break;
				}
			}

			if( DscInfoIdx != VD_RSC_ID_INVALID )
				break;
		}
	}

	if( (DscInfoIdx == VD_RSC_ID_INVALID) ||(RefInfoIdx == VD_REF_ID_ERROR) )
	{
		HxLOG_Print("RscID(0x%X), Reference ID(0x%X)\n", DscInfoIdx, RefInfoIdx);
		return VD_WRONG_REF_ID;
	}
	HxLOG_Debug("DSC RSC:%d(%d:%d)] ulPalDscIdx: %d, SetPID(0x%X) => ", s_stHVD_DSC_Info[DscInfoIdx].ResourceID, DscInfoIdx, RefInfoIdx, s_stHVD_DSC_Info[DscInfoIdx].RefInfo[RefInfoIdx].ulPalDscIdx, DdPid.wPid);

	if(ERR_OK !=svc_cas_DevDscSetPid(s_stHVD_DSC_Info[DscInfoIdx].RefInfo[RefInfoIdx].ulPalDscIdx, DdPid.wPid ) )
	{
		HxLOG_Print("RscID(0x%X), Reference ID(0x%X)\n", DscInfoIdx, RefInfoIdx);
		return IA_FAIL;
	}

	return IA_SUCCESS;
}

#endif

static HERROR local_VdDsc_SetKEY(vd_ref_id wRefId, vd_dd_key_st DdKey, HUINT8 *p)
{
	HUINT32			i, j;
	HUINT32			DescInfoIdx = 0xFFFFFFFF;
	HUINT32			RefInfoIdx;
	HUINT32			usTempPID;
	HERROR		hErr = ERR_FAIL;
#if defined(CONFIG_WA_2SC_SAME_ENTITLEMENT)	// ewlee 20100831
	vd_rsc_id	DemuxId = VD_RSC_ID_INVALID;

	DemuxId = local_VdDsc_GetDemuxIdOfHmxRefId(wRefId);

	if (DemuxId == VD_RSC_ID_INVALID)
		return ERR_FAIL;
#endif
	HxLOG_Debug("[%s] wRefId(%d)\n", __FUNCTION__, wRefId);

	RefInfoIdx = 0;
	for( i = 0; i < HMX_VD_DMX_MAX_NUM; i++ )
	{
#if defined(CONFIG_WA_2SC_SAME_ENTITLEMENT)	// ewlee 20100831
		if( s_stHVD_DSC_Info[i].ResourceID != VD_RSC_ID_INVALID && s_stHVD_DSC_Info[i].ResourceID == DemuxId)
#else
		if( s_stHVD_DSC_Info[i].ResourceID != VD_RSC_ID_INVALID )
#endif
		{
			for( j = 0; j < HMX_VD_DSC_SLOT_MAX_IN_ONE_DEMUX; j++ )
			{
				if( s_stHVD_DSC_Info[i].RefInfo[j].ReferenceID == wRefId )
				{
					DescInfoIdx = i;
					RefInfoIdx = j;
					usTempPID = s_stHVD_DSC_Info[i].RefInfo[j].usPid;
					break;
				}
			}

			if( DescInfoIdx != 0xFFFFFFFF )
				break;
		}
	}

	if( DescInfoIdx == 0xFFFFFFFF )
	{
		HxLOG_Print("Invalid Reference ID\n");
		return ERR_FAIL;
	}

	HxLOG_Debug("DESC(rscId=%d,ref id=%d), %s, PID(0x%X)\n",
			s_stHVD_DSC_Info[DescInfoIdx].ResourceID,
			s_stHVD_DSC_Info[DescInfoIdx].RefInfo[RefInfoIdx].ReferenceID,
			(DdKey.eKeyType== VD_DD_KEY_ODD)?"ODD":"EVEN",
			s_stHVD_DSC_Info[DescInfoIdx].RefInfo[RefInfoIdx].usPid);

#if defined(PRINT_CW)
	if(DdKey.eKeyType == VD_DD_KEY_EVEN)
		HxLOG_Debug("EVEN Key: \n");
	else
		HxLOG_Debug("ODD Key: \n");

	for( i = 0; i < DdKey.wKeyLength; i++ )
	{
		HxLOG_Debug("key[%d] 0x%0x\n", i, p[i]);
	}
	HxLOG_Debug("\n");
#endif

	for ( i = 0; i < HMX_VD_DSC_SLOT_MAX_IN_ONE_DEMUX; i++)
	{
		if ( s_stHVD_DSC_Info[DescInfoIdx].RefInfo[i].usPid == usTempPID)
		{
#if defined(CONFIG_MW_CAS_IRDETO_SECURE)
			if (s_EnabledCWMode == 1)
			{
				#if defined(PRINT_CW)
				{
					HINT32		i;
					for(i = 0; i < 16; i++)
					{
						HxLOG_Info("\t pCSSK[%d] = 0x%0x\n", i, pCSSK[i]);
					}
				}
				#endif

				hErr = svc_cas_DevDscSetProtectionKey(s_stHVD_DSC_Info[DescInfoIdx].RefInfo[RefInfoIdx].ulPalDscIdx, pCSSK);

				if(hErr != ERR_OK)
				{
					HxLOG_Print("svc_cas_DevDscSetProtectionKey\n");
				}

			}
#endif
			{
				if( DdKey.eKeyType == VD_DD_KEY_EVEN)
				{
					HxSTD_memcpy(s_stHVD_DSC_Info[DescInfoIdx].RefInfo[RefInfoIdx].EvenKey, p, DdKey.wKeyLength);

					HxLOG_Debug("set even key to DSC: 0x%x \n\n\n", s_stHVD_DSC_Info[DescInfoIdx].RefInfo[RefInfoIdx].ulPalDscIdx);
					hErr =svc_cas_DevDscSetKey(s_stHVD_DSC_Info[DescInfoIdx].RefInfo[RefInfoIdx].ulPalDscIdx,
												ePAL_DSC_KEYPARITY_EVEN,
												s_stHVD_DSC_Info[DescInfoIdx].RefInfo[RefInfoIdx].EvenKey);
					if(hErr != ERR_OK)
					{
						HxLOG_Print("svc_cas_DevDscSetKey fail\n");
					}
				}
				else if( DdKey.eKeyType == VD_DD_KEY_ODD )
				{
					HxLOG_Debug("set odd key to DSC: 0x%x \n\n\n", s_stHVD_DSC_Info[DescInfoIdx].RefInfo[RefInfoIdx].ulPalDscIdx);

					HxSTD_memcpy(s_stHVD_DSC_Info[DescInfoIdx].RefInfo[RefInfoIdx].OddKey, p, DdKey.wKeyLength);
					hErr = svc_cas_DevDscSetKey(s_stHVD_DSC_Info[DescInfoIdx].RefInfo[RefInfoIdx].ulPalDscIdx,
												ePAL_DSC_KEYPARITY_ODD,
												s_stHVD_DSC_Info[DescInfoIdx].RefInfo[RefInfoIdx].OddKey);
					if(hErr != ERR_OK)
					{
						HxLOG_Print("svc_cas_DevDscSetKey fail\n");
					}
				}
				else
				{
					HxLOG_Print(": Invalid KeyType\n");
				}
			}
		}
	}

	return hErr;
}

static ia_result local_VdDsc_ClearKey(HUINT32  dscIdx)
{
	HUINT8	EvenKey[8];
	HUINT8	OddKey[8];

	HxSTD_memset( EvenKey, 0xFF, 8);
	HxSTD_memset( OddKey, 0xFF, 8);

	svc_cas_DevDscSetKey(dscIdx, ePAL_DSC_KEYPARITY_EVEN, EvenKey);
	svc_cas_DevDscSetKey(dscIdx, ePAL_DSC_KEYPARITY_ODD, OddKey);

	HxLOG_Info("[DESC : local_VdDsc_ClearKey dscIdx (%d)\n", dscIdx);

	return IA_SUCCESS;
}

static ia_result local_VdDsc_RemovePID(vd_ref_id wRefId, vd_dd_pid_st DdPid)
{
#if defined(CONFIG_WA_2SC_SAME_ENTITLEMENT)	// ewlee 20100825
	HUINT32			i, j;
	HUINT32			DscInfoIdx = VD_RSC_ID_INVALID;
	HUINT32			RefInfoIdx;
	vd_rsc_id	DemuxId = VD_RSC_ID_INVALID;
	RefId_PIDS	*pRefPIDs = NULL;
	HUINT32		PIDIdx = HMX_VD_REF_PID_MAX;

	DemuxId = local_VdDsc_GetDemuxIdOfScRefId(wRefId);

	if (DemuxId == VD_RSC_ID_INVALID)
		return IA_FAIL;

	pRefPIDs = local_VdDsc_GetRefPIDInfo(wRefId);
	if (pRefPIDs == NULL)
		return IA_FAIL;

	// ewlee 20100831 - check whether pid is already removed or not because of Softcell bug(?).
	for (i = 0; i < HMX_VD_REF_PID_MAX; i++)
	{
		if (pRefPIDs->usPID[i] != DdPid.wPid)
			continue;

		if (pRefPIDs->ucRemovePID[i] == 1)
		{
			HxLOG_Info(" ########## wRefId(%d) DdPid(0x%x) is already removed\n", wRefId, DdPid.wPid);
			return IA_SUCCESS;
		}

		PIDIdx = i;
		break;
	}

	RefInfoIdx = 0;
	HxLOG_Info(" wRefId(%d) DdPid(0x%x)\n", wRefId, DdPid.wPid);
	for( i = 0; i < HMX_VD_DMX_MAX_NUM; i++ )
	{
		if( s_stHVD_DSC_Info[i].ResourceID != VD_RSC_ID_INVALID && s_stHVD_DSC_Info[i].ResourceID == DemuxId)
		{
			for( j = 0; j < HMX_VD_DSC_SLOT_MAX_IN_ONE_DEMUX; j++ )
			{
				if( s_stHVD_DSC_Info[i].RefInfo[j].usPid == DdPid.wPid)
				{
					if (s_stHVD_DSC_Info[i].RefInfo[j].usUsedCount > 0)	// ewlee 20100826 - decrease only if it is over 0.
						s_stHVD_DSC_Info[i].RefInfo[j].usUsedCount--;

					if (PIDIdx < HMX_VD_REF_PID_MAX)      // prevent fix (overrun_static)
						pRefPIDs->ucRemovePID[PIDIdx] = 1;	// ewlee 20100831 - set flag

					HxLOG_Info(" PID in DSC[%d-%d] info(0x%x) UsedCount %d\n", i, j, s_stHVD_DSC_Info[i].RefInfo[j].usPid, s_stHVD_DSC_Info[i].RefInfo[j].usUsedCount);

					if (s_stHVD_DSC_Info[i].RefInfo[j].usUsedCount > 0)
						return IA_SUCCESS;

					DscInfoIdx = i;
					RefInfoIdx = j;
					break;
				}
			}

			if( DscInfoIdx != VD_RSC_ID_INVALID )
				break;
		}
	}

	if( (DscInfoIdx == VD_RSC_ID_INVALID) ||(RefInfoIdx == VD_REF_ID_ERROR) )
	{
		HxLOG_Print("RscID(0x%X), Reference ID(0x%X)\n", DscInfoIdx, RefInfoIdx);
		return VD_WRONG_REF_ID;
	}
	HxLOG_Info("DSC RSC:%d(%d:%d)] RemovePID(0x%X) => ", s_stHVD_DSC_Info[DscInfoIdx].ResourceID, DscInfoIdx, RefInfoIdx, DdPid.wPid);

	local_VdDsc_ClearKey(s_stHVD_DSC_Info[DscInfoIdx].RefInfo[RefInfoIdx].ulPalDscIdx);

	if(ERR_OK !=svc_cas_DevDscResetPid(s_stHVD_DSC_Info[DscInfoIdx].RefInfo[RefInfoIdx].ulPalDscIdx, DdPid.wPid ) )
	{
		HxLOG_Print("RscID(0x%X), Reference ID(0x%X)\n", DscInfoIdx, RefInfoIdx);
		return IA_FAIL;
	}

	return IA_SUCCESS;
#else
	HUINT32			i, j;
	HUINT32			DscInfoIdx = VD_RSC_ID_INVALID;
	HUINT32			RefInfoIdx;

	RefInfoIdx = 0;
	HxLOG_Info(" wRefId(%d) DdPid(0x%x)\n", (unsigned int)wRefId, DdPid.wPid);
	for( i = 0; i < HMX_VD_DMX_MAX_NUM; i++ )
	{
		if( s_stHVD_DSC_Info[i].ResourceID != VD_RSC_ID_INVALID )
		{
			for( j = 0; j < HMX_VD_DSC_SLOT_MAX_IN_ONE_DEMUX; j++ )
			{
				if( s_stHVD_DSC_Info[i].RefInfo[j].ReferenceID == wRefId )
				{
					DscInfoIdx = i;
					RefInfoIdx = j;
					s_stHVD_DSC_Info[i].RefInfo[j].usPid = DdPid.wPid;
					break;
				}
			}

			if( DscInfoIdx != VD_RSC_ID_INVALID )
				break;
		}
	}

	if( (DscInfoIdx == VD_RSC_ID_INVALID) ||(RefInfoIdx == VD_REF_ID_ERROR) )
	{
		HxLOG_Print("RscID(0x%X), Reference ID(0x%X)\n", DscInfoIdx, RefInfoIdx);
		return VD_WRONG_REF_ID;
	}
	HxLOG_Info("DSC RSC:%d(%d:%d)] RemovePID(0x%X) => ", (unsigned int) (s_stHVD_DSC_Info[DscInfoIdx].ResourceID), DscInfoIdx, RefInfoIdx, DdPid.wPid);

	local_VdDsc_ClearKey(s_stHVD_DSC_Info[DscInfoIdx].RefInfo[RefInfoIdx].ulPalDscIdx);

	if(ERR_OK !=svc_cas_DevDscResetPid(s_stHVD_DSC_Info[DscInfoIdx].RefInfo[RefInfoIdx].ulPalDscIdx, DdPid.wPid ) )
	{
		HxLOG_Print("RscID(0x%X), Reference ID(0x%X)\n", DscInfoIdx, RefInfoIdx);
		return IA_FAIL;
	}

	return IA_SUCCESS;
#endif
}

ia_result HVD_DSC_Initialise(void)
{
	HUINT32	i, j;

	HxSTD_memset(s_stHVD_DSC_Info, 0, sizeof(HVD_DSC_RSC) * HMX_VD_DMX_MAX_NUM);

	for( i = 0; i < HMX_VD_DMX_MAX_NUM; i++ )
	{
		s_stHVD_DSC_Info[i].ResourceID = VD_RSC_ID_INVALID;
		for( j = 0; j < HMX_VD_DSC_SLOT_MAX_IN_ONE_DEMUX; j++ )
		{
			s_stHVD_DSC_Info[i].RefInfo[j].ReferenceID = VD_REF_ID_ERROR;
			s_stHVD_DSC_Info[i].RefInfo[j].ulPalDscIdx = HMX_DSC_INVALID_PAL_IDX;
			s_stHVD_DSC_Info[i].RefInfo[j].usPid = HMX_INVALID_PID;
			s_stHVD_DSC_Info[i].RefInfo[j].bNeedToClose = FALSE;
			s_stHVD_DSC_Info[i].RefInfo[j].usUsedCount = 0;	// ewlee 20100825
		}
	}

	for(i=0; i< HMX_VD_DSC_REFID_MAX; i++)
	{
		DSC_RefIdList[i].bUsed = FALSE;
		DSC_RefIdList[i].ulRefId = VD_REF_ID_ERROR;
	}

	// ewlee 20100830
#if defined(CONFIG_WA_2SC_SAME_ENTITLEMENT)
	local_VdDsc_InitDemuxInfo();
#endif

#if defined(IRPLAT_SYS_LIMITED_DESC_SLOT)
	HxSTD_memset(s_DescRefCount, 0, sizeof(DSC_RefCount) * HMX_VD_DMX_MAX_NUM);
#endif

	if(VK_OK != VK_SEM_Create(&s_ulDESCAPISem, "s_ulDESCAPISem", VK_SUSPENDTYPE_FIFO)  )
	{
		HxLOG_Print("Semaphore Create Error\n");
		return IA_FAIL;
	}

	return IA_SUCCESS;
}

vd_ref_id HVD_DSC_Open( vd_rsc_id wResourceID, void *pvData )
{
	vd_ref_id	ReferenceID;
	HUINT32		ulPalDscIdx;
	HUINT32		i;

	HxLOG_Info("\n\t(+)HVD_DSC_Open wRefId(0x%X)\n\n", (unsigned int)wResourceID);
	DESC_SEM_GET();

	if( wResourceID >= HMX_VD_DMX_MAX_NUM )
	{
		HxLOG_Print("Invalid wResourceID = %d\n", (unsigned int)wResourceID);
		ReferenceID = VD_REF_ID_ERROR;
	}
	else
	{
		s_stHVD_DSC_Info[wResourceID].ResourceID = wResourceID;

		/* platform의 모든 descrambler slot 에 대한 unique 한 reference ID 를 만들어 낸다. */
		ReferenceID = local_VdDsc_GetAvailableRefID();

		HxLOG_Info("Ref ID = 0x%X\n", (unsigned int) ReferenceID);

		if( ReferenceID == VD_REF_ID_ERROR )
		{
			HxLOG_Print(" No more valid referenceID\n");
			ReferenceID = VD_REF_ID_ERROR;
		}
		else
		{
			/* link ref id list & vd dsc info table */
			for( i = 0; i < HMX_VD_DSC_SLOT_MAX_IN_ONE_DEMUX; i++ )
			{
				if( s_stHVD_DSC_Info[wResourceID].RefInfo[i].ReferenceID == VD_REF_ID_ERROR )
					break;
			}

			if( i == HMX_VD_DSC_SLOT_MAX_IN_ONE_DEMUX )
			{
				HxLOG_Print("RefInfo was full\n");
				ReferenceID = VD_REF_ID_ERROR;
			}
			else
			{
#if defined(IRPLAT_SYS_LIMITED_DESC_SLOT)
				if(i >= MAX_DESC_SLOT)
				{
					HUINT32	ulShareDscIdx = s_stHVD_DSC_Info[wResourceID].RefInfo[i-1].ulPalDscIdx;

					s_stHVD_DSC_Info[wResourceID].RefInfo[i].ReferenceID = ReferenceID;
					s_stHVD_DSC_Info[wResourceID].RefInfo[i].ulPalDscIdx = ulShareDscIdx;
					s_stHVD_DSC_Info[wResourceID].RefInfo[i].bNeedToClose = FALSE;
					DSC_RefIdList[ReferenceID].bUsed = TRUE;

					if(s_DescRefCount[wResourceID].ulCount == 0)
					{
						s_DescRefCount[wResourceID].ulPalDscIdx = ulShareDscIdx;
					}
					else
					{
						if(s_DescRefCount[wResourceID].ulPalDscIdx != ulShareDscIdx)		IR3VDDscAssert(0);
					}
					s_DescRefCount[wResourceID].ulCount++;
				}
				else
#endif
				/* Demux resource id 와 동일한 값으로 보이는 wResourceID 로 open 시킨다. */
				//Current only DVB-CSA is supported. 11 Jan 2010

				if(ERR_OK != svc_cas_DevDscOpen(wResourceID,  ePAL_DSC_DSCTYPE_DVB, ePAL_DSC_DATATYPE_PES, &ulPalDscIdx ) )
				{
					// pal layer에서 가용 dsc 가 없는 것임.
					ReferenceID = VD_REF_ID_ERROR;
					HxLOG_Print(" : No more valid referenceID\n");
				}
				else
				{
				#if defined(CONFIG_WA_2SC_SAME_ENTITLEMENT)	// ewlee 20100830
					local_VdDsc_SetDemuxRefId(wResourceID, ReferenceID);
				#endif
				HxLOG_Debug("%s dsc open, idx: 0x%x \n", __FUNCTION__, ulPalDscIdx);

					s_stHVD_DSC_Info[wResourceID].RefInfo[i].ReferenceID = ReferenceID;
					s_stHVD_DSC_Info[wResourceID].RefInfo[i].ulPalDscIdx = ulPalDscIdx;
					s_stHVD_DSC_Info[wResourceID].RefInfo[i].bNeedToClose = FALSE;
					DSC_RefIdList[ReferenceID].bUsed = TRUE;
				}
			}
		}
	}
	DESC_SEM_RELEASE();
	HxLOG_Info("\n\t(-)HVD_DSC_Open\n\n");
	return ReferenceID;
}

#if defined(CONFIG_WA_2SC_SAME_ENTITLEMENT)	// ewlee 20100825
ia_result HVD_DSC_MultipleClose(vd_ref_id wRefId)
{
	HUINT32			j;
	HUINT16		usIsClose = 1;
	ia_result	nRet = IA_FAIL;
	RefId_PIDS	*pRefPIDInfo = NULL;

	DESC_SEM_GET();

	pRefPIDInfo = local_VdDsc_GetRefPIDInfo(wRefId);

	if (pRefPIDInfo == NULL)
		return nRet;

	HxLOG_Info("Softcell Ref ID:%d\n", pRefPIDInfo->ulSCellRefId);
	for (j = 0; j < HMX_VD_REF_PID_MAX; j++)
	{
		if (pRefPIDInfo->ulHmxRefId[j] == VD_REF_ID_ERROR)
			continue;

		HxLOG_Info("HUMAX Ref ID:%d\n", pRefPIDInfo->ulHmxRefId[j]);
		if (wRefId == pRefPIDInfo->ulHmxRefId[j])
			usIsClose = 0;
		nRet = local_VdDsc_Close(pRefPIDInfo->ulHmxRefId[j], 1);
	}

	if (usIsClose == 1)
		local_VdDsc_Close(wRefId, 0);

	local_VdDsc_ClearSecureReferenceID(wRefId);
	DESC_SEM_RELEASE();
	return nRet;
}
#endif

ia_result HVD_DSC_Close( vd_ref_id wRefId )
{
	HUINT32	i, j;
	HUINT16	usTempPID;
	HVD_DSC_RSC_INFO *pDscRscInfo;

	HxLOG_Info("\n\t(+)HVD_DSC_Close wRefId(0x%X)\n\n", (unsigned int)wRefId);
	DESC_SEM_GET();

	for( i = 0; i < HMX_VD_DMX_MAX_NUM; i++ )
	{
		HxLOG_Info("Scan ResoruceID(0x%X)\n", s_stHVD_DSC_Info[i].ResourceID);
		if( s_stHVD_DSC_Info[i].ResourceID != VD_RSC_ID_INVALID )
		{
			for( j = 0; j < HMX_VD_DSC_SLOT_MAX_IN_ONE_DEMUX; j++ )
			{
				pDscRscInfo  = &(s_stHVD_DSC_Info[i].RefInfo[j]);

				if( pDscRscInfo->ReferenceID == wRefId )
				{
					HxLOG_Info(" (%d:%d), (0x%X:0x%X)\n", i, j, pDscRscInfo->ReferenceID, wRefId);

					usTempPID = s_stHVD_DSC_Info[i].RefInfo[j].usPid;

					#if defined(CONFIG_WA_2SC_SAME_ENTITLEMENT)	// ewlee 20100825
					if (pDscRscInfo->usUsedCount > 0)
					{
						HxLOG_Info(" This descrambler is still being used(%d) PID : 0x%04X\n", pDscRscInfo->usUsedCount, pDscRscInfo->usPid);
						DESC_SEM_RELEASE();
						return IA_SUCCESS;
					}
					#endif
#if defined(IRPLAT_SYS_LIMITED_DESC_SLOT)
					if(s_DescRefCount[i].ulPalDscIdx == pDscRscInfo->ulPalDscIdx)
					{
						HUINT32			k = 0;
						vd_dd_pid_st		tmpPid;

						for(k=0;k< HMX_VD_DSC_SLOT_MAX_IN_ONE_DEMUX; k++)
						{
							if(s_stHVD_DSC_Info[i].RefInfo[k].ulPalDscIdx == pDscRscInfo->ulPalDscIdx)
							{
								tmpPid.wPid = s_stHVD_DSC_Info[i].RefInfo[k].usPid;
								local_VdDsc_RemovePID(s_stHVD_DSC_Info[i].RefInfo[k].ReferenceID, tmpPid);
							}
						}

					if(ERR_OK != svc_cas_DevDscClose(pDscRscInfo->ulPalDscIdx) )
					{
						HxLOG_Print(" svc_cas_DevDscClose: FAIL!!!(pal idx=0x%X)\n", pDscRscInfo->ulPalDscIdx);
						DESC_SEM_RELEASE();
						return IA_FAIL;
					}
					else
					{
						s_DescRefCount[i].ulPalDscIdx = 0xffffffff;

						local_VdDsc_ClearReferenceID(wRefId);
						local_VdDsc_ClearRefInfo(pDscRscInfo);
						break;
					}
				}
				else if(j >= MAX_DESC_SLOT)
				{
					s_DescRefCount[i].ulCount--;
					if(s_DescRefCount[i].ulCount< 0)
					{
						IR3VDDscAssert(0);
						s_DescRefCount[i].ulCount = 0;
					}

					local_VdDsc_ClearReferenceID(wRefId);
					local_VdDsc_ClearRefInfo(pDscRscInfo);
					break;
				}
				else
#endif
				if(ERR_OK != PAL_DSC_Close(pDscRscInfo->ulPalDscIdx) )
				{
					HxLOG_Info(" PAL_DSC_Close: FAIL!!!(pal idx=0x%X)\n", pDscRscInfo->ulPalDscIdx);
					DESC_SEM_RELEASE();
					return IA_FAIL;
				}
				else
				{
					local_VdDsc_ClearReferenceID(wRefId);
					local_VdDsc_ClearRefInfo(pDscRscInfo);
					break;
				}
			}
		}

			/* Step: when there are no valid ref id, it makes resouce id goes to invalid value. */
			for( j = 0; j < HMX_VD_DSC_SLOT_MAX_IN_ONE_DEMUX; j++ )
			{
				if( s_stHVD_DSC_Info[i].RefInfo[j].ReferenceID != VD_REF_ID_ERROR )
					break;
			}

			if( j == HMX_VD_DSC_SLOT_MAX_IN_ONE_DEMUX )
			{
				// s_stHVD_DSC_Info[i]에 더이상 valid한 reference ID를 가진 RefInfo가 없으면 s_stHVD_DSC_Info[i]를 reset.
				s_stHVD_DSC_Info[i].ResourceID = VD_RSC_ID_INVALID;
			}
		}
	}

	DESC_SEM_RELEASE();
	HxLOG_Info("\n\t(-)HVD_DSC_Close\n\n");
	return IA_SUCCESS;
}

ia_result HVD_DSC_IO( vd_ref_id wRefId, ia_word16 wOpCode, ia_word32 wDataWriteSize, void *pvDataWrite, ia_word32 *pwDataReadSize, void *pvDataRead )
{
	vd_dd_pid_st			DdPid;
	vd_dd_key_st			DdKey;
	vd_dd_scp3_skey_st		DdScp3Key;
#if 0
	vd_dd_enforce_secure	DdEnforceSecure;
#endif

	HUINT8					*p = (HUINT8 *)pvDataWrite;
	HINT32					i;
	HUINT16					usKeyLen, usSwap;
	ia_result				result = IA_SUCCESS;

HxLOG_Info(" wRefId(0x%X), wOpCode(0x%X), wDataWriteSize(0x%X)\n", wRefId, wOpCode, wDataWriteSize);
	DESC_SEM_GET();

	switch(wOpCode)
	{
		case VD_DD_SET_PID:
			HxLOG_Debug("\n\t(+)VD_DD_SET_PID\n");
			HxSTD_memcpy((HUINT8 *)&DdPid, p, sizeof(vd_dd_pid_st));
			result = local_VdDsc_SetPID(wRefId, DdPid);
			HxLOG_Info("\t(-)VD_DD_SET_PID\n\n");
			break;

		case VD_DD_ADD_PID:
			HxLOG_Debug("\n\t(+)VD_DD_ADD_PID\n");
			HxSTD_memcpy((HUINT8 *)&DdPid, p, sizeof(vd_dd_pid_st));
			result = local_VdDsc_SetPID(wRefId, DdPid);
			HxLOG_Info("\t(-)VD_DD_ADD_PID\n\n");
			break;

		case VD_DD_SET_KEY:
			HxLOG_Info("\n\t(+)VD_DD_SET_KEY\n");
			HxSTD_memcpy((HUINT8 *)&DdKey, p, sizeof(vd_dd_key_st));

			if( VD_DESCRAMBLER_VERSION >= VD_DESCRAMBLER_VERSION_V5_SECURE_CHIP	)
			{
				s_EnabledCWMode = 0;
				if (pCSSK != NULL)
				{
					OxCAS_Free(pCSSK);
					pCSSK = NULL;

					local_VdDsc_ClearProtectionKey(wRefId);
				}
			}

			p += sizeof(vd_dd_key_st);
#if defined(CONFIG_WA_2SC_SAME_ENTITLEMENT)	// ewlee 20100825
			local_VdDsc_SetKEY_MultipleDSC(wRefId, DdKey, p);
#else
			local_VdDsc_SetKEY(wRefId, DdKey, p);
#endif
			HxLOG_Info("\t(-)VD_DD_SET_KEY\n\n");
			break;

		case VD_DD_SET_ARIB_DATA:
			HxLOG_Info("\tVD_DD_SET_ARIB_DATA\n");
			break;

		case VD_DD_SET_SKEY:
			HxLOG_Info("\n\t(+)VD_DD_SET_SKEY\n");
			if(wDataWriteSize < 0)
			{
				result = IA_INVALID_PARAMETER;
				break;
			}

			if(wDataWriteSize == 0)
			{
				s_EnabledCWMode = 0;
				if (pCSSK != NULL)
				{
					OxCAS_Free(pCSSK);
					pCSSK = NULL;
				}

				local_VdDsc_ClearProtectionKey(wRefId);

				result = IA_SUCCESS;
				break;
			}

			if (pCSSK != NULL)
			{
				OxCAS_Free(pCSSK);
				pCSSK = NULL;
			}

			pCSSK = OxCAS_Malloc(wDataWriteSize);
			HxSTD_memset(pCSSK, 0,wDataWriteSize );

			/*
				DI쪽에서 skey를 swap해서 secure cw가 동작하지 않음.
				다른 모델 영향때문에 윗단에서 한번더 swap하기로함.
				ST chip 에서는 swap 하지 않음
				*/
#if !defined(CONFIG_PROD_CPU_ST)
			for(i=0; i<8; i++)
			{
				pCSSK[i+8]=p[i];
				pCSSK[i]=p[i+8];
			}
#else
			HxSTD_memcpy(pCSSK, p, wDataWriteSize);
#endif

			s_EnabledCWMode = 1;
			#if 0	// DEBUG
			{
				HINT32		i;
				for(i = 0; i < 16; i++)
				{
					HxLOG_Print("\t pCSSK[%d] = 0x%0x\n", i, pCSSK[i]);
				}
			}
			#endif
			HxLOG_Info("\t(-)tVD_DD_SET_SKEY\n\n");
			break;

#if defined(CONFIG_MW_CAS_IRDETO_SECURE)
		case	VD_DD_REMOVE_PID:
			HxLOG_Info("\n\t(+)VD_DD_REMOVE_PID\n");
			HxSTD_memcpy((HUINT8 *)&DdPid, p, sizeof(vd_dd_pid_st));
			result = local_VdDsc_RemovePID(wRefId, DdPid);
			HxLOG_Info("\t(-)VD_DD_REMOVE_PID\n\n");
			break;
#if 0
		case	VD_DD_ENFORCE_SECURE_MODE:
			HxLOG_Info("\n\t(+)VD_DD_ENFORCE_SECURE_MODE\n");
			HxSTD_memcpy((HUINT8 *)&DdEnforceSecure, p, sizeof(vd_dd_enforce_secure));

			if (DdEnforceSecure.fSecureCW == IA_TRUE)
			{
				HUINT8 ucSCS, ucJTAG, ucOTP, ucCWE;

				if (CARM_SYS_GetFusingInfo(&ucSCS, &ucJTAG, &ucOTP, &ucCWE) != ERR_OK)
				{
					HxLOG_Info("[CAS_IR_SECURECHIP_UpdateFusingInfo] fail to get fuse statue\n");
					break;
				}

				if (ucCWE != 1)
				{
					PAL_SYS_SetSecureCWFusingInfo(TRUE);

					VK_TASK_Sleep(500);
					MGR_ACTION_RebootSystem();
					//PAL_PWR_Reboot();
				}

			HxLOG_Info("\t(-)VD_DD_ENFORCE_SECURE_MODE\n\n");
			}
			break;
#endif

		case	VD_DD_SET_SESSION_KEY:
			HxLOG_Info("\n\t(+)VD_DD_SET_SESSION_KEY\n");
			HxSTD_memcpy((HUINT8 *)&DdScp3Key, p, sizeof(vd_dd_scp3_skey_st));
			p += sizeof(vd_dd_scp3_skey_st);

			usKeyLen = DdScp3Key.wKeyLength;
			usSwap = (usKeyLen >> 1);

			HxLOG_Info("usKeyLen = %d, usSwap = %d, Algorithm = %d\n", usKeyLen, usSwap, DdScp3Key.eAlgorithm);

			if(usKeyLen == 0)
			{
				s_EnabledCWMode = 0;
				if (pCSSK != NULL)
				{
					OxCAS_Free(pCSSK);
					pCSSK = NULL;
				}

				/* SESSION KEY RESET */
				local_VdDsc_ClearProtectionKey(wRefId);

				result = IA_SUCCESS;
				break;
			}

			if (pCSSK != NULL)
			{
				OxCAS_Free(pCSSK);
				pCSSK = NULL;
			}

			pCSSK = OxCAS_Malloc(usKeyLen);

			HxSTD_memset(pCSSK, 0,usKeyLen );

			/*
				DI쪽에서 skey를 swap해서 secure cw가 동작하지 않음.
				다른 모델 영향때문에 윗단에서 한번더 swap하기로함.
			*/
		#if 1
			for(i=0; i<usSwap; i++)
			{
				pCSSK[i+usSwap]=p[i];
				pCSSK[i]=p[i+usSwap];
			}
		#else
			HxSTD_memcpy(pCSSK, p, usKeyLen);
		#endif

		#if 0 // Debug
			HxLOG_Print("pCSSK: ");
			for( i = 0; i < usKeyLen; i++ )
			{
				HxLOG_Print(".%02X", pCSSK[i]);
			}
			HxLOG_Print("\n");
		#endif

			s_EnabledCWMode = 1;
			HxLOG_Info("\t(-)VD_DD_SET_SESSION_KEY\n\n");
			break;

		case	VD_DD_SET_ENCRYPTED_KEY:
			HxLOG_Info("\n\t(+)VD_DD_SET_ENCRYPTED_KEY\n");
			HxSTD_memcpy((HUINT8 *)&DdKey, p, sizeof(vd_dd_key_st));
			p += sizeof(vd_dd_key_st);
#if defined(CONFIG_WA_2SC_SAME_ENTITLEMENT)	// ewlee 20100825
			local_VdDsc_SetKEY_MultipleDSC(wRefId, DdKey, p);
#else
			local_VdDsc_SetKEY(wRefId, DdKey, p);
#endif
			HxLOG_Info("\t(-)VD_DD_SET_ENCRYPTED_KEY\n\n");
			break;
#endif

		default:
			HxLOG_Print("Unknown type(%d)\n", wOpCode);
			break;
	}
	DESC_SEM_RELEASE();
	return result;
}

#ifdef CONFIG_DEBUG
void HVD_DBGAPI_DSC_INFO(void)
{
	int i,j;
	HVD_DSC_RSC_INFO	*pRefInfo;
#if defined (CONFIG_WA_2SC_SAME_ENTITLEMENT)
	int k;
#endif

	DESC_SEM_GET();
	HLIB_CMD_Printf("====================s_stHVD_DSC_Info==============\n");
	for(i=0; i< HMX_VD_DMX_MAX_NUM; i++)
	{
		if (s_stHVD_DSC_Info[i].ResourceID == VD_RSC_ID_INVALID)
			continue;

		HLIB_CMD_Printf("[%d] DescInfo Resource Id %d\n", i, s_stHVD_DSC_Info[i].ResourceID);
		for(j=0;j< HMX_VD_DSC_SLOT_MAX_IN_ONE_DEMUX; j++)
		{
			pRefInfo = &s_stHVD_DSC_Info[i].RefInfo[j];


			HLIB_CMD_Printf("\t[%d] ReferenceID[%d] DSC palDscIdx[%d] PID[0x%04X] UsedCount[%d]\n"\
				, j, pRefInfo->ReferenceID\
				, pRefInfo->ulPalDscIdx, pRefInfo->usPid, pRefInfo->usUsedCount
				);
			HLIB_CMD_Printf("\t      EVEN %02X %02X %02X %02X   %02X %02X %02X %02X\n\t      ODD  %02X %02X %02X %02X   %02X %02X %02X %02X\n"
				, pRefInfo->EvenKey[0], pRefInfo->EvenKey[1], pRefInfo->EvenKey[2], pRefInfo->EvenKey[3], pRefInfo->EvenKey[4], pRefInfo->EvenKey[5], pRefInfo->EvenKey[6], pRefInfo->EvenKey[7]
				, pRefInfo->OddKey[0], pRefInfo->OddKey[1], pRefInfo->OddKey[2], pRefInfo->OddKey[3], pRefInfo->OddKey[4], pRefInfo->OddKey[5], pRefInfo->OddKey[6], pRefInfo->OddKey[7]
				);

			if(pRefInfo->ReferenceID == VD_REF_ID_ERROR)
				break;
		}
	}

#if defined (CONFIG_WA_2SC_SAME_ENTITLEMENT)	// ewlee 20100825
	for(i=0; i< HMX_VD_DMX_MAX_NUM; i++)
	{
		if (DemuxInfo[i].DmxId == VD_RSC_ID_INVALID)
			continue;

		HLIB_CMD_Printf("[%d] Demux ID %d\n", i, DemuxInfo[i].DmxId);
		for (j = 0; j < HMX_VD_DSC_REFID_MAX; j++)
		{
			if (DemuxInfo[i].RefIdPID_List[j].ulSCellRefId == VD_REF_ID_ERROR)
				continue;

			HLIB_CMD_Printf("SoftCell RefID : %d\n", DemuxInfo[i].RefIdPID_List[j].ulSCellRefId);

			for (k = 0; k < HMX_VD_REF_PID_MAX; k++)
			{
				if (DemuxInfo[i].RefIdPID_List[j].ulHmxRefId[k] == VD_REF_ID_ERROR)
					continue;
				HLIB_CMD_Printf("\t HUMAX RefID : %d	TS PID : 0x%04X\n", DemuxInfo[i].RefIdPID_List[j].ulHmxRefId[k], DemuxInfo[i].RefIdPID_List[j].usPID[k]);
			}
		}
	}

#endif
	DESC_SEM_RELEASE();
}
#endif



/* end of file */
