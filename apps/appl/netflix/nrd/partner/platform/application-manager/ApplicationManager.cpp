#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <signal.h>
#include <assert.h>
#include <sys/wait.h>

#include <nrdbase/Log.h>

#include "ApplicationManager.h"

static ApplicationManager *sApplicationManager = 0;

ApplicationManager::CmdEntry ApplicationManager::mCmdTable[] = {
    { ApplicationManager::CMD_CNT_SUSPEND,           "suspend",             "Tell the application to move into 'suspend' state."},
    { ApplicationManager::CMD_CNT_BACKGROUND,        "background",          "Tell the application into 'background' state."},
    { ApplicationManager::CMD_CNT_RESUME,            "resume",              "Tell the application to resume from 'background' or 'suspend' state."},
    { ApplicationManager::CMD_CNT_RESUME_LINK,       "resume_link",         "Tell the application to resume from 'background' or 'suspend' state with an asset to play."},
    { ApplicationManager::CMD_CNT_STOP_LOW_RESOURCES,"stop_resources",      "Tell the application to self terminate because of an existing or anticipated resource constraint."},
    { ApplicationManager::CMD_CNT_STOP_USER_KILL,    "stop_user",           "Tell the application to self terminate because the user explicity requests it."},
    { ApplicationManager::CMD_CNT_STOP_DIAL,         "stop_dial",           "Tell the application to self terminate because of a DIAL stop."},
    { ApplicationManager::CMD_CNT_EXIT,              "exit",                "Exit the application and the application manager."},
    { ApplicationManager::CMD_CNT_SIGKILL,           "sigkill",             "Send the SIGKILL signal to the application to simulate a crash."},
    { ApplicationManager::CMD_CNT_KEY_L,             "left",                "Send the 'left' key pair (press/release) to the application."},
    { ApplicationManager::CMD_CNT_KEY_R,             "right",               "Send the 'right' key pair (press/release) to the application."},
    { ApplicationManager::CMD_CNT_KEY_U,             "up",                  "Send the 'up' key pair (press/release) to the application."},
    { ApplicationManager::CMD_CNT_KEY_D,             "down",                "Send the 'down' key pair (press/release) to the application."},
    { ApplicationManager::CMD_CNT_KEY_E,             "enter",               "Send the 'enter' key pair (press/release) to the application."},
    { ApplicationManager::CMD_CNT_POWER_SLEEP,       "sleep",               "Send the 'sleep' event to the application for logging."},
    { ApplicationManager::CMD_CNT_POWER_WAKE,        "wake",                "Send the 'wake' event to the application for logging."}
};

ApplicationManager::ApplicationManager() : mTileManager(false), mControllerFd(-1) , mApplicationFd(-1)
{
    assert(!sApplicationManager);
    sApplicationManager = this;
}

ApplicationManager::~ApplicationManager()
{
    assert(sApplicationManager == this);
    sApplicationManager = 0;
}

void ApplicationManager::setTilesVisible(bool v)
{
    if(mTileManager) {
        if(v) {
            if(!mTiles) {
                mTiles.reset(new Application);
                std::vector<const char*> args;
                args.push_back(mArgs[0]);
                args.push_back("-x");
                args.push_back("tilemanager");
                args.push_back("--no-console"); //give it to the application
                mTiles->start(args);
            }
            mTiles->setState(Application::FOREGROUNDED);
            printf("Show TILES!\n");
        } else if(mTiles) {
            mTiles->setState(Application::SUSPENDED);
            printf("Hide TILES!\n");
        }
    }
}


void ApplicationManager::handleStateChange(const Application *application, Application::State state)
{
    if(mTileManager && application == mApplication.get()) {
        if(state == Application::FOREGROUNDED) {
            setTilesVisible(false);
        } else if(state == Application::SUSPENDED || state == Application::BACKGROUNDED) {
            setTilesVisible(true);
        } else if(mTiles) {
            printf("Kill TILES!\n");
            mTiles->stop();
            mTiles.reset();
        }
    }
}

int ApplicationManager::handleCommand(ControllerCommand cmd)
{
    const Application::State state = mApplication->stateForCommand(cmd);
    if(state == Application::INVALID)
        return Error_State;
    if(state != Application::NO_CHANGE)
        handleStateChange(mApplication.get(), state);
    return mApplication->handleCommand(cmd);
}

ApplicationManager::ControllerCommand ApplicationManager::StringToControllerCommand(const char * cmd)
{
    for (unsigned int i = 0; i < sizeof(ApplicationManager::mCmdTable)/sizeof(ApplicationManager::mCmdTable[0]); i++) {
        if(!strcmp(ApplicationManager::mCmdTable[i].cmd_str, cmd)) {
            return ApplicationManager::mCmdTable[i].cmd;
        }
    }
    return (ControllerCommand)-1;
}

const char * ApplicationManager::ControllerCommandToString(ApplicationManager::ControllerCommand cmd)
{
    const char * s = "unknown";
    for (unsigned int i = 0; i < sizeof(ApplicationManager::mCmdTable)/sizeof(ApplicationManager::mCmdTable[0]); i++) {
        if(ApplicationManager::mCmdTable[i].cmd == cmd) {
            return ApplicationManager::mCmdTable[i].cmd_str;
        }
    }
    return s;
}

void ApplicationManager::ListCommands(void)
{
    for (unsigned int i = 0; i < sizeof(ApplicationManager::mCmdTable)/sizeof(ApplicationManager::mCmdTable[0]); i++)
        printf("%20s  %s\n", ApplicationManager::mCmdTable[i].cmd_str, ApplicationManager::mCmdTable[i].desc);
}

void ApplicationManager::SignalHandler(int signal)
{
    if(sApplicationManager) {
        if(signal == SIGCHLD) {
            int pid;
            int status;
            printf("SIGCHLD caught\n") ;
            pid = waitpid(-1, &status, WUNTRACED | WCONTINUED);

            printf("signal's wait() caught pid %d\n", pid) ;
            if(sApplicationManager->mApplication && pid == sApplicationManager->mApplication->pid()) {
                if(sApplicationManager->mTileManager)
                    sApplicationManager->setTilesVisible(true);
                else
                    sApplicationManager->mRunning = false;
                sApplicationManager->mApplication->handleSignal(status);
            }
        } else if(signal == SIGINT || signal == SIGTERM) {
            sApplicationManager->mRunning = false;
        }
    }
    ::signal(signal, ApplicationManager::SignalHandler);
}

int ApplicationManager::exec(int argc, char *argv[])
{
    if(argc == 1) {
        printf("Usage: am <netflix application and arguments>\n");
        printf("   ex: am ./netflix -U \"https://tenfootui.netflix.com/htmltvui\"\n");
        return -1;
    }

    assert(!mApplication);
    assert(!mTiles);

    mRunning = true;
    mTileManager = false;
    mControllerFd  = socket(AF_UNIX, SOCK_STREAM, 0);
    mApplicationFd = socket(AF_UNIX, SOCK_STREAM, 0);

    int arg_offset = 1;
    while(true) { //arguments for me
        if(!strcmp(argv[arg_offset], "--tiles")) {
            mTileManager = true;
            ++arg_offset;
        } else {
            break;
        }
    }
    mArgs.push_back(argv[arg_offset++]); //executable
    mArgs.push_back("--watchdog");
    while(arg_offset < argc) //rest
        mArgs.push_back(argv[arg_offset++]);

    if((mControllerFd == -1) || (mApplicationFd == -1)) {
        perror("socket error");
        exit(-1);
    }

    memset(&mControllerAddr, 0, sizeof(mControllerAddr));
    memset(&mApplicationAddr, 0, sizeof(mApplicationAddr));
    mControllerAddr.sun_family = AF_UNIX;
    mApplicationAddr.sun_family = AF_UNIX;
    strncpy(mControllerAddr.sun_path, ControllerSocketPath(), sizeof(mControllerAddr.sun_path)-1);
    strncpy(mApplicationAddr.sun_path, ApplicationSocketPath(), sizeof(mApplicationAddr.sun_path)-1);

    unlink(ControllerSocketPath());
    unlink(ApplicationSocketPath());

    int rc1 = bind(mControllerFd, (struct sockaddr*)&mControllerAddr, sizeof(mControllerAddr));
    int rc2 = bind(mApplicationFd, (struct sockaddr*)&mApplicationAddr, sizeof(mApplicationAddr));

    if((rc1 == -1) || (rc2 == -1)) {
        perror("bind error");
        exit(-1);
    }

    rc1 = listen(mControllerFd, 1);     // Only one controller
    rc2 = listen(mApplicationFd, 1);    // Only one application

    if((rc1 == -1) || (rc2 == -1)) {
        perror("listen error");
        exit(-1);
    }

    fcntl(mControllerFd, F_SETFL, O_NONBLOCK);

    while(mRunning) {
        // The application should always be running if resources are available
        // unless the user exits, there is a DIAL stop or the user via some
        // external UI kills the application.
        if(!mApplication || (!mApplication->isRunning() && mApplication->shouldRestart())) {
            if(!mApplication)
                mApplication.reset(new Application);
            printf("Launching Application.\n");
            mApplication->start(mArgs);
            if(!mApplication->isRunning())
                return Error_General;
            printf("  PID is '%d'.\n", mApplication->pid());
            if(mApplication->state() == Application::SUSPENDED || mApplication->state() == Application::BACKGROUNDED)
                setTilesVisible(true);
        }

        fd_set set;
        struct timeval timeout;
        FD_ZERO(&set);
        const int applicationFD = (mApplication && mApplication->isRunning()) ? mApplication->fd() : -1;
        if(applicationFD != -1)
            FD_SET(applicationFD, &set);
        timeout.tv_sec = 0;
        timeout.tv_usec = AppRequestTimeoutInUsec;

        // Is the application requesting to be suspended or backgrounded?
        if(select(FD_SETSIZE, &set, NULL, NULL, &timeout) == 1) {
            if(FD_ISSET(applicationFD, &set)) {
                unsigned char cmd;
                NRDP_UNUSED_RETURN(read(applicationFD, &cmd, sizeof(cmd)));
                mApplication->handleCommand((AMCommand)cmd);
            }
        }

        const int controllerSessionFd = accept(mControllerFd, NULL, NULL);
        if(controllerSessionFd != -1) {
            unsigned char cmd;
            NRDP_UNUSED_RETURN(read(controllerSessionFd, &cmd, sizeof(cmd)));
            const char rc = handleCommand((ApplicationManager::ControllerCommand)cmd);
            NRDP_UNUSED_RETURN(write(controllerSessionFd, &rc, sizeof(rc)));
            close(controllerSessionFd);

            printf("Processed '%s', returning ", ApplicationManager::ControllerCommandToString((ApplicationManager::ControllerCommand)cmd));
            switch (rc) {
            case ApplicationManager::Error_General:
                printf("'Error_General'\n");
                break;
            case ApplicationManager::Error_Timeout:
                printf("'Error_Timeout'\n");
                break;
            case ApplicationManager::Error_State:
                printf("'Error_State[0x%x]'\n", mApplication->state());
                break;
            default:
                printf("%d\n", rc);
                break;
            }
            printf("  Controller disconnected\n");
            if(cmd == ApplicationManager::CMD_CNT_EXIT)
                return 0;
        }
    }

    return 0;
}

int ApplicationManager::attachApplication() const
{
    const int fd = accept(mApplicationFd, NULL, NULL);
    if(fd == -1)
        printf("accept error\n");
    return fd;
}

ApplicationManager::Application::Application() : mFD(-1), mPid(0), mCrashed(false), mState(FOREGROUNDED), mSupportBackground(!getenv("NF_ONLY_SUSPEND"))
{
    if(mSupportBackground) { // If the device supports background mode and suspend mode use this table
        printf("NF_ONLY_SUSPEND is NOT set.  Background state is supported by the application manager.\n");
        // If the device only supports suspend mode (and not background mode) use this table
        CommandEntry ct[] = {
            { CMD_CNT_SUSPEND,              BACKGROUNDED | FOREGROUNDED,                        SUSPENDED,      &Application::doSuspend           },
            { CMD_CNT_BACKGROUND,           FOREGROUNDED,                                       BACKGROUNDED,   &Application::doBackground        },
            { CMD_CNT_RESUME,               SUSPENDED | BACKGROUNDED,                           FOREGROUNDED,   &Application::doResume            },
            { CMD_CNT_RESUME_LINK,          SUSPENDED | BACKGROUNDED,                           FOREGROUNDED,   &Application::doResumeLink        },
            { CMD_CNT_STOP_LOW_RESOURCES,   SUSPENDED | BACKGROUNDED,                           SHUTDOWN,       &Application::doStopLowResources  },
            { CMD_CNT_STOP_USER_KILL,       SUSPENDED | BACKGROUNDED | FOREGROUNDED,            SHUTDOWN,       &Application::doStopUserKill      },
            { CMD_CNT_STOP_DIAL,            SUSPENDED | BACKGROUNDED | FOREGROUNDED,            SHUTDOWN,       &Application::doStopDial          },
            { CMD_CNT_EXIT,                 SUSPENDED | BACKGROUNDED | FOREGROUNDED | SHUTDOWN, SHUTDOWN,       &Application::doExit              },  // Special, kills AM too
            { CMD_CNT_SIGKILL,              SUSPENDED | BACKGROUNDED | FOREGROUNDED,            SHUTDOWN,       &Application::doSigKill           },  // Special, simulate crash
            { CMD_CNT_POWER_SLEEP,          SUSPENDED | BACKGROUNDED | FOREGROUNDED,            NO_CHANGE,      &Application::doSleep             },
            { CMD_CNT_POWER_WAKE,           SUSPENDED | BACKGROUNDED | FOREGROUNDED,            NO_CHANGE,      &Application::doWake              },
            { CMD_CNT_KEY_L,                FOREGROUNDED,                                       NO_CHANGE,      &Application::doKeyLeft           },
            { CMD_CNT_KEY_R,                FOREGROUNDED,                                       NO_CHANGE,      &Application::doKeyRight          },
            { CMD_CNT_KEY_U,                FOREGROUNDED,                                       NO_CHANGE,      &Application::doKeyUp             },
            { CMD_CNT_KEY_D,                FOREGROUNDED,                                       NO_CHANGE,      &Application::doKeyDown           },
            { CMD_CNT_KEY_E,                FOREGROUNDED,                                       NO_CHANGE,      &Application::doKeyEnter          }
        };
        for (unsigned int i = 0; i < sizeof(ct)/sizeof(ct[0]); i++)
            mCommandTable.push_back(ct[i]);
    } else {
        printf("NF_ONLY_SUSPEND is set.  Background state is not supported by the application manager.\n");
        CommandEntry ct[] = {
            { CMD_CNT_SUSPEND,              FOREGROUNDED,                           SUSPENDED,      &Application::doSuspend           },
            { CMD_CNT_RESUME,               SUSPENDED,                              FOREGROUNDED,   &Application::doResume            },
            { CMD_CNT_RESUME_LINK,          SUSPENDED,                              FOREGROUNDED,   &Application::doResumeLink        },
            { CMD_CNT_STOP_LOW_RESOURCES,   SUSPENDED,                              SHUTDOWN,       &Application::doStopLowResources  },
            { CMD_CNT_STOP_USER_KILL,       SUSPENDED | FOREGROUNDED,               SHUTDOWN,       &Application::doStopUserKill      },
            { CMD_CNT_STOP_DIAL,            SUSPENDED | FOREGROUNDED,               SHUTDOWN,       &Application::doStopDial          },
            { CMD_CNT_EXIT,                 SUSPENDED | FOREGROUNDED | SHUTDOWN,    SHUTDOWN,       &Application::doExit              },  // Special, kills AM too
            { CMD_CNT_SIGKILL,              SUSPENDED | FOREGROUNDED,               SHUTDOWN,       &Application::doSigKill           },  // Special, simulate crash
            { CMD_CNT_POWER_SLEEP,          SUSPENDED | FOREGROUNDED,               NO_CHANGE,      &Application::doSleep             },
            { CMD_CNT_POWER_WAKE,           SUSPENDED | FOREGROUNDED,               NO_CHANGE,      &Application::doWake              },
            { CMD_CNT_KEY_L,                FOREGROUNDED,                           NO_CHANGE,      &Application::doKeyLeft           },
            { CMD_CNT_KEY_R,                FOREGROUNDED,                           NO_CHANGE,      &Application::doKeyRight          },
            { CMD_CNT_KEY_U,                FOREGROUNDED,                           NO_CHANGE,      &Application::doKeyUp             },
            { CMD_CNT_KEY_D,                FOREGROUNDED,                           NO_CHANGE,      &Application::doKeyDown           },
            { CMD_CNT_KEY_E,                FOREGROUNDED,                           NO_CHANGE,      &Application::doKeyEnter          }
        };
        for (unsigned int i = 0; i < sizeof(ct)/sizeof(ct[0]); i++)
            mCommandTable.push_back(ct[i]);
    }
}

void ApplicationManager::Application::start(const std::vector<const char *> &args)
{
    assert(sApplicationManager);
    mRestart = mCrashed = false;

    mInitialState = FOREGROUNDED;
    for(std::vector<const char*>::const_iterator it = args.begin(); it != args.end(); ++it) {
        if(!strcmp((*it), "--suspend")) {
            mInitialState = SUSPENDED;
        } else if(!strcmp((*it), "--suspend=")) {
            if(!strcmp((*it)+10, "background"))
                mInitialState = BACKGROUNDED;
            else
                mInitialState = SUSPENDED;
        } else if(!strcmp((*it), "--no-suspend")) {
            mInitialState = FOREGROUNDED;
        }
    }
    mState = mInitialState;
    printf("StartState(%d): 0x%x\n", __LINE__, mState);

    mPid = fork();
    if(!mPid) {
        execv(args[0], (char*const*)args.data());
        printf("  Launching failed.\n");
        exit(1);
    }
    mFD = sApplicationManager->attachApplication();
}

bool ApplicationManager::Application::stop()
{
    if(mPid > 0) {
        bool exited = false;
        int status;
        signal(SIGCHLD, SIG_DFL);
        ::kill(mPid, SIGTERM);
        for (int i = 0; i < TimeoutInSec*1000; i++) {
            if(waitpid(mPid, &status, WNOHANG) == mPid) {
                if(WIFEXITED(status)) {
                    printf("application exited normally with rc of %d\n", WEXITSTATUS(status));
                    exited = true;
                    break;
                }
                if(WIFSIGNALED(status)) {
                    printf("application exited due to signal %d\n", WTERMSIG(status));
                    exited = true;
                    break;
                }
            }
            usleep(1000000/1000);
        }
        if(!exited) {
            ::kill(mPid, SIGKILL);
            wait(NULL);
            return false;
        }
        mPid = 0;
        signal(SIGCHLD, ApplicationManager::SignalHandler);
    }
    if(mFD != -1) {
        close(mFD);
        mFD = -1;
    }
    return true;
}

bool ApplicationManager::Application::kill()
{
    if(mPid > 0) {
        const bool result = !::kill(mPid, SIGTERM);
        mPid = 0;
        return result;
    }
    return false;
}

char ApplicationManager::Application::send(ClientCommand cmd)
{
    if(mFD == -1)
        return Error_State;

    char rc = 0;
    fd_set set;
    struct timeval timeout;

    FD_ZERO(&set);
    FD_SET(mFD, &set);
    timeout.tv_sec = TimeoutInSec;
    timeout.tv_usec = 0;

    if(write(mFD, &cmd, 1) != 1)
        return Error_General;

    if(select(FD_SETSIZE, &set, NULL, NULL, &timeout) == 1) {
        NRDP_UNUSED_RETURN(read(mFD, &rc, 1));
    } else {
        printf("Application timed out and was killed\n");
        stop();
        rc = Error_Timeout;
    }
    return rc;
}

void ApplicationManager::Application::handleSignal(int status)
{
    if(WIFSTOPPED(status)) {
        printf("application stopped by pid %d\n", WSTOPSIG(status));
    } else if(WIFCONTINUED(status)) {
        printf("application continued\n");
    } else {
        mPid = 0;
        mCrashed = true;
        mState = SHUTDOWN;
        printf("SwitchedState(%d): 0x%x\n", __LINE__, mState);
        if(WIFEXITED(status)) {
            printf("application exited with rc of %d\n", WEXITSTATUS(status));
            if(WEXITSTATUS(status) == 0)
                mCrashed = false;
        }
        if(WIFSIGNALED(status))
            printf("application exited due to signal %d\n", WTERMSIG(status));
    }
}

int ApplicationManager::Application::handleCommand(ControllerCommand command)
{
    for(std::list<CommandEntry>::const_iterator it = mCommandTable.begin(); it != mCommandTable.end(); it++) {
        if(it->command == command) {
            assert(mState & it->validStates);
            if(int rc = (this->*(it->action))())
                return rc;
            if(!isRunning()) // Signal handler will set mState to SHUTDOWN
                return 0;
            if(it->newState != NO_CHANGE) {
                mState = it->newState;
                printf("SwitchedState(%d): 0x%x\n", __LINE__, mState);
            }
            return 0;
        }
    }
    return Error_General;
 }

ApplicationManager::Application::State ApplicationManager::Application::stateForCommand(ControllerCommand command) const
{
    for(std::list<CommandEntry>::const_iterator it = mCommandTable.begin(); it != mCommandTable.end(); it++) {
        if(it->command == command) {
            if(mState & it->validStates)
                return it->newState;
            break;
        }
    }
    return Application::INVALID;
}

bool ApplicationManager::Application::handleCommand(AMCommand command)
{
    switch(command) {
    case ApplicationManager::CMD_AM_STOP:
        break;
    case ApplicationManager::CMD_AM_REQUEST_SUSPEND:
        send(CMD_SUSPEND_APPLICATION_REQUEST);
        if(mState != SUSPENDED) {
            mState = SUSPENDED;
            printf("SwitchedState(%d): 0x%x\n", __LINE__, mState);
            if(sApplicationManager)
                sApplicationManager->handleStateChange(this, mState);
        }
        break;
    case ApplicationManager::CMD_AM_REQUEST_BACKGROUND: {
        send(CMD_BACKGROUND_APPLICATION_REQUEST);
        const State state = mSupportBackground ? BACKGROUNDED : SUSPENDED;
        if(mState != state) {
            mState = state;
            printf("SwitchedState(%d): 0x%x\n", __LINE__, mState);
            if(sApplicationManager)
                sApplicationManager->handleStateChange(this, mState);
        }
        break; }
    case ApplicationManager::CMD_AM_STARTED:
        mState = mInitialState;
        printf("SwitchedState(%d): 0x%x\n", __LINE__, mState);
        if(sApplicationManager)
            sApplicationManager->handleStateChange(this, mState);
        break;
    case ApplicationManager::CMD_AM_SUSPENDED:
        if(mState == FOREGROUNDED) {
            printf("Application switched to suspended state!\n");
            mState = SUSPENDED;
            printf("SwitchedState(%d): 0x%x\n", __LINE__, mState);
            if(sApplicationManager)
                sApplicationManager->handleStateChange(this, mState);
        }
        break;
    }
    return true;
}


