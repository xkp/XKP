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
        var child_process  = require("child_process");
	application.home = function(request,response) 
	{
SessionManager.httpRequest(request, response, function (err, session) {
    if (err) 
        throw err;
        var __params = url.parse(request.url, true).query;
function return_function()
{
response.write("");
response.end();
}var logged = session.get("logged");
if (!logged)
{
application.login(request, response);
return true;
}
application.file_list(request, response);
return true;
return_function();
});
	};
	application.do_login = function(request,response) 
	{
SessionManager.httpRequest(request, response, function (err, session) {
    if (err) 
        throw err;
        var _form = new require('formidable').IncomingForm();
        var post = {fields: [], field_values: {}, files: []};
        _form.on('error', function(err) 
        {
            throw err;
        }).on('field', function(field, value) 
        {
            post.fields.push([field, value]);
            post.field_values[field] = value;
        }).on('file', function(field, file) 
        {
            post.files.push(file);
        }).on('end', function() 
        {
            var user = post.field_values["user"];
            var password = post.field_values["password"];
function return_function()
{
response.write("");
response.end();
}var user_data;
function __callback1() 
{
if (!user_data)
{
application.login(request, response);
return true;
}
if (user_data.Password && user_data.Password == password)
{
session.set("logged", true);
session.set("user", user);
session.set("user_id", user_data.UserID);
session.set("user_map", user_data.Map);
application.file_list(request, response);
return true;
}
else
{
application.login(request, response);
return true;
}
return_function();
}
        var __connection1 = mysql.createConnection(application.compressor_db);
        __connection1.connect(function(error) {
            if (error)
     throw error;
        var __query1 = "select * from Users\n                    where UserName = '" + user + "'";
        __connection1.query(__query1, function(error, rows, cols) {
            if (error)
     throw error;
                if (rows.length == 0)
                    user_data = null;
                else if (rows.length == 1)
                    user_data = rows[0];
                else
                    user_data = rows;
            __connection1.end();
            __callback1();
        });
        });
        });
        _form.parse(request);    
});
	};
	application.logout = function(request,response) 
	{
SessionManager.httpRequest(request, response, function (err, session) {
    if (err) 
        throw err;
        var __params = url.parse(request.url, true).query;
function return_function()
{
response.write("");
response.end();
}session.set("logged", false);
application.login(request, response);
return true;
return_function();
});
	};
	application.do_register = function(request,response) 
	{
        var _form = new require('formidable').IncomingForm();
        var post = {fields: [], field_values: {}, files: []};
        _form.on('error', function(err) 
        {
            throw err;
        }).on('field', function(field, value) 
        {
            post.fields.push([field, value]);
            post.field_values[field] = value;
        }).on('file', function(field, file) 
        {
            post.files.push(file);
        }).on('end', function() 
        {
            var user = post.field_values["user"];
            var password = post.field_values["password"];
            var location = post.field_values["location"];
function return_function()
{
response.write("");
response.end();
}var user_data;
function __callback2() 
{
if (user_data)
{
application.login(request, response);
return true;
}
var map_file = "usermaps/" + user + "_map.png";
var map_center = location || "Burundi";
var zoom = 8;
if (location)
{
zoom = 12;
}
var img_data;
function __callback3() 
{
function __callback4() 
{
function __callback5() 
{
application.login(request, response);
return true;
return_function();
}
        var __connection2 = mysql.createConnection(application.compressor_db);
        __connection2.connect(function(error) {
            if (error)
     throw error;
        var __query2 = "insert into Users (UserName, Password, Map)\n        values ('" + user + "', '" + password + "', '" + map_file + "')";
        __connection2.query(__query2, function(error, rows, cols) {
            if (error)
     throw error;
            __connection2.end();
            __callback5();
        });
        });
}
fs.writeFile("files/" + map_file, img_data, "binary", function(err)
{
	if (err)
		throw err;__callback4()
});
}
        var __ws_url1 = "http://maps.googleapis.com/maps/api/staticmap"
     + "?center=" + map_center + "&zoom=" + zoom + "&sensor=" + false + "&size=" + "300x200";
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
__callback3();
            }
            res.on('end', done);
            res.on('close', done);
        }).on('error', function(e) {
          throw e;
        });
}
        var __connection3 = mysql.createConnection(application.compressor_db);
        __connection3.connect(function(error) {
            if (error)
     throw error;
        var __query3 = "select * from Users\n                    where UserName = '" + user + "'";
        __connection3.query(__query3, function(error, rows, cols) {
            if (error)
     throw error;
                if (rows.length == 0)
                    user_data = null;
                else if (rows.length == 1)
                    user_data = rows[0];
                else
                    user_data = rows;
            __connection3.end();
            __callback2();
        });
        });
        });
        _form.parse(request);    
	};
	application.file_list = function(request,response) 
	{
SessionManager.httpRequest(request, response, function (err, session) {
    if (err) 
        throw err;
        var __params = url.parse(request.url, true).query;
        var user_name = {};
var files = {};
var user_map = {};
function return_function()
{
response.write("<!DOCTYPE html>\n<html>\n    <body>\n        <b>Welcome: </b>\n        ");
response.write( "<b id=\"user_name\">" + user_name.inner_html + "</b>");response.write(" <a href=\"/logout\">Log out</a>\n        <br/>\n        <br/>\n        \n        <h3>Compress a file</h3>\n        <form action=\"/compress\" enctype=\"multipart/form-data\" method=\"post\">\n            <p>\n                    <input type=\"file\" name=\"datafile\" size=\"40\">\n            </p>\n            <div>\n                <input type=\"submit\" value=\"Compress\">\n            </div>\n        </form>\n\n        <h3>Or select one already compressed</h3>\n        ");
response.write("<ul id=\"files\">");
for(var files_iterator = 0; files_iterator < files.data.length; files_iterator++)
{
var item = files.data[files_iterator];
var file_name = {};
var link = {};
file_name.inner_html = item.File;
link.href = item.Url;
response.write("<li>\n                ");
response.write( "<b id=\"file_name\">" + file_name.inner_html + "</b>");response.write(": ");
response.write("<a href = ");
response.write('\"' + link.href + '\"' );
response.write("id = \"link\">");
response.write("Download link</a> \n            </li>");
}
response.write("</ul>");
response.write("\n        <h3>You also live here:</h3>\n        ");
response.write("<img src = ");
response.write('\"' + user_map.src + '\"' );
response.write("id = \"user_map\">");
response.write("\n    </body>\n</html>\n\n");
response.end();
}var logged = session.get("logged");
if (!logged)
{
application.login(request, response);
return true;
}
var files;
var user_id = session.get("user_id");
function __callback6() 
{
user_name.inner_html = session.get("user");
files.data = files;
user_map.src = session.get("user_map");
return_function();
}
        var __connection4 = mysql.createConnection(application.compressor_db);
        __connection4.connect(function(error) {
            if (error)
     throw error;
        var __query4 = "select * from UserFiles\n                where UserId = " + user_id + "";
        __connection4.query(__query4, function(error, rows, cols) {
            if (error)
     throw error; files = rows; 
            __connection4.end();
            __callback6();
        });
        });
});
	};
	application.compress = function(request,response) 
	{
SessionManager.httpRequest(request, response, function (err, session) {
    if (err) 
        throw err;
        var _form = new require('formidable').IncomingForm();
        var post = {fields: [], field_values: {}, files: []};
            var upload_dir  = "./upload";
            _form.uploadDir = upload_dir;
        _form.on('error', function(err) 
        {
            throw err;
        }).on('field', function(field, value) 
        {
            post.fields.push([field, value]);
            post.field_values[field] = value;
        }).on('file', function(field, file) 
        {
            post.files.push(file);
        }).on('end', function() 
        {
            var file = {};
var link = {};
function return_function()
{
response.write("<!DOCTYPE html>\n<html>\n    <script>\n        function delayer(){\n        window.location = \"/\"\n        }\n    </script>\n    <body onLoad=\"setTimeout('delayer()', 3000)\">\n        You requested a compressed file with name: ");
response.write( "<b id=\"file\">" + file.inner_html + "</b>");response.write("<br>\n            Here is your ");
response.write("<a href = ");
response.write('\"' + link.href + '\"' );
response.write("id = \"link\">");
response.write("download link</a>\n            <br>\n            <a href=\"/\">Go back</a>\n    </body>\n</html>\n\n");
response.end();
}var logged = session.get("logged");
if (!logged)
{
application.login(request, response);
return true;
}
var uploaded_file = post.files[0].path;
function __callback7() 
{
var filename = post.files[0].name;
var url = "compressed/" + filename + ".gz";
function __callback8() 
{
file.inner_html = filename;
link.href = url;
var user_id = session.get("user_id");
function __callback9() 
{
return_function();
}
        var __connection5 = mysql.createConnection(application.compressor_db);
        __connection5.connect(function(error) {
            if (error)
     throw error;
        var __query5 = "insert into UserFiles (UserId, File, Url)\n        values (" + user_id + ", '" + filename + "', '" + url + "')";
        __connection5.query(__query5, function(error, rows, cols) {
            if (error)
     throw error;
            __connection5.end();
            __callback9();
        });
        });
}
fs.rename(uploaded_file + ".gz", "files/" + url, function(err)
{
	if (err)
		throw err;__callback8()
});
}
        var __cmd1 = "gzip " + uploaded_file + "";
        child_process.exec(__cmd1, {}, function(error, stdout, stderr){
            if (error)
     throw error;__callback7();
        });
        });
        _form.parse(request);    
});
	};
        application.login = function(request, response)
        {
            response.end("<!DOCTYPE html>\n<html>\n    <body>\n        <b>Please Login</b>\n        <br/>\n        <br/>\n        <form action=\"/do_login\" method=\"post\">\n            User: <input type=\"text\" name=\"user\"/> <br/>\n            Password: <input type=\"text\" name=\"password\"/> <br/>\n            <input type=\"submit\" value=\"Login\"/>\n        </form>\n        <br/>\n        <a href=\"/register\">Or Register</a>\n    </body>\n</html>\n\n");
        }
        application.register = function(request, response)
        {
            response.end("<!DOCTYPE html>\n<html>\n    <body>\n        <b>Please Register</b>\n        <br/>\n        <br/>\n        <form action=\"/do_register\" method=\"post\">\n            User: <input type=\"text\" name=\"user\"/> <br/>\n            Password: <input type=\"text\" name=\"password\"/> <br/>\n            Location: <input type=\"text\" name=\"location\" width=\"100\"/>\n            <input type=\"submit\" value=\"Join\"/>\n        </form>\n    </body>\n</html>\n\n");
        }
application.init = function()
{
            var static = require('node-static');
            this.file_server = new(static.Server)('/root/nodejs/samples/files/', { cache: 7200 });
        }
application.init();
http.createServer(function(request, response) 
{
    var pathname = url.parse(request.url).pathname;
    if (pathname == '/'){application.home(request, response);} else if (pathname == '/login'){application.login(request, response);} else if (pathname == '/do_login'){application.do_login(request, response);} else if (pathname == '/logout'){application.logout(request, response);} else if (pathname == '/register'){application.register(request, response);} else if (pathname == '/do_register'){application.do_register(request, response);} else if (pathname == '/file_list'){application.file_list(request, response);} else if (pathname == '/compress'){application.compress(request, response);}
            else
            {
                application.file_server.serve(request, response, function (err, res) {
                    if (err) { 
                        console.error("> Error serving " + request.url + " - " + err.message);
                        response.writeHead(err.status, err.headers);
                        response.end();
                    } 
                });            
            }
        }).listen(8888);
