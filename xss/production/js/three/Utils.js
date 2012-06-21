var threejs = {};
threejs.Manager = Class.extend(
{
	init: function(scene, renderer, streamer, canvas_position)
	{			
		this.scene = scene;
		this.renderer = renderer;
		this.streamer = streamer;
		this.canvas_position = canvas_position;
	},		
});

function getElementPosition(element) {
	var elem=element, tagname="", x=0, y=0;
	while(elem && (typeof(elem) == "object") && (typeof(elem.tagName) != "undefined")) {
		y += elem.offsetTop;
		x += elem.offsetLeft;
		tagname = elem.tagName.toUpperCase();
		if(tagname == "BODY")
			elem=0;
		if(typeof(elem) == "object") {
			if(typeof(elem.offsetParent) == "object")
				elem = elem.offsetParent;
		}
	}
	return {x: x, y: y};
}

function distance3d(vector1, vector2)
{
	if(vector1 && vector2)
	{
		var x = vector1.x - vector2.x;
		var y = vector1.y - vector2.y;
		var z = vector1.z - vector2.z;
		return Math.sqrt(x * x + y * y + z * z);
	}
	else return -1;
}

function set_active_camera( camera )
{
	camera.manager.scene.add(camera);
	camera.manager.renderer.render(camera.manager.scene, camera);	
	active_camera = camera;
}

function threejs_load_resources(manager, callback)
{		
	var resources = [];
	if(manager.streamer)
		for(var i = 0; i < manager.streamer.resources.length; i++)
		{
			var res = manager.streamer.resources[i];
			if(!res.loaded)
				resources.push({ type: res.type, url: res.asset});			
		}
	if (resources.length > 0)
	{		
		var stream_client =
		{
			resource_loaded: function(res, data)
			{					
			},

			finished_loading: function()
			{
				callback();
			}
		};

		manager.streamer.request(resources, stream_client);
	}
	else callback();
}

function set_transform_image( path, value )
{
	var resource = path.manager.streamer.get_resource(value);
	if(!resource)
		resource = path.manager.streamer.get_resource("invalid_res");
	for(var i = 0; i < path.children.length; i++)
	{
		var child = path.children[i];	
		child.material.map = new THREE.Texture(resource.data);	
	}	
}

function set_3js_object_texture( path, value )
{
	var resource = path.manager.streamer.get_resource(value);
	if(!resource)
		resource = path.manager.streamer.get_resource("invalid_res");
	path.material.map = new THREE.Texture(resource.data);
	path.material.map.needsUpdate = true;
}

function set_webgl_object_texture( path, value )
{
	var resource = path.manager.streamer.get_resource(value);
	if(!resource)
		resource = path.manager.streamer.get_resource("invalid_res");
	path.material.map = THREE.ImageUtils.loadTexture(resource.asset);
	path.material.map.needsUpdate = true;
}

function set_3js_texture_object( path, value )
{
	path.material.map = new THREE.Texture(value);
	path.material.map.needsUpdate = true;
}

function set_cube_face_object( path, face, value )
{
	path.geometry.faces[face].material.map = new THREE.Texture(value);	
	path.geometry.faces[face].material.map.needsUpdate = true;
}

function set_transform_texture_object( path, value )
{
	for(var i = 0; i < path.children.length; i++)
	{
		var child = path.children[i];		
		child.material.map = new THREE.Texture(value);
		child.material.map.needsUpdate = true;	
	}	
}

function set_transform_material( path, value )
{
	for(var i = 0; i < path.children.length; i++)
	{
		var child = path.children[i];
		child.material = value;		
	}	
}

function set_object_alpha( path, value )
{	         	
	path.material.opacity = value / 100;	
}

function get_object_alpha( path )
{	         
	return path.material.opacity * 100;	
}

function set_material_alpha( path, value )
{	     	
	path.opacity = value / 100;
}

function set_rotation_x( path, value )
{
	path.x = value * Math.PI/180;	
}

function set_rotation_y( path, value )
{
	path.y = value * Math.PI/180;
}

function set_rotation_z( path, value )
{
	path.z = value * Math.PI/180;
}

function get_rotation_x(path)
{
	return path.x * 180 / Math.PI;	
}

function get_rotation_y(path)
{
	return path.y * 180 / Math.PI;	
}

function get_rotation_z(path)
{
	return path.z * 180 / Math.PI;	
}

function default_interpolate(a, b, t)
{
    return a + (b - a)*t;
}

function get_intersects( x, y, object )
{
	var vector = new THREE.Vector3( ( x / window.innerWidth ) * 2 - 1, - ( y / window.innerHeight ) * 2 + 1, 0.5 );
	projector = new THREE.Projector();
	projector.unprojectVector( vector, active_camera );
	var ray = new THREE.Ray( active_camera.position, vector.subSelf( active_camera.position ).normalize() );
	var intersects = ray.intersectObject( object );	
	return intersects;
}

function generate_model_loader( type )
{
	if(type == RESOURCE_JSON_MODEL)
		return new THREE.JSONLoader();
	return new THREE.BinaryLoader();
}

var program_fill = function ( context ) 
{
	context.beginPath();
	context.arc( 0, 0, 1, 0, Math.PI * 2, true );
	context.closePath();
	context.fill();
}

var program_stroke = function ( context ) 
{
	context.beginPath();
	context.arc( 0, 0, 1, 0, Math.PI * 2, true );
	context.closePath();
	context.stroke();
}

function vector2( x, y )
{	
	return new THREE.Vector2( x, y );
}

function vector3( x, y, z )
{	
	return new THREE.Vector3( x, y, z );
}

function color( hex )
{	
	return new THREE.Color( hex );
}

var flat_shading = THREE.FlatShading;
var smooth_shading = THREE.SmoothShading;
var refraction_mapping = new THREE.CubeRefractionMapping();
var additive_blending = THREE.AdditiveBlending;