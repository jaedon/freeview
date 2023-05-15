#!/usr/bin/env python

import sys
from traceback import print_exc

import dbus
import dbus.glib

def action():
    from dbus.mainloop.glib import DBusGMainLoop
    from _dbus_bindings import BUS_SYSTEM
    dbus_loop = DBusGMainLoop()
    bus = dbus.bus.BusConnection("tcp:host=10.0.12.150,port=55884", mainloop=dbus_loop)
    bus._bus_type = BUS_SYSTEM

    try:
        ifaceModuleSubmodule = bus.get_object("Octopus.Appkit.Module.Submodule","/Octopus/Appkit/Module/Submodule")
        moduleSubmodule = dbus.Interface(ifaceOutputAudio, "Octopus.Appkit.Module.Submodule")

	moduleSubmodule.doSometing();
	reval = moduleSubmodule.doSometing(100, 200);
	moduleSubmodule.doSometing(param1, (100, 200));

    except dbus.DBusException:
        print_exc()
        sys.exit(1)


if __name__ == '__main__':
    action()

