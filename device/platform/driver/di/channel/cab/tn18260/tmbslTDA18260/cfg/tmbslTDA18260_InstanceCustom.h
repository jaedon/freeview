/*
  Copyright (C) 2006-2009 NXP B.V., All Rights Reserved.
  This source code and any compilation or derivative thereof is the proprietary
  information of NXP B.V. and is confidential in nature. Under no circumstances
  is this software to be  exposed to or placed under an Open Source License of
  any type without the expressed written permission of NXP B.V.
 *
 * \file          tmbslTDA18260_InstanceCustom.h
 *
 *                1
 *
 * \date          %modify_time%
 *
 * \brief         Describe briefly the purpose of this file.
 *
 * REFERENCE DOCUMENTS :
 *
 *
 * Detailed description may be added here.
 *
 * \section info Change Information
 *
*/

#ifndef _TMBSL_TDA18260_INSTANCE_CUSTOM_H
#define _TMBSL_TDA18260_INSTANCE_CUSTOM_H


#ifdef __cplusplus
extern "C"
{
#endif

/*============================================================================*/
/* Types and defines:                                                         */
/*============================================================================*/

/* Driver settings version definition */
#define TMBSL_TDA18260_SETTINGS_CUSTOMER_NUM    0  /* SW Settings Customer Number */
#define TMBSL_TDA18260_SETTINGS_PROJECT_NUM     1  /* SW Settings Project Number  */
#define TMBSL_TDA18260_SETTINGS_MAJOR_VER       2  /* SW Settings Major Version   */
#define TMBSL_TDA18260_SETTINGS_MINOR_VER       4  /* SW Settings Minor Version   */

/*======================================================================================================*/
/*======================================================================================================*/
/* Custom Driver: RF Plan                                                                               */
/*======================================================================================================*/
/*======================================================================================================*/
typedef enum _tmTDA18260_BandIndex_t
{
    tmTDA18260_IND_DIG_Band_1 = 0,
    tmTDA18260_IND_DIG_Band_2 = 1,
    tmTDA18260_IND_DIG_Band_3 = 2,
    tmTDA18260_IND_DIG_Band_4 = 3,
    tmTDA18260_IND_DIG_Band_5 = 4,
    tmTDA18260_IND_DIG_Band_6 = 5,
    tmTDA18260_IND_DIG_Band_7 = 6,
    tmTDA18260_IND_DIG_Band_8 = 7,
    tmTDA18260_IND_DIG_Band_9 = 8,
    tmTDA18260_IND_DIG_Band_10 = 9,
    tmTDA18260_IND_DIG_Band_11 = 10,
    tmTDA18260_IND_DIG_Band_12 = 11,
    tmTDA18260_IND_DIG_Band_13 = 12,
    tmTDA18260_IND_Band_Max = 13
} tmTDA18260_BandIndex_t, *ptmTDA18260_BandIndex_t;


#define tmTDA18260_RF_MAX_Band_1    67000000
#define tmTDA18260_RF_MAX_Band_2    93000000
#define tmTDA18260_RF_MAX_Band_3   134000000
#define tmTDA18260_RF_MAX_Band_4   200000000
#define tmTDA18260_RF_MAX_Band_5   216000000
#define tmTDA18260_RF_MAX_Band_6   252000000
#define tmTDA18260_RF_MAX_Band_7   300000000
#define tmTDA18260_RF_MAX_Band_8   365000000
#define tmTDA18260_RF_MAX_Band_9   500000000
#define tmTDA18260_RF_MAX_Band_10  548000000
#define tmTDA18260_RF_MAX_Band_11  625000000
#define tmTDA18260_RF_MAX_Band_12  725000000
#define tmTDA18260_RF_MAX_Band_13 1004000000


/*======================================================================================================*/
/*======================================================================================================*/
/* Custom Driver: Get Power Level                                                                       */
/*======================================================================================================*/
/*======================================================================================================*/
/*  { X,     X   }  */
/*  { 0,     1   }  */
/*  {               */
#define TMBSL_TDA18260_INSTANCE_CUSTOM_COMMON_Lvl_RSSI \
    {-175, 25}


#define tmTDA18260_RfFilter_Index_Max tmTDA18260_IND_Band_Max
/*  {    X   ,   X   ,   X   ,               R             }      */
/*  {    0   ,   1   ,   2   ,               f             },     */
/*  {        ,       ,       ,               M             },     */
/*  {        ,       ,       ,               a             },     */
/*  {        ,       ,       ,               x             },     */
/*  {        ,       ,       ,                             },     */
/*  {        ,       ,       ,                             },     */
#define TMBSL_TDA18260_INSTANCE_CUSTOM_COMMON_Lvl_RfFilter         \
{                                                                  \
    { 41490418, 338087, -29717, tmTDA18260_RF_MAX_Band_1},          \
    { 41300630, 277538, -18204, tmTDA18260_RF_MAX_Band_2},          \
    { 46334655, 111782,  -5103, tmTDA18260_RF_MAX_Band_3},          \
    { 46521670,  88977,  -3123, tmTDA18260_RF_MAX_Band_4},          \
    { 54461962, -50194,   1506, tmTDA18260_RF_MAX_Band_5},          \
    { 30035204, 166828,  -3315, tmTDA18260_RF_MAX_Band_6},          \
    { 47724988,  23632,   -421, tmTDA18260_RF_MAX_Band_7},          \
    { 45109586,  29273,   -437, tmTDA18260_RF_MAX_Band_8},          \
    { 44764759,  32405,   -404, tmTDA18260_RF_MAX_Band_9},          \
    { 16309161, 145612,  -1441, tmTDA18260_RF_MAX_Band_10},         \
    { -8087804, 193126,  -1571, tmTDA18260_RF_MAX_Band_11},         \
    {-11835590, 184448,  -1339, tmTDA18260_RF_MAX_Band_12},         \
    { 20692875,  73258,   -465, tmTDA18260_RF_MAX_Band_13}          \
}


#define tmTDA18260_Lvl_AGC2b_Index_Max 13
/*  {    X  ,  X  ,  X ,  u  },   */
/*  {    0  ,  1  ,  2 ,  A  },   */
/*  {       ,     ,    ,  G  },   */
/*  {       ,     ,    ,  C  },   */
/*  {       ,     ,    ,  2  },   */
/*  {       ,     ,    ,  b  },   */
/*  {       ,     ,    ,     },   */
#define TMBSL_TDA18260_INSTANCE_CUSTOM_COMMON_Lvl_AGC2b             \
{                                                                   \
    {-260959, 2122, 227,   10 },                                    \
    {-263556, 2119, 215,   21 },                                    \
    {-265240, 2050, 209,   32 },                                    \
    {-270116, 2114, 199,   43 },                                    \
    {-270395, 2050, 191,   54 },                                    \
    {-274770, 2084, 181,   65 },                                    \
    {-277373, 2077, 172,   76 },                                    \
    {-277134, 2034, 155,   87 },                                    \
    {-281277, 2053, 138,   98 },                                    \
    {-280568, 2014, 116,  109 },                                    \
    {-280370, 1999,  86,  120 },                                    \
    {-275609, 1950,  47,  131 },                                    \
    {-272356, 1918,   0,  142 }                                     \
}



#define tmTDA18260_Lvl_AGC1_Index_Max 10

/* {    X     ,   X   ,  X ,                  u                 },      */
/* {    0     ,   1   ,  2 ,                  A                 },      */
/* {          ,       ,    ,                  G                 },      */
/* {          ,       ,    ,                  C                 },      */
/* {          ,       ,    ,                  1                 },      */
/* {          ,       ,    ,                                    },      */
#define TMBSL_TDA18260_INSTANCE_CUSTOM_COMMON_Lvl_AGC1_MasterMulti       \
{  /* AGC1 Master with Slave(s) */                                       \
    {-191465210,  11163,  41, tmTDA18260_AGC1_Gain_Fixed_0dB   },      \
    {-154791080, -27725, 322, tmTDA18260_AGC1_Gain_Fixed_3dB   },        \
    {-140061105,   7679,  34, tmTDA18260_AGC1_Gain_Fixed_4_875dB },        \
    {-119861050,   6130,  32, tmTDA18260_AGC1_Gain_Fixed_6_75dB   },        \
    {-100046806,   5489,  12, tmTDA18260_AGC1_Gain_Fixed_8_625dB },        \
    { -80752436,   4326,   2, tmTDA18260_AGC1_Gain_Fixed_10_5dB   },        \
    { -60957864,    518,   1, tmTDA18260_AGC1_Gain_Fixed_12_375dB},        \
    { -41298061,  -4999,  13, tmTDA18260_AGC1_Gain_Fixed_14_25dB  },        \
    { -21309749, -12147,  12, tmTDA18260_AGC1_Gain_Fixed_16_125dB},        \
    {    -27632, -25095,  54, tmTDA18260_AGC1_Gain_Fixed_18dB  }         \
}


/* {     X    ,    X  ,  X  ,                  u                 },      */
/* {     0    ,    1  ,  2  ,                  A                 },      */
/* {          ,       ,     ,                  G                 },      */
/* {          ,       ,     ,                  C                 },      */
/* {          ,       ,     ,                  1                 },      */
/* {          ,       ,     ,                                    },      */
#define TMBSL_TDA18260_INSTANCE_CUSTOM_COMMON_Lvl_AGC1                    \
{                                                                         \
    /* AGC1 Single Master or Slave */                                     \
    {-191288065,  15612,  49,  tmTDA18260_AGC1_Gain_Fixed_0dB   },        \
    {-154398135, -22420, 340,  tmTDA18260_AGC1_Gain_Fixed_3dB   },        \
    {-139806355,  14740,  58,  tmTDA18260_AGC1_Gain_Fixed_4_875dB },        \
    {-119619210,  14635,  44,  tmTDA18260_AGC1_Gain_Fixed_6_75dB   },        \
    { -99935846,  15804,  16,  tmTDA18260_AGC1_Gain_Fixed_8_625dB },        \
    { -80445418,  14766,  18,  tmTDA18260_AGC1_Gain_Fixed_10_5dB   },        \
    { -60817485,  13080,  14,  tmTDA18260_AGC1_Gain_Fixed_12_375dB},        \
    { -41173897,  11238,   4,  tmTDA18260_AGC1_Gain_Fixed_14_25dB  },        \
    { -21107238,   7394, -10,  tmTDA18260_AGC1_Gain_Fixed_16_125dB},        \
    {         0,      0,   0,  tmTDA18260_AGC1_Gain_Fixed_18dB  }         \
}


/* {X, X, X, S}, */
/* {0, 1, 2, U}, */
/* { ,  ,  , M}, */
/* { ,  ,  ,  }, */
#define TMBSL_TDA18260_INSTANCE_CUSTOM_COMMON_Lvl_XDIV10                                        \
{                                                                                               \
   /* Division by 10 coefficients  */                                                           \
   {4, 4, 5, 0},   /* RF Filter    */                                                           \
   {2, 0, 3, 0},   /* AGC2         */                                                           \
   {0, 0, 1, 5},   /* AGC1         */                                                           \
   {0, 0, 1, 5},   /* AGC1Multi    */                                                           \
   {0, 0, 0, 5}    /* Offset Slave */                                                           \
}


/*  { X,     X   }  */
/*  { 0,     1   }  */
/*  {               */
#define TMBSL_TDA18260_INSTANCE_CUSTOM_COMMON_Lvl_Offset \
    {90000000, -14584}

/*======================================================================================================*/
/*======================================================================================================*/
/* Custom Driver Instance Parameters: AGC_TOP                                                           */
/*======================================================================================================*/
/*======================================================================================================*/
#define TMBSL_TDA18260_INSTANCE_CUSTOM_COMMON_AGC_TOP                                                    \
    {   /* AGC1_Gud */                                                                                   \
        0x06,                                               /* AGC1_Gud_Up    */                         \
        0x03                                                /* AGC1_Gud_Down  */                         \
    },                                                                                                   \
    {   /* AGC1_Aup */                                                                                   \
        0x02,                                               /* AGC1_Aup_0dB   */                         \
        0x05,                                               /* AGC1_Aup_3dB   */                         \
        0x06,                                               /* AGC1_Aup_4_5dB */                         \
        0x05,                                               /* AGC1_Aup_6dB   */                         \
        0x06,                                               /* AGC1_Aup_7_5dB */                         \
        0x06,                                               /* AGC1_Aup_9dB   */                         \
        0x06,                                               /* AGC1_Aup_10_5dB*/                         \
        0x05,                                               /* AGC1_Aup_12dB  */                         \
        0x05                                                /* AGC1_Aup_13_5dB*/                         \
    },                                                                                                   \
    {   /* AGC1_Adown */                                                                                 \
        0x02,                                               /* AGC1_Ado_3dB   */                         \
        0x06,                                               /* AGC1_Ado_4_5dB */                         \
        0x07,                                               /* AGC1_Ado_6dB   */                         \
        0x06,                                               /* AGC1_Ado_7_5dB */                         \
        0x07,                                               /* AGC1_Ado_9dB   */                         \
        0x07,                                               /* AGC1_Ado_10_5dB*/                         \
        0x06,                                               /* AGC1_Ado_12dB  */                         \
        0x05,                                               /* AGC1_Ado_13_5dB*/                         \
        0x05                                                /* AGC1_Ado_15dB  */                         \
    },                                                                                                   \
    {   /* AGC2_GdownDetector[] */                                                                       \
        0x08,                                               /* AGC2_Detector2 */                         \
        0x08,                                               /* AGC2_Detector3 */                         \
        0x0B                                                /* AGC2_Detector4 */                         \
    }

/*======================================================================================================*/
/*======================================================================================================*/
/* Custom Driver Instance Parameters: STD_ARRAY                                                         */
/*======================================================================================================*/
/*======================================================================================================*/
#define TMBSL_TDA18260_INSTANCE_CUSTOM_COMMON_STD_DIGITAL_6MHz                                           \
        {                                                   /* Digital 6MHz   */                         \
            3500000,                                        /* IF             */                         \
            tmTDA18260_LPF_6MHz,                            /* LPF            */                         \
            tmTDA18260_LPF_Offset_0,                        /* LPF offset     */                         \
            tmTDA18260_PD_NotActivated,                     /* hpBypass       */                         \
            tmTDA18260_HPF_Freq_400KHz                      /* IF_hpf         */                         \
        }

#define TMBSL_TDA18260_INSTANCE_CUSTOM_COMMON_STD_DIGITAL_7MHz                                           \
        {                                                   /* Digital 7MHz   */                         \
            4000000,                                        /* IF             */                         \
            tmTDA18260_LPF_8MHz,                            /* LPF            */                         \
            tmTDA18260_LPF_Offset_0,                        /* LPF offset     */                         \
            tmTDA18260_PD_NotActivated,                     /* hpBypass       */                         \
            tmTDA18260_HPF_Freq_400KHz                      /* IF_hpf         */                         \
        }

#define TMBSL_TDA18260_INSTANCE_CUSTOM_COMMON_STD_DIGITAL_8MHz                                           \
        {                                                   /* Digital 8MHz   */                         \
            4250000,                                        /* IF             */                         \
            tmTDA18260_LPF_8MHz,                            /* LPF            */                         \
            tmTDA18260_LPF_Offset_0,                        /* LPF offset     */                         \
            tmTDA18260_PD_NotActivated,                     /* hpBypass       */                         \
            tmTDA18260_HPF_Freq_400KHz                      /* IF_hpf         */                         \
        }

#define TMBSL_TDA18260_INSTANCE_CUSTOM_COMMON_STD_DIGITAL_9MHz                                           \
        {                                                   /* Digital 9MHz   */                         \
            5000000,                                        /* IF             */                         \
            tmTDA18260_LPF_9MHz,                            /* LPF            */                         \
            tmTDA18260_LPF_Offset_0,                        /* LPF offset     */                         \
            tmTDA18260_PD_NotActivated,                     /* PD_hpf         */                         \
            tmTDA18260_HPF_Freq_400KHz                      /* IF_hpf         */                         \
        }

#define TMBSL_TDA18260_INSTANCE_CUSTOM_COMMON_STD_ARRAY                                                  \
    {   /* stdArray */                                                                                   \
        TMBSL_TDA18260_INSTANCE_CUSTOM_COMMON_STD_DIGITAL_6MHz,                                          \
        TMBSL_TDA18260_INSTANCE_CUSTOM_COMMON_STD_DIGITAL_7MHz,                                          \
        TMBSL_TDA18260_INSTANCE_CUSTOM_COMMON_STD_DIGITAL_8MHz,                                          \
        TMBSL_TDA18260_INSTANCE_CUSTOM_COMMON_STD_DIGITAL_9MHz,                                          \
    }

/*======================================================================================================*/
/*======================================================================================================*/
/* Custom Driver Instance Parameters: BAND_PARAM                                                        */
/*======================================================================================================*/
/*======================================================================================================*/
/*                                                          {{            F             F},              L           ,         S             , i, c, c, c, B, B, c, c, r,             p             , a, b, S, S, p, p}*/
/*                                                          {{            i             i},              o           ,         w             , n, a, a, a, P, P, a, a, f,             d             , g, p, t, t, s, s}*/
/*                                                          {{            l             l},              M           ,         i             , d, p, p, p, I, I, p, p, F,             A             , c, F, o, o, m, m}*/
/*                                                          {{            t             t},              a           ,         t             , D, H, H, H, n, n, L, L, i,             G             , 1, i, b, b, S, B}*/
/*                                                          {{            e             e},              x           ,         c             , i, P, P, P, d, d, 6, 1, l,             C             , b, l, G, D, t, u}*/
/*                                                          {{            r             r},              -           ,         h             , v, L, M, R, 1, 2, n, 0, t,             1             , i, t, a, C, o, f}*/
/*                                                          {{                           },              I           ,         T             , 2, e, i, i,  ,  ,  , n, e,             b             , s, e, i,  , b, 1}*/
/*                                                          {{            T             n},              F           ,         y             ,  , f, d, g,  ,  ,  ,  , r,             i             , G, r, n,  ,  , b}*/
/*                                                          {{            y             u},                          ,         p             ,  , t, i, h,  ,  ,  ,  , S,             s             , a,  ,  ,  ,  ,  }*/
/*                                                          {{            p             m},              (           ,         e             ,  ,  ,  , t,  ,  ,  ,  , w,                           , i,  ,  ,  ,  ,  }*/
/*                                                          {{            e             b},              H           ,                       ,  ,  ,  ,  ,  ,  ,  ,  , i,                           , n,  ,  ,  ,  ,  }*/
/*                                                          {{                          e},              z           ,                       ,  ,  ,  ,  ,  ,  ,  ,  , t,                           ,  ,  ,  ,  ,  ,  }*/
/*                                                          {{                          r},              )           ,                       ,  ,  ,  ,  ,  ,  ,  ,  , c,                           ,  ,  ,  ,  ,  ,  )*/
/*                                                          {{                           },                          ,                       ,  ,  ,  ,  ,  ,  ,  ,  , h,                           ,  ,  ,  ,  ,  ,  )*/
#define TMBSL_TDA18260_INSTANCE_CUSTOM_COMMON_BP_DIG_Band1  {{tmTDA18260_FILTER_BYPASS, 0}, tmTDA18260_RF_MAX_Band_1 , tmTDA18260_RFSW_BYPASS, 0, 0, 0, 0, 0, 0, 0, 0, 0, tmTDA18260_PD_Activated   , 4, 0, 1, 1, 2, 2}
#define TMBSL_TDA18260_INSTANCE_CUSTOM_COMMON_BP_DIG_Band2  {{tmTDA18260_FILTER_BYPASS, 1}, tmTDA18260_RF_MAX_Band_2 , tmTDA18260_RFSW_BYPASS, 0, 0, 0, 0, 0, 0, 0, 0, 0, tmTDA18260_PD_Activated   , 4, 1, 1, 1, 2, 2}
#define TMBSL_TDA18260_INSTANCE_CUSTOM_COMMON_BP_DIG_Band3  {{tmTDA18260_FILTER_BYPASS, 2}, tmTDA18260_RF_MAX_Band_3 , tmTDA18260_RFSW_BYPASS, 0, 0, 0, 0, 0, 0, 0, 0, 0, tmTDA18260_PD_Activated   , 4, 2, 1, 1, 2, 2}
#define TMBSL_TDA18260_INSTANCE_CUSTOM_COMMON_BP_DIG_Band4  {{tmTDA18260_FILTER_BYPASS, 3}, tmTDA18260_RF_MAX_Band_4 , tmTDA18260_RFSW_BYPASS, 0, 0, 0, 0, 0, 0, 0, 0, 0, tmTDA18260_PD_Activated   , 4, 3, 1, 1, 2, 2}
#define TMBSL_TDA18260_INSTANCE_CUSTOM_COMMON_BP_DIG_Band5  {{tmTDA18260_FILTER_LPF,    0}, tmTDA18260_RF_MAX_Band_5 , tmTDA18260_RFSW_LP    , 1, 0, 3, 0, 0, 0, 2, 1, 0, tmTDA18260_PD_NotActivated, 2, 6, 0, 1, 1, 2}
#define TMBSL_TDA18260_INSTANCE_CUSTOM_COMMON_BP_DIG_Band6  {{tmTDA18260_FILTER_LPF,    1}, tmTDA18260_RF_MAX_Band_6 , tmTDA18260_RFSW_LP    , 1, 0, 3, 0, 0, 0, 2, 2, 0, tmTDA18260_PD_NotActivated, 2, 6, 0, 1, 1, 2}
#define TMBSL_TDA18260_INSTANCE_CUSTOM_COMMON_BP_DIG_Band7  {{tmTDA18260_FILTER_LPF,    2}, tmTDA18260_RF_MAX_Band_7 , tmTDA18260_RFSW_LP    , 1, 0, 3, 0, 0, 0, 2, 0, 0, tmTDA18260_PD_NotActivated, 2, 6, 0, 1, 1, 2}
#define TMBSL_TDA18260_INSTANCE_CUSTOM_COMMON_BP_DIG_Band8  {{tmTDA18260_FILTER_LPF,    3}, tmTDA18260_RF_MAX_Band_8 , tmTDA18260_RFSW_LP    , 1, 0, 3, 0, 2, 0, 3, 3, 0, tmTDA18260_PD_NotActivated, 2, 6, 0, 1, 1, 2}
#define TMBSL_TDA18260_INSTANCE_CUSTOM_COMMON_BP_DIG_Band9  {{tmTDA18260_FILTER_LPF,    5}, tmTDA18260_RF_MAX_Band_9 , tmTDA18260_RFSW_LP    , 1, 0, 3, 0, 0, 0, 1, 1, 0, tmTDA18260_PD_NotActivated, 2, 6, 0, 1, 1, 2}
#define TMBSL_TDA18260_INSTANCE_CUSTOM_COMMON_BP_DIG_Band10 {{tmTDA18260_FILTER_BPF,    0}, tmTDA18260_RF_MAX_Band_10, tmTDA18260_RFSW_HP    , 1, 3, 3, 0, 1, 2, 3, 1, 0, tmTDA18260_PD_NotActivated, 2, 6, 0, 1, 1, 2}
#define TMBSL_TDA18260_INSTANCE_CUSTOM_COMMON_BP_DIG_Band11 {{tmTDA18260_FILTER_HPF,    0}, tmTDA18260_RF_MAX_Band_11, tmTDA18260_RFSW_HP    , 0, 2, 3, 3, 4, 1, 2, 0, 1, tmTDA18260_PD_NotActivated, 1, 6, 0, 1, 1, 1}
#define TMBSL_TDA18260_INSTANCE_CUSTOM_COMMON_BP_DIG_Band12 {{tmTDA18260_FILTER_HPF,    1}, tmTDA18260_RF_MAX_Band_12, tmTDA18260_RFSW_HP    , 0, 2, 3, 3, 2, 1, 1, 0, 1, tmTDA18260_PD_NotActivated, 1, 6, 0, 1, 1, 1}
#define TMBSL_TDA18260_INSTANCE_CUSTOM_COMMON_BP_DIG_Band13 {{tmTDA18260_FILTER_HPF,    2}, tmTDA18260_RF_MAX_Band_13, tmTDA18260_RFSW_HP    , 0, 1, 0, 1, 3, 0, 0, 0, 1, tmTDA18260_PD_NotActivated, 1, 6, 0, 1, 1, 1}

#define TMBSL_TDA18260_INSTANCE_CUSTOM_COMMON_BAND_PARAM                                                 \
    tmTDA18260_IND_DIG_Band_1,                              /* BandIndex     */                          \
    {   /* BandParam */                                                                                  \
        TMBSL_TDA18260_INSTANCE_CUSTOM_COMMON_BP_DIG_Band1,                                              \
        TMBSL_TDA18260_INSTANCE_CUSTOM_COMMON_BP_DIG_Band2,                                              \
        TMBSL_TDA18260_INSTANCE_CUSTOM_COMMON_BP_DIG_Band3,                                              \
        TMBSL_TDA18260_INSTANCE_CUSTOM_COMMON_BP_DIG_Band4,                                              \
        TMBSL_TDA18260_INSTANCE_CUSTOM_COMMON_BP_DIG_Band5,                                              \
        TMBSL_TDA18260_INSTANCE_CUSTOM_COMMON_BP_DIG_Band6,                                              \
        TMBSL_TDA18260_INSTANCE_CUSTOM_COMMON_BP_DIG_Band7,                                              \
        TMBSL_TDA18260_INSTANCE_CUSTOM_COMMON_BP_DIG_Band8,                                              \
        TMBSL_TDA18260_INSTANCE_CUSTOM_COMMON_BP_DIG_Band9,                                              \
        TMBSL_TDA18260_INSTANCE_CUSTOM_COMMON_BP_DIG_Band10,                                             \
        TMBSL_TDA18260_INSTANCE_CUSTOM_COMMON_BP_DIG_Band11,                                             \
        TMBSL_TDA18260_INSTANCE_CUSTOM_COMMON_BP_DIG_Band12,                                             \
        TMBSL_TDA18260_INSTANCE_CUSTOM_COMMON_BP_DIG_Band13                                              \
    }

#define TMBSL_TDA18260_MASTER_UNIT (0)

/*======================================================================================================*/
/*======================================================================================================*/
/* Custom Driver Instance Parameters: (Path 0)                                                          */
/*======================================================================================================*/
/*======================================================================================================*/
#define TMBSL_TDA18260_INSTANCE_CUSTOM_0                                                                 \
    { 18260 },                                                                                           \
    tmTDA18260Shared_PowerOn,                               /* curSharedPowerS*/                         \
    tmTDA18260Stream_PowerStandby,                          /* curStreamPowerS*/                         \
    tmTDA18260_PowerSuspend,                                /* minPowerState  */                         \
    tmTDA18260_DeviceTypeMaster,                            /* deviceType     */                         \
    tmTDA18260_xtal_xtout_drive_800mV,                      /* xtout_drive    */                         \
    tmTDA18260_DIG_7MHz,                                    /* standardMode   */                         \
    tmTDA18260_AGC1_Gain_Free,                              /* agc1Gain       */                         \
    tmTDA18260_TCST_8,                                      /* agc1TimeCst    */                         \
    tmTDA18260_TCST_8,                                      /* ltTimeCst      */                         \
    tmTDA18260_IF_Level_0_5V,                               /* ifLevel        */                         \
    tmTDA18260_OFF,                                         /* MtoState       */                         \
    TMBSL_TDA18260_INSTANCE_CUSTOM_COMMON_AGC_TOP,                                                       \
    TMBSL_TDA18260_INSTANCE_CUSTOM_COMMON_STD_ARRAY,                                                     \
    TMBSL_TDA18260_INSTANCE_CUSTOM_COMMON_BAND_PARAM,                                                    \
    True,                                                   /* bLoopThroughAlwaysOn */                   \
    False                                                   /* bLockSharedPartAccess */

/*======================================================================================================*/
/*======================================================================================================*/
/* Custom Driver Instance Parameters: (Path 1)                                                          */
/*======================================================================================================*/
/*======================================================================================================*/
#define TMBSL_TDA18260_INSTANCE_CUSTOM_1                                                                 \
    { 18260 },                                                                                           \
    tmTDA18260Shared_PowerOn,                               /* curSharedPowerS*/                         \
    tmTDA18260Stream_PowerStandby,                          /* curStreamPowerS*/                         \
    tmTDA18260_PowerSuspend,                                /* minPowerState  */                         \
    tmTDA18260_DeviceTypeMaster,                            /* deviceType     */                         \
    tmTDA18260_xtal_xtout_drive_800mV,                      /* xtout_drive    */                         \
    tmTDA18260_DIG_7MHz,                                    /* standardMode   */                         \
    tmTDA18260_AGC1_Gain_Free,                              /* agc1Gain       */                         \
    tmTDA18260_TCST_8,                                      /* agc1TimeCst    */                         \
    tmTDA18260_TCST_8,                                      /* ltTimeCst      */                         \
    tmTDA18260_IF_Level_0_5V,                               /* ifLevel        */                         \
    tmTDA18260_OFF,                                         /* MtoState       */                         \
    TMBSL_TDA18260_INSTANCE_CUSTOM_COMMON_AGC_TOP,                                                       \
    TMBSL_TDA18260_INSTANCE_CUSTOM_COMMON_STD_ARRAY,                                                     \
    TMBSL_TDA18260_INSTANCE_CUSTOM_COMMON_BAND_PARAM,                                                    \
    True,                                                   /* bLoopThroughAlwaysOn */                   \
    False                                                   /* bLockSharedPartAccess */

/*======================================================================================================*/
/*======================================================================================================*/
/* Custom Driver Instance Parameters: (Path 2)                                                          */
/*======================================================================================================*/
/*======================================================================================================*/
#define TMBSL_TDA18260_INSTANCE_CUSTOM_2                                                                 \
    { 18260 },                                                                                           \
    tmTDA18260Shared_PowerOn,                               /* curSharedPowerS*/                         \
    tmTDA18260Stream_PowerStandby,                          /* curStreamPowerS*/                         \
    tmTDA18260_PowerSuspend,                                /* minPowerState  */                         \
    tmTDA18260_DeviceTypeSlave,                             /* deviceType     */                         \
    tmTDA18260_xtal_xtout_drive_Off,                        /* xtout_drive    */                         \
    tmTDA18260_DIG_7MHz,                                    /* standardMode   */                         \
    tmTDA18260_AGC1_Gain_Fixed_6_75dB,                      /* agc1Gain       */                         \
    tmTDA18260_TCST_8,                                      /* agc1TimeCst    */                         \
    tmTDA18260_TCST_8,                                      /* ltTimeCst      */                         \
    tmTDA18260_IF_Level_0_5V,                               /* ifLevel        */                         \
    tmTDA18260_OFF,                                         /* MtoState       */                         \
    TMBSL_TDA18260_INSTANCE_CUSTOM_COMMON_AGC_TOP,                                                       \
    TMBSL_TDA18260_INSTANCE_CUSTOM_COMMON_STD_ARRAY,                                                     \
    TMBSL_TDA18260_INSTANCE_CUSTOM_COMMON_BAND_PARAM,                                                    \
    False,                                                  /* bLoopThroughAlwaysOn */                   \
    False                                                   /* bLockSharedPartAccess */

/*======================================================================================================*/
/*======================================================================================================*/
/* Custom Driver Instance Parameters: (Path 3)                                                          */
/*======================================================================================================*/
/*======================================================================================================*/
#define TMBSL_TDA18260_INSTANCE_CUSTOM_3                                                                 \
    { 18260 },                                                                                           \
    tmTDA18260Shared_PowerOn,                               /* curSharedPowerS*/                         \
    tmTDA18260Stream_PowerStandby,                          /* curStreamPowerS*/                         \
    tmTDA18260_PowerSuspend,                                /* minPowerState  */                         \
    tmTDA18260_DeviceTypeSlave,                             /* deviceType     */                         \
    tmTDA18260_xtal_xtout_drive_Off,                        /* xtout_drive    */                         \
    tmTDA18260_DIG_7MHz,                                    /* standardMode   */                         \
    tmTDA18260_AGC1_Gain_Fixed_6_75dB,                      /* agc1Gain       */                         \
    tmTDA18260_TCST_8,                                      /* agc1TimeCst    */                         \
    tmTDA18260_TCST_8,                                      /* ltTimeCst      */                         \
    tmTDA18260_IF_Level_0_5V,                               /* ifLevel        */                         \
    tmTDA18260_OFF,                                         /* MtoState       */                         \
    TMBSL_TDA18260_INSTANCE_CUSTOM_COMMON_AGC_TOP,                                                       \
    TMBSL_TDA18260_INSTANCE_CUSTOM_COMMON_STD_ARRAY,                                                     \
    TMBSL_TDA18260_INSTANCE_CUSTOM_COMMON_BAND_PARAM,                                                    \
    False,                                                  /* bLoopThroughAlwaysOn */                   \
    False                                                   /* bLockSharedPartAccess */

/*======================================================================================================*/
/*======================================================================================================*/
/* Custom Driver Instance Parameters: (Path 4)                                                          */
/*======================================================================================================*/
/*======================================================================================================*/
#define TMBSL_TDA18260_INSTANCE_CUSTOM_4                                                                 \
    { 18260 },                                                                                           \
    tmTDA18260Shared_PowerOn,                               /* curSharedPowerS*/                         \
    tmTDA18260Stream_PowerStandby,                          /* curStreamPowerS*/                         \
    tmTDA18260_PowerSuspend,                                /* minPowerState  */                         \
    tmTDA18260_DeviceTypeSlave,                             /* deviceType     */                         \
    tmTDA18260_xtal_xtout_drive_Off,                        /* xtout_drive    */                         \
    tmTDA18260_DIG_7MHz,                                    /* standardMode   */                         \
    tmTDA18260_AGC1_Gain_Fixed_6_75dB,                      /* agc1Gain       */                         \
    tmTDA18260_TCST_8,                                      /* agc1TimeCst    */                         \
    tmTDA18260_TCST_8,                                      /* ltTimeCst      */                         \
    tmTDA18260_IF_Level_0_5V,                               /* ifLevel        */                         \
    tmTDA18260_OFF,                                         /* MtoState       */                         \
    TMBSL_TDA18260_INSTANCE_CUSTOM_COMMON_AGC_TOP,                                                       \
    TMBSL_TDA18260_INSTANCE_CUSTOM_COMMON_STD_ARRAY,                                                     \
    TMBSL_TDA18260_INSTANCE_CUSTOM_COMMON_BAND_PARAM,                                                    \
    False,                                                  /* bLoopThroughAlwaysOn */                   \
    False                                                   /* bLockSharedPartAccess */

/*======================================================================================================*/
/*======================================================================================================*/
/* Custom Driver Instance Parameters: (Path 5)                                                          */
/*======================================================================================================*/
/*======================================================================================================*/
#define TMBSL_TDA18260_INSTANCE_CUSTOM_5                                                                 \
    { 18260 },                                                                                           \
    tmTDA18260Shared_PowerOn,                               /* curSharedPowerS*/                         \
    tmTDA18260Stream_PowerStandby,                          /* curStreamPowerS*/                         \
    tmTDA18260_PowerSuspend,                                /* minPowerState  */                         \
    tmTDA18260_DeviceTypeSlave,                             /* deviceType     */                         \
    tmTDA18260_xtal_xtout_drive_Off,                        /* xtout_drive    */                         \
    tmTDA18260_DIG_7MHz,                                    /* standardMode   */                         \
    tmTDA18260_AGC1_Gain_Fixed_6_75dB,                      /* agc1Gain       */                         \
    tmTDA18260_TCST_8,                                      /* agc1TimeCst    */                         \
    tmTDA18260_TCST_8,                                      /* ltTimeCst      */                         \
    tmTDA18260_IF_Level_0_5V,                               /* ifLevel        */                         \
    tmTDA18260_OFF,                                         /* MtoState       */                         \
    TMBSL_TDA18260_INSTANCE_CUSTOM_COMMON_AGC_TOP,                                                       \
    TMBSL_TDA18260_INSTANCE_CUSTOM_COMMON_STD_ARRAY,                                                     \
    TMBSL_TDA18260_INSTANCE_CUSTOM_COMMON_BAND_PARAM,                                                    \
    False,                                                  /* bLoopThroughAlwaysOn */                   \
    False                                                   /* bLockSharedPartAccess */


#ifdef __cplusplus
}
#endif

#endif /* _TMBSL_TDA18260_INSTANCE_CUSTOM_H */

