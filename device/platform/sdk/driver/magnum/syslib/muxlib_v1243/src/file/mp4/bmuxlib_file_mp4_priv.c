/***************************************************************************
 *     Copyright (c) 2010-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmuxlib_file_mp4_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/67 $
 * $brcm_Date: 6/28/12 4:17p $
 *
 * [File Description:]
 * Private functions for File-based MP4 software mux
 *
 * NOTE: MP4 is a big-endian format!
 *
 * Operational details:
 * ====================
 *
 *
 * Typical file structure:
 * =======================
 * ftyp (m)                [fixed - can create brands based on content]
 * pdin (?)                [required only for assisting streamers]
 * moov (m)                [container only]
 *   mvhd (m)              [createtime, modtime, timescale, duration, next_track_ID]
 *   trak (t)              [container only]
 *      tkhd               [createtime, modtime, trackID, duration, width/height]
 *      tref (?)           [only required if this track references another - e.g. for Parameter track (media track references the param track)]
 *      edts               [container only]
 *         elst            [specifies initial offset between tracks]
 *      mdia               [container only]
 *         mdhd            [createtime, modtime, timescale (90kHz for video, sample rate for audio), duration, language (currently set to "unk")]
 *         hdlr            [handler type (based on track type), no other info required]
 *         minf            [container only]
 *            vmhd/smhd/etc  [typically, these contain predefined information]
 *            dinf           [container only]
 *               dref        [URL - empty]
 *            stbl           [container only]
 *               stsd        [codec-specific information]
 *                  avc1     [AVC only]
 *                     avcc  [AVC only]
 *                  mp4a/v/s [MPEG 4 only]
 *                     esds  [MPEG 4 only - contains an ES Descriptor and its Decoder Specific Config]
 *               stts        [from stored metadata - DTS deltas]
 *               ctts        [from stored metadata - DTS/CTS offset]
 *               stsc        [from analysis of sample offsets to create chunks]
 *               stsz        [from stored metadata - sample size]
 *               stco        [from stored metadata - mdat offsets]
 *               stss        [from stored metadata - RAP indications]
 *               padb (?)    [only needed if encoder output needs byte alignment]
 *    udta (?)
 *       cprt (?)  [might be required by customers]
 * mdat (m)   [container only]
 *
 *  (m) = once per file
 *  (t) = once per track
 *  (?) = optional or may not be needed
 *
 ***************************************************************************
 * [Revision History:]
 * $brcm_Log: /magnum/syslib/muxlib/7425/file/mp4/bmuxlib_file_mp4_priv.c $
 * 
 * Hydra_Software_Devel/67   6/28/12 4:17p delkert
 * SW7231-874: Fix bug in unbounded box size calculations. Add debug to
 * verify box buffer usage matches requested bytes. Improve debug output
 * for boxes.
 *
 * Hydra_Software_Devel/66   6/20/12 11:41a delkert
 * SW7425-3272: Fix up coverity annotation so it is by itself in the
 * comment
 *
 * Hydra_Software_Devel/65   6/20/12 10:36a delkert
 * SW7425-3272: fixes for coverity errors
 *
 * Hydra_Software_Devel/64   5/29/12 1:53p delkert
 * SW7425-2567: Add return code checks to fix coverity errors
 *
 * Hydra_Software_Devel/63   4/12/12 10:27a delkert
 * SW7425-2567: Fix usage of BDBG_DEBUG_BUILD
 *
 * Hydra_Software_Devel/62   2/6/12 9:52a delkert
 * SW7425-2315: Restructure release queues to support "empty frame"
 * disposal.  Modify state machines to treat empty frame as end of
 * sample.
 *
 * Hydra_Software_Devel/61   1/25/12 1:47p delkert
 * SW7425-1378: Fix compilation warning.
 *
 * Hydra_Software_Devel/60   1/19/12 4:25p delkert
 * SW7425-1378: Refactor to use common muxlib input module. Remove support
 * for stream trucation for offset correction (edit list always used).
 * Remove stall timeout. Remove workarounds for SW7425-357 and SW7425-
 * 624.
 *
 * Hydra_Software_Devel/59   1/9/12 3:50p delkert
 * SW7425-2121: Fix storage create settings to match changes in output API
 *
 * Hydra_Software_Devel/58   11/16/11 12:07p delkert
 * SW7425-1514: Fix up timescale for composition time
 * SWCHECKFRAME-114: Use accessor macros to manipulate mux state
 *
 * Hydra_Software_Devel/57   11/14/11 4:08p delkert
 * SW7425-1379: Refactor to use common output module:
 * Add Release Qs & callback data to support out-of-order "completion".
 * Remove uiNumActiveInputs (use interface to ecide if input is acive).
 * Split box processing so that only one descriptor output at-a-time.
 * Add cleanup if any output fails creation.
 * App "blocked" status is now dependent only on storage.
 *
 * Hydra_Software_Devel/56   10/18/11 3:25p delkert
 * SW7425-1513: Remove late-input warning. Add warning if MP4V
 * configuration data changes, and made the warning indicate that dynamic
 * configuration is not supported
 *
 * Hydra_Software_Devel/55   10/18/11 2:10p delkert
 * SW7425-1513: Add warning when any input is late by more than approx.
 * 500ms.  Add warning when AVC SPS or PPS data changes.
 *
 * Hydra_Software_Devel/54   10/11/11 5:12p delkert
 * SW7425-1477: Move EOS detection during video descriptor processing to
 * ensure that EOS is detected on any descriptor.
 *
 * Hydra_Software_Devel/53   10/5/11 6:06p delkert
 * SW7425-1415: Fix coverity issues
 *
 * Hydra_Software_Devel/52   10/5/11 3:00p delkert
 * SW7425-1410: Split Output processing & modify sequencing to ensure
 * completed desc processed first & waiting desc processed last.  Move
 * output processing into input & box processing funcs.
 *
 * Hydra_Software_Devel/51   9/30/11 8:42p delkert
 * SW7425-624: Add temporary workaround for bad buffer wrap behaviour
 *
 * Hydra_Software_Devel/50   9/14/11 11:39a delkert
 * SW7425-1282: Add input stall timeout to start setting.
 *
 * Hydra_Software_Devel/49   7/26/11 10:43a delkert
 * SW7425-989: Remove error condition when input type or protocol is bad -
 * stream should be ignored instead.
 *
 * Hydra_Software_Devel/48   7/7/11 9:54a delkert
 * SW7425-326: Clean up output descriptor logs (use common macro).
 *
 * Hydra_Software_Devel/47   6/30/11 3:50p delkert
 * SW7425-326: Fix error in output descriptor diagnostics
 *
 * Hydra_Software_Devel/46   6/29/11 11:41a delkert
 * SW7425-673: Use DoMux status for finish steps.
 *
 * Hydra_Software_Devel/45   6/28/11 12:54p delkert
 * SW7425-326: Improve Output (Storage I/O) debug
 *
 * Hydra_Software_Devel/44   6/23/11 8:43p delkert
 * SW7425-673: Add debug to indicate % complete during finalization
 *
 * Hydra_Software_Devel/43   6/17/11 9:41p delkert
 * SW7425-738: Fix up 64-bit offset handling for co64 box.
 *
 * Hydra_Software_Devel/42   6/15/11 3:50p delkert
 * SW7425-711: Ignore all input data after EOS is seen (workaround for
 * audio discontinuties upon stream wrap).  Cleanup input descriptor
 * debug
 *
 * Hydra_Software_Devel/41   6/7/11 4:33p delkert
 * SW7425-325: Move status into context struct, and use top-level state
 * direct from there. Split blocked indication into enum indicating
 * cause. Return blocked output indication in status. Change execution
 * interval during finalization to zero (AFAP)
 *
 * Hydra_Software_Devel/40   6/1/11 5:09p delkert
 * SW7425-377: Add relocation buffer for mdat relocation/progressive
 * download support
 *
 * Hydra_Software_Devel/39   5/31/11 3:40p delkert
 * SW7425-446: Increase timeout on "stalled input" detection (some nexus
 * examples timeout on audio).  Minor cleanup.
 *
 * Hydra_Software_Devel/38   5/27/11 2:49p delkert
 * SW7425-446: Fix up processing of mp4v configuration DUs.
 *
 * Hydra_Software_Devel/37   5/27/11 2:16p delkert
 * SW7425-657: Add detection of "stalled" input to allow processing to
 * ignore the input and move on.
 *
 * Hydra_Software_Devel/36   5/27/11 10:22a delkert
 * SW7425-446: Ignore metadata descriptors if metadata buffer base address
 * is invalid
 *
 * Hydra_Software_Devel/35   5/26/11 5:07p delkert
 * SW7425-657: Skip inputs that are invalid or not supported.  Fix
 * IsInputProcessingDone to skip inputs that never processed any data
 *
 * Hydra_Software_Devel/34   5/26/11 2:21p delkert
 * SW7425-446: Ignore mp4v userdata data units (generated by encoder for
 * testing purposes)
 *
 * Hydra_Software_Devel/33   5/25/11 12:00p delkert
 * SW7425-446: Add eAac protocol equivalency for eAacPlusAdts
 *
 * Hydra_Software_Devel/32   5/24/11 2:45p delkert
 * SW7425-446: Initial processing of MPEG 4 part 2 data units and config
 * data extraction
 *
 * Hydra_Software_Devel/31   5/24/11 10:15a delkert
 * SW7425-446: Fix up sync processing to correctly work when DTS is
 * invalid (e.g. for mux_static_test)
 *
 * Hydra_Software_Devel/30   5/23/11 6:06p delkert
 * SW7425-491: Add checks for changing video metadata
 *
 * Hydra_Software_Devel/29   5/23/11 5:56p delkert
 * SW7425-491: Add checks for missing codec metadata and use defaults. Add
 * checks for changing codec metadata (not supported). Add Checks if
 * configuration data exceeds available storage space.
 *
 * Hydra_Software_Devel/28   5/23/11 9:16a delkert
 * SW7425-491: Add metadata descriptor processing to video state machine.
 * Change assert for bad audio descriptor offsets to a warning (see
 * SW7425-624)
 *
 * Hydra_Software_Devel/27   5/23/11 8:12a delkert
 * SW7425-491: Move Sample metadata processing to common code to avoid
 * problems due to duplication
 *
 * Hydra_Software_Devel/26   5/20/11 1:40p delkert
 * SW7425-491: Move startup descriptor processing to a separate state. Fix
 * metadata and EOS descriptor processing to correctly consume/release
 * the descriptors. Fix EOS handling (Input done condition now requires
 * EOS)
 *
 * Hydra_Software_Devel/25   5/18/11 5:27p delkert
 * SW7425-491: Initial changes to audio state machine to support in-band
 * metadata descriptors
 *
 * Hydra_Software_Devel/24   5/13/11 11:56a delkert
 * SW7425-446: Split box and metadata processing into separate files. Add
 * edit list box to specify initial offset between streams to fix lipsync
 * issue.
 *
 * Hydra_Software_Devel/23   5/5/11 1:39p delkert
 * SW7425-301: fix segfault due to descriptor destination being NULL
 * (correctly reset descriptors used count).  Add compile option (debug)
 * to dump output descriptor queue.
 *
 * Hydra_Software_Devel/22   5/4/11 4:15p delkert
 * SW7425-302: Fix offset stack pop to occur after descriptor allocation
 * to avoid empty stack if run out of descriptors.  Separate input and
 * output descriptor debug.
 *
 * Hydra_Software_Devel/21   5/4/11 2:45p delkert
 * SW7425-300: Fix box buffer size setting to represent usable space not
 * total buffer size. Fix debug reporting of max buffer usage.
 *
 * Hydra_Software_Devel/20   5/4/11 1:50p delkert
 * SW7425-300: Fix assert if Box Buffer size set less than peak usage.
 * Minor cleanup of Box Buffer debug. Minor cleanup of API descriptions.
 *
 * Hydra_Software_Devel/19   5/3/11 4:48p delkert
 * SW7425-327: Change box buffer writes to be byte-based to avoid
 * alignment exceptions. Remove byte-swapping on box buffer
 * constants/values. Change FourCC byte order. Merge FINISH and
 * FINISH_PRIV debug. Cleanup descriptor macros.
 *
 * Hydra_Software_Devel/18   5/2/11 8:49a delkert
 * SW7425-232: Fix up usage of BERR_TRACE.
 *
 * Hydra_Software_Devel/17   5/2/11 8:34a delkert
 * SW7425-232: Fix up usage of BERR_TRACE, and improve error reporting
 *
 * Hydra_Software_Devel/16   4/27/11 8:08p delkert
 * SW7425-382: Fix processing of waiting descriptors when storage is busy.
 *
 * Hydra_Software_Devel/15   4/26/11 12:13p delkert
 * SW7425-382: Removed temporary increase of cache entries.  Add check to
 * detect when storage is "busy" when queing descriptors to avoid
 * corrupted queue, and subsequent assert
 *
 * Hydra_Software_Devel/14   4/20/11 2:57p nilesh
 * SW7425-389: Fix A/V lip sync by synchronizing initial DTS value across
 * all inputs
 *
 * Hydra_Software_Devel/13   4/20/11 1:11a nilesh
 * SW7425-382: Fixed audio buffer descriptor management when descriptors
 * are split
 *
 * Hydra_Software_Devel/12   4/19/11 11:42p nilesh
 * SW7425-382: Fixed finish EOS handling
 *
 * Hydra_Software_Devel/11   4/19/11 6:17p nilesh
 * SW7425-288: MP4 Mux Audio Support
 *
 * Hydra_Software_Devel/SW7425-288/5   4/19/11 5:57p nilesh
 * SW7425-382: Added BMUX_MP4_FINISH_PRIV debug messages.  Temporary fix
 * for finish timeout.
 *
 * Hydra_Software_Devel/SW7425-288/4   4/19/11 4:57p nilesh
 * SW7425-288: Use uiRawDataOffset and uiRawDataLength for audio
 * descriptors to get RAW audio data
 *
 * Hydra_Software_Devel/SW7425-288/3   4/19/11 4:53p nilesh
 * SW7425-288: Fixed bug in selection of lowest DTS
 *
 * Hydra_Software_Devel/SW7425-288/2   4/19/11 2:40a nilesh
 * SW7425-373: Updated for latest API PI
 *
 * Hydra_Software_Devel/10   4/18/11 11:59p nilesh
 * SW7425-373: Fixed metadata cache flush logic when write pointer doesn't
 * change
 *
 * Hydra_Software_Devel/SW7425-288/1   4/18/11 1:32p nilesh
 * SW7425-288: Added audio metadata support
 *
 * Hydra_Software_Devel/9   4/15/11 9:18p delkert
 * SW7425-73: Allocate DecoderSpecificInfo storage on a per-track basis.
 * Fix ordering issue with init of audio timescale. Add init for audio
 * max bitrate until we get the real metadata.
 *
 * Hydra_Software_Devel/8   4/14/11 4:05p delkert
 * SW7425-357: Add workaround for SW7425-355 (encoder produces bad NALU
 * causing descriptors that have SOF but not DU start). Output warning
 * when this condition detected.
 *
 * Hydra_Software_Devel/7   4/14/11 10:32a delkert
 * SW7425-75: Add video metadata descriptor support.
 * SW7425-73: Improve descriptor debug. Add error check for invalid input
 * protocol.
 *
 * Hydra_Software_Devel/6   4/13/11 12:35p delkert
 * SW7425-73: Correct the deltaDTS calc for audio. Keep duration
 * calculation in 90kHz and scale for audio.
 *
 * Hydra_Software_Devel/5   4/11/11 8:45p delkert
 * SW7425-325: Return 50ms "next execution time" from DoMux.
 * SW7425-73: Change default progressive download support to false (not
 * implemented yet). Remove assert for output queue clear. Change DTS
 * delta asserts into warnings.
 *
 * Hydra_Software_Devel/4   4/7/11 7:46p delkert
 * SW7425-73: Add Average Bitrate calculations. Add hard-coded
 * AudioSpecificConfig metadata (for testing purposes). Fix up MPEG 4
 * descriptor sizes.
 *
 * Hydra_Software_Devel/3   4/7/11 1:24p delkert
 * SW7425-73: Modifications to match changes to Storage
 * DestroyStorageObject() interface
 *
 * Hydra_Software_Devel/2   4/6/11 9:11p delkert
 * SW7425-73: Modifications to match Storage API changes
 *
 * Hydra_Software_Devel/1   4/4/11 12:53p delkert
 * SW7425-73: Merged mp4_mux_devel to mainline
 *
 * Hydra_Software_Devel/mp4_mux_devel/37   4/1/11 5:16p delkert
 * SW7425-73: Fix AVC Parameter set processing to correctly work with
 * split NALU. Temporarily remove assert when two consecutive frames have
 * same DTS (to workaround VCE issue). Fix warnings when compiling non-
 * debug build
 *
 * Hydra_Software_Devel/mp4_mux_devel/36   3/31/11 11:58a delkert
 * SW7425-73: Move sampling of buffer base address to be once after first
 * descriptor seen, rather than once per frame.
 *
 * Hydra_Software_Devel/mp4_mux_devel/35   3/31/11 10:48a delkert
 * SW7425-73: Improve debugging output
 *
 * Hydra_Software_Devel/mp4_mux_devel/34   3/30/11 6:06p delkert
 * SW7425-73: Add information to assist with setting minimums for storage
 * allocation. Add debug to determine usage (instantaneous and peak) of
 * allocated storage.  Made offset stack internal to the mux (based on
 * max box nesting level)
 *
 * Hydra_Software_Devel/mp4_mux_devel/33   3/30/11 1:30p delkert
 * SW7425-73: Add MPEG 4 ES (Audio/Video) support. Add MPEG 4 Descriptors
 * required for ESDS box. Add input checking for supported codec types.
 * Fix duration/timescale calcs for audio.
 *
 * Hydra_Software_Devel/mp4_mux_devel/32   3/28/11 8:54a delkert
 * SW7425-73: Fix up selection of stream with Lowest DTS to correctly take
 * into account DTS Wrap.  Similarly for the CTS/DTS diff and delta DTS
 * asserts.
 *
 * Hydra_Software_Devel/mp4_mux_devel/31   3/25/11 4:41p delkert
 * SW7425-73: Add audio input descriptor processing
 *
 * Hydra_Software_Devel/mp4_mux_devel/30   3/25/11 3:12p delkert
 * SW7425-73: Fix Box Buffer processing to allow for "out of space" error
 * between boxes when creating nested boxes
 *
 * Hydra_Software_Devel/mp4_mux_devel/29   3/24/11 2:18p delkert
 * SW7425-73: Major rework of input descriptor processing to support split
 * NALU / split start codes
 *
 * Hydra_Software_Devel/mp4_mux_devel/28   3/18/11 6:05p delkert
 * SW7425-73: Add read-back of metadata from storage to create relevant
 * box contents. Merged CreateAuxiliaryTracks() and
 * CreateMetadataStorage() into Start().  Merged queue checking and
 * DestroyMetadataStorage() into Stop()
 *
 * Hydra_Software_Devel/mp4_mux_devel/27   3/18/11 11:24a delkert
 * SW7425-73: Fix NALU processing to extract NALU type from data unit type
 * provided in descriptor to match expected VCE PI behaviour.  Add assert
 * to check for NALU split across descriptors (not supported ... yet).
 *
 * Hydra_Software_Devel/mp4_mux_devel/26   3/17/11 8:33a delkert
 * SW7425-73: Fix bug with endianness of the AVC profile/level
 *
 * Hydra_Software_Devel/mp4_mux_devel/25   3/16/11 7:48p delkert
 * SW7425-73: Add metadata processing to finalize the moov.
 *
 * Hydra_Software_Devel/mp4_mux_devel/24   3/16/11 1:54p delkert
 * SW7425-73: Add processing of AVC PPS and SPS, and add creation of AVC1
 * and AVCC boxes
 *
 * Hydra_Software_Devel/mp4_mux_devel/23   3/15/11 5:55p delkert
 * SW7425-73: Set timescale, durations and width/height.  Add AVC NALU
 * processing to remove start codes and add NALU length fields
 *
 * Hydra_Software_Devel/mp4_mux_devel/22   3/14/11 8:20p delkert
 * SW7425-73: Fix problem with write index when writing stco metadata to
 * cache.
 *
 * Hydra_Software_Devel/mp4_mux_devel/21   3/11/11 6:34p delkert
 * SW7425-73: Add ctts, stts, stsc, stsz, stco, stss and co64 boxes. Store
 * metadata in cache in file format. Fix up optional box handling (must
 * be per-track). Add run-length coding for stts and ctts boxes. Add end-
 * of-stream metadata finalization
 *
 * Hydra_Software_Devel/mp4_mux_devel/20   3/10/11 5:59p delkert
 * SW7425-73: Add flags for controlling inclusion of optional boxes.
 * Added creation of vmhd, smhd, hmhd, nmhd, dinf, dref and url boxes.
 * Added creation of Video and Audio Sample Entry Boxes
 *
 * Hydra_Software_Devel/mp4_mux_devel/19   3/10/11 11:39a delkert
 * SW7425-73: Fix bug in box size calcs: must be big-endian!
 *
 * Hydra_Software_Devel/mp4_mux_devel/18   3/10/11 11:12a delkert
 * SW7425-73: Add track types for moov creation.  Rework state machine to
 * allow properly nested boxes. Add box size update code. Add trak, tkhd,
 * mdia, mdhd & hdlr box cration.
 *
 * Hydra_Software_Devel/mp4_mux_devel/17   3/8/11 5:54p delkert
 * SW7425-73: Add storage for size fields (updates to boxes, or for NALU).
 * Unify OutputMP4Headers() and FinalizeOutput() into ProcessBoxes() to
 * allow sharing of states. Add states for moov processing.  Add initial
 * moov and mvhd creation.
 *
 * Hydra_Software_Devel/mp4_mux_devel/16   3/8/11 10:15a delkert
 * SW7425-73: Add finalization of mdat (update size). Add ability for box
 * creation to update output storage (i.e. seek within the storage).
 *
 * Hydra_Software_Devel/mp4_mux_devel/15   3/7/11 4:24p delkert
 * SW7425-73: Fix bug with active output count re: handling of mdat output
 * when not used
 *
 * Hydra_Software_Devel/mp4_mux_devel/14   3/7/11 4:11p delkert
 * SW7425-73: Add stack for managing starting offsets of nested boxes.
 * Fix up error codes, and error handling
 *
 * Hydra_Software_Devel/mp4_mux_devel/13   3/7/11 11:04a delkert
 * SW7425-73: Add input and output completion predicates, and add metadata
 * cache final flush upon "finishing"
 *
 * Hydra_Software_Devel/mp4_mux_devel/12   3/4/11 2:55p delkert
 * SW7425-73: Fix segfault problem when destroying metadata storage.
 *
 * Hydra_Software_Devel/mp4_mux_devel/11   3/3/11 4:55p delkert
 * SW7425-73: Add metadata caching and cache processing to input
 * processing
 *
 * Hydra_Software_Devel/mp4_mux_devel/10   3/2/11 3:04p delkert
 * SW7425-73: Add returning of input descriptors back to encoder. Fix bug
 * where all descriptors in part 1 of the descriptor array are skipped
 *
 * Hydra_Software_Devel/mp4_mux_devel/9   3/2/11 11:50a delkert
 * SW7425-73: Get input descriptor processing operational, and fix up
 * descriptor allocation.  Add temporary processing of H.264 NALU start
 * codes for evaluation purposes.
 *
 * Hydra_Software_Devel/mp4_mux_devel/8   2/25/11 8:37p delkert
 * SW7425-73: Add mdat header creation.  Fix descriptor processing, and
 * improve descriptor allocation to allow proper error handling if
 * resources unavailable.
 *
 * Hydra_Software_Devel/mp4_mux_devel/7   2/24/11 10:59a delkert
 * SW7425-73: Add processing of completed descriptors
 *
 * Hydra_Software_Devel/mp4_mux_devel/6   2/23/11 5:57p delkert
 * SW7425-73: Restructure metadata cache handling to be type-independent.
 * Restructure storage handling to make it easier to traverse active
 * outputs for processing
 *
 * Hydra_Software_Devel/mp4_mux_devel/5   2/23/11 1:41p delkert
 * SW7425-73: Add outgoing (waiting) descriptor queue processing
 *
 * Hydra_Software_Devel/mp4_mux_devel/4   2/22/11 7:50p delkert
 * SW7425-73: Add metadescriptors for managing output descriptors.
 * Initial box processing to enable box creation using mux_static_test
 * * Hydra_Software_Devel/mp4_mux_devel/3   2/15/11 5:08p delkert
 * SW7425-73: Add infrastructure for metadata storage and caching.
 * Initialise context, create metadata storage, and add functions to
 * destroy storage.
 *
 * Hydra_Software_Devel/mp4_mux_devel/2   2/13/11 9:10p delkert
 * SW7425-73: Add initial API functionality, stubs and prototypes, and get
 * it to compile
 *
 * Hydra_Software_Devel/mp4_mux_devel/1   2/13/11 8:26p delkert
 * SW7425-73: Add initial private file for MP4 mux
 *
 ***************************************************************************/

#include "bstd.h" /* also includes berr, bdbg, etc */
#include "bkni.h"

#include "bmuxlib_file_mp4_priv.h"

BDBG_MODULE(BMUXLIB_FILE_MP4_PRIV);
BDBG_FILE_MODULE(BMUX_MP4_IN_DESC);    /* enables input descriptor diagnostics */
BDBG_FILE_MODULE(BMUX_MP4_OUT_DESC);   /* enables output descriptor diagnostics */
BDBG_FILE_MODULE(BMUX_MP4_RELEASEQ);   /* enables release Q diagnostics */
BDBG_FILE_MODULE(BMUX_MP4_DU);         /* enables data unit diagnostics */
BDBG_FILE_MODULE(BMUX_MP4_INPUT);      /* enables input diagnostics */
BDBG_FILE_MODULE(BMUX_MP4_OUTPUT);     /* enables output diagnostics */
BDBG_FILE_MODULE(BMUX_MP4_STATE);      /* enables state machine diagnostics */
BDBG_FILE_MODULE(BMUX_MP4_FINISH);     /* enables finish diagnostics */

/* TODO:
   [ ] Auxiliary track creation (SDS, ODS, AVC Params)
   [ ] Store H.264 PPS and SPS in parameter set track
   [ ] Get PI bitrates for up-front estimation of mdat and metadata temp storage requirements
   [ ] If necessary, add CreateExtendedBox and CreateLargeExtendedBox etc for extended type support.
   [ ] See if there's a way to eliminate the FinalizeMdat and AllocMdatSizeDesc states and reuse the FinishBox and AllocOutDescSize
   [ ] Optimize for chunk creation (coalesce runs of samples from the same input) - and fix stsc and stco boxes
   [ ] Determine Max bitrate from metadata?
   [ ] Review ASSERT usage and detemine if any (or all) need to be error codes
   [ ] get necessary metadata from encoder/PI for use in headers:
Metadata that's currently missing:
* HRD Size (for ESD in MPEG4 config).
*/

/****************************
     Static Prototypes
****************************/

static void InitializeAuxiliaryTracks(BMUXlib_File_MP4_Handle hMP4Mux);
static BERR_Code CreateOutputs(BMUXlib_File_MP4_Handle hMP4Mux);
static void DestroyOutputs(BMUXlib_File_MP4_Handle hMP4Mux);

static BERR_Code ProcessVideoDescriptors(BMUXlib_File_MP4_Handle hMP4Mux, BMUXlib_File_MP4_P_BlockedCause *peBlocked);
static BERR_Code ProcessAudioDescriptors(BMUXlib_File_MP4_Handle hMP4Mux, BMUXlib_File_MP4_P_BlockedCause *peBlocked);

static void InitializeSampleMetadata(BMUXlib_File_MP4_P_CurrentSample *pCurrentSample , const BMUXlib_Input_Descriptor *pInDesc, const uint64_t uiOffset);
static void UpdateSampleMetadata(BMUXlib_File_MP4_P_CurrentSample *pCurrentSample, const BMUXlib_Input_Descriptor *pInDesc);
static void AudioNewDescriptor(BMUXlib_File_MP4_Handle hMP4Mux, BMUXlib_Input_Descriptor *pInDesc);
static void ProcessAudioMetadataDescriptor(const BMUXlib_File_MP4_P_Input *pInput, const BMUXlib_Input_Descriptor *pInDesc);
static void ProcessVideoMetadataDescriptor(const BMUXlib_File_MP4_P_Input *pInput, const BMUXlib_Input_Descriptor *pInDesc);

static bool IsOutputQueueClear(BMUXlib_File_MP4_Handle hMP4Mux);

/* callback handlers ... */
static void ProcessCompletedOutputDescriptors_BoxBuffer(BMUXlib_File_MP4_Handle hMP4Mux, const BMUXlib_Output_Descriptor *pOutDesc);
static void ProcessCompletedOutputDescriptors_SizeEntry(BMUXlib_File_MP4_P_OutputCallbackData *pCallbackData, const BMUXlib_Output_Descriptor *pOutDesc);
static void ProcessCompletedOutputDescriptors_Input(BMUXlib_File_MP4_P_OutputCallbackData *pCallbackData, const BMUXlib_Output_Descriptor *pOutDesc);
static void ProcessCompletedOutputDescriptors_MetadataCache(BMUXlib_File_MP4_P_MetadataCache *pMetadataCache, const BMUXlib_Output_Descriptor *pOutDesc);

static void ProcessBoxBufferReleaseQueue(BMUXlib_File_MP4_Handle hMP4Mux);
static BERR_Code ProcessInputReleaseQueues(BMUXlib_File_MP4_Handle hMP4Mux);

static void ReleaseQAddEntry(BMUXlib_File_MP4_Handle hMP4Mux, BMUXlib_File_MP4_P_ReleaseQ *pReleaseQ,
   uint8_t *pSourceData, uint32_t uiSourceLength, uint32_t uiSequenceID);

#if BDBG_DEBUG_BUILD
static void PrintFinalizationPercentComplete(BMUXlib_File_MP4_Handle hMP4Mux);
#endif

/**************************
    Static Definitions
**************************/
#define PrintVideoDescriptor(where, pDesc) \
   BDBG_MODULE_MSG(BMUX_MP4_IN_DESC, ("Video (%s): %d @ %d, pts:%lld (%d), dts:%lld (%d) %s%s%s%s%s 0x%2.2x", \
                  where, BMUXLIB_INPUT_DESCRIPTOR_LENGTH(pDesc), BMUXLIB_INPUT_DESCRIPTOR_OFFSET(pDesc), \
                  BMUXLIB_INPUT_DESCRIPTOR_PTS(pDesc), (BMUXLIB_INPUT_DESCRIPTOR_IS_PTS_VALID(pDesc))?1:0, \
                  BMUXLIB_INPUT_DESCRIPTOR_DTS(pDesc), (BMUXLIB_INPUT_DESCRIPTOR_IS_DTS_VALID(pDesc))?1:0, \
                  (BMUXLIB_INPUT_DESCRIPTOR_IS_FRAMESTART(pDesc))?"SOF ":"", \
                  (BMUXLIB_INPUT_DESCRIPTOR_IS_METADATA(pDesc))?"META ":"",      \
                  (BMUXLIB_INPUT_DESCRIPTOR_IS_EOS(pDesc))?"EOS ":"", \
                  (BMUXLIB_INPUT_DESCRIPTOR_IS_EMPTYFRAME(pDesc))?"EMPTY ":"", \
                  (BMUXLIB_INPUT_DESCRIPTOR_VIDEO_IS_DATA_UNIT_START(pDesc))?"DU ":"", BMUXLIB_INPUT_DESCRIPTOR_VIDEO_DATA_UNIT_TYPE(pDesc)));

#define PrintAudioDescriptor(where, pDesc) \
   BDBG_MODULE_MSG(BMUX_MP4_IN_DESC, ("Audio (%s): %d @ %d (raw:%d @ %d), pts:%lld (%d) %s%s%s%s", \
                  where, BMUXLIB_INPUT_DESCRIPTOR_LENGTH(pDesc), BMUXLIB_INPUT_DESCRIPTOR_OFFSET(pDesc), \
                  BMUXLIB_INPUT_DESCRIPTOR_AUDIO_RAWLENGTH(pDesc), BMUXLIB_INPUT_DESCRIPTOR_AUDIO_RAWOFFSET(pDesc), \
                  BMUXLIB_INPUT_DESCRIPTOR_PTS(pDesc), BMUXLIB_INPUT_DESCRIPTOR_IS_PTS_VALID(pDesc)?1:0, \
                  BMUXLIB_INPUT_DESCRIPTOR_IS_FRAMESTART(pDesc)?"SOF ":"", \
                  BMUXLIB_INPUT_DESCRIPTOR_IS_METADATA(pDesc)?"META ":"", \
                  BMUXLIB_INPUT_DESCRIPTOR_IS_EMPTYFRAME(pDesc)?"EMPTY ":"", \
                  BMUXLIB_INPUT_DESCRIPTOR_IS_EOS(pDesc)?"EOS ":""));

#if BDBG_DEBUG_BUILD
static const char *DebugTrackTypeTable[] =
{
   "Unknown",        /* BMUXlib_File_MP4_P_TrackType_eUnknown */
   "Video",          /* BMUXlib_File_MP4_P_TrackType_eVideo */
   "Audio",          /* BMUXlib_File_MP4_P_TrackType_eAudio */
};

extern const char *DebugBlockedCauseTable[];
#endif

/*************************
* P R I V A T E   A P I  *
**************************/

/* Function:
      BMUXlib_File_MP4_P_InitializeContext

   Initialise the internal context state of the MP4 mux to default settings
   Used by both Create() and Stop() APIs

   Note: this will only initialise the parts of context that are NOT configured
   by a Create() call.
*/
void BMUXlib_File_MP4_P_InitializeContext(BMUXlib_File_MP4_Handle hMP4Mux)
{
   int i, j;
   BMUXlib_File_MP4_P_CreateData *pCreateData = &hMP4Mux->stCreate;

   BDBG_ENTER(BMUXlib_File_MP4_P_InitializeContext);

   /* clear out the created context (DO NOT overwrite Create Data) */
   BKNI_Memset(hMP4Mux, 0,  (size_t)pCreateData - (size_t)hMP4Mux);

   /* initialise required values (those not set by memset above) ... */
   hMP4Mux->eCurrentBox = BMUXlib_File_MP4_P_BoxType_eFtyp;          /* always start with the ftyp box */
   hMP4Mux->eBoxState = BMUXlib_File_MP4_P_BoxState_eCreateBox;      /* want to start by creating the ftyp box! */
   hMP4Mux->uiBoxIndex = 0;                                         /* box sequence index - always start with box 0 */
   hMP4Mux->eInputState = BMUXlib_File_MP4_P_InputState_eUnknown;

   /* initialise inputs ... */
   for (i = 0; i < BMUXLIB_FILE_MP4_P_MAX_ACTIVE_INPUTS; i++)
   {
      BMUXlib_File_MP4_P_Input *pInput = &hMP4Mux->aActiveInputs[i];
      pInput->hInput = NULL;
      pInput->uiInitialDTS = BMUXLIB_FILE_MP4_P_INVALID_DTS;
      pInput->bEOS = false;
      pInput->bDataProcessed = false;
      pInput->stReleaseQ.pHead = pInput->stReleaseQ.pTail = NULL;    /* release Q is empty */
      pInput->uiDescSeqCount = 0;
      pInput->uiReleaseSeqCount = 0;
#if BDBG_DEBUG_BUILD
      pInput->uiIndex = i;
#endif
   }
   hMP4Mux->pLeadInput = NULL;                                       /* no lead input detected yet (input with the lowest DTS) */

   /* NOTE: can't initialise outputs until Start() since the output storage interface is supplied in start settings, and the number
            of active tracks is not known until then also */

   for (i = 0; i < BMUXLIB_FILE_MP4_P_MAX_TRACKS; i++)
   {
      BMUXlib_File_MP4_P_TrackInfo *pTrack = &hMP4Mux->aTracks[i];
      /* initialise track metadata from allocated buffers ... */
      for (j = 0; j < BMUXlib_File_MP4_P_MetadataType_eMax; j++)
      {
         BMUXlib_File_MP4_P_MetadataInterface *pMetadata = &pTrack->aMetadata[j];
         uint32_t uiEntrySize = pCreateData->aMetadataCacheBuffer[j].uiEntrySize;
         uint8_t *pBufferBase = (uint8_t *)(pCreateData->aMetadataCacheBuffer[j].pBuffer);

         pMetadata->stCache.uiNumEntries = pCreateData->uiMetadataCacheEntryCount;
         pMetadata->stCache.uiEntrySize = uiEntrySize;
         pMetadata->stCache.pBuffer = pBufferBase + (i * pCreateData->uiMetadataCacheEntryCount * uiEntrySize);
         pMetadata->stCache.bFlushDone = true;     /* not time to flush the cache yet */
      }
   }

   /* initialise box buffer from allocated buffer (leaving reserved area for "unwrap" space) ... */
   hMP4Mux->stBoxBuffer.uiBufSize = pCreateData->uiBoxBufferSize - BMUXLIB_FILE_MP4_P_BOX_BUFFER_RESERVED;
   hMP4Mux->stBoxBuffer.pBase = hMP4Mux->stBoxBuffer.pRead = hMP4Mux->stBoxBuffer.pWrite = hMP4Mux->stBoxBuffer.pTempWrite = pCreateData->pBoxBuffer;
   hMP4Mux->stBoxBuffer.pEnd = pCreateData->pBoxBuffer + hMP4Mux->stBoxBuffer.uiBufSize;
   BDBG_MSG(("Box Buffer: pBase = %p, pEnd = %p, size = %d (usable: %d bytes)",
         hMP4Mux->stBoxBuffer.pBase, hMP4Mux->stBoxBuffer.pEnd, pCreateData->uiBoxBufferSize, hMP4Mux->stBoxBuffer.uiBufSize - 1));

   hMP4Mux->stBoxBufferReleaseQ.pHead = hMP4Mux->stBoxBufferReleaseQ.pTail = NULL;   /* release Q is empty */

   /* initialise the size storage (unused entries are indicated by saved size of zero) ... */
   hMP4Mux->stSizes.pData = pCreateData->pSizeBuffer;
   hMP4Mux->stSizes.uiNumEntries = pCreateData->uiSizeBufferEntryCount;
   for (i = 0; i < (int)(hMP4Mux->stSizes.uiNumEntries); i++)
      pCreateData->pSizeBuffer[i] = BMUXLIB_FILE_MP4_P_SIZE_ENTRY_UNUSED;
   hMP4Mux->stSizes.pCurrentEntry = hMP4Mux->stSizes.pData;

   /* initialise the relocation buffer ... */
   hMP4Mux->stRelocationBuffer.uiSize = pCreateData->uiRelocationBufferSize;
   hMP4Mux->stRelocationBuffer.pBase = pCreateData->pRelocationBuffer;
   hMP4Mux->stRelocationBuffer.uiBytesUsed = 0;

   /* setup output completion callbacks ... */
   hMP4Mux->aOutputCallbacks[BMUXlib_File_MP4_P_OutputCallback_eBoxBuffer].pCallbackData = NULL;
   hMP4Mux->aOutputCallbacks[BMUXlib_File_MP4_P_OutputCallback_eBoxBuffer].pCallback = (BMUXlib_Output_CallbackFunction)ProcessCompletedOutputDescriptors_BoxBuffer;
   hMP4Mux->aOutputCallbacks[BMUXlib_File_MP4_P_OutputCallback_eInput].pCallbackData = NULL;
   hMP4Mux->aOutputCallbacks[BMUXlib_File_MP4_P_OutputCallback_eInput].pCallback = (BMUXlib_Output_CallbackFunction)ProcessCompletedOutputDescriptors_Input;
   hMP4Mux->aOutputCallbacks[BMUXlib_File_MP4_P_OutputCallback_eMetadataCache].pCallbackData = NULL;
   hMP4Mux->aOutputCallbacks[BMUXlib_File_MP4_P_OutputCallback_eMetadataCache].pCallback = (BMUXlib_Output_CallbackFunction)ProcessCompletedOutputDescriptors_MetadataCache;
   hMP4Mux->aOutputCallbacks[BMUXlib_File_MP4_P_OutputCallback_eSizeBuffer].pCallbackData = NULL;
   hMP4Mux->aOutputCallbacks[BMUXlib_File_MP4_P_OutputCallback_eSizeBuffer].pCallback = (BMUXlib_Output_CallbackFunction)ProcessCompletedOutputDescriptors_SizeEntry;

   BKNI_Memset(pCreateData->pReleaseQFreeList, 0, pCreateData->uiReleaseQFreeCount * sizeof(BMUXlib_File_MP4_P_ReleaseQEntry));
   /* create a free list of release Q entries ... */
   for (i = 0; i < (int)pCreateData->uiReleaseQFreeCount-1; i++)
      pCreateData->pReleaseQFreeList[i].pNext = &pCreateData->pReleaseQFreeList[i+1];
   pCreateData->pReleaseQFreeList[i].pNext = NULL;
   hMP4Mux->pReleaseQFreeList = pCreateData->pReleaseQFreeList;

   BKNI_Memset(pCreateData->pOutputCBDataFreeList, 0, pCreateData->uiOutputCBDataFreeCount * sizeof(BMUXlib_File_MP4_P_OutputCallbackData));
   /* create a free list of callback data entries
      NOTE: when chained into the free list, the pData field is used as a "next" pointer */
   for (i = 0; i < (int)pCreateData->uiOutputCBDataFreeCount-1; i++)
      pCreateData->pOutputCBDataFreeList[i].pData = &pCreateData->pOutputCBDataFreeList[i+1];
   pCreateData->pOutputCBDataFreeList[i].pData = NULL;
   hMP4Mux->pOutputCBDataFreeList = pCreateData->pOutputCBDataFreeList;

   /* initialise default status ... */
   hMP4Mux->stStatus.uiNextExecutionTime = BMUXLIB_FILE_MP4_P_EXEC_INTERVAL;
   BMUXLIB_FILE_MP4_P_SET_MUX_STATE(hMP4Mux, BMUXlib_State_eStopped);       /* top-level state */
   hMP4Mux->stStatus.bBlockedOutput = false;                /* mux not blocked ... yet */
   hMP4Mux->stStatus.uiFinishStepsComplete = 0;
   hMP4Mux->stStatus.uiFinishStepsTotal = 1;                /* first step is mdat size update */
   BDBG_LEAVE(BMUXlib_File_MP4_P_InitializeContext);
}

/*
   Function:
      BMUXlib_File_MP4_P_Start

   Initialise any settings necessary before starting the Mux process,
   create any necessary auxiliary tracks based on user settings, and create
   the necessary temporary storage for the metadata.

   NOTE: typically, the only way this fails is due to storage error
   (unable to create the metadata storage)
*/
BERR_Code BMUXlib_File_MP4_P_Start(BMUXlib_File_MP4_Handle hMP4Mux)
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BMUXlib_File_MP4_P_Start);

   if (BERR_SUCCESS == rc)
   {
      /* initialise other tracks, as required ... */
      InitializeAuxiliaryTracks(hMP4Mux);

      /* create the necessary temporary storage and output interfaces for
         accumulating the metadata based upon the number of tracks */
      rc = CreateOutputs(hMP4Mux);
   }
   if (BERR_SUCCESS == rc)
      /* everything OK, so start the mux ... */
      hMP4Mux->eInputState = BMUXlib_File_MP4_P_InputState_eStartup;
   else
      /* else, cleanup anything partially created */
      BMUXlib_File_MP4_P_Stop(hMP4Mux);

   BDBG_LEAVE(BMUXlib_File_MP4_P_Start);
   return rc;
}

/*
   Function:
      BMUXlib_File_MP4_P_Stop

   Shut down all muxing and release resources in use (typically
   this involves destroying the temporary storage used for metadata
   etc.).

   Also return context to post-create state.

   NOTE: this can be called at any time if the user wishes
   to perform a "hard" stop (i.e. without calling Finish())
*/
void BMUXlib_File_MP4_P_Stop(BMUXlib_File_MP4_Handle hMP4Mux)
{
   uint32_t uiInputIndex;

   BDBG_ENTER(BMUXlib_File_MP4_P_Stop);

   /* release the outputs and temporary storage to the system */
   DestroyOutputs(hMP4Mux);

   /* destroy input group */
   if (NULL != hMP4Mux->hInputGroup)
   {
      BMUXlib_InputGroup_Destroy(hMP4Mux->hInputGroup);
      hMP4Mux->hInputGroup = NULL;
   }

   /* destroy active inputs */
   for (uiInputIndex = 0; uiInputIndex < BMUXLIB_FILE_MP4_P_MAX_ACTIVE_INPUTS; uiInputIndex++)
   {
      BMUXlib_File_MP4_P_Input *pInput = &hMP4Mux->aActiveInputs[uiInputIndex];
      if (NULL != pInput->hInput)
      {
         BMUXlib_Input_Destroy(pInput->hInput);
         pInput->hInput = NULL;
      }
   }

   BDBG_LEAVE(BMUXlib_File_MP4_P_Stop);
}

/* Functions:
      BMUXlib_File_MP4_P_GetVideoCodingType
      BMUXlib_File_MP4_P_GetAudioCodingType

   Fetch the coding type for a given input protocol.
   These can also be used as an "IsSuported" test for a protocol
   (return of "unknown" indicates unsupported protocol)
*/
BMUXlib_File_MP4_P_CodingType BMUXlib_File_MP4_P_GetVideoCodingType(BAVC_VideoCompressionStd eProtocol)
{
   switch (eProtocol)
   {
      case BAVC_VideoCompressionStd_eMPEG4Part2:
         return BMUXlib_File_MP4_P_CodingType_eMpeg4Video;

      case BAVC_VideoCompressionStd_eH264:
         return BMUXlib_File_MP4_P_CodingType_eAVC;

      default:
         /* all other protocols are currently not supported */
         break;
   }
   return BMUXlib_File_MP4_P_CodingType_eUnknown;
}

BMUXlib_File_MP4_P_CodingType BMUXlib_File_MP4_P_GetAudioCodingType(BAVC_AudioCompressionStd eProtocol)
{
   switch (eProtocol)
   {
      case BAVC_AudioCompressionStd_eAac:              /* AAC (ADTS) - same as eAacAdts */
      /*case BAVC_AudioCompressionStd_eAacLoas:*/      /* AAC LOAS */
      /*case BAVC_AudioCompressionStd_eAacPlus:*/      /* AAC Plus (HE/SBR) (LOAS) */
      case BAVC_AudioCompressionStd_eAacPlusAdts:      /* AAC Plus (HE/SBR) ADTS */
         return BMUXlib_File_MP4_P_CodingType_eMpeg4Audio;

      default:
         /* all other protocols are currently not supported */
         break;
   }
   return BMUXlib_File_MP4_P_CodingType_eUnknown;
}

/*
   Function:
      BMUXlib_File_MP4_P_ProcessOutputDescriptorsWaiting

   Processed descriptors that are queued up and waiting to be output to storage
   If a storage interface is busy, descriptors will remain waiting until the next iteration

   Notes:
   The current design uses a single descriptor queue for all the outgoing descriptors.
   Thus, if any single interface stops processing descriptors in a timely manner, this will
   eventually block processing completely.  If this becomes an issue separate queues could be
   maintained for each output, although this will not solve the ultimate problem, only delay it

   Returns
      Storage interface error code
   (This will only fail if an error occurs on the storage interface)
*/
BERR_Code BMUXlib_File_MP4_P_ProcessOutputDescriptorsWaiting(BMUXlib_File_MP4_Handle hMP4Mux)
{
   BERR_Code rc = BERR_SUCCESS;
   uint32_t uiOutputIndex;

   BDBG_ENTER(BMUXlib_File_MP4_P_ProcessOutputDescriptorsWaiting);
   hMP4Mux->stStatus.bBlockedOutput= false;

   for (uiOutputIndex = 0; (uiOutputIndex < BMUXLIB_FILE_MP4_P_MAX_ACTIVE_OUTPUTS) && (BERR_SUCCESS == rc); uiOutputIndex++)
   {
      if (NULL != hMP4Mux->aActiveOutputs[uiOutputIndex].hOutput)
      {
         rc = BMUXlib_Output_ProcessNewDescriptors(hMP4Mux->aActiveOutputs[uiOutputIndex].hOutput);
         /* if any output did not queue up all descriptors, then we are blocked ... */
         /* NOTE: it is questionable whether blocked due to lack of a resource that
            is output dependent should be treated as "blocked output" also
            (since processing the output will free the blocked resources)
            Perhaps we may need to include all these "blocked" criteria also */
         if (BMUXlib_Output_IsDescriptorPendingQueue(hMP4Mux->aActiveOutputs[uiOutputIndex].hOutput))
            hMP4Mux->stStatus.bBlockedOutput = true;
      }
   }

   BDBG_LEAVE(BMUXlib_File_MP4_P_ProcessOutputDescriptorsWaiting);
   return rc;
}

/*
   Function:
      BMUXlib_File_MP4_P_ProcessOutputDescriptorsCompleted

   Process completed descriptors returned by the storage interface.  Release
   all resources pointed to by these descriptors (such as box buffer, metadata cache
   input descriptors, etc) and then free the descriptors

   Returns:
      BERR_UNKNOWN -    bad source type detected in descriptor
      + Other errors from storage interface
      + Other errors from input interface(s)
*/
BERR_Code BMUXlib_File_MP4_P_ProcessOutputDescriptorsCompleted(BMUXlib_File_MP4_Handle hMP4Mux)
{
   BERR_Code rc = BERR_SUCCESS;
   uint32_t uiOutputIndex;

   BDBG_ENTER(BMUXlib_File_MP4_P_ProcessOutputDescriptorsCompleted);

   for (uiOutputIndex = 0; (uiOutputIndex < BMUXLIB_FILE_MP4_P_MAX_ACTIVE_OUTPUTS) && (BERR_SUCCESS == rc); uiOutputIndex++)
      if (NULL != hMP4Mux->aActiveOutputs[uiOutputIndex].hOutput)
         rc = BMUXlib_Output_ProcessCompletedDescriptors(hMP4Mux->aActiveOutputs[uiOutputIndex].hOutput);

   ProcessBoxBufferReleaseQueue(hMP4Mux);  /* release any box buffer space that is complete */
   rc = ProcessInputReleaseQueues(hMP4Mux);
   /* NOTE: metadata cache and size entries are freed in the callback (they have no ordering requirements) */

   BDBG_LEAVE(BMUXlib_File_MP4_P_ProcessOutputDescriptorsCompleted);
   return rc;
}

/*
   Function:
      BMUXlib_File_MP4_P_ProcessInputDescriptorsWaiting

   Process new input descriptors that are waiting

   Returns:
      Input interface error code
*/
BERR_Code BMUXlib_File_MP4_P_ProcessInputDescriptorsWaiting(BMUXlib_File_MP4_Handle hMP4Mux)
{
   BERR_Code rc = BERR_SUCCESS;
   uint32_t uiInputIndex;

   BDBG_ENTER(BMUXlib_File_MP4_P_ProcessInputDescriptorsWaiting);

   for (uiInputIndex = 0; (uiInputIndex < BMUXLIB_FILE_MP4_P_MAX_ACTIVE_INPUTS) && (BERR_SUCCESS == rc); uiInputIndex++)
   {
      if (NULL != hMP4Mux->aActiveInputs[uiInputIndex].hInput)
      {
         rc = BMUXlib_Input_ProcessNewDescriptors(hMP4Mux->aActiveInputs[uiInputIndex].hInput);
      }
   }

   BDBG_LEAVE(BMUXlib_File_MP4_P_ProcessInputDescriptorsWaiting);
   return rc;
}

/*
   Function:
      BMUXlib_File_MP4_P_ProcessInputDescriptors

   Process new data from encoder(s) and create output descriptor lists to describe the data
   for the mdat chunks, as well as record the metadata for each sample

   NOTES:
   Descriptors are always processed in DTS-order (the ESCR is not used here, although
   it is assumed that there is a constant relationship between DTS and ESCR).
   It is expected that video frames are delivered in DTS order from the encoder.

   EVERY input descriptor *must* have a corresponding output descriptor created. The output
   descriptor is the link that causes the input descriptor to be freed when the data is
   consumed.  If the data is not required, then the output descriptor length must be zero.
   This ensures the input descriptor gets freed correctly in the order they are provided

   The only exception to this is when input descriptors are consumed *prior* to processing
   descriptors into output descriptors, whereby input descriptors can be discarded by
   simply returning them to the input.  Once samples have been processed, this can no
   longer be done (i.e. can only be done in the startup state).
*/
BERR_Code BMUXlib_File_MP4_P_ProcessInputDescriptors(BMUXlib_File_MP4_Handle hMP4Mux)
{
   BERR_Code rc = BERR_SUCCESS;
   BMUXlib_File_MP4_P_CurrentSample *pCurrentSample = &hMP4Mux->stCurrentSample;
   BMUXlib_File_MP4_P_Input *pCurrentInput = pCurrentSample->pInput;
   BMUXlib_File_MP4_P_BlockedCause eBlocked = BMUXlib_File_MP4_P_BlockedCause_eNone;
   bool bDescAvail;

   BDBG_ENTER(BMUXlib_File_MP4_P_ProcessInputDescriptors);

   /* NOTE: the following will also indirectly process any consumed(completed) input descriptors */
   rc = BMUXlib_File_MP4_P_ProcessOutputDescriptorsCompleted(hMP4Mux);
   if (BERR_SUCCESS != rc)
   {
      BDBG_LEAVE(BMUXlib_File_MP4_P_ProcessInputDescriptors);
      return rc;
   }

   rc = BMUXlib_File_MP4_P_ProcessInputDescriptorsWaiting(hMP4Mux);
   if (BERR_SUCCESS != rc)
   {
      BDBG_LEAVE(BMUXlib_File_MP4_P_ProcessInputDescriptors);
      return rc;
   }

   BDBG_MODULE_MSG(BMUX_MP4_STATE, ("====Processing Input Descriptors===="));

   /* keep processing as long as there are input descriptors available, and resources to process them ... */
   while ((BMUXlib_File_MP4_P_BlockedCause_eNone == eBlocked) && (BERR_SUCCESS == rc))
   {
#if BDBG_DEBUG_BUILD
      if (NULL != pCurrentInput)
      {
         /* check that the input for the current sample is valid (should always be so) */
         BDBG_ASSERT(NULL != pCurrentInput->hInput);
         BDBG_MODULE_MSG(BMUX_MP4_STATE, ("Current Input: %d, State = %d", pCurrentInput->uiIndex, hMP4Mux->eInputState));
      }
#endif
      switch (hMP4Mux->eInputState)
      {

         /* Find the SOF (start of frame) on each stream, and process any initial metadata ...
            NOTE: This is the only state in here that is permitted to consume input descriptors directly */
         case BMUXlib_File_MP4_P_InputState_eStartup:
         {
            BMUXlib_Input_Descriptor stInDesc, *pInDesc = &stInDesc;
            uint32_t uiInputIndex;
            bool bSOF = true;

            BDBG_MODULE_MSG(BMUX_MP4_STATE, ("Startup: Synchronising inputs to SOF, processing initial metadata ..."));

            /* iterate over the inputs, and for each one, ensure that we have an SOF.  At the same time, discard any
               descriptors not required, latch initial DTS, and process any metadata descriptors seen */
            for (uiInputIndex = 0; uiInputIndex < BMUXLIB_FILE_MP4_P_MAX_ACTIVE_INPUTS; uiInputIndex++)
            {
               BMUXlib_File_MP4_P_Input *pInput = &hMP4Mux->aActiveInputs[uiInputIndex];
               uint32_t uiDescProcessed = 0;

               if (NULL == pInput->hInput)
                  /* this input is not active, so skip it ... */
                  continue;

               /* if SOF already found on this input, we're done
                  (if initial DTS has been seen, then we have the SOF for this input) */
               if (BMUXLIB_FILE_MP4_P_INVALID_DTS != pInput->uiInitialDTS)
                  continue;                        /* check the next input */

               while (BMUXlib_Input_IsDescriptorAvailable(pInput->hInput))
               {
                  bDescAvail = BMUXlib_Input_PeekAtNextDescriptor(pInput->hInput, pInDesc);
                  BDBG_ASSERT(bDescAvail == true);

                  /* unable to process any data if base address is not set ... this is catastrophic failure */
                  BDBG_ASSERT(NULL != pInDesc->pBaseAddress);

                  if (BMUXLIB_INPUT_DESCRIPTOR_IS_FRAMESTART(pInDesc))
                     /* SOF found! */
                     break;

                  /* descriptor available, and is not SOF ... */
                  switch (pInput->pTrack->eType)
                  {
                     case BMUXlib_File_MP4_P_TrackType_eVideo:
                        PrintVideoDescriptor("Startup", pInDesc);
                        if (BMUXLIB_INPUT_DESCRIPTOR_IS_METADATA(pInDesc))
                           ProcessVideoMetadataDescriptor(pInput, pInDesc);
                        break;
                     case BMUXlib_File_MP4_P_TrackType_eAudio:
                        PrintAudioDescriptor("Startup", pInDesc);
                        if (BMUXLIB_INPUT_DESCRIPTOR_IS_METADATA(pInDesc))
                           ProcessAudioMetadataDescriptor(pInput, pInDesc);
                        break;
                     default:
                        /* do nothing, simply ignore this descriptor */
                        break;
                  }

                  /* skip all other descriptors that do not have SOF */
                  bDescAvail = BMUXlib_Input_GetNextDescriptor(pInput->hInput, pInDesc);
                  BDBG_ASSERT(bDescAvail == true);

                  uiDescProcessed++;                    /* return this descriptor to the input */
               } /* end: while descriptors available */

               if (!BMUXlib_Input_IsDescriptorAvailable(pInput->hInput))
               {
                  /* we ran out of decriptors, so no SOF ... */
                  bSOF = false;
               }
               else
               {
                  BDBG_ASSERT(BMUXLIB_INPUT_DESCRIPTOR_IS_FRAMESTART(pInDesc));

                  /* if PTS not valid, we have no DTS or PTS on this input - can't establish lowest DTS
                     (PTS interpolation is not supported) */
                  BDBG_ASSERT(BMUXLIB_INPUT_DESCRIPTOR_IS_PTS_VALID(pInDesc));
                  /* save the first DTS seen for this input ...
                     This used to determine initial offset for edit list */
                  if (BMUXLIB_FILE_MP4_P_INVALID_DTS == pInput->uiInitialDTS)
                  {
                     pInput->uiInitialDTS = BMUXLIB_INPUT_DESCRIPTOR_DTS(pInDesc);
                     BDBG_MSG(("SOF found on input %d: Initial DTS: %llx", uiInputIndex, pInput->uiInitialDTS));
                  }
               }

               if (0 != uiDescProcessed)
               {
                  rc = BMUXlib_Input_ConsumeDescriptors(pInput->hInput, uiDescProcessed);
               }
            } /* end: for each input */

            if (true == bSOF)
            {
               /* we have SOF on all inputs */
               hMP4Mux->eInputState = BMUXlib_File_MP4_P_InputState_eFindNewSample;
            }
            else
            {
               /* SOF was not found on all inputs - at least one input has no descriptors */
               eBlocked = BMUXlib_File_MP4_P_BlockedCause_eInput;
            }
            break;
         } /* end: state: startup */

         /* locate the new sample to process (the input with the lowest DTS).
            NOTE: This state MUST NOT directly consume any of the input descriptors once output
                  descriptors have been generated, otherwise input descriptors could be freed
                  out-of-order.
                  This also assumes that the first descriptor in each input queue has a SOF marker!
                  (otherwise, we cannot guarantee that DTS is valid/available) */
         case BMUXlib_File_MP4_P_InputState_eFindNewSample:
         {
            BMUXlib_Input_Handle hSelectedInput;

            /* locate the new sample to process (the input with the lowest DTS) */
            BDBG_MODULE_MSG(BMUX_MP4_STATE, ("Finding next sample to process ..."));
            /* determine the next input to process, based on which has the lowest DTS */
            BMUXlib_InputGroup_GetNextInput(hMP4Mux->hInputGroup, &hSelectedInput);
            if (NULL == hSelectedInput)
            {
               /* no more data to process */
               eBlocked = BMUXlib_File_MP4_P_BlockedCause_eInput;
               break;
            }
            else
            {
               /* found the next input to process ... */
               BMUXlib_Input_CreateSettings stSettings;

               /* fetch the metadata for the current input - this is the pointer to the input to process */
               BMUXlib_Input_GetCreateSettings(hSelectedInput, &stSettings);
               BDBG_ASSERT(stSettings.pMetadata != NULL);
               pCurrentInput = pCurrentSample->pInput = stSettings.pMetadata; /* save the input this sample is from */
               BDBG_MODULE_MSG(BMUX_MP4_STATE, ("Lowest DTS found on input: %d", pCurrentInput->uiIndex));

               /* peek at the first descriptor for this input */
               bDescAvail = BMUXlib_Input_PeekAtNextDescriptor(pCurrentInput->hInput, &pCurrentSample->stInDesc);
               /* NOTE: we know there is a descriptor available here */
               BDBG_ASSERT(bDescAvail == true);

               /* if its an empty frame descriptor, discard it and look for a new input to process */
               if (BMUXLIB_INPUT_DESCRIPTOR_IS_EMPTYFRAME(&pCurrentSample->stInDesc))
               {
                  BMUXlib_Input_Descriptor *pInDesc = &pCurrentSample->stInDesc;
                  /* this simply discards the descriptor that was peeked at previously */
                  bDescAvail = BMUXlib_Input_GetNextDescriptor(pCurrentInput->hInput, pInDesc);
                  BDBG_ASSERT(bDescAvail == true); /* there must always be a descriptor to "get" */
                  BDBG_MODULE_MSG(BMUX_MP4_IN_DESC, ("Discarding 'Empty Frame' descriptor from input %d", pCurrentInput->uiIndex));
                  /* add the descriptor to the release Q for the current input, with the current sequence count */
                  ReleaseQAddEntry(hMP4Mux, &pCurrentInput->stReleaseQ, NULL, 0, pCurrentInput->uiDescSeqCount);
                  pCurrentInput->uiWaitingCount++;
                  pCurrentInput->uiDescSeqCount++;
                  break;   /* stay in this state */
               }

               /* latch the input index of the "lead" input (the input that has the lowest
                  initial DTS value) - used for initial offset calcs */
               if (NULL == hMP4Mux->pLeadInput)
               {
                  BDBG_MSG(("Lead input = %d", pCurrentInput->uiIndex));
                  hMP4Mux->pLeadInput =  pCurrentInput;
               }

               pCurrentInput->bDataProcessed = true;
               pCurrentSample->bComplete = false;

               /* clear the metadata for the current sample - this ensures that the last sample
                  in the presentation will have deltaDTS of zero */
               BKNI_Memset(&pCurrentSample->stMetadata, 0, sizeof(BMUXlib_File_MP4_P_Metadata));

               /* initialize the processing state for this sample (input-type dependent) such that it will
                  begin with "sample start" (this will correctly initialize the audio state also) */
               pCurrentSample->eState.Video = 0;

               /* process the descriptors from the new sample next ...*/
               hMP4Mux->eInputState = BMUXlib_File_MP4_P_InputState_eProcessDescriptors;
            }
            break;
         }

         case BMUXlib_File_MP4_P_InputState_eProcessDescriptors:
         {
            /* process the currently chosen input until end of sample (i.e. start of next sample) detected */
            /* NOTE: pCurrentInput is always non-NULL here since it is allocated in the state
               that precedes this one - its the only way to get to this state */
            /* coverity[var_deref_op] */
            switch (pCurrentInput->pTrack->eType)
            {
               /* these will process the available descriptors until they run out, or until blocked by other resource requirement */
               case BMUXlib_File_MP4_P_TrackType_eVideo:
                  rc = ProcessVideoDescriptors(hMP4Mux, &eBlocked);
                  break;

               case BMUXlib_File_MP4_P_TrackType_eAudio:
                  rc = ProcessAudioDescriptors(hMP4Mux, &eBlocked);
                  break;

               default:
                  /* unknown/unexpected input type - should not happen ... a bad input
                     should never get selected as the active input */
                  BDBG_ERR(("Unknown or Unexpected Input Type: %d", pCurrentInput->pTrack->eType));
                  rc = BERR_TRACE(BERR_UNKNOWN);
                  break;
            } /* end: switch input type */
            if (BERR_SUCCESS != rc)
               break;                        /* error processing descriptors for this input - quit processing */

            /* if EOS was seen then complete the current sample (it is the last for this input).  */
            if (pCurrentSample->pInput->bEOS)
            {
               pCurrentSample->bComplete = true;
               BDBG_MODULE_MSG(BMUX_MP4_FINISH, ("EOS Detected on input %d", pCurrentSample->pInput->uiIndex));
               /* deactivate this input after EOS, so it will no longer be considered for selection */
               rc = BMUXlib_InputGroup_ActivateInput(hMP4Mux->hInputGroup, pCurrentSample->pInput->hInput, false);
               BDBG_ASSERT(BERR_SUCCESS == rc); /* this should always succeed */
            }

            /* if current sample was completed, then process metadata and then find a new sample to process ...
               (note: this metadata is input-type independent) */
            if (pCurrentSample->bComplete)
            {
               BMUXlib_File_MP4_P_StoreMetadataToCache(hMP4Mux, &pCurrentSample->stMetadata);
               pCurrentInput->pTrack->uiTotalBytes += pCurrentSample->stMetadata.uiSampleSize;
               pCurrentInput->pTrack->uiDuration90kHz += pCurrentSample->uiDTSDelta90kHz;               /* track duration in 90kHz timescale */

               BDBG_MSG(("*** %s Sample [%d]: %d bytes, PTS: %lld, DTS: %lld", DebugTrackTypeTable[pCurrentInput->pTrack->eType],
                           pCurrentInput->pTrack->uiSampleCount,
                           pCurrentSample->stMetadata.uiSampleSize,
                           pCurrentSample->uiDTS + pCurrentSample->stMetadata.uiCTSDTSDiff, pCurrentSample->uiDTS));

               hMP4Mux->eInputState = BMUXlib_File_MP4_P_InputState_eFindNewSample;
               break;
            }

            /* else, current sample is not complete, so come back here next time */
            break;
         }

         default:
            /* Unknown state! should not happen ... typically indicates memory overwrite
               this is also a catch-all if it somehow gets to "started" state without invoking Start() */
            BDBG_ERR(("ProcessInputDescriptors:: Unexpected State: %d", hMP4Mux->eInputState));
            rc = BERR_TRACE(BERR_NOT_SUPPORTED);
            break;
      } /* end: switch input state */
   } /* end: while not blocked, not done && not error */

   if (BMUXlib_File_MP4_P_BlockedCause_eNone != eBlocked)
      BDBG_MODULE_MSG(BMUX_MP4_STATE, ("Process Input: Blocked: %s", DebugBlockedCauseTable[eBlocked]));

   if (BERR_SUCCESS == rc)
      rc = BMUXlib_File_MP4_P_ProcessOutputDescriptorsWaiting(hMP4Mux);

   BDBG_LEAVE(BMUXlib_File_MP4_P_ProcessInputDescriptors);
   return rc;
}

/*
   Function:
      BMUXlib_File_MP4_P_IsInputProcessingDone

   Predicate to determine if Input processing has completed

   This is only called after Finish() has been invoked.  Thus, this
   will define "done" as all input queues empty, all descriptors returned
   to encoder(s) and all input processing is complete.
*/
bool BMUXlib_File_MP4_P_IsInputProcessingDone(BMUXlib_File_MP4_Handle hMP4Mux)
{
   bool bDone = false;
   uint32_t uiInputIndex;

   BDBG_ENTER(BMUXlib_File_MP4_P_IsInputProcessingDone);

   for (uiInputIndex = 0; uiInputIndex < BMUXLIB_FILE_MP4_P_MAX_ACTIVE_INPUTS; uiInputIndex++)
   {
      BMUXlib_File_MP4_P_Input *pInput = &hMP4Mux->aActiveInputs[uiInputIndex];

      /* if this input did not process any data, consider it done
         (if no data was ever processed by this input, it is unlikely we will see an EOS)
         Note that invalid inputs will never process any data */
      if (false == pInput->bDataProcessed)
      {
         BDBG_MODULE_MSG(BMUX_MP4_FINISH, ("Skipping input %d... no data was processed", uiInputIndex));
         continue;
      }

      if (BMUXlib_Input_IsDescriptorAvailable(pInput->hInput) && (false == pInput->bEOS))
      {
         BDBG_MODULE_MSG(BMUX_MP4_FINISH, ("Waiting for Input %d", uiInputIndex));
         break;
      }

      /* ensure that there are no waiting storage descriptors ...
         NOTE: This also ensures that the input release queues are clear */
      if (0 != pInput->uiWaitingCount)
      {
         BDBG_MODULE_MSG(BMUX_MP4_FINISH, ("Waiting for Input %d (%d descriptors waiting to be released)", uiInputIndex, pInput->uiWaitingCount));
         break;
      }

      /* ensure EOS seen on this input ... */
      if (false == pInput->bEOS)
      {
         BDBG_MODULE_MSG(BMUX_MP4_FINISH, ("Waiting for EOS on Input %d", uiInputIndex));
         break;
      }
   }

   /* NOTE: it is not necessary to check for "leaked" dependent data from the
      storage descriptors, since anything left behind will be cleaned up when a
      Stop() is issued, or by a subsequent Destroy() */
   if (uiInputIndex == BMUXLIB_FILE_MP4_P_MAX_ACTIVE_INPUTS)
   {
      /* all inputs satisfy the criteria for "done" */

      /* since all input processing is done, no more metadata will be collected,
         so flush all metadata caches - this necessary, since the next state needs
         to be able to read the metadata to process it into the moov */
      if (!hMP4Mux->bMetadataFinalized)
      {
         /* Prior to flushing the caches - this must complete the metadata cache entries
            for metadata that is run-length coded (stts and ctts). */
         BDBG_MODULE_MSG(BMUX_MP4_FINISH, ("Finalizing Metadata"));
         BMUXlib_File_MP4_P_FinalizeMetadata(hMP4Mux);
         hMP4Mux->bMetadataFinalized = true;    /* don't do it more than once!! */
      }
      BDBG_MODULE_MSG(BMUX_MP4_FINISH, ("Flushing Metadata"));
      bDone = BMUXlib_File_MP4_P_FlushAllMetadata(hMP4Mux);
   }

   if (bDone)
   {
      /* if we're done, then move on to updating mdat and creating moov ... */
      hMP4Mux->eBoxState = BMUXlib_File_MP4_P_BoxState_eFinalizeMdat;
       /* indicate to app to run mux ASAP to reduce delay finalizing the file ... */
      hMP4Mux->stStatus.uiNextExecutionTime = BMUXLIB_FILE_MP4_P_EXEC_INTERVAL_NONE;
      BDBG_MODULE_MSG(BMUX_MP4_FINISH, ("Input Processing Done"));
   }

   BDBG_LEAVE(BMUXlib_File_MP4_P_IsInputProcessingDone);
   return bDone;
}

/*
   Function:
      BMUXlib_File_MP4_P_IsOutputProcessingDone

   Predicate to determine if Output processing has completed

   This is only called after input is complete (as defined above)
   This will determine "done" when all output sources have finalised
   output, all output descriptors have been returned and dependent
   data sources released
*/
bool BMUXlib_File_MP4_P_IsOutputProcessingDone(BMUXlib_File_MP4_Handle hMP4Mux)
{
   bool bDone = false;
   BDBG_ENTER(BMUXlib_File_MP4_P_IsOutputProcessingDone);
#if BDBG_DEBUG_BUILD
   PrintFinalizationPercentComplete(hMP4Mux);
#endif
   /* ensure all output-generating code is finished ... */
   if (hMP4Mux->eBoxState == BMUXlib_File_MP4_P_BoxState_eDone)
   {
      /* ensure all output descriptors have been returned and all
         dependent data sources have been released ... */
      bDone = IsOutputQueueClear(hMP4Mux);
      /* all descriptors must be free when done */
   }
   if (bDone)
   {
      BDBG_MODULE_MSG(BMUX_MP4_FINISH, ("Output Done - %lld bytes written", (hMP4Mux->bMoovAtStart)?
                     (hMP4Mux->uiNewMdatOffset+BMUXlib_Output_GetEndOffset(hMP4Mux->pMdatOutput->hOutput)):
                     (BMUXlib_Output_GetEndOffset(hMP4Mux->pMdatOutput->hOutput))));
   }
   BDBG_LEAVE(BMUXlib_File_MP4_P_IsOutputProcessingDone);
   return bDone;
}

/* find a free entry in the size storage.
   Performs a simple linear search looking for an unused entry
   Since entries are typically freed as they are used, the free entries will
   be near the start of the buffer unless all entries are used
   TODO: find a more efficient implementation? */
uint32_t *BMUXlib_File_MP4_P_FindFreeSizeEntry(BMUXlib_File_MP4_P_SizeStore *pSizes)
{
   uint32_t *pData = pSizes->pData;
   uint32_t uiIndex;

   BDBG_LEAVE(BMUXlib_File_MP4_P_FindFreeSizeEntry);

   for (uiIndex = 0; (uiIndex < pSizes->uiNumEntries) && (*pData != BMUXLIB_FILE_MP4_P_SIZE_ENTRY_UNUSED); uiIndex++, pData++);
   /* save the selected entry for later use ...
      (if a free entry not found, pointer is NULL) */
   pSizes->pCurrentEntry = (uiIndex == pSizes->uiNumEntries)?NULL:pData;

#if BDBG_DEBUG_BUILD
   if (uiIndex > pSizes->uiMaxSearchDepth)
      pSizes->uiMaxSearchDepth = uiIndex;
   if (NULL != pSizes->pCurrentEntry)
      pSizes->uiUsageCount++;
   if (pSizes->uiUsageCount > pSizes->uiMaxUsage)
      pSizes->uiMaxUsage = pSizes->uiUsageCount;
#endif

   BDBG_LEAVE(BMUXlib_File_MP4_P_FindFreeSizeEntry);
   return pSizes->pCurrentEntry;
}

/* append the specified data to the specified output and
   register the specified "complete" callback */
void BMUXlib_File_MP4_P_OutputDescriptorAppend(BMUXlib_File_MP4_Handle hMP4Mux, BMUXlib_File_MP4_P_Output *pOutput, void *pAddress, uint32_t uiLength, BMUXlib_File_MP4_P_OutputCallback eCallback)
{
   BERR_Code rc;
   BMUXlib_Output_Descriptor stOutDesc;
   BMUXlib_Output_CompletedCallbackInfo *pCallback;

   /* clear output descriptor to ensure any unused fields are cleared */
   BKNI_Memset(&stOutDesc, 0, sizeof(BMUXlib_Output_Descriptor));

   stOutDesc.stStorage.bWriteOperation = true;     /* appends are always write operations */
   stOutDesc.stStorage.pBufferAddress = pAddress;
   stOutDesc.stStorage.uiLength = uiLength;
   stOutDesc.stStorage.uiOffset = 0;               /* append data to end of output */
   stOutDesc.stStorage.eOffsetFrom = BMUXlib_Output_OffsetReference_eEnd;

   pCallback = (eCallback == BMUXlib_File_MP4_P_OutputCallback_eNone)?NULL:(&hMP4Mux->aOutputCallbacks[eCallback]);

   /* NOTE: the callback info is copied, so we can use a common location for callback creation */
   rc = BMUXlib_Output_AddNewDescriptor(pOutput->hOutput, &stOutDesc, pCallback);
   BDBG_ASSERT(BERR_SUCCESS == rc); /* should not happen, since check for space is done in advance */

   BDBG_MODULE_MSG(BMUX_MP4_OUT_DESC, ("%-16.16s [W]: mem:%8.8x (%6.6d bytes)   to out:%8.8x @ off:%lld",
                  pOutput->pDesc, pAddress, uiLength, pOutput, BMUXlib_Output_GetCurrentOffset(pOutput->hOutput) - uiLength));
}

/* Update the specified output with the data, at the requested location
   register the specified "completion" callback */
void BMUXlib_File_MP4_P_OutputDescriptorUpdate(BMUXlib_File_MP4_Handle hMP4Mux, BMUXlib_File_MP4_P_Output *pOutput, void *pAddress, uint32_t uiLength, uint64_t uiOffset, BMUXlib_File_MP4_P_OutputCallback eCallback)
{
   BERR_Code rc;
   BMUXlib_Output_Descriptor stOutDesc;
   BMUXlib_Output_CompletedCallbackInfo *pCallback;

   /* clear output descriptor to ensure any unused fields are cleared */
   BKNI_Memset(&stOutDesc, 0, sizeof(BMUXlib_Output_Descriptor));

   stOutDesc.stStorage.bWriteOperation = true;     /* updates are always write operatons */
   stOutDesc.stStorage.pBufferAddress = pAddress;
   stOutDesc.stStorage.uiLength = uiLength;
   stOutDesc.stStorage.uiOffset = uiOffset;        /* offset to update */
   stOutDesc.stStorage.eOffsetFrom = BMUXlib_Output_OffsetReference_eStart;   /* for update, always reference from start of output */

   pCallback = (eCallback == BMUXlib_File_MP4_P_OutputCallback_eNone)?NULL:(&hMP4Mux->aOutputCallbacks[eCallback]);

   rc = BMUXlib_Output_AddNewDescriptor(pOutput->hOutput, &stOutDesc, pCallback);
   BDBG_ASSERT(BERR_SUCCESS == rc); /* should not happen, since check for space is done in advance */

   BDBG_MODULE_MSG(BMUX_MP4_OUT_DESC, ("%-16.16s [W]: mem:%8.8x (%6.6d bytes)   to out:%8.8x @ off:%lld",
                  pOutput->pDesc, pAddress, uiLength, pOutput, uiOffset));
}

/* Set descriptor to read from the current location
   Register the specified "completion" callback */
void BMUXlib_File_MP4_P_OutputDescriptorRead(BMUXlib_File_MP4_Handle hMP4Mux, BMUXlib_File_MP4_P_Output *pOutput, void *pAddress, uint32_t uiLength, BMUXlib_File_MP4_P_OutputCallback eCallback)
{
   BERR_Code rc;
   BMUXlib_Output_Descriptor stOutDesc;
   BMUXlib_Output_CompletedCallbackInfo *pCallback;

   /* clear output descriptor to ensure any unused fields are cleared */
   BKNI_Memset(&stOutDesc, 0, sizeof(BMUXlib_Output_Descriptor));

   stOutDesc.stStorage.bWriteOperation = false;
   stOutDesc.stStorage.pBufferAddress = pAddress;
   stOutDesc.stStorage.uiLength = uiLength;
   stOutDesc.stStorage.uiOffset = 0;
   stOutDesc.stStorage.eOffsetFrom = BMUXlib_Output_OffsetReference_eCurrent;

   pCallback = (eCallback == BMUXlib_File_MP4_P_OutputCallback_eNone)?NULL:(&hMP4Mux->aOutputCallbacks[eCallback]);

   rc = BMUXlib_Output_AddNewDescriptor(pOutput->hOutput, &stOutDesc, pCallback);
   BDBG_ASSERT(BERR_SUCCESS == rc); /* should not happen, since check for space is done in advance */

   BDBG_MODULE_MSG(BMUX_MP4_OUT_DESC, ("%-16.16s [R]: mem:%8.8x (%6.6d bytes) from out:%8.8x @ off:%lld",
                  pOutput->pDesc, pAddress, uiLength, pOutput, BMUXlib_Output_GetCurrentOffset(pOutput->hOutput) - uiLength));
}

BMUXlib_File_MP4_P_OutputCallbackData *BMUXlib_File_MP4_P_NewOutputCallbackData(BMUXlib_File_MP4_Handle hMP4Mux, void *pData, uint32_t uiSeqCount)
{
   BMUXlib_File_MP4_P_OutputCallbackData *pCallbackData;

   /* fetch a callback entry from the free list */
   pCallbackData = hMP4Mux->pOutputCBDataFreeList;
   if (NULL != pCallbackData)
   {
      hMP4Mux->pOutputCBDataFreeList = pCallbackData->pData;

      pCallbackData->hMux = hMP4Mux;
      pCallbackData->pData = pData;
      pCallbackData->uiSequenceID = uiSeqCount;
#if BDBG_DEBUG_BUILD
      hMP4Mux->uiCallbackDataUsageCount++;
      if (hMP4Mux->uiCallbackDataUsageCount > hMP4Mux->uiCallbackDataMaxUsage)
         hMP4Mux->uiCallbackDataMaxUsage = hMP4Mux->uiCallbackDataUsageCount;
#endif
   }
   return pCallbackData;
}

void BMUXlib_File_MP4_P_FreeOutputCallbackData(BMUXlib_File_MP4_Handle hMP4Mux, BMUXlib_File_MP4_P_OutputCallbackData *pCallbackData)
{
   /* return the callback data entry to the free list */
   pCallbackData->pData = hMP4Mux->pOutputCBDataFreeList;
   hMP4Mux->pOutputCBDataFreeList = pCallbackData;
#if BDBG_DEBUG_BUILD
   hMP4Mux->uiCallbackDataUsageCount--;
#endif
}

/***********************************
* S T A T I C   F U N C T I O N S  *
***********************************/

/*******************
   Initialization
********************/
/*
   Initialize any tracks that are required, but are not directly specified as inputs
   (such as H.264 parameter set tracks, ODS and SDS tracks, etc)
*/
static void InitializeAuxiliaryTracks(BMUXlib_File_MP4_Handle hMP4Mux)
{
   BSTD_UNUSED(hMP4Mux);
   BDBG_MSG(("Creating Auxiliary Tracks ..."));
   /* For now, this does nothing ... not sure if we need other tracks as yet */
}

/*
   Create the necessary output interfaces and temporary storage for accumulating the
   metadata during mdat creation, based on the number of tracks in use, and for
   storage of the mdat itself if required by the bMoovAtStart setting (if set,
   mdat needs to go to temporary storage)

   The following metadata is collected (per sample) for each track for use in creating the
   stbl and the boxes within it:
   * stts: sample-to-sample delta (i.e. DTS to DTS) - 32 bits
   * ctts: PTS-DTS diff - 32 bits  (if required)
   * stsz: size in bytes of each "sample" (frame) - 32 bits
   * stss: Random access indicator - bool
   * stco: FileOffset - 64 bits
*/
static BERR_Code CreateOutputs(BMUXlib_File_MP4_Handle hMP4Mux)
{
   BERR_Code rc = BERR_SUCCESS;
   uint32_t uiTrackIndex, uiOutputIndex;
   uint32_t i;
   BMUXlib_StorageSystemInterface *pStorage = &hMP4Mux->stStorage;
   BMUXlib_StorageSettings stStorageSettings;
   BMUXlib_Output_CreateSettings stOutputSettings;
   BMUXlib_File_MP4_P_CreateData *pCreateData = &hMP4Mux->stCreate;

   BDBG_ENTER(CreateOutputs);

   BDBG_MODULE_MSG(BMUX_MP4_OUTPUT, ("Creating Outputs/Storage ..."));

   /* NOTE: since each usage of these settings sets the same fields, we can simply fetch these once */
   pStorage->pfGetDefaultStorageSettings(&stStorageSettings);
   BMUXlib_Output_GetDefaultCreateSettings(&stOutputSettings);

   /* create the output interface for the main output ... */
   stOutputSettings.uiCount = pCreateData->uiOutDescCount;
   stOutputSettings.stStorage = hMP4Mux->aActiveOutputs[BMUXLIB_FILE_MP4_P_OUTPUT_MAIN].stInterface;
   stOutputSettings.uiOutputID = BMUXLIB_FILE_MP4_P_OUTPUT_MAIN;
   rc = BMUXlib_Output_Create(&hMP4Mux->aActiveOutputs[BMUXLIB_FILE_MP4_P_OUTPUT_MAIN].hOutput, &stOutputSettings);
   if (BERR_SUCCESS != rc)
   {
      BDBG_ERR(("Unable to create main output interface"));
      BDBG_LEAVE(CreateOutputs);
      return BERR_TRACE(rc);
   }

   /* open the required temporary storage for the mdat depending on creation mode */
   if (hMP4Mux->bMoovAtStart)
   {
      /* moov at start implies mdat is written to temporary file, moov created at end of mdat creation, then mdat moved to output */
      BMUXlib_File_MP4_P_Output *pOutput = &hMP4Mux->aActiveOutputs[BMUXLIB_FILE_MP4_P_OUTPUT_MDAT];
      /* estimate the required storage for the mdat based on duration supplied by the app and the bitrates of the active inputs */
      stStorageSettings.uiExpectedSizeBytes = 0; /*FIXME: not known as yet - bitrate unavailable */
      /* create the storage interface for this output ... */
      rc = BERR_TRACE(pStorage->pfCreateStorageObject(pStorage->pContext, &pOutput->stInterface, &stStorageSettings));
      if (BERR_SUCCESS != rc)
      {
         BDBG_ERR(("Unable to create mdat storage object"));
         BDBG_LEAVE(CreateOutputs);
         return rc;
      }
      /* create output interface for this storage */
      stOutputSettings.uiCount = pCreateData->uiOutDescCount;
      stOutputSettings.stStorage = pOutput->stInterface;
      stOutputSettings.uiOutputID = BMUXLIB_FILE_MP4_P_OUTPUT_MDAT;
      rc = BERR_TRACE(BMUXlib_Output_Create(&pOutput->hOutput, &stOutputSettings));
      if (BERR_SUCCESS != rc)
      {
         BDBG_ERR(("Unable to create mdat output interface"));
         BDBG_LEAVE(CreateOutputs);
         return rc;
      }
      hMP4Mux->pMdatOutput = pOutput;
   }
   else
   {
      /* moov at end implies mdat is not relocated, and can be written directly to the output */
      hMP4Mux->pMdatOutput = &hMP4Mux->aActiveOutputs[BMUXLIB_FILE_MP4_P_OUTPUT_MAIN];
      /* mdat counts as an "active" output even if its not used - it will be skipped later if required */
   }

   /* for each active track, open the required metadata storage locations */
   for (uiTrackIndex = 0, uiOutputIndex = BMUXLIB_FILE_MP4_P_METADATA_START; (uiTrackIndex < hMP4Mux->uiNumTracks) && (BERR_SUCCESS == rc); uiTrackIndex++)
   {
      BMUXlib_File_MP4_P_TrackInfo *pTrack = &hMP4Mux->aTracks[uiTrackIndex];
      uint32_t uiFrameRate = 30; /* FIXME: need the real frame rate for the input - what is it for audio??? */
      uint32_t uiExpectedSamples;

      BDBG_MODULE_MSG(BMUX_MP4_OUTPUT, ("Track ID %d: %p", pTrack->uiTrackID, pTrack));

      if (0 == hMP4Mux->uiExpectedDurationMs)
      {
         /* duration unknown, so we can't estimate sizes */
         uiExpectedSamples = 0;
      }
      else
      {
         uiExpectedSamples = ((hMP4Mux->uiExpectedDurationMs * uiFrameRate) / 1000) + 1;
      }

      /* expected size = expected duration in seconds * frame (sample) rate * size of metadata entry per sample */
      for (i = 0; i < BMUXlib_File_MP4_P_MetadataType_eMax; i++, uiOutputIndex++)
      {
         BMUXlib_File_MP4_P_MetadataInterface *pMetadata = &pTrack->aMetadata[i];
         pMetadata->pOutput = &hMP4Mux->aActiveOutputs[uiOutputIndex];
         stStorageSettings.uiExpectedSizeBytes = uiExpectedSamples * pCreateData->aMetadataCacheBuffer[i].uiEntrySize;
         rc = BERR_TRACE(pStorage->pfCreateStorageObject(pStorage->pContext, &pMetadata->pOutput->stInterface, &stStorageSettings));
         if (BERR_SUCCESS != rc)
         {
            /* unable to create the temporary storage required */
            BDBG_ERR(("Unable to create storage for metadata %d, track %d", i, pTrack->uiTrackID));
            break;
         }
         /* create output interface for this metadata storage */
         stOutputSettings.uiCount = BMUXLIB_FILE_MP4_P_NUM_OUT_DESC_METADATA;
         stOutputSettings.stStorage = pMetadata->pOutput->stInterface;
         stOutputSettings.uiOutputID = (uiTrackIndex * BMUXlib_File_MP4_P_MetadataType_eMax) + (BMUXLIB_FILE_MP4_P_METADATA_START + i);
         rc = BERR_TRACE(BMUXlib_Output_Create(&pMetadata->pOutput->hOutput, &stOutputSettings));
         if (BERR_SUCCESS != rc)
         {
            BDBG_ERR(("Unable to create output interface for metadata %d, track %d", i, pTrack->uiTrackID));
            break;
         }
      }
   } /* end: for each track */
   BDBG_ASSERT(uiOutputIndex < BMUXLIB_FILE_MP4_P_MAX_ACTIVE_OUTPUTS);

   BDBG_LEAVE(CreateOutputs);
   return rc;
}

/*
   Release any output interfaces and temporary storage that is being
   used for mdat creation and metadata storage
*/
static void DestroyOutputs(BMUXlib_File_MP4_Handle hMP4Mux)
{
   BMUXlib_StorageSystemInterface *pStorage = &hMP4Mux->stStorage;
   uint32_t uiTrackIndex, uiOutputIndex;

   BDBG_ENTER(DestroyOutputs);
   BDBG_MODULE_MSG(BMUX_MP4_OUTPUT, ("Destroying Metadata Storage ..."));

   /* if temp storage used, destroy it */
   if (hMP4Mux->pMdatOutput == &hMP4Mux->aActiveOutputs[BMUXLIB_FILE_MP4_P_OUTPUT_MDAT])
   {
      /* if this interface is active, destroy the storage */
      if ((NULL != hMP4Mux->pMdatOutput->stInterface.pfAddDescriptors)
         && (NULL != hMP4Mux->pMdatOutput->stInterface.pfGetCompleteDescriptors))
      {
         BDBG_MODULE_MSG(BMUX_MP4_OUTPUT, ("Destroying Mdat Temp Storage ... "));
         pStorage->pfDestroyStorageObject(pStorage->pContext, hMP4Mux->pMdatOutput->stInterface.pContext);
         BKNI_Memset(&hMP4Mux->pMdatOutput->stInterface, 0, sizeof(BMUXlib_StorageObjectInterface));
      }
      if (NULL != hMP4Mux->pMdatOutput->hOutput)
      {
         BDBG_MODULE_MSG(BMUX_MP4_OUTPUT, ("Destroying Mdat Output Interface ... "));
         BMUXlib_Output_Destroy(hMP4Mux->pMdatOutput->hOutput);
         hMP4Mux->pMdatOutput->hOutput = NULL;
      }
   }
   /* destroy each temp file used for metadata */
   for (uiTrackIndex = 0, uiOutputIndex = BMUXLIB_FILE_MP4_P_METADATA_START; uiTrackIndex < hMP4Mux->uiNumTracks; uiTrackIndex++)
   {
      BMUXlib_File_MP4_P_TrackInfo *pTrack = &hMP4Mux->aTracks[uiTrackIndex];
      uint32_t uiMetadataType;
      BDBG_MODULE_MSG(BMUX_MP4_OUTPUT, ("Destroying Metadata file(s) for track %d ... ", pTrack->uiTrackID));
      for (uiMetadataType = 0; uiMetadataType < BMUXlib_File_MP4_P_MetadataType_eMax; uiMetadataType++, uiOutputIndex++)
      {
         BMUXlib_File_MP4_P_MetadataInterface *pMetadata = &pTrack->aMetadata[uiMetadataType];
         if (NULL == pMetadata->pOutput)
            continue;      /* skip anything that didn't get initialized */
         if ((NULL != pMetadata->pOutput->stInterface.pfAddDescriptors)
            && (NULL != pMetadata->pOutput->stInterface.pfGetCompleteDescriptors))
         {
            pStorage->pfDestroyStorageObject(pStorage->pContext, pMetadata->pOutput->stInterface.pContext);
            BKNI_Memset(&pMetadata->pOutput->stInterface, 0, sizeof(BMUXlib_StorageObjectInterface));
         }
         if (NULL != pMetadata->pOutput->hOutput)
         {
            BMUXlib_Output_Destroy(pMetadata->pOutput->hOutput);
            pMetadata->pOutput->hOutput = NULL;
         }
      }
   }
   /* destroy the output interface for the main output ... */
   if (NULL != hMP4Mux->aActiveOutputs[BMUXLIB_FILE_MP4_P_OUTPUT_MAIN].hOutput)
   {
      BMUXlib_Output_Destroy(hMP4Mux->aActiveOutputs[BMUXLIB_FILE_MP4_P_OUTPUT_MAIN].hOutput);
      hMP4Mux->aActiveOutputs[BMUXLIB_FILE_MP4_P_OUTPUT_MAIN].hOutput = NULL;
   }
   BDBG_LEAVE(DestroyOutputs);
}

/*********************************
   Input Descriptor Processing
*********************************/

/* Initialise the metadata for the current sample, based on the new descriptor */
static void InitializeSampleMetadata(BMUXlib_File_MP4_P_CurrentSample *pCurrentSample , const BMUXlib_Input_Descriptor *pInDesc, const uint64_t uiOffset)
{
   BMUXlib_File_MP4_P_Metadata *pMetadata = &pCurrentSample->stMetadata;
   BMUXlib_File_MP4_P_Input *pCurrentInput = pCurrentSample->pInput;
   uint64_t uiPTS = BMUXLIB_INPUT_DESCRIPTOR_PTS(pInDesc);
   uint64_t uiResult;
   uint32_t uiCTSDTSDiff90kHz;
   uint64_t guiFullScaleMod33 = 1;
   guiFullScaleMod33 <<= 33;
   pCurrentSample->uiDTS = BMUXLIB_INPUT_DESCRIPTOR_DTS(pInDesc);  /* save this for calculation of deltaDTS at the end of the sample */

   uiResult = uiPTS - pCurrentSample->uiDTS;
   if ((int64_t)uiResult < 0)
      uiResult += guiFullScaleMod33;    /* convert to modulo-33-bits */

   if (/*uiCTSDTSDiff90kHz == 0 ||*/ uiResult > BMUXLIB_FILE_MP4_P_MODULO_33BITS_MID_RANGE)
   {
      BDBG_WRN(("Input %d: Negative CTS/DTS diff detected (%d)", pCurrentInput->uiIndex, uiResult));
      uiResult = BMUXLIB_FILE_MP4_P_MODULO_33BITS_MID_RANGE;   /* clip to max */
   }

   uiCTSDTSDiff90kHz = (uint32_t)uiResult;
   /* pre-initialize DTSdelta, so if EOS occurs, this will already contain the correct value */
   pCurrentSample->uiDTSDelta90kHz = pMetadata->uiDTSDelta = 0;

   /* Ensure that this descriptor represents the first descriptor of the sample */
   if (!BMUXLIB_INPUT_DESCRIPTOR_IS_FRAMESTART(pInDesc))
      BDBG_WRN(("Sample Start is missing SOF indicator (sample's metadata may be invalid)"));

   switch (pCurrentInput->pTrack->eType)
   {
      case BMUXlib_File_MP4_P_TrackType_eVideo:
      {
         /* NOTE: CTSDTSDiff must be in media timescale, not presentation timescale
            For video, it happens that the two timescales are the same */
         pMetadata->uiCTSDTSDiff = uiCTSDTSDiff90kHz;
         break;
      }
      case BMUXlib_File_MP4_P_TrackType_eAudio:
      {
         /* For audio, the DTS and PTS should always be equal, so this should be zero,
            and no CTTS box should be required - this calc is done incase this is not true */
         /* NOTE: CTSDTSDiff must be in media timescale, not presentation timescale... */
         uiResult = uiCTSDTSDiff90kHz * pCurrentInput->pTrack->uiTimescale;
         pMetadata->uiCTSDTSDiff = uiResult / BMUXLIB_FILE_MP4_P_TIMESCALE_90KHZ;
         break;
      }
      default:
      {
         BDBG_WRN(("InitializeSampleMetadata:: Input %d: Unexpected track type (%d) detected",
                  pCurrentInput->uiIndex, pCurrentInput->pTrack->eType));
         pMetadata->uiCTSDTSDiff = 0;
         break;
      }
   } /* end: switch input type */

   pMetadata->bRandomAccess = BMUXLIB_INPUT_DESCRIPTOR_IS_KEYFRAME(pInDesc);  /* for audio, all samples are RAP */

   /* if any sample has a non-zero CTS/DTS diff, then ctts box required
      (for audio, this should never be true)  */
   if (!pCurrentInput->pTrack->bCttsRequired && (0 != pMetadata->uiCTSDTSDiff))
      pCurrentInput->pTrack->bCttsRequired = true;
   /* if any sample is not a RAP, then a stss box is required.  Box is not
      required if ALL samples are RAP (and no RAP samples is invalid) */
   if (!pCurrentInput->pTrack->bStssRequired && (false == pMetadata->bRandomAccess))
      pCurrentInput->pTrack->bStssRequired = true;
   pMetadata->uiOffset = uiOffset;
}

/*
   Since this sample is complete, update the metadata based on the metadata from the new sample

   Note that we do not care what kind of descriptor this is
   - if its a metadata descriptor, it will be processed when the new sample is processed

   NOTE: This function is not invoked if the EOS is seen, and the current sample is completed using
         the existing metadata for the sample.  Thus, the DTSDelta value is zero (its initial value)
         since the DTS of the "next sample" cannot be known (end of stream, so there is no "next");
         the DTS delta for the last sample in the trak is permitted to be zero
*/
static void UpdateSampleMetadata(BMUXlib_File_MP4_P_CurrentSample *pCurrentSample , const BMUXlib_Input_Descriptor *pInDesc)
{
   BMUXlib_File_MP4_P_Metadata *pMetadata = &pCurrentSample->stMetadata;
   BMUXlib_File_MP4_P_Input *pCurrentInput = pCurrentSample->pInput;
   uint64_t uiDTS = BMUXLIB_INPUT_DESCRIPTOR_DTS(pInDesc);     /* DTS(n+1) */
   uint64_t uiResult;
   uint32_t uiDTSDelta90kHz;
   uint64_t guiFullScaleMod33 = 1;
   guiFullScaleMod33 <<= 33;

   /* delta DTS (duration of the current sample) is the DTS of the new sample - DTS of the sample being processed */
   uiResult = uiDTS - pCurrentSample->uiDTS;

   if ((int64_t)uiResult < 0)
      uiResult += guiFullScaleMod33;    /* convert to modulo-33-bits */

   /* NOTE: This code assumes any deltaDTS > 1/2 full scale indicates the DTS of the new frame is BEFORE the DTS of the current frame
      => this is an invalid scenario (would indicate a bogus DTS or a discontinuity). */
   if (uiResult == 0 || uiResult > BMUXLIB_FILE_MP4_P_MODULO_33BITS_MID_RANGE)
   {
      BDBG_WRN(("Input %d: Delta DTS should be positive and non-zero (%lld detected)", pCurrentInput->uiIndex, uiResult));
      uiResult = BMUXLIB_FILE_MP4_P_MODULO_33BITS_MID_RANGE;   /* clip to max */
   }

   uiDTSDelta90kHz = (uint32_t)uiResult;

   switch (pCurrentInput->pTrack->eType)
   {
      case BMUXlib_File_MP4_P_TrackType_eVideo:
      {
         /* for video, timescale of the media is 90khz so the metadata deltaDTS is the same as the duration increment */
         pCurrentSample->uiDTSDelta90kHz = pMetadata->uiDTSDelta = uiDTSDelta90kHz;
         break;
      }
      case BMUXlib_File_MP4_P_TrackType_eAudio:
      {
         pCurrentSample->uiDTSDelta90kHz = uiDTSDelta90kHz;
         /* convert delta DTS into timescale units ... */
         uiResult = uiDTSDelta90kHz * pCurrentInput->pTrack->uiTimescale;
         pMetadata->uiDTSDelta = uiResult / BMUXLIB_FILE_MP4_P_TIMESCALE_90KHZ;
         break;
      }
      default:
      {
         BDBG_WRN(("UpdateSampleMetadata:: Input %d: Unexpected track type (%d) detected",
                  pCurrentInput->uiIndex,pCurrentInput->pTrack->eType));
         pCurrentSample->uiDTSDelta90kHz = pMetadata->uiDTSDelta = 0;
         break;
      }
   } /* end: switch input type */

   /* NOTE: sample size is updated on-the-fly as blocks are added to the output */
}

/****************************************
   Input Descriptor Processing (Video)
*****************************************/

/*
   Process available descriptors for a video input

   NOTE: this routine expects that at least one input descriptor is available before entering
   It also expects that the current input descriptor is the descriptor that has SOF set and
   has the relevant frame information (PTS/DTS etc).

   This will process one complete sample consisting of all the descriptors it detects on the
   specified input (including any metadata descriptors) until it sees the next descriptor
   that has a SOF.

   The location referenced by peBlocked will indicate whether this routine was blocked waiting
   for a resource.

   *** VERY IMPORTANT *** This code must cope with the possibility that ANY sample or data unit
   can be split into any number of input descriptors.

   NOTES:
   For H.264, the start code from the start of the NALU is skipped, and a size field is created
   which is subsequently updated once the NALU is complete and the size becomes known.  The
   param set NALUs are currently copied locally and then skipped.

   For MPEG 4, the VisObjSeq, VisObj and VidObjLayer DUs are copied locally and then skipped.
   All other DUs are processed as-is. MPEG 4 does not require the use of size fields
   (start codes are retained)

   Returns:
      BERR_SUCCESS - video descriptors processed successfully
      else error from an interface API, or bad state.
*/
/* TODO: This state machine could be cleaned up/optimized much the same as was done for audio */
static BERR_Code ProcessVideoDescriptors(BMUXlib_File_MP4_Handle hMP4Mux, BMUXlib_File_MP4_P_BlockedCause *peBlocked)
{
   BERR_Code rc = BERR_SUCCESS;
   BMUXlib_File_MP4_P_CurrentSample * const pCurrentSample = &hMP4Mux->stCurrentSample;
   BMUXlib_File_MP4_P_Input * const pCurrentInput = pCurrentSample->pInput;
   BMUXlib_File_MP4_P_DataUnitInfo * const pDataUnit = &pCurrentSample->stDataUnit;

   BMUXlib_File_MP4_P_VideoState eState = pCurrentSample->eState.Video;
   /* data from input descriptors always goes to the mdat unless determined otherwise ... */
   BMUXlib_File_MP4_P_Output *pOutput = hMP4Mux->pMdatOutput;
   BMUXlib_Input_Descriptor *pInDesc = &pCurrentSample->stInDesc;
   BMUXlib_File_MP4_P_OutputCallbackData *pCallbackData;
   bool bDescAvail;

   /* process descriptors until either all available input descriptors processed,
      run out of output descriptors, or the end of the current sample detected */
   while ((BMUXlib_File_MP4_P_BlockedCause_eNone == *peBlocked)         /* we're not blocked ... */
            && !pCurrentSample->bComplete                               /* current sample is not complete ... */
            && (BERR_SUCCESS == rc))                                    /* and no error ... */
   {
      BDBG_MODULE_MSG(BMUX_MP4_STATE, ("Processing Video Descriptors (state: %d)", eState));
      switch (eState)
      {
         case BMUXlib_File_MP4_P_VideoState_eSampleStart:
         {
            /* start processing a new video sample - we come here after performing a new input search
               hence this descriptor represents the *start* of the sample to process */

            /* peek at the first descriptor of this sample
               NOTE: we do not do a "get" since the get will automatically be performed when the
               descriptor has been processed */
            bDescAvail = BMUXlib_Input_PeekAtNextDescriptor(pCurrentInput->hInput, pInDesc);
            /* NOTE: we know there is a descriptor available here */
            BDBG_ASSERT(bDescAvail == true);

            PrintVideoDescriptor("Sample Start", pInDesc);

            pCurrentInput->pTrack->uiSampleCount++;

            /* create the initial metadata for this sample, based on data from the "Frame start" descriptor */
            InitializeSampleMetadata(pCurrentSample, pInDesc, BMUXlib_Output_GetEndOffset(pOutput->hOutput));

            /* see if this is a metadata descriptor */
            if (BMUXLIB_INPUT_DESCRIPTOR_IS_METADATA(pInDesc))
            {
               /* if so, process the metadata contents, then discard the descriptor ... */
               ProcessVideoMetadataDescriptor(pCurrentSample->pInput, pInDesc);
               pCurrentSample->uiBytesSkipped = BMUXLIB_INPUT_DESCRIPTOR_LENGTH(pInDesc);
               /* go create a "NOP" output descriptor to consume this input descriptor */
               eState = BMUXlib_File_MP4_P_VideoState_ePopulateOutDesc;
               break;
            }
            else
            {
               /* new descriptor, so reset the bytes skipped count */
               pCurrentSample->uiBytesSkipped = 0;
            }

            if (BMUXLIB_FILE_MP4_P_IS_DU_ENABLED(pCurrentInput->pTrack->eCoding))
            {
               /* process Data Unit descriptors for this input ... */
               if (BMUXlib_File_MP4_P_CodingType_eAVC == pCurrentInput->pTrack->eCoding)
               {
                  /* Data unit markers expected with H.264 content ... */
                  BDBG_ASSERT(BMUXLIB_INPUT_DESCRIPTOR_VIDEO_IS_DATA_UNIT_START(pInDesc));

                  pDataUnit->uiType = BMUXLIB_FILE_MP4_P_VIDEO_DESC_GET_DU_TYPE(pInDesc);
                  pDataUnit->uiLength = 0;
                  pDataUnit->uiBytesToSkip = BMUXLIB_FILE_MP4_P_NALU_START_CODE_SIZE;

                  BDBG_MODULE_MSG(BMUX_MP4_DU, ("NALU Start detected. Type: %d", pDataUnit->uiType));
                  eState = BMUXlib_File_MP4_P_VideoState_eNALUSkipStartCode;
               }
               else if (BMUXlib_File_MP4_P_CodingType_eMpeg4Video == pCurrentInput->pTrack->eCoding)
               {
                  /* Data unit markers expected with mp4v content ... */
                  BDBG_ASSERT(BMUXLIB_INPUT_DESCRIPTOR_VIDEO_IS_DATA_UNIT_START(pInDesc));

                  pDataUnit->uiType = BMUXLIB_FILE_MP4_P_VIDEO_DESC_GET_DU_TYPE(pInDesc);
                  pDataUnit->uiLength = 0;
                  /* for MPEG4 , we do not need to skip bytes */
                  pDataUnit->uiBytesToSkip = 0;
                  BDBG_MODULE_MSG(BMUX_MP4_DU, ("MP4V Start Code detected. Type: %d", pDataUnit->uiType));
                  /* see if this DU is a sequence/config DU ... */
                  if (BMUXLIB_FILE_MP4_P_DU_IS_MP4V_CONFIG(pDataUnit->uiType) || BMUXLIB_FILE_MP4_P_DU_IS_MP4V_USERDATA(pDataUnit->uiType))
                  {
                     /* if so, process the DU (extract the DU and store it, if necessary)  ... */
                     eState = BMUXlib_File_MP4_P_VideoState_eMP4VProcessConfig;
                  }
                  else
                  {
                     /* if not, process normally ... */
                     eState = BMUXlib_File_MP4_P_VideoState_ePopulateOutDesc;
                  }
               }
               else
               {
                  /* not expecting DUs on this input, so process as a normal descriptor */
                  BDBG_WRN(("Video: Not expecting DUs for coding type %d", pCurrentInput->pTrack->eCoding));
                  eState = BMUXlib_File_MP4_P_VideoState_ePopulateOutDesc;
               }
            }
            else
            {
               /* No Data Units expected, so process this descriptor as-is */
               eState = BMUXlib_File_MP4_P_VideoState_ePopulateOutDesc;
            }
            break;
         }

         case BMUXlib_File_MP4_P_VideoState_ePopulateOutDesc:
         {
            uint32_t uiLength;                        /* input data length */
            uint8_t *pAddress;                        /* input data address */

            /* fill in an output descriptor to feed this input descriptor's contents
               to the output storage */
            /* NOTE: this is the only state that consumes input descriptors
               (except AVC param set NALU processing and MPEG 4 VObjSeq processing) */

            /* check that there is an output descriptor available ... */
            if (!BMUXlib_Output_IsSpaceAvailable(pOutput->hOutput))
            {
               *peBlocked = BMUXlib_File_MP4_P_BlockedCause_eOutput;   /* out of descriptors */
               break;                                 /* wait until we have more available */
            }
            /* check that there are release q entries available (each output descriptor also requires a release Q entry
               when the associated callback is invoked */
            if (hMP4Mux->pReleaseQFreeList == NULL)
            {
               *peBlocked = BMUXlib_File_MP4_P_BlockedCause_eOutput;   /* out of release Q entries */
               break;                                 /* wait until we have more available */
            }

            uiLength = BMUXLIB_INPUT_DESCRIPTOR_LENGTH(pInDesc) - pCurrentSample->uiBytesSkipped;
            pAddress = (uint8_t *)(BMUXLIB_INPUT_DESCRIPTOR_VIRTUAL_ADDRESS(pInDesc)) + pCurrentSample->uiBytesSkipped;

            BDBG_MODULE_MSG(BMUX_MP4_OUTPUT, ("Video: %d bytes (%d bytes skipped)", uiLength, pCurrentSample->uiBytesSkipped));

            /* NOTE: if this represents a NALU that is to be skipped, then length will be zero (i.e. a NOP)
               This ensures that the input descriptor corresponding to the skipped NALU is still freed as
               appropriate */
            /* check if there are callback entries available ... */
            pCallbackData = BMUXlib_File_MP4_P_NewOutputCallbackData(hMP4Mux, pCurrentInput, pCurrentInput->uiDescSeqCount);
            if (NULL == pCallbackData)
            {
               /* no more callback data entries available
                  (should not happen, since this implies there would be no output descriptors
                  available either) */
               *peBlocked = BMUXlib_File_MP4_P_BlockedCause_eCallbackData;
               break;                                 /* wait until we have more available */
            }
            InitOutputCallback(hMP4Mux, BMUXlib_File_MP4_P_OutputCallback_eInput, pCallbackData);
            /* append this data to the mdat ... */
            DebugSetDescriptorSource(pOutput, "Video");
            BMUXlib_File_MP4_P_OutputDescriptorAppend(hMP4Mux, pOutput, pAddress, uiLength, BMUXlib_File_MP4_P_OutputCallback_eInput);

            pCurrentSample->stMetadata.uiSampleSize += uiLength;
            pCurrentInput->uiWaitingCount++;
            pCurrentSample->stDataUnit.uiLength += uiLength;            /* update data unit length incase it is required */
            pCurrentInput->uiDescSeqCount++;

            /* finished with this descriptor (fetch next descriptors) ... */
            /* NOTE: this simply discards the descriptor that was peeked at previously,
               and has since been processed */
            bDescAvail = BMUXlib_Input_GetNextDescriptor(pCurrentInput->hInput, pInDesc);
            BDBG_ASSERT(bDescAvail == true); /* there must always be a descriptor to "get" */

            eState = BMUXlib_File_MP4_P_VideoState_eNextDescriptor;
            break;
         }

         case BMUXlib_File_MP4_P_VideoState_eNextDescriptor:
            if (!BMUXlib_Input_IsDescriptorAvailable(pCurrentInput->hInput))
            {
               *peBlocked = BMUXlib_File_MP4_P_BlockedCause_eInput;   /* out of descriptors */
               break;
            }
            bDescAvail = BMUXlib_Input_PeekAtNextDescriptor(pCurrentInput->hInput, pInDesc);
            BDBG_ASSERT(bDescAvail == true);

            pCurrentSample->uiBytesSkipped = 0;          /* new input descriptor - no bytes skipped yet */

            PrintVideoDescriptor("Next Desc", pInDesc);

            if (!pCurrentSample->pInput->bEOS)
               pCurrentSample->pInput->bEOS = BMUXLIB_INPUT_DESCRIPTOR_IS_EOS(pInDesc);

            /* if DU processing active ... */
            if (BMUXLIB_FILE_MP4_P_IS_DU_ENABLED(pCurrentInput->pTrack->eCoding))
            {
               if (BMUXlib_File_MP4_P_CodingType_eMpeg4Video == pCurrentInput->pTrack->eCoding)
               {
                  /* is this descriptor the start of a new DU? (signifies the end of the current DU) */
                  /* SW7425-357: Consider DU or SOF to be indicators of new DU */
                  if (BMUXLIB_INPUT_DESCRIPTOR_VIDEO_IS_DATA_UNIT_START(pInDesc) || BMUXLIB_FILE_MP4_P_IS_SAMPLE_END(pInDesc))
                  {
                     if (BMUXLIB_INPUT_DESCRIPTOR_IS_FRAMESTART(pInDesc) && !BMUXLIB_INPUT_DESCRIPTOR_VIDEO_IS_DATA_UNIT_START(pInDesc))
                        BDBG_WRN(("Video Descriptor: DU indicator missing (SOF detected)"));

                     BDBG_MODULE_MSG(BMUX_MP4_DU, ("DU End detected.  Size = %d bytes", pCurrentSample->stDataUnit.uiLength));
                     /* if this is the first Video Object Layer seen, then configuration data is done */
                     if (BMUXLIB_FILE_MP4_P_DU_IS_MP4V_VIDOBJL(pDataUnit->uiType))
                     {
#if BDBG_DEBUG_BUILD
                        if (pCurrentInput->pTrack->bDecSpecInfoDone)
                        {
                           /* verify the length did not change */
                           if ((pCurrentInput->pTrack->uiDecSpecInfoSize != pCurrentInput->pTrack->uiDecSpecInfoValidateLength) && !pCurrentInput->pTrack->bDecSpecInfoValidated)
                           {
                              BDBG_WRN(("MP4V configuration data length has changed (%d -> %d) - Dynamic configuration not supported!!", pCurrentInput->pTrack->uiDecSpecInfoSize, pCurrentInput->pTrack->uiDecSpecInfoValidateLength));
                              pCurrentInput->pTrack->bDecSpecInfoValidated = true;
                           }
                           /* reset the validation information */
                           pCurrentInput->pTrack->uiDecSpecInfoValidateLength = 0;
                        }
#endif
                        pCurrentInput->pTrack->bDecSpecInfoDone = true;
                     }
                     eState = BMUXlib_File_MP4_P_VideoState_eNextDU;
                     break;
                  }
                  else
                  {
                     /* else, not DU start, so is this still part of a param set DU? */
                     if (BMUXLIB_FILE_MP4_P_DU_IS_MP4V_CONFIG(pDataUnit->uiType) || BMUXLIB_FILE_MP4_P_DU_IS_MP4V_USERDATA(pDataUnit->uiType))
                     {
                        /* if so, process the DU (extract the DU and store it, if necessary)  ... */
                        eState = BMUXlib_File_MP4_P_VideoState_eMP4VProcessConfig;
                        break;
                     }
                     /* continue on and check for EOS/SOF etc... */
                  }
               } /* end MPEG 4 processing */

               if (BMUXlib_File_MP4_P_CodingType_eAVC == pCurrentInput->pTrack->eCoding)
               {
                  /* ... and Start Code bytes still remain to be skipped, then go back and skip more */
                  if (0 != pCurrentSample->stDataUnit.uiBytesToSkip)
                  {
                     eState = BMUXlib_File_MP4_P_VideoState_eNALUSkipStartCode;
                     break;
                  }

                  /* is this descriptor the start of a new DU? (signifies the end of the current DU) */
                  /* SW7425-357: Consider DU or SOF to be indicators of new DU */
                  if (BMUXLIB_INPUT_DESCRIPTOR_VIDEO_IS_DATA_UNIT_START(pInDesc) || BMUXLIB_FILE_MP4_P_IS_SAMPLE_END(pInDesc))
                  {
                     if (BMUXLIB_INPUT_DESCRIPTOR_IS_FRAMESTART(pInDesc) && !BMUXLIB_INPUT_DESCRIPTOR_VIDEO_IS_DATA_UNIT_START(pInDesc))
                        BDBG_WRN(("Video Descriptor: DU indicator missing (SOF detected)"));

                     BDBG_MODULE_MSG(BMUX_MP4_DU, ("DU End detected.  Size = %d bytes", pCurrentSample->stDataUnit.uiLength));
                     /* if this current DU is a param set DU ... */
                     if (BMUXLIB_FILE_MP4_P_DU_IS_NALU_SPS(pDataUnit->uiType))
                     {
#if BDBG_DEBUG_BUILD
                        if (hMP4Mux->bAVCSPSDone)
                        {
                           /* verify the length did not change */
                           if ((hMP4Mux->uiAVCSPSLength != hMP4Mux->uiAVCSPSValidateLength) && !hMP4Mux->bAVCSPSValidateDone)
                           {
                              BDBG_WRN(("AVC SPS Data length has changed (%d -> %d) - Dynamic configuration is not supported!!", hMP4Mux->uiAVCSPSLength, hMP4Mux->uiAVCSPSValidateLength));
                              hMP4Mux->bAVCSPSValidateDone = true;
                           }
                           /* reset the validation information */
                           hMP4Mux->uiAVCSPSValidateLength = 0;
                        }
#endif
                        hMP4Mux->bAVCSPSDone = true;  /* indicate this param set is complete */
                     }

                     if (BMUXLIB_FILE_MP4_P_DU_IS_NALU_PPS(pDataUnit->uiType))
                     {
#if BDBG_DEBUG_BUILD
                        if (hMP4Mux->bAVCPPSDone)
                        {
                           /* verify the length did not change */
                           if ((hMP4Mux->uiAVCPPSLength != hMP4Mux->uiAVCPPSValidateLength) && !hMP4Mux->bAVCPPSValidateDone)
                           {
                              BDBG_WRN(("AVC PPS Data length has changed (%d -> %d) - Dynamic configuration is not supported!!", hMP4Mux->uiAVCPPSLength, hMP4Mux->uiAVCPPSValidateLength));
                              hMP4Mux->bAVCPPSValidateDone = true;
                           }
                           /* reset the validation information */
                           hMP4Mux->uiAVCPPSValidateLength = 0;
                        }
#endif
                        hMP4Mux->bAVCPPSDone = true;  /* indicate this param set is complete */
                     }

                     if (0 == pCurrentSample->stDataUnit.uiLength)
                     {
                        /* the entire contents of this descriptor were skipped, so no need to update the size
                           (none was sent) */
                        eState = BMUXlib_File_MP4_P_VideoState_eNextDU;
                        break;
                     }
                     /* else, the previous NALU size needs to be updated */
                     eState = BMUXlib_File_MP4_P_VideoState_eNALUSizeUpdate;
                     break;
                  }
                  /* else, not DU start, so is this still part of a param set DU? */
                  if (BMUXLIB_FILE_MP4_P_DU_IS_AVC_PARAM(pDataUnit->uiType))
                  {
                     eState = BMUXlib_File_MP4_P_VideoState_eNALUProcessParams;
                     break;
                  }
               } /* end: AVC processing */
            } /* end: DU expected */

            /* if not DU, is this descriptor the start of a new sample? */
            if (BMUXLIB_FILE_MP4_P_IS_SAMPLE_END(pInDesc))
            {
               /* if so, update metadata information, then go back to look for a new sample
                  (this sample is complete)  */
               /* NOTE: do not consume this descriptor - it will be processed
                  when this sample is selected in the future */
               UpdateSampleMetadata(pCurrentSample, pInDesc);
               /* next sample on this input detected - force it to find the next sample to process
                  (may not be this one, depends on timestamp) next iteration */
               /* NOTE: when the new sample is found, the next state will be forced to "sample start" */
               pCurrentSample->bComplete = true;
               break;
            }
            else
            {
               /* else, neither start of sample, nor start of DU, so go on to process the next descriptor */
               /* see if this is a metadata descriptor */
               if (BMUXLIB_INPUT_DESCRIPTOR_IS_METADATA(pInDesc))
               {
                  /* if so, process the metadata contents, then discard the descriptor ... */
                  ProcessVideoMetadataDescriptor(pCurrentSample->pInput, pInDesc);
                  pCurrentSample->uiBytesSkipped = BMUXLIB_INPUT_DESCRIPTOR_LENGTH(pInDesc);
                  /* go create a "NOP" output descriptor to consume this input descriptor */
               }
               eState = BMUXlib_File_MP4_P_VideoState_ePopulateOutDesc;
            }
            break;

         /* ---- these states are specific to H.264 video data ---- */
         case BMUXlib_File_MP4_P_VideoState_eNALUSkipStartCode:
         {
            /* remove (skip) the start code from each NALU (we do not want it in the output) ...
               This assumes that the start code is the first bytes of the descriptor (i.e. NALU starts at
               the start of the descriptor). The code comes here whenever a DU start is detected */
            uint32_t uiBytesToSkip;
            /* NOTE: length in the descriptor is the length of the data for *this* descriptor - it may NOT
               be the length of the NALU. NALU size cannot be determined until the *next* NALU start seen */

            /* bytes to skip this time is smaller of: bytes remaining to be skipped, or
               bytes in this descriptor */
            uiBytesToSkip = (BMUXLIB_INPUT_DESCRIPTOR_LENGTH(pInDesc) <= pDataUnit->uiBytesToSkip)?
                                 BMUXLIB_INPUT_DESCRIPTOR_LENGTH(pInDesc):
                                 pDataUnit->uiBytesToSkip;

            pCurrentSample->uiBytesSkipped += uiBytesToSkip;
            pDataUnit->uiBytesToSkip -= uiBytesToSkip;

            if (BMUXLIB_INPUT_DESCRIPTOR_LENGTH(pInDesc) == uiBytesToSkip)
            {
               /* we consumed this whole descriptor... */
               /* populate an output descriptor for this input descriptor - this will
                  effectively be a "NOP" since we skipped all the bytes of this descriptor */
               eState = BMUXlib_File_MP4_P_VideoState_ePopulateOutDesc;
               break;
            }
            /* done skipping the start code (there should not be any more bytes to skip - if the
               start code is split, the first descriptor will be consumed and the rest will be in
               the next descriptor; if the start code is not split, all bytes will be skipped in
               one go) */
            BDBG_ASSERT(0 == pDataUnit->uiBytesToSkip);

            /* if this NALU is a PPS or SPS, then remove it from the sample */
            if (BMUXLIB_FILE_MP4_P_DU_IS_AVC_PARAM(pDataUnit->uiType))
            {
               hMP4Mux->pAVCSPSData = hMP4Mux->aAVCSPSData;
               hMP4Mux->pAVCPPSData = hMP4Mux->aAVCPPSData;
               eState = BMUXlib_File_MP4_P_VideoState_eNALUProcessParams;
               break;
            }

            /* else, create a size entry for this NALU ... */
            eState = BMUXlib_File_MP4_P_VideoState_eNALUCreateSize;
            break;
         }

         case BMUXlib_File_MP4_P_VideoState_eNALUProcessParams:
         {
            uint32_t uiLength = BMUXLIB_INPUT_DESCRIPTOR_LENGTH(pInDesc) - pCurrentSample->uiBytesSkipped;
            uint8_t *pAddress = (uint8_t *)BMUXLIB_INPUT_DESCRIPTOR_VIRTUAL_ADDRESS(pInDesc) + pCurrentSample->uiBytesSkipped;
            /* process a parameter set NALU
               TEMPORARY: for now, we simply copy the NALU contents out of the CDB.
               This is not permissible in the protected CDB - would need to output to file and restore when creating box */
            /* once the NALU processed, we skip the entire NALU contents
               i.e. output a NOP output descriptor if we have copied all the bytes */
            /* NOTE: we do not know in advance how many bytes to skip - we simply skip
               (process) all bytes until the end of the DU */
            if (BMUXLIB_FILE_MP4_P_DU_IS_NALU_SPS(pDataUnit->uiType))
            {
               /* FIXME: This is temporary: ultimately this needs to go to a separate file, along with relevant metadata
                         for now, we simply copy the SPS/PPS to a local buffer for storage so that it can be written to the
                         avcC box later - strictly speaking we are NOT permitted to read directly from the CDB!! */
               if (!hMP4Mux->bAVCSPSDone)
               {
                  BDBG_MODULE_MSG(BMUX_MP4_DU, ("SPS NALU detected (%d bytes)", uiLength));
                  if (hMP4Mux->uiAVCSPSLength + uiLength > sizeof(hMP4Mux->aAVCSPSData))
                     BDBG_WRN(("AVC SPS data exceeds available storage - data truncated (output file likely corrupt)"));
                  else
                  {
                     /* copy the SPS to local storage */
                     BKNI_Memcpy(hMP4Mux->pAVCSPSData, pAddress, uiLength);
                     hMP4Mux->pAVCSPSData += uiLength;
                     hMP4Mux->uiAVCSPSLength += uiLength;
                  }
               }
#if BDBG_DEBUG_BUILD
               else if (!hMP4Mux->bAVCSPSValidateDone)
               {
                  /* check to see if the SPS changed from the one we captured ... */
                  if (0 != BKNI_Memcmp(hMP4Mux->pAVCSPSData+hMP4Mux->uiAVCSPSValidateLength, pAddress, uiLength))
                  {
                     BDBG_WRN(("AVC SPS Data has changed - Dynamic configuration is not supported!!"));
                     hMP4Mux->bAVCSPSValidateDone = true;
                  }
                  hMP4Mux->uiAVCSPSValidateLength += uiLength;
               }
#endif
            }
            if (BMUXLIB_FILE_MP4_P_DU_IS_NALU_PPS(pDataUnit->uiType))
            {
               if (!hMP4Mux->bAVCPPSDone)
               {
                  BDBG_MODULE_MSG(BMUX_MP4_DU, ("PPS NALU detected (%d bytes)", uiLength));
                  if (hMP4Mux->uiAVCPPSLength + uiLength > sizeof(hMP4Mux->aAVCPPSData))
                     BDBG_WRN(("AVC PPS data exceeds available storage - data truncated (output file likely corrupt)"));
                  else
                  {
                     /* copy the PPS to local storage */
                     BKNI_Memcpy(hMP4Mux->pAVCPPSData, pAddress, uiLength);
                     hMP4Mux->pAVCPPSData += uiLength;
                     hMP4Mux->uiAVCPPSLength += uiLength;
                  }
               }
#if BDBG_DEBUG_BUILD
               else if (!hMP4Mux->bAVCPPSValidateDone)
               {
                  /* check to see if the PPS changed from the one we captured ... */
                  if (0 != BKNI_Memcmp(hMP4Mux->pAVCPPSData+hMP4Mux->uiAVCPPSValidateLength, pAddress, uiLength))
                  {
                     hMP4Mux->bAVCPPSValidateDone = true;
                     BDBG_WRN(("AVC PPS Data has changed - Dynamic configuration is not supported!!"));
                  }
                  hMP4Mux->uiAVCPPSValidateLength += uiLength;
               }
#endif
            }

            /* skip the entire contents of this descriptor ... */
            /* FIXME: in theory, we do not need to do this anymore - we can simply invoke the release callback
               here with the relevant sequence number, and it will queue it for release at the appropriate time
               - there should no longer be a need to queue up a NOP to the output */
            pCurrentSample->uiBytesSkipped = BMUXLIB_INPUT_DESCRIPTOR_LENGTH(pInDesc);
            eState = BMUXlib_File_MP4_P_VideoState_ePopulateOutDesc;
            break;
         }

         case BMUXlib_File_MP4_P_VideoState_eNALUCreateSize:
         {
            /* create a size entry for this data unit. */
            uint32_t *pSize = BMUXlib_File_MP4_P_FindFreeSizeEntry(&hMP4Mux->stSizes);

            if (NULL == pSize)
            {
               /* if out of size entries, then wait ... */
               *peBlocked = BMUXlib_File_MP4_P_BlockedCause_eSizeEntry;
               break;
            }

            /* since we don't know the size of this NALU yet, size is 0 */
            *pSize = 0;

            eState = BMUXlib_File_MP4_P_VideoState_eNALUPopulateOutDescSize;
            break;
         }

         case BMUXlib_File_MP4_P_VideoState_eNALUPopulateOutDescSize:
         {
            /* create a descriptor to represent this size ...
               (note: this descriptor is *inserting* the size into the stream, so is an
                      append descriptor) */
            /* check that there is an output descriptor available ... */
            if (!BMUXlib_Output_IsSpaceAvailable(pOutput->hOutput))
            {
               *peBlocked = BMUXlib_File_MP4_P_BlockedCause_eOutput;   /* out of descriptors */
               break;                                 /* wait until we have more available */
            }

            /* save the offset of the NALU size entry (for the size update) */
            pDataUnit->uiSizeOffset = BMUXlib_Output_GetEndOffset(pOutput->hOutput);

            pCallbackData = BMUXlib_File_MP4_P_NewOutputCallbackData(hMP4Mux, hMP4Mux->stSizes.pCurrentEntry, 0);
            if (NULL == pCallbackData)
            {
               /* no more callback data entries available
                  (should not happen, since this implies there would be no output descriptors
                  available either) */
               *peBlocked = BMUXlib_File_MP4_P_BlockedCause_eCallbackData;
               break;                                 /* wait until we have more available */
            }
            InitOutputCallback(hMP4Mux, BMUXlib_File_MP4_P_OutputCallback_eSizeBuffer, pCallbackData);

            /* append this data to the end of the mdat ... */
            DebugSetDescriptorSource(pOutput, "NALU size entry");
            BMUXlib_File_MP4_P_OutputDescriptorAppend(hMP4Mux, pOutput, hMP4Mux->stSizes.pCurrentEntry, sizeof(uint32_t), BMUXlib_File_MP4_P_OutputCallback_eSizeBuffer);
            pCurrentSample->stMetadata.uiSampleSize += sizeof(uint32_t);
            /* NOTE: NALU size does NOT include the size field itself */
            eState = BMUXlib_File_MP4_P_VideoState_ePopulateOutDesc;
            break;
         }

         case BMUXlib_File_MP4_P_VideoState_eNALUSizeUpdate:
         {
            uint32_t *pSize;

            /* NOTE: if the size entry is still available and hasn't yet been sent to storage,
               do an in-place update and then skip creating a descriptor for it */
            /* FIXME: How to determine if the descriptor is pending? Need to know when it has
               been sent to storage, but there is no callback to track this - perhaps we have
               to only allow in-place until the descriptor has been sent with ProcessNewDescriptors
               and thus only within the current service period */
            /* FIXME: might be able to zero out a flag when the Process Waiting has been done,
            and thus, we can check that flag, which is set when the descriptor is written to the output
            so that we'd know when that decsriptor has been sent out */
#if 0
            if (BMUXlib_File_MP4_P_DescriptorStatus_eInUse != pDataUnit->pSizeDesc->eStatus)
            {
               /* the descriptor referencing this size entry is not In Use by storage, so check
                  if it has not already been freed... */
               if ((NULL != pSize) && (BMUXLIB_FILE_MP4_P_SIZE_ENTRY_UNUSED != *pSize))
               {
                  /* if not, size still waiting, so update it directly in-place */
                  BDBG_MODULE_MSG(BMUX_MP4_DU, ("NALU Size Entry Update (in-place): Length = %d", pDataUnit->uiLength));

                  *pSize = MP4MUX_SWAP_U32(pDataUnit->uiLength);

                  /* find the next Data unit ... */
                  eState = BMUXlib_File_MP4_P_VideoState_eNextDU;
                  break;
               }
               /* else, do nothing and proceed with creating a new size update descriptor ... */
            }
#endif
            /* else create a new size entry for the update ...*/
            pSize = BMUXlib_File_MP4_P_FindFreeSizeEntry(&hMP4Mux->stSizes);

            if (NULL == pSize)
            {
               /* if out of size entries, then wait ... */
               *peBlocked = BMUXlib_File_MP4_P_BlockedCause_eSizeEntry;
               break;
            }
            BDBG_MODULE_MSG(BMUX_MP4_DU, ("NALU Size Entry Update: Length = %d", pDataUnit->uiLength));
            *pSize = MP4MUX_SWAP_U32(pDataUnit->uiLength);

            eState = BMUXlib_File_MP4_P_VideoState_eNALUPopulateOutDescUpdate;
            break;
         }

         case BMUXlib_File_MP4_P_VideoState_eNALUPopulateOutDescUpdate:
         {
            uint64_t uiSizeOffset = pDataUnit->uiSizeOffset;

            /* fill in an output descriptor to represent this size update ... */
            /* check that there is an output descriptor available ... */
            if (!BMUXlib_Output_IsSpaceAvailable(pOutput->hOutput))
            {
               *peBlocked = BMUXlib_File_MP4_P_BlockedCause_eOutput;   /* out of descriptors */
               break;                                 /* wait until we have more available */
            }
            pCallbackData = BMUXlib_File_MP4_P_NewOutputCallbackData(hMP4Mux, hMP4Mux->stSizes.pCurrentEntry, 0);
            if (NULL == pCallbackData)
            {
               /* no more callback data entries available
                  (should not happen, since this implies there would be no output descriptors
                  available either) */
               *peBlocked = BMUXlib_File_MP4_P_BlockedCause_eCallbackData;
               break;                                 /* wait until we have more available */
            }
            InitOutputCallback(hMP4Mux, BMUXlib_File_MP4_P_OutputCallback_eSizeBuffer, pCallbackData);
            DebugSetDescriptorSource(pOutput, "NALU size update");
            BMUXlib_File_MP4_P_OutputDescriptorUpdate(hMP4Mux, pOutput, hMP4Mux->stSizes.pCurrentEntry, sizeof(uint32_t), uiSizeOffset, BMUXlib_File_MP4_P_OutputCallback_eSizeBuffer);
            /* NOTE: since this is a size *update* we do not modify sample or DU sizes, etc */
            eState = BMUXlib_File_MP4_P_VideoState_eNextDU;
            break;
         }
         /* ----- END H.264-specific states ----- */

         /* ---- these states are specific to Mpeg4 part 2 video data ---- */
         case BMUXlib_File_MP4_P_VideoState_eMP4VProcessConfig:
         {
            /* process Visual Object Sequence, Visual Object, Video Object and Video Object Layer DUs (if present) ... */
            uint32_t uiLength = BMUXLIB_INPUT_DESCRIPTOR_LENGTH(pInDesc) - pCurrentSample->uiBytesSkipped;
            uint8_t *pAddress = (uint8_t *)BMUXLIB_INPUT_DESCRIPTOR_VIRTUAL_ADDRESS(pInDesc) + pCurrentSample->uiBytesSkipped;

            if (BMUXLIB_FILE_MP4_P_DU_IS_MP4V_CONFIG(pDataUnit->uiType))
            {
               /* if Video Object Layer detected, then assume all sequence information is done
                  (assumed that vis. seq. first, then vis. object, then video object layer)
                  - we do not support changing configuration data  */
               if (!pCurrentInput->pTrack->bDecSpecInfoDone)
               {
                  uint32_t uiDecSpecInfoSize = pCurrentInput->pTrack->uiDecSpecInfoSize;
                  /* pull out the DU contents and store it in DecoderSpecConfig for later insertion into ES descriptor */
                  if (uiDecSpecInfoSize + uiLength > BMUXLIB_FILE_MP4_P_MAX_DEC_SPEC_SIZE)
                  {
                     BDBG_WRN(("Mpeg 4 Configuration Data exceeds available storage - data ignored (output likely corrupt)"));
                  }
                  else
                  {
                     /* copy DU contents to DecSpecInfo (append to existing data) ... */
                     uint8_t *pDest = &pCurrentInput->pTrack->aDecSpecInfo[uiDecSpecInfoSize];
                     BKNI_Memcpy(pDest, pAddress, uiLength);
                     pCurrentInput->pTrack->uiDecSpecInfoSize += uiLength;
                  }
               }
#if BDBG_DEBUG_BUILD
               else if (!pCurrentInput->pTrack->bDecSpecInfoValidated)
               {
                  uint8_t *pSavedData = &pCurrentInput->pTrack->aDecSpecInfo[pCurrentInput->pTrack->uiDecSpecInfoValidateLength];
                  /* check to see if the DU(s) changed from the one we captured ... */
                  if (0 != BKNI_Memcmp(pSavedData, pAddress, uiLength))
                  {
                     pCurrentInput->pTrack->bDecSpecInfoValidated = true;
                     BDBG_WRN(("MP4V Configuration Data changed - Dynamic configuration not supported!!"));
                  }
                  pCurrentInput->pTrack->uiDecSpecInfoValidateLength += uiLength;
               }
#endif
            }
            /* skip the entire contents of this descriptor (this data does not go into the stream) */
            pCurrentSample->uiBytesSkipped = BMUXLIB_INPUT_DESCRIPTOR_LENGTH(pInDesc);

            eState = BMUXlib_File_MP4_P_VideoState_ePopulateOutDesc;
            break;
         }
         /* ----- END Mpeg4 part 2 specific states ----- */

         case BMUXlib_File_MP4_P_VideoState_eNextDU:
         {
            PrintVideoDescriptor("Next DU", pInDesc);
            if (!pCurrentSample->pInput->bEOS)
               pCurrentSample->pInput->bEOS = BMUXLIB_INPUT_DESCRIPTOR_IS_EOS(pInDesc);

            /* is this descriptor the start of a new sample? (signifies the end of the current sample) */
            if (BMUXLIB_FILE_MP4_P_IS_SAMPLE_END(pInDesc))
            {
               /* if so, update metadata information, then go back to look for a new sample
                  (this sample is complete)  */
               /* NOTE: do not consume this descriptor - it will be processed
                  when this sample is selected in the future */
               UpdateSampleMetadata(pCurrentSample, pInDesc);
               /* next sample on this input detected - force it to find the next sample to process
                  (may not be this one, depends on timestamp) next iteration */
               /* NOTE: when the new sample is found, the next state will be forced to "sample start" */
               pCurrentSample->bComplete = true;
               break;
            }
            else
            {
               /* new descriptor, so reset the bytes skipped count. */
               /* see if this is a metadata descriptor */
               if (BMUXLIB_INPUT_DESCRIPTOR_IS_METADATA(pInDesc))
               {
                  /* if so, process the metadata contents, then discard the descriptor ... */
                  ProcessVideoMetadataDescriptor(pCurrentSample->pInput, pInDesc);
                  pCurrentSample->uiBytesSkipped = BMUXLIB_INPUT_DESCRIPTOR_LENGTH(pInDesc);

                  /* go create a "NOP" output descriptor to consume this input descriptor */
                  eState = BMUXlib_File_MP4_P_VideoState_ePopulateOutDesc;
               }
               else
               {
                  /* else, is the next DU in the current sample */
                  pDataUnit->uiType = BMUXLIB_FILE_MP4_P_VIDEO_DESC_GET_DU_TYPE(pInDesc);
                  BDBG_MODULE_MSG(BMUX_MP4_DU, ("DU Start detected. Type: %d", pDataUnit->uiType));
                  pDataUnit->uiLength = 0;
                  if (BMUXlib_File_MP4_P_CodingType_eAVC == pCurrentInput->pTrack->eCoding)
                  {
                     /* AVC video ... */
                     pDataUnit->uiBytesToSkip = BMUXLIB_FILE_MP4_P_NALU_START_CODE_SIZE;
                     eState = BMUXlib_File_MP4_P_VideoState_eNALUSkipStartCode;
                  }
                  else if (BMUXlib_File_MP4_P_CodingType_eMpeg4Video == pCurrentInput->pTrack->eCoding)
                  {
                     /* MPEG 4 video ... */
                     pDataUnit->uiBytesToSkip = 0;
                     if (BMUXLIB_FILE_MP4_P_DU_IS_MP4V_CONFIG(pDataUnit->uiType) || BMUXLIB_FILE_MP4_P_DU_IS_MP4V_USERDATA(pDataUnit->uiType))
                        /* if so, process the DU (extract the DU and store it, if necessary)  ... */
                        eState = BMUXlib_File_MP4_P_VideoState_eMP4VProcessConfig;
                     else
                        /* else send the whole Du to the output ... */
                        eState = BMUXlib_File_MP4_P_VideoState_ePopulateOutDesc;
                  }
                  else
                  {
                     /* unknown codec */
                     pDataUnit->uiBytesToSkip = 0;
                     eState = BMUXlib_File_MP4_P_VideoState_ePopulateOutDesc;
                  }
               }
            }
            break;
         }

         default:
            /* should not happen */
            BDBG_ERR(("Bad Video State detected: %d", eState));
            rc = BERR_TRACE(BERR_UNKNOWN);
            break;
      } /* end: switch video state */
   } /* end while descriptors available, not error and not blocked */
   pCurrentSample->eState.Video = eState;                /* update current state ... */
   return rc;
}

/* process a video metadata descriptor, and set the relevant track information
   for the track corresponding to this input */
static void ProcessVideoMetadataDescriptor(const BMUXlib_File_MP4_P_Input *pInput, const BMUXlib_Input_Descriptor *pInDesc)
{
   /* ensure the metatdata base address is valid, if not, ignore this descriptor */
   if (NULL == pInDesc->pBaseAddress)
   {
      BDBG_WRN(("Metadata base address not set: Ignoring metadata descriptor ..."));
      return;
   }

   /* read the data unit type to determine the metadata type */
   if (BAVC_VideoMetadataType_eCommon == BMUXLIB_INPUT_DESCRIPTOR_VIDEO_DATA_UNIT_TYPE(pInDesc))
   {
      /* process this metadata (the descriptor's size and offset give the location) */
      BAVC_VideoMetadataDescriptor *pMetadata = (BAVC_VideoMetadataDescriptor *)BMUXLIB_INPUT_DESCRIPTOR_VIRTUAL_ADDRESS(pInDesc);

      BMUXlib_File_MP4_P_TrackInfo *pTrack = pInput->pTrack;
      bool bChanged = false;

      BDBG_MODULE_MSG(BMUX_MP4_IN_DESC, ("Video Metadata: %dx%d (%d), fr:%d (%d), max br: %d (%d)", pMetadata->stDimension.coded.uiWidth, pMetadata->stDimension.coded.uiHeight,
            BMUXLIB_FILE_MP4_P_VIDEO_METADATA_IS_DIMENSION_VALID(pMetadata),
            pMetadata->stFrameRate.eFrameRateCode, BMUXLIB_FILE_MP4_P_VIDEO_METADATA_IS_FRAMERATE_VALID(pMetadata),
            pMetadata->stBitrate.uiMax, BMUXLIB_FILE_MP4_P_VIDEO_METADATA_IS_BITRATE_VALID(pMetadata)));

      if (BMUXLIB_FILE_MP4_P_VIDEO_METADATA_IS_DIMENSION_VALID(pMetadata))
      {
         if ((pMetadata->stDimension.coded.uiWidth != pTrack->uiWidth) && (0 != pTrack->uiWidth))
         {
            bChanged = true;
            BDBG_WRN(("Video metadata field (Width) changed (%d -> %d)", pTrack->uiWidth, pMetadata->stDimension.coded.uiWidth));
         }
         if ((pMetadata->stDimension.coded.uiHeight != pTrack->uiHeight) && (0 != pTrack->uiHeight))
         {
            bChanged = true;
            BDBG_WRN(("Video metadata field (uiHeight) changed (%d -> %d)", pTrack->uiHeight, pMetadata->stDimension.coded.uiHeight));
         }
         if (!bChanged)
         {
            pTrack->uiWidth = pMetadata->stDimension.coded.uiWidth;
            pTrack->uiHeight = pMetadata->stDimension.coded.uiHeight;
         }
      }
      /* ignore frame rate for now - not useful unless provided PRIOR to start() */

      if (BMUXLIB_FILE_MP4_P_VIDEO_METADATA_IS_BITRATE_VALID(pMetadata))
      {
         if ((pMetadata->stBitrate.uiMax != pTrack->uiMaxBitrate) && (0 != pTrack->uiMaxBitrate))
         {
            bChanged = true;
            BDBG_WRN(("Video metadata field (Max. Bitrate) changed (%d -> %d)", pTrack->uiMaxBitrate, pMetadata->stBitrate.uiMax));
         }
         else
            pTrack->uiMaxBitrate = pMetadata->stBitrate.uiMax;
      }
      if (bChanged)
         BDBG_WRN(("MP4 Mux does not support changing metadata"));
   }
   else
   {
      /* else, unknown metadata, so ignore this descriptor */
      BDBG_MODULE_MSG(BMUX_MP4_IN_DESC, ("Unrecognized video metadata type (%d) detected - ignoring descriptor", BMUXLIB_INPUT_DESCRIPTOR_VIDEO_DATA_UNIT_TYPE(pInDesc)));
   }
}

/****************************************
   Input Descriptor Processing (Audio)
*****************************************/

/*
   Process incoming decriptors from an audio input

   NOTE: this routine expects that at least one input descriptor is available before entering
   It also expects that the current input descriptor is the descriptor that has SOF set and
   has the relevant frame information (PTS/DTS etc).

   This will process one complete sample consisting of all the descriptors it detects on the
   specified input (including any metadata descriptors) until it sees the next descriptor
   that has a SOF.

   The location referenced by peBlocked will indicate whether this routine was blocked waiting
   for a resource.

   *** VERY IMPORTANT *** This code must cope with the possibility that ANY sample or data unit
   can be split into any number of input descriptors.

   Returns:
      BERR_SUCCESS - audio descriptors processed successfully
      else error from an interface API, or bad state.
*/
static BERR_Code ProcessAudioDescriptors(BMUXlib_File_MP4_Handle hMP4Mux, BMUXlib_File_MP4_P_BlockedCause *peBlocked)
{
   BERR_Code rc = BERR_SUCCESS;
   BMUXlib_File_MP4_P_CurrentSample * const pCurrentSample = &hMP4Mux->stCurrentSample;
   BMUXlib_File_MP4_P_Input * const pCurrentInput = pCurrentSample->pInput;
   BMUXlib_File_MP4_P_AudioState eState = pCurrentSample->eState.Audio;
   /* data from input descriptors always goes to the mdat unless determined otherwise ... */
   BMUXlib_File_MP4_P_Output *pOutput = hMP4Mux->pMdatOutput;
   BMUXlib_Input_Descriptor *pInDesc = &pCurrentSample->stInDesc;
   BMUXlib_File_MP4_P_OutputCallbackData *pCallbackData;
   bool bDescAvail;

   /* process descriptors until either all available input descriptors processed,
      run out of output descriptors, or the end of the current sample detected */
   while ((BMUXlib_File_MP4_P_BlockedCause_eNone == *peBlocked)         /* we're not blocked ... */
            && !pCurrentSample->bComplete                               /* current sample is not complete ... */
            && (BERR_SUCCESS == rc))                                    /* and no error ... */
   {
      BDBG_MODULE_MSG(BMUX_MP4_STATE, ("Processing Audio Descriptors (state: %d)", eState));
      switch (eState)
      {
         case BMUXlib_File_MP4_P_AudioState_eSampleStart:
         {
            /* start processing a new audio sample - we come here after performing a new input search
               hence this descriptor represents the *start* of the sample to process */

            /* peek at the first descriptor of this sample
               NOTE: we do not do a "get" since the get will automatically be performed when the
               descriptor has been processed */
            bDescAvail = BMUXlib_Input_PeekAtNextDescriptor(pCurrentInput->hInput, pInDesc);
            BDBG_ASSERT(bDescAvail == true);

            PrintAudioDescriptor("Sample Start", pInDesc);

            pCurrentInput->pTrack->uiSampleCount++;

            /* create the initial metadata for this sample, based on data from the "Frame start" descriptor */
            InitializeSampleMetadata(pCurrentSample, pInDesc, BMUXlib_Output_GetEndOffset(pOutput->hOutput));

            /* prepare for processing of this new descriptor (if its a metadata descriptor, then process its contents) */
            AudioNewDescriptor(hMP4Mux, pInDesc);
            eState = BMUXlib_File_MP4_P_AudioState_ePopulateOutDesc;
            break;
         }

         case BMUXlib_File_MP4_P_AudioState_ePopulateOutDesc:
         {
            uint32_t uiLength;                        /* input data length */
            uint8_t *pAddress;                        /* input data address */

            /* check that there is an output descriptor available ... */
            if (!BMUXlib_Output_IsSpaceAvailable(pOutput->hOutput))
            {
               *peBlocked = BMUXlib_File_MP4_P_BlockedCause_eOutput;   /* out of descriptors */
               break;                                 /* wait until we have more available */
            }
            /* check that there are release q entries available (each output descriptor also requires a release Q entry
               when the associated callback is invoked */
            if (hMP4Mux->pReleaseQFreeList == NULL)
            {
               *peBlocked = BMUXlib_File_MP4_P_BlockedCause_eOutput;   /* out of release Q entries */
               break;                                 /* wait until we have more available */
            }

            /* fill in an output descriptor to feed this input descriptor's Raw contents to the output storage
               NOTE: Be aware that this is using RawDataLength, not stCommon.uiLength - this is only outputting
               the part of the descriptor contents that represents the RAW data, any encapsulation is auto-
               matically skipped */
            uiLength = BMUXLIB_INPUT_DESCRIPTOR_LENGTH(pInDesc) - pCurrentSample->uiBytesSkipped;
            pAddress = (uint8_t *)BMUXLIB_INPUT_DESCRIPTOR_VIRTUAL_ADDRESS(pInDesc) + pCurrentSample->uiBytesSkipped;

            BDBG_MODULE_MSG(BMUX_MP4_OUTPUT, ("Audio: %d bytes (%d bytes skipped)", uiLength, pCurrentSample->uiBytesSkipped));

            /* else, fill in the descriptor from the input descriptor's information */
            pCallbackData = BMUXlib_File_MP4_P_NewOutputCallbackData(hMP4Mux, pCurrentInput, pCurrentInput->uiDescSeqCount);
            if (NULL == pCallbackData)
            {
               /* no more callback data entries available
                  (should not happen, since this implies there would be no output descriptors
                  available either) */
               *peBlocked = BMUXlib_File_MP4_P_BlockedCause_eCallbackData;
               break;                                 /* wait until we have more available */
            }
            InitOutputCallback(hMP4Mux, BMUXlib_File_MP4_P_OutputCallback_eInput, pCallbackData);
            /* append this data to the mdat ... */
            DebugSetDescriptorSource(pOutput, "Audio");
            BMUXlib_File_MP4_P_OutputDescriptorAppend(hMP4Mux, pOutput, pAddress, uiLength, BMUXlib_File_MP4_P_OutputCallback_eInput);

            pCurrentSample->stMetadata.uiSampleSize += uiLength;
            pCurrentInput->uiWaitingCount++;
            pCurrentInput->uiDescSeqCount++;

            /* finished with this descriptor (fetch next descriptors) ... */
            /* NOTE: this simply discards the descriptor that was peeked at previously,
               and has since been processed */
            bDescAvail = BMUXlib_Input_GetNextDescriptor(pCurrentInput->hInput, pInDesc);
            BDBG_ASSERT(bDescAvail == true); /* there must always be a descriptor to "get" */

            eState = BMUXlib_File_MP4_P_AudioState_eNextDescriptor;
            break;
         }

         case BMUXlib_File_MP4_P_AudioState_eNextDescriptor:
            if (!BMUXlib_Input_IsDescriptorAvailable(pCurrentInput->hInput))
            {
               *peBlocked = BMUXlib_File_MP4_P_BlockedCause_eInput;   /* out of descriptors */
               break;
            }
            bDescAvail = BMUXlib_Input_PeekAtNextDescriptor(pCurrentInput->hInput, pInDesc);
            BDBG_ASSERT(bDescAvail == true);

            PrintAudioDescriptor("Next Desc", pInDesc);

            /* Detect the EOS here, but this does not complete the sample until we run out of descriptors.
               For the case where EOS on last descriptor in the stream, the data from the EOS decriptor is part
               of the sample, so it will get processed accordingly next iteration.
               For the "empty" EOS case, the descriptor will be consumed creating a NOP output descriptor
               In both cases, the "end of sample" is detected by the outer loop, and it will complete the sample then */
            if (!pCurrentSample->pInput->bEOS)
               pCurrentSample->pInput->bEOS = BMUXLIB_INPUT_DESCRIPTOR_IS_EOS(pInDesc);

            /* is this the end of the current sample (i.e. start of next sample detected)? */
            /* NOTE: This assumes empty frame descriptors ONLY occur between frames */
            if (BMUXLIB_FILE_MP4_P_IS_SAMPLE_END(pInDesc))
            {
               /* NOTE: do not consume this descriptor - it will be processed
                  when this sample is selected in the future */
               pCurrentSample->bComplete = true;
               /* update the metadata for the current sample ... */
               UpdateSampleMetadata(pCurrentSample, pInDesc);
               /* NOTE: There is no point setting the state to "sample start" here since this applies to the current sample
                  (which is done), so the next state applies to the new sample, which may be from a different input
                  This is taken care of by the outer loop when it detects a new sample to process */
            }
            else
            {
               /* prepare for processing of this new descriptor (if its a metadata descriptor, then process its contents) */
               AudioNewDescriptor(hMP4Mux, pInDesc);
               eState = BMUXlib_File_MP4_P_AudioState_ePopulateOutDesc; /* process this next descriptor in the current sample ... */
            }
            break;

         default:
            /* should not happen */
            BDBG_ERR(("Bad Audio State detected: %d", eState));
            rc = BERR_TRACE(BERR_UNKNOWN);
            break;
      } /* end: switch audio state */
   } /* end while descriptors available, not error and not blocked */

   pCurrentSample->eState.Audio = eState;                /* update current state ... */
   return rc;
}

/* process an audio metadata descriptor, and set the relevant track information
   for the track corresponding to this input */
static void ProcessAudioMetadataDescriptor(const BMUXlib_File_MP4_P_Input *pInput, const BMUXlib_Input_Descriptor *pInDesc)
{
   /* ensure the metatdata base address is valid, if not, ignore this descriptor */
   if (NULL == pInDesc->pBaseAddress)
   {
      BDBG_WRN(("Metadata base address not set: Ignoring metadata descriptor ..."));
      return;
   }

   /* read the data unit type to determine the metadata type */
   if (BAVC_AudioMetadataType_eCommon == BMUXLIB_INPUT_DESCRIPTOR_AUDIO_DATA_UNIT_TYPE(pInDesc))
   {
      /* process this metadata (the descriptor's size and offset give the location) */
      BAVC_AudioMetadataDescriptor *pMetadata = (BAVC_AudioMetadataDescriptor *)BMUXLIB_INPUT_DESCRIPTOR_VIRTUAL_ADDRESS(pInDesc);
      BMUXlib_File_MP4_P_TrackInfo *pTrack = pInput->pTrack;
      bool bChanged = false;

      BDBG_MODULE_MSG(BMUX_MP4_IN_DESC, ("Audio Metadata: max br: %d (%d), sf:%d (%d), asc:%02x%02x%02x%02x%02x%02x%02x%02x [%d,%d] (%d)",
               pMetadata->stBitrate.uiMax, BMUXLIB_FILE_MP4_P_AUDIO_METADATA_IS_BITRATE_VALID(pMetadata),
               pMetadata->stSamplingFrequency.uiSamplingFrequency, BMUXLIB_FILE_MP4_P_AUDIO_METADATA_IS_SAMPLINGFREQ_VALID(pMetadata),
               pMetadata->uProtocolData.stAac.auiASC[0],
               pMetadata->uProtocolData.stAac.auiASC[1],
               pMetadata->uProtocolData.stAac.auiASC[2],
               pMetadata->uProtocolData.stAac.auiASC[3],
               pMetadata->uProtocolData.stAac.auiASC[4],
               pMetadata->uProtocolData.stAac.auiASC[5],
               pMetadata->uProtocolData.stAac.auiASC[6],
               pMetadata->uProtocolData.stAac.auiASC[7],
               pMetadata->uProtocolData.stAac.uiASCLengthBits,
               pMetadata->uProtocolData.stAac.uiASCLengthBytes,
               BMUXLIB_FILE_MP4_P_AUDIO_METADATA_IS_PROTOCOLDATA_VALID(pMetadata)
               ));

      if (BMUXLIB_FILE_MP4_P_AUDIO_METADATA_IS_BITRATE_VALID(pMetadata))
      {
         if ((pMetadata->stBitrate.uiMax != pTrack->uiMaxBitrate) && (0 != pTrack->uiMaxBitrate))
         {
            bChanged = true;
            BDBG_WRN(("Audio metadata field (max bitrate) changed (%d -> %d)", pTrack->uiMaxBitrate, pMetadata->stBitrate.uiMax));
         }
         else
            pTrack->uiMaxBitrate = pMetadata->stBitrate.uiMax;
      }

      /* Note: Sample Rate sets the timescale, and timescale is used to scale the deltaDTS for every sample.
               Therefore, this metadata is required BEFORE any samples are processed. */
      if (BMUXLIB_FILE_MP4_P_AUDIO_METADATA_IS_SAMPLINGFREQ_VALID(pMetadata))
      {
         if ((pMetadata->stSamplingFrequency.uiSamplingFrequency != pTrack->uiSampleRate) && (0 != pTrack->uiSampleRate))
         {
            bChanged = true;
            BDBG_WRN(("Audio metadata field (sample rate) changed (%d -> %d)", pTrack->uiSampleRate, pMetadata->stSamplingFrequency.uiSamplingFrequency));
         }
         else
            pTrack->uiTimescale = pTrack->uiSampleRate = pMetadata->stSamplingFrequency.uiSamplingFrequency;
      }
      else
         if (0 == pTrack->uiTimescale)
         {
            BDBG_WRN(("Audio Timescale not set - expecting metadata descriptor prior to first sample. Using default timescale (%d) - output may be unplayable", BMUXLIB_FILE_MP4_P_DEFAULT_AUDIO_SAMPLE_RATE));
            pTrack->uiTimescale = pTrack->uiSampleRate = BMUXLIB_FILE_MP4_P_DEFAULT_AUDIO_SAMPLE_RATE;
         }

      if (BMUXLIB_FILE_MP4_P_AUDIO_METADATA_IS_PROTOCOLDATA_VALID(pMetadata))
      {
         uint32_t uiLength = pMetadata->uProtocolData.stAac.uiASCLengthBytes;
         /* NOTE: for audio, only the ASC Bytes are stored in aDecSpecInfo, so ensure that
            we do not exceed the max size of this array.  Length should be the min of:
            dec spec storage and ASC data from descriptor */
         uint32_t uiMaxLength = (BMUXLIB_FILE_MP4_P_MAX_DEC_SPEC_SIZE > sizeof(pMetadata->uProtocolData.stAac.auiASC))?
                           sizeof(pMetadata->uProtocolData.stAac.auiASC):BMUXLIB_FILE_MP4_P_MAX_DEC_SPEC_SIZE;
         if (uiLength > uiMaxLength)
         {
            BDBG_WRN(("Audio Decoder Specfic Data exceeds storage (%d > %d) - truncating (output file may be corrupt)", uiLength, uiMaxLength));
            uiLength = uiMaxLength;
         }

         if (0 != pTrack->uiDecSpecInfoSize)
         {
            /* dec spec info already set, so check if it changed ... */
            if (uiLength != pTrack->uiDecSpecInfoSize)
            {
               bChanged = true;
               BDBG_WRN(("Audio Decoder Specific Information changed (%d bytes -> %d)", pTrack->uiDecSpecInfoSize, uiLength));
            }
            else
            {
               /* sizes are the same, so check contents ... */
               if (0 != (BKNI_Memcmp(pTrack->aDecSpecInfo, pMetadata->uProtocolData.stAac.auiASC, uiLength)))
               {
                  bChanged = true;
                  BDBG_WRN(("Audio Decoder Specific Information of %d bytes changed", uiLength));
               }
               /* otherwise, its good! (no change) */
            }
         }
         else
         {
            /* decoder specific information not yet set, so copy into storage ... */
            BKNI_Memcpy(pTrack->aDecSpecInfo, pMetadata->uProtocolData.stAac.auiASC, uiLength);
            pTrack->uiDecSpecInfoSize = uiLength;
         }
      }
      if (bChanged)
         BDBG_WRN(("MP4 Mux does not support changing metadata"));
   }
   else
   {
      /* else, unknown metadata, so ignore this descriptor */
      BDBG_MODULE_MSG(BMUX_MP4_IN_DESC, ("Unrecognized audio metadata type (%d) detected - ignoring descriptor", BMUXLIB_INPUT_DESCRIPTOR_AUDIO_DATA_UNIT_TYPE(pInDesc)));
   }
}

static void AudioNewDescriptor(BMUXlib_File_MP4_Handle hMP4Mux, BMUXlib_Input_Descriptor *pInDesc)
{
   BMUXlib_File_MP4_P_CurrentSample * const pCurrentSample = &hMP4Mux->stCurrentSample;

   /* see if this is a metadata descriptor */
   if (BMUXLIB_INPUT_DESCRIPTOR_IS_METADATA(pInDesc))
   {
      /* if so, process the metadata contents, then discard the descriptor ... */
      ProcessAudioMetadataDescriptor(pCurrentSample->pInput, pInDesc);
      pCurrentSample->uiBytesSkipped = BMUXLIB_INPUT_DESCRIPTOR_LENGTH(pInDesc);
      /* go create a "NOP" output descriptor to consume this input descriptor */
   }
   else
   {
      /* new descriptor, so reset the bytes skipped count.
         here, the number of bytes to skip is the offset to the raw data (i.e. is the number of
         encapsulation bytes) */
      pCurrentSample->uiBytesSkipped = BMUXLIB_INPUT_DESCRIPTOR_LENGTH(pInDesc) - BMUXLIB_INPUT_DESCRIPTOR_AUDIO_RAWLENGTH(pInDesc);
      if ((BMUXLIB_INPUT_DESCRIPTOR_AUDIO_RAWOFFSET(pInDesc) - BMUXLIB_INPUT_DESCRIPTOR_OFFSET(pInDesc)) != pCurrentSample->uiBytesSkipped)
         BDBG_WRN(("Audio Descriptor: Offsets do not match length (delta offsets = %d, delta length = %d)",
               BMUXLIB_INPUT_DESCRIPTOR_AUDIO_RAWOFFSET(pInDesc) - BMUXLIB_INPUT_DESCRIPTOR_OFFSET(pInDesc), pCurrentSample->uiBytesSkipped));
   }
}

/*********************************
   Storage Descriptor Processing
*********************************/
/*
   Function:
      IsOutputQueueClear

   Assert that the output descriptors queue has been left in a clean state -
   i.e. all descriptors are free and that the dependent data referenced by each
   descriptor has been released (ensures no memory leaks)
*/
static bool IsOutputQueueClear(BMUXlib_File_MP4_Handle hMP4Mux)
{
   uint32_t uiOutputIndex;
   bool bClear;

   BDBG_ENTER(IsOutputQueueClear);
      for (uiOutputIndex = 0; uiOutputIndex < BMUXLIB_FILE_MP4_P_MAX_ACTIVE_OUTPUTS; uiOutputIndex++)
         if (NULL != hMP4Mux->aActiveOutputs[uiOutputIndex].hOutput)
            if (BMUXlib_Output_IsDescriptorPendingCompletion(hMP4Mux->aActiveOutputs[uiOutputIndex].hOutput))
               break;
   bClear = (uiOutputIndex == BMUXLIB_FILE_MP4_P_MAX_ACTIVE_OUTPUTS);  /* done if all outputs completed */

   /* make sure release queue finished ... */
   if (bClear && (hMP4Mux->stBoxBufferReleaseQ.pHead != NULL))
      bClear = false; /* release Q not done */

   BDBG_LEAVE(IsOutputQueueClear);
   return bClear;
}

/*****************************
  Completed Output Callbacks
*****************************/

/* Process the returned output descriptors that refer to box buffer data
   Since these descriptors can be completed out-of-order relative to the
   order in which they were created, construct a list of descriptors that
   are complete, which can then be traversed later to free the box buffer
   resources in the correct order */
static void ProcessCompletedOutputDescriptors_BoxBuffer(BMUXlib_File_MP4_Handle hMP4Mux, const BMUXlib_Output_Descriptor *pOutDesc)
{
   BDBG_MODULE_MSG(BMUX_MP4_RELEASEQ, ("Queueing box buffer entry (%d bytes @ %p) for release", pOutDesc->stStorage.uiLength, pOutDesc->stStorage.pBufferAddress));

   /* NOTE: Sequence ID not used (read pointer used to locate the entry to free) */
   ReleaseQAddEntry(hMP4Mux, &hMP4Mux->stBoxBufferReleaseQ, pOutDesc->stStorage.pBufferAddress, pOutDesc->stStorage.uiLength, 0);
}


/*
   Process the returned output descriptors that refer to size entries

   Note: since size entries are independently maintained, we can
   directly free the entry in this callback (no out-of-order issues
   to worry about, so no release queue required)
*/
static void ProcessCompletedOutputDescriptors_SizeEntry(BMUXlib_File_MP4_P_OutputCallbackData *pCallbackData, const BMUXlib_Output_Descriptor *pOutDesc)
{
   BMUXlib_File_MP4_Handle hMP4Mux = pCallbackData->hMux;
   uint32_t *pSizeEntry = pCallbackData->pData;
   /* sequence count not used */

   BDBG_MODULE_MSG(BMUX_MP4_RELEASEQ, ("Freeing Size Entry: pAddr = %p, size = %d @ %lld",
         pOutDesc->stStorage.pBufferAddress, pOutDesc->stStorage.uiLength, pOutDesc->stStorage.uiOffset));
   /* return the callback data entry to the free list */
   BMUXlib_File_MP4_P_FreeOutputCallbackData(hMP4Mux, pCallbackData);
   *pSizeEntry = BMUXLIB_FILE_MP4_P_SIZE_ENTRY_UNUSED;      /* this size entry no longer in use */
#if BDBG_DEBUG_BUILD
   hMP4Mux->stSizes.uiUsageCount--;
#endif
}

/*
   Process the returned output descriptors that refer to input descriptors

   Since these descriptors may be completed in a different order in which
   they were queued (for example, if they go to different outputs), we need
   to construct a list of the completed descriptors so that they can be
   freed in the correct order.

   The sequence ID is used to maintain the correct release order.
*/
static void ProcessCompletedOutputDescriptors_Input(BMUXlib_File_MP4_P_OutputCallbackData *pCallbackData, const BMUXlib_Output_Descriptor *pOutDesc)
{
   BMUXlib_File_MP4_Handle hMP4Mux = pCallbackData->hMux;
   BMUXlib_File_MP4_P_Input *pInput = pCallbackData->pData;

   BDBG_MODULE_MSG(BMUX_MP4_RELEASEQ, ("Queueing input descriptors (%d bytes @ %p, seq: %d) for release",
         pOutDesc->stStorage.uiLength, pOutDesc->stStorage.pBufferAddress, pCallbackData->uiSequenceID));

   /* return the callback data entry to the free list */
   BMUXlib_File_MP4_P_FreeOutputCallbackData(hMP4Mux, pCallbackData);
   /* release the input descriptor associated with this data */
   ReleaseQAddEntry(hMP4Mux, &pInput->stReleaseQ, pOutDesc->stStorage.pBufferAddress, pOutDesc->stStorage.uiLength, pCallbackData->uiSequenceID);
}

/* add the resource to be freed to the specified release queue with the indicated sequence number */
static void ReleaseQAddEntry(BMUXlib_File_MP4_Handle hMP4Mux, BMUXlib_File_MP4_P_ReleaseQ *pReleaseQ,
   uint8_t *pSourceData, uint32_t uiSourceLength, uint32_t uiSequenceID)
{
   BMUXlib_File_MP4_P_ReleaseQEntry *pEntry;

   /* fetch a new entry from the free list */
   BDBG_ASSERT(hMP4Mux->pReleaseQFreeList != NULL);   /* check free list is not empty! */
   pEntry = hMP4Mux->pReleaseQFreeList;
   hMP4Mux->pReleaseQFreeList = pEntry->pNext;
#if BDBG_DEBUG_BUILD
   hMP4Mux->uiReleaseQUsageCount++;
   if (hMP4Mux->uiReleaseQUsageCount > hMP4Mux->uiReleaseQMaxUsage)
      hMP4Mux->uiReleaseQMaxUsage = hMP4Mux->uiReleaseQUsageCount;
#endif

   /* add new entry to the tail of the queue */
   pEntry->pSourceData = pSourceData;
   pEntry->uiSourceLength = uiSourceLength;
   pEntry->uiSequenceID = uiSequenceID;
   pEntry->pNext = NULL;
   if (NULL == pReleaseQ->pHead)
      /* queue is empty, so this is the one and only node */
      pReleaseQ->pTail = pReleaseQ->pHead = pEntry;
   else
   {
      pReleaseQ->pTail->pNext = pEntry;
      pReleaseQ->pTail = pEntry;
   }
}

/* NOTE: the metadata callback MUST NOT use release queues or callback
   data unless the quantity of each is changed to include this extra
   usage (should not be necessary) */
/*
   Process the returned output descriptors that refer to metadata cache

   NOTE: since each metadata cache goes to its own unique output, there
   is no danger of out-of-order return of the descriptors, so we can
   "free" the relevant metadata cache here
*/
static void ProcessCompletedOutputDescriptors_MetadataCache(BMUXlib_File_MP4_P_MetadataCache *pMetadataCache, const BMUXlib_Output_Descriptor *pOutDesc)
{
   BDBG_MODULE_MSG(BMUX_MP4_RELEASEQ, ("Freeing Metadata: pAddr = %p, size = %d @ %lld",
         pOutDesc->stStorage.pBufferAddress, pOutDesc->stStorage.uiLength, pOutDesc->stStorage.uiOffset));
   BMUXlib_File_MP4_P_FreeMetadataCache(pMetadataCache);
}

/**********************
  Source Data Freeing
**********************/

/*
   Go thru the release queues for all the inputs, and return the descriptors
   that have been processed back to the input.  Use the sequence ID to determine
   which descriptors to return and which to hold onto (if any)

   For example, if descriptors 1,2,4 and 5 go to output 1, and descriptor
   3 goes to output 2, but output 2 is blocked for some reason, descriptors
   1,2,4 and 5 are returned, but we can only free descriptors 1 and 2
   (descriptor 3 is still in-use) - descriptors 4 and 5 must wait to be
   released back to the input until descriptor 3 is completed.
*/
static BERR_Code ProcessInputReleaseQueues(BMUXlib_File_MP4_Handle hMP4Mux)
{
   BERR_Code rc = BERR_SUCCESS;
   uint32_t uiInputIndex;

   for (uiInputIndex = 0; (uiInputIndex < BMUXLIB_FILE_MP4_P_MAX_ACTIVE_INPUTS) && (BERR_SUCCESS == rc); uiInputIndex++)
   {
      BMUXlib_File_MP4_P_Input *pInput = &hMP4Mux->aActiveInputs[uiInputIndex];
      uint32_t uiExpectedSeqCount = pInput->uiReleaseSeqCount;
      uint32_t uiDescFreeCount = 0;
      bool bDone = false;

      if (NULL == pInput->hInput)
         continue;         /* skip invalid or inactive inputs */

      BDBG_MODULE_MSG(BMUX_MP4_RELEASEQ, ("Input %d: Looking for sequence %d", uiInputIndex, uiExpectedSeqCount));
      while (!bDone)
      {
         BMUXlib_File_MP4_P_ReleaseQEntry *pEntry = pInput->stReleaseQ.pHead;
         BMUXlib_File_MP4_P_ReleaseQEntry **ppPrevNext = &pInput->stReleaseQ.pHead;

         /* traverse the list to find an entry that matches the expected sequence count
            => this is the entry that can be freed */
         while((pEntry != NULL) && (pEntry->uiSequenceID != uiExpectedSeqCount))
         {
            BDBG_MODULE_MSG(BMUX_MP4_RELEASEQ, ("Current SeqCount: %d", pEntry->uiSequenceID));
            ppPrevNext = &pEntry->pNext;
            pEntry = pEntry->pNext;
         }
         if (pEntry != NULL)
         {
            /* descriptor to be freed found */
            BDBG_MODULE_MSG(BMUX_MP4_RELEASEQ, ("Freeing Input Descriptor for Input %d: pAddr = %p, %d bytes, seq: %d",
               uiInputIndex, pEntry->pSourceData, pEntry->uiSourceLength, uiExpectedSeqCount));
            uiDescFreeCount++;
            uiExpectedSeqCount++;
            /* unlink the descriptor and return it to the free list */
            *ppPrevNext = pEntry->pNext;
            pEntry->pNext = hMP4Mux->pReleaseQFreeList;
            hMP4Mux->pReleaseQFreeList = pEntry;
#if BDBG_DEBUG_BUILD
            hMP4Mux->uiReleaseQUsageCount--;
#endif
         }
         else
            /* descriptor for current read pointer has not been completed yet, so we wait */
            bDone = true;
      } /* end: while not done */
      if (0 != uiDescFreeCount)
      {
         BDBG_MODULE_MSG(BMUX_MP4_INPUT, ("Freeing %d descriptors from input %d", uiDescFreeCount, uiInputIndex));
         rc = BERR_TRACE(BMUXlib_Input_ConsumeDescriptors(pInput->hInput, uiDescFreeCount));
         pInput->uiWaitingCount -= uiDescFreeCount;
         pInput->uiReleaseSeqCount = uiExpectedSeqCount;
      } /* end: decriptors to free */
   } /* end: for each input */
   return rc;
}

/*
   go thru the release queue for the box buffer and move the box buffer
   read pointer (i.e. free the data) for any descriptors that match the
   read location (i.e. this location is complete).

   Data must be freed in the order in which it was created.
*/
static void ProcessBoxBufferReleaseQueue(BMUXlib_File_MP4_Handle hMP4Mux)
{
   bool bDone = false;
   while (!bDone)
   {
      BMUXlib_File_MP4_P_ReleaseQEntry *pEntry = hMP4Mux->stBoxBufferReleaseQ.pHead;
      BMUXlib_File_MP4_P_ReleaseQEntry **ppPrevNext = &hMP4Mux->stBoxBufferReleaseQ.pHead;
      /* traverse the list to find an entry that matches the box-buffer's read address
         => this is the entry that can be freed */
      while((pEntry != NULL) && (pEntry->pSourceData != hMP4Mux->stBoxBuffer.pRead))
      {
         ppPrevNext = &pEntry->pNext;
         pEntry = pEntry->pNext;
      }
      if (pEntry != NULL)
      {
         /* descriptor to be freed found */
         BDBG_MODULE_MSG(BMUX_MP4_RELEASEQ, ("Freeing Box Buffer Descriptor: pAddr = %p, %d bytes",
            pEntry->pSourceData, pEntry->uiSourceLength));
         BMUXlib_File_MP4_P_FreeBoxBufferData(&hMP4Mux->stBoxBuffer, pEntry->pSourceData,
               pEntry->uiSourceLength);
         /* unlink the descriptor and return it to the free list */
         *ppPrevNext = pEntry->pNext;
         pEntry->pNext = hMP4Mux->pReleaseQFreeList;
         hMP4Mux->pReleaseQFreeList = pEntry;
#if BDBG_DEBUG_BUILD
         hMP4Mux->uiReleaseQUsageCount--;
#endif
      }
      else
         /* descriptor for current read pointer has not been completed yet, so we wait */
         bDone = true;
   }
}

/*******************
    D E B U G
*******************/

#if BDBG_DEBUG_BUILD
static void PrintFinalizationPercentComplete(BMUXlib_File_MP4_Handle hMP4Mux)
{
   static uint32_t uiPrevPctComplete = 0;
   uint32_t uiCurrentPctComplete;
   uint32_t uiAccuracy = (hMP4Mux->stStatus.uiFinishStepsTotal > 1000)?1000:100;
   uiCurrentPctComplete = (hMP4Mux->stStatus.uiFinishStepsTotal)?((hMP4Mux->stStatus.uiFinishStepsComplete * uiAccuracy) / hMP4Mux->stStatus.uiFinishStepsTotal):0;
   if (uiCurrentPctComplete != uiPrevPctComplete)
   {
      if (uiAccuracy != 100)
         BDBG_MODULE_MSG(BMUX_MP4_FINISH, ("Finalizing ... %d.%d%% done (current state: %d)", uiCurrentPctComplete/10, uiCurrentPctComplete%10, hMP4Mux->eBoxState));
      else
         BDBG_MODULE_MSG(BMUX_MP4_FINISH, ("Finalizing ... %d%% done (current state: %d)", uiCurrentPctComplete, hMP4Mux->eBoxState));
      uiPrevPctComplete = uiCurrentPctComplete;
   }
}
#endif

/*****************************************************************************
* EOF
******************************************************************************/
