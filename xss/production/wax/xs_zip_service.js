var http = require("http");
var url = require("url");
var fs = require('fs');
var sessions = require("sessions");
require("./smart.min.js");
var application = {};
//init sessions
SessionManager = new sessions();
SessionManager.on("expired", function (sid)
{
    if (application.session_expired)
    {
        application.session_expired(sid);
    }
});
var child_process = require("child_process");
application.compress = function (request, response)
{
    var _form = new require('formidable').IncomingForm();
    var post = {
        fields: [],
        field_values: {},
        files: []
    };
    var upload_dir = "./upload";
    _form.uploadDir = upload_dir;
    _form.on('error', function (err)
    {
        throw err;
    }).on('field', function (field, value)
    {
        post.fields.push([field, value]);
        post.field_values[field] = value;
    }).on('file', function (field, file)
    {
        post.files.push(file);
    }).on('end', function ()
    {
        function return_function(return_value)
        {
            response.end(JSON.stringify(return_value))
        }
        var uploaded_file = post.files[0].path;

        function __callback1()
        {
            var filename = post.files[0].name;
            var url = "compressed/" + filename + ".gz";

            function __callback2()
            {
                return_function(
                {
                    success: true,
                    url: url
                });
                return true;
            }
            fs.rename(uploaded_file + ".gz", "files/" + url, function (err)
            {
                if (err) throw err;
                __callback2()
            });
        }
        var __cmd1 = "gzip " + uploaded_file + "";
        child_process.exec(__cmd1,
        {}, function (error, stdout, stderr)
        {
            if (error) throw error;
            __callback1();
        });
    });
    _form.parse(request);
};
application.home = function (request, response)
{
    response.end("<!DOCTYPE html>\n<html>\n    <body>\n        <form action=\"/compress\" enctype=\"multipart/form-data\" method=\"post\">\n            <p>\n                Please specify a file: <br>\n                <input type=\"file\" name=\"datafile\" size=\"40\">\n                <input type=\"submit\" value=\"Zip it\">\n            </p>\n        </form>    \n    </body>\n</html>\n\n");
}
application.init = function ()
{
    var static = require('node-static');
    this.file_server = new(static.Server)('/root/nodejs/samples/files/',
    {
        cache: 7200
    });
}
application.init();
http.createServer(function (request, response)
{
    var pathname = url.parse(request.url).pathname;
    if (pathname == '/')
    {
        application.home(request, response);
    }
    else if (pathname == '/compress')
    {
        application.compress(request, response);
    }
    else
    {
        application.file_server.serve(request, response, function (err, res)
        {
            if (err)
            {
                console.error("> Error serving " + request.url + " - " + err.message);
                response.writeHead(err.status, err.headers);
                response.end();
            }
        });
    }
}).listen(8888);