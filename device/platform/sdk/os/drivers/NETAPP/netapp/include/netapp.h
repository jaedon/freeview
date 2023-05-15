/***************************************************************************
 *     (c)2005-2012 Broadcom Corporation
 *
 *  This program is the proprietary software of Broadcom Corporation and/or its licensors,
 *  and may only be used, duplicated, modified or distributed pursuant to the terms and
 *  conditions of a separate, written license agreement executed between you and Broadcom
 *  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 *  no license (express or implied), right to use, or waiver of any kind with respect to the
 *  Software, and Broadcom expressly reserves all rights in and to the Software and all
 *  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 *  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 *  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 *
 *  Except as expressly set forth in the Authorized License,
 *
 *  1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 *  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 *  and to use this information only in connection with your use of Broadcom integrated circuit products.
 *
 *  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 *  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 *  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
 *  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
 *  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
 *  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
 *  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
 *  USE OR PERFORMANCE OF THE SOFTWARE.
 *
 *  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
 *  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
 *  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 *  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 *  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
 *  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 *  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 *  ANY LIMITED REMEDY.
 *
 */

/**
 * @mainpage
 *
 * Network Application API
 *
 * @author Steven Hartley (steven@broadcom.com)
 *
 * The network API is designed to abstract the Linux(R) IPv4 network stack
 * to allow the application to configure networking devices like wired, wireless,
 * or Bluetooth(R) with a thin and simple API.
 *
 * NetApp currently supports these features:
 *  <ul>
 *      <li>Networking (TCP/IP):
 *      <ul>
 *          <li>Get/Set IP settings
 *          <li>DHCP Client Daemons
 *          <li>DHCP Server (for Wi-Fi DirectTM)
 *          <li>Bonjour(R)/Zero Configurations (Service Discovery)
 *          <li>IPv4LL, Multicast-DNS, DNS-Service Discovery
 *          <li>Multi-Threaded Single Process Support
 *          <li>Multiple clients connect and interact with NetApp (VUDU(R), DLNA(R),
 *          GUI/System Settings)
 *      </ul>
 *      <li>Wi-Fi:
 *      <ul>
 *          <li>Connection Manager
 *          <li>Wi-Fi Protected Setup (version 1.0 and 2.0)
 *          <li>Wi-Fi Invite
 *          <li>Wake-on-Wireless-LAN (WoWL)
 *          <li>Wi-Fi Direct
 *          <li>MiraCastTM (Wi-Fi Display)
 *      </ul>
 *      <li>Bluetooth:
 *      <ul>
 *          <li>Pairing/Bonding of HID remote control devices
 *          <li>Device and service discovery
 *          <li>HID Voice: Using FLAC and Google(R) Voice recognition search
 *          <li>Voice Recognition (Audio HID)
*          <li>A2DP (AV Sink)
*          <li>AVRCP
 *      </ul>
 *      <li>USB Hotplug
 *      <li>Database Back End:
 *      <ul>
 *          <li>SQLite
 *          <li>Automatically get/set settings to reconnect to previous access
 *          points, Bluetooth devices, etc.
 *      </ul>
 *      <li> iperf
 *      <li> flac (used for PCM->flac conversion for voice recognition)
 * </ul>
 */

/**
 * @defgroup NetApp NetApp API Overview
 * @brief The NetApp API is a collection of APIs that are used to control and configure the wired
 * and wireless network interfaces.
 * @{
 */

#ifndef NETAPP_APP_H
#define NETAPP_APP_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

//!< NetApp major version
#define NETAPP_VERSION_MAJOR    7
//!< NetApp minor version.
#define NETAPP_VERSION_MINOR    1
//!< NetApp inc version.
#define NETAPP_VERSION_INC      5


#define NETAPP_ENET_LEN         17      //!< Length of bytes for displaying an
                                        //!< Ethernet address, e.g., 00:00:00:00:00:00.
#define NETAPP_HW_ADDR_LEN      6       //!< Hardware address length.
#define NETAPP_NO_WAIT          0       //!< Not wait.
#define NETAPP_WAIT_FOREVER     -1      //!< Wait forever.
#define NETAPP_IFACE_NAME_LEN   10      //!< Interface name length

#define NETAPP_WOWL_NET_PATTERN_MAX_LENGTH  128 //!< Maximum size of a net pattern.
#define NETAPP_WOWL_MAX_NET_PATTERNS        4   //!< Maximum # net patterns we can set.

#define NETAPP_BT_NAME_LEN      248     //!< Length of a Bluetooth device name.


/**
 * @brief Hardware Address (MAC or BD/Bluetooth)
 */
typedef uint8_t NETAPP_HW_ADDR[NETAPP_HW_ADDR_LEN];


/**
 * @brief NetApp Module Handle
 */
typedef void *NETAPP_HANDLE;


/**
 * @brief The possible destinations for debug output from NetApp.
 */
typedef enum
{
    NETAPP_DEBUG_DESTINATION_NONE = 0x00,                 //!< no debug output
    NETAPP_DEBUG_DESTINATION_STDOUT = 0x01,                 //!< debug output goes to stdout.
    NETAPP_DEBUG_DESTINATION_SYSLOG = 0x02                 //!< debug output goes to syslog.
} NETAPP_DEBUG_DESTINATION;


/**
 * @brief The return code for most NetApp APIs.
 */
typedef enum
{
    NETAPP_SUCCESS = 0,                 //!< Success.
    NETAPP_FAILURE,                     //!< General failure.
    NETAPP_INVALID_PARAMETER,           //!< Invalid parameter.
    NETAPP_NULL_PTR,                    //!< Null handle detected or invalid state.
    NETAPP_OUT_OF_MEMORY,               //!< Malloc has failed.
    NETAPP_NOT_IMPLEMENTED,             //!< Function not implemented.
    NETAPP_NETWORK_UNREACHABLE,         //!< Unable to reach destination network.
    NETAPP_SOCKET_ERROR,                //!< Error creating the Linux socket.
    NETAPP_TIMEOUT,                     //!< Timeout error occurred.
    NETAPP_DHCP_FAILURE,                //!< Failure to fetch DHCPD address.
    NETAPP_HOST_NOT_FOUND,              //!< Not able to find host in DNS server.
    NETAPP_CANCELED,                    //!< The function was canceled.
    NETAPP_INCORRECT_PASSWORD,          //!< Incorrect password provided.
    NETAPP_INVALID_PIN,                 //!< Invalid WPS pin used.
    NETAPP_NOT_FOUND,                   //!< Tried to execute system command and
                                        //!< the search string was not found.
    NETAPP_NOT_SUPPORTED,               //!< Requesting an API or function that was
                                        //!< not supported/compiled in.
    NETAPP_WPS_MULTIPLE_AP_FOUND,       //!< Found more than one AP in WPS PBC (overlap).
    NETAPP_SCAN_EMPTY,                  //!< Scan was complete and no access points found.
    NETAPP_INVALID_STATE,               //!< Calling the API when the system is in an invalid state.
    NETAPP_WPS_2_ERR_INCOMPATIBLE       //!< WPS detected an AP that support a WPS 1.0
                                        //!< depreciated setting that is not supported in
                                        //!< WPS 2.0. The application should restart WPS
                                        //!< with NETAPP_SETTINGS.bWPS2_0 set to false.
} NETAPP_RETCODE;


/**
 * @brief Determins which interface to use: wired or wireless.
 */
typedef enum
{
    NETAPP_IFACE_WIRED,                     //!< Backwards compatibility
    NETAPP_IFACE_ETH0 = NETAPP_IFACE_WIRED, //!< 1st Wired
    NETAPP_IFACE_ETH1,                      //!< 2nd Wired
    NETAPP_IFACE_ETH2,                      //!< 3rd Wired
    NETAPP_IFACE_ETH3,                      //!< 4th Wired
    NETAPP_IFACE_ETH4,                      //!< 5th Wired
    NETAPP_IFACE_ETH5,                      //!< 6th Wired
    NETAPP_IFACE_BR0,						// br0
    NETAPP_IFACE_WIRED_MAX,                 //!< Number of Wired interfaces (used internally).
    NETAPP_IFACE_WIRELESS,                  //!< Wireless (might be remapped to an ETHx interface).
    NETAPP_IFACE_LOOPBACK,                  //!< Loopback (LO)
    NETAPP_IFACE_P2P,                       //!< Wi-Fi Direct
    NETAPP_IFACE_BLUETOOTH,                 //!< Bluetooth
    NETAPP_IFACE_MAX
} NETAPP_IFACE;


/**
 * @brief Interface information
 * @see NetAppGetIfaceInfo()
 */
typedef struct {
    NETAPP_IFACE tIface;                            //!< Interface type
    bool         bPresent;                          //!< Interface is present
    char         cName[NETAPP_IFACE_NAME_LEN+1];    //!< String interface name
} NETAPP_IFACE_INFO;


/**
 * @brief Network Access settings
 */
typedef enum
{
    NETAPP_IP_MODE_OFF = 0,             //!< Network off
    NETAPP_IP_MODE_STATIC,              //!< Network static
    NETAPP_IP_MODE_DYNAMIC,             //!< Network dynamic using dhcpcd
    NETAPP_IP_MODE_AUTO_IP              //!< RFC 3927 compliant IPv4LL
} NETAPP_IP_MODE;


/**
 * @brief Wi-Fi Security Type
 */
typedef enum
{
    NETAPP_WIFI_SECURITY_INVALID = 0,   //!< The security is not set or Invalid
    NETAPP_WIFI_SECURITY_AUTO_DETECT,   //!< Auto-detect the security type
    NETAPP_WIFI_SECURITY_NONE,          //!< No Security
    NETAPP_WIFI_SECURITY_WEP,           //!< Shared or Open, WEP
    NETAPP_WIFI_SECURITY_WPA_PSK_AES,   //!< WPA-Personal, AES encryption
    NETAPP_WIFI_SECURITY_WPA_PSK_TKIP,  //!< WPA-Personal, TKIP encryption
    NETAPP_WIFI_SECURITY_WPA2_PSK_AES,  //!< WPA2-Personal, AES encryption
    NETAPP_WIFI_SECURITY_WPA2_PSK_TKIP, //!< WPA-Personal, TKIP encryption
    NETAPP_WIFI_SECURITY_NOT_SUPPORTED  //!< Security format not supported
} NETAPP_WIFI_SECURITY;


/**
 * @brief Wi-Fi IEEE 802.11 Modes
 */
typedef enum
{
    NETAPP_WIFI_802_11_NONE     = 0x0000,   //!< None are supported (invalid)
    NETAPP_WIFI_802_11_MODE_A   = 0x0001,   //!< IEEE 802.11A
    NETAPP_WIFI_802_11_MODE_B   = 0x0002,   //!< IEEE 802.11B
    NETAPP_WIFI_802_11_MODE_G   = 0x0004,   //!< IEEE 802.11G
    NETAPP_WIFI_802_11_MODE_N   = 0x0008    //!< IEEE 802.11N
} NETAPP_WIFI_802_11_MODE;


/**
 * @brief Wi-Fi Received Signal Strength Indicator
 */
typedef enum
{
    NETAPP_WIFI_RSSI_NONE = 0,      //!< No signal (0 bar)
    NETAPP_WIFI_RSSI_POOR,          //!< Poor (1 bar)
    NETAPP_WIFI_RSSI_FAIR,          //!< Fair (2 bars)
    NETAPP_WIFI_RSSI_GOOD,          //!< Good (3 bars)
    NETAPP_WIFI_RSSI_EXCELLENT,     //!< Excellent (4 bars)
    NETAPP_WIFI_RSSI_MAX
} NETAPP_WIFI_RSSI;


/**
 * @brief Link Status
 */
typedef enum
{
    NETAPP_LINK_DOWN = 0,               //!< Network link is down.
    NETAPP_LINK_UP,                     //!< Network link is up and IP address is obtained.
    NETAPP_LINK_ACQUIRING              //!< In the process of fetching the IP address from DHCPCD.
} NETAPP_LINK_STATE;


/**
  * @brief IPv4 Internet address type definition.
  * The networking stack used for the BCM7XXX family of chips uses an unsigned
  * 32-bit integer.
  */
typedef uint32_t NETAPP_IPV4_ADDR;


/**
  * @brief NetApp Settings.
  * This structure contains the network configuration settings.
  */
typedef struct
{
    char             cMacAddress[NETAPP_ENET_LEN+1];//!< MAC Address
    NETAPP_IPV4_ADDR tIpAddress;                  //!< IP address
    NETAPP_IPV4_ADDR tSubnetMask;                 //!< Subnet Mask
    NETAPP_IPV4_ADDR tGateway;                    //!< Gateway Address
    NETAPP_IPV4_ADDR tPrimaryDNS;                 //!< Primary DNS Address
    NETAPP_IPV4_ADDR tSecondaryDNS;               //!< Secondary DNS Address
} NETAPP_IP_SETTINGS;


#define NETAPP_MAX_SSID_LEN        32       //!< Maximum SSID name
#define NETAPP_MAX_PASSWORD_LEN    64       //!< Maximum password length
#define NETAPP_UUID_LEN            16       //!< Length of a UUID in Bytes
#define NETAPP_ZEROCONF_NAME_LEN   32       //!< Length of service type and name
#define NETAPP_MAX_ANTENNAS        4       //!< Maximum number of antennas supported

/**
 * @brief Wi-FiChannel Bandwidth
 */
typedef enum
{
    NETAPP_WIFI_BANDWIDTH_INVALID,          //!< Invalid bandwidth setting
    NETAPP_WIFI_BANDWIDTH_10MHz,            //!< 10 MHz
    NETAPP_WIFI_BANDWIDTH_20MHz,            //!< 20 MHz
    NETAPP_WIFI_BANDWIDTH_40MHz             //!< 40 MHz
} NETAPP_WIFI_BANDWIDTH;


/**
 * @brief Wi-Fi Channel Sideband
 */
typedef enum
{
    NETAPP_WIFI_SIDEBAND_INVALID,          //!< Invalid sideband setting
    NETAPP_WIFI_SIDEBAND_LOWER,            //!< lower 20 MHz of a 40 MHz band
    NETAPP_WIFI_SIDEBAND_UPPER             //!< upper 20 MHz of a 40 MHz band
} NETAPP_WIFI_SIDEBAND;


/**
 * @brief Wi-Fi Antennas - used for antenna selection
 */
typedef enum
{
    NETAPP_WIFI_ANTENNA_INVALID = 0x00,
    NETAPP_WIFI_ANTENNA_1 = 0x01,
    NETAPP_WIFI_ANTENNA_2 = 0x02,
    NETAPP_WIFI_ANTENNA_3 = 0x04,
    NETAPP_WIFI_ANTENNA_4 = 0x08
} NETAPP_WIFI_ANTENNA;

/**
 * @brief Wi-Fi antenna mode of operation - used for antenna selection
 */
typedef enum
{
    NETAPP_WIFI_ANTENNA_MODE_RX,
    NETAPP_WIFI_ANTENNA_MODE_TX
} NETAPP_WIFI_ANTENNA_MODE;


/**
 * @brief Wi-Fi transmission and receive settings
 */
typedef struct
{
    uint32_t inter_packet_gap_us;    //!< interframe gap (IFG), interframe spacing, or interpacket gap (IPG) in micro seconds.
    uint32_t packet_size;            //!< packet size in bytes.
    int8_t transmit_power;           //!< transmit power in dBm.
    uint8_t channel;                 //!< transmit/receive channel.
    NETAPP_WIFI_BANDWIDTH bandwidth;       //!< channel bandwidth.
    NETAPP_WIFI_SIDEBAND sideband;         //!< channel sideband on a 40Mhz channel.
    NETAPP_WIFI_802_11_MODE protocol;      //!< 802.11 protocol.
    uint8_t mcsindex;                //!< MCS index.
    uint32_t data_rate;              //!< transmit/receive data rate in Mbps.
    char acMac[NETAPP_ENET_LEN + 1]; //!< address of sender (used only in receive mode).
} NETAPP_WIFI_TXRX_SETTINGS;


/**
 * @brief RSSI values per antenna
 */
typedef struct
{
    NETAPP_WIFI_RSSI tRSSI_ant[NETAPP_MAX_ANTENNAS];
    NETAPP_WIFI_RSSI tRSSI;
    int32_t lRSSI_ant[NETAPP_MAX_ANTENNAS];
    int32_t lRSSI;
    uint32_t ulNumAntennas;
} NETAPP_WIFI_RSSI_PER_ANTENNA;

/**
  * @brief NetApp Wi-Fi Access Point Information
  *
  */
typedef struct
{
    char                    cSSID[NETAPP_MAX_SSID_LEN+1];       //!< Service Set Identifier (BSSID)
    char                    cBSSID[NETAPP_ENET_LEN+1];             //! Basic Service set Identifier (BSSID)
    char                    cPassword[NETAPP_MAX_PASSWORD_LEN+1];//!< Password
    NETAPP_WIFI_RSSI        tRSSI;                      //!< Received Signal Strength Indicator (generalized)
    int32_t                 lRSSI;                      //!< Received Signal Strength Indicator (-db)
    uint32_t                tMode;                      //!< Supported IEEE 802.11 modes (a, b, g, n, ac, etc.)
                                                        //!< This is a bitmask using NETAPP_WIFI_802_11_MODE.
    NETAPP_WIFI_SECURITY    tSecurity;                  //!< Security modes supported.
    bool                    bAdHoc;                     //!< AdHoc network or not.
    bool                    bWPS;                       //!< AP supports/implements WPS.
    uint32_t                ulChannel;                  //!< Channel AP is configured on.
    int32_t                 lRate;                      //!< Calculated Speed/Rate in 500 Kbps .units
    int32_t                 lPhyNoise;                  //!< The physical noise (in dBm).
    NETAPP_WIFI_BANDWIDTH   tChanBandwidth;             //!< The current channel bandwidth (20 MHz, 40 MHz, etc.).
} NETAPP_WIFI_AP_INFO;


/**
 * @brief SoftAp Settings
 *
 * Structure containing the various settings when NetApp is configured as a SoftAP or
 * when we are chosen to be the group owner in a Wi-Fi Direct Connection
 */
typedef struct
{
    NETAPP_WIFI_AP_INFO tApInfo;                    //!< Access point info (SSID, security, etc...)
    NETAPP_IPV4_ADDR    tIpAddress;                 //!< IP address
    NETAPP_IPV4_ADDR    tSubnetMask;                //!< Subnet Mask
} NETAPP_SOFTAP_SETTINGS;


/**
 * @brief Callback Type
 *
 * List of supported callbacks from events that occur in the NetApp API.
 */
typedef enum
{
    NETAPP_CB_INVALID = 0,          //!< Initialization for this enum.
    NETAPP_CB_LINK,                 //!< Link change Event:<br>
                                    //!<    pvBuffer: N/A<br>
                                    //!<    ulData0:  NETAPP_LINK_STATE

    NETAPP_CB_CONNECT,              //!< Connection results for Wi-Fi or Bluetooth<br>
                                    //!<    pvBuffer: Pointer to either the NETAPP_WIFI_AP_INFO
                                    //!<              or NETAPP_BT_DEV_INFO structure<br>
                                    //!<    ulData0:  NETAPP_BT_SERVICE_TYPE for Bluetooth callbacks

    NETAPP_CB_DISCONNECT,           //!< Disconnection results for Bluetooth.<br>
                                    //!<    pvBuffer: NETAPP_BT_DEV_INFO for Bluetooth<br>
                                    //!<    ulData0:  N/A

    NETAPP_CB_INPUT_EVENT,          //!< AV Remote control from Bluetooth audio<br>
                                    //!<    pvBuffer: Pointer to the NETAPP_INPUT_INFO structure<br>
                                    //!<    ulData0:  N/A

    NETAPP_CB_PING,                 //!< Results from a Ping request. <br>
                                    //!<    pvBuffer: The server name passed to the ping request<br>
                                    //!<    ulData0:  N/A

    NETAPP_CB_DNSLOOKUP,            //!< DNSLookup results. <br>
                                    //!<    pvBuffer: The server name passed to the lookup request<br>
                                    //!<    ulData0:  N/A

    NETAPP_CB_INVITE,               //!< Wi-Fi Invite request received. <br>
                                    //!<    pvBuffer: The SSID from the inviting device<br>
                                    //!<    ulData0:  N/A

    NETAPP_CB_SCAN_DONE,            //!< A scan is complete and results are available. <br>
                                    //!<    pvBuffer: N/A<br>
                                    //!<    ulData0:

    NETAPP_CB_SCANNED_APINFO,       //!< A scan is complete and scanned AP info is included. <br>
                                    //!<    pvBuffer: The server name passed to the lookup request<br>
                                    //!<    ulData0:  Scan count (for background scans)

    NETAPP_CB_FETCHED_APINFO,       //!< Received the results from the API NetAppWiFiGetApInfo() <br>
                                    //!<    pvBuffer: Pointer to the NETAPP_WIFI_AP_INFO<br>
                                    //!<    ulData0:  N/A

    NETAPP_CB_NTPDATE,              //!< NTPDate request is finished with results. <br>
                                    //!<    pvBuffer: N/A<br>
                                    //!<    ulData0:  N/A

    NETAPP_CB_SETSETTINGS,          //!< The result from a call to NetAppSetNetworkSettings(). <br>
                                    //!<    pvBuffer: N/A <br>
                                    //!<    ulData0:  N/A

    NETAPP_CB_HOTPLUG,              //!< NetApp Detected a hotplug. <br>
                                    //!<    pvBuffer: Pointer to the NETAPP_HOTPLUG_DEVICE_INFO structure<br>
                                    //!<    ulData0:  N/A

    NETAPP_CB_RSSI_EVENT,           //!< The RSSI of a connected AP changed levels. <br>
                                    //!<    pvBuffer: N/A<br>
                                    //!<    ulData0:  The RSSI (NETAPP_WIFI_RSSI)

    NETAPP_CB_ZEROCONF,             //!< NetApp has found a browse/discovery request. <br>
                                    //!< service or the service is removed.
                                    //!<    pvBuffer: Service name<br>
                                    //!<    ulData0:  Service state (NETAPP_ZEROCONF_SERVICE_STATE)

    NETAPP_CB_P2P_PEER,             //!< Discovered Wi-Fi Direct Peer information. <br>
                                    //!<    pvBuffer: Pointer to the peer info structure NETAPP_P2P_PEER_INFO<br>
                                    //!<    ulData0:  Discovery count

    NETAPP_CB_P2P_CONNECT,          //!< Wi-Fi Direct Connection is established. <br>
                                    //!<    pvBuffer: Pointer to the peer info structure NETAPP_P2P_PEER_INFO<br>
                                    //!<    ulData0:  N/A

    NETAPP_CB_BT_DISCOVERY_RESULTS, //!< Bluetooth discovery is complete and results are available. <br>
                                    //!<    pvBuffer: N/A<br>
                                    //!<    ulData0:  N/A

    NETAPP_CB_BT_SP_CONFIRM_REQ,    //!< Simple pairing confirm request <br>
                                    //!< The user must then Accept or Reject the SP request. <br>
                                    //!<    pvBuffer: Pointer to the device info structure NETAPP_BT_DEV_INFO<br>
                                    //!<    ulData0:  Simple pairing password key

    NETAPP_CB_BT_SP_NOTIFY,         //!< Simple pairing notification. <br>
                                    //!<    pvBuffer: N/A<br>
                                    //!<    ulData0:  Simple pairing key

    NETAPP_CB_BT_AUTH_COMPLETE,     //!< Bluetooth authentication. <br>
                                    //!<    pvBuffer: N/A<br>
                                    //!<    ulData0:  Simple pairing key

    NETAPP_CB_BT_HID_VOICE_INFO,    //!< Notify the application a file has been created for HID  audio. <br>
                                    //!<    pvBuffer: Pointer to the NETAPP_BT_HID_VOICE_INFO structure <br>
                                    //!<    ulData0:  N/A

    NETAPP_CB_VOICE_REC_DONE,       //!< Finished a voice recognition request and a string is available. <br>
                                    //!<    pvBuffer: Voice recognized string (char*)<br>
                                    //!<    ulData0:  N/A

    NETAPP_CB_DHCP_LEASE_RESPONSE,  //!< Responded to a DHCP lease request when NetApp is a DHCP server (SoftAP or P2P). <br>
                                    //!<    pvBuffer: N/A<br>
                                    //!<    ulData0:  IP Address in IPv4 notation

    NETAPP_CB_BT_AVK_STATE,         //!< Received an AVK state change notification that must be acted upon. <br>
                                    //!<    pvBuffer: Pointer to the NETAPP_BT_DEV_INFO structure <br>
                                    //!<    ulData0:  State information (PLAY, PAUSE, STOP, etc.) NETAPP_BT_AVK_STATE

    NETAPP_CB_BT_AVK_CHUNK,         //!< Received Audio buffer that needs to be pushed to some playback engine <br>
                                    //!<    pvBuffer: Pointer to the received PCM data <br>
                                    //!<    ulData0:  Buffer size

    NETAPP_CB_DYING,                //!< NetApp encountered a fatal error and cannot recover. <br>
    NETAPP_CB_MAX = NETAPP_CB_DYING //!< End of the callback list.
} NETAPP_CB_TYPE;


/**
 * @brief AVK State notification from the AV Source device
 */
typedef enum
{
    NETAPP_BT_AVK_STATE_PLAY,       //!< Received the Play notification
    NETAPP_BT_AVK_STATE_STOP        //!< Received the Stop notification
} NETAPP_BT_AVK_STATE;


/**
 * @brief The Browsed service state "hotplug" information (inserted or removed).
 */
typedef enum
{
    NETAPP_ZEROCONF_SERVICE_FOUND,    //!< Service name is found (discovered).
    NETAPP_ZEROCONF_SERVICE_REMOVED   //!< Service name was removed, no longer available.
} NETAPP_ZEROCONF_SERVICE_STATE;



/**
 * @brief WoWL Net Pattern Info
 */
typedef struct
{
    uint32_t    ulOffset;               //!< Offset within payload to start
                                        //!< looking for the pattern.

    char        cMask[NETAPP_WOWL_NET_PATTERN_MAX_LENGTH/8]; //!< Each bit of
                                        //!< the mask corresponds to a byte of
                                        //!< date in 'value' of the pattern --
                                        //!< bit i of the mask = 1 => match byte
                                        //!< i of the pattern with payload.

    char        cValue[NETAPP_WOWL_NET_PATTERN_MAX_LENGTH]; //!< Pattern data,
                                        //!< In bytes, of payload to match against.
    uint8_t     ucLength;               //!< Pattern Length (bytes).
} NETAPP_WOWL_NET_PATTERN;


/**
 * @brief The P2P device type
 */
typedef enum
{
    NETAPP_DEVICE_TYPE_OTHER,       //!< Device type is not specified or is not
                                    //!< one of those in the list below.
    NETAPP_DEVICE_TYPE_DTV,         //!< Digital Television
    NETAPP_DEVICE_TYPE_BD           //!< Blu-ray Player
} NETAPP_DEVICE_TYPE;


/**
 * @brief Wi-Fi Direct Service List
 *
 * Enum to tell the other device what type of device we are.
 */
typedef enum
{
    NETAPP_P2P_SVC_NONE     = 0,
    NETAPP_P2P_SVC_FILE_TX  = 0x001,    //!< File Transfer
    NETAPP_P2P_SVC_PRINT    = 0x0002,   //!< Print service
    NETAPP_P2P_SVC_DISPLAY  = 0x0004,   //!< Display
    NETAPP_P2P_SVC_ALL      = (NETAPP_P2P_SVC_FILE_TX   |
                               NETAPP_P2P_SVC_PRINT     |
                               NETAPP_P2P_SVC_DISPLAY)    //!< All services
} NETAPP_P2P_SERVICES;


/**
 * @struct NETAPP_P2P_DISCOVER_PARAMS
 * @brief Parameters for a Wi-Fi Direct Discovery
 */
typedef struct
{
    int32_t                 lTimeoutSec;    //!< How long do we discover for (sec) -1 is forever.
    int32_t                 lScanTimeMs;    //!< How long to linger on a channel
                                            //!< (in ms). Setting to -1 will choose default.
    uint32_t                ulServices;     //!< Bitmask of services we want to support.
                                            //!< @see NETAPP_P2P_SERVICES
    uint32_t                ulSocialCh;     //!< The listen channel to park on to listen
                                            //!< for probe requests during the Listen phases
                                            //!< of the P2P SIG discovery procedure. If 0,
                                            //!< a default value will be used.
} NETAPP_P2P_DISCOVER_PARAMS;


/**
 * @struct NETAPP_P2P_PEER_INFO
 * @brief Wi-Fi Direct Peer Info
 */
typedef struct
{
    NETAPP_WIFI_AP_INFO     tInfo;          //!< Common Wi-Fi info (SSID, BSSID,
                                            //!< channel, signal strength, and IEEE 802.11 modes.
    uint32_t                ulServices;     //!< Bitmask of services we want to support
                                            //!< @see NETAPP_P2P_SERVICES
    bool                    bIsGO;          //!< The peer is the group owner.
    NETAPP_IPV4_ADDR    	tIpAddress;     //!< IP address

    /* The following items are fetched from the IE exchange in WiFi Direct and is
     * only supported in the latest driver version.
     */
    uint32_t                ulRtspPort;     //!< RTSP port number found in WiFi
    uint32_t                ulRtpPort;      //!< RTP port number
    uint32_t                ulVideoPid;     //!< Video PID
    uint32_t                ulAudioPid;     //!< Audio PID
} NETAPP_P2P_PEER_INFO;


/**
 * @brief Wake-on-Wireless-LAN Wakeup Event Type
 */
typedef enum
{
    NETAPP_WOWL_EVENT_NONE              = 0x00, //!< Do not wake up on any event.
    NETAPP_WOWL_EVENT_MAGIC_PATTERN     = 0x01, //!< Wake up on magic pattern.
    NETAPP_WOWL_EVENT_DISASSOC_DEAUTH   = 0x02, //!< Wake up on disassociate from AP.
    NETAPP_WOWL_EVENT_LOSS_OF_BEACON    = 0x04, //!< Wake up on loss of beacon.
    NETAPP_WOWL_EVENT_NET_PATTERN       = 0x08  //!< Wake up on a special net pattern.
} NETAPP_WOWL_EVENT;


/**
 * @brief WoWL Settings
 */
typedef struct
{
    uint32_t                ulMask;             //!< Mask of events to wakeup on.
    NETAPP_WOWL_NET_PATTERN tNetPattern[NETAPP_WOWL_MAX_NET_PATTERNS]; //!< Net Patterns.
    uint32_t                ulBeaconLossSeconds;//!< Number of second of beacon loss.
} NETAPP_WOWL_SETTINGS;


/**
 * @brief NetApp Wi-Fi Callback
 *
 * Refer to the NetApp Design document for more information on the parameters
 * passed to each NetApp Callback.
 */
typedef void (*NETAPP_CALLBACK)(
    void                *pParam,        //!< The pParam from NetAppOpen.
    NETAPP_CB_TYPE      tCbType,        //!< The Callback type.
    const void          *pvBuffer,      //!< Pointer to data specific to the callback.
    uint32_t            ulData0,        //!< Callback specific data 0.
    NETAPP_RETCODE      tResult,        //!< Callback results from the.
    NETAPP_IFACE        tIFace);        //!< Callback Info structure.


/**
 * @brief Bluetooth Service Type
 *
 * List of the possible service types discovered or supported
 */
typedef enum
{
    NETAPP_BT_SERVICE_NONE  = 0x0000,   //!< None.
    NETAPP_BT_SERVICE_HID   = 0x0001,   //!< Human Interface Device.
    NETAPP_BT_SERVICE_HSP   = 0x0002,   //!< Headset profile.
    NETAPP_BT_SERVICE_HFP   = 0x0004,   //!< Hands-free profile.
    NETAPP_BT_SERVICE_OPP   = 0x0008,   //!< Object push.
    NETAPP_BT_SERVICE_FTP   = 0x0010,   //!< File transfer.
    NETAPP_BT_SERVICE_A2DP  = 0x0020,   //!< Advanced audio distribution.
    NETAPP_BT_SERVICE_AVRCP = 0x0040,   //!< A/V remote control.
    NETAPP_BT_SERVICE_ALL   = 0xffff    //!< All Services.
} NETAPP_BT_SERVICE_TYPE;


#define NETAPP_BT_PIN_CODE_LEN 128      //!< Length of a pin code.

/**
 * @brief Bluetooth Simple Pairing Notification Event
 *
 * This enum is passed as the ulData0 in the NETAPP_CB_BT_SIMPLE_PAIRING callback.
 */
typedef enum
{
    NETAPP_BT_SP_CONFIRM_REQUEST,       //!< Notify the user that they must accept or
                                        //!< reject a simple pairing request.
    NETAPP_BT_SP_NOTIFY                 //!< Simple pairing notification event.

} NETAPP_BT_SP_EVENT;

/**
 * @brief Bluetooth Settings
 */
typedef struct
{
    bool                bDiscoverable;  //!< Device is/is not discoverable.
    bool                bAutoPair;      //!< Automatically accept Incoming Pair requests.
    uint8_t             ucPinCode[NETAPP_BT_PIN_CODE_LEN];//!< Security pin used for Pairing.
    uint32_t            ulPinLength;    //!< Length of the security pin.
} NETAPP_BT_SETTINGS;


#define NETAPP_HID_DSCPINFO_MAX     800
#define NETAPP_LINK_KEY_LEN         16


/**
 * @brief Bluetooth HID Information
 */
typedef struct
{
    uint32_t    ulLength;                       //!< Length of the descriptor.
    uint8_t     usData[NETAPP_HID_DSCPINFO_MAX];//!< Buffer containing the descriptor.
} NETAPP_BT_HID_INFO;


#define NETAPP_BT_HID_AUDIO_FILENAME_LEN    50 //!< Max filename path
/**
 * @brief HID Voice Info structure
 */
typedef struct
{
    uint8_t   nbChannels;           //!< Number of channels (generally 1 mono)
    uint32_t  sampleRate;           //!< Sample rate in Hertz
    uint16_t  bitsPerSample;        //!< Number of bits per sample
    char      hidAudioFilename[NETAPP_BT_HID_AUDIO_FILENAME_LEN];//!< Audio filename full path
    bool      isAudioDevice;
} NETAPP_BT_HID_VOICE_INFO;


#define BT_DEVICE_FEATURE_LEN   8   //!< Length of Bluetooth device features list.

/**
 * @brief Bluetooth Device Information
 */
typedef struct
{
    char                cAddr[NETAPP_ENET_LEN+1];   //! Hardware Address in xx:xx:xx:xx:xx notation.
    char                cName[NETAPP_BT_NAME_LEN+1];//!< Device.
    int32_t             lRssi;                      //!< Receiver Signal Strength Indicator.
    uint32_t            ulServiceMask;              //!< Discovered services.
    uint8_t             usLinkKey[NETAPP_LINK_KEY_LEN];//!< Link key obtained from connection.
    bool                bHasLinkKey;                //!< There is a link key present
    NETAPP_BT_HID_INFO  tHidInfo;                   //!< HID descriptor information
    uint8_t             ucMajorClassDev;            //!< Major class of device (see spec)
    uint8_t             ucMinorClassDev;            //!< Minor class of device (see spec)
    uint16_t            usServiceClassDev;          //!< Service class of device (see spec)
    uint16_t            usProductID;                //!< Product ID
    uint16_t            usVendorID;                 //!< Vendor ID

    /* Items below are needed for reconnection  only if you do not use dtaabase backend*/
    uint32_t            ulTrustedServiceMask;       //!< List of Trusted Service
    uint8_t             ucKeyType;                  //!< Key Type Information
    uint8_t             ucDeviceFeatures[BT_DEVICE_FEATURE_LEN]; //!< Device Features
} NETAPP_BT_DEV_INFO;


/**
 * @brief Hotplug Action Type (Add/Remove)
 */
typedef enum
{
    NETAPP_HOTPLUG_ADD,                 //!< The device is inserted/added.
    NETAPP_HOTPLUG_REMOVE               //!< Device has been removed.
} NETAPP_HOTPLUG_ACTION;


/**
 * @brief Hotplug Device Type
 */
typedef enum
{
    NETAPP_HOTPLUG_DEVICE_USB_INPUT,            //!< USB Input Device.
    NETAPP_HOTPLUG_DEVICE_USB,                  //!< Lower level USB device information.
    NETAPP_HOTPLUG_DEVICE_BLUETOOTH,            //!< Bluetooth device.
    NETAPP_HOTPLUG_DEVICE_WIFI                 //!< Wi-Fi Interface.
} NETAPP_HOTPLUG_DEVICE_TYPE;


/**
 * @brief USB hotplug information sent when NetApp detects a hotplug event
 * @remarks The strings returned form a hotplug event are not cached so you MUST
 * keep a copy of them if you need them after the hotplug event.
 */
#define NETAPP_HP_MAX_LEN_SHORT 10
#define NETAPP_HP_MAX_LEN 128
typedef struct
{
    NETAPP_HOTPLUG_ACTION       tAction;        //!< Hotplug Action (insert/remove)
    NETAPP_HOTPLUG_DEVICE_TYPE  tType;          //!< Device Type (e.g., input)
    char                        cSysName[NETAPP_HP_MAX_LEN_SHORT+1];      //!< System name (e.g., event0, event1, etc.)
    char                        cSysPath[NETAPP_HP_MAX_LEN+1];      //!< System Path (path in )

    /* Below is only available for NETAPP_HOTPLUG_DEV_USB_DEVICE */
    uint16_t                    usVendorID;     //!< Vendor ID (VID)
    uint16_t                    usProductID;    //!< Product ID (PID)
    char                        cManufacturer[NETAPP_HP_MAX_LEN+1]; //!< Manufacturer Name
    char                        cProduct[NETAPP_HP_MAX_LEN+1];      //!< Product Name
    char                        cSerialNumber[NETAPP_HP_MAX_LEN+1]; //!< Serial information
    char                        cDevType[NETAPP_HP_MAX_LEN+1];      //!< Device Type
} NETAPP_HOTPLUG_DEVICE_INFO;


/**
 * @brief AV Audio mode (number of channels)
 */
typedef enum
{
    NETAPP_BT_AV_MODE_NONE = 0,     //!< None (invalid case)
    NETAPP_BT_AV_MODE_MONO,         //!< Mono
    NETAPP_BT_AV_MODE_STEREO        //!< Stereo
} NETAPP_BT_AV_MODE;


/**
 * @brief Bluetooth Audio Format Information
 */
typedef struct
{
    NETAPP_BT_AV_MODE   tMode;              //!< Mode (Number of channels)
    uint32_t            ulSampleRate;       //!< Sample Rate
    uint16_t            ucBitsPerSample;    //!< Bits per sample
} NETAPP_BT_AUDIO_FORMAT;


/**
 * @brief Input Event information
 * @see NETAPP_CB_INPUT_EVENT
 */
typedef struct
{
    uint32_t        ulKey;              //!< Input code
    bool            bPressed;           //!< Pressed or released
} NETAPP_INPUT_INFO;


/**
 * @brief MIMO Bandwidth Capabilities
 */
typedef enum {
    NETAPP_MIMO_BW_CAP_20MHZ = 0,       //!< 20MHz on both 2.4 and 5 GHz bands
    NETAPP_MIMO_BW_CAP_40MHZ,           //!< 40MHz on both 2.4 and 5 GHz bands
    NETAPP_MIMO_BW_CAP_20MHZ_40MHz      //!< 20MHz on 2.4GHz and 40MHz on 5 GHz band
} NETAPP_MIMO_BW_CAP;


/**
 * @brief NetApp Initialization Settings Structure
 *
 * Settings structure that is passed to the first NetAppOpen Call as these settings
 * can only be set when NetApp is initiated and last for the duration of the API.
 */
typedef struct
{
    char                *pDeviceName;           //!< Null terminated Device Name string (max. 32 characters).
    char                *WiFiIfacePrefix;       //!< Wi-Fi interface name prefix, only used on NetAppOpen(),
                                                //!< If not set the default is wln.
    bool                bAllowNFS;              //!< Allow Wired network config when there is
                                                //!< an NFS mount
    bool                bBurstScanResults;      //!< Send scan results one ap at a time (burst) or
                                                //!< only as a single notification that the scan results
                                                //!< are available.
    char                *pCountryCode;          //!< The settings is used to determine the country and power
                                                //!< level settings for the dongle. Normally this settings is
                                                //!< programmed into the OTP of the dongle however sometimes the
                                                //!< setting needs to change (i.e., FCC testing is done after the
                                                //!< dongle was manufactured or a more optimized value is found.
                                                //!< The correct country code setting should come from the WLAN
                                                //!< team. Please consult your PM for the product to know the
                                                //!< right setting. If left blank the country code is not set.
    char                *pManufacturer;         //!< Manufacturer Name (max. 64 characters
    char                *pModelName;            //!< Model Name (max. 32 characters
    char                *pModelNumber;          //!< Model Number (max. 32 characters
    char                *pSerialNumber;         //!< Manufacturer Name (max. 32 characters)
    uint8_t             cWPSUUID[NETAPP_UUID_LEN];//!< The WPS UUID inserted in the M1/M2
                                                //!< UUID-E/UUID-R fields
    uint8_t             cTransportUUID[NETAPP_UUID_LEN]; //!< UUID passed to Microsoft Rally Virtual Paring
                                                //!< vendor extension to support WCN-NET
    const char          *pDBPath;               //!< The path to dump the database (default = /tmp)
    NETAPP_MIMO_BW_CAP  tMimoBwCap;             //!< Set the MIMO Bandwidth Capabilities for WiFi (default is
                                                //!< NETAPP_MIMO_BW_CAP_20MHZ_40MHz. For higher throughput at
                                                //!< 2.4GHz band you can change this to NETAPP_MIMO_BW_CAP_40MHZ
} NETAPP_INIT_SETTINGS;


/**
 * @brief NetApp Open Settings Structure
 *
 * Structure passed to all NetAppOpen() API calls to set a callback
 */
typedef struct
{
    NETAPP_CALLBACK tCallback;              //!< The callback to notify application of an event
    void            *pParam;                //!< Parameter passed to the callback (can be NULL)
} NETAPP_OPEN_SETTINGS;


/**
 * @brief General NetApp Settings Structure
 *
 * These are settings that can change on the fly in NetApp and should be passed to the first
 * NetAppOpen() call.
 */
typedef struct
{
    bool                    bZeroconfOn;        //!< Automatically run the zeroconf networking upon the interface
                                                //!< coming up/down.
    bool                    bAutoReConnect;     //!< Automatically reconnect to the previously successful
                                                //!< connected Wi-Fi access point if the connection goes down or if the
                                                //!< wired interface goes down and we need to bring up the wireless
                                                //!< interface. Also automatically reconnect to saved Bluetooth devices.
    bool                    bForceWiFi;         //!< Enables Multi-interface support so WiFi can be enabled when Ethernet
                                                //!< link is up. If set to false, WiFi will not reconnect when WiFi link goes down.
    bool                    bWPS2_0;            //!< WPS 2.0 Support enabled
    bool                    bHideDuplicateAPs;  //!< If we have multiple AP's with the same SSID, we will hide all the duplicates
    NETAPP_WOWL_SETTINGS    tWoWLSettings;      //!< Wake-On-Wireless-LAN Settings
    NETAPP_BT_SETTINGS      tBtSettings;        //!< Bluetooth Settings
    NETAPP_P2P_DISCOVER_PARAMS tDefP2PParams;   //!< Default P2P connection Parameters
    uint8_t                 ucP2PGOIntent;      //!< The Group owner Intent value (<7 we would prefer to be the client,
                                                //!< >7 we would prefer to be the GO).
    bool                    bAutoP2PDiscover;   //!< Run P2P Discovery in the background and allow
                                                //!< automatic connection to the device (not used
                                                //!< if bP2PGOset to true.
    bool                    bWiFiBridgingMode;  //!< Enable bridging support puts the WiFi interface in promisc
                                                //!< mode and enables WET
    /* Below are settings to make NetApp as an access point */
    bool                    bIsSoftAp;          //!< Enable SoftAP, default: false
    NETAPP_SOFTAP_SETTINGS  tSoftApSettings;    //!< SoftAp Settings to configure NetApp when the
                                                //!< Wi-Fi interface is configured as an access point.
} NETAPP_SETTINGS;


/**
 * @brief Zero Configuration Service Information
 *
 * The following structure is passed in the callback @see NETAPP_CB_ZEROCONF_SERVICE
 * when we browse for a service and a service is found. The service information is
 * cached inside of NetApp and you can get a reference to the cached data by
 * calling @see NetAppZeroConfGetBrowseResults().
 *
 * @remarks DO NOT free this structure; NetApp will take care of cleaning up.
 */
typedef struct sNETAPP_ZEROCONF_SERVICE_INFO
{
    char                *pName;             //!< Name (used to lookup the rest of the data)
    char                *pType;             //!< type (e.g., _http._tcp)
    char                *pDomain;           //!< Domain (e.g., local)
    char                *pHostName;         //!< Host Name
    uint32_t            ulPort;             //!< Port number used for the service
    char                *pTxtRecord;        //!< TXT Records for the discovered service
    uint32_t            ulTxtLength;        //!< TXT Records for the discovered service
    NETAPP_IPV4_ADDR    tIpAddress;         //!< IP address
} NETAPP_ZEROCONF_SERVICE_INFO;


/**
 * @brief Version Information
 *
 */
typedef struct sNETAPP_VERSION_INFO
{
    uint8_t                ulMajor;             //!< NetApp major version
    uint8_t                ulMinor;             //!< NetApp minor version
    uint8_t                ulInc;             //!< NetApp inc version
} NETAPP_VERSION_INFO;



/**
 * @brief Fetch NetApp version information
 *
 * @param[out] pVersionInfo  - NetApp Settings structure
 *
 * @return NETAPP_RETCODE
 */
NETAPP_RETCODE NetAppGetVersionInfo(
    NETAPP_VERSION_INFO     *pVersionInfo);


/**
 * @ingroup NetApp
 * @defgroup NetApp_Core Core
 * @brief A set of Core APIs are used to control and configure all interfaces.
 * @{
 */

/**
 * @brief Fetch Default Settings
 *
 * Fill the passed NETAPP_SETTINGS structure with the default settings
 *
 * @param[out] pSettings  - NetApp Settings structure
 *
 * @return NETAPP_RETCODE
 * @see NetAppClose
 */
NETAPP_RETCODE NetAppGetDefaultSettings(
    NETAPP_SETTINGS     *pSettings);


/**
 * @brief Fetch Default Initialization Settings
 *
 * Fill the passed NETAPP_INIT_SETTINGS structure with the default values.
 * @param[out] pSettings  - NetApp Settings structure
 *
 * @return NETAPP_RETCODE
 * @see NetAppClose
 */
NETAPP_RETCODE NetAppGetDefaultInitSettings(
    NETAPP_INIT_SETTINGS     *pSettings);


/**
 * @brief Open the NetApp API
 *
 * API is now multi entry and can be called by numerous applications passing
 * more than one callback. Each callback is registered in a dynamic linklist so
 * each callback will be called when a network event occurs. The first time
 * this function is called, we create the NetApp handle and opens a DGRAM socket
 * to get/set ip settings. If the application is run by NFS, the Wi-Fi Hotplug
 * handler is called to initialize BWL APIs (if compiled with BWL support).
 *
 * @remarks NetApp Settings structure can be NULL, if so the default settings
 * will be used or the existing settings from a previous API init.
 *
 * @remarks It is recommended to set the settings accordingly when opening
 * NetApp the first time. The main settings structure is saved the first time
 * NetApp is opened, upon subsequent calls to NetAppOpen only the callback
 * information is saved.
 *
 * @param[out] tHandle          Returned handle to the NetApp API
 * @param[in]  pOpenSettings    Open settings to set callbacks. Can be NULL
 * @param[in]  pInitSettings    Initialization Settings called on the first call to
 *                              this API. Must be NULL on subsequent calls to this API
 * @param[in]  pSettings        General configurable (on the fly) NetApp Settings.
 *                              Can be NULL as well.
 *
 * @return NETAPP_RETCODE
 * @see NetAppClose
 * @see NetAppSetSettings
 * @see NetAppGetSettings
 */
NETAPP_RETCODE NetAppOpen(
    NETAPP_HANDLE       *tHandle,
    NETAPP_OPEN_SETTINGS    *pOpenSettings,
    NETAPP_INIT_SETTINGS    *pInitSettings,
    NETAPP_SETTINGS         *pSettings);


/**
 * @brief Update NetApp settings NetApp with updated Settings
 *
 * There are some settings of NetApp (like WPS 2.0 support) that can be enabled/
 * disabled on the fly. This method allows you to change these settings.
 *
 * @param[in] tHandle    NetApp handle
 * @param[in] tSettings  NetApp settings structure
 *
 * @return NETAPP_RETCODE
 * @see NetAppOpen
 * @see NetAppGetSettings
 */
NETAPP_RETCODE NetAppSetSettings(
    NETAPP_HANDLE       tHandle,
    NETAPP_SETTINGS     tSettings);


/**
 * @brief Retrieve NetApp's current settings
 *
 * @param[in]  tHandle    NetApp handle
 * @param[out] pSettings  NetApp settings
 *
 * @return NETAPP_RETCODE
 * @see NetAppOpen
 * @see NetAppSetSettings
 */
NETAPP_RETCODE NetAppGetSettings(
    NETAPP_HANDLE       tHandle,
    NETAPP_SETTINGS     *pSettings);


/**
 * @brief Control the debug output from NetApp
 *
 * This function will enable or disable the debug output from NetApp
 * Two possible destinations (stdout and syslog) can be independently controlled.
 *
 * @param[in]  tHandle   NetApp handle
 * @param[in]  ulDest   Mask of NETAPP_DEBUG_DESTINATION values
 *
 * @return NETAPP_RETCODE
 */
NETAPP_RETCODE NetAppSetDebugOutput(
    NETAPP_HANDLE tHandle,
    uint32_t ulDest);


/**
 * @brief Close the NetApp API
 *
 * This function will close the NetApp API and unregister the callback.
 *
 * @param[in]  tHandle   NetApp handle
 *
 * @return NETAPP_RETCODE
 * @see NetAppOpen
 */
NETAPP_RETCODE NetAppClose(
    NETAPP_HANDLE   tHandle);


/**
 * @brief Change the network settings
 *
 * This function is responsible for applying the new network settings. Behavior is
 * different for each network IP mode discussed below. If the network is
 * configured in either NETAPP_IP_MODE_OFF or NETAPP_IP_MODE_STATIC we first
 * check to see if the dhcpcd daemon is running, if so we turn it off. Then
 * we use the IOCTL SIOCGIFFLAGS and SIOCSIFFLAGS to turn on or off the
 * interface depending again on the NETAPP_IP_MODE. <br>
 *
 * If the network is configured as static IP, the ip, netmask address are set
 * using IOCTLs SIOCSIFADDR and SIOCSIFNETMASK to the AF_PACKET interface. The
 * gateway address is configured by adding a default routes in the routing table
 * using again AF_PACKET and the IOCTL SIOCADDRT. DNS servers are configured
 * my reading and writing resolv.conf file in the root file system.
 *
 * The Linux resolver is also re-initialized each time the network settings are
 * applied.
 *
 * @param[in]  tHandle      NetApp handle
 * @param[in]  tIface       The interface
 * @param[in]  tMode        IP Mode
 * @param[in]  pSettings    Settings to apply. This parameter can be NULL for any
 *                          IP mode other than NETAPP_IP_MODE_STATIC.
 *
 * @return NETAPP_RETCODE
 * @see NetAppOpen NETAPP_SETTINGS
 */
NETAPP_RETCODE NetAppSetNetworkSettings(
    NETAPP_HANDLE       tHandle,
    NETAPP_IFACE        tIface,
    NETAPP_IP_MODE      tMode,
    NETAPP_IP_SETTINGS  *pSettings);


/**
 * @brief Set the MAC address
 *
 * Function to change the Hardware MAC address for the Specified interface.
 *
 * @param[in]  tHandle      NetApp handle
 * @param[in]  tIface       The interface
 * @param[in]  pMacAddress  New MAC Address
 *
 * @return NETAPP_RETCODE
 */
NETAPP_RETCODE NetAppSetMacAddress(
    NETAPP_HANDLE       tHandle,
    NETAPP_IFACE        tIface,
    char                *pMacAddress);


/**
 * @brief Fetch the current network settings
 *
 * This function retrieves the various network settings such as the MAC, IP, netmask,
 * gateway, and DNS addresses. The MAC, IP, and netmask addresses are fetched using IOCTLs
 * SIOCGIFHWADDR, SIOCGIFADDR, and SIOCGIFNETMASK respectively. The gateway
 * address is fetched by using the AF_NETLINK socket and sending the request
 * to fetch the routing tables by RTM_GETROUTE and then nlmsg_flags =
 * NLM_F_DUMP | NLM_F_REQUEST. In order to ensure that the get request does not
 * Interfere with the link change notification or other get requests (make this
 * function thread safe), a separate AF_NETLINK socket is used. Finally, the DNS
 * servers are read from the resolv.conf file.
 *
 * @param[in]  tHandle      NetApp handle
 * @param[in]  tIface       The interface
 * @param[out] pSettings    Settings structure to fill
 *
 * @return NETAPP_RETCODE
 * @see NetAppOpen NETAPP_SETTINGS
 */
NETAPP_RETCODE NetAppGetNetworkSettings(
    NETAPP_HANDLE       tHandle,
    NETAPP_IFACE        tIface,
    NETAPP_IP_SETTINGS  *pSettings);


/**
 * @brief Poll the link state from the kernel
 *
 * The function does not block and will poll the kernel for the current link state
 * for the passed interface. Generally the link state is disseminated to the
 * application through the callback mechanism but this API is added in case the
 * application wants to also poll the link state.
 *
 * @param[in]  tHandle      NetApp handle
 * @param[in]  tIface       The interface
 * @param[out] pLink        Current link state
 *
 * @return NETAPP_RETCODE
 */
NETAPP_RETCODE NetAppGetLinkState(
    NETAPP_HANDLE       tHandle,
    NETAPP_IFACE        tIface,
    NETAPP_LINK_STATE   *pLink);


/**
 * @brief Ping a network server
 *
 * Asynchronous ping request using a NetAppSystem call. The system call with call the
 * callback when either the pin returns successfully or times out.
 *
 * @remarks Use caution with the timeout value of NETAPP_WAIT_FOREVER since this
 * could result in this function blocking forever if the network is unreachable.
 *
 *
 * @param[in]  tHandle    NetApp handle
 * @param[in]  lTimeoutMs Time to wait for a response
 * @param[in]  pcAddress  Server name to ping
 *
 * @return NETAPP_RETCODE
 */
NETAPP_RETCODE NetAppPing(
    NETAPP_HANDLE   tHandle,
    int32_t          lTimeoutMs,
    const char      *pcAddress);


/**
 * @brief DNS Lookup
 *
 * This function will kick off a background DNS request to lookup an IP address for
 * the passed hostname. The results are fed back to the application in the form
 * of a NETAPP_CP_DNSLOOKUP since NetApp can only make one asynchronous DNSLookup
 * request at a time.
 *
 *
 * @param[in]  tHandle      NetApp handle
 * @param[in]  pcHostname   Server name to lookup
 *
 * @return NETAPP_RETCODE
 */
NETAPP_RETCODE NetAppDNSLookup(
    NETAPP_HANDLE       tHandle,
    const char          *pcHostname);


/**
 * @brief Set the Date/Time using NTPDate
 *
 * The function will kick off a background the NTPDate request reading the server
 * list from /etc/ntp/step-tickers
 *
 * When the background request is finished, the callback NETAPP_CB_NTPDATE is
 * called passing the results if the NtpSetDate request was not canceled
 *
 * @remarks Calling this API while another NtpSetDate request is in process will
 * result in the first request being canceled and the second (new) request is made
 *
 * @param[in]  tHandle      NetApp handler
 * @param[in]  ulPeriodMs   How often we want to update the date and time in the
 *                          background in milliseconds. 0 means only update once
 *
 * @return NETAPP_RETCODE
 */
NETAPP_RETCODE NetAppNtpSetDate(
    NETAPP_HANDLE       tHandle,
    uint32_t            ulPeriodMs);


/**
 * @brief Set Interface Up or Down
 *
 * NetApp automatically controls the interface state (up or down) for you so under
 * normal circumstances you do not need to call this API. <br>
 * NetApp wired link monitoring uses the Linux NetLink API to detect when the link
 * goes up or down and in order to continue to receive these events from the Kernel
 * when the cable is removed, we need to keep the interface up. This API can allow
 * the application to completely disable the interface.
 *
 * @remarks This API is not needed under normal circumstances and using it will
 * disable NETLINK interface monitoring. USE CAUTION WHEN USING THIS API.
 *
 * @remarks The API NetAppSetNetworkSettings() MUST be called with
 * NETAPP_IP_MODE_OFF before calling this API
 *
 * @param[in]  tHandle      NetApp handle
 * @param[in]  tIface       Interface to bring up/down
 * @param[in]  bUp          TRUE for up, false for DOWN
 *
 * @return NETAPP_RETCODE
 */
NETAPP_RETCODE NetAppSetIfaceUp(
    NETAPP_HANDLE       tHandle,
    NETAPP_IFACE        tIface,
    bool                bUp);


/**
 * @brief Get Interface Name
 *
 * Return a strdup string for the interface name of the passed NETAPP_IFACE enum.
 *
 * @remarks This API will return NETAPP_NOT_SUPPORTED if the interface support
 * was not compiled in.
 * @remarks YOU MUST FREE THE STRING RETURNED in pString.
 *
 * @param[in]  tHandle      NetApp handle
 * @param[in]  tIface       Interface to fetch the name
 * @param[out] pString      Reference of a pointer to an Interface name that YOU
 *                          MUST FREE THIS STRING WHEN YOU ARE FINISHED
 *
 * @return NETAPP_RETCODE
 */
NETAPP_RETCODE NetAppGetIfaceName(
    NETAPP_HANDLE       tHandle,
    NETAPP_IFACE        tIface,
    char                **pString);


/**
 * @brief Get Default Interface
 *
 * API will parse the routing table and determine what is the current default
 * route which in essence can refer to the default interface (where all nonlocalized
 * packets are sent).
 *
 * @param[in]  tHandle      NetApp handle
 * @param[out] pIface       Reference to a NETAPP_IFACE that will be set with the
 *                          default interface.
 *
 * @return NETAPP_RETCODE
 */
NETAPP_RETCODE NetAppGetDefaultIface(
    NETAPP_HANDLE       tHandle,
    NETAPP_IFACE        *pIface);


/**
 * @brief Convert network notation to string
 *
 * Function shall convert the Internet host address specified by ulAddress to
 * a string in the Internet standard dot notation. This function wraps the
 * IPv4 address manipulation function inet_ntoa().
 *
 * @param[in]  ulAddress   uint32_t representation of IPV4 address
 *
 * @return pointer to the Numbers-and-dots notation of IPV4 address
 */
char* NetAppNtoA(
    NETAPP_IPV4_ADDR    ulAddress);


/**
 * @brief Convert string to internet notation
 *
 * Function converts the Internet host address pcString from the standard
 * numbers-and-dots notation into binary data.
 * This function wraps the IPv4 address manipulation function inet_aton().
 *
 * @param[in]  pcString    Numbers-and-dots notation of IPV4 address
 *
 * @return NETAPP_IPV4_ADDR
 */
NETAPP_IPV4_ADDR NetAppAtoN(
    char                *pcString);


/**
 * @brief Convert a hardware address to a string.
 *
 * This function converts the standard hardware address (e.g., BSSID, MAC address,
 * etc.) to a string.
 *
 * @remarks You must pass a buffer to contain at least @see NETAPP_ENET_LEN +1 bytes
 *
 * @param[in]  tHwAddr      NETAPP_HW_ADDR to convert
 * @param[in]  pcString     Pointer to a buffer to store the string
 * @param[in]  ulLength     Length of the buffer pcString
 *
 * @return String representation of the hardware address.
 */
char* NetAppHwAddrToA(
    NETAPP_HW_ADDR      tHwAddr,
    char                *pcString,
    uint32_t            ulLength);


/**
 * @brief Convert string to hardware address
 *
 * Function converts the String hardware address xx:xx:xx:xx:xx:xx into binary data.
 *
 * @param[in]  pcString   Numbers-and-dots notation of IPv4 address
 * @param[out] tHwAddr    Hardware Address
 *
 * @return NETAPP_RETCODE
 */
NETAPP_RETCODE NetAppAToHwAddr(
    char                *pcString,
    NETAPP_HW_ADDR      tHwAddr);


/**
 * @brief Asynchronous voice recognition search using NETAPP_BT_HID_VOICE_INFO
 *
 * Function takes in NETAPP_BT_HID_VOICE_INFO and first converts the PCM to FLAC
 * to then perform a Google voice recognition query to convert the voice to a
 * string. the result is passed in the callback NETAPP_CB_VOICE_REC_DONE
 *
 * @remarks The passed pString MUST be freed after it is finished being used
 *
 * @param[in]  pHidVoiceInfo    Bluetooth HID Voice info structure
 * @param[in]  pLanguage        Langauge string represented in ISO 639-1 Code
 *                     @see http://en.wikipedia.org/wiki/List_of_ISO_639-1_codes
 *
 * @return NETAPP_RETCODE
 */
NETAPP_RETCODE NetAppHttpVoiceSearch(
    NETAPP_BT_HID_VOICE_INFO *pHidVoiceInfo,
    const char               *pLanguage);


/**
 * @brief Return the systems interface information
 *
 * @param[in]  tHandle      NetApp handle
 * @param[in]  pIfaceInfo   Pointer to contain the list of interface information
 * @param[out] pListLength  Length of the list (should always be NETAPP_IFACE_MAX)
 *
 * @remarks The caller of this API must free the returned pointer list
 *
 * @return NETAPP_RETCODE
 */
NETAPP_RETCODE NetAppGetIfaceInfo(
    NETAPP_HANDLE       tHandle,
    NETAPP_IFACE_INFO   **pIfaceInfo,
    uint32_t            *pListLength);

/// @}


/**
 * @ingroup NetApp
 * @defgroup NetApp_Wi-Fi Wi-Fi API
 * @brief This includes APIs used to control and configure the wireless interface.
 * @{
 */

/**
 * @brief Scan for Wireless Networks
 *
 * This API will start the Wi-Fi network scan that will return results every ulTickMs.
 *
 * @param[in]  tHandle     NetApp handle
 * @param[in]  lTickMs     How often the scan and results should be done and sent
 *                         to the application.
 * @param[in]  lScanTimeMs How long to spent scanning each channel in ms.
 *
 * @remarks Default ScanTimeMs is 40 when connected to an AP and 80 when not
 * connected to an AP. It is recommended that you choose a value greater than 100 ms
 * to pick up more APs, however, note that the larger the number, the longer
 * a scan will take.
 *
 * @remarks Setting lTickMs to 0 will result in only one scan
 *
 * @return NETAPP_RETCODE
 */
NETAPP_RETCODE NetAppWiFiStartScan(
    NETAPP_HANDLE       tHandle,
    int32_t             lTickMs,
    int32_t             lScanTimeMs);


/**
 * @brief Stop a Wireless Scan
 *
 * @param[in]  tHandle    NetApp handle
 *
 * @return NETAPP_RETCODE
 */
NETAPP_RETCODE NetAppWiFiStopScan(
    NETAPP_HANDLE       tHandle);


/**
 * @brief Get Wi-Fi Scan Results
 *
 * After the interface has notified the application that scan results are
 * available, the application can call this function to fetch the scan results.
 * This function will create a dynamic array of NETAPP_WIFI_AP_INFO structures
 * one for each detected AP.
 *
 * @remarks CALLERS OF THIS FUNCTION MUST FREE THE POINTER pApInfoList
 *
 * @param[in]  tHandle      NetApp handle
 * @param[out] pApInfoList  Pointer to the list of APs scanned.
 * @param[out] pulScanCount The number of scanned APs.
 *
 * @return NETAPP_RETCODE
 */
NETAPP_RETCODE NetAppWiFiGetScanResults(
    NETAPP_HANDLE           tHandle,
    NETAPP_WIFI_AP_INFO     **pApInfoList,
    uint32_t                *pulScanCount);


/**
 * @brief Wi-Fi Protected Setup Push Button
 *
 * The auto configuration will take place in the background with the router and
 * the result of the auto configuration will be sent to the application through
 * the registered callback function tCallback.
 *
 * @remarks WPS will run with the WPS mode specified from the NETAPP_SETTINGS
 * structure passed to NetAppSetSettings or the first call to NetAppOpen. To change
 * to change WPS mode (2.0 vs 1.0), call NetAppSetSettings again.
 *
 * @param[in]  tHandle  NetApp handle
 *
 * @return NETAPP_RETCODE
 */
NETAPP_RETCODE NetAppWiFiConnectByPb(
    NETAPP_HANDLE         tHandle);


/**
 * @brief Start an Wi-Fi Auto Setup Configuration using a Pin.
 *
 * The auto configuration will take place in the background with the router and
 * the result of the auto configuration will be sent to the application through
 * the registered callback function tCallback.
 *
 * @remarks bEnrollee(true) implements section "5.1 Add to AP as an Enrollee"
 * (more precisely 5.1.1, 5.1.3, 5.1.4, and 5.1.5) of the WPS Test Plan ver 1.10.
 * bEnrollee(false) implements "5.2 Act as Registrar and Configure AP" also from
 * the WPS Test Plan ver 1.10.
 *
 * @remarks It is no longer necessary to specify the SSID of the router to
 * perform WPS Pin with (as an Enrollee only) as NetApp will automatically scan
 * for APs that have opened up a WPS window if the SSID is not specified. If
 * you do not specify an SSID you MUST start WPS on the AP first before calling
 * this API.
 *
 * @remarks WPS will run with the WPS mode specified from the NETAPP_SETTINGS
 * structure passed to NetAppSetSettings or the first call to NetAppOpen. Please
 * call NetAppSetSettings again to change WPS mode (2.0 vs 1.0)
 *
 * @param[in]  tHandle      NetApp handle
 * @param[in]  pSsid        The name of the router connect too (or null to do a
 *                          search for APs).
 * @param[in]  ulPin        Autoconfiguration pin code
 * @param [in] bEnrollee    This device will be the enrollee; otherwise, NetApp is
 *                          the registrar.
 *
 * @return NETAPP_RETCODE
 * @return NETAPP_INVALID_PIN if the ulPin is not a valid WPS pin
 */
NETAPP_RETCODE NetAppWiFiConnectByPin(
    NETAPP_HANDLE         tHandle,
    char                  *pSsid,
    uint32_t               ulPin,
    bool                  bEnrollee);


/**
 * @brief Generate a WPS Pin
 *
 * API will generate a WPS PIN that meets section 6.4.1 of the
 * "Wi-Fi Protected Setup Specifications 1.0h".
 *
 * @param[out] pulPin      The autogenerated WPS pin.
 *
 * @return NETAPP_RETCODE
 */
NETAPP_RETCODE NetAppWiFiGenerateWPSPin(
    uint32_t            *pulPin);


/**
 * @brief Connect to a particular Access point
 * @param[in]   tHandle     NetApp handle
 * @param[in]   pApInfo     The access point information structure.
 *
 * @remarks All that is needed to connect to an AP is the SSID and a password if the
 * security settings is set to NETAPP_WIFI_SECURITY_AUTO_DETECT. In this scenario
 * you must clear the BSSID since BSSID can also be used for connecting.
 *
 * @return NETAPP_RETCODE
 * @see NETAPP_WIFI_AP_INFO
 */
NETAPP_RETCODE NetAppWiFiConnect(
    NETAPP_HANDLE           tHandle,
    NETAPP_WIFI_AP_INFO     *pApInfo);


/**
 * @brief Wi-Fi Disconnect.
 *
 * Disconnect/disassociate from the current connected access point (if any) and
 * stops any ongoing connection attempt.
 *
 * @param[in]       tHandle      NetApp handle
 *
 * @return NETAPP_RETCODE
 */
NETAPP_RETCODE NetAppWiFiDisconnect(
    NETAPP_HANDLE           tHandle);


/**
 * @brief Return current Connected Access Point.
 *
 * Function will return the full NETAPP_WIFI_AP_INFO structure for the current
 * connected AP (if we are connected)
 *
 * @param[in]  tHandle      NetApp handle
 * @param[out] pApInfo      the AP Info structure filled for the connected AP
 *
 * @return NETAPP_RETCODE
 *
 */
NETAPP_RETCODE NetAppWiFiGetConnectedApInfo(
    NETAPP_HANDLE       tHandle,
    NETAPP_WIFI_AP_INFO *pApInfo);


/**
 * @brief Return Access Point Info for a Scanned AP.
 *
 * This function will return the full NETAPP_WIFI_AP_INFO structure for a scanned AP
 * where the name is set in cSSID of APInfo structure.
 *
 * @param[in]  tHandle      NetApp handle
 * @param[out] pApInfo      the AP Info structure filled for the connected AP
 *
 * @return NETAPP_RETCODE
 *
 */
NETAPP_RETCODE NetAppWiFiGetScannedApInfo(
    NETAPP_HANDLE       tHandle,
    NETAPP_WIFI_AP_INFO *pApInfo);


/**
 * @brief Check if Wi-Fi is connected or not
 *
 * @param[in]  tHandle      NetApp handle
 * @param[out] pIsConnected True if connected, otherwise false
 *
 * @return NETAPP_RETCODE
 */
NETAPP_RETCODE NetAppWiFiIsConnected(
    NETAPP_HANDLE       tHandle,
    bool                *pIsConnected);


/**
 * NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE
 *
 * THIS API IS BEING DEPRECIATED AND REPLACED BY NetAppIsEnabled()
 * @brief Check if Wi-Fi is enabled
 *
 * @param[in]  tHandle      NetApp handle
 * @param[out] pIsEnabled   True if enabled, otherwise false
 *
 * @return NETAPP_RETCODE
 */
NETAPP_RETCODE NetAppWiFiIsEnabled(
    NETAPP_HANDLE       tHandle,
    bool                *pIsEnabled);


/**
 * @brief Fetch NETAPP_WIFI_AP_INFO for a particular SSID
 *
 * This asynchronous API will fetch in the background the full AP info (minus the
 * password) of an access point with the SSID pSSID. The applications of this is
 * to fetch all the credentials (minus the password) of a hidden access point to
 * be able to do a manual configuration to that hidden AP without having to prompt
 * the user for the security type.
 *
 * When NetApp has finished attempting to fetch the NETAPP_WIFI_AP_INFO for the AP,
 * the status notification @see NETAPP_CB_FETCHED_APINFO is called passing the
 * NETAPP_WIFI_AP_INFO structure and the NETAPP_RETCODE that can be either: <br>
 * NETAPP_SUCCESS: Successfully fetched the AP info. <br>
 * NETAPP_FAILURE: Failure in NetApp to fetch the AP Info. <br>
 * NETAPP_TIMEOUT: Timed out trying to connect to the AP
 *
 * @remarks If the NETAPP_TIMEOUT return code is sent to the app with the
 * NETAPP_CB_FETCHED_APINFO then it is possible that the SSID is misspelled or
 * the AP is not found.
 *
 * @remarks Calling this function will disconnect the interface from a current
 * connected access point (if any) and stop any active scan.
 *
 * @param[in]  tHandle      NetApp handle
 * @param[in]  pSSID        Null terminated SSID of the access point
 *
 * @return NETAPP_RETCODE
 */
NETAPP_RETCODE NetAppWiFiGetApInfo(
    NETAPP_HANDLE           tHandle,
    char                    *pSSID);


/**
 * @brief Join Wi-Fi interface with a bridge
 *
 * Hooks up the interface with a bridge such that all event notifications are
 * properly fed back to NetApp
 *
 * @param[in]       tHandle      NetApp handle
 * @param[in]       pBridgeName  Name of the bridge
 *
 * @return NETAPP_RETCODE
 */
NETAPP_RETCODE NetAppWiFiJoinBridge(
    NETAPP_HANDLE           tHandle,
    const char              *pBridgeName);


/**
 * @brief Leave Wi-Fi interface from the bridge
 *
 * @param[in]       tHandle      NetApp handle
 *
 * @return NETAPP_RETCODE
 */
NETAPP_RETCODE NetAppWiFiLeaveBridge(
    NETAPP_HANDLE           tHandle);


/**
 * @brief Selects antenna(s) to be activated
 *
 * Please note that this API is typically only used in 'controlled' test environments,
 * and is not expected to be useful in a general use-case. If you exercise this API,
 * we expect you to know what you are doing - please check with the support team
 * if you have any questions.
 *
 * @param[in]       tHandle      NetApp handle
 * @param[in]       tMode      mode of operation for antenna(s) being selected
 * @param[in]       ulAntennas  A bitmask of NETAPP_WIFI_ANTENNA values
 *
 * @return NETAPP_RETCODE
 */
NETAPP_RETCODE NetAppWiFiSetAntenna(
    NETAPP_HANDLE tHandle,
    NETAPP_WIFI_ANTENNA_MODE tMode,
    uint32_t ulAntennas);


/**
 * @brief Selects antenna(s) to be activated
 *
 * Please note that this API is typically only used in 'controlled' test environments,
 * and is not expected to be useful in a general use-case. If you exercise this API,
 * we expect you to know what you are doing - please check with the support team
 * if you have any questions.
 *
 * @param[in]       tHandle      NetApp handle
 * @param[in]       tMode      mode of operation for antenna(s) being queried
 * @param[out]       pulAntennas  A bitmask of NETAPP_WIFI_ANTENNA values
 *
 * @return NETAPP_RETCODE
 */
NETAPP_RETCODE NetAppWiFiGetAntenna(
    NETAPP_HANDLE tHandle,
    NETAPP_WIFI_ANTENNA_MODE tMode,
    uint32_t *pulAntennas);


/**
 * @brief Returns the number of antenna(s) available
 *
 * Please note that this API is typically only used in 'controlled' test environments,
 * and is not expected to be useful in a general use-case. If you exercise this API,
 * we expect you to know what you are doing - please check with the support team
 * if you have any questions.
 *
 * @param[in]       tHandle      NetApp handle
 * @param[out]       pulAntennas  Number of antennas found
 *
 * @return NETAPP_RETCODE
 */
NETAPP_RETCODE NetAppWiFiGetAntennaCount(
    NETAPP_HANDLE tHandle,
    uint32_t *pulAntennas);


/**
 * @brief Starts transmission of data
 *
 * Please note that this API is typically only used in 'controlled' test environments,
 * and is not expected to be useful in a general use-case. If you exercise this API,
 * we expect you to know what you are doing - please check with the support team
 * if you have any questions.
 *
 * @param[in]       tHandle      NetApp handle
 * @param[in]       pTxSettings  Parameters for transmission
 *
 * @return NETAPP_RETCODE
 */
NETAPP_RETCODE NetAppWiFiTestStartTransmission(
    NETAPP_HANDLE tHandle,
    NETAPP_WIFI_TXRX_SETTINGS *pTxSettings);


/**
 * @brief Stops transmission of data
 *
 * Please note that this API is typically only used in 'controlled' test environments,
 * and is not expected to be useful in a general use-case. If you exercise this API,
 * we expect you to know what you are doing - please check with the support team
 * if you have any questions.
 *
 * @param[in]       tHandle      NetApp handle
 *
 * @return NETAPP_RETCODE
 */
NETAPP_RETCODE NetAppWiFiTestStopTransmission(
    NETAPP_HANDLE tHandle);


/**
 * @brief Starts receiving data
 *
 * Please note that this API is typically only used in 'controlled' test environments,
 * and is not expected to be useful in a general use-case. If you exercise this API,
 * we expect you to know what you are doing - please check with the support team
 * if you have any questions.
 *
 * @param[in]       tHandle      NetApp handle
 * @param[in]       pRxSettings  Parameters for receiving
 *
 * @return NETAPP_RETCODE
 */
NETAPP_RETCODE NetAppWiFiTestStartReceiving(
    NETAPP_HANDLE tHandle,
    NETAPP_WIFI_TXRX_SETTINGS *pRxSettings);


/**
 * @brief Stops receiving data
 *
 * Please note that this API is typically only used in 'controlled' test environments,
 * and is not expected to be useful in a general use-case. If you exercise this API,
 * we expect you to know what you are doing - please check with the support team
 * if you have any questions.
 *
 * @param[in]       tHandle      NetApp handle
 *
 * @return NETAPP_RETCODE
 */
NETAPP_RETCODE NetAppWiFiTestStopReceiving(
    NETAPP_HANDLE tHandle);


/**
 * @brief Retrieves RSSI information for each antenna
 *
 * Please note that this API is typically only used in 'controlled' test environments,
 * and is not expected to be useful in a general use-case. If you exercise this API,
 * we expect you to know what you are doing - please check with the support team
 * if you have any questions.
 *
 * @param[in]       tHandle      NetApp handle
 * @param[out]       pRssiPerAntenna  Structure will be populated with antenna specific RSSI
 *
 * @return NETAPP_RETCODE
 */
 NETAPP_RETCODE NetAppWiFiTestGetRssiPerAntenna(
    NETAPP_HANDLE tHandle,
    NETAPP_WIFI_RSSI_PER_ANTENNA *pRssiPerAntenna);


/**
 * @brief Enables or disables continuous waveform mode
 *
 * Please note that this API is typically only used in 'controlled' test environments,
 * and is not expected to be useful in a general use-case. If you exercise this API,
 * we expect you to know what you are doing - please check with the support team
 * if you have any questions.
 *
 * @param[in]       tHandle      NetApp handle
 * @param[in]       ulChannel  Channel within which to test the frequency accuracy
 * @param[in]       bEnable  Controls whether to start or stop the test
 *
 * @return NETAPP_RETCODE
 */
 NETAPP_RETCODE NetAppWiFiTestControlWaveform(
    NETAPP_HANDLE tHandle,
    uint32_t ulChannel,
    bool bEnable);


/**
 * @ingroup NetApp_WIFI
 * @defgroup NetApp_WFI Wi-Fi Invite
 * @brief APIs to support Wi-Fi Invite Feature
 * @{
 */

/**
 * @brief Start Wi-Fi Invite
 *
 * Start the Wi-Fi Invite feature which will kick off a prob request to notify all
 * Wi-Fi enabled routers that the client device is Wi-Fi Invite capable.
 *
 * @remarks This API should only be called when the AP is not connected and where
 * the application is not trying to reconnect to an AP.
 *
 * @param[in]  tHandle    NetApp handle
 *
 * @return NETAPP_RETCODE Will return a failure if the AP is currently connected
 * to an access point.
 */
NETAPP_RETCODE NetAppWiFiInviteStart(
    NETAPP_HANDLE           tHandle);


/**
 * @brief Stop Wi-Fi Invite
 *
 * @param[in]  tHandle    NetApp handle
 *
 * @return NETAPP_RETCODE
 */
NETAPP_RETCODE NetAppWiFiInviteStop(
    NETAPP_HANDLE           tHandle);


/**
 * @brief Accept a Wi-Fi Invite Request
 *
 * Kick off an asynchronous Wi-Fi Invite accept request that will fetch the access
 * point credentials using WPS and once the credentials are obtained NetApp will
 * connect to the access point and call the NETAPP_CB_CONNECT callback when finished.
 *
 * @remarks Calling this function will free the invite context that NetApp was
 * saving for this AP
 *
 * @remarks WPS will run with the WPS mode specified from the NETAPP_SETTINGS
 * structure passed to NetAppSetSettings or the first call to NetAppOpen.
 * To change WPS mode (2.0 vs 1.0), call NetAppSetSettings again.
 *
 *
 * @param[in]  tHandle      NetApp handle
 * @param[in]  pBSSID       The BSSID of the AP we want to accept or reject.
 *
 * @return NETAPP_RETCODE
 * @see NetAppWiFiInviteReject
 */
NETAPP_RETCODE NetAppWiFiInviteAccept(
    NETAPP_HANDLE           tHandle,
    char                    *pBSSID);


/**
 * @brief Reject a Wi-Fi Invite Request
 *
 * Send an asynchronous Wi-Fi Invite reject notification to the access point so that
 * we no longer receive invites from this AP.
 *
 * @remarks Calling this function will free the invite context that NetApp was
 * saving for this AP.
 *
 * @remarks WPS will run with the WPS mode specified from the NETAPP_SETTINGS
 * structure passed to NetAppSetSettings or the first call to NetAppOpen. Please
 * call NetAppSetSettings again to change WPS mode (2.0 vs 1.0).
 *
 * @param[in]  tHandle      NetApp handle
 * @param[in]  pBSSID       The BSSID of the access point to reject
 *
 * @return NETAPP_RETCODE
 * @see NetAppWiFiInviteAccept
 */
NETAPP_RETCODE NetAppWiFiInviteReject(
    NETAPP_HANDLE           tHandle,
    char                    *pBSSID);
/// @} NetApp_WFI


/**
 * @ingroup NetApp_WIFI
 * @defgroup NetApp_P2P Wi-Fi Direct
 * @brief This section describes Broadcom's P2P implementation of Wi-Fi Direct.
 * @{
 */


/**
 * @brief Start Wi-Fi Direct Discovery
 *
 * This API will start a Wi-Fi Direct discovery to find all P2P capable devices.
 * As part of the discovery, the device will be put in the Listen Mode, scan and find
 * phases as defined in Wi-Fi Peer-to-Peer (P2P) Technical Specification Draft
 * Version 1.15.
 *
 *
 *
 * @param[in]  tHandle      NetApp handle
 *
 * @return NETAPP_RETCODE
 * @see NetAppWiFI P2PStopDiscovery()
 */
NETAPP_RETCODE NetAppWiFiP2PDiscover(
    NETAPP_HANDLE               tHandle);


/**
 * @brief Stop Wi-Fi Direct Discovery
 *
 * Stop Wi-Fi Direct Discovery and automatically re-start Wi-Fi Invite if we are
 * not associated.
 *
 * @param[in]  tHandle      NetApp handle
 *
 * @return NETAPP_RETCODE
 * @see NetAppWiFiP2PDiscover()
 */
NETAPP_RETCODE NetAppWiFiP2PStopDiscovery(
    NETAPP_HANDLE               tHandle);


/**
 * @brief Start Wi-Fi Direct Connection
 * @param[in]   tHandle         NetApp handle
 * @param[in]   pName           the P2P Device Name
 * @param[in]   ulTimeoutSec    How long to wait for P2P connect before we timeout
 *
 * @return NETAPP_RETCODE
 */
NETAPP_RETCODE NetAppWiFiP2PConnect(
    NETAPP_HANDLE           tHandle,
    char                    *pName,
    uint32_t                ulTimeoutSec);


/**
 * @brief Stop P2P Connection attempt and disconnect
 * @param[in]   tHandle     NetApp handle
 *
 * @return NETAPP_RETCODE
 */
NETAPP_RETCODE NetAppWiFiP2PDisconnect(
    NETAPP_HANDLE           tHandle);


/**
 * @brief Retrieve the group owner SSID
 * @param[in]   tHandle     NetApp handle
 * @param[out]  pBuf        Pointer to a string buffer to store the SSID
 * @param[in]   ulBufSize   Buffer size
 *
 * @return NETAPP_RETCODE
 */
NETAPP_RETCODE NetAppWiFiP2PGetSSID(
    NETAPP_HANDLE           tHandle,
    char                    *pBuf,
    uint32_t                ulBufSize);


/**
 * @brief Push the (virtual) WPS pushbutton and starts WPS
 * @param[in]   tHandle     NetApp handle
 *
 * @return NETAPP_RETCODE
 */
NETAPP_RETCODE NetAppWiFiP2PWpsPushButton(
    NETAPP_HANDLE           tHandle);

/// @} NetApp_P2P

/// @} NetApp_Wi-Fi


/**
 * @ingroup NetApp_API
 * @defgroup NetApp_Zeroconf Zeroconf (Bonjour)
 * @brief Zero Configuration (Bonjour) library to support Multicast-DNS and
 * DNS-Service Discovery.
 * @{
 */


/**
 * @brief Publish a Service using Bonjour/Zeroconf
 *
 * Using DNS Service Discovery portion of Zero Configuration Networking, this
 * API will publish a service that the device will support.
 *
 * NetApp will add in the following TXT records automatically taken from the
 * NETAPP_SETTINGS structure passed to NetAppOpen(): <br>
 * <ul>
 * <li>manufacturer=pManufacturer</li>
 * <li>model_name=pModelName</li>
 * <li>model_number=pModelNumber</li>
 * <li>serial_number=pSerialNumber</li>
 * </ul>
 *
 * @remarks the value of NETAP_SETTINGS.pDeviceName is used for the HostName where
 * any spaces are converted to underscores.
 *
 * @param[in]  tHandle      NetApp handle
 * @param[in]  pName        The Service Name
 * @param[in]  pType        The Service Type (e.g., _http._tcp)
 * @param[in]  ulPort       The Port for the service
 * @param[in]  pTxtRecord   Pointer to a buffer containing the TXT record (if any)
 * @param[in]  ulTxtLength  Length of the TXT record
 *
 * @return NETAPP_RETCODE
 */
NETAPP_RETCODE NetAppZeroConfPublish(
    NETAPP_HANDLE           tHandle,
    char                    *pName,
    char                    *pType,
    uint32_t                ulPort,
    char                    *pTxtRecord,
    uint32_t                ulTxtLength);


/**
 * @brief Browse for Zeroconfig/Bonjour services
 *
 * Initiate a single service discovery. When a service is found, NetApp will
 * call the callback NETAPP_CB_FOUND_SERVICE and the application should then
 * call NetAppZeroConfGetServices() to return a list of discovered services and
 * the TXT record for each discovered service
 *
 * @remarks Currently we can only browse for one service at a time, this can
 * change when there are more services supported.
 *
 * @param[in]  tHandle      NetApp handle
 * @param[in]  pType        Service type to browse for (e.g., _http._tcp)
 *
 * @return NETAPP_RETCODE
 * @see NetAppZeroConfGetBrowseResults
 */
NETAPP_RETCODE NetAppZeroConfBrowse(
    NETAPP_HANDLE           tHandle,
    char                    *pType);


/**
 * @brief Get Browsed service results
 *
 * Fetch a reference to the NETAPP_ZEROCONF_SERVICE_INFO for the passed service
 * name. The reference to pInfo will be available until the service is removed
 * at which point NetApp will free the handle.
 *
 * @remarks DO NOT free the pInfo structure, garbage collection of the service
 * information is taken care of inside of NetApp. The reference to
 * NETAPP_ZEROCONF_SERVICE_INFO will be available for the whole life of the service
 * until when the service is removed. NetApp will free the NETAPP_ZEROCONF_SERVICE_INFO
 * after the callback NETAPP_CB_ZEROCONF_SERVICE is called.
 *
 * @param[in]  tHandle      NetApp handle
 * @param[in]  pName        Service name that we want to fetch the info for
 * @param[out] pInfo        Pointer to the SERVICE_INFO structure that is cached
 *                          inside of NetApp
 * @return NETAPP_RETCODE
 */
NETAPP_RETCODE NetAppZeroConfGetBrowseResults(
    NETAPP_HANDLE           tHandle,
    char                    *pName,
    NETAPP_ZEROCONF_SERVICE_INFO *pInfo);

/// @} NetApp_Zeroconf


/**
 * @ingroup NetApp_API
 * @defgroup NetApp_Bluetooth Bluetooth
 * @brief Bluetooth library to support various Bluetooth profiles (HID, AV, etc.)
 * @{
 */

/**
 * @brief Bluetooth Asynchronous Discovery
 *
 * Kick off a background discovery request to find Bluetooth devices by a particular
 * service type or all services. Once a device is found the callback
 * NETAPP_CB_BT_DISCOVERY_RESULTS is called with the Bluetooth discovery is completed.
 *
 * @param[in]  tHandle      NetApp handle
 * @param[in]  tServices    Service type to search for or all services
 *                          @see NETAPP_BT_SERVICE_TYPE
 * @return NETAPP_RETCODE
 */
NETAPP_RETCODE NetAppBluetoothDiscovery(
    NETAPP_HANDLE           tHandle,
    uint32_t                tServices);


/**
 * @brief Get Discovery Results
 *
 * Return a pointer to a newly allocated array of discovered Bluetooth devices.
 *
 * @remarks USER MUST FREE THE RETURNED ARRAY ONCE YOU ARE FINISH WITH IT!
 *
 * @param[in]   tHandle     NetApp handle,
 * @param[out]  pBtDevInfo  Pointer to an array of discovered Bluetooth devices.
 * @param[out]  pulCount    The number of discovered devices.
 * @return NETAPP_RETCODE
 */
NETAPP_RETCODE NetAppBluetoothGetDiscoveryResults(
    NETAPP_HANDLE           tHandle,
    NETAPP_BT_DEV_INFO      **pBtDevInfo,
    uint32_t                *pulCount);


/**
 * @brief Connect to a Bluetooth Device
 *
 * Initiate a pairing session with a discovered Bluetooth device. This API will
 * do the correct pairing/bonding process depending on the service type the
 * Bluetooth device is.
 *
 * @param[in]   tHandle     NetApp handle.
 * @param[in]   pBtDevInfo  Pointer to an array of discovered Bluetooth devices.
 *
 * @return NETAPP_RETCODE
 */
NETAPP_RETCODE NetAppBluetoothConnect(
    NETAPP_HANDLE           tHandle,
    NETAPP_BT_DEV_INFO      *pBtDevInfo);


/**
 * @brief Disconnect the Bluetooth Device
 *
 * @param[in]   tHandle     NetApp handle.
 * @param[in]   pBtDevInfo  Pointer to an array of discovered Bluetooth devices.
 *
 * @return NETAPP_RETCODE
 */
NETAPP_RETCODE NetAppBluetoothDisconnect(
    NETAPP_HANDLE           tHandle,
    NETAPP_BT_DEV_INFO      *pBtDevInfo);

/**
 * @brief Get Bluetooth Device List
 *
 * @param[in]   tHandle     NetApp handle.
 *
 * @return NETAPP_RETCODE
 */
NETAPP_RETCODE NetAppBluetoothGetDevList(
    NETAPP_HANDLE           tHandle,
    NETAPP_BT_DEV_INFO      **pBtDevInfoList,
    uint32_t                *ulCount);


/**
 * @brief Send Audio Buffer to A2DP stream
 *
 * @param[in]   tHandle     NetApp handle
 * @param[in]   pBuf        Pointer to buffer containing audio data
 * @param[in]   ulLength    Number of bytes sent
 *
 * @return NETAPP_RETCODE
 */
NETAPP_RETCODE NetAppBluetoothSendAudioBuffer(
    NETAPP_HANDLE           tHandle,
    void                    *pBuf,
    uint32_t                ulLength);


/**
 * @brief Start AV (Audio Source) streaming to Bluetooth headset
 *
 * @param[in]   tHandle         NetApp handle
 * @param[in]   bSynchronous    We will be feeding AV synchronously (ex. Nexus audio capture) or
 *                              asynchronously (readying from a file).
 * @param[in]   pBtAudioFormat  Pointer to BT audio format parameters
 *
 * @return NETAPP_RETCODE
 */
NETAPP_RETCODE NetAppBluetoothAvStart(
    NETAPP_HANDLE           tHandle,
    bool                    bSynchronous,
    NETAPP_BT_AUDIO_FORMAT  *pBtAudioFormat);


/**
 * @brief Stop AV (Audio Source) streaming to Bluetooth headest
 *
 * @param[in]   tHandle     NetApp handle
 *
 * @return NETAPP_RETCODE
 */
NETAPP_RETCODE NetAppBluetoothAvStop(
    NETAPP_HANDLE           tHandle);



/**
 * @brief Start AVK (Audio Sink) streaming from a Bluetooth device
 *
 * @param[in]   tHandle         NetApp handle
 * @param[in]   pBtAudioFormat  Pointer to BT audio format parameters
 *
 * @return NETAPP_RETCODE
 */
NETAPP_RETCODE NetAppBluetoothAvkStart(
    NETAPP_HANDLE           tHandle,
    NETAPP_BT_AUDIO_FORMAT  *pBtAudioFormat);


/**
 * @brief Stop AVK (Audio Sink) streaming from a Bluetooth device
 *
 * @param[in]   tHandle     NetApp handle
 *
 * @return NETAPP_RETCODE
 */
NETAPP_RETCODE NetAppBluetoothAvkStop(
    NETAPP_HANDLE           tHandle);

/**
 * @brief Accept or Reject a Simple Pairing Request
 *
 * This API should be called after receiving the NETAPP_CB_BT_SIMPLE_PAIRING
 * callback from NetApp to accept or reject a simple pairing request.
 *
 * @remarks If the bAutoPair variable set in BT_SETTINGS structure then
 * NetApp will automatically accept simple pairing requests.
 *
 * @param[in]  tHandle      NetApp handle
 * @param[in]  bAccept      True to accept, false to reject
 * @param[in]  pDevInfo     Pointer to Bluetooth device information
 * @return NETAPP_RETCODE
 */
NETAPP_RETCODE NetAppBluetoothSimplePairingAck(
    NETAPP_HANDLE           tHandle,
    bool                    bAccept,
    NETAPP_BT_DEV_INFO      *pDevInfo);

/// @} NetApp_Bluetooth


/**
 * @ingroup NetApp
 * @defgroup NetApp_Database Database APIs
 * @brief API to fetch information from the built-in database back end.
 *
 * @{
 */

/**
 * @brief Delete a Saved Access point
 *
 * Removed the access point from the database back end.
 *
 * @param[in]  tHandle      NetApp handle
 * @param[in]  pApInfo      the AP to delete
 *
 * @return NETAPP_RETCODE
 */
NETAPP_RETCODE NetAppWiFiDeleteSavedApInfo(
    NETAPP_HANDLE       tHandle,
    NETAPP_WIFI_AP_INFO *pApInfo);


/**
 * @brief Fetch saved Access Point List
 *
 *
 * @remarks This function returns a copy of the saved list. You must free the list
 * once you are finished with it.
 *
 * @param[in]  tHandle      NetApp handle
 * @param[out] pApInfoList  Saved AP list
 * @param[out] pulCount     Number of saved AP
 *
 * @return NETAPP_RETCODE
 *
 */
NETAPP_RETCODE NetAppWiFiGetSavedApInfoList(
    NETAPP_HANDLE       tHandle,
    NETAPP_WIFI_AP_INFO **pApInfoList,
    uint32_t            *pulCount);


/**
 * @brief Delete a Saved Bluetooth Device info from the database.
 *
 * Removed the saved information from the database back end.
 *
 * @param[in]  tHandle      NetApp handle
 * @param[in]  pDevInfo     Bluetooth Device Info (only the tAddr value is used
 *                          in this structure to lookup the hardware address to
 *                          delete from the database back end.
 * @return NETAPP_RETCODE
 */
NETAPP_RETCODE NetAppBluetoothDeleteSavedDevInfo(
    NETAPP_HANDLE       tHandle,
    NETAPP_BT_DEV_INFO  *pDevInfo);


/**
 * @brief Fetch saved Bluetooth pre-paired list
 *
 * @remarks This function returns a copy of the Bluetooth device list. You must
 * free the list once you are finished with it.
 *
 * @param[in]  tHandle      NetApp handle
 * @param[out] pDevInfoList  Saved Bluetooth device list
 * @param[out] pulCount     Number of saved AP
 *
 * @return NETAPP_RETCODE
 *
 */
NETAPP_RETCODE NetAppBluetoothGetSavedBtDevList(
    NETAPP_HANDLE       tHandle,
    NETAPP_BT_DEV_INFO  **pDevInfoList,
    uint32_t            *pulCount);

/// @} NetApp_Database

#ifdef __cplusplus
}
#endif

#endif
/// @} NetApp_API

