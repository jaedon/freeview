/*---------------------------------------------------------------------------*/
/* mwapp.c: standalone CWPP stream decryption using emulated components      */
/*---------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include <arpa/inet.h>

#include "mwapp.h"
#include "VMClient.h"
#include "VMCallbackTypes.h"
#include "libsoc.h"

#include "tspktfile.h"
#include "vmparse.h"

#include "vmx_vr_iptvh_testcmd_join.h"

#define DEFAULT_BOOT_PORT 12686
#define DEFAULT_STORE "./Verimatrix.store"
#define LIST_SERVICES

static int InitVerimatrixCA( void **pContext, const char * serverip, int bOffline
                     , int bSetVer, eVcasVersion_t ver );


#ifndef _CA_H_
#if 1
#define DUMPKEYS(x, y) { int i; unsigned char *p = y; HxLOG_Print ("%s()-"x":\t", __FUNCTION__); for (i=0;i<16;i++) HxLOG_Print ("%02x ", *(p+i));HxLOG_Print ("\n"); }
#else
#define DUMPKEYS(x, y)
#endif
#endif

/*----------------------------------------------------------------------------*/
/* usage() - usage description                                                */
/*----------------------------------------------------------------------------*/
static void usage(char *cmd)
{
    HxLOG_Print ("usage %s [options]\n", cmd);
    HxLOG_Print ("options:\n");
    HxLOG_Print ("  -s <Boot Server address as ip:port>\n");
    HxLOG_Print ("  -O <Start client in OFFLINE mode>\n");
    HxLOG_Print ("  -h (print this help)\n");
}

#define DATA_LEN 16

/*---------------------------------------------------------------------------*/
static mw_config_t mw_config;

/*---------------------------------------------------------------------------*/
/* Global functions                                                          */
/*---------------------------------------------------------------------------*/
mw_config_t *mw_app_get_config(void)
{
   return &mw_config;
}

/*---------------------------------------------------------------------------*/
/* Local functions                                                           */
/*---------------------------------------------------------------------------*/
static void parse_cmdline(mw_config_t *g, int argc, char **argv);
static void read_keyfiles(mw_config_t *g);
static void init_key_ladder(mw_config_t *g);
static void init_ca(mw_config_t *g);
static void decrypt_stream(mw_config_t *g);
static int hsm_set_scw_cb(void *pUserParam, descramble_param_t *dscParam);
#ifdef LIST_SERVICES
static void ListAuthorizedChannels( void * pContext );
#endif

/*---------------------------------------------------------------------------*/
/* main() - in a simple app like this we just exit on error in local function*/
/*---------------------------------------------------------------------------*/

void vmx_vr_iptvh_testcmd_ExampleJoinRefsw(void *pContext, unsigned char *pfname_in, unsigned char  *pucIpBootAddress, unsigned short usBootPort, unsigned char *pucVmxStore, eVcasVersion_t eVcasVersion, int bOnlineMode)
{
	if( pContext == NULL )
	{
		HxLOG_Print("[%s:%d] pContext : error \n", __FUNCTION__, __LINE__);
		return;
	}

	if( pfname_in == NULL )
	{
		HxLOG_Print("[%s:%d] pfname_in : error \n", __FUNCTION__, __LINE__);
		return;
	}

	if( pucIpBootAddress == NULL )
	{
		HxLOG_Print("[%s:%d] pucIpBootAddress : error \n", __FUNCTION__, __LINE__);
		return;
	}

	if( pucVmxStore == NULL )
	{
		HxLOG_Print("[%s:%d] pucVmxStore : error \n", __FUNCTION__, __LINE__);
		return;
	}

	// init g context
	memset(&mw_config, 0, sizeof(mw_config_t));
	mw_config.pCtx = NULL;;
	mw_config.fname_in = NULL;
	mw_config.addr = NULL;
	mw_config.offline = 1;
	mw_config.encAlg = eEncAlg_null;

	// set g context
	mw_config.pCtx = pContext;
      mw_config.fname_in = pfname_in;
      mw_config.addr = pucIpBootAddress;
	mw_config.offline = (bOnlineMode==1?0:1);

	// print
	HxLOG_Print("[%s:%d] ================\n", __FUNCTION__, __LINE__);
	HxLOG_Print("\t pCtx = 0x%08x\n", mw_config.pCtx);
	HxLOG_Print("\t fname_in = %s\n", mw_config.fname_in);
	HxLOG_Print("\t addr = %s\n", mw_config.addr);
	HxLOG_Print("\t offline = %d\n", mw_config.offline);
	HxLOG_Print("\t encAlg = %d\n", mw_config.encAlg);

	decrypt_stream(&mw_config);
}

#if 0
int main(int argc, char **argv)
{
   HxLOG_Print ("\n\n\nMain\n\n\n");
   memset(&mw_config, 0, sizeof(mw_config_t));

   parse_cmdline(&mw_config, argc, argv);

   /* If no test vector file was specified, assume stream processing. */
   init_key_ladder(&mw_config);
	HxLOG_Print ("@@@@@@@@@@ %s, %s, %d, CALL init_ca\n", __FILE__, __FUNCTION__, __LINE__);
   init_ca(&mw_config);
	HxLOG_Print ("@@@@@@@@@@ %s, %s, %d, CALL decrypt_stream\n", __FILE__, __FUNCTION__, __LINE__);
   decrypt_stream(&mw_config);

	HxLOG_Print ("@@@@@@@@@@ %s, %s, %d, EXIT\n", __FILE__, __FUNCTION__, __LINE__);
   return 0;
}
#endif

/*---------------------------------------------------------------------------*/
/* parse_cmdline() -                                                         */
/*---------------------------------------------------------------------------*/
static void parse_cmdline(mw_config_t *g, int argc, char **argv)
{
   if (argc < 3)
   {
	  usage(argv[0]);
	  exit(-1);
   }


    int c;

    g->fname_in = NULL;
    g->addr = NULL;
    g->offline = 0;
    g->encAlg = eEncAlg_null;

    /* parse the command options */
    while ((c = getopt(argc, argv, "Oh:i:s:")) != -1) {
        switch (c) {
            case 'i': 
                g->fname_in = optarg;
                break;

            case 's':
                g->addr = optarg;
                break;

            case 'O':
            	g->offline = 1;
            	break;

            case '?':
            case 'h':
            default:
                usage(argv[0]);
                exit(-1);
        }
    }

}

/*---------------------------------------------------------------------------*/
/* init_key_ladder() - Key ladder must have device key loaded. Normally this */
/*                     is done by a provisioning mechanism, or in some cases */
/*                     through a 'self' provisioning mechanism on boot       */
/*---------------------------------------------------------------------------*/
static void init_key_ladder(mw_config_t *g)
{
   HxLOG_Print("\n\n Before nexus platform init\n\n\n");
   nexus_platform_init();
   /*
   VMXSEC_Init(64);
    */
   /* in a real system the IDotp must be obtained from secure hardware and */
   /* passed into the CA client                                            */
}

/*---------------------------------------------------------------------------*/
/* init_ca() - CA library must be initialized. Normally CA will provision    */
/*             the CWPP keys out-of-band on startup, but since we are        */
/*             emulating, we provision it here                               */
/*---------------------------------------------------------------------------*/
static void init_ca(mw_config_t *g)
{
   void              *pContext;     /* CA access handle */

   /*--- normal client startup ---*/
	int bSetVer = 0;
	eVcasVersion_t eVer     = eVcasVersion_340;

	if (!InitVerimatrixCA( &g->pCtx, g->addr, g->offline, bSetVer, eVer ))
	{
		HxLOG_Print("@@@@@@@@@@ %s, %s, %d, InitVerimatrixCA Failed!\n", __FILE__, __FUNCTION__, __LINE__);
	}
}


// 디버깅 편하게 편의상...
void decrypt_print(unsigned char *pszStr, unsigned char *pucBuf, unsigned long ulLen)
{
#ifdef CONFIG_DEBUG
	int i;

	HxLOG_Print ("\n===================== ECM ==========================\n");;

	HxLOG_Print ("\n");

	for(i = 0 ; i < ulLen ; i++)
	{
		if(i % 16 == 0)
			HxLOG_Print ("\n");

		HxLOG_Print ("%02x ", pucBuf[i]);
	}
	HxLOG_Print ("\n");
#endif
}


/*---------------------------------------------------------------------------*/
/* decrypt_stream() - application example to decrypt stream with key ladder  */
/*                    emulation, uses libca.a                                */
/*---------------------------------------------------------------------------*/
static void decrypt_stream(mw_config_t *g)
{
	TSPKT_IF_FILE   fin;
	unsigned char packet[188];
	int status;
	int result;
	int packetcount = 1;

	HxLOG_Print("- example mwapp.c [%s:%d]\n", __FUNCTION__, __LINE__);

	/* open files */
	tspkt_file_init(&fin);

	if (g->fname_in)
	{
		if (tspkt_file_open(&fin, g->fname_in, "r") < 0)
		{     
			fprintf(stderr, "could not open input file %s\n", g->fname_in);
			HxLOG_Print("- example mwapp.c [%s:%d] file is not open\n", __FUNCTION__, __LINE__);
			HxLOG_Print("- example mwapp.c [%s:%d] file is not open\n", __FUNCTION__, __LINE__);
			HxLOG_Print("- example mwapp.c [%s:%d] file is not open\n", __FUNCTION__, __LINE__);			
			exit(-1);
		}
	}

	/* initialize transport parser */
	__xpt_init_transport();

	/* pass every packet through transport, ECMs go to CA, otherwise packets */
	/* are decrypted by the descrambler (in place) to be written out         */
	while (1)
	{
		if (g->fname_in)
		{
			if (tspkt_file_rcv(&fin, packet) < 0) break;
		}

		status = __xpt_process(packet);
		switch(status) {
			case PARSEFAIL:
			HxLOG_Print("@@@@@@@@@@ %s, %d : PARSEFAIL\n", __FUNCTION__, __LINE__);
			fprintf(stderr, "parse error for file %s\n", g->fname_in);
			exit(-1);
			break;

			case NEWECM:
			HxLOG_Print("@@@@@@@@@@ %s, %d, NEWECM\n",  __FUNCTION__, __LINE__);
			HxLOG_Print("@@@@@@@@@@ %s, %d, CALLING VMSetECM\n", __FUNCTION__, __LINE__);
			HxLOG_Print("@@@@@@@@@@ %s, %d, g->Ctx=0x%x\n", __FUNCTION__, __LINE__, (int) g->pCtx);
			decrypt_print("cmd_ecm", packet, 188);
			result = VMSetECM(g->pCtx, 0, GET_PID(packet), packet, 188);
			if (result)
			{
				fprintf(stderr, "VMSetECM failed with error code: %d\n", result);
			}

			{
				unsigned char rlStb, rlOride, rlStream;
				int i;
				static cnt = 0;
				
				 rlStb = VMGetRatingLevel( g->pCtx, &rlOride, &rlStream );
				
			    HxLOG_Print("Rating Level STB=%u, Stream=%u, Override=%s\n"
	                          , (unsigned)rlStb, (unsigned)rlStream, rlOride?"TRUE":"FALSE" );
				HxLOG_Print("=================================== ecm packet cnt = %d=====================\n", ++cnt);
				HxLOG_Print ("{ \n");
				for(i=0;i<188;i++)
				{
					if(i%16==0)
						HxLOG_Print ("\n");
					HxLOG_Print ("0x%02x,  ", packet[i]);
				}
				HxLOG_Print("\n} \n");
				HxLOG_Print("\n");
			}

			break;

			case ENCRYPTED:
			HxLOG_Print("@@@@@@@@@@ %s, %d : ENCRYPTED\n", __FUNCTION__, __LINE__);
			//                nexus_write_ts_to_playpump(packet, 188); // temp_humandks
			//                xmgr_vmxiptv_cmd_nexus_write_ts_to_playpump(packet, 188); // temp_humandks			
			break;

			/* all other packets are passed through, decrypted */
			default:
//			HxLOG_Print("@@@@@@@@@@ %s, %d : default\n", __FUNCTION__, __LINE__);
			//            	HxLOG_Print("@@@@@@@@@@ %s, %s, %d, packetcount=%d\n", __FILE__, __FUNCTION__, __LINE__, packetcount);
			//                nexus_write_ts_to_playpump(packet, 188); // temp_humandks
			//                xmgr_vmxiptv_cmd_nexus_write_ts_to_playpump(packet, 188); // temp_humandks			
			break;
			}
		packetcount++;
//		HxLOG_Print("@@@@@@@@@@ %s, %d : packetcount = %d\n", __FUNCTION__, __LINE__, packetcount);
	}

	if (g->fname_in)
	{
		tspkt_file_close(&fin);
	}

	tspkt_file_term(&fin);
}

/*---------------------------------------------------------------------------*/
/* cw_cb() - Control words are received from CA via this callback. The       */
/*           callback includes any additional data needed to setup the       */
/*           key ladder, such as the protected CWPP key which must be loaded */
/*           before the protected CW can be derived. Also, if the keys are   */
/*           clear, the eCWtype_clear flag will be passed in rather than     */
/*           a eCWtype_*_cwpp flag.                                          */
/*                                                                           */
/*           Once the control words are received they are passed straight    */
/*           into the key ladder for routing to the transport descrambler    */
/*---------------------------------------------------------------------------*/
static int hsm_set_scw_cb(void *pUserParam, descramble_param_t *dscParam)
{
	HxLOG_Print("@@@@@@@@@@ %s, %s, %d\n", __FILE__, __FUNCTION__, __LINE__);
   /* Get the common config */
   mw_config_t *config = mw_app_get_config();

   /* normally this would consist of a table of supported algorithms that */
   /* are registered with this callback is registered, but for this trivial */
   /* example, we only deal with AES ECB with clear trailing remnant */
   if (dscParam->encAlg == eEncAlg_AESECBT || dscParam->encAlg == eEncAlg_DVBCSA)
   {
	   config->encAlg = dscParam->encAlg;
	   HxLOG_Print("@@@@@@@@@@ %s, %s, %d, ALGORITHM(%d)\n", __FILE__, __FUNCTION__, __LINE__, dscParam->encAlg);

   }
   else
   {
	   HxLOG_Print("@@@@@@@@@@ %s, %s, %d, INVALID ALGORITHM(%d) , EXITING callback\n", __FILE__, __FUNCTION__, __LINE__, dscParam->encAlg);
      return -1;
   }

   return 0;
}

#ifdef LIST_SERVICES
static void ListAuthorizedChannels( void * pContext )
{
    int   nServices = 0;
    int * pServices = NULL;
    int   res;
    int   n;

    res = VMGetAuthorizedServices( pContext, NULL, &nServices );
    if ( 0 != res )
    {
        HxLOG_Print("Failed to get authorized channels (for num)\n");
        return;
    }
    if ( 0 == nServices )
    {
        HxLOG_Print("No authorized channels\n");
        return;
    }
    // allocate buffer for services
    pServices = (int *)malloc( nServices * sizeof(int) );
    if ( NULL == pServices )
    {
        HxLOG_Print("Failed to allocate buffer for %d services\n", nServices);
        return;
    }
    res = VMGetAuthorizedServices( pContext, pServices, &nServices );
    if ( 0 != res )
    {
        HxLOG_Print("Failed to get authorized channels (for list)\n");
        return;
    }
    HxLOG_Print("\n---------------------------------------\n");
    HxLOG_Print("Begin List of %d Services\n", nServices);
    for( n = 0; n < nServices; n++ )
    {
        HxLOG_Print ("   %4d: %d\n", n+1, pServices[ n ]);
    }
    HxLOG_Print("End List of %d Services\n", nServices);
    HxLOG_Print("---------------------------------------\n\n");

}
#endif

static int InitVerimatrixCA( void **pContext, const char * serverip, int bOffline
                     , int bSetVer, eVcasVersion_t ver )
{
   int result = 0;
   char addr[128];
   int port;

   if ( ! IpAddrSplit( serverip, addr, sizeof(addr), &port, NULL, 0, DEFAULT_BOOT_PORT ))
   {
//       HxLOG_Print("Bad Server Address: failed split of '%s'\n", serverip);
       HxLOG_Print("@@@@@@@@@@ %s, %s, %d, Bad Server Address: failed split of '%s'\n", __FILE__, __FUNCTION__, __LINE__, serverip);
       return 0;
   }
//   HxLOG_Print(" Storefile-> %s\n",DEFAULT_STORE );
   HxLOG_Print("@@@@@@@@@@ %s, %s, %d, Storefile-> %s\n", __FILE__, __FUNCTION__, __LINE__, DEFAULT_STORE);
//   HxLOG_Print(" ServerIPaddress-> %s\n", addr );
   HxLOG_Print("@@@@@@@@@@ %s, %s, %d, ServerIPaddress-> %s\n", __FILE__, __FUNCTION__, __LINE__, addr);
//   HxLOG_Print(" Server Port number-> %d\n", port );
   HxLOG_Print("@@@@@@@@@@ %s, %s, %d, Server Port number-> %d\n", __FILE__, __FUNCTION__, __LINE__, port);

   VMSetErrorLevel(NULL, 5);
   if ( bSetVer )
   {
       VMSetVcasCommunicationProtocol( ver );
   }

   if ( !bOffline )
   {
		HxLOG_Print ("@@@@@@@@@@ %s, %s, %d, CALLING VMConfig\n", __FILE__, __FUNCTION__, __LINE__);
       if ( VMConfig( DEFAULT_STORE, addr, port, 1, 1, 0 ) != 0 )
       {
          /* VMConfig failed, this is a stopping failure.
          // Middleware app can not proceed with Mutual
          // Authentication mode until this basic VRClient
          // step is successful.  */
//          HxLOG_Print( "VMConfig failed, middleware must handle error accordingly\n" );
          HxLOG_Print("@@@@@@@@@@ %s, %s, %d, VMConfig failed, middleware must handle error accordingly\n", __FILE__, __FUNCTION__, __LINE__);
          return 0;
       }
   }
   else
   {
		HxLOG_Print("@@@@@@@@@@ %s, %s, %d, CALLING VMConfigOffline\n", __FILE__, __FUNCTION__, __LINE__);
       if ( VMConfigOffline( DEFAULT_STORE ) != 0 )
       {
          /* VMConfig failed, this is a stopping failure.
          // Middleware app can not proceed with Mutual
          // Authentication mode until this basic VRClient
          // step is successful.  */
//          HxLOG_Print( "VMConfigOffline failed\n" );
          HxLOG_Print("@@@@@@@@@@ %s, %s, %d, VMConfigOffline failed\n", __FILE__, __FUNCTION__, __LINE__);
          return 0;
       }
   }

	HxLOG_Print("@@@@@@@@@@ %s, %s, %d, CALLING VMCreateContext\n", __FILE__, __FUNCTION__, __LINE__);
   *pContext =  VMCreateContext(NULL,1);
   if ( NULL == *pContext )
   {
//      HxLOG_Print("Failed to create decryption context\n");
      HxLOG_Print("@@@@@@@@@@ %s, %s, %d, Failed to create decryption context\n", __FILE__, __FUNCTION__, __LINE__);
      return 0;
   }
   else
	      HxLOG_Print("@@@@@@@@@@ %s, %s, %d, CREATED decryption context, g->Ctx=0x%x\n", __FILE__, __FUNCTION__, __LINE__, (int)*pContext);


   int    all_algorithms[1] = { -1 };
	HxLOG_Print("@@@@@@@@@@ %s, %s, %d, CALLING VMRegisterControlWordCallback\n", __FILE__, __FUNCTION__, __LINE__);
   VMRegisterControlWordCallback( *pContext, hsm_set_scw_cb, (void *)NULL, 1, all_algorithms );

   VMLogVersion();
   VMSetErrorLevel(NULL, 4);
	HxLOG_Print("@@@@@@@@@@ %s, %s, %d, CALLING VMResetStream\n", __FILE__, __FUNCTION__, __LINE__);
   result = VMResetStream( *pContext );
   if (result) fprintf(stderr, "VMResetStream() failed with result: %d\n", result);
   else fprintf(stderr, "VMResetStream() success with result: %d\n", result);
   VMSetErrorLevel(NULL, 4);

sleep(1);
#ifdef LIST_SERVICES
   ListAuthorizedChannels( *pContext );
#endif

#if 0
 {
 int vod = 666;
 HxLOG_Print("VOD %d Request %s\n", vod, VMRequestVODKey( *pContext, vod )? "FAIL":"OK");
 }
#endif

   return result? 0 : 1;
}

int IpAddrSplit( const char * ip_and_port
                , char       * addr
                , unsigned     maxaddr
                , int        * port
                , char       * adapter_ip
                , unsigned     max_adapter
                , int          default_port
                )
{
    const char * sep;
    const char * portstr;
    const char * adaptstr;
    unsigned len;

    if ( !ip_and_port || !addr || !port )
        return 0;

    memset( addr, 0, maxaddr );

    if ( adapter_ip )
        memset( adapter_ip, 0, max_adapter );

    portstr = strchr( ip_and_port, ':' );
    adaptstr = strchr( ip_and_port, '@' );

    // Extract port (if there is one) or use default
    if ( !portstr )
    {
        if ( !default_port )
            return 0;
        *port = default_port;
    }
    else
        *port = atoi( portstr+1 );

    // Extract Adapter IP if there is one
    if ( adapter_ip && adaptstr )
    {
        ++adaptstr;
        if ( strlen( adaptstr ) < max_adapter )
            strcpy( adapter_ip, adaptstr );
        else
            return 0;
    }

    // Find end of ip address
    sep = NULL;
    if ( portstr )
        sep = portstr;
    if ( adaptstr && (!sep || (adaptstr < sep)) )
        sep = adaptstr;

    // Determine length of ip address
    if ( !sep )
        len = strlen(ip_and_port);
    else
        len = (unsigned long)sep - (unsigned long)ip_and_port;

    if ( len >= maxaddr )
        return 0;

    // Copy ip address
    strncpy( addr, ip_and_port, len );

    return 1;
}
/* end of mwapp.c */

