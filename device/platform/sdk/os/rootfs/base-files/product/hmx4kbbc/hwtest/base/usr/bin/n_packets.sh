if [ "$1" == "" ] ; then
  echo "... : $0 ... [delay]"
  echo ".) $0 eth0 3 "
  exit 1
fi
if [ "$2" == "" ] ; then delay=3 ; else delay=$2 ; fi

echo ".. : rx(Kbit/Sec) / tx(Kbit/Sec)"
while ( true ) ; do
  rx1=`grep $1 /proc/net/dev | awk '{print $3}'`
  tx1=`grep $1 /proc/net/dev | awk '{print $11}'`
  sleep $delay
  rx2=`grep $1 /proc/net/dev | awk '{print $3}'`
  tx2=`grep $1 /proc/net/dev | awk '{print $11}'`

  # 1 packet = minimum 64-byte
  rx3=$(((rx2-rx1)*64*8/delay))
  tx3=$(((tx2-tx1)*64*8/delay))

  echo "`date '+%k:%M:%S'` : $rx3 / $tx3"
done

