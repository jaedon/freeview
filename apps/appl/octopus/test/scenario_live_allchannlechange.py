#!/usr/bin/env python

import sys
from traceback import print_exc

import gobject
import dbus
import dbus.glib

import thread
import time
import threading

g_MediaPlay = 0
g_MetaService = 0

g_lock = thread.allocate_lock()
g_loop = gobject.MainLoop()

def event_signal_handler_mediaPlay(msgInt, msgStr, param1, param2, param3):
    g_lock.acquire()
    print ("Received signal : " + "[ %2d"%(msgInt)+" ]\t" + "%40s" %(msgStr) + "\tp1: %6d" %(param1) + "\tp2: %6d" %(param2) + "\tp3: %6d" %(param3) )
    g_lock.release()

def testAction():

    g_lock.acquire()
    # Action
    print("****************************************************************")
    print(" All channel change test ")
    print("****************************************************************")
    print("\n\n")
    print("1.Get Service List")
    svcList = g_MetaService.GetServiceList()

    for svc in svcList:
        print("**************************")
        print("=> UID\t: " + str(svc[0]))
        print("=> LCN\t: " + str(svc[7]))
        print("=> URI\t: " + "dvb://" + str(svc[6]) + "." +str(svc[5]) + "." + str(svc[4]))
        print("=> NAME\t: " + str(svc[11].encode('utf-8')))


    print("****************************************************************")
    print("2.Change Service")

    mainviewid = g_MediaPlay.GetMainViewId()
    print("[MainView : " + str(mainviewid) + "]")
    print("\n\n")

    g_MediaPlay.Stop(mainviewid)
    g_lock.release()
    for svc in svcList:
        print("**************************")
        print("=> UID\t: " + str(svc[0]))
        g_lock.acquire()
        g_MediaPlay.StartLive(mainviewid, (1, svc[0], svc[0], 0, 0))
        g_lock.release()
        time.sleep(5)

    print("end test...")
    g_loop.quit()

def main_loop():
    g_loop.run()

def action():
    global g_MediaPlay
    global g_MetaService

    from dbus.mainloop.glib import DBusGMainLoop
    from _dbus_bindings import BUS_SYSTEM

    dbus_loop = DBusGMainLoop()
    bus = dbus.bus.BusConnection("tcp:host=10.0.12.150,port=55884", mainloop=dbus_loop)
    bus._bus_type = BUS_SYSTEM

    try:
        # Get Interface
        ifaceMediaPlay = bus.get_object("Octopus.Appkit.Media.Play", "/Octopus/Appkit/Media/Play")
        ifaceMediaPlay.connect_to_signal("event", event_signal_handler_mediaPlay, dbus_interface="Octopus.Appkit.Media.Play")
        ifaceMetaService = bus.get_object("Octopus.Appkit.Meta.Service", "/Octopus/Appkit/Meta/Service")

        g_MediaPlay = dbus.Interface(ifaceMediaPlay, "Octopus.Appkit.Media.Play")
        g_MetaService = dbus.Interface(ifaceMetaService, "Octopus.Appkit.Meta.Service")

        actionCall = threading.Thread(target=testAction, args=[])
        actionCall.start()
        #actionCall.join()

    except dbus.DBusException:
        print_exc()
        sys.exit(1)

if __name__ == '__main__':
    action()
    main_loop()

