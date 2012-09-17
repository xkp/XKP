var http = require("http");
var url  = require("url");
var fs = require('fs');
var sessions = require("sessions");
require("./smart.min.js");
var application = {};
//init sessions
SessionManager = new sessions();
SessionManager.on("expired", function(sid){
    if (application.session_expired)
    {
        application.session_expired(sid);
    }
});
        var mysql  = require("mysql");
	application.home = function(request,response) 
	{
        var __params = url.parse(request.url, true).query;
        var user_name = {};
var followers = {};
function return_function()
{
response.write("<!DOCTYPE html>\n<html>\n    <body>\n\n        <h4>\n            Followers of ");
response.write("<a text = ");
response.write('\"' + user_name.text + '\"' );
response.write("id = \"user_name\">");
response.write("Mike</a>:\n        </h4>\n        ");
response.write("<ul id=\"followers\">");
for(var followers_iterator = 0; followers_iterator < followers.data.length; followers_iterator++)
{
var item = followers.data[followers_iterator];
var fimg = {};
var fname = {};
fname.inner_html = item.name;
fimg.src = item.image;
response.write("<li>\n                ");
response.write("<img src = ");
response.write('\"' + fimg.src + '\"' );
response.write("id = \"fimg\">");
response.write("</img>\n                ");
response.write( "<a id=\"fname\">" + fname.inner_html + "</a>");response.write("\n            </li>");
}
response.write("</ul>");
response.write("\n\n    </body>\n</html>\n\n");
response.end();
}var user = {id : "admin"};
user_name.text = user.id;
function __callback1() 
{
return_function();
}
function __catch1(__error)
{
__callback1();
}
try
{var data = [];
function __callback2() 
{
	try
	{function __callback3() 
{
	try
	{followers.data = data;
__callback1();
	}
	catch(__error)
	{
		__catch1(__error);
	}
}
var follower_iterable = data;
var follower_iterator = 0;
function __for_code1()
{
	var follower = follower_iterable[follower_iterator];function __callback4() 
{
	try
	{__for_iter1();
	}
	catch(__error)
	{
		__catch1(__error);
	}
}
function __catch2(__error)
{
 var err = __error;follower.map = "images/nomap.png";
__callback4();
}
try
{var img_cache;
function __callback5() 
{
	try
	{function __callback6() 
{
	try
	{follower.map = img_cache;
__callback4();
	}
	catch(__error)
	{
		__catch2(__error);
	}
}
if (!img_cache)
{var img_data;
function __callback7() 
{
	try
	{img_cache = "mapcache/" + Math.random() * 1000 + ".png";
function __callback8() 
{
	try
	{
        var __connection1 = mysql.createConnection(null);
        __connection1.connect(function(error) {
            if (error)
     {__catch2(error); return;}
        var __query1 = "INSERT INTO   MapCache(addr, fname)\n                               VALUES (" + follower.address + ", " + img_cache + ")";
        __connection1.query(__query1, function(error, rows, cols) {
            if (error)
     {__catch2(error); return;}
            __connection1.end();
            __callback6();
        });
        });
	}
	catch(__error)
	{
		__catch2(__error);
	}
}
fs.writeFile(img_cache, img_data, utf8, function(err)
{
	if (err)
	{
		__catch2(err); 
		 return; 
	}__callback8()
});
	}
	catch(__error)
	{
		__catch2(__error);
	}
}
        var __ws_url1 = "http://maps.googleapis.com/maps/api/staticmap"
     + "?center=" + follower.address + "&zoom=" + 5 + "&sensor=" + false;
        http.get(__ws_url1, function(res) {
            var data = '';
            res.setEncoding('binary');
            res.on('data', function(chunk){
                data += chunk;
            });
            function done()
            {
                var return_value = data;
                img_data = return_value;
__callback7();
            }
            res.on('end', done);
            res.on('close', done);
        }).on('error', function(e) {
          __catch2(e);
        });
}
else
{
__callback6();
}
	}
	catch(__error)
	{
		__catch2(__error);
	}
}
        var __connection2 = mysql.createConnection(null);
        __connection2.connect(function(error) {
            if (error)
     {__catch2(error); return;}
        var __query2 = "SELECT fname FROM MapCache\n                                WHERE addr = " + follower.address + "";
        __connection2.query(__query2, function(error, rows, cols) {
            if (error)
     {__catch2(error); return;}
                if (rows.length == 0)
                    img_cache = null;
                else if (rows.length == 1)
                {
                    var __found = false;
                    var __obj = rows[0];
                    for(var __prop in __obj)
                    {
                        if (!__found)
                        {
                            __found = true;
                            img_cache = __obj[__prop];
                            break;
                        }
                    }
                    if (!__found)
                        img_cache = null;
                }
                else
                    throw {error: "Expecting a single value returned by query"};
            __connection2.end();
            __callback5();
        });
        });
}
catch(__error)
{
	__catch2(__error)
}
}
function __for_iter1()
{
	follower_iterator++;
	__for_cond1();
}
function __for_cond1()
{
	if (follower_iterator < follower_iterable.length)
	__for_code1();
	else
	__callback3();
}
__for_cond1();
	}
	catch(__error)
	{
		__catch1(__error);
	}
}
        var __connection3 = mysql.createConnection(null);
        __connection3.connect(function(error) {
            if (error)
     {__catch1(error); return;}
        var __query3 = "SELECT * FROM FOLLOWERS\n                   WHERE USERID = " + user.id + "";
        __connection3.query(__query3, function(error, rows, cols) {
            if (error)
     {__catch1(error); return;} data = rows; 
            __connection3.end();
            __callback2();
        });
        });
}
catch(__error)
{
	__catch1(__error)
}
	};
application.init = function()
{
    }
application.init();
http.createServer(function(request, response) 
{
    var pathname = url.parse(request.url).pathname;
    if (pathname == '/home'){application.home(request, response);}}).listen(8888);
