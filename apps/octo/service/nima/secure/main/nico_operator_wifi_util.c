/*************************************************************************************************************
	File 		: nico_operator_wifi_util.c
	author 		: IPM tipmsw1@humaxdigital.com
	comment		:
	date    	: 2013/06/03
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

#include "nico.h"
#include "nico_operator_wifi_util.h"

#include "hlib.h"

static HBOOL		__nico_operator_wifi_is_Ascii(HUINT8 *pszPasswd, HINT32 nLen)
{
	HINT32	i = 0;

	if( (nLen == 0) || HxSTD_StrLen(pszPasswd) < nLen)
	{
		return FALSE;
	}

	for(i = 0; i < nLen; i++)
	{
		if((pszPasswd[i] < 0x20)
			|| (pszPasswd[i] > 0x7E)) //All printable characters in ASCII table, including Space
		{
			return FALSE;
		}
	}

	return TRUE;
}

static HBOOL		__nico_operator_wifi_is_Hex(HUINT8 *pszPasswd, HINT32 nLen)
{
	HINT32	i = 0;
	if(	(nLen == 0) || HxSTD_StrLen(pszPasswd) < nLen)
	{
		return FALSE;
	}

	for(i = 0; i < nLen; i++)
	{
		if((pszPasswd[i] < 0x30)	/* 0 ���� �۰ų� */
			|| ((pszPasswd[i] > 0x39) && (pszPasswd[i] < 0x41))	/* 9���� ũ�ų� A ���� �۰ų� */
			|| ((pszPasswd[i] > 0x46) && (pszPasswd[i] < 0x61))	/* F���� ũ�ų� a ���� �۰ų� */
			|| (pszPasswd[i] > 0x66))	/*  f���� ũ�ų� */
		{
			return FALSE;
		}
	}

	return TRUE;
}

HBOOL nico_wifiutil_is_validkey(eNicoWifiEncrypt eAuthnEnc, HUINT8 *key)
{
	HINT32		nStrLen = 0;
	ENTRY;
	nStrLen = HxSTD_StrLen(key);
	HxLOG_Debug("encrypt : %d, key : %s \n", eAuthnEnc, key);

	switch(eAuthnEnc)
	{
	case eNICO_WIFI_ENCRYPT_WEP_64HEX:
		if(nStrLen != 10)
		{
			return FALSE;
		}
		return __nico_operator_wifi_is_Hex(key, HxSTD_StrLen(key));

	case eNICO_WIFI_ENCRYPT_WEP_128HEX:
		if(nStrLen != 26)
		{
			return FALSE;
		}
		return __nico_operator_wifi_is_Hex(key, HxSTD_StrLen(key));

	case eNICO_WIFI_ENCRYPT_WEP_64:
		if( 5 == nStrLen || 13 == nStrLen  )
		{
			return __nico_operator_wifi_is_Ascii(key, HxSTD_StrLen(key));
		}
		else if ( 10 == nStrLen || 26 == nStrLen )
		{
			return __nico_operator_wifi_is_Hex(key, HxSTD_StrLen(key));
		}
		break;

	case eNICO_WIFI_ENCRYPT_WEP_128:
		if(nStrLen != 13)
		{
			return FALSE;
		}
		return __nico_operator_wifi_is_Ascii(key, HxSTD_StrLen(key));
		break;

	case eNICO_WIFI_ENCRYPT_TKIP:
	case eNICO_WIFI_ENCRYPT_AES:
	case eNICO_WIFI_ENCRYPT_TKIP_AES:
	//case eNet_Wifi_Au_En_Type_WPA2_TKIP:
	//case eNet_Wifi_Au_En_Type_WPA2_AES:
		/* 8���� �۰ų� 64���� ū ���� false */
		if((nStrLen > 64) || (nStrLen < 8))
		{
			return FALSE;
		}

		/* 8���� 63�̶�� ascii �ȿ��� ������ ok */
		if((nStrLen >= 8) && (nStrLen <= 63))
		{
			return __nico_operator_wifi_is_Ascii(key, HxSTD_StrLen(key));
		}

		/* ���� 64�� hex�ȿ� ���� �Ѵ�. */
		return __nico_operator_wifi_is_Hex(key, HxSTD_StrLen(key));
		break;

	case eNICO_WIFI_ENCRYPT_NONE:
	default:
		/* ��ȣ �ʿ����. */
		nStrLen = 0;
	}
	EXIT;
	return FALSE;
}

HBOOL		nico_wifiutil_encrypt_autowep(NICO_Wifi_APInfo_t * apinfo)
{
	HINT32		nStrLen = 0;

	nStrLen = HxSTD_StrLen(apinfo->apkey);

	if(apinfo->encrypt_type <= eNICO_WIFI_ENCRYPT_WEP_128HEX)
	{
		if( nStrLen == 5 )
			apinfo->encrypt_type = eNICO_WIFI_ENCRYPT_WEP_64;
		else if( nStrLen == 13 )
			apinfo->encrypt_type = eNICO_WIFI_ENCRYPT_WEP_128 ;
		else if( nStrLen == 10 )
			apinfo->encrypt_type = eNICO_WIFI_ENCRYPT_WEP_64HEX ;
		else if( nStrLen == 26 )
			apinfo->encrypt_type = eNICO_WIFI_ENCRYPT_WEP_128HEX;
		else
			return FALSE;
		}

	return TRUE;
}


