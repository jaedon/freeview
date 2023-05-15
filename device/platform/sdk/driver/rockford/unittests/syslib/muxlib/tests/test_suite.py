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
# $brcm_Workfile: test_suite.py $
# $brcm_Revision: Hydra_Software_Devel/21 $
# $brcm_Date: 9/10/12 5:58p $
#
# [File Description:]
# Test Suite Handler
# provides parsing of configuration file for Generation, Build,
# Execution and Analysis/Verification of necessary tests
#
# Revision History:
#
# $brcm_Log: /rockford/unittests/syslib/muxlib/tests/test_suite.py $
# 
# Hydra_Software_Devel/21   9/10/12 5:58p delkert
# SW7425-3912: Add support for including common configuration files. Add
# support for imported values. Fix up case insensitivity for keys.
# Enable substitution in iteration commands
#
# Hydra_Software_Devel/20   9/6/12 4:15p delkert
# SW7425-2567: Fix bug in contents of test_suites.txt file.
#
# Hydra_Software_Devel/19   6/13/12 11:41a delkert
# SW7425-3214: Don't stop on error if multiple tests specified unless -
# stop option given
#
# Hydra_Software_Devel/18   6/11/12 6:07p delkert
# SW7425-2567: Add source file existence verification to avoid segfaults
# in app and subsequent board reboots
#
# Hydra_Software_Devel/17   6/11/12 5:02p delkert
# SW7425-2567: Add print_tests() for use in verification script
#
# Hydra_Software_Devel/16   5/10/12 4:36p delkert
# SW7425-2567: Redirect sub-process stderr to stdout and flush stdout to
# ensure output from subprocesses appears in correct order
#
# Hydra_Software_Devel/15   5/10/12 3:31p delkert
# SW7425-2567: Add clean options to generate script. Fix up verification
# exception handling.  Separate config handling from generate/build so
# verification does not have build dependencies
#
# Hydra_Software_Devel/14   5/9/12 12:41p delkert
# SW7425-2567: Add option to script to allow additional build options to
# be specified
#
# Hydra_Software_Devel/13   5/4/12 4:38p delkert
# SW7425-2567: Add test_out_path keyword to all tests. Move directory
# make to generate step (not required for verify). Add verification of
# test results
#
# Hydra_Software_Devel/12   5/4/12 9:51a delkert
# SW7425-2567: Improve generated runner script to allow running all tests
# or all tests in a group for use in standalone testing
#
# Hydra_Software_Devel/11   5/3/12 5:48p delkert
# SW7425-2567: Add a "no clean" option. Simplify build command to be list
# of options (make is implied).  Explicitly clean nexus/bin for nexus
# apps. Copy perms when copying files
#
# Hydra_Software_Devel/10   5/3/12 11:03a delkert
# SW7425-2567: Switch ordering of build and gen_suites steps so suite
# listings are available regardless of build status
#
# Hydra_Software_Devel/9   5/2/12 9:17p delkert
# SW7425-2567: Merge generate and build. Make app an object. Add app
# "instantiation". Add permissions checks
#
# Hydra_Software_Devel/8   4/30/12 8:24p delkert
# SW7425-2567: Add support for parameter sets and test iteration
#
# Hydra_Software_Devel/7   4/25/12 6:18p delkert
# SW7425-2567: Move generated artifacts to "autogen" dir. Fix up
# locations of generated content for BAT. Improve error checking. Fix up
# permissions issues. Target input handling automatically handles nexus
# app now.
#
# Hydra_Software_Devel/6   4/24/12 4:52p delkert
# SW7425-2567: Fix up handling of nexus apps. Make directory for storing
# artifacts.
#
# Hydra_Software_Devel/5   4/23/12 10:29a delkert
# SW7425-2567: Create test-specific output directories.  Add support for
# no-build option
#
# Hydra_Software_Devel/4   4/22/12 10:20p delkert
# SW7425-2567: Add app build and copy
#
# Hydra_Software_Devel/3   4/20/12 5:00p delkert
# SW7425-2567: Separated config file parsing into separate utility so
# that test-definitions can be updated on the fly
#
# Hydra_Software_Devel/2   4/18/12 2:58p delkert
# SW7425-2567: Improved error handling. Add support for multiline
# configuration values. Add target input file generation
#
# Hydra_Software_Devel/1   4/18/12 9:08a delkert
# SW7425-2567: Initial test suite config parser and generation tools
#
#########################################################################
#
#  Test suite handler script
#
#  Generates a list of tests to be executed, using a specified test suite configuration file
#  Generates the required command-line arguments for executing the various parts of the test
#  Generates the required input files that may be required for the execution (for example
#  command strings that get piped into a test)
#  Builds the specified applications
#  Copies required artifacts to specified location
#  Performs server-side post-execution analysis
#
# allows specification of static platform-specific parameters used for the test generation
# for example: directory on the STB/Target for sources and executable, directory for the execution of
# the analysis

# steps:
#  * copy test_suite script and test suite config file (called <suite_name>.cfg) to server directory
#  * build
#    => produces the app(s) to be be run on the target, and copies them to required location
#       (server-side; runner script will copy them to the target if necessary)
#  * generate (if build failed for a dependent app, then runner will not execute that test)
#    => produces runner script, any necessary command file(s), etc.
#  * obtain list of tests, generate schedule
#  * copy runner script to target
#  per-test...
#  * ON TARGET: run-target <path to server directory> <testname>
#  * ON SERVER: test_suite -analyze <testname>
#

# FIXME: expand this to support a TestSuites object that contains TestSuite objects.
# add suites and generate the suites file and the runner script from there.
# (would apps be comon between suites?)
# a suite is a single configuration file - there could be multiple configuration files

import os
import sys
import shutil
import datetime
import subprocess
import configfile

# code uses with statement
if sys.version_info < (2, 7):
    print "Require python 2.7 or greater"
    sys.exit(-1)

_config_section_key = 'test_name'
_config_subsection_key = 'group'
_config_iterate_key = 'iterate'
_config_include_key = 'include'
_config_import_marker = 'import'
_config_mandatory_globals = None
_config_mandatory_section_keys = ['test_name', 'app_name', 'app_path', 'target_command']
_config_allow_multiple = ['target_verify', 'target_requires', 'target_produces', 'generate']
_config_dont_substitute = ['build_opts', 'build_clean', 'target_command', 'verify_command']

_build_command = 'make'
_build_clean_option = 'clean'

# all generated artifacts are stored in this directory within the <dest_path>
# supplied to the TestSuite()
_gen_dir = "autogen"

_runner_filename = "run_tests.sh"
_suites_filename = "test_suites.txt"

# argument position of testname specified to runner script
_runner_testname_argument = "$1"
# argument position of server_path specified to runner script
_runner_server_path_argument = "$2"

_runner_header = """#!/bin/bash
##################################################################
#  GENERATED FILE - DO NOT MODIFY
#
#  {title}
#
#  Generated by: {program}
#  Generated on: {date}
#
##################################################################
"""

_runner_usage_template="""
set -e   # error-check all commands, and exit upon failure

if [ $# -ne 2 ]
then
   echo "Usage: {filename} <testname> <path-to-server>  "
   echo "---------------"
   echo "where:"
   echo "<path-to-server> is the location of the '{gen_dir}' directory"
   echo "<testname> can be any of:"
   echo
{list_of_tests}
   echo
   echo "or 'all' to run all tests"
   echo
   echo "or any of: "
   echo
{list_of_groups}
   echo
   echo "... to run the specified group of tests"
   exit -1
fi

if [ ! -e $2 ] || [ ! -d $2 ]
then
   echo "Unable to locate server-path: "$2
   exit -1
fi

echo "Server-path is "$2
ret=0
"""

_runner_body_template="""
case "$1" in
   "all" )
      test_list="{list_of_all_tests}"
      ;;
   {runner_test_groups}
   *)
      # default - just a single test
      test_list="$1"
      ;;
esac

for test in $test_list
do
   echo "Running test: "$test
   case "$test" in
      {runner_test_entries}
      *)
         echo "Invalid testname: $test"
         exit -1
         ;;
   esac
done

exit $ret
"""

_runner_test_group_template="""
   "{test_group}" )
      test_list="{list_of_tests}"
      ;;
"""

_runner_test_entry_template="""
      "{test_name}" )
         {verify_commands}
         {copy_to_commands}
         {target_command}
         {copy_from_commands}
         ;;
"""

_runner_test_verify_template="""if {verify_tests}
         then
            echo "Required File(s) not found: {file_list}"
            exit -1
         fi"""

_runner_failed_build_template = """
      "{test_name}" )
         echo "Application {app_name} failed to build for this test ... skipping test"
         ret=-1
         ;;
"""

class TestSuiteError(Exception):
   def __init__(self, msg):
      self.msg = msg
   def __str__(self):
      return "Error! %s" % self.msg

class TestVerifyError(Exception):
   pass

# local error raise if app fails to build
class AppBuildError(Exception):
   pass

#
# A single test-entry in the config file
# all access to test definitions includes on-demand substitution to ensure
# that previous modifications to definitions gives up-to-date information
#
class Test(object):
   def __init__(self, testname, config, dest_path):
      self.testname = testname
      # save the config - needed for substituting final values
      self._config = config
      # destination directory on the server
      self._dest_path = dest_path
      # obtain the raw section information so test creation can update it as needed
      self._test_info = config.get_section(testname, raw=True)
      self._runner_server_dir = config.get_value(None, 'server_path', raw=True)
      self._runner_gen_dir = config.get_value(None, 'gen_path', raw=True)
      # set the test_out_path key in the current test's section
      # (for use by subsequent verification stage)
      self._test_info['test_out_path'] = os.path.join(dest_path, testname)
      self.built_ok = False
      self._build_opts = ""

      # NOTE: all definitions may change when app information is processed
      # process target input definitions
      self._target_inputs = {}
      target_inputs = [key for key in self._test_info.keys() if key.startswith('target_input')]
      for key in target_inputs:
         num = key.split('target_input')[1] or '0'
         target_input_fname = testname+"_input"+num+".txt"
         # store the value in the list of target inputs for later file generation
         self._target_inputs[target_input_fname] = self._test_info[key]
         # replace the original definition in the test info with the new filename
         # (target inputs always come directly from the server)
         # this updated value is then used to write to the runner script
         self._test_info[key] = os.path.join(self._runner_gen_dir, target_input_fname)

      self._app = None

   def __getattr__(self, name):
      if name == 'test_info':
         # return the raw test info for use in app processing
         return self._test_info

      if name == 'test_entry_map':
         # mapping for filling in test-entry in target runner script
         test_entry_map = {}
         test_entry_map['test_name'] = self.testname
         test_entry_map['target_command'] = self._config.get_value(self.testname,'target_command')
         target_requires = self._config.get_value(self.testname, 'target_requires') or []
         copy_to_target = []
         for required in target_requires:
            # if required is not an absolute path, then copy from _gen_dir in server_path location
            if not os.path.isabs(required):
               required = os.path.join(self._runner_gen_dir, required)
               # if required is a directory on the server's gen_dir, then copy
               # everything in the directory to the target (only relevant for nexus apps)
               if os.path.isdir(required.replace('$2',self._dest_path,1)):
                  required = os.path.join(required, "*")
            copy_to_target.append("cp " + required + " .")
         test_entry_map['copy_to_commands'] = "\n".join(copy_to_target)
         # generate list of artifacts to copy from the target ...
         # NOTE: artifacts are stored in a test-specific directory at the specified server-path location
         target_produces = self._config.get_value(self.testname, 'target_produces') or []
         copy_from_target = []
         for produced in target_produces:
            # NOTE: the empty last item in the list ensures a trailing "/" on the path
            # (necessary to prevent a file rename during copy)
            copy_from_target.append("cp " + produced + " " + os.path.join(self._runner_server_dir,self.testname,""))
         test_entry_map['copy_from_commands'] = "\n         ".join(copy_from_target)
         # need the app name to indicate which app if it failed to build
         test_entry_map['app_name'] = self._config.get_value(self.testname, 'app_name')
         # if verification required, add an entry to verify existence of specified (input) file(s)
         verify = self._config.get_value(self.testname,'target_verify')
         if verify:
            if isinstance(verify,list):
               verify_entries = " || ".join(["[ ! -e " + filename + " ]" for filename in verify])
               filenames = ",".join(verify)
            else:
               verify_entries = "[ ! -e " + verify + " ]"
               filenames = verify
            test_entry_map['verify_commands'] = _runner_test_verify_template.format(verify_tests=verify_entries,
               file_list=filenames)
         return test_entry_map

      if name == 'target_inputs':
         return self._config.substitute(self._target_inputs, self.testname)

      if name == 'app':
         return self._app

      if name == 'build_opts':
         # first look for build options in the current test, then in globals, else return empty string
         opts = self._config.get_value(self.testname, name) or self._config.get_value(None, name) or ""
         return " ".join([opts,self._build_opts])

      if name == 'build_clean':
         # first look in current test, then in globals, else return True (clean unless specified otherwise)
         clean = self._config.get_bool(self.testname, name)
         if clean is None:
            clean = self._config.get_bool(None, name)
            if clean is None:
               return True
         return clean

      # get all other values directly from the test entry in the config
      value = self._config.get_value(self.testname, name)
      if value is None and name in _config_allow_multiple:
         return []
      return value

   def set_app(self, app):
      # if this app is an "instance" of another app, it will have a
      # different name, so modify the config for this test to reflect
      # the new name
      # NOTE: self._test_info is a *reference* to the original section
      # for this test, so any changes to it affect all subsequent operations
      if app.orig_name != app.name:
         for key in self._test_info:
            value = self._test_info[key]
            if isinstance(value, str) and (value.find(app.orig_name) != -1):
               self._test_info[key] = value.replace(app.orig_name, app.name)
      # save the app incase its required later
      self._app = app

   def set_build_opts(self, opts):
      self._build_opts = opts

# a unique application as defined by a test in the config file
# if an app has the same name but a different location then
# it is unique. If it has the same name and path, but different build
# parameters it is also a unique app
class App(object):
   def __init__(self, test, suite):
      # construct a new app object from the information defined by the
      # specified test
      self._suite = suite    # the test suite
      self.orig_name = test.app_name
      self._path = test.app_path
      self._build_clean = test.build_clean
      self._build_opts = test.build_opts
      self._search_path = None
      self._full_path = None

      # app is assumed to have built ok unless determined otherwise
      # allows a "no build" option for pre-built apps.
      self.build_ok = True

      # list of tests that require this app...
      self.tests = [test]

      # if an app has several instances that differ only by their
      # build options, then those apps MUST be rebuilt each time
      self.requires_rebuild = False

      # see if there is an existing app with a matching name ...
      matching_app = None
      for app in suite._apps:
         if test.app_name == app.orig_name:
            matching_app = app
            break
      if matching_app:
         # same app as an existing app, but path or build (or both) differs
         if (matching_app._path == test.app_path) and (matching_app._build_opts != test.build_opts):
            # app has same name/path, but different build params,
            # so app requires a rebuild before generation
            self.requires_rebuild = True
         # so make a new version of the existing app
         self._instance = matching_app._instance + 1
         # this is the name the test will be referred to by the runner script
         self.name = '%s_%s' % (test.app_name, self._instance)
      else:
         # completely different app
         self._instance = 0
         self.name = test.app_name

      try:
         # see if the target is a nexus app by looking at 'target_command' ...
         test.target_command.index('nexus')
      except ValueError:
         self.is_nexus = False
      else:
         self.is_nexus = True

   def match(self, test):
      # original name, path and build must be same for the app to be a match
      return (self.orig_name == test.app_name and self._path == test.app_path and
          self._build_opts == test.build_opts)

   def append(self, test):
      # add the test to the list of tests this app is required by
      self.tests.append(test)

   def set_path(self):
      # go to the app location
      try:
         os.chdir(self._path)
      except OSError:
         self.build_ok = False
         print "App Path does not exist. ",
         raise AppBuildError()

   def build(self, no_clean=False):
      do_clean = (not no_clean and self._build_clean)
      # for a nexus app, if build_clean, then explicitly do the rm -f {view_path}/nexus/bin/*
      if self.is_nexus and do_clean:
         # clean out the nexus/bin dir (if it exists)
         nexus_bin = os.path.join(self._suite._view_path, "nexus", "bin")
         if os.path.exists(nexus_bin):
            for root, dirs, files in os.walk(nexus_bin):
               for name in files:
                  os.remove(os.path.join(root, name))
      try:
         sys.stdout.flush()
         if do_clean:
            subprocess.check_call([_build_command, _build_clean_option], stderr=subprocess.STDOUT)
         build_args = [_build_command]
         if self._build_opts:
            build_args.extend(self._build_opts.split())
         subprocess.check_call(build_args, stderr=subprocess.STDOUT)
         sys.stdout.flush()
      except subprocess.CalledProcessError as failed:
         # return code was non-zero (failed.returncode contains the return code)
         self.build_ok = False
         raise AppBuildError()

   # verify the target(s) exists
   def verify(self):
      if self.is_nexus:
         # look in nexus/bin for app
         self._search_path = os.path.join(self._suite._view_path, "nexus", "bin")
         self._full_name = os.path.join(self._search_path, self.orig_name)
         if not os.path.exists(self._full_name):
            # if not found, look in curr directory
            self._search_path = "."
            self._full_name = os.path.join(self._search_path, self.orig_name)
            if not os.path.exists(self._full_name):
               self.build_ok = False
               raise AppBuildError()
      else:
         # can't determine DEBUG vs Release, so look for Debug directory, and if not found, look for release directory
         # look in <platform>.<chipver>.debug
         platform = self._suite._platform[0]
         chip_ver = self._suite._platform[1]
         self._search_path = os.path.join(self._path, platform + "." + chip_ver + ".debug")
         self._full_name = os.path.join(self._search_path, self.orig_name)
         if not os.path.exists(self._full_name):
            # if not found look in <platform>.<chipver>.release
            self._search_path = os.path.join(self._path, platform + "." + chip_ver + ".release")
            self._full_name = os.path.join(self._search_path, self.orig_name)
            if not os.path.exists(self._full_name):
               self.build_ok = False
               raise AppBuildError()

   def copy(self, dest_path):
      # copy each app to the specified destination ...
      if self.is_nexus:
         # copy path/* dest_dir/<app_name>/
         dest_dir = os.path.join(dest_path, self.name, "")
         if os.path.exists(dest_dir):
            print "Dest. Exists (overwriting) ...",
            # copytree won't work if directory already exists, so remove it
            # (NOTE: for safety, this will only remove the top-level files)
            for root, dirs, files in os.walk(dest_dir):
               for name in files:
                  os.remove(os.path.join(root, name))
            os.rmdir(dest_dir)
         if self._instance:
            # rename the original source file before copying the directory
            try:
               os.rename(self._full_name, os.path.join(self._search_path, self.name))
            except OSError, IOError:
               self.build_ok = False
               raise AppBuildError
         try:
            shutil.copytree(self._search_path, dest_dir)
         except (shutil.Error, OSError, IOError):
            self.build_ok = False
            raise AppBuildError
      else:
         # copy app to dest_dir (this will silently overwrite an existing file)
         # In theory, this shouldn't fail
         # NOTE: For "instance" apps, this will copy from original name to new name
         # (i.e. it simultaneously renames the file)
         app_name = os.path.join(self._search_path, self.orig_name)
         try:
            shutil.copyfile(app_name, os.path.join(dest_path, self.name))
            shutil.copystat(app_name, os.path.join(dest_path, self.name))
         except (shutil.Error,IOError):
            self.build_ok = False
            raise AppBuildError

   def update_tests(self):
      for test in self.tests:
         test.built_ok = self.build_ok

   def __str__(self):
      if self._instance:
         return "%s[%s]" % (self.orig_name, self._instance)
      else:
         return self.orig_name

#
# Class to handle generate/build operations on a test-suite (derived from a config file)
#
# All generated content goes into <dest_path>/_gen_dir or a subdirectory within this
# (the use of a subdirectory ensures test-artifacts are separated from test execution
#  environment artifacts - e.g. BAT logs, etc)
# All test-specific artifacts produced by the executed test go into <dest_path>/<testname>
#
# build_opts is an optional list of additional build options to be added to each test
#
#  additional arguments (not implemented yet):
#  log <logfile>  => output to logfile as well as stdout
#  quiet          => no stdout, but if -log specified, goes to logfile
#  stop           => stop upon first failure (otherwise will continue until done)
class TestSuite(object):
   def __init__(self, config_file, dest_path, build_opts=None, config_only=False):
      # NOTE: this is provided the filename (and path) to the configuration file for the test suite
      # since this script may reside in a different location to the config file; the script is executed
      # from where it resides, not where the config file is
      self._config_fname = os.path.split(config_file)[1]

      # save the current directory this script was executed from
      self.cwd = os.getcwd()
      # verify that the current execution directory is writeable (content needs to be generated there)
      if not os.access(self.cwd, os.W_OK):
         raise TestSuiteError("Current Directory must be writeable (generated content cannot be written)")

      # dest_path must be an absolute path
      if not os.path.isabs(dest_path):
         raise TestSuiteError("Destination Path must be an absolute path")
      if not os.path.exists(dest_path):
         raise TestSuiteError("Destination Path not found")
      if not os.access(dest_path, os.W_OK):
         raise TestSuiteError("Destination Path is not writeable")
      # verify that dest dir is NOT the current directory!
      if dest_path == self.cwd:
         raise TestSuiteError("Destination Path must not be the current directoy!")

      # NOTE: all generated content must be output to (or copied to) "dest_path"/_gen_dir
      self._dest_path = os.path.normcase(dest_path)
      # NOTE: the genpath ensures that user cannot supply a path such that
      # this script will accidentally remove/overwrite files it shouldn't!
      # (e.g. if dest_path is /
      self._gen_path = os.path.join(self._dest_path, _gen_dir)

      self._process_config(config_file)

      self.suitename = self._config.get_value(None, 'suite_name')
      if self.suitename:
         print "Suite: " + self.suitename + "\n"

      self._process_tests(config_only)

      if config_only:
         # don't process the apps or anything related to generation/build
         # (mainly to allow verification to be done without build dependencies)
         return

      # add external build options to the tests
      for test in self._tests:
         if build_opts:
            test.set_build_opts(build_opts)

      # process plat to obtain chip and version for build
      try:
         self._platform = (os.environ['PLATFORM'], os.environ['BCHP_VER'])
      except KeyError:
         # plat not set ...
         # this is catastrophic since all tests will fail - no point continuing
         raise TestSuiteError("Platform not set. Require 'plat' command")

      # determine the filename for the suite filename
      # NOTE: if no platform provided in the config, use platform from "plat"
      platform = self._config.get_value(None, 'platform_config') or self._platform[0]
      self._suite_filename = platform + "_test_suite.txt"

      # create a list of apps required by the tests
      # NOTE: app info processing (may) modify the test definitions, so dont
      # access substituted definitions until after apps processed, since they may change
      self._process_apps()

   def _process_config(self, config_file):
      print "==== Processing Configuration: %s ====\n" % self._config_fname
      # setup default definitions for configuration file parser...
      default_defs = {}
      # set view_path common definition
      # view_path is path to this script, stripped at the "vobs" location
      (base, sep, tail) = sys.path[0].partition("vobs")
      print "Current View Path: ",
      if not sep:
         raise TestSuiteError("Unable to determine view_path")
      self._view_path = os.path.join(base, sep)
      print self._view_path + "\n"
      default_defs['view_path'] = self._view_path
      # server_path as provided by test environment to the generated runner script
      default_defs['server_path'] = _runner_server_path_argument
      # gen_path is the location of generated artifacts as seen by the runner script
      default_defs['gen_path'] = os.path.join(_runner_server_path_argument, _gen_dir)

      self._config = configfile.ConfigFile(default_defs)
      self._config.set_section_key(_config_section_key)
      self._config.set_subsection_key(_config_subsection_key)
      self._config.set_iteration_key(_config_iterate_key)
      self._config.set_include_key(_config_include_key)
      self._config.set_import_marker(_config_import_marker)
      self._config.set_mandatory_section_keys(*_config_mandatory_section_keys)
      self._config.allow_multiple(*_config_allow_multiple)
      self._config.dont_substitute(*_config_dont_substitute)
      print "[Reading ] %s ..." % self._config_fname,
      try:
         self._config.parse(config_file)
      except (configfile.Error, configfile.ParseError), why:
         raise TestSuiteError(why)
      print "OK"

   def _process_tests(self, suppress_output=False):
      # FIXME: generate an inclusion/exclusion list based on supplied list of tests (if any)
      self._exclusions = []

      # create a list of tests, and determine their information
      self._tests = []
      self._groups = {}      # information about groups of tests
      testnames = [x for x in self._config.get_sections() if x not in self._exclusions]
      for testname in testnames:
         # FIXME: add friendly name if one exists

         # construct each test based on information from the config file
         test = Test(testname, self._config, self._dest_path)
         if test.iterate:
            orig_name = test._test_info['__name__']
            if orig_name in self._groups:
               self._groups[orig_name].append(test.testname)
            else:
               if not suppress_output:
                  print "Found Test Group: " + orig_name
               self._groups[orig_name] = [test.testname]
         self._tests.append(test)
         if not suppress_output:
            print "Found Test: " + testname
      print

   def _process_apps(self):
      # create the apps list, including determining if different app names required
      self._apps = []
      for test in self._tests:
         # verify paths are valid
         if not os.path.isabs(test.app_path):
            raise TestSuiteError("Test %s :: App path must be absolute" % test.testname)
         if not test.app_path.startswith(self._view_path):
            raise TestSuiteError("Test %s :: App path must be within the current view" % test.testname)

         # see if there is an existing app that matches the one for this test
         matching_app = None
         for app in self._apps:
            if app.match(test):
               matching_app = app
         if matching_app:
            # app is exactly the same as an existing app
            # append the current test to the test list for this app
            # (everything else stays the same)
            matching_app.append(test)
            test.set_app(matching_app)
         else:
            # this app is different (name, path or build params differ),
            # so create a new app and add it to the list of apps
            new_app = App(test, self)
            self._apps.append(new_app)
            test.set_app(new_app)

      for app in self._apps:
         print "Found %s: %s (used by tests): \n  - %s" % (("Nexus App. " if app.is_nexus else "Application"),
            app, "\n  - ".join([test.testname for test in app.tests]))
      print

   def _make_directories(self):
      # make the directory for generated content
      # NOTE: This is an absolute path
      print "[Make Dir] %s ... " % os.path.split(self._gen_path)[1],
      try:
         os.mkdir(self._gen_path)
      except OSError:
         print "Exists (overwrite)"
      else:
         # NOTE: this directory should be writeable by the current process
         print "OK"

      # make the output directories for each test
      # NOTE: These are absolute paths
      for test in self._tests:
         print "[Make Dir] %s ..." % test.testname,
         test_dir = os.path.join(test.test_out_path, "")
         try:
            os.mkdir(test_dir)
         except OSError:
            print "Exists (overwrite)"
         else:
            # ensure created directories are writeable by all
            # (since the runner script on the STB needs to write to them)
            os.chmod(test_dir, 0777)
            print "OK"
      print

   def _gen_test_suites(self):
      # assert directory is script's execution directory
      # (generated test suites files must go there)
      assert(os.getcwd() == self.cwd)

      # generate YYY_test_suite.txt file(s) containing a list of tests
      # (YYY = platform)
      print "[Generate] %s ..." % self._suite_filename,
      with open(self._suite_filename, "w") as suite_file:
         suite_file.write("\n".join([test.testname for test in self._tests]))
      # NOTE: this will silently overwrite an existing file
      shutil.copy2(self._suite_filename, self._gen_path)
      print "OK"

      # generate test_suites.txt file which contains a list of test suite files
      print "[Generate] %s ..." % _suites_filename,
      with open(_suites_filename, "w") as suites_file:
         suites_file.write(self._suite_filename)
      # copy the generated files to "gen_path" for storage as artifacts
      # NOTE: this will silently overwrite an existing file
      shutil.copy2(_suites_filename, self._gen_path)
      print "OK\n"

   def _gen_target_runner(self):
      # assert directory is script's execution directory
      # (generated runner script must go there)
      assert(os.getcwd() == self.cwd)

      print "[Generate] %s ..." % _runner_filename,
      # generate information for script header ...
      suite_name = self.suitename
      if suite_name is None:
         # no suitename provided, so create one from config filename
         suite_name, junk = os.path.splitext(self._config_fname)
         suite_name = suite_name.replace("_"," ").capitalize()

      header_info = { 'title': suite_name+" Runner",
                      'program': os.path.split(sys.argv[0])[1],
                      'date': datetime.datetime.now().strftime("%Y-%m-%d %H:%M") }

      # generate information for the script usage section ...
      tests_list = ["   echo \"   " + test.testname + "\"" for test in self._tests]
      group_list = ["   echo \"   " + group + "\"" for group in self._groups]
      usage_info = { 'filename': _runner_filename,
                     'gen_dir' : _gen_dir,
                     'list_of_tests': "\n".join(tests_list),
                     'list_of_groups': "\n".join(group_list) }

      # generate information for the script body...
      all_groups = []      # list of all groups, for use in script body
      test_entries = []    # for creating test entries in the script body
      for test in self._tests:
         if test.built_ok:
            test_entries.append(_runner_test_entry_template.format(**test.test_entry_map))
         else:
            test_entries.append(_runner_failed_build_template.format(**test.test_entry_map))
      for group in self._groups:
         all_groups.append(_runner_test_group_template.format(test_group=group, list_of_tests=" ".join(self._groups[group])))

      body_info = { 'list_of_all_tests': " ".join([test.testname for test in self._tests]),
                    'runner_test_groups': " ".join(all_groups),
                    'runner_test_entries': " ".join(test_entries) }

      # NOTE: open as binary to ensure file is written as Unix mode
      with open(_runner_filename, "wb") as runner_script:
         runner_script.write(_runner_header.format(**header_info))
         runner_script.write(_runner_usage_template.format(**usage_info))
         runner_script.write(_runner_body_template.format(**body_info))

      # ensure file is executable by all
      os.chmod(_runner_filename, 0777)
      # copy target runner to _gen_path for storage of artifacts
      # (will overwrite an existing file)
      shutil.copy2(_runner_filename, self._gen_path)
      print "OK\n"

   def _gen_test_specific(self):
      # change directory to the _gen_path so that generated artifacts required by
      # the tests (required by the application or the runner script) go there
      os.chdir(self._gen_path)

      for test in self._tests:
         print "--- Test: %s ---" % test.testname
         if not test.built_ok:
            print "Nothing to generate ... failed to build"
            continue

         generate_list = test.generate
         target_inputs = test.target_inputs
         if not generate_list and not target_inputs:
            print "Nothing to generate ... Test done"
            continue

         if generate_list:
            # for each generation command encountered in the test configuration, run the command
            for generate in generate_list:
               print "[Execute ] " + generate,
               try:
                  sys.stdout.flush()
                  # FIXME: This is dangerous, and we should probably check for commands such as rm, etc to prevent
                  # accidental problems !
                  # check for and disallow some commands (such as rm)
                  subprocess.check_call(generate, stderr=subprocess.STDOUT, shell=True)
                  sys.stdout.flush()
               except subprocess.CalledProcessError as failed:
                  # return code was non-zero (failed.returncode contains the return code)
                  raise TestSuiteError("Test %s :: Failed to invoke generation command (Return Code: %s):\n" % (test.testname, failed.returncode) + generate)
               print "OK"

         if target_inputs:
            # Process target_input generations
            # (always write the specified value to a file named <test_name>_inputX.txt)
            for target_input_fname,value in target_inputs.iteritems():
               print "[Generate] " + target_input_fname + " ...",
               with open(target_input_fname, "w") as input_file:
                  input_file.write(value)
               print "OK"
         print
         # -- end: for each test --
      # restore current directory
      os.chdir(self.cwd)

   #  tests TEST,... => specify a list of tests to generate and/or build
   #  ex TEST,...    => specify a list of tests to exclude from generate/build

   # build the application(s) required for the specified tests
   def _build_apps(self, no_build, no_clean):
      for app in self._apps:
         print "[Build   ] %s ..." % app
         try:
            # go to app dir
            app.set_path()
            # if an app has multiple instances due to build param changes,
            # then the app must be rebuilt each time
            if no_build and app.requires_rebuild:
               raise TestSuiteError("App requires a rebuild")
            if not no_build:
               app.build(no_clean)
               print "OK"
            else:
               print "Skipping build"
            # verify app exists (i.e. was built correctly, or pre-exists)
            print "[Verify  ] %s ..." % app,
            app.verify()
            print "OK"
            # copy the app to the specified location
            print "[Copy    ] %s ..." % app,
            app.copy(self._gen_path)
            print "OK"
         except AppBuildError:
            # app failed to build correctly
            print "Failed"
         print
         # update tests requiring this app to indicate build status
         app.update_tests()

      # restore current directory to starting directory
      os.chdir(self.cwd)

      # check if any app built - testing will proceed if at least one app built ok
      if not any([app.build_ok for app in self._apps]):
         raise TestSuiteError("All apps failed to build!")

   # verify the existence of the apps required for the tests, copy
   # them to the generated artifacts directory, and then generate all
   # other artifacts required for the tests (input files, test suite
   # lists, target runner script, etc)
   def generate_and_build(self, no_build=False, no_clean=False):
      # make the directories required by generate(), build() and verify()
      self._make_directories()

      # generate the global test suite definitions files (used by test environment - e.g. BAT)
      # NOTE: this generates files in the current directory, which are then also copied to _gen_path
      # (this step is done first so that even if build fails, the list of suites/tests is available)
      self._gen_test_suites()

      # build the applications required by the tests, verify build
      # OK and copy app to gen_dir (NOTE: contents of runner script
      # change if apps fail to build)
      self._build_apps(no_build, no_clean)

      # generate target runner script ...
      # (runner script generated in script directory for access by test environment,
      #  and copied to _gen_path)
      self._gen_target_runner()

      # generate anything specified by "generate" commands in the config...
      # and generate target inputs
      self._gen_test_specific()

   # FIXME: add result logging capability (use Logger?)
   def verify(self, testname, stop=False):
      if testname == 'all':
         test_list = self._tests
      elif testname in self._groups:
         test_list = [test for test in self._tests if test.testname in self._groups[testname]]
      elif testname in [test.testname for test in self._tests]:
         test_list = [test for test in self._tests if test.testname == testname]
      else:
         raise TestVerifyError("Invalid Testname: Test %s not found" % testname)
      #if only one test to verify, always stop (i.e. raise Exception)
      if len(test_list) == 1:
         stop=True
      failed_tests = []
      for test in test_list:
         # execute the necessary verification command to verify the test operation
         print "[Verify ] %s ... " % test.testname
         if not test.verify_command:
            print "Skipping - no verification specified"
            continue
         try:
            sys.stdout.flush()
            # FIXME: This is dangerous, and we should probably check for commands such as rm, etc to prevent
            # accidental problems !
            # check for and disallow some commands (such as rm)
            subprocess.check_call(test.verify_command, stderr=subprocess.STDOUT, shell=True)
            sys.stdout.flush()
         except subprocess.CalledProcessError as failed:
            # return code was non-zero (failed.returncode contains the return code)
            if stop:
               raise TestVerifyError("Test %s :: Failed (Return Code: %s)" % (test.testname, failed.returncode))
            else:
               print "Test %s :: Failed (Return Code: %s)" % (test.testname, failed.returncode)
               failed_tests.append(test.testname)
         print
      if not stop and failed_tests:
         print "Failed Tests:"
         print "  \n".join(failed_tests)
         raise TestVerifyError("%s Test(s) Failed" % len(failed_tests))

   def clean(self, all=False):
      # assert directory is script's execution directory
      # (generated test suites files were created there)
      assert(os.getcwd() == self.cwd)

      # remove the generated files from the current directory
      for filename in [_runner_filename, _suites_filename, self._suite_filename]:
         if os.path.exists(filename):
            print "Removing %s ..." % filename
            os.remove(filename)
      if all:
         # remove the complete contents of generated directories
         if os.path.exists(self._gen_path):
            print "Cleaning generated directory '%s' ..." % _gen_dir
            try:
               shutil.rmtree(self._gen_path)
               pass
            except OSError:
               raise TestSuiteError("Unable to remove " + self._gen_path)
         for test in self._tests:
            print "Cleaning test output directory %s ..." % os.path.split(test.test_out_path)[1]
            if os.path.exists(test.test_out_path):
               try:
                  shutil.rmtree(test.test_out_path)
                  pass
               except OSError:
                  raise TestSuiteError("Unable to remove " + test.test_out_path)
         # do this last to minimize impact on imported modules
         # (don't use imported local modules here)
         pyc_files = [x for x in os.listdir('.') if os.path.splitext(x)[1] == '.pyc']
         if pyc_files:
            print "Removing Python Compiled Modules ..."
            for filename in pyc_files:
               os.remove(filename)

   def print_tests(self):
      print "Available Tests:"
      print "\n".join(["  " + test.testname for test in self._tests])
      print
      print "or any of: "
      print "\n".join(["  " + groupname for groupname in self._groups])
      print "... to run the specified group of tests"
      print "\nOr use 'all' to verify all tests"

if __name__ == '__main__':
   suite = TestSuite('./mux_tests.cfg', r'/projects/stbdev_ext2/delkert/shared')
   suite.build_and_generate()

