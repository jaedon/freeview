var http = require('http');
var express = require('express');
var bodyParser = require('body-parser');

require('./sockio');


// local variable
var HTTP_PORT_NUMBER = 12321;
var app = express();

app.use(bodyParser.json());
app.use(bodyParser.urlencoded({ extended: true }));
app.use(function (req, res, next) {
    res.header('Access-Control-Allow-Origin', '*');
    next();
});
app.use(express.static('public'));

process.on('uncaughtException', function(err) {
	console.log('###################################################');
	console.log('Exception!!!!!');
	console.log('###################################################');
	console.error(err);
	console.log('###################################################');

	emitEvent('exception', err);
});


require('./routes').setRouter(app);


var httpServer = http.createServer(app).listen(HTTP_PORT_NUMBER, function () {
	console.log('##############################################################');
	console.log('Simple RP Echo Server running port : ' + HTTP_PORT_NUMBER);
	console.log('##############################################################\n\n');
});

// start socket.io
startSocketIO(httpServer);
