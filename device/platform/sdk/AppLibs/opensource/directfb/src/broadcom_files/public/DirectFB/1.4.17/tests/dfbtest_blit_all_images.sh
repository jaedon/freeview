#!/bin/bash

if [ $# -gt 0 ]
then
    IMAGES_DIR=$1
else
    IMAGES_DIR=/usr/local/share/directfb-1.4.15/images/
fi

if ! [ -d "$IMAGES_DIR" ]
then
    echo "Images dir $IMAGES_DIR doesn't exist!"
    exit
fi

echo "Decoding images from : $IMAGES_DIR"

find $IMAGES_DIR | grep -i -e "\.jpg"  -e "\.h264" -e "\.png" -e "\.mpg" | xargs -n 1 -t ./rundfb.sh  dfbtest_blit
