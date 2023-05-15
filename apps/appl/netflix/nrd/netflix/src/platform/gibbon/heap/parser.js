#!/usr/bin/env node

var fs = require("fs");
var crypto = require("crypto");

function parse(filename)
{
    console.log("parsing", filename);
    try {
        var contents = fs.readFileSync(filename, { encoding: "utf8"} );
        var rep = contents.replace(/-?\bnan\b/g, "null");
        rep = rep.replace(/-?\binf\b/g, "null");
        rep = rep.replace(/\t/g, "\\t");
        var json = JSON.parse(rep);
    } catch (e) {
        console.error(e.message);
        process.exit(1);
    }
    if (typeof json !== "object") {
        console.error("invalid JSON.parse result of", filename, typeof json);
        process.exit(1);
    }
    var d = json.data;
    if (typeof d !== "object" || !(d instanceof Array)) {
        console.error("invalid data of", filename, typeof d);
        process.exit(1);
    }
    var sizes = {};
    for (var i = 0; i < d.length; ++i) {
        var size = d[i].cellSize;
        var cells = d[i].cells;
        if (typeof sizes[size] !== "object")
            sizes[size] = [];
        for (var j = 0; j < cells.length; ++j) {
            sizes[size].push(cells[j].data);
        }
    }
    return sizes;
}

function printTypes(name, data)
{
    console.log("  ", name);
    var types = {};
    for (var i = 0; i < data.length; ++i) {
        var t = data[i].type;
        if (typeof types[t] !== "number") {
            types[t] = 1;
        } else {
            ++types[t];
        }
    }
    for (var k in types) {
        console.log("    ", k, types[k]);
    }
}

function mapObjects(obj)
{
    var ret = {};
    for (var i = 0; i < obj.length; ++i) {
        if (obj[i].type === "Object") {
            var sha1 = crypto.createHash("sha1");
            var data = obj[i].object;
            for (var k in data) {
                sha1.update(k + "");
            }
            var digest = sha1.digest("base64");
            if (!ret.hasOwnProperty(digest)) {
                ret[digest] = [obj[i]];
            } else {
                ret[digest].push(obj[i]);
            }
        }
    }
    return ret;
}

function compareObjects(js1, js2)
{
    console.log("      js1 length", js1.length);
    var js1objs = mapObjects(js1);
    console.log("      js2 length", js2.length);
    var js2objs = mapObjects(js2);
    var has = [], k;
    for (k in js1objs) {
        if (!js2objs.hasOwnProperty(k)) {
            console.log("      js1 has", JSON.stringify(js1objs[k]));
        } else {
            has.push(k);
        }
    }
    for (k in js2objs) {
        if (!js1objs.hasOwnProperty(k)) {
            console.log("      js2 has", JSON.stringify(js2objs[k]));
        }
    }
    console.log("      both had", has.length);
}

function compare(js1, js2)
{
    var taken = {}, k;
    for (k in js1) {
        if (js1.hasOwnProperty(k)) {
            console.log("key", k);
            taken[k] = true;
            if (!js2.hasOwnProperty(k)) {
                console.log("  not present in js2");
                continue;
            }
            if (js1[k].length != js2[k].length) {
                console.log("  js1 length", js1[k].length, "js2 length", js2[k].length);
                // type statistics
                printTypes("js1", js1[k]);
                printTypes("js2", js2[k]);
                compareObjects(js1[k], js2[k]);
            }
        }
    }
    for (k in js2) {
        if (!taken.hasOwnProperty(k)) {
            console.error("key " + k + " was not present in js1");
        }
    }
}

if (process.argv.length < 4) {
    console.error("Need two files as arguments");
    process.exit(1);
}

var js1 = parse(process.argv[2]);
var js2 = parse(process.argv[3]);
compare(js1, js2);
