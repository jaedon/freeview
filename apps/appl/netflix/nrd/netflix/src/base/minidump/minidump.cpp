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

// minidump.cc: A minidump reader.
//
// See minidump.h for documentation.
//
// Author: Mark Mentovai

#include "minidump.h"

#include <assert.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <fstream>
#include <iostream>
#include <limits>
#include <map>
#include <vector>

#include "scoped_ptr.h"

namespace google_breakpad {


using std::istream;
using std::ifstream;
using std::numeric_limits;
using std::vector;

//
// Swapping routines
//
// Inlining these doesn't increase code size significantly, and it saves
// a whole lot of unnecessary jumping back and forth.
//


// Swapping an 8-bit quantity is a no-op.  This function is only provided
// to account for certain templatized operations that require swapping for
// wider types but handle uint8_t too
// (MinidumpMemoryRegion::GetMemoryAtAddressInternal).
//static inline void Swap(uint8_t* value) {
//}


// Optimization: don't need to AND the furthest right shift, because we're
// shifting an unsigned quantity.  The standard requires zero-filling in this
// case.  If the quantities were signed, a bitmask whould be needed for this
// right shift to avoid an arithmetic shift (which retains the sign bit).
// The furthest left shift never needs to be ANDed bitmask.


static inline void Swap(uint16_t* value) {
  *value = (*value >> 8) |
           (*value << 8);
}


static inline void Swap(uint32_t* value) {
  *value =  (*value >> 24) |
           ((*value >> 8)  & 0x0000ff00) |
           ((*value << 8)  & 0x00ff0000) |
            (*value << 24);
}


static inline void Swap(uint64_t* value) {
  uint32_t* value32 = reinterpret_cast<uint32_t*>(value);
  Swap(&value32[0]);
  Swap(&value32[1]);
  uint32_t temp = value32[0];
  value32[0] = value32[1];
  value32[1] = temp;
}

// Swapping signed integers
static inline void Swap(int16_t* value) {
  Swap(reinterpret_cast<uint16_t*>(value));
}

static inline void Swap(int32_t* value) {
  Swap(reinterpret_cast<uint32_t*>(value));
}

static inline void Swap(int64_t* value) {
  Swap(reinterpret_cast<uint64_t*>(value));
}


static inline void Swap(MDLocationDescriptor* location_descriptor) {
  Swap(&location_descriptor->data_size);
  Swap(&location_descriptor->rva);
}


static inline void Swap(MDMemoryDescriptor* memory_descriptor) {
  Swap(&memory_descriptor->start_of_memory_range);
  Swap(&memory_descriptor->memory);
}


static inline void Swap(MDGUID* guid) {
  Swap(&guid->data1);
  Swap(&guid->data2);
  Swap(&guid->data3);
  // Don't swap guid->data4[] because it contains 8-bit quantities.
}

static inline void Swap(MDSystemTime* system_time) {
  Swap(&system_time->year);
  Swap(&system_time->month);
  Swap(&system_time->day_of_week);
  Swap(&system_time->day);
  Swap(&system_time->hour);
  Swap(&system_time->minute);
  Swap(&system_time->second);
  Swap(&system_time->milliseconds);
}

static inline void Swap(uint16_t* data, size_t size_in_bytes) {
  size_t data_length = size_in_bytes / sizeof(data[0]);
  for (size_t i = 0; i < data_length; i++) {
    Swap(&data[i]);
  }
}

//
// MinidumpObject
//


MinidumpObject::MinidumpObject(Minidump* minidump)
    : minidump_(minidump),
      valid_(false) {
}


//
// MinidumpStream
//


MinidumpStream::MinidumpStream(Minidump* minidump)
    : MinidumpObject(minidump) {
}

//
// MinidumpException
//

MinidumpException::MinidumpException(Minidump* minidump)
    : MinidumpStream(minidump),
      exception_() {
}


MinidumpException::~MinidumpException() {
}


bool MinidumpException::Read(uint32_t expected_size) {
  // Invalidate cached data.

  valid_ = false;

  if (expected_size != sizeof(exception_)) {
    return false;
  }

  if (!minidump_->ReadBytes(&exception_, sizeof(exception_))) {
    return false;
  }

  if (minidump_->swap()) {
    Swap(&exception_.thread_id);
    // exception_.__align is for alignment only and does not need to be
    // swapped.
    Swap(&exception_.exception_record.exception_code);
    Swap(&exception_.exception_record.exception_flags);
    Swap(&exception_.exception_record.exception_record);
    Swap(&exception_.exception_record.exception_address);
    Swap(&exception_.exception_record.number_parameters);
    // exception_.exception_record.__align is for alignment only and does not
    // need to be swapped.
    for (unsigned int parameter_index = 0;
         parameter_index < MD_EXCEPTION_MAXIMUM_PARAMETERS;
         ++parameter_index) {
      Swap(&exception_.exception_record.exception_information[parameter_index]);
    }
    Swap(&exception_.thread_context);
  }

  valid_ = true;
  return true;
}


bool MinidumpException::GetThreadID(uint32_t *thread_id) const {
  assert(thread_id);
  *thread_id = 0;

  if (!valid_) {
    return false;
  }

  *thread_id = exception_.thread_id;
  return true;
}


//
// Minidump
//


uint32_t Minidump::max_streams_ = 128;
unsigned int Minidump::max_string_length_ = 1024;


Minidump::Minidump(const string& path)
    : header_(),
      directory_(NULL),
      stream_map_(new MinidumpStreamMap()),
      path_(path),
      stream_(NULL),
      swap_(false),
      valid_(false) {
}

Minidump::Minidump(istream& stream)
    : header_(),
      directory_(NULL),
      stream_map_(new MinidumpStreamMap()),
      path_(),
      stream_(&stream),
      swap_(false),
      valid_(false) {
}

Minidump::~Minidump() {
  if (!path_.empty()) {
    delete stream_;
  }
  delete directory_;
  delete stream_map_;
}


bool Minidump::Open() {
  if (stream_ != NULL) {
    // The file is already open.  Seek to the beginning, which is the position
    // the file would be at if it were opened anew.
    return SeekSet(0);
  }

  stream_ = new ifstream(path_.c_str(), std::ios::in | std::ios::binary);
  if (!stream_ || !stream_->good()) {
    return false;
  }

  return true;
}

bool Minidump::Read() {
  // Invalidate cached data.
  delete directory_;
  directory_ = NULL;
  stream_map_->clear();

  valid_ = false;

  if (!Open()) {
    return false;
  }

  if (!ReadBytes(&header_, sizeof(MDRawHeader))) {
    return false;
  }

  if (header_.signature != MD_HEADER_SIGNATURE) {
    // The file may be byte-swapped.  Under the present architecture, these
    // classes don't know or need to know what CPU (or endianness) the
    // minidump was produced on in order to parse it.  Use the signature as
    // a byte order marker.
    uint32_t signature_swapped = header_.signature;
    Swap(&signature_swapped);
    if (signature_swapped != MD_HEADER_SIGNATURE) {
      return false;
    }
    swap_ = true;
  } else {
    // The file is not byte-swapped.  Set swap_ false (it may have been true
    // if the object is being reused?)
    swap_ = false;
  }

  if (swap_) {
    Swap(&header_.signature);
    Swap(&header_.version);
    Swap(&header_.stream_count);
    Swap(&header_.stream_directory_rva);
    Swap(&header_.checksum);
    Swap(&header_.time_date_stamp);
    Swap(&header_.flags);
  }

  if ((header_.version & 0x0000ffff) != MD_HEADER_VERSION) {
    return false;
  }

  if (!SeekSet(header_.stream_directory_rva)) {
    return false;
  }

  if (header_.stream_count > max_streams_) {
    return false;
  }

  if (header_.stream_count != 0) {
    scoped_ptr<MinidumpDirectoryEntries> directory(
        new MinidumpDirectoryEntries(header_.stream_count));

    // Read the entire array in one fell swoop, instead of reading one entry
    // at a time in the loop.
    if (!ReadBytes(&(*directory)[0],
                   sizeof(MDRawDirectory) * header_.stream_count)) {
      return false;
    }

    for (unsigned int stream_index = 0;
         stream_index < header_.stream_count;
         ++stream_index) {
      MDRawDirectory* directory_entry = &(*directory)[stream_index];

      if (swap_) {
        Swap(&directory_entry->stream_type);
        Swap(&directory_entry->location);
      }

      // Initialize the stream_map_ map, which speeds locating a stream by
      // type.
      unsigned int stream_type = directory_entry->stream_type;
      switch (stream_type) {
        case MD_THREAD_LIST_STREAM:
        case MD_MODULE_LIST_STREAM:
        case MD_MEMORY_LIST_STREAM:
        case MD_EXCEPTION_STREAM:
        case MD_SYSTEM_INFO_STREAM:
        case MD_MISC_INFO_STREAM:
        case MD_BREAKPAD_INFO_STREAM: {
          if (stream_map_->find(stream_type) != stream_map_->end()) {
            // Another stream with this type was already found.  A minidump
            // file should contain at most one of each of these stream types.
            return false;
          }
          // Fall through to default
        }

        default: {
          // Overwrites for stream types other than those above, but it's
          // expected to be the user's burden in that case.
          (*stream_map_)[stream_type].stream_index = stream_index;
        }
      }
    }

    directory_ = directory.release();
  }

  valid_ = true;
  return true;
}

MinidumpException* Minidump::GetException() {
  MinidumpException* exception;
  return GetStream(&exception);
}

const MDRawDirectory* Minidump::GetDirectoryEntryAtIndex(unsigned int index)
      const {
  if (!valid_) {
    return NULL;
  }

  if (index >= header_.stream_count) {
    return NULL;
  }

  return &(*directory_)[index];
}


bool Minidump::ReadBytes(void* bytes, size_t count) {
  // Can't check valid_ because Read needs to call this method before
  // validity can be determined.
  if (!stream_) {
    return false;
  }
  stream_->read(static_cast<char*>(bytes), count);
  std::streamsize bytes_read = stream_->gcount();
  if (bytes_read == -1) {
    return false;
  }

  // Convert to size_t and check for data loss
  size_t bytes_read_converted = static_cast<size_t>(bytes_read);
  if (static_cast<std::streamsize>(bytes_read_converted) != bytes_read) {
    return false;
  }

  if (bytes_read_converted != count) {
    return false;
  }

  return true;
}


bool Minidump::SeekSet(off_t offset) {
  // Can't check valid_ because Read needs to call this method before
  // validity can be determined.
  if (!stream_) {
    return false;
  }
  stream_->seekg(offset, std::ios_base::beg);
  if (!stream_->good()) {
    return false;
  }
  return true;
}

off_t Minidump::Tell() {
  if (!valid_ || !stream_) {
    return (off_t)-1;
  }

  // Check for conversion data loss
  std::streamoff std_streamoff = stream_->tellg();
  off_t rv = static_cast<off_t>(std_streamoff);
  if (static_cast<std::streamoff>(rv) == std_streamoff) {
    return rv;
  } else {
    return (off_t)-1;
  }
}

bool Minidump::SeekToStreamType(uint32_t  stream_type,
                                uint32_t* stream_length) {
  assert(stream_length);
  *stream_length = 0;

  if (!valid_) {
    return false;
  }

  MinidumpStreamMap::const_iterator iterator = stream_map_->find(stream_type);
  if (iterator == stream_map_->end()) {
    // This stream type didn't exist in the directory.
    return false;
  }

  MinidumpStreamInfo info = iterator->second;
  if (info.stream_index >= header_.stream_count) {
    return false;
  }

  MDRawDirectory* directory_entry = &(*directory_)[info.stream_index];
  if (!SeekSet(directory_entry->location.rva)) {
    return false;
  }

  *stream_length = directory_entry->location.data_size;

  return true;
}


template<typename T>
T* Minidump::GetStream(T** stream) {
  // stream is a garbage parameter that's present only to account for C++'s
  // inability to overload a method based solely on its return type.

  const uint32_t stream_type = T::kStreamType;

  assert(stream);
  *stream = NULL;

  if (!valid_) {
    return NULL;
  }

  MinidumpStreamMap::iterator iterator = stream_map_->find(stream_type);
  if (iterator == stream_map_->end()) {
    // This stream type didn't exist in the directory.
    return NULL;
  }

  // Get a pointer so that the stored stream field can be altered.
  MinidumpStreamInfo* info = &iterator->second;

  if (info->stream) {
    // This cast is safe because info.stream is only populated by this
    // method, and there is a direct correlation between T and stream_type.
    *stream = static_cast<T*>(info->stream);
    return *stream;
  }

  uint32_t stream_length;
  if (!SeekToStreamType(stream_type, &stream_length)) {
    return NULL;
  }

  scoped_ptr<T> new_stream(new T(this));

  if (!new_stream->Read(stream_length)) {
    return NULL;
  }

  *stream = new_stream.release();
  info->stream = *stream;
  return *stream;
}


}  // namespace google_breakpad
