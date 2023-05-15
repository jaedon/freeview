#!/bin/sh

sysctl -p /etc/sysctl.conf


IP4T="/usr/local/sbin/iptables"


$IP4T -F
$IP4T -X
$IP4T -Z

$IP4T -P INPUT DROP
$IP4T -P OUTPUT DROP
$IP4T -P FORWARD DROP

$IP4T -A INPUT -m state --state INVALID -j DROP
$IP4T -A OUTPUT -m state --state INVALID -j DROP

$IP4T -A INPUT -i eth0 -s 127.0.0.1/8 -j DROP
$IP4T -A INPUT -i wlan0 -s 127.0.0.1/8 -j DROP

$IP4T -A INPUT -i eth0 -m addrtype --src-type UNSPEC,BLACKHOLE,PROHIBIT -j DROP
$IP4T -A INPUT -i wlan0 -m addrtype --src-type UNSPEC,BLACKHOLE,PROHIBIT -j DROP

$IP4T -A INPUT -i lo -j ACCEPT
$IP4T -A OUTPUT -o lo -j ACCEPT

$IP4T -A INPUT -p udp --sport domain -m state --state ESTABLISHED -j ACCEPT
$IP4T -A OUTPUT -p udp --dport domain -m state --state NEW,ESTABLISHED -j ACCEPT

$IP4T -A INPUT -p tcp --sport www --dport first-os-port:rc1 -m state --state ESTABLISHED -j ACCEPT
$IP4T -A OUTPUT -p tcp --sport first-os-port:rc1 --dport www -m state --state NEW,ESTABLISHED -j ACCEPT
$IP4T -A INPUT -p tcp --sport webcache --dport first-os-port:rc1 -m state --state ESTABLISHED -j ACCEPT
$IP4T -A OUTPUT -p tcp --sport first-os-port:rc1 --dport webcache -m state --state NEW,ESTABLISHED -j ACCEPT

$IP4T -A INPUT -p tcp --sport https --dport first-os-port:rc1 -m state --state ESTABLISHED -j ACCEPT
$IP4T -A OUTPUT -p tcp --sport first-os-port:rc1 --dport https -m state --state NEW,ESTABLISHED -j ACCEPT

$IP4T -A OUTPUT -p udp --sport bootpc --dport bootps -m state --state NEW,ESTABLISHED -j ACCEPT
$IP4T -A INPUT -p udp --sport bootps --dport bootpc -m state --state ESTABLISHED -j ACCEPT

$IP4T -A INPUT -p udp --dport ssdp -m state --state NEW,ESTABLISHED -j ACCEPT
$IP4T -A OUTPUT -p udp --sport ssdp -m state --state NEW,ESTABLISHED -j ACCEPT

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

$IP4T -A OUTPUT -p tcp --dport icslap -m state --state NEW,ESTABLISHED -j ACCEPT
$IP4T -A INPUT -p udp --dport wsd -m state --state ESTABLISHED -j ACCEPT

$IP4T -A INPUT -p icmp --icmp-type echo-reply -m state --state ESTABLISHED,RELATED -j ACCEPT
$IP4T -A OUTPUT -p icmp --icmp-type echo-reply -m state --state ESTABLISHED,RELATED -j ACCEPT
$IP4T -A INPUT -p icmp --icmp-type echo-request -j ACCEPT
$IP4T -A OUTPUT -p icmp --icmp-type echo-request -j ACCEPT
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

$IP4T -A INPUT -p TCP --dport dlna-start:dlna-end -m state --state NEW,ESTABLISHED -j ACCEPT
$IP4T -A OUTPUT -p TCP --sport dlna-start:dlna-end -m state --state NEW,ESTABLISHED -m owner --uid-owner homma -j ACCEPT
$IP4T -A INPUT -p UDP --dport dlna-start:dlna-end -m state --state NEW,ESTABLISHED -j ACCEPT
$IP4T -A OUTPUT -p UDP --sport dlna-start:dlna-end -m state --state NEW,ESTABLISHED -m owner --uid-owner homma -j ACCEPT

$IP4T -A INPUT -p icmp --icmp-type redirect -j LOG
$IP4T -A FORWARD -p icmp --icmp-type redirect -j DROP
$IP4T -A OUTPUT -p icmp --icmp-type redirect -j DROP

$IP4T -A INPUT -p udp --sport ntp -m state --state ESTABLISHED -j ACCEPT
$IP4T -A OUTPUT -p udp --dport ntp -m state --state NEW,ESTABLISHED -j ACCEPT

