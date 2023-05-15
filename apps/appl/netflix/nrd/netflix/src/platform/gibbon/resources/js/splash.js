/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

/*global nrdp*/

nrdp.gibbon.splash = {
    options: {},

    addEventListener: function addEventListener(evt, listener) { nrdp._addEventListener(this, evt, listener); },
    removeEventListener: function removeEventListener(evt, listener) { nrdp._removeEventListener(this, evt, listener); },

    init: function init(visible) {
        var firstPage = (nrdp.gibbon._locationCount == 1);
        var splash_queryParams = nrdp.gibbon._parseQueryParams(nrdp.gibbon._splashUrl);
        var animated = splash_queryParams["animated"];
        if(animated === undefined || parseInt(animated)) {
            animated = {
                    in: splash_queryParams["animatedIn"],
                   out: splash_queryParams["animatedOut"],
               spinner: splash_queryParams["animatedSpinner"]
            };
            animated.out = (animated.out === undefined) ? true : (parseInt(animated.out) != 0);
            animated.spinner = (animated.spinner === undefined) ? true : (parseInt(animated.spinner) != 0);
            animated.in = (animated.in === undefined) ? firstPage : (parseInt(animated.in) != 0);
            nrdp.gibbon.splash.options.animated = animated;
        }

        if(nrdp.device.capability.supportUiSound && !nrdp.suspended) {
            var sound = splash_queryParams["sound"];
            if(sound === undefined || parseInt(sound)) {
                sound = {
                     in: splash_queryParams["soundIn"],
                    out: splash_queryParams["soundOut"]
                };
                sound.in = (sound.in === undefined) ? false : (parseInt(sound.in) != 0);
                sound.out = (sound.out === undefined) ? false : (parseInt(sound.out) != 0);
            }
            nrdp.gibbon.splash.options.sound = sound;
        }
        if(nrdp.gibbon._locationCount == 1)
            nrdp.log.warn("SplashOptions: " + JSON.stringify(nrdp.gibbon.splash.options));
        nrdp.gibbon.splash._update(visible);
    },
    _update: function _update(visible) {
        var splashWidget;
        if(!visible) {
            splashWidget = nrdp.gibbon.splash.root;
            if(splashWidget) {
                if(splashWidget.c.soundIn) {
                    nrdp.audio.unload(splashWidget.c.soundIn);
                    splashWidget.c.soundIn = undefined;
                }
                if(splashWidget.c.soundOut) {
                    if(nrdp.suspended) {
                        nrdp.audio.unload(splashWidget.c.soundOut);
                        splashWidget.c.soundOut = undefined;
                    } else {
                        nrdp.audio.play(splashWidget.c.soundOut, 1, function() {
                            nrdp.audio.unload(splashWidget.c.soundOut);
                            splashWidget.c.soundOut = undefined;
                        });
                    }
                }
                if(nrdp.gibbon.splash.options.animated && nrdp.gibbon.splash.options.animated.out && splashWidget.c.background.visible) {
                    splashWidget.opaque = false;
                    splashWidget.color = undefined;
                    splashWidget.c.background.startAnimation("opacity", undefined, 0, 1200, "ease-in-out-quint");
                    splashWidget.c.background.addEventListener("animationFinished", function(a) {
                        if(a.data.property == "opacity" && !a.data.value) {
                            nrdp.gibbon.scene.overlay = undefined;
                            nrdp._callEventListeners(nrdp.gibbon.splash, { type: "splashChanged", visible: visible });
                        }
                    });
                    if(splashWidget.c.spinner)
                        splashWidget.c.spinner.startAnimation("opacity", undefined, 0, 300, "linear");

                    splashWidget.c.logo.startAnimation("opacity", undefined, 0, 600, "ease-in-quint");
                    splashWidget.c.logo.startAnimation("scale", undefined, 1.2, 1000, "ease-in-out-quint");

                    splashWidget.c.shadow.startAnimation("opacity", undefined, 0, 600, "ease-in-quint");
                    splashWidget.c.shadow.startAnimation("scale", undefined, 1.2, 1000, "ease-in-out-quint");
                    splashWidget.c.shadow.startAnimation("y", 489, 503, 1000, "ease-in-out-quint");
                } else {
                    nrdp.gibbon.scene.overlay = undefined;
                }
            } else {
                nrdp.gibbon.scene.overlay = undefined;
            }
            nrdp.gibbon.splash.root = undefined;
        } else if(!nrdp.gibbon.splash.root) {
            if(nrdp.gibbon.scene.root && nrdp.gibbon.scene.root.visible)
                return;

            splashWidget = nrdp.gibbon.splash.root = nrdp.gibbon.makeWidget();
            splashWidget.name = "SplashWidget";
            splashWidget.cache = false;
            splashWidget.color = "#050505";
            splashWidget.opaque = true;
            splashWidget.x = 0;
            splashWidget.y = 0;
            splashWidget.width = 1280;
            splashWidget.height = 720;
            splashWidget.c = {};

            var background = splashWidget.c.background = nrdp.gibbon.makeWidget();
            background.pending = 0;
            background.visible = false;
            background.parent = splashWidget;
            background.name = "SplashWidget::Background";
            background.cache = false;
            background.color = "#050505";
            background.x = 0;
            background.y = 0;
            background.width = 1280;
            background.height = 720;
            background.addPending = function() {
                ++background.pending;
                return function() {
                    if(nrdp.gibbon.splash.root && !--background.pending) {
                        nrdp.gibbon.scene.overlay = splashWidget;
                        background.visible = true;
                        if(nrdp.gibbon.splash.options.animated && nrdp.gibbon.splash.options.animated.in) {
                            if(!nrdp.gibbon.capabilities.splash.device || nrdp.gibbon._locationCount != 1)
                                background.startAnimation("opacity", 0, 1, 500, "ease-in-quint");
                            if(spinner)
                                spinner.startAnimation("opacity", 0, 1, 2000, "ease-in-quint");
                        }
                        nrdp._callEventListeners(nrdp.gibbon.splash, { type: "splashChanged", visible: visible });
                    }
                };
            };
            {
                var backgroundImage = nrdp.gibbon.makeWidget();
                backgroundImage.cache = false;
                backgroundImage.name = "SplashWidget::Background::Image";
                backgroundImage.x = 0;
                backgroundImage.y = 320;
                backgroundImage.width = 1280;
                backgroundImage.height = 353;
                backgroundImage.image.url = "http://localcontrol.netflix.com/img/Netflix_Background_Splash.png";
                backgroundImage.image.addEventListener("imageLoaded", background.addPending());
                backgroundImage.parent = background;
            }

            var logo = splashWidget.c.logo = nrdp.gibbon.makeWidget();
            logo.parent = background;
            logo.name = "SplashWidget::Logo";
            logo.cache = false;
            logo.smoothScale = true;
            logo.transformOriginX = 200;
            logo.transformOriginY = 54;
            logo.x = 440;
            logo.y = 306;
            logo.width = 400;
            logo.height = 108;
            logo.image.halign = logo.image.valign = "center";
            logo.image.url = "http://localcontrol.netflix.com/img/Netflix_Logo_Splash.png";
            logo.image.addEventListener("imageLoaded", background.addPending());

            var shadow = splashWidget.c.shadow = nrdp.gibbon.makeWidget();
            shadow.parent = background;
            shadow.name = "SplashWidget::Shadow";
            shadow.cache = false;
            shadow.smoothScale = true;
            shadow.transformOriginX = 260;
            shadow.transformOriginY = 18;
            shadow.x = 380;
            shadow.y = 489;
            shadow.width = 521;
            shadow.height = 37;
            shadow.image.halign = shadow.image.valign = "center";
            shadow.image.url = "http://localcontrol.netflix.com/img/Netflix_Shadow_Splash.png";
            shadow.image.addEventListener("imageLoaded", background.addPending());

            if(nrdp.gibbon.splash.options.sound && nrdp.gibbon.splash.options.sound.in) {
                var soundIn = "http://localcontrol.netflix.com/wav/splash.wav";
                nrdp.audio.load(soundIn, function() {
                    if(nrdp.suspended) {
                        nrdp.audio.unload(soundIn);
                    } else {
                        splashWidget.c.soundIn = soundIn;
                        nrdp.audio.play(splashWidget.c.soundIn, 1, function() {
                            if(splashWidget.c.soundIn) {
                                nrdp.audio.unload(splashWidget.c.soundIn);
                                splashWidget.c.soundIn = undefined;
                            }
                        });
                    }
                });
            }
            if(nrdp.gibbon.splash.options.sound && nrdp.gibbon.splash.options.sound.out) {
                var soundOut = "http://localcontrol.netflix.com/wav/splash.wav";
                nrdp.audio.load(soundOut, function() { splashWidget.c.soundOut = soundOut; });
            }
            if(nrdp.gibbon.splash.options.animated && nrdp.gibbon.splash.options.animated.spinner) {
                var spinner = splashWidget.c.spinner = nrdp.gibbon.makeWidget();
                spinner.parent = background;
                spinner.name = "SplashWidget::Spinner";
                spinner.cache = false;
                spinner.x = 616;
                spinner.y = 585;
                spinner.width = 48;
                spinner.height = 48;
                spinner.image.halign = spinner.image.valign = "center";
                spinner.image.sourceRect = { x: 0, y: 0, width: 48, height: 48 };
                spinner.image.url = "http://localcontrol.netflix.com/img/Spinner_Splash.mng";
                spinner.image.addEventListener("imageLoaded", background.addPending());
            }
        }
    }
};
nrdp.addEventListener("init", function() { nrdp.gibbon.splash.init(true); });
nrdp.gibbon.scene.addEventListener("rootChanged", function() {
    nrdp.gibbon.splash._update(!nrdp.gibbon.scene.root || !nrdp.gibbon.scene.root.visible);
});
nrdp.isReady ? nrdp.gibbon.splash.init(true) : nrdp.init();
