
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
// app.use(express.logger('dev'));
app.use(express.static(path.join(__dirname, '../webapp')));

server = http.createServer(app);
server.listen(app.get('port'), function(){
	console.log('Express server listening on port ' + app.get('port'));
});

io = require('socket.io')(server, { pingInterval: 60000, pingTimeout: 120000});
io.on('connection', connectionHandler);

function connectionHandler(socket) {
	console.log('Socket.IO connected');

	socket.on('disconnect', function(data) {
		console.log('Someone disconnected: ' + JSON.stringify(data));
	});

	socket.on('error', function(data) {
		console.log('Data error: ' + data);
	});

	socket.on('sigrok-test', function(data) {
		child_process = require('child_process');
		
		args = [
				'-d', 'beaglelogic',
				'--samples', data.samplecount,
				'-c', 'samplerate=' + data.samplerate,
				'--channels', data.channels,
				'--output-format', 'ascii:width=' + data.samplecount];

		if (data.extended) {
			args[1] += ":logic_channels=14"
		}

		if (data.trigger) {
			args.push('-t');
			args.push(data.trigger);
		}
	
		console.log('args = ' + args);

		// Run sigrok-cli
		var sigrok = child_process.spawn('sigrok-cli', args);

		console.log('Launching sigrok-cli...');

		// Receive data
		var output = '';
		sigrok.stdout.on('data', function(data) {
			output += data;
		});

		// Emit it
		sigrok.on('close', function(ret) {
			console.log('process exited with code ' + ret);
			socket.emit('sigrok-data', output);
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
