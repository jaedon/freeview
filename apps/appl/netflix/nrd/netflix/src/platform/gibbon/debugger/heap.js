var tree = undefined;
var flat = undefined;
var heapHasOwnProperty = Object.prototype.hasOwnProperty;

function processHeapData(parsed)
{
    var tmp = {};
    for (var k in parsed) {
        var item = parsed[k];
        if (item !== null) {
            //console.log(JSON.stringify(parsed[k]));
            switch (parsed[k].type) {
            case "String":
                tmp[k] = { data: parsed[k] };
                tmp[k].obj = tmp[k].data.data;
                break;
            case "Object":
                tmp[k] = { data: parsed[k] };
                switch (tmp[k].data.name) {
                case "Array":
                    tmp[k].obj = [];
                    break;
                default:
                    tmp[k].obj = {};
                    break;
                }
                break;
            case "Function":
                // record it as an object if it has an activation
                if (heapHasOwnProperty.call(parsed[k], "activations")) {
                    tmp[k] = { data: parsed[k], obj: { file: parsed[k].file, start: "" + parsed[k].start, end: "" + parsed[k].end } };
                    tmp[k].data.type = "Object";
                    tmp[k].data.name = "ActivationFunction";
                    tmp[k].data.object = { own: true, value: { activation: { type: "Object", id: parsed[k].activations[0] } } };
                }
                break;
            case "Unknown cell":
                // do nothing
                break;
            default:
                console.log("Unknown object " + parsed[k].type);
                break;
            }
        }
    }
    return tmp;
}

function makeTree()
{
    // postprocess, create the tree
    tree = [];
    var subs = {};
    for (var k in flat) {
        if (flat[k].data.type === "Object") {
            for (var l in flat[k].data.object) {
                var sub = flat[k].data.object[l];
                if (!sub || !sub.own) {
                    continue;
                }
                sub = sub.value;
                var cur;
                if (typeof sub === "object") {
                    if (sub === null || !sub.hasOwnProperty("id")) {
                        continue;
                    }
                    cur = flat[sub.id];
                    if (cur === undefined) {
                        //console.log("Didn't find item in toplevel: " + JSON.stringify(sub));
                        continue;
                    }
                    if (typeof cur === "object") {
                        cur = cur.obj;
                        if (cur === undefined) {
                            //console.log("cur was an object but cur.obj was not: " + JSON.stringify(sub));
                            continue;
                        }
                    }
                    subs[sub.id] = sub.id;
                } else {
                    cur = sub;
                }
                switch (flat[k].data.name) {
                case "Array":
                    flat[k].obj.push(cur);
                    break;
                default:
                    flat[k].obj[l] = cur;
                    break;
                }
            }
        }
    }
    // brute-force find the toplevels
    for (var k in flat) {
        if (flat[k].data.type === "Object") {
            if (!heapHasOwnProperty.call(subs, k)) {
                tree.push(flat[k].obj);
            }
        }
    }
}

function countKeys(obj)
{
    var cnt = 0;
    for (var k in obj) {
        ++cnt;
    }
    return cnt;
}

function objectsAreEqual(obj1, obj2)
{
    if (typeof obj1 !== typeof obj2)
        return false;
    if (typeof obj1 !== "object" || (obj1 === null || obj2 === null))
        return obj1 === obj2;
    // we assume no cyclic objects here
    for (var k in obj1) {
        if (!heapHasOwnProperty.call(obj2, k))
            return false;
        if (typeof obj1[k] !== typeof obj2[k])
            return false;
        if (typeof obj1[k] === "object") {
            if (!objectsAreEqual(obj1[k], obj2[k]))
                return false;
        } else if (obj1[k] !== obj2[k]) {
            return false;
        }
    }
    for (var k in obj2) {
        if (!heapHasOwnProperty.call(obj1, k))
            return false;
    }
    return true;
}

function mergeFlat(newflat, ctx)
{
    var tmp = {};
    for (var k in flat) {
        if (heapHasOwnProperty.call(newflat, k))
            tmp[k] = flat[k];
        else
            ctx.deleted.push(k);
    }
    for (var k in newflat) {
        if (!heapHasOwnProperty.call(tmp, k)) {
            ctx.created.push(k);
            tmp[k] = {};
        } else {
            if (!objectsAreEqual(tmp[k], newflat[k]))
                ctx.modified.push(k);
        }
        // keep obj from the old tree if present
        var mod = (!tmp[k].data || (tmp[k].data.type !== newflat[k].data.type || tmp[k].data.name !== newflat[k].data.name));
        tmp[k].data = newflat[k].data;
        if (!heapHasOwnProperty.call(tmp[k], "obj") || mod) {
            tmp[k].obj = newflat[k].obj;
        }
    }
    return tmp;
}

function diffTree(merged, ctx)
{
    var subs = [];
    for (var k in ctx.created) {
        var key = ctx.created[k];
        var sub = merged[key];
        if (sub.data.type === "Object") {
            // set it up
            for (var l in merged[key].data.object) {
                var subsub = merged[key].data.object[l];
                if (!subsub || !subsub.own) {
                    continue;
                }
                subsub = subsub.value;
                var cur;
                if (typeof subsub === "object") {
                    if (subsub === null || !subsub.hasOwnProperty("id")) {
                        continue;
                    }
                    cur = merged[subsub.id];
                    if (cur === undefined) {
                        //console.log("Didn't find item in toplevel: " + JSON.stringify(subsub));
                        continue;
                    }
                    if (typeof cur === "object") {
                        cur = cur.obj;
                        if (cur === undefined) {
                            //console.log("cur was an object but cur.obj was not: " + JSON.stringify(subsub));
                            continue;
                        }
                    }
                } else {
                    cur = subsub;
                }
                switch (merged[key].data.name) {
                case "Array":
                    merged[key].obj.push(cur);
                    break;
                default:
                    merged[key].obj[l] = cur;
                    break;
                }
            }
        }
    }

    // no need to look at deleted, we'll figure that out when we iterate over modified
    for (var k in ctx.modified) {
        var key = ctx.modified[k];
        var newobj = merged[key];
        var oldobj = flat[key];
        if (newobj.data.type !== oldobj.data.type) {
            // the entire thing has changed, update
            newobj.data = oldobj.data;
        } else if (newobj.data.type === "Object") {
            for (var l in newobj.data.object) {
                // just update?
                var sub = newobj.data.object[l];
                if (!sub || !sub.own) {
                    continue;
                }
                sub = sub.value;
                var cur;
                if (typeof sub === "object") {
                    if (sub === null || !sub.hasOwnProperty("id")) {
                        continue;
                    }
                    cur = merged[sub.id];
                    if (cur === undefined) {
                        //console.log("Didn't find item in toplevel: " + JSON.stringify(sub));
                        continue;
                    }
                    if (typeof cur === "object") {
                        cur = cur.obj;
                        if (cur === undefined) {
                            //console.log("cur was an object but cur.obj was not: " + JSON.stringify(sub));
                            continue;
                        }
                    }
                } else {
                    cur = sub;
                }
                switch (merged[key].data.name) {
                case "Array":
                    merged[key].obj.push(cur);
                    break;
                default:
                    merged[key].obj[l] = cur;
                    break;
                }
            }
            for (var l in oldobj.data.object) {
                if (!heapHasOwnProperty.call(newobj.data.object, l)) {
                    // deleted, remove from the object
                    delete newobj.obj[l];
                }
            }
        }
    }

    tree = [];
    // brute-force find the toplevels
    for (var k in merged) {
        if (merged[k].data.type === "Object") {
            for (var l in merged[k].data.object) {
                var sub = merged[k].data.object[l];
                if (!sub || !sub.own)
                    continue;
                if (sub.value === null || !sub.value.hasOwnProperty("id"))
                    continue;
                subs[sub.value.id] = sub.value.id;
            }
        }
    }
    for (var k in merged) {
        if (merged[k].data.type === "Object") {
            if (!heapHasOwnProperty.call(subs, k)) {
                tree.push(merged[k].obj);
            }
        }
    }
}

var idx = 0;

function go(data)
{
    console.log("going(ids) " + data.length);
    var parsed;
    try {
        parsed = JSON.parse(data);
    } catch (e) {
        console.log("JSON parse error");
        console.log(e);
        throw e;
    }
    var newflat = processHeapData(parsed);
    if (flat === undefined) {
        flat = newflat;
        makeTree();
    } else {
        var ctx = { deleted: [], created: [], modified: [] };
        var merged = mergeFlat(newflat, ctx);
        diffTree(merged, ctx);
        flat = merged;
        ctx = undefined;
    }
    console.log("gone " + tree.length);

    callLater("dump");
}

function dump()
{
    var fn = "/tmp/gibbon-snapshot-" + (idx++) + ".heapsnapshot";
    console.log("keys before serialize " + countKeys(flat));
    //console.log(JSON.stringify(flat));
    serialize(flat);
    flat = undefined;
    snapshot(fn);
    flat = deserialize();
    console.log("keys after deserialize " + countKeys(flat));
    //console.log(JSON.stringify(flat));
    notify(fn);
}

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

function start()
{
    var fs = require("fs");
    var d1 = fs.readFileSync("d1");
    var d2 = fs.readFileSync("d2");
    var o1 = JSON.parse(d1);
    var o2 = JSON.parse(d2);
    var h1 = {};
    var h2 = {};
    processHeapBlocks(o1.data, function(size, cell) {
        if (cell.id === undefined)
            return;
        h1[cell.id] = cell;
    });
    processHeapBlocks(o2.data, function(size, cell) {
        if (cell.id === undefined)
            return;
        h2[cell.id] = cell;
    });
    go(JSON.stringify(h1));
    go(JSON.stringify(h2));
}

// start();
