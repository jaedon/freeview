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
# $brcm_Workfile: configfile.py $
# $brcm_Revision: Hydra_Software_Devel/10 $
# $brcm_Date: 9/10/12 5:58p $
#
# [File Description:]
# Configuration File Processor
#
# Revision History:
# $brcm_Log: /rockford/unittests/syslib/muxlib/tests/configfile.py $
# 
# Hydra_Software_Devel/10   9/10/12 5:58p delkert
# SW7425-3912: Add support for including common configuration files. Add
# support for imported values. Fix up case insensitivity for keys.
# Enable substitution in iteration commands
#
# Hydra_Software_Devel/9   6/12/12 2:30p delkert
# SW7425-3214: zero-pad count values in test names so tests sort
# correctly in EC. Improve test names for tests that use groups
#
# Hydra_Software_Devel/8   5/4/12 9:48a delkert
# SW7425-2567: retain the original section name from iterated sections
#
# Hydra_Software_Devel/7   5/3/12 5:43p delkert
# SW7425-2567: Fix preservation of whitespace in multiline values. Fix
# handling of keys that are not present in value conversion methods.
#
# Hydra_Software_Devel/6   5/2/12 9:17p delkert
# SW7425-2567: Merge generate and build. Make app an object. Add app
# "instantiation". Add permissions checks
#
# Hydra_Software_Devel/5   4/30/12 8:30p delkert
# SW7425-2567: Add support for sub-sections and iteration. Clean up end-
# section processing
#
# Hydra_Software_Devel/4   4/27/12 12:05p delkert
# SW7425-2567: Fix up error in substitutions
#
# Hydra_Software_Devel/3   4/26/12 6:27p delkert
# SW7425-2567: Improve error checking on substitutions.  Modify
# substitution to allow nested structs.
#
# Hydra_Software_Devel/2   4/24/12 4:49p delkert
# SW7425-2567: Fix error handling and add dialect checking. Allow for
# None returns on missing keys. Make keys case-insensitive. Add
# iterative substitutions and add override definitions. Add conversion
# helper methods.
#
# Hydra_Software_Devel/1   4/20/12 4:56p delkert
# SW7425-2567: Initial config file parser utility
#
#########################################################################

# parse configuration file of the form:
# key <sep> value
# where separator can be specfied
#
# Keys are case-insensitive (i.e. 'MY_KEY' is the same key as 'my_key')
# NOTE: keys are stored lower-case
# Values, however, are case-sensitive (and therefore, section names are
# also case-sensitive)
#
# sections can be indicated by specifying the key that identifies the
# start of the section.  The key's value is the section name
# substitution of values is not permitted in the section name
#
# multiline values can be supported by specifying the value that indicates
# the beginning of the multiline block - all lines following this line are
# taken to be part of the value up until a blank line is seen
#
# whitespace handling can be specified for both single-line values and
# multi-line values separately (since some applications require multi-line
# whitespace to be significant, but don't generally require whitespace
# preservation on other values)

# possible enhancements:
# * allow a "continuation" character, and any line that ends with this
#   will be considered to be multiline that ends with a line without it
# * allow partial value to be consumed for lines that mark a multiline
#   (currently, it expects no value data on the line that marks a multiline)
#   for example:
#   my_var: partial_value \
#        rest of value
#   => partial_value and rest_of_value would be combined into the value
#      NOTE: need to be careful of whitespace (may need to remove the
#      continuation char leaving whitespace intact)
# * add key case-sensitivity: if case sensitive dont lower() keys
#   may need a transform function since this would need to be done to
#   substitution map keys also
# * allow a "allow_multiple" key that appends entries to the allow_multiple list

import sys
import os
import itertools
from collections import OrderedDict

# code uses with statement, and ordered dictionaries
if sys.version_info < (2, 7):
    print "Require python 2.7 or greater"
    sys.exit(-1)

PRESERVE_ALL = 0
PRESERVE_LEADING = 1
PRESERVE_TRAILING = 2
PRESERVE_NONE = 3

# this is an arbitrary number, simply restricted to cap performance
# this allows 5 nested substitutions (unlikely to occur in normal use)
# increase this if DepthError frequently encountered
_MAX_SUBSTITUTION_ITERATIONS = 5

class Error(Exception):
   def __init__(self, msg):
      self.msg = msg
   def __str__(self):
      return self.msg
   __repr__ = __str__

class DialectError(Error):
   def __init__(self, dialect, msg):
      self.msg = "%s :: %s" % (dialect.__class__.__name__, msg)

class ParseError(Error):
   def __init__(self, config, msg):
      self.msg = "%s, line %s :: %s" % (os.path.split(config._filename)[1], config._line_num, msg)

class SubstitutionError(Error):
   def __init__(self, section=None, value=None, sub_into=None):
      self.section = section
      self.value = value  # this may be the key also
      self.sub_into = sub_into
      self.section_msg = ("Section '%s' :: " % section if section is not None else "")
      self.sub_into_msg = ("Key '%s' :: " % sub_into if sub_into is not None else "")
   def __str__(self):
      return "%s%sSubstitution Failed" % (section_msg, sub_into_msg)
   __repr__ = __str__

class SubstitutionTypeError(SubstitutionError):
   def __str__(self):
      return "%s%sSubstitution into Value '%s' of %s not supported" % (self.section_msg, self.sub_into_msg, self.value, type(self.value))

class SubstitutionMissingError(SubstitutionError):
   def __str__(self):
      return "%s%sSubstitution Key '%s' not found" % (self.section_msg, self.sub_into_msg, self.value)

class SubstitutionInvalidError(SubstitutionError):
   def __str__(self):
      return "%s%sInvalid Substitution Value for Key '%s'" % (self.section_msg, self.sub_into_msg, self.value)

class SubstitutionDepthError(SubstitutionError):
   def __str__(self):
      if self.value is not None:
         return "%s%sToo many substitutions: %s (max. %s supported)" + (self.section_msg, self.sub_into_msg, self.value, _MAX_SUBSTITUTION_ITERATIONS)
      else:
         return "%s%sToo many nested structures (%s levels supported)" % (self.section_msg, self.sub_into_msg, _MAX_NESTING_DEPTH)

class SectionError(Error):
   def __init__(self, section):
      self.msg = "Section '%s' not found" % section

# Create a sub-class of ConfigFileDialect to define new dialects to suit a particular purpose
class ConfigFileDialect(object):
   # placeholders
   allow_empty = False              # allow empty keys (keys without a value) => returns None for the key value, otherwise Error
   allow_multiline = False          # allow multi-line values with optional whitespace preservation
   allow_substitutions = False      # allow substitution of key value into other values
   allow_sections = False           # allow sections to provide grouping of key/value pairs
   allow_multiple = False           # allow multiple values for the same key => creates key : list[values...] pairing
   allow_iteration = False          # allow iteration over a range of values specified by a defined command
   allow_subsections = False        # allow sub-grouping of key/values within a section (mainly for specification of parameter sets for iteration)
   allow_includes = False           # allow inclusion of other configuration files
   allow_imports = False            # allow importing of a key's value from a file (verbatim input, no whitespace conversion)
   separator = ''
   multiline_marker = ''
   comment_string = ''
   preserve_order = False
   # whitepace preservation is for string values
   # NOTE: whitespace around keys is always stripped.
   # If a key has a transform associated with it, all whitespace stripped from the value first
   preserve_whitespace = PRESERVE_NONE
   # NOTE: PRESERVE_NONE for multi-line whitespace will concatentate lines together!
   preserve_multiline_whitespace = PRESERVE_NONE
   # if a key requested by get_value() is not found, KeyError will be raised unless this is
   # true, in which case None is returned
   missing_key_returns_none = False

   def __init__(self):
      if self.__class__ is ConfigFileDialect:
         raise DialectError("Invalid Dialect")
      allowed_preserved = [PRESERVE_NONE, PRESERVE_LEADING, PRESERVE_TRAILING, PRESERVE_ALL]
      if not self.separator:
         raise DialectError(self, "Separator must be specified")
      if self.allow_multiline and not self.multiline_marker:
         raise DialectError(self, "Multiline marker required to support multiline values")
      if self.preserve_whitespace not in allowed_preserved or \
         self.preserve_multiline_whitespace not in allowed_preserved:
            raise DialectError(self, "Invalid whitespace preservation")

# this default dialect is configured mainly to allow specification of
# tests, for automation, via the config file
class DefaultDialect(ConfigFileDialect):
   allow_empty = False
   allow_multiline = True
   allow_substitutions = True
   allow_sections = True
   allow_multiple = True
   allow_iteration = True
   allow_subsections = True
   allow_includes = True
   allow_imports = True
   separator = ':'
   multiline_marker = 'multiline'
   comment_string = '#'
   preserve_order = False
   preserve_whitespace = PRESERVE_NONE
   preserve_multiline_whitespace = PRESERVE_TRAILING
   missing_key_returns_none = True

class ConfigFile(object):

   # default_defs specifies a dictionary of pre-defined key/value pairs
   # that may not appear in the configuration file being parsed but
   # may be substituted into other key/values defined in the config file
   def __init__(self, default_defs=None, dialect=DefaultDialect):
      self._dialect = dialect()
      if self._dialect.preserve_order:
         self._dict = OrderedDict
      else:
         self._dict = dict

      self._definitions = self._dict()
      if default_defs:
         self._definitions.update(default_defs)

      self._section_key = None  # linear file
      self._iterate_key = None  # iteration not supported until this set
      self._subsection_key = None # sub-sections not supported until this set
      self._subsection_start = None
      self._subsection_end = None
      self._include_key = None  # included files not supported until this set
      self._import_key = None   # imported values not supported until this set
      self._mandatory_section_keys = []
      self._mandatory_global_keys = []
      self._allow_multiple = []
      self._dont_substitute = []
      self._sections = []
      self._curr_section = None
      self._curr_subsection = None

      # min depth is 1 for the section/globals and 1 for the string value
      # multiple   subsections  depth
      # N          N            2       Singular Item                      dict (section) (1) -> string (2)
      # Y          N            3       Multiple Items                     dict (section) (1) -> list (2) -> string (3)
      # N          Y            4       Subsection with Singular Item      dict (section) (1) -> list (2) -> dict (sub) (3) -> string (4)
      # Y          Y            5       Subsection with Multiple Items     dict (section) (1) -> list (2) -> dict (sub) (3) -> list (4) -> string (5)

      # subsections are always a list of dict, but may contain just one item
      # dict (section) -> dict is not possible, nor is: dict (sub) -> dict
      # nested subsections not supported, so we won't see: dict (sub) -> list -> dict
      # multiple items (lists) and subsections (dicts) cannot be substituted into values

      self._max_nesting_depth = 2
      if self._dialect.allow_multiple:
         # add one for the list item to contain multiple value strings
         self._max_nesting_depth += 1
      if self._dialect.allow_subsections:
         # add two for the list containing the group and the dictionary of grouped items
         self._max_nesting_depth += 2

      self._filename = None
      self._line_num = 0
      self._parse_level = 0

   # specify the key to use for defining a section
   def set_section_key(self, key):
      if not self._dialect.allow_sections:
         raise DialectError(self._dialect, "Sections not supported")
      self._section_key = key.lower()

   # specify the key to use for defining iteration
   def set_iteration_key(self, key):
      if not self._dialect.allow_iteration:
         raise DialectError(self._dialect, "Iteration not supported")
      self._iterate_key = key.lower()

   # the sub-section key is the name of the key to use to identify use of a sub-section and its "start"/"stop" markers
   # e.g. sub-section key = "group" allows use of:
   #  iterate : group <groupname>
   #  group_start: <groupname>
   #...
   #  group_end: <groupname>
   # => this will always create the subsection start/stop markers: <subsection_key>_start and <subsection_key>_end
   def set_subsection_key(self, key):
      if not self._dialect.allow_subsections:
         raise DialectError(self._dialect, "Subsections not supported")
      self._subsection_key = key.lower()
      self._subsection_start = key.lower() + '_start'
      self._subsection_end = key.lower() + '_end'

   # specify the key used to identify an include
   def set_include_key(self, key):
      if not self._dialect.allow_includes:
         raise DialectError(self._dialect, "Includes not supported")
      self._include_key = key.lower()

   # specify the marker used to indicate an imported value
   def set_import_marker(self, marker):
      if not self._dialect.allow_imports:
         raise DialectError(self._dialect, "Imported Values not supported")
      self._import_marker = marker.lower()

   # specifies the list of keys that MUST appear in a section
   def set_mandatory_section_keys(self, *mandatory_keys):
      if not self._dialect.allow_sections:
         raise DialectError(self._dialect, "Sections not supported")
      self._mandatory_section_keys = list(mandatory_keys)

   # specifies the list of keys that MUST appear in the globals
   def sef_mandatory_global_keys(self, *mandatory_keys):
      self._mandatory_global_keys = list(mandatory_keys)

   # specifies the list of keys that support multiple values
   def allow_multiple(self, *key_list):
      if not self._dialect.allow_multiple:
         raise DialectError(self._dialect, "Multiple values for a key not suported")
      # these keys will create a list of all values seen with this key
      # (keys not specified here raise a DuplicatedKeyError if keys are defined more than once)
      # Keys specified here automatically cannot be used for substitution
      self._allow_multiple.extend(key_list)

   # specifies the list of keys which should NOT be substituted into other values
   # => typically keys that are commands, for example, should not be used for substitution
   def dont_substitute(self, *key_list):
      if not self._dialect.allow_substitutions:
         raise DialectError(self._dialect, "Substitutions not supported")
      self._dont_substitute.extend(key_list)

   def parse(self, filename):
      # parse the file and build up a list of sections containing key:value pairs
      if not os.path.exists(filename):
         if self._parse_level:
            raise ParseError(self, "Included File %s not found" % filename)
         else:
            # top-level config file not found
            raise Error("File %s not found" % filename)
      self._filename = filename
      with open(filename) as config_file:
         process_multiline = False
         for line_num,line in enumerate(config_file):
            self._line_num = line_num+1
            if line.lstrip().startswith(self._dialect.comment_string):
               # ignore comment lines
               continue

            if line.isspace():
               # blank line
               if self._dialect.allow_multiline and process_multiline:
                  # blank line indicates end of multiline entry
                  process_multiline = False
                  # convert multiline entry back into a string
                  if self._dialect.preserve_multiline_whitespace == PRESERVE_NONE:
                     # if all whitespace stripped, need to join with a single space
                     value = " ".join(value)
                  else:
                     value = "".join(value)
                  self._process_key(key, value, True)
               # ignore the blank line
               continue

            if self._dialect.allow_multiline and process_multiline:
               # append line to multi-line entry (no key/value to process yet)
               value.append(self._strip_whitespace(line, True))
               continue

            # split into key/value pairs (split only at the first separator)
            try:
               (key,value) = line.split(self._dialect.separator, 1)
            except ValueError:
               raise ParseError(self, "Expecting: <Key> %s <Value>" % self._dialect.separator)

            if self._dialect.allow_multiline and value.strip().lower().endswith(self._dialect.multiline_marker):
               process_multiline = True
               value = []
               # dont process this value - its only purpose is to mark start of a multiline value
               # the key and the final multiline value will be processed when the end is detected
               continue

            self._process_key(key, value)

            # -- end: for line in config file --

         # finalize the last section processed
         self._end_section(end_file=True)

      # -- end: with config file --

   def _process_key(self, key, value, multiline=False):
      key = key.strip().lower()
      if not key:
         raise ParseError(self, "Missing Key")

      if not value.strip():
         if self._dialect.allow_empty:
            value = None
         else:
            raise ParseError(self, "Empty Value field for Key: " + key)
      else:
         value = self._strip_whitespace(value, multiline)

      if key == self._include_key:
         # parse the included file (value => filename)
         self._parse_level += 1
         self.parse(value)
         self._parse_level -= 1
      elif key == self._section_key:
         # end the current section
         self._end_section()
         # start a new section ...
         if value in [x[self._section_key] for x in self._sections]:
            raise ParseError(self, "Section: %s already defined" % value)
         self._curr_section = self._dict()
         self._curr_section[key] = value
         self._curr_section['__name__'] = value
      elif key == self._subsection_start:
         # -- Process Subsection start --
         if self._curr_subsection is not None:
            raise ParseError(self, "Nested sub-sections not supported")
         else:
            if self._curr_section is None:
               raise ParseError(self, "Sub-sections not supported in Globals")
         # a subsection is a list of key:value sets.  So a subsection is a list containing dictionaries
         self._curr_subsection = self._dict()
         self._curr_subsection_name = value
      elif key == self._subsection_end:
         # -- Process Subsection end --
         if self._curr_subsection is None:
            raise ParseError(self, "Found sub-section end without start")
         if value != self._curr_subsection_name:
            raise ParseError(self, "Sub-section start/end mismatch (current: %s)" % self._curr_subsection_name)
         if value not in self._curr_section:
            # make a new sub-section list in the current section
            self._curr_section[value] = []
         # append current sub-section to the subsection list
         self._curr_section[value].append(self._curr_subsection)
         self._curr_subsection = None
         self._curr_subsection_name = None
      else:
         # -- Process unreserved keywords --

         # look for import statement...
         if self._import_marker and value.lower().startswith(self._import_marker):
            param, rest = value.split(None,1)
            if param.lower() == self._import_marker:
               # if so, import the value for this key from the specified file
               if not os.path.exists(rest):
                  raise ParseError(self, "Import:: File %s not found" % rest)
               with open(rest) as import_file:
                  # verbatim read, no whitespace conversion
                  value = "".join(import_file.readlines())

         if self._curr_subsection is not None:
            # add the key/value to the current sub-section
            if key == self._iterate_key:
               raise ParseError(self, "Iteration not permitted within a subsection")
            where = self._curr_subsection
         elif self._curr_section is not None:
            # add the key/value to the current section
            where = self._curr_section
         else:
            # add the key/value to the global definitions
            # FIXME: if key is "allow_multiple" or "dont_substitute" append it to the appropriate list instead
            # (only do this if allow_multiple or allow_substitutions is set in the dialect)
            where = self._definitions

         # iteration can automatically have multiple values
         if key in self._allow_multiple or key == self._iterate_key:
            if key not in where:
               where[key] = []
            where[key].append(value)
         else:
            if key not in where:
               where[key] = value
            else:
               raise ParseError(self, "Key: %s already defined" % key)

   def _end_section(self, end_file=None):
      if end_file and self._parse_level != 0:
         # do nothing for end of included files
         # (sections will be completed when either another section is seen
         # in the enclosing file or the end of the top-level file is found)
         return
      if self._curr_section is None:
         # end the globals ...
         if end_file and self._section_key:
            # this is the end of the file, and sections were expected,
            # but no sections were seen
            raise ParseError(self, "No Sections!")
         else:
            # not end of file or no sections expected, so process globals as normal...
            # verify existence of mandatory definitions
            for mandatory in self._mandatory_global_keys:
               if mandatory not in self._definitions:
                  raise ParseError(self, "Missing Expected Key: " + mandatory)
      else:
         # end this section ...
         # verify existence of mandatory definitions for the current section
         for mandatory in self._mandatory_section_keys:
            if mandatory not in self._curr_section:
               raise ParseError(self, "Section %s: Missing Expected Key: %s" % (self._curr_section[self._section_key], mandatory))

         if self._dialect.allow_iteration and self._iterate_key in self._curr_section:
            # if iterate commands encountered, then perform the iteration(s) and produce the required sections...
            self._process_iterables()
            # discard the current section
         else:
            # add the current section to the section list
            self._sections.append(self._curr_section)

   def _process_iterables(self):
      param_lists = []
      #iterate_commands = self._curr_section[self._iterate_key]
      iterate_commands = self.substitute(self._curr_section[self._iterate_key], overrides=self._curr_section)
      groups = []
      for item in iterate_commands:
         # NOTE: if split can't be done, then
         try:
            (param, rest) = item.split(None, 1)
         except ValueError:
            # we have a bare parameter, which is presumed to be specified
            # elsewhere in the section (one or more values)
            # (iterates in the order the value(s) were defined)
            param = item.strip().lower()
            if param not in self._curr_section:
               raise ParseError(self, "Iteration parameter not found: " + param)
            if isinstance(self._curr_section[param], list):
               iter_list = self._curr_section[param]
            else:
               # a single-valued iteration!
               iter_list = [self._curr_section[param]]
         else:
            param = param.strip().lower()      # the parameter to iterate on (may indicate a sub-section)
            rest = rest.strip().lower()
            if self._dialect.allow_subsections and param == self._subsection_key:
               # subsection iteration (parameter sets)
               groups.append(rest.strip())
               iter_list = self._curr_section[rest.strip()]
            else:
               # iteration on specified parameter list/range
               try:
                  # NOTE: restrict globals/locals contexts to prevent accidental damage
                  iter_list = eval(rest,{},{})
               except SyntaxError:
                  raise ParseError(self, "Iteration Syntax Invalid: unable to evaluate")
               if not isinstance(iter_list, list):
                  raise ParseError(self, "Iteration specifier invalid: must produce a list")
         # NOTE: This is intentionally adding a list to a list
         param_lists.append(list(itertools.izip_longest([param], iter_list, fillvalue=param)))

      # create a product of all iterable parameters  to determine overall list of new sections ...
      all_sections = list(itertools.product(*param_lists))
      width_all_sections = len(str(len(all_sections)))
      for index,params in enumerate(all_sections):
         new_section = self._curr_section.copy()
         section_name = [new_section[self._section_key] + str(index+1).zfill(width_all_sections)]
         for param_name,value in params:
            if param_name == self._subsection_key:
               # iterating on a sub-section
               # write the contents of the current subsection into the section for substitution
               # NOTE: items in the subsection will overwrite any that may already exist in the section
               # FIXME: add friendly-name processing
               for group in groups:
                  if value in self._curr_section[group]:
                     index = self._curr_section[group].index(value)
                     width = len(str(len(self._curr_section[group])))
                     section_name.append(group + str(index+1).zfill(width))
               new_section.update(value)
            else:
               # convert any values back into strings if necessary
               if type(value) is not str:
                  value = str(value)
               # add the parameter(s) to the test
               new_section[param_name] = value
               # FIXME: Should we make the following a dialect setting?
               # add the parameter and value to the new section name
               section_name.append("_".join([param_name,value]))
         new_section[self._section_key] = "_".join(section_name)
         self._sections.append(new_section)

   def _find_section(self, section_name):
      for section in self._sections:
         if section[self._section_key] == section_name:
            return section
      raise SectionError(section_name)

   # perform iterative replacement using the supplied map
   def _do_replacement(self, value, map, sub_into_key):
      if not isinstance(value, str):
         raise TypeError(value,sub_into_key)
      iterations = _MAX_SUBSTITUTION_ITERATIONS
      while iterations:
         if '{' in value and '}' in value:
            try:
               value = value.format(**map)
            except KeyError as key:
               raise KeyError(str(key)[1:-1],sub_into_key)
         else:
            return value
         iterations -= 1
      raise SubstitutionDepthError(value=value,sub_into=sub_into_key)

   def _do_substitution(self, sub_into, map, depth, sub_into_key=None):
      # substitute for each element within nested structures
      if depth > self._max_nesting_depth:
         raise SubstitutionDepthError(sub_into=sub_into_key)
      if isinstance(sub_into, dict):
         # make a new object to return to the caller (dont want to modify the original)
         substituted = self._dict()
         for item in sub_into:
            substituted[item] = self._do_substitution(sub_into[item], map, depth+1, item)
         return substituted
      elif isinstance(sub_into, list):
         # this makes a new list
         return [self._do_substitution(x, map, depth+1, x) for x in sub_into]
      else:
         # This assumes sub_into is a string
         # if not, this will result in a TypeError
         # this returns a new string
         return self._do_replacement(sub_into, map, sub_into_key)

   def _strip_whitespace(self, string, multiline=False):
      if multiline and not self._dialect.allow_multiline:
         raise DialectError("Multiline values not supported")
      whitespace = (self._dialect.preserve_multiline_whitespace if multiline else self._dialect.preserve_whitespace)
      if whitespace == PRESERVE_NONE:
         return string.strip()
      elif whitespace == PRESERVE_LEADING:
         return string.rstrip()
      elif whitespace == PRESERVE_TRAILING:
         return string.lstrip()
      else:
         return string

   # substitution is performed from globals only if section_name is None
   # (i.e. globals substitute into globals)
   # if a section is specified, then substitute globals and section
   # definitions into globals, such that section definitions have precedence
   # if overrides are specified, these have precedence over any other def
   def substitute(self, sub_into, section_name=None, overrides={}):
      if not self._dialect.allow_substitutions:
         # return the source - acts as if "raw" had been set
         return sub_into
      # determine the map to use based on section name
      # the following ensures the map is a copy of the definitions to substitute
      # NOTE: always substitute globals
      # NOTE: The following update order ensures that when keys appear in
      # both globals and the section, the section has precedence
      map = self._definitions.copy()
      if section_name:
         map.update(self._find_section(section_name))
      if overrides:
         map.update(overrides)
      # remove the "dont substitute" keys and map items that are not strings
      for key in [x for x in map if x in self._dont_substitute or not isinstance(map[x], str)]:
         del map[key]

      try:
         return self._do_substitution(sub_into, map, 1)
      except KeyError as key:
         # missing substitution key
         missing, sub_into_key = key.args
         if section_name is not None:
            section = self._find_section(section_name)
         else:
            section = self._definitions
         try:
            value = section[missing]
         except KeyError:
            # this key does not exist in the section, so raise a normal "Missing" error
            pass
         else:
            # if the error was caused by invalid substitution type
            if not isinstance(value,str):
               raise SubstitutionInvalidError(section_name, missing, sub_into_key)
         raise SubstitutionMissingError(section_name, missing, sub_into_key)
      except TypeError as value:
         # attempt to substitute into something not a string
         raise SubstitutionTypeError(section_name, value.args[0], value.args[1])
      except SubstitutionDepthError as value:
         # iteration or nesting depth exceeded - fill in section name
         raise SubstitutionDepthError(section_name, value.value, value.sub_into)

   # fetch list of section names - if no sections, returns an empty list
   # Does not include the implied "globals" section
   def get_sections(self):
      return [x[self._section_key] for x in self._sections]

   # NOTE: 'raw' causes a reference to the original data to be returned, not a copy
   # modifications to the object returned will then affect all subsequent get() operations
   # this is by design (to allow update of information by an external process before
   # substitutions are performed)

   # return the global definitions - if none available, returns an empty dict
   def get_globals(self, raw=False):
      # NOTE: substitute globals into globals
      return (self._definitions if raw else self.substitute(self._definitions))

   def get_section(self, section_name, raw=False):
      section = self._find_section(section_name)
      return (section if raw else self.substitute(section, section_name))

   def get_value(self, section_name, key, raw=False):
      # use section_name = None to fetch a value from globals
      from_section = (self._definitions if not section_name else self._find_section(section_name))
      try:
         value = from_section[key.lower()]
      except KeyError:
         if self._dialect.missing_key_returns_none:
            return None
         else:
            # re-raise the KeyError
            raise
      if raw or value is None:
         return value
      return self.substitute(value, section_name)

   def get_bool(self, section_name, key):
      allowed_bool = { 'on':True, 'off':False, 'true':True, 'false':False, 'yes':True, 'no':False, '1':True, '0':False }
      value = self.get_value(section_name, key)
      if value is None:
         return value
      if value.lower() not in allowed_bool:
         raise ValueError("Section %s :: Value %s is not a boolean" % (section_name, value))
      return allowed_bool[value.lower()]

   def get_int(self, section_name, key):
      value = self.get_value(section_name, key)
      if value is None:
         return value
      return int(value)

   def get_hex(self, section_name, key):
      value = self.get_value(section_name, key)
      if value is None:
         return value
      return int(value, 16)

if __name__ == '__main__':
   # Config File testing...
   try:
      config = ConfigFile(default_defs = { 'view_path':'/view_path/', 'server_path':'/server_path/',
               'gen_path':'/gen_path/', 'test_out_path':'/test_path/'})
      config.set_section_key("test_name")
      config.set_iteration_key("iterate")
      config.set_subsection_key("group")
      config.set_include_key("include")
      config.set_import_marker("import")
      config.set_mandatory_section_keys('test_name', 'app_name', 'app_path', 'target_command')
      #config.set_mandatory_global_keys('hello')
      # The following automatically cannot be substituted
      config.allow_multiple('target_verify', 'target_requires', 'target_produces', 'generate')
      config.dont_substitute('build_opts', 'build_clean', 'target_command', 'verify_command')
      config.parse('mux_tests.cfg')
   except (Error,ParseError) as why:
      print why
      sys.exit(-1)

   print "Parse OK"

   print "Globals:\nRaw:",
   print config.get_globals(raw=True)
   print "Subst:",
   print config.get_globals()


   sections = config.get_sections()
   if False:"""
   for section in sections:
      print "Section %s (Raw) " % section,
      print config.get_section(section, raw=True)
      print "Section %s (subst) " % section,
      print config.get_section(section)
      print "App path (raw) ",
      print config.get_value(section, 'app_path', raw=True)
      print "App path ",
      print config.get_value(section, 'app_path')
   """
   print "Sections (%s):" % len(sections)

   for section in sections:
      print section
   #print config.get_section('hello')

   # test for sub into invalid (non-string) type
   my_data = { 'hello': 1 }
   #config.substitute(my_data)
   # test for nesting depth
   my_data = { 'hello' : [ 'there' , [ 'my_tag', 'value' ] ] }
   #config.substitute(my_data, 'TS_pcr_test')
   print config.get_value(None, 'suite_name')

   #print config.get_value('bogus', 'suite_name')
   #print config.get_value('TS_userdata_teletext_sample', 'bogus')
   sys.exit(0)


