var http = require("http");
var url  = require("url");
        var mysql  = require("mysql");
var application = {};
	application.home = function(request,response) 
	{
		var user_name = {};
var followers = {};
function return_function()
{
response.write("<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n<html xmlns=\"http://www.w3.org/1999/xhtml\">\n<head>\n<meta http-equiv=\"Content-Type\" content=\"text/html; charset=iso-8859-2\" />\n<title>Twootter &raquo;</title>\n<link rel=\"stylesheet\" type=\"text/css\" href=\"style.css\" />\n</head>\n<body>\n\n	<!-- Begin Wrapper -->\n	\n		<div id=\"wrapper\">\n		\n		<!-- Begin Header -->\n			\n			<div id=\"header\">\n				<h1>Twootter</h1>\n				<h2> Dont even have to think... </h2>		 \n			</div>\n			\n		<!-- End Header -->\n			\n		<!-- Begin Strap -->\n		\n        <div id=\"strap\">\n            Twoottering: &raquo; ");
response.write("<b text = ");
response.write('\"' + user_name.text + '\"' );
response.write("id = \"user_name\"");
response.write(">User</b>\n        </div>\n		\n		<!-- End Strap -->\n		 \n		<!-- Begin Navigation -->\n		\n		<div id=\"navigation\">\n			<ul id=\"menu\">\n				<li><a href=\"#\">Home</a></li>\n				<li><a href=\"#\">Profile</a></li>\n			</ul>\n			<p>Followers:</p>\n			");
for(var followers_iterator = 0; followers_iterator < followers.data.lenght; followers_iterator++)
{
var item = followers.data[followers_iterator];
var follower = {};
follower.inner_html = item.name;
response.write("<li>");
response.write( "<a id=\"follower\" href=\"#\">" + follower.inner_html + "</a>");response.write(">\n			</ul");
}response.write(">\n			    <br />\n				    <p id=\"pquote\"> Lorem ipsum dolor sit amet, consectetuer adipiscing elit. \n				    Aenean pharetra sollicitudin diam. Mauris auctor elementum mi. Vivamus lobortis diam eu sapien commodo porttitor. Mauris non \n				    ligula. Integer scelerisque sem at magna. nibh felis, faucibus a, congue vel, tempus ut, arcu. Cras \n				    pretium tempor tortor.Donec suscipit. Duis nec enim. Cras vel \n				    libero. Sed vulputate feugiat velit. Pellentesque sed magna. \n				    Vestibulum ante ipsum primis in faucibus orci luctus et ultrices \n				    posuere cubilia Curae; Fusce eget mi id est cursus placerat. \n				    Cras in ante. Praesent imperdiet neque in nibh. Sed ac enim.\n				    </p>\n				    <p>By: <b id=\"pquote_author\"> Author </b> </p> \n			</div>\n		</div>\n		\n		<!-- End Content -->\n		\n		<!-- Start Footer -->\n		\n		<div id=\"footer\">\n			<p>&copy; 2009 Twootter.All right Reserved.</p>\n		</div>\n		\n		<!-- End Footer -->\n		 \n    </div>\n	\n    <!-- End Wrapper -->\n	\n</body>\n</html>\n");
}user_name.text = user.id;
user_image.src = user.image_url;
var data = [];
function __callback1() 
{
followers.data = data;
return_function();
}
        new mysql.Database({
            hostname: 'localhost', 
            user: 'user', 
            password: 'password', 
            database: 'test'
        }).connect(function(error) {
        var __query1 = "SELECT * FROM FOLLOWERS\n               WHERE USERID = " + user + ".id";
        this.query(__query1).execute(function(error, rows, cols) {
     data = rows; __callback1();
        });
        });
    	};
http.createServer(function(request, response) 
{
    var pathname = url.parse(request.url).pathname;
    if (pathname == 'home'){application.home(request, response);}}).listen(8888);
