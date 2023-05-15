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
        remote_obj = bus.get_object("Octopus.Appkit.Meta.Service","/Octopus/Appkit/Meta/Service")
	#remote_obj.connect_to_signal("event", event_signal_handler, dbus_interface="Octopus.Appkit.Meta.Service")

	metasvc  = dbus.Interface(remote_obj, "Octopus.Appkit.Meta.Service")

        # you can either specify the dbus_interface in each call...
	svclist = metasvc.GetServiceList();
	svccount = len(svclist);	
	print("Service Count : "+str(svccount))

	for i in range(svccount):
		print("\n[Index \t: "+str(i)+"]")
		print("+UID\t\t: "+str(svclist[i][0]))
                print("+NAME\t\t: "+svclist[i][11])



    except dbus.DBusException:
        print_exc()
        sys.exit(1)



if __name__ == '__main__':
    
#    thread.start_new_thread( main, () )
    action()

