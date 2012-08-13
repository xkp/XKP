var http = require("http");
var url = require("url");
require("./smart.min.js");
var mysql = require("mysql");
var application = {};
application.myService = function (request, response)
{
    function return_function(return_value)
    {
        response.end(JSON.stringify(return_value))
    }
    var __params = url.parse(request.url, true).query;
    return_function([Math.random(), Math.random(), Math.random()]);
    return true;
};
application.myPage = function (request, response)
{
    var rnumber = {};
    var rnumbers = {};
    var smarty_div = {};

    function return_function()
    {
        response.write("<!DOCTYPE html>\n<html>\n    <script type=\"text/javascript\">\n    function loadXMLDoc()\n        {\n            var xmlhttp;\n            if (window.XMLHttpRequest)\n              {// code for IE7+, Firefox, Chrome, Opera, Safari\n                xmlhttp=new XMLHttpRequest();\n              }\n            else\n              {// code for IE6, IE5\n                xmlhttp=new ActiveXObject(\"Microsoft.XMLHTTP\");\n              }\n            \n            xmlhttp.onreadystatechange=function()\n              {\n                if (xmlhttp.readyState==4 && xmlhttp.status==200)\n                {\n                    document.getElementById(\"myDiv\").innerHTML=xmlhttp.responseText;\n                }\n              }\n            xmlhttp.open(\"GET\",\"/myService\",true);\n            xmlhttp.send();\n        }\n    </script>\n    <body>\n        <h4>\n            Here is a random number: ");
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
        response.write("\n\n        <h3> Rendered as smarty: </h3>\n        ");
        response.write("<div id=\"smarty_div\">" + smarty_div.inner_html + "</div>");
        response.write("\n\n        <button type=\"button\" onclick=\"loadXMLDoc()\">Get From Service</button>\n        <div id=\"myDiv\">\n        </div>\n\n    </body>\n</html>\n\n");
        response.end();
    }
    rnumber.inner_html = Math.random();
    var numbers = [Math.random(), Math.random(), Math.random()];
    rnumbers.data = numbers;
    smarty_div.inner_html = application.tpl4067313322.fetch(
    {
        items: numbers
    });
    return_function();
};
application.init = function ()
{
    this.tpl4067313322 = new jSmart("\n        <ul>\n        {foreach from=$items item=rn}\n            <li>{$rn}</li>\n        {/foreach}\n        </ul>    \n    ");
}
application.init();
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