/*
  Copyright (C) 2006-2009 NXP B.V., All Rights Reserved.
  This source code and any compilation or derivative thereof is the proprietary
  information of NXP B.V. and is confidential in nature. Under no circumstances
  is this software to be  exposed to or placed under an Open Source License of
  any type without the expressed written permission of NXP B.V.
 *
 * \file          tmddTDA18260Shared_InstanceCustom.h
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

#ifndef _TMDD_TDA18260SHARED_INSTANCE_CUSTOM_H
#define _TMDD_TDA18260SHARED_INSTANCE_CUSTOM_H


#ifdef __cplusplus
extern "C"
{
#endif

/*============================================================================*/
/* Types and defines:                                                         */
/*============================================================================*/

/* Driver settings version definition */
#define TMDD_TDA18260SHARED_SETTINGS_CUSTOMER_NUM     0  /* SW Settings Customer Number */
#define TMDD_TDA18260SHARED_SETTINGS_PROJECT_NUM      0  /* SW Settings Project Number */
#define TMDD_TDA18260SHARED_SETTINGS_MAJOR_VER        0  /* SW Settings Major Version */
#define TMDD_TDA18260SHARED_SETTINGS_MINOR_VER        0  /* SW Settings Minor Version */

#ifdef _TMDD_TDA18260_REGMAP_DEFINED
#define TMDD_TDA18260SHARED_INSTANCE_CUSTOM_REGMAP_0                                                              \
    {                                   /* RegMap */                                                              \
        { 0 }, { 0 }, { 0 }, { 0 }, { 0 }                                                                         \
    }

#define TMDD_TDA18260SHARED_INSTANCE_CUSTOM_REGMAP_1                                                              \
    {                                   /* RegMap */                                                              \
        { 0 }, { 0 }, { 0 }, { 0 }, { 0 }                                                                         \
    }

 #define TMDD_TDA18260SHARED_INSTANCE_CUSTOM_0 _TMDD_TDA18260SHARED_INSTANCE_CUSTOM_0,TMDD_TDA18260SHARED_INSTANCE_CUSTOM_REGMAP_0
 #define TMDD_TDA18260SHARED_INSTANCE_CUSTOM_1 _TMDD_TDA18260SHARED_INSTANCE_CUSTOM_1,TMDD_TDA18260SHARED_INSTANCE_CUSTOM_REGMAP_1
#else
 #define TMDD_TDA18260SHARED_INSTANCE_CUSTOM_0 _TMDD_TDA18260SHARED_INSTANCE_CUSTOM_0
 #define TMDD_TDA18260SHARED_INSTANCE_CUSTOM_1 _TMDD_TDA18260SHARED_INSTANCE_CUSTOM_1
#endif


#ifdef __cplusplus
}
#endif

#endif /* _TMDD_TDA18260SHARED_INSTANCE_CUSTOM_H */

