var http = require("http");
var url  = require("url");
var application = {};
	application.util1 = function(return_function) 
	{
		var c;
function __callback1() 
{
function __callback2() 
{
if (c < 20)
{return_function(c);
return true;
}else
{
c += 20;
bar_foo(function(return_value)
{
return_function(return_value);
});
}
}
foo_bar(function(return_value)
{
c += return_value;
__callback2();
});
}
foo_bar(function(return_value)
{
c = return_value;
__callback1();
});
	};
	application.service1 = function() 
	{
function return_function(return_value)
{
reqest.append(to_jason(return_value))
}function __callback3() 
{
var x = 10;
function __callback4() 
{
return_function(x);
return true;
}
bar(function(return_value)
{
__callback4();
});
}
bar(function(return_value)
{
if (return_value)
{
return_function(application.util1());
return true;
__callback3();
}else
{
bar_foo(function(return_value)
{
__callback3();
});
}
});	};
http.createServer(function(request, response) 
{
    var pathname = url.parse(request.url).pathname;
    if (pathname == service1){}}).listen(8888);
