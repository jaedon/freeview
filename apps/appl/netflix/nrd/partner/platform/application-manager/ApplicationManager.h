#ifndef _NF_APPLICATION_MANAGER_H_
#define _NF_APPLICATION_MANAGER_H_

#include <nrdbase/tr1.h>

#include <unistd.h>
#include <string.h>
#include <sys/signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <vector>
#include <list>

class ApplicationManager {
public:
    ApplicationManager();
    ~ApplicationManager();

    //-----------------------------------
    //  Commands from AM to AM client (app)
    //-----------------------------------
    enum AMCommand {
        CMD_AM_STOP,

        //-----------------------------------
        //  Commands from AM client (app) to AM
        //-----------------------------------
        CMD_AM_REQUEST_SUSPEND,
        CMD_AM_REQUEST_BACKGROUND,

        // State changes
        CMD_AM_STARTED,
        CMD_AM_SUSPENDED
    };

    enum ClientCommand {
        //-----------------------------------
        //  Commands from AM to AM client (app)
        //-----------------------------------
        CMD_SUSPEND,
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

        CMD_SUSPEND_APPLICATION_REQUEST,
        CMD_BACKGROUND_APPLICATION_REQUEST,

        // Stop cases
        CMD_STOP_LOW_RESOURCES,
        CMD_STOP_USER_KILL,
        CMD_STOP_DIAL,

        // Power events
        CMD_POWER_SLEEP,
        CMD_POWER_WAKE
    };

    enum ControllerCommand {
        //-----------------------------------
        //  Commands from AM controller to AM
        //-----------------------------------
        CMD_CNT_SUSPEND,            // Suspend app
        CMD_CNT_RESUME,             // Resume app
        CMD_CNT_STOP_LOW_RESOURCES, // Terminate app nicely because of anticipated resource contention.
        CMD_CNT_STOP_USER_KILL,     // Terminate app nicely because the user killed Netflix.
        CMD_CNT_STOP_DIAL,          // Terminate app nicely DIAL wants to kill Netflix.
        CMD_CNT_BACKGROUND,         // Background app

        CMD_CNT_EXIT,               // Terminate AM & app

        CMD_CNT_SIGKILL,            // Send SIGKILL to app process

        CMD_CNT_RESUME_LINK,        // Resume app with content to play

        CMD_CNT_POWER_SLEEP,        // Device will go into a low power state
        CMD_CNT_POWER_WAKE,         // Device resumed from a low power state

        CMD_CNT_KEY_L,
        CMD_CNT_KEY_R,
        CMD_CNT_KEY_U,
        CMD_CNT_KEY_D,
        CMD_CNT_KEY_E
    };

    enum Error {
        Error_General = -1,
        Error_Timeout = -2,
        Error_State   = -3
    };

    class Application {
    public:
        enum State {
            INVALID         = 0x00,
            SUSPENDED       = 0x01,
            FOREGROUNDED    = 0x02,
            BACKGROUNDED    = 0x04,
            SHUTDOWN        = 0x08,
            NO_CHANGE
        };

        Application();
        inline ~Application() { kill(); }

        inline pid_t pid() const { return mPid; }
        inline int fd() const { return mFD; }

        inline bool shouldRestart() const { return mCrashed || mRestart; }

        inline State state() const { return mState; }
        inline bool setState(State state) {
            if(mState == state)
                return true;
            if(state == SUSPENDED) {
                if(!setSuspended(true)) {
                    mState = state;
                    return true;
                }
            } else if(state == FOREGROUNDED) {
                if(!setSuspended(false)) {
                    mState = state;
                    return true;
                }
            }
            return false;
        }

        bool handleCommand(AMCommand command);
        int handleCommand(ControllerCommand command);
        State stateForCommand(ControllerCommand command) const;

        inline bool isRunning() const { return mPid; }

        char send(ClientCommand cmd);
        void handleSignal(int status);

        void start(const std::vector<const char *> &args);
        bool stop();
        bool kill();
    private:
        int setSuspended(bool s) {
            if(!s)
                return send(CMD_RESUME);
            if(mSupportBackground || (mState == BACKGROUNDED))
                return send(CMD_SUSPEND_FROM_BACKGROUND);
            return send(CMD_SUSPEND);
        }
        int doExit() {
            if(!send(CMD_STOP_USER_KILL))
                return 0;
            if(!stop())
                return Error_Timeout;
            return 0;
        }
        int doSuspend() { return setSuspended(true); }
        int doStopDial() {
            if(!send(CMD_STOP_DIAL))
                return 0;
            if(!stop())
                return Error_Timeout;
            mRestart = true;
            return 0;
        }
        int doStopUserKill() {
            if(!send(CMD_STOP_USER_KILL))
                return 0;
            if(!stop())
                return Error_Timeout;
            mRestart = true;
            return 0;
        }
        int doStopLowResources() {
            if(!send(CMD_STOP_LOW_RESOURCES))
                return 0;
            if(!stop())
                return Error_Timeout;
            mRestart = true;
            return 0;
        }
        int doBackground() { return send(CMD_BACKGROUND); }
        int doResume() { return setSuspended(false); }
        int doResumeLink() { return send(CMD_RESUME_LINK); }
        int doSigKill() {
            if(!kill())
                return 0;
            return Error_State;
        }
        int doSleep() { return send(CMD_POWER_SLEEP); }
        int doWake() { return send(CMD_POWER_WAKE); }
        int doKeyLeft() { return send(CMD_KEY_L); }
        int doKeyRight() { return send(CMD_KEY_R); }
        int doKeyUp() { return send(CMD_KEY_U); }
        int doKeyDown() { return send(CMD_KEY_D); }
        int doKeyEnter() { return send(CMD_KEY_E); }

        int mFD;
        pid_t mPid;
        bool mCrashed, mRestart;
        State mInitialState, mState;
        struct CommandEntry {
            ControllerCommand   command;
            int                 validStates;
            State               newState;
            int (Application::*action)(void);
        };
        std::list<CommandEntry> mCommandTable;
        bool mSupportBackground;
    };

    void handleStateChange(const Application *application, Application::State state);

    static const char * ControllerCommandToString(ControllerCommand cmd);
    static ControllerCommand StringToControllerCommand(const char * cmd);
    static void ListCommands(void);

    static const char * ControllerSocketPath() {
        const char * c = "/tmp/nfappmancon";
        return c;
    }

    static const char * ApplicationSocketPath() {
        const char * c = "/tmp/nfappman";
        return c;
    }

    static const int TimeoutInSec = 5;
    static const int RestartTimeoutInSec = 10;
    static const int AppRequestTimeoutInUsec = 100000;

    static void SignalHandler(int);

    int attachApplication() const;
    void setTilesVisible(bool v);

    int exec(int argc, char *argv[]);

private:
    std::vector<const char *> mArgs;

    struct sockaddr_un mControllerAddr;
    struct sockaddr_un mApplicationAddr;

    bool mRunning;
    bool mTileManager;
    int mControllerFd, mApplicationFd;

    struct CmdEntry {
        ControllerCommand   cmd;
        const char *        cmd_str;
        const char *        desc;
    };

    static CmdEntry mCmdTable[];

    shared_ptr<Application> mApplication, mTiles;

    int handleCommand(ControllerCommand cmd);
};

#endif /* _NF_APPLICATION_MANAGER_H_ */
