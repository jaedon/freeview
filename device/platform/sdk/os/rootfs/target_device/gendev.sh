#!/bin/sh

#debug=echo
#base=/home/jdshin/nfsroot/root
base=$1

egrep -v '^[[:space:]]*(#|$)' $base/dev/device_table.txt | \
while read TYPE name arg1 arg2 arg3 arg4 arg5 arg6; do
	name=$base$name
	case $TYPE in
	'file')
		$debug touch $name
		$debug chmod $arg2 $name
		;;
	'dir')
		$debug mkdir -m $arg1 -p $name
		;;
	'nod')
		if [ ! -e $name ] ; then 
			sudo mknod -m $arg1 $name $arg4 $arg5 $arg6
		fi
		;;
	'slink')
		$debug ln -sf $arg1 $name
		;;
	'pipe')
		;;
	'sock')
		;;
	*)
		exit 1
		;;
	esac
done
