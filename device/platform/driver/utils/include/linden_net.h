
#ifndef __LINDEN_NET_H__
#define __LINDEN_NET_H__

#if CONFIG_TELNET_CMD
#define GetChar()	getchar_net()
#define PutChar(x)	putchar_net(x)
#else
#define GetChar()	VK_getchar()
#define PutChar(x)	VK_putchar(x)
#endif

#if CONFIG_TELNET_CMD
void setSocketFd(int fd);
char getchar_net(void);
void putchar_net(char buf);
#endif

#endif	/* ! __LINDEN_NET_H__ */
