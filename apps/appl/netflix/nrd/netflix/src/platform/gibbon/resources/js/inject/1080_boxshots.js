nrdp.gibbon._hookProperty(nrdp.gibbon.Widget.prototype, 'name', function(_name, old) {
    var widget = this;
    var image_url = widget.image.url;
    if(image_url && parseInt(_name) == _name) {
        function loadImage(widget, url) {
            var hackImage = widget.addBackgroundImage();
            hackImage.url = widget.image.url;
            widget.image.url = url;
            function imageLoaded() {
                widget.removeImage(hackImage);
                widget.image.removeEventListener("imageLoaded", imageLoaded);
            };
            widget.image.addEventListener("imageLoaded", imageLoaded);
        };
        if(image_url instanceof Object)
            image_url = image_url.url;
        if(!nrdp.gibbon.Widget._1080Translation)
            nrdp.gibbon.Widget._1080Translation = {};
        var translation = nrdp.gibbon.Widget._1080Translation[image_url];
        if(translation) {
            loadImage(widget, { scale: 1.0, url: translation, lazy: widget.image.url.lazy, fast: widget.image.url.fast });
        } else {
            this._translateRequest = nrdp.gibbon.load({url: "https://api-global.netflix.com/tvui/innovation/release/66/darwin/pathEvaluator?method=get&pathFormat=hierarchical&withSize=true&path=%5B%22movies%22%2C%20" + _name + "%2C%20%22artWork%22%2C%20%22horizontalDisplayArt%22%2C%22byClosestFormat%22%2C%22webp%22%2C%22byClosestWidth%22%2C349%5D&v=next&output=json&engine=gibbon&haj=false&routing=reject&device_type=NFPS3-001-&certification_version=2&uiVersion=UI-release-66-gibbon-onyx-darwin&ui_trace_tag=darwin&languages=en-US,en&country=US&application_name=htmltvui&application_v=release-66&esn=NFPS3-001-YZ19CXPKDQXX4GTCW2DR83FPQW&user_id=BQAJAAEDELuBKgfRlXg7kFCFk1BmTKMws3CAnVPdh3X8Fl-lLASqulx7wvTZsgCn_1S9ggLQPv8NPIr0nWvt2czItRSa8aOV&uuid=1392936769702r504.48453705757856eNFPS3-001-YZ19CXPKDQXX4GTCW2DR83FPQWyClosestFormat%22%2C%22webp%22%2C%22byClosestWidth%22%2C349%5D" }, function(data) {
                var result = JSON.parse(data.data);
                var artWork;
                if(result.value.movies)
                    artWork = result.value.movies[_name].artWork;
                else if(result.value.shows)
                    artWork = result.value.shows[_name].artWork;
                if(artWork) {
                    var url = artWork.horizontalDisplayArt.byClosestFormat.webp.byClosestWidth["349"].url;
                    nrdp.gibbon.Widget._1080Translation[image_url] = url;
                    loadImage(widget, { scale: 1.0, url: url, lazy: widget.image.url.lazy, fast: widget.image.url.fast });
                }
            });
        }
    }
    old.call(widget, _name);
});

