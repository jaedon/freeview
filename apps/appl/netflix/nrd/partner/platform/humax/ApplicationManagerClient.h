#ifndef _NF_APPLICATION_MANAGER_CLIENT_H_
#define _NF_APPLICATION_MANAGER_CLIENT_H_

#include "ApplicationSuspendTypes.h"

class ApplicationManagerClient {
public:
    enum PowerReason {
        PowerReason_Device_Sleep,               // Device will go into a low power state.
        PowerReason_Device_Wake                 // Device has resumed from a low power state.
    };

    enum DummyReason {
        DummyReason = (Reason_LAST_ENTRY_POSITION + 1) // Dummy event that is not passed to Gibbon
    };

    enum Error {
        Error_OK = 0,
        Error_General = -1,
        Error_Timeout = -2,
        Error_State   = -3,
        Error_SuspendFailed = -4,
    };

    ApplicationManagerClient();
    ~ApplicationManagerClient();

    void stop();
    inline bool isRunning() const { return mRunning; }

    // Application notifies that it has entered a started state
    void notifyStarted();
    // Application notifies that it has entered a suspended state
    void notifySuspended();
    // Application requests the application manager suspend the current application
    void notifyRequestSuspend();
    // Application requests the application manager background the current application
    void notifyRequestBackground();
    // Application requests the application manager cleanup resource
    void notifyRequestCleanup();

    struct Callbacks {
        char (*onResume)(const char *payload);          // Application may acquire resources before return.
        char (*onSuspend)(Reason reason);               // Application must release resources before return
        char (*onBackground)(Reason Reason);            // Application has lost focus but does not need to release resources
        char (*onStop)(Reason Reason);                  // Application must terminate and provide a reason.
        void (*onKeyUp)(int);                           // Application is given a key to process.
        void (*onKeyDown)(int);                         // Application is given a key to process.
        char (*onPower)(PowerReason Reason);            // Application needs to notify the application for logging purposes
    };
    void setCallbacks(const Callbacks &cb);
	void (*pfnCB)(void);

private:
    void send(unsigned char);
    int receive(unsigned char *);
    static void * Task(void * ctx);
    pthread_t   mThread;
    bool        mRunning;

    int         mPipe[2];
    int         mApplicationManagerSocketFd;

    char        *mCmdBuf;

    static ApplicationManagerClient * sInstance;

    Callbacks mCb;
};

#endif /* _NF_APPLICATION_MANAGER_CLIENT_H_ */

