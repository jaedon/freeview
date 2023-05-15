/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include <nrdbase/Log.h>
#include <nrdbase/MemoryStats.h>
#include <sys/types.h>
#include <unistd.h>

#include <fstream>
#include <sstream>
#include <stdio.h>

using namespace netflix::util;
using namespace netflix;

Variant MemoryStats::collectRaw(int pid)
{
    if(pid == -1)
        pid = getpid();

    Variant result;
    result["pid"] = pid;
    result["time"] = Time::mono().ms();
#if defined(__linux__)
    std::ostringstream filename;
    filename << "/proc/" << pid << "/smaps";
    if(FILE *f = fopen(filename.str().c_str(), "r")) {
        int total=0, privateclean=0, shared=0, privatedirty=0, line=0;
        char buf[1024];

        bool useSegment = true;
        while (fgets(buf, sizeof(buf), f)) {
            const char *ch = buf;
            int *value = 0;

            uint64_t beginning, end;
            char c1, c2, c3, c4;
            if(sscanf(ch, "%llx-%llx %c%c%c%c", &beginning, &end, &c1, &c2, &c3, &c4) == 6) {
                // useSegment = true;
                // if(c4 == 's')
                //     useSegment = false;
                //printf("Segment(%d): %llx-%llx %c%c%c%c [%d]\n", line, beginning, end, c1, c2, c3, c4, useSegment);
            } else if(useSegment) {
                if (!strncmp(ch, "Rss: ", 5)) {
                    value = &total;
                    ch += 5;
                } else if (!strncmp(ch, "Private_Clean: ", 15)) {
                    value = &privateclean;
                    ch += 15;
                } else if (!strncmp(ch, "Private_Dirty: ", 15)) {
                    value = &privatedirty;
                    ch += 15;
                } else if (!strncmp(ch, "Shared_Clean: ", 14) || !strncmp(ch, "Shared_Dirty: ", 14)) {
                    value = &shared;
                    ch += 14;
                }
                if(value) {
                    while (*ch && !isdigit(*ch))
                        ++ch;
                    if (*ch) {
                        if(const int v = atoi(ch)) {
                            //printf("Data(%d): %d\n", line, v);
                            *value += v;
                        }
                    }
                }
            }
            ++line;
        }
        result["total"] = total;
        result["privateclean"] = privateclean;
        result["privatedirty"] = privatedirty;
        result["shared"] = shared;
        fclose(f);

    } else {
        filename.str("");
        filename.clear();
        filename << "/proc/" << pid << "/statm";
        if(FILE *f = fopen(filename.str().c_str(), "r")) {
            char buffer[2048];
            if(fgets(buffer, sizeof(buffer), f)) {
                const long page_size = 1 /*sysconf(_SC_PAGESIZE)*/;
                char *offset = buffer;
                result["total"] = strtoul(offset, &offset, 10) * page_size;
                ++offset;
                int ignored = strtoul(offset, &offset, 10);
                (void)ignored;
                ++offset;
                result["shared"] = strtoul(offset, &offset, 10) * page_size;
                ++offset;
                result["privateclean"] = strtoul(offset, &offset, 10) * page_size;
                ++offset;
                ignored = strtoul(offset, &offset, 10);
                (void)ignored;
                ++offset;
                result["privatedirty"] = strtoul(offset, &offset, 10) * page_size;
            }
        }
    }

#elif defined __APPLE__
    // NRDP_UNUSED(rawOutput);
    // std::ostringstream cmd;
    // cmd << "vmmap -v ";
    // cmd << pid;
    // FILE *vm = popen(cmd.str().c_str(), "r");
    // std::ostringstream buffer;
    // char rdbuf[1024];
    // size_t sz;
    // while ((sz = fread(rdbuf, sizeof(rdbuf), sizeof(rdbuf[0]), vm)) > 0)
    //     buffer.write(rdbuf, sz);
    // pclose(vm);
    // rawOutput = buffer.str();
#endif
    return result;
}

