#!/bin/sh
#version :  v1.0.7 2012-10-24
#List of Functions:
#wifiInit
#wifiTerm
#wifiList 
#wifiAPInfo [SSID]
#wifiConnect [SSID][Password]
#wifiConnectIP [SSID][Password][IP Address]
#wifiConnected
#wifiStatus
#wifiDisconnect
#wifiMCSindex [index]
#wifiDataRate [Protocol] [index] [Ant #]
#wifiSetAntenna [antenna #]
#wifiSetChannel [channel][band][bandwidth][ctl sideband]
#wifiIperf2way [Server IP][timeSecs]
#wifiIperfSend [Server IP][timeSecs]
#wifiIperfReceive
#wifiIperfSilentReceive
#wifiCWWave [channel]
#wifiCWWaveStop
#wifiRxMode [channel][bandwidth][ctl sideband][antenna #][protocol][mcs index/data rate]<IQ nxn MAC address> 
#wifiRxModeStop
#wifiRxMeasure
#wifiRxPER [channel][bandwidth][ctl sideband][antenna #][protocol][mcs index/data rate]<IQ nxn MAC address>
#wifiRxPER_hmx [Packet Counters] [channel][bandwidth][ctl sideband][antenna #][protocol][mcs index/data rate]<IQ nxn MAC address>
#wifiTxMode [inter-packet gap][bytes per frame][channel][bandwidth][ctl sideband][antenna #][power][protocol][mcs index/data rate] 
#wififccTxMode [inter-packet gap][bytes per frame][channel][bandwidth][ctl sideband][antenna #][power][protocol][mcs index/data rate] 
#wifiTxModeStop
#wifiSetPower [power]
#wifiReset
#wifiLogCheck [iperf log filename]

source ./env.sh

########################################################################
#                               Functions                              #
########################################################################

#=======================================================================
# Syntax: wifiInit
#
# Makes sure that the drivers are installed and the module is in station
# mode.
#
# This function must succeed if the WiFi driver is already up and ready
#=======================================================================
wifiInit()
{
	if [ -e $INSTALL ]; then
		if [ $1 = "prd" ]; then
			echo "Selected Production Driver."
			driver="prd"
			if [ -e $WIFI_CUR_DRIVER_TXT ]; then
				rm -rf $WIFI_CUR_DRIVER_TXT
			fi
			echo $driver >> $WIFI_CUR_DRIVER_TXT
		elif [ $1 = "mfg" ]; then
			echo "Selected MFG Driver."
			driver="mfg"
			if [ -e $WIFI_CUR_DRIVER_TXT ]; then
				rm -rf $WIFI_CUR_DRIVER_TXT
			fi 
			echo $driver >> $WIFI_CUR_DRIVER_TXT
		else
			echo "Selection Error : wifi_hmx.sh init [prd/mfg]"
			driver=""
			return
		fi
		
		$INSTALL $driver
		
		# Make sure interface is down during configuration
		ifconfig $IFNAME down
		
		# Set to Country Code
		$WL_DUT country $WIFI_COUNTRY_CODE  
		
		# Set to station mode
		$WL_DUT ap 0
		exitifError $? $WIFI_ERR_CMD "Command '$WL_DUT ap 0' failed"
		
		# Reinitialize with new settings
		$WL_DUT up
		exitifError $? $WIFI_ERR_CMD "Command '$WL_DUT up' failed"
		
		# Up the device
		ifconfig $IFNAME up
	else
		exitError $WIFI_ERR_NOT_FOUND "Not Found install.sh"
	fi
}

#=======================================================================
# syntax: wifiTerm
#
# Reset and mark adapter down (disabled)
#=======================================================================
wifiTerm()
{
  $WL_DUT down
  
##  if [ -n "`ps aux | grep bcmdl`" ]; then
	  if [ -n "`ps | grep bcmdl`" ]; then
    killall bcmdl
    sleep 1
  fi
  
  if [ -n "`lsmod | grep $WIFI_DRV_NAME`" ]; then
    rmmod $WIFI_DRV_NAME
    sleep 1
  fi

  if [ -n "`lsmod | grep $WIFI_DBUS_NAME`" ]; then
  	rmmod $WIFI_DBUS_NAME 
    sleep 1
  fi

  printInfo "Adapters down. Drivers Uninstalled"
}

#=======================================================================
# Syntax: wifiReset
#
# Resets the wifi Module to default state
#
# Inputs:
# none
#=======================================================================
wifiReset()
{
  wifiTerm
  sleep 1
  if [ -e $WIFI_CUR_DRIVER_TXT ]; then
    CUR_DRIVER=$(cat $WIFI_CUR_DRIVER_TXT)
    echo $CUR_DRIVER
    if [ $CUR_DRIVER = "prd" ]; then
      wifiInit prd
    else
      wifiInit mfg
    fi
  else
    wifiInit
  fi 
}

#=======================================================================
# Syntax: wifiList
#
# Prints the list of the SSIDs found
# This command shall only print the SSID names.
# The command wifiAPInfo can be used to retrieve more details about each SSID
#=======================================================================
wifiList()
{
  $WL_DUT down
  $WL_DUT country $WIFI_COUNTRY_CODE  
  $WL_DUT ap 0
  $WL_DUT band auto
  $WL_DUT up
  sleep 1
  
  printInfo "Please wait for a while..."
  $WL_DUT scan
  sleep $SCAN_TIMEOUT
  
  SSID_LIST=`$WL_DUT scanresults | grep "SSID: \"*\"" | cut -d' ' -f2- | cut -d"\"" -f2`
  
  printInfo "Networks found:"
  #Network list prints here"
  for ssid in $SSID_LIST
  do 
    printInfo $ssid
  done
}

#=======================================================================
# Syntax: wifiAPInfo [SSID]
#
# Prints the details for the given SSID
#
# Inputs:
# - SSID: SSID string of the network to retrieve details for
#
# This command shall display the following information for the given SSID:
# - SSID
# - RSSI
# - SNR
# - Noise
# - Channel
# - BSSID
# - Bandwidth
# - Encryption type
#=======================================================================
wifiAPInfo()
{
  #checkNumParams 1 ${#@} $FUNCNAME
  
  BAND=`$WL_DUT band`
  SSID=$1
  
  $WL_DUT band $BAND
  sleep 1
  printInfo "Please wait for a while..."
  
  $WL_DUT scan $SSID
  
  second=0
  while [ $second -lt $SCAN_TIMEOUT ]; do
    sleep 1
    let second=$second+1
    $WL_DUT scanresults &> /dev/null
    if [ $? -eq 0 ]; then
      break
    fi
  done
  
  if [ -z "`$WL_DUT scanresults`" ]; then
    exitError $WIFI_ERR_NOT_FOUND "Not Found $SSID AP"
  fi
  
  RSSI=`$WL_DUT scanresults | grep -o "RSSI: .[0-9]* dBm" | cut -d " " -f2`
  SNR=`$WL_DUT scanresults | grep -o "SNR: [0-9]* dB" | cut -d " " -f2`
  NOISE=`$WL_DUT scanresults | grep -o "noise: .[0-9]* dBm" | cut -d " " -f2`
  if [ -n "`$WL_DUT scanresults | grep -o "802.11N Capable:"`" ]; then
    CHANNEL=`$WL_DUT scanresults | grep "Chanspec:" | grep -o "channel [0-9]* " | cut -d " " -f2`
    BANDWIDTH=`$WL_DUT scanresults | grep "Chanspec:" | grep -o "[0-9]*MHz"`
  else
    CHANNEL=`$WL_DUT scanresults | grep -o "Channel:.*" | cut -d " " -f2`
    BANDWIDTH=default
  fi
  BSSID=`$WL_DUT scanresults | grep -o BSSID:*..[a-zA-Z0-9]:*..[a-zA-Z0-9]:*..[a-zA-Z0-9]:*..[a-zA-Z0-9]:*..[a-zA-Z0-9]:*..[a-zA-Z0-9] | cut -d " " -f2`
  
  if [ -n "`$WL_DUT scanresults | grep -o "Capability:.*WEP.*"`" ]; then
    if [ -n "`$WL_DUT scanresults | grep -o "RSN:"`" ] ; then
      ENC_TYPE=WPA2-PSK
      if [ -n "`$WL_DUT scanresults | grep -o "AES-CCMP"`" ]; then
        SEC_TYPE=AES
      else
        SEC_TYPE=TKIP
      fi
    elif [ -n "`$WL_DUT scanresults | grep -o "WPA:"`" ] ; then
      ENC_TYPE=WPA-PSK
      if [ -n "`$WL_DUT scanresults | grep -o "AES-CCMP"`" ]; then
        SEC_TYPE=AES
      else
        SEC_TYPE=TKIP
      fi
    else 
      ENC_TYPE=WEP
      SEC_TYPE=
    fi
  else
    ENC_TYPE=None
    SEC_TYPE=
  fi
  
  #AP info prints here
  printInfo "--------- $SSID info ---------"
  printInfo "RSSI : $RSSI dBm"
  printInfo "SNR : $SNR dB"
  printInfo "Noise : $NOISE dBm"
  printInfo "Channel : $CHANNEL"
  printInfo "BSSID : $BSSID"
  printInfo "Bandwidth : $BANDWIDTH"
  printInfo "Encrytion type : $ENC_TYPE $SEC_TYPE"

}
#=======================================================================
# Syntax: wifiConnect [SSID] [Password]
#
# Joins the given SSID. open, WEP, WPA and WPA2 required
# This command must be able to detect the encryption system for the SSID
# provided and connect to it.
#
# Inputs:
# - SSID: SSID string of the network to join
# - Password: Password, if required, to join the network
#=======================================================================
wifiConnect()
{
  $WL_DUT down
  $WL_DUT country $WIFI_COUNTRY_CODE  
  $WL_DUT mimo_bw_cap 1    #  using  bandwidth  40Mhz  both 2.4Ghz and 5Ghz
  $WL_DUT obss_coex 1   #   5.2.35 Basic Association in 802.11n Environment /  HT Capability IE - 20/40 MHz Supported in 2.4 GHz for  Wifi Alliance  Cerfitication      
  $WL_DUT up
  sleep 1
  $WL_DUT auth 0
  $WL_DUT infra 1
  printInfo "Please wait for a while..."

  SSID=$1
  $WL_DUT scan $SSID
  
  second=0
  while [ $second -lt $SCAN_TIMEOUT ]; do
    sleep 1
    let second=$second+1
    $WL_DUT scanresults &> /dev/null
    if [ $? -eq 0 ]; then
      printInfo "Scan within $second seconds"
      break
    fi
  done

  if [ -z "`$WL_DUT scanresults`" ]; then
    exitError $WIFI_ERR_NOT_FOUND "Not Found $SSID AP"
  fi

  if [ -n "`$WL_DUT scanresults | grep -o "Capability: ESS.*" | grep "WEP"`" ]; then
    if [ -n "`$WL_DUT scanresults | grep -o "WPA:"`" ] ; then
      ENC_TYPE=WPA-PSK
      if [ -n "`$WL_DUT scanresults | grep -o "AES-CCMP"`" ]; then
        WSEC=4
      else
        WSEC=2
      fi
    elif [ -n "`$WL_DUT scanresults | grep -o "RSN:"`" ] ; then
      ENC_TYPE=WPA2-PSK
      if [ -n "`$WL_DUT scanresults | grep -o "AES-CCMP"`" ]; then
        WSEC=4
      else
        WSEC=2
      fi
    else
      ENC_TYPE=WEP
    fi
  else
    ENC_TYPE=None
  fi
  
  if [ $ENC_TYPE = "None" ]; then
    #checkNumParams 1 ${#@} $FUNCNAME
    PASSWORD=
  else 
    #checkNumParams 2 ${#@} $FUNCNAME
    PASSWORD=$2
  fi
  

  ifconfig $IFNAME up
  
  case $ENC_TYPE in
  "None")
    printInfo "Mode Open"
    $WL_DUT wsec 0
    $WL_DUT sup_wpa 1
    $WL_DUT wpa_auth 0
    option=""
    ;;
  "WEP")
    printInfo "Mode WEP"
    $WL_DUT wsec 1
    $WL_DUT sup_wpa 1
    $WL_DUT wpa_auth 0
    $WL_DUT addwep 0 $PASSWORD
    option=""
    ;;
  "WPA-PSK")
    printInfo "Mode WPA-PSK :$WSEC"
    $WL_DUT wsec $WSEC
    $WL_DUT sup_wpa 1
    $WL_DUT wpa_auth 4
    $WL_DUT set_pmk $PASSWORD
    option="imode bss amode wpapsk"
    ;;
  "WPA2-PSK")
    printInfo "Mode WPA2-PSK :$WSEC"
    $WL_DUT wsec $WSEC
    $WL_DUT sup_wpa 1
    $WL_DUT wpa_auth 128
    $WL_DUT set_pmk $PASSWORD
    option="imode bss amode wpa2psk"
    ;;
  *)  ;;
  esac
  $WL_DUT join $SSID $option
  second=0
  while [ $second -lt $JOIN_TIMEOUT ]; do
    sleep 1
    let second=$second+1
    $WL_DUT assoc | grep $SSID &> /dev/null
    if [ $? -eq 0 ]; then
       printInfo "Joined within $second seconds"
       break
    fi
  done

  if [ -n "`$WL_DUT status | grep "Not associated."`" ] ; then
    exitError $WIFI_ERR_NOT_CONNECTED "Failed connected"
  fi

  /sbin/udhcpc --interface=$IFNAME --quit --now --timeout=10 --retries=2 &> /dev/null
  exitifError $? $WIFI_ERR_IP "Getting dhcp IP address failed"

  printInfo "Connection established"
  # Print status
  wifiStatus
}

#=======================================================================
# Syntax: wifiConnectIP [SSID] [Password] [IP Address]
#
# Joins the given SSID with a specified static IP address.
# open, WEP, WPA and WPA2 security required
# This command must be able to detect the encryption system for the SSID
# provided and connect to it.
#
# Inputs:
# - SSID: SSID string of the network to join
# - Password: Password to join the network
# - IP Address: Static IP address of device on the network
#=======================================================================
wifiConnectIP()
{
        $WL_DUT down
  $WL_DUT country $WIFI_COUNTRY_CODE  
  $WL_DUT mimo_bw_cap 1    #  using  bandwidth  40Mhz  both 2.4Ghz and 5Ghz
  $WL_DUT obss_coex 1   #   5.2.35 Basic Association in 802.11n Environment /  HT Capability IE - 20/40 MHz Supported in 2.4 GHz for  Wifi Alliance  Cerfitication      
  sleep 1
  $WL_DUT auth 0
  $WL_DUT infra 1
  $WL_DUT up
  SSID=$1
  printInfo "Please wait for a while..."

  $WL_DUT scan $SSID
  
  second=0
  while [ $second -lt $SCAN_TIMEOUT ]; do
    sleep 1
    let second=$second+1
    $WL_DUT scanresults &> /dev/null
    if [ $? -eq 0 ]; then
      printInfo "Scan within $second seconds"
      break
    fi
  done
        
  if [ -z "`$WL_DUT scanresults`" ]; then
    exitError $WIFI_ERR_NOT_FOUND "Not Found $SSID AP"
  fi

  if [ -n "`$WL_DUT scanresults | grep -o "Capability: ESS.*" | grep "WEP"`" ]; then
    if [ -n "`$WL_DUT scanresults | grep -o "WPA:"`" ] ; then
      ENC_TYPE=WPA-PSK
      if [ -n "`$WL_DUT scanresults | grep -o "AES-CCMP"`" ]; then
        WSEC=4
      else
        WSEC=2
      fi
    elif [ -n "`$WL_DUT scanresults | grep -o "RSN:"`" ] ; then
      ENC_TYPE=WPA2-PSK
      if [ -n "`$WL_DUT scanresults | grep -o "AES-CCMP"`" ]; then
        WSEC=4
      else
        WSEC=2
      fi
    else
      ENC_TYPE=WEP
    fi
  else
    ENC_TYPE=None
  fi

  if [ $ENC_TYPE = "None" ]; then
    #checkNumParams 2 ${#@} $FUNCNAME
    IP_ADDRESS=$2
  else
    #checkNumParams 3 ${#@} $FUNCNAME
    PASSWORD=$2
    IP_ADDRESS=$3
  fi

  ifconfig $IFNAME up

  case $ENC_TYPE in
  "None")
    printInfo "Mode Open"
    $WL_DUT wsec 0
    $WL_DUT sup_wpa 1
    $WL_DUT wpa_auth 0
    option=""
    ;;
  "WEP")
    printInfo "Mode WEP"
    $WL_DUT wsec 1
    $WL_DUT sup_wpa 1
    $WL_DUT wpa_auth 0
    $WL_DUT addwep 0 $PASSWORD
    option=""
    ;;
  "WPA-PSK")
    printInfo "Mode WPA-PSK :$WSEC"
    $WL_DUT wsec $WSEC
    $WL_DUT sup_wpa 1
    $WL_DUT wpa_auth 4
    $WL_DUT set_pmk $PASSWORD
    option="imode bss amode wpapsk"
    ;;
  "WPA2-PSK")
    printInfo "Mode WPA2-PSK :$WSEC"
    $WL_DUT wsec $WSEC
    $WL_DUT sup_wpa 1
    $WL_DUT wpa_auth 128
    $WL_DUT set_pmk $PASSWORD
    option="imode bss amode wpa2psk"
    ;;
  *)      ;;
  esac
  $WL_DUT join $SSID $option
  
  second=0
  while [ $second -lt $JOIN_TIMEOUT ]; do
    # Wait for join to finish
    sleep 1
    let second=$second+1
    
    $WL_DUT assoc | grep $SSID &> /dev/null
    if [ $? -eq 0 ]; then
       printInfo "Joined within $second seconds"
       break
    fi
  done
  
  if [ -n "`$WL_DUT status | grep "Not associated."`" ] ; then
    exitError $WIFI_ERR_NOT_CONNECTED "Failed connected"
  fi
  
  ifconfig $IFNAME $IP_ADDRESS up
  
  printInfo "Connection established"
  
  # Print status
  wifiStatus

}

#=======================================================================
# Syntax: wifiConnected
#
# Returns OK if connected, error if not
#=======================================================================
wifiConnected()
{
  IP=`ifconfig $IFNAME | grep "inet addr" | cut -d ":" -f2 | cut -d " " -f1`
  if [ -n $IP ]; then
    printInfo "Client IP : $IP" 
  else
    exitError $WIFI_ERR_IP "Not Allocate IP address"
  fi

  AP_ADDRESS=`echo $IP | grep -o "[0-9]*\.[0-9]*\.[0-9]*\."`1
  printInfo "AP Address : $AP_ADDRESS"

  ping $AP_ADDRESS -c 4

  printInfo "WiFi connected to $SSID"

  return $WIFI_OK
}

#=======================================================================
# Syntax: wifiStatus
#
# Prints the following details for the current connection:
# - SSID
# - BSSID
# - IP
# - RSSI global
# - RSSI antenna 0
# - RSSI antenna 1
# - Noise level
# - MCS index
# - TX power level
#=======================================================================
wifiStatus()
{
  if [ `$WL_DUT status | grep "associated."` ]; then
    exitError $WIFI_ERR_NOT_CONNECTED "Failed connected"
  fi  

  SSID=`$WL_DUT status | grep "SSID: \"*\"" | cut -d " " -f2-`
  BSSID=`$WL_DUT status | grep -o BSSID:*..[a-zA-Z0-9]:*..[a-zA-Z0-9]:*..[a-zA-Z0-9]:*..[a-zA-Z0-9]:*..[a-zA-Z0-9]:*..[a-zA-Z0-9]`
  IP=`ifconfig $IFNAME | grep "inet addr" | cut -d ":" -f2 | cut -d " " -f1`
  RSSI=`$WL_DUT status | grep -o "RSSI: .[0-9]* dBm" | cut -d " " -f2`
  RSSI_ANT0=`$WL_DUT status | grep -o "rssi\[0\] .[0-9]*" | cut -d " " -f2`
  RSSI_ANT1=`$WL_DUT status | grep -o "rssi\[1\] .[0-9]*" | cut -d " " -f2`
  NOISE=`$WL_DUT status | grep -o "noise: .[0-9]*" | cut -d " " -f2`
  MCSINDEX=`$WL_DUT nrate`
  TXPOWER=`$WL_DUT txpwr1`

  printInfo "SSID:    $SSID"
  printInfo "BSSID:   $BSSID"
  printInfo "IP:      $IP"
  printInfo "RSSI:    $RSSI dBm"
  printInfo "RSSI[0]: $RSSI_ANT0 dBm"
  printInfo "RSSI[1]: $RSSI_ANT1 dBm"
  printInfo "Noise:   $NOISE dBm"
  printInfo "MCS:     $MCSINDEX"
  printInfo "TX pwr:\n  $TXPOWER"
}

#=======================================================================
# Syntax: wifiDisconnect
#
# Disconnects from the current connection
#=======================================================================
wifiDisconnect()
{
  $WL_DUT out
  
  if [ -z "`$WL_DUT status | grep "Not associated."`" ]; then
    exitError $WIFI_ERR_DISCONNECT "Failed disconnected"
    return $WIFI_ERR_DISCONNECT
  fi
  
  printInfo "Disconnected from $SSID successfully"
  return $WIFI_OK
}

#=======================================================================
# Syntax: wifiMCSindex [index]
#
# Sets the MCS index and then prints the new value
#
# Inputs:
# - index: new MCS index
#=======================================================================
wifiMCSindex()
{
  #checkNumParams 1 ${#@} $FUNCNAME
  
  MCS_INDEX=$1
  
  printInfo "Prev MCS index info : `$WL_DUT nrate`"
  
  $WL_DUT nrate -m $MCS_INDEX
  sleep 1 
  
  printInfo "MCS current index info : `$WL_DUT nrate`"
  
  # Print MCS current index
}

#=======================================================================
# Syntax: wifiDataRate [Protocol] [modulation Index(DataRate/MCS Index) ]  [Antenna #]
#
# Sets the MCS index and then prints the new value
#
# Inputs:
# - protocol: 802.11 flavor:  a b g or n
# - modulation index: If 802.11n, which MCS index (0-15), if not 802.11n, enter data rate in Mbps
#=======================================================================
wifiDataRate()
{
  # checkNumParams 3 ${#@} $FUNCNAME
  
  SET_PROTOCOL=$1
  MCS_INDEX=$2
  SET_ANT=$3
  
  if [ $SET_PROTOCOL = n ]; then
    case $SET_ANT in
      1|2) stf_mode=0;;  # SISO Mode
      *) stf_mode=1;;    # CCD Mode
    esac
    if [ $SET_MCS_INDEX -ge 8 ]; then
                stf_mode=3     # STM Mode
    fi
    $WL_DUT nrate -m $SET_MCS_INDEX -s $stf_mode
  else
    case $SET_ANT in
    1|2) stf_mode=0;;  # SISO Mode
    *) stf_mode=1;;    # CCD Mode
    esac
    
    $WL_DUT nrate -r $SET_MCS_INDEX -s $stf_mode
  fi
  
  printInfo " Current Modulation index info : `$WL_DUT nrate`"

        # Print MCS current index
}

#=======================================================================
# Syntax: wifiSetAntenna [antenna #]
#
# Sets IRD to use either a specific single antenna or all antennas
#
# Inputs:
# - antenna #: sets antennas to use, takes a number such that in binary form, bit0 controls ANT0, bit1 controls ANT1, bit2 controls ANT2 and bit3 controls ANT3. If it is a 0, the selected antenna is to be disabled.  If it is a 1, the selected antenna is to be enabled.
#=======================================================================
wifiSetAntenna()
{
   #checkNumParams 1 ${#@} $FUNCNAME
   SET_ANT=$1
   
   case $SET_ANT in 
	 3)
		$WL_DUT rxchain  3
		$WL_DUT txchain  3
		$WL_DUT antdiv 3 
		$WL_DUT txant 3 
		printInfo "Antennas set to ANT0 and ANT1"
		;;
	 2)
		$WL_DUT rxchain 2 
		$WL_DUT txchain  2
	 	$WL_DUT antdiv 1
		$WL_DUT txant 1
		printInfo "Antennas set to ANT$SET_ANT ONLY"
		;;
	 1)
		$WL_DUT rxchain 1 
		$WL_DUT txchain 1
		$WL_DUT antdiv 0
		$WL_DUT txant 0
		printInfo "Antennas set to ANT$SET_ANT ONLY"
		;;
  esac	  
}

#=======================================================================
# Syntax: wifiSetChannel [channel][band][bandwidth][sideband]
#
# Sets the channel and channel details
#
# Inputs:
# - channel:   0 - 224
# - band:      5 (a), 2 (b/g)
# - bandwidth: 20, 40
# - sideband:  -1 (lower), 0 (none), 1 (upper)
#=======================================================================
wifiSetChannel()
{
  #checkNumParams 4 ${#@} $FUNCNAME
	
	SET_CHANNEL_NUM=$1
	SET_BAND=$2
	SET_BANDWIDTH=$3
	SET_CTL=$4
	
	if [ $SET_CHANNEL -ge 34 ]; then
		$WL_DUT band a
		SET_CHANNEL_BAND=5
	else
		$WL_DUT band b
		SET_CHANNEL_BAND=2
	fi
	
	case $SET_CTL in
	-1|lower|-l)
		SET_40_SIDEBAND=l
		SET_CHANNEL_SIDEBAND=-1
		;;
	1|upper|l)
		SET_40_SIDEBAND=u
		SET_CHANNEL_SIDEBAND=1
		;;
	0|none|None|NONE)
		SET_CHANNEL_SIDEBAND=0
		;;
	*)
		exitError $WIFI_ERR_BAD_PARAM "Not supported sideband : $SET_CTL"
		;;
	esac 
	
	case $SET_BANDWIDTH in
	40)		
			$WL_DUT mimo_bw_cap 1				
			;;
	10|20)		
			$WL_DUT mimo_bw_cap 2
			if [ $SET_CHANNEL_SIDEBAND -ne 0 ] ; then
				exitError $WIFI_ERR_BAD_PARAM "$SET_BANDWIDTH bandwidth not supported $SET_CTL ctl sideband"
			fi
			;;
	*)		
			exitError $WIFI_ERR_BAD_PARAM "Not supported $SET_BANDWIDTH bandwidth"
			;;
	esac
	printInfo "Prev chanspec info : `$WL_DUT chanspec`"
	
	#$WL_DUT chanspec -c $SET_CHANNEL_NUM -b $SET_CHANNEL_BAND -w $SET_BANDWIDTH -s $SET_CHANNEL_SIDEBAND
	if [ $SET_CHANNEL_SIDEBAND -eq 0 ]; then
		$WL_DUT chanspec $SET_CHANNEL_NUM
	else
		$WL_DUT chanspec $SET_CHANNEL_NUM$SET_40_SIDEBAND
	fi	
	sleep 1
	printInfo "Current chanspec info : `$WL_DUT chanspec`"	
	
	#Code goes here
}

#=======================================================================
# Syntax: wifiSetPower [power]
#
# Sets the power level in dBm then prints the current power level
#
# Inputs:
# - power: Power level in dBm
#=======================================================================
wifiSetPower()
{
  #checkNumParams 1 ${#@} $FUNCNAME
  #Code goes here
  SET_POWER=$1
  printInfo "Prev Power level info : `$WL_DUT txpwr1`"
  if [ $SET_POWER -eq -1 ]; then
    $WL_DUT txpwr1 -1
  else
    $WL_DUT txpwr1 -o -d $SET_POWER
  fi
  sleep 1
  
  printInfo "Current Power level info : `$WL_DUT txpwr1`"
}

#=======================================================================
# Syntax: wifiIperf2way [Server IP][timeSecs]
#
# Sends AND receives data through the WiFi port for the specified time
# This command must run in the background
# This command saves a log file of the iperf output to a file in /tmp with the name:
#       iPerfLog_2way_YYYY-M-D_HH-MM-SS.txt
#
# Inputs:
# - Server IP: IP of the PC that will be receiving the data
# - timeSecs: time in seconds for this test
#=======================================================================
wifiIperf2way()
{
  #checkNumParams 2 ${#@} $FUNCNAME
  
  serverIP=$1
  timeSecs=$2
  
  if [ ! -e $IPERF ]; then
      exitError $WIFI_ERR_NOT_FOUND "iperf utility not available"
  fi
  
  mount -t tmptfs -o remount -o size=10M tmpfs /tmp
  printInfo -n "Please start iperf utility in the server PC with the following command and press a key to continue: ./iperf -s -w 256K -i 1"; read key
  
  # Start client side from box, which is the TX side
  DATE_STRING=$(/bin/date +%Y-%m-%d_%H-%M-%S)
  $IPERF -c $serverIP  -i 1 -w 256K -t $timeSecs -d -f m | tee /tmp/iPerfLog_2way_$DATE_STRING.txt
  exitifError $? $WIFI_ERR_CMD "Command: $IPERF -c <serverip>  -i 1 -t $timeSecs -d -w 256K | tee /tmp/iPerfLog_$DATE_STRING.txt"
  
  #printInfo "Sending data"
}

#=======================================================================
# Syntax: wifiIperfSend [Server IP][timeSecs]
#
# Sends data through the WiFi port for the specified time
# This command must run in the background
# This command saves a log file of the iperf output to a file in /tmp with the name:
#       iPerfLog_Send_YYYY-M-D_HH-MM-SS.txt
#
# Inputs:
# - Server IP: IP of the PC that will be receiving the data
# - timeSecs: time in seconds for this test
#=======================================================================
wifiIperfSend()
{
  #checkNumParams 2 ${#@} $FUNCNAME
  
  serverIP=$1
  timeSecs=$2
  
  if [ ! -e $IPERF ]; then
        exitError $WIFI_ERR_NOT_FOUND "iperf utility not available"
  fi
  
  mount -t tmptfs -o remount -o size=10M tmpfs /tmp
  printInfo -n "Please start iperf utility in the server PC with the following command and press a key to continue: ./iperf -s -w 256K -i 1"; read key
  
  # Start client side from box, which is the TX side
  DATE_STRING=$(/bin/date +%Y-%m-%d_%H-%M-%S)
  $IPERF -c $serverIP -i 1 -w 256K -t $timeSecs -f m | tee /tmp/iPerfLog_Send_$DATE_STRING.txt &
  
  exitifError $? $WIFI_ERR_CMD "Command: $IPERF -c <serverip> -i 1 -w 256K -t $timeSecs | tee /tmp/iPerfLog_$DATE_STRING.txt"
  
  #printInfo "Sending data"
}

#=======================================================================
# Syntax: wifiIperfReceive
#
# Receives data from the WiFi port for the specified time
# This command must run in the background
# This command saves a log file of the iperf output to a file in /tmp with the name:
#       iPerfLog_Receive_YYYY-M-D_HH-MM-SS.txt
#
# Inputs:
# - none
#=======================================================================
wifiIperfReceive()
{
  if [ ! -e $IPERF ]; then
        exitError $WIFI_ERR_NOT_FOUND "iperf utility not available"
  fi
  
  # Start server side in box, which is the RX side
  DATE_STRING=$(/bin/date +%Y-%m-%d_%H-%M-%S)
  $IPERF -s -w 256K -i 1 -f m | tee /tmp/iPerfLog_Receive_$DATE_STRING.txt &
  exitifError $? $WIFI_ERR_CMD "Command: $IPERF -s -w 256K -i 1 -f m"
  
  BOX_IP=$(ifconfig $IFNAME | grep inet | xargs | sed 's/ /:/g' | cut -d':' -f3)
  printInfo -n "Ready to receive data. Please start iperf utility in the client PC with the following command and press a key to continue: ./iperf -c $BOX_IP -w 256K -i 1 -t [duration]"; read key
}

#=======================================================================
# Syntax: wifiIperfSilentReceive
#
# Receives data from the WiFi port for the specified time
# This command must run in the background
# This command saves a log file of the iperf output to a file in /tmp with the name:
#       iPerfLog_Receive_YYYY-M-D_HH-MM-SS.txt
#
# Inputs:
# - none
#=======================================================================
wifiIperfSilentReceive()
{
  if [ ! -e $IPERF ]; then
        exitError $WIFI_ERR_NOT_FOUND "iperf utility not available"
  fi
  
  # Start server side in box, which is the RX side
  DATE_STRING=$(/bin/date +%Y-%m-%d_%H-%M-%S)
  $IPERF -s -w 256K -i 1 -f m > /dev/null &
  exitifError $? $WIFI_ERR_CMD "Command: $IPERF -s -w 256K -i 1 -f m"
  
  BOX_IP=$(ifconfig $IFNAME | grep inet | xargs | sed 's/ /:/g' | cut -d':' -f3)
  printInfo -n "Ready to receive data. Please start iperf utility in the client PC with the following command and press a key to continue: ./iperf -c $BOX_IP -w 256K -i 1 -t [duration]"; read key
}

#=======================================================================
# Syntax: wifiLogCheck [log filename]
#
# Checks the log file created after running iPerf for throughput dropouts
#
# Inputs:
# - log filename: path and filename of the log file
#=======================================================================
wifiLogCheck()
{
  #checkNumParams 1 ${#@} $FUNCNAME

  logfilename=$1

  DROP_EXIST=$(cat $logfilename |grep sec |grep "0\.00")

  cat $logfilename |grep sec |grep "0\.00" |while read line; do if echo $line &> /dev/null; then echo "Dropout occurred at $(echo $line |cut -d']' -f2 |cut -c1-5 |xargs) seconds"; fi; done
   #if [ $DROP_EXIST != "" ]; then
        #       SEC=$($DROP_EXIST |cut -d']' -f2 |cut -c1-5 |xargs)
        #       printInfo "Dropouts occurred at $SEC seconds"
   #fi

  DOWNLINK_ID=$(cat $logfilename |grep "connected" |grep "port 5001 connected" |cut -d'[' -f2 |cut -d']' -f1 |xargs)
  UPLINK_ID=$(cat $logfilename |grep "connected" |grep -v "port 5001 connected" |cut -d'[' -f2 |cut -d']' -f1 |xargs)

  DOWN_LINE=$(cat $logfilename |grep " 0\.0-" |grep -v " 1\.0" |grep "$DOWNLINK_ID]")
  UP_LINE=$(cat $logfilename |grep " 0\.0-" |grep -v " 1\.0" |grep "$UPLINK_ID]")

  DOWN_AVG=$(echo $DOWN_LINE |cut -d' ' -f7-8)
  UP_AVG=$(echo $UP_LINE |cut -d' ' -f7-8)

  printInfo "Average throughput for Downlink was $DOWN_AVG"
  printInfo "Average throughput for Uplink was $UP_AVG"
}


#=======================================================================
# Syntax: wifiCWWave [channel]
#
# Transmits a CW waveform on the selected channel
#
# Inputs:
# - channel: channel to use to transmit the CW wave
#=======================================================================
wifiCWWave()
{
  #checkNumParams 1 ${#@} $FUNCNAME
  
  #Code goes here
  SET_CHANNEL=$1
  
  if [ $SET_CHANNEL -ge 34 ]; then
    SET_BAND=a
  else
    SET_BAND=b
  fi

  printInfo "CW waveform Setting info"
  printInfo "Channel : $SET_CHANNEL"
  printInfo "Band : $SET_BAND"

  $WL_DUT down
  $WL_DUT phy_txpwrctrl 0
  $WL_DUT mpc 0
  $WL_DUT phy_watchdog 0
  $WL_DUT down
  $WL_DUT scansuppress 1
  $WL_DUT band $SET_BAND
  $WL_DUT country $WIFI_COUNTRY_CODE_ALL 
  $WL_DUT bi 65535
  $WL_DUT ampdu 1
  $WL_DUT txchain 3
  $WL_DUT antdiv 3
  $WL_DUT txant 3
  $WL_DUT up
  $WL_DUT interference 0
  $WL_DUT phy_forcecal 1
  $WL_DUT disassoc
  $WL_DUT out
  $WL_DUT fqacurcy $SET_CHANNEL
  sleep 1
    
  printInfo "CW test started on channel $channel"
}

#=======================================================================
# Syntax: wifiCWWaveStop
#
# Stops the CW wave test
#=======================================================================
wifiCWWaveStop()
{
  $WL_DUT fqacurcy 0
  $WL_DUT down
  $WL_DUT country $WIFI_COUNTRY_CODE  
  $WL_DUT mpc 1
  $WL_DUT up
    
  #Code goes here
  printInfo "CW test stopped"
}

#=======================================================================
# Syntax: wifiRxMode [channel][bandwidth][ctl sideband][antenna #][protocol][mcs index/data rate]<IQ nxn MAC address> 
# Sets chip to receive a modulated signal on the selected channel
#       Note: MAC address of wifi module for testing purposes is set to 00:11:22:33:44:55
#
# Inputs:
# - channel: channel to use to receive (for 40 MHz channels this is the center)
# - bandwidth: bandwidth to use, 20 or 40 MHz
# - ctl sideband: upper or lower control channel, 1 for upper, -1 for lower, 0 for 20 MHz channels
# - protocol: 802.11 flavor:  a b g or n
# - mcs index: If 802.11n, which index (0-15), if not 802.11n, enter data rate in Mbps
#=======================================================================
wifiRxMode()
{
  ##checkNumParams 5 ${#@} $FUNCNAME
  #Code goes here
  SET_BAND=`$WL_DUT band`
  SET_CHANNEL=$1
  SET_BANDWIDTH=$2
  SET_CTL=$3
  SET_ANT=$4
  SET_PROTOCOL=$5
  SET_MCS_INDEX=$6
  SET_MAC_ADDRESS=$7

  case $SET_BANDWIDTH in
  10|20|40)       ;;
  *)
  exitError $WIFI_ERR_BAD_PARAM "Not supported $SET_BANDWIDTH bandwidth"
  ;;
  esac

  case $SET_CTL in
  -1|0|1)         ;;
  *)
    exitError $WIFI_ERR_BAD_PARAM "Not supported $SET_CTL ctl sideband"
              ;;
  esac

  case $SET_PROTOCOL in
  a|b|g)
    if [ $SET_CTL -ne 0 ] ; then
      exitError $WIFI_ERR_BAD_PARAM "$SET_PROTOCOL protocol not supported $SET_CTL ctl sideband"
    fi
  
    if [ $SET_BANDWIDTH -eq 40 ]; then
      exitError $WIFI_ERR_BAD_PARAM "$SET_PROTOCOL protocol not supported $SET_BANDWIDTH bandwidth"
    fi
    ;;
  n)
    ;;
  *)
    exitError $WIFI_ERR_BAD_PARAM "Not supported protocol : $SET_PROTOCOL"
    ;;
  esac

  $WL_DUT phy_watchdog 1   #default
  sleep 2
  $WL_DUT mpc 0
  $WL_DUT down
  $WL_DUT country $WIFI_COUNTRY_CODE_ALL 
  $WL_DUT wsec 0
  $WL_DUT stbc_tx 0 #default
  $WL_DUT stbc_rx 1 #default
  $WL_DUT scansuppress 1
  $WL_DUT spect 0
  $WL_DUT mimo_bw_cap 1
  $WL_DUT up
  
  $WL_DUT down
  $WL_DUT bi 65535
  $WL_DUT phy_watchdog 0

  if [ $SET_BANDWIDTH -eq 40 ]; then
    $WL_DUT mimo_txbw 4
  else
    $WL_DUT mimo_txbw 2 
  fi

  if [ $SET_CHANNEL -ge 34 ]; then
    $WL_DUT band a
    SET_BAND=5
  else
    $WL_DUT band b
    SET_BAND=2
  fi

  if [ $SET_ANT -eq 3 ] ; then
    wifiSetAntenna $SET_ANT
    wifiSetChannel $SET_CHANNEL $SET_BAND $SET_BANDWIDTH $SET_CTL 
    wifiDataRate   $SET_PROTOCOL $SET_MCS_INDEX $SET_ANT
  else
    wifiDataRate   $SET_PROTOCOL $SET_MCS_INDEX $SET_ANT
    wifiSetAntenna $SET_ANT
    wifiSetChannel $SET_CHANNEL $SET_BAND $SET_BANDWIDTH $SET_CTL 
  fi
  
  $WL_DUT up
  $WL_DUT join test2 imode adhoc
  $WL_DUT disassoc

  if [ -n "$SET_MAC_ADDRESS" ]; then
    $WL_DUT pkteng_start $SET_MAC_ADDRESS rxwithack
  else
    $WL_DUT pkteng_start 00:11:22:33:44:55 rx 25 1500 0
  fi
  
  printInfo "RX Mode started on channel $SET_CHANNEL with $SET_BANDWIDTH MHz BW"
}

#=======================================================================
# Syntax: wifiRxMeasure
#
# Sets chip to measure RSSI on current channel
#
# Inputs:
# - none
#=======================================================================
wifiRxMeasure()
{
  $WL_DUT pkteng_stats
  #Print RSSI of current channel

}

#=======================================================================
# Syntax: wifiRxModeStop
#
# Stops reception of modulated signals
#
# Inputs:
# - none
#=======================================================================
wifiRxModeStop()
{
  #Code goes here
  $WL_DUT pkteng_stop rx

  $WL_DUT down
  $WL_DUT country $WIFI_COUNTRY_CODE  
  $WL_DUT mpc 1
  $WL_DUT scansuppress 0
  $WL_DUT spect 1
  $WL_DUT mimo_bw_cap 1 
  $WL_DUT mimo_txbw -1 
  $WL_DUT mimo_preamble -1
  $WL_DUT rxchain 3
  $WL_DUT txchain 3
  $WL_DUT antdiv 3
  $WL_DUT txant 3
  $WL_DUT phy_watchdog 1
  $WL_DUT interference 4
  $WL_DUT interference_override -1

  printInfo "RX Mode stopped."
}

#=======================================================================
# Syntax: wifiRxPER [channel][bandwidth][ctl sideband][antenna #][protocol][mcs index/data rate]<TX device MAC address>
#
# Sets chip to receive a modulated signal on the selected channel
#       Note: MAC address of wifi module for counting purposes is set to 00:11:22:33:44:55
#       Reports PER based on 10000 Packet input
#
# Inputs:
# - channel: channel to use to receive (for 40 MHz channels this is the center)
# - bandwidth: bandwidth to use, 20 or 40 MHz
# - ctl sideband: upper or lower control channel, 1 for upper, -1 for lower, 0 for 20 MHz channels
# - protocol: 802.11 flavor:  a b g or n
# - mcs index: If 802.11n, which index (0-15), if not 802.11n, enter data rate in Mbps
#=======================================================================
wifiRxPER()
{
  #checkNumParams 5 ${#@} $FUNCNAME

  SET_CHANNEL=$1
  SET_BANDWIDTH=$2
  SET_CTL=$3
  SET_ANT=$4
  SET_PROTOCOL=$5
  SET_MCS_INDEX=$6
  SET_MAC_ADDRESS=$7
  SET_PACKET_COUNTERS=10000

  if [ -n "$SET_MAC_ADDRESS" ]; then
    wifiRxMode $SET_CHANNEL $SET_BANDWIDTH $SET_CTL $SET_ANT $SET_PROTOCOL $SET_MCS_INDEX $SET_MAC_ADDRESS
  else
    wifiRxMode $SET_CHANNEL $SET_BANDWIDTH $SET_CTL $SET_ANT $SET_PROTOCOL $SET_MCS_INDEX
  fi

  SAVE_RXPER_TXT=/tmp/wifi_saved_rxper.txt
  #Code goes here
  $WL_DUT counters > $SAVE_RXPER_TXT 
  saved_rxdfrmocast=`cat $SAVE_RXPER_TXT | grep -o "rxdfrmocast [0-9]*" | cut -d " " -f2`
  saved_rxdfrmmcast=`cat $SAVE_RXPER_TXT | grep -o "rxdfrmmcast [0-9]*" | cut -d " " -f2`
  saved_rxdfrmucastmbss=`cat $SAVE_RXPER_TXT | grep -o "rxdfrmucastmbss [0-9]*" | cut -d " " -f2`
  saved_rxmfrmmcast=`cat $SAVE_RXPER_TXT | grep -o "rxmfrmmcast [0-9]*" | cut -d " " -f2`
  saved_rxbadfcs=`cat $SAVE_RXPER_TXT | grep -o "rxbadfcs [0-9]*" | cut -d " " -f2`
  saved_rxbadplcp=`cat $SAVE_RXPER_TXT | grep -o "rxbadplcp [0-9]*" | cut -d " " -f2`
  saved_rxcrsglitch=`cat $SAVE_RXPER_TXT | grep -o "rxcrsglitch [0-9]*" | cut -d " " -f2`
  saved_pktengrxdmcast=`cat $SAVE_RXPER_TXT | grep -o "pktengrxdmcast [0-9]*" | cut -d " " -f2`
  
  printInfo "When test transmission is complete hit Enter"
  read input
  
  printInfo "RX PER test stopped."
  
  #Calculation of PER goes here
  CUR_RXPER_TXT=/tmp/wifi_cur_rxper.txt
  $WL_DUT counters > $CUR_RXPER_TXT
  cur_rxdfrmocast=`cat $CUR_RXPER_TXT | grep -o "rxdfrmocast [0-9]*" | cut -d " " -f2`
  cur_rxdfrmmcast=`cat $CUR_RXPER_TXT | grep -o "rxdfrmmcast [0-9]*" | cut -d " " -f2`
  cur_rxdfrmucastmbss=`cat $CUR_RXPER_TXT | grep -o "rxdfrmucastmbss [0-9]*" | cut -d " " -f2`
  cur_rxmfrmmcast=`cat $CUR_RXPER_TXT | grep -o "rxmfrmmcast [0-9]*" | cut -d " " -f 2`
  cur_rxbadfcs=`cat $CUR_RXPER_TXT | grep -o "rxbadfcs [0-9]*" | cut -d " " -f2`
  cur_rxbadplcp=`cat $CUR_RXPER_TXT | grep -o "rxbadplcp [0-9]*" | cut -d " " -f2`
  cur_rxcrsglitch=`cat $CUR_RXPER_TXT | grep -o "rxcrsglitch [0-9]*" | cut -d " " -f2`
  cur_pktengrxdmcast=`cat $CUR_RXPER_TXT | grep -o "pktengrxdmcast [0-9]*" | cut -d " " -f2`
  
  #PER based on Sequence of "SET_PACKET_COUNTERS" packets sent
  cal_rxdfrmocast=`expr $cur_rxdfrmocast - $saved_rxdfrmocast`
  cal_rxdfrmmcast=`expr $cur_rxdfrmmcast - $saved_rxdfrmmcast`
  cal_rxdfrmucastmbss=`expr $cur_rxdfrmucastmbss - $saved_rxdfrmucastmbss`
  cal_rxmfrmmcast=`expr $cur_rxmfrmmcast - $saved_rxmfrmmcast`
  cal_rxbadfcs=`expr $cur_rxbadfcs - $saved_rxbadfcs`
  cal_rxbadplcp=`expr $cur_rxbadplcp - $saved_rxbadplcp`
  cal_rxcrsglitch=`expr $cur_rxcrsglitch - $saved_rxcrsglitch`
  cal_pktengrxdmcast=`expr $cur_pktengrxdmcast - $saved_pktengrxdmcast`
  
  PER=$(($cal_rxdfrmucastmbss * 100))
  wholePct=`expr $PER / $SET_PACKET_COUNTERS`
  
  printInfo "--------------------------------------------------------"
  # printInfo "rxdfrmocast : $cal_rxdfrmocast"
  # printInfo "rxdfrmmcast : $cal_rxdfrmmcast"
  printInfo "rxdfrmucastmbss : $cal_rxdfrmucastmbss"
  # printInfo "rxmfrmmcast : $cal_rxmfrmmcast"
  # printInfo "rxbadfcs : $cal_rxbadfcs"
  # printInfo "rxbadplcp : $cal_rxbadplcp"
  # printInfo "rxcrsglitch : $cal_rxcrsglitch"
  # printInfo "pktengrxdmcast : $cal_pktengrxdmcast"
  echo " "
  printInfo "PER is $wholePct%"
  printInfo "--------------------------------------------------------"
}

#=======================================================================
# Syntax: wifiRxPER_hmx [Packet Counters] [channel][bandwidth][ctl sideband][antenna #][protocol][mcs index/data rate]<TX device MAC address>
#
# Sets chip to receive a modulated signal on the selected channel
#       Note: MAC address of wifi module for counting purposes is set to 00:11:22:33:44:55
#       Reports PER based on Send Packet Counters input
#
# Inputs:
# - Packet Counters :  Send Packet Counters from Instrument
# - channel: channel to use to receive (for 40 MHz channels this is the center)
# - bandwidth: bandwidth to use, 20 or 40 MHz
# - ctl sideband: upper or lower control channel, 1 for upper, -1 for lower, 0 for 20 MHz channels
# - protocol: 802.11 flavor:  a b g or n
# - mcs index: If 802.11n, which index (0-15), if not 802.11n, enter data rate in Mbps
#=======================================================================
wifiRxPER_hmx()
{
  #checkNumParams 5 ${#@} $FUNCNAME

  SET_PACKET_COUNTERS=$1
  SET_CHANNEL=$2
  SET_BANDWIDTH=$3
  SET_CTL=$4
  SET_ANT=$5
  SET_PROTOCOL=$6
  SET_MCS_INDEX=$7
  SET_MAC_ADDRESS=$8

  INPUT="a"
  
  if [ -n "$SET_MAC_ADDRESS" ]; then
    wifiRxMode $SET_CHANNEL $SET_BANDWIDTH $SET_CTL $SET_ANT $SET_PROTOCOL $SET_MCS_INDEX $SET_MAC_ADDRESS
  else
    wifiRxMode $SET_CHANNEL $SET_BANDWIDTH $SET_CTL $SET_ANT $SET_PROTOCOL $SET_MCS_INDEX
  fi

  while [ -n $INPUT ]; do
    SAVE_RXPER_TXT=/tmp/wifi_saved_rxper.txt
    #Code goes here
    $WL_DUT counters > $SAVE_RXPER_TXT 
    saved_rxdfrmocast=`cat $SAVE_RXPER_TXT | grep -o "rxdfrmocast [0-9]*" | cut -d " " -f2`
    saved_rxdfrmmcast=`cat $SAVE_RXPER_TXT | grep -o "rxdfrmmcast [0-9]*" | cut -d " " -f2`
    saved_rxdfrmucastmbss=`cat $SAVE_RXPER_TXT | grep -o "rxdfrmucastmbss [0-9]*" | cut -d " " -f2`
    saved_rxmfrmmcast=`cat $SAVE_RXPER_TXT | grep -o "rxmfrmmcast [0-9]*" | cut -d " " -f2`
    saved_rxbadfcs=`cat $SAVE_RXPER_TXT | grep -o "rxbadfcs [0-9]*" | cut -d " " -f2`
    saved_rxbadplcp=`cat $SAVE_RXPER_TXT | grep -o "rxbadplcp [0-9]*" | cut -d " " -f2`
    saved_rxcrsglitch=`cat $SAVE_RXPER_TXT | grep -o "rxcrsglitch [0-9]*" | cut -d " " -f2`
    saved_pktengrxdmcast=`cat $SAVE_RXPER_TXT | grep -o "pktengrxdmcast [0-9]*" | cut -d " " -f2`
    
    printInfo ""
    printInfo "When test transmission is complete hit Enter"
    printInfo "Enter \"q\" for EXIT"
    
    read INPUT
    
    if [ "$INPUT" = "q" ];then
      printInfo "RX PER test stopped."
      break
    fi
  
    printInfo "Input : $INPUT"
    
    #Calculation of PER goes here
    CUR_RXPER_TXT=/tmp/wifi_cur_rxper.txt
    $WL_DUT counters > $CUR_RXPER_TXT
    cur_rxdfrmocast=`cat $CUR_RXPER_TXT | grep -o "rxdfrmocast [0-9]*" | cut -d " " -f2`
    cur_rxdfrmmcast=`cat $CUR_RXPER_TXT | grep -o "rxdfrmmcast [0-9]*" | cut -d " " -f2`
    cur_rxdfrmucastmbss=`cat $CUR_RXPER_TXT | grep -o "rxdfrmucastmbss [0-9]*" | cut -d " " -f2`
    cur_rxmfrmmcast=`cat $CUR_RXPER_TXT | grep -o "rxmfrmmcast [0-9]*" | cut -d " " -f 2`
    cur_rxbadfcs=`cat $CUR_RXPER_TXT | grep -o "rxbadfcs [0-9]*" | cut -d " " -f2`
    cur_rxbadplcp=`cat $CUR_RXPER_TXT | grep -o "rxbadplcp [0-9]*" | cut -d " " -f2`
    cur_rxcrsglitch=`cat $CUR_RXPER_TXT | grep -o "rxcrsglitch [0-9]*" | cut -d " " -f2`
    cur_pktengrxdmcast=`cat $CUR_RXPER_TXT | grep -o "pktengrxdmcast [0-9]*" | cut -d " " -f2`
  
    #PER based on Sequence of "SET_PACKET_COUNTERS" packets sent
    cal_rxdfrmocast=`expr $cur_rxdfrmocast - $saved_rxdfrmocast`
    cal_rxdfrmmcast=`expr $cur_rxdfrmmcast - $saved_rxdfrmmcast`
    cal_rxdfrmucastmbss=`expr $cur_rxdfrmucastmbss - $saved_rxdfrmucastmbss`
    cal_rxmfrmmcast=`expr $cur_rxmfrmmcast - $saved_rxmfrmmcast`
    cal_rxbadfcs=`expr $cur_rxbadfcs - $saved_rxbadfcs`
    cal_rxbadplcp=`expr $cur_rxbadplcp - $saved_rxbadplcp`
    cal_rxcrsglitch=`expr $cur_rxcrsglitch - $saved_rxcrsglitch`
    cal_pktengrxdmcast=`expr $cur_pktengrxdmcast - $saved_pktengrxdmcast`
  
    PER=$(($cal_rxdfrmucastmbss * 100))
    wholePct=`expr $PER / $SET_PACKET_COUNTERS`
  
    printInfo "--------------------------------------------------------"
  # printInfo "rxdfrmocast : $cal_rxdfrmocast"
  # printInfo "rxdfrmmcast : $cal_rxdfrmmcast"
    printInfo "cur_rxdfrmucastmbss   : $cur_rxdfrmucastmbss"
    printInfo "saved_rxdfrmucastmbss : $saved_rxdfrmucastmbss"
    printInfo "rxdfrmucastmbss : $cal_rxdfrmucastmbss"
  # printInfo "rxmfrmmcast : $cal_rxmfrmmcast"
  # printInfo "rxbadfcs : $cal_rxbadfcs"
  # printInfo "rxbadplcp : $cal_rxbadplcp"
  # printInfo "rxcrsglitch : $cal_rxcrsglitch"
  # printInfo "pktengrxdmcast : $cal_pktengrxdmcast"
    echo " "
    printInfo "PER is $wholePct%"
    printInfo "--------------------------------------------------------"
  done
}

#=======================================================================
# Syntax: wifiTxMode [inter-packet gap][bytes per frame][channel][bandwidth][ctl sideband][antenna #][power][protocol][mcs index/data rate] 
# Transmits a modulated signal on the selected channel
#
# Inputs:
# - ipg: set inter-packet gap
# - frame: bytes per frame
# - channel: channel to use to transmit (for 40 MHz channels this is the center channel)
# - bandwidth: bandwidth to use, 20 or 40 MHz
# - ctl sideband: upper or lower control channel, 1 for upper, -1 for lower, 0 for 20 MHz channels
# - antenna: 1 to antenna 1, 2 to antenna 2
# - power: set Power of TxMode
# - protocol: 802.11 flavor:  a b g or n
# - modulation index: If 802.11n, which MCS index (0-15), if not 802.11n, enter data rate in Mbps
#=======================================================================
wifiTxMode()
{
  #checkNumParams 9 ${#@} $FUNCNAME
  #Code goes here
  SET_IPG=$1
  SET_FRAME=$2
  SET_BAND=`$WL_DUT band`
  SET_CHANNEL=$3
  SET_BANDWIDTH=$4
  SET_CTL=$5
  SET_ANT=$6
  SET_POWER=$7
  SET_PROTOCOL=$8
  SET_MCS_INDEX=$9

  case $SET_BANDWIDTH in
    10|20|40) ;;
    *)  
        exitError $WIFI_ERR_BAD_PARAM "Not supported $SET_BANDWIDTH bandwidth"
        ;;
  esac 

  case $SET_CTL in
    -1|0|1)   ;;
    *)    exitError $WIFI_ERR_BAD_PARAM "Not supported $SET_CTL ctl sideband"
        ;;
  esac 

  case $SET_PROTOCOL in
    a|b|g)
      if [ $SET_CTL -ne 0 ] ; then
              exitError $WIFI_ERR_BAD_PARAM "$SET_PROTOCOL protocol not supported $SET_CTL ctl sideband"
      fi
    
      if [ $SET_BANDWIDTH -eq 40 ]; then
              exitError $WIFI_ERR_BAD_PARAM "$SET_PROTOCOL protocol not supported $SET_BANDWIDTH bandwidth"
      fi
      ;;
    n)
      ;;
    *)
      exitError $WIFI_ERR_BAD_PARAM "Not supported protocol : $SET_PROTOCOL"
      ;;
  esac

  $WL_DUT phy_watchdog 1   #default
  sleep 2
  $WL_DUT mpc 0
  $WL_DUT down
  $WL_DUT country  $WIFI_COUNTRY_CODE_ALL
  $WL_DUT wsec 0
  $WL_DUT stbc_tx 0       #default
  $WL_DUT stbc_rx 1       #default
  $WL_DUT scansuppress 1
  $WL_DUT spect 0
  $WL_DUT mimo_bw_cap 1
  $WL_DUT up

  $WL_DUT down
  $WL_DUT bi 65535
  $WL_DUT phy_watchdog 0

  if [ $SET_BANDWIDTH -eq 40 ]; then
    $WL_DUT mimo_txbw 4
  else
    $WL_DUT mimo_txbw 2 
  fi

    $WL_DUT mimo_preamble 0
  
  if [ $SET_CHANNEL -ge 34 ]; then
    $WL_DUT band a
    SET_BAND=5
  else
    $WL_DUT band b
    SET_BAND=2
  fi

  if [ $SET_ANT -eq 3 ] ; then
    wifiSetAntenna $SET_ANT
    wifiSetChannel $SET_CHANNEL $SET_BAND $SET_BANDWIDTH $SET_CTL 
    wifiDataRate   $SET_PROTOCOL $SET_MCS_INDEX $SET_ANT
  else
    wifiDataRate   $SET_PROTOCOL $SET_MCS_INDEX $SET_ANT
    wifiSetAntenna $SET_ANT
    wifiSetChannel $SET_CHANNEL $SET_BAND $SET_BANDWIDTH $SET_CTL 
  fi
    
    $WL_DUT up
    $WL_DUT interference 0
    $WL_DUT interference_override 0
    $WL_DUT join test2 imode adhoc

    $WL_DUT phy_forcecal 1
    wifiSetPower $SET_POWER
    $WL_DUT disassoc

    $WL_DUT pkteng_maxlen
    $WL_DUT pkteng_start 00:11:22:33:44:55 tx $SET_IPG $SET_FRAME 0

    printInfo "TX Mode started on channel $SET_CHANNEL with $SET_BANDWIDTH MHz BW"
}

#=======================================================================
# Syntax: wififccTxMode [inter-packet gap][bytes per frame][channel][bandwidth][ctl sideband][antenna #][power][protocol][mcs index/data rate] 
# Transmits a modulated signal on the selected channel
#
# Inputs:
# - ipg: set inter-packet gap
# - frame: bytes per frame
# - channel: channel to use to transmit (for 40 MHz channels this is the center channel)
# - bandwidth: bandwidth to use, 20 or 40 MHz
# - ctl sideband: upper or lower control channel, 1 for upper, -1 for lower, 0 for 20 MHz channels
# - antenna: 1 to antenna 1, 2 to antenna 2
# - power: set Power of TxMode
# - protocol: 802.11 flavor:  a b g or n
# - modulation index: If 802.11n, which MCS index (0-15), if not 802.11n, enter data rate in Mbps
#=======================================================================
wififccTxMode()
{
  #checkNumParams 9 ${#@} $FUNCNAME
  #Code goes here
  SET_IPG=$1
  SET_FRAME=$2
  SET_BAND=`$WL_DUT band`
  SET_CHANNEL=$3
  SET_BANDWIDTH=$4
  SET_CTL=$5
  SET_ANT=$6
  SET_POWER=$7
  SET_PROTOCOL=$8
  SET_MCS_INDEX=$9

  case $SET_BANDWIDTH in
    10|20|40) ;;
    *)  
        exitError $WIFI_ERR_BAD_PARAM "Not supported $SET_BANDWIDTH bandwidth"
        ;;
  esac 

  case $SET_CTL in
    -1|0|1)   ;;
    *)    exitError $WIFI_ERR_BAD_PARAM "Not supported $SET_CTL ctl sideband"
        ;;
  esac 

  case $SET_PROTOCOL in
    a|b|g)
      if [ $SET_CTL -ne 0 ] ; then
              exitError $WIFI_ERR_BAD_PARAM "$SET_PROTOCOL protocol not supported $SET_CTL ctl sideband"
      fi
    
      if [ $SET_BANDWIDTH -eq 40 ]; then
              exitError $WIFI_ERR_BAD_PARAM "$SET_PROTOCOL protocol not supported $SET_BANDWIDTH bandwidth"
      fi
      ;;
    n)
      ;;
    *)
      exitError $WIFI_ERR_BAD_PARAM "Not supported protocol : $SET_PROTOCOL"
      ;;
  esac

    $WL_DUT phy_watchdog 0   #default
    sleep 2
    $WL_DUT mpc 0
    $WL_DUT down
    $WL_DUT country $WIFI_COUNTRY_CODE_ALL 
    $WL_DUT wsec 0
    $WL_DUT stbc_tx 0       #default
    $WL_DUT stbc_rx 1       #default
    $WL_DUT scansuppress 1
 #  $WL_DUT tempsense_disable 1
    $WL_DUT spect 0
    $WL_DUT mimo_bw_cap 1
    $WL_DUT up

    $WL_DUT down
    $WL_DUT bi 65535

  if [ $SET_BANDWIDTH -eq 40 ]; then
      $WL_DUT mimo_txbw 4
    else
      $WL_DUT mimo_txbw 2 
    fi

    $WL_DUT mimo_preamble 0
  
  if [ $SET_CHANNEL -ge 34 ]; then
    $WL_DUT band a
    SET_BAND=5
  else
    $WL_DUT band b
    SET_BAND=2
  fi

  case $SET_PROTOCOL in
    b)
      $WL_DUT rateset 1b;;
    a|g|n)
      $WL_DUT rateset 6b;;
  esac

  if [ $SET_ANT -eq 3 ] ; then
    wifiSetAntenna $SET_ANT
    wifiSetChannel $SET_CHANNEL $SET_BAND $SET_BANDWIDTH $SET_CTL 
    wifiDataRate   $SET_PROTOCOL $SET_MCS_INDEX $SET_ANT
  else
    wifiDataRate   $SET_PROTOCOL $SET_MCS_INDEX $SET_ANT
    wifiSetAntenna $SET_ANT
    wifiSetChannel $SET_CHANNEL $SET_BAND $SET_BANDWIDTH $SET_CTL 
  fi
  
  $WL_DUT up
  $WL_DUT interference 0
  $WL_DUT interference_override 0
  $WL_DUT join test2 imode adhoc

  $WL_DUT phy_forcecal 1
  wifiSetPower $SET_POWER
  $WL_DUT disassoc

  $WL_DUT pkteng_maxlen
  $WL_DUT pkteng_start 00:11:22:33:44:55 tx $SET_IPG $SET_FRAME 0

  printInfo "TX Mode started on channel $SET_CHANNEL with $SET_BANDWIDTH MHz BW"
}
#=======================================================================
# Syntax: wifiTxModeStop
#
# Stops transmission of modulated signal
#
# Inputs:
# - none
#=======================================================================
wifiTxModeStop()
{
  #Code goes here
  $WL_DUT pkteng_stop tx
  
  $WL_DUT down
  $WL_DUT country $WIFI_COUNTRY_CODE  
  $WL_DUT mpc 1
  $WL_DUT scansuppress 0
  $WL_DUT spect 1
  $WL_DUT mimo_bw_cap 1 
  $WL_DUT mimo_txbw -1 
  $WL_DUT mimo_preamble -1
  $WL_DUT rateset all
  $WL_DUT txchain 3
  $WL_DUT antdiv 3
  $WL_DUT txant 3
  $WL_DUT phy_watchdog 1
  $WL_DUT interference 4
  $WL_DUT interference_override -1
  $WL_DUT phy_forcecal 0
  printInfo "TX Mode stopped."
}

# Errors
WIFI_OK=0
WIFI_ERR_BAD_PARAM=1
WIFI_ERR_NOT_NETWORKS=88
WIFI_ERR_NOT_CONNECTED=89
WIFI_ERR_INIT=90
WIFI_ERR_CMD=91
WIFI_ERR_JOIN=92
WIFI_ERR_IP=93
WIFI_ERR_DISCONNECT=94
WIFI_ERR_NOT_FOUND=95

########################################################################
#                              Utilities                               #
########################################################################

#=======================================================================
# Syntax : *
#
# Help command
#=======================================================================
wifiHelp()
{
  printInfo "Wi-Fi script command"
  echo "./wifi.sh init [mfg/prd]"
  echo "./wifi.sh term"
  echo "./wifi.sh list [band]"
  echo "./wifi.sh apinfo [SSID]"
  echo "./wifi.sh connect [SSID][Password]"
  echo "./wifi.sh connectIP [SSID][Password][IP address]"
  echo "./wifi.sh connected"
  echo "./wifi.sh status"
  echo "./wifi.sh disconnect"
  echo "./wifi.sh setMCSindex [index]"
  echo "./wifi.sh setChannel [channel][band][bandwidth][ctl sideband]"
  echo "./wifi.sh setPower [power]"
  echo "./wifi.sh iperf2way [Server IP][timeSecs]"
  echo "./wifi.sh iperfSend [Server IP][timeSecs]"
  echo "./wifi.sh iperfReceive"
  echo "./wifi.sh logCheck [/tmp/iperf log filename]"
  echo "./wifi.sh cwwave [channel]"
  echo "./wifi.sh cwwaveStop"
  echo "./wifi.sh rxMode [channel][bandwidth][ctl sideband][antenna #][protocol][mcs index/data rate]<IQ nxn MAC address> "
  echo "./wifi.sh rxModeStop"
  echo "./wifi.sh rxMeasure"
  echo "./wifi.sh rxPER [channel][bandwidth][ctl sideband][antenna #][protocol][mcs index/data rate]<TX device MAC address>"
  echo "./wifi.sh rxPER_hmx [send Packet] [channel][bandwidth][ctl sideband][antenna #][protocol][mcs index/data rate]<TX device MAC address>"
  echo "./wifi.sh txMode [inter-packet gap][bytes per frame][channel][bandwidth][ctl sideband][antenna #][power][protocol][mcs index/data rate]"
  echo "./wifi.sh fcctxMode [inter-packet gap][bytes per frame][channel][bandwidth][ctl sideband][antenna #][power][protocol][mcs index/data rate]"
  echo "./wifi.sh txModeStop"
  echo "./wifi.sh reset"
}


#=======================================================================
# Syntax: printInfo [message]
#
# Prints the information message given
#=======================================================================
printInfo()
{
  echo -e "[INFO]  $@"
}

#=======================================================================
# Syntax: printError [message]
#
# Prints the error message given
#=======================================================================
printError()
{
  echo -e "[ERROR] $@"
}

#=======================================================================
# Syntax: exitError [errcode] [message]
#
# Internal function to print an error and exit with the given error code
#=======================================================================
exitError()
{
  errorcode=$1
  shift
  message="$@"
  errorhex=`printf 0x%02X $errorcode`

  if [ ${#message} -gt 0 ]; then
        printError "($errorhex): $message"
  else
        printError "($errorhex)"
  fi
  exit $errorcode
}

#=======================================================================
# Syntax: exitifError [value] [errcode] [message]
#
# If value is not 0, it exits with the error given
#=======================================================================
exitifError()
{
  if [ $1 -ne 0 ]; then
    shift
    exitError $@
  fi
}

#=======================================================================
# Syntax: checkNumParams [expected] [numparams] [functionname]
#
# Validates the input params to a function
#=======================================================================
checkNumParams()
{
  expected=$1
  numparams=$2
  function=$3
  
  if [ $numparams -ne $expected ]; then
    exitError $WIFI_ERR_BAD_PARAM "$function needs $expected, given $numparams"
  fi
}

########################################################################
#                             Script Main                              #
##n#####################################################################


#Param 1 is the function, the rest the params for that function
func=$1
shift
params="$@"

# Call the function given
case $func in
  "init")                      wifiInit $params ;;
  "term")                      wifiTerm $params ;;
  "list")                      wifiList $params ;;
  "connected")                 wifiConnected $params ;;
  "status")                    wifiStatus $params ;;
  "apinfo")                    wifiAPInfo $params ;;
  "connect")                   wifiConnect $params ;;
  "connectIP")                 wifiConnectIP $params ;;
  "disconnect")                wifiDisconnect $params ;;
  "logCheck")                  wifiLogCheck $params ;;
  "setMCSindex")               wifiMCSindex $params ;;
  "setDataRate")               wifiDataRate $params ;;
  "setAntenna")                wifiSetAntenna $params ;;
  "setChannel")                wifiSetChannel $params ;;
  "iperf2way")                 wifiIperf2way $params ;;
  "iperfSend")                 wifiIperfSend $params ;;
  "iperfReceive")              wifiIperfReceive $params ;;
  "iperfSilentReceive")        wifiIperfSilentReceive $params ;;
  "txMode")                    wifiTxMode $params ;;
  "fcctxMode")                 wififccTxMode $params ;;
  "txModeStop")                wifiTxModeStop $params ;;
  "rxPER")                     wifiRxPER $params ;;
  "rxPER_hmx")                 wifiRxPER_hmx $params ;;
  "rxMode")                    wifiRxMode $params ;;
  "rxModeStop")                wifiRxModeStop $params ;;
  "rxMeasure")                 wifiRxMeasure $params ;;
  "cwwave")                    wifiCWWave $params ;;
  "cwwaveStop")                wifiCWWaveStop $params ;;
  "setPower")                  wifiSetPower $params ;;
  "reset")                     wifiReset $params ;;
  *)                           wifiHelp;;
esac

# Exit if script not sourced
[ "$0" != "-sh" ] && exit $WIFI_
