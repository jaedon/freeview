#ifndef NALU_HELPER_H
#define NALU_HELPER_H

#include <vector>
#include <string>
#include <stdint.h>

namespace netflix {
namespace device {
namespace esplayer {

class h264NaluHelper
{
public:
    h264NaluHelper();
    bool analyze(const uint8_t * auData, uint32_t auDataSize);
    bool dump(const uint8_t * auData, uint32_t auDataSize, const char * name);
    bool print(const uint8_t * auData, uint32_t auDataSize);
    bool parse(const uint8_t * auData, uint32_t auDataSize, bool & idrSliceFound, bool & spsFound, bool & ppsFound);
    bool reset();
private:
    int             mState;
    int             mFramesParsed;
    std::vector<std::string> mNALTable;
};

} // namespace esplayer
} // namespace device
} // namespace netflix

#endif
