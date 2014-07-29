
/**
 * Module dependencies.
 */

var express = require('express'),
	http = require('http'),
	fs = require('fs'),
	path = require('path'),
	app = express(),
	server,
	io;

// all environments
app.set('port', process.env.PORT || 4000);

app.use(express.favicon());
app.use(express.logger('dev'));

app.use(express.static(path.join(__dirname, '../webapp')));

server = http.createServer(app);
server.listen(app.get('port'), function(){
	console.log('Express server listening on port ' + app.get('port'));
});

io = require('socket.io')(server);
io.on('connection', connectionHandler);

function connectionHandler(socket) {
	console.log('Socket.IO connected');
	
	socket.on('message', function(data) {
		console.log('How is it?' + data);
	});
	
	socket.on('disconnect', function(data) {
		console.log('Someone disconnected: ' + JSON.stringify(data));
	});
	
	socket.on('sigrok-test-mock', function(data) {
		socket.emit('sigrok-data-receive', data);
	});
	
	socket.on('sigrok-test', function(data) {
		var child_process = require('child_process');
		var sigrok = child_process.spawn('sigrok-cli', [
				'-d', 'beaglelogic',
				'--samples', '1k', '-c',
				'samplerate=3M',
				'--channels', 'P8_40,P8_45,P8_39',
				'-t', 'P8_39=f',
				'--output-format', 'ascii:width=2000']);
		
		console.log('Launching sigrok-cli...');
		
		sigrok.stdout.on('data', function(data) {
			socket.emit('sigrok-data', ('' + data).split('\n'));
		});
		
		sigrok.on('close', function(ret) {
			console.log('process exited with code ' + ret);
		});
	});
	
	socket.on('beaglelogic-test', function(data) {
		// Read /dev/beaglelogic and stream it over to the host
	});
}
