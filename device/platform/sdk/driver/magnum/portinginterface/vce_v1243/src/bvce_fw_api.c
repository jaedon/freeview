/***************************************************************************
 *     Copyright (c) 2003-2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/
/*
   Title: calcDelay

   Computes End-To-End delay for Video

   $brcm_Revision: Hydra_Software_Devel/27 $
   $brcm_Date: 8/28/13 4:35p $


   NOTE: THIS FILE SHOULD NOT CONTAIN ANY C++-STYLE COMMENTS

*/


/* ==========================  INCLUDES ============================== */
#ifdef BCHP_CHIP
#include "bstd.h"
#include "bdbg.h"
#else
#include <stdint.h>
#include <assert.h>
#if !defined(CEB2APP) && !defined(NGE_FW_SIM_APP)
#include "defs.h"
#include "util.h"
#include "platform.h"
#include "fw_debug.h"
#endif
#endif

#include "bvce_fw_api.h"


/* ==========================  DEFINITIONS ======================== */

#define MAX_CABAC_PIC_DELAY                       15
#define MAX_NUM_EBM_PIC_DATA                      50        /* should be greater than MAX_CABAC_PIC_DELAY and cover max number of Pictures in EndToEndDelay */

/* Definitions used for arrival-to-presentation delay used in rate control */
#define DTS_OFFSET_IN_27MHZ_TICKS                  0            /* Delay offset used to ensure no late arrivals */
#define MAX_NUM_REPEAT_PIC_COUNT                   2            /* Max allowed count of repeated pictures */

#define NORMALIZE_ONE_TO_15_BITS_MANTISSA   (0x4000)
#define NORMALIZE_ONE_TO_15_BITS_EXPONENT   (14)
#define  SYSTEM_CLOCK_27_MHZ            27000000
#define TWENTY_SEVEN_KHZ                27000
#define BITS_PER_PIC2MAXPIC_MANTISSA    27
#define BITS_PER_PIC2MAXPIC_EXPONENT    1

#define INVERSE_FRAMERATE_6000_IN_SECONDS_MANTISSA  0x4444
#define INVERSE_FRAMERATE_5994_IN_SECONDS_MANTISSA  0x4455
#define INVERSE_FRAMERATE_5000_IN_SECONDS_MANTISSA  0x51EB
#define INVERSE_FRAMERATE_3000_IN_SECONDS_MANTISSA  0x4444
#define INVERSE_FRAMERATE_2997_IN_SECONDS_MANTISSA  0x4455
#define INVERSE_FRAMERATE_2500_IN_SECONDS_MANTISSA  0x51EB
#define INVERSE_FRAMERATE_2400_IN_SECONDS_MANTISSA  0x5555
#define INVERSE_FRAMERATE_2397_IN_SECONDS_MANTISSA  0x556B
#define INVERSE_FRAMERATE_2000_IN_SECONDS_MANTISSA  0x6666
#define INVERSE_FRAMERATE_1500_IN_SECONDS_MANTISSA  0x4444
#define INVERSE_FRAMERATE_1498_IN_SECONDS_MANTISSA  0x4455

#define INVERSE_FRAMERATE_6000_IN_SECONDS_EXPONENT  20
#define INVERSE_FRAMERATE_5994_IN_SECONDS_EXPONENT  20
#define INVERSE_FRAMERATE_5000_IN_SECONDS_EXPONENT  20
#define INVERSE_FRAMERATE_3000_IN_SECONDS_EXPONENT  19
#define INVERSE_FRAMERATE_2997_IN_SECONDS_EXPONENT  19
#define INVERSE_FRAMERATE_2500_IN_SECONDS_EXPONENT  19
#define INVERSE_FRAMERATE_2400_IN_SECONDS_EXPONENT  19
#define INVERSE_FRAMERATE_2397_IN_SECONDS_EXPONENT  19
#define INVERSE_FRAMERATE_2000_IN_SECONDS_EXPONENT  19
#define INVERSE_FRAMERATE_1500_IN_SECONDS_EXPONENT  18
#define INVERSE_FRAMERATE_1498_IN_SECONDS_EXPONENT  18

#define INVERSE_FRAMERATE_6000_IN_27MHZ_TICKS    450000
#define INVERSE_FRAMERATE_5994_IN_27MHZ_TICKS    450450
#define INVERSE_FRAMERATE_5000_IN_27MHZ_TICKS    540000
#define INVERSE_FRAMERATE_3000_IN_27MHZ_TICKS    900000
#define INVERSE_FRAMERATE_2997_IN_27MHZ_TICKS    900900
#define INVERSE_FRAMERATE_2500_IN_27MHZ_TICKS    1080000
#define INVERSE_FRAMERATE_2400_IN_27MHZ_TICKS    1125000
#define INVERSE_FRAMERATE_2397_IN_27MHZ_TICKS    1126126
#define INVERSE_FRAMERATE_2000_IN_27MHZ_TICKS    1350000
#define INVERSE_FRAMERATE_1500_IN_27MHZ_TICKS    1800000
#define INVERSE_FRAMERATE_1498_IN_27MHZ_TICKS    1801800

#define INVERSE_FRAMERATE_1498_IN_90KHZ_TICKS    6006
#define MAX_PIC_INTERVAL_IN_90KHZ_TICKS         INVERSE_FRAMERATE_1498_IN_90KHZ_TICKS
#define MAX_PIC_INTERVAL_IN_27MHZ_TICKS         (MAX_PIC_INTERVAL_IN_90KHZ_TICKS*300)
#define MIN_PIC_INTERVAL_IN_27MHZ_TICKS         INVERSE_FRAMERATE_6000_IN_27MHZ_TICKS
#define MAX_PRE_ENCODE_DELAY_VIP                INVERSE_FRAMERATE_2400_IN_27MHZ_TICKS
#define MAX_PRE_ENCODE_DELAY_CME                INVERSE_FRAMERATE_3000_IN_27MHZ_TICKS
#define MAX_PRE_ENCODE_DELAY_ITFP               (MAX_NUM_REPEAT_PIC_COUNT * INVERSE_FRAMERATE_6000_IN_27MHZ_TICKS)
#define MAX_ENCODE_DELAY                        INVERSE_FRAMERATE_3000_IN_27MHZ_TICKS

/* define max CPB size according to level fo H264 */
#define H264_MAX_CPB_LEVEL_10  (1000*175)
#define H264_MAX_CPB_LEVEL_1b  (1000*350)
#define H264_MAX_CPB_LEVEL_11  (1000*500)
#define H264_MAX_CPB_LEVEL_12  (1000*1000)
#define H264_MAX_CPB_LEVEL_13  (1000*2000)
#define H264_MAX_CPB_LEVEL_20  (1000*2000)
#define H264_MAX_CPB_LEVEL_21  (1000*4000)
#define H264_MAX_CPB_LEVEL_22  (1000*4000)
#define H264_MAX_CPB_LEVEL_30  (1000*10000)
#define H264_MAX_CPB_LEVEL_31  (1000*14000)
#define H264_MAX_CPB_LEVEL_32  (1000*20000)
#define H264_MAX_CPB_LEVEL_40  (1000*25000)
#define H264_MAX_CPB_LEVEL_41  (1000*62500)
#define H264_MAX_CPB_LEVEL_42  (1000*62500)
#define H264_MAX_CPB_LEVEL_50  (1000*135000)
#define H264_MAX_CPB_LEVEL_51  (1000*240000)

/* define max CPB size according to level fo MPEG2 Main Profile */
#define MPEG2_MAX_VBV_LEVEL_LOW         475136
#define MPEG2_MAX_VBV_LEVEL_MAIN        1835008
#define MPEG2_MAX_VBV_LEVEL_HIGH1440    7340032
#define MPEG2_MAX_VBV_LEVEL_HIGH        9781248

/* define max buffer size according to profile and level for MPEG4 */
#define MPEG4_MAX_VBV_SP_LEVEL_0    (10*16384)
#define MPEG4_MAX_VBV_SP_LEVEL_1    (10*16384)
#define MPEG4_MAX_VBV_SP_LEVEL_2    (40*16384)
#define MPEG4_MAX_VBV_SP_LEVEL_3    (40*16384)

#define MPEG4_MAX_VBV_ASP_LEVEL_0    (10*16384)
#define MPEG4_MAX_VBV_ASP_LEVEL_1    (10*16384)
#define MPEG4_MAX_VBV_ASP_LEVEL_2    (40*16384)
#define MPEG4_MAX_VBV_ASP_LEVEL_3    (40*16384)
#define MPEG4_MAX_VBV_ASP_LEVEL_3B   (65*16384)
#define MPEG4_MAX_VBV_ASP_LEVEL_4    (80*16384)
#define MPEG4_MAX_VBV_ASP_LEVEL_5    (112*16384)

/* FIXME: this is just to keep the interface, however at this point VP8 does not define any HRD */
#define VP8_DUMMY_HRD_BUFFER_SIZE          (1000*240000)

#define RESOLUTION_T0_27MHZ_MBPDELAY_MANT       56471
#define RESOLUTION_T0_27MHZ_MBPDELAY_EXP        17
#define LOW_DELAY_ADDITIVE_DELAY_IN_27MHZ       108000      /* 4 milliseconds for coding several MB rows */
#define MAX_DEE_IN_MILLISECONDS                 3000        /* used to limit internal rate buffer delay calculation */
#define MAX_DEE_IN_27_MHZ_TICKS                 (MAX_DEE_IN_MILLISECONDS*TWENTY_SEVEN_KHZ )
#define HORIZONTAL_SIZE_IN_PELS_SD              720
#define HORIZONTAL_SIZE_IN_PELS_HD              1280


/* DEFINE TEMPORARY INPUT UNTIL API COMPLETE FOR A2PDELAY CALC */
#define TEMP_INPUT_API_MODE                 ENCODER_MODE_AFAP
#define TEMP_INPUT_API_RATE_BUFFER          0
#define TEMP_INPUT_API_MIN_FRAMERATE        ENCODING_FRAME_RATE_1498
#define TEMP_INPUT_API_BVN_RATE             ENCODING_FRAME_RATE_5994
#define TEMP_INPUT_API_MULTI_CHAN           0
#define TEMP_INPUT_API_INPUT_TYPE           ENCODER_INPUT_TYPE_INTERLACED
#define TEMP_INPUT_API_GOP_STRUCT           ENCODING_GOP_STRUCT_IBBP
#define TEMP_INPUT_API_MAX_PIC_WIDTH_PELS   1920
#define TEMP_INPUT_API_MAX_PIC_HEIGT_PELS   1088


#if defined(BCHP_CHIP) || defined(CEB2APP) || defined(NGE_FW_SIM_APP)

#define NORM31(output, input) {uint8_t i; for (i=0; (i<=31) && (input>>i != 0) && (input>>i != -1); i++); output= 31-i;}
#define DIVIDE16(numerator, denominator) arcDivide16((int16_t)numerator, (int16_t)denominator)
#define UMULT16(operandA, operandB) (uint32_t)((uint16_t)((operandA) & UINT16_MAX) * (uint16_t)((operandB) & UINT16_MAX))
#define U32xU16MULTIPLY(operand32bit, operand16bit) ((UMULT16((operand32bit)>>16, (operand16bit))<<16) + UMULT16((operand32bit), (operand16bit)))
#define U32xU32MULTIPLY(operand32bit1, operand32bit2) (((u64_t)UMULT16((operand32bit1 >> 16), (operand32bit2>>16)) << 32) +\
        ((u64_t)UMULT16((operand32bit1 >> 16),operand32bit2) << 16) +\
        ((u64_t)UMULT16(operand32bit1,(operand32bit2 >> 16)) << 16) + (u64_t)UMULT16(operand32bit1, operand32bit2))
#define U32xPOSQ15MULTIPLY(operand32bit,  operand16bit) ((UMULT16((operand32bit)>>16, (operand16bit))<<1) + (UMULT16((operand32bit), (operand16bit))>>15))
#define U32xPOSQ8MULTIPLY(operand32bit,  operand16bit) ((UMULT16((operand32bit)>>16, (operand16bit))<<8) + (UMULT16((operand32bit), (operand16bit))>>8))

/* ==========================  STRUCTURES  ================================== */
typedef struct FPNumber
{
    uint32_t mantissa;        /* the significant digits of the floating point number */
    int32_t exponent;         /* the floating point number exponent */
} FPNumber_t;


static void RcNumToFP(FPNumber_t *outputFPNumber, uint32_t inputNumber, uint8_t numBits);
static uint32_t RcFPToNum(FPNumber_t localFPNumber);
static uint32_t RcU32ScaleFP(FPNumber_t FP_operand, uint32_t scaleFactor, uint8_t bypassNormFlag);
static int16_t arcDivide16(int16_t numerator, int16_t denominator);
static void NormalizeFP(FPNumber_t *pOutputFPNumber, FPNumber_t pInputFPNumber, uint8_t numBits);

#else

extern void_t RcNumToFP(FPNumber_t *outputFPNumber, u32_t inputNumber, u8_t numBits);
extern u32_t RcFPToNum(FPNumber_t localFPNumber);
extern u32_t RcU32ScaleFP(FPNumber_t FP_operand, u32_t scaleFactor, uint8_t bypassNormFlag);
extern void_t NormalizeFP(FPNumber_t *pOutputFPNumber, FPNumber_t pInputFPNumber, u8_t numBits);

#endif /* defined(BCHP_CHIP) || defined(CEB2APP) || defined(NGE_FW_SIM_APP) */

/* ==========================  DATA  ============================== */

/* ==========================  CODE  ============================== */
uint32_t BVCE_FW_P_ConvertFrameRate(FrameRate_e framesPerSecond, FPNumber_t *FP_inverse);
uint32_t BVCE_FW_P_CalcHRDbufferSize(uint32_t  Protocol, uint32_t  Profile, EncoderLevel_e  Level);
uint32_t BVCE_FW_P_Compute_DeeIn27MhzTicks(uint32_t averageBitsPerPic, uint32_t hrdBufferSize, FPNumber_t FP_ticksPerBit, 
                                           uint32_t maxPictureIntervalIn27MhzTicks, uint32_t *pMaxAllowedBitsPerPicture, uint8_t MaxNumFNRT);

uint32_t BVCE_FW_P_Compute_MaxDeeIn27MhzTicks(uint32_t hrdBufferSize, FPNumber_t FP_ticksPerBit);
void BVCE_FW_P_ComputeRateInTicksPerBit(FPNumber_t *pt_FP_ticksPerBit, uint32_t bitrateBPS);
uint32_t BVCE_FW_P_Compute_EncodeDelayIn27MHzTicks(uint32_t MinAllowedBvnFrameRate, uint8_t ITFPenable, uint8_t Mode, FrameRate_e MinFrameRateLimit, uint8_t InputType, uint8_t MaxAllowedGopStruct, uint32_t PictureWidthInPels, uint32_t PictureHeightInPels);

#ifndef BSTD_UNUSED
#define BSTD_UNUSED(x)
#endif

#if defined(BCHP_CHIP) || defined(CEB2APP) || defined(NGE_FW_SIM_APP)
/***********************************************************************
*  CalcVideoEndToEndDelay()
*    Calculates Arrival-To-Presentation (A2P) delay and maxAllowed A2P
*    delay for video
************************************************************************/
uint32_t
BVCE_FW_P_CalcVideoA2Pdelay(
         uint32_t            *MaxAllowedA2Pdelay,       /* output max A2P delay allowed for Video                */
         uint32_t            Protocol,                  /* encoding standard                                     */
         uint32_t            Profile,                   /* encoding profile                                      */
         EncoderLevel_e      Level,                     /* Profile Level                                         */
         FrameRate_e         FrameRate,                 /* Frame rate in unit of integer (100*frames/sec)        */
         uint32_t            MaxBitrate,                /* Peak output bit rate in bits/sec                      */
         uint8_t             Mode,                      /* Encoder mode of operation                             */
         uint32_t            RateBufferDelay,           /* Input rate buffer delay                               */
         FrameRate_e         MinFrameRateLimit,         /* Minimum frame rate allowed during dynamic rate change */
         FrameRate_e         MinAllowedBvnFrameRate,    /* Input BVN frame rate                                  */
         uint8_t             MultiChannelEnable,        /* Flag dual channel operation                           */
         uint8_t             ITFPenable,                /* ITFP enable                                           */
         uint8_t             InputType,                 /* Interlaced or Progressive input type                  */
         uint8_t             MaxAllowedGopStruct,                   /* Gop structure                                         */
         uint32_t            PictureWidthInPels,        /* Max picture resolution allowed                        */
         uint32_t            PictureHeightInPels,
         uint8_t			 MaxNumFNRT 				/* Max number os parallel transcodes					*/
)
{
    FPNumber_t FP_ticksPerBit, FP_inverse;
    uint32_t MaxSupportedEndToEndDelayIn27MHz;
    uint32_t delayEndToEndIn27MhzTicks;
    uint32_t   hrdBufferSize;
    uint32_t   averageBitsPerPic;
    uint32_t   minPictureIntervalIn27MhzTicks, ptsDtsDelayOffset = 0;
    uint32_t encodingDelays, A2PdelayIn27MHzTicks, maxAllowedBitsPerPicture;
    uint32_t   maxPictureIntervalIn27MhzTicks = 0;

    BSTD_UNUSED( FrameRate );
    BSTD_UNUSED( MultiChannelEnable );

    /* Initialize output parameters */
    delayEndToEndIn27MhzTicks = 0;
    MaxAllowedA2Pdelay[0] = MaxSupportedEndToEndDelayIn27MHz = 0;
    averageBitsPerPic = 0;
    hrdBufferSize = 0;

    /* Dynamic frame rate change support requires that storage be set aside for maximum supported frame rate */
    minPictureIntervalIn27MhzTicks = MIN_PIC_INTERVAL_IN_27MHZ_TICKS;

    /* Convert min frame rate to inverse  */
    maxPictureIntervalIn27MhzTicks = BVCE_FW_P_ConvertFrameRate(MinFrameRateLimit, &FP_inverse);

    /* Compute max average bits per pic for input frame rate */
    FP_inverse.mantissa = U32xPOSQ15MULTIPLY(MaxBitrate, FP_inverse.mantissa);
    FP_inverse.exponent -= 15;
    averageBitsPerPic = RcFPToNum(FP_inverse);


    /* Compute size based on encoding standard level and profile */
    hrdBufferSize = BVCE_FW_P_CalcHRDbufferSize(Protocol, Profile, Level);

    /* Convert rate in ticks per bit */
    BVCE_FW_P_ComputeRateInTicksPerBit(&FP_ticksPerBit, MaxBitrate);
    NormalizeFP(&FP_ticksPerBit, FP_ticksPerBit, 16);

    /* Compute end to end delay in 27Mhz ticks. Also update the max allowed bits per picture */
    if(RateBufferDelay == 0)
    {
        if(Mode == ENCODER_MODE_LOW_DELAY)
        {
                delayEndToEndIn27MhzTicks = maxPictureIntervalIn27MhzTicks;
        }
        else
        {
            /* if not in low Delay, make minimum delayEndToEnd to be hard-coded max pic interval for potentially better picture quality */
            delayEndToEndIn27MhzTicks = BVCE_FW_P_Compute_DeeIn27MhzTicks(averageBitsPerPic, hrdBufferSize, FP_ticksPerBit, 
                                                                           MAX_PIC_INTERVAL_IN_27MHZ_TICKS, &maxAllowedBitsPerPicture, MaxNumFNRT);
        }
    }
    else
    {
        /* convert milliseconds to 27 MHz ticks */
        FP_inverse.mantissa = RateBufferDelay;
        FP_inverse.exponent = 0;
        delayEndToEndIn27MhzTicks = RcU32ScaleFP(FP_inverse,  TWENTY_SEVEN_KHZ, 0);
    }


    /* Compute the true max end to end delay supported based on HRD size and bit rate */
    MaxSupportedEndToEndDelayIn27MHz = BVCE_FW_P_Compute_MaxDeeIn27MhzTicks(hrdBufferSize, FP_ticksPerBit);
    MaxSupportedEndToEndDelayIn27MHz -= maxPictureIntervalIn27MhzTicks;
    
    /* end-to-end delay should be less than Max supported to always ensure no HRD overflow */
    if (delayEndToEndIn27MhzTicks > MaxSupportedEndToEndDelayIn27MHz)
    {
        delayEndToEndIn27MhzTicks = MaxSupportedEndToEndDelayIn27MHz;
    }

    /* add max pre-encoding and encoding delays */
    encodingDelays = BVCE_FW_P_Compute_EncodeDelayIn27MHzTicks(MinAllowedBvnFrameRate, ITFPenable, Mode, MinFrameRateLimit, InputType, MaxAllowedGopStruct, PictureWidthInPels, PictureHeightInPels);

    if ((MaxAllowedGopStruct == ENCODING_GOP_STRUCT_IBP) ||
        (MaxAllowedGopStruct == ENCODING_GOP_STRUCT_IBBP) ||
        (MaxAllowedGopStruct == ENCODING_GOP_STRUCT_IBBBP) ||
        (MaxAllowedGopStruct == ENCODING_GOP_STRUCT_TRACK_INPUT))
    {
        /* Offset between PTS and DTS when B pictures allowed */
        ptsDtsDelayOffset = maxPictureIntervalIn27MhzTicks;
    }
    else
    {
        ptsDtsDelayOffset = 0;
    }

    MaxAllowedA2Pdelay[0] = MaxSupportedEndToEndDelayIn27MHz + encodingDelays + ptsDtsDelayOffset;

    /* check for overflow and saturate if it occurs */
    if(MaxAllowedA2Pdelay[0] < MaxSupportedEndToEndDelayIn27MHz )
    {
        MaxAllowedA2Pdelay[0] = UINT32_MAX;
    }

    A2PdelayIn27MHzTicks  = delayEndToEndIn27MhzTicks + encodingDelays + ptsDtsDelayOffset;

    if(A2PdelayIn27MHzTicks > MaxAllowedA2Pdelay[0])
    {
        A2PdelayIn27MHzTicks = MaxAllowedA2Pdelay[0];
    }

    return(A2PdelayIn27MHzTicks);
}
#endif

/***********************************************************************
*  BVCE_FW_P_Compute_EncodeDelayIn27MHzTicks() - Computes PreEncode and
*  Encode Delay in units expressed as 27 MHz clock ticks
************************************************************************/
uint32_t
BVCE_FW_P_Compute_EncodeDelayIn27MHzTicks(
        uint32_t    MinAllowedBvnFrameRate,
        uint8_t     ITFPenable,
        uint8_t     Mode,
        FrameRate_e MinFrameRateLimit,
        uint8_t     InputType,
        uint8_t     MaxAllowedGopStruct,
        uint32_t    PictureWidthInPels,
        uint32_t    PictureHeightInPels
    )
{
    uint32_t vipdelay = 0;
    uint32_t mbpdelay = 0;
    uint32_t vipItfpDelay = 0;
    uint32_t reorderDelay = 0;
    uint32_t mbpCmeDelay = 0;
    uint32_t PictureIntervalIn27MhzTicks = 0;
    uint32_t pixelsPerFrame = 0;
    FPNumber_t FP_temp;


    PictureIntervalIn27MhzTicks = BVCE_FW_P_ConvertFrameRate(MinFrameRateLimit, &FP_temp);
    if(Mode == ENCODER_MODE_LOW_DELAY)
    {
        mbpCmeDelay = PictureIntervalIn27MhzTicks + LOW_DELAY_ADDITIVE_DELAY_IN_27MHZ;
    }
    else
    {

        /* compute Encode Delay based on BVN Rate */
        switch(MinAllowedBvnFrameRate)
        {
        /* stored as 100xFrameRate */
        case ENCODING_FRAME_RATE_6000:
            /* need BVN picture interval in 27 MHZ ticks */
            vipItfpDelay = INVERSE_FRAMERATE_6000_IN_27MHZ_TICKS;
            break;
        case ENCODING_FRAME_RATE_5994:
            vipItfpDelay = INVERSE_FRAMERATE_5994_IN_27MHZ_TICKS;
            break;
        case ENCODING_FRAME_RATE_5000:
            vipItfpDelay = INVERSE_FRAMERATE_5000_IN_27MHZ_TICKS;
            break;
        case ENCODING_FRAME_RATE_3000:
            vipItfpDelay = INVERSE_FRAMERATE_3000_IN_27MHZ_TICKS;
            break;
        case ENCODING_FRAME_RATE_2997:
            vipItfpDelay = INVERSE_FRAMERATE_2997_IN_27MHZ_TICKS;
            break;
        case ENCODING_FRAME_RATE_2500:
            vipItfpDelay = INVERSE_FRAMERATE_2500_IN_27MHZ_TICKS;
            break;
        case ENCODING_FRAME_RATE_2400:
            vipItfpDelay = INVERSE_FRAMERATE_2400_IN_27MHZ_TICKS;
            break;
        case ENCODING_FRAME_RATE_2397/*.6*/:
            vipItfpDelay = INVERSE_FRAMERATE_2397_IN_27MHZ_TICKS;
            break;
        case ENCODING_FRAME_RATE_2000:
            vipItfpDelay = INVERSE_FRAMERATE_2000_IN_27MHZ_TICKS;
            break;
        case ENCODING_FRAME_RATE_1500:
            vipItfpDelay = INVERSE_FRAMERATE_1500_IN_27MHZ_TICKS;
            break;
        case ENCODING_FRAME_RATE_1498:
            vipItfpDelay = INVERSE_FRAMERATE_1498_IN_27MHZ_TICKS;
            break;
        default:
#ifdef BCHP_CHIP
            BDBG_ASSERT(!"unsupported BVN frame rate");
            return 0;
#else
#if defined(CEB2APP) || defined (NGE_FW_SIM_APP)
            assert(!"unsupported BVN frame rate");
#else
            ASSERT(!"unsupported BVN frame rate");
#endif
            break;
#endif
        }

        vipdelay = vipItfpDelay;

        /* Scale Delay to b 4x BVN rate based on ITFP enable flag for interlace */
        if((ITFPenable == 1) && (InputType == ENCODER_INPUT_TYPE_INTERLACED))
        {
            vipItfpDelay <<= 2;
        }

        /* Add Reorder Delay based on max allowed gopstruct and minFrameRate Limit by scaling 1/MinFrameRateLimit */
        if(MaxAllowedGopStruct == ENCODING_GOP_STRUCT_IBBP)
        {
            /* Only support B structure GOP*/
            if(ITFPenable == 1)
            {
                if((InputType == ENCODER_INPUT_TYPE_PROGRESSIVE)&& (MinFrameRateLimit == ENCODING_FRAME_RATE_6000))
                {
                    /* scale by 5 */
                    reorderDelay = (PictureIntervalIn27MhzTicks << 2) + PictureIntervalIn27MhzTicks;
                }
                else
                {
                    /* scale by 3 */
                    reorderDelay = (PictureIntervalIn27MhzTicks << 1) + PictureIntervalIn27MhzTicks;
                }
            }
            else
            {
                /* scale by 2 */
                reorderDelay <<= 1;
            }
        }
        else
        {
            reorderDelay  = 0;
        }


        /* compute MBP delay in 27 MHZ ticks using RESOLUTION_T0_27MHZ_MBPDELAY = (27e6/30)*(1/(1920*1088) */
        FP_temp.mantissa = RESOLUTION_T0_27MHZ_MBPDELAY_MANT;
        FP_temp.exponent = RESOLUTION_T0_27MHZ_MBPDELAY_EXP;
        pixelsPerFrame = UMULT16(PictureWidthInPels, PictureHeightInPels);
        mbpCmeDelay = RcU32ScaleFP(FP_temp, pixelsPerFrame, 1);

        mbpdelay = mbpCmeDelay;

        if (InputType == ENCODER_INPUT_TYPE_INTERLACED)
        {
            if (PictureWidthInPels <= HORIZONTAL_SIZE_IN_PELS_HD)
            {
                /* CME delay = 2*alpha/30 for resolution less than HD */
                mbpCmeDelay = (mbpCmeDelay << 1);
            }
            if(MaxAllowedGopStruct == ENCODING_GOP_STRUCT_IBBP)
            {
                mbpCmeDelay = (mbpCmeDelay << 1);
            }
        }
        else if (InputType == ENCODER_INPUT_TYPE_PROGRESSIVE)
        {
            if(MaxAllowedGopStruct == ENCODING_GOP_STRUCT_I)
            {
                /* scale for cme according to gop struct */
                if (PictureWidthInPels <= HORIZONTAL_SIZE_IN_PELS_SD)
                {
                    if (MinFrameRateLimit == ENCODING_FRAME_RATE_3000)
                    {
                        /* CME delay = (2*alpha)/30 */
                        mbpCmeDelay = (mbpCmeDelay << 1);
                    }
                }
            }
            else if((MaxAllowedGopStruct == ENCODING_GOP_STRUCT_IBBP) || (MaxAllowedGopStruct == ENCODING_GOP_STRUCT_IP) || (MaxAllowedGopStruct == ENCODING_GOP_STRUCT_INFINITE_IP))
            {
                /* scale for cme according to gop struct */
                if (PictureWidthInPels <= HORIZONTAL_SIZE_IN_PELS_SD)
                {
                    if (MinFrameRateLimit == ENCODING_FRAME_RATE_3000)
                    {
                        /* CME delay = (4*alpha)/30 */
                        mbpCmeDelay = (mbpCmeDelay << 2);
                    }
                    else if (MinFrameRateLimit == ENCODING_FRAME_RATE_6000)
                    {
                        /* CME delay = (2*alpha)/30 */
                        mbpCmeDelay = (mbpCmeDelay << 1);
                    }
                }
                else
                {
                     if (MinFrameRateLimit == ENCODING_FRAME_RATE_3000)
                     {
                        /* CME delay = (2*alpha)/30 */
                        mbpCmeDelay = (mbpCmeDelay << 1);

                     }
                }
            }

            /* Add additional delay of 1/BVNframerate for IBBP Gop structure */
            if (MaxAllowedGopStruct == ENCODING_GOP_STRUCT_IBBP)
            {
                mbpCmeDelay += vipdelay;

                if (MinFrameRateLimit == ENCODING_FRAME_RATE_3000)
                {
                    mbpCmeDelay += vipdelay;
                }

            }

        }

        /* Add the MBP delay of alpha/30 */
        mbpCmeDelay += mbpdelay;

        /* accumulate total */
        mbpCmeDelay += vipItfpDelay + reorderDelay;
    }

    return(mbpCmeDelay);
}

/***********************************************************************
*  Compute_DeeIn27MhzTicks() - Compute max supported end to end delay
*  in units expressed as 27 MHz clock ticks
************************************************************************/
uint32_t
BVCE_FW_P_Compute_MaxDeeIn27MhzTicks(
    uint32_t    hrdBufferSize,
    FPNumber_t  FP_ticksPerBit
)
{
    uint32_t MaxSupportedEndToEndDelayIn27MHz;

    /* update Max delay to that which garantees no HRD buffer overflow for any encoder when  EndToEndDelay * Rate < HRDsize */
    MaxSupportedEndToEndDelayIn27MHz = RcU32ScaleFP(FP_ticksPerBit, hrdBufferSize, 1);

    return(MaxSupportedEndToEndDelayIn27MHz);
}

/**********************************************************
*  Compute_DeeIn27MhzTicks() - Compute end to end delay
*  in units expressed as 27 MHz clock ticks
**********************************************************/
uint32_t
BVCE_FW_P_Compute_DeeIn27MhzTicks(
    uint32_t        averageBitsPerPic,
    uint32_t        hrdBufferSize,
    FPNumber_t      FP_ticksPerBit,
    uint32_t        maxPictureIntervalIn27MhzTicks,
    uint32_t        *pMaxAllowedBitsPerPicture,
    uint8_t         MaxNumFNRT
)
{
    uint32_t delayEndToEndIn27MhzTicks, delayEndToEndTimesMaxRate;
    FPNumber_t      FP_inverse;

    /* Set endToEnd Delay = (SizeHRD - MAX(PicSize))/Rate for CBR */
    /* pMaxAllowedBitsPerPicture[0] must be less than HRD size */
    pMaxAllowedBitsPerPicture[0] = U32xU16MULTIPLY(averageBitsPerPic, BITS_PER_PIC2MAXPIC_MANTISSA);
    pMaxAllowedBitsPerPicture[0] = pMaxAllowedBitsPerPicture[0] >> BITS_PER_PIC2MAXPIC_EXPONENT;
    if((hrdBufferSize <= averageBitsPerPic) || (pMaxAllowedBitsPerPicture[0] >= (hrdBufferSize - averageBitsPerPic)))
    {
        /* Potentially too small HRD if sizeHRD <= MAX_INTER_PICTURE_INTERVAL * MAX_BIT_RATE */
        pMaxAllowedBitsPerPicture[0] = hrdBufferSize;
        delayEndToEndIn27MhzTicks = maxPictureIntervalIn27MhzTicks;
    }
    else
    {
        /* Dee*Rcbr = (Sizehrd - Sizemax) */
        delayEndToEndTimesMaxRate = (hrdBufferSize - pMaxAllowedBitsPerPicture[0]);

        /* saturate multiplication to 32 bits (which also limits delay to 2.65 minutes) */
        delayEndToEndIn27MhzTicks = RcU32ScaleFP(FP_ticksPerBit, delayEndToEndTimesMaxRate, 1);

        if(delayEndToEndIn27MhzTicks < maxPictureIntervalIn27MhzTicks)
        {
            delayEndToEndIn27MhzTicks = maxPictureIntervalIn27MhzTicks;
        }
    }

    /* Ensure reasonable delay when automatically computed */
    if(delayEndToEndIn27MhzTicks > MAX_DEE_IN_27_MHZ_TICKS)
    {
        delayEndToEndIn27MhzTicks = (uint32_t)MAX_DEE_IN_27_MHZ_TICKS;
    }
    
    /* Dee = delayEndToEndIn27MhzTicks / MaxNumFNRT */
    if (MaxNumFNRT > 1)
    {
        RcNumToFP(&FP_inverse, MaxNumFNRT, 15);

        FP_inverse.mantissa = DIVIDE16(NORMALIZE_ONE_TO_15_BITS_MANTISSA, FP_inverse.mantissa);
        FP_inverse.exponent = (NORMALIZE_ONE_TO_15_BITS_EXPONENT - FP_inverse.exponent + 15);

        delayEndToEndIn27MhzTicks = RcU32ScaleFP(FP_inverse, delayEndToEndIn27MhzTicks, 0);
    }
        
    return(delayEndToEndIn27MhzTicks);
}

/**********************************************************
*  ComputeRateInTicksPerBit() - Converts frame rate in bps
*  to bits per ticks based on 27 MHz clock cycles
**********************************************************/
void
BVCE_FW_P_ComputeRateInTicksPerBit(
    FPNumber_t      *pt_FP_ticksPerBit,
    uint32_t            bitrateBPS
)
{
    FPNumber_t FP_inverseRate;

    RcNumToFP(&FP_inverseRate, bitrateBPS, 15);
    FP_inverseRate.mantissa = DIVIDE16(NORMALIZE_ONE_TO_15_BITS_MANTISSA, FP_inverseRate.mantissa);
    FP_inverseRate.exponent = (NORMALIZE_ONE_TO_15_BITS_EXPONENT - FP_inverseRate.exponent + 15);

    /*Can treat multiply below as 32bit constant times positive signed Q8.8 number and it will not overflow nor
     lose any significant bits given that ultimately want full 16 bit mantissa for CABAC interface. */
    pt_FP_ticksPerBit->mantissa = U32xPOSQ8MULTIPLY(SYSTEM_CLOCK_27_MHZ, FP_inverseRate.mantissa);
    pt_FP_ticksPerBit->exponent = FP_inverseRate.exponent - 8;
}

/********************************************************
*  ConvertFrameRate() - Converts frame rate and
*   computes avg bits per frame.
*
********************************************************/
uint32_t
BVCE_FW_P_ConvertFrameRate(
    FrameRate_e     framesPerSecond,
    FPNumber_t      *FP_inverse
)
{
    uint32_t   PictureIntervalIn27MhzTicks = 0;

    switch(framesPerSecond)
    {
    /* stored as 100xFrameRate */
    case ENCODING_FRAME_RATE_6000:
        /* need picture interval in 27 MHZ ticks */
        /* need picture interval in seconds ==> 1/FrameRate */
        PictureIntervalIn27MhzTicks = INVERSE_FRAMERATE_6000_IN_27MHZ_TICKS;
        FP_inverse->mantissa = INVERSE_FRAMERATE_6000_IN_SECONDS_MANTISSA;
        FP_inverse->exponent = INVERSE_FRAMERATE_6000_IN_SECONDS_EXPONENT;
        break;
    case ENCODING_FRAME_RATE_5994:
        PictureIntervalIn27MhzTicks = INVERSE_FRAMERATE_5994_IN_27MHZ_TICKS;
        FP_inverse->mantissa = INVERSE_FRAMERATE_5994_IN_SECONDS_MANTISSA;
        FP_inverse->exponent = INVERSE_FRAMERATE_5994_IN_SECONDS_EXPONENT;
        break;
    case ENCODING_FRAME_RATE_5000:
        PictureIntervalIn27MhzTicks = INVERSE_FRAMERATE_5000_IN_27MHZ_TICKS;
        FP_inverse->mantissa = INVERSE_FRAMERATE_5000_IN_SECONDS_MANTISSA;
        FP_inverse->exponent = INVERSE_FRAMERATE_5000_IN_SECONDS_EXPONENT;
        break;
    case ENCODING_FRAME_RATE_3000:
        PictureIntervalIn27MhzTicks = INVERSE_FRAMERATE_3000_IN_27MHZ_TICKS;
        FP_inverse->mantissa = INVERSE_FRAMERATE_3000_IN_SECONDS_MANTISSA;
        FP_inverse->exponent = INVERSE_FRAMERATE_3000_IN_SECONDS_EXPONENT;
        break;
    case ENCODING_FRAME_RATE_2997:
        PictureIntervalIn27MhzTicks = INVERSE_FRAMERATE_2997_IN_27MHZ_TICKS;
        FP_inverse->mantissa = INVERSE_FRAMERATE_2997_IN_SECONDS_MANTISSA;
        FP_inverse->exponent = INVERSE_FRAMERATE_2997_IN_SECONDS_EXPONENT;
        break;
    case ENCODING_FRAME_RATE_2500:
        PictureIntervalIn27MhzTicks = INVERSE_FRAMERATE_2500_IN_27MHZ_TICKS;
        FP_inverse->mantissa = INVERSE_FRAMERATE_2500_IN_SECONDS_MANTISSA;
        FP_inverse->exponent = INVERSE_FRAMERATE_2500_IN_SECONDS_EXPONENT;
        break;
    case ENCODING_FRAME_RATE_2400:
        PictureIntervalIn27MhzTicks = INVERSE_FRAMERATE_2400_IN_27MHZ_TICKS;
        FP_inverse->mantissa = INVERSE_FRAMERATE_2400_IN_SECONDS_MANTISSA;
        FP_inverse->exponent = INVERSE_FRAMERATE_2400_IN_SECONDS_EXPONENT;
        break;
    case ENCODING_FRAME_RATE_2397/*.6*/:
        PictureIntervalIn27MhzTicks = INVERSE_FRAMERATE_2397_IN_27MHZ_TICKS;
        FP_inverse->mantissa = INVERSE_FRAMERATE_2397_IN_SECONDS_MANTISSA;
        FP_inverse->exponent = INVERSE_FRAMERATE_2397_IN_SECONDS_EXPONENT;
        break;
    case ENCODING_FRAME_RATE_2000:
        PictureIntervalIn27MhzTicks = INVERSE_FRAMERATE_2000_IN_27MHZ_TICKS;
        FP_inverse->mantissa = INVERSE_FRAMERATE_2000_IN_SECONDS_MANTISSA;
        FP_inverse->exponent = INVERSE_FRAMERATE_2000_IN_SECONDS_EXPONENT;
        break;
    case ENCODING_FRAME_RATE_1500:
        PictureIntervalIn27MhzTicks = INVERSE_FRAMERATE_1500_IN_27MHZ_TICKS;
        FP_inverse->mantissa = INVERSE_FRAMERATE_1500_IN_SECONDS_MANTISSA;
        FP_inverse->exponent = INVERSE_FRAMERATE_1500_IN_SECONDS_EXPONENT;
        break;
    case ENCODING_FRAME_RATE_1498:
        PictureIntervalIn27MhzTicks = INVERSE_FRAMERATE_1498_IN_27MHZ_TICKS;
        FP_inverse->mantissa = INVERSE_FRAMERATE_1498_IN_SECONDS_MANTISSA;
        FP_inverse->exponent = INVERSE_FRAMERATE_1498_IN_SECONDS_EXPONENT;
        break;
    default:
#ifdef BCHP_CHIP
        BDBG_ASSERT(!"unsupported frame rate");
        return 0;
#else
#if defined(CEB2APP) || defined (NGE_FW_SIM_APP)
        assert(!"unsupported frame rate");
#else
        ASSERT(!"unsupported frame rate");
#endif
        break;
#endif
    }

    return(PictureIntervalIn27MhzTicks);
}


/********************************************************
*  CalcHRDbufferSize() - Determines HRD buffer size
*    to use.
********************************************************/
uint32_t
BVCE_FW_P_CalcHRDbufferSize(
        uint32_t            Protocol,       /* encoding standard    */
        uint32_t            Profile,        /* encoding profile     */
        EncoderLevel_e      Level           /* Profile Level        */
)
{
    uint32_t   hrdBufferSize;

    /* compute size based on encoding standard level and profile */
    switch(Protocol)
    {
    case ENCODING_STD_H264:
        switch(Level)
        {
        case ENCODING_AVC_LEVEL_10: hrdBufferSize = H264_MAX_CPB_LEVEL_10; break;
        case ENCODING_AVC_LEVEL_11: hrdBufferSize = H264_MAX_CPB_LEVEL_11; break;
        case ENCODING_AVC_LEVEL_12: hrdBufferSize = H264_MAX_CPB_LEVEL_12; break;
        case ENCODING_AVC_LEVEL_13: hrdBufferSize = H264_MAX_CPB_LEVEL_13; break;
        case ENCODING_AVC_LEVEL_20: hrdBufferSize = H264_MAX_CPB_LEVEL_20; break;
        case ENCODING_AVC_LEVEL_21: hrdBufferSize = H264_MAX_CPB_LEVEL_21; break;
        case ENCODING_AVC_LEVEL_22: hrdBufferSize = H264_MAX_CPB_LEVEL_22; break;
        case ENCODING_AVC_LEVEL_30: hrdBufferSize = H264_MAX_CPB_LEVEL_30; break;
        case ENCODING_AVC_LEVEL_31: hrdBufferSize = H264_MAX_CPB_LEVEL_31; break;
        case ENCODING_AVC_LEVEL_32: hrdBufferSize = H264_MAX_CPB_LEVEL_32; break;
        case ENCODING_AVC_LEVEL_40: hrdBufferSize = H264_MAX_CPB_LEVEL_40; break;
        default: hrdBufferSize = 0; /*error*/
        }
        break;

    case ENCODING_STD_MPEG2:
        switch(Level)
        {
        case ENCODING_MPEG2_LEVEL_LOW:  hrdBufferSize = MPEG2_MAX_VBV_LEVEL_LOW; break;
        case ENCODING_MPEG2_LEVEL_MAIN: hrdBufferSize = MPEG2_MAX_VBV_LEVEL_MAIN; break;
        case ENCODING_MPEG2_LEVEL_HIGH: hrdBufferSize = MPEG2_MAX_VBV_LEVEL_HIGH; break;
        default: hrdBufferSize = 0; /*error*/
        }
        break;

    case ENCODING_STD_MPEG4:
        switch(Profile)
        {
        case ENCODING_MPEG4_PROFILE_SIMPLE:
            switch(Level)
            {
            case ENCODING_MPEG4_LEVEL_1: hrdBufferSize = MPEG4_MAX_VBV_SP_LEVEL_1; break;
            case ENCODING_MPEG4_LEVEL_2: hrdBufferSize = MPEG4_MAX_VBV_SP_LEVEL_2; break;
            case ENCODING_MPEG4_LEVEL_3: hrdBufferSize = MPEG4_MAX_VBV_SP_LEVEL_3; break;
            default: hrdBufferSize = 0; /*error*/
            }
            break;

        case ENCODING_MPEG4_PROFILE_ADVANCED_SIMPLE:
            switch(Level)
            {
            case ENCODING_MPEG4_LEVEL_1: hrdBufferSize = MPEG4_MAX_VBV_ASP_LEVEL_1; break;
            case ENCODING_MPEG4_LEVEL_2: hrdBufferSize = MPEG4_MAX_VBV_ASP_LEVEL_2; break;
            case ENCODING_MPEG4_LEVEL_3: hrdBufferSize = MPEG4_MAX_VBV_ASP_LEVEL_3; break;
            case ENCODING_MPEG4_LEVEL_4: hrdBufferSize = MPEG4_MAX_VBV_ASP_LEVEL_4; break;
            case ENCODING_MPEG4_LEVEL_5: hrdBufferSize = MPEG4_MAX_VBV_ASP_LEVEL_5; break;
            default: hrdBufferSize = 0; /*error*/
            }
            break;
        default:
            hrdBufferSize = 0; /*error*/
            break;
        }
        break;

    case ENCODING_STD_VP8:
        hrdBufferSize = VP8_DUMMY_HRD_BUFFER_SIZE; 
        break;
        

    default:
        hrdBufferSize = 0; /*error*/
        break;
    }

    if(hrdBufferSize == 0)
    {
#ifdef BCHP_CHIP
        BDBG_ASSERT(!"Unsupported Profile or Level");
#else
#if defined(CEB2APP) || defined (NGE_FW_SIM_APP)
        assert(!"Unsupported Profile or Level");
#else
        ASSERT(!"Unsupported Profile or Level");
#endif
#endif
    }

    return(hrdBufferSize);
}




#if defined(BCHP_CHIP) || defined(CEB2APP) || defined (NGE_FW_SIM_APP)

/***************************************************************************************************************************
*     FLOATING POINT Functions
****************************************************************************************************************************/

/****************************************************************
*  FPNumber_t RcNumToFP(uint32_t Number, uint8_t Bits)
*   convert integer Number to floating point representation with
*   "Bits" number of bits for mantissa
*****************************************************************/
static void RcNumToFP(FPNumber_t *outputFPNumber, uint32_t inputNumber, uint8_t numBits)
{

    outputFPNumber->mantissa=inputNumber;

    if(((int32_t)inputNumber) > 0)
    {
        NORM31(outputFPNumber->exponent, (int32_t) inputNumber);
    }
    else
    {
        outputFPNumber->exponent = -1;
    }

    if(numBits<= 32)
    {
        outputFPNumber->exponent -= (31 - numBits);
    }

    if(outputFPNumber->exponent < 0)
    {
        outputFPNumber->mantissa >>= (-outputFPNumber->exponent);
    }
    else
    {
        outputFPNumber->mantissa <<= outputFPNumber->exponent;
    }
}

/****************************************************************
*  RcFPToNum(FPNumber_t localFPNumber)
*   Round floating point number to 32 bit integer number
*****************************************************************/
static uint32_t RcFPToNum(FPNumber_t localFPNumber)
{
    uint32_t Number;

    if(localFPNumber.exponent > 0)
    {
        Number = localFPNumber.mantissa  >>  localFPNumber.exponent;
    }
    else
    {
        Number = localFPNumber.mantissa << (-localFPNumber.exponent);
    }
    return Number;
}

/**********************************************************************************
*  RcU32ScaleFP(FPNumber_t FP_operand, uint32_t scaleFactor, uint8_t bypassNormFlag)
**********************************************************************************/
static uint32_t RcU32ScaleFP(FPNumber_t FP_operand, uint32_t scaleFactor, uint8_t bypassNormFlag)
{
        uint32_t scaledMSW, scaledLSW, product;
        FPNumber_t FP_short;
    int8_t maxLeftShiftAllowedMinusOne;

    if(bypassNormFlag)
    {
        FP_short.mantissa = FP_operand.mantissa;
            FP_short.exponent = FP_operand.exponent;
    }
    else
    {
        RcNumToFP(&FP_short, FP_operand.mantissa, 16);
            FP_short.exponent += FP_operand.exponent;
    }

    /* multiply 16-bit mantissa with  32 bit unsigned integer */
    scaledMSW = UMULT16(scaleFactor>>16, FP_short.mantissa);
    scaledLSW = UMULT16(scaleFactor, FP_short.mantissa);
    if(FP_short.exponent < 16)
    {
        /* net left shift for MSW with unsigned 32-bit saturation */
        switch(scaledMSW & 0x80000000)
        {
        case 0:
            NORM31(maxLeftShiftAllowedMinusOne, (int32_t) scaledMSW);
            if(maxLeftShiftAllowedMinusOne >= ((16 - FP_short.exponent) - 1))
            {
                scaledMSW <<= (16 - FP_short.exponent);
                product = scaledMSW + (scaledLSW >> FP_short.exponent);

                if(product  >= scaledMSW)
                {
                    break;  /* no overflow */
                }
            }
        default:
            /* overflow */
            /* BitsPErTick  > 1 at bit rates above 27Mbps */
            product = UINT32_MAX;
            break;
        }
    }
    else
    {
        /* net (exponent - 16) right shift for MSW */
        product = (scaledMSW + (scaledLSW >> 16)) >> (FP_short.exponent - 16);
    }

        return(product);
}
/**********************************************************************************
*  NormalizeFP(FPNumber_t *outputFPNumber, FPNumber_t inputFPNumber, u8_t numBits)
*    Normalize any nput  Floating point Number to numBits for manitssa
**********************************************************************************/
static void NormalizeFP(FPNumber_t *pOutputFPNumber, FPNumber_t pInputFPNumber, uint8_t numBits)
{
    /* shift the number accordingly */
    RcNumToFP(pOutputFPNumber, pInputFPNumber.mantissa, numBits);
    pOutputFPNumber->exponent += pInputFPNumber.exponent;
}

/****************************************************************
* Utility to simulate hardware function used for rate control
*****************************************************************/
static int16_t arcDivide16(int16_t numerator, int16_t denominator)
{
    int16_t iteration, var_out;
    int32_t Lm, L_denom;


    var_out =  0;
    Lm = numerator << 15;

    L_denom = denominator << 15;

    for(iteration=0; iteration < 15; iteration++)
    {
        Lm <<= 1;
        if(Lm >= L_denom)
        {
            /*32 bit subtract*/
            Lm = Lm - L_denom;
            Lm = Lm + 1;
        }
    }

    /* remainder in MSW of Lm, quotient in LSW of Lm */
    var_out = (int16_t)(Lm & 0x7fff);
    return(var_out);
}




/************************************************************************
* Function: bvceEpmCalcWidthInStripes
*
* Actions: find number of stripes in a picture
*
* Params:
*        PictureWidthInPels: picture width in pels
*
* Returns:
*        Number of stripes in a picture width
*
************************************************************************/
uint8_t bvceEpmCalcWidthInStripes(uint32_t PictureWidthInPels,uint16_t DRAM_STRIPE_WIDTH)
{
    uint8_t WidthInStripes;

    if (DRAM_STRIPE_WIDTH==128)
    {
        WidthInStripes=(PictureWidthInPels>>7);
        if ((PictureWidthInPels-(WidthInStripes<<7))>0)
            WidthInStripes++;
    }
    else
    {
        WidthInStripes=(PictureWidthInPels>>6);
        if ((PictureWidthInPels-(WidthInStripes<<6))>0)
            WidthInStripes++;
    }

    return(WidthInStripes);

}


/************************************************************************
* Function: bvceEpmAlignJword
*
* Actions: align a number to Jword(unit of 256bits).
*
* Params:
*        UnalignedNumber
*
* Returns:
*        Aligned number
*
************************************************************************/
uint32_t bvceEpmAlignJword(uint32_t UnalignedNumber)
{
    uint32_t AlignedNumber;
    AlignedNumber=(UnalignedNumber+31) & 0xffffffe0;
    return(AlignedNumber);
}



/************************************************************************
* Function: bvceEpmCalcNMBY
*
* Actions: find the number of macroblocs in stripe height
*
* Params:
*        PictureHeightInMbs: hight of picture in macroblock units
*
* Returns:
*        Number of macroblocks per stripe height
*
************************************************************************/
uint8_t bvceEpmCalcNMBY(uint32_t PictureHeightInMbs)
{

    uint8_t nmby,n;

    #define DRAM_VERT_MBS_PER_PAGE 2
    for(n=0; n<18; ++n)  /* Supports frame sizes up to 1088, need to increase for bigger sizes */
    {/* pay attention is was only > due to a bug from the architecture cmodel */
        if((4*(uint32_t)n+2)*DRAM_VERT_MBS_PER_PAGE >= PictureHeightInMbs)
            break;
    }

    nmby = (4*n+2) * DRAM_VERT_MBS_PER_PAGE;
    return(nmby);


}


/* ==============   MEMORY CALCULATIONS   ================== */


#define EPM_IBBP_NUMBER_OF_ORIGINAL_PICTURE_QUEUES                      (5+1)
#define EPM_IBBP_NUMBER_OF_DECIMATED_PICTURE_QUEUES                     (8+1)
#define EPM_IBBP_NUMBER_OF_CSC_QUEUES                                   (3)
#define EPM_IBBP_NUMBER_OF_RECONSTRUCTED_PICTURE_QUEUES                 (5)

#define EPM_IP_NUMBER_OF_ORIGINAL_PICTURE_QUEUES_PROGRESSIVE            (4)
#define EPM_IP_NUMBER_OF_DECIMATED_PICTURE_QUEUES_PROGRESSIVE           (5)
#define EPM_IP_NUMBER_OF_CSC_QUEUES_PROGRESSIVE                         (3)
#define EPM_IP_NUMBER_OF_RECONSTRUCTED_PICTURE_QUEUES_PROGRESSIVE       (3)

#define EPM_IP_NUMBER_OF_ORIGINAL_PICTURE_QUEUES_INTERLACE              (4)
#define EPM_IP_NUMBER_OF_DECIMATED_PICTURE_QUEUES_INTERLACE             (4)
#define EPM_IP_NUMBER_OF_CSC_QUEUES_INTERLACE                           (3)
#define EPM_IP_NUMBER_OF_RECONSTRUCTED_PICTURE_QUEUES_INTERLACE         (3)

#define EPM_MAX_NUMBER_OF_ORIGINAL_PICTURE_QUEUES                       EPM_IBBP_NUMBER_OF_ORIGINAL_PICTURE_QUEUES
#define EPM_MAX_NUMBER_OF_DECIMATED_PICTURE_QUEUES                      EPM_IBBP_NUMBER_OF_DECIMATED_PICTURE_QUEUES
#define EPM_MAX_NUMBER_OF_CSC_QUEUES                                    EPM_IBBP_NUMBER_OF_CSC_QUEUES
#define EPM_MAX_NUMBER_OF_RECONSTRUCTED_PICTURE_QUEUES                  EPM_IBBP_NUMBER_OF_RECONSTRUCTED_PICTURE_QUEUES

#define EPM_MAX_NUMBER_OF_IBBP_ORIGINAL_PICTURE_QUEUES                  EPM_IBBP_NUMBER_OF_ORIGINAL_PICTURE_QUEUES
#define EPM_MAX_NUMBER_OF_IBBP_DECIMATED_PICTURE_QUEUES                 EPM_IBBP_NUMBER_OF_DECIMATED_PICTURE_QUEUES
#define EPM_MAX_NUMBER_OF_IBBP_CSC_QUEUES                               EPM_IBBP_NUMBER_OF_CSC_QUEUES
#define EPM_MAX_NUMBER_OF_IBBP_RECONSTRUCTED_PICTURE_QUEUES             EPM_IBBP_NUMBER_OF_RECONSTRUCTED_PICTURE_QUEUES

#define EPM_MAX_NUMBER_OF_IP_ORIGINAL_PICTURE_QUEUES                    EPM_IP_NUMBER_OF_ORIGINAL_PICTURE_QUEUES_PROGRESSIVE
#define EPM_MAX_NUMBER_OF_IP_DECIMATED_PICTURE_QUEUES                   EPM_IP_NUMBER_OF_DECIMATED_PICTURE_QUEUES_PROGRESSIVE
#define EPM_MAX_NUMBER_OF_IP_CSC_QUEUES                                 EPM_IP_NUMBER_OF_CSC_QUEUES_PROGRESSIVE
#define EPM_MAX_NUMBER_OF_IP_RECONSTRUCTED_PICTURE_QUEUES               EPM_IP_NUMBER_OF_RECONSTRUCTED_PICTURE_QUEUES_PROGRESSIVE

#define EPM_NUMBER_OF_ORIGINAL_PICTURE_DESCRIPTORS                      (EPM_MAX_NUMBER_OF_IBBP_ORIGINAL_PICTURE_QUEUES*2)

#define HOR_SIZE_IN_PELS_720P                                           1280
#define VER_SIZE_IN_PELS_720P                                           720

#define HOR_SIZE_IN_PELS_PAL                                            720
#define VER_SIZE_IN_PELS_PAL                                            576


/************************************************************************
* Function: BVCE_FW_P_CalcNonSecureMemory
*
* Calculates the amount of non-secure memory that is necessary for ViCE2 FW
* to operate depending on various input parameters.
*
* Params:
*       InputType               - Input Type: ENCODER_INPUT_TYPE_INTERLACED or ENCODER_INPUT_TYPE_PROGRESSIVE
*       DRAM_STRIPE_WIDTH_Enum  - Stripe Width: E_STRIPE_WIDTH_128 or E_STRIPE_WIDTH_64
*       MaxPictureWidthInPels   - Max picture H resolution allowed
*       MaxPictureHeightInPels  - Max picture V resolution allowed
*
* Returns:
*        Number of macroblocks per stripe height
*
************************************************************************/

uint32_t BVCE_FW_P_CalcNonSecureMemory (
                             uint8_t             InputType,
                             uint8_t             DRAM_STRIPE_WIDTH_Enum,
                             uint32_t            MaxPictureWidthInPels,
                             uint32_t            MaxPictureHeightInPels
                                )
{
    uint8_t DRAM_STRIPE_WIDTH;
    uint8_t  StackPointer;
    uint32_t BufferSize;
    uint32_t CurrAddress;

    uint32_t max_horizontal_size_in_pels,max_horizontal_size_in_mbs;
    uint32_t max_vertical_size_in_pels,max_vertical_size_in_mbs;

    uint32_t size_of_luma_buffer;
    uint32_t size_of_420_chroma_buffer;
    uint32_t size_of_shifted_422_chroma_buffer;

    uint32_t size_of_2h1v_luma_buffer;
    uint32_t size_of_2h2v_luma_buffer;

    uint32_t size_of_csc_buffer;

    uint32_t size_of_reconstructed_luma;
    uint32_t size_of_reconstructed_chroma;

    uint16_t NmbyLuma,NmbyChroma;

    uint8_t EPM_NUMBER_OF_ORIGINAL_PICTURE_QUEUES;
    uint8_t EPM_NUMBER_OF_DECIMATED_PICTURE_QUEUES;
    uint8_t EPM_NUMBER_OF_CSC_QUEUES;
    uint8_t EPM_NUMBER_OF_RECONSTRUCTED_PICTURE_QUEUES;


    if (DRAM_STRIPE_WIDTH_Enum==E_STRIPE_WIDTH_128) {
        DRAM_STRIPE_WIDTH = 128;
    }
    else {
        DRAM_STRIPE_WIDTH = 64;
    }


    /* for progressive */

    /* Initialize EPM stack of buffers */
    CurrAddress = 0;

    if( InputType == ENCODER_INPUT_TYPE_PROGRESSIVE )
    {
        max_horizontal_size_in_pels = MaxPictureWidthInPels;
        max_vertical_size_in_pels   = MaxPictureHeightInPels;

        EPM_NUMBER_OF_ORIGINAL_PICTURE_QUEUES=EPM_MAX_NUMBER_OF_IP_ORIGINAL_PICTURE_QUEUES;
        EPM_NUMBER_OF_DECIMATED_PICTURE_QUEUES=EPM_MAX_NUMBER_OF_IP_DECIMATED_PICTURE_QUEUES;
        EPM_NUMBER_OF_CSC_QUEUES=EPM_MAX_NUMBER_OF_IP_CSC_QUEUES;
        EPM_NUMBER_OF_RECONSTRUCTED_PICTURE_QUEUES=EPM_MAX_NUMBER_OF_IP_RECONSTRUCTED_PICTURE_QUEUES;

        max_horizontal_size_in_mbs=max_horizontal_size_in_pels>>4;
        max_vertical_size_in_mbs=max_vertical_size_in_pels>>4;

        NmbyLuma=bvceEpmCalcNMBY(max_vertical_size_in_mbs);
        NmbyChroma=bvceEpmCalcNMBY((max_vertical_size_in_mbs + 1) >> 1);

        size_of_luma_buffer=bvceEpmAlignJword(bvceEpmCalcWidthInStripes(max_horizontal_size_in_pels,DRAM_STRIPE_WIDTH)*DRAM_STRIPE_WIDTH*(NmbyLuma<<4));
        size_of_luma_buffer = ((size_of_luma_buffer+1023) & 0xfffffc00);
        size_of_420_chroma_buffer=bvceEpmAlignJword(bvceEpmCalcWidthInStripes(max_horizontal_size_in_pels,DRAM_STRIPE_WIDTH)*DRAM_STRIPE_WIDTH*(NmbyChroma<<4));
        size_of_420_chroma_buffer = ((size_of_420_chroma_buffer+1023) & 0xfffffc00);
        size_of_shifted_422_chroma_buffer=size_of_420_chroma_buffer;

        size_of_2h1v_luma_buffer=bvceEpmAlignJword(bvceEpmCalcWidthInStripes(max_horizontal_size_in_pels>>1,DRAM_STRIPE_WIDTH)*DRAM_STRIPE_WIDTH*(NmbyLuma<<4));
        size_of_2h1v_luma_buffer = ((size_of_2h1v_luma_buffer+1023) & 0xfffffc00);
        size_of_2h2v_luma_buffer=bvceEpmAlignJword(bvceEpmCalcWidthInStripes(max_horizontal_size_in_pels>>1,DRAM_STRIPE_WIDTH)*DRAM_STRIPE_WIDTH*(NmbyChroma<<4));
        size_of_2h2v_luma_buffer = ((size_of_2h2v_luma_buffer+1023) & 0xfffffc00);

        size_of_csc_buffer=bvceEpmAlignJword(6*max_horizontal_size_in_mbs*max_vertical_size_in_mbs);
        size_of_csc_buffer = ((size_of_csc_buffer+1023) & 0xfffffc00);

        size_of_reconstructed_luma = ((size_of_luma_buffer+1023) & 0xfffffc00);/* TODO align according to page size */
        size_of_reconstructed_chroma=size_of_420_chroma_buffer;

        /* Allocate Stack of buffers for the Original picture */
        BufferSize=size_of_luma_buffer+size_of_420_chroma_buffer;

        /*save start address and buffer size for on the fly address calculation */
        CurrAddress = ((CurrAddress+1023) & 0xfffffc00); /*TODO align according to page size */

        for (StackPointer=0; StackPointer<EPM_NUMBER_OF_ORIGINAL_PICTURE_QUEUES ; StackPointer++)
        {
            CurrAddress = ((CurrAddress+1023) & 0xfffffc00); /*TODO align according to page size*/
            CurrAddress=CurrAddress+BufferSize;
        }

        /*Allocate stack of buffers for the Decimated picture*/
        BufferSize=size_of_2h1v_luma_buffer+size_of_2h2v_luma_buffer;

        /*save start address and buffer size for on the fly address calculation*/
        CurrAddress = ((CurrAddress+1023) & 0xfffffc00); /*TODO align according to page size*/

        for (StackPointer=0; StackPointer<EPM_NUMBER_OF_DECIMATED_PICTURE_QUEUES ; StackPointer++)
        {
            CurrAddress = ((CurrAddress+1023) & 0xfffffc00); /*TODO align according to page size*/
            CurrAddress=CurrAddress+BufferSize;
        }

        /*Allocated stack of buffers for the CSC*/
        BufferSize=size_of_csc_buffer*4;/*In progressive mode there are only two references*/

        /*save start address and buffer size for on the fly address calculation*/
        CurrAddress = ((CurrAddress+1023) & 0xfffffc00); /*TODO align according to page size*/

        for (StackPointer=0; StackPointer<EPM_NUMBER_OF_CSC_QUEUES ; StackPointer++)
        {
            CurrAddress = ((CurrAddress+1023) & 0xfffffc00); /*TODO align according to page size*/
            CurrAddress=CurrAddress+BufferSize;
        }

        /*Allocate stack of buffers for the Reconstructed*/
        BufferSize=size_of_reconstructed_luma+size_of_reconstructed_chroma;

        /*save start address and buffer size for on the fly address calculation*/
        CurrAddress = ((CurrAddress+1023) & 0xfffffc00); /*TODO align according to page size*/

        for (StackPointer=0; StackPointer<EPM_NUMBER_OF_RECONSTRUCTED_PICTURE_QUEUES ; StackPointer++)
        {
            CurrAddress = ((CurrAddress+1023) & 0xfffffc00); /*TODO align according to page size*/
            CurrAddress=CurrAddress+BufferSize;
        }

    /*allocate 720p buffers*/

        /*max_horizontal_size_in_pels = MIN(HOR_SIZE_IN_PELS_720P,MaxPictureWidthInPels);*/
        max_horizontal_size_in_pels=HOR_SIZE_IN_PELS_720P;
        if (max_horizontal_size_in_pels>MaxPictureWidthInPels)
            {
                max_horizontal_size_in_pels=MaxPictureWidthInPels;
            }

        /*max_vertical_size_in_pels   = MIN(VER_SIZE_IN_PELS_720P,MaxPictureHeightInPels);*/
        max_vertical_size_in_pels=VER_SIZE_IN_PELS_720P;
        if (max_vertical_size_in_pels>MaxPictureHeightInPels)
        {
            max_vertical_size_in_pels=MaxPictureHeightInPels;
        }


        EPM_NUMBER_OF_ORIGINAL_PICTURE_QUEUES=EPM_MAX_NUMBER_OF_IBBP_ORIGINAL_PICTURE_QUEUES-EPM_MAX_NUMBER_OF_IP_ORIGINAL_PICTURE_QUEUES;
        EPM_NUMBER_OF_DECIMATED_PICTURE_QUEUES=EPM_MAX_NUMBER_OF_IBBP_DECIMATED_PICTURE_QUEUES-EPM_MAX_NUMBER_OF_IP_DECIMATED_PICTURE_QUEUES;



        max_horizontal_size_in_mbs=max_horizontal_size_in_pels>>4;
        max_vertical_size_in_mbs=max_vertical_size_in_pels>>4;

        NmbyLuma=bvceEpmCalcNMBY(max_vertical_size_in_mbs);
        NmbyChroma=bvceEpmCalcNMBY((max_vertical_size_in_mbs + 1) >> 1);


        size_of_luma_buffer=bvceEpmAlignJword(bvceEpmCalcWidthInStripes(max_horizontal_size_in_pels,DRAM_STRIPE_WIDTH)*DRAM_STRIPE_WIDTH*(NmbyLuma<<4));
        size_of_luma_buffer = ((size_of_luma_buffer+1023) & 0xfffffc00);
        size_of_420_chroma_buffer=bvceEpmAlignJword(bvceEpmCalcWidthInStripes(max_horizontal_size_in_pels,DRAM_STRIPE_WIDTH)*DRAM_STRIPE_WIDTH*(NmbyChroma<<4));
        size_of_420_chroma_buffer = ((size_of_420_chroma_buffer+1023) & 0xfffffc00);
        size_of_shifted_422_chroma_buffer=size_of_420_chroma_buffer;

        size_of_2h1v_luma_buffer=bvceEpmAlignJword(bvceEpmCalcWidthInStripes(max_horizontal_size_in_pels>>1,DRAM_STRIPE_WIDTH)*DRAM_STRIPE_WIDTH*(NmbyLuma<<4));
        size_of_2h1v_luma_buffer = ((size_of_2h1v_luma_buffer+1023) & 0xfffffc00);
        size_of_2h2v_luma_buffer=bvceEpmAlignJword(bvceEpmCalcWidthInStripes(max_horizontal_size_in_pels>>1,DRAM_STRIPE_WIDTH)*DRAM_STRIPE_WIDTH*(NmbyChroma<<4));
        size_of_2h2v_luma_buffer = ((size_of_2h2v_luma_buffer+1023) & 0xfffffc00);

        size_of_csc_buffer=bvceEpmAlignJword(6*max_horizontal_size_in_mbs*max_vertical_size_in_mbs);
        size_of_csc_buffer = ((size_of_csc_buffer+1023) & 0xfffffc00);

        size_of_reconstructed_luma = ((size_of_luma_buffer+1023) & 0xfffffc00);/*TODO align according to page size*/
        size_of_reconstructed_chroma=size_of_420_chroma_buffer;

        /*Allocate Stack of buffers for the Original picture*/
        BufferSize=size_of_luma_buffer+size_of_420_chroma_buffer;

        /*save start address and buffer size for on the fly address calculation*/
        CurrAddress = ((CurrAddress+1023) & 0xfffffc00); /*TODO align according to page size*/

        for (StackPointer=0; StackPointer<EPM_NUMBER_OF_ORIGINAL_PICTURE_QUEUES ; StackPointer++)
        {
            CurrAddress = ((CurrAddress+1023) & 0xfffffc00); /*TODO align according to page size*/
            CurrAddress=CurrAddress+BufferSize;
        }

        /*Allocate stack of buffers for the Decimated picture*/
        BufferSize=size_of_2h1v_luma_buffer+size_of_2h2v_luma_buffer;

        /*save start address and buffer size for on the fly address calculation*/
        CurrAddress = ((CurrAddress+1023) & 0xfffffc00); /*TODO align according to page size*/

        for (StackPointer=0; StackPointer<EPM_NUMBER_OF_DECIMATED_PICTURE_QUEUES ; StackPointer++)
        {
            CurrAddress = ((CurrAddress+1023) & 0xfffffc00); /*TODO align according to page size*/
            CurrAddress=CurrAddress+BufferSize;
        }

        return(CurrAddress);
    }

    /*Interlace*/
    else
    {

        /*Initialize EPM stack of buffers*/
        CurrAddress=0;

        /*fill HD buffers information*/
        max_horizontal_size_in_pels = MaxPictureWidthInPels;
        max_vertical_size_in_pels   = MaxPictureHeightInPels;


        EPM_NUMBER_OF_ORIGINAL_PICTURE_QUEUES=EPM_MAX_NUMBER_OF_IP_ORIGINAL_PICTURE_QUEUES;
        EPM_NUMBER_OF_DECIMATED_PICTURE_QUEUES=EPM_MAX_NUMBER_OF_IP_DECIMATED_PICTURE_QUEUES;
        EPM_NUMBER_OF_CSC_QUEUES=EPM_MAX_NUMBER_OF_IP_CSC_QUEUES;
        EPM_NUMBER_OF_RECONSTRUCTED_PICTURE_QUEUES=EPM_MAX_NUMBER_OF_IP_RECONSTRUCTED_PICTURE_QUEUES;

        max_horizontal_size_in_mbs=max_horizontal_size_in_pels>>4;
        max_vertical_size_in_mbs=max_vertical_size_in_pels>>4;

        NmbyLuma=bvceEpmCalcNMBY(max_vertical_size_in_mbs);
        NmbyChroma=bvceEpmCalcNMBY((max_vertical_size_in_mbs + 1) >> 1);


        size_of_luma_buffer=bvceEpmAlignJword(bvceEpmCalcWidthInStripes(max_horizontal_size_in_pels,DRAM_STRIPE_WIDTH)*DRAM_STRIPE_WIDTH*(NmbyLuma<<4));
        size_of_luma_buffer = ((size_of_luma_buffer+1023) & 0xfffffc00);
        size_of_420_chroma_buffer=bvceEpmAlignJword(bvceEpmCalcWidthInStripes(max_horizontal_size_in_pels,DRAM_STRIPE_WIDTH)*DRAM_STRIPE_WIDTH*(NmbyChroma<<4));
        size_of_420_chroma_buffer = ((size_of_420_chroma_buffer+1023) & 0xfffffc00);
        size_of_shifted_422_chroma_buffer=size_of_420_chroma_buffer;

        size_of_2h1v_luma_buffer=bvceEpmAlignJword(bvceEpmCalcWidthInStripes(max_horizontal_size_in_pels>>1,DRAM_STRIPE_WIDTH)*DRAM_STRIPE_WIDTH*(NmbyLuma<<4));
        size_of_2h1v_luma_buffer = ((size_of_2h1v_luma_buffer+1023) & 0xfffffc00);
        size_of_2h2v_luma_buffer=bvceEpmAlignJword(bvceEpmCalcWidthInStripes(max_horizontal_size_in_pels>>1,DRAM_STRIPE_WIDTH)*DRAM_STRIPE_WIDTH*(NmbyChroma<<4));
        size_of_2h2v_luma_buffer = ((size_of_2h2v_luma_buffer+1023) & 0xfffffc00);

        size_of_csc_buffer=bvceEpmAlignJword(6*max_horizontal_size_in_mbs*max_vertical_size_in_mbs);
        size_of_csc_buffer = ((size_of_csc_buffer+1023) & 0xfffffc00);

        size_of_reconstructed_luma = ((size_of_luma_buffer+1023) & 0xfffffc00);/*TODO align according to page size*/
        size_of_reconstructed_chroma=size_of_420_chroma_buffer;

        /*Allocate Stack of buffers for the Original picture*/
        BufferSize=size_of_luma_buffer+size_of_420_chroma_buffer;

        /*save start address and buffer size for on the fly address calculation*/
        CurrAddress = ((CurrAddress+1023) & 0xfffffc00); /*TODO align according to page size*/


        for (StackPointer=0; StackPointer<(EPM_NUMBER_OF_ORIGINAL_PICTURE_QUEUES<<1) ; StackPointer+=2)
        {
            CurrAddress = ((CurrAddress+1023) & 0xfffffc00); /*TODO align according to page size*/
            CurrAddress=CurrAddress+BufferSize;
        }

        /*Allocate Stack of buffers for the Original Shifted 4:2:2 top field Chroma*/
        BufferSize=size_of_shifted_422_chroma_buffer;

        /*save start address and buffer size for on the fly address calculation*/
        CurrAddress = ((CurrAddress+1023) & 0xfffffc00); /*TODO align according to page size*/

        for (StackPointer=0; StackPointer<(EPM_NUMBER_OF_ORIGINAL_PICTURE_QUEUES) ; StackPointer+=1)
        {
            CurrAddress = ((CurrAddress+1023) & 0xfffffc00); /*TODO align according to page size*/
            CurrAddress=CurrAddress+BufferSize;
        }

        /*Allocate stack of buffers for the Decimated picture*/
        BufferSize=size_of_2h1v_luma_buffer+size_of_2h2v_luma_buffer;

        /*save start address and buffer size for on the fly address calculation*/
        CurrAddress = ((CurrAddress+1023) & 0xfffffc00); /*TODO align according to page size*/

        for (StackPointer=0; StackPointer<(EPM_NUMBER_OF_DECIMATED_PICTURE_QUEUES<<1) ; StackPointer+=2)
        {
            CurrAddress = ((CurrAddress+1023) & 0xfffffc00); /*TODO align according to page size*/
            CurrAddress=CurrAddress+BufferSize;
        }

        /*Allocated stack of buffers for the CSC*/
        BufferSize=size_of_csc_buffer*4;/*In progressive mode there are only two references*/

        /*save start address and buffer size for on the fly address calculation*/
        CurrAddress = ((CurrAddress+1023) & 0xfffffc00); /*TODO align according to page size*/

        for (StackPointer=0; StackPointer<EPM_NUMBER_OF_CSC_QUEUES ; StackPointer++)
        {
            CurrAddress = ((CurrAddress+1023) & 0xfffffc00); /*TODO align according to page size*/
            CurrAddress=CurrAddress+BufferSize;
        }

        /*Allocate stack of buffers for the Reconstructed*/
        BufferSize=size_of_reconstructed_luma+size_of_reconstructed_chroma;

        /*save start address and buffer size for on the fly address calculation*/
        CurrAddress = ((CurrAddress+1023) & 0xfffffc00); /*TODO align according to page size*/

        for (StackPointer=0; StackPointer<EPM_NUMBER_OF_RECONSTRUCTED_PICTURE_QUEUES ; StackPointer++)
        {
            CurrAddress = ((CurrAddress+1023) & 0xfffffc00); /*TODO align according to page size*/
            CurrAddress=CurrAddress+BufferSize;
        }

        /*allocate SD buffers*/
        EPM_NUMBER_OF_ORIGINAL_PICTURE_QUEUES=EPM_MAX_NUMBER_OF_IBBP_ORIGINAL_PICTURE_QUEUES-EPM_MAX_NUMBER_OF_IP_ORIGINAL_PICTURE_QUEUES;
        EPM_NUMBER_OF_DECIMATED_PICTURE_QUEUES=EPM_MAX_NUMBER_OF_IBBP_DECIMATED_PICTURE_QUEUES-EPM_MAX_NUMBER_OF_IP_DECIMATED_PICTURE_QUEUES;

        max_horizontal_size_in_pels=HOR_SIZE_IN_PELS_PAL;
        if (max_horizontal_size_in_pels>MaxPictureWidthInPels)
        {
            max_horizontal_size_in_pels=MaxPictureWidthInPels;
        }

        max_vertical_size_in_pels   = VER_SIZE_IN_PELS_PAL;
        if (max_vertical_size_in_pels>MaxPictureHeightInPels)
        {
            max_vertical_size_in_pels=MaxPictureHeightInPels;
        }


        max_horizontal_size_in_mbs=max_horizontal_size_in_pels>>4;
        max_vertical_size_in_mbs=max_vertical_size_in_pels>>4;

        NmbyLuma=bvceEpmCalcNMBY(max_vertical_size_in_mbs);
        NmbyChroma=bvceEpmCalcNMBY((max_vertical_size_in_mbs + 1) >> 1);


        size_of_luma_buffer=bvceEpmAlignJword(bvceEpmCalcWidthInStripes(max_horizontal_size_in_pels,DRAM_STRIPE_WIDTH)*DRAM_STRIPE_WIDTH*(NmbyLuma<<4));
        size_of_luma_buffer = ((size_of_luma_buffer+1023) & 0xfffffc00);
        size_of_420_chroma_buffer=bvceEpmAlignJword(bvceEpmCalcWidthInStripes(max_horizontal_size_in_pels,DRAM_STRIPE_WIDTH)*DRAM_STRIPE_WIDTH*(NmbyChroma<<4));
        size_of_420_chroma_buffer = ((size_of_420_chroma_buffer+1023) & 0xfffffc00);
        size_of_shifted_422_chroma_buffer=size_of_420_chroma_buffer;

        size_of_2h1v_luma_buffer=bvceEpmAlignJword(bvceEpmCalcWidthInStripes(max_horizontal_size_in_pels>>1,DRAM_STRIPE_WIDTH)*DRAM_STRIPE_WIDTH*(NmbyLuma<<4));
        size_of_2h1v_luma_buffer = ((size_of_2h1v_luma_buffer+1023) & 0xfffffc00);
        size_of_2h2v_luma_buffer=bvceEpmAlignJword(bvceEpmCalcWidthInStripes(max_horizontal_size_in_pels>>1,DRAM_STRIPE_WIDTH)*DRAM_STRIPE_WIDTH*(NmbyChroma<<4));
        size_of_2h2v_luma_buffer = ((size_of_2h2v_luma_buffer+1023) & 0xfffffc00);

        size_of_csc_buffer=bvceEpmAlignJword(6*max_horizontal_size_in_mbs*max_vertical_size_in_mbs);
        size_of_csc_buffer = ((size_of_csc_buffer+1023) & 0xfffffc00);

        size_of_reconstructed_luma = ((size_of_luma_buffer+1023) & 0xfffffc00);/*TODO align according to page size*/
        size_of_reconstructed_chroma=size_of_420_chroma_buffer;

        /*Allocate Stack of buffers for the Original picture*/
        BufferSize=size_of_luma_buffer+size_of_420_chroma_buffer;

        /*save start address and buffer size for on the fly address calculation*/
        CurrAddress = ((CurrAddress+1023) & 0xfffffc00); /*TODO align according to page size*/

        for (StackPointer=0; StackPointer<(EPM_NUMBER_OF_ORIGINAL_PICTURE_QUEUES<<1) ; StackPointer+=2)
        {
            CurrAddress = ((CurrAddress+1023) & 0xfffffc00); /*TODO align according to page size*/
            CurrAddress=CurrAddress+BufferSize;
        }

        /*Allocate Stack of buffers for the Original Shifted 4:2:2 top field Chroma*/
        BufferSize=size_of_shifted_422_chroma_buffer;

        /*save start address and buffer size for on the fly address calculation*/
        CurrAddress = ((CurrAddress+1023) & 0xfffffc00); /*TODO align according to page size*/

        for (StackPointer=0; StackPointer<(EPM_NUMBER_OF_ORIGINAL_PICTURE_QUEUES) ; StackPointer+=1)
        {
            CurrAddress = ((CurrAddress+1023) & 0xfffffc00); /*TODO align according to page size*/
            CurrAddress=CurrAddress+BufferSize;
        }

        /*Allocate stack of buffers for the Decimated picture*/
        BufferSize=size_of_2h1v_luma_buffer+size_of_2h2v_luma_buffer;

        /*save start address and buffer size for on the fly address calculation*/
        CurrAddress = ((CurrAddress+1023) & 0xfffffc00); /*TODO align according to page size*/

        for (StackPointer=0; StackPointer<(EPM_NUMBER_OF_DECIMATED_PICTURE_QUEUES<<1) ; StackPointer+=2)
        {
            CurrAddress = ((CurrAddress+1023) & 0xfffffc00); /*TODO align according to page size*/
            CurrAddress=CurrAddress+BufferSize;
        }

        return(CurrAddress);
    }


}

#endif /* defined(BCHP_CHIP) || defined(CEB2APP) || defined(NGE_FW_SIM_APP) */

