#!/bin/bash

HUMAX_APP_STAGGING_TOP_DIR=${PWD}/_gen_rootfs
HUMAX_APP_STAGGING_ROOT_DIR=${HUMAX_APP_STAGGING_TOP_DIR}/root
HUMAX_NODE_STAGGING_TARGET_DIR=${HUMAX_APP_STAGGING_TOP_DIR}/root/home/node
YUI_COMPRESSOR_DIR=${PWD}/../../model/tool/yuicompressor

function __minify_js()
{
	JS_SRCS=$(find $HUMAX_NODE_STAGGING_TARGET_DIR/ -name *.js)

	for i in ${JS_SRCS}; do
		echo ${i}		
#		${PWD}/jsmin.py < ${i} > ${i}_
		java -jar $YUI_COMPRESSOR_DIR/yuicompressor-2.4.7.jar --type js ${i} > ${i}_
		mv ${i}_ ${i}
	done;
}

function __minify_css()
{
	CSS_SRCS=$(find $HUMAX_NODE_STAGGING_TARGET_DIR/ -name *.css)

	for i in ${CSS_SRCS}; do
		echo ${i}		
		java -jar $YUI_COMPRESSOR_DIR/yuicompressor-2.4.7.jar --type css ${i} > ${i}_
		mv ${i}_ ${i}
	done;
}

echo "NODE :: START JS MINIFY"
__minify_js;
echo "NODE :: END JS MINIFY"

echo "NODE :: START CSS MINIFY"
__minify_css;
echo "NODE :: END CSS MINIFY"
