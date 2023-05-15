#include <stdio.h>
#include "chipif.h"
#include "pagelib.h"

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define EMU_HOST "x-irva-01"
#define EMU_PORT "9999"

#define EMU_NAK                 0
#define EMU_ACK                 1

#define EMU_STOP_SERVER         0
#define EMU_STOP_CLIENT         1
#define EMU_REG_READ            2
#define EMU_REG_WRITE           4
#define EMU_MEM_READ            6
#define EMU_MEM_WRITE           8
#define EMU_TIMEOUT            10
#define EMU_INIT_CAPTURE       12
#define EMU_START_CAPTURE      14
#define EMU_STOP_CAPTURE       16

#define pout(string, integer) { printf( "ERROR: %s %d: %s: %d\n", string, integer, __FILE__, __LINE__ ); } 

#define SOCKET_ERROR -1

static int s ;

int my_recv
(
	int s,
	void *buf,
	int len,
	int flags
)
{
	int result;
	int original_len = len;

	for( ;; )
	{
		result = recv( s, buf, len, flags );
		if( result <= 0 )
		{
			break;
		}

		buf = (char *) buf + result;
		len -= result;

		if( !len )
		{
			result = original_len;
			break;
		}
	}

	return result;
}

int my_send
(
	int s, 
	void *buf, 
	int len, 
	int flag
)
{
    int total = 0;        /* how many bytes we've sent */
    int bytesleft = len; /* how many we have left to send*/
    int n;

    while(total < len) {
        n = send(s, (char *)buf+total, bytesleft, flag);
        if (n == -1) { break; }
        total += n;
        bytesleft -= n;
    }

    return total; /* return -1 on failure, 0 on success*/
}


int BCHIPIF_Init(char *hostname)
{
	struct addrinfo ai;
	struct addrinfo *pai;
	int iError = 0;
	unsigned char in_buffer[ 5 ];
	
	s = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
	if( s == -1 )
	{
		perror( "socket()" );
		goto done;
	}

	ai.ai_flags = AI_CANONNAME;
	ai.ai_family = AF_UNSPEC;
	ai.ai_socktype = SOCK_STREAM;
	ai.ai_protocol = IPPROTO_TCP;
	iError = getaddrinfo( hostname? hostname:EMU_HOST, EMU_PORT, &ai, &pai );
	if( iError )
	{
		pout( "getaddrinfo()", iError );
		goto done;
	}

	iError = connect( s, pai->ai_addr, (int) pai->ai_addrlen );
	if( iError == SOCKET_ERROR )
	{
		perror( "connect()" );
		goto done;
	}

	/* get Ack */
	iError = recv( s, in_buffer, 1, 0 );
	if( iError <= 0 )
	{
		printf( "Connection is rejected by server. Another host may be connecting with it.\n" );
		return 1;
	} 
	else
	{
		printf( "Connected with the server.\n" );
	}

done:
	return iError;
}

int BCHIPIF_Close(void)
{
	int iError = 0;
	char c ;
	
	printf( "Sending terminate messages for client and server \n" );
	/* This flag is use to shutdown the server.  When done
	 * with EMU set this c=EMU_STOP_SERVER, and re-run it to shutdown the
	 * server.  Don't forget to set it back to EMU_STOP_CLIENT */
	c = EMU_STOP_CLIENT;

	iError = my_send( s, &c, 1, 0 );
	if( iError != 1 )
	{
		perror( "send()");
		goto done ;
	}

	c = EMU_STOP_SERVER;

	iError = my_send( s, &c, 1, 0 );
	if( iError != 1 )
	{
		perror( "send()");
	}

done:
	return iError;
}

int BCHIPIF_ReadRegister( uint32_t reg_addr, uint32_t *data )
{
	int iReturn = SOCKET_ERROR;
	int iResult;
	char out_buffer[ 5 ];
	unsigned char in_buffer[ 5 ];

	PageWriteBack();
	reg_addr |= 0x10000000;

	out_buffer[ 0 ] = EMU_REG_READ;

	out_buffer[ 1 ] = (uint8_t) (reg_addr >> 24);
	out_buffer[ 2 ] = (uint8_t) (reg_addr >> 16);
	out_buffer[ 3 ] = (uint8_t) (reg_addr >>  8);
	out_buffer[ 4 ] = (uint8_t) (reg_addr >>  0);

	iResult = my_send( s, out_buffer, sizeof out_buffer, 0 );
	if( iResult < (int)sizeof(out_buffer) )
	{
		pout( "EMU_REG_READ send() only sent", iResult );
		goto done;
	}

	iResult = my_recv( s, (char *)in_buffer, sizeof in_buffer, 0 );
	if( iResult < 0 )
	{
		pout( "EMU_REG_READ recv():",  iResult);
		goto done;
	}
	else if( iResult != (int)sizeof(in_buffer) )
	{
		pout( "EMU_REG_READ unexpected ACK size", iResult );
		goto done;
	}
	else if( in_buffer[ 0 ] != (out_buffer[ 0 ] | EMU_ACK) )
	{
		pout( "EMU_REG_READ wrong ACK", in_buffer[ 0 ] );
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


int BCHIPIF_WriteRegister( uint32_t reg_addr, uint32_t data )
{
	int iReturn = SOCKET_ERROR;
	int iResult;
	char out_buffer[ 9 ];
	char in_buffer[ 1 ];

	PageWriteBack();
	reg_addr |= 0x10000000;

	out_buffer[ 0 ] = EMU_REG_WRITE;

	out_buffer[ 1 ] = (uint8_t) (reg_addr >> 24);
	out_buffer[ 2 ] = (uint8_t) (reg_addr >> 16);
	out_buffer[ 3 ] = (uint8_t) (reg_addr >>  8);
	out_buffer[ 4 ] = (uint8_t) (reg_addr >>  0);

	out_buffer[ 5 ] = (uint8_t) (data >> 24);
	out_buffer[ 6 ] = (uint8_t) (data >> 16);
	out_buffer[ 7 ] = (uint8_t) (data >>  8);
	out_buffer[ 8 ] = (uint8_t) (data >>  0);

	iResult = my_send( s, out_buffer, sizeof out_buffer, 0 );
	if( iResult < 0 )
	{
		perror( "EMU_REG_WRITE send():");
		goto done;
	}
	else if( iResult != sizeof out_buffer )
	{
		perror( "EMU_REG_WRITE send() only sent");
		goto done;
	}

	iResult = my_recv( s, in_buffer, sizeof in_buffer, 0 );
	if( iResult < 0 )
	{
		perror( "EMU_REG_WRITE my_recv()" );
		goto done;
	}
	else if( iResult != sizeof in_buffer )
	{
		perror( "EMU_REG_WRITE unexpected ACK size");
		goto done;
	}
	else if( in_buffer[ 0 ] != (out_buffer[ 0 ] | EMU_ACK) )
	{
		pout( "EMU_REG_WRITE wrong ACK", out_buffer[ 0 ]);
		goto done;
	}

	iReturn = 0;

done:
	return iReturn;
}

int BCHIPIF_ReadMemory( uint32_t mem_addr, void *data, size_t size )
{
	int iReturn = SOCKET_ERROR;
	int iResult;
	char out_buffer[ 9 ];
	unsigned char in_buffer[ 4097 ];
	size_t ss;

	if( size & 3 )
	{
		pout( "EMU_MEM_READ non-dword size:", size);
		goto done;
	}

	out_buffer[ 0 ] = EMU_MEM_READ;

	out_buffer[ 1 ] = (uint8_t) (mem_addr >> 24);
	out_buffer[ 2 ] = (uint8_t) (mem_addr >> 16);
	out_buffer[ 3 ] = (uint8_t) (mem_addr >>  8);
	out_buffer[ 4 ] = (uint8_t) (mem_addr >>  0);

	out_buffer[ 5 ] = (uint8_t) (size >> 24);
	out_buffer[ 6 ] = (uint8_t) (size >> 16);
	out_buffer[ 7 ] = (uint8_t) (size >>  8);
	out_buffer[ 8 ] = (uint8_t) (size >>  0);

	iResult = my_send( s, out_buffer, sizeof out_buffer, 0 );
	if( iResult < 0 )
	{
		perror( "EMU_MEM_READ send()" );
		goto done;
	}
	else if( iResult != sizeof out_buffer )
	{
		pout( "EMU_MEM_READ send() only sent", iResult );
		goto done;
	}

	iResult = my_recv( s, (char *)in_buffer, (int) size + 1, 0 );
	if( iResult < 0 )
	{
		perror( "EMU_MEM_READ my_recv()");
		goto done;
	}
	else if( iResult != (int)(size + 1) )
	{
		pout( "EMU_MEM_READ unexpected ACK size", iResult );
		goto done;
	}
	else if( in_buffer[ 0 ] != (out_buffer[ 0 ] | EMU_ACK) )
	{
		pout( "EMU_MEM_READ wrong ACK", in_buffer[ 0 ] );
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

int BCHIPIF_WriteMemory( uint32_t mem_addr, void *data, size_t size )
{
	int iReturn = SOCKET_ERROR;
	int iResult;
	char out_buffer[ 4106 ];
	char in_buffer[ 1 ];
	size_t ss;
	uint32_t uu;

	if( size & 3 )
	{
		pout( "EMU_MEM_WRITE non-dword size:", size);
		goto done;
	}

	out_buffer[ 0 ] = EMU_MEM_WRITE;

	out_buffer[ 1 ] = (uint8_t) (mem_addr >> 24);
	out_buffer[ 2 ] = (uint8_t) (mem_addr >> 16);
	out_buffer[ 3 ] = (uint8_t) (mem_addr >>  8);
	out_buffer[ 4 ] = (uint8_t) (mem_addr >>  0);

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

	iResult = my_send( s, out_buffer, (int) size + 9, 0 );
	if( iResult < 0 )
	{
		perror( "EMU_MEM_WRITE send()" );
		goto done;
	}
	else if( iResult != ((int) size + 9) )
	{
		pout( "EMU_MEM_WRITE send() only sent", iResult );
		goto done;
	}

	iResult = my_recv( s, in_buffer, sizeof in_buffer, 0 );
	if( iResult < 0 )
	{
		perror( "EMU_MEM_WRITE my_recv()" );
		goto done;
	}
	else if( iResult != sizeof in_buffer )
	{
		pout( "EMU_MEM_WRITE unexpected ACK size", iResult );
		goto done;
	}
	else if( in_buffer[ 0 ] != (out_buffer[ 0 ] | EMU_ACK) )
	{
		pout( "EMU_MEM_WRITE wrong ACK", in_buffer[ 0 ] );
		goto done;
	}

	iReturn = 0;

done:
	return iReturn;
}

