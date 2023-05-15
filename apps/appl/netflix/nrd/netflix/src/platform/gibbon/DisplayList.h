/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef DISPLAYLIST_H
#define DISPLAYLIST_H

#include <gibbon/config.h>

#include <Rect.h>
#include <Surface.h>

#include <nrdbase/tr1.h>
#include <nrdbase/Log.h>
#include <nrdbase/Variant.h>
#include <nrdbase/LogSink.h>
#include <nrdbase/Serialize.h>
#include <nrdbase/LinkedList.h>
#include <nrdbase/ObjectCount.h>
#include <nrdbase/Noncopyable.h>

#include <list>
#include <arpa/inet.h>

NRDP_DECLARE_OBJECTCOUNT(GibbonDisplayListCommand);

namespace netflix {
namespace gibbon {

class GraphicsEngine;

class DisplayList : public enable_shared_from_this<DisplayList>, public Noncopyable
{
public:
    typedef shared_ptr<DisplayList> SharedPtr;
    typedef weak_ptr<DisplayList> WeakPtr;

    class Reader;
    class Writer;

    class Command : public enable_shared_from_this<Command>
    {
    public:
        typedef shared_ptr<Command> SharedPtr;
        typedef weak_ptr<Command> WeakPtr;

        enum Type {
            Type_Unknown,
            Type_Comment,
            Type_Push,
            Type_Pop,

            Type_Start,
            Type_ApplyState,

            Type_SetClip,
            Type_ClearClip,

            Type_SetScale,
            Type_SetOpacity,
            Type_SetSmoothScale,
            Type_SetCompositionMode,

            Type_Blit,
            Type_BatchBlit,
            Type_ClearRect,
            Type_FillRect,
            Type_DrawRect,
            Type_DrawText,

            Type_GLVBO
        };
        Command(Type type) : mType(type) { NRDP_OBJECTCOUNT_REF(GibbonDisplayListCommand, this); }
        virtual ~Command() { NRDP_OBJECTCOUNT_DEREF(GibbonDisplayListCommand, this); }

        inline Type type() const { return mType; }

        struct State {
            inline State() : scale(1), opacity(1), offsetX(0), offsetY(0), scaleX(1), scaleY(1) { }

            Rect clip;
            float scale;
            float opacity;
            Coordinate offsetX, offsetY;
            float scaleX, scaleY;
            inline Rect transform(const Rect &rect) const { return Rect(rect.x+offsetX, rect.y+offsetY,
                                                                        rect.width*scaleX, rect.height*scaleY); }
            inline std::vector<Rect> transform(const std::vector<Rect>& rects)
            {
                const size_t sz = rects.size();
                std::vector<Rect> out(sz);
                for (size_t i = 0; i < sz; ++i)
                    out[i] = transform(rects.at(i));
                return out;
            }
        };

        virtual std::string describe() const { return std::string(); }

        virtual bool isClipped(const Rect &) const { return false; }
        virtual void apply(GraphicsEngine *graphics, State *state) const = 0;
        virtual Command::SharedPtr duplicate(State *) { return shared_from_this(); }

        virtual void write(Writer &writer) const = 0;
        virtual void read(Reader &reader) = 0;

    protected:
        Type mType;
    };

    class Writer
    {
    public:
        inline Writer() : mFrames(0) { }
        virtual ~Writer() { }

        inline void clear() {
            mSerializer.clear();
            mFrames = 0;
        }

        void write(const DisplayList::SharedPtr& list);

        virtual void writeSurface(const Surface::SharedPtr &) = 0;
        inline void writeCoordinate(const Coordinate &coord) { serializeCoordinate(mSerializer, coord); }
        template <typename T> inline void writeValue(T &n) { mSerializer << n; }

        inline DataBuffer data() const { return mSerializer.data(); }
        inline int frames() const { return mFrames; }

    private:
        int mFrames;
        Serializer mSerializer;
    };

    class Reader
    {
    public:
        inline Reader() { }
        virtual ~Reader() { }

        DisplayList::SharedPtr read(const DataBuffer &data);

        virtual void readSurface(Surface::SharedPtr &) = 0;
        inline void readCoordinate(Coordinate &coord) { deserializeCoordinate(mDeserializer, coord); }
        template <typename T> inline void readValue(T &n) { mDeserializer >> n; }

        void addDisplayList(const std::string &name, const DisplayList::Command::SharedPtr &list)
        {
            mDisplayLists[name] = list;
        }
        DisplayList::Command::SharedPtr displayList(const std::string &name)
        {
            return mDisplayLists[name];
        }
        void addPendingPopCommand(const std::string &name, const DisplayList::Command::SharedPtr &list)
        {
            mPendingPops[name] = list;
        }
        DisplayList::Command::SharedPtr pendingPopCommand(const std::string &name)
        {
            return mPendingPops[name];
        }
        void clear()
        {
            mDeserializer.clear();
            mDisplayLists.clear();
            mPendingPops.clear();
        }

    private:
        Deserializer mDeserializer;
        std::map<std::string, DisplayList::Command::SharedPtr> mDisplayLists;
        std::map<std::string, DisplayList::Command::SharedPtr> mPendingPops;
    };

    explicit DisplayList(const Size &size);
    DisplayList(Surface::SharedPtr surface);
    DisplayList(Coordinate width=0, Coordinate height=0);
    ~DisplayList();

    inline void clear() { clearCommands(); }
    inline void apply(GraphicsEngine *graphics, const Rect &srcRect, const Rect &dstRect, int steps) {
        apply_internal(graphics, srcRect, dstRect, steps);
    }
    inline int applied() const { return mApplied; }

    Surface::SharedPtr flatten();
    Surface::SharedPtr getSurface() const;
    inline bool isFlattened() const { return static_cast<bool>(getSurface()); }

    DisplayList::SharedPtr duplicate(int steps);

    void appendComment(const std::string &comment);

    void push(const Rect &rect);
    void pop();

    void clearCommands();
    inline bool hasCommands() const { return !mCommands.empty(); }
    inline void appendCommand(Command *command) { appendCommand(Command::SharedPtr(command)); }
    inline void prependCommand(Command *command) { prependCommand(Command::SharedPtr(command)); }
    inline void appendCommand(const Command::SharedPtr &command) { mCommands.push_back(command); }
    inline void prependCommand(const Command::SharedPtr &command) { mCommands.push_front(command); }

    inline const std::list<Command::SharedPtr> &getCommands() const { return mCommands; }

    void offset(Coordinate x, Coordinate y);

    inline void setSize(const Size& size) { mSize = size; }
    inline const Size &getSize() const { return mSize; }
    inline Coordinate getWidth() const { return mSize.width; }
    inline Coordinate getHeight() const { return mSize.height; }
    inline Rect getRect() const { return Rect(0, 0, getSize()); }

    inline void setDescription(const std::string &d) { mDescription = d; }
    inline std::string getDescription() const { return mDescription; }
    std::string describe() const;
    void dump() const;

    template<typename T>
    static void registerFactory(Command::Type type)
    {
        ScopedMutex locker(sMutex);
        CommandCreatorBase*& base = sFactories[type];
        if (!base)
            base = new CommandCreator<T>;
    }
    static void cleanupFactories()
    {
        ScopedMutex locker(sMutex);
        std::map<Command::Type, CommandCreatorBase*>::const_iterator it = sFactories.begin();
        while (it != sFactories.end()) {
            delete it->second;
            ++it;
        }
        sFactories.clear();
    }

    static void initCommands();
    static Command::SharedPtr createCommand(Reader &reader, Command::Type type)
    {
        assert(sFactories.count(type) > 0);
        return sFactories[type]->create(reader);
    }

#if defined(GIBBON_DISPLAYLIST_SURFACE)
#elif defined(GIBBON_GRAPHICSENGINE_GLES2)
#elif defined(GIBBON_GRAPHICSENGINE_NOVA)
#endif

protected:
    void apply_internal(GraphicsEngine *graphics, const Rect &srcRect, const Rect &dstRect, int steps);

    Size mSize;
    int mApplied;
    std::string mDescription;
    std::list<Command::SharedPtr> mCommands;

#if defined(GIBBON_GRAPHICSENGINE_DIRECTFB)
#elif defined(GIBBON_GRAPHICSENGINE_NOVA)
#elif defined(GIBBON_GRAPHICSENGINE_GLES2)
#endif
    class PlatformData;
    PlatformData *mPlatform;

private:

    class CommandCreatorBase
    {
    public:
        virtual ~CommandCreatorBase() { }
        virtual Command::SharedPtr create(Reader &reader) = 0;
    };
    template<typename T>
    class CommandCreator : public CommandCreatorBase
    {
    public:
        virtual Command::SharedPtr create(Reader &reader)
        {
            Command::SharedPtr cmd(new T);
            cmd->read(reader);
            return cmd;
        }
    };

    static Mutex sMutex;
    static std::map<Command::Type, CommandCreatorBase*> sFactories;
};

inline std::ostream &operator<<(std::ostream &out, const DisplayList &s)
{
    return out << s.describe();
}


template <typename T> inline DisplayList::Writer &operator<<(DisplayList::Writer &writer, const T &n)
{
    writer.writeValue(n);
    return writer;
}

template <typename T> inline DisplayList::Reader &operator>>(DisplayList::Reader &reader, T &n)
{
    reader.readValue(n);
    return reader;
}

inline DisplayList::Writer &operator<<(DisplayList::Writer &writer, const Surface::SharedPtr &s)
{
    writer.writeSurface(s);
    return writer;
}

inline DisplayList::Reader &operator>>(DisplayList::Reader &reader, Surface::SharedPtr &s)
{
    reader.readSurface(s);
    return reader;
}

SERIALIZE_ALIAS(DisplayList::Command::Type, uint8_t)

}}

#endif // DISPLAYLIST_H
