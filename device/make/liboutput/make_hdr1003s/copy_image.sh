#! /bin/sh
mkdir -p ../LIB
mkdir -p ../image
mkdir -p ../image/tools
mkdir -p ../output

PROJECTNAME=hdr1003s
USERNAME=$(whoami)
PROJECT_OUTPUT_DIR=/nfsroot/${USERNAME}/${PROJECTNAME}

#copy platform packages
cp -dpR ../../../images/${PROJECTNAME}/platforms/debug ../image/
cp -dpR ../../../images/${PROJECTNAME}/libs/debug/libdriver.a ../image/debug/
cp -dpR ../../../images/${PROJECTNAME}/platforms/release ../image/
cp -dpR ../../../images/${PROJECTNAME}/libs/release/libdriver.a ../image/release/

#copy main.o file
mkdir -p ../image/object
cp -dpR ../../../apps/main/DEBUG ../image/object/
cp -dpR ../../../apps/main/RELEASE ../image/object/

#copy static lib
mkdir -p ../image/static_lib
cp -dpR ../../output/lib/* ../image/static_lib
rm ../image/static_lib/*.ld
rm ../image/static_lib/libdriver.a

#copy 3rd party lib
cp -dpR /nfsroot/${USERNAME}/${PROJECTNAME}/root/usr/lib ../image

#copy browser lib
cp -dpR /nfsroot/${USERNAME}/${PROJECTNAME}/root/usr/browser ../image
cp -dpR /nfsroot/${USERNAME}/${PROJECTNAME}/root/opt/share ../image

#copy squashfs
cp -dpR ../../../../model/tool/squashfs ../image/tools

#copy target_device
cp -dpR ../../../platform/sdk/os/rootfs/target_device ../image/tools


#delete .svn 
find ../LIB -type d \( -name CVS -o -name .svn \) | xargs rm -rf
find ../image -type d \( -name CVS -o -name .svn \) | xargs rm -rf
