if [ "$1" == "" ] ; then
  echo "... : $0 ... [delay]"
  echo ".) $0 eth0 3 "
  exit 1
fi
if [ "$2" == "" ] ; then delay=3 ; else delay=$2 ; fi

echo ".. : rx(Kbit/Sec) / tx(Kbit/Sec)"
while ( true ) ; do
  rx1=`grep $1 /proc/net/dev | awk '{print $2}'`
  tx1=`grep $1 /proc/net/dev | awk '{print $10}'`
  sleep $delay
  rx2=`grep $1 /proc/net/dev | awk '{print $2}'`
  tx2=`grep $1 /proc/net/dev | awk '{print $10}'`

  # 1024/8 == 128 
  rx3=$(((rx2-rx1)/128/delay))
  tx3=$(((tx2-tx1)/128/delay))

  echo "`date '+%k:%M:%S'` : $rx3 / $tx3"
done

