#include <stdio.h>
#include <stdlib.h>

#include "ApplicationManager.h"

class ApplicationManagerController {
public:
    ApplicationManagerController() {};
    ~ApplicationManagerController() {};

    int exec(int argc, char *argv[]);

private:
    char SendCommandToApplicationManager(ApplicationManager::ControllerCommand cmd);
    int mSocketFd;
};

char ApplicationManagerController::SendCommandToApplicationManager(ApplicationManager::ControllerCommand cmd)
{
    char rc;
    send(mSocketFd, &cmd, 1, 0);
    recv(mSocketFd, &rc, 1, 0);
    return rc;
}

int ApplicationManagerController::exec(int argc, char *argv[])
{
    if (argc != 2) {
        printf("Usage: amcontroller <command>\n");
        printf("   ex: amcontroller start_foreground\n\n");
        printf("Supported commands are:\n");
        ApplicationManager::ListCommands();
        printf("\n");

        return -1;
    }

    char rc;
    ApplicationManager::ControllerCommand cmd = ApplicationManager::StringToControllerCommand(argv[1]);

    if (cmd != -1) {
        struct sockaddr_un  addr;

        if ((mSocketFd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
            printf("socket problem\n");
            return -1;
        }

        memset(&addr, 0, sizeof(addr));
        addr.sun_family = AF_UNIX;
        strncpy(addr.sun_path, ApplicationManager::ControllerSocketPath(), sizeof(addr.sun_path)-1);

        if (connect(mSocketFd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
            printf("Unable to connect to the application manager.\n");
            return -1;
        }

        rc = SendCommandToApplicationManager(cmd);
        printf("sent '%s', received ", ApplicationManager::ControllerCommandToString(cmd));
        switch (rc) {
        case ApplicationManager::Error_General:
            printf("'Error_General'\n");
            break;
        case ApplicationManager::Error_Timeout:
            printf("'Error_Timeout'\n");
            break;
        case ApplicationManager::Error_State:
            printf("'Error_State'\n");
            break;
        default:
            printf("%d\n", rc);
            break;
        }

        close(mSocketFd);
    } else {
        printf("Unknown command '%s'\n\n", argv[1]);
        printf("Supported commands are:\n");
        ApplicationManager::ListCommands();
        printf("\n");
        rc = -1;
    }

    return rc;
}

int main(int argc, char *argv[])
{
    ApplicationManagerController AMC;
    return AMC.exec(argc, argv);
}
