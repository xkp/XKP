var http = require("http");
var url  = require("url");
var application = {};
http.createServer(function(request, response) 
{
    var pathname = url.parse(request.url).pathname;
    if (pathname == 'home'){home(request);}}).listen(8888);
