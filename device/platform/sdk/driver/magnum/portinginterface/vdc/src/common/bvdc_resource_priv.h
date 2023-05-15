/***************************************************************************
 *     Copyright (c) 2004-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvdc_resource_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/32 $
 * $brcm_Date: 3/21/12 1:38p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vdc/7038/bvdc_resource_priv.h $
 * 
 * Hydra_Software_Devel/32   3/21/12 1:38p syang
 * SW7425-2699: put mpaa under the control of vdc resource manager; handle
 * correctly both order of enabling/disabling mpaa and turning on/off
 * hdmi/component output
 * 
 * Hydra_Software_Devel/31   6/21/11 11:02p pntruong
 * SW7425-337: Temporary restricted 2nd deinterlacer (madr) for encoder
 * path only to match 7425 usages.
 *
 * Hydra_Software_Devel/30   9/23/10 10:29a vanessah
 * SW7425-29:  STG support for transcoding
 *
 * Hydra_Software_Devel/29   6/18/10 4:23p rpan
 * SW7400-2763: New BVN resource management.
 *
 * Hydra_Software_Devel/28   4/7/10 11:34a tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic.  Rename TLA
 *
 * Hydra_Software_Devel/27   4/5/10 4:13p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic
 *
 * Hydra_Software_Devel/26   6/25/09 1:30p rpan
 * PR56137: Removed unused resource type CMP.
 *
 * Hydra_Software_Devel/25   6/25/09 12:12p rpan
 * PR56137, PR56138, PR56139, PR56166, PR56167, PR56168: Support for
 * various orthogonal VEC configurations.
 *
 * Hydra_Software_Devel/24   3/16/09 10:49p tdo
 * PR45785, PR45789: Merge from MCVP branch
 *
 * Hydra_Software_Devel/7420_mcvp/1   1/23/09 11:31p syang
 * PR 45785, PR 45789: add PI support for new the new module  MCVP
 * (MCTF+MCDI)
 *
 * Hydra_Software_Devel/23   12/8/08 5:44p rpan
 * PR45804: Switched to VDC generic resource management lib.
 *
 * Hydra_Software_Devel/22   9/16/08 10:17p pntruong
 * PR46494: B0: Remove 3rd output dac.
 *
 * Hydra_Software_Devel/21   3/26/08 7:03p pntruong
 * PR40448: Re-arraged resource info table and added place holder for
 * pcpll resource.
 *
 * Hydra_Software_Devel/20   3/19/08 5:44p tdo
 * PR39333: Implement HIST core
 *
 * Hydra_Software_Devel/19   2/28/08 6:23p hongtaoz
 * PR39154, PR36273, PR39202: added HSCL block; enabled MAD/ANR for 3548;
 * fixed non-linear scaling when MAD is on;
 *
 * Hydra_Software_Devel/18   12/10/07 4:02p pntruong
 * PR38046: Added work-around by preventing HD_DVI_1 connecting to drain 1
 * and 2.
 *
 * Hydra_Software_Devel/17   11/5/07 4:59p albertl
 * PR36701:  Added MUD functionality for 7440 C0.
 *
 * Hydra_Software_Devel/16   10/23/07 11:17a yuxiaz
 * PR29569, PR36290: Add FGT support on 7405.
 *
 * Hydra_Software_Devel/PR29569/1   10/12/07 10:49a yuxiaz
 * PR29569: Add FGT support on 7405
 *
 * Hydra_Software_Devel/15   4/2/07 11:30a syang
 * PR 29243: clean up: take off the obselete old-vnet-code
 *
 * Hydra_Software_Devel/14   2/20/07 9:34p albertl
 * PR23117:  Changed 7440 to use new VNET algorithm and fixed DNR compile
 * errors and warnings.
 *
 * Hydra_Software_Devel/13   2/20/07 5:22p darnstein
 * PR27101: Use new VNET programming method on 97403 chipset.
 *
 * Hydra_Software_Devel/12   2/9/07 2:52p pntruong
 * PR27557:  Support new vnet mode for all active chipsets except for 7440
 * and 7403.
 *
 * Hydra_Software_Devel/11   2/2/07 11:12a tdo
 * PR 27557:  Enable the new vnet mode for 7400
 *
 * Hydra_Software_Devel/10   1/22/07 1:18p darnstein
 * PR26619: silence compiler warnings.
 *
 * Hydra_Software_Devel/9   1/17/07 3:44p pntruong
 * PR23225: VDEC - PC input.  Added initial PC format support for
 * 640x480p@60.
 *
 * Hydra_Software_Devel/8   1/5/07 4:25p syang
 * PR 22569: almost completed coding for new vnet build/shutdown algorithm
 * (by each sub-modules)
 *
 * Hydra_Software_Devel/7   1/2/07 4:54p yuxiaz
 * PR25995: Added code to reserve a resource in resource manager.
 *
 * Hydra_Software_Devel/6   12/18/06 11:34p pntruong
 * PR22577: Merged back to mainline.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/3   11/28/06 11:42a yuxiaz
 * PR23638: Added new type BVDC_P_ResourceType_eDrainCrc for vnet drain.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/2   11/22/06 3:52p syang
 * PR 26151: re-write shared resource manager. update letter box, mad, dnr
 * shared-scl acquire/release code accordingly
 *
 * Hydra_Software_Devel/5   3/7/06 4:04p syang
 * PR 19670: added 7438 support
 *
 * Hydra_Software_Devel/4   2/22/06 12:13p hongtaoz
 * PR19082: added shared DNR support for 7400;
 *
 * Hydra_Software_Devel/3   8/19/05 2:53p hongtaoz
 * PR15888: adding support for dynamic switch on/off shared scaler on
 * bypass window;
 *
 * Hydra_Software_Devel/XVDPhase1/XVDPhase1_merge/1   8/19/05 2:28p hongtaoz
 * PR15888: adding support for dynamic switch on/off of shared scaler with
 * Bypass window;
 *
 * Hydra_Software_Devel/2   9/22/04 12:32p pntruong
 * PR12728: Fixed -pdantic build warnings and errors.
 *
 * Hydra_Software_Devel/1   7/8/04 2:44p jasonh
 * PR 11867: Adding resource control to VDC
 *
 *
 ***************************************************************************/
#ifndef BVDC_RESOURCE_PRIV_H__
#define BVDC_RESOURCE_PRIV_H__

#include "bvdc.h"
#include "bvdc_common_priv.h"

#ifdef __cplusplus
extern "C" {
#endif


/**************************************************************************/
/*
 * List of possible shared resource types. When a shared resource is acquired,
 * its type is passed as input, and the module handle or HW Id is output.
 * NOTE:  This enumeration is use to index into info table and swtich statement.
 *   Need to update following code when adding new resources.
 *
 * (1) s_aResInfoTbl
 * (2) 2 switch statements in create/destroy
 */
typedef enum BVDC_P_ResourceType
{
	/* shared resources represented by handle */
	BVDC_P_ResourceType_eVfd = 0,          /* shared VFD */
	BVDC_P_ResourceType_eCap,              /* shared CAP */
	BVDC_P_ResourceType_eAnr,              /* shared ANR */
	BVDC_P_ResourceType_eMad,              /* shared MAD */
	BVDC_P_ResourceType_eHscl,             /* shared HSCL */
	BVDC_P_ResourceType_eMcvp,             /* shared Mcvp */
	BVDC_P_ResourceType_eMud,              /* shared MUD */
	BVDC_P_ResourceType_eDnr,              /* shared DNR */
	BVDC_P_ResourceType_eBoxDetect,        /* shared box detetct */
	BVDC_P_ResourceType_eScl,              /* shared SCL */
	BVDC_P_ResourceType_eFgt,              /* shared FGT */
	BVDC_P_ResourceType_eHist,             /* shared HIST */

	/* separator between handle resources and HwId resources */
	BVDC_P_ResourceType_eHandleCntr,

	/* shared resources idendified by Hw Id */
	BVDC_P_ResourceType_eFreeCh,           /* shared free channels */
	BVDC_P_ResourceType_eLpBck,            /* shared loop back */
	BVDC_P_ResourceType_eDrainFrn,         /* shared front drains */
	BVDC_P_ResourceType_eDrainBck,         /* shared back drains */
	BVDC_P_ResourceType_eDrainCrc,         /* shared crc back drains */
#if BVDC_P_ORTHOGONAL_VEC
	BVDC_P_ResourceType_e656,              /* VEC 656 output */
	BVDC_P_ResourceType_eDvi,              /* VEC DVI output  */
	BVDC_P_ResourceType_eStg,              /* VEC STG output  */
	BVDC_P_ResourceType_eRf,               /* VEC RFM output  */
	BVDC_P_ResourceType_eIt,               /* VEC IT */
	BVDC_P_ResourceType_eVf,               /* VEC VF */
	BVDC_P_ResourceType_eSecam,            /* VEC SECAM */
	BVDC_P_ResourceType_eSecam_HD,         /* VEC HD pass-thru SECAM */
	BVDC_P_ResourceType_eSdsrc,            /* VEC SD SRC */
	BVDC_P_ResourceType_eHdsrc,            /* VEC HD SRC */
	BVDC_P_ResourceType_eDac,              /* DAC */
	BVDC_P_ResourceType_eMpaa,             /* MPAA */
#endif
	BVDC_P_ResourceType_ePcPll,            /* PLL shared by vdec0/1 */
	BVDC_P_ResourceType_eInvalid           /* Invalid. Do not use! */
} BVDC_P_ResourceType;

/***************************************************************************
 * capability flags are or-ed during acquiring.
 */
typedef enum
{
	BVDC_P_Able_eMem0    =     (1<<0),      /* able to access mem ctrl 0 */
	BVDC_P_Able_eMem1    =     (1<<1),      /* able to access mem ctrl 1 */
	BVDC_P_Able_eAllSrc  =     (1<<2),      /* able to use by all sources */
	BVDC_P_Able_eHd      =     (1<<3),      /* able to handle HD size */
	BVDC_P_Able_eXcode   =     (1<<4),      /* able to handle transcode */
	BVDC_P_Able_eInvalid =     (0xffff)     /* cause acquire to fail */

} BVDC_P_Able;

#define BVDC_P_ACQUIRE_ID_INVALID     (0xffffffff)
#define BVDC_P_HW_ID_INVALID          (0xffffffff)
#define BVDC_P_RESOURCE_ID_AVAIL      (0xffff0001)
#define BVDC_P_RESOURCE_ID_TOBEFREED  (0xffff0003)

/* the acquiring record for a resource */
typedef struct BVDC_P_ResourceEntry
{
	union
	{
		void   *  pvHandle;       /* handle */
		uint32_t  ulHwId;         /* or HW ID */
	} Id;
	BVDC_P_ResourceType  eType;   /* type of resource shared */
	uint32_t  ulCapabilities;     /* or-ed mem ctrl access and hd capability */
	uint32_t  ulAcquireId;        /* id of acquirer or user */
	uint32_t  ulAcquireCntr;      /* aquiring times by this acquiring id */
	uint32_t  bAcquireLock;       /* lock from acquiring untill release */
} BVDC_P_ResourceEntry;

/***************************************************************************
 * BVDC_P_ResourceContext
 *
 * It contains the acquiring/releasing record for each shared resource.
 *
 ***************************************************************************/
typedef struct BVDC_P_ResourceContext
{
	BDBG_OBJECT(BVDC_RES)

	/* ptr to the array for all shared resources */
	BVDC_P_ResourceEntry *pResourceRecords;

	/* ptr to the array of index to the entry of the 1st HW module of
	 * each resource type */
	uint32_t  *pulIndex1stEntry;
} BVDC_P_ResourceContext;


/***************************************************************************
 * {private}
 *
 */
BERR_Code  BVDC_P_Resource_Create
	( BVDC_P_Resource_Handle           *phResource,
	  BVDC_Handle                       hVdc );

/***************************************************************************
 * {private}
 *
 */
void  BVDC_P_Resource_Destroy
	( BVDC_P_Resource_Handle            hResource );


/***************************************************************************
 * {private}
 *
 * This func is used to aquire a resource that is represented by a handle.
 * Input: hResource - BVDC_P_Resource_Handle
 *        eType - the type of shared resource
 *        ulCapabilties - required memeory access and HD support capabililies
 *            are or-ed, refer to s_ul*Able[] in bvdc_resource_priv.c for
 *            valid capability combinations. If s_ul*Able[] does not exist
 *            for a type, all hw modules of the type has the same capability
 *            set, and "0" is used for ulCapabilties during acquiring.
 *        ulAcquireId - Idendify the usage (For example, when two windows
 *            that share the same source want to use box detect, they should
 *            acquire box detect module with the same source Id or handle as
 *            ulAcquireId. Later on, one window could release the resource
 *            and another window could still occupy it. This is done with an
 *            internal acquire counter. As another example, a window might
 *            use a scaler in reader or writer side, it should use different
 *            acquireId for the reader usage and writer usage, such as WinId
 *            for reader usage and (0x8000 | WinId) for writer usage).
 *		  bQuery - query if the resource is available when this flag is true
 *
 * Output: the assigned module handle.
 *
 * Module assignment rule:
 *    if the resource type has already be acquired for the acquire ID
 * successfully, assign it the same resource module, and increase the
 * acquiring cntr; otherwise, we try to find a perfect match for capabilities;
 * if we can not find a perfect match, the first module that satisfies the
 * capability requirement will be used.
 */
BERR_Code  BVDC_P_Resource_AcquireHandle_isr
	( BVDC_P_Resource_Handle            hResource,
	  BVDC_P_ResourceType               eType,
	  uint32_t                          ulCapabilities,
	  uint32_t                          ulAcquireId,
	  void                            **ppvResourceHandle,
	  bool                              bQuery);

/***************************************************************************
 * {private}
 *
 * This func is used to aquire a resource that is NOT represented by a handle.
 * Input: hResource - BVDC_P_Resource_Handle
 *        eType - the type of shared resource
 *        ulCapabilties - required memeory access and HD support capabililies
 *            are or-ed, refer to s_ul*Able[] in bvdc_resource_priv.c for
 *            valid capability combinations. If s_ul*Able[] does not exist
 *            for a type, all hw modules of the type has the same capability
 *            set, and "0" is used for ulCapabilties during acquiring.
 *        ulAcquireId - Idendify the usage (For example, when two windows
 *            that share the same source want to use box detect, they should
 *            acquire box detect module with the same source Id or handle as
 *            ulAcquireId. Later on, one window could release the resource
 *            and another window could still occupy it. This is done with an
 *            internal acquire counter. As another example, a window might
 *            use a scaler in reader or writer side, it should use different
 *            acquireId for the reader usage and writer usage, such as WinId
 *            for reader usage and (0x8000 | WinId) for writer usage).
 *        bQuery - query if the resource is available when this flag is true
 *
 * Output: the assigned module HW ID.
 *
 * Module assignment rule:
 *    if the resource type has already be acquired for the acquire ID
 * successfully, assign it the same resource module, and increase the
 * acquiring cntr; otherwise, we try to find a perfect match for capabilities;
 * if we can not find a perfect match, the first module that satisfies the
 * capability requirement will be used.
 */
BERR_Code  BVDC_P_Resource_AcquireHwId_isr
	( BVDC_P_Resource_Handle            hResource,
	  BVDC_P_ResourceType               eType,
	  uint32_t                          ulCapabilities,
	  uint32_t                          ulAcquireId,
	  uint32_t                         *pulHwId,
	  bool                              bQuery);

/***************************************************************************
 * {private}
 *
 * This func is used to release a resource that is represented by a handle.
 * It is user's responsibilty to make sure that the HW resource is really no
 * longer be used in its vnet configuration, before calling to relase.
 *
 * Module realease rule:
 *    when an acquiring record entry with the handle is found, its ulAcquireCntr
 * is decreased by 1. When ulAcquireCntr reaches 0, the module is really
 * released to be acquired with new acquire ID.
 */
BERR_Code  BVDC_P_Resource_ReleaseHandle_isr
	( BVDC_P_Resource_Handle            hResource,
	  BVDC_P_ResourceType               eType,
	  void                             *pvResourceHandle );

/***************************************************************************
 * {private}
 *
 * This func is used to release a resource that is NOT represented by a handle.
 * It is user's responsibilty to make sure that the HW resource is really no
 * longer be used in its vnet configuration, before calling to relase.
 *
 * Module realease rule:
 *    when an acquiring record entry with the handle is found, its ulAcquireCntr
 * is decreased by 1. When ulAcquireCntr reaches 0, the module is really
 * released to be acquired with new acquire ID.
 */
BERR_Code  BVDC_P_Resource_ReleaseHwId_isr
	( BVDC_P_Resource_Handle            hResource,
	  BVDC_P_ResourceType               eType,
	  uint32_t                          ulHwId );

/***************************************************************************
 * {private}
 *
 * This func returns the number of acquires of a handle resource.
 *
 * For an example of usage, the box detect HW should be disabled only if the
 * acquiring counter becomes 0. If two or more windows are using the box
 * detect, disabling with one window should not really shut down the HW, but
 * causes that one window no longer involves box detect. Notice that tearing
 * off the HW module from the vnet might take one or more field after apply.
 *
 */
uint32_t  BVDC_P_Resource_GetHandleAcquireCntr_isr
	( BVDC_P_Resource_Handle            hResource,
	  BVDC_P_ResourceType               eType,
	  void                             *pvResourceHandle );

/***************************************************************************
 * {private}
 *
 * This func returns the number of acquires of a HW ID resource.
 *
 * For an example of usage, the box detect HW should be disabled only if the
 * acquiring counter becomes 0. If two or more windows are using the box
 * detect, disabling with one window should not really shut down the HW, but
 * causes that one window no longer involves box detect. Notice that tearing
 * off the HW module from the vnet might take one or more field after apply.
 *
 */
uint32_t  BVDC_P_Resource_GetHwIdAcquireCntr_isr
	( BVDC_P_Resource_Handle            hResource,
	  BVDC_P_ResourceType               eType,
	  uint32_t                          ulHwId );

/***************************************************************************
 * {private}
 *
 * This func lock the handle resource from acquiring with the same acquireId,
 * until its acquireCntr is released to 0. It might be used during shut-down
 * process to simplify the state logic.
 */
BERR_Code  BVDC_P_Resource_LockHandle_isr
	( BVDC_P_Resource_Handle            hResource,
	  BVDC_P_ResourceType               eType,
	  void                             *pvResourceHandle );

/***************************************************************************
 * {private}
 *
 * This func lock the handle resource from acquiring with the same acquireId,
 * until its acquireCntr is released to 0. It might be used during shut-down
 * process to simplify the state logic.
 */
BERR_Code  BVDC_P_Resource_LockHwId_isr
	( BVDC_P_Resource_Handle            hResource,
	  BVDC_P_ResourceType               eType,
	  uint32_t                          ulHwId );


/***************************************************************************
 * {private}
 *
 * This func is similar to BVDC_P_Resource_AcquireHandle_isr, and used to
 * reserve a resource with specific ulResourceIndex that is represented by a
 * handle.
 *
 * Input: hResource - BVDC_P_Resource_Handle
 *        eType - the type of shared resource
 *        ulResourceIndex - index to the resouce to be reserved.
 *        ulAcquireId - see BVDC_P_Resource_AcquireHandle_isr
 *
 * Output: the assigned module handle.
 *
 */
BERR_Code  BVDC_P_Resource_ReserveHandle_isr
	( BVDC_P_Resource_Handle            hResource,
	  BVDC_P_ResourceType               eType,
	  uint32_t                          ulResourceIndex,
	  uint32_t                          ulAcquireId,
	  void                            **ppvResourceHandle );


#ifdef __cplusplus
}
#endif

#endif /* #ifndef BVDC_RESOURCE_PRIV_H__*/

/* End of file. */
