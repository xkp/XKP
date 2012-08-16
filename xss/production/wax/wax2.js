var http = require("http");
var url  = require("url");
require("./smart.min.js");
        var mysql  = require("mysql");
        var child_process  = require("child_process");
var application = {};
            application.project_db = {hostname : "localhost",user : "root",password : "xs@2011",database : "NodeTest"};
	application.compile = function(request,response) 
	{
function return_function(return_value)
{
response.end(JSON.stringify(return_value))
}
var __params = url.parse(request.url, true).query;
var project_id = __params.project_id;var project_info;
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
	application.list_projects = function(request,response) 
	{
function return_function(return_value)
{
response.end(JSON.stringify(return_value))
}
var __params = url.parse(request.url, true).query;var result;
function __callback2() 
{
if (result.output)
{
return_function(result.output);
return true;
}
else
{
return_function({error : true,error_text : result.error});
return true;
}
}
        var __cmd1 = "ls /root/nod";
        child_process.exec(__cmd1, {cwd : "/root/nodejs/samples/"}, function(error, stdout, stderr){
            if (error)
     throw error; 
            result = {output: null, error: null};
            if (stdout.length > 0)
                result.output = stdout.split("\n"); 
            if (stderr.length > 0)
                result.error = stderr.split("\n"); 
        __callback2();
        });
    	};
application.init = function()
{
    }
application.init();
http.createServer(function(request, response) 
{
    var pathname = url.parse(request.url).pathname;
    if (pathname == '/compile'){application.compile(request, response);}if (pathname == '/list_projects'){application.list_projects(request, response);}}).listen(8888);
