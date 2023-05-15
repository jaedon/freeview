/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvdc_vnet_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/87 $
 * $brcm_Date: 8/16/12 10:10a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vdc/src/common/bvdc_vnet_priv.c $
 * 
 * Hydra_Software_Devel/87   8/16/12 10:10a tdo
 * SW7445-8:  Fix assertion error due to new source
 *
 * Hydra_Software_Devel/86   11/24/11 1:56a tdo
 * SW7435-9: Add support for CMP4-5, GFD4-5, MFD3, VFD5
 *
 * Hydra_Software_Devel/85   12/16/10 4:02p yuxiaz
 * SW7422-35: Added new DRAIN HW version. Fixed build error on 7358, 7552,
 * 7231, 7346, 7344.
 *
 * Hydra_Software_Devel/84   12/16/10 2:23p yuxiaz
 * SW7422-35: Move drain to BVDC_P_Drain_BuildFormatRul_isr
 *
 * Hydra_Software_Devel/83   11/15/10 6:00p pntruong
 * SW7231-2: Initial support to make 7231 compile.
 *
 * Hydra_Software_Devel/82   10/18/10 3:07p yuxiaz
 * SW7422-39: Added BAVC_SourceId_eMpeg2 support in VDC.
 *
 * Hydra_Software_Devel/81   6/18/10 4:24p rpan
 * SW7400-2763: New BVN resource management.
 *
 * Hydra_Software_Devel/80   4/19/10 10:24p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic. Move
 * BDBG_OBJECT_ID_DECLARE private header files instead of .c.
 *
 * Hydra_Software_Devel/79   4/7/10 11:37a tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic.  Rename TLA
 *
 * Hydra_Software_Devel/78   4/5/10 4:17p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic
 *
 * Hydra_Software_Devel/77   3/26/08 7:03p pntruong
 * PR40448: Re-arraged resource info table and added place holder for
 * pcpll resource.
 *
 * Hydra_Software_Devel/76   2/21/08 5:02p pntruong
 * PR39244: Need drain debugging hooked up to HD_DVI, VDEC, and 656in.
 * Improved code readability.
 *
 * Hydra_Software_Devel/75   12/10/07 4:03p pntruong
 * PR38046: Added work-around by preventing HD_DVI_1 connecting to drain 1
 * and 2.
 *
 * Hydra_Software_Devel/74   4/2/07 11:32a syang
 * PR 29243: clean up: take off the obselete old-vnet-code
 *
 * Hydra_Software_Devel/73   1/11/07 7:04p syang
 * PR 22569: 1st time check in after anr building vnet correctly and not
 * hang the sys. But display is still static
 *
 * Hydra_Software_Devel/72   1/10/07 9:04p pntruong
 * PR 22569: Backed out offending loopback code that conflict with dnr and
 * anr until the full anr code is tested.
 *
 * Hydra_Software_Devel/71   1/10/07 3:31p syang
 * PR 22569:  check in old vnet code that support one anr
 *
 * Hydra_Software_Devel/70   1/5/07 4:26p syang
 * PR 22569: almost completed coding for new vnet build/shutdown algorithm
 * (by each sub-modules)
 *
 * Hydra_Software_Devel/69   1/3/07 5:27p yuxiaz
 * PR25995: Add DNR support for 7400 B0.
 *
 * Hydra_Software_Devel/68   12/18/06 11:30p pntruong
 * PR22577: Merged back to mainline.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/12   12/18/06 1:48p yuxiaz
 * PR25995: Add DNR support for 7400 B0.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/11   12/15/06 3:32p yuxiaz
 * PR26637: Clean up vnet tables.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/10   12/15/06 10:35a yuxiaz
 * PR 25963: Merge in from main branch: Initial VDC bringup for 7400 B0
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/9   12/11/06 5:25p albertl
 * PR24587:  Added 3rd GFX Feeder source.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/8   11/29/06 1:58p yuxiaz
 * PR23638: always direct bypass compositor to BVN CRC on 3563.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/7   11/28/06 3:36p yuxiaz
 * PR23638: Rewrite vnet drain code by using shared resource manager.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/6   11/16/06 1:53p hongtaoz
 * PR25668: merge in vdc support for 7403;
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/5   11/15/06 6:19p albertl
 * PR24587:  Removed errant debug printf.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/4   11/15/06 5:08p albertl
 * PR24587:  Added downsample source for 7440.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/3   10/20/06 1:55p hongtaoz
 * PR23260: fixed DAC settings limitation on the swapped display for 356x;
 * support 3560B;
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/2   10/19/06 6:59p hongtaoz
 * PR23260: add dvo master mode support; add support for cmp/display
 * connection at create time; restructured display code to be scalable
 * for 7400B support;
 *
 * Hydra_Software_Devel/65   8/23/06 3:44p yuxiaz
 * PR23307, PR22801: Rewrite BVDC_P_Vnet_GetDrainId to fix a strange
 * optimization problem on 7038 B1.
 *
 * Hydra_Software_Devel/64   8/23/06 9:25a yuxiaz
 * PR23307, PR22801: Added Release drain in BVDC_Source_Destroy.
 *
 * Hydra_Software_Devel/63   8/22/06 2:58p tdo
 * PR 20277: Bringup 656in support for 7400
 *
 * Hydra_Software_Devel/62   8/21/06 6:31p tdo
 * PR 23197: Bringup DNR for 3563
 *
 * Hydra_Software_Devel/61   8/18/06 5:44p albertl
 * PR23117:  Fixed #elif with no condition.
 *
 * Hydra_Software_Devel/60   8/18/06 4:29p albertl
 * PR23117:  Added 7440 support.
 *
 * Hydra_Software_Devel/59   8/7/06 5:37p pntruong
 * PR22577: Initial bringup of VDC.
 *
 * Hydra_Software_Devel/58   8/4/06 6:45p pntruong
 * PR23307: Fixed build errors for 7118, 7400, 7438, 7440, 3563.
 *
 * Hydra_Software_Devel/57   8/4/06 4:52p yuxiaz
 * PR23307: Share vnet drain between 656 and HD_DVI sources.
 *
 * Hydra_Software_Devel/56   6/14/06 1:45p syang
 * PR 21689: add support for 7118
 *
 * Hydra_Software_Devel/55   5/26/06 3:43p pntruong
 * PR20642: Refactored handling of hList.
 *
 * Hydra_Software_Devel/53   4/20/06 5:51p pntruong
 * PR20750: Please update bavc.h to reflect presence of third VEC.
 *
 * Hydra_Software_Devel/52   4/4/06 5:00p pntruong
 * PR20403:  Account for the new gfx2 source added.
 *
 * Hydra_Software_Devel/51   4/4/06 1:50p hongtaoz
 * PR20403: add triple displays for 7400;
 *
 * Hydra_Software_Devel/50   3/7/06 4:19p syang
 * PR 19670: added 7438 support
 *
 * Hydra_Software_Devel/49   2/22/06 12:13p hongtaoz
 * PR19082: added shared DNR support for 7400;
 *
 * Hydra_Software_Devel/48   1/17/06 4:48p hongtaoz
 * PR19082: first compile for 7400;
 *
 * Hydra_Software_Devel/47   11/23/05 5:28p hongtaoz
 * PR18180: added 7401 support for 2 CMP's;
 *
 * Hydra_Software_Devel/46   10/31/05 6:26p hongtaoz
 * PR17514: add DNR support in VDC;
 *
 * Hydra_Software_Devel/45   9/21/05 3:10p hongtaoz
 * PR17233: added eScaler mode for bypass window to avoid assertion;
 *
 * Hydra_Software_Devel/44   9/6/05 7:02p hongtaoz
 * PR16812, PR15888: mute writer blocks and muxe earlier; window blocks
 * shutdown and normal programming order are reverse; not to re-init
 * window shared resource at create time;
 *
 * Hydra_Software_Devel/43   9/1/05 6:24p jasonh
 * PR 16967: Removing XVD source
 *
 * Hydra_Software_Devel/42   8/31/05 3:41p hongtaoz
 * PR16812, PR15495: assure MAD disconnect RUL got executed;
 *
 * Hydra_Software_Devel/41   8/22/05 5:24p hongtaoz
 * PR12192, PR15888: make MAD sharable between the two video windows of
 * main display; reduced unnecessary window shutdown initiation if it's
 * already in the middle of shutdown; build both slots RULs for analog
 * progressive source format during shutdown to workaround unexpected
 * trigger pattern;
 *
 * Hydra_Software_Devel/40   8/20/05 11:04p hongtaoz
 * PR15888: fixed 7401 vnet setting;
 *
 * Hydra_Software_Devel/39   8/19/05 2:55p hongtaoz
 * PR15888: adding support for dynamic switch on/off shared scaler on
 * bypass window;
 *
 * Hydra_Software_Devel/XVDPhase1/XVDPhase1_merge/1   8/19/05 2:28p hongtaoz
 * PR15888: adding support for dynamic switch on/off of shared scaler with
 * Bypass window;
 *
 * Hydra_Software_Devel/38   8/4/05 4:58p yuxiaz
 * PR16182: Fixed vnet settings for 7401.
 *
 * Hydra_Software_Devel/37   8/2/05 5:28p syang
 * PR 16536: fix vnet assert for 7401
 *
 * Hydra_Software_Devel/36   6/30/05 2:53p hongtaoz
 * PR16015, PR16043, PR15921, PR15888: set vdec source trigger count
 * immediately to avoid forever vec takeover; added compile option
 * support for scaling on bypass video;
 *
 * Hydra_Software_Devel/35   6/28/05 11:04a jasonh
 * PR 16012: Adding support for 7401
 *
 * Hydra_Software_Devel/34   6/15/05 4:22p hongtaoz
 * PR14510, PR15163, PR15743, PR15785: vnet switch shall be carefully
 * programmed such that the vnet_f to be connected with non-feeder source
 * is always set at last to avoid garbage data being temporarily injected
 * into the BVN back switch;
 *
 * Hydra_Software_Devel/33   3/18/05 6:31p pntruong
 * PR14494: Basic vdc up and running.  Background and passed output format
 * switch test.
 *
 * Hydra_Software_Devel/32   3/17/05 6:41p pntruong
 * PR14494: Add preliminary software support to 3560 A0.
 *
 * Hydra_Software_Devel/31   2/4/05 4:55p hongtaoz
 * PR13793: added compile option to use scaler_3 for bypass window;
 *
 * Hydra_Software_Devel/30   1/25/05 2:17p yuxiaz
 * PR13569: Use CRC to drain HD_DVI data when XVD tells VDC to switch
 * format.
 *
 * Hydra_Software_Devel/29   1/21/05 9:55a yuxiaz
 * PR13733: Need to disable CRC after bringup HD_DVI to get duall HD_DVI
 * work.
 *
 ***************************************************************************/
#include "bstd.h"
#include "bvdc.h"
#include "bvdc_vnet_priv.h"
#include "bvdc_common_priv.h"
#include "bvdc_window_priv.h"
#include "bvdc_source_priv.h"
#include "bvdc_priv.h"

BDBG_MODULE(VNET);
BDBG_OBJECT_ID(BVDC_VNT);

/* INDEX: By source id, do not put ifdefs and nested ifdefs around these that
 * become impossible to decipher. */
static const uint32_t  s_aulVnetFDrainSrc[] =
{
	BVDC_P_VnetF_eInvalid,     /* Mpeg0 */
	BVDC_P_VnetF_eInvalid,     /* Mpeg1 */
	BVDC_P_VnetF_eInvalid,     /* Mpeg2 */
	BVDC_P_VnetF_eInvalid,     /* Mpeg3 */
	BVDC_P_VnetF_eInvalid,     /* Mpeg4 */
	BVDC_P_VnetF_eInvalid,     /* Mpeg5 */
	BVDC_P_VnetF_eAnalog_0,    /* Vdec0 */
	BVDC_P_VnetF_eAnalog_1,    /* Vdec1 */
	BVDC_P_VnetF_eCcir656_0,   /* 656In0 */
	BVDC_P_VnetF_eCcir656_1,   /* 656In1 */
	BVDC_P_VnetF_eInvalid,     /* Gfx0 */
	BVDC_P_VnetF_eInvalid,     /* Gfx1 */
	BVDC_P_VnetF_eInvalid,     /* Gfx2 */
	BVDC_P_VnetF_eInvalid,     /* Gfx3 */
	BVDC_P_VnetF_eInvalid,     /* Gfx4 */
	BVDC_P_VnetF_eInvalid,     /* Gfx5 */
	BVDC_P_VnetF_eInvalid,     /* Gfx6 */
	BVDC_P_VnetF_eHdDvi_0,     /* HdDvi0 */
	BVDC_P_VnetF_eHdDvi_1,     /* HdDvi0 */
	BVDC_P_VnetF_eInvalid,     /* Ds0 */
};

/***************************************************************************
 * {private}
 */
BERR_Code BVDC_P_Vnet_Create
	( BVDC_P_Vnet_Handle               *phVnet,
	  BREG_Handle                       hRegister )
{
	BVDC_P_VnetContext    *pVnet;

	BDBG_ENTER(BVDC_P_Vnet_Create);

	BDBG_ASSERT(phVnet);
	BDBG_ASSERT(hRegister);

	/* The handle will be NULL if create fails. */
	*phVnet = NULL;

	/* Alloc the context. */
	pVnet = (BVDC_P_VnetContext*)(BKNI_Malloc(sizeof(BVDC_P_VnetContext)));
	if(!pVnet)
	{
		return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
	}

	/* Clear out the context and set defaults. */
	BKNI_Memset((void*)pVnet, 0x0, sizeof(BVDC_P_VnetContext));
	BDBG_OBJECT_SET(pVnet, BVDC_VNT);

	/* Store the id & hRegister for activating the triggers. */
	pVnet->hRegister = hRegister;

	/* All done. now return the new fresh context to user. */
	*phVnet = (BVDC_P_Vnet_Handle)pVnet;

	BDBG_LEAVE(BVDC_P_Vnet_Create);
	return BERR_SUCCESS;
}


/***************************************************************************
 * {private}
 */
void BVDC_P_Vnet_Destroy
	( BVDC_P_Vnet_Handle                hVnet )
{
	BDBG_ENTER(BVDC_P_Vnet_Destroy);
	BDBG_OBJECT_ASSERT(hVnet, BVDC_VNT);

	BDBG_OBJECT_DESTROY(hVnet, BVDC_VNT);
	/* Release context in system memory */
	BKNI_Free((void*)hVnet);

	BDBG_LEAVE(BVDC_P_Vnet_Destroy);
	return;
}


/***************************************************************************
 * Trying to aquire (1) front drain or (2) back drain.
 *
 * (1) SRC -> VNET_F_DRN_x [ 1 resource, x]
 *
 * if fails try
 *
 * (2) SRC -> VNET_F_FCH_x -> VNET_B_DRN_y  [ 2 resources, x and y ]
 *
 */
BERR_Code BVDC_P_Drain_Acquire
	( BVDC_P_DrainContext             *pDrain,
	  BVDC_P_Resource_Handle           hResource,
	  BAVC_SourceId                    eSourceId )
{
	uint32_t ulVnetFDrainId = 0, ulVnetBDrainId = 0;
	BERR_Code eResult = BERR_SUCCESS;

	/* Sanity check and get context */
	BDBG_OBJECT_ASSERT(hResource, BVDC_RES);
	BDBG_ASSERT(pDrain);

	/* Init */
	pDrain->ulDbgOffset = BVDC_P_DRN_INVALID_OFFSET;

	/* acquire BVDC_P_ResourceType_eDrainFrn first */
	BKNI_EnterCriticalSection();
	eResult = BVDC_P_Resource_AcquireHwId_isr(hResource,
		BVDC_P_ResourceType_eDrainFrn,
		(BAVC_SourceId_eHdDvi1 == eSourceId) ? BVDC_P_Able_eAllSrc : 0,
		eSourceId, &ulVnetFDrainId, false);
	BKNI_LeaveCriticalSection();

	if(BERR_SUCCESS == eResult)
	{
		BDBG_ASSERT(BVDC_P_DrainFrnId_eUnknown != ulVnetFDrainId);
		pDrain->eVnetFDrainType = BVDC_P_ResourceType_eDrainFrn;
		pDrain->ulVnetFDrainId  = ulVnetFDrainId;
		pDrain->eVnetFDrainSrc  = s_aulVnetFDrainSrc[eSourceId];
		pDrain->eVnetBDrainType = BVDC_P_ResourceType_eInvalid;
		pDrain->ulVnetBDrainId  = BVDC_P_VnetB_eInvalid;
		pDrain->eVnetBDrainSrc  = BVDC_P_VnetB_eDisabled;

#if (BVDC_P_SUPPORT_DRAIN_VER >= BVDC_P_VNET_VER_1) /* Has drain */
#if (BVDC_P_SUPPORT_DRAIN_F > 1)
		pDrain->ulDbgOffset = ((ulVnetFDrainId) *
			 (BCHP_VNET_F_DRAIN_1_ERR_STATUS - BCHP_VNET_F_DRAIN_0_ERR_STATUS));
#endif
#endif
		return BERR_SUCCESS;
	}

	/* didn't get a BVDC_P_ResourceType_eDrainFrn,
	 * try BVDC_P_ResourceType_eFreeCh with BVDC_P_ResourceType_eDrainBck or
	 * BVDC_P_ResourceType_eDrainCrc */
	BKNI_EnterCriticalSection();
	eResult = BVDC_P_Resource_AcquireHwId_isr(hResource,
		BVDC_P_ResourceType_eFreeCh, 0, eSourceId, &ulVnetFDrainId, false);
	BKNI_LeaveCriticalSection();

	if(BERR_SUCCESS == eResult)
	{
		BDBG_ASSERT(BVDC_P_DrainFrnId_eUnknown != ulVnetFDrainId);
		BDBG_ERR(("Got FCH, try to get Back drain"));

		/* Got FCH, try BVDC_P_ResourceType_eDrainBck */
		BKNI_EnterCriticalSection();
		eResult = BVDC_P_Resource_AcquireHwId_isr(hResource,
			BVDC_P_ResourceType_eDrainBck, 0, eSourceId, &ulVnetBDrainId, false);
		BKNI_LeaveCriticalSection();

		if(BERR_SUCCESS == eResult)
		{
			BDBG_ASSERT(BVDC_P_DrainBckId_eUnknown != ulVnetBDrainId);

			/* Use back drain */
			pDrain->eVnetFDrainType = BVDC_P_ResourceType_eFreeCh;
			pDrain->ulVnetFDrainId  = ulVnetFDrainId;
			pDrain->eVnetFDrainSrc  = s_aulVnetFDrainSrc[eSourceId];
			pDrain->eVnetBDrainType = BVDC_P_ResourceType_eDrainBck;
			pDrain->ulVnetBDrainId  = ulVnetBDrainId;
			pDrain->eVnetBDrainSrc  = BVDC_P_VnetB_eChannel_0 + ulVnetFDrainId;

			/* Offset from drain F_DRAIN_0.  Currently only 1 drain available. */
#if (BVDC_P_SUPPORT_DRAIN_VER >= BVDC_P_VNET_VER_1) /* Has drain */
			pDrain->ulDbgOffset = (
				BCHP_VNET_B_DRAIN_0_ERR_STATUS -
				BCHP_VNET_F_DRAIN_0_ERR_STATUS);
			BDBG_ASSERT(BVDC_P_SUPPORT_DRAIN_B == 1);
#endif
			return BERR_SUCCESS;
		}
		else
		{
			/* Got FCH, try BVDC_P_ResourceType_eDrainCrc */
			BKNI_EnterCriticalSection();
			eResult = BVDC_P_Resource_AcquireHwId_isr(hResource,
				BVDC_P_ResourceType_eDrainCrc, 0, eSourceId, &ulVnetBDrainId, false);
			BKNI_LeaveCriticalSection();

			if(BERR_SUCCESS == eResult)
			{
				BDBG_ASSERT(BVDC_P_DrainCrcId_eUnknown != ulVnetBDrainId);

				/* Use crc drain */
				pDrain->eVnetFDrainType = BVDC_P_ResourceType_eFreeCh;
				pDrain->ulVnetFDrainId  = ulVnetFDrainId;
				pDrain->eVnetFDrainSrc  = s_aulVnetFDrainSrc[eSourceId];
				pDrain->eVnetBDrainType = BVDC_P_ResourceType_eDrainCrc;
				pDrain->ulVnetBDrainId  = ulVnetBDrainId;
				pDrain->eVnetBDrainSrc  = BVDC_P_VnetB_eChannel_0 + ulVnetFDrainId;
				return BERR_SUCCESS;
			}
			else
			{
				/* Release BVDC_P_ResourceType_eDrainBck */
				BVDC_P_Resource_ReleaseHwId_isr(hResource,
					BVDC_P_ResourceType_eFreeCh, ulVnetFDrainId);
				goto done;
			}
		}
	}

done:
	return BERR_TRACE(eResult);

}

/***************************************************************************
 *
 */
void BVDC_P_Drain_Release
	( const BVDC_P_DrainContext       *pDrain,
	  BVDC_P_Resource_Handle           hResource )
{
	/* Sanity check and get context */
	BDBG_OBJECT_ASSERT(hResource, BVDC_RES);

	if(pDrain->eVnetFDrainType != BVDC_P_ResourceType_eInvalid)
	{
		BVDC_P_Resource_ReleaseHwId_isr(hResource,
			pDrain->eVnetFDrainType, pDrain->ulVnetFDrainId);
	}

	if(pDrain->eVnetBDrainType != BVDC_P_ResourceType_eInvalid)
	{
		BVDC_P_Resource_ReleaseHwId_isr(hResource,
			pDrain->eVnetBDrainType, pDrain->ulVnetBDrainId);
	}

	return;
}


/***************************************************************************
 *
 */
void BVDC_P_Drain_BuildRul_isr
	( const BVDC_P_DrainContext       *pDrain,
	  BVDC_P_ListInfo                 *pList )
{
	/* Build VNET_F */
	if(pDrain->eVnetFDrainType == BVDC_P_ResourceType_eDrainFrn)
	{
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ =
			BRDC_REGISTER(BCHP_VNET_F_DRAIN_0_SRC + pDrain->ulVnetFDrainId * 4);
		*pList->pulCurrent++ = pDrain->eVnetFDrainSrc;
	}
	else if(pDrain->eVnetFDrainType == BVDC_P_ResourceType_eFreeCh)
	{
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ =
			BRDC_REGISTER(BCHP_VNET_F_FCH_0_SRC + pDrain->ulVnetFDrainId * 4);
		*pList->pulCurrent++ = pDrain->eVnetFDrainSrc;
	}

	/* Build VNET_B */
	if(pDrain->eVnetBDrainType == BVDC_P_ResourceType_eDrainBck)
	{
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ =
			BRDC_REGISTER(BCHP_VNET_B_DRAIN_0_SRC + pDrain->ulVnetBDrainId * 4);
		*pList->pulCurrent++ = pDrain->eVnetBDrainSrc;
	}
	else if(pDrain->eVnetBDrainType == BVDC_P_ResourceType_eDrainCrc)
	{
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ =
			BRDC_REGISTER(BCHP_VNET_B_CRC_SRC + pDrain->ulVnetBDrainId * 4);
		*pList->pulCurrent++ = pDrain->eVnetBDrainSrc;
	}

	return;
}


/***************************************************************************
 * Build Format
 *
 */
void BVDC_P_Drain_BuildFormatRul_isr
	( const BVDC_P_DrainContext       *pDrain,
	  const BVDC_P_Rect               *pScanOut,
	  const BFMT_VideoInfo            *pFmtInfo,
	  BVDC_P_ListInfo                 *pList )
{

	if(BVDC_P_DRN_HAS_DEBUG(pDrain))
	{
#if (BVDC_P_SUPPORT_DRAIN_VER >= BVDC_P_VNET_VER_3)
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ =
			BRDC_REGISTER(BCHP_VNET_F_DRAIN_0_DEBUG_CTRL + pDrain->ulDbgOffset);
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(VNET_F_DRAIN_0_DEBUG_CTRL, VIDEO_3D_MODE,
				pFmtInfo->eOrientation ) |
			BCHP_FIELD_DATA(VNET_F_DRAIN_0_DEBUG_CTRL, EXP_PIC_XSIZE,
				pScanOut->ulWidth ) |
			BCHP_FIELD_DATA(VNET_F_DRAIN_0_DEBUG_CTRL, EXP_PIC_YSIZE,
				pScanOut->ulHeight >> pFmtInfo->bInterlaced);

#elif (BVDC_P_SUPPORT_DRAIN_VER == BVDC_P_VNET_VER_2)
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ =
			BRDC_REGISTER(BCHP_VNET_F_DRAIN_0_DEBUG_CTRL + pDrain->ulDbgOffset);
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(VNET_F_DRAIN_0_DEBUG_CTRL, BVB_VIDEO,
				pFmtInfo->eOrientation ) |
			BCHP_FIELD_DATA(VNET_F_DRAIN_0_DEBUG_CTRL, EXP_PIC_XSIZE,
				pScanOut->ulWidth ) |
			BCHP_FIELD_DATA(VNET_F_DRAIN_0_DEBUG_CTRL, EXP_PIC_YSIZE,
				pScanOut->ulHeight >> pFmtInfo->bInterlaced);

#elif (BVDC_P_SUPPORT_DRAIN_VER >= BVDC_P_VNET_VER_1)
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ =
			BRDC_REGISTER(BCHP_VNET_F_DRAIN_0_EXP_PIC_SIZE + pDrain->ulDbgOffset);
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(VNET_F_DRAIN_0_EXP_PIC_SIZE, EXP_PIC_XSIZE,
				pScanOut->ulWidth                                        ) |
			BCHP_FIELD_DATA(VNET_F_DRAIN_0_EXP_PIC_SIZE, EXP_PIC_YSIZE,
				pScanOut->ulHeight >> pFmtInfo->bInterlaced              );
#else
		BSTD_UNUSED(pList);
		BSTD_UNUSED(pDrain);
		BSTD_UNUSED(pFmtInfo);
		BSTD_UNUSED(pScanOut);
#endif
	}

	return;
}

/* End of file. */
