############################################################################
#     Copyright (c) 2006-2010, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
############################################################################
#
# DESCRIPTION:
#   Script to test that we can build the various methods
#
############################################################################
#!/bin/bash

echo "*************************************************************************"
echo "                     Doing initial cleanup....                           "
echo "*************************************************************************"
sleep 1
make distclean
make -C ../osapi distclean

echo "*************************************************************************"
echo "                     Building: Standalone i386                           "
echo "*************************************************************************"
sleep 2
make test NETAPP_STANDALONE=y NO_WIFI_INVITE=y
if [ ! -f stage/usr/local/bin/NetAppTest ] ; then 
    echo "Build Failed"
    exit
fi
make distclean NETAPP_STANDALONE=y NO_WIFI_INVITE=y


echo "*************************************************************************"
echo "                   Building: Standalone i386 (No WiFi)                   "
echo "*************************************************************************"
sleep 2
make test NETAPP_STANDALONE=y NO_WIFI=y
if [ ! -f stage/usr/local/bin/NetAppTest ] ; then 
    echo "Build Failed"
    exit
fi
make distclean NETAPP_STANDALONE=y NO_WIFI=y


#
echo "*************************************************************************"
echo "                        Building: Standalone MIPS                        "
echo "*************************************************************************"
sleep 2
make test NETAPP_STANDALONE=y CROSS_COMPILE=mipsel-uclibc-
if [ ! -f stage/usr/local/bin/NetAppTest ] ; then 
    echo "Build Failed"
    exit
fi
make distclean NETAPP_STANDALONE=y CROSS_COMPILE=mipsel-uclibc-


#
echo "*************************************************************************"
echo "               Building: Standalone MIPS (External OpenSSL)              "
echo "*************************************************************************"
sleep 2
make test NETAPP_STANDALONE=y EXTERNAL_LIBS=-lz EXTERNAL_OPENSSL=1 \
    EXTERNAL_OPENSSL_BASE=$PWD/../../stage/usr/local CROSS_COMPILE=mipsel-uclibc- \
    EXTERNAL_CPPFLAGS="-DNETAPP_DNS_FILE='\"/etc/resolv.conf\"'"
    
if [ ! -f stage/usr/local/bin/NetAppTest ] ; then 
    echo "Build Failed"
    exit
fi
make distclean NETAPP_STANDALONE=y EXTERNAL_LIBS=-lz EXTERNAL_OPENSSL=1 EXTERNAL_OPENSSL_BASE=$PWD/../../stage/usr/local CROSS_COMPILE=mipsel-uclibc-


echo "*************************************************************************"
echo "                   Building: Standalone MIPS (no WiFi)                   "
echo "*************************************************************************"
sleep 2
make test NETAPP_STANDALONE=y CROSS_COMPILE=mipsel-uclibc- NO_WIFI=y
if [ ! -f stage/usr/local/bin/NetAppTest ] ; then 
    echo "Build Failed"
    exit
fi
make distclean NETAPP_STANDALONE=y CROSS_COMPILE=mipsel-uclibc- NO_WIFI=y


echo "*************************************************************************"
echo "                          Building: BD                                   "
echo "*************************************************************************"
sleep 2
make -C ../osapi install
make test 
if [ ! -f ../../stage/usr/local/bin/NetAppTest ] ; then 
    echo "Build Failed"
    exit
fi
make distclean


