#!/usr/bin/python

# (c) 1997-2015 Netflix, Inc.  All content herein is protected by
# U.S. copyright and other applicable intellectual property laws and
# may not be copied without the express permission of Netflix, Inc.,
# which reserves all rights.  Reuse of any of this content for any
# purpose without the permission of Netflix, Inc. is strictly
# prohibited.

import os
import sys, getopt

# ############################################
# function that generates the part of "using" string
# ###
def generateGPUsingTargetString(maxIndex):
   s = "1:($2"
   i = 3
   while (maxIndex > 1 and i <= maxIndex + 1):
      s += " + $" + str(i)
      i = i + 1
   s += ")"
   return s

# ############################################
# function that generates GnuPlot script
# ###
def generateGnuPlotScript(outputScriptName, finalSummaryFile, width, height, interval, outputPngName, wxtSizeSupported):
   # remove old output script
   if os.path.isfile(outputScriptName):
      os.remove(outputScriptName)

   # open output script for write
   f = open(outputScriptName, 'w')

   # generate first variable parts (summary file, category file, width, height, and etc)
   f.write("\n# set variables\n")
   f.write("width=" + width + "\n")
   f.write("height=" + height + "\n")
   f.write("interval=" + interval + "\n")
   f.write("outimagename=\"" + outputPngName + "\"\n")
   f.write("finalSummary=\"" + finalSummaryFile + "\"\n")

   # get legends from the summary file (expecting it to be in the first line)
   cf = open(finalSummaryFile, "r")
   legends = cf.readline().split(" ")
   cf.close()

   # pop the first column since it is not necessary
   legends.pop(0)

   # pop the last column to remove total
   legends.pop(len(legends)-1)
   maxIdx = len(legends)

   # print categories
   f.write("categories=\"" + ' '.join(legends) + "\"\n")

   # open the first template (the constant part of the gnuplot script)
   scriptDir = os.path.dirname(os.path.realpath(__file__))
   with open(scriptDir + "/gnuplot_script_template1.gp") as ft1:
      content = ft1.readlines()
      f.write(''.join(content))

   # set wxt size (GnuPlot 4.2 does not support set terminal wxt size)
   if wxtSizeSupported == True:
      f.write('set terminal wxt size ' + width + ', ' + height)

   # generate plot commands
   f.write("# plot\n");
   f.write("plot \\\n");

   plotStr1 = "  finalSummary every ::1 using "
   numColors = 17
   
   for idx,entry in enumerate(legends[::-1]):
      modifiedIdx = maxIdx-idx
      f.write(plotStr1 + generateGPUsingTargetString(modifiedIdx) + " t word(categories, " + str(modifiedIdx) + ") w filledcurves x1 lc rgb word(colors, " + str((modifiedIdx-1) % numColors + 1) + ")")
      if idx < maxIdx - 1:
          f.write(", \\\n")
      else:
          f.write("\n")



   # write the rest of the script
   f.write("# save the image\n")
   if wxtSizeSupported == True:
      f.write("set terminal png size width, height\n")
   else:
      f.write("set terminal png\n")
   f.write("set output outimagename\n")
   f.write("replot\n")
   f.write("\n")
   f.write("# pause and reload\n")
   f.write("pause interval\n")
   f.write("reread\n")

   ## open the second template (the constant part of the gnuplot script)
   #with open(scriptDir + "/gnuplot_script_template2.gp") as ft2:
   #   content = ft2.readlines()
   #   f.write(''.join(content))

   f.close()



def main(argv):

   ######
   # Set default parameters
   ##
   outputScriptName = 'gnuplot_script.gp'
   finalSummaryFile = 'finalSummary.txt'
   width = '1400'
   height = '600'
   interval = '3'
   outputPngName = 'ma_analysis_result.png'
   verboseMode = False
   wxtSizeSupported = True
   usage = 'create_gnu_plot_script.py -s <final summary list> -i <interval(sec)> --width=<width> --height=<height> -p <output ping file> '

   ######
   # Parse parameters
   ##
   try:
      opts, args = getopt.getopt(argv,"ho:s:i:p:v", ["help", "width=", "height=","nowxtsizesupport"])
   except getopt.GetoptError:
      print usage
      sys.exit(2)
   #if not opts:
   #   print usage
   #   sys.exit()
   for opt, arg in opts:
      if opt in ('h', '--help'):
         print usage
         sys.exit()
      elif opt in ('-o'):
         outputScriptName = arg
      elif opt in ('-s'):
         finalSummaryFile = arg
      elif opt in ('-i'):
         interval = arg
      elif opt in ('--width'):
         width = arg
      elif opt in ('--height'):
         height = arg
      elif opt in ('-p'):
         outputPngName = arg
      elif opt in ('-v'):
         verboseMode = True
      elif opt in ('--nowxtsizesupport'):
         wxtSizeSupported = False

   if verboseMode == True:
      print 'outputScriptName = ' + outputScriptName
      print 'finalSummaryFile = ' + finalSummaryFile
      print 'interval         = ' + interval
      print 'width            = ' + width
      print 'height           = ' + height
      print 'outputPngName    = ' + outputPngName


   ######
   # Check if the input files exist
   ##
   if os.path.isfile(finalSummaryFile) == False:
      print finalSummaryFile + " does not exist"
      sys.exit()

   ######
   # Generate GnuPlot script
   ##
   generateGnuPlotScript(outputScriptName, finalSummaryFile, width, height, interval, outputPngName, wxtSizeSupported)

if __name__ == '__main__':
   main(sys.argv[1:])

