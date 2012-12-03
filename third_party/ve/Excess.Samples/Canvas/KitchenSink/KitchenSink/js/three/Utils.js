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

threejs.Utils = Class.extend(
{
	init: function()
	{					
	},	
	color: function( hex )
	{	
		return new THREE.Color( hex );
	},
	vector2: function( x, y )
	{	
		return new THREE.Vector2( x, y );
	},
	vector3: function( x, y, z )
	{	
		return new THREE.Vector3( x, y, z );
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
	if(camera)
	{
		camera.manager.active_camera = camera;
		if(camera.manager.active_composer)
			camera.manager.active_composer.render();
		else
			camera.manager.renderer.render(camera.manager.scene, camera);
	}		
}

function set_active_composer( composer )
{	
	if(composer)
	{
		composer.manager.active_composer = composer;
		if(composer.manager.active_composer)
			composer.manager.active_composer.render();
		else
			composer.manager.renderer.render(composer.manager.scene, composer.manager.active_camera);
	}
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
		console.error("Unknow resource: " + resource);
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
		console.error("Unknow resource: " + resource);
	path.material.map = new THREE.Texture(resource.data);
	path.material.map.needsUpdate = true;
}

function set_webgl_object_texture( path, value )
{
	var resource = path.manager.streamer.get_resource(value);
	if(!resource)
		console.error("Unknow resource: " + resource);
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
	projector.unprojectVector( vector, manager.active_camera );
	var ray = new THREE.Ray( manager.active_camera.position, vector.subSelf( manager.active_camera.position ).normalize() );
	var intersects = ray.intersectObject( object );	
	return intersects;
}

function setMorphTargetInfluences( path, index, value)
{
	path.morphTargetInfluences[ index ] = value;
}

function createModelSrc( path, src, manager  )
{	
	manager.scene.remove(path);
	var res = manager.streamer.get_resource(src).data;
	var obj = clone3jsObject( res, manager )
	obj.manager = manager;
	manager.scene.add(obj);
	return obj;
}

function clone3jsObject( obj, manager )
{
	var clone = new THREE.Object3D();	
	
	for(var name in obj)
	{
		if(obj[name] instanceof THREE.PerspectiveCamera)
		{
			clone[name] = new THREE.PerspectiveCamera(obj[name].fov, obj[name].aspect, obj[name].near, obj[name].far, obj[name].target);
			clone[name].manager = manager;
			clone[name].position = obj[name].position.clone();
			clone[name].scale = obj[name].scale.clone();
			clone[name].rotation = obj[name].rotation.clone();
			clone[name].lookAt(manager.scene.position);
			clone[name].manager.scene.add(clone[name]);
		}
		else if(obj[name] instanceof THREE.Vector3)
			clone[name] = obj[name].clone();		
		else
			clone[name] = obj[name];			
	}
	
	return clone;
}

function generate_model_loader( type )
{
	if(type == RESOURCE_JSON_MODEL)
		return new THREE.JSONLoader();
	return new THREE.ColladaLoader();
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

function texture_shader_setter(path, value, complete_path, set_value)
{	
	var resource = path.manager.streamer.get_resource(value);
	if(!resource)
		console.error("Unknow resource: " + resource);
	complete_path.texture = THREE.ImageUtils.loadTexture( resource.asset );
	complete_path.texture.wrapS = complete_path.texture.wrapT = THREE.Repeat;	
}

var flat_shading = THREE.FlatShading;
var smooth_shading = THREE.SmoothShading;
var refraction_mapping = new THREE.CubeRefractionMapping();
var additive_blending = THREE.AdditiveBlending;