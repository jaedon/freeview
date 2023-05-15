/*
  Copyright (C) 2006-2009 NXP B.V., All Rights Reserved.
  This source code and any compilation or derivative thereof is the proprietary
  information of NXP B.V. and is confidential in nature. Under no circumstances
  is this software to be  exposed to or placed under an Open Source License of
  any type without the expressed written permission of NXP B.V.
 *
 * \file          tmbslTDA18260_LoDetuning.h
 *
 * \date          %modify_time%
 *
 * \brief         Describe briefly the purpose of this file.
 *
 * REFERENCE DOCUMENTS :
 *
 * Detailed description may be added here.
 *
 * \section info Change Information
 *
*/

#ifndef _TMBSL_TDA18260_LO_DETUNING_H
#define _TMBSL_TDA18260_LO_DETUNING_H

#ifdef __cplusplus
extern "C"
{
#endif

#define TMTDA18260_NKCORR_MAIN_PLL_MAP_NB_ROWS 40

#define TMTDA18260_NKCORR_MAIN_PLL_MAP_A    \
        {                                   \
            {33562500, 0x04, 0x0F, 0xF0},   \
            {36000000, 0x04, 0x0E, 0xE0},   \
            {38750000, 0x04, 0x0D, 0xD0},   \
            {42000000, 0x04, 0x0C, 0xC0},   \
            {45812500, 0x04, 0x0B, 0xB0},   \
            {50375000, 0x04, 0x0A, 0xA0},   \
            {56000000, 0x04, 0x09, 0x90},   \
            {63000000, 0x04, 0x08, 0x80},   \
            {67125000, 0x03, 0x0F, 0x78},   \
            {72000000, 0x03, 0x0E, 0x70},   \
            {77500000, 0x03, 0x0D, 0x68},   \
            {84000000, 0x03, 0x0C, 0x60},   \
            {91625000, 0x03, 0x0B, 0x58},   \
            {100750000, 0x03, 0x0A, 0x50},  \
            {112000000, 0x03, 0x09, 0x48},  \
            {126000000, 0x03, 0x08, 0x40},  \
            {134250000, 0x02, 0x0F, 0x3C},  \
            {144000000, 0x02, 0x0E, 0x38},  \
            {155000000, 0x02, 0x0D, 0x34},  \
            {168000000, 0x02, 0x0C, 0x30},  \
            {183250000, 0x02, 0x0B, 0x2C},  \
            {201500000, 0x02, 0x0A, 0x28},  \
            {224000000, 0x02, 0x09, 0x24},  \
            {252000000, 0x02, 0x08, 0x20},  \
            {268500000, 0x01, 0x0F, 0x1E},  \
            {288000000, 0x01, 0x0E, 0x1C},  \
            {310000000, 0x01, 0x0D, 0x1A},  \
            {336500000, 0x01, 0x0C, 0x18},  \
            {366500000, 0x01, 0x0B, 0x16},  \
            {403000000, 0x01, 0x0A, 0x14},  \
            {448000000, 0x01, 0x09, 0x12},  \
            {504000000, 0x01, 0x08, 0x10},  \
            {537000000, 0x00, 0x0F, 0x0F},  \
            {576000000, 0x00, 0x0E, 0x0E},  \
            {620000000, 0x00, 0x0D, 0x0D},  \
            {672000000, 0x00, 0x0C, 0x0C},  \
            {733000000, 0x00, 0x0B, 0x0B},  \
            {806000000, 0x00, 0x0A, 0x0A},  \
            {896000000, 0x00, 0x09, 0x09},  \
            {1008000000, 0x00, 0x08, 0x08}  \
        }

#define TMTDA18260_NKCORR_MAIN_PLL_MAP_B    \
        {                                   \
            {33875000, 0x04, 0x0D, 0xD0},   \
            {36750000, 0x04, 0x0C, 0xC0},   \
            {40062500, 0x04, 0x0B, 0xB0},   \
            {44062500, 0x04, 0x0A, 0xA0},   \
            {49000000, 0x04, 0x09, 0x90},   \
            {55125000, 0x04, 0x08, 0x80},   \
            {63000000, 0x04, 0x07, 0x70},   \
            {67750000, 0x03, 0x0D, 0x68},   \
            {73500000, 0x03, 0x0C, 0x60},   \
            {80125000, 0x03, 0x0B, 0x58},   \
            {88125000, 0x03, 0x0A, 0x50},   \
            {98000000, 0x03, 0x09, 0x48},   \
            {110250000, 0x03, 0x08, 0x40},  \
            {126000000, 0x03, 0x07, 0x38},  \
            {135500000, 0x02, 0x0D, 0x34},  \
            {147000000, 0x02, 0x0C, 0x30},  \
            {160250000, 0x02, 0x0B, 0x2C},  \
            {176250000, 0x02, 0x0A, 0x28},  \
            {196000000, 0x02, 0x09, 0x24},  \
            {220500000, 0x02, 0x08, 0x20},  \
            {252000000, 0x02, 0x07, 0x1C},  \
            {271000000, 0x01, 0x0D, 0x1A},  \
            {294000000, 0x01, 0x0C, 0x18},  \
            {320500000, 0x01, 0x0B, 0x16},  \
            {352500000, 0x01, 0x0A, 0x14},  \
            {392000000, 0x01, 0x09, 0x12},  \
            {441000000, 0x01, 0x08, 0x10},  \
            {504000000, 0x01, 0x07, 0x0E},  \
            {542000000, 0x00, 0x0D, 0x0D},  \
            {588000000, 0x00, 0x0C, 0x0C},  \
            {641000000, 0x00, 0x0B, 0x0B},  \
            {705000000, 0x00, 0x0A, 0x0A},  \
            {784000000, 0x00, 0x09, 0x09},  \
            {882000000, 0x00, 0x08, 0x08},  \
            {1008000000, 0x00, 0x07, 0x07}  \
        }

#define TMTDA18260_NKCORR_MAIN_PLL_MAP    \
    {                                     \
        TMTDA18260_NKCORR_MAIN_PLL_MAP_A, \
        TMTDA18260_NKCORR_MAIN_PLL_MAP_B  \
    };


/* PLL Map - scaler and divider configuration with a LO configuration */
typedef struct _tmTDA18260_NKCorr_PllMap_t
{
    UInt32 uLO_Max;
    UInt8  uPost_Div;
    UInt8  uPresc;
    UInt8  uDiv;
} tmTDA18260_NKCorr_PllMap_t, *ptmTDA18260_NKCorr_PllMap_t;


/* tmbslTDA18260_GetMainPLL            */
/* I: Unit number                      */
/* I: local oscillator frequency in Hz */
/* O: PreScaler                        */
/* O: PostDiv                          */
extern tmErrorCode_t tmbslTDA18260_GetMainPLL
(
    tmUnitSelect_t tUnit,
    UInt32         uLO,
    UInt8*         puPreScaler,
    UInt8*         puPostDiv
);

#ifdef __cplusplus
}
#endif

#endif /* _TMBSL_TDA18260_LO_DETUNING_H */
