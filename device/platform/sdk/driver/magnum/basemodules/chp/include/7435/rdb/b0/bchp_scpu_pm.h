/***************************************************************************
 *     Copyright (c) 1999-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *
 * THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 * AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 * EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bchp_scpu_pm.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 5/9/12 9:01a $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Wed May  9 08:36:42 2012
 *                 MD5 Checksum         d41d8cd98f00b204e9800998ecf8427e
 *
 * Compiled with:  RDB Utility          combo_header.pl
 *                 RDB Parser           3.0
 *                 unknown              unknown
 *                 Perl Interpreter     5.008008
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/7435/rdb/b0/bchp_scpu_pm.h $
 * 
 * Hydra_Software_Devel/1   5/9/12 9:01a tdo
 * SW7435-40: Resync header files
 *
 ***************************************************************************/

#ifndef BCHP_SCPU_PM_H__
#define BCHP_SCPU_PM_H__

/***************************************************************************
 *SCPU_PM - Peripheral Module Configuration
 ***************************************************************************/
#define BCHP_SCPU_PM_CLK_CTRL                    0x00308984 /* UPG Clock control register */
#define BCHP_SCPU_PM_RST_CTRL                    0x00308988 /* UPG reset control register */

/***************************************************************************
 *CLK_CTRL - UPG Clock control register
 ***************************************************************************/
/* SCPU_PM :: CLK_CTRL :: reserved0 [31:18] */
#define BCHP_SCPU_PM_CLK_CTRL_reserved0_MASK                       0xfffc0000
#define BCHP_SCPU_PM_CLK_CTRL_reserved0_SHIFT                      18

/* SCPU_PM :: CLK_CTRL :: reserved_for_eco1 [17:17] */
#define BCHP_SCPU_PM_CLK_CTRL_reserved_for_eco1_MASK               0x00020000
#define BCHP_SCPU_PM_CLK_CTRL_reserved_for_eco1_SHIFT              17
#define BCHP_SCPU_PM_CLK_CTRL_reserved_for_eco1_DEFAULT            0x00000000

/* SCPU_PM :: CLK_CTRL :: timer [16:16] */
#define BCHP_SCPU_PM_CLK_CTRL_timer_MASK                           0x00010000
#define BCHP_SCPU_PM_CLK_CTRL_timer_SHIFT                          16
#define BCHP_SCPU_PM_CLK_CTRL_timer_DEFAULT                        0x00000001

/* SCPU_PM :: CLK_CTRL :: reserved_for_eco2 [15:00] */
#define BCHP_SCPU_PM_CLK_CTRL_reserved_for_eco2_MASK               0x0000ffff
#define BCHP_SCPU_PM_CLK_CTRL_reserved_for_eco2_SHIFT              0
#define BCHP_SCPU_PM_CLK_CTRL_reserved_for_eco2_DEFAULT            0x00000000

/***************************************************************************
 *RST_CTRL - UPG reset control register
 ***************************************************************************/
/* SCPU_PM :: RST_CTRL :: reserved0 [31:18] */
#define BCHP_SCPU_PM_RST_CTRL_reserved0_MASK                       0xfffc0000
#define BCHP_SCPU_PM_RST_CTRL_reserved0_SHIFT                      18

/* SCPU_PM :: RST_CTRL :: reserved_for_eco1 [17:17] */
#define BCHP_SCPU_PM_RST_CTRL_reserved_for_eco1_MASK               0x00020000
#define BCHP_SCPU_PM_RST_CTRL_reserved_for_eco1_SHIFT              17
#define BCHP_SCPU_PM_RST_CTRL_reserved_for_eco1_DEFAULT            0x00000000

/* SCPU_PM :: RST_CTRL :: timer [16:16] */
#define BCHP_SCPU_PM_RST_CTRL_timer_MASK                           0x00010000
#define BCHP_SCPU_PM_RST_CTRL_timer_SHIFT                          16
#define BCHP_SCPU_PM_RST_CTRL_timer_DEFAULT                        0x00000000

/* SCPU_PM :: RST_CTRL :: reserved_for_eco2 [15:00] */
#define BCHP_SCPU_PM_RST_CTRL_reserved_for_eco2_MASK               0x0000ffff
#define BCHP_SCPU_PM_RST_CTRL_reserved_for_eco2_SHIFT              0
#define BCHP_SCPU_PM_RST_CTRL_reserved_for_eco2_DEFAULT            0x00000000

#endif /* #ifndef BCHP_SCPU_PM_H__ */

/* End of File */