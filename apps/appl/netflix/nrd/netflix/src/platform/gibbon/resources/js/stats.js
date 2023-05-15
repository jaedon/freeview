var startTime = new Date().getTime();

if (typeof String.prototype.startsWith != 'function') {
    String.prototype.startsWith = function (str){
	return this.slice(0, str.length) == str;
    };
}
if (typeof String.prototype.endsWith != 'function') {
    String.prototype.endsWith = function (str){
	return this.slice(-str.length) == str;
    };
}

var memory_chart;
var event_backlog_chart;
var instrumentation_chart;

function prettyBytes(bytes)
{
    if(bytes === undefined)
        bytes = 0;
    var negative = bytes < 0;
    if(negative) 
        bytes = -bytes;
    var unit = 'B';
    if(bytes > 1024) {
        bytes /= 1024;
        unit = 'KB';
    }
    if(bytes > 1024) {
        bytes /= 1024;
        unit = 'MB';
    }
    if(bytes > 1024) {
        bytes /= 1024;
        unit = 'GB';
    }
    if(negative)
        bytes = -bytes;
    var num = bytes.toFixed(2);
    if(parseInt(num) == num)
        num = parseInt(num);
    return num + unit;
}

var _nbpdUrl;
function nbpdUrl() {
    if(!_nbpdUrl) {
	var port;
	var hostname = window.location.host;
	var h = hostname.split(":");
	hostname = h[0];
	if(h[1].length)
	    port = h[1];
	else
	    port = 6359;
	var query = window.location.search;
	if(query && query.length) {
	    query = query.substring(1);
	    var vars = query.split('&');
	    for (var i = 0; i < vars.length; i++) {
		var pair = vars[i].split('=');
		if(decodeURIComponent(pair[0]) == "nbpd") {
		    var h = decodeURIComponent(pair[1]).split(":");
		    if(h[0].length)
			hostname = h[0];
		    if(h[1].length)
			port = h[1];
		    break;
		}
	    }
	}
	_nbpdUrl = "http://" + hostname + ":" + port;
    }
    return _nbpdUrl;
}

var snapshotDiff;
var snapshots = [];
var snapshotCostType;
var snapshotCellConstantCost = 16;
var snapshot_visible = { old: true, new: true, changed: true };
function updateSnapshotVisible(type) {
    var summary = document.getElementById('snapshot_diff');
    var items = summary.querySelectorAll('.snapshot_' + type + '_item');
    for (var i = 0; i < items.length; i++) 
        items[i].style.display = snapshot_visible[type] ? 'list-item' : 'none';
}
function updateSnapshotCost() {
    snapshotCostType = document.getElementById('snapshot_cost_type').value;
    var items = document.getElementsByClassName('snapshot_item_cost');
    for(var i = 0; i < items.length; ++i) {
        var cost = items[i], li;
        for(var p = cost.parentNode; p; p = p.parentNode) {
            if(p.className.indexOf(' snapshot_item') != -1) {
                cost.innerHTML = prettySnapshotItemCost(p.item, p.item_type);
                break;
            }
        }
    }
}
function toggleSnapshotVisible(type) {
    snapshot_visible[type] = !snapshot_visible[type];
    updateSnapshotVisible(type);
}
function snapshotItemCost(item) {
    if(item.cost instanceof Object) {
        var cost = item.cost.self;
        if(item.cost.direct !== undefined)
            cost += item.cost.direct;
        if(item.cost.indirect !== undefined && snapshotCostType == "indirect")
            cost += item.cost.indirect;
        return cost;
    }
    return item.cost;
}
function prettySnapshotItemCost(item, type) {
    var bytes = snapshotItemCost(item);
    if(snapshotCostType == "direct_percent") {
        var total = snapshots[1].cost-snapshots[0].cost;
        if(type && snapshotDiff.counts && snapshotDiff.counts[type]) 
            total = snapshotDiff.counts[type + "_cost"];
        return (bytes / total * 100).toFixed(2) + "%";
    }
    return prettyBytes(bytes);
}
function snapshotItemValue(value) {
    if(value instanceof Object) {
        if(value.type == "String") {
            return "\"" + value.data + "\"";
        } else if(value.type == "Function") {
            if(value.name)
                return value.name;
            return value.file + "[" + value.start + "-" + value.end + "]";
        }
        return (value ? value.id : "undefined");
    }
    return value;
}
function snapshotItem(item, type) 
{
    var li = document.createElement("li");
    li.className = "snapshot_" + type + "_item snapshot_item";
    li.item_type = type;
    li.item = item;

    var children = 0;
    if(item.value instanceof Object && type == "new") {
        if(item.value.type == "Function") {
            if(item.value.activations)
                children += item.value.activations.length;
        } else if(item.value.type == "Object" && item.value.id) {
            if(item.value.object) {
                for(var i in item.value.object)
                    ++children;
            }
        }
        if(children)
            li.id = item.value.id;
    }
    {
        var html = "";
        if(children) 
            html += "<a class='nounderline' href='javascript:snapshotItemExpand(\"" + li.id + "\")'>";
        html += "(<div id='cost' class='snapshot_item_cost'>" + prettySnapshotItemCost(item, type) + "</div>) " + item.name + " [";
        if(type == "changed") 
            html += snapshotItemValue(item.oldValue) + " vs " + snapshotItemValue(item.value);
        else
            html += snapshotItemValue(item.value);
        html += "]";
        if(children) 
            html += " [" + children + " children]" + "</a>";
        li.innerHTML = html;
    }
    return li;
}
function snapshotSortItems(obj) {
    var arr = [];
    for(var prop in obj) {
        if (obj.hasOwnProperty(prop)) 
            arr.push({ key: prop, value: obj[prop] });
    }
    arr.sort(function(a, b) { return b.value.size - a.value.size; });
    return arr; 
}
function snapshotItemExpand(id) {
    var li = document.getElementById(id);
    //console.log(li.item.snapshot.objects[id]);
    if(!li.expansion) {
        li.expansion = document.createElement("ul");
        li.expansion.style.display = 'block';
        li.appendChild(li.expansion);

        var snapshot = li.item.snapshot;
        var item = snapshot.objects[id];

        var objects = [];
        if(item.cell.data.type == "Function") {
            if(item.cell.data.activations) {
                for(var activation = 0; activation < item.cell.data.activations.length; ++activation)
                    objects.push(snapshot.objects[item.cell.data.activations[activation]]);
            }
        } else {
            objects.push(item);
        }
        
        if(objects.length) {
            var items = [];
            for(var i = 0; i < objects.length; ++i) {
                object = objects[i];
                for(var key in object.cell.data.object) {
                    var value = object.cell.data.object[key].value;
                    if(value instanceof Object) {
                        var subItem = snapshot.objects[value.id];
                        if(subItem && subItem.cell)
                            items.push({ name: key, snapshot: snapshot, value: subItem.cell.data, cost: subItem.cost });
                    } else {
                        items.push({ name: key, snapshot: snapshot, value: value, cost: snapshotCellConstantCost });
                    }
                }
            }
            items.sort(function(a, b) { return snapshotItemCost(b) - snapshotItemCost(a); });
            for(var i = 0; i < items.length; ++i) 
                li.expansion.appendChild(snapshotItem(items[i], "new"));
        }
    } else {
        if(li.expansion.style.display == "none")
            li.expansion.style.display = "block";
        else
            li.expansion.style.display = "none";
    }
}
function updateSnapshotDiff() {
    function snapshotChargeCost(snapshot, item) {
        if(!item.references || item.cost.charged)
            return;
        item.cost.charged = true;
        var direct_cost = item.cost.self / item.references.length;
        var indirect_cost = item.cost.self - direct_cost;
        for(var r = 0; r < item.references.length; ++r) {
            var reference_id = item.references[r];
            var reference_item = snapshot.objects[reference_id];
            reference_item.cost.direct += direct_cost;
            reference_item.cost.indirect += indirect_cost;
            snapshotChargeCost(snapshot, reference_item);
        }
    }
    function snapshotCellInit(snapshot, cell, cellSize) {
        function snapshotAddReference(id) {
            if(!snapshot.objects[id])
                snapshot.objects[id] = { references: [] };
            else if(!snapshot.objects[id].references)
                snapshot.objects[id].references = [];
            snapshot.objects[id].references.push(cell.id);
        }

        var result = snapshot.objects[cell.id];
        if(!result) {
            result = {};
            snapshot.objects[cell.id] = result;
        } else if(result.cell) {
            console.error("Found same cell twice?");
        }
        result.cell = cell;
        result.cost = { cellSize: cellSize, self: cellSize, direct: 0, indirect: 0 };

        if(cell && cell.data) {
            if(cell.data.type == "String") {
                result.cost.self += (cell.data.data.length+1) * 2;
            } else {
                if(cell.data.type == "Function") {
                    if(cell.data.end !== undefined && cell.data.start !== undefined) 
                        result.cost.self += (cell.data.end - cell.data.start) * 2;
                    if(cell.data.activations) {
                        for(var activation = 0; activation < cell.data.activations.length; ++activation) 
                            snapshotAddReference(cell.data.activations[activation]);
                    }
                }
            }
            if(cell.data.object) {
                for(var key in cell.data.object) {
                    var subObject = cell.data.object[key];
                    if(subObject && subObject.own) {
                        if(subObject.value && subObject.value.id)
                            snapshotAddReference(subObject.value.id);
                        else
                            result.cost.direct += snapshotCellConstantCost;
                        result.cost.self += (key.length+1) * 2;
                    }
                }
            }
        }
    }
    function snapshotInit(snapshot) {
        //First pass - create the cells
        snapshot.objects = {};
        snapshot.counts = { cells_cost: 0, cells: 0, referenced: 0, referenced_cost: 0 };
        var parsed = snapshot.parsed;
        if(!parsed)
            parsed = JSON.parse(snapshot.contents);
        for(var data = 0; data < parsed.data.length; ++data) {
            var block = parsed.data[data];
            var cells = block.cells;
            var cellSize = block.cellSize;
            for(var i = 0; i < cells.length; ++i) {
                snapshotCellInit(snapshot, cells[i], cellSize);
                ++snapshot.counts.cells;
            }
        }

        //Second pass - charge up the cost
        for(var s in snapshot.objects) {
            var item = snapshot.objects[s];
            if(!item || !item.cost)
                continue;
            snapshotChargeCost(snapshot, item);
            snapshot.counts.cells_cost += item.cost.self;

            var referenced = false;
            if(item.cell.data.name == "GlobalObject") {
                snapshot.globalObject = item.cell.data;
                referenced = true;
            } else if(item.references) {
                referenced = true;
            }
            if(referenced) {
                ++snapshot.counts.referenced;
                snapshot.counts.referenced_cost += item.cost.self;
            } else {
                if(!snapshot.unknown)
                    snapshot.unknown = [];
                snapshot.unknown.push(item);
            }
        }
    }
    function snapshotWalk(path, snapshot, data, other) {
        var otherObject = other.objects[data.id];
        if(!otherObject) {
            var item = snapshot.objects[data.id];
            snapshotDiff.items[path] = { snapshot: snapshot, name: path, value: data, cost: item ? item.cost : snapshotCellConstantCost };
            return;
        }

        if(path.length)
            path += ".";
        for(var key in data.object) {
            var object = data.object[key];
            if(!object.own)
                continue;
            var subPath = path+key;
            var subItem = (object.value instanceof Object) ? snapshot.objects[object.value.id] : undefined;

            if(subItem) {
                if(!subItem.paths)
                    subItem.paths = [];
                subItem.paths.push(subPath);
            }
            if(!otherObject.cell || !otherObject.cell.data.object[key]) {
                if(subItem) 
                    snapshotDiff.items[subPath] = { snapshot: snapshot, name: subPath, value: subItem.cell.data, cost: subItem.cost };
                else 
                    snapshotDiff.items[subPath] = { snapshot: snapshot, name: subPath, value: object.value, cost: snapshotCellConstantCost };
            } else if(subItem) {
                if(object.value.type == "String") {
                    var subOtherObject;
                    if(otherObject.cell.data.object[key].value)
                        subOtherObject = other.objects[otherObject.cell.data.object[key].value.id];
                    if(!subOtherObject)
                        snapshotDiff.items[subPath] = { snapshot: snapshot, name: subPath, value: subItem.cell.data, cost: subItem.cost };
                    else if(subItem.cell.data.data != subOtherObject.cell.data.data)
                        snapshotDiff.items[subPath] = { snapshot: snapshot, name: subPath, value: subItem.cell.data, oldValue: subOtherObject.cell.data, cost: subItem.cost };
                } else {
                    if(!snapshotDiff.seen[object.value.id]) {
                        snapshotDiff.seen[object.value.id] = 1;
                        snapshotWalk(subPath, snapshot, subItem.cell.data, other);
                    }
                }
            } else if(object.value != otherObject.cell.data.object[key].value) {
                snapshotDiff.items[subPath] = { snapshot: snapshot, name: subPath, value: object.value, oldValue: otherObject.cell.data.object[key].value, cost: snapshotCellConstantCost };
            }
        }
    }

    var snapshot_summary = document.getElementById('snapshot_summary');
    var snapshot_diff = document.getElementById('snapshot_diff');
    snapshot_summary.innerHTML = "No delta";
    snapshot_diff.innerHTML = "";

    var snapshot1_index = document.getElementById('snapshot_1').selectedIndex;
    var snapshot2_index = document.getElementById('snapshot_2').selectedIndex;
    if(snapshot1_index != snapshot2_index) {
        var snapshot1 = snapshots[snapshot1_index];
        var snapshot2 = snapshots[snapshot2_index];
        for(var snapshot_index = 0; snapshot_index < snapshots.length; ++snapshot_index) {
            var snapshot = snapshots[snapshot_index];
            if(snapshot_index == snapshot1_index || snapshot_index == snapshot2_index) {
                snapshotInit(snapshot);
            } else { //free up some memory
                snapshot.objects = undefined;
                snapshot.globalObject = undefined;
            }
            snapshot.parsed = undefined; 
        }
        snapshotDiff = { counts: { new: 0, new_cost: 0, old: 0, old_cost: 0, changed: 0, changed_cost: 0 }, 
                         items: {}, seen: {} };

        if(snapshotDiffType == "unreachable") {
            function mark(snapshot, data) {
                for(var key in data.object) {
                    var object = data.object[key];
                    if(!object.own)
                        continue;
                    var subItem = (object.value instanceof Object) ? snapshot.objects[object.value.id] : undefined;
                    if(subItem && subItem.cell && subItem.cell.data && !subItem.marked) {
                        subItem.marked = true;
                        mark(snapshot, subItem.cell.data);
                    }
                }
            }
            mark(snapshot2, snapshot2.globalObject);
            for(var o in snapshot2.objects) {
                var item = snapshot2.objects[o];
                if(item) {
                    if(!item.cell || !item.cell.data || item.cell.data.type == "API value wrapper" || item.cell.data.type == "Unknown cell")
                        continue;
                    if(!item.marked && !snapshot1.objects[o]) 
                        snapshotDiff.items[o] = { snapshot: snapshot2, name: o, value: item.cell.data, cost: item.cost };
                }
            }
        } else {
            var globalObject1 = snapshot1.globalObject;
            var globalObject2 = snapshot2.globalObject;

            snapshotWalk("", snapshot1, globalObject1, snapshot2);
            snapshotDiff.seen = {}; //reset it for the next walk
            snapshotWalk("", snapshot2, globalObject2, snapshot1);
        }

        var items = [];
        for(var path in snapshotDiff.items) 
            items.push(snapshotDiff.items[path]);
        items.sort(function(a, b) { return snapshotItemCost(b) - snapshotItemCost(a); });
        for(var i = 0; i < items.length; ++i) {
            var item = items[i];
            var type = "new";
            if(item.oldValue) 
                type = "changed";
            else if(item.snapshot == snapshot1) 
                type = "old";
            snapshot_diff.appendChild(snapshotItem(item, type));
            snapshotDiff.counts[type + "_cost"] += snapshotItemCost(item);
            ++snapshotDiff.counts[type];
        }

        var html = "<span title='" + snapshot1.counts.cells + "(" + prettyBytes(snapshot1.counts.cells_cost) + ") " + snapshot1.counts.referenced + "(" + prettyBytes(snapshot1.counts.referenced_cost) + ") " + 
                " VS " + snapshot2.counts.cells + "(" + prettyBytes(snapshot2.counts.cells_cost) + ") " + snapshot2.counts.referenced + "(" + prettyBytes(snapshot2.counts.referenced_cost) + ") " + 
                "'>Increase:" + (snapshot2.counts.cells-snapshot1.counts.cells) + "(" + prettyBytes(snapshot2.counts.cells_cost-snapshot1.counts.cells_cost) + ") " +
                (snapshot2.counts.referenced-snapshot1.counts.referenced) + "(" + prettyBytes(snapshot2.counts.referenced_cost-snapshot1.counts.referenced_cost) + ")</span> === ";
        for(var i in { new:1, old:1, changed:1 } ) 
            html += "&nbsp;<a class='nounderline' href='javascript:toggleSnapshotVisible(\"" + i +"\")'>" +
            "<div style='display: inline' class='snapshot_" + i + "_item'>" + i + ":" + snapshotDiff.counts[i] + "(" + prettyBytes(snapshotDiff.counts[i + "_cost"]) + ")</div>" +
            "</a>";
        snapshot_summary.innerHTML = html;
    }
    updateSnapshotVisible("changed");
    updateSnapshotVisible("old");
    updateSnapshotVisible("new");
    updateSnapshotCost();
}
function takeSnapshot() {
    document.getElementById("snapshot_pane").style.height = "auto";
    if(memory_chart) 
        memory_chart.xAxis[0].addPlotLine({value: (new Date()).getTime(), width: 1, color: "#000000"});
    $.get(nbpdUrl() + "/gibbon/mem/js", function(contents) {
        try {
            var parsed = JSON.parse(contents);
            var snapshot = { contents: contents, parsed: parsed, size: parsed.totalSize };
            var desc = "[" + (snapshots.length+1) + "] SNAPSHOT " + (new Date()).toString() + " [" + prettyBytes(snapshot.size) +"]";
            {
                var option1 = document.createElement("option");
                option1.text = desc;
                var snapshot1 = document.getElementById('snapshot_1');
                snapshot1.add(option1);
                snapshot1.selectedIndex = snapshot1.options.length - (snapshot1.options.length > 1 ? 2 : 1);
            }
            {
                var option2 = document.createElement("option");
                option2.text = desc;
                var snapshot2 = document.getElementById('snapshot_2');
                snapshot2.add(option2);
                snapshot2.selectedIndex = snapshot2.options.length-1;
            }
            snapshots.push(snapshot);
        } catch(e) { 
            console.error("Unable to process snapshot! " + e.toString());
        }
        updateSnapshotDiff();
    });
}

$(document).ready(function() {
    Highcharts.setOptions({
        global: {
            useUTC: false
        }
    });

    var threads = {};
    var objectsChart;

    function prettyJSON(obj) {
        var json = JSON.stringify(obj, undefined, 2);
        json = json.replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;');
        return json.replace(/("(\\u[a-zA-Z0-9]{4}|\\[^u]|[^\\"])*"(\s*:)?|\b(true|false|null)\b|-?\d+(?:\.\d*)?(?:[eE][+\-]?\d+)?)/g, function (match) {
	    var cls = 'prettyJSON_number';
	    if (/^\\"/.test(match)) {
		if (/:$/.test(match)) {
		    cls = 'prettyJSON_key';
                } else {
		    cls = 'prettyJSON_string';
                }
	    } else if (/true|false/.test(match)) {
		cls = 'prettyJSON_boolean';
	    } else if (/null/.test(match)) {
		cls = 'prettyJSON_null';
	    }
	    return '<span class="' + cls + '">' + match + '</span>';
	});
    }

    function createChart(div, parent, name, desc, yAxis) {
        if(!yAxis) {
            var yAxis = {
    	        title: {
    		    text: 'Data'
    	        },
    	        minorGridLineDashStyle: 'ShortDot',
    	        minorGridLineColor: 'rgba(191,191,191,0.9)',
    	        minorTickInterval: 'auto',
    	        GridLineColor: 'rgba(191,0,0,0.9)'
    	    };
        }
	$("#" + parent).append("<div name=\"" + name + "\" id=\"" + div + "\" style=\"min-width: 310px; height: 150px; margin: 0 auto\"></div>");
	return new Highcharts.Chart({
    	    chart: {
		renderTo: div,
		backgroundColor:'rgba(192, 192, 192, 1)',
    		zoomType: 'x',
    		spacingRight: 20,
    		type: 'spline'
    	    },

    	    title: {
    		text: desc
    	    },

    	    scrollbar: {
    		enabled: false
    	    },

    	    colors: [ '#2f7ed8' ],

    	    xAxis: {
    		type: 'datetime',
    		tickPixelInterval: 100,

    		title: {
    		    text: null
    		},

    		dateTimeLabelFormats: {
    		    millisecond: '%H:%M:%S.%L'
    		}

    	    },

    	    yAxis: yAxis,

    	    tooltip: {
    		shared: false,
    		formatter: function() {
    		    return '<b>' + Highcharts.dateFormat('%e %b, %H:%M:%S', this.point.x) + '</b><br/><span style="color:' + this.series.color + '">' + this.series.name +'</span>: ' + this.point.y + "%";
    		},
    		crosshairs: [true, true]
    	    },

    	    legend: {
    		enabled: false
    	    },

    	    plotOptions: {
    		series: {
    		    animation: false
    		},
    		line: {
    		    marker: {
    			enabled: false
    		    }
    		}
    	    },
    	    series: [{ name: 'usage',
    		       type: 'line', zIndex: 999,
    		       data: []
    		     }]
    	});
    }

    function createThreadChart(id, name) { 
        var yAxis = {
    	    title: {
    		text: 'CPU'
    	    },
	    labels: {
		formatter: function() {
		    return this.value + "%";
		}
	    },
    	    min: 0,
	    max: 100,
    	    minorGridLineDashStyle: 'ShortDot',
    	    minorGridLineColor: 'rgba(191,191,191,0.9)',
    	    minorTickInterval: 'auto',
    	    GridLineColor: 'rgba(191,0,0,0.9)'
    	};
        return createChart("thread_" + id, "threads", name, "Thread: " + name + " (" + id + ")", yAxis);
    }

    function makeNumber(v) { return (parseInt(v) || 0); }

    function updateThread(id, name, val) {
   	var thread = threads[id];
   	if(!thread && val) {
	    var chart = createThreadChart(id, name);
	    threads[id] = { chart: chart, name: name };
	}
	if(thread) 
	    thread.chart.series[0].addPoint([(new Date()).getTime(), val], true, false);
    }

    function updateStats() {
	if(memory_chart) {
	    var req;
            var showCPU = document.getElementById("pollCPU").checked;
            var showMem = document.getElementById("pollMemory").checked;
            var showObjects = document.getElementById("pollObjects").checked;
	    if(showCPU || showObjects)
		req = "/stats/";
	    else if(showMem)
		req = "/stats/memory/all";
	    if(req) {
		$.get(nbpdUrl() + req, function(stat) {
    		    stat = JSON.parse(stat);
    		    var x = (new Date()).getTime();
	            {
                        if(stat.events && !event_backlog_chart) {
                            var yAxis = {
    	                        title: {
    		                    text: 'Time'
    	                        },
	                        labels: {
		                    formatter: function() {
		                        return this.value + "MS";
		                    }
	                        },
    	                        min: 0,
    	                        minorGridLineDashStyle: 'ShortDot',
    	                        minorGridLineColor: 'rgba(191,191,191,0.9)',
    	                        minorTickInterval: 'auto',
    	                        GridLineColor: 'rgba(191,0,0,0.9)'
    	                    };
                            event_backlog_chart = createChart("event_backlog", "threads", "event_backlog", "Event Backlog", yAxis);
                        }
                        if(event_backlog_chart)
	                    event_backlog_chart.series[0].addPoint([(new Date()).getTime(), makeNumber(stat.events)], true, false);
                    }
                    if(showObjects) {
                        if(!objectsChart) {
                            objectsChart = createChart("objects", "objects", "objects", "Objects");
                            var object_types = document.getElementById('object_type');
                            for(var obj in stat.objects.values) {
                                var option1 = document.createElement("option");
                                option1.text = obj;
                                object_types.add(option1);
                            }
                        }
    			for(var obj in stat.objects.values) {
                        }
                    }
                    if(showCPU) {
    			for(var id in stat.threads) {
    			    var thread = stat.threads[id];
    			    updateThread(id, thread.name, thread.user_time+thread.system_time);
    			}
                    }
                    if(showMem) {
    			var series = memory_chart.series;
    			for(var d in series) {
    			    var data = series[d];
    			    if(data.name == "privatedirty")
    			        data.addPoint([x, makeNumber(stat.memory.smaps.privatedirty)*1024], false, false);
    			    else if(data.name == "databuffers")
    			        data.addPoint([x, makeNumber(stat.databuffers)], false, false);
    			    else if(data.name == "privateclean")
    			        data.addPoint([x, makeNumber(stat.memory.smaps.privateclean)*1024], false, false);
    			    else if(data.name == "shared")
    			        data.addPoint([x, makeNumber(stat.memory.smaps.shared)*1024], false, false);
    			    else if(data.name == "video")
    			        data.addPoint([x, makeNumber(stat.memory.video)], false, false);
                            else if(data.name == "jsc")
    			        data.addPoint([x, (makeNumber(stat.memory.jsc.fastmalloc) + makeNumber(stat.memory.jsc.osAllocatorCommitted))], false, false);
                            else if(data.name == "fastmalloc")
    			        data.addPoint([x, makeNumber(stat.memory.jsc.fastmalloc)], false, false);
                            else if(data.name == "fastmalloc_used")
    			        data.addPoint([x, (makeNumber(stat.memory.jsc.fastmalloc) - makeNumber(stat.memory.jsc.fastmalloc_free))], false, false);
    			    else
                                console.error("Unhandled datapoint: " + data.name);
    			}
    			memory_chart.redraw();
                    }
    		    setTimeout(updateStats, 1000);
    		});
		return;
	    }
	}
	setTimeout(updateStats, 1000);
    }

    function setDebugFlags(name, value) {
	var data = {
	    object: "nrdp.gibbon.debugger",
	    method: "enableRender",
	    types: [ { name: name, value: value }]
	};
	jQuery.ajax({
	    url: nbpdUrl() + "/ipc",
	    type: "POST",
	    contentType:"text/plain",
	    dataType: "json",
	    data: JSON.stringify(data)
	});
    }

    var nbpdEventSource;
    function nbpdInit() {
	if(nbpdEventSource)
	    return;
	updateStats();
	updateEvents();
    }

    snapshotCostType = document.getElementById('snapshot_cost_type').value;
    snapshotDiffType = document.getElementById('snapshot_diff_type').value;
    document.getElementById('snapshot_1').onchange = function() { updateSnapshotDiff(); };
    document.getElementById('snapshot_2').onchange = function() { updateSnapshotDiff(); };
    document.getElementById('snapshot_diff_type').onchange = function() { 
        snapshotDiffType = document.getElementById('snapshot_diff_type').value;
        setDebugFlags("testScriptDumpHeapNoGC", snapshotDiffType == "unreachable");
        updateSnapshotDiff(); 
    };
    document.getElementById('snapshot_cost_type').onchange = function() { updateSnapshotCost(); };

    document.getElementById("object_type").onchange = updateEvents;

    function updateEvents() {
	if(!document.getElementById("pollEvents").checked) {
            if(nbpdEventSource) {
		nbpdEventSource.close();
		clearInterval(nbpdEventSource.timer);
		$.post(nbpdUrl() + "/ipc", "object=nrdp.gibbon.debugger&method=disableDebug");
		$.post(nbpdUrl() + "/ipc", "object=nrdp.gibbon.debugger&method=disableProfile");
		nbpdEventSource = undefined;
            }
	} else {
            document.getElementById("instrumentation_container").style.height = "auto";
	    instrumentation_chart.series[0].setData([]);
	    document.getElementById("instRender").onchange = function() {
		setDebugFlags("instrumentWidgetRender", this.checked);
	    };
	    document.getElementById("instAnimation").onchange = function() {
		setDebugFlags("instrumentWidgetAnimation", this.checked);
	    };
	    nbpdEventSource = new EventSource(nbpdUrl() + "/eventsource?objectFilter=nrdp.gibbon.debugger");
	    nbpdEventSource.events_base = (new Date()).getTime();
	    nbpdEventSource.events = [];
	    nbpdEventSource.onmessage = function(event) {
		var data = JSON.parse(event.data);
		if(data.object == "nrdp.gibbon.debugger" && data.name == "InstrumentationEvent") {
		    if(instrumentation_chart) {
			var category;
			for(var e in nbpdEventSource.events) {
		    	    if(nbpdEventSource.events[e] == data.data.instrumentation[0]) {
		    		category = parseInt(e);
		    		break;
		    	    }
			}
			if(category === undefined) {
		    	    category = nbpdEventSource.events.length;
		    	    nbpdEventSource.events.push(data.data.instrumentation[0]);
		    	    instrumentation_chart.xAxis[0].setCategories(nbpdEventSource.events);
			}
			if(!nbpdEventSource.events_first)
			    nbpdEventSource.events_first = data.data.instrumentation[1];
			var x = nbpdEventSource.events_base + (data.data.instrumentation[1] - nbpdEventSource.events_first);
			instrumentation_chart.series[0].addPoint({ x: category, low: x - data.data.instrumentation[5], high: x, customTooltip: data.data.instrumentation }, false, false);
		    }
		}
	    };
	    nbpdEventSource.timer = setInterval(function() { if(instrumentation_chart) instrumentation_chart.redraw(); }, 1000);
	    $.post(nbpdUrl() + "/ipc", "object=nrdp.gibbon.debugger&method=enableDebug");
	    $.post(nbpdUrl() + "/ipc", "object=nrdp.gibbon.debugger&method=enableProfile");
	}
    }
    document.getElementById("pollEvents").onchange = updateEvents;

    $('#instrumentation').highcharts({
    	chart: {
	    backgroundColor:'rgba(192, 192, 192, 1)',
	    zoomType: 'y',
    	    type: 'columnrange',
    	    inverted: true,
	    events: {
                load: function() { instrumentation_chart = this; nbpdInit(); }
	    }
    	},
	scrollbar: {
	    enabled: true
	},
    	title: {
    	    text: 'Events'
    	},
    	xAxis: {
    	    categories: []
    	},
    	yAxis: {
	    type: 'datetime',
	    tickPixelInterval: 500,
	    title: { text: null }
    	},

    	tooltip: {
	    shared: false,
	    formatter: function() {
		var result = '<b>' + Highcharts.dateFormat('%e %b, %H:%M:%S.%L', this.point.y) + '</b><br/>' +
			'<span style="color:' + this.series.color + '">' + this.point.category +'</span>: ' + (this.point.high-this.point.low)  + "ms" + '<br>';
        	var thread = this.point.customTooltip[3];
                if(threads[thread])
		    thread = threads[thread].name;
		result += '<span style="color:' + this.series.color + '">Thread</span>: ' + thread + '<br>';
		//if(this.point.customTooltip[7])
		//	result += '<span style="color:' + this.series.color + '">Marks</span>: ' + prettyJSON(this.point.customTooltip[7]) + "<br>";
		// if(this.point.customTooltip[4])
		// 	result += prettyJSON(this.point.customTooltip[4]);
                return result;
	    },
	    crosshairs: [true, true]
	    //pointFormat: '<span style="color:{series.color}">{series.name}</span>: <b>{point.y}Mb</b><br/>'
        },

    	plotOptions: {
    	    columnrange: {
    		events: {
		    click: function(event) {
                        $('#instrumentation_drilldown_pane').empty();
  			if(event.point.category.startsWith("gibbon.graphics.render")) {
    			    $('#instrumentation_drilldown_pane').html("<div id=\"instrumentation_drilldown_tt\"></div><svg id=\"instrumentation_drilldown\"></svg><a href=\"javascript:$('#instrumentation_drilldown_pane').empty()\">Close</a> (<input type=\"checkbox\" id=\"drillDownPixels\">Pixels)");
			    function showData(pixels) {
				var width = $('body').innerWidth(), height = 310;
				var x = d3.scale.linear().range([0, width]);
				var y = d3.scale.linear().range([0, height]);
				var color = d3.scale.category20c();
				var partition = d3.layout.partition()
					.children(function(d) { return isNaN(d.value) ? d3.entries(d.value) : null; })
					.value(function(d) { return d.value; });

				var svg = d3.select("#instrumentation_drilldown")
					.attr("width", width)
					.attr("height", height);

				svg.text("");
				svg.append("rect")
				    .attr("width", "100%")
				    .attr("height", "100%")
				    .attr("fill", "rgba(192, 192, 192, 1)");

				var root = {};
				var stack = [root];
				var operations = event.point.customTooltip[4].operations;
                                for(var i = 0; i < operations.length; ++i) {
				    var op = operations[i];
				    if(op.op == "comment") {
                                        //drop
				    } else if(op.op == "push") {
					var obj = {};
                                        stack[stack.length-1][i] = obj;
					stack.push(obj);
				    } else if(op.op == "pop") {
					stack.pop();
				    } else if(pixels) {
					stack[stack.length-1][i] = op.args.pixels;
				    } else {
					stack[stack.length-1][i] = op.duration;
				    }
				}

				var rect = svg.selectAll("rect");
				rect = rect
				    .data(partition(d3.entries(root)[0]))
				    .enter().append("rect")
				    .attr("x", function(d) { return x(d.x); })
				    .attr("y", function(d) { return y(d.y); })
				    .attr("width", function(d) { return x(d.dx); })
				    .attr("height", function(d) { return y(d.dy); })
				    .attr("stroke", function(d) { return "white" })
				    .attr("fill", function(d) {
					if(operations[d.key].op == "push")
					    return "black";
					else if(operations[d.key].op == "blit")
					    return "red";
					else if(operations[d.key].op == "fill")
					    return "green";
					return color((d.children ? d : d.parent).key); })
				    .on("mouseover", itemMouseover)
				    .on("click", itemClicked);

				function itemMouseover(d) {
				    if(d && d.key) {
					var msg = "", overlay;
					if(operations[d.key].op == "push") {
					    overlay = operations[d.key].args.rect;
					    msg += "[" + d.value + "ms] ";
                                            if(operations[parseInt(d.key)+1].op == "comment")
						msg += JSON.stringify(operations[parseInt(d.key)+1].args);
					} else {
					    if(operations[d.key].op == "blit")
						overlay = operations[d.key].args.dstRect;
					    else
						overlay = operations[d.key].args.rect;
					    msg += "[" + operations[d.key].duration + "ms] ";
					    msg += operations[d.key].op + "::" + JSON.stringify(operations[d.key].args);
					}
					if(overlay) {
					    overlay = /\[([0-9]+),([0-9]+)\+([0-9]+)x([0-9]+)\]/.exec(overlay);
					    $.post(nbpdUrl() + "/ipc", "object=nrdp.gibbon.debugger&method=overlay&replace=true&x=" +
						   overlay[1] + "&y=" + overlay[2] + "&width=" + overlay[3] + "&height=" + overlay[4]);
					}
					$('#instrumentation_drilldown_tt').text(msg);
				    }
				}
				function itemClicked(d) {
				    x.domain([d.x, d.x + d.dx]);
				    y.domain([d.y, 1]).range([d.y ? 20 : 0, height]);

				    rect.transition()
					.duration(750)
					.attr("x", function(d) { return x(d.x); })
					.attr("y", function(d) { return y(d.y); })
					.attr("width", function(d) { return x(d.x + d.dx) - x(d.x); })
					.attr("height", function(d) { return y(d.y + d.dy) - y(d.y); });
				}
			    }
			    showData(false);
			    document.getElementById("drillDownPixels").onchange = function() { showData(this.checked); };
			} else if(event.point.category.startsWith("gibbon.widget.animation")) {
    			    $('#instrumentation_drilldown_pane').html("<div id=\"instrumentation_drilldown\" style=\"min-width: 310px; height: 300px; margin: 0 auto\"></div><a href=\"javascript:$('#instrumentation_drilldown_pane').empty()\">Close</a>");
			    var value_data = [];
			    for(var i = 0; i < event.point.series.points.length; ++i) {
				var point = event.point.series.points[i];
				if(point.category == event.point.category) {
				    for(var m = 0; m < point.customTooltip[7].value.length; ++m)
					value_data.push([point.low+point.customTooltip[7].value[m][0], point.customTooltip[7].value[m][2]]);
				}
			    }
			    $('#instrumentation_drilldown').highcharts({
                                chart: {
				    backgroundColor:'rgba(192, 192, 192, 1)',
				    zoomType: 'x',
				    type: 'line',
				    events: {
                                        load: function() {
					    this.series[0].setData(value_data);
                                        }
				    }
    	                        },
    	                        title: {
    				    text: ''
    	                        },
    	                        yAxis: {},
                                xAxis: {
				    type: 'datetime',
				    tickPixelInterval: 100,
				    title: { text: null },
				    dateTimeLabelFormats: { millisecond: '%H:%M:%S.%L' }
                                },

                                tooltip: {
				    shared: false,
				    formatter: function() {
     		                        return '<b>' + Highcharts.dateFormat('%e %b, %H:%M:%S', this.point.x) + '</b><br/><span style="color:' + this.series.color + '">' + this.series.name +'</span>: ' + this.point.y;
				    },
				    crosshairs: [true, true]
                                },
    	                        plotOptions: {
    				    line: {
    		                        marker: {
    					    enabled: false
    		                        }
    				    }
    				},
    				legend: { enabled: false },
    				series: [	{ name: 'Events', data: [] } ]
			    });
			}
    		    }
		}
	    }
    	},

    	legend: {
    	    enabled: false
    	},

    	series: [
	    { name: 'Events', data: [] }
    	]
    });

    $('#memory').highcharts({
        chart: {
	    backgroundColor:'rgba(192, 192, 192, 1)',
	    zoomType: 'x',
	    spacingRight: 20,
	    type: 'area',

	    events: {
                load: function() { memory_chart = this; nbpdInit(); }
	    }
        },

        title: {
	    text: "Memory Usage"
        },

        subtitle: {
	    text: ""
        },

        scrollbar: {
	    enabled: true
        },

        colors: [
	    '#2f7ed8', 
	    '#0d233a', 
	    '#8bbc21', 
	    '#910000', 
	    '#aaaa55',
	    '#55aaaa',
	    '#aa55aa', 
	    '#aa5555', 
	    '#c42525', 
	    '#a6c96a'
        ],

        xAxis: {
	    type: 'datetime',
	    tickPixelInterval: 100,

	    title: {
                text: null
	    },

	    dateTimeLabelFormats: {
                millisecond: '%H:%M:%S.%L'
	    }

        },

        yAxis: {
	    title: {
                text: 'Memory'
	    },
    	    labels: {
    		formatter: function() {
    		    return this.value / 1000000 + "Mb";
    		}
    	    },
	    min: 0,
	    minorGridLineDashStyle: 'ShortDot',
	    minorGridLineColor: 'rgba(191,191,191,0.9)',
	    minorTickInterval: 'auto',
	    GridLineColor: 'rgba(191,0,0,0.9)'
        },

        tooltip: {
	    shared: false,
	    formatter: function() {
                return '<b>' + Highcharts.dateFormat('%e %b, %H:%M:%S', this.point.x) + '</b><br/><span style="color:' + this.series.color + '">' + this.series.name +'</span>: ' + (this.point.y / 1000000).toFixed(2)  + "Mb";
	    },
	    crosshairs: [true, true]
	    //pointFormat: '<span style="color:{series.color}">{series.name}</span>: <b>{point.y}Mb</b><br/>'
        },

        legend: {
            margin: 2,
	    enabled: true
        },

        plotOptions: {
	    series: {
                animation: false
	    },
	    area: {
                stacking: 'normal',
                lineWidth: 1,
                marker: {
		    enabled: false
                },
                shadow: false,
                states: {
		    hover: {
                        lineWidth: 1
		    }
                },
                threshold: null
	    },
	    line: {
                marker: {
		    enabled: false
                }
	    }
        },
	series: [{ name: 'databuffers',
		   type: 'line', zIndex: 999,
		   trend: true,
		   data: []
		 },
                 { name: 'jsc',
		   type: 'line', zIndex: 999,
		   trend: true,
		   data: []
		 },
                 { name: 'fastmalloc',
		   type: 'line', zIndex: 999,
		   trend: true,
		   data: []
		 },
                 { name: 'fastmalloc_used',
		   type: 'line', zIndex: 999,
		   trend: true,
		   data: []
		 },
                 { name: 'privatedirty',
		   type: 'area',
		   trend: true,
		   data: []
		 },
                 { name: 'privateclean',
		   type: 'area',
		   trend: true,
		   data: []
		 },
                 { name: 'shared',
		   type: 'area',
		   trend: true,
		   data: []
		 },
                 { name: 'video',
		   type: 'area',
		   trend: true,
		   data: []
		 }
	]
    });
});
