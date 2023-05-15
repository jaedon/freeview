/***************************************************************************
 *     Copyright (c) 1999-2009, Broadcom Corporation
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
 * $brcm_Date: 9/14/09 4:28p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Fri Sep 11 19:40:44 2009
 *                 MD5 Checksum         957f01e03a68c1766fd2e8ad6484f5f9
 *
 * Compiled with:  RDB Utility          combo_header.pl
 *                 RDB Parser           3.0
 *                 unknown              unknown
 *                 Perl Interpreter     5.008008
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/7468/rdb/a0/bchp_gio.h $
 * 
 * Hydra_Software_Devel/1   9/14/09 4:28p albertl
 * SW7468-3: Initial revision.
 *
 ***************************************************************************/

#ifndef BCHP_GIO_H__
#define BCHP_GIO_H__

/***************************************************************************
 *GIO - GPIO
 ***************************************************************************/
#define BCHP_GIO_ODEN_LO                         0x00306700 /* GENERAL PURPOSE I/O OPEN DRAIN ENABLE [31:0] */
#define BCHP_GIO_DATA_LO                         0x00306704 /* GENERAL PURPOSE I/O DATA [31:0] */
#define BCHP_GIO_IODIR_LO                        0x00306708 /* GENERAL PURPOSE I/O DIRECTION [31:0] */
#define BCHP_GIO_EC_LO                           0x0030670c /* GENERAL PURPOSE I/O EDGE CONFIGURATION [31:0] */
#define BCHP_GIO_EI_LO                           0x00306710 /* GENERAL PURPOSE I/O EDGE INSENSITIVE [31:0] */
#define BCHP_GIO_MASK_LO                         0x00306714 /* GENERAL PURPOSE I/O INTERRUPT MASK [31:0] */
#define BCHP_GIO_LEVEL_LO                        0x00306718 /* GENERAL PURPOSE I/O INTERRUPT TYPE [31:0] */
#define BCHP_GIO_STAT_LO                         0x0030671c /* GENERAL PURPOSE I/O INTERRUPT STATUS [31:0] */
#define BCHP_GIO_ODEN_HI                         0x00306720 /* GENERAL PURPOSE I/O OPEN DRAIN ENABLE [52:32] */
#define BCHP_GIO_DATA_HI                         0x00306724 /* GENERAL PURPOSE I/O DATA [52:32] */
#define BCHP_GIO_IODIR_HI                        0x00306728 /* GENERAL PURPOSE I/O DIRECTION [52:32] */
#define BCHP_GIO_EC_HI                           0x0030672c /* GENERAL PURPOSE I/O EDGE CONFIGURATION [52:32] */
#define BCHP_GIO_EI_HI                           0x00306730 /* GENERAL PURPOSE I/O EDGE INSENSITIVE [52:32] */
#define BCHP_GIO_MASK_HI                         0x00306734 /* GENERAL PURPOSE I/O INTERRUPT MASK [52:32] */
#define BCHP_GIO_LEVEL_HI                        0x00306738 /* GENERAL PURPOSE I/O INTERRUPT TYPE [52:32] */
#define BCHP_GIO_STAT_HI                         0x0030673c /* GENERAL PURPOSE I/O INTERRUPT STATUS [52:32] */
#define BCHP_GIO_ODEN_EXT                        0x00306740 /* GENERAL PURPOSE I/O OPEN DRAIN ENABLE [67:64] (SGPIO[3:0]) */
#define BCHP_GIO_DATA_EXT                        0x00306744 /* GENERAL PURPOSE I/O DATA [67:64] (SGPIO[3:0]) */
#define BCHP_GIO_IODIR_EXT                       0x00306748 /* GENERAL PURPOSE I/O DIRECTION [67:64] (SGPIO[3:0]) */
#define BCHP_GIO_EC_EXT                          0x0030674c /* GENERAL PURPOSE I/O EDGE CONFIGURATION [67:64] (SGPIO[3:0]) */
#define BCHP_GIO_EI_EXT                          0x00306750 /* GENERAL PURPOSE I/O EDGE INSENSITIVE [67:64] (SGPIO[3:0]) */
#define BCHP_GIO_MASK_EXT                        0x00306754 /* GENERAL PURPOSE I/O INTERRUPT MASK [67:64] (SGPIO[3:0]) */
#define BCHP_GIO_LEVEL_EXT                       0x00306758 /* GENERAL PURPOSE I/O INTERRUPT TYPE [67:64] (SGPIO[3:0]) */
#define BCHP_GIO_STAT_EXT                        0x0030675c /* GENERAL PURPOSE I/O INTERRUPT STATUS [67:64] (SGPIO[3:0]) */

/***************************************************************************
 *ODEN_LO - GENERAL PURPOSE I/O OPEN DRAIN ENABLE [31:0]
 ***************************************************************************/
/* GIO :: ODEN_LO :: oden [31:00] */
#define BCHP_GIO_ODEN_LO_oden_MASK                                 0xffffffff
#define BCHP_GIO_ODEN_LO_oden_SHIFT                                0

/***************************************************************************
 *DATA_LO - GENERAL PURPOSE I/O DATA [31:0]
 ***************************************************************************/
/* GIO :: DATA_LO :: data [31:00] */
#define BCHP_GIO_DATA_LO_data_MASK                                 0xffffffff
#define BCHP_GIO_DATA_LO_data_SHIFT                                0

/***************************************************************************
 *IODIR_LO - GENERAL PURPOSE I/O DIRECTION [31:0]
 ***************************************************************************/
/* GIO :: IODIR_LO :: iodir [31:00] */
#define BCHP_GIO_IODIR_LO_iodir_MASK                               0xffffffff
#define BCHP_GIO_IODIR_LO_iodir_SHIFT                              0

/***************************************************************************
 *EC_LO - GENERAL PURPOSE I/O EDGE CONFIGURATION [31:0]
 ***************************************************************************/
/* GIO :: EC_LO :: edge_config [31:00] */
#define BCHP_GIO_EC_LO_edge_config_MASK                            0xffffffff
#define BCHP_GIO_EC_LO_edge_config_SHIFT                           0

/***************************************************************************
 *EI_LO - GENERAL PURPOSE I/O EDGE INSENSITIVE [31:0]
 ***************************************************************************/
/* GIO :: EI_LO :: edge_insensitive [31:00] */
#define BCHP_GIO_EI_LO_edge_insensitive_MASK                       0xffffffff
#define BCHP_GIO_EI_LO_edge_insensitive_SHIFT                      0

/***************************************************************************
 *MASK_LO - GENERAL PURPOSE I/O INTERRUPT MASK [31:0]
 ***************************************************************************/
/* GIO :: MASK_LO :: irq_mask [31:00] */
#define BCHP_GIO_MASK_LO_irq_mask_MASK                             0xffffffff
#define BCHP_GIO_MASK_LO_irq_mask_SHIFT                            0

/***************************************************************************
 *LEVEL_LO - GENERAL PURPOSE I/O INTERRUPT TYPE [31:0]
 ***************************************************************************/
/* GIO :: LEVEL_LO :: level [31:00] */
#define BCHP_GIO_LEVEL_LO_level_MASK                               0xffffffff
#define BCHP_GIO_LEVEL_LO_level_SHIFT                              0

/***************************************************************************
 *STAT_LO - GENERAL PURPOSE I/O INTERRUPT STATUS [31:0]
 ***************************************************************************/
/* GIO :: STAT_LO :: irq_status [31:00] */
#define BCHP_GIO_STAT_LO_irq_status_MASK                           0xffffffff
#define BCHP_GIO_STAT_LO_irq_status_SHIFT                          0

/***************************************************************************
 *ODEN_HI - GENERAL PURPOSE I/O OPEN DRAIN ENABLE [52:32]
 ***************************************************************************/
/* GIO :: ODEN_HI :: oden [31:00] */
#define BCHP_GIO_ODEN_HI_oden_MASK                                 0xffffffff
#define BCHP_GIO_ODEN_HI_oden_SHIFT                                0

/***************************************************************************
 *DATA_HI - GENERAL PURPOSE I/O DATA [52:32]
 ***************************************************************************/
/* GIO :: DATA_HI :: data [31:00] */
#define BCHP_GIO_DATA_HI_data_MASK                                 0xffffffff
#define BCHP_GIO_DATA_HI_data_SHIFT                                0

/***************************************************************************
 *IODIR_HI - GENERAL PURPOSE I/O DIRECTION [52:32]
 ***************************************************************************/
/* GIO :: IODIR_HI :: iodir [31:00] */
#define BCHP_GIO_IODIR_HI_iodir_MASK                               0xffffffff
#define BCHP_GIO_IODIR_HI_iodir_SHIFT                              0

/***************************************************************************
 *EC_HI - GENERAL PURPOSE I/O EDGE CONFIGURATION [52:32]
 ***************************************************************************/
/* GIO :: EC_HI :: edge_config [31:00] */
#define BCHP_GIO_EC_HI_edge_config_MASK                            0xffffffff
#define BCHP_GIO_EC_HI_edge_config_SHIFT                           0

/***************************************************************************
 *EI_HI - GENERAL PURPOSE I/O EDGE INSENSITIVE [52:32]
 ***************************************************************************/
/* GIO :: EI_HI :: edge_insensitive [31:00] */
#define BCHP_GIO_EI_HI_edge_insensitive_MASK                       0xffffffff
#define BCHP_GIO_EI_HI_edge_insensitive_SHIFT                      0

/***************************************************************************
 *MASK_HI - GENERAL PURPOSE I/O INTERRUPT MASK [52:32]
 ***************************************************************************/
/* GIO :: MASK_HI :: irq_mask [31:00] */
#define BCHP_GIO_MASK_HI_irq_mask_MASK                             0xffffffff
#define BCHP_GIO_MASK_HI_irq_mask_SHIFT                            0

/***************************************************************************
 *LEVEL_HI - GENERAL PURPOSE I/O INTERRUPT TYPE [52:32]
 ***************************************************************************/
/* GIO :: LEVEL_HI :: level [31:00] */
#define BCHP_GIO_LEVEL_HI_level_MASK                               0xffffffff
#define BCHP_GIO_LEVEL_HI_level_SHIFT                              0

/***************************************************************************
 *STAT_HI - GENERAL PURPOSE I/O INTERRUPT STATUS [52:32]
 ***************************************************************************/
/* GIO :: STAT_HI :: irq_status [31:00] */
#define BCHP_GIO_STAT_HI_irq_status_MASK                           0xffffffff
#define BCHP_GIO_STAT_HI_irq_status_SHIFT                          0

/***************************************************************************
 *ODEN_EXT - GENERAL PURPOSE I/O OPEN DRAIN ENABLE [67:64] (SGPIO[3:0])
 ***************************************************************************/
/* GIO :: ODEN_EXT :: reserved0 [31:04] */
#define BCHP_GIO_ODEN_EXT_reserved0_MASK                           0xfffffff0
#define BCHP_GIO_ODEN_EXT_reserved0_SHIFT                          4

/* GIO :: ODEN_EXT :: oden [03:00] */
#define BCHP_GIO_ODEN_EXT_oden_MASK                                0x0000000f
#define BCHP_GIO_ODEN_EXT_oden_SHIFT                               0

/***************************************************************************
 *DATA_EXT - GENERAL PURPOSE I/O DATA [67:64] (SGPIO[3:0])
 ***************************************************************************/
/* GIO :: DATA_EXT :: reserved0 [31:04] */
#define BCHP_GIO_DATA_EXT_reserved0_MASK                           0xfffffff0
#define BCHP_GIO_DATA_EXT_reserved0_SHIFT                          4

/* GIO :: DATA_EXT :: data [03:00] */
#define BCHP_GIO_DATA_EXT_data_MASK                                0x0000000f
#define BCHP_GIO_DATA_EXT_data_SHIFT                               0

/***************************************************************************
 *IODIR_EXT - GENERAL PURPOSE I/O DIRECTION [67:64] (SGPIO[3:0])
 ***************************************************************************/
/* GIO :: IODIR_EXT :: reserved0 [31:04] */
#define BCHP_GIO_IODIR_EXT_reserved0_MASK                          0xfffffff0
#define BCHP_GIO_IODIR_EXT_reserved0_SHIFT                         4

/* GIO :: IODIR_EXT :: iodir [03:00] */
#define BCHP_GIO_IODIR_EXT_iodir_MASK                              0x0000000f
#define BCHP_GIO_IODIR_EXT_iodir_SHIFT                             0

/***************************************************************************
 *EC_EXT - GENERAL PURPOSE I/O EDGE CONFIGURATION [67:64] (SGPIO[3:0])
 ***************************************************************************/
/* GIO :: EC_EXT :: reserved0 [31:04] */
#define BCHP_GIO_EC_EXT_reserved0_MASK                             0xfffffff0
#define BCHP_GIO_EC_EXT_reserved0_SHIFT                            4

/* GIO :: EC_EXT :: edge_config [03:00] */
#define BCHP_GIO_EC_EXT_edge_config_MASK                           0x0000000f
#define BCHP_GIO_EC_EXT_edge_config_SHIFT                          0

/***************************************************************************
 *EI_EXT - GENERAL PURPOSE I/O EDGE INSENSITIVE [67:64] (SGPIO[3:0])
 ***************************************************************************/
/* GIO :: EI_EXT :: reserved0 [31:04] */
#define BCHP_GIO_EI_EXT_reserved0_MASK                             0xfffffff0
#define BCHP_GIO_EI_EXT_reserved0_SHIFT                            4

/* GIO :: EI_EXT :: edge_insensitive [03:00] */
#define BCHP_GIO_EI_EXT_edge_insensitive_MASK                      0x0000000f
#define BCHP_GIO_EI_EXT_edge_insensitive_SHIFT                     0

/***************************************************************************
 *MASK_EXT - GENERAL PURPOSE I/O INTERRUPT MASK [67:64] (SGPIO[3:0])
 ***************************************************************************/
/* GIO :: MASK_EXT :: reserved0 [31:04] */
#define BCHP_GIO_MASK_EXT_reserved0_MASK                           0xfffffff0
#define BCHP_GIO_MASK_EXT_reserved0_SHIFT                          4

/* GIO :: MASK_EXT :: irq_mask [03:00] */
#define BCHP_GIO_MASK_EXT_irq_mask_MASK                            0x0000000f
#define BCHP_GIO_MASK_EXT_irq_mask_SHIFT                           0

/***************************************************************************
 *LEVEL_EXT - GENERAL PURPOSE I/O INTERRUPT TYPE [67:64] (SGPIO[3:0])
 ***************************************************************************/
/* GIO :: LEVEL_EXT :: reserved0 [31:04] */
#define BCHP_GIO_LEVEL_EXT_reserved0_MASK                          0xfffffff0
#define BCHP_GIO_LEVEL_EXT_reserved0_SHIFT                         4

/* GIO :: LEVEL_EXT :: level [03:00] */
#define BCHP_GIO_LEVEL_EXT_level_MASK                              0x0000000f
#define BCHP_GIO_LEVEL_EXT_level_SHIFT                             0

/***************************************************************************
 *STAT_EXT - GENERAL PURPOSE I/O INTERRUPT STATUS [67:64] (SGPIO[3:0])
 ***************************************************************************/
/* GIO :: STAT_EXT :: reserved0 [31:04] */
#define BCHP_GIO_STAT_EXT_reserved0_MASK                           0xfffffff0
#define BCHP_GIO_STAT_EXT_reserved0_SHIFT                          4

/* GIO :: STAT_EXT :: irq_status [03:00] */
#define BCHP_GIO_STAT_EXT_irq_status_MASK                          0x0000000f
#define BCHP_GIO_STAT_EXT_irq_status_SHIFT                         0

#endif /* #ifndef BCHP_GIO_H__ */

/* End of File */