#include <stdio.h>
#include <syslog.h>

#include <fcntl.h>

#include <arpa/inet.h> /* for htonl and ntohl */

#include "tlv.h"
#include "tlvlib.h"

#include "toollib.h"
#include "proclib.h"
#include "cert_bridge.h"
#include "ca_cert.h"
#include "ca_cert_impl.h"
#include "cert_scr.h"
#include "cert_dsc.h"
#include "cert_reset.h"
#include "comcmd.h"

/* #define LOCAL_BUFSIZE 4096 */
#define MAXBUFSIZE 16384

int socDSC(U8 xTag, U8 *xpArgs, U32 xArgLen,U8 *xpOutBuf, U32 xBufLen);
int socSCR(U8 xTag, U8 *xpArgs, U32 xArgLen,U8 *xpOutBuf, U32 xBufLen);
int socResetAkl(U32 xArgLen,U8 *xpOutBuf, U32 xBufLen);

/*------------------------------------------------------------------------------
 Process a buffer containing command TLVS that should have been previously 
 validated with tlvChkFirstSeq. For each command its output is put in xpOutBuf.
 Returns the number of bytes written to xpOutBuf if succeeded else <0 if not.
------------------------------------------------------------------------------ */
int procBuf(U8 * xpBuf, U32 xBufLen, U8 *xpOutBuf, U32 xOutBufLen)
{
	U32 i;
	U8 *p;
	U8 *po;
	U8	tag=0;
	U32 valLen;
	U32 lenLen;
	U32 tlvLen;
	int stat;
	int rest;
	int tlvCnt;
	int outCnt;
	
	outCnt=0;
	tlvCnt=0;
	po = xpOutBuf;
	for (i=0, p=xpBuf; i<xBufLen && tag != 0x0e; i+=tlvLen, p+=tlvLen)
	{
		/*extract tag, length and tlvLen */
		rest=xBufLen-i;
		tag=*p;
		tlvGetLen(&valLen,&lenLen,p+1,rest-1);
		tlvLen=1+lenLen+valLen;
			
		/* process function with args and update output buffer pointers */
		stat = procFun(tag, p+1+lenLen, valLen, po, xOutBufLen-outCnt);
		outCnt += stat;
		po += stat;
		
		tlvCnt++;
	}
	
	
	return outCnt;
}

/*------------------------------------------------------------------------------
 Process function with tag xTag and its arguments xArgs TLVs 
 Outputs TLVs in xpOutBuf.
 Returns the number of bytes written to xpOutBuf if succeeded else <0 if not.
------------------------------------------------------------------------------ */
int procFun(U8 xTag, U8 *xpArgs, U32 xArgLen, U8 *xpOutBuf, U32 xBufLen)
{
	U8 tag, *val;
	U32 len;

	U8 tParam[MAXBUFSIZE]; /* temporary buffer to hold response TLVs */
	
	int stat; /* local functions status */
	
	U8 status = 0;/* overall command status 0=all is fine. */
	
	U32 bi=0; /* nb bytes read from xpArgs */
	int bo=0; /* nb bytes written in xpOutBuf */
	TCertResourceHandle certRessource;

	switch(xTag) {

	case TAG_CERT_BRIDGE_ISEMULATOR:
		bo=procCertBridgeIsEmulator(xArgLen,xpOutBuf,xBufLen);
		break;

	case TAG_CERT_BRIDGE_VERSION:
		bo=procCertBridgeVersion(xArgLen,xpOutBuf,xBufLen);
		break;

	case TAG_AKL_XCHG:	
		certLock(&certRessource);
		bo=procAklXchg(xTag,xpArgs,xArgLen,xpOutBuf,xBufLen);
		certUnlock(certRessource);
		
		break;
	
	case TAG_CERT_EXCHANGE:	
		{
			TCertCommand *pCertCmds = NULL;
			TCertStatus op_status;
			size_t xNumOfCommands=0;
			size_t xNumOfProcessedCommands=0;
			
			
			/* arguments processing */
			bi = 0;

			stat = tlvGetTlv(&tag, &len, &val, xpArgs+bi, xArgLen-bi);
			if (stat < 0) {
				bo = procErr(ERR_PARAM_TYPE, xpOutBuf, xBufLen, __FILE__, __LINE__);
				break;
			}
			bi += stat;
			
			if ( tag != TAG_IN ) { /* check tag */
				bo = procErr(ERR_PARAM_TYPE, xpOutBuf, xBufLen, __FILE__, __LINE__);
				break;
			}
			if ((len%sizeof(TCertCommand)) != 0) { /* check length */
				bo = procErr(ERR_PARAM_TYPE, xpOutBuf, xBufLen, __FILE__, __LINE__);
				break;
			}
			if ( bi != xArgLen ) {
				bo = procErr(ERR_PARAM_NUMBER, xpOutBuf, xBufLen, __FILE__, __LINE__);
				break;
			}
			/* end of arguments processing */
			
			/* execute function */
			xNumOfCommands = len/sizeof(TCertCommand);
			
			pCertCmds = (TCertCommand *) val;

			op_status = certLock(&certRessource);
			
			if(op_status != CERT_NO_ERROR) goto cert_exchange_end;
			op_status = certExchange(certRessource, 
			                      xNumOfCommands,
			                      pCertCmds,
			                      &xNumOfProcessedCommands );
			
			certUnlock(certRessource);

			/* end of function execution */
			cert_exchange_end:
			
			/* format response	 */
			bo = 0;
			stat = tlvPutTlv(tParam+bo, MAXBUFSIZE-bo, TAG_STATUS, sizeof(U8),((U8*)&status));
			if( stat < 0 ) { bo = procErr(ERR_UNKNOWN, xpOutBuf, xBufLen, __FILE__, __LINE__); break; }
			bo += stat;
			
			xNumOfProcessedCommands = htonl(xNumOfProcessedCommands);
			stat = tlvPutTlv(tParam+bo, MAXBUFSIZE-bo, TAG_OUT, sizeof(xNumOfProcessedCommands), ((U8*)&xNumOfProcessedCommands));
			if( stat < 0 ) { bo = procErr(ERR_UNKNOWN, xpOutBuf, xBufLen, __FILE__, __LINE__); break; }
			bo += stat;
						
			stat = tlvPutTlv(tParam+bo, MAXBUFSIZE-bo, TAG_OUT, xNumOfCommands*sizeof(TCertCommand), pCertCmds);
			if( stat < 0 ) { bo = procErr(ERR_UNKNOWN, xpOutBuf, xBufLen, __FILE__, __LINE__); break; }
			bo += stat;
			
			stat = tlvPutTlv(xpOutBuf, xBufLen, TAG_CERT_EXCHANGE, bo, tParam);
			if( stat < 0 ) { bo = procErr(ERR_UNKNOWN, xpOutBuf, xBufLen, __FILE__, __LINE__); break; }
			bo = stat;

			break;
		}

	case TAG_CERT_LOCK:
		bo=procCertLock(xArgLen,xpOutBuf,xBufLen);
		break;

	case TAG_CERT_UNLOCK:
		bo=procCertUnlock(xArgLen,xpOutBuf,xBufLen);
		break;

	case TAG_CERT_RAM_TO_RAM:
		bo=procCertRamToRam(xTag,xpArgs,xArgLen,xpOutBuf,xBufLen);
		break;

	case TAG_CERT_SCR:
		bo=socSCR(xTag,xpArgs,xArgLen,xpOutBuf,xBufLen);
		break;

	case TAG_CERT_DSC:
		bo=socDSC(xTag,xpArgs,xArgLen,xpOutBuf,xBufLen);
		break;

	case TAG_CERT_RESET_AKL:
		bo=socResetAkl(xArgLen,xpOutBuf,xBufLen);
		break;

	case TAG_SYSTEM_CMD:
		bo=comSystemCmd(xpArgs,xArgLen,xpOutBuf,xBufLen);
		break;

	case 0x0e: /* END TAG */
		stat = tlvPutTagLen(xpOutBuf, xBufLen, 0x0e, 0);
		if( stat < 0 ) { bo = procErr(ERR_UNKNOWN, xpOutBuf, xBufLen, __FILE__, __LINE__); break; }
		bo = stat;
		break;
		
	default:
		bo = procErr(ERR_COMMAND, xpOutBuf, xBufLen, __FILE__, __LINE__);
		syslog(LOG_ERR, "Unknown function tag: 0x%02x.\n", xTag);
		break;
	} /* end of switch */

	return bo; /* returns the number of bytes written to xpOutBuf */
}

/*------------------------------------------------------------------------------
 Process error
 Writes error TLV in xpOutBuf 
 Returns the number of bytes written to xpOutBuf if succeeded else <0 if not.
------------------------------------------------------------------------------ */
int procErr(U8 xErr, U8 *xpOutBuf, U32 xBufLen, char *xFile, int xLineNb)
{
	int stat;
	syslog(LOG_ERR, "Error 0x%02x while processing buffer @ %s:%d.", xErr, xFile, xLineNb);
	stat = tlvPutTlv(xpOutBuf, xBufLen, TAG_STATUS, 1, &xErr);
	return stat;
}


/*------------------------------------------------------------------------------
 Execute the AKL exchange command
 Returns the number of bytes written to xpOutBuf.
------------------------------------------------------------------------------*/
int procAklXchg(U8 xTag, U8 *xpArgs, U32 xArgLen,U8 *xpOutBuf, U32 xBufLen)
{
	U8 	status=0; 	/* overall command status */
	int stat; 		/* local functions status */
	int bo; 		/* nb bytes written in xpOutBuf */
	U32 bi; 		/* nb bytes read from xpArgs */
	U8  tag, *val;
	U32 len;
	int m,n,p,q,r;

	TCertCmdInput *pCertCmdIn = NULL;
	TCertCmdOutput *pCertCmdOut = NULL;
		
	U8  tBuf[MAXBUFSIZE]; /* buffer to hold results */

	U8  op_status;
	U32 nbBlocks=0;
	U16 nbBlocksProcessed;
	U32 i;
		
	/* arguments processing */
	stat = tlvGetTlv(&tag, &len, &val, xpArgs, xArgLen);
	if (stat < 0)
		return procErr(ERR_PARAM_TYPE, xpOutBuf, xBufLen, __FILE__, __LINE__);
	bi = stat;
		
	if ( tag != TAG_IN ) /* check tag */
		return procErr(ERR_PARAM_TYPE, xpOutBuf, xBufLen, __FILE__, __LINE__);
	if ((len%sizeof(TCertCmdInput)) != 0) /* check length */
		return procErr(ERR_PARAM_TYPE, xpOutBuf, xBufLen, __FILE__, __LINE__);
	if ( bi != xArgLen )
		return procErr(ERR_PARAM_NUMBER, xpOutBuf, xBufLen, __FILE__, __LINE__);
		
	/*  execute function */
	nbBlocks = len/sizeof(TCertCmdInput);
	if( nbBlocks > (sizeof(tBuf) / sizeof(TCertCmdOutput))) 
	{
		syslog(LOG_WARNING, 
			"procFun: warning tag=0x%02X. nbBlocks(=%d) exceeds maximum supported (%d) for output structure. Truncating.",
			xTag, nbBlocks, (sizeof(tBuf) / sizeof(TCertCmdOutput)) );
		nbBlocks = (sizeof(tBuf) / sizeof(TCertCmdOutput));
		status = 1;
	}

	/*  NOTE: this code can be used as a reference for DAL Cert CertExchange implementation */
	pCertCmdIn = (TCertCmdInput *) val;
	pCertCmdOut = (TCertCmdOutput *) tBuf;
	
	for (i=0; i<nbBlocks; i++,pCertCmdIn++,pCertCmdOut++) 
	{
		pCertCmdIn->cert_cmd_maxcycles = ntohl(pCertCmdIn->cert_cmd_maxcycles);

		op_status = certOperation(pCertCmdIn->cert_cmd,
				pCertCmdIn->cert_cmd_maxcycles,
				8*4, /*  din size (bytes) */
				(U8*)pCertCmdIn->cert_data_in, 
				8*4, /*  dout size (bytes) */
				(U8*)pCertCmdOut->cert_data_out, 
				pCertCmdOut->cert_status, 
				&(pCertCmdOut->cmd_cycles));
		pCertCmdOut->cmd_status = op_status;
		pCertCmdOut->rfu = 0;
		pCertCmdOut->cmd_status = htonl(pCertCmdOut->cmd_status); /*  byte-swap U32 value */
		pCertCmdOut->cmd_cycles = htonl(pCertCmdOut->cmd_cycles); /*  byte-swap U32 value */

		if (	( (*(U32*)pCertCmdOut->cert_status) & (*(U32*)pCertCmdIn->cert_status_mask) )
			 != (*(U32*)pCertCmdIn->cert_status_val)
		   )
		{
			i++;
			status += op_status;
			break;
		}
	}
	nbBlocksProcessed = i;
	
	/*  Prepare response	 */
	m=tlvCalcTlvLen(sizeof(U8));	/*  length of status TLV */
	p=nbBlocksProcessed*sizeof(TCertCmdOutput);
	q=tlvCalcTlvLen(p);				/*  length of OUT TLV */
    r=m+q;							/*  Length of Status TLV and OUT TLV */
	n=tlvCalcTagLenLen(r);			/*  length of TAG_AKL_XCHG tag+len */
	bo=n+r;
	stat=tlvPutTagLen(xpOutBuf,xBufLen,TAG_AKL_XCHG,r);
	if (stat<0) 
		return procErr(ERR_PARAM_TYPE, xpOutBuf, xBufLen, __FILE__, __LINE__);
	stat=tlvPutTlv(xpOutBuf+n,xBufLen-n,TAG_STATUS,sizeof(U8),&status);
	if (stat<0) 
		return procErr(ERR_PARAM_TYPE, xpOutBuf, xBufLen, __FILE__, __LINE__);
	r=n+m;
	stat=tlvPutTlv(xpOutBuf+r,xBufLen-r,TAG_OUT,p,tBuf);
	if (stat<0) 
		return procErr(ERR_PARAM_TYPE, xpOutBuf, xBufLen, __FILE__, __LINE__);

	return bo;
}



/* ------------------------------------------------------------------------------ */
/*  Execute the CERT exchange command */
/*  Returns the number of bytes written to xpOutBuf. */
/* ------------------------------------------------------------------------------ */
#ifdef SOC_BRIDGE
int procCertXchg(U8 xTag, U8 *xpArgs, U32 xArgLen,U8 *xpOutBuf, U32 xBufLen)
{
	U8 	status=0; 	/*  overall command status  */
	int stat; 		/*  local functions status */
	int bo; 		/*  nb bytes written in xpOutBuf */
	U32 bi; 		/* nb bytes read from xpArgs */
	U8  tag, *val;
	U32 len;

	TCertCommand *pCertCmds = NULL;
	TCertResourceHandle certRessource;
	TCertStatus op_status;
	size_t xNumOfCommands=0;
	size_t xNumOfProcessedCommands=0;
	
	U8 tBuf[MAXBUFSIZE]; /*  buffer to hold results */
	
	/*  arguments processing */
	stat = tlvGetTlv(&tag, &len, &val, xpArgs, xArgLen);
	if (stat < 0)
		return procErr(ERR_PARAM_TYPE, xpOutBuf, xBufLen, __FILE__, __LINE__);
	bi = stat;
	
	if ( tag != TAG_IN ) /* check tag */
		return procErr(ERR_PARAM_TYPE, xpOutBuf, xBufLen, __FILE__, __LINE__);
	if ((len%sizeof(TCertCommand)) != 0) /* check length */
		return procErr(ERR_PARAM_TYPE, xpOutBuf, xBufLen, __FILE__, __LINE__);
	if ( bi != xArgLen )
		return procErr(ERR_PARAM_NUMBER, xpOutBuf, xBufLen, __FILE__, __LINE__);
	
	/*  execute function */
	xNumOfCommands = len/sizeof(TCertCommand);
	
	pCertCmds = (TCertCommand *) val;
	
	status = certLock(&certRessource);
	if (status != CERT_NO_ERROR) 
	{
		status = certExchange(certRessource, 
	                      xNumOfCommands,
	                      pCertCmds,
	                      &xNumOfProcessedCommands );
	
		certUnlock(certRessource);
	}
	
	/*  Prepare response	 */
	stat = tlvPutTlv(tBuf, sizeof(tBuf), TAG_STATUS, sizeof(U8),((U8*)&status));
	if( stat < 0 ) 
		{ bo = procErr(ERR_UNKNOWN, xpOutBuf, xBufLen, __FILE__, __LINE__); return bo; }
	bo = stat;
	
	xNumOfProcessedCommands = htonl(xNumOfProcessedCommands);
	stat = tlvPutTlv(tBuf+bo, sizeof(tBuf)-bo, TAG_OUT, sizeof(xNumOfProcessedCommands), ((U8*)&xNumOfProcessedCommands));
	if ( stat < 0 ) 
		{ bo = procErr(ERR_UNKNOWN, xpOutBuf, xBufLen, __FILE__, __LINE__); return bo; }
	bo += stat;
				
	stat = tlvPutTlv(tBuf+bo, sizeof(tBuf)-bo, TAG_OUT, xNumOfCommands*sizeof(TCertCommand), pCertCmds);
	if ( stat < 0 ) 
		{ bo = procErr(ERR_UNKNOWN, xpOutBuf, xBufLen, __FILE__, __LINE__); return bo; }
	bo += stat;
	
	stat = tlvPutTlv(xpOutBuf, xBufLen, TAG_CERT_EXCHANGE, bo, tBuf);
	if ( stat < 0 ) 
		{ bo = procErr(ERR_UNKNOWN, xpOutBuf, xBufLen, __FILE__, __LINE__); return bo; }
	bo = stat;

	return bo;
}
#endif
/*  endif SOC_BRIDGE */

	

/* ------------------------------------------------------------------------------ */
/*  Execute the CERT Bridge Is Emulator command */
/*  Returns the number of bytes written to xpOutBuf. */
/* ------------------------------------------------------------------------------ */
int procCertBridgeIsEmulator(U32 xArgLen,U8 *xpOutBuf, U32 xBufLen)
{
	int stat; 		/*  local functions status */
	U8 	status=0; 	/*  overall command status  */
	int bo; 		/*  nb bytes written in xpOutBuf */
	U8  is_emulator;
	int m,n;
		
	/*  arguments processing */
	if (xArgLen!=0)
		return procErr(ERR_PARAM_NUMBER, xpOutBuf, xBufLen, __FILE__, __LINE__);
	
	is_emulator = 0;
	
	/*  Prepare response	 */
	m=2*tlvCalcTlvLen(sizeof(U8));	/*  length of status TLV + OUT TLV (here: m==6) */
	n=tlvCalcTagLenLen(m);			/*  length of TAG_CERT_BRIDGE_ISEMULATOR tag+len (here: n==2) */
	bo=m+n;
	stat=tlvPutTagLen(xpOutBuf,xBufLen,TAG_CERT_BRIDGE_ISEMULATOR,m);
	if (stat<0) 
		return procErr(ERR_PARAM_TYPE, xpOutBuf, xBufLen, __FILE__, __LINE__);
	stat=tlvPutTlv(xpOutBuf+n,xBufLen-n,TAG_STATUS,sizeof(U8),&status);
	if (stat<0) 
		return procErr(ERR_PARAM_TYPE, xpOutBuf, xBufLen, __FILE__, __LINE__);
	n+=stat;
	stat=tlvPutTlv(xpOutBuf+n,xBufLen-n,TAG_OUT,sizeof(U8),&is_emulator);
	if (stat<0) 
		return procErr(ERR_PARAM_TYPE, xpOutBuf, xBufLen, __FILE__, __LINE__);
		
	return bo;
}



/* ------------------------------------------------------------------------------ */
/*  Execute the CERT Bridge Version command */
/*  Returns the number of bytes written to xpOutBuf. */
/* ------------------------------------------------------------------------------ */
int procCertBridgeVersion(U32 xArgLen,U8 *xpOutBuf, U32 xBufLen)
{
	U32 bridge_version;
	int stat; 		/*  local functions status */
	U8 	status=0; 	/*  overall command status  */
	int bo; 		/*  nb bytes written in xpOutBuf */
	int m,n;
	
	/*  arguments processing */
	if (xArgLen!=0) 
		return procErr(ERR_PARAM_NUMBER, xpOutBuf, xBufLen, __FILE__, __LINE__);
	
	/*  execute function */
	bridge_version = htonl(CERT_BRIDGE_VERSION);
	
	/*  Prepare response	 */
	m=2*tlvCalcTlvLen(sizeof(U8));	/*  length of status TLV + OUT TLV (here: m==6) */
	n=tlvCalcTagLenLen(m);			/*  length of CERT_BRIDGE_VERSION tag+len (here: n==2) */
	bo=m+n;
	stat=tlvPutTagLen(xpOutBuf,xBufLen,TAG_CERT_BRIDGE_VERSION,m);
	if (stat<0) 
		return procErr(ERR_PARAM_TYPE, xpOutBuf, xBufLen, __FILE__, __LINE__);
	stat=tlvPutTlv(xpOutBuf+n,xBufLen-n,TAG_STATUS,sizeof(U8),&status);
	if (stat<0) 
		return procErr(ERR_PARAM_TYPE, xpOutBuf, xBufLen, __FILE__, __LINE__);
	n+=stat;
	stat=tlvPutTlv(xpOutBuf+n,xBufLen-n,TAG_OUT,sizeof(U32),&bridge_version);
	if (stat<0) 
		return procErr(ERR_PARAM_TYPE, xpOutBuf, xBufLen, __FILE__, __LINE__);
	
	return bo;
}


/* ------------------------------------------------------------------------------ */
/*  Execute the CERT Lock command */
/*  Returns the number of bytes written to xpOutBuf. */
/* ------------------------------------------------------------------------------ */
int procCertLock(U32 xArgLen,U8 *xpOutBuf, U32 xBufLen)
{
	int   bo; 		    /*  nb bytes written in xpOutBuf */
	int   stat; 		/*  local functions status */
	U8 	  status;   	/*  overall command status  */
	int   m,n;
	
	/*  arguments processing */
	if (xArgLen!=0) 
		return procErr(ERR_PARAM_NUMBER, xpOutBuf, xBufLen, __FILE__, __LINE__);

	status = 0;

	/*  TODO: Call DAL CERT and return status */
	/*  TCertStatus cert_status; */
	/*  cert_status = certLock(); */

	/*  Prepare response */
	m=tlvCalcTlvLen(sizeof(U8));	/*  length of status TLV (here: m==3) */
	n=tlvCalcTagLenLen(m);			/*  length of CERT_LOCK tag+len (here: n==2) */
	stat=tlvPutTagLen(xpOutBuf,xBufLen,TAG_CERT_LOCK,m);
	if (stat<0) 
		return procErr(ERR_PARAM_TYPE, xpOutBuf, xBufLen, __FILE__, __LINE__);
	stat=tlvPutTlv(xpOutBuf+n,xBufLen-n,TAG_STATUS,sizeof(U8),&status);
	if (stat<0) 
		return procErr(ERR_PARAM_TYPE, xpOutBuf, xBufLen, __FILE__, __LINE__);
    bo=n+m;

	return bo;
}



/* ------------------------------------------------------------------------------ */
/*  Execute the RAM-to-RAM command */
/*  Returns the number of bytes written to xpOutBuf. */
/* ------------------------------------------------------------------------------ */
int procCertRamToRam(U8 xTag, U8 *xpArgs, U32 xArgLen,U8 *xpOutBuf, U32 xBufLen)
{
	int  stat; 		 /*  local functions status */
	S8 	 status; 	 /*  overall command status  */
	int  bo; 		 /*  nb bytes written in xpOutBuf */
	int  m,n;
	int  q,s,t;
	U8   tag;
	U32  len;
	U8	 *pVal;
	CERT_RAM_TO_RAM *pR2R;
	DATA_IO xIO;
	U8	 buf[MAXBUFSIZE];
	uint32_t numread;
	

	static int callCnt=0;

	callCnt++;

	memset(&xIO, 0, sizeof(DATA_IO));

	syslog(LOG_DEBUG, "\nprocCertRamToRam call #%d",callCnt); /*  DEBUGGING */
	
	/*  arguments processing */
	/*  There should be exactly 2 TLVs */
	n=tlvChkSeq(xpArgs,xArgLen);
	if (n!=2)
		return procErr(ERR_PARAM_NUMBER, xpOutBuf, xBufLen, __FILE__, __LINE__);

	/*  Check first TLV, which should contain a CERT_RAM_TO_RAM structure */
	stat=tlvGetTlv(&tag,&len,&pVal,xpArgs,xArgLen);
	if ( (stat<0) || (tag!=TAG_RAM_TO_RAM) || len!=sizeof(CERT_RAM_TO_RAM) )
		return procErr(ERR_PARAM_TYPE, xpOutBuf, xBufLen, __FILE__, __LINE__);
	pR2R=(CERT_RAM_TO_RAM*)pVal;
        /* fix U16 fields endianness */
	pR2R->emi = ntohs(pR2R->emi);

	/*  Find offset of 2nd TLV */
	m=tlvCalcTagLenLen(len)+len;
	/*  Check 2nd TLV, which should contain input data */
	stat=tlvGetTlv(&tag,&len,&pVal,xpArgs+m,xArgLen-m);
	if ( (stat<0) || (tag!=TAG_IN) )
		return procErr(ERR_PARAM_TYPE, xpOutBuf, xBufLen, __FILE__, __LINE__);
	
	printf("*******************************************************\n");
	printf("*  Begin of R2R call #%d (EMI=%04x, dir=%d) \n", callCnt, pR2R->emi, pR2R->cmd.r2r.dir);
	printf("*******************************************************\n");
	{ /* CODE adapted from cert -r2r */
		TCsdUnsignedInt8 *dataIn;
		TCsdUnsignedInt8 *dataOut;
		TCsdUnsignedInt8* dataIv;
		TCertKeyPathHandle *pxKeyPathHandle;
		TCsdR2RKeyPathHandle* pxR2rHandle;
		TCsdInitParameters csdInit;
		TCsdTerminateParameters csdTerminateHandle; 
		TCsdStatus csdStatus = CSD_ERROR;
		TCertStatus useKeyStatus = CERT_ERROR;
		uint32_t ivSize;

		/* Initialize the CSD driver. */	
		memset (&csdInit, 0, sizeof(TCsdInitParameters));

		csdStatus=csdInitialize(&csdInit);
	
		if( (csdStatus!=CSD_NO_ERROR) && (csdStatus !=CSD_ERROR_DRIVER_ALREADY_INITIALIZED) )	{
			printf("Error: cannot initialize CSD driver due to error %d!\n", csdStatus); 
			return procErr(ERR_UNKNOWN, xpOutBuf, xBufLen, __FILE__, __LINE__);
		}

		/* Setup the RAM2RAM Handle. */	
		NEXUS_Memory_Allocate(sizeof(TCertKeyPathHandle), NULL, (void *)&pxKeyPathHandle);
		NEXUS_Memory_Allocate(sizeof(TCertKeyPathHandle), NULL, (void *)&pxR2rHandle);

		memset( pxKeyPathHandle, 0, sizeof(TCertKeyPathHandle));
		memset( pxR2rHandle, 0, sizeof(TCertKeyPathHandle));

		pxKeyPathHandle->r2rHandle = pxR2rHandle;
		pxKeyPathHandle->r2rHandle->initHandle = &csdInit;

		ivSize = 0;

		dataIv = pR2R->iv;

		if((pR2R->emi == CSD_Algorithm_M2M_AES_128_CBC_CLEAR) ||
		   (pR2R->emi == CSD_Algorithm_M2M_AES_128_CTR_CLEAR))
			ivSize = 16;
		else if(pR2R->emi == CSD_Algorithm_M2M_TDES_ABA_CBC_CLEAR)
			ivSize = 8;


		dataIn = (TCsdUnsignedInt8 *)pVal;
		dataOut = (TCsdUnsignedInt8 *)&buf;
		
		/* Copy the right amount of data (8 vs 16 bytes) depending on the EMI. */
		numread = len;
		if((pR2R->emi == CSD_Algorithm_M2M_TDES_ABA_ECB_CLEAR) ||
		   (pR2R->emi == CSD_Algorithm_M2M_TDES_ABA_CBC_CLEAR) ) {
			
			if((len % 8) != 0) {
				numread = (len/8)*8;
				printf("R2R data size (%d) must be multiple of 8 (for the moment).", len );
			}
		
		} else	{
			if((len % 16) !=0) {
				numread = (len/16)*16;
				printf("R2R data size (%d) must be multiple of 8 (for the moment).", len);
			}
		}
			
			
		pxKeyPathHandle->r2rHandle->keyHandle = NULL;
		pxKeyPathHandle->dest = NEXUS_SecurityAlgorithmConfigDestination_eMem2Mem;
		pxKeyPathHandle->keySlotEngine = NEXUS_SecurityEngine_eM2m;
	
		if(pR2R->cmd.r2r.dir) {
			if(!pR2R->cmd.r2r.skipusekey)
				useKeyStatus = certUseEncryptionKey(
					pR2R->emi,
					pxKeyPathHandle,
					CERT_KEY_PARITY_UNDEFINED
					);
			else
				useKeyStatus=CERT_NO_ERROR;

			if(useKeyStatus == CERT_NO_ERROR ) {
				csdStatus = csdEncryptData(
				       pR2R->emi,
				       pxKeyPathHandle->r2rHandle,
				       dataIv,
				       ivSize,
				       dataIn,
				       dataOut,
				       numread
				       );	
			}
		}
		else {
			if(!pR2R->cmd.r2r.skipusekey)
				useKeyStatus = certUseDecryptionKey(
					pR2R->emi,
					pxKeyPathHandle,
					CERT_KEY_PARITY_UNDEFINED
					);
			else
				useKeyStatus=CERT_NO_ERROR;

			if(useKeyStatus == CERT_NO_ERROR ) {
				csdStatus = csdDecryptData(
					pR2R->emi,
					pxKeyPathHandle->r2rHandle,
					dataIv,
					ivSize,
					dataIn,
					dataOut,
					numread
					);
			}
				
		}
	
		if( useKeyStatus != CERT_NO_ERROR || csdStatus != CSD_NO_ERROR ) {
			status = -1;
			numread = 0;
		} else {
			status = 0;
		}

		NEXUS_Memory_Free(pxR2rHandle);
		NEXUS_Memory_Free(pxKeyPathHandle);

		csdTerminateHandle.initParam = &csdInit; 
		csdTerminateHandle.keyHandleToRelease= NULL; 
		csdTerminate(&csdTerminateHandle);

	}
	printf("*******************************************************\n");
	printf("*  End of R2R call #%d (EMI=%04x, dir=%d) \n", callCnt, pR2R->emi, pR2R->cmd.r2r.dir);
	printf("*******************************************************\n");

	/* printf("\ncert_bridge: cert status: %d", status); */
	/* if(status==0) */
	/* 	printf("(success)\n"); */
	/* else  */
	/* 	printf("(failure)\n"); */

	fflush(stdout);
	
	
#if (0)	
	/*  DEBUGGING: Display buffer content */
	printf("\ncert_bridge: buf:");
	for (n=0; n<numread; n++)
		printf("%02X",buf[n]);
	printf("\n");
#endif
	
	/*  Prepare response   */
	m=tlvCalcTlvLen(sizeof(U8));	/*  length of status TLV (here: m==3) */
	q=tlvCalcTlvLen(numread);	/*  length of data TLV */
	s=m+q;							/*  length of status and data TLV */
	n=tlvCalcTagLenLen(s);			/*  length of TAG_CERT_RAM_TO_RAM tag+len  */
	bo=n+s;
	
	stat=tlvPutTagLen(xpOutBuf,xBufLen,TAG_CERT_RAM_TO_RAM,s);
	if (stat<0) 
		return procErr(ERR_PARAM_TYPE, xpOutBuf, xBufLen, __FILE__, __LINE__);
	t=stat;
	stat=tlvPutTlv(xpOutBuf+t,xBufLen-t,TAG_STATUS,sizeof(U8),&status);
	if (stat<0) 
		return procErr(ERR_PARAM_TYPE, xpOutBuf, xBufLen, __FILE__, __LINE__);
	t+=stat;
	stat=tlvPutTlv(xpOutBuf+t,xBufLen-t,TAG_OUT,numread,buf);
	if (stat<0) 
		return procErr(ERR_PARAM_TYPE, xpOutBuf, xBufLen, __FILE__, __LINE__);
	
	return bo;
}



/* ------------------------------------------------------------------------------ */
/*  Execute the CERT Unlock command */
/*  Returns the number of bytes written to xpOutBuf. */
/* ------------------------------------------------------------------------------ */
int procCertUnlock(U32 xArgLen,U8 *xpOutBuf, U32 xBufLen)
{
	int   bo; 		    /*  nb bytes written in xpOutBuf */
	int   stat; 		/*  local functions status */
	U8 	  status;   	/*  overall command status  */
	int   m,n;
		
	/*  arguments processing */
	if (xArgLen!=0) 
		return procErr(ERR_PARAM_NUMBER, xpOutBuf, xBufLen, __FILE__, __LINE__);

	status = 0;

	/*  TODO: Call DAL CERT and return status */
	/*  TCertStatus cert_status; */
	/*  cert_status = certUnlock(); */
	
	/*  Prepare response */
	m=tlvCalcTlvLen(sizeof(U8));	/*  length of status TLV (here: m==3) */
	n=tlvCalcTagLenLen(m);			/*  length of CERT_UNLOCK tag+len (here: n==2) */
	stat=tlvPutTagLen(xpOutBuf,xBufLen,TAG_CERT_UNLOCK,m);
	if (stat<0) 
		return procErr(ERR_PARAM_TYPE, xpOutBuf, xBufLen, __FILE__, __LINE__);
	stat=tlvPutTlv(xpOutBuf+n,xBufLen-n,TAG_STATUS,sizeof(U8),&status);
	if (stat<0) 
		return procErr(ERR_PARAM_TYPE, xpOutBuf, xBufLen, __FILE__, __LINE__);
    bo=n+m;

	return bo;
}

/*----------------------------------------------------------------------------*\
|* Execute the DSC command
|* Returns the number of bytes written to xpOutBuf.
\* ---------------------------------------------------------------------------*/
int socDSC(U8 xTag, U8 *xpArgs, U32 xArgLen,U8 *xpOutBuf, U32 xBufLen)
{
	int  stat; 		 /*  local functions status */
	U8 	 status; 	 /*  overall command status  */
	int  bo; 		 /*  nb bytes written in xpOutBuf */
	int  m,n;
	int  q,s,t;
	U8   tag;
	U32  len;
	U8	 *pVal;
	CERT_DSC *pDSC;
	DATA_IO xIO;
	U8	 buf[MAXBUFSIZE];
	U32	 bufCont;
	
	static int callCnt=0;

	callCnt++;
	
	memset(&xIO, 0, sizeof(DATA_IO));
	
	syslog(LOG_DEBUG, "\nsocDSC call #%d",callCnt); /*  DEBUGGING */

	/*  arguments processing */
	/*  There should be exactly 2 TLVs */
	n=tlvChkSeq(xpArgs,xArgLen);
	if (n!=2)
		return procErr(ERR_PARAM_NUMBER, xpOutBuf, xBufLen, __FILE__, __LINE__);
	
	/*  Check first TLV, which should contain a CERT_DSC structure */
	stat=tlvGetTlv(&tag,&len,&pVal,xpArgs,xArgLen);
	if ( (stat<0) || (tag!=TAG_DSC) || (len!=sizeof(CERT_DSC)) )
		return procErr(ERR_PARAM_TYPE, xpOutBuf, xBufLen, __FILE__, __LINE__);
	pDSC=(CERT_DSC*)pVal;
	/* fix U16 fields endianness */
	pDSC->emi = ntohs(pDSC->emi);
	pDSC->vidPid = ntohs(pDSC->vidPid);
	pDSC->audPid = ntohs(pDSC->audPid);
	pDSC->pcrPid = ntohs(pDSC->pcrPid);
	pDSC->ecmPid = ntohs(pDSC->ecmPid);

	/*  Find offset of 2nd TLV */
	m=tlvCalcTagLenLen(len)+len;
	/*  Check 2nd TLV, which should contain input data */
	stat=tlvGetTlv(&tag,&len,&pVal,xpArgs+m,xArgLen-m);
	if ( (stat<0) || (tag!=TAG_IN) )
		return procErr(ERR_PARAM_TYPE, xpOutBuf, xBufLen, __FILE__, __LINE__);
	
	/* dsc parameters */
	xIO.din = pVal;
	xIO.din_size = len;
	xIO.din_len = len;
	xIO.dout = buf;
	xIO.dout_size = MAXBUFSIZE;
	xIO.dout_len = 0;




	syslog(LOG_DEBUG, "pDSC->emi=%04x", pDSC->emi);
	syslog(LOG_DEBUG, "pDSC->vidPid=%04x", pDSC->vidPid);
	syslog(LOG_DEBUG, "pDSC->audPid=%04x", pDSC->audPid);
	syslog(LOG_DEBUG, "pDSC->pcrPid=%04x", pDSC->pcrPid);
	syslog(LOG_DEBUG, "pDSC->ecmPid=%04x", pDSC->ecmPid);
	syslog(LOG_DEBUG, "pDSC->cmd.dsc.skipusekey=%d", pDSC->cmd.dsc.skipusekey);
	syslog(LOG_DEBUG, "pDSC->cmd.dsc.swtch=src/dest=%s", pDSC->cmd.dsc.swtch?"ram/ram":"src/dest");
	syslog(LOG_DEBUG, "pDSC->cmd.dsc.parity=%s", pDSC->cmd.dsc.parity?"even":"odd");

	/* Execute DSC */
	printf("*******************************************************\n");
	printf("*  Begin of DSC call #%d (EMI=%04x) \n", callCnt, pDSC->emi);
	printf("*******************************************************\n");
	stat=cert_dsc_ciph(pDSC,&xIO, NULL); 
	printf("*******************************************************\n");
	printf("*  End of DSC call #%d (EMI=%04x) \n", callCnt, pDSC->emi);
	printf("*******************************************************\n");
	
	bufCont = xIO.dout_len;
	
	if (stat<0)
		status=stat;
	else
		status=0;
	
	syslog(LOG_DEBUG, "socDSC: stat=%d, len=%u, bufCont=%u",
			stat,len,bufCont); /*  DEBUGGING */
	
	/*  Prepare response   */
	m=tlvCalcTlvLen(sizeof(U8));	/*  length of status TLV (here: m==3) */
	q=tlvCalcTlvLen(bufCont);		/*  length of data TLV */
	s=m+q;							/*  length of status and data TLV */
	n=tlvCalcTagLenLen(s);			/*  length of TAG_CERT_RAM_TO_RAM tag+len  */
	bo=n+s;
	
	stat=tlvPutTagLen(xpOutBuf,xBufLen,TAG_CERT_DSC,s);
	if (stat<0) 
		return procErr(ERR_PARAM_TYPE, xpOutBuf, xBufLen, __FILE__, __LINE__);
	t=stat;
	stat=tlvPutTlv(xpOutBuf+t,xBufLen-t,TAG_STATUS,sizeof(U8),&status);
	if (stat<0) 
		return procErr(ERR_PARAM_TYPE, xpOutBuf, xBufLen, __FILE__, __LINE__);
	t+=stat;
	stat=tlvPutTlv(xpOutBuf+t,xBufLen-t,TAG_OUT,bufCont,buf);
	if (stat<0) 
		return procErr(ERR_PARAM_TYPE, xpOutBuf, xBufLen, __FILE__, __LINE__);
	
	return bo;
}

/*----------------------------------------------------------------------------*\
|* Execute the SCR command
|* Returns the number of bytes written to xpOutBuf.
\* ---------------------------------------------------------------------------*/
int socSCR(U8 xTag, U8 *xpArgs, U32 xArgLen,U8 *xpOutBuf, U32 xBufLen)
{
	int  stat; 		 /*  local functions status */
	U8 	 status; 	 /*  overall command status  */
	int  bo; 		 /*  nb bytes written in xpOutBuf */
	int  m,n;
	int  q,s,t;
	U8   tag;
	U32  len;
	U8	 *pVal;
	CERT_SCR *pSCR;
	DATA_IO xIO;
	U8	 buf[MAXBUFSIZE];
	U32	 bufCont;
	
	static int callCnt=0;

	callCnt++;
	
	memset(&xIO, 0, sizeof(DATA_IO));
	
	syslog(LOG_DEBUG, "\nemuSCR call #%d",callCnt); /*  DEBUGGING */
	
	/*  arguments processing */
	/*  There should be exactly 2 TLVs */
	n=tlvChkSeq(xpArgs,xArgLen);
	if (n!=2)
		return procErr(ERR_PARAM_NUMBER, xpOutBuf, xBufLen, __FILE__, __LINE__);
	
	/*  Check first TLV, which should contain a CERT_DSC structure */
	stat=tlvGetTlv(&tag,&len,&pVal,xpArgs,xArgLen);
	if ( (stat<0) || (tag!=TAG_SCR) || (len!=sizeof(CERT_SCR)) )
		return procErr(ERR_PARAM_TYPE, xpOutBuf, xBufLen, __FILE__, __LINE__);
	pSCR=(CERT_SCR*)pVal;
	/* fix U16 fields endianness */
	pSCR->emi = ntohs(pSCR->emi);
	pSCR->vidPid = ntohs(pSCR->vidPid);
	pSCR->audPid = ntohs(pSCR->audPid);
	pSCR->pcrPid = ntohs(pSCR->pcrPid);

	/*  Find offset of 2nd TLV */
	m=tlvCalcTagLenLen(len)+len;
	/*  Check 2nd TLV, which should contain input data */
	stat=tlvGetTlv(&tag,&len,&pVal,xpArgs+m,xArgLen-m);
	if ( (stat<0) || (tag!=TAG_IN) )
		return procErr(ERR_PARAM_TYPE, xpOutBuf, xBufLen, __FILE__, __LINE__);
	
	/* scr parameters */
	xIO.din = pVal;
	xIO.din_size = len;
	xIO.din_len = len;
	xIO.dout = buf;
	xIO.dout_size = MAXBUFSIZE;
	xIO.dout_len = 0;

	syslog(LOG_DEBUG, "pSCR->emi=%04x", pSCR->emi);
	syslog(LOG_DEBUG, "pSCR->vidPid=%04x", pSCR->vidPid);
	syslog(LOG_DEBUG, "pSCR->audPid=%04x", pSCR->audPid);
	syslog(LOG_DEBUG, "pSCR->pcrPid=%04x", pSCR->pcrPid);
	syslog(LOG_DEBUG, "pSCR->cmd.scr.dir=%s", pSCR->cmd.scr.dir?"enc":"dec");
	syslog(LOG_DEBUG, "pSCR->cmd.scr.skipusekey=%d", pSCR->cmd.scr.skipusekey);
	syslog(LOG_DEBUG, "pSCR->cmd.scr.swtch=src/dest=%s", pSCR->cmd.scr.swtch?"ram/ram":"src/dest");
	syslog(LOG_DEBUG, "pSCR->cmd.scr.parity=%s", pSCR->cmd.scr.parity?"even":"odd");

	/* Execute SCR */
	printf("*******************************************************\n");
	printf("*  Begin of SCR call #%d (EMI=%04x, dir=%d) \n", callCnt, pSCR->emi, pSCR->cmd.scr.dir);
	printf("*******************************************************\n");
	stat=cert_scr_ciph(pSCR, &xIO);
	printf("*******************************************************\n");
	printf("*  End of SCR call #%d (EMI=%04x, dir=%d) \n", callCnt, pSCR->emi, pSCR->cmd.scr.dir);
	printf("*******************************************************\n");
	
	bufCont = xIO.dout_len;
	
	if (stat<0)
		status=stat;
	else
		status=0;
	
	syslog(LOG_DEBUG, "emuSCR: stat=%d, len=%u, bufCont=%u",
			stat,len,bufCont); /*  DEBUGGING */
	
	/*  Prepare response */
	m=tlvCalcTlvLen(sizeof(U8));	/*  length of status TLV (here: m==3) */
	q=tlvCalcTlvLen(bufCont);		/*  length of data TLV */
	s=m+q;							/*  length of status and data TLV */
	n=tlvCalcTagLenLen(s);			/*  length of TAG_CERT_SCR tag+len  */
	bo=n+s;
	
	stat=tlvPutTagLen(xpOutBuf,xBufLen,TAG_CERT_SCR,s);
	if (stat<0) 
		return procErr(ERR_PARAM_TYPE, xpOutBuf, xBufLen, __FILE__, __LINE__);
	t=stat;
	stat=tlvPutTlv(xpOutBuf+t,xBufLen-t,TAG_STATUS,sizeof(U8),&status);
	if (stat<0) 
		return procErr(ERR_PARAM_TYPE, xpOutBuf, xBufLen, __FILE__, __LINE__);
	t+=stat;
	stat=tlvPutTlv(xpOutBuf+t,xBufLen-t,TAG_OUT,bufCont,buf);
	if (stat<0) 
		return procErr(ERR_PARAM_TYPE, xpOutBuf, xBufLen, __FILE__, __LINE__);
	
	return bo;
}


/*----------------------------------------------------------------------------*\
|* Reset AKL and OTP interfaces
|* Returns the number of bytes written to xpOutBuf.
\* ---------------------------------------------------------------------------*/
int socResetAkl(U32 xArgLen,U8 *xpOutBuf, U32 xBufLen)
{
	int stat; 		 /*  local functions status */
	U8 	status; 	 /*  overall command status  */
	int bo; 		 /*  nb bytes written in xpOutBuf */
	int m,n;

	/*  arguments processing */
	if (xArgLen!=0) 
		return procErr(ERR_PARAM_NUMBER, xpOutBuf, xBufLen, __FILE__, __LINE__);
	
	/*  execute function */
	stat = do_cert_reset(1); /* default to 1 second delay */

	status = (U8) stat;
	
	/*  Prepare response */
	m=tlvCalcTlvLen(sizeof(U8));	/*  length of status TLV (here: m==3) */
	n=tlvCalcTagLenLen(m);			/*  length of TAG_CERT_RESET_AKL tag+len (here: n==2) */
	stat=tlvPutTagLen(xpOutBuf,xBufLen,TAG_CERT_RESET_AKL,m);
	if (stat<0) 
		return procErr(ERR_PARAM_TYPE, xpOutBuf, xBufLen, __FILE__, __LINE__);
	stat=tlvPutTlv(xpOutBuf+n,xBufLen-n,TAG_STATUS,sizeof(U8),&status);
	if (stat<0) 
		return procErr(ERR_PARAM_TYPE, xpOutBuf, xBufLen, __FILE__, __LINE__);
	bo=n+m;
	
	return bo;
}


/* ------------------------------------------------------------------------------ */
/*  Return 0 if xEmi is a leagl R2R EMI, otherwise return -1 */
/* ------------------------------------------------------------------------------ */
int procR2rEmi(int xEmi)
{
	switch (xEmi)
	{
		case 0x4025:
		case 0x4026:
		case 0x4027:
		case 0x4042:
		case 0x4043:
			return 0; /*  OK */
		default:
			return -1;
	}
}

/* emacs vars
Local Variables:
c-basic-offset: 8
*/
/*** end of file ***/
