var http = require("http");
var url  = require("url");
var application = {};
	application.util1 = function() 
	{
		var c;
function __callback1() 
{
function __callback2() 
{
if (c < 20)
{return_function(c);
return true;
__callback2()
}
else
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
c += return_value;__callback1()
});
}
foo_bar(function(return_value)
{
c = return_value;
});
	};
	application.service1 = function() 
	{
		function __callback3() 
{
var x = 10;
function __callback4() 
{
return_function(x);
return true;
}
bar(function(return_value)
{
__callback3()
});
}
bar(function(return_value)
{
	if (return_value)
	{
util1(function(return_value)
{
return_function(return_value);
});
	}
	else
{
bar_foo(function(return_value)
{
});
	}
});	};
http.createServer(function(request, response) 
{
    var pathname = url.parse(request.url).pathname;
    if (pathname == 'service1'){application.service1(request, response);}}).listen(8888);
