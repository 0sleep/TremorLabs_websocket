//NOTE: THIS IS SUPPOSED TO BE RUN ON THE COMMAND LINE USING NODE.JS
console.log("program started!")
const WebSocket = require('ws');
var webSocket = new WebSocket(`ws://192.168.137.${process.argv[2]}:80/ws`);
webSocket.onopen = function(event) {
  console.log("Sending HELO")
  webSocket.send("start stream")
}
webSocket.onmessage = function(event) {
  console.log(`got message: ${event.data}`)
}
webSocket.onclose = function(event) {
  console.log("socket closed")
}
