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
# $brcm_Workfile: generate_tests.py $
# $brcm_Revision: Hydra_Software_Devel/9 $
# $brcm_Date: 9/10/12 6:11p $
#
# [File Description:]
# Mux Test Generation and Build
#
# Revision History:
#
# $brcm_Log: /rockford/unittests/syslib/muxlib/tests/generate_tests.py $
# 
# Hydra_Software_Devel/9   9/10/12 6:11p delkert
# SW7425-3912: Add argument to specify configuration file. Use platform
# from environment to select configuration file. Remove platform
# specific information from common config file
#
# Hydra_Software_Devel/8   5/10/12 3:31p delkert
# SW7425-2567: Add clean options to generate script. Fix up verification
# exception handling.  Separate config handling from generate/build so
# verification does not have build dependencies
#
# Hydra_Software_Devel/7   5/9/12 12:41p delkert
# SW7425-2567: Add option to script to allow additional build options to
# be specified
#
# Hydra_Software_Devel/6   5/3/12 5:48p delkert
# SW7425-2567: Add a "no clean" option. Simplify build command to be list
# of options (make is implied).  Explicitly clean nexus/bin for nexus
# apps. Copy perms when copying files
#
# Hydra_Software_Devel/5   5/2/12 9:17p delkert
# SW7425-2567: Merge generate and build. Make app an object. Add app
# "instantiation". Add permissions checks
#
# Hydra_Software_Devel/4   4/23/12 10:29a delkert
# SW7425-2567: Add support for no-build option
#
# Hydra_Software_Devel/3   4/20/12 5:00p delkert
# SW7425-2567: Separated config file parsing into separate utility so
# that test-definitions can be updated on the fly
#
# Hydra_Software_Devel/2   4/18/12 2:58p delkert
# SW7425-2567: Improved error handling. Add support for multiline
# configuration values. Add target input file generation
#
# Hydra_Software_Devel/1   4/18/12 9:07a delkert
# SW7425-2567: Initial generate and build script for mux tests
#
#########################################################################

import sys
import os
import argparse
from test_suite import TestSuite, TestSuiteError

_version = '0.0.0.0'

_config_file_base = 'mux_tests.cfg'

if __name__ == '__main__':
   parser = argparse.ArgumentParser(description='Mux Test Generation/Build', formatter_class=argparse.ArgumentDefaultsHelpFormatter)
   parser.add_argument('dest_path', type=str, help='The destination path for generated and built artifacts for the tests')
   parser.add_argument('-config', dest='config_file', help="Specify the config file to use for the tests")
   parser.add_argument('--version', action='version', version='Generate Tests: Ver. ' + _version)
   parser.add_argument('-nb', '-no_build', dest='no_build', action='store_true', default=False, help="Generate, but don't build." \
         " Note: this assumes the applications have been pre-built via some other means")
   parser.add_argument('-nc', '-no_clean', dest='no_clean', action='store_true', default=False, help="Don't clean before build.")
   parser.add_argument('-bo', '-build_opts', dest='build_opts', help="Additional build options to use during build process")
   parser.add_argument('-clean', action='store_true', default=False, help="Clean all generated content from the current directory")
   parser.add_argument('-clean_all', action='store_true', default=False, help="Clean all generated content (current dir and generated directory(-ies))")
   args = parser.parse_args()

   if args.config_file:
      # if config file specified on command line, use that file (overrides platform selection)
      config_file = args.config_file
   else:
      # pull the platform from the environment
      # select the config file to use based on the platform detected
      try:
         platform = (os.environ['PLATFORM'], os.environ['BCHP_VER'])
      except KeyError:
         # plat not set ...
         print "Config File not specified (Platform not set)"
         sys.exit(-1)
      config_file = platform[0] + "_" + _config_file_base

   if args.clean or args.clean_all:
      try:
         suite = TestSuite(config_file, args.dest_path)
         suite.clean(args.clean_all)
      except TestSuiteError, why:
         print why
         sys.exit(-1)

   else:
      # standard generate/build
      try:
         suite = TestSuite(config_file, args.dest_path, build_opts=args.build_opts)
         suite.generate_and_build(args.no_build, args.no_clean)
      except TestSuiteError, why:
         print why
         sys.exit(-1)

   # success
   sys.exit(0)
