/***************************************************************************
 *     Copyright (c) 2004-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: lm_helper.cpp $
 * $brcm_Revision: 13 $
 * $brcm_Date: 4/28/10 3:00p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/lm_helper.cpp $
 * 
 * 13   4/28/10 3:00p mward
 * SW7125-376:  Eliminate warning for uninitialized variable.
 * 
 * 12   7/22/08 10:14a ssood
 * PR42739: moving the rtp header stats function to ip applib
 * 
 * 11   7/15/08 2:29p lwhite
 * PR42739: RTCP stats. Consolidate code
 * 
 * 10   7/14/08 4:15p lwhite
 * PR42739: Added RTCP support
 * 
 * 9   5/18/07 12:01p jjordan
 * PR30200: avoid invoking unsupported feature (if not playpump_ip build)
 * 
 * 8   4/27/07 2:01p mward
 * PR29594: Try eth0:1 also, for 7118.
 * 
 * 7   1/30/07 2:46p ssood
 * PR27364: lm_helper in infinite loop when used w/o IP Playpump
 * 
 * 6   12/11/06 2:52p jjordan
 * PR26420: fix generic keep-alive
 * 
 * 5   12/8/06 3:19p jjordan
 * PR26420: add support for new style of RTSP keep-alive required for
 * Bitband Vision 280 video server, software version 5.1 and later
 * 
 * 4   10/11/06 1:47p jjordan
 * PR24714: must populate the dst IP address string
 * 
 * 3   10/3/06 4:51p jjordan
 * PR24714: Add support for RTSP protocol
 * 
 * 2   9/28/06 11:53p ssood
 * PR24576: Integration w/ BLIVE_EXT Library (SAP + LM Scheduler)
 * 
 * 1   7/25/06 5:03p jjordan
 * PR22906: Integrate liveMedia open-source for RTP and RTCP support
 * 
 ***************************************************************************/

/* liveMedia helper functions */
#include <pthread.h>
#include <sys/ioctl.h>
#include <net/if.h>

#include "bstd.h"
#include "bsettop.h"
#include "bsettop_playpump.h"
#include "bsettop_pvr.h"
#include "mpeg2transportstreamrtpsource.h"
#include "lm_helper.h"
#include "blive_ext.h"

BDBG_MODULE(brutus_lm_helper);

static UsageEnvironment* lm_env;

// open livemedia
void liveMediaOpen(void) {
	char const* const libName = "LIVE555 Streaming Media v";
	char const* const libVersionStr = LIVEMEDIA_LIBRARY_VERSION_STRING;
	BDBG_MSG(("%s%s", libName, libVersionStr));

	if ( (lm_env = blive_scheduler_open()) == NULL ) {
		BDBG_ERR(("ERROR: failed to Open the Live Media Scheduler"));
		BDBG_ASSERT((lm_env != NULL));
	}
}

// close livemedia
void liveMediaClose(void) {
	blive_scheduler_close();
}

/////////////////////// livemedia session /////////////////////////////

#define B_LM_MAX_CNAME_LEN			100
#define B_LM_ESTIMATED_RTCP_BW		160			// in kbps; for RTCP b/w share
#define B_BITBAND_TIMESTAMP_FREQ   	1000		// Bitband RTP timestamp frequency (1KHz)
#define B_MP2T_TIMESTAMP_FREQ		90000		// MP2T RTP timestamp frequency (90KHz)
#define B_LM_KEEPALIVE_USEC 		15000000	// time between RTSP keep-alive messages
#define B_LM_PERIODIC_TASK_USEC		1000000		// time between execution of periodic task

// the session state
typedef enum lmSessionState {
	lmSessionStateNone,
	lmSessionStateOpened,
	lmSessionStateStarted,
	lmSessionStateClosing
} lmSessionState;

// the session
typedef struct liveMediaSession {
	RTPSource* source;
	Groupsock *rtpGroupsock;
	Groupsock *rtcpGroupsock;
	RTCPInstance* rtcpInstance;
	RTSPClient* rtspClient;
	MediaSession* rtspMediaSession;
	bplaypump_rtp_header *rtpHeader;
	bplaypump_rtp_hdr_data_t *rtpHdrData;
	bplaypump_t pump;
	char *url;
	bsettop_ip_protocol protocol;
	lmSessionState state;
	unsigned prevSeqNum;
	unsigned periodicTaskPendingCount;
	unsigned periodicTaskSchedCount;
	unsigned timestampFreq;
	bool gotFirstPacket;
    struct timeval presentationStartTime; 
    struct timeval presentationTime; 
    struct timeval NPT; 
} liveMediaSession;

// the session context
typedef struct liveMediaSessionContext {		// pass into blive_scheduler_queue_delayed_task(), new/delete this with each unique session
	liveMediaSession *session;					// pointer to the session
	void *params;								// new/delete params on each scheduled "delayed task", or set to NULL
	blive_scheduler_task_wait_ctx_t waitCtx;	// used to wait/signal scheduler "delayed task" completion
} liveMediaSessionContext;

static unsigned char CNAME[B_LM_MAX_CNAME_LEN+1];

static void doSessionInit(void *context);
static void doSessionOpen(void *context);
static void doSessionStart(void *context);
static void doSessionRTSPControl(void *context);
static void doSessionClose(void *context);
static void doPeriodicTask(void *context);

static void processReceptionStats(liveMediaSession *session);
static void sendKeepAlive(liveMediaSession *session);
static bool hasBitbandRSTPServer(liveMediaSession *session);

// open a livemedia RTP or RTSP session
void liveMediaSessionOpen(void **context, bplayback_socket_open_params *socketParams) {
 	liveMediaSessionContext *lmContext;

	if(lm_env == NULL) liveMediaOpen();

 	lmContext = new liveMediaSessionContext;
	lmContext->params = socketParams;
	lmContext->session = new liveMediaSession;
	memset(lmContext->session, 0, sizeof(liveMediaSession));

	// schedule initialization of session
	blive_scheduler_queue_delayed_task(0, (TaskFunc*)doSessionInit, lmContext);

	// wait for init to complete
	blive_scheduler_wait(&lmContext->waitCtx);

	// return the context and socket back to the caller
	*context = lmContext;
	socketParams->fd = lmContext->session->rtpGroupsock->socketNum();
	if(lmContext->session->protocol == bsettop_ip_protocol_rtsp) {
		// populate the dst IP address string
		struct ifreq ifr;
		sockaddr_in *ifaddr;
		struct in_addr inp;

		// TODO: Add code to retrieve the interface index instead of hard coding the i/f name
		strcpy(ifr.ifr_name, "eth0");

		// now retrieve the IP address associated with the media
		if (ioctl(socketParams->fd, SIOCGIFADDR, &ifr) != 0)
		{	// Try 7118 on-chip eth, via BNM front end
			strcpy(ifr.ifr_name, "eth0:1");
			if (ioctl(socketParams->fd, SIOCGIFADDR, &ifr) != 0)
				BDBG_WRN(("Failed to get Interface Address Information"));
		}
		ifaddr = (sockaddr_in *)&ifr.ifr_addr;
		strncpy(socketParams->ip_addr, inet_ntoa(ifaddr->sin_addr), 16);
	}

	BDBG_ASSERT(socketParams->fd);

	// schedule open of session (don't wait for completion)
	blive_scheduler_queue_delayed_task(0, (TaskFunc*)doSessionOpen, lmContext);
}

// start a livemedia RTP or RTSP session
void liveMediaSessionStart(void *context, bplaypump_t pump) {
	liveMediaSessionContext *lmContext = (liveMediaSessionContext *)context;
	liveMediaSession *session = lmContext->session;

	BDBG_ASSERT(pump);

	session->pump = pump;

	// schedule start
	blive_scheduler_queue_delayed_task(0, (TaskFunc*)doSessionStart, lmContext);
}

// control an RTSP session (eg FFW, REW, etc)
void liveMediaSessionRTSPControl(void *context, bplayback_rtsp_ctrl_cmd *cmd) {
	liveMediaSessionContext *lmContext = (liveMediaSessionContext *)context;
	liveMediaSession *session = lmContext->session;

	BDBG_ASSERT(session);

	// create a copy of the params and manage its lifecycle
	lmContext->params = new bplayback_rtsp_ctrl_cmd;
	memcpy(lmContext->params, cmd, sizeof(bplayback_rtsp_ctrl_cmd));

	// schedule
	blive_scheduler_queue_delayed_task(0, (TaskFunc*)doSessionRTSPControl, lmContext);
}

// close a livemedia RTP or RTSP session
void liveMediaSessionClose(void *context) {
	liveMediaSessionContext *lmContext = (liveMediaSessionContext *)context;
	liveMediaSession *session = lmContext->session;

	BDBG_ASSERT(session);

	// shedule close
	blive_scheduler_queue_delayed_task(0, (TaskFunc*)doSessionClose, lmContext);
}

// get data from an RTP or RTSP session
void liveMediaSessionGetData(void *context, void *buffer, unsigned int *buffer_size) {
	// stub for future use
}

// stop a livemedia RTP or RTSP session
void liveMediaSessionStop(void *context) {
	// stub for future use
}

static void doSessionInit(void *context) {
	liveMediaSessionContext *lmContext = (liveMediaSessionContext *)context;
	liveMediaSession *session = lmContext->session;
	bplayback_socket_open_params *socketParams = (bplayback_socket_open_params *)lmContext->params;
	struct in_addr sessionAddress;
	const unsigned char ttl = 1; // low, in case routers don't admin scope
	bool verbose = false;

	// setup buffer for reception stats
	session->rtpHeader = (bplaypump_rtp_header *)malloc(sizeof(bplaypump_rtp_header)*B_MAX_RTP_HDRS);
	session->rtpHdrData = (bplaypump_rtp_hdr_data_t *)malloc(sizeof(bplaypump_rtp_hdr_data_t));
        memset(session->rtpHdrData, 0, sizeof(bplaypump_rtp_hdr_data_t));
	session->rtpHdrData->data = session->rtpHeader;
	session->timestampFreq = B_MP2T_TIMESTAMP_FREQ;
	session->protocol = socketParams->protocol;

	if(session->protocol == bsettop_ip_protocol_rtsp) {
		portNumBits portNum;
		Port srcPort(0);
		Groupsock *prevGroupsock = NULL;

		BDBG_MSG(("Got URL: %s\n", socketParams->url));
		session->url = socketParams->url;

		if(getenv("rtsp_verbose")) {
			if((strcasecmp(getenv("rtsp_verbose"), "y"))==0)
				verbose = true;
		}

		// create RTSP Client object
		session->rtspClient = RTSPClient::createNew(*lm_env, verbose, NULL, 0);
		session->rtspClient->setUserAgentString("brutus (C)2001-2006, Broadcom Corporation");

		for(;;) {
			// open RTP socket
			session->rtpGroupsock = new Groupsock(*lm_env, sessionAddress, 0, ttl);
			getSourcePort(*lm_env, session->rtpGroupsock->socketNum(), srcPort);
			if(prevGroupsock !=  NULL) {
				delete prevGroupsock;
				prevGroupsock = NULL;
			}
			portNum = ntohs(srcPort.num());
			// use only even numbered ephemeral port
			if ((portNum&1) == 0) {
				break;
			}
			else {
				BDBG_WRN(("Rejecting non-even port number: %d", portNum));
				prevGroupsock = session->rtpGroupsock;	// need to keep this until we new another one
			}
		}
		// open RTCP socket
		session->rtcpGroupsock = new Groupsock(*lm_env, sessionAddress, portNum+1, ttl);
	}
	else {
		// open RTP/RTCP sockets
		BDBG_MSG(("Got IP address: %s, port number: %d, protocol: %d", socketParams->ip_addr, socketParams->port, socketParams->protocol));
		sessionAddress.s_addr = our_inet_addr(socketParams->ip_addr);
		session->rtpGroupsock = new Groupsock(*lm_env, sessionAddress, socketParams->port, ttl);
		session->rtcpGroupsock = new Groupsock(*lm_env, sessionAddress, socketParams->port+1, ttl);
	}

	// signal caller that init is complete
	blive_scheduler_signal(&lmContext->waitCtx);
}

// open a livemedia RTP or RTSP session
static void doSessionOpen(void *context) {
	liveMediaSessionContext *lmContext = (liveMediaSessionContext *)context;
	liveMediaSession *session = lmContext->session;

#if 0
	printf("@@@ Entered doSessionOpen() @ 0x%x, session: 0x%x\n", doSessionOpen, (unsigned)session);
#endif

	if(session->protocol == bsettop_ip_protocol_rtsp) {
		// do OPTIONS
		BDBG_MSG(("OPTIONS"));
		char* optionsResponse = session->rtspClient->sendOptionsCmd(session->url, /*username*/ NULL, /*password*/ NULL);
		if(optionsResponse == NULL) {
			BDBG_ERR(("Error: Failed to get OPTIONS response"));
		}
		else {
			BDBG_MSG(("OPTIONS response:\n %s", optionsResponse));
			delete[] optionsResponse;
		}

		// do DESCRIBE
		BDBG_MSG(("DESCRIBE"));
		char* sdpDescription = session->rtspClient->describeURL(session->url);
		if (sdpDescription == NULL) {
			BDBG_ERR(("Error: Failed to get a SDP description from URL"));
			return;
		}
	
		BDBG_MSG(("Opened URL: %s", session->url));
		BDBG_MSG(("Got SDP description:"));
		BDBG_MSG(("%s", sdpDescription));
	
		// Create a media session object from the SDP description
		BDBG_MSG(("Creating the media session object from the SDP description..."));
		session->rtspMediaSession = MediaSession::createNew(*lm_env, sdpDescription);
		delete[] sdpDescription;
		if (session->rtspMediaSession == NULL) {
			BDBG_ERR(("Error: Failed to create a MediaSession object from the SDP description"));
			return;
		} else if (!session->rtspMediaSession->hasSubsessions()) {
			BDBG_ERR(("Error: This RTSP session has no media subsessions"));
			return;
		}
	
		// do SETUP
		BDBG_MSG(("SETUP"));
		MediaSubsessionIterator iter(*session->rtspMediaSession);
		MediaSubsession *subsession = iter.next();

		// set ephemeral port number
		Port srcPort(0);
		portNumBits portNum = 0;
		getSourcePort(*lm_env, session->rtpGroupsock->socketNum(), srcPort);
		if (srcPort.num() != 0) 
			portNum = ntohs(srcPort.num());
		else
			BDBG_ERR(("Error: error getting ephemeral port number"));

		subsession->setClientPortNum(portNum);
		if (!session->rtspClient->setupMediaSubsession(*subsession)) {
			BDBG_ERR(("Error: Failed to  setup sub-session"));
		} else {
			BDBG_MSG(("Successfully setup %s, %s, on port: %d",
					  subsession->mediumName(),
					  subsession->codecName(),
					  subsession->clientPortNum()));
		}

		// setup destination for RTCP messeages
		Port destPort(subsession->serverPortNum+1);
		unsigned destAddress = subsession->connectionEndpointAddress();
		struct in_addr destAddr; destAddr.s_addr = destAddress;
		BDBG_MSG(("connectionEndpointAddress: %s", inet_ntoa(destAddr)));
		session->rtcpGroupsock->changeDestinationParameters(destAddr, destPort, /*destTTL*/ 1);

		// bitband server has proprietary use of timestamp
		if(hasBitbandRSTPServer(session)) session->timestampFreq = B_BITBAND_TIMESTAMP_FREQ;
	}

	// create the data source: a "MPEG Video RTP source"
	session->source = MPEG2TransportStreamRTPSource::createNew(*lm_env, session->rtpGroupsock, session->timestampFreq);

	// create (and start) a 'RTCP instance' for the RTP source:
	gethostname((char*)CNAME, B_LM_MAX_CNAME_LEN);
	CNAME[B_LM_MAX_CNAME_LEN] = '\0'; // just in case

	// Note: This starts RTCP running automatically
	session->rtcpInstance = RTCPInstance::createNew(*lm_env, session->rtcpGroupsock,
													B_LM_ESTIMATED_RTCP_BW, CNAME,
													NULL /* we're a client */, 
													session->source);

	session->state = lmSessionStateOpened;
}

static void doSessionStart(void *context) {
	liveMediaSessionContext *lmContext = (liveMediaSessionContext *)context;
	liveMediaSession *session = lmContext->session;

#if 0
	printf("@@@ Entered doSessionStart() @ 0x%x, session: 0x%x\n", doSessionStart, (unsigned)session);
#endif

	if(session->protocol == bsettop_ip_protocol_rtsp) {
		// do PLAY
		BDBG_MSG(("PLAY"));
		if(!session->rtspClient->playMediaSession(*session->rtspMediaSession)) {
			// this can happen in response to incompatible server response, which is not a failure mode
			BDBG_MSG(("RTSP PLAY request returned bad value"));
		}
	}

	if(session->state == lmSessionStateOpened) {
		session->periodicTaskPendingCount++;
		BDBG_MSG(("queuing doPeriodicTask() in doSessionStart()\n"));
		blive_scheduler_queue_delayed_task(B_LM_PERIODIC_TASK_USEC, (TaskFunc*)doPeriodicTask, lmContext);
		session->state = lmSessionStateStarted;
	}
}

static void doSessionRTSPControl(void *context) {
	liveMediaSessionContext *lmContext = (liveMediaSessionContext *)context;
	liveMediaSession *session = lmContext->session;
	bplayback_rtsp_ctrl_cmd *cmd = (bplayback_rtsp_ctrl_cmd *)lmContext->params;

#if 0
	printf("@@@ Entered doSessionRTSPControl() @ 0x%x, session: 0x%x\n", doSessionRTSPControl, (unsigned)session);
	printf("cmd type: %d, params.scale: %d, params.start: %d, params.end: %d\n", 
		   cmd->type, 
		   cmd->params.scale, 
		   cmd->params.start, 
		   cmd->params.end);
#endif

	switch(cmd->type) {
	case bplayback_rtsp_ctrl_cmd_type_play:
		// do PLAY
		BDBG_MSG(("PLAY (trick-mode)"));
		// JJ - TODO: this is a hack... should use values reported in OPTIONS
		{
			// convert Broadcom trickmode speed to RTSP scale value
			int scale_tbl[11] = {-24,-16,-8,-4,-2,1,2,4,8,16,24};
			int scale_idx;
			int scale;
			scale_idx = cmd->params.scale/100;	// we don't support slow-motion rtsp yet
			if(scale_idx==0) break;
			if(scale_idx>6) scale_idx = 6;		// we're limited to a range of 11 settings at this time
			if(scale_idx<-5) scale_idx = -5;
			if(scale_idx>0)
				scale_idx += 4;
			else
				scale_idx += 5;
			scale = scale_tbl[scale_idx];
			BDBG_MSG(("scale passed to PLAY: %d", scale));
			if(!session->rtspClient->playMediaSession(*session->rtspMediaSession, 
													  (1.0*session->NPT.tv_sec)+(1.0*session->NPT.tv_usec/1000000.0),
													  cmd->params.end,
													  scale)) {
				BDBG_ERR(("Error: Failed to PLAY (trick-mode)"));
			}
		}
		break;
	case bplayback_rtsp_ctrl_cmd_type_pause:
		// do PAUSE
		BDBG_MSG(("PAUSE"));
		if(!session->rtspClient->pauseMediaSession(*session->rtspMediaSession)) {
			BDBG_ERR(("Error: Failed to PAUSE"));
		}
		break;
	case bplayback_rtsp_ctrl_cmd_type_resume:
		// do PLAY
		BDBG_MSG(("PLAY (resume)"));
		if(!session->rtspClient->playMediaSession(*session->rtspMediaSession, 
												  /*start*/ -1,
												  cmd->params.end,
												  cmd->params.scale)) {
			BDBG_ERR(("Error: Failed to PLAY (trick-mode)"));
		}
		break;
	default:
		BDBG_ERR(("Error: unknown RTSP control command"));
		break;
	}

	// delete the command params, we're managing its lifecycle in this module
	delete cmd;
}

static void doSessionClose(void *context) {
	liveMediaSessionContext *lmContext = (liveMediaSessionContext *)context;
	liveMediaSession *session = lmContext->session;

#if 0
	printf("@@@ Entered doSessionClose() @ 0x%x, session: 0x%x\n", doSessionClose, (unsigned)session);
#endif

	if(session->protocol == bsettop_ip_protocol_rtsp) {
		// do TEARDOWN
		if(!session->rtspClient->teardownMediaSession(*session->rtspMediaSession)) {
			BDBG_ERR(("Error: Failed to TEARDOWN media session"));
		}
		Medium::close(session->rtspMediaSession);
		Medium::close(session->rtspClient);
		session->rtspClient = NULL;
	}

	Medium::close(session->rtcpInstance); // Note: Sends a RTCP BYE
	Medium::close(session->source);

	delete session->rtpGroupsock;
	delete session->rtcpGroupsock;

	free(session->rtpHeader);
	free(session->rtpHdrData);

	// defer session deletion to doPeriodicTask()
	session->state = lmSessionStateClosing;
}

// execute/reschedule periodic tasks
static void doPeriodicTask(void *context) {
	liveMediaSessionContext *lmContext = (liveMediaSessionContext *)context;
	liveMediaSession *session = lmContext->session;

	BDBG_ASSERT(session);

#if 1
	BDBG_MSG(("Entered doPeriodicTask() @ 0x%x, session: 0x%x, session->state: %d, sched_count: %d", 
			  doPeriodicTask, (unsigned)session, session->state, session->periodicTaskSchedCount));
#endif

	session->periodicTaskSchedCount++;
	if(session->periodicTaskPendingCount > 0) session->periodicTaskPendingCount--;

	if(session->state == lmSessionStateStarted) {
		BDBG_ASSERT(session->source);
		if((session->periodicTaskSchedCount%(B_LM_KEEPALIVE_USEC/B_LM_PERIODIC_TASK_USEC))==0)
			sendKeepAlive(session);
		processReceptionStats(session);

		// avoid reschedule if scheduler falls behind
		if(session->periodicTaskPendingCount == 0) {
			session->periodicTaskPendingCount++;
			// reschedule this function
			blive_scheduler_queue_delayed_task(B_LM_PERIODIC_TASK_USEC, (TaskFunc*)doPeriodicTask, lmContext);
		}
	}
	else if(session->state == lmSessionStateClosing) {
		// we're done
		delete session;
		delete lmContext;
	}
}

// process RTP reception statistics
static void processReceptionStats(liveMediaSession *session) {
    Boolean hasBeenSyncedUsingRTCP = false;

	// get the header data from the playpump
#if B_HAS_PLAYPUMP_IP
	if(bplaypump_get_rtp_hdr_data(session->pump, session->rtpHdrData, &session->rtpHdrData->entry_cnt))
		BDBG_ERR(("Error: Returned error from bplaypump_get_status()!"));
#else
    /* TODO: add support for RTCP stats gathering for playback_ip case (which is always user-space, and that includes the brtp module) */
    session->rtpHdrData->entry_cnt = 0;
#endif
	if(session->rtpHdrData->entry_cnt!=0) {
#if 1
		BDBG_MSG(("entry_cnt: %d, first entry seq_num: %u, ssrc:%u, timestamp: %u", 
				  session->rtpHdrData->entry_cnt, session->rtpHeader[0].sequence_num, session->rtpHeader[0].ssrc, 
				  session->rtpHeader[0].timestamp));
#endif

		// process each header via the reception stats database
		for(unsigned i=0; i<session->rtpHdrData->entry_cnt; i++) {
			if(session->prevSeqNum+1 != session->rtpHeader[i].sequence_num && session->prevSeqNum != 65535) {
				BDBG_WRN(("break in RTP sequence number, prevSeqNum: %d, seq_num: %d", 
						  session->prevSeqNum,
						  session->rtpHeader[i].sequence_num));
			}
			session->prevSeqNum = session->rtpHeader[i].sequence_num;
			session->source->receptionStatsDB().noteIncomingPacket(session->rtpHeader[i].ssrc, 
																   session->rtpHeader[i].sequence_num, 
																   session->rtpHeader[i].timestamp,
																   session->timestampFreq,
																   /*True*/ False,
																   session->presentationTime,
																   hasBeenSyncedUsingRTCP,
																   RTP_FIXED_HEADER_SIZE+RTP_PAYLOAD_SIZE);
			if(!session->gotFirstPacket && i==0) {
				session->gotFirstPacket = true;
				session->presentationStartTime = session->presentationTime;
			}
		}
	}

	if(session->protocol == bsettop_ip_protocol_rtsp) {
		if(session->rtpHdrData->entry_cnt) {
			// update our Normal Play Time
			if(hasBitbandRSTPServer(session)) {
				session->NPT.tv_sec = session->rtpHeader[session->rtpHdrData->entry_cnt-1].timestamp/session->timestampFreq;
				session->NPT.tv_usec = session->rtpHeader[session->rtpHdrData->entry_cnt-1].timestamp%session->timestampFreq;
			}
			else {
				timersub(&session->presentationTime, &session->presentationStartTime, &session->NPT);
			}
		}
		BDBG_MSG(("NPT: %ld.%ld, hasBeenSyncedUsingRTCP: %d", session->NPT.tv_sec, session->NPT.tv_usec, hasBeenSyncedUsingRTCP));
	}
}

// send keep-alive message to RTSP server
static void sendKeepAlive(liveMediaSession *session) {
	if(session->protocol == bsettop_ip_protocol_rtsp) {
		if(hasBitbandRSTPServer(session)) {
			// bitband specific keep-alive (needed for pre bitband ver. 5.1 builds)
			session->rtspClient->setMediaSessionParameter(*session->rtspMediaSession, /*parameter*/"Speed", /*value*/"1.0");
		}
		else {
			// a widely supported form of keep-alive
			if(!session->rtspClient->playMediaSession(*session->rtspMediaSession, 
													  /*start*/ -1,
													  /*end*/ -1,
													  1.0)) {
				BDBG_ERR(("Error: Failed to PLAY (keep-alive)"));
			}
		}
	}
}

// determine if the session is using a Bitband RTSP server
static bool hasBitbandRSTPServer(liveMediaSession *session) {
	if(session->rtspClient->serverType() != NULL) {
		if(strcmp(session->rtspClient->serverType(), "BitBand-Vision")==0 ||
		   // TODO: remove "VideoServer" workaround type in preference of acquiring timestamp frequency from SDP
		   strcmp(session->rtspClient->serverType(), "VideoServer")==0) {
			return true;
		}
	}
done:
	return false;
}

