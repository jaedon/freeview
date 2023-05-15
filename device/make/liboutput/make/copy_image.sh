#! /bin/sh
mkdir ../LIB
mkdir ../image
mkdir ../image/tools
mkdir ../output

PROJECTNAME=udfoxhd
USERNAME=$(whoami)
PROJECT_OUTPUT_DIR=/nfsroot/${USERNAME}/${PROJECTNAME}

#copy platform packages
cp -dpR ../../images/${PROJECTNAME}/platforms/debug ../image/
cp -dpR ../../images/${PROJECTNAME}/platforms/release ../image/

#copy rootfs
sudo cp -dpR ${PROJECT_OUTPUT_DIR}/root ../image/root

#copy main.o file
mkdir ../image/object
cp -dpR ../../../main/make/DEBUG ../image/object/
cp -dpR ../../../main/make/RELEASE ../image/object/

#copy static lib
mkdir ../image/static_lib
cp -dpR ../../output/lib/* ../image/static_lib
rm ../image/static_lib/*.ld

#copy 3rd party lib
cp -dpR /nfsroot/${USERNAME}/${PROJECTNAME}/root/usr/lib ../image

#copy squashfs
cp -dpR ../../../../model/tool/squashfs ../image/tools
#copy target_device
cp -dpR ../../../platform/sdk/os/rootfs/target_device ../image/tools


#delete .svn 
find ../LIB -type d \( -name CVS -o -name .svn \) | xargs rm -rf
find ../image -type d \( -name CVS -o -name .svn \) | xargs rm -rf
