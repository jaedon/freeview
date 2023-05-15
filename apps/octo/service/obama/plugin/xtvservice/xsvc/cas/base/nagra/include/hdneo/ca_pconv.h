/**
**  @file ca_pconv.h
**
**  @brief
**    Pairing data conversion functions.
**
**  This tool provides primitives to convert former pairing data formats
**  stored on 12 or 94 bytes to newer format using up to 512 bytes.
**  It also offers a primitive to convert separated pairing data as specified
**  in the CAK V5 API into the newer format.
**
**  @ingroup CAK
**
**  $Revision: 1.8 $
**
**  COPYRIGHT:
**    © 2003 Nagravision S.A.
**
**  HISTORY:
**    $Log: ca_pconv.h,v $
**    Revision 1.8  2011/03/07 16:19:33  cuenin
**    PRMS ID: 66379, CCUBE-109, CCUBE-111
**    - Retrieve pairing data from persistent memory storage.
**
**    Revision 1.7  2006/08/31 15:58:29  dfarcy
**    PRMS ID: 34668
**    - Add NAGRA_CA_API before caConvertFrom12/96BytesPairingData APIs
**
**    Revision 1.6  2003/09/15 11:04:28  erard
**    PRMS ID: 20575
**    - Correct the parameters of caConvertPairingDataFrom50To61: the IRD serial
**      number is first, then the DNasp-2 DES pairing key.
**
**    Revision 1.5  2003/02/06 16:35:38  schyrr
**    PRMS ID: NONE
**    - Add missing const qualifiers.
**
**    Revision 1.4  2003/01/31 09:54:57  schyrr
**    PRMS ID: NONE
**    - Add comments.
**
**    Revision 1.3  2003/01/29 09:45:41  schyrr
**    PRMS ID: NONE
**    - Change parameters of caConvertPairingDataFrom50To61 to get rid of CAK V5
**      parameter format.
**
**    Revision 1.2  2003/01/23 10:20:45  schyrr
**    PRMS ID: NONE
**    - Rename file and methods following to the move to CAK component. File no
**      longer contains entries for the ca_cak.h generation.
**
**    Revision 1.1  2003/01/20 09:16:48  schyrr
**    PRMS ID: NONE
**    - Pairing conversion methods for CAK V5 and V6.0 to V6.1 pairing format.
**    - First archived version.
**
*/

#ifndef CA_PCONV_H
#define CA_PCONV_H

/******************************************************************************/
/*                                                                            */
/*                               INCLUDE FILES                                */
/*                                                                            */
/******************************************************************************/

#include "ca_defs.h"

/******************************************************************************/
/*                                                                            */
/*                                 DEFINITIONS                                */
/*                                                                            */
/******************************************************************************/

/******************************************************************************/
/*                                                                            */
/*                              TYPES & STRUCTURES                            */
/*                                                                            */
/******************************************************************************/


/******************************************************************************/
/*                                                                            */
/*                              PUBLIC FUNCTIONS                              */
/*                                                                            */
/******************************************************************************/

/**
 *  @brief
 *    Convert pairing data from 12 bytes pairing data format to newer extended
 *    format.
 *
 *  This function shall be used when using a CAK version > 6.0 on set-top boxes
 *  having pairing data on 12 bytes.
 *
 *  @pre
 *    None.
 *
 *  @post
 *    None.
 *
 *  @param  xPairingData
 *            Nagravision pairing data on 12 bytes.
 *  @param  ppxExtendedNaspPairingData
 *            Pointer to the pairing data in newer format.
 *
 *  @retval CA_NO_ERROR
 *            Success.
 *  @retval CA_ERROR
 *            Bad input data.
 *
 *  @remarks
 *    -# The data pointed to by ppxExtendedNaspPairingData remain valid during
 *       the application's lifetime.
 *    -# Successive calls to caConvertFrom12BytesPairingData overwrite the
 *       previously converted data.
 *
 *  @see TPairingDataImportation
 *
 *  @warning
 *    Do not use this function if your STB has pairing data on 94 bytes or more.
*/
NAGRA_CA_API TCaStatus caConvertFrom12BytesPairingData
(
  const TUnsignedInt8     xPairingData[12],
  const TUnsignedInt8**   ppxExtendedNaspPairingData
);


/**
 *  @brief
 *    Convert pairing data from 94 bytes pairing data format to newer extended
 *    format.
 *
 *  This function shall be used when using a CAK version > 6.0 on set-top boxes
 *  having pairing data on 94 bytes.
 *
 *  @pre
 *    None.
 *
 *  @post
 *    None.
 *
 *  @param  xPairingData
 *            Nagravision pairing data on 94 bytes.
 *  @param  ppxExtendedNaspPairingData
 *            Pointer to the pairing data in newer format.
 *
 *  @retval CA_NO_ERROR
 *            Success.
 *  @retval CA_ERROR
 *            Bad input data.
 *
 *  @remarks
 *    -# The data pointed to by ppxExtendedNaspPairingData remain valid during
 *       the application's lifetime.
 *    -# Successive calls to caConvertFrom94BytesPairingData overwrite the
 *       previously converted data.
 *
 *  @see TPairingDataImportation
 *
 *  @warning
 *    Do not use this function if your STB has pairing data on 12 bytes or more
 *    than 94 bytes.
*/
NAGRA_CA_API TCaStatus caConvertFrom94BytesPairingData
(
  const TUnsignedInt8     xPairingData[94],
  const TUnsignedInt8**   ppxExtendedNaspPairingData
);



/******************************************************************************/
/*                                                                            */
/*                                GLUE INTERNAL                               */
/*                                                                            */
/******************************************************************************/

#ifdef OPTION_SD_MERLIN
/**
 *  @brief
 *    Convert pairing data from 224 bytes pairing data format to newer extended
 *    format.
 *
 *  @param  xPairingData
 *            Nagravision pairing data on 224 bytes.
 *  @param  ppxExtendedNaspPairingData
 *            Pointer to the pairing data in newer format.
 *
 *  @retval CA_NO_ERROR
 *            Success.
 *  @retval CA_ERROR
 *            Bad input data.
*/
TCaStatus caConvertFrom224BytesPairingData
(
  const TUnsignedInt8     xPairingData[224],
  const TUnsignedInt8**   ppxExtendedNaspPairingData
);
#endif /* OPTION_SD_MERLIN */

/**
 *  @brief
 *    Convert pairing data from CAK V5.0 format to CAK V6.1 format.
 *
 *  This function is intended to the glue between CAK V6.1 and applications
 *  using CAK V5 API.
 *
 *  @pre
 *    None.
 *
 *  @post
 *    None.
 *
 *  @param  pxIrdSerialNb
 *            Pointer to the CA IRD serial number in CAK V5 format.
 *  @param  px50PairingKey
 *            Pointer to the pairing key in CAK V5 format.
 *  @param  ppx61NaspPairingData
 *            Pointer to the pairing data in CAK V6.1 format.
 *
 *  @retval CA_NO_ERROR
 *            Success.
 *  @retval CA_ERROR
 *            Bad input data.
 *
 *  @see TPairingDataImportation, TPairingKeyImportation
 *
 *  @internal
 *
 *  @remarks
 *    -# This function is intended for internal CAK use only.
 *    -# The data pointed to by ppx61NaspPairingData remain valid during
 *       the application's lifetime.
 *    -# Successive calls to caConvertPairingDataFrom50To61 overwrite the
 *       previously converted data.
*/
TCaStatus caConvertPairingDataFrom50To61
(
  const TUnsignedInt8*    pxIrdSerialNb,
  const TUnsignedInt8*    px50PairingKey,
  const TUnsignedInt8**   ppx61NaspPairingData
);


#endif /* CA_PCONV_H */

/* EOF */
