#!/usr/bin/python
import os, sys, shutil, tempfile, re
from optparse import OptionParser

parser = OptionParser()
parser.add_option("-n", "--name", action="store", type="string", dest="dpiname", help="New DPI name")
parser.add_option("-b", "--basedir", action="store", type="string", dest="basedir", help="DPI directory location")

(options, args) = parser.parse_args()

filelist=[]
filelist.append("ESPlayerNative.cpp")
filelist.append("ESPlayerNative.h")
filelist.append("AudioESPlayerNative.cpp")
filelist.append("AudioESPlayerNative.h")
filelist.append("AudioMixerSink.cpp")
filelist.append("AudioMixerSink.h")
filelist.append("BufferManagerNative.cpp")
filelist.append("BufferManagerNative.h")
filelist.append("CMakeLists.txt")
filelist.append("CurlUtils.cpp")
filelist.append("CurlUtils.h")
filelist.append("DeviceLibNative.cpp")
filelist.append("DeviceLibNative.h")
filelist.append("DeviceThread.cpp")
filelist.append("DeviceThread.h")
filelist.append("drm.cmake")
filelist.append("DrmDecrypter.cpp")
filelist.append("DrmDecrypter.h")
filelist.append("DrmError.cpp")
filelist.append("DrmError.h")
filelist.append("ESManagerNative.cpp")
filelist.append("ESManagerNative.h")
filelist.append("ESPlayerConstants.cpp")
filelist.append("ESPlayerConstants.h")
filelist.append("FileSystem.cpp")
filelist.append("FileSystem.h")
filelist.append("JsConstants.h")
filelist.append("LinuxInterfaces.cpp")
filelist.append("ManufSS.cpp")
filelist.append("ManufSS.h")
filelist.append("MediaUtils.cpp")
filelist.append("MediaUtils.h")
filelist.append("NetworkInterfaceListener.h")
filelist.append("PlaybackGroupNative.cpp")
filelist.append("PlaybackGroupNative.h")
filelist.append("PlayReadyDrmSession.cpp")
filelist.append("PlayReadyDrmSession.h")
filelist.append("PlayReadyDrmSystem.cpp")
filelist.append("PlayReadyDrmSystem.h")
filelist.append("QADeviceBridge.cpp")
filelist.append("QADeviceBridge.h")
filelist.append("SampleWriterNative.cpp")
filelist.append("SampleWriterNative.h")
filelist.append("SignupBridge.cpp")
filelist.append("SignupBridge.h")
filelist.append("SSOBridge.cpp")
filelist.append("SSOBridge.h")
filelist.append("SystemValues.cpp")
filelist.append("SystemValues.h")
filelist.append("VideoESPlayerNative.cpp")
filelist.append("VideoESPlayerNative.h")
filelist.append("VideoPipelineCapability.cpp")
filelist.append("wirelesstools.h")


def get_skeletondir(d):
    orig=d
    result='f'
    for filename in os.listdir(d):
        if(filename=='skeleton'):
            result='t'
            break
        #print 'File: ', filename
    if(result=='f'):
        print 'basedir(', orig, ') is not valid'
        return None
    else: 
        if(d.endswith('/')==False):
            d+='/'
        return d+'skeleton'

def ifblockremoval(f):
    #Create temp file
    fh, abs_path = tempfile.mkstemp()
    new_file = open(abs_path,'w')
    old_file = open(f)
    flag=False
    for line in old_file:
        if(re.match("^if",line)): flag=True
        if(flag==False): new_file.write(line)
        if(re.match("^endif",line)): flag=False

    #close temp file
    new_file.close()
    os.close(fh)
    old_file.close()
    #Remove original file
    os.remove(f)
    #Move new file
    shutil.move(abs_path, f)

def object_replace(f, o, r):
    #print "replace ", o, "with ", r, "in ", f
    #Create temp file
    fh, abs_path = tempfile.mkstemp()
    new_file = open(abs_path,'w')
    old_file = open(f)
    for line in old_file:
        if(line.find(o)==-1):
            new_file.write(line)
        elif(line.find(r)==-1): 
            new_line = line.replace(o, r)
            new_file.write(new_line)
        else: 
            new_file.write(line)
    #close temp file
    new_file.close()
    os.close(fh)
    old_file.close()
    #Remove original file
    os.remove(f)
    #Move new file
    shutil.move(abs_path, f)

def remove_lines(f, s):
    #print "remove", s, "from ", f
    #Create temp file
    fh, abs_path = tempfile.mkstemp()
    new_file = open(abs_path,'w')
    old_file = open(f)
    for line in old_file:
        if(line.find(s)==-1):
            # Only write lines that don't match
            new_file.write(line)

    #close temp file
    new_file.close()
    os.close(fh)
    old_file.close()
    #Remove original file
    os.remove(f)
    #Move new file
    shutil.move(abs_path, f)

# first make sure the DPI directory is good.  We do that by ensuring reference exists.
if( options.basedir is None ):
    print 'basedir is not set, assuming current directory'
    options.basedir='./'

print 'basedir is', options.basedir
print 'name is', options.dpiname

skeletondir=get_skeletondir(options.basedir)
if(skeletondir is None):
    sys.exit(0)

print 'skeletondir: ', skeletondir

newdpidir=options.basedir
if(newdpidir.endswith('/')==False):
    newdpidir+='/'
newdpidir+=options.dpiname
print 'new dpi dir: ', newdpidir

# make the new DPI directory if it doesn't exist
if not os.path.exists(newdpidir):
    os.makedirs(newdpidir)

# Now make the DPI name properly capitalized
options.dpiname=options.dpiname.capitalize()

# List of object tuples to replace in the final directory
rlist=[]

# Copy the files into the new directory
listSkeletonDir = os.listdir(skeletondir)
listSkeletonDir.sort(key = lambda s: len(s), reverse=True)
for f in listSkeletonDir:
    filename=''
    newfile=f
    found=False
    objreplace=[]
    for validfile in filelist:
        if(validfile in f):
            found=True
            break
    if(found==False):
        print "Not copying ", f
        continue;
    if(f.find('Native')>-1):
        i=newfile.index('Native')
        tmpfile=newfile[:i]+newfile[i+6:]
        #dont remove native suffix
        #tmpfile=newfile 
        newfile=options.dpiname+tmpfile
        rlist.append(f.split('.')[0])
        rlist.append(newfile.split('.')[0])
        shutil.copyfile(skeletondir+'/'+f,newdpidir+'/'+newfile)
    elif(f.find('FileSystem')>-1):
        rlist.append('FileSystem')
        rlist.append(options.dpiname+'System')
        shutil.copyfile(skeletondir+'/'+f,newdpidir+'/'+options.dpiname+'System'+'.'+f.split('.')[1])
    elif(f.find(' Drm')>-1):
        newfile=options.dpiname+newfile
        rlist.append(f.split('.')[0])
        rlist.append(newfile.split('.')[0])
        shutil.copyfile(skeletondir+'/'+f,newdpidir+'/'+newfile)
    elif(newfile.endswith("cpp")|newfile.endswith("h")|newfile.endswith("txt")|newfile.endswith("cmake")):
        shutil.copyfile(skeletondir+'/'+f,newdpidir+'/'+newfile)
    #print 'copying ' + skeletondir+'/'+f,newdpidir+'/'+newfile

# some cleaning up for corner case
rlist.append(options.dpiname+"Audio"+options.dpiname+"ESPlayer")
rlist.append(options.dpiname+"AudioESPlayer")
rlist.append(options.dpiname+"Video"+options.dpiname+"ESPlayer")
rlist.append(options.dpiname+"VideoESPlayer")

# Replace objects in the new directory
index=0
while index < len(rlist):
    #print "Changing for string - " + rlist[index] + " to " + rlist[index+1] 
    for f in os.listdir(newdpidir):
        object_replace(newdpidir+'/'+f, rlist[index], rlist[index+1])
        object_replace(newdpidir+'/'+f, rlist[index].upper(), rlist[index+1].upper())
    index+=2


