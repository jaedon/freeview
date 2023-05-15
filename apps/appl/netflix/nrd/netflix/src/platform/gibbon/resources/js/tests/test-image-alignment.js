
var widgets = [];
var scene;
function startSpecial() {
    scene = nrdp.gibbon.makeWidget();
    scene.color = { r: 255, g: 255, b: 0 };
    scene.width = 1280;
    scene.height = 720;
    nrdp.gibbon.scene.widget = scene;

    var x = 0;
    var halignments = [ "normal", "center", "stretch", "tile", "tile right", "right", "stretch aspect" ];
    var valignments = [ "normal", "center", "stretch", "tile", "tile bottom", "bottom", "stretch aspect" ];
    // halignments = [ "stretch" ];
    // valignments = [ "normal" ];

    var width = parseInt(1280 / halignments.length);
    var height = parseInt(720 / valignments.length);
    var fontSize = 15;
    height -= fontSize;
    for (var h=0; h<halignments.length; ++h) {
        var y = 0;
        for (var v=0; v<valignments.length; ++v) {
            var w = nrdp.gibbon.makeWidget();
            widgets[widgets.length] = w;
            w.color = { r: 180, g: 180, b: 180 };
            w.x = x;
            w.y = y;
            w.width = width;
            w.height = height;
            w.image.url = { url: "http://localcontrol.netflix.com/img/Netflix_Logo_Splash.png" };
            w.image.halign = halignments[h];
            w.image.valign = valignments[v];
            w.drawOrder = -1;
            var text = nrdp.gibbon.makeWidget();
            text.text = "h: " + halignments[h] + " v: " + valignments[v];
            text.width = w.width;
            text.textStyle = { align: "bottom", color: { r: 0, g: 0, b: 255 }, size: fontSize };
            text.x = x + 3;
            text.y = w.y + w.height;
            w.parent = scene;
            text.parent = scene;
            y += height + fontSize;
            if (!h) {
                var line = nrdp.gibbon.makeWidget();
                line.x = 0;
                line.y = y;
                line.height = 1;
                line.width = 1280;
                line.color = { r: 0, g: 0, b: 0, a: 255 };
                line.parent = scene;
            }
        }
        x += width;
        var line = nrdp.gibbon.makeWidget();
        line.x = x;
        line.y = 0;
        line.height = 720;
        line.width = 1;
        line.color = { r: 0, g: 0, b: 0, a: 255 };
        line.parent = scene;
    }
}
function setImage(url)
{
    for (var i=0; i<widgets.length; ++i) {
        widgets[i].image.url = url;
    }
}
nrdp.addEventListener("init", startSpecial);
nrdp.init();

