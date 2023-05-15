#!/usr/bin/python

# (c) 1997-2015 Netflix, Inc.  All content herein is protected by
# U.S. copyright and other applicable intellectual property laws and
# may not be copied without the express permission of Netflix, Inc.,
# which reserves all rights.  Reuse of any of this content for any
# purpose without the permission of Netflix, Inc. is strictly
# prohibited.

import telnetlib
import time
import os
import sys, getopt

def main(argv):
   global RUNNING
   global OUTDIR

   ######
   # Set default parameters
   ##
   HOST = 'localhost'
   PORT = 1234
   OUTDIR = 'output'
   INTERVAL = 1
   connected = False
   retrieveTimeOut = 10


   Terminator = "smaps_total"
   usage = 'collect_logs.py -s <server> -p <port> -d <output directory>'

   ######
   # Parse parameters
   ##
   try:
      opts, args = getopt.getopt(argv,"hs:p:d:i")
   except getopt.GetoptError:
      print usage
      sys.exit(2)
   for opt, arg in opts:
      if opt == '-h':
         print usage
         sys.exit()
      elif opt in ("-s"):
         HOST = arg
      elif opt in ("-p"):
         PORT = int(arg)
      elif opt in ("-d"):
         OUTDIR = arg
      elif opt in ("-i"):
         INTERVAL = int(arg)

   ######
   # Print parameters
   ##
   print "HOST     = " + HOST
   print "PORT     = " + str(PORT)
   print "OUTDIR   = " + OUTDIR
   print "INTERVAL = " + str(INTERVAL)

   ######
   # Create directory
   ##
   os.makedirs(OUTDIR)

   ######
   # Connect to smaps analyzer telner server
   ##
   print "CONNECTING:"
   while ( connected == False ):
      try:
         Telnet = telnetlib.Telnet(HOST, PORT)
         connected = True
      except:
         print "smaps_analyzer server not ready yet. trying it again in 1 sec"
         time.sleep(1)
   print "CONNECTED:"

   ######
   # Start srss command loop
   ##
   #CommandLine = "status  "
   #CommandLine = "totalsmaps  "
   CommandLine = "srss  "
   print CommandLine
   i = 0
   RUNNING = 1
   while RUNNING == 1:
      #f = open(OUTDIR + '/srss_' + format(i, '04d') + '.log', 'w')
      now = time.strftime('%Y%m%d-%H%M%S')
      #fileName = OUTDIR + "/ma_" + str("{0:010d}".format(int(now))) + ".log"
      fileName = OUTDIR + "/ma_" + now + ".log"
      f = open(fileName, 'w')
      Telnet.write(CommandLine)
      finished = False
      while finished == False:
         tmp1 = time.time()
         Blob = Telnet.read_until(Terminator, retrieveTimeOut)
         tmp2 = time.time()
         if ( (tmp2 - tmp1) >= retrieveTimeOut ):
            print 'Timed out when retrieving RSS info, timeOut value = ' + str(retrieveTimeOut) + '. Retrying...'
         else:
            if ( Blob[1:].rfind(Terminator, len(Blob[1:]) - 100) != -1 ):
               finished = True
            else:
               print Terminator + ' is not found in the blob. Retrying...'
         f.write(Blob[1:])
      print 'done retrieving ' + fileName
      f.close()
      i = i + 1
      time.sleep(INTERVAL)
      #print Blob


if __name__ == "__main__":
   main(sys.argv[1:])

