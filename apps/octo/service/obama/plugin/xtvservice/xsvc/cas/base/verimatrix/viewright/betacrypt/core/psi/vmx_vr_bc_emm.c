/* (c) 2011-2015 Humax Co., Ltd. 
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
	@file     vmx_vr_bc_ecm.c
	@brief    vmx_vr_bc_ecm.c (Verimatrix BC EMM)

	Description:   \n
	Module: MW/CAS/ VERIMATRIX \n
	Remarks :\n

	Copyright (c) 2015 HUMAX Co., Ltd. \n
	All rights reserved. \n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include "_xsvc_cas_vmx_bc_main.h"
#include "_xsvc_cas_vmx_psi.h"
#include "_xsvc_cas_vmx_os.h"
#include "vmx_vr_bc_api.h"

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

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

typedef struct _VmxBcEmmBufNode_t
{
	HUINT8 	ucBuf[1024];	/* one section size is 1024 byte */
	HUINT16 usSize;
	struct _VmxBcEmmBufNode_t 	*pNextNode;
} VmxBc_EmmBufNode_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

STATIC VmxBc_EmmBufNode_t		s_stVmxBcEmmBuf;
STATIC HUINT8 	TempReadEmmBuf[1024];	/* one section size is 1024 byte */

STATIC HULONG 			s_ulVmxEmmBufSemId;

STATIC void bc_emm_FlushEmmBuf(void);
STATIC HERROR bc_emm_WriteEmmBuf(HUINT8 * pucEmmBuf, HUINT16 usEmmSize);
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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define DEF_EMM_FILTERING__________________________________________________________________________________
STATIC HERROR bc_emm_SetEmmPid(HUINT16 usEmmPid)
{
	HERROR ret = ERR_OK;

	HxLOG_Print(DBG_PRINT, ("[Enter] usEmmPid(0x%x)\r\n", usEmmPid);

	if(usEmmPid > 0x1FFF)
	{
		HxLOG_Error(DBG_ASSERT, ("null handle\n");
		return ERR_FAIL;
	}

	if(usEmmPid == 0x1FFF)
	{
		ret = xsvc_vmx_psi_emm_Delete();
	}
	else
	{
		ret = xsvc_vmx_psi_emm_SetEmmPid(usEmmPid);
	}

	HxLOG_Print(DBG_PRINT, ("[Exit] ret(%d)\n", ret);

	return ret;
}

void bc_emm_printEmmAddress(HUINT8 ucAllFlag, HUINT8 ucFilterIndex)
{
	Vmx_Emm_t		*pEmm = NULL;
	Vmx_EmmAddr_t	*pEmmAdd = NULL;
	int i, j;

	pEmm = xsvc_vmx_psi_emm_GetEmmInfo();
	if(pEmm == NULL)
	{
		VmxBcEmmError(DBG_ASSERT, ("\n"));
		return;
	}

	HxLOG_Print ("[%s:%d] [Enter]\n", __FUNCTION__, __LINE__);

	for(i=0;i<31;i++)
	{
		if( (ucAllFlag==FALSE && i != ucFilterIndex) )
		{
			continue;
		}

		pEmmAdd = &pEmm->stEmmAddr[i];
		if(pEmmAdd == NULL)
		{
			VmxBcEmmError(DBG_ASSERT, ("\n"));
			return;
		}

		HxLOG_Print ("idx = %d\n", i);
		HxLOG_Print ("\tucFilterIndex = %d\n", pEmmAdd->ucFilterIndex);
		HxLOG_Print ("\teState = %d\n", pEmmAdd->eState);
		HxLOG_Print ("\tusUniqueId = 0x%x\n", pEmmAdd->usUniqueId);
		HxLOG_Print ("\tadd = { ");
		for(j=0;j<4;j++)
		{
			HxLOG_Print ("0x%02x ", pEmmAdd->stEmmFilterAdd.address[j]);
		}

		HxLOG_Print (" }\n");
	}
	HxLOG_Print ("\n");
	HxLOG_Print ("[%s:%d] [Exit]\n", __FUNCTION__, __LINE__);
}

STATIC HERROR bc_emm_SetEmmFilter(HUINT8 ucFilterIndex, HUINT8 ucAddressLength, HUINT8* pucAddress)
{
	HUINT8 			ucFirstAdd;
	Vmx_Emm_t		*pEmm = NULL;
	Vmx_EmmAddr_t	*pEmmAdd = NULL;

	HxLOG_Print(DBG_PRINT, ("[Enter] ucFilterIndex(%d), \n", ucFilterIndex);
	xsvc_vmx_main_PrintDump(VMX_DBG_VR_BC_PSI_EMM, "SetEMMFilter", pucAddress, ucAddressLength);

	if(pucAddress == NULL)
	{
		HxLOG_Error(DBG_ASSERT, ("\n");
		return ERR_FAIL;
	}

	ucFirstAdd = pucAddress[0];

	if(ucAddressLength == 1 && ucFirstAdd == 0xff)
	{
		//bc_emm_FlushEmmBuf();
		//xsvc_vmx_psi_emm_Delete();
		 xsvc_vmx_psi_emm_DeleteEmmAdd_ByFilterIndex(ucFilterIndex);
		HxLOG_Print(DBG_PRINT, ("[Exit]\n");
		return ERR_OK;
	}

	pEmm = xsvc_vmx_psi_emm_GetEmmInfo();
	if(pEmm == NULL)
	{
		HxLOG_Error(DBG_ASSERT, ("\n");
		return ERR_OK;//ERR_FAIL;
	}

	pEmmAdd = &pEmm->stEmmAddr[ucFilterIndex];
	if(pEmmAdd == NULL)
	{
		HxLOG_Error(DBG_ASSERT, ("\n");
		return ERR_FAIL;
	}

	memset(&pEmmAdd->aucFilter, 0x00, sizeof(pEmmAdd->aucFilter));
	memset(&pEmmAdd->aucMask, 0x00, sizeof(pEmmAdd->aucMask));

	pEmmAdd->aucFilter[0] 	= VMX_EMM_TABLE_ID;
	pEmmAdd->aucMask[0] 	= 0xff;

	pEmmAdd->aucFilter[3] 	= ucFilterIndex;
	pEmmAdd->aucMask[3] 	= 0xff;

	pEmmAdd->aucFilter[5] 	= pucAddress[0];
	pEmmAdd->aucMask[5] 	= 0xff;

	VMX_memcpy(&pEmmAdd->stEmmFilterAdd.address, pucAddress, ucAddressLength);
	pEmmAdd->ucFilterIndex = ucFilterIndex;

	HxLOG_Print(DBG_PRINT, ("[Exit]\n");

	return xsvc_vmx_psi_emm_AddByFilterIndex(pEmm, pEmmAdd->ucFilterIndex);
}

STATIC VmxDvb_EmmMatch_e bc_emm_GetMatchedEmmAddress(Vmx_Emm_t *pstEmm, HUINT8 usFilterIdx, HUINT8 * pucEmmBuf, HUINT16 usAddLen)
{
	Vmx_EmmAddr_t *pEmmAdd = NULL;	
	VmxDvb_EmmMatch_e eEmmMatch = eVmxDvb_Emm_Match;
	HUINT8 emm_index = pucEmmBuf[3];
	HUINT8 add_len = pucEmmBuf[4];

	HxLOG_Print(DBG_PRINT, ("[Enter] usFilterIdx(%d)(%d), usAddLen(%d)\n", usFilterIdx,emm_index, add_len);

	if(pstEmm == NULL)
	{
		HxLOG_Error(DBG_ASSERT, ("\n");
		return eEmmMatch;
	}

	pEmmAdd = &pstEmm->stEmmAddr[emm_index];

	if(pEmmAdd == NULL)
	{
		HxLOG_Error(DBG_ASSERT, ("\n");
		return eEmmMatch;
	}

	switch(add_len)
	{
		case 4:
			HxLOG_Print(DBG_PRINT, ("Len(%d) - (0x%x)(0x%x)\n", add_len, pEmmAdd->stEmmFilterAdd.address[3], pucEmmBuf[8]);
			if(pEmmAdd->stEmmFilterAdd.address[3] != pucEmmBuf[8])
				eEmmMatch =  eVmxDvb_Emm_NoMatch;
			break;
		case 3:
			HxLOG_Print(DBG_PRINT, ("Len(%d) - (0x%x)(0x%x)\n", add_len, pEmmAdd->stEmmFilterAdd.address[2], pucEmmBuf[7]);
			if(pEmmAdd->stEmmFilterAdd.address[2] != pucEmmBuf[7])
				eEmmMatch =  eVmxDvb_Emm_NoMatch;
			break;
		case 2:
			HxLOG_Print(DBG_PRINT, ("Len(%d) - (0x%x)(0x%x)\n", add_len, pEmmAdd->stEmmFilterAdd.address[1], pucEmmBuf[6]);
			if(pEmmAdd->stEmmFilterAdd.address[1] != pucEmmBuf[6])
				eEmmMatch =  eVmxDvb_Emm_NoMatch;
			break;
		case 1:
			HxLOG_Print(DBG_PRINT, ("Len(%d) - (0x%x)(0x%x)\n", add_len, pEmmAdd->stEmmFilterAdd.address[0], pucEmmBuf[5]);
			if(pEmmAdd->stEmmFilterAdd.address[0] != pucEmmBuf[5])
				eEmmMatch = eVmxDvb_Emm_NoMatch;
			break;
		case 0:
			HxLOG_Print(DBG_PRINT, ("Len(%d)\n", add_len);
			eEmmMatch =  eVmxDvb_Emm_Match;
			break;
		default :
			eEmmMatch =  eVmxDvb_Emm_NoMatch;
			break;
	}

	HxLOG_Print(DBG_PRINT, ("[Exit] %s \n", eEmmMatch==eVmxDvb_Emm_Match?"Good Match":"No Match");

	return eEmmMatch;
}

STATIC void bc_emm_SaveEmmData(Vmx_Emm_t *pstEmm, HUINT8 *pucData, HUINT16 usSize)
{
	HUINT16  usEmmTotalLen = 0;
	HUINT16  usEmmEachLen = 0;
	HUINT8 ucTableId = 0;
	HUINT16  usCaSectionLen =0;
	HUINT8 filter_index = 0;
	Vmx_EmmAddr_t	*pEmmAdd = NULL;
	
	ucTableId = pucData[0];

	HxLOG_Print(DBG_PRINT, ("[Enter]\n");

	if(pstEmm == NULL)
	{
		HxLOG_Error(DBG_ASSERT, ("\n");
		return;	
	}

	if(ucTableId!=VMX_EMM_TABLE_ID) // emm table id
	{
		HxLOG_Error(DBG_ASSERT, ("\n");
		return;	
	}

	for(usEmmTotalLen=0; usEmmTotalLen<usSize; usEmmTotalLen+=usEmmEachLen)
	{		
		if(pucData[usEmmTotalLen]!=VMX_EMM_TABLE_ID) /* emm ucTableId */
		{
			break;
		}

		usCaSectionLen = VMXGETBIT16(&pucData[usEmmTotalLen+1], 0, 12);

		filter_index = VMXGETBIT8(&pucData[usEmmTotalLen+3], 0, 5);

		usEmmEachLen = usCaSectionLen+3; /*emm size until  usCaSectionLen*/		

		HxLOG_Print(DBG_PRINT, ("totalLen(%d) - filter_index(%d)(%d) <--  same\n", usCaSectionLen, pstEmm->stEmmAddr[filter_index].ucFilterIndex, filter_index);
#if 0
		if(pstEmmAdd->ucFilterIndex != filter_index)
		{
			//HxLOG_Print(DBG_PRINT, ("[Error]FilterIndex is not matched\n");
			break;
		}
#endif		

		if(bc_emm_GetMatchedEmmAddress(pstEmm, filter_index, &pucData[usEmmTotalLen], usEmmEachLen) == eVmxDvb_Emm_Match)
		{
			bc_emm_WriteEmmBuf(&pucData[usEmmTotalLen], usEmmEachLen);
		}				
	}

	HxLOG_Print(DBG_PRINT, ("[Exit]\n");	
	return;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define DEF_EMM_BUFFER__________________________________________________________________________________

STATIC VmxBc_EmmBufNode_t * bc_emm_GetEmmBuf(void)
{
	return &s_stVmxBcEmmBuf;
}

STATIC HERROR bc_emm_EmmBufInit(void)
{
	bc_emm_FlushEmmBuf();
	s_stVmxBcEmmBuf.pNextNode 	= NULL;
	return ERR_OK;
}

STATIC HERROR bc_emm_WriteEmmBuf(HUINT8 * pucEmmBuf, HUINT16 usEmmSize)
{
	VmxBc_EmmBufNode_t *pNode, *pNewNode;

	VMX_SEM_Get(s_ulVmxEmmBufSemId);

	pNewNode = VMX_MEM_Alloc(sizeof(VmxBc_EmmBufNode_t));
	if (pNewNode == NULL)
	{
		VMX_SEM_Release(s_ulVmxEmmBufSemId);
		HxLOG_Error(DBG_ASSERT, ("alloc fail\r\n");
		return ERR_FAIL;
	}

	VMX_memset(&pNewNode->ucBuf, 0, 1024);
	VMX_memcpy(&pNewNode->ucBuf, pucEmmBuf, usEmmSize);
	pNewNode->usSize 	= usEmmSize;
	pNewNode->pNextNode 	= NULL;

	// 가장 마지막 node를 찾는다
	pNode = bc_emm_GetEmmBuf();

	while (pNode->pNextNode != NULL)
	{
		pNode = pNode->pNextNode;
	}

	pNode->pNextNode = pNewNode;

	VMX_SEM_Release(s_ulVmxEmmBufSemId);

	return ERR_OK;
}

STATIC HERROR bc_emm_ReadEmmBuf(HUINT8* pabBuffer, HUINT16* pwLen, HBOOL *pbError)
{
	VmxBc_EmmBufNode_t *pFirstNode, *pNode;

	HxLOG_Print(DBG_PRINT, ("[Enter]\n");

	*pbError = FALSE;

	VMX_SEM_Get(s_ulVmxEmmBufSemId);

	pNode =  bc_emm_GetEmmBuf();
	if(pNode == NULL)
	{
		VMX_SEM_Release(s_ulVmxEmmBufSemId);
		HxLOG_Error(DBG_ASSERT, ("pNode is null\r\n");
		* pwLen = 0;
		return ERR_OK;
	}

	pFirstNode = pNode->pNextNode;
	if (pFirstNode == NULL)
	{
		// 남아 있는 node가 없음
		VMX_SEM_Release(s_ulVmxEmmBufSemId);
		HxLOG_Print(DBG_ASSERT, ("Emm QUEUE EMPTY\r\n");
		* pwLen = 0;
		return ERR_OK;
	}

	if(*pwLen < pFirstNode->usSize)
	{
		VMX_SEM_Release(s_ulVmxEmmBufSemId);
		HxLOG_Print(DBG_ASSERT, ("Emm size is bigger than the maximum allowed length\n");
		*pbError = TRUE;
		* pwLen = 0;
		return ERR_FAIL;
	}

	// 첫번째 node의 값을 pNode에 복사해준다
	VMX_memset(&TempReadEmmBuf, 0, sizeof(TempReadEmmBuf));
	VMX_memcpy(&TempReadEmmBuf, pFirstNode->ucBuf, pFirstNode->usSize);
	VMX_memcpy(pabBuffer, &TempReadEmmBuf, pFirstNode->usSize);
	* pwLen = pFirstNode->usSize;

	pNode->pNextNode = pFirstNode->pNextNode;

	VMX_MEM_Free(pFirstNode);

	VMX_SEM_Release(s_ulVmxEmmBufSemId);

	HxLOG_Print(DBG_PRINT, ("[Exit]\n");

	return ERR_OK;;
}

STATIC HERROR bc_emm_FreeEmmBuf(void)
{
	VmxBc_EmmBufNode_t *pFirstNode, *pNode;

	HxLOG_Print(DBG_PRINT, ("[Enter]\n");

	VMX_SEM_Get(s_ulVmxEmmBufSemId);

	pNode =  bc_emm_GetEmmBuf();
	if(pNode == NULL)
	{
		VMX_SEM_Release(s_ulVmxEmmBufSemId);
		HxLOG_Error(DBG_ASSERT, ("pNode is null\r\n");
		return ERR_FAIL;
	}

	pFirstNode = pNode->pNextNode;
	if (pFirstNode == NULL)
	{
		// 남아 있는 node가 없음
		VMX_SEM_Release(s_ulVmxEmmBufSemId);
		HxLOG_Print(DBG_ASSERT, ("Emm QUEUE EMPTY\r\n");
		return ERR_FAIL;
	}

	pNode->pNextNode = pFirstNode->pNextNode;

	VMX_MEM_Free(pFirstNode);

	VMX_SEM_Release(s_ulVmxEmmBufSemId);

	HxLOG_Print(DBG_PRINT, ("[Exit]\n");

	return ERR_OK;;
}

STATIC void bc_emm_FlushEmmBuf(void)
{
	while(1)
	{
		if(bc_emm_FreeEmmBuf() != ERR_OK)
			break;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define DEF_GLOBAL__________________________________________________________________________________

HERROR vmx_vr_bc_emm_SetEmmPid(HUINT16 usEmmPid)
{
	return bc_emm_SetEmmPid(usEmmPid);
}

void vmx_vr_bc_emm_SaveEmmData(Vmx_Emm_t *pstEmm, HUINT8 *pucData, HUINT16 usSize)
{
	return bc_emm_SaveEmmData(pstEmm, pucData, usSize);
}

HERROR vmx_vr_bc_emm_SetEmmFilter(HUINT8 ucFilterIndex, HUINT8 ucAddressLength, HUINT8* pucAddress)
{
	return bc_emm_SetEmmFilter(ucFilterIndex, ucAddressLength, pucAddress);
}

HERROR vmx_vr_bc_emm_WriteEmmBuf(HUINT8 * pucEmmBuf, HUINT16 usEmmSize)
{
	return bc_emm_WriteEmmBuf(pucEmmBuf, usEmmSize);
}

HERROR vmx_vr_bc_emm_ReadEmmBuf(HUINT8* pabBuffer, HUINT16* pwLen, HBOOL *pbError)
{
	return bc_emm_ReadEmmBuf(pabBuffer, pwLen, pbError);
}

void vmx_vr_bc_emm_FlushEmmBuf(void)
{
	return bc_emm_FlushEmmBuf();
}

HERROR vmx_vr_bc_emm_SetEmmPidToBC(HUINT16 usEmmPid)
{
	return vmx_vr_bc_api_SetEmmPid(usEmmPid);
}

HERROR vmx_vr_bc_emm_Init(void)
{
	HINT32 	nResult = 0;

	nResult = VMX_SEM_Create("VmxEmmBufSem", &s_ulVmxEmmBufSemId);	
	
	if(nResult != ERR_OK)
	{
		HxLOG_Error(DBG_ASSERT, ("Failure in Semaphore Creation \r\n");
		return ERR_FAIL;
	}	
	
	bc_emm_EmmBufInit();

	return ERR_OK;
}

