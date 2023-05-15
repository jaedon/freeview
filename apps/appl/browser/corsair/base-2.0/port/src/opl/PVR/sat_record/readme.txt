REMOTE RECORD RECEIVER SAMPLE CODE
(c) Red Bee Media 2012, all rights reserved
This code is strictly confidential and may only be used or
reproduced under strict licence

This source code is provided as part of the SES Remote Recording
Receiver specification version as listed below and is based on
work by S&T, TV Genius, and SES S.A.

CONTENTS
========

readme.txt - this file

rr./h - the source code for calculating and parsing receiver registration information according to the specification

main.c - a test harness for the code

md5.c/h - the reference implementation of the md5 hash algorithm from http://people.csail.mit.edu/rivest/Md5.c

crc.c/h - public domain implementations of CRC suitable for embedded devices from 
http://www.barrgroup.com/Embedded-Systems/How-To/CRC-Calculation-C-Code


remote_record_sample.exe - a windows executatable that is compiled with the test harness.

syntax
   remote_record_sample <receiverId> <platform_id> <manufacturer_id> <tuner_config>
   remote_record_sample -test runs through each combination for every 100 received IDs, confirming that the coded results can be decoded successfully.

examples:
   remote_record_sample 184537583 1 34 3
   remote_record_sample 148572648 1 22 1
   remote_record_sample -test
