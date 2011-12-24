var http = require("http");
var url  = require("url");
var application = {};
	application.home = function(request,response) 
	{
function return_function()
{
response.write(">\n			    <br />\n				    <p id=\"pquote\"> Lorem ipsum dolor sit amet, consectetuer adipiscing elit. \n				    Aenean pharetra sollicitudin diam. Mauris auctor elementum mi. Vivamus lobortis diam eu sapien commodo porttitor. Mauris non \n				    ligula. Integer scelerisque sem at magna. nibh felis, faucibus a, congue vel, tempus ut, arcu. Cras \n				    pretium tempor tortor.Donec suscipit. Duis nec enim. Cras vel \n				    libero. Sed vulputate feugiat velit. Pellentesque sed magna. \n				    Vestibulum ante ipsum primis in faucibus orci luctus et ultrices \n				    posuere cubilia Curae; Fusce eget mi id est cursus placerat. \n				    Cras in ante. Praesent imperdiet neque in nibh. Sed ac enim.\n				    </p>\n				    <p>By: <b id=\"pquote_author\"> Author </b> </p> \n			</div>\n		</div>\n		\n		<!-- End Content -->\n		\n		<!-- Start Footer -->\n		\n		<div id=\"footer\">\n			<p>&copy; 2009 Twootter.All right Reserved.</p>\n		</div>\n		\n		<!-- End Footer -->\n		 \n    </div>\n	\n    <!-- End Wrapper -->\n	\n</body>\n</html>\n");
}user_name.text = "somethng dynamic";
return_function();	};
http.createServer(function(request, response) 
{
    var pathname = url.parse(request.url).pathname;
    if (pathname == 'home'){application.home(request, response);}}).listen(8888);
