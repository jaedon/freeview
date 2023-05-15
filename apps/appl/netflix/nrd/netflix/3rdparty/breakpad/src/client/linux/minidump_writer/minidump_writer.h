// Copyright (c) 2009, Google Inc.
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

#ifndef CLIENT_LINUX_MINIDUMP_WRITER_MINIDUMP_WRITER_H_
#define CLIENT_LINUX_MINIDUMP_WRITER_MINIDUMP_WRITER_H_

#include <stdint.h>
#include <sys/types.h>
#include <sys/ucontext.h>
#include <unistd.h>

#include <list>
#include <utility>

#include "client/linux/minidump_writer/linux_dumper.h"
#include "google_breakpad/common/minidump_format.h"

namespace google_breakpad {

class ExceptionHandler;

struct MappingEntry {
  MappingInfo first;
  uint8_t second[sizeof(MDGUID)];
};

// Content flags that control the amount of information written to the Minidump file
enum ContentFlags {
    DUMP_CRASH_THREAD       = (1 << 0),  // Dump the crashing thread
    DUMP_ADDITIONAL_THREADS = (1 << 1),  // Dump the other threads
    DUMP_LIMIT_STACK_SIZE   = (1 << 2),  // Limit the stack size
    DUMP_MAPPINGS           = (1 << 3),  // Write information about mappings in effect
    DUMP_APP_MEMORY         = (1 << 4),  // Write application provided memory regions
    DUMP_MEMORY_LIST        = (1 << 5),  // Write the memory list stream
    DUMP_EXCEPTION          = (1 << 6),  // Write the exception information stream
    DUMP_SYSTEM_INFO        = (1 << 7),  // Write the system information (CPU/OS) stream
    DUMP_LINUX_CPU_INFO     = (1 << 8),  // Add the contents of the /proc/cpuinfo file
    DUMP_LINUX_PROC_STATUS  = (1 << 9),  // Add the contents of the /proc/<crashtread>/status file
    DUMP_LINUX_LSB_RELEASE  = (1 << 10), // Add the contents of the /etc/lsb_release file
    DUMP_LINUX_CMD_LINE     = (1 << 11), // Add the contents of the /proc/<crashthread>/cmdline file
    DUMP_LINUX_ENVIRON      = (1 << 12), // Add the contents of the /proc/<crashthread>/environ file
    DUMP_LINUX_AUXV         = (1 << 13), // Add the contents of the /proc/<crashthread>/auxv file
    DUMP_LINUX_MAPS         = (1 << 14), // Add the contents of the /proc/<crashthread>/maps file
    DUMP_LINUX_DSO_DEBUG    = (1 << 15), // Write information about loaded DSOs, if available

    // Dump all possible information (default behavior)
    DUMP_ALL                = -1 & ~(DUMP_LIMIT_STACK_SIZE),

    // Predefined crash report sizes. They do not have a stack size limit (use DUMP_LIMIT_STACK_SIZE for that)
    // Small size: smallest possible minidump that produces a human readable stack trace
    DUMP_SMALL              = DUMP_EXCEPTION | DUMP_SYSTEM_INFO | DUMP_MAPPINGS | DUMP_CRASH_THREAD,
    // Medium size: add also information about all other running threads
    DUMP_MEDIUM             = DUMP_SMALL | DUMP_ADDITIONAL_THREADS,
    // Large size: add everything else, except for the /proc file dumps
    DUMP_LARGE              = DUMP_MEDIUM | DUMP_APP_MEMORY | DUMP_MEMORY_LIST
};

inline ContentFlags operator | (ContentFlags a, ContentFlags b) {
    return static_cast<ContentFlags>(static_cast<int>(a) | static_cast<int>(b));
}

inline ContentFlags operator & (ContentFlags a, ContentFlags b) {
    return static_cast<ContentFlags>(static_cast<int>(a) & static_cast<int>(b));
}

inline ContentFlags& operator |= (ContentFlags& a, ContentFlags b) {
    return a = a|b;
}

inline ContentFlags& operator &= (ContentFlags& a, ContentFlags b) {
    return a = a & b;
}

// A list of <MappingInfo, GUID>
typedef std::list<MappingEntry> MappingList;

#if defined(__aarch64__)
typedef struct fpsimd_context fpstate_t;
#elif !defined(__ARM_EABI__) && !defined(__mips__)
typedef struct _libc_fpstate fpstate_t;
#endif

// These entries store a list of memory regions that the client wants included
// in the minidump.
struct AppMemory {
  void* ptr;
  size_t length;

  bool operator==(const struct AppMemory& other) const {
    return ptr == other.ptr;
  }

  bool operator==(const void* other) const {
    return ptr == other;
  }
};
typedef std::list<AppMemory> AppMemoryList;

// Writes a minidump to the filesystem. These functions do not malloc nor use
// libc functions which may. Thus, it can be used in contexts where the state
// of the heap may be corrupt.
//   minidump_path: the path to the file to write to. This is opened O_EXCL and
//     fails open fails.
//   crashing_process: the pid of the crashing process. This must be trusted.
//   blob: a blob of data from the crashing process. See exception_handler.h
//   blob_size: the length of |blob|, in bytes
//
// Returns true if successful.
bool WriteMinidump(const char* minidump_path, pid_t crashing_process,
                   const void* blob, size_t blob_size,
                   ContentFlags flags);
// Same as above but takes an open file descriptor instead of a path.
bool WriteMinidump(int minidump_fd, pid_t crashing_process,
                   const void* blob, size_t blob_size,
                   ContentFlags flags);

// Alternate form of WriteMinidump() that works with processes that
// are not expected to have crashed.  If |process_blamed_thread| is
// meaningful, it will be the one from which a crash signature is
// extracted.  It is not expected that this function will be called
// from a compromised context, but it is safe to do so.
bool WriteMinidump(const char* minidump_path, pid_t process,
                   ContentFlags flags,
                   pid_t process_blamed_thread);

// These overloads also allow passing a list of known mappings and
// a list of additional memory regions to be included in the minidump.
bool WriteMinidump(const char* minidump_path, pid_t crashing_process,
                   const void* blob, size_t blob_size,
                   ContentFlags flags,
                   const MappingList& mappings,
                   const AppMemoryList& appdata);
bool WriteMinidump(int minidump_fd, pid_t crashing_process,
                   const void* blob, size_t blob_size,
                   ContentFlags flags,
                   const MappingList& mappings,
                   const AppMemoryList& appdata);

// These overloads also allow passing a file size limit for the minidump.
bool WriteMinidump(const char* minidump_path, off_t minidump_size_limit,
                   pid_t crashing_process,
                   const void* blob, size_t blob_size,
                   ContentFlags flags,
                   const MappingList& mappings,
                   const AppMemoryList& appdata);
bool WriteMinidump(int minidump_fd, off_t minidump_size_limit,
                   pid_t crashing_process,
                   const void* blob, size_t blob_size,
                   ContentFlags flags,
                   const MappingList& mappings,
                   const AppMemoryList& appdata);

bool WriteMinidump(const char* filename,
                   ContentFlags flags,
                   const MappingList& mappings,
                   const AppMemoryList& appdata,
                   LinuxDumper* dumper);

}  // namespace google_breakpad

#endif  // CLIENT_LINUX_MINIDUMP_WRITER_MINIDUMP_WRITER_H_
