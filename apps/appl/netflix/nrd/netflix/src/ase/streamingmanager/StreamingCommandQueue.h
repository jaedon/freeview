/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef STREAMING_COMMAND_QUEUE_H
#define STREAMING_COMMAND_QUEUE_H

#include <nrdase/AseCommonDataTypes.h>

#include <deque>

namespace netflix {
namespace ase {

class StreamingCommand;

/**
 * This is not a thread safe command queue
 */
class StreamingCommandQueue
{
public:
    StreamingCommandQueue();
    // Add a streaming command into the command queue
    void addCommand(shared_ptr<StreamingCommand> pStreamingCommand);
    // execute all commands inside the command queue
    void runCommands();

    void clear();

    volatile uint32_t mNumOfCommands;
protected:
    std::deque<shared_ptr<StreamingCommand> > mCommandQueue;
};


}}
#endif
