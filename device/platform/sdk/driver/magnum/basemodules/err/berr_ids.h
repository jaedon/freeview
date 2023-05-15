/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: berr_ids.h $
 * $brcm_Revision: Hydra_Software_Devel/58 $
 * $brcm_Date: 6/13/12 4:35p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: X:/win_views/97231/magnum/basemodules/err/berr_ids.h $
 * 
 * Hydra_Software_Devel/58   6/13/12 4:35p vishk
 * SW7405-3392: Update 'berr_ids.h' for HAB error code
 * 
 * Hydra_Software_Devel/57   4/10/12 4:09p atanugul
 * SW3462-4: Add BERR_ODS_ID
 * 
 * Hydra_Software_Devel/56   2/27/12 11:12a agin
 * SWNOOS-518:  Create BICAP PI.
 * 
 * Hydra_Software_Devel/55   12/7/11 12:51p nilesh
 * SW7425-1877: Added VCE, MUXlib, and XUDlib Error IDs
 * 
 * Hydra_Software_Devel/54   11/22/11 5:59p vle
 * SW7425-1140: Add BERR_CEC_ID
 * 
 * Hydra_Software_Devel/SW7425-1140/1   11/16/11 12:18p vle
 * SW7425-1140: Add support for new CEC PI
 * 
 * Hydra_Software_Devel/53   5/11/11 2:41p ronchan
 * SWSATFE-107: added BERR_SCS_ID
 * 
 * Hydra_Software_Devel/52   5/11/11 2:40p ronchan
 * SWSATFE-108: added BERR_WFE_ID
 * 
 * Hydra_Software_Devel/51   4/7/11 4:19p jtna
 * SW7422-416: add BERR_MMD_ID
 * 
 * Hydra_Software_Devel/50   3/16/11 4:47p atanugul
 * SW3461-3: Add BERR_TC2_ID
 * 
 * Hydra_Software_Devel/49   1/17/11 11:03a jgarrett
 * SW7422-146: Adding BDSP ERR_ID
 * 
 * Hydra_Software_Devel/48   9/14/10 12:59p yuxiaz
 * SW35230-1355: Added IDs for 35230 magnum PIs.
 * 
 * Hydra_Software_Devel/47   7/27/10 5:03p darnstein
 * SW3548-3022: add support for the new BUDP commonutils module.
 * 
 * Hydra_Software_Devel/46   5/18/10 2:16p mbatchel
 * SW35230-76: Add id for ANV.
 * 
 * Hydra_Software_Devel/45   4/30/10 3:20p atanugul
 * SW35230-119: Add BERR_TFE_ID
 * 
 * Hydra_Software_Devel/44   12/22/09 12:54p nickh
 * SW7420-70: Add 1394
 * 
 * Hydra_Software_Devel/43   12/9/09 3:34p brianlee
 * SW7420-332: Fixed error IDs for TMON.
 * 
 * Hydra_Software_Devel/42   11/20/09 5:23p jgarrett
 * SW7408-17: Adding ELF and APE
 * 
 * Hydra_Software_Devel/41   8/16/09 6:21p nickh
 * PR47760: Add 7420 PWR PI support
 * 
 * Hydra_Software_Devel/40   7/15/09 2:16p abelous
 * PR14735: [DVD Subpicture] Subpicture is not displayed inspecific disc.
 * 
 * Hydra_Software_Devel/40   7/15/09 2:09p abelous
 * PR14735: [DVD Subpicture] Subpicture is not displayed inspecific disc.
 * 
 * Hydra_Software_Devel/bdvd_v2.0/1   7/15/09 12:31p tross
 * PR_14735[DVD]:[ see HiDef-DVD bug tracking system for more info ].
 * Bubble errors all the way up to player SPU decoder where stream-in
 * errors are now ignored. This allows invalid SPU's with insufficient
 * encoded data to be displayed.
 * 
 * Hydra_Software_Devel/fix_bdvd_v2.0_pr14735/1   7/15/09 10:40a tross
 * Add RLE ID.
 * 
 * Hydra_Software_Devel/39   8/18/08 5:17p garylin
 * PR43788: added more id for FTM, QPSK, and AST1
 * 
 * Hydra_Software_Devel/38   8/14/08 2:38p garylin
 * PR45679: add device id for 3556 video input (frontend)
 * 
 * Hydra_Software_Devel/37   6/19/08 1:23p garylin
 * PR43788: add more block ids: UPG for Sundry block and MDM for soft
 * modem. Both ids are used in power PI.
 * 
 * Hydra_Software_Devel/36   6/2/08 10:34a rgreen
 * PR38946: Add ERR ID for LVD PI
 * 
 * Hydra_Software_Devel/35   5/29/08 11:34a garylin
 * PR42849: add new id for UHF
 * 
 * Hydra_Software_Devel/34   4/7/08 11:25p rgreen
 * PR38947: Add error code id for HDR
 * 
 * Hydra_Software_Devel/33   1/10/08 12:04p rjlewis
 * PR38655: missing parens generated lots of warnings in Vxworks.
 * 
 * Hydra_Software_Devel/32   12/27/07 5:56p jessem
 * PR 38367: Used BERR_START_ID as base number for assigning values to
 * BERR_ID_xxx entries and BERR_START_LIB_ID for assigning values to
 * BERR_XXXlib_ID.
 * 
 * Hydra_Software_Devel/31   12/6/07 3:21p dliu
 * PR36603: Add DVB-T card support
 * 
 * Hydra_Software_Devel/30   5/31/07 4:13p shyam
 * PR 27542 : Added Id for the XCU common utilties
 * 
 * Hydra_Software_Devel/29   3/5/07 1:12p shyam
 * PR 27542 : Added Id for Transcoder or XCD PI
 * 
 * Hydra_Software_Devel/28   6/27/06 2:49p haisongw
 * PR22368: Add AOB, AUS and TNR3255ob support.
 * 
 * Hydra_Software_Devel/27   5/24/06 7:02p albertl
 * PR21392:  Added BINT error id.
 * 
 * Hydra_Software_Devel/26   5/12/06 6:10p haisongw
 * PR 20984: add BERR_RPC_ID
 * 
 * Hydra_Software_Devel/25   11/21/05 2:59p rjlewis
 * PR18214: added support for TMR module errors.
 * 
 * Hydra_Software_Devel/24   11/14/05 9:39a enavarro
 * PR 18135: added BERR_AST_ID
 * 
 * Hydra_Software_Devel/23   8/9/05 3:10p pntruong
 * PR13641:  Added BMRC error codes.
 * 
 * Hydra_Software_Devel/22   1/12/05 6:45p bmishra
 * PR 13082: Added BERR_RAP_ID
 * 
 * Hydra_Software_Devel/21   1/5/05 1:47p dlwin
 * PR 11480:Added BERR_BTSlib_ID
 * 
 * Hydra_Software_Devel/20   11/1/04 3:27p darnstein
 * PR 13115: Add module error code for new ARC (BARC) porting interface
 * module.
 * 
 * Hydra_Software_Devel/19   6/11/04 1:58p maivu
 * PR 10943: Added error code id for XVD
 * 
 * Hydra_Software_Devel/18   3/9/04 11:13a nissen
 * PR 10024: Added error code ids for GRC and P3D.
 * 
 * Hydra_Software_Devel/17   12/1/03 7:08p dlwin
 * Added error code for LNA
 * 
 * Hydra_Software_Devel/16   10/23/03 3:42p syang
 * added error code id for dma and gio
 * 
 * Hydra_Software_Devel/15   10/22/03 3:19p vsilyaev
 * Added mpeg encoder.
 * 
 * Hydra_Software_Devel/14   10/8/03 11:56a darnstein
 * Add code for BVBIlib (syslib) module.
 * 
 * Hydra_Software_Devel/13   9/11/03 3:17p enavarro
 * added IDs for SDS and VSB
 * 
 * Hydra_Software_Devel/12   9/11/03 11:24a brianlee
 * Added IDs for UPG modules.
 * 
 * Hydra_Software_Devel/11   9/3/03 4:32p jasonh
 * Re-ordered IDs. Added HDM ID.
 * 
 * Hydra_Software_Devel/9   9/2/03 1:30p gmullen
 * Added XPT module ID.
 * 
 * Hydra_Software_Devel/8   8/29/03 12:20p sri
 * Added BERR_MVD_ID.
 * 
 * Hydra_Software_Devel/7   8/13/03 2:14p aram
 * added aud module
 * 
 * Hydra_Software_Devel/6   7/11/03 1:35p jasonh
 * Added BERR_RDC_ID.
 * 
 * Hydra_Software_Devel/5   7/11/03 1:22p jasonh
 * Re-ordered IDs. Removed VBIlib. Added BERR_VDC_ID.
 * 
 * Hydra_Software_Devel/4   6/18/03 3:50p dlwin
 * Added BERR_QDS_ID, BERR_QOB_ID, BERR_QUS_ID, BERR_TNR_ID, BERR_RFM_ID,
 * BERR_IFD_ID, and BERR_TNRlib_ID.
 * 
 * Hydra_Software_Devel/3   6/16/03 6:40p btan
 * Changed BERR_H__  to BERR_ID_H__.
 * 
 * Hydra_Software_Devel/2   6/10/03 4:59p btan
 * Added ids for smart card (SCD) and NDS ICAM (ICM)
 *
 ***************************************************************************/
#ifndef BERR_ID_H__
#define BERR_ID_H__

/* The standard ID. This ID is present when the standard error codes are
   being used without a module specific ID */
#define BERR_STD_ID     0x00

/* The application ID. This ID is reserved for use by top level applications
   so they can extend our error codes if they wish. This ID cannot be used
   in any module or library */
#define BERR_APP_ID     0x01

#define BERR_START_ID   0x02

/* porting interfaces */
#define BERR_AUD_ID     (BERR_START_ID + 0)
#define BERR_HDM_ID     (BERR_START_ID + 1)
#define BERR_ICM_ID     (BERR_START_ID + 2)
#define BERR_IFD_ID     (BERR_START_ID + 3)
#define BERR_MVD_ID     (BERR_START_ID + 4)
#define BERR_QDS_ID     (BERR_START_ID + 5)
#define BERR_QOB_ID     (BERR_START_ID + 6)
#define BERR_QUS_ID     (BERR_START_ID + 7)
#define BERR_RDC_ID     (BERR_START_ID + 8)
#define BERR_RFM_ID     (BERR_START_ID + 9)
#define BERR_SCD_ID     (BERR_START_ID + 10)
#define BERR_TNR_ID     (BERR_START_ID + 11)
#define BERR_VBI_ID     (BERR_START_ID + 12)
#define BERR_VDC_ID     (BERR_START_ID + 13)
#define BERR_XPT_ID     (BERR_START_ID + 14)
#define BERR_I2C_ID     (BERR_START_ID + 15)
#define BERR_SPI_ID     (BERR_START_ID + 16)
#define BERR_ICP_ID     (BERR_START_ID + 17)
#define BERR_IRB_ID     (BERR_START_ID + 18)
#define BERR_KIR_ID     (BERR_START_ID + 19)
#define BERR_KPD_ID     (BERR_START_ID + 20)
#define BERR_LED_ID     (BERR_START_ID + 21)
#define BERR_PWM_ID     (BERR_START_ID + 22)
#define BERR_URT_ID     (BERR_START_ID + 23)
#define BERR_SDS_ID     (BERR_START_ID + 24)
#define BERR_VSB_ID     (BERR_START_ID + 25)
#define BERR_ENC_ID     (BERR_START_ID + 26)
#define BERR_DMA_ID     (BERR_START_ID + 27)
#define BERR_GIO_ID     (BERR_START_ID + 28)
#define BERR_LNA_ID     (BERR_START_ID + 29)
#define BERR_GRC_ID     (BERR_START_ID + 30)
#define BERR_P3D_ID     (BERR_START_ID + 31)
#define BERR_XVD_ID     (BERR_START_ID + 32)
#define BERR_ARC_ID     (BERR_START_ID + 33)
#define BERR_RAP_ID     (BERR_START_ID + 34)
#define BERR_MRC_ID     (BERR_START_ID + 35)
#define BERR_AST_ID     (BERR_START_ID + 36)
#define BERR_TMR_ID     (BERR_START_ID + 37)
#define BERR_RPC_ID     (BERR_START_ID + 38)
#define BERR_MEM_ID     (BERR_START_ID + 39)
#define BERR_INT_ID     (BERR_START_ID + 40)
#define BERR_ADS_ID     (BERR_START_ID + 41)
#define BERR_AOB_ID     (BERR_START_ID + 42)
#define BERR_AUS_ID     (BERR_START_ID + 43)
#define BERR_XCD_ID     (BERR_START_ID + 44)
#define BERR_THD_ID     (BERR_START_ID + 45)
#define BERR_XCU_ID     (BERR_START_ID + 46)
#define BERR_NET_ID     (BERR_START_ID + 47)
#define BERR_USB_ID     (BERR_START_ID + 48)
#define BERR_ATA_ID     (BERR_START_ID + 49)
#define BERR_HDR_ID     (BERR_START_ID + 50)
#define BERR_UHF_ID     (BERR_START_ID + 51)
#define BERR_LVD_ID     (BERR_START_ID + 52)
#define BERR_UPG_ID     (BERR_START_ID + 53)
#define BERR_MDM_ID     (BERR_START_ID + 54)
#define BERR_VIF_ID     (BERR_START_ID + 55)
#define BERR_AST1_ID    (BERR_START_ID + 56)
#define BERR_QPSK_ID    (BERR_START_ID + 57)
#define BERR_FTM_ID     (BERR_START_ID + 58)
#define BERR_RLE_ID     (BERR_START_ID + 59)
#define BERR_CTK_ID     (BERR_START_ID + 60)
#define BERR_TMN_ID     (BERR_START_ID + 61)
#define BERR_ELF_ID     (BERR_START_ID + 62)
#define BERR_APE_ID     (BERR_START_ID + 63)
#define BERR_TNF_ID     (BERR_START_ID + 64)
#define BERR_TFE_ID     (BERR_START_ID + 65)
#define BERR_ANV_ID     (BERR_START_ID + 66)
#define BERR_UDP_ID     (BERR_START_ID + 67)
#define BERR_ANA_ID     (BERR_START_ID + 68)
#define BERR_SID_ID     (BERR_START_ID + 69)
#define BERR_V3D_ID     (BERR_START_ID + 70)
#define BERR_DSP_ID     (BERR_START_ID + 71)
#define BERR_TC2_ID     (BERR_START_ID + 72)
#define BERR_MMD_ID     (BERR_START_ID + 73)
#define BERR_WFE_ID     (BERR_START_ID + 74)
#define BERR_SCS_ID     (BERR_START_ID + 75)
#define BERR_CEC_ID		(BERR_START_ID + 76)
#define BERR_VCE_ID     (BERR_START_ID + 77)
#define BERR_BCP_ID     (BERR_START_ID + 78)
#define BERR_ODS_ID     (BERR_START_ID + 79)
#define BERR_HAB_ID     (BERR_START_ID + 80)
#define BERR_SAT_ID     (BERR_START_ID + 81)
#define BERR_DSQ_ID     (BERR_START_ID + 82)
#define BERR_FSK_ID     (BERR_START_ID + 83)
#define BERR_BOX_ID     (BERR_START_ID + 84)
#define BERR_MMA_ID     (BERR_START_ID + 85)
#define BERR_DBG_ID     (BERR_START_ID + 86)


/* syslibs */
#define BERR_START_LIB_ID     0x100

#define BERR_TNRlib_ID        (BERR_START_LIB_ID + 0)
#define BERR_VBIlib_ID        (BERR_START_LIB_ID + 1)
#define BERR_BTSlib_ID        (BERR_START_LIB_ID + 2)
#define BERR_MUXlib_ID        (BERR_START_LIB_ID + 3)
#define BERR_XUDlib_ID        (BERR_START_LIB_ID + 4)

#endif /* #ifndef BERR_ID_H__ */

/* end of file */
