/**
**  @file nocs1x_csd.h
**
**  @brief
**    Chipset Security Driver API for NOCS 1.x Secure chipset
**
**  @ingroup
**    CSD
**
**  @author
**    NOCS team
**
**  @version
**    Revision: 1.4
**
** COPYRIGHT:
**   2010 NagraVision S.A.
**
** HISTORY:
**   $Log: __________ $
**   Revision 1.0 2008/05/22 11:30:00 Nocs team
**   First published version
**   
**	 Revision 1.1 2009/03/18 14:00:00 Nocs team
**	 Change the name of LAST_CSD_DEBUG_INTERFACE_ACCESSS_MODE to
**	 LAST_CSD_DEBUG_INTERFACE_ACCESS_MODE
**	 
**   Revision 1.2 2009/07/09 09:30:00 Nocs team
**   Adding the functions csdGetCSCDCheckNumber(), csdGetSTBCASNCheckNumber()
**   and csdGetDataIntegrityCheckNumber(). Rename the function 
**   csdGetCheckNumber() to csdGetNUIDCheckNumber().
**
**   Revision 1.3 2010/03/29 09:30:00 Nocs team
**   Adding EMI type, updated the following API: csdSetClearTextDscHostKeys(),
**   csdSetProtectedDscContentKeys(), csdSetClearTextScrDecryptionKey(), 
**   csdSetClearTextScrEncryptionKey().
**   Added API function csdSetProtectedScrDecryptionKey().
**
**   Revision 1.4 2010/05/27 09:30:00 Nocs team
**   Added csdSetProtectedScrEncryptionKey() API function
**   Corrected the type of the xScrCipheredProtectingKeys parameter from 
**   csdSetProtectedScrDecryptionKey() functions to TCsdScrCipheredProtectingKeys.
**
**   Revision 1.5 2010/06/24 09:30:00 Nocs team
**   Corrected the TCsdDscKeyPathHandle to TCsdScrKeyPathHandle on function csdSetProtectedScrDecryptionKey()
**   Corrected the TCsdDscKeyPathHandle to TCsdScrKeyPathHandle on function csdSetProtectedScrEncryptionKey()
*/
/*
** REMARK:
**    Comments in this file use special tags to allow automatic API
**    documentation generation in HTML format, using the
**    GNU-General Public Licensed Doxygen tool.
**    For more information about Doxygen, please check www.doxygen.org
*/
/**
 * @defgroup
 *    CSD Chipset Security Driver
 * @brief
 *    API to access security features of a given chipset
 *
 * This component is the interface to the security features of a given
 * chipset.
*/

#ifndef CSD_H
#define CSD_H

/******************************************************************************/
/*                                                                            */
/*                               INCLUDE FILES                                */
/*                                                                            */
/******************************************************************************/

#include <stddef.h>

/******************************************************************************/
/*                                                                            */
/*                              TYPES DEFINITIONS                             */
/*                                                                            */
/******************************************************************************/

/**
 *  @brief
 *    Boolean Type.
*/
typedef unsigned char TCsdBoolean;

#ifndef FALSE
/**
 *  @brief
 *    The boolean FALSE value.
*/
#define FALSE      0
#endif

#ifndef TRUE
/**
 *  @brief
 *    The boolean TRUE value.
*/
#define TRUE        (!FALSE)
#endif

/**
 *  @brief
 *    Define the parameters for the initialization of the Chipset Security
 *    Driver.
 *
 *  Defined as an opaque pointer. Each implementation shall define its own
 *  proper structure according to its needs in the nocs1x_csd_impl.h file.
*/
/* SCsdInitParameters has to be defined in nocs1x_csd_impl.h */
typedef struct SCsdInitParameters TCsdInitParameters;

/**
 *  @brief
 *    Define the parameters for the termination of the Chipset Security Driver.
 *
 *  Defined as an opaque pointer. Each implementation shall define its own
 *  proper structure according to its needs in the nocs1x_csd_impl.h file.
*/
/* SCsdTerminateParameters has to be defined in nocs1x_csd_impl.h */
typedef struct SCsdTerminateParameters TCsdTerminateParameters;

/**
 *  @brief
 *    20 characters strings.
 *
 *  Defines a string buffer of 20 characters.
*/
typedef unsigned char TCsd20CharsString[20];

/**
 *  @brief
 *    32 bits vectors.
 *
 *  TCsd4BytesVector defines a 32 bits vector for the MarketSegmentID,
 *  CheckNumber and NUID values. This value can be converted in integer type
 *  according to the platform architecture.
*/
typedef unsigned char TCsd4BytesVector[4];

/**
 *  @brief
 *    Define the set of available memory interface during the chipset boot.
 *
 *  TCsdBootMode is defined as an opaque pointer. Each implementation shall
 *  define its own set of available memory interface from which the chipset is
 *  able to boot according to its capabilities. TCsdBootMode must be defined in
 *  the nocs1x_csd_impl.h file.
*/
/* TCsdBootMode has to be defined in nocs1x_csd_impl.h */
typedef enum ECsdBootMode TCsdBootMode;

/**
 *  @brief
 *    Size of SCS_Total_Area.
 *
 *  Defines the size of the SCS_Total_Area as defined in NOCS 1.X.
*/
typedef enum
{
  CSD_SCS_TOTAL_AREA_SIZE_16K = 0,
    /**< The total area size checked by SCS is of 16 Kbytes.
     */
  CSD_SCS_TOTAL_AREA_SIZE_32K,
    /**< The total area size checked by SCS is of 32 Kbytes.
     */
  CSD_SCS_TOTAL_AREA_SIZE_64K,
    /**< The total area size checked by SCS is of 64 Kbytes.
     */
  CSD_SCS_TOTAL_AREA_SIZE_128K,
    /**< The total area size checked by SCS is of 128 Kbytes.
     */
  CSD_SCS_TOTAL_AREA_SIZE_256K,
    /**< The total area size checked by SCS is of 256 Kbytes.
     */
  CSD_SCS_TOTAL_AREA_SIZE_UNPROGRAMMED,
    /**< The total area size checked by SCS is not programmed.
     */
  CSD_SCS_TOTAL_AREA_SIZE_NOT_SUPPORTED,
    /**< The total area size checked by SCS is not supported.
     */
  LAST_CSD_SCS_TOTAL_AREA_SIZE
    /**< Upper boundary of the TCsdScsTotalAreaSize type.
     */
}TCsdScsTotalAreaSize;

/**
 *  @brief
 *    Defines the state of the SCS Activation Flag.
 *
 *  Defines the state of the SCS Activation Flag.
*/
typedef enum
{
  CSD_SCS_ACTIVATION_FLAG_SET = 0,
  CSD_SCS_ACTIVATION_FLAG_NOT_SET,
  LAST_CSD_SCS_ACTIVATION_FLAG
}TCsdScsActivationFlag;

/**
 *  @brief
 *    Defines the state of the Key Ladder Deactivation Flag.
 *
 *  Defines the state of the Key Ladder Deactivation Flag.
*/
typedef enum
{
  CSD_KEY_LADDER_DEACTIVATION_FLAG_SET = 0,
  CSD_KEY_LADDER_DEACTIVATION_FLAG_NOT_SET,
  LAST_CSD_KEY_LADDER_DEACTIVATION_FLAG
}TCsdKeyLadderDeactivationFlag;

/**
 *  @brief
 *    Define the debug interface access mode.
 *
 *  Defines the Debug Interface Access Mode as specify in NOCS 1.x.
*/
typedef enum
{
  CSD_DEBUG_INTERFACE_ACCESS_MODE_OPEN = 0,
    /**< The debug interface is completly open without access protection.
     */
  CSD_DEBUG_INTERFACE_ACCESS_MODE_PASSWORD_PROTECTED,
    /**< The debug interface is protected by a password which has to be checked
         to unlock the interface.
     */
  CSD_DEBUG_INTERFACE_ACCESS_MODE_CLOSED,
    /**< The debug interface is completly closed and debug access is impossible.
      */
  CSD_DEBUG_INTERFACE_ACCESS_MODE_INCONSISTENT_STATE,
    /**< The debug interface has an inconsistent protection state.
     */
  LAST_CSD_DEBUG_INTERFACE_ACCESS_MODE
    /**< Upper boundary of the TCsdDebugInterfaceAccessMode type.
     */
}TCsdDebugInterfaceAccessMode;

/**
 *  @brief
 *    Define the byte as an 8 bits vector.
 *
 *  Definition of the byte as an 8 bits vector. This type is the smallest piece
 *  of data in the API.
*/
typedef unsigned char TCsdUnsignedInt8;

/**
 *  @brief
 *    Define a type as an 16 bits vector.
 *
 *  Definition of a type as an 16 bits vector. 
*/
typedef unsigned short int TCsdUnsignedInt16;

/**
 *  @brief
 *    Define the Size data type.
 *
 *  Re-definition of the size from the size_t defined in the stddefs.h file.
*/
typedef size_t TCsdSize;

/**
 *  @brief
 *    Define the cryptographic algorithm supported by Nagra for the RAM2RAM
 *    Crypto Engine operations.
 *
*/
typedef enum
{
  CSD_R2R_ALGORITHM_AES_128 = 0,
    /**< AES 128 bits defined in FIPS-197, Advanced Encryption Standard.
     */
  CSD_R2R_ALGORITHM_TDES_K1K2K1,
    /**< TDES algorithm in key option 2 defined in FIPS-46-3, Data
         Encryption Standard (DES).
     */
  LAST_CSD_R2R_ALGORITHM
    /**< Upper boundary of the TCsdAlgorithm type.
     */
}TCsdR2RAlgorithm;

/**
 *  @brief
 *    Define the cryptographic algorithm mode supported by Nagra for the data
 *    operations.
 *
 *  Define the cryptographic algorithm mode supported by Nagra for the data
 *  operations except the received transport stream processing.
*/
typedef enum
{
  CSD_R2R_CRYPTO_OPERATION_MODE_ECB = 0,
    /**< Electronic Code Book mode defined in FIPS-
     */
  CSD_R2R_CRYPTO_OPERATION_MODE_CBC,
  LAST_CSD_R2R_CRYPTO_OPERATION_MODE
}TCsdR2RCryptoOperationMode;

/**
 *  @brief
 *   TCsdScrKeyPathHandle holds all relevant information needed by the CSD in
 *   order to set the key of the Stream Crypto Engine into the appropriated
 *   location. It is an opaque type definition. The chipset manufacturer has to
 *   define it in the nocs1x_csd_impl.h file.
*/
typedef struct SCsdScrKeyPathHandle TCsdScrKeyPathHandle;

/**
 *  @brief
 *   TCsdDscKeyPathHandle holds all relevant information needed by the CSD
 *   in order to set the key, used by the Transport Stream Crypto Engine, into
 *   the appropriated location. It is an opaque type definition. The chipset
 *   manufacturer has to define it in the nocs1x_csd_impl.h file.
*/
typedef struct SCsdDscKeyPathHandle TCsdDscKeyPathHandle;

/**
 *  @brief
 *   TCsdR2RKeyPathHandle holds all relevant information needed by the CSD
 *   in order to set the key, used by the RAM2RAM Crypto Engine, into the
 *   appropriated location. It is an opaque type definition. The chipset
 *   manufacturer has to define it in the nocs1x_csd_impl.h file.
*/
typedef struct SCsdR2RKeyPathHandle TCsdR2RKeyPathHandle;

/**
 *  @brief
 *   TCsdR2RCipheredProtectingKeys defines an array of 16-bytes R2R Ciphered
 *   Protecting Keys.
 *   CSD_R2R_NB_OF_PROTECTING_KEYS defines the number of Ciphered Protecting
 *   Keys for the Key Ladder. The default value is set to 0. If the chipset key
 *   ladder has more Key Ladder Elements than the default one, the chipset
 *   manufacturer has to define CSD_R2R_NB_OF_PROTECTING_KEYS in the
 *   nocs1x_csd_impl.h file.
*/
#ifndef CSD_R2R_NB_OF_PROTECTING_KEYS
#define CSD_R2R_NB_OF_PROTECTING_KEYS 0
#endif

#if CSD_R2R_NB_OF_PROTECTING_KEYS == 0
typedef void* TCsdR2RCipheredProtectingKeys;
#define NO_PROTECTING_KEYS NULL
#else
typedef TCsdUnsignedInt8 TCsdR2RCipheredProtectingKeys \
          [CSD_R2R_NB_OF_PROTECTING_KEYS][16];
#endif

/**
 *  @brief
 *   TCsdDscCipheredProtectingKeys defines an array of 16-bytes DSC Ciphered
 *   Protecting Keys.
 *   CSD_DSC_NB_OF_PROTECTING_KEYS defines the number of Ciphered Protecting
 *   Keys for the Transport Stream Crypto Engine Key Ladder. The default value
 *   is set to 1. If the chipset key ladder has more Key Ladder Elements than
 *   the default one, the chipset manufacturer has to define
 *   CSD_DSC_NB_OF_PROTECTING_KEYS in the nocs1x_csd_impl.h file.
*/
#ifndef CSD_DSC_NB_OF_PROTECTING_KEYS
#define CSD_DSC_NB_OF_PROTECTING_KEYS 1
#endif

typedef TCsdUnsignedInt8 TCsdDscCipheredProtectingKeys \
          [CSD_DSC_NB_OF_PROTECTING_KEYS][16];

/**
 *  @brief
 *   TCsdScrCipheredProtectingKeys defines an array of 16-bytes SCR Ciphered
 *   Protecting Keys.
 *   CSD_SCR_NB_OF_PROTECTING_KEYS defines the number of Ciphered Protecting
 *   Keys for the Stream Crypto Engine Key Ladder. The default value
 *   is set to 1. If the chipset key ladder has more Key Ladder Elements than
 *   the default one, the chipset manufacturer has to define
 *   CSD_DSC_NB_OF_PROTECTING_KEYS in the nocs1x_csd_impl.h file.
*/
#ifndef CSD_SCR_NB_OF_PROTECTING_KEYS
#define CSD_SCR_NB_OF_PROTECTING_KEYS 1
#endif

typedef TCsdUnsignedInt8 TCsdScrCipheredProtectingKeys \
          [CSD_SCR_NB_OF_PROTECTING_KEYS][16];

/*
 * EXTERNAL_CSD_STATUS_OFFSET could be defined in nocs1x_csd_impl.h and so,
 * allows the chipset manufacturer to define an offset on the lowest status
 * value returned by the CSD driver to specify its own return status code range.
*/
#ifdef EXTERNAL_CSD_STATUS_OFFSET
#define CSD_STATUS_OFFSET   EXTERNAL_CSD_STATUS_OFFSET
#else
#define CSD_STATUS_OFFSET   0
#endif

/**
 *  @brief
 *    Return values of the CSD functions.
 *
*/
typedef enum
{
  CSD_NO_ERROR = CSD_STATUS_OFFSET,
    /**< The intended operation was executed successfully.
     */
  CSD_ERROR,
    /**< The function terminated abnormally. The intended operation failed.
     */
  CSD_ERROR_DRIVER_ALREADY_INITIALIZED,
    /**<  The CSD is already initialized.
     */
  CSD_ERROR_INVALID_PARAMETERS,
    /**< The passed parameters are invalid.
     */
  CSD_ERROR_OPERATION_NOT_ALLOWED,
    /**< The requested operation is not allowed.
     */
  CSD_ERROR_OPERATION_NOT_SUPPORTED,
    /**< The requested operation is not supported by the chipset.
     */
  CSD_ERROR_INVALID_KEY_PATH_HANDLE,
    /**< The handle used for the cryptographic operation are invalid.
     */
  LAST_CSD_STATUS
    /**< TCsdStatus type upper boundary.
     */
} TCsdStatus;




/******************************************************************************/
/*                                                                            */
/*                             FUNCTION PROTOTYPES                            */
/*                                                                            */
/******************************************************************************/


/**
 *  @brief
 *    Initialize the CSD.
 *
 *  @pre
 *    None.
 *
 *  @post
 *    None.
 *
 *  @param[in,out]  pxInitParameters
 *                   Pointer to the location where to find the initialization
 *                   parameters of the Chipset Security Driver.
 *
 *  @retval   CSD_NO_ERROR
 *              Initialization was completed successfully.
 *  @retval   CSD_ERROR_DRIVER_ALREADY_INITIALIZED
 *              CSD has been already initialized.
 *  @retval   CSD_ERROR_INVALID_PARAMETERS
 *              Structure pointed by pxInitParameters is invalid or corrupted.
 *  @retval   CSD_ERROR
 *              The function terminated abnormally. The intended operation
 *              failed.
 *
 *
 *
 *  This function has to be called once before any other function of the CSD
 *  API. This function initializes the software module that implements the API,
 *  which can in turn initialize and configure hardware.
 *  pxInitParameters is an opaque pointer. Therefore this structure has to be
 *  set prior to any initialization of CSD driver. In the case where external
 *  drivers would be used by the CSD, these drivers shall be initialized outside
 *  the CSD and before any calls to the CSD API.
*/
TCsdStatus csdInitialize
(
  TCsdInitParameters*  pxInitParameters
);

/**
 *  @brief
 *    Terminate and clean CSD internal structures.
 *
 *  @param[in,out]  pxTerminateParameters
 *                   Pointer to the location where to find the termination
 *                   parameters of the Chipset Security Driver.
 *
 *  @retval   CSD_NO_ERROR
 *              Termination was completed successfully.
 *  @retval   CSD_ERROR_INVALID_PARAMETERS
 *              Structure pointed by pxTerminateParameters is invalid or
 *              corrupted.
 *  @retval   CSD_ERROR
 *              The function terminated abnormally. The intended operation
 *              failed.
 *
 *  This function terminates operations of the software that implements the CSD
 *  API. Among others, the function shall free all resources previously
 *  allocated by the CSD API.
*/
TCsdStatus csdTerminate
(
  TCsdTerminateParameters*  pxTerminateParameters
);

/**
 *  @brief
 *    Return the current version of the CSD
 *
 *  @param[out]     xSoftwareVersion
 *                   20 characters string where the function has to write the
 *                   software version string.
 *
 *  @retval   CSD_NO_ERROR
 *              The intended operation was executed successfully.
 *  @retval   CSD_ERROR
 *              The function terminated abnormally. The intended operation
 *              failed.
 *
 *  This function returns the version of the CSD. It is
 *  represented as a character string, the internal format being chipset
 *  manufacturer dependent. However, the length of the string is fixed at 20
 *  characters. The string must be terminated by the character '\0'.
*/
TCsdStatus csdGetSoftwareVersion
(
  TCsd20CharsString  xSoftwareVersion
);

TCsdStatus csdGetChipExtension(
  TCsd20CharsString xChipExtension
);

/**
 * @brief
 *   Return the current revision of the chipset
 *
 *  @param[out]     xChipRevision
 *                   20 characters string where the function has to write the
 *                   chipset revision string.
 *
 *  @retval   CSD_NO_ERROR
 *              The intended operation was executed successfully.
 *  @retval   CSD_ERROR
 *              The function terminated abnormally. The intended operation
 *              failed.
 *
 *  This function returns the revision (also called die version) for a given
 *  chipset. Revision may include major and minor revision information. It is
 *  represented as a character string, the internal format being chipset
 *  manufacturer dependent. However, this internal format has to be agreed with
 *  Nagravision and the chipset manufacturer. In any case, the size of the
 *  string returned by the function is fixed at 20 characters. The string must
 *  be terminated by the character '\0'.
*/
TCsdStatus csdGetChipRevision
(
  TCsd20CharsString xChipRevision
);

/**
 *  @brief
 *    Get the chip Nagra Unique Id
 *
 *  @param[out]     xNuid
 *                   4-bytes vector where the function has to write the NUID.
 *
 *  @retval   CSD_NO_ERROR
 *              The intended operation was executed successfully.
 *  @retval   CSD_ERROR
 *              The function terminated abnormally. The intended operation
 *              failed.
 *
 *  This function returns the NagraUniqueID32 (NUID) that
 *  is programmed into the on-chip non-modifiable memory by the chipset
 *  manufacturer.
*/
TCsdStatus csdGetNuid
(
  TCsd4BytesVector xNuid
);

/**
 *  @brief
 *    Return the NAGRA NUID check number of the chipset.
 *
 *  @param[out]     xNUIDCheckNumber
 *                   4-bytes vector where the function has to write the NUID
 *                   Check Number.
 *
 *  @retval   CSD_NO_ERROR
 *              The intended operation was executed successfully.
 *  @retval   CSD_ERROR
 *              The function terminated abnormally. The intended operation
 *              failed.
 *
 *  This function returns the NUID Check Number, as requested by the Nagravision
 *  NUID Reporting procedure [2]. The computation of NUID Check Number is
 *  chipset-specific, and is the result of a computation involving the
 *  R2R-RootKey128, the NagraUniqueID32 as well as the RAM2RAM Crypto Engine. As
 *  there may be several ways of configuring the RAM2RAM Crypto Engine and its
 *  Key Ladder to compute the NUID Check Number, Nagravision and the chipset
 *  manufacturer will commonly agree during the NOCS certification process of a
 *  chipset on how to compute the NUID Check Number for this secure chipset.
 *  Refer to the "Implementation Proposal for CSD & CTD APIs v2.0" document [10]
 *  for more details about this computation.
*/
TCsdStatus csdGetNUIDCheckNumber
(
  TCsd4BytesVector xNUIDCheckNumber
);

/**
 *  @brief
 *    Return the CSC Data check number of the chipset.
 *
 *  @param[in]      xCSCData
 *                   16-bytes vector where the function has to 
 *					         read the Data used to compute the check number.
 *  @param[out]     xCSCDCheckNumber
 *                   4-bytes vector where the function has to write the CSC
 *                   Data Check Number.
 *
 *  @retval   CSD_NO_ERROR
 *              The intended operation was executed successfully.
 *  @retval   CSD_ERROR_INVALID_PARAMETERS,
 *              The passed parameter is invalid.
 *  @retval   CSD_ERROR
 *              The function terminated abnormally. The intended operation
 *              failed.
 *
 *  This function returns the CSC Data Check Number, as requested by the 
 *	Nagravision NUID Reporting procedure [2]. The computation of CSC Data 
 *	Check Number is chipset-specific, and is the result of a computation 
 *	involving the R2R-RootKey128, a 16-bytes data as well as the RAM2RAM Crypto 
 *  Engine. As there may be several ways of configuring the RAM2RAM Crypto 
 *	Engine and its Key Ladder to compute the CSC Data Check Number, Nagravision 
 *	and the chipset manufacturer will commonly agree during the NOCS 
 *	certification	process of a chipset on how to compute the CSC Data Check 
 *	Number for this secure chipset. Refer to the “Implementation Proposal for 
 *	CSD & CTD APIs v2.0” document [10] for more details about this computation.
*/
TCsdStatus csdGetCSCDCheckNumber
(
  const TCsdUnsignedInt8    xCSCData[16],
        TCsd4BytesVector    xCSCDCheckNumber
);

/**
 *  @brief
 *    Return the STBCASN check number of the chipset.
 *
 *  @param[in]      xSTBCASNData
 *                   4-bytes vector where the function has to 
 *					         read the Data used to compute the check number.
 *  @param[out]     xSTBCASNCheckNumber
 *                   4-bytes vector where the function has to write the STB CA
 *                   Check Number.
 *
 *  @retval   CSD_NO_ERROR
 *              The intended operation was executed successfully.
 *  @retval   CSD_ERROR_INVALID_PARAMETERS,
 *              The passed parameter is invalid.
 *  @retval   CSD_ERROR
 *              The function terminated abnormally. The intended operation
 *              failed.
 *
 *  This function returns the STBCASN Check Number. The computation of STBCASN 
 *	Check Number is chipset-specific, and is the result of a computation 
 *	involving the R2R-RootKey128, a 4-bytes data as well as the RAM2RAM Crypto 
 *	Engine. As there may be several ways of configuring the RAM2RAM Crypto 
 *	Engine and its Key Ladder to compute the STBCASN Check Number, Nagravision 
 *	and the chipset manufacturer will commonly agree during the NOCS 
 *	certification process of a chipset on how to compute the STBCASN Check 
 *	Number for this secure chipset. Refer to the “Implementation Proposal for 
 *	CSD & CTD APIs v2.0” document [10] for more details about this computation.
*/
TCsdStatus csdGetSTBCASNCheckNumber
(
  const TCsdUnsignedInt8    xSTBCASNData[4],
        TCsd4BytesVector    xSTBCASNCheckNumber
);

/**
 *  @brief
 *    Return the Data Integrity check number of the chipset.
 *
 *  @param[in]      pxData
 *                   Pointer to the location where the function has to 
 *					         read the Data used to compute the check number.
 *	@param[in]      xDataSize
 *                   Size in bytes of the Data.
 *  @param[out]     xDataIntegrityCheckNumber
 *                   4-bytes vector where the function has to write the Data 
 *                   Integrity Check Number.
 *
 *  @retval   CSD_NO_ERROR
 *              The intended operation was executed successfully.
 *  @retval   CSD_ERROR_INVALID_PARAMETERS,
 *              The passed parameter is invalid.
 *  @retval   CSD_ERROR
 *              The function terminated abnormally. The intended operation
 *              failed.
 *
 *  This function returns the Data Integrity Check Number. The computation of 
 *	Data Integrity Check Number is chipset-specific, and is the result of a 
 *	computation involving the R2R-RootKey128, a data as well as the RAM2RAM 
 *	Crypto Engine. As there may be several ways of configuring the RAM2RAM 
 *	Crypto Engine and its Key Ladder to compute the Data Integrity Check Number,
 *	Nagravision and the chipset manufacturer will commonly agree during the NOCS
 *  certification process of a chipset on how to compute the Data Integrity 
 *	Check Number for this secure chipset. Refer to the “Implementation Proposal 
 *	for CSD & CTD APIs v2.0” document [10] for more details about this 
 *	computation.
 *	The size in byte of xDataSize shall be defined between 1 and 16 bytes. 
 *  Otherwise, the function shall return CSD_ERROR_INVALID_PARAMETERS.
*/
TCsdStatus csdGetDataIntegrityCheckNumber
(
  const TCsdUnsignedInt8*   pxData,
  		  TCsdSize           xDataSize,
        TCsd4BytesVector     xDataIntegrityCheckNumber
);

/**
 * @brief
 *   Set the chipset active memory interface during boot.
 *
 *  @param[in]      xBootMode
 *                   Mode which has to be used by the Boot Mode is requested to
 *                   be set.
 *
 *  @retval   CSD_NO_ERROR
 *              The intended operation was executed successfully.
 *  @retval   CSD_ERROR_INVALID_PARAMETERS,
 *              The boot mode passed as parameter is out of type.
 *  @retval   CSD_ERROR_OPERATION_NOT_ALLOWED
 *              The boot mode has already been set to a different valid boot
 *              mode and has been locked.
 *  @retval   CSD_ERROR
 *              The function terminated abnormally. The intended operation
 *              failed.
 *
 *  This function allows the client software to configure the Boot Mode of the
 *  chipset. Once this mode has been been set, the chipset will always boot in
 *  this Boot Mode. TCsdBootMode is an opaque enumeration type which has to
 *  be defined by the chipset manufacturer depending on the Boot Mode supported
 *  by the chipset.
 *  If the Boot Mode passed as parameter is already set when this function is
 *  called or if the function has successfully set the Boot Mode, the function
 *  shall return CSD_NO_ERROR. If the Boot Mode is already set and the client
 *  software tries to set it to a different value, the function shall
 *  return CSD_ERROR_OPERATION_NOT_ALLOWED.
*/
TCsdStatus csdSetBootMode
(
  TCsdBootMode xBootMode
);

/**
 * @brief
 *   Get the selected active memory interface during the boot.
 *
 *  @param[out]     pxBootMode
 *                   Pointer to the location where the function has to write the
 *                   Boot Mode.
 *
 *  @retval   CSD_NO_ERROR
 *              The intended operation was executed successfully.
 *  @retval   CSD_ERROR
 *              The function terminated abnormally. The intended operation
 *              failed.
 *
 *  This function allows the client software to read the configured Boot Mode.
 *  If the hardware chipset technology allows an unprogrammed state for the Boot
 *  Mode, then the enumeration type shall define this state. The function shall
 *  return this state when it is necessary.
*/
TCsdStatus csdGetBootMode
(
  TCsdBootMode* pxBootMode
);

/**
 *  @brief
 *    Enable the Secure Chipset Startup
 *
 *  @retval   CSD_NO_ERROR
 *              The intended operation was executed successfully.
 *  @retval   CSD_ERROR
 *              The function terminated abnormally. The intended operation
 *              failed.
 *
 *  This function allows the client software to enable the SCS mechanism. Once
 *  enabled, this feature cannot be disabled (this mechanism is enforced by
 *  hardware). If the SCS feature is already enabled when this function is
 *  called or if the function has successfully enabled the SCS feature, the
 *  function shall return CSD_NO_ERROR. In all other cases, the call shall have
 *  no effect and the function shall return CSD_ERROR.
*/
TCsdStatus csdEnableScs
(
  void
);

/**
 *  @brief
 *    Get the SCS Activation Flag
 *
 *  @param[out]     pxScsActivationFlagState
 *                   Pointer to the location where the function has to write the
 *                   SCS Activation Flag State.
 *
 *  @retval   CSD_NO_ERROR
 *              The intended operation was executed successfully.
 *  @retval   CSD_ERROR
 *              The function terminated abnormally. The intended operation
 *              failed.
 *
 *  This function allows the client software to read the SCS Activation Flag
 *  State.
*/
TCsdStatus csdGetScsActivationFlag
(
  TCsdScsActivationFlag* pxScsActivationFlagState
);

/**
 *  @brief
 *    Set the size of the memory to be check by the SCS mechanism.
 *
 *  @param[in]      xScsTotalAreaSize
 *                   Size which has to be used by the SCS_Total_Area is requested
 *                   to be set.
 *
 *  @retval   CSD_NO_ERROR
 *              The intended operation was executed successfully.
 *  @retval   CSD_ERROR_INVALID_PARAMETERS
 *              The SCS_Total_Area passed as parameter is out of type.
 *  @retval   CSD_ERROR_OPERATION_NOT_ALLOWED
 *              The SCS_Total_Area has already been set to a different valid
 *              size and has been locked.
 *  @retval   CSD_ERROR
 *              The function terminated abnormally. The intended operation
 *              failed.
 *
 *  This function allows client software to configure the size of the
 *  SCS_Total_Area for chipsets where this size is configured in Persistent
 *  Values (e.g. fuses). For all other cases or if the desired size is already
 *  set when this function is called or if the function has successfully
 *  configured the desired size, the function shall return CSD_NO_ERROR.
 *  If the SCS_Total_Area size is already set and the client software tries to
 *  set it to a different value, the function shall return
 *  CSD_ERROR_OPERATION_NOT_ALLOWED.
*/
TCsdStatus csdSetScsTotalAreaSize
(
  TCsdScsTotalAreaSize xScsTotalAreaSize
);

/**
 *  @brief
 *    Get the set memory size which will be checked by the SCS.
 *
 *  @param[out]     pxScsTotalAreaSize
 *                   Pointer to the location where the function has to write the
 *                   SCS_Total_Area.
 *
 *  @retval   CSD_NO_ERROR
 *              The intended operation was executed successfully.
 *  @retval   CSD_ERROR
 *              The function terminated abnormally. The intended operation
 *              failed.
 *
 *  This function allows the client software to read the SCS_Total_Area.
 *  If the hardware technology allows an unprogrammed state, the function shall
 *  return CSD_SCS_ACTIVATION_FLAG_UNPROGRAMMED. Or if the chipset supports more
 *  sizes than the ones defined in NOCS 1.x [1] and the function reads one of
 *  these values, it shall return CSD_SCS_ACTIVATION_FLAG_NOT_SUPPORTED.
*/
TCsdStatus csdGetScsTotalAreaSize
(
  TCsdScsTotalAreaSize* pxTotalAreaSize
);

/**
 *  @brief
 *    Set the STBCASN.
 *
 *  @param[in]      xStbCaSn
 *                   4-bytes vector to which the STBCASN is requested to
 *                   be set.
 *
 *  @retval   CSD_NO_ERROR
 *              The intended operation was executed successfully.
 *  @retval   CSD_ERROR_OPERATION_NOT_ALLOWED
 *              The STBCASN has already been set to a different value
 *              and has been locked.
 *  @retval   CSD_ERROR
 *              The function terminated abnormally. The intended operation
 *              failed.
 *
 *  This function allows the client software to set a value into the
 *  STBCASN. The STBCASN is defined as a MSBF 4-bytes vector.
 *  If the STBCASN is already set and the client software tries to set
 *  it to a different value, the function shall return
 *  CSD_ERROR_OPERATION_NOT_ALLOWED.
*/
TCsdStatus csdSetStbCaSn
(
  const TCsd4BytesVector xStbCaSn
);

/**
 *  @brief
 *    Get the STBCASN of the chipset.
 *
 *  @param[out]     xStbCaSn
 *                   4-bytes vector to which the STBCASN is requested to
 *                   be set.
 *
 *  @retval   CSD_NO_ERROR
 *              The intended operation was executed successfully.
 *  @retval   CSD_ERROR
 *              The function terminated abnormally. The intended operation
 *              failed.
 *
 *  This function allows the client software to read the STBCASN. The
 *  output is a MSBF 4-bytes vector.
*/
TCsdStatus csdGetStbCaSn
(
  TCsd4BytesVector xStbCaSn
);

/**
 *  @brief
 *    Set the chipset market segment ID.
 *
 *  @param[in]      xMarketSegmentId
 *                   4-bytes vector to which the MarketSegmentId is requested to
 *                   be set.
 *
 *  @retval   CSD_NO_ERROR
 *              The intended operation was executed successfully.
 *  @retval   CSD_ERROR_OPERATION_NOT_ALLOWED
 *              The market segment ID has already been set to a different value
 *              and has been locked.
 *  @retval   CSD_ERROR
 *              The function terminated abnormally. The intended operation
 *              failed.
 *
 *  This function allows the client software to set a value into the
 *  MarketSegmentId. The MarketSegmentId is defined as a MSBF 4-bytes vector.
 *  If the MarketSegmentId is already set and the client software tries to set
 *  it to a different value, the function shall return
 *  CSD_ERROR_OPERATION_NOT_ALLOWED.
*/
TCsdStatus csdSetMarketSegmentId
(
  const TCsd4BytesVector xMarketSegmentId
);

/**
 *  @brief
 *    Get the market segment ID of the chipset.
 *
 *  @param[out]     xMarketSegmentId
 *                   4-bytes vector where the function has to write the
 *                   MarketSegmentId.
 *
 *  @retval   CSD_NO_ERROR
 *              The intended operation was executed successfully.
 *  @retval   CSD_ERROR
 *              The function terminated abnormally. The intended operation
 *              failed.
 *
 *  This function allows the client software to read the MarketSegmentId. The
 *  output is a MSBF 4-bytes vector.
*/
TCsdStatus csdGetMarketSegmentId
(
  TCsd4BytesVector xMarketSegmentId
);

/**
 *  @brief
 *    Select the Debug Interfaces protection level in the Access mode range of
 *    value.
 *
 *  @param[in]      xDebugInterfaceProtectionLevel
 *                   New protection level which has to be used by the Debug
 *                   Interface Protection Level is requested to switch.
 *
 *  @retval   CSD_NO_ERROR
 *              The intended operation was executed successfully.
 *  @retval   CSD_ERROR_INVALID_PARAMETERS,
 *              The protection level passed as parameter is out of type.
 *  @retval   CSD_ERROR_OPERATION_NOT_ALLOWED
 *              The requested protection is less secure than the current set
 *              protection level.
 *  @retval   CSD_ERROR
 *              The function terminated abnormally.
 *              The intended operation failed.
 *
 *  Client software can change the protection state of the Debug Interface
 *  by calling this function. Only transitions from a less secure to a more
 *  secure state are permitted (this mechanism is enforced by hardware). If
 *  client software requests to switch from a more secure to a less secure
 *  state, the function shall return CSD_ERROR_OPERATION_NOT_ALLOWED.
 *  If the client software requests to switch to the current level of security
 *  or if the function has successfully switched in the desired level of
 *  security, the function shall return CSD_NO_ERROR. If an invalid parameter is
 *  passed to the function, the function shall return
 *  CSD_ERROR_INVALID_PARAMETERS.  In all other cases, the call shall have no
 *  effect and the function shall return CSD_ERROR.
 *  In the event the Debug Interface would be in an inconsistent state when this
 *  function is called, and assuming the new state selected is more secure, then
 *  the implementation of this function shall anyway switch to the new state.
*/
TCsdStatus csdSelectDebugInterfaceProtectionLevel
(
  TCsdDebugInterfaceAccessMode xDebugInterfaceProtectionLevel
);

/**
 *  @brief
 *    Get the set debug interface protection level.
 *
 *  @param[out]     pxDebugInterfaceProtectionLevel
 *                   Pointer to the location where the function has to write the
 *                   Debug Interface Protection Level.
 *
 *  @retval   CSD_NO_ERROR
 *              The intended operation was executed successfully.
 *  @retval   CSD_ERROR
 *              The function terminated abnormally. The intended operation
 *              failed.
 *
 *  This function returns the current protection level of the Debug Interface.
 *  This output can be one out of three values, depending on whether the port is
 *  fully and unconditionally functional (CSD_DEBUG_INTERFACE_ACCESS_MODE_OPEN),
 *  password-protected (CSD_DEBUG_INTERFACE_ACCESS_MODE_PASSWORD_PROTECTED), or
 *  unconditionally disabled (CSD_DEBUG_INTERFACE_ACCESS_MODE_CLOSED).
 *  If the Debug Interface is in an inconsistent state, then the output shall
 *  be CSD_DEBUG_INTERFACE_ACCESS_MODE_INCONSISTENT_STATE.
 */
TCsdStatus csdGetDebugInterfaceProtectionLevel
(
  TCsdDebugInterfaceAccessMode* pxDebugInterfaceProtectionLevel
);

/**
 *  @brief
 *    Deactivate the usage of the embedded secret Key and the attached Key
 *    Ladder.
 *
 *  @retval   CSD_NO_ERROR
 *              The intended operation was executed successfully.
 *  @retval   CSD_ERROR
 *              The function terminated abnormally. The intended operation
 *              failed.
 *
 *  This function allows the client software to disable the usage of the
 *  ContentKey by deactivating the Key Ladder.
*/
TCsdStatus csdDeactivateKeyLadder
(
  void
);

/**
 *  @brief
 *    Get the Key ladder deactivation flag state.
 *
 *  @param[out]     pxKeyLadderDeactivationFlagState
 *                   Pointer to the location where the function has to write the
 *                   Key ladder deactivation flag state.
 *
 *  @retval   CSD_NO_ERROR
 *              The intended operation was executed successfully.
 *  @retval   CSD_ERROR
 *              The function terminated abnormally. The intended operation
 *              failed.
 *
 *  This function allows the client software to read the state of the Key ladder
 *  deactivation flag.
*/
TCsdStatus csdGetKeyLadderDeactivationFlag
(
 TCsdKeyLadderDeactivationFlag* pxKeyLadderDeactivationFlagState
);

/**
 *  @brief
 *    Return the DVR Key of the chipset.
 *
 *  @param[out]     xDvrKey
 *                   16-bytes vector where the function has to write the DvrKey.
 *
 *  @retval   CSD_NO_ERROR
 *              The intended operation was executed successfully.
 *  @retval   CSD_ERROR
 *              The function terminated abnormally. The intended operation
 *              failed.
 *
 *  When the STB application needs to encrypt or decrypt a piece of content for
 *  HDD recording and playback, it has to use a specific key. This key is not
 *  stored in clear-text form, but encrypted with secret ContentKey. This
 *  encrypted version of the key is called a Keysafe.
 *  In order to recover the actual key and use it to encrypt or decrypt content,
 *  the following step must be performed:
 *      1.	Decrypt the KeySafe = [0xbf 0x18 0x6a 0x73 0xec 0x86 0x3f 0x25
 *                                 0x9b 0xe8 0x03 0x52 0x40 0xd7 0x37 0x8c]
 *          with the secret ContentKey.
 *
 *  The decryption of Keysafe is chipset-specific, and is the result of a
 *  computation involving the R2R-RootKey128, the Key Ladder as well as the
 *  RAM2RAM Crypto Engine. As there may be several ways of configuring the
 *  RAM2RAM Crypto Engine and its Key Ladder to compute the DvrKey,
 *  Nagravision and the chipset manufacturer will commonly agree during the NOCS
 *   certification process of a chipset on how to decrypt the KeySafe
 *  for this secure chipset. Refer to the "Implementation Proposal for CSD & CTD
 *  APIs v2.0" document [10] for more details about this computation.
*/
TCsdStatus csdGetDvrKey
(
  TCsdUnsignedInt8    xDvrKey[16]
);

/**
 *  @brief
 *    Set the software key and used the encryption RAM2RAM Engine.
 *
 *  @param[in]      xAlgorithm
 *                   Algorithm to which the RAM2RAM Crypto Engine is requested
 *                   to operate.
 *  @param[in]      xMode
 *                   Cryptographic operation mode (ECB, CBC) which has to be used
 *                   by the RAM2RAM Crypto Engine is requested to operate.
 *  @param[in]      pxClearTextHostKey
 *                   Pointer to the location where the function has to read the
 *                   Clear Text Hostkey.
 *  @param[in]      xClearTextHostKeySize
 *                   Size in bytes of the Clear Text Hostkey.
 *  @param[in]      pxInitializationVector
 *                   Pointer to the location where the function has to read the
 *                   Initialization Vector.
 *  @param[in]      xInitializationVectorSize
 *                   Size in bytes of the Initialization Vector.
 *  @param[in]      xRefreshIv
 *                   If TRUE, the CBC chaining register shall be loaded with the
 *                   value pointed by pxInitializationVector.
 *  @param[in]      pxR2RKeyPathHandle
 *                   Pointer to the location where the function has to read the
 *                   structure which defines the key path used to set the
 *                   Clear Text Hostkey.
 *  @param[in]      pxInputData
 *                   Pointer to the location where the function has to read the
 *                   Input Data.
 *  @param[out]     pxOutputData
 *                   Pointer to the location where the function has to read the
 *                   Output Data.
 *  @param[in]      xDataSize
 *                   Size in bytes of the Data.
 *
 *  @retval   CSD_NO_ERROR
 *              The intended operation was executed successfully.
 *  @retval   CSD_ERROR_OPERATION_NOT_SUPPORTED
 *              The requested operation is not supported by the chipset.
 *  @retval   CSD_ERROR_INVALID_PARAMETERS
 *              At least one of the passed parameters is invalid.
 *  @retval   CSD_ERROR_INVALID_KEY_PATH_HANDLE
 *              The key path handle passed as parameter is invalid.
 *  @retval   CSD_ERROR
 *              The function terminated abnormally.
 *              The intended operation failed.
 *
 *  This function set the Clear Text Hostkey (software provided key) into the
 *  specified path of the Ram2Ram Crypto Engine and requests the Ram2Ram Crypto
 *  Engine to encrypt a block of data stored in RAM and write the result into
 *  RAM.
 *  pxClearTextHostKey contains the Clear Text HostKey. The size of this key
 *  must be a 16-byte vector. In all other cases, the function shall return
 *  CSD_ERROR_INVALID_PARAMETERS.
 *  xAlgorithm selects the cryptographic algorithm used to encrypt the data and
 *  xMode selects the operation mode (ECB, CBC) of the cryptographic
 *  algorithm.
 *  If the cryptographic algorithm mode is set to CBC and xRefreshIv is set to
 *  TRUE, pxInitializationVector is used to initialize the CBC chaining register
 *  to the Ram2Ram Crypto Engine. Otherwise the CBC chaining register shall not
 *  be changed. The size of this vector depends on the cryptographic algorithm
 *  (16 bytes for AES and 8 bytes for TDES). In all other cases, the function
 *  shall return CSD_ERROR_INVALID_PARAMETERS.
 *  The size of the input data block must be a multiple of the data block size
 *  of the cryptographic algorithm that will be used to encrypt it. For
 *  instance, the size of the input data block must be a multiple of 8 bytes
 *  if it must be encrypted with the TDES algorithm or a multiple of 16 bytes if
 *  it must be encrypted with the AES algorithm. Otherwise, the function shall
 *  return CSD_ERROR_INVALID_PARAMETERS.
 *  If a valid cryptographic algorithm and/or an operation mode selected by the
 *  client software but the chipset does not support it, the function shall
 *  return CSD_ERROR_OPERATION_NOT_SUPPORTED.
 *  If the client software selects a wrong key path handle, the function
 *  shall return CSD_ERROR_INVALID_KEY_PATH_HANDLE.
*/
TCsdStatus csdEncryptDataWithClearTextHostKey
(
        TCsdR2RAlgorithm            xAlgorithm,
        TCsdR2RCryptoOperationMode  xMode,
  const TCsdUnsignedInt8*          pxClearTextHostKey,
        TCsdSize                    xClearTextHostKeySize,
  const TCsdUnsignedInt8*          pxInitializationVector,
        TCsdSize                    xInitializationVectorSize,
        TCsdBoolean        			xRefreshIv,
        TCsdR2RKeyPathHandle*      pxR2RKeyPathHandle,
  const TCsdUnsignedInt8*          pxInputData,
        TCsdUnsignedInt8*          pxOutputData,
        TCsdSize                    xDataSize
);

/**
 *  @brief
 *    Set the software key and used the decryption RAM2RAM Engine.
 *
 *  @param[in]      xAlgorithm
 *                   Algorithm to which the RAM2RAM Crypto Engine is requested
 *                   to operate.
 *  @param[in]      xMode
 *                   Cryptographic operation mode (ECB, CBC) which has to be used
 *                   by the RAM2RAM Crypto Engine is requested to operate.
 *  @param[in]      pxClearTextHostKey
 *                   Pointer to the location where the function has to read the
 *                   Clear Text Hostkey.
 *  @param[in]      xClearTextHostKeySize
 *                   Size in bytes of the Clear Text Hostkey.
 *  @param[in]      pxInitializationVector
 *                   Pointer to the location where the function has to read the
 *                   Initialization Vector.
 *  @param[in]      xInitializationVectorSize
 *                   Size in bytes of the Initialization Vector.
 *  @param[in]      xRefreshIv
 *                   If TRUE, the CBC chaining register shall be loaded with the
 *                   value pointed by pxInitializationVector.
 *  @param[in,out]  pxR2RKeyPathHandle
 *                   Pointer to the location where the function has to read the
 *                   structure which defines the key path used to set the
 *                   Clear Text Hostkey.
 *  @param[in]      pxInputData
 *                   Pointer to the location where the function has to read the
 *                   Input Data.
 *  @param[out]     pxOutputData
 *                   Pointer to the location where the function has to read the
 *                   Output Data.
 *  @param[in]      xDataSize
 *                   Size in bytes of the Data.
 *
 *  @retval   CSD_NO_ERROR
 *              The intended operation was executed successfully.
 *  @retval   CSD_ERROR_OPERATION_NOT_SUPPORTED
 *              The requested operation is not supported by the chipset.
 *  @retval   CSD_ERROR_INVALID_PARAMETERS
 *              At least one of the passed parameters is invalid.
 *  @retval   CSD_ERROR_INVALID_KEY_PATH_HANDLE
 *              The key path handle passed as parameter is invalid.
 *  @retval   CSD_ERROR
 *              The function terminated abnormally.
 *              The intended operation failed.
 *
 *  This function set the Clear Text Hostkey (software provided key) into the
 *  specified path of the Ram2Ram Crypto Engine and requests the Ram2Ram Crypto
 *  Engine to decrypt a block of data stored in RAM and write the result into
 *  RAM.
 *  pxClearTextHostKey contains the Clear Text HostKey. The size of this key
 *  must a 16-byte vector. In all other cases, the function shall return
 *  CSD_ERROR_INVALID_PARAMETERS.
 *  xAlgorithm selects the cryptographic algorithm used to decrypt the data and
 *  xMode selects the operation mode (ECB, CBC) of the cryptographic algorithm.
 *  If the cryptographic algorithm mode is set to CBC and xRefreshIv is set to
 *  TRUE, pxInitializationVector is used to initialize the CBC chaining register
 *  to the Ram2Ram Crypto Engine. Otherwise the CBC chaining register shall not
 *  be changed. The size of this vector depends on the cryptographic algorithm
 *  (16 bytes for AES and 8 bytes for TDES). In all other cases, the function
 *  shall return CSD_ERROR_INVALID_PARAMETERS.
 *  The size of the input data block must be a multiple of the data block size
 *  of the cryptographic algorithm that will be used to decrypt it. For
 *  instance, the size of the input data block must be a multiple of 8 bytes
 *  if it must be decrypted with the TDES algorithm or a multiple of 16 bytes if
 *  it must be decrypted with the AES algorithm. Otherwise, the function shall
 *  return CSD_ERROR_INVALID_PARAMETERS.
 *  If a valid cryptographic algorithm and/or an operation mode are selected by
 *  the client software but the chipset does not support it, the function shall
 *  return CSD_ERROR_OPERATION_NOT_SUPPORTED.
 *  If the client software selects a wrong key path handle,
 *  the function shall return CSD_ERROR_INVALID_KEY_PATH_HANDLE.
*/
TCsdStatus csdDecryptDataWithClearTextHostKey
(
        TCsdR2RAlgorithm            xAlgorithm,
        TCsdR2RCryptoOperationMode  xMode,
  const TCsdUnsignedInt8*          pxClearTextHostKey,
        TCsdSize                    xClearTextHostKeySize,
  const TCsdUnsignedInt8*          pxInitializationVector,
        TCsdSize                    xInitializationVectorSize,
        TCsdBoolean        			xRefreshIv,
        TCsdR2RKeyPathHandle*      pxR2RKeyPathHandle,
  const TCsdUnsignedInt8*          pxInputData,
        TCsdUnsignedInt8*          pxOutputData,
        TCsdSize                    xDataSize
);

/**
 *  @brief
 *    Set the secret key and used the encryption RAM2RAM Engine.
 *
 *  @param[in]      xAlgorithm
 *                   Algorithm to which the RAM2RAM Crypto Engine is requested
 *                   to operate.
 *  @param[in]      xMode
 *                   Cryptographic operation mode (ECB, CBC) which has to be used
 *                   by the RAM2RAM Crypto Engine is requested to operate.
 *  @param[in]      xR2RCipheredProtectingKeys
 *                   Array of Ciphered Protecting Keys.
 *  @param[in]      pxCipheredContentKey
 *                   Pointer to the location where the function has to read the
 *                   Ciphered Content Key.
 *  @param[in]      xCipheredContentKeySize
 *                   Size in bytes of the Ciphered Content Key.
 *  @param[in]      pxInitializationVector
 *                   Pointer to the location where the function has to read the
 *                   Initialization Vector.
 *  @param[in]      xInitializationVectorSize
 *                   Size in bytes of the Initialization Vector.
 *  @param[in]      xRefreshIv
 *                   If TRUE, the CBC chaining register shall be loaded with the
 *                   value pointed by pxInitializationVector.
 *  @param[in,out]  pxR2RKeyPathHandle
 *                   Pointer to the location where the function has to read the
 *                   structure which defines the key path used to set the
 *                   Ciphered Content Key.
 *  @param[in]      pxInputData
 *                   Pointer to the location where the function has to read the
 *                   Input Data.
 *  @param[out]     pxOutputData
 *                   Pointer to the location where the function has to read the
 *                   Output Data.
 *  @param[in]      xDataSize
 *                   Size in bytes of the Data.
 *
 *  @retval   CSD_NO_ERROR
 *              The intended operation was executed successfully.
 *  @retval   CSD_ERROR_OPERATION_NOT_SUPPORTED
 *              The requested operation is not supported by the chipset.
 *  @retval   CSD_ERROR_INVALID_PARAMETERS
 *              At least one of the passed parameters is invalid.
 *  @retval   CSD_ERROR_INVALID_KEY_PATH_HANDLE
 *              The key path handle passed as parameter is invalid.
 *  @retval   CSD_ERROR
 *              The function terminated abnormally.
 *              The intended operation failed.
 *
 *  This function set the Secret Content Key (key ladder provided key) into the
 *  specified path of the Ram2Ram Crypto Engine and requests the Ram2Ram Crypto
 *  Engine to encrypt a block of data stored in RAM and write the result into
 *  RAM.
 *  xR2RCipheredProtectingKeys contains all the Ciphered Protecting Keys. The
 *  number of those keys contained in this array depends on the number of
 *  Key Ladder Elements which is defined by the constant
 *  CSD_R2R_NB_OF_PROTECTING_KEYS in csd_nocs1x_imp.h file.
 *  pxCipheredContentKey contains the ciphered form of Secret Content Key. The
 *  size of this key must be a 16-byte vector. In all other cases, the function
 *  shall return CSD_ERROR_INVALID_PARAMETERS.
 *  xAlgorithm selects the cryptographic algorithm used to encrypt the data and
 *  xMode selects the operation mode (ECB, CBC) of the cryptographic algorithm.
 *  If the cryptographic algorithm mode is set to CBC and xRefreshIv is set to
 *  TRUE, pxInitializationVector is used to initialize the CBC chaining register
 *  to the Ram2Ram Crypto Engine. Otherwise the CBC chaining register shall not
 *  be changed. The size of this vector depends on the cryptographic algorithm
 *  (16 bytes for AES and 8 bytes for TDES). In all other cases, the function
 *  shall return CSD_ERROR_INVALID_PARAMETERS.
 *  The size of the input data block must be a multiple of the data block size
 *  of the cryptographic algorithm that will be used to encrypt it. For
 *  instance, the size of the input data block must be a multiple of 8 bytes
 *  if it must be encrypted with the TDES algorithm or a multiple of 16 bytes if
 *  it must be encrypted with the AES algorithm. Otherwise, the function shall
 *  return CSD_ERROR_INVALID_PARAMETERS.
 *  If a valid cryptographic algorithm and/or an operation mode are selected by
 *  the client software but the chipset does not support it, the function shall
 *  return CSD_ERROR_OPERATION_NOT_SUPPORTED.
 *  If the client software selects a wrong key path handle,
 *  the function shall return CSD_ERROR_INVALID_KEY_PATH_HANDLE.
*/
TCsdStatus csdEncryptDataWithSecretContentKey
(
        TCsdR2RAlgorithm                xAlgorithm,
        TCsdR2RCryptoOperationMode      xMode,
  const TCsdR2RCipheredProtectingKeys   xR2RCipheredProtectingKeys,
  const TCsdUnsignedInt8*              pxCipheredContentKey,
        TCsdSize                        xCipheredContentKeySize,
  const TCsdUnsignedInt8*              pxInitializationVector,
        TCsdSize                        xInitializationVectorSize,
        TCsdBoolean        			    xRefreshIv,
        TCsdR2RKeyPathHandle*          pxR2RKeyPathHandle,
  const TCsdUnsignedInt8*              pxInputData,
        TCsdUnsignedInt8*              pxOutputData,
        TCsdSize                        xDataSize
);

/**
 *  @brief
 *    Set the secret key and used the decryption RAM2RAM Engine.
 *
 *  @param[in]      xAlgorithm
 *                   Algorithm to which the RAM2RAM Crypto Engine is requested
 *                   to operate.
 *  @param[in]      xMode
 *                   Cryptographic operation mode (ECB, CBC, etc) which has to be
 *                   used by the RAM2RAM Crypto engine is requested to operate.
 *  @param[in]      xR2RCipheredProtectingKeys
 *                   Array of Ciphered Protecting Keys.
 *  @param[in]      pxCipheredContentKey
 *                   Pointer to the location where the function has to read the
 *                   Ciphered Content Key.
 *  @param[in]      xCipheredContentKeySize
 *                   Size in bytes of the Ciphered Content Key.
 *  @param[in]      pxCipheredContentKey
 *                   Pointer to the location where the function has to read the
 *                   Ciphered Content Key.
 *  @param[in]      xCipheredContentKeySize
 *                   Size in bytes of the Ciphered Content Key.
 *  @param[in]      pxInitializationVector
 *                   Pointer to the location where the function has to read the
 *                   Initialization Vector.
 *  @param[in]      xInitializationVectorSize
 *                   Size in bytes of the Initialization Vector.
 *  @param[in]      xRefreshIv
 *                   If TRUE, the CBC chaining register shall be loaded with the
 *                   value pointed by pxInitializationVector.
 *  @param[in,out]  pxR2RKeyPathHandle
 *                   Pointer to the location where the function has to read the
 *                   structure which defines the key path used to set the
 *                   Ciphered Content Key.
 *  @param[in]      pxInputData
 *                   Pointer to the location where the function has to read the
 *                   Input Data.
 *  @param[in]      pxOutputData
 *                   Pointer to the location where the function has to read the
 *                   Output Data.
 *  @param[in]      xDataSize
 *                   Size in bytes of the Data.
 *
 *  @retval   CSD_NO_ERROR
 *              The intended operation was executed successfully.
 *  @retval   CSD_ERROR_OPERATION_NOT_SUPPORTED
 *              The requested operation is not supported by the chipset.
 *  @retval   CSD_ERROR_INVALID_PARAMETERS
 *              At least one of the passed parameters is invalid.
 *  @retval   CSD_ERROR_INVALID_KEY_PATH_HANDLE
 *              The key path handle passed as parameter is invalid.
 *  @retval   CSD_ERROR
 *              The function terminated abnormally.
 *              The intended operation failed.
 *
 *  This function set the Secret Content Key (key ladder provided key) into the
 *  specified path of the Ram2Ram Crypto Engine and requests the Ram2Ram Crypto
 *  Engine to decrypt a block of data stored in RAM and write the result into
 *  RAM.
 *  xR2RCipheredProtectingKeys contains all the Ciphered Protecting Keys. The
 *  number of those keys contained in this array depends on the number of Key
 *  Ladder Elements which is defined by the constant
 *  CSD_R2R_NB_OF_PROTECTING_KEYS in csd_nocs1x_imp.h file.
 *  pxCipheredContentKey contains the ciphered form of Secret Content Key. The
 *  size of this key must be a 16-byte vector. In all other cases, the function
 *  shall return CSD_ERROR_INVALID_PARAMETERS.
 *  xAlgorithm selects the cryptographic algorithm used to decrypt the data and
 *  xMode selects the operation mode (ECB, CBC) of the cryptographic algorithm.
 *  If the cryptographic algorithm mode is set to CBC and xRefreshIv is set to
 *  TRUE, pxInitializationVector is used to initialize the CBC chaining register
 *  to the Ram2Ram Crypto Engine. Otherwise the CBC chaining register shall not
 *  be changed. The size of this vector depends on the cryptographic algorithm
 *  (16 bytes for AES and 8 bytes for TDES). In all other cases, the function
 *  shall return CSD_ERROR_INVALID_PARAMETERS.
 *  The size of the input data block must be a multiple of the data block size
 *  of the cryptographic algorithm that will be used to decrypt it. For
 *  instance, the size of the input data block must be a multiple of 8 bytes if
 *  it must be decrypted with the TDES algorithm or a multiple of 16 bytes if it
 *  must be decrypted with the AES algorithm. Otherwise, the function shall
 *  return CSD_ERROR_INVALID_PARAMETERS.
 *  If a valid cryptographic algorithm and/or an operation mode are selected by
 *  the client software but the chipset does not support it, the function shall
 *  return CSD_ERROR_OPERATION_NOT_SUPPORTED.
 *  If the client software selects a wrong key path handle, the function shall
 *  return CSD_ERROR_INVALID_KEY_PATH_HANDLE.
*/
TCsdStatus csdDecryptDataWithSecretContentKey
(
        TCsdR2RAlgorithm                xAlgorithm,
        TCsdR2RCryptoOperationMode      xMode,
  const TCsdR2RCipheredProtectingKeys   xR2RCipheredProtectingKeys,
  const TCsdUnsignedInt8*              pxCipheredContentKey,
        TCsdSize                        xCipheredContentKeySize,
  const TCsdUnsignedInt8*              pxInitializationVector,
        TCsdSize                        xInitializationVectorSize,
        TCsdBoolean        			    xRefreshIv,
        TCsdR2RKeyPathHandle*          pxR2RKeyPathHandle,
  const TCsdUnsignedInt8*              pxInputData,
        TCsdUnsignedInt8*              pxOutputData,
        TCsdSize                        xDataSize
);

/**
 *  @brief
 *    Set the software key for the Transport Stream Crypto Engine.
 *
 *  @param[in]      xEmi
 *                   Algorithm which has to be used by the Transport Stream
 *                   Crypto engine.
 *  @param[in]      pxClearTextDscOddHostKey
 *                   Pointer to the location where the function has to read the
 *                   Clear Text DSC Odd Hostkey.
 *  @param[in]      xClearTextDscOddHostKeySize
 *                   Size in bytes of the Clear Text DSC Odd Hostkey.
 *  @param[in]      pxClearTextDscEvenHostKey
 *                   Pointer to the location where the function has to read the
 *                   Clear Text DSC Even Hostkey.
 *  @param[in]      xClearTextDscEvenHostKeySize
 *                   Size in bytes of the Clear Text DSC Even Hostkey.
 *  @param[in,out]  pxDscKeyPathHandle
 *                   Pointer to the location where the function has to read the
 *                   structure which defines the key path used to set the
 *                   Clear Text DSC Odd Hostkey and Clear Text DSC Even Hostkey.
 *
 *  @retval   CSD_NO_ERROR
 *              The intended operation was executed successfully.
 *  @retval   CSD_ERROR_OPERATION_NOT_SUPPORTED
 *              The requested operation is not supported by the chipset.
 *  @retval   CSD_ERROR_INVALID_PARAMETERS
 *              At least one of the passed parameters is invalid.
 *  @retval   CSD_ERROR_INVALID_KEY_PATH_HANDLE
 *              The key path handle passed as parameter is invalid.
 *  @retval   CSD_ERROR
 *              The function terminated abnormally.
 *              The intended operation failed.
 *
 *  This function set the CWs provided in clear-text form into the specified
 *  path of the Transport Stream Crypto Engine.
 *  If one of the CWs also called HostKeys (odd or even) is not available, the
 *  given pointer is set to NULL and its corresponding length to 0. The driver
 *  should not change the previous CW in this case.
 *  The size of these keys shall be:
 *      "	0 if the CW is not available.
 *      "	8 if the cryptographic algorithm is requesting 8 bytes.
 *      "	16 if the cryptographic algorithm requesting 16 bytes.
 *      "	In all other cases, the function shall return
 *          CSD_ERROR_INVALID_PARAMETERS.
 *  If a valid cryptographic algorithm is selected by the client software but
 *  the chipset does not support it, the function shall return
 *  CSD_ERROR_OPERATION_NOT_SUPPORTED.
 *  If the client software selects a wrong key path handle, the function shall
 *  return CSD_ERROR_INVALID_KEY_PATH_HANDLE.
*/
TCsdStatus csdSetClearTextDscHostKeys
(
        TCsdUnsignedInt16         xEmi,
  const TCsdUnsignedInt8*        pxClearTextDscOddHostKey,
        TCsdSize                  xClearTextDscOddHostKeySize,
  const TCsdUnsignedInt8*        pxClearTextDscEvenHostKey,
        TCsdSize                  xClearTextDscEvenHostKeySize,
        TCsdDscKeyPathHandle*    pxDscKeyPathHandle
);

/**
 *  @brief
 *    Set the protected key for the Transport Stream Crypto Engine.
 *
 *  @param[in]      xEmi
 *                   Algorithm which has to be used by the Transport Stream
 *                   Crypto engine.
 *  @param[in]      xDscCipheredProtectingKeys
 *                   Array of Ciphered Protecting Keys
 *  @param[in]      pxCipheredDscOddContentKey
 *                   Pointer to the location where the function has to read the
 *                   Ciphered DSC Odd Contentkey.
 *  @param[in]      xCipheredDscOddContentKeySize
 *                   Size in bytes of the Ciphered DSC Odd Contentkey.
 *  @param[in]      pxCipheredDscEvenContentKey
 *                   Pointer to the location where the function has to read the
*                   Ciphered DSC Even Contentkey.
 *  @param[in]      xCipheredDscEvenContentKeySize
 *                   Size in bytes of the Ciphered DSC Even Contentkey.
 *  @param[in,out]  pxDscKeyPathHandle
 *                   Pointer to the location where the function has to read the
 *                   structure which defines the key path used to set the
 *                   Ciphered DSC Odd Contentkey and Ciphered DSC Even
 *                   Contentkey.
 *
 *  @retval   CSD_NO_ERROR
 *              The intended operation was executed successfully.
 *  @retval   CSD_ERROR_OPERATION_NOT_SUPPORTED
 *              The requested operation is not supported by the chipset.
 *  @retval   CSD_ERROR_INVALID_PARAMETERS
 *              At least one of the passed parameters is invalid.
 *  @retval   CSD_ERROR_INVALID_KEY_PATH_HANDLE
 *              The key path handle passed as parameter is invalid.
 *  @retval   CSD_ERROR
 *              The function terminated abnormally.
 *              The intended operation failed.
 *
 *  This function configures the key ladder with the Ciphered Protecting Keys
 *  and sets the CWs provided in protected form into the specified path of the
 *  Transport Stream Crypto Engine.
 *  xDscCipheredProtectingKeys contains all the Ciphered Protecting Keys. The
 *  number of those keys contained in this array depends on the number of Key
 *  Ladder Elements which is defined by the constant
 *  CSD_DSC_NB_OF_PROTECTING_KEYS in csd_nocs1x_imp.h file.
 *  If one of the ciphered CWs also called Ciphered ContentKey (odd or even) is
 *  not available, the given pointer is set to NULL and its corresponding length
 *   to 0. The driver should not change the previous CW in this case.
 *  The size of these keys shall be:
 *      "	0 if the CW is not available.
 *      "	8 if the cryptographic algorithm is requesting 8 bytes.
 *      "	16 if the cryptographic algorithm  is requesting 16 bytes.
 *      "	In all other cases, the function shall return
 *          CSD_ERROR_INVALID_PARAMETERS.
 *  If a valid cryptographic algorithm is selected by the client software but
 *  the chipset does not support it, the function shall return
 *  CSD_ERROR_OPERATION_NOT_SUPPORTED.
 *  If the client software selects a wrong key path handle, the function shall
 *  return CSD_ERROR_INVALID_KEY_PATH_HANDLE.
*/
TCsdStatus csdSetProtectedDscContentKeys
(
        TCsdUnsignedInt16               xEmi,
  const TCsdDscCipheredProtectingKeys   xDscCipheredProtectingKeys,
  const TCsdUnsignedInt8*              pxCipheredDscOddContentKey,
        TCsdSize                        xCipheredDscOddContentKeySize,
  const TCsdUnsignedInt8*              pxCipheredDscEvenContentKey,
        TCsdSize                        xCipheredDscEvenContentKeySize,
        TCsdDscKeyPathHandle*          pxDscKeyPathHandle
);

/**
 *  @brief
 *    Set the software encryption key for the Stream Crypto Engine
 *
 *  @param[in]      xEmi
 *                   Algorithm which has to be used by the Stream
 *                   Crypto engine.
 *  @param[in]      pxClearTextScrHostKey
 *                   Pointer to the location where the function has to read the
 *                   Clear Text SCR Hostkey.
 *  @param[in]      xClearTextScrHostKeySize
 *                   Size in bytes of the Clear Text SCR Hostkey.
 *  @param[in]      pxInitializationVector
 *                   Pointer to the location where the function has to read the
 *                   Initialization Vector.
 *  @param[in]      xInitializationVectorSize
 *                   Size in bytes of the Initialization Vector.
 *  @param[in,out]  pxScrKeyPathHandle
 *                   Pointer to the location where the function has to read the
 *                   structure which defines the key path used to set the
 *                   Clear Text SCR Hostkey.
 *
 *  @retval   CSD_NO_ERROR
 *              The intended operation was executed successfully.
 *  @retval   CSD_ERROR_OPERATION_NOT_SUPPORTED
 *              The requested operation is not supported by the chipset.
 *  @retval   CSD_ERROR_INVALID_PARAMETERS
 *              At least one of the passed parameters is invalid.
 *  @retval   CSD_ERROR_INVALID_KEY_PATH_HANDLE
 *              The key path handle passed as parameter is invalid.
 *  @retval   CSD_ERROR
 *              The function terminated abnormally.
 *              The intended operation failed.
 *
 *  This function sets the encryption Clear Text SCR Hostkey (software provided
 *  key) into the specified path of the Stream Crypto Engine.
 *  pxClearTextScrHostKey contains the Clear Text HostKey. The size of this key
 *  must be a 16-byte vector. In all other cases, the function shall return
 *  CSD_ERROR_INVALID_PARAMETERS.
 *  xEmi selects the cryptographic algorithm used to encrypt the stream.
 *  If the xEmi is set to one of the CBC mode, pxInitializationVector
 *  is used to pass the Initialization Vector to the Stream Crypto Engine. The
 *  size of this vector depends on the cryptographic algorithm (16 bytes for AES
 *  and 8 bytes for TDES). In all other cases, the function shall return
 *  CSD_ERROR_INVALID_PARAMETERS.
 *  If a valid cryptographic algorithm is selected by
 *  the client software but the chipset does not support it, the function shall
 *  return CSD_ERROR_OPERATION_NOT_SUPPORTED.
 *  If the client software selects a wrong key path handle, the function shall
 *  return CSD_ERROR_INVALID_KEY_PATH_HANDLE.
*/
TCsdStatus csdSetClearTextScrEncryptionKey
(
        TCsdUnsignedInt16           xEmi,
  const TCsdUnsignedInt8*          pxClearTextScrHostKey,
        TCsdSize                    xClearTextScrHostKeySize,
  const TCsdUnsignedInt8*          pxInitializationVector,
        TCsdSize                    xInitializationVectorSize,
        TCsdScrKeyPathHandle*      pxScrKeyPathHandle
);

/**
 *  @brief
 *    Set the software decryption key for the Stream Crypto Engine
 *
 *  @param[in]      xEmi
 *                   Algorithm which has to be used by the Stream
 *                   Crypto engine.
 *  @param[in]      pxClearTextScrHostKey
 *                   Pointer to the location where the function has to read the
 *                   Clear Text SCR Hostkey.
 *  @param[in]      xClearTextScrHostKeySize
 *                   Size in bytes of the Clear Text SCR Hostkey.
 *  @param[in]      pxInitializationVector
 *                   Pointer to the location where the function has to read the
 *                   Initialization Vector.
 *  @param[in]      xInitializationVectorSize
 *                   Size in bytes of the Initialization Vector.
 *  @param[in,out]  pxScrKeyPathHandle
 *                   Pointer to the location where the function has to write the
 *                   structure which defines the harware path used to set the
 *                   Clear Text SCR Hostkey.
 *
 *  @retval   CSD_NO_ERROR
 *              The intended operation was executed successfully.
 *  @retval   CSD_ERROR_OPERATION_NOT_SUPPORTED
 *              The requested operation is not supported by the chipset.
 *  @retval   CSD_ERROR_INVALID_PARAMETERS
 *              At least one of the passed parameters is invalid.
 *  @retval   CSD_ERROR_INVALID_KEY_PATH_HANDLE
 *              The key path handle passed as parameter are invalid.
 *  @retval   CSD_ERROR
 *              The function terminated abnormally.
 *              The intended operation failed.
 *
 *  This function sets the decryption Clear Text SCR Hostkey (software provided
 *  key) into the specified path of the Stream Crypto Engine.
 *  pxClearTextScrHostKey contains the Clear Text HostKey. The size of this key
 *  must be a 16-byte vector. In all other cases, the function shall return
 *  CSD_ERROR_INVALID_PARAMETERS.
 *  xEmi selects the cryptographic algorithm used to decrypt the stream.
 *  If the xEmi is set to one of the CBC mode, pxInitializationVector
 *  is used to pass the Initialization Vector to the Stream Crypto Engine. The
 *  size of this vector depends on the cryptographic algorithm (16 bytes for AES
 *  and 8 bytes for TDES). In all other cases, the function shall return
 *  CSD_ERROR_INVALID_PARAMETERS.
 *  If a valid cryptographic algorithm is selected by
 *  the client software but the chipset does not support it, the function shall
 *  return CSD_ERROR_OPERATION_NOT_SUPPORTED.
 *  If the client software selects a wrong key path handle, the function shall
 *  return CSD_ERROR_INVALID_KEY_PATH_HANDLE.
*/
TCsdStatus csdSetClearTextScrDecryptionKey
(
        TCsdUnsignedInt16           xEmi,
  const TCsdUnsignedInt8*          pxClearTextScrHostKey,
        TCsdSize                    xClearTextScrHostKeySize,
  const TCsdUnsignedInt8*      	   pxInitializationVector,
        TCsdSize                    xInitializationVectorSize,
        TCsdScrKeyPathHandle*      pxScrKeyPathHandle
);


/**
 *  @brief
 *    Set the protected key for the Stream Crypto Engine in Decryption.
 *
 *  @param[in]      xEmi
 *                   Algorithm which has to be used by the Stream
 *                   Crypto engine.
 *  @param[in]      xScrCipheredProtectingKeys
 *                   Array of Ciphered Protecting Keys
 *  @param[in]      pxCipheredScrContentKey
 *                   Pointer to the location where the function has to read the
 *                   Ciphered SCR Contentkey.
 *  @param[in]      xCipheredScrContentKeySize
 *                   Size in bytes of the Ciphered SCR Contentkey.
 *  @param[in]      pxInitializationVector
 *                   Pointer to the location where the function has to read the
*                    InitializationVector.
 *  @param[in]      xInitializationVectorSize
 *                   Size in bytes of the Initialization Vector.
 *  @param[in,out]  pxScrKeyPathHandle
 *                   Pointer to the location where the function has to read the
 *                   structure which defines the key path used to set the
 *                   Ciphered SCR Contentkey 
 *
 *  @retval   CSD_NO_ERROR
 *              The intended operation was executed successfully.
 *  @retval   CSD_ERROR_OPERATION_NOT_SUPPORTED
 *              The requested operation is not supported by the chipset.
 *  @retval   CSD_ERROR_INVALID_PARAMETERS
 *              At least one of the passed parameters is invalid.
 *  @retval   CSD_ERROR_INVALID_KEY_PATH_HANDLE
 *              The key path handle passed as parameter is invalid.
 *  @retval   CSD_ERROR
 *              The function terminated abnormally.
 *              The intended operation failed.
 *
 *  This function configures the key ladder with the Ciphered Protecting Keys
 *  and sets the CWs provided in protected form into the specified path of the
 *  Stream Crypto Engine.
 *  xScrCipheredProtectingKeys contains all the Ciphered Protecting Keys. The
 *  number of those keys contained in this array depends on the number of Key
 *  Ladder Elements which is defined by the constant
 *  CSD_SCR_NB_OF_PROTECTING_KEYS in csd_nocs1x_imp.h file.
 *  The size of these keys shall be:
 *      "	0 if the CW is not available.
 *      "	8 if the cryptographic algorithm is requesting 8 bytes.
 *      "	16 if the cryptographic algorithm  is requesting 16 bytes.
 *      "	In all other cases, the function shall return
 *          CSD_ERROR_INVALID_PARAMETERS.
 *  If a valid cryptographic algorithm is selected by the client software but
 *  the chipset does not support it, the function shall return
 *  CSD_ERROR_OPERATION_NOT_SUPPORTED.
 *  If the client software selects a wrong key path handle, the function shall
 *  return CSD_ERROR_INVALID_KEY_PATH_HANDLE.
*/
TCsdStatus csdSetProtectedScrDecryptionKey
(
        TCsdUnsignedInt16               xEmi,
  const TCsdScrCipheredProtectingKeys   xScrCipheredProtectingKeys,
  const TCsdUnsignedInt8*               pxCipheredScrContentKey,
        TCsdSize                        xCipheredScrContentKeySize,
  const TCsdUnsignedInt8*               pxInitializationVector,
        TCsdSize                        xInitializationVectorSize,
        TCsdScrKeyPathHandle*           pxScrKeyPathHandle
);


/**
 *  @brief
 *    Set the protected key for the Stream Crypto Engine in Encryption.
 *
 *  @param[in]      xEmi
 *                   Algorithm which has to be used by the Stream
 *                   Crypto engine.
 *  @param[in]      xScrCipheredProtectingKeys
 *                   Array of Ciphered Protecting Keys
 *  @param[in]      pxCipheredScrContentKey
 *                   Pointer to the location where the function has to read the
 *                   Ciphered SCR Contentkey.
 *  @param[in]      xCipheredScrContentKeySize
 *                   Size in bytes of the Ciphered SCR Contentkey.
 *  @param[in]      pxInitializationVector
 *                   Pointer to the location where the function has to read the
*                    InitializationVector.
 *  @param[in]      xInitializationVectorSize
 *                   Size in bytes of the Initialization Vector.
 *  @param[in,out]  pxScrKeyPathHandle
 *                   Pointer to the location where the function has to read the
 *                   structure which defines the key path used to set the
 *                   Ciphered SCR Contentkey 
 *
 *  @retval   CSD_NO_ERROR
 *              The intended operation was executed successfully.
 *  @retval   CSD_ERROR_OPERATION_NOT_SUPPORTED
 *              The requested operation is not supported by the chipset.
 *  @retval   CSD_ERROR_INVALID_PARAMETERS
 *              At least one of the passed parameters is invalid.
 *  @retval   CSD_ERROR_INVALID_KEY_PATH_HANDLE
 *              The key path handle passed as parameter is invalid.
 *  @retval   CSD_ERROR
 *              The function terminated abnormally.
 *              The intended operation failed.
 *
 *  This function configures the key ladder with the Ciphered Protecting Keys
 *  and sets the CWs provided in protected form into the specified path of the
 *  Stream Crypto Engine.
 *  xScrCipheredProtectingKeys contains all the Ciphered Protecting Keys. The
 *  number of those keys contained in this array depends on the number of Key
 *  Ladder Elements which is defined by the constant
 *  CSD_SCR_NB_OF_PROTECTING_KEYS in csd_nocs1x_imp.h file.
 *  The size of these keys shall be:
 *      "	0 if the CW is not available.
 *      "	8 if the cryptographic algorithm is requesting 8 bytes.
 *      "	16 if the cryptographic algorithm  is requesting 16 bytes.
 *      "	In all other cases, the function shall return
 *          CSD_ERROR_INVALID_PARAMETERS.
 *  If a valid cryptographic algorithm is selected by the client software but
 *  the chipset does not support it, the function shall return
 *  CSD_ERROR_OPERATION_NOT_SUPPORTED.
 *  If the client software selects a wrong key path handle, the function shall
 *  return CSD_ERROR_INVALID_KEY_PATH_HANDLE.
*/
TCsdStatus csdSetProtectedScrEncryptionKey
(
        TCsdUnsignedInt16               xEmi,
  const TCsdScrCipheredProtectingKeys   xScrCipheredProtectingKeys,
  const TCsdUnsignedInt8*               pxCipheredScrContentKey,
        TCsdSize                        xCipheredScrContentKeySize,
  const TCsdUnsignedInt8*               pxInitializationVector,
        TCsdSize                        xInitializationVectorSize,
        TCsdScrKeyPathHandle*           pxScrKeyPathHandle
); 


#endif /* CSD_H */

/* nocs1x_csd.h */
