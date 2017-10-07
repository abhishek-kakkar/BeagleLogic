var fs = require("fs")
var net = require("net")
var path = require("path")
//var lz4 = require("lz4")

var port = process.env.PORT || 5555

function beaglelogic_sysfs_attr(s) {
    return path.normalize("/sys/devices/virtual/misc/beaglelogic/" + s)
}

function beaglelogic_sysfs_attr_read(attr) {
    return String(fs.readFileSync(beaglelogic_sysfs_attr(attr))).trim()
}

function beaglelogic_sysfs_attr_write(attr, value) {
    fs.writeFileSync(beaglelogic_sysfs_attr(attr), value)
}

var server = net.createServer((socket) => {
    console.log(`Accepted connection from ${socket.remoteAddress}`)

    var limit_samples = -1
    var bl_read_stream

    socket.on("error", (err) => {
        // Silently suppress
        //console.log(err)
    })

    socket.on("data", (data) => {
        var cmd = String(data).trim().split(" ")
        var valid_sysfs_attrs = ["samplerate", "sampleunit", "triggerflags", "bufunitsize",
                                 "memalloc", "state"]

        console.log("Received command: " + cmd)
        // Command as:
        //      samplerate
        // Outputs an integer value i.e. the samplerate
        //
        // Command as:
        //      samplerate <value>
        // Sets the samplerate to the value passed.
        if (valid_sysfs_attrs.indexOf(cmd[0]) != -1) {
            if (cmd.length == 1) {
                resp = parseInt(beaglelogic_sysfs_attr_read(cmd[0]))
                // sampleunit outputs 8bit (=1) and 16bit (=0)
                if (cmd[0] == "sampleunit")
                    resp = resp == 8 ? 1 : 0;
                socket.write(resp + "\r\n")
            } else {
                try {
                    beaglelogic_sysfs_attr_write(cmd[0], cmd[1])
                    socket.write("OK\r\n")
                } catch (error) {
                    socket.write("ERR\r\n")
                }
            }
        } else {
            switch (cmd[0]) {
                case "limit":
                    if (cmd.length == 1)
                        socket.write(limit_samples + "\r\n")
                    else {
                        limit_samples = parseInt(cmd[1])
                        socket.write("OK\r\n")
                    }
                    break

                case "get":
                    bl_read_stream = fs.createReadStream('/dev/beaglelogic',
                        {highWaterMark: 128 * 1024})
                    bl_read_stream.pipe(socket, {end: false})
                    break

                case "close":
                    bl_read_stream.unpipe()
                    bl_read_stream.close()
                    bl_read_stream.destroy()
                    break

                case "version":
                    model = String(fs.readFileSync("/proc/device-tree/model")).slice(0, -1)
                    if (model == "TI AM335x BeagleLogic Standalone")
                        socket.write("BeagleLogic Standalone 1.0\r\n")
                    else
                        socket.write("BeagleLogic 1.0\r\n")
                    break

                case "exit":
                    socket.end()
                    break
            }
        }
    })

    socket.on("end", (err) => {
        console.log("Session ended")
    })
})

server.maxConnections = 1
server.listen(port, () => { console.log(`Server listening at port ${port}`) })
