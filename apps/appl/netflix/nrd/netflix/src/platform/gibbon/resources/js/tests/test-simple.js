
function startSpecial() {
    nrdp.log.debug("Gibbon lives! "  + nrdp.gibbon.scene.width + " " + nrdp.gibbon.scene.height);
    w = nrdp.gibbon.makeWidget();
    {
        nrdp.gibbon.scene.widget = w;
        w.x = 10;
        w.y = 0;
        w.width = 1000;
        w.height = 1000;
        w.opacity = 1.0;
        w.color = { a: 255, r: 255, g: 0, b: 0 };
        //w.image.url = "http://www.google.com/intl/en_com/images/srpr/logo3w.png";
    }

    left = nrdp.gibbon.makeWidget();
    {
        left.parent = w;
        left.x = 0;
        left.y = 0;
        left.width = 25;
        left.height = 110;
        left.opacity = 1.0;
        left.color = { a: 255, r: 0, g: 0, b: 255 };
        left.image.url = "http://cdn-0.nflximg.com/us/ffe/htmltvui/pluskids/720/bg-bob-gallery.png";
        left.image.sourceRect = { x: 0, y: 222, width: 25, height: 110 };
    }

    mid = nrdp.gibbon.makeWidget();
    {
        mid.parent = w;
        mid.x = left.x+left.width;
        mid.y = 0;
        mid.width = 300;
        mid.height = 110;
        mid.opacity = 1.0;
        mid.color = { a: 255, r: 0, g: 0, b: 255 };
        mid.image.halign = "scale";
        mid.image.url = "http://cdn-0.nflximg.com/us/ffe/htmltvui/pluskids/720/bg-bob-gallery.png";
        mid.image.sourceRect = { x: 0, y: 0, width: 25, height: 110 };
    }

    right = nrdp.gibbon.makeWidget();
    {
        right.parent = w;
        right.x = mid.x+mid.width;
        right.y = 0;
        right.width = 25;
        right.height = 110;
        right.opacity = 1.0;
        right.color = { a: 255, r: 0, g: 0, b: 255 };
        right.image.url = "http://cdn-0.nflximg.com/us/ffe/htmltvui/pluskids/720/bg-bob-gallery.png";
        right.image.sourceRect = { x: 0, y: 111, width: 25, height: 110 };
    }

}
nrdp.addEventListener("init", startSpecial);
nrdp.init();


