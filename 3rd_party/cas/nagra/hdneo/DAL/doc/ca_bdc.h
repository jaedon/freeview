/**
**  @file ca_bdc.h
**
**  @brief
**    This file describes the NagraVision basic display control API.
**
**  @ingroup BDC
**
**  $Revision: 1.10 $
**
**  COPYRIGHT:
**   2003 Nagravision S.A.
*/
/**
 *  @defgroup BDC Basic Display Control
*/

#ifndef CA_BDC_H
#define CA_BDC_H

/******************************************************************************/
/*                                                                            */
/*                               INCLUDE FILES                                */
/*                                                                            */
/******************************************************************************/
#ifdef BDC_WRAP
#include "bdcwrap.h"
#endif /* BDC_WRAP */

#include "ca_defs.h"

/******************************************************************************/
/*                                                                            */
/*                            TYPES AND STRUCTURES                            */
/*                                                                            */
/******************************************************************************/
/**
 * @brief
 *    This type defines the status returned by BDC functions.
*/
typedef enum
{
  BDC_NO_ERROR,
  /**< The function call succeeded
  */
  BDC_ERROR
  /**< The function call failed
  */
} TBdcStatus;

/**
 *  @brief
 *    Deprecated. The ISO language code is no longer used. The BDC shall no 
 *    longer check it. Its definition is kept for backward compatibility only.
 *
 * @deprecated
*/
typedef TChar TBdcIsoLanguage[3];

/**
 * @brief
 *   Deprecated. BDC pop-ups are no longer subject to any particular persistence 
 *   requirements. They shall be managed as any other CA pop-ups.
 *
 * @deprecated
*/
typedef enum
{
  BDC_POP_UP_PERSISTENCE_ALWAYS,
  /**< Deprecated
  */
  BDC_POP_UP_PERSISTENCE_USER,
  /**< Deprecated
  */
  BDC_POP_UP_PERSISTENCE_TIMEOUT
  /**< Deprecated
   *    
  */
} TBdcPopUpPersistence;

/**
 * @brief
 *   This structure is used to define the content of a pop-up. All texts 
 *   displayed through the BDC are encoded with the same and single character 
 *   set. The choice of this character set is operator dependent.
*/
typedef struct
{
  TBdcIsoLanguage   language;
  /**< Deprecated. The BDC shall no longer check the language of a pop-up.
  */
  TSize             numBytes;
  /**< Number of bytes of pContent, terminator not included
  */
  TUnsignedInt8*   pContent;
  /**< Buffer containing the text to be displayed in the pop-up, terminated by 
   *   0.
  */
} TBdcPopUpContent;

/******************************************************************************/
/*                                                                            */
/*                         PUBLIC FUNCTION PROTOTYPES                         */
/*                                                                            */
/******************************************************************************/
/**
 * @brief
 *   This function displays a pop-up. 
 *
 * @param[in]   xPersistence
 *                Deprecated.
 *
 * @param[in]   xNumLanguages
 *                Deprecated. The multiple-language support is no longer 
 *                required. Consequently this parameter is always set to 1.
 *                
 * @param[in]   pxContent
 *                Content of the pop-up.
 *
 * @retval  BDC_NO_ERROR
 *            Successful
 *
 * @retval  BDC_ERROR
 *            Failure
 *
 * @remarks
 *   -# This function returns immediately. The text to be displayed  must be 
 *      copied within the function, since the calling task may free it once the 
 *      function has returned.
 *   .
 *   -# The pop-up displayed by means of this function shall be considered
 *      as top priority pop-up and shall overwrite any other standard pop-ups, 
 *      including the smartcard or access  pop-ups. Otherwise, BDC pop-ups 
 *      should be subject to the same policies than any other pop-ups.
 *   .
 *   -# When this function is called twice consecutively without calling 
 *     \c bdcRemovePopUp(), the first popup shall be replaced by the second one
 *     and none of the \c bdcDisplayPopUp() calls shall return \c BDC_ERROR.
 *   .   
 *   -# This function shall be compatible with the unix-like new line character 
 *      (Unicode code point U+000A) whose encoding depends on the character set 
 *      selected, e.g. \c 0x0A for ASCII-compatible systems, \c 0x000A in 
 *      UTF-16, etc.
 *   .   
 *   -# This function shall support line wrapping (i.e. continuing on a new line 
 *      when a line is full) so that the whole message is displayed on a 
 *      viewable window. Word wrapping (i.e. breaking lines between and not 
 *      within words) is recommended but not formally required.
 *   .
 *   -# This function shall be able to display a message requiring a maximum of 
 *      5 lines, including new line characters and line wrapping, and shall 
 *      support at least 40 displayable characters per line, a character being 
 *      possibly coded on several bytes.
 *
*/   
NAGRA_CA_API TBdcStatus bdcDisplayPopUp
(
        TBdcPopUpPersistence  xPersistence,
        TUnsignedInt8         xNumLanguages,
  const TBdcPopUpContent*     pxContent
);

/**
 * @brief
 *   This function removes the pop-up displayed through a call to
 *   \c bdcDisplayPopUp().
 *
 * @retval  BDC_NO_ERROR
 *            Successful
 *
 * @retval  BDC_ERROR
 *            Failure
 *
 * @remarks
 *   -# If no BDC pop-up is currently displayed (e.g. pop-up already removed by
 *      the user), the function shall return \c BDC_NO_ERROR without doing
 *      anything.
*/
NAGRA_CA_API TBdcStatus bdcRemovePopUp
(
  void
);

#endif /* CA_BDC_H */

/* END OF FILE */
