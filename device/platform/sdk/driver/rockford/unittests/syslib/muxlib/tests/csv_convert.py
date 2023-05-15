#########################################################################
#     Copyright (c) 2003-2012, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: csv_convert.py $
# $brcm_Revision: Hydra_Software_Devel/4 $
# $brcm_Date: 6/11/12 9:55a $
#
# [File Description:]
# CSV converter utility
#
# read a CSV converting to Dictionary, and convert the values
# according to a supplied map, or in the absence of a map,
# according to general rules:
#
# if value startswith 0x: convert to int(x,16)
# all other values: convert to int
# if name contains 45khz: * 2
#
# Revision History:
#
# $brcm_Log: /rockford/unittests/syslib/muxlib/tests/csv_convert.py $
# 
# Hydra_Software_Devel/4   6/11/12 9:55a delkert
# SW7425-2567: keep the original _45kHz key/value as well as the
# converted pair.
#
# Hydra_Software_Devel/3   6/11/12 9:46a delkert
# SW7425-2567: Fix indentation error. Converted 45kHz values need to go
# to output dict.
#
# Hydra_Software_Devel/2   6/8/12 2:58p delkert
# SW7425-2567: Fix up exception handling. Strip keys to allow for
# whitespace in header
#
# Hydra_Software_Devel/1   5/4/12 2:29p delkert
# SW7425-2567: CSV File conversion tool
#
##########################################################################

from csv import DictReader, Error

class CSVConverter(DictReader):
   def __init__(self, file, map=None):
      self._map = map
      # NOTE: can't use super() here, since DictReader is not a new-style class
      DictReader.__init__(self, file)

   def __iter__(self):
        return self

   def next(self):
      x = DictReader.next(self)
      return self.convert(x)

   def convert(self, x):
      out = {}
      for key in x.keys():
         value = x[key].strip()
         key = key.strip()
         if self._map is not None and key in self._map:
            type_func = self._map[key]
            if not callable(type_func):
               raise Error("Invalid Map: '%s' Not callable" % type_func)
            if type_func is bool:
               value = bool(int(value))
            else:
               value = type_func(value)
         else:
            try:
               if value.startswith('0x'):
                  value = int(value, 16)
               else:
                  value = int(value)
            except ValueError, x:
               raise Error("Key '%s': %s" %(key, x))
            # if the key contains '45khz' calculate a 90khz version
            # and store a new key with this value
            index_45khz = key.lower().rfind('_45khz')
            if index_45khz != -1:
               out[key[:index_45khz]] = value * 2
         out[key] = value
      return out





