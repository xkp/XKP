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
}
else
{
c += 20;
bar_foo(function()
{
{
return_function(return_value);
});
});
}
}
foo_bar(function()
{c += return_value;__callback2()
});
}
foo_bar(function()
{c = return_value;
__callback1();
});	};
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
bar(function()
{__callback4()
});
}
bar(function()
{
	if (return_value)
	{
util1(function()
{
{
return_function(return_value);
});
});
	}
	else
{
bar_foo(function()
{()
});
	}
});	};
http.createServer(function(request, response) 
{
    var pathname = url.parse(request.url).pathname;
    if (pathname == 'service1'){application.service1(request, response);}}).listen(8888);
