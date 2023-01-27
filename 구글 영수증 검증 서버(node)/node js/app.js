const chk = require('./chk.js');
var http = require("http");


var ret = chk.checkReceipt('camera_unlimited','ejehaaiidbgecokgbcdibfne.AO-J1OwjId4laUSSVwIoTDB0KfK-UH3RTp2rJObFt5MsAwXfvLuupFjDXoVpsmxOcA0iwXdXMSfO0ExyN6rX_7jHvUyliEl7GA','com.blessnhs.cca');

console.log(ret);

// Create HTTP server and listen on port 8000 for requests
http.createServer(function(request, response) {

   // Set the response HTTP header with HTTP status and Content type
   response.writeHead(200, {'Content-Type': 'text/plain'});

   // Send the response body "Hello World"
   response.end('Hello World\n');
   
}).listen(8080);
