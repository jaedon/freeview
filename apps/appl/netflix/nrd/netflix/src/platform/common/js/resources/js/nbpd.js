/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

/*
 * all nrdp communication helpers are required to do four things:
 *
 * 1. add object to nrdp._backchannels for initialization
 * 2. provide nrdp._setProperty()
 * 3. provide nrdp._invoke()
 * 4. call nrdp._gotEvent() when an event comes in.
 */

if(typeof window !== "undefined") {
(function() {

var evtsrc;
var iframe;
var nbpdUrl;

nrdp._nbpdAsync = false;

function _gotJSON(json) {
    var event;
    try {
        event = JSON.parse(json);
    } catch (err) {
        console.log("unhandled exception in eventsource " + json);
        console.log(err.toString());
        return;
    }
    if (!event.object && event.name == "aboutToQuit") {
        if (evtsrc) {
            evtsrc.close();
        } else {
            iframe.contentWindow.postMessage("quit", '*');
        }
    }
    nrdp._sendSyncdEvent(nrdp._gotEvent, nrdp, [event]);
};

function _POST(params, async) {
    var xhr = new XMLHttpRequest();
    xhr.open("POST", nbpdUrl + "ipc", async);
    xhr.setRequestHeader("Content-type", "text/plain");
    xhr.onerror = function() { console.log("lost IPC connection for POST"); };
    xhr.onreadystatechange = function() {
        if(this.readyState == 4) {
            this.onreadystatechange = undefined;
            this.onerror = undefined;
        }
    };
    xhr.send(JSON.stringify(params));
    return async || xhr.responseText;
};

function _setProperty(obj, prop, val) {
    var args = {
        object: obj ? "nrdp." + obj : "nrdp",
        property: prop,
        value: (typeof val === "undefined" ? null : val)
    };
    _POST(args, nrdp._nbpdAsync);
};

function _invoke(obj, method, args) {
    if (!args) args = {};
    args.object = obj ? "nrdp." + obj : "nrdp";
    args.method = method;
    _POST(args, nrdp._nbpdAsync);
};

function _construct(obj, construct, args) {
    if (!args) args = {};
    args.object = obj ? "nrdp." + obj : "nrdp";
    args.construct = construct;
    var result = _POST(args, false);
    if (result)
        return JSON.parse(result);
    return undefined;
};

function _setupBackchannel() {
    /*
    var urlParams = location.search;
    var getParam = function(key) {
        var sIdx = urlParams.indexOf(key), sep, eIdx;
        if (sIdx !== -1) {
            sep = urlParams.charAt(sIdx - 1);
            if (!sep || sep === "?" || sep === "&") {
                eIdx = urlParams.indexOf("&", sIdx);
                return urlParams.substring(sIdx + (key.length + 1), eIdx !== -1 && eIdx || urlParams.length);
            }
        }
        return undefined;
    };
    var url = getParam("nbpdHost");
    if (url)
    {
        nbpdUrl = url;
    }
    else
    */
    {
        for (var i in document.scripts) {
            if (/^http.*js\/NetflixBridge.js$/.test(document.scripts[i].src)) {
                nbpdUrl = document.scripts[i].src.replace("js/NetflixBridge.js", "");
                break;
            }
        }
    }

    try {
        var evurl = nbpdUrl + "eventsource?objectFilter=-nrdp.gibbon.debugger&name=NBPDJS";
        evtsrc = new EventSource(evurl);
        evtsrc.onmessage = function(evt) { _gotJSON(evt.data); };
        evtsrc.onerror = function(evt) {
            evtsrc.close();
            nrdp._gotEvent({type: "EventSourceError"});
        };
        mongooseBackchannel.evtsrc = evtsrc;
    } catch (x) {
        NTRACE("Direct EventSource connection failed, using iframe");
        window.addEventListener("message",
                                function(e) {
                                    if (e.origin + "/" === nbpdUrl)
                                        _gotJSON(e.data);
                                },
                                false);

        iframe = document.createElement('iframe');
        iframe.style.setProperty("opacity", 0);
        iframe.width = iframe.height = 0;
        iframe.src = nbpdUrl + "html/iframe.html";
        document.body.appendChild(iframe);
    }

    return true;
};

var mongooseBackchannel = {
    name: "http",
    isNative: false,
    init: _setupBackchannel,
    setProperty: _setProperty,
    construct: _construct,
    invoke: _invoke
};

nrdp._backchannels.push(mongooseBackchannel);

})();
}
