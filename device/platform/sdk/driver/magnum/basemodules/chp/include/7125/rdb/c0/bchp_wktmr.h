/***************************************************************************
 *     Copyright (c) 1999-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *
 * THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 * AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 * EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bchp_wktmr.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 1/25/10 10:14p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Fri Jan 22 20:07:13 2010
 *                 MD5 Checksum         a2d1f2163f65e87d228a0fb491cb442d
 *
 * Compiled with:  RDB Utility          combo_header.pl
 *                 RDB Parser           3.0
 *                 unknown              unknown
 *                 Perl Interpreter     5.008008
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/7125/rdb/c0/bchp_wktmr.h $
 * 
 * Hydra_Software_Devel/1   1/25/10 10:14p albertl
 * SW7125-177: Initial revision.
 *
 ***************************************************************************/

#ifndef BCHP_WKTMR_H__
#define BCHP_WKTMR_H__

/***************************************************************************
 *WKTMR - Wakeup timer
 ***************************************************************************/
#define BCHP_WKTMR_EVENT                         0x00407380 /* Wakeup Timer Register */
#define BCHP_WKTMR_COUNTER                       0x00407384 /* Wakeup Counter */
#define BCHP_WKTMR_ALARM                         0x00407388 /* Wakeup Timer Alarm */
#define BCHP_WKTMR_PRESCALER                     0x0040738c /* Wakeup Timer Prescaler */
#define BCHP_WKTMR_PRESCALER_VAL                 0x00407390 /* Wakeup Timer Prescaler Value */

/***************************************************************************
 *EVENT - Wakeup Timer Register
 ***************************************************************************/
/* WKTMR :: EVENT :: reserved0 [31:01] */
#define BCHP_WKTMR_EVENT_reserved0_MASK                            0xfffffffe
#define BCHP_WKTMR_EVENT_reserved0_SHIFT                           1

/* WKTMR :: EVENT :: wktmr_alarm_event [00:00] */
#define BCHP_WKTMR_EVENT_wktmr_alarm_event_MASK                    0x00000001
#define BCHP_WKTMR_EVENT_wktmr_alarm_event_SHIFT                   0

/***************************************************************************
 *COUNTER - Wakeup Counter
 ***************************************************************************/
/* WKTMR :: COUNTER :: wktmr_counter [31:00] */
#define BCHP_WKTMR_COUNTER_wktmr_counter_MASK                      0xffffffff
#define BCHP_WKTMR_COUNTER_wktmr_counter_SHIFT                     0

/***************************************************************************
 *ALARM - Wakeup Timer Alarm
 ***************************************************************************/
/* WKTMR :: ALARM :: wktmr_alarm [31:00] */
#define BCHP_WKTMR_ALARM_wktmr_alarm_MASK                          0xffffffff
#define BCHP_WKTMR_ALARM_wktmr_alarm_SHIFT                         0

/***************************************************************************
 *PRESCALER - Wakeup Timer Prescaler
 ***************************************************************************/
/* WKTMR :: PRESCALER :: reserved0 [31:25] */
#define BCHP_WKTMR_PRESCALER_reserved0_MASK                        0xfe000000
#define BCHP_WKTMR_PRESCALER_reserved0_SHIFT                       25

/* WKTMR :: PRESCALER :: wktmr_prescaler [24:00] */
#define BCHP_WKTMR_PRESCALER_wktmr_prescaler_MASK                  0x01ffffff
#define BCHP_WKTMR_PRESCALER_wktmr_prescaler_SHIFT                 0

/***************************************************************************
 *PRESCALER_VAL - Wakeup Timer Prescaler Value
 ***************************************************************************/
/* WKTMR :: PRESCALER_VAL :: reserved0 [31:25] */
#define BCHP_WKTMR_PRESCALER_VAL_reserved0_MASK                    0xfe000000
#define BCHP_WKTMR_PRESCALER_VAL_reserved0_SHIFT                   25

/* WKTMR :: PRESCALER_VAL :: wktmr_prescaler_val [24:00] */
#define BCHP_WKTMR_PRESCALER_VAL_wktmr_prescaler_val_MASK          0x01ffffff
#define BCHP_WKTMR_PRESCALER_VAL_wktmr_prescaler_val_SHIFT         0

#endif /* #ifndef BCHP_WKTMR_H__ */

/* End of File */