function set_active_camera( camera )
{
	renderer.render(scene, camera);
	soundRenderer.render(scene, camera);
	active_camera = camera;
}

function threejs_load_resources(callback)
{
	var resources = [];
	for(var i = 0; i < streamer.resources.length; i++)
	{
		var res = streamer.resources[i];
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

		streamer.request(resources, stream_client);
	}
	else callback();
}

function set_transform_image( path, value )
{
	for(var i = 0; i < path.children.length; i++)
	{
		var child = path.children[i];		
		child.materials[ 0 ].map = new THREE.Texture(streamer.get_resource(value).data);	
	}	
}

function set_transform_material( path, value )
{
	for(var i = 0; i < path.children.length; i++)
	{
		var child = path.children[i];
		child.materials[ 0 ] = value;		
	}	
}

function set_object_alpha( path, value )
{	         
	path.materials[0].opacity = value / 100;	
}

function get_object_alpha( path )
{	         
	return path.materials[0].opacity * 100;	
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

var flat_shading = THREE.FlatShading;
var smooth_shading = THREE.SmoothShading;
var refraction_mapping = new THREE.CubeRefractionMapping();
var additive_blending = THREE.AdditiveBlending;