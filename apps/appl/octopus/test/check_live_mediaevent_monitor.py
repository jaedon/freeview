#!/usr/bin/env python

import sys
from traceback import print_exc

import gobject
import dbus
import dbus.glib

import thread

def event_signal_handler(msgInt, msgStr, param1, param2, param3):

    print ("Received signal : "
          +"[ " + str(msgInt)+" ]\t" + "%40s" %(msgStr) + "\tp1: %6d" %(param1) + "\tp2: %6d" %(param2) + "\tp3: %6d" %(param3) )

def main_loop():

    print ("loop starting...")

    loop = gobject.MainLoop()
    loop.run()

def action():
    from dbus.mainloop.glib import DBusGMainLoop
    from _dbus_bindings import BUS_SYSTEM
    dbus_loop = DBusGMainLoop()
    bus = dbus.bus.BusConnection("tcp:host=10.0.12.150,port=55884", mainloop=dbus_loop)
    bus._bus_type = BUS_SYSTEM


    try:
        remote_obj = bus.get_object("Octopus.Appkit.Media.Play","/Octopus/Appkit/Media/Play")
        remote_obj.connect_to_signal("event", event_signal_handler, dbus_interface="Octopus.Appkit.Media.Play")

        # you can either specify the dbus_interface in each call...
        main_loop()

    except dbus.DBusException:
        print_exc()
        sys.exit(1)



if __name__ == '__main__':
    
#    thread.start_new_thread( main, () )
    action()

