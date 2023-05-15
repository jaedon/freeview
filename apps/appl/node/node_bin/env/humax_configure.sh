#echo ARG1 is $1 and ARG2 is $2

export HUMAX_CROSS=$1
export RESTNODE_SRC_DIR=$2

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
./configure --without-snapshot --dest-os=linux --dest-cpu=mipsel --prefix=./ --with-mips-float-abi=hard
