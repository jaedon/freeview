/*----------------------------------------------------------------------*
<:copyright-broadcom 
 
 Copyright (c) 2005 Broadcom Corporation 
 All Rights Reserved 
 No portions of this material may be reproduced in any form without the 
 written permission of: 
          Broadcom Corporation 
          16215 Alton Parkway 
          Irvine, California 92619 
 All information contained in this document is Broadcom Corporation 
 company private, proprietary, and trade secret. 
 
:>
 *----------------------------------------------------------------------*
 * File Name  : main.c for tr69 CPE client
 *
 * Description: 
 *   
 *   
 *----------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/signal.h>
#include <sys/time.h>
#include <string.h>
#include <syslog.h>
#include <netinet/in.h>
#include <netdb.h>
#include <linux/if.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include "nexus_platform.h"
#include "../inc/appdefs.h"
#include "utils.h"
#include "event.h"
#include "informer.h"
#include "../SOAPParser/CPEframework.h"
#include "../webproto/protocol.h"
#include "../bcmLibIF/bcmWrapper.h"
#include "../SOAPParser/RPCState.h"
#include "../inc/tr69cdefs.h"
#include "../bcmLibIF/bcm_types.h"

#ifndef TESTBOX
/*#include "bcmprocfs.h"*/
#ifdef DMALLOC
#include "dmalloc.h"
#endif
#endif

#define CONF_FILE_PATH "/mnt/flash/data/tr69c.conf"
//BcmCfm_TR069Cfg_t	defTr69;

ACSState acsState;

int tr69cTerm = 0;	/*TR69C termination flag*/

void sigIgnore(int sig)
{
	BSTD_UNUSED(sig);
    slog(LOG_DEBUG, "Ignore sig %d");
    return;
}

void tr69c_sigTermHandler(int sig)
{
	BSTD_UNUSED(sig);
	/*printf("TR69C terminating......\n");*/
 	tr69cTerm = 1;

	/*Set the timer to send out an inform to ACS.  The purpose is to
	 *create an ACS disconnect event so that the tr69c termination
	 *flag will be examined and acted on.
	 */
	resetPeriodicInform(1);
}

/*
* Initialize all the various tasks 
 */
static void initTasks(void)
{
    /* debugging init */
#ifdef TESTBOX
printf("insize test box\n");
    extern void initTestSetup();
    initTestSetup();
#endif
    /* INIT Protocol http, ssl */
printf("init protocol\n");
    proto_Init();
    /* init bcm library interface */
printf("init BCMLibIF\n");
    initBCMLibIF();
    /* Just booted so send initial Inform */
printf("init Informer\n");
    initInformer();
	/* init cfm listener */
printf("init CFMListentner\n");
	initCFMListener();
}

static void daemonize(void) {

    slog(LOG_DEBUG,"Daemonizing process ");
    if (fork()!=0) exit(0);
    setsid();

}

void init_tr69_def(void)
{
#if 0
	defTr69.status			= BcmCfm_CfgDisabled;
	defTr69.connReqUser 	= strdup(ACSCONNPATH);
	defTr69.acsUser 		= strdup(ACS_USER);
	defTr69.acsPwd 			= strdup(ACS_PASSWD);
	defTr69.connReqUser 	= strdup(ACSCONN_USER);
	defTr69.connReqPwd 		= strdup(ACSCONN_PASSWD);
	defTr69.upgradesManaged = 1;
#endif
	acsState.acsURL			= strdup(ACS_URL);
	acsState.acsUser 		= strdup(ACS_USER);
	acsState.acsPwd 		= strdup(ACS_PASSWD);
	acsState.connReqPath 	= strdup(ACSCONNPATH);
	acsState.connReqUser 	= strdup(ACSCONN_USER);
	acsState.connReqPwd 	= strdup(ACSCONN_PASSWD);
	acsState.kickURL		= strdup(ACS_KICK_URL);
	acsState.provisioningCode = strdup(ACS_DEF_STRING);
	acsState.downloadCommandKey = NULL;
	acsState.rebootCommandKey 	= NULL;
	acsState.rebootCommandKey 	= NULL;
	acsState.parameterKey 		= strdup(ACS_DEF_STRING);
	acsState.newParameterKey 	= NULL;
	acsState.upgradesManaged = 1;
	acsState.dlFaultStatus = 0;
	acsState.startDLTime = 0;
	acsState.endDLTime = 0;
	acsState.informEnable = 0;
	acsState.informInterval = 10;
	acsState.informTime = 0;
}

int main(int argc, char** argv)
{
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    int verbose = 0;
    int no_daemonize=0;
    char    c;
	int		fd;
	int		numBytes;

    /* Bring up all modules for a platform in a default configuration for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

#if 0
	for (i=0; i<100000;i++)
	{
		cptr = malloc(20);
		printf("malloc: %d\n", i);
	}
#endif

    printf("Starting TR69\n");
#if 0
	fd = open((char *)CONF_FILE_PATH, O_RDWR);
	if (fd < 0)
	{
		printf("Initialize basic config data from default values\n");
		init_tr69_def();
		fd = open((char *)CONF_FILE_PATH, O_CREAT|O_RDWR);
		if (fd < 0)
		{
			printf("!!!Error opening a file\n");
			return 0;
		}
		write(fd, &acsState, sizeof(ACSState));
		close(fd); 
	}
	else
	{
		printf("Initialize basic config data from existing file\n");
		if ( (numBytes = read(fd, &acsState, sizeof(ACSState))) != sizeof(ACSState))
		{
			printf("!!!Error opening a file\n");
			return 0;
		}
		close(fd); 
	}
#else
		printf("Initialize basic config data from default values\n");
		init_tr69_def();
#endif
    #ifdef WRITEPIDFILE
    FILE *pidfile;
    mkdirs(TR69_DIR);

    pidfile = fopen(TR69_PID_FILE, "w");
    if (pidfile != NULL) {
        fprintf(pidfile, "%d\n", getpid());
        fclose(pidfile);
    }
    #endif
    
    while ((c=getopt(argc, argv, "dv")) != -1) {
        switch (c) {
        case 'v':
            verbose = 1;
            break;
        case 'd':
            no_daemonize = 1;
            break;
        default:
            break;
        }
    }

    initLog(verbose);
    if (!no_daemonize)
        daemonize();

    /* set signal masks */
    signal(SIGPIPE, SIG_IGN); /* Ignore SIGPIPE signals */
    signal(SIGTERM, tr69c_sigTermHandler);

    printf("InitTask\n");
    initTasks();
    printf("Before eventLoop\n");
    eventLoop();

    printf("After eventLoop\n");
    /* example shutdown */
    NEXUS_Platform_Uninit();

    return 0;
}

#if 0

#ifdef TESTBOX
int main(int argc, char** argv)
{
    int verbose = 0;
    int no_daemonize=0;
    char    c;

    #ifdef WRITEPIDFILE
    FILE *pidfile;
    mkdirs(TR69_DIR);

    pidfile = fopen(TR69_PID_FILE, "w");
    if (pidfile != NULL) {
        fprintf(pidfile, "%d\n", getpid());
        fclose(pidfile);
    }
    #endif
    
    while ((c=getopt(argc, argv, "dv")) != -1) {
        switch (c) {
        case 'v':
            verbose = 1;
            break;
        case 'd':
            no_daemonize = 1;
            break;
        default:
            break;
        }
    }
#else
int tr69c_main(void)
{
    int verbose = 1;
    int no_daemonize = 1;
	int fd;
    
#ifdef DEBUG
	fprintf(stderr, "******* tr69c_main called **********************\n");
#endif // DEBUG      
    
    if ((fd = open("/dev/bcm", O_RDWR)) < 0)
        perror("TR69C:open");
    if (ioctl(fd, RENAME_TR69C, 0) < 0)
        perror("ioctl");
    if ( fd > 0)
        close(fd);

#endif // TESTBOX

    initLog(verbose);
    if (!no_daemonize)
        daemonize();

    /* set signal masks */
    signal(SIGPIPE, SIG_IGN); /* Ignore SIGPIPE signals */
	signal(SIGTERM, tr69c_sigTermHandler);

    initTasks();
    eventLoop();
    return 0;
}

#endif
