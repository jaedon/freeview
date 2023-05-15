#!/bin/sh

# Appy ICMP Transport Layer protection
sysctl -p /etc/sysctl.conf

#export LD_LIBRARY_PATH=/usr/libexec/xtables

IP4T="/usr/local/sbin/iptables"

# Clean out tables and rules

$IP4T -F
$IP4T -X
$IP4T -Z

# Set filter chain policies to ensure fail-secure behavior
# Default is therefore to ALWAYS drop all packets
# $IP4T -P INPUT DROP
# $IP4T -P OUTPUT DROP
# $IP4T -P FORWARD DROP

# Drop some invalid packet types
# Drop packets with invalid connection state
$IP4T -A INPUT -m state --state INVALID -j DROP
$IP4T -A OUTPUT -m state --state INVALID -j DROP

# Drop packets entering via the physical ethernet interface, but claiming
# to originate from inside the machine (For Conax SE)
$IP4T -A INPUT -i eth0 -s 127.0.0.1/8 -j DROP
$IP4T -A INPUT -i wlan0 -s 127.0.0.1/8 -j DROP

# Drop packets with invalid source address types (For Conax SE)
$IP4T -A INPUT -i eth0 -m addrtype --src-type UNSPEC,BLACKHOLE,PROHIBIT -j DROP
$IP4T -A INPUT -i wlan0 -m addrtype --src-type UNSPEC,BLACKHOLE,PROHIBIT -j DROP

# Loopback rules
$IP4T -A INPUT -i lo -j ACCEPT
$IP4T -A OUTPUT -o lo -j ACCEPT

# Allow some types of outgoing TCP connections
# DNS client
$IP4T -A INPUT -p udp --sport domain -m state --state ESTABLISHED -j ACCEPT
$IP4T -A OUTPUT -p udp --dport domain -m state --state NEW,ESTABLISHED -j ACCEPT

# HTTP client
$IP4T -A INPUT -p tcp --sport www --dport first-os-port:rc1 -m state --state ESTABLISHED -j ACCEPT
$IP4T -A OUTPUT -p tcp --sport first-os-port:rc1 --dport www -m state --state NEW,ESTABLISHED -j ACCEPT
$IP4T -A INPUT -p tcp --sport webcache --dport first-os-port:rc1 -m state --state ESTABLISHED -j ACCEPT
$IP4T -A OUTPUT -p tcp --sport first-os-port:rc1 --dport webcache -m state --state NEW,ESTABLISHED -j ACCEPT

# HTTPS client
$IP4T -A INPUT -p tcp --sport https --dport first-os-port:rc1 -m state --state ESTABLISHED -j ACCEPT
$IP4T -A OUTPUT -p tcp --sport first-os-port:rc1 --dport https -m state --state NEW,ESTABLISHED -j ACCEPT

# DHCP client
$IP4T -A OUTPUT -p udp --sport bootpc --dport bootps -m state --state NEW,ESTABLISHED -j ACCEPT
$IP4T -A INPUT -p udp --sport bootps --dport bootpc -m state --state NEW,ESTABLISHED -j ACCEPT

# DLNA DMP of SSDP
$IP4T -A INPUT -p udp --dport ssdp -m state --state NEW,ESTABLISHED -j ACCEPT
$IP4T -A OUTPUT -p udp --sport ssdp -m state --state NEW,ESTABLISHED -j ACCEPT

###    ----  HOMMA  ----    ##
$IP4T -A INPUT -p tcp --dport 50001 -m state --state NEW,ESTABLISHED -j ACCEPT
$IP4T -A OUTPUT -p tcp --sport 50001 -m state --state NEW,ESTABLISHED -m owner --uid-owner homma -j ACCEPT
$IP4T -A INPUT -p tcp --dport 9000:9002 -m state --state NEW,ESTABLISHED -j ACCEPT
$IP4T -A OUTPUT -p tcp --sport 9000:9002 -m state --state NEW,ESTABLISHED -m owner --uid-owner homma -j ACCEPT

$IP4T -A INPUT -p tcp --dport first-os-port:rc1 -m state --state ESTABLISHED -j ACCEPT
$IP4T -A OUTPUT -p tcp --sport first-os-port:rc1 -m state --state NEW,ESTABLISHED -m owner --uid-owner obama -j ACCEPT
$IP4T -A OUTPUT -p tcp --sport first-os-port:rc1 -m state --state NEW,ESTABLISHED -m owner --uid-owner umma -j ACCEPT
$IP4T -A OUTPUT -p tcp --sport first-os-port:rc1 -m state --state NEW,ESTABLISHED -m owner --uid-owner thma -j ACCEPT
$IP4T -A OUTPUT -p tcp --sport first-os-port:rc1 -m state --state NEW,ESTABLISHED -m owner --uid-owner corsair -j ACCEPT
$IP4T -A OUTPUT -p tcp --sport first-os-port:rc1 -m state --state NEW,ESTABLISHED -m owner --uid-owner imma -j ACCEPT
$IP4T -A OUTPUT -p tcp --sport first-os-port:rc1 -m state --state NEW,ESTABLISHED -m owner --uid-owner homma -j ACCEPT
##    ----  ----    ##

$IP4T -A OUTPUT -p tcp --dport icslap -m state --state NEW,ESTABLISHED -j ACCEPT
$IP4T -A INPUT -p udp --dport wsd -m state --state ESTABLISHED -j ACCEPT

# ICMP for IPv4
$IP4T -A INPUT -p icmp --icmp-type echo-reply -m state --state ESTABLISHED,RELATED -j ACCEPT
$IP4T -A OUTPUT -p icmp --icmp-type echo-reply -m state --state ESTABLISHED,RELATED -j ACCEPT
$IP4T -A INPUT -p icmp --icmp-type echo-request -j ACCEPT
$IP4T -A OUTPUT -p icmp --icmp-type echo-request -j ACCEPT
# ICMP for IPv4 (For PPPOE)
$IP4T -A INPUT -p icmp --icmp-type 3/0 -j ACCEPT
$IP4T -A OUTPUT -p icmp --icmp-type 3/0 -j ACCEPT
$IP4T -A INPUT -p icmp --icmp-type 3/1 -j ACCEPT
$IP4T -A OUTPUT -p icmp --icmp-type 3/1 -j ACCEPT
$IP4T -A INPUT -p icmp --icmp-type 3/2 -j ACCEPT
$IP4T -A OUTPUT -p icmp --icmp-type 3/2 -j ACCEPT
$IP4T -A INPUT -p icmp --icmp-type 3/3 -j ACCEPT
$IP4T -A OUTPUT -p icmp --icmp-type 3/3 -j ACCEPT
$IP4T -A INPUT -p icmp --icmp-type 3/4 -j ACCEPT
$IP4T -A OUTPUT -p icmp --icmp-type 3/4 -j ACCEPT
$IP4T -A INPUT -p icmp --icmp-type 11/1 -j ACCEPT
$IP4T -A OUTPUT -p icmp --icmp-type 11/1 -j ACCEPT

##    ----  WIDE RANGE PERMISSION  ----    ##

# $IP4T -A OUTPUT -p TCP --sport first-os-port:rc1 -m state --state NEW,ESTABLISHED -j ACCEPT
# $IP4T -A OUTPUT -p UDP --sport first-os-port:rc1 -m state --state NEW,ESTABLISHED -j ACCEPT
# $IP4T -A INPUT -p TCP --dport first-os-port:rc1 -m state --state ESTABLISHED -j ACCEPT
# $IP4T -A INPUT -p UDP --dport first-os-port:rc1 -m state --state ESTABLISHED -j ACCEPT

##    ----  DLNA  ----    ##

$IP4T -A INPUT -p TCP --dport dlna-start:dlna-end -m state --state NEW,ESTABLISHED -j ACCEPT
$IP4T -A OUTPUT -p TCP --sport dlna-start:dlna-end -m state --state NEW,ESTABLISHED -m owner --uid-owner homma -j ACCEPT
$IP4T -A INPUT -p UDP --dport dlna-start:dlna-end -m state --state NEW,ESTABLISHED -j ACCEPT
$IP4T -A OUTPUT -p UDP --sport dlna-start:dlna-end -m state --state NEW,ESTABLISHED -m owner --uid-owner homma -j ACCEPT

##    ----  ----    ##
# Ignore ICMP redirect packets.
$IP4T -A INPUT -p icmp --icmp-type redirect -j LOG
$IP4T -A FORWARD -p icmp --icmp-type redirect -j DROP
$IP4T -A OUTPUT -p icmp --icmp-type redirect -j DROP

# FTP for OTA (No needed for this model)
# $IP4T -A OUTPUT -p tcp --sport 1024:65535 --dport 20 -m state --state NEW,ESTABLISHED -j ACCEPT
# $IP4T -A OUTPUT -p tcp --sport 1024:65535 --dport 21 -m state --state NEW,ESTABLISHED -j ACCEPT
# $IP4T -A INPUT -p tcp --sport 20 --dport 1024:65535 -m state --state ESTABLISHED,RELATED -j ACCEPT
# $IP4T -A INPUT -p tcp --sport 21 --dport 1024:65535 -m state --state ESTABLISHED,RELATED -j ACCEPT

# NTP client
$IP4T -A INPUT -p udp --sport ntp -m state --state ESTABLISHED -j ACCEPT
$IP4T -A OUTPUT -p udp --dport ntp -m state --state NEW,ESTABLISHED -j ACCEPT

# For Remote APP
# $IP4T -A INPUT -p udp --dport remote-app -m state --state NEW,ESTABLISHED -j ACCEPT
# $IP4T -A INPUT -p tcp --dport remote-app-upnp -m state --state NEW,ESTABLISHED -j ACCEPT
# $IP4T -A OUTPUT -p tcp --dport remote-app-upnp -m state --state NEW,ESTABLISHED -j ACCEPT
