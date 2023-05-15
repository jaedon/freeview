// Copyright (c) 2010 Google Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

// minidump.h: A minidump reader.
//
// The basic structure of this module tracks the structure of the minidump
// file itself.  At the top level, a minidump file is represented by a
// Minidump object.  Like most other classes in this module, Minidump
// provides a Read method that initializes the object with information from
// the file.  Most of the classes in this file are wrappers around the
// "raw" structures found in the minidump file itself, and defined in
// minidump_format.h.  For example, each thread is represented by a
// MinidumpThread object, whose parameters are specified in an MDRawThread
// structure.  A properly byte-swapped MDRawThread can be obtained from a
// MinidumpThread easily by calling its thread() method.
//
// Most of the module lazily reads only the portion of the minidump file
// necessary to fulfill the user's request.  Calling Minidump::Read
// only reads the minidump's directory.  The thread list is not read until
// it is needed, and even once it's read, the memory regions for each
// thread's stack aren't read until they're needed.  This strategy avoids
// unnecessary file input, and allocating memory for data in which the user
// has no interest.  Note that although memory allocations for a typical
// minidump file are not particularly large, it is possible for legitimate
// minidumps to be sizable.  A full-memory minidump, for example, contains
// a snapshot of the entire mapped memory space.  Even a normal minidump,
// with stack memory only, can be large if, for example, the dump was
// generated in response to a crash that occurred due to an infinite-
// recursion bug that caused the stack's limits to be exceeded.  Finally,
// some users of this library will unfortunately find themselves in the
// position of having to process potentially-hostile minidumps that might
// attempt to cause problems by forcing the minidump processor to over-
// allocate memory.
//
// Memory management in this module is based on a strict
// you-don't-own-anything policy.  The only object owned by the user is
// the top-level Minidump object, the creation and destruction of which
// must be the user's own responsibility.  All other objects obtained
// through interaction with this module are ultimately owned by the
// Minidump object, and will be freed upon the Minidump object's destruction.
// Because memory regions can potentially involve large allocations, a
// FreeMemory method is provided by MinidumpMemoryRegion, allowing the user
// to release data when it is no longer needed.  Use of this method is
// optional but recommended.  If freed data is later required, it will
// be read back in from the minidump file again.
//
// There is one exception to this memory management policy:
// Minidump::ReadString will return a string object to the user, and the user
// is responsible for its deletion.
//
// Author: Mark Mentovai

#ifndef GOOGLE_BREAKPAD_PROCESSOR_MINIDUMP_H__
#define GOOGLE_BREAKPAD_PROCESSOR_MINIDUMP_H__

#ifndef _WIN32
#include <unistd.h>
#endif

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "using_std_string.h"
#include "minidump_format.h"

namespace google_breakpad {

using std::map;
using std::vector;

class Minidump;
template<typename AddressType, typename EntryType> class RangeMap;


// MinidumpObject is the base of all Minidump* objects except for Minidump
// itself.
class MinidumpObject {
 public:
  virtual ~MinidumpObject() {}

  bool valid() const { return valid_; }

 protected:
  explicit MinidumpObject(Minidump* minidump);

  // Refers to the Minidump object that is the ultimate parent of this
  // Some MinidumpObjects are owned by other MinidumpObjects, but at the
  // root of the ownership tree is always a Minidump.  The Minidump object
  // is kept here for access to its seeking and reading facilities, and
  // for access to data about the minidump file itself, such as whether
  // it should be byte-swapped.
  Minidump* minidump_;

  // MinidumpObjects are not valid when created.  When a subclass populates
  // its own fields, it can set valid_ to true.  Accessors and mutators may
  // wish to consider or alter the valid_ state as they interact with
  // objects.
  bool      valid_;
};


// This class exists primarily to provide a virtual destructor in a base
// class common to all objects that might be stored in
// Minidump::mStreamObjects.  Some object types (MinidumpContext) will
// never be stored in Minidump::mStreamObjects, but are represented as
// streams and adhere to the same interface, and may be derived from
// this class.
class MinidumpStream : public MinidumpObject {
 public:
  virtual ~MinidumpStream() {}

 protected:
  explicit MinidumpStream(Minidump* minidump);

 private:
  // Populate (and validate) the MinidumpStream.  minidump_ is expected
  // to be positioned at the beginning of the stream, so that the next
  // read from the minidump will be at the beginning of the stream.
  // expected_size should be set to the stream's length as contained in
  // the MDRawDirectory record or other identifying record.  A class
  // that implements MinidumpStream can compare expected_size to a
  // known size as an integrity check.
  virtual bool Read(uint32_t expected_size) = 0;
};

// MinidumpException wraps MDRawExceptionStream, which contains information
// about the exception that caused the minidump to be generated, if the
// minidump was generated in an exception handler called as a result of
// an exception.  It also provides access to a MinidumpContext object,
// which contains the CPU context for the exception thread at the time
// the exception occurred.
class MinidumpException : public MinidumpStream {
 public:
  virtual ~MinidumpException();

  const MDRawExceptionStream* exception() const {
    return valid_ ? &exception_ : NULL;
  }

  // The thread ID is used to determine if a thread is the exception thread,
  // so a special getter is provided to retrieve this data from the
  // MDRawExceptionStream structure.  Returns false if the thread ID cannot
  // be determined.
  bool GetThreadID(uint32_t *thread_id) const;

  // Print a human-readable representation of the object to stdout.
  void Print();

 private:
  friend class Minidump;

  static const uint32_t kStreamType = MD_EXCEPTION_STREAM;

  explicit MinidumpException(Minidump* minidump);

  bool Read(uint32_t expected_size);

  MDRawExceptionStream exception_;
};

// Minidump is the user's interface to a minidump file.  It wraps MDRawHeader
// and provides access to the minidump's top-level stream directory.
class Minidump {
 public:
  // path is the pathname of a file containing the minidump.
  explicit Minidump(const string& path);
  // input is an istream wrapping minidump data. Minidump holds a
  // weak pointer to input, and the caller must ensure that the stream
  // is valid as long as the Minidump object is.
  explicit Minidump(std::istream& input);

  virtual ~Minidump();

  // path may be empty if the minidump was not opened from a file
  virtual string path() const {
    return path_;
  }
  static void set_max_streams(uint32_t max_streams) {
    max_streams_ = max_streams;
  }
  static uint32_t max_streams() { return max_streams_; }

  static void set_max_string_length(uint32_t max_string_length) {
    max_string_length_ = max_string_length;
  }
  static uint32_t max_string_length() { return max_string_length_; }

  virtual const MDRawHeader* header() const { return valid_ ? &header_ : NULL; }

  // Reads the CPU information from the system info stream and generates the
  // appropriate CPU flags.  The returned context_cpu_flags are the same as
  // if the CPU type bits were set in the context_flags of a context record.
  // On success, context_cpu_flags will have the flags that identify the CPU.
  // If a system info stream is missing, context_cpu_flags will be 0.
  // Returns true if the current position in the stream was not changed.
  // Returns false when the current location in the stream was changed and the
  // attempt to restore the original position failed.
  bool GetContextCPUFlagsFromSystemInfo(uint32_t* context_cpu_flags);

  // Reads the minidump file's header and top-level stream directory.
  // The minidump is expected to be positioned at the beginning of the
  // header.  Read() sets up the stream list and map, and validates the
  // Minidump object.
  virtual bool Read();

  // The next set of methods are stubs that call GetStream.  They exist to
  // force code generation of the templatized API within the module, and
  // to avoid exposing an ugly API (GetStream needs to accept a garbage
  // parameter).
  MinidumpException* GetException();

  // The next set of methods are provided for users who wish to access
  // data in minidump files directly, while leveraging the rest of
  // this class and related classes to handle the basic minidump
  // structure and known stream types.

  unsigned int GetDirectoryEntryCount() const {
    return valid_ ? header_.stream_count : 0;
  }
  const MDRawDirectory* GetDirectoryEntryAtIndex(unsigned int index) const;

  // The next 2 methods are lower-level I/O routines.  They use fd_.

  // Reads count bytes from the minidump at the current position into
  // the storage area pointed to by bytes.  bytes must be of sufficient
  // size.  After the read, the file position is advanced by count.
  bool ReadBytes(void* bytes, size_t count);

  // Sets the position of the minidump file to offset.
  bool SeekSet(off_t offset);

  // Returns the current position of the minidump file.
  off_t Tell();

  // The next 2 methods are medium-level I/O routines.

  // ReadString returns a string which is owned by the caller!  offset
  // specifies the offset that a length-encoded string is stored at in the
  // minidump file.
  string* ReadString(off_t offset);

  // SeekToStreamType positions the file at the beginning of a stream
  // identified by stream_type, and informs the caller of the stream's
  // length by setting *stream_length.  Because stream_map maps each stream
  // type to only one stream in the file, this might mislead the user into
  // thinking that the stream that this seeks to is the only stream with
  // type stream_type.  That can't happen for streams that these classes
  // deal with directly, because they're only supposed to be present in the
  // file singly, and that's verified when stream_map_ is built.  Users who
  // are looking for other stream types should be aware of this
  // possibility, and consider using GetDirectoryEntryAtIndex (possibly
  // with GetDirectoryEntryCount) if expecting multiple streams of the same
  // type in a single minidump file.
  bool SeekToStreamType(uint32_t stream_type, uint32_t* stream_length);

  bool swap() const { return valid_ ? swap_ : false; }

  // Print a human-readable representation of the object to stdout.
  void Print();

 private:
  // MinidumpStreamInfo is used in the MinidumpStreamMap.  It lets
  // the Minidump object locate interesting streams quickly, and
  // provides a convenient place to stash MinidumpStream objects.
  struct MinidumpStreamInfo {
    MinidumpStreamInfo() : stream_index(0), stream(NULL) {}
    ~MinidumpStreamInfo() { delete stream; }

    // Index into the MinidumpDirectoryEntries vector
    unsigned int    stream_index;

    // Pointer to the stream if cached, or NULL if not yet populated
    MinidumpStream* stream;
  };

  typedef vector<MDRawDirectory> MinidumpDirectoryEntries;
  typedef map<uint32_t, MinidumpStreamInfo> MinidumpStreamMap;

  template<typename T> T* GetStream(T** stream);

  // Opens the minidump file, or if already open, seeks to the beginning.
  bool Open();

  // The largest number of top-level streams that will be read from a minidump.
  // Note that streams are only read (and only consume memory) as needed,
  // when directed by the caller.  The default is 128.
  static uint32_t max_streams_;

  // The maximum length of a UTF-16 string that will be read from a minidump
  // in 16-bit words.  The default is 1024.  UTF-16 strings are converted
  // to UTF-8 when stored in memory, and each UTF-16 word will be represented
  // by as many as 3 bytes in UTF-8.
  static unsigned int max_string_length_;

  MDRawHeader               header_;

  // The list of streams.
  MinidumpDirectoryEntries* directory_;

  // Access to streams using the stream type as the key.
  MinidumpStreamMap*        stream_map_;

  // The pathname of the minidump file to process, set in the constructor.
  // This may be empty if the minidump was opened directly from a stream.
  const string              path_;

  // The stream for all file I/O.  Used by ReadBytes and SeekSet.
  // Set based on the path in Open, or directly in the constructor.
  std::istream*             stream_;

  // swap_ is true if the minidump file should be byte-swapped.  If the
  // minidump was produced by a CPU that is other-endian than the CPU
  // processing the minidump, this will be true.  If the two CPUs are
  // same-endian, this will be false.
  bool                      swap_;

  // Validity of the Minidump structure, false immediately after
  // construction or after a failed Read(); true following a successful
  // Read().
  bool                      valid_;
};


}  // namespace google_breakpad


#endif  // GOOGLE_BREAKPAD_PROCESSOR_MINIDUMP_H__
