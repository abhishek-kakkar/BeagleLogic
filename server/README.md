

# beaglelogic-server

Backend of the BeagleLogic web client that runs on Node.JS and serves the static
application and acts as the data link between the Bone and the browser client.

## Usage

First run `npm install` to install the dependencies and then run the app.js
file. This can be done using either `node app.js` or `npm start`

The web client can then be accessed on port 4000 . The URL will generally be
`http://192.168.7.2:4000/`

```
root@beaglebone:~$ cd 'dir-to-webapp'
root@beaglebone:~/BeagleLogic/beaglelogic-server$ npm install
root@beaglebone:~/BeagleLogic/beaglelogic-server$ npm start
```

Later versions of this app shall install as a service on the Bone image, similar
to Bone101.

## License

MIT


