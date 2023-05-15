/***************************************************************************
 *     Copyright (c) 2003-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: client.c $
 * $brcm_Revision: Hydra_Software_Devel/5 $
 * $brcm_Date: 10/29/10 7:30p $
 *
 * Module Description:
 *   The source code for the emulator client.
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/appframework/src/linuxemu/client/client.c $
 * 
 * Hydra_Software_Devel/5   10/29/10 7:30p hongtaoz
 * SW7425-9: fixed multi-thread re-entry problem that caused intermitent
 * reg/mem behaviors; added support for user specified server port
 * number;
 * 
 * Hydra_Software_Devel/4   10/15/10 3:08p hongtaoz
 * SW7425-1, SW7425-10: added MEMC1 support to the emulation framework;
 * 
 * Hydra_Software_Devel/3   9/24/10 5:20p hongtaoz
 * SW7425-9, SW7425-10: emulation isr task needs to watch out bad pointer
 * in case nexus creates the task before it inits BINT;
 * 
 * Hydra_Software_Devel/2   5/8/09 10:59a jessem
 * PR 54881: Cleaned up compile warnings in BEMU_Client_Close
 * 
 * Hydra_Software_Devel/4   4/28/09 3:16p jessem
 * PR 54380: Changed all references to IKOS to EMULATION, EMU or BEMU.
 * 
 * Hydra_Software_Devel/3   10/17/07 2:23p jessem
 * PR 36129: Added delay after call to IKOS_Client_AcquireMutex in
 * register read/write and memory read/write functions.
 * 
 * Hydra_Software_Devel/2   10/16/07 3:49p jessem
 * PR 36129: Cleanup
 * 
 * Hydra_Software_Devel/1   10/16/07 3:15p jessem
 * PR 36129: Initial version. This supercedes bchipif.c.
 ***************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "bstd.h"
#include "bkni.h"
#include "bkni_multi.h"
#include "btst_kni.h"
#include "bchp_hif_cpu_intr1.h"
#include "bint_plat.h"
#include "bdbg.h"

#include "pagelib.h"
#include "client.h"
#include "mutex.h"

BDBG_MODULE(client);

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
#define EMU_COMMAND		       18

#define SERVER_VERSION_ID       "Hydra_Software_Devel/17"

/* to protect the shared socket used by all threads of the client */
static BEMU_Client_MutexHandle g_hSocketMutex;

/***********************************
  if on real set-top box
 */
#ifdef SETTOPBOX/* to run on real set-top box */

#include <stdio.h>
#include <unistd.h>
#include <stdarg.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <signal.h>
#include <time.h>
#include "systeminit.h"
#include "memmgr.h"
#include "bchp_common.h"
#include "bchp_sun_top_ctrl.h"
#include "bstd.h"

BSystem_Info g_stSysInfo;
bool         g_bFail;
#endif

/***********************************
  if real IKOS simulation
 */
#ifdef TIPSIM
#include <testbench.h>
#include <host_api.h>

#define EMU_PORT "9999"

#define pout(string, integer) { fprintf(stderr, "ERROR: %s %d: %s: %d\n", string, integer, __FILE__, __LINE__ ); } 

/*
#define pout(s) do { fprintf( stderr, \
	"%s line %d errno %d:" s "\n", __FILE__, __LINE__, errno ); \
*/
} while( 0 )

/*============= Modules List ===============================
	add/remove the modules according to emulator's capacity;
	if the module is not compiled in the emulator, GISB access
	timeout might terminate the server.
 */
#define IS_VALID_ADDR( addr ) ( \
	/* GFX */\
	(( addr >= 0x100c0000 ) && ( addr < 0x100c23ff )) ||\
	/* BVN_FRONT */\
	(( addr >= 0x10100000 ) && ( addr < 0x10109fff )) ||\
	/* BVN_BACK */\
	(( addr >= 0x10140000 ) && ( addr < 0x101458ff )) ||\
	/* VIDEO_ENC */\
	(( addr >= 0x10180000 ) && ( addr < 0x101827ff )) ||\
	/* VDECS_TOP */\
	(( addr >= 0x101c0000 ) && ( addr < 0x101c2fff )) ||\
	/* MiniTitanPlus1 */\
	(( addr >= 0x10300000 ) && ( addr < 0x10360bff )) ||\
	/* MiniTitanPlus2 */\
	(( addr >= 0x10380000 ) && ( addr < 0x103e0bff )) ||\
	/* Xpt */\
	(( addr >= 0x10200000 ) && ( addr < 0x1027ffff )) ||\
	/* SUNDRY */\
	(( addr >= 0x10400000 ) && ( addr < 0x1040bfff )) \
)

/***********************************
  else stub server on 'usul' or real stb
 */
#else

#define EMU_PORT 9999
#define pout(string, integer) { fprintf(stderr, "ERROR: %s %d: %s: %d\n", string, integer, __FILE__, __LINE__ ); } 

/*
#define pout(s) do { printf( \
	"%s line %d errno %d:" s "\n", __FILE__, __LINE__, errno ); \
} while( 0 )
*/

#define IS_VALID_ADDR( addr ) 1

#endif

#define SOCKET_ERROR -1

/* Globals */
static int s;

static int my_recv
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
			pout("recv failed", result);
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

static int my_send
(
	int s, 
	void *buf, 
	int len, 
	int flag
)
{
	int total = 0;		  /* how many bytes we've sent */
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


int BEMU_Client_OpenSocket
	( const char *hostname )
{
	int iError = 0;
	unsigned char in_buffer[ 5 ];
	char *ptr = strchr(hostname, ':');

	struct sockaddr_in dest_addr;	/* will hold the destination addr */

	s = socket(PF_INET, SOCK_STREAM, 0); 
	if( s == -1 )
	{
		perror("socket() error");
		goto done;
	}

	if(ptr) /* with user specified port number */
	{
		*ptr++ = '\0';
	}

	dest_addr.sin_family = AF_INET; 		 /* host byte order */
	dest_addr.sin_port = htons(ptr? atoi(ptr) : EMU_PORT);	 /* short, network byte order */
	dest_addr.sin_addr.s_addr = inet_addr(hostname);
	memset(dest_addr.sin_zero, '\0', sizeof dest_addr.sin_zero);

	/* don't forget to error check the connect()! */
	iError = connect(s, (struct sockaddr *)&dest_addr, sizeof(struct sockaddr));
	if( iError == SOCKET_ERROR )
	{
		perror("connect() error");
		goto done;
	}

	/* get Ack */
	iError = recv( s, in_buffer, 1, 0 );
	if( iError <= 0 )
	{
		fprintf(stderr, "Connection is rejected by server. Another host may be connecting with it\n" );
		return 1;
	} 
	else
	{
		fprintf(stderr, "Connected with the server @ %s\n", hostname);
	}

done:
	return iError;
}

int BEMU_Client_CloseSocket
	( void )
{
	int iError = 0;
	char c ;
	
	fprintf(stderr, "Sending terminate messages for client and server\n");
	/* This flag is use to shutdown the server.  When done
	 * with the emulator set this c=EMU_STOP_SERVER, and re-run it to shutdown the
	 * server.	Don't forget to set it back to EMU_STOP_CLIENT */

	c = EMU_STOP_CLIENT;

	/* This mutex is to protect the shared socket used by threads of the client! */
	if (0 != BEMU_Client_AcquireMutex(g_hSocketMutex))
	{
		fprintf(stderr, "Failed to acquire mutex in BEMU_Client_CloseSocket\n");
		return SOCKET_ERROR;
	}
    
	iError = my_send( s, &c, 1, 0 );
	if( iError != 1 )
	{
		perror("send() error");
		goto done ;
	}

	c = EMU_STOP_SERVER;

	iError = my_send( s, &c, 1, 0 );
	if( iError != 1 )
	{
		perror("send() error");
		goto done ;
	}

	iError = 0;
done:
	BEMU_Client_ReleaseMutex(g_hSocketMutex);
	return iError;
}

static void BEMU_Client_Interrupt_Handler
	( BINT_Handle hInt, 
	  uint32_t    data )
{
	/* Call BINT_isr */	
	BKNI_EnterCriticalSection();
	BINT_Isr(hInt, data);
	BKNI_LeaveCriticalSection();
}

void BEMU_Client_Interrupt_Monitor
	( BTST_TaskHandle  hTask,
	  void            *pContext )
{
	
#if 0
	uint32_t ulL1IntReg = BCHP_HIF_CPU_INTR1_INTR_W0_STATUS;
	uint32_t ulData;
#endif

	BSTD_UNUSED(hTask);

	while(1)
	{
#if 0
		uint32_t i;
		
		BEMU_Client_ReadRegister(ulL1IntReg, &ulData);
		if (ulData & ( (BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_BVNF_CPU_INTR_3_SHIFT) | 
					   (BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_BVNF_CPU_INTR_2_SHIFT) | 
					   (BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_BVNF_CPU_INTR_1_SHIFT) | 
					   (BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_BVNF_CPU_INTR_0_SHIFT) |
					   (BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_BVNB_CPU_INTR_SHIFT)	|
					   (BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_VEC_CPU_INTR_SHIFT) ) )
		{
			
			BEMU_Client_Interrupt_Handler((BINT_Handle)pContext, ulData);
		}
#else	
		/* nexus might create this task earlier than BINT being init */
		if(pContext)
		{
			BEMU_Client_Interrupt_Handler((BINT_Handle)pContext, 0xc);
		}
#endif
		BEMU_Client_Sleep(1000);
	}
}

int BEMU_Client_Open
	( void )
{
	return BEMU_Client_CreateMutex(&g_hSocketMutex);
}

int BEMU_Client_Close
	( void )
{
	BEMU_Client_DestroyMutex(g_hSocketMutex);

	return 0;
}

int BEMU_Client_ReadRegister
	( uint32_t  reg_addr, 
	  uint32_t *pData )
{
	int iReturn = 0;
	int iResult;
	char out_buffer[ 5 ];
	unsigned char in_buffer[ 5 ];
	
	/* NOTE: page wb will call client memory write which acquires mutex; so
	   this function should be outside of AcquireMutex! */
	BEMU_Client_PageWriteBack();
	reg_addr |= 0x10000000;

	out_buffer[ 0 ] = EMU_REG_READ;

	out_buffer[ 1 ] = (uint8_t) (reg_addr >> 24);
	out_buffer[ 2 ] = (uint8_t) (reg_addr >> 16);
	out_buffer[ 3 ] = (uint8_t) (reg_addr >>  8);
	out_buffer[ 4 ] = (uint8_t) (reg_addr >>  0);

	/* This mutex is to protect the shared socket used by threads of the client! */
	if (0 != BEMU_Client_AcquireMutex(g_hSocketMutex))
	{
		fprintf(stderr, "Failed to acquire mutex in BEMU_Client_ReadRegister\n");
		return 0;
	}
    
	iResult = my_send( s, out_buffer, sizeof out_buffer, 0 );
	if( iResult < (int)sizeof(out_buffer) )
	{
		pout("BEMU_REG_READ send() only sent", iResult);
		goto done;
	}

	iResult = my_recv( s, (char *)in_buffer, sizeof in_buffer, 0 );
	if( iResult < 0 )
	{
		pout("BEMU_REG_READ recv(): ", iResult);
		goto done;
	}
	else if( iResult != (int)sizeof(in_buffer) )
	{
		pout("BEMU_REG_READ unexpected ACK size", iResult);
		goto done;
	}
	else if( in_buffer[ 0 ] != (out_buffer[ 0 ] | EMU_ACK) )
	{
		pout("BEMU_REG_READ wrong ACK", in_buffer[ 0 ]);
		BDBG_ERR(("RegAddr = 0x%x, Val = 0x%x", reg_addr,
			((uint32_t) in_buffer[ 1 ] << 24) |
			((uint32_t) in_buffer[ 2 ] << 16) |
			((uint32_t) in_buffer[ 3 ] <<  8) |
			((uint32_t) in_buffer[ 4 ] <<  0)));
		goto done;
	}

	*pData =
		((uint32_t) in_buffer[ 1 ] << 24) |
		((uint32_t) in_buffer[ 2 ] << 16) |
		((uint32_t) in_buffer[ 3 ] <<  8) |
		((uint32_t) in_buffer[ 4 ] <<  0);

	iReturn = 0;

done:
	BEMU_Client_ReleaseMutex(g_hSocketMutex);

	return iReturn;
}

int BEMU_Client_WriteRegister
	( uint32_t reg_addr, 
	  uint32_t data )
{
	int iReturn = SOCKET_ERROR;
	int iResult;
	char out_buffer[ 9 ];
	char in_buffer[ 1 ];

	BEMU_Client_PageWriteBack();
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

	/* This mutex is to protect the shared socket used by threads of the client! */
	if (0 != BEMU_Client_AcquireMutex(g_hSocketMutex))
	{
		fprintf(stderr, "Failed to acquire mutex in BEMU_Client_WriteRegister\n");
		return iReturn;
	}
    
	iResult = my_send( s, out_buffer, sizeof out_buffer, 0 );
	if( iResult < 0 )
	{
		pout("BEMU_REG_WRITE send(): ", iResult);
		goto done;
	}
	else if( iResult != sizeof out_buffer )
	{
		pout("BEMU_REG_WRITE send() only sent", iResult);
		goto done;
	}

	iResult = my_recv( s, in_buffer, sizeof in_buffer, 0 );
	if( iResult < 0 )
	{
		perror("BEMU_REG_WRITE my_recv()");
		goto done;
	}
	else if( iResult != sizeof in_buffer )
	{
		pout("BEMU_REG_WRITE unexpected ACK size", iResult);
		goto done;
	}
	else if( in_buffer[ 0 ] != (out_buffer[ 0 ] | EMU_ACK) )
	{
		pout("BEMU_REG_WRITE wrong ACK", out_buffer[ 0 ]);
		BDBG_ERR(("RegAddr = 0x%x, data = 0x%x", reg_addr, data));
		goto done;
	}

	iReturn = 0;

done:
	BEMU_Client_ReleaseMutex(g_hSocketMutex);

	return iReturn;
}

int BEMU_Client_ReadMemory
	( uint32_t mem_addr, 
	  void *data, 
	  size_t size )
{
	int iReturn = SOCKET_ERROR;
	int iResult;
	char out_buffer[ 9 ];
	unsigned char in_buffer[ 4097 ];
	size_t ss;

	if( size & 3 )
	{
		pout("BEMU_MEM_READ non-dword size: ", size);
		return iReturn;
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

	/* This mutex is to protect the shared socket used by threads of the client! */
	if (0 != BEMU_Client_AcquireMutex(g_hSocketMutex))
	{
		fprintf(stderr, "Failed to acquire mutex in BEMU_Client_ReadMemory\n");
		return iReturn;
	}
    
	iResult = my_send( s, out_buffer, sizeof out_buffer, 0 );
	if( iResult < 0 )
	{
		pout("BEMU_MEM_READ send(): ", iResult);
		goto done;
	}
	else if( iResult != sizeof out_buffer )
	{
		pout("BEMU_MEM_READ send() only sent", iResult);
		goto done;
	}

	iResult = my_recv( s, (char *)in_buffer, (int) size + 1, 0 );
	if( iResult < 0 )
	{
		perror("BEMU_MEM_READ my_recv()");
		goto done;
	}
	else if( iResult != (int)(size + 1) )
	{
		pout("BEMU_MEM_READ unexpected ACK size", iResult);
		goto done;
	}
	else if( in_buffer[ 0 ] != (out_buffer[ 0 ] | EMU_ACK) )
	{
		pout("BEMU_MEM_READ wrong ACK", in_buffer[ 0 ]);
		BDBG_ERR(("mem_addr = 0x%x, size = 0x%x", mem_addr, size));
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
	
	BEMU_Client_ReleaseMutex(g_hSocketMutex);
	return iReturn;
}

int BEMU_Client_WriteMemory
	( uint32_t mem_addr, 
	  void *data, 
	  size_t size )
{
	int iReturn = SOCKET_ERROR;
	int iResult;
	char out_buffer[ 4106 ];
	char in_buffer[ 1 ];
	size_t ss;
	uint32_t uu;

	if( size & 3 )
	{
		pout("BEMU_MEM_WRITE non-dword size: ", size);
		return iReturn;
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

	/* This mutex is to protect the shared socket used by threads of the client! */
	if (0 != BEMU_Client_AcquireMutex(g_hSocketMutex))
	{
		fprintf(stderr, "Failed to acquire mutex in BEMU_Client_WriteMemory\n");
		return iReturn;
	}
    
	iResult = my_send( s, out_buffer, (int) size + 9, 0 );
	if( iResult < 0 )
	{
		pout("BEMU_MEM_WRITE send(): ", iResult);
		goto done;
	}
	else if( iResult != ((int) size + 9) )
	{
		pout("BEMU_MEM_WRITE send() only sent", iResult);
		goto done;
	}

	iResult = my_recv( s, in_buffer, sizeof in_buffer, 0 );
	if( iResult < 0 )
	{
		perror("BEMU_MEM_WRITE my_recv()");
		goto done;
	}
	else if( iResult != sizeof in_buffer )
	{
		pout("BEMU_MEM_WRITE unexpected ACK size", iResult);
		goto done;
	}
	else if( in_buffer[ 0 ] != (out_buffer[ 0 ] | EMU_ACK) )
	{
		pout("BEMU_MEM_WRITE wrong ACK", in_buffer[ 0 ]);
		BDBG_ERR(("mem_addr = 0x%x, size = 0x%x", mem_addr, size));
		goto done;
	}

	iReturn = 0;

done:
	
	BEMU_Client_ReleaseMutex(g_hSocketMutex);
	return iReturn;
}

int BEMU_Client_Command
	( uint32_t  ulCmd, 
	  int32_t   ilSize, 
	  int32_t  *pilData )
{
	int iReturn = SOCKET_ERROR;
	int iResult;
	char out_buffer[ 4106 ];
	char in_buffer[ 1 ];
	int32_t ilVal;
	size_t size = ilSize * sizeof(int32_t);
	size_t i;

	out_buffer[ 0 ] = EMU_COMMAND;
	
	out_buffer[ 1 ] = (uint8_t) (ulCmd >> 24);
	out_buffer[ 2 ] = (uint8_t) (ulCmd >> 16);
	out_buffer[ 3 ] = (uint8_t) (ulCmd >>  8);
	out_buffer[ 4 ] = (uint8_t) (ulCmd >>  0);

	out_buffer[ 5 ] = (uint8_t) (size >> 24);
	out_buffer[ 6 ] = (uint8_t) (size >> 16);
	out_buffer[ 7 ] = (uint8_t) (size >>  8);
	out_buffer[ 8 ] = (uint8_t) (size >>  0);

	for( i=0; i<size; i+=sizeof(uint32_t))
	{
		ilVal = *(pilData++);

		out_buffer[ i +  9 ] = (uint8_t) (ilVal >> 24);
		out_buffer[ i + 10 ] = (uint8_t) (ilVal >> 16);
		out_buffer[ i + 11 ] = (uint8_t) (ilVal >>  8);
		out_buffer[ i + 12 ] = (uint8_t) (ilVal >>  0);
	}

	/* This mutex is to protect the shared socket used by threads of the client! */
	if (0 != BEMU_Client_AcquireMutex(g_hSocketMutex)) 	
	{
		fprintf(stderr, "Failed to acquire mutex in BEMU_Client_WriteMemory\n");
		return iReturn;
	}
    
	/* send out as 8-bit values instead of 32-bit values */
	iResult = my_send( s, out_buffer, (int) size + 9, 0 );
	if( iResult < 0 )
	{
		pout("BEMU_COMMAND send(): ", iResult);
		goto done;
	}
	else if( iResult != ((int) size + 9) )
	{
		pout("BEMU_COMMAND send() only sent", iResult);
		goto done;
	}

	iResult = my_recv( s, in_buffer, sizeof in_buffer, 0 );
	if( iResult < 0 )
	{
		perror("BEMU_COMMAND my_recv()");
		goto done;
	}
	else if( iResult != sizeof in_buffer )
	{
		pout("BEMU_COMMAND unexpected ACK size", iResult);
		goto done;
	}
	else if( in_buffer[ 0 ] != (out_buffer[ 0 ] | EMU_ACK) )
	{
		pout("BEMU_COMMAND wrong ACK", in_buffer[ 0 ]);
		goto done;
	}

	iReturn = 0;

done:
	
	BEMU_Client_ReleaseMutex(g_hSocketMutex);
	return iReturn;
}


int BEMU_Client_InitCapture
	( uint32_t uiCmd, 
	  uint32_t size, 
	  const char *acPath )
{
	int iReturn = SOCKET_ERROR;
	int iResult;
	char out_buffer[ 9 ];
	char in_buffer[ 1 ];

	if( size > 240 )
	{
		pout( "BEMU_INIT_CAPTURE path is too long", size );
		return iReturn;
	}

	out_buffer[ 0 ] = EMU_INIT_CAPTURE;

	out_buffer[ 1 ] = (uint8_t) (uiCmd >> 24);
	out_buffer[ 2 ] = (uint8_t) (uiCmd >> 16);
	out_buffer[ 3 ] = (uint8_t) (uiCmd >>  8);
	out_buffer[ 4 ] = (uint8_t) (uiCmd >>  0);

	out_buffer[ 5 ] = (uint8_t) (size >> 24);
	out_buffer[ 6 ] = (uint8_t) (size >> 16);
	out_buffer[ 7 ] = (uint8_t) (size >>  8);
	out_buffer[ 8 ] = (uint8_t) (size >>  0);

	/* This mutex is to protect the shared socket used by threads of the client! */
	if (0 != BEMU_Client_AcquireMutex(g_hSocketMutex))
	{
		fprintf(stderr, "Failed to acquire mutex in BEMU_Client_CloseSocket\n");
		return SOCKET_ERROR;
	}
    
	iResult = my_send( s, out_buffer, sizeof out_buffer, 0 );
	if( iResult < (int)sizeof out_buffer )
	{
		pout( "BEMU_START_CAPTURE send()", iResult );
		goto done;
	}

	iResult = my_send( s, (char *)acPath, size, 0 );
	if( iResult < (int)sizeof out_buffer )
	{
		pout( "BEMU_START_CAPTURE send()", iResult );
		goto done;
	}

	iResult = my_recv( s, in_buffer, 1, 0 );
	if( iResult < 0 )
	{
		pout( "BEMU_START_CAPTURE recv()", iResult );
		goto done;
	}
	else if( iResult != 1 )
	{
		pout( "BEMU_START_CAPTURE unexpected ACK size", iResult );
		goto done;
	}

	iReturn = 0;

done:
	BEMU_Client_ReleaseMutex(g_hSocketMutex);
	return iReturn;
}

int BEMU_Client_StartCapture( void )
{
	int iReturn = SOCKET_ERROR;
	int iResult;
	char out_buffer[ 1 ];
	char in_buffer[ 1 ];

	out_buffer[ 0 ] = EMU_START_CAPTURE;

	/* This mutex is to protect the shared socket used by threads of the client! */
	if (0 != BEMU_Client_AcquireMutex(g_hSocketMutex))
	{
		fprintf(stderr, "Failed to acquire mutex in BEMU_Client_CloseSocket\n");
		return SOCKET_ERROR;
	}
    
	iResult = my_send( s, out_buffer, sizeof out_buffer, 0 );
	if( iResult < 0 )
	{
		pout( "BEMU_START_CAPTURE send()", iResult );
		goto done;
	}
	else if( iResult != (int)sizeof out_buffer )
	{
		pout( "BEMU_START_CAPTURE send() only sent", iResult );
		goto done;
	}

	iResult = my_recv( s, in_buffer, 1, 0 );
	if( iResult < 0 )
	{
		pout( "BEMU_START_CAPTURE recv()", iResult );
		goto done;
	}
	else if( iResult != 1 )
	{
		pout( "BEMU_START_CAPTURE unexpected ACK size", iResult );
		goto done;
	}

	iReturn = 0;

done:
	BEMU_Client_ReleaseMutex(g_hSocketMutex);
	return iReturn;
}

/*****************************************************************************/
int BEMU_Client_StopCapture( void )
{
	int iReturn = SOCKET_ERROR;
	int iResult;
	char out_buffer[ 1 ];
	char in_buffer[ 1 ];

	out_buffer[ 0 ] = EMU_STOP_CAPTURE;

	/* This mutex is to protect the shared socket used by threads of the client! */
	if (0 != BEMU_Client_AcquireMutex(g_hSocketMutex))
	{
		fprintf(stderr, "Failed to acquire mutex in BEMU_Client_CloseSocket\n");
		return SOCKET_ERROR;
	}
    
	iResult = my_send( s, out_buffer, sizeof out_buffer, 0 );
	if( iResult < 0 )
	{
		pout( "BEMU_STOP_CAPTURE send()", iResult );
		goto done;
	}
	else if( iResult != (int)sizeof out_buffer )
	{
		pout( "BEMU_STOP_CAPTURE send() only sent", iResult );
		goto done;
	}

	iResult = my_recv( s, in_buffer, 1, 0 );
	if( iResult < 0 )
	{
		pout( "BEMU_STOP_CAPTURE recv()", iResult );
		goto done;
	}
	else if( iResult != 1 )
	{
		pout( "BEMU_STOP_CAPTURE unexpected ACK size", iResult );
		goto done;
	}

	iReturn = 0;

done:
	BEMU_Client_ReleaseMutex(g_hSocketMutex);
	return iReturn;
}

/*****************************************************************************/
int BEMU_Client_AdvanceClock( uint32_t nanoseconds )
{
	int iReturn = SOCKET_ERROR;
	int iResult;
	char out_buffer[ 5 ];
	char in_buffer[ 1 ];

	out_buffer[ 0 ] = EMU_TIMEOUT;

	out_buffer[ 1 ] = (uint8_t) (nanoseconds >> 24);
	out_buffer[ 2 ] = (uint8_t) (nanoseconds >> 16);
	out_buffer[ 3 ] = (uint8_t) (nanoseconds >>  8);
	out_buffer[ 4 ] = (uint8_t) (nanoseconds >>  0);

	/* This mutex is to protect the shared socket used by threads of the client! */
	if (0 != BEMU_Client_AcquireMutex(g_hSocketMutex))
	{
		fprintf(stderr, "Failed to acquire mutex in BEMU_Client_AdvanceClock\n");
		return SOCKET_ERROR;
	}
    
	iResult = my_send( s, out_buffer, sizeof out_buffer, 0 );
	if( iResult < 0 )
	{
		pout( "BEMU_Client_AdvanceClock send()", iResult );
		goto done;
	}
	else if( iResult != (int)sizeof out_buffer )
	{
		pout( "BEMU_Client_AdvanceClock send() only sent", iResult );
		goto done;
	}

	iResult = my_recv( s, in_buffer, 1, 0 );
	if( iResult < 0 )
	{
		pout( "BEMU_Client_AdvanceClock recv()", iResult );
		goto done;
	}
	else if( iResult != 1 )
	{
		pout( "BEMU_Client_AdvanceClock unexpected ACK size", iResult );
		goto done;
	}

	iReturn = 0;

done:
	BEMU_Client_ReleaseMutex(g_hSocketMutex);
	return iReturn;
}


/* end of file */
