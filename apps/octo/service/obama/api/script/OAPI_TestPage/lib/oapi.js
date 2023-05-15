
var OAPI = function(appName) {
	// TODO: initialization
/*
	if(!("WebSocket" in window)) {
		throw "'WebSocket' does NOT supported!";
	}
*/
	var a = this;
	a.name = appName;
}

OAPI.prototype.name = 'Unknown';
OAPI.prototype.socket = null;
OAPI.prototype.id = 0;
OAPI.prototype.onmessage = {
	success : null,
	error : null
};

OAPI.prototype.open = function(url, success, error) {
	var a = this;

	console.log("open: " + url);

	try {
		if(navigator.userAgent.match("firefox")=="firefox")
			a.socket = new MozWebSocket(url, "octo-protocol-via-websocket");
		else
			a.socket = new WebSocket(url, "octo-protocol-via-websocket");
	} catch (e) {
		console.log("new WebSocket(" + url + ") error!");
		error(e);
		return;
	}

	a.socket.onopen = function() {
		console.log("socket.onopen");

		a.query({
			"application" : {
				"target" : a.name,
				"operation" : "start"
			}
		}, success, error);

	}

	a.socket.onclose = function() {
		console.log("socket.onclose");
		error();
	}

	a.socket.onmessage = function(e) {
		console.log("socket.onmessage: " + e.data);

		try {
			// 뒤에 뭐가 붙는지 하나 잘라줘야 동작함 ...
			var message = JSON.parse(e.data.substr(0, e.data.length - 1));
		} catch (e) {
			console.log(e);
		}

		a.id = message.id;

		if(message.hugedata) {
			console.log("message: " + message);
			a.query({
				"datastorage" : {
					"accesscode" : message.hugedata.accesscode,
					"operation" : "get"
				}
			}, a.onmessage.success, a.onmessage.error);
		} else {
			console.log("message is not hugedata");
			if(a.onmessage.success)
				a.onmessage.success(message);
		}
	}

	a.socket.onerror = function(e) {
		console.log("socket.onerror: " + e);

		if(a.onmessage.error)
			a.onmessage.error();
	}
}

OAPI.prototype.close = function() {
	var a = this;

	console.log("close");

	a.socket = null;

}

OAPI.prototype.call = function(command) {
	var a = this;

	if(!(a.socket) || a.socket.readyState != WebSocket.OPEN)
		throw "'OAPI' does not opened!!";

	command.id = a.id;
	a.socket.send(JSON.stringify(command));
}

OAPI.prototype.query = function(command, success, error) {
	var a = this;

	if(!(a.socket) || a.socket.readyState != WebSocket.OPEN)
		throw "'OAPI does not opened!!";

	a.onmessage.success = success;
	a.onmessage.error = error;

	command.id = a.id;
	a.socket.send(JSON.stringify(command));

	return null;
}