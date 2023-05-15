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
# $brcm_Workfile: fnrt_verify.py $
# $brcm_Revision: Hydra_Software_Devel/1 $
# $brcm_Date: 8/13/12 12:28p $
#
# [File Description:]
# Fast Non-RealTime Transcode verification
#
# Revision History:
#
# $brcm_Log: /rockford/unittests/syslib/muxlib/tests/fnrt_verify.py $
# 
# Hydra_Software_Devel/1   8/13/12 12:28p nilesh
# SW7425-3362: Merged FNRT support
# 
# Hydra_Software_Devel/SW7425-3362/3   8/7/12 4:46p delkert
# SW7425-3370: Add support for master group indication in config file
# 
# Hydra_Software_Devel/SW7425-3362/2   8/7/12 4:33p delkert
# SW7425-3370: Clean up input group handling. Fix descriptor verification
# to support audio.
# 
# Hydra_Software_Devel/SW7425-3362/1   8/7/12 4:22p delkert
# SW7425-3370: Initial FNRT testing
#
##########################################################################
#
# Verify the FNRT library functionality
#
# Ensure that given N different encoder descriptor sources that have
# been segmented from a single media source, that the FNRT lib
# properly retimes and resynchronizes the chunks into a single
# contiguous stream for input to the mux
#
# Verify:
# - PTS, DTS, OPTS, ESCR etc are contiguous and have worst-case dPTS variation
# of 1 frame at chunk boundaries (should be < that everywhere)
# - overall duration is correct given the sum of all the input chunks
# - lip-sync is correct between audio/video (separation between APTS and VPTS
# is within some allowed range)
# - output descriptors contain the correct metadata and EOS
# - chunk ordering is correct (i.e. chunks have been re-ordered correctly)

# TODO:
# - discontinuity handling - need to break into segments based on OPTS of input descriptors
# - Deal with dynamic input addition/removal
#
import argparse
import os
import sys
import itertools

import muxlib_input
from csv_convert import CSVConverter
from csv_convert import Error as CSVError

class TestFail(Exception):
   pass

class ConfigError(Exception):
   def __init__(self, filename, line_num, msg):
      self.msg = "%s:: Line %s: %s" % (filename, line_num, msg)

   def __str__(self):
      return self.msg


def get_cmd_line_args():
   parser = argparse.ArgumentParser(description='Fast Non-Realtime Transcode Verification')

   parser.add_argument('-results', metavar='result_dir', type=str, help='The directory where the generated output from the test can be found'
      'If not specified, current directory is assumed')

   parser.add_argument('-stop', action='store_true', default=False, help='Stop after the first error. Otherwise continue to the end of the test')

   return parser.parse_args()


# if a test fails, and -stop specified, then bail out immediately
# else print a message and continue with the rest of the tests
def test_fail(reason):
   if args.stop:
      raise TestFail(reason)
   else:
      print reason
      return False


def pairwise(iterable):
    "s -> (s0,s1), (s1,s2), (s2, s3), ..."
    a, b = itertools.tee(iterable)
    next(b, None)
    return itertools.izip(a, b)


def process_config(results_dir):
   # read the FNRT configuration file that defines active groups, base addresses,
   # and input addition/removal
   config_filename = 'BMUXlib_FNRT_CONFIG.csv'
   filename = os.path.join(results_dir, config_filename)
   groups = {}
   master_already_set = False
   print "Processing FNRT configuration ..."
   with open(filename, "r") as  config_file:
      try:
         config_reader = CSVConverter(config_file, {'action':str,'input_type':str})
         for entry in config_reader:
            # unique group key is combination of type and ID (since currently FNRT supports
            # same ID for each input type)
            input_type = entry['input_type']
            input_index = entry['input_index']
            group_key = input_type + '_' + str(entry['group_id'])
            if group_key not in groups:
               # new group entry
               groups[group_key] = {'inputs':{}, 'output':{}, 'chunks':{}}
               groups[group_key]['type'] = input_type
               groups[group_key]['instance'] = entry['group_id']
               groups[group_key]['is_master'] = False

            try:
               curr_input = groups[group_key]['inputs'][input_index]
            except KeyError:
               curr_input = None
            # FIXME: Create an action list for the input?  Indexed based on desc_count
            # so that at that given descriptor it calls the specified action handler?

            if entry['action'] == 'add_input':
               # if input index not in input list, then add a new entry
               # FIXME: what do we need to know about the input?  Range of descriptors its valid for?
               # what if it goes away and comes back again later - need a list of ranges!
               # for each active input we need to store the base addresses
               if curr_input is None:
                  # new input
                  curr_input = groups[group_key]['inputs'][input_index] = {}
                  curr_input['active'] = True
               else:
                  if curr_input['active']:
                     raise ConfigError(config_filename, config_reader.line_num, "Input %d already added" % input_index)
                  else:
                     # mark this input as active
                     curr_input['active'] = True

            # FIXME: if remove_input, store the ending descriptor index!?
            elif entry['action'] == 'remove_input':
               if curr_input is None or not curr_input['active']:
                  raise ConfigError(config_filename, config_reader.line_num, "Attempt to remove unknown input (%s)" % input_index)
               else:
                  curr_input['active'] = False

            elif entry['action'] == 'buf_status':
               # buf_status updates tell us:
               # - new chunk boundary
               # - new input for the chunk (verify input index same for a given group type/id)
               # - base addresses for the input (verify addresses same for given input)

               # verify this input is marked as active (-1 indictes output buffer information)
               if input_index != -1 and (curr_input is None or not curr_input['active']):
                  raise ConfigError(config_filename, config_reader.line_num, "Status update for an inactive input (%s)" % input_index)

               # FIXME: base addresses are input-specific.  For a given input, the base addresses
               # must be the same.
               # FIXME: Store the starting chunk ID for this input - allows verification of input selection
               # FIXME: if input_index is -1, store output base addresses (those used by muxlib)
               # FIXME: for every buf-status entry for the group, store the chunk starting descriptors
               # => allows verification of chunk count and starting descriptor index

            elif entry['action'] == 'set_master':
               # indicate that this is the "master" group for lip-sync purposes
               if master_already_set:
                  raise ConfigError(config_filename, config_reader.line_num, "Action 'Set Master' : Master has already been set!")
               else:
                  master_already_set = groups[group_key]['is_master'] = True
            else:
               # unknown action
               raise ConfigError(config_filename, config_reader.line_num, "Unknown Action (%s) detected " % entry['action'] )

            # FIXME: for each group type/id:
            # - store list of inputs
            #      - and their corresponding base addresses
            #      - and list of chunk indexes for this input
            # - store output base addresses (those used by muxlib)
            # - store list of chunk starting descriptor indexes


            # FIXME: for a new group, create a dictionary that contains: type, dict of inputs, entries
            # also determine expected number of chunks based on chunk boundaries
            # FIXME: Verify all entries for a given group ID have the same group type
            # FIXME: create a list of inputs for the given group, and store the addresses for that input
            # FIXME: Store the addresses of the output (sent to muxlib) by finding status update for input -1
            # FIXME: Store the descriptor counts of chunk boundaries ??
            # FIXME: verify no status updates occur for a given input after it has been removed

         # end: for each entry
      except ConfigError as e:
         test_fail(e)
         return None

      except CSVError as e:
         test_fail("Unable to process configuration file %s: %s" % (filename, e))
         return None

   #for group in groups:
   #   groups[group]['max_inputs']
   print "Detected %s group%s" % (len(groups), "s" if len(groups) != 1 else "")

   print groups
   # FIXME: process status updates: verify all updates for the same input have the same addresses
   print
   return groups

def verify_descriptors(group):
   test_result = True    # test passes until determined otherwise
   input_type = group['type']

   # one group of FNRT descriptors corresponds to one instance of muxlib input descriptors for the given type
   # muxlib inputs are named INPUT_<TYPE>_<Type Inst>_<MuxID>.csv
   # for FNRT testing, each group of the same type translates to a muxlib type instance.

   # read output descriptor file (mux input descriptors)
   try:
      out_desc_reader = muxlib_input.DescriptorFile(results_dir, input_type, group['instance'])
   except IOError,e:
      return test_fail(e)
   except muxlib_input.Error, e:
      return test_fail("Unable to process %s timing file" % input_type)
   try:
      muxlib_desc = [x for x in out_desc_reader]
   except muxlib_input.Error, e:
      return test_fail(e)

   # FNRT descriptos are named FNRT_DESC_<TYPE>_<group_inst>
   try:
      fnrt_desc_reader = muxlib_input.FnrtDescriptorFile(results_dir, input_type, group['instance'])
   except IOError,e:
      return test_fail(e)
   except muxlib_input.Error, e:
      return test_fail("Unable to process FNRT %s descriptor file" % input_type)

   print "Verifying descriptor validity ... "
   # fetch the FNRT input descriptors and output descriptors from the FNRT log
   fnrt_in_desc = []
   fnrt_out_desc = []
   fnrt_next = []
   fnrt_dropped = []
   for desc in fnrt_desc_reader:
      if desc.is_input_desc:
         fnrt_in_desc.append(desc)
      elif desc.is_output_desc:
         fnrt_out_desc.append(desc)
      elif desc.is_desc_dropped:
         fnrt_dropped.append(desc)
      elif desc.is_next_chunk:
         fnrt_next.append(desc)
      else:
         test_result = test_fail("Descriptor with unknown action %s encountered" % desc.action)

   print "FNRT to muxlib descriptor match ...",
   # verify that the FNRT output descriptors are the same as those seen at the input to muxlib
   if fnrt_out_desc != muxlib_desc:
      test_result = test_fail("FNRT output descriptors do not match muxlib input descriptors")
   else:
      print "OK"

   print "Chunk match (input to output) ...",
   num_chunks_in = max([x.chunk_id for x in fnrt_in_desc])+1
   num_chunks_out = max([x.chunk_id for x in fnrt_out_desc])+1
   if num_chunks_in != num_chunks_out:
      test_result = test_fail("%s chunks seen on input, but %s chunks seen on output!" %(num_chunks_in, num_chunks_out))
   else:
      print "OK"

   # verify that for every descriptor that came IN, there is a corresponding OUT
   # (ignoring differences in timing and offsets which are all changed by FNRT lib)
   # except metadata descriptors at chunk boundaries - look for NEXT or DROPPED
   # FIXME: This will not work for audio where it has to do lip-sync adjustments
   # (there may be audio inserted or removed to adjust timing)
   out_index = 0
   dropped_index = 0
   next_index = 0
   desc_map = {}
   print "FNRT input to output descriptor match ...",
   for desc in fnrt_in_desc:
      # if desc.count in group_config:
      #   process config change
      if out_index == len(fnrt_out_desc):
         test_result = test_fail("Insufficient output descriptors generated for supplied input descriptors")
         break;

      # see if this descriptor has a corresponding OUT descriptor (ignoring timing difference, and offset changes)
      out_desc = fnrt_out_desc[out_index]
      diffs = desc.compare(out_desc, exclude=['pts','escr','dts','offset', 'action'])
      if diffs:
         # NOTE: the ordering of the comparisons is important, since the last descriptor will
         # NOT have a "NEXT", so check for matching "DROPPED" first.

         # see if its a dropped descriptor (must be same except for offset and action)
         if dropped_index < len(fnrt_dropped) and not desc.compare(fnrt_dropped[dropped_index], ['offset', 'action']):
            # found a matchng "DROPPED" descriptor
            dropped_index += 1
         # NOTE: This comparison must be made using a filter to remove the chunk_id since
         # a "Next" descriptor has the next chunk's id, and input index is -1
         elif next_index < len(fnrt_next) and not desc.compare(fnrt_next[next_index], ['chunk','input','offset', 'action']):
            # found a matching "NEXT" descriptor
            next_index += 1
         else:
            # not dropped or next, so is missing
            test_result = test_fail("Input descriptor [%s] not found in output,dropped or next" % desc.count)
      else:
         # FIXME: store the descriptors or the indicies?
         desc_map[desc] = out_desc
         out_index += 1
   if test_result:
      print "OK"

   inputs = {}
   chunks = {}
   # FIXME: verify that the output descriptors corresponding to a given input match correctly with the
   # inputs specified by the config
   # e.g. if an input is removed at descriptor X, then there must be no descriptors with that input beyond descriptor X
   for desc in fnrt_out_desc:
      # FIXME: if desc_index in group[actions] the do action
      # if not input[active] and desc.input_index = input[index] the error (descriptors found for inactive input)
      if desc.input_index in inputs:
         # existing input
         inputs[desc.input_index].append(desc)
      else:
         # new input
         inputs[desc.input_index] = [desc]
      if desc.chunk_id in chunks:
         chunks[desc.chunk_id].append(desc)
      else:
         chunks[desc.chunk_id] = [desc]

   print "Verifying concurrent inputs count ... %s inputs ..." % len(inputs),
   if len(inputs) != len(group['inputs']):
      test_result = test_fail("Input count mismatch (%s inputs indicated in config, %s inputs seen in descriptors)" % (len(group['inputs']), len(inputs)))
   else:
      print "OK"
   #FIXME: Verify input match ... inputs specified in config have descriptors in the dump

   print "Verifying Chunk Count ... %s chunks ..." % len(chunks),
   if len(chunks) != num_chunks_out:
      test_result = test_fail("Chunk Count mismatch (%s marked as OUT, %s chunks encountered)" % (num_chunks_out, len(chunks)))
   else:
      print "OK"

   # FIXME: can't verify base addresses anymore, so verify that offsets are correct for a given input
   # (original offset + base address for the input == new offset + base address from the muxlib descriptor)

   # FIXME: for each chunk verify that input is same (can't verify offsets or base-addresses anymore)
   # NOTE: can't rely on counts since audio/video will be interleaved
   # Verify chunk IDs are in order (0 -> N)
   print "Verifying Chunk Order ...",
   expected_chunk_id = 0
   for desc in fnrt_out_desc:
      if desc.chunk_id != expected_chunk_id:
         if desc.chunk_id != expected_chunk_id+1:
            test_result = test_fail("Desc [%s]:: Chunk ID Invalid (expecting %s, got %s)" %
               (desc.count, expected_chunk_id, desc.chunk_id))
         else:
            expected_chunk_id = desc.chunk_id
   if test_result:
      print "OK"

   print "Verifying Metadata ...",
   # verify that the first metadata descriptor is the one that is retained and has a chunk ID of zero
   metadata_desc = [x for x in fnrt_out_desc if x.is_metadata]
   if len(metadata_desc) != 1:
      test_result = test_fail("Expecting only on metadata descriptor in the output")

   if not fnrt_out_desc[0].is_metadata and fnrt_out_desc[0].chunk_id == 0 and fnrt_out_desc[0].count == 0:
      test_result = test_fail("Expecting metadata descriptor as first output descriptor in the first chunk")

   # verify that only metadata descriptors were dropped (only true for the "master" group -
   # slaved groups can drop descriptor for lip-sync purposes)
   if group['is_master'] and not all([x.is_metadata for x in fnrt_dropped]):
      test_result = test_fail("Non-metadata descriptor was dropped!")
   if test_result:
      print "OK"

   return test_result


def dummy_func(hello):






   print "Verifying input base addresses ...",
   # FIXME: verify that FNRT input base + offset == bmuxlib base + offset (i.e. the resultant address is same in both cases)
   # NOTE: need to add a config entry that specifies the FNRT base address whenever a status update is made

   # verify that the descriptor base addresses are correct for each input
   # (all descriptors from the same input must have the same base address)
   for input,desc_list in vid_inputs.items():
      base_address = None
      for desc in desc_list:
         if base_address is None:
            if not desc.is_metadata:
               base_address = desc.base_addr
            else:
               # skip initial outgoing metadata descriptor
               continue
         elif base_address != desc.base_addr:
            test_result = test_fail("Input [%s]: Desc [%s]:: base address incorrect (expecting %s, got %s)" %
               (input, desc.count, base_address, desc.base_addr))
         else:
            pass
   if test_result:
      print "OK"

   print "Verifying Chunk Base addresses and input index ...",
   # verify that the base address and input index for each chunk is correct
   # (all descriptors for a given chunk must have the same base address and input index)
   for chunk_id, desc_list in vid_chunks.items():
      base_address = None
      input_index = None
      for desc in desc_list:
         if base_address is None:
            if not desc.is_metadata:
               base_address = desc.base_addr
            else:
               continue
         elif base_address != desc.base_addr:
            test_result = test_fail("Input [%s]: Desc [%s]:: base address incorrect (expecting %s, got %s)" %
             (input, desc.count, base_address, desc.base_addr))

         if input_index is None:
            input_index = desc.input_index
         elif input_index != desc.input_index:
            test_result = test_fail("Input [%s]: Desc [%s]:: input index incorrect (expecting %s, got %s)" %
             (input, desc.count, input_index, desc.input_index))
   if test_result:
      print "OK"





   print "Verifying Video Timing..."
   # FIXME: timing only applies to frame-boundary descriptors - so need to filter these
   delta_dts = [y.dts - x.dts for (x,y) in pairwise(video_out_desc)]
   # read dts values
   # determine dDTS values
   # verify dDTS within allowed range
   # NOTE: dPTS values vary due to B-frame reordering!!
   # verify dDTS or dPTS is not negative

   # FIXME: To verify PTS timing values, we need to determine the original PTS-DTS diff and apply that to
   # the new DTS to get the adjusted PTS - then compare that value

   # FIXME: verify ESCR timing


   video_desc_duration = video_out_desc[-1].dts - video_out_desc[0].dts
   print "Duration = %s (%s ms)" % (video_desc_duration, video_desc_duration/90)

   # FIXME: Verify sum of chunk durations is equal to input duration and to the OPTS duration
   # (ensures that there is no long-term drift or error accumulation)

   print "Verifying Audio Descriptor Validity ... "
   print "Verifying Audio Metadata ... "
   print "Verifying Audio Timing ..."

   print "Verifying Lip-Sync ... "
   # ensure ADTS - VDTS is consistent (can't do it for PTS due to reordering, unless we account for PTS-DTS delay)
   # if DTS not valid, use PTS.
   # ensure AESCR - VESCR is consistent (i.e. within allowed bounds)
   # FIXME: For each video verify lip-sync against each audio

   # TO DO: Discontinuitites - break the incoming data into contiguous "segments".  Ensure timing params are consistent
   # within each segment, and ensure that sum of each segment's duration (PTS, DTS, ESCR etc) equals the output
   # duration (there should be NO discontinuities in the output/ input to mux - mux does not support it)
   return True

def verify_fnrt(results_dir):
   test_result = True   # test passes until determined otherwise

   # need to be able to deal with audio-only and video-only vs combined
   # thus, if either file is missing, simply do a single-stream verification
   # if no video INPUTs active, then there will be no outputs. So we need to know
   # what the FNRT config is.  Thus, we expect that there will be an FNRT config file
   # that indicates when inputs are added/removed so we can determine which are present
   # FIXME: The issue here is that addition or removal of interfaces is dynamic
   # so this has to be synchronized with the input file descriptor handling.  If an
   # input is removed, then we have to stop reading the descriptor file for it AT THE RIGHT TIME!
   # would this be synchronized based on time, or descriptor count across all active inputs.
   # (the latter would be better, since we can add/remove at the correct descriptor index)


   # determine how many input groups there are and create a per-group config
   # based around a group index/ID ... the corresponding descriptor file will be numbered based
   # on that ID (currently expect zero or one video and zero or more audio)
   groups = process_config(results_dir)
   if not groups:
      # group processing failed, so test failed
      return False

   # verify the descriptors processed for each group of inputs specified by the config ...
   for group_name,group in groups.items():
      print "Processing Group %s: %s concurrent %s input%s..." % (str(group['instance']), len(group['inputs']), group['type'], "" if len(group['inputs']) == 1 else "s")
      test_result = verify_descriptors(group)
      print "Group %s (%s):" % (group['instance'], group['type']),
      print "OK" if test_result else "has ERRORS!"
      print

   types = [groups[x]['type'] for x in groups]
   if types.count('video') > 1:
      test_result = test_fail("Currently only a single video is supported/expected");

   # if both audio and video types are present, then verify lip-sync
   # between each video and each audio present
   if 'audio' in types and 'video' in types:
      print "Verifying lip-sync ..."
      # FIXME: print video group X to audio group Y ... OK
      # Verify lip sync by ensuring that error VPTS/VDTS to APTS/ADTS does not exceed some allowed maximum
      # do same with ESCR
      verify_lip_sync()

   print
   return test_result

if __name__ == '__main__':
   args = get_cmd_line_args()
   test_result = False

   results_dir = args.results or "."

   try:
      test_result = verify_fnrt(results_dir)
   except TestFail, reason:
      print "FNRT Test aborted: %s" % reason

   print ("Test OK" if test_result else "Test FAILED!")
   sys.exit(not test_result)
