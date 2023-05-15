/*
  Copyright (C) 2006-2009 NXP B.V., All Rights Reserved.
  This source code and any compilation or derivative thereof is the proprietary
  information of NXP B.V. and is confidential in nature. Under no circumstances
  is this software to be  exposed to or placed under an Open Source License of
  any type without the expressed written permission of NXP B.V.
 *
 * \file          tmbslTDA18260_Instance.h
 *
 *                1
 *
 * \date          %modify_time%
 *
 * \brief         Describe briefly the purpose of this file.
 *
 * REFERENCE DOCUMENTS :
 *                TDA18260_Driver_User_Guide.pdf
 *
 * Detailed description may be added here.
 *
 * \section info Change Information
 *
*/

#ifndef _TMBSL_TDA18260_INSTANCE_H
#define _TMBSL_TDA18260_INSTANCE_H

#ifdef __cplusplus
extern "C"
{
#endif

/*============================================================================*/
/* Internal Prototypes:                                                       */
/*============================================================================*/

extern tmErrorCode_t TDA18260_AllocInstance(tmUnitSelect_t tUnit, pptmbslTDA18260_Object_t ppDrvObject);
extern tmErrorCode_t TDA18260_DeAllocInstance(tmUnitSelect_t tUnit);
extern tmErrorCode_t TDA18260_GetInstance(tmUnitSelect_t tUnit, pptmbslTDA18260_Object_t ppDrvObject);


#ifdef __cplusplus
}
#endif

#endif /* _TMBSL_TDA18260_INSTANCE_H */

