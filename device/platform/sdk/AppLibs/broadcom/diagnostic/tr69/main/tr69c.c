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

#include "../inc/appdefs.h"
#include "../inc/utils.h"
#include "../inc/event.h"
#include "../inc/informer.h"
#include "../inc/CPEframework.h"
#include "../inc/protocol.h"
#include "../inc/bcmWrapper.h"
#include "../inc/RPCState.h"
#include "../inc/tr69cdefs.h"
#include "../inc/bcm_types.h"

#define CONF_FILE_PATH "/mnt/flash/data/tr69c.conf"
//#define NEXUS_SUPPORT=0
//BcmCfm_TR069Cfg_t     defTr69;

//ACSState acsState;

int tr69cTerm = 0;      /*TR69C termination flag*/

void sigIgnore(int sig)
{
        BSTD_UNUSED(sig);
    //slog(LOG_DEBUG, "Ignore sig %d");
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
       // resetPeriodicInform(1);
}

/*
* Initialize all the various tasks
 */
static void initTasks(void)
{
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

    //slog(LOG_DEBUG,"Daemonizing process ");
    if (fork()!=0) exit(0);
    setsid();

}

int main(int argc, char** argv)
{

#ifdef NEXUS_SUPPORT
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;

    /* Bring up all modules for a platform in a default configuration for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);
#endif

    int verbose = 0;
    int no_daemonize=0;
    char    c;
        int             fd;
        int             numBytes;


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
    initTr69();
    printf("Before eventLoop\n");
    eventLoop();

    printf("After eventLoop\n");
    /* example shutdown */
   // NEXUS_Platform_Uninit();

    return 0;
}

