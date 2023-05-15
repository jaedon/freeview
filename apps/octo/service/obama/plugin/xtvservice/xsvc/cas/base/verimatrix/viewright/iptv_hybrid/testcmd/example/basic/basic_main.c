/*****************************************************************************/
/*                                                                           */
/*  File:           main.c                                                   */
/*  Description:    Basic CA application to test Verimatrix CA               */
/*                                                                           */
/*****************************************************************************/

/*-------------------------- System Includes --------------------------------*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <hlib.h>

/*-------------------------- Local Includes ---------------------------------*/
#include "io.h"
#include "VMClient.h"
#define ERR_DISABLED         -6667
#define ERR_OFFLINE          -6669

#define DBG_PRINT			HxLOG_Error

/*-------------------------- Local Variables --------------------------------*/
#define SERVERPORT 12686

static char    *instream;
static int     intype = 0;
static char    *outstream;
static char    outtype = 0;
static int     nodecrypt = 0;
static char    *Storefilepath = NULL;
static char    *Storefile = NULL;
static char    *ServerIPaddress = NULL;
static char    *Portnumber = NULL;
static int     ServerPortno = 0;
static char    *pInterfaceAddress = NULL;
static int     initialized_openssl = 0;
static int     bFreeStorefile = 0;
static int     bConfigSuccess = 0;

/*-------------------------- Local Functions --------------------------------*/
static int ProcessArgs(int argc, char **argv);
static int InitVerimatrixCA(void **pContext);
static int InitVCASConfigure();
static void usage(char *cmd);
static void InitOpenssl();
static void UnInitOpenssl();

void vmx_vr_iptvh_testcmd_ExampleJoinBasic(void *pContext, unsigned char *pfname_in, unsigned char *pfname_out, unsigned char  *pucIpBootAddress, unsigned short usBootPort, unsigned char *pucVmxStore, eVcasVersion_t eVcasVersion, int bOnlineMode)
{
	int result = 0;
	int ecount = 0;
	int pcount = 0;
	int iBytesRead;
	unsigned char pBuffer[TS_PACKET_SIZE];

	DBG_PRINT("[%s:%d]\n", __FUNCTION__, __LINE__);

	if( pContext == NULL )
	{
		DBG_PRINT("[%s:%d] pContext : error \n", __FUNCTION__, __LINE__);
		return;
	}

	if( pfname_in == NULL )
	{
		DBG_PRINT("[%s:%d] pfname_in : error \n", __FUNCTION__, __LINE__);
		return;
	}

	if( pfname_out == NULL )
	{
		DBG_PRINT("[%s:%d] pfname_out : error \n", __FUNCTION__, __LINE__);
		return;
	}

	if( pucIpBootAddress == NULL )
	{
		DBG_PRINT("[%s:%d] pucIpBootAddress : error \n", __FUNCTION__, __LINE__);
		return;
	}

	if( pucVmxStore == NULL )
	{
		DBG_PRINT("[%s:%d] pucVmxStore : error \n", __FUNCTION__, __LINE__);
		return;
	}

	if(bOnlineMode == 0) // case offline
	{
		DBG_PRINT("[%s:%d] bOnlineMode : error \n", __FUNCTION__, __LINE__);
		return;
	}

      ServerIPaddress = pucIpBootAddress;
	Storefilepath = pucVmxStore;
	Portnumber = usBootPort;

	intype = IOFILE;
	instream = pfname_in;
	outtype = IOFILE;
	outstream = pfname_out;

	pInterfaceAddress = NULL;

	// print
	DBG_PRINT("[%s:%d] ================\n", __FUNCTION__, __LINE__);

	result = InitInput(instream, intype, pInterfaceAddress);

	if ( result )
	{
		DBG_PRINT("[%s:%d] InitInput : error(%d)\n", __FUNCTION__, __LINE__, result);
		return result;
	}

	result = InitOutput(outstream, outtype, pInterfaceAddress);
	if ( result )
	{
		DBG_PRINT("[%s:%d] InitOutput : error(%d)\n", __FUNCTION__, __LINE__, result);
		return result;
	}



//	InitOpenssl( );


	/* Decryption will be disabled until the request for the             */
	/* EMM Decryption Key succeeds. There is currently no API            */
	/* to check for this state. One solution would be for the main       */
	/* decrypt loop to continue if we see this error rather than exit.   */
	/* Also note that this error code is defined above.  Future          */
	/* versions may have different codes, or alternate methods           */
	/* for handling this condition.                                      */

	/* Start processing input stream */
	while(StreamInput(pBuffer))
	{
//		if (!nodecrypt)
		{
			static int cnt = 0;
			result = VMDecryptStreamData(pContext,pBuffer,TS_PACKET_SIZE,&iBytesRead);
			DBG_PRINT("cnt[%d] - result(%d), iBytesRead(%d)\n", ++cnt, result, iBytesRead);

		}
		if ( result < 0 ) {
			switch( result )
			{
				case ERR_DISABLED:
					DBG_PRINT("[%s:%d] VMDecryptStreamData : ERR_DISABLED error \n", __FUNCTION__, __LINE__);
					if (!(ecount++ % 100)) { DBG_PRINT("\n***Disabled***\n\n");fflush(stdout);}
					continue;
				case ERR_OFFLINE:
					DBG_PRINT("[%s:%d] VMDecryptStreamData : ERR_OFFLINE error \n", __FUNCTION__, __LINE__);
					if (!(ecount++ % 100)) { DBG_PRINT("\n***Offline***\n\n");fflush(stdout);}
					continue;
				default: /* Fatal Error */
					DBG_PRINT("[%s:%d] VMDecryptStreamData : Decode error\n", __FUNCTION__, __LINE__);
					fprintf(stderr, "Decode Error %d\n", result );
					return;
			}
		}
		else ecount = 0;
		StreamOutput(pBuffer);
		pcount++;
		/* print a '.' every 1000 packets if we are receiving mcast */
		if (!(pcount % 1000) && intype == IOMCAST) { DBG_PRINT(".");fflush(stdout);}
	}


	CloseInput();
	CloseOutput();

	VMDestroyContext(pContext);
	#if 0
	UnInitOpenssl();
	if ( bFreeStorefile && Storefile ) free( Storefile );
	return;
	#endif
}

#if 0
/*---------------------------------------------------------------------------*/
/* main() - simple packet by packet decrypt                                  */
/*          NOTE: in the real world you may want to handle an MTU or more    */
/*          just pass in a larger # of ts packets                            */
/*---------------------------------------------------------------------------*/
int main(int argc, char **argv)
{
   int result = 0;
   int pcount = 0;
   int ecount = 0;
   void *pContext = NULL;
   int iBytesRead;
   unsigned char pBuffer[TS_PACKET_SIZE];

   if ( (result = ProcessArgs(argc, argv)) ) return result;
   if ( (result = InitVCASConfigure()) ) return result;
   if ( (result = InitInput(instream, intype, pInterfaceAddress)) ) return result;
   if ( (result = InitOutput(outstream, outtype, pInterfaceAddress)) ) return result;

   /* If openssl is shared, we must init it before invoking the client. */
   /* If openssl is not shared we do nothing. */
   InitOpenssl( );

   result = InitVerimatrixCA(&pContext);
   if ( result && (result != ERR_OFFLINE) )
      return result;

   /* Decryption will be disabled until the request for the             */
   /* EMM Decryption Key succeeds. There is currently no API            */
   /* to check for this state. One solution would be for the main       */
   /* decrypt loop to continue if we see this error rather than exit.   */
   /* Also note that this error code is defined above.  Future          */
   /* versions may have different codes, or alternate methods           */
   /* for handling this condition.                                      */

   /* Start processing input stream */
   while(StreamInput(pBuffer))
   {
      if (!nodecrypt)
         result = VMDecryptStreamData(pContext,pBuffer,TS_PACKET_SIZE,&iBytesRead);
      if ( result < 0 ) {
          switch( result )
          {
          case ERR_DISABLED:
              if (!(ecount++ % 100)) { DBG_PRINT("\n***Disabled***\n\n");fflush(stdout);}
              continue;
          case ERR_OFFLINE:
              if (!(ecount++ % 100)) { DBG_PRINT("\n***Offline***\n\n");fflush(stdout);}
              continue;
          default: /* Fatal Error */
              fprintf(stderr, "Decode Error %d\n", result );
              return -1;
          }
      }
      else ecount = 0;
      StreamOutput(pBuffer);
      pcount++;
      /* print a '.' every 1000 packets if we are receiving mcast */
      if (!(pcount % 1000) && intype == IOMCAST) { DBG_PRINT(".");fflush(stdout);}
   }

   CloseInput();
   CloseOutput();

   VMDestroyContext(pContext);

   UnInitOpenssl();
   if ( bFreeStorefile && Storefile ) free( Storefile );
   return result;
}
#endif

/*----------------------------------------------------------------------------*/
/* InitializeOpenssl() - Perform required openssl initialization              */
/*----------------------------------------------------------------------------*/
#ifdef  SHARE_OPENSSL
   #include <pthread.h>
   #if 1
      /** !!! BEST TO USE THESE INCLUDES !!! **/
      #include <openssl/crypto.h>
      #include <openssl/ssl.h>
      #include <openssl/evp.h>
      #include <openssl/err.h>
      #include <openssl/engine.h>
   #else
      extern void OPENSSL_add_all_algorithms_conf(void);
      extern void OpenSSL_add_all_digests(void);
      extern void OpenSSL_add_all_ciphers(void);
      extern int SSL_library_init(void);
      extern void SSL_load_error_strings(void);
      extern void CRYPTO_set_locking_callback(void (*func)(int,int,const char *,int));

#if OPENSSL_VERSION_NUMBER >= 0x10000000L
      typedef struct crypto_threadid_st
      {
      void *ptr;
      unsigned long val;
      } CRYPTO_THREADID;
      extern int CRYPTO_THREADID_set_callback(void (*threadid_func)(CRYPTO_THREADID *));
      void ERR_remove_thread_state(const CRYPTO_THREADID *tid);
#else
      extern void CRYPTO_set_id_callback(unsigned long (*func)(void));
      extern void ERR_remove_state(unsigned long pid);
#endif
      extern void EVP_cleanup(void);
      extern void CRYPTO_cleanup_all_ex_data(void);
      extern void ERR_free_strings(void);
      extern void ENGINE_cleanup(void);
      #define OpenSSL_add_all_algorithms OPENSSL_add_all_algorithms_conf
      #define CRYPTO_NUM_LOCKS  50
      #define CRYPTO_LOCK        1
   #endif

   static pthread_mutex_t lock_cs[CRYPTO_NUM_LOCKS];
   static void mutex_setup()
   {
      int i;
      for (i=0; i<CRYPTO_NUM_LOCKS; i++)
         pthread_mutex_init(&(lock_cs[i]),NULL);
   }
   static void mutex_destroy()
   {
      int i;
      for (i=0; i<CRYPTO_NUM_LOCKS; i++)
         pthread_mutex_destroy(&(lock_cs[i]));
   }
   void my_locking_callback(int mode, int type, const char *file, int line)
   {
      if (mode & CRYPTO_LOCK)
         pthread_mutex_lock(&(lock_cs[type]));
      else
         pthread_mutex_unlock(&(lock_cs[type]));
   }


#if OPENSSL_VERSION_NUMBER >= 0x10000000L
   static void my_thread_id_cb( CRYPTO_THREADID * id )
   {
      CRYPTO_THREADID_set_numeric( id, (unsigned long)pthread_self() );
   }
#else
   static unsigned long my_thread_id_cb()
   {
      return (unsigned long)pthread_self();
   }
#endif

   static void InitOpenssl( )
   {
      if ( initialized_openssl )
          return;
      mutex_setup();
      OpenSSL_add_all_algorithms();
      OpenSSL_add_all_digests();
      OpenSSL_add_all_ciphers();
      SSL_library_init();
      SSL_load_error_strings();
      CRYPTO_set_locking_callback( my_locking_callback );
#if OPENSSL_VERSION_NUMBER >= 0x10000000L
      CRYPTO_THREADID_set_callback( my_thread_id_cb );
#else
      CRYPTO_set_id_callback( my_thread_id_cb );
#endif
      initialized_openssl = 1;
   }


   static void UnInitOpenssl()
   {
      CRYPTO_set_locking_callback(NULL);
      mutex_destroy();
      EVP_cleanup();
      CRYPTO_cleanup_all_ex_data();
#if OPENSSL_VERSION_NUMBER < 0x10000000L
      ERR_remove_state(0);
#else
      ERR_remove_thread_state(NULL);
#endif
      ERR_free_strings();
      ENGINE_cleanup();
      initialized_openssl = 0;
   }
#else

   /* Openssl is not shared, so libvmclient owns the initialization. */
   static void InitOpenssl( )
   {
       initialized_openssl = 1;
   }
   static void UnInitOpenssl()
   {
       initialized_openssl = 0;
   }

#endif
/*-------------------------------------------------------------------------------*/
/* InitVCASConfigure() - setup the environment for Verimatrix client to interact */
/*                    with the VCAS Server.                                      */
/*-------------------------------------------------------------------------------*/
static int InitVCASConfigure()
{
	if(NULL == Storefilepath)
		Storefile = "./Verimatrix.store";
	else
	{
		int len = strlen(Storefilepath) + strlen("/Verimatrix.store");
		Storefile = (char *)malloc((len*sizeof(char))+1);
		strcpy(Storefile,Storefilepath);
		strcat(Storefile, "/Verimatrix.store");
		bFreeStorefile = 1;
	}

	if(NULL == ServerIPaddress)
	{
		DBG_PRINT("\nPlease provide VCAS server ip address\n");
		return -1;
	}

	if(NULL == Portnumber)
		ServerPortno = SERVERPORT;
	else
		ServerPortno = atoi(Portnumber);

	return 0;

}


void * cfg_thread( void * arg )
{
   do {
      sleep( 60 );
      DBG_PRINT("Cfg Thread calling VMConfig\n");
   } while( VMConfig( Storefile, ServerIPaddress, ServerPortno, 2, 2, 0 ) );

   VMLogVersion();
   while( 0 != VMUpdateChannelKeys(NULL) )
      sleep( 60 );
   return 0;
}

void StartConfigThread()
{
   pthread_t t;
   int res = pthread_create( &t, NULL, cfg_thread, (void *)0 );
   if ( 0 == res )
      DBG_PRINT("created cfg thread\n");
   else
      DBG_PRINT("failed to create cfg thread\n");
}

/*----------------------------------------------------------------------------*/
/* InitVerimatrixCA() - startup the Verimatrix client                         */
/*----------------------------------------------------------------------------*/
static int InitVerimatrixCA(void **pContext)
{
   int result = 0;
   char idbuf[ 128 ];
   unsigned int idsize = 0;

   DBG_PRINT(" Storefile-> %s\n",Storefile);
   DBG_PRINT(" ServerIPaddress-> %s\n", ServerIPaddress);
   DBG_PRINT(" Server Port number-> %d\n", ServerPortno);

   if ( 0 == GetUniqueIdentifier( idbuf, sizeof(idbuf), &idsize ) )
   {
      DBG_PRINT("Got ClientID as '%s'\n", idbuf );
   }
   else
   {
      DBG_PRINT("GetUniqueIdentifier Failed\n");
   }

VMSetErrorLevel(NULL, 5);
   if ( VMConfig( Storefile, ServerIPaddress, ServerPortno, 2, 2, 0 ) != 0 ) {
      /* VMConfig failed, this is a stopping failure.
      // Middleware app can not proceed with Mutual
      // Authentication mode until this basic VRClient
      // step is successful.  */
      DBG_PRINT( "VMConfig failed, middleware must handle error accordingly\n" );
      StartConfigThread();
      //return -1;
   }
   else
   {
      bConfigSuccess = 1;
   }
   *pContext =  VMCreateContext(NULL,1);
   if ( NULL == *pContext )
   {
      DBG_PRINT("Failed to create decryption context\n");
      return -1;
   }

   VMLogVersion();
   result = VMResetStream( *pContext );
   if (result) fprintf(stderr, "VMResetStream() failed with result: %d\n", result);
   return result;
}



/*----------------------------------------------------------------------------*/
/* ProcessArgs() - processes command line args for file or mcast i/o          */
/*----------------------------------------------------------------------------*/
static int ProcessArgs(int argc, char **argv)
{
   int c;

   if (argc < 3) {
      usage(argv[0]); return -1;
   }

   // parse the command options
   while ((c = getopt(argc, argv, "hnm:a:i:o:r:s:c:p:")) != -1)
        switch (c) {
        case 'i':
         instream = optarg;
         intype = IOFILE;
        break;
        case 'o':
         outstream = optarg;
         outtype = IOFILE;
        break;
        case 'm':
         instream = optarg;
         intype = IOMCAST;
        break;
        case 'r':
         outstream = optarg;
         outtype = IOMCAST;
        break;
        case 'n':
         nodecrypt++;
        break;
        case 'a':
         pInterfaceAddress = optarg;
        break;
	case 's':
		Storefilepath = optarg;
		break;
	case 'p':
		Portnumber = optarg;
		break;
	case 'c':
		ServerIPaddress = optarg;
		break;
        case '?':
        case 'h':
         usage(argv[0]);
         return -1;
        }
   return 0;
}

/*----------------------------------------------------------------------------*/
/* usage() - usage description                                                */
/*----------------------------------------------------------------------------*/
static void usage(char *cmd)
{
   DBG_PRINT("usage %s [-i input_file -m mcastaddr -o output_file -r mcastaddr -n]\n", cmd );
   DBG_PRINT("where:\n");
   DBG_PRINT("    -p port number of the VCAS configuration server\n");
   DBG_PRINT("    -c IP address of the VCAS configuration server\n");
   DBG_PRINT("    -s path of the Store file\n");
   DBG_PRINT("    -a adapter IP address on local host for stream i/o (for multi-nic)\n");
   DBG_PRINT("    -i input_file\n");
   DBG_PRINT("    -m mcastaddr is ip:port to receive encrypted stream\n");
   DBG_PRINT("    -o output_file\n");
   DBG_PRINT("    -r mcastaddr is ip:port to retransmit decrypted stream\n");
   DBG_PRINT("    -n skip decryption\n");
   DBG_PRINT("examples:\n");
   DBG_PRINT("    '%s -m 235.1.1.1:8208 -o out.mpg'    will decrypt a stream into a file\n",cmd);
   DBG_PRINT("    '%s -i rtes.mpg -o out.mpg'          will decrypt a file into a file\n",cmd);
   DBG_PRINT("    '%s -m 235.1.1.1:8208'               will decrypt a stream and not save\n",cmd);
   DBG_PRINT("    '%s -m 235.1.1.1:8208 -r 224.1.2.1:2000'  will decrypt a stream and retransmit\n",cmd);
   DBG_PRINT("    '%s -c 10.0.249.23 -p 12686 -s /root/stb will contact to 10.0.249.23 VCAS server on its 1234 port and save the .store file in the /root/stb location\n",cmd);
}

/* end of main.c */
