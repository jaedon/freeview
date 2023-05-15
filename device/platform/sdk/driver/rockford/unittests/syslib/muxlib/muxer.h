/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: muxer.h $
 * $brcm_Revision: Hydra_Software_Devel/3 $
 * $brcm_Date: 8/14/12 10:12a $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/syslib/muxlib/muxer.h $
 * 
 * Hydra_Software_Devel/3   8/14/12 10:12a nilesh
 * SW7425-3362: Moved FNRT support to separate command to be backwards
 * compatible with old scripts
 * 
 * Hydra_Software_Devel/2   4/12/12 12:47p nilesh
 * SW7425-2568: Added explicit config_mux/cfgm command
 * 
 * Hydra_Software_Devel/1   11/3/10 2:40p nilesh
 * SW7425-38: Added TS Muxlib syslib support
 *
 ***************************************************************************/

#ifndef MUXER_H_
#define MUXER_H_

#ifdef __cplusplus
extern "C" {
#if 0
}
#endif
#endif

void BTST_OpenMux
        (BTST_P_Context *pContext);

void BTST_CloseMux
        (BTST_P_Context *pContext);

void BTST_ConfigMux
        (BTST_P_Context *pContext);

void BTST_StartMux
        (BTST_P_Context *pContext);

void BTST_StopMux
        (BTST_P_Context *pContext);

void BTST_GetMuxedFrames
        (BTST_P_Context *pContext);

void BTST_ConfigFNRT
        (BTST_P_Context *pContext);

#ifdef __cplusplus
}
#endif

#endif /* MUXER_H_ */
