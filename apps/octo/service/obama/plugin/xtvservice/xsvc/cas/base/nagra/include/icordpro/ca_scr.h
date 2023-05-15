/**
**  @file ca_scr.h
**
**  @brief
**   Stream Crypto Engine.
**
**   This file specifies the Local Stream Encryption API that provides
**   Nagra software with access to local stream encryption and decryption
**   resources managed by STB software. Access to these resources is needed
**   typically for setting content encryption and decryption keys.
**
**  @ingroup SCR
**
**  @version $Revision: 1.19 $
**
** Copyright:
**  2006-2007 Nagravision S.A.
**
*/
/*
** REMARK:
**    Comments in this file use special tags to allow automatic API
**    documentation generation in HTML format, using the
**    GNU-General Public Licensed Doxygen tool.
**    For more information about Doxygen, please check www.doxygen.org
*/
/**
 *  @defgroup SCR Stream Crypto Engine
 *  @brief
 *    Stream Crypto Engine component
*/


#ifndef CA_SCR_H
#define CA_SCR_H

/******************************************************************************/
/*                                                                            */
/*                               INCLUDE FILES                                */
/*                                                                            */
/******************************************************************************/

#ifdef SCR_WRAP_2_1
#include "scr_2_1w.h"
#endif /* SCR_WRAP_2_1 */

#ifdef SCR_GLUE_2_1
#include "scr_2_1g.h"
#endif /* SCR_GLUE_2_1 */

#include "ca_defs.h"
#include "ca_defsx.h"


/******************************************************************************/
/*                                                                            */
/*                              GROUPS DEFINITION                             */
/*                                                                            */
/******************************************************************************/

/**
 *  @defgroup groupEncrypt Stream Encryption
*/

/**
 *  @defgroup groupDecrypt Stream Decryption
*/

/******************************************************************************/
/*                                                                            */
/*                              TYPES DEFINITIONS                             */
/*                                                                            */
/******************************************************************************/

/**
 *  @ingroup groupEncrypt
 *  @brief
 *    Definition of the Stream Encryption Session Handler that uniquely
 *    identifies a stream encryption session used for locally encrypting an A/V
 *    stream possibly comprising multiple elementary streams.
*/
typedef struct SScrEncSessionHandler  TScrEncSessionHandler;

/**
 *  @ingroup groupDecrypt
 *  @brief
 *    Definition of the Stream Decryption Session Handler that uniquely
 *    identifies a stream decryption session used for locally decrypting an A/V
 *    stream possibly comprising multiple elementary streams.
*/
typedef struct SScrDecSessionHandler  TScrDecSessionHandler;


/**
 *  @brief
 *    Return values of the SCR methods.
*/
typedef enum
{
  SCR_NO_ERROR,
   /**<  The intended operation was executed successfully.
    */
  SCR_ERROR,
   /**<  The function terminated abnormally. The intended operation failed.
    */
  SCR_ERROR_WRONG_HANDLER,
   /**<  The session handler is invalid.
    */
  SCR_ERROR_WRONG_TS_ID,
   /**<  The transport session identifier is at this time unknown.
    */
  SCR_ERROR_NOT_IMPLEMENTED,
   /**< The function is not implemented,according to the project specifications.
    */
  SCR_ERROR_WRONG_EMI,
   /**< The EMI is either not set or not supported by the SCR implementation.
    */
  LAST_SCR_STATUS
   /**<  Not used.
    */
} TScrStatus;


/******************************************************************************/
/*                                                                            */
/*                             FUNCTION PROTOTYPES                            */
/*                                                                            */
/******************************************************************************/

/**
 *  @ingroup groupEncrypt
 *  @brief
 *    This function opens a stream encryption session. The pipe through which
 *    the stream to cipher flows is identified by the transport session
 *    identifier, which is provided by the STB appication software.
 *
 *    The function returns a handler that will be later used by the CA software
 *    to interact with the decryption session.
 *
 *  @param[out] ppxScrEncSessionHandler
 *    Pointer to the stream encryption session handler. It cannot be NULL.
 *
 *  @param[in] xTransportSessionId
 *    Identifier of the transport stream (or pipe) to which the encryption 
 *    shall be applied.
 *
 *  @retval  SCR_NO_ERROR
 *    The stream encryption session was successfully instantiated.
 *  @retval  SCR_ERROR_WRONG_TS_ID
 *    The transport session identifier is unknown at that time.
 *  @retval  SCR_ERROR
 *    The stream encryption session could not be instantiated.
 *
*/
TScrStatus scrOpenEncryptionSession
(
  TScrEncSessionHandler**   ppxScrEncSessionHandler,
  TTransportSessionId         xTransportSessionId
);

/**
 *  @ingroup groupEncrypt
 *  @brief
 *    This function closes a previously opened stream encryption session.
 *    Once this function has been called, the CA software will not use this session
 *    anymore and the driver can free all its resources.
 *
 *  @param[in] pxScrEncSessionHandler
 *    Stream encryption session handler provided by the SCR at session 
 *    instantiation.
 *
 *  @retval  SCR_NO_ERROR
 *    The stream encryption session was successfully closed.
 *  @retval  SCR_ERROR_WRONG_HANDLER
 *    The session handler is invalid.
 *  @retval  SCR_ERROR
 *    The stream encryption session could not be closed.
 *
*/
TScrStatus scrCloseEncryptionSession
(
  TScrEncSessionHandler*   pxScrEncSessionHandler
);

/**
 *  @ingroup groupEncrypt
 *  @brief
 *    This function set the EMI defining the encryption method to be used 
 *    to cipher the stream. This function is called before the call to 
 *    \c scrSetClearTextEncryptionKey().
 *
 *  @param[in] pxScrEncSessionHandler
 *    Pointer to the stream encryption session handler identifying the session
 *  @param[in] xEncryptionEmi
 *    Encryption Method to be used to cipher the stream.
 *
 *  @retval  SCR_NO_ERROR
 *    The encryption EMI was successfully set.
 *  @retval  SCR_ERROR_WRONG_HANDLER
 *    The session handler is invalid.
 *  @retval  SCR_ERROR_WRONG_EMI
 *    The encryption EMI is not supported by the SCR implementation.
 *  @retval  SCR_ERROR
 *    The encryption EMI could not be set.
 *
*/
TScrStatus scrSetEncryptionEmi
(
  TScrEncSessionHandler*  pxScrEncSessionHandler,
  TUnsignedInt16           xEncryptionEmi
);

/**
 *  @ingroup groupEncrypt
 *  @brief
 *    This function is used by the CA Software to set a clear-text stream 
 *    encryption key. It is called after \c scrSetEncryptionEmi() has been 
 *    called.
 *
 *    The key set by the CA software is in clear text.
 *    
 *    At the time this function returns without error (return value 
 *    \c SCR_NO_ERROR), the encryption key shall be properly set and the stream
 *    crypto engine ready to cipher the A/V stream.
 *
 *  @param[in] pxScrEncSessionHandler
 *    Stream encryption session handler identifying the session
 *  @param[in] xScrEncryptionKeySize
 *    Size of the encryption key.
 *  @param[in] pxScrEncryptionKey
 *    Pointer to the buffer containing the encryption key. That key is in clear 
 *    text.
 *
 *  @retval  SCR_NO_ERROR
 *    The function terminated successfully. The encryption key has been 
 *    properly set into the chipset and the stream crypto engine is ready to 
 *    work.
 *  @retval  SCR_ERROR
 *    The intended operation failed. The key has not been successfully set into
 *    the chipset and the crypto engine is not ready to work.
 *  @retval  SCR_ERROR_WRONG_HANDLER
 *    The handler is invalid.
 *  @retval  SCR_ERROR_NOT_IMPLEMENTED
 *    This function has not been implemented, according to the project 
 *    specifications.
 *  @retval  SCR_ERROR_WRONG_EMI
 *    The EMI is not set on the encryption session.
 *    
 *
*/
TScrStatus scrSetClearTextEncryptionKey
(
        TScrEncSessionHandler*  pxScrEncSessionHandler,
        TSize                    xScrEncryptionKeySize,
  const TUnsignedInt8*          pxScrEncryptionKey
);


/**
 *  @ingroup groupDecrypt
 *  @brief
 *    This function opens a stream decryption session. The pipe through which 
 *    the stream to cipher flows is identified by the transport session 
 *    identifier, which is provided by the STB application software.
 *
 *    The function returns a handler that will be later used by the CA software 
 *    to interact with the decryption session.
 *
 *  @param[out] ppxScrDecSessionHandler
 *    Pointer to the stream decryption session handler. It cannot be NULL.
 *  @param[in] xTransportSessionId
 *    Identifier of the transport stream (or pipe) to which the decryption 
 *    shall be applied.
 *
 *  @retval  SCR_NO_ERROR
 *    The stream decryption session was successfully opened.
 *  @retval  SCR_ERROR_WRONG_TS_ID
 *    The transport session identifier is unknown at that time.
 *  @retval  SCR_ERROR
 *    The stream decryption session could not be instantiated.
 *
*/
TScrStatus scrOpenDecryptionSession
(
  TScrDecSessionHandler** ppxScrDecSessionHandler,
  TTransportSessionId       xTransportSessionId
);


/**
 *  @ingroup groupDecrypt
 *  @brief
 *    This function closes a previously opened stream decryption session. Once
 *    this function called, the CA software will not use this session anymore 
 *    and the driver can free all its resources.
 *
 *  @param[in] pxScrDecSessionHandler
 *    Stream decryption session handler provided by the SCR at session 
 *    instantiation.
 *
 *  @retval  SCR_NO_ERROR
 *    The stream decryption session was successfully closed.
 *  @retval  SCR_ERROR_WRONG_HANDLER
 *    The decryption session handler is invalid.
 *  @retval  SCR_ERROR
 *    The stream decryption session could not be closed.
 *
*/
TScrStatus scrCloseDecryptionSession
(
  TScrDecSessionHandler* pxScrDecSessionHandler
);


/**
 *  @ingroup groupDecrypt
 *  @brief
 *    This function set the EMI defining the encryption method to be used to 
 *    decipher the stream by the SCR implementation. This function is called 
 *    before the call to \c scrSetClearTextDecryptionKey() or 
 *    \c scrSet1LevelProtectedDecryptionKey().
 *
 *  @param[in] pxScrDecSessionHandler
 *    Pointer to the stream decryption session handler identifying the session
 *  @param[in] xDecryptionEmi
 *    Encryption Method to be used to decipher the stream
 *
 *  @retval  SCR_NO_ERROR
 *    The decryption EMI was successfully set.
 *  @retval  SCR_ERROR_WRONG_HANDLER
 *    The session handler is invalid.
 *  @retval  SCR_ERROR_WRONG_EMI
 *    The decryption EMI is not supported by the SCR implementation.
 *  @retval  SCR_ERROR
 *    The decryption EMI could not be set.
*/
TScrStatus scrSetDecryptionEmi
(
  TScrDecSessionHandler*  pxScrDecSessionHandler,
  TUnsignedInt16           xDecryptionEmi
);

/**
 *  @ingroup groupDecrypt
 *  @brief
 *    This function is used by the CA Software to set a clear-text stream 
 *    decryption key. It is called after \c scrSetDecryptionEmi() has been 
 *    called.
 *
 *    At the time this function returns without error (return value 
 *    \c SCR_NO_ERROR), the decryption key shall be properly set and the stream
 *    crypto engine ready to decipher the A/V stream.
 *
 *  @param[in] pxScrDecSessionHandler
 *    Pointer to the stream decryption session handler identifying the session
 *  @param[in] xScrDecryptionKeySize
 *    Size of the decryption key
 *  @param[in] pxScrDecryptionKey
 *    Pointer to the buffer containing the decryption key. That key is in clear
 *    text.
 *
 *  @retval  SCR_NO_ERROR
 *    The function terminated successfully. The decryption key has been 
 *    properly set into the chipset and the stream crypto engine is ready to 
 *    work.
 *  @retval  SCR_ERROR_WRONG_HANDLER
 *    The handler is invalid.
 *  @retval  SCR_ERROR
 *    The intended operation failed. The key has not been successfully set into
 *    the chipset and the crypto engine is not ready to work.
 *  @retval  SCR_ERROR_WRONG_EMI
 *    The EMI is not set on the decryption session.
 *
*/
TScrStatus scrSetClearTextDecryptionKey
(
        TScrDecSessionHandler*  pxScrDecSessionHandler,
        TSize                    xScrDecryptionKeySize,
  const TUnsignedInt8*          pxScrDecryptionKey
);

/**
 *  @ingroup groupDecrypt
 *  @brief
 *    This function is used by the CA Software to set a protected stream 
 *    decryption key. The key set by the CA software is in cipher-text and is
 *    decrypted in a secure way by the hardware key ladder. It is called after 
 *    \c scrSetDecryptionSessionEmi() has been called.
 *
 *    At the time this function returns without error (return value 
 *    \c SCR_NO_ERROR), the decryption key shall be properly set and the stream
 *    crypto engine ready to decipher the A/V stream.
 *
 *  @param[in] pxScrDecSessionHandler
 *    Pointer to the stream decryption session handler identifying the session
 *  @param[in] xL1ProtectingKeySize
 *    Size of the Level 1 Key cipher-text in bytes
 *  @param[in] pxL1ProtectingKey
 *    Cipher-text input to level 1 key of Key Ladder
 *  @param[in] xProtectedDecryptionKeySize
 *    Size of the cipher-text decryption key
 *  @param[in] pxProtectedDecryptionKey
 *    Pointer to the buffer containing the decryption key. That key is in cipher
 *    text.
 *
 *  @retval  SCR_NO_ERROR
 *    The function terminated successfully. The decryption key has been 
 *    properly set into the chipset and the stream crypto engine is ready to 
 *    work.
 *  @retval  SCR_ERROR_WRONG_HANDLER
 *    The handler is invalid.
 *  @retval  SCR_ERROR
 *    The intended operation failed. The key has not been successfully set into
 *    the chipset and the crypto engine is not ready to work.
 *  @retval  SCR_ERROR_WRONG_EMI
 *    The EMI is not set on the decryption session.
 *
*/
TScrStatus scrSet1LevelProtectedDecryptionKey
(
        TScrDecSessionHandler*  pxScrDecSessionHandler,
        TSize                    xL1ProtectingKeySize,
  const TUnsignedInt8*          pxL1ProtectingKey,
        TSize                    xProtectedDecryptionKeySize,
  const TUnsignedInt8*          pxProtectedDecryptionKey
);

/**
 *  @ingroup groupDecrypt
 *  @brief
 *    This function configures the SCR to use the legacy 
 *    DVR mode for this decryption session. This mode is used to be compatible
 *    with legacy DVR contents, it replaces the call to \c scrSetDecryptionEmi 
 *    and  \c scrSet1LevelProtectedDecryptionKey or 
 *    \c scrSetClearTextDecryptionKey. In this mode, the SCR implementation
 *    is responsible to generate the content key itself.
 *
 *  @param[in] pxScrDecSessionHandler
 *    Pointer to the stream decryption session handler identifying the session
 *
 *  @retval  SCR_NO_ERROR
 *    The legacy decryption mode was successfully set.
 *  @retval  SCR_ERROR_WRONG_HANDLER
 *    The session handler is invalid.
 *  @retval  SCR_ERROR
 *    The legacy decryption mode could not be set.
*/
TScrStatus scrSetLegacyDecryptionMode
(
  TScrDecSessionHandler*  pxScrDecSessionHandler
);

#endif /* CA_SCR_H */

/* END OF FILE */
