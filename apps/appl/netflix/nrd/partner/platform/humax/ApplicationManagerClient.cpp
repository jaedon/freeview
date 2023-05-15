#include <sys/select.h>
#include <sys/socket.h>
#include <sys/fcntl.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <dlfcn.h>

//#include <fcntl.h>
#include <nxlink.h>
#include <time.h>
#include <errno.h>
#include <GibbonApplication.h>

#include "ApplicationManagerClient.h"

#define		AM_CMD_STR_MAX		5120//(5 * 1024)  5 Kbytes

typedef enum _SuspendStatus_t 
{
	NFLIX_SunpendStatus_NA,
	NFLIX_SunpendStatus_Ongoing,
	NFLIX_SunpendStatus_Completed,
	NFLIX_SunpendStatus_Timeout,
	/* NFLIX_SunpendStatus_Exiting, */
	/* NFLIX_SunpendStatus_Exited */
}NFLIX_SuspendStatus_t;


enum ClientCommand 
{ /* TODO : Sync-up with Nflix_ClientCmd_t in Netflixmanager.c */
        //-----------------------------------
        //  Commands from AM to AM client (app)
        //-----------------------------------
        CMD_SUSPEND = 1,
        CMD_RESUME,
        CMD_BACKGROUND,
        CMD_STATUS,

        CMD_SUSPEND_FROM_BACKGROUND,
        CMD_RESUME_LINK,
        CMD_KEY_L,
        CMD_KEY_R,
        CMD_KEY_U,
        CMD_KEY_D,
        CMD_KEY_E,
        CMD_KEY_PAUSE,
        CMD_KEY_PLAY,
        CMD_KEY_OVERLAY_PAUSE, /* overlay popup */
        CMD_KEY_OVERLAY_PLAY, /* overlay close */

        CMD_SUSPEND_APPLICATION_REQUEST,
        CMD_BACKGROUND_APPLICATION_REQUEST,
        
        // Stop cases
        CMD_STOP_LOW_RESOURCES,
        CMD_STOP_USER_KILL,
        CMD_STOP_DIAL,

        CMD_PREPARE_FORCE_KILL,

        // Power events
        CMD_POWER_SLEEP,
        CMD_POWER_WAKE//,
        //CMD_MAX        
 };

enum AMCommand 
{
        CMD_AM_STOP = 0x10,        
            
        //-----------------------------------        
        //  Commands from AM client (app) to AM        
        //-----------------------------------        
        CMD_AM_REQUEST_SUSPEND,        
        CMD_AM_REQUEST_BACKGROUND,        

        // State changes        
        CMD_AM_STARTED,        
        CMD_AM_SUSPENDED    
};


static const char * ApplicationManagerSocketPath() 
{
    const char * c = "/tmp/nfappman";
    return c;    
}  


ApplicationManagerClient::ApplicationManagerClient() : mThread(0), mRunning(false), pfnCB(0)
{
    mCmdBuf = NULL;
    mRunning = false;
        
    
    memset(&mCb, 0, sizeof(mCb));
    if(pipe(mPipe) == -1)
    {
        printf("\n\n [AMClient] Error > unable to create pipe\n");
        return;
    }

    if(fcntl(mPipe[0], F_SETFL, O_NONBLOCK) == -1)
    {
        printf("\n\n [AMClient] Error > call fcntl()\n");
        return;
    }        

    mCmdBuf = (char*)malloc(AM_CMD_STR_MAX);
    if(mCmdBuf == NULL)
    {
        printf("\n\n [AMClient] Error > malloc is failed..!!\n");
        return;
    }
    
    memset(mCmdBuf, 0, AM_CMD_STR_MAX);

    struct sockaddr_un addr;
    if((mApplicationManagerSocketFd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
    {
        printf("\n\n [AMClient] Error > unable to create socket\n");        
    }
    else
    {
        memset(&addr, 0, sizeof(addr));
        addr.sun_family = AF_UNIX;
        strncpy(addr.sun_path, ApplicationManagerSocketPath(), sizeof(addr.sun_path)-1);

        if(connect(mApplicationManagerSocketFd, (struct sockaddr*)&addr, sizeof(addr)) == -1)
        {
            printf("\n\n\n [AMClient] Error > unable to connect to the AM.\n");
            close(mApplicationManagerSocketFd);
            mApplicationManagerSocketFd = -1;
        } 
        else 
        {
            mRunning = true;
        }
    }
}

ApplicationManagerClient::~ApplicationManagerClient()
{
    stop();
    close(mPipe[1]);

    if(mCmdBuf)
    {
        free(mCmdBuf);
        mCmdBuf = NULL;
    }
    
    if(mThread) {
        void *returnValue;
        pthread_join(mThread, &returnValue);
    }
}

void ApplicationManagerClient::send(unsigned char a)
{
    if(mApplicationManagerSocketFd != -1)
    {
        if(::send(mApplicationManagerSocketFd, &a, 1, 0) != 1)
        {
            printf("\n\n\n [AMClient] Error > send() is failed..!!\n");
        }
    }
}

int ApplicationManagerClient::receive(unsigned char *a)
{
    if(mApplicationManagerSocketFd != -1)
    {
        if(a != NULL)
        {
            return ::recv(mApplicationManagerSocketFd, a, 1, 0);
        }
        else
        {
            memset(mCmdBuf, 0, AM_CMD_STR_MAX);
		    return ::recv(mApplicationManagerSocketFd, mCmdBuf, AM_CMD_STR_MAX, 0);
        }
    }
    return -1;
}

void * ApplicationManagerClient::Task(void * ctx)
{
    char rc = 0;
    
    ApplicationManagerClient * AMC = (ApplicationManagerClient *)ctx;
    while(AMC->isRunning()) 
    {
        {
            fd_set set;
            FD_ZERO(&set);
            FD_SET(AMC->mApplicationManagerSocketFd, &set);
            FD_SET(AMC->mPipe[0], &set);

            if(-1 == select(FD_SETSIZE, &set, NULL, NULL, NULL)) {
                printf("Error!\n");
                pthread_exit(NULL);
            }

            if(!AMC->isRunning())
                break;

            if(FD_ISSET(AMC->mPipe[0], &set)) {
                while(1) {
                    unsigned char msg;
                    const int r = read(AMC->mPipe[0], &msg, sizeof(msg));
                    if(r < 0)
                        break;

                    printf("[AMClient::Task] send cmd(%d) \n", msg);
                    AMC->send(msg);

                    if(msg == CMD_AM_STOP)
                    {
                        AMC->mRunning = false;
                        break;
                    }
                }
            }

            if(!FD_ISSET(AMC->mApplicationManagerSocketFd, &set)) {
                continue;
            }
        }

        if(!AMC->receive(NULL)) {
            break;
        }

        switch (AMC->mCmdBuf[0]) 
        {                
            case CMD_SUSPEND_FROM_BACKGROUND:
                if(AMC->mCb.onSuspend) rc = AMC->mCb.onSuspend(static_cast<Reason>(DummyReason));
                break;
                
            case CMD_SUSPEND_APPLICATION_REQUEST:    
            case CMD_SUSPEND:
                if(AMC->mCb.onSuspend) rc = AMC->mCb.onSuspend(Reason_USER_Nav);
                break;

            /*    
            case CMD_RESUME:
                if(AMC->mCb.onResume) rc = AMC->mCb.onResume(NULL,0, (Source)AMC->mCmdBuf[1]);
                break;
                
            case CMD_RESUME_LINK:
                if(AMC->mCb.onResume) rc = AMC->mCb.onResume(&AMC->mCmdBuf[2], 0, (Source)AMC->mCmdBuf[1]);
                break;
            */      
            case CMD_RESUME:               
            case CMD_RESUME_LINK:
                if(AMC->mCb.onResume) rc = AMC->mCb.onResume(&AMC->mCmdBuf[1]);
                break;

            case CMD_BACKGROUND_APPLICATION_REQUEST:
            case CMD_BACKGROUND:
                if(AMC->mCb.onBackground) rc = AMC->mCb.onBackground(Reason_USER_Nav);
                break;
                
            case CMD_STOP_LOW_RESOURCES:
                if(AMC->mCb.onStop) rc = AMC->mCb.onStop(Reason_AM_LowResource);
                AMC->stop();
                break;
                
            case CMD_STOP_USER_KILL:
                if(AMC->mCb.onStop) rc = AMC->mCb.onStop(Reason_USER_Kill);
                AMC->stop();
                break;
                
            case CMD_STOP_DIAL:
                if(AMC->mCb.onStop) rc = AMC->mCb.onStop(Reason_DIAL_Stop);
                AMC->stop();
                break;
                
            case CMD_KEY_L:
                if(AMC->mCb.onKeyDown) AMC->mCb.onKeyDown(0x25);
                if(AMC->mCb.onKeyUp) AMC->mCb.onKeyUp(0x25);
                break;
            case CMD_KEY_R:
                if(AMC->mCb.onKeyDown) AMC->mCb.onKeyDown(0x27);
                if(AMC->mCb.onKeyUp) AMC->mCb.onKeyUp(0x27);
                break;
            case CMD_KEY_U:
                if(AMC->mCb.onKeyDown) AMC->mCb.onKeyDown(0x26);
                if(AMC->mCb.onKeyUp) AMC->mCb.onKeyUp(0x26);
                break;
            case CMD_KEY_D:
                if(AMC->mCb.onKeyDown) AMC->mCb.onKeyDown(0x28);
                if(AMC->mCb.onKeyUp) AMC->mCb.onKeyUp(0x28);
                break;
            case CMD_KEY_E:
                if(AMC->mCb.onKeyDown) AMC->mCb.onKeyDown(0x0D);
                if(AMC->mCb.onKeyUp) AMC->mCb.onKeyUp(0x0D);
                break;
            case CMD_POWER_SLEEP:
                if(AMC->mCb.onPower) rc = AMC->mCb.onPower(PowerReason_Device_Sleep);
                break;
            case CMD_POWER_WAKE:
                if(AMC->mCb.onPower) rc = AMC->mCb.onPower(PowerReason_Device_Wake);
                break;

            case CMD_KEY_PAUSE:			    
			    {
			        //(void)NXL_PLAYBACKGROUP_CheckLastPlayCmd(&currentPlaybackState);
			        if (AMC->mCb.onKeyDown) AMC->mCb.onKeyDown(0x75);
			        if (AMC->mCb.onKeyUp) AMC->mCb.onKeyUp(0x75);        
			    }
			    break;        
			case CMD_KEY_PLAY:			    
			    {
			        //(void)NXL_PLAYBACKGROUP_CheckLastPlayCmd(&currentPlaybackState);
			        if (AMC->mCb.onKeyDown) AMC->mCb.onKeyDown(0x77);
			        if (AMC->mCb.onKeyUp) AMC->mCb.onKeyUp(0x77);
			    }
			    break;
                
            default:
                // Ignore unknown messages
                break;
        }
        printf("[AMClient::Task] send result(%d) for command(%d)\n", rc, (int)AMC->mCmdBuf[0]);
        AMC->send(rc);
    }

    close(AMC->mPipe[0]);
    close(AMC->mPipe[1]);
    close(AMC->mApplicationManagerSocketFd);
    AMC->mThread = 0;

    printf("\n\n [AMClient] exiting task\n");
    pthread_exit(NULL);
}

void ApplicationManagerClient::notifyRequestSuspend()
{
    const unsigned char cmd = CMD_AM_REQUEST_SUSPEND;
    if(write(mPipe[1], &cmd, sizeof(cmd)) == -1)
        printf("unable to send command: %d\n", cmd);
}

void ApplicationManagerClient::notifySuspended()
{
    const unsigned char cmd = CMD_AM_SUSPENDED;
    if(write(mPipe[1], &cmd, sizeof(cmd)) == -1)
        printf("unable to send command: %d\n", cmd);
}

void ApplicationManagerClient::notifyStarted()
{
    const unsigned char cmd = CMD_AM_STARTED;
    if(write(mPipe[1], &cmd, sizeof(cmd)) == -1)
        printf("unable to send command: %d\n", cmd);
}

void ApplicationManagerClient::notifyRequestBackground(void)
{
    const unsigned char cmd = CMD_AM_REQUEST_BACKGROUND;
    if(write(mPipe[1], &cmd, sizeof(cmd)) == -1)
        printf("unable to send command: %d\n", cmd);
}

void ApplicationManagerClient::setCallbacks(const Callbacks &cb)
{
    if(isRunning()) {
        mCb = cb;
        pthread_create(&mThread, NULL, &ApplicationManagerClient::Task, this);
    }
}

void ApplicationManagerClient::stop()
{
   //mRunning = false;

    if(mPipe[1] != -1) {
        const unsigned char cmd = CMD_AM_STOP;
        if(write(mPipe[1], &cmd, sizeof(cmd)) == -1)
            printf("unable to send command: %d\n", cmd);
    }
}
