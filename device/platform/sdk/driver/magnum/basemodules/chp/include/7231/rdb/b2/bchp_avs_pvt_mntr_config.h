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
 * $brcm_Workfile: bchp_avs_pvt_mntr_config.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 1/24/12 4:39p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Mon Jan 23 14:23:47 2012
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
 * $brcm_Log: /magnum/basemodules/chp/7231/rdb/b2/bchp_avs_pvt_mntr_config.h $
 * 
 * Hydra_Software_Devel/1   1/24/12 4:39p katrep
 * SW7231-1:rdb header files for 7231B2
 *
 ***************************************************************************/

#ifndef BCHP_AVS_PVT_MNTR_CONFIG_H__
#define BCHP_AVS_PVT_MNTR_CONFIG_H__

/***************************************************************************
 *AVS_PVT_MNTR_CONFIG - AVS PVT Monitor Configuration Core
 ***************************************************************************/
#define BCHP_AVS_PVT_MNTR_CONFIG_PVT_MNTR_CTRL   0x00423100 /* Control bits for PVT monitor */
#define BCHP_AVS_PVT_MNTR_CONFIG_PVT_MNTR_IDOFFMON_PWRDN 0x00423104 /* Power down the IDOFFMON */
#define BCHP_AVS_PVT_MNTR_CONFIG_PVT_MNTR_TP_MODE_ENABLE 0x00423108 /* Enabling TP mode to use data from tp_in/out pins */
#define BCHP_AVS_PVT_MNTR_CONFIG_DAC_CODE_PROGRAMMING_ENABLE 0x00423110 /* SW must set this bit to 1 to modify DAC_CODE, MIN_DAC_CODE and MAX_DAC_CODE */
#define BCHP_AVS_PVT_MNTR_CONFIG_DAC_CODE        0x00423114 /* Represents the input code of the DAC */
#define BCHP_AVS_PVT_MNTR_CONFIG_MIN_DAC_CODE    0x00423118 /* SW may set this to a lower value to prevent the DAC from driving too high of a voltage */
#define BCHP_AVS_PVT_MNTR_CONFIG_MAX_DAC_CODE    0x0042311c /* SW may set this to a higher value to prevent the DAC from driving too low of a voltage */

/***************************************************************************
 *PVT_MNTR_CTRL - Control bits for PVT monitor
 ***************************************************************************/
/* AVS_PVT_MNTR_CONFIG :: PVT_MNTR_CTRL :: pvtmon_ctrl [31:00] */
#define BCHP_AVS_PVT_MNTR_CONFIG_PVT_MNTR_CTRL_pvtmon_ctrl_MASK    0xffffffff
#define BCHP_AVS_PVT_MNTR_CONFIG_PVT_MNTR_CTRL_pvtmon_ctrl_SHIFT   0
#define BCHP_AVS_PVT_MNTR_CONFIG_PVT_MNTR_CTRL_pvtmon_ctrl_DEFAULT 0x001d0003

/***************************************************************************
 *PVT_MNTR_IDOFFMON_PWRDN - Power down the IDOFFMON
 ***************************************************************************/
/* AVS_PVT_MNTR_CONFIG :: PVT_MNTR_IDOFFMON_PWRDN :: reserved0 [31:01] */
#define BCHP_AVS_PVT_MNTR_CONFIG_PVT_MNTR_IDOFFMON_PWRDN_reserved0_MASK 0xfffffffe
#define BCHP_AVS_PVT_MNTR_CONFIG_PVT_MNTR_IDOFFMON_PWRDN_reserved0_SHIFT 1

/* AVS_PVT_MNTR_CONFIG :: PVT_MNTR_IDOFFMON_PWRDN :: idoffmon_pwrdn [00:00] */
#define BCHP_AVS_PVT_MNTR_CONFIG_PVT_MNTR_IDOFFMON_PWRDN_idoffmon_pwrdn_MASK 0x00000001
#define BCHP_AVS_PVT_MNTR_CONFIG_PVT_MNTR_IDOFFMON_PWRDN_idoffmon_pwrdn_SHIFT 0
#define BCHP_AVS_PVT_MNTR_CONFIG_PVT_MNTR_IDOFFMON_PWRDN_idoffmon_pwrdn_DEFAULT 0x00000000

/***************************************************************************
 *PVT_MNTR_TP_MODE_ENABLE - Enabling TP mode to use data from tp_in/out pins
 ***************************************************************************/
/* AVS_PVT_MNTR_CONFIG :: PVT_MNTR_TP_MODE_ENABLE :: reserved0 [31:01] */
#define BCHP_AVS_PVT_MNTR_CONFIG_PVT_MNTR_TP_MODE_ENABLE_reserved0_MASK 0xfffffffe
#define BCHP_AVS_PVT_MNTR_CONFIG_PVT_MNTR_TP_MODE_ENABLE_reserved0_SHIFT 1

/* AVS_PVT_MNTR_CONFIG :: PVT_MNTR_TP_MODE_ENABLE :: tp_mode_en [00:00] */
#define BCHP_AVS_PVT_MNTR_CONFIG_PVT_MNTR_TP_MODE_ENABLE_tp_mode_en_MASK 0x00000001
#define BCHP_AVS_PVT_MNTR_CONFIG_PVT_MNTR_TP_MODE_ENABLE_tp_mode_en_SHIFT 0
#define BCHP_AVS_PVT_MNTR_CONFIG_PVT_MNTR_TP_MODE_ENABLE_tp_mode_en_DEFAULT 0x00000000

/***************************************************************************
 *DAC_CODE_PROGRAMMING_ENABLE - SW must set this bit to 1 to modify DAC_CODE, MIN_DAC_CODE and MAX_DAC_CODE
 ***************************************************************************/
/* AVS_PVT_MNTR_CONFIG :: DAC_CODE_PROGRAMMING_ENABLE :: reserved0 [31:01] */
#define BCHP_AVS_PVT_MNTR_CONFIG_DAC_CODE_PROGRAMMING_ENABLE_reserved0_MASK 0xfffffffe
#define BCHP_AVS_PVT_MNTR_CONFIG_DAC_CODE_PROGRAMMING_ENABLE_reserved0_SHIFT 1

/* AVS_PVT_MNTR_CONFIG :: DAC_CODE_PROGRAMMING_ENABLE :: code_program_en [00:00] */
#define BCHP_AVS_PVT_MNTR_CONFIG_DAC_CODE_PROGRAMMING_ENABLE_code_program_en_MASK 0x00000001
#define BCHP_AVS_PVT_MNTR_CONFIG_DAC_CODE_PROGRAMMING_ENABLE_code_program_en_SHIFT 0
#define BCHP_AVS_PVT_MNTR_CONFIG_DAC_CODE_PROGRAMMING_ENABLE_code_program_en_DEFAULT 0x00000000

/***************************************************************************
 *DAC_CODE - Represents the input code of the DAC
 ***************************************************************************/
/* AVS_PVT_MNTR_CONFIG :: DAC_CODE :: reserved0 [31:10] */
#define BCHP_AVS_PVT_MNTR_CONFIG_DAC_CODE_reserved0_MASK           0xfffffc00
#define BCHP_AVS_PVT_MNTR_CONFIG_DAC_CODE_reserved0_SHIFT          10

/* AVS_PVT_MNTR_CONFIG :: DAC_CODE :: dac_code [09:00] */
#define BCHP_AVS_PVT_MNTR_CONFIG_DAC_CODE_dac_code_MASK            0x000003ff
#define BCHP_AVS_PVT_MNTR_CONFIG_DAC_CODE_dac_code_SHIFT           0
#define BCHP_AVS_PVT_MNTR_CONFIG_DAC_CODE_dac_code_DEFAULT         0x00000000

/***************************************************************************
 *MIN_DAC_CODE - SW may set this to a lower value to prevent the DAC from driving too high of a voltage
 ***************************************************************************/
/* AVS_PVT_MNTR_CONFIG :: MIN_DAC_CODE :: reserved0 [31:10] */
#define BCHP_AVS_PVT_MNTR_CONFIG_MIN_DAC_CODE_reserved0_MASK       0xfffffc00
#define BCHP_AVS_PVT_MNTR_CONFIG_MIN_DAC_CODE_reserved0_SHIFT      10

/* AVS_PVT_MNTR_CONFIG :: MIN_DAC_CODE :: min_dac_code [09:00] */
#define BCHP_AVS_PVT_MNTR_CONFIG_MIN_DAC_CODE_min_dac_code_MASK    0x000003ff
#define BCHP_AVS_PVT_MNTR_CONFIG_MIN_DAC_CODE_min_dac_code_SHIFT   0
#define BCHP_AVS_PVT_MNTR_CONFIG_MIN_DAC_CODE_min_dac_code_DEFAULT 0x00000000

/***************************************************************************
 *MAX_DAC_CODE - SW may set this to a higher value to prevent the DAC from driving too low of a voltage
 ***************************************************************************/
/* AVS_PVT_MNTR_CONFIG :: MAX_DAC_CODE :: reserved0 [31:10] */
#define BCHP_AVS_PVT_MNTR_CONFIG_MAX_DAC_CODE_reserved0_MASK       0xfffffc00
#define BCHP_AVS_PVT_MNTR_CONFIG_MAX_DAC_CODE_reserved0_SHIFT      10

/* AVS_PVT_MNTR_CONFIG :: MAX_DAC_CODE :: max_dac_code [09:00] */
#define BCHP_AVS_PVT_MNTR_CONFIG_MAX_DAC_CODE_max_dac_code_MASK    0x000003ff
#define BCHP_AVS_PVT_MNTR_CONFIG_MAX_DAC_CODE_max_dac_code_SHIFT   0
#define BCHP_AVS_PVT_MNTR_CONFIG_MAX_DAC_CODE_max_dac_code_DEFAULT 0x000003ff

#endif /* #ifndef BCHP_AVS_PVT_MNTR_CONFIG_H__ */

/* End of File */