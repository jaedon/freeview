/*
 * (c) 2011-2015 Humax Co., Ltd.
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
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies
 * of the Software including all documentation. This License will terminate immediately without notice from Humax
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
 * of the Software and all documentation.

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
 * in the Republic of Korea.
*/
/**
	@file     xsvc_cas_vmx_dscr.c
	@brief    xsvc_cas_vmx_dscr.c (Verimatrix DSCR)

	Description:   \n
	Module: MW/CAS/ VERIMATRIX \n
	Remarks :\n

	Copyright (c) 2015 HUMAX Co., Ltd. \n
	All rights reserved. \n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include "_svc_cas_mgr_common.h"
#include "_svc_cas_mgr_pal_port.h"

#include "_xsvc_cas_vmx_dscr.h"
#include "_xsvc_cas_vmx_os.h"

#if defined(CONFIG_MW_CAS_VERIMATRIX_IPTV)
#include "_xsvc_cas_vmx_iptv_main.h"
#elif defined(CONFIG_MW_CAS_VERIMATRIX_DVB)
#include "_xsvc_cas_vmx_bc_main.h"
#endif

#include "pal_pipe.h"
#include "vkernel.h"
#include "pal_dsc.h"

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define VMX_DSCR_SLOT_NUM 				(4 * 9)	// 한 서비스당 4개 es가 있다고 가정하고 최대 9개 세션  	// DDI_DSC_DESCRAMBLER_NO in ddi_dsc.c

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
/*
typedef enum
{
	eSvcXxx_Type,

} xsvcXxx_Type_e;

typedef  struct
{
	...
} xsvcXxx_Type_t;
*/

typedef struct {
	HUINT32 ulVmxIptvStreamIdx;
	HUINT32 ulDscrHandle;
	HUINT16 usPid;
	void *pUserData;
} VMX_Dscr_Slot_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC VMX_Dscr_Slot_t 	*s_pstVmxDscrSlot;

STATIC HULONG			s_ulVmxDscrSemId;

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/
/*
// Local function
STATIC HERROR xsvc_xxx_DoSomething_Yyy( ... )
{
	...
}
*/

/*
// Global function
HERROR xsvc_xxx_DoSomething_Yyy( ... )
{
	...
}
*/

STATIC HERROR vmx_dscr_Init(void)
{
	int nErr;
	int i;

	HxLOG_Print("[Enter] \n");

	nErr =VMX_SEM_Create("VmxDscr", &s_ulVmxDscrSemId);
	if (nErr)
	{
		HxLOG_Error("failed to create semaphore (%d)\n", nErr);
		return ERR_FAIL;
	}

	s_pstVmxDscrSlot = (VMX_Dscr_Slot_t *)VMX_MEM_Alloc(sizeof(VMX_Dscr_Slot_t) * VMX_DSCR_SLOT_NUM);

	for (i = 0; i < VMX_DSCR_SLOT_NUM; i++)
	{
		s_pstVmxDscrSlot[i].ulVmxIptvStreamIdx = HANDLE_NULL;
	}

	HxLOG_Print("[Exit]\n");

	return svc_cas_DevDscInit();
}


STATIC HERROR vmx_dscr_ChangePid(HUINT32 ulDscrHandle, HUINT16 usNewPid)
{
	HERROR nErr;

	HxLOG_Print("[Enter]\n");

	nErr = svc_cas_DevDscSetPid(ulDscrHandle, usNewPid);
	if (nErr != ERR_OK)
	{
		HxLOG_Error("DevDscSetPid() error! = %d\n", nErr);
		return ERR_FAIL;
	}
	HxLOG_Print("[Exit]\n");

	return ERR_OK;
}

STATIC HERROR vmx_dscr_LinkWithPid(HUINT32 *pulDscrHandle, HUINT16 usPid, HUINT32 ulActionId)
{
	HERROR 			nErr;
	HUINT32	ulDeviceId;

	HxLOG_Print("[Enter]link pid:%x\n", usPid);

	if (pulDscrHandle == NULL)
	{
		HxLOG_Error("pulDscrHandle=NULL\n");
		return ERR_FAIL;
	}
	nErr = svc_cas_SubSvcRmGetResourceId(ulActionId, eRxRSCTYPE_DEMUX, &ulDeviceId);
	if (nErr != ERR_OK)
	{
		HxLOG_Print("[dscOpen] svc_cas_SubSvcRmGetResourceId error:0x%x\n", nErr);
		return ERR_FAIL;
	}
	// ePAL_DSC_DATATYPE_PSIPES --> 이거 별 의미 없어 보임..
	nErr = svc_cas_DevDscOpen((HUINT32)ulDeviceId, ePAL_DSC_DSCTYPE_DVB, ePAL_DSC_DATATYPE_PSIPES, pulDscrHandle);
	if (nErr != ERR_OK)
	{
		HxLOG_Error("svc_cas_DevDscOpen() error! = %d\n", nErr);
		return ERR_FAIL;
	}

	nErr = svc_cas_DevDscSetPid(*pulDscrHandle, usPid);
	if (nErr != ERR_OK)
	{
		HxLOG_Error("svc_cas_DevDscSetPid() error! = %d\n", nErr);
		return ERR_FAIL;
	}
	HxLOG_Print("[Exit][%x]\n", *pulDscrHandle);
	return ERR_OK;
}

STATIC HERROR vmx_dscr_Unlink(HUINT32 ulDscrHandle, HUINT16 usPid)
{
	HERROR 	nErr;

	HxLOG_Print("[Enter][%x]link pid:%x\n", ulDscrHandle, usPid);

	nErr =  svc_cas_DevDscClose(ulDscrHandle);
	if (nErr != ERR_OK)
	{
		HxLOG_Error("svc_cas_DevDscClose() error! = %d\n", nErr);
		return ERR_FAIL;
	}

	HxLOG_Print("[Exit]\n");
	return ERR_OK;
}

STATIC HERROR vmx_dscr_Close(HUINT32 ulStbDscrHandle, HUINT16 usPid)
{
	HUINT32 nIdx;

	HxLOG_Print("[Enter][%x]\n", ulStbDscrHandle);

	nIdx = (int)ulStbDscrHandle;

	VMX_SEM_Get(s_ulVmxDscrSemId);

	// ulStbDscrHandle does not exist
	if (nIdx >= VMX_DSCR_SLOT_NUM || s_pstVmxDscrSlot[nIdx].ulVmxIptvStreamIdx == HANDLE_NULL)
	{
		VMX_SEM_Release(s_ulVmxDscrSemId);
		HxLOG_Error("wrong dwStbStreamHandle or already closed\n");
		return ERR_FAIL;
	}

	// close descrambler
 	HxLOG_Print("[%x %x %x]\n", s_pstVmxDscrSlot[ulStbDscrHandle].ulVmxIptvStreamIdx, s_pstVmxDscrSlot[ulStbDscrHandle].ulDscrHandle, s_pstVmxDscrSlot[ulStbDscrHandle].usPid);

	if (vmx_dscr_Unlink(s_pstVmxDscrSlot[nIdx].ulDscrHandle, s_pstVmxDscrSlot[nIdx].usPid) != ERR_OK)
	{
		VMX_SEM_Release(s_ulVmxDscrSemId);
		HxLOG_Error("vmx_dscr_Unlinka error\n");
		return ERR_FAIL;
	}

	s_pstVmxDscrSlot[nIdx].ulVmxIptvStreamIdx = HANDLE_NULL;
	s_pstVmxDscrSlot[nIdx].ulDscrHandle = HANDLE_NULL;

	if (s_pstVmxDscrSlot[nIdx].pUserData)
	{
		VMX_MEM_Free(s_pstVmxDscrSlot[nIdx].pUserData);
	}

	VMX_SEM_Release(s_ulVmxDscrSemId);

	HxLOG_Print("[Exit]\n");
	return ERR_OK;
}

STATIC HUINT32 vmx_dscr_Open(Handle_t hEcm, HUINT32 ulVmxIptvStreamIdx, HUINT16 usPid, HUINT32 ulActionId)
{
	HUINT32 	ulStbDscrHandle;
	int 		i;

	HxLOG_Print("[Enter][%x] \n", usPid);

	VMX_SEM_Get(s_ulVmxDscrSemId);

	// allocate descramler
	ulStbDscrHandle = HANDLE_NULL;

	// check if a slot has been opened for dwStbStreamHandle
	for (i = 0; i <VMX_DSCR_SLOT_NUM; i++)
	{
		if (s_pstVmxDscrSlot[i].ulVmxIptvStreamIdx == ulVmxIptvStreamIdx)
		{
			HxLOG_Print("ulVmxIptvStreamIdx is already assigned to a descrambler slot(%d, %x, %x)\n", i, s_pstVmxDscrSlot[i].ulVmxIptvStreamIdx, s_pstVmxDscrSlot[i].ulDscrHandle);

			if (s_pstVmxDscrSlot[i].ulDscrHandle != HANDLE_NULL)
			{
				if (s_pstVmxDscrSlot[i].usPid != usPid)
				{
					vmx_dscr_ChangePid(s_pstVmxDscrSlot[i].ulDscrHandle, usPid);
					s_pstVmxDscrSlot[i].usPid = usPid;
				}

				VMX_SEM_Release(s_ulVmxDscrSemId);
				return i;
			}
			else
			{
				// strange situation
				s_pstVmxDscrSlot[i].ulVmxIptvStreamIdx = HANDLE_NULL;
			}

			break;
		}
	}

	// find a slot
	for (i = 0; i < VMX_DSCR_SLOT_NUM; i++)
	{
		if (s_pstVmxDscrSlot[i].ulVmxIptvStreamIdx == HANDLE_NULL)
		{
			HxLOG_Print("slot[%d] is allocated\n", i);
			s_pstVmxDscrSlot[i].usPid = PID_NULL;
			s_pstVmxDscrSlot[i].pUserData = NULL;
			s_pstVmxDscrSlot[i].ulVmxIptvStreamIdx = ulVmxIptvStreamIdx;
			s_pstVmxDscrSlot[i].ulDscrHandle = HANDLE_NULL;
			ulStbDscrHandle = i;
			break;
		}
	}

	// fail to allocate descrambler
	if (ulStbDscrHandle == HANDLE_NULL)
	{
		HxLOG_Error("failed to allocate a descrambler\n");
		VMX_SEM_Release(s_ulVmxDscrSemId);
		return HANDLE_NULL;
	}

	// set pid
	if (INVALID_PID(usPid))
	{
		VMX_SEM_Release(s_ulVmxDscrSemId);
		HxLOG_Error("wrong dwStbStreamHandle\n");
		return HANDLE_NULL;
	}

	if (s_pstVmxDscrSlot[ulStbDscrHandle].usPid != usPid)
	{
		if (vmx_dscr_LinkWithPid(&s_pstVmxDscrSlot[ulStbDscrHandle].ulDscrHandle, usPid, ulActionId) !=  ERR_OK)
		{
			VMX_SEM_Release(s_ulVmxDscrSemId);
			// it failed to open a descrambler slot : it shouln't be happend!!
			HxLOG_Error("vmx_dscr_LinkWithPid error\n");
			return HANDLE_NULL;
		}

		s_pstVmxDscrSlot[ulStbDscrHandle].usPid = usPid;
	}

	HxLOG_Print("ulStbDscrHandle = %d(%d), pid = 0x%x\n", ulStbDscrHandle, s_pstVmxDscrSlot[ulStbDscrHandle].ulDscrHandle, usPid);

	VMX_SEM_Release(s_ulVmxDscrSemId);
	HxLOG_Print("[Exit][%x]\n", s_pstVmxDscrSlot[ulStbDscrHandle].ulDscrHandle);
	return ulStbDscrHandle;
}

HERROR xsvc_vmx_dscr_Init(void)
{
	return vmx_dscr_Init();
}

HUINT32 xsvc_vmx_dscr_Open(Handle_t hEcm, HUINT32 ulVmxIptvStreamIdx, HUINT16 usPid, HUINT32 ulActionId)
{
	return vmx_dscr_Open(hEcm, ulVmxIptvStreamIdx, usPid, ulActionId);
}

HERROR xsvc_vmx_dscr_Close(HUINT32 ulStbDscrHandle, HUINT16 usPid)
{
	return vmx_dscr_Close(ulStbDscrHandle, usPid);
}

void xsvc_vmx_dscr_ChangePid(HUINT32 ulVmxIptvStreamIdx, HUINT16 usPid)
{
	HUINT32 i;

	HxLOG_Print("[Enter]\n");

	VMX_SEM_Get(s_ulVmxDscrSemId);

	if (ulVmxIptvStreamIdx == HANDLE_NULL)
	{
		VMX_SEM_Release(s_ulVmxDscrSemId);
		HxLOG_Error("wrong ulStbDscrHandle\n");
		return;
	}

	// find a slot
	for (i = 0; i < VMX_DSCR_SLOT_NUM; i++)
	{
		if (s_pstVmxDscrSlot[i].ulVmxIptvStreamIdx == ulVmxIptvStreamIdx)
		{
			if (s_pstVmxDscrSlot[i].usPid != usPid)
			{
				vmx_dscr_ChangePid(s_pstVmxDscrSlot[i].ulDscrHandle, usPid);
				s_pstVmxDscrSlot[i].usPid = usPid;
			}
			break;
		}
	}

	VMX_SEM_Release(s_ulVmxDscrSemId);

	HxLOG_Print("[Exit]\n");
}

#if defined(CONFIG_MW_CAS_VERIMATRIX_DVB) && defined(VMX_SVC_TEST)
HERROR xsvc_vmx_dscr_VerifyValidDscrPid(HUINT16 usPid)
{
	int i;

	for (i = 0; i < VMX_DSCR_SLOT_NUM; i++)
	{
		if(s_pstVmxDscrSlot[i].usPid == usPid)
		{
			return ERR_OK;
		}
	}

	return ERR_FAIL;
}
#endif

HUINT32 xsvc_vmx_dscr_GetDscHandleFromStbHandle(Handle_t ulStbDscHandle)
{
	return s_pstVmxDscrSlot[ulStbDscHandle].ulDscrHandle;
}

#if defined(CONFIG_MW_CAS_VERIMATRIX_IPTV)
HERROR xsvc_vmx_dscr_SetDemuxId(HUINT32 ulDemuxId)
{
	return svc_cas_PalPortDscSetVmxIptvDemuxId(ulDemuxId);
}

HERROR xsvc_vmx_dscr_SetEncryptionAlgorithm(HUINT8 ucSvcIdx, HUINT8 ucEncAlgo)
{
	return svc_cas_DeDscSetVmxIptvEncryptionAlgorithm(ucSvcIdx, ucEncAlgo);
}
#endif

/* end of file */
