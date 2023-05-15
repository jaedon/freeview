#!/bin/bash

if [[ "$1" == *"/usr" ]]; then
	for d in $1/*; do
		if [[ "$d" == *"lib" ]]; then
			for l in $d/*; do
				if [[ "$l" == *"/libclib"* ]]; then
					continue
				elif [[ "$l" == *"/libdlib"* ]]; then
					continue
				elif [[ "$l" == *"/libhlib"* ]]; then
					continue
				elif [[ "$l" == *"/libmemory"* ]]; then
					continue
				elif [[ "$l" == *"/liboapi"* ]]; then
					continue
				elif [[ "$l" == *"/libondk"* ]]; then
					continue
				elif [[ "$l" == *"/librlib"* ]]; then
					continue
				elif [[ "$l" == *"/libsilib"* ]]; then
					continue
				elif [[ "$l" == *"/libdi_"* ]]; then
					continue
				elif [[ "$l" == *"/libmi"* ]]; then
					continue
				elif [[ "$l" == *"/libdrv_"* ]]; then
					continue
				elif [[ "$l" == *"/libcutest"* ]]; then
					continue
				fi
				sudo rm -rf $l
			done
			continue
		elif [[ "$d" == *"bin" ]]; then
			for l in $d/*; do
				if [[ "$l" == *"dfbmaster" ]]; then
					continue
				fi
				sudo rm -rf $l
			done
			continue
		fi
		sudo rm -rf $d
	done
elif [[ "$1" == *"/home" ]]; then
	for d in $1/*; do
		if [[ "$d" == *"/obama" ]]; then
			continue
		fi
		sudo rm -rf $d
	done
else
	sudo rm -rf $1
fi
