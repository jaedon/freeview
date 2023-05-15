/******************************************************************************
 
    Copyright
    This code is strictly confidential and the receiver is obliged to use it 
    exclusively for his or her own purposes. No part of Viaccess code may be 
    reproduced or transmitted in any form or by any means, electronic or 
    mechanical, including photocopying, recording, or by any information storage 
    and retrieval system, without permission in writing from Viaccess. 
    The information in this code is subject to change without notice. Viaccess 
    does not warrant that this code is error free. If you find any problems 
    with this code or wish to make comments, please report them to Viaccess.
 
    Trademarks 
    Viaccess is a registered trademark of Viaccess ® in France and/or other 
    countries. All other product and company names mentioned herein are the 
    trademarks of their respective owners.
    Viaccess may hold patents, patent applications, trademarks, copyrights 
    or other intellectual property rights over the code hereafter. Unless 
    expressly specified otherwise in a Viaccess written license agreement, the
    delivery of this code does not imply the concession of any license over 
    these patents, trademarks, copyrights or other intellectual property.
 
******************************************************************************/


#ifndef _VA_SC_H_
#define _VA_SC_H_

#include "va_types.h"
#include "va_errors.h"


/**
 @ingroup drivers
 */

/**
 @defgroup scDriver VA_SC Driver
 @brief This API is the interface for ACS to access Smartcard resources.
 @{
 */

/**
 @defgroup scConst VA_SC Constants
 @brief The smartcard driver constants

 The smartcard driver uses a list of constants. These are in the va_errors.h header file:
 - @ref kVA_OK
 - @ref kVA_INVALID_PARAMETER
 - @ref kVA_RESOURCE_BUSY
 - @ref kVA_UNEXPECTED_CALLBACK
 - @ref kVA_NOT_IMPLEMENTED
 - @ref kVA_RESOURCE_ALREADY_LOCKED
 - @ref kVA_RESOURCE_NOT_LOCKED
 
 @{
 @}
 */


 
/**
 @defgroup scEnum VA_SC Enumerations
 @brief The smartcard driver enumerations
 @{
 */

/** This enumeration specifies the smartcard slot state */
typedef enum
{
    eCARD_EXTRACTED = 0, /**< The smartcard is extracted. */
    eCARD_INSERTED,      /**< The smartcard is inserted but not reset. */
    eCARD_READY          /**< The smartcard is inserted and reset. A command can be sent. */

} tVA_SC_State;

/** @} */


/**
 @defgroup manSCFunc VA_SC Manufacturer Functions
 @brief The smartcard driver functions to be implemented by the STB manufacturer.
 @{
*/

/**
 This function resets a given smartcard.
 @param [in] dwScSlot Indicates the slot number of the smartcard. This value ranges from 0 to uiNbSmartCard--1.
 @param [in] uiPPSNegotiationValue This parameter is set with the PPS (Protocol Parameter Selection) negotiation
                                   value requested by the ACS.
                             <br> If this value is 0, the PPS negotiation must not take place.
                             <br> On the other hand, if the ATR (Answer To Reset) specifies that the card permits negotiation,
							 the smartcard driver proceeds with a PPS negotiation to request a change in the communication speed.
                             <br> The 4 MSB of uiPpsNegotiationValue indicates the factor F and the 4 LSB specifies the factor D,
							 similarly to the interface byte TA1 described in the ISO/IEC 7816-3 document.
                             <br> The STB manufacturer must fine tune the negotiated Fn value as close as possible to F (Fn < = F),
							 taking into account the limitations of the smartcard given by the ATR.
							 Likewise, the value Dn must be as close as possible to D (Dn < = D).
                             <br> Note that according to the ISO/IEC 7816-3 tables, it is the referenced values and
							 not the references that must be tuned.
							 For example, if the negotiation value D is 0x6 and the smartcard handles 0x1 and 0x8,
							 the negotiated value should be 0x8 (as the referenced value for 0x6 is greater than the referenced
							 value for 0x8).
 @retval kVA_OK                If the request is accepted.
 @retval kVA_INVALID_PARAMETER If the value of dwScSlot is out of range.
 @retval kVA_RESOURCE_BUSY     If the smartcard reader is already processing a reset command previously required by Viaccess.
 @retval kVA_INSERT_CARD_FIRST If no smartcard is present in the slot.

 @note Once the reset process has been successfully carried out, the STB calls the Viaccess function @ref VA_SC_ResetDone.
       <br> On the other hand, if an error occurs during the reset process, the STB calls @ref VA_SC_ResetFailed.
       <br> The STB must call the @ref VA_SC_ResetFailed function when the PPS exchange between the driver and the smartcard fails.
 
 For related information, refer to @ref VA_SC_ResetDone and @ref VA_SC_ResetFailed.
*/
INT VA_SC_Reset(DWORD dwScSlot, UINT8 uiPPSNegotiationValue);


/**
 This function retrieves information on the state of a given smartcard.
 @param [in] dwScSlot Indicates the slot number of the smartcard. This value ranges from 0 to uiNbSmartCard--1.
 @param [in] pScState Points to the address of a @ref tVA_SC_State enumeration.
                      The memory is allocated by Viaccess, but the enumeration is updated by the STB.
 @retval kVA_OK                If the request is correctly accepted.
 @retval kVA_INVALID_PARAMETER the dwScSlot value is out of range or if pScState is NULL.

 For related information, refer to @ref tVA_SC_State
*/
INT VA_SC_GetState(DWORD dwScSlot, tVA_SC_State *pScState);

/**
 This function sends a command to a given smartcard.
 @param [in] dwScSlot Indicates the slot number of the smartcard to be activated. This value runs from 0 to uiNbSmartCard--1.
 @param [in] uiCommandSize Specifies the size of the command and its associated data sent to the smartcard
 @param [in] pCommand Contains the command and associated data that is sent to the smartcard
 @retval kVA_OK If the request is accepted.
 @retval kVA_INVALID_PARAMETER If the dwScSlot value is out of range or if pCommand is NULL.
 @retval kVA_RESOURCE_BUSY     If the smartcard is already processing a Viaccess command.
 @retval kVA_INSERT_CARD_FIRST If no smartcard is inserted in the slot.
 @retval kVA_RESET_CARD_FIRST  If the smartcard is not reset.
 @note If the command is accepted and is being processed any subsequent @ref VA_SC_SendCommand request is answered
       with a kVA_RESOURCE_BUSY and is ignored.
       @n Once the command is successfully processed, the STB calls the @ref VA_SC_CommandDone function.
	   However, if the command was not processed, the STB calls the @ref VA_SC_CommandFailed function.
       <br> As soon as the @ref VA_SC_CommandDone or the @ref VA_SC_CommandFailed function returns,
	   the smartcard can receive a new @ref VA_SC_SendCommand request.
       <br> If an error occurred (kVA_INVALID_PARAMETER or kVA_RESOURCE_BUSY), the STB ignores the send command request. 
	   In this case, the Viaccess ACS does not expect any @ref VA_SC_CommandDone or @ref VA_SC_CommandFailed response.
       <br> Viaccess allocates, fills and frees the memory block pointed by pCommand. 
	   If the return value is kVA_OK, Viaccess waits for one of the two callback functions 
	   @ref VA_SC_CommandDone or @ref VA_SC_CommandFailed before freeing the allocated memory. 
	   In all other cases the memory is freed immediately.

 For related Viaccess functions, refer to @ref VA_SC_CommandDone and @ref VA_SC_CommandFailed
*/
INT VA_SC_SendCommand(DWORD dwScSlot, UINT32 uiCommandSize, BYTE *pCommand);


/**
 This function activates a smartcard. It is used for ISO compatibility tests only.
 @param [in] dwScSlot Indicates the slot number of the smartcard to be deactivated. This value ranges from 0 to uiNbSmartCard--1.
 @retval kVA_OK                If the request is accepted.
 @retval kVA_INVALID_PARAMETER If dwScSlot is out of range.
 @retval kVA_NOT_IMPLEMENTED   If the request is not supported by the STB smartcard driver.
 @retval kVA_INSERT_CARD_FIRST If no smartcard is inserted in the slot.
 @note The activation process must follow the recommendations documented in ISO/IEC 7816-3.
 
 For related information, refer to @ref VA_SC_Deactivate
*/
INT VA_SC_Activate(DWORD dwScSlot);


/**
 This function deactivates a smartcard. It is used for ISO compatibility tests only.
 @param [in] dwScSlot Indicates the slot number of the smartcard. This value ranges from 0 to uiNbSmartCard--1.
 @retval kVA_OK                If the request is accepted.
 @retval kVA_INVALID_PARAMETER If dwScSlot is out of range.
 @retval kVA_NOT_IMPLEMENTED   If the request is not supported by the STB smartcard driver.
 @retval kVA_INSERT_CARD_FIRST If no smartcard is inserted in the slot.
 @note The deactivation process must follow the recommendations documented in ISO/IEC 7816-3.

 For related information, refer to @ref VA_SC_Activate
*/
INT VA_SC_Deactivate(DWORD dwScSlot);


/**
 This function locks a smartcard reader.
 @param [in] dwScSlot Indicates the slot number of the smartcard to be locked. This value ranges from 0 to uiNbSmartCard--1.
 @retval kVA_OK                      If the request is accepted.
 @retval kVA_INVALID_PARAMETER       If dwScSlot is out of range.
 @retval kVA_RESOURCE_ALREADY_LOCKED If the smartcard reader is already locked by Viaccess or by a concurrent software.
 @retval kVA_INSERT_CARD_FIRST       If no smartcard is inserted in the slot.
 @note This function ensures that no other processes, like for example reset requests, gain access to the smartcard reader 
       while it is locked. This function is useful for processing a list of smartcard commands that must not be interrupted.

 For related information, refer to @ref VA_SC_Unlock
*/
INT VA_SC_Lock(DWORD dwScSlot);


/**
 This function unlocks a smartcard reader.
 @param [in] dwScSlot Indicates the slot number of the smartcard to be unlocked. This value ranges from 0 to uiNbSmartCard--1.
 @retval kVA_OK                  If the request is accepted.
 @retval kVA_INVALID_PARAMETER   If dwScSlot is out of range.
 @retval kVA_RESOURCE_NOT_LOCKED If the smartcard reader is already unlocked by Viaccess or by a concurrent software.
 @retval kVA_INSERT_CARD_FIRST   If no smartcard is inserted in the slot.
 
 For related information, refer to @ref VA_SC_Unlock
*/
INT VA_SC_Unlock(DWORD dwScSlot);
	
/** @} */


/**
 @defgroup viaccessSCFunc VA_SC Viaccess Functions
 @brief The smartcard driver functions to be implemented by Viaccess.
 @{
 */

/**
 The STB calls this function once a smartcard has been successfully reset, including an eventual PPS negotiation.
 @param [in] dwScSlot Indicates the slot number of the smartcard to be unlocked. This value ranges from 0 to uiNbSmartCard--1.
 @param [in] uiAtrSize Specifies the size of the ATR message sent by the smartcard. This value ranges between 2 and 33 bytes.
 @param [in] pAtr Buffer that contains the ATR information, including the initial character TS (see the ISO/IEC 7816-3 document).
 @param [in] uiPPSNegotiatedvalue Indicates the PPS negotiated value.
                                  This parameter specifies the communication speed set during the reset in the case of a PPS 
								  negotiation. uiPpsnegotiatedValue must be less than or equal to the uiPpsNegotiationValue 
								  given as a parameter of the @ref VA_SC_Reset function.
                                  The 4 MSB of uiPpsnegotiatedValue indicate the negotiated factor F and the 4 LSB indicate 
								  the negotiated factor D.
			@note This value is set to 0 only if uiPpsNegotiationValue is set to 0 in the @ref VA_SC_Reset function. 
			      Otherwise this value must reflect the actual communication speed.
 @retval kVA_OK                  If the callback is successful.
 @retval kVA_INVALID_PARAMETER   If any of the function parameters do not meet the above conditions.
 @retval kVA_UNEXPECTED_CALLBACK If Viaccess did not require a reset operation.

 @note The pAtr argument points to a buffer that is allocated and filled by the STB. 
       This buffer can be freed as soon as the @ref VA_SC_ResetDone function returns.
       <br> For information on the related STB function, refer to @ref VA_SC_Reset.
       <br> After a call to @ref VA_SC_ResetDone and before it returns, 
	   Viaccess must be able to call the function that triggered this call without it returning @ref kVA_RESOURCE_BUSY. 
	   (This function is not called in the @ref VA_SC_ResetDone function.)
 
 For information on the related STB function, refer to @ref VA_SC_Reset
*/
INT VA_SC_ResetDone(DWORD dwScSlot, UINT8 uiAtrSize, BYTE *pAtr, UINT8 uiPPSNegotiatedvalue);


/**
 The STB calls this function when the reset of a smartcard fails.
 @param [in] dwScSlot Indicates the slot number of the smartcard to be unlocked. This value ranges from 0 to uiNbSmartCard--1.
 @retval kVA_OK                  If the callback is successful.
 @retval kVA_INVALID_PARAMETER   If dwScSlot is out of range.
 @retval kVA_UNEXPECTED_CALLBACK If Viaccess did not require a reset operation.

 @note After a call to VA_SC_ResetFailed and before it returns, Viaccess must be able to call the function that triggered 
       this call without it returning kVA_RESOURCE_BUSY. (This function is not called in the VA_SC_ResetFailed function.)

 For information on the related STB function, refer to @ref VA_SC_Reset
*/
INT VA_SC_ResetFailed(DWORD dwScSlot);


/**
 The STB calls this callback function when a smartcard is inserted in the smartcard slot.
 @param [in] dwScSlot Indicates the slot number of the smartcard to be unlocked. This value ranges from 0 to uiNbSmartCard--1. 
 @retval kVA_OK                  If the notification was successful.
 @retval kVA_INVALID_PARAMETER   If dwScSlot is out of range.
 @retval kVA_UNEXPECTED_CALLBACK If Viaccess did not require a reset operation.
 
 @note After a call to the VA_SC_CardInserted function and before it returns, Viaccess must be able to call 
       the @ref VA_SC_Reset function without it returning kVA_INSERT_CARD_FIRST and the VA_SC_GetState function 
	   must indicate that the smartcard is inserted. (VA_SC_Reset and VA_SC_GetState are not called in 
	   the @ref VA_SC_CardInserted function.)
 
 For related information, refer to: @ref VA_SC_CardExtracted, @ref VA_SC_Reset
*/
INT VA_SC_CardInserted(DWORD dwScSlot);

/**
 The STB calls this function when a smartcard is extracted.
 @param [in] dwScSlot Indicates the slot number of the smartcard to be unlocked. This value ranges from 0 to uiNbSmartCard--1. 
 @retval kVA_OK                If the notification was successful.
 @retval kVA_INVALID_PARAMETER If dwScSlot is out of range.
 
 @note If a smartcard is extracted whilst a command is being processed, the STB calls the @ref VA_SC_CommandFailed function
       before calling the @ref VA_SC_CardExtracted function.
 
  For related information, refer to: @ref VA_SC_CardExtracted, @ref VA_SC_Reset
  
  @note After a call to the VA_SC_CardExtracted function and before it returns, Viaccess must be able to call 
  the @ref VA_SC_GetState function that must indicate that the card is extracted. 
  (@ref VA_SC_GetState is not called in the @ref VA_SC_CardExtracted function.)
*/
INT VA_SC_CardExtracted(DWORD dwScSlot);


/**
 This function is called to indicate that a given smartcard command has been successfully processed.
 @param [in] dwScSlot Indicates the slot number of the smartcard to be unlocked. This value ranges from 0 to uiNbSmartCard--1.
 @param [in] uiReturnSize Indicates the number of bytes returned by the smartcard.
 @param [in] pReturn Details the buffer that contains the bytes returned by the smartcard.
 @retval kVA_OK                  If the callback is successful.
 @retval kVA_INVALID_PARAMETER   If dwScSlot is out of range or if pReturn equals NULL.
 @retval kVA_UNEXPECTED_CALLBACK If Viaccess did not require a send command operation.
 
 @note The memory block that pReturn points to is allocated and filled by the STB. It can be freed as soon as 
       the @ref VA_SC_CommandDone function returns.
       <br> For information on the related STB function, refer to @ref VA_SC_SendCommand.
       <br> After a call to @ref VA_SC_CommandDone and before it returns, Viaccess must be able to call the function that triggered 
	   this call without it returning @ref kVA_RESOURCE_BUSY. (This function is not called in the @ref VA_SC_CommandDone function.)
*/
INT VA_SC_CommandDone(DWORD dwScSlot, UINT32 uiReturnSize, BYTE *pReturn);

/**
 This function is called to indicate that a smartcard command has failed due to a communication problem with
 the smartcard. Reasons for this can be either an extracted smartcard or a timeout occurrence during the
 processing of the smartcard command.
 @param [in] dwScSlot Indicates the slot number of the smartcard to be unlocked. This value ranges from 0 to uiNbSmartCard--1.  
 @retval kVA_OK                  If the callback is successful.
 @retval kVA_INVALID_PARAMETER   If dwScSlot is out of range.
 @retval kVA_UNEXPECTED_CALLBACK If Viaccess did not require a send command operation.
 
 @note After a call to VA_SC_CommandFailed and before it returns, Viaccess must be able to call the function that triggered 
       this call without it returning kVA_RESOURCE_BUSY. (This function is not called in the @ref VA_SC_CommandFailed function.)

 For information on the related STB function, refer to @ref VA_SC_SendCommand
*/
INT VA_SC_CommandFailed(DWORD dwScSlot);

/** @} */

/** @} */

#endif /* _VA_SC_H_ */

