nrdp.gibbon._runConsole("/command ssl-peer-verification false");
nrdp.gibbon._hookProperty(nrdp.gibbon, 'defaultLocale', function(_locale) { nrdp.log.error("Ignoring set locale: " + _locale); });
nrdp.gibbon._hookProperty(nrdp.gibbon.Text.prototype, 'contents', function(_contents, old) {
    var text = this;
    if(this._translateRequest) {
        nrdp.gibbon.stopLoad(this._translateRequest);
        this._translateRequest = 0;
    }
    var translations;
    if(!nrdp.storage.transientData) {
        translations = {};
        nrdp.storage.transientData = { _translations: translations };
    } else {
        translations = nrdp.storage.transientData._translations;
        if(!translations) 
            translations = nrdp.storage.transientData._translations = {};
    }
    var translation = translations[_contents];
    old.call(text, translation);
    if(!translation) {
        var that = this;
        if(!translations["_token"]) {
            function _getTranslateToken(async)
            {
                nrdp.log.error("Getting translation token.");
                nrdp.gibbon.load({async: async, url: "https://datamarket.accesscontrol.windows.net/v2/OAuth2-13", requestMethod: "POST",
                                  headers: { "X-Gibbon-Cache-Control": "no-cache" },
                                  body: "client_secret=au1Nh3XgVhrdlxr7rR%2F%2B4f4x3eE0luDogoS7AHBfqPU%3D&client_id=Gibbon&scope=http%3A%2F%2Fapi.microsofttranslator.com&grant_type=client_credentials"}, function(data) {
                                      translations["_token"] = JSON.parse(data.data).access_token;
                                  });
                nrdp.gibbon.setTimeout(_getTranslateToken, 540000); // 9 minutes
            }
            _getTranslateToken(false);
        }
        //nrdp.log.error("Token: " + translations["_token"]);
        this._translateRequest = nrdp.gibbon.load({url: "http://api.microsofttranslator.com/v2/Ajax.svc/Translate?From=en&To=" + nrdp.gibbon.defaultLocale + "&Text=" + encodeURIComponent(_contents), headers: { "X-Gibbon-Cache-Control": "no-cache", "Authorization": "Bearer " + translations["_token"] } }, function(data) {
            var translation = ((data && data.data && data.data.length) ? data.data.substring(2, data.data.length-1) : _contents);
            translation = translation.replace("\\/","/");
            //nrdp.log.warn("Translated: " + text._widget._name + "@" + text._widget._id + " :: " + _contents + " -> " + translation);
            translations[_contents] = translation;//translation.replace("/span>", "</span>");
            old.call(text, translation);
        });
    }
});
