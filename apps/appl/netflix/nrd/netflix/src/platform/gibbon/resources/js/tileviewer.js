/*global nrdp*/
var tile_pad = 10;

var tile;
var groups;
var tile_groups_widget, tile_widgets, tile_label;

function updateSelection()
{
    if(groups && groups[tile.group].tiles && groups[tile.group].tiles.length)
        tile_label.text = groups[tile.group].tiles[tile.offset].description;
    for(var i = 0; i < tile_widgets.length; ++i) 
        tile_widgets[i].color = (i == tile.group) ? "#aaaaaa" : undefined;
}

function updateTiles()
{
    if(tile_widgets) {
        for(var i = 0; i < tile_widgets.length; ++i)
            tile_widgets[i].parent = undefined;
        tile_widgets = undefined;
    }
    groups = undefined;
    tile_label.text = "";
    tile = { group: 0, offset: 0 };
    tile_groups_widget.y = 0;
    if(nrdp.gibbon.tilemanager.tiles) {
        groups = nrdp.gibbon.tilemanager.tiles.groups;
        if(groups) {
            tile_widgets = [];
            for(var j = 0; j < groups.length; ++j) {
                if(groups[j].tiles && groups[j].tiles.length) {
                    var tile_widget = nrdp.gibbon.makeWidget({ image: { src: groups[j].tiles[0].url, halign: "center" }, layoutSpacing: tile_pad, sendRenderProperties: true, padding: 5 });
                    tile_widget.parent = tile_groups_widget;
                    tile_widgets[j] = tile_widget;
                }
            }
            if(tile_widgets.length)
                updateSelection();
        } else if(nrdp.gibbon.tilemanager.status != nrdp.gibbon.tilemanager.STATUS_Ok) {
            nrdp.log.error("Got tile error: " + nrdp.gibbon.tilemanager.tiles.status);
        }
    }
}

function keyboardHandler(key)
{
    if(key.data.type == "release") {
        function dumpResult(evt) {
            if (!evt.success) {
                nrdp.log.error("setTiles failed: " + evt.error);
            } else {
                nrdp.log.error("setTiles succeeded");
            }
        }
        if (key.data.text == 'a') {
            nrdp.tile.setTiles({
                groups: [
                    {
                        "title": "group name",
                        "tiles": [
                            {
                                "url":"http://cdn0.nflximg.net/images/7894/4177894.jpg",
                                "width": 100,
                                "height":  100,
                                "title": "Optional title text (50 characters)",
                                "description": "Optional text (150 characters)",
                                "shortText": "Optional text (50 characters)",
                                "deepLink": "Optional deep-link payload",
                                "expiry": parseInt(new Date().valueOf() / 1000) + 10000
                            },
                            {
                                "url":"http://cdn0.nflximg.net/images/0728/9600728.jpg",
                                "width": 100,
                                "height":  100,
                                "title": "Optional other title text (50 characters)",
                                "description": "Optional other text (150 characters)",
                                "shortText": "Optional other text (50 characters)",
                                "deepLink": "Optional other deep-link payload",
                                "expiry": parseInt(new Date().valueOf() / 1000) + 10000
                            }
                        ]
                    }
                ]
            }, dumpResult);
        } else if (key.data.text == 'b') {
            nrdp.tile.setTiles({
                groups: [
                    {
                        "title": "group name 2",
                        "tiles": [
                            {
                                "url":"http://cdn0.nflximg.net/images/0728/9600728.jpg",
                                "width": 1002,
                                "height":  1002,
                                "title": "Optional title text (50 characters) 2",
                                "description": "Optional text (150 characters) 2",
                                "shortText": "Optional text (50 characters) 2",
                                "deepLink": "Optional deep-link payload 2",
                                "expiry": (new Date().valueOf() / 1000) + 10000
                            }
                        ]
                    },
                    {
                        "title": "group name 3",
                        "tiles": [
                            {
                                "url":"http://cdn0.nflximg.net/images/0728/9600728.jpg",
                                "width": 1002,
                                "height":  1002,
                                "expiry": (new Date().valueOf() / 1000) + 10000
                            }
                        ]
                    }

                ]
            }, dumpResult);
        } else if(key.data.text == 'c') {
            nrdp.tile.setTiles({ groups: [ {  "title": "Group1",  "tiles": [ { "url":"http://lgux-mdxapp2.corp.netflix.com/movies/tiles/arresteddev.jpg", "width": 100, "height":  100, "title": "Arrested dev", "description": "Arrested dev - description", "shortText": "AD", "deepLink": "This deep links into Arrested dev", "expiry": parseInt(new Date().valueOf() / 1000) + 10 }, { "url":"http://lgux-mdxapp2.corp.netflix.com/movies/tiles/houseOfCards.jpg", "width": 100, "height":  100, "title": "House of cards", "description": "House of cards - description", "shortText": "HOC", "deepLink": "This deep links into House of cards", "expiry": parseInt(new Date().valueOf() / 1000) + 20 } ] }, {  "title": "Kids Group", "tiles": [ { "url":"http://lgux-mdxapp2.corp.netflix.com/movies/tiles/richierich.JPG", "width": 300, "height":  300, "title": "Richie rich", "description": "Richie rich - description", "shortText": "RR", "deepLink": "This deep links into Richie rich kids show", "expiry": parseInt(new Date().valueOf() / 1000) + 30 }, { "url":"http://lgux-mdxapp2.corp.netflix.com/movies/tiles/clifford.jpg", "width": 300, "height":  200, "title": "Clifford - the big red dog", "description": "Clifford - Watch the big red dog in action", "shortText": "CBRD", "deepLink": "This deep links into Clifford", "expiry": parseInt(new Date().valueOf() / 1000) + 40 } ] } ] }, dumpResult);
        }
    } else if(groups && groups.length && key.data.type == "press") {
        if(key.data.uiEvent == "key.left") {
            tile.offset = (tile.offset - 1) % groups[tile.group].tiles.length;
            if(tile.offset < 0)
                tile.offset = groups[tile.group].tiles.length-1;
            tile_widgets[tile.group].image.src = groups[tile.group].tiles[tile.offset].url;
        } else if(key.data.uiEvent == "key.right") {
            tile.offset = (tile.offset + 1) % groups[tile.group].tiles.length;
            tile_widgets[tile.group].image.src = groups[tile.group].tiles[tile.offset].url;
        } else if(key.data.uiEvent == "key.enter") {
            nrdp.log.error("DeepLink: " + groups[tile.group].tiles[tile.offset].deepLink);
        } else if(key.data.uiEvent == "key.up") {
            tile.group = (tile.group - 1) % tile_widgets.length;
            if(tile.group < 0)
                tile.group = tile_widgets.length-1;
            tile_groups_widget.y = -(tile_widgets[tile.group].renderY-tile_pad);
        } else if(key.data.uiEvent == "key.down") {
            tile.group = (tile.group + 1) % tile_widgets.length;
            tile_groups_widget.y = -(tile_widgets[tile.group].renderY-tile_pad);
        }
        updateSelection();
    }
}

function main() {
    var w;
    nrdp.gibbon.addEventListener("key", keyboardHandler);
    nrdp.gibbon.scene.widget = w = nrdp.gibbon.makeWidget({width:1280, height:720, color:"#220000", layout: "vertical"});

    tile_label = nrdp.gibbon.makeWidget({height: 40, name: "label", text: { color: "#aaaaaa" } });
    tile_label.parent = w;

    var container = nrdp.gibbon.makeWidget({name: "container", layout: "horizontal", layoutStretch: 90 });
    container.parent = w;

    var logo = nrdp.gibbon.makeWidget({name: "logo", layoutStretch: 90, image: { src: "http://localcontrol.netflix.com/img/Netflix_Logo_Splash.png", halign: "center", valign: "center" } });
    logo.parent = container;

    var inner = nrdp.gibbon.makeWidget({name: "inner"});
    inner.parent = container;
    tile_groups_widget = nrdp.gibbon.makeWidget({name: "groups", padding: tile_pad, layout: "vertical"});
    tile_groups_widget.animate("y", 500, "ease-out-bounce");
    tile_groups_widget.parent = inner;

    if(nrdp.gibbon.tilemanager) {
        nrdp.gibbon.tilemanager.init();
        nrdp.gibbon.tilemanager.addEventListener("tileschanged", function() { nrdp.log.error("tilesChanged()"); updateTiles(); });
    }
    updateTiles();
}
nrdp.gibbon.init(main);
