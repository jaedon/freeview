/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#include "StreamingCommandQueue.h"
#include "StreamingCommand.h"

using namespace netflix::ase;
using namespace std;

StreamingCommandQueue::StreamingCommandQueue() :
                mNumOfCommands(0)
{
}

void StreamingCommandQueue::addCommand(shared_ptr<StreamingCommand> pStreamingCommand)
{
    mCommandQueue.push_back(pStreamingCommand);
    ++mNumOfCommands;
}

void StreamingCommandQueue::runCommands()
{
    mNumOfCommands = 0;
    while (!mCommandQueue.empty())
    {
        shared_ptr<StreamingCommand> pStreamingCommand = mCommandQueue.front();
        mCommandQueue.pop_front();
        pStreamingCommand->execute();
    }
}

void StreamingCommandQueue::clear()
{
    mCommandQueue.clear();
    mNumOfCommands = 0;
}
