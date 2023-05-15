var ws = new nrdp.WebSocket("http://localhost:8089/json");
ws.onmessage = function(msg) {
    nrdp.log.error("got message '" + msg.data + "'");
    var obj;
    try {
        obj = JSON.parse(msg.data);
    } catch (e) {
        nrdp.log.error("Unable to parse JSON from websocket");
        return;
    }
    var resp = { result: null, error: null, id: obj.id };
    if (obj.method === "eval") {
        try {
            resp.result = eval(obj.params[0]);
        } catch (e) {
            resp.error = e;
        }
    } else {
        resp.error = "unknown method";
    }
    var data;
    try {
        data = JSON.stringify(resp);
    } catch (e) {
        data = JSON.stringify({ result: null, error: e, id: obj.id });
    }
    try {
        ws.send(data);
    } catch (e) {
        nrdp.log.error("Unable to send data to websocket");
    }
};
ws.onerror = function() {
    nrdp.log.error("WS: got error");
};
ws.onclose = function() {
    nrdp.log.error("WS: got close, should reconnect");
};
