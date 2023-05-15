/**
**  @file ca_types.h
**
**  @brief
**    This file defines the general types and constants used by the
**    NagraVision embedded software.
**
**  $Id: //CAK/components/glbapi/TAGS/GLBAPI_1_1_1/src/ca_types.h#1 $
**
**  COPYRIGHT:
**    2014 Nagravision S.A.
**
*/


#ifndef CA_TYPES_H
#define CA_TYPES_H

/******************************************************************************/
/*                                                                            */
/*                           GENERAL INCLUDE FILES                            */
/*                                                                            */
/******************************************************************************/

/* Grab size_t */
#include <stddef.h>


/******************************************************************************/
/*                                                                            */
/*                            CAT TYPES DEFINITIONS                           */
/*                                                                            */
/******************************************************************************/

/**
 *  @brief
 *    Signed 8-bit type.
*/
typedef signed char           TSignedInt8;

/**
 *  @brief
 *    Signed 16-bit type.
*/
typedef signed short int      TSignedInt16;

/**
 *  @brief
 *    Signed 32-bit type.
*/
typedef signed long int       TSignedInt32;


/**
 *  @brief
 *    Unsigned 8-bit type.
*/
typedef unsigned char         TUnsignedInt8;

/**
 *  @brief
 *    Unsigned 16-bit type.
*/
typedef unsigned short int    TUnsignedInt16;

/**
 *  @brief
 *    Unsigned 32-bit type.
*/
typedef unsigned long int     TUnsignedInt32;

/**
 *  @brief
 *    Character type
*/
typedef char                  TChar;

/**
 *  @brief
 *    Size type
*/
typedef size_t                TSize;

/**
   *  @brief
   *    Boolean Type.
  */
typedef TUnsignedInt8         TBoolean;

/**
 * @brief
 *   The evaluation of a condition is false
 * @see TBoolean
*/
#ifndef FALSE
#define FALSE      0
#endif

/**
 * @brief
 *   The evaluation of a condition is true
 * @see TBoolean
*/
#ifndef TRUE
#define TRUE       (!FALSE)
#endif


#endif /* CA_TYPES_H */

/* END OF FILE */
