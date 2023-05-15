/**************************************************************************
 *
 * Copyright (c) 2000-2003 Intel Corporation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 * - Neither name of Intel Corporation nor the names of its contributors
 * may be used to endorse or promote products derived from this software
 * without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL INTEL OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 **************************************************************************/

/*!
 * \addtogroup SSDPlib
 *
 * @{
 *
 * \file
 */

#ifndef WIN32
    #include <sys/param.h>
#endif /* WIN32 */

#include "upnp_nflx_config.h"

#if EXCLUDE_SSDP == 0

#include "ssdplib.h"

#include "httpparser.h"
#include "httpreadwrite.h"
#include "membuffer.h"
#include "miniserver.h"
#include "sock.h"
#include "ThreadPool.h"
#include "upnpapi.h"
#include "upnpdebug.h"

#include <stdio.h>


#define MAX_TIME_TOREAD  45

#ifdef INCLUDE_CLIENT_APIS
    SOCKET gSsdpReqSocket4 = INVALID_SOCKET;
#ifdef UPNP_ENABLE_IPV6
        SOCKET gSsdpReqSocket6 = INVALID_SOCKET;
    #endif /* UPNP_ENABLE_IPV6 */
#endif /* INCLUDE_CLIENT_APIS */

void RequestHandler();

enum Listener {
    Idle,
    Stopping,
    Running
};

struct SSDPSockArray {
    /*! socket for incoming advertisments and search requests */
    SOCKET ssdpSock;
#ifdef INCLUDE_CLIENT_APIS
    /*! socket for sending search requests and receiving search replies */
    int ssdpReqSock;
#endif /* INCLUDE_CLIENT_APIS */
};

#ifdef INCLUDE_DEVICE_APIS
static const char SERVICELIST_STR[] = "serviceList";

int AdvertiseAndReply(int AdFlag, UpnpDevice_Handle Hnd,
              enum SsdpSearchType SearchType,
              struct sockaddr *DestAddr, char *DeviceType,
              char *DeviceUDN, char *ServiceType, int Exp)
{
    int retVal = UPNP_E_SUCCESS;
    long unsigned int i;
    long unsigned int j;
    int defaultExp = DEFAULT_MAXAGE;
    struct Handle_Info *SInfo = NULL;
    char UDNstr[LINE_SIZE];
    char devType[LINE_SIZE];
    char servType[LINE_SIZE];
    IXML_NodeList *nodeList = NULL;
    IXML_NodeList *tmpNodeList = NULL;
    IXML_Node *tmpNode = NULL;
    IXML_Node *tmpNode2 = NULL;
    IXML_Node *textNode = NULL;
    const DOMString tmpStr;
    char SERVER[200];
    const DOMString dbgStr;
    int NumCopy = 0;
    int NumReplyAdvertCopy;
    Upnp_FunPtr callback = NULL;

    memset(UDNstr, 0, sizeof(UDNstr));
    memset(devType, 0, sizeof(devType));
    memset(servType, 0, sizeof(servType));

    UpnpPrintf(UPNP_ALL, API, __FILE__, __LINE__,
           "Inside AdvertiseAndReply with AdFlag = %d\n", AdFlag);

    /* Use a read lock */
    HandleReadLock();
    if (GetHandleInfo(Hnd, &SInfo) != HND_DEVICE) {
        retVal = UPNP_E_INVALID_HANDLE;
        goto end_function;
    }
    defaultExp = SInfo->MaxAge;
    NumReplyAdvertCopy = SInfo->NumReplyAdvertCopy;
    callback = SInfo->Callback;
    /* get server info */
    get_sdk_info(SERVER);
    /* parse the device list and send advertisements/replies */
    while (NumCopy == 0 || (AdFlag && NumCopy < NumReplyAdvertCopy)) {
        if (NumCopy != 0)
            imillisleep(SSDP_PAUSE);
        NumCopy++;
        for (i = 0;; i++) {
            UpnpPrintf(UPNP_ALL, API, __FILE__, __LINE__,
                   "Entering new device list with i = %lu\n\n",
                   i);
            tmpNode = ixmlNodeList_item(SInfo->DeviceList, i);
            if (!tmpNode) {
                UpnpPrintf(UPNP_ALL, API, __FILE__, __LINE__,
                       "Exiting new device list with i = %lu\n\n",
                       i);
                break;
            }
            dbgStr = ixmlNode_getNodeName(tmpNode);
            UpnpPrintf(UPNP_INFO, API, __FILE__, __LINE__,
                   "Extracting device type once for %s\n",
                   dbgStr);
            ixmlNodeList_free(nodeList);
            nodeList = ixmlElement_getElementsByTagName((IXML_Element *) tmpNode, "deviceType");
            if (!nodeList)
                continue;
            UpnpPrintf(UPNP_ALL, API, __FILE__, __LINE__,
                   "Extracting UDN for %s\n", dbgStr);
            dbgStr = ixmlNode_getNodeName(tmpNode);
            UpnpPrintf(UPNP_ALL, API, __FILE__, __LINE__,
                   "Extracting device type\n");
            tmpNode2 = ixmlNodeList_item(nodeList, 0);
            if (!tmpNode2)
                continue;
            textNode = ixmlNode_getFirstChild(tmpNode2);
            if (!textNode)
                continue;
            UpnpPrintf(UPNP_ALL, API, __FILE__, __LINE__,
                   "Extracting device type \n");
            tmpStr = ixmlNode_getNodeValue(textNode);
            if (!tmpStr)
                continue;
            strncpy(devType, tmpStr, sizeof(devType) - 1);
            UpnpPrintf(UPNP_ALL, API, __FILE__, __LINE__,
                   "Extracting device type = %s\n", devType);
            if (!tmpNode) {
                UpnpPrintf(UPNP_ALL, API, __FILE__, __LINE__,
                       "TempNode is NULL\n");
            }
            dbgStr = ixmlNode_getNodeName(tmpNode);
            UpnpPrintf(UPNP_ALL, API, __FILE__, __LINE__,
                   "Extracting UDN for %s\n", dbgStr);
            ixmlNodeList_free(nodeList);
            nodeList = ixmlElement_getElementsByTagName((IXML_Element *) tmpNode, "UDN");
            if (!nodeList) {
                UpnpPrintf(UPNP_CRITICAL, API, __FILE__,
                       __LINE__, "UDN not found!\n");
                continue;
            }
            tmpNode2 = ixmlNodeList_item(nodeList, 0);
            if (!tmpNode2) {
                UpnpPrintf(UPNP_CRITICAL, API, __FILE__,
                       __LINE__, "UDN not found!\n");
                continue;
            }
            textNode = ixmlNode_getFirstChild(tmpNode2);
            if (!textNode) {
                UpnpPrintf(UPNP_CRITICAL, API, __FILE__,
                       __LINE__, "UDN not found!\n");
                continue;
            }
            tmpStr = ixmlNode_getNodeValue(textNode);
            if (!tmpStr) {
                UpnpPrintf(UPNP_CRITICAL, API, __FILE__,
                       __LINE__, "UDN not found!\n");
                continue;
            }
            strncpy(UDNstr, tmpStr, sizeof(UDNstr) - 1);
            UpnpPrintf(UPNP_INFO, API, __FILE__, __LINE__,
                   "Sending UDNStr = %s \n", UDNstr);
            if (AdFlag) {
                /* send the device advertisement */
                if (AdFlag == 1) {
                    if(callback)
                    {
                        callback(UPNP_TRACE_EVENT, "Advertisement", NULL);
                    }
                    DeviceAdvertisement(devType, i == 0,
                                        UDNstr,
                                        SInfo->DescURL,
                                        SInfo->FriendlyName,
                                        &SInfo->DeviceReplyHeaderList,
                                        Exp,
                                        SInfo->DeviceAf);
                } else {
                    if(callback)
                    {
                        callback(UPNP_TRACE_EVENT, "Shutdown", NULL);
                    }
                    /* AdFlag == -1 */
                    DeviceShutdown(devType, i == 0,
                                   UDNstr,
                                   SERVER,
                                   SInfo->DescURL,
                                   SInfo->FriendlyName,
                                   Exp,
                                   SInfo->DeviceAf);
                }
            } else {
                switch (SearchType) {
                case SSDP_ALL:
                    if(callback)
                    {
                        callback(UPNP_TRACE_EVENT, "Reply:All", NULL);
                    }
                    DeviceReply(DestAddr, devType, i == 0,
                                UDNstr,
                                SInfo->DescURL,
                                SInfo->FriendlyName,
                                &SInfo->DeviceReplyHeaderList,
                                defaultExp);
                    break;
                case SSDP_ROOTDEVICE:
                    if (i == 0) {
                        if(callback)
                        {
                            callback(UPNP_TRACE_EVENT, "Reply:Root", NULL);
                        }
                        SendReply(DestAddr, devType, 1,
                                  UDNstr,
                                  SInfo->DescURL,
                                  SInfo->FriendlyName,
                                  &SInfo->DeviceReplyHeaderList,
                                  defaultExp, 0);
                    }
                    break;
                case SSDP_DEVICEUDN: {
                    if (DeviceUDN && strlen(DeviceUDN) != 0) {
                        if (strcasecmp(DeviceUDN, UDNstr)) {
                            UpnpPrintf(UPNP_INFO, API, __FILE__, __LINE__,
                                "DeviceUDN=%s and search UDN=%s DID NOT match\n",
                                UDNstr, DeviceUDN);
                            break;
                        } else {
                            UpnpPrintf(UPNP_INFO, API, __FILE__, __LINE__,
                                "DeviceUDN=%s and search UDN=%s MATCH\n",
                                UDNstr, DeviceUDN);
                            if(callback)
                            {
                                callback(UPNP_TRACE_EVENT, "Reply:UDN", NULL);
                            }
                            SendReply(DestAddr,
                                      devType,
                                      0,
                                      UDNstr,
                                      SInfo->DescURL,
                                      SInfo->FriendlyName,
                                      &SInfo->DeviceReplyHeaderList,
                                      defaultExp,
                                      0);
                            break;
                        }
                    }
                }
                case SSDP_DEVICETYPE: {
                    if (!strncasecmp(DeviceType, devType, strlen(DeviceType) - 2)) {
                        if (atoi(strrchr(DeviceType, ':') + 1)
                            < atoi(&devType[strlen(devType) - 1])) {
                            /* the requested version is lower than the device version
                             * must reply with the lower version number and the lower
                             * description URL */
                            UpnpPrintf(UPNP_INFO, API, __FILE__, __LINE__,
                                   "DeviceType=%s and search devType=%s MATCH\n",
                                   devType, DeviceType);
                            if(callback)
                            {
                                callback(UPNP_TRACE_EVENT, "Reply:DeviceType", NULL);
                            }
                            SendReply(DestAddr,
                                      DeviceType,
                                      0,
                                      UDNstr,
                                      SInfo->LowerDescURL,
                                      SInfo->FriendlyName,
                                      &SInfo->DeviceReplyHeaderList,
                                      defaultExp, 1);
                        } else if (atoi(strrchr(DeviceType, ':') + 1)
                               == atoi(&devType[strlen(devType) - 1])) {
                            UpnpPrintf(UPNP_INFO, API, __FILE__, __LINE__,
                                   "DeviceType=%s and search devType=%s MATCH\n",
                                   devType, DeviceType);
                            if(callback)
                            {
                                callback(UPNP_TRACE_EVENT, "Reply:DeviceType", NULL);
                            }
                            SendReply(DestAddr,
                                      DeviceType,
                                      0,
                                      UDNstr,
                                      SInfo->DescURL,
                                      SInfo->FriendlyName,
                                      &SInfo->DeviceReplyHeaderList,
                                      defaultExp, 1);
                        } else {
                            UpnpPrintf(UPNP_INFO, API, __FILE__, __LINE__,
                                   "DeviceType=%s and search devType=%s DID NOT MATCH\n",
                                   devType, DeviceType);
                        }
                    } else {
                        UpnpPrintf(UPNP_INFO, API, __FILE__, __LINE__,
                               "DeviceType=%s and search devType=%s DID NOT MATCH\n",
                               devType, DeviceType);
                    }
                    break;
                }
                default:
                    break;
                }
            }
            /* send service advertisements for services corresponding
             * to the same device */
            UpnpPrintf(UPNP_INFO, API, __FILE__, __LINE__,
                   "Sending service Advertisement\n");
            /* Correct service traversal such that each device's serviceList
             * is directly traversed as a child of its parent device. This
             * ensures that the service's alive message uses the UDN of
             * the parent device. */
            tmpNode = ixmlNode_getFirstChild(tmpNode);
            while (tmpNode) {
                dbgStr = ixmlNode_getNodeName(tmpNode);
                if (!strncmp
                    (dbgStr, SERVICELIST_STR,
                     sizeof SERVICELIST_STR)) {
                    break;
                }
                tmpNode = ixmlNode_getNextSibling(tmpNode);
            }
            ixmlNodeList_free(nodeList);
            if (!tmpNode) {
                nodeList = NULL;
                continue;
            }
            nodeList = ixmlElement_getElementsByTagName((IXML_Element *) tmpNode, "service");
            if (!nodeList) {
                UpnpPrintf(UPNP_INFO, API, __FILE__, __LINE__,
                       "Service not found 3\n");
                continue;
            }
            for (j = 0;; j++) {
                tmpNode = ixmlNodeList_item(nodeList, j);
                if (!tmpNode) {
                    break;
                }
                ixmlNodeList_free(tmpNodeList);
                tmpNodeList = ixmlElement_getElementsByTagName((IXML_Element *) tmpNode, "serviceType");
                if (!tmpNodeList) {
                    UpnpPrintf(UPNP_CRITICAL, API, __FILE__,
                           __LINE__,
                           "ServiceType not found \n");
                    continue;
                }
                tmpNode2 = ixmlNodeList_item(tmpNodeList, 0);
                if (!tmpNode2)
                    continue;
                textNode = ixmlNode_getFirstChild(tmpNode2);
                if (!textNode)
                    continue;
                /* servType is of format Servicetype:ServiceVersion */
                tmpStr = ixmlNode_getNodeValue(textNode);
                if (!tmpStr)
                    continue;
                strncpy(servType, tmpStr, sizeof(servType) - 1);
                UpnpPrintf(UPNP_INFO, API, __FILE__, __LINE__,
                       "ServiceType = %s\n", servType);
                if (AdFlag) {
                    if (AdFlag == 1) {
                        if(callback)
                        {
                            callback(UPNP_TRACE_EVENT, "Advertisement", NULL);
                        }
                        ServiceAdvertisement(UDNstr,
                                             servType,
                                             SInfo->DescURL,
                                             SInfo->FriendlyName,
                                             &SInfo->DeviceReplyHeaderList,
                                             Exp,
                                             SInfo->DeviceAf);
                    } else {
                        /* AdFlag == -1 */
                        if(callback)
                        {
                            callback(UPNP_TRACE_EVENT, "Shutdown", NULL);
                        }
                        ServiceShutdown(UDNstr,
                                        servType,
                                        SInfo->DescURL,
                                        SInfo->FriendlyName,
                                        Exp,
                                        SInfo->DeviceAf);
                    }
                } else {
                    switch (SearchType) {
                    case SSDP_ALL:
                        if(callback)
                        {
                            callback(UPNP_TRACE_EVENT, "Reply:All", NULL);
                        }
                        ServiceReply(DestAddr, servType,
                                     UDNstr,
                                     SInfo->DescURL,
                                     SInfo->FriendlyName,
                                     &SInfo->DeviceReplyHeaderList,
                                     defaultExp);
                        break;
                    case SSDP_SERVICE:
                        if (ServiceType) {
                            if (!strncasecmp(ServiceType, servType, strlen(ServiceType) - 2)) {
                                if (atoi(strrchr(ServiceType, ':') + 1) <
                                    atoi(&servType[strlen(servType) - 1])) {
                                    /* the requested version is lower than the service version
                                     * must reply with the lower version number and the lower
                                     * description URL */
                                    UpnpPrintf(UPNP_INFO, API, __FILE__, __LINE__,
                                           "ServiceType=%s and search servType=%s MATCH\n",
                                           ServiceType, servType);
                                    if(callback)
                                    {
                                        callback(UPNP_TRACE_EVENT, "Reply:Service", NULL);
                                    }
                                    SendReply(DestAddr,
                                              ServiceType,
                                              0,
                                              UDNstr,
                                              SInfo->LowerDescURL,
                                              SInfo->FriendlyName,
                                              &SInfo->DeviceReplyHeaderList,
                                              defaultExp, 1);
                                } else if (atoi(strrchr (ServiceType, ':') + 1) ==
                                       atoi(&servType[strlen(servType) - 1])) {
                                    UpnpPrintf(UPNP_INFO, API, __FILE__, __LINE__,
                                           "ServiceType=%s and search servType=%s MATCH\n",
                                           ServiceType, servType);
                                    if(callback)
                                    {
                                        callback(UPNP_TRACE_EVENT, "Reply:Service", NULL);
                                    }
                                    SendReply(DestAddr,
                                              ServiceType,
                                              0,
                                              UDNstr,
                                              SInfo->DescURL,
                                              SInfo->FriendlyName,
                                              &SInfo->DeviceReplyHeaderList,
                                              defaultExp, 1);
                                } else {
                                    UpnpPrintf(UPNP_INFO, API, __FILE__, __LINE__,
                                       "ServiceType=%s and search servType=%s DID NOT MATCH\n",
                                       ServiceType, servType);
                                }
                            } else {
                                UpnpPrintf(UPNP_INFO, API, __FILE__, __LINE__,
                                       "ServiceType=%s and search servType=%s DID NOT MATCH\n",
                                       ServiceType, servType);
                            }
                        }
                        break;
                    default:
                        break;
                    }
                }
            }
            ixmlNodeList_free(tmpNodeList);
            tmpNodeList = NULL;
            ixmlNodeList_free(nodeList);
            nodeList = NULL;
        }
    }

end_function:
    ixmlNodeList_free(tmpNodeList);
    ixmlNodeList_free(nodeList);
    UpnpPrintf(UPNP_ALL, API, __FILE__, __LINE__,
           "Exiting AdvertiseAndReply.\n");
    HandleUnlock();

    return retVal;
}
#endif /* INCLUDE_DEVICE_APIS */

int unique_service_name(char *cmd, SsdpEvent *Evt)
{
    char TempBuf[COMMAND_LEN];
    char *TempPtr = NULL;
    char *Ptr = NULL;
    char *ptr1 = NULL;
    char *ptr2 = NULL;
    char *ptr3 = NULL;
    int CommandFound = 0;
    size_t n = 0;

    if ((TempPtr = strstr(cmd, "uuid:schemas")) != NULL) {
        ptr1 = strstr(cmd, ":device");
        if (ptr1 != NULL)
            ptr2 = strstr(ptr1 + 1, ":");
        else
            return -1;
        if (ptr2 != NULL)
            ptr3 = strstr(ptr2 + 1, ":");
        else
            return -1;
        if (ptr3 != NULL) {
            if (strlen("uuid:") + strlen(ptr3 + 1) >= sizeof Evt->UDN)
                return -1;
            snprintf(Evt->UDN, sizeof Evt->UDN, "uuid:%s", ptr3 + 1);
        }
        else
            return -1;
        ptr1 = strstr(cmd, ":");
        if (ptr1 != NULL) {
            n = (size_t) (ptr3 - ptr1);
            n = n >= sizeof TempBuf ? sizeof TempBuf - 1 : n;
            strncpy(TempBuf, ptr1, n);
            TempBuf[n] = '\0';
            if (strlen("urn") + strlen(TempBuf) >= sizeof(Evt->DeviceType))
                return -1;

            snprintf(Evt->DeviceType, sizeof Evt->DeviceType, "urn%s", TempBuf);
        } else
            return -1;
        return 0;
    }
    if ((TempPtr = strstr(cmd, "uuid")) != NULL) {
        if ((Ptr = strstr(cmd, "::")) != NULL) {
            n = (size_t) (Ptr - TempPtr);
            n = n >= sizeof Evt->UDN ? sizeof Evt->UDN - 1 : n;
            strncpy(Evt->UDN, TempPtr, n);
            Evt->UDN[n] = '\0';
        } else {
            memset(Evt->UDN, 0, sizeof Evt->UDN);
            strncpy(Evt->UDN, TempPtr, sizeof Evt->UDN - 1);
        }
        CommandFound = 1;
    }
    if (strstr(cmd, "urn:") != NULL && strstr(cmd, ":service:") != NULL) {
        if ((TempPtr = strstr(cmd, "urn")) != NULL) {
            memset(Evt->ServiceType, 0, sizeof Evt->ServiceType);
            strncpy(Evt->ServiceType, TempPtr, sizeof Evt->ServiceType - 1);
            CommandFound = 1;
        }
    }
    if (strstr(cmd, "urn:") != NULL && strstr(cmd, ":device:") != NULL) {
        if ((TempPtr = strstr(cmd, "urn")) != NULL) {
            memset(Evt->DeviceType, 0, sizeof Evt->DeviceType);
            strncpy(Evt->DeviceType, TempPtr, sizeof Evt->DeviceType - 1);
            CommandFound = 1;
        }
    }
    if ((TempPtr = strstr(cmd, "::upnp:rootdevice")) != NULL) {
        /* Everything before "::upnp::rootdevice" is the UDN. */
        if (TempPtr != cmd) {
            n = (size_t) (TempPtr - cmd);
            n = n >= sizeof Evt->UDN ? sizeof Evt->UDN - 1 : n;
            strncpy(Evt->UDN, cmd, n);
            Evt->UDN[n] = 0;
            CommandFound = 1;
        }
    }
    if (CommandFound == 0)
        return -1;

    return 0;
}

enum SsdpSearchType ssdp_request_type1(char *cmd)
{
    if (strstr(cmd, ":all"))
        return SSDP_ALL;
    if (strstr(cmd, ":rootdevice"))
        return SSDP_ROOTDEVICE;
    if (strstr(cmd, "uuid:"))
        return SSDP_DEVICEUDN;
    if (strstr(cmd, "urn:") && strstr(cmd, ":device:"))
        return SSDP_DEVICETYPE;
    if (strstr(cmd, "urn:") && strstr(cmd, ":service:"))
        return SSDP_SERVICE;
    return SSDP_SERROR;
}

int ssdp_request_type(char *cmd, SsdpEvent *Evt)
{
    /* clear event */
    memset(Evt, 0, sizeof(SsdpEvent));
    unique_service_name(cmd, Evt);
    Evt->ErrCode = NO_ERROR_FOUND;
    if ((Evt->RequestType = ssdp_request_type1(cmd)) == SSDP_SERROR) {
        Evt->ErrCode = E_HTTP_SYNTEX;
        return -1;
    }
    return 0;
}

/*!
 * \brief Frees the ssdp request.
 */
static void free_ssdp_event_handler_data(
    /*! [in] ssdp_thread_data structure. This structure contains SSDP
     * request message. */
    void *the_data)
{
    ssdp_thread_data *data = (ssdp_thread_data *) the_data;

    if (data != NULL) {
        http_message_t *hmsg = &data->parser.msg;
        /* free data */
        httpmsg_destroy(hmsg);
        free(data);
    }
}

/*!
 * \brief Does some quick checking of the ssdp msg.
 *
 * \return TRUE if msg is valid, else FALSE.
 */
static UPNP_INLINE int valid_ssdp_msg(
    /*! [in] ssdp_thread_data structure. This structure contains SSDP
     * request message. */
    http_message_t * hmsg)
{
    memptr hdr_value;
    char ipv4hdrbuf[32];
    char ipv6hdrbuf[32];
    char ipv6hdrbuf2[32];
    char ipv6hdrbuf3[32];
    char ipv6hdrbuf4[32];

    /* check for valid methods - NOTIFY or M-SEARCH */
    if (hmsg->method != HTTPMETHOD_NOTIFY &&
        hmsg->method != HTTPMETHOD_MSEARCH &&
        hmsg->request_method != HTTPMETHOD_MSEARCH) {
        return FALSE;
    }
    if (hmsg->request_method != HTTPMETHOD_MSEARCH) {
        /* check PATH == "*" */
        if (hmsg->uri.type != RELATIVE ||
            strncmp("*", hmsg->uri.pathquery.buff,
                hmsg->uri.pathquery.size) != 0) {
            return FALSE;
        }
        /* check HOST header */
        memset(ipv4hdrbuf, 0, sizeof ipv4hdrbuf);
        memset(ipv6hdrbuf, 0, sizeof ipv6hdrbuf);
        memset(ipv6hdrbuf2, 0, sizeof ipv6hdrbuf2);
        memset(ipv6hdrbuf3, 0, sizeof ipv6hdrbuf3);
        memset(ipv6hdrbuf4, 0, sizeof ipv6hdrbuf4);

        snprintf(ipv4hdrbuf, sizeof ipv4hdrbuf - 1, "239.255.255.250:%d", gSSDP_PORT);
        snprintf(ipv6hdrbuf, sizeof ipv6hdrbuf - 1, "[FF02::C]:%d", gSSDP_PORT);
        snprintf(ipv6hdrbuf2, sizeof ipv6hdrbuf2 - 1, "[ff02::c]:%d", gSSDP_PORT);
        snprintf(ipv6hdrbuf3, sizeof ipv6hdrbuf3 - 1, "[FF05::C]:%d", gSSDP_PORT);
        snprintf(ipv6hdrbuf4, sizeof ipv6hdrbuf4 - 1, "[ff05::c]:%d", gSSDP_PORT);

        if (httpmsg_find_hdr(hmsg, HDR_HOST, &hdr_value) == NULL ||
            (memptr_cmp(&hdr_value, ipv4hdrbuf) != 0 &&
             memptr_cmp(&hdr_value, ipv6hdrbuf) != 0 &&
             memptr_cmp(&hdr_value, ipv6hdrbuf2) != 0 &&
             memptr_cmp(&hdr_value, ipv6hdrbuf3) != 0 &&
             memptr_cmp(&hdr_value, ipv6hdrbuf4) != 0)) {
            UpnpPrintf(UPNP_INFO, SSDP, __FILE__, __LINE__,
                   "Invalid HOST header from SSDP message\n");

            return FALSE;
        }
    }

    /* passed quick check */
    return TRUE;
}

/*!
 * \brief Parses the message and dispatches it to a handler which handles the
 * ssdp request msg.
 *
 * \return 0 if successful, -1 if error.
 */
static UPNP_INLINE int start_event_handler(
    /*! [in] ssdp_thread_data structure. This structure contains SSDP
     * request message. */
    void *Data)
{
    http_parser_t *parser = NULL;
    parse_status_t status;
    ssdp_thread_data *data = (ssdp_thread_data *) Data;

    parser = &data->parser;
    status = parser_parse(parser);
    if (status == PARSE_FAILURE) {
        if (parser->msg.method != HTTPMETHOD_NOTIFY ||
            !parser->valid_ssdp_notify_hack) {
            UpnpPrintf(UPNP_INFO, SSDP, __FILE__, __LINE__,
                   "SSDP recvd bad msg code = %d\n", status);
            /* ignore bad msg, or not enuf mem */
            goto error_handler;
        }
        /* valid notify msg */
    } else if (status != PARSE_SUCCESS) {
        UpnpPrintf(UPNP_INFO, SSDP, __FILE__, __LINE__,
               "SSDP recvd bad msg code = %d\n", status);

        goto error_handler;
    }
    /* check msg */
    if (valid_ssdp_msg(&parser->msg) != TRUE) {
        goto error_handler;
    }
    /* done; thread will free 'data' */
    return 0;

 error_handler:
    free_ssdp_event_handler_data(data);
    return -1;
}

/*!
 * \brief This function is a thread that handles SSDP requests.
 */
static void ssdp_event_handler_thread(
    /*! [] ssdp_thread_data structure. This structure contains SSDP
     * request message. */
    void *the_data)
{
    ssdp_thread_data *data = (ssdp_thread_data *) the_data;
    http_message_t *hmsg = &data->parser.msg;

    if (start_event_handler(the_data) != 0)
        return;
    /* send msg to device or ctrlpt */
    if (hmsg->method == HTTPMETHOD_NOTIFY ||
        hmsg->request_method == HTTPMETHOD_MSEARCH) {
#ifdef INCLUDE_CLIENT_APIS
        ssdp_handle_ctrlpt_msg(hmsg,
                       &data->dest_addr,
                       FALSE, NULL);
#endif /* INCLUDE_CLIENT_APIS */
    } else {
        ssdp_handle_device_request(hmsg,
                       &data->dest_addr);
    }

    /* free data */
    free_ssdp_event_handler_data(data);
}

void readFromSSDPSocket(SOCKET socket)
{
    char *requestBuf = NULL;
    char staticBuf[BUFSIZE];
    struct sockaddr_storage __ss;
    ThreadPoolJob job;
    ssdp_thread_data *data = NULL;
    socklen_t socklen = sizeof(__ss);
    ssize_t byteReceived = 0;
    char ntop_buf[64];

    memset(&job, 0, sizeof(job));

    requestBuf = staticBuf;
    /* in case memory can't be allocated, still drain the socket using a
     * static buffer. */
    data = malloc(sizeof(ssdp_thread_data));
    if (data) {
        /* initialize parser */
#ifdef INCLUDE_CLIENT_APIS
        if (socket == gSsdpReqSocket4
#ifdef UPNP_ENABLE_IPV6
            || socket == gSsdpReqSocket6
    #endif /* UPNP_ENABLE_IPV6 */
            )
            parser_response_init(&data->parser, HTTPMETHOD_MSEARCH);
        else
            parser_request_init(&data->parser);
#else /* INCLUDE_CLIENT_APIS */
        parser_request_init(&data->parser);
#endif /* INCLUDE_CLIENT_APIS */
        /* set size of parser buffer */
        if (membuffer_set_size(&data->parser.msg.msg, BUFSIZE) == 0)
            /* use this as the buffer for recv */
            requestBuf = data->parser.msg.msg.buf;
        else {
            free(data);
            data = NULL;
        }
    }
    byteReceived = recvfrom(socket, requestBuf, BUFSIZE - 1, 0,
                (struct sockaddr *)&__ss, &socklen);
    if (byteReceived > 0) {
        requestBuf[byteReceived] = '\0';
        if (__ss.ss_family == AF_INET)
            inet_ntop(AF_INET,
                  &((struct sockaddr_in *)&__ss)->sin_addr,
                  ntop_buf, sizeof(ntop_buf));
#ifdef UPNP_ENABLE_IPV6
        else if (__ss.ss_family == AF_INET6)
            inet_ntop(AF_INET6,
                  &((struct sockaddr_in6 *)&__ss)->sin6_addr,
                  ntop_buf, sizeof(ntop_buf));
#endif /* UPNP_ENABLE_IPV6 */
        else
        {
            memset(ntop_buf, 0, sizeof(ntop_buf));
            strncpy(ntop_buf, "<Invalid address family>",
                sizeof(ntop_buf) - 1);
        }
        UpnpPrintf(UPNP_INFO, SSDP, __FILE__, __LINE__,
               "Start of received response ----------------------------------------------------\n"
               "%s\n"
               "End of received response ------------------------------------------------------\n"
               "From host %s\n", requestBuf, ntop_buf);
        /* add thread pool job to handle request */
        if (data != NULL) {
            data->parser.msg.msg.length += (size_t) byteReceived;
            /* null-terminate */
            data->parser.msg.msg.buf[byteReceived] = 0;
            memcpy(&data->dest_addr, &__ss, sizeof(__ss));
            TPJobInit(&job, (start_routine)
                  ssdp_event_handler_thread, data);
            TPJobSetFreeFunction(&job,
                         free_ssdp_event_handler_data);
            TPJobSetPriority(&job, MED_PRIORITY);
            if (ThreadPoolAdd(&gRecvThreadPool, &job, NULL) != 0)
                free_ssdp_event_handler_data(data);
        }
    } else
        free_ssdp_event_handler_data(data);
}

/*!
 * \brief
 */
static int create_ssdp_sock_v4(
    /*! [] SSDP IPv4 socket to be created. */
    SOCKET *ssdpSock)
{
    char errorBuffer[ERROR_BUFFER_LEN];
    int onOff;
    unsigned char ttl = gSSDP_TTL;
    struct ip_mreq ssdpMcastAddr;
    struct sockaddr_storage __ss;
    struct sockaddr_in *ssdpAddr4 = (struct sockaddr_in *)&__ss;
    int ret = 0;
    struct in_addr addr;

    *ssdpSock = socket(AF_INET, SOCK_DGRAM, 0);
    if (*ssdpSock == -1) {
        (void)strerror_r(errno, errorBuffer, ERROR_BUFFER_LEN);
        UpnpPrintf(UPNP_CRITICAL, SSDP, __FILE__, __LINE__,
               "Error in socket(): %s\n", errorBuffer);

        return UPNP_E_OUTOF_SOCKET;
    }
    onOff = 1;
    ret = setsockopt(*ssdpSock, SOL_SOCKET, SO_REUSEADDR,
             (char *)&onOff, sizeof(onOff));
    if (ret == -1) {
        (void)strerror_r(errno, errorBuffer, ERROR_BUFFER_LEN);
        UpnpPrintf(UPNP_CRITICAL, SSDP, __FILE__, __LINE__,
               "Error in setsockopt() SO_REUSEADDR: %s\n",
               errorBuffer);
        shutdown(*ssdpSock, SD_BOTH);
        UpnpCloseSocket(*ssdpSock);

        return UPNP_E_SOCKET_ERROR;
    }
#if defined(BSD) || defined(__OSX__) || defined(__APPLE__)
    onOff = 1;
    ret = setsockopt(*ssdpSock, SOL_SOCKET, SO_REUSEPORT,
             (char *)&onOff, sizeof(onOff));
    if (ret == -1) {
        strerror_r(errno, errorBuffer, ERROR_BUFFER_LEN);
        UpnpPrintf(UPNP_CRITICAL, SSDP, __FILE__, __LINE__,
               "Error in setsockopt() SO_REUSEPORT: %s\n",
               errorBuffer);
        shutdown(*ssdpSock, SD_BOTH);
        UpnpCloseSocket(*ssdpSock);

        return UPNP_E_SOCKET_ERROR;
    }
#endif /* BSD, __OSX__, __APPLE__ */
    memset(&__ss, 0, sizeof(__ss));
    ssdpAddr4->sin_family = AF_INET;
    ssdpAddr4->sin_addr.s_addr = htonl(INADDR_ANY);
    ssdpAddr4->sin_port = htons(gSSDP_PORT);
    ret = bind(*ssdpSock, (struct sockaddr *)ssdpAddr4, sizeof(*ssdpAddr4));
    if (ret == -1) {
        (void)strerror_r(errno, errorBuffer, ERROR_BUFFER_LEN);
//		UpnpPrintf(UPNP_CRITICAL, SSDP, __FILE__, __LINE__,
        printf(
               "Error in bind(), addr=0x%08X, port=%d: %s\n",
               INADDR_ANY, gSSDP_PORT, errorBuffer);
        shutdown(*ssdpSock, SD_BOTH);
        UpnpCloseSocket(*ssdpSock);

        return UPNP_E_SOCKET_BIND;
    }
    memset((void *)&ssdpMcastAddr, 0, sizeof(struct ip_mreq));
    ssdpMcastAddr.imr_interface.s_addr = inet_addr(gIF_IPV4);
    ssdpMcastAddr.imr_multiaddr.s_addr = inet_addr(SSDP_IP);
    ret = setsockopt(*ssdpSock, IPPROTO_IP, IP_ADD_MEMBERSHIP,
             (char *)&ssdpMcastAddr, sizeof(struct ip_mreq));
    if (ret == -1) {
        (void)strerror_r(errno, errorBuffer, ERROR_BUFFER_LEN);
        UpnpPrintf(UPNP_CRITICAL, SSDP, __FILE__, __LINE__,
               "Error in setsockopt() IP_ADD_MEMBERSHIP (join multicast group): %s\n",
               errorBuffer);
        shutdown(*ssdpSock, SD_BOTH);
        UpnpCloseSocket(*ssdpSock);

        return UPNP_E_SOCKET_ERROR;
    }
    /* Set multicast interface. */
    memset((void *)&addr, 0, sizeof(struct in_addr));
    addr.s_addr = inet_addr(gIF_IPV4);
    ret = setsockopt(*ssdpSock, IPPROTO_IP, IP_MULTICAST_IF,
             (char *)&addr, sizeof addr);
    if (ret == -1) {
        (void)strerror_r(errno, errorBuffer, ERROR_BUFFER_LEN);
        UpnpPrintf(UPNP_INFO, SSDP, __FILE__, __LINE__,
               "Error in setsockopt() IP_MULTICAST_IF (set multicast interface): %s\n",
               errorBuffer);
        /* This is probably not a critical error, so let's continue. */
    }
    /* result is not checked becuase it will fail in WinMe and Win9x. */
    ret = setsockopt(*ssdpSock, IPPROTO_IP,
             IP_MULTICAST_TTL, &ttl, sizeof(ttl));
    onOff = 1;
    ret = setsockopt(*ssdpSock, SOL_SOCKET, SO_BROADCAST,
             (char *)&onOff, sizeof(onOff));
    if (ret == -1) {
        (void)strerror_r(errno, errorBuffer, ERROR_BUFFER_LEN);
        UpnpPrintf(UPNP_CRITICAL, SSDP, __FILE__, __LINE__,
               "Error in setsockopt() SO_BROADCAST (set broadcast): %s\n",
               errorBuffer);
        shutdown(*ssdpSock, SD_BOTH);
        UpnpCloseSocket(*ssdpSock);

        return UPNP_E_NETWORK_ERROR;
    }

    return UPNP_E_SUCCESS;
}

#ifdef INCLUDE_CLIENT_APIS
/*!
 * \brief Creates the SSDP IPv4 socket to be used by the control point.
 *
 * \return UPNP_E_SUCCESS on successful socket creation.
 */
static int create_ssdp_sock_reqv4(
    /*! [out] SSDP IPv4 request socket to be created. */
    SOCKET *ssdpReqSock)
{
    char errorBuffer[ERROR_BUFFER_LEN];
    unsigned char ttl = gSSDP_TTL;

    *ssdpReqSock = socket(AF_INET, SOCK_DGRAM, 0);
    if (*ssdpReqSock == -1) {
        (void)strerror_r(errno, errorBuffer, ERROR_BUFFER_LEN);
        UpnpPrintf(UPNP_CRITICAL, SSDP, __FILE__, __LINE__,
               "Error in socket(): %s\n", errorBuffer);
        return UPNP_E_OUTOF_SOCKET;
    }
    setsockopt(*ssdpReqSock, IPPROTO_IP, IP_MULTICAST_TTL,
           &ttl, sizeof(ttl));
    /* just do it, regardless if fails or not. */
    sock_make_no_blocking(*ssdpReqSock);

    return UPNP_E_SUCCESS;
}

#ifdef UPNP_ENABLE_IPV6
/*!
 * \brief This function ...
 */
static int create_ssdp_sock_v6(
    /* [] SSDP IPv6 socket to be created. */
    SOCKET *ssdpSock)
{
    char errorBuffer[ERROR_BUFFER_LEN];
    struct ipv6_mreq ssdpMcastAddr;
    struct sockaddr_storage __ss;
    struct sockaddr_in6 *ssdpAddr6 = (struct sockaddr_in6 *)&__ss;
    int onOff;
    int ret = 0;

    *ssdpSock = socket(AF_INET6, SOCK_DGRAM, 0);
    if (*ssdpSock == -1) {
        strerror_r(errno, errorBuffer, ERROR_BUFFER_LEN);
        UpnpPrintf(UPNP_CRITICAL, SSDP, __FILE__, __LINE__,
               "Error in socket(): %s\n", errorBuffer);

        return UPNP_E_OUTOF_SOCKET;
    }
    onOff = 1;
    ret = setsockopt(*ssdpSock, SOL_SOCKET, SO_REUSEADDR,
             (char *)&onOff, sizeof(onOff));
    if (ret == -1) {
        strerror_r(errno, errorBuffer, ERROR_BUFFER_LEN);
        UpnpPrintf(UPNP_CRITICAL, SSDP, __FILE__, __LINE__,
               "Error in setsockopt() SO_REUSEADDR: %s\n",
               errorBuffer);
        shutdown(*ssdpSock, SD_BOTH);
        UpnpCloseSocket(*ssdpSock);

        return UPNP_E_SOCKET_ERROR;
    }
#if defined(BSD) || defined(__OSX__) || defined(__APPLE__)
    onOff = 1;
    ret = setsockopt(*ssdpSock, SOL_SOCKET, SO_REUSEPORT,
             (char *)&onOff, sizeof(onOff));
    if (ret == -1) {
        strerror_r(errno, errorBuffer, ERROR_BUFFER_LEN);
        UpnpPrintf(UPNP_CRITICAL, SSDP, __FILE__, __LINE__,
               "Error in setsockopt() SO_REUSEPORT: %s\n",
               errorBuffer);
        shutdown(*ssdpSock, SD_BOTH);
        UpnpCloseSocket(*ssdpSock);

        return UPNP_E_SOCKET_ERROR;
    }
#endif /* BSD, __OSX__, __APPLE__ */
    memset(&__ss, 0, sizeof(__ss));
    ssdpAddr6->sin6_family = AF_INET6;
    ssdpAddr6->sin6_addr = in6addr_any;
    ssdpAddr6->sin6_scope_id = gIF_INDEX;
    ssdpAddr6->sin6_port = htons(gSSDP_PORT);
    ret = bind(*ssdpSock, (struct sockaddr *)ssdpAddr6, sizeof(*ssdpAddr6));
    if (ret == -1) {
        strerror_r(errno, errorBuffer, ERROR_BUFFER_LEN);
        UpnpPrintf(UPNP_CRITICAL, SSDP, __FILE__, __LINE__,
               "Error in bind(), addr=0x%032lX, port=%d: %s\n",
               0lu, gSSDP_PORT, errorBuffer);
        shutdown(*ssdpSock, SD_BOTH);
        UpnpCloseSocket(*ssdpSock);

        return UPNP_E_SOCKET_BIND;
    }
    memset((void *)&ssdpMcastAddr, 0, sizeof(ssdpMcastAddr));
    ssdpMcastAddr.ipv6mr_interface = gIF_INDEX;
    inet_pton(AF_INET6, SSDP_IPV6_LINKLOCAL,
          &ssdpMcastAddr.ipv6mr_multiaddr);
    ret = setsockopt(*ssdpSock, IPPROTO_IPV6, IPV6_JOIN_GROUP,
            (char *)&ssdpMcastAddr, sizeof(ssdpMcastAddr));
    if (ret == -1) {
        strerror_r(errno, errorBuffer, ERROR_BUFFER_LEN);
        UpnpPrintf(UPNP_CRITICAL, SSDP, __FILE__, __LINE__,
               "Error in setsockopt() IPV6_JOIN_GROUP (join multicast group): %s\n",
               errorBuffer);
        shutdown(*ssdpSock, SD_BOTH);
        UpnpCloseSocket(*ssdpSock);

        return UPNP_E_SOCKET_ERROR;
    }
    onOff = 1;
    ret = setsockopt(*ssdpSock, SOL_SOCKET, SO_BROADCAST,
             (char *)&onOff, sizeof(onOff));
    if (ret == -1) {
        strerror_r(errno, errorBuffer, ERROR_BUFFER_LEN);
        UpnpPrintf(UPNP_CRITICAL, SSDP, __FILE__, __LINE__,
               "Error in setsockopt() SO_BROADCAST (set broadcast): %s\n",
               errorBuffer);
        shutdown(*ssdpSock, SD_BOTH);
        UpnpCloseSocket(*ssdpSock);

        return UPNP_E_NETWORK_ERROR;
    }

    return UPNP_E_SUCCESS;
}
#endif /* IPv6 */

#ifdef UPNP_ENABLE_IPV6
/*!
 * \brief This function ...
 */
static int create_ssdp_sock_v6_ula_gua(
    /*! [] SSDP IPv6 socket to be created. */
    SOCKET * ssdpSock)
{
    char errorBuffer[ERROR_BUFFER_LEN];
    struct ipv6_mreq ssdpMcastAddr;
    struct sockaddr_storage __ss;
    struct sockaddr_in6 *ssdpAddr6 = (struct sockaddr_in6 *)&__ss;
    int onOff;
    int ret = 0;

    *ssdpSock = socket(AF_INET6, SOCK_DGRAM, 0);
    if (*ssdpSock == -1) {
        strerror_r(errno, errorBuffer, ERROR_BUFFER_LEN);
        UpnpPrintf(UPNP_CRITICAL, SSDP, __FILE__, __LINE__,
               "Error in socket(): %s\n", errorBuffer);

        return UPNP_E_OUTOF_SOCKET;
    }
    onOff = 1;
    ret = setsockopt(*ssdpSock, SOL_SOCKET, SO_REUSEADDR,
             (char *)&onOff, sizeof(onOff));
    if (ret == -1) {
        strerror_r(errno, errorBuffer, ERROR_BUFFER_LEN);
        UpnpPrintf(UPNP_CRITICAL, SSDP, __FILE__, __LINE__,
               "Error in setsockopt() SO_REUSEADDR: %s\n",
               errorBuffer);
        shutdown(*ssdpSock, SD_BOTH);
        UpnpCloseSocket(*ssdpSock);

        return UPNP_E_SOCKET_ERROR;
    }
#if defined(BSD) || defined(__OSX__) || defined(__APPLE__)
    onOff = 1;
    ret = setsockopt(*ssdpSock, SOL_SOCKET, SO_REUSEPORT,
             (char *)&onOff, sizeof(onOff));
    if (ret == -1) {
        strerror_r(errno, errorBuffer, ERROR_BUFFER_LEN);
        UpnpPrintf(UPNP_CRITICAL, SSDP, __FILE__, __LINE__,
               "Error in setsockopt() SO_REUSEPORT: %s\n",
               errorBuffer);
        shutdown(*ssdpSock, SD_BOTH);
        UpnpCloseSocket(*ssdpSock);

        return UPNP_E_SOCKET_ERROR;
    }
#endif /* BSD, __OSX__, __APPLE__ */
    memset(&__ss, 0, sizeof(__ss));
    ssdpAddr6->sin6_family = AF_INET6;
    ssdpAddr6->sin6_addr = in6addr_any;
    ssdpAddr6->sin6_scope_id = gIF_INDEX;
    ssdpAddr6->sin6_port = htons(gSSDP_PORT);
    ret = bind(*ssdpSock, (struct sockaddr *)ssdpAddr6, sizeof(*ssdpAddr6));
    if (ret == -1) {
        strerror_r(errno, errorBuffer, ERROR_BUFFER_LEN);
        UpnpPrintf(UPNP_CRITICAL, SSDP, __FILE__, __LINE__,
               "Error in bind(), addr=0x%032lX, port=%d: %s\n",
               0lu, gSSDP_PORT, errorBuffer);
        shutdown(*ssdpSock, SD_BOTH);
        UpnpCloseSocket(*ssdpSock);

        return UPNP_E_SOCKET_BIND;
    }
    memset((void *)&ssdpMcastAddr, 0, sizeof(ssdpMcastAddr));
    ssdpMcastAddr.ipv6mr_interface = gIF_INDEX;
    /* SITE LOCAL */
    inet_pton(AF_INET6, SSDP_IPV6_SITELOCAL,
          &ssdpMcastAddr.ipv6mr_multiaddr);
    ret = setsockopt(*ssdpSock, IPPROTO_IPV6, IPV6_JOIN_GROUP,
            (char *)&ssdpMcastAddr, sizeof(ssdpMcastAddr));
    if (ret == -1) {
        strerror_r(errno, errorBuffer, ERROR_BUFFER_LEN);
        UpnpPrintf(UPNP_CRITICAL, SSDP, __FILE__, __LINE__,
               "Error in setsockopt() IPV6_JOIN_GROUP (join multicast group): %s\n",
               errorBuffer);
        shutdown(*ssdpSock, SD_BOTH);
        UpnpCloseSocket(*ssdpSock);

        return UPNP_E_SOCKET_ERROR;
    }
    onOff = 1;
    ret = setsockopt(*ssdpSock, SOL_SOCKET, SO_BROADCAST,
             (char *)&onOff, sizeof(onOff));
    if (ret == -1) {
        strerror_r(errno, errorBuffer, ERROR_BUFFER_LEN);
        UpnpPrintf(UPNP_CRITICAL, SSDP, __FILE__, __LINE__,
               "Error in setsockopt() SO_BROADCAST (set broadcast): %s\n",
               errorBuffer);
        shutdown(*ssdpSock, SD_BOTH);
        UpnpCloseSocket(*ssdpSock);

        return UPNP_E_NETWORK_ERROR;
    }

    return UPNP_E_SUCCESS;
}
#endif /* IPv6 */

/*!
 * \brief Creates the SSDP IPv6 socket to be used by the control point.
 */
#ifdef UPNP_ENABLE_IPV6
static int create_ssdp_sock_reqv6(
    /* [out] SSDP IPv6 request socket to be created. */
    SOCKET *ssdpReqSock)
{
    char errorBuffer[ERROR_BUFFER_LEN];
    char hops = gSSDP_HOPS;

    *ssdpReqSock = socket(AF_INET6, SOCK_DGRAM, 0);
    if (*ssdpReqSock == -1) {
        strerror_r(errno, errorBuffer, ERROR_BUFFER_LEN);
        UpnpPrintf(UPNP_CRITICAL, SSDP, __FILE__, __LINE__,
               "Error in socket(): %s\n", errorBuffer);
        return UPNP_E_OUTOF_SOCKET;
    }
    /* MUST use scoping of IPv6 addresses to control the propagation os SSDP
     * messages instead of relying on the Hop Limit (Equivalent to the TTL
     * limit in IPv4). */
    setsockopt(*ssdpReqSock, IPPROTO_IPV6, IPV6_MULTICAST_HOPS,
           &hops, sizeof(hops));
    /* just do it, regardless if fails or not. */
    sock_make_no_blocking(*ssdpReqSock);

    return UPNP_E_SUCCESS;
}
#endif /* IPv6 */
#endif /* INCLUDE_CLIENT_APIS */

static UPNP_INLINE void shutdownSocket(int sock)
{
    if (sock != -1) {
        shutdown(sock, SD_BOTH);
        UpnpCloseSocket(sock);
    }
}
int get_ssdp_sockets(MiniServerSockArray * out)
{
    int retVal;

#ifdef INCLUDE_CLIENT_APIS
    out->ssdpReqSock4 = INVALID_SOCKET;
    out->ssdpReqSock6 = INVALID_SOCKET;
    /* Create the IPv4 socket for SSDP REQUESTS */
    if (strlen(gIF_IPV4) > 0) {
        retVal = create_ssdp_sock_reqv4(&out->ssdpReqSock4);
        if (retVal != UPNP_E_SUCCESS)
            return retVal;
        /* For use by ssdp control point. */
        gSsdpReqSocket4 = out->ssdpReqSock4;
    } else
        out->ssdpReqSock4 = INVALID_SOCKET;
    /* Create the IPv6 socket for SSDP REQUESTS */
#ifdef UPNP_ENABLE_IPV6
    if (strlen(gIF_IPV6) > 0) {
        retVal = create_ssdp_sock_reqv6(&out->ssdpReqSock6);
        if (retVal != UPNP_E_SUCCESS) {
            shutdownSocket(out->ssdpReqSock4);
            return retVal;
        }
        /* For use by ssdp control point. */
        gSsdpReqSocket6 = out->ssdpReqSock6;
    } else
        out->ssdpReqSock6 = INVALID_SOCKET;
#endif /* IPv6 */
#endif /* INCLUDE_CLIENT_APIS */
    /* Create the IPv4 socket for SSDP */
    if (strlen(gIF_IPV4) > 0) {
        retVal = create_ssdp_sock_v4(&out->ssdpSock4);
        if (retVal != UPNP_E_SUCCESS) {
#ifdef INCLUDE_CLIENT_APIS
            shutdownSocket(out->ssdpReqSock4);
            shutdownSocket(out->ssdpReqSock6);
#endif /* INCLUDE_CLIENT_APIS */
            return retVal;
        }
    } else
        out->ssdpSock4 = INVALID_SOCKET;
    /* Create the IPv6 socket for SSDP */
#ifdef UPNP_ENABLE_IPV6
    if (strlen(gIF_IPV6) > 0) {
        retVal = create_ssdp_sock_v6(&out->ssdpSock6);
        if (retVal != UPNP_E_SUCCESS) {
            shutdownSocket(out->ssdpSock4);
#ifdef INCLUDE_CLIENT_APIS
            shutdownSocket(out->ssdpReqSock4);
            shutdownSocket(out->ssdpReqSock6);
#endif /* INCLUDE_CLIENT_APIS */
            return retVal;
        }
    } else
        out->ssdpSock6 = INVALID_SOCKET;
    if (strlen(gIF_IPV6_ULA_GUA) > 0) {
        retVal = create_ssdp_sock_v6_ula_gua(&out->ssdpSock6UlaGua);
        if (retVal != UPNP_E_SUCCESS) {
            shutdownSocket(out->ssdpSock4);
            shutdownSocket(out->ssdpSock6);
#ifdef INCLUDE_CLIENT_APIS
            shutdownSocket(out->ssdpReqSock4);
            shutdownSocket(out->ssdpReqSock6);
#endif /* INCLUDE_CLIENT_APIS */
            return retVal;
        }
    } else
        out->ssdpSock6UlaGua = INVALID_SOCKET;
#endif /* UPNP_ENABLE_IPV6 */

    return UPNP_E_SUCCESS;
}
#endif /* EXCLUDE_SSDP */

/* @} SSDPlib */