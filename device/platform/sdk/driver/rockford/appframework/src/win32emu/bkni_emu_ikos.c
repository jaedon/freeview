/***************************************************************************
*     Copyright (c) 2003, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: bkni_emu_ikos.c $
* $brcm_Revision: Hydra_Software_Devel/2 $
* $brcm_Date: 10/5/03 6:16p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /TestTools/midas/magnum/services/basemodules/kni/win32emu/bkni_emu_ikos.c $
* 
* Hydra_Software_Devel/2   10/5/03 6:16p frusso
* changed init call to use uri
* 
* Hydra_Software_Devel/1   9/24/03 6:39p frusso
* initial checkin
* 
***************************************************************************/

#include <winsock2.h>
#include <ws2tcpip.h>

#include "bstd.h"
#include "bmem.h"
#include "bkni.h"
#include "bdbg.h"
#include "bkni_emu.h"
#include "bkni_emu_ikos.h"

#define IKOS_NAK                 0
#define IKOS_ACK                 1

#define IKOS_REG_READ            2
#define IKOS_REG_WRITE           4
#define IKOS_MEM_READ            6
#define IKOS_MEM_WRITE           8

#define MAX(a,b) ( a > b ) ? a : b;
#define MIN(a,b) ( a > b ) ? b : a;

#define pout(string, integer) {\
	printf( "ERROR: %s %d: %s: %d\n", string, integer, __FILE__, __LINE__ );\
}

static SOCKET g_socketIKOS;

bool getIkosEndpoint( const char* pszURI, char* pszHost, char* pszPort )
{
	int nLen, nMaxLen=31;
	char *pStart, *pEnd, *pColon;

	// Check for the correct protocol
	pColon = strstr( pszURI, ":" );
	if ( !pColon )
		return false; // Invalid URI

	pStart = pColon + 1;
	pEnd = (char*)pszURI + strlen(pszURI);

	// Check for a host name
	pszHost[0] = 0;
	if ( (pStart < pEnd) && *pStart == '/' )
	{
		pStart++;
		if ( (pStart < pEnd) && *pStart == '/' )
			pStart++;

		// Get the host name
		pEnd = pStart + strcspn( pStart, ":/\0" );
		nLen = MIN( (pEnd - pStart), nMaxLen );
		strncpy( pszHost, pStart, pEnd-pStart );
		pszHost[nLen] = 0;

		// Check for a port number
		if ( *pEnd == ':' )
		{
			pStart = pEnd+1;
			pEnd = pStart + strcspn( pStart, " /\0" );
			nLen = MIN( (pEnd - pStart), nMaxLen );
			strncpy( pszPort, pStart, nLen );
			pszPort[nLen] = 0;
		}
	}
	else
	{
		if ( pStart < pEnd )
		{
			nLen = MIN( (pEnd - pStart), nMaxLen );
			strncpy( pszPort, pStart, nLen );
			pszPort[nLen] = 0;
		}
	}

	return true;
}

/*****************************************************************************/

int BKNI_EMU_IkosRecv( SOCKET s, char *buf, int len, int flags )
{
	int result;
	int original_len = len;

	for( ;; )
	{
		result = recv( s, buf, len, flags );

		if( result == SOCKET_ERROR )
		{
			break;
		}

		buf += result;
		len -= result;

		if( !len )
		{
			result = original_len;
			break;
		}
	}

	return result;
}

/*****************************************************************************/

int BKNI_EMU_IkosSend( SOCKET s, const char *buf,	int len, int flags )
{
	int total = 0;        // how many bytes we've sent
    int bytesleft = len;  // how many we have left to send
    int n;

    while(total < len) 
	{
        n = send(s, buf+total, bytesleft, flags);
        if (n == SOCKET_ERROR) break;
        total += n;
        bytesleft -= n;
    }

    return total;
}

/*****************************************************************************/

int BKNI_EMU_IkosReadReg( uint32_t address, uint32_t *data )
{
	int iReturn = SOCKET_ERROR;
	int iResult;
	char out_buffer[ 5 ];
	unsigned char in_buffer[ 5 ];

	BKNI_EMU_P_PageUnmapAll();
	address |= 0x10000000;

	out_buffer[ 0 ] = IKOS_REG_READ;

	out_buffer[ 1 ] = (uint8_t) (address >> 24);
	out_buffer[ 2 ] = (uint8_t) (address >> 16);
	out_buffer[ 3 ] = (uint8_t) (address >>  8);
	out_buffer[ 4 ] = (uint8_t) (address >>  0);

	iResult = BKNI_EMU_IkosSend( g_socketIKOS, out_buffer, sizeof(out_buffer), 0 );
	if( iResult < sizeof(out_buffer) )
	{
		pout( "IKOS_REG_READ send() only sent", iResult );
		goto done;
	}

	iResult = BKNI_EMU_IkosRecv( g_socketIKOS, in_buffer, sizeof in_buffer, 0 );
	if( iResult < 0 )
	{
		pout( "IKOS_REG_READ recv():", WSAGetLastError() );
		goto done;
	}
	else if( iResult != sizeof in_buffer )
	{
		pout( "IKOS_REG_READ unexpected ACK size", iResult );
		goto done;
	}
	else if( in_buffer[ 0 ] != (out_buffer[ 0 ] | IKOS_ACK) )
	{
		pout( "IKOS_REG_READ wrong ACK", in_buffer[ 0 ] );
		goto done;
	}

	*data =
		((uint32_t) in_buffer[ 1 ] << 24) |
		((uint32_t) in_buffer[ 2 ] << 16) |
		((uint32_t) in_buffer[ 3 ] <<  8) |
		((uint32_t) in_buffer[ 4 ] <<  0);

	iReturn = 0;

done:
	return iReturn;
}

/*****************************************************************************/

int BKNI_EMU_IkosWriteReg( uint32_t address, uint32_t data )
{
	int iReturn = SOCKET_ERROR;
	int iResult;
	char out_buffer[ 9 ];
	char in_buffer[ 1 ];

	BKNI_EMU_P_PageUnmapAll();
	address |= 0x10000000;

	out_buffer[ 0 ] = IKOS_REG_WRITE;

	out_buffer[ 1 ] = (uint8_t) (address >> 24);
	out_buffer[ 2 ] = (uint8_t) (address >> 16);
	out_buffer[ 3 ] = (uint8_t) (address >>  8);
	out_buffer[ 4 ] = (uint8_t) (address >>  0);

	out_buffer[ 5 ] = (uint8_t) (data >> 24);
	out_buffer[ 6 ] = (uint8_t) (data >> 16);
	out_buffer[ 7 ] = (uint8_t) (data >>  8);
	out_buffer[ 8 ] = (uint8_t) (data >>  0);

	iResult = BKNI_EMU_IkosSend( g_socketIKOS, out_buffer, sizeof out_buffer, 0 );
	if( iResult < 0 )
	{
		pout( "IKOS_REG_WRITE send():", WSAGetLastError() );
		goto done;
	}
	else if( iResult != sizeof out_buffer )
	{
		pout( "IKOS_REG_WRITE send() only sent", iResult );
		goto done;
	}

	iResult = BKNI_EMU_IkosRecv( g_socketIKOS, in_buffer, sizeof in_buffer, 0 );
	if( iResult < 0 )
	{
		pout( "IKOS_REG_WRITE BKNI_EMU_IkosRecv()", WSAGetLastError() );
		goto done;
	}
	else if( iResult != sizeof in_buffer )
	{
		pout( "IKOS_REG_WRITE unexpected ACK size", iResult );
		goto done;
	}
	else if( in_buffer[ 0 ] != (out_buffer[ 0 ] | IKOS_ACK) )
	{
		pout( "IKOS_REG_WRITE wrong ACK", in_buffer[ 0 ] );
		goto done;
	}

	iReturn = 0;

done:
	return iReturn;
}

/*****************************************************************************/

int BKNI_EMU_IkosReadMem( uint32_t address, void *data, size_t size )
{
	int iReturn = SOCKET_ERROR;
	int iResult;
	char out_buffer[ 9 ];
	unsigned char in_buffer[ 4097 ];
	size_t ss;

	if( size & 3 )
	{
		pout( "IKOS_MEM_READ non-dword size:", size);
		goto done;
	}

	out_buffer[ 0 ] = IKOS_MEM_READ;

	out_buffer[ 1 ] = (uint8_t) (address >> 24);
	out_buffer[ 2 ] = (uint8_t) (address >> 16);
	out_buffer[ 3 ] = (uint8_t) (address >>  8);
	out_buffer[ 4 ] = (uint8_t) (address >>  0);

	out_buffer[ 5 ] = (uint8_t) (size >> 24);
	out_buffer[ 6 ] = (uint8_t) (size >> 16);
	out_buffer[ 7 ] = (uint8_t) (size >>  8);
	out_buffer[ 8 ] = (uint8_t) (size >>  0);

	iResult = BKNI_EMU_IkosSend( g_socketIKOS, out_buffer, sizeof out_buffer, 0 );
	if( iResult < 0 )
	{
		pout( "IKOS_MEM_READ send()", WSAGetLastError() );
		goto done;
	}
	else if( iResult != sizeof out_buffer )
	{
		pout( "IKOS_MEM_READ send() only sent", iResult );
		goto done;
	}

	iResult = BKNI_EMU_IkosRecv( g_socketIKOS, in_buffer, (int) size + 1, 0 );
	if( iResult < 0 )
	{
		pout( "IKOS_MEM_READ BKNI_EMU_IkosRecv()", WSAGetLastError() );
		goto done;
	}
	else if( iResult != (int)(size + 1) )
	{
		pout( "IKOS_MEM_READ unexpected ACK size", iResult );
		goto done;
	}
	else if( in_buffer[ 0 ] != (out_buffer[ 0 ] | IKOS_ACK) )
	{
		pout( "IKOS_MEM_READ wrong ACK", in_buffer[ 0 ] );
		goto done;
	}

	for( ss = 0; ss < size; ss += sizeof (uint32_t) )
	{
		*(uint32_t *) ((uintptr_t) data + ss) =
			(in_buffer[ ss + 1 ] << 24) | (in_buffer[ ss + 2 ] << 16) |
			(in_buffer[ ss + 3 ] <<  8) | (in_buffer[ ss + 4 ] <<  0);
	}

	iReturn = 0;

done:
	return iReturn;
}

/*****************************************************************************/

int BKNI_EMU_IkosWriteMem( uint32_t address, void *data, size_t size )
{
	int iReturn = SOCKET_ERROR;
	int iResult;
	char out_buffer[ 4106 ];
	char in_buffer[ 1 ];
	size_t ss;
	uint32_t uu;

	if( size & 3 )
	{
		pout( "IKOS_MEM_WRITE non-dword size:", size);
		goto done;
	}

	out_buffer[ 0 ] = IKOS_MEM_WRITE;

	out_buffer[ 1 ] = (uint8_t) (address >> 24);
	out_buffer[ 2 ] = (uint8_t) (address >> 16);
	out_buffer[ 3 ] = (uint8_t) (address >>  8);
	out_buffer[ 4 ] = (uint8_t) (address >>  0);

	out_buffer[ 5 ] = (uint8_t) (size >> 24);
	out_buffer[ 6 ] = (uint8_t) (size >> 16);
	out_buffer[ 7 ] = (uint8_t) (size >>  8);
	out_buffer[ 8 ] = (uint8_t) (size >>  0);

	for( ss = 0; ss < size; ss += sizeof (uint32_t) )
	{
		uu = *(uint32_t *) ((uintptr_t) data + ss);

		out_buffer[ ss +  9 ] = (uint8_t) (uu >> 24);
		out_buffer[ ss + 10 ] = (uint8_t) (uu >> 16);
		out_buffer[ ss + 11 ] = (uint8_t) (uu >>  8);
		out_buffer[ ss + 12 ] = (uint8_t) (uu >>  0);
	}

	iResult = BKNI_EMU_IkosSend( g_socketIKOS, out_buffer, (int) size + 9, 0 );
	if( iResult < 0 )
	{
		pout( "IKOS_MEM_WRITE send()", WSAGetLastError() );
		goto done;
	}
	else if( iResult != ((int) size + 9) )
	{
		pout( "IKOS_MEM_WRITE send() only sent", iResult );
		goto done;
	}

	iResult = BKNI_EMU_IkosRecv( g_socketIKOS, in_buffer, sizeof in_buffer, 0 );
	if( iResult < 0 )
	{
		pout( "IKOS_MEM_WRITE BKNI_EMU_IkosRecv()", WSAGetLastError() );
		goto done;
	}
	else if( iResult != sizeof in_buffer )
	{
		pout( "IKOS_MEM_WRITE unexpected ACK size", iResult );
		goto done;
	}
	else if( in_buffer[ 0 ] != (out_buffer[ 0 ] | IKOS_ACK) )
	{
		pout( "IKOS_MEM_WRITE wrong ACK", in_buffer[ 0 ] );
		goto done;
	}

	iReturn = 0;

done:
	return iReturn;
}

/*****************************************************************************/

int BKNI_EMU_IkosInit( const char* pszUri )
{
	WSADATA wsaData;
	WORD wVersion = MAKEWORD( 2, 2 );
	char in_buffer[2];
	char szHost[256];
	char szPort[16];
	ADDRINFO ai;
	ADDRINFO *pai;
	int iError;

	getIkosEndpoint( pszUri, szHost, szPort );

	iError = WSAStartup( wVersion, &wsaData );
	if( iError )
		return iError;

	g_socketIKOS = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
	if( g_socketIKOS == INVALID_SOCKET )
		return WSAGetLastError();

	memset( &ai, 0, sizeof ai );
	ai.ai_flags = AI_CANONNAME;
	ai.ai_family = AF_UNSPEC;
	ai.ai_socktype = SOCK_STREAM;
	ai.ai_protocol = IPPROTO_TCP;

	iError = getaddrinfo( szHost, szPort, &ai, &pai );
	if( iError )
		return iError;

	iError = connect( g_socketIKOS, pai->ai_addr, (int) pai->ai_addrlen );
	if( iError == SOCKET_ERROR )
		return WSAGetLastError();

	/* get Ack */
	iError = recv( g_socketIKOS, in_buffer, 1, 0 );
	if( iError == SOCKET_ERROR )
		return WSAGetLastError();
	if( iError <= 0 )
		return -1; // connection refused

	return 0;
}

int BKNI_EMU_IkosUninit(void)
{
	shutdown( g_socketIKOS, SD_BOTH );
	closesocket( g_socketIKOS );
	WSACleanup();
	return 0;
}
