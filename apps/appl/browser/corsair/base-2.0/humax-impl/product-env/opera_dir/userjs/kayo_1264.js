if(location.hostname.indexOf('fetd.prod.cps.awseuwest1.itvcloud.zone')>-1){
	window.addEventListener('load', function (e){
		function insertAfter(newElement,targetElement) {
			var parent = targetElement.parentNode;
			if(parent.lastchild == targetElement) {
				parent.appendChild(newElement);
			} else {
				parent.insertBefore(newElement, targetElement.nextSibling);
			}
		}
		var elem = document.createElement('style');
		var textnode = document.createTextNode(".production-thumbnail__channel {z-index:10;}"); 	
		elem.appendChild(textnode);
		insertAfter(elem, document.head.firstChild);
}, false);
}
