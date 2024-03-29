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
 * $brcm_Workfile: bchp_scb.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 8/17/11 3:26p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Thu Aug 11 21:49:12 2011
 *                 MD5 Checksum         30eb4d41dba6899590ea7664187e6ba5
 *
 * Compiled with:  RDB Utility          combo_header.pl
 *                 RDB Parser           3.0
 *                 unknown              unknown
 *                 Perl Interpreter     5.008008
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/7429/rdb/a0/bchp_scb.h $
 * 
 * Hydra_Software_Devel/1   8/17/11 3:26p pntruong
 * SW7429-2: Initial version.
 *
 ***************************************************************************/

#ifndef BCHP_SCB_H__
#define BCHP_SCB_H__

/***************************************************************************
 *SCB - Smart Card B
 ***************************************************************************/
#define BCHP_SCB_SC_UART_CMD_1                   0x00406900 /* SMART CARD UART COMMAND REGISTER */
#define BCHP_SCB_SC_UART_CMD_2                   0x00406940 /* SMART CARD UART COMMAND REGISTER */
#define BCHP_SCB_SC_PROTO_CMD                    0x0040690c /* SMART CARD PROTOCOL COMMAND REGISTER */
#define BCHP_SCB_SC_FLOW_CMD                     0x00406928 /* SMART CARD INTERFACE COMMAND REGISTER */
#define BCHP_SCB_SC_IF_CMD_1                     0x00406904 /* SMART CARD INTERFACE COMMAND REGISTER */
#define BCHP_SCB_SC_IF_CMD_2                     0x0040694c /* SMART CARD INTERFACE COMMAND REGISTER */
#define BCHP_SCB_SC_INTR_STAT_1                  0x00406958 /* SMART CARD INTERRUPT STATUS REGISTER */
#define BCHP_SCB_SC_INTR_STAT_2                  0x0040695c /* SMART CARD INTERRUPT STATUS REGISTER */
#define BCHP_SCB_SC_INTR_EN_1                    0x00406950 /* SMART CARD INTERRUPT ENABLE REGISTER */
#define BCHP_SCB_SC_INTR_EN_2                    0x00406954 /* SMART CARD INTERRUPT ENABLE REGISTER */
#define BCHP_SCB_SC_CLK_CMD_1                    0x00406908 /* SMART CARD CLOCK COMMAND */
#define BCHP_SCB_SC_CLK_CMD_2                    0x0040693c /* SMART CARD CLOCK COMMAND */
#define BCHP_SCB_SC_PRESCALE                     0x00406910 /* SMART CARD CLOCK PRESCALE */
#define BCHP_SCB_SC_TIMER_CMD                    0x00406948 /* SMART CARD TIMER COMMAND REGISTER */
#define BCHP_SCB_SC_BGT                          0x00406944 /* SMART CARD BLOCK GUARD TIME REGISTER */
#define BCHP_SCB_SC_TIMER_CNT_1                  0x00406968 /* SMART CARD GENERAL PURPOSE TIMER COUNT REGISTER */
#define BCHP_SCB_SC_TIMER_CNT_2                  0x0040696c /* SMART CARD GENERAL PURPOSE TIMER COUNT REGISTER */
#define BCHP_SCB_SC_TIMER_CMP_1                  0x00406960 /* SMART CARD GENERAL PURPOSE TIMER COMPARE REGISTER */
#define BCHP_SCB_SC_TIMER_CMP_2                  0x00406964 /* SMART CARD GENERAL PURPOSE TIMER COMPARE REGISTER */
#define BCHP_SCB_SC_WAIT_1                       0x00406970 /* SMART CARD WAITING TIMER REGISTER */
#define BCHP_SCB_SC_WAIT_2                       0x00406974 /* SMART CARD WAITING TIMER REGISTER */
#define BCHP_SCB_SC_WAIT_3                       0x00406978 /* SMART CARD INTERFACE COMMAND REGISTER */
#define BCHP_SCB_SC_IF_CMD_3                     0x0040697c /* SMART CARD WAITING TIMER REGISTER */
#define BCHP_SCB_SC_TGUARD                       0x00406914 /* SMART CARD TRANSMIT GUARD TIME REGISTER */
#define BCHP_SCB_SC_TRANSMIT                     0x00406918 /* SMART CARD TRANSMIT REGISTER */
#define BCHP_SCB_SC_RECEIVE                      0x0040691c /* SMART CARD RECEIVE REGISTER */
#define BCHP_SCB_SC_STATUS_1                     0x00406934 /* SMART CARD STATUS 1 REGISTER */
#define BCHP_SCB_SC_STATUS_2                     0x00406938 /* SMART CARD STATUS 2 REGISTER */
#define BCHP_SCB_SC_TLEN_2                       0x00406920 /* SMART CARD TRANSMIT LENGTH REGISTER */
#define BCHP_SCB_SC_TLEN_1                       0x00406924 /* SMART CARD TRANSMIT LENGTH REGISTER */
#define BCHP_SCB_SC_RLEN_2                       0x0040692c /* SMART CARD RECEIVE LENGTH REGISTER */
#define BCHP_SCB_SC_RLEN_1                       0x00406930 /* SMART CARD RECEIVE LENGTH REGISTER */
#define BCHP_SCB_SC_EVENT1_CNT                   0x00406980 /* SMART CARD EVENT 1 COUNT REGISTER */
#define BCHP_SCB_SC_EVENT1_CMP                   0x00406988 /* SMART CARD EVENT 1 COMPARE REGISTER */
#define BCHP_SCB_SC_EVENT1_CMD_1                 0x00406990 /* SMART CARD EVENT 1 COMMAND 1 REGISTER */
#define BCHP_SCB_SC_EVENT1_CMD_2                 0x00406994 /* SMART CARD EVENT 1 COMMAND 2 REGISTER */
#define BCHP_SCB_SC_EVENT1_CMD_3                 0x00406998 /* SMART CARD EVENT 1 COMMAND 3 REGISTER */
#define BCHP_SCB_SC_EVENT1_CMD_4                 0x0040699c /* SMART CARD EVENT 1 COMMAND 4 REGISTER */
#define BCHP_SCB_SC_EVENT2_CNT                   0x004069a8 /* SMART CARD EVENT 2 COUNT REGISTER */
#define BCHP_SCB_SC_EVENT2_CMP                   0x004069a0 /* SMART CARD EVENT 2 COMPARE REGISTER */
#define BCHP_SCB_SC_EVENT2_CMD_1                 0x004069b0 /* SMART CARD EVENT 2 COMMAND 1 REGISTER */
#define BCHP_SCB_SC_EVENT2_CMD_2                 0x004069b4 /* SMART CARD EVENT 2 COMMAND 2 REGISTER */
#define BCHP_SCB_SC_EVENT2_CMD_3                 0x004069b8 /* SMART CARD EVENT 2 COMMAND 3 REGISTER */
#define BCHP_SCB_SC_EVENT2_CMD_4                 0x004069bc /* SMART CARD EVENT 2 COMMAND 4 REGISTER */

#endif /* #ifndef BCHP_SCB_H__ */

/* End of File */
