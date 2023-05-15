/**
**  @file ca_sec.h
**
**  @brief
**   Secure Chipset management.
**
**   This file specifies the Secure Chipset API that allows Nagra CA
**   software to access security features of the MPEG chipset.
**
**  @ingroup SEC
**
**  @version $Revision: 1.15 $
**
** Copyright:
**   2006-2007 Nagravision S.A.
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
 *  @defgroup SEC Secure Chipset
 *  @brief
 *    Descrambler component
 *
 *    This component is the interface to the descrambler driver.
*/


#ifndef CA_SEC_H
#define CA_SEC_H

/******************************************************************************/
/*                                                                            */
/*                               INCLUDE FILES                                */
/*                                                                            */
/******************************************************************************/

#ifdef SEC_WRAP_2_1
#include "sec_2_1w.h"
#endif

#ifdef SEC_GLUE_2_1
#include "sec_2_1g.h"
#endif

#include "ca_defs.h"


/******************************************************************************/
/*                                                                            */
/*                              TYPES DEFINITIONS                             */
/*                                                                            */
/******************************************************************************/

/**
 *  @brief
 *    Definition of the NUID programmed in the OTP memory of the chipset by the
 *    chip manufacturer.
*/
typedef TUnsignedInt8 TSecNuid[4];


/**
 *  @brief
 *    Status code returned by functions of the SEC API.
 *
*/
typedef enum
{
  SEC_NO_ERROR,
   /**<  The intended operation was executed successfully.
    */
  SEC_ERROR,
   /**<  The function terminated abnormally. The intended operation failed.
    */
  SEC_ERROR_NOT_IMPLEMENTED,
   /**<  The function is not implemented.
    */
  LAST_SEC_STATUS
   /**<  Not used.
    */
} TSecStatus;


/******************************************************************************/
/*                                                                            */
/*                             FUNCTION PROTOTYPES                            */
/*                                                                            */
/******************************************************************************/

/**
 *  @brief
 *    This function returns a 4-byte array containing the unique chipset 
 *    identifier (NUID) that was programmed into the chipset's internal OTP
 *    memory by the chip manufacturer.
 *    
 *  @param[out] pxNuid
 *    Address of the array in which the function has to write the 4-byte NUID.
 *    Watch out, in order to avoid any alignement or endianness issues, the
 *    NUID has to handled as an array of bytes and not as a 32-bit unsigned
 *    integer.
 *
 *  @retval  SEC_NO_ERROR
 *    The NUID was read successfully.
 *
 *  @retval  SEC_ERROR
 *    Found problems while trying to read the NUID.
 *
*/
TSecStatus secGetNuid
(
  TSecNuid* pxNuid
);


/**
 *  @brief
 *    This function returns a null-terminated string containing the revision of
 *    the chipset (aka chipset cut).
 *
 *  @param[out] ppxChipsetRevision
 *    Pointer to be filled with the chipset revision coded as a null-terminated
 *    string. The SEC shall guaranty the integrity of the buffer and never free
 *    it.
 *
 *  @retval  SEC_NO_ERROR
 *    The operation was completed successfully.
 *
 *  @retval  SEC_ERROR
 *    The operation failed.
 *
*/
TSecStatus secGetChipsetRevision
(
  const TChar** ppxChipsetRevision
);


/**
 *  @brief
 *    This function asks the RAM2RAM Crypto Engine to encrypt a block of data
 *    stored in RAM and write the result back in RAM.
 *
 *    The length of the input data block must be a multiple of 8 bytes. 
 *    Otherwise, the function shall return SEC_ERROR.
 *
 *    The contents of the buffers pointed at by pxDst and pxSrc are both a 
 *    sequence of 8-byte blocks in memory. Each 8-byte block shall be encoded 
 *    in MSBF (Most-Significant Byte First, First meaning lowest address). 
 *    Specifically, given a 64 bit data block as represented in FIPS PUB 46-3, 
 *    the bit number 1 of the block shall be stored as the most-significant bit 
 *    of the first byte in memory (lowest address) and bit number 64 of the 
 *    block shall be stored as the least-significant bit of the last byte in 
 *    memory (highest address).
 *  
 *    Beware that underlying hardware may use a byte ordering other than what 
 *    is specified in this document. For example, some DMA-based engines may 
 *    read and write data in a little-endian way, whereas this API requires 
 *    big-endian representation. Similar issues may appear with non-DMA 
 *    hardware engines. If the underlying hardware is not accessed directly but
 *    through a software library provided by the chip vendor, similar issues 
 *    may arise with the chip vendor's API.
 *
 *    In all such cases, the DAL implementation shall change the byte ordering 
 *    of input data so it is interpreted correctly by the underlying hardware; 
 *    it shall also change the byte ordering of output data so it is presented 
 *    in the right order to the API client.
 *
 *  @param[out] pxDst
 *    Pointer to the location in RAM where the encrypted result has to be 
 *    written. The client software is responsible for allocating memory.
 *
 *  @param[in] pxSrc
 *    Block of data in RAM that has to be encrypted.
 *
 *  @param[in] xDataLength
 *    Length in bytes of the input (pxSrc) and output (pxDst) data blocks. It 
 *    shall be a multiple of 8.
 *
 *  @retval SEC_NO_ERROR
 *    The operation was completed successfully.
 *
 *  @retval SEC_ERROR
 *    Errors occurred and the operation could not be completed successfully.
 *
 *  @retval SEC_ERROR_NOT_IMPLEMENTED
 *    The function is not implemented
 *
*/
TSecStatus secEncryptData
(
        TUnsignedInt8*  pxDst,
  const TUnsignedInt8*  pxSrc,
        TSize           xDataLength
);


/**
 *  @brief
 *    This function asks the RAM2RAM Crypto Engine to decrypt a block of data
 *    stored in RAM and write the result back in RAM.
 *
 *    The length of the input data block must be a multiple of 8 bytes. 
 *    Otherwise, the function shall return SEC_ERROR.
 *
 *    The contents of the buffers pointed at by pxDst and pxSrc are both a 
 *    sequence of 8-byte blocks in memory. Each 8-byte block shall be encoded 
 *    in MSBF (Most-Significant Byte First, First meaning lowest address). 
 *    Specifically, given a 64 bit data block as represented in FIPS PUB 46-3, 
 *    the bit number 1 of the block shall be stored as the most-significant bit 
 *    of the first byte in memory (lowest address) and bit number 64 of the 
 *    block shall be stored as the least-significant bit of the last byte in 
 *    memory (highest address).
 *  
 *    Beware that underlying hardware may use a byte ordering other than what 
 *    is specified in this document. For example, some DMA-based engines may 
 *    read and write data in a little-endian way, whereas this API requires 
 *    big-endian representation. Similar issues may appear with non-DMA 
 *    hardware engines. If the underlying hardware is not accessed directly but
 *    through a software library provided by the chip vendor, similar issues 
 *    may arise with the chip vendor's API.
 *
 *    In all such cases, the DAL implementation shall change the byte ordering 
 *    of input data so it is interpreted correctly by the underlying hardware; 
 *    it shall also change the byte ordering of output data so it is presented 
 *    in the right order to the API client.
 *
 *  @param[out] pxDst
 *    Pointer on the location in RAM where the decrypted result has to be 
 *    written. The client software is responsible for allocating memory.
 *
 *  @param[in] pxSrc
 *    Block of data that has to be decrypted.
 *
 *  @param[in] xDataLength
 *    Length in bytes of the input (pxSrc) and output (pxDst) data blocks. 
 *    It shall be a multiple of 8.
 *
 *  @retval  SEC_NO_ERROR
 *    The operation was completed successfully.
 *
 *  @retval  SEC_ERROR
 *    Errors occurred and the operation could not be completed successfully.
 *
 *  @retval  SEC_ERROR_NOT_IMPLEMENTED
 *    The function is not implemented.
 *
*/
TSecStatus secDecryptData
(
        TUnsignedInt8*  pxDst,
  const TUnsignedInt8*  pxSrc,
        TSize           xDataLength
);
#endif /* CA_SEC_H */

/* END OF FILE */
