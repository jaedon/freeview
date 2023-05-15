# (c) 1997-2012 Netflix, Inc.  All content herein is protected by
# U.S. copyright and other applicable intellectual property laws and
# may not be copied without the express permission of Netflix, Inc.,
# which reserves all rights.  Reuse of any of this content for any
# purpose without the permission of Netflix, Inc. is strictly
# prohibited.

cmake_minimum_required(VERSION 2.8)

include_directories(${CMAKE_CURRENT_LIST_DIR}
                    ${CMAKE_CURRENT_LIST_DIR}/ixml/inc
                    ${CMAKE_CURRENT_LIST_DIR}/ixml/src/inc
                    ${CMAKE_CURRENT_LIST_DIR}/threadutil/inc
                    ${CMAKE_CURRENT_LIST_DIR}/upnp/src/inc
                    ${CMAKE_CURRENT_LIST_DIR}/upnp/inc)

add_definitions(-D_GNU_SOURCE)

# project(mdxlib_upnp)

set(NETFLIX_UPNP_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/upnp/inc/UpnpUniStd.h
    ${CMAKE_CURRENT_LIST_DIR}/upnp/inc/upnptools.h
    ${CMAKE_CURRENT_LIST_DIR}/upnp/inc/upnpconfig.h
    ${CMAKE_CURRENT_LIST_DIR}/upnp/inc/UpnpInet.h
    ${CMAKE_CURRENT_LIST_DIR}/upnp/inc/UpnpStdInt.h
    ${CMAKE_CURRENT_LIST_DIR}/upnp/inc/UpnpGlobal.h
    ${CMAKE_CURRENT_LIST_DIR}/upnp/inc/upnpdebug.h
    ${CMAKE_CURRENT_LIST_DIR}/upnp/inc/upnp.h
    ${CMAKE_CURRENT_LIST_DIR}/upnp/inc/UpnpString.h
    ${CMAKE_CURRENT_LIST_DIR}/upnp/inc/UpnpIntTypes.h
    ${CMAKE_CURRENT_LIST_DIR}/upnp/src/inc/upnp_timeout.h
    ${CMAKE_CURRENT_LIST_DIR}/upnp/src/inc/uuid.h
    ${CMAKE_CURRENT_LIST_DIR}/upnp/src/inc/strintmap.h
    ${CMAKE_CURRENT_LIST_DIR}/upnp/src/inc/sysdep.h
    ${CMAKE_CURRENT_LIST_DIR}/upnp/src/inc/server.h
    ${CMAKE_CURRENT_LIST_DIR}/upnp/src/inc/global.h
    ${CMAKE_CURRENT_LIST_DIR}/upnp/src/inc/httpparser.h
    ${CMAKE_CURRENT_LIST_DIR}/upnp/src/inc/membuffer.h
    ${CMAKE_CURRENT_LIST_DIR}/upnp/src/inc/statuscodes.h
    ${CMAKE_CURRENT_LIST_DIR}/upnp/src/inc/miniserver.h
    ${CMAKE_CURRENT_LIST_DIR}/upnp/src/inc/statcodes.h
    ${CMAKE_CURRENT_LIST_DIR}/upnp/src/inc/VirtualDir.h
    ${CMAKE_CURRENT_LIST_DIR}/upnp/src/inc/ssdplib.h
    ${CMAKE_CURRENT_LIST_DIR}/upnp/src/inc/client_table.h
    ${CMAKE_CURRENT_LIST_DIR}/upnp/src/inc/netall.h
    ${CMAKE_CURRENT_LIST_DIR}/upnp/src/inc/unixutil.h
    ${CMAKE_CURRENT_LIST_DIR}/upnp/src/inc/uri.h
    ${CMAKE_CURRENT_LIST_DIR}/upnp/src/inc/httpreadwrite.h
    ${CMAKE_CURRENT_LIST_DIR}/upnp/src/inc/upnp_nflx_config.h
    ${CMAKE_CURRENT_LIST_DIR}/upnp/src/inc/parsetools.h
    ${CMAKE_CURRENT_LIST_DIR}/upnp/src/inc/upnpapi.h
    ${CMAKE_CURRENT_LIST_DIR}/upnp/src/inc/upnputil.h
    ${CMAKE_CURRENT_LIST_DIR}/upnp/src/inc/sock.h
    ${CMAKE_CURRENT_LIST_DIR}/upnp/src/inc/gmtdate.h
    ${CMAKE_CURRENT_LIST_DIR}/upnp/src/inc/urlconfig.h
    ${CMAKE_CURRENT_LIST_DIR}/upnp/src/inc/md5.h
    ${CMAKE_CURRENT_LIST_DIR}/upnp/src/uuid/sysdep.c
    ${CMAKE_CURRENT_LIST_DIR}/upnp/src/uuid/md5.c
    ${CMAKE_CURRENT_LIST_DIR}/upnp/src/uuid/uuid.c
    ${CMAKE_CURRENT_LIST_DIR}/upnp/src/ssdp/ssdp_ResultData.h
    ${CMAKE_CURRENT_LIST_DIR}/upnp/src/ssdp/ssdp_device.c
    ${CMAKE_CURRENT_LIST_DIR}/upnp/src/ssdp/ssdp_server.c
    ${CMAKE_CURRENT_LIST_DIR}/upnp/src/ssdp/ssdp_ctrlpt.c
    ${CMAKE_CURRENT_LIST_DIR}/upnp/src/genlib/miniserver/miniserver.c
    ${CMAKE_CURRENT_LIST_DIR}/upnp/src/genlib/util/upnp_timeout.c
    ${CMAKE_CURRENT_LIST_DIR}/upnp/src/genlib/util/membuffer.c
    ${CMAKE_CURRENT_LIST_DIR}/upnp/src/genlib/util/strintmap.c
    ${CMAKE_CURRENT_LIST_DIR}/upnp/src/genlib/util/util.c
    ${CMAKE_CURRENT_LIST_DIR}/upnp/src/genlib/net/uri/uri.c
    ${CMAKE_CURRENT_LIST_DIR}/upnp/src/genlib/net/sock.c
    ${CMAKE_CURRENT_LIST_DIR}/upnp/src/genlib/net/http/statcodes.c
    ${CMAKE_CURRENT_LIST_DIR}/upnp/src/genlib/net/http/httpreadwrite.c
    ${CMAKE_CURRENT_LIST_DIR}/upnp/src/genlib/net/http/parsetools.c
    ${CMAKE_CURRENT_LIST_DIR}/upnp/src/genlib/net/http/httpparser.c
    ${CMAKE_CURRENT_LIST_DIR}/upnp/src/api/upnpapi.c
    ${CMAKE_CURRENT_LIST_DIR}/upnp/src/api/UpnpString.c
    ${CMAKE_CURRENT_LIST_DIR}/upnp/src/api/upnptools.c
    ${CMAKE_CURRENT_LIST_DIR}/threadutil/inc/FreeList.h
    ${CMAKE_CURRENT_LIST_DIR}/threadutil/inc/LinkedList.h
    ${CMAKE_CURRENT_LIST_DIR}/threadutil/inc/ithread.h
    ${CMAKE_CURRENT_LIST_DIR}/threadutil/inc/TimerThread.h
    ${CMAKE_CURRENT_LIST_DIR}/threadutil/inc/ThreadPool.h
    ${CMAKE_CURRENT_LIST_DIR}/threadutil/src/FreeList.c
    ${CMAKE_CURRENT_LIST_DIR}/threadutil/src/LinkedList.c
    ${CMAKE_CURRENT_LIST_DIR}/threadutil/src/ThreadPool.c
    ${CMAKE_CURRENT_LIST_DIR}/threadutil/src/TimerThread.c
    ${CMAKE_CURRENT_LIST_DIR}/autoconfig.h
    ${CMAKE_CURRENT_LIST_DIR}/ixml/inc/ixmldebug.h
    ${CMAKE_CURRENT_LIST_DIR}/ixml/inc/ixml.h
    ${CMAKE_CURRENT_LIST_DIR}/ixml/src/node.c
    ${CMAKE_CURRENT_LIST_DIR}/ixml/src/attr.c
    ${CMAKE_CURRENT_LIST_DIR}/ixml/src/namedNodeMap.c
    ${CMAKE_CURRENT_LIST_DIR}/ixml/src/nodeList.c
    ${CMAKE_CURRENT_LIST_DIR}/ixml/src/ixmlmembuf.c
    ${CMAKE_CURRENT_LIST_DIR}/ixml/src/inc/ixmlparser.h
    ${CMAKE_CURRENT_LIST_DIR}/ixml/src/inc/ixmlmembuf.h
    ${CMAKE_CURRENT_LIST_DIR}/ixml/src/ixml.c
    ${CMAKE_CURRENT_LIST_DIR}/ixml/src/element.c
    ${CMAKE_CURRENT_LIST_DIR}/ixml/src/document.c
    ${CMAKE_CURRENT_LIST_DIR}/ixml/src/ixmlparser.c)
