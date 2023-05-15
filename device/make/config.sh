#!/bin/bash

# modify top .config file
ENABLE="=y"
DISABLE=" is not set"
PRODUCTIMAGE="CONFIG_PRODUCT_"
BOARDREVISION="CONFIG_BOARD_REVISION="
LISTIMAGETYPE="hwtest uocto upgrader factory octo octo20 netflix noapp baseplatform"
LISTIMAGEMODE="release"
LISTIMAGEFEATURE="develop"
LISTPROCESSTYPE="multi"
LISTBOARDREV="rev0.1 rev0.15 rev0.2 rev0.3 rev0.4 rev0.5 rev0.6 rev1.0"

EnableConfig(){
	Upper=$( echo $1 | tr '[:lower:]' '[:upper:]')
	if [ -z "$1" ]; then
		exit 1
	fi
	CONFIG=$PRODUCTIMAGE$Upper$ENABLE
	DISCONFIG="# "$PRODUCTIMAGE$Upper$DISABLE
	sed -e "s/$DISCONFIG/$CONFIG/g" $2 > $2.new && mv $2.new $2
}

EnableFeatureConfig(){
	Upper=$( echo $1 | tr '[:lower:]' '[:upper:]')
	if [ -z "$1" ]; then
		exit 1
	fi
	CONFIG=CONFIG_$Upper$ENABLE
	DISCONFIG="# "CONFIG_$Upper$DISABLE
	sed -e "s/$DISCONFIG/$CONFIG/g" $2 > $2.new && mv $2.new $2
}

EnableNetflix(){
	CONFIG=CONFIG_SUPPORT_NETFLIX_TEMP$ENABLE
	DISCONFIG="# "CONFIG_SUPPORT_NETFLIX_TEMP$DISABLE
	sed -e "s/$DISCONFIG/$CONFIG/g" $1 > $1.new && mv $1.new $1
}

ModifyConfig(){
	if [ -z "$1" ]; then
		exit 1
	fi
	REV=`cat "$2" | grep $BOARDREVISION`
	CONFIG=$BOARDREVISION\"$1\"
	sed -e "s/$REV/$CONFIG/g" $2 > $2.new && mv $2.new $2
}

ParseConfig() {
	for list in $1
	do 
		for config in $2
		do 
			if [ "$list" = "$config" ]; then
				echo $list
			fi
		done
	done
}

ReadAppName() {
	grep "CONFIG_PRODUCT_APP_NAME" $1 | awk -F= '{ print $2 }' | sed -e 's/[."]//g'
}

if [ -z "$1" ]; then
	echo no .config
	exit 1
fi

CONFIG=`echo "$1" | sed -e "s/\_/\ /g"`
PROJECT=`echo "$1" | awk -F_ '{ print $1 }'`

APPIMAGETYPE=$(ParseConfig "$LISTIMAGETYPE" "$CONFIG")
if [ "$APPIMAGETYPE" = "baseplatform" ]; then
	mkdir -p .$APPIMAGETYPE
	APPIMAGETYPE="octo"
fi
if [ -z "$APPIMAGETYPE" ]; then
	APPIMAGETYPE=$(ReadAppName $2)
fi
if [ "$APPIMAGETYPE" = "netflix" ]; then
	APPIMAGETYPE="octo"
	EnableNetflix $2
fi
APPIMAGETYPE="image_"$APPIMAGETYPE

APPIMAGEMODE=$(ParseConfig "$LISTIMAGEMODE" "$CONFIG")
if [ -z "$APPIMAGEMODE" ]; then
	APPIMAGEMODE="debug"
fi
APPIMAGEMODE="image_"$APPIMAGEMODE

APPIMAGEFEATURE=$(ParseConfig "$LISTIMAGEFEATURE" "$CONFIG")

APPPROCESSTYPE=$(ParseConfig "$LISTPROCESSTYPE" "$CONFIG")
if [ -z "$APPPROCESSTYPE" ]; then
	APPPROCESSTYPE="single"
fi
APPPROCESSTYPE=$APPPROCESSTYPE"_process"

BOARDREV=$(ParseConfig "$LISTBOARDREV" "$CONFIG")

echo Project name : $PROJECT
echo Application type : $APPIMAGETYPE
echo Apllication process type : $APPPROCESSTYPE
echo Application mode : $APPIMAGEMODE
echo Application feature : $APPIMAGEFEATURE
if [ ! -z "$BOARDREV" ]; then
	echo Board Rev : $BOARDREV
fi

# Read .config 
if [ -z "$2" ]; then
   FILE="/dev/stdin"
else
   FILE="$2"
   # make sure file exist and readable
   if [ ! -f $FILE ]; then
  	echo "$FILE : does not exists"
  	exit 1
   elif [ ! -r $FILE ]; then
  	echo "$FILE: can not read"
  	exit 2
   fi
fi

EnableConfig $APPIMAGETYPE $2
EnableConfig $APPIMAGEMODE $2
if [ ! -z "$APPIMAGEFEATURE" ]; then
	EnableFeatureConfig $APPIMAGEFEATURE $2
fi
EnableConfig $APPPROCESSTYPE $2
if [ ! -z "$BOARDREV" ]; then
	ModifyConfig $BOARDREV $2
fi
