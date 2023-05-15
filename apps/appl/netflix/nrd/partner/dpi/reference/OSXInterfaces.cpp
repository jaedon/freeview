/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "FileSystem.h"

#include <sys/ioctl.h>
#include <sys/sysctl.h>
#include <net/route.h>
#include <net/if.h>
#include <net/if_dl.h>
#include <net/if_media.h>
#include <ifaddrs.h>
#include <netdb.h>

static int getSockLen(struct sockaddr *sa)
{
    switch (sa->sa_family) {
    case AF_INET:
        return sizeof(struct sockaddr_in);
    case AF_INET6:
        return sizeof(struct sockaddr_in6);
    case AF_LINK:
        return sizeof(struct sockaddr_dl);
    default:
        return -1;
    }
}

static int getNumericAddress(struct sockaddr *sa, char *outbuf, size_t buflen) {
    socklen_t socklen;
    int len;
    if ((len = getSockLen(sa)) < 0) {
        return -1;
    }
    socklen = len;
    if (getnameinfo(sa, len, outbuf, buflen, NULL, 0, NI_NUMERICHOST)) {
        return -1;
    }
    return 0;
}

using namespace netflix::device;
using namespace netflix;

std::vector< struct ISystem::NetworkInterface > FileSystem::getNetworkInterfaces()
{
    std::vector< struct ISystem::NetworkInterface > listnf;

    char def_if_name[IF_NAMESIZE];
    def_if_name[0] = 0;

#define NEXTADDR(w, u)                                              \
    if (rtm_addrs & (w)) {                                          \
        l = sizeof(struct sockaddr); memmove(cp, &(u), l); cp += l; \
    }

#define rtm m_rtmsg.m_rtm

    struct {
        struct rt_msghdr m_rtm;
        char       m_space[512];
    } m_rtmsg;

    int s, seq, l, rtm_addrs;
    pid_t pid;
    struct sockaddr so_dst, so_mask;
    char *cp = m_rtmsg.m_space;

    pid = getpid();
    seq = 0;
    rtm_addrs = RTA_DST | RTA_NETMASK;

    memset(&so_dst, 0, sizeof(so_dst));
    memset(&so_mask, 0, sizeof(so_mask));
    memset(&rtm, 0, sizeof(struct rt_msghdr));

    rtm.rtm_type = RTM_GET;
    rtm.rtm_flags = RTF_UP | RTF_GATEWAY;
    rtm.rtm_version = RTM_VERSION;
    rtm.rtm_seq = ++seq;
    rtm.rtm_addrs = rtm_addrs;

    so_dst.sa_family = AF_INET;
    so_mask.sa_family = AF_INET;

    NEXTADDR(RTA_DST, so_dst);
    NEXTADDR(RTA_NETMASK, so_mask);

    rtm.rtm_msglen = l = cp - (char *)&m_rtmsg;

    s = socket(PF_ROUTE, SOCK_RAW, 0);

    if (write(s, (char *)&m_rtmsg, l) < 0) {
        close(s);
    } else {
        do {
            l = read(s, (char *)&m_rtmsg, sizeof(m_rtmsg) * 10);
        } while (l > 0 && (rtm.rtm_seq != seq || rtm.rtm_pid != pid));

        close(s);

        if_indextoname(rtm.rtm_index, def_if_name);
    }

    ifaddrs *pifaddrs, *pifaddr;
    char addrBuf[NI_MAXHOST];
    int fd;

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd >= 0) {

        if(0 == getifaddrs(&pifaddrs)) {
            pifaddr = pifaddrs;
            while(pifaddr) {

                std::string name = pifaddr->ifa_name;

                std::vector<struct ISystem::NetworkInterface>::iterator it;
                bool found = false;
                for (it = listnf.begin(); it != listnf.end(); ++it) {
                    if ((*it).name == name) {
                        found = true;
                        break;
                    }
                }

                if(!found) {
                    struct ISystem::NetworkInterface nf;
                    nf.name = name;
                    if (nf.name == def_if_name)
                        nf.isDefault = true;
                    listnf.push_back(nf);
                    it = listnf.end() - 1;
                }

                sockaddr *paddr = pifaddr->ifa_addr;

                if(paddr) {
                    if (paddr->sa_family == AF_LINK) {
                        if(0 == getNumericAddress(paddr, addrBuf, sizeof(addrBuf))) {
                            it->macAddress = addrBuf;
                        }

                        if(pifaddr->ifa_flags & IFF_UP) {
                            it->linkConnected = CONN_STATE_CONNECTED;
                        }
                    } else if (paddr->sa_family == AF_INET) {
                        if(0 == getNumericAddress(paddr, addrBuf, sizeof(addrBuf))) {
                            it->ipAddress = addrBuf;
                        }

                        if(pifaddr->ifa_flags & IFF_UP) {
                            it->internetConnected = CONN_STATE_CONNECTED;
                        }
                    } else if (paddr->sa_family == AF_INET6) {
                        if(0 == getNumericAddress(paddr, addrBuf, sizeof(addrBuf))) {
                            it->ipv6Addresses.push_back(addrBuf);
                        }

                        if(pifaddr->ifa_flags & IFF_UP) {
                            it->internetConnected = CONN_STATE_CONNECTED;
                        }
                    }
                }

                ifmediareq mediareq;
                memset(&mediareq, 0, sizeof(ifmediareq));
                strncpy(mediareq.ifm_name, it->name.c_str(), IFNAMSIZ);
                if(ioctl(fd, SIOCGIFMEDIA, &mediareq) >= 0)
                {
                    if(IFM_TYPE(mediareq.ifm_active) == IFM_ETHER) {
                        it->physicalLayerType = PHYSLAYER_TYPE_WIRED;
                        switch(IFM_SUBTYPE(mediareq.ifm_active))
                        {
                        case IFM_10_T:
                            it->physicalLayerSubtype = PHYSLAYER_SUBTYPE_10MBPS_ETH;
                            break;
                        case IFM_100_TX:
                            it->physicalLayerSubtype = PHYSLAYER_SUBTYPE_100MBPS_ETH;
                            break;
                        case IFM_1000_T:
                            it->physicalLayerSubtype = PHYSLAYER_SUBTYPE_1000MBPS_ETH;
                            break;
                        default:
                            break;
                        }

                    } else if(IFM_TYPE(mediareq.ifm_active) == IFM_IEEE80211) {
                        it->physicalLayerType = PHYSLAYER_TYPE_WIFI;
                    }
                }

                pifaddr = pifaddr->ifa_next;
            }

            freeifaddrs(pifaddrs);
        }

        close(fd);
    }

    return listnf;
}
