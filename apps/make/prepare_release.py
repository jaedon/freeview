'''
Created on 2012. 3. 28.

@author: swcho
'''

import getopt, sys, os
import shutil, re
import string

gExcludedFolders = ['.project', '.svn', 'emul', 'Test', 'test', 'design_result', 'testpage' ]

def printUsage():
    print 'Usage:'
    print 'prepare_release.py -s [Source Folder] -t [Target Folder]'
    pass

if __name__ == '__main__':
    try:
        opts, args = getopt.getopt(sys.argv[1:], "s:t:")
    except getopt.GetoptError, err:
        print str(err)
        printUsage()
        sys.exit(2)

    dirSource = None
    dirTarget = None

    for o, a in opts:
        if o == '-s': dirSource = a
        elif o == '-t': dirTarget = a

    if not dirSource or not dirTarget:
        printUsage()
        sys.exit(2)

    print 'Source: ' + dirSource
    print 'Target: ' + dirTarget
    if os.path.exists(dirTarget + os.sep + dirSource):
        shutil.rmtree(dirTarget + os.sep + dirSource)
    freesat_debug = re.compile('.*DBG\(')
    sugar_debug = re.compile('.*hx.log\(')
    console_debug = re.compile('.*console.log\(')
    for dirname, dirnames, filenames in os.walk(dirSource):
        relativeDirName = string.replace(dirname, dirSource, '', 1)
        relativeDirName = string.rstrip(relativeDirName, os.sep)
        dirs = relativeDirName.split(os.sep)
        if len(dirs) > 1 and dirs[1] in gExcludedFolders:
            continue

        targetDir = dirTarget + os.sep + relativeDirName
        if not os.path.exists(targetDir):
            os.makedirs(targetDir)

        for filename in filenames:
            if filename in gExcludedFolders: continue
            sourceFileName = dirname + os.sep + filename
            targetFileName = targetDir + os.sep + filename
            name, extension = os.path.splitext(filename)
            if extension == '.js':
                print 'JS: ' + sourceFileName
                sourceFile = open(sourceFileName)
                targetFile = open(targetFileName, 'w')
                lineNo = 1
                for line in sourceFile.readlines():
                    if freesat_debug.match(line) or sugar_debug.match(line) or console_debug.match(line):
                        print str(lineNo) + ':' + line.strip()
                    else:
                        targetFile.write(line)
                    lineNo += 1
                sourceFile.close()
                targetFile.close()
            else:
                shutil.copy2(sourceFileName, targetFileName)

    pass
