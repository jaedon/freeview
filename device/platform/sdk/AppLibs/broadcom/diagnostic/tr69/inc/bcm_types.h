/*****************************************************************************
//
//  Copyright (c) 2005  Broadcom Corporation
//  All Rights Reserved
//  No portions of this material may be reproduced in any form without the
//  written permission of:
//          Broadcom Corporation
//          16215 Alton Parkway
//          Irvine, California 92619
//  All information contained in this document is Broadcom Corporation
//  company private, proprietary, and trade secret.
//
******************************************************************************
//
//  Filename:       bcmcfm.h
//  Author:         Peter Tran
//  Creation Date:  04/28/05
//
******************************************************************************
//  Description:
//  Define the global types, enums. and constants for BcmCfm_ in object format
//
*****************************************************************************/

#ifndef __BCM_CFM_H__
#define __BCM_CFM_H__

#include <netinet/in.h>

/* System Group Objects */
#define BCMCFM_OBJ_SYS_DEVICE_INFO              0x00100001 /* BcmCfm_DevInfoCfg_t       */
#define BCMCFM_OBJ_SYS_MGMT_ADMIN_LOGIN         0x00100002 /* BcmCfm_MgmtLoginCfg_t     */
#define BCMCFM_OBJ_SYS_MGMT_USER_LOGIN          0x00100003 /* BcmCfm_MgmtLoginCfg_t     */
#define BCMCFM_OBJ_SYS_MGMT_REMOTE_LOGIN        0x00100004 /* BcmCfm_MgmtLoginCfg_t     */
#define BCMCFM_OBJ_SYS_SYSLOG                   0x00100005 /* BcmCfm_SysLogCfg_t        */
#define BCMCFM_OBJ_SYS_UPNP                     0x00100006 /* BcmCfm_UpnpCfg_t          */
#define BCMCFM_OBJ_SYS_SNMP                     0x00100007 /* BcmCfm_SnmpCfg_t          */
#define BCMCFM_OBJ_SYS_DDNS                     0x00100008 /* BcmCfm_DDnsCfg_t          */
#define BCMCFM_OBJ_SYS_NTP                      0x00100009 /* BcmCfm_NtpCfg_t           */
#define BCMCFM_OBJ_SYS_IPP                      0x0010000A /* BcmCfm_IppCfg_t           */
#define BCMCFM_OBJ_SYS_TR069                    0x0010000B /* BcmCfm_TR069Cfg_t         */
#define BCMCFM_OBJ_SEC_ACC_CTRL_BY_HOST         0x0010000C /* BcmCfm_ACHostCfg_t    */        
#define BCMCFM_OBJ_SEC_ACC_CTRL_BY_SERVICE      0x0010000D /* BcmCfm_ACServiceCfg_t */         

/* Physical Interface Group Objects     */
#define BCMCFM_OBJ_IFC_ETHERNET                 0x00200001 /* BcmCfm_EthIfcCfg_t        */
#define BCMCFM_OBJ_IFC_USB                      0x00200002 /* BcmCfm_UsbIfcCfg_t        */
#define BCMCFM_OBJ_IFC_WLAN                     0x00200003 /* BcmCfm_WlanIfcCfg_t       */
#define BCMCFM_OBJ_IFC_ADSL                     0x00200004 /* BcmCfm_AdslIfcCfg_t       */
#define BCMCFM_OBJ_IFC_ATM                      0x00200005 /* BcmCfm_AtmIfcCfg_t        */
#define BCMCFM_OBJ_IFC_ATM_VCC                  0x00200006 /* BcmCfm_AtmVccCfg_t        */
                                                                                                                                                                                
/* Network Group Objects */
#define BCMCFM_OBJ_NTWK_INTF                    0x00300001 /* BcmCfm_NtwkIntfCfg_t      */
#define BCMCFM_OBJ_NTWK_BRIDGE                  0x00300002 /* BcmCfm_NtwkBridgeCfg_t*/
#define BCMCFM_OBJ_NTWK_DEFAULT_GATEWAY         0x00300007 /* BcmCfm_NtwkDefaultGatewayCfg_t */
#define BCMCFM_OBJ_NTWK_ROUTE_ENTRY             0x00300008 /* BcmCfm_NtwkRouteEntryCfg_t         */
#define BCMCFM_OBJ_NTWK_DNS                     0x00300009 /* BcmCfm_NtwkDnsCfg_t                        */
#define BCMCFM_OBJ_NTWK_RIP                     0x0030000A /* BcmCfm_NtwkRipCfg_t                        */
#define BCMCFM_OBJ_NTWK_LAN_HOST                0x0030000B /* !!!JK BcmCfm_NtwkRipCfg_t          */

/* Network Security Group Objects */
#define BCMCFM_OBJ_SEC_DMZ_HOST                 0x00400001 /* BcmCfm_SecDmzHostCfg_t  */  
#define BCMCFM_OBJ_SEC_VIRTUAL_SERVER           0x00400002 /* BcmCfm_SecVirtualServerCfg_t */   
#define BCMCFM_OBJ_SEC_FILTER                   0x00400003 /* BcmCfm_SecFilterCfg_t  */  
#define BCMCFM_OBJ_SEC_PRT_TRIGGER              0x00400004 /* BcmCfm_SecPortTriggerCfg_t */   
#define BCMCFM_OBJ_SEC_MAC_FILTER               0x00400005 /* BcmCfm_SecMacFilterCfg_t  */  
#define BCMCFM_OBJ_SEC_PARENTAL_CONTROL         0x00400006 /* BcmCfm_SecParentalControlCfg_t */      

/* Network Performance Group Objects */
#define BCMCFM_OBJ_PERF_QOS_CLS                 0x00500001 /* BcmCfm_PerfQosCfg_t */

/* Network Voice Group Objects */
#define BCMCFM_OBJ_VOICE                        0x00600001 /* BcmCfm_VoiceCfg_t */

/* Network Bluetooth Group Objects */
#define BCMCFM_OBJ_BLUETOOTH                    0x00700001 /* BcmCfm_BluetoothCfg_t*/

/*interface definition*/
#define IFC_ENET_ID                                                             0x1
#define IFC_USB_ID                                                              0x2

/* API return status*/
typedef enum {
   BcmCfm_Ok = 0,
   BcmCfm_Fail
} BcmCfm_Status;

typedef enum {
        BcmCfm_BroadcomImageFormat = 0,
        BcmCfm_FlashImageFormat,
        BcmCfm_PsiTextFormat
} BcmCfm_DownloadFormat, BcmCfm_UploadFormat;

/* Configuration Status*/
typedef enum {
    BcmCfm_CfgDisabled,
    BcmCfm_CfgEnabled
} BcmCfm_CfgStatus;

typedef enum {
    BcmCfm_StatsTxBytes,
        BcmCfm_StatsRxBytes,
        BcmCfm_StatsTxPkts,
        BcmCfm_StatsRxPkts
} BcmCfm_Stats;

/* Network interface protocols*/
typedef enum {
    BcmCfm_NtwkIntfProtoPPPoE,
    BcmCfm_NtwkIntfProtoPPPoA,
    BcmCfm_NtwkIntfProtoIPoA,
    BcmCfm_NtwkIntfProtoMAC
} BcmCfm_NtwkIntfProtocol;

typedef struct BcmCfm_TR069Cfg_t {
        BcmCfm_CfgStatus    status;
        char                *acsUrl;
        char                *acsUser;
        char                *acsPwd;
        char                    *connReqUser;
        char                    *connReqPwd;
        char                    upgradesManaged;
} BcmCfm_TR069Cfg_t;

typedef BcmCfm_TR069Cfg_t *PBcmCfm_TR069Cfg_t;

#ifndef uint32
typedef unsigned int uint32;
#endif

#if defined(__cplusplus)
extern "C" {
#endif
    BcmCfm_Status BcmCfm_objCreate(uint32 objId, void *objValue, uint32 *objIndex);
    BcmCfm_Status BcmCfm_objGet(uint32 objId, void **objValue, uint32 *objIndex);
    BcmCfm_Status BcmCfm_objSet(uint32 objId, void *objValue, uint32 objIndex);
    BcmCfm_Status BcmCfm_objDelete(uint32 objId, uint32 objIndex);
    BcmCfm_Status BcmCfm_objAlloc(uint32 objId, void **objValue);
    BcmCfm_Status BcmCfm_objFree(uint32 objId, void *objValue);
    BcmCfm_Status BcmCfm_alloc(uint32 size, void **objValue);
    BcmCfm_Status BcmCfm_free(void *objValue);
    BcmCfm_Status BcmCfm_stsGet(uint32 objId, void **objValue, uint32 objIndex);
    BcmCfm_Status BcmCfm_stsClear(uint32 objId, uint32 objIndex);
    BcmCfm_Status BcmCfm_stsFree(uint32 objId, void *objValue);
    BcmCfm_Status BcmCfm_reboot(void);
    BcmCfm_Status BcmCfm_isRebootNeeded(uint32 *needReboot);
    BcmCfm_Status BcmCfm_restoreDefault(void);
    BcmCfm_Status BcmCfm_saveConfig(void);
    BcmCfm_Status BcmCfm_download(char *data, uint32 size);
    BcmCfm_Status BcmCfm_upload(char *data, uint32 size);
    BcmCfm_Status BcmCfm_isConfigChanged(uint32 *changeConfig);
//    extern BcmCfm_TR069Cfg_t defTr69;
#if defined(__cplusplus)
}
#endif

#endif /* __BCM_CFM_H__ */
