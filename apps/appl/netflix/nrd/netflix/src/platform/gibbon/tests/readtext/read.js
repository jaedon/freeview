nrdp.gibbon.setTimeout(function() {
    if (nrdp.gibbon.Widget.prototype.__textIntercepted)
        return;
    nrdp.gibbon.Widget.prototype.__textIntercepted = true;
    var setter = nrdp.gibbon.Widget.prototype.__lookupSetter__("text");

    var tts = {
        synopsis: undefined,
        titleSpoken: false,

        speak: function(widget, text) {
            nrdp.gibbon.setTimeout(function() {
                if (widget.name === "mdp-outline-title") {
                    nrdp.texttospeech.say(text);
                    nrdp.texttospeech.silence(1000);

                    if (tts.synopsis) {
                        nrdp.texttospeech.say(tts.synopsis);
                        nrdp.texttospeech.silence(1000);
                        tts.synopsis = undefined;
                    } else {
                        tts.titleSpoken = true;
                    }
                } else if (widget.name === "bob-synopsis") {
                    if (!tts.titleSpoken) {
                        tts.synopsis = text;
                    } else {
                        nrdp.texttospeech.say(text);
                        nrdp.texttospeech.silence(1000);
                        tts.titleSpoken = false;
                    }
                }
            }, 100);
        }
    };

    nrdp.gibbon.Widget.prototype.__defineSetter__("text", function(val) {
        setter.call(this, val);
        if (this.name === "mdp-outline-title" || this.name === "bob-synopsis")
            tts.speak(this, val);
    });
}, 5000);

nrdp.gibbon.addEventListener("key", function(evt) {
    if (evt.data.type === "release")
        nrdp.texttospeech.stopAndFlush();
});
