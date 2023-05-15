/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

/* global nrdp */

nrdp.websocket = {
    _nextWebSocket: 0,
    _websockets: {},

    _nextIdx: 1,
    _cbs: [],
    _fn: function _fn(name, params, cb) {
        if (!params) params = {};
        params.idx = this._nextIdx++;
        this._cbs[params.idx] = cb;
        nrdp._invoke("websocket", name, params);
        return params.idx;
    },

    _handleEvent: function _handleEvent(event) {
        var that;
        if (event.name === "result") {
            if(!event.data || !event.data.idx) {
                return false;
            }

            if (typeof this._cbs[event.data.idx] === "function") {
                this._cbs[event.data.idx](event.data);
            }
            delete this._cbs[event.data.idx];
        } else if (event.name == "open") {
            that = this._websockets[event.data.websocket];
            if (!that) return false;
            that._protocol = event.data.protocol;
            that._url = event.data.url;
            that._readyState = that.OPEN;
            if(typeof that.onopen === "function") {
                NTRACE("ws: calling " + that._websocket + " onopen()", "NRDP_SCRIPT");
                that.onopen();
            }
        } else if (event.name == "message") {
            that = this._websockets[event.data.websocket];
            if (!that) return false;
            if(that.readyState === that.OPEN) {
                if(typeof that.onmessage === "function") {
                    NTRACE("ws: calling " + that._websocket + " onmessage()", "NRDP_SCRIPT");
                    that.onmessage(event.data);
                }
            }
        } else if (event.name == "error") {
            that = this._websockets[event.data.websocket];
            if (!that) return false;
            that._readyState = that.CLOSING;
            if(typeof that.onerror === "function") {
                NTRACE("ws: calling " + that._websocket + " onerror()", "NRDP_SCRIPT");
                that.onerror();
            }
        } else if (event.name == "close") {
            that = this._websockets[event.data.websocket];
            if (!that)
                return false;
            that._readyState = that.CLOSED;
            delete this._websockets[that._websocket];

            if (that.onclose instanceof Function) {
                NTRACE("ws: calling " + that._websocket + " onclose()", "NRDP_SCRIPT");
                that.onclose();
            }
            that._websocket = null;
        } else {
            return false;
        }
        return true;
    },
    _onShutdown: function _shutdown() {
        for (var i in nrdp.websocket._websockets) {
            nrdp.websocket._websockets[i].close();
        }
    },

    create: function create(url, protocols, cb) {
        if (!this._nextWebSocket)
            nrdp.addEventListener("shutdown", nrdp.websocket._onShutdown);
        var websocket = ++this._nextWebSocket;
        this._fn("create",
                 { websocket: websocket,
                   url: url,
                   protocols: protocols },
                 cb);
        return websocket;
    },

    send: function send(websocket, data, cb) {
        this._fn("send",
                 { websocket: websocket,
                   data: data },
                 cb);
    },

    close: function close(websocket, cb) {
        this._fn("close",
                 { websocket: websocket },
                 cb);
    },

    test: function test(url, cb) {
        this._fn("test",
                 { url: url },
                 cb);
    }
};

nrdp.WebSocket = function(url, protocols) {
    var that = this;

    that._websocket = nrdp.websocket.create(url, protocols ? protocols : "", function(result) {
        if(result.success) {
            that._readyState = that.CONNECTING;
        }
    });

    nrdp.websocket._websockets[that._websocket] = that;
};

nrdp.WebSocket.prototype = (function() {
    var proto = {
        _websocket: null,
        _readyState: 3,
        _protocol: "",
        _url: "",

        get protocol()      { return this._protocol; },
        get url()           { return this._url; },
        get URL()           { return this._url; },
        get readyState()    { return this._readyState; },
        get CONNECTING()    { return 0; },
        get OPEN()          { return 1; },
        get CLOSING()       { return 2; },
        get CLOSED()        { return 3; },

        onclose: null,
        onerror: null,
        onmessage: null,
        onopen: null,

        send: function send(message) {
            if(this.readyState === this.OPEN) {
                nrdp.websocket.send(this._websocket, message);
            } else {
                NTRACE("WebSocket " + this._websocket + " not OPEN", "NRDP_SCRIPT");
            }
        },
        close: function close() {
            if(this._websocket) {
                nrdp.websocket.close(this._websocket);
            }
        }
    };
    return proto;
})();
