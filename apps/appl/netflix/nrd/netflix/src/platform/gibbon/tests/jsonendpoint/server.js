var WebSocketServer = require('ws').Server;
var server = new WebSocketServer({port: 8089});
var readline = require('readline');
var rl = readline.createInterface({input: process.stdin, output: process.stdout});
rl.setPrompt("ws> ");

console.log("listening on " + server.options.port);

var sockets = [];
server.on('connection', function(ws) {
    console.log("got connection");
    rl.prompt();
    sockets.push(ws);
    ws.on('message', function(message) {
        console.log('received: %s', message);
        rl.prompt();
    });
});

var id = 0;
rl.prompt();
rl.on("line", function(l) {
    var obj = JSON.stringify({ method: "eval", params: [l], id: ++id });
    for (var idx in sockets) {
        sockets[idx].send(obj);
    }
    rl.prompt();
});
