/***************************************************************************
 *     Copyright (c) 2005-2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: $
 * $brcm_Revision: $
 * $brcm_Date: $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: $
 * 
  ***************************************************************************/

#ifndef BWFE_PRIV_H__
#define BWFE_PRIV_H__

#include "bwfe.h"


#define BWFE_CHK_RETCODE(x) \
   { if ((retCode = (x)) != BERR_SUCCESS) goto done; }

#define BWFE_ABS(x) (((x) < 0) ? -(x):(x))

#ifdef BWFE_FLOAT_MODE
   #define BWFE_ROUND(x) ((x >= 0) ? (int)(x + 0.5) : (int)(x - 0.5))
   #define BWFE_CORDIC_NUM_ITER  40
   #define BWFE_CORDIC_K         0.60725293500888
   #define BWFE_PI               3.14159265358979323846
   #define BWFE_SINC_LBOUND      1.0e-8   /* lower bound for sin(x)/x */
   #define BWFE_DELAY_SCALE      262144.0 /* 2^18 is a very rough femtosecond ~2^18.49 */
   #define BWFE_DELAY_DELTA      (int32_t)(2e-10 * BWFE_DEF_FS_ADC_KHZ * BWFE_DELAY_SCALE)   /* 0.2ps*Fadc  *DLY_SCL */
#else
   #define BWFE_CORDIC_NUM_ITER  28          /* iterations needs to be <= 31 to avoid div by 2^31 int/uint issue */
   #define BWFE_CORDIC_SCALE     1073741824  /* 2^30 */
   #define BWFE_CORDIC_K         652032874   /* 0.60725293500888 * 2^30 */
   #define BWFE_CORDIC_SHIFT     30
   #define BWFE_DELAY_SCALE      262144      /* 2^18 is a very rough femtosecond ~2^18.49 */
   #define BWFE_AGC_SCALE        16384       /* 2^14 */
#endif


struct BWFE_P_ChannelHandle;


/******************************************************************************
Summary:
   This is the structure for the BWFE_Handle. 
******************************************************************************/
typedef struct BWFE_P_Handle
{
   BWFE_Settings                 settings;            /* user settings */
   uint8_t                       totalChannels;       /* total input channels */
   uint8_t                       totalRefChannels;    /* total reference channels */
   struct BWFE_P_ChannelHandle   **pChannels;         /* pointer to array of channel handles */
   struct BWFE_P_ChannelHandle   **pRefChannels;      /* pointer to array of ref channel handles */
   void                          *pImpl;              /* pointer to specific implementation */
} BWFE_P_Handle;


/******************************************************************************
Summary:
   This is the structure for the BWFE_ChannelHandle. 
******************************************************************************/
typedef struct BWFE_P_ChannelHandle
{
   BWFE_ChannelSettings    settings;      /* channel settings */
   BWFE_P_Handle           *pDevice;      /* pointer to device handle */
   uint8_t                 channel;       /* channel number */
   uint8_t                 totalSlices;   /* number of slices */
   bool                    bEnabled;      /* ADC enabled */
   bool                    bReference;    /* reference ADC indicator */
   void                    *pImpl;        /* pointer to specific implementation */
} BWFE_P_ChannelHandle;


/* private functions */
BERR_Code BWFE_P_InitConfig(BWFE_ChannelHandle h);
BERR_Code BWFE_P_ReadRegister(BWFE_ChannelHandle h, uint32_t reg, uint32_t *val);
BERR_Code BWFE_P_WriteRegister(BWFE_ChannelHandle h, uint32_t reg, uint32_t val);
BERR_Code BWFE_P_ReadSliceRegister(BWFE_ChannelHandle h, uint8_t slice, uint32_t reg, uint32_t *val);
BERR_Code BWFE_P_WriteSliceRegister(BWFE_ChannelHandle h, uint8_t slice, uint32_t reg, uint32_t val);
BERR_Code BWFE_P_SetAdcSampleFreq(BWFE_ChannelHandle h, uint32_t freqKhz);
BERR_Code BWFE_P_GetAdcSampleFreq(BWFE_ChannelHandle h, uint32_t *freqKhz);
BERR_Code BWFE_P_SetDpmPilotFreq(BWFE_ChannelHandle h, uint32_t freqKhz);
BERR_Code BWFE_P_GetDpmPilotFreq(BWFE_ChannelHandle h, uint32_t *freqKhz);
BERR_Code BWFE_P_EnableDpmPilot(BWFE_ChannelHandle h);
BERR_Code BWFE_P_DisableDpmPilot(BWFE_ChannelHandle h);
BERR_Code BWFE_P_InitAdc(BWFE_ChannelHandle h);
BERR_Code BWFE_P_ShutdownAdc(BWFE_ChannelHandle h);
bool BWFE_P_IsAdcOn(BWFE_ChannelHandle h);
#ifndef BWFE_EXCLUDE_SPECTRUM_ANALYZER
BERR_Code BWFE_P_CalcCorrPower(BWFE_ChannelHandle h, int32_t *corrI, int32_t *corrQ, uint32_t *corrPowerDb);
#endif

/* bwfe_priv.c */
void BWFE_P_ReadModifyWriteRegister(BWFE_ChannelHandle h, uint32_t reg, uint32_t and_mask, uint32_t or_mask);
void BWFE_P_OrRegister(BWFE_ChannelHandle h, uint32_t reg, uint32_t or_mask);
void BWFE_P_AndRegister(BWFE_ChannelHandle h, uint32_t reg, uint32_t and_mask);
void BWFE_P_ToggleBit(BWFE_ChannelHandle h, uint32_t reg, uint32_t mask);
void BWFE_P_ReadModifyWriteSliceReg(BWFE_ChannelHandle h, uint8_t slice, uint32_t reg, uint32_t and_mask, uint32_t or_mask);
void BWFE_P_OrSliceReg(BWFE_ChannelHandle h, uint8_t slice, uint32_t reg, uint32_t or_mask);
void BWFE_P_AndSliceReg(BWFE_ChannelHandle h, uint8_t slice, uint32_t reg, uint32_t and_mask);
void BWFE_P_ToggleSliceBit(BWFE_ChannelHandle h, uint8_t slice, uint32_t reg, uint32_t mask);
#ifdef BWFE_FLOAT_MODE
BERR_Code BWFE_P_CordicRotationMode(double *vec, double theta, bool unitMode);
BERR_Code BWFE_P_CordicVectorMode(double *vec, double *theta);
#else
BERR_Code BWFE_P_CordicRotationMode(int32_t *vec, int32_t theta, bool unitMode);
BERR_Code BWFE_P_CordicVectorMode(int32_t *vec, int32_t *theta);
#endif
uint32_t BWFE_P_GCF(uint32_t m, uint32_t n);
uint8_t BWFE_P_GetMsb(uint32_t i);
void BWFE_P_SumThenMultiply(int32_t x, int32_t y, int32_t m, uint32_t *result_hi, uint32_t *result_lo);


#endif /* BWFE_PRIV_H__ */