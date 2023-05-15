#! /usr/local/bin/python
#########################################################################
#     Copyright (c) 2003-2012, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: verify_test.py $
# $brcm_Revision: Hydra_Software_Devel/5 $
# $brcm_Date: 6/13/12 11:41a $
#
# [File Description:]
# Mux Test Verification
#
# Revision History:
#
# $brcm_Log: /rockford/unittests/syslib/muxlib/tests/verify_test.py $
# 
# Hydra_Software_Devel/5   6/13/12 11:41a delkert
# SW7425-3214: Don't stop on error if multiple tests specified unless -
# stop option given
#
# Hydra_Software_Devel/4   6/11/12 5:03p delkert
# SW7425-3214: Add option to show list of tests to verify
#
# Hydra_Software_Devel/3   5/10/12 3:31p delkert
# SW7425-2567: Add clean options to generate script. Fix up verification
# exception handling.  Separate config handling from generate/build so
# verification does not have build dependencies
#
# Hydra_Software_Devel/2   5/4/12 4:23p delkert
# SW7425-2567: Allow verification of groups of tests
#
# Hydra_Software_Devel/1   4/19/12 3:29p delkert
# SW7425-2567: Initial verification script
#
#########################################################################

import sys
import argparse
from test_suite import TestSuite, TestSuiteError, TestVerifyError

_version = '0.0.0.0'

_config_file = './mux_tests.cfg'

if __name__ == '__main__':
   parser = argparse.ArgumentParser(description='Mux Test Verification')

   parser.add_argument('test_name', metavar='<test_name>|<group_name>|all', type=str, help='The name of the test (or group of tests) to verify (Use "all" to verify all tests)')
   parser.add_argument('dest_path', type=str, help='The path to artifacts produced by the tests for input to verification')
   parser.add_argument('-show_tests', action='store_true', default=False, help='Show a list of possible tests')
   parser.add_argument('-stop', action='store_true', default=False, help='Stop on the first error found')
   parser.add_argument('--version', action='version', version='Mux Test Verification: Ver. ' + _version)

   args = parser.parse_args()

   try:
      suite = TestSuite(_config_file, args.dest_path, config_only=True)
      if args.show_tests:
         suite.print_tests()
      else:
         # run the verification tool necessary to verify the results of the specified test
         suite.verify(args.test_name,args.stop)
   except (TestSuiteError, TestVerifyError), why:
      print why
      sys.exit(-1)

   # success
   sys.exit(0)
