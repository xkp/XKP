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
        var child_process  = require("child_process");
	application.compress_file = function(file,rename_to,return_function) 
	{
		function __callback1() 
{
var new_path = "./files/compressed/" + rename_to;
function __callback2() 
{
return_function({local_path : new_path,url : "compressed/" + rename_to});
return true;
return_function();
}
fs.rename(file + ".gz", new_path, function(err)
{
	if (err)
		throw err;__callback2()
});
}
        var __cmd1 = "gzip " + file + "";
        child_process.exec(__cmd1, {}, function(error, stdout, stderr){
            if (error)
     throw error;__callback1();
        });
    	};
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
function return_function()
{
response.write("");
response.end();
}session.set("user", user);
session.set("logged", true);
application.file_list(request, response);
return true;
return_function();
        });
        _form.parse(request);    
});
	};
	application.file_list = function(request,response) 
	{
SessionManager.httpRequest(request, response, function (err, session) {
    if (err) 
        throw err;
        var __params = url.parse(request.url, true).query;
        var user_name = {};
var files = {};
function return_function()
{
response.write("<!DOCTYPE html>\n<html>\n    <body>\n        <b>Welcome: </b>\n        ");
response.write( "<b id=\"user_name\">" + user_name.inner_html + "</b>");response.write("\n        <br/>\n        <br/>\n        \n        <h3>Compress a file</h3>\n        <form action=\"/compress\" enctype=\"multipart/form-data\" method=\"post\">\n            <p>\n                    <input type=\"file\" name=\"datafile\" size=\"40\">\n                     <input type=\"submit\" value=\"Compress\">\n            </p>\n            </form>\n\n        <h3>Or select one already compressed in the session</h3>\n        ");
response.write("<ul id=\"files\">");
for(var files_iterator = 0; files_iterator < files.data.length; files_iterator++)
{
var item = files.data[files_iterator];
var file_name = {};
var link = {};
file_name.inner_html = item.file;
link.href = item.url;
response.write("<li>\n                ");
response.write( "<b id=\"file_name\">" + file_name.inner_html + "</b>");response.write(": ");
response.write("<a href = ");
response.write('\"' + link.href + '\"' );
response.write("id = \"link\">");
response.write("Download link</a> \n            </li>");
}
response.write("</ul>");
response.write("\n    </body>\n</html>\n\n");
response.end();
}var logged = session.get("logged");
if (!logged)
{
application.login(request, response);
return true;
}
var history = session.get("history") || [];
user_name.inner_html = session.get("user");
files.data = history;
return_function();
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
var filename = post.files[0].name + ".gz";
var uploaded_file = post.files[0].path;
var result;
function __callback3() 
{
file.inner_html = filename;
link.href = result.url;
result.file = filename;
var history = session.get("history") || [];
history.push(result);
session.set("history", history);
return_function();
}
application.compress_file(uploaded_file, filename, function(return_value)
{result = return_value;
__callback3();
});
        });
        _form.parse(request);    
});
	};
        application.login = function(request, response)
        {
            response.end("<!DOCTYPE html>\n<html>\n    <body>\n        <b>Please Login</b>\n        <br/>\n        <br/>\n        <form action=\"/do_login\" method=\"post\">\n            User: <input type=\"text\" name=\"user\"/>\n            <input type=\"submit\" value=\"Join\"/>\n        </form>\n    </body>\n</html>\n\n");
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
    if (pathname == '/'){application.home(request, response);} else if (pathname == '/login'){application.login(request, response);} else if (pathname == '/do_login'){application.do_login(request, response);} else if (pathname == '/file_list'){application.file_list(request, response);} else if (pathname == '/compress'){application.compress(request, response);}
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
