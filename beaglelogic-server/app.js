
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

io = require('socket.io')(server, { pingInterval: 60000, pingTimeout: 120000});
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

	socket.on('error', function(data) {
		console.log('Data error: ' + data);
	});

	socket.on('sigrok-test', function(data) {
		var child_process = require('child_process');
		var sigrok = child_process.spawn('sigrok-cli', [
				'-d', 'beaglelogic',
				'--samples', '3k',
				'-c', 'samplerate=3M',
				'--channels', 'P8_40,P8_45,P8_39',
				'-t', 'P8_39=f',
				'--output-format', 'ascii:width=3000']);

		console.log('Launching sigrok-cli...');

		var x = '';
		sigrok.stdout.on('data', function(data) {
			x = x + data;
		});

		sigrok.on('close', function(ret) {
			socket.emit('sigrok-data', x);
			console.log('process exited with code ' + ret);
		});
	});

	// Reserved functions
	
	// Sending a buffer over a socket is "terrible"
	// So encode it into a base64 string in 512K byte chunks and send it over
	function sendBuffer(buf, completion) {
		socket.emit('sigrok-data-header', { size: buf.length, enc: 'base64' });

		(function b(buf, i, completion) {
			blockSize = 512 * 1024;
			slice = buf.slice(i, i + blockSize)
			if (slice.length) {
				str = slice.toString('base64');
				socket.emit('sigrok-data-big', str, function(data) {
					b(buf, i + slice.length, completion);
				});
			} else
				completion();
		})(buf, 0, completion);
	}

	socket.on('beaglelogic-test', function(data) {
		var LZ4 = require('lz4');

		// Read /dev/beaglelogic and stream it over to the host
		sizes = [256, 262144, 524288, 1048576, 4194304];
		t1 = process.hrtime();

		console.log('L='+ LZ4.encodeBound(buf.length));
		var output = new Buffer( LZ4.encodeBound(buf.length) );

		// block compression (no archive format)
		var compressedSize = LZ4.encodeBlock(buf, output);

		console.log('L='+ compressedSize);

		// remove unnecessary bytes
		output = output.slice(0, compressedSize);
		sz = compressedSize;

		sendBuffer(output, function() {
			diff = process.hrtime(t1);
			timediff = diff[0] + diff[1] * 1e-9;
			console.log('sz = ' + sz + ' bytes, t = ' + timediff + 'sec, Speed =' + sz / timediff / 768 + ' KB/s - ' + data);
		});
	});
}
