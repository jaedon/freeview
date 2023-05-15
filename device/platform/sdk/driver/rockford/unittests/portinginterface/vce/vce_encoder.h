/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: vce_encoder.h $
 * $brcm_Revision: Hydra_Software_Devel/10 $
 * $brcm_Date: 3/7/12 4:10p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/portinginterface/vce/vce_encoder.h $
 * 
 * Hydra_Software_Devel/10   3/7/12 4:10p nilesh
 * SW7435-1: VCE Refactoring
 * 
 * Hydra_Software_Devel/SW7435-1/1   3/7/12 2:43p nilesh
 * SW7435-1: Refactored to support quad transcode
 * 
 * Hydra_Software_Devel/9   12/6/11 4:27p nilesh
 * SW7425-1746: Added "ie" command to force encoder to insert an I frame
 * 
 * Hydra_Software_Devel/8   12/5/11 2:15p nilesh
 * SW7425-960: Added VCE user data support
 * 
 * Hydra_Software_Devel/7   9/26/11 3:21p nilesh
 * SW7425-1213: Added BVCE_Debug_SendCommand support
 * 
 * Hydra_Software_Devel/6   8/16/11 1:58p nilesh
 * SW7425-448: Added VCE Standby/Resume Commands
 * 
 * Hydra_Software_Devel/5   2/10/11 4:08p hongtaoz
 * SW7425-1: added VCE channel open/close test;
 * 
 * Hydra_Software_Devel/4   12/20/10 3:51p nilesh
 * SW7425-1: Added VCE ITB/CDB buffer dump command "ice"
 * 
 * Hydra_Software_Devel/3   11/2/10 12:24p nilesh
 * SW7425-1: Added encoder debug log command "le"
 * 
 * Hydra_Software_Devel/2   10/21/10 4:59p nilesh
 * SW7425-1: Implemented GetStatus. Added GetEncodedFrames.
 * 
 * Hydra_Software_Devel/1   10/21/10 11:19a nilesh
 * SW7425-1: Added VCE Encoder Support (enable by compiling with
 * SUPPORT_VCE=y)
 *
 ***************************************************************************/

#ifndef VCE_ENCODER_H_
#define VCE_ENCODER_H_

#ifdef __cplusplus
extern "C" {
#if 0
}
#endif
#endif

void BTST_StartEncoder
        (BTST_P_Context *pContext);

void BTST_StopEncoder
        (BTST_P_Context *pContext);

void BTST_OpenEncoder
        ( BTST_P_Context *pContext );

void BTST_CloseEncoder
        ( BTST_P_Context *pContext );

void BTST_OpenEncChan
        ( BTST_P_Context *pContext );

void BTST_CloseEncChan
        ( BTST_P_Context *pContext );

void BTST_GetEncoderStatus
        ( BTST_P_Context *pContext );

void BTST_GetEncodedFrames
        ( BTST_P_Context *pContext );
void BTST_GetItbBCdbBuffers
        ( BTST_P_Context *pContext );
void BTST_GetEncoderDebugLog
        ( BTST_P_Context *pContext );

void BTST_StandbyEncoder
        (BTST_P_Context *pContext);

void BTST_ResumeEncoder
        ( BTST_P_Context *pContext );

void BTST_DebugEncoder
         ( BTST_P_Context *pContext );

void BTST_UserDataEncode
         ( BTST_P_Context *pContext );

void BTST_BeginNewRapEncode
         ( BTST_P_Context *pContext );

void BTST_SetActiveEncoder
         ( BTST_P_Context *pContext );

void BTST_GetActiveEncoder
         ( BTST_P_Context *pContext );

void BTST_InitializeEncoder
         ( BTST_P_Context *pContext );

#ifdef __cplusplus
}
#endif

#endif /* VCE_ENCODER_H_ */
