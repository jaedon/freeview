#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>

#include "ApplicationManager.h"

int main(int argc, char *argv[])
{
    ApplicationManager AM;

    signal(SIGPIPE, SIG_IGN);
    signal(SIGINT, ApplicationManager::SignalHandler);
    signal(SIGTERM, ApplicationManager::SignalHandler);
    signal(SIGCHLD, ApplicationManager::SignalHandler);
    return AM.exec(argc, argv);
}
