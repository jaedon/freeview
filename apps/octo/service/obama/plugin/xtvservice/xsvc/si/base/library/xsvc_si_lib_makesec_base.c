/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  xsvc_si_lib_makesec_base.c
	@brief

	Description:  									\n
	Module: Porting Layer/TV Service/SI /LIBRARY		\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding 을 사용하는 source file 입니다.
* MS949 encodingを使用して source fileです。
* Quelldatei, die MS949-Codierung verwendet.
*
*/
/*
 * (c) 2011-2013 Humax Co., Ltd.
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

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <hlib.h>
#include <silib.h>
#include <vkernel.h>
#include <octo_common.h>
#include <isosvc.h>

#include <pal_sirawpool.h>
#include <pal_pipe.h>
#include <svc_si.h>
#include <_svc_si.h>
#include <_svc_si_lib.h>
#include <_svc_si_filtering.h>


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define SVCSI_MAX_PMT_LOOP_NUM									(16)

#define SVCSI_PAT_SECTION_LENGTH								(20-4)
#define SVCSI_PMT_SECTION_LENGTH_MAX							(1024)
#define SVCSI_DTCP_DESCRIPTOR_LEN								(6)
#define SVCSI_SIT_DESCRIPTOR_BUFFER_MAX							(512)
#define SVCSI_SIT_SECTION_HEADER_LENGTH							(10)
#define SVCSI_SIT_SECTION_LENGTH								(4096)
#define SVCSI_SIT_SERVICE_LOOP_HEADER_LENGTH					(4)

#define SVCSI_REPLACEMENT_PAT_SIZE								(16)

#define SVCSI_SECTION_TID           							(0)              	/* TID index                    			*/
#define SVCSI_SECTION_LEN_HI        							(1)              	/* length hi index              		*/
#define SVCSI_SECTION_LEN_LO        							(2)              	/* length lo index              		*/
#define SVCSI_SECTION_TSID_HI	  								(3)              	/* Transport Stream ID high index	*/
#define SVCSI_SECTION_TSID_LO    								(4)              	/* Transport Stream ID low index 	*/
#define SVCSI_SECTION_VERSION       							(5)              	/* version number index        		*/
#define SVCSI_SECTION_NUM           							(6)              	/* section number index         		*/
#define SVCSI_SECTION_NUM_LAST      							(7)              	/* last section number index    	*/

/* PAT only */
#define SVCSI_SECTION_PROGRAM0_NUM_HI							(8)
#define SVCSI_SECTION_PROGRAM0_NUM_LO							(9)
#define SVCSI_SECTION_NETWORK_PID_HI							(10)
#define SVCSI_SECTION_NETWORK_PID_LO							(11)
#define SVCSI_SECTION_PROGRAM1_NUM_HI							(12-4)
#define SVCSI_SECTION_PROGRAM1_NUM_LO							(13-4)
#define SVCSI_SECTION_PMT_PID_HI								(14-4)
#define SVCSI_SECTION_PMT_PID_LO								(15-4)

/* PMT only */
#define SVCSI_SECTION_PROGRAM_NUM_HI							(3)
#define SVCSI_SECTION_PROGRAM_NUM_LO							(4)
#define SVCSI_SECTION_PCR_PID_HI								(8)
#define SVCSI_SECTION_PCR_PID_LO								(9)
#define SVCSI_SECTION_PROGRAM_LEN_HI							(10)
#define SVCSI_SECTION_PROGRAM_LEN_LO							(11)
#define SVCSI_SECTION_PMT_HEADER_LENGTH							(12)


#define SVCSI_SECTION_CRC32_1									(16-4)
#define SVCSI_SECTION_CRC32_2									(17-4)
#define SVCSI_SECTION_CRC32_3									(18-4)
#define SVCSI_SECTION_CRC32_4									(19-4)

#define SVCSI_SECTION_LEN_MASK      							(0x0FFF)			/* length bit mask              		*/
#define SVCSI_SECTION_VERSION_MASK  							(0x3E)				/* version number bit mask      	*/
#define SVCSI_SECTION_VERSION_SHIFT 							(1)					/* version number bit shift     		*/
#define SVCSI_SECTION_CNI_MASK      							(0x01)				/* current next indicator mask  	*/
#define SVCSI_PCR_PID_MASK_HI      								(0x1F00)			/* mask for high bits of PCR PID 	*/
#define SVCSI_PCR_PID_MASK_LO       							(0x00FF)			/* mask for low bits of PCR PID 	*/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

typedef struct
{
	HUINT16				 usProgNum;
	HUINT16				 usPcrPid;

	// Program Info Descriptors
	HxList_t			*pstPmtProgramInfoDesList;		// xsvcMakeSec_RawDesInfo_t

	// Element Info:
	HxList_t			*pstPmtElementList;				// xsvcMakeSec_PmtElemInfo_t
//} _SiPacket_PmtSectionInfo_t;
} xsvcMakeSec_PmtSecInfo_t;

typedef struct
{
	HUINT32		 ulDescriptorLen;
	HUINT8		*pucDescriptor;
//} _SiPacket_RawDesInfo_t;
} xsvcMakeSec_RawDesInfo_t;

typedef struct
{
	HUINT8				 ucStreamType;
	HUINT16				 usElemPid;

	// Element Info Descriptors:
	HxList_t			*pstPmtElementInfoDesList;		// xsvcMakeSec_RawDesInfo_t
//} _SiPacket_PmtElemInfo_t;
} xsvcMakeSec_PmtElemInfo_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
#define ___LOCAL_FUNCTIONS___

STATIC xsvcMakeSec_PmtSecInfo_t *xsvc_si_LibMakeSecAllocPmtSection (void)
{
	xsvcMakeSec_PmtSecInfo_t		*pstPmtSecInfo;

	pstPmtSecInfo = (xsvcMakeSec_PmtSecInfo_t *)OxSI_Malloc (sizeof(xsvcMakeSec_PmtSecInfo_t));
	if (NULL != pstPmtSecInfo)
	{
		HxSTD_memset (pstPmtSecInfo, 0, sizeof(xsvcMakeSec_PmtSecInfo_t));
	}

	return pstPmtSecInfo;
}

STATIC xsvcMakeSec_RawDesInfo_t* xsvc_si_LibMakeSecAllocDescriptor (void)
{
	xsvcMakeSec_RawDesInfo_t		*pstDes;

	pstDes = (xsvcMakeSec_RawDesInfo_t *)OxSI_Malloc (sizeof(xsvcMakeSec_RawDesInfo_t));
	if (NULL != pstDes)
	{
		HxSTD_memset (pstDes, 0, sizeof(xsvcMakeSec_RawDesInfo_t));
	}

	return pstDes;
}

xsvcMakeSec_PmtElemInfo_t* xsvc_si_LibMakeSecAllocPmtElementInfo (void)
{
	xsvcMakeSec_PmtElemInfo_t		*pstPacketElemInfo;

	pstPacketElemInfo = (xsvcMakeSec_PmtElemInfo_t *)OxSI_Malloc (sizeof(xsvcMakeSec_PmtElemInfo_t));
	if (NULL != pstPacketElemInfo)
	{
		HxSTD_memset (pstPacketElemInfo, 0, sizeof(xsvcMakeSec_PmtElemInfo_t));
	}

	return pstPacketElemInfo;
}

STATIC void xsvc_si_LibMakeSecFreeDescriptor (xsvcMakeSec_RawDesInfo_t *pstDes)
{
	if (NULL != pstDes)
	{
		if (NULL != pstDes->pucDescriptor)
		{
			OxSI_Free (pstDes->pucDescriptor);
		}

		OxSI_Free (pstDes);
	}

	return;
}

STATIC void xsvc_si_LibMakeSecFreePmtElementInfo (xsvcMakeSec_PmtElemInfo_t *pstPacketElemInfo)
{
	HxList_t				*pstItem;

	if (NULL != pstPacketElemInfo)
	{
		pstItem = pstPacketElemInfo->pstPmtElementInfoDesList;
		while (pstItem)
		{
			xsvc_si_LibMakeSecFreeDescriptor ((xsvcMakeSec_RawDesInfo_t *)pstItem->data);
			pstItem = pstItem->next;
		}

		pstPacketElemInfo->pstPmtElementInfoDesList = pstPacketElemInfo->pstPmtElementInfoDesList;
		OxSI_Free (pstPacketElemInfo);
	}

	return;
}

STATIC void xsvc_si_LibMakeSecFreePmtSection (xsvcMakeSec_PmtSecInfo_t *pstPmtSec)
{
	HxList_t				*pstItem;

	if (NULL != pstPmtSec)
	{
		// Program Descriptors
		pstItem = pstPmtSec->pstPmtProgramInfoDesList;
		while (pstItem)
		{
			xsvc_si_LibMakeSecFreeDescriptor ((xsvcMakeSec_RawDesInfo_t *)pstItem->data);
			pstItem = pstItem->next;
		}
		pstPmtSec->pstPmtProgramInfoDesList = HLIB_LIST_RemoveAll (pstPmtSec->pstPmtProgramInfoDesList);

		// Program Elements
		pstItem = pstPmtSec->pstPmtElementList;
		while (pstItem)
		{
			xsvc_si_LibMakeSecFreePmtElementInfo ((xsvcMakeSec_PmtElemInfo_t *)pstItem->data);
			pstItem = pstItem->next;
		}
		pstPmtSec->pstPmtElementList = HLIB_LIST_RemoveAll (pstPmtSec->pstPmtElementList);

		OxSI_Free (pstPmtSec);
	}

	return;
}

STATIC HERROR xsvc_si_LibMakeSecGetPmtSectionInfoByRawSections (HUINT32 ulActionId, SvcSi_MakeSec_Pmt_t *pstPmtArg, xsvcMakeSec_PmtSecInfo_t *pstPmtPacketSec)
{
	SIxTable_PsiTable_t 		*pstPmtTable = NULL, *pstCurrPmt = NULL;
	HxList_t					*pst2ndLoop = NULL;
	HxList_t					*pstDesLoop = NULL;

	xsvcMakeSec_RawDesInfo_t	*pstPacketDes;
	xsvcMakeSec_PmtElemInfo_t	*pstPmtPacketElem;

	pstPmtTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_Pmt, SILIB_TABLE_EXT_ID_ALL);
	if (NULL == pstPmtTable)
	{
		HxLOG_Error ("PAL_SIRAWPOOL_ParseTable ulActionId=%d, PMT\n", ulActionId);
		return ERR_FAIL;
	}

	// 1. Section loop
	for (pstCurrPmt = pstPmtTable; pstCurrPmt != NULL; pstCurrPmt = pstCurrPmt->pstNextTable)
	{
		SIxTable_PmtSec_t		*pstPmtSec = &pstCurrPmt->unInfo.stPmt;

		if (pstPmtSec == NULL)								{ continue; }
		if (pstPmtSec->usProgNum != pstPmtArg->usSvcId)		{ continue; }

		pstPmtPacketSec->usPcrPid = pstPmtSec->usPcrPid;
		pstPmtPacketSec->usProgNum = pstPmtSec->usProgNum;

		// 2. Program descriptors
		for (pstDesLoop = pstCurrPmt->pstDesLoop; pstDesLoop != NULL; pstDesLoop = pstDesLoop->next)
		{
			HUINT8		*pucRawDes = HLIB_LIST_Data (pstDesLoop);
			HUINT8		 ucDesLen = 0;

			if (pucRawDes == NULL)		{ continue; }

			ucDesLen = HxMACRO_Get8Bit(pucRawDes + 1);
			if (ucDesLen == 0)			{ continue; }

			pstPacketDes = xsvc_si_LibMakeSecAllocDescriptor();
			if (NULL != pstPacketDes)
			{
				pstPacketDes->ulDescriptorLen = ucDesLen;
				pstPacketDes->pucDescriptor = pucRawDes;

				pstPmtPacketSec->pstPmtProgramInfoDesList = HLIB_LIST_Append (pstPmtPacketSec->pstPmtProgramInfoDesList, (void *)pstPacketDes);
			}
		}

		// 3. Program Elements
		for (pst2ndLoop = pstCurrPmt->pst2ndLoop; pst2ndLoop != NULL; pst2ndLoop = pst2ndLoop->next)
		{
			SIxTable_2ndLoop_t		*pst2ndItem = (SIxTable_2ndLoop_t *)HLIB_LIST_Data (pst2ndLoop);
			SIxTable_PmtElement_t	*stPmtElem = NULL;

			if (pst2ndItem == NULL)				{ continue; }

			stPmtElem = &pst2ndItem->unInfo.stPmt;
			if (stPmtElem == NULL)				{ continue; }

			pstPmtPacketElem = xsvc_si_LibMakeSecAllocPmtElementInfo();
			if (NULL != pstPmtPacketElem)
			{
				pstPmtPacketSec->pstPmtElementList = HLIB_LIST_Append (pstPmtPacketSec->pstPmtElementList, (void *)pstPmtPacketElem);

				pstPmtPacketElem->ucStreamType		= stPmtElem->ucStreamType;
				pstPmtPacketElem->usElemPid			= stPmtElem->usElemPid;

				// 4. Program Element Descriptors
				for (pstDesLoop = pst2ndItem->pstDesLoop; pstDesLoop != NULL; pstDesLoop = pstDesLoop->next)
				{
					HUINT8		*pucRawDes = HLIB_LIST_Data (pstDesLoop);
					HUINT8		 ucDesLen = 0;

					if (pucRawDes == NULL)		{ continue; }

					ucDesLen = HxMACRO_Get8Bit(pucRawDes + 1);
					if (ucDesLen == 0)			{ continue; }

					pstPacketDes = xsvc_si_LibMakeSecAllocDescriptor();
					if (NULL != pstPacketDes)
					{
						pstPacketDes->ulDescriptorLen = ucDesLen;
						pstPacketDes->pucDescriptor = pucRawDes;

						pstPmtPacketElem->pstPmtElementInfoDesList = HLIB_LIST_Append (pstPmtPacketElem->pstPmtElementInfoDesList, (void *)pstPacketDes);
					}
				}
			}
		}
	}

	if (pstPmtTable != NULL)
	{
		PAL_SIRAWPOOL_FreeTable(pstPmtTable);
	}

	// Section이 발견되었으면 ERR_OK
	return (NULL != pstPmtPacketSec->pstPmtElementList) ? ERR_OK : ERR_FAIL;
}


STATIC HERROR xsvc_si_LibMakeSecGetPmtSectionInfoBySetting (HUINT32 ulActionId, SvcSi_MakeSec_Pmt_t *pstPmtArg, xsvcMakeSec_PmtSecInfo_t *pstPmtPacketSec)
{
	xsvcMakeSec_PmtElemInfo_t		*pstPmtPacketElem;
	SvcSi_MakeSec_PmtSetting_t		*pstPmtSettingArg = &(pstPmtArg->unArgs.stSettingArg);

	// Program Number, PCR PID
	pstPmtPacketSec->usPcrPid = pstPmtSettingArg->usPcrPid;
	pstPmtPacketSec->usProgNum = pstPmtArg->usSvcId;

	// Video
	pstPmtPacketElem = xsvc_si_LibMakeSecAllocPmtElementInfo();
	if (NULL != pstPmtPacketElem)
	{
		pstPmtPacketSec->pstPmtElementList	= HLIB_LIST_Append (pstPmtPacketSec->pstPmtElementList, (void *)pstPmtPacketElem);
		pstPmtPacketElem->ucStreamType		= pstPmtSettingArg->ucVideoStreamType;
		pstPmtPacketElem->usElemPid 		= pstPmtSettingArg->usVideoPid;
	}

	// Audio
	pstPmtPacketElem = xsvc_si_LibMakeSecAllocPmtElementInfo();
	if (NULL != pstPmtPacketElem)
	{
		pstPmtPacketSec->pstPmtElementList	= HLIB_LIST_Append (pstPmtPacketSec->pstPmtElementList, (void *)pstPmtPacketElem);
		pstPmtPacketElem->ucStreamType		= pstPmtSettingArg->ucAudioStreamType;
		pstPmtPacketElem->usElemPid 		= pstPmtSettingArg->usAudioPid;
	}

	return (NULL != pstPmtPacketSec->pstPmtElementList) ? ERR_OK : ERR_FAIL;
}

STATIC HERROR xsvc_si_LibMakeSecGetPmtSectionInfoForTranscode (HUINT32 ulActionId, SvcSi_MakeSec_Pmt_t *pstPmtArg, xsvcMakeSec_PmtSecInfo_t *pstPmtPacketSec)
{
	SIxTable_PsiTable_t 		*pstPmtTable = NULL, *pstCurrPmt = NULL;
	HxList_t					*pst2ndLoop = NULL;
	HxList_t					*pstDesLoop = NULL;

	xsvcMakeSec_RawDesInfo_t	*pstPacketDes;
	xsvcMakeSec_PmtElemInfo_t	*pstPmtPacketElem;
	SvcSi_MakeSec_PmtSetting_t	*pstPmtSettingArg = &(pstPmtArg->unArgs.stSettingArg);
	HBOOL						 bFoundAudio = FALSE, bFoundVideo = FALSE, bAdd = FALSE;

	pstPmtTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_Pmt, SILIB_TABLE_EXT_ID_ALL);
	if (NULL == pstPmtTable)
	{
		HxLOG_Error ("PAL_SIRAWPOOL_ParseTable ulActionId=%d, PMT\n", ulActionId);
		return ERR_FAIL;
	}

	// 1. Section loop
	for (pstCurrPmt = pstPmtTable; pstCurrPmt != NULL; pstCurrPmt = pstCurrPmt->pstNextTable)
	{
		SIxTable_PmtSec_t		*pstPmtSec = &pstCurrPmt->unInfo.stPmt;

		if (pstPmtSec == NULL)								{ continue; }
		if (pstPmtSec->usProgNum != pstPmtArg->usSvcId)		{ continue; }

		pstPmtPacketSec->usPcrPid = pstPmtSec->usPcrPid;
		pstPmtPacketSec->usProgNum = pstPmtSec->usProgNum;

		// 2. Program descriptors
		for (pstDesLoop = pstCurrPmt->pstDesLoop; pstDesLoop != NULL; pstDesLoop = pstDesLoop->next)
		{
			HUINT8		*pucRawDes = HLIB_LIST_Data (pstDesLoop);
			HUINT8		 ucDesLen = 0;

			if (pucRawDes == NULL)		{ continue; }

			ucDesLen = HxMACRO_Get8Bit(pucRawDes + 1);
			if (ucDesLen == 0)			{ continue; }

			pstPacketDes = xsvc_si_LibMakeSecAllocDescriptor();
			if (NULL != pstPacketDes)
			{
				pstPacketDes->ulDescriptorLen = ucDesLen;
				pstPacketDes->pucDescriptor = pucRawDes;

				pstPmtPacketSec->pstPmtProgramInfoDesList = HLIB_LIST_Append (pstPmtPacketSec->pstPmtProgramInfoDesList, (void *)pstPacketDes);
			}
		}

		// 3. Program Elements
		for (pst2ndLoop = pstCurrPmt->pst2ndLoop; pst2ndLoop != NULL; pst2ndLoop = pst2ndLoop->next)
		{
			SIxTable_2ndLoop_t		*pst2ndItem = (SIxTable_2ndLoop_t *)HLIB_LIST_Data (pst2ndLoop);
			SIxTable_PmtElement_t	*stPmtElem = NULL;

			if (pst2ndItem == NULL)				{ continue; }

			stPmtElem = &pst2ndItem->unInfo.stPmt;
			if (stPmtElem == NULL)				{ continue; }

			bAdd = FALSE;
			pstPmtPacketElem = xsvc_si_LibMakeSecAllocPmtElementInfo();
			if (NULL != pstPmtPacketElem)
			{
				if ((stPmtElem->ucStreamType == ISO_VIDEO_1) ||	(stPmtElem->ucStreamType == ISO_VIDEO_2) ||	(stPmtElem->ucStreamType == ISO_VIDEO_4))
				{
					if (bFoundVideo == FALSE)	/* Transcode시에는 한 개의 Video 정보만 처리 */
					{
						bFoundVideo = TRUE;
						bAdd = TRUE;
						pstPmtPacketElem->ucStreamType	 = pstPmtSettingArg->ucVideoStreamType;
					}
					else
					{
						HxLOG_Print("[%s:%d] Already get video element=%d\n", __FUNCTION__, __LINE__);
					}
				}
				else if ((stPmtElem->ucStreamType == ISO_AUDIO_1) || (stPmtElem->ucStreamType == ISO_AUDIO_2) || (stPmtElem->ucStreamType == ISO_AUDIO_AAC) ||
						 (stPmtElem->ucStreamType == ISO_AUDIO_4))
				{
					if (bFoundAudio == FALSE)	/* Transcode시에는 한 개의 Audio 정보만 처리 */
					{
						bFoundAudio = TRUE;
						bAdd = TRUE;
						pstPmtPacketElem->ucStreamType	 = pstPmtSettingArg->ucAudioStreamType;
					}
					else
					{
						HxLOG_Print("[%s:%d] Already get audio element=%d\n", __FUNCTION__, __LINE__);
					}
				}
				else							/* Transcode시에는 Data 정보는 제외 */
				{
					HxLOG_Print("[%s:%d] This is not video and audio stream type. It is %#x \n", __FUNCTION__, __LINE__, stPmtElem->ucStreamType);
				}

				if (bAdd == TRUE)
				{
					pstPmtPacketElem->usElemPid 		= stPmtElem->usElemPid;
					pstPmtPacketSec->pstPmtElementList = HLIB_LIST_Append (pstPmtPacketSec->pstPmtElementList, (void *)pstPmtPacketElem);
				}
				else
				{
					xsvc_si_LibMakeSecFreePmtElementInfo(pstPmtPacketElem);
					pstPmtPacketElem = NULL;
					continue;
				}

				// 4. Program Element Descriptors
				for (pstDesLoop = pst2ndItem->pstDesLoop; pstDesLoop != NULL; pstDesLoop = pstDesLoop->next)
				{
					HUINT8		*pucRawDes = HLIB_LIST_Data (pstDesLoop);
					HUINT8		 ucDesLen = 0;

					if (pucRawDes == NULL)		{ continue; }

					ucDesLen = HxMACRO_Get8Bit(pucRawDes + 1);
					if (ucDesLen == 0)			{ continue; }

					pstPacketDes = xsvc_si_LibMakeSecAllocDescriptor();
					if (NULL != pstPacketDes)
					{
						pstPacketDes->ulDescriptorLen = ucDesLen;
						pstPacketDes->pucDescriptor = pucRawDes;

						pstPmtPacketElem->pstPmtElementInfoDesList = HLIB_LIST_Append (pstPmtPacketElem->pstPmtElementInfoDesList, (void *)pstPacketDes);
					}
				}
			}
		}
	}

	if (pstPmtTable != NULL)
	{
		PAL_SIRAWPOOL_FreeTable(pstPmtTable);
	}

	// Section이 발견되었으면 ERR_OK
	return (NULL != pstPmtPacketSec->pstPmtElementList) ? ERR_OK : ERR_FAIL;
}

STATIC HERROR xsvc_si_LibMakeSecWritePmtRawByPmtSectionInfo  (xsvcMakeSec_PmtSecInfo_t *pstPmtPacketSec, HUINT8 *pucPmtSec, HUINT32 *pulPmtSectionLen)
{
	static HUINT8			 s_ucVersion = 0;				/* will be incremented each time called */
	HUINT32					 ulTotalLen = SVCSI_SECTION_PMT_HEADER_LENGTH;
	HUINT32					 ulCrc32;
	HUINT32					 ulProgramInfoLen = 0, ulEsInfoLen = 0;
	HxList_t				*pstItem, *pstSubItem;
	HUINT8					*pucRaw, *pucEsInfoLenPos;

	pucPmtSec[SVCSI_SECTION_TID] = PMT_TABLE_ID;

	pucPmtSec[SVCSI_SECTION_PROGRAM_NUM_HI] = (pstPmtPacketSec->usProgNum >> 8) & 0xFF;
	pucPmtSec[SVCSI_SECTION_PROGRAM_NUM_LO] = pstPmtPacketSec->usProgNum & 0xFF;

	/*	0xC0 : reserved
		version_number
		current_next_indicator */
	pucPmtSec[SVCSI_SECTION_VERSION] = (0xC0 |((s_ucVersion << SVCSI_SECTION_VERSION_SHIFT) & SVCSI_SECTION_VERSION_MASK) | SVCSI_SECTION_CNI_MASK);

	pucPmtSec[SVCSI_SECTION_NUM] = 0;
	pucPmtSec[SVCSI_SECTION_NUM_LAST] = 0;

	/*	0xE0 : reserved
		PCR PID */
	pucPmtSec[SVCSI_SECTION_PCR_PID_HI] = (0xE0 | ((pstPmtPacketSec->usPcrPid & SVCSI_PCR_PID_MASK_HI) >> 8));
	pucPmtSec[SVCSI_SECTION_PCR_PID_LO] = (pstPmtPacketSec->usPcrPid & SVCSI_PCR_PID_MASK_LO);

	/* PMT Program Info */
	/* 한정수신기술자(0x09)를 제외하고 원래의 기술자를 복사한다. */
	pucRaw = pucPmtSec + SVCSI_SECTION_PMT_HEADER_LENGTH;

	for (pstItem = pstPmtPacketSec->pstPmtProgramInfoDesList; NULL != pstItem; pstItem = pstItem->next)
	{
		xsvcMakeSec_RawDesInfo_t			*pstPacketDes = (xsvcMakeSec_RawDesInfo_t *)HLIB_LIST_Data (pstItem);
		if ((NULL != pstPacketDes) && (NULL != pstPacketDes->pucDescriptor))
		{
			// CAS Descriptor는 비포함
			if (pstPacketDes->pucDescriptor[0] != TAG_CA)
			{
				HxSTD_memcpy (pucRaw, pstPacketDes->pucDescriptor, pstPacketDes->ulDescriptorLen);

				pucRaw += pstPacketDes->ulDescriptorLen;
				ulTotalLen += pstPacketDes->ulDescriptorLen;
				ulProgramInfoLen += pstPacketDes->ulDescriptorLen;
			}
		}
	}

	/* PMT Program Info Length */
	pucPmtSec[SVCSI_SECTION_PROGRAM_LEN_HI] = (0xF0 | ((ulProgramInfoLen & 0xF00) >> 8));
	pucPmtSec[SVCSI_SECTION_PROGRAM_LEN_LO] = (ulProgramInfoLen & 0xFF);

	/* PMT Elements */
	for (pstItem = pstPmtPacketSec->pstPmtElementList; NULL != pstItem; pstItem = pstItem->next)
	{
		xsvcMakeSec_PmtElemInfo_t			*pstPmtPacketElem = (xsvcMakeSec_PmtElemInfo_t *)HLIB_LIST_Data (pstItem);

		if (NULL == pstPmtPacketElem)					{ continue; }

		// Stream Type:
		*pucRaw ++ = pstPmtPacketElem->ucStreamType;		ulTotalLen ++;

		// Elementary PID:
		*pucRaw++ = (0xE0 | ((pstPmtPacketElem->usElemPid & 0x1F00) >> 8));
		*pucRaw++ = (pstPmtPacketElem->usElemPid & 0xFF);
		ulTotalLen += 2;
		// ES Info length: 현재 위치에 2byte 적어야 하므로 위치만을 기억해둔다.
		pucEsInfoLenPos = pucRaw;
		pucRaw += 2;
		ulTotalLen += 2;

		ulEsInfoLen = 0;
		for (pstSubItem = pstPmtPacketElem->pstPmtElementInfoDesList; NULL != pstSubItem; pstSubItem = pstSubItem->next)
		{
			xsvcMakeSec_RawDesInfo_t			*pstPacketDes = (xsvcMakeSec_RawDesInfo_t *)HLIB_LIST_Data (pstSubItem);

			if ((NULL != pstPacketDes) && (NULL != pstPacketDes->pucDescriptor))
			{
				// CAS Descriptor는 비포함
				if (pstPacketDes->pucDescriptor[0] != TAG_CA)
				{
					HxSTD_memcpy (pucRaw, pstPacketDes->pucDescriptor, pstPacketDes->ulDescriptorLen);

					pucRaw += pstPacketDes->ulDescriptorLen;
					ulTotalLen += pstPacketDes->ulDescriptorLen;
					ulEsInfoLen += pstPacketDes->ulDescriptorLen;
				}
			}
		}

		// ES Info Length
		pucEsInfoLenPos[0] = (0xF0 | ((ulEsInfoLen & 0xF00) >> 8));
		pucEsInfoLenPos[1] = (ulEsInfoLen & 0xFF);
	}

	ulTotalLen += 4;

	// PMT Section Length
	pucPmtSec[SVCSI_SECTION_LEN_HI] = (0x80 | 0x30 | ((ulTotalLen - 3) & 0xF00) >> 8);
	pucPmtSec[SVCSI_SECTION_LEN_LO]= ((ulTotalLen - 3) & 0xFF);

	ulCrc32 = HLIB_MATH_GetCrc32 (pucPmtSec, (ulTotalLen - 4));
	*pucRaw++ = (HUINT8)((ulCrc32 >> 24) & 0x00FF);
	*pucRaw++ = (HUINT8)((ulCrc32 >> 16) & 0x00FF);
	*pucRaw++ = (HUINT8)((ulCrc32 >> 8) & 0x00FF);
	*pucRaw++ = (HUINT8)((ulCrc32) & 0x00FF);

	s_ucVersion = (s_ucVersion + 1) & 0x1F;					/* New version - must be in range 0 ~ 31 */

	*pulPmtSectionLen = ulTotalLen;
	return ERR_OK;

}

STATIC HERROR xsvc_si_LibMakeSecMakePatSection (HUINT32 ulActionId, SvcSi_MakeSec_Pat_t *pstPatArg, HUINT8 *pucSecRaw, HUINT32 *pulSecLen)
{
	STATIC HUINT8		 s_ucVersion = 0;			/* will be incremented each time called */
	HUINT32				 ulCrc32;

	pucSecRaw[SVCSI_SECTION_TID] = PAT_TABLE_ID;

	/*	0x80 : section_syntax_indicator, 0
		0x30 : reserved
		-3 : minus table_id, section_syntax_indicator, 0, reserved, section_length from total length */
	pucSecRaw[SVCSI_SECTION_LEN_HI] = (0x80 | 0x30 | ((SVCSI_PAT_SECTION_LENGTH - 3) & 0xF00) >> 8);
	pucSecRaw[SVCSI_SECTION_LEN_LO] = ((SVCSI_PAT_SECTION_LENGTH - 3) & 0xFF);

	pucSecRaw[SVCSI_SECTION_TSID_HI] = (pstPatArg->usTsId & 0xFF00) >> 8;
	pucSecRaw[SVCSI_SECTION_TSID_LO] = (pstPatArg->usTsId & 0xFF);

	/*	0xC0 : reserved
		version_number
		current_next_indicator */
	pucSecRaw[SVCSI_SECTION_VERSION] = (0xC0 | ((s_ucVersion << SVCSI_SECTION_VERSION_SHIFT) & SVCSI_SECTION_VERSION_MASK) | SVCSI_SECTION_CNI_MASK);

	pucSecRaw[SVCSI_SECTION_NUM] = 0;
	pucSecRaw[SVCSI_SECTION_NUM_LAST] = 0;

	/* Program0 SIT의 PID를 고정으로 넣는다. */
/*
	pucSecRaw[SVCSI_SECTION_PROGRAM0_NUM_HI] = 0;
	pucSecRaw[SVCSI_SECTION_PROGRAM0_NUM_LO] = 0;

	pucSecRaw[SVCSI_SECTION_NETWORK_PID_HI] = 0xE0;
	pucSecRaw[SVCSI_SECTION_NETWORK_PID_LO] = EIT_PID;
*/
	/* Program1 */
	pucSecRaw[SVCSI_SECTION_PROGRAM1_NUM_HI] = (HUINT8)((pstPatArg->usSvcId >> 8) & 0x00FF);
	pucSecRaw[SVCSI_SECTION_PROGRAM1_NUM_LO] = (HUINT8)(pstPatArg->usSvcId & 0x00FF);

	pucSecRaw[SVCSI_SECTION_PMT_PID_HI] = (HUINT8)(0xE0 | ((pstPatArg->usPmtPid >> 8) & 0x001F));
	pucSecRaw[SVCSI_SECTION_PMT_PID_LO] = (HUINT8)((pstPatArg->usPmtPid) & 0x00FF);

	ulCrc32 = HLIB_MATH_GetCrc32(pucSecRaw, SVCSI_PAT_SECTION_LENGTH - 4);
	pucSecRaw[SVCSI_SECTION_CRC32_1] = (HUINT8)((ulCrc32 >> 24) & 0x00FF);
	pucSecRaw[SVCSI_SECTION_CRC32_2] = (HUINT8)((ulCrc32 >> 16) & 0x00FF);
	pucSecRaw[SVCSI_SECTION_CRC32_3] = (HUINT8)((ulCrc32 >> 8) & 0x00FF);
	pucSecRaw[SVCSI_SECTION_CRC32_4] = (HUINT8)((ulCrc32) & 0x00FF);

	*pulSecLen = SVCSI_PAT_SECTION_LENGTH;

	s_ucVersion = (s_ucVersion + 1) & 0x1F;	/* New version - must be in range 0 ~ 31 */

	return ERR_OK;
}

STATIC HERROR xsvc_si_LibMakeSecMakePmtSection (HUINT32 ulActionId, SvcSi_MakeSec_Pmt_t *pstPmtArg, HUINT8 *pucSecRaw, HUINT32 *pulSecLen)
{
	HUINT32						 ulPmtSecLen = 0;
	xsvcMakeSec_PmtSecInfo_t	*pstPmtPacketSec = NULL;
	HERROR						 hErr, hResult = ERR_FAIL;

	pstPmtPacketSec = xsvc_si_LibMakeSecAllocPmtSection();
	if (NULL == pstPmtPacketSec)
	{
		HxLOG_Error ("Memory allocation failed...\n");
		goto END_FUNC;
	}

	switch (pstPmtArg->eMakeSrcType)
	{
	case eSVCSI_MAKESEC_PMT_SiSections:
		hErr = xsvc_si_LibMakeSecGetPmtSectionInfoByRawSections (ulActionId, pstPmtArg, pstPmtPacketSec);
		break;

	case eSVCSI_MAKESEC_PMT_SetByArgs:
		hErr = xsvc_si_LibMakeSecGetPmtSectionInfoBySetting (ulActionId, pstPmtArg, pstPmtPacketSec);
		break;

	case eSVCSI_MAKESEC_PMT_Transcode:
		hErr = xsvc_si_LibMakeSecGetPmtSectionInfoForTranscode (ulActionId, pstPmtArg, pstPmtPacketSec);
		break;

	default:
		HxLOG_Error ("pstPmtArg->eMakeSrcType (%d), not supporting value.\n", pstPmtArg->eMakeSrcType);
		hErr = ERR_FAIL;
		break;
	}

	if (ERR_OK != hErr)
	{
		HxLOG_Error ("make pmt section failed!! srcType(%d) \n", pstPmtArg->eMakeSrcType);
		goto END_FUNC;
	}

	hErr = xsvc_si_LibMakeSecWritePmtRawByPmtSectionInfo (pstPmtPacketSec, pucSecRaw, &ulPmtSecLen);
	if ((ERR_OK != hErr) || (0 == ulPmtSecLen))
	{
		HxLOG_Error ("svc_siarib_packet_WritePmtRawByPmtSectionInfo failed!!\n");
		goto END_FUNC;
	}

	*pulSecLen = ulPmtSecLen;
	hResult = ERR_OK;

END_FUNC:
	xsvc_si_LibMakeSecFreePmtSection (pstPmtPacketSec);

	return hResult;
}

#define ___MEMBER_FUNCTIONS___

HERROR xsvc_si_LibMakeSiSection_Base (HUINT32 ulActionId, SvcSi_MakeSecArg_t *punArg, HUINT8 *pucRawSec, HUINT32 *pulSecLen)
{
	HERROR				hResult = ERR_FAIL;

	if (NULL == punArg)										{ return ERR_FAIL; }
	if (NULL == pucRawSec)									{ return ERR_FAIL; }
	if (NULL == pulSecLen)									{ return ERR_FAIL; }

	switch (punArg->eSiType)
	{
	case eSIxTABLETYPE_Pat:
		hResult = xsvc_si_LibMakeSecMakePatSection (ulActionId, &(punArg->stPat), pucRawSec, pulSecLen);
		break;

	case eSIxTABLETYPE_Pmt:
		hResult = xsvc_si_LibMakeSecMakePmtSection (ulActionId, &(punArg->stPmt), pucRawSec, pulSecLen);
		break;

	default:
		HxLOG_Error("invalid table type(%d) \n", punArg->eSiType);
		hResult = ERR_FAIL;
		break;
	}

	return hResult;
}


/* end of file */

