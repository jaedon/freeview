/***************************************************************************
 *     Copyright (c) 2003-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: server.c $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 4/28/09 2:41p $
 *
 * Module Description:
 *   The source code for the emulator server.
 *
 * Revision History:
 *
 ***************************************************************************/
#include <errno.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "server.h"
#include "berr.h"
#include "bdbg.h"
#include "bstd_defs.h"
#include "bkni.h"

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
#define EMU_COMMAND            18

#define SERVER_VERSION_ID      "Emulator_Server/1"

/* helper macros */
#define pout(s) do { printf( \
	"%s line %d errno %d:" s "\n", __FILE__, __LINE__, errno ); \
} while( 0 )

BDBG_MODULE(SERVER);

/* data types */
typedef struct BEMU_P_Server_Context
{
	/* Black magic */
	uint32_t               ulBlackMagic;

	BEMU_Server_Settings   stSettings;

} BEMU_P_Server_Context;


/* Default settings. */
static const BEMU_Server_Settings s_stServerSettings =
{
	9999 /* ulPort  used with socket connection */
};

/* Private functions */
static int BEMU_P_Send
	( int   s, 
	  void *buf, 
	  int   len, 
	  int flag )
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

static int BEMU_P_Recv
	( int   s,
	  void *buf,
	  int   len,
	  int   flags )
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


void BEMU_Server_GetSettings
	( struct BEMU_Settings *pstSettings, 
	  BEMU_Server_Settings *pstServerSettings)
{
	BDBG_ENTER(BEMU_Server_GetSettings);

	if (pstServerSettings)
	{
		*pstServerSettings = s_stServerSettings;
	}

	BEMU_GetSettings(pstSettings);

	BDBG_LEAVE(BEMU_Server_GetSettings);
}

BERR_Code BEMU_Server_Open
	 ( BEMU_Server_Handle         *phEmuServer, 
	   const struct BEMU_Settings *pstSettings, 
	   const BEMU_Server_Settings *pstServerSettings)
{
	BERR_Code err = BERR_SUCCESS;
    fd_set master;   /* master file descriptor list*/
    fd_set read_fds; /* temp file descriptor list for select()*/
    struct sockaddr_in myaddr;     /* server address*/
    struct sockaddr_in remoteaddr; /* client address*/
    int fdmax;        /* maximum file descriptor number*/
    int listener;  /* listening socket descriptor*/
    int newfd;        /* newly accept()ed socket descriptor*/
    int nbytes;
    int yes=1;        /* for setsockopt() SO_REUSEADDR, below*/
    uint32_t addrlen;
    int i;
	int bConnected = 0;
	uint32_t client_addr = 0;
	uint32_t client_num = 0;
	BEMU_P_Server_Context *pEmuServer;
	BEMU_Handle hEmu;

	BDBG_ENTER(BEMU_Server_Open);

	BDBG_ASSERT(pstSettings);
	BDBG_ASSERT(pstServerSettings);

	pEmuServer = (BEMU_P_Server_Context *)BKNI_Malloc(sizeof(BEMU_P_Server_Context));
	if (pEmuServer == NULL)
	{
		BDBG_ERR(("Failed ot allocate memory."));
		err = BERR_OUT_OF_SYSTEM_MEMORY;
		return err;
	}
	
	err = BEMU_Open(&hEmu, pstSettings);
	if (err != BERR_SUCCESS)
	{
		BDBG_ERR(("Failed top open emulator"));
		BKNI_Free(pEmuServer);
		return err;
	}

	printf("Version %s.\n", SERVER_VERSION_ID);

	FD_ZERO(&master);    /* clear the master and temp sets*/
    FD_ZERO(&read_fds);

	if( (listener = socket( PF_INET, SOCK_STREAM, 0 )) < 0 )
	{
		pout( "socket()" );
		BEMU_Close(hEmu);
		BKNI_Free(pEmuServer);
		return err;
	}

    /* lose the "address already in use" error message*/
    if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes,
                   sizeof(int)) == -1) {
        perror("setsockopt");
        goto fail_close;
    }

	myaddr.sin_family = AF_INET;
	myaddr.sin_port = htons( pstServerSettings->ulPort );
	myaddr.sin_addr.s_addr = htonl( INADDR_ANY );

	if( bind( listener, (struct sockaddr *) &myaddr, sizeof myaddr ) < 0 )
	{
		pout( "bind()" );
		goto fail_close;
	}

	if( listen( listener, SOMAXCONN ) < 0 )
	{
		perror( "listen()" );
		goto fail_close;
	}
	
	puts( "Waiting for connection..." );

    /* add the listener to the master set*/
    FD_SET(listener, &master);

    /* keep track of the biggest file descriptor*/
    fdmax = listener; /* so far, it's this one*/

	for( ;; )
	{
		unsigned char buffer[ 9 ];
		uint32_t addr;
		int size;

		int stop_server = 0;
		int stop_client = 0;
		uint32_t data[1024] = {0};

		char acPath[241];
		uint32_t ulCmd;
		int32_t ilData[1024] = {0};


        read_fds = master; /* copy it*/
        if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) 
		{
            /*perror("select");*/
            if(errno == EINTR)
		    {
				continue;
		    }
		    else
		    {
	        	goto fail_close;
		    }
        }

		/*puts("activities detected");*/
        /* run through the existing connections looking for data to read*/
        for(i = 0; i <= fdmax; i++)
        {
            if (FD_ISSET(i, &read_fds))
            { /* we got one!!*/
                if (i == listener)
                {
                    /* handle new connections*/
                    /*puts("new connection...");*/
                    addrlen = sizeof(remoteaddr);
                    if ((newfd = accept(listener, (struct sockaddr *)&remoteaddr, &addrlen)) == -1)
                    {
                        perror("accept");
                    } 
					else
                    {
						/*printf("client_num = %d, bConnect = %d, client_addr = 0x%x, remote_addr= 0x%x\n", client_num,bConnected, client_addr, remoteaddr.sin_addr.s_addr);*/
						/* to prevent a different host trying to connect */
#ifdef BCU_TBX_MODE
#if 0
						if( bConnected && (client_addr != remoteaddr.sin_addr.s_addr) )
						{
							close( newfd );
							continue;
						}
#endif						
#endif
						if( !bConnected )
						{/* The reason to reset system is to prevent the following GISB access to IKOS,
							from timeout error, which might terminate the server prematurely!
							Only reset chip for the first successful connection from the fresh new host */
#if 0							
							memc32_init();
#endif
						}
						
						BEMU_P_Send(newfd, (char *)data, 1, 0); /* ack*/

						client_addr = remoteaddr.sin_addr.s_addr;
						bConnected = 1;
						client_num ++;

                        FD_SET(newfd, &master); /* add to master set*/
                        if (newfd > fdmax)
                        {   /* keep track of the maximum*/
                            fdmax = newfd;
                        }
                        printf("New connection from %s on "
                                "socket %d\n", inet_ntoa(remoteaddr.sin_addr), newfd);
                    }
                } 
				else 
				{
                    /* handle data from a client*/
					if( (nbytes = BEMU_P_Recv( i, buffer, 1, 0 )) < 1 )
					{
						printf( "client at socket %d hang up.\n", i );
						if( --client_num == 0 )
						{
							client_addr = 0;
							bConnected = 0;
						}
                        close(i); /* bye!*/
                        FD_CLR(i, &master); /* remove from master set*/
						continue;
					}

					/* the real data service */
					switch( buffer[ 0 ] )
					{
					case EMU_STOP_SERVER:
						stop_server = 1;
					case EMU_STOP_CLIENT:
						stop_client = 1;
						break;
					case EMU_REG_READ:
						if( BEMU_P_Recv( i, &buffer[ 1 ], 4, 0 ) < 4 )
						{
							pout( "EMU_REG_READ recv()" );
							stop_client = 1;
							break;
						}
						addr =
							((unsigned long) buffer[ 1 ] << 24) |
							((unsigned long) buffer[ 2 ] << 16) |
							((unsigned long) buffer[ 3 ] <<  8) |
							((unsigned long) buffer[ 4 ] <<  0);

						err = BEMU_RegisterRead(hEmu, addr, &data[0]);
						if (err == BERR_SUCCESS)
						{
							printf( "EMU_REG_READ address: 0x%08X\n", addr);
						}
						else
						{
							printf( "EMU_REG_READ addr 0x%08x out of range!\n", addr);
						}

						buffer[ 0 ] |= EMU_ACK;
						buffer[ 1 ] = (unsigned char) (data[0] >> 24);
						buffer[ 2 ] = (unsigned char) (data[0] >> 16);
						buffer[ 3 ] = (unsigned char) (data[0] >>  8);
						buffer[ 4 ] = (unsigned char) (data[0] >>  0);
						if( BEMU_P_Send( i, buffer, 5, 0 ) < 5 )
						{
							pout( "EMU_REG_READ send()" );
							stop_client = 1;
						}
						break;
					case EMU_REG_WRITE:
						if( BEMU_P_Recv( i, &buffer[ 1 ], 8, 0 ) < 8 )
						{
							pout( "EMU_REG_WRITE recv()" );
							stop_client = 1;
							break;
						}
						addr =
							((unsigned long) buffer[ 1 ] << 24) |
							((unsigned long) buffer[ 2 ] << 16) |
							((unsigned long) buffer[ 3 ] <<  8) |
							((unsigned long) buffer[ 4 ] <<  0);
						data[0] =
							((unsigned long) buffer[ 5 ] << 24) |
							((unsigned long) buffer[ 6 ] << 16) |
							((unsigned long) buffer[ 7 ] <<  8) |
							((unsigned long) buffer[ 8 ] <<  0);

						err = BEMU_RegisterWrite(hEmu, addr, data[0]);
						if (err == BERR_SUCCESS)
						{
							printf( "EMU_REG_WRITE address: 0x%08X\n" ,addr);
						}
						else
						{
							printf( "EMU_REG_WRITE addr 0x%08x out of range!\n", addr);
						}

						buffer[ 0 ] |= EMU_ACK;
						if( BEMU_P_Send( i, buffer, 1, 0 ) < 1 )
						{
							pout( "EMU_REG_WRITE send()" );
							stop_client = 1;
						}
						break;
					case EMU_MEM_READ:
						if( BEMU_P_Recv( i, &buffer[ 1 ], 8, 0 ) < 8 )
						{
							pout( "EMU_MEM_READ recv()" );
							stop_client = 1;
							break;
						}
						addr =
							((unsigned long) buffer[ 1 ] << 24) |
							((unsigned long) buffer[ 2 ] << 16) |
							((unsigned long) buffer[ 3 ] <<  8) |
							((unsigned long) buffer[ 4 ] <<  0);
						size =
							((unsigned long) buffer[ 5 ] << 24) |
							((unsigned long) buffer[ 6 ] << 16) |
							((unsigned long) buffer[ 7 ] <<  8) |
							((unsigned long) buffer[ 8 ] <<  0);

						err = BEMU_MemoryRead(hEmu, addr, size, (uint8_t *)data);
						if (err == BERR_SUCCESS)
						{
							buffer[ 0 ] |= EMU_ACK;
							printf("EMU_MEM_READ: addr = 0x%x, size = 0x%x\n", addr, size );
						}
						else
						{
							buffer[ 0 ] = EMU_NAK;
							printf("EMU_MEM_READ: addr = 0x%x, size = 0x%x failed!\n", addr, size );
						}
						
						if( BEMU_P_Send( i, buffer, 1, 0 ) < 1 )
						{
							pout( "EMU_MEM_READ send() ack" );
							stop_client = 1;
							break;
						}
						if( BEMU_P_Send( i, data, size, 0 ) < size )
						{
							pout( "EMU_MEM_READ send() data" );
							printf( "BEMU_P_Send sent < %d bytes due to the error.\n", size );
							stop_client = 1;
						}
						break;
						
					case EMU_MEM_WRITE:
						if( BEMU_P_Recv( i, &buffer[ 1 ], 8, 0 ) < 8 )
						{
							pout( "EMU_MEM_WRITE recv()" );
							stop_client = 1;
							break;
						}
						addr =
							((unsigned long) buffer[ 1 ] << 24) |
							((unsigned long) buffer[ 2 ] << 16) |
							((unsigned long) buffer[ 3 ] <<  8) |
							((unsigned long) buffer[ 4 ] <<  0);
						size =
							((unsigned long) buffer[ 5 ] << 24) |
							((unsigned long) buffer[ 6 ] << 16) |
							((unsigned long) buffer[ 7 ] <<  8) |
							((unsigned long) buffer[ 8 ] <<  0);

						if( BEMU_P_Recv( i, data, size, 0 ) < size )
						{
							pout( "EMU_MEM_WRITE recv() data" );
							stop_client = 1;
							break;
						}

						err = BEMU_MemoryWrite(hEmu, addr, size, (uint8_t *)data);
						if (err == BERR_SUCCESS)
						{
							buffer[ 0 ] |= EMU_ACK;
							printf( "EMU_MEM_WRITE: addr = 0x%x, size = 0x%x, data = 0x%x\n", addr, size, *data );
						}
						else
						{
							buffer[ 0 ] = EMU_NAK;
							printf( "EMU_MEM_WRITE: addr = 0x%x, size = 0x%x failed!\n", addr, size );
						}
						
						if( BEMU_P_Send( i, buffer, 1, 0 ) < 1 )
						{
							pout( "EMU_MEM_WRITE send() ack" );
							stop_client = 1;
						}
						break;

					case EMU_COMMAND:						
						if( BEMU_P_Recv( i, &buffer[ 1 ], 8, 0 ) < 8 )
						{
							pout( "EMU_COMMAND recv()" );
							stop_client = 1;
							break;
						}
						ulCmd =
							((unsigned long) buffer[ 1 ] << 24) |
							((unsigned long) buffer[ 2 ] << 16) |
							((unsigned long) buffer[ 3 ] <<  8) |
							((unsigned long) buffer[ 4 ] <<  0);
						size =
							((unsigned long) buffer[ 5 ] << 24) |
							((unsigned long) buffer[ 6 ] << 16) |
							((unsigned long) buffer[ 7 ] <<  8) |
							((unsigned long) buffer[ 8 ] <<  0);

						/* receive as 8-bit values */
						if( BEMU_P_Recv( i, ilData, size, 0 ) < size )
						{
							pout( "EMU_COMMAND recv() data" );
							stop_client = 1;
							break;
						}

						/* BEMU_Command expects 32-bit values */
						err = BEMU_Command(hEmu, ulCmd, size/sizeof(uint32_t), ilData);
						if (err == BERR_SUCCESS)
						{
							buffer[ 0 ] |= EMU_ACK;
							printf( "EMU_COMMAND: ulCmd = 0x%x, size = 0x%x, data = 0x%x\n", ulCmd, size, *ilData );
						}
						else
						{
							buffer[ 0 ] = EMU_NAK;
							printf( "EMU_COMMAND: ulCmd = 0x%x, size = 0x%x failed!\n", ulCmd, size );
						}
						
						if( BEMU_P_Send( i, buffer, 1, 0 ) < 1 )
						{
							pout( "EMU_COMMAND send() ack" );
							stop_client = 1;
						}
						break;
#if 0						
					case EMU_TIMEOUT:
						if( BEMU_P_Recv( i, &size, sizeof size, 0 ) < (int)sizeof size )
						{
							pout( "EMU_TIMEOUT recv() size" );
							stop_client = 1;
						}

						BEMU_Timeout(hEmu, size);

						buffer[ 0 ] |= EMU_ACK;
						if( BEMU_P_Send( i, buffer, 1, 0 ) < 1 )
						{
							pout( "EMU_TIMEOUT send() ack" );
							stop_client = 1;
						}
						break;
#endif						
					case EMU_INIT_CAPTURE:
						
						if( BEMU_P_Recv( i, &buffer[ 1 ], 8, 0 ) < 8 )
						{
							pout( "EMU_INIT_CAPTURE recv() cmd and size" );
							stop_client = 1;
							break;
						}
						ulCmd =
							((unsigned long) buffer[ 1 ] << 24) |
							((unsigned long) buffer[ 2 ] << 16) |
							((unsigned long) buffer[ 3 ] <<  8) |
							((unsigned long) buffer[ 4 ] <<  0);
						size =
							((unsigned long) buffer[ 5 ] << 24) |
							((unsigned long) buffer[ 6 ] << 16) |
							((unsigned long) buffer[ 7 ] <<  8) |
							((unsigned long) buffer[ 8 ] <<  0);

						if( size >= (int)sizeof acPath )
						{
							pout( "EMU_INIT_CAPTURE recv() size is wrong" );
							stop_client = 1;
							break;
						}

						if( BEMU_P_Recv( i, acPath, size, 0 ) < size )
						{
							pout( "EMU_INIT_CAPTURE recv() path" );
							stop_client = 1;
						}
						
						acPath[size + 1] = '\0';
#if 0						
						printf( "EMU_INIT_CAPTURE(iCapCmd = %d, size = %d, path = %s)\n",\
							ulCmd, size, acPath );
#else
						printf("EMU_INIT_CAPTURE's acPath is ignored. Capture data is in server directory.\n");
#endif				

						/* start to init VEC's capture output data */
						err = BEMU_Command(hEmu, ulCmd, 0, NULL);
						if (err == BERR_SUCCESS)
						{
							buffer[ 0 ] |= EMU_ACK;
						}
						else
						{
							buffer[ 0 ] = EMU_NAK;
						}
						
						if( BEMU_P_Send( i, buffer, 1, 0 ) < 1 )
						{
							pout( "EMU_INIT_CAPTURE send() ack" );
							stop_client = 1;
						}
						break;

					case EMU_START_CAPTURE:
						/* start to capture VEC's output data */
						err = BEMU_Command(hEmu, EMU_START_CAPTURE, 0, NULL);
						if (err == BERR_SUCCESS)
						{
							buffer[ 0 ] |= EMU_ACK;
						}
						else
						{
							buffer[ 0 ] = EMU_NAK;
						}

						if( BEMU_P_Send( i, buffer, 1, 0 ) < 1 )
						{
							pout( "EMU_START_CAPTURE send() ack" );
							stop_client = 1;
						}
						break;

					case EMU_STOP_CAPTURE:
						/* stop to capture VEC's output data */
						err = BEMU_Command(hEmu, EMU_START_CAPTURE, 0, NULL);
						if (err == BERR_SUCCESS)
						{
							buffer[ 0 ] |= EMU_ACK;
						}
						else
						{
							buffer[ 0 ] = EMU_NAK;
						}
						if( BEMU_P_Send( i, buffer, 1, 0 ) < 1 )
						{
							pout( "EMU_STOP_CAPTURE send() ack" );
							stop_client = 1;
						}
						break;
					default:
						stop_client = 1;
					}/* end of switch*/

				}/* end of else: Handle data from client*/
			}/* end of if FD_ISSET*/

			if( stop_client )
			{
				stop_client = 0;
				if( --client_num == 0 )
				{
					client_addr = 0;
					bConnected = 0;
				}

				if( shutdown( i, 2 ) < 0 )
				{
					pout( "shutdown(cs)" );
				}

				if( close( i ) < 0 )
				{
					pout( "close(cs)" );
				}
                FD_CLR(i, &master); /* remove from master set*/

				printf( "Socket %d is disconnected.\n", i );
			}
		}/* end of for( i <= fd_max; )*/


		if( stop_server )
		{
			break;
		}
	}/* end of infinite for(;;)*/


fail_close:
	if( close( listener ) < 0 )
	{
		pout( "close()" );
	}
	puts( "Server is stopped." );

	BEMU_Close(hEmu);

	*phEmuServer = (BEMU_Server_Handle)pEmuServer;
	
	puts( "Done." );

	BDBG_LEAVE(BEMU_Server_Open);

	return err;
}


BERR_Code BEMU_Server_Close(BEMU_Server_Handle hEmuServer)
{
	BEMU_P_Server_Context *pEmuServer;
	
	BDBG_ENTER(BEMU_Server_Close);

	BEMU_SERVER_P_GET_CONTEXT(hEmuServer, pEmuServer);
	BKNI_Free(pEmuServer);

	BDBG_LEAVE(BEMU_Server_Close);

	return BERR_SUCCESS;
}

/* end of file */
