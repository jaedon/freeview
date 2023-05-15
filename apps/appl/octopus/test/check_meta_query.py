#!/usr/bin/env python

import sys
from traceback import print_exc

import gobject
import dbus
import dbus.glib


def action():
    from dbus.mainloop.glib import DBusGMainLoop
    from _dbus_bindings import BUS_SYSTEM
    dbus_loop = DBusGMainLoop()
    bus = dbus.bus.BusConnection("tcp:host=10.0.12.150,port=55884", mainloop=dbus_loop)
    bus._bus_type = BUS_SYSTEM

    try:
        remote_media_play_obj = bus.get_object("Octopus.Appkit.Media.Play","/Octopus/Appkit/Media/Play")
        remote_obj = bus.get_object("Octopus.Appkit.Meta.Query","/Octopus/Appkit/Meta/Query")
        #remote_obj.connect_to_signal("event", event_signal_handler, dbus_interface="Octopus.Appkit.Meta.Service")

        mediaplay  = dbus.Interface(remote_media_play_obj, "Octopus.Appkit.Media.Play")
        metaq  = dbus.Interface(remote_obj, "Octopus.Appkit.Meta.Query")

            # you can either specify the dbus_interface in each call...
        uid = mediaplay.GetRequestId(0,1)
        event  = metaq.GetEvent_uid(0xFFFF, uid);

        print("UID : "+str(uid))
        print("+TYPE\t\t: "+str(event[0][3]))
        print("+NAME\t\t: "+str(event[1][0]))

    except dbus.DBusException:
        print_exc()
        sys.exit(1)


if __name__ == '__main__':
    
#    thread.start_new_thread( main, () )
    action()

