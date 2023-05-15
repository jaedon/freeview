
Instructions on building a PlayReady PK static library for Linux:

1. Create a directory as the root of PlayReady. Copy everything from the PlayReady PK installation root (the directory that contains the "source" directory) to the PlayReady root directory just created.

2. Define the following environmental variables:
PLAYERADY_ROOT: It is the full path of the PlayReady root directory created above.
PLAYREADY_DIR: It is the full path of "source/linux" directory under the PlayReady root directory.
PLAYREADY_PROFILE: It is the name of the build profile being used and should be "drmprofilelinux.mk" unless a custom build profile is used.
PLAYREADY_GXX: It is the name of the compiler being used. If it is unspecified then its default value is g++.
LINUX_BUILD: It should be set to 1.
_NTROOT: It should be set to be the same as PLAYREADY_ROOT.

3. Create a 'PK' directory under the PlayReady root directory. Under the 'PK' directory just created, create a symbolic link named 'WMDRM-MD' pointing to the directory specified by PLAYREADY_DIR.

4. Below is an example (assume the PlayReady root is ~/playready):
export PLAYREADY_ROOT=~/playready
export PLAYREADY_DIR=$PLAYREADY_ROOT/source/linux
export PLAYREADY_PROFILE=drmprofilelinux.mk
export LINUX_BUILD=1
export _NTROOT=$PLAYREADY_ROOT

cd ~/playready
mkdir PK
cd PK
ln -s ~/playready/source/linux WMDRM-MD

5. To build the PlayReady PK static library, go to the 'source' directory under the PlayReady root and type 'make clean all'.

6. The generated library is binplaced under the 'bin/lib' directory under the PlayReady root.

7. You may compile a different profile by changing the PLAYREADY_PROFILE variable (above) and also setting the DRM_BUILD_PROFILE variable.
   You should not use any DRM_BUILD_PROFILE values that are already pre-defined in drmfeatures.h.
   You will also need to update the codebase anywhere DRM_BUILD_PROFILE_LINUX is referenced.
   For example (assume myprofile.mk contains your profile and your desired value for DRM_BUILD_PROFILE is 901):
export PLAYREADY_PROFILE=myprofile.mk
export DRM_BUILD_PROFILE=901

