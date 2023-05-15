#!/bin/sh

HUMAX_STRIP=$1
FILE_NAME=$2

KO_FILE=$(file -i $FILE_NAME | grep "application/x-object" | grep "binary")
SO_FILE=$(file -i $FILE_NAME | grep "application/x-sharedlib" | grep "binary")
USER_APP_FILE=$(file -i $FILE_NAME | grep "application/x-executable" | grep "binary")

KO_STRIP_OPTION="--strip-unneeded"
SO_STRIP_OPTION="--strip-unneeded"
USER_APP_STRIP_OPTION="--strip-all"


if [ "$KO_FILE" ]; then
	echo $HUMAX_STRIP $KO_STRIP_OPTION $FILE_NAME
	$HUMAX_STRIP $KO_STRIP_OPTION $FILE_NAME
elif [ "$USER_APP_FILE" ]; then
	echo $HUMAX_STRIP $USER_APP_STRIP_OPTION $FILE_NAME
	$HUMAX_STRIP $USER_APP_STRIP_OPTION $FILE_NAME
elif [ "$SO_FILE" ]; then
	if [[ $FILE_NAME == *"libViewRightWeb"* ]]; then
		echo Skip strip $FILE_NAME
	else
		echo $HUMAX_STRIP $SO_STRIP_OPTION $FILE_NAME
		$HUMAX_STRIP $SO_STRIP_OPTION $FILE_NAME
	fi
else
	echo Skip strip $FILE_NAME
fi;
