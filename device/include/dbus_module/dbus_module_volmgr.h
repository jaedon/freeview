#ifndef __DBUS_MODULE_VOLMGR__
#define __DBUS_MODULE_VOLMGR__

#define VMGR_OK       0
#define VMGR_ERR      1

#define VOLMGR_HOTPLUG_BUS_NAME    "dbus.module.signal.volmgr.hotplug.source"      
#define VOLMGR_HOTPLUG_OBJECT      "/dbus/module/signal/volmgr/hotplug"
#define VOLMGR_HOTPLUG_INTERFACE   "dbus.module.signal.volmgr.hotplug"
#define VOLMGR_HOTPLUG_SIG_NAME    "hotplug"


int DBUS_MODULE_Volmgr_Init(void);
int DBUS_MODULE_Volmgr_HotplugInit(void);

#endif