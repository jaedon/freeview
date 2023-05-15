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
g_MetaRecord = 0

g_lock = thread.allocate_lock()
g_loop = gobject.MainLoop()

def event_signal_handler_mediaPlay(msgInt, msgStr, param1, param2, param3):
    g_lock.acquire()
    print("Received signal : " + "[ %2d"%(msgInt) + " ]\t" + "%40s" % (msgStr) + "\tp1: %6d" % (
        param1) + "\tp2: %6d" % (param2) + "\tp3: %6d" % (param3))
    g_lock.release()


def testAction():
    g_lock.acquire()
    # Action
    print("****************************************************************")
    print(" All contents change test ")
    print("****************************************************************")
    print("\n\n")
    print("1.Get Content List")

    contentCnt = g_MetaRecord.Count()
    print("Record Count : " + str(contentCnt))

    print("****************************************************************")
    print("2.Change Service")

    mainviewid = g_MediaPlay.GetMainViewId()
    print("[MainView : " + str(mainviewid) + "]")
    print("\n\n")

    g_MediaPlay.Stop(mainviewid)
    g_lock.release()

    for i in range(contentCnt):
        g_lock.acquire()
        item = g_MetaRecord.GetByIdx(i)
        print("\n[Index \t: " + str(i) + "]")
        print("+CID\t\t: " + str(item[0]))
        print("+Url\t\t: " + item[1])
        print("+Name\t\t: " + item[2])
        print("+TPath\t\t: " + item[3])
        print("+RecStartTime\t: " + str(item[4]))
        print("+RecDuration\t: " + str(item[5]))
        print("+RecOrgDurat\t: " + str(item[6]))
        print("+ulType\t\t: " + str(item[7]))
        print("+eRecStatus\t: " + str(item[8]))
        print("+eRecEndReason\t: " + str(item[9]))

        g_MediaPlay.StartPvrPb(mainviewid, (item[0], 0))
        g_lock.release()
        time.sleep(10)


        #g_lock.acquire()
        #g_MediaPlay.Stop(mainviewid)
        #g_lock.release()

    print("end test...")
    g_loop.quit()


def main_loop():
    g_loop.run()


def action():
    global g_MediaPlay
    global g_MetaRecord

    from dbus.mainloop.glib import DBusGMainLoop
    from _dbus_bindings import BUS_SYSTEM

    dbus_loop = DBusGMainLoop()
    bus = dbus.bus.BusConnection("tcp:host=10.0.12.150,port=55884", mainloop=dbus_loop)
    bus._bus_type = BUS_SYSTEM

    try:
        # Get Interface
        ifaceMediaPlay = bus.get_object("Octopus.Appkit.Media.Play", "/Octopus/Appkit/Media/Play")
        ifaceMediaPlay.connect_to_signal("event", event_signal_handler_mediaPlay, dbus_interface="Octopus.Appkit.Media.Play")
        ifaceMetaRecord = bus.get_object("Octopus.Appkit.Meta.Record", "/Octopus/Appkit/Meta/Record")

        g_MediaPlay = dbus.Interface(ifaceMediaPlay, "Octopus.Appkit.Media.Play")
        g_MetaRecord = dbus.Interface(ifaceMetaRecord, "Octopus.Appkit.Meta.Record")

        actionCall = threading.Thread(target=testAction, args=[])
        actionCall.start()
        #actionCall.join()

    except dbus.DBusException:
        print_exc()
        sys.exit(1)


if __name__ == '__main__':
    action()
    main_loop()
