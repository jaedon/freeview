/***************************************************************************
*     Copyright (c) 2003-2012, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: framework_board_bvdc.h $
* $brcm_Revision: Hydra_Software_Devel/4 $
* $brcm_Date: 9/12/12 5:38p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /rockford/appframework/src/board/97422/framework_board_bvdc.h $
* 
* Hydra_Software_Devel/4   9/12/12 5:38p vanessah
* SW7425-2501: add hdmi i2c index and xport streamer input band etc
* 
* Hydra_Software_Devel/3   2/29/12 12:44p tdo
* SW7425-2330: Add svc/mvc support back
*
* Hydra_Software_Devel/2   11/1/10 5:24p tdo
* SW7422-13: add 7422 support to vdc_test
*
* Hydra_Software_Devel/1   6/22/10 4:01p vanessah
* SW7422-12:  add source control framework_board.c
*
* Hydra_Software_Devel/3   10/14/09 4:05p tdo
* SW7420-376: Define number of XVD and MVD core in appframework
*
* Hydra_Software_Devel/2   9/29/09 4:48p tdo
* SW7420-360: Fix Dac assignment for Svideo
*
* Hydra_Software_Devel/1   8/31/09 3:01p darnstein
* SW7342-26: Declarations that are specific to board, and to BVDC
* software.
***************************************************************************/

#ifndef FRAMEWORK_BOARD_BVDC_H
#define FRAMEWORK_BOARD_BVDC_H

#define BFramework_Dac_Composite_0   BVDC_Dac_3
#define BFramework_Dac_Composite_1   0x0
#define BFramework_Dac_Svideo_Luma   0x0
#define BFramework_Dac_Svideo_Chroma 0x0
#define BFramework_Dac_Component_y   BVDC_Dac_0
#define BFramework_Dac_Component_pr  BVDC_Dac_2
#define BFramework_Dac_Component_pb  BVDC_Dac_1

#define BFramework_NUM_XVD_CORES     2
#define BFramework_NUM_SVD           1
#define BFramework_NUM_MVD_CORES     0

/* Default I2C Channel used for HDMI output
   source: nexus_platform_config.c NEXUS_HDMI_OUTPUT_I2C_CHANNEL 
*/
#if (BCHP_VER >= BCHP_VER_B0)
#define BFramework_HDMI_OUTPUT_I2C_CHANNEL  (0)
#else
#define BFramework_HDMI_OUTPUT_I2C_CHANNEL  (3)
#endif
/* Xport streamer input band 
   source:nexus_platform_frontend.c NEXUS_Platform_GetStreamerInputBand
*/
#define BFramework_XPT_STREAMER_INPUTBAND   (5)
#define BFramework_XPT_PARSERBAND_0         (0)

#endif /* FRAMEWORK_BOARD_BVDC_H */

/* End of file */

