/***************************************************************************
 *     Copyright (c) 2004-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmrc_monitor_clients.c $
 * $brcm_Revision: Hydra_Software_Devel/43 $
 * $brcm_Date: 4/25/12 3:10p $
 *
 * Module Description:
 *
 * Implementation of the Realtime Memory Monitor for 7038
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/commonutils/mrc/7346/bmrc_monitor_clients.c $
 * 
 * Hydra_Software_Devel/43   4/25/12 3:10p albertl
 * SW7346-796: Moved MFD access to VDE module.
 * 
 * Hydra_Software_Devel/42   4/25/12 3:04p albertl
 * SW7346-796: Updated client lists and added MFD clients to DSP files.
 * 
 * Hydra_Software_Devel/41   4/24/12 5:40p albertl
 * SW7346-796: Added new modules and enabled AEGIS/BSP access to needed
 * modules.
 * 
 * Hydra_Software_Devel/40   11/2/11 6:10p albertl
 * SW7435-12: Added 7435 support to MRC.
 * 
 * Hydra_Software_Devel/39   9/16/11 6:19p albertl
 * SW7425-1298: Updated clients for 7425.
 * 
 * Hydra_Software_Devel/38   3/23/11 4:44p albertl
 * SWDTV-5969: Updated MRC to be compatible with 35233 a0.
 * 
 * Hydra_Software_Devel/37   11/22/10 6:15p albertl
 * SW7422-15, SW7125-27: Fixed SVD and AVD client names.  Fixed wrong
 * client assignment for 7135.
 * 
 * Hydra_Software_Devel/36   10/11/10 11:25a jessem
 * SW7420-173: Removed VFD from HW client table.
 * 
 * Hydra_Software_Devel/SW7420-173/2   10/1/10 3:12p jessem
 * SW7420-173: Added VFD to VDC client table and removed VFD from HW
 * client table.
 *
 * Hydra_Software_Devel/SW7420-173/1   6/11/10 5:01p jessem
 * SW7420-173: Removed VFD clients from VDC list since VFD will be
 * accessing host memory when VFD is used as a source.
 * 
 * Hydra_Software_Devel/35   6/30/10 5:08p albertl
 * SW35230-498: Changed to use BMRC_ALLOW_PCU_TO_ACCESS_KERNEL flag.
 * 
 * Hydra_Software_Devel/34   6/30/10 7:20p jhaberf
 * SW35230-498: Allow PCU client to access kernel space on the 35230
 * 
 * Hydra_Software_Devel/33   6/29/10 7:40p albertl
 * SW7422-15: Updated for 7422 again and removed extraneous clients from
 * default hardware jail.
 * 
 * Hydra_Software_Devel/32   6/25/10 5:06p pntruong
 * SW7422-15: Rolled back support for 7422 for further review.
 * 
 * Hydra_Software_Devel/30   6/9/10 1:23p albertl
 * SW35230-250: Merged 35230 additions to monitor client lists.
 * 
 * Hydra_Software_Devel/SW35230-250/2   6/4/10 11:26a mbatchel
 * SW35230-250: Update vdc clients list for 35230.
 * 
 * Hydra_Software_Devel/SW35230-250/1   6/2/10 1:12p mbatchel
 * SW35230-250: Update blocked clients list for 35230.
 * 
 * Hydra_Software_Devel/29   3/24/10 5:35p albertl
 * SW7335-689: Updated hardware client list to be more complete and
 * ordered same as enum list.
 * 
 * Hydra_Software_Devel/28   3/18/10 11:55a erickson
 * SW7405-3892: change default list of non-OS HW clients to prevent
 * GFD/M2MC access to kernel memory. this is rarely done. added export
 * BMRC_ALLOW_GFX_TO_ACCESS_KERNEL=y compile time option to allow this
 * access.
 *
 * Hydra_Software_Devel/27   3/2/10 1:21p erickson
 * SW7405-3892: add new enum values to various arrays
 *
 * Hydra_Software_Devel/26   2/17/09 6:43p albertl
 * PR51612: Updated to correct naming conventions.
 *
 * Hydra_Software_Devel/25   11/3/08 7:19p albertl
 * PR47849: Removed M2MC_0 from list of clients monitored.
 *
 * Hydra_Software_Devel/24   10/6/08 6:11p albertl
 * PR47324: Removed GFD from the list of clients monitored.
 *
 * Hydra_Software_Devel/23   9/19/08 5:21p albertl
 * PR46837: Removed most chip specific defines and consolidated clients of
 * different platforms.  BMRC_Monitor implementation will ignore invalid
 * clients for a platform.
 *
 * Hydra_Software_Devel/22   9/11/08 6:09p albertl
 * PR46761: Fixed build errors.
 *
 * Hydra_Software_Devel/21   9/11/08 5:51p albertl
 * PR46761: Updated for 7420.
 *
 * Hydra_Software_Devel/20   3/5/08 10:20p pntruong
 * PR34853: Added ifdef for 3556.
 *
 * Hydra_Software_Devel/19   2/25/08 10:38p albertl
 * PR34853:  Added 3548 MRC support.  Fixed client naming errors.
 *
 * Hydra_Software_Devel/18   2/25/08 7:03p albertl
 * PR36876:  Rewrote MRC to abstract client names and streamline adding of
 * future chips.  3548 support added.
 *
 * Hydra_Software_Devel/17   12/14/07 3:49p tdo
 * PR36876: Add mrc support for 7335
 *
 * Hydra_Software_Devel/16   10/30/07 7:25p albertl
 * PR35322:  Updated mrc for 7325.
 *
 * Hydra_Software_Devel/15   6/8/07 5:09p mward
 * PR21938:  For 7118 don't exclude AVD clients from s_astHwClients.  They
 * do use common region.
 *
 * Hydra_Software_Devel/14   5/1/07 5:08p albertl
 * PR29633:  Updated and changed MRC for 7405.
 *
 * Hydra_Software_Devel/13   1/16/07 11:58a erickson
 * PR25037: change AI0 (with a zero) to AIO (with the letter 'o'). this
 * was a typo that makes for frustrating text searches.
 *
 * Hydra_Software_Devel/12   10/31/06 9:10a erickson
 * PR25108: add 7403 support
 *
 * Hydra_Software_Devel/11   10/25/06 11:43a pntruong
 * PR24997:  Since the MFD need to read memory from xvd/mvd decoded
 * buffers, it also need to be in the list of allow access for xvd/mvd.
 *
 * Hydra_Software_Devel/10   8/8/06 6:14p albertl
 * PR23361:  Added 3563 support.
 *
 * Hydra_Software_Devel/9   6/22/06 2:22p albertl
 * PR21938:  Added 7118 support.
 *
 * Hydra_Software_Devel/8   5/8/06 2:33p erickson
 * PR18701: fix AUD_ZSP name for 7401
 *
 * Hydra_Software_Devel/7   4/24/06 3:02p erickson
 * PR18701: fix AUD_AIO name for 7401
 *
 * Hydra_Software_Devel/6   3/7/06 3:53p syang
 * PR 19670: added 7438 support
 *
 * Hydra_Software_Devel/5   3/2/06 3:51p albertl
 * PR18701:  Fixed build errors for 7401 and 7400.
 *
 * Hydra_Software_Devel/4   3/1/06 5:27p albertl
 * PR18701:  Renamed structs to use private names.
 *
 * Hydra_Software_Devel/3   3/1/06 5:16p albertl
 * PR18701:  Updated to match include latest RMM updates.
 *
 * Hydra_Software_Devel/2   2/16/06 7:46p albertl
 * PR19101:  Fixed #elif for 7401.
 *
 * Hydra_Software_Devel/1   2/16/06 7:41p albertl
 * PR19101:  Moved chip specific tables to bmrc_monitor_clients.c.
 *
 ***************************************************************************/

#include "bmrc.h"
#include "bmrc_monitor_priv.h"

/* MVD and AUD modules for older chips */
static const BMRC_Client s_astMvdFileClients[] =
{
    BMRC_Client_eBVNF_MFD_0,
    BMRC_Client_eBVNF_MFD_1,
    BMRC_Client_eMTP_0_MTT,
    BMRC_Client_eMTP_0_VDMA,
    BMRC_Client_eMTP_0_MRERD,
    BMRC_Client_eMTP_0_MREWR,
    BMRC_Client_eMTP_0_SRERD,
    BMRC_Client_eMTP_0_SREWR,
    BMRC_Client_eMTP_1_MTT,
    BMRC_Client_eMTP_1_VDMA,
    BMRC_Client_eMTP_1_MRERD,
    BMRC_Client_eMTP_1_MREWR,
    BMRC_Client_eMTP_1_SRERD,
    BMRC_Client_eMTP_1_SREWR,

    BMRC_Client_eMaxCount
};

static const BMRC_Client s_astAudFileClients[] =
{
    BMRC_Client_eBACH_ADP_SPDIF_WR,
    BMRC_Client_eBACH_ADP_1_WR,
    BMRC_Client_eBACH_ADP_0_WR,
    BMRC_Client_eBACH_ADP_SPDIF_RD,
    BMRC_Client_eBACH_ADP_1_RD,
    BMRC_Client_eBACH_ADP_0_RD,
    BMRC_Client_eBACH_PCM,

    BMRC_Client_eMaxCount
};

/* XVD, XPT, and RAP modules for newer chips */
static const BMRC_Client s_astXvdFileClients[] =
{
	BMRC_Client_eBVNF_MFD_0,
	BMRC_Client_eBVNF_MFD_0_1,         
	BMRC_Client_eBVNF_MFD_1,           
	BMRC_Client_eBVNF_MFD_1_1,         
	BMRC_Client_eBVNF_MFD_2,           
	BMRC_Client_eBVNF_MFD_2_1,         
	BMRC_Client_eBVNF_MFD_3,           
	BMRC_Client_eBVNF_MFD_3_1,         
    BMRC_Client_eAVD_0_BLK,
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
	BMRC_Client_eAEGIS,

	BMRC_Client_eMaxCount
};

static const BMRC_Client s_astXptRaveFileClients[] =
{
    BMRC_Client_eAVD_0_BLK,
    BMRC_Client_eAVD_0_ILA,
    BMRC_Client_eAVD_0_OLA,
    BMRC_Client_eAVD_0_CAB,
    BMRC_Client_eAVD_0_SYM,
    BMRC_Client_eAUD_ZSP,
    BMRC_Client_eAUD_AIO,
    BMRC_Client_eXPT_0,
    BMRC_Client_eXPT_1,
    BMRC_Client_eXPT_2,
    BMRC_Client_eXPT_3,
    BMRC_Client_eXPT_WR_0,
    BMRC_Client_eXPT_WR_1,
    BMRC_Client_eXPT_WR_2,
    BMRC_Client_eXPT_RD_0,
    BMRC_Client_eXPT_RD_1,
    BMRC_Client_eXPT_RD_2,
	BMRC_Client_eAEGIS,

	BMRC_Client_eMaxCount,
};

/* Common file modules */
static const BMRC_Client s_astRapFileClients[] =
{
    BMRC_Client_eAUD_ZSP,
    BMRC_Client_eAUD_AIO,
	BMRC_Client_eAEGIS,

	BMRC_Client_eMaxCount
};

static const BMRC_Client s_astVdcFileClients[] =
{
	BMRC_Client_eBVNF_MFD_0,
	BMRC_Client_eBVNF_MFD_0_1,         
	BMRC_Client_eBVNF_MFD_1,           
	BMRC_Client_eBVNF_MFD_1_1,         
	BMRC_Client_eBVNF_MFD_2,           
	BMRC_Client_eBVNF_MFD_2_1,         
	BMRC_Client_eBVNF_MFD_3,           
	BMRC_Client_eBVNF_MFD_3_1,         
    BMRC_Client_eBVNF_VFD_0,
    BMRC_Client_eBVNF_VFD_1,
    BMRC_Client_eBVNF_VFD_2,
    BMRC_Client_eBVNF_VFD_3,
    BMRC_Client_eBVNF_VFD_4,
    BMRC_Client_eBVNF_VFD_5,
    BMRC_Client_eBVNB_CAP_0,
    BMRC_Client_eBVNB_CAP_1,
    BMRC_Client_eBVNB_CAP_2,
    BMRC_Client_eBVNB_CAP_3,
    BMRC_Client_eBVNB_CAP_4,
    BMRC_Client_eBVNB_CAP_5,
    BMRC_Client_eVEC_0_VBI_ENC,
    BMRC_Client_eVEC_1_VBI_ENC,
    BMRC_Client_eVEC_2_VBI_ENC,
    BMRC_Client_eVDEC_0_3D,
    BMRC_Client_eVDEC_1_3D,
    BMRC_Client_eVDEC_0_TTX,
    BMRC_Client_eVDEC_0_656,
    BMRC_Client_eVDEC_0_WR_0,
    BMRC_Client_eVDEC_0_RD_0,
    BMRC_Client_eVDEC_0_RD_1,
    BMRC_Client_eBVN_DNR_RW,
	BMRC_Client_eBVN_MCVP_0,
	BMRC_Client_eBVN_MCVP_1,
	BMRC_Client_eBVN_MCVP_2,
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
    BMRC_Client_eBVN_FRC_0_RSPL,
    BMRC_Client_eBVN_FRC_0_WSEG,
    BMRC_Client_eBVN_FRC_0_RSEG,
    BMRC_Client_eMAD_0_PIX,
    BMRC_Client_eMAD_0_PIX_CAP,
    BMRC_Client_eMAD_0_PIX_FD,
    BMRC_Client_eMAD_0_QUANT,

	BMRC_Client_eMaxCount
};

static const BMRC_Client s_astVbiFileClients[] =
{
    BMRC_Client_eVEC_0_VBI_ENC,
    BMRC_Client_eVEC_1_VBI_ENC,
    BMRC_Client_eVEC_2_VBI_ENC,
    BMRC_Client_eVDEC_0_TTX,

    BMRC_Client_eMaxCount
};

static const BMRC_Client s_astRdcFileClients[] =
{
    BMRC_Client_eBVNF_RDC_0,

	BMRC_Client_eMaxCount
};

static const BMRC_Client s_astSurFileClients[] =
{
    BMRC_Client_eBVNB_GFD_0,
    BMRC_Client_eBVNB_GFD_1,
    BMRC_Client_eBVNB_GFD_2,
    BMRC_Client_eBVNB_GFD_3,

	BMRC_Client_eMaxCount
};

static const BMRC_Client s_astGrcFileClients[] =
{
    BMRC_Client_eM2MC_0,
    BMRC_Client_eM2MC_1,

    BMRC_Client_eMaxCount
};

static const BMRC_Client s_astXptFileClients[] =
{
    BMRC_Client_eXPT_0,
    BMRC_Client_eXPT_1,
    BMRC_Client_eXPT_2,
    BMRC_Client_eXPT_3,
    BMRC_Client_eXPT_WR_0,
    BMRC_Client_eXPT_WR_1,
    BMRC_Client_eXPT_WR_2,
    BMRC_Client_eXPT_RD_0,
    BMRC_Client_eXPT_RD_1,
    BMRC_Client_eXPT_RD_2,
	BMRC_Client_eAEGIS,

    BMRC_Client_eMaxCount
};

static const BMRC_Client s_astPvrFileClients[] =
{
    BMRC_Client_eXPT_0,
    BMRC_Client_eXPT_1,
    BMRC_Client_eXPT_2,
    BMRC_Client_eXPT_3,
    BMRC_Client_eXPT_WR_0,
    BMRC_Client_eXPT_WR_1,
    BMRC_Client_eXPT_WR_2,
    BMRC_Client_eXPT_RD_0,
    BMRC_Client_eXPT_RD_1,
    BMRC_Client_eXPT_RD_2,

    BMRC_Client_eMaxCount
};

static const BMRC_Client s_astVceFileClients[] =
{
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
	BMRC_Client_eVICE_1_SG,
	BMRC_Client_eVICE_1_CAB_0,
	BMRC_Client_eVICE_1_CAB_1,
	BMRC_Client_eAEGIS,

    BMRC_Client_eMaxCount
};

static const BMRC_Client s_astSidFileClients[] =
{
	BMRC_Client_eSID,
	BMRC_Client_eAEGIS,

    BMRC_Client_eMaxCount
};

static const BMRC_Client s_astApeFileClients[] =
{
	BMRC_Client_eRAAGA_0_0,
	BMRC_Client_eRAAGA_0_1,
	BMRC_Client_eRAAGA_1_0,
	BMRC_Client_eRAAGA_1_1,

    BMRC_Client_eMaxCount
};

static const BMRC_Client s_astDspFileClients[] =
{
	BMRC_Client_eRAAGA_0_0,
	BMRC_Client_eRAAGA_0_1,
	BMRC_Client_eRAAGA_1_0,
	BMRC_Client_eRAAGA_1_1,
	BMRC_Client_eAUD_AIO,
	BMRC_Client_eMOCA_MIPS,
	BMRC_Client_eAEGIS,

	BMRC_Client_eMaxCount
};

static const BMRC_Client s_astVdeFileClients[] =
{
	BMRC_Client_eBVNF_MFD_0,
	BMRC_Client_eBVNF_MFD_0_1,         
	BMRC_Client_eBVNF_MFD_1,           
	BMRC_Client_eBVNF_MFD_1_1,         
	BMRC_Client_eBVNF_MFD_2,           
	BMRC_Client_eBVNF_MFD_2_1,         
	BMRC_Client_eBVNF_MFD_3,           
	BMRC_Client_eBVNF_MFD_3_1,         
	BMRC_Client_eRAAGA_0_0,
	BMRC_Client_eRAAGA_0_1,
	BMRC_Client_eRAAGA_1_0,
	BMRC_Client_eRAAGA_1_1,
	BMRC_Client_eMOCA_MIPS,
	BMRC_Client_eAEGIS,

	BMRC_Client_eMaxCount
};

static const BMRC_Client s_astVeeFileClients[] =
{
	BMRC_Client_eRAAGA_0_0,
	BMRC_Client_eRAAGA_0_1,
	BMRC_Client_eRAAGA_1_0,
	BMRC_Client_eRAAGA_1_1,
	BMRC_Client_eMOCA_MIPS,
	BMRC_Client_eAEGIS,

	BMRC_Client_eMaxCount
};

const BMRC_P_Monitor_FileClientInfo BMRC_P_Monitor_astFileClients[] =  {
    {"bmvd",      s_astMvdFileClients },
    {"baud",      s_astAudFileClients },
    {"bxpt_rave", s_astXptRaveFileClients },
    {"bxvd",      s_astXvdFileClients },
    {"brap",      s_astRapFileClients },
    {"bvdc",      s_astVdcFileClients },
    {"bvbi",      s_astVbiFileClients },
    {"brdc",      s_astRdcFileClients },
    {"bsur",      s_astSurFileClients },
    {"bgrc",      s_astGrcFileClients },
    {"bxpt",      s_astXptFileClients },
    {"bpvr",      s_astPvrFileClients },
    {"bvce",      s_astVceFileClients },
	{"bsid",      s_astSidFileClients },
	{"bape",      s_astApeFileClients },
	{"bdsp",      s_astDspFileClients },
	{"bvde",      s_astVdeFileClients },
	{"bvee",      s_astVeeFileClients },
};

const uint32_t BMRC_P_Monitor_FileClientsTblSize = (sizeof(BMRC_P_Monitor_astFileClients) / sizeof(BMRC_P_Monitor_FileClientInfo));

/* The following clients are only allowed to access the MEM region (i.e. not allowed to access the OS region) */
const BMRC_Client BMRC_P_Monitor_astHwClients[]= {
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

	BMRC_Client_eANR_0_REC_CAP,
	BMRC_Client_eANR_0_REC_VFD,

	BMRC_Client_eMBR_CAP_0,
	BMRC_Client_eMBR_CAP_1,
	BMRC_Client_eMBR_VFD_0,
	
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

	BMRC_Client_eBVNB_CAP_0,
	BMRC_Client_eBVNB_CAP_1,
	BMRC_Client_eBVNB_CAP_2,
	BMRC_Client_eBVNB_CAP_3,
	BMRC_Client_eBVNB_CAP_4,
	BMRC_Client_eBVNB_CAP_5,

	BMRC_Client_eBVNF_RDC_0,

	BMRC_Client_eBVNF_MFD_0,
	BMRC_Client_eBVNF_MFD_0_1,         
	BMRC_Client_eBVNF_MFD_1,           
	BMRC_Client_eBVNF_MFD_1_1,         
	BMRC_Client_eBVNF_MFD_2,           
	BMRC_Client_eBVNF_MFD_2_1,         
	BMRC_Client_eBVNF_MFD_3,           
	BMRC_Client_eBVNF_MFD_3_1,         

	BMRC_Client_eBVNM_FGT_0,
	BMRC_Client_eBVN_FGT_0,

	BMRC_Client_eBVN_FGT_0_BLKAVG_RD,
	BMRC_Client_eBVN_FGT_0_RDMA,
	BMRC_Client_eBVN_FGT_0_WR,

	BMRC_Client_eBVN_MCVP_0,
	BMRC_Client_eBVN_MCVP_1,
	BMRC_Client_eBVN_MCVP_2,           

	BMRC_Client_eBVN_DNR_RW,
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

	BMRC_Client_ePC_VBI_TTX_REC,
	BMRC_Client_ePC_VBI_656_REC,
	BMRC_Client_ePC_VBI_FD,
	
	BMRC_Client_eMAD_0_PIX,
	BMRC_Client_eMAD_0_QUANT,

	BMRC_Client_eMAD_0_FD_0,
	BMRC_Client_eMAD_0_FD_1,
	BMRC_Client_eMAD_0_FD_2,
	BMRC_Client_eMAD_0_FD_3,
	BMRC_Client_eMAD_0_CAP,
	BMRC_Client_eMAD_0_QM,

	BMRC_Client_eMAD_1_FD_0,
	BMRC_Client_eMAD_1_FD_1,
	BMRC_Client_eMAD_1_FD_2,
	BMRC_Client_eMAD_1_FD_3,
	BMRC_Client_eMAD_1_CAP,
	BMRC_Client_eMAD_1_QM,
	
	BMRC_Client_eTHD,

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
	BMRC_Client_eVICE_1_SG,
	BMRC_Client_eVICE_1_CAB_0,
	BMRC_Client_eVICE_1_CAB_1,

	BMRC_Client_eSID,

#if BMRC_ALLOW_PCU_TO_ACCESS_KERNEL 
    /* PCU DMA client is used for page swapping on the 35230 so */
    /* it requires access to the kernel space.                  */
#else
	BMRC_Client_ePCU,
#endif	
	BMRC_Client_eRAAGA_0_0,
	BMRC_Client_eRAAGA_0_1,
	BMRC_Client_eRAAGA_1_0,
	BMRC_Client_eRAAGA_1_1,
	BMRC_Client_eTDI_RD,
	BMRC_Client_eTDI_WR,

#if BMRC_ALLOW_GFX_TO_ACCESS_KERNEL
    /* allow magnum graphics HW cores (feeder and blitter) to read/write to/from kernel memory. */
#else
    /* prevent magnum graphics HW cores (feeder and blitter) from reading/writing to/from kernel memory.
    This is default.
    See BMRC_ALLOW_GFX_TO_ACCESS_KERNEL in bmrc.inc if you want to change this default. */
	BMRC_Client_eBVNB_GFD_0,
	BMRC_Client_eBVNB_GFD_1,
	BMRC_Client_eBVNB_GFD_2,
	BMRC_Client_eBVNB_GFD_3,
	BMRC_Client_eBVNB_GFD_4,
	BMRC_Client_eBVNB_GFD_5,
	BMRC_Client_eBVNB_GFD_CCC,
    BMRC_Client_eM2MC_0,
    BMRC_Client_eM2MC_1,
	BMRC_Client_eSID,
	BMRC_Client_ePIX_3D,
	BMRC_Client_eBVN_JFD_0,
	BMRC_Client_eV3D_0,
	BMRC_Client_eV3D_1,
#endif

    BMRC_Client_eMaxCount
};
