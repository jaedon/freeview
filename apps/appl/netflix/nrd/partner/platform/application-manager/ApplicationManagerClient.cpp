#include <sys/select.h>
#include <sys/socket.h>
#include <sys/fcntl.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <dlfcn.h>
#include "ApplicationManager.h"
#include "ApplicationManagerClient.h"

ApplicationManagerClient::ApplicationManagerClient() : mThread(0), mRunning(false)
{
    memset(&mCb, 0, sizeof(mCb));
    if(pipe(mPipe) == -1)
        printf("Application Manager Client is unable to create pipe\n");

    fcntl(mPipe[0], F_SETFL, O_NONBLOCK);

    struct sockaddr_un addr;
    if((mApplicationManagerSocketFd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
        printf("Application Manager Client is unable to create socket\n");

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, ApplicationManager::ApplicationSocketPath(), sizeof(addr.sun_path)-1);

    if(connect(mApplicationManagerSocketFd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        printf("Application Manager Client is unable to connect to the Application Manager.\n");
        close(mApplicationManagerSocketFd);
        mApplicationManagerSocketFd = -1;
    } else {
        mRunning = true;
    }
}

ApplicationManagerClient::~ApplicationManagerClient()
{
    stop();
    close(mPipe[1]);
    if(mThread) {
        void *returnValue;
        pthread_join(mThread, &returnValue);
    }
}

void ApplicationManagerClient::send(unsigned char a)
{
    if(mApplicationManagerSocketFd != -1)
        ::send(mApplicationManagerSocketFd, &a, 1, 0);
}

int ApplicationManagerClient::receive(unsigned char *a)
{
    if(mApplicationManagerSocketFd != -1)
        return ::recv(mApplicationManagerSocketFd, a, 1, 0);
    return -1;
}

void * ApplicationManagerClient::Task(void * ctx)
{
    ApplicationManagerClient * AMC = (ApplicationManagerClient *)ctx;
    while(AMC->isRunning()) {
        unsigned char a = 0;

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
                    AMC->send(msg);
                }
            }

            if(!FD_ISSET(AMC->mApplicationManagerSocketFd, &set)) {
                continue;
            }
        }

        if(!AMC->receive(&a)) {
            break;
        }

        switch (a) {
        case ApplicationManager::CMD_SUSPEND_APPLICATION_REQUEST:
            if(AMC->mCb.onSuspend) AMC->mCb.onSuspend(static_cast<Reason>(DummyReason));
            break;
        case ApplicationManager::CMD_BACKGROUND_APPLICATION_REQUEST:
            if(AMC->mCb.onBackground) AMC->mCb.onBackground(static_cast<Reason>(DummyReason));
            break;
        case ApplicationManager::CMD_SUSPEND_FROM_BACKGROUND:
            if(AMC->mCb.onSuspend) AMC->mCb.onSuspend(static_cast<Reason>(DummyReason));
            break;
        case ApplicationManager::CMD_SUSPEND:
            if(AMC->mCb.onSuspend) AMC->mCb.onSuspend(Reason_USER_Nav);
            break;
        case ApplicationManager::CMD_RESUME:
            if(AMC->mCb.onResume) AMC->mCb.onResume(NULL,0,Source_TESTING);
            break;
        case ApplicationManager::CMD_RESUME_LINK:
            if(AMC->mCb.onResume) AMC->mCb.onResume("http%3A%2F%2Fapi.netflix.com%2Fcatalog%2Ftitles%2Fmovies%2F70103775", 6054700, Source_TESTING);
            break;
        case ApplicationManager::CMD_BACKGROUND:
            if(AMC->mCb.onBackground) AMC->mCb.onBackground(Reason_USER_Nav);
            break;
        case ApplicationManager::CMD_STOP_LOW_RESOURCES:
            if(AMC->mCb.onStop) AMC->mCb.onStop(Reason_AM_LowResource);
            AMC->stop();
            break;
        case ApplicationManager::CMD_STOP_USER_KILL:
            if(AMC->mCb.onStop) AMC->mCb.onStop(Reason_USER_Kill);
            AMC->stop();
            break;
        case ApplicationManager::CMD_STOP_DIAL:
            if(AMC->mCb.onStop) AMC->mCb.onStop(Reason_DIAL_Stop);
            AMC->stop();
            break;
        case ApplicationManager::CMD_KEY_L:
            if(AMC->mCb.onKeyDown) AMC->mCb.onKeyDown(0x25);
            if(AMC->mCb.onKeyUp) AMC->mCb.onKeyUp(0x25);
            break;
        case ApplicationManager::CMD_KEY_R:
            if(AMC->mCb.onKeyDown) AMC->mCb.onKeyDown(0x27);
            if(AMC->mCb.onKeyUp) AMC->mCb.onKeyUp(0x27);
            break;
        case ApplicationManager::CMD_KEY_U:
            if(AMC->mCb.onKeyDown) AMC->mCb.onKeyDown(0x26);
            if(AMC->mCb.onKeyUp) AMC->mCb.onKeyUp(0x26);
            break;
        case ApplicationManager::CMD_KEY_D:
            if(AMC->mCb.onKeyDown) AMC->mCb.onKeyDown(0x28);
            if(AMC->mCb.onKeyUp) AMC->mCb.onKeyUp(0x28);
            break;
        case ApplicationManager::CMD_KEY_E:
            if(AMC->mCb.onKeyDown) AMC->mCb.onKeyDown(0x0D);
            if(AMC->mCb.onKeyUp) AMC->mCb.onKeyUp(0x0D);
            break;
        case ApplicationManager::CMD_POWER_SLEEP:
            if(AMC->mCb.onPower) AMC->mCb.onPower(PowerReason_Device_Sleep);
            break;
        case ApplicationManager::CMD_POWER_WAKE:
            if(AMC->mCb.onPower) AMC->mCb.onPower(PowerReason_Device_Wake);
            break;
        default:
            // Ignore unknown messages
            break;
        }

        AMC->send(0);
    }

    close(AMC->mPipe[0]);
    close(AMC->mPipe[1]);
    close(AMC->mApplicationManagerSocketFd);
    AMC->mThread = 0;

    printf("exiting ApplicationManagerClient task\n");
    pthread_exit(NULL);
}

void ApplicationManagerClient::notifyRequestSuspend()
{
    const unsigned char cmd = ApplicationManager::CMD_AM_REQUEST_SUSPEND;
    if(write(mPipe[1], &cmd, sizeof(cmd)) == -1)
        printf("unable to send command: %d\n", cmd);
}

void ApplicationManagerClient::notifySuspended()
{
    const unsigned char cmd = ApplicationManager::CMD_AM_SUSPENDED;
    if(write(mPipe[1], &cmd, sizeof(cmd)) == -1)
        printf("unable to send command: %d\n", cmd);
}

void ApplicationManagerClient::notifyStarted()
{
    const unsigned char cmd = ApplicationManager::CMD_AM_STARTED;
    if(write(mPipe[1], &cmd, sizeof(cmd)) == -1)
        printf("unable to send command: %d\n", cmd);
}

void ApplicationManagerClient::notifyRequestBackground(void)
{
    const unsigned char cmd = ApplicationManager::CMD_AM_REQUEST_BACKGROUND;
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
    mRunning = false;
    if(mPipe[1] != -1) {
        const unsigned char cmd = ApplicationManager::CMD_AM_STOP;
        if(write(mPipe[1], &cmd, sizeof(cmd)) == -1)
            printf("unable to send command: %d\n", cmd);
    }
}
