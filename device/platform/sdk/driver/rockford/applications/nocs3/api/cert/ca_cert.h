/**
**  @file ca_cert.h
**
**  @brief
**   CERT.
**
**   This file specifies the CERT API that allows Nagra CA
**   software to access the Nagra CERT block.
**
**  @version $Revision: 1.5 $
**
** Copyright:
**   2011 Nagravision S.A.
**
*/
/*
** REMARK:
**    Comments in this file use special tags to allow automatic API
**    documentation generation in HTML format, using the
**    GNU-General Public Licensed Doxygen tool.
**    For more information about Doxygen, please check www.doxygen.org
*/



#ifndef CA_CERT_H
#define CA_CERT_H

/******************************************************************************/
/*                                                                            */
/*                           GENERAL INCLUDE FILES                            */
/*                                                                            */
/******************************************************************************/

#include <stddef.h>

/******************************************************************************/
/*                                                                            */
/*                              GROUPS DEFINITION                             */
/*                                                                            */
/******************************************************************************/

/** @defgroup functionTable Function Table */
/** @defgroup lockMgt Resource Lock Management */
/** @defgroup commandHandling Command Handling */
/** @defgroup keysMgt Keys Management */



/******************************************************************************/
/*                                                                            */
/*                              VERSION TOOL                                  */
/*                                                                            */
/******************************************************************************/

/**
 * Defines the version number of the CERT API to be implemented. This version 
 * has to be included in the function table returned by certGetFunctionTable(). 
 * To do so, use the macro CERTAPI_VERSION_INT to put it in the right format. 
 * Note also that this version number does not necessarily correspond to the 
 * version number of the PDF document defining the CERT Driver API.
*/ 
#define CERTAPI_VERSION_MAJOR  1
#define CERTAPI_VERSION_MEDIUM 0
#define CERTAPI_VERSION_MINOR  1

#ifndef CERT_TOOL_VERSION
#define CERT_TOOL_STRINGIFY(s) CERT_TOOL_TOSTRING(s)
#define CERT_TOOL_TOSTRING(s)  #s

#define CERT_TOOL_VERSION_INT(a, b, c) (a<<16 | b<<8 | c)
#define CERT_TOOL_VERSION_DOT(a, b, c) a ##.## b ##.## c
#define CERT_TOOL_VERSION(a, b, c) CERT_TOOL_VERSION_DOT(a, b, c)
#endif

#define CERTAPI_VERSION_INTER CERT_TOOL_VERSION(CERTAPI_VERSION_MAJOR, \
              CERTAPI_VERSION_MEDIUM, \
              CERTAPI_VERSION_MINOR)

#define CERTAPI_VERSION_INT   CERT_TOOL_VERSION_INT(CERTAPI_VERSION_MAJOR, \
            CERTAPI_VERSION_MEDIUM, \
            CERTAPI_VERSION_MINOR)

#define CERTAPI_VERSION_STRING  "CERTAPI_" CERT_TOOL_STRINGIFY(CERTAPI_VERSION_INTER)

/******************************************************************************/
/*                                                                            */
/*                              TYPES DEFINITIONS                             */
/*                                                                            */
/******************************************************************************/

/**
 *  @brief
 *    Unsigned 8-bit type.
*/
typedef unsigned char         TCertUInt8;

/**
 *  @brief
 *    Unsigned 16-bit type.
*/
typedef unsigned short int    TCertUInt16;

/**
 *  @brief
 *    Unsigned 32-bit type.
*/
typedef unsigned long int     TCertUInt32;

/**
 *  @brief
 *    Status code returned by functions of the CERT API
*/
typedef enum
{
  CERT_NO_ERROR,
   /**< Requested operation was completed successfully
    */
  CERT_ERROR_BAD_HANDLE,
   /**< Handle provided is invalid or unknown
    */
  CERT_ERROR_BAD_EMI,
   /**< EMI provided is not supported
    */
  CERT_ERROR_BAD_USAGE,
   /**< Key usage rules are not respected
    */
  CERT_ERROR_TIMEOUT,
   /**< Execution of a CERT command lasted too long
    */
  CERT_ERROR,
   /**< Requested operation failed
    */
  LAST_CERT_STATUS
   /**< Internal use only.
    */
} TCertStatus;



/**
 *  @ingroup commandHandling
 *
 *  @brief
 *    Type used to characterize the processing duration a CERT command.
 *    To be used by the CERT driver to know how long to wait for the end of a
 *    command processing. Associated timeout values is to be defined by the
 *    entity in charge of developing the CERT driver.
*/
typedef enum
{
  CERT_TIMEOUT_DEFAULT,
   /**<  Default timeout. Indicates that the associated command does not write
    *    in OTP.
    */
  CERT_TIMEOUT_OTP,
   /**<  The associated command is writing in OTP memory.
    */
  LAST_CERT_TIMEOUT
   /**<  Internal use only.
    */
} TCertTimeoutType;


/**
 *  @ingroup keysMgt
 *
 *  @brief
 *    Defines whether the given key is even or odd. It is used to set the key in
 *    the right key table register. If the parity is not defined, the key has to
 *    be set in both even and odd registers.
 *
*/
typedef enum
{
  CERT_KEY_PARITY_EVEN,
   /**<  Signaled an even key
    */
  CERT_KEY_PARITY_ODD,
   /**<  Signaled an odd key
    */
  CERT_KEY_PARITY_UNDEFINED,
   /**<  The parity of is not defined, thus not relevant. Key has to be set in
    *    both even and odd key regiters.
    */
  LAST_CERT_KEY_PARITY
   /**<  Internal use only.
    */
} TCertKeyParity;


/**
 *  @brief
 *    Pointer to an opaque structure to be defined by the entity in charge of
 *    developing the CERT driver and used as a handle on the CERT block resource.
 *    
 *    This handle is an output of certLock() and given back in input of 
 *    certExchange() and certUnlock(). It is not used by certUseEncryptionKey() 
 *    and certUseDecryptionKey() because these key management functions are 
 *    called by the DAL layer and not by CA applications in charge of locking 
 *    the CERT resource.
 */
typedef struct SCertResourceHandle* TCertResourceHandle;


/**
 *  @ingroup keysMgt
 *
 *  @brief
 *    Opaque structure used to accommodate parameters defining a key path. This
 *    structure is to be defined by the entity in charge of developing the CERT
 *    driver.
 */
typedef struct SCertKeyPathHandle TCertKeyPathHandle;


/**
 *  @ingroup commandHandling
 *
 *  @brief
 *    Structure used to accommodate parameters of a CERT command.
 */
typedef struct
{
  TCertUInt8 inputData[32];
   /**< Input data to be written in the 8 32-bit CERT common interface DATA_IN_X
    *   registers. First 4 MSB inputData[0:3] shall be written in DATA_IN_0
    *   register, next 4 MSB inputData[4:7] in DATA_IN_1 register and so on.
    */
  TCertUInt8 outputData[32];
   /**< Buffer where to write values of the 8 32-bit CERT common inteface
    *   DATA_OUT_X registers following the processing of a command. DATA_OUT_0
    *   register value shall be written in the first 4 MSB outputData[0:3],
    *   DATA_OUT_1 register in next 4 MSB outputData[4:7] and so on.
    */
  TCertUInt8 status[4];
   /**< Buffer where to write the value of the CERT common interface STATUS_AKL
    *   register following the processing of a command.
    */
  TCertUInt8 opcodes[4];
   /**< Command operation codes to be written in the CERT common interface
    *   COMMAND register. The least signigicant bit that acts as the command
    *   start bit is already set to 1.
    */
  TCertTimeoutType timeout;
   /**<  This field characterizes the processing duration of the command. It is 
    *    not expressed as a duration. Associated timeout values is to be defined 
    *    by the entity in charge of developing the CERT driver. This field is 
    *    mainly used to know whether the command is going to write in OTP or 
    *    not.
    */
} TCertCommand;


/******************************************************************************/
/*                                                                            */
/*                           FUNCTION TABLE DEFINITION                        */
/*                                                                            */
/******************************************************************************/

/**
 *  @ingroup lockMgt
 *
 *  @brief
 *    This function reserves the CERT resource for exclusive use to the caller.
 *    It returns a handle to the CERT resource if it is not in use. Otherwise,
 *    the function blocks until the resource is freed.
 *
 *  @param[out] pxResourceHandle
 *    Handle assigned to the CERT resource.
 *
 *  @retval CERT_NO_ERROR
 *    The CERT resource was locked successfully
 *
 *  @retval CERT_ERROR
 *    Errors occurred and operation could not be completed successfully (e.g. OS
 *    errors). This status has nothing to do with the availability of the CERT
 *    resource.
 *
*/
typedef TCertStatus (*TCertLock)
(
  TCertResourceHandle* pxResourceHandle
);


/**
 *  @ingroup lockMgt
 *
 *  @brief
 *    This function releases the CERT resource previously locked by
 *    certLock().
 *
 *  @param[in] xResourceHandle
 *    Handle to the CERT resource.
 *
 *  @retval CERT_NO_ERROR
 *    The operation was completed successfully and the CERT lock released.
 *
 *  @retval CERT_ERROR_BAD_HANDLE
 *    Handle provided is invalid or unknown. One typically drops in this case if
 *    this function is called while the CERT resource has not been locked.
 *
 *  @retval CERT_ERROR
 *    Errors occurred and the operation could not be completed successfully.
 *
*/
typedef TCertStatus (*TCertUnlock)
(
  TCertResourceHandle xResourceHandle
);


/**
 *  @ingroup commandHandling
 *
 *  @brief
 *    This function is used to send a series of commands to the CERT block. For
 *    each command part pxCommands buffer, the CERT driver has to systematically
 *    write the 8 32-bit CERT input registers (DATA_IN_X), wait for the command
 *    processing completion and read the 8 32-bit CERT output registers
 *    (DATA_OUT_X) as well as the 32-bit status register (STATUS_AKL).
 *
 *    If a command fails (least significant bit of STATUS_AKL set to 1) or lasts
 *    longer than expected (timeout), certExchange must return to the caller
 *    without handling remaining commands, if any. The number of commands
 *    processed so far, including the command that failed, must be signaled
 *    accordingly in pxNumOfProcessedCommands. A command that timed out is not
 *    considered as processed, thus must not be counted in
 *    pxNumOfProcessedCommands. Values of DATA_OUT_X registers and STATUS_AKL
 *    register of the faulty command must be returned as well.
 *
 *  @param[in] xResourceHandle
 *    Handle to the CERT resource.
 *
 *  @param[in] xNumOfCommands
 *    Number of commands to be processed by the CERT block.
 *
 *  @param[in,out] pxCommands
 *    Buffer containing xNumOfCommands instantiations of TCertCommand structure.
 *    This structure is used to accommodate input parameters of each command as
 *    well as resulting output. The memory is allocated by the CA application.
 *
 *  @param[out] pxNumOfProcessedCommands
 *    Number of commands actually processed by the CERT block. If all commands 
 *    are processed successfully, this number is equal to xNumOfCommands. If a 
 *    command fails or times out, this parameter indicates the number of 
 *    commands processed so far, including the faulty command but not the timed 
 *    out command.
 *
 *  @retval CERT_NO_ERROR
 *    All commands have been processed successfully. A command is successful if
 *    the least significant bit of STATUS_AKL is equal to 0 after the command
 *    completion.
 *
 *  @retval CERT_ERROR_BAD_HANDLE
 *    Handle provided is invalid or unknown. One typically drops in this case if
 *    this function is called while the CERT resource has not been locked.
 *
 *  @retval CERT_ERROR_TIMEOUT
 *    The timeout has expired and the CERT block is still processing a command
 *    (least significant bit of COMMAND register still to 1).
 *
 *  @retval CERT_ERROR
 *    Any other errors occurred at driver level or reported by the CERT block
 *    during the processing of a command (least significant bit of STATUS_AKL
 *    equal to 1).
 *
*/
typedef TCertStatus (*TCertExchange)
(
        TCertResourceHandle  xResourceHandle,
        size_t               xNumOfCommands,
  const TCertCommand*       pxCommands,
        size_t*             pxNumOfProcessedCommands
);


/**
 *  @ingroup keysMgt
 *
 *  @brief
 *    This function fetches a key and related usage rules pending on the CERT
 *    bus and sets it into the secure key table of a crypto engine of the SoC
 *    for encryption operations. The encryption algorithm is specified by the
 *    xEmi parameter. This function also acknowledges the CERT block
 *    that another key may be output on the bus if necessary.
 *
 *  @param[in] xEmi
 *    Encryption method indicator identifying the encryption algorithm.
 *
 *  @param[in,out] pxKeyPathHandle
 *    Pointer to an opaque structure defining the key path used to set the key.
 *
 *  @param[in] xKeyParity
 *    Indicates whether the parity of the CERT-provided key is even, odd or
 *    undefined.
 *
 *  @retval CERT_NO_ERROR
 *    The operation was completed successfully.
 *
 *  @retval CERT_ERROR_BAD_HANDLE
 *    Key path handle is invalid or unknown.
 *
 *  @retval CERT_ERROR_BAD_EMI
 *    EMI provided is not supported
 *
 *  @retval CERT_ERROR_BAD_USAGE
 *    Key usage rules are not respected, e.g. algorithm or operation (encryption,
 *    decryption) is not allowed.
 *
 *  @retval CERT_ERROR
 *    Errors occurred and the operation could not be completed successfully.
 *
*/
typedef TCertStatus (*TCertUseEncryptionKey)
(
  TCertUInt16           xEmi,
  TCertKeyPathHandle*  pxKeyPathHandle,
  TCertKeyParity        xKeyParity
);


/**
 *  @ingroup keysMgt
 *
 *  @brief
 *    This function fetches a key and related usage rules pending on the CERT
 *    bus and sets it into the secure key table of a crypto engine of the SoC
 *    for decryption operations. The encryption algorithm is specified by the
 *    xEmi parameter. This function also acknowledges the CERT block
 *    that another key may be output on the bus if necessary.
 *
 *  @param[in] xEmi
 *    Encryption method indicator identifying the encryption algorithm.
 *
 *  @param[in,out] pxKeyPathHandle
 *    Pointer to an opaque structure defining the key path used to set the key.
 *
 *  @param[in] xKeyParity
 *    Indicates whether the parity of the CERT-provided key is even, odd or
 *    undefined.
 *
 *  @retval CERT_NO_ERROR
 *    The operation was completed successfully
 *
 *  @retval CERT_ERROR_BAD_HANDLE
 *    Key path handle is invalid or unknown
 *
 *  @retval CERT_ERROR_BAD_EMI
 *    EMI provided is not supported
 *
 *  @retval CERT_ERROR_BAD_USAGE
 *    Key usage rules are not respected, e.g. algorithm or operation (encryption,
 *    decryption) is not allowed.
 *
 *  @retval CERT_ERROR
 *    Errors occurred and the operation could not be completed successfully.
 *
*/
typedef TCertStatus (*TCertUseDecryptionKey)
(
  TCertUInt16           xEmi,
  TCertKeyPathHandle*  pxKeyPathHandle,
  TCertKeyParity        xKeyParity
);



/**
 *  @ingroup functionTable
 *
 *  @brief
 *    Structure defining the CERT driver function table.
 */
typedef struct
{
  TCertUInt32 certApiVersion;
   /**< Version of the CERT API. Use the macro CERTAPI_VERSION_INT to assign the
    *   the right value.
    */
  TCertLock certLock;
   /**< Function to reserve the CERT resource for exclusive use
    */
  TCertUnlock certUnlock;
   /**< Function to release the CERT resource
    */
  TCertExchange certExchange;
   /**< Function to perform transactions with the CERT block
    */
  TCertUseEncryptionKey certUseEncryptionKey;
   /**< Use key available on CERT block key interface bus for encryption 
    *   operations.
    */
  TCertUseDecryptionKey certUseDecryptionKey;
   /**< Use key available on CERT block key interface bus for decryption
    *   operations.
    */
} TCertFunctionTable;



/******************************************************************************/
/*                                                                            */
/*                             FUNCTION PROTOTYPES                            */
/*                                                                            */
/******************************************************************************/


/**
 *  @ingroup functionTable
 *
 *  @brief
 *    This function returns a pointer to the CERT driver function table if
 *    successful, NULL in case of error.
 *
*/
TCertFunctionTable* certGetFunctionTable
(
  void
);


#endif /* CA_CERT_H */

/* END OF FILE */
