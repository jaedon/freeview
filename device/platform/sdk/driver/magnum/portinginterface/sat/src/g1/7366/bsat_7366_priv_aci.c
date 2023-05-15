/******************************************************************************
*    (c)2011-2013 Broadcom Corporation
*
* This program is the proprietary software of Broadcom Corporation and/or its licensors,
* and may only be used, duplicated, modified or distributed pursuant to the terms and
* conditions of a separate, written license agreement executed between you and Broadcom
* (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
* no license (express or implied), right to use, or waiver of any kind with respect to the
* Software, and Broadcom expressly reserves all rights in and to the Software and all
* intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
* HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
* NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
*
* Except as expressly set forth in the Authorized License,
*
* 1.     This program, including its structure, sequence and organization, constitutes the valuable trade
* secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
* and to use this information only in connection with your use of Broadcom integrated circuit products.
*
* 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
* AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
* WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
* THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
* OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
* LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
* OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
* USE OR PERFORMANCE OF THE SOFTWARE.
*
* 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
* LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
* EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
* USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
* THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
* ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
* LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
* ANY LIMITED REMEDY.
*
* $brcm_Workfile: $
* $brcm_Revision: $
* $brcm_Date: $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: $
*
*****************************************************************************/
#include "bstd.h"
#include "bsat.h"
#include "bsat_priv.h"
#include "bsat_g1_priv.h"


#if (BCHP_CHIP != 7366)
#error "This file is for BCM7366 firmware only (not for host software)"
#endif

BDBG_MODULE(bsat_7366_priv_aci);


/* this file contains the channelizer ACI filter coefficients */

/* ACI for fb = 1000000.000000 fs = 4860.000000  */
/* ACI for ftrans = 3300000.000000 sba =  70.300000  */
static const int16_t BSAT_AciCoeff_1[36] = {
   -10,
   -13,
   -20,
   -30,
   -42,
   -56,
   -72,
   -90,
   -108,
   -125,
   -141,
   -153,
   -160,
   -159,
   -150,
   -130,
   -97,
   -50,
   13,
   92,
   186,
   297,
   422,
   560,
   707,
   862,
   1020,
   1177,
   1330,
   1473,
   1603,
   1715,
   1807,
   1874,
   1916,
   1929
};


/* ACI for fb = 2000000.000000 fs = 4860.000000  */
/* ACI for ftrans = 3300000.000000 sba =  70.300000  */
static const int16_t BSAT_AciCoeff_2[36] = {
   14,
   17,
   25,
   34,
   43,
   50,
   53,
   52,
   44,
   28,
   2,
   -34,
   -78,
   -130,
   -187,
   -243,
   -293,
   -332,
   -352,
   -346,
   -309,
   -234,
   -118,
   40,
   240,
   477,
   746,
   1036,
   1337,
   1637,
   1920,
   2175,
   2388,
   2549,
   2649,
   2683
};


/* ACI for fb = 3000000.000000 fs = 4860.000000  */
/* ACI for ftrans = 3300000.000000 sba =  70.300000  */
static const int16_t BSAT_AciCoeff_3[36] = {
   -13,
   -12,
   -14,
   -13,
   -8,
   3,
   21,
   44,
   73,
   103,
   131,
   151,
   157,
   144,
   109,
   47,
   -38,
   -142,
   -257,
   -370,
   -464,
   -524,
   -530,
   -468,
   -326,
   -98,
   213,
   599,
   1042,
   1517,
   1997,
   2448,
   2840,
   3142,
   3334,
   3399
};


/* ACI for fb = 4000000.000000 fs = 4860.000000  */
/* ACI for ftrans = 3300000.000000 sba =  70.300000  */
static const int16_t BSAT_AciCoeff_4[36] = {
   7,
   -2,
   -9,
   -22,
   -38,
   -57,
   -76,
   -88,
   -90,
   -76,
   -44,
   7,
   73,
   146,
   213,
   260,
   271,
   236,
   146,
   4,
   -176,
   -373,
   -555,
   -684,
   -722,
   -636,
   -404,
   -18,
   508,
   1144,
   1842,
   2540,
   3174,
   3681,
   4007,
   4120
};


/* ACI for fb = 5000000.000000 fs = 4860.000000  */
/* ACI for ftrans = 3300000.000000 sba =  70.300000  */
static const int16_t BSAT_AciCoeff_5[36] = {
   6,
   22,
   32,
   49,
   61,
   65,
   55,
   29,
   -14,
   -67,
   -121,
   -161,
   -172,
   -143,
   -70,
   42,
   174,
   299,
   382,
   391,
   306,
   120,
   -145,
   -446,
   -718,
   -888,
   -883,
   -648,
   -162,
   559,
   1456,
   2435,
   3377,
   4160,
   4678,
   4859
};


/* ACI for fb = 6000000.000000 fs = 4860.000000  */
/* ACI for ftrans = 3300000.000000 sba =  70.300000  */
static const int16_t BSAT_AciCoeff_6[36] = {
   -14,
   -21,
   -30,
   -32,
   -22,
   2,
   38,
   79,
   112,
   124,
   102,
   42,
   -48,
   -146,
   -224,
   -248,
   -196,
   -65,
   125,
   326,
   474,
   508,
   385,
   103,
   -293,
   -706,
   -1010,
   -1073,
   -793,
   -127,
   888,
   2133,
   3425,
   4555,
   5325,
   5599
};


/* ACI for fb = 7000000.000000 fs = 4860.000000  */
/* ACI for ftrans = 3300000.000000 sba =  70.300000  */
static const int16_t BSAT_AciCoeff_7[36] = {
   12,
   11,
   6,
   -9,
   -34,
   -63,
   -85,
   -86,
   -56,
   4,
   80,
   146,
   171,
   132,
   26,
   -122,
   -261,
   -332,
   -285,
   -108,
   161,
   436,
   606,
   572,
   291,
   -195,
   -747,
   -1163,
   -1228,
   -783,
   217,
   1666,
   3321,
   4854,
   5938,
   6328
};


/* ACI for fb = 8000000.000000 fs = 4860.000000  */
/* ACI for ftrans = 3300000.000000 sba =  70.300000  */
static const int16_t BSAT_AciCoeff_8[36] = {
   -6,
   6,
   23,
   45,
   65,
   68,
   45,
   -5,
   -68,
   -117,
   -122,
   -66,
   41,
   160,
   232,
   208,
   72,
   -138,
   -336,
   -417,
   -310,
   -17,
   364,
   664,
   711,
   407,
   -199,
   -901,
   -1376,
   -1302,
   -478,
   1072,
   3072,
   5060,
   6521,
   7058
};


/* ACI for fb = 9000000.000000 fs = 4860.000000  */
/* ACI for ftrans = 3300000.000000 sba =  70.300000  */
static const int16_t BSAT_AciCoeff_9[36] = {
   -5,
   -27,
   -45,
   -59,
   -51,
   -16,
   38,
   87,
   98,
   53,
   -39,
   -136,
   -181,
   -128,
   18,
   194,
   301,
   254,
   40,
   -255,
   -473,
   -462,
   -167,
   311,
   732,
   823,
   432,
   -358,
   -1199,
   -1599,
   -1122,
   388,
   2686,
   5167,
   7077,
   7793
};


/* ACI for fb = 10000000.000000 fs = 4860.000000  */
/* ACI for ftrans = 3300000.000000 sba =  70.300000  */
static const int16_t BSAT_AciCoeff_10[36] = {
   17,
   32,
   39,
   26,
   -13,
   -62,
   -92,
   -75,
   -5,
   83,
   133,
   96,
   -28,
   -173,
   -240,
   -157,
   58,
   286,
   366,
   202,
   -153,
   -494,
   -572,
   -259,
   327,
   843,
   895,
   299,
   -734,
   -1613,
   -1630,
   -330,
   2176,
   5171,
   7598,
   8530
};


/* ACI for fb = 11000000.000000 fs = 4860.000000  */
/* ACI for ftrans = 3300000.000000 sba =  70.300000  */
static const int16_t BSAT_AciCoeff_11[36] = {
   -14,
   -14,
   0,
   33,
   69,
   79,
   42,
   -32,
   -97,
   -98,
   -13,
   112,
   181,
   120,
   -60,
   -241,
   -270,
   -81,
   227,
   427,
   322,
   -80,
   -524,
   -647,
   -258,
   464,
   1008,
   859,
   -94,
   -1341,
   -1931,
   -1014,
   1575,
   5073,
   8080,
   9263
};


/* ACI for fb = 12000000.000000 fs = 4860.000000  */
/* ACI for ftrans = 3300000.000000 sba =  70.300000  */
static const int16_t BSAT_AciCoeff_12[36] = {
   7,
   -9,
   -36,
   -64,
   -65,
   -22,
   47,
   90,
   58,
   -41,
   -133,
   -125,
   7,
   172,
   219,
   68,
   -190,
   -336,
   -198,
   167,
   469,
   399,
   -75,
   -606,
   -697,
   -133,
   734,
   1155,
   564,
   -838,
   -1998,
   -1619,
   905,
   4876,
   8520,
   9994
};


/* ACI for fb = 13000000.000000 fs = 4860.000000  */
/* ACI for ftrans = 3300000.000000 sba =  70.300000  */
static const int16_t BSAT_AciCoeff_13[36] = {
   4,
   32,
   57,
   58,
   17,
   -47,
   -78,
   -32,
   64,
   122,
   63,
   -83,
   -183,
   -111,
   103,
   265,
   182,
   -123,
   -375,
   -284,
   143,
   526,
   434,
   -160,
   -744,
   -671,
   175,
   1097,
   1092,
   -186,
   -1823,
   -2100,
   193,
   4582,
   8920,
   10727
};


/* ACI for fb = 14000000.000000 fs = 4860.000000  */
/* ACI for ftrans = 3300000.000000 sba =  70.300000  */
static const int16_t BSAT_AciCoeff_14[36] = {
   -20,
   -49,
   -56,
   -23,
   39,
   68,
   23,
   -67,
   -103,
   -23,
   112,
   150,
   13,
   -180,
   -205,
   15,
   277,
   268,
   -73,
   -414,
   -335,
   179,
   607,
   399,
   -363,
   -891,
   -457,
   698,
   1362,
   503,
   -1422,
   -2418,
   -532,
   4195,
   9278,
   11465
};


/* ACI for fb = 15000000.000000 fs = 4860.000000  */
/* ACI for ftrans = 3300000.000000 sba =  70.300000  */
static const int16_t BSAT_AciCoeff_15[36] = {
   18,
   22,
   -4,
   -55,
   -82,
   -38,
   48,
   79,
   -2,
   -114,
   -109,
   46,
   182,
   105,
   -140,
   -261,
   -57,
   281,
   322,
   -69,
   -469,
   -329,
   302,
   686,
   224,
   -684,
   -907,
   84,
   1304,
   1099,
   -848,
   -2541,
   -1229,
   3727,
   9587,
   12198
};


/* ACI for fb = 16000000.000000 fs = 4860.000000  */
/* ACI for ftrans = 3300000.000000 sba =  70.300000  */
static const int16_t BSAT_AciCoeff_16[36] = {
   -9,
   9,
   48,
   74,
   38,
   -42,
   -73,
   1,
   99,
   77,
   -71,
   -157,
   -28,
   182,
   175,
   -102,
   -300,
   -95,
   309,
   351,
   -131,
   -541,
   -234,
   514,
   682,
   -156,
   -1006,
   -553,
   939,
   1501,
   -172,
   -2466,
   -1870,
   3187,
   9848,
   12929
};


/* ACI for fb = 17000000.000000 fs = 4860.000000  */
/* ACI for ftrans = 3300000.000000 sba =  70.300000  */
static const int16_t BSAT_AciCoeff_17[36] = {
   -2,
   -37,
   -67,
   -48,
   26,
   68,
   8,
   -85,
   -65,
   72,
   132,
   -12,
   -180,
   -94,
   174,
   226,
   -86,
   -339,
   -93,
   374,
   338,
   -273,
   -590,
   0,
   754,
   446,
   -718,
   -1022,
   352,
   1642,
   525,
   -2198,
   -2429,
   2584,
   10061,
   13662
};


/* ACI for fb = 18000000.000000 fs = 4860.000000  */
/* ACI for ftrans = 3300000.000000 sba =  70.300000  */
static const int16_t BSAT_AciCoeff_18[36] = {
   17,
   57,
   58,
   0,
   -60,
   -25,
   67,
   66,
   -58,
   -116,
   22,
   162,
   46,
   -189,
   -145,
   176,
   263,
   -107,
   -380,
   -31,
   467,
   242,
   -486,
   -518,
   396,
   840,
   -149,
   -1178,
   -323,
   1495,
   1162,
   -1756,
   -2884,
   1927,
   10226,
   14398
};


/* ACI for fb = 19000000.000000 fs = 4860.000000  */
/* ACI for ftrans = 3300000.000000 sba =  70.300000  */
static const int16_t BSAT_AciCoeff_19[36] = {
   -28,
   -47,
   -10,
   61,
   60,
   -28,
   -59,
   45,
   115,
   -3,
   -136,
   -23,
   183,
   95,
   -198,
   -170,
   209,
   282,
   -180,
   -401,
   119,
   543,
   2,
   -686,
   -190,
   833,
   480,
   -966,
   -920,
   1084,
   1657,
   -1173,
   -3209,
   1231,
   10340,
   15135
};


/* ACI for fb = 20000000.000000 fs = 4860.000000  */
/* ACI for ftrans = 3300000.000000 sba =  70.300000  */
static const int16_t BSAT_AciCoeff_20[36] = {
   15,
   -2,
   -59,
   -77,
   0,
   65,
   -5,
   -93,
   -10,
   121,
   31,
   -155,
   -63,
   192,
   107,
   -231,
   -167,
   272,
   246,
   -314,
   -349,
   354,
   484,
   -393,
   -665,
   428,
   917,
   -459,
   -1296,
   484,
   1944,
   -503,
   -3390,
   514,
   10401,
   15866
};


/* ACI for fb = 21000000.000000 fs = 4860.000000  */
/* ACI for ftrans = 3300000.000000 sba =  70.300000  */
static const int16_t BSAT_AciCoeff_21[36] = {
   -7,
   -49,
   -70,
   -17,
   57,
   30,
   -68,
   -56,
   78,
   93,
   -81,
   -140,
   75,
   198,
   -55,
   -265,
   15,
   339,
   49,
   -419,
   -145,
   502,
   281,
   -583,
   -473,
   660,
   744,
   -729,
   -1151,
   787,
   1834,
   -830,
   -3322,
   857,
   10378,
   15518
};


/* ACI for fb = 22000000.000000 fs = 4860.000000  */
/* ACI for ftrans = 3300000.000000 sba =  70.300000  */
static const int16_t BSAT_AciCoeff_22[36] = {
   28,
   39,
   -13,
   -77,
   -41,
   49,
   22,
   -92,
   -64,
   97,
   81,
   -130,
   -128,
   148,
   174,
   -177,
   -241,
   200,
   319,
   -226,
   -423,
   249,
   555,
   -272,
   -731,
   292,
   975,
   -310,
   -1343,
   324,
   1979,
   -335,
   -3412,
   341,
   10409,
   16041
};


/* ACI for fb = 23000000.000000 fs = 4860.000000  */
/* ACI for ftrans = 3300000.000000 sba =  70.300000  */
static const int16_t BSAT_AciCoeff_23[36] = {
   2,
   45,
   75,
   20,
   -59,
   -20,
   76,
   27,
   -102,
   -36,
   134,
   47,
   -176,
   -59,
   227,
   72,
   -291,
   -86,
   370,
   100,
   -470,
   -114,
   599,
   127,
   -770,
   -139,
   1008,
   150,
   -1370,
   -158,
   1999,
   164,
   -3424,
   -168,
   10413,
   16554
};


/* ACI for fb = 24000000.000000 fs = 4860.000000  */
/* ACI for ftrans = 3300000.000000 sba =  70.300000  */
static const int16_t BSAT_AciCoeff_24[36] = {
   -34,
   -68,
   -22,
   50,
   18,
   -69,
   -14,
   93,
   10,
   -126,
   2,
   164,
   -22,
   -209,
   55,
   259,
   -104,
   -315,
   175,
   373,
   -272,
   -433,
   405,
   491,
   -588,
   -546,
   847,
   595,
   -1235,
   -636,
   1898,
   667,
   -3361,
   -686,
   10391,
   17076
};


/* ACI for fb = 25000000.000000 fs = 4860.000000  */
/* ACI for ftrans = 3300000.000000 sba =  70.300000  */
static const int16_t BSAT_AciCoeff_25[36] = {
   4,
   -38,
   -79,
   -26,
   59,
   10,
   -79,
   6,
   105,
   -31,
   -132,
   71,
   155,
   -129,
   -170,
   206,
   166,
   -302,
   -137,
   415,
   70,
   -542,
   49,
   677,
   -235,
   -812,
   518,
   939,
   -953,
   -1049,
   1680,
   1134,
   -3224,
   -1188,
   10344,
   17590
};


/* ACI for fb = 26000000.000000 fs = 4860.000000  */
/* ACI for ftrans = 3300000.000000 sba =  70.300000  */
static const int16_t BSAT_AciCoeff_26[36] = {
   29,
   72,
   28,
   -51,
   -13,
   70,
   -8,
   -91,
   41,
   107,
   -90,
   -110,
   155,
   88,
   -230,
   -32,
   304,
   -67,
   -360,
   214,
   377,
   -406,
   -330,
   636,
   187,
   -888,
   88,
   1143,
   -556,
   -1375,
   1359,
   1562,
   -3015,
   -1683,
   10272,
   18109
};


/* ACI for fb = 27000000.000000 fs = 4860.000000  */
/* ACI for ftrans = 3300000.000000 sba =  70.300000  */
static const int16_t BSAT_AciCoeff_27[36] = {
   -12,
   26,
   83,
   34,
   -57,
   2,
   78,
   -37,
   -87,
   90,
   75,
   -152,
   -29,
   209,
   -59,
   -240,
   187,
   218,
   -338,
   -118,
   482,
   -79,
   -573,
   373,
   555,
   -747,
   -360,
   1166,
   -92,
   -1578,
   954,
   1927,
   -2739,
   -2161,
   10174,
   18628
};


/* ACI for fb = 28000000.000000 fs = 4860.000000  */
/* ACI for ftrans = 3300000.000000 sba =  70.300000  */
static const int16_t BSAT_AciCoeff_28[36] = {
   -24,
   -75,
   -36,
   51,
   9,
   -69,
   29,
   76,
   -81,
   -56,
   139,
   -5,
   -178,
   107,
   168,
   -232,
   -81,
   341,
   -93,
   -377,
   335,
   283,
   -588,
   -16,
   760,
   -428,
   -736,
   1009,
   380,
   -1643,
   494,
   2216,
   -2404,
   -2616,
   10052,
   19143
};


/* ACI for fb = 29000000.000000 fs = 4860.000000  */
/* ACI for ftrans = 3300000.000000 sba =  70.300000  */
static const int16_t BSAT_AciCoeff_29[36] = {
   24,
   -5,
   -81,
   -46,
   50,
   -17,
   -74,
   64,
   52,
   -122,
   13,
   151,
   -119,
   -114,
   230,
   -11,
   -286,
   208,
   219,
   -413,
   6,
   517,
   -364,
   -404,
   746,
   -1,
   -971,
   693,
   813,
   -1562,
   -3,
   2420,
   -2012,
   -3050,
   9903,
   19665
};


/* ACI for fb = 30000000.000000 fs = 4860.000000  */
/* ACI for ftrans = 3300000.000000 sba =  70.300000  */
static const int16_t BSAT_AciCoeff_30[36] = {
   19,
   75,
   43,
   -52,
   -5,
   67,
   -47,
   -52,
   104,
   -11,
   -128,
   115,
   77,
   -209,
   64,
   217,
   -252,
   -77,
   379,
   -201,
   -320,
   508,
   2,
   -647,
   515,
   419,
   -1011,
   274,
   1140,
   -1346,
   -491,
   2526,
   -1582,
   -3446,
   9733,
   20178
};


/* ACI for fb = 31000000.000000 fs = 4860.000000  */
/* ACI for ftrans = 3300000.000000 sba =  70.300000  */
static const int16_t BSAT_AciCoeff_31[36] = {
   -44,
   -42,
   53,
   44,
   -45,
   37,
   73,
   -76,
   3,
   126,
   -92,
   -63,
   192,
   -75,
   -167,
   255,
   -6,
   -310,
   290,
   138,
   -483,
   264,
   377,
   -672,
   129,
   749,
   -854,
   -197,
   1340,
   -1006,
   -960,
   2535,
   -1107,
   -3818,
   9535,
   20703
};


/* ACI for fb = 32000000.000000 fs = 4860.000000  */
/* ACI for ftrans = 3300000.000000 sba =  70.300000  */
static const int16_t BSAT_AciCoeff_32[36] = {
   -12,
   -74,
   -52,
   52,
   1,
   -63,
   62,
   22,
   -104,
   75,
   64,
   -161,
   76,
   134,
   -230,
   57,
   237,
   -307,
   4,
   383,
   -386,
   -99,
   588,
   -463,
   -286,
   886,
   -532,
   -626,
   1372,
   -585,
   -1358,
   2443,
   -619,
   -4142,
   9317,
   21214
};


/* ACI for fb = 33000000.000000 fs = 4860.000000  */
/* ACI for ftrans = 3300000.000000 sba =  70.300000  */
static const int16_t BSAT_AciCoeff_33[36] = {
   48,
   71,
   -28,
   -26,
   61,
   -34,
   -45,
   95,
   -45,
   -77,
   142,
   -55,
   -125,
   207,
   -65,
   -193,
   293,
   -75,
   -288,
   409,
   -85,
   -423,
   572,
   -93,
   -627,
   817,
   -100,
   -968,
   1244,
   -105,
   -1683,
   2255,
   -108,
   -4433,
   9073,
   21736
};


/* ACI for fb = 34000000.000000 fs = 4860.000000  */
/* ACI for ftrans = 3300000.000000 sba =  70.300000  */
static const int16_t BSAT_AciCoeff_34[36] = {
   2,
   68,
   64,
   -49,
   2,
   57,
   -73,
   11,
   85,
   -115,
   25,
   121,
   -177,
   54,
   164,
   -263,
   103,
   211,
   -381,
   186,
   259,
   -544,
   321,
   305,
   -778,
   548,
   347,
   -1152,
   966,
   379,
   -1898,
   1979,
   399,
   -4676,
   8809,
   22252
};


/* ACI for fb = 35000000.000000 fs = 4860.000000  */
/* ACI for ftrans = 3300000.000000 sba =  70.300000  */
static const int16_t BSAT_AciCoeff_35[36] = {
   -45,
   -78,
   24,
   27,
   -60,
   44,
   23,
   -90,
   85,
   10,
   -125,
   146,
   -26,
   -156,
   233,
   -98,
   -170,
   342,
   -223,
   -146,
   468,
   -417,
   -56,
   601,
   -708,
   149,
   727,
   -1160,
   575,
   831,
   -2000,
   1626,
   899,
   -4875,
   8524,
   22768
};


/* ACI for fb = 36000000.000000 fs = 4860.000000  */
/* ACI for ftrans = 3300000.000000 sba =  70.300000  */
static const int16_t BSAT_AciCoeff_36[36] = {
   14,
   -54,
   -79,
   40,
   -6,
   -52,
   80,
   -42,
   -48,
   123,
   -105,
   -15,
   158,
   -197,
   68,
   159,
   -304,
   215,
   89,
   -394,
   428,
   -92,
   -415,
   690,
   -428,
   -291,
   966,
   -985,
   109,
   1213,
   -1978,
   1205,
   1384,
   -5026,
   8213,
   23290
};


/* ACI for fb = 37000000.000000 fs = 4860.000000  */
/* ACI for ftrans = 3300000.000000 sba =  70.300000  */
static const int16_t BSAT_AciCoeff_37[36] = {
   41,
   85,
   -20,
   -28,
   60,
   -53,
   -1,
   73,
   -105,
   58,
   54,
   -155,
   156,
   -28,
   -160,
   268,
   -188,
   -67,
   335,
   -403,
   165,
   268,
   -601,
   542,
   -30,
   -653,
   1013,
   -662,
   -360,
   1482,
   -1837,
   745,
   1828,
   -5126,
   7890,
   23801
};


/* ACI for fb = 38000000.000000 fs = 4860.000000  */
/* ACI for ftrans = 3300000.000000 sba =  70.300000  */
static const int16_t BSAT_AciCoeff_38[36] = {
   -49,
   4,
   81,
   -21,
   25,
   56,
   -74,
   75,
   6,
   -92,
   147,
   -99,
   -31,
   179,
   -235,
   135,
   91,
   -309,
   363,
   -169,
   -196,
   515,
   -550,
   200,
   390,
   -867,
   861,
   -225,
   -801,
   1628,
   -1578,
   241,
   2247,
   -5178,
   7536,
   24330
};


/* ACI for fb = 39000000.000000 fs = 4860.000000  */
/* ACI for ftrans = 3300000.000000 sba =  70.300000  */
static const int16_t BSAT_AciCoeff_39[36] = {
   -34,
   -90,
   14,
   31,
   -60,
   59,
   -19,
   -47,
   104,
   -107,
   37,
   79,
   -177,
   186,
   -74,
   -117,
   284,
   -311,
   145,
   158,
   -439,
   513,
   -278,
   -198,
   680,
   -863,
   544,
   232,
   -1129,
   1624,
   -1231,
   -254,
   2599,
   -5176,
   7177,
   24838
};


/* ACI for fb = 40000000.000000 fs = 4860.000000  */
/* ACI for ftrans = 3300000.000000 sba =  70.300000  */
static const int16_t BSAT_AciCoeff_40[36] = {
   68,
   46,
   -51,
   44,
   -14,
   -34,
   78,
   -88,
   48,
   34,
   -122,
   164,
   -121,
   -5,
   163,
   -271,
   252,
   -84,
   -175,
   402,
   -461,
   277,
   112,
   -542,
   785,
   -654,
   113,
   667,
   -1335,
   1480,
   -799,
   -754,
   2906,
   -5125,
   6789,
   25362
};


/* ACI for fb = 41000000.000000 fs = 4860.000000  */
/* ACI for ftrans = 3300000.000000 sba =  70.300000  */
static const int16_t BSAT_AciCoeff_41[36] = {
   20,
   92,
   -2,
   -35,
   59,
   -64,
   37,
   18,
   -83,
   124,
   -112,
   38,
   78,
   -188,
   233,
   -172,
   8,
   204,
   -370,
   396,
   -233,
   -87,
   450,
   -689,
   654,
   -287,
   -334,
   989,
   -1375,
   1205,
   -324,
   -1217,
   3143,
   -5022,
   6390,
   25878
};


/* ACI for fb = 42000000.000000 fs = 4860.000000  */
/* ACI for ftrans = 3300000.000000 sba =  70.300000  */
static const int16_t BSAT_AciCoeff_42[36] = {
   -69,
   -55,
   53,
   -44,
   20,
   21,
   -65,
   94,
   -88,
   39,
   44,
   -132,
   187,
   -173,
   79,
   76,
   -240,
   342,
   -324,
   161,
   112,
   -409,
   610,
   -605,
   341,
   145,
   -715,
   1160,
   -1255,
   829,
   167,
   -1630,
   3310,
   -4873,
   5979,
   26390
};


/* ACI for fb = 43000000.000000 fs = 4860.000000  */
/* ACI for ftrans = 3300000.000000 sba =  70.300000  */
static const int16_t BSAT_AciCoeff_43[36] = {
   13,
   -84,
   -26,
   34,
   -59,
   68,
   -54,
   13,
   48,
   -108,
   145,
   -137,
   73,
   37,
   -163,
   263,
   -292,
   221,
   -51,
   -183,
   409,
   -547,
   523,
   -303,
   -86,
   556,
   -965,
   1151,
   -976,
   364,
   666,
   -1991,
   3408,
   -4668,
   5537,
   26921
};


/* ACI for fb = 44000000.000000 fs = 4860.000000  */
/* ACI for ftrans = 3300000.000000 sba =  70.300000  */
static const int16_t BSAT_AciCoeff_44[36] = {
   68,
   65,
   -55,
   44,
   -24,
   -8,
   49,
   -87,
   106,
   -96,
   50,
   28,
   -119,
   197,
   -234,
   206,
   -105,
   -56,
   242,
   -401,
   478,
   -428,
   233,
   86,
   -467,
   814,
   -1015,
   966,
   -597,
   -108,
   1097,
   -2257,
   3426,
   -4429,
   5106,
   27423
};


/* ACI for fb = 45000000.000000 fs = 4860.000000  */
/* ACI for ftrans = 3300000.000000 sba =  70.300000  */
static const int16_t BSAT_AciCoeff_45[36] = {
   -82,
   1,
   15,
   -37,
   59,
   -73,
   70,
   -46,
   1,
   59,
   -120,
   167,
   -182,
   152,
   -75,
   -44,
   182,
   -309,
   390,
   -393,
   297,
   -102,
   -169,
   470,
   -735,
   891,
   -869,
   619,
   -122,
   -601,
   1489,
   -2450,
   3370,
   -4132,
   4635,
   27957
};


const int16_t* BSAT_AciCoeff_Table[45] =
{
   BSAT_AciCoeff_1,
   BSAT_AciCoeff_2,
   BSAT_AciCoeff_3,
   BSAT_AciCoeff_4,
   BSAT_AciCoeff_5,
   BSAT_AciCoeff_6,
   BSAT_AciCoeff_7,
   BSAT_AciCoeff_8,
   BSAT_AciCoeff_9,
   BSAT_AciCoeff_10,
   BSAT_AciCoeff_11,
   BSAT_AciCoeff_12,
   BSAT_AciCoeff_13,
   BSAT_AciCoeff_14,
   BSAT_AciCoeff_15,
   BSAT_AciCoeff_16,
   BSAT_AciCoeff_17,
   BSAT_AciCoeff_18,
   BSAT_AciCoeff_19,
   BSAT_AciCoeff_20,
   BSAT_AciCoeff_21,
   BSAT_AciCoeff_22,
   BSAT_AciCoeff_23,
   BSAT_AciCoeff_24,
   BSAT_AciCoeff_25,
   BSAT_AciCoeff_26,
   BSAT_AciCoeff_27,
   BSAT_AciCoeff_28,
   BSAT_AciCoeff_29,
   BSAT_AciCoeff_30,
   BSAT_AciCoeff_31,
   BSAT_AciCoeff_32,
   BSAT_AciCoeff_33,
   BSAT_AciCoeff_34,
   BSAT_AciCoeff_35,
   BSAT_AciCoeff_36,
   BSAT_AciCoeff_37,
   BSAT_AciCoeff_38,
   BSAT_AciCoeff_39,
   BSAT_AciCoeff_40,
   BSAT_AciCoeff_41,
   BSAT_AciCoeff_42,
   BSAT_AciCoeff_43,
   BSAT_AciCoeff_44,
   BSAT_AciCoeff_45
};


/******************************************************************************
 BSAT_g1_P_ChanSetAciCoeff_isr()
******************************************************************************/
BERR_Code BSAT_g1_P_ChanSetAciCoeff_isr(BSAT_ChannelHandle h)
{
   BSAT_g1_P_ChannelHandle *hChn = (BSAT_g1_P_ChannelHandle *)h->pImpl;
   uint32_t i, f, Fb_mbaud, val;
   const int16_t *pCoeff;

   if (hChn->bOverrideAciBandwidth)
      f = hChn->aciBandwidth;
   else
      f = hChn->acqSettings.symbolRate;

   Fb_mbaud = (f + 500000) / 1000000;
   if (Fb_mbaud < 1)
      Fb_mbaud = 1;
   else if (Fb_mbaud > 45)
      Fb_mbaud = 45;

   if (hChn->bOverrideAciBandwidth == false)
      hChn->aciBandwidth = Fb_mbaud * 1000000;

   pCoeff = (const int16_t *)(BSAT_AciCoeff_Table[Fb_mbaud-1]);

   for (i = 0; i < 36; i++)
   {
      val = (uint32_t)pCoeff[i];
      BSAT_g1_P_WriteRegister_isrsafe(h, BCHP_STB_CHAN_CHx_ACI_H0 + (i*4), val);
   }
   return BERR_SUCCESS;
}

