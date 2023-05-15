#!/usr/bin/env python

import sys
from traceback import print_exc

import gobject
import dbus
import dbus.glib
import time

def action():
    from dbus.mainloop.glib import DBusGMainLoop
    from _dbus_bindings import BUS_SYSTEM
    dbus_loop = DBusGMainLoop()
    bus = dbus.bus.BusConnection("tcp:host=192.168.111.10,port=55884", mainloop=dbus_loop)
    bus._bus_type = BUS_SYSTEM

    try:
        remote_media_play_obj = bus.get_object("Octopus.Appkit.Media.Play","/Octopus/Appkit/Media/Play")
        #remote_obj.connect_to_signal("event", event_signal_handler, dbus_interface="Octopus.Appkit.Meta.Service")

        mediaplay  = dbus.Interface(remote_media_play_obj, "Octopus.Appkit.Media.Play")

        # you can either specify the dbus_interface in each call...
        uid = mediaplay.GetRequestId(0,1)
        startTime = mediaplay.GetTsrStartTime(0)

        mainViewID = 0
        loopRange = range(5)

        for i in loopRange:
            playPosition = mediaplay.GetPlayPosition(0)
            print("-"*40)
            print("-TEST " + str(i))
            print("-"*40)
            print("UID : "+str(uid))
            print("Play Position : " + str(playPosition))
            if playPosition <= 10:
                print("TSR is not ready!!!")
                time.sleep(5)
                continue
            print(" T1. TSR from BOF")
            mediaplay.StartTsrPb(mainViewID, (uid, uid, 0, 0))
            time.sleep(2)
            print(" T2. TSR 2X FF")
            mediaplay.SetPlaySpeed(mainViewID, 400)
            time.sleep(2)
            print(" T3. Start LIVE")
            mediaplay.StartLive(mainViewID, (1, uid, uid, 0, 0))
            time.sleep(4)
            print(" ")
        
    except dbus.DBusException:
        print_exc()
        sys.exit(1)


if __name__ == '__main__':
    
#    thread.start_new_thread( main, () )
    action()

