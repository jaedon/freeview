#!/bin/bash

HUMAX_APP_STAGGING_TOP_DIR=${PWD}/_gen_rootfs
HUMAX_APP_STAGGING_ROOT_DIR=${HUMAX_APP_STAGGING_TOP_DIR}/root
HUMAX_WEBAPP_STAGGING_TARGET_DIR=${HUMAX_APP_STAGGING_TOP_DIR}/root/usr/browser/webapps
YUI_COMPRESSOR_DIR=${PWD}/../../model/tool/yuicompressor

function __minify_js()
{
	JS_SRCS=$(find $HUMAX_WEBAPP_STAGGING_TARGET_DIR/ -name *.js -not -path "$HUMAX_WEBAPP_STAGGING_TARGET_DIR/lib/ext/*")

	for i in ${JS_SRCS}; do
		echo ${i}		
#		${PWD}/jsmin.py < ${i} > ${i}_
		java -jar $YUI_COMPRESSOR_DIR/yuicompressor-2.4.7.jar --type js ${i} > ${i}_
		if [ -s ${i}_ ]
		then			
			mv ${i}_ ${i}
		else
			rm ${i}_
		fi
	done;
}

function __minify_css()
{
	CSS_SRCS=$(find $HUMAX_WEBAPP_STAGGING_TARGET_DIR/ -name *.css)

	for i in ${CSS_SRCS}; do
		echo ${i}		
		java -jar $YUI_COMPRESSOR_DIR/yuicompressor-2.4.7.jar --type css ${i} > ${i}_
		if [ -s ${i}_ ]
		then			
			mv ${i}_ ${i}
		else
			rm ${i}_
		fi
	done;
}

echo "START JS MINIFY"
__minify_js;
echo "END JS MINIFY"

echo "START CSS MINIFY"
__minify_css;
echo "END CSS MINIFY"
