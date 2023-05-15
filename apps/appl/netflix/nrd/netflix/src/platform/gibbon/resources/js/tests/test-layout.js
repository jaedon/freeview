
var w2, w3, w4;

function startSpecial() {
    nrdp.log.debug("Gibbon lives! "  + nrdp.gibbon.scene.width + " " + nrdp.gibbon.scene.height);
    var w_outer = nrdp.gibbon.makeWidget();
    w_outer.width = 950;
    w_outer.height = 400;
    w_outer.name = "outer";
    w_outer.color = { a: 255, r:255, g:255, b:0 };
    w_outer.layout = { layout: "vertical", margin: 20, spacing: 40 };

    var w = nrdp.gibbon.makeWidget();
    w.parent = w_outer;
    w.layoutStretch = 1;
    w.name = "inner";
    //w.width = 450;
    //w.height = 250;
    //w.maxWidth = 250;
    w.maxHeight = 350;
    w.color = { a: 255, r:255, g:255, b:255 };
    w.layout = { layout: "horizontal", align: [ "left" ] };

    w2 = nrdp.gibbon.makeWidget();
    w2.layoutStretch = 1;
    w2.name = "w2";
    w2.parent = w;
    w2.maxWidth = 250;
    w2.minWidth = 100;
    w2.minHeight = 50;
    //w2.text = "Hello world!";
    w2.color = { a: 255, r:255, g:0, b:0 };

    w3 = nrdp.gibbon.makeWidget();
    w3.layoutStretch = 1;
    w3.name = "w3";
    w3.width = 50;
    w3.minHeight = 50;
    w3.animate("width", 500);
    w3.parent = w;
    w3.color = { a: 255, r:0, g:255, b:0 };

    w4 = nrdp.gibbon.makeWidget();
    w4.layoutStretch = 1;
    w4.name = "w4";
    w4.layoutStretch = 90;
    w4.minHeight = 50;
    w4.maxWidth = 200;
    //w4.text = "Goodbye world!";
    w4.parent = w;
    w4.color = { a: 255, r:128, g:0, b:0 };

//     var w4 = nrdp.gibbon.makeWidget();
//     w4.parent = w;
//     w4.color = { a: 255, r:0, g:0, b:255 };

//     var w5 = gibbon.makeWidget();
//     w5.parent = w;
//     w5.x = 0;
//     w5.y = 400;
//     w5.width = 100;
//     w5.height = 100;
//     w5.color = { a: 255, r:255, g:0, b:255 };

    nrdp.gibbon.scene.widget = w_outer;
}
if (nrdp.isReady) {
    startSpecial();
} else {
    nrdp.addEventListener("init", startSpecial);
    nrdp.init();
}


