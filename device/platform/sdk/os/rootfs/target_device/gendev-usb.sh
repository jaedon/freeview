#!/bin/sh

#const variable
max_buses=10
usb_major=189

buses=1
minor_num=0
i=1
targ_dir=$1

while [ "$buses" -le $max_buses ] 
do
	dir=$targ_dir/dev/bus/usb/$(seq --format "%03g" $buses $buses)
	$debug mkdir -m 775 -p $dir
	
	while [ "$i" -le 128 ]
	do
		name=$(seq --format "%03g" $i $i) 
		if [ ! -e $dir/$name ]; then
			sudo mknod -m 666 $dir/$name c $usb_major $minor_num
		fi
		i=$(expr $i + 1)
		minor_num=$(expr $minor_num + 1)
	done
	
	i=1
	buses=$(expr $buses + 1)
done
