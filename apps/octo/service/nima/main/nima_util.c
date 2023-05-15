/*************************************************************************************************************
	File 		: nico_operator_netconf_util.c
	author 		: IPM tipmsw1@humaxdigital.com
	comment		:
	date    	: 2013/06/04
	attention 	:

	(c) 2011-2012 Humax Co., Ltd.
	This program is produced by Humax Co., Ltd. (Humax) and
	the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
	non-assignable, non-transferable and non-exclusive license to use this Software.
	You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
	you agree to the responsibility to take all reasonable efforts to protect the any information
	you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
	reverse engineer or extract the source code of this Software unless you have Humax's written permission
	to do so.
	If you have no authorized license, discontinue using this Software immediately.

	THE SOFTWARE IS PROVIDED ¡°AS IS¡± AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES,
	EITHER EXPRESS, IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
	IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR
	INDIRECT, SPECIAL, CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR
	INABILITY TO USE THE SOFTWARE.

	This License is effective until terminated. You may terminate this License at any time
	by destroying all copies of the Software including all documentation. This License will terminate
	immediately without notice from Humax to you if you fail to comply with any provision of this License.
	Upon termination, you must destroy all copies of the Software and all documentation.

	The laws of the Republic of Korea will apply to any disputes arising out of or relating to
	this Copyright Notice. All claims arising out of or relating to this Copyright Notice will be litigated
	in the Seoul Central District Court, in the Republic of Korea.
*************************************************************************************************************/

#include "nima_util.h"
#include "hlib.h"

#define NICO_DHCPS_DEFAULT_LEASE_TIME 864000
#define NICO_DHCPS_DEFAULT_DOMAIN	"local"
#define NICO_DHCPS_DEFAULT_ROUTER	"192.168.11.2"
#define NICO_DHCPS_DEFAULT_NETMSK	"255.255.255.0"
#define NICO_DHCPS_DEFAULT_DNS		"129.219.13.81"

HUINT8* nima_util_ConverMacAddressStringIntoByte(const HCHAR *srcMacString, HUINT8* dstMacBytes)
{
	const HCHAR cSep = '-';
	HINT32 iCounter = 0;
	for (iCounter = 0; iCounter < 6; ++iCounter)
	{
		HUINT32 iNumber = 0;
		HCHAR ch;

		//Convert letter into lower case.
		ch = tolower (*srcMacString++);

		if ((ch < '0' || ch > '9') && (ch < 'a' || ch > 'f'))
		{
			return NULL;
		}

		//Convert into number.
		//       a. If character is digit then ch - '0'
		//	b. else (ch - 'a' + 10) it is done
		//	because addition of 10 takes correct value.
		iNumber = isdigit (ch) ? (ch - '0') : (ch - 'a' + 10);
		ch = tolower (*srcMacString);

		if ((iCounter < 5 && ch != cSep) ||
			(iCounter == 5 && ch != '\0' && !isspace (ch)))
		{
			++srcMacString;

			if ((ch < '0' || ch > '9') && (ch < 'a' || ch > 'f'))
			{
				return NULL;
			}

			iNumber <<= 4;
			iNumber += isdigit (ch) ? (ch - '0') : (ch - 'a' + 10);
			ch = *srcMacString;

			if (iCounter < 5 && ch != cSep)
			{
				return NULL;
			}
		}
		/* Store result.  */
		dstMacBytes[iCounter] = (HUINT8) iNumber;
		/* Skip cSep.  */
		++srcMacString;
	}
	return dstMacBytes;
}


