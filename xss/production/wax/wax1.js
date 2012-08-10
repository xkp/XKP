var http = require("http");
var url  = require("url");
require("./smart.min.js");
        var mysql  = require("mysql");
var application = {};
	application.myService = function(request,response) 
	{
function return_function(return_value)
{
response.end(JSON.stringify(return_value))
}
var __params = url.parse(request.url, true).query;return_function([Math.random(), Math.random(), Math.random()]);
return true;
	};
	application.myPage = function(request,response) 
	{
		var rnumber = {};
var rnumbers = {};
var smarty_div = {};
function return_function()
{
response.write("<!DOCTYPE html>\n<html>\n    <script type=\"text/javascript\">\n    function loadXMLDoc()\n        {\n            var xmlhttp;\n            if (window.XMLHttpRequest)\n              {// code for IE7+, Firefox, Chrome, Opera, Safari\n                xmlhttp=new XMLHttpRequest();\n              }\n            else\n              {// code for IE6, IE5\n                xmlhttp=new ActiveXObject(\"Microsoft.XMLHTTP\");\n              }\n            \n            xmlhttp.onreadystatechange=function()\n              {\n                if (xmlhttp.readyState==4 && xmlhttp.status==200)\n                {\n                    document.getElementById(\"myDiv\").innerHTML=xmlhttp.responseText;\n                }\n              }\n            xmlhttp.open(\"GET\",\"/myService\",true);\n            xmlhttp.send();\n        }\n    </script>\n    <body>\n        <h4>\n            Here is a random number: ");
response.write( "<a id=\"rnumber\">" + rnumber.inner_html + "</a>");response.write("\n        </h4>\n        <h3> And some more: </h3>\n        ");
response.write("<ul id=\"rnumbers\">");
for(var rnumbers_iterator = 0; rnumbers_iterator < rnumbers.data.length; rnumbers_iterator++)
{
var item = rnumbers.data[rnumbers_iterator];
var arnumber = {};
arnumber.inner_html = item;
response.write("<li>\n                ");
response.write( "<a id=\"arnumber\">" + arnumber.inner_html + "</a>");response.write("\n            </li>");
}
response.write("</ul>");
response.write("\n\n        <h3> Rendered as smarty: </h3>\n        ");
response.write( "<div id=\"smarty_div\">" + smarty_div.inner_html + "</div>");response.write("\n\n        <button type=\"button\" onclick=\"loadXMLDoc()\">Get From Service</button>\n        <div id=\"myDiv\">\n        </div>\n\n    </body>\n</html>\n\n");
response.end();
}var number = Math.random();
if (number > 0.699999988)
{
response.end(application.tpl2306366569.fetch({tpl : "wax1.tpl",target : number}));
return true;
}
var numbers = [Math.random(), Math.random(), Math.random()];
rnumber.inner_html = number;
rnumbers.data = numbers;
smarty_div.inner_html = application.tpl4067313322.fetch({items : numbers});
return_function();
	};
	application.myApp = function(request,response) 
	{
		var btnFullName = {};
function return_function()
{
response.write("<html>\n<body>\n\n<table width=\"500\" border=\"0\">\n<tr>\n<td colspan=\"2\" style=\"background-color:#FFA500;\">\n<h1>Main Title of Web Page</h1>\n</td>\n</tr>\n\n<tr valign=\"top\">\n<td style=\"background-color:#FFD700;width:100px;text-align:top;\">\n<b>Menu</b><br />\nHTML<br />\nCSS<br />\nJavaScript\n</td>\n<td id=\"content\" style=\"background-color:#EEEEEE;height:200px;width:400px;text-align:top;\">\n\n\n\n<div id=\"__div1\">\n\n\n\n				");
response.write("<input value = ");
response.write('\"' + btnFullName.value + '\"' );
response.write("id = \"btnFullName\"name = \"btnFullName\"type = \"button\"");
response.write("\n				</input>\n			\n\n\n\n				<label id=\"lblFullName\"\n			>\n					\n				\n				</label>\n			\n</div>\n\n\n\n\n</td>\n</tr>\n\n<tr>\n<td colspan=\"2\" style=\"background-color:#FFA500;text-align:center;\">\nCopyright © 2011 W3Schools.com</td>\n</tr>\n</table>\n\n    <script type=\"text/javascript\" src=\"../js/jquery/jquery.js\"></script>\n    <script type=\"text/javascript\" src=\"../js/jquery/jquery-ui.js\"></script>\n    <script type=\"text/javascript\" src=\"../js/jquery/utils.js\"></script>\n\n\n            <script type=\"text/javascript\" src=\"../js/jquery-1.7.1.min.js\"></script>\n        \n\n\n\n            <script type=\"text/javascript\" src=\"../js/jquery-ui-1.8.18.custom.min.js\"></script>\n        \n\n\n\n            <script type=\"text/javascript\" src=\"../js/jquery-idiom.js\"></script>\n        \n\n\n\n            <link rel=\"stylesheet\" href=\"../css/ui-lightness/jquery-ui-1.8.18.custom.css\"></link>\n        \n\n<style>\n\n\n#__div1 {position : relative}\n\n\n#btnFullName {left : 140px;top : 110px;width : 200px;height : 43px;position : absolute}\n\n\n#lblFullName {left : 10px;top : 170px;width : 300px;height : 25px;position : absolute}\n\n</style>\n<script type=\"text/javascript\">\n\n        var application = {};\n    \n			var __div1 = $(\"#__div1\");\n		\n			var btnFullName = $(\"#btnFullName\").button();\n		\n			var lblFullName = $(\"#lblFullName\");\n		\n\n\n\n\n\n\n\n\n\n\n\n		    btnFullName.click( function() { \n	    \n			lblFullName.text(\"Grabbing \");\nvar loc;\nfunction __callback1() \n{\nlblFullName.text(loc.geometry.location.Lat + \" \" + loc.geometry.location.Long);\n\n}\n\n\n\n\n\n\n        var __url1 = \"http://maps.googleapis.com/maps/api/geocode/json\"\n     + \"?address=\" + \"780 NE 69st\" + \"&sensor=\" + false;\n        $.ajax(\n		{\n		    url: __url1,\n			type: \"get\",\n			dataType: \"json\",\n			error: function()\n            {\n                throw e;\n			},\n			success: function(data)\n            {\n                var return_value = JSON.parse(data);\n                loc = return_value;\n__callback1();\n			}\n		});\n		\n		});\n	\n\n\n\n\n\n\n\n\n\n\n\n                    if (application.init)\n                        application.init();\n                \n</script>\n</body>\n</html>\n");
response.end();
}btnFullName.value = Math.random();
return_function();
	};
application.init = function()
{
this.tpl2306366569 = new jSmart( "<!DOCTYPE html>\n<html>\n    <body>\n        <h4>\n            Your 30% chance took you here: <strong>{$target}</strong>\n        </h4>\n        <a href=\"/myPage\">You can try again</a>\n    </body>\n</html>\n\n" );
this.tpl4067313322 = new jSmart( "\n        <ul>\n        {foreach from=$items item=rn}\n            <li>{$rn}</li>\n        {/foreach}\n        </ul>    \n    " );
}
application.init();
http.createServer(function(request, response) 
{
    var pathname = url.parse(request.url).pathname;
    if (pathname == '/myService'){application.myService(request, response);}if (pathname == '/myPage'){application.myPage(request, response);}if (pathname == '/myApp'){application.myApp(request, response);}}).listen(8888);
