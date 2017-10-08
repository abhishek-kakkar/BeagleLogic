package main

import (
	"bufio"
	"fmt"
	"io/ioutil"
	"net"
	"os"
	"path/filepath"
	"strings"
)

func beaglelogicSysfsAttr(attr string) string {
	return filepath.Join("/sys/devices/virtual/misc/beaglelogic/", attr)
}

func beaglelogicSysfsAttrRead(attr string) (string, error) {
	dat, err := ioutil.ReadFile(beaglelogicSysfsAttr(attr))

	if err == nil {
		return strings.TrimSpace(string(dat)), err
	}
	return "", err
}

func beaglelogicSysfsAttrWrite(attr string, value string) error {
	return ioutil.WriteFile(beaglelogicSysfsAttr(attr), []byte(value), 0644)
}

func beagleLogicIsStandalone() bool {
	model, err := ioutil.ReadFile("/proc/device-tree/model")

	if err == nil {
		dt := strings.TrimSpace(string(model))
		return dt[0:len(dt)-1] == "TI AM335x BeagleLogic Standalone"
	}
	return false
}

var abort = false
var localBuf = make([]byte, 1024*1024)

func beaglelogicRun(conn net.Conn) {
	f, err := os.Open("/dev/beaglelogic")

	defer f.Close()

	if err != nil {
		return
	}
	for {
		if abort {
			break
		}
		n, _ := f.Read(localBuf)
		if n == 0 {
			break
		}
		conn.Write(localBuf[:n])
	}
}

func connHandler(conn net.Conn) {
	buf := bufio.NewReader(conn)

	sysfsattrs := []string{"samplerate", "sampleunit", "triggerflags", "bufunitsize", "memalloc", "state"}

	write := func(str string) {
		conn.Write([]byte(str))
	}

	for {
		_cmd, err := buf.ReadString('\n')
		if err != nil {
			fmt.Println("Session ended")
			break
		}
		cmd := strings.Split(strings.TrimSpace(_cmd), " ")

		fmt.Printf("Received command: %q\n", cmd)

		// Command as:
		//      samplerate
		// Outputs an integer value i.e. the samplerate
		//
		// Command as:
		//      samplerate <value>
		// Sets the samplerate to the value passed.
		for _, s := range sysfsattrs {
			if s == cmd[0] {
				if len(cmd) == 2 {
					err := beaglelogicSysfsAttrWrite(cmd[0], cmd[1])
					if err == nil {
						write("OK\r\n")
					} else {
						write("ERR\r\n")
					}
				} else {
					resp, _ := beaglelogicSysfsAttrRead(cmd[0])
					if cmd[0] == "sampleunit" {
						if resp[0] == '8' {
							resp = "1"
						} else {
							resp = "0"
						}
					} else if cmd[0] == "triggerflags" {
						resp = resp[0:1]
					}
					write(resp + "\r\n")
				}
			}
		}

		switch cmd[0] {
		case "version":
			if beagleLogicIsStandalone() {
				write("BeagleLogic Standalone 1.0\r\n")
			} else {
				write("BeagleLogic 1.0\r\n")
			}

		case "get":
			abort = false
			go beaglelogicRun(conn)

		case "close":
			abort = true

		case "exit":
			conn.Close()
			fmt.Println("Session closed")
			break
		}
	}
}

func main() {
	port := os.Getenv("PORT")
	if port == "" {
		port = "5555"
	}
	ln, err := net.Listen("tcp", ":"+port)
	if err != nil {
		panic(err)
	}

	fmt.Println("Listening on port " + port)

	defer ln.Close()
	for {
		conn, err := ln.Accept()
		if err != nil {
			panic(err)
		} else {
			fmt.Println("Accepted connection from: " + conn.RemoteAddr().String())
		}

		connHandler(conn)
	}
}
