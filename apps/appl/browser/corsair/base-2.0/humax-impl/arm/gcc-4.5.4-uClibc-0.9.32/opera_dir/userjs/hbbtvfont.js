 if ( location.hostname.indexOf('humaxtvportal') == -1 &&
      location.hostname.indexOf('localhost') == -1  &&
      location.hostname.indexOf('appspot.com') == -1  &&
      location.hostname.indexOf('googleapis.com') == -1  &&
      location.hostname.indexOf('youtube.com') == -1 ){
		function addCssToDocument(cssText, doc, mediaType){
			    doc = doc||document;
			    mediaType = mediaType||'';
			    addCssToDocument.styleObj=addCssToDocument.styleObj||{};
			    var styles = addCssToDocument.styleObj[mediaType];
			    if(!styles){
			            var head = document.getElementsByTagName("head")[0];
			            if( !head ){
			                    var docEl = document.getElementsByTagName("html")[0];
			                    if(!docEl){
			                            window.addEventListener("DOMContentLoaded",
			                            function(){ addCssToDocument(cssText, doc); },false);
			                            return;
			                    }
			                    head = document.createElement("head");
			                    if(head) document.insertBefore(head,docEl.firstChild);
			                    else head = docEl;
			            }
			            addCssToDocument.styleObj[mediaType] = styles = document.createElement("style");
			            styles.setAttribute("type","text/css");
			            if(mediaType)styles.setAttribute("media", mediaType);
			            styles.appendChild(document.createTextNode(' '));
			            head.appendChild(styles);
			    }
			    styles.innerHTML += cssText+"\n";
			    return true;
			}
	        addCssToDocument('* {font-family: "TiresiasScreenfont" !important; }');
	}
