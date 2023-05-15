/*
 * (c) 2011-2012 Humax Co., Ltd.
 * This program is produced by Humax Co., Ltd. ("Humax") and
 * the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
 * non-assignable, non-transferable and non-exclusive license to use this Software.
 * You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
 * you agree to the responsibility to take all reasonable efforts to protect the any information
 * you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
 * reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
 * If you have no authorized license, discontinue using this Software immediately.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
 * IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
 * IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies
 * of the Software including all documentation. This License will terminate immediately without notice from Humax
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
 * of the Software and all documentation.

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
 * in the Republic of Korea.
*/

#include "octo_common.h"
#include "UniversalClient_API.h"

/**
 * Obtain the current version of the Cloaked CA Agent
 *
 * This function can be called to obtain the current version of the Cloaked CA Agent. This version can be used by applications to
 * check for supported features and application compatibility.  An application might also display a status screen with
 * version information on it that can be used for technical support purposes.
 *
 * \note The information from this method must be displayed in a GUI dialog.
 *
 * @param[in,out] pVersion Receives the version string associated with the client.
 *     The application must set the uc_buffer_st::bytes member to point to a buffer in memory, and the uc_buffer_st::length member
 *     to the maximum length of the buffer, in bytes.  The function will modify the contents of the buffer to contain the version
 *     string, including NULL terminator, and set the uc_buffer_st::length member to the actual length of data returned.
 *     If the uc_buffer_st::length member is not large enough to hold the entire version string, the uc_buffer_st::length member
 *     will be set to the minimum required amount, and the function will return ::UC_ERROR_INSUFFICIENT_BUFFER. The application
 *     can then allocate memory of a sufficient size and attempt to call the function again.
 *
 *     \note For simplicity, the application can allocate ::UC_BUFFER_GENERIC_SIZE bytes memory for uc_buffer_st::bytes
 *       and set uc_buffer_st::length to ::UC_BUFFER_GENERIC_SIZE. Cloaked CA Agent will set uc_buffer_st::length to the actual size,
 *       and the length includes 1 byte NULL terminator.
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *    ::UC_ERROR_INSUFFICIENT_BUFFER
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClient_GetVersion(
        uc_buffer_st *pVersion)
{
	return UC_ERROR_SUCCESS;
}

/**
 * Obtain the build information of the Cloaked CA Agent
 *
 * This function can be called to obtain the build information of the Cloaked CA Agent.
 * The Application shall display the build info in the client status screen.
 *
 * \note The information from this method must be displayed in a GUI dialog.
 *
 * @param[in,out] pBuildInformation Receives the build string associated with the client.
 *     The application must set the uc_buffer_st::bytes member to point to a buffer in memory, and the uc_buffer_st::length member
 *     to the maximum length of the buffer, in bytes.  The function will modify the contents of the buffer to contain the build
 *     string, including NULL terminator, and set the uc_buffer_st::length member to the actual length of data returned.
 *     If the uc_buffer_st::length member is not large enough to hold the entire build string, the uc_buffer_st::length member
 *     will be set to the minimum required amount, and the function will return ::UC_ERROR_INSUFFICIENT_BUFFER. The application
 *     can then allocate memory of a sufficient size and attempt to call the function again.
 *
 *     \note For simplicity, the application can allocate ::UC_BUFFER_GENERIC_SIZE bytes memory for uc_buffer_st::bytes
 *       and set uc_buffer_st::length to ::UC_BUFFER_GENERIC_SIZE. Cloaked CA Agent will set uc_buffer_st::length to the actual size,
 *       and the length includes 1 byte NULL terminator.
 *
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *    ::UC_ERROR_INSUFFICIENT_BUFFER
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClient_GetBuildInformation(
        uc_buffer_st *pBuildInformation)
{
	return UC_ERROR_SUCCESS;
}

/**
 * Obtain a description for functionalities provided by Cloaked CA Agent
 *
 * This function can to called to obtain a description for the functionalities provided by Cloaked CA Agent.
 *
 * \note The information from this method must be displayed in a GUI dialog.
 *
 * @param[in,out] pCapabilities Receives the string of functionalities description associated with the client.
 *     On output, the uc_buffer_st::bytes member will be filled with a NULL-terminated string describing the functionalities.
 *
 *     The application must set the uc_buffer_st::bytes member to point to a buffer in memory, and the uc_buffer_st::length member
 *     to the maximum length of the buffer, in bytes.  The function will modify the contents of the buffer to contain the capabilities
 *     string, including NULL terminator, and set the uc_buffer_st::length member to the actual length of data returned.
 *     If the uc_buffer_st::length member is not large enough to hold the entire string of functionalities description, the uc_buffer_st::length member
 *     will be set to the minimum required amount, and the function will return ::UC_ERROR_INSUFFICIENT_BUFFER. The application
 *     can then allocate memory of a sufficient size and attempt to call the function again.
 *
 *     \note For simplicity, the application can allocate ::UC_BUFFER_GENERIC_SIZE bytes memory for uc_buffer_st::bytes
 *       and set uc_buffer_st::length to ::UC_BUFFER_GENERIC_SIZE. Cloaked CA Agent will set uc_buffer_st::length to the actual size,
 *       and the length includes 1 byte NULL terminator.
 *
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *    ::UC_ERROR_INSUFFICIENT_BUFFER
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClient_GetCapabilities(
        uc_buffer_st *pCapabilities)
{
	return UC_ERROR_SUCCESS;
}

/**
 * Obtain the secure type of Cloaked CA Agent
 *
 * This function can be called to obtain the secure type of Cloaked CA Agent.
 * If the output value is 0, the secure type is "Secure Chipset".
 * If the output value is 1, the secure type is "Security ID".
 *
 * \note The information from this method must be displayed in a GUI dialog.
 *
 * @param[out] pSecureType Gets the current secure type of Cloaked CA Agent.
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClient_GetSecureType(
        uc_byte *pSecureType)
{
	return UC_ERROR_SUCCESS;
}

/**
 * Obtain the current LOCK ID in Cloaked CA Agent
 *
 * This function can be called to obtain the LOCK ID embeded in the Cloaked CA Agent.
 * A LOCK ID is a value assigned to a manufacturer.
 *
 * \note The information from this method must be displayed in a GUI dialog.
 *
 * @param[out] pLockId Receives the current LOCK ID in Cloaked CA Agent.
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClient_GetLockId(
        uc_uint16 *pLockId)
{
	return UC_ERROR_SUCCESS;
}

/**
 * Obtain the current Serial Number of the Cloaked CA Agent
 *
 * This function can be used to obtain the Serial Number of the Cloaked CA Agent.  This serial
 * number is assigned by the headend system, and is used to uniquely address a particular device
 * when sending \ref glossary_emm EMMs to it.
 *
 * \note The information from this method must be displayed in a GUI dialog.
 *
 * \note This serial number MAY BE DIFFERENT from the Chipset Serial Number (CSSN) that is returned
 * from ::UniversalClientSPI_Device_GetCSSN.
 *
 * @param[in,out] pSN Receives the serial number assigned to the client.
 *     The application must set the uc_buffer_st::bytes member to point to a buffer in memory, and the uc_buffer_st::length
 *     member to the maximum length of the buffer, in bytes.  The function will modify the contents of the buffer
 *     to contain the serial number in binary big-endian format, without any terminating character, and set the
 *     uc_buffer_st::length member to the actual length of data returned.
 *     If the uc_buffer_st::length member is not large
 *     enough to hold the entire serial number, the uc_buffer_st::length member will be set to the minimum required
 *     amount, and the function will return ::UC_ERROR_INSUFFICIENT_BUFFER. The application can then allocate
 *     memory of a sufficient size and attempt to call the function again.
 *
 *     \note For simplicity, the application can allocate ::UC_BUFFER_GENERIC_SIZE bytes memory for uc_buffer_st::bytes
 *       and set uc_buffer_st::length to ::UC_BUFFER_GENERIC_SIZE. Cloaked CA Agent will set uc_buffer_st::length to the actual size.
 *
 *     \n
 *     \note
 *     Suppose the Serial Number is 2864434397 i.e. 0xAABBCCDD, uc_buffer_st::bytes will have the following values: \n
 *     uc_buffer_st::bytes[0] = 0xAA, \n
 *     uc_buffer_st::bytes[1] = 0xBB, \n
 *     uc_buffer_st::bytes[2] = 0xCC, \n
 *     uc_buffer_st::bytes[3] = 0xDD. \n
 *     and uc_buffer_st::length is set to 4.
 *     \n

 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *    ::UC_ERROR_INSUFFICIENT_BUFFER
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClient_GetSerialNumber(
        uc_buffer_st *pSN)
{
	return UC_ERROR_SUCCESS;
}


/**
 * Obtain the current Secure Chipset Serial Number
 *
 *This function can be called to obtain the secure chipset serial number. This secure chipset
 *serial number is associated with the device.
 *
 * @param[in,out] pCSSN Receives the CSSN assigned to the client.
 *     The application must set the uc_buffer_st::bytes member to point to a buffer in memory, and the uc_buffer_st::length
 *     member to the maximum length of the buffer, in bytes.  The function will modify the contents of the buffer
 *     to contain the CSSN in binary big-endian format, without any terminating character, and set the
 *     uc_buffer_st::length member to the actual length of data returned. If the uc_buffer_st::length member is not large
 *     enough to hold the entire CSSN, the uc_buffer_st::length member will be set to the minimum required
 *     amount, and the function will return ::UC_ERROR_INSUFFICIENT_BUFFER. The application can then allocate
 *     memory of a sufficient size and attempt to call the function again.
 *
 *     \note For simplicity, the application can allocate ::UC_BUFFER_GENERIC_SIZE bytes memory for uc_buffer_st::bytes
 *       and set uc_buffer_st::length to ::UC_BUFFER_GENERIC_SIZE. Cloaked CA Agent will set uc_buffer_st::length to the actual size.
 *     \n
 *     \note
 *     Suppose the CSSN is 4293844428 i.e. 0xFFEEDDCC, uc_buffer_st::bytes will have the following values: \n
 *     uc_buffer_st::bytes[0] = 0xFF, \n
 *     uc_buffer_st::bytes[1] = 0xEE, \n
 *     uc_buffer_st::bytes[2] = 0xDD, \n
 *     uc_buffer_st::bytes[3] = 0xCC. \n
 *     and uc_buffer_st::length is set to 4.
 *     \n
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
  * @retval
 *    ::UC_ERROR_INSUFFICIENT_BUFFER
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClient_GetCSSN(
        uc_buffer_st *pCSSN)
{
	return UC_ERROR_SUCCESS;
}

/**
 * Obtain a string to describe the ID of current client.
 *
 * This function can be called to obtain the client ID string.
 * The client ID string is supposed to be shown in the error banner, so that the viewer can read the identifiers directly from the error banner.
 * If current client has been initialized with a unique address, the string will look like "xxxxxxxxxx(y)". xxxxxxxxxx will be the unique address in decimal format.
 * If current client is a virgin one, the string will look like "xxxxxxxxxx(y)-z". xxxxxxxxxx will be CSSN in decimal format and z will be the Lock ID in decimal format.
 * y is the check digit for xxxxxxxxxx.\n
 * An example string containing a unique address:0000004097(2) \n
 * An example string containing a CSSN and Lock ID:0000004097(2)-1 \n
 *
 * \note The information from this method must be displayed in the error banner dialog.
 *
 * @param[in,out] pClientIDString Receives the Client ID string assigned to the client.
 *     The application must set the uc_buffer_st::bytes member to point to a buffer in memory, and the uc_buffer_st::length
 *     member to the maximum length of the buffer, in bytes.  The function will modify the contents of the buffer
 *     to contain the Client ID string, including NULL terminator, and set the uc_buffer_st::length member to the actual length of data returned.
 *     If the uc_buffer_st::length member is not large enough to hold the entire Client ID string, the uc_buffer_st::length member will be set to the minimum required
 *     amount, and the function will return ::UC_ERROR_INSUFFICIENT_BUFFER. The application can then allocate
 *     memory of a sufficient size and attempt to call the function again.
 *
 *     \note For simplicity, the application can allocate ::UC_BUFFER_GENERIC_SIZE bytes memory for uc_buffer_st::bytes
 *       and set uc_buffer_st::length to ::UC_BUFFER_GENERIC_SIZE. Cloaked CA Agent will set uc_buffer_st::length to the actual size,
 *       and this length include 1 byte terminator.
 *
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *    ::UC_ERROR_INSUFFICIENT_BUFFER
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClient_GetClientIDString(uc_buffer_st *pClientIDString)
{
	return UC_ERROR_SUCCESS;
}


/**
 * Obtain the current CA System ID
 *
 * This method can be called to obtain the current CA System ID. This value is typically used to
 * select the correct Irdeto CA stream when multiple streams exist from different CA vendors.
 *
 * \note The information from this method must be displayed in a GUI dialog.
 *
 * @param[out] pCASystemID Receives the current CA System ID. When Cloaked CA Agent support a range
 * of CA System IDs, the output of this method may vary: If Cloaked CA Agent has chosen a valid ID in
 * the range as its active ID, it returns the chosen ID, otherwise, it returns the start ID in its range.
 * If Cloaked CA Agent only supports a single ID, the result is the single one. See ::UniversalClient_GetCASystemIDRange
 * for getting the range of CA System IDs.
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClient_GetCASystemID(
        uc_uint32 * pCASystemID)
{
	return UC_ERROR_SUCCESS;
}

/**
 * Obtain the CA System ID range
 *
 * This method can be called to obtain the valid CA System ID range if Cloaked CA Agent supports
 * a range of CA System IDs. A valid CA System ID should be: (start ID <= valid ID <= end ID).If only
 * a single CA System ID is supported, this can be omitted.
 *
 *
 * @param[out] pCASystemIDStart Receives the start value of the CA System ID range.
 * @param[out] pCASystemIDEnd Receives the end value of the CA System ID range.
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClient_GetCASystemIDRange(
    uc_uint32 *pCASystemIDStart,
    uc_uint32 *pCASystemIDEnd)
{
	return UC_ERROR_SUCCESS;
}

/**
 * Obtain the Nationality
 *
 * This function can be called to obtain the nationality of the Cloaked CA Agent.
 * The nationality is described by three ISO 3166-1 Alpha-3 identifier (e.g. 'CHN', 'GBR', 'USA').
 *
 * \note The information from this method must be displayed in a GUI dialog.
 *
 * @param[in,out] pNationality Receives the nationality associated with the client.
 *     On output, the uc_buffer_st::bytes member will be filled with a NULL-terminated string.
 *
 *     The application must set the uc_buffer_st::bytes member to point to a buffer in memory, and the uc_buffer_st::length member
 *     to the maximum length of the buffer, in bytes.  The function will modify the contents of the buffer to contain the nationality
 *     , including NULL terminator, and set the uc_buffer_st::length member to the actual length of data returned.
 *     If the uc_buffer_st::length member is not large enough to hold the entire nationality, the uc_buffer_st::length member
 *     will be set to the minimum required amount, and the function will return ::UC_ERROR_INSUFFICIENT_BUFFER.
 *
 *     \note For simplicity, the application can allocate ::UC_BUFFER_GENERIC_SIZE bytes memory for uc_buffer_st::bytes
 *       and set uc_buffer_st::length to ::UC_BUFFER_GENERIC_SIZE. Cloaked CA Agent will set uc_buffer_st::length to the actual size.
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *    ::UC_ERROR_NO_NATIONALITY
 * @retval
 *    ::UC_ERROR_INSUFFICIENT_BUFFER
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClient_GetNationality(
        uc_buffer_st *pNationality)
{
	return UC_ERROR_SUCCESS;
}


/**
* Obtain Homing Channel data
*
* This function can be called to obtain Homing Channel data.
*
*
* @param[in,out] pHomingChannelData Receives Homing Channel data.
*     The application must set the uc_buffer_st::bytes member to point to a buffer in memory, and the uc_buffer_st::length member
*     to the maximum length of the buffer, in bytes.  The function will modify the contents of the buffer to contain the Homing Channel data,
*     in binary big-endian format, without any terminating character and set the uc_buffer_st::length member to the actual length of data returned.
*     If the uc_buffer_st::length member is not large enough to hold the entire Homing Channel data, the uc_buffer_st::length member
*     will be set to the minimum required amount, and the function will return ::UC_ERROR_INSUFFICIENT_BUFFER. The application
*     can then allocate memory of a sufficient size and attempt to call the function again.
*
*     \note For simplicity, the application can allocate ::UC_BUFFER_GENERIC_SIZE bytes memory for uc_buffer_st::bytes
*       and set uc_buffer_st::length to ::UC_BUFFER_GENERIC_SIZE. Cloaked CA Agent will set uc_buffer_st::length to the actual size.
*
*     @code
*     Format of the data(The bit order is bslbf):
*
*     Field Name      Length(bits)
*     DTH_IRD         1
*     ProfDec_IRD     2
*     Spare           4
*     Monitoring_IRD  1
*     Homing_Channel  96
*     Spare           152
*
*     In the above strucure, Homing_Channel is defined as:
*
*     Field Name            Length(bits)
*     Frequency             32
*     Orbital_Position      16
*     Symbol_Rate           28
*     Reserved              4
*     MuxVerifier_Mode      4
*     FEC_Inner             4
*     West_East_Flag        1
*     Polarisation          2
*     Roll off              2
*     Modulation System     1
*     Modulation Type       2
*
*     The detailed bit format of the above fields in Home_Channel structure is:
*
*     Field Name          Value                     Description
*     Frequency           DD D.D DD DD              where D = BCD digit
*     Orbital Pos         DD D.D                    where D = BCD digit
*     Symbol Rate         DD D.D DD D               where D = BCD digit
*     Mux Verifier        0                         Mux No Allowed
*                         1                         Mux Early
*                         2                         Mux Late
*                         3                         Mux Middle
*     FEC lnner           1                         1/2
*                         2                         2/3
*                         3                         3/4
*                         4                         5/6
*                         5                         7/8
*     West/East           0                         West
*                         1                         East
*     Polarisation        0                         Lin Horizontal
*                         1                         Lin Vertical
*                         2                         Cir Left
*                         3                         Cir Right
*     Modulation system   0                         DVB-S
*                         1                         DVB-S2
*     Roll off            0                         a=0.35
*                         1                         a=0.25
*                         2                         a=0.20
*                         3                         Reserved
*     Modulation type
*     (DVB-S)             0                         Reserved
*                         1                         QPSK
*                         2                         8PSK
*                         3                         16-QAM
*     Modulation type
*     (DVB-S2)            0                         Reserved
*                         1                         QPSK
*                         2                         8PSK
*                         3                         Reserved
*
*     Below is an example of a 32 Byte Homeing Channel Data:
*
*     DTH IRD with PAS4 settings
*     Byte             Value            Description
*     [0]              0x80             DTH IRD
*     [1]              0x01             Frequency = 12.5175
*     [2]              0x25
*     [3]              0x17
*     [4]              0x50
*     [5]              0x06             Orbital Pos = 68.5 deg
*     [6]              0x85
*     [7]              0x02             Symbol rate = 21.850
*     [8]              0x18
*     [9]              0x50
*     [10]             0x00
*     [11]             0x04             FEC 5/6
*     [12]             0x95             East, DVB-S2, a=0.20  Horizontal, QPSK
*     [13]-[31]        0x00             Spare
*     @endcode
*
* @retval
*    ::UC_ERROR_SUCCESS
* @retval
*    ::UC_ERROR_NULL_PARAM
* @retval
*    ::UC_ERROR_INSUFFICIENT_BUFFER
* @retval
*     Other The method may return any other errors listed in \ref result "Result Code List".
*
*/
uc_result UniversalClient_GetHomingChannelData(uc_buffer_st* pHomingChannelData)
{
	return UC_ERROR_SUCCESS;
}


/**
* Obtain Middleware user data
*
* This function can be called to obtain Middleware user data.
*
* @param[in,out] pMiddlewareUserData Receives Middleware user data.
*     The application must set the uc_buffer_st::bytes member to point to a buffer in memory, and the uc_buffer_st::length member
*     to the maximum length of the buffer, in bytes.  The function will modify the contents of the buffer to contain the Middleware user data,
*     in binary big-endian format, without any terminating character and set the uc_buffer_st::length member to the actual length of data returned.
*     If the uc_buffer_st::length member is not large enough to hold the entire Middleware user data, the uc_buffer_st::length member
*     will be set to the minimum required amount, and the function will return ::UC_ERROR_INSUFFICIENT_BUFFER. The application
*     can then allocate memory of a sufficient size and attempt to call the function again.
*
*     \note For simplicity, the application can allocate ::UC_BUFFER_GENERIC_SIZE bytes memory for uc_buffer_st::bytes
*       and set uc_buffer_st::length to ::UC_BUFFER_GENERIC_SIZE. Cloaked CA Agent will set uc_buffer_st::length to the actual size.
*
*
* @retval
*    ::UC_ERROR_SUCCESS
* @retval
*    ::UC_ERROR_NULL_PARAM
* @retval
*    ::UC_ERROR_INSUFFICIENT_BUFFER
* @retval
*     Other The method may return any other errors listed in \ref result "Result Code List".
*
*/

uc_result UniversalClient_GetMiddlewareUserData(uc_buffer_st * pMiddlewareUserData)
{
	return UC_ERROR_SUCCESS;
}


/**
 * Obtains the list of current products.
 *
 * This function can be used to obtain a list of products.  Each product consists of an ID and
 * an entitlement state.  This function could be used for example to display a different image or
 * color in a program guide for programs associated with these products.
 *
 * \note The information from this method must be displayed in a GUI dialog.
 *
 * @param[out] pProductCount If the function succeeds, this receives the number of
 *     elements returned in the ppProductList array.
 * @param[out] ppProductList If the function succeeds, this receives a pointer to a function-allocated
 *     array of ::uc_product_status structures.  When the caller no longer needs the information in the
 *     array, it must call ::UniversalClient_FreeProductList to free the array.
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClient_GetProductList(uc_uint32 *pProductCount, uc_product_status **ppProductList)
{
	return UC_ERROR_SUCCESS;
}

/**
 * Free resources allocated in ::UniversalClient_GetProductList.
 *
 * This function will free the memory allocated by a previous call to ::UniversalClient_GetProductList.
 *
 * @param[in,out] ppProductList On input, this is the address of the pointer received in a previous call to ::UniversalClient_GetProductList.
 *       On output, the function sets this to NULL.
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClient_FreeProductList(uc_product_status **ppProductList)
{
	return UC_ERROR_SUCCESS;
}

/**
 * Obtain the count of ECM and EMM sections processed by Cloaked CA Agent.
 *
 * This function can be called to obtain count of CA sections that have been processed by Cloaked CA Agent.
 *
 * \note The information from this method must be displayed in a GUI dialog.
 *
 * @param[out] pCaSectionCount If the function succeeds, this receives the structure of
 *     ::uc_ca_section_count structures contained the count of ECM and EMM processed by Cloaked CA Agent.
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClient_GetEcmEmmCount(uc_ca_section_count *pCaSectionCount)
{
	return UC_ERROR_SUCCESS;
}

/**
 * Notify OOB EMM sections to the Client
 *
 * This function is intended to notify the client with oob emm sections.
 *
  *
  * @param[in] pOobEmmSection Buffer containing the raw content of at least one oob emm sections.
  *     The uc_buffer_st::bytes member must point to a buffer containing the section data, and
  *     the uc_buffer_st::length must be set to the number of bytes in the buffer.
  *
  * @retval
  *    ::UC_ERROR_SUCCESS
  * @retval
  *    ::UC_ERROR_OUT_OF_MEMORY
  * @retval
  *    ::UC_ERROR_NULL_PARAM
  * @retval
  *     Other The method may return any other errors listed in \ref result "Result Code List".
  */
uc_result UniversalClient_SendOobEmmSection(const uc_buffer_st * pOobEmmSection)
{
	return UC_ERROR_SUCCESS;
}


/**
 * This function will free the generic memory allocated by Cloaked CA Agent.
 *
 * @param[in,out] pMemoryData On input, the uc_buffer_st::bytes member is
 *       the address holding a buffer, and the uc_buffer_st::length member is the number of bytes in the buffer.
 *       On output, the function frees the memeory, sets uc_buffer_st::bytes member to NULL,
 *       and sets uc_buffer_st::bytes member to 0.
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */

uc_result UniversalClient_FreeBuffer(
    uc_buffer_st *pMemoryData )
{
	return UC_ERROR_SUCCESS;
}


/**
 * Clear the CA data on Persistent Storage.
 *
 * This function will clear all CA data on Persistent Storage. All CA data will be lost.
 * This function shall be called when the user wants to reset its device to factory state.
 * After the data is cleared, ::UC_GLOBAL_NOTIFY_CA_DATA_CLEARED will be sent to device application
 * as a notification.
 *
 * \note This function is dangerous! Device application or the middleware shall make sure that it's
 * never triggered by accidental user actions.
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_ALREADY_STOPPED
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result  UniversalClient_ResetCAData(void)
{
	return UC_ERROR_SUCCESS;
}


/**
 * Obtain the current version of Secure Core.
 *
 * This function can be called to obtain the current version of Secure Core. This version can be used by applications to
 * check for supported features and application compatibility.  An application might also display a status screen with
 * version information on it that can be used for technical support purposes.
 *
 * \note The information from this method must be displayed in a GUI dialog.
 *
 * @param[in,out] pVersion Receives the version string associated with Secure Core.
 *     The application must set the uc_buffer_st::bytes member to point to a buffer in memory, and the uc_buffer_st::length member
 *     to the maximum length of the buffer, in bytes.  The function will modify the contents of the buffer to contain the version
 *     string, including NULL terminator, and set the uc_buffer_st::length member to the actual length of data returned.
 *     If the uc_buffer_st::length member is not large enough to hold the entire version string, the uc_buffer_st::length member
 *     will be set to the minimum required amount, and the function will return ::UC_ERROR_INSUFFICIENT_BUFFER. The application
 *     can then allocate memory of a sufficient size and attempt to call the function again.
 *
 *     \note For simplicity, the application can allocate ::UC_BUFFER_GENERIC_SIZE bytes memory for uc_buffer_st::bytes
 *       and set uc_buffer_st::length to ::UC_BUFFER_GENERIC_SIZE. Cloaked CA Agent will set uc_buffer_st::length to the actual size,
 *       and the length includes 1 byte NULL terminator.
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *    ::UC_ERROR_INSUFFICIENT_BUFFER
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClient_GetSecureCoreVersion( uc_buffer_st *pVersion)
{
	return UC_ERROR_SUCCESS;
}


/** @} */ /* End of groups */


/** @defgroup tmsapi Cloaked CA Agent TMS APIs
 *  Cloaked CA Agent TMS APIs
 *
 *  These API methods provide interfaces needed by Targeted Messaging System (TMS).
 *  TMS handles text and fingerprint messages to target specific groups of subscribers according to attributes,
 *  which can be dynamically generated and set. These specific groups are identified by attribute settings, set by CA server.
 *
 *  @{
 */


/**
* Set current service ID, transport ID and network ID for TMS
*
* This method is designed to set current tuned service information for TMS. The tuned service information
* is represented by the uc_dvb_tuned_info structure that contains serviceID, transportID and networkID. See ::uc_dvb_tuned_info.
*
* If TMS is activated, it must be called when any one of service ID, transport ID and networkID is changed.
* ServiceID/TransportID/NetworkID is one of the ways to set TMS data.
*
* \note If the client device application supports ONLY 1 descramble service, there is no need to invoke ::UniversalClient_SetTunedInfoList.
* ::UniversalClient_SetTunedInfo is designed to support 1 descramble service ONLY, and ::UniversalClient_SetTunedInfoList is designed to support multiple descramble services.
* Each time this function is called, Univesal Client will clean up the existing tuned service info set via ::UniversalClient_SetTunedInfo or ::UniversalClient_SetTunedInfoList.
*
* @param[in] serviceID current service ID
*
* @param[in] transportID current tansport ID
*
* @param[in] networkID current network ID
*
* @retval
*    ::UC_ERROR_SUCCESS
* @retval
*     Other The method may return any other errors listed in \ref result "Result Code List".
*
*/
uc_result UniversalClient_SetTunedInfo(uc_uint16 serviceID, uc_uint16 transportID, uc_uint16 networkID)
{
	return UC_ERROR_SUCCESS;
}

/**
* Set a list of tuned services for TMS to Cloaked CA Agent.
*
* This method is designed to set a list of tuned services for TMS.
* If TMS is activated, it must be called when the tuned information is changed.
*
* \note This function is similar to ::UniversalClient_SetTunedInfo, but this function can set a list of tuned services to Cloaked CA Agent.
* Each time this function is called, Univesal Client will clean up the existing tuned service info set via ::UniversalClient_SetTunedInfo or ::UniversalClient_SetTunedInfoList.
* When ::tunedInfoCount is 1, this function acts like ::UniversalClient_SetTunedInfo.
*
* @param[in] tunedInfoCount The number of elements in the pTunedInfoList. If the value of tunedInfoCount is 0,
*        Cloaked CA Agent will clean up the existing tuned information and return directly.
*
* @param[in] pTunedInfoList Pointer to a ::uc_dvb_tuned_info array containing ::tunedInfoCount items.
*
* @retval
*    ::UC_ERROR_SUCCESS
* @retval
*     Other The method may return any other errors listed in \ref result "Result Code List".
*
*/
uc_result UniversalClient_SetTunedInfoList(uc_uint32 tunedInfoCount, uc_dvb_tuned_info* pTunedInfoList)
{
	return UC_ERROR_SUCCESS;
}


/**
* Obtain TMS data
*
* This function can be called to obtain TMS data.
*
* \note The information from this method must be displayed in a GUI dialog. TMS data should be displayed in a hexadecimal format.
* TMS data from this method is for display purpose only.
*
*
* @param[in,out] pTmsData Receives TMS data.
*     The application must set the uc_buffer_st::bytes member to point to a buffer in memory, and the uc_buffer_st::length member
*     to the maximum length of the buffer, in bytes.  The function will modify the contents of the buffer to contain the TMS data,
*     in binary big-endian format, without any terminating character and set the uc_buffer_st::length member to the actual length of data returned.
*     If the uc_buffer_st::length member is not large enough to hold the entire TMS data, the uc_buffer_st::length member
*     will be set to the minimum required amount, and the function will return ::UC_ERROR_INSUFFICIENT_BUFFER. The application
*     can then allocate memory of a sufficient size and attempt to call the function again.
*
*     \note For simplicity, the application can allocate ::UC_BUFFER_GENERIC_SIZE bytes memory for uc_buffer_st::bytes
*       and set uc_buffer_st::length to ::UC_BUFFER_GENERIC_SIZE. Cloaked CA Agent will set uc_buffer_st::length to the actual size.
*
*
* @retval
*    ::UC_ERROR_SUCCESS
* @retval
*    ::UC_ERROR_NULL_PARAM
* @retval
*    ::UC_ERROR_INSUFFICIENT_BUFFER
* @retval
*     Other The method may return any other errors listed in \ref result "Result Code List".
*
*/
uc_result UniversalClient_GetTmsData(uc_buffer_st* pTmsData)
{
	return UC_ERROR_SUCCESS;
}

/**
* Set TMS attributes.
*
* The function shall be called when middleware wants to set client-based tms attributes.
*
*
* @param[in] attributeCount Indicate the count of client-based TMS attributes in ::pTmsAttributesItemInfoList.
*
* @param[in] pTmsAttributesItemInfoList Pointer to an array of ::uc_tms_attribute_item_info items.
*     The device application is responsible for maintaining this buffer.
*
*
* @retval
*    ::UC_ERROR_SUCCESS
* @retval
*    ::UC_ERROR_NULL_PARAM
* @retval
*     Other The method may return any other errors listed in \ref result "Result Code List".
*
*/
uc_result UniversalClient_SetTMSAttributes(uc_uint16 attributeCount,  uc_tms_attribute_item_info* pTmsAttributesItemInfoList)
{
	return UC_ERROR_SUCCESS;
}


/** @} */ /* End of groups */


/** @defgroup copycontrolapis Cloaked CA Agent Copy Control APIs
 *  Cloaked CA Agent Copy Control APIs
 *
 *  These APIs are for copy control functionality.
 *  In order to achieve copy control functionality, \ref copycontrolspi "Cloaked CA Agent Copy Control SPIs" must be implemented.
 *
 *  @{
 */

/**
  * Enable/Disable Macrovision
  *
  * \note This API is abandoned. Cloaked CA Agent will check the SPI interfaces to decide whether the client device supports Macrovision.
  * If the Macrovision SPI interfaces in \ref copycontrolspi "Cloaked CA Agent Copy Control SPIs" are implemented, Cloaked CA Agent will treat the client device
  * as Macrovision supported.
  *
  * @param[in] enable Set to ::UC_TRUE if the platform supports Macrovision, else ::UC_FALSE.
  *
  * @retval
  *    ::UC_ERROR_SUCCESS
  * @retval
  *     Other The method may return any other errors listed in \ref result "Result Code List".
  */
uc_result UniversalClient_EnableMacrovision(uc_bool enable)
{
	return UC_ERROR_SUCCESS;
}

/** @} */ /* End of groups */


/** @defgroup regionalfilteringapis Cloaked CA Agent Regional Filtering APIs
 *  Cloaked CA Agent Regional Filtering APIs
 *
 *  Regional Filtering is not a mandatory feature, but if the operator requires the client devices to implement it, then it is a must.
 *
 *  These APIs are for Regional Filtering.
 *  Regional Filtering is a feature to lock the secure client in a specific operational region.
 *  The region info is played out in certain service information tables, and also downloaded into the secure client.
 *  Cloaked CA Agent checks before descrambling whether the region info of the service and that for the secure client match,
 *  unless Cloaked CA Agent or secure client is configured to ignore the checking.
 *
 *  @{
 */

/**
 * Enable or disable regional filtering
 *
 * This function can be used to enable or disable regional filtering from the application level.
 * If regional filtering is enabled, devices are prevented from descrambling services in
 * regions where they are not blocked.
 *
 * By default, regional filtering is disabled.  The client does not remember the state
 * of regional filtering between restarts, so this function must be called at each initialization time.
 *
 * @param[in] enable Indicates whether to enable or disable regional filtering. If this value is ::UC_TRUE,
 *        Regional Filtering will be enabled. Otherwise, it will be disabled.
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClient_EnableRegionalFiltering(uc_bool enable)
{
	return UC_ERROR_SUCCESS;
}


/**
 * Notify the client of a new regional filtering descriptor
 *
 * This function is intended to notify a client of a new IA regional filtering descriptor.
 * A 'IA regional filtering descriptor' is a block of data that is intended to notify a device
 * of which geographical region it is in. If regional filtering is enabled, this information
 * is later used to determine if a client is entitled to descramble services in this region.
 * A client is entitled to descramble services in a region if it has received an appropriate
 * 'regional filtering EMM' via an open EMM service.
 *
 * In a typical \ref glossary_dvb "DVB" system, regional filtering descriptors are included
 * as part of the 'CA descriptor' field within the \ref glossary_cat "CAT". In unusual configurations
 * or in other transports, the regional filtering may take different forms.
 *
 * \note Regional filtering does not apply until it is enabled by calling ::UniversalClient_EnableRegionalFiltering.
 *
 * The format of the regional filtering descriptor is as follows
 *
 \code
    Regional_Filtering_Descriptor()
    {
        descriptor_tag = 0xA0                       (8 bits)
        descriptor_length                           (8 bits)

        IA_Regional_Filtering_Descriptor()
        {
            descriptor_tag = 0xA0                   (8 bits)
            descriptor_version                      (8 bits)
            sector_count                            (8 bits)
            for(i=0;i<sector_count;I++)
            {
                sector_index                        (8 bits)
                region_info                         (8 bits)
                sub_region_info                     (8 bits)
            }
        }
    }
 \endcode
 *
 * \note IA_Regional_Filtering_Descriptor() is different from Regional_Filtering_Descriptor().
 *
    Explanation of the fields:
    - \b descriptor_tag: The 8-bit tag value for this descriptor will always be set to 0xA0.
    - \b descriptor_length:  The length of the descriptor in bytes.
    - \b descriptor_tag: The 8-bit tag value for this descriptor will always be set to 0xA0.
    - \b descriptor_version:  The 8-bit version value should be different for different descriptor.
    - \b sector_count: The 8-bit count value should be the total sector count in this RF descriptor.
    - \b sector_index:  The 8-bit sector index value should be the index num of the sector.
    - \b region_info: The 8-bit region value should be the region of the sector.
    - \b sub_region_info: The 8-bit sub region value should be the sub region of the sector.
  *
  * @param[in] pDescriptor Buffer containing the raw contents of the IA_Regional_Filtering_Descriptor.
  *     The uc_buffer_st::bytes member must point to a buffer containing the descriptor, and
  *     the uc_buffer_st::length must be set to the number of bytes in the buffer.
  *
  * @retval
  *    ::UC_ERROR_SUCCESS
  * @retval
  *    ::UC_ERROR_OUT_OF_MEMORY
  * @retval
  *    ::UC_ERROR_NULL_PARAM
  * @retval
  *     Other The method may return any other errors listed in \ref result "Result Code List".
  */
uc_result UniversalClient_NotifyRegionalFilteringDescriptor(
     const uc_buffer_st *pDescriptor)
{
	return UC_ERROR_SUCCESS;
}

/**
 * Obtains the list of Region Information
 *
 * This function can be called to obtain the list of region and subregion indexed by sector number.
 *
 * \note The information from this method must be displayed in a GUI dialog.
 *
 * @param[out] pRegioninfoCount If the function succeeds, this receives the number of
 *     elements returned in the ppRegioninfoList array.
 * @param[out] ppRegioninfoList If the function succeeds, this receives a pointer to a function-allocated
 *     array of ::uc_region_information structures.  When the caller no longer needs the information in the
 *     array, it must call ::UniversalClient_FreeRegionInfoList to free the array.
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *    ::UC_ERROR_MISSING_DATA
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClient_GetRegionInfoList(uc_uint32 *pRegioninfoCount, uc_region_information **ppRegioninfoList)
{
	return UC_ERROR_SUCCESS;
}

/**
 * Free resources allocated in ::UniversalClient_GetRegionInfoList.
 *
 * This function will free the memory allocated by a previous call to ::UniversalClient_GetRegionInfoList.
 *
 * @param[in,out] ppRegioninfoList On input, this is the address of the pointer received in a previous call to ::UniversalClient_GetRegionInfoList.
 *       On output, the function sets this to NULL.
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClient_FreeRegionInfoList(uc_region_information **ppRegioninfoList)
{
	return UC_ERROR_SUCCESS;
}

/** @} */ /* End of groups */



/** @defgroup testingapi Cloaked CA Agent Testing APIs
 *  Cloaked CA Agent Testing APIs
 *
 *  These API methods provide the necessary basic functionality to test secure chip with STB file from SCPC,
 *  and generate \ref glossary_tsc "TSC" CHIP record needed by SCPC to create upload files for this device.
 *
 *  Testing APIs are ONLY used on the product stage. They can be ignored in the trial stage.
 *
 *  Please refer to \ref usestestingkeys "Use device testing APIs" on how to use these APIs.
 *
 *  @{
 */

/**
 * Max length of a \ref glossary_tsc "TSC" CHIP record.
 * Used by ::UniversalClient_CreateTscChipRecord
 */
#define SIZE_MAX_TSC_CHIP_RECORD (4096)


/**
 * Set testing CSSK and CW.
 *
 * This method is designed to test secure chip key ladder.It shall ONLY be called at verification phase.
 * When this method is called, Cloaked CA Agent will enter Test Mode.
 * In Test Mode, any incoming ECM section will trigger the call to ::UniversalClientSPI_Device_SetCSSK and ::UniversalClientSPI_Stream_SetDescramblingKey.
 * with the CSSK and CW from this method.
 *
 * Irdeto will provide CW and CSSK together with a corresponding bitstream.
 *
 * This method will just cache the CSSK and CW in a internal buffer and then return.
 *
 *
 * @param[in] pCssk CSSK key material to the device. This is normally protected by a secure chipset unique key
 *     (CSUK) that resides within the secure chipset such that an attacker gains no advantage by intercepting this method.
 *     The uc_buffer_st::bytes member points to a buffer containing the raw key material, and the uc_buffer_st::length member contains
 *     the number of bytes in the buffer.
 * @param[in] pEvenDescramblingKeyInfo Information about the even key being used to descramble content. This contains key algorithm, protection,
 *     cryptography context, and the key material. See ::uc_key_info.
 * @param[in] pOddDescramblingKeyInfo Information about the odd key being used to descramble content. This contains key algorithm, protection,
 *     cryptography context, and the key material. See ::uc_key_info.
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *    ::UC_ERROR_INVALID_HANDLE
 * @retval
 *    ::UC_ERROR_WRONG_HANDLE_TYPE
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClient_SetTestingKey(
    const uc_buffer_st *pCssk,
    const uc_key_info *pEvenDescramblingKeyInfo,
    const uc_key_info *pOddDescramblingKeyInfo
    )
{
	return UC_ERROR_SUCCESS;
}

/**
 * Set testing CSSK and CW with key infomation to support multi key ladder.
 *
 * This is the extension of ::UniversalClient_SetTestingKey function with additional key protection infomation provided,
 * in order to support differents key ladder.It shall ONLY be called at verification phase.
 * When this method is called, Cloaked CA Agent will enter Test Mode.Any incoming ECM section will trigger the call to
 * ::UniversalClientSPI_Device_SetCSSK and ::UniversalClientSPI_Stream_SetDescramblingKey.
 *
 * Irdeto will provide CW, CSSK, their key protection info together with a corresponding bitstream.
 *
 * This method will just cache the CSSK, CW, their key protection info in a internal buffer and then return.
 *
 *
 * @param[in] pCSSKInfo Information about the CSSK being used to protect the descrambling key. The CSSK is also protected by CSUK.
 *     This contain CSSK protection and CSSK material.See ::uc_cssk_info.
 * @param[in] pEvenDescramblingKeyInfo Information about the even key being used to descramble content. This contains key algorithm, protection,
 *     cryptography context, and the key material. See ::uc_key_info.
 * @param[in] pOddDescramblingKeyInfo Information about the odd key being used to descramble content. This contains key algorithm, protection,
 *     cryptography context, and the key material. See ::uc_key_info.
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *    ::UC_ERROR_INVALID_HANDLE
 * @retval
 *    ::UC_ERROR_WRONG_HANDLE_TYPE
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClient_SetExtendedTestingKey(
    const uc_cssk_info * pCSSKInfo,
    const uc_key_info *pEvenDescramblingKeyInfo,
    const uc_key_info *pOddDescramblingKeyInfo
    )
{
	return UC_ERROR_SUCCESS;
}

/**
 * Request Cloaked CA Agent to reload personalized data from Persistent Storage.
 *
 * This method can be used to let Cloaked CA Agent reload the personalized data by calling
 * ::UniversalClientSPI_Device_GetPersonalizedData.
 *
 * \note This API is not used anymore.
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClient_ReloadPersonalizedData(void)
{
	return UC_ERROR_SUCCESS;
}


/**
 * Create a tsc CHIP record to describe current device.
 *
 * This method will generate a \ref glossary_tsc "TSC" CHIP record.
 * Irdeto SCPC is expected to receive the \ref glossary_tsc "TSC" CHIP record in order to generate upload file for this device.
 * The call to this method will lead to the invoke to the following SPIs.
 *     -# ::UniversalClientSPI_Device_GetCSSN
 *     -# ::UniversalClientSPI_Device_GetSecurityState
 *     -# ::UniversalClientSPI_Device_GetPlatformIdentifiers
 *     -# ::UniversalClientSPI_Device_GetPrivateData
 *
 *
 * @param[in,out] tscChipRecord On input, Cloaked CA Agent will ZERO the incoming buffer.
 *     On output, it will be filled with a NULL-ended ASCII string with a max length ::SIZE_MAX_TSC_CHIP_RECORD,
 *     suitable for being inserted into the relevant CHIP entry of the \ref glossary_tsc "TSC File".
 *
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *    ::UC_ERROR_INVALID_HANDLE
 * @retval
 *    ::UC_ERROR_WRONG_HANDLE_TYPE
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClient_CreateTscChipRecord(
    uc_char tscChipRecord[SIZE_MAX_TSC_CHIP_RECORD]
    )
{
	return UC_ERROR_SUCCESS;
}

/** @} */ /* End of groups */


/** @defgroup ppvvodapis Cloaked CA Agent PPV VOD APIs
 *  Cloaked CA Agent PPV VOD APIs
 *
 *  These API methods provide the necessary functionality for \ref glossary_ppv_vod "PPV VOD".
 *
 *  In this solution, Cloaked CA Agent will generate the Client Transaction Data at the start of VOD session via ::UniversalClient_PPV_VOD_PrepareRequest.
 *  The application should submit the Client Transaction Data to the CA server and send the response from CA server to Cloaked CA Agent via ::UniversalClient_SendOobEmmSection.
 *  VOD content is regarded as a normal scrambled service. If the client is entitled to view the VOD content, control words will be output to the SPI via ::UniversalClientSPI_Stream_SetDescramblingKey
 *  as normal.
 *
 *  To cancel a VOD request, the client device application can call ::UniversalClient_PPV_VOD_CancelRequest with the nonce returned from ::UniversalClient_PPV_VOD_PrepareRequest.
 *
 *  Refer to \ref ppv_vod_client_communication "PPV VOD Flow" to get an
 *  overview on how to play a \ref glossary_ppv_vod "PPV VOD" service.
 *
 *  For more details for integration code, please refer to \ref playppvvodservie "User plays PPV VOD service".
 *
 *  \note Cloaked CA Agent ONLY provides \ref glossary_ppv_vod "PPV VOD" feature for platform with secure chipset support.
 *
 *  @{
 */

/**
 * Request the Client Transaction Data at the start of \ref glossary_ppv_vod "PPV VOD" session.
 *
 * The client device application can call this method to get the Client Transaction Data from Cloaked CA Agent.
 * The Client Transaction Data is generated by Cloaked CA Agent in response to a VOD request.
 *
 * Refer to \ref ppv_vod_client_communication "PPV VOD Flow" to get an
 * overview on how to play a \ref glossary_ppv_vod "PPV VOD" service.
 *
 * The Client Transaction Data is organized with the following TLV format.
 * The application can parse the TLV format to get related information, including the VOD nonce,
 * the client Unique Address, client version, etc.
 *
 \code
Client_Transaction_Data()
{
    Field Name                      Field Length     Field Value

    tag_number                      8   bits         The total number of the tags
    length                          16  bits         The total byte length of the tags
    for(i=0;i<tag_number;i++)                        Loop tag_number times for the tags
    {
        tag                         8   bits         Tag. This can be UC_TAG_TYPE_VOD_NONCE, UC_TAG_TYPE_UNIQUE_ADDRESS and UC_TAG_TYPE_CLIENT_VERSION currently.
        length(N)                   8   bits         Tag length
        value                       8*N bits         Tag value
    }
    HMAC                            64  bits         The HMAC value of the whole parameters.
                                                     The application do not need to care about it.
}
 \endcode

 * \note For above tags, please refer to \ref vodtlvtags "Cloaked CA Agent TLV Tags for PPV VOD" for a list of TVL tags and its values.
 *
 * @param[out] pTransactionData Buffer containing the client transaction data.
 *     If the function succeeds, the uc_buffer_st::bytes member points to a function-allocated buffer containing a Client_Transaction_Data().
 *     The uc_buffer_st::length member must be set to the number of bytes in the buffer.
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_VOD_CLIENT_TYPE_ERROR
 * @retval
 *    ::UC_ERROR_VOD_NO_SERIAL_NUMBER
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 *
 * \note When the caller no longer needs the information, it must call ::UniversalClient_FreeBuffer
         to free the uc_buffer_st::bytes member.
 *
 */
uc_result UniversalClient_PPV_VOD_PrepareRequest(
    uc_buffer_st *pTransactionData )
{
	return UC_ERROR_SUCCESS;
}

/**
 * Cancel a \ref glossary_ppv_vod "PPV VOD" request to the client.
 *
 * The client device application can call this method to cancel a request to the client with a VOD nonce data.
 *
 * Refer to \ref ppv_vod_client_communication "PPV VOD Flow" to get an
 * overview on how to play a \ref glossary_ppv_vod "PPV VOD" service.
 *
 * @param[in] pNonceData Buffer containing the nonce data.
 *     The uc_buffer_st::bytes member must point to a buffer containing the nonce data,
 *     which is the VOD nonce previously retrieved by a call to ::UniversalClient_PPV_VOD_PrepareRequest.
 *     The uc_buffer_st::length member must be set to the number of bytes in the buffer.
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_VOD_INVALID_NONCE
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClient_PPV_VOD_CancelRequest(
    const uc_buffer_st *pNonceData )
{
	return UC_ERROR_SUCCESS;
}


/** @} */ /* End of groups */

/** @} */ /* End of Univeral Client APIs */


/** @defgroup convergentclient_api Unified Client Specific APIs

 *  Unified Client APIs
 *
 *  These API methods provide the necessary interfaces for the Unified Client which includes the functionality
 *  of Cloaked CA Agent and Softcell. When Unified Client is integrated, the following APIs shall be used in replace
 *  of the corresponding Universal Client APIs that are only for Cloaked CA Agent. However, not all APIs have its Unified
 *  Client specific version. Some of them can be used without caring whether Cloaked CA Agent or Unified Client is integrated.
 *  , such as ::UniversalClient_GetCSSN.
 *  @{
 */


/**
 * Obtain the current version of Softcell.
 *
 * This function can be called to obtain the current version of Softcell. This version can be used by applications to
 * check for supported features and application compatibility.  An application might also display a status screen with
 * version information on it that can be used for technical support purposes.
 *
 * \note The information from this method must be displayed in a GUI dialog.
 *
 * @param[in,out] pVersion Receives the version string associated with Softcell.
 *     The application must set the uc_buffer_st::bytes member to point to a buffer in memory, and the uc_buffer_st::length member
 *     to the maximum length of the buffer, in bytes.  The function will modify the contents of the buffer to contain the version
 *     string, including NULL terminator, and set the uc_buffer_st::length member to the actual length of data returned.
 *     If the uc_buffer_st::length member is not large enough to hold the entire version string, the uc_buffer_st::length member
 *     will be set to the minimum required amount, and the function will return ::UC_ERROR_INSUFFICIENT_BUFFER. The application
 *     can then allocate memory of a sufficient size and attempt to call the function again.
 *
 *     \note For simplicity, the application can allocate ::UC_BUFFER_GENERIC_SIZE bytes memory for uc_buffer_st::bytes
 *       and set uc_buffer_st::length to ::UC_BUFFER_GENERIC_SIZE. Cloaked CA Agent will set uc_buffer_st::length to the actual size,
 *       and the length includes 1 byte NULL terminator.
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *    ::UC_ERROR_INSUFFICIENT_BUFFER
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClient_GetSoftcellVersion( uc_buffer_st *pVersion)
{
	return UC_ERROR_SUCCESS;
}


/**
 * Obtain the build information of Softcell
 *
 * This function can be called to obtain the build information of Softcell.
 * The application shall display the build info in the client status screen.
 *
 * \note The information from this method must be displayed in a GUI dialog.
 *
 * @param[in,out] pBuildInfo Receives the build string associated with the client.
 *     The application must set the uc_buffer_st::bytes member to point to a buffer in memory, and the uc_buffer_st::length member
 *     to the maximum length of the buffer, in bytes.  The function will modify the contents of the buffer to contain the build
 *     string, including NULL terminator, and set the uc_buffer_st::length member to the actual length of data returned.
 *     If the uc_buffer_st::length member is not large enough to hold the entire build string, the uc_buffer_st::length member
 *     will be set to the minimum required amount, and the function will return ::UC_ERROR_INSUFFICIENT_BUFFER. The application
 *     can then allocate memory of a sufficient size and attempt to call the function again.
 *
 *     \note For simplicity, the application can allocate ::UC_BUFFER_GENERIC_SIZE bytes memory for uc_buffer_st::bytes
 *       and set uc_buffer_st::length to ::UC_BUFFER_GENERIC_SIZE. Cloaked CA Agent will set uc_buffer_st::length to the actual size,
 *       and the length includes 1 byte NULL terminator.
 *
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *    ::UC_ERROR_INSUFFICIENT_BUFFER
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClient_GetSoftcellBuildInfo(
    uc_buffer_st *pBuildInfo)
{
	return UC_ERROR_SUCCESS;
}


/**
 * Obtain the status of smartcard.
 *
 * This function can be called to obtain smartcard status.
 *
 * \note The information from this method must be displayed in a GUI dialog.
 *
 * @param[in] smartcardID smartcard id. This parameter is not used.
 * @param[out] pSmartcardStatus If the function succeeds, the smartcard status information is copied in it.
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClient_GetSmartcardStatus(
    uc_uint32 smartcardID,
    uc_smartcard_status* pSmartcardStatus )
{
	return UC_ERROR_SUCCESS;
}


/**
 * Obtain the smartcard infomation.
 *
 * This function can be called to obtain smartcard infomation.
 *
 * \note The information from this method must be displayed in a GUI dialog.
 *
 * @param[in] smartcardID smartcard id. This parameter is not used.
 * @param[out] pSmartcardInfo If the function succeeds, the smartcard info is copied in it.
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClient_GetSmartcardInfo(
    uc_uint32 smartcardID,
    uc_smartcard_info* pSmartcardInfo )
{
	return UC_ERROR_SUCCESS;
}


/**
 * Obtain the CA System ID from smartcard.
 *
 * This method can be called to obtain the active CA sytem ID from the smartcard.
 *
 * @param[in] smartcardID smartcard id. This parameter is not used.
 * @param[out] pCaSystemID If the function succeeds, the active CA system ID in smartcard is copied in it.
 *
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClient_GetSmartcardCASystemID(
    uc_uint32 smartcardID,
    uc_uint32 *pCaSystemID)
{
	return UC_ERROR_SUCCESS;
}


/**
 * Obtain the nationality of smartcard.
 *
 * This function can be called to obtain the nationality of Smartcard.
 * The nationality is described by three ISO 3166-1 Alpha-3 identifier (e.g. 'CHN', 'GBR', 'USA').
 *
 * \note The information from this method must be displayed in a GUI dialog.
 *
 * @param[in] smartcardID smartcard id. This parameter is not used.
 * @param[in,out] pNationality Receives the nationality associated with the client.
 *     On output, the uc_buffer_st::bytes member will be filled with a NULL-terminated string.
 *
 *     The application must set the uc_buffer_st::bytes member to point to a buffer in memory, and the uc_buffer_st::length member
 *     to the maximum length of the buffer, in bytes.  The function will modify the contents of the buffer to contain the nationality
 *     , including NULL terminator, and set the uc_buffer_st::length member to the actual length of data returned.
 *     If the uc_buffer_st::length member is not large enough to hold the entire nationality, the uc_buffer_st::length member
 *     will be set to the minimum required amount, and the function will return ::UC_ERROR_INSUFFICIENT_BUFFER.
 *
 *     \note For simplicity, the application can allocate ::UC_BUFFER_GENERIC_SIZE bytes memory for uc_buffer_st::bytes
 *       and set uc_buffer_st::length to ::UC_BUFFER_GENERIC_SIZE. Cloaked CA Agent will set uc_buffer_st::length to the actual size.
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *    ::UC_ERROR_NO_NATIONALITY
 * @retval
 *    ::UC_ERROR_INSUFFICIENT_BUFFER
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClient_GetSmartcardNationality(
        uc_uint32 smartcardID,
        uc_buffer_st *pNationality)
{
	return UC_ERROR_SUCCESS;
}

/**
 * Obtain the TMS data of smartcard.
 *
 * This function can be called to obtain TMS data of Smartcard.
 *
 * @param[in] smartcardID smartcard id. This parameter is not used.
 * @param[in,out] pTMSData Receives the TMS data of smartcard.
 *
 *     The application must set the uc_buffer_st::bytes member to point to a buffer in memory, and the uc_buffer_st::length member
 *     to the maximum length of the buffer, in bytes.  The function will modify the contents of the buffer to contain the nationality
 *     , including NULL terminator, and set the uc_buffer_st::length member to the actual length of data returned.
 *     If the uc_buffer_st::length member is not large enough to hold the entire nationality, the uc_buffer_st::length member
 *     will be set to the minimum required amount, and the function will return ::UC_ERROR_INSUFFICIENT_BUFFER.
 *
 *     \note For simplicity, the application can allocate ::UC_BUFFER_GENERIC_SIZE bytes memory for uc_buffer_st::bytes
 *       and set uc_buffer_st::length to ::UC_BUFFER_GENERIC_SIZE. Cloaked CA Agent will set uc_buffer_st::length to the actual size.
 *
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *    ::UC_ERROR_INSUFFICIENT_BUFFER
 * @retval
 *    ::UC_ERROR_ALREADY_STOPPED
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClient_GetSmartcardTMSData(
    uc_uint32 smartcardID,
    uc_buffer_st *pTMSData)
{
	return UC_ERROR_SUCCESS;
}

/**
 * Obtain the unique address of smartcard.
 *
 * This function can be called to obtain smartcard unique address.
 *
 * \note The information from this method must be displayed in a GUI dialog.
 *
 * @param[in] smartcardID smartcard id. This parameter is not used.
 * @param[out] pSmartcardUA receives the smartcard Unique Address.
 *      The application must set the uc_buffer_st::bytes member to point to a buffer in memory, and the uc_buffer_st::length
 *      member to the maximum length of the buffer, in bytes. The function will modify the contents of the buffer
 *      to contain the Smartcard Unique Address in binary big-endian format, without any terminating character, and set the
 *      uc_buffer_st::length member to the actual length of data returned. If the uc_buffer_st::length member is not large enough
 *      to hold the entire Smartcard Unique Address, the uc_buffer_st::length member will be set to the minimum required value,
 *      and the function will return ::UC_ERROR_INSUFFICIENT_BUFFER. The application can then allocate memory of a sufficient size
 *      and attempt to call the function again.
 *
 *     \note For simplicity, the application can allocate ::UC_BUFFER_GENERIC_SIZE bytes memory for uc_buffer_st::bytes
 *       and set uc_buffer_st::length to ::UC_BUFFER_GENERIC_SIZE. Cloaked CA Agent will set uc_buffer_st::length to the actual size.
 *     \n
 *     \note
 *     Suppose the Smartcard Unique Address is 12345678 i.e. 0xBC614E, uc_buffer_st::bytes will have the following values: \n
 *     uc_buffer_st::bytes[0] = 0xBC, \n
 *     uc_buffer_st::bytes[1] = 0x61, \n
 *     uc_buffer_st::bytes[2] = 0x4E, \n
 *     and uc_buffer_st::length is set to 3.
 *     \n
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
  * @retval
 *    ::UC_ERROR_INSUFFICIENT_BUFFER
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClient_GetSmartcardUniqueAddress(
    uc_uint32 smartcardID,
    uc_buffer_st* pSmartcardUA )
{
	return UC_ERROR_SUCCESS;
}


/** @} */ /* End of Unified Client APIs */


