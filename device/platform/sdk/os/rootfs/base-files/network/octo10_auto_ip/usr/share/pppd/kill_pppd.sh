#!/bin/sh

# kill pppd $interface process (ciyoon@humaxdigital.com)

ps | grep "pppd $1" | grep -v "grep" | while read S1 S2
do
	echo "before kill pppd if"
	if [ -n $S1 ] ;
	then
		echo "kill pppd process " $S1
		kill $S1
	fi
done
