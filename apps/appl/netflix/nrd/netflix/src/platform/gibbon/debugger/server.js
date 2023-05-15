#!/usr/bin/env node
/*global require, Buffer, process, setTimeout*/
var http = require("http");
var urlParser = require("url");
var fs = require("fs");
var vm = require("vm");
var WebSocketServer = require("ws").Server;
var net = require("net");
var crypto = require("crypto");
var os = require("os");
var nbpdHost = "127.0.0.1";
var nbpdPort = "6359";
var nrdappVersion = undefined;
var connectCount = 0, maxConnectCount = 5;
var port = 8085;
var scene = {};
var isolateheap;
var nocomplete = false;
try {
    isolateheap = new (require("isolateheap")).isolateheap();
} catch (e) {
    console.log("isolateheap not built, the heap snapshot feature will not work.");
}

var deepDiffMapper = {
    VALUE_CREATED: 'created',
    VALUE_UPDATED: 'updated',
    VALUE_DELETED: 'deleted',
    VALUE_UNCHANGED: 'unchanged',
    map: function(obj1, obj2) {
        if (this.isValue(obj1) || this.isValue(obj2)) {
            return {
                _diff_type: obj1 === obj2 ? this.VALUE_UNCHANGED : this.VALUE_UPDATED,
                _diff_data: obj2
            };
        }

        var diff = {};
        var key;
        for (key in obj1) {
            if (!obj1.hasOwnProperty(key))
                continue;

            if (!obj2.hasOwnProperty(key)) {
                diff[key] = { _diff_type: this.VALUE_DELETED, _diff_data: obj1[key] };
                continue;
            }

            diff[key] = this.map(obj1[key], obj2[key]);
        }
        for (key in obj2) {
            if (!obj2.hasOwnProperty(key))
                continue;

            if (!obj1.hasOwnProperty(key)) {
                diff[key] = { _diff_type: this.VALUE_CREATED, _diff_data: obj2[key] };
                continue;
            }
        }

        return diff;
    },
    isValue: function(obj) { return !(obj instanceof Object) || typeof obj === 'function'; }
};

function trim(str) {
    if(str)
        str = str.replace(/^\s\s*/, '').replace(/\s\s*$/, '');
    return str;
}

function toNumber(num, base, totalDigits) {
    num = num.toString(base);
    if(base === undefined)
        base = 16;
    if(totalDigits === undefined)
        totalDigits = num.length;
    var pad = '';
    if(totalDigits > num.length) {
        for (i=0; i < (totalDigits-num.length); i++)
            pad += '0';
    }
    return pad + num;
}

function fractionalSeconds(ms) {
    //return Math.floor(ms / 1000) + ((ms % 1000) / 1000);
    return ms;
}

var EventSource = function(url) {
    var conn = net.createConnection(url.port, url.hostname, function() {
        if (self._ons.connect)
            self._ons.connect();
        conn.write("GET " + url.path + " HTTP/1.1\r\nConnection: keep-alive\r\nHost: " + url.hostname + "\r\n\r\n");
    });
    var self = this;
    conn.setEncoding("utf8");
    conn.on("data", function(data) {
        self._data += data;
        var onmsg = self._ons.message;
        do {
            var idx = self._data.indexOf("\r\n\r\n");
            if (idx === -1)
                break;
            var msg = self._data.substr(0, idx);
            self._data = self._data.substr(idx + 4);
            msg = msg.split("\r\n");
            var fin = "";
            var evt = "message";
            for (var i in msg) {
                var cl = msg[i].indexOf(":");
                if (cl !== -1) {
                    var type = msg[i].substr(0, cl);
                    if (type == "data")
                        fin += msg[i].substr(cl + 1);
                    else if (type === "event")
                        evt = msg[i].substr(cl + 1);
                }
            }
            if (onmsg !== undefined && fin.length > 0)
                onmsg({data: fin, event: evt});
        } while (true);
    });
    conn.on("end", function() {
        if (self._ons.end)
            self._ons.end();
    });
    conn.on("error", function(err) {
        if (self._ons.error)
            self._ons.error(err);
        else
            throw err;
    });
    this._connection = conn;
    this._data = "";
    this._ons = {};
    this.on = function(msg, handler) {
        this._ons[msg] = handler;
    };
};

function sendMessage(message, ws)
{
    if (typeof(message) === "object")
        message = JSON.stringify(message);
    //console.log("->" + message);
    if(ws) {
        try {
            ws.send(message);
        } catch (e) {
            console.log("Couldn't send message, dropping:");
            console.log(e);
        }
    } else {
        for (var client in clients) {
            try {
                clients[client].send(message);
            } catch (e) {
                console.log("Couldn't send message, dropping:");
                console.log(e);
            }
        }
    }
}

function sendTimeline(timeline, counters)
{
    if(timeline) {
        var param_counters;
        if(counters) {
            param_counters = {};
            if(counters.jsc !== undefined) {
                // param_counters.jsHeapSizeUsed = counters.jsc.fastMalloc - counters.jsc.fastMalloc_free;
                // param_counters.jsHeapSizeUsed += counters.jsc.osAllocatorCommitted - counters.jsc.heap;
                // param_counters.jsHeapSizeUsed += counters.jsc.heap;
                param_counters.jsHeapSizeUsed = counters.jsc.heap;
            }
            if(counters.views !== undefined)
                param_counters.nodes = counters.views;
        }
        var record = { method: "Timeline.eventRecorded",
                       params: {
                           record: {
                               type: timeline.type,
                               data: timeline.data,
                               startTime: timeline.started,
                               counters: param_counters
                           }
                       }};
        if(timeline.children) {
            record.params.record.children = [];
            for(var c in timeline.children) {
                var child = timeline.children[c];
                record.params.record.children.push({
                    type: child.type,
                    data: child.data,
                    startTime: child.started,
                    endTime: child.ended,
                    counters: counters
                });
            }
        }
        if(timeline.ended)
            record.params.record.endTime = timeline.ended;
        sendMessage(record);
    }
}

function guessMimeType(path) {
    var i = path.lastIndexOf(".");
    var ext = (i < 0) ? '' : path.substr(i);
    if (ext === ".css")
        return "text/css";
    else if (ext === ".js")
        return "text/javascript";
    else if (ext === ".html")
        return "text/html";
    else if (ext === ".png")
        return "image/png";
    return "application/octet-stream";
}

function serve(path, response) {
    if (path.indexOf("..") != -1) {
        console.log("serving 400 for " + path);
        response.writeHead(400, {"Content-Type": "text/plain"});
        response.end("Invalid path");
        return;
    }
    fs.readFile("front-end" + path,
                function(err, data) {
                    if (err) {
                        console.log("serving 404 for " + path);
                        response.writeHead(404, {"Content-Type": "text/plain"});
                        response.end(err.message);
                    } else {
                        response.writeHead(202, {"Content-Type": guessMimeType(path)});
                        response.end(data);
                    }
                });
};

function redirect(path, response) {
    response.writeHead(303, {"Location": path});
    response.end();
}

function makeRect(quad) {
    if (quad[3] !== quad[1]) // not a rect
        return null;
    if (quad[4] !== quad[2]) // not a rect
        return null;
    if (quad[5] !== quad[7]) // again
        return null;
    if (quad[0] !== quad[6]) // you guessed it
        return null;
    return { x: quad[0], y: quad[1], width: quad[2] - quad[0], height: quad[5] - quad[1] };
}

var ws;
var timeline = 0;
var properties = {};
var waiting = [];
var page = 0;
var clients = [];
var objects = {};
var heapUid = 0;
var snapshots = {};

function processHeapBlocks(data, callback)
{
    var len = data.length;
    console.log("length " + len);
    for (var i = 0; i < len; ++i) {
        switch (data[i].type) {
        case "Block":
            var cellSize = data[i].cellSize;
            var cells = data[i].cells;
            if (cellSize && cells) {
                var cellLen = cells.length;
                for (var j = 0; j < cellLen; ++j) {
                    //var id = cells[j].id;
                    if (cells[j].type !== "Cell") {
                        console.log("Invalid cell type: '" + cells[j].type + "'");
                        continue;
                    }
                    callback(cellSize, cells[j].data);
                }
            }
            break;
        default:
            console.log("Unrecognized block type: '" + data[i].type + "' in " + JSON.stringify(data[i]));
            break;
        }
    }
}

function sendSnapshot(uid, data, ws)
{
    console.log("called back!");

    var result = { method: "HeapProfiler.addHeapSnapshotChunk",
                   params: { uid: uid, chunk: data } };
    sendMessage(result, ws);

    for (var i in waiting) {
        if (waiting[i].heap !== undefined && waiting[i].heap.params.uid === uid) {
            console.log("found waiting heap " + JSON.stringify(waiting[i].heap));
            result = { id: waiting[i].id };
            sendMessage(result, waiting[i].ws);

            waiting.splice(i, 1);
            return;
        }
    }
}

var idx = 0;
function processHeapData(uid, ws)
{
    if (!isolateheap) {
        console.log("isolateheap not built, the heap snapshot feature will not work.");
        return;
    }

    var snapshot = snapshots[uid];
    var data = snapshot.json;
    var rep = data.replace(/-?\bnan\b/g, "null");
    rep = rep.replace(/-?\binf\b/g, "null");
    rep = rep.replace(/\t/g, "\\t");
    console.log("writing index " + idx);
    //fs.writeFileSync("/tmp/gibbonheap.pre" + (idx++), rep);
    if (idx >= 10)
        idx = 0;
    var parsed;
    try {
        parsed = JSON.parse(rep);
    } catch (e) {
        console.log("JSON parse error");
        console.log(e);
        throw e;
    }

    var json = "{";
    processHeapBlocks(parsed.data, function(size, cell) {
                          if (cell.id === undefined) {
                              //console.log("No id for " + JSON.stringify(cell));
                              return;
                          }
                          //snapshot.ids[cell.id] = cell;
                          // if (json.length > 1)
                          //     json += ",";
                          json += (json.length > 1 ? ",\"" : "\"") + cell.id + "\":" + JSON.stringify(cell);
                      });
    json += "}";
    snapshot.json = undefined;

    if (!isolateheap.isRunning()) {
        var script = fs.readFileSync("heap.js", { encoding: "utf8" });
        if (!script) {
            console.log("No heap.js found");
            return;
        }

        console.log("script " + script.length + " " + (typeof script));

        console.log("starting isolate thread");
        isolateheap.run(script);
    }
    console.log("calling in isolate");
    isolateheap.call("go", function(fn) {
        var data = fs.readFileSync(fn, { encoding: "utf8" });
        sendSnapshot(uid, data, ws);
    }, json);
    console.log("called");
}

var cpuProfileCount = 0;
var cpuSampleInterval = 0;

var handlers = { "Inspector.enable": null,
                 "Profiler.causesRecompilation": true,
                 "Profiler.enable": null,
                 "Profiler.isSampling": false,
                 "Profiler.hasHeapProfiler": (isolateheap !== undefined),
                 "Profiler.setSamplingInterval": function(msg) {
                     cpuSampleInterval = msg.params.interval;
                 },
                 "Profiler.start": function(msg) {
                     sendRequest({ method: "startCPUProfiler", profileId: ++cpuProfileCount });
                 },
                 "Profiler.stop": function(msg) {
                     sendRequest({ method: "stopCPUProfiler", profileId: cpuProfileCount });
                     return { wait: true, cpuProfileId: cpuProfileCount };
                 },
                 "Runtime.enable": null,
                 "DOMStorage.enable": null,
                 "Database.enable": null,
                 "Page.enable": null,
                 "Page.reload": function(msg) {
                     sendRequest({ method: "reload" });
                 },
                 "Page.canOverrideDeviceMetrics": false,
                 "Page.canOverrideGeolocation": false,
                 "Page.canOverrideDeviceOrientation": false,
                 "Page.getResourceTree": function(msg) {
                     var frameTree = { frame: { url: "http://gibbon/main",
                                                mimeType: "text/html",
                                                id: "100",
                                                loaderId: "101"
                                              },
                                       resources: [] };
                     if (properties.sources) {
                         for (var i in properties.sources)
                             frameTree.resources.push({ url: i, mimeType: "text/javascript", type: "Script" });
                     }
                     return { frameTree: frameTree };
                 },
                 "Page.setTouchEmulationEnabled": null,
                 "Page.getResourceContent": function(msg) {
                     var url = msg.params.url;
                     sendRequest({ method: "resource", url: url });
                     return { wait: true, url: url };
                 },
                 "Page.setShowPaintRects": function(msg) {
                     sendRequest({ method: "enableRender", types: [ { name: "drawWidgetRectangles", value: msg.params.result } ] });
                 },
                 "Page.setShowDebugBorders": function(msg) {
                     sendRequest({ method: "enableRender", types: [ { name: "drawWidgetCache", value: msg.params.result } ] });
                 },
                 "Page.setShowFPSCounter": function(msg) {
                     sendRequest({ method: "enableRender", types: [ { name: "drawScreenFrameGraph", value: msg.params.result } ] });
                 },
                 "Page.setContinuousPaintingEnabled": function(msg) {
                     sendRequest({ method: "enableRender", types: [ { name: "testAnimationNoIdle", value: msg.params.result } ] });
                 },
                 "Timeline.supportsFrameInstrumentation": true,
                 "Timeline.start": function(msg) {
                     timeline++;
                     sendRequest({ method: "enableProfile"});
                 },
                 "Timeline.stop": function(msg) {
                     timeline--;
                     sendRequest({ method: "disableProfile"});
                 },
                 "Timeline.setIncludeMemoryDetails": null,
                 "Network.canClearBrowserCache": true,
                 "Network.canClearBrowserCookies": true,
                 "Network.clearBrowserCookies": function(msg) {
                     sendRequest({ method: "clearNetworkCookies"});
                 },
                 "Network.clearBrowserCache": function(msg) {
                     sendRequest({ method: "clearNetworkCache"});
                 },
                 "Network.enable": function(msg) {
                     sendRequest({ method: "enableNetwork"});
                 },
                 "Network.getResponseBody": function(msg) {
                     var base64 = true;
                     var request = networkEvents[msg.params.requestId];
                     if(request.response.headers && request.response.headers["Content-Type"]) {
                         var mimetype = trim(request.response.headers["Content-Type"]);
                         if(mimetype.indexOf("application/") == 0 || mimetype.indexOf("text/") == 0)
                             base64 = false;
                     }
                     if(base64)
                         return { body: request.response.data, base64Encoded: true };
                     var buf = new Buffer(request.response.data, "base64");
                     return { body: buf.toString("utf8"), base64Encoded: false };
                 },
                 "Network.loadResourceForFrontend": function(msg) {
                     var url = msg.params.url;
                     sendHttpRequest(url, handleHttpResourceResponse);
                     return { wait: true, url: url };
                 },
                 "Debugger.getScriptSource": function(msg) {
                     var url = msg.params.scriptId;
                     sendHttpRequest(url, handleHttpScriptResponse);
                     return { wait: true, url: url };
                 },
                 "Debugger.enable": null,
                 "Debugger.causesRecompilation": true,
                 "Debugger.supportsSeparateScriptCompilationAndExecution": false,
                 "Debugger.canSetScriptSource": false,
                 "Debugger.setPauseOnExceptions": function(msg) {
                     var ignore = (msg.params.state == "none") ? 0 : ((msg.params.state == "all") ? 1 : 2);
                     sendRequest({ method: "ignoreExceptions", ignore: ignore });
                 },
                 "Debugger.evaluateOnCallFrame": function(msg) {
                     var frame = parseInt(msg.params.callFrameId) + 1;
                     var expr = msg.params.expression;
                     var byValue = msg.params.returnByValue;
                     sendRequest({ method: "evaluateOnCallFrame", frame: frame, expression: expr });
                     return { wait: true, byValue: byValue, ident: { frame: frame, expression: expr } };
                 },
                 "Debugger.setBreakpointByUrl": function(msg) {
                     var line = msg.params.lineNumber + 1;
                     var colum = msg.params.columnNumber;
                     var url = msg.params.url;
                     var condition = msg.params.condition;
                     sendRequest({ method: "addBreakpoint", url: url, condition: condition, line: line });
                     return { breakpointId: url + ":" + line, locations: [] };
                 },
                 "Debugger.removeBreakpoint": function(msg) {
                     var id = msg.params.breakpointId;
                     if (!id)
                         return;
                     var key;
                     for (var i in properties.breakpoints) {
                         key = properties.breakpoints[i].url + ":" + properties.breakpoints[i].line;
                         if (key === id) {
                             sendRequest({ method: "removeBreakpoint", id: parseInt(i) + 1 });
                             return;
                         }
                     }
                     console.log("Breakpoint not found: " + id);
                 },
                 "Debugger.setSkipAllPauses": function(msg) {
                     var skipped = msg.params.skipped;
                     var untilReload = msg.params.untilReload;
                     sendRequest({ method: "setSkipAllPauses", skipped: skipped, untilReload: untilReload });
                 },
                 "Debugger.pause": function(msg) {
                     sendRequest({ method: "pause" });
                 },
                 "Debugger.stepOver": function(msg) {
                     sendRequest({ method: "stepOver" });
                 },
                 "Debugger.stepInto": function(msg) {
                     sendRequest({ method: "stepInto" });
                 },
                 "Debugger.stepOut": function(msg) {
                     sendRequest({ method: "finish" });
                 },
                 "Debugger.resume": function(msg) {
                     sendRequest({ method: "continueFromBreakpoint" });
                     var resp = { method: "Debugger.resumed",
                                  params: { }
                                };
                     sendMessage(resp);
                 },
                 "Console.enable": null,
                 "Runtime.evaluate": function(msg) {
                     var expr = msg.params.expression;
                     var grp = msg.params.objectGroup;
                     var byValue = msg.params.returnByValue;
                     var noPause = msg.params.doNotPauseOnExceptionsAndMuteConsole;
                     sendRequest({ method: "evaluate", expression: expr, group: grp });
                     return { wait: true, noPause: noPause, expression: expr, byValue: byValue };
                 },
                 "Runtime.callFunctionOn": function(msg) {
                     console.log("callFunctionOn");
                     console.log(msg);
                     var func = eval("(" + msg.params.functionDeclaration + ")");
                     var res = func.apply(objects[msg.params.objectId], msg.params.arguments);
                     console.log(JSON.stringify({ result: { type: "object", objectId: msg.params.objectId, value: res }, wasThrown: false }));
                     return { result: { type: "object", objectId: msg.params.objectId, value: res }, wasThrown: false };
                 },
                 "Runtime.getProperties": function(msg) {
                     if (msg.params.accessorPropertiesOnly)
                         return {};
                     sendRequest({ method: "getProperties", objectId: msg.params.objectId, ownProperties: msg.params.ownProperties });
                     return { wait: true, objectId: msg.params.objectId };
                 },
                 "CSS.enable": null,
                 "CSS.getSupportedCSSProperties": { cssProperties: [] },
                 "CSS.getComputedStyleForNode": function(msg) {
                     var widget = findWidget(scene, msg.params.nodeId);
                     var computedStyle = [];
                     if(widget) {
                         var cssProperties = convertObjectToCSS(widget);
                         for(var i = 0; i < cssProperties.length; i++) {
                             var property = cssProperties[i];
                             if(property.name.indexOf("render") == 0)
                                 computedStyle.push({name: property.name, value: property.value, active: true});
                         }
                     }
                     return { computedStyle: computedStyle };
                 },
                 "CSS.getInlineStylesForNode": function(msg) {
                     var widget = findWidget(scene, msg.params.nodeId);
                     var inlineStyle = { styleId: { widget: msg.params.nodeId }, cssProperties: [], shorthandEntries: [] };
                     if(widget) {
                         var cssProperties = convertObjectToCSS(widget);
                         for(var i = 0; i < cssProperties.length; i++) {
                             var property = cssProperties[i];
                             if(property.name.indexOf("render") != 0)
                                 inlineStyle.cssProperties.push({name: property.name, value: property.value, active: true, styleBased: true});
                         }
                         inlineStyle.width = widget.width;
                         inlineStyle.height = widget.height;
                     }
                     return { inlineStyle: inlineStyle };
                 },
                 "CSS.getMatchedStylesForNode": function(msg) {
                     var widget = findWidget(msg.params.nodeId);
                     var matchedCSSRules = { id: msg.params.nodeId, cssProperties: [], shorthandEntries: [] };
                     var inherited = { id: msg.params.nodeId, cssProperties: [], shorthandEntries: [] };
                     var pseudoElements = { id: msg.params.nodeId, cssProperties: [], shorthandEntries: [] };
                     if(widget) {

                     }
                     return { matchedCSSRules: matchedCSSRules, inherited: inherited, pseudoElements: pseudoElements };
                 },
                 "CSS.setPropertyText": function(msg) {
                     var colon = msg.params.text.indexOf(':'), semicolon = msg.params.text.lastIndexOf(';');
                     if(colon != -1 && semicolon > colon) {
                         var property = msg.params.text.substr(0, colon);
                         var value = trim(msg.params.text.substr(colon+1, semicolon-colon-1));
                         if(property == "background")
                             property = "backgroundColor";
                         var rgb_re, rgb;
                         {
                             rgb_re = /rgb\(([0-9]*), *([0-9]*), *([0-9]*) *\)/;
                             rgb = rgb_re.exec(value);
                             if(rgb)
                                 value = { r: rgb[1], g: rgb[2], b: rgb[3] };
                         }
                         {
                             rgb_re = /rgba\(([0-9]*), *([0-9]*), *([0-9]*), *([0-9.]*) *\)/;
                             rgb = rgb_re.exec(value);
                             if(rgb)
                                 value = { r: rgb[1], g: rgb[2], b: rgb[3], a: rgb[4] * 255 };
                         }
                         {
                             var url_re = /url\( *(.*) *\)/;
                             var url = url_re.exec(value);
                             if(url)
                                 value = url[1];
                         }
                         //console.log("Set:" + msg.params.styleId.widget + ":" + property + "->" + JSON.stringify(value))
                         sendRequest({object: "nrdp.gibbon.widgets.widget" + msg.params.styleId.widget, property: property, value: value});

                         // var widget = findWidget(msg.params.nodeId);
                         // var style = { styleId: { widget: msg.params.styleId.widget }, cssProperties: [], shorthandEntries: [] };
                         // if(widget) {
                         //     var cssProperties = convertObjectToCSS(widget);
                         //     for(var i = 0; i < cssProperties.length; i++) {
                         //         var property = cssProperties[i];
                         //         if(property.name.indexOf("render") != 0)
                         //             style.cssProperties.push({name: property.name, value: property.value, active: true, styleBased: true});
                         //     }
                         //     style.width = widget.width;
                         //     style.height = widget.height;
                         // }
                         // return { style: style };
                     }
                 },
                 "DOM.getDocument": function(msg) {
                     sendRequest({ method: "scene", "id": msg.id });
                     return { wait: true, scene_id: msg.id };
                 },
                 "DOM.highlightNode": function(msg) {
                     sendRequest({ method: "overlay", "widget": msg.params.nodeId, replace: true });
                 },
                 "DOM.hideHighlight": function(msg) {
                     sendRequest({ method: "overlay"});
                 },
                 "DOM.highlightRect": function(msg) {
                     sendRequest({ method: "overlay", "x": msg.params.x, "y": msg.params.y, "width": msg.params.width, "height": msg.params.height, replace: true });
                 },
                 "DOM.highlightQuad": function(msg) {
                     // verify that the quad is a rect
                     var rect = makeRect(msg.params.quad);
                     if (!rect)
                         return;
                     sendRequest({ method: "overlay", "x": rect.x, "y": rect.y, "width": rect.width, "height": rect.height, replace: true });
                 },
                 "DOM.requestNode": function(msg) {
                     return { nodeId: parseInt(msg.params.objectId.substr(7)) }; // strip 'widget:' before parsing the int
                 },
                 "DOMDebugger.setDOMBreakpoint": function(msg) {
                     sendRequest({ method: "addDOMBreakpoint", "widget": msg.params.nodeId, type: msg.params.type });
                 },
                 "DOMDebugger.removeDOMBreakpoint": function(msg) {
                     sendRequest({ method: "removeDOMBreakpoint", "widget": msg.params.nodeId, type: msg.params.type });
                 },
                 "HeapProfiler.takeHeapSnapshot": function(msg, ws) {
                     sendDataRequest("/gibbon/mem/js/", function(url, data) {
                                         console.log("got data " + data.length);
                                         var uid = ++heapUid;
                                         var result = { method: "HeapProfiler.addProfileHeader",
                                                        params: { header: { title: "Profile " + uid,
                                                                            uid: uid,
                                                                            maxJSObjectId: 1000 } } };
                                         sendMessage(result, ws);
                                         snapshots[uid] = { json: data, ids: {} };
                                     });
                 },
                 "HeapProfiler.getHeapSnapshot": function(msg, ws) {
                     var uid = msg.params.uid;
                     processHeapData(uid, ws);
                     return { wait: true, heap: msg };
                 }
               };

function handleWait(result, id, ws)
{
    if (result && result.wait) {
        result.id = id;
        result.ws = ws;
        waiting.push(result);
        return true;
    }
    return false;
}

function handleMessage(ws, msg)
{
    //console.log("Handling message:" + msg.id + ": " + msg.method);
    var result = handlers[msg.method];
    var resp = { id: msg.id };
    if (result === undefined) {
        resp.error = "Unknown method '" + msg.method + "'";
        console.log(resp.error);
    } else if (result !== null) {
        if (typeof(result) === "function") {
            resp.result = result(msg, ws);
            if (handleWait(resp.result, msg.id, ws))
                return;
        } else if (typeof(result) === "object") {
            if (handleWait(result, msg.id, ws))
                return;
            resp.result = result;
        } else
            resp.result = { result: result };
    }
    sendMessage(resp, ws);
}

function handleHttpScriptResponse(url, body)
{
    for (var i in waiting) {
        if (waiting[i].url === url) {
            var data = { id: waiting[i].id, result: { scriptSource: body } };
            sendMessage(data, waiting[i].ws);
            waiting.splice(i, 1);
            return;
        }
    }
}

function handleHttpResourceResponse(url, body)
{
    for (var i in waiting) {
        if (waiting[i].url === url) {
            var data = { id: waiting[i].id,
                         result: { status: body.length > 0 ? 200 : 404,
                                   content: body, headers: {} } };
            sendMessage(data, waiting[i].ws);
            waiting.splice(i, 1);
            return;
        }
    }
}

function resolveNetflixHosts(url)
{
    if (url.host === "localcontrol.netflix.com") {
        url.hostname = nbpdHost;
        url.port = nbpdPort;
    } else {
        url.hostname.replace(os.hostname(), "127.0.0.1");
    }
}

function sendHttpRequest(url, callback)
{
    var parsedUrl = urlParser.parse(url);
    resolveNetflixHosts(parsedUrl);
    var opts = { host: parsedUrl.hostname,
                 port: parsedUrl.port ? parsedUrl.port : 80,
                 headers: {
                     "Connection": "close"
                 },
                 path: parsedUrl.pathname };

    var req = http.request(opts, function(response) {
        var data = "";
        response.setEncoding("utf8");
        response.on("data", function(body) {
            data += body;
        });
        response.on("end", function() {
            callback(url, data);
        });
    });
    req.on("error", function(err) {
        console.log("http request error for " + url);
        console.log(err);
        callback(url, "");
    });
    req.end();
}

function sendDataRequest(path, callback)
{
    var url = "http://" + nbpdHost + ":" + nbpdPort + path;
    sendHttpRequest(url, callback);
}

function sendRequest(data)
{
    if (nocomplete && data.method === "evaluate" && data.group === "completion")
        return;
    if(!data.object)
        data.object = "nrdp.gibbon.debugger";
    var payload = JSON.stringify(data);

    var opts = { host: nbpdHost, port: nbpdPort, method: "POST", headers:
                 { "Content-Type": "text/plain",
                   "Content-Length": Buffer.byteLength(payload, 'utf8'),
                   "Connection": "close" },
                 path: "/ipc" };

    var request = http.request(opts, function(response) {
        response.on("end", function() {
            console.log("ipc request complete");
        });
    });
    request.on("error", function(err) {
        console.log("ipc request error for: " + payload);
        console.log(err);
    });
    request.end(payload);
}

function sendState(ws)
{
    if (properties.sources !== undefined) {
        var src = { method: "Debugger.scriptParsed" };
        for (var k in properties.sources) {
            src.params = { scriptId: k, url: k, isContentScript: true, sourceMapURL: properties.sources[k] };
            sendMessage(src, ws);
        }
    }

    if (properties.state === "stopped" || properties.state === "exception") {
        var resp = { method: "Debugger.paused",
                     params: {
                         reason: "other",
                         callFrames: []
                     }
                   };
        for (var j in properties.stack) {
            var frameno = parseInt(j);
            var frame = properties.stack[j];
            var scopeChain = [];
            if (properties.scopeChain) {
                scopeChain = [{type: "local", object: { type: "object", objectId: "scope+1:" + (frameno + 1), description: "object" }},
                              {type: "global", object: { type: "object", objectId: "scope+0:" + (frameno + 1), description: "object" }}];
                var scopeCount = properties.scopeChain[frameno];
                for (var i = 2; i < scopeCount; ++i) {
                    scopeChain.push({type: "closure", object: { type: "object", objectId: ("scope+" + i + ":" + (frameno + 1)), description: "object"}});
                }
            }
            resp.params.callFrames.push({ callFrameId: j, functionName: frame.function, location: { lineNumber: frame.line - 1, columnNumber: frame.column, scriptId: frame.url }, scopeChain: scopeChain, this: null });
        }
        sendMessage(resp, ws);
    }
}

function handleDebuggerMessage(prop)
{
    //console.log(prop);
    for (var i in prop) {
        properties[i] = prop[i];
    }
    if (prop.state === "stopped" || prop.state === "exception") {
        // broken
        var resp = { method: "Debugger.paused",
                     params: {
                         reason: "other",
                         callFrames: []
                     }
                   };
        for (var j in properties.stack) {
            var frameno = parseInt(j);
            var frame = properties.stack[j];
            var scopeChain = [];
            if (properties.scopeChain) {
                scopeChain = [{type: "local", object: { type: "object", objectId: "scope+1:" + (frameno + 1), description: "object" }},
                              {type: "global", object: { type: "object", objectId: "scope+0:" + (frameno + 1), description: "object" }}];
                var scopeCount = properties.scopeChain[frameno];
                for (var k = 2; k < scopeCount; ++k) {
                    scopeChain.push({type: "closure", object: { type: "object", objectId: ("scope+" + k + ":" + (frameno + 1)), description: "object"}});
                }
            }
            resp.params.callFrames.push({ callFrameId: j, functionName: frame.function, location: { lineNumber: frame.line - 1, columnNumber: frame.column, scriptId: frame.url }, scopeChain: scopeChain, this: null });
        }
        sendMessage(resp);
    }
    if (prop.sources !== undefined) {
        var src = { method: "Debugger.scriptParsed" };
        for (var l in prop.sources) {
            src.params = { scriptId: l, url: l, isContentScript: true, sourceMapURL: prop.sources[l] };
            sendMessage(src);
        }
    }
}

function handleResourceResponse(resp)
{
    for (var i in waiting) {
        if (waiting[i].url === resp.data.url) {
            if (resp.data.data !== undefined) {
                var buf = new Buffer(resp.data.data, "base64");
                var data = { id: waiting[i].id, result: { content: buf.toString("utf8"), base64Encoded: false } };
                sendMessage(data, waiting[i].ws);
                waiting.splice(i, 1);
                return;
            }
        }
    }
}

function handleInstrumentationEvent(resp)
{
    if(timeline) {
        var name = resp.instrumentation[0];
        var started = fractionalSeconds(resp.instrumentation[1]);
        var data = resp.instrumentation[4];
        var ended = fractionalSeconds(resp.instrumentation[1]+resp.instrumentation[5]);
        var marks = resp.instrumentation[7];
        if(resp.counters.surfaceCacheUsed !== undefined)
            sendTimeline({type: "GPUTask", started: started, ended: ended, data: { usedGPUMemoryBytes: resp.counters.surfaceCacheUsed }});
        if(name == "gibbon.jsc.callback" && data.name) {
            data.type = data.name;
            sendTimeline({type: "EventDispatch", data: data, started: started, ended: ended}, resp.counters);
        } else if(name == "gibbon.layout") {
            sendTimeline({type: "Layout", started: started, ended: ended}, resp.counters);
        } else if(name == "gibbon.render") {
            if(data.dirtyRect && data.root) {
                sendTimeline({type: "BeginFrame", started: started}, resp.counters);
                sendTimeline({type: "Paint",
                              started: started, ended: ended,
                              data: { "x": data.dirtyRect[0], "y": data.dirtyRect[1], "width": data.dirtyRect[2], "height": data.dirtyRect[3] }},
                             resp.counters);
            }
        } else if(name == "gibbon.graphics.render") {
            var children = [];
            for(var markType in marks) {
                var mark = marks[markType];
                for(var m in mark) {
                    children.push({type: markType,
                                   started: fractionalSeconds(resp.instrumentation[1]+mark[m][0]),
                                   ended:  fractionalSeconds(resp.instrumentation[1]+mark[m][0]+mark[m][1])});
                }
            }
            children.sort(function(a,b){return a.started-b.started;});
            sendTimeline({type: "Rasterize", started: started, ended: ended, children: children}, resp.counters);
        } else {
            sendTimeline({type: name, started: started, ended: ended}, resp.counters);
        }
    }
}

function handleLogEvent(resp)
{
    //console.log("Log: " + JSON.stringify(resp));
    if (resp.traceArea != "INSTRUMENTATION" && resp.traceArea != "LOG") { //timeline event
        var message = { method: "Console.messageAdded",
                        params: {
                            message: {
                                text: resp.message,
                                source: "other"
                            }
                        }
                      };
        if (resp.level == "trace" || resp.level == "debug")
            message.params.message.level = "log";
        else if(resp.level == "info" || resp.level == "warn")
            message.params.message.level = "warning";
        else if(resp.level == "error" || resp.level == "fatal")
            message.params.message.level = "error";
        if (resp.traceArea == "UI_SCRIPT")
            message.params.message.source = "console-api";
        else if (resp.traceArea == "GIBBON_NETWORK")
            message.params.message.source = "network";
        else
            message.params.message.text = resp.traceArea + ": " + resp.message;
        var callstack;
        if (resp.tags.callstack && resp.tags.callstack.length) {
            callstack = JSON.parse(resp.tags.callstack);
            if (Array.isArray(callstack) && callstack.length) {
                message.params.message.stackTrace = [];
                for (var i = 0; i < callstack.length; i++)
                    message.params.message.stackTrace.push({functionName: callstack[i].name,
                                                            lineNumber: 0, url: callstack[i].name, columnNumber: 0});
            }
        }
        if (!callstack) {
            if (resp.tags.stack && typeof resp.tags.stack === "string") {
                callstack = resp.tags.stack.split('\n');
            } else if (resp.callstack && typeof resp.callstack === "string") {
                callstack = resp.callstack.split('\n');
            }
            if (callstack) {
                message.params.message.stackTrace = callstack.map(
                    function(e) {
                        // function@[native code]
                        // or function@url:line
                        var re = /^([^@]*)@(\[.*\]|(.+):(\d+))/;
                        var arr = e.match(re);
                        if (arr && arr.length >= 5) {
                            return {
                                functionName: arr[1],
                                lineNumber: arr[4],
                                scriptId: arr[3],
                                url: arr[3],
                                columnNumber: 0
                            };
                        } else {
                            return {
                                functionName: e,
                                lineNumber: 0,
                                scriptId: e,
                                url: e,
                                columnNumber: 0
                            };
                        }
                    });
            }
        }
        sendMessage(message);
    }
}

var networkEvents = {};
function handleNetworkEvent(resp)
{
    if (resp.joined)
        return;

    var message;
    resp.request += "";
    var now_ms = fractionalSeconds(resp.now);
    var now_seconds = fractionalSeconds(resp.now / 1000);
    //console.log("Network(" + resp.request + "): " + resp.type + " " + resp.url + " " + now_ms + " " + resp.now + " " + resp.statusCode);
    if(resp.type == "begin") {
        networkEvents[resp.request] = { request: resp };
        sendMessage(
            { method: "Network.requestWillBeSent",
              params: {
                  requestId: resp.request,
                  loaderId: resp.request,
                  documentUrl: resp.location,
                  timestamp: now_seconds,
                  initiator: { type: "other" },
                  request: {
                      method: resp.method,
                      postData: resp.data,
                      url: resp.url,
                      headers: resp.headers
                  }
              }});
        sendTimeline({type: "ResourceSendRequest",
                      data: {
                          requestId: resp.request,
                          url: resp.url,
                          requestMethod: resp.method
                      },
                      started: now_ms});
        sendMessage(
            { method: "Network.dataReceived",
              params: {
                  requestId: resp.request,
                  timestamp: now_seconds
              }});
    } else if(resp.type == "end" && networkEvents[resp.request]) {
        networkEvents[resp.request].response = resp;

        if(resp.data) {
            var encodedDataLength = (new Buffer(resp.data, "base64")).toString("utf8").length;
            var dataLength = encodedDataLength;
            if(resp.headers) {
                var contentLength = parseInt(trim(resp.headers["Content-Length"]));
                if(contentLength)
                    dataLength = contentLength;
            }
            sendMessage(
                { method: "Network.dataReceived",
                  params: {
                      requestId: resp.request,
                      timestamp: now_seconds,
                      dataLength: dataLength,
                      encodedDataLength: encodedDataLength
                  }});
        }

        if(resp.memory)
            sendMessage(
                { method: "Network.requestServedFromMemoryCache",
                  params: {
                      requestId: resp.request,
                      initiator: { type: "other" }
                  }});
        if(resp.disk)
            sendMessage(
                { method: "Network.requestServedFromCache",
                  params: {
                      requestId: resp.request
                  }});
        if(resp.error) {
            sendMessage(
                { method: "Network.loadingFailed",
                  params: {
                      requestId: resp.request,
                      timestamp: now_seconds,
                      errorText: resp.error.string
                  }});
        } else {
            var mimeType = trim(resp.headers["Content-Type"]);
            if (typeof mimeType === "string")
                mimeType = mimeType.split(';', 1)[0];
            message = { method: "Network.responseReceived",
                        params: {
                            requestId: resp.request,
                            loaderId: resp.url,
                            documentUrl: resp.location,
                            timestamp: now_seconds,
                            response: {
                                headers: resp.headers,
                                fromDiskCache: resp.disk,
                                headers: resp.headers,
                                mimeType: mimeType,
                                status: resp.statusCode,
                                statusText: "" //can we do better?
                            }
                        }};
            if(resp.kind == "data")
                message.params.type = "XHR";
            else if(resp.kind == "surface")
                message.params.type = "Image";
            else if(resp.kind == "script")
                message.params.type = "Script";
            sendMessage(message);
        }
        sendMessage(
            { method: "Network.loadingFinished",
              params: {
                  requestId: resp.request,
                  timestamp: now_seconds
              }});
        sendTimeline({type: "ResourceReceiveResponse",
                      data: {
                          requestId: resp.request,
                          url: resp.url,
                          status: resp.statusCode,
                          mimeType: trim(resp.headers ? resp.headers["Content-Type"] : "text/plain")
                      },
                      started: now_ms});
    }
}

function handleWidgetChanged_internal(properties, oldScene)
{
    var resp;
    var oldWidget;
    if (properties.hasOwnProperty("parent")) {
        oldWidget = findWidget(oldScene, properties.id);
        if (oldWidget && oldWidget.parent) {
            var oldParent = findWidget(oldScene, oldWidget.parent);
            resp = { method: "DOM.childNodeRemoved",
                     params: { parentNodeId: oldWidget.parent,
                               nodeId: oldWidget.id
                             }
                   };
            for (var c in oldParent.children) {
                if (oldParent.children[c].id == properties.id) {
                    oldParent.children.splice(c, 1);
                    break;
                }
            }
            sendMessage(resp);
        }
        if (properties.parent) {
            var widget = properties;
            var parent = findWidget(oldScene, properties.parent);
            if (!parent || !parent.hasOwnProperty("children") || parent.children.length === 0) {
                resp = { method: "DOM.setChildNodes",
                         params: { parentId: properties.parent,
                                   nodes: [convertWidgetToDOM(widget)]
                                 }
                       };
            } else {
                resp = { method: "DOM.childNodeInserted",
                         params: { parentNodeId: properties.parent,
                                   node: convertWidgetToDOM(widget)
                                 }
                       };
                if (parent.children) {
                    resp.params.previousNodeId = parent.children[parent.children.length - 1].id;
                }
            }
            if (parent) {
                if (!parent.children)
                    parent.children = [];
                parent.children.push(widget);
                addParents(widget, properties.parent);
            }
            sendMessage(resp);
        }
    } else {
        var sent = false;
        for (var p in properties) {
            if (p === "text") {
                resp = { method: "DOM.attributeModified",
                         params: { nodeId: properties.id,
                                   name: p,
                                   value: properties[p] + ""
                                 }
                       };
                sendMessage(resp);
                sent = true;
            }
        }
        if (!sent) {
            resp = { method: "DOM.attributeModified",
                     params: { nodeId: properties.id,
                               name: "",
                               value: ""
                             }
                   };
            sendMessage(resp);
        }

        oldWidget = findWidget(oldScene, properties.id);
        if (oldWidget) {
            for (var i in properties) {
                if (i !== "children" && i !== "parent") {
                    oldWidget[i] = properties[i];
                }
            }
        }

        for (var cc in properties.children) {
            handleWidgetChanged_internal(properties.children[cc], oldScene);
        }
    }
}


function handleWidgetChanged(properties)
{
    handleWidgetChanged_internal(properties, scene);
}

function handleWidgetDestroyed(id)
{
    var oldWidget = findWidget(scene, id);
    if (!oldWidget)
        return;

    var resp;
    var oldParent = findWidget(scene, oldWidget.parent);
    if (!oldParent) {
        if (oldWidget.parent) {
            resp = { method: "DOM.childNodeRemoved",
                     params: { parentNodeId: oldWidget.parent,
                               nodeId: oldWidget.id
                             }
                   };
            sendMessage(resp);
        }

        console.log("parent " + oldWidget.parent + " not found for widget " + id);
        return;
    }
    for (var c in oldParent.children) {
        if (oldParent.children[c].id == id) {
            oldParent.children.splice(c, 1);
            break;
        }
    }

    resp = { method: "DOM.childNodeRemoved",
             params: { parentNodeId: oldWidget.parent,
                       nodeId: oldWidget.id
                     }
           };
    sendMessage(resp);
}

function findWidget(widget, id)
{
    if(widget) {
        if(widget.id == id)
            return widget;
        var children = widget.children;
        if(children) {
            for(var child = 0; child < children.length; ++child) {
                var found = findWidget(children[child], id);
                if(found)
                    return found;
            }
        }
    }
    return undefined;
}

function prefixify(prefix, name)
{
    if (prefix.length === 0)
        return name;
    var ret = "";
    for (var i in prefix) {
        ret = ret + prefix[i] + "-";
    }
    return ret + name;
}

function convertObjectToCSS_helper(object, result, prefix)
{
    var p, properties, sub;
    for (var v in object) {
        var value = undefined, name = v;
        if(v == "parent" || v == "children" || v == "name") {
            //drop
        } else if(v == "images") {
            var images = object[v], bgc = 0, fgc = 0;
            for (var i in images) {
                if (images[i].background)
                    convertObjectToCSS_helper(images[i], result, prefix.concat(["backgroundImage[" + (bgc++) + "]" ]));
                else
                    convertObjectToCSS_helper(images[i], result, prefix.concat(["image[" + (fgc++) + "]" ]));
            }
        } else if(v == "src") {
            value = "url(" + object[v] + ")";
        } else if(v == "backgroundColor") {
            name = "background";
            if(object[v].a)
                value = "rgba(" + object[v].r + ", " + object[v].g + ", " + object[v].b + ", " + (255 / object[v].a) + ")";
        } else if(v == "backgroundImage" || v == "image") {
            if(object[v].src && object[v].src.length)
                value = "url(" + object[v].src + ")";
        } else if(v == "padding") {
            if(object[v].left || object[v].top || object[v].right || object[v].bottom)
                value = object[v].left + " " + object[v].top + " " + object[v].right + " " + object[v].bottom;
        } else if(v == "text") {
            if(object[v].length)
                value = object[v];
        } else if(v == "textStyle") {
            if(object.text && object.text.length) {
                properties = [ "family", "size", "weight", "variant", "align", "wrap", "maxLines", "lineHeight", "underline", "richText", "locale" ];
                for(p in properties) {
                    sub = object[v][properties[p]];
                    if(sub !== null && sub !== undefined)
                        result.push({name: prefixify(prefix, "textStyle-" + properties[p]), value: sub + ""});
                }
            }
        } else if(v == "layout") {
            if(object.layout) {
                if (object.layout.layout) {
                    result.push({name: prefixify(prefix, "layout"), value: object.layout.layout + ""});
                }
                if (object.layout.align) {
                    result.push({name: prefixify(prefix, "layout-align"), value: object.layout.align + ""});
                }
            }
        } else if(typeof(object[v]) === "object") {
            convertObjectToCSS_helper(object[v], result, prefix.concat([v]));
        } else if(object[v] !== null && object[v] !== undefined) {
            value = object[v] + "";
        }
        if(value !== undefined)
            result.push({name: prefixify(prefix, name), value: value + ""});
    }
}

function convertObjectToCSS(object)
{
    var result = [];
    convertObjectToCSS_helper(object, result, []);
    return result;
}

function convertWidgetToDOM(widget)
{
    var name = "empty";
    var result = { nodeType: 1, nodeValue: "", attributes: [], children: [], childrenNodeCount: 0 };
    if(widget) {
        name = "widget" + widget.id;
        for(var v in widget) {
            if(widget[v] === undefined || widget[v] === null)
                continue;
            if(v == "parent") { //drop
            } else if(v == "id") {
                result.nodeId = widget[v];
            } else if(v == "children") {
                result.childrenNodeCount = widget[v].length;
                for(var child = 0; child < result.childrenNodeCount; ++child)
                    result.children.push(convertWidgetToDOM(widget[v][child]));
            } else if(v == "name") {
                if(widget[v] && widget[v].length)
                    name = widget[v];
            } else if(v == "image" || v == "backgroundImage") {
                if(widget[v] && widget[v].src && widget[v].src.length) {
                    result.attributes.push(v);
                    result.attributes.push(widget[v].src);
                }
            } else if(v == "text") {
                if(widget[v] && widget[v].length) {
                    result.nodeValue = widget[v];
                    result.attributes.push(v);
                    result.attributes.push(widget[v]+"");
                }
                // } else if(typeof(widget[v]) != "object"){
                //     result.attributes.push(v);
                //     result.attributes.push(widget[v]+"");
            }
        }
    }
    result.nodeName = name;
    result.localName = name;
    result.name = name;
    return result;
}

function addParents(widget, parent)
{
    if (parent !== undefined) {
        widget.parent = parent;
    }
    for (var c in widget.children) {
        addParents(widget.children[c], widget.id);
    }
}

function handleSceneResponse(resp)
{
    scene = resp.data.root;
    // walk and add parents
    if (scene !== undefined)
        addParents(scene);

    for (var i in waiting) {
        if (waiting[i].scene_id === resp.data.id) {
            var data = { id: waiting[i].scene_id };
            data.result = {
                root: {
                    nodeId: 0,
                    nodeType: 9,
                    name: "scene",
                    nodeName: "scene",
                    localName: "scene",
                    baseURL: "http://localcontrol.netflix.com/uiboot/",
                    childrenNodeCount: 1,
                    children: [convertWidgetToDOM(resp.data.root)]
                }
            };
            sendMessage(data, waiting[i].ws);
            waiting.splice(i, 1);
            return;
        }
    }
}

function handleWidgetClicked(resp)
{
    var data = { method: "Inspector.inspect",
                 params: { object: { type: "object", objectId: ("widget:" + resp.widgets[0]), subtype: "node" } }
               };
    sendMessage(data);
}

function adjustCPUSampleEntry(entry, total, sample)
{
    entry.hitCount = ((entry.hitCount / 1000) * sample) || 1;
    for (var idx = 0; idx < entry.children.length; ++idx) {
        adjustCPUSampleEntry(entry.children[idx], total, sample);
    }
}

function adjustCPUSamples(profile, sample)
{
    var head = profile.head;
    var total = head.self;
    profile.startTime /= 1000;
    profile.endTime /= 1000;
    adjustCPUSampleEntry(head, total, sample);
}

function handleCPUProfile(resp)
{
    for (var i in waiting) {
        if (waiting[i].cpuProfileId === resp.data.profileId) {
            var profile = JSON.parse(resp.data.json);
            adjustCPUSamples(profile, cpuSampleInterval);
            //fs.writeFileSync("/tmp/cpuprofile", JSON.stringify(profile, null, 4));
            var data = { id: waiting[i].id, result: { profile: profile } };
            sendMessage(data, waiting[i].ws);
            waiting.splice(i, 1);
            return;
        }
    }
}

function handleGetProperties(resp)
{
    for (var i in waiting) {
        if (waiting[i].objectId === resp.objectId) {
            var data = { id: waiting[i].id };
            data.result = { result: resp.data };

            sendMessage(data, waiting[i].ws);
            waiting.splice(i, 1);
            return;
        }
    }
}

function handleEvaluate(resp)
{
    for (var i in waiting) {
        if (waiting[i].expression === resp.expression) {
            var data = { id: waiting[i].id, error: resp.error };
            data.result = { result: { type: typeof(resp.result), value: resp.result }, wasThrown: resp.error !== undefined };
            if (data.result.result.value === null) {
                data.result.result.value = undefined;
            } else if (data.result.result.type === "object") {
                data.result.result.objectId = resp.expression;
                data.result.result.description = "object";
                objects[resp.expression] = resp.result;
            }
            sendMessage(data, waiting[i].ws);
            waiting.splice(i, 1);
            return;
        }
    }
}

function handleEvaluateOnCallFrame(resp)
{
    for (var i in waiting) {
        if (waiting[i].ident !== undefined) {
            if (waiting[i].ident.frame == resp.ident.frame
                && waiting[i].ident.expression == resp.ident.expression) {
                var data = { id: waiting[i].id, error: resp.error };
                data.result = { result: { type: typeof(resp.result.result), value: resp.result.result }, wasThrown: resp.result.wasThrown };
                if (data.result.result.value === null) {
                    data.result.result.value = undefined;
                } else if (data.result.result.type === "object") {
                    data.result.result.objectId = resp.ident.expression + ":" + resp.ident.frame;
                    data.result.result.description = "object";
                    objects[resp.ident.expression + ":" + resp.ident.frame] = resp.result.result;
                }
                //console.log("Sending " + JSON.stringify(data));
                sendMessage(data, waiting[i].ws);
                waiting.splice(i, 1);
                return;
            }
        }
    }
}

var server = http.createServer(
    function(request, response) {
        //console.log(request.url);
        //console.log(request.headers);
        //console.log("-----");
        var url = urlParser.parse(request.url, true);
        if (url.pathname === "/")
            redirect("/inspector.html?page=" + (page++), response);
        else
            serve(url.pathname, response);
    }
);

for(var i = 0; i < process.argv.length; ++i) {
    if (process.argv[i] === "-h") {
        nbpdHost = process.argv[++i];
        var colon = nbpdHost.indexOf(":");
        if(colon != -1) {
            nbpdPort = nbpdHost.substr(colon+1);
            nbpdHost = nbpdHost.substr(0, colon);
        }
    } else if(process.argv[i] === "-p") {
        port = process.argv[++i];
    } else if (process.argv[i] === "-c") {
        nocomplete = true;
    }
}
function eventSourceConnect()
{
    ++connectCount;
    var es = new EventSource(urlParser.parse("http://" + nbpdHost + ":" + nbpdPort + "/eventsource?objectFilter=nrdp.gibbon.debugger&raw=true", true));
    es.on("message", function(event) {
        //console.log("<-" + event.data);
        try {
            var obj = JSON.parse(event.data);
        } catch (e) {
            console.log("unable to parse JSON from eventsource: '%s'", event);
            //console.log(event);
        }
        if (obj && obj.object === "nrdp.gibbon.debugger") {
            if (obj.type === "PropertyUpdate") {
                handleDebuggerMessage(obj.properties);
            } else if (obj.type === "Event") {
                if (obj.name === "ResourceResponse") {
                    handleResourceResponse(obj);
                } else if (obj.name === "SceneResponse") {
                    handleSceneResponse(obj);
                } else if (obj.name === "NetworkEvent") {
                    handleNetworkEvent(obj.data);
                } else if (obj.name === "EnableDebugResponse") {
                    nrdappVersion = obj.data; //use this to make decisions
                    console.log(" Version: " + obj.data);
                } else if (obj.name === "SceneChangedEvent") {
                    sendMessage({method: "DOM.documentUpdated"});
                } else if (obj.name === "WidgetChangedEvent") {
                    handleWidgetChanged(obj.data);
                } else if (obj.name === "WidgetDestroyedEvent") {
                    handleWidgetDestroyed(obj.data);
                } else if (obj.name === "LocationChangedEvent") {
                    sendMessage({method: "DOM.documentUpdated"});
                    sendMessage({method: "Console.messagesCleared"});
                    sendMessage({method: "Page.frameNavigated", params: { url: obj.data, frame: { id: "100" } }});
                    sendMessage({method: "Runtime.isolatedContextCreated", params: { id: 1, isPageContext: true } });
                    //sendMessage({method: "Page.loadEventFired"});
                    sendMessage({method: "Debugger.globalObjectCleared"});
                } else if (obj.name === "LogEvent") {
                    handleLogEvent(obj.data);
                } else if (obj.name === "InstrumentationEvent") {
                    handleInstrumentationEvent(obj.data);
                } else if (obj.name === "EvaluateOnCallFrame") {
                    handleEvaluateOnCallFrame(obj.data);
                } else if (obj.name === "Evaluate") {
                    handleEvaluate(obj.data);
                } else if (obj.name === "GetProperties") {
                    handleGetProperties(obj.data);
                } else if (obj.name === "WidgetClicked") {
                    handleWidgetClicked(obj.data);
                } else if (obj.name === "CPUProfile") {
                    handleCPUProfile(obj.data);
                } else {
                    console.log("Unhandled EventSourceEvent: " + obj.name);
                }
            }
        }
    });
    es.on("connect", function() {
        connectCount = 0;
        console.log("Connected to: " + nbpdHost + ":" + nbpdPort);
        sendRequest({ method: "enableDebug" });

        server.listen(port);
        console.log("Listening on " + os.hostname() + ":" + port);
        ws = new WebSocketServer({server: server});
        ws.on("connection", function(ws) {
            var pageUrl = ws.upgradeReq.url;
            var slashIdx = pageUrl.lastIndexOf("/");
            var page = (slashIdx < 0) ? 0 : parseInt(pageUrl.substr(slashIdx + 1));
            console.log("Inspector connected!");
            setTimeout(function() { sendState(ws); }, 1000);
            clients.push(ws);
            ws.on("message", function(msg) {
                try {
                    handleMessage(ws, JSON.parse(msg));
                } catch (err) {
                    console.log("Error parsing WebSocket message " + err);
                    console.log(msg);
                }
            });
            ws.on("close", function(code) {
                console.log("Inspector disconnected(" + code + ")!");
                var idx = clients.indexOf(ws);
                if (idx != -1)
                    clients.splice(idx, 1);
                else
                    console.log("Unable to find socket in list of clients!");
            });
        });
    });
    es.on("end", function() {
        console.log("NBPD closed!");
        for (var client in clients)
            clients[client].close();
        if(ws) {
            ws.close();
            ws = undefined;
        }
        setTimeout(eventSourceConnect, 5000);
    });
    es.on("error", function(err) {
        console.log("[" + connectCount + "] Unable to connect(" + nbpdHost + ":" + nbpdPort + "):" + err);
        if(err.code == "ECONNREFUSED")
            console.log("Is NBPD running?");
        if(connectCount == maxConnectCount)
            process.exit();
        else
            setTimeout(eventSourceConnect, 5000);
    });
}
eventSourceConnect();
