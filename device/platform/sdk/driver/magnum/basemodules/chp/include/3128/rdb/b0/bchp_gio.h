/***************************************************************************
 *     Copyright (c) 1999-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *
 * THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 * AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 * EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bchp_gio.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 4/14/11 12:12p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Thu Apr 14 10:09:27 2011
 *                 MD5 Checksum         b0ba84764310f22a148b48dae3bcde09
 *
 * Compiled with:  RDB Utility          combo_header.pl
 *                 RDB Parser           3.0
 *                 unknown              unknown
 *                 Perl Interpreter     5.008008
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/3128/rdb/b0/bchp_gio.h $
 * 
 * Hydra_Software_Devel/1   4/14/11 12:12p farshidf
 * SW3128-5: add 3128 RDb for b0
 *
 ***************************************************************************/

#ifndef BCHP_GIO_H__
#define BCHP_GIO_H__

/***************************************************************************
 *GIO - GPIO
 ***************************************************************************/
#define BCHP_GIO_RSVDO                           0x000c1100 /* RESERVED0 */
#define BCHP_GIO_RSVD1                           0x000c1104 /* RESERVED1 */
#define BCHP_GIO_RSVD2                           0x000c1108 /* RESERVED2 */
#define BCHP_GIO_EC_LO                           0x000c110c /* GENERAL PURPOSE I/O EDGE CONFIGURATION */
#define BCHP_GIO_EI_LO                           0x000c1110 /* GENERAL PURPOSE I/O EDGE INSENSITIVE */
#define BCHP_GIO_MASK_LO                         0x000c1114 /* GENERAL PURPOSE I/O INTERRUPT MASK */
#define BCHP_GIO_LEVEL_LO                        0x000c1118 /* GENERAL PURPOSE I/O INTERRUPT TYPE */
#define BCHP_GIO_STAT_LO                         0x000c111c /* GENERAL PURPOSE I/O INTERRUPT STATUS */

/***************************************************************************
 *RSVDO - RESERVED0
 ***************************************************************************/
/* GIO :: RSVDO :: reserved0 [31:00] */
#define BCHP_GIO_RSVDO_reserved0_MASK                              0xffffffff
#define BCHP_GIO_RSVDO_reserved0_SHIFT                             0

/***************************************************************************
 *RSVD1 - RESERVED1
 ***************************************************************************/
/* GIO :: RSVD1 :: reserved0 [31:00] */
#define BCHP_GIO_RSVD1_reserved0_MASK                              0xffffffff
#define BCHP_GIO_RSVD1_reserved0_SHIFT                             0

/***************************************************************************
 *RSVD2 - RESERVED2
 ***************************************************************************/
/* GIO :: RSVD2 :: reserved0 [31:00] */
#define BCHP_GIO_RSVD2_reserved0_MASK                              0xffffffff
#define BCHP_GIO_RSVD2_reserved0_SHIFT                             0

/***************************************************************************
 *EC_LO - GENERAL PURPOSE I/O EDGE CONFIGURATION
 ***************************************************************************/
/* GIO :: EC_LO :: reserved0 [31:26] */
#define BCHP_GIO_EC_LO_reserved0_MASK                              0xfc000000
#define BCHP_GIO_EC_LO_reserved0_SHIFT                             26

/* GIO :: EC_LO :: edge_config [25:00] */
#define BCHP_GIO_EC_LO_edge_config_MASK                            0x03ffffff
#define BCHP_GIO_EC_LO_edge_config_SHIFT                           0
#define BCHP_GIO_EC_LO_edge_config_DEFAULT                         0

/***************************************************************************
 *EI_LO - GENERAL PURPOSE I/O EDGE INSENSITIVE
 ***************************************************************************/
/* GIO :: EI_LO :: reserved0 [31:26] */
#define BCHP_GIO_EI_LO_reserved0_MASK                              0xfc000000
#define BCHP_GIO_EI_LO_reserved0_SHIFT                             26

/* GIO :: EI_LO :: edge_insensitive [25:00] */
#define BCHP_GIO_EI_LO_edge_insensitive_MASK                       0x03ffffff
#define BCHP_GIO_EI_LO_edge_insensitive_SHIFT                      0
#define BCHP_GIO_EI_LO_edge_insensitive_DEFAULT                    0

/***************************************************************************
 *MASK_LO - GENERAL PURPOSE I/O INTERRUPT MASK
 ***************************************************************************/
/* GIO :: MASK_LO :: reserved0 [31:26] */
#define BCHP_GIO_MASK_LO_reserved0_MASK                            0xfc000000
#define BCHP_GIO_MASK_LO_reserved0_SHIFT                           26

/* GIO :: MASK_LO :: irq_mask [25:00] */
#define BCHP_GIO_MASK_LO_irq_mask_MASK                             0x03ffffff
#define BCHP_GIO_MASK_LO_irq_mask_SHIFT                            0
#define BCHP_GIO_MASK_LO_irq_mask_DEFAULT                          0

/***************************************************************************
 *LEVEL_LO - GENERAL PURPOSE I/O INTERRUPT TYPE
 ***************************************************************************/
/* GIO :: LEVEL_LO :: reserved0 [31:26] */
#define BCHP_GIO_LEVEL_LO_reserved0_MASK                           0xfc000000
#define BCHP_GIO_LEVEL_LO_reserved0_SHIFT                          26

/* GIO :: LEVEL_LO :: level [25:00] */
#define BCHP_GIO_LEVEL_LO_level_MASK                               0x03ffffff
#define BCHP_GIO_LEVEL_LO_level_SHIFT                              0
#define BCHP_GIO_LEVEL_LO_level_DEFAULT                            0

/***************************************************************************
 *STAT_LO - GENERAL PURPOSE I/O INTERRUPT STATUS
 ***************************************************************************/
/* GIO :: STAT_LO :: reserved0 [31:26] */
#define BCHP_GIO_STAT_LO_reserved0_MASK                            0xfc000000
#define BCHP_GIO_STAT_LO_reserved0_SHIFT                           26

/* GIO :: STAT_LO :: irq_status [25:00] */
#define BCHP_GIO_STAT_LO_irq_status_MASK                           0x03ffffff
#define BCHP_GIO_STAT_LO_irq_status_SHIFT                          0
#define BCHP_GIO_STAT_LO_irq_status_DEFAULT                        0

#endif /* #ifndef BCHP_GIO_H__ */

/* End of File */