/***************************************************************************
 *	   Copyright (c) 2003-2012, Broadcom Corporation
 *	   All Rights Reserved
 *	   Confidential Property of Broadcom Corporation
 *
 *	THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *	AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *	EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmrc.h $
 * $brcm_Revision: Hydra_Software_Devel/68 $
 * $brcm_Date: 10/24/12 3:51p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/commonutils/mrc/include/bmrc.h $
 * 
 * Hydra_Software_Devel/68   10/24/12 3:51p vsilyaev
 * SW7435-444: Added Standby support
 * 
 * Hydra_Software_Devel/67   8/14/12 3:56p albertl
 * SW7445-10: Added MRC support for 7445.
 * 
 * Hydra_Software_Devel/66   5/2/12 2:48p albertl
 * SW7584-24: Updated 7584 support.
 * 
 * Hydra_Software_Devel/65   11/2/11 6:09p albertl
 * SW7435-12: Added 7435 support to MRC.
 * 
 * Hydra_Software_Devel/64   9/16/11 6:19p albertl
 * SW7425-1298: Updated clients for 7425.
 * 
 * Hydra_Software_Devel/63   3/23/11 4:44p albertl
 * SWDTV-5969: Updated MRC to be compatible with 35233 a0.
 * 
 * Hydra_Software_Devel/62   12/29/10 3:18p pntruong
 * SW7422-168: Remove bogus warning msg.
 *
 * Hydra_Software_Devel/61   12/9/10 9:08p albertl
 * SWBLURAY-23579: Fix issues with renamed memory controller.
 *
 * Hydra_Software_Devel/60   12/3/10 10:05p albertl
 * SWBLURAY-23672: Added MRC support to 7640.
 *
 * Hydra_Software_Devel/59   11/22/10 6:07p albertl
 * SW7422-15, SW7125-27: Fixed SVD and AVD client names.  Fixed wrong
 * client assignment for 7135.
 *
 * Hydra_Software_Devel/58   11/15/10 3:51p albertl
 * SW35125-23: Added 35125 support to MRC.
 *
 * Hydra_Software_Devel/57   9/15/10 6:43p albertl
 * SW7358-9: Added initial support for 7135 in MRC.
 *
 * Hydra_Software_Devel/56   8/27/10 6:39p albertl
 * SW7125-27: Updated for 7425 support.
 *
 * Hydra_Software_Devel/55   6/29/10 8:28p albertl
 * SW7422-15: Added 7422 clients back in.
 *
 * Hydra_Software_Devel/53   6/24/10 7:06p vanessah
 * SW7422-12:  fix run time error. Enum order issue
 *
 * Hydra_Software_Devel/52   6/24/10 3:52p vanessah
 * SW7422-12:  Add additional mrc client support for 7422
 *
 * Hydra_Software_Devel/51   5/28/10 8:39p albertl
 * SW35230-250: Updated 35230 MRC clients.
 *
 * Hydra_Software_Devel/50   3/15/10 2:19p albertl
 * SW3548-2832: Removed extraneous BMRC_Client_eHIF_CPU enum.
 *
 * Hydra_Software_Devel/49   3/12/10 6:14p albertl
 * SW3548-2832: Updated clients for 3548 and added client define macros to
 * keep track of alternate names.
 *
 * Hydra_Software_Devel/48   3/2/10 1:22p erickson
 * SW7405-3892: clarify comments
 *
 * Hydra_Software_Devel/47   9/14/09 7:43p albertl
 * SW7630-7: Added 7630 support.
 *
 * Hydra_Software_Devel/46   9/4/09 3:45p albertl
 * SW7630-7: Added symbolic link "7630".
 *
 * Hydra_Software_Devel/45   6/16/09 6:21p albertl
 * PR55764, PR55875, PR55993: Refactored MRC to keep chip specific
 * definitions in bmrc_priv.h and for maintainability.  Updated support
 * for 7420 b0, 7430, 35130, and 7550.
 *
 * Hydra_Software_Devel/44   6/8/09 7:25p albertl
 * PR55764: Added 7340 support.
 *
 * Hydra_Software_Devel/43   6/5/09 6:30p albertl
 * PR55676: Added 7550 support.
 *
 * Hydra_Software_Devel/42   6/4/09 7:05p albertl
 * PR55641: Allowed MRC to report maximum number of checkers and changed
 * BMRC_Monitor to use that instead of hardcoded maximums.
 *
 * Hydra_Software_Devel/41   3/30/09 5:25p albertl
 * PR52965: Fixed missing 7635 MRC support.
 *
 * Hydra_Software_Devel/40   9/11/08 6:02p albertl
 * PR46761: Updated for 7420.
 *
 * Hydra_Software_Devel/39   7/23/08 1:34p albertl
 * PR33688:  Fixed 7601 mrc build errors.
 *
 * Hydra_Software_Devel/38   7/22/08 6:14p albertl
 * PR33688:  Added support for 7601.
 *
 * Hydra_Software_Devel/37   3/5/08 10:19p pntruong
 * PR34853: Added ifdef for 3556.
 *
 * Hydra_Software_Devel/36   2/25/08 8:45p albertl
 * PR36876:  Fixed missing THD define.
 *
 * Hydra_Software_Devel/35   2/25/08 7:04p albertl
 * PR36876:  Rewrote MRC to abstract client names and streamline adding of
 * future chips.  3548 support added.
 *
 * Hydra_Software_Devel/34   12/7/07 11:09a yuxiaz
 * PR35322:  Set BMRC_Client_eMaxCount to back to 128 on 7325.
 *
 * Hydra_Software_Devel/33   12/6/07 10:01a erickson
 * PR36876: fix logic for BMRC_MAX_CLIENTS. 7405 was broken.
 *
 * Hydra_Software_Devel/32   11/19/07 3:22p tdo
 * PR36876: Add mrc support for 7335
 *
 * Hydra_Software_Devel/31   11/14/07 1:50p albertl
 * PR35322:  Updated mrc monitor to properly support 128 clients.
 *
 * Hydra_Software_Devel/30   11/8/07 5:02p yuxiaz
 * PR35322:  set BMRC_Client_eMaxCount to 64 for now.
 *
 * Hydra_Software_Devel/29   10/30/07 7:25p albertl
 * PR35322:  Updated mrc for 7325.
 *
 * Hydra_Software_Devel/28   10/19/07 3:47p jrubio
 * PR35015: adding intial 7325
 *
 * Hydra_Software_Devel/27   5/1/07 5:07p albertl
 * PR29633:  Updated and changed MRC for 7405.
 *
 * Hydra_Software_Devel/26   3/9/07 7:19p albertl
 * PR28182:  No longer programs ARC when aligned size is 0.  Memc id now
 * reported on violations.
 *
 * Hydra_Software_Devel/25   3/1/07 8:16p albertl
 * PR26116:  Fixed support for multiple memory modules for 7400 and 3563.
 *
 * Hydra_Software_Devel/24   1/17/07 7:48p albertl
 * PR19101:  Added missing bExclusive field from BMRC_CheckerInfo.
 *
 * Hydra_Software_Devel/22   1/17/07 6:39p albertl
 * PR19101:  Updated violation messages to indicate exclusive mode for
 * clarity.
 *
 * Hydra_Software_Devel/21   1/16/07 11:58a erickson
 * PR25037: change AI0 (with a zero) to AIO (with the letter 'o'). this
 * was a typo that makes for frustrating text searches.
 *
 * Hydra_Software_Devel/20   10/31/06 2:29p erickson
 * PR25108: add 7403 support
 *
 * Hydra_Software_Devel/19   10/13/06 8:12p albertl
 * PR19101:   NMBX value now returned upon violation.
 *
 * Hydra_Software_Devel/18   10/10/06 6:43p albertl
 * PR23111:  Added 7440 support using ARB block.
 *
 * Hydra_Software_Devel/17   8/8/06 6:15p albertl
 * PR23361:  Added 3563 support.
 *
 * Hydra_Software_Devel/16   6/22/06 2:18p albertl
 * PR21938:  Added 7118 support.
 *
 * Hydra_Software_Devel/15   6/13/06 6:07p syang
 * PR 21689: add support for 7118
 *
 * Hydra_Software_Devel/14   3/8/06 5:30p albertl
 * PR19101:  BMRC_Checker_SetRange now only accepts 8 byte alligned
 * addresses and sizes, and returns error otherwise.
 *
 * Hydra_Software_Devel/13   3/7/06 3:54p syang
 * PR 19670: added 7438 support
 *
 * Hydra_Software_Devel/12   3/3/06 7:25p albertl
 * PR18701:  Added functions to disable and enable a checker's callback.
 *
 * Hydra_Software_Devel/11   2/16/06 7:34p albertl
 * PR19101:  Updated tables for 7400.
 *
 * Hydra_Software_Devel/10   2/14/06 1:12p jasonh
 * PR 19565: Added support for 7438
 *
 * Hydra_Software_Devel/9   1/17/06 4:53p hongtaoz
 * PR19082: support 7400;
 *
 * Hydra_Software_Devel/8   11/29/05 2:23p albertl
 * PR18307:  Added SATA client to 7038 for Cx series.
 *
 * Hydra_Software_Devel/7   11/7/05 5:05p albertl
 * PR17403:  Fixed build errors on 97401.  Renamed clients to be
 * consistent with RTS architecture client documentation.
 *
 * Hydra_Software_Devel/6   11/4/05 6:48p albertl
 * PR17403:  Added BMRC_Client_eAVD_ILA_0 and BMRC_Client_eAVD_OLA_0
 * clients to xpt module.  Renamed client enums to standard names.
 *
 * Hydra_Software_Devel/5   9/23/05 7:06p hongtaoz
 * PR17131: added BMRC support for 7401;
 *
 * Hydra_Software_Devel/4   8/8/05 9:50p albertl
 * PR13641:  Added BMRC error codes.
 *
 * Hydra_Software_Devel/3   8/4/05 6:51p albertl
 * PR13641:  Fixed incorrect PR numbers in chagelog.
 *
 * Hydra_Software_Devel/1   8/1/05 10:12p albertl
 * PR13641:  Initial Revision.
 *
 ***************************************************************************/

#ifndef BMRC_H
#define BMRC_H

#include "bchp.h"                /* Chip information */
#include "berr_ids.h"            /* Error codes */
#include "bmem.h"                /* Chip memory access */
#include "bint.h"                /* Returning the interrupt ID of slot */

#ifdef __cplusplus
extern "C" {
#endif


/***************************************************************************
Summary:
	List of errors unique to MRC
****************************************************************************/
#define BMRC_CHECKER_ERR_ENABLED_CANT_SET        BERR_MAKE_CODE(BERR_MRC_ID, 0)
#define BMRC_CHECKER_ERR_ALL_USED                BERR_MAKE_CODE(BERR_MRC_ID, 1)
#define BMRC_CHECKER_ERR_NO_CALLBACK_SET         BERR_MAKE_CODE(BERR_MRC_ID, 2)

/***************************************************************************
Summary:
	Memory checker access types.

Description:
	The different types of memory accesses handled by the memory range
	checkers.

See Also:
	BMRC_Checker_SetAccessCheck
	BMRC_Checker_SetClient
****************************************************************************/
typedef enum
{
	BMRC_AccessType_eNone,
	BMRC_AccessType_eRead,
	BMRC_AccessType_eWrite,
	BMRC_AccessType_eBoth
} BMRC_AccessType;


/***************************************************************************
Summary:
	List of memory ranger checker clients.

Description:
	This is the enumerated list of clients that the Memory Range Checker
	module checks and grants memory access rights to.

See Also:
	BMRC_Checker_SetClient
****************************************************************************/
typedef enum
{

	BMRC_Client_eXPT_0,
	BMRC_Client_eXPT_1,
	BMRC_Client_eXPT_2,
	BMRC_Client_eXPT_3,
	BMRC_Client_eXPT_4,
	BMRC_Client_eXPT_5,
	BMRC_Client_eXPT_6,

	BMRC_Client_eXPT_WR_0,
	BMRC_Client_eXPT_WR_1,
	BMRC_Client_eXPT_WR_2,
	BMRC_Client_eXPT_RD_0,
	BMRC_Client_eXPT_RD_1,
	BMRC_Client_eXPT_RD_2,

	BMRC_Client_eXPT_MEM_DMA,

	BMRC_Client_eXPT_WR_RS,
	BMRC_Client_eXPT_WR_XC,
	BMRC_Client_eXPT_WR_CDB,
	BMRC_Client_eXPT_WR_ITB_MSG,
	BMRC_Client_eXPT_RD_RS,
	BMRC_Client_eXPT_RD_XC_RMX_MSG,
	BMRC_Client_eXPT_RD_XC_RAVE,
	BMRC_Client_eXPT_RD_PB,
	BMRC_Client_eXPT_WR_MEMDMA,
	BMRC_Client_eXPT_RD_MEMDMA,

	BMRC_Client_eANR_0_REC_CAP,
	BMRC_Client_eANR_0_REC_VFD,

	BMRC_Client_eMBR_CAP_0,
	BMRC_Client_eMBR_CAP_1,
	BMRC_Client_eMBR_VFD_0,

	BMRC_Client_eM2MC_0,
	BMRC_Client_eM2MC_0_VC4_0,
	BMRC_Client_eM2MC_0_VC4_1,
	BMRC_Client_eM2MC_1,

	BMRC_Client_eMTP_0_SREWR,
	BMRC_Client_eMTP_0_SRERD,
	BMRC_Client_eMTP_0_MREWR,
	BMRC_Client_eMTP_0_MRERD,
	BMRC_Client_eMTP_0_VDMA,
	BMRC_Client_eMTP_0_MTT,

	BMRC_Client_eMTP_1_SREWR,
	BMRC_Client_eMTP_1_SRERD,
	BMRC_Client_eMTP_1_MREWR,
	BMRC_Client_eMTP_1_MRERD,
	BMRC_Client_eMTP_1_VDMA,
	BMRC_Client_eMTP_1_MTT,

	BMRC_Client_eAVD_0_BLK,
	BMRC_Client_eAVD_0_BLK_1,
	BMRC_Client_eAVD_0_ILA,
	BMRC_Client_eAVD_0_OLA,
	BMRC_Client_eAVD_0_CAB,
	BMRC_Client_eAVD_0_SYM,
	BMRC_Client_eAVD_0_PFR,
	BMRC_Client_eAVD_0_MCP,

	BMRC_Client_eAVD_1_BLK,
	BMRC_Client_eAVD_1_BLK_1,
	BMRC_Client_eAVD_1_ILA,
	BMRC_Client_eAVD_1_OLA,
	BMRC_Client_eAVD_1_CAB,
	BMRC_Client_eAVD_1_SYM,
	BMRC_Client_eAVD_1_PFR,
	BMRC_Client_eAVD_1_MCP,

	BMRC_Client_eSVD_0_BLK,
	BMRC_Client_eSVD_0_BLK_1,
	BMRC_Client_eSVD_0_ILA,
	BMRC_Client_eSVD_0_OLA,
	BMRC_Client_eSVD_0_CAB,
	BMRC_Client_eSVD_0_ILSYM,
	BMRC_Client_eSVD_0_BLA,
	BMRC_Client_eSVD_0_BLSYM,
	BMRC_Client_eSVD_0_MVSCL,
	BMRC_Client_eSVD_0_SPIXSTR,
	BMRC_Client_eSVD_0_PFR,
	BMRC_Client_eSVD_0_MCP,

	BMRC_Client_eSHVD_0_BLK,
	BMRC_Client_eSHVD_0_BLK_1,
	BMRC_Client_eSHVD_0_ILA,
	BMRC_Client_eSHVD_0_OLA,
	BMRC_Client_eSHVD_0_CAB,
	BMRC_Client_eSHVD_0_ILSYM,
	BMRC_Client_eSHVD_0_BLA,
	BMRC_Client_eSHVD_0_BLSYM,

	BMRC_Client_eHVD_0_BLK,
	BMRC_Client_eHVD_0_BLK_1,
	BMRC_Client_eHVD_0_ILA,
	BMRC_Client_eHVD_0_OLA,
	BMRC_Client_eHVD_0_CAB,
	BMRC_Client_eHVD_0_ILSYM,

	BMRC_Client_eHVD_1_BLK,
	BMRC_Client_eHVD_1_BLK_1,
	BMRC_Client_eHVD_1_ILA,
	BMRC_Client_eHVD_1_OLA,
	BMRC_Client_eHVD_1_CAB,
	BMRC_Client_eHVD_1_ILSYM,

	BMRC_Client_eMCP_R_HI,
	BMRC_Client_eMCP_R_LO,
	BMRC_Client_eMCP_W_HI,
	BMRC_Client_eMCP_W_LO,

	BMRC_Client_eBLKAVG_0,
	BMRC_Client_eBLKAVG_1,

	BMRC_Client_eBACH_PCM,
	BMRC_Client_eBACH_ADP_0_RD,
	BMRC_Client_eBACH_ADP_1_RD,
	BMRC_Client_eBACH_ADP_SPDIF_RD,
	BMRC_Client_eBACH_ADP_0_WR,
	BMRC_Client_eBACH_ADP_1_WR,
	BMRC_Client_eBACH_ADP_SPDIF_WR,

	BMRC_Client_eAUD_ZSP,
	BMRC_Client_eAUD_AIO,

	BMRC_Client_eRAPTOR_0,
	BMRC_Client_eRAPTOR_1,

	BMRC_Client_eVDEC_0_TTX,
	BMRC_Client_eVDEC_0_3D,
	BMRC_Client_eVDEC_0_656,
	BMRC_Client_eVDEC_1_3D,

	BMRC_Client_eVDEC_0_WR_0,
	BMRC_Client_eVDEC_0_RD_0,
	BMRC_Client_eVDEC_0_RD_1,

	BMRC_Client_eAVFE_VDEC_0_TTX,
	BMRC_Client_eAVFE_VDEC_0_MD_RD,
	BMRC_Client_eAVFE_VDEC_0_MD_WR,
	BMRC_Client_eAVFE_VDEC_0_VD_RD,
	BMRC_Client_eAVFE_VDEC_0_VD_WR,
	BMRC_Client_eAVFE_VIDBLK_CAP0,
	BMRC_Client_eAVFE_VIDBLK_CAP1,

	BMRC_Client_eVEC_0_VBI_ENC,
	BMRC_Client_eVEC_1_VBI_ENC,
	BMRC_Client_eVEC_2_VBI_ENC,

	BMRC_Client_eVEC_0,
	BMRC_Client_eVEC_1,

	BMRC_Client_eI656_ANC_0,
	BMRC_Client_eI656_ANC_1,

	BMRC_Client_eAEGIS,
	BMRC_Client_eAEGIS_M2M,

	BMRC_Client_eBVNB_GFD_0,
	BMRC_Client_eBVNB_GFD_1,
	BMRC_Client_eBVNB_GFD_2,
	BMRC_Client_eBVNB_GFD_3,
	BMRC_Client_eBVNB_GFD_4,
	BMRC_Client_eBVNB_GFD_5,
	BMRC_Client_eBVNB_GFD_6,
	BMRC_Client_eBVNB_GFD_CCC,
	BMRC_Client_eBVNB_CAP_0,
	BMRC_Client_eBVNB_CAP_1,
	BMRC_Client_eBVNB_CAP_2,
	BMRC_Client_eBVNB_CAP_3,
	BMRC_Client_eBVNB_CAP_4,
	BMRC_Client_eBVNB_CAP_5,
	BMRC_Client_eBVNB_CAP_6,
	BMRC_Client_eBVNB_CAP_7,

	BMRC_Client_eBVNF_RDC_0,
	BMRC_Client_eBVNF_VFD_0,
	BMRC_Client_eBVNF_VFD_1,
	BMRC_Client_eBVNF_VFD_2,
	BMRC_Client_eBVNF_VFD_3,
	BMRC_Client_eBVNF_VFD_4,
	BMRC_Client_eBVNF_VFD_5,
	BMRC_Client_eBVNF_VFD_6,
	BMRC_Client_eBVNF_VFD_7,
	BMRC_Client_eBVNF_MFD_0,
	BMRC_Client_eBVNF_MFD_0_1,
	BMRC_Client_eBVNF_MFD_1,
	BMRC_Client_eBVNF_MFD_1_1,
	BMRC_Client_eBVNF_MFD_2,
	BMRC_Client_eBVNF_MFD_2_1,
	BMRC_Client_eBVNF_MFD_3,
	BMRC_Client_eBVNF_MFD_3_1,
	BMRC_Client_eBVNF_MFD_4,
	BMRC_Client_eBVNF_MFD_4_1,
	BMRC_Client_eBVNF_MFD_5,
	BMRC_Client_eBVNF_MFD_5_1,

	BMRC_Client_eBVNM_FGT_0,
	BMRC_Client_eBVN_FGT_0,

	BMRC_Client_eBVN_FGT_0_BLKAVG_RD,
	BMRC_Client_eBVN_FGT_0_RDMA,
	BMRC_Client_eBVN_FGT_0_WR,

	BMRC_Client_eBVN_MCVP_0,
	BMRC_Client_eBVN_MCVP_1,
	BMRC_Client_eBVN_MCVP_2,

	BMRC_Client_eBVN_DNR_RW,
	BMRC_Client_eBVN_JFD_0,
	BMRC_Client_eBVN_MCVP_0_RD,
	BMRC_Client_eBVN_MCVP_0_RW_0,
	BMRC_Client_eBVN_MCVP_0_RW_1,

	BMRC_Client_eBVN_PDP_RD_0,
	BMRC_Client_eBVN_PDP_WR_0,
	BMRC_Client_eBVN_PDP_RD_1,
	BMRC_Client_eBVN_PDP_WR_1,
	BMRC_Client_eBVN_PDP_WR_2,
	BMRC_Client_eBVN_PDP_WR_3,

	BMRC_Client_eBVN_FRC_0_CE,
	BMRC_Client_eBVN_FRC_0_TMRV,
	BMRC_Client_eBVN_FRC_0_PROF,
	BMRC_Client_eBVN_FRC_0_WS2,
	BMRC_Client_eBVN_FRC_0_WS1B,
	BMRC_Client_eBVN_FRC_0_WS1,
	BMRC_Client_eBVN_FRC_0_WS0,
	BMRC_Client_eBVN_FRC_0_RSM,
	BMRC_Client_eBVN_FRC_0_RS2,
	BMRC_Client_eBVN_FRC_0_RS1,
	BMRC_Client_eBVN_FRC_0_RS0P,
	BMRC_Client_eBVN_FRC_0_RS0N,
	BMRC_Client_eBVN_FRC_0_SRD,
	BMRC_Client_eBVN_FRC_0_RS1B,
	BMRC_Client_eBVN_FRC_0_WSM,
	BMRC_Client_eBVN_FRC_0_WSPL,
	BMRC_Client_eBVN_FRC_0_RSPL,
	BMRC_Client_eBVN_FRC_0_WSEG,
	BMRC_Client_eBVN_FRC_0_RSEG,

	BMRC_Client_eV3D_0,
	BMRC_Client_eV3D_1,

	BMRC_Client_eB3DC,

	BMRC_Client_eHIF_MISB,
	BMRC_Client_eHIF_PCI_DMA,
	BMRC_Client_eHIF_PCIE_DMA,
	BMRC_Client_eHIF_EBI_DMA,
	BMRC_Client_eHIF_M2M_DMA_0,
	BMRC_Client_eHIF_M2M_DMA_1,

	BMRC_Client_eHIF_CPU_CACHE_0,
	BMRC_Client_eHIF_CPU_CACHE_1,

	BMRC_Client_eMIPS_0_RD,
	BMRC_Client_eMIPS_0_WR,
	BMRC_Client_eMIPS_1_RD,
	BMRC_Client_eMIPS_1_WR,

	BMRC_Client_eMOCA_DATA_RD,
	BMRC_Client_eMOCA_DATA_WR,
	BMRC_Client_eMOCA_MIPS,

	BMRC_Client_eFLASH_DMA,

	BMRC_Client_ePC_VBI_TTX_REC,
	BMRC_Client_ePC_VBI_656_REC,
	BMRC_Client_ePC_VBI_FD,

	BMRC_Client_eENET_0,
	BMRC_Client_eENET_1,
	BMRC_Client_ePIX_3D,
	BMRC_Client_eGPORT,

	BMRC_Client_eGENET0_RD,
	BMRC_Client_eGENET0_WR,
	BMRC_Client_eGENET1_RD,
	BMRC_Client_eGENET1_WR,
	BMRC_Client_eGENET2_RD,
	BMRC_Client_eGENET2_WR,

	BMRC_Client_eUSB_X_WR_0,
	BMRC_Client_eUSB_X_RD_0,
	BMRC_Client_eUSB_X_CTRL_0,

	BMRC_Client_eUSB20_0,
	BMRC_Client_eUSB20_1,
	BMRC_Client_eUSB11_0,
	BMRC_Client_eUSB11_1,

	BMRC_Client_eUSB_0_0,
	BMRC_Client_eUSB_0_1,
	BMRC_Client_eUSB_1_0,
	BMRC_Client_eUSB_1_1,

	BMRC_Client_eIEEE_1394,

	BMRC_Client_eMAD_0_PIX,

	BMRC_Client_eMAD_0_PIX_CAP,
	BMRC_Client_eMAD_0_PIX_FD,

	BMRC_Client_eMAD_0_FD_0,
	BMRC_Client_eMAD_0_FD_1,
	BMRC_Client_eMAD_0_FD_2,
	BMRC_Client_eMAD_0_FD_3,
	BMRC_Client_eMAD_0_CAP,
	BMRC_Client_eMAD_0_QM,

	BMRC_Client_eMAD_0_A_WR,
	BMRC_Client_eMAD_0_B_RD,
	BMRC_Client_eMAD_0_D_RD,
	BMRC_Client_eMAD_0_G_RD,
	BMRC_Client_eMAD_0_J_RD,

	BMRC_Client_eMAD_1_PIX_CAP,
	BMRC_Client_eMAD_1_PIX_FD,

	BMRC_Client_eMAD_1_FD_0,
	BMRC_Client_eMAD_1_FD_1,
	BMRC_Client_eMAD_1_FD_2,
	BMRC_Client_eMAD_1_FD_3,
	BMRC_Client_eMAD_1_CAP,
	BMRC_Client_eMAD_1_QM,

	BMRC_Client_eMAD_2_PIX_CAP,
	BMRC_Client_eMAD_2_PIX_FD,
	BMRC_Client_eMAD_2_QM,

	BMRC_Client_eMAD_3_PIX_CAP,
	BMRC_Client_eMAD_3_PIX_FD,
	BMRC_Client_eMAD_3_QM,

	BMRC_Client_eADC_CAP,

	BMRC_Client_eSUN_MCIF_RD,
	BMRC_Client_eSUN_MCIF_WR,
	BMRC_Client_eIDE,
	BMRC_Client_eSATA,
	BMRC_Client_eSATA_1,

	BMRC_Client_eMEMC64_SOFT_0,
	BMRC_Client_eMEMC64_REF_0,
	BMRC_Client_eMEMC64_REF_1,
	BMRC_Client_eMEMC64_DIS_0,
	BMRC_Client_eMEMC64_DIS_1,
	BMRC_Client_eMEMC32_SOFT_0,
	BMRC_Client_eMEMC32_REF_0,
	BMRC_Client_eMEMC16_0_SOFT_0,
	BMRC_Client_eMEMC16_0_REF_0,
	BMRC_Client_eMEMC16_1_SOFT_0,
	BMRC_Client_eMEMC16_1_REF_0,

	BMRC_Client_eDRAM_INIT,

	BMRC_Client_eBNM_MIPS,
	BMRC_Client_eBNM_UBUS,
	BMRC_Client_eBNM_DMA,

	BMRC_Client_eSDIO,
	BMRC_Client_eSID,
	BMRC_Client_eUART_DMA,
	BMRC_Client_eUART_DMA_RD,
	BMRC_Client_eUART_DMA_WR,
	BMRC_Client_eSDIO_EMMC,
	BMRC_Client_eSDIO_CARD,

	BMRC_Client_eNRDF,

	BMRC_Client_eOFE_ARM,
	BMRC_Client_eOFE_PERF,
	BMRC_Client_eOFE_HOST,
	BMRC_Client_eOFE_DEC,

	BMRC_Client_eMUD_RD,
	BMRC_Client_eMUD_WR,
	BMRC_Client_eTRAP,
	BMRC_Client_eSHARF,

	BMRC_Client_eBCC,
	BMRC_Client_eTRACE,

	BMRC_Client_eTHD,

	BMRC_Client_eTPCAP,

	BMRC_Client_eSCPU,

	BMRC_Client_ePCU,
	BMRC_Client_ePCU_DMAS,
	BMRC_Client_eRAAGA_0_0,
	BMRC_Client_eRAAGA_0_1,
	BMRC_Client_eRAAGA_1_0,
	BMRC_Client_eRAAGA_1_1,
	BMRC_Client_eTDI_RD,
	BMRC_Client_eTDI_WR,

	BMRC_Client_eVICE_0_VIP_0_0,
	BMRC_Client_eVICE_0_VIP_0_1,
	BMRC_Client_eVICE_0_VIP_0_2,
	BMRC_Client_eVICE_0_VIP_1_0,
	BMRC_Client_eVICE_0_VIP_1_1,
	BMRC_Client_eVICE_0_VIP_1_2,
	BMRC_Client_eVICE_0_CME_0,
	BMRC_Client_eVICE_0_CME_1,
	BMRC_Client_eVICE_0_FME_0,
	BMRC_Client_eVICE_0_FME_1,
	BMRC_Client_eVICE_0_FME_2,
	BMRC_Client_eVICE_0_SG,
	BMRC_Client_eVICE_0_BLK,
	BMRC_Client_eVICE_0_CAB_0,
	BMRC_Client_eVICE_0_CAB_1,
	BMRC_Client_eVICE_0_ARCSS_0,
	BMRC_Client_eVICE_0_PFR_REQ,
	BMRC_Client_eVICE_0_PFR_OPT,
	BMRC_Client_eVICE_0_PFR,

	BMRC_Client_eVICE_1_VIP_0_0,
	BMRC_Client_eVICE_1_VIP_0_1,
	BMRC_Client_eVICE_1_VIP_1_0,
	BMRC_Client_eVICE_1_VIP_1_1,
	BMRC_Client_eVICE_1_CME_0,
	BMRC_Client_eVICE_1_CME_1,
	BMRC_Client_eVICE_1_FME_0,
	BMRC_Client_eVICE_1_FME_1,
	BMRC_Client_eVICE_1_FME_2,
	BMRC_Client_eVICE_1_SG,
	BMRC_Client_eVICE_1_BLK,
	BMRC_Client_eVICE_1_CAB_0,
	BMRC_Client_eVICE_1_CAB_1,
	BMRC_Client_eVICE_1_ARCSS_0,
	BMRC_Client_eVICE_1_PFR,

	BMRC_Client_eZQCS_0,
	BMRC_Client_eZQCS_1,

	/* NOTE: If you modify this list, you will likely need to modify BMRC_P_Monitor_astHwClients[] in bmrc_monitor_clients.c and
	BMRC_P_astClientTbl[] in bmrc_clienttable_priv.c. */

	BMRC_Client_eMaxCount,
	BMRC_Client_eInvalid = BMRC_Client_eMaxCount       /* must be last */

} BMRC_Client;

/* Alternate client names used by some chips for the same client */
#define BMRC_Client_eCPU_BRIDGE   BMRC_Client_eHIF_MISB
#define BMRC_Client_eHIF_CPU      BMRC_Client_eHIF_MISB
#define BMRC_Client_eMEM_DMA      BMRC_Client_eHIF_M2M_DMA_0
#define BMRC_Client_eHIF_EDU      BMRC_Client_eHIF_EBI_DMA
#define	BMRC_Client_eMAD_0_QUANT  BMRC_Client_eMAD_0_QM


/***************************************************************************
Summary:
	The handle for the MRC module.

Description:
	This is the main module handle required in order to create checkers.

See Also:
	BMRC_Open
****************************************************************************/
typedef struct BMRC_P_Context *BMRC_Handle;


/***************************************************************************
Summary:
	The handle for memory access checkers.

Description:
	Handle for memory access checkers returned by BMRC_Checker_Create.

See Also:
	BMRC_Checker_Create
****************************************************************************/
typedef struct BMRC_P_CheckerContext *BMRC_Checker_Handle;


/***************************************************************************
Summary:
	This structure contains the Memory Range Checker module settings.

Description:
	Configures the Memory Range Checker module when it is opened.

See Also:
	BMRC_Open
***************************************************************************/
typedef struct BMRC_Settings
{
	uint16_t usMemcId; /* Selects which memc to use */
} BMRC_Settings;


/***************************************************************************
Summary:
	This structure contains a client's information.

Description:
	This information is returned by BMRC_Checker_GetClientInfo.
	The translation from eClient to usClientId is done in BMRC_P_astClientTbl[] which is in bmrc_clienttable_priv.c.
***************************************************************************/
typedef struct BMRC_ClientInfo
{
	const char     *pchClientName;
	BMRC_Client     eClient;    /* SW client enum */
	uint16_t        usClientId; /* HW client bit position (values 0..127 spanning 4 32-bit registers) */
} BMRC_ClientInfo;


/***************************************************************************
Summary:
	This structure contains the checker and access violation information
	returned to a registered callback.

Description:
	BMRC_CheckerInfo provides the checker id, memory range, address that
	violated the checker, request type, nmbx id, and the client responsible
	for the violation.
***************************************************************************/
typedef struct BMRC_CheckerInfo
{
	uint16_t        usMemcId;     /* memory controller id */
	uint16_t        usCheckerId;  /* checker id */
	uint32_t        ulStart;      /* memory range start */
	uint32_t        ulSize;       /* memory range size */
	uint32_t        ulAddress;    /* start address that violated the range checker */
	uint32_t        ulAddressEnd; /* end address that violated the range checker */
#if (BCHP_CHIP != 7440) && (BCHP_CHIP != 7601) && (BCHP_CHIP != 7635) && (BCHP_CHIP != 7630) && (BCHP_CHIP != 7640)
	uint32_t        ulReqType;    /* req type */
	uint32_t        ulNmbxId;     /* nmbx id */
	uint32_t        ulNmbx;       /* nmbx */
#else
	uint32_t        ulLength;
	uint32_t        ulMode;
	uint32_t        ulWrite;
#endif
	uint16_t        usClientId;    /* client that violated the range checker */
	const char     *pchClientName; /* client name */
	bool            bExclusive;    /* exclusive mode */
} BMRC_CheckerInfo;


/***************************************************************************
Summary:
	Prototype of a memory range checker callback function.

Description:
	Upper level applications register callbacks to a checker that will be
	executed when a checker violation occurs.

See Also:
	BMRC_Checker_SetCallback
**************************************************************************/
typedef void (*BMRC_CallbackFunc_isr)( void *pvData1, int iData2, BMRC_CheckerInfo *pInfo);


/***************************************************************************
Summary:
	Gets the default settings.

Description:
	Fills the default settings structure with default values.

Returns:
	BERR_SUCCESS - Handle was successfully closed.
	BERR_INVALID_PARAMETER - Handle was invalid.

See Also:
	BMRC_Open
**************************************************************************/
BERR_Code BMRC_GetDefaultSettings
	( BMRC_Settings *pDefSettings  /* [out] Default settings structure to fill */
	);


/***************************************************************************
Summary:
	Opens the MRC module.

Description:
	Opens the Memory Range Checker module and creates its context.

Returns:
	BERR_SUCCESS - Handle was successfully created.
	BERR_OUT_OF_SYSTEM_MEMORY - Unable to allocate memory for the handle.

See Also:
	BMRC_Close
**************************************************************************/
BERR_Code BMRC_Open
	( BMRC_Handle                     *phMrc,        /* [out] MRC handle to be returned */
	  BREG_Handle                      hRegister,    /* [in] Register access handle */
	  BINT_Handle                      hInterrupt,   /* [in] Interrupt handle */
	  const BMRC_Settings             *pDefSettings  /* [in] Default settings */
	);


/***************************************************************************
Summary:
	Closes the MRC module.

Description:
	Closes the Memory Range Checker module and its context.

Returns:
	BERR_SUCCESS - Handle was successfully closed.
	BERR_INVALID_PARAMETER - Handle was invalid.

See Also:
	BMRC_Open
**************************************************************************/
BERR_Code BMRC_Close
	( BMRC_Handle hMrc /* [in] MRC handle to close */
	);


/***************************************************************************
Summary:
	Return settings passed into BMRC_Open
**************************************************************************/
void BMRC_GetSettings
	( BMRC_Handle hMrc,                     /* [in] MRC handle to close */
	  BMRC_Settings             *pSettings  /* [out] */
	);

/***************************************************************************
Summary:
	Gets maximum number of MRC checkers.

Description:
	Gets maximum number of checkers available to this MRC isntance, depending
	on its associated memory controller.

Returns:
	BERR_SUCCESS - Handle was successfully closed.
	BERR_INVALID_PARAMETER - Handle was invalid.

See Also:
**************************************************************************/
BERR_Code BMRC_GetMaxCheckers
	( BMRC_Handle hMrc,         /* [in] MRC handle */
	  uint32_t *pulMaxCheckers  /* [out] Maximum number of checkers for this MRC */
	);


/***************************************************************************
Summary:
	Creates a memory range access Checker.

Description:
	Creates a checker that can be used to check memory access to a certain
	range in memory, with the ability to check for specific types of
	violations by specific clients based on how the checker is configured.

Returns:
	BERR_SUCCESS - Checker handle was successfully created.
	BERR_INVALID_PARAMETER - Mrc handle was invalid.
	BMRC_CHECKER_ERR_ALL_USED - Maximum number of checkers reached and no
	                            additional checkers can be created.


See Also:
	BMRC_Checker_Destroy
**************************************************************************/
BERR_Code BMRC_Checker_Create
	( BMRC_Handle hMrc,               /* [in] MRC Module Handle */
	  BMRC_Checker_Handle *phChecker  /* [out] Checker handle to be returned */
	);


/***************************************************************************
Summary:
	Destroys a memory range access Checker.

Description:
	Disables and destroys an existing checker and cleans up its resources.

Returns:
	BERR_SUCCESS - Checker handle was successfully destroyed.
	BERR_INVALID_PARAMETER - Checker handle was invalid.

See Also:
	BMRC_Checker_Create
**************************************************************************/
BERR_Code BMRC_Checker_Destroy
	( BMRC_Checker_Handle hChecker  /* [in] Checker handle to be destroyed */
	);


/***************************************************************************
Summary:
	Sets a checker's memory access range.

Description:
	Configures the memory range that a checker is to check, based on a
	given memory address and range size.  The function cannot be called
	when a checker is already enabled.

Returns:
	BERR_SUCCESS - Checker range was successfully set.
	BERR_INVALID_PARAMETER - Checker handle was invalid, ulStart address
							 or ulSize in bytes aren't 8 byte aligned.
	BMRC_CHECKER_ERR_ENABLED_CANT_SET - Checker already enabled, cannot
	                                    be set.

See Also:
**************************************************************************/
BERR_Code BMRC_Checker_SetRange
	( BMRC_Checker_Handle hChecker,  /* [in] Checker handle */
	  uint32_t ulStart,              /* [in] Memory range start address */
	  uint32_t ulSize                /* [in] Memory range size */
	);


/***************************************************************************
Summary:
	Configures what types of accesses a checker should check for.

Description:
	This function sets up what sorts of accesses a checker checks.

	Setting eAccessType to BMRC_AccessType_eRead causes violations to occur
	only when clients without read rights attempt to read from the checker's
	range.

	Setting eAccessType to BMRC_AccessType_eWrite causes violations to occur
	only when clients without write rights attempt to write from the
	checker's range.

	Setting eAccessType to BMRC_AccessType_eBoth causes violations when
	either unauthorized reads or writes occur.  The function cannot be
	called when a checker is already enabled.

Returns:
	BERR_SUCCESS - The access check type was successfully set.
	BERR_INVALID_PARAMETER - Checker handle was invalid, or
	                         BMRC_Access_eNone was used as the
	                         access type to check.
	BMRC_CHECKER_ERR_ENABLED_CANT_SET - Checker already enabled, cannot
	                                    be set.

See Also:
**************************************************************************/
BERR_Code BMRC_Checker_SetAccessCheck
	( BMRC_Checker_Handle hChecker,  /* [in] Checker handle */
	  BMRC_AccessType eAccessType    /* [in] Access type to check */
	);


/***************************************************************************
Summary:
	Configures what types of accesses are blocked in the event of a
	violation.

Description:
	This function blocks read and/or write access to a checker's range when
	a violation does occur.

	Setting eBlockType to BMRC_AccessType_eRead blocks further read
	accesses

	Setting eBlockType to BMRC_AccessType_eWrite blocks further write
	accesses

	Setting eBlockType to BMRC_AccessType_eBoth blocks both types of
	accesses when a violation occurs.

	The function cannot be called when a checker is already enabled.

Returns:
	BERR_SUCCESS - The block type was successfully set.
	BERR_INVALID_PARAMETER - Checker handle was invalid.
	BMRC_CHECKER_ERR_ENABLED_CANT_SET - Checker already enabled, cannot
	                                    be set.

See Also:
**************************************************************************/
BERR_Code BMRC_Checker_SetBlock
	( BMRC_Checker_Handle hChecker,  /* [in] Checker handle */
	  BMRC_AccessType eBlockType     /* [in] Access type to block on violations*/
	);


/***************************************************************************
Summary:
	Sets a checker's exclusive mode.

Description:
	When a checker is configured for exclusive mode, clients with read
	access to the checker's memory range can only read from that range, and
	clients	with write access can only write to that range.  Accesses
	to other memory ranges by these clients will cause a violation.	The
	function cannot be called when a checker is already enabled.

Returns:
	BERR_SUCCESS - The checker's exlusive mode was successfully set.
	BERR_INVALID_PARAMETER - Checker handle was invalid.
	BMRC_CHECKER_ERR_ENABLED_CANT_SET - Checker already enabled, cannot
	                                    be set.

See Also:
**************************************************************************/
BERR_Code BMRC_Checker_SetExclusive
	( BMRC_Checker_Handle hChecker,  /* [in] Checker handle */
	  bool bExclusive                /* [in] Enable/disable exclusive mode */
	);


/***************************************************************************
Summary:
	Configures a client's access rights to the checker's memory range.

Description:
	This function sets a client's permissions to access the checker's memory
	range.  A client can be given read rights, write rights, both, or none
	by setting eAccessType to BMRC_AccessType_eRead, BMRC_AccessType_eWrite,
	BMRC_AccessType_eBoth, or BMRC_AccessType_eNone.

Returns:
	BERR_SUCCESS - Client was successfully configured.
	BERR_INVALID_PARAMETER - Checker handle was invalid.
	BMRC_CHECKER_ERR_ENABLED_CANT_SET - Checker already enabled, cannot
	                                    be set.

See Also:
	BMRC_Checker_SetAccess
**************************************************************************/
BERR_Code BMRC_Checker_SetClient
	( BMRC_Checker_Handle hChecker,  /* [in] Checker handle */
	  BMRC_Client eClient,           /* [in] The client to configure */
	  BMRC_AccessType eAccessType    /* [in] The client's access rights */
	);


/***************************************************************************
Summary:
	Enables a client's access checking.

Description:
	This function enables a client's access checking and will produce
	violations when they occur.

Returns:
	BERR_SUCCESS - Checker was successfully enabled.
	BERR_INVALID_PARAMETER - Checker handle was invalid.
	BMRC_CHECKER_ERR_NO_CALLBACK_SET - No callback set.

See Also:
	BMRC_Checker_Disable
	BMRC_Checker_EnableCallback
	BMRC_Checker_DisableCallback
**************************************************************************/
BERR_Code BMRC_Checker_Enable
	( BMRC_Checker_Handle hChecker  /* [in] Checker handle */
	);


/***************************************************************************
Summary:
	Disables a client's access checking.

Description:
	This function disables a client's access checking.

Returns:
	BERR_SUCCESS - Checker was successfully disabled.
	BERR_INVALID_PARAMETER - Checker handle was invalid.

See Also:
	BMRC_Checker_Enable
	BMRC_Checker_EnableCallback
	BMRC_Checker_DisableCallback
	BMRC_Checker_EnableCallback_isr
	BMRC_Checker_DisableCallback_isr
**************************************************************************/
BERR_Code BMRC_Checker_Disable
	( BMRC_Checker_Handle hChecker  /* [in] Checker handle */
	);


/***************************************************************************
Summary:
	Enables a client's callback.

Description:
	This function enables a client's callback to be run when violations
	occur.  A checker must also be enabled for callbacks to work.

Returns:
	BERR_SUCCESS - Checker callback was successfully enabled.
	BERR_INVALID_PARAMETER - Checker handle was invalid.
	BMRC_CHECKER_ERR_NO_CALLBACK_SET - No callback set.

See Also:
	BMRC_Checker_Enable
	BMRC_Checker_Disable
	BMRC_Checker_DisableCallback
	BMRC_Checker_EnableCallback_isr
	BMRC_Checker_DisableCallback_isr
**************************************************************************/
BERR_Code BMRC_Checker_EnableCallback
	( BMRC_Checker_Handle hChecker  /* [in] Checker handle */
	);


/***************************************************************************
Summary:
	Disables a client's callback.

Description:
	This function disables a client's callback, and the callback will
	not be run even when violations occur.

Returns:
	BERR_SUCCESS - Checker callback was successfully disabled.
	BERR_INVALID_PARAMETER - Checker handle was invalid.

See Also:
	BMRC_Checker_Enable
	BMRC_Checker_Disable
	BMRC_Checker_EnableCallback
	BMRC_Checker_EnableCallback_isr
	BMRC_Checker_DisableCallback_isr
**************************************************************************/
BERR_Code BMRC_Checker_DisableCallback
	( BMRC_Checker_Handle hChecker  /* [in] Checker handle */
	);


/***************************************************************************
Summary:
	Enables a client's callback during an isr.

Description:
	This function enables a client's callback to be run when violations
	occur.  A checker must also be enabled for callbacks to work.

Returns:
	BERR_SUCCESS - Checker was successfully enabled.
	BERR_INVALID_PARAMETER - Checker handle was invalid.
	BMRC_CHECKER_ERR_NO_CALLBACK_SET - No callback set.

See Also:
	BMRC_Checker_DisableCallback_isr
	BMRC_Checker_Enable
	BMRC_Checker_Disable
	BMRC_Checker_EnableCallback
	BMRC_Checker_DisableCallback
**************************************************************************/
BERR_Code BMRC_Checker_EnableCallback_isr
	( BMRC_Checker_Handle hChecker  /* [in] Checker handle */
	);


/***************************************************************************
Summary:
	Disables a client's callback during an isr.

Description:
	This function disables a client's callback, and the callback will
	not be run even when violations occur.

Returns:
	BERR_SUCCESS - Checker was successfully disabled.
	BERR_INVALID_PARAMETER - Checker handle was invalid.

See Also:
	BMRC_Checker_EnableCallback_isr
	BMRC_Checker_Enable
	BMRC_Checker_Disable
	BMRC_Checker_EnableCallback
	BMRC_Checker_DisableCallback
**************************************************************************/
BERR_Code BMRC_Checker_DisableCallback_isr
	( BMRC_Checker_Handle hChecker  /* [in] Checker handle */
	);


/***************************************************************************
Summary:
	Registers a callback to the checker.

Description:
	This function registers the callback that will be called when a
	violation occurs.

Returns:
	BERR_SUCCESS - Callback was successfully registered.

See Also:
**************************************************************************/
BERR_Code BMRC_Checker_SetCallback
	( BMRC_Checker_Handle hChecker,          /* [in] Checker handle */
	  const BMRC_CallbackFunc_isr pfCbFunc,  /* [in] Pointer to the callback function */
	  void *pvCbData1,                       /* [in] User defined callback data structure. */
	  int iCbData2                           /* [in] User defined callback value */
	);


/**************************************************************************
Summary:
	Gets a client's info.

Description:
	This function returns the client's information, such as client id and
	name in a client info structure.

Returns:

See Also:
**************************************************************************/
BERR_Code BMRC_Checker_GetClientInfo
	( BMRC_Handle hMrc,               /* [in] MRC Module Handle */
	  BMRC_Client eClient,            /* [in]  The client to get the info of */
	  BMRC_ClientInfo *pClientInfo    /* [out] Pointer to the client info structure */
	);

/**************************************************************************
Summary:
	Suspend instance of BMRC_Checker

Description:
	This function prepares instance of BMRC to power off hardware.
    It would save its internal state to memory, and prevent any further activity.
    Sate could be restored by calling BMRC_Resume.

See Also:
    BMRC_Checker_Resume
**************************************************************************/
void BMRC_Standby(
        BMRC_Handle hMrc /* [in] MRC Module Handle */
        );

/**************************************************************************
Summary:
	Resumes instance of BMRC_Checker

Description:
	This function restores instance of BMRC after powering up hardware.
    It would restore its internal state from memory saved by BMRC_Standby, and allow activity.

See Also:
    BMRC_Standby
**************************************************************************/
void BMRC_Resume(
        BMRC_Handle hMrc /* [in] MRC Module Handle */
        );

#ifdef __cplusplus
}
#endif

#endif
/* End of File */
