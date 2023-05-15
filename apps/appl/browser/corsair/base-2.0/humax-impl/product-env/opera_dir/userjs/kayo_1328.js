// ==UserScript==
// @include http://app.10ft.itv.com/edna/*/freeview/*
// ==UserScript==

addEventListener('load', function(e) {
    var elem = document.createElement('style');
    var text = document.createTextNode(".loading .content .throbber {height: 1.57rem;}");
    elem.appendChild(text);
    document.head.appendChild(elem);
}, false);
