#!/usr/bin/python

import time
import os
import sys, getopt
import shutil

# ############################################
# debug print function
# ###
def debugPrint(whatever):
   global verboseMode
   if verboseMode == True:
      print whatever

# ############################################
# function that appends one summary line (per RSS info file)
# ###
def appendData(finalSummaryFile, summaryFile, column, numData):
   print 'generating ' + summaryFile + ' ...'

   # read summary file
   f = open(summaryFile, 'r')
   summary = f.readlines()
   f.close()

   # check if the summary is empty or the some data is missing
   actualLen = len(summary)
   if (actualLen == 0) or (numData != -1 and numData != actualLen):
       # do not append this line since it will throw off gnuplot
       return numData

   # Generate transposed version with additional first column
   transposedColumn = []
   if column == 1 :
      transposedColumn.append('legend')
   else:
      # append the time stamp, which is part of a log file
      transposedColumn.append(summaryFile.split('ma_')[1].split('.')[0])

   for line in summary:
      debugPrint (line.split()[column])
      transposedColumn.append(line.split()[column])

   # write the list into summary file as one line
   finalSummaryFile.write(' '.join(transposedColumn) + '\n')

   # return the numbeer of column excluding the first added column
   return actualLen

# ############################################
# function that generates RSS summary
# ###
def generateSummary(logDir, summaryDir, finalSummaryFile, consolidationScript, categoryFile):

   # set finalSummaryPath
   finalSummaryPath = summaryDir + '/' + finalSummaryFile
   debugPrint ('finalSummaryPath = ' + finalSummaryPath)

   # initialize the number of data in summary file
   numData = -1


   ######
   # Create directory
   ##
   tmpSummaryName = '/tmp/tmpSummary'
   updateSummary = 'false'

   # remove temp file
   if os.path.isfile(tmpSummaryName):
      os.remove(tmpSummaryName)

   # check if the directory exists
   if os.path.isdir(summaryDir) == 0:
      # create directory
      os.mkdir(summaryDir)
   else:
      # check if final summary file exists
      if (os.path.isfile(finalSummaryPath) and os.stat(finalSummaryPath).st_size != 0):
         # if yes, prepare for update mode
         updateSummary = True


   # check if it is the update mode
   if updateSummary == True:
      # count the number of lines
      with open(finalSummaryPath) as fin:
         lines = sum(1 for line in fin)
      # copy it to tmp summary file and open as append
      shutil.copyfile(finalSummaryPath, tmpSummaryName)
      tmpSummary = open(tmpSummaryName, 'a')
   else:
      # if no, simply create a new file
      tmpSummary = open(tmpSummaryName, 'w')


   # get the smaps analysis log file list
   loglist = []
   for logfile in os.listdir(logDir):
      if os.path.isfile(logDir + '/' + logfile):
         loglist.append(logfile)

   # sort the list
   sortedList = sorted(loglist);
   debugPrint(sortedList)

   # get the size of the list to skip the last line
   size = len(sortedList)

   # for each log, generate the summary (using consolidation script) and update the finalSummary file
   for idx, logfile in enumerate(sortedList):
      if ( updateSummary == True and idx < lines ):
          continue

      summaryFileName = summaryDir + '/' + logfile + '.summary'
      os.system(consolidationScript + ' ' + logDir + '/' + logfile + ' ' + categoryFile + ' true > ' + summaryFileName)

      # Append category names for the first time
      if ( updateSummary == 'false' and idx == 0):
         numData = appendData(tmpSummary, summaryFileName, 1, numData)

      # Append it except for the last log
      if idx < size - 1: # avoid the last one because it is sometimes not ready
         numData = appendData(tmpSummary, summaryFileName, 0, numData)

   # close tmp file and copy it into the final summary file
   tmpSummary.close()
   shutil.copyfile(tmpSummaryName, finalSummaryPath)


def main(argv):
   global verboseMode
   ######
   # Set default parameters
   ##
   scriptDir = os.path.dirname(os.path.realpath(__file__))
   verboseMode = False
   logDir = 'output'
   summaryDir = 'summary'
   finalSummaryFile = 'finalSummary.txt'
   usage = 'generate_summary.py -d <input log directory> -o <output directory>'
   consolidationScript = scriptDir + '/ma_rss_info_consolidater.py'
   categoryFile = scriptDir + '/configurations/nrdp41_conf.txt'

   ######
   # Parse parameters
   ##
   try:
      opts, args = getopt.getopt(argv,"hd:o:v", ["help","consolidationscript=","categoryfile="])
   except getopt.GetoptError:
      print usage
      sys.exit(2)
   if not opts:
      print usage
      sys.exit()
   for opt, arg in opts:
      if opt in ('-h', '--help'):
         print usage
         sys.exit()
      elif opt in ('-d'):
         logDir = arg
      elif opt in ('-o'):
         summaryDir = arg
      elif opt in ('-v'):
         verboseMode = True
      elif opt in ("--consolidationscript"):
         consolidationScript = arg
      elif opt in ("--categoryfile"):
         categoryFile = arg


   ######
   # Print parameters
   ##
   debugPrint ("logDir             = " + logDir)
   debugPrint ("summaryDir         = " + summaryDir)
   debugPrint ("finalSummaryFile   = " + finalSummaryFile)
   debugPrint ("categoryfile       = " + categoryFile)

   ######
   # Check if the directory exists
   ##
   while ( True ):
      if ( os.path.exists(logDir) == True and os.listdir(logDir) != "" ):
         break
      print logDir + " does not exist. Waiting..."
      time.sleep(1)

   # sleep some more
   time.sleep(2)

   ######
   # Generate summary
   ##
   generateSummary(logDir, logDir + '/' + summaryDir, finalSummaryFile, consolidationScript, categoryFile)


if __name__ == "__main__":
   main(sys.argv[1:])

