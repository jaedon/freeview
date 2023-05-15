#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define DIRECTFB_HOTPLUG_SOCKET     "/org/kernel/udev/monitor"

#define MAX_LENGTH_OF_EVENT_STRING  (1024)

typedef enum {
     ERR_NONE,
     ERR_PARSE,
     ERR_SOCKET,
     ERR_BIND,
     ERR_CONNECT,
     ERR_UEVENT,
     ERR_SELECT,
     ERR_EVENT_FMT
} ErrCode;

int main(int argc, char *argv[])
{
     int                 socket_fd;
     int                 rt;
     struct sockaddr_un  sock_addr;
     char                *action;
     char                *devpath;
     char                udev_event[MAX_LENGTH_OF_EVENT_STRING];
     int                 send_len;

     action = getenv("ACTION");
     devpath = getenv("DEVPATH");

     socket_fd = socket(AF_UNIX, SOCK_DGRAM, 0);
     if (socket_fd == -1)
          return ERR_SOCKET;

     memset(&sock_addr, 0, sizeof(sock_addr));
     sock_addr.sun_family = AF_UNIX;
     strncpy(&sock_addr.sun_path[1], DIRECTFB_HOTPLUG_SOCKET, sizeof(sock_addr.sun_path) - 1);

     rt = connect(socket_fd, &sock_addr, sizeof(sock_addr.sun_family) + 1 + strlen(&sock_addr.sun_path[1]));
     if (rt < 0)
          return ERR_CONNECT;

     sprintf(udev_event, "%s@%s", action, devpath);
     send_len = send(socket_fd, udev_event, sizeof(udev_event), 0);
     if (send_len <= 0)
          return ERR_UEVENT;

     return ERR_NONE;
}
