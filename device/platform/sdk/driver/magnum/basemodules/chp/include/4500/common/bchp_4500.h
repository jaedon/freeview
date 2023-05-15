/***************************************************************************
 *     Copyright (c) 2003, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bchp_4500.h $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 9/5/03 1:55p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/4500/bchp_4500.h $
 * 
 * Hydra_Software_Devel/2   9/5/03 1:55p enavarro
 * added BCM4500_REVID register
 * 
 * Hydra_Software_Devel/1   8/28/03 11:01a enavarro
 * initial version
 *
 ***************************************************************************/

#ifndef BCHP_4500_H__
#define BCHP_4500_H__

/* BCM4500 registers */
#define BCM4500_HCTL1   0xA0
#define BCM4500_APSTAT1 0xA2
#define BCM4500_APMSK1  0xA3
#define BCM4500_APSTAT2 0xA4
#define BCM4500_APMSK2  0xA5
#define BCM4500_HABADR  0xA6
#define BCM4500_HABDATA 0xA7
#define BCM4500_HABSTAT 0xA8
#define BCM4500_MADRH   0xA9
#define BCM4500_MADRL   0xAA
#define BCM4500_MDATA   0xAB
#define BCM4500_APSTAT3 0xCD
#define BCM4500_APSTAT4 0xCE
#define BCM4500_APSTAT5 0xCF
#define BCM4500_REVID   0x0A

/* HCTL1 */
#define BCM4500_HCTL1_CLEAR_RESET 0x00
#define BCM4500_HCTL1_RESET       0x01
#define BCM4500_HCTL1_IDLE        0x02
#define BCM4500_HCTL1_STOP        0x04
#define BCM4500_HCTL1_CLEAR_IDLE  0x20
#define BCM4500_HCTL1_SSIN        0x80

/* APSTAT1/APMSK1 */
#define BCM4500_APSTAT1_RESET  0x01
#define BCM4500_APSTAT1_IDLE   0x02
#define BCM4500_APSTAT1_STOP   0x04
#define BCM4500_APSTAT1_RUN    0x08
#define BCM4500_APSTAT1_HABAV  0x10
#define BCM4500_APSTAT1_MEMAV  0x20
#define BCM4500_APSTAT1_ALDONE 0x40
#define BCM4500_APSTAT1_REIRQ  0x80

/* APSTAT2/APMSK2 */
#define BCM4500_APSTAT2_HABERR    0x01
#define BCM4500_APSTAT2_SCRERR    0x02
#define BCM4500_APSTAT2_FECNL     0x10
#define BCM4500_APSTAT2_FECL      0x20
#define BCM4500_APSTAT2_DISEQC    0x40
#define BCM4500_APSTAT2_INIT_DONE 0x80

/* HABSTAT */
#define BCM4500_HABSTAT_HABR     0x01
#define BCM4500_HABSTAT_LDHABR   0x02
#define BCM4500_HABSTAT_SET_HABR 0x03
#define BCM4500_HABSTAT_MCMSK    0x04
#define BCM4500_HABSTAT_HMSK     0x08
#define BCM4500_HABSTAT_LDMSK    0x10

/* APSTAT3 */
#define BCM4500_APSTAT3_DISEQC_REPLY_BYTES  0x0F
#define BCM4500_APSTAT3_DISEQC_REPLY_EXPECT 0x10
#define BCM4500_APSTAT3_DISEQC_TONE         0x20
#define BCM4500_APSTAT3_DISEQC_TONE_PRESENT 0x20
#define BCM4500_APSTAT3_DISEQC_VOLTAGE      0x40
#define BCM4500_APSTAT3_DISEQC_VTOP         0x40
#define BCM4500_APSTAT3_DISEQC_BUSY         0x80

/* APSTAT4 */
#define BCM4500_APSTAT4_DISEQC_REPLY_AVAIL  0x04
#define BCM4500_APSTAT4_DISEQC_DEMOD_ERR    0x08
#define BCM4500_APSTAT4_DISEQC_PARITY_ERR   0x10
#define BCM4500_APSTAT4_DISEQC_RRTO_ERR     0x20
#define BCM4500_APSTAT4_DISEQC_RCV_OVERFLOW 0x40
#define BCM4500_APSTAT4_DISEQC_TRANS_DONE   0x80

#endif /* #ifndef BCHP_4500_H__ */
