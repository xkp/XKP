var http = require("http");
var url = require("url");
var mysql = require("mysql");
var application = {};
application.myService = function (response, request)
{
    function return_function(return_value)
    {
        request.end(JSON.stringify(return_value))
    }
    var __params = url.parse(request.url, true).query;
    return_function([Math.random(), Math.random(), Math.random()]);
    return true;
};
application.myPage = function (request, response)
{
    var rnumber = {};
    var rnumbers = {};

    function return_function()
    {
        response.write("<!DOCTYPE html>\n<html>\n    <body>\n        <h4>\n            Here is a random number: ");
        response.write("<a id=\"rnumber\">" + rnumber.inner_html + "</a>");
        response.write("\n        </h4>\n        <h3> And some more: </h3>\n        ");
        response.write("<ul id=\"rnumbers\">");
        for (var rnumbers_iterator = 0; rnumbers_iterator < rnumbers.data.length; rnumbers_iterator++)
        {
            var item = rnumbers.data[rnumbers_iterator];
            var arnumber = {};
            arnumber.inner_html = item;
            response.write("<li>\n                ");
            response.write("<a id=\"arnumber\">" + arnumber.inner_html + "</a>");
            response.write("\n            </li>");
        }
        response.write("</ul>");
        response.write("\n</html>\n\n");
        response.end();
    }
    rnumber.inner_html = Math.random();
    rnumbers.data = [Math.random(), Math.random(), Math.random()];
    return_function();
};
http.createServer(function (request, response)
{
    var pathname = url.parse(request.url).pathname;
    if (pathname == '/myService')
    {
        application.myService(request, response);
    }
    if (pathname == '/myPage')
    {
        application.myPage(request, response);
    }
}).listen(8888);