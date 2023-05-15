#echo ARG1=$1, ARG2=$2, ARG3=$3, ARG4=$4

export HUMAX_CROSS=$1
export RESTNODE_SRC_DIR=$2
export HUMAX_APP_STAG_DIR=$3
export AP_STATIC_LIB_DIR=$4

#echo HUMAX_CROSS is $HUMAX_CROSS
#echo RESTNODE_SRC_DIR is $RESTNODE_SRC_DIR

export CC=${HUMAX_CROSS}gcc
export CXX=${HUMAX_CROSS}g++
export AR=${HUMAX_CROSS}ar
export RANLIB=${HUMAX_CROSS}ranlib
export LINK=$CXX
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:${HUMAX_CROSS}ld
export GYP_DEFINES="target_arch=mipsel v8_target_arch=mipsel mips_arch_variant=mips32r1"

#echo CC is $CC

cd ${RESTNODE_SRC_DIR}
make --jobs=2
