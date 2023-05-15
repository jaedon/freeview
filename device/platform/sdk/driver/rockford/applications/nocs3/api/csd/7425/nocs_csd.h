/**
**  @file nocs_csd.h 
**
** @brief
**    Chipset Security Driver API for NOCS Secure chipset
**
**  $Id: //CAK/components/csdapi/MAIN/src/nocs_csd.h#8 $
**  $Change: 21216 $
**
** REMARK:
**   Comments in this file use special tags to allow automatic API documentation 
**   generation in HTML format, using the GNU-General Public Licensed Doxygen 
**   tool. For more information about Doxygen, please check www.doxygen.org
**
**   Depending on the platform, the CHM file may not open properly if it is 
**   stored on a network drive. So either the file should be moved on a local 
**   drive or add the following registry entry on Windows platform (regedit):\n
**   [HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\HTMLHelp\1.x\\ItssRestrictions] "MaxAllowedZone"=dword:00000003
*/  

#ifndef CSD_H
#define CSD_H

/**
 * Defines the version number of the CSD API. Note also that this version number 
 * does not correspond to the version number of the software version. 
 * @warning Do not update these values
*/
#define CSDAPI_VERSION_MAJOR  3
#define CSDAPI_VERSION_MEDIUM 3
#define CSDAPI_VERSION_MINOR  3

/** @page p_history History
 *
 *   - <b> 3.3.3 2012/05/25 Jean-Luc Bussy</b>
 *     - The function csdGetNuid() must apply to NOCS3.0 chipsets as well. 
 *       Modified the function description accordingly.
 *
 *   - <b> 3.3.2 2012/05/15 Jean-Luc Bussy</b>
 *     - Updated some doxygen group definitions
 *     - Fixed few minor description inconsistencies
 *
 *   - <b> 3.3.1 2012/05/02 Jean-Luc Bussy</b>
 *     - Removed all flash protection key encryption/decryption operations. Now 
 *       part of SEC API RAM2RAM operations.
 *     - Removed variable declaration for TCsdKeyLadderDeactivationFlag and 
 *       TCsdScsActivationFlag and made it types of TCsdActivationFlag 
 *
 *   - <b> 3.2.1 2012/03/01 11:30:00 Nocs team</b>
 *     - Cosmetical changes only. Added Copyright information in order to generate 
 *       new chm file.
 *
 *   - <b> 3.2.0 2012/02/22 11:30:00 Nocs team</b>
 *     - Added NOCS1.x functions csdGetNuid(), csdSetScsTotalAreaSize() 
 *       and csdGetScsTotalAreaSize(), csdEncryptDataWithClearTextHostKey(),
 *       csdDecryptDataWithClearTextHostKey(), csdEncryptDataWithSecretContentKey()
 *       and csdEncryptDataWithSecretContentKey(). 
 *     - Removed the functions csdEncryptData(), csdDecryptData(), 
 *       csdSetClearTextRam2RamHostKey(), csdSetProtectedRam2RamContentKeys(). 
 *     - Removed csdEncryptDataWithClearTextFlashProtectionKey().
 *  
 *   - <b> 3.1.0 2011/12/13 11:30:00 Nocs team</b>
 *     - Added csdGetApiVersion function
 *  
 *   - <b> 3.0.1 2011/08/15 11:30:00 Nocs team </b>
 *     - Updated comments 
 *     - Added the following functions:
 *       - csdSetRamUnscrambledStorageSize
 *       - csdGetRamUnscrambledStorageSize
 *     - Change CSD_SCS_PV_DTE_BOOT_CODE_AREA_SIZE name to
 *       CSD_SCS_PV_SCS_TOTAL_AREA_SIZE.
 *     - Added missing type: TCsdFPKeyPathHandle
 *  
 *   - <b> 3.0.0 2011/07/29 11:30:00 Nocs team </b>
 *     - First version to be used for NOCS3 Certification.
 */

/******************************************************************************/
/*                                                                            */
/*                               INCLUDE FILES                                */
/*                                                                            */
/******************************************************************************/

#include <stddef.h>


/******************************************************************************/
/*                                                                            */
/*                             OVERALL DOCUMENTATION                          */
/*                                                                            */
/******************************************************************************/

/** 
 * @mainpage Overview
 *  - @subpage p_preface
 *  - @subpage p_history
 *  - @subpage p_emi
 *  
*  <hr>Copyright &copy; 2012 Nagravision. All rights reserved.\n
 *  CH-1033 Cheseaux, Switzerland\n
 *  Tel: +41 21 7320311  Fax: +41 21 7320100\n
 *  http://www.nagra.com
 *
 *  All trademarks and registered trademarks are the property of their respective
 *  owners.
 *
 *  This document is supplied with an understanding that the notice(s) herein or
 *  any other contractual agreement(s) made that instigated the delivery of a
 *  hard copy, electronic copy, facsimile or file transfer of this document are
 *  strictly observed and maintained.
 *
 *  The information contained in this document is subject to change without notice.
 *
 *  <b>Security Policy of Nagravision Kudelski Group</b>\n
 *  Any recipient of this document, without exception, is subject to a
 *  Non-Disclosure Agreement (NDA) and access authorization.
 */

/** @page p_preface Preface
 *
 *  <h2>Purpose</h2>
 *  This document is intended for the manufacturers of MPEG secure chipsets who 
 *  are partners of Nagravision and whose chipset implements security features as 
 *  required  by NOCS requirement specification. \n
 *  This document specifies the CSD - Chipset Security Driver Application Programming 
 *  Interface that exposes security functionalities defined in the NOCS specification.\n
 *  It is assumed that the reader is familiar with the NOCS concepts, requirements 
 *  and recommendations.\n
 *  This API shall be implemented by the chipset manufacturers for any new NOCS secure
 *  chipset that is candidate to a Nagra NOCS certification.
 *
 *  <hr><h2>Audience</h2>
 *  This document is intended for MPEG secure chipset manufacturers implementing NOCS 
 *  requirements and candidate to a NOCS certification.
 *  
 *  <hr><h2>References</h2>
 *  - [EMI] Nagravision, System Architecture, Encryption Method Indicator Specification, 
 *    Version 1.0.7\n
 *  - [NOCS3REQ] Nagravision, NOCS3.0 Security Requirements Specification\n
 */

/** @page p_emi Encryption Method Indicator (EMI)
 *  
 *  There are many ways to encrypt data. An encryption algorithm (like AES, 
 *  TDES, ...) with optionally its associated mode of operation (like ECB, CBC 
 *  or CTR ) has, of course, to be chosen but this is often not enough to 
 *  specify unequivocally an encryption method. Indeed, some less obvious 
 *  information like data handling and key formats have also to be well defined 
 *  if interoperability between different platforms shall be ensured. Moreover 
 *  it is impossible to build a pre-defined exhaustive list of parameters 
 *  qualifying explicitly each Encryption Method because the parameters needed 
 *  could be specific to a given Encryption Method only. 
 *  
 *  Therefore, in order to deal with this complexity and to have a mean to 
 *  uniquely identify the way data are encrypted/decrypted, Nagravision 
 *  allocates a unique 16-bits identifier, referred to as EMI, to all referenced 
 *  Encryption Methods (see [EMI]). Each Encryption Method identified by its EMI 
 *  has a dedicated detailed specification. That specification might either be 
 *  an international standard or a Nagravision specification.
 *  
 */

/******************************************************************************/
/*                                                                            */
/*                              GROUPS DEFINITION                             */
/*                                                                            */
/******************************************************************************/

/** @defgroup driversetting Driver Settings */

/** @defgroup chipinfo Chipset Configuration */

/** @defgroup scsboot SCS & Boot Configuration */

/** @defgroup memprot Memory Protection 
 *  @defgroup flashprot Flash Protection 
 *  @ingroup memprot
 *
 *  @defgroup ramprot RAM Protection 
 *  @ingroup memprot
*/

/** @defgroup debug Debug Interface */

/** @defgroup ram2ram RAM 2 RAM Operations */

/** @defgroup dvr DVR Key Safe */

/** @defgroup dsc DSC Functions */

/** @defgroup scr SCR Functions*/


/******************************************************************************/
/*                                                                            */
/*                              VERSION TOOL                                  */
/*                                                                            */
/******************************************************************************/

#ifndef CSD_TOOL_VERSION
#define CSD_TOOL_STRINGIFY(s) CSD_TOOL_TOSTRING(s)
#define CSD_TOOL_TOSTRING(s)  #s

#define CSD_TOOL_VERSION_INT(a, b, c) (a<<16 | b<<8 | c)
#define CSD_TOOL_VERSION_DOT(a, b, c) a ##.## b ##.## c
#define CSD_TOOL_VERSION(a, b, c) CSD_TOOL_VERSION_DOT(a, b, c)
#endif

#define CSDAPI_VERSION_INTER CSD_TOOL_VERSION(CSDAPI_VERSION_MAJOR, \
              CSDAPI_VERSION_MEDIUM, \
              CSDAPI_VERSION_MINOR)

#define CSDAPI_VERSION_INT   CSD_TOOL_VERSION_INT(CSDAPI_VERSION_MAJOR, \
            CSDAPI_VERSION_MEDIUM, \
            CSDAPI_VERSION_MINOR)

#define CSDAPI_VERSION_STRING  "CSDAPI_" CSD_TOOL_STRINGIFY(CSDAPI_VERSION_INTER)



/******************************************************************************/
/*                                                                            */
/*                              TYPE DEFINITION                               */
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
 *    SCsdInitParameters is a chipset specific structure that has to be defined
 *    in nocs_csd_impl.h.
*/

typedef struct SCsdInitParameters TCsdInitParameters;

/**
 *  @brief
 *    Define the parameters for the termination of the Chipset Security Driver.
 *
 *    SCsdTerminateParameters is a chipset specific structure that has to be
 *    defined in nocs_csd_impl.h.
*/

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
 *  CheckNumber and NUID values. 
*/
typedef unsigned char TCsd4BytesVector[4];

/**
 *  @brief
 *    64 bits vectors.
 *
 *  TCsd8BytesVector defines a 64 bits vector for the NUID 64 bit
 *  value. 
*/
typedef unsigned char TCsd8BytesVector[8];

/**
 *  @brief
 *    Define the set of available memory interface during the chipset boot.
 *
 *  ECsdBootMode is a chipset specific enumeration to be defined in 
 *  nocs_csd_impl.h. Each implementation shall define its own set of available 
 *  memory interface from which the chipset is able to boot according to its 
 *  capabilities.
*/
typedef enum ECsdBootMode TCsdBootMode;


/**
 *  @brief
 *    Define the set of available NOCS3 SCS sizes.
 *
 *  ECsdScsSize is a chipset specific enumeration to be defined in 
 *  nocs_csd_impl.h. Each implementation shall define its own set of 4 available 
 *  sizes from which the chipset is able to boot.
*/
typedef enum ECsdScsSize TCsdScsSize;

/**
 *  @brief
 *    Defines the state of the Activation Flag of either the SCS, Scrambling or Flash Protection.
 *
 *  Defines the state of the SCS, Scrambling or Flash Protection Activation Flag.
*/
typedef enum
{
  CSD_ACTIVATION_FLAG_SET = 0,
    /**< The PV value is set or the functionality is enabled.
     */
  CSD_ACTIVATION_FLAG_NOT_SET,
    /**< The PV value is not see or the functionality is disabled.
     */
  LAST_CSD_ACTIVATION_FLAG
    /**< Upper boundary of the TCsdActivationFlag type.
     */
}TCsdActivationFlag;

/*
 * @brief
 * NOCS1.x Type, defines the state of the SCS Activation, used by the function 
 * csdGetScsActivationFlag().
 *
 */
typedef TCsdActivationFlag TCsdScsActivationFlag;
    /**< Defines the state of the SCS Activation, used by the function 
     * csdGetScsActivationFlag().
     */
typedef TCsdActivationFlag TCsdKeyLadderDeactivationFlag;
    /**< Defines the state of the Key ladder deactivation, used by the 
     * NOCS1.x function API csdGetKeyLadderDeactivationFlag().
     */


/**
 *  @brief
 *    Defines names related to the SCS Persistent Values.
 *
*/
typedef enum
{
  CSD_SCS_PV_EXT_BOOT_FLASH_DEV_PARAMS = 0,
    /**< External Boot Flash Device Parameters: PV to configure the External 
     * Non-Volatile Storage.
     */
  CSD_SCS_PV_EXT_RAM_PARAMS,
    /**< External RAM Parameters: PV to configure the External RAM.
     */
  CSD_SCS_PV_SCS_TOTAL_AREA_SIZE,
    /**< SCS Total Area Size: PV to set the SCS Total Area Size.
     */
  CSD_SCS_PV_ACTIVE_INDICATOR,
    /**< Active Indicator: PV to determine the active region of the Ext. SCS FW.
     */
  CSD_SCS_PV_FULL_EXT_FW_UPGRADE_ADDR,
    /**< Full External Firmware Upgrade Address: PV to set the address of the Full External
     * FW Upgrade.
     */
  CSD_SCS_PV_FULL_EXT_FW_UPGRADE_SIZE,
    /**< Full External Firmware Upgrade Size: PV to set the size of the Full External
     * FW Upgrade.
     */
  CSD_SCS_PV_PATCH_EXT_FW_UPGRADE_ADDR,
    /**< Patch External Firmware Upgrade Address: PV to set the address of the Patch External
     * FW Upgrade.
     */
  CSD_SCS_PV_PATCH_EXT_FW_UPGRADE_SIZE,
    /**< Patch External Firmware Upgrade Size: PV to set the Size of the Patch External
     * FW Upgrade.
     */
  CSD_SCS_PV_VERSIONING_REF,
    /**< Versioning_Ref: PV to set Versioning_REF value.
     */
  LAST_CSD_SCS_PV_ID
    /**< Upper boundary of the TCsdScsPvId type.
     */
}TCsdScsPvId;

/*!
 *  @brief
 *    NOCS1.x Type\n
 *    Size of SCS_Total_Area.
 *
 *  Defines the size of the SCS_Total_Area as defined in NOCS 1.X.
*/
typedef enum
{
  CSD_SCS_TOTAL_AREA_SIZE_16K = 0,
    /*!< The total area size checked by SCS is of 16 Kbytes.
     */
  CSD_SCS_TOTAL_AREA_SIZE_32K,
    /*!< The total area size checked by SCS is of 32 Kbytes.
     */
  CSD_SCS_TOTAL_AREA_SIZE_64K,
    /*!< The total area size checked by SCS is of 64 Kbytes.
     */
  CSD_SCS_TOTAL_AREA_SIZE_128K,
    /*!< The total area size checked by SCS is of 128 Kbytes.
     */
  CSD_SCS_TOTAL_AREA_SIZE_256K,
    /*!< The total area size checked by SCS is of 256 Kbytes.
     */
  CSD_SCS_TOTAL_AREA_SIZE_UNPROGRAMMED,
    /*!< The total area size checked by SCS is not programmed.
     */
  CSD_SCS_TOTAL_AREA_SIZE_NOT_SUPPORTED,
    /*!< The total area size checked by SCS is not supported.
     */
  LAST_CSD_SCS_TOTAL_AREA_SIZE
    /*!< Upper boundary of the TCsdScsTotalAreaSize type.
     */
}TCsdScsTotalAreaSize;

/*!
 *  @brief
 *    NOCS1.x Type\n
 *    Define the cryptographic algorithm supported by Nagra for the RAM2RAM
 *    Crypto Engine operations.
 *
*/
typedef enum
{
  CSD_R2R_ALGORITHM_AES_128 = 0,
    /*!< AES 128 bits defined in FIPS-197, Advanced Encryption Standard.
     */
  CSD_R2R_ALGORITHM_TDES_K1K2K1,
    /*!< TDES algorithm in key option 2 defined in FIPS-46-3, Data
         Encryption Standard (DES).
     */
  LAST_CSD_R2R_ALGORITHM
    /*!< Upper boundary of the TCsdAlgorithm type.
     */
}TCsdR2RAlgorithm;

/*!
 *  @brief
 *    NOCS1.x Type\n
 *    Define the cryptographic algorithm mode supported by Nagra for the data
 *    operations.
 *
 *  Define the cryptographic algorithm mode supported by Nagra for the data
 *  operations except the received transport stream processing.
*/
typedef enum
{
  CSD_R2R_CRYPTO_OPERATION_MODE_ECB = 0,
    /*!< Electronic Code Book mode 
     */
  CSD_R2R_CRYPTO_OPERATION_MODE_CBC,
    /*!< Ciphered Block Chaining mode
     */
  CSD_R2R_CRYPTO_OPERATION_MODE_CTR,
    /*!< Counter mode
     */
  LAST_CSD_R2R_CRYPTO_OPERATION_MODE
    /*!< Upper boundary of the TCsdR2RCryptoOperationMode type.
     */
}
TCsdR2RCryptoOperationMode;


/**
 *  @brief
 *   SCsdScsPvPathHandle is a chipset specific structure to be defined in 
 *   nocs_csd_impl.h. It holds all relevant information needed by the CSD in 
 *   order to set PV Params into the appropriated location. 
*/ 
typedef struct SCsdScsPvPathHandle TCsdScsPvPathHandle;


/**
 *  @brief
 *    Defines the debug interface access mode.
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
 *    Define a type as an 32 bits vector.
 *
 *  Definition of a type as an 32 bits vector. 
*/
typedef unsigned long int     TCsdUnsignedInt32;

/**
 *  @brief
 *    Define the Size data type.
 *
 *  Re-definition of the size from the size_t defined in the stddef.h file.
*/
typedef size_t TCsdSize;


/**
 *  @brief
 *   SCsdScrKeyPathHandle is a chipset specific structure to be defined in 
 *   nocs_csd_impl.h. It holds all relevant information needed by the CSD in 
 *   order to set the key of the Stream Crypto Engine into the appropriated 
 *   location.
*/ 
typedef struct SCsdScrKeyPathHandle TCsdScrKeyPathHandle;

/**
 *  @brief
 *   SCsdDscKeyPathHandle is a chipset specific structure to be defined in 
 *   nocs_csd_impl.h. It holds all relevant information needed by the CSD in 
 *   order to set the key, used by the Transport Stream Crypto Engine, into the 
 *   appropriated location.
*/ 
typedef struct SCsdDscKeyPathHandle TCsdDscKeyPathHandle;

/**
 *  @brief
 *   SCsdR2RKeyPathHandle is a chipset specific structure to be defined in 
 *   nocs_csd_impl.h. It holds all relevant information needed by the CSD in 
 *   order to set the key, used by the RAM2RAM Crypto Engine, into the 
 *   appropriated location.
*/ 
typedef struct SCsdR2RKeyPathHandle TCsdR2RKeyPathHandle;

/**
 *  @brief
 *   TCsdR2RCipheredProtectingKeys defines an array of 16-bytes R2R Ciphered
 *   Protecting Keys.
 *   CSD_R2R_NB_OF_PROTECTING_KEYS defines the number of Ciphered Protecting
 *   Keys for the Key Ladder. The default value is set to 2 as required in NOCS3. 
*/
#ifndef CSD_R2R_NB_OF_PROTECTING_KEYS
#define CSD_R2R_NB_OF_PROTECTING_KEYS 2
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
 *   Keys for the Key Ladder. The default value is set to 2 as required in NOCS3. 
*/
#ifndef CSD_DSC_NB_OF_PROTECTING_KEYS
#define CSD_DSC_NB_OF_PROTECTING_KEYS 2
#endif

typedef TCsdUnsignedInt8 TCsdDscCipheredProtectingKeys \
          [CSD_DSC_NB_OF_PROTECTING_KEYS][16];

/**
 *  @brief
 *   TCsdScrCipheredProtectingKeys defines an array of 16-bytes SCR Ciphered
 *   Protecting Keys.
 *   CSD_SCR_NB_OF_PROTECTING_KEYS defines the number of Ciphered Protecting
 *   Keys for the Key Ladder. The default value is set to 2 as required in NOCS3. 
*/
#ifndef CSD_SCR_NB_OF_PROTECTING_KEYS
#define CSD_SCR_NB_OF_PROTECTING_KEYS 2
#endif

typedef TCsdUnsignedInt8 TCsdScrCipheredProtectingKeys \
          [CSD_SCR_NB_OF_PROTECTING_KEYS][16];

/*
 * EXTERNAL_CSD_STATUS_OFFSET could be defined in nocs_csd_impl.h and so,
 * allows the chipset manufacturer to define an offset on the lowest status
 * value returned by the CSD driver to specify its own return status code range.
 * By default CSD_STATUS_OFFSET will be set to 0.
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
 *  @ingroup driversetting
 *
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
 *  This function has to be called once before any other function of the CSD
 *  API. This function initializes the dedicated hardware is required.
 *
 *  pxInitParameters is a pointer to a chipset specific structure. It has to be
 *  set prior to any initialization of CSD driver. In the case where external
 *  drivers would be used by the CSD, these drivers shall be initialized outside
 *  the CSD and before any calls to the CSD API.
*/
TCsdStatus csdInitialize
(
  TCsdInitParameters*  pxInitParameters
);

/**
 *  @ingroup driversetting
 *
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
 *  @ingroup driversetting
 *
 *  @brief
 *    Return the CSD API version
 *
 *  @param[out]     pxCsdApiVersion
 *                   32 bit value where the function has to write the CSD 
 *                   driver API version using the macro CSDAPI_VERSION_INT.
 *
 *  @retval   CSD_NO_ERROR
 *              The intended operation was executed successfully.
 *  @retval   CSD_ERROR_INVALID_PARAMETERS
 *              pxCsdApiVersion parameter is invalid.
 *  @retval   CSD_ERROR
 *              The function terminated abnormally. The intended operation
 *              failed.
 *
 *  This function returns the version of the CSD API.  
 *  Use the macro CSDAPI_VERSION_INT to assign the right value.
*/
TCsdStatus csdGetApiVersion
(
  TCsdUnsignedInt32*  pxCsdApiVersion
);

/**
 *  @ingroup driversetting
 *
 *  @brief
 *    Return the current version of the CSD Driver Implementation
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
 *  This function returns the version of the CSD Driver Implementation. It is
 *  represented as a character string, the internal format being chipset
 *  manufacturer dependent. However, the length of the string is fixed at 20
 *  characters. The string must be terminated by the end character '\\0'.
*/
TCsdStatus csdGetSoftwareVersion
(
  TCsd20CharsString  xSoftwareVersion
);

/**
 *  @ingroup chipinfo
 *
 * @brief
 *   Return the current revision of the chipset silicon
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
 *  string returned by the function is a maximum size of 20 characters including
 *  the end character '\\0'.
*/
TCsdStatus csdGetChipRevision
(
  TCsd20CharsString xChipRevision
);

/**
 *  @ingroup chipinfo
 *
 * @brief
 *   Return the Chip Persistent Value Configuration (chip extension)
 *
 *  @param[out]     xChipExtension
 *                   20 characters string where the function has to write the
 *                   chipset configuration string.
 *
 *  @retval   CSD_NO_ERROR
 *              The intended operation was executed successfully.
 *  @retval   CSD_ERROR
 *              The function terminated abnormally. The intended operation
 *              failed.
 *
 *  This function returns the chip extension (Persistent Value Configuration) 
 *  for a given chipset.
 *  It is represented as a character string, the internal format being chipset
 *  manufacturer dependent. However, this internal format has to be agreed with
 *  Nagravision and the chipset manufacturer. In any case, the size of the
 *  string returned by the function is fixed at 20 characters. The string must
 *  be terminated by the end character '\\0'.
*/
TCsdStatus csdGetChipExtension
(
  TCsd20CharsString xChipExtension
);

/*!
 *  @ingroup chipinfo
 *
 *  @brief
 *    Get the chip Nagra Unique Id (NUID)
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
 *  For NOCS1.x, this function returns the NagraUniqueID32 that is programmed
 *  into the on-chip non-modifiable memory by the chipset manufacturer.
 *
 *  For NOCS3.0, this function returns the 32 least significant bits of 
 *  NagraUniqueID64 that is programmed into the on-chip non-modifiable memory by
 *  the chipset manufacturer. 
*/
TCsdStatus csdGetNuid
(
  TCsd4BytesVector xNuid
);

/**
 *  @ingroup chipinfo
 *
 *  @brief
 *    Get the chip Nagra Unique Id
 *
 *  @param[out]     xNuid64
 *                   8-bytes vector where the function has to write the NUID64.
 *
 *  @retval   CSD_NO_ERROR
 *              The intended operation was executed successfully.
 *  @retval   CSD_ERROR
 *              The function terminated abnormally. The intended operation
 *              failed.
 *
 *  This function returns the NagraUniqueID64 (NUID) that
 *  is programmed into the NOCS3 on-chip non-modifiable memory by the chipset
 *  manufacturer.
*/
TCsdStatus csdGetNuid64
(
  TCsd8BytesVector xNuid64
);

/**
 *  @ingroup chipinfo
 *
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
 *  NUID Reporting procedure. The computation of NUID Check Number is
 *  chipset-specific, and is the result of a computation involving the
 *  NUID64 and the Master Root Key. 
 *  In case of NOCS1.x implementation, Nagravision and the chipset
 *  manufacturer will commonly agree during the NOCS certification process on 
 *  how to compute the NUID Check Number for this secure chipset.
*/
TCsdStatus csdGetNUIDCheckNumber
(
  TCsd4BytesVector xNUIDCheckNumber
);

/**
 *  @ingroup chipinfo
 *
 *  @brief
 *    Return the CSC Data check number of the chipset.
 *
 *  @param[in]      xCSCData
 *                   16-bytes vector where the function has to 
 *		     read the Data used to compute the check number.
 *  @param[out]     xCSCDCheckNumber
 *                   4-bytes vector where the function has to write the CSC
 *                   Data Check Number.
 *
 *  @retval   CSD_NO_ERROR
 *              The intended operation was executed successfully.
 *  @retval   CSD_ERROR_INVALID_PARAMETERS
 *              The passed parameter is invalid.
 *  @retval   CSD_ERROR
 *              The function terminated abnormally. The intended operation
 *              failed.
 *
 *  This function returns the CSC Data Check Number, as requested by the 
 *  Nagravision NUID Reporting procedure [2]. The computation of CSC Data 
 *  Check Number is chipset-specific, and is the result of a computation 
 *  involving the TDES-RootKey128, a 16-bytes data as well as the RAM2RAM Crypto 
 *  Engine. As there may be several ways of configuring the RAM2RAM Crypto 
 *  Engine and its Key Ladder to compute the CSC Data Check Number, Nagravision 
 *  and the chipset manufacturer will commonly agree during the NOCS 
 *  certification process of a chipset on how to compute the CSC Data Check 
 *  Number for this secure chipset. Refer to the Security Driver Implementation
 *  Proposal document for more details about this computation.
*/
TCsdStatus csdGetCSCDCheckNumber
(
  const TCsdUnsignedInt8    xCSCData[16],
        TCsd4BytesVector    xCSCDCheckNumber
);

/**
 *  @ingroup chipinfo
 *
 *  @brief
 *    Return the STBCASN check number of the chipset.
 *
 *  @param[in]      xSTBCASNData
 *                   4-bytes vector where the function has to 
 *		     read the Data used to compute the check number.
 *  @param[out]     xSTBCASNCheckNumber
 *                   4-bytes vector where the function has to write the STB CA
 *                   Check Number.
 *
 *  @retval   CSD_NO_ERROR
 *              The intended operation was executed successfully.
 *  @retval   CSD_ERROR_INVALID_PARAMETERS
 *              The passed parameter is invalid.
 *  @retval   CSD_ERROR
 *              The function terminated abnormally. The intended operation
 *              failed.
 *
 *  This function returns the STBCASN Check Number. The computation of STBCASN 
 *  Check Number is chipset-specific, and is the result of a computation 
 *  involving the TDES-RootKey128, a 4-bytes data as well as the RAM2RAM Crypto 
 *  Engine. As there may be several ways of configuring the RAM2RAM Crypto 
 *  Engine and its Key Ladder to compute the STBCASN Check Number, Nagravision 
 *  and the chipset manufacturer will commonly agree during the NOCS 
 *  certification process of a chipset on how to compute the STBCASN Check 
 *  Number for this secure chipset. Refer to the Security Driver Implementation
 *  Proposal document for more details about this computation.
*/
TCsdStatus csdGetSTBCASNCheckNumber
(
  const TCsdUnsignedInt8    xSTBCASNData[4],
        TCsd4BytesVector    xSTBCASNCheckNumber
);

/**
 *  @ingroup chipinfo
 *
 *  @brief
 *    Return the Data Integrity check number of the chipset.
 *
 *  @param[in]      pxData
 *                   Pointer to the location where the function has to 
 *					         read the Data used to compute the check number.
 *  @param[in]      xDataSize
 *                   Size in bytes of the Data.
 *  @param[out]     xDataIntegrityCheckNumber
 *                   4-bytes vector where the function has to write the Data 
 *                   Integrity Check Number.
 *
 *  @retval   CSD_NO_ERROR
 *              The intended operation was executed successfully.
 *  @retval   CSD_ERROR_INVALID_PARAMETERS
 *              The passed parameter is invalid.
 *  @retval   CSD_ERROR
 *              The function terminated abnormally. The intended operation
 *              failed.
 *
 *  This function returns the Data Integrity Check Number. The computation of 
 *  Data Integrity Check Number is chipset-specific, and is the result of a 
 *  computation involving the TDES-RootKey128, a data as well as the RAM2RAM 
 *  Crypto Engine. As there may be several ways of configuring the RAM2RAM 
 *  Crypto Engine and its Key Ladder to compute the Data Integrity Check Number,
 *  Nagravision and the chipset manufacturer will commonly agree during the NOCS
 *  certification process of a chipset on how to compute the Data Integrity 
 *  Check Number for this secure chipset. Refer to the Security Driver Implementation
 *  Proposal document for more details about this computation.
 *  The size in byte of xDataSize shall be defined between 1 and 16 bytes. 
 *  Otherwise, the function shall return CSD_ERROR_INVALID_PARAMETERS.
*/
TCsdStatus csdGetDataIntegrityCheckNumber
(
  const TCsdUnsignedInt8* pxData,
  		TCsdSize           xDataSize,
        TCsd4BytesVector   xDataIntegrityCheckNumber
);

/**
 *  @ingroup chipinfo
 *
 *  @brief
 *    Set the STBCASN.
 *
 *  @param[in]      xStbCaSn
 *                   4-bytes vector to which the STBCASN is requested to
 *                   be set.
 *
 *  @retval   CSD_NO_ERROR
 *              The intended operation was executed successfully.
 *  @retval   CSD_ERROR_INVALID_PARAMETERS
 *              The passed parameter is invalid.
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
 *  @ingroup chipinfo
 *
 *  @brief
 *    Get the STBCASN of the chipset.
 *
 *  @param[out]     xStbCaSn
 *                   4-bytes vector to which the STBCASN is requested to
 *                   be set.
 *
 *  @retval   CSD_NO_ERROR
 *              The intended operation was executed successfully.
 *  @retval   CSD_ERROR_INVALID_PARAMETERS
 *              The passed parameter is invalid.
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
 *  @ingroup chipinfo
 *
 *  @brief
 *    Set the chipset market segment ID.
 *
 *  @param[in]      xMarketSegmentId
 *                   4-bytes vector to which the MarketSegmentId is requested to
 *                   be set.
 *
 *  @retval   CSD_NO_ERROR
 *              The intended operation was executed successfully.
 *  @retval   CSD_ERROR_INVALID_PARAMETERS
 *              The passed parameter is invalid.
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
 *  @ingroup chipinfo
 *
 *  @brief
 *    Get the market segment ID of the chipset.
 *
 *  @param[out]     xMarketSegmentId
 *                   4-bytes vector where the function has to write the
 *                   MarketSegmentId.
 *
 *  @retval   CSD_NO_ERROR
 *              The intended operation was executed successfully.
 *  @retval   CSD_ERROR_INVALID_PARAMETERS
 *              The passed parameter is invalid.
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
 *  @ingroup scsboot
 *
 * @brief
 *   Set the chipset active memory interface during boot.
 *
 *  @param[in]      xBootMode
 *                   Mode which has to be used by the Boot Mode is requested to
 *                   be set.
 *
 *  @retval   CSD_NO_ERROR
 *              The intended operation was executed successfully.
 *  @retval   CSD_ERROR_INVALID_PARAMETERS
 *              The boot mode passed as parameter is out of type.
 *  @retval   CSD_ERROR_OPERATION_NOT_ALLOWED
 *              The boot mode has already been set to a different valid boot
 *              mode and has been locked.
 *  @retval   CSD_ERROR_OPERATION_NOT_SUPPORTED
 *              The requested operation is not supported by the chipset.
 *  @retval   CSD_ERROR
 *              The function terminated abnormally. The intended operation
 *              failed.
 *
 *  This function allows the client software to configure the Boot Mode of the
 *  chipset. Once this mode has been been set, the chipset will always boot in
 *  this Boot Mode. TCsdBootMode is a specific enumeration type which has to
 *  be defined by the chipset manufacturer depending on the Boot Mode supported
 *  by the chipset.
 *  If the Boot Mode passed as parameter is already set when this function is
 *  called or if the function has successfully set the Boot Mode, the function
 *  shall return CSD_NO_ERROR. If the Boot Mode is already set and the client
 *  software tries to set it to a different value, the function shall
 *  return CSD_ERROR_OPERATION_NOT_ALLOWED.
 *  If the chipset does not need an internal configuration setting to boot, then 
 *  the function shall return CSD_ERROR_OPERATION_NOT_SUPPORTED 
*/
TCsdStatus csdSetBootMode
(
  TCsdBootMode xBootMode
);

/**
 *  @ingroup scsboot
 *
 * @brief
 *   Get the selected active memory interface during the boot.
 *
 *  @param[out]     pxBootMode
 *                   Pointer to the location where the function has to write the
 *                   Boot Mode.
 *
 *  @retval   CSD_NO_ERROR
 *              The intended operation was executed successfully.
 *  @retval   CSD_ERROR_INVALID_PARAMETERS
 *              The passed parameter is invalid.
 *  @retval   CSD_ERROR_OPERATION_NOT_SUPPORTED
 *              The requested operation is not supported by the chipset.
 *  @retval   CSD_ERROR
 *              The function terminated abnormally. The intended operation
 *              failed.
 *
 *  This function allows the client software to read the configured Boot Mode.
 *  If the hardware chipset technology allows an unprogrammed state for the Boot
 *  Mode, then the enumeration type shall define this state. The function shall
 *  return this state when it is necessary.
 *  If the chipset does not have an internal configuration setting to boot, then 
 *  the function shall return CSD_ERROR_OPERATION_NOT_SUPPORTED 
*/
TCsdStatus csdGetBootMode
(
  TCsdBootMode* pxBootMode
);

/**
 *  @ingroup scsboot
 *
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
 *  @ingroup scsboot
 *
 *  @brief
 *    Get the SCS Activation Flag
 *
 *  @param[out]     pxScsActivationFlagState
 *                   Pointer to the location where the function has to write the
 *                   SCS Activation Flag State.
 *
 *  @retval   CSD_NO_ERROR
 *              The intended operation was executed successfully.
 *  @retval   CSD_ERROR_INVALID_PARAMETERS
 *              The passed parameter is invalid.
 *  @retval   CSD_ERROR
 *              The function terminated abnormally. The intended operation
 *              failed.
 *
 *  This function allows the client software to read the SCS Activation Flag
 *  State.
*/
TCsdStatus csdGetScsActivationFlag
(
  TCsdActivationFlag* pxScsActivationFlagState
);

/**
 *  @ingroup scsboot
 *
 *  @brief
 *    Set the values of the SCS Persistent Value Parameters.
 *
 *  @param[in]      xScsPvParamsId
 *                   SCS Persistent Value Parameters Identification.
 *  @param[in,out]  pxPvPathHandle
 *                   Pointer to the location where the function has to read the
 *                   structure which defines the SCS PV path used during SCS.
 *  @param[in]      xPvLength
 *                   Size in bytes of pxPvValue arrays.
 *  @param[in]      pxPvValue
 *                   Pointer to the location where the to read the value of the field
 *                   
 *  @retval   CSD_NO_ERROR
 *              The intended operation was executed successfully.
 *  @retval   CSD_ERROR_INVALID_PARAMETERS
 *              One or several parameter are invalid.
 *  @retval   CSD_ERROR_OPERATION_NOT_ALLOWED
 *              The PV has already been set to a different value
 *              and has been locked.
 *  @retval   CSD_ERROR_OPERATION_NOT_SUPPORTED
 *              The requested PV Parameter is not supported by the chipset.
 *  @retval   CSD_ERROR
 *              The function terminated abnormally. The intended operation
 *              failed.
 *
 *  This function requests to switch the state of one of the NOCS related SCS
 *  Persistent Value bit fields identified by xScsPvParamsId to the state
 *  specified in pxPvValue.
 *  xPvLength specifies the length of the array of pxPvValue.  
 *  pxPvValue is an arrays of bytes. 
 *  If the Persistent Value configuration bit field is already in the requested
 *  state when this function is called or if the function has successfully set
 *  the Persistent Value configuration bit field in the requested state, the
 *  function shall return CSD_NO_ERROR. In all other cases, the call shall have
 *  no effect and the function shall return CSD_ERROR.
 *  If the xScsPvParamsId is already set and the client software tries to set
 *  it to a different value, the function shall return
 *  CSD_ERROR_OPERATION_NOT_ALLOWED.
 *  If the chipset does not implement the PV parameter requested or does not 
 *  support this function, then the call shall return 
 *  CSD_ERROR_OPERATION_NOT_SUPPORTED 
 *
*/
TCsdStatus csdSetScsPv
(
  TCsdScsPvId            xScsPvParamsId,
  TCsdScsPvPathHandle*  pxPvPathHandle, 
  TCsdUnsignedInt16      xPvLength,
  TCsdUnsignedInt8*     pxPvValue
);

/**
 *  @ingroup scsboot
 *
 *  @brief
 *    Get the SCS Persistent Value.
 *
 *  @param[in]      xScsPvParamsId
 *                   SCS Persistent Value Parameters Identification.
 *  @param[in,out]  pxPvPathHandle
 *                   Pointer to the location where the function has to read the
 *                   structure which defines the SCS PV path used during SCS.
 *  @param[in]      xPvLength
 *                   Size in bytes of the pxPvValue array
 *  @param[in]      pxPvValue
 *                   Pointer to the location where the to read the value of the field
 *                   
 *  @retval   CSD_NO_ERROR
 *              The intended operation was executed successfully.
 *  @retval   CSD_ERROR_INVALID_PARAMETERS
 *              One or several parameter are invalid.
 *  @retval   CSD_ERROR_OPERATION_NOT_SUPPORTED
 *              The requested PV Parameter is not supported by the chipset.
 *  @retval   CSD_ERROR
 *              The function terminated abnormally. The intended operation
 *              failed.
 *
 *  This function returns the state of the Persistent Value configuration bit
 *  field corresponding to the handle pxPvPathHandle passed as argument.
 *  xPvLength specifies the length of the array of pxPvValue. 
 *  pxPvValue is an array of bytes. 
 *  If the Persistent Value configuration bit field is already in the requested
 *  state when this function is called or if the function has successfully set
 *  the Persistent Value configuration bit field in the requested state, the
 *  function shall return CSD_NO_ERROR. In all other cases, the call shall have
 *  no effect and the function shall return CSD_ERROR.
 *  If the chipset does not implement the PV parameter requested or does not 
 *  support this function, then the call shall return 
 *  CSD_ERROR_OPERATION_NOT_SUPPORTED 
 */
TCsdStatus csdGetScsPv
(
  TCsdScsPvId            xScsPvParamsId,
  TCsdScsPvPathHandle*  pxPvPathHandle,
  TCsdUnsignedInt16      xPvLength,
  TCsdUnsignedInt8*     pxPvValue
);

/*!
 *  @ingroup scsboot
 *
 *  @brief
 *    NOCS1.x Function\n
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
 *  @retval   CSD_ERROR_OPERATION_NOT_SUPPORTED
 *              The requested operation is not supported by the chipset.
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
 *  If the function is not supported by the chispet it shall return 
 *  CSD_ERROR_OPERATION_NOT_SUPPORTED.
*/
TCsdStatus csdSetScsTotalAreaSize
(
  TCsdScsTotalAreaSize xScsTotalAreaSize
);

/*!
 *  @ingroup scsboot
 *
 *  @brief
 *    Get the set memory size which will be checked by the SCS.
 *
 *  @param[out]     pxTotalAreaSize
 *                   Pointer to the location where the function has to write the
 *                   SCS_Total_Area.
 *
 *  @retval   CSD_NO_ERROR
 *              The intended operation was executed successfully.
 *  @retval   CSD_ERROR_OPERATION_NOT_SUPPORTED
 *              The requested operation is not supported by the chipset.
 *  @retval   CSD_ERROR
 *              The function terminated abnormally. The intended operation
 *              failed.
 *
 *  This function allows the client software to read the SCS_Total_Area.
 *  if the chipset supports more sizes than the ones defined in NOCS 1.x [1] 
 *  and the function reads one of these values or the function does not support
 *  this function, it shall return CSD_ERROR_OPERATION_NOT_SUPPORTED.
 *  For any other Error the function shall return CSD_ERROR
 */
TCsdStatus csdGetScsTotalAreaSize
(
  TCsdScsTotalAreaSize* pxTotalAreaSize
);


/**
 *  @ingroup ramprot
 *
 *  @brief
 *    Set the RAM Unscrambled Storage Size.
 *
 *  @param[in]      pxPvRamUnscrambledStorageSize
 *                   Persistent Value in MSBF which will defines the Unscrambled Storage Size.
 *
 *  @param[in]      xPvLength
 *                   Size in bytes of the pxUnscrambledStorageSize array
 *
 *  @retval   CSD_NO_ERROR
 *              The intended operation was executed successfully.
 *  @retval   CSD_ERROR_INVALID_PARAMETERS
 *              The passed parameter is invalid.
 *  @retval   CSD_ERROR_OPERATION_NOT_ALLOWED
 *              The RAM Unscrambled Storage Size has already been set to a different value
 *              and has been locked.
 *  @retval   CSD_ERROR_OPERATION_NOT_SUPPORTED
 *              The function is not supported by the chipset.
 *  @retval   CSD_ERROR
 *              The function terminated abnormally. The intended operation
 *              failed.
 *
 *  This function allows the client software to set the Ram Unscrambled Storage Size.
 *  The Ram Unscrambled Storage Size is defined as a MSBF byte vector.
 *  If the Ram Unscrambled Storage Size is already set and the client software tries to set
 *  it to a different value, the function shall return
 *  CSD_ERROR_OPERATION_NOT_ALLOWED.
 *  If at least one parameter is incorrect or not defined the function shall return
 *  CSD_ERROR_INVALID_PARAMETERS.
 *  If the function is not supported by the chipset, it shall return 
 *  CSD_ERROR_OPERATION_NOT_SUPPORTED. 
 *  If the operation to set the Persistent Value failed the function shall return
 *  CSD_ERROR
*/
TCsdStatus csdSetRamUnscrambledStorageSize
(
  TCsdUnsignedInt8*  pxPvRamUnscrambledStorageSize,
  TCsdUnsignedInt16   xPvLength
);

/**
 *  @ingroup ramprot
 *
 *  @brief
 *    Get the RAM Unscrambled Storage Size.
 *
 *  @param[out]     pxPvRamUnscrambledStorageSize
 *                   Pointer to the location where the function has to write the
 *                   Persistent Value in MSBF which will defines the Ram Unscrambled Storage Size.
 *
 *  @param[in]      xPvLength
 *                   Size in bytes of the pxUnscrambledStorageSize array
 *
 *  @retval   CSD_NO_ERROR
 *              The intended operation was executed successfully.
 *  @retval   CSD_ERROR_INVALID_PARAMETERS
 *              The passed parameter is invalid.
 *  @retval   CSD_ERROR_OPERATION_NOT_SUPPORTED
 *              The function is not supported by the chipset.
 *  @retval   CSD_ERROR
 *              The function terminated abnormally. The intended operation
 *              failed.
 *
 *  This function allows the client software to read the Unscrambled Storage Size. 
 *  The output is a MSBF bytes vector.
 *  If at least one parameter is incorrect or not defined the function shall return
 *  CSD_ERROR_INVALID_PARAMETERS.
 *  If the function is not supported by the chipset, it shall return CSD_ERROR_OPERATION_NOT_SUPPORTED. 
 *  If the operation to set the Persistent Value failed the function shall return
 *  CSD_ERROR
 */
TCsdStatus csdGetRamUnscrambledStorageSize
(
  TCsdUnsignedInt8*  pxPvRamUnscrambledStorageSize,
  TCsdUnsignedInt16   xPvLength
);

/**
 *  @ingroup ramprot
 *
 *  @brief
 *    Enable the RAM Scrambling
 *
 *  @retval   CSD_NO_ERROR
 *              The intended operation was executed successfully.
 *  @retval   CSD_ERROR_OPERATION_NOT_SUPPORTED
 *              The function is not supported by the chipset.
 *  @retval   CSD_ERROR
 *              The function terminated abnormally. The intended operation
 *              failed.
 *
 *  This function allows the client software to enable the Scrambling mechanism. Once
 *  enabled, this feature cannot be disabled. If the RAM Scrambling feature is 
 *  already enabled when this function is called or if the function has successfully 
 *  enabled the RAM Scrambling feature, the function shall return CSD_NO_ERROR. 
 *  If the function is not supported by the chipset, it shall return CSD_ERROR_OPERATION_NOT_SUPPORTED. 
 *  In all other cases, the call shall have no effect and the function shall 
 *  return CSD_ERROR.
*/
TCsdStatus csdEnableRamScrambling
(
  void
);

/**
 *  @ingroup ramprot
 *
 *  @brief
 *    Get the RAM Scrambling Activation Flag
 *
 *  @param[out]     pxScramblingActivationFlagState
 *                   Pointer to the location where the function has to write the
 *                   Scrambling Activation Flag State.
 *
 *  @retval   CSD_NO_ERROR
 *              The intended operation was executed successfully.
 *  @retval   CSD_ERROR_INVALID_PARAMETERS
 *              The passed parameter is invalid.
 *  @retval   CSD_ERROR_OPERATION_NOT_SUPPORTED
 *              The function is not supported by the chipset.
 *  @retval   CSD_ERROR
 *              The function terminated abnormally. The intended operation
 *              failed.
 *
 *  This function allows the client software to read the RAM Scrambling Activation Flag
 *  State. If an invalid parameter is passed to the function, the function shall return
 *  CSD_ERROR_INVALID_PARAMETERS. If the function is not supported by the chipset, 
 *  it shall return CSD_ERROR_OPERATION_NOT_SUPPORTED. In all other cases, the call 
 *  shall have no effect and the function shall return CSD_ERROR.
*/
TCsdStatus csdGetScramblingActivationFlag
(
  TCsdActivationFlag* pxScramblingActivationFlagState
);

/**
 *  @ingroup flashprot
 *
 *  @brief
 *    Enable the Flash Protection
 *
 *  @retval   CSD_NO_ERROR
 *              The intended operation was executed successfully.
 *  @retval   CSD_ERROR_OPERATION_NOT_SUPPORTED
 *              The function is not supported by the chipset.
 *  @retval   CSD_ERROR
 *              The function terminated abnormally. The intended operation
 *              failed.
 *
 *  This function allows the client software to enable the Flash Protection mechanism. Once
 *  enabled, this feature cannot be disabled (this mechanism is enforced by
 *  hardware). If the Flash Protection feature is already enabled when this function is
 *  called or if the function has successfully enabled the Flash Protection feature, the
 *  function shall return CSD_NO_ERROR. If the function is not supported by the chipset, 
 *  it shall return CSD_ERROR_OPERATION_NOT_SUPPORTED. In all other cases, the call shall have
 *  no effect and the function shall return CSD_ERROR.
*/
TCsdStatus csdEnableFlashProtection
(
  void
);

/**
 *  @ingroup flashprot
 *
 *  @brief
 *    Get the Flash Protection Activation Flag
 *
 *  @param[out]     pxFlashProtectionActivationFlagState
 *                   Pointer to the location where the function has to write the
 *                   Flash Protection Activation Flag State.
 *
 *  @retval   CSD_NO_ERROR
 *              The intended operation was executed successfully.
 *  @retval   CSD_ERROR_INVALID_PARAMETERS
 *              The passed parameter is invalid.
 *  @retval   CSD_ERROR_OPERATION_NOT_SUPPORTED
 *              The function is not supported by the chipset.
 *  @retval   CSD_ERROR
 *              The function terminated abnormally. The intended operation
 *              failed.
 *
 *  This function allows the client software to read the Flash Protection Activation Flag
 *  State. If an invalid parameter is passed to the function, the function shall return
 *  CSD_ERROR_INVALID_PARAMETERS. If the function is not supported by the chipset, 
 *  it shall return CSD_ERROR_OPERATION_NOT_SUPPORTED. In all other cases, the call 
 *  shall have no effect and the function shall return CSD_ERROR.
*/
TCsdStatus csdGetFlashProtectionActivationFlag
(
  TCsdActivationFlag* pxFlashProtectionActivationFlagState
);

/**
 *  @ingroup debug
 *
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
 *  @retval   CSD_ERROR_INVALID_PARAMETERS
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
 *  @ingroup debug
 *
 *  @brief
 *    Get the set debug interface protection level.
 *
 *  @param[out]     pxDebugInterfaceProtectionLevel
 *                   Pointer to the location where the function has to write the
 *                   Debug Interface Protection Level.
 *
 *  @retval   CSD_NO_ERROR
 *              The intended operation was executed successfully.
 *  @retval   CSD_ERROR_INVALID_PARAMETERS
 *              The passed parameter is invalid.
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
 *  @ingroup chipinfo
 *
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
 *  @ingroup chipinfo
 *
 *  @brief
 *    Get the Key ladder deactivation flag state.
 *
 *  @param[out]     pxKeyLadderDeactivationFlagState
 *                   Pointer to the location where the function has to write the
 *                   Key ladder deactivation flag state.
 *
 *  @retval   CSD_NO_ERROR
 *              The intended operation was executed successfully.
 *  @retval   CSD_ERROR_INVALID_PARAMETERS
 *              The passed parameter is invalid.
 *  @retval   CSD_ERROR
 *              The function terminated abnormally. The intended operation
 *              failed.
 *
 *  This function allows the client software to read the state of the Key ladder
 *  deactivation flag.
*/
TCsdStatus csdGetKeyLadderDeactivationFlag
(
 TCsdActivationFlag* pxKeyLadderDeactivationFlagState
);

/**
 *  @ingroup dvr
 *
 *  @brief
 *    Return the DVR Key of the chipset.
 *
 *  @param[out]     xDvrKey
 *                   16-bytes vector where the function has to write the DvrKey.
 *
 *  @retval   CSD_NO_ERROR
 *              The intended operation was executed successfully.
 *  @retval   CSD_ERROR_INVALID_PARAMETERS
 *              The passed parameter is invalid.
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
 *    -	Decrypt the KeySafe = [0xbf 0x18 0x6a 0x73 0xec 0x86 0x3f 0x25
 *                             0x9b 0xe8 0x03 0x52 0x40 0xd7 0x37 0x8c]
 *          with the secret ContentKey.
 *    .
 *
 *  The decryption of Keysafe is chipset-specific, and is the result of a
 *  computation involving the TDES-RootKey128, the Key Ladder as well as the
 *  RAM2RAM Crypto Engine. As there may be several ways of configuring the
 *  RAM2RAM Crypto Engine and its Key Ladder to compute the DvrKey,
 *  Nagravision and the chipset manufacturer will commonly agree during the NOCS
 *   certification process of a chipset on how to decrypt the KeySafe
 *  for this secure chipset. Refer to the Security Driver Implementation
 *  Proposal document for more details about this computation.
*/
TCsdStatus csdGetDvrKey
(
  TCsdUnsignedInt8    xDvrKey[16]
);


/******************************************************************************/
/*                                                                            */
/*               NOCS1.x RAM2RAM Functions                                    */
/*                                                                            */
/******************************************************************************/



/*!
 *  @ingroup ram2ram
 *
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
        TCsdBoolean        	    xRefreshIv,
        TCsdR2RKeyPathHandle*      pxR2RKeyPathHandle,
  const TCsdUnsignedInt8*          pxInputData,
        TCsdUnsignedInt8*          pxOutputData,
        TCsdSize                    xDataSize
);

/*!
 *  @ingroup ram2ram
 *
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
        TCsdBoolean        	    xRefreshIv,
        TCsdR2RKeyPathHandle*      pxR2RKeyPathHandle,
  const TCsdUnsignedInt8*          pxInputData,
        TCsdUnsignedInt8*          pxOutputData,
        TCsdSize                    xDataSize
);

/*!
 *  @ingroup ram2ram
 *
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
        TCsdBoolean                     xRefreshIv,
        TCsdR2RKeyPathHandle*          pxR2RKeyPathHandle,
  const TCsdUnsignedInt8*              pxInputData,
        TCsdUnsignedInt8*              pxOutputData,
        TCsdSize                        xDataSize
);

/*!
 *  @ingroup ram2ram
 *
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
        TCsdBoolean        		        xRefreshIv,
        TCsdR2RKeyPathHandle*          pxR2RKeyPathHandle,
  const TCsdUnsignedInt8*              pxInputData,
        TCsdUnsignedInt8*              pxOutputData,
        TCsdSize                        xDataSize
);



/******************************************************************************/
/*                                                                            */
/*                                    DSC                                     */
/*                                                                            */
/******************************************************************************/

/**
 *  @ingroup dsc
 *
 *  @brief
 *    Set the Host key for the Transport Stream Crypto Engine.
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
 *      - 0 if the CW is not available.
 *      - 8 if the cryptographic algorithm is requesting 8 bytes.
 *      - 16 if the cryptographic algorithm requesting 16 bytes.
 *      - In all other cases, the function shall return CSD_ERROR_INVALID_PARAMETERS.
 *      .
 *  If a valid EMI is selected by the client software but the chipset does 
 *  not support it, the function shall return CSD_ERROR_OPERATION_NOT_SUPPORTED.
 *  If the client software selects a wrong key path handle, the function shall
 *  return CSD_ERROR_INVALID_KEY_PATH_HANDLE.
 *
 *  @note
 *    EMI supported by this function are the "MPEG TS Stream EMIs" defined in [EMI]
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
 *  @ingroup dsc
 *
 *  @brief
 *    Set the content key for the Transport Stream Crypto Engine.
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
 *  CSD_DSC_NB_OF_PROTECTING_KEYS, fixed to 2 for NOCS3.
 *  If one of the ciphered CWs also called Ciphered ContentKey (odd or even) is
 *  not available, the given pointer is set to NULL and its corresponding length
 *   to 0. The driver should not change the previous CW in this case.
 *  The size of these keys shall be:
 *      - 0 if the CW is not available.
 *      - 8 if the cryptographic algorithm is requesting 8 bytes.
 *      - 16 if the cryptographic algorithm  is requesting 16 bytes.
 *      - In all other cases, the function shall return CSD_ERROR_INVALID_PARAMETERS.
 *      .
 *  If a valid EMI is selected by the client software but the chipset does 
 *  not support it, the function shall return CSD_ERROR_OPERATION_NOT_SUPPORTED.
 *  If the client software selects a wrong key path handle, the function shall
 *  return CSD_ERROR_INVALID_KEY_PATH_HANDLE.
 *
 *  @note
 *    EMIs supported by this function are the "MPEG TS Stream EMIs" defined 
 *    in [EMI]
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


/******************************************************************************/
/*                                                                            */
/*                                    SCR                                     */
/*                                                                            */
/******************************************************************************/

/**
 *  @ingroup scr
 *
 *  @brief
 *    Set the Host encryption key for the Stream Crypto Engine
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
 *                   Initialization Vector or the first counter block.
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
 *  If the xEmi is set to one of the chaining mode, pxInitializationVector
 *  is used to pass the Initialization Vector to the Stream Crypto Engine. The
 *  size of this vector depends on the cryptographic algorithm (exemple: 16 bytes
 *  for AES and 8 bytes for TDES). In all other cases, the function shall return
 *  CSD_ERROR_INVALID_PARAMETERS.
 *  If a valid EMI is selected by the client software but the chipset does 
 *  not support it, the function shall return CSD_ERROR_OPERATION_NOT_SUPPORTED.
 *  If the client software selects a wrong key path handle, the function shall
 *  return CSD_ERROR_INVALID_KEY_PATH_HANDLE.
 *
 *  @note
 *    EMIs supported by this function are defined in [EMI] and are:
 *    - "MPEG TS Stream AES & TDES EMIs"  
 *    - "Other Standards Stream EMIs"
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
 *  @ingroup scr
 *
 *  @brief
 *    Set the Host decryption key for the Stream Crypto Engine
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
 *                   Initialization Vector or the first counter block.
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
 *  If the xEmi is set to one of the chaining mode, pxInitializationVector
 *  is used to pass the Initialization Vector to the Stream Crypto Engine. The
 *  size of this vector depends on the cryptographic algorithm (exemple: 16 bytes 
 *  for AES and 8 bytes for TDES). In all other cases, the function shall return
 *  CSD_ERROR_INVALID_PARAMETERS.
 *  If a valid EMI is selected by the client software but the chipset does 
 *  not support it, the function shall return CSD_ERROR_OPERATION_NOT_SUPPORTED.
 *  If the client software selects a wrong key path handle, the function shall
 *  return CSD_ERROR_INVALID_KEY_PATH_HANDLE.
 *
 *  @note
 *    EMIs supported by this function are defined in [EMI] and are:
 *    - "MPEG TS Stream AES & TDES EMIs"  
 *    - "Other Standards Stream EMIs"
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
 *  @ingroup scr
 *
 *  @brief
 *    Set the Content key for the Stream Crypto Engine in Decryption.
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
 *  @param[in]      pxInitializationVector or the first counter block.
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
 *  number of those keys contained in this array is defined by the constant
 *  CSD_SCR_NB_OF_PROTECTING_KEYS, fixed to 2 in NOCS3.
 *  The size of these keys shall be:
 *      - 0 if the CW is not available.
 *      - 8 if the cryptographic algorithm is requesting 8 bytes.
 *      - 16 if the cryptographic algorithm  is requesting 16 bytes.
 *      - In all other cases, the function shall return CSD_ERROR_INVALID_PARAMETERS.
 *      .
 *  If the xEmi is set to one of the chaining mode, pxInitializationVector
 *  is used to pass the Initialization Vector to the Stream Crypto Engine. The
 *  size of this vector depends on the cryptographic algorithm (exemple: 16 bytes 
 *  for AES and 8 bytes for TDES). In all other cases, the function shall return
 *  CSD_ERROR_INVALID_PARAMETERS.
 *  If a valid EMI is selected by the client software and the chipset does 
 *  not support it, the function shall return CSD_ERROR_OPERATION_NOT_SUPPORTED.
 *  If the client software selects a wrong key path handle, the function shall
 *  return CSD_ERROR_INVALID_KEY_PATH_HANDLE.
 *
 *  @note
 *    EMIs supported by this function are defined in [EMI] and are:
 *    - "MPEG TS Stream AES & TDES EMIs"  
 *    - "Other Standards Stream EMIs"
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
 *  @ingroup scr
 *
 *  @brief
 *    Set the Content key for the Stream Crypto Engine in Encryption.
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
*                    InitializationVector or the first counter block.
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
 *  number of those keys contained in this array is defined by the constant
 *  CSD_SCR_NB_OF_PROTECTING_KEYS, fixed to 2 in NOCS3.
 *  The size of these keys shall be:
 *      - 0 if the CW is not available.
 *      - 8 if the cryptographic algorithm is requesting 8 bytes.
 *      - 16 if the cryptographic algorithm  is requesting 16 bytes.
 *      - In all other cases, the function shall return CSD_ERROR_INVALID_PARAMETERS.
 *      .      
 *  If the xEmi is set to one of the chaining mode, pxInitializationVector
 *  is used to pass the Initialization Vector to the Stream Crypto Engine. The
 *  size of this vector depends on the cryptographic algorithm (exemple: 16 bytes 
 *  for AES and 8 bytes for TDES). In all other cases, the function shall return
 *  CSD_ERROR_INVALID_PARAMETERS.
 *  If a valid EMI is selected by the client software but the chipset does 
 *  not support it, the function shall return CSD_ERROR_OPERATION_NOT_SUPPORTED.
 *  If the client software selects a wrong key path handle, the function shall
 *  return CSD_ERROR_INVALID_KEY_PATH_HANDLE.
 *
 *  @note
 *    EMIs supported by this function are defined in [EMI] and are:
 *    - "MPEG TS Stream AES & TDES EMIs"  
 *    - "Other Standards Stream EMIs"
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

/* nocs_csd.h */
