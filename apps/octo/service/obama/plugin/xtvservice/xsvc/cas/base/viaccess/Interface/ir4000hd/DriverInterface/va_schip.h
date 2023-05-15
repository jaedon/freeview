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

/*
    $Revision: 5833 $
    $Date: 2007-05-10 17:56:04 +0200 (jeu., 10 mai 2007) $
*/


#ifndef _VA_SCHIP_H_
#define _VA_SCHIP_H_

#include "va_types.h"
#include "va_errors.h"


/**
 @ingroup drivers
 */

/**
 @defgroup schipDriver VA_SCHIP Driver
 @brief This API is the interface for ACS to access Secured Chipset resources.
 @{ */

/**
 * @defgroup schipEnum VA_SCHIP Enumerations
 * @brief The chipset driver's enumerations

 * @{ */

/**
The sVA_SCHIP_CipheringMode enumeration defines the possible modes of activation for the control word protection.
*/
typedef enum sVA_SCHIP_CipheringMode
{
	/**
	The chipset only sets the CW on the descrambler.
	*/
	eINACTIVE,
	/**
	The CW is deciphered by the chipset and set on the descrambler. This chipset-working mode is reversible.
	*/
	eSESSION,
	/**
	The CW is deciphered by the chipset and set on the descrambler. This chipset-working mode is not reversible.
	*/
	eLOCKED

} tVA_SCHIP_CipheringMode;
/**@}*/


/**
 * @defgroup schipFunc VA_SCHIP Manufacturer Functions
@brief The following section gives detailed information on the different secured chipset mechanisms.

* @{
*/

/*---------------------------------------------------------------------------*/
/**
    This function gives the chipset its ciphering activation mode. The CW that is received after this call will then be deciphered or not deciphered.
    @param  eChipsetMode    Identifies the activation mode. Please refer to the enumeration description for details.
    @return                 	<ul>
					<li>
						<code>kVA_OK</code>
						<br/>
						If the secured chipset has successfully returned the mode.
					</li>
					<li>
						<code>kVA_NOT_IMPLEMENTED</code>
						<br/>
						If the secured chipset is not present.
					</li>
				</ul>
	@note When calling this function with <code>eLOCKED</code>, the chipset must be locked by blowing the dedicated hardware fuse.
*/
INT VA_SCHIP_SetChipsetMode ( tVA_SCHIP_CipheringMode eChipsetMode );

/*---------------------------------------------------------------------------*/
/**
    This function returns the ciphering activation mode of the chipset.
    @param  pChipsetMode    Pointer on the activation mode. Please refer to the enumeration description for details.
    @return                 	<ul>
					<li>
						<code>kVA_OK</code>
						<br/>
						The chipset has successfully applied the mode.
					</li>
					<li>
						<code>kVA_NOT_IMPLEMENTED</code>
						<br/>
						The mode cannot be applied because the chipset is locked, or because the requested mode is not supported
						by the chipset or because no secure chipset is present.
					</li>
					<li>
						<code>kVA_INVALID_PARAMETER</code>
						<br/>
						The mode doesn't belong to the tVA_SCHIP_CipheringMode enum.
					</li>
				</ul>
*/
INT VA_SCHIP_GetChipsetMode ( tVA_SCHIP_CipheringMode* pChipsetMode );

/**
	This function provides the chipset with a session key that will be used later to decipher the received CW.
	@param  uiSessionKeyLength    	Size in bytes of the session key.
	@param  pSessionKey    		Buffer of bytes containing the session key.
	@return                 	<ul>
					<li>
						<code>kVA_OK</code>
						<br/>
						If the chipset has successfully received the session key.
					</li>
					<li>
						<code>kVA_NOT_IMPLEMENTED</code>
						<br/>
						If the secured chipset is not present.
					</li>
					<li>
						<code>kVA_INVALID_PARAMETER</code>
						<br/>
						If the key pointer is NULL.
						<br/>
						If the key length is not compatible with the secured chipset cryptographic protocol (e.g. the 3DES protocol usually uses a 16-bytes key).
					</li>
				</ul>
*/
INT VA_SCHIP_SetSessionKey ( UINT32 uiSessionKeyLength, const BYTE *pSessionKey );

/*---------------------------------------------------------------------------*/
/**
    This function returns the unique chipset identifier that is written at the manufacturing site by Viaccess equipment; it is publicly accessible.
    @return                 	
			<ul>
				<li>
					<code>kVA_ILLEGAL_HANDLE</code>
					<br/>
					If the chipset identifier can not be retrieved.
				</li>
				<li>
					Otherwise, 
					<br/>
					the chipset identifier.
				</li>
			</ul>
*/
DWORD VA_SCHIP_GetChipsetId ( void );

/**@}*/
/**@}*/

#endif /* _VA_SCHIP_H_ */

