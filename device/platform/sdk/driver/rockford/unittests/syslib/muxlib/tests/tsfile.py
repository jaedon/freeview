#
# TS stream manipulation classes
# Provides TSReader, PESReader and TSPacket/PESPacket classes
# for reading and manipulating these packets from a TS file
#
# TSReader is a wrapper on a standard binary file
# PESReader is a TSPacket list processor (for now)

#
# Performance Issues/Optimization:
# If this code turns out to be very slow, there are some options:
# - change parsing to be on an "as needed" basis - headers are not
#   parsed until they are required (by a find, or a match, etc)
# - currently, the code slices the raw_data to obtain the payload
#   and the header data, but this slicing could be deferred such
#   that it is only done when the data is requested, such as
#   during a diff/compare, or when the payload is accessed
# - minimize data copies in general
#
# NOTE: it is important to note that use of caching of data read
# from the stream may not be viable due to the by-reference
# nature of the data storage.  If caches are used, then it may
# require copies to be stored to prevent corruption when the cache
# is refilled.  This would result in the same overhead we are
# trying to avoid
#

# TO DO:
# * modify to use BitString
#
import __builtin__
import sys
import struct
import itertools

TS_PACKET_LEN = 188
TS_PACKET_SYNC = 0x47

PES_STREAM_ID_PSM = 0xBC
PES_STREAM_ID_PADDING = 0xBE
PES_STREAM_ID_PRIVATE2 = 0xBF
PES_STREAM_ID_ECM = 0xF0
PES_STREAM_ID_EMM = 0xF1
PES_STREAM_ID_PSD = 0xFF
PES_STREAM_ID_DSMCC = 0xF2
PES_STREAM_ID_H222_TYPE_E = 0xF8

_MAX_SYNC_SEARCH = 64 * 1024

_struct_ts_header = '>BHB'
_struct_pcr = '>IH'
_struct_pes_timestamp = '>BHH'
_struct_pes_header = '>BBBBH'
_struct_pes_std_header = '>BBB'

# code uses dictionary comprehensions
if sys.version_info < (2, 7):
    print "Require python 2.7 or greater"
    sys.exit(-1)

class SyncError(Exception):
   # sync error is raised if TS-packet sync is lost
   pass

#
# Filter object for TSReader
#
# Specifies a dictionary of fields (and values) that are required to match
# during search operations or that should be ignored during comparison operations
#
# e.g: { 'pid':2501, 'pcr_present':True } => find packets from PID 2501 with PCR
#
# each field can contain a list of items to ignore or match:
# e.g. { 'pid': [ 2501, 2515 ] } => match packets from either pid
# FIXME: allow include/exclude in the same filter?
# exclusion filter only requires a list of fields to ignore
# inclusion filter requires the fields and their expected values
# => may not need this class if we simply use a standard dict
class TSFilter(dict):
   pass


#
# TSReader
# Wrapper class for reading a Transport Stream File
#
# getfp - obtain the underlying file pointer to allow manipulation of the baseline stream
#         BE CAREFUL when using this - changing the underlying stream status can affect
#         packet processing, particularly if done while performing an iteration.
#
# rewind - return to the first sync point in the stream
#
# extract_pids(pids) - return a dict of PIDs (pid and corresponding list of packets)
#                      (faster than doing find_all() when extracting multiple PIDs)
#
# set_filter(filt) - specify the filter to be used for iteration (e.g. for packet in ... )
#                    or for comparison functions.  When used for search, it is an inclusion
#                    filter; when used for comparisons, it is an exclusion filter
#
# find(filt) - find the first matching packet using the specified optional filter
#              if no filter specified, then the filter installed using set_filter is used
#              (if available - if no filter in use, returns the next packet)
#              calling find repeatedly will return next matching packet, and so on
#
# find_all(filt) - same as above but returns all the matching packets as a list
#
# find_last(filt) - same as find() but searches backward
#
# get_pids - return a dictionary of the pids found and their counts
#
# get_packets(n) - returns a list of up to n packets.  It may return less than n if at the
#                  end of the stream, or sync loss occurs and ignore-sync-loss not set
#                  omitting n (or n = 1) will return a single packet (not a list)
#
# length or len(<tsfile>) - returns the number of packets in the file
#
# seek(n) - seek to packet n.- use getfp().seek() to seek to actual file position
#
# tell - get the current packet number - use getpf().tell() to get the actual file position
#
# NOTE: this reader does NOT use caching of packets: all packets that reference the data
# would be corrupted when the cache is refreshed for new packets - using a cache would
# therefore require all data in packets returned to be copied to the packet object!
# This copying is prohibitive.
#
class TSReader(object):

   def __init__(self, stream, sync_pkts=3, ignore_nulls=True, ignore_sync_loss=False):
      if sync_pkts < 3:
         sync_pkts = 3
      self.pids = {}
      self.sync_pkts = sync_pkts
      self.ignore_nulls = ignore_nulls
      self.ignore_sync_loss = ignore_sync_loss
      if ignore_sync_loss:
         raise IgnoreSyncLossBroken
      self.filt = None
      self.stream = stream
      self.sync_points = []
      self.packet_count = 0
      # perform a sync-search to verify the file is a TS file
      # if no sync, raise TypeError and close the file
      try:
         self._reset()
      except SyncError:
         self.stream.close()
         raise TypeError

   def _reset(self):
      if self.sync_points:
         # sync has already been determined, so seek to the known sync point
         self.stream.seek(self.sync_points[0])
      else:
         self.stream.seek(0)     # restart the underlying stream
         # reset all values that must be initialized when returning to the start of the file
         self.sync_points = []
         self.sync_loss = []
         self._sync(0)
      self.packet_index = 0

   def __del__(self):
      self.close()

   def close(self):
      self.stream.close()

   def _sync(self, curr_loc):
      # resync the stream to the nearest TS header
      # curr_loc is the location from which sync was attempted
      sync_data = self.stream.read(_MAX_SYNC_SEARCH)
      buf_offset = 0
      while True:
         # find the first sync byte in the buffer
         try:
            sync_offset = sync_data.index(chr(TS_PACKET_SYNC), buf_offset)
         except ValueError:
            # no sync byte found in the buffer
            raise SyncError(curr_loc)
         # since we found one sync byte we need sync_pkts - 1 more
         sync_end_offset = sync_offset + ((self.sync_pkts-1) * TS_PACKET_LEN) + 1
         if sync_end_offset > _MAX_SYNC_SEARCH:
            # not enough data left in buffer to sync to
            raise SyncError(curr_loc)

         syncbytes = [x == chr(TS_PACKET_SYNC) for x in sync_data[sync_offset:sync_end_offset:TS_PACKET_LEN]]
         # FIXME: use all()?
         if reduce(lambda x,y: x and y, syncbytes):
            # we have found
            self.sync_points.append(sync_offset)
            self.stream.seek(sync_offset)
            break
         else:
            buf_offset = sync_offset + 1

   def _get_packet(self):
      # return a TS packet
      curr_loc = self.stream.tell()
      data = self.stream.read(TS_PACKET_LEN)
      if data:
         if len(data) != TS_PACKET_LEN:
            return None

         if data[0] != chr(TS_PACKET_SYNC):
            self.sync_loss.append(curr_loc)
            if self.ignore_sync_loss:
               # FIXME: For some reason this does not work
               raise IgnoreSyncLossBroken
               try:
                  self.stream.seek(curr_loc)
                  self._sync(curr_loc)
                  curr_loc = self.stream.tell()
               except SyncError:
                  return None
            else:
               raise SyncError(curr_loc)

         packet = TSPacket(data, curr_loc, self.packet_index)
         self.packet_index += 1
         return packet
      else:
         return None

   # tell, seek, len operate based on packet indexes/intervals in the TSReader
   # getfp allows the user to get the base stream handle to then
   # do these operations to use byte offsets, etc
   def getfp(self):
      return self.stream

   def rewind(self):
      self._reset()

   # seek to the specified packet index within the file
   # (0 = start)
   def seek(self, packet_index):
      # if all sync points known, then this is easy
      # simply count packets until each sync point and locate the packet within the sync interval
      # if not, we need to scan until we find the specified packet
      raise NotImplemented

   # provide the current packet index
   def tell(self):
      return self.packet_index

   # GetPids() => return a dictionary of PIDs giving their packet counts
   def get_pids(self, save_indexes=False):
      # return to the first located sync point
      self._reset()

      # scan thru the entire file and record the PIDs seen
      # also determines the sync points and the total packet count
      # FIXME: To minimize processing, this could just parse the headers only
      while True:
         # FIXME: make it an option to parse? - in this case we only need to parse headers (does this save us anything?)
         try:
            packet = self._get_packet()
         except SyncError, where:
            # lost sync - try to re-sync
            self.sync_losses.append(where)
            try:
               self._sync()
            except SyncError:
               break
         else:
            if not packet:
               break
            self.pids.__setitem__(packet.pid,1+self.pids.get(packet.pid,0))
      self.packet_count = self.packet_index

      # FIXME: if we stored the offset of the packet in a list in the PID table, we could access any packet
      # at any time => problem is: for large files requires a lot of storage
      return self.pids

   # iterate over the file and find all packets belonging to the specified PIDs
   # More efficient than doing repeated findall() with pid filter
   # pids is a series of arguments specifying the pid(s) to extract
   # returns dictionary of the form: { pid : list of packets }
   def extract_pids(self, *pids):
      result = {k:[] for k in pids}
      if not pids:
         return result
      while True:
         packet = self._get_packet()
         if not packet:
            self.packet_count = self.packet_index
            break
         if packet.pid in pids:
            # this packet is a desired packet
            result[packet.pid].append(packet)
      return result

   def get_packets(self, num_pkts=1):
      # FIXME: need a start, stop range
      if num_pkts == 1:
         return self._get_packet()
      return [self._get_packet() for x in range(num_pkts)]

   def set_filter(self, filt):
      self.filt = filt

   # find the first available packet that matches the criteria specified by "filt"
   # Any fields not specified in filt will be ignored in the match
   # searches from the current file location
   def find(self, filt=None):
      # FIXME: add a start and end packet number? Currently, will search from the current file offset
      # if starting offset given, will seek there first then search, else search from start
      # if no end, will search from start index to end of file
      # is it useful to specify a step?
      if filt is None:
         filt = self.filt
      while True:
         packet = self._get_packet()
         if not packet:
            return None
         if packet.match(filt):
            return packet

   def findall(self, filt=None):
      # FIXME: This needs a limit, since not specifying a filter will return ALL packets!!
      # FIXME: if we allowed both inclusion and exclusion filter, then this could use match() for an inclusion
      # filter and compare() for an exclusion filter (either/or, not both)
      if filt is None:
         filt = self.filt
      result = []
      # FIXME: This could call next() or use find()?
      while True:
         packet = self._get_packet()
         if not packet:
            self.packet_count = self.packet_index
            break
         if packet.match(filt):
            result.append(packet)
      return result

   def __iter__(self):
      # NOTE: This does not allow the iterator to be restarted - the iterator returned is the same each time
      # i.e. calling
      #    for x in file:
      #        do something
      #        if some condition:
      #           break
      #    for x in file:
      #        => continues where left off before!
      # use rewind() to start again
      return self

   def next(self):
      # iterator to return packets
      # this assumes that the current location in the stream is aligned to a packet boundary
      # (i.e. we have sync)
      # FIXME: if current location is not current iteration offset, seek to required offset (allows other APIs to interrupt an
      # iteration
      # FIXME: ignore nulls
      while True:
         packet = self._get_packet()
         if not packet:
            # no more packets
            self.packet_count = self.packet_index
            raise StopIteration
         if packet.match(self.filt):
            return packet


   # define comparison functions for TS files?
   # compare the whole file, or specify filter to compare only specific PIDs etc
   def __cmp__(self):
      # packet by packet compare using specified exclusion filter
      # compare without a filter, but wil ignore_nulls set can be used to compare files
      # if any packet does not match return false
      # if all packet match return true
      # there is no < or >, only eq or neq
      pass

   # length function to determine number TS packets
   def __len__(self):
      return self.length()

   def length(self):
      if not self.packet_count:
         self.get_pids()
      return self.packet_count


#
# Class to process a list of TS packets belonging to the same PID and produce PES packets
# NOTE: This does no verification of the input stream - it is up to the caller to ensure
# that each packet in the stream does indeed belong to the same PID
#
# Allows iteration such as:
#    for PES in PESReader:
#      do something with PES packet
#
class PESReader(object):

   def __init__(self, stream):
      # takes a TS packet list as input - the iterator pulls packets from the list as required
      # FIXME: adjust this so that it can also work using a file containing PES packets??
      if type(stream) is not list:
         raise TypeError("For now, PESReader only works with list of TS Packets")
      self.stream = stream
      self.stream_len = len(self.stream)
      self.ts_pkt_index = 0
      self.packet_index = 0
      # sync until we are aligned to a packet that has a payload start indication
      self._sync()
      if not self.in_sync:
         raise TypeError("Unable to locate first PES header")

   def _sync(self):
      # skip over TS packets until we find PUSI (first PES header expected)
      while self.ts_pkt_index != self.stream_len and not self.stream[self.ts_pkt_index].payload_start:
         self.ts_pkt_index += 1
      self.in_sync = self.ts_pkt_index != self.stream_len

   def __iter__(self):
      return self

   def next(self):
      # scan thru the TS payloads, parsing each until we have a full PES packet
      # FIXME: make this a get_packet() and then make this like the TS code
      packet = None
      if not self.in_sync:
         self._sync()
      while True:
         if self.ts_pkt_index == self.stream_len:
            # run out of TS packets
            raise StopIteration
         tspacket = self.stream[self.ts_pkt_index]
         if not tspacket.payload:
            # skip this packet
            self.ts_pkt_index += 1
            continue
         if tspacket.payload_start:
            packet = PESPacket(tspacket.payload, self.packet_index, self.ts_pkt_index)
            self.packet_index += 1
         else:
            if not packet:
               # not payload start and no current PES being constructed - this is an error
               # indicates TS packets are missing from the input stream
               # if this occurs - the next iteration should perform a re-align-to-pusi operation
               self.in_sync = False
               raise SyncError("Packet is missing PUSI (packets may be missing)")

            # add the current TS packet's payload to the PES packet
            packet += tspacket.payload
         self.ts_pkt_index += 1
         # if no packet length, then header has not been parsed yet, so we need to wait until
         # next packet
         if packet.length is not None:
            if packet.length == 0:
               # look at next packet to see if this packet is complete
               if self.ts_pkt_index == self.stream_len:
                  # no more packets, so complete this packet
                  return packet
               else:
                  if self.stream[self.ts_pkt_index].payload_start:
                     return packet
            else:
               if packet.is_complete():
                  return packet
         # we get here if current packet is not complete

     # FIXME: we need filtering capabilities on the iterator (i.e. obtain only packets that match certain criteria)
     # find()
     # findall()
     # set_filter()
     # length => count of PES packet
     # get_packets() ??

class PacketDiffs(dict):
   # pretty-print a summary of the diffs found
   # NOTE: repr() will still print out the raw dictionary contents, if required
   def __str__(self):
      if not self.keys():
         # empty dict
         return "No Differences"
      max_key_len = max([len(x) for x in self.keys()])
      string = "Original Packet[%s] / Other Packet[%s]\n" % (self['pkt_index'][0], self['pkt_index'][1])
      string += "%-*s %20s %20s\n" % (max_key_len, "Key", "Value (Original)", "Value (Other)")
      string += ('='*(max_key_len+43) + "\n")
      for key in self.keys():
         if key == 'pkt_index':
            continue
         val1 = self[key][0]
         val2 = self[key][1]
         # NOTE: currently, all packet fields have values that are either numerical or boolean
         if type(val1) is str: # and not val1.isalpha():
            val1 = "<binary[%d]>" % len(val1)
         if type(val2) is str: # and not val2.isalpha():
            val2 = "<binary[%d]>" % len(val2)
         string += "%-*s %20s %20s\n" % (max_key_len, key, val1, val2)
      return string

# common base class for TS and PES packets
# This is an abstract class - not to be used directly
class Packet(object):
   # list of field names allowed in this packet (this is the list of fields compared
   # using compare, or filtered on during match)
   _allowed_fields = []

   # subclasses must override this
   def __init__(self):
      if self.__class__ is Packet:
         raise TypeError("Packet is an abstract class. Use TSPacket or PESPacket")
      raise NotImplemented

   #
   # common methods used by all packet types
   #

   # list the names of fields in this packet
   def list_fields(self):
      return self.__class__._allowed_fields

   # the following allows access to fields within the parsed packet data as if
   # they were attributes of the packet (e.g. tspacket.pid)
   def __getattr__(self, name):
      if name in self.__class__._allowed_fields:
         # accessing a field in the packet
         try:
            return self.fields[name]
         except KeyError:
            # this packet does not have this field
            return None
      else:
         # attempting to access some other attribute that does not exist
         raise AttributeError(name)

   # determine if this packet matches a set of criteria defined by 'filter'
   # filter must define the properties that need to be matched - all other
   # properties not defined by filter are ignored
   # All fields that specify multiple values are indicating an 'OR' value match:
   # e.g. filt = { 'pid':[19, 20] } => find packets with PID 19 OR PID 20
   # NOTE: we never need 'AND' value capability - a single field cannot have two values.
   # multiple fields specified indicate an 'AND' operation - ALL fields in the specified
   # packet must match for this to match the packet
   # e.g. filt = { 'pid': 19, 'adaptation_present':True }
   # => only match packets with PID 19 that also have an adaptation field
   # to do 'OR' field matching, search with the different set of fields and merge the results
   def match(self, filt=None):
      # determine if this packet matches the specified filter criteria
      # this is an inclusion filter - only the fields indicated in the filter
      # are matched unless filter is None
      # All fields must match to return a True result
      # NOTE: Exclusion filter does not apply to a match, only to a compare
      if filt is None:
         return True
      else:
         for field, value in filt.items():
            if field not in self.fields:
               # this packet does not have the field to be matched, so no match
               return False
            if type(value) is list:
               if self.fields[field] not in value:
                  return False
            else:
               if value != self.fields[field]:
                  return False
      return True

   # compare this packet to another packet, excluding specified fields
   # returns a dictionary containing the field that is different and a tuple of the values from both packets
   # if a field does not exist in either packet, its corresponding value will be None
   def compare(self, other, exclude=[]):
      if not isinstance(other, self.__class__):
         return TypeError

      # find differences in fields...
      diffs = {k:(self.fields[k], other.fields[k]) for k,v in self.fields.iteritems() if k in other.fields and v != other.fields[k] and k not in exclude}
      diffs.update({k:(self.fields[k], None) for k in self.fields if k not in other.fields and k not in exclude})
      diffs.update({k:(None, other.fields[k]) for k in other.fields if k not in self.fields and k not in exclude})

      if diffs:
         # add the packet index values for the difference location
         diffs['pkt_index'] = (self.packet_index, other.packet_index)

      # FIXME: When differencing data blocks would we want all the data in the diff, or just the fact that they differ (perhaps give length of data)
      # Maybe we need an option to specify whether to diff payload and/or header data
      return PacketDiffs(diffs)

   def __cmp__(self, other):
      if not isinstance(other, self.__class__):
         return TypeError
      # compare packets using specified filter: for use in find()
      # if no filter installed, this will do a standard compare of the entire packet
      # for use with eq or neq comparsions
      pass

   # subclasses must override this
   def parse(self):
      raise NotImplemented


class TSPacket(Packet):
   # class to handle processing of TS packets - parses the TS packet
   # initially, the object indicates only the packet number/offset of the packet
   _allowed_fields = ['payload', 'adaptation_data', 'transport_error', 'payload_start',
                      'transport_priority', 'pid', 'scr_control', 'payload_present',
                      'adaptation_present', 'cont_count','adaptation_length', 'discontinuity',
                      'random_access', 'es_priority', 'pcr_flag', 'opcr_flag', 'splice_point',
                      'transport_private_data', 'adaptation_extension', 'pcr']

   def __init__(self, data, offset=None, packet_index=None):
      self.raw_data = data
      assert len(data) == TS_PACKET_LEN, "Data supplied has invalid length (%s) - need one TS packet" % len(data)
      self.packet_index = packet_index
      self.offset = offset
      self.bytes_processed = 0
      self.payload_length = 0
      self.adapt_data_length = 0
      self.fields = {}
      self.parse()

   def parse(self):
      # FIXME: option to parse - if no parse, just parse the header?
      self.parse_header()
      if self.fields['adaptation_present']:
         self.parse_adaptation_field()
      if self.fields['payload_present']:
         self.payload_offset = self.bytes_processed + self.adapt_data_length
         self.payload_length = TS_PACKET_LEN - self.payload_offset
         # NOTE: The following does the slice copy at parsing time.  if performance is an issue, we
         # could defer this so that the copy is done when the data is accessed
         if self.payload_length:
            self.fields['payload'] = self.raw_data[self.payload_offset:self.payload_offset+self.payload_length]

   def parse_header(self) :
      (sync, pid, flags) = struct.unpack_from(_struct_ts_header, self.raw_data)
      if sync != TS_PACKET_SYNC:
         raise TypeError("TS Sync byte (0x%x) not found (got: 0x%x)" % (TS_PACKET_SYNC, sync))
      self.fields['transport_error'] = 0 != (pid & 0x8000)
      self.fields['payload_start'] = 0 != (pid & 0x4000)
      self.fields['transport_priority'] = 0 != (pid & 0x2000)
      self.fields['pid'] = pid & 0x1FFF
      self.fields['scr_control'] = (flags & 0xc0) >> 6
      self.fields['payload_present'] = 0 != (flags & 0x10)
      self.fields['adaptation_present'] = 0 != (flags & 0x20)
      self.fields['cont_count'] = flags & 0xF
      self.bytes_processed += struct.calcsize(_struct_ts_header)

   # TestBit(field, bit_pos) and ExtractBits(field, mask)

   # NOTE: This only processes the adaptation field up to and including the PCR (if it exists)
   # any remaining adaptation field data is unparsed - it can be obtained from self.adaptation_data
   def parse_adaptation_field(self):
      self.fields['adaptation_length'] = ord(self.raw_data[self.bytes_processed])
      self.bytes_processed += 1
      if self.fields['adaptation_length']:
         start_bytes = self.bytes_processed
         flags = ord(self.raw_data[self.bytes_processed])
         self.fields['discontinuity'] = 0 != (flags & 0x80)
         self.fields['random_access'] = 0 != (flags & 0x40)
         self.fields['es_priority'] = 0 != (flags & 0x20)
         self.fields['pcr_flag'] = 0 != (flags & 0x10)
         self.fields['opcr_flag'] = 0 != (flags & 0x8)
         self.fields['splice_point'] = 0 != (flags & 0x4)
         self.fields['transport_private_data'] = 0 != (flags & 2)
         self.fields['adaptation_extension'] = 0 != (flags & 1)
         self.bytes_processed += 1
         if self.fields['pcr_flag']:
            self.fields['pcr'] = self.parse_clock_reference()
         self.adapt_data_length = self.fields['adaptation_length'] - (self.bytes_processed - start_bytes)
         self.adapt_data_offset = self.bytes_processed
         # NOTE: The following does the slice copy at parsing time.  if performance is an issue, we
         # could defer this so that the copy is done when the data is accessed
         if self.adapt_data_length:
            self.fields['adaptation_data'] = self.raw_data[self.adapt_data_offset:self.adapt_data_offset+self.adapt_data_length]

   def parse_clock_reference(self):
      (base, ext) = struct.unpack_from(_struct_pcr, self.raw_data, self.bytes_processed)
      self.bytes_processed += struct.calcsize(_struct_pcr)
      return (((base << 1) | ((ext & 0x8000) >> 15)) * 300) + (ext & 0x1FF)

   def is_null(self):
      return self.fields['pid'] == 0x1FFF

   def __str__(self):
      #FIXME: Clean this up to print formatted packet
      return "PID: %x" % self.fields['pid']


class PESPacket(Packet):
   _private_packet_types = [PES_STREAM_ID_PSM, PES_STREAM_ID_PADDING, PES_STREAM_ID_PRIVATE2,
                           PES_STREAM_ID_ECM, PES_STREAM_ID_EMM, PES_STREAM_ID_PSD,
                           PES_STREAM_ID_DSMCC, PES_STREAM_ID_H222_TYPE_E]

   _allowed_fields = ['payload', 'header_data', 'stream_id', 'length', 'scr_control', 'priority',
                     'data_alignment', 'copyright', 'orig_copy', 'pts_present', 'dts_present',
                     'escr_present', 'es_rate_present', 'dsm_trick_mode', 'extra_copy_info',
                     'crc_present', 'extension_flag', 'header_data_length', 'pts', 'dts']

   _min_header_bytes = struct.calcsize(_struct_pes_header)
   _min_header_data_bytes = struct.calcsize(_struct_pes_std_header)

   _parsers = [None, 'self.process_payload()', 'self.parse_extra_header_data()', 'self.parse_to_header_data_length()', 'self.parse_id_length()']

   def __init__(self, data, packet_index, start_idx):
      # here, data is the TS packet payload data for the packet that has PUSI
      # (i.e. it is the PES header)
      self.bytes_processed = 0
      self.total_bytes = 0
      self.fields = {}
      self.raw_data = data
      self.packet_index = packet_index    # index of the packet within the source it was extracted from
      self.start_idx = start_idx   # index of the TS packet this PES starts in
      self.extra_header_length = 0
      # parse headers in steps: up to length, up to header_data_length, the remaining header data, and then initial payload
      self.headers_remaining = 4
      self.parse()

   def is_complete(self):
      # if length has not been processed yet, or length field is 0, then not complete
      if not self.fields['length']:
         return False
      return (True if self.bytes_processed == self.total_bytes else False)

   def is_private(self):
      return self.fields['stream_id'] == PES_STREAM_ID_PRIVATE2

   # parse PES packet headers -
   # NOTE: for now this only parses certain fields and assumes they are within the same TS packet
   # i.e. parse flags, header_data_length, PTS and DTS (if present)
   def parse(self):
      while self.headers_remaining:
         if not eval(PESPacket._parsers[self.headers_remaining]):
            # can't process this header yet, so wait for more data
            break

   # parse the start code prefix, stream ID and length
   def parse_id_length(self):
      header_size = PESPacket._min_header_bytes
      if len(self.raw_data) < header_size:
         # do nothing - not enough data to process the header yet
         return False

      (sc1, sc2, sc3, stream_id, length) = struct.unpack_from(_struct_pes_header, self.raw_data)
      if sc1 != 0 or sc2 != 0 or sc3 != 1:
         # not a valid PES packet
         raise TypeError("Bad PES Start Code detected")
      self.fields['stream_id'] = stream_id
      # length is the length of the PES after the length field (includes some additional header information).
      # So the total PES packet length is length + 6
      self.fields['length'] = length
      self.total_bytes = length + header_size
      self.bytes_processed += header_size
      self.headers_remaining -= 1
      return True

   # if this is a standard (non-private) packet, parse flags and header_data_length
   def parse_to_header_data_length(self):
      if self.fields['stream_id'] in PESPacket._private_packet_types:
         self.headers_remaining = 0   # skip standard header for private packet types
         return True

      std_header_size = PESPacket._min_header_data_bytes
      if len(self.raw_data) < std_header_size:
         # not enough bytes available to process the header yet
         return False

      (flags1, flags2, hdr_len) = struct.unpack_from(_struct_pes_std_header, self.raw_data, self.bytes_processed)
      # ignore the 2 bits that should be '10' - we don't care what they are
      self.fields['scr_control'] = ((flags1 & 0x30) >> 4)
      self.fields['priority'] = 0 != (flags1 & 0x8)
      self.fields['data_alignment'] = 0 != (flags1 & 0x4)
      self.fields['copyright'] = 0 != (flags1 & 0x2)
      self.fields['orig_copy'] = 0 != (flags1 & 0x1)
      self.fields['pts_present'] = 0 != (flags2 & 0x80)
      self.fields['dts_present'] = 0 != (flags2 & 0x40)
      self.fields['escr_present'] = 0 != (flags2 & 0x20)
      self.fields['es_rate_present'] = 0 != (flags2 & 0x10)
      self.fields['dsm_trick_mode'] = 0 != (flags2 & 0x8)
      self.fields['extra_copy_info'] = 0 != (flags2 & 0x4)
      self.fields['crc_present'] = 0 != (flags2 & 0x2)
      self.fields['extension_flag'] = 0 != (flags2 & 0x1)
      # NOTE: the following indicates the number of header bytes following this field
      # including any stuffing bytes
      self.fields['header_data_length'] = hdr_len
      self.bytes_processed += std_header_size
      self.headers_remaining -= 1
      return True

   # parse remaining header data after header_data_length
   def parse_extra_header_data(self):
      if len(self.raw_data) < PESPacket._min_header_data_bytes + self.fields['header_data_length']:
         # not enough data to process rest of header
         return False

      start_bytes = self.bytes_processed

      # if pts present, then parse PTS and/or DTS
      if self.fields['pts_present']:
         if self.fields['pts_present']:
            self.fields['pts'] = self.parse_timestamp()
         if self.fields['dts_present']:
            self.fields['dts'] = self.parse_timestamp()

      # if pts not present, but DTS is, then we cannot parse it (invalid) so simply store the header as extra data
      # if neither present, then move on to store header

      self.extra_header_offset = self.bytes_processed
      self.extra_header_length = self.fields['header_data_length'] - (self.bytes_processed - start_bytes)
      # NOTE: The following does the slice copy at parsing time.  if performance is an issue, we
      # could defer this so that the copy is done when the data is accessed
      if self.extra_header_length:
         self.fields['extra_header_data'] = self.raw_data[self.extra_header_offset:self.extra_header_offset+self.extra_header_length]
      self.headers_remaining -= 1
      return True

   def parse_timestamp(self):
      timestamp_size = struct.calcsize(_struct_pes_timestamp)
      (high, mid, low) = struct.unpack_from(_struct_pes_timestamp, self.raw_data, self.bytes_processed)
      # ignore the 4bits preceding the timestamp - we don't care what they are
      timestamp = (high & 0xe) << 29
      timestamp |= ((mid & 0xFFFE) << 14)
      timestamp |= ((low & 0xFFFE) >> 1)
      self.bytes_processed += timestamp_size
      return timestamp

   def process_payload(self):
      # additional payload may be appended from later TS packets
      self.payload_offset = self.bytes_processed + self.extra_header_length
      self.payload_length = len(self.raw_data) - self.payload_offset
      # NOTE: bytes processed is used to determine when this packet is done
      # so we need to account for all the data in this packet
      self.bytes_processed = len(self.raw_data)
      # NOTE: The following does the slice copy at parsing time.  if performance is an issue, we
      # could defer this so that the copy is done when the data is accessed
      self.fields['payload'] = (self.raw_data[self.payload_offset:self.payload_offset+self.payload_length] if self.payload_length else [])
      self.headers_remaining -= 1
      return True

   def __add__(self, data):
      # add the specified data to the payload of this PES
      self.append(data)
      return self

   def append(self, data):
      # add the specified data to this PES packet.
      # if headers not parsed, then parse them
      # else, this data belongs to the payload
      self.raw_data += data
      if self.headers_remaining:
         self.parse()
      else:
         self.bytes_processed += len(data)
         self.payload_length += len(data)
         self.fields['payload'] += data

   def __str__(self):
      return "PES[%2.2x]: %d bytes" % (self.stream_id, self.length)

#
# Opens a transport stream file and if successfully identified as a
# TS file, returns a TSReader object
#
# infile = tsfile.open(filename, "r")
#
# currently only supports reading TS files - writing not supported
#
# Raises a TypeError if the specified file is not a TS file
#
def open(fname, mode='rb', sync_pkts=3, ignore_nulls=True, ignore_sync_loss=False):
      if mode in ('r','rb'):
         stream = __builtin__.open(fname, 'rb')
         return TSReader(stream, sync_pkts, ignore_nulls, ignore_sync_loss)
      else:
         raise NotImplementedError

def pairwise(iterable):
    "s -> (s0,s1), (s1,s2), (s2, s3), ..."
    a, b = itertools.tee(iterable)
    next(b, None)
    return itertools.izip(a, b)

# sync the packets in 'source' with the packets in 'target' by skipping
# packets in 'source' until they align
# returns the index of the first matching packet in the 'source'
# or None if no sync
# both must be lists of packets (either TS packets or PES packets)
# NOTE: if either list is shorter than the required number of syncs,
# this will still match as long as ALL available packets match!
#
# FIXME: If we make the readers slice capable and use itertools.izip, then we could
# process the streams directly
def sync_to(source, target, filt=[], required_sync=10):
   src_index = 0
   end_index = len(source)

   # since the data we are comparing may be substantially the same after we have
   # excluded certain fields, the sync must account for this (e.g. we typically
   # exclude changes in timing, yet that may be the only thing that changes between
   # packets).Similarly, continuity count is modulo-16, so false sync can easily
   # occur if data is not varying by some other means.
   # therefore, determine how many leading target packets are the same after excluding
   # the fields specified by the filter => this is the minimum sync length required
   prefix_filt = filt
   if target[0].__class__ is TSPacket:
      # when comparing TS for determining lead-in, we must ignore continuity count
      prefix_filt.append('cont_count')
   pairs = pairwise(target)
   initial_count = 0
   for x,y in pairs:
      if x.compare(y,prefix_filt):
         break             # this packet not same as previous, so we're done
      initial_count += 1   # else, count how many initial packets are the same
   required_sync += initial_count

   # this is essentially trying to match a sliding window of "required_sync" packet
   # in the source with the first "required_sync" packets in the target
   target_pkts = target[0:required_sync]
   while True:
      window_both = zip(source[src_index:src_index+required_sync], target_pkts)
      diffs = [diff for diff in [src_pkt.compare(target_pkt, filt) for (src_pkt,target_pkt) in window_both] if diff]
      if not diffs:
         # no diffs, so we must be synced!
         return src_index
      src_index += 1
      if src_index == end_index:
         # Not found!
         return None

#
# Test...
#
if __name__ == '__main__':
   print "Running ..."
   # add support for contexts, then code this as a with ... as ??
   try:
      infile = open('C:\\test.ts')
   except TypeError:
      print "File is not a TS file! "
   else:
      print "looking at packets"
      #filt = { 'pid': 17, 'pcr_flag': True }
      #infile.set_filter(filt)
      #pid_17_with_pcr = infile.findall(filt)

      #for packet in pid_17_with_pcr:
      #   print packet.pcr

      #infile.rewind()

      filt = { 'pid': 19 }
      pid_19 = infile.findall(filt)

      print "comparing..."
      diffs = pid_19[0].compare(pid_19[1], ['payload'])
      print diffs

      #pes_stream = PESReader(pid_19)

      #for packet in pid_19:
      #   print packet.fields
      #packet_count = 0
      #for packet in pes_stream:
      #   if not packet_count:
      #      print packet.list_fields()
      #   packet_count += 1
      #   print packet.pts
      #   #if packet_count > 5:
      #   #   break

      #while True:
      #   packet = infile.find(filt)
      #   if not packet:
      #      break
      #   print packet.pcr

      #for packet in infile:
      #   print packet.pcr
      #   if packet.packet_index > 100000:
      #      break
      #for packet in infile:
      #   print packet.pcr

      #packets = infile.findall()
      #print len(packets)
      print "packet count = " + str(len(infile))
      print "sync points = " + str(infile.sync_points)
      print "sync loss = " + str(infile.sync_loss)
      #pids = infile.get_pids()
      #print "pids = " + str(pids)
   print "done"

#
#Requirements:
#Userdata verification:
#   open two ts files, specify pids to extract, and compare the data in those packets, ignoring specified fields within the payload data
#   => requires filtering on pids - either for x in , with an installed filter, and read entire packet OR read all packets header only, and extract those that are
#          needed for a match
#      requires compare function with filters
#      align streams by collecting all relevant packets for both pids and then doing a sync with the filtered compare
#
#System Data Verification:
#   open a single TS file, and verify that PCR packets are correct spaced (PCR values are correct, time intervals are correct)
#      => requires reading all the PCR values for a specified program
#
#   open a single TS file, and verify that the expected number of packets of a particular PID occurs between the PCRs, and that those packets are
#      correct spaced.
#      => need to know where the PCRs are for the specified program, and then count the packets of the specified PID between those packets
#      => this requires doing a find() of PCRs - gives a list of packet numbers/offsets of the PCRS
#         and doing a find() of specified PID - gives a list of packet numbers/offsets of that PID
#
#tsfile.find() => returns a list of packet number/offset pairs (note that the offset does not necessarily match the packet count due to possible sync losses)
#tspacket needs a stream to access if required
#perhaps tsfile.get_packet() needs an offset argument to allow reading of individual packets.  Would doing so break a for x in ... construct? Could we return
#to a previous offset to continue an iteration => save the current offset immediately after reading a packet during an iteration.  If a get is performed during
#an iteration loop, it can flag the interruption to cause the file offset to be reset for the interations => best to find the relevant packet locations, then load
#the packets for processing.
#
#tsfile.set_filter(filter) => filter to be used by for x in ... iterations
#tsfile.find() => uses the installed filter, or can specify a filter via parameter.  If no filter, returns the index of all packets.  Does not parse any of the
#packets except as required to do the filtering (i.e. no filter will not parse any packets, pid filter will process the headers, etc)

