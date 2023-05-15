/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

/*global nrdp */

function MediaSourcePlayer(fullpath, shortpath)
{
    this._path = fullpath;
    this._shortpath = shortpath;
};
MediaSourcePlayer.constructor = MediaSourcePlayer;

Object.defineProperties(MediaSourcePlayer.prototype, {
    // native object
    classname: { value: "MediaSourcePlayerBridge" },
    path: { get: function() { return this._shortpath; } },

    // IAdaptiveStreamingPlayer state
    OPENING: { value: 0 },
    PLAYING: { value: 1 },
    PAUSED:  { value: 2 },
    STOPPED: { value: 3 },
    CLOSED:  { value: 4 },

    // MediaType
    MEDIA_UNKNOWN: { value: -1 },
    MEDIA_AUDIO:   { value: 0 },
    MEDIA_VIDEO:   { value: 1 },
    MEDIA_TEXT:    { value: 2 },

    // synced properties
    bufferPoolSize: { get: function() { return this._syncData.bufferPoolSize; } },
    state: { get: function() { return this._syncData.state; } }
});

//////////
// Events
//////////
MediaSourcePlayer.prototype.addEventListener = function addEventListener(evt, listener)
{
    nrdp._addEventListener(this, evt, listener);
};

MediaSourcePlayer.prototype.removeEventListener = function removeEventListener(
    evt, listener)
{
    nrdp._removeEventListener(this, evt, listener);
};

MediaSourcePlayer.prototype._handleEvent = function _handleEvent(event)
{
    for (var prop in event.data._propups)
    {
        this._syncData[prop] = event.data._propups[prop];
    }
    if ((event.name == "MediaSourcePlayer") || (event.name == "PlaybackReporter"))
    {
        event.data.time = event.time;
        nrdp._callEventListeners(this, event.data);
        return true;
    }
    else
    {
        return false;
    }
};

//////////
// player control
//////////
MediaSourcePlayer.prototype.open = function open(args)
{
    nrdp._invoke(this._path, "open", args);
};

MediaSourcePlayer.prototype.play = function play(pts)
{
    var args = {};
    if (pts != undefined && pts != null)
        args.pts = pts;
    nrdp._invoke(this._path, "play", args);
};

MediaSourcePlayer.prototype.stop = function stop()
{
    nrdp._invoke(this._path, "stop");
};

MediaSourcePlayer.prototype.pause = function pause()
{
    nrdp._invoke(this._path, "pause");
};

MediaSourcePlayer.prototype.unpause = function unpause()
{
    nrdp._invoke(this._path, "unpause");
};

MediaSourcePlayer.prototype.skip = function skip(ms)
{
    nrdp._invoke(this._path, "skip", {pts:ms});
};

MediaSourcePlayer.prototype.swim = function swim(pts, absolute, fuzz, allowRebuffer)
{
    var args = { pts: pts };
    if (typeof absolute == "boolean")
        args.absolute = absolute;
    if (typeof fuzz == "number" && !isNaN(fuzz))
        args.fuzz = fuzz;
    if (typeof allowRebuffer == "boolean")
        args.allowRebuffer = allowRebuffer;
    nrdp._invoke(this._path, "swim", args);
};

MediaSourcePlayer.prototype.close = function close()
{
    nrdp._invoke(this._path, "close");
};

//////////
// display control
//////////
MediaSourcePlayer.prototype.setVideoWindow = function setVideoWindow(
    x, y, width, height, transitionDuration, zOrder)
{
    var args = {
        x: x,
        y: y,
        width: width,
        height: height,
        transitionDuration: transitionDuration,
        zOrder : zOrder
    };
    nrdp._invoke(this._path, "setVideoWindow", args);
};

MediaSourcePlayer.prototype.bringVideoToFront = function bringVideoToFront()
{
    // no operation
};

MediaSourcePlayer.prototype.sendVideoToBack = function sendVideoToBack()
{
    // no operation
};

//////////
// audio control
//////////
MediaSourcePlayer.prototype.disableAudio = function()
{
    // disable audio so that audio track switching can happen
    nrdp._invoke(this._path, "disableAudio");
};

MediaSourcePlayer.prototype.enableAudio = function()
{
    // audio data for new track is buffered enough, and notify player to enable audio
    nrdp._invoke(this._path, "enableAudio");
};

MediaSourcePlayer.prototype.getVolume = function(cb)
{
    // getVolume is returned in an event type "volume"
    nrdp._invoke(this._path, "getVolume");
};

MediaSourcePlayer.prototype.setVolume = function(volume, transition, ease)
{
    nrdp._invoke(this._path, "setVolume", {
        "targetVolume": volume,
        "transitionDuration": transition,
        "ease": ease
    });
};

//////////
// player stats
//////////

MediaSourcePlayer.prototype.getBufferRange = function getBufferRange(cb)
{
    // returns in event type "bufferrange"
    nrdp._invoke(this._path, "getBufferRange");
};

MediaSourcePlayer.prototype.obtainPlaybackStat = function obtainPlaybackStat()
{
    nrdp._invoke(this._path, "obtainPlaybackStat");
};

//////////
// factory
//////////
nrdp.mediasourceplayerfactory = {
    classname: "MediaSourcePlayerFactoryBridge",
    _path: "mediasourceplayerfactory",

    createPlayer: function createPlayer(index, cb)
    {
        var name = "mediasourceplayer" + index;

        // check to see if a player already exists here
        if (nrdp[this._path][name])
        {
            // if so return that one
            var result = {
                "success": true,
                "path": name,
                "player": nrdp[this._path][name]
            };
            cb(result);
            return nrdp[this._path][name];
        }

        // create the JS player bridge
        var fullpath = this._path + "." + name;
        var player = new MediaSourcePlayer(fullpath, name);

        // store the player into nrdp
        nrdp[this._path][name] = player;

        // create the native player bridge
        var args = {
            "name": name
        };
        this._fn(
            "createPlayer", args,
            function(result)
            {
                if (result["success"])
                {
                    // add player to the result
                    result["path"] = name;
                    result["player"] = player;
                }

                // continue to original cb
                if (typeof cb == 'function')
                    cb(result);
            });

        // return the JS player
        return player;
    },

    destroyPlayer: function destroyPlayer(index, cb)
    {
        var name = "mediasourceplayer" + index;
        if (nrdp[this._path][name])
        {
            // remove the js object
            delete nrdp[this._path][name];

            // destroy the native object
            return this._fn("destroyPlayer", {"name": name}, cb);
        }
        return undefined;
    },

    // private
    _nextIdx: 0,
    _cbs: {},

    _fn: function(name, params, cb)
    {
        // add index to params
        if (!params) params = {};
        params.idx = this._nextIdx++;

        // store cb away
        this._cbs[params.idx] = cb;

        // invoke native method
        nrdp._invoke(this._path, name, params);

        return params.idx;
    },

    _handleEvent: function(event)
    {
        var cb, idx;

        // handle invoke callbacks
        if (event.name == "result")
        {
            idx = event.data.idx;

            if (typeof this._cbs[idx] == 'function')
            {
                cb = this._cbs[idx];
                delete event.data.idx;
                cb(event.data);
            }
            delete this._cbs[idx];

            return true;
        }

        // unhandled
        return false;
    }
};
