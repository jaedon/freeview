/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "DisplayList.h"

#include <DisplayList.h>
#include <SurfaceCache.h>
#include <GraphicsEngine.h>
#include <GibbonApplication.h>
#include <SurfaceDecoder.h>

#include <nrd/AppLog.h>
#include <nrdbase/Version.h>

#include <nrdbase/Log.h>
#include <nrdbase/ObjectCount.h>

#include <errno.h>
#include <stdlib.h>
#include <sys/select.h>

using namespace netflix;
using namespace netflix::gibbon;

NRDP_DEFINE_OBJECTCOUNT(GibbonDisplayListCommand);
NRDP_DEFINE_OBJECTCOUNT(GibbonDisplayList);

namespace netflix {
namespace gibbon {

extern std::string gibbon_indent(int indent);

}} // netflix::applicaton::gibbon

class StartCommand : public DisplayList::Command
{
public:
    inline StartCommand() : Command(Type_Start) {}
    inline StartCommand(const Surface::SharedPtr &_surface, const Size &_size) : Command(Type_Start), surface(_surface), size(_size) {}

    virtual std::string describe() const {
        std::ostringstream s;
        s << "Start(" << surface->describe() << ", " << size << ")";
        return s.str();
    }
    virtual void apply(GraphicsEngine *graphics, State *state) const {
        const Rect rect(0, 0, size);
        graphics->blit(surface, rect, state->transform(rect));
    }
    virtual void write(DisplayList::Writer &writer) const {
        writer << surface;
        writer << size;
    }
    virtual void read(DisplayList::Reader &reader) {
        reader >> surface;
        reader >> size;
    }

    Surface::SharedPtr surface;
    Size size;
};

Mutex DisplayList::sMutex(ZERO_MUTEX, "DisplayList mutex");
std::map<DisplayList::Command::Type, DisplayList::CommandCreatorBase*> DisplayList::sFactories;

DisplayList::DisplayList(Surface::SharedPtr surface) : mSize(surface->getSize())
{
    surface->upload();

    appendCommand(new StartCommand(surface, mSize));
    NRDP_OBJECTCOUNT_REF(GibbonDisplayList, this);
}

DisplayList::DisplayList(const Size &size) : mSize(size)
{
    NRDP_OBJECTCOUNT_REF(GibbonDisplayList, this);
}

DisplayList::DisplayList(Coordinate width, Coordinate height) : mSize(width, height)
{
    NRDP_OBJECTCOUNT_REF(GibbonDisplayList, this);
}

DisplayList::~DisplayList()
{
    NRDP_OBJECTCOUNT_DEREF(GibbonDisplayList, this);
    clearCommands();
}

void DisplayList::clearCommands()
{
    mCommands.clear();
    mApplied = 0;
}

class ApplyStateCommand : public DisplayList::Command
{
public:
    inline ApplyStateCommand() : Command(Type_ApplyState) {}

    void setData(const GraphicsEngine::State &graphicsState, const State &displayListState)
    {
        mGraphicsState = graphicsState;
        mDisplayListState = displayListState;
    }

    virtual std::string describe() const {
        std::ostringstream s;
        s << (mPopCommand ? "PushState" : "PopState") << "(";
        s << "composition=" << mGraphicsState.compositionMode;
        s << " smoothScale=" << mGraphicsState.smoothScale;
        s << " opacity=" << mGraphicsState.opacity;
        s << " scale=" << mGraphicsState.scale << "(" << mDisplayListState.scaleX << "," << mDisplayListState.scaleY << ")";
        if(mGraphicsState.hasClip)
            s << " clip=" << mGraphicsState.clip.toString();
        s << ")";
        return s.str();
    }
    virtual void apply(GraphicsEngine *graphics, State *state) const {
        if(mPopCommand)
            mPopCommand->setData(graphics->getState(), *state);
        graphics->setState(mGraphicsState);
        *state = mDisplayListState;
    }
    virtual void write(DisplayList::Writer &writer) const {
        Variant display;
        Variant graphics;

        writer << (mPopCommand ? true : false);
        writer << StringFormatter::sformat("%p", this);
        if(mPopCommand) {
            writer << StringFormatter::sformat("%p", mPopCommand.get());

            writer << mDisplayListState.clip;
            writer << mDisplayListState.scale;
            writer << mDisplayListState.opacity;
            writer.writeCoordinate(mDisplayListState.offsetX);
            writer.writeCoordinate(mDisplayListState.offsetY);
            writer << mDisplayListState.scaleX;
            writer << mDisplayListState.scaleY;

            writer << mGraphicsState.compositionMode;
            writer << mGraphicsState.smoothScale;
            writer << mGraphicsState.opacity;
            writer << mGraphicsState.scale;
            writer << mGraphicsState.clip;
            writer << mGraphicsState.hasClip;
        }
    }
    virtual void read(DisplayList::Reader &reader) {

        bool hasPopCommand;
        reader >> hasPopCommand;
        std::string that;
        reader >> that;
        if(hasPopCommand) {
            std::string pop;
            reader >> pop;
            const DisplayList::Command::SharedPtr popList = reader.displayList(pop);
            if(popList) {
                mPopCommand = static_pointer_cast<ApplyStateCommand>(popList);
            } else {
                reader.addPendingPopCommand(pop, shared_from_this());
            }

            reader >> mDisplayListState.clip;
            reader >> mDisplayListState.scale;
            reader >> mDisplayListState.opacity;
            reader.readCoordinate(mDisplayListState.offsetX);
            reader.readCoordinate(mDisplayListState.offsetY);
            reader >> mDisplayListState.scaleX;
            reader >> mDisplayListState.scaleY;

            reader >> mGraphicsState.compositionMode;
            reader >> mGraphicsState.smoothScale;
            reader >> mGraphicsState.opacity;
            reader >> mGraphicsState.scale;
            reader >> mGraphicsState.clip;
            reader >> mGraphicsState.hasClip;
        } else {
            if(DisplayList::Command::SharedPtr pending = reader.pendingPopCommand(that)) {
                shared_ptr<ApplyStateCommand> apply = static_pointer_cast<ApplyStateCommand>(pending);
                apply->mPopCommand = static_pointer_cast<ApplyStateCommand>(shared_from_this());
            } else {
                reader.addDisplayList(that, shared_from_this());
            }
        }
    }

    shared_ptr<ApplyStateCommand> createPopCommand() {
        assert(!mPopCommand);
        mPopCommand.reset(new ApplyStateCommand);
        return mPopCommand;
    }

    shared_ptr<ApplyStateCommand> mPopCommand;
    GraphicsEngine::State mGraphicsState;
    State mDisplayListState;
};


void DisplayList::apply_internal(GraphicsEngine *graphics, const Rect &srcRect, const Rect &dstRect, int steps)
{
    ++mApplied;
    if(mCommands.empty())
        return;

    PERFORMANCE_MARKER_SCOPED("DisplayList.apply_internal");

    const GraphicsEngine::State &gfxState = graphics->getState();
    graphics->setClip(dstRect.scaled(gfxState.scale, gfxState.scale).intersected(gfxState.clip));
    Command::State state;
    state.clip = gfxState.clip;
    state.opacity = gfxState.opacity;
    state.scale = gfxState.scale;
    state.offsetX = dstRect.x;
    state.offsetY = dstRect.y;

    if(DisplayList::SharedPtr displayList = graphics->getDisplayList()) {
        ApplyStateCommand *applyCommand = new ApplyStateCommand;
        applyCommand->setData(gfxState, state);
        displayList->appendCommand(applyCommand);
        for(std::list<Command::SharedPtr>::const_iterator it = mCommands.begin(); it != mCommands.end(); ++it)
            displayList->appendCommand((*it)->duplicate(&state));
        displayList->appendCommand(applyCommand->createPopCommand());
        return;
    }

    int cur = 0;
    if(srcRect.width != dstRect.width)
        state.scaleX = float(dstRect.width) / srcRect.width;
    if(srcRect.height != dstRect.height)
        state.scaleY = float(dstRect.height) / srcRect.height;
    for(std::list<Command::SharedPtr>::const_iterator it = mCommands.begin(); it != mCommands.end(); ++it) {
        if(steps >= 0 && ++cur > steps)
            break;
        PERFORMANCE_MARKER_SCOPED("DisplayList.apply_internal.command");
        const Rect clipRect = graphics->clip();
        if((*it)->isClipped(graphics->clip())) {
            if((*it)->type() == Command::Type_Push) {
                ++it;
                int depth = 1;
                for( ; depth && it != mCommands.end(); ) {
                    if(steps >= 0 && ++cur > steps)
                        break;
                    switch((*it)->type()) {
                    case Command::Type_Push:
                        ++depth;
                        break;
                    case Command::Type_ApplyState:
                    case Command::Type_SetClip:
                    case Command::Type_ClearClip:
                    case Command::Type_SetScale:
                    case Command::Type_SetOpacity:
                    case Command::Type_SetSmoothScale:
                    case Command::Type_SetCompositionMode:
                        (*it)->apply(graphics, &state);
                        break;
                    case Command::Type_Pop:
                        --depth;
                    default:
                        break;
                    }
                    if(!depth)
                        break;
                    ++it;
                }
            }
            if(it == mCommands.end())
                break;
            continue;
        }
        (*it)->apply(graphics, &state);
    }
    graphics->setState(gfxState);
}

std::string DisplayList::describe() const
{
    std::ostringstream s;
    s << "DisplayList(" << this << ") "
      << "size=" << getWidth() << "x" << getHeight() << ": " << mDescription;
    return s.str();
}

void DisplayList::offset(Coordinate x, Coordinate y)
{
    if(Surface::SharedPtr surface = getSurface()) {
        surface->offset(x, y);
    } else {
        //offset each command? ###
    }
}

void DisplayList::dump() const {
    int indent = 0;
    Log::warn(TRACE_GIBBON_DISPLAYLIST, "%s\n", describe().c_str());
    for(std::list<Command::SharedPtr>::const_iterator it = mCommands.begin(); it != mCommands.end(); ++it) {
        switch((*it)->type()) {
        case Command::Type_Push:
            ++indent;
            break;
        case Command::Type_Pop:
            --indent;
            break;
        default:
            Log::warn(TRACE_GIBBON_DISPLAYLIST, "%s%s\n", gibbon_indent(indent).c_str(), (*it)->describe().c_str());
            break;
        }
    }
}

Surface::SharedPtr DisplayList::getSurface() const
{
    if(mCommands.size() == 1 && mCommands.front()->type() == Command::Type_Start) {
        Surface::SharedPtr surface = static_pointer_cast<StartCommand>(mCommands.front())->surface;
        GibbonApplication::instance()->getSurfaceCache()->touch(surface);
        return surface;
    }
    return Surface::SharedPtr();
}

DisplayList::SharedPtr DisplayList::duplicate(int steps)
{
    DisplayList::SharedPtr result(new DisplayList(mSize.width, mSize.height));
    for(std::list<Command::SharedPtr>::const_iterator it = mCommands.begin(); it != mCommands.end(); ++it) {
        if(!steps--)
            break;
        result->appendCommand((*it));
    }
    return result;
}

Surface::SharedPtr DisplayList::flatten()
{
    Surface::SharedPtr surface = getSurface();
    if(!mSize.empty() && !surface && hasCommands()) {
        const uint8_t flags = Surface::Flags_Accelerated|Surface::Flags_Alpha|Surface::Flags_RenderTarget;
        surface = GibbonApplication::instance()->getSurfaceCache()->requestSurface(mSize.width, mSize.height, flags, false);
        if(!surface && !surface->isValid())
            return Surface::SharedPtr();
        surface->setDescription(getDescription());
        {
            GraphicsEngine engine(surface);
            const Rect rect(getRect());
            engine.clear(Rect(0, 0, mSize.width, mSize.height));
            engine.apply(shared_from_this(), rect, rect);
        }
        mCommands.clear();
        appendCommand(new StartCommand(surface, mSize));
    }
    return surface;
}

class CommentCommand : public DisplayList::Command
{
public:
    inline CommentCommand() : Command(Type_Comment) {}
    inline CommentCommand(const std::string &_comment) : Command(Type_Comment), comment(_comment) {}

    virtual std::string describe() const {
        std::ostringstream s;
        s << "# " << comment;
        return s.str();
    }
    virtual void apply(GraphicsEngine *, State *) const { }
    virtual void write(DisplayList::Writer &writer) const {
        writer << comment;
    }
    virtual void read(DisplayList::Reader &reader) {
        reader >> comment;
    }

    std::string comment;
};

void DisplayList::appendComment(const std::string &comment)
{
    appendCommand(new CommentCommand(comment));
}

class PushCommand : public DisplayList::Command
{
public:
    inline PushCommand() : Command(Type_Push) { }
    inline PushCommand(const Rect &_rect) : Command(Type_Push), rect(_rect) {}

    virtual std::string describe() const {
        std::ostringstream s;
        s << "Push(" << rect << ")";
        return s.str();
    }
    virtual bool isClipped(const Rect &clipRect) const {
        return !rect.empty() && !rect.intersects(clipRect);
    }
    virtual void apply(GraphicsEngine *, State *) const { }
    virtual void write(DisplayList::Writer &writer) const {
        writer << rect;
    }
    virtual void read(DisplayList::Reader &reader) {
        reader >> rect;
    }

    Rect rect;
};

void DisplayList::push(const Rect &rect)
{
    appendCommand(new PushCommand(rect));
}

class PopCommand : public DisplayList::Command
{
public:
    PopCommand() : Command(Type_Pop) {}

    virtual std::string describe() const { return "Pop()"; }
    virtual void apply(GraphicsEngine *, State *) const { }
    virtual void write(DisplayList::Writer &) const { }
    virtual void read(DisplayList::Reader &) { }
};


void DisplayList::pop()
{
    appendCommand(new PopCommand);
}

void DisplayList::initCommands()
{
    registerFactory<StartCommand>(Command::Type_Start);
    registerFactory<ApplyStateCommand>(Command::Type_ApplyState);
    registerFactory<CommentCommand>(Command::Type_Comment);
    registerFactory<PushCommand>(Command::Type_Push);
    registerFactory<PopCommand>(Command::Type_Pop);
}

DisplayList::SharedPtr DisplayList::Reader::read(const DataBuffer &data)
{
    mDeserializer.setData(data);

    uint8_t flags;
    *this >> flags;
    mDeserializer.setFlags(flags);

    uint16_t major, minor, patch;
    *this >> major >> minor >> patch;
    //Log::info(TRACE_GIBBON_DISPLAYLIST, "Generated with(%x): %d.%d.%d [%d]", flags, major, minor, patch, data.size());

    Size displayListSize;
    *this >> displayListSize;
    uint32_t commandCount;
    *this >> commandCount;
    if(commandCount) {
        DisplayList::SharedPtr result(new DisplayList(displayListSize));
        for(uint32_t i = 0; i < commandCount; ++i) {
            Command::Type type;
            *this >> type;
            if(type != DisplayList::Command::Type_Unknown)
                result->appendCommand(createCommand(*this, type));
        }
        return result;
    }
    return DisplayList::SharedPtr();
}

void DisplayList::Writer::write(const DisplayList::SharedPtr &list)
{
    mSerializer.clear();
    ++mFrames;

    *this << mSerializer.flags();
    *this << uint16_t(NRDP_VERSION_MAJOR) << uint16_t(NRDP_VERSION_MINOR) << uint16_t(NRDP_VERSION_PATCH);

    const std::list<DisplayList::Command::SharedPtr> &cmds = list->getCommands();
    *this << list->getSize();
    *this << uint32_t(cmds.size());
    for(std::list<DisplayList::Command::SharedPtr>::const_iterator cmd = cmds.begin(); cmd != cmds.end(); ++cmd) {
        *this << (*cmd)->type();
        if((*cmd)->type() == DisplayList::Command::Type_Unknown) {
            Log::warn(TRACE_GIBBON_DISPLAYLIST, "Unable to save: %s", (*cmd)->describe().c_str());
            continue;
        }
        (*cmd)->write(*this);
    }
}


