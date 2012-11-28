var http = require("http");
var url  = require("url");
var fs = require('fs');
var sessions = require("sessions");
require("./smart.min.js");
var application = {};
//init sessions
SessionManager = new sessions();
SessionManager.on("expired", function(sid){
    if (application.session_expired)
    {
        application.session_expired(sid);
    }
});
        var child_process  = require("child_process");
            application.name = null;
	application.edit_page = function(request,response) 
	{
        var __params = url.parse(request.url, true).query;
        var initial_script = {};
function return_function()
{
response.write("<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n<html xmlns=\"http://www.w3.org/1999/xhtml\">\n<head>\n<title>BlendXS</title>\n<meta http-equiv=\"Content-Language\" content=\"English\" />\n<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\" />\n<link rel=\"stylesheet\" type=\"text/css\" href=\"blend_xs.css\" media=\"screen\" />\n</head>\n<body>\n<script type=\"text/javascript\" src=\"../js/XMLWriter.js\"></script>\n");
response.write( "<script id=\"initial_script\" type=\"text/javascript\">" + initial_script.inner_html + "</script>");response.write("\n<div id=\"wrap\">\n\n<div id=\"header\">\n<h1><a href=\"#\">BlendXS</a></h1>\n<h2>A new way to process your 3d models</h2>\n</div>\n\n\n<div class=\"left\"> \n	<div id=\"left-container\">\n\n\n				<canvas id=\"app\"\n			>\n				</canvas>\n			\n\n\n	</div>\n</div>\n\n<div class=\"right\"> \n	<div id=\"right-container\">\n\n\n\n<div id=\"__div1\">\n\n\n\n\n<div id=\"acc1\">\n\n\n            <h3><a href=\"#\">Translate</a></h3>\n		\n\n\n\n<div id=\"__div2\">\n\n\n\n				<label id=\"lbl_xt\"\n			>\n					X: \n				\n				</label>\n			\n\n\n\n				<input id=\"edit_xt\"\n			\n						type=\"text\" \n					\n						name=\"edit_xt\" \n					\n						value=\"0\" \n					>\n				</input>\n			\n\n\n<br/>\n\n\n<br/>\n\n\n\n				<label id=\"lbl_yt\"\n			>\n					Y: \n				\n				</label>\n			\n\n\n\n				<input id=\"edit_yt\"\n			\n						type=\"text\" \n					\n						name=\"edit_yt\" \n					\n						value=\"0\" \n					>\n				</input>\n			\n\n\n<br/>\n\n\n<br/>\n\n\n\n				<label id=\"lbl_zt\"\n			>\n					X: \n				\n				</label>\n			\n\n\n\n				<input id=\"edit_zt\"\n			\n						type=\"text\" \n					\n						name=\"edit_zt\" \n					\n						value=\"0\" \n					>\n				</input>\n			\n\n\n<br/>\n\n\n<br/>\n</div>\n\n\n\n            <h3><a href=\"#\">Rotate</a></h3>\n		\n\n\n\n<div id=\"__div3\">\n\n\n\n				<label id=\"lbl_xr\"\n			>\n					X: \n				\n				</label>\n			\n\n\n\n				<input id=\"edit_xr\"\n			\n						type=\"text\" \n					\n						name=\"edit_xr\" \n					\n						value=\"0\" \n					>\n				</input>\n			\n\n\n<br/>\n\n\n<br/>\n\n\n\n				<label id=\"lbl_yr\"\n			>\n					Y: \n				\n				</label>\n			\n\n\n\n				<input id=\"edit_yr\"\n			\n						type=\"text\" \n					\n						name=\"edit_yr\" \n					\n						value=\"0\" \n					>\n				</input>\n			\n\n\n<br/>\n\n\n<br/>\n\n\n\n				<label id=\"lbl_zr\"\n			>\n					Z: \n				\n				</label>\n			\n\n\n\n				<input id=\"edit_zr\"\n			\n						type=\"text\" \n					\n						name=\"edit_zr\" \n					\n						value=\"0\" \n					>\n				</input>\n			\n\n\n<br/>\n\n\n<br/>\n</div>\n\n\n\n            <h3><a href=\"#\">Scale</a></h3>\n		\n\n\n\n<div id=\"__div4\">\n\n\n\n				<label id=\"lbl_xs\"\n			>\n					X: \n				\n				</label>\n			\n\n\n\n				<input id=\"edit_xs\"\n			\n						type=\"text\" \n					\n						name=\"edit_xs\" \n					\n						value=\"1\" \n					>\n				</input>\n			\n\n\n<br/>\n\n\n<br/>\n\n\n\n				<label id=\"lbl_ys\"\n			>\n					Y: \n				\n				</label>\n			\n\n\n\n				<input id=\"edit_ys\"\n			\n						type=\"text\" \n					\n						name=\"edit_ys\" \n					\n						value=\"1\" \n					>\n				</input>\n			\n\n\n<br/>\n\n\n<br/>\n\n\n\n				<label id=\"lbl_zs\"\n			>\n					Z: \n				\n				</label>\n			\n\n\n\n				<input id=\"edit_zs\"\n			\n						type=\"text\" \n					\n						name=\"edit_zs\" \n					\n						value=\"1\" \n					>\n				</input>\n			\n\n\n<br/>\n\n\n<br/>\n</div>\n\n\n</div>\n\n\n</div>\n\n\n\n\n	</div>\n</div>\n<div style=\"clear: both;\"> </div>\n\n\n\n<div id=\"footer\">\nSupported by <a href=\"http://www.codexcess.com/\"> XS Programming Base </a>, Thanks to <a href=\"http://www.codexcess.com/about\">The XS Team</a>\n</div>\n</div>\n<form action=\"/apply\" enctype=\"multipart/form-data\" method=\"post\">\n	<p>		\n		<input type=\"submit\" value=\"Apply\" size=\"40\">\n	</p>\n</form>\n\n\n            <script type=\"text/javascript\" src=\"../js/resig.js\"></script>\n        \n\n\n\n            <script type=\"text/javascript\" src=\"../js/event.js\"></script>\n        \n\n\n\n            <script type=\"text/javascript\" src=\"../js/keys.js\"></script>\n        \n\n\n\n            <script type=\"text/javascript\" src=\"../js/colors.js\"></script>\n        \n\n\n\n            <script type=\"text/javascript\" src=\"../js/jquery-1.7.1.min.js\"></script>\n        \n\n\n\n            <script type=\"text/javascript\" src=\"../js/jquery-ui-1.8.18.custom.min.js\"></script>\n        \n\n\n\n            <script type=\"text/javascript\" src=\"../js/jquery-idiom.js\"></script>\n        \n\n\n\n            <link rel=\"stylesheet\" href=\"../css/ui-lightness/jquery-ui-1.8.18.custom.css\"></link>\n        \n\n\n\n            <script type=\"text/javascript\" src=\"../js/streamer.js\"></script>\n        \n\n\n\n            <script type=\"text/javascript\" src=\"../js/three/Three.js\"></script>\n        \n\n\n\n            <script type=\"text/javascript\" src=\"../js/three/Stats.js\"></script>\n        \n\n\n\n            <script type=\"text/javascript\" src=\"../js/three/Utils.js\"></script>\n        \n\n\n\n            <script type=\"text/javascript\" src=\"../js/three/Detector.js\"></script>\n        \n\n\n\n            <script type=\"text/javascript\" src=\"../js/three/loaders/ColladaLoader.js\"></script>\n        \n\n<style>\n\n\n#acc1 {height : 360px}\n\n\n#edit_xt {width : 100px}\n\n\n#edit_yt {width : 100px}\n\n\n#edit_zt {width : 100px}\n\n\n#edit_xr {width : 100px}\n\n\n#edit_yr {width : 100px}\n\n\n#edit_zr {width : 100px}\n\n\n#edit_xs {width : 100px}\n\n\n#edit_ys {width : 100px}\n\n\n#edit_zs {width : 100px}\n\n\n#app {width : 500px;height : 300px}\n\n</style>\n<script type=\"text/javascript\">\n\n        var application = {};\n    \n			var __div1 = $(\"#__div1\");\n		\n			var acc1 = $(\"#acc1\");\n		\n			var __div2 = $(\"#__div2\");\n		\n			var lbl_xt = $(\"#lbl_xt\");\n		\n			var edit_xt = $(\"#edit_xt\");\n		\n			var __br1 = $(\"#__br1\");\n		\n			var __br2 = $(\"#__br2\");\n		\n			var lbl_yt = $(\"#lbl_yt\");\n		\n			var edit_yt = $(\"#edit_yt\");\n		\n			var __br3 = $(\"#__br3\");\n		\n			var __br4 = $(\"#__br4\");\n		\n			var lbl_zt = $(\"#lbl_zt\");\n		\n			var edit_zt = $(\"#edit_zt\");\n		\n			var __br5 = $(\"#__br5\");\n		\n			var __br6 = $(\"#__br6\");\n		\n			var __div3 = $(\"#__div3\");\n		\n			var lbl_xr = $(\"#lbl_xr\");\n		\n			var edit_xr = $(\"#edit_xr\");\n		\n			var __br7 = $(\"#__br7\");\n		\n			var __br8 = $(\"#__br8\");\n		\n			var lbl_yr = $(\"#lbl_yr\");\n		\n			var edit_yr = $(\"#edit_yr\");\n		\n			var __br9 = $(\"#__br9\");\n		\n			var __br10 = $(\"#__br10\");\n		\n			var lbl_zr = $(\"#lbl_zr\");\n		\n			var edit_zr = $(\"#edit_zr\");\n		\n			var __br11 = $(\"#__br11\");\n		\n			var __br12 = $(\"#__br12\");\n		\n			var __div4 = $(\"#__div4\");\n		\n			var lbl_xs = $(\"#lbl_xs\");\n		\n			var edit_xs = $(\"#edit_xs\");\n		\n			var __br13 = $(\"#__br13\");\n		\n			var __br14 = $(\"#__br14\");\n		\n			var lbl_ys = $(\"#lbl_ys\");\n		\n			var edit_ys = $(\"#edit_ys\");\n		\n			var __br15 = $(\"#__br15\");\n		\n			var __br16 = $(\"#__br16\");\n		\n			var lbl_zs = $(\"#lbl_zs\");\n		\n			var edit_zs = $(\"#edit_zs\");\n		\n			var __br17 = $(\"#__br17\");\n		\n			var __br18 = $(\"#__br18\");\n		\n			var app = $(\"#app\");\n		\n\n\n\n\n\n\n\n\n\n\n\n            acc1.accordion({\n        });\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n            app.application = (function (host)\n{\n    \nif ( ! Detector.webgl ) Detector.addGetWebGLMessage();\n			\n			var canvasElement = document.getElementById(\"app\");	\n			var renderer = new THREE.WebGLRenderer({canvas:canvasElement, antialias: false});\n			renderer.domElement.width = renderer.domElement.offsetWidth;\n			renderer.domElement.height = renderer.domElement.offsetHeight;\n		var ui_;\n\nvar canvas_position = {x:0, y:0};\nvar g_elapsed;\nvar g_delta;	\nvar scene = new THREE.Scene();\nscene.mouse_thru = true;\nvar streamer = {};\nvar manager = new threejs.Manager(scene, renderer, null, canvas_position);\nvar three_utils = new threejs.Utils();\nmanager.events = new ev.EventHolder();\nmanager.events.parent = manager;\nmanager.c_anims = [];\nmanager.js_anims = [];\nmanager.c_skins = [];\nvar app_width = canvasElement.width;\nvar app_height = canvasElement.height;\n\nvar application = {};\nvar clock = new THREE.Clock();\napplication.events = new ev.EventHolder();\n\n\n		var streamer = new stream.Streamer();		\n		var ResourceUtils = new stream.ResourceUtils();\n	streamer.manager = manager;\n// --------RESOURCES---------\n        \n		var global_package = new stream.Package(streamer);	\n    \n                global_package.add_item(\n            \n\n\n		{\n		id:				\"res_model\",\n		resource_type:	RESOURCE_COLLADA_MODEL,\n		src:			\"null\",\n		runtime_src:	dynamic_src,\n		frame_width:	null,\n		frame_height:	null,\n		animations:		[],	\n		children:		{}\n		}\n	\n);	\n				var res_model;\n				global_package.events.addListener(\"loaded\", function()\n				{\n					res_model = streamer.get_resource(\"res_model\");\n				});\n			\n\n\n	\n		global_package.add_item({\n			id:				\"invalid_res\",\n			resource_type:	RESOURCE_IMAGE,\n			src:			\"images/no_res.png\",\n			frame_width:	null,\n			frame_height:	null,\n			animations:		[]\n		});		\n		global_package.load();	\n    // --------RESOURCES---------\n\nrenderer.setSize( app_width, app_height );\nmanager.streamer = streamer;\nfunction render() {\nmanager.canvas_position = getElementPosition(renderer.domElement);\nvar now   = new Date().getTime();\nif (g_elapsed < 0)\n	g_delta = 0; //first update\nelse\n	g_delta  = now - g_elapsed;  \n\ng_elapsed = now; \n\nif(!g_delta) g_delta = 0;\nmanager.delta = g_delta;\nc_delta = clock.getDelta(),\nc_elapsed = clock.getElapsedTime() * 10;\n\n\nmanager.events.dispatch(\"update\", [g_delta, g_elapsed]);	\n\n				manager.scene.add(camera_1);\n				set_active_camera(camera_1);\n				\napplication.events.dispatch(\"switch_cameras\", []);\n\n}\n\nvar camera_1;var a_ligth;var d_ligth_1;var model1;\n\n\nthreejs_load_resources(manager, function()\n{\n\n\n\n\n\n\n		    camera_1 = \n		    new THREE.PerspectiveCamera(75, 1.5, 0.100000001, 1000, null) ;	\n			manager.active_camera = camera_1;\n		\n\n\n			camera_1.manager = manager;\n		\n\n\n\n\n\n\n\n	    a_ligth = \n	    new THREE.AmbientLight(ivory) ;\n	    manager.scene.add(a_ligth);\n	\n\n			a_ligth.manager = manager;\n		\n\n\n\n\n\n\n\n	    d_ligth_1 = \n	    new THREE.DirectionalLight(White, 2, 0, null) ;\n	    manager.scene.add(d_ligth_1);\n	\n\n			d_ligth_1.manager = manager;\n		\n\n\n\n\n\n			\n		model1 = clone3jsObject( manager.streamer.get_resource('res_model').data, manager );	\n		manager.scene.add(model1);\n		model1.events = new ev.EventHolder();			\n	\n\n			model1.manager = manager;\n		\n\n\n\n\n\n\n\n\n\n\n\n\n\n            manager.events.addListener('update', function(){if(camera_1)camera_1.lookAt(scene.position)});\n        \n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n            camera_1.position.y = 50;\n        \n\n\n\n\n\n\n            camera_1.position.z = 500;\n        \n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n            a_ligth.position.y = 5000;\n        \n\n\n\n\n\n\n            a_ligth.position.z = 5000;\n        \n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n            d_ligth_1.position.y = 5000;\n        \n\n\n\n\n\n\n            d_ligth_1.position.z = 5000;\n        \n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n            model1.scale.x = 20;\n        \n\n\n\n\n\n\n            model1.scale.y = 20;\n        \n\n\n\n\n\n\n            model1.scale.z = 20;\n        \n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n            set_rotation_x(model1.rotation, -90);\n        \n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n            model1.position.y = -50;\n        \n\n\n\n\n\n\n\n\n\n\n\nvar event_manager = new ev.ThreeEventManager();\n\ndocument.onmousemove = function(ev)\n{\n	event_manager.mousemove(manager, ev.clientX, ev.clientY);                  \n};\ndocument.onmousedown = function(ev)\n{\n	event_manager.mousedown(manager, ev.clientX, ev.clientY);                  \n};\ndocument.onmouseup = function(ev)\n{\n	event_manager.mouseup(manager, ev.clientX, ev.clientY);		\n};\ndocument.onkeydown = function(ev)\n{\n	event_manager.keydown(manager, ev.keyCode);                  \n};\ndocument.onkeyup = function(ev)\n{\n	event_manager.keyup(manager, ev.keyCode);                  \n};\ndocument.onkeypress = function(ev)\n{\n	event_manager.keypress(manager, ev.keyCode);                  \n};\n\nmanager.events.addListener(\"mousedown\", function(x,y)\n{    \n	application.events.dispatch(\"mousedown\", [x, y]);\n});\nmanager.events.addListener(\"mouseup\", function(x,y)\n{    \n	application.events.dispatch(\"mouseup\", [x, y]);\n});\nmanager.events.addListener(\"mousemove\", function(x,y)\n{    \n	application.events.dispatch(\"mousemove\", [x, y]);\n});\nmanager.events.addListener(\"click\", function(x,y)\n{    \n	application.events.dispatch(\"click\", [x, y]);\n});\nmanager.events.addListener(\"drag\", function(x,y)\n{    \n	application.events.dispatch(\"drag\", [x, y]);\n});\nmanager.events.addListener(\"dragend\", function(x,y)\n{    \n	application.events.dispatch(\"dragend\", [x, y]);\n});\nmanager.events.addListener(\"keyup\", function(keycode)\n{    \n	application.events.dispatch(\"keyup\", [keycode]);\n});\nmanager.events.addListener(\"keydown\", function(keycode)\n{    \n	application.events.dispatch(\"keydown\", [keycode]);\n});\nmanager.events.addListener(\"keypress\", function(keycode)\n{    \n	application.events.dispatch(\"keydown\", [keycode]);\n});\nmanager.events.addListener(\"update\", function(delta, elapsed)\n{    \n	application.events.dispatch(\"update\", [delta, elapsed]);\n});\napplication.events.dispatch(\"init\", []);\n	\n\nanimate();\n});\nfunction animate() {	\n	requestAnimationFrame( animate );\n	render();	\n}\n})(application);\n\n\n\n\n        \n\n\n\n\n\n\n\n\n                    if (application.init)\n                        application.init();\n                \n</script>\n</body>\n</html>\n");
response.end();
}initial_script.inner_html = "var dynamic_src = 'upload/" + application.name + ".dae';";
return_function();
	};
	application.blend = function(request,response) 
	{
        var _form = new require('formidable').IncomingForm();
        var post = {fields: [], field_values: {}, files: []};
            var upload_dir  = "D:/Pabli/Trabajo/Excess/Projects/file_server/upload";
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
function return_function()
{
response.write("");
response.end();
}application.name = post.files[0].name;
var url = "D:/Pabli/Trabajo/Excess/Projects/file_server/upload/" + post.files[0].name;
function __callback1() 
{
console.log(url);
var output;
function __callback2() 
{
console.log(output);
application.edit_page(request, response);
return true;
return_function();
}
        var __cmd1 = "blender -b -P load.py -- " + url + "";
        child_process.exec(__cmd1, {}, function(error, stdout, stderr){
            if (error)
     throw error; 
            output = {output: null, error: null};
            if (stdout.length > 0)
                output.output = stdout.split("\n"); 
            if (stderr.length > 0)
                output.error = stderr.split("\n"); 
        __callback2();
        });
}
fs.rename(post.files[0].path, url, function(err)
{
	if (err)
		throw err;__callback1()
});
        });
        _form.parse(request);    
	};
        application.home = function(request, response)
        {
            response.end("<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n<html xmlns=\"http://www.w3.org/1999/xhtml\">\n<head>\n<title>BlendXS</title>\n<meta http-equiv=\"Content-Language\" content=\"English\" />\n<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\" />\n<link rel=\"stylesheet\" type=\"text/css\" href=\"blend_xs.css\" media=\"screen\" />\n</head>\n<body>\n<div id=\"wrap\">\n\n<div id=\"header\">\n<h1><a href=\"#\">BlendXS</a></h1>\n<h2>A new way to process your 3d models</h2>\n</div>\n\n<form action=\"/blend\" enctype=\"multipart/form-data\" method=\"post\">\n	<p>\n		Please specify a file: <br>\n		<input type=\"file\" name=\"datafile\" size=\"200\">\n		<input type=\"submit\" value=\"Go!!!\" size=\"40\">\n	</p>\n</form>\n\n<div id=\"footer\">\nSupported by <a href=\"http://www.codexcess.com/\"> XS Programming Base </a>, Thanks to <a href=\"http://www.codexcess.com/about\">The XS Team</a>\n</div>\n</div>\n\n</body>\n</html>\n");
        }
application.init = function()
{
            var static = require('node-static');
            this.file_server = new(static.Server)('D:/Pabli/Trabajo/Excess/Projects/file_server', { cache: 7200 });
        }
application.init();
http.createServer(function(request, response) 
{
    var pathname = url.parse(request.url).pathname;
    if (pathname == '/'){application.home(request, response);} else if (pathname == '/edit_page'){application.edit_page(request, response);} else if (pathname == '/blend'){application.blend(request, response);}
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
