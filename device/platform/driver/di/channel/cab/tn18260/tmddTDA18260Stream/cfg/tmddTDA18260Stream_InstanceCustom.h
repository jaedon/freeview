/*
  Copyright (C) 2006-2009 NXP B.V., All Rights Reserved.
  This source code and any compilation or derivative thereof is the proprietary
  information of NXP B.V. and is confidential in nature. Under no circumstances
  is this software to be  exposed to or placed under an Open Source License of
  any type without the expressed written permission of NXP B.V.
 *
 * \file          tmddTDA18260Stream_InstanceCustom.h
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

#ifndef _TMDD_TDA18260STREAM_INSTANCE_CUSTOM_H
#define _TMDD_TDA18260STREAM_INSTANCE_CUSTOM_H


#ifdef __cplusplus
extern "C"
{
#endif

/*============================================================================*/
/* Types and defines:                                                         */
/*============================================================================*/

/* Driver settings version definition */
#define TMDD_TDA18260STREAM_SETTINGS_CUSTOMER_NUM     0  /* SW Settings Customer Number */
#define TMDD_TDA18260STREAM_SETTINGS_PROJECT_NUM      0  /* SW Settings Project Number */
#define TMDD_TDA18260STREAM_SETTINGS_MAJOR_VER        0  /* SW Settings Major Version */
#define TMDD_TDA18260STREAM_SETTINGS_MINOR_VER        0  /* SW Settings Minor Version */

/* Custom Driver Instance Parameters: (Path 0) */
#define _TMDD_TDA18260STREAM_INSTANCE_CUSTOM_0                                                                          \
    0x40                                                     /* uBaseAddress  */                                        \
           
    /* Custom Driver Instance Parameters: (Path 1) */
#define _TMDD_TDA18260STREAM_INSTANCE_CUSTOM_1                                                                          \
    0x80                                                     /* uBaseAddress  */                                        \

    /* Custom Driver Instance Parameters: (Path 2) */
#define _TMDD_TDA18260STREAM_INSTANCE_CUSTOM_2                                                                          \
    0x40                                                     /* uBaseAddress  */                                        \

    /* Custom Driver Instance Parameters: (Path 3) */
#define _TMDD_TDA18260STREAM_INSTANCE_CUSTOM_3                                                                          \
    0x80                                                     /* uBaseAddress  */                                        \

    /* Custom Driver Instance Parameters: (Path 4) */
#define _TMDD_TDA18260STREAM_INSTANCE_CUSTOM_4                                                                          \
    0x40                                                     /* uBaseAddress  */                                        \

    /* Custom Driver Instance Parameters: (Path 5) */
#define _TMDD_TDA18260STREAM_INSTANCE_CUSTOM_5                                                                          \
    0x80                                                     /* uBaseAddress  */                                        \

#ifdef _TMDD_TDA18260_REGMAP_DEFINED
#define TMDD_TDA18260STREAM_INSTANCE_CUSTOM_REGMAP_0                                                                    \
    {                                   /* RegMap */                                                                    \
        { 0 }, { 0 }, { 0 }, { 0 }, { 0 }                                                                               \
    }

#define TMDD_TDA18260STREAM_INSTANCE_CUSTOM_REGMAP_1                                                                    \
    {                                   /* RegMap */                                                                    \
        { 0 }, { 0 }, { 0 }, { 0 }, { 0 }                                                                               \
    }

#define TMDD_TDA18260STREAM_INSTANCE_CUSTOM_REGMAP_2                                                                    \
    {                                   /* RegMap */                                                                    \
        { 0 }, { 0 }, { 0 }, { 0 }, { 0 }                                                                               \
    }

#define TMDD_TDA18260STREAM_INSTANCE_CUSTOM_REGMAP_3                                                                    \
    {                                   /* RegMap */                                                                    \
        { 0 }, { 0 }, { 0 }, { 0 }, { 0 }                                                                               \
    }

#define TMDD_TDA18260STREAM_INSTANCE_CUSTOM_REGMAP_4                                                                    \
    {                                   /* RegMap */                                                                    \
        { 0 }, { 0 }, { 0 }, { 0 }, { 0 }                                                                               \
    }

#define TMDD_TDA18260STREAM_INSTANCE_CUSTOM_REGMAP_5                                                                    \
    {                                   /* RegMap */                                                                    \
        { 0 }, { 0 }, { 0 }, { 0 }, { 0 }                                                                               \
    }

 #define TMDD_TDA18260STREAM_INSTANCE_CUSTOM_0 _TMDD_TDA18260STREAM_INSTANCE_CUSTOM_0,TMDD_TDA18260STREAM_INSTANCE_CUSTOM_REGMAP_0
 #define TMDD_TDA18260STREAM_INSTANCE_CUSTOM_1 _TMDD_TDA18260STREAM_INSTANCE_CUSTOM_1,TMDD_TDA18260STREAM_INSTANCE_CUSTOM_REGMAP_1
 #define TMDD_TDA18260STREAM_INSTANCE_CUSTOM_2 _TMDD_TDA18260STREAM_INSTANCE_CUSTOM_2,TMDD_TDA18260STREAM_INSTANCE_CUSTOM_REGMAP_2
 #define TMDD_TDA18260STREAM_INSTANCE_CUSTOM_3 _TMDD_TDA18260STREAM_INSTANCE_CUSTOM_3,TMDD_TDA18260STREAM_INSTANCE_CUSTOM_REGMAP_3
 #define TMDD_TDA18260STREAM_INSTANCE_CUSTOM_4 _TMDD_TDA18260STREAM_INSTANCE_CUSTOM_2,TMDD_TDA18260STREAM_INSTANCE_CUSTOM_REGMAP_4
 #define TMDD_TDA18260STREAM_INSTANCE_CUSTOM_5 _TMDD_TDA18260STREAM_INSTANCE_CUSTOM_3,TMDD_TDA18260STREAM_INSTANCE_CUSTOM_REGMAP_5
#else
 #define TMDD_TDA18260STREAM_INSTANCE_CUSTOM_0 _TMDD_TDA18260STREAM_INSTANCE_CUSTOM_0
 #define TMDD_TDA18260STREAM_INSTANCE_CUSTOM_1 _TMDD_TDA18260STREAM_INSTANCE_CUSTOM_1
 #define TMDD_TDA18260STREAM_INSTANCE_CUSTOM_2 _TMDD_TDA18260STREAM_INSTANCE_CUSTOM_2
 #define TMDD_TDA18260STREAM_INSTANCE_CUSTOM_3 _TMDD_TDA18260STREAM_INSTANCE_CUSTOM_3
 #define TMDD_TDA18260STREAM_INSTANCE_CUSTOM_4 _TMDD_TDA18260STREAM_INSTANCE_CUSTOM_4
 #define TMDD_TDA18260STREAM_INSTANCE_CUSTOM_5 _TMDD_TDA18260STREAM_INSTANCE_CUSTOM_5
#endif


#ifdef __cplusplus
}
#endif

#endif /* _TMDD_TDA18260STREAM_INSTANCE_CUSTOM_H */

