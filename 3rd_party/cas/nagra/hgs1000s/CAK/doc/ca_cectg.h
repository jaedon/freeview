/**
**  @file ca_cectg.h
**
**  @brief
**   CECT globals.
**
**   This file specifies an API that can be used to provided the CECT with
**   an area where global variables must be stored.<br>
**   For regular platforms, this API is implemented by the CECT itself on
**   static variables.<br>
**   For specific platform where static variables are not available, such
**   as a Trusted Execution Environment, the API can be overriden and mapped
**   on a platform-specific global storage.
**
**  $Id: $
**  $Change: $
**
** Copyright:
**   2015-2015 Nagravision S.A.
**
*/
/*
** REMARK:
**    Comments in this file use special tags to allow automatic API
**    documentation generation in HTML format, using the
**    GNU-General Public Licensed Doxygen tool.
**    For more information about Doxygen, please check www.doxygen.org
*/

#ifndef CA_CECTG_H
#define CA_CECTG_H

/******************************************************************************/
/*                                                                            */
/*                               INCLUDE FILES                                */
/*                                                                            */
/******************************************************************************/

#include "ca_defs.h"

/******************************************************************************/
/*                                                                            */
/*                              DEFINES                                       */
/*                                                                            */
/******************************************************************************/
/**
 * @brief
 *  Size in bytes of the global storage for the CECT.
*/
#define CECT_GLOBALS_SIZE        (512/sizeof(TUnsignedInt32))

/**
 * @brief
 *  Placeholder to store global variables of CECT.
 *
 * Data must be aligned on a structure.
*/
typedef struct
{
  TUnsignedInt32 globals[CECT_GLOBALS_SIZE];
} TCectGlobals;


/******************************************************************************/
/*                                                                            */
/*                              GLOBAL VARIABLES                              */
/*                                                                            */
/******************************************************************************/


/**
 * @brief
 *  This method provides the CECT with the location where its global (static)
 *  variables are placed.
 *
 * A default implementation is offered for regular platforms. For platforms
 * with no static storage, such as Trusted Execution Environment, the method
 * can be overriden on platform-specific global storage.<p>
 *
 * It is called each time the CECT needs to access its global variables.
 *
 * @remarks
 *  The provided buffer must be initialized to 0 at startup.
 *
 * @return
 *  A pointer to the location where the CECT can place its global variables.<br>
 *  The pointer must remain valid during the lifetime of the CECT.<br>
 *  CECT will not be usable if NULL is returned.
 */
TCectGlobals* pCectGetGlobals
(
  void
);

#endif /* CA_CECTG_H */

/* END OF FILE */
