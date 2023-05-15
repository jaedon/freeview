
TESTING THE APP
---------------

   Before running the standby app, make sure you have the test stream in videos/cnnticker.mpg. Then run the standby app as follows
   
   nexus standby
   
   This will run the test app with a default standby timeout of 5 seconds. Press the buttons 1 to 3 on the remote to enter the standby modes S1, S2, S3. 
   Button 1 -> S1
   Button 2 -> S2
   Button 3 -> S3
   
   To resume from standby press the "Power" button on the remote. 
   
   Pressing the power button on the remote will cycle through all the standby modes with the default timeout value. 

   Run the app as follows for a user specified timeout value
   
   nexus standby -timeout X
   
   Where X is the timeout value in seconds. A timeout of 0 will disable the timer wakeup. 
   
   For more usage options type 

   nexus standby --help



WAKEUP DEVICES
--------------
   
   By default the following wakeup devices are enabled
   
   IR
   UHF 
   XPT 	
   CEC 
   WOL

   Any wakeup device can be disabled by passing in a user param. For example IR wake up can be disabled as follows
   
   nexus standby -ir 0 

   To disable all wakeup devices 
   
   nexus standby -ir 0 -ufh 0 -xpt 0 -cec 0- wol 0 -timeout 0



STREAM SOURCE
-------------

   By default the standby app uses playback stream as source. It also supports QAM and SAT sources. This is particularly useful to test frontend power down and resume. 
   
   To use QAM source 

   nexus standby -qam

   To use SAT source 
   
   nexus standby -sat



WOL NOTES
---------

   Standby app uses a utility called 'ethtool' for wake on LAN. 'ethtool' can be found in /BSEAV/app/standby. Copy this to the same location as the standby binary. 
   
   To test WOL ENET, run the standby app 
   
   nexus standby

   
   To test magic packet wakeup, run the wakeonlan utility from a PC.
   
   wakeonlan -i 'ip_address' 'hw_address'
   
   e.g. wakeonlan -i 10.13.135.174 00:10:18:DA:C3:93

   The wakeonlan utility can be found in /BSEAV/app/standby.


   To test ARP packet wakeup, clear the ARP cache entry and ping the STB from a PC
   
   arp -d	
   ping 10.13.135.174

   For more information on WOL refer to http://stb-bld-00.broadcom.com/gitweb/2637.git/blob_plain/HEAD:/doc/networking.html#h_6__Wake_on_LAN

