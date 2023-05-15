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
 * $brcm_Workfile: bchp_cap_2.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 4/4/11 1:04p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Fri Apr  1 16:38:29 2011
 *                 MD5 Checksum         d03d08c4839c3311c9d35c4cd5e10373
 *
 * Compiled with:  RDB Utility          combo_header.pl
 *                 RDB Parser           3.0
 *                 unknown              unknown
 *                 Perl Interpreter     5.008008
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/7344/rdb/b0/bchp_cap_2.h $
 * 
 * Hydra_Software_Devel/1   4/4/11 1:04p albertl
 * SW7344-40: Initial revision.
 *
 ***************************************************************************/

#ifndef BCHP_CAP_2_H__
#define BCHP_CAP_2_H__

/***************************************************************************
 *CAP_2 - Capture 2 Registers
 ***************************************************************************/
#define BCHP_CAP_2_REVISION                      0x00140400 /* Capture Engine Revision ID */
#define BCHP_CAP_2_PIC_SIZE                      0x00140404 /* Capture Vertical and Horizontal Size */
#define BCHP_CAP_2_PIC_OFFSET                    0x00140408 /* Capture 2D or 3D Left View Vertical and Horizontal Start Offset */
#define BCHP_CAP_2_PIC_OFFSET_R                  0x0014040c /* Capture 3D Right View Horizontal Start Offset */
#define BCHP_CAP_2_BVB_IN_SIZE                   0x00140410 /* Capture BVB Source Vertical and Horizontal Size */
#define BCHP_CAP_2_MSTART                        0x00140414 /* Capture Memory Buffer Starting Address */
#define BCHP_CAP_2_MSTART_R                      0x00140418 /* Capture Memory Buffer Right Window Starting Address */
#define BCHP_CAP_2_PITCH                         0x0014041c /* Capture Memory Buffer Pitch */
#define BCHP_CAP_2_CTRL                          0x00140420 /* Capture Control */
#define BCHP_CAP_2_MODE                          0x00140424 /* Capture Mode Control */
#define BCHP_CAP_2_BYTE_ORDER                    0x00140428 /* Capture Byte Order Control */
#define BCHP_CAP_2_RX_CTRL                       0x0014042c /* Capture BVB Receiver Control */
#define BCHP_CAP_2_TRIG_CTRL                     0x00140430 /* Capture RDMA Trigger Control */
#define BCHP_CAP_2_BVB_TRIG_0_CFG                0x00140434 /* Capture BVB Trigger 0 Configuration */
#define BCHP_CAP_2_BVB_TRIG_1_CFG                0x00140438 /* Capture BVB Trigger 1 Configuration */
#define BCHP_CAP_2_LINE_CMP_TRIG_0_CFG           0x0014043c /* Capture Line Compare Trigger 0 Configuration */
#define BCHP_CAP_2_LINE_CMP_TRIG_1_CFG           0x00140440 /* Capture Line Compare Trigger 1 Configuration */
#define BCHP_CAP_2_STATUS                        0x00140444 /* Capture Engine Status */
#define BCHP_CAP_2_BVB_STATUS                    0x00140448 /* Capture BVB Status */
#define BCHP_CAP_2_BVB_STATUS_CLEAR              0x0014044c /* Capture BVB Status Clear */
#define BCHP_CAP_2_FIFO_STATUS                   0x00140450 /* Capture FIFO Status */
#define BCHP_CAP_2_FIFO_STATUS_CLEAR             0x00140454 /* Capture FIFO Status Clear */
#define BCHP_CAP_2_SCRATCH                       0x0014047c /* Scratch Register */

#endif /* #ifndef BCHP_CAP_2_H__ */

/* End of File */