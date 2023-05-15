
var widgets = [];
var scene;
var w;
function startSpecial() {
    scene = nrdp.gibbon.makeWidget();
    scene.color = { r: 255, g: 255, b: 0 };
    scene.width = 1280;
    scene.height = 720;
    // scene.height -= 100;
    // scene.width -= 100;
    nrdp.gibbon.scene.widget = scene;

    var alignments = [ "normal", "center", "stretch", "tile", "tile bottom", "bottom", "stretch aspect", "stretch aspect tile" ];
    var h=7;
    var v=7;

    var width = scene.width;
    var height = scene.height;

    var fontSize = 20;
    height -= fontSize;
    w = nrdp.gibbon.makeWidget();
    w.color = { r: 180, g: 180, b: 180 };
    w.width = width;
    w.height = height;
    w.x = 0;
    w.y = 0;
    // w.image.url = "http://www.amcuruguay.com/Randy%20Travis.jpg";
    w.image.url = "http://localcontrol.netflix.com/img/turkey.jpg";
    w.image.halign = alignments[h];
    w.image.valign = alignments[v];
    widgets[widgets.length] = w;
    w.drawOrder = -1;
    w.parent = scene;
    var text = nrdp.gibbon.makeWidget();
    text.text = "h: " + alignments[h] + " v: " + alignments[v];
    text.textStyle = { align: "bottom", color: { r: 0, g: 0, b: 0, a: 255 }, size: fontSize };
    text.x = 3;
    text.y = w.height;
    text.width = width;
    // text.color = { b: 255 };
    text.parent = scene;
    nrdp.gibbon.addEventListener("key",
                                 function(evt) {
                                     if (evt.data.type == "press") {
                                         switch (evt.data.code) {
                                         case 37: // left
                                             if (--h < 0)
                                                 h = alignments.length - 1;
                                             break;
                                         case 39: // right
                                             if (++h == alignments.length)
                                                 h = 0;
                                             break;
                                         case 38: // up
                                             if (--v < 0)
                                                 v = alignments.length - 1;
                                             break;
                                         case 40: // down
                                             if (++v == alignments.length)
                                                 v = 0;
                                             break;
                                         default:
                                             return;
                                         }
                                         w.image.halign = alignments[h];
                                         w.image.valign = alignments[v];
                                         text.text = "h: " + alignments[h] + " v: " + alignments[v];

                                     }
                                 });
}
function setImage(url)
{
    for (var i=0; i<widgets.length; ++i) {
        widgets[i].image.url = url;
    }
}
nrdp.addEventListener("init", startSpecial);
nrdp.init();

