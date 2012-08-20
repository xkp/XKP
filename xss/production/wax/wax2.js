var http = require("http");
var url  = require("url");
var fs = require('fs');
require("./smart.min.js");
        var mysql  = require("mysql");
        var child_process  = require("child_process");
var application = {};
            application.project_db = {hostname : "localhost",user : "root",password : "xs@2011",database : "NodeTest"};
	application.compile = function(request,response) 
	{
        var __params = url.parse(request.url, true).query;
            var project_id = __params.project_id;
function return_function(return_value)
{
response.end(JSON.stringify(return_value))
}var project_info;
function __callback1() 
{
if (!project_info)
{
return_function({success : false});
return true;
}
return_function({success : true,folder : project_info.LocalFolder});
return true;
}
        var __connection1 = mysql.createConnection(application.project_db);
        __connection1.connect(function(error) {
            if (error)
     throw error;
        var __query1 = "SELECT * FROM  Project\n                                WHERE Id = " + project_id + "";
        __connection1.query(__query1, function(error, rows, cols) {
            if (error)
     throw error;
                if (rows.length == 0)
                    project_info = null;
                else if (rows.length == 1)
                    project_info = rows[0];
                else
                    project_info = rows;
            __callback1();
        });
        });
	};
	application.compress_file = function(file,rename_to,return_function) 
	{
		function __callback2() 
{
var new_path = "./files/compressed/" + rename_to;
function __callback3() 
{
return_function({local_path : new_path,url : "compressed/" + rename_to});
return true;
return_function();
}
fs.rename(file + ".gz", new_path, function(err)
{
	if (err)
		throw err;__callback3()
});
}
        var __cmd1 = "gzip " + file + "";
        child_process.exec(__cmd1, {}, function(error, stdout, stderr){
            if (error)
     throw error;__callback2();
        });
    	};
	application.compress_service = function(request,response) 
	{
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
            var filename = post.field_values["filename"];
function return_function(return_value)
{
response.end(JSON.stringify(return_value))
}var uploaded_file = post.files[0].path;
var result;
function __callback4() 
{
return_function({success : true,url : result.url,local_path : result.local_path});
return true;
}
application.compress_file(uploaded_file, filename, function(return_value)
{result = return_value;
__callback4();
});
        });
        _form.parse(request);    
	};
	application.compress_page = function(request,response) 
	{
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
            var filename = post.field_values["filename"];
            var lblFile = {};
var lblLink = {};
function return_function()
{
response.write("<!DOCTYPE html>\n<html>\n    <body>\n        You requested a compressed file with name: ");
response.write( "<b id=\"lblFile\">" + lblFile.inner_html + "</b>");response.write("<br>\n            Here is your ");
response.write("<a href = ");
response.write('\"' + lblLink.href + '\"' );
response.write("id = \"lblLink\">");
response.write("download link</a>\n    </body>\n</html>\n\n");
response.end();
}var last = cookies.get("last_file");
lblFile.inner_html = filename;
var uploaded_file = post.files[0].path;
var result;
function __callback5() 
{
lblLink.href = result.url;
return_function();
}
application.compress_file(uploaded_file, filename, function(return_value)
{result = return_value;
__callback5();
});
        });
        _form.parse(request);    
	};
application.init = function()
{
            var static = require('node-static');
            this.file_server = new(static.Server)('/root/nodejs/samples/files/', { cache: 7200 });
        }
application.init();
http.createServer(function(request, response) 
{
    var pathname = url.parse(request.url).pathname;
    if (pathname == '/compile'){application.compile(request, response);} else if (pathname == '/compress_service'){application.compress_service(request, response);} else if (pathname == '/compress_page'){application.compress_page(request, response);}
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
