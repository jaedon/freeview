/**
**  @file ca_cect.h
**
**  @brief
**   CERT Commands Translator (CECT)
**
**   This file specifies the CERT Commands Translator API that allows Nagra CA
**   software to send CERT transactions to the CERT block.
**
**  $Id: //0183-CAK-CAS-SUPPORT-L1/components/cect/MAIN/src/com/ca_cect.h#7 $
**  $Change: 22818 $
**
** Copyright:
**   2011-2015 Nagravision S.A.
**
*/
/*
** REMARK:
**    Comments in this file use special tags to allow automatic API
**    documentation generation in HTML format, using the
**    GNU-General Public Licensed Doxygen tool.
**    For more information about Doxygen, please check www.doxygen.org
*/



#ifndef CA_CECT_H
#define CA_CECT_H

/**
 * Defines the version number of the CECT API to be implemented. This version
 * has to be included in the function table returned by cectGetFunctionTable().
 * To do so, use the macro CECTAPI_VERSION_INT to put it in the right format.
*/
#define CECTAPI_VERSION_MAJOR  3
#define CECTAPI_VERSION_MEDIUM 4
#define CECTAPI_VERSION_MINOR  1

/** @page p_history Changes history
 *
 *  - <b> 3.4.1 - 02-Apr-2015 </b>
 *    - Fix alignment issue on global variable space getter.
 *
 *  - <b> 3.4.0 - 01-Apr-2015 </b>
 *    - cectGetFunctionTable now returns a constant pointer to cater for
 *      plaforms with no static storage.
 *    - Introduce new header file ca_cectg.h to cater for platforms with no
 *      static storage.
 *
 *  - <b> 3.3.0 - 01-Jul-2014 </b>
 *    - Updated cectProgramSeg to take a set of descriptors in input instead of
 *      a SEG structure. Watch out, this change is not backward compatible.
 *    - Updated defaut BAE ECM countermeasure field value.
 *
 *  - <b> 3.2.0 - 27-Mar-2014 </b>
 *    - Added cectEncryptIdeaNxtHammerHead
 *    - Added xVariant parameter on cectDecryptScwalDataHardware and cectDecryptScwalDataHammerHead
 *
 *  - <b> 3.1.0 - 17-Feb-2014 </b>
 *    - Added cectDecryptScwalDataHardware
 *
 *  - <b> 3.0.1 - 29-Jan-2014 </b>
 *    - Minor modifications following review
 *
 *  - <b> 3.0.0 - 12-Dec-2013 </b>
 *    - Added support of BAE, IDEANxT, SCWAL-DATA, LocalAlgo, CusHash features
 *    - Added support OTAKeys and SEG records programming
 *    - Updated activation to avoid providing the NUID
 *    - Added CI status in system info
 *
 *  - <b> 2.0.1 - 27-Jun-2012 </b>
 *    - Miscellanuous updates following review
 *
 *  - <b> 2.0.0 - 12-Jun-2012 </b>
 *    - Major rework
*/
/******************************************************************************/
/*                                                                            */
/*                           GENERAL INCLUDE FILES                            */
/*                                                                            */
/******************************************************************************/

#include "ca_defs.h"
#include <stddef.h>


/******************************************************************************/
/*                                                                            */
/*                               DOXYGEN PAGES                                */
/*                                                                            */
/******************************************************************************/

/** @mainpage Introduction
 *  - @subpage p_history
 *  - @subpage p_ref
 *  - @subpage p_key_ladder
 *  - @subpage p_cipher
 *  - @subpage p_hash
 *  - @subpage p_perso
 */

 
/** @page p_ref References
 *    [NOCS3DS] Nagravision, NOCS3 IP Datasheet, Version 1.0.4\n
 *    [NOCS3HW] Nagravision, NOCS 3.0 IP Hardware Architecture, Version 1.1.2
 *    [NOCSAD] Nagravision, NOCS Architecture Document, Version 1.0.x\n
*/


/**  @page p_key_ladder Key Ladders
 *    The main purpose of a key ladder is to protect a key used to encrypt or 
 *    decrypt a content. The output of the key ladder may feed an MPEG transport
 *    stream descrambler or a RAM2RAM cipher block.
 *
 *    In order to use a key ladder, the caller has to create a logical instance,
 *    TCectCreateKeyLadder(). Then, it can process keys - TCectProcessRootLevelProtKey(), 
 *    TCectProcess1LevelProtKey() or TCectProcess2LevelProtKey() - and finally 
 *    dispose of the key ladder instance, TCectDisposeKeyLadder(). 
 *
 *    The BAE enforcement also makes use of a key ladder. Once created, the key
 *    ladder can process BAE keys with TCectProcessBaeKeys() and the processing
 *    finalized with TCectFinalizeBaeProcessing().
 *    
 *  @warning
 *    It is important to keep the sequence as short as possible, because the CERT
 *    resource remains locked as long as the key ladder instance is not deleted.
 *  @warning
 *    Moreover, in order to avoid any deadlock, the caller must avoid having
 *    several active key ladder instances created from the same thread.
*/

/**  @page p_cipher Block Cipher
 *    The CERT block supports several block cipher that are accessible through
 *    the following functions:
 *    - TCectEncryptLocalAlgoHardware()
 *    - TCectDecryptLocalAlgoHardware()
 *    - TCectDecryptIdeaNxtHammerHead()
 *    - TCectDecryptIdeaNxtSoftware()
 *    - TCectDecryptScwalDataHammerHead()
*/

/**  @page p_hash Hash
 *    The CERT block is able to process a custom hash accessible through
 *    the following function:
 *    - TCectProcessCusHash()
*/

/**  @page p_perso Personalization
 *    The CERT block can be segmented and personalized through the following functions:
 *    - TCectProgramSeg()
 *    - TCectProgramOtaKeys()
*/

/** @defgroup fct_table Function Table
 *  @defgroup config System Configuration
 *  @defgroup act Activation
 *  @defgroup kl Key Ladders
 *  @defgroup bae BAE
 *  @defgroup la Local Algo
 *  @defgroup idea IDEANxT
 *  @defgroup scwal SCWAL-DATA
 *  @defgroup cushash Custom Hash
 *  @defgroup perso Personalization
*/


/******************************************************************************/
/*                                                                            */
/*                              VERSION TOOL                                  */
/*                                                                            */
/******************************************************************************/


#ifndef CECT_TOOL_VERSION
#define CECT_TOOL_STRINGIFY(s) CECT_TOOL_TOSTRING(s)
#define CECT_TOOL_TOSTRING(s)  #s

#define CECT_TOOL_VERSION_INT(a, b, c) (a<<16 | b<<8 | c)
#define CECT_TOOL_VERSION_DOT(a, b, c) a ##.## b ##.## c
#define CECT_TOOL_VERSION(a, b, c) CECT_TOOL_VERSION_DOT(a, b, c)
#endif

#define CECTAPI_VERSION_INTER CECT_TOOL_VERSION(CECTAPI_VERSION_MAJOR, \
              CECTAPI_VERSION_MEDIUM, \
              CECTAPI_VERSION_MINOR)

#define CECTAPI_VERSION_INT   CECT_TOOL_VERSION_INT(CECTAPI_VERSION_MAJOR, \
            CECTAPI_VERSION_MEDIUM, \
            CECTAPI_VERSION_MINOR)

#define CECTAPI_VERSION_STRING  "CECTAPI_" CECT_TOOL_STRINGIFY(CECTAPI_VERSION_INTER)


/******************************************************************************/
/*                                                                            */
/*                              TYPES DEFINITIONS                             */
/*                                                                            */
/******************************************************************************/


/**
 *  @brief
 *    Bitmap indicating which system information field is present
 *
 *  @see CECT_MASK_PCUID, CECT_MASK_OCUID, CECT_MASK_SEGUID, CECT_MASK_SOCUID,
 *    CECT_MASK_BOOT_CODE_VERSION, CECT_MASK_MSID, CECT_MASK_IRD_SN,
 *    CECT_MASK_SOC_CONFIG
 */
typedef TUnsignedInt8 TCectSystemFieldBitmap;
#define CECT_MASK_PCUID                  BIT0
/**< Mask to be applied on TCectSystem.fieldBitmap to know whether
 *   TCectSystem.pcuid contains a significant value.
 */
#define CECT_MASK_OCUID                  BIT1
/**< Mask to be applied on TCectSystem.fieldBitmap to know whether
 *   TCectSystem.ocuid contains a significant value.
 */
#define CECT_MASK_SEGUID                 BIT2
/**< Mask to be applied on TCectSystem.fieldBitmap to know whether
 *   TCectSystem.segUid contains a significant value.
 */
#define CECT_MASK_SOCUID                 BIT3
/**< Mask to be applied on TCectSystem.fieldBitmap to know whether
 *   TCectSystem.socUid contains a significant value.
 */
#define CECT_MASK_BOOT_CODE_VERSION      BIT4
/**< Mask to be applied on TCectSystem.fieldBitmap to know whether
 *   TCectSystem.bootCodeVersion contains a significant value.
 */
#define CECT_MASK_MSID                   BIT5
/**< Mask to be applied on TCectSystem.fieldBitmap to know whether
 *   TCectSystem.msid contains a significant value.
 */
#define CECT_MASK_IRD_SN                 BIT6
/**< Mask to be applied on TCectSystem.fieldBitmap to know whether
 *   TCectSystem.irdSerialNumber contains a significant value.
 */
#define CECT_MASK_SOC_CONFIG             BIT7
/**< Mask to be applied on TCectSystem.fieldBitmap to know whether
 *   TCectSystem.socConfig contains a significant value.
 */

/**
 *  @brief
 *    Status code returned by functions of the CECT API
*/
typedef enum
{
  CECT_NO_ERROR,
   /**< Requested operation was completed successfully
    */
  CECT_ERROR_BAD_PARAMETER,
   /**< Invalid or unknown parameter
    */
  CECT_ERROR,
   /**< Requested operation failed
    */
  CECT_ERROR_OTP_LOCKED,
   /**< CERT OTP record is locked
    */
  CECT_ERROR_KEY_PENDING,
   /**< A key is already pending on the CERT key interface. The CERT is no longer
    *   able to output another as long as the previous has not been ackknowledged
    *   with a secUseCertKey call or bsdUseCertKey call.
    */
  LAST_CECT_STATUS
   /**< Internal use only.
    */
} TCectStatus;


#define CECT_CUID_SIZE               6
/**< CERT unique ID size
 */
#define CECT_SEGUID_SIZE             2
/**< CERT segment unique ID size
 */
#define CECT_SEGNBR_SIZE             4
/**< CERT segment number size
 */
#define CECT_SOCUID_SIZE             8
/**< SoC unique ID size
 */
#define CECT_BOOT_CODE_VERSION_SIZE  4
/**< Boot code version size
 */
#define CECT_MSID_SIZE               4
/**< CERT market segment ID size
 */
#define CECT_IRDSN_SIZE              4
/**< IRD serial number size
 */
#define CECT_ACT_BITMAP_SIZE         9
/**< CERT activation bitmap size
 */
#define CECT_NETLIST_VERSION_SIZE    4
/**< CERT netlist version size
 */
#define CECT_CI_STATUS_SIZE          4
/**< CERT Common Interface status register size
 */
#define CECT_KEY_COMP_SIZE           2
/**< Key component size
 */
#define CECT_INTEGRA_TYPE_SIZE       CECT_KEY_COMP_SIZE
/**< Integra type size
 */
#define CECT_INTEGRA_VALUE_SIZE      8
/**< Integra value size
 */
#define CECT_OTA_KEY_SIZE            16
/**< CERT OTA key size
 */
#define CECT_CIPHERED_HH_KEY_SIZE    32
/**< Ciphered hammer-head key size, including IV and TAG
 */
#define CECT_IDEANXT_SW_KEY_SIZE     16
/**< IDEANxT software key size
 */
#define CECT_PPID_SIZE               2
/**< PPID size
 */
#define CECT_DATE_TIME_SIZE          4
/**< Date and time size
 */
#define CECT_ADF_SIZE                64
/**< ADF size
 */
#define CECT_GA_SIZE                 4
/**< GA size
 */
#define CECT_CAID_SIZE               4
/**< CAID size
 */
#define CECT_GUA_SIZE                4
/**< GUA size
 */
#define CECT_BAE_REVOCATION_SIZE     8
/**< BAE revocation size
 */
#define CECT_BAE_COUNTERMEASURE_SIZE 12
/**< BAE countermeasure size
 */
#define CECT_AUTHENT_TAG_SIZE        8
/**< Authentication tag size
 */
#define CECT_IV_SIZE                 8
/**< Initialization vectore size
 */
#define CECT_HASH_SIZE               16
/**< CERT custom hash size
 */
 


/**
 *  @ingroup config
 *
 *  @brief
 *    Structure used to accommodate all CERT system information parameters
 */
typedef struct
{
  TUnsignedInt8 pcuid[CECT_CUID_SIZE];
   /**< CUID programmed in OTP personalization slot
    */
  TUnsignedInt8 ocuid[CECT_CUID_SIZE];
   /**< CUID programmed in OTP over-the-air slot
    */
  TUnsignedInt8 segUid[CECT_SEGUID_SIZE];
   /**< Segment unique identifier
    */
  TUnsignedInt8 socUid[CECT_SOCUID_SIZE];
   /**< Unique identifier of the SoC. This ID is defined and owned by the SoC
    *   manufacturer.
    */
  TUnsignedInt8 bootCodeVersion[CECT_BOOT_CODE_VERSION_SIZE];
   /**< This 4-byte parameter is used for the versioning of the STB boot code.
    */
  TUnsignedInt8 msid[CECT_MSID_SIZE];
   /**< Market segment identifier as defined in NOCS specification
    */
  TUnsignedInt8 irdSerialNumber[CECT_IRDSN_SIZE];
   /**< Serial number of the set-top box (aka STB CA S/N) stored in the SoC.
    */
  TUnsignedInt8 socConfig;
   /**< Bitmap providing the "security state" of the SoC
    */
  TCectSystemFieldBitmap fieldBitmap;
   /**< Bitmap signaling presence/absence of system information fields above
    *   (pcuid to socConfig). A bit set to '1' means the corresponding field
    *   is present.
    */
  TUnsignedInt8 actBitmap[CECT_ACT_BITMAP_SIZE];
   /**< Activation bitmap defining which CERT commands are currently active.
    */
  TUnsignedInt8 actVersion;
   /**< Version of the activation bitmap. Mainly used to revoke activation of
    *   some mode of operations or commands.
    */
  TUnsignedInt8 netlistVersion[CECT_NETLIST_VERSION_SIZE];
   /**< Version of the netlist
    */
  TUnsignedInt8 ciStatus[CECT_CI_STATUS_SIZE];
   /**< This 4-byte parameter corresponds to the Common Interface status
    *   register.
    */
} TCectSystem;


/**
 *  @ingroup kl
 *
 *  @brief
 *    Pointer to an opaque structure to be defined by the entity in charge of
 *    developing the CECT component and used as a handle on a CERT key ladder.
 */
typedef struct SCectKlHandle* TCectKlHandle;

/**
 *  @brief
 *    Type used to specify the parity of the content key (control word).
*/
typedef enum
{
  CECT_PARITY_EVEN,
  CECT_PARITY_ODD
} TCectParity;


/** @addtogroup bae
 * @{
*/

/**
 *  @brief
 *    Type used to specify a CAID range
*/
typedef struct
{
  TUnsignedInt8 min[CECT_CAID_SIZE];
  /**< CAID range lower bound
  */
  TUnsignedInt8 max[CECT_CAID_SIZE];
  /**< CAID range upper bound
  */
} TCectCaidRange;


/**
 *  @brief
 *    BAE ENT record
*/
typedef struct
{
  TUnsignedInt8   integraType[CECT_INTEGRA_TYPE_SIZE];
  /**< Entitlement integra type. Used as a key component by the CERT key
   *   generator.
  */
  TUnsignedInt8   ppid[CECT_PPID_SIZE];
  /**< Entitlement's PPID
   */
  TUnsignedInt8   ruCaid[CECT_CAID_SIZE];
  /**< Entitlement's ruCAID
   */
  TUnsignedInt8   level;
  /**< Entitlement's level
   */
  size_t          caidRangesNum;
  /**< Number of CAID ranges
   */
  TCectCaidRange* pCaidRanges;
  /**< Buffer allocated by the caller containing caidRangesNum CAID ranges
   */
  size_t          caidRangeIndex;
  /**< Index, starting from 0, of the CAID range that matches the ECM CAID
   *   range.
   */
  TUnsignedInt8   integraValue[CECT_INTEGRA_VALUE_SIZE];
  /**< Value of the entitlement's integra
   */
} TCectBaeEnt;


/**
 *  @brief
 *    BAE OPE record
*/
typedef struct
{
  TUnsignedInt8   integraType[CECT_INTEGRA_TYPE_SIZE];
  /**< OPE integra type. Used as a key component by the CERT key
   *   generator.
   */
  TUnsignedInt8   gua[CECT_GUA_SIZE];
   /**< MOP GUA
    */
  size_t          caidRangesNum;
  /**< Number of CAID ranges
   */
  TCectCaidRange* pCaidRanges;
  /**< Buffer allocated by the caller containing caidRangesNum CAID ranges
   */
  size_t          caidRangeIndex;
  /**< Index, starting from 0, of the CAID range that matches the entitlement
   *   ruCAID.
   */
  TUnsignedInt8   integraValue[CECT_INTEGRA_VALUE_SIZE];
  /**< Value of the OPE integra
   */
} TCectBaeOpe;


/**
 *  @brief
 *    BAE PAO record
*/
typedef struct
{
  TUnsignedInt8   integraType[CECT_INTEGRA_TYPE_SIZE];
  /**< PAO integra type. Used as a key component by the CERT key
   *   generator.
   */
  TUnsignedInt8   rDateTime[CECT_DATE_TIME_SIZE];
  /**< MOP RDateTime
   */
  TUnsignedInt8   ga[CECT_GA_SIZE];
  /**< MOP GA
   */
  TUnsignedInt8   adf[CECT_ADF_SIZE];
  /**< MOP ADF
   */
  TUnsignedInt8   integraValue[CECT_INTEGRA_VALUE_SIZE];
  /**< Value of the PAO integra
   */
} TCectBaePao;


/**
 *  @brief
 *    BAE ECM record
*/
typedef struct
{
  TUnsignedInt8   revocation[CECT_BAE_REVOCATION_SIZE];
  /**< Revocation. Default value is 0x00000000'00000000.
   */
  TUnsignedInt8   integraType[CECT_INTEGRA_TYPE_SIZE];
  /**< ECM integra type. Used as a key component by the CERT key
   *   generator.
   */
  TUnsignedInt8   timestamp[CECT_DATE_TIME_SIZE];
  /**< ECM timestamp
   */
  TUnsignedInt8   level;
  /**< ECM level
   */
  TUnsignedInt8   version;
  /**< ECM version
   */
  TUnsignedInt8   counterMeasure[CECT_BAE_COUNTERMEASURE_SIZE];
  /**< Countermeasure. Default value is 0x00000000'00000000'00000000
   */
  size_t          cipheredContentKeySize;
  /**< Size, in bytes, of the ciphered content keys (aka control words). It
   *   depends on the scrambling algorithm:
   *    - AES: 16 bytes                        
   *    - DVB-CSA2: 8 bytes                    
   *    - DVB-CSA3: 16 bytes                   
   *    - ASA64: 8 bytes                       
   *    - ASA128: 16 bytes                     
   */
  TUnsignedInt8*  pCipheredEvenContentKey;
  /**< Buffer, allocated by the caller, containing the ciphered even content key.  
   */
  TUnsignedInt8*  pCipheredOddContentKey;
  /**< Buffer, allocated by the caller, containing the ciphered odd content key.  
   */
  size_t          caidRangesNum;
  /**< Number of CAID ranges
   */
  TCectCaidRange* pCaidRanges;
  /**< Buffer allocated by the caller containing caidRangesNum CAID ranges
   */
  size_t          caidRangeIndex;
  /**< Index, starting from 0, of the CAID range that matches the entitlement
   *   CAID range.
   */
  TUnsignedInt8   integraValue[CECT_INTEGRA_VALUE_SIZE];
  /**< Value of the ECM integra
   */
} TCectBaeEcm;


/**
 *  @brief
 *    BAE keys
*/
typedef struct
{
  TUnsignedInt8   gHhKeyComp[CECT_KEY_COMP_SIZE];
  /**< Key component of GHHCWKey
   */
  TUnsignedInt8   cipheredGHhCwKey[CECT_CIPHERED_HH_KEY_SIZE];
  /**< 32-byte buffer, allocated by the caller, containing the ciphered GHHCWKey.
   *   This buffer also contain the IV and the authentication tag: 
   *   cipheredGHhCwKey = IV(8) || [GHHCWKey](16) || TAG(8)
   */
  TUnsignedInt8   uHhKeyComp[CECT_KEY_COMP_SIZE];
  /**< Key component of UHHCWKey
   */
  TUnsignedInt8   cipheredUHhCwKey[CECT_CIPHERED_HH_KEY_SIZE];
  /**< 32-byte buffer, allocated by the caller, containing the ciphered UHHCWKey.
   *   This buffer also contain the IV and the authentication tag: 
   *   cipheredUHhCwKey = IV(8) || [UHHCWKey](16) || TAG(8)
   */
} TCectBaeKeys;

/** @} bae */

/** @addtogroup perso
 * @{
*/

/**
 *  @brief
 *    OTA keys record
*/
typedef struct
{
  TUnsignedInt8   integraType[CECT_INTEGRA_TYPE_SIZE];
  /**< OTA keys integra type used as key component by the CERT key generator. 
   */
  TUnsignedInt8   ocuid[CECT_CUID_SIZE];
  /**< OTA CUID (aka NUID OTA)
   */
  TUnsignedInt8   ukOta[CECT_OTA_KEY_SIZE];
  /**< UKota key programmed (or to be program) in the CERT OTA key slot. It is
   *   unique and identified by the OCUID.
   */
  TUnsignedInt8   gkOta[CECT_OTA_KEY_SIZE];
  /**< GKota key programmed (or to be program) in the CERT OTA key slot. It is
   *   global and associated to the OCUID.
   */
  TUnsignedInt8   integraValue[CECT_INTEGRA_VALUE_SIZE];
  /**< Value of the OTA keys integra
   */
} TCectOtaKeys;


/**
 *  @brief
 *    SEG record
 *
 *  @deprecated It is replaced by a TLV descriptor
*/
typedef struct
{
  TUnsignedInt8   integraType[CECT_INTEGRA_TYPE_SIZE];
  /**< OTA keys integra type used as key component by the CERT key generator. 
   */
  TUnsignedInt8   segUid[CECT_SEGUID_SIZE];
  /**< SEGUID programmed (or to be program) in the CERT SEG record. This identifier
   *   is public and identifies the segNbr field.
   */
  TUnsignedInt8   segNbr[CECT_SEGNBR_SIZE];
  /**< Segmentation number programmed (or to be program) in the CERT SEG record.
   *   It is identified by the SEGUID.
   */
  TUnsignedInt8   integraValue[CECT_INTEGRA_VALUE_SIZE];
  /**< Value of the SEG integra
   */
} TCectSeg;

/** @} perso */


/******************************************************************************/
/*                                                                            */
/*                           FUNCTION TABLE DEFINITION                        */
/*                                                                            */
/******************************************************************************/

/**
 *  @ingroup config
 *
 *  @brief
 *    This function is used to read system information (public data) of the CERT
 *    block.
 *
 *  @param[out] pxSystem
 *    Pointer to system object allocated by the caller and to be filled in with
 *    CERT system information.
 *
 *  @retval CECT_NO_ERROR
 *    Operation completed successfully
 *
 *  @retval CECT_ERROR
 *    Operation failed
*/
typedef TCectStatus (*TCectSystemGetInfo)
(
  TCectSystem*  pxSystem
);


/**
 *  @ingroup act
 *
 *  @brief
 *    This function activates the CERT block. It requires an unique and/or
 *    a global activation descriptor complying with the following format
 *    (see CAS_IrdPersoData):
 *
 *      tag            (1) = A0h
 *      length         (1) = 14h
 *      key_comp       (2)
 *      version        (1)
 *      act_bitmap     (9)
 *      integra        (8)
 *
 *    Unique and global descriptors are identified thanks to the key_comp field. 
 *                     
 *  @param[in] xNuidSize
 *    This parameter is no longer required and can be set to 0. 
 *    However, it is kept for backward compatiblity. In such a case it indicates
 *    the size in bytes of the NUID: 4 bytes (standard NUID) or to 6 bytes
 *    (extended NUID).
 *
 *  @param[in] pxNuid
 *    This parameter is no longer required and can be set to NULL. 
 *    However, it is kept for backward compatibility. In such a case it corresponds 
 *    to the buffer containing the xNuidSize bytes of the NUID in MSBF format.
 *
 *  @param[in] xDescriptorsSize
 *    Size in bytes of pxDescriptors buffer.
 *
 *  @param[in] pxDescriptors
 *    Buffer, allocated by the caller, containing a collection of TLV descriptors
 *    including the activation descriptor.
 *
 *  @retval CECT_NO_ERROR
 *    Operation completed successfully
 *
 *  @retval CECT_ERROR
 *    Operation failed
*/
typedef TCectStatus (*TCectActivate)
(
        size_t              xNuidSize,
  const TUnsignedInt8*     pxNuid,
        size_t              xDescriptorsSize,
  const TUnsignedInt8*     pxDescriptors
);


/**
 *  @ingroup kl
 *
 *  @brief
 *    This function creates a key ladder instance
 *
 *  @pre
 *    This functions requires the CERT block to be activated (TCectActivate).
 *
 *  @param[out] pxHandle
 *    Handle assigned to the key ladder instance
 *
 *  @retval CECT_NO_ERROR
 *    Operation completed successfully
 *
 *  @retval CECT_ERROR
 *    - Operation failed
 *
 *  @warning
 *    In order to avoid a deadlock, it is not authorized to have several key 
 *    ladder instances created from the \b same thread. A key ladder instance 
 *    has to be diposed of as soon as possible. 
*/
typedef TCectStatus (*TCectCreateKeyLadder)
(
  TCectKlHandle* pxHandle
);


/**
 *  @ingroup kl
 *
 *  @brief
 *    This function disposes of a key ladder instance
 *
 *  @param[out] xHandle
 *    Handle on key ladder instance
 *
 *  @retval CECT_NO_ERROR
 *    Operation completed successfully
 *
 *  @retval CECT_ERROR_BAD_PARAMETER
 *    Handle provided is invalid or unknown.
 *
 *  @retval CECT_ERROR
 *    Operation failed
 *
 *  @warning
 *    It is important to dispose of a key ladder instance as soon as possible in
 *    order to unlock the underlying CERT resource.
*/
typedef TCectStatus (*TCectDisposeKeyLadder)
(
  TCectKlHandle xHandle
);


/**
 *  @ingroup kl
 *
 *  @brief
 *    This function requests the CERT block to output a key from a root level
 *    key ladder.
 *
 *    The output key available on the CERT key interface is then mainly intended 
 *    to be used for RAM2RAM operations. This is achieved by calling 
 *    secUseCertKey() or bsdUseCertKey functions. Refer to the SEC and BSD API 
 *    for further details about these functions.
 *
 *  @pre
 *    - The CERT is activated (see TCectActivate)
 *    - A key ladder instance has been created (see TCectCreateKeyLadder)
 *
 *  @param[in] xKlHandle
 *    Handle on key ladder instance
 *
 *  @param[in] xKeyProtPath
 *    Key protection path (KPP) to be used for the operation. It is used by the
 *    CECT to select the right CERT key ladder. Refer to [NOCSAD] for KPP values
 *    definitions.
 *
 *  @retval CECT_NO_ERROR
 *    Operation completed successfully
 *
 *  @retval CECT_ERROR_BAD_PARAMETER
 *    A parameter is invalid
 *
 *  @retval CECT_ERROR
 *    Operation failed
 *
 *  @retval CECT_ERROR_KEY_PENDING
 *    Operation failed because a key is aleardy pending on the CERT key
 *    interface. Call secUseCertKey() or bsdUseCertKey() to solve this issue.
 *
 *  @note
 *    The content cipher algorithm signaled in the key protection path (KPP)
 *    has to match the EMI of the targeted RAM2RAM crypto-engine, i.e. the EMI
 *    parameter of secUseCertKey() or bsdUseCertKey().
 *
*/
typedef TCectStatus (*TCectProcessRootLevelProtKey)
(
        TCectKlHandle       xHandle,
        TUnsignedInt8       xKeyProtPath
);


/**
 *  @ingroup kl
 *
 *  @brief
 *    This function requests the CERT block to output a key from a 1-level
 *    key ladder.
 *
 *    The output key available on the CERT key interface may be used then by a
 *    crypto-engine for encryption/decryption operations.
 *
 *  @pre
 *    - The CERT is activated (see TCectActivate)
 *    - A key ladder instance has been created (see TCectCreateKeyLadder)
 *
 *  @param[in] xKlHandle
 *    Handle on key ladder instance
 *
 *  @param[in] xKeyProtPath
 *    Key protection path (KPP) to be used for the operation. It is used by the
 *    CECT to select the right CERT key ladder. Refer to [NOCSAD] for KPP values
 *    definitions.
 *
 *  @param[in]  xCipheredContentKeySize
 *    Size in bytes of the ciphered content key. It depends on the content cipher
 *    algorithm signaled in the KPP:
 *    - TDES keying option 2: 2*8=16 bytes
 *    - AES: 16 bytes
 *    - DVB-CSA2: 8 bytes
 *    - DVB-CSA3: 16 bytes
 *    - ASA64: 8 bytes
 *    - ASA128: 16 bytes
 *
 *  @param[in]  pxCipheredContentKey
 *    Buffer, allocated by the caller, containing the ciphered content key. It
 *    is equal to CipheredContentKey=TDES(ContentKey, L1ProtKey).
 *
 *  @param[in]  xCipheredProtKeySize
 *    Size in bytes of the intermediate level protection keys used within the
 *    key ladder. It is always equal to 16 bytes for NOCS chipset.
 *
 *  @param[in]  pxL1CipheredProtKey
 *    Buffer, allocated by the caller, containing the first ciphered protection
 *    key fed into the key ladder. It is equal to
 *    L1CipheredProtKey=TDES(L1ProtKey, RootKey).
 *
 *  @retval CECT_NO_ERROR
 *    Operation completed successfully
 *
 *  @retval CECT_ERROR_BAD_PARAMETER
 *    A parameter is invalid
 *
 *  @retval CECT_ERROR
 *    Operation failed
 *
 *  @retval CECT_ERROR_KEY_PENDING
 *    Operation failed because a key is aleardy pending on the CERT key
 *    interface. Call secUseCertKey() or bsdUseCertKey() to solve this issue.
 *
 *  @note
 *    The content cipher algorithm signaled in the key protection path (KPP)
 *    has to match the EMI of the targeted crypto-engine, i.e. the EMI
 *    parameter of secUseCertKey() or bsdUseCertKey().
*/
typedef TCectStatus (*TCectProcess1LevelProtKey)
(
        TCectKlHandle       xHandle,
        TUnsignedInt8       xKeyProtPath,
        size_t              xCipheredContentKeySize,
  const TUnsignedInt8*     pxCipheredContentKey,
        size_t              xCipheredProtKeySize,
  const TUnsignedInt8*     pxL1CipheredProtKey
);


/**
 *  @ingroup kl
 *
 *  @brief
 *    This function requests the CERT block to output a key from a 2-level
 *    key ladder.
 *
 *    The output key available on the CERT key interface may be used then by a
 *    crypto-engine for encryption/decryption operations.
 *
 *  @pre
 *    - The CERT is activated (see TCectActivate)
 *    - A key ladder instance has been created (see TCectCreateKeyLadder)
 *
 *  @param[in] xKlHandle
 *    Handle on key ladder instance
 *
 *  @param[in] xKeyProtPath
 *    Key protection path (KPP) to be used for the operation. It is used by the
 *    CECT to select the right CERT key ladder. Refer to [NOCSAD] for KPP values
 *    definitions.
 *
 *  @param[in]  xCipheredContentKeySize
 *    Size in bytes of the ciphered content key. It depends on the cipher content
 *    algorithm signaled in the KPP:
 *    - TDES keying option 2: 2*8=16 bytes
 *    - AES: 16 bytes
 *    - DVB-CSA2: 8 bytes
 *    - DVB-CSA3: 16 bytes
 *    - ASA64: 8 bytes
 *    - ASA128: 16 bytes
 *
 *  @param[in]  pxCipheredContentKey
 *    Buffer, allocated by the caller, containing the ciphered content key. It
 *    is equal to CipheredContentKey=TDES(ContentKey, L1ProtKey).
 *
 *  @param[in]  xCipheredProtKeySize
 *    Size in bytes of the intermediate level protection keys used within the
 *    key ladder. It is always equal to 16 bytes for NOCS chipset.
 *
 *  @param[in]  pxL1CipheredProtKey
 *    Buffer, allocated by the caller, containing the first ciphered protection
 *    key fed into the key ladder. It is equal to
 *    L1CipheredProtKey=TDES(L1ProtKey, L2ProtKey).
 *
 *  @param[in]  pxL2CipheredProtKey
 *    Buffer, allocated by the caller, containing the second ciphered protection
 *    key fed into the key ladder. It is equal to
 *    L2CipheredProtKey=TDES(L2ProtKey, RootKey).
 *
 *  @retval CECT_NO_ERROR
 *    Operation completed successfully
 *
 *  @retval CECT_ERROR_BAD_PARAMETER
 *    A parameter is invalid
 *
 *  @retval CECT_ERROR
 *    Operation failed
 *
 *  @retval CECT_ERROR_KEY_PENDING
 *    Operation failed because a key is aleardy pending on the CERT key
 *    interface. Call secUseCertKey() or bsdUseCertKey() to solve this issue.
 *
 *  @note
 *    The content cipher algorithm signaled in the key protection path (KPP)
 *    has to match the EMI of the targeted crypto-engine, i.e. the EMI
 *    parameter of secUseCertKey() or bsdUseCertKey().
*/
typedef TCectStatus (*TCectProcess2LevelProtKey)
(
        TCectKlHandle       xHandle,
        TUnsignedInt8       xKeyProtPath,
        size_t              xCipheredContentKeySize,
  const TUnsignedInt8*     pxCipheredContentKey,
        size_t              xCipheredProtKeySize,
  const TUnsignedInt8*     pxL1CipheredProtKey,
  const TUnsignedInt8*     pxL2CipheredProtKey
);


/**
 *  @ingroup bae
 *
 *  @brief
 *    This function requests the CERT block to output a key from a 2-level
 *    key ladder.
 *
 *    The output key available on the CERT key interface may be used then by a
 *    crypto-engine for encryption/decryption operations.
 *
 *  @pre
 *    - The CERT is activated (see TCectActivate)
 *    - A key ladder instance has been created (see TCectCreateKeyLadder)
 *
 *  @param[in] xKlHandle
 *    Handle on key ladder instance
 *
 *  @param[in] xKeyProtPath
 *    Key protection path (KPP) to be used for the operation. Refer to [NOCSAD] 
 *    for KPP values definitions.
 *
 *  @param[in]  xParity
 *    Parity of the first key to output on the CERT key interface
 *
 *  @param[in]  pxEnt
 *    Pointer to the BAE ENT record
 *
 *  @param[in]  pxOpe
 *    Pointer to the BAE OPE record
 *
 *  @param[in]  pxPao
 *    Pointer to the BAE PAO record
 *
 *  @param[in]  pxEcm
 *    Pointer to the BAE ECM record
 *
 *  @param[in]  pxKeys
 *    Pointer to the BAE keys structure
 *
 *  @retval CECT_NO_ERROR
 *    Operation completed successfully
 *
 *  @retval CECT_ERROR_BAD_PARAMETER
 *    A parameter is invalid
 *
 *  @retval CECT_ERROR
 *    Operation failed
 *
 *  @retval CECT_ERROR_KEY_PENDING
 *    Operation failed because a key is aleardy pending on the CERT key
 *    interface. Call secUseCertKey() or bsdUseCertKey() to solve this issue.
*/
typedef TCectStatus (*TCectProcessBaeKeys)
(
        TCectKlHandle  xHandle,
        TUnsignedInt8  xKeyProtPath,
        TCectParity    xParity,
  const TCectBaeEnt*  pxEnt, 
  const TCectBaeOpe*  pxOpe, 
  const TCectBaePao*  pxPao, 
  const TCectBaeEcm*  pxEcm,
  const TCectBaeKeys* pxKeys
);

/**
 *  @ingroup bae
 *
 *  @brief
 *    This function requests the CERT block to output the second key of a BAE
 *    transaction.
 *
 *    The output key available on the CERT key interface may be used then by a
 *    crypto-engine for encryption/decryption operations.
 *
 *  @pre
 *    - The CERT is activated (see TCectActivate)
 *    - A key ladder instance has been created (see TCectCreateKeyLadder)
 *    - BAE keys have been processed (see TCectProcessBaeKeys)
 *
 *  @param[in] xKlHandle
 *    Handle on key ladder instance
 *
 *  @param[in]  xParity
 *    Parity of the second key to output on the CERT key interface
 *
 *  @retval CECT_NO_ERROR
 *    Operation completed successfully
 *
 *  @retval CECT_ERROR_BAD_PARAMETER
 *    A parameter is invalid
 *
 *  @retval CECT_ERROR
 *    Operation failed
 *
 *  @retval CECT_ERROR_KEY_PENDING
 *    Operation failed because a key is aleardy pending on the CERT key
 *    interface. Call secUseCertKey() or bsdUseCertKey() to solve this issue.
*/
typedef TCectStatus (*TCectFinalizeBaeProcessing)
(
  TCectKlHandle  xHandle,
  TCectParity    xParity
);


/**
 *  @ingroup la
 *
 *  @brief
 *    This function encrypt a message with the CERT LocalAlgo algorithm in hardware
 *    mode.
 *
 *    This function generates an initialization vector and authentication tag 
 *    that must be given back to the decryption function TCectDecryptLocalAlgoHardware.
 *
 *  @pre
 *    The CERT is activated (see TCectActivate)
 *
 *  @param[in]  pxInput
 *    Buffer containing the input message to encrypt.
 *
 *  @param[out]  pxOutput
 *    Buffer where to write the encrypted message.
 *
 *  @param[in]  xMessageSize
 *    Size in bytes of messages (pxInput, pxOutput) to operate on. It must be a
 *    multiple of 8.
 *
 *  @param[in] pxKeyComp
 *    2-byte key component
 *
 *  @param[out] pxInitVector
 *    8-byte buffer containing the initialization vector (IV) computed by the 
 *    CERT block. This IV has to be given back to the decryption function
 *    (TCectDecryptLocalAlgoHardware).
 *
 *  @param[out] pxTag
 *    8-byte authentication tag computed by the CERT block. This tag has to be
 *    given back to the decryption function (TCectDecryptLocalAlgoHardware).
 *
 *  @retval CECT_NO_ERROR
 *    Operation completed successfully
 *
 *  @retval CECT_ERROR_BAD_PARAMETER
 *    - xMessageSize is not a multiple of 8
 *    - pxInput, pxOutput, pxKeyComp, pxInitVector or pxTag is NULL
 *
 *  @retval CECT_ERROR
 *    Operation failed
 *
 *  @note
 *    - All buffers must be allocated by the caller
 *    - In-place operation (pxOutput=pxIntput) is supported
*/
typedef TCectStatus (*TCectEncryptLocalAlgoHardware)
(
        TUnsignedInt8*  pxInput,
        TUnsignedInt8*  pxOutput,
        size_t           xMessageSize,
  const TUnsignedInt8*  pxKeyComp,
        TUnsignedInt8*  pxInitVector,
        TUnsignedInt8*  pxTag
);


/**
 *  @ingroup la
 *
 *  @brief
 *    This function decrypt a message with the CERT LocalAlgo algorithm in hardware
 *    mode.
 *
 *    This function requires an IV and the authentication tag generated by the
 *    encryption TCectEncryptLocalAlgoHardware.
 *
 *  @pre
 *    - The CERT is activated (see TCectActivate)
 *    - Input message is encrypted with TCectEncryptLocalAlgoHardware
 *
 *  @param[in]  pxInput
 *    Buffer containing the input message to decrypt.
 *
 *  @param[out]  pxOutput
 *    Buffer where to write the decrypted message.
 *
 *  @param[in]  xMessageSize
 *    Size in bytes of messages (pxInput, pxOutput) to operate on. It must be a
 *    multiple of 8.
 *
 *  @param[in] pxKeyComp
 *    2-byte key component
 *
 *  @param[in] pxInitVector
 *    8-byte buffer containing the initialization vector (IV) computed by the
 *    encryption function.
 *
 *  @param[in] pxTag
 *    8-byte authentication tag computed by the encryption function
 *
 *  @retval CECT_NO_ERROR
 *    Operation completed successfully
 *
 *  @retval CECT_ERROR_BAD_PARAMETER
 *    - xMessageSize is not a multiple of 8
 *    - pxInput, pxOutput, pxKeyComp, pxInitVector or pxTag is NULL
 *
 *  @retval CECT_ERROR
 *    Operation failed, including message authentication failure.
 *
 *  @note
 *    - All buffers must be allocated by the caller
 *    - In-place operation (pxOutput=pxIntput) is supported
*/
typedef TCectStatus (*TCectDecryptLocalAlgoHardware)
(
        TUnsignedInt8*  pxInput,
        TUnsignedInt8*  pxOutput,
        size_t           xMessageSize,
  const TUnsignedInt8*  pxKeyComp,
  const TUnsignedInt8*  pxInitVector,
  const TUnsignedInt8*  pxTag
);

  
/**
 *  @ingroup idea
 *
 *  @brief
 *    This function encrypt a message with the CERT IDEANxT algorithm in
 *    hammer-head (HH) mode.
 *
 *  @pre
 *    The CERT is activated (see TCectActivate)
 *
 *  @param[in]  pxInput
 *    Buffer containing the input message to encrypt.
 *
 *  @param[out]  pxOutput
 *    Buffer where to write the encrypted message.
 *
 *  @param[in]  xMessageSize
 *    Size in bytes of messages (pxInput, pxOutput) to operate on. It must be a
 *    multiple of 8.
 *
 *  @param[in] pxInitVector
 *    8-byte buffer containing the IDEANxT initialization vector (IV). This 
 *    parameter is optional. If it is NULL, message is encrypted in ECB mode. 
 *    Otherwise it is encrypted in CBC mode.
 *
 *  @param[in] pxKeyComp
 *    2-byte key component
 *
 *  @param[in] pxCipheredHhIdeaNxtKey
 *    32-byte buffer containing the ciphered HHIdeaNxtKey. This buffer also
 *    contain the hammer-head IV and authentication tag:
 *    pxCipheredHhIdeaNxtKey = IV(8) || [HHIdeaNxtKey](16) || TAG(8)
 *
 *  @retval CECT_NO_ERROR
 *    Operation completed successfully
 *
 *  @retval CECT_ERROR_BAD_PARAMETER
 *    - xMessageSize is not a multiple of 8
 *    - pxInput, pxOutput, pxKeyComp or pxCipheredHhIdeaNxtKey is NULL
 *
 *  @retval CECT_ERROR
 *    Operation failed
 *
 *  @note
 *    - All buffers must be allocated by the caller
 *    - In-place operation (pxOutput=pxIntput) is supported
*/
typedef TCectStatus (*TCectEncryptIdeaNxtHammerHead)
(
        TUnsignedInt8*  pxInput,
        TUnsignedInt8*  pxOutput,
        size_t           xMessageSize,
  const TUnsignedInt8*  pxInitVector,
  const TUnsignedInt8*  pxKeyComp,
  const TUnsignedInt8*  pxCipheredHhIdeaNxtKey
);

  
/**
 *  @ingroup idea
 *
 *  @brief
 *    This function decrypt a message with the CERT IDEANxT algorithm in
 *    hammer-head (HH) mode.
 *
 *  @pre
 *    The CERT is activated (see TCectActivate)
 *
 *  @param[in]  pxInput
 *    Buffer containing the input message to decrypt.
 *
 *  @param[out]  pxOutput
 *    Buffer where to write the decrypted message.
 *
 *  @param[in]  xMessageSize
 *    Size in bytes of messages (pxInput, pxOutput) to operate on. It must be a
 *    multiple of 8.
 *
 *  @param[in] pxInitVector
 *    8-byte buffer containing the IDEANxT initialization vector (IV). This 
 *    parameter is optional. If it is NULL, message is decrypted in ECB mode. 
 *    Otherwise it is decrypted in CBC mode.
 *
 *  @param[in] pxKeyComp
 *    2-byte key component
 *
 *  @param[in] pxCipheredHhIdeaNxtKey
 *    32-byte buffer containing the ciphered HHIdeaNxtKey. This buffer also
 *    contain the hammer-head IV and authentication tag:
 *    pxCipheredHhIdeaNxtKey = IV(8) || [HHIdeaNxtKey](16) || TAG(8)
 *
 *  @retval CECT_NO_ERROR
 *    Operation completed successfully
 *
 *  @retval CECT_ERROR_BAD_PARAMETER
 *    - xMessageSize is not a multiple of 8
 *    - pxInput, pxOutput, pxKeyComp or pxCipheredHhIdeaNxtKey is NULL
 *
 *  @retval CECT_ERROR
 *    Operation failed
 *
 *  @note
 *    - All buffers must be allocated by the caller
 *    - In-place operation (pxOutput=pxIntput) is supported
*/
typedef TCectStatus (*TCectDecryptIdeaNxtHammerHead)
(
        TUnsignedInt8*  pxInput,
        TUnsignedInt8*  pxOutput,
        size_t           xMessageSize,
  const TUnsignedInt8*  pxInitVector,
  const TUnsignedInt8*  pxKeyComp,
  const TUnsignedInt8*  pxCipheredHhIdeaNxtKey
);


/**
 *  @ingroup idea
 *
 *  @brief
 *    This function decrypt a message with the CERT IDEANxT algorithm in
 *    software mode.
 *
 *    This function requires an IV and the authentication tag generated by the
 *    encryption TCectEncryptLocalAlgoHardware.
 *
 *  @pre
 *    The CERT is activated (see TCectActivate)
 *
 *  @param[in]  pxInput
 *    Buffer containing the input message to decrypt.
 *
 *  @param[out]  pxOutput
 *    Buffer where to write the decrypted message.
 *
 *  @param[in]  xMessageSize
 *    Size in bytes of messages (pxInput, pxOutput) to operate on. It must be a
 *    multiple of 8.
 *
 *  @param[in] pxInitVector
 *    8-byte buffer containing the IDEANxT initialization vector (IV). This 
 *    parameter is optional. If it is NULL, message is decrypted in ECB mode. 
 *    Otherwise it is decrypted in CBC mode.
 *
 *  @param[in] pxSwIdeaNxtKey
 *    16-byte buffer containing the IDEANxT software key.
 *
 *  @retval CECT_NO_ERROR
 *    Operation completed successfully
 *
 *  @retval CECT_ERROR_BAD_PARAMETER
 *    - xMessageSize is not a multiple of 8
 *    - pxInput, pxOutput or pxSwIdeaNxtKey is NULL
 *
 *  @retval CECT_ERROR
 *    Operation failed
 *
 *  @note
 *    - All buffers must be allocated by the caller
 *    - In-place operation (pxOutput=pxIntput) is supported
*/
typedef TCectStatus (*TCectDecryptIdeaNxtSoftware)
(
        TUnsignedInt8*  pxInput,
        TUnsignedInt8*  pxOutput,
        size_t           xMessageSize,
  const TUnsignedInt8*  pxInitVector,
  const TUnsignedInt8*  pxSwIdeaNxtKey
);


/**
 *  @ingroup scwal
 *
 *  @brief
 *    This function decrypt a message with the CERT SCWAL-DATA algorithm in
 *    hardware (HW) mode.
 *
 *  @pre
 *    The CERT is activated (see TCectActivate)
 *
 *  @param[in]  pxInput
 *    Buffer containing the input message to decrypt.
 *
 *  @param[out]  pxOutput
 *    Buffer where to write the decrypted message.
 *
 *  @param[in]  xMessageSize
 *    Size in bytes of messages (pxInput, pxOutput) to operate on. It must be a
 *    multiple of 8.
 *
 *  @param[in] pxInitVector
 *    8-byte buffer containing the SCWAL-DATA initialization vector (IV). This 
 *    parameter is optional. If it is NULL, message is decrypted in ECB mode. 
 *    Otherwise it is decrypted in CBC mode.
 *
 *  @param[in] pxKeyComp
 *    2-byte key component
 *
 *  @param[in] xVariant
 *    SCWAL-DATA algorithm variant to be used. The value of the variant is in
 *    range [0..7]
 *
 *  @retval CECT_NO_ERROR
 *    Operation completed successfully
 *
 *  @retval CECT_ERROR_BAD_PARAMETER
 *    - xMessageSize is not a multiple of 8
 *    - pxInput, pxOutput or pxKeyComp is NULL
 *    - xVariant is greater than 7
 *
 *  @retval CECT_ERROR
 *    Operation failed
 *
 *  @note
 *    - All buffers must be allocated by the caller
 *    - In-place operation (pxOutput=pxIntput) is supported
*/
typedef TCectStatus (*TCectDecryptScwalDataHardware)
(
        TUnsignedInt8*  pxInput,
        TUnsignedInt8*  pxOutput,
        size_t           xMessageSize,
  const TUnsignedInt8*  pxInitVector,
  const TUnsignedInt8*  pxKeyComp,
        TUnsignedInt8    xVariant
);


/**
 *  @ingroup scwal
 *
 *  @brief
 *    This function decrypt a message with the CERT SCWAL-DATA algorithm in
 *    hammer-head (HH) mode.
 *
 *  @pre
 *    The CERT is activated (see TCectActivate)
 *
 *  @param[in]  pxInput
 *    Buffer containing the input message to decrypt.
 *
 *  @param[out]  pxOutput
 *    Buffer where to write the decrypted message.
 *
 *  @param[in]  xMessageSize
 *    Size in bytes of messages (pxInput, pxOutput) to operate on. It must be a
 *    multiple of 8.
 *
 *  @param[in] pxInitVector
 *    8-byte buffer containing the SCWAL-DATA initialization vector (IV). This 
 *    parameter is optional. If it is NULL, message is decrypted in ECB mode. 
 *    Otherwise it is decrypted in CBC mode.
 *
 *  @param[in] pxKeyComp
 *    2-byte key component
 *
 *  @param[in] pxCipheredHhScwalKey
 *    32-byte buffer containing the ciphered HHSCWALKey. This buffer also
 *    contain the hammer-head IV and authentication tag:
 *    pxCipheredHhScwalKey = IV(8) || [HHSCWALKey](16) || TAG(8)
 *
 *  @param[in] xVariant
 *    SCWAL-DATA algorithm variant to be used. The value of the variant is in
 *    range [0..7]
 *
 *  @retval CECT_NO_ERROR
 *    Operation completed successfully
 *
 *  @retval CECT_ERROR_BAD_PARAMETER
 *    - xMessageSize is not a multiple of 8
 *    - pxInput, pxOutput, pxKeyComp or pxCipheredHhScwalKey is NULL
 *    - xVariant is greater than 7
 *
 *  @retval CECT_ERROR
 *    Operation failed
 *
 *  @note
 *    - All buffers must be allocated by the caller
 *    - In-place operation (pxOutput=pxIntput) is supported
*/
typedef TCectStatus (*TCectDecryptScwalDataHammerHead)
(
        TUnsignedInt8*  pxInput,
        TUnsignedInt8*  pxOutput,
        size_t           xMessageSize,
  const TUnsignedInt8*  pxInitVector,
  const TUnsignedInt8*  pxKeyComp,
  const TUnsignedInt8*  pxCipheredHhScwalKey,
        TUnsignedInt8    xVariant
);


/**
 *  @ingroup cushash
 *
 *  @brief
 *    This function computes a hash on a message with the CERT CusHash algorithm.
 *
 *  @pre
 *    The CERT is activated (see TCectActivate)
 *
 *  @param[in]  pxMessage
 *    Buffer containing the message to hash
 *
 *  @param[in]  xMessageSize
 *    Size in bytes of the message to operate on. It must be a multiple of 16.
 *
 *  @param[out]  pxHash
 *    16-byte buffer where to write the hash computed
 *
 *  @retval CECT_NO_ERROR
 *    Operation completed successfully
 *
 *  @retval CECT_ERROR_BAD_PARAMETER
 *    - xMessageSize is not a multiple of 16
 *    - pxMessage or pxHash is NULL
 *
 *  @retval CECT_ERROR
 *    Operation failed
 *
 *  @note
 *    - All buffers must be allocated by the caller
*/
typedef TCectStatus (*TCectProcessCusHash)
(
  const TUnsignedInt8*  pxMessage,
        size_t           xMessageSize,
        TUnsignedInt8*  pxHash
);


/**
 *  @ingroup perso
 *
 *  @brief
 *    This function programs the CERT SEG OTA record. This operation writes
 *    OTP memory and is non-reversible. If the operation is successful, it
 *    requires a reboot of the hardware platform.
 *
 *    This function requires an unique and/or a global segmentation descriptor 
 *    complying with the following format (see CAS_IrdPersoData):
 *
 *      tag            (1) = B0h
 *      length         (1) = 10h
 *      key_comp       (2)
 *      seguid         (2)
 *      segnr          (4)
 *      integra        (8)
 *
 *    Unique and global descriptors are identified thanks to the key_comp field. 
 *                     
 *  @param[in] xDescriptorsSize
 *    Size in bytes of pxDescriptors buffer.
 *
 *  @param[in] pxDescriptors
 *    Buffer, allocated by the caller, containing a collection of TLV descriptors
 *    including the segmentation descriptor to program in OTP memory.
 *
 *  @retval CECT_NO_ERROR
 *    Operation completed successfully
 *
 *  @retval CECT_ERROR_BAD_PARAMETER
 *    pxDescriptors is NULL
 *
 *  @retval CECT_ERROR
 *    Operation failed, including record authentication failure.
 *
 *  @retval CECT_ERROR_OTP_LOCKED
 *    The SEG record is locked and no longer modifiable
 *
 *  @note
 *    It is not necessary to activate the CERT block before calling this
 *    function.
*/
typedef TCectStatus (*TCectProgramSeg)
(
        size_t              xDescriptorsSize,
  const TUnsignedInt8*     pxDescriptors
);

/**
 *  @ingroup per
 *
 *  @brief
 *    This function programs the CERT OTAKeys record. This operation writes
 *    OTP memory and is non-reversible. If the operation is successful, it
 *    requires a reboot of the hardware platform.
 *
 *  @param[in]  pxOtaKeys
 *    Buffer containing the record to program in CERT OTP memory.
 *
 *  @retval CECT_NO_ERROR
 *    Operation completed successfully
 *
 *  @retval CECT_ERROR_BAD_PARAMETER
 *    pxOtaKeys is NULL
 *
 *  @retval CECT_ERROR
 *    Operation failed, including record authentication failure.
 *
 *  @retval CECT_ERROR_OTP_LOCKED
 *    The SEG record is locked and no longer modifiable
 *
 *  @note
 *    It is not necessary to activate the CERT block before calling this
 *    function.
*/
typedef TCectStatus (*TCectProgramOtaKeys)
(
  TCectOtaKeys*  pxOtaKeys
);



/**
 *  @ingroup functionTable
 *
 *  @brief
 *    Structure defining the CERT driver function table.
 */
typedef struct
{
  TUnsignedInt32 cectApiVersion;
   /**< Version of the CECT API. Use the macro CECTAPI_VERSION_INT to assign the
    *   the right value.
    */
  TCectSystemGetInfo cectSystemGetInfo;
   /**< Get CERT block system information (public data)
    */
  TCectActivate cectActivate;
   /**< Activate the CERT block
    */
  TCectCreateKeyLadder cectCreateKeyLadder;
   /**< Create a key ladder instance
    */
  TCectDisposeKeyLadder cectDisposeKeyLadder;
   /**< Dipose of a key ladder instance
    */
  TCectProcessRootLevelProtKey cectProcessRootLevelProtKey;
   /**< Process a root level protection key
    */
  TCectProcess1LevelProtKey cectProcess1LevelProtKey;
   /**< Process a 1-level protection key
    */
  TCectProcess2LevelProtKey cectProcess2LevelProtKey;
   /**< Process a 2-level protection key
    */
  TCectProcessBaeKeys cectProcessBaeKeys;
   /**<  Process BAE keys
    */
  TCectFinalizeBaeProcessing cectFinalizeBaeProcessing;
   /**< Finalize BAE processing
    */
  TCectEncryptLocalAlgoHardware cectEncryptLocalAlgoHardware;
   /**< Encrypt data with LocalAlgo in hardware mode
    */
  TCectDecryptLocalAlgoHardware cectDecryptLocalAlgoHardware;
   /**< Decrypt data with LocalAlgo in hardware mode
    */
  TCectEncryptIdeaNxtHammerHead cectEncryptIdeaNxtHammerHead;
   /**< Encrypt data with IDEANxT in hammer-head mode
    */
  TCectDecryptIdeaNxtHammerHead cectDecryptIdeaNxtHammerHead;
   /**< Decrypt data with IDEANxT in hammer-head mode
    */
  TCectDecryptIdeaNxtSoftware cectDecryptIdeaNxtSoftware;
   /**< Decrypt data with IDEANxT in software mode
    */
  TCectDecryptScwalDataHardware cectDecryptScwalDataHardware;
   /**< Decrypt data with SCWAL-DATA in hardware mode
    */
  TCectDecryptScwalDataHammerHead cectDecryptScwalDataHammerHead;
   /**< Decrypt data with SCWAL-DATA in hammer-head mode
    */
  TCectProcessCusHash cectProcessCusHash;
   /**< Process a custom hash
    */
  TCectProgramSeg cectProgramSeg;
   /**< Program SEG record
    */
  TCectProgramOtaKeys cectProgramOtaKeys;
   /**< Program OTAKeys record
    */
} TCectFunctionTable;



/******************************************************************************/
/*                                                                            */
/*                             FUNCTION PROTOTYPES                            */
/*                                                                            */
/******************************************************************************/


/**
 *  @ingroup fct_table
 *
 *  @brief
 *    This function returns a pointer to the CERT Command Translator function
  *   table if successful, NULL in case of error.
 *
*/
const TCectFunctionTable* cectGetFunctionTable
(
  void
);

#endif /* CA_CECT_H */

/* END OF FILE */

