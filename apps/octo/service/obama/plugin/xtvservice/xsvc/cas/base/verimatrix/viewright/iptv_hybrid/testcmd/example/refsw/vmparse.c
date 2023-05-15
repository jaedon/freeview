#if !USE_VMCLIENT
/* Do not enable when using VM ECM	*/
#define OPEN_FORMAT_ECM
#endif
/*****************************************************************************/
/* vmparse.c - simple parser for Verimatrix transport streams                */
/*                                                                           */
/*    int ParseStream() - returns packet type                                */
/*                                                                           */
/*****************************************************************************/

/*------------ Standard Includes (for standalone and in driver)--------------*/
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include "vmparse.h"
//#include "bc_vmxsec.h"
#include "mwapp.h"
#include "VMClient.h"

#include "vmx_vr_iptvh_testcmd_join.h"

#define TEMP_HUMANDKS

//extern int nexus_service_start(unsigned short pid);

/*------------ Local Variables ----------------------------------------------*/
static unsigned char vm_NullPacket[5] = {0x10,0x47,0x47,0x47,0x47};
int packetcount = 0;
#if 0
int vdebug = 0;
#else
#if defined(TEMP_HUMANDKS)
int vdebug = 4; // 6
#else
int vdebug = 4;
#endif
#endif

typedef struct
{
	int ucUserParamFlag;
	unsigned char usUserParamData[50];
} user_t;
user_t user[2];


static int test1(void *pUserParam, descramble_param_t *dscParam)
{
	int usCnt = 0;	

	HxLOG_Print("[%s:%d] callback...........................\n", __FUNCTION__, __LINE__);
	HxLOG_Print("encAlg = %d\n", dscParam->encAlg);
	if(pUserParam)
	{
		user_t *userparam;		
		HxLOG_Print("[%s:%d] pUserParam is exist\n", __FUNCTION__, __LINE__);
		userparam = (user_t *)pUserParam;
		HxLOG_Print("ucUserParamFlag = %d, usUserParamData = %s\n", userparam->ucUserParamFlag, userparam->usUserParamData);
	}
	else
	{
		HxLOG_Print("[%s:%d] pUserParam is not exist\n", __FUNCTION__, __LINE__);
	}
}

static int test2(void *pUserParam, descramble_param_t *dscParam)
{
	int usCnt = 0;	

	HxLOG_Print("[%s:%d] callback...........................\n", __FUNCTION__, __LINE__);
	HxLOG_Print("encAlg = %d\n", dscParam->encAlg);
	if(pUserParam)
	{
		user_t *userparam;		
		HxLOG_Print("[%s:%d] pUserParam is exist\n", __FUNCTION__, __LINE__);
		userparam = (user_t *)pUserParam;
		HxLOG_Print("ucUserParamFlag = %d, usUserParamData = %s\n", userparam->ucUserParamFlag, userparam->usUserParamData);
	}
	else
	{
		HxLOG_Print("[%s:%d] pUserParam is not exist\n", __FUNCTION__, __LINE__);
	}
}

/*---------------------------------------------------------------------------*/
/* ParseStream() - Parses a packet at a time and returns type, maintains     */
/*                 parse state for PAT/PMT/ECM                               */
/*                                                                           */
/*   Possible returns are:                                                   */
/*                         PASS        packet not relevent, pass through     */
/*                         ENCRYPTED   packet need decryption                */
/*                         NEWNULL     new Verimatrix null, process          */
/*                         NEWECM      new Verimatrix ecm                    */
/*                                                                           */
/*---------------------------------------------------------------------------*/

int ParseStream(unsigned char *p, struct _streaminfo *s)
{
	int parsetype = PASS;
	unsigned short pid;
	unsigned char version;
	int a = 0;
	static int lasttsc = 0;

	#define RESET_STREAM {  DEBUG(1, "RESET STREAM\n"); \
	                        s->ecmtype = s->streamtype = s->pmtpid = 0;\
	                        s->pcrpid = 0; s->audpid = 0; s->vidpid = 0; \
	                        s->ecmtid = 0; s->ecmpid = 0; s->channel_id = 0; \
	                        s->patver = s->pmtver = s->ecmver = -1; }

	/* implicit reset */
	if (!p) {
		RESET_STREAM;
		return PASS;
	}

	pid = GET_PID(p);
	packetcount++;

   /*------------------*/
   /* look for new ECM */
   /*------------------*/
   if (s->ecmpid && (pid == s->ecmpid)) {
	   	
      int pslen;
      unsigned int channel_id;

      DEBUG(6, "[%d] ECM Hdr %02x %02x %02x %02x   %02x %02x %02x %02x\n",
                packetcount, p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7]);

      /* skip over adaptation field */
      if ((p[3] & 0x30) == 0x30)
         a = p[4] + 1;

      version = (p[10+a] & 0x3E) >> 1;
      pslen =  ((p[6+a] & 0x0F) << 8) + p[7+a];
      channel_id = ntohl(*(unsigned int *)(p+a+21));

      /* valid table id must be 0x80 or 0x81 */
      if (!( (p[5+a] == 0x80) || (p[5+a] == 0x81) )) {
         parsetype = PASS;
         goto done;
      }

#ifndef OPEN_FORMAT_ECM
      /* is this a Verimatrix ECM */
      else if ( strncmp((const char *)(p+13+a), "VMECM", 5)) {
        DEBUG(1,"[%d] Non-Verimatrix ECM rcvd, skipping\n", packetcount);
        parsetype = PASS;
        goto done;
      }
      else
      {
          DEBUG(5,"[%d] Verimatrix ECM rcvd\n", packetcount);
      }
#endif
      DEBUG(6,"[%d] ECM rcvd\n", packetcount);

#ifndef OPEN_FORMAT_ECM
      /* channel check */
      if (channel_id != s->channel_id) {
		DEBUG(1,"[%d] Channel change detect in ECM: chanel id %d, prev %d\n", packetcount, channel_id, s->channel_id);
		s->channel_id  = channel_id;
		s->reset = 1;
		s->ecmtid = 0;
		s->ecmtype = ECMSTREAM;
		if (s->ecmver == 255)
			parsetype = NEWECM;
		s->ecmver = version;
		memcpy(s->ecmpacket, p, TS_PACKET_SIZE);
      }
      /* version check */
      else if (version != s->ecmver) {
		DEBUG(1,"[%d] New ecm version: table id 0x%02x, version %d, prev %d\n", packetcount, p[5+a], version, s->ecmver);
		s->ecmtype = ECMSTREAM;
		//         if (s->ecmver == 255)
		parsetype = NEWECM;
		s->ecmver = version;
		memcpy(s->ecmpacket, p, TS_PACKET_SIZE);
      }
      /* difference check for the case where PAT/PMT/ECM pids are same between channels */
      else if (memcmp(p+29+a, &s->ecmpacket[29+a],pslen-25)) {
		s->ecmtype = ECMSTREAM;
		if (s->ecmver == 255)
			parsetype = NEWECM;
		s->ecmver = version;
		memcpy(s->ecmpacket, p, TS_PACKET_SIZE);
		DEBUG(1,"[%d] New stream by ECM difference\n", packetcount);
		s->havekey = 0;
		s->encrypted = 0;
      }
      else {
         parsetype = PASS;
         goto done;
      }
#else
      if (version != s->ecmver) {
		DEBUG(1,"[%d] New ecm version: table id 0x%02x, version %d, prev %d\n", packetcount, p[5+a], version, s->ecmver);
		s->ecmtype = ECMSTREAM;
		if (s->ecmver == 255)
			parsetype = NEWECM;
		s->ecmver = version;
		memcpy(s->ecmpacket, p, TS_PACKET_SIZE);
      }
      else {
		parsetype = ECM;
		goto done;
      }
#endif
   }

   /*------------------*/
   /* look for new PMT */
   /*------------------*/
   else if (s->pmtpid && (s->pmtpid == pid)) {
      int pmtlen;
      unsigned short pcrpid;
      int pilen;
      unsigned char *q;
      int pmtrem;
      unsigned short vidpid;
      unsigned short audpid;
      unsigned char *e;
      int esl;
      unsigned short pid[8];

      DEBUG(6, "Rcvd PMT For program.\n");

      /* skip over adaptation field */
      if ((p[3] & 0x30) == 0x30)
         a = p[4] + 1;

      /* table id must be 2 for PMT or error */
      if (p[a+5] != 0x2)
      {
         goto done;
      }
      pmtlen = ((p[a+6] << 8) + p[a+7]) & 0x0FFF;
      version = (p[a+10] & 0x3E) >> 1;
      pcrpid = ((p[a+13] << 8) + p[a+14]) & 0x1FFF;
      pilen = ((p[a+15] << 8) + p[a+16]) & 0x0FFF;
      pmtrem = pmtlen - (8 + pilen + 4);

      q = p + a + 17;

      /* check every PMT for ECM's */
      while (pilen > 0)
      {
         /* look for a CA_descriptor_tag, 0x09 and Verimatrix id 5601 */
         if (q[0] == 0x09)
         {
            if ((q[2] == 0x56) && (q[3] == 0x01)) {
               DEBUG(6, "Found VMX CA descriptor in PMT.\n");
               break;
            }
            else {
               DEBUG(6,"non-Verimatrix ECM detected\n");
            }
         }
         pilen -= q[1]+2;
         q += q[1]+2;
      }

      /* advance to after the program_info loop */
      e = q + pilen;

      vidpid = 0;
      audpid = 0;

      while (pmtrem > 0) {
         /* save first video and audio pid found */
		 if ((vidpid == 0) && (e[0] == 0x02)) {
		    vidpid = ((e[1] << 8) + e[2]) & 0x1FFF;
		    //HxLOG_Print("[%s:%d] vidpid = 0x%x\n", __FUNCTION__, __LINE__, vidpid);			
		 }
		 else if ((audpid == 0) && ((e[0] == 0x03) || (e[0] == 0x04) || (e[0] == 0x81))) {
		    audpid = ((e[1] << 8) + e[2]) & 0x1FFF;
		  // HxLOG_Print("[%s:%d] audpid = 0x%x\n", __FUNCTION__, __LINE__, audpid);			
		 }
		 esl = ((e[3] << 8) + e[4]) & 0x0FFF;
		 pmtrem -= (5 + esl);
		 e += (5 + esl);
		}

		if (pilen < 0)
		 pilen = 0;

		if (version != s->pmtver) {
		 DEBUG(3,"[%d] New stream by PMT version %d, prev %d, %s, vid pid %d\n", packetcount, version, s->pmtver, pilen ? "with ECM" : "without ECM", vidpid);

#if 0//defined(TEMP_HUMANDKS)
		// vipid가 0인데 위의 code 확인 필요..
		vidpid = pcrpid;
#endif
		 
		 s->havekey = 0;
		 s->encrypted = 0;
		 parsetype = PMT;
		 s->pmtver = version;
		 s->reset = 1;
		 s->ecmtid = 0;
		 s->pcrpid = pcrpid;
		 s->vidpid = vidpid;
		 s->audpid = audpid;
		 pid[0] = vidpid;

         /* New PMT detected */
         /* VMXSEC_ServiceStart(s->pmtpid, 0, NULL); */ 
#if defined(TEMP_HUMANDKS)
//         nexus_service_start(vidpid); // temp_humandks
//         xmgr_vmxiptv_cmd_nexus_service_start(vidpid); // temp_humandks
#endif

		HxLOG_Print("[%s:%d] vidpid = 0x%x, audpid= 0x%x, pcrpid = 0x%x\n", __FUNCTION__, __LINE__, vidpid, audpid, pcrpid);

		mw_config_t *g = mw_app_get_config();
		HxLOG_Print("@@@@@@@@@@ %s, %d, CALLING VMServiceStart\n", __FUNCTION__, __LINE__);
		HxLOG_Print("@@@@@@@@@@ %s, %d, g->Ctx=0x%x\n",__FUNCTION__, __LINE__, (int) g->pCtx);
		unsigned int ServiceIdx = 0x40;
#if 0 // test only
{
		static int cnt= 0;
		#if 0
		if(cnt==0)
			ServiceIdx = 0x40;
		else
			ServiceIdx = 0x41;
		#endif
		if(cnt==2)
			cnt = 0;
		if(cnt==0)
		{
					static int callback=0;
				   	int    all_algorithms[1] = { -1 };
					memset(&user[0], 0, sizeof(user[0]));
					user[0].ucUserParamFlag = 1;
					memcpy(&user[0].usUserParamData[0], "0x40", 10);
					VMServiceStart(g->pCtx, 0x40, 1, pid);			
					HxLOG_Print("================> start 1\n");
					HxLOG_Print("================> start 1\n");
					HxLOG_Print("================> start 1\n");				
					#if 0
					if(callback==0)
					{
						if( VMRegisterControlWordCallback( g->pCtx, test1, (void *)&user[0], 1, all_algorithms ) != 0 )
						{
							HxLOG_Print("[%s:%d] VMRegisterControlWordCallback() : error\n", __FUNCTION__, __LINE__);
						}
					}
					#endif
					cnt = 1;
					callback++;
		}
		else if(cnt==1)
		{
				   	int    all_algorithms[1] = { -1 };
					static int callback=0;
					memset(&user[1], 0, sizeof(user[0]));
					user[1].ucUserParamFlag = 2;
					memcpy(&user[1].usUserParamData[0], "0x41", 10);
					HxLOG_Print("================> start 2\n");					
					HxLOG_Print("================> start 2\n");
					HxLOG_Print("================> start 2\n");					
					VMServiceStart(g->pCtx, 0x41, 1, pid);	
					#if 0
					if(callback==0)
					{
						if( VMRegisterControlWordCallback( g->pCtx, test1, (void *)&user[1], 1, all_algorithms ) != 0 )
						{
							HxLOG_Print("[%s:%d] VMRegisterControlWordCallback() : error\n", __FUNCTION__, __LINE__);
						}
					}
					#endif
					cnt = 2;	
					callback++;
		}
}
#endif
		if(VMServiceStart(g->pCtx, 0x40, 1, pid) != 0)
		{
			HxLOG_Print("@@@@@@@@@@@ error........\n");
		}
		HxLOG_Print("@@@@@@@@@@ %s, %d, RETURN FROM VMServiceStart, pid[0] = 0x%x, audpid = 0x%x, ServiceIdx = 0x%x\n", __FUNCTION__, __LINE__, pid[0], audpid, ServiceIdx);

#if defined(TEMP_HUMANDKS)
//		nexus_add_key_slot_to_channel(0x40, vidpid); // temp_humandks
//		xmgr_vmxiptv_cmd_nexus_add_key_slot_to_channel(0x40, vidpid); // temp_humandks
#endif
      }

	/* if PMT has ECMs, use it */
	if (pilen) {
		unsigned short ecmpid = ((q[4] << 8) + q[5]) & 0x1FFF;
		DEBUG(6,"[%d] New ecmpid %04x, old %04x\n", packetcount, ecmpid, s->ecmpid);
		if (ecmpid != s->ecmpid) {
			s->ecmpid = ecmpid;
			DEBUG(6, "[%d] Using ECM: 0x%04x\n", packetcount, s->ecmpid);
		}
	}
   }
   /*------------------*/
   /* look for new PAT */
   /*------------------*/
   else if (pid == 0) {
      int changed = 0;

      /* skip over adaptation field */
      if ((p[3] & 0x30) == 0x30) {
         a = p[4] + 1;
      }
      /* skip over stuffing */
      else {
         a = p[4];
      }

      version = (p[a+10] & 0x3E) >> 1;
      if (s->patver != version) {
        DEBUG(6,"[%d] New stream by PAT version %d, prev %d\n", packetcount, version, s->patver);
        memcpy(s->patpacket, p, TS_PACKET_SIZE);
        changed++;
      }
      else if (memcmp(s->patpacket+4, p+4, TS_PACKET_SIZE-4)) {
        DEBUG(6,"[%d] New stream by PAT difference, version %d\n", packetcount, version);
            memcpy(s->patpacket, p, TS_PACKET_SIZE);
            changed++;
      }

      if (changed)
      {
         int pgm = 0;

         s->havekey = 0;
         s->encrypted = 0;
         s->patver = version;
         s->pmtver = s->ecmver = -1;
         s->ecmpid = 0;
         s->reset = 1;
         s->ecmtid = 0;
         memset(s->ecmpacket, 0, TS_PACKET_SIZE);

         /* if the adaptation field is set with payload, skip adaptation part */
         if ((p[3] & 0x30) == 0x30)
            a = p[4] + 1;

         /* only look at first non-zero program #! */
         pgm = ((p[a+13] << 8) + p[a+14]) & 0x1FFF;
         if (pgm == 0)
            s->pmtpid = ((p[a+19] << 8) + p[a+20]) & 0x1FFF;
         else
            s->pmtpid = ((p[a+15] << 8) + p[a+16]) & 0x1FFF;

         DEBUG(1, "Using PMT: 0x%04x PGM %d\n", s->pmtpid, pgm);
         parsetype = PASS;
      }
   }

   /*-------------------------------------*/
   /* look for new Verimatrix null packet */
   /*-------------------------------------*/
   else if (pid == 0x1FFF) {
      /* is this a Verimatrix null */
      if (!memcmp(p + 3, vm_NullPacket, sizeof(vm_NullPacket))) {
         /* is this a new Verimatrix null */
         if (memcmp(p + 8, s->ecmpacket + 8, 40)) {
            DEBUG(6,"[%d] Detected new NULL packet\n", packetcount);
            parsetype = NEWNULL;
            /* we save the packet in process_null(), don't do it here!!
            // memcpy(s->ecmpacket, p, TS_PACKET_SIZE); */
            if (s->ecmtype == ECMSTREAM)
               DEBUG(6,"[%d] Detected new NULL packet in an ECM stream!\n", packetcount);
            s->ecmtype = NULLSTREAM;
         }
      }
   }

   /*---------------------*/
   /* look for PCR packet */
   /*---------------------*/
   else if ((s->pcrpid) && (pid == s->pcrpid)) {
      parsetype = PCR;
   }

done:

   /* put a state machine here if needed */
   if (((parsetype == PASS) || (parsetype == PCR)) && (TSC_ISSET(p))) {
      if (TSC_ISSET(p) != lasttsc) {
         DEBUG(2, "[%d] tsc change %x -> %x\n", packetcount, lasttsc, TSC_ISSET(p));
         lasttsc = TSC_ISSET(p);
      }

      if (!s->havekey && !s->encrypted) {
         s->encrypted++;
         DEBUG(1,"[%d] First encrypted\n", packetcount);
      }
      parsetype = ENCRYPTED;
   }
   return parsetype;
}

