/***************************************************************************
 *     Copyright (c) 2003-2007, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: btnr_3418_regio.h $
 * $brcm_Revision: Hydra_Software_Devel/4 $
 * $brcm_Date: 2/2/07 2:00p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/tnr/3418/btnr_3418_regio.h $
 * 
 * Hydra_Software_Devel/4   2/2/07 2:00p dlwin
 * PR 27583: 3418 Tuner: Reduce tuning time by using shadow registers
 * 
 * Hydra_Software_Devel/customer_OCAP_SUPPORT/1   2/1/07 4:38p ayxiang
 * tuner 3418 optimization, put shadow into 3418 handle
 * 
 * Hydra_Software_Devel/3   4/1/05 5:27p dlwin
 * PR 14682: Changed semi-private function names to a specific 3418 name.
 * 
 * Hydra_Software_Devel/2   10/29/03 8:19p dlwin
 * Added BTNR_P_ReadField16() function.
 * 
 * Hydra_Software_Devel/1   10/6/03 3:58p dlwin
 * Initial version
 * 
 ***************************************************************************/

#ifndef BTNR_3418_REGIO_H__
#define BTNR_3418_REGIO_H__

#ifdef __cplusplus
extern "C" {
#endif

void BTNR_P_3418_WriteReg( BTNR_P_3418_Settings *pTnrImplData, unsigned bytOffset, uint8_t bytData );

void BTNR_P_3418_ReadReg( BTNR_P_3418_Settings *pTnrImplData, unsigned bytOffset, uint8_t *pbytData );

void BTNR_P_3418_WriteField8( BTNR_P_3418_Settings *pTnrImplData, char *pszField, uint8_t bytData );

void BTNR_P_3418_WriteField16( BTNR_P_3418_Settings *pTnrImplData, char *pszField, uint16_t sData );

void BTNR_P_3418_ReadField8( BTNR_P_3418_Settings *pTnrImplData, char *pszField, uint8_t *pbytData );

void BTNR_P_3418_ReadField16( BTNR_P_3418_Settings *pTnrImplData, char *pszField, uint16_t *psData );

int BTNR_P_3418_GetRegNumber();

#ifdef __cplusplus
}
#endif
 
#endif

