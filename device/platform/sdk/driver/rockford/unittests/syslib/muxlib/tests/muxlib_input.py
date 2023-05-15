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
# $brcm_Workfile: muxlib_input.py $
# $brcm_Revision: Hydra_Software_Devel/2 $
# $brcm_Date: 8/13/12 12:28p $
#
# [File Description:]
# Muxlib Input Descriptor Processing library
#
# Read and process Muxlib Input descriptor CSV files and metadata CSV files
# produced by BMUXLIB_Input module
#
# Revision History:
#
# $brcm_Log: /rockford/unittests/syslib/muxlib/tests/muxlib_input.py $
# 
# Hydra_Software_Devel/2   8/13/12 12:28p nilesh
# SW7425-3362: Merged FNRT support
# 
# Hydra_Software_Devel/SW7425-3362/2   8/7/12 4:11p delkert
# SW7425-3370: Add support for audio FNRT descriptors. Generalize
# Descriptor File reader.
#
# Hydra_Software_Devel/SW7425-3362/1   7/26/12 2:29p delkert
# SW7425-3370: Use getters for attributes so that attributes that do not
# exist do not cause init-time failure (only fails when the attribute is
# accessed)
# Add initial FNRT lib descriptor dump parsing.
#
# Hydra_Software_Devel/1   6/11/12 3:38p delkert
# SW7425-3214: Initial file reader for Video/Audio input descriptors &
# metadata
#
#########################################################################

import os
from csv_convert import CSVConverter, Error

_video_protocol_h264 = 0
_video_protocol_mpeg2 = 1
_video_protocol_h261 = 2
_video_protocol_h263 = 3
_video_protocol_vc1 = 4    # advanced profile
_video_protocol_mpeg1 = 5
_video_protocol_mpeg2_dtv = 6
_video_protocol_vc1_simple_main = 7
_video_protocol_mpeg4pt2 = 8
_video_protocol_avs = 9
_video_protocol_mpeg2_dss = 10
_video_protocol_svc = 11
_video_protocol_svc_bl = 12
_video_protocol_mvc = 13
_video_protocol_vp6 = 14
_video_protocol_vp7 = 15
_video_protocol_vp8 = 16
_video_protocol_rv9 = 17
_video_protocol_spark = 18
_video_protocol_mjpeg = 19

# NOTE: audio protocol is an incomplete list
_audio_protocol_mpeg_l1 = 0
_audio_protocol_mpeg_l2 = 1
_audio_protocol_mpeg_l3 = 2
_audio_protocol_aac_adts = 3
_audio_protocol_aac_loas = 4
_audio_protocol_aac_plus_loas = 5
_audio_protocol_aac_plus_adts = 6
_audio_protocol_ac3 = 7
_audio_protocol_ac3_plus = 8
_audio_protocol_ac3_lossless = 9
_audio_protocol_dts = 10
_audio_protocol_dts_hd = 11
_audio_protocol_dts_legacy = 12
_audio_protocol_wma_std = 13
_audio_protocol_wma_std_ts = 14
_audio_protocol_wma_pro = 15

# common muxlib input descriptor
class InputDescriptor(object):
   # process common stuff here
   def __init__(self, desc_dict):
      self.desc = desc_dict
      # the flags field is required
      flags = desc_dict['flags']
      self.flags = flags
      self.is_opts_valid = flags & 0x00000001
      self.is_pts_valid = flags & 0x00000002
      self.is_escr_valid = flags & 0x00000004
      self.is_tpb_valid = flags & 0x00000008
      self.is_shr_valid = flags & 0x00000010
      self.is_stc_valid = flags & 0x00000020
      self.is_frame_start = flags & 0x00010000
      self.is_eos = flags & 0x00020000
      self.is_empty_frame = flags & 0x00040000
      self.is_frame_end = flags & 0x00080000
      self.is_eoc = flags & 0x00100000
      self.is_metadata = flags & 0x40000000

   # NOTE: following only called when name is NOT found in the descriptor object
   def __getattr__(self, name):
      # check for fields that are controlled by validity flags
      # of have different names than the requested attribute
      # FIXME: Could this be automated using a lookup?
      if name == 'opts':
         self.opts = self.desc['opts'] if self.is_opts_valid else None
         return self.opts
      elif name == 'pts':
         self.pts = self.desc['pts'] if self.is_pts_valid else None
         return self.pts
      elif name == 'stc_snapshot':
         self.stc_snapshot = self.desc['stc_snapshot'] if self.is_stc_valid else None
         return self.stc_snapshot
      elif name == 'escr':
         self.escr = self.desc['escr'] if self.is_escr_valid else None
         return self.escr
      elif name == 'tbp':
         self.tbp = self.desc['tpb'] if self.is_tpb_valid else None
         return self.tbp
      elif name == 'shr':
         self.shr = self.desc['shr'] if self.is_shr_valid else None
         return self.shr
      else:
         try:
            value = self.desc[name]
         except KeyError:
            # specified field does not exist
            raise AttributeError(name)
         else:
            # cache the value so it can be directly accessed next time
            self.__dict__[name] = value
            return value

   def compare(self, other, exclude=[]):
      # find differences in fields...
      diffs = {k:(self.desc[k], other.desc[k]) for k,v in self.desc.iteritems() if k in other.desc and v != other.desc[k] and k not in exclude}
      diffs.update({k:(self.desc[k], None) for k in self.desc if k not in other.desc and k not in exclude})
      diffs.update({k:(None, other.desc[k]) for k in other.desc if k not in self.desc and k not in exclude})
      return diffs

   def __eq__(self, other):
      return not self.compare(other)

   def __ne__(self, other):
      return not self.__eq__(other)

class VideoDescriptor(InputDescriptor):
   # process video-specific stuff here
   def __init__(self, desc_dict):
      super(VideoDescriptor, self).__init__(desc_dict)
      vflags = desc_dict['vflags']
      self.vflags = vflags
      self.is_dts_valid = vflags & 0x00000001
      self.is_rap = vflags & 0x00010000
      self.is_data_unit_start = vflags & 0x00020000

   # NOTE: following only called when name is NOT found in the descriptor object
   def __getattr__(self, name):
      # check for fields that are controlled by validity flags
      if name == 'dts':
         self.dts = self.desc['dts'] if self.is_dts_valid else self.pts
         return self.dts
      elif name == 'data_unit_type':
         self.data_unit_type = self.desc['dut'] if self.is_data_unit_start else None
         return self.data_unit_type
      elif name == 'metadata_type':
         self.metadata_type = self.desc['dut'] if self.is_metadata else None
         return self.metadata_type
      else:
         return super(VideoDescriptor, self).__getattr__(name)

class AudioDescriptor(InputDescriptor):
   # process audio-specific stuff here
   def __init__(self, desc_dict):
      super(AudioDescriptor, self).__init__(desc_dict)

   def __getattr__(self, name):
      # check for fields that are controlled by validity flags
      if name == 'metadata_type':
         self.metadata_type = self.desc['dut'] if self.is_metadata else None
         return self.metadata_type
      elif name == 'raw_offset':
         self.raw_offset = self.desc['raw offset']
         return self.raw_offset
      elif name == 'raw_length':
         self.raw_length = self.desc['raw length']
         return self.raw_length
      else:
         return super(AudioDescriptor, self).__getattr__(name)

# FIXME: parts of this are common between Audio and Video FNRT descriptors
class VideoFnrtDescriptor(VideoDescriptor):
   def compare(self, other, exclude=[]):
      # compare either FNRT descriptor to another, or FNRT Desc to a muxlib input descriptor
      if other.__class__ is not VideoFnrtDescriptor:
         # ignore FNRT-specific fields and muxlib_input specific fields
         exclude.extend(['count', 'chunk', 'input', 'action', 'frame_size', 'base_addr', 'stc_snapshot'])
      return super(VideoFnrtDescriptor,self).compare(other, exclude)

   # NOTE: no specific init required for FNRT descriptors

   def __getattr__(self,name):
      if name == 'chunk_id':
         self.chunk_id = self.desc['chunk']
         return self.chunk_id
      elif name == 'input_index':
         self.input_index = self.desc['input']
         return self.input_index
      elif name == 'is_input_desc':
         self.is_input_desc = (self.action == "IN")
         return self.is_input_desc
      elif name == 'is_output_desc':
         self.is_output_desc = (self.action == "OUT")
         return self.is_output_desc
      elif name == 'is_desc_dropped':
         self.is_desc_dropped = (self.action == "DROPPED")
         return self.is_desc_dropped
      elif name == 'is_next_chunk':
         self.is_next_chunk = (self.action == "NEXT")
         return self.is_next_chunk
      else:
         # if not, look for Video Descriptor attribute
         return super(VideoFnrtDescriptor, self).__getattr__(name)

class AudioFnrtDescriptor(AudioDescriptor):
   def compare(self, other, exclude=[]):
      # compare either FNRT descriptor to another, or FNRT Desc to a muxlib input descriptor
      if other.__class__ is not AudioFnrtDescriptor:
         # ignore FNRT-specific fields and muxlib_input specific fields
         exclude.extend(['count', 'chunk', 'input', 'action', 'frame_size', 'base_addr', 'stc_snapshot'])
      return super(AudioFnrtDescriptor,self).compare(other, exclude)

   # NOTE: no specific init required for FNRT descriptors

   def __getattr__(self,name):
      if name == 'chunk_id':
         self.chunk_id = self.desc['chunk']
         return self.chunk_id
      elif name == 'input_index':
         self.input_index = self.desc['input']
         return self.input_index
      elif name == 'is_input_desc':
         self.is_input_desc = (self.action == "IN")
         return self.is_input_desc
      elif name == 'is_output_desc':
         self.is_output_desc = (self.action == "OUT")
         return self.is_output_desc
      elif name == 'is_desc_dropped':
         self.is_desc_dropped = (self.action == "DROPPED")
         return self.is_desc_dropped
      elif name == 'is_next_chunk':
         self.is_next_chunk = (self.action == "NEXT")
         return self.is_next_chunk
      else:
         # if not, look for Audio Descriptor attribute
         return super(AudioFnrtDescriptor, self).__getattr__(name)

class VideoMetadataDescriptor(object):
   def __init__(self, meta_desc):
      self.desc = meta_desc
      flags = meta_desc['flags']
      self.is_bitrate_valid = flags & 0x00000001
      self.is_framerate_valid = flags & 0x00000002
      self.is_coded_dimension_valid = flags & 0x00000004
      self.is_buffer_info_valid = flags & 0x00000008
      self.is_protocol_data_valid = flags & 0x00000010
      self.is_stc_snapshot_valid = flags & 0x00000020

      self.protocol = (meta_desc['protocol'] if self.is_buffer_info_valid else None)
      self.profile = (meta_desc['profile'] if self.is_buffer_info_valid else None)
      self.level = (meta_desc['level'] if self.is_buffer_info_valid else None)
      self.max_bitrate = (meta_desc['max_bitrate'] if self.is_bitrate_valid else None)
      self.frame_rate = (meta_desc['frame_rate'] if self.is_framerate_valid else None)
      self.coded_width = (meta_desc['coded_width'] if self.is_coded_dimension_valid else None)
      self.coded_height = (meta_desc['coded_height'] if self.is_coded_dimension_valid else None)
      self.stc_snapshot = (meta_desc['stc_snapshot'] if self.is_stc_snapshot_valid else None)

   def get_protocol_data(self, protocol):
      if protocol != _video_protocol_vc1_simple_main:
         return None
      if not self.is_protocol_data_valid:
         return None
      # extract and return a dict containing the protocol data
      # FIXME:
      return {}

class AudioMetadataDescriptor(object):
   def __init__(self, meta_desc):
      self.desc = meta_desc
      flags = meta_desc['flags']
      self.is_bitrate_valid = flags & 0x00000001
      self.is_sampling_frequency_valid = flags & 0x00000002
      self.is_protocol_data_valid = flags & 0x00000004
      self.is_stc_snapshot_valid = flags & 0x00000008

      self.protocol = meta_desc['protocol']
      self.max_bitrate = (meta_desc['max_bitrate'] if self.is_bitrate_valid else None)
      self.sampling_frequency = (meta_desc['samp_freq'] if self.is_sampling_frequency_valid else None)
      self.stc_snapshot = (meta_desc['stc_snapshot'] if self.is_stc_snapshot_valid else None)

   def get_protocol_data(self, protocol):
      if not self.is_protocol_data_valid:
         return None
      if protocol == _audio_protocol_aac_adts or protocol == _audio_protocol_aac_loas or \
         protocol == _audio_protocol_aac_plus_atds or protocol == _audio_protocol_aac_plus_loas:
         data = {}
         length_bits = self.desc['aac_audio_spec_length_bits']
         data['length_bits'] = length_bits
         data['length_bytes'] = length_bits / 8
         data['data'] = self.desc['aac_audio_spec_data']
         return data
      if protocol == _audio_procotol_wma_std:
         data = {}
         data['samples_per_block'] = self.desc['wma_samp_per_block']
         data['encode_options'] = self.desc['wma_enc_options']
         data['super_block_align'] = self.desc['wma_super_block_align']
         data['block_align'] = self.desc['wma_block_align']
         data['num_channels'] = self.desc['wma_num_channels']
         return data
      # unrecognized protocol
      return None

class CSVDescriptorFile(object):
   _field_map = None
   _filename_pattern = ""
   _allowed_types = {}

   def __init__(self, results_dir, input_type, type_instance=0):
      filename = os.path.join(results_dir, self._filename_pattern % (input_type.upper(), type_instance))
      desc_file = open(filename, "r")
      self._descriptor_reader = CSVConverter(desc_file, self._field_map)

      if input_type.lower() in self._allowed_types:
         self._desc_type = self._allowed_types[input_type.lower()]
      else:
         raise TypeError(input_type)

   def __iter__(self):
      return self

   def next(self):
      desc = self._descriptor_reader.next()
      if desc:
         return self._desc_type(desc)
      raise StopIteration

class DescriptorFile(CSVDescriptorFile):
   # csv file naming is done INPUT_DESC_VIDEO/AUDIO_<type_instance>_<mux_instance>
   # => type instance increments one per instance of input within the same type (audio or video)
   _filename_pattern = "BMUXlib_INPUT_DESC_%s_%2.2d_00.csv"
   _allowed_types = {'video':VideoDescriptor, 'audio':AudioDescriptor}

class MetadataFile(CSVDescriptorFile):
   _filename_pattern = "BMUXlib_METADATA_DESC_%s_%2.2d_00.csv"
   _allowed_types = {'video':VideoMetadataDescriptor, 'audio':AudioMetadataDescriptor}

class FnrtDescriptorFile(CSVDescriptorFile):
   _filename_pattern = "BMUXlib_FNRT_DESC_%s_%02d.csv"
   _allowed_types = {'video':VideoFnrtDescriptor, 'audio':AudioFnrtDescriptor}
   _field_map = {'action':str}


if __name__ == '__main__':
   import sys
   if sys.platform.startswith('win32'):
      X = FnrtDescriptorFile('results', 'video');
   else:
      X = FnrtDescriptorFile('/projects/stbdev_ext2/delkert/shared/FNRT', 'video');
   desc = X.next();
   desc2 = X.next();
   print desc.action
   print desc.compare(desc2)

