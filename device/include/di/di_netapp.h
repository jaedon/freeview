
#ifndef	_DI_NETAPP_H_
#define	_DI_NETAPP_H_

#include "htype.h"
#include "drv_err.h"
#include "netapp.h"

typedef void (*P_NETAPP_EVENT_CALLBACK)(
    void                *pParam,        //!< The pParam from NetAppOpen.
    NETAPP_CB_TYPE      tCbType,        //!< The Callback type.
    const void          *pvBuffer,      //!< Pointer to data specific to the callback.
    uint32_t            ulData0,        //!< Callback specific data 0.
    NETAPP_RETCODE      tResult,        //!< Callback results from the.
    NETAPP_IFACE        tIFace          //!< Callback Info structure.
);

P_NETAPP_EVENT_CALLBACK	CB_NetAppWifiEvent;
P_NETAPP_EVENT_CALLBACK	CB_NetAppBTEvent;

DRV_Error DRV_NETAPP_GetHandle(NETAPP_HANDLE *pstNetappHandle);
DRV_Error DRV_NETAPP_Init(void);

void P_WIFI_Netapp_Callback(void* pParam, NETAPP_CB_TYPE tCbType, const void *pvBuffer, HUINT32 ulData0, NETAPP_RETCODE tResult, NETAPP_IFACE tIFace);



#endif	/* _DI_NETAPP_H_ */
/* end of file */


