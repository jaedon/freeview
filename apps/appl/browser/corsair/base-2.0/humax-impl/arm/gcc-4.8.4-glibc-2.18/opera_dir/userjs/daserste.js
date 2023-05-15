// USERJS FILE FOR OPERA
// url: http://hbbtv.daserste.de
// Temporal solution for the issue of HbbTV Application who created as OIPF
// author: Jongwon Kim

if (window.location.href.indexOf('hbbtv.daserste.de')>-1 || window.location.href.indexOf('hbbtv.daserste.de')>-1
	|| window.location.href.indexOf('itv.ard.de/ardpeg')>-1 || window.location.href.indexOf('itv.ard.de/ardepg')>-1) {
	window.opera.defineMagicFunction('showApplication', function(){
		try {
			var app = document.getElementById('appmgr').getOwnerApplication(document);
			app.show();
			if (typeof app.activate != "undefined") {
				app.activate();
			}
			if (typeof app.activateInput != "undefined") {
				app.activateInput(true);
			}
		}
		catch(e) {}
	});
}
