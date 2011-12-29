var http = require("http");
var url = require("url");
var mysql = require('db-mysql'); //<--------
var application = {};
application.service1 = function () {
    function return_function(return_value) {
        reqest.append(to_jason(return_value))
    }
    var data;
    new mysql.Database({
        hostname: 'localhost',
        user: 'user',
        password: 'password',
        database: 'test'
    }).connect(function (error) {
        this.query("SELECT  * \n                FROM    DataTable\n                WHERE   Column > 20").execute(function (error, rows, cols) { //<----------------- break
            data = rows;
            __callback1();
        });
    });
};
http.createServer(function (request, response) {
    var pathname = url.parse(request.url).pathname;
    console.log(pathname);
    if (pathname == '/service1') //<------------- 
    {
        console.log("PROCESSING");
        application.service1(request, response);
    }
}).listen(8888); 