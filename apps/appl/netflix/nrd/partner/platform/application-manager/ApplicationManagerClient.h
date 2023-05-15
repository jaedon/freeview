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

    ApplicationManagerClient();
    ~ApplicationManagerClient();

    void stop();
    inline bool isRunning() const { return mRunning; }

    // Application notifies that it has entered a suspended state
    void notifyStarted();
    // Application notifies that it has entered a suspended state
    void notifySuspended();
    // Application requests the application manager suspend the current application
    void notifyRequestSuspend();
    // Application requests the application manager background the current application
    void notifyRequestBackground();

    struct Callbacks {
        void (*onResume)(const char * p, int TrackId, Source source); // Application may acquire resources before return.
        void (*onSuspend)(Reason reason);               // Application must release resources before return
        void (*onBackground)(Reason Reason);            // Application has lost focus but does not need to release resources
        void (*onStop)(Reason Reason);                  // Application must terminate and provide a reason.
        void (*onKeyUp)(int);                           // Application is given a key to process.
        void (*onKeyDown)(int);                         // Application is given a key to process.
        void (*onPower)(PowerReason Reason);            // Application needs to notify the application for logging purposes
    };
    void setCallbacks(const Callbacks &cb);

private:
    void send(unsigned char);
    int receive(unsigned char *);
    static void * Task(void * ctx);
    pthread_t   mThread;
    bool        mRunning;

    int         mPipe[2];
    int         mApplicationManagerSocketFd;

    static ApplicationManagerClient * sInstance;

    Callbacks mCb;
};

#endif /* _NF_APPLICATION_MANAGER_CLIENT_H_ */

