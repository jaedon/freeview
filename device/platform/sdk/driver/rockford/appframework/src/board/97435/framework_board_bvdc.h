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
* $brcm_Revision: Hydra_Software_Devel/3 $
* $brcm_Date: 9/12/12 5:35p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /rockford/appframework/src/board/97435/framework_board_bvdc.h $
* 
* Hydra_Software_Devel/3   9/12/12 5:35p vanessah
* SW7425-2501: add hdmi i2c index and xport streamer input band etc
* 
* Hydra_Software_Devel/2   2/29/12 12:45p tdo
* SW7425-2330: Add svc/mvc support back
*
* Hydra_Software_Devel/1   10/26/11 1:57p mward
* SW7435-9: Add 97435 appframework board.
*
* Hydra_Software_Devel/2   12/28/10 11:45a vanessah
* SW7425-24:
* Add support for 7425
*
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
#define BFramework_HDMI_OUTPUT_I2C_CHANNEL  (0)
/* Xport streamer input band 
   source:nexus_platform_frontend.c NEXUS_Platform_GetStreamerInputBand
*/
#define BFramework_XPT_STREAMER_INPUTBAND   (5)
#define BFramework_XPT_PARSERBAND_0         (0)
#endif /* FRAMEWORK_BOARD_BVDC_H */

/* End of file */

