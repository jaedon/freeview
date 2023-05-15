#!/bin/bash
# example output file: "${CMAKE_CURRENT_BINARY_DIR}/hash.txt"
# example configuration file "${APP_DATA_DIR}etc/conf/common.xml"
# example binary file : "${CMAKE_CURRENT_BINARY_DIR}/netflix"
OUTPUTFILE=$1
CONFFILE=$2
BINARY=$3

if [ -x /sbin/md5 ] ; then
    HASH=$(md5 -q $BINARY)
else
    HASH=$(md5sum $BINARY | sed -e 's, .*,,')
fi

echo $HASH > $OUTPUTFILE

# find the binary_hash entry in common.xml and replace the value with hash
TMPFILE=$(mktemp /tmp/hash.XXXXXX)
sed -e "s/<binary_hash>.*</<binary_hash>$HASH</g" "$CONFFILE" > "$TMPFILE"
mv -f "$TMPFILE" "$CONFFILE" || rm -f "$TMPFILE"


