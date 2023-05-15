#!/bin/sh

HUMAX_STRIP=$1
FILE_NAME=$2

USER_APP_FILE=$(file -i $FILE_NAME | grep "application/x-executable" | grep "binary")

USER_APP_STRIP_OPTION="--strip-all"

if [ "$USER_APP_FILE" ]; then
	echo $HUMAX_STRIP $USER_APP_STRIP_OPTION $FILE_NAME
	$HUMAX_STRIP $USER_APP_STRIP_OPTION $FILE_NAME
else
	echo Skip strip $FILE_NAME
fi;
