opera.postError("Opera HbbTV user JS loading ..");

// Add onreadystatechange for XMLHttpRequests
(function() {
	var tmpInstance=new XMLHttpRequest();// because Opera doesn't support getter/setter lookup on prototype
	var orscGetter=tmpInstance.__lookupGetter__('onreadystatechange');
	var orscSetter=tmpInstance.__lookupSetter__('onreadystatechange');
	XMLHttpRequest.prototype.__defineGetter__('onreadystatechange', function(){
		return orscGetter.call(this);
	});
	XMLHttpRequest.prototype.__defineSetter__('onreadystatechange', function( actualListener ){
		var intermediateListener = function(){
			actualListener.call(this, {target: this});
		}
		return orscSetter.call(this, intermediateListener);
	});
})();


// Set overflow:hidden needs to be done on both <html> and <body>
window.addEventListener('load', function() {
	if (getComputedStyle(document.getElementsByTagName('body')[0], null).overflow == 'hidden') {
		document.getElementsByTagName('html')[0].style.overflow = 'hidden';
	}
}, false);


// Patch for Redmine issue #1246
if (location.hostname.indexOf('hbbtv.ndr.de') != -1) {
	opera.addEventListener('BeforeScript', function(e) {
		e.element.text = e.element.text.replace("startFallbackUrl: 'dvb://current.ait/13.1B?autoshow=1", "startFallbackUrl: 'dvb://current.ait/13.1?autoshow=1");
	}, false);
}


// Patch for issues in jQuery 1.6.2 related to operations on innerHTML within an XML context (HSDK-1522).
if (location.hostname.indexOf('json.bild.de') != -1) {
	opera.addEventListener('BeforeScript', function(e) {
		if (e.element.src.indexOf('jquery1.6.2.min.js') != -1) {
			e.element.text = e.element.text.replace('o.innerHTML=m[1]+k+m[2];', 'try{o.innerHTML=m[1]+k+m[2];}catch(e){};');
		}
	}, false);
}

// KAYO-868 
if ( location.hostname.indexOf('.prosieben.de') != -1 ||
	 location.hostname.indexOf('hbbtv.kabeleins.de') != -1 ||
	 location.hostname.indexOf('hbbtv.sat1.de') != -1 ) {
        opera.addEventListener('BeforeScript', function(e) {
                if (e.element.src.indexOf('new.js') != -1) {
                        e.element.text = e.element.text.replace('o(U);h.setFullScreen&&h.setFullScreen(!!U)', 'h.setFullScreen&&h.setFullScreen(!!U);o(U)');
                        e.element.text = e.element.text.replace('j.body.style.visibility="hidden";', 'j.body.style.visibility="hidden";j.body.style.background="none";');
                }
        }, false);
}

// KAYO-1045 
if ( location.hostname.indexOf('dash-mse-test') != -1 ||
	 location.hostname.indexOf('dash-player.html') != -1 ||
	 location.hostname.indexOf('googleapis.com') != -1 ) {

opera.addEventListener('BeforeScript', function(e) { 
                if (e.element.src.indexOf('player.js') != -1) { 
                        e.element.text = e.element.text.split("change', true, false").join("change', [true, false]");
                }
        }, false);
}

(function() {
	var NON_CHAR_KEYS = [
		KeyboardEvent.DOM_VK_BACKSPACE,
		KeyboardEvent.DOM_VK_LEFT,
		KeyboardEvent.DOM_VK_UP,
		KeyboardEvent.DOM_VK_RIGHT,
		KeyboardEvent.DOM_VK_DOWN,

		KeyboardEvent.DOM_VK_RED,
		KeyboardEvent.DOM_VK_GREEN,
		KeyboardEvent.DOM_VK_YELLOW,
		KeyboardEvent.DOM_VK_BLUE,
		
		KeyboardEvent.DOM_VK_PLAY,
		KeyboardEvent.DOM_VK_STOP,
		KeyboardEvent.DOM_VK_PAUSE,
		KeyboardEvent.DOM_VK_FAST_FWD,
		KeyboardEvent.DOM_VK_REWIND,
		KeyboardEvent.DOM_VK_TRACK_NEXT,
		KeyboardEvent.DOM_VK_TRACK_PREV,
		
		KeyboardEvent.DOM_VK_BACK,
		KeyboardEvent.DOM_VK_INFO,
	];

	window.opera.addEventListener("AfterEvent.keydown", function (evt) {
		var keydown = evt.event;  // the keydown event

		// Apply the fix for NON_CHAR_KEYS only
		if (NON_CHAR_KEYS.indexOf(keydown.keyCode) === -1) {
			return;
		}

		var keypress = document.createEvent("Event");
		keypress.initEvent("keypress", true, true);
		keypress.keyCode = keydown.keyCode;
		keypress.which = keydown.keyCode;
		keypress.charCode = 0;

		keydown.target.dispatchEvent(keypress);

		if (keypress.defaultPrevented) {
			// Prevent the parent keydown event
			keydown.preventDefault();
		}
	});

	// Fix the window.close() function. Normally, it can only be used to close windows opened from JavaScript
	if ( oipfObjectFactory.createApplicationManagerObject().getOwnerApplication(window.document) )
	{
			window.close = function() {

			var appman = oipfObjectFactory.createApplicationManagerObject();
			var app = appman.getOwnerApplication(window.document);
			app.destroyApplication();
		}
	}
})();

