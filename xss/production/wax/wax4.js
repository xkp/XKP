var http = require("http");
var url  = require("url");
var mysql  = require("mysql");
var child_process  = require("child_process");
var application = {};
application.service1 = function(response,request) 
{
    function return_function(return_value)
    {
        reqest.end(JSON.stringify(return_value))
    }
    
    var __params = url.parse(request.url, true).query;
    var file = __params.file;var data;
    function __callback1() 
    {
        var count = 0;
        function __callback2() 
        {
            return_function(true);
            return true;
        }
        
        var d_iterable = data;
        var d_iterator = 0;
        function __for_code1()
        {
	        var d = d_iterable[d_iterator];
            var curr_file = d.file;
            count += d.views;
            
            function __callback3() //shell
            {
                function __callback4() //sql
                {
                    function __callback5() 
                    {
                        __for_iter1();
                    }
                    
                    if (count > 5)
                    {
                        function __callback6() 
                        {
                            if (__callback5())
                                return true;
                            __for_iter1();
                        }
                        
                        function __while_code1()
                        {
                            function __callback7() 
                            {
                                count--;
                                if (__callback6())
                                    return true;
                                if (__callback5())
                                    return true;
                                __while_cond1();
                            }
                            
                            var __cmd1 = "git mark " + curr + "_file " + count + "";
                            child_process.exec(__cmd1, function(error, stdout, stderr)
                            {
                                __callback7();
                            });
                        }
                        
                        function __while_cond1()
                        {
	                        if (count > 0)
	                            __while_code1();
	                        else
	                            __callback6();
                        }
                        
                        __while_cond1();
                    }
                    else
                    {
                        var result = {};
                        result.desc = "Not enough views";
                        result.status = "suspended";
                        return_function(result);
                        return true;
                        
                        if (__callback5())
                            return true;
                        __for_iter1();
                    }
                }
                
                new mysql.Database({
                    hostname: 'localhost', 
                    user: 'user', 
                    password: 'password', 
                    database: 'test'
                }).connect(function(error) 
                {
                    var __query1 = "INSERT INTO Registry(file_name, visited)\n            VALUES              (" + file + ", 1)";
                    this.query(__query1).execute(function(error, rows, cols) 
                    {
                        __callback4();
                    });
                });
            }
            
            var __cmd2 = "git commit " + curr + "_file";
            child_process.exec(__cmd2, function(error, stdout, stderr)
            {
                __callback3();
            });
        }
        
        function __for_iter1()
        {
	        d_iterator++;
	        __for_cond1();
        }
        
        function __for_cond1()
        {
	        if (d_iterator < d_iterable.length)
	            __for_code1();
	        else
	            __callback2();
        }
        
        __for_cond1();
    }
    
    new mysql.Database({
        hostname: 'localhost', 
        user: 'user', 
        password: 'password', 
        database: 'test'
    }).connect(function(error) 
    {
        var __query2 = "SELECT  * \n                FROM    Registry\n                WHERE   file = " + file + " AND\n                        visited = 0";
        this.query(__query2).execute(function(error, rows, cols) 
        {
            data = rows; 
            __callback1();
        });
    });
};

http.createServer(function(request, response) 
{
    var pathname = url.parse(request.url).pathname;
    if (pathname == 'service1')
    {
        application.service1(request, response);
    }
}).listen(8888);
